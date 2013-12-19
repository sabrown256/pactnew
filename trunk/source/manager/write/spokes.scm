#!/bin/csh -f
#
# SPOKES.SCM - write the SPOKES.SCM file
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

set Me = $0
source $Me:h/pre-common

Note $WLog "----- write/spokes.scm -----"
Note $WLog "Write: spokes.scm"
Note $WLog ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $WLog

NoteD $WLog "   Binary File Translation Spokes Loader - spokes.scm"
Note $WLog ""

dbmget $WLog RF_SPOKES_C^         \
             RF_SPOKES_SCHEME^    \
             SHELL_Cont^          \
             PSY_Base^            \
             PSY_Root^            \
             PSY_Cfg

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

if (-d $PSY_Base/sx/applications) then

    set STDOUT = $PSY_Root/scheme/spokes.scm

    flog $WLog $RM $STDOUT
    flog $WLog touch $STDOUT

    Note $STDOUT ";"
    Note $STDOUT "; SPOKES.SCM - initialization of SX translation spokes for $PSY_Cfg"
    Note $STDOUT ";"
    Note $STDOUT ""
    Note $STDOUT '(define (safe-load file)'
    Note $STDOUT '  (if (file? file nil nil "global")'
    Note $STDOUT '      (load file #t)'
    Note $STDOUT '      (printf nil "Cannot find %s'$SHELL_Cont'n" file)))'
    Note $STDOUT ""

    flog $WLog set lspokes = ( `echo $RF_SPOKES_SCHEME` )
    flog $WLog set nspokes = $#lspokes
    flog $WLog set ispoke  = 0
    while (`expr $ispoke \< $nspokes`)
       set ispoke = `expr $ispoke + 1`
       set tspoke = $lspokes[$ispoke]
       Note $STDOUT "(safe-load" '"'$tspoke".scm"'"'")"
    end

    flog $WLog set lspokes = ( `echo $RF_SPOKES_C` )
    flog $WLog set nspokes = $#lspokes
    flog $WLog set ispoke  = 0
    while (`expr $ispoke \< $nspokes`)
       set ispoke = `expr $ispoke + 1`
       set tspoke = $lspokes[$ispoke]
       Note $STDOUT "(safe-load" '"'$tspoke".scm"'"'")"
    end
    Note $STDOUT ""

endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

source $Me:h/post-common

exit(0)

