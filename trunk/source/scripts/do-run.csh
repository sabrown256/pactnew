#!/bin/csh -f
#
# DO-RUN - shell to drive an executable parallel program
#        - this includes debugging and batch support
#        - NOTE: a design goal in this script is to support
#        -       MPI in such a way that if you run a code that
#        -       has MPI linked in but never called (usually an option)
#        -       in a session, making it essentially a non-parallel run,
#        -       then the MPI frontends and other restrictive features
#        -       of distributed parallel code runs may be avoided.
#        -       This does mean that a "parallel" run with one process
#        -       cannot be run with this script.
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

onintr INTERRUPT

set HostOS = `uname -s`
set Host   = `uname -n`

pushd `dirname $0` >& /dev/null
set BinDir = $cwd
popd >& /dev/null

unalias cd
unalias rm

set Which = "$BinDir/swhich -1"

setenv PACT $BinDir:h

if ((-x /usr/bin/bpsh) && ($?NODES == 1)) then
   set HaveBPSH = TRUE
   setenv MP_PROCS     0
else
   set HaveBPSH = FALSE
   setenv MP_PROCS     1
endif

setenv MP_LABELIO   no
setenv MP_RMPOOL    pdebug
setenv MP_NODES     1
setenv MP_INFOLEVEL 0

set MPIExec   = none
set MPINot    = FALSE
set Partition = ( -p pdebug )

if (!($?RM)) then
   setenv RM "rm -f"
endif

set Code = ""
if (!($?Wrap)) then
   if (-x $BinDir/mpi-io-wrap) then
      setenv Wrap $BinDir/mpi-io-wrap
   else
      setenv Wrap "`$Which mpi-io-wrap`"
      if (!(-x "$Wrap")) then
         setenv Wrap ""
      endif
   endif
endif

if (!($?TVDirs)) then
   setenv TVDirs "$cwd/.tv.$$"
   $RM $TVDirs
   touch $TVDirs
endif

set Debug   = FALSE
set Prob    = ""
set NThread = 1
set NCPU    = 1
set Batch   = FALSE
set DryRun  = FALSE
set Err     = 100
set Force   = FALSE
set UseVG   = FALSE
set UseVGDB = FALSE
set UseVGDS = FALSE
set Verbose = FALSE
set UseZF   = FALSE
set DArgs   = ""
set BArgs   = ""
set DBGArgs = ""

while ($#argv > 0)
   switch ($1)
      case -b:
           set Batch = TRUE
           breaksw

      case -bf:
           shift
           set BArgs = ( $BArgs -f $1 )
           set Batch = TRUE
           breaksw

      case -d:
           set Debug = TRUE
           set DBG   = `$Which totalview`
           breaksw

      case -dr:
           set DryRun  = TRUE
           set Verbose = TRUE
           breaksw

      case -f:
           set Force = TRUE
           breaksw

      case -g:
           shift
           set Debug   = TRUE
           set DBG     = ""
           set DBGArgs = ""
           set List    = ( $1 )
           while ("$List" != "")
              set BCdr = `expr "$List" : '\(.*\) .*'`
              set BCar = `expr "$List" : '.* \(.*\)'`
              if (("$BCar" == "") && ("$BCdr" == "")) then
		 set DBG     = `$Which $List`
                 set List    = ""
              else
                 set DBGArgs = ( $BCar $DBGArgs )
                 set List    = ( $BCdr )
              endif
           end
           breaksw

      case -h:
      case help:
           echo ""
           echo "Usage: do-run [-b] [-bf <file>] [-d] [-dr] [-f] [-h] [-m] [-n #]"
           echo "              [-o <file>] [-p #] [-part <value>]"
           echo "              [-q] [-t #] [-v] [-vg] [-vgd] [-x] [-z] args"
           echo ""
           echo "       b    - run in batch"
           echo "       bf   - file containing info for local batch system"
           echo "       d    - run the code under the debugger"
           echo "       dr   - report but do not execute command"
           echo "       f    - force a distributed run even with 1 MPI process"
           echo "       h    - this help message"
           echo "       m    - suppress MPI I/O bug fix (diagnostic)"
           echo "       n    - number of nodes to use - default is 1 (MPI)"
           echo "       o    - output file for DP runs"
           echo "       p    - number of processes to use (MPI)"
           echo "       part - the name of the partition in which to run"
           echo "       q    - suppress do-batch messages in logs"
           echo "       t    - number of threads to use (SMP)"
           echo "       v    - verbose mode"
           echo "       vg   - run under Valgrind"
           echo "       vgd  - run with interactive GDB Valgrind"
           echo "       x    - suppress MPI frontend (sometimes useful in batch)"
           echo "       z    - run under ZeroFault on AIX only"
           echo ""
           exit(1)
           breaksw

      case -m:
           setenv Wrap ""
           breaksw

      case -o:
           shift
           set DArgs = ( $DArgs -o $1 )
           breaksw

      case -n:
           shift
           setenv MP_NODES $1
           set DArgs = ( $DArgs -n $MP_NODES )
           breaksw

      case -p:
           shift
	   if (`expr "$1" : '.*,.*'` > 0) then
              setenv MP_PROCS  `expr "$1" : '\(.*\),.*'`
              setenv MP_NODES  `expr "$1" : '.*,\(.*\)'`
           else
              setenv MP_PROCS $1
           endif
           set DArgs = ( $DArgs -p $MP_PROCS )
           breaksw

      case -part:
           set Partition = ( -p $2 )
           setenv MP_RMPOOL $2
           shift
           breaksw

      case -q:
           set DArgs = ( $DArgs $1 )
           breaksw

      case -t:
           shift
           @ NThread = $1
           if ($NThread > 0) then
              setenv OMP_NUM_THREADS $NThread
           endif
           breaksw

      case -v:
           set Verbose = TRUE
           breaksw

      case -vg:
           set UseVG = TRUE
           breaksw

      case -vgd:
           set UseVG   = TRUE
           set UseVGDB = TRUE
           breaksw

      case -vgs:
           set UseVG   = TRUE
           set UseVGDS = TRUE
           breaksw

      case -x:
           setenv HaveMPI FALSE
           breaksw

      case -z:
           set UseZF = TRUE
           breaksw

      default:
           set Code = $1
           shift
           set Prob = "$argv"
           break
           breaksw
   endsw
   shift
end

set NCPU = $MP_PROCS
set Code = `$Which $Code`
if (!(-x "$Code") && ($DryRun == FALSE)) then
   echo "No '"$Code"' on your path"
   exit(1)
endif

set RM  = ( /bin/rm -f )
set Tmp = ./.tmp-$$

if ($HostOS != AIX) then
   set UseZF = FALSE
endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# figure out whether we have an MPI code

file $Code |& grep script >& /dev/null
if ($status == 0) then
   setenv HaveMPI FALSE
endif

if (!($?HaveMPI)) then

# this is not because AIX lacks nm but because it is pretty
# slow and they can have large binaries
# NOTE: do case insensitive check to pick up Fortran codes
   if ($HostOS == AIX) then
#      set havempi = `strings -a $Code |& grep -i mpi_init`
      set havempi = `dump -X32_64 -H $Code |& egrep -e 'libmpi_*r*.a'`
   else
      set havempi = `nm $Code |& grep -i mpi_init`
   endif

   if ("$havempi" == "") then
      setenv HaveMPI FALSE
   else
      setenv HaveMPI TRUE
   endif
endif

if ($HaveMPI == FALSE) then
   setenv Wrap ""
endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# setup args for MPI front end

if ($HaveMPI == TRUE) then

   set SRUN    = `$Which srun`
   set PRUN    = `$Which prun`
   set DMPIRUN = `$Which dmpirun`
   set MPIRUN  = `$Which mpirun`
   set RUN     = `$Which run`

   if ($HostOS == AIX) then
      set MPIArgs = $Code
      set MPIExec = poe
      if (($MP_PROCS < 2) && ($Batch == FALSE)) then
         set MPINot = TRUE

         setenv MP_NODES          1
         setenv MP_EUILIB         ip
         setenv MP_RESD           NO
         setenv MP_CPU_USE        multiple
         setenv MP_TASK_AFFINITY  0
         setenv MP_HOSTFILE       .dp-aix.$Host.$$

         unsetenv MP_EUIDEVICE
         unsetenv MP_TASKS_PER_NODE
         unsetenv MP_RMPOOL
         unsetenv LOADLBATCH

# NOTE: completely control the MPI environment
# an MPI code might be running this do-run session
# as a child process - it has happened
         unsetenv MP_CHILD
         unsetenv MP_SHARED_MEMORY
         unsetenv MP_SYNC_ON_CONNECT
         unsetenv MP_COREFILE_FORMAT
         unsetenv MP_PGMMODEL
         unsetenv MP_ISATTY_STDIN
         unsetenv MP_ISATTY_STDOUT
         unsetenv MP_ISATTY_STDERR
         unsetenv MP_PARTITION
         unsetenv MP_PIPE_IN
         unsetenv MP_PIPE_OUT
         unsetenv MP_CHILD_INET_ADDR
         unsetenv MP_MPI_NETWORK
         unsetenv MP_STDIN_DESC
         unsetenv MP_STDOUT_DESC
         unsetenv MP_STDERR_DESC
         unsetenv MP_COMMON_TASKS
         unsetenv MP_RMLIB
         unsetenv MP_PRIORITY

         echo $Host >! $MP_HOSTFILE
      endif

   else if (-x "$SRUN") then
      if (($MP_PROCS > 1) || ($Force == TRUE)) then
         if ($NThread == 0) then
            set TArg = ( -O )
         else
            set TArg = ( -c $NThread )
         endif
         if (($MP_NODES > 1) || ($Force == TRUE)) then
            set MPIArgs = ( -i all -I $TArg -n $NCPU -N $MP_NODES -p $Partition[2] $Code )
         else
            set MPIArgs = ( -i all -I $TArg -n $NCPU -p $Partition[2] $Code )
         endif
         set MPIExec = $SRUN

# with 1 or fewer processors do NOT use srun
      else
         set MPIArgs = ""
# NOTE: for debug MPIExec MUST be Code, the -a goes between
         set MPIExec = $Code
      endif

   else if (-x "$PRUN") then
#      setenv RMS_PARTITION $Partition[2]
      if (($MP_PROCS > 1) || ($Force == TRUE)) then
         if ($NThread == 0) then
            set TArg = ( -O )
         else
            set TArg = ( -c $NThread )
         endif
         if (($MP_NODES > 1) || ($Force == TRUE)) then
            set MPIArgs = ( -i all -I $TArg -n $NCPU -N $MP_NODES $Code )
         else
            set MPIArgs = ( -i all -I $TArg -n $NCPU $Code )
         endif
         set MPIExec = $PRUN

# with 1 or fewer processors do NOT use prun unless forced to
      else
         set MPIArgs = ""
# NOTE: for debug MPIExec MUST be Code, the -a goes between
         set MPIExec = $Code
      endif

   else if (-x "$DMPIRUN") then
      set MPIArgs = ( -np $NCPU $Code )
      set MPIExec = $DMPIRUN

   else if (-x "$MPIRUN") then
      if (-x /usr/bin/bpsh) then
         unsetenv OMP_NUM_THREADS   # work around until lampi 1.5.16 bug fixed
      endif
      if ($HaveBPSH == TRUE) then
         if ($MP_PROCS > 0) then
            set MPIArgs = ( -q -np $NCPU $Code )
            set MPIExec = $MPIRUN
         else if ($Force == TRUE) then
            set lNODES = ( `echo $NODES | sed 's/,/ /g'` )
            set MPIArgs = ( $lNODES[1] $Code )
            set MPIExec = /usr/bin/bpsh
         else if ($Debug == TRUE) then
            set MPIArgs = ""
            set MPIExec = ( $Code )
         else
            set MPIArgs = ( $Code )
            set MPIExec = ""
         endif

      else
         if (($MP_PROCS > 1) || ($Force == TRUE)) then

# check for use of PBS (Portable Batch System)
            if ($?PBS_NODEFILE == 1) then
               set MPIArgs = ( -machinefile $PBS_NODEFILE -np $NCPU $Code )
            else
               set MPIArgs = ( -q -np $NCPU $Code )
            endif
            set MPIExec = $MPIRUN
         else
            set MPIArgs = ( $Code )
            set MPIExec = ""
         endif
      endif   

   else if (-x "$RUN") then
      set MPIArgs = ( -rsi $Code )
      set MPIExec = $RUN
   endif

else
   set SRUN    = ""
   set PRUN    = ""
   set DMPIRUN = ""
   set MPIRUN  = ""
   set RUN     = ""
endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# SETUP_DEBUG - work out command options to run the program under
#             - a Totalview type debugger

if ($Debug == TRUE) then

# see if TV does better with this
# NOTE: this is absolutely fatal on Alpha Linux
#   setenv LD_BIND_NOW 1

# find PACT source paths
   set PactRoot = $PACT
   set PactRoot = $PactRoot:h
   set PactRoot = $PactRoot:h
   if (-d $PactRoot/score) then
      foreach i ($PactRoot/*)
         set j = $i:t
         if ((-d $i) && ($j != "CVS") && ($j != "dev")) then
            echo $i >> $TVDirs
         endif
      end
   else if (-d $PactRoot/sources) then
      foreach i ($PactRoot/sources/*)
         set j = $i:t
         if ((-d $i) && ($j != "CVS") && ($j != "dev")) then
            echo $i >> $TVDirs
         endif
      end
   endif

# figure out the back slash treatment
   set BackSlash = "\\"
   $RM $Tmp
   echo $BackSlash > $Tmp
   if (`cat $Tmp` == "$BackSlash") then
      set BackSlash = "\"
   endif
   $RM $Tmp

# figure which debugger to use
   if (!(-x "$DBG")) then
      set DBG = `$Which tv5`
      if (!(-x "$DBG")) then
         set DBG = `$Which totalview`
         if (!(-x "$DBG")) then
            echo "No TOTALVIEW on your path"
            exit(1)
         endif
      endif
   endif

   set DBG = ( $DBG $DBGArgs )

# set up the debugger to know about the sources
   set XDef = Xdef.tmp
   $RM $XDef
   touch $XDef
   echo "totalview*geometry: 640x960+0+30" >>& $XDef
   echo "totalview*searchPath: .,$BackSlash" >>& $XDef
   awk '{ printf("                      %s,\\\n", $0); }' $TVDirs >>& $XDef
   echo "                      .." >>& $XDef

   xrdb -merge $XDef >& /dev/null
   $RM $XDef $TVDirs

   if ($HaveMPI == TRUE) then
      set Job = ( $DBG $MPIExec -a $MPIArgs )
   else
      set Job = ( $DBG $Code -a )
   endif

   if ($Verbose == TRUE) then
      echo "Running under $DBG"
   endif

   set DArgs = ( $DArgs -l )

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# SETUP_NON_DEBUG - setup the command line for a direct run of the program
#                 - as opposed to under the debugger

else
   $RM $TVDirs
   if ($HaveMPI == TRUE) then
      if ($HostOS == AIX) then
         set Job = ( $MPIArgs )
      else
         set Job = ( $MPIExec $MPIArgs )
      endif
   else
      set Job = ( $Code )
   endif

endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# UseVG - set the command and options to use valgrind
#       - your environment must be properly set already

if ($UseVG == TRUE) then
   if ($UseVGDS == TRUE) then
      set Job = ( valgrind --tool=memcheck --gen-suppressions=all $Job )
   else if ($UseVGDB == TRUE) then
      set Job = ( valgrind --tool=memcheck --db-attach=yes $Job )
   else
      set Job = ( valgrind --tool=memcheck $Job )
   endif
endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# UseZF - set the command and options to use ZeroFault
#       - your environment must be properly set already

if ($UseZF == TRUE) then
   set Job = ( poe zf $Code )
endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# EXECUTE - run whatever it was we decided to do

if ($Batch == FALSE) then
   if ($Verbose == TRUE) then
      if ($HostOS == AIX) then
         echo "setenv MP_LABELIO  $MP_LABELIO"
         echo "setenv MP_NODES    $MP_NODES"
         echo "setenv MP_PROCS    $MP_PROCS"
         if ($MPINot == TRUE) then
            echo "setenv MP_EUILIB   $MP_EUILIB"
            echo "setenv MP_RESD     $MP_RESD"
            echo "setenv MP_CPU_USE  $MP_CPU_USE"
         else
            echo "setenv MP_RMPOOL   $MP_RMPOOL"
         endif
         echo "setenv MP_HOSTFILE $MP_HOSTFILE"
      endif
      echo "$Wrap $Job $Prob"
      if ($DryRun == TRUE) then
         exit(1)
      endif
   endif

# NOTE: mpirun (on the SGI at least) processes its args in such a way
# that a program invocation such as: mpirun foo "a ; b"
# works like "mpirun foo a" ; b
# so that foo does not get b - the shell does
# and the shell typically sees b as an invalid command 
   if (("$MPIExec" == "$MPIRUN") && ($HostOS == IRIX64)) then
      $Wrap $Job '"'$Prob'"'
      set Err = $status
   else

# NOTE: mpirun on some vendors responds to a Verbose environment
# variable and gives lots of diagnostic output that do-run users
# probably do not want
      unsetenv Verbose
      unset Verbose

      $Wrap $Job $Prob
      set Err = $status
   endif

else
   if ($Verbose == TRUE) then
      echo "HaveMPI = |$HaveMPI|"
      echo "Wrap    = |$Wrap|"
      echo "do-batch $BArgs -p $NCPU $Job $Prob"
      if ($DryRun == TRUE) then
         exit(1)
      endif
   endif
   do-batch $BArgs -p $NCPU $Job $Prob
   set Err = $status
endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# EXIT - semi-arbitrarily declare that failure to run the
#      - job by reason of lack of resources (usually CPUs)
#      - causes an exit status value of -1
#      - this is AIXs and OSFs value

if ((("$MPIExec" == "$PRUN") && ($Err == 126)) || \
    (("$MPIExec" == "$SRUN") && ($Err == 1))) then
   set Err = -1
endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit($Err)

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

# INTERRUPT - cleanup properly after an interrupt

INTERRUPT:

   $RM $TVDirs

   exit(1)

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------
