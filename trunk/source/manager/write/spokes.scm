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
source $Me:h/common

Note $Log "----- write/spokes.scm -----"
Note $Log "Write: spokes.scm"
Note $Log ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $Log

NoteD $Log "   Binary File Translation Spokes Loader - spokes.scm"
Note $Log ""

dbmget $Log RF_SPOKES_C^         \
            RF_SPOKES_SCHEME^    \
            SHELL_Cont^          \
            PSY_Base^            \
            PSY_Root^            \
            PSY_Cfg

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

if (-d $PSY_Base/sx/applications) then

    set STDOUT = $PSY_Root/scheme/spokes.scm

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    Note $STDOUT ";"
    Note $STDOUT "; SPOKES.SCM - initialization of SX translation spokes for $PSY_Cfg"
    Note $STDOUT ";"
    Note $STDOUT ""
    Note $STDOUT '(define (safe-load file)'
    Note $STDOUT '  (if (file? file nil nil "global")'
    Note $STDOUT '      (load file #t)'
    Note $STDOUT '      (printf nil "Cannot find %s'$SHELL_Cont'n" file)))'
    Note $STDOUT ""

    flog $Log set lspokes = ( `echo $RF_SPOKES_SCHEME` )
    flog $Log set nspokes = $#lspokes
    flog $Log set ispoke  = 0
    while (`expr $ispoke \< $nspokes`)
       set ispoke = `expr $ispoke + 1`
       set tspoke = $lspokes[$ispoke]
       Note $STDOUT "(safe-load" '"'$tspoke".scm"'"'")"
    end

    flog $Log set lspokes = ( `echo $RF_SPOKES_C` )
    flog $Log set nspokes = $#lspokes
    flog $Log set ispoke  = 0
    while (`expr $ispoke \< $nspokes`)
       set ispoke = `expr $ispoke + 1`
       set tspoke = $lspokes[$ispoke]
       Note $STDOUT "(safe-load" '"'$tspoke".scm"'"'")"
    end
    Note $STDOUT ""

endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

