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

set tag = MSUB
set sub = msub

set hst    = none
set prob   = unknown
set ntfy   = "n"
set datest = `date`
set TTY    = ""
set pool   = ""
set quiet  = FALSE
set info   = ""
set tdir   = $cwd
set tgt    = batch

# $sub controls
set tmx  = 10

# dual controls
set NThreads = 1
set NNodes   = 1

alias Note 'echo \!\!:2-$ >>& \!\!:^'

set cmdln = ( $0 $argv )
set cmnd  = ""
set evar  = ""

while ($#argv > 0)
   switch ($1)
      case -a:
           shift
           set cmnd = ( $cmnd $1 )
           breaksw
      case -d:
           shift
           set tdir = $1
           breaksw
      case -dt:
           shift
           set tmx = `timer -t $1`
           breaksw
      case -e:
           shift
           set evar = ( $evar $1 )
           set evar = ( $evar $2 )
           shift
           breaksw
      case -f:
           shift
           set info = $1
           breaksw
      case -g:
           set tgt = group
           breaksw
      case -h:
           echo ""
           echo "Usage: do-batch [-a <args>] [-d <dir>] [-dt #] [-e <var> <value>]"
           echo "                [-f <file>] [-g] [-h] [-H <host>] [-m]"
           echo "                [-o <file>] [-p #] [-prt <queue>] [-q] [-t #] [-w]"
           echo ""
           echo "Options:"
           echo "     a   - quoted string with arguments for the command"
           echo "     d   - temporary file directory"
           echo "     dt  - time limit in S or M:S or H:M:S form"
           echo "     e   - add environment variables to the batch script"
           echo "     f   - file containing batch control specifications"
           echo "     g   - run group of jobs in preallocated pool of nodes"
           echo "     h   - this help message"
           echo "     H   - host on which to run"
           echo "     m   - notify by mail"
           echo "     o   - set the name of the output file"
           echo "     p   - number of processes to use"
           echo "     prt - batch pool/queue/partition to use"
           echo "     q   - suppress do-batch messages in the logs"
           echo "     t   - number of threads"
           echo "     w   - notify via the write utiliy"
           echo ""
           exit(1)
           breaksw
      case -H:
           shift
           set hst = $1
           breaksw
      case -m:
           set ntfy = "m"
           breaksw
      case -o:
           shift
           set TTY = $1
           breaksw
      case -p:
           shift
           set NNodes = $1
           breaksw
      case -prt:
           shift
           set pool = $1
           breaksw
      case -q:
           set quiet = TRUE
           breaksw
      case -t:
           shift
           set NThreads = $1
           breaksw
      case -w:
           set ntfy = "w"
           breaksw
      default:
           set prob = $1

           shift
           set cmnd = ( $prob $cmnd )
           while ($#argv > 0)
              set cmnd = ( $cmnd $1 )
              shift
           end

           set prob = $prob:t
           break
           breaksw
   endsw
   shift
end

switch ($tgt)

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# GROUP - srun a bunch of jobs in a pool already salloc'd

case group:
   set root = $cwd
   set tdir = `dirname $0`
   set scr  = $0:t
   set prob = ""

   set exe = $prob[1]

   cd $root >& /dev/null

# sleep until there are fewer jobs running than nodes
   pushd $tdir >& /dev/null
   @ nn = $SLURM_JOB_NUM_NODES
   @ nr = $nn + 1
   while ($nr > $nn)
      touch running.group
      @ nr = `ls -1 running.* | wc -l`
      @ nr = $nr - 1
      rm -f running.group
      if ($nr > $nn) then
         sleep 10
      endif
   end
   popd >& /dev/null

   set TTY = "$tdir/b.$scr.$$.out"
   rm -f $TTY
   touch $TTY

   echo "Job      : $exe $prob" >> $TTY
   echo "MPI Job  : no" >> $TTY
   echo "Date     : $datest" >> $TTY
   echo "----------------------------------------------------" >> $TTY

   setenv OMP_NUM_THREADS $NThreads

   if ("$pool" == "") then
      mpi-io-run srun -i all -I -c 1 -n 1 $exe $prob >>& $TTY &
   else
      mpi-io-run srun -i all -I -c 1 -n 1 -p $pool $exe $prob >>& $TTY &
   endif

   exit($err)

   breaksw

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# PROCESS_INFO_FILE

case batch:

# Batch info file may contain any of the following:
#   host = <host>
#   time = <time>
# Note: time is in seconds

   if ($info != "") then
      set Batch   = `awk '$1 == "batch"  { print $3 }' $info`
      set lhst    = `awk '$1 == "host"   { print $3 }' $info`
      set ltmx    = `awk '$1 == "time"   { print $3 }' $info`
      set HaveMPI = `awk '$1 == "mpi"    { print $3 }' $info`
      set evar    = `awk '$1 == "setenv" { printf("%s %s\n", $2, $3); }' $info`

      if ("$lhst" != "") then
         set hst = $lhst
      endif
      if ("$ltmx" != "") then
         set tmx = `timer -t $ltmx`
      endif
      unset ltmx
      unset lhst

# Note: count on the info file having been automatically generated
# in a script that may never regain control to remove it
      rm -f $info

   endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# WRITE_SCRIPT

   set scr = $tdir/b.$prob.$$
   set log = $scr.err
   if ("$TTY" == "") then
      set TTY = $scr.out
   endif

   rm -f $scr
   touch $scr

   Note $scr '#\!/bin/csh'
   Note $scr "#"
   Note $scr "# Script generated by do-batch from"
   Note $scr "# command line: $cmdln"
   Note $scr "#"
   Note $scr ""

   Note $scr "# $tag -o $log"
   if ("$pool" != "") then
      Note $scr "# $tag -q $pool"
   endif

# setup resources
   set res = ""

   if (($NThreads > 1) || ($NNodes > 1)) then
      set res = "nodes=$NNodes"
      set res = "${res}:ppn=$NThreads"
   endif

# convert time to seconds
   set tmx = `timer -t $tmx`
   set res = "${res}:walltime=$tmx"

# add the resource request
   Note $scr "# $tag -l $res"
   Note $scr ''

   Note $scr 'unalias *'
   Note $scr ''

   Note $scr "cd $tdir >& /dev/null"
   Note $scr ''

   Note $scr 'set TTY = "'$TTY'"'
   Note $scr 'rm -f $TTY'
   Note $scr 'touch $TTY'
   Note $scr ''

   if ($quiet == FALSE) then
      Note $scr 'echo "Batch Job: '$cmnd'" >> $TTY'
      Note $scr 'echo "MPI Job  : no" >> $TTY'
      Note $scr 'echo "Date     : '$datest'" >> $TTY'
      Note $scr 'echo "----------------------------------------------------" >> $TTY'
      Note $scr ''
   endif

# add specified environment variables
   while ($#evar > 1)
      set Var = $evar[1]
      set Val = $evar[2]
      shift evar
      shift evar
      Note $scr "setenv $Var $Val" '>>& $TTY'
   end
   Note $scr ''

   Note $scr "setenv OMP_NUM_THREADS $NThreads"
   Note $scr ''

   Note $scr "do-run -p $NNodes $cmnd" '>>& $TTY'
   Note $scr 'set Err = $status'
   Note $scr ''

   if ($quiet == FALSE) then
      Note $scr 'echo "----------------------------------------------------" >> $TTY'
      Note $scr ''
      Note $scr 'if ($Err == 0) then'
      Note $scr '   set ErrMsg = "'$prob' succeeded"'
      Note $scr 'else'
      Note $scr '   set ErrMsg = "'$prob' failed ($Err)"'
      Note $scr 'endif'
      Note $scr 'echo "$ErrMsg" >> $TTY'
      Note $scr ''
   endif

   switch ($ntfy)
      case m:
           Note $scr '(echo "BATCH job for '$prob' has finished in '$cwd'" ; \'
           Note $scr ' echo "$ErrMsg" ) | \'
           Note $scr 'mail "'$USER'"'
           breaksw
      case w:
           Note $scr '(echo "BATCH job for '$prob' has finished in '$cwd'" ; \'
           Note $scr ' echo "$ErrMsg" ) | \'
           Note $scr 'write "'$USER'"'
           breaksw
           breaksw
   endsw

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# EXECUTE

   chmod 750 $scr
   ( $sub $scr ) |& cat
   breaksw

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

endsw

exit($status)
