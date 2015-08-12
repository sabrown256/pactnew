cmake_minimum_required(VERSION 3.1)

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/manager/cmake" ${CMAKE_MODULE_PATH})

################################
# ASCTK ToolKit
################################
project(PACT)
enable_language(Fortran)  # OPTIONAL)

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
add_subdirectory(scripts)

add_subdirectory(score)
add_subdirectory(pml)

# NOTE: pdb requires file pgs_gen.h from pgs
add_subdirectory(pdb)
add_subdirectory(pgs)

add_subdirectory(scheme)
add_subdirectory(panacea)
add_subdirectory(sx)  #dependencies to generate gs-score.h first

add_subdirectory(fortran)
#add_subdirectory(python)

#add_subdirectory(doc)


get_property(generate_target_list GLOBAL PROPERTY generate_target) 
add_custom_target(generate
    DEPENDS pact-build-tools ${generate_target_list}
    COMMENT "generate - ${generate_target_list}"
    )

get_property(incinstall_target_list GLOBAL PROPERTY incinstall_target) 
add_custom_target(incinstall
    DEPENDS pact-build-tools generate ${incinstall_target_list}
    COMMENT "incinstall  - ${incinstall_target_list}"
    )

get_property(devlib_target_list GLOBAL PROPERTY devlib_target) 
add_custom_target(devlib ALL
    DEPENDS incinstall ${devlib_target_list}
    COMMENT "devlib ${devlib_target_list}"
    )

################################
# install targets from build
################################s
#install(DIRECTORY ${PROJECT_BINARY_DIR}/bin DESTINATION . )
#install(DIRECTORY ${PROJECT_BINARY_DIR}/include DESTINATION .)

# debug help
# make VERBOSE=1
