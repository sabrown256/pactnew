###############################################################################
# Copyright (c) 2014, Lawrence Livermore National Security, LLC.
#
# Produced at the Lawrence Livermore National Laboratory
#
# LLNL-CODE-666778
#
# All rights reserved.
#
# This file is part of Conduit.
#
# For details, see https://lc.llnl.gov/conduit/.
#
# Please also read conduit/LICENSE
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the disclaimer below.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the disclaimer (as noted below) in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the name of the LLNS/LLNL nor the names of its contributors may
#   be used to endorse or promote products derived from this software without
#   specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
# LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
###############################################################################

include(CMakeParseArguments)

################################
# Standard Build Layout
################################

# PACT_SOURCE_DIR and PACT_BINARY_DIR created by cmake

set (PACT_MANAGER_DIR ${PACT_SOURCE_DIR}/manager)
set (PSY_MngDir ${PACT_SOURCE_DIR}/manager)
set (PACT_ANALYZE_BINARY_DIR ${PACT_BINARY_DIR}/bin/analyze)
set (PACT_ANALYZE_SOURCE_DIR ${PACT_SOURCE_DIR}/manager/cmake)

set (PSY_Base    ${PACT_SOURCE_DIR})
set (PSY_ScrDir  ${PACT_SOURCE_DIR}/scripts)
set (PSY_Build   ${PACT_BINARY_DIR})
if(NOT PSY_Root)
  set (PSY_Root    ${PACT_BINARY_DIR} CACHE STRING "" FORCE) 
endif()
set (ScmDir  ${PSY_Root}/scheme)
set (BinDir  ${PSY_Root}/bin)
set (LibDir  ${PSY_Root}/lib)
set (IncDir  ${PSY_Root}/include)
set (EtcDir  ${PSY_Root}/etc)
set (Man1Dir ${PSY_Root}/man/man1)
set (Man3Dir ${PSY_Root}/man/man3)
set (LogDir  ${PSY_Root}/log)

##
## Defines the layout of the build directory. Namely,
## it indicates the location where the various header files should go,
## where to store libraries (static or shared), the location of the
## bin directory for all executables and the location for fortran moudules.
##
include_directories(${IncDir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LibDir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LibDir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BinDir})

 ## Set the Fortran module directory
set(CMAKE_Fortran_MODULE_DIRECTORY
    ${IncDir}
    CACHE PATH
    "Directory where all Fortran modules will go in the build tree"
)

## Mark as advanced
#mark_as_advanced(
#     LIBRARY_OUTPUT_PATH
#     EXECUTABLE_OUTPUT_PATH
#     CMAKE_Fortran_MODULE_DIRECTORY
#     )


################################
# Standard CMake Options
################################
#--
#--include(ExternalProject)
#--
#--option(BUILD_TESTING "Builds unit tests" ON)
#--if (BUILD_TESTING)
#--
#--  ## add catch
#--  include_directories(${PROJECT_SOURCE_DIR}/TPL/catch/single_include)
#--
#--  ## add google test
#--  add_subdirectory(${PROJECT_SOURCE_DIR}/TPL/gtest-1.7.0)
#--  set(GTEST_INCLUDES ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR}
#--            CACHE INTERNAL "GoogleTest include directories" FORCE)
#--  set(GTEST_LIBS gtest_main gtest
#--            CACHE INTERNAL "GoogleTest link libraries" FORCE)
#--
#--  enable_testing()
#--
#--endif()
#--
#--################################################################
#--# For convenience, we support directly building uncrustify
#--################################################################
#--option(BUILD_UNCRUSTIFY "Build uncrustify binary" ON)
#--if (BUILD_UNCRUSTIFY)
#--    # use cmake external project
#--    # using the tarball is an easy way to keep .o files from 
#--    # uncrustify's in source build from entering our build tree
#--    #
#--    ExternalProject_Add(uncrustify_build
#--                PREFIX uncrustify_build
#--                URL ${PROJECT_SOURCE_DIR}/TPL/uncrustify-0.61.tar.gz
#--                BUILD_IN_SOURCE 1
#--                CONFIGURE_COMMAND  CC=${CMAKE_C_COMPILER} 
#--                                   CXX=${CMAKE_CXX_COMPILER} 
#--                                   ./configure 
#--                                   --prefix=${PROJECT_BINARY_DIR}/TPL/uncrustify
#--                BUILD_COMMAND ${make} )
#--    # use this copy as our uncrustify exe 
#--    set(UNCRUSTIFY_EXECUTABLE ${PROJECT_BINARY_DIR}/TPL/uncrustify/bin/uncrustify)
#--
#--endif()
#--
#--## Enable ENABLE C++ 11 features
#--option(ENABLE_CXX11 "Enables C++11 features" OFF)
#--if (ENABLE_CXX11)
#--  # define a macro so the code can ifdef accordingly.
#--  add_definitions("-DUSE_CXX11")
#--endif()
#--
#--## Choose static or shared libraries.
#--option(BUILD_SHARED_LIBS "Build shared libraries." OFF)
#--
#--option(ENABLE_WARNINGS "Enable Compiler warnings." OFF)
#--if(ENABLE_WARNINGS)
#--
#--    # set the warning levels we want to abide by
#--    if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
#--        add_definitions(/W2)
#--    else()
#--        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
#--            "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#--            # using clang or gcc
#--            add_definitions(-Wall -Wextra)
#--        endif()
#--    endif()
#--
#--endif()
#--
#--
#--#############################################
#--# Support extra compiler flags and defines
#--#############################################
#--#
#--# We don't try to use this approach for CMake generators that support 
#--# multiple configurations. See: CZ JIRA: ATK-45
#--#
#--if(NOT CMAKE_CONFIGURATION_TYPES)
#--    
#--    ######################################################
#--    # Add define we can use when debug builds are enabled
#--    ######################################################
#--    if(CMAKE_BUILD_TYPE MATCHES Debug)
#--        add_definitions(-DATK_DEBUG)
#--    endif()
#--
#--    ##########################################
#--    # Support Extra Flags for the C compiler.
#--    ##########################################
#--    if(EXTRA_C_FLAGS)
#--        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_C_FLAGS}")
#--    endif()
#--
#--    # Extra Flags for the debug builds with the C compiler.
#--    if(EXTRA_C_FLAGS_DEBUG AND CMAKE_BUILD_TYPE MATCHES Debug)
#--        add_compile_options("${EXTRA_C_FLAGS_DEBUG}")
#--    endif()
#--
#--    # Extra Flags for the release builds with the C compiler.
#--    if(EXTRA_C_FLAGS_RELEASE AND CMAKE_BUILD_TYPE MATCHES RELEASE)
#--        add_compile_options("${EXTRA_C_FLAGS_RELEASE}")
#--    endif()
#--
#--    #############################################
#--    # Support Extra Flags for the C++ compiler.
#--    #############################################
#--    if(EXTRA_CXX_FLAGS)
#--        add_compile_options("${EXTRA_CXX_FLAGS}")
#--    endif()
#--
#--    # Extra Flags for the debug builds with the C++ compiler.
#--    if(EXTRA_CXX_FLAGS_DEBUG AND CMAKE_BUILD_TYPE MATCHES Debug)
#--        add_compile_options("${EXTRA_CXX_FLAGS_DEBUG}")
#--    endif()
#--
#--    # Extra Flags for the release builds with the C++ compiler.
#--    if(EXTRA_CXX_FLAGS_RELEASE AND CMAKE_BUILD_TYPE MATCHES RELEASE)
#--        add_compile_options("${EXTRA_CXX_FLAGS_RELEASE}")
#--    endif()
#--
#--endif()
#--
#--################################
#--# RPath Settings
#--################################
#--
#--# use, i.e. don't skip the full RPATH for the build tree
#--SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
#--
#--# when building, don't use the install RPATH already
#--# (but later on when installing)
#--set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#--set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#--set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
#--
#--# add the automatically determined parts of the RPATH
#--# which point to directories outside the build tree to the install RPATH
#--set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#--
#--# the RPATH to be used when installing, but only if it's not a system directory
#--list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
#--if("${isSystemDir}" STREQUAL "-1")
#--   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#--endif()
#--
#--################################
#--#  macros
#--################################
#--
#--
#--##------------------------------------------------------------------------------
#--## - Adds a component to the build given the component's name and default state
#--##   (ON/OFF). This macro also adds an "option" so that the user can control
#--##   which components to build.
#--##------------------------------------------------------------------------------
#--macro(add_component)
#--
#--    set(options)
#--    set(singleValueArgs COMPONENT_NAME DEFAULT_STATE )
#--    set(multiValueArgs)
#--
#--    ## parse the arugments to the macro
#--    cmake_parse_arguments(arg
#--         "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})
#--    
#--    ## setup a cmake vars to capture sources added via our macros
#--    set("${arg_COMPONENT_NAME}_ALL_SOURCES" CACHE PATH "" FORCE)
#--        
#--    ## adds an option so that the user can control whether to build this
#--    ## component.
#--    option( ENABLE_${arg_COMPONENT_NAME}
#--            "Enables ${arg_component_name}"
#--            ${arg_DEFAULT_STATE})
#--
#--    if ( ENABLE_${arg_COMPONENT_NAME} )
#--        add_subdirectory( ${arg_COMPONENT_NAME} )
#--    endif()
#--
#--endmacro(add_component)
#--
#--##------------------------------------------------------------------------------
#--## - Given a library target nane and a list of sources, this macros builds
#--##   a static or shared library according to a user-supplied BUILD_SHARED_LIBS
#--##   option.
#--##
#--## make_library(libtarget srcs)
#--##------------------------------------------------------------------------------
#--macro(make_library libtarget srcs)
#--
#--   if ( BUILD_SHARED_LIBS )
#--      add_library(${libtarget} SHARED ${srcs})
#--   else()
#--      add_library(${libtarget} STATIC ${srcs})
#--   endif()
#--
#--   if ( ENABLE_CXX11 )
#--      ## Note, this requires cmake 3.1 and above
#--      set_property(TARGET ${libtarget} PROPERTY CXX_STANDARD 11)
#--   endif()
#--    
#--    
#--   foreach(src ${srcs})
#--       if(IS_ABSOLUTE)
#--           list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${src}")
#--       else()
#--           list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
#--       endif()
#--   endforeach()
#--       
#--    set("${PROJECT_NAME}_ALL_SOURCES" "${${PROJECT_NAME}_ALL_SOURCES}" CACHE STRING "" FORCE )
#--
#--endmacro(make_library)
#--
#--##------------------------------------------------------------------------------
#--## - Make executable
#--##
#--## make_executable( EXECUTABLE_SOURCE source DEPENDS_ON dep1 dep2...)
#--##------------------------------------------------------------------------------
#--macro(make_executable)
#--
#--   set(options)
#--   set(singleValueArgs EXECUTABLE_SOURCE)
#--   set(multiValueArgs DEPENDS_ON)
#--
#--   ## parse the arugments to the macro
#--   cmake_parse_arguments(arg
#--        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})
#--
#--    get_filename_component(exe_name ${arg_EXECUTABLE_SOURCE} NAME_WE)
#--    add_executable( ${exe_name} ${arg_EXECUTABLE_SOURCE} )
#--    target_link_libraries( ${exe_name} "${arg_DEPENDS_ON}" )
#--
#--    if ( ENABLE_CXX11 )
#--      ## Note, this requires cmake 3.1 and above
#--      set_property(TARGET ${exe_name} PROPERTY CXX_STANDARD 11)
#--    endif()
#--    
#--        
#--    if(IS_ABSOLUTE)
#--        list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${arg_EXECUTABLE_SOURCE}")
#--    else()
#--        list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/${arg_EXECUTABLE_SOURCE}")
#--    endif()
#--
#--    set("${PROJECT_NAME}_ALL_SOURCES" "${${PROJECT_NAME}_ALL_SOURCES}" CACHE STRING "" FORCE )
#--    
#--endmacro(make_executable)
#--
#--##------------------------------------------------------------------------------
#--## - Builds and adds a test which employs the Catch unit testing framework.
#--##
#--## add_gtest( TEST_SOURCE testX.cxx DEPENDS_ON dep1 dep2... )
#--##------------------------------------------------------------------------------
#--macro(add_gtest)
#--
#--   set(options)
#--   set(singleValueArgs TEST_SOURCE)
#--   set(multiValueArgs DEPENDS_ON)
#--
#--   ## parse the arguments to the macro
#--   cmake_parse_arguments(arg
#--        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )
#--
#--   get_filename_component(test_name ${arg_TEST_SOURCE} NAME_WE)
#--   add_executable( ${test_name} ${arg_TEST_SOURCE} )
#--   target_include_directories(${test_name} PRIVATE "${GTEST_INCLUDES}")
#--   target_link_libraries( ${test_name} "${GTEST_LIBS}" )
#--   target_link_libraries( ${test_name} "${arg_DEPENDS_ON}" )
#--
#--    if ( ENABLE_CXX11 )
#--      ## Note, this requires cmake 3.1 and above
#--      set_property(TARGET ${test_name} PROPERTY CXX_STANDARD 11)
#--    endif()
#--
#--    add_test( NAME ${test_name}
#--              COMMAND ${test_name}
#--              WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
#--              )
#--
#--    # add any passed source files to the running list for this project
#--    if(IS_ABSOLUTE)
#--        list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${arg_TEST_SOURCE}")
#--    else()
#--          list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/${arg_TEST_SOURCE}")
#--    endif()
#--
#--    set("${PROJECT_NAME}_ALL_SOURCES" "${${PROJECT_NAME}_ALL_SOURCES}" CACHE STRING "" FORCE )
#--    
#--endmacro(add_gtest)
#--
#--##------------------------------------------------------------------------------
#--## - Builds and adds a test which employs the Catch unit testing framework.
#--##
#--## add_catch_test( TEST_SOURCE testX.cxx DEPENDS_ON dep1 dep2... )
#--##------------------------------------------------------------------------------
#--macro(add_catch_test)
#--
#--   set(options)
#--   set(singleValueArgs TEST_SOURCE)
#--   set(multiValueArgs DEPENDS_ON)
#--
#--   ## parse the arguments to the macro
#--   cmake_parse_arguments(arg
#--        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )
#--
#--   get_filename_component(test_name ${arg_TEST_SOURCE} NAME_WE)
#--   add_executable( ${test_name} ${arg_TEST_SOURCE} )
#--   target_link_libraries( ${test_name} "${arg_DEPENDS_ON}" )
#--
#--    if ( ENABLE_CXX11 )
#--      ## Note, this requires cmake 3.1 and above
#--      set_property(TARGET ${test_name} PROPERTY CXX_STANDARD 11)
#--    endif()
#--
#--    add_test( NAME ${test_name}
#--              COMMAND ${test_name}
#--              WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
#--              )
#--
#--    # add any passed source files to the running list for this project
#--    if(IS_ABSOLUTE)
#--        list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${arg_TEST_SOURCE}")
#--    else()
#--        list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/${arg_TEST_SOURCE}")
#--    endif()
#--
#--    set("${PROJECT_NAME}_ALL_SOURCES" "${${PROJECT_NAME}_ALL_SOURCES}" CACHE STRING "" FORCE )
#--
#--endmacro(add_catch_test)

##------------------------------------------------------------------------------
## - Adds a custom "copy_headers" target to copy the given list of headers to
##   the supplied destination directory.
##
## copy_headers_copy( hdrs dest )
##------------------------------------------------------------------------------
#--macro(copy_headers_target proj hdrs dest)
#--
#--add_custom_target(copy_headers_${proj}
#--     COMMAND ${CMAKE_COMMAND}
#--             -DHEADER_INCLUDES_DIRECTORY=${dest}
#--             -DLIBHEADERS="${hdrs}"
#--             -DTGTLIB=${proj}
#--             -P ${CMAKE_MODULE_PATH}/copy_headers.cmake
#--
#--     DEPENDS
#--        ${hdrs}
#--
#--     WORKING_DIRECTORY
#--        ${PROJECT_SOURCE_DIR}/${proj}
#--
#--     COMMENT
#--        "copy headers ${proj}"
#--     )
#--     
#--     # add any passed source files to the running list for this project
#--     foreach(hdr ${hdrs})
#--         if(IS_ABSOLUTE)
#--             list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${hdr}")
#--         else()
#--             list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/${proj}/${hdr}")
#--         endif()
#--     endforeach()
#--
#--     set("${PROJECT_NAME}_ALL_SOURCES" "${${PROJECT_NAME}_ALL_SOURCES}" CACHE STRING "" FORCE )    
#--     
#--endmacro(copy_headers_target)
#--
#--

##############################
# make-def
#LDPath     = -L/home/taylor16/gapps/gcc-4.9.0/lib64 -L/home/taylor16/gapps/gcc-4.9.0/lib -L/home/taylor16/pact/cmake/dev/lnx-2.12-o/lib -L/usr/lib64 -L/usr/lib
#LDRPath    = -Wl,-rpath,/home/taylor16/gapps/gcc-4.9.0/lib64:/home/taylor16/gapps/gcc-4.9.0/lib:/home/taylor16/pact/cmake/dev/lnx-2.12-o/lib:/usr/lib64:/usr/lib
#LDFLAGS    = -Wl,--disable-new-dtags -rdynamic ${LDRPath} ${LDPath}
#LXFLAGS    = -fPIC -w -m64 -fPIC
#MDInc      =   
set(MDLib  -lz -lc -lm)
#DPInc      = 
#DPLib      = 
#MDI_Inc    = -I/usr/include/openssl
#MDI_Lib    = -lsqlite3 -lssl
set(MDE_Lib  /usr/lib64/libbfd.a -liberty -ldl)
#FLib       = 


##############################

# Copy file to dest directory using sinstall
macro(sinstall_file file dest)
    set(infile  ${CMAKE_CURRENT_SOURCE_DIR}/${file})
    add_custom_target(
        ${file}
        COMMAND ${PSY_ScrDir}/sinstall ${infile} ${dest}
        DEPENDS ${infile}
#        COMMENT "XXXXXXXXX  Install ${file}"
    )
endmacro(sinstall_file)









# PACT config
include(CheckFunctionExists)
#CHECK_LIBRARY_EXISTS(rt clock_gettime "time.h" HAVE_CLOCK_GETTIME)

#exec_program(Executable [directory in which to run]
#		[ARGS <arguments to executable>]
#		[OUTPUT_VARIABLE <var>]
#		[RETURN_VALUE <var>])

#try_run(RUN_RESULT_VAR COMPILE_RESULT_VAR
#        bindir srcfile [CMAKE_FLAGS <flags>...]
#        [COMPILE_DEFINITIONS <defs>...]
#        [LINK_LIBRARIES <libs>...]
#        [COMPILE_OUTPUT_VARIABLE <var>]
#        [RUN_OUTPUT_VARIABLE <var>]
#legacy  [OUTPUT_VARIABLE <var>]
#        [ARGS <args>...])


# from dsys
file (STRINGS ${PACT_MANAGER_DIR}/.pact-version PACT_VERSION)

exec_program(${PACT_SCRIPTS_DIR}/system-id
             OUTPUT_VARIABLE PACT_PSY_Arch)

#!!exec_program(${PACT_SCRIPTS_DIR}/nbin -d
#!!             OUTPUT_VARIABLE PACT_Bits)



# manager/std/program-analyze

# figure out the system basics now
#--Run {
#--
#--# figure out libm first to minimize variable reset tangle
#--   ../analyze/libm
#--
#--# greater context
#--   ../analyze/site
#--
#--# hardware
#--   ../analyze/hw
#--
#--# operating system
#--   ../analyze/os
#--
#--# file system
#--   ../analyze/fs
#--
#--# shells
#--   ../analyze/csh
#--   ../analyze/sh
#--
#--# compilers/linkers
#--   ../analyze/cc
#--}
#--
#--# with the C compiler in hand figure out the remaining utilities
#--Run {
#--   ../analyze/fc
#--   ../analyze/ld
#--   ../analyze/yacc
#--   ../analyze/lex
#--
#--# system level utilities
#--   ../analyze/make
#--   ../analyze/python
#--   ../analyze/ranlib
#--   ../analyze/install
#--}
#--
#--Note "   System features:"
#--
#--Run {
#--
#--# C features
#--   ../analyze/socket
check_function_exists(getsockopt HAVE_SOCKETS)

#--   ../analyze/vsnprintf
#--   ../analyze/strerror_r
#--   ../analyze/fpe
#--
#--# POSIX features
#--   ../analyze/fork
#--   ../analyze/ipc
try_run(HAVE_PROCESS_CONTROL
    COMPILE_RESULT_VAR
    ${PACT_ANALYZE_BINARY_DIR}
    ${PACT_ANALYZE_SOURCE_DIR}/analyze_ipc.c
#    OUTPUT_VARIABLE OUTPUT
    )
if(HAVE_PROCESS_CONTROL)
  message("-- HAVE_PROCESS_CONTROL defined")
else()
  message("-- HAVE_PROCESS_CONTROL not defined")
endif()
#--   ../analyze/signal
#--   ../analyze/mmap
#--   ../analyze/gethost
#--   ../analyze/rusage
#--   ../analyze/getpwuid
#--   ../analyze/poll
#--   ../analyze/pty
#--   ../analyze/termio
#--   ../analyze/asyncio
#--
#--# library support
#--   ../analyze/math
#--
#--# parallel support
#--   ../analyze/openmp
#--   ../analyze/mpi
#--   ../analyze/jobs
#--
#--# I/O
#--   ../analyze/tracker
#--   ../analyze/readline
#--   ../analyze/hdf5
#--   ../analyze/sql
#--   ../analyze/ssl
#--
#--# graphics
#--   ../analyze/quickdraw
#--   ../analyze/opengl
#--   ../analyze/x11
#--   ../analyze/png
#--   ../analyze/jpeg
#--}
#--
#--# wrap up
#--run ../analyze/finish


file(MAKE_DIRECTORY ${IncDir}/shell)
file(MAKE_DIRECTORY ${LogDir})


# 'include' generated files
# dsys PCO_CONFIG
#   set GenFiles = ( $GenFiles f90-interface )
#   set GenFiles = ( $GenFiles iso-c.h scconfig.h scdecls.h syntax.h)
#   set GenFiles = ( $GenFiles spokes.scm )

foreach(file iso-c.h scconfig.h scdecls.h syntax.h)
  configure_file(${PACT_MANAGER_DIR}/cmake/${file}.in
                 ${IncDir}/${file}
                 @ONLY)
endforeach()

foreach(file env-pact.csh env-pact.dk env-pact.mdl env-pact.sh)
  configure_file(${PACT_MANAGER_DIR}/cmake/${file}.in
                 ${EtcDir}/${file}
                 @ONLY)
endforeach()

# copied by dsys
set (DB_TYPES ${EtcDir}/types.db)
file(COPY ${PSY_MngDir}/types.db DESTINATION ${EtcDir})

# install cpyright.h for compiles
#file(COPY cpyright.h DESTINATION ${HEADER_INCLUDES_DIRECTORY})
configure_file(cpyright.h ${IncDir}/cpyright.h COPYONLY)
configure_file(manager/types.db ${EtcDir}/types.db COPYONLY)


# 'etc' generated files
#   set GenFiles = ( $GenFiles package configured )
#   set GenFiles = ( $GenFiles make-def make-macros Makefile )


# XXX intel math library
add_library( libm SHARED IMPORTED )
set_target_properties( libm PROPERTIES
    IMPORTED_LOCATION /lib/libm.so.6)


set(YACC_EXECUTABLE  /usr/bin/yacc)
set(LEX_EXECUTABLE   /usr/bin/lex)
