#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = scheme/syntax/m

include $(top_srcdir)/Makefile.common


AM_CFLAGS = -I$(top_builddir) -I$(top_builddir)/config \
-I$(top_srcdir)/scheme -I$(top_srcdir)/pml -I$(top_srcdir)/score \
-I$(top_srcdir)/ppc -I$(top_srcdir)/pdb


scheme_SCRIPTS = msynt.scm

CLEANFILES = shlrm.c shgrm.c

EXTRA_DIST = \
shlrm.l \
shgrm.y \
msynt.scm

dist-hook:
	rm -f $(distdir)/shlrm.c
	rm -f $(distdir)/shgrm.c


noinst_LTLIBRARIES = libsyntaxm.la
libsyntaxm_la_SOURCES = shm.h shmsup.c shgrm.c
libsyntaxm_la_DEPENDENCIES = shlrm.c


shlrm.c: shlrm.l
	$(LEX) -t $(top_srcdir)/scheme/syntax/m/shlrm.l 2> /dev/null | sed -e "s/lex.yy.c/$@/g" | sed -e "s/yy/shlrm_/g" > $@

shgrm.c: shgrm.y
	${YACC} $(top_srcdir)/scheme/syntax/m/shgrm.y 2> /dev/null
	sed -e "s/y.tab.c/$@/g" y.tab.c | sed -e "s/yy/shgrm_/g" > $@
	rm y.tab.c

# MUST do dependencies *first* 
libsyntaxm.la: $(libsyntaxm_la_DEPENDENCIES) $(libsyntaxm_la_OBJECTS)
	$(LINK) -rpath $(libdir) $(libsyntaxm_la_LDFLAGS) $(libsyntaxm_la_OBJECTS) $(libsyntaxm_la_LIBADD) $(LIBS)

