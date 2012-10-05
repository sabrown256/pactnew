#!/bin/csh -f
#
# SPOKES.SCM - write the SPOKES.SCM file
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
source $ScrDir/functions-pact.csh

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $Log

NoteD $Log "   Binary File Translation Spokes Loader - spokes.scm"
Note $Log ""

set vl = ""
set vl = ( $vl BaseDir SchDir Sys )
set vl = ( $vl SchemeSpokes CSpokes )

source $MngDir/write/import-db

dbget BackSlash

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

if (-d $BaseDir/sx/applications) then

    set STDOUT = $SchDir/spokes.scm

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    Note $STDOUT ";"
    Note $STDOUT "; SPOKES.SCM - initialization of SX translation spokes for $Sys"
    Note $STDOUT ";"
    Note $STDOUT ""
    Note $STDOUT '(define (safe-load file)'
    Note $STDOUT '  (if (file? file nil nil "global")'
    Note $STDOUT '      (load file #t)'
    Note $STDOUT '      (printf nil "Cannot find %s'$BackSlash'n" file)))'
    Note $STDOUT ""

    flog $Log set lSpokes = ( `echo $SchemeSpokes` )
    flog $Log set nspokes = $#lSpokes
    flog $Log set ispoke  = 0
    while (`expr $ispoke \< $nspokes`)
       set ispoke = `expr $ispoke + 1`
       set tspoke = $lSpokes[$ispoke]
       Note $STDOUT "(safe-load" '"'$tspoke".scm"'"'")"
    end

    flog $Log set lSpokes = ( `echo $CSpokes` )
    flog $Log set nspokes = $#lSpokes
    flog $Log set ispoke  = 0
    while (`expr $ispoke \< $nspokes`)
       set ispoke = `expr $ispoke + 1`
       set tspoke = $lSpokes[$ispoke]
       Note $STDOUT "(safe-load" '"'$tspoke".scm"'"'")"
    end
    Note $STDOUT ""

endif

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

