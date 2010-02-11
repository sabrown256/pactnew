#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = scheme
testdirectory = $(builddir)/tests/check 

include $(top_srcdir)/Makefile.common


CLEANFILES = shlrc.c shgrc.c shlrf.c shgrf.c shlrm.c shgrm.c

if DO_SYNTAX
  DIRS = applications
endif

SUBDIRS = $(DIRS)

AM_CFLAGS = @COptimize@ -I$(top_srcdir)/scheme -I$(top_builddir) -I$(top_builddir)/config \
-I$(top_srcdir)/ppc -I$(top_srcdir)/pdb -I$(top_srcdir)/pml -I$(top_srcdir)/score
 
ADD_LD_FLAGS = \
${top_builddir}/ppc/libppc.la \
${top_builddir}/pdb/libpdb.la \
${top_builddir}/pml/libpml.la \
${top_builddir}/score/libscore.la \
-lm


EXTRA_DIST = \
DISCLAIMER \
scheme.FAQ \
shtest \
tests/shcdcl.c \
tests/shcppts.c \
tests/shcppts.h \
tests/shftst.f \
tests/synt1.m \
tests/synt2.m \
tests/mmtst.scm  \
tests/shproc.scm  \
tests/shctst.c  \
tests/shmtst.m \
tests/shnum.scm \
tests/shstr.scm \
tests/shenv.scm \
tests/ref/shcdcl \
tests/ref/shcppts \
tests/ref/shctst \
tests/ref/shmtst \
tests/ref/shproc \
tests/ref/shenv \
shlrc.l \
shgrc.y \
shlrf.l \
shgrf.y \
shlrm.l \
shgrm.y 


bin_PROGRAMS = scheme
lib_LTLIBRARIES = libscheme.la

if DO_SYNTAX
  noinst_LTLIBRARIES = libsyntaxc.la libsyntaxm.la libsyntaxf.la
endif

include_HEADERS = scheme.h

scheme_SOURCES = shmain.c 
scheme_LDFLAGS = -static 
scheme_LDADD = libscheme.la -lm -lc 
scheme_DEPENDENCIES = libscheme.la


if DO_SYNTAX
  libscheme_la_LIBADD = \
  $(top_builddir)/scheme/shcsup.lo \
  $(top_builddir)/scheme/shgrc.lo \
  $(top_builddir)/scheme/shfsup.lo \
  $(top_builddir)/scheme/shgrf.lo \
  $(top_builddir)/scheme/shmsup.lo \
  $(top_builddir)/scheme/shgrm.lo
endif

if DO_SYNTAX
libscheme_la_DEPENDENCIES = libsyntaxc.la libsyntaxf.la libsyntaxm.la
endif

libscheme_la_LDFLAGS = $(ADD_LD_FLAGS)
libscheme_la_SOURCES = \
shenvr.c \
sheval.c \
shint.c \
shlarg.c \
shmm.c \
shprm1.c \
shprm2.c \
shprm3.c \
shprnt.c \
shproc.c \
shread.c \
shstrg.c \
shsynt.c \
shtlev.c \
shtty.c \
shvar.c \
shchr.c \
$(include_HEADERS)


# Remove generated files from the distribution tarball
dist-hook:
	rm -f $(distdir)/shlrc.c
	rm -f $(distdir)/shgrc.c
	rm -f $(distdir)/shlrf.c
	rm -f $(distdir)/shgrf.c
	rm -f $(distdir)/shlrm.c
	rm -f $(distdir)/shgrm.c
        
if DO_SYNTAX
libsyntaxc_la_SOURCES = shgrc.c shcsup.c shc.h 

shgrc.c: shgrc.y shlrc.l
	$(LEX) -t $(sourcedir)/shlrc.l 2> /dev/null | sed -e "s/lex.yy.c/shlrc.c/g" | sed -e "s/yy/shlrc_/g" > shlrc.c
	${YACC} -b shgrc $(sourcedir)/shgrc.y 2> /dev/null
	sed -e "s/shgrc.tab.c/shgrc.c/g" shgrc.tab.c | sed -e "s/yy/shgrc_/g" > shgrc.c 
	rm -f shgrc.tab.c

libsyntaxf_la_SOURCES = shgrf.c shfsup.c shf.h 

shgrf.c: shgrf.y shlrf.l
	$(LEX) -t $(sourcedir)/shlrf.l 2> /dev/null | sed -e "s/lex.yy.c/shlrf.c/g" | sed -e "s/yy/shlrf_/g" > shlrf.c
	${YACC} -b shgrf $(sourcedir)/shgrf.y 2> /dev/null
	sed -e "s/shgrf.tab.c/shgrf.c/g" shgrf.tab.c | sed -e "s/yy/shgrf_/g" > shgrf.c 
	rm -f shgrf.tab.c


libsyntaxm_la_SOURCES = shgrm.c shmsup.c shm.h  

shgrm.c: shgrm.y shlrm.l
	$(LEX) -t $(sourcedir)/shlrm.l 2> /dev/null | sed -e "s/lex.yy.c/shlrm.c/g" | sed -e "s/yy/shlrm_/g" > shlrm.c
	${YACC} -b shgrm $(sourcedir)/shgrm.y 2> /dev/null
	sed -e "s/shgrm.tab.c/shgrm.c/g" shgrm.tab.c | sed -e "s/yy/shgrm_/g" > shgrm.c
	rm -f shgrm.tab.c
endif

TESTS = $(top_srcdir)/scheme/shtest
