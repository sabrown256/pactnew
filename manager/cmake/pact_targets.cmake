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
    set(gcb ${CMAKE_CURRENT_BINARY_DIR}/gc-${Package}.c)
    set(ghb ${CMAKE_CURRENT_BINARY_DIR}/gc-${Package}.h)

#  SET_SOURCE_FILES_PROPERTIES(${gcb} PROPERTIES GENERATED TRUE)

    #set(GenInc ${GenInc} ${ghb} )
    #LIST(APPEND myVar newItem)
    LIST(APPEND GSrcs ${gcb})
    LIST(APPEND GenInc ${ghb})
    LIST(APPEND hdrf ${ghb})

    add_custom_command(
        OUTPUT ${gcb} ${ghb}
#        OUTPUT ${gcb} ${ghb} ${bproto-output}
        DEPENDS bproto
        COMMAND ${BinDir}/bproto
               -d ${CMAKE_CURRENT_BINARY_DIR}
               -move
              ${ExcludeType} -f ${Srcs} ${Hdrs}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "bproto ${Package}"
    )
endif()

#--${IncDir}/score_gen.h : ${GenInc}
#--	@echo "build score_gen.h"
#--	@rm -f ${IncDir}/score_gen.h
#--	@cat ${GenInc} > ${IncDir}/score_gen.h
if(GenInc)
    add_custom_command(
        OUTPUT ${IncDir}/${Package}_gen.h
        DEPENDS ${GenInc}
        COMMAND rm -f ${IncDir}/${Package}_gen.h
        COMMAND cat ${GenInc} > ${IncDir}/${Package}_gen.h
        COMMENT "build ${Package}_gen.h"
    )
    LIST(APPEND hdrf ${IncDir}/${Package}_gen.h)
endif()

if(hdrf)
    add_custom_target(generate_${Package}
        DEPENDS ${hdrf}
        COMMENT "generate ${Package}   DEBUG ${hdrf}"
    )
    set_property(GLOBAL APPEND PROPERTY generate_target generate_${Package})
endif()

#
# install includes
#
#--incinstall : ${TGTInc}
#--	@[ ! -f ${IncDir}/.lock ] || { echo "${IncDir} is locked" ; exit 1 ; }
#--	${IncAction}
#--	@[ "${TGTInc}" != "noinc" ] && { echo "sinstall ${TGTInc} ${IncDir}" ; sinstall ${TGTInc} ${IncDir} ; } || { exit 0 ; }

foreach(file ${TGTInc})
    set(infile  ${CMAKE_CURRENT_SOURCE_DIR}/${file})
    set(outfile ${IncDir}/${file})
    add_custom_command(
        OUTPUT ${outfile}
        COMMAND ${CMAKE_COMMAND} -E copy ${infile} ${outfile}
        MAIN_DEPENDENCY ${infile}
        COMMENT "Copy ${file}"
    )
    LIST(APPEND TGTInc_LIST ${outfile})
endforeach(file)

add_custom_target(incinstall_${Package}
#   noinc
    DEPENDS ${TGTInc_LIST}
    COMMENT "incinstall in ${Package}"
)
set_property(GLOBAL APPEND PROPERTY incinstall_target incinstall_${Package})

if(hdrf)
    add_dependencies(incinstall_${Package} generate_${Package})
endif()


add_custom_target(devlib_${Package}
#   noinc
#    COMMAND ${PSY_ScrDir}/sinstall ${TGTInc} ${IncDir}
#    DEPENDS ${TGTInc}
#    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "devlib in ${Package}"
)
set_property(GLOBAL APPEND PROPERTY devlib_target devlib_${Package})

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
    add_library(${TGTLib} ${GSrcs} ${Srcs})
#    add_dependencies(${TGTLib} incinstall_${Package})
    add_dependencies(${TGTLib} incinstall)
    add_dependencies(devlib_${Package} ${Package})
endif()





# Add to global targets


#set(MY_GLOBAL_VARIABLE "Some value" CACHE STRING "Description")
#list(APPEND $pact_generate generate_${Package})
#set (SOURCEFILES main.cpp foo.cpp PARENT_SCOPE) 
