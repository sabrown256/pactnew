/*
 * SCLNX.H - LINUX support header for PACT codes
 *         - NOTE: Linux has __i386__, __alpha__, __mips__, __sparc__
 *         - predefined for these processors
 *         - these can be used inside '#ifdef LINUX' clauses to
 *         - distinguish which flavor of Linux is relevant
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef LINUX

#ifndef PCK_LINUX

#define PCK_LINUX

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                         STANDARD CONSTANT MACROS                         */

/*--------------------------------------------------------------------------*/

#ifndef _LARGE_FILES
# define _LARGE_FILES
#endif

#ifndef IBM_BGL
# ifndef _FILE_OFFSET_BITS
#  define _FILE_OFFSET_BITS 64
# endif
#endif

#ifndef _LARGEFILE64_SOURCE
# define _LARGEFILE64_SOURCE 1
#endif

#ifndef _LARGEFILE_SOURCE
# define _LARGEFILE_SOURCE 1
#endif

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

# ifdef ISO_C99

extern int
 getpagesize(void);

# endif

# ifdef HAVE_GNU_LIBC_6

#  include <sys/wait.h>

# else

extern int
 bsd_ioctl(int fildes, int request, ...);

# endif

#endif

#ifdef __cplusplus
}
#endif

#endif

#endif

