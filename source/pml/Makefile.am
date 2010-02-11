#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = pml
testdirectory = $(builddir)/check

include $(top_srcdir)/Makefile.common


if X86
  patchX86Obj = mlfpex86.c
else
  patchX86Obj = 
endif

AM_CFLAGS = @COptimize@ -I$(top_srcdir)/pml -I$(top_builddir) -I$(top_builddir)/config \
-I$(top_srcdir)/score -I$(top_srcdir)/pdb

ADD_LD_FLAGS = libpml.la $(top_builddir)/score/libscore.la -lm


EXTRA_DIST = \
DISCLAIMER \
pml.FAQ \
mlfpex86.c \
mltest 


lib_LTLIBRARIES = libpml.la
include_HEADERS = pml.h scope_complex.h scope_math.h


libpml_la_DEPENDENCIES = $(top_builddir)/score/libscore.la
libpml_la_SOURCES = \
scope_math.h \
mlcfnc.c \
mlrfnc.c \
mlsfnc.c \
mlmath.c \
mlsrch.c \
mlfft.c \
mlmatr.c \
mlsort.c \
mlnls.c \
mlsolv.c \
mlsvd.c \
mlcg.c \
mliccg.c \
mlicgs.c \
mlintp.c \
mlfpe.c \
mloper.c \
mlgeom.c \
mlrand.c \
mlmm.c \
mlmesh.c \
mlcmsh.c \
mlmextr.c \
mlchksm.c \
mlmsrch.c \
mlvect.c \
mlfia.c $(patchX86Obj) \
$(include_HEADERS)


# Some compilers don't understand __asm__ but we know gcc does
mlfpex86.lo: mlfpex86.c
	$(LIBTOOL_COMPILE) $(TAG_CC) gcc -c -I$(top_builddir) -I$(top_builddir)/config -I$(top_srcdir)/score $(top_srcdir)/pml/mlfpex86.c

# Targets for mltest
matrix: mlmtrt
funcs: mlfnts
icsolv: mlicts mlicst mlcgts
svdsolv: mlsvdt
cgsolv: mlcgts
checksum: mlckts


noinst_PROGRAMS = mlmtrt mlicts mlicst mlsvdt mlcgts mlfnts mllnnmts mlckts
TESTS = $(top_srcdir)/pml/mltest

mlckts_SOURCES = mlckts.c
mlckts_LDADD = $(ADD_LD_FLAGS)
mlckts_DEPENDENCIES = libpml.la

mlmtrt_SOURCES = mlmtrt.c
mlmtrt_LDADD = $(ADD_LD_FLAGS)
mlmtrt_DEPENDENCIES = libpml.la

mlfnts_SOURCES = mlfnts.c
mlfnts_LDADD = $(ADD_LD_FLAGS)
mlfnts_DEPENDENCIES = libpml.la

mlicts_SOURCES = mlicts.c
mlicts_LDADD = $(ADD_LD_FLAGS)
mlicts_DEPENDENCIES = libpml.la

mlsvdt_SOURCES = mlsvdt.c
mlsvdt_LDADD = $(ADD_LD_FLAGS)
mlsvdt_DEPENDENCIES = libpml.la

mlicst_SOURCES = mlicst.c
mlicst_LDADD = $(ADD_LD_FLAGS)
mlicst_DEPENDENCIES = libpml.la

mlcgts_SOURCES = mlcgts.c
mlcgts_LDADD = $(ADD_LD_FLAGS)
mlcgts_DEPENDENCIES = libpml.la

mllnnmts_SOURCES = mllnnmts.c
mllnnmts_LDADD = $(ADD_LD_FLAGS)
mllnnmts_DEPENDENCIES = libpml.la 
