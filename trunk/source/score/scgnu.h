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

/* handle the cases:
 *   1) GNU C versions before 4.x when full C99 support came along
 *      e.g. Solaris 11 with GCC 3.4.3
 *   2) an incomplete installation of 4.x with no complex.h
 *      e.g. Cygwin 1.7
 */

#ifndef __cplusplus

# if (__GNUC__ < 4)
#  undef complex
#  undef I
/* typedef unsigned int u_int32_t; */
# endif

# ifndef complex
#  define complex _Complex
# endif
#

# ifndef I
#  define I 	(__extension__ 1.0iF)
# endif

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

#ifndef HAVE_GNU_LIBC_6

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
