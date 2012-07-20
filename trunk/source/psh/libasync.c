/*
 * LIBASYNC.C - asynchronous execution and I/O for shell like functionality
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 * There are two API's here:
 *
 * 1) Single job
 *
 *    process *job_launch(char *cmd, char *mode, void *a)
 *       Launch a job to do CMD in MODE.  A single variable A is
 *       passed to any callbacks associated with the job.
 *
 *    int job_read(process *pp, void (*out)(process *pp, char *s))
 *       Read from the job PP and pass anything read to the callback OUT.
 *
 *    int job_write(process *pp, char *fmt, ...)
 *       Write a message to the job PP.
 *
 *    int job_response(process *pp, int to, char *fmt, ...)
 *       Write a message to job PP and give all output from the job
 *       in response to the method of PP for that purpose.
 *
 *    void job_wait(process *pp)
 *       Do a non-blocking wait and set the state of PP accordingly.
 *
 *    int job_signal(process *pp, int sig)
 *       Send SIG to the job PP.
 *
 *    int job_done(process *pp, int sig)
 *       End the job PP using SIG.
 *
 *
 * 2) Multiple asynchronous jobs
 *
 *    void asetup(int n, int na)
 *       Prepare N jobs to be run asynchronously.  Each job may
 *       be attempted up to NA times using arelaunch.
 *
 *    process *alaunch(int ip, char *cmd, char *mode, void *a,
 *                     void (*acc)(process *pp, char *s),
 *                     void (*rej)(process *pp, char *s),
 *                     void (*wt)(process *pp))
 *       Launch the next prepared job to do CMD in MODE.  Attach callbacks
 *       to handle accepted and rejected (according to poll) messages
 *       and to act on successful a wait.
 *
 *    process *arelaunch(process *pp)
 *       Relaunch the job PP, assuming prior attempts failed for
 *       transient causes.  You may relaunch up to the maximum number
 *       specified in the asetup call.
 *
 *    int apoll(int to)
 *       Wait TO milliseconds for I/O from the jobs.  Hand any I/O
 *       to the appropriate callback.
 *
 *    int acheck(void)
 *       Do a job_wait on each job and return the number of jobs which
 *       are done.
 *
 *    int await(unsigned int tf, int dt, char *tag,
 *              int (*tty)(char *tag),
 *              void (*f)(int i, char *tag, void *a, int nd, int np, int tc, int tf),
 *              void *a)
 *       Monitor the jobs until TF seconds have elapsed, all jobs
 *       are done, or TTY returns FALSE.  Call TTY and F each cycle.
 *       Poll for DT milliseconds in each cycle.
 *
 *    void amap(void (*f)(process *pp, void *a))
 *       Apply the function F to each job.
 *
 *    void afin(void (*f)(process *pp, void *a))
 *       Finish up the asynchronous jobs.  The function F is called
 *       for each one to give access to the final job state.
 *
 *  Template/Example:
 *     n = 4;
 *     asetup(n, 3);
 *
 *  * launch the jobs - io_data passed to macc, myrej, and mydone *
 *     for (i = 0; i < n; i++)
 *         alaunch(i, cmd[i], "r", io_data, myacc, myrej, mydone);
 *
 *  * wait for the work to complete - work_data passed to mywork *
 *     tc = await(30, 100, "commands", mytty, mywork, work_data);
 *
 *  * close out the jobs *
 *     afin(myfin);
 *
 */

#ifndef LIBASYNC

#define LIBASYNC

#include "common.h"

#define JOB_NOT_FINISHED    -1000
#define N_IO_CHANNELS       3

/* job status values */
#define JOB_RUNNING      0x0
#define JOB_STOPPED      0x1
#define JOB_CHANGED      0x2
#define JOB_EXITED       0x4
#define JOB_COREDUMPED   0x8
#define JOB_SIGNALED     0x10
#define JOB_KILLED       0x20
#define JOB_DEAD         0x40
#define JOB_RESTART      0x80

#define JOB_NO_SETSID  72
#define JOB_NO_TTY     73
#define JOB_NO_CLOSE   74
#define JOB_NO_FMT     75
#define JOB_NO_EXEC    76

#define job_alive(pp)                                                             \
   ((pp != NULL) && (pp->io[0].fd != -1))

#define job_running(pp)                                                           \
   ((pp != NULL) && (pp->io[0].fd != -1) && (pp->status == JOB_RUNNING))

typedef void (*PFSIGHand)(int sig);
typedef void (*PFIOHand)(int fd, int mask, void *a);

enum e_io_hand
   {IO_HND_NONE, IO_HND_CLOSE, IO_HND_PIPE, IO_HND_POLL};

typedef enum e_io_hand io_hand;

enum e_io_kind
   {IO_STD_NONE = -1, IO_STD_IN, IO_STD_OUT, IO_STD_ERR, IO_STD_BOND};

typedef enum e_io_kind io_kind;

enum e_io_device
   {IO_DEV_NONE,
    IO_DEV_PIPE, IO_DEV_SOCKET, IO_DEV_PTY, 
    IO_DEV_TERM, IO_DEV_FILE, IO_DEV_VAR, IO_DEV_EXPR };

typedef enum e_io_device io_device;

enum e_io_mode
   {IO_MODE_NONE,
    IO_MODE_RO, IO_MODE_WO, IO_MODE_WD, IO_MODE_APPEND };

typedef enum e_io_mode io_mode;

enum e_st_sep
   {ST_NEXT, ST_AND, ST_OR};

typedef enum e_st_sep st_sep;

typedef struct pollfd apollfd;
typedef struct s_process process;
typedef struct s_process_group process_group;
typedef struct s_process_stack process_stack;
typedef struct s_iodes iodes;

struct s_iodes
   {int fd;
    int fid;                /* file descriptor for redirect - 2>&1 type */
    int gid;                /* index of process group member for redirect */
    int flag;               /* file open mode flags */
    io_hand hnd;
    io_kind knd;
    io_device dev;
    io_mode mode;
    char *file;
    char **raw;
    FILE *fp;};

struct s_process
   {int ip;                                                /* process index */
    int id;                                      /* process id of the child */
    int pgid;                                  /* OS level process group id */
    int status;            /* process status - JOB_RUNNING, JOB_EXITED, ... */
    int reason; /* reason for latest status change - also child exit status */
    int ischild;                             /* TRUE iff started as a child */
    int nattempt;                   /* count the times the job is attempted */
    char *cmd;
    char **ios;
    char **arg;
    char **env;
    char *shell;
    char mode[10];

    iodes io[N_IO_CHANNELS];

    double start_time;
    double stop_time;

    int (*accept)(int fd, process *pp, char *s);   /* accept messages read from job */
    int (*reject)(int fd, process *pp, char *s);   /* reject messages read from job */
    void (*wait)(process *pp);              /* call when wait says its done */
    void *a;};                 /* external data associated with the process */

struct s_process_group
   {int np;                 /* number of processes in group */
    int to;                 /* group time out */
    char *mode;             /* IPC mode */
    char *shell;
    char **env;
    process *terminal;      /* terminal process */
    process **parents;      /* parent process array */
    process **children;};   /* child process array */

struct s_process_stack
   {int ip;
    int np;
    int ifd;
    int nfd;
    int nattempt;
    int mask_acc;
    int mask_rej;
    int *map;
    int *io;
    apollfd *fd;
    process **proc;};

static process_stack
 stck = { 0, 0, 0, 0, 3,
          (POLLIN | POLLPRI),
	  (POLLERR | POLLHUP | POLLNVAL),
          NULL, NULL, NULL, NULL};

static sigjmp_buf
 _job_cpu;

/*--------------------------------------------------------------------------*/

/*                              INFRASTRUCTURE                              */

/*--------------------------------------------------------------------------*/

/* _FD_CLOSE - wrap close to be able to monitor/log them all */

int _fd_close(int fd)
   {int rv;

#ifdef DEBUG
    fprintf(stderr, "dbg> closing %d\n", fd);
#endif

    rv = close(fd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_IO_CLOSE - close the KND I/O channel of PP */

void _job_io_close(process *pp, io_kind knd)
   {int fd;
    FILE *fp;

    fd = pp->io[knd].fd;
    fp = pp->io[knd].fp;

    if (fd >= 0)
       _fd_close(fd);

    if (fp != NULL)
       fclose(fp);

    pp->io[knd].fd  = -1;
    pp->io[knd].fp  = NULL;
    pp->io[knd].hnd = IO_HND_NONE;
    pp->io[knd].dev = IO_DEV_NONE;
    pp->io[knd].knd = IO_STD_NONE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

sigset_t _block_all_sig(void)
   {sigset_t os;

    memset(&os, 0, sizeof(os));

#if 0
    int rv;
    sigset_t ns;

    sigemptyset(&ns);
    sigfillset(&ns);
    rv = sigprocmask(SIG_BLOCK, &ns, &os);
    ASSERT(rv == 0);
#endif

    return(os);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

sigset_t _unblock_all_sig(void)
   {sigset_t os;

    memset(&os, 0, sizeof(os));

#if 0
    int rv;
    sigset_t ns;

    sigemptyset(&ns);
    sigfillset(&ns);
    rv = sigprocmask(SIG_UNBLOCK, &ns, &os);
    ASSERT(rv == 0);
#endif

    return(os);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_GRP_ATTR - add PP to the process group if G is TRUE
 *               - have PP take the controlling terminal
 *               - iff T is TRUE
 */

void _job_grp_attr(process *pp, int g, int t)
   {int pid, pgid;

    pid  = pp->id;
    pgid = pp->pgid;

    if (g == TRUE)
       setpgid(pid, pgid);

    if (t == TRUE)
       tcsetpgrp(STDIN_FILENO, pgid);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _INIT_IODES - initialize a set of N_IO_CH iodes structures */

static void _init_iodes(int n, iodes *fd)
   {int i;

    for (i = 0; i < n; i++)
        {fd[i].fd   = -1;
	 fd[i].fid  = -1;
	 fd[i].gid  = -1;
	 fd[i].flag = -1;
	 fd[i].knd  = IO_STD_NONE;
	 fd[i].hnd  = IO_HND_NONE;
	 fd[i].dev  = IO_DEV_NONE;
	 fd[i].mode = IO_MODE_NONE;
	 fd[i].file = NULL;
	 fd[i].raw  = NULL;
	 fd[i].fp   = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _DEFAULT_IODES - initialize a set of N_IO_CH iodes structures */

void _default_iodes(iodes *fd)
   {int i;

    for (i = 0; i < N_IO_CHANNELS; i++)
        {if (fd[i].fd == -1)
	    fd[i].fd = i;

	 if (fd[i].dev == IO_DEV_NONE)
	    fd[i].dev = IO_DEV_TERM;};

    if (fd[0].knd == IO_STD_NONE)
       {fd[0].knd  = IO_STD_IN;
	fd[0].mode = IO_MODE_RO;};

    if (fd[1].knd == IO_STD_NONE)
       {fd[1].knd  = IO_STD_OUT;
	fd[1].mode = IO_MODE_WO;};

    if (fd[2].knd == IO_STD_NONE)
       {fd[2].knd  = IO_STD_ERR;
	fd[2].mode = IO_MODE_WO;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _INIT_PROCESS - set process state to defaults */

void _init_process(process *pp)
   {

    _init_iodes(N_IO_CHANNELS, pp->io);

    pp->ip          = -1;
    pp->id          = -1;
    pp->pgid        = -1;
    pp->status      = JOB_NOT_FINISHED;
    pp->reason      = JOB_NOT_FINISHED;
    pp->ischild     = FALSE;
    pp->nattempt    = 1;
    pp->mode[0]     = '\0';
    pp->cmd         = NULL;
    pp->arg         = NULL;
    pp->env         = NULL;
    pp->shell       = NULL;
    pp->start_time  = -1.0;
    pp->stop_time   = -1.0;
    pp->accept      = NULL;
    pp->reject      = NULL;
    pp->wait        = NULL;
    pp->a           = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_MK_PROCESS - initialize and return a process */

static process *_job_mk_process(int child, char **arg,
				char **env, char *shell)
   {char s[MAXLINE];
    char *sc;
    process *pp;

    pp = MAKE(process);
    if (pp != NULL)
       {if (arg == NULL)
	   sc = NULL;
	else
	   {sc = concatenate(s, MAXLINE, arg, " ");
	    sc = STRSAVE(sc);};

	_init_process(pp);

	pp->pgid    = getpgrp();
	pp->ischild = child;
	pp->cmd     = sc;
	pp->arg     = arg;
	pp->env     = env;
	pp->shell   = shell;};

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_RELEASE - close the file descriptors */

static int _job_release(process *pp)
   {int i, rv;

    rv = FALSE;
    if (job_alive(pp))
       {for (i = N_IO_CHANNELS - 1; i >= 0; i--)
	    _job_io_close(pp, i);

        FREE(pp->cmd);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_FREE - free up the job */

static void _job_free(process *pp)
   {

    _job_release(pp);

/* set the process index to -2 to indicate that it has been freed
 * in case there is a second pointer to this space
 * and try to free it just once
 */
    if (pp->ip > -2)
       {pp->ip = -2;
	FREE(pp);}
    else
       printf("Job freed twice\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_EXEC - exec the named program together with args and env
 *           - in the current process
 *           - if successful this routine should not return
 *           - return -1 indicating failure
 */

static int _job_exec(process *cp, char **argv, char **env, char *mode)
   {int i, err, fg, rv;
    int io[N_IO_CHANNELS];

    err = 0;

    if (cp != NULL)

/* put the process into the process group and
 * give the process group the terminal
 */
       {fg = TRUE;
	_job_grp_attr(cp, TRUE, fg);

/* reset the signal handlers for the child */
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGCHLD, SIG_DFL);

/* setup the I/O descriptors */
	for (i = 0; i < N_IO_CHANNELS; i++)
	    io[i] = cp->io[i].fd;

	rv = block_fd(io[0], TRUE);
	ASSERT(rv == 0);

/* set the process stdin, stdout, and stderr to those from the pipe */
	for (i = 0; i < N_IO_CHANNELS; i++)
	    dup2(io[i], i);

/* now that they are copied release the old values */
	for (i = 0; i < N_IO_CHANNELS; i++)
	    _fd_close(io[i]);

/* exec the process with args and environment - this won't return */
	err = execvp(argv[0], argv);};

/* if we get here the exec failed */
    if (err == -1)
       fprintf(stdout, "EXECVP ERROR: '%s' - _JOB_EXEC", argv[0]);
    else
       fprintf(stdout, "EXECVP RETURNED: '%s' - _JOB_EXEC", argv[0]);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_SET_ATTR - set the control flags on a file descriptor
 *               - the following are more or less portable
 *               -
 *               -  NDELAY  non-blocking I/O
 *               -  APPEND  append (writes guaranteed at the end)
 *               -  ASYNC   synchronous write option
 *               -
 *               - the STATE argument specifies how the flag is to
 *               - be set from I
 *               -    1  I added to flag (with |)
 *               -    0  I removed from flag (with & ~)
 *               -   -1  flag set to I
 *               - return TRUE iff successful
 */

static int _job_set_attr(int fd, int i, int state)
   {int rv;
    int arg, status;

    arg = fcntl(fd, F_GETFL);
    if (arg < 0)
       fprintf(stdout, "COULDN'T GET DESCRIPTOR FLAG - _JOB_SET_ATTR");

    switch (state)
       {case 1 :
	     arg |= i;
	     break;
	case 0 :
	     arg &= ~i;
	     break;
	case -1 :
	     arg = i;
	     break;};

    status = fcntl(fd, F_SETFL, arg);
    if (status < 0)
       fprintf(stdout, "COULDN'T SET DESCRIPTOR FLAG - _JOB_SET_ATTR");

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_INIT_IPC - establish two inter-process communications channels
 *               - the input channel should always be unblocked
 *               - return TRUE iff successful
 */

static int _job_init_ipc(process *pp, process *cp)
   {int ret;
    int ports[2];

    ret = TRUE;

/* child stdin */
    if (pipe(ports) < 0)
       {_fd_close(pp->io[0].fd);
	_fd_close(cp->io[1].fd);
	fprintf(stdout, "COULDN'T CREATE PIPE #1 - _JOB_INIT_IPC");
	return(FALSE);};

    cp->io[0].fd = ports[0];
    pp->io[1].fd = ports[1];

/* child stdout */
    if (pipe(ports) < 0)
       {fprintf(stdout, "COULDN'T CREATE PIPE #2 - _JOB_INIT_IPC");
	return(FALSE);};

    cp->io[1].fd = ports[1];
    pp->io[0].fd = ports[0];

/* child stderr */
    if (pipe(ports) < 0)
       {fprintf(stdout, "COULDN'T CREATE PIPE #e - _JOB_INIT_IPC");
	return(FALSE);};

    cp->io[2].fd = ports[1];
    pp->io[2].fd = ports[0];

    _job_set_attr(pp->io[0].fd, O_RDONLY | O_NDELAY, TRUE);
    _job_set_attr(pp->io[1].fd, O_WRONLY, TRUE);
    _job_set_attr(pp->io[2].fd, O_WRONLY, TRUE);

    _job_set_attr(cp->io[0].fd, O_RDONLY & ~O_NDELAY, TRUE);
    _job_set_attr(cp->io[1].fd, O_WRONLY, TRUE);
    _job_set_attr(cp->io[2].fd, O_WRONLY, TRUE);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_CHILD_FORK - the child process comes here and
 *                 - it will never return
 */

static void _job_child_fork(process *pp, process *cp, char **argv, char *mode)
   {int rv;
    extern char **environ;

/* free the parent state which the child does not need */
    _job_free(pp);

/* setup the running child state */
    cp->ischild    = TRUE;
    cp->start_time = wall_clock_time();
    cp->stop_time  = 0.0;

    rv = _job_exec(cp, argv, environ, mode);

    exit(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_PARENT_FORK - the parent process comes here */

static int _job_parent_fork(process *pp, process *cp, char *mode)
   {int i, st;
    char *modes[N_IO_CHANNELS] = { "r", "w", "w" };
   
    st = TRUE;

/* take the controlling terminal */
    _job_grp_attr(pp, isatty(STDIN_FILENO), FALSE);

    pp->id         = cp->id;
    pp->start_time = wall_clock_time();
    pp->stop_time  = 0.0;

    for (i = 0; i < N_IO_CHANNELS; i++)
        {pp->io[i].fp = fdopen(pp->io[i].fd, modes[i]);
	 if (pp->io[i].fp != NULL)
	    setbuf(pp->io[i].fp, NULL);};

    _job_free(cp);

    sched_yield();

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_ERROR_FORK - come here if fork failed
 *                 - just cleanup the remains
 */

static void _job_error_fork(process *pp, process *cp)
   {

/* mark this as having to do with the child so that
 * the managed process list works right
 */
    pp->ischild = TRUE;

    _job_free(pp);
    _job_free(cp);

    fprintf(stdout, "COULDN'T FORK process - _JOB_ERROR_FORK");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_COMMAND_STR - return a single string containing the command
 *                  - specified by the tokens in AL
 */

static char *_job_command_str(char **al)
   {int i;
    char cmd[MAXLINE];
    char *s;

    nstrncpy(cmd, MAXLINE, al[0], -1);

    for (i = 1; al[i] != NULL; i++)
        {nstrcat(cmd, MAXLINE, " ");
	 nstrcat(cmd, MAXLINE, al[i]);};

    s = STRSAVE(cmd);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_SETUP_PROC - allocate process structures for the parent (PP)
 *                 - and the child (CP)
 *                 - do the initial I/O setup and any other
 *                 - preparation prior to the fork
 *                 - return the timeout in milliseconds for the post-fork
 *                 - setup and exec
 */

static int _job_setup_proc(process **ppp, process **pcp, 
			   char **arg, char **env, char *shell)
   {int to, flag;
    process *pp, *cp;

    cp = _job_mk_process(TRUE, arg, env, shell);
    pp = _job_mk_process(FALSE, arg, env, shell);

    if ((cp != NULL) && (pp != NULL))

/* set up the communications pipe */
       {flag = _job_init_ipc(pp, cp);
	if (flag == FALSE)
	   {FREE(pp);
	    FREE(cp);
	    fprintf(stdout, "COULDN'T CREATE IPC CHANNELS - _JOB_SETUP_PROC");
	    return(-1);};

#if 0
/* disable SIGTTOU when running in background */
	if (is_background() & 1)
	   SC_io_suspend(FALSE);
#endif

	pp->status = JOB_RUNNING;
	pp->reason = 0;

	cp->status = JOB_RUNNING;
	cp->reason = 0;

	*ppp = pp;
	*pcp = cp;};

    to = 60000;

    return(to);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_FORK - fork PP/CP and exec the command in CP with AL */

static process *_job_fork(process *pp, process *cp,
			  char **argv, char *mode, void *a)
   {int st, pid;

    if (argv == NULL)
       argv = pp->arg;

    pp->a = a;
    if (pp->cmd == NULL)
       pp->cmd = _job_command_str(argv);
    strcpy(pp->mode, mode);

    _block_all_sig();

/* fork the process */
    pid    = fork();
    pp->id = pid;
    cp->id = pid;

    switch (pid)
        {case -1 :
	      _job_error_fork(pp, cp);
	      pp = NULL;
	      break;

/* the child process comes here and if successful will never return */
	 case 0 :
	      _job_child_fork(pp, cp, argv, mode);
	      break;

/* the parent process comes here */
	 default :
	      st = _job_parent_fork(pp, cp, mode);
	      if (st == FALSE)
		 _job_free(pp);
	      break;};

    _unblock_all_sig();

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _TIMEOUT_ERROR - tell somebody that we timed out and exit */

static void _timeout_error(int sig)
   {

    siglongjmp(_job_cpu, 1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _JOB_TIMEOUT - process will timeout and call FNC in TO seconds if
 *              - a second call to _job_timeout isn't made with
 *              - a value of TO = 0 before then
 */

static void _job_timeout(int to)
   {int ns;
    static PFSIGHand lst = (PFSIGHand) -1;
    static PFSIGHand err = SIG_DFL;

/* since alarms do not nest (without much more logic than we have here)
 * do nothing if there is an active handler going
 */
    if ((lst == (PFSIGHand) -1) || (lst == _timeout_error))

/* set the handler and the alarm */
       {err = signal(SIGALRM, _timeout_error);
	ns  = alarm(to);
	ASSERT(ns == 0);

	if (err == SIG_ERR)
	   printf("Setting SIGALRM failed\n");

/* set lst appropriately */
	if (to == 0)
	   lst = (PFSIGHand) -1;
        else
	   lst = _timeout_error;};

    return;}

/*--------------------------------------------------------------------------*/

/*                                 SINGLE JOB                               */

/*--------------------------------------------------------------------------*/

/* JOB_LAUNCH - start up a process with an I/O connection
 *            - for further communications
 */

process *job_launch(char *cmd, char *mode, void *a)
   {char **argv;
    process *pp, *cp;

    pp = NULL;

    argv = tokenize(cmd, " \t");
    if (argv != NULL)
       {_job_setup_proc(&pp, &cp, argv, NULL, NULL);
	pp = _job_fork(pp, cp, argv, mode, a);

	free_strings(argv);};

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JOB_READ - read from PP and close the pipes if DF is TRUE
 *          - return the number of lines read iff successful
 */

int job_read(int fd, process *pp, int (*out)(int fd, process *pp, char *s))
   {int nl;
    char s[LRG];
    char *p;
    FILE *fi;

    nl = 0;

    if (job_alive(pp))
       {fi = pp->io[0].fp;
	if (fi != NULL)
	   {/* _block_all_sig(); */

	    while (TRUE)
	       {p = fgets(s, MAXLINE, fi);
		if (p == NULL)
		   break;
		nl++;
		if (out != NULL)
		   out(fd, pp, s);

		sched_yield();};

/*	    _unblock_all_sig(); */};};

    return(nl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JOB_WRITE - write to PP and close the pipes if DF is TRUE
 *           - return the number of characters written iff successful
 */

int job_write(process *pp, char *fmt, ...)
   {int nc, ns;
    char s[LRG];
    FILE *fo;

    VA_START(fmt);
    VSNPRINTF(s, LRG, fmt);
    VA_END;

    if (LAST_CHAR(s) != '\n')
       nstrcat(s, LRG, "\n");

    nc = 0;

    if (job_alive(pp))
       {fo = pp->io[1].fp;
	if (fo != NULL)
	   {_block_all_sig();

	    ns = strlen(s);
	    nc = fwrite_safe(s, 1, ns, fo);

	    _unblock_all_sig();};};

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JOB_POLL - poll the job PP
 *          - return TRUE iff input is ready
 *          - TO is the number of milliseconds to wait for input
 *          - TO = -1 means block
 */

int job_poll(process *pp, int to)
   {int n, ok;
    apollfd fds;

    n = 0;
    if (job_running(pp))
       {fds.fd      = pp->io[0].fd;
	fds.events  = stck.mask_acc;
	fds.revents = 0;
	n = 1;};

/* now poll the active descriptors */
    ok = poll(&fds, n, to);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JOB_RESPONSE - send text to PP and get a response
 *              - wait only TO milliseconds (-1 waits forever)
 *              - return the number of lines of response iff successful
 *              - return -1 if we fail to send the message
 *              - return -2 if we timeout waiting for the response
 */

int job_response(process *pp, int to, char *fmt, ...)
   {volatile int i, nl, nc, ns, ok, dt, fd;
    char s[LRG], t[LRG];
    char *p;
    FILE *fi, *fo;
    int (*rsp)(int fd, process *pp, char *s);

    VA_START(fmt);

    if (fmt != NULL)
       {VSNPRINTF(s, LRG, fmt);
	if (LAST_CHAR(s) != '\n')
	   nstrcat(s, LRG, "\n");}
    else
       s[0] = '\0';

    VA_END;

    nl  = 0;
    rsp = pp->accept;

    if (job_alive(pp))
       {fi = pp->io[0].fp;
	fd = pp->io[0].fd;
	fo = pp->io[1].fp;
	ns = strlen(s);
	nc = -1;

	ok = TRUE;
	if ((fo != NULL) && (IS_NULL(s) == FALSE))
	   {nc = fwrite_safe(s, 1, ns, fo);
	    ok = (ns == nc);};

	if (ok == FALSE)
	   nl = -1;

	else if (fi != NULL)
	   {ok = FALSE;
	    dt = (to < 0) ? -1 : to*1.0e-3;
	    if (sigsetjmp(_job_cpu, TRUE) == 0)
	       {if (dt > 0)
		   _job_timeout(dt);

		for (i = 0; ok == FALSE; i++)
		    {if (job_poll(pp, to) == TRUE)
		        {p = fgets(t, LRG, fi);
			 if (p == NULL)
			    {ok = TRUE;
			     break;};
			 nl++;
			 if (rsp != NULL)
			    ok = rsp(fd, pp, t);};};

		if (dt > 0)
		   _job_timeout(0);}

/* if we time out treat it as a reject message */
	    else if (pp->reject != NULL)
	       pp->reject(-1, pp, t);

	    if (ok == FALSE)
	       nl = -2;};};

    return(nl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JOB_WAIT - check to update the process status */

void job_wait(process *pp)
   {int i, st, w, pid, sts, cnd;

    if (pp != NULL)
       {pid = pp->id;

	_block_all_sig();

	st  = waitpid(pid, &w, WNOHANG);
	if (st == 0)
	   pp->status = JOB_RUNNING;

	else if (st == pid)
	   {if (WIFEXITED(w))
	       {cnd = JOB_EXITED;
		sts = WEXITSTATUS(w);}

	    else if (WIFSIGNALED(w))
	       {cnd = JOB_SIGNALED;
		sts = WTERMSIG(w);}

	    else if (WIFSTOPPED(w))
	       {cnd = JOB_STOPPED;
		sts = WSTOPSIG(w);}

	    else
	       {cnd = -1;
		sts = -1;};

	    pp->stop_time = wall_clock_time();
	    pp->status    = cnd;
	    pp->reason    = sts;

	    if (pp->wait != NULL)
	       {pp->wait(pp);

		for (i = 0; i < N_IO_CHANNELS; i++)
		    _job_io_close(pp, i);};}

	else if ((st < 0) && (pp->status == JOB_RUNNING))
	   pp->status = JOB_DEAD;

	_unblock_all_sig();};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JOB_SIGNAL - send signal SIG to the process associated with PP
 *            - return TRUE iff successful
 *            - return FALSE otherwise
 */

int job_signal(process *pp, int sig)
   {int rv, st, pid;

    rv = FALSE;

    if ((job_running(pp) == TRUE) && (sig > 0))
       {pid = pp->id;
	st  = kill(pid, sig);
        if (st == 0)
	   {rv = TRUE;
	    pp->status = JOB_SIGNALED;
	    pp->reason = sig;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JOB_DONE - when a job is done do the following:
 *          -   kill if (send SIG if > 0)
 *          -   waitpid
 *          -   drain
 *          - return TRUE iff successful
 */

int job_done(process *pp, int sig)
   {int rv;

    rv = job_signal(pp, sig);

    job_wait(pp);

    rv = job_read(-1, pp, pp->accept);

    _job_release(pp);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            ASYNCHRONOUS JOBS                             */

/*--------------------------------------------------------------------------*/

/* ASETUP - setup state to run and monitor N jobs */

void asetup(int n, int na)
   {

/* per process members */
    if (stck.proc != NULL)
       FREE(stck.proc);

    stck.ip   = 0;
    stck.np   = n;
    stck.proc = MAKE_N(process *, n);

/* per descriptor members */
    if (stck.fd != NULL)
       FREE(stck.fd);

    stck.ifd  = 0;
    stck.nfd  = N_IO_CHANNELS*n;
    stck.fd   = MAKE_N(apollfd, N_IO_CHANNELS*n);
    stck.map  = MAKE_N(int, N_IO_CHANNELS*n);
    stck.io   = MAKE_N(int, N_IO_CHANNELS*n);

/* everything else */
    stck.nattempt = na;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _AWATCH_FD - register the KND file descriptor of PP as the SIPth element */

static int _awatch_fd(process *pp, io_kind knd, int sip)
   {int ip, ifd, fd, rv;

/* handle the process */
    ip = (sip < 0) ? stck.ip++ : sip;
    if (ip >= stck.np)
       {int i, n;

	printf("ERROR: Bad process accounting (%d > %d) - exiting\n",
	       stck.ip, stck.np);

	n = stck.np;
	for (i = 0; i < n; i++)
	    printf("%d ", stck.proc[i]->ip);
	printf("\n");
	exit(1);};

    stck.proc[ip] = pp;

/* handle the descriptor */
    fd = pp->io[knd].fd;
    rv = block_fd(fd, FALSE);
    ASSERT(rv == 0);

    ifd = stck.ifd++;
    stck.fd[ifd].fd      = 0;
    stck.fd[ifd].events  = 0;
    stck.fd[ifd].revents = 0;

    stck.io[ifd]  = fd;
    stck.map[ifd] = ip;

    return(ip);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _AFINISH - finish the state of the launched jobs */

static void _afinish(void)
   {int i, np;

    np = stck.np;

    if (stck.proc != NULL)
       {for (i = 0; i < np; i++)
	    _job_free(stck.proc[i]);
	FREE(stck.proc);};

    FREE(stck.fd);
    FREE(stck.map);
    FREE(stck.io);

    stck.ip  = 0;
    stck.np  = 0;
    stck.ifd = 0;
    stck.nfd = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ALAUNCH - launch the job specified in CMD and
 *         - place it in the list of file descriptors to watch
 *         - if SIP < 0 add it to the end
 *         - return the FILE pointer returned by popen
 */

process *alaunch(int sip, char *cmd, char *mode, void *a,
		 int (*acc)(int fd, process *pp, char *s),
		 int (*rej)(int fd, process *pp, char *s),
		 void (*wt)(process *pp))
   {int ip;
    process *pp;

    pp = job_launch(cmd, mode, a);
    if (pp != NULL)
       {pp->accept   = acc;
	pp->reject   = rej;
	pp->wait     = wt;
	pp->nattempt = 1;

	ip = _awatch_fd(pp, IO_STD_IN, sip);

	pp->ip = ip;};

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ARELAUNCH - relaunch the job PP */

process *arelaunch(process *pp)
   {process *npp;

    if ((pp != NULL) && (pp->nattempt <= stck.nattempt))
       {npp = alaunch(pp->ip, pp->cmd, pp->mode, pp->a,
		      pp->accept, pp->reject, pp->wait);

	if (npp != NULL)
	   {npp->nattempt = pp->nattempt + 1;

	    _job_free(pp);

	    pp = npp;};};

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* APOLL - poll all the active jobs
 *       - TO is the number of milliseconds to wait for input
 *       - TO = -1 means block
 */

int apoll(int to)
   {int i, j, n, ifd, nfd, fd, np, nrdy;
    int *map, *io;
    short rev;
    process *pp;
    apollfd *fds;

    np  = stck.np;
    nfd = stck.ifd;
    fds = stck.fd;
    map = stck.map;
    io  = stck.io;

/* find the descriptors of running jobs only */
    for (n = 0, i = 0; i < np; i++)
        {pp = stck.proc[i];
	 if (job_running(pp))
	    {for (ifd = 0; ifd < nfd; ifd++)
	         {if (map[ifd] == i)
		     {fds[n].fd      = io[ifd];
		      fds[n].events  = stck.mask_acc;
		      fds[n].revents = 0;
		      n++;};};};};

/* now poll the active descriptors */
    nrdy = poll(fds, n, to);

    if (nrdy > 0)
       {for (i = 0; i < n; i++)
            {rev = fds[i].revents;
	     fd  = fds[i].fd;

/* find the process containing the file desciptor FD and drain it */
	     for (j = 0; j < np; j++)
	         {pp = stck.proc[j];
		  if (job_alive(pp) == TRUE)
		     {for (ifd = 0; ifd < nfd; ifd++)
			  {if ((map[ifd] == j) && (io[ifd] == fd))
			      {if (rev & stck.mask_acc)
				  {nrdy--;
				   job_read(fd, pp, pp->accept);}

			       else if (rev & stck.mask_rej)
				  {nrdy--;
				   job_read(fd, pp, pp->reject);};};};};};};};

    return(nrdy);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ACHECK - check the asynchronous jobs of the stack for run status
 *        - return the number of jobs which are done
 */

int acheck(void)
   {int ip, np, nf;
    process *pp;

    np = stck.np;

    for (nf = 0, ip = 0; ip < np; ip++)
        {pp = stck.proc[ip];
	 if (job_alive(pp) == TRUE)
	    {job_wait(pp);

/* restart job if indicated */
	     if (pp->status & JOB_RESTART)
	        pp = arelaunch(pp);};

	 nf += (job_running(pp) == FALSE);};

    return(nf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* AWAIT - wait TF seconds for the jobs in the process stack
 *       - or until they are all done,
 *       - or time is exhausted, or we get an interrupt from the terminal
 *       - return the time taken in seconds
 *       - check TTY and call F each cycle
 *       - poll for DT milliseconds in each cycle
 */

int await(unsigned int tf, int dt, char *tag,
	  int (*tty)(char *tag),
	  void (*f)(int i, char *tag, void *a, int nd, int np, int tc, int tf),
	  void *a)
   {int i, nd, np, st, ti, tc;

    np = stck.np;
    nd = -1;
    ti = wall_clock_time();
    tc = 0;
    st = FALSE;
    for (i = 0; (nd < np) && (tc <= tf) && (st == FALSE); i++)
        {if (tty == NULL)
	    st = FALSE;
	 else
	    st = tty(tag);

	 tc = wall_clock_time() - ti;
	 nd = acheck();
	 apoll(dt);

	 if (f != NULL)
	    f(i, tag, a, nd, np, tc, tf);};

    return(tc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* AMAP - map the function F over the process stack */

void amap(void (*f)(process *pp, void *a))
   {int i, np;
    process *pp;
    void *a;

    np = stck.np;

    for (i = 0; i < np; i++)
        {pp = stck.proc[i];
	 if (pp != NULL)
	    {a  = pp->a;

	     if (f != NULL)
	        f(pp, a);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* AFIN - finish out the jobs on the process stack */

void afin(void (*f)(process *pp, void *a))
   {int nd;

/* check the job status one last time */
    nd = acheck();
    ASSERT(nd >= 0);

    amap(f);

    _afinish();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
