#!/bin/csh -f
#
# DO-BATCH.SLURM - run a batch job on a site using MOAB and SLURM
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

unalias *

set HostOS  = `uname -s`
set HostHW  = `uname -m`

set Host     = none
set Prob     = unknown
set Notify   = "n"
set Date     = `date`
set TTY      = ""
set Quiet    = FALSE
set Info     = ""

# msub controls
set TimeMax  = 10

# dual srun/msub controls
set NThreads = 1
set NNodes   = 1

alias Note 'echo \!\!:2-$ >>& \!\!:^'

set Cmnd   = ""
set EnvVar = ""
while ($#argv > 0)
   switch ($1)
      case -dt:
           shift
           set TimeMax = $1
           breaksw
      case -e:
           shift
           set EnvVar = ( $EnvVar $1 )
           set EnvVar = ( $EnvVar $2 )
           shift
           breaksw
      case -f:
           shift
           set Info = $1
           breaksw
      case -h:
           echo ""
           echo "Usage: do-batch [-dt #] [-e <var> <value>] [-f <file>] [-h]"
           echo "                [-H <host>] [-m] [-o <file>] [-p #]"
           echo "                [-q] [-t #] [-w]"
           echo ""
           echo "Options:"
           echo "     dt - time limit in seconds"
           echo "     e  - add environment variables to the batch script"
           echo "     f  - file containing batch control specifications"
           echo "     h  - this help message"
           echo "     H  - host on which to run"
           echo "     m  - notify by mail"
           echo "     o  - set the name of the output file"
           echo "     p  - number of processes to use"
           echo "     q  - suppress do-batch messages in the logs"
           echo "     t  - number of threads"
           echo "     w  - notify via the write utiliy"
           echo ""
           exit(1)
           breaksw
      case -H:
           shift
           set Host = $1
           breaksw
      case -m:
           set Notify = "m"
           breaksw
      case -o:
           shift
           set TTY = $1
           breaksw
      case -p:
           shift
           set NNodes = $1
           breaksw
      case -q:
           set Quiet = TRUE
           breaksw
      case -t:
           shift
           set NThreads = $1
           breaksw
      case -w:
           set Notify = "w"
           breaksw
      default:
           set Prob = $1
           set Prob = $Prob:t
           while ($#argv > 0)
              set Cmnd = ( $Cmnd $1 )
              shift
           end

           break
           breaksw
   endsw
   shift
end

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# PROCESS_INFO_FILE

# Batch info file may contain any of the following:
#   host = <host>
#   time = <time>
# Note: time is in seconds

if ($Info != "") then
   set Batch    = `awk '$1 == "batch"  { print $3 }' $Info`
   set tHost    = `awk '$1 == "host"   { print $3 }' $Info`
   set tTimeMax = `awk '$1 == "time"   { print $3 }' $Info`
   set HaveMPI  = `awk '$1 == "mpi"    { print $3 }' $Info`
   set EnvVar   = `awk '$1 == "setenv" { printf("%s %s\n", $2, $3); }' $Info`

   if ("$tHost" != "") then
      set Host = $tHost
   endif
   if ("$tTimeMax" != "") then
      set TimeMax = $tTimeMax
   endif

# Note: count on the Info file having been automatically generated
# in a script that may never regain control to remove it
   rm -f $Info

endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# WRITE_SCRIPT

set Script = $cwd/b.$Prob.$$
set Log    = $Script.err
if ("$TTY" == "") then
   set TTY = $Script.out
endif

rm -f $Script
touch $Script

Note $Script '#\!/bin/csh'
if ("$Host" != "none") then
   Note $Script '# MSUB -c' "$Host"
endif

Note $Script '# MSUB -o' $Log
if (($NThreads > 1) || ($NNodes > 1)) then
   Note $Script '# MSUB -l nodes='$NNodes
#   Note $Script '# MSUB -l nodes='$NNodes":ppn="$NThreads
endif

# convert time to seconds
set TimeMax = `timer -t $TimeMax`
Note $Script '# MSUB -l walltime='$TimeMax
Note $Script ''

Note $Script 'cd' $cwd
Note $Script ''

Note $Script 'set TTY = "'$TTY'"'
Note $Script 'rm -f $TTY'
Note $Script 'touch $TTY'
Note $Script ''

if ($Quiet == FALSE) then
   Note $Script 'echo "Batch Job: '$Cmnd'" >> $TTY'
   Note $Script 'echo "MPI Job  : no" >> $TTY'
   Note $Script 'echo "Date     : '$Date'" >> $TTY'
   Note $Script 'echo "----------------------------------------------------" >> $TTY'
   Note $Script ''
endif

# add specified environment variables
while ($#EnvVar > 1)
   set Var = $EnvVar[1]
   set Val = $EnvVar[2]
   shift EnvVar
   shift EnvVar
   Note $Script "setenv $Var $Val" '>>& $TTY'
end
Note $Script ''

Note $Script "setenv OMP_NUM_THREADS $NThreads"
Note $Script ''

Note $Script "do-run -p $NNodes $Cmnd" '>>& $TTY'
Note $Script 'set Err = $status'
Note $Script ''

if ($Quiet == FALSE) then
   Note $Script 'echo "----------------------------------------------------" >> $TTY'
   Note $Script ''
   Note $Script 'if ($Err == 0) then'
   Note $Script '   set ErrMsg = "'$Prob' succeeded"'
   Note $Script 'else'
   Note $Script '   set ErrMsg = "'$Prob' failed ($Err)"'
   Note $Script 'endif'
   Note $Script 'echo "$ErrMsg" >> $TTY'
   Note $Script ''
endif

switch ($Notify)
   case m:
        Note $Script '(echo "BATCH job for '$Prob' has finished in '$cwd'" ; \'
        Note $Script ' echo "$ErrMsg" ) | \'
        Note $Script 'mail "'$USER'"'
        breaksw
   case w:
        Note $Script '(echo "BATCH job for '$Prob' has finished in '$cwd'" ; \'
        Note $Script ' echo "$ErrMsg" ) | \'
        Note $Script 'write "'$USER'"'
        breaksw
        breaksw
endsw

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# EXECUTE

chmod 750 $Script
msub $Script

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit($status)
