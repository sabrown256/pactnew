/*
 * SCHP.C - high level process control
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

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

#if defined(HAVE_POSIX_SYS)

#ifdef HAVE_PROCESS_CONTROL

static char
 *_SC_not_stopped = "running";

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RESET_TERMINAL - reset terminal state */

void SC_reset_terminal(void)
   {

    if (_SC.orig_tty != NULL)
       {SC_change_term_state(-1, SC_TERM_CUSTOM, TRUE, _SC.orig_tty);
        CFREE(_SC.orig_tty);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DETHREAD - work around OpenMP implementations that are bad
 *              - at over-scheduling threads
 */

void _SC_dethread(void)
   {

#if defined(SMP_OpenMP) && !defined(OpenMP_OVER_SCHED)
    omp_set_num_threads(1);
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DIAGNOSTIC - log a diagnostic message */

void _SC_diagnostic(char *fmt, ...)
   {SC_thread_proc *ps;

    ps = _SC_get_thr_processes(-1);

    if (ps->debug == TRUE)
       {char *s;

	SC_VDSNPRINTF(TRUE, s, fmt);

	fputs(s, ps->diag);
	SC_fflush_safe(ps->diag);

	CFREE(s);};

    return;}

/*--------------------------------------------------------------------------*/

/*                           EXEC TESTER ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SC_EXEC_TEST - test the exec system call */

NORETURN void _SC_exec_test(char **argv, char **envp, char *mode)
   {int i;
    PROCESS *pp;

    pp = SC_mk_process(argv, mode, SC_CHILD, -1);

    pp->medium     = USE_PIPES;
    pp->data_type  = SC_ASCII;

    for (i = 0; i < SC_N_IO_CH; i++)
        pp->io[i] = i;

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

#ifdef HAVE_PROCESS_CONTROL

static int _SC_complete_messagep(PROCESS *pp)
   {int rv;

    rv = PS_ring_ready(&pp->ior, pp->line_sep);

    return(rv);}

#endif

/*--------------------------------------------------------------------------*/

#ifdef HAVE_PROCESS_CONTROL

#ifdef SGI

/*--------------------------------------------------------------------------*/

static int _SC_open_pty_unix(PROCESS *pp, PROCESS *cp)
   {int i, pty, ok;
    char *pc;
    extern char *_getpty(int *pfd, int mode, mode_t perm, int flag);

    pc = _getpty(&pty, O_RDWR | O_NDELAY, 0600, FALSE);
    if (pc != NULL)
       {cp->spty = CSTRSAVE(pc);

	for (i = 0; i < SC_N_IO_CH; i++)
	    {cp->io[i] = pty;
	     pp->io[i] = pty;};

	ok = TRUE;}

    else
       ok = FALSE;

    return(ok);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* _SC_OPEN_PTY_UNIX - generic unix pseudo terminal open routine */

static int _SC_open_pty_unix(PROCESS *pp, PROCESS *cp)
   {int i, c, d, ok, pty, p;
    char *mpty, *spty, *pc, *pd;

    p = SC_get_perm(FALSE);

    c  = ' ';
    pc = NULL;
    pty = -1;
    for (pc = SC_MASTER_PTY_LETTERS; ((c = *pc) != '\0') && (pty < 0); pc++)
        {for (pd = SC_MASTER_PTY_DIGITS; (d = *pd) != '\0'; pd++)
	     {mpty = SC_dsnprintf(FALSE, "/dev/%s%c%c",
				  SC_MASTER_PTY_NAME, c, d);
	      pty = SC_open_safe(mpty, O_RDWR | O_NDELAY, p);
	      if (pty >= 0)
		 {spty = SC_dsnprintf(TRUE, "/dev/%s%c%c",
				      SC_SLAVE_PTY_NAME, c, d);
                  
		  cp->spty = spty;

		  for (i = 0; i < SC_N_IO_CH; i++)
		      {cp->io[i] = pty;
		       pp->io[i] = pty;};

		  break;};};};

    ok = (pty >= 0);

    return(ok);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

#ifdef HAVE_UNIX98_PTY

/*--------------------------------------------------------------------------*/

static int _SC_open_pty_unix98(PROCESS *pp, PROCESS *cp)
   {int i, ok, err, pty, p;
    char *ps;
    extern char *ptsname(int fd);
    extern int grantpt(int fd);
    extern int unlockpt(int fd);

    p = SC_get_perm(FALSE);

    ok   = FALSE;
    pty  = SC_open_safe("/dev/ptmx", O_RDWR|O_NOCTTY, p);
    err  = grantpt(pty);
    err |= unlockpt(pty);
    if ((pty >= 0) && (err == 0))
       {ps = ptsname(pty);
	cp->spty = CSTRSAVE(ps);

	for (i = 0; i < SC_N_IO_CH; i++)
	    {cp->io[i] = pty;
	     pp->io[i] = pty;};

	ok = TRUE;}

    else if (pty >= 0)
       SC_close_safe(pty);

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

#endif

/*--------------------------------------------------------------------------*/

/* SC_MK_PROCESS - initialize and return a PROCESS */

PROCESS *SC_mk_process(char **argv, char *mode, int type, int iex)
   {int i;
    unsigned int nb;
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

    pp->tstate      = _SC_tid_proc();

    pp->status      = NOT_FINISHED;
    pp->reason      = NOT_FINISHED;

    for (i = 0; i < SC_N_IO_CH; i++)
        pp->io[i] = -1;

    pp->data        = -1;
    pp->medium      = NO_IPC;
    pp->gone        = -1;
    pp->data_type   = SC_UNKNOWN;

    PS_ring_init(&pp->ior, nb);

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
    pp->pru = NULL;

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

# ifdef HAVE_SOCKETS
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

#ifdef HAVE_PROCESS_CONTROL

static int _SC_init_pty(PROCESS *pp, PROCESS *cp)
   {int ret, nr, nc, pw, ph;

    ret = (*open_pty)(pp, cp);
    if (ret == FALSE)
       SC_error(-1, "CANNOT OPEN PTY - _SC_INIT_IPC");

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

#endif

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

#ifdef HAVE_PROCESS_CONTROL

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

/* child stdin */
              if (pipe(ports) < 0)
                 {SC_close_safe(pp->io[0]);
                  SC_close_safe(cp->io[1]);
                  SC_error(-1, "COULD NOT CREATE PIPE #1 - _SC_INIT_IPC");};

              cp->io[0] = ports[0];
              pp->io[1] = ports[1];

/* child stdout */
              if (pipe(ports) < 0)
                 SC_error(-1, "COULD NOT CREATE PIPE #2 - _SC_INIT_IPC");

              cp->io[1] = ports[1];
              pp->io[0] = ports[0];

/* child stderr */
              if (pipe(ports) < 0)
                 {SC_close_safe(pp->io[0]);
                  SC_close_safe(cp->io[1]);
                  SC_error(-1, "COULD NOT CREATE PIPE #3 - _SC_INIT_IPC");};

              cp->io[2] = ports[0];
              pp->io[2] = ports[1];

              SC_set_fd_attr(pp->io[0], O_RDONLY | O_NDELAY, TRUE);
              SC_set_fd_attr(pp->io[1], O_WRONLY, TRUE);
              SC_set_fd_attr(pp->io[2], O_WRONLY, TRUE);
              SC_set_fd_attr(cp->io[0], O_RDONLY & ~O_NDELAY, TRUE);
              SC_set_fd_attr(cp->io[1], O_WRONLY, TRUE);
              SC_set_fd_attr(cp->io[2], O_WRONLY, TRUE);};

              break;

# ifdef HAVE_SOCKETS

        case USE_SOCKETS :
             {int ports[2];

/* child stdin */
              if (socketpair(PF_UNIX, SOCK_STREAM, 0, ports) < 0)
                 {SC_close_safe(pp->io[0]);
                  SC_close_safe(cp->io[1]);
                  SC_error(-1, "COULD NOT CREATE SOCKET PAIR #1 - _SC_INIT_IPC");};
              cp->io[0] = ports[0];
              pp->io[1] = ports[1];

/* child stdout */
              if (socketpair(PF_UNIX, SOCK_STREAM, 0, ports) < 0)
                 SC_error(-1, "COULD NOT CREATE SOCKET PAIR #2 - _SC_INIT_IPC");
              cp->io[1] = ports[1];
              pp->io[0] = ports[0];

/* child stderr */
              if (socketpair(PF_UNIX, SOCK_STREAM, 0, ports) < 0)
                 {SC_close_safe(pp->io[0]);
                  SC_close_safe(cp->io[1]);
                  SC_error(-1, "COULD NOT CREATE SOCKET PAIR #3 - _SC_INIT_IPC");};
              cp->io[2] = ports[1];
              pp->io[2] = ports[0];

              SC_set_fd_attr(pp->io[0], O_RDONLY | O_NDELAY, TRUE);
              SC_set_fd_attr(pp->io[1], O_WRONLY, TRUE);
              SC_set_fd_attr(pp->io[2], O_WRONLY, TRUE);
              SC_set_fd_attr(cp->io[0], O_RDONLY & ~O_NDELAY, TRUE);
              SC_set_fd_attr(cp->io[1], O_WRONLY, TRUE);
              SC_set_fd_attr(cp->io[2], O_WRONLY, TRUE);};

             break;

# endif

        default :
             ret = FALSE;};

#endif

    return(ret);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHILD_FORK - the child process comes here and
 *                - it will never return
 */

#ifdef HAVE_PROCESS_CONTROL

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

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PARENT_FORK - the parent process comes here */

#ifdef HAVE_PROCESS_CONTROL

static int _SC_parent_fork(PROCESS *pp, PROCESS *cp, int to,
                           int rcpu, char *mode)
   {int i, st;
   
    st = TRUE;

    pp->id         = cp->id;
    pp->rcpu       = rcpu;
    pp->start_time = SC_datef();
    pp->stop_time  = _SC_not_stopped;
    pp->pru        = SC_process_init_rusage();

    SC_process_state(pp, SC_PROC_EXEC);

#ifdef F_SETOWN
    if (pp->medium == USE_SOCKETS)
       fcntl(pp->io[0], F_SETOWN, pp->root);
#endif

    if (strchr(mode, 'o') != NULL)
       _SC_dethread();

    if (SC_BINARY_MODEP(mode))
       {if (!(*pp->recv_formats)(pp))
	   pp = NULL;};

    if (SC_process_alive(pp) && (pp->medium == USE_PTYS))
       {if (_SC.orig_tty != NULL)
	   SC_change_term_state(0, SC_TERM_RAW, FALSE, NULL);

	SC_change_term_state(pp->io[0], SC_TERM_RAW, FALSE, NULL);

	for (i = 0; i < SC_N_IO_CH; i++)
	    cp->io[i] = -1;};

    cp->release(cp);
    cp = NULL;

    return(st);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ERROR_FORK - come here if fork failed
 *                - just cleanup the remains
 */

#ifdef HAVE_PROCESS_CONTROL

static void _SC_error_fork(PROCESS *pp, PROCESS *cp)
   {

/* mark this as having to do with the child so that
 * the managed process list works right
 */
    pp->ischild = TRUE;

    pp->release(pp);

    cp->release(cp);

    SC_error(-1, "COULD NOT FORK PROCESS - _SC_ERROR_FORK");

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_GROUP_TASKS - scan ARGV for pipe indicators and
 *                       - NULL out those entries
 *                       - return a list of offsets into ARGV for each
 *                       - individual process
 *                       - in effect break ARGV into multiple ARGVs
 *                       - one for each process
 */

#ifdef HAVE_PROCESS_CONTROL

static subtask *_SC_make_group_tasks(char **argv, char **env)
   {int i, n, na;
    char cmd[MAXLINE];
    char **al;
    subtask *pg;

    n = 1;
    for (i = 0; argv[i] != NULL; i++)
        n += ((strcmp(argv[i], SC_PIPE_DELIM) == 0) ||
	      (argv[i][0] == SC_PROCESS_DELIM));

    pg = CMAKE_N(subtask, n+1);
    SC_MEM_INIT_N(subtask, pg, n+1);

    n          = 0;
    pg[n].argf = argv;
    pg[n].env  = env;

    for (i = 0; argv[i] != NULL; i++)
        {if ((strcmp(argv[i], SC_PIPE_DELIM) == 0) ||
	     (argv[i][0] == SC_PROCESS_DELIM))
            {pg[n].ios  = argv[i];

	     if (argv[i+1] != NULL)
	        {n++;
		 pg[n].argf = argv + i + 1;
		 pg[n].env  = env;};

	     argv[i] = NULL;};};

    for (i = 0; i <= n; i++)
        {al = pg[i].argf;
	 SC_ptr_arr_len(na, al);
	 SC_concatenate(cmd, MAXLINE, al, 0, na, " ", FALSE);
	 pg[i].command = CSTRSAVE(cmd);
	 pg[i].kind    = TASK_GROUP;};

    n++;
    pg[n].command = NULL;

    return(pg);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_GROUP_N - return the number of processes
 *                     - in the process_group PG
 */

#ifdef HAVE_PROCESS_CONTROL

static int _SC_process_group_n(subtask *pg)
   {int n;

    for (n = 0; pg[n].command != NULL; n++);

    return(n);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_IO_KIND - return the SC_io_kind of IOS in KND
 *             - the SC_io_device of IOS in DEV
 *             - and the file mode in MODE
 *             - the mode is for >, >>, or >!
 */

int _SC_io_kind(SC_iodes *pio, char *ios)
   {int ck, cd, fid, gid, nc, rv;
    SC_io_mode mode;
    SC_io_device dev;
    SC_io_kind knd;

    rv = FALSE;

    dev  = SC_IO_DEV_NONE;
    knd  = SC_IO_STD_NONE;
    mode = SC_IO_MODE_NONE;

    gid = pio->gid;
    fid = pio->fid;
    if (ios == NULL)
       ios = pio->raw;

    if (ios[0] == SC_PROCESS_DELIM)
       ios++;

    nc = 0;
    ck = ios[nc++];
    cd = ios[nc++];

    if (ios != NULL)

/* get >&, &>, >>&, and &>> */
       {if ((strstr(ios, ">&") != NULL) ||
	    (strstr(ios, "&>") != NULL))
	   {knd = SC_IO_STD_BOND;
	    dev = SC_IO_DEV_FILE;}

        else if (strncmp(ios, "1>", 2) == 0)
	   {knd = SC_IO_STD_OUT;
	    dev = SC_IO_DEV_FILE;}

	else if (strncmp(ios, "2>", 2) == 0)
	   {knd = SC_IO_STD_ERR;
	    dev = SC_IO_DEV_FILE;}

	else if ((strstr(ios, "|&") != NULL) ||
		 (strstr(ios, "&|") != NULL))
	   {knd = SC_IO_STD_BOND;
	    dev = SC_IO_DEV_PIPE;}

	else if (strchr(ios, '|') != NULL)
	   {knd = SC_IO_STD_OUT;
	    dev = SC_IO_DEV_PIPE;}

	else
	   {switch (ck)
	       {case 'e' :
		     knd = SC_IO_STD_ERR;
		     break;
		case '<' :
		     cd = ck;
	        case 'i' :
		     knd = SC_IO_STD_IN;
		     break;
		case '>' :
		     cd = ck;
	        case 'o' :
		     knd = SC_IO_STD_OUT;
		     break;};};

/* next for the device type */
	switch (cd)
	   {case '+' :
	    case '-' :
	         dev = SC_IO_DEV_PTY;
	         dev = SC_IO_DEV_SOCKET;
	         dev = SC_IO_DEV_PIPE;
		 nc++;
		 break;
	    case 'e' :
	    case '&' :
	         dev = SC_IO_DEV_EXPR;
		 break;
	    case 'f' :
	    case 'w' :
	    case '!' :
	    case 'a' :
	    case '>' :
	         dev = SC_IO_DEV_FILE;
		 break;
	    case 'v' :
	         dev = SC_IO_DEV_VAR;
		 break;
	    case 't' :
	         dev = SC_IO_DEV_TERM;
		 break;};

	if (dev == SC_IO_DEV_PIPE)
	   gid = SC_stoi(ios+nc);

/* now for the mode */
	if (strstr(ios, "<") != NULL)
	   mode = SC_IO_MODE_RO;
	else if (strstr(ios, ">>") != NULL)
	   mode = SC_IO_MODE_APPEND;
	else if (strchr(ios, '!') != NULL)
	   mode = SC_IO_MODE_WD;
	else if (strstr(ios, ">") != NULL)
	   mode = SC_IO_MODE_WO;
	else if (strchr("ioeb", ios[0]) != NULL)
	   {switch (cd)
	       {case 'f' :
                     mode = SC_IO_MODE_WO;
                     break;
		case 'w' :
                     mode = SC_IO_MODE_WD;
                     break;
		case 'a' :
                     mode = SC_IO_MODE_APPEND;
		     break;};};

	rv = TRUE;};

    pio->knd  = knd;
    pio->fid  = fid;
    pio->gid  = gid;
    pio->dev  = dev;
    pio->mode = mode;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_KIND_IO - return the character matching SC_io_kind K */

int _SC_kind_io(SC_io_kind k)
   {int rv;

    switch (k)
       {case SC_IO_STD_IN :
	     rv = 'i';
	     break;
        case SC_IO_STD_OUT :
	     rv = 'o';
	     break;
        case SC_IO_STD_ERR :
	     rv = 'e';
	     break;
        case SC_IO_STD_BOND :
	     rv = 'b';
	     break;
        default :
	     rv = '\0';
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RECONNECT_PROCESS_GROUP - reconnect the N PROCCESS's in PA and CA
 *                             - from the canonical parent/child topology
 *                             - into a process_group topology
 *                             - NOTE: this logic is good for both pipes and
 *                             - sockets but bad for PTYs
 *                             - on the other hand why do PTYs
 *                             - in a process_group
 */

#ifdef HAVE_PROCESS_CONTROL

static void _SC_reconnect_process_group(int n, subtask *pg,
					PROCESS **pa, PROCESS **ca)
   {int i, ido, nm;
    subtask *g;
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
	        {SC_set_fd_attr(pp->io[0], O_RDWR,   -1);
		 SC_set_fd_attr(pp->io[1], O_RDWR,   -1);
		 SC_set_fd_attr(pp->io[2], O_WRONLY, -1);
		 SC_set_fd_attr(cp->io[0], O_RDWR,   -1);
		 SC_set_fd_attr(cp->io[1], O_RDWR,   -1);
		 SC_set_fd_attr(cp->io[2], O_WRONLY, -1);};};

/* reconnect terminal process output to first process */
	pp = pa[nm];
	pn = pa[0];
	SC_close_safe(pp->io[1]);
	pp->io[1] = pn->io[1];
	pn->io[1] = -2;

/* close all other parent to child lines */
	for (i = 1; i < nm; i++)
	    {pp = pa[i];
	     cp = ca[i];

	     SC_close_safe(pp->io[1]);
	     SC_close_safe(cp->io[0]);

	     pp->io[1] = -2;
	     cp->io[0] = -2;};

/* connect all non-terminal child out to next child in */
	for (i = 0; i < nm; i++)
	    {g = pg + i;

	     ido = g->fd[SC_IO_STD_OUT].gid;

	     pp = pa[i];
	     cp = ca[i];
	     cn = ca[ido];

	     SC_close_safe(cn->io[0]);

	     cn->io[0] = pp->io[0];
	     pp->io[0] = -2;};};

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SETUP_PROC - allocate PROCESS structures for the parent (PP)
 *                - and the child (CP)
 *                - do the initial I/O setup and any other
 *                - preparation prior to the fork
 *                - return the timeout in milliseconds for the post-fork
 *                - setup and exec
 */

#ifdef HAVE_PROCESS_CONTROL

static int _SC_setup_proc(PROCESS **ppp, PROCESS **pcp,
			  char **argv, char *mode,
			  SC_iodes *fd, int retry, int iex,
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
       _SC.orig_tty = SC_term_get_state(0, TRUE);

/* set up the communications pipe */
    flag = _SC_init_ipc(pp, cp);
    if (flag == FALSE)
       {CFREE(pp->tty);
	CFREE(pp);
	CFREE(cp->tty);
        CFREE(cp);
        SC_error(-1, "COULD NOT CREATE IPC CHANNELS - _SC_SETUP_PROC");
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

#endif

/*--------------------------------------------------------------------------*/

#ifdef HAVE_PROCESS_CONTROL

/*--------------------------------------------------------------------------*/

/* _SC_SET_IO_SPEC - initialize an SC_iodes specification PG->fd[KIND]
 *                 - according to LINK whose syntax is:
 *                 -   <ios><gid><iod>
 *                 - where
 *                 -   <ios>  := 'i' | 'o' | 'e' | 'b'
 *                 -   <gid>  := <n> | +<n> | -<n> | <expr>
 *                 -   <expr> := 't' | <var> | <file>
 *                 -   <var>  := environment variable
 *                 -   <file> := file name
 */

static void _SC_set_io_spec(subtask *pg, int n, int id,
			    SC_io_kind kind, char *link)
   {int fd, gid, nc, md, prm, excl;
    SC_io_device dev;
    SC_iodes ios, iod;
    SC_iodes *ips, *ipd;
    char s[MAXLINE];
    char *p, *ps, *nm;
    subtask *pgs, *pgd;

    if ((0 <= id) && (id < n))
       {pgs  = pg + id;
	fd  = -1;
	gid = -1;
	dev = SC_IO_DEV_NONE;

	SC_strncpy(s, MAXLINE, link, -1);
	nc  = strlen(s);
	_SC_io_kind(&ios, s);
	_SC_io_kind(&iod, s + nc - 1);

	SC_ASSERT(ios.knd == kind);

	ps = s + 1;
	switch (ps[0])
	   {case '+' :
	    case '-' :
	         dev = SC_IO_DEV_PIPE;
		 gid = pgs->id + SC_stoi(ps);
		 break;
	    case 'e' :
		 dev = SC_IO_DEV_EXPR;
		 break;

	    case 'a' :
	    case 'f' :
	    case 'w' :
	         nm  = ps + 1;
		 dev = SC_IO_DEV_FILE;

/* GOTCHA: what should the real permissions be? */
		 prm = 0600;
		 prm = SC_get_perm(FALSE);

/* do not try to use O_EXCL bit with devices - think about it */
		 if (strncmp(nm, "/dev/", 5) == 0)
		    excl = 0;
		 else
		    excl = O_EXCL;

/*  <   ->  O_RDONLY */
		 if (kind == SC_IO_STD_IN)
		    fd = SC_open_safe(nm, O_RDONLY, prm);
		 else
		    {switch (ps[0])

/*  >   ->  O_WRONLY | O_CREAT | O_EXCL */
		        {case 'f' :
			      md = O_WRONLY | O_CREAT | excl;
			      break;

/*  >!  ->  O_WRONLY | O_CREAT | O_TRUNC */
			 case 'w' :
			 default  :
			      md = O_WRONLY | O_CREAT | O_TRUNC;
			      break;

/*  >>  ->  O_WRONLY | O_APPEND */
			 case 'a' :
			      md = O_WRONLY | O_APPEND;
			      break;};
		     fd = SC_open_safe(nm, md, prm);};
		 break;

	    case 's' :
		 dev = SC_IO_DEV_SOCKET;
		 {int to, fm, port;
		  char *host;

		  to = 10000;         /* timeout after 10 seconds */
		  fm = FALSE;         /* fatal on timeout */

		  host = ps + 1;
		  p    = strchr(host, ':');
		  *p++ = '\0';
		  port = SC_stoi(p);

		  fd = _SC_tcp_connect(host, port, to, fm);};
		 break;
	    case 't' :
		 dev = SC_IO_DEV_TERM;
		 s[nc-1] = '\0';
		 break;
	    case 'v' :
		 dev = SC_IO_DEV_VAR;
		 break;
	    default :
		 dev = SC_IO_DEV_PIPE;
		 if (SC_numstrp(ps) == TRUE)
		    gid = SC_stoi(ps) - 1;
		 else
		    {s[nc-1] = '\0';
		     if (SC_numstrp(ps) == TRUE)
		        gid = SC_stoi(ps) - 1;};
		 break;};

/* set the destination io spec for a pipe */
	if ((dev == SC_IO_DEV_PIPE) &&
	    (0 <= gid) && (gid < n))
	   {pgd = pg + gid;
	    if ((0 <= iod.knd) && (iod.knd < (int) SC_N_IO_CH))
	       {ipd = pgd->fd + iod.knd;
		if (ipd != NULL)
		   {ipd->knd = iod.knd;
		    ipd->dev = dev;
		    ipd->fd  = fd;
		    ipd->gid = id;};};};

/* set the source io spec */
	if ((0 <= ios.knd) && (ios.knd < (int) SC_N_IO_CH))
	   {ips = pgs->fd + ios.knd;
	    if (ips != NULL)
	       {ips->knd = ios.knd;
		ips->dev = dev;
		ips->fd  = fd;
		ips->gid = gid;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_IO_SPEC - initialize an SC_iodes specification PG->fd[KIND] */

static void _SC_init_io_spec(subtask *pg, int n, int id)
   {

    pg->fd[SC_IO_STD_IN].knd = SC_IO_STD_IN;
    pg->fd[SC_IO_STD_IN].fd  = -1;
    pg->fd[SC_IO_STD_IN].gid = pg->id - 1;

    pg->fd[SC_IO_STD_OUT].knd = SC_IO_STD_OUT;
    pg->fd[SC_IO_STD_OUT].fd  = -1;
    pg->fd[SC_IO_STD_OUT].gid = (id < n-1) ? pg->id + 1 : -1;

    pg->fd[SC_IO_STD_ERR].knd = SC_IO_STD_ERR;
    pg->fd[SC_IO_STD_ERR].fd  = -1;
    pg->fd[SC_IO_STD_ERR].gid = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_GROUP_PARSE - parse out the I/O specifications for the ID job
 *                         - out of the N PG
 */

static void _SC_process_group_parse(subtask *pg, int n, int id)
   {int i;
    char **sa;
    subtask *pgs;

    if ((pg != NULL) && (id >= 0))
       {pgs = pg + id;

	pgs->id   = id;
	pgs->exit = -1;

	_SC_init_io_spec(pgs, n, id);
	if (pgs->ios != NULL)
	   {sa = SC_tokenize(pgs->ios, "@");

	    for (i = 0; sa[i] != NULL; i++)
	        {switch (sa[i][0])
		    {case 'i' :
		          _SC_set_io_spec(pg, n, id, SC_IO_STD_IN,  sa[i]);
			  break;
		     case 'o' :
			  _SC_set_io_spec(pg, n, id, SC_IO_STD_OUT, sa[i]);
			  break;
		     case 'e' :
			  _SC_set_io_spec(pg, n, id, SC_IO_STD_ERR, sa[i]);
			  break;
		     case 'b' :
			  _SC_set_io_spec(pg, n, id, SC_IO_STD_OUT, sa[i]);
			  _SC_set_io_spec(pg, n, id, SC_IO_STD_ERR, sa[i]);
			  break;};};

	    SC_free_strings(sa);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SETUP_PROCESS_GROUP - setup and return a process group
 *                         - as specified in ARGV
 */

static SC_process_group *_SC_setup_process_group(char **argv, char **envp,
						 char *mode, SC_iodes *fd,
						 int retry, int iex,
						 int rcpu,
						 PFProcInit initf,
						 PFProcExit exitf,
						 void *exita)
   {int i, n, to;
    char *mod, **al;
    PROCESS **pa, **ca;
    subtask *pg;
    SC_iodes *pfd;
    SC_process_group *pgr;

    pg = _SC_make_group_tasks(argv, envp);
    n  = _SC_process_group_n(pg);

    pa = CMAKE_N(PROCESS *, n);
    ca = CMAKE_N(PROCESS *, n);

    to = -1;

/* setup each process in the process_group */
    for (i = 0; i < n; i++)
        {al = pg[i].argf;

/* NOTE: use sockets for the non-terminal process */
	 mod = (i < n-1) ? "as" : mode;
	 pfd = (i < n-1) ? NULL : fd;

	 to = _SC_setup_proc(&pa[i], &ca[i], al, mod,
			     pfd, retry, iex, initf, exitf, exita);};

/* parse out the process group links */
    for (i = 0; i < n; i++)
        _SC_process_group_parse(pg, n, i);

    _SC_reconnect_process_group(n, pg, pa, ca);

    pgr = CMAKE(SC_process_group);
    if (pgr != NULL)
       {pgr->np       = n;
	pgr->to       = to;
	pgr->rcpu     = rcpu;
	pgr->mode     = mode;
	pgr->jobs     = pg;
	pgr->terminal = NULL;
	pgr->parents  = pa;
	pgr->children = ca;};

    return(pgr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_PROCESS_GROUP - release a process group instance */

static void _SC_free_process_group(SC_process_group *pgr)
   {

    if (pgr != NULL)
       {CFREE(pgr->jobs);
	CFREE(pgr->parents);
	CFREE(pgr->children);
	CFREE(pgr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LAUNCH_PROCESS_GROUP - fork and exec each job
 *                          - in the process group PGR
 *                          - return the terminal process
 */

static PROCESS *_SC_launch_process_group(SC_process_group *pgr)
   {int i, n, pid, to, st, rcpu;
    char *md, **al, **el;
    PROCESS *pp, *cp, **pa, **ca;
    subtask *pg;

    n    = pgr->np;
    to   = pgr->to;
    pg   = pgr->jobs;
    pa   = pgr->parents;
    ca   = pgr->children;
    md   = pgr->mode;
    rcpu = pgr->rcpu;

    pp = NULL;

/* launch each process in the process_group */
    for (i = 0; i < n; i++)
        {pp = pa[i];
	 cp = ca[i];

         al = pg[i].argf;
	 el = pg[i].env;

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
		  _SC_child_fork(pp, cp, to, al, el, md);
		  break;

/* the parent process comes here */
	     default :
	          st = _SC_parent_fork(pp, cp, to, rcpu, md);
		  if (st == FALSE)
		     {SC_close(pp);
		      pp = NULL;};
		  break;};};

    return(pp);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SC_OPEN_PROC - start up a process on the current CPU
 *               - and open a connection to it for further communications
 */

static PROCESS *_SC_open_proc(int rcpu, char *name, char **argv,
			      char **envp, char *mode,
			      SC_iodes *fd, int retry, int iex,
			      PFProcInit initf, PFProcExit exitf, void *exita)
   {PROCESS *pp;

    pp = NULL;

#ifdef HAVE_PROCESS_CONTROL

    SC_process_group *pgr;

    if (_SC_redir_fail(fd) != -1)
       return(NULL);

    pgr = _SC_setup_process_group(argv, envp, mode, fd, retry, iex, rcpu,
				  initf, exitf, exita);

    pp  = _SC_launch_process_group(pgr);

    _SC_free_process_group(pgr);

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
   {int rv;

    rv = PS_ring_push(&pp->ior, bf, n);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_BUFFER_OUT - return the oldest message from the ring in BF */

int SC_buffer_out(char *bf, PROCESS *pp, int n, int binf)
   {int i;

    i = PS_ring_pop(&pp->ior, bf, n, pp->line_sep);

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

#ifdef HAVE_PROCESS_CONTROL

static char *_SC_get_input(char *bf, int len, PROCESS *pp)
   {int status, blck, reset, run, nb, st;
    char *pbf;
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
       {SC_block_fd(pp->io[0]);
        status = FALSE;
        reset  = FALSE;}

/* if blocked when should be unblocked */
    else if (!status && !blck)
       {SC_unblock_fd(pp->io[0]);
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
	     SC_unblock_fd(pp->io[0]);
	     break;
        case 1 :
	     SC_block_fd(pp->io[0]);
	     break;};

    return(pbf);}

#endif

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

int _SC_redir_fail(SC_iodes *fd)
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
       {nm = fd[0].file;
	fl = fd[0].flag;
	if ((nm != NULL) && (SC_isfile(nm) == FALSE))
	   fail = 0;};

/* stdout redirection fails if file exists and mode has exclusive bit */
    if (fail == -1)
       {nm = fd[1].file;
	fl = fd[1].flag;
	if ((nm != NULL) && (SC_isfile(nm) == TRUE) && (fl & O_EXCL))
	   fail = 1;};

/* stderr redirection fails if file exists and mode has exclusive bit */
    if (fail == -1)
       {nm = fd[2].file;
	fl = fd[2].flag;
	if ((nm != NULL) && (SC_isfile(nm) == TRUE) && (fl & O_EXCL))
	   fail = 2;};

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REDIR_FILEDES - set the parts of FD implied by REDIR
 *                   - NAME specifies the file to be used
 */

void _SC_redir_filedes(SC_iodes *fd, int nfd, int ifd, SC_iodes *pio)
   {int ofd, excl, fl;
    int flc, flt, fla;
    SC_io_kind knd;
    SC_io_mode md;
    char *nm, *name;

    name = pio->file;
    md   = pio->mode;
    knd  = pio->knd;

    if (name != NULL)
       {if (SC_numstrp(name))
	   {ofd = SC_stoi(name);
	    if (ofd >= nfd)
	       return;
	    nm  = fd[ofd].file;
	    fl  = fd[ofd].flag;}
	else
	   {ofd = ifd;
	    nm  = name;
	    fl  = -1;};

/* do not try to use O_EXCL bit with devices - think about it */
	if ((nm != NULL) && (strncmp(nm, "/dev/", 5) == 0))
	   excl = 0;
	else
	   excl = O_EXCL;

/* work redirection syntax */
	flc = O_WRONLY | O_CREAT | excl;
	flt = O_WRONLY | O_CREAT | O_TRUNC;
	fla = O_WRONLY | O_CREAT | O_APPEND;

/* shell redirection syntax and file mode correspondence
 *  <   ->  O_RDONLY
 *  >   ->  O_WRONLY | O_CREAT | O_EXCL
 *  >!  ->  O_WRONLY | O_CREAT | O_TRUNC
 *  >>  ->  O_WRONLY | O_APPEND
 */
	switch (knd)
	   {case SC_IO_STD_IN :
		 fl = O_RDONLY;
	         break;
	    case SC_IO_STD_OUT :
	         switch (md)
		    {case SC_IO_MODE_WD :
		          fl = flt;
			  break;
		     case SC_IO_MODE_APPEND :
		          fl = fla;
			  break;
		     default :
		          fl = flc;
			  break;};
	         break;
	    case SC_IO_STD_ERR :
	         break;
	    case SC_IO_STD_BOND :
	         switch (md)
		    {case SC_IO_MODE_WD :
		          fl = (fl == -1) ? flt : fl;
			  break;
		     case SC_IO_MODE_APPEND :
		          fl = (fl == -1) ? fla : fl;
			  break;
		     default :
		          fl = (fl == -1) ? flc : fl;
			  break;};
	         break;
	    default :
	         break;};

/* set the files */
	switch (knd)
	   {case SC_IO_STD_IN :
	         fd[ifd].file = nm;
	         break;
	    case SC_IO_STD_OUT :
		 fd[ifd].file = nm;
	         break;
	    case SC_IO_STD_ERR :
	         break;
	    case SC_IO_STD_BOND :
		 fd[1].file = nm;
		 fd[2].file = nm;
	         break;
	    default :
	         break;};

/* set the flags */
	switch (knd)
	   {case SC_IO_STD_IN :
		 fd[ifd].flag = fl;
	         break;
	    case SC_IO_STD_OUT :
		 fd[ifd].flag = fl;
	         break;
	    case SC_IO_STD_ERR :
		 fd[2].flag = fl;
	         break;
	    case SC_IO_STD_BOND :
		 fd[1].flag = fl;
		 fd[2].flag = fl;
	         break;
	    default :
	         break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_FILEDES - set the parts of FD implied by REDIR
 *                 - NAME specifies the file to be used
 */

void _SC_set_filedes(SC_iodes *fd, int ifd, char *name, int fl)
   {char *nm;

    if (name != NULL)
       {nm = CSTRSAVE(name);

	fd[ifd].file = nm;
	fd[ifd].flag = fl;
	fd[ifd].dev  = SC_IO_DEV_FILE;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIN_FILEDES - release the contents of the SC_iodes' FD */

void _SC_fin_filedes(SC_iodes *fd)
   {int i, n;

    if (fd[2].file != fd[1].file)
       n = 3;
    else
       n = 2;

    for (i = 0; i < n; i++)
        {CFREE(fd[i].file);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_FILEDES - initialize a set of SC_N_IO_CH SC_iodes structures */

void _SC_init_filedes(SC_iodes *fd)
   {int i;

    for (i = 0; i < SC_N_IO_CH; i++)
        {fd[i].fd    = -1;
	 fd[i].fid   = -1;
	 fd[i].gid   = -1;
	 fd[i].flag  = -1;
	 fd[i].knd   = SC_IO_STD_NONE;
	 fd[i].dev   = SC_IO_DEV_NONE;
	 fd[i].mode  = SC_IO_MODE_NONE;
	 fd[i].file  = NULL;
	 fd[i].raw   = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_COPY_FILEDES - copy a set of SC_N_IO_CH SC_iodes FA
 *                  - into another FB
 */

void _SC_copy_filedes(SC_iodes *fb, SC_iodes *fa)
   {int i;

    for (i = 0; i < SC_N_IO_CH; i++)
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
    SC_iodes fd[SC_N_IO_CH];
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
		else if (strcmp(key, "IO-DES") == 0)
		   {int nfd;
		    SC_iodes *pfd;

		    pfd = SC_VA_ARG(SC_iodes *);
		    nfd = SC_VA_ARG(int);
		    for (ifd = 0; ifd < nfd; ifd++)
		        fd[ifd] = pfd[ifd];}
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
    
    _SC_diagnostic("   SC_init_server: %d\n", step);

    rv = -1;
    ok = SC_ERR_TRAP();
    if (ok != 0)
       {if (closep)
	   {if (_SC.sfd >= 0)
	       SC_close_safe(_SC.sfd);
	    _SC.sfd = -1;};

	rv = _SC.sfd;}

    else
       {switch (step)
	   {case SC_GET_PORT :
	         _SC.sfd = socket(PF_INET, SOCK_STREAM, 0);
		 if (_SC.sfd < 0)
		    SC_error(-1, "COULD NOT OPEN SOCKET - SC_INIT_SERVER");

		 _SC_diagnostic("      Socket opened: %d\n", _SC.sfd);

		 _SC.srvr = _SC_tcp_bind(_SC.sfd, -1);
		 if (_SC.srvr.in == NULL)
		    SC_error(-1, "BIND FAILED - SC_INIT_SERVER");
		 else
		    rv = ntohs(_SC.srvr.in->sin_port);

		 break;

	    case SC_GET_CONNECTION :
	         _SC.nfd = _SC_tcp_accept_connection(_SC.sfd, _SC.srvr);
		 if (_SC.nfd < 0)
		    SC_error(-1, "ACCEPT FAILED - SC_INIT_SERVER");

		 if (closep)
		    {SC_close_safe(_SC.sfd);

		     _SC_diagnostic("      Socket closed: %d\n", _SC.sfd);

		     _SC.sfd = -1;
		     CFREE(_SC.srvr.in);};

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

#endif
