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

#include <cpio.h>
#include <grp.h>
#include <pwd.h>
#include <tar.h>
#include <termios.h>
#include <poll.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/utsname.h>
#include <sys/wait.h>

/* include this here to control complications with non-standard ones */
#include <unistd.h>

/* not strictly POSIX but not ANSI C either */
#include <sys/ioctl.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

extern char
 *getcwd(char *buf, size_t size);

/* NOTE: not POSIX function but differs between compilers */
extern int
 strcasecmp(const char *a, const char *b);

/*
extern int
 sched_yield(void);
*/

#endif

