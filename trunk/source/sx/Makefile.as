#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = sx
testdirectory = $(builddir)/tests/check 

include $(top_srcdir)/Makefile.common


SUBDIRS = applications

if USE_INC
  usrinclude = -I/usr/include
else
  usrinclude = 
endif


AM_CFLAGS = @CDebug@ -I$(sourcedir) -I$(top_builddir) -I$(top_builddir)/config \
-I$(ac_x_includes) -I$(top_srcdir)/panacea -I$(top_srcdir)/scheme \
-I$(top_srcdir)/ppc -I$(top_srcdir)/pml -I$(top_srcdir)/pdb \
-I$(top_srcdir)/pgs -I$(top_srcdir)/score ${usrinclude}

ADD_LD_FLAGS = \
${top_builddir}/panacea/libpanacea.la \
${top_builddir}/scheme/libscheme.la \
${top_builddir}/pgs/libpgs.la \
${top_builddir}/ppc/libppc.la \
${top_builddir}/pdb/libpdb.la \
${top_builddir}/pml/libpml.la \
${top_builddir}/score/libscore.la \
-lm 


EXTRA_DIST = \
DISCLAIMER \
sx.FAQ \
sxtest \
tests/sxpdb.scm  \
tests/ref/sxpdb  \
tests/sxpdba.scm  \
tests/ref/sxpdba \
tests/sxpdbi.scm  \
tests/ref/sxpdbi \
tests/sxhash.scm \
tests/ref/sxhash  \
tests/sxpgs.scm  \
tests/ref/sxpgs.ps  \
tests/sxoper.scm \
tests/ref/pdbvbatch \
tests/pdbvtest.scm \
tests/ref/pdbvtest.ps \
tests/pdbvinterp.scm \
tests/pdbdtu.scm


bin_PROGRAMS = sx
lib_LTLIBRARIES = libsx.la
include_HEADERS = sx.h


sx_SOURCES = sxmain.c $(include_HEADERS)
sx_LDADD = $(SPOKE_LIBS)
sx_LDFLAGS = -L$(ac_x_libraries) -lX11 libsx.la $(ADD_LD_FLAGS)
sx_DEPENDENCIES = libsx.la $(SPOKE_LIBS)

libsx_la_LDFLAGS = $(ADD_LD_FLAGS)
libsx_la_SOURCES = \
sxhook.c \
sxmm.c   \
sxgc.c   \
sxmode.c \
sxhand.c \
sxfunc.c \
sxhbo.c  \
sxpdb.c  \
sxpdba.c \
sxpdbr.c \
sxpdbc.c \
sxpdbd.c \
sxpdbf.c \
sxpdbl.c \
sxpan.c  \
sxpanw.c \
sxpml.c  \
sxgri.c  \
sxpgs.c  \
sxpgsp.c \
sxgrot.c \
sxshar.c \
sxset.c  \
sxcont.c \
sxulio.c \
sxcrv.c  

TESTS = $(sourcedir)/sxtest
