#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#

#include "cpyright.h"

AUTOMAKE_OPTIONS = no-dependencies

# ORDERING
SUBDIRS = . global lag misc source main 

# CLEAN
CLEANFILES = \
misc/misc.c \
misc/misc.h \
lag/lag.c \
lag/lag.h \
global/global.c \
global/global.h \
main/b.h \
main/b.c \
main/a.c 

# EXPLICIT RULES
all-local: $(top_srcdir)/panacea/abc/abc.dict
	if test "${top_srcdir}" != "${top_builddir}"; \
	then \
	  cp -f -R ${top_srcdir}/panacea/abc/abc.dict . ; \
	  cp -f -R ${top_srcdir}/panacea/abc/global/*.[ch] ./global/ ; \
	  cp -f -R ${top_srcdir}/panacea/abc/lag/*.[ch] ./lag/ ; \
	  cp -f -R ${top_srcdir}/panacea/abc/main/*.[ch] ./main/ ; \
	  cp -f -R ${top_srcdir}/panacea/abc/misc/*.[ch] ./misc/ ; \
	  cp -f -R ${top_srcdir}/panacea/abc/source/*.[ch] ./source/ ; \
	fi 
	cp ${top_builddir}/sx/sx . 
	cp ${top_srcdir}/panacea/applications/panache . 
	panache abc.dict 
	rm ./sx
	rm ./panache
	mv a.c b.c b.h ./main/
	mv global.h ./global/global.h
	mv global.c ./global/global.c 
	mv lag.h ./lag/lag.h 
	mv lag.c ./lag/lag.c 
	mv misc.h ./misc/misc.h 
	mv misc.c ./misc/misc.c 

