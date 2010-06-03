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

if (-f ../scripts/env-csh) then
   set SrcDir = $cwd
else if (-f ../../scripts/env-csh) then
   set SrcDir = $cwd:h
endif
set ldir = $SrcDir:h/scripts
set path = ( $ldir $path )
source $ldir/env-csh

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    Separator $Log
    NoteD $Log "   C Environment Configuration - scdecls.h"

    set STDOUT = $IncDir/scdecls.h

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    Note $STDOUT "/*"
    Note $STDOUT " * SCDECLS.H - define and declares things missing from the system headers"
    Note $STDOUT " *"
    Note $STDOUT " */"
    Note $STDOUT ""

    flog $Log pushd $Base/score

    set opt = ""
    set opt = ( $opt $Cfe_CC_Flags )
    set opt = ( $opt $Cfe_LD_Flags $Cfe_LD_RPath )

    flog $Log mkdir $CfgDir
    flog $Log cd $CfgDir
    flog $Log $Cfe_CC_Exe -DMM_CONFIG -I.. -I$IncDir ../scmemi.c -o score-config $opt
    if ($status == 0) then
       flog $Log ( $CFE ./score-config >>& $STDOUT )
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

