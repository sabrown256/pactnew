/*
 * SCUNIX.H - UNIX support header for PACT codes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef UNIX

#ifndef PCK_UNIX

#define PCK_UNIX

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                          STANDARD INCLUDES                               */

/*--------------------------------------------------------------------------*/

#if defined(AIX)
# ifdef HAVE_SELECT
#  include <sys/select.h>
# endif
# define BAD_FLUSH_SEMANTICS
#endif

#if defined(MACOSX)
# include <term.h>
# include <sys/ioctl_compat.h>
# define DISABLE_STROPTS

/* NOTE: OSX has broken termios (POSIX)
 * you get bus errors using termios and select calls
 * you also do not get a true raw mode with termios only with sgttyb
 */
# define BSD_TERMINAL
# define TERMINAL struct sgttyb

# undef tab
# undef newline
# undef buttons
# undef lines
# undef bell
# undef user1

#endif

#if defined(CYGWIN)
# define DISABLE_STROPTS
#endif

#if defined(FREEBSD)
# include <term.h>
# undef tab
#endif

#if defined(BEOS)
# define SIGIO SIGPOLL
#endif

#if defined(SOLARIS)
# include <sys/file.h>
# define BAD_FLUSH_SEMANTICS
#endif

#if defined(SGI)
# define _SOCKLEN_T
typedef int socklen_t;
#endif

#ifdef HAVE_STREAMS
# ifdef HAVE_ASYNC_STREAMS
#  include <stropts.h>
# endif
# include <poll.h>
# define SC_SIGIO SIGPOLL
#endif

#ifdef HAVE_SELECT
# undef KERNEL
# ifndef SC_SIGIO
#  define SC_SIGIO SIGIO
# endif
#endif

#include <sched.h>

/*--------------------------------------------------------------------------*/

/*                           STANDARD PRAGMAS                               */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                      STANDARD CONSTANT MACROS                            */

/*--------------------------------------------------------------------------*/

#define directory_delim "/"
#define directory_delim_c '/'

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                                SETJMP MACROS                             */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                          STANDARD PROCEDURAL MACROS                      */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                    STANDARD VARIABLE DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                    STANDARD FUNCTION DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif

#endif
