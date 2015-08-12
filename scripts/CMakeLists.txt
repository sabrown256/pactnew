#
# CMakeLists.txt for scripts
#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#
set(Package scripts)
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
    pwhere bproto
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
sinstall_target("${TGTEtc}" ${EtcDir} incinstall_scripts_etc incinstall)

sinstall_target("${TGTScr}" ${BinDir} incinstall_scripts_bin incinstall)

## These are runtime dependencies
#add_dependencies(do-automake pwhere)
#add_dependencies(do-net pwhere)
#add_dependencies(gcc-info pwhere)
add_dependencies(import-api elide perdb functions-pact.csh)
#add_dependencies(loco pwhere)
#add_dependencies(ls-all pwhere)
#add_dependencies(sign system-id pwhere)
#add_dependencies(sinstall pwhere)
#add_dependencies(system-id pwhere)
#add_dependencies(update-sys pwhere)
#add_dependencies(update-watch pwhere)
add_dependencies(bproto elide blang functions-pact.csh)


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

