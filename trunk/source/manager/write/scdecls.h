#!/bin/csh -f
#
# SCDECLS.H - write the SCDECLS.H file
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

unalias *

# put these in shell variables since
# prune-env will remove them as environment variables
set Log    = $1
set ScrDir = $2

eval `$ScrDir/prune-env pact`
set path = ( $ScrDir $path )
source $ScrDir/env-csh

dbget BaseDir
dbget CFE
dbget Cfe_CC_Exe
dbget Cfe_CC_Flags
dbget Cfe_LD_Flags
dbget Cfe_LD_RPath
dbget CfgDir
dbget IncDir

Separator $Log
Note $Log "   BaseDir      = $BaseDir"
Note $Log "   CFE          = $CFE"
Note $Log "   Cfe_CC_Exe   = $Cfe_CC_Exe"
Note $Log "   Cfe_CC_Flags = $Cfe_CC_Flags"
Note $Log "   Cfe_LD_Flags = $Cfe_LD_Flags"
Note $Log "   Cfe_LD_RPath = $Cfe_LD_RPath"
Note $Log "   CfgDir       = $CfgDir"
Note $Log "   IncDir       = $IncDir"

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    NoteD $Log "   C Environment Configuration - scdecls.h"

    set STDOUT = $IncDir/scdecls.h

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    Note $STDOUT "/*"
    Note $STDOUT " * SCDECLS.H - define and declares things missing from the system headers"
    Note $STDOUT " *"
    Note $STDOUT " */"
    Note $STDOUT ""

    flog $Log pushd $BaseDir/score

    set opt = ""
    set opt = ( $opt $Cfe_CC_Flags )
    set opt = ( $opt $Cfe_LD_Flags $Cfe_LD_RPath )
    set opt = ( `echo $opt | sed 's|\"||g'` )

    flog $Log mkdir $CfgDir
    flog $Log cd $CfgDir
    flog $Log $Cfe_CC_Exe -DMM_CONFIG -I.. -I$IncDir ../scmemi.c -o score-config $opt
    if ($status == 0) then

# NOTE: coded this way to throw away message to stderr
# which may come from CFE instead of score-config
       flog $Log ( ( $CFE ./score-config >> $STDOUT ) >& /dev/null )
       if ($status == 0) then
          flog $Log $RM ./score-config
          Note $Log "score-config execution succeeded"
       else
          Note $Log "score-config execution failed"
       endif

# if score-config fails choose largest possible value for N_DOUBLES_MD
# so the build can proceed successfully (if non-optimally)
    else
       NoteD $Log "Failed to correctly configure the SCORE Memory Manager"
       Note $STDOUT "#define N_DOUBLES_MD 8"
    endif
    flog $Log cd ..
    flog $Log $RMDir $CfgDir

    flog $Log popd
    Note $STDOUT ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

