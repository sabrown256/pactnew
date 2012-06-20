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

#if defined(UNIX)

/*--------------------------------------------------------------------------*/

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

/* faux set of terminal constants */

#define OPOST	0000001
#define ICANON	0000002
#define ECHO	0000010
#define ECHOE	0000020
#define ECHOK	0000040

#define POLLIN		0x001
#define POLLPRI		0x002
#define POLLOUT		0x004
#define POLLERR		0x008		/* Error condition.  */
#define POLLHUP		0x010		/* Hung up.  */
#define POLLNVAL	0x020		/* Invalid polling request.  */

#define BRKINT	0000002
#define ICRNL	0000400
#define INPCK	0000020
#define ISTRIP	0000040
#define IXON	0002000
#define IEXTEN	0100000
#define ISIG	0000001
#define CSIZE	0000060
#define   CS8	0000060
#define PARENB	0000400
#define VMIN 6
#define VTIME 5

#define TOSTOP        0000400
#define TIOCGWINSZ     0x5413
#define TIOCSWINSZ     0x5414
#define F_GETFL             3

/* tcsetattr uses these */
#define	TCSANOW		0
#define	TCSADRAIN	1
#define	TCSAFLUSH	2


#define F_RDLCK		0
#define F_WRLCK		1
#define F_UNLCK		2
#define F_SETLKW	7

/* faux set of open mode constants */

#define O_NOCTTY	   0400
#define O_NONBLOCK	  04000
#define O_NDELAY	O_NONBLOCK
#define O_SYNC	       04010000
#define O_FSYNC		 O_SYNC
#define O_ASYNC		 020000

#if 0

/* Values for the second argument to `fcntl'.  */
#define F_DUPFD		0	/* Duplicate file descriptor.  */
#define F_GETFD		1	/* Get file descriptor flags.  */
#define F_SETFD		2	/* Set file descriptor flags.  */
#define F_GETFL		3	/* Get file status flags.  */
#define F_SETFL		4	/* Set file status flags.  */
#if __WORDSIZE == 64
# define F_GETLK	5	/* Get record locking info.  */
# define F_SETLK	6	/* Set record locking info (non-blocking).  */
/* Not necessary, we always have 64-bit offsets.  */
# define F_GETLK64	5	/* Get record locking info.  */
# define F_SETLK64	6	/* Set record locking info (non-blocking).  */
# define F_SETLKW64	7	/* Set record locking info (blocking).	*/
#else
# ifndef __USE_FILE_OFFSET64
#  define F_GETLK	5	/* Get record locking info.  */
#  define F_SETLK	6	/* Set record locking info (non-blocking).  */
#  define F_SETLKW	7	/* Set record locking info (blocking).	*/
# else
#  define F_GETLK	F_GETLK64  /* Get record locking info.	*/
#  define F_SETLK	F_SETLK64  /* Set record locking info (non-blocking).*/
#  define F_SETLKW	F_SETLKW64 /* Set record locking info (blocking).  */
# endif
# define F_GETLK64	12	/* Get record locking info.  */
# define F_SETLK64	13	/* Set record locking info (non-blocking).  */
# define F_SETLKW64	14	/* Set record locking info (blocking).	*/
#endif

#if defined __USE_BSD || defined __USE_UNIX98 || defined __USE_XOPEN2K8
# define F_SETOWN	8	/* Get owner (process receiving SIGIO).  */
# define F_GETOWN	9	/* Set owner (process receiving SIGIO).  */
#endif

#ifdef __USE_GNU
# define F_SETSIG	10	/* Set number of signal to be sent.  */
# define F_GETSIG	11	/* Get number of signal to be sent.  */
# define F_SETOWN_EX	15	/* Get owner (thread receiving SIGIO).  */
# define F_GETOWN_EX	16	/* Set owner (thread receiving SIGIO).  */
#endif

#ifdef __USE_GNU
# define F_SETLEASE	1024	/* Set a lease.	 */
# define F_GETLEASE	1025	/* Enquire what lease is active.  */
# define F_NOTIFY	1026	/* Request notfications on a directory.	 */
# define F_SETPIPE_SZ	1031	/* Set pipe page size array.  */
# define F_GETPIPE_SZ	1032	/* Set pipe page size array.  */
#endif
#ifdef __USE_XOPEN2K8
# define F_DUPFD_CLOEXEC 1030	/* Duplicate file descriptor with
				   close-on-exit set.  */
#endif

/* For F_[GET|SET]FD.  */
#define FD_CLOEXEC	1	/* actually anything with low bit set goes */

/* For posix fcntl() and `l_type' field of a `struct flock' for lockf().  */

/* For old implementation of bsd flock().  */
#define F_EXLCK		4	/* or 3 */
#define F_SHLCK		8	/* or 4 */

#endif

#if 0

/* c_cc characters */
#define VINTR 0
#define VQUIT 1
#define VERASE 2
#define VKILL 3
#define VEOF 4
#define VSWTC 7
#define VSTART 8
#define VSTOP 9
#define VSUSP 10
#define VEOL 11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE 14
#define VLNEXT 15
#define VEOL2 16

/* c_iflag bits */
#define IGNBRK	0000001
#define IGNPAR	0000004
#define PARMRK	0000010
#define INLCR	0000100
#define IGNCR	0000200
#define IUCLC	0001000
#define IXANY	0004000
#define IXOFF	0010000
#define IMAXBEL	0020000
#define IUTF8	0040000

/* c_oflag bits */
#define OPOST	0000001
#define OLCUC	0000002
#define ONLCR	0000004
#define OCRNL	0000010
#define ONOCR	0000020
#define ONLRET	0000040
#define OFILL	0000100
#define OFDEL	0000200
#define NLDLY	0000400
#define   NL0	0000000
#define   NL1	0000400
#define CRDLY	0003000
#define   CR0	0000000
#define   CR1	0001000
#define   CR2	0002000
#define   CR3	0003000
#define TABDLY	0014000
#define   TAB0	0000000
#define   TAB1	0004000
#define   TAB2	0010000
#define   TAB3	0014000
#define   XTABS	0014000
#define BSDLY	0020000
#define   BS0	0000000
#define   BS1	0020000
#define VTDLY	0040000
#define   VT0	0000000
#define   VT1	0040000
#define FFDLY	0100000
#define   FF0	0000000
#define   FF1	0100000

/* c_cflag bit meaning */
#define CBAUD	0010017
#define  B0	0000000		/* hang up */
#define  B50	0000001
#define  B75	0000002
#define  B110	0000003
#define  B134	0000004
#define  B150	0000005
#define  B200	0000006
#define  B300	0000007
#define  B600	0000010
#define  B1200	0000011
#define  B1800	0000012
#define  B2400	0000013
#define  B4800	0000014
#define  B9600	0000015
#define  B19200	0000016
#define  B38400	0000017
#define EXTA B19200
#define EXTB B38400
#define   CS5	0000000
#define   CS6	0000020
#define   CS7	0000040
#define CSTOPB	0000100
#define CREAD	0000200
#define PARODD	0001000
#define HUPCL	0002000
#define CLOCAL	0004000
#define CBAUDEX 0010000
#define    BOTHER 0010000
#define    B57600 0010001
#define   B115200 0010002
#define   B230400 0010003
#define   B460800 0010004
#define   B500000 0010005
#define   B576000 0010006
#define   B921600 0010007
#define  B1000000 0010010
#define  B1152000 0010011
#define  B1500000 0010012
#define  B2000000 0010013
#define  B2500000 0010014
#define  B3000000 0010015
#define  B3500000 0010016
#define  B4000000 0010017
#define CIBAUD	  002003600000	/* input baud rate */
#define CMSPAR	  010000000000	/* mark or space (stick) parity */
#define CRTSCTS	  020000000000	/* flow control */

#define IBSHIFT	  16		/* Shift from CBAUD to CIBAUD */

/* c_lflag bits */
#define ICANON	0000002
#define XCASE	0000004
#define ECHO	0000010
#define ECHONL	0000100
#define NOFLSH	0000200
#define TOSTOP	0000400
#define ECHOCTL	0001000
#define ECHOPRT	0002000
#define ECHOKE	0004000
#define FLUSHO	0010000
#define PENDIN	0040000
#define EXTPROC	0200000

/* tcflow() and TCXONC use these */
#define	TCOOFF		0
#define	TCOON		1
#define	TCIOFF		2
#define	TCION		3

/* tcflush() and TCFLSH use these */
#define	TCIFLUSH	0
#define	TCOFLUSH	1
#define	TCIOFLUSH	2

#endif

/*--------------------------------------------------------------------------*/

/*                            TERMINAL HANDLING                             */

/*--------------------------------------------------------------------------*/

#define TERMINFO struct termios
/* #define TERMINAL struct termios */

struct termios
   {int c_iflag;		/* input mode flags */
    int c_oflag;		/* output mode flags */
    int c_cflag;		/* control mode flags */
    int c_lflag;		/* local mode flags */
    int c_line;			/* line discipline */
    int c_cc[8];		/* control characters */
    int c_ispeed;		/* input speed */
    int c_ospeed;};		/* output speed */

typedef struct s_TERMINAL_STATE TERMINAL_STATE;

struct winsize
   {unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;
    unsigned short ws_ypixel;};

struct s_TERMINAL_STATE
   {int fd;
    int full_info;
    int valid_size;
/*    TERMINAL term; */
    struct winsize window_size;};    

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

