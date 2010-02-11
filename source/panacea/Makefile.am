#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = panacea
testdirectory = $(builddir)/check

include $(top_srcdir)/Makefile.common


SUBDIRS = applications 

AM_CFLAGS = @CDebug@ -I$(top_srcdir)/panacea -I$(top_builddir) \
-I$(top_builddir)/config -I$(top_srcdir)/pgs -I$(top_srcdir)/ppc \
-I$(top_srcdir)/pdb -I$(top_srcdir)/pml -I$(top_srcdir)/score 

ADD_LD_FLAGS = $(LDFLAGS) libpanacea.la \
${top_builddir}/pgs/libpgs.la \
${top_builddir}/ppc/libppc.la \
${top_builddir}/pdb/libpdb.la \
${top_builddir}/pml/libpml.la \
${top_builddir}/score/libscore.la \
-lm 


EXTRA_DIST = \
DISCLAIMER \
panacea.FAQ \
abc-tester \
patest \
abc/Makefile.am \
abc/Makefile.in \
abc/abc.dict \
abc/global/Makefile.am \
abc/global/Makefile.in \
abc/global/ginit.c \
abc/global/gloin.h \
abc/global/gmesh.c \
abc/global/gplot.c \
abc/global/gpp.c \
abc/global/mesh.h \
abc/lag/Makefile.am \
abc/lag/Makefile.in \
abc/lag/lagin.c \
abc/lag/lagin.h \
abc/main/Makefile.am \
abc/main/Makefile.in \
abc/main/bs \
abc/main/b.scm \
abc/misc/Makefile.am \
abc/misc/Makefile.in \
abc/misc/miscin.c \
abc/misc/miscin.h \
abc/source/Makefile.am \
abc/source/Makefile.in \
abc/source/s.c \
abc/source/scmd.c \
abc/source/s.h \
abc/test/st.g \
abc/test/tsrc1 \
applications/panabgc.scm \
applications/panache \
applications/panache.scm \
applications/panagc.scm \
applications/panbgc.scm \
applications/panint.scm \
applications/panpgc.scm \
pathts.c
 


bin_PROGRAMS    = toul
lib_LTLIBRARIES = libpanacea.la
include_HEADERS = panace.h f77std.h


toul_SOURCES = toul.c 
toul_LDFLAGS = $(ADD_LD_FLAGS) -lc
toul_DEPENDENCIES = libpanacea.la

libpanacea_la_SOURCES = \
padef.c \
pacpp.c \
papkg.c \
pagnrd.c \
pahand.c \
pafi.c \
panacea.c \
pashar.c \
pashas.c \
paccess.c \
padsys.c \
pamm.c \
pavars.c \
papp.c \
padump.c \
panth.c \
pasrc.c \
pastr.c \
pafia.c


# Targets for patest
Cth: pathts

noinst_PROGRAMS = pathts 
TESTS = $(top_srcdir)/panacea/patest

pathts_SOURCES = pathts.c
pathts_LDFLAGS = $(ADD_LD_FLAGS)
pathts_DEPENDENCIES = libpanacea.la
