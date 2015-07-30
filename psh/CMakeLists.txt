#
# CMakeLists.txt for psh
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#
set(Package psh)
set(LibName psh)

set(POSIXHdrs posix.h posix-unix.h posix-msw.h posix-beos.h)
set(ANSIHdrs  ansic.h)
set(NETWHdrs  network.h network-unix.h network-msw.h network-beos.h)
set(StdHdrs   ${POSIXHdrs} ${ANSIHdrs} ${NETWHdrs})

set(LibFiles libpsh.c libtyp.c libasync.c libpgrp.c libeval.c libtime.c
           libfifo.c libsock.c libdb.c libsrv.c libhash.c libstack.c
           libfio.c libio.c libsubst.c liblog.c libinfo.c)
set(IncFiles common.h nonstd.h ${LibFiles})

#--InstallDep = ${BinDir}/pco ${BinDir}/do-net ${BinDir}/do-run \
#--             ${BinDir}/atdbg ${BinDir}/pcsh ${BinDir}/rpath \
#--             ${BinDir}/gexec ${BinDir}/perdb \
#--             ${BinDir}/elide ${BinDir}/blang \
#--             ${BinDir}/smake ${BinDir}/pact-info \
#--             ${BinDir}/osrc

#--#RemoveFiles = ${InstallDep} ${StdHdrs} ${IncFiles}
#--RemoveFiles = ${InstallDep}

#--${InstallDep} : ${StdHdrs} ${IncFiles}

#--#
#--# Generated code rules
#--#

#--# headers

#--hdrf = 

#--IncAction = sinstall ${IncFiles} ${IncDir}/shell


#--pco : ${BinDir}/pco

#--${BinDir}/pco : pco.c libpsh.c
#--	@(echo "${PCAnnounce} pco.c -o ${BinDir}/pco" ; \
#--	  ${PCC} pco.c -o ${BinDir}/pco ${MDLib})
add_executable(pco pco.c)

#--do-net : ${BinDir}/do-net

#--${BinDir}/do-net : do-net.c ${LibFiles}
#--	@(echo "${PCAnnounce} do-net.c -o ${BinDir}/do-net" ; \
#--	  ${PCC} do-net.c -o ${BinDir}/do-net ${MDLib})
add_executable(do-net do-net.c)

#--do-run : ${BinDir}/do-run

#--${BinDir}/do-run : do-run.c ${LibFiles}
#--	@(echo "${PCAnnounce} do-run.c -o ${BinDir}/do-run" ; \
#--	  ${PCC} -DPSY_Prefix=\"${PSY_Prefix}\" do-run.c -o ${BinDir}/do-run ${MDLib})
set(PSY_Prefix z-)
set_property( SOURCE do-run.c PROPERTY
   COMPILE_DEFINITIONS PSY_Prefix="${PSY_Prefix}" )
add_executable(do-run do-run.c)

#--atdbg : ${BinDir}/atdbg

#--${BinDir}/atdbg : atdbg.c libpsh.c
#--	@(echo "${PCAnnounce} atdbg.c -o ${BinDir}/atdbg" ; \
#--	  ${PCC} atdbg.c -o ${BinDir}/atdbg ${MDLib})
add_executable(atdbg atdbg.c)

#--gexec : ${BinDir}/gexec

#--${BinDir}/gexec : gexec.c libpsh.c
#--	@(echo "${PCAnnounce} gexec.c -o ${BinDir}/gexec" ; \
#--	  ${PCC} gexec.c -o ${BinDir}/gexec ${MDLib})
add_executable(gexec gexec.c)

#--pcsh : ${BinDir}/pcsh

#--${BinDir}/pcsh : pcsh.c libpsh.c
#--	@(echo "${PCAnnounce} pcsh.c -o ${BinDir}/pcsh" ; \
#--	  ${PCC} pcsh.c -o ${BinDir}/pcsh)
add_executable(pcsh  pcsh.c)

#--perdb : ${BinDir}/perdb

#--${BinDir}/perdb : perdb.c ${LibFiles}
#--	@(echo "${PCAnnounce} perdb.c -o ${BinDir}/perdb" ; \
#--	  ${PCC} perdb.c -o ${BinDir}/perdb ${MDLib})
add_executable(perdb perdb.c)

#--elide : ${BinDir}/elide

#--${BinDir}/elide : elide.c ${LibFiles}
#--	@(echo "${PCAnnounce} elide.c -o ${BinDir}/elide" ; \
#--	  ${PCC} elide.c -o ${BinDir}/elide ${MDLib})
add_executable(elide elide.c)

#--blang : ${BinDir}/blang

#--BLANG_Src = blang-ba.c blang-doc.c blang-f.c blang-py.c blang-scm.c blang.c

#--${BinDir}/blang : ${BLANG_Src} ${LibFiles}
#--	@(echo "${PCAnnounce} blang.c -o ${BinDir}/blang" ; \
#--	  ${PCC} blang.c -o ${BinDir}/blang)
add_executable(blang blang.c)

#--rpath : ${BinDir}/rpath

#--${BinDir}/rpath : rpath.c libpsh.c
#--	@(echo "${PCAnnounce} rpath.c -o ${BinDir}/rpath" ; \
#--	  ${PCC} rpath.c -o ${BinDir}/rpath)
add_executable(rpath rpath.c)

#--osrc : ${BinDir}/osrc

#--${BinDir}/osrc : osrc.c ${LibFiles}
#--	@(echo "${PCAnnounce} osrc.c -o ${BinDir}/osrc" ; \
#--	  ${PCC} osrc.c -o ${BinDir}/osrc ${MDLib})
add_executable(osrc  osrc.c)

#--pact-info : ${BinDir}/pact-info ${BinDir}/pact-info

#--${BinDir}/pact-info : pact-info.c common.h
#--	@(echo "${PCAnnounce} -DPACT_VERSION=\"${Version}\" pact-info.c -o ${BinDir}/pact-info" ; \
#--	 ${PCC} -DPACT_VERSION=\"${Version}\" pact-info.c -o ${BinDir}/pact-info)
set_property( SOURCE pact-info.c PROPERTY
    COMPILE_DEFINITIONS PACT_VERSION="${PACT_VERSION}" )
add_executable(pact-info pact-info.c common.h)


#--smake : ${BinDir}/pact-info ${BinDir}/smake

#--${BinDir}/smake : smake.c common.h
#--	ccw -DPSY_ID=\"${PSY_ID}\" -DPSY_Prefix=\"${PSY_Prefix}\" -DPACT_VERSION=\"${Version}\" smake.c -o ${BinDir}/smake
add_executable(smake pact-info.c common.h)


#--# tester for strcpy_tok, strcpy_str
#--tpstok : ${BinDir}/tpstok

#--${BinDir}/tpstok : tpstok.c ${LibFiles}
#--	@(echo "${PCAnnounce} tpstok.c -o ${BinDir}/tpstok" ; \
#--	  ${PCC} tpstok.c -o ${BinDir}/tpstok ${MDLib})
add_executable(tpstok tpstok.c)

#--# debugging only - never to be included in InstallDep
#--template : ${BinDir}/template

#--${BinDir}/template : template.c ${LibFiles}
#--	@(echo "${PCAnnounce} template.c -o ${BinDir}/template" ; \
#--	  ccw -DAF_LONG_DOUBLE_IO=${AF_LONG_DOUBLE_IO} -g template.c -o ${BinDir}/template)
add_executable(template template.c)

#--link : ${InstallDep}

#--install :
#--	pact link

#--mkinstall :
#--	${IncAction}

set(TGTInc ${StdHdrs})

include(${PACT_MANAGER_DIR}/cmake/pact_targets.cmake)

#--IncAction = sinstall ${IncFiles} ${IncDir}/shell

# Make a target for each include file
foreach(file ${IncFiles})
    set(infile  ${CMAKE_CURRENT_SOURCE_DIR}/${file})
    set(outfile ${IncDir}/shell/${file})
    add_custom_command(
        OUTPUT ${outfile}
        COMMAND ${CMAKE_COMMAND} -E copy ${infile} ${outfile}
        MAIN_DEPENDENCY ${infile}
        COMMENT "Copy ${file}"
    )
    LIST(APPEND IncFiles_LIST ${outfile})
endforeach(file)

add_custom_target(incinstall_psh_shell
    DEPENDS ${IncFiles_LIST}
    COMMENT "incinstall shell in ${Package}"
)
add_dependencies(incinstall_psh incinstall_psh_shell)

# installed by 'dsys config'
add_custom_target(pact-build-tools
    DEPENDS pco perdb gexec elide blang rpath template
)

#--#
#--# Run the PSH test suite
#--#
#--test :
#--	@./pstest

set(Tests 
# from psh/tests/ext
    ext-job ext-pipe ext-int
    ext-fnc ext-file ext-var ext-fan ext-spec

# list from psh/pstest
    perdb
    do-net elide blang templ
#   info do-net elide blang templ
    regx pcsh dorun verify
#   pco
)

# simplified for testing
#set(Tests 
#    perdb
#)

foreach(test ${Tests})
    set(testname psh.${test})
    add_test(
        NAME ${testname}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/pstest.cmake 
           -base  ${PSY_Base}
           -build ${PSY_Build}
           -root  ${PSY_Root}
           ${test}
    #   [CONFIGURATIONS <config>...]
    #   [WORKING_DIRECTORY <dir>])
    )

    set_property(TEST ${testname} PROPERTY 
        ENVIRONMENT
#             TestScrDir=${PSY_ScrDir}
             TestDir=${CMAKE_CURRENT_BINARY_DIR}/test
#             PSY_TmpDir=.
             PSY_Base=${PSY_Base}
#             PSY_Root=${PSY_Root}
    )

endforeach(test)
