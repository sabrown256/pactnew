/*
 * POSIX-UNIX.H - header to provide a POSIX environment on UNIX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#ifndef PCK_POSIX_UNIX
#define PCK_POSIX_UNIX

#define HAVE_POSIX_SYS

/* NOTE: threading is handled elsewhere and BG/L
 * does not have this header
#include <pthread.h>
 */

/* #include <cpio.h> */
#include <grp.h>
#include <pwd.h>
#include <tar.h>
#include <termios.h>
#include <poll.h>
#include <fnmatch.h>
#include <wordexp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/resource.h>

/* include this here to control complications with non-standard ones */
#include <unistd.h>

/* not strictly POSIX but not ANSI C either */
#include <sys/ioctl.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* necessitated by MS non-standard intransigence */

#define LL_FMT  "%lld"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GCC predefined OS designation macros
 * and any non-standard or non-portable specifications
 */

/* Linux */

#ifdef __linux__
#endif

/* CYGWIN */

#ifdef __CYGWIN__
#define strtold strtod
#endif

/* FreeBSD */

#ifdef __FreeBSD__
# undef lines
# undef newline
# undef buttons
#endif

/* Solaris */

#ifdef __sun__

typedef uint64_t u_int64_t;
typedef uint32_t u_int32_t;
typedef uint16_t u_int16_t;
typedef uint8_t  u_int8_t;

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef __cplusplus

extern char
 *getcwd(char *buf, size_t size);

/* NOTE: not POSIX function but differs between compilers */
extern int
 strcasecmp(const char *a, const char *b);

extern int
 sched_yield(void);

#endif

#endif

