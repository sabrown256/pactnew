/*
 * SCTERM.C - terminal mode control routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_term.h"
#include "scope_proc.h"

#define OLDWAY

#define SET_ATTR(_v, _a, _s)                                                 \
    switch (_s)                                                              \
       {case 1 :                                                             \
	     _v |= _a;                                                       \
	     break;                                                          \
	case 0 :                                                             \
	     _v &= ~_a;                                                      \
	     break;                                                          \
	case -1 :                                                            \
	     _v = _a;                                                        \
	     break;}

/*--------------------------------------------------------------------------*/

#if defined(BSD_TERMINAL)

/*--------------------------------------------------------------------------*/

/* _SC_GET_TTY_ATTR - get the attributes of the file descriptor FD into
 *                  - the TERMINAL struct T
 */

static int _SC_get_tty_attr(int fd, TERMINAL *t)
   {int rv;

    rv = ioctl(fd, TIOCGETP, t);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_TTY_ATTR - set the attributes of the file descriptor FD from
 *                  - the TERMINAL struct T
 */

static int _SC_set_tty_attr(int fd, TERMINAL *t, int now)
   {int rv;

    if (now == TRUE)
       rv = ioctl(fd, TIOCSETN, t);
    else
       rv = ioctl(fd, TIOCSETP, t);

    return(rv);}

/*--------------------------------------------------------------------------*/

#elif !defined(MSW)

/*--------------------------------------------------------------------------*/

/* _SC_GET_TTY_ATTR - get the attributes of the file descriptor FD into
 *                  - the TERMINAL struct T
 */

static int _SC_get_tty_attr(int fd, TERMINAL *t)
   {int rv;

    rv = tcgetattr(fd, t);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_TTY_ATTR - set the attributes of the file descriptor FD from
 *                  - the TERMINAL struct T
 */

static int _SC_set_tty_attr(int fd, TERMINAL *t, int now)
   {int rv;

    if (now == TRUE)
       rv = tcsetattr(fd, TCSANOW, t);
    else
       rv = tcsetattr(fd, TCSAFLUSH, t);

    return(rv);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SC_SET_IO_ATTR - set the ATTR to STATE in terminal T
 *                 - STATE is either 1 (on), 0 (off), -1 (only)
 *                 - NOTE: this is really file static but conditionalizations
 *                 - cause compiler warnings which are silenced by
 *                 - removing the static declaration
 */

void _SC_set_io_attr(void *pt, int class, int attr, int state)
   {

#ifdef TERMINAL

    TERMINAL *t;

    t = (TERMINAL *) pt;

# ifdef BSD_TERMINAL

    SET_ATTR(t->sg_flags, attr, state);

# else

/* input mode constants (POSIX)
 * IGNBRK - ignore break
 * BRKINT - break causes input and output to be flushed
 * IGNPAR - ignore parity error
 * PARMRK - prefix character with parity error
 * INPCK  - enable input parity check
 * ISTRIP - strip off bit 8
 * IGNCR  - ignore CR on input
 * INLCR  - NL -> CR on input
 * ICRNL  - CR -> NL on input
 * IXON   - enable XON/XOFF on output
 * IXOFF  - enable XON/XOFF on input
 * IUCLC  - UC -> LC on input   (non-POSIX)
 * IXANY  - enable any character restart   (non-POSIX)
 */
    if (class == SC_TERM_INPUT)
       {SET_ATTR(t->c_iflag, attr, state);}

/* output mode constants (POSIX)
 * OPOST  - enable output processing
 * ONLCR  - NL -> CR-NL on output
 * OCRNL  - CR -> NL on output
 * ONOCR  - no CR at column 0
 * ONLRET - no CR
 * OFILL  - fill for delay
 * NLDLY  - NL delay mask
 * CRDLY  - CR delay mask
 * TABDLY - hor. tab delay mask
 * BSDLY  - backspace delay mask
 * VTDLY  - vert. tab delay mask
 * FFDLY  - form feed delay mask
 * OLCUC  - LC -> UC on output (non-POSIX)
 * OFDEL  - fill is DEL (NUL be default) (non-POSIX)
 */
    else if (class == SC_TERM_OUTPUT)
       {SET_ATTR(t->c_oflag, attr, state);}

/* control mode constants (POSIX)
 * CSIZE   - char size mask
 * CSTOPB  - 2 stop bits
 * CREAD   - enable recv
 * PARENB  - enable parity gen on output and chk on input
 * PARODD  - parity for in and out is odd
 * HUPCL   - hang up after last process
 * CLOCAL  - ignore modem control lines
 * CBAUD   - baud mask (non-POSIX)
 * CBAUDEX - extra baud mask (non-POSIX)
 * CIBAUD  - input baud mask (non-POSIX)
 * CRTSCTS - enable RTS/CTS control (non-POSIX)
 */
    else if (class == SC_TERM_CONTROL)
       {SET_ATTR(t->c_cflag, attr, state);}

/* local mode constants (POSIX)
 * ISIG    - pass on INTR, QUIT, SUSP, DSUSP signals
 * ICANON  - enable canonical mode (buffers by line)
 * ECHO    - echo input
 * ECHOE   - if ICANON enable ERASE/WERASE
 * ECHOK   - if ICANON enable KILL
 * ECHONL  - if ICANON echo NL
 * NOFLSH  - disable flush on signals
 * TOSTOP  - send SIGTTOU when needed
 * IEXTEN  - enable input processing
 * ECHOCTL - if ECHO Ctl-X echos as ^X (non-POSIX)
 * ECHOPRT - if ICANON and IECHO print char as they are erased (non-POSIX)
 * ECHOKE  - if ICANON echo KILL by erasing line (non-POSIX)
 */
    else if (class == SC_TERM_LOCAL)
       {SET_ATTR(t->c_lflag, attr, state);}

/* c_cc defines the special control characters (POSIX)
 * VINTR    - ^C or DEL - send SIGINT
 * VQUIT    - send SIGQUIT
 * VERASE   - DEL or ^H - erase char
 * VKILL    - ^U - kill line
 * VEOF     - ^D - end of file
 * VMIN     - min number of chars for read
 * VEOL     - NUL - end of line
 * VTIME    - timeout in deciseconds
 * VSTART   - ^Q - start char
 * VSTOP    - ^S - stop char
 * VSUSP    - ^Z - suspend char
 * VEOL2    - NUL - another end of line (non-POSIX)
 * VLNEXT   - ^V - literal next (non-POSIX)
 * VWERASE  - ^W - word erase (non-POSIX)
 * VREPRINT - ^R - reprint unread chars (non-POSIX)
 */
    else if (class == SC_TERM_CHAR)
       t->c_cc[attr] = state;

# endif
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_IO_ATTR - set the specified attribute to be ON or OFF */

int SC_set_io_attr(int fd, int attr, int state)
   {int rv;

    rv = FALSE;

#ifdef TERMINAL
    int ok;

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {TERMINAL s;

	rv = _SC_get_tty_attr(fd, &s);
	if (rv == -1)
	   SC_error(-1, "COULDN'T GET STATE - SC_SET_IO_ATTR");

	_SC_set_io_attr(&s, SC_TERM_LOCAL, attr, state);

	rv = _SC_set_tty_attr(fd, &s, TRUE);
	if (rv == -1)
	   SC_error(-1, "COULDN'T SET STATE - SC_SET_IO_ATTR");
	else
	   rv = TRUE;};

    SC_ERR_UNTRAP();

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_IO_ATTRS - set the specified set of attributes to be ON or OFF
 *                 - terminated by attribute value of 0
 */

int SC_set_io_attrs(int fd, ...)
   {int rv;

    rv = FALSE;

#ifdef TERMINAL
    int ok, attr, state, class;

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {int ond, offd;
	TERMINAL s;

	rv = _SC_get_tty_attr(fd, &s);
	if (rv == -1)
	   SC_error(-1, "COULDN'T GET STATE - SC_SET_IO_ATTR");

	ond  = 0;
	offd = 0;

	SC_VA_START(fd);

	while (TRUE)
	   {attr = SC_VA_ARG(int);
	    if (attr == 0)
	       break;

	    class = SC_VA_ARG(int);
	    state = SC_VA_ARG(int);

	    if (class == SC_TERM_DESC)
	       {if (state == 0)
		   offd |= attr;
	        else if (state == 1)
		   ond |= attr;}

	    else
	       _SC_set_io_attr(&s, class, attr, state);};

	SC_VA_END;

/* set the attributes that go via ioctl first */
	rv = _SC_set_tty_attr(fd, &s, TRUE);
	if (rv == -1)
	   SC_error(-1, "COULDN'T SET STATE - SC_SET_IO_ATTR");
	else
	   rv = TRUE;

/* now set the attribute that go via fcntl last */
	if (offd != 0)
	   rv = SC_set_fd_attr(fd, offd, FALSE);

	if (ond != 0)
	   rv = SC_set_fd_attr(fd, ond, TRUE);};

    SC_ERR_UNTRAP();

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_FD_ATTR - set the control flags on a file descriptor
 *                - the following are more or less portable
 *                -
 *                - SC_NDELAY  non-blocking I/O
 *                - SC_APPEND  append (writes guaranteed at the end)
 *                - SC_SYNC    synchronous write option
 *                -
 *                - the STATE argument specifies how the flag is to
 *                - be set from I
 *                -    1  I added to flag (with |)
 *                -    0  I removed from flag (with & ~)
 *                -   -1  flag set to I
 *                - return TRUE iff successful
 */

int SC_set_fd_attr(int fd, int i, int state)
   {int rv;

    rv = FALSE;

#ifdef HAVE_POSIX_SYS
    int ok;

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {if (i != SC_ASYNC)
	   {int arg, status;

	    arg = fcntl(fd, F_GETFL);
	    if (arg < 0)
	       SC_error(-1, "COULDN'T GET DESCRIPTOR FLAG - SC_SET_FD_ATTR");

	    SET_ATTR(arg, i, state);

	    status = fcntl(fd, F_SETFL, arg);
	    if (status < 0)
	       SC_error(-1, "COULDN'T SET DESCRIPTOR FLAG - SC_SET_FD_ATTR");

	    rv = TRUE;};};

    SC_ERR_UNTRAP();

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PRINT_TERM_STATE - print to the specified file the state of the
 *                     - specified file descriptor
 */

void SC_print_term_state(FILE *fp, int fd)
   {int c;

#ifdef TERMINAL
    int rv;
    TERMINFO s;

    if (fp != NULL)
       fd = fileno(fp);

    rv = tcgetattr(fd, &s);
    if (rv == -1)
       SC_error(-1, "COULDN'T GET STATE - SC_PRINT_TERM_STATE");

/* input mode constants */
    c = s.c_iflag;
# ifdef IGNBRK
    if (c & IGNBRK) printf("i> IGNBRK  - ignore break\n");
# endif
# ifdef BRKINT
    if (c & BRKINT) printf("i> BRKINT  - break causes input and output to be flushed\n");
# endif
# ifdef IGNPAR
    if (c & IGNPAR) printf("i> IGNPAR  - ignore parity error\n");
# endif
# ifdef PARMRK
    if (c & PARMRK) printf("i> PARMRK  - prefix character with parity error\n");
# endif
# ifdef INPCK
    if (c & INPCK)  printf("i> INPCK   - enable input parity check\n");
# endif
# ifdef ISTRIP
    if (c & ISTRIP) printf("i> ISTRIP  - strip off bit 8\n");
# endif
# ifdef IGNCR
    if (c & IGNCR)  printf("i> IGNCR   - ignore CR on input\n");
# endif
# ifdef INLCR
    if (c & INLCR)  printf("i> INLCR   - NL -> CR on input\n");
# endif
# ifdef ICRNL
    if (c & ICRNL)  printf("i> ICRNL   - CR -> NL on input\n");
# endif
# ifdef IXON
    if (c & IXON)   printf("i> IXON    - enable XON/XOFF on output\n");
# endif
# ifdef IXOFF
    if (c & IXOFF)  printf("i> IXOFF   - enable XON/XOFF on input\n");
# endif
# ifdef IUCLC
    if (c & IUCLC)  printf("i> IUCLC   - UC -> LC on input   (non-POSIX)\n");
# endif
# ifdef IXANY
    if (c & IXANY)  printf("i> IXANY   - enable any character restart   (non-POSIX)\n");
# endif

/* output mode constants */
    c = s.c_oflag;
# ifdef OPOST
    if (c & OPOST)  printf("o> OPOST   - enable output processing\n");
# endif
# ifdef ONLCR
    if (c & ONLCR)  printf("o> ONLCR   - NL -> CR-NL on output\n");
# endif
# ifdef OCRNL
    if (c & OCRNL)  printf("o> OCRNL   - CR -> NL on output\n");
# endif
# ifdef ONOCR
    if (c & ONOCR)  printf("o> ONOCR   - no CR at column 0\n");
# endif
# ifdef ONLRET
    if (c & ONLRET) printf("o> ONLRET  - no CR\n");
# endif
# ifdef OFILL
    if (c & OFILL)  printf("o> OFILL   - fill for delay\n");
# endif
# ifdef NLDLY
    if (c & NLDLY)  printf("o> NLDLY   - NL delay mask\n");
# endif
# ifdef CRDLY
    if (c & CRDLY)  printf("o> CRDLY   - CR delay mask\n");
# endif
# ifdef TABDLY
    if (c & TABDLY) printf("o> TABDLY  - horizontal tab delay mask\n");
# endif
# ifdef BSDLY
    if (c & BSDLY)  printf("o> BSDLY   - backspace delay mask\n");
# endif
# ifdef VTDLY
    if (c & VTDLY)  printf("o> VTDLY   - vertical tab delay mask\n");
# endif
# ifdef FFDLY
    if (c & FFDLY)  printf("o> FFDLY   - form feed delay mask\n");
# endif
# ifdef OLCUC
    if (c & OLCUC)  printf("o> OLCUC   - LC -> UC on output (non-POSIX)\n");
# endif
# ifdef OFDEL
    if (c & OFDEL)  printf("o> OFDEL   - fill is DEL (NUL be default) (non-POSIX)\n");
# endif

/* control mode constants */
    c = s.c_cflag;
# ifdef CSIZE
    if (c & CSIZE)   printf("c> CSIZE   - char size mask\n");
# endif
# ifdef CSTOPB
    if (c & CSTOPB)  printf("c> CSTOPB  - 2 stop bits\n");
# endif
# ifdef CREAD
    if (c & CREAD)   printf("c> CREAD   - enable recv\n");
# endif
# ifdef PARENB
    if (c & PARENB)  printf("c> PARENB  - enable parity gen on output and chk on input\n");
# endif
# ifdef PARODD
    if (c & PARODD)  printf("c> PARODD  - parity for in and out is odd\n");
# endif
# ifdef HUPCL
    if (c & HUPCL)   printf("c> HUPCL   - hang up after last process\n");
# endif
# ifdef CLOCAL
    if (c & CLOCAL)  printf("c> CLOCAL  - ignore modem control lines\n");
# endif
# ifdef CBAUD
    if (c & CBAUD)   printf("c> CBAUD   - baud mask (non-POSIX)\n");
# endif
# ifdef CBAUDEX
    if (c & CBAUDEX) printf("c> CBAUDEX - extra baud mask (non-POSIX)\n");
# endif
# ifdef CIBAUD
    if (c & CIBAUD)  printf("c> CIBAUD  - input baud mask (non-POSIX)\n");
# endif
# ifdef CRTSCTS
    if (c & CRTSCTS) printf("c> CRTSCTS - enable RTS/CTS control (non-POSIX)\n");
# endif

/* local mode constants */
    c = s.c_lflag;
# ifdef ISIG
    if (c & ISIG)    printf("l> ISIG    - pass on INTR, QUIT, SUSP, DSUSP signals\n");
# endif
# ifdef ICANON
    if (c & ICANON)  printf("l> ICANON  - enable canonical mode (buffers by line)\n");
# endif
# ifdef ECHO
    if (c & ECHO)    printf("l> ECHO    - echo input\n");
# endif
# ifdef ECHOE
    if (c & ECHOE)   printf("l> ECHOE   - if ICANON enable ERASE/WERASE\n");
# endif
# ifdef ECHOK
    if (c & ECHOK)   printf("l> ECHOK   - if ICANON enable KILL\n");
# endif
# ifdef ECHONL
    if (c & ECHONL)  printf("l> ECHONL  - if ICANON echo NL\n");
# endif
# ifdef ECHOPRT
    if (c & ECHOPRT) printf("l> ECHOPRT - if ICANON and ECHO print char as they are erased (non-POSIX)\n");
# endif
# ifdef ECHOKE
    if (c & ECHOKE)  printf("l> ECHOKE  - if ICANON echo KILL by erasing line (non-POSIX)\n");
# endif
# ifdef ECHOCTL
    if (c & ECHOCTL) printf("l> ECHOCTL - if ECHO Ctl-X echos as ^X (non-POSIX)\n");
# endif
# ifdef IEXTEN
    if (c & IEXTEN)  printf("l> IEXTEN  - enable input processing\n");
# endif
# ifdef NOFLSH
    if (c & NOFLSH)  printf("l> NOFLSH  - disable flush on signals\n");
# endif
# ifdef TOSTOP
    if (c & TOSTOP)  printf("l> TOSTOP  - send SIGTTOU when needed\n");
# endif

    printf("s> VMIN   %d\n", s.c_cc[VMIN]);
    printf("s> VTIME  %d\n", s.c_cc[VTIME]);

#endif

    c = fcntl(fd, F_GETFL);
    if (c & O_CREAT)     printf("f> O_CREAT     - create file\n");
#ifdef O_EXCL
    if (c & O_EXCL)      printf("f> O_EXCL      - fail to open existing file\n");
#endif
#ifdef O_NOCTTY
    if (c & O_NOCTTY)    printf("f> O_NOCTTY    - cannot be controlling term\n");
#endif
#ifdef O_TRUNC
    if (c & O_TRUNC)     printf("f> O_TRUNC     - truncate existing file\n");
#endif
#ifdef O_APPEND
    if (c & O_APPEND)    printf("f> O_APPEND    - append mode\n");
#endif
#ifdef O_NONBLOCK
    if (c & O_NONBLOCK)  printf("f> O_NONBLOCK  - non-blocking I/O\n");
#endif
#ifdef O_NDELAY
    if (c & O_NDELAY)    printf("f> O_NDELAY    - non-blocking I/O\n");
#endif
#ifdef O_SYNC
    if (c & O_SYNC)      printf("f> O_SYNC      - synchronous I/O\n");
#endif
#ifdef O_ASYNC  /* Solaris does not have this */
    if (c & O_ASYNC)     printf("f> O_ASYNC     - asynchronous I/O - SIGIO\n");
#endif
#ifdef O_LARGEFILE
    if (c & O_LARGEFILE) printf("f> O_LARGEFILE - allow large files\n");
#endif
#ifdef O_NOFOLLOW
    if (c & O_NOFOLLOW)  printf("f> O_NOFOLLOW  - no sym links\n");
#endif
#ifdef O_DIRECTORY
    if (c & O_DIRECTORY) printf("f> O_DIRECTORY - open directory\n");
#endif
#ifdef O_DIRECT
    if (c & O_DIRECT)    printf("f> O_DIRECT    - user space buffers\n");
#endif

    return;}

/*--------------------------------------------------------------------------*/

/*                        TERMINAL SPECIFIC ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* DBCK - diagnostic print of info for background determination */

void dbck(void)
   {int bg, pid, ppid, pgid, ptid;

    pid  = getpid();
    ppid = getppid();
    pgid = getpgrp();
    ptid = tcgetpgrp(fileno(stdin));
    bg   = SC_is_background_process(pid);

    printf("-> ppid(%d) ptid(%d) pgid(%d) pid(%d) background(%d)\n",
	   ppid, ptid, pgid, pid, bg);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_RAW_STATE - set the RAW processing on the given descriptor
 *                   - don't do anything else if you can't get the
 *                   - parameters for the stream
 *                   - this also weeds out descriptors (e.g. sockets
 *                   - and pipes) for which this is inappropriate
 *                   - (TTY's and PTY's need this stuff)
 */

static int _SC_set_raw_state(int fd, int trap)
   {int rv;

    rv = FALSE;

#ifdef TERMINAL
    int ok;

    ok = (trap == TRUE) ? SC_ERR_TRAP() : 0;
    if (ok == 0)

# ifdef BSD_TERMINAL

       {TERMINAL t;

	rv = _SC_get_tty_attr(fd, &t);
	if (rv > -1)
	   {t.sg_flags |= RAW;

	    rv = _SC_set_tty_attr(fd, &t, TRUE);
	    if (rv < 0)
	       SC_error(-1, "COULDN'T SET I/O FLAGS %d - _SC_SET_RAW_STATE",
			errno);};};

# else

       rv = SC_set_io_attrs(fd,
			    SC_NDELAY, SC_TERM_DESC,     TRUE,
			    ICRNL,     SC_TERM_INPUT,    FALSE,
			    IXON,      SC_TERM_INPUT,    FALSE,
			    OPOST,     SC_TERM_OUTPUT,   FALSE,
			    ISIG,      SC_TERM_LOCAL,    FALSE,
			    ICANON,    SC_TERM_LOCAL,    FALSE,
			    ECHO,      SC_TERM_LOCAL,    FALSE,
			    ECHOE,     SC_TERM_LOCAL,    FALSE,
			    ECHOK,     SC_TERM_LOCAL,    FALSE,
			    IEXTEN,    SC_TERM_LOCAL,    FALSE,
/* GCC C99 change
			    ECHOCTL,   SC_TERM_LOCAL,    FALSE,
			    ECHOKE,    SC_TERM_LOCAL,    FALSE,
*/
			    CSIZE,     SC_TERM_CONTROL,  FALSE,
			    PARENB,    SC_TERM_CONTROL,  FALSE,
			    CS8,       SC_TERM_CONTROL,  TRUE,
			    VMIN,      SC_TERM_CHAR,     0,
			    VTIME,     SC_TERM_CHAR,     0,
			    0);
# endif

    if (rv == TRUE)
       _SC.term = SC_TERM_RAW;

    if (trap)
       {SC_ERR_UNTRAP();};

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_COOKED_STATE - set the COOKED processing on the given descriptor
 *                      - don't do anything else if you can't get the
 *                      - parameters for the stream
 *                      - this also weeds out descriptors (e.g. sockets
 *                      - and pipes) for which this is inappropriate
 *                      - (TTY's and PTY's need this stuff)
 */

static int _SC_set_cooked_state(int fd, int trap)
   {int rv;

    rv = FALSE;

#ifdef TERMINAL
    int ok;

    ok = (trap == TRUE) ? SC_ERR_TRAP() : 0;
    if (ok == 0)

# ifdef BSD_TERMINAL
       {TERMINAL t;

	rv = _SC_get_tty_attr(fd, &t);
	if (rv > -1)

/* this is cooked */
	   {t.sg_flags |= ( ECHO | CRMOD | CBREAK | TANDEM | ALLDELAY );

	    rv = _SC_set_tty_attr(fd, &t, TRUE);
	    if (rv < 0)
	       SC_error(-1, "COULDN'T SET I/O FLAGS %d - _SC_SET_COOKED_STATE",
			errno);};};
# else

       rv = SC_set_io_attrs(fd,
			    ICRNL,     SC_TERM_INPUT,    TRUE,
			    IXON,      SC_TERM_INPUT,    TRUE,
			    OPOST,     SC_TERM_OUTPUT,   TRUE,
			    ISIG,      SC_TERM_LOCAL,    TRUE,
			    ICANON,    SC_TERM_LOCAL,    TRUE,
			    ECHO,      SC_TERM_LOCAL,    TRUE,
			    ECHOE,     SC_TERM_LOCAL,    TRUE,
			    ECHOK,     SC_TERM_LOCAL,    TRUE,
			    IEXTEN,    SC_TERM_LOCAL,    TRUE,
/* GCC C99 change
			    ECHOCTL,   SC_TERM_LOCAL,    TRUE,
			    ECHOKE,    SC_TERM_LOCAL,    TRUE,
*/
			    CSIZE,     SC_TERM_CONTROL,  TRUE,
			    PARENB,    SC_TERM_CONTROL,  TRUE,
			    CS8,       SC_TERM_CONTROL,  TRUE,
			    VMIN,      SC_TERM_CHAR,     1,
			    VTIME,     SC_TERM_CHAR,     1,
			    0);
# endif

#ifndef OLDWAY
    if (rv == TRUE)
       _SC.term = SC_TERM_COOKED;
#endif

    if (trap)
       {SC_ERR_UNTRAP();};

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TERM_GET_STATE - get the terminal state on the given descriptor
 *                   - allocate and return a TERMINAL_STATE
 *                   - containing the old state
 *                   - don't do anything else if you can't get the
 *                   - parameters for the stream
 *                   - this also weeds out descriptors (e.g. sockets and
 *                   - pipes) for which this is inappropriate (PTY's need
 *                   - this stuff)
 */

void *SC_term_get_state(int fd, int size)
   {TERMINAL_STATE *t;
    void *rv;

    t = NULL;

#ifdef TERMINAL
    {int st;

     t = CMAKE(TERMINAL_STATE);
     t->fd        = fd;
     t->full_info = TRUE;

     st = _SC_get_tty_attr(fd, &t->term);
     if (st < 0)
        {CFREE(t);
	 return(NULL);};

# ifdef BSD_TERMINAL

    if (ioctl(fd, TIOCGETC, &t->tch) < 0)
       return(NULL);

    if (ioctl(fd, TIOCGLTC, &t->ltc) < 0)
       return(NULL);

    if (ioctl(fd, TIOCLGET, &t->localmode) < 0)
       return(NULL);

/* Mac OS X does not implement this and complains only at runtime! */
#  if !defined(MACOSX)
    if (ioctl(fd, TIOCGETD, &t->discipline) < 0)
       return(NULL);
#  endif
# endif

      t->valid_size = size;
      if (size == TRUE)
	 {SC_set_term_size(fd, -1, -1, -1, -1);
	  st = ioctl(fd, TIOCGWINSZ, &t->window_size);
	  if (st < 0)
	     {CFREE(t);
	      return(NULL);};};};
#endif

    rv = t;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TERM_SET_STATE - set the terminal state on the given descriptor
 *                   - don't do anything else if you can't get the
 *                   - parameters for the stream
 *                   - this also weeds out descriptors (e.g. sockets and
 *                   - pipes) for which this is inappropriate (PTY's need
 *                   - this stuff)
 *                   - return TRUE iff successful
 */

int SC_term_set_state(int fd, void *a)
   {int rv;
    SC_contextdes oh;

    rv = FALSE;
    oh = SC_signal_n(SIGTTOU, SIG_IGN, NULL);

#ifdef TERMINAL
    int st;
    TERMINAL_STATE *t;

    t = (TERMINAL_STATE *) a;

    if (t != NULL)
       {if (fd < 0)
	   fd = t->fd;

	st = _SC_set_tty_attr(fd, &t->term, TRUE);
	if (st >= 0)
	   {rv = TRUE;

# ifdef BSD_TERMINAL
	    if (t->full_info)
	       {if (ioctl(fd, TIOCSETC, &t->tch) < 0)
		   rv = FALSE;

		else if (ioctl(fd, TIOCSLTC, &t->ltc) < 0)
		   rv = FALSE;

		else if (ioctl(fd, TIOCLSET, &t->localmode) < 0)
		   rv = FALSE;

		else if (ioctl(fd, TIOCSETD, &t->discipline) < 0)
		   rv = FALSE;};
# endif

	    if (t->valid_size == TRUE)
	       {if (ioctl(fd, TIOCSWINSZ, &t->window_size) < 0)
		   rv = FALSE;};

	    if (rv == TRUE)
	       _SC.term = SC_TERM_CUSTOM;};};

#endif

    SC_signal_n(SIGTTOU, oh.f, oh.a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHANGE_TERM_STATE - change the terminal state to one of the
 *                      - modes defined by the SC_termst enum
 *                      - return the prior state
 */

SC_termst SC_change_term_state(int fd, SC_termst st, int trap, void *a)
   {SC_termst rv;

    rv = _SC.term;
    if (st != rv)
       {switch (st)
	   {case SC_TERM_COOKED :
	         _SC_set_cooked_state(fd, trap);
		 break;
	    case SC_TERM_RAW :
		 _SC_set_raw_state(fd, trap);
		 break;
	    case SC_TERM_LEH_RAW :
		 _SC_leh_ena_raw(fd);
		 break;
	    case SC_TERM_CUSTOM :
		 SC_term_set_state(fd, a);
		 break;
	    case SC_TERM_UNKNOWN :
	    default:
		 break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MPI_PROC_STR - copy the input string S into D replacing all occurences
 *                  - of '\n' with '\n'TAG
 *                  - return string D
 *                  - this structure is predicated on the fact that
 *                  - while messages can be constructed on the sender
 *                  - in arbitrary pieces the receiver gets full lines
 *                  - terminated by \n
 */

static char *_SC_mpi_proc_str(const char *s, int trm)
   {char *tag, *pt, *d, *t, *pe;

    d = NULL;

    tag = getenv("SC_MPI_TAG_IO");
    if (tag == NULL)
       d = SC_dstrcat(d, (char *) s);

    else
       {if (s != NULL)
	   {d = SC_dstrcat(d, tag);

	    t = CSTRSAVE((char *) s);

	    for (pt = t; TRUE; )
	        {pe = strchr(pt, '\n');
		 if (pe == NULL)
		    {d = SC_dstrcat(d, pt);
		     break;}
		 else
		    {*pe = '\0';
		     d = SC_dstrcat(d, pt);
		     d = SC_dstrcat(d, "\n");
		     d = SC_dstrcat(d, tag);

		     pt = pe + 1;};};

	    CFREE(t);

	    if (trm == TRUE)
	       d = SC_dstrcat(d, SC_DEFEAT_MPI_BUG);};};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MPI_FPUTS - do MPI output according to POSIX standards
 *              - with mpi-io-wrap this evades the MPI buffer flush
 *              - bug
 */

int SC_mpi_fputs(const char *s, FILE *fp)
   {int nc;
    char *p;

#ifdef HAVE_BAD_MPI_IO

    if (fp == stdout)
       p = _SC_mpi_proc_str(s, TRUE);
    else
       p = CSTRSAVE((char *) s);

#else

    p = CSTRSAVE((char *) s);

#endif

    nc = SAFE_FPUTS(p, fp);

    CFREE(p);

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MPI_PRINTF - do MPI output according to POSIX standards
 *               - with mpi-io-wrap this evades the MPI buffer flush
 *               - bug
 */

int SC_mpi_printf(FILE *fp, char *fmt, ...)
   {int nc;
    char *s;

    SC_VDSNPRINTF(TRUE, s, fmt);

    nc = SC_mpi_fputs(s, fp);

    CFREE(s);

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MPI_FTN_SNPRINTF - do MPI output according to POSIX standards
 *                     - with mpi-io-wrap this evades the MPI buffer flush bug
 *                     - this prepares a buffer for Fortran output
 *                     - Fortran supplies a trailing \n which we cannot control
 */

int SC_mpi_ftn_snprintf(char *bf, int nc, char *fmt, ...)
   {char *s, *t;

    SC_VDSNPRINTF(TRUE, s, fmt);

    t = _SC_mpi_proc_str(s, FALSE);
    strncpy(bf, t, nc);
    nc = strlen(bf);

    CFREE(s);
    CFREE(t);

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MPI_IO_SUPPRESS - send a special message to the receiver that is
 *                    - interpreted to start suppressing subsequent untagged
 *                    - output if ON is TRUE and resume processing output if
 *                    - ON is FALSE
 *                    - return the value of the prior suppress state
 */

int SC_mpi_io_suppress(int on)
   {int rv;

#ifdef HAVE_BAD_MPI_IO
    char *s;
    static int use_mpi = -1;

    if (use_mpi == -1)
       use_mpi = (getenv("SC_MPI_NO_SUPPRESSION") == NULL);

    rv = SC_mpi_suppress(on);

/* NOTE: if this is called from a non-MPI code you probabaly
 * are not running this with mpi-io-wrap and hence do not want
 * these messages
 * since you are calling MPI related functions in a non-MPI code
 * the onus is on you to set the SC_MPI_NO_SUPPRESSION variable
 * in order to turn off this message
 */
    if (use_mpi == TRUE)
       {if (on == TRUE)
	   s = "+SC_SUPPRESS_UNTAGGED_ON+\n";
        else
	   s = "+SC_SUPPRESS_UNTAGGED_OFF+\n";

	SAFE_FPUTS(s, stdout);};
#else
    rv = FALSE;
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MPI_SUPPRESS - set the MPI IO suppress state to ST
 *                 - legal values are 1 for ON, 0 for OFF, and -1 to query
 *                 - return the prior state
 *                 - if ST is -1 just return the current state
 */

int SC_mpi_suppress(int st)
   {int rv;

    if (_SC.suppress == -1)

#ifdef HAVE_BAD_MPI_IO
       _SC.suppress = (getenv("SC_MPI_SUPPRESS_UNTAGGED") != NULL);
#else
       _SC.suppress = FALSE;
#endif

    rv = _SC.suppress;

    if (st != -1)
       _SC.suppress = st;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                    TERMINAL WINDOW SIZE ROUTINES                         */

/*--------------------------------------------------------------------------*/

#if !defined(MACOSX)

/*--------------------------------------------------------------------------*/

/* _SC_SET_QUERY_STATE - set up the given descriptor
 *                     - for nicely querying escape codes from the terminal
 */

static int _SC_set_query_state(int fd)
   {int rv;

    rv = SC_set_io_attrs(fd,
			 SC_NDELAY, SC_TERM_DESC,     TRUE,

			 ICANON,    SC_TERM_LOCAL,    FALSE,
			 ECHO,      SC_TERM_LOCAL,    FALSE,

			 CS8,       SC_TERM_CONTROL,  TRUE,

			 VMIN,      SC_TERM_CHAR,     0,
			 VTIME,     SC_TERM_CHAR,     0,
			 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_TERM_TIMEOUT - timeout waiting for terminal response */

static void _SC_get_term_timeout(int sig)
   {JMP_BUF *cpu;

    cpu = SC_GET_CONTEXT(_SC_get_term_timeout);

    LONGJMP(*cpu, ABORT);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_TERM_RESP - read and parse the reply of terminal
 *                   - in response to a width/height query
 */

void _SC_get_term_resp(int fd, int c, int *pw, int *ph)
   {int i, w, h, nc;
    char s[MAXLINE];
    char *t;
    JMP_BUF cpu;

    h = -1;
    w = -1;

/* we will wait one second to hear back from the terminal */
    SC_timeout(2, _SC_get_term_timeout, &cpu);

    if (SETJMP(cpu) == 0)

/* get the reply to the query from the terminal */
       {for (i = 0; i < MAXLINE; )
	    {nc = SC_read_sigsafe(fd, s+i, MAXLINE);
	     if (nc > 0)
	        i += nc;
	     else if ((i > 0) && (strchr(s, c) != NULL))
	        break;
	     SC_sleep(10);};

	s[i] = '\0';

/* parse out the width and height - throw away the sequence id */
	t = SC_firsttok(s, ";\n");
	t = SC_firsttok(s, ";\n");
	h = SC_stoi(t);
	t = SC_firsttok(s, ";t\n");
	w = SC_stoi(t);};

/* cancel the timer */
    SC_timeout(0, _SC_get_term_timeout, NULL);

    if (pw != NULL)
       *pw = w;

    if (ph != NULL)
       *ph = h;

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* SC_GET_TERM_ATTR - get the value of some terminal attribute
 *                  - in the current program session
 *                  - return TRUE iff successful
 *                  - example terminal window size
 *                  -  SC_get_term_attr("14t", "t", 2, val)
 */

int SC_get_term_attr(char *cmd, char *rsp, int n, int *val)
   {int rv;

#if defined(MACOSX)

    rv  = FALSE;

#else

    int i, fd, bg, act, p;
    char *name, *s;
    TERMINAL_STATE *ts;

    rv  = FALSE;
    bg  = SC_is_background_process(-1);
    act = bg & 5;

    if (act == 0)
       {SC_putenv("TERM=xterm");

/* open the actual terminal device */
	fd   = fileno(stderr);
	name = ttyname(fd);
	if (name == NULL)
	   name = "/dev/tty";

	p  = SC_get_perm(FALSE);
	fd = SC_open_safe(name, O_RDWR | O_NDELAY | O_NOCTTY, p);

	if (fd < 0)
	   rv = FALSE;

	else
	   {ts = SC_term_get_state(fd, FALSE);

/* set the terminal for nice querying */
	    _SC_set_query_state(fd);

/* send the query code */
	    for (i = 0; i < n; i++)
	        val[i] = -1;

	    s = SC_dsnprintf(TRUE, "%c[%s", SC_ESC_CHAR, cmd);
	    SC_write_sigsafe(fd, s, strlen(s));
	    CFREE(s);

	    _SC_get_term_resp(fd, rsp[0], &val[0], &val[1]);

	    rv = TRUE;

/* close the terminal */
	    SC_change_term_state(fd, SC_TERM_CUSTOM, TRUE, ts);
	    SC_close_safe(fd);};};

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_TERM_SIZE - get the number of lines NR and columns NC
 *                  - in the current program session
 *                  - return TRUE iff successful
 */

int SC_get_term_size(int *pcr, int *pcc, int *ppr, int *ppc)
   {int rv;

#if defined(MACOSX)

    rv  = FALSE;

#else

    int fd, bg, act, p;
    int pw, ph, cw, ch;
    char *name, *s;
    TERMINAL_STATE *ts;

    rv  = FALSE;
    bg  = SC_is_background_process(-1);
    act = bg & 5;

    if (act == 0)
       {SC_putenv("TERM=xterm");

/* open the actual terminal device */
	fd   = fileno(stderr);
	name = ttyname(fd);
	if (name == NULL)
	   name = "/dev/tty";

	p  = SC_get_perm(FALSE);
	fd = SC_open_safe(name, O_RDWR | O_NDELAY | O_NOCTTY, p);

	if (fd < 0)
	   rv = FALSE;

	else
	   {ts = SC_term_get_state(fd, FALSE);

/* set the terminal for nice querying */
	    _SC_set_query_state(fd);

/* send the code to report the pixel height and width of
 * the terminal window: ESC[14t
 */
	    pw = -1;
	    ph = -1;
	    if ((ppr != NULL) || (ppc != NULL))
	       {s = SC_dsnprintf(TRUE, "%c[14t", SC_ESC_CHAR);
		SC_write_sigsafe(fd, s, strlen(s));
                CFREE(s);

		_SC_get_term_resp(fd, 't', &pw, &ph);
		if (ppr != NULL)
		   *ppr = ph;

		if (ppc != NULL)
		   *ppc = pw;

		rv = TRUE;};

/* send the code to report the character height and width of
 * the terminal text area: ESC[18t
 */
	    cw = -1;
	    ch = -1;
	    if ((pcr != NULL) || (pcc != NULL))
	       {s = SC_dsnprintf(TRUE, "%c[18t", SC_ESC_CHAR);
		SC_write_sigsafe(fd, s, strlen(s));
                CFREE(s);

		_SC_get_term_resp(fd, 't', &cw, &ch);
		if (pcr != NULL)
		   *pcr = ch;

		if (pcc != NULL)
		   *pcc = cw;

		rv = TRUE;};

/* close the terminal */
	    SC_change_term_state(fd, SC_TERM_CUSTOM, TRUE, ts);
	    SC_close_safe(fd);};};

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPTERM - print the term size
 *        - intended to be called under the debugger
 */

void dpterm(void)
   {int cw, ch;

    SC_get_term_size(&ch, &cw, NULL, NULL);

    io_printf(stdout, "# rows = %d\n", ch);
    io_printf(stdout, "# cols = %d\n", cw);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_TERM_SIZE - set the number of lines NR and columns NC of text
 *                  - in the terminal session identified by FD
 *                  - return TRUE iff successful
 */

int SC_set_term_size(int fd, int nr, int nc, int pw, int ph)
   {int rv, pr, pc;
    char *s;
    struct winsize w;

    rv = FALSE;

    pr = pw;
    pc = ph;

/* query the terminal if reqested via specific nonsense request */
    if ((nr == -1) && (nc == -1))
       SC_get_term_size(&nr, &nc, &pr, &pc);
    else
       SC_get_term_size(NULL, NULL, &pr, &pc);

    if ((0 < nr) && (0 < nc))

/* try to set them using ioctls */
       {rv = ioctl(fd, TIOCGWINSZ, &w);
	if (rv != -1)
	   {w.ws_row = nr;
	    w.ws_col = nc;
	    if ((0 < pr) && (0 < pc))
	       {w.ws_xpixel = pc;
		w.ws_ypixel = pr;};
	    rv = ioctl(fd, TIOCSWINSZ, &w);};

/* try to set them using LINES/COLUMNS environment variables */
	s = SC_dsnprintf(FALSE, "LINES=%d", nr);
	SC_putenv(s);
	s = SC_dsnprintf(FALSE, "COLUMNS=%d", nc);
	SC_putenv(s);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IS_BACKGROUND - signal handler serving SC_IS_BACKGROUND_PROCESS */

static void _SC_is_background(int sig)
   {

    LONGJMP(_SC.btt, 1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_BACKGROUND_PROCESS - determine whether the process PID is
 *                          - running in the background
 *                          - if PID is -1 check the current process
 *                          - the return value is a bit array
 *                          -   set bit 1 if PID cannot write to its
 *                          -   controlling terminal without incurring a
 *                          -   a suspend signal
 *                          -   set bit 2 if PID is different from the
 *                          -   process group leader
 *                          -   set bit 4 if PID is different from the
 *                          -   terminal group ID
 */

int SC_is_background_process(int pid)
   {int rv, fd, ppid, pgid, ptid;
    SC_contextdes oh;

    if (pid == -1)
       pid = getpid();
    ppid = getppid();
    pgid = getpgrp();
    ptid = tcgetpgrp(fileno(stdin));

    rv = 0;

/* set bit 4 if terminal group id is different from the pid */
    if (pid != ptid)
       rv |= 4;

/* set bit 2 if the parent is init or the process group id
 * is different from the pid
 */
    if ((ppid == 1) || (pid != pgid))
       rv |= 2;

/* set bit 1 if we cannot write to the
 * controlling terminal without getting SIGTTOU
 */
    oh = SC_signal_n(SIGTTOU, _SC_is_background, NULL);

    if (SETJMP(_SC.btt) == 0)
       {fd = fileno(stdout);
	SC_set_io_attr(fd, TOSTOP, 0);}

    else
       rv |= 1;

    SC_signal_n(SIGTTOU, oh.f, oh.a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DISCONNECT_TTY - break the association with the controlling terminal */

void SC_disconnect_tty(void)
   {

#ifdef HAVE_SYSV
    if ((cp->ischild == TRUE) && (setsid() < 0))
       SC_error(SC_NO_SETSID,
		"COULDN'T DITCH CONTROLLING TERMINAL - SC_DISCONNECT_TTY");
#endif

#ifdef BSD_TERMINAL
    int fd, p;

    p  = SC_get_perm(FALSE);
    fd = SC_open_safe("/dev/tty", O_RDWR, p);
    if (fd >= 0)
       {if (ioctl(fd, TIOCNOTTY, NULL) < 0)
	   SC_error(SC_NO_TTY,
		    "COULDN'T DITCH CONTROLLING TERMINAL - SC_DISCONNECT_TTY");
	SC_close_safe(fd);};
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IO_SUSPEND - have process suspend on I/O if FL is TRUE
 *               - and proceed if FL is FALSE
 */

int SC_io_suspend(int fl)
   {

    if (fl == FALSE)
       {SC_signal(SIGTTOU, SIG_IGN);
	SC_signal(SIGTTIN, SIG_IGN);
	SC_signal(SIGTSTP, SIG_IGN);}
    else
       {SC_signal(SIGTTOU, SIG_DFL);
	SC_signal(SIGTTIN, SIG_DFL);
	SC_signal(SIGTSTP, SIG_DFL);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LINE_BUFFER_FD - set file descriptor FD to be in line buffered mode */

int SC_line_buffer_fd(int fd)
   {int rv;

    rv = SC_set_io_attrs(fd,
			 ICANON,    SC_TERM_LOCAL,    TRUE,
			 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LINE_BUFFER_FILE - set file FP to be in line buffered mode */

int SC_line_buffer_file(FILE *fp)
   {int fd, rv;

    fd = fileno(fp);
    rv = SC_line_buffer_fd(fd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHAR_BUFFER_FD - set file descriptor FD to be in char at a time mode */

int SC_char_buffer_fd(int fd)
   {int rv;

    rv = SC_set_io_attrs(fd,
			 ICANON,    SC_TERM_LOCAL,    FALSE,
			 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHAR_BUFFER_FILE - set file FP to be in char at a time mode */

int SC_char_buffer_file(FILE *fp)
   {int fd, rv;

    fd = fileno(fp);
    rv = SC_char_buffer_fd(fd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_LINE_BUFFERED_FD - return TRUE iff FD is in line buffered mode */

int SC_is_line_buffered_fd(int fd)
   {int rv;

    rv = TRUE;

#ifdef TERMINAL

    {int c;
     TERMINFO s;

     rv = tcgetattr(fd, &s);
     if (rv == -1)
        SC_error(-1, "COULDN'T GET STATE - SC_IS_LINE_BUFFERED");

/* local mode constants */
     c = s.c_lflag;

# ifdef ICANON
     rv = ((c & ICANON) != 0);
# endif

     };

#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_LINE_BUFFERED_FILE - return TRUE iff FP is in line buffered mode */

int SC_is_line_buffered_file(FILE *fp)
   {int fd, rv;

    fd = fileno(fp);
    rv = SC_is_line_buffered_fd(fd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

