#
# CMakeLists.txt for scripts
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#
#set(LibName scripts)

# these are now C programs in PSH
#--TGTCScr = do-net atdbg

set(TGTScr
    do-local do-stats do-automake distribute
    cfgsys cfgman system-id sys-site nbit plock
    iopr iocl nfsmon timer px
    ls-jobs ls-jobs-cygwin ls-all running run-all load-ave
    rmalbut sign cleanup-jobs remod mem-stats
    herd cover find-fnc diff-code label-ps bckgr
    broadcast descend dot-silent no-op dargs fix-group
    merge-aix update-sys update-watch
    check-hosts check-service all-service
    cdlog which-cfg mpi-info diff-tree use-tcsh
    simplify-path simplify-list gcc-info sinstall fill-string
    mall png-diff sunrace cross-info find-comments
    import-api import-so prune-env measure-env
    which-scs scs-update scs-commit scs-co scs-history scs-meta
    scs-add scs-remove scs-lock scs-diff scs-move scs-tag
    scs-sync scs-revert scs-init
    var-list winer loco who-does get-line frnsic stan
    insert-text file-time
#    change-vname
    exchf
    pwhere
)

set(TGTEtc
    csh-subroutines
    functions-pact.csh functions-pact.sh functions-pact.pl
)

#--IncAction = sinstall cross-info ${BinDir}

#--targets :
#--	@echo "Targets for SCRIPTS"
#--	@echo "   install   : install utilities in dev/sys_id"
#--	@echo "   link      : install utilities for public use"
#--	@echo "   test      : run the SCRIPTS test suite"

#
# Generated code rules
#

# headers

#--hdrf = 

# language bindings

#--bindf = 

#--generate : ${bindf} ${hdrf}

#--IncAction = sinstall ${TGTEtc} ${EtcDir}
add_custom_target(incinstall_scripts_bin
  COMMAND ${PSY_ScrDir}/sinstall ${TGTScr} ${BinDir}
  DEPENDS ${TGTScr}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "incinstall bin in scripts  DEBUG ${TGTgScr}"
)
set_property(GLOBAL APPEND PROPERTY incinstall_target incinstall_scripts_bin)

add_custom_target(incinstall_scripts_etc
  COMMAND ${PSY_ScrDir}/sinstall ${TGTEtc} ${EtcDir}
  DEPENDS ${TGTEtc}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "incinstall etc in scripts  DEBUG ${TGTEtc}"
)
set_property(GLOBAL APPEND PROPERTY incinstall_target incinstall_scripts_etc)
#add_dependencies(incinstall_scripts incinstall_scripts_IncAction)


#--link :
#--	sinstall do-batch.${JOBS_System} ${BinDir}/do-batch
#--	pact scrinstall

#--install :
#--	pact link

#
# Run the SCRIPTS test suite
#
#--test :
#--	@./srtest

