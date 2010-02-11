#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

package = score
testdirectory = $(builddir)/check

include $(top_srcdir)/Makefile.common


NO_MPI_AM_FFLAGS = -I$(top_srcdir)/score -I$(top_builddir) -I$(top_builddir)/config
AM_FFLAGS = $(NO_MPI_AM_FFLAGS) @MPIFLAGS@
NO_MPI_AM_CFLAGS = @COptimize@ -I$(top_srcdir)/score -I$(top_builddir) -I$(top_builddir)/config
AM_CFLAGS = $(NO_MPI_AM_CFLAGS) @MPIFLAGS@
NO_MPI_LIBS = @LIBS@
LIBS = $(NO_MPI_LIBS) @MPILIBS@

ADD_LD_FLAGS=$(LDFLAGS) libscore.la -lm @LIBS@
ADD_MPI_LD_FLAGS=$(ADD_LD_FLAGS) @MPILIBS@

SC_CC=$(LIBTOOL_LINK) $(TAG_CC) $(CCConfig) $(CFLAGS) $(NO_MPI_AM_CFLAGS) 
SC_MPCC=$(LIBTOOL_LINK) $(TAG_CC) $(CCConfig) $(CFLAGS) $(AM_CFLAGS)
SC_F77=$(LIBTOOL_LINK)$(TAG_F77) $(F77) $(FFLAGS)


EXTRA_DIST = \
DISCLAIMER \
score.FAQ \
scmemi.c \
sctest \
fltfil \
intfil \
regxfil \
tokfil \
sclsts.res.ref \
scmpiwrap.res.ref \
sclsts.src \
scf77lr.l \
scompt.f \
scmm.c \
schtst.f \
tscstr.c \
tscctl.c \
tscmemc.c \
interact.c \
scatst.c \
sclsts.c \
sclsts.c \
scmtrt.c \
scthrt.c \
sctrts.c \
scmfts.c \
scprts.c \
scarsc.c \
scsmp.c \
scerts.c \
scsmpb.c \
scpsd.c \
sctty.c \
scmkut.c \
schsru.c \
scrp.c \
scncpu.c

CLEANFILES = foo scf77lr.c sctty ps-diff dmake pact hserve \
sctrts tscstr tscctl tscmemc interact scatst sclsts sclsts \
scmtrt scthrt scerts scmfts smprts fcscan


bin_PROGRAMS = mpi-io-wrap 
lib_LTLIBRARIES = libscore.la libpsmm.la
include_HEADERS = scstd.h score.h scope_hash.h sclexan.h $(SCOPEHEADERS_INSTALL)

SCOPEHEADERS_INSTALL = scope_mem.h scope_mpi.h scope_proc.h scope_term.h 
SCOPEHEADERS_LOCAL   = scope_make.h scope_mmap.h  
SCOPEHEADERS         = $(SCOPEHEADERS_INSTALL) $(SCOPEHEADERS_LOCAL)

mpi_io_wrap_SOURCES = scwrap.c
mpi_io_wrap_LDADD = libscore.la -lm -lc

libpsmm_la_SOURCES = scmemr.c
 
libscore_la_LDFLAGS = -lm
libscore_la_SOURCES = \
$(SCOPEHEADERS) \
scmemc.c \
scmemda.c \
scmemdb.c \
scmemf.c \
scmemg.c \
scpman.c \
sctyp.c \
scctl.c \
scerr.c \
scfia.c \
scfio.c \
scfis.c \
scfnca.c \
scfncb.c \
schash.c \
schsrv.c \
scstr.c \
schp.c \
scprts.c \
schttp.c \
scioctl.c \
sctree.c \
sclmmap.c \
sclppsx.c \
scmkdp.c \
scmkpr.c \
scmmap.c \
scmmaps.c \
scterm.c \
scpar.c \
scsprc.c \
sctrk.c \
sclog.c \
scwin32.c \
scsysa.c \
scsysb.c \
scsysc.c \
scsysd.c \
scf77lr.c 


BINARIES = ps-diff dmake hserve rpath ncpu sctty
all-local: $(BINARIES)

install-exec-local:
	cp $(BINARIES) $(bindir)
	rm -f $(bindir)/pact
	ln -s $(bindir)/dmake $(bindir)/pact

dist-hook:
	rm -f $(distdir)/scf77lr.c

  

# ptime: sctime.c 
# dmnz:  scdmnz.c 
# scmm:  scmm.c
      
scf77lr.c: scf77lr.l
	$(LEX) -t $(top_srcdir)/score/scf77lr.l | sed -e "s|lex.yy.c|scf77lr.c|" | sed -e "s/yy/scf77lr_/g" > $(top_builddir)/score/scf77lr.c

ps-diff: scpsd.c libscore.la
	@rm -f ps-diff
	$(SC_CC) $(sourcedir)/scpsd.c -o $(builddir)/ps-diff $(ADD_LD_FLAGS)

rpath: scrp.c libscore.la
	@rm -f rpath
	$(SC_CC) $(sourcedir)/scrp.c -o $(builddir)/rpath $(ADD_LD_FLAGS)

dmake: scmkut.c libscore.la
	@rm -f dmake
	$(SC_CC) $(sourcedir)/scmkut.c -o $(builddir)/dmake $(ADD_LD_FLAGS)

hserve: schsru.c libscore.la
	@rm -f hserve
	$(SC_CC) $(sourcedir)/schsru.c -o $(builddir)/hserve $(ADD_LD_FLAGS)
        
ncpu: scncpu.c libscore.la
	@rm -f ncpu
	$(SC_CC) $(sourcedir)/scncpu.c -o $(builddir)/ncpu $(ADD_LD_FLAGS)

sctty: sctty.c libscore.la
	@rm -f sctty
	$(SC_MPCC) $(sourcedir)/sctty.c -o $(builddir)/sctty $(ADD_MPI_LD_FLAGS)

# Targets for sctest
compre: tscstr tscctl tscmemc interact 
mpitty: mpi-io-wrap sctty
ha: scatst sctrts 
scan: sclsts
memchk: scmtrt
smp: scthrt scsmp scsmpb
fha: schtst
mm: scmm
omp: scompt
error: scerts
mf: scmfts
print: scprts
fcscan: fcscan

TESTS = $(top_srcdir)/score/sctest 

scerts: $(sourcedir)/scerts.c libscore.la
	@rm -f $(builddir)/scerts
	$(SC_CC) $(sourcedir)/scerts.c -o $(builddir)/scerts $(ADD_LD_FLAGS)

scmm: $(sourcedir)/scmm.c libscore.la
	@rm -f $(builddir)/scmm
	$(SC_CC) $(sourcedir)/scmm.c -o $(builddir)/scmm $(ADD_LD_FLAGS)

scompt: $(sourcedir)/scompt.f libscore.la
	@rm -f $(builddir)/scompt
	$(SC_F77) $(sourcedir)/scompt.f -o $(builddir)/scompt $(ADD_LD_FLAGS)

schtst: $(sourcedir)/schtst.f libscore.la
	@rm -f $(builddir)/schtst
	$(SC_F77) $(sourcedir)/schtst.f -o $(builddir)/schtst $(ADD_LD_FLAGS)

tscstr: $(sourcedir)/tscstr.c libscore.la
	@rm -f $(builddir)/tscstr
	$(SC_CC) $(sourcedir)/tscstr.c -o $(builddir)/tscstr $(ADD_LD_FLAGS)

tscctl: $(sourcedir)/tscctl.c libscore.la
	@rm -f $(builddir)/tscctl
	$(SC_CC) $(sourcedir)/tscctl.c -o $(builddir)/tscctl $(ADD_LD_FLAGS)

tscmemc: $(sourcedir)/tscmemc.c libscore.la
	@rm -f $(builddir)/tscmemc
	$(SC_CC) $(sourcedir)/tscmemc.c -o $(builddir)/tscmemc $(ADD_LD_FLAGS)

interact: $(sourcedir)/interact.c libscore.la
	@rm -f $(builddir)/interact
	$(SC_CC) $(sourcedir)/interact.c -o $(builddir)/interact $(ADD_LD_FLAGS)

scatst: $(sourcedir)/scatst.c libscore.la
	@rm -f $(builddir)/scatst
	$(SC_CC) $(sourcedir)/scatst.c -o $(builddir)/scatst $(ADD_LD_FLAGS)

sclsts: $(sourcedir)/sclsts.c libscore.la
	@rm -f $(builddir)/sclsts
	$(SC_CC) $(sourcedir)/sclsts.c -o $(builddir)/sclsts $(ADD_LD_FLAGS)

scmtrt: $(sourcedir)/scmtrt.c libscore.la
	@rm -f $(builddir)/scmtrt
	$(SC_CC) $(sourcedir)/scmtrt.c -o $(builddir)/scmtrt $(ADD_LD_FLAGS) -lpsmm

scthrt: $(sourcedir)/scthrt.c libscore.la
	@rm -f $(builddir)/scthrt
	$(SC_CC) $(sourcedir)/scthrt.c -o $(builddir)/scthrt $(ADD_LD_FLAGS)

sctrts: $(sourcedir)/sctrts.c libscore.la
	@rm -f $(builddir)/sctrts
	$(SC_CC) $(sourcedir)/sctrts.c -o $(builddir)/sctrts $(ADD_LD_FLAGS)

scsmp: $(sourcedir)/scsmp.c libscore.la
	@rm -f $(builddir)/scsmp
	$(SC_CC) $(sourcedir)/scsmp.c -o $(builddir)/scsmp $(ADD_LD_FLAGS)

scsmpb: $(sourcedir)/scsmpb.c libscore.la
	@rm -f $(builddir)/scsmpb
	$(SC_CC) $(sourcedir)/scsmpb.c -o $(builddir)/scsmpb $(ADD_LD_FLAGS)

scmfts: $(sourcedir)/scmfts.c libscore.la
	@rm -f $(builddir)/scmfts
	$(SC_CC) $(sourcedir)/scmfts.c -o $(builddir)/scmfts $(ADD_LD_FLAGS)

scprts: $(sourcedir)/scprts.c libscore.la
	@rm -f $(builddir)/scprts
	$(SC_CC) $(sourcedir)/scprts.c -o $(builddir)/scprts $(ADD_LD_FLAGS)

fcscan: $(sourcedir)/scarsc.c libscore.la
	@rm -f $(builddir)/fcscan
	$(SC_CC) $(sourcedir)/scarsc.c -o $(builddir)/fcscan $(ADD_LD_FLAGS)
