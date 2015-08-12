#
# CMakeList.txt for fortran
#
# Files and Directories
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#
set(Package fortran)
set(LibName fpact)
set(TGTLib  fpact)

#--TGTLib = ${LibDir}/lib${LibName}.a
#--TGTInc = noinc

#--OD = DefaultO

#
# Compiler/Loader Options
#
#--CcFlags = -I. -I${PACTSrcDir} -I${IncDir} ${MDInc} ${MDGInc}
#--FcFlags = ${FC_MOD_FLAG}${IncDir} ${MDInc}
#--LdFlags = -g -L${LibDir} ${LDFLAGS}

#--ACFlags = -avrb ${AODC} ${CFLAGS} ${Shared_CC_Flags} ${CcFlags}
#--AFFlags = -avrb ${AODF} ${FFLAGS} ${FC_MOD_FLAG}${tmpobj} ${FcFlags}

#--AFC = afc ${AODF} ${FFLAGS} ${FcFlags}

#
# Files to Save for Backup (make save)
#
#--SaveFiles = *.f

#
# Files to remove in directory clean (pact clean)
#
#--CleanFiles = ${gmo} *~ core

#
# Files to remove in directory clean (pact clean)
#
#--RemoveFiles = ${gmo} *~ core

#
# Things upon which install depends (pact install)
#
#--InstallDep = ${TGTLib}
#--BinDep =
#--BinObj =

#
# Required Libraries
#

#--targets :
#--	@echo "Targets for PFTN"
#--	@echo "   contour      : make PGS Fortran contour test"
#--	@echo "   fdp          : make PDB Fortran DP IO test"
#--	@echo "   fha          : make Fortran API of hash test"
#--	@echo "   install      : install PFTN headers and utilities in dev/PSY_id"
#--	@echo "   link         : link PFTN utilities for public use"
#--	@echo "   panacea      : make PANACEA Fortran API tests"
#--	@echo "   pdb          : make PDB Fortran API tests"
#--	@echo "   scan         : make lexical scanner test"
#--	@echo "   test         : run the PFTN test suite"

#
# Headers Files
#
set(Hdrs fpact.h)

set(PACTTmpDir ${CMAKE_CURRENT_BINARY_DIR})

set(gfs
    ${PACTTmpDir}/gf-score.c
    ${PACTTmpDir}/gf-pml.c
    ${PACTTmpDir}/gf-pdb.c
    ${PACTTmpDir}/gf-pgs.c
    ${PACTTmpDir}/gf-panacea.c
    ${PACTTmpDir}/gf-scheme.c
    ${PACTTmpDir}/gf-fortran.c
)

set(gms
    ${PACTTmpDir}/gm-score.f
    ${PACTTmpDir}/gm-pml.f
    ${PACTTmpDir}/gm-pdb.f
    ${PACTTmpDir}/gm-pgs.f
    ${PACTTmpDir}/gm-panacea.f
    ${PACTTmpDir}/gm-scheme.f
    ${PACTTmpDir}/gm-fortran.f
)

set_source_files_properties(
    ${gfs} ${gms} PROPERTIES GENERATED TRUE
)

#Command: mv gs-fortran.c ../../../sx/z-lnx-2.12-o/obj
#Command: mv gs-fortran.h ../../../sx/z-lnx-2.12-o/obj
#Command: mv gp-fortran.c ../../../python/z-lnx-2.12-o/obj
#Command: mv gp-fortran.h ../../../python/z-lnx-2.12-o/obj
#Command: mv gh-fortran.html ../../../doc
#set(GenTargets
#    bproto-score
#    bproto-pml
#    bproto-pdb
#    bproto-pgs
#    bproto-panacea
#    bproto-scheme
##    bproto-sx   include automatically
#)


set(FSrcs ${gms} flibc.f fapdprm.f)

set(CSrcs
    ${gfs} fasca.c fascb.c fascs.c fascm.c
    famla.c fapda.c fapana.c
    fagsa.c fagsb.c fagsc.c
    fasxa.c
)

set(Srcs ${CSrcs} ${FSrcs})

#
# Object Files
#

#--gfo = gf-score.o gf-pml.o gf-pdb.o   \
#--      gf-pgs.o gf-panacea.o \
#--      gf-scheme.o gf-fortran.o

#--gmo = gm-score.o gm-pml.o gm-pdb.o   \
#--      gm-pgs.o gm-panacea.o \
#--      gm-scheme.o gm-fortran.o

#--Objs = ${gmo} ${gfo} flibc.o \
#--       fasca.o fascb.o fascs.o fascm.o \
#--       famla.o fapda.o fapana.o \
#--       fagsa.o fagsb.o fagsc.o \
#--       fasxa.o

#--gfa = ${TGTLib}(gf-score.o) ${TGTLib}(gf-pml.o) ${TGTLib}(gf-pdb.o)   \
#--      ${TGTLib}(gf-pgs.o) ${TGTLib}(gf-panacea.o) \
#--      ${TGTLib}(gf-scheme.o) ${TGTLib}(gf-fortran.o)

#--gma = ${TGTLib}(gm-score.o) ${TGTLib}(gm-pml.o) ${TGTLib}(gm-pdb.o)   \
#--      ${TGTLib}(gm-pgs.o) ${TGTLib}(gm-panacea.o) \
#--      ${TGTLib}(gm-scheme.o) ${TGTLib}(gm-fortran.o)

#--ArObjs = ${gma} ${gfa} ${TGTLib}(flibc.o) \
#--         ${TGTLib}(fasca.o) ${TGTLib}(fascb.o) ${TGTLib}(fascs.o) \
#--         ${TGTLib}(fascm.o) \
#--         ${TGTLib}(famla.o) ${TGTLib}(fapda.o) ${TGTLib}(fapana.o) \
#--         ${TGTLib}(fagsa.o) ${TGTLib}(fagsb.o) ${TGTLib}(fagsc.o) \
#--         ${TGTLib}(fasxa.o) ${TGTLib}(fapdprm.o)

#--${ArObjs} : ${Hdrs}
#--${Objs} : ${Hdrs}
#--${LibDep} : ${Hdrs}

#--modules : ${LibDep}
#--	@touch dummy.mod
#--	sinstall *.mod ${IncDir}
#--	@rm -f dummy.mod ${IncDir}/dummy.mod
#--	@rm -rf ${PSY_TmpDir}




#--${PACTTmpDir}/gm-${LibName}.f : ${PACTSrcDir}/flibc.f
#--	@../scripts/bproto

#--genmod : ${PACTTmpDir}/gm-${LibName}.f

#--IncAction = @pact genmod

#LibAction = @pact devgen

#--SDir = ${PACTSrcDir}
#--LibAction = @(cd ${PACTTmpDir} ; pact SDir=\"${SDir}\" PACTSrcDir=\".\" PACTTmpDir=\".\" LibAction=\"\" -f ../../pre-Make Hdrs=../../fpact.h modules)

set(SLibs  ${DPLib} ${FLib} ${MDLib})
set(Libs   fpact panacea pgs ${MDGLib} pdb pml score ${SLibs})
set(NGLibs fpact panacea pgs ${MDGLib} pdb pml score ${SLibs})

set(do-bproto TRUE)

include(${PACT_MANAGER_DIR}/cmake/pact_targets.cmake)

#--build :
#--	@rm -f *.mod >& /dev/null
#--	@acc ${ACFlags} -c ${CSrcs} -a ${TGTLib}
#--	@afc ${AFFlags} -c ${FSrcs} -a ${TGTLib}
#--	@(cd ${PACTTmpDir} ; \
#--	  touch dummy.mod ; \
#--	  sinstall *.mod ${IncDir} ; \
#--	  rm -f dummy.mod ${IncDir}/dummy.mod ; \
#--	  rm -rf ${PSY_TmpDir})

#
# SCORE tests
#

#--fha : ${BinDir}/tscfh

#--${BinDir}/tscfh : tscfh.f ${InstallDep}
#--	${AFC} ${LdFlags} tscfh.f -o ${BinDir}/tscfh ${Libs}
add_executable(tscfh tscfh.f)
target_link_libraries(tscfh ${Libs})

#--omp : ${BinDir}/tscfomp

#--${BinDir}/tscfomp : tscfomp.f ${InstallDep}
#--	${AFC} ${LdFlags} tscfomp.f -o ${BinDir}/tscfomp ${Libs}
#add_executable(tscfomp tscfomp.f)
#target_link_libraries(tscfomp ${Libs})

#--scan : ${BinDir}/tscls

#--${BinDir}/tscls : tscls.c ${InstallDep}
#--	@(echo "${CCAnnounce} tscls.c -o ${BinDir}/tscls" ; \
#--	  ${CLD} ${LdFlags} tscls.c -o ${BinDir}/tscls ${Libs})
add_executable(tscls tscls)
target_link_libraries(tscls ${Libs})


# PDB tests

#--pdb : ${BinDir}/tpdf ${BinDir}/tpdfw ${BinDir}/tpdfg ${BinDir}/tpdfhy \
#--      ${BinDir}/tpcf

#--${BinDir}/tpdf : tpdf.f ${InstallDep}
#--	${AFC} ${LdFlags} tpdf.f -o ${BinDir}/tpdf ${Libs}
add_executable(tpdf2 tpdf.f)
target_link_libraries(tpdf2 ${Libs})

#--${BinDir}/tpdfw : tpdfw.f ${InstallDep}
#--	${AFC} ${LdFlags} tpdfw.f -o ${BinDir}/tpdfw ${Libs}

#--${BinDir}/tpdfg : tpdfg.f ${InstallDep}
#--	${AFC} ${LdFlags} tpdfg.f -o ${BinDir}/tpdfg ${Libs}
add_executable(tpdfg tpdfg.f)
target_link_libraries(tpdfg ${Libs})

#--${BinDir}/tpdfhy : tpdfhy.f ${InstallDep}
#--	${AFC} ${LdFlags} tpdfhy.f -o ${BinDir}/tpdfhy ${Libs}
add_executable(tpdfhy tpdfhy.f)
target_link_libraries(tpdfhy ${Libs})

#--${BinDir}/tpcf : tpcf.f ${InstallDep}
#--	${AFC} ${LdFlags} tpcf.f -o ${BinDir}/tpcf ${Libs}

#--fdp : ${BinDir}/tpdfd

#--${BinDir}/tpdfd : tpdfd.f ${InstallDep}
#--	${AFC} ${LdFlags} tpdfd.f -o ${BinDir}/tpdfd ${Libs}
#add_executable(tpdfd2 tpdfd.f)   # MPI
#target_link_libraries(tpdfd2 ${Libs})


# PGS tests

#--pgs : ${BinDir}/tgsf ${BinDir}/tgsfg ${BinDir}/tgsfc

#--${BinDir}/tgsf : tgsf.f ${InstallDep}
#--	${AFC} ${LdFlags} tgsf.f -o ${BinDir}/tgsf ${NGLibs}
add_executable(tgsf tgsf.f)
target_link_libraries(tgsf ${NGLibs})

#--${BinDir}/tgsfg : tgsfg.f ${InstallDep}
#--	${AFC} ${LdFlags} tgsfg.f -o ${BinDir}/tgsfg ${NGLibs}
add_executable(tgsfg tgsfg.f)
target_link_libraries(tgsfg ${NGLibs})

#--contour : ${BinDir}/tgsfc

#--${BinDir}/tgsfc : tgsfc.f ${InstallDep}
#--	${AFC} ${LdFlags} tgsfc.f -o ${BinDir}/tgsfc ${NGLibs}
add_executable(tgsfc tgsfc.f)
target_link_libraries(tgsfc ${NGLibs})

#
# PANACEA tests
#
#--panacea : ${BinDir}/tpath ${BinDir}/tpafth

#--${BinDir}/tpath : tpath.c ${InstallDep}
#--	@(echo "${CCAnnounce} tpath.c -o ${BinDir}/tpath" ; \
#--	  ${CLD} ${LdFlags} tpath.c -o ${BinDir}/tpath ${NGLibs})
add_executable(tpath tgsfg.c)
target_link_libraries(tpath ${NGLibs})

#--${BinDir}/tpafth : tpafth.f ${InstallDep}
#--	${AFC} ${LdFlags} tpafth.f -o ${BinDir}/tpafth ${NGLibs}


set_target_properties(
    ${TGTLib} tscfh
# tscfomp
    tscls

    tpdf2 tpdfg tpdfhy #tpdfd2
    tgsf tgsfg tgsfc
    PROPERTIES Fortran_FORMAT "FREE")

#set_property(GLOBAL PROPERTY Fortran_FORMAT "FREE")


#
# link 
#
#--link: ${InstallDep}

#
# install
#
#--install:
#--	pact link

#
# sharedlib
#
#--sharedlib:
#--	pact shared

#
# Run the PFTN test suite
#
#--test :
#--	@./fatest

set(Tests score pdb pgs panacea)
foreach(test ${Tests})
    set(testname "${Package}.${test}")
    add_test(
        NAME ${testname}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/fatest ${test}
    )
endforeach(test)
