#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

AUTOMAKE_OPTIONS = no-dependencies

sourcedir=$(top_srcdir)/scripts
builddir=$(top_builddir)/scripts
 
bin_SCRIPTS = \
atdbg \
broadcast \
cleanup-jobs \
descend \
diff-code \
distribute \
do-batch \
do-local \
do-net \
do-run.csh \
do-stats \
dot-silent \
find-fnc \
label-ps \
load-all \
ls-all \
ls-jobs \
no-op \
dargs \
merge-aix \
iopr \
iocl \
nfsmon \
px \
rmalbut \
run-all \
sign \
swhich \
system-id \
test-env \
timer

include_HEADERS = env-csh csh-subroutines

EXTRA_DIST = $(bin_SCRIPTS) DISCLAIMER 

all-local: $(bin_SCRIPTS)
	@test -e $(builddir)/do-run.csh || cp $(sourcedir)/do-run   $(builddir)/do-run
	@test -e $(builddir)/do-batch   || cp $(sourcedir)/do-batch $(builddir)/do-batch
	@test -e $(builddir)/timer      || cp $(sourcedir)/timer    $(builddir)/timer
	@test -e $(builddir)/test-env   || cp $(sourcedir)/test-env $(builddir)/test-env
        
