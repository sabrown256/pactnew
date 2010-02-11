#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

AUTOMAKE_OPTIONS = no-dependencies

AM_CFLAGS = -I${top_builddir}/config \
-I${top_builddir}panacea/abc/include -I${top_srcdir}/sx \
-I${top_srcdir}/panacea -I${top_srcdir}/scheme \
-I${top_srcdir}/ppc -I${top_srcdir}/pgs -I${top_srcdir}/pml \
-I${top_srcdir}/pdb -I${top_srcdir}/score \
-I../global

noinst_LTLIBRARIES = liblag.la
liblag_la_SOURCES = lag.c lagin.c lagin.h
liblag_la_LIBADD = ${top_builddir}/sx/libsx.la \
${top_builddir}/panacea/libpanacea.la \
${top_builddir}/scheme/libscheme.la \
${top_builddir}/ppc/libppc.la \
${top_builddir}/pgs/libpgs.la \
${top_builddir}/pml/libpml.la \
${top_builddir}/pdb/libpdb.la \
${top_builddir}/score/libscore.la \
${top_builddir}/panacea/abc/global/libmesh.la

