#!/bin/csh -f
#
# SCDECLS.H - write the SCDECLS.H file
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

set Me = $0
source $Me:h/pre-common

Note $WLog "----- write/package -----"
Note $WLog "Write: package"
Note $WLog ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $WLog

NoteD $WLog "   C Environment Configuration - scdecls.h"
Note $WLog ""

dbmget $WLog Cfe_CC_Exe^      \
             Cfe_CC_Flags^    \
             Cfe_LD_Flags^    \
             Cfe_LD_RPath^    \
             CROSS_FE^        \
             IncDir^          \
             PSY_Base^        \
             PSY_CfgDir^      \
             PACT_CC_VERSION

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    set STDOUT = $IncDir/scdecls.h

    flog $WLog $RM $STDOUT
    flog $WLog touch $STDOUT

    Note $STDOUT "/*"
    Note $STDOUT " * SCDECLS.H - define and declares things missing from the system headers"
    Note $STDOUT " *"
    Note $STDOUT " */"
    Note $STDOUT ""

    flog $WLog pushd $PSY_Base/score

    set opt = ""
    set opt = ( $opt $Cfe_CC_Flags )
    set opt = ( $opt $Cfe_LD_Flags $Cfe_LD_RPath )
    set opt = ( `echo $opt | sed 's|\"||g'` )

    set inc = ""
    set inc = ( $inc -I.. -I../../psh -I$IncDir )

    flog $WLog mkdir $PSY_CfgDir
    flog $WLog cd $PSY_CfgDir
    flog $WLog $Cfe_CC_Exe -DMM_CONFIG $inc ../scmemi.c -o score-config $opt
    if ($status == 0) then

# NOTE: coded this way to throw away message to stderr
# which may come from CROSS_FE instead of score-config
       flog $WLog ( ( $CROSS_FE ./score-config >> $STDOUT ) >& /dev/null )
       if ($status == 0) then
          flog $WLog $RM ./score-config
          Note $WLog "score-config execution succeeded"
       else
          Note $WLog "score-config execution failed"
       endif

# if score-config fails choose largest possible value for N_DOUBLES_MD
# so the build can proceed successfully (if non-optimally)
    else
       NoteD $WLog "Failed to correctly configure the SCORE Memory Manager"
       Note $STDOUT "#define N_DOUBLES_MD 8"
    endif
    flog $WLog cd ..
    flog $WLog $RMDir $PSY_CfgDir

    flog $WLog popd
    Note $STDOUT ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

source $Me:h/post-common

exit(0)

