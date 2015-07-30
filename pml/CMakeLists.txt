#
# CMakeLists.txt for Portable Math Library
#
# Files and Directories
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#
set(Package pml)
set(LibName pml)
set(TGTLib  pml)

#--TGTLib = ${LibDir}/lib${LibName}.a
#--TGTInc = ${Hdrs}

#--OD = DefaultO

#
# Compiler/Loader Options
#
#--CcFlags = -I${IncDir} ${MDInc}
#--LdFlags = -L${LibDir} ${LDFLAGS}

#--ACFlags = -avrb ${AODC} ${CFLAGS} ${Shared_CC_Flags} -I. ${CcFlags}

#--CXX = ${CC_Exe} ${CcFlags}

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
#--InstallDep = ${TGTLib} ${BinDir}/tmlcsm
#--BinDep =
#--BinObj =

#
# Required Libraries
#
set(Libs pml score ${MDLib})

#--targets :
#--	@echo "Targets for PML"
#--	@echo "   cgsolv    : make sparse linear CG solvers test"
#--	@echo "   checksum  : make checksum routines test"
#--	@echo "   eigen     : make eigen systems tests"
#--	@echo "   fpe       : make FPE tests"
#--	@echo "   funcs     : make functions test"
#--	@echo "   icsolv    : make matrix ICG solvers test"
#--	@echo "   install   : install PML headers and utilities in dev/sys_id"
#--	@echo "   interp    : make interpolation test"
#--	@echo "   link      : link PML utilities for public use"
#--	@echo "   lnnorm    : make ln norm test"
#--	@echo "   lrac      : make LR to AC mapping test"
#--	@echo "   matrix    : make matrix tests"
#--	@echo "   perf      : test arithmetic performance"
#--	@echo "   sharedlib : build and install PML shared libs in dev/sys_id"
#--	@echo "   svdsolv   : make SVD solvers test"
#--	@echo "   topsort   : make topological sort test"
#--	@echo "   test      : run the PML test suite"

#
# Headers Files
#
#XXX added scope_math.h
set(Hdrs pml.h pml_int.h scope_constants.h scope_complex.h scope_quaternion.h scope_math.h)
set(TGTInc ${Hdrs})


#
# patch FPE handling
# XXX - see make-macros for options
#--    if ("$HSY_FPU" == "x87") then
#--       set lcc = ( $FPE_Exe $FPE_Flags )
#--
#--       Note $STDOUT '	@(cd ${PACTTmpDir} ; \'
#--       Note $STDOUT '          echo "'$lcc' -c mlfpex86.c -o mlfpex86.o" ; \'
#--       Note $STDOUT '          '$lcc' -I${IncDir} -c ${PACTSrcDir}/mlfpex86.c -o mlfpex86.o ; \'
#--       Note $STDOUT '          ${AR} ${AROpt} ${TGTLib} mlfpex86.o 2> /dev/null ; \'
#--       Note $STDOUT '          ${RM} mlfpex86.o 2> /dev/null )'
#--
#--    else if ("$HSY_FPU" == "ppc") then
#--       set lcc = ( $FPE_Exe -g -Wall )
#--       if ($?Bits == 1) then
#--          set lcc = ( $lcc -m$Bits )
#--       endif
#--
#--       Note $STDOUT '	@(cd ${PACTTmpDir} ; \'
#--       Note $STDOUT '          echo "'$lcc' -c mlfpeppc.c -o mlfpeppc.o" ; \'
#--       Note $STDOUT '          '$lcc' -I${IncDir} -c ${PACTSrcDir}/mlfpeppc.c -o mlfpeppc.o ; \'
#--       Note $STDOUT '          ${AR} ${AROpt} ${TGTLib} mlfpeppc.o 2> /dev/null ; \'
#--       Note $STDOUT '          ${RM} mlfpeppc.o 2> /dev/null )'
#--    endif
#--
#--    if ($HAVE_GNU_FENV == TRUE) then
#--       set lcc = ( $FPE_Exe -g -Wall )
#--       if ($MAKE_Shared_Libs == TRUE) then
#--          set lcc = ( $lcc -fPIC )
#--       endif
#--       Note $STDOUT '	@(cd ${PACTTmpDir} ; \'
#--       Note $STDOUT '          '$lcc' -I${IncDir} -c ${PACTSrcDir}/mlfpegnu.c -o mlfpegnu.o ; \'
#--       Note $STDOUT '          ${AR} ${AROpt} ${TGTLib} mlfpegnu.o 2> /dev/null ; \'
#--       Note $STDOUT '          ${RM} mlfpegnu.o 2> /dev/null )'
#--    endif
#--    Note $STDOUT ""

#
set(patch-fpe  mlfpex86.c mlfpegnu.c)


#
# Object Files
#
#--gcs = ${PACTTmpDir}/gc-pml.c

set(Srcs  mlconst.c mlmath.c mlsrch.c mlrand.c mlnls.c mlmatr.c
       mlrfnc.c mlcfnc.c mlsfnc.c mlqfnc.c
       mlsolv.c mlsvd.c mlvnd.c mleigen.c
       mlcg.c mliccg.c mlicgs.c
       mlfft.c mllsq.c mlfpe.c mloper.c mlgeom.c mlsort.c mlmm.c
       mlintm.c mlintp.c mlints.c
       mlchksm.c mlmesh.c mlmsrch.c mlcmsh.c mlmextr.c
       mlvect.c mlsmp.c  ${patch-fpe}
#       ${gcs}
    )

#--gco = gc-pml.o

#--Objs = mlconst.o mlmath.o mlsrch.o mlrand.o mlnls.o mlmatr.o      \
#--       mlrfnc.o mlcfnc.o mlsfnc.o mlqfnc.o                        \
#--       mlsolv.o mlsvd.o mlvnd.o mleigen.o                         \
#--       mlcg.o mliccg.o mlicgs.o                                   \
#--       mlfft.o mllsq.o mlfpe.o mloper.o mlgeom.o mlsort.o mlmm.o  \
#--       mlintm.o mlintp.o mlints.o                                 \
#--       mlchksm.o mlmesh.o mlmsrch.o mlcmsh.o mlmextr.o            \
#--       mlvect.o mlsmp.o \
#--       ${gco}

#--gca = ${TGTLib}(gc-pml.o)

#--ArObjs = ${TGTLib}(mlconst.o) ${TGTLib}(mlmath.o)                       \
#--         ${TGTLib}(mlrfnc.o)  ${TGTLib}(mlcfnc.o)                       \
#--         ${TGTLib}(mlqfnc.o)  ${TGTLib}(mlsfnc.o)                       \
#--         ${TGTLib}(mlmatr.o)  ${TGTLib}(mlsort.o)  ${TGTLib}(mlnls.o)   \
#--         ${TGTLib}(mlsolv.o)  ${TGTLib}(mlsvd.o)   ${TGTLib}(mlvnd.o)   \
#--         ${TGTLib}(mlcg.o)    ${TGTLib}(mliccg.o)  ${TGTLib}(mlicgs.o)  \
#--         ${TGTLib}(mleigen.o) ${TGTLib}(mlsrch.o)  ${TGTLib}(mlfft.o)   \
#--         ${TGTLib}(mlintm.o)  ${TGTLib}(mlintp.o)  ${TGTLib}(mlints.o)  \
#--         ${TGTLib}(mllsq.o)   ${TGTLib}(mlfpe.o)   ${TGTLib}(mloper.o)  \
#--	 ${TGTLib}(mlgeom.o)  ${TGTLib}(mlrand.o)  ${TGTLib}(mlmm.o)    \
#--         ${TGTLib}(mlmesh.o)  ${TGTLib}(mlmsrch.o) ${TGTLib}(mlcmsh.o)  \
#--         ${TGTLib}(mlmextr.o) ${TGTLib}(mlchksm.o)                      \
#--         ${TGTLib}(mlvect.o)  ${TGTLib}(mlsmp.o) \
#--         ${gca}

#--${LibDep} : ${hdrf} ${Hdrs}

#
# Generated code rules
#

# headers

#--${IncDir}/mlsrch.h : mlsrch.t
#--	@(echo "template mlsrch.t -o ${IncDir}/mlsrch.h" ;  \
#--	  ${BinDir}/template mlsrch.t -o ${IncDir}/mlsrch.h)
add_custom_command(
    OUTPUT ${IncDir}/mlsrch.h
    DEPENDS template ${CMAKE_CURRENT_SOURCE_DIR}/mlsrch.t
    COMMAND ${BinDir}/template
        ${CMAKE_CURRENT_SOURCE_DIR}/mlsrch.t
        -t ${DB_TYPES}
        -o ${IncDir}/mlsrch.h
    COMMENT "template mlsrch.h"
    )
LIST(APPEND hdrf ${IncDir}/mlsrch.h)

LIST(APPEND GenInc ${CMAKE_CURRENT_BINARY_DIR}/gc-pml.h)

#Command: mv gf-pml.c ../../../fortran/z-lnx-2.12-o/obj
#Command: mv gm-pml.f ../../../fortran/z-lnx-2.12-o/obj
#Command: mv gs-pml.c ../../../sx/z-lnx-2.12-o/obj
#Command: mv gs-pml.h ../../../sx/z-lnx-2.12-o/obj
#Command: mv gp-pml.c ../../../python/z-lnx-2.12-o/obj
#Command: mv gp-pml.h ../../../python/z-lnx-2.12-o/obj
#Command: mv gh-pml.html ../../../doc

#--IncAction = @pact generate
set(do-bproto TRUE)

include(${PACT_MANAGER_DIR}/cmake/pact_targets.cmake)

#--LibAction = @(echo "pact patch-fpe" ; pact patch-fpe ; pact devgen)

#--build : ${hdrf}
#--	@acc ${ACFlags} -c ${Srcs} -a ${TGTLib}
#--	@pact patch-fpe

#
# verify C++ compiles
#
#--chk++ :
#--	${CXX} -c tests/chk++.cc -o ${PACTTmpDir}/chk++.o
# XXX = want to compile only
#add_executable(chk++ tml.c)
#target_link_libraries(tml ${Libs})

#
# make new test executable
#
#--matrix : ${BinDir}/tmlmtx

#--${BinDir}/tmlmtx : tmlmtx.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlmtx.c -o ${BinDir}/tmlmtx" ; \
#--	  ${CLD} ${LdFlags} tmlmtx.c -o ${BinDir}/tmlmtx ${Libs})
add_executable(tmlmtx tmlmtx.c)
target_link_libraries(tmlmtx ${Libs})

#
# test functions
#
#--funcs : ${BinDir}/tmlfn

#--${BinDir}/tmlfn : tmlfn.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlfn.c -o ${BinDir}/tmlfn" ; \
#--	  ${CLD} ${LdFlags} tmlfn.c -o ${BinDir}/tmlfn ${Libs})
add_executable(tmlfn tmlfn.c)
target_link_libraries(tmlfn ${Libs})

#
# test LR to AC mapping
#
#--lrac : ${BinDir}/tmlac

#--${BinDir}/tmlac : tmlac.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlac.c -o ${BinDir}/tmlac" ; \
#--	  ${CLD} ${LdFlags} tmlac.c -o ${BinDir}/tmlac ${Libs})

#
# test topological sort
#
#--topsort : ${BinDir}/tmlstp

#--${BinDir}/tmlstp : tmlstp.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlstp.c -o ${BinDir}/tmlstp" ; \
#--	  ${CLD} ${LdFlags} tmlstp.c -o ${BinDir}/tmlstp ${Libs})

#
# test matrix ICG solvers
#
#--icsolv : ${BinDir}/tmlic ${BinDir}/tmlics ${BinDir}/tmlcg

#--${BinDir}/tmlic : tmlic.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlic.c -o ${BinDir}/tmlic" ; \
#--	  ${CLD} ${LdFlags} tmlic.c -o ${BinDir}/tmlic ${Libs})
add_executable(tmlic tmlic.c)
target_link_libraries(tmlic ${Libs})

#--${BinDir}/tmlics : tmlics.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlics.c -o ${BinDir}/tmlics" ; \
#--	  ${CLD} ${LdFlags} tmlics.c -o ${BinDir}/tmlics ${Libs})

#
# test sparse linear CG solvers
#
#--cgsolv : ${BinDir}/tmlcg

#--${BinDir}/tmlcg : tmlcg.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlcg.c -o ${BinDir}/tmlcg" ; \
#--	  ${CLD} ${LdFlags} tmlcg.c -o ${BinDir}/tmlcg ${Libs})
add_executable(tmlcg tmlcg.c)
target_link_libraries(tmlcg ${Libs})

#
# test SVD solvers
#
#--svdsolv : ${BinDir}/tmlsvd

#--${BinDir}/tmlsvd : tmlsvd.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlsvd.c -o ${BinDir}/tmlsvd" ; \
#--	  ${CLD} ${LdFlags} tmlsvd.c -o ${BinDir}/tmlsvd ${Libs})
add_executable(tmlsvd tmlsvd.c)
target_link_libraries(tmlsvd ${Libs})

#
# test EIGEN systems
#
#--eigen : ${BinDir}/tmlev

#--${BinDir}/tmlev : tmlev.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlev.c -o ${BinDir}/tmlev" ; \
#--	  ${CLD} ${LdFlags} tmlev.c -o ${BinDir}/tmlev ${Libs})
add_executable(tmlev tmlev.c)
target_link_libraries(tmlev ${Libs})

#
# test polygon intersection
#
#--ipoly : ${BinDir}/tmlip

#--${BinDir}/tmlip : tmlip.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlip.c -o ${BinDir}/tmlip" ; \
#--	  ${CLD} ${LdFlags} tmlip.c -o ${BinDir}/tmlip ${Libs})

#
# test interpolation
#
#--interp : ${BinDir}/tmli

#--${BinDir}/tmli : tmli.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmli.c -o ${BinDir}/tmli" ; \
#--	  ${CLD} ${LdFlags} tmli.c -o ${BinDir}/tmli ${Libs})
add_executable(tmli tmli.c)
target_link_libraries(tmli ${Libs})

#
# test ln norm
#
#--lnnorm : ${BinDir}/tmllnnm

#--${BinDir}/tmllnnm : tmllnnm.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmllnnm.c -o ${BinDir}/tmllnnm" ; \
#--	  ${CLD} ${LdFlags} tmllnnm.c -o ${BinDir}/tmllnnm ${Libs})

#
# test checksum routines
#
#--checksum : ${BinDir}/tmlck ${BinDir}/tmlcsm

#--${BinDir}/tmlck : tmlck.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlck.c -o ${BinDir}/tmlck" ; \
#--	  ${CLD} ${LdFlags} tmlck.c -o ${BinDir}/tmlck ${Libs})
add_executable(tmlck tmlck.c)
target_link_libraries(tmlck ${Libs})

#--${BinDir}/tmlcsm : tmlcsm.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlcsm.c -o ${BinDir}/tmlcsm" ; \
#--	  ${CLD} ${LdFlags} tmlcsm.c -o ${BinDir}/tmlcsm ${Libs})

#
# test FPE
#
#--fpe : ${BinDir}/tmlfpe ${BinDir}/tmlnan

#--${BinDir}/tmlfpe : tmlfpe.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlfpe.c -o ${BinDir}/tmlfpe" ; \
#--	  ${CLD} ${LdFlags} tmlfpe.c -o ${BinDir}/tmlfpe ${Libs})
add_executable(tmlfpe tmlfpe.c)
target_link_libraries(tmlfpe ${Libs})

#--${BinDir}/tmlnan : tmlnan.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlnan.c -o ${BinDir}/tmlnan" ; \
#--	  ${CLD} ${LdFlags} tmlnan.c -o ${BinDir}/tmlnan ${Libs})
add_executable(tmlnan tmlnan.c)
target_link_libraries(tmlnan ${Libs})


#
# test performance
#
#--perf : ${BinDir}/tmlpf ${BinDir}/tmlnan

#--${BinDir}/tmlpf : tmlpf.c ${TGTLib}
#--	@(echo "${CCAnnounce} tmlpf.c -o ${BinDir}/tmlpf" ; \
#--	  ${CLD} ${LdFlags} tmlpf.c -o ${BinDir}/tmlpf ${Libs})
add_executable(tmlpf tmlpf.c)
target_link_libraries(tmlpf ${Libs})

#
# link 
#
#--link: ${InstallDep}
#--	pact man3install

#
# install 
#
#--install:
#--	pact link
#--	pact incinstall

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
# Run the PML test suite
#
#--test :
#--	@./mltest


set(Tests
    matrix eigen func interp perf
    fpe nan solv svd csum cgsolv
    # chk++  # XXX - compile only
)

foreach(test ${Tests})
    set(testname "${Package}.${test}")
    add_test(
        NAME ${testname}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mltest ${test}
    #   [CONFIGURATIONS <config>...]
    #   [WORKING_DIRECTORY <dir>])
    )

#    set_property(TEST ${testname} PROPERTY 
#        ENVIRONMENT
#             PSY_Base=${PSY_Base}
#    )

endforeach(test)
