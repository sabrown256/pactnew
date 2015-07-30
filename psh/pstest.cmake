#!/bin/csh -f
#
# Run a single test for CTest
#
set PSY_Base  = ""
set PSY_Build = ""
set PSY_Root  = ""
set Test      = ""
set opt       = ""

while ($#argv > 0)
   switch ($1)
      case -base:
           shift
           set PSY_Base = $1
	   breaksw
      case -build:
           shift
           set PSY_Build = $1
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
   echo "-base must be specified"
   exit(1)
endif
if ($PSY_Build == "") then
   echo "-build must be specified"
   exit(1)
endif
if ($PSY_Root == "") then
   echo "-root must be specified"
   exit(1)
endif
setenv PSY_Base  $PSY_Base
setenv PSY_Build $PSY_Build
setenv PSY_Root  $PSY_Root

setenv PERDB_PATH $PSY_Build/bin
setenv TestDir  $PSY_Build/psh/test
setenv TestScrDir $PSY_Base/scripts

# debug optmize mixed   XXX - 
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
