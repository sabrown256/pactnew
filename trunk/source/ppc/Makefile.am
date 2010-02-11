#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = ppc
testdirectory = $(builddir)/check

include $(top_srcdir)/Makefile.common


NO_MPI_AM_CFLAGS = @CDebug@ -I$(sourcedir) -I$(top_builddir) -I$(top_builddir)/config \
-I$(top_srcdir)/pdb -I$(top_srcdir)/score -I$(top_srcdir)/pml 
AM_CFLAGS = $(NO_MPI_AM_CFLAGS) @MPIFLAGS@
LIBS = @LIBS@ @MPILIBS@
 
ADD_LD_FLAGS = $(LDFLAGS) libppc.la \
$(top_builddir)/pdb/libpdb.la \
$(top_builddir)/pml/libpml.la \
$(top_builddir)/score/libscore.la \
-lm -lc @LIBS@

PC_CC=$(LTCC) $(NO_MPI_AM_CFLAGS) 


EXTRA_DIST = \
DISCLAIMER \
ppc.FAQ \
pctest \
pcmtst.c \
pcbtst.c \
pcctst.c \
pcexec.c

CLEANFILES = pcmtst pcbtst pcctst pcexec


lib_LTLIBRARIES = libppc.la
include_HEADERS = ppc.h


libppc_la_DEPENDENCIES = $(top_builddir)/pdb/libpdb.la 
libppc_la_SOURCES = \
pcparc.c \
pcparmp.c \
ppc.c \
pcbin.c \
pcpio.c \
ppc.h


all-local: pcexec

install-exec-local:
	$(LIBTOOL) --mode=install cp pcexec $(bindir)/pcexec

        
pcexec: $(sourcedir)/pcexec.c libppc.la
	rm -f $(builddir)/pcexec
	$(PC_CC) $(sourcedir)/pcexec.c -o $(builddir)/pcexec $(ADD_LD_FLAGS)


# Targets for pctest
Cstd: pcexec pcmtst pcbtst pcctst

TESTS = $(top_srcdir)/ppc/pctest

pcmtst: $(sourcedir)/pcmtst.c libppc.la
	@rm -f pcmtst
	$(PC_CC) -static $(sourcedir)/pcmtst.c -o $(builddir)/pcmtst $(ADD_LD_FLAGS)

pcbtst: $(sourcedir)/pcbtst.c libppc.la
	@rm -f pcbtst
	$(PC_CC) -static $(sourcedir)/pcbtst.c -o $(builddir)/pcbtst $(ADD_LD_FLAGS)

pcctst: $(sourcedir)/pcctst.c libppc.la
	@rm -f pcctst
	$(PC_CC) -static $(sourcedir)/pcctst.c -o $(builddir)/pcctst $(ADD_LD_FLAGS)
