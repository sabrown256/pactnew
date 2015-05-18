#
# CMakeLists.txt for SCORE
#
# Files and Directories
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

set(LibName score)
set(TGTLib TRUE)

#--TGTLib = ${LibDir}/lib${LibName}.a
#--MMLib = ${LibDir}/libpsmm.a
#--OMLib = ${LibDir}/libosmm.so

#--OD = DefaultO

#
# Compiler/Loader Options
#
#--CcFlags = -I. -I${PACTSrcDir} -I${IncDir} ${MDInc}
#--FcFlags = 
#--LdFlags = -L${LibDir} ${LDFLAGS}

#--ACFlags = -avrb ${ACCFlags}

#
# Files to Save for Backup (make save)
#
#--SaveFiles = *.c *.h *.r

#
# Files to remove in directory clean (pact clean)
#
#--CleanFiles  = scf77lr.c
#--RemoveFiles = ${BinDir}/ptime ${BinDir}/ncpu \
#--              ${BinDir}/ps-diff ${BinDir}/mpi-io-wrap \
#--              ${BinDir}/dmnz ${BinDir}/hserve \
#--              ${BinDir}/fcscan

#
# Things upon which install depends (pact install)
#
#--InstallDep = ${TGTLib} ${MMLib} ${OMLib} ${hdrf}
#--BinDep =
#--BinObj =

#
# Required Libraries
#
#--Libs  = ${LSCORE} ${MDLib}
#--MLibs = ${LSCORE} ${LMM} ${MDLib}
#--DLibs = ${LSCORE} ${MDE_Lib} ${MDLib}
#--OLibs = ${LSCORE} -losmm ${MDE_Lib} ${MDLib}

add_custom_target(targets
    COMMAND ${CMAKE_COMMAND} -E echo "Targets for SCORE\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   compre       : make comprehensive unit test programs\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   disk         : make simple utility to measure available disk space\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   dmake        : make the dmake utility\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   dmt          : make and test the dmake utility\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   error        : make test program for error handling routines in SCORE\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   exec         : make test program for SC_exec_server\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   fcscan       : make simple utility to scan containers such as tar files\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   ha           : make tsca and tsctr test programs for hash and alist functionality\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   install      : install SCORE headers and utilities in dev/sys_id\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   io           : make test programs for I/O functionality\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   latency      : make simple utility to measure file system latencies\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   lf           : make tscoff test program for large file access functionality\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   link         : link SCORE utilities for public use\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   memchk       : make the tscmtr and tscmpf memory management testing programs\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   mf           : make tscmf test program for memory mapped file functionality\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   mm           : make memory manager testing and performance measurement utility\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   mmlib        : make archive containing replacements for malloc, etc\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   omlib        : make archive containing overrides for malloc, etc\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   mpitty       : make mpi-io-wrap and sctty for MPI testing\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   omp          : make scompt test program for OMP functionality\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   print        : make tscpr test program for isprint, isspace functions\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   resource     : make tscrs test program for resource usage functionality\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   retrace      : make tscrtr test program for debug/retrace functionality\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   scan         : make test program for lexical scanner functionality\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   score-config : make SCORE memory manager configuration utility\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   sharedlib    : build and install SCORE shared libs in dev/sys_id\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   smp          : make tscthr tscsmp and tscsmpb for SMP parallel testing\n"
    COMMAND ${CMAKE_COMMAND} -E echo "   test         : run the SCORE test suite\n"
    )
#
# Headers Files
#
set(Hdrs  scstd.h scapi.h scunix.h scwin32.h scbgl.h
          scgnu.h scpcc.h schpux.h scirix.h
          sclnx.h scosx.h sccyg.h scbsd.h
          scaix.h scsol.h scbeos.h schurd.h
          score.h score_int.h score_old.h
          scope_hash.h scope_type.h scope_io.h scope_array.h
          scope_dl.h)

set(TGTInc ${Hdrs} sclexan.h scope_mem.h scope_proc.h
         scope_term.h scope_mpi.h scope_make.h scope_psh.h
         scope_mmap.h   #  XXX Not in pre-Make
         scope_exe.h scope_lex.h scope_thread.h )

set(APIs ${IncDir}/bfd.api)

#
# Source Files
#

set(DSrcs scmkdp.c scmkpr.c scar.c sctar.c scfcnr.c)

set(ESrcs scexe.c)

set(MSrcs scmemc.c scmemcl.c scmemcs.c 
          scmemda.c scmemdb.c scmemdc.c scmeme.c
          scmemg.c scmemh.c scmemt.c sctyp.c )

set(PSrcs scerr.c sccntx.c
        schp.c scpman.c schttp.c scinet.c
        scmmap.c sclmmap.c scmmaps.c
        scsysa.c scsysb.c scsysc.c scsysd.c scsyse.c
        scfio.c scbio.c scudl.c scioctl.c scsig.c schsrv.c
        scterm.c scleh.c
        scdl.c sctrace.c scpsh.c )

set(TSrcs scpar.c scthn.c scthe.c scthp.c sctho.c scths.c)

set(W32Srcs sclpw32.c scwin32.c)
set(UnxSrcs sclppsx.c scunix.c scaix.c scbgl.c scpcc.c sccyg.c scbsd.c scbeos.c)

#
# Site Dependent OS Support
#
# XXXX make-def, add conditional here
set(OSSrcs ${UnxSrcs})

set(Srcs ${DSrcs} ${ESrcs} ${MSrcs} ${PSrcs} ${TSrcs} ${OSSrcs}
    scctl.c scapi.c scfnca.c scfncb.c scfnct.c
    scarrd.c scarrs.c
    schash.c sctree.c sctab.c
    scstr.c scrsca.c scrscb.c
    sclexs.c sctrk.c sclog.c scold.c
#    scf77lr.l
    )
       
#--#${LibDep} : ${hdrf} ${HDrs}
#--${LibDep} : ${PACTTmpDir}/gc_type.c ${hdrf} ${TGTInc}
#--${MMDep}  : ${Hdrs}

#
# Generated code rules
#

# generated headers and sources

#--${PACTTmpDir}/gc_type.c :
#--	@(echo "template -gc -o ${PACTTmpDir}/gc_type.c" ; \
#--	  ${CROSS_FE} template -gc -t ${EtcDir}/types.db -o ${PACTTmpDir}/gc_type.c)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gc_type.c
    DEPENDS template
    COMMAND template -gc -t ${EtcDir}/types.db -o ${CMAKE_CURRENT_BINARY_DIR}/gc_type.c
    COMMENT "template gc_type.c"
    )
LIST(APPEND GSrcs ${CMAKE_CURRENT_BINARY_DIR}/gc_type.c)

#--${PACTTmpDir}/gc_type.h :
#--	@(echo "template -gh -o ${PACTTmpDir}/gc_type.h" ; \
#--	  ${BinDir}/template -gh -t ${EtcDir}/types.db -o ${PACTTmpDir}/gc_type.h)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gc_type.h
    COMMAND ${BinDir}/template -gh -t ${EtcDir}/types.db -o ${CMAKE_CURRENT_BINARY_DIR}/gc_type.h
    DEPENDS template
    COMMENT "template gc_type.h"
    )
LIST(APPEND GenInc ${CMAKE_CURRENT_BINARY_DIR}/gc_type.h)

#--${IncDir}/scfnca.h : scfnca.t
#--	@(echo "template scfnca.t -o ${IncDir}/scfnca.h" ;  \
#--	  ${BinDir}/template scfnca.t -o ${IncDir}/scfnca.h)
add_custom_command(
    OUTPUT ${IncDir}/scfnca.h
    DEPENDS template ${CMAKE_CURRENT_SOURCE_DIR}/scfnca.t
    COMMAND ${BinDir}/template ${CMAKE_CURRENT_SOURCE_DIR}/scfnca.t -o ${IncDir}/scfnca.h
    COMMENT "template scfnca.h"
    )
LIST(APPEND hdrf ${IncDir}/scfnca.h)

#--IncAction = @pact generate
set(ExcludeType  -excl omp_lockdes)
#set(LExcludeType ${ExcludeType})
set(do-bproto TRUE)

include(${PACT_MANAGER_DIR}/cmake/pact_targets.cmake)

#--${IncDir}/bfd.api : bfd.def
#--	@echo "import-api bfd.def -o ${IncDir}/bfd.api"
#--	@import-api bfd.def ${MDE_Inc} ${LDPath} -o ${IncDir}/bfd.api
add_custom_command(
    OUTPUT ${IncDir}/bfd.api
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/bfd.def
    COMMAND ${PSY_ScrDir}/import-api ${CMAKE_CURRENT_SOURCE_DIR}/bfd.def ${MDE_Inc} ${LDPath} -o ${IncDir}/bfd.api -basedir ${PSY_Base} -root ${PSY_Root}
    COMMENT "import-api bfd.def -o ${IncDir}/bfd.api"
    )
add_custom_target(import_bfd
    DEPENDS ${IncDir}/bfd.api
   )
add_dependencies(score import_bfd)


#--LibAction = @pact devgen

#--build : ${PACTTmpDir}/gc_type.c ${hdrf} ${APIs} ${TGTInc}
#--	@acc ${ACFlags} -c ${Srcs} -a ${TGTLib}

#
# MMLib 
#
#--${MMLib} : scmemr.c
#--	@echo "${CCAnnounce} -c $<"
#--	@(cd ${PACTTmpDir} ; \
#--          ${CC} -c ${PACTSrcDir}/$< -o scmemr.o ; \
#--          ${AR} ${AROpt} ${MMLib} scmemr.o 2> /dev/null ; \
#--          ${RM} scmemr.o 2> /dev/null )

#--mmlib :
#--	pact ${MMLib}

#
# OMLib 
#
#--${OMLib} : scmemo.c scexe.c
#--	@echo "${CCAnnounce} -c $<"
#--	@(cd ${PACTTmpDir} ; \
#--          ${CC} ${Shared_CC_Flags} -c ${PACTSrcDir}/$< -o scmemo.o ; \
#--	  echo "${Shared_LD_Exe} ${Shared_LD_Flags} -o $@ scmemo.o -L${LibDir} -lscore" ; \
#--	  ${Shared_LD_Exe} ${Shared_LD_Flags} -o $@ scmemo.o ${Shared_LD_Lib} -L${LibDir} -lscore ; \
#--          ${RM} scmemo.o 2> /dev/null )

#--omlib :
#--	pact ${OMLib}

#
# CONFIG
#
#--score-config :
#--	${CLD} -DMM_CONFIG scmemi.c -o ${BinDir}/score-config -lc

#
# FCSCAN
#
#--fcscan : ${BinDir}/fcscan

#--${BinDir}/fcscan : ${TGTLib} scfcsc.c
#--	@(echo "${CCAnnounce} scfcsc.c -o ${BinDir}/fcscan" ; \
#--	  ${CLD} ${LdFlags} scfcsc.c -o ${BinDir}/fcscan ${Libs})
add_executable(fcscan scfcsc.c)
target_link_libraries(fcscan score -lm)

#
# PTIME
#
#--${BinDir}/ptime : ${TGTLib} sctime.c
#--	@(echo "${CCAnnounce} sctime.c -o ${BinDir}/ptime" ; \
#--	  ${CLD} ${LdFlags} sctime.c -o ${BinDir}/ptime ${Libs})
add_executable(ptime sctime.c)
target_link_libraries(ptime score -lm)

#
# NCPU
#
#--${BinDir}/ncpu : ${TGTLib} scncpu.c
#--	@(echo "${CCAnnounce} scncpu.c -o ${BinDir}/ncpu" ; \
#--	  ${CLD} ${LdFlags} scncpu.c -o ${BinDir}/ncpu ${Libs})
add_executable(ncpu scncpu.c)
target_link_libraries(ncpu score -lm)

#
# LATENCY
#
#--${BinDir}/latency : ${TGTLib} sclatnt.c
#--	@(echo "${CCAnnounce} sclatnt.c -o ${BinDir}/latency" ; \
#--	  ${CLD} ${LdFlags} sclatnt.c -o ${BinDir}/latency ${Libs})

#--latency : ${BinDir}/latency
add_executable(latency sclatnt.c)
target_link_libraries(latency score -lm)

#
# PS-DIFF
#
#--${BinDir}/ps-diff : ${TGTLib} scpsd.c
#--	@(echo "${CCAnnounce} scpsd.c -o ${BinDir}/ps-diff" ; \
#--	  ${CLD} ${LdFlags} scpsd.c -o ${BinDir}/ps-diff ${Libs})
add_executable(ps-diff scpsd.c)
target_link_libraries(ps-diff score -lm)

#
# DMNZ
#
#--${BinDir}/dmnz : ${TGTLib} scdmnz.c
#--	@(echo "${CCAnnounce} scdmnz.c -o ${BinDir}/dmnz" ; \
#--	  ${CLD} ${LdFlags} scdmnz.c -o ${BinDir}/dmnz ${Libs})
add_executable(dmnz scdmnz.c)
target_link_libraries(dmnz score -lm)

#
# PROPR
#
#--${BinDir}/propr : ${TGTLib} scprpr.c
#--	@(echo "${CCAnnounce} scprpr.c -o ${BinDir}/propr" ; \
#--	  ${CLD} ${LdFlags} scprpr.c -o ${BinDir}/propr ${Libs})
add_executable(propr scprpr.c)
target_link_libraries(propr score -lm)

#
# HSERVE
#
#--${BinDir}/hserve : ${TGTLib} schsru.c
#--	@(echo "${CCAnnounce} schsru.c -o ${BinDir}/hserve" ; \
#--	  ${CLD} ${LdFlags} schsru.c -o ${BinDir}/hserve ${Libs})
add_executable(hserve schsru)
target_link_libraries(hserve score -lm)

#
# DSK-SPACE
#
#--disk : ${BinDir}/dspace

#--${BinDir}/dspace : scdsk.c ${TGTLib}
#--	@(echo "${CCAnnounce} scdsk.c -o ${BinDir}/dspace" ; \
#--	  ${CLD} ${LdFlags} scdsk.c -o ${BinDir}/dspace ${Libs})
add_executable(dspace scdsk.c)
target_link_libraries(dspace score -lm)

#
# ERROR
#
#--error : ${BinDir}/tscer

#--${BinDir}/tscer : ${TGTLib} tscer.c
#--	${CLD} ${LdFlags} tscer.c -o ${BinDir}/tscer ${Libs}
#--#	${CCCfe} ${LdFlags} tscer.c -o ${BinDir}/tscer ${Libs}
add_executable(tscer tscer.c)
target_link_libraries(tscer score -lm)

#
# EXEC SERVER
#

#--exec : ${BinDir}/tscex ${BinDir}/tscpgrp

#--${BinDir}/tscex : ${TGTLib} tscex.c
#--	${CCCfe} ${LdFlags} tscex.c -o ${BinDir}/tscex ${Libs}

#--${BinDir}/tscpgrp : ${TGTLib} tscpgrp.c
#--	${CCCfe} ${LdFlags} tscpgrp.c -o ${BinDir}/tscpgrp ${Libs}
add_executable(tscpgrp tscpgrp.c)
target_link_libraries(tscpgrp score -lm)

#
# MPI
#
#--mpitty : ${BinDir}/sctty ${BinDir}/mpi-io-wrap

#--${BinDir}/mpi-io-wrap : ${TGTLib} scwrap.c
#--	@(echo "${CCAnnounce} scwrap.c -o ${BinDir}/mpi-io-wrap" ; \
#--	  make-wrap ${CCCfg} ${LdFlags} scwrap.c -o ${BinDir}/mpi-io-wrap ${Libs})
add_executable(mpi-io-wrap scwrap.c)
target_link_libraries(mpi-io-wrap score -lm)

#--${BinDir}/sctty : ${TGTLib} sctty.c
#--	@(echo "${CCAnnounce} sctty.c -o ${BinDir}/sctty" ; \
#--	  ${CC} ${LdFlags} sctty.c -o ${BinDir}/sctty ${Libs} ${DPLib})
add_executable(sctty sctty.c)
target_link_libraries(sctty score -lm)

#
# shared memory parallel test
#
#--smp : ${BinDir}/tscthr ${BinDir}/tsctl ${BinDir}/tscsmp ${BinDir}/tscsmpb

#--${BinDir}/tscthr : tscthr.c ${TGTLib}
#--	@(echo "${CCAnnounce} tscthr.c -o ${BinDir}/tscthr" ; \
#--	  ${CLD} ${LdFlags} tscthr.c -o ${BinDir}/tscthr ${Libs})

#--${BinDir}/tsctl : tsctl.c ${TGTLib}
#--	@(echo "${CCAnnounce} tsctl.c -o ${BinDir}/tsctl" ; \
#--	  ${CLD} ${LdFlags} tsctl.c -o ${BinDir}/tsctl ${Libs})

#--${BinDir}/tscsmp : tscsmp.c ${TGTLib}
#--	@(echo "${CCAnnounce} tscsmp.c -o ${BinDir}/tscsmp" ; \
#--	  ${CLD} ${LdFlags} tscsmp.c -o ${BinDir}/tscsmp ${Libs})

#--${BinDir}/tscsmpb : tscsmpb.c ${TGTLib}
#--	@(echo "${CCAnnounce} tscsmpb.c -o ${BinDir}/tscsmpb" ; \
#--	  ${CLD} ${LdFlags} tscsmpb.c -o ${BinDir}/tscsmpb ${Libs})

#
# memory management test
#
#--mm : ${BinDir}/scmm

#--${BinDir}/scmm : scmm.c ${TGTLib}
#--	@(echo "${CCAnnounce} scmm.c -o ${BinDir}/scmm" ; \
#--	  ${CLD} ${LdFlags} scmm.c -o ${BinDir}/scmm ${Libs})
#
# memory diagnostic test
#
#--memchk : ${MMLib} ${BinDir}/tscmtr

#--${BinDir}/tscmtr : tscmtr.c ${TGTLib}
#--	@(echo "${CCAnnounce} tscmtr.c -o ${BinDir}/tscmtr" ; \
#--	  ${CLD} ${LdFlags} tscmtr.c -o ${BinDir}/tscmtr ${MLibs})

#--memprf : ${MMLib} ${BinDir}/tscmpf

#--${BinDir}/tscmpf : tscmpf.c ${TGTLib}
#--	@(echo "${CCAnnounce} tscmpf.c -o ${BinDir}/tscmpf" ; \
#--	  ${CLD} ${LdFlags} tscmpf.c -o ${BinDir}/tscmpf ${Libs})

#--memovr : ${OMLib} ${BinDir}/tscmo

#--${BinDir}/tscmo : ${TGTLib} tscmo.c
#--	@(echo "${CCAnnounce} tscmo.c -o ${BinDir}/tscmo" ; \
#--	  ${CLD} ${LdFlags} tscmo.c -o ${BinDir}/tscmo ${OLibs})

#
# hash/alist test
#
#--ha : ${BinDir}/tsca ${BinDir}/tsctr

#--${BinDir}/tsca : ${TGTLib} tsca.c
#--	@(echo "${CCAnnounce} tsca.c -o ${BinDir}/tsca" ; \
#--	  ${CLD} ${LdFlags} tsca.c -o ${BinDir}/tsca ${Libs})

#--${BinDir}/tsctr : ${TGTLib} tsctr.c
#--	@(echo "${CCAnnounce} tsctr.c -o ${BinDir}/tsctr" ; \
#--	  ${CLD} ${LdFlags} tsctr.c -o ${BinDir}/tsctr ${Libs})

#
# resource usage test
#
#--resource : ${BinDir}/tscrs

#--${BinDir}/tscrs : ${TGTLib} tscrs.c
#--	@(echo "${CCAnnounce} tscrs.c -o ${BinDir}/tscrs" ; \
#--	  ${CLD} ${LdFlags} tscrs.c -o ${BinDir}/tscrs ${Libs})

#
# io file test
#
#--io : ${BinDir}/tscio ${BinDir}/tscbf

#--${BinDir}/tscio : tscio.c ${TGTLib}
#--	@(echo "${CCAnnounce} tscio.c -o ${BinDir}/tscio" ; \
#--	  ${CLD} ${LdFlags} tscio.c -o ${BinDir}/tscio ${Libs})

#--${BinDir}/tscbf : tscbf.c ${TGTLib}
#--	@(echo "${CCAnnounce} tscbf.c -o ${BinDir}/tscbf" ; \
#--	  ${CLD} ${LdFlags} tscbf.c -o ${BinDir}/tscbf ${Libs})

#
# mapped file test
#
#--mf : ${BinDir}/tscmf

#--${BinDir}/tscmf : tscmf.c ${TGTLib}
#--	@(echo "${CCAnnounce} tscmf.c -o ${BinDir}/tscmf" ; \
#--	  ${CLD} ${LdFlags} tscmf.c -o ${BinDir}/tscmf ${Libs})

#
# isprint test
#
#--print : ${BinDir}/tscpr

#--${BinDir}/tscpr : tscpr.c ${TGTLib}
#--	@(echo "${CCAnnounce} tscpr.c -o ${BinDir}/tscpr" ; \
#--	  ${CLD} ${LdFlags} tscpr.c -o ${BinDir}/tscpr ${Libs})

#
# retrace test
#

#--retrace : ${BinDir}/tscrtr

#--${BinDir}/tscrtr : ${TGTLib} tscrtr.c
#--	@(echo "${CCAnnounce} tscrtr.c -o ${BinDir}/tscrtr" ; \
#--	  ${CLD} ${LdFlags} tscrtr.c -o ${BinDir}/tscrtr ${DLibs})

#
# large file types
#
#--lf : ${BinDir}/tscoff

#--${BinDir}/tscoff : ${TGTLib} tscoff.c
#--	@(echo "${CCAnnounce} tscoff.c -o ${BinDir}/tscoff" ; \
#--	  ${CLD} ${LdFlags} tscoff.c -o ${BinDir}/tscoff ${Libs})

#
# http reader
#
#--web : ${BinDir}/scweb

#--${BinDir}/scweb : ${TGTLib} scweb.c
#--	@(echo "${CCAnnounce} scweb.c -o ${BinDir}/scweb" ; \
#--	  ${CLD} ${LdFlags} scweb.c -o ${BinDir}/scweb ${Libs})

#
# memory manager overhead
#
#--mmov : ${BinDir}/tscmov

#--${BinDir}/tscmov : ${TGTLib} tscmov.c
#--	@(echo "${CCAnnounce} tscmov.c -o ${BinDir}/tscmov" ; \
#--	  ${CLD} ${LdFlags} tscmov.c -o ${BinDir}/tscmov ${Libs})

#
# type manager
#
#--type : ${BinDir}/tsctyp

#--${BinDir}/tsctyp : ${TGTLib} tsctyp.c
#--	@(echo "${CCAnnounce} tsctyp.c -o ${BinDir}/tsctyp" ; \
#--	  ${CLD} ${LdFlags} tsctyp.c -o ${BinDir}/tsctyp ${Libs})

#
# comprehensive test suite
#
#--compre : ${BinDir}/tscstr ${BinDir}/tscctl ${BinDir}/tscmemc \
#--         ${BinDir}/interact ${BinDir}/tscvs ${BinDir}/tscar \
#--         ${BinDir}/tscdl

#--${BinDir}/tscstr : ${TGTLib} tscstr.c
#--	@(echo "${CCAnnounce} tscstr.c -o ${BinDir}/tscstr" ; \
#--	  ${CLD} ${LdFlags} tscstr.c -o ${BinDir}/tscstr ${Libs})

#--${BinDir}/tscctl : ${TGTLib} tscctl.c
#--	@(echo "${CCAnnounce} tscctl.c -o ${BinDir}/tscctl" ; \
#--	  ${CLD} ${LdFlags} tscctl.c -o ${BinDir}/tscctl ${Libs})

#--${BinDir}/tscmemc : ${TGTLib} tscmemc.c
#--	@(echo "${CCAnnounce} tscmemc.c -o ${BinDir}/tscmemc" ; \
#--	  ${CLD} ${LdFlags} tscmemc.c -o ${BinDir}/tscmemc ${Libs})

#--${BinDir}/interact : ${TGTLib} interact.c
#--	@(echo "${CCAnnounce} interact.c -o ${BinDir}/interact" ; \
#--	  ${CLD} ${LdFlags} interact.c -o ${BinDir}/interact ${Libs})

#--${BinDir}/tscvs : ${TGTLib} tscvs.c
#--	@(echo "${CCAnnounce} tscvs.c -o ${BinDir}/tscvs" ; \
#--	  ${CLD} ${LdFlags} tscvs.c -o ${BinDir}/tscvs ${Libs})

#--${BinDir}/dmake : ${TGTLib} scmkut.c
#--	@(echo "${CCAnnounce} ${LdFlags} scmkut.c -o ${BinDir}/dmake" ; \
#--	  ${CLD} -I../psh ${LdFlags} scmkut.c -o ${BinDir}/dmake ${Libs})

#--${BinDir}/tscar : ${TGTLib} tscar.c
#--	@(echo "${CCAnnounce} tscar.c -o ${BinDir}/tscar" ; \
#--	  ${CLD} ${LdFlags} tscar.c -o ${BinDir}/tscar ${Libs})

#--${BinDir}/tscdl : ${TGTLib} tscdl.c
#--	@(echo "${CCAnnounce} tscdl.c -o ${BinDir}/tscdl" ; \
#--	  ${CLD} ${LdFlags} tscdl.c -o ${BinDir}/tscdl ${DLibs})

#--foo : ${TGTLib} foo.c
#--	${CC} -c foo.c -o foo.o
#--	${CLD} ${LdFlags} foo.o -o foo ${Libs}

#--example : ${TGTLib} example.c
#--	@(echo "${CCAnnounce} example.c -o example" ; \
#--	  ${CLD} ${LdFlags} example.c -o example ${Libs})

#
# dmake - build and test dmake
#       - this is a special target that you cannot build with dmake
#       - it must be built with smake
#

#--dmake :
#--	${PSY_ScrDir}/bproto ${LExcludeType}
#--	@smake build
#--	@acc ${ACFlags} ${LdFlags} scmkut.c -o ${BinDir}/dmake ${Libs}

#dmake :
#	@echo "import-api bfd.def -o ${IncDir}/bfd.api"
#	@import-api bfd.def ${MDE_Inc} ${LDPath} -o ${IncDir}/bfd.api
#	@acc ${ACFlags} -c ${Srcs} -a ${TGTLib}
#	@acc ${ACFlags} ${LdFlags} scmkut.c -o ${BinDir}/dmake ${Libs}

#--dmt : ${hdrf}
#--	${BinDir}/smake dmake
#--	sctest mk

#
# link 
#
#--link: ${InstallDep} ${BinDir}/ps-diff ${BinDir}/ptime \
#--      ${BinDir}/ncpu ${BinDir}/propr ${BinDir}/dmnz \
#--      ${BinDir}/hserve ${BinDir}/latency \
#--      ${BinDir}/mpi-io-wrap ${BinDir}/fcscan
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
# Run the SCORE test suite
#
#--test :
#--	@./sctest

