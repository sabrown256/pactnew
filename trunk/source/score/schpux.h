/*
 * SCHPUX.H - HPUX support header for PACT codes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef HPUX

#ifndef PCK_HPUX

#define PCK_HPUX

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                         STANDARD CONSTANT MACROS                         */

/*--------------------------------------------------------------------------*/

/* #define __YYSCLASS extern */
#undef RS

#define O_NDELAY  0000004

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                        STANDARD PROCEDURAL MACROS                        */

/*--------------------------------------------------------------------------*/

#define F77_FUNC(x, X)    x
#define F77_ID(x_, x, X)  x
#define FF_ID(x, X)       x


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                      STANDARD VARIABLE DECLARATIONS                      */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                      STANDARD FUNCTION DECLARATIONS                      */

/*--------------------------------------------------------------------------*/

extern char
 *strdup(const char *s);

extern int
 select(size_t nfds, int *readfds, int *writefds,
	int *exceptfds, const struct timeval *timeout),
 ioctl(int fildes, int request, ...);

extern void
 bzero(char *b, int length);


#ifdef __cplusplus
}
#endif

#endif

#endif

