/*
 * POSIX-MSW.H - header to provide a POSIX environment on MS Windows
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#ifndef PCK_POSIX_MSW
#define PCK_POSIX_MSW

/* to the extent that these routines emulate the
 * POSIX routines we need this is a POSIX system
 */
#define HAVE_POSIX_SYS

#undef byte

#define FLOAT   FLOAT_MSW
#define LONG    LONG_MSW
#define INT     INT_MSW
#define SHORT   SHORT_MSW
#define CHAR    CHAR_MSW
#define boolean boolean_msw

#include <ws2tcpip.h>

#define cwait cwait_msw
#include <unistd.h>
#undef cwait

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>

#include <io.h>
#include <direct.h>
#include <process.h>

#include <windows.h>

#undef boolean
#undef TEXT
#undef CHAR
#undef SHORT
#undef INT
#undef LONG
#undef FLOAT

/*--------------------------------------------------------------------------*/

/*                        STANDARD CONFIGURATIONS                           */

/*--------------------------------------------------------------------------*/

/* NO_SHELL should be set for platforms lacking shell windows
 * this would include DOS and Windows
 */
/*
#define NO_SHELL
*/

#define directory_delim "\\"
#define directory_delim_c '\\'

/* errno set */

#define	ETXTBSY		26
#define	ETIMEDOUT	110
#define	EINPROGRESS	115

/* signal set */

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

/* permission bits
 * special owner group other 
 *   bbb    bbb   bbb   bbb
 *
 */

#define S_ISUID	   (1 << 11)         /* set user ID on execution */
#define S_ISGID	   (1 << 10)         /* set group ID on execution */
#define S_ISVTX	   (1 << 9)

#if 0

#define S_IRUSR	   (1 << 8)          /* read by owner */
#define S_IWUSR	   (1 << 7)          /* write by owner */
#define S_IXUSR	   (1 << 6)          /* execute by owner */
#define S_IRWXU	   (S_IRUSR | S_IWUSR | S_IXUSR)

#endif

#define S_IRGRP	   (1 << 5)          /* read by group */
#define S_IWGRP	   (1 << 4)          /* write by group */
#define S_IXGRP	   (1 << 3)          /* execute by group */
#define S_IRWXG	   (S_IRGRP | S_IWGRP | S_IXGRP)

#define S_IROTH	   (1 << 2)           /* read by others */
#define S_IWOTH	   (1 << 1)           /* write by others */
#define S_IXOTH	   (1 << 0)           /* execute by others */
#define S_IRWXO	   (S_IROTH | S_IWOTH | S_IXOTH)

#define _UTSNAME_LENGTH    65


/* poll constants */

#define POLLIN    0x0001
#define POLLPRI   0x0002
#define POLLOUT   0x0004
#define POLLERR   0x0008
#define POLLHUP   0x0010
#define POLLNVAL  0x0020


/* process constants */

#define WNOHANG           1

#define WEXITSTATUS(_s)	  ((_s) & 1)
#define WTERMSIG(_s)	  ((_s) & 2)
#define WSTOPSIG(_s)	  ((_s) & 4)

#define WIFEXITED(_s)	  ((_s) & 8)
#define WIFSIGNALED(_s)	  ((_s) & 16)
#define WIFSTOPPED(_s)	  ((_s) & 32)

/* faux set of terminal constants */

#define OPOST	0000001
#define ICANON	0000002
#define ECHO	0000010
#define ECHOE	0000020
#define ECHOK	0000040

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


/* fcntl constants */

#define O_NOCTTY	0400
#define O_NONBLOCK	04000
#define O_NDELAY	O_NONBLOCK
#define O_SYNC	        04010000
#define O_FSYNC		O_SYNC
#define O_ASYNC		020000

#define F_DUPFD		0
#define F_GETFD		1
#define F_SETFD		2
#define F_GETFL		3
#define F_SETFL		4

#if 0

/* Values for the second argument to `fcntl'.  */
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

/*                          STANDARD PROCEDURAL MACROS                      */

/*--------------------------------------------------------------------------*/

#define HAVE_STREAMS_P

#define JMP_BUF            jmp_buf
#define SETJMP(_x)         setjmp(_x)
#define LONGJMP(_x, _v)    longjmp(_x, _v)

#define fileno(x)          _fileno(x)
/* #define stat(_a, _b)       _stat(_a, _b) */

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

typedef unsigned char       u_int8_t;
typedef unsigned short      u_int16_t;
typedef unsigned int        u_int32_t;
typedef unsigned long long  u_int64_t;

typedef int uid_t;
typedef int gid_t;
typedef int sigset_t;

typedef unsigned long nfds_t;

typedef jmp_buf sigjmp_buf;

struct pollfd
   {int fd;
    short int events;
    short int revents;};

struct flock
   {int l_type;
    int l_start;
    int l_whence;
    int l_len;
    int l_pid;};

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

struct utsname
   {char sysname[_UTSNAME_LENGTH];
    char nodename[_UTSNAME_LENGTH];
    char release[_UTSNAME_LENGTH];
    char version[_UTSNAME_LENGTH];
    char machine[_UTSNAME_LENGTH];
    char domainname[_UTSNAME_LENGTH];};

struct s_TERMINAL_STATE
   {int fd;
    int full_info;
    int valid_size;
/*    TERMINAL term; */
    struct winsize window_size;};    

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int
 sleep(unsigned int seconds);

extern char
 *getcwd(char *buf, int size);

extern struct passwd
 *getpwuid(uid_t uid);

extern uid_t
 getuid(void);

extern gid_t
 getgid(void);

extern pid_t
 getpid(void),
 getppid(void),
 getpgrp(void),
 tcgetpgrp(int fd),
 setsid(void),
 fork(void),
 waitpid(pid_t pid, int *status, int options);

extern int
 getgroups(int size, gid_t list[]),
 kill(pid_t pid, int sig),
 sched_yield(void),
 fsync(int fd),
 ioctl(int d, unsigned long request, ...),
 fcntl(int fd, int cmd, ...),
 poll(struct pollfd *fds, nfds_t nfds, int timeout),
 nanosleep(const struct timespec *req, struct timespec *rem),
 setenv(const char *name, const char *value, int overwrite),
 unsetenv(const char *name),
 uname(struct utsname *buf),
 pipe(int pipefd[2]),
 sigsetjmp(sigjmp_buf env, int savesigs),
 mkdir_msw(const char *pathname, mode_t mode),
 select_msw(int nfds, fd_set *readfds, fd_set *writefds,
	    fd_set *exceptfds, struct timeval *timeout);

extern unsigned int
 alarm(unsigned int seconds);

extern long
 random(void);

extern char
 *ttyname(int fd),
 *realpath(const char *path, char *resolved_path);

extern void
 siglongjmp(sigjmp_buf env, int val),
 srandom(unsigned int seed),
 sync(void);

/*--------------------------------------------------------------------------*/

#ifndef NO_DEFINE_MSW_POSIX_FUNCS

/*--------------------------------------------------------------------------*/

/*                              POSIX ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* GETPWUID - getpwuid for MSW */

struct passwd *getpwuid(uid_t uid)
   {struct passwd *rv;

    rv = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETUID - getuid for MSW */

uid_t getuid(void)
   {uid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETGID - getgid for MSW */

gid_t getgid(void)
   {gid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETPID - getpid for MSW */

pid_t getpid(void)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETPPID - getppid for MSW */

pid_t getppid(void)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETPGRP - getpgrp for MSW */

pid_t getpgrp(void)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TCGETPGRP - tcgetpgrp for MSW */

pid_t tcgetpgrp(int fd)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETSID - setsid for MSW */

pid_t setsid(void)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GETGROUPS - getgroups for MSW */

int getgroups(int size, gid_t list[])
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORK - fork for MSW */

pid_t fork(void)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WAITPID - waitpid for MSW */

pid_t waitpid(pid_t pid, int *status, int options)
   {pid_t rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* KILL - kill for MSW */

int kill(pid_t pid, int sig)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCHED_YIELD - SCHED_yield for MSW */

int sched_yield(void)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FSYNC - fsync for MSW */

int fsync(int fd)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IOCTL - ioctl for MSW */

int ioctl(int d, unsigned long request, ...)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FCNTL - fcntl for MSW */

int fcntl(int fd, int cmd, ...)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POLL - poll for MSW */

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NANOSLEEP - nanosleep for MSW */

int nanosleep(const struct timespec *req, struct timespec *rem)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETENV - setenv for MSW */

int setenv(const char *name, const char *value, int overwrite)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNSETENV - unsetenv for MSW */

int unsetenv(const char *name)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TTYNAME - ttyname for MSW */

char *ttyname(int fd)
   {char *rv;

    rv = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNAME - uname for MSW */

int uname(struct utsname *buf)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PIPE - pipe for MSW */

int pipe(int pipefd[2])
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ALARM - alarm for MSW */

unsigned int alarm(unsigned int seconds)
   {unsigned int rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGSETJMP - sigsetjmp for MSW */

int sigsetjmp(sigjmp_buf env, int savesigs)
   {int rv;

    rv = setjmp(env);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIGLONGJMP - siglongjmp for MSW */

void siglongjmp(sigjmp_buf env, int val)
   {

    longjmp(env, val);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SYNC - sync for MSW */

void sync(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REALPATH - realpath for MSW */

char *realpath(const char *path, char *resolved_path)
   {char *rv;

    rv = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SRANDOM - srandom for MSW */

void srandom(unsigned int seed)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RANDOM - random for MSW */

long random(void)
   {long rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MKDIR_MSW - mkdir for MSW */

int mkdir_msw(const char *pathname, mode_t mode)
   {int rv;

    rv = mkdir(pathname);

    return(rv);}

#define mkdir    mkdir_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SELECT_MSW - select for MSW */

int select_msw(int nfds, fd_set *readfds, fd_set *writefds,
	       fd_set *exceptfds, struct timeval *timeout)
   {int rv;

    rv = -1;

    return(rv);}

#define select    select_msw

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

#ifdef __cplusplus
}
#endif

#endif

