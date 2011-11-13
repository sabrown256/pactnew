/*
 * SCBSD.H - (Free)BSD support header for PACT codes
 *         - these can be used inside '#ifdef FREEBSD' clauses to
 *         - distinguish which flavor of FreeBSD is relevant
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef FREEBSD

#ifndef PCK_FREEBSD

#define PCK_FREEBSD

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                         STANDARD CONSTANT MACROS                         */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                        STANDARD PROCEDURAL MACROS                        */

/*--------------------------------------------------------------------------*/


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                      STANDARD VARIABLE DECLARATIONS                      */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                      STANDARD FUNCTION DECLARATIONS                      */

/*--------------------------------------------------------------------------*/

#ifdef __GNUC__

/*--------------------------------------------------------------------------*/

# ifndef __cplusplus
#  ifdef ISO_C99

extern int
 getpagesize(void);

#  endif

/* older compilers in the 3.x series are missing these */
extern long double
 powl(long double x, long double y),
 expl(long double x),
 sqrtl(long double x);

# endif

# ifdef HAVE_GNU_LIBC_6

#  include <sys/wait.h>

# else

extern int
 bsd_ioctl(int fildes, int request, ...);

# endif

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif

#endif

