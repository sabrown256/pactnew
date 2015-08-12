#
# CMakeLists.txt for PGS
#
# Files and Directories
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#
set(Package pgs)
set(LibName pgs)
set(TGTLib  pgs)

#--TGTLib = ${LibDir}/lib${LibName}.a
#--TGTInc = ${Hdrs} ${XHdr}

#--OD = DefaultG

#
# Compiler/Loader Options
#
#--CcFlags = -I${IncDir} ${MDInc} ${MDGInc}
#--LdFlags = -L${LibDir} ${LDFLAGS}

#--ACFlags = -avrb ${AODC} ${CFLAGS} ${Shared_CC_Flags} -I. ${CcFlags}
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

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
#--InstallDep = ${TGTLib}
#--BinDep = ${TGTLib}
#--BinObj =


#--targets :
#--	@echo "Targets for PGS"
#--	@echo "   axis      : make axis test"
#--	@echo "   colormap  : make show colormap utility"
#--	@echo "   contour   : make contour test"
#--	@echo "   Cstd      : make basic drawing test"
#--	@echo "   curve     : make curve drawing test"
#--	@echo "   event     : make event handling test"
#--	@echo "   fillpoly  : make fill-poly test"
#--	@echo "   Fstd      : make fortran API test"
#--	@echo "   grotrian  : make grotrian test"
#--	@echo "   image     : make image test"
#--	@echo "   install   : install PGS headers and utilities in dev/sys_id"
#--	@echo "   line      : make line drawing test"
#--	@echo "   link      : link PGS utilities for public use"
#--	@echo "   mpeg      : make MPEG test"
#--	@echo "   polyseg   : make poly segment test"
#--	@echo "   raster    : make raster test"
#--	@echo "   scontour  : make simple contour test"
#--	@echo "   sharedlib : build and install PGS shared libs in dev/sys_id"
#--	@echo "   surface   : make surface test"
#--	@echo "   test      : run the PGS test suite"
#--	@echo "   text      : make text handling test"
#--	@echo "   trans     : make tranformation test"
#--	@echo "   vector    : make vector test"

#
# Headers Files
#
set(Hdrs pgs.h pgs_int.h pgs_old.h scope_cgm.h scope_raster.h)
set(MpgHdr scope_mpeg.h)
set(XHdr   scope_x11.h)

set(TGTInc ${Hdrs} ${XHdr})

#
# Object Files
#
set(PACTTmpDir  ${CMAKE_CURRENT_BINARY_DIR})

set(gcs ${PACTTmpDir}/gc-pgs.c)

set(SPS  gsdv_ps.c gspr_ps.c)
set(SCGM gsdv_cgm.c gspr_cgm.c)
set(SQD  gsdv_qd.c gspr_qd.c)
set(SX   gsdv_X.c gspr_X.c)
set(SOGL gsdv_GL.c gspr_GL.c)
set(SJPG gsdv_jpg.c)
set(SMPG gsdv_mpg.c gspr_mpg.c gsmpeg.c)
set(SPNG gsdv_png.c)

# XXX - configuration decides which backends to use
set(GRDevicesS  ${SPS} ${SCGM} ${SMPG} ${SPNG} ${SJPG} ${SOGL} ${SX})

##############################

set(Srcs
    gsdv.c gspr.c gsacc.c gsvsp.c gsdv_im.c gsevmn.c
    gsdv_rst.c gspr_rst.c gsdlr.c gsaux.c gsmm.c gsattr.c
    gshigh.c gsrend.c gstxt.c gstx_X.c gsiob.c gsiod.c gsrwi.c
    gsdplt.c gsaxis.c gsscale.c gsdlin.c gscrv.c gscntr.c
    gsncnt.c gsvect.c gsimag.c gssurf.c gsdvb.c gspolf.c
    gsgrot.c gshls.c gsscat.c gsclr.c gsold.c gssfnt.c
    ${SPS} ${SCGM} ${SMPG} ${GRDevicesS}
    ${gcs}
)


#
# Required Libraries
#
#--Libs = ${LPGS} ${MDGLib} ${LPDB} ${LPML} ${LSCORE} ${MDLib}
set(Libs pgs ${MDGLib} pdb pml score ${MDLib})

#--gco = gc-pgs.o

#--GObjsA = gsaux.o gsaxis.o gsscale.o gsdlin.o gscrv.o gscntr.o gsvect.o gsimag.o gssurf.o gshls.o gsacc.o
#--GObjsB = gsdvb.o gspolf.o gsdplt.o gsgrot.o gsscat.o gsiob.o gsiod.o gsrwi.o
#--GObjsC = gsmm.o gsattr.o gstxt.o gsclr.o gshigh.o gsdv.o gspr.o gsvsp.o gsrend.o
#--GObjsD = gsdv_im.o gsdv_rst.o gspr_rst.o gsdlr.o gsold.o gssfnt.o
#--GObjsE = gsfia.o gsfib.o
#--Objs = ${GObjsA} ${GObjsB} ${GObjsC} ${GObjsD} ${GObjsE} \
#--       ${gco} ${GRDevicesO}

#--OPS  = gsdv_ps.o gspr_ps.o
#--OCGM = gsdv_cgm.o gspr_cgm.o
#--OQD  = gsdv_qd.o gspr_qd.o
#--OX   = gsdv_X.o gspr_X.o
#--OOGL = gsdv_GL.o gspr_GL.o
#--OJPG = gsdv_jpg.o
#--OMPG = gsdv_mpg.o gspr_mpg.o gsmpeg.o
#--OPNG = gsdv_png.o

#--${Objs} : ${Hdrs}
#--${OX} : ${XHdr}
#--${OOGL} : ${XHdr}

#--gca = ${TGTLib}(gc-pgs.o)

#--ArObjs = ${TGTLib}(gsdv.o) ${TGTLib}(gspr.o) \
#--         ${TGTLib}(gsacc.o) ${TGTLib}(gsvsp.o) \
#--         ${TGTLib}(gsdv_im.o) ${TGTLib}(gsevmn.o) \
#--         ${TGTLib}(gsdv_rst.o) ${TGTLib}(gspr_rst.o) ${TGTLib}(gsdlr.o) \
#--	 ${TGTLib}(gsaux.o) ${TGTLib}(gsmm.o) ${TGTLib}(gsattr.o) \
#--	 ${TGTLib}(gshigh.o) ${TGTLib}(gsrend.o) \
#--         ${TGTLib}(gstxt.o) ${TGTLib}(gstx_X.o) \
#--         ${TGTLib}(gsiob.o) ${TGTLib}(gsiod.o) ${TGTLib}(gsrwi.o) \
#--	 ${TGTLib}(gsdplt.o) ${TGTLib}(gsaxis.o) ${TGTLib}(gsscale.o) \
#--         ${TGTLib}(gsdlin.o) ${TGTLib}(gscrv.o) ${TGTLib}(gscntr.o) \
#--         ${TGTLib}(gsncnt.o) ${TGTLib}(gsvect.o) \
#--         ${TGTLib}(gsimag.o) ${TGTLib}(gssurf.o) \
#--         ${TGTLib}(gsdvb.o) ${TGTLib}(gspolf.o) \
#--         ${TGTLib}(gsgrot.o) ${TGTLib}(gshls.o) ${TGTLib}(gsscat.o) \
#--         ${TGTLib}(gsclr.o) ${TGTLib}(gsold.o) ${TGTLib}(gssfnt.o) \
#--         ${gca}

#--ArPS  = ${TGTLib}(gsdv_ps.o) ${TGTLib}(gspr_ps.o)
#--ArCGM = ${TGTLib}(gsdv_cgm.o) ${TGTLib}(gspr_cgm.o)
#--ArQD  = ${TGTLib}(gsdv_qd.o) ${TGTLib}(gspr_qd.o)
#--ArX   = ${TGTLib}(gsdv_X.o) ${TGTLib}(gspr_X.o)
#--ArOGL = ${TGTLib}(gsdv_GL.o) ${TGTLib}(gspr_GL.o)
#--ArJPG = ${TGTLib}(gsdv_jpg.o)
#--ArMPG = ${TGTLib}(gsdv_mpg.o) ${TGTLib}(gspr_mpg.o) ${TGTLib}(gsmpeg.o)
#--ArPNG = ${TGTLib}(gsdv_png.o)

#--${LibDep} : ${hdrf} ${Hdrs} ${GRDevicesO}

#--${ArMPG} : ${MpgHdr}
#--${ArX}   : ${XHdr}
#--${ArOGL} : ${XHdr}

#
# Generated code rules
#

# headers

#--${IncDir}/gsimag.h : gsimag.t
#--	@(echo "template gsimag.t -o ${IncDir}/gsimag.h" ;  \
#--	  ${BinDir}/template gsimag.t -o ${IncDir}/gsimag.h)
add_custom_command(
    OUTPUT ${IncDir}/gsimag.h
    DEPENDS template gsimag.t
    COMMAND template
        ${CMAKE_CURRENT_SOURCE_DIR}/gsimag.t
        -t ${DB_TYPES}
        -o ${IncDir}/gsimag.h
    COMMENT "template gsimag.t -o ${IncDir}/gsimag.h"
    )
LIST(APPEND hdrf ${IncDir}/gsimag.h)

#--GenInc = ${PACTTmpDir}/gc-pgs.h
LIST(APPEND GenInc ${CMAKE_CURRENT_BINARY_DIR}/gc-pgs.h)

#--${IncDir}/pgs_gen.h : ${GenInc}
#--	@echo "build pgs_gen.h"
#--	@rm -f ${IncDir}/pgs_gen.h
#--	@cat ${GenInc} > ${IncDir}/pgs_gen.h

#set(hdrf ${IncDir}/gsimag.h ${IncDir}/pgs_gen.h)

#Command: mv gf-pgs.c ../../../fortran/z-lnx-2.12-o/obj
#Command: mv gm-pgs.f ../../../fortran/z-lnx-2.12-o/obj
#Command: mv gs-pgs.c ../../../sx/z-lnx-2.12-o/obj
#Command: mv gs-pgs.h ../../../sx/z-lnx-2.12-o/obj
#Command: mv gp-pgs.c ../../../python/z-lnx-2.12-o/obj
#Command: mv gp-pgs.h ../../../python/z-lnx-2.12-o/obj
#Command: mv gh-pgs.html ../../../doc

#--IncAction = @pact generate
set(do-bproto TRUE)

include(${PACT_MANAGER_DIR}/cmake/pact_targets.cmake)

#--LibAction = @pact devgen

#--build : ${hdrf}
#--	@acc ${ACFlags} -c ${Srcs} -a ${TGTLib}

#
# link 
#
#--link: ${InstallDep}
#--	pact query
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
# Basic drawing test
#
#--Cstd : ${BinDir}/tgsa

#--${BinDir}/tgsa : tgsa.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsa.c -o ${BinDir}/tgsa" ; \
#--	  ${CLD} ${LdFlags} tgsa.c -o ${BinDir}/tgsa ${Libs})
add_executable(tgsa tgsa.c)
target_link_libraries(tgsa ${Libs})

#
# query event utility
#

#--query : ${BinDir}/quev

#--${BinDir}/quev : ${TGTLib} quev.c
#--	@(echo "${CCAnnounce} quev.c -o ${BinDir}/quev" ; \
#--	  ${CLD} ${LdFlags} quev.c -o ${BinDir}/quev ${Libs})

#
# event handling test
#

#--event : ${BinDir}/tgsev

#--${BinDir}/tgsev : ${TGTLib} tgsev.c
#--	@(echo "${CCAnnounce} tgsev.c -o ${BinDir}/tgsev" ; \
#--	  ${CLD} ${LdFlags} tgsev.c -o ${BinDir}/tgsev ${Libs})

#
# line drawing test
#
#--line : ${BinDir}/tgscl

#--${BinDir}/tgscl : tgscl.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgscl.c -o ${BinDir}/tgscl" ; \
#--	  ${CLD} ${LdFlags} tgscl.c -o ${BinDir}/tgscl ${Libs})

#
# poly segment test
#
#--polyseg : ${BinDir}/tgsml

#--${BinDir}/tgsml : tgsml.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsml.c -o ${BinDir}/tgsml" ; \
#--	  ${CLD} ${LdFlags} tgsml.c -o ${BinDir}/tgsml ${Libs})

#
# text handling test
#
#--text : ${BinDir}/tgstx

#--${BinDir}/tgstx : tgstx.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgstx.c -o ${BinDir}/tgstx" ; \
#--	  ${CLD} ${LdFlags} tgstx.c -o ${BinDir}/tgstx ${Libs})

#
# transformation test
#
#--trans : ${BinDir}/tgstr

#--${BinDir}/tgstr : tgstr.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgstr.c -o ${BinDir}/tgstr" ; \
#--	  ${CLD} ${LdFlags} tgstr.c -o ${BinDir}/tgstr ${Libs})
add_executable(tgstr tgstr.c)
target_link_libraries(tgstr ${Libs})

#
# curve drawing test
#
#--curve : ${BinDir}/tgsln

#--${BinDir}/tgsln : tgsln.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsln.c -o ${BinDir}/tgsln" ; \
#--	  ${CLD} ${LdFlags} tgsln.c -o ${BinDir}/tgsln ${Libs})

#
# surface test
#
#--surface : ${BinDir}/tgssf

#--${BinDir}/tgssf : tgssf.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgssf.c -o ${BinDir}/tgssf" ; \
#--	  ${CLD} ${LdFlags} tgssf.c -o ${BinDir}/tgssf ${Libs})

#
# fill-poly test
#
#--fillpoly: ${BinDir}/tgsfp

#--${BinDir}/tgsfp : tgsfp.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsfp.c -o ${BinDir}/tgsfp" ; \
#--	  ${CLD} ${LdFlags} tgsfp.c -o ${BinDir}/tgsfp ${Libs})

#
# contour test
#
#--contour : ${BinDir}/tgscn

#--${BinDir}/tgscn : tgscn.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgscn.c -o ${BinDir}/tgscn" ; \
#--	  ${CLD} ${LdFlags} tgscn.c -o ${BinDir}/tgscn ${Libs})

#
# simple contour test
#
#--scontour : ${BinDir}/tgscnb

#--${BinDir}/tgscnb : tgscnb.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgscnb.c -o ${BinDir}/tgscnb" ; \
#--	  ${CLD} ${LdFlags} tgscnb.c -o ${BinDir}/tgscnb ${Libs})

#
# vector test
#
#--vector : ${BinDir}/tgsvc

#--${BinDir}/tgsvc : tgsvc.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsvc.c -o ${BinDir}/tgsvc" ; \
#--	  ${CLD} ${LdFlags} tgsvc.c -o ${BinDir}/tgsvc ${Libs})

#
# image test
#
#--image : ${BinDir}/tgsim

#--${BinDir}/tgsim : tgsim.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsim.c -o ${BinDir}/tgsim" ; \
#--	  ${CLD} ${LdFlags} tgsim.c -o ${BinDir}/tgsim ${Libs})

#
# show colormap
#
#--colormap : ${BinDir}/tgsscm

#--${BinDir}/tgsscm : tgsscm.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsscm.c -o ${BinDir}/tgsscm" ; \
#--	  ${CLD} ${LdFlags} tgsscm.c -o ${BinDir}/tgsscm ${Libs})

#--${BinDir}/tgscm : tgscm.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgscm.c -o ${BinDir}/tgscm" ; \
#--	  ${CLD} ${LdFlags} tgscm.c -o ${BinDir}/tgscm ${Libs})

#
# Raster test
#
#--raster : ${BinDir}/tgsrst

#--${BinDir}/tgsrst : tgsrst.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsrst.c -o ${BinDir}/tgsrst" ; \
#--	  ${CLD} ${LdFlags} tgsrst.c -o ${BinDir}/tgsrst ${Libs})

#
# axis test
#
#--axis : ${BinDir}/tgslax

#--${BinDir}/tgslax : tgslax.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgslax.c -o ${BinDir}/tgslax" ; \
#--	  ${CLD} ${LdFlags} tgslax.c -o ${BinDir}/tgslax ${Libs})
add_executable(tgslax tgslax.c)
target_link_libraries(tgslax ${Libs})

#
# grotrian test
#
#--grotrian : ${BinDir}/tgsgrt

#--${BinDir}/tgsgrt : tgsgrt.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsgrt.c -o ${BinDir}/tgsgrt" ; \
#--	  ${CLD} ${LdFlags} tgsgrt.c -o ${BinDir}/tgsgrt ${Libs})
add_executable(tgsgrt tgsgrt.c)
target_link_libraries(tgsgrt ${Libs})

#
# inform
#
#--inform :
#--	@rm -f required.objs
#--	@echo ${GObjsA} ${GObjsB} ${GObjsC} ${GObjsD} ${OPS} ${OCGM} ${ORST} > required.objs

#
# Run the PGS test suite
#
#--test :
#--	@./gstest

set(Tests trans axis rendlr rendac grot mpeg smp)
foreach(test ${Tests})
    set(testname "${Package}.${test}")
    add_test(
        NAME ${testname}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gstest ${test}
    )
endforeach(test)


#
# MPEG test
#
#--mpeg : ${BinDir}/tgsmpg1 ${BinDir}/tgsmpg2

#--${BinDir}/tgsmpg1 : tgsmpg1.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsmpg1.c -o ${BinDir}/tgsmpg1" ; \
#--	  ${CLD} ${LdFlags} tgsmpg1.c -o ${BinDir}/tgsmpg1 ${Libs})
add_executable(tgsmpg1 tgsmpg1.c)
target_link_libraries(tgsmpg1 ${Libs})

#--${BinDir}/tgsmpg2 : tgsmpg2.c ${TGTLib}
#--	@(echo "${CCAnnounce} tgsmpg2.c -o ${BinDir}/tgsmpg2" ; \
#--	  ${CLD} ${LdFlags} tgsmpg2.c -o ${BinDir}/tgsmpg2 ${Libs})
add_executable(tgsmpg2 tgsmpg2.c)
target_link_libraries(tgsmpg2 ${Libs})

