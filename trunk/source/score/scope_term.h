/*
 * SCOPE_TERM.H - define TERM scope for portable terminal handling
 *              - aim at using POSIX standard termios controls
 *              - avoid older SYSV (termio) or BSD (sgttyb) protocols
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_TERM

#define PCK_SCOPE_TERM

#include "cpyright.h"

#define SIZE_BUF   0x20000

#define SC_ESC_CHAR   '\033'

/*--------------------------------------------------------------------------*/

/*                          CLIENT/SERVER SUPPORT                           */

/*--------------------------------------------------------------------------*/

#ifndef INADDR_NONE
# define INADDR_NONE -1
#endif

/*--------------------------------------------------------------------------*/

/*                     INTERRUPT DRIVEN I/O HANDLING                        */

/*--------------------------------------------------------------------------*/

#ifndef HAVE_STREAMS_P
# ifndef POLLIN
#  define POLLIN    0x0001
# endif
# ifndef POLLPRI
#  define POLLPRI   0x0002
# endif
# ifndef POLLOUT
#  define POLLOUT   0x0004
# endif
# ifndef POLLERR
#  define POLLERR   0x0008
# endif
# ifndef POLLHUP
#  define POLLHUP   0x0010
# endif
# ifndef POLLNVAL
#  define POLLNVAL  0x0020
# endif
# ifdef FASYNC
#  define HAVE_INTERRUPTS_P
# endif
#else
# define HAVE_INTERRUPTS_P
# define SC_poll poll
#endif

/*--------------------------------------------------------------------------*/

/*                                PTY HANDLING                              */

/*--------------------------------------------------------------------------*/

#ifdef SGI
# define SC_MASTER_PTY_NAME      "ptc"
# define SC_MASTER_PTY_LETTERS   NULL
# define SC_MASTER_PTY_DIGITS    "1234"
# define SC_SLAVE_PTY_NAME       "ttyp"
# define SC_SLAVE_PTY_LETTERS    "dfmq"
# define SC_SLAVE_PTY_DIGITS     "0123456789"
#endif

#ifndef SC_MASTER_PTY_NAME
# define SC_MASTER_PTY_NAME      "pty"
# define SC_MASTER_PTY_LETTERS   "pqrstuvwxyz"
# define SC_MASTER_PTY_DIGITS    "0123456789abcdef"
# define SC_SLAVE_PTY_NAME       "tty"
# define SC_SLAVE_PTY_LETTERS    "pqrstuvwxyz"
# define SC_SLAVE_PTY_DIGITS     "0123456789abcdef"
#endif

/*--------------------------------------------------------------------------*/

#if defined(UNIX)

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                            TERMINAL HANDLING                             */

/*--------------------------------------------------------------------------*/

#ifndef TERMINFO
# define TERMINFO struct termios
#endif

#ifndef TERMINAL
# define TERMINAL struct termios
#endif

/*--------------------------------------------------------------------------*/

/*                             TYPEDEFS AND STRUCTS                         */

/*--------------------------------------------------------------------------*/

#if defined(SOLARIS)

# ifndef _SOCKLEN_T
typedef int socklen_t;
# endif

#elif !defined(LINUX) && !defined(AIX) && !defined(HPUX) && !defined(MACOSX) && !defined(FREEBSD)

# ifdef GETSOCKNAME_LONG
typedef unsigned long socklen_t;
# else
#  if !defined(SGI) && !defined(BEOS)
typedef int socklen_t;
#  endif
# endif

#endif

typedef struct s_TERMINAL_STATE TERMINAL_STATE;

struct s_TERMINAL_STATE
   {int fd;
    int full_info;
    int valid_size;
    TERMINAL term;
    struct winsize window_size;

#ifdef BSD_TERMINAL
    struct tchars tch;
    struct ltchars ltc;
    int localmode;
    int discipline;
#endif

   };    

/*--------------------------------------------------------------------------*/

#elif defined(MSW)

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

enum e_SC_tcp_oper
   {SC_GET_PORT,
    SC_GET_CONNECTION};

typedef enum e_SC_tcp_oper SC_tcp_oper;

#ifndef SC_SIGIO
# define SC_SIGIO -1
#endif

#ifdef FASYNC
# define SC_ASYNC     FASYNC
#else
# define SC_ASYNC     -1
#endif

#define SC_NDELAY    O_NDELAY
#define SC_APPEND    FAPPEND
#define SC_SYNC      FSYNC

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

/* SCTERM.C declarations */

extern int
 SC_get_term_attr(char *cmd, char *rsp, int n, int *val),
 SC_line_buffer_fd(int fd),
 SC_line_buffer_file(FILE *fp),
 SC_char_buffer_fd(int fd),
 SC_char_buffer_file(FILE *fp),
 SC_is_line_buffered_fd(int fd),
 SC_is_line_buffered_file(FILE *fp);


#ifdef __cplusplus
}
#endif

#endif

