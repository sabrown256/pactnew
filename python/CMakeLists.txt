#
# CMakeLists.txt for Python wrappers
#
# Files and Directories
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#
set(Package python)
set(LibName py)

# dummy target lib just to get to LibAction
#--TGTLib = ${LibDir}/libpy.a
set(TGTInc py_int.h pputil.h ppcommon.h)

#--RemoveAction = @./psys remove
#--LibAction    = @./psys devlib
#--IncAction    = @./psys incinstall

#--------------------------------------------------------------------------

set(GenInc
    ${CMAKE_CURRENT_BINARY_DIR}/gp-score.h
    ${CMAKE_CURRENT_BINARY_DIR}/gp-pml.h
    ${CMAKE_CURRENT_BINARY_DIR}/gp-pdb.h
    ${CMAKE_CURRENT_BINARY_DIR}/gp-pgs.h
    ${CMAKE_CURRENT_BINARY_DIR}/gp-panacea.h
)

#--${IncDir}/py_gen.h : ${GenInc}
#--	@echo "build py_gen.h"
#--	@rm -f ${IncDir}/py_gen.h
#--	@cat ${GenInc} > ${IncDir}/py_gen.h

#hdrf = ${IncDir}/py_gen.h

#--generate:
#--	@./psys generate

#--IncAction = @pact ${IncDir}/py_gen.h

#--------------------------------------------------------------------------

#--build:
#--	@./psys build

#--install:
#--	@./psys link

#--------------------------------------------------------------------------

#--link:
#--	@./psys link
#--	cp py-install ${BinDir}

#--sharedlib:
#--	@./psys sharedlib

#--inform :
#--	@./psys inform

#--test :
#--	@./psys test


