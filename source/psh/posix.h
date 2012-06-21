/*
 * POSIX.H - header to provide POSIX environment
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#ifndef PCK_POSIX
#define PCK_POSIX

#if 0

/* GOTCHA: we would like to do this but bootstrapping configuration
 * is a nightmare for now
  #include "iso-c.h"
 */

#define ISO_C99

/* introduced with C99 compiles */
#ifdef ISO_C99
# ifdef __GNUC__
#  define _POSIX_C_SOURCE   200112L
#  define _XOPEN_SOURCE     600
#  define _DARWIN_C_SOURCE
# endif
#endif

#endif

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <utime.h>
#include <setjmp.h>

#if defined(_WIN32)
# include "posix-msw.h"
#elif defined(BEOS)
# include "posix-beos.h"
#else
# include "posix-unix.h"
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

