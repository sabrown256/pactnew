#!/bin/csh -f
#
# Run a single test for CTest
#
set PSY_Base = ""
set PSY_Root = ""
set Test     = ""
set opt      = ""

while ($#argv > 0)
   switch ($1)
      case -base:
           shift
           set PSY_Base = $1
	   breaksw
      case -root:
           shift
           set PSY_Root = $1
	   breaksw
      case -*:
           set opt = ( $opt $1 )
           breaksw
      default:
           if ($Test != "") then
             echo "Only one test may be run"
             exit(1)
           endif
           set Test = $1
           breaksw
   endsw
   shift
end

if ($PSY_Base == "") then
   echo "-PSY_Base must be specified"
   exit(1)
endif
setenv PSY_Base $PSY_Base
setenv TestScrDir $PSY_Base/scripts

setenv PSY_Root $PSY_Root
setenv TestDir  $PSY_Root/psh

# debug optmize mixed
setenv PSY_Dbg_Opt debug

#setenv PSY_TmpDir $PSY_Root/psh  # location of log

# setenv TestDir   $SrcDir/$PSY_TmpDir/test
# log is created at $TestDir/log.$TLog.$LogSfx

# from psh/test/common
setenv Package  psh
setenv Test     $Test
source $PSY_Base/scripts/test-env
#/bin/csh -xv $PSY_Base/psh/tests/$Test $opt
$PSY_Base/psh/tests/$Test $opt
