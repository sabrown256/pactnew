#
# Source Version: 3.0
# Software Release #: LLNL-CODE-422942
#
# include "cpyright.h"
#

FC     = pgf90
FFLAGS = 

.c.o:
	${CC} -g ${CFLAGS} -I.. -c $< -o $@

all : genfiles from_f from_c
	./genfiles

genfiles : genfiles.o
	${CC} -g ${CFLAGS} genfiles.o -o genfiles

from_f from_c : getbytes
	./getbytes

getbytes : csizer.o getbytes.f
	${FC} ${FFLAGS} getbytes.f csizer.o -o getbytes

getbytes.f : getkinds
	./getkinds > getbytes.f

getkinds : getkinds.f
	${FC} ${FFLAGS} getkinds.f -o getkinds

sizeint : sizeint.f
	${FC} ${FFLAGS} sizeint.f -o sizeint
	sizeint

clean:
	rm -f getkinds getbytes genfiles
	rm -f getbytes.f
	rm -f from_f from_c
	rm -f *.o  *.mod *.M core *~
