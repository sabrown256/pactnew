#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = scheme/syntax/f

include $(top_srcdir)/Makefile.common


AM_CFLAGS = -I$(top_builddir) -I$(top_builddir)/config \
-I$(top_srcdir)/scheme -I$(top_srcdir)/pml -I$(top_srcdir)/score \
-I$(top_srcdir)/ppc -I$(top_srcdir)/pdb


scheme_SCRIPTS = fsynt.scm

CLEANFILES = shlrf.c shgrf.c

EXTRA_DIST = \
shlrf.l \
shgrf.y \
fsynt.scm

dist-hook:
	rm -f $(distdir)/shlrf.c
	rm -f $(distdir)/shgrf.c


noinst_LTLIBRARIES = libsyntaxf.la
libsyntaxf_la_SOURCES = shf.h shfsup.c shgrf.c
libsyntaxf_la_DEPENDENCIES = shlrf.c


shlrf.c: shlrf.l
	$(LEX) -t $(top_srcdir)/scheme/syntax/f/shlrf.l 2> /dev/null | sed -e "s/lex.yy.c/$@/g" | sed -e "s/yy/shlrf_/g" > $@

shgrf.c: shgrf.y
	${YACC} $(top_srcdir)/scheme/syntax/f/shgrf.y 2> /dev/null
	sed -e "s/y.tab.c/$@/g" y.tab.c | sed -e "s/yy/shgrf_/g" > $@
	rm y.tab.c

# MUST do dependencies *first*
libsyntaxf.la: $(libsyntaxf_la_DEPENDENCIES) $(libsyntaxf_la_OBJECTS)
	$(LINK) -rpath $(libdir) $(libsyntaxf_la_LDFLAGS) $(libsyntaxf_la_OBJECTS) $(libsyntaxf_la_LIBADD) $(LIBS)

