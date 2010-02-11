#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = python
testdirectory = $(builddir)/check

include $(top_srcdir)/Makefile.common


if BUILDPYTHON
  ALL=bindings
else
  ALL=
endif

PDBSRC = \
pdbmodule.c \
pdbPDBfile.c \
pdbmemdes.c \
pdbdefstr.c \
pdbpdbdata.c \
pdbhashtab.c \
pdbassoc.c 

PMLSRC = \
pmlfield.c \
pmlset.c \
pmltopology.c \
pmlmapping.c 

PPSRC = \
pputil.c \
pptypes.c \
pparrays.c \
ppdefstr.c \
pphashtab.c \
ppassoc.c \
pppdbr.c \
pypdbr.c \
pypdbl.c \
pypdbd.c \
struct.c \
pppml.c 

PGSSRC = \
pgsmodule.c \
pgsdevice.c \
pgsgraph.c \
pgsimage.c \
pgspalette.c \
utilpgs.c

PYTHON_BINDINGS_HDRS = \
pdbmodule.h \
pdbrw.h \
pputil.h \
pgsmodule.h \
utilpgs.h 

PYTHON_BINDINGS_SRC = \
$(PDBSRC) \
$(PMLSRC) \
$(PPSRC) \
$(PGSSRC) \
$(PYTHON_BINDINGS_HDRS)

EXTRA_DIST = \
DISCLAIMER \
setup.py \
pytest \
$(PYTHON_BINDINGS_SRC) \
pact/__init__.py \
pact/pdb.py \
pact/pgs.py \
python-cc.py \
python-inc-dir.py \
tests/test_assoc.py \
tests/test_pdbdata_write.py \
tests/test_pdbdata_unpack.py \
tests/test_defstr.py \
tests/test_device.py \
tests/test_eos.py \
tests/test_file.py \
tests/test_structs.py \
tests/test_hash.py \
tests/test_leak.py \
tests/test_mem.py \
tests/test_meta.py \
tests/test_mod.py \
tests/test_numpy.py \
tests/unittests.py \
tests/integrationtests.py \
tests/work_gsatst.py \
applications/pdb.idl \
applications/pdbview.py \
applications/pgs.idl 


all: $(ALL)

clean: 
	rm -f core *.so *.o *.pyc */*.pyc
	rm -rf $(builddir)/build

distclean: clean
	rm -f Makefile.in testfile

bindings:
	mkdir -p pact
	(cd pact && test -e pdb.py      || cp $(sourcedir)/pact/pdb.py pdb.py)
	(cd pact && test -e pgs.py      || cp $(sourcedir)/pact/pgs.py pgs.py)
	(cd pact && test -e __init__.py || cp $(sourcedir)/pact/__init__.py __init__.py)
	(cd $(sourcedir) && test -e $(builddir)/pputil.h || cp $(PYTHON_BINDINGS_SRC) $(builddir))
	$(PYTHON) $(sourcedir)/setup.py build --autotool
	cp build/lib.*/*.so pact
        
install-exec-local:
	$(PYTHON) $(sourcedir)/setup.py install --autotool --prefix=$(prefix)
        

TESTS = $(sourcedir)/pytest 
