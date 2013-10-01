#!/bin/csh -f
#
# MAKE-MACROS.MP - write the MAKE-MACROS.MP file
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

set Me = $0
source $Me:h/common

Note $Log "----- write/make-macros.mp -----"
Note $Log "Write: make-macros.mp"
Note $Log ""

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

Separator $Log

NoteD $Log "   System Dependent MAKE Macros - make-macros.mp"
Note $Log ""

dbmget $Log EtcDir^            \
            Sys

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

    set STDOUT = $EtcDir/make-macros.mp

    flog $Log $RM $STDOUT
    flog $Log touch $STDOUT

    Note $STDOUT '#'
    Note $STDOUT '# MAKE-MACROS.MP - PACT Makefile macros for '$Sys
    Note $STDOUT '#'
    Note $STDOUT ""

    Note $STDOUT 'shared :'
    Note $STDOUT ''

    Note $STDOUT '#'
    Note $STDOUT '# make new library'
    Note $STDOUT '#'
    Note $STDOUT '${TGTLib} : ${LibDep}'
    Note $STDOUT ''

    Note $STDOUT 'devlib : ${TGTLib}'
    Note $STDOUT ''

    Note $STDOUT '#'
    Note $STDOUT '# make place-holder for executable'
    Note $STDOUT '#'
    Note $STDOUT '${TGTBin} : ${BinDep}'
    Note $STDOUT ''

    Note $STDOUT '#'
    Note $STDOUT '# install library'
    Note $STDOUT '#'
    if ($MAKE_Shared_Libs == TRUE) then
       Note $STDOUT 'libinstall : ${TGTLib} ${TGTShLib}'
    else
       Note $STDOUT 'libinstall : ${TGTLib}'
    endif
    Note $STDOUT ''

    Note $STDOUT '#'
    Note $STDOUT '# install library archive'
    Note $STDOUT '#'
    Note $STDOUT 'arcinstall : ${TGTLib}'
    Note $STDOUT ''

    Note $STDOUT '#'
    Note $STDOUT '# install includes'
    Note $STDOUT '#'
    Note $STDOUT 'incinstall : ${TGTInc}'
    Note $STDOUT ""

    Note $STDOUT '#'
    Note $STDOUT '# install binaries'
    Note $STDOUT '#'
    Note $STDOUT 'bininstall : ${TGTBin}'
    Note $STDOUT ""

    Note $STDOUT '#'
    Note $STDOUT '# install scheme files'
    Note $STDOUT '#'
    Note $STDOUT 'scminstall : ${TGTScm}'
    Note $STDOUT ""

    Note $STDOUT '#'
    Note $STDOUT '# install (scheme) extensions files'
    Note $STDOUT '#'
    Note $STDOUT 'extinstall :'
    Note $STDOUT ""

    Note $STDOUT '#'
    Note $STDOUT '# install scripts'
    Note $STDOUT '#'
    Note $STDOUT 'scrinstall : ${TGTScr}'
    Note $STDOUT '	@[ ! -f ${BinDir}/.lock ] || { echo "${BinDir} is locked" ; exit 1 ; }'
    Note $STDOUT '	@[ "${TGTScr}" != "noscr" ] && { echo "sinstall ${TGTScr} ${BinDir}" ; sinstall ${TGTScr} ${BinDir} ; } || { exit 0 ; }'
#    Note $STDOUT '	sinstall ${TGTScr} ${BinDir}'
    Note $STDOUT ""

    Note $STDOUT '#'
    Note $STDOUT '# install man pages'
    Note $STDOUT '#'
    Note $STDOUT 'man1install :'
    Note $STDOUT ""
    Note $STDOUT 'man3install :'
    Note $STDOUT ""

    Note $STDOUT '#'
    Note $STDOUT '# clean up the directory'
    Note $STDOUT '#'
    Note $STDOUT 'clean :'
    Note $STDOUT ''

    Note $STDOUT '#'
    Note $STDOUT '# remove binaries and libraries'
    Note $STDOUT '#'
    Note $STDOUT 'remove :'
    Note $STDOUT ''

#--------------------------------------------------------------------------
#--------------------------------------------------------------------------

exit(0)

