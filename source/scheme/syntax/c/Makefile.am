#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = scheme/syntax/c

include $(top_srcdir)/Makefile.common


CLEANFILES = shlrc.c shgrc.c

AM_CFLAGS = -I$(top_builddir) -I$(top_builddir)/config \
-I$(top_srcdir)/scheme -I$(top_srcdir)/pml -I$(top_srcdir)/score \
-I$(top_srcdir)/ppc -I$(top_srcdir)/pdb 
 
EXTRA_DIST = \
shlrc.l \
shgrc.y \
csynt.scm

dist-hook:
	rm -f $(distdir)/shlrc.c
	rm -f $(distdir)/shgrc.c


scheme_SCRIPTS = csynt.scm

noinst_LTLIBRARIES = libsyntaxc.la
libsyntaxc_la_SOURCES = shc.h shcsup.c shgrc.c
libsyntaxc_la_DEPENDENCIES = shlrc.c

shlrc.c: shlrc.l
	$(LEX) -t $(top_srcdir)/scheme/syntax/c/shlrc.l 2> /dev/null | sed -e "s/lex.yy.c/$@/g" | sed -e "s/yy/shlrc_/g" > $@
 
shgrc.c: shgrc.y
	${YACC} $(top_srcdir)/scheme/syntax/c/shgrc.y 2> /dev/null
	sed -e "s/y.tab.c/$@/g" y.tab.c | sed -e "s/yy/shgrc_/g" > $@
	rm y.tab.c

# MUST do dependencies *first*
libsyntaxc.la: $(libsyntaxc_la_DEPENDENCIES) $(libsyntaxc_la_OBJECTS) 
	$(LINK) -rpath $(libdir) $(libsyntaxc_la_LDFLAGS) $(libsyntaxc_la_OBJECTS) $(libsyntaxc_la_LIBADD) $(LIBS) 

