#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = pgs
testdirectory = $(builddir)/check

include $(top_srcdir)/Makefile.common


if USE_INC
  usrinclude = -I/usr/include
else
  usrinclude = 
endif

if USE_OPENGL
  libpgsGraphicsLib = -lGL
else 
  libpgsGraphicsLib = 
endif

AM_CFLAGS = @CDebug@ -I$(sourcedir) -I$(top_builddir)/config \
-I$(top_srcdir)/score -I$(top_srcdir)/pml -I$(top_srcdir)/ppc -I$(top_srcdir)/pdb \
-I$(ac_x_includes) ${usrinclude}

ADD_LD_FLAGS = $(LDFLAGS) libpgs.la \
${top_builddir}/ppc/libppc.la \
${top_builddir}/pdb/libpdb.la \
${top_builddir}/pml/libpml.la \
${top_builddir}/score/libscore.la \
-lm


EXTRA_DIST = \
DISCLAIMER \
pgs.FAQ \
gstest \
gslaxt.dat \
ref/gslaxt \
ref/gslaxt.ps \
ref/gspsta.ps \
ref/gspsts.ps 


lib_LTLIBRARIES = libpgs.la
include_HEADERS = pgs.h scope_raster.h scope_cgm.h 


libpgs_la_LDFLAGS = \
$(top_builddir)/ppc/libppc.la \
$(top_builddir)/pdb/libpdb.la \
$(top_builddir)/pml/libpml.la \
$(top_builddir)/score/libscore.la \
-L$(ac_x_libraries) -lX11 $(libpgsGraphicsLib) \
-lm -lc
libpgs_la_SOURCES = \
scope_mpeg.h \
gsdv_jpg.c \
gsdv_png.c \
gsdv_ps.c \
gspr_ps.c \
gsdv_cgm.c \
gspr_cgm.c \
gsdv_mpg.c \
gspr_mpg.c \
gsdv_X.c \
gspr_X.c \
gstx_X.c \
gsdv_im.c \
gsdv_rst.c \
gspr_rst.c \
gsdv.c \
gspr.c \
gsmpeg.c \
gsaux.c \
gsattr.c \
gsmm.c \
gshigh.c \
gsrend.c \
gsiob.c \
gsiod.c \
gsrwi.c \
gstxt.c \
gsdplt.c \
gsaxis.c \
gsdlin.c \
gscrv.c \
gscntr.c \
gsncnt.c \
gsvect.c \
gsimag.c \
gssurf.c \
gsdvb.c \
gspolf.c \
gsgrot.c \
gshls.c \
gsscat.c \
gsclr.c \
gsfia.c \
gsfib.c \
$(include_HEADERS) 


# Targets for gstest
Cstd: gsatst
logaxis: gslaxt
grotrian : gsgrts

noinst_PROGRAMS = gsatst gslaxt gsgrts gsclts gsctst gsevts gsfpts gsimts gsltst 
TESTS = $(top_srcdir)/pgs/gstest 

gsltst_SOURCES = gsltst.c
gsltst_LDFLAGS = $(ADD_LD_FLAGS)
gsltst_DEPENDENCIES = libpgs.la

gsimts_SOURCES = gsimts.c
gsimts_LDFLAGS = $(ADD_LD_FLAGS)
gsimts_DEPENDENCIES = libpgs.la

gsfpts_SOURCES = gsfpts.c
gsfpts_LDFLAGS = $(ADD_LD_FLAGS)
gsfpts_DEPENDENCIES = libpgs.la

gsevts_SOURCES = gsevts.c
gsevts_LDFLAGS = $(ADD_LD_FLAGS)
gsevts_DEPENDENCIES = libpgs.la

gsctst_SOURCES = gsctst.c
gsctst_LDFLAGS = $(ADD_LD_FLAGS)
gsctst_DEPENDENCIES = libpgs.la

gsclts_SOURCES = gsclts.c
gsclts_LDFLAGS = $(ADD_LD_FLAGS)
gsclts_DEPENDENCIES = libpgs.la

gsatst_SOURCES = gsatst.c
gsatst_LDFLAGS = $(ADD_LD_FLAGS)
gsatst_DEPENDENCIES = libpgs.la

gslaxt_SOURCES = gslaxt.c 
gslaxt_LDFLAGS = $(ADD_LD_FLAGS)
gslaxt_DEPENDENCIES = libpgs.la

gsgrts_SOURCES =  gsgrts.c
gsgrts_LDFLAGS = $(ADD_LD_FLAGS)
gsgrts_DEPENDENCIES = libpgs.la
