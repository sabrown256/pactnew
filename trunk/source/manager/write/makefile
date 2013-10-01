#!/bin/csh -f
#
# MAKEFILE - write the distribution MAKEFILE
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

set Me = $0
source $Me:h/common

Note $Log "----- write/makefile -----"
Note $Log "Write: makefile"
Note $Log ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $Log

NoteD $Log "   Distribution Makefile - Makefile"
Note $Log ""

dbmget $Log BinDir^   \
            EtcDir^   \
            System^   \
            Pck

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    set STDOUT = $EtcDir/Makefile

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    set Pck = `cat $EtcDir/package`

    Note $Log "Packages: $Pck"

    Note $STDOUT "#"
    Note $STDOUT "# MAKEFILE - operate on this distribution"
    Note $STDOUT "#"
    Note $STDOUT ""

    Note $STDOUT "PACT   = $BinDir/pact"
    Note $STDOUT "MkFile = $EtcDir/Makefile"
    Note $STDOUT ""

# build target
    Note $STDOUT 'build:'
    Note $STDOUT '	@(echo "" ; echo "Generate sources for '$System'")'
    Note $STDOUT '	${BARRIER}'
    foreach i ($Pck)
       Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact generate in '$i'" ; ${PACT} -rcr generate)'
       if ($i == score) then
          Note $STDOUT '	${BARRIER}'
       endif
    end
    Note $STDOUT '	${BARRIER}'

    Note $STDOUT '	@(echo "" ; echo "Install headers for '$System'")'
    Note $STDOUT '	${BARRIER}'
    foreach i ($Pck)
       Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact incinstall in '$i'" ; ${PACT} -rcr incinstall)'
    end
    Note $STDOUT '	${BARRIER}'

    Note $STDOUT '	@(echo "" ; echo "Compile sources for '$System'")'
    Note $STDOUT '	${BARRIER}'
    foreach i ($Pck)
       Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact devlib in '$i'" ; ${PACT} -rcr devlib)'
    end
    Note $STDOUT '	${BARRIER}'
    Note $STDOUT '	${PACT} -f ${MkFile} link'
    Note $STDOUT ''

# link target
    Note $STDOUT 'link:'
    Note $STDOUT '	@(echo "" ; echo "Link Executables for '$System'")'
    Note $STDOUT '	${BARRIER}'
    foreach i ($Pck)
       Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact link in '$i'" ; ${PACT} -rcr link)'
    end
    Note $STDOUT '	${BARRIER}'
    Note $STDOUT '	${PACT} -f ${MkFile} unilib'
    Note $STDOUT ''

# unilib target
    Note $STDOUT 'unilib:'
    Note $STDOUT '	@(cd manager ; ./pact-lib -a '$System')'
    Note $STDOUT ''

# test target
    Note $STDOUT 'test:'

    foreach i ($Pck)
       Note $STDOUT '	@(cd '$i' ; echo "" ; echo "pact test in '$i'" ; ${PACT} -rcr test)'
    end
    Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

