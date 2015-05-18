#XXX pre-make   gcs?

#
# do-proto - flag if proto should be run
# GSrcs    - generated source files for TGTLib
# GenInc   - generated include files
# hdrf     - 
#


#
# language bindings
#
## XXX make-macros  generate target
## XXX premake -- IncAction = @pact generate
## XXX generate : ${gcb} ${hdrf}
if(do-bproto)
    set(gcb ${CMAKE_CURRENT_BINARY_DIR}/gc-${LibName}.c)
    set(ghb ${CMAKE_CURRENT_BINARY_DIR}/gc-${LibName}.h)

    #set(GenInc ${GenInc} ${ghb} )
    #LIST(APPEND myVar newItem)
    LIST(APPEND GSrcs ${gcb})
    LIST(APPEND GenInc ${ghb})
    LIST(APPEND hdrf ${ghb})

    add_custom_command(
        OUTPUT ${gcb} ${ghb}
        PRE_BUILD
        COMMAND ${PSY_ScrDir}/bproto -root ${PSY_Root} -d ${CMAKE_CURRENT_BINARY_DIR} ${ExcludeType} -f ${Srcs} ${Hdrs}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "bproto ${LibName}"
    )
endif()

#--${IncDir}/score_gen.h : ${GenInc}
#--	@echo "build score_gen.h"
#--	@rm -f ${IncDir}/score_gen.h
#--	@cat ${GenInc} > ${IncDir}/score_gen.h
if(GenInc)
    add_custom_command(
        OUTPUT ${IncDir}/${LibName}_gen.h
        DEPENDS ${GenInc}
        COMMAND rm -f ${IncDir}/${LibName}_gen.h
        COMMAND cat ${GenInc} > ${IncDir}/${LibName}_gen.h
        COMMENT "build ${LibName}_gen.h"
    )
    LIST(APPEND hdrf ${IncDir}/${LibName}_gen.h)
endif()

if(hdrf)
    add_custom_target(generate_${LibName}
        DEPENDS ${hdrf}
        COMMENT "generate ${LibName}   DEBUG ${hdrf}"
    )
    set_property(GLOBAL APPEND PROPERTY generate_target generate_${LibName})
endif()

#
# install includes
#
#--incinstall : ${TGTInc}
#--	@[ ! -f ${IncDir}/.lock ] || { echo "${IncDir} is locked" ; exit 1 ; }
#--	${IncAction}
#--	@[ "${TGTInc}" != "noinc" ] && { echo "sinstall ${TGTInc} ${IncDir}" ; sinstall ${TGTInc} ${IncDir} ; } || { exit 0 ; }
add_custom_target(incinstall_${LibName}
#   noinc
    COMMAND ${PSY_ScrDir}/sinstall ${TGTInc} ${IncDir}
    DEPENDS ${TGTInc}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "incinstall in ${LibName}  DEBUG ${TGTInc}"
)
set_property(GLOBAL APPEND PROPERTY incinstall_target incinstall_${LibName})

if(hdrf)
    add_dependencies(incinstall_${LibName} generate_${LibName})
endif()


add_custom_target(devlib_${LibName}
#   noinc
#    COMMAND ${PSY_ScrDir}/sinstall ${TGTInc} ${IncDir}
#    DEPENDS ${TGTInc}
#    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "devlib in ${LibName}"
)
set_property(GLOBAL APPEND PROPERTY devlib_target devlib_${LibName})

#
# make new library
#
#--${TGTLib} : ${LibDep}
#--	@[ ! -f ${LibDir}/.lock ] || { echo "${LibDir} is locked" ; exit 1 ; }
#--	${LibAction}
#--	aranlib ${TGTLib}
#--
#--devlib : ${TGTLib}
if(TGTLib)
    add_library(${LibName} ${GSrcs} ${Srcs})
#    add_dependencies(${LibName} incinstall_${LibName})
    add_dependencies(${LibName} incinstall)
    add_dependencies(devlib_${LibName} ${LibName})
endif()





# Add to global targets


#set(MY_GLOBAL_VARIABLE "Some value" CACHE STRING "Description")
#list(APPEND $pact_generate generate_${LibName})
#set (SOURCEFILES main.cpp foo.cpp PARENT_SCOPE) 
