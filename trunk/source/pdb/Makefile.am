#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = pdb
testdirectory = $(builddir)/check

include $(top_srcdir)/Makefile.common


NO_MPI_AM_CFLAGS = @COptimize@ -I$(top_srcdir)/pdb -I$(top_builddir) \
-I$(top_builddir)/config -I$(top_srcdir)/score -I$(top_srcdir)/pml
AM_CFLAGS = $(NO_MPI_AM_CFLAGS) @MPIFLAGS@
LIBS = @LIBS@ @MPILIBS@

ADD_LD_FLAGS=$(LDFLAGS) libpdb.la $(top_builddir)/score/libscore.la \
$(top_builddir)/pml/libpml.la -lm @LIBS@

PD_CC=$(LTCC) $(NO_MPI_AM_CFLAGS) 
PD_MPCC=$(LTCC) $(AM_CFLAGS)

EXTRA_DIST = \
DISCLAIMER \
pdb.FAQ \
pdtest \
detect.c \
pdopttst.c \
pdrectst.c \
pdtdeftst.c \
pdbtst.c \
pdntst.c \
pdasts.c \
pdatst.c \
pdgsts.c \
pdlsts.c \
pdnormtst.c \
pdcktst.c \
pdftst.f \
pdhyptst.f \
pddpts.c \
pdsmp.c

CLEANFILES = pdopttst pdbtst pdntst pdasts pdatst pdgsts pdlsts \
pdnormtst pdftst pdhyptst pdsmp pdcktst pdrectst pdtdeftst


lib_LTLIBRARIES = libpdb.la
include_HEADERS = pdb.h pdb_internal.h


libpdb_la_DEPENDENCIES = $(top_builddir)/score/libscore.la $(top_builddir)/pml/libpml.la
libpdb_la_SOURCES = \
pdb.c \
pdbaib.c \
pdbmp.c \
pdser.c \
pdlow.c \
pdrdwr.c \
pdmemb.c \
pdbmm.c \
pdconv.c \
pdbio.c \
pdpath.c \
pdbnet.c \
pdprnt.c \
pdbx.c \
pdgs.c \
pdfia.c \
pdfmt.c \
pdfmta.c \
pdfmtb.c \
pdfmtc.c \
pdbdir.c \
pdszof.c \
pdpar.c \
pdparmp.c \
$(include_HEADERS)

        
Cstd: pdbtst pdntst pdatst pdasts pdopttst pdgsts pdlsts pdnormtst \
pdrectst pdtdeftst pdcktst
Fstd: pdftst pdhyptst
smp: pdsmp
dmp: pddpts

TESTS = $(top_srcdir)/pdb/pdtest

#pddpts_SOURCES = pddpts.c
#pddpts_LDFLAGS = $(ADD_LD_FLAGS)
#pddpts_DEPENDENCIES = libpdb.la

pddpts: $(sourcedir)/pddpts.c libpdb.la
	@rm -f pddpts
	$(PD_MPCC) $(sourcedir)/pddpts.c -o $(builddir)/pddpts $(ADD_LD_FLAGS)
        
pdopttst: $(sourcedir)/pdopttst.c libpdb.la
	@rm -f pdopttst
	$(PD_CC) $(sourcedir)/pdopttst.c -o $(builddir)/pdopttst $(ADD_LD_FLAGS)

pdbtst: $(sourcedir)/pdbtst.c libpdb.la
	@rm -f pdbtst
	$(PD_CC) $(sourcedir)/pdbtst.c -o $(builddir)/pdbtst $(ADD_LD_FLAGS)

pdntst: $(sourcedir)/pdntst.c libpdb.la
	@rm -f pdntst
	$(PD_CC) $(sourcedir)/pdntst.c -o $(builddir)/pdntst $(ADD_LD_FLAGS)

pdasts: $(sourcedir)/pdasts.c libpdb.la
	@rm -f pdasts
	$(PD_CC) $(sourcedir)/pdasts.c -o $(builddir)/pdasts $(ADD_LD_FLAGS)

pdatst: $(sourcedir)/pdatst.c libpdb.la
	@rm -f pdatst
	$(PD_CC) $(sourcedir)/pdatst.c -o $(builddir)/pdatst $(ADD_LD_FLAGS)

pdgsts: $(sourcedir)/pdgsts.c libpdb.la
	@rm -f pdgsts
	$(PD_CC) $(sourcedir)/pdgsts.c -o $(builddir)/pdgsts $(ADD_LD_FLAGS)

pdlsts: $(sourcedir)/pdlsts.c libpdb.la
	@rm -f pdlsts
	$(PD_CC) $(sourcedir)/pdlsts.c -o $(builddir)/pdlsts $(ADD_LD_FLAGS)

pdnormtst: $(sourcedir)/pdnormtst.c libpdb.la
	@rm -f pdnormtst
	$(PD_CC) $(sourcedir)/pdnormtst.c -o $(builddir)/pdnormtst $(ADD_LD_FLAGS)

pdftst: $(sourcedir)/pdftst.f libpdb.la
	@rm -f pdftst
	$(LTF77) $(sourcedir)/pdftst.f -o $(builddir)/pdftst $(ADD_LD_FLAGS)

pdhyptst: $(sourcedir)/pdhyptst.f libpdb.la
	@rm -f pdhyptst
	$(LTF77) $(sourcedir)/pdhyptst.f -o $(builddir)/pdhyptst $(ADD_LD_FLAGS)

pdsmp: $(sourcedir)/pdsmp.c libpdb.la
	@rm -f pdsmp
	$(PD_CC) $(sourcedir)/pdsmp.c -o $(builddir)/pdsmp $(ADD_LD_FLAGS)

pdrectst: $(sourcedir)/pdrectst.c libpdb.la
	@rm -f pdrectst
	$(PD_CC) $(sourcedir)/pdrectst.c -o $(builddir)/pdrectst $(ADD_LD_FLAGS)

pdtdeftst: $(sourcedir)/pdtdeftst.c libpdb.la
	@rm -f pdtdeftst
	$(PD_CC) $(sourcedir)/pdtdeftst.c -o $(builddir)/pdtdeftst $(ADD_LD_FLAGS)

pdcktst: $(sourcedir)/pdcktst.c libpdb.la
	@rm -f pdcktst
	$(PD_CC) $(sourcedir)/pdcktst.c -o $(builddir)/pdcktst $(ADD_LD_FLAGS)
        
