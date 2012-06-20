/*
 * SCWIN32.H - WIN32 support header
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef WIN32

#ifndef PCK_WIN32

#define PCK_WIN32

#include "cpyright.h"

#undef byte

#include <ws2tcpip.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>

#include <io.h>
#include <direct.h>
#include <process.h>

/*--------------------------------------------------------------------------*/

/*                        STANDARD CONFIGURATIONS                           */

/*--------------------------------------------------------------------------*/

/* NO_SHELL should be set for platforms lacking shell windows
 * this would include DOS and Windows
 */

#define NO_SHELL

#define directory_delim "\\"
#define directory_delim_c '\\'

#define _PC_PATH_MAX   1024

/* define a faux signal set */

#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGBUS		 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO
#define SIGPWR		30
#define SIGSYS		31
#define	SIGUNUSED	31

/*--------------------------------------------------------------------------*/

/*                          STANDARD PROCEDURAL MACROS                      */

/*--------------------------------------------------------------------------*/

#define JMP_BUF            jmp_buf
#define SETJMP(_x)         setjmp(_x)
#define LONGJMP(_x, _v)    longjmp(_x, _v)

#define fileno(x)          _fileno(x)
/* #define stat(_a, _b)       _stat(_a, _b) */

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

typedef int sigset_t;

struct flock
   {int l_type;
    int l_start;
    int l_whence;
    int l_len;
    int l_pid;};

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
