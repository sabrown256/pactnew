#
# CMakeLists.txt for PANACEA
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#
set(Package panacea)
set(LibName panacea)
set(TGTLib  panacea)

#--TGTLib = ${LibDir}/lib${LibName}.a
#--TGTExe = ${BinDir}/toul
set(TGTInc panace.h panacea_int.h)

set(TGTScm
    applications/panache.scm applications/panabgc.scm
    applications/panagc.scm
    applications/panbgc.scm applications/panpgc.scm
    applications/panint.scm
)

#--TGTScr = applications/panache

#--OD = DefaultG

#
# Compiler/Loader Options
#
#--CcFlags = -I${IncDir} ${MDInc} ${MDGInc}
#--LdFlags = -L${LibDir} ${LDFLAGS}

#--ACFlags = -avrb ${AODC} ${CFLAGS} ${Shared_CC_Flags} -I. ${CcFlags}

#
# Files to Save for Backup (pact save)
#
#--SaveFiles = *.c *.h

#
# Files to remove in directory clean (pact clean)
#
#--CleanFiles = 

#
# Things upon which install depends (pact install)
#
#--DepLib = ${LibDir}/libpdb.a \
#--         ${LibDir}/libpml.a \
#--	 ${LibDir}/libscore.a

#--InstallDep = ${TGTLib} ${TGTExe} ${TGTScr}
#--BinDep = ${Hdrs} ${TGTLib} toul.c ${DepLib}
#--BinObj = ${LdFlags} toul.c

#
# Required Libraries
#
# NOTE: building shared seems to require these:
#--Libs   = ${LPANACEA} ${LPGS} ${MDGLib} ${LPDB} ${LPML} ${LSCORE} ${MDLib}
#--NGLibs = ${LPANACEA} ${LPGS} ${MDGLib} ${LPDB} ${LPML} ${LSCORE} ${MDLib}
# NOTE: building static seems to require only these:
#Libs   = ${LPANACEA} ${LPDB} ${LPML} ${LSCORE} ${MDLib}
#NGLibs = ${LPANACEA} ${LPDB} ${LPML} ${LSCORE} ${MDLib}

#--targets :
#--	@echo "Targets for PANACEA"
#--	@echo "   Cth       : make C time history tests"
#--	@echo "   defvar    : make variable definition test"
#--	@echo "   Fth       : make Fortran time history tests"
#--	@echo "   install   : install PANACEA headers and utilities in dev/sys_id"
#--	@echo "   link      : link PANACEA utilities for public use"
#--	@echo "   prep      : make preprocessing test"
#--	@echo "   sharedlib : build and install PANACEA shared libs in dev/sys_id"
#--	@echo "   test      : run the PANACEA test suite"

#
# Headers Files
#
set(Hdrs ${TGTInc} ${hdrf})

#
# Object Files
#
set(PACTTmpDir  ${CMAKE_CURRENT_BINARY_DIR})

set(gcs ${PACTTmpDir}/gc-panacea.c)

set(Srcs
    padef.c pacpp.c papkg.c pagnrd.c pahand.c pafi.c
    panacea.c pashar.c pashas.c paccess.c padsys.c pamm.c
    pavars.c papp.c padump.c pasrc.c pastr.c pasmp.c panth.c
    ${gcs}
)

#--gco = gc-panacea.o

#--GenObjs = pagnrd.o pahand.o pafi.o
#--PanObjs = padsys.o pamm.o panacea.o paccess.o pashar.o pashas.o \
#--          pastr.o pavars.o padef.o pacpp.o papkg.o pasmp.o
#--PPObjs  = papp.o padump.o panth.o
#--SorObjs = pasrc.o
#--Objs    = ${GenObjs} ${SorObjs} ${PanObjs} ${PPObjs} ${gco}

#--gca = ${TGTLib}(gc-panacea.o)

#--ArObjs = ${TGTLib}(padef.o) ${TGTLib}(pacpp.o) ${TGTLib}(papkg.o)      \
#--	 ${TGTLib}(pagnrd.o) ${TGTLib}(pahand.o) ${TGTLib}(pafi.o)     \
#--         ${TGTLib}(panacea.o) ${TGTLib}(pashar.o) ${TGTLib}(pashas.o)  \
#--         ${TGTLib}(paccess.o) ${TGTLib}(padsys.o) ${TGTLib}(pamm.o)    \
#--         ${TGTLib}(pavars.o) ${TGTLib}(papp.o) ${TGTLib}(padump.o)     \
#--         ${TGTLib}(pasrc.o) ${TGTLib}(pastr.o)                         \
#--         ${TGTLib}(pasmp.o) ${TGTLib}(panth.o) \
#--         ${gca}

#--${LibDep} : ${hdrf} ${Hdrs}

#
# Generated code rules
#

#--${IncDir}/pasrc.h : pasrc.t
#--	@(echo "template pasrc.t -o ${IncDir}/pasrc.h" ;  \
#--	  ${BinDir}/template pasrc.t -o ${IncDir}/pasrc.h)
add_custom_command(
    OUTPUT ${IncDir}/pasrc.h
    DEPENDS template ${CMAKE_CURRENT_SOURCE_DIR}/pasrc.t
    COMMAND template
        ${CMAKE_CURRENT_SOURCE_DIR}/pasrc.t
        -t ${DB_TYPES}
        -o ${IncDir}/pasrc.h
    COMMENT "template pasrc.t -o ${IncDir}/pasrc.h"
    )
LIST(APPEND hdrf ${IncDir}/pasrc.h)

#--GenInc = ${PACTTmpDir}/gc-panacea.h

#--${IncDir}/panacea_gen.h : ${GenInc}
#--	@echo "build panacea_gen.h"
#--	@rm -f ${IncDir}/panacea_gen.h
#--	@cat ${GenInc} > ${IncDir}/panacea_gen.h

#--hdrf = ${IncDir}/pasrc.h ${IncDir}/panacea_gen.h

#Command: mv gf-panacea.c ../../../fortran/z-lnx-2.12-o/obj
#Command: mv gm-panacea.f ../../../fortran/z-lnx-2.12-o/obj
#Command: mv gs-panacea.c ../../../sx/z-lnx-2.12-o/obj
#Command: mv gs-panacea.h ../../../sx/z-lnx-2.12-o/obj
#Command: mv gp-panacea.c ../../../python/z-lnx-2.12-o/obj
#Command: mv gp-panacea.h ../../../python/z-lnx-2.12-o/obj
#Command: mv gh-panacea.html ../../../doc

#--IncAction = @pact generate
set(do-bproto TRUE)

include(${PACT_MANAGER_DIR}/cmake/pact_targets.cmake)

#--LibAction = @pact devgen

#--build : ${hdrf}
#--	@acc ${ACFlags} -c ${Srcs} -a ${TGTLib}

#
# preprocessing test
#
#--prep : ${BinDir}/tpacpp

#--${BinDir}/tpacpp : tpacpp.c ${TGTLib}
#--	@(echo "${CCAnnounce} tpacpp.c -o ${BinDir}/tpacpp" ; \
#--	  ${CLD} ${LdFlags} tpacpp.c -o ${BinDir}/tpacpp ${NGLibs})
#
# test new variable definition
#
#--defvar : ${BinDir}/tpadef

#--${BinDir}/tpadef : tpadef.c ${TGTLib}
#--	@(echo "${CCAnnounce} tpadef.c -o ${BinDir}/tpadef" ; \
#--	  ${CLD} ${LdFlags} tpadef.c -o ${BinDir}/tpadef ${NGLibs})

#
# make the executable
#
#--${TGTExe} : ${BinDep}
#--	@(echo "${CCAnnounce} toul.c -o ${TGTExe}" ; \
#--	  ${CLD} ${LdFlags} toul.c -o ${TGTExe} ${Libs})

#
# link 
#
#--link: ${InstallDep}
#--	pact man3install
#--	pact scminstall
#--	pact scrinstall

#
# install 
#
#--install:
#--	pact link
#--	pact incinstall
#--	pact scminstall
#--	pact scrinstall

#
# sharedlib
#
#--sharedlib:
#--	pact shared
#--	pact incinstall

#
# inform
#
#--inform :
#--	@rm -f required.objs
#--	@echo ${Objs} > required.objs

#
# Run the PANACEA test suite
#
#--test :
#--	@./patest

#set(Tests gen abc)
foreach(test ${Tests})
    set(testname "${Package}.${test}")
    add_test(
        NAME ${testname}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/patest ${test}
    )
endforeach(test)
