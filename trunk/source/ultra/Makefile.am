#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = ultra
testdirectory = $(builddir)/check 

include $(top_srcdir)/Makefile.common


SUBDIRS = applications

AM_CFLAGS = @CDebug@ -I$(sourcedir) -I$(top_builddir) -I$(top_builddir)/config \
-I$(top_srcdir)/sx -I$(top_srcdir)/panacea -I$(top_srcdir)/scheme \
-I$(top_srcdir)/ppc -I$(top_srcdir)/pml -I$(top_srcdir)/pdb \
-I$(top_srcdir)/pgs -I$(top_srcdir)/score

ADD_LD_FLAGS = $(LDFLAGS) \
${top_builddir}/sx/libsx.la \
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
ultra.FAQ \
ultest \
ulfft.scm \
ulfnc.scm \
uliot.scm \
ulmmt.scm \
ulann.scm \
ultest.scm \
ultest.ps.ref \
ultra.ini


bin_PROGRAMS    = ultra
noinst_LTLIBRARIES = libultra.la
scheme_SCRIPTS = ultra.ini


libultra_la_SOURCES = \
ultra.h \
ulfunc.c \
ulauxf.c \
ulhand.c \
ulcntl.c 

ultra_SOURCES = ultra.c $(include_HEADERS) 
ultra_LDFLAGS = -static
ultra_LDADD = -L$(ac_x_libraries) -lX11 libultra.la $(ADD_LD_FLAGS) -lc
ultra_DEPENDENCIES = libultra.la

TESTS = $(sourcedir)/ultest 

