cmake_minimum_required(VERSION 3.1)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/manager/cmake" ${CMAKE_MODULE_PATH})

################################
# ASCTK ToolKit
################################
project(PACT)

################################
# Prevent in-source builds
################################
#include(PreventInSourceBuilds)

################################
# Set some standard cmake opts
################################
include(CMakeBasics)

################################
# Setup 3rd Party Libs
################################
#include(Setup3rdParty)

################################
# Setup toolkit docs targets
################################
#include(SetupDocs)

# Setup toolkit source checks
################################
#include(SetupCodeChecks)

###############
enable_testing()

set_property(GLOBAL PROPERTY generate_target )

add_subdirectory(psh)
#add_subdirectory(score)
#add_subdirectory(pml)
#add_subdirectory(pdb)
#add_subdirectory(pgs)
#add_subdirectory(scheme)
#add_subdirectory(panacea)
##add_subdirectory(sx)

#add_subdirectory(doc)
#add_subdirectory(fortran)
#add_subdirectory(python)
add_subdirectory(scripts)


get_property(generate_target_list GLOBAL PROPERTY generate_target) 
#message(STATUS "generate_target_list = ${generate_target_list}")
add_custom_target(generate
    DEPENDS ${generate_target_list}
    )

get_property(incinstall_target_list GLOBAL PROPERTY incinstall_target) 
message(STATUS "incinstall_target_list = ${incinstall_target_list}")
add_custom_target(incinstall
    DEPENDS generate ${incinstall_target_list}
    )

get_property(devlib_target_list GLOBAL PROPERTY devlib_target) 
#message(STATUS "devlib_target_list = ${devlib_target_list}")
add_custom_target(devlib ALL
    DEPENDS incinstall ${devlib_target_list}
    )

################################
# install targets from build
################################s
#install(DIRECTORY ${PROJECT_BINARY_DIR}/bin DESTINATION . )
#install(DIRECTORY ${PROJECT_BINARY_DIR}/include DESTINATION .)

# debug help
# make VERBOSE=1
