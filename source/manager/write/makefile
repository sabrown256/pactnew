#!/bin/csh -f
#
# MAKEFILE - write the distribution MAKEFILE
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

dbget BinDir
dbget IncDir
dbget Pck

Separator $Log
Note $Log "   BinDir  = $BinDir"
Note $Log "   IncDir  = $IncDir"
Note $Log "   Pck     = $Pck"

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    NoteD $Log "   Distribution Makefile - Makefile"

    set STDOUT = $IncDir/Makefile

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    set Pck = `cat $IncDir/package`

    Note $Log "Packages: $Pck"

    Note $STDOUT "#"
    Note $STDOUT "# MAKEFILE - operate on this distribution"
    Note $STDOUT "#"
    Note $STDOUT ""

    Note $STDOUT "PACT = $BinDir/pact"
    Note $STDOUT ""

    Note $STDOUT 'build:'
    Note $STDOUT '	@(echo "" ; echo "Generate sources")'
    Note $STDOUT '	${BARRIER}'
    foreach i ($Pck)
       Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact generate in '$i'" ; ${PACT} -rcr generate)'
       if ($i == score) then
          Note $STDOUT '	${BARRIER}'
       endif
    end
    Note $STDOUT '	${BARRIER}'

    Note $STDOUT '	@(echo "" ; echo "Install headers")'
    Note $STDOUT '	${BARRIER}'
    foreach i ($Pck)
       Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact incinstall in '$i'" ; ${PACT} -rcr incinstall)'
    end
    Note $STDOUT '	${BARRIER}'

    Note $STDOUT '	@(echo "" ; echo "Compile sources")'
    Note $STDOUT '	${BARRIER}'
    foreach i ($Pck)
       Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact devlib in '$i'" ; ${PACT} -rcr devlib)'
    end
    Note $STDOUT '	${BARRIER}'

    Note $STDOUT '	@(echo "" ; echo "Link Executables")'
    Note $STDOUT '	${BARRIER}'
    foreach i ($Pck)
       Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact link in '$i'" ; ${PACT} -rcr link)'
    end
    Note $STDOUT ''

    Note $STDOUT 'link:'
    Note $STDOUT '	@(echo "" ; echo "Link Executables")'
    Note $STDOUT '	${BARRIER}'
    foreach i ($Pck)
       Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact link in '$i'" ; ${PACT} -rcr link)'
    end
    Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

