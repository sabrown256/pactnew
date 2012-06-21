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

#include <unistd.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#if defined(AIX)
# include <termios.h>
# ifdef HAVE_SELECT_P
#  include <sys/select.h>
# endif
# define BAD_FLUSH_SEMANTICS
#elif defined(BEOS)
# include <termios.h>
#else
# include <sys/termios.h>
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

#ifdef HAVE_STREAMS_P
# ifdef HAVE_ASYNC_STREAMS
#  include <stropts.h>
# endif
# include <poll.h>
# define SC_SIGIO SIGPOLL
#endif

#ifdef HAVE_SELECT_P
# undef KERNEL
# ifndef SC_SIGIO
#  define SC_SIGIO SIGIO
# endif
#endif

#ifdef HAVE_SOCKETS_P
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netdb.h>
#endif

#include <pwd.h>
#include <sched.h>

/*--------------------------------------------------------------------------*/

/*                           STANDARD PRAGMAS                               */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                      STANDARD CONSTANT MACROS                            */

/*--------------------------------------------------------------------------*/

#define HAVE_POSIX_SYS

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
