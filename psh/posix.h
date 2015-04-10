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

