/*
 * SCHP.C - high level process control
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_proc.h"

/* ISSUES and PROBLEMS
 *
 *  (1) Buffer flushing has been a plague since day 1.  The result
 *      is lost characters.
 *         (a) flush sometimes means pass the contents on and
 *             sometimes it means throw the contents away
 *         (b) PACT wants "pass it on" semantics or nothing
 *  (2) The IOCTL semantic variations are hard to follow. Try to
 *      use POSIX terminal controls (termios) instead of older
 *      SYSV (termio) and BSD (sgttyb) protocols.
 *  (3) WINDOWS should be able to support a reasonably complete
 *      port of this.
 */

typedef PROCESS *(*PFPPROC)(char **argv, char *mode, int type);

SC_scope_proc
 _SC_ps = { -1, FALSE, NULL };

static char
 *_SC_not_stopped = "running";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RESET_TERMINAL - reset terminal state */

void SC_reset_terminal(void)
   {

    if (_SC.orig_tty != NULL)
       {SC_set_term_state(_SC.orig_tty, -1);
        CFREE(_SC.orig_tty);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DETHREAD - work around PGI failure wrt OpenMP */

void _SC_dethread(void)
   {

#if defined(PTHREAD_OMP) && defined(COMPILER_PGI)
    omp_set_num_threads(1);
#endif

    return;}

/*--------------------------------------------------------------------------*/

/*                           EXEC TESTER ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SC_EXEC_TEST - test the exec system call */

void _SC_exec_test(char **argv, char **envp, char *mode)
   {PROCESS *pp;

    pp = SC_mk_process(argv, mode, SC_CHILD, -1);

    pp->medium     = USE_PIPES;
    pp->data_type  = SC_ASCII;
    pp->in         = 0;
    pp->out        = 1;
    pp->ischild    = FALSE;
    pp->status     = SC_RUNNING;
    pp->reason     = 0;
    pp->start_time = NULL;
    pp->stop_time  = NULL;

    if (pp->exec != NULL)
       pp->exec(pp, argv, envp, mode);

    else
       {pp->release(pp);
	SC_error(-1, "NO EXEC - _SC_EXEC_TEST");};

    exit(SC_EXIT_SELF);}

/*--------------------------------------------------------------------------*/

/*                         PROCESS MANAGER ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _SC_COMPLETE_MESSAGEP - return TRUE iff there is a:
 *                       -   \n terminated string
 *                       -   EOF
 *                       - in the PROCESS ring buffer
 */

static int _SC_complete_messagep(PROCESS *pp)
   {unsigned int nb, ob, ls;
    unsigned char *ring, c;

    ob   = pp->ob_in;
    nb   = pp->nb_ring;
    ring = pp->in_ring;
    ls   = pp->line_sep;

    while ((c = ring[ob]) != '\0')
       {ob = (ob + 1) % nb;
        if ((c == ls) || (c == (unsigned char) EOF))
           return(TRUE);};

    return(FALSE);}

/*--------------------------------------------------------------------------*/

#ifdef SGI

/*--------------------------------------------------------------------------*/

static int _SC_open_pty_unix(PROCESS *pp, PROCESS *cp)
   {int pty, ok;
    char *pc;
    extern char *_getpty(int *pfd, int mode, mode_t perm, int flag);

    pc = _getpty(&pty, O_RDWR | O_NDELAY, 0600, FALSE);
    if (pc != NULL)
       {cp->spty = CSTRSAVE(pc);

	cp->in  = pty;
	cp->out = pty;

	pp->in  = pty;
	pp->out = pty;

	ok = TRUE;}

    else
       ok = FALSE;

    return(ok);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* _SC_OPEN_PTY_UNIX - generic unix pseudo terminal open routine */

static int _SC_open_pty_unix(PROCESS *pp, PROCESS *cp)
   {int c, d, ok, pty;
    char *mpty, *spty, *pc, *pd;

    c  = ' ';
    pc = NULL;
    pty = -1;
    for (pc = SC_MASTER_PTY_LETTERS; ((c = *pc) != '\0') && (pty < 0); pc++)
        {for (pd = SC_MASTER_PTY_DIGITS; (d = *pd) != '\0'; pd++)
	     {mpty = SC_dsnprintf(FALSE, "/dev/%s%c%c",
				  SC_MASTER_PTY_NAME, c, d);
	      pty = open(mpty, O_RDWR | O_NDELAY);
	      if (pty >= 0)
		 {spty = SC_dsnprintf(TRUE, "/dev/%s%c%c",
				      SC_SLAVE_PTY_NAME, c, d);
                  
		  cp->spty = spty;
		  cp->in   = pty;
		  cp->out  = pty;

		  pp->in  = pty;
		  pp->out = pty;

		  break;};};};

    ok = (pty >= 0);

    return(ok);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

#ifdef HAVE_UNIX98_PTY

/*--------------------------------------------------------------------------*/

static int _SC_open_pty_unix98(PROCESS *pp, PROCESS *cp)
   {int ok, err, pty;
    char *ps;
    extern char *ptsname(int fd);
    extern int grantpt(int fd);
    extern int unlockpt(int fd);

    ok   = FALSE;
    pty  = open("/dev/ptmx", O_RDWR|O_NOCTTY);
    err  = grantpt(pty);
    err |= unlockpt(pty);
    if ((pty >= 0) && (err == 0))
       {ps = ptsname(pty);
	cp->spty = CSTRSAVE(ps);

	cp->in  = pty;
	cp->out = pty;

	pp->in  = pty;
	pp->out = pty;

	ok = TRUE;}

    else if (pty >= 0)
       close(pty);

    if (ok == FALSE)
       ok = _SC_open_pty_unix(pp, cp);

    return(ok);}

static int
 (*open_pty)(PROCESS *pp, PROCESS *cp) = _SC_open_pty_unix98;

#define PTY_OPEN_DEFINED

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

#ifndef PTY_OPEN_DEFINED

/*--------------------------------------------------------------------------*/

static int
 (*open_pty)(PROCESS *pp, PROCESS *cp) = _SC_open_pty_unix;

#define PTY_OPEN_DEFINED

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* SC_MK_PROCESS - initialize and return a PROCESS */

PROCESS *SC_mk_process(char **argv, char *mode, int type, int iex)
   {unsigned int nb;
    PROCESS *pp;

    pp = CMAKE(PROCESS);

    if ((iex < 10) || (30 < iex))
       nb = SIZE_BUF;
    else
       nb = 1 << iex;

/* NOTE: the value of pp->id can be passed to waitpid so the initial
 * value of -1 is essential!
 */
    pp->id          = -1;

    pp->status      = NOT_FINISHED;
    pp->reason      = NOT_FINISHED;
    pp->in          = -1;
    pp->out         = -1;
    pp->data        = -1;
    pp->medium      = NO_IPC;
    pp->gone        = -1;
    pp->data_type   = SC_UNKNOWN;

    pp->nb_ring     = nb;
    pp->in_ring     = CMAKE_N(unsigned char, nb);
    pp->ib_in       = 0;
    pp->ob_in       = 0;

    memset(pp->in_ring, 0, nb);

    pp->type        = SC_LOCAL;
    pp->rcpu        = -1;
    pp->acpu        = -1;

    pp->line_mode   = FALSE;
    pp->line_sep    = 0x0a;
    pp->blocking    = FALSE;

    pp->n_pending   = 0;
    pp->pending     = NULL;
    pp->vif         = NULL;
    pp->spty        = NULL;

    pp->index       = SC_PROC_ALLOC;
    pp->flags       = 0;

    pp->data_buffer = NULL;
    pp->nb_data     = 0L;
    pp->nx_data     = 0L;

    pp->n_zero      = 0;                     /* work around OSF SIGCHLD bug */
    pp->n_read      = 0L;
    pp->n_write     = 0L;

    pp->start_time  = NULL;
    pp->stop_time   = NULL;

/* function and arg to call when process finishes */
    pp->on_exit     = NULL;
    pp->exit_arg    = NULL;

/* for binary data */
    pp->read         = NULL;
    pp->write        = NULL;
    pp->recv_formats = NULL;
    pp->send_formats = NULL;
    pp->rl_vif       = NULL;

    pp->open_retry   = -1;
    _SC_init_filedes(pp->fd);

    pp->tty = NULL;

    if (type == SC_PARENT)
       {pp->ischild = FALSE;
        _SC_manage_process(pp);}
    else
       pp->ischild = TRUE;

/* application supplied heuristic function to identify lost processes */
    pp->lost = NULL;

    SC_PROCESS_METHODS(pp);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIXUP_SOCKET - reset socket setup to values we want
 *                  - instead of system defaults
 */

void _SC_fixup_socket(int s)
   {

# ifdef HAVE_SOCKETS_P
    int ok, nb, iv;

    iv = 0;
    ok = 0;
    nb = sizeof(int);

#ifdef SO_SNDBUF
    iv = 65535;
    ok = setsockopt(s, SOL_SOCKET, SO_SNDBUF, &iv, nb);
    SC_ASSERT(ok == 0);
#endif

#ifdef SO_RCVBUF
    iv = 65535;
    ok = setsockopt(s, SOL_SOCKET, SO_RCVBUF, &iv, nb);
    SC_ASSERT(ok == 0);
#endif

#ifdef SO_SNDLOWAT
    iv = 1;
    ok = setsockopt(s, SOL_SOCKET, SO_SNDLOWAT, &iv, nb);
    SC_ASSERT(ok == 0);
#endif

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_PTY - do PTY initializations for _SC_INIT_IPC */

static int _SC_init_pty(PROCESS *pp, PROCESS *cp)
   {int ret, nr, nc, pw, ph;

    ret = (*open_pty)(pp, cp);
    if (ret == FALSE)
       SC_error(-1, "CAN'T OPEN PTY - _SC_INIT_IPC");

    nr = -1;
    nc = -1;
    pw = -1;
    ph = -1;
    SC_get_term_size(&nr, &nc, &ph, &pw);

    pp->tty = CMAKE(SC_ttydes);
    cp->tty = CMAKE(SC_ttydes);

    pp->tty->nr = nr;
    pp->tty->nc = nc;
    pp->tty->pw = pw;
    pp->tty->ph = ph;

    cp->tty->nr = nr;
    cp->tty->nc = nc;
    cp->tty->pw = pw;
    cp->tty->ph = ph;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_IPC - establish two inter-process communications channels
 *              - use one of three media:
 *              -  1) open an available pseudo terminal, pty
 *              -     connect the child to the master side and
 *              -     the parent to the slave side (thanks to Neale Smith
 *              -     for the explanation of PTY's)
 *              -  2) socket
 *              -  3) pipe
 *              - the input channel should always be unblocked
 *              - return TRUE iff successful
 */

static int _SC_init_ipc(PROCESS *pp, PROCESS *cp)
   {int ret;

    ret = TRUE;

#ifdef HAVE_PROCESS_CONTROL

    switch (pp->medium)
       {case USE_PTYS :
	     ret = _SC_init_pty(pp, cp);
             break;

        case USE_PIPES :
             {int ports[2];

              if (pipe(ports) < 0)
                 SC_error(-1, "COULDN'T CREATE PIPE #1 - _SC_INIT_IPC");

              pp->in  = ports[0];
              cp->out = ports[1];

              if (pipe(ports) < 0)
                 {close(pp->in);
                  close(cp->out);
                  SC_error(-1, "COULDN'T CREATE PIPE #2 - _SC_INIT_IPC");};

              cp->in  = ports[0];
              pp->out = ports[1];

              SC_set_fd_attr(pp->in, O_RDONLY | O_NDELAY, TRUE);
              SC_set_fd_attr(pp->out, O_WRONLY, TRUE);
              SC_set_fd_attr(cp->in, O_RDONLY & ~O_NDELAY, TRUE);
              SC_set_fd_attr(cp->out, O_WRONLY, TRUE);};

              break;

# ifdef HAVE_SOCKETS_P

        case USE_SOCKETS :
             {int ports[2];

              if (socketpair(PF_UNIX, SOCK_STREAM, 0, ports) < 0)
                 SC_error(-1, "COULDN'T CREATE SOCKET PAIR #1 - _SC_INIT_IPC");
/*
	      _SC_fixup_socket(ports[0]);
	      _SC_fixup_socket(ports[1]);
*/
              pp->in  = ports[0];
              cp->out = ports[1];
              if (socketpair(PF_UNIX, SOCK_STREAM, 0, ports) < 0)
                 {close(pp->in);
                  close(cp->out);
                  SC_error(-1, "COULDN'T CREATE SOCKET PAIR #2 - _SC_INIT_IPC");};
/*
	      _SC_fixup_socket(ports[0]);
	      _SC_fixup_socket(ports[1]);
*/
              cp->in  = ports[0];
              pp->out = ports[1];

              SC_set_fd_attr(pp->in, O_RDONLY | O_NDELAY, TRUE);
              SC_set_fd_attr(pp->out, O_WRONLY, TRUE);
              SC_set_fd_attr(cp->in, O_RDONLY & ~O_NDELAY, TRUE);
              SC_set_fd_attr(cp->out, O_WRONLY, TRUE);};

             break;

# endif

        default :
             ret = FALSE;};

#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHILD_FORK - the child process comes here and
 *                - it will never return
 */

static void _SC_child_fork(PROCESS *pp, PROCESS *cp, int to,
			   char **argv, char **envp, char *mode)
   {int rv;

/* clear the timer */
    ALARM(0);

/* set all signal handlers to their default state */
    SC_set_signal_handlers(SIG_DFL, NULL, 0, SC_NSIG);

/* free the parent state which the child does not need */
    pp->release(pp);

/* setup the running child state */
    cp->ischild    = TRUE;
    cp->start_time = SC_datef();
    cp->stop_time  = _SC_not_stopped;

    rv = SC_EXIT_SELF;
    if (cp->exec != NULL)
       {rv = cp->exec(cp, argv, envp, mode);

	io_printf(stdout, "%s\n", SC_error_msg());};

    exit(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PARENT_FORK - the parent process comes here */

static int _SC_parent_fork(PROCESS *pp, PROCESS *cp, int to,
                           int rcpu, char *mode)
   {int st;
   
    st = TRUE;

    pp->id         = cp->id;
    pp->rcpu       = rcpu;
    pp->start_time = SC_datef();
    pp->stop_time  = _SC_not_stopped;
    pp->pru        = SC_process_init_rusage();

    SC_process_state(pp, SC_PROC_EXEC);

#ifdef F_SETOWN
    if (pp->medium == USE_SOCKETS)
       fcntl(pp->in, F_SETOWN, pp->root);
#endif

    if (strchr(mode, 'o') != NULL)
       _SC_dethread();

    if (SC_BINARY_MODEP(mode))
       {if (!(*pp->recv_formats)(pp))
	   pp = NULL;};

    if (SC_process_alive(pp) && (pp->medium == USE_PTYS))
       {if (_SC.orig_tty != NULL)
	   SC_set_raw_state(0, FALSE);

	SC_set_raw_state(pp->in, FALSE);

	cp->in  = -1;
	cp->out = -1;};

    cp->release(cp);
    cp = NULL;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ERROR_FORK - come here if fork failed
 *                - just cleanup the remains
 */

static void _SC_error_fork(PROCESS *pp, PROCESS *cp)
   {

/* mark this as having to do with the child so that
 * the managed process list works right
 */
    pp->ischild = TRUE;

    pp->release(pp);

    cp->release(cp);

    SC_error(-1, "COULDN'T FORK PROCESS - _SC_ERROR_FORK");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_PIPELINE - scan ARGV for pipe indicators and
 *                   - NULL out those entries
 *                   - return a list of offsets into ARGV for each
 *                   - individual process
 *                   - in effect break ARGV into multiple ARGVs
 *                   - one for each process
 */

static int *_SC_make_pipeline(char **argv)
   {int i, n;
    int *pl;

    n = 1;
    for (i = 0; argv[i] != NULL; i++)
        n += (strcmp(argv[i], "|") == 0);

    pl = CMAKE_N(int, n+1);
    n  = 0;

    pl[n++] = 0;
    for (i = 0; argv[i] != NULL; i++)
        {if (strcmp(argv[i], "|") == 0)
            {pl[n++] = i+1;
	     argv[i] = NULL;};};

    pl[n++] = -1;

    return(pl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PIPELINE_LENGTH - return the length of the pipeline PL */

static int _SC_pipeline_length(int *pl)
   {int n;

    for (n = 0; pl[n] >= 0; n++);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RECONNECT_PIPELINE - reconnect the N PROCCESS's in PA and CA
 *                        - from the canonical parent/child topology
 *                        - into a pipeline topology
 *                        - NOTE: this logic is good for both pipes and
 *                        - sockets but bad for PTYs
 *                        - on the other hand why do PTYs in a pipeline
 */

static void _SC_reconnect_pipeline(int n, PROCESS **pa, PROCESS **ca)
   {int i, nm;
    PROCESS *pp, *cp, *pn, *cn;

    nm = n - 1;

/* remove the parent to child channels except for the last one
 * the final parent out gets connected to the first child in
 */
    if (nm > 0)

/* set any sockets to read/write mode */
       {for (i = 0; i < n; i++)
	    {pp = pa[i];
	     cp = ca[i];
	     if (cp->medium == USE_SOCKETS)
	        {SC_set_fd_attr(pp->in,  O_RDWR, -1);
		 SC_set_fd_attr(pp->out, O_RDWR, -1);
		 SC_set_fd_attr(cp->in,  O_RDWR, -1);
		 SC_set_fd_attr(cp->out, O_RDWR, -1);};};

/* reconnect terminal process output to first process */
	pp = pa[nm];
	pn = pa[0];
	close(pp->out);
	pp->out = pn->out;
	pn->out = -2;

/* close all other parent to child lines */
	for (i = 1; i < nm; i++)
	    {pp = pa[i];
	     cp = ca[i];

	     close(pp->out);
	     close(cp->in);

	     pp->out = -2;
	     cp->in  = -2;};

/* connect all non-terminal child out to next child in */
	for (i = 0; i < nm; i++)
	    {pp = pa[i];
	     cp = ca[i];
	     cn = ca[i+1];

	     close(cn->in);

	     cn->in = pp->in;
	     pp->in = -2;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SETUP_PROC - allocate PROCESS structures for the parent (PP)
 *                - and the child (CP)
 *                - do the initial I/O setup and any other
 *                - preparation prior to the fork
 *                - return the timeout in milliseconds for the post-fork
 *                - setup and exec
 */

static int _SC_setup_proc(PROCESS **ppp, PROCESS **pcp,
			  char **argv, char *mode,
			  SC_filedes *fd, int retry, int iex,
			  PFProcInit initf, PFProcExit exitf, void *exita)
   {int to, flag, rpid, savetty;
    PROCESS *pp, *cp;

    if (initf == NULL)
       {cp = SC_mk_process(argv, mode, SC_CHILD, iex);
        pp = SC_mk_process(argv, mode, SC_PARENT, iex);}
    else
       {cp = (*initf)(argv, mode, SC_CHILD);
        pp = (*initf)(argv, mode, SC_PARENT);};

    if (fd != NULL)
       _SC_copy_filedes(cp->fd, fd);

    savetty = FALSE;
    switch (mode[1])
       {case 't' :
             savetty = TRUE;
        case 'T' :
             pp->medium = USE_PTYS;
             cp->medium = USE_PTYS;
             break;
        case 's' :
             pp->medium = USE_SOCKETS;
             cp->medium = USE_SOCKETS;
             break;
        default  :
        case 'p' :
             pp->medium = USE_PIPES;
             cp->medium = USE_PIPES;
             break;};

    if (SC_BINARY_MODEP(mode))
       {pp->data_type = SC_BINARY;
        cp->data_type = SC_BINARY;}
    else
       {pp->data_type = SC_ASCII;
        cp->data_type = SC_ASCII;};

/* mixing PROCESS's using PTYs and other media is a bad idea at
 * this point
 */
    if ((savetty == TRUE) && (_SC.orig_tty == NULL))
       _SC.orig_tty = SC_get_term_state(0, TRUE);

/* set up the communications pipe */
    flag = _SC_init_ipc(pp, cp);
    if (flag == FALSE)
       {CFREE(pp->tty);
	CFREE(pp);
	CFREE(cp->tty);
        CFREE(cp);
        SC_error(-1, "COULDN'T CREATE IPC CHANNELS - _SC_SETUP_PROC");
	return(-1);};

/* disable SIGTTOU when running in background */
    if (SC_is_background_process(-1) & 1)
       SC_io_suspend(FALSE);

    if (exitf != NULL)
       {pp->on_exit  = exitf;
	pp->exit_arg = exita;};

    pp->gone       = FALSE;
    pp->status     = SC_RUNNING;
    pp->reason     = 0;
    cp->gone       = FALSE;
    cp->status     = SC_RUNNING;
    cp->reason     = 0;
    cp->open_retry = retry;

    rpid     = getpid();
    pp->root = rpid;
    cp->root = rpid;

    *ppp = pp;
    *pcp = cp;

#ifdef SOLARIS
    to = 2000;
#else
    to = 60000;
#endif

    return(to);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_OPEN_PROC - start up a process on the current CPU
 *               - and open a connection to it for further communications
 */

static PROCESS *_SC_open_proc(int rcpu, char *name, char **argv,
			      char **envp, char *mode,
			      SC_filedes *fd, int retry, int iex,
			      PFProcInit initf, PFProcExit exitf, void *exita)
   {PROCESS *pp;

#ifdef HAVE_PROCESS_CONTROL
    int i, n, pid, to, st, off;
    int *pl;
    char **al, *mod;
    PROCESS *cp, **pa, **ca;
    SC_filedes *pfd;

    if (_SC_redir_fail(fd) != -1)
       return(NULL);

    pp = NULL;
    pl = _SC_make_pipeline(argv);
    n  = _SC_pipeline_length(pl);

    pa = CMAKE_N(PROCESS *, n);
    ca = CMAKE_N(PROCESS *, n);

/* setup each process in the pipeline */
    for (i = 0; i < n; i++)
        {off = pl[i];

/* NOTE: use sockets for the non-terminal process */
	 mod = (i < n-1) ? "as" : mode;
	 pfd = (i < n-1) ? NULL : fd;

	 to = _SC_setup_proc(&pa[i], &ca[i], argv+off, mod,
			     pfd, retry, iex, initf, exitf, exita);};

    _SC_reconnect_pipeline(n, pa, ca);

/* launch each process in the pipeline */
    for (i = 0; i < n; i++)
        {pp = pa[i];
	 cp = ca[i];

	 off = pl[i];
         al  = argv + off;

/* fork the process */
         pid    = fork();
	 pp->id = pid;
	 cp->id = pid;

	 switch (pid)
	    {case -1 :
	          _SC_error_fork(pp, cp);
		  pp = NULL;
		  break;

/* the child process comes here and if successful will never return */
	     case 0 :
		  _SC_child_fork(pp, cp, to, al, envp, mode);
		  break;

/* the parent process comes here */
	     default :
	          st = _SC_parent_fork(pp, cp, to, rcpu, mode);
		  if (st == FALSE)
		     {SC_close(pp);
		      pp = NULL;};
		  break;};};

    CFREE(pl);
    CFREE(pa);
    CFREE(ca);

#else

    pp = NULL;

#endif

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_OPEN_REMOTE - start up CMND with arguments ARGV on a remote HOST
 *                - and open a connection to it for further communications
 *                - INIT is a function to special initializations in the
 *                - PROCESS
 *                - return (PROCESS *) -1    if the open fails
 *                - return (PROCESS *) st-2  if the host cannot be verified
 */

PROCESS *SC_open_remote(char *host, char *cmnd,
			char **argv, char **envp, char *mode,
			PFProcInit init)
   {int i, ia, na, st, ok;
    char **ca;
    PROCESS *pp;

    pp         = NULL;
    SC_gs.errn = 0;

    if (argv == NULL)
       na = 0;
    else
       SC_ptr_arr_len(na, argv);

    ca = CMAKE_N(char *, na+7);

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {st = SC_verify_host(host, -2);
	if (st > 0)
	   {i = 0;
	    ca[i++] = "ssh";
	    ca[i++] = "-o";
	    ca[i++] = "SendEnv=SC_EXEC_RLIMIT_AS";
	    ca[i++] = "-q";
	    ca[i++] = host;
	    ca[i++] = cmnd;

	    for (ia = 0; ia < na; ia++)
	        ca[i++] = argv[ia];

	    ca[i++] = NULL;

	    pp = SC_open(ca, envp, mode, "INIT", init, NULL);}

	else
	   SC_error(st-10, "CANNOT VERIFY HOST %s - SC_OPEN_REMOTE", host);

	if (pp == NULL)
	   SC_error(SC_gs.errn, "OPEN FAILED %s - SC_OPEN_REMOTE", host);}

    else
      {SC_gs.errn = ok;
       pp         = NULL;};

    SC_ERR_UNTRAP();

    CFREE(ca);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BUFFER_IN - put new bytes on the ring
 *              - everything must go into the buffer
 *              - the routine that extracts from the buffer will
 *              - decide whether ASCII or BINARY information is expected
 *              - return TRUE iff there is enough room to hold the bytes
 */

int SC_buffer_in(char *bf, int n, PROCESS *pp)
   {int i;
    unsigned int ib, ob, ab, db, mb, nb, nnb;
    unsigned char *ring, *po, *pn;

    ib   = pp->ib_in;
    ob   = pp->ob_in;
    nb   = pp->nb_ring;
    ring = pp->in_ring;    

/* compute available space remaining in the ring */
    while (TRUE)
       {if (ib >= ob)
	   ab = nb - ib + ob;
        else
	   ab = ob - ib;

	if (ab >= n)
	   break;

/* find the size delta - try doubling */
	db = nb;

/* resize the ring */
	nnb = nb + db;
	CREMAKE(ring, unsigned char, nnb);

/* adjust the ring if the "in use" section is discontiguous */
	if (ib < ob)
	   {mb = nb - ob;
	    pn = ring + nnb - 1;
	    po = ring + nb - 1;
	    for (i = 0; i < mb; i++)
	        *pn-- = *po--;

	    ob += db;};

	nb = nnb;

	pp->in_ring = ring;
	pp->nb_ring = nb;
	pp->ob_in   = ob;};

    if (bf != NULL)
       {for (i = 0; i < n; i++, ib = (ib + 1) % nb)
            ring[ib] = *bf++;

        pp->ib_in = ib;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BUFFER_OUT - return the oldest message from the ring in BF */

int SC_buffer_out(char *bf, PROCESS *pp, int n, int binf)
   {int i, ok;
    unsigned int ib, nb, ob, ls;
    unsigned char *ring, c;
    char *pbf;

/* setup to copy from the ring to the buffer */
    ib   = pp->ib_in;
    ob   = pp->ob_in;
    nb   = pp->nb_ring;
    ring = pp->in_ring;    
    ls   = pp->line_sep;

/* copy until hitting a line separator or N characters have been copied */
    pbf = bf;
    ok  = TRUE;
    for (i = 0; ok && (ob != ib) && (i < n); i++, ob = (ob + 1) % nb)
        {c        = ring[ob];
	 *pbf++   = c;
	 ring[ob] = '\0';

	 if ((c == ls) && (binf == FALSE))
	    ok = FALSE;};

/* null terminate unless the character count maxed out */
    if (i < n)
       *pbf++ = '\0';

    pp->ob_in = ob;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_HANDLE_SIGPIPE - handle a SIGPIPE for PROCESS I/O */

static void _SC_handle_sigpipe(int signo)
   {

    SC_error(-2, "BROKEN PIPE - _SC_HANDLE_SIGPIPE");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_INPUT - get a string from the specified process
 *               - if none are available or there is an error return NULL
 */

static char *_SC_get_input(char *bf, int len, PROCESS *pp)
   {char *pbf;

#ifdef HAVE_PROCESS_CONTROL

    int status, blck, reset, run, nb, st;
    int (*get)(char *bf, int len, PROCESS *pp);

    if (pp == NULL)
       return(NULL);

    st = SC_status(pp);

/* if the job is not running then it must not be blocked */
    run           = (st == SC_RUNNING);
    pp->blocking &= run;

/* NOTE: attempt to be a bit safer by accessing pp->gets just one time */
    get = pp->gets;

    blck   = pp->blocking;
    status = pp->in_ready(pp);
    reset  = -1;

/* if unblocked when should be blocked */
    if (status && blck)
       {SC_block_fd(pp->in);
        status = FALSE;
        reset  = FALSE;}

/* if blocked when should be unblocked */
    else if (!status && !blck)
       {SC_unblock_fd(pp->in);
        status = TRUE;
        reset  = TRUE;};

/* if we are unblocked check incoming messages */
    pbf = bf;
    if (status)
       {nb = (*get)(bf, len, pp);
        if (!SC_buffer_in(bf, nb, pp))
           SC_error(-1, "NOT ENOUGH ROOM IN BUFFER - _SC_GET_INPUT");

        if (pp->line_mode)
           {if (_SC_complete_messagep(pp))
               pbf = SC_buffer_out(bf, pp, len, FALSE) ? bf : NULL;
            else
               pbf = NULL;}
        else
           pbf = SC_buffer_out(bf, pp, len, FALSE) ? bf : NULL;}

/* if we are blocked and there are already messages return the next one */
    else if (_SC_complete_messagep(pp))
       pbf = SC_buffer_out(bf, pp, len, FALSE) ? bf : NULL;

/* if we are blocked and there are no messages go wait for one with
 * full FGETS semantics (i.e. terminated with \n)
 */
    else
       {while (TRUE)
          {nb = (*get)(bf, len, pp);
           if (!SC_buffer_in(bf, nb, pp))
              SC_error(-1, "NOT ENOUGH ROOM IN BUFFER - _SC_GET_INPUT");

	   if (_SC_complete_messagep(pp))
	      {pbf = SC_buffer_out(bf, pp, len, FALSE) ? bf : NULL;
	       break;};

	   SC_sleep(100);};};

/* reset the blocking to its state upon entry */
    switch (reset)
       {case 0 :
	     SC_unblock_fd(pp->in);
	     break;
        case 1 :
	     SC_block_fd(pp->in);
	     break;};

#endif

    return(pbf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GETS - get a string from the specified process
 *         - if none are available or there is an error return NULL
 */

char *SC_gets(char *bf, int len, PROCESS *pp)
   {char *pbf;

#ifdef HAVE_PROCESS_CONTROL

    int ok, st;
    SC_contextdes oph;

    pbf = NULL;
    oph = SC_signal_n(SIGPIPE, _SC_handle_sigpipe, NULL);

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {if (!SC_process_alive(pp))
	   SC_error(-1, "BAD PROCESS - SC_GETS");

	st = SC_status(pp);
	if ((st & SC_DEAD) == 0)
	   pbf = _SC_get_input(bf, len, pp);}

    else if (ok == -2)
       {SC_close(pp);};

    SC_ERR_UNTRAP();

    SC_signal_n(SIGPIPE, oph.f, oph.a);

#else

    pbf = bf;

#endif

    return(pbf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PRINTF - do an fprintf style output to a process */

int SC_printf(PROCESS *pp, char *fmt, ...)
   {int ret, ok;
    int (*prnt)(PROCESS *pp, char *bf);
    char *bf;
    SC_contextdes oph;

    ret = FALSE;
    oph = SC_signal_n(SIGPIPE, _SC_handle_sigpipe, NULL);

    ok = SC_ERR_TRAP();
    if (ok == 0)
       {if (!SC_process_alive(pp))
	   SC_error(-1, "BAD PROCESS - SC_PRINTF");

/* NOTE: attempt to be a bit safer by accessing pp->printf just one time */
	if (pp != NULL)
	   prnt = pp->printf;

        SC_VDSNPRINTF(TRUE, bf, fmt);

	if (SC_process_alive(pp) && (prnt != NULL))
	   ret = (*prnt)(pp, bf);

	CFREE(bf);}

    else if (ok == -2)
       {SC_close(pp);};

    SC_ERR_UNTRAP();

    SC_signal_n(SIGPIPE, oph.f, oph.a);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REDIR_FAIL - return the index of the first of the redirections
 *                - which will fail or -1 if all will succeed
 *                - so that we can quit early and not even fork
 */

int _SC_redir_fail(SC_filedes *fd)
   {int fl, fail;
    char *nm;

    fail = -1;

/* shell redirection syntax and file mode correspondence
 *  <   ->  O_RDONLY
 *  >   ->  O_WRONLY | O_CREAT | O_EXCL
 *  >!  ->  O_WRONLY | O_CREAT | O_TRUNC
 *  >>  ->  O_WRONLY | O_APPEND
 */

/* input redirection fails if file does not exist */
    if (fail == -1)
       {nm = fd[0].name;
	fl = fd[0].flag;
	if ((nm != NULL) && !SC_isfile(nm))
	   fail = 0;};

/* stdout redirection fails if file exists and mode has exclusive bit */
    if (fail == -1)
       {nm = fd[1].name;
	fl = fd[1].flag;
	if ((nm != NULL) && SC_isfile(nm) && (fl & O_EXCL))
	   fail = 1;};

/* stderr redirection fails if file exists and mode has exclusive bit */
    if (fail == -1)
       {nm = fd[2].name;
	fl = fd[2].flag;
	if ((nm != NULL) && SC_isfile(nm) && (fl & O_EXCL))
	   fail = 2;};

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REDIR_FILEDES - set the parts of FD implied by REDIR
 *                   - NAME specifies the file to be used
 */

void _SC_redir_filedes(SC_filedes *fd, int nfd, int ifd,
		       char *redir, char *name)
   {int ofd, excl, fl, flc, flt, fla;
    char *nm;

    if ((name != NULL) && (redir != NULL))
       {if (SC_numstrp(name))
	   {ofd = SC_stoi(name);
	    if (ofd >= nfd)
	       return;
	    nm  = fd[ofd].name;
	    fl  = fd[ofd].flag;}
	else
	   {ofd = ifd;
	    nm  = CSTRSAVE(name);
	    fl  = -1;};

/* do not try to use O_EXCL bit with devices - think about it */
	if (strncmp(nm, "/dev/", 5) == 0)
	   excl = 0;
	else
	   excl = O_EXCL;

	flc = O_WRONLY | O_CREAT | excl;
	flt = O_WRONLY | O_CREAT | O_TRUNC;
	fla = O_WRONLY | O_CREAT | O_APPEND;

/* shell redirection syntax and file mode correspondence
 *  <   ->  O_RDONLY
 *  >   ->  O_WRONLY | O_CREAT | O_EXCL
 *  >!  ->  O_WRONLY | O_CREAT | O_TRUNC
 *  >>  ->  O_WRONLY | O_APPEND
 */
	if (strcmp(redir, "<") == 0)
	   {fd[ifd].name = nm;
	    fd[ifd].flag = O_RDONLY;}

/* dual stream redirects: >&, >&!, and >>& */
	else if (strchr(redir, '&') != NULL)
	   {if (strcmp(redir, ">&") == 0)
	       fl = (fl == -1) ? flc : fl;

	    else if (strcmp(redir, ">&!") == 0)
	       fl = (fl == -1) ? flt : fl;

	    else if (strcmp(redir, ">>&") == 0)
	       fl = (fl == -1) ? fla : fl;

	    fd[1].name = nm;
	    fd[2].name = nm;
	    fd[1].flag = fl;
	    fd[2].flag = fl;}

/* single stream redirects: >, >!, and >> */
	else
	   {if (strcmp(redir, ">") == 0)
	       fl = flc;

	    else if (strcmp(redir, ">!") == 0)
	       fl = flt;

	    else if (strcmp(redir, ">>") == 0)
	       fl = fla;

	    fd[ifd].name = nm;
	    fd[ifd].flag = fl;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_FILEDES - set the parts of FD implied by REDIR
 *                 - NAME specifies the file to be used
 */

void _SC_set_filedes(SC_filedes *fd, int ifd, char *name, int fl)
   {char *nm;

    if (name != NULL)
       {nm = CSTRSAVE(name);

	fd[ifd].name = nm;
	fd[ifd].flag = fl;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIN_FILEDES - release the contents of the SC_filedes' FD */

void _SC_fin_filedes(SC_filedes *fd)
   {int i, n;

    if (fd[2].name != fd[1].name)
       n = 3;
    else
       n = 2;

    for (i = 0; i < n; i++)
        {CFREE(fd[i].name);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_FILEDES - initialize a set of 3 SC_filedes structures */

void _SC_init_filedes(SC_filedes *fd)
   {int i;

    for (i = 0; i < 3; i++)
        {fd[i].name  = NULL;
	 fd[i].flag  = -1;
         fd[i].fd    = -1;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_COPY_FILEDES - copy a set of 3 SC_filedes' FA into another FB */

void _SC_copy_filedes(SC_filedes *fb, SC_filedes *fa)
   {int i;

    for (i = 0; i < 3; i++)
        fb[i] = fa[i];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_OPEN - start up a process and open a pipe to it for further
 *         - communications
 *         - there are the following variants for the "name" of the process
 *         - to be opened:
 *         -
 *         -  1) <name>              - process "name" to be forked on
 *         -                         - the current CPU/host
 *         -  2) <host>:<name>       - process "name" to be exec'd
 *         -                         - on remote "host"
 *         -  3) <CPU>@<name>        - process "name" to be forked
 *         -                         - on processor number "CPU" of
 *         -                         - the current, parallel machine
 *         -  4) <host>:<CPU>@<name> - process "name" to be forked on
 *         -                         - processor number "CPU" of the
 *         -                         - remote, parallel "host"
 *         - the legal modes are:
 *         -  r    - read only (child stdout only connected to parent)
 *         -  w    - write only (child stdin only connected to parent)
 *         -  a    - read/write (child stdin and stdout connnect to parent)
 *         -
 *         - after rwa can come (in any order)
 *         -  b    - binary data exchanged
 *         -  o    - set OMP_NUM_THREADS to 1 in the parent
 *         -  p    - communicate via pipe
 *         -  s    - communicate via socket
 *         -  t    - communicate via pty (save original terminal state)
 *         -  T    - communicate via pty (do not save original terminal state)
 *         -  v    - process variable argument list of key, value pairs
 *         -         ending in LAST
 *         - only one of "pst" can be used and only for current CPU/host
 *         - processes
 *         - NOTE: for ftp or telnet the "t" option is useful
 *         - return NULL and set SC_gs.errn on failure
 */

PROCESS *SC_open(char **argv, char **envp, char *mode, ...)
   {int ok, ifd, iex, retry;
    char name[MAXLINE];
    char *key, *host, *proc, *s, *nm;
    SC_filedes fd[3];
    PROCESS *pp;
    PFProcInit initf;
    PFProcExit exitf;
    void *exita;

    SC_gs.errn = 0;

    if ((argv == NULL) || (argv[0] == NULL))
       return(NULL);

    ok = SC_ERR_TRAP();
    if (ok != 0)
       {SC_gs.errn = ok;
	pp         = NULL;}

    else
       {initf = NULL;
	exitf = NULL;
	exita = NULL;
	retry = -1;
	iex   = -1;

	_SC_init_filedes(fd);

/* get any optional arguments */
	SC_VA_START(mode);

	if (strchr(mode, 'v') != NULL)
	   {while (TRUE)
	       {key = SC_VA_ARG(char *);
		if ((key == (char *) LAST) || (key == NULL))
		   break;
		else if (strcmp(key, "INIT") == 0)
		   initf = SC_VA_ARG(PFProcInit);
		else if (strcmp(key, "OPEN-RETRY-TIME") == 0)
		   retry = SC_VA_ARG(int);
		else if (strcmp(key, "RING-EXP") == 0)
		   iex = SC_VA_ARG(int);
		else if (strcmp(key, "EXIT") == 0)
		   {exitf = SC_VA_ARG(PFProcExit);
		    exita = SC_VA_ARG(void *);}
		else if (strcmp(key, "STDIN") == 0)
		   {nm  = SC_VA_ARG(char *);
		    ifd = SC_VA_ARG(int);
		    _SC_set_filedes(fd, 0, nm, ifd);}
		else if (strcmp(key, "STDOUT") == 0)
		   {nm  = SC_VA_ARG(char *);
		    ifd = SC_VA_ARG(int);
		    _SC_set_filedes(fd, 1, nm, ifd);}
		else if (strcmp(key, "STDERR") == 0)
		   {nm  = SC_VA_ARG(char *);
		    ifd = SC_VA_ARG(int);
		    _SC_set_filedes(fd, 2, nm, ifd);}
		else
		   SC_error(-1, "Unknown option: %s", key);};};

	SC_VA_END;

/* copy the name to see where we are going */
	SC_strncpy(name, MAXLINE, argv[0], -1);

	if (SC_OTHER_HOSTP(name))
	   {host = SC_strtok(name, ":", s);
	    proc = SC_strtok(NULL, "\n", s);
	    pp   = SC_open_remote(host, proc, argv+1, envp, mode, initf);}

	else
	   pp = _SC_open_proc(-1, name, argv, envp, mode, fd, retry, iex,
			      initf, exitf, exita);};

    SC_ERR_UNTRAP();

    return(pp);}

/*--------------------------------------------------------------------------*/

/*                         CLIENT/SERVER ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* SC_INIT_SERVER - perform steps to initialize a server
 *                - for SC_GET_PORT return the port for the client connection
 *                - for SC_GET_CONNECTION return the socket for the client
 *                - connection
 */

int SC_init_server(int step, int closep)
   {int rv;

#ifdef HAVE_PROCESS_CONTROL

    int ok;
    
    if (_SC_ps.debug)
       {fprintf(_SC_ps.diag, "   SC_init_server: %d\n", step);
        fflush(_SC_ps.diag);};

    rv = -1;
    ok = SC_ERR_TRAP();
    if (ok != 0)
       {if (closep)
	   {if (_SC.sfd >= 0)
	       close(_SC.sfd);
	    _SC.sfd = -1;};

	rv = _SC.sfd;}

    else
       {switch (step)
	   {case SC_GET_PORT :
	         _SC.sfd = socket(PF_INET, SOCK_STREAM, 0);
		 if (_SC.sfd < 0)
		    SC_error(-1, "COULDN'T OPEN SOCKET - SC_INIT_SERVER");

		 if (_SC_ps.debug)
		    {fprintf(_SC_ps.diag, "      Socket opened: %d\n", _SC.sfd);
		     fflush(_SC_ps.diag);};

		 _SC.srvr = _SC_tcp_bind(_SC.sfd, -1);
		 if (_SC.srvr == NULL)
		    SC_error(-1, "BIND FAILED - SC_INIT_SERVER");
		 else
		    rv = ntohs(_SC.srvr->sin_port);

		 break;

	    case SC_GET_CONNECTION :
	         _SC.nfd = _SC_tcp_accept_connection(_SC.sfd, _SC.srvr);
		 if (_SC.nfd < 0)
		    SC_error(-1, "ACCEPT FAILED - SC_INIT_SERVER");

		 if (closep)
		    {close(_SC.sfd);

		     if (_SC_ps.debug)
		        {fprintf(_SC_ps.diag, "      Socket closed: %d\n", _SC.sfd);
			 fflush(_SC_ps.diag);};

		     _SC.sfd = -1;
		     CFREE(_SC.srvr);};

		 rv = _SC.nfd;

		 break;};};

    SC_ERR_UNTRAP();

#else
    rv = 0;
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INIT_CLIENT - initialize a client
 *                - return the socket which connects to the server
 */

int SC_init_client(char *host, int port)
   {int fd;

#ifdef HAVE_PROCESS_CONTROL
    fd = _SC_tcp_connect(host, port, -1, FALSE);
#else
    fd = 0;
#endif

    return(fd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_NUMBER_PROCESSORS - return the number of processors in the 
 *                          - current simulation
 */

int SC_get_number_processors(void)
   {int n;

    n = SC_gs.comm_size;
    n = max(n, 1);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_PROCESSOR_NUMBER - return the index of the current processor */

int SC_get_processor_number(void)
   {int n;

    n = SC_gs.comm_rank;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_PROCESSOR_INFO - set the SCORE rank and size if application
 *                       - initialized the parallel environment
 */

void SC_set_processor_info(int size, int rank)
   {

    SC_gs.comm_size = size;
    SC_gs.comm_rank = rank;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
