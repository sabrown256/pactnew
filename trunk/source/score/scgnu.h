/*
 * SCGNU.H - support header for GNU compilers
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef __GNUC__

#ifndef PCK_GNU

#define PCK_GNU

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                      STANDARD CONSTANT MACROS                            */

/*--------------------------------------------------------------------------*/

/* for GNU C versions before 4.x when full C99 support came along
 * this was done for 3.4.3
 */

#if (__GNUC__ < 4)
# include <complex.h>
# undef I
# ifndef I
#  define I 	(__extension__ 1.0iF)
# endif

typedef unsigned int u_int32_t;

#endif

#define _FORTIFY_SOURCE 2

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                          STANDARD PROCEDURAL MACROS                      */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                    STANDARD VARIABLE DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

#ifndef __cplusplus
# ifdef ISO_C99

extern int
 getpagesize(void);

# endif

/* older compilers in the 3.x series are missing these */
extern long double
 powl(long double x, long double y),
 expl(long double x),
 sqrtl(long double x);

#endif

#ifdef HAVE_GNU_LIBC_6

# include <sys/wait.h>

#else

extern int
 bsd_ioctl(int fildes, int request, ...);

#endif

/*--------------------------------------------------------------------------*/

/*                    STANDARD FUNCTION DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
