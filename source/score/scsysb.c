/*
 * SCSYSB.C - SCORE system comand equivalent
 *          - command server
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_proc.h"

/* server activities */

#define SERVER_CORE       201
#define IN_REJECT         202
#define IN_ACCEPT         203
#define SERVER_JOB        204
#define SERVER_COMMAND    205
#define SERVER_ENV        206
#define SERVER_HEARTBEAT  207
#define SERVER_EXIT       208
#define SERVER_CHECK      209
#define JOB_COMPLETE      210
#define SERVER_LOG        211

#define GET_TASKS(tsk, n, state)                                              \
    n   = SC_array_get_n(state->tasks);                                       \
    tsk = SC_array_array(state->tasks);                                       \
    SC_mark(tsk, 1);

#define REL_TASKS(tsk)                                                        \
    SFREE(tsk)

asyncstate
 _SC_server_state;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_SHOW_LOG - print the server's state log to stdout
 *                     - this is a server side routine
 */

static void _SC_server_show_log(parstate *state, FILE *fp)
   {int i, n;
    char **log, *home, *dir;

    SC_START_ACTIVITY(state, SERVER_LOG);

    state->is_stdout = (fp == stdout);

    log = SC_array_array(state->log);
    n   = SC_array_get_n(state->log);

    if (fp == NULL)
       {home = getenv("HOME");
	dir  = SC_dsnprintf(TRUE, "%s/dmake-srv.%d", home, (int) getpid());
	fp   = io_open(dir, "w");
	SFREE(dir);

	io_printf(fp, "----- server log -----------------------\n");

	for (i = 0; i < n; i++)
	    {io_printf(fp, "%s", log[i]);};

	io_printf(fp, "----- server log -----------------------\n");

	io_close(fp);}

    else
       {io_printf(fp, "%s ----- server log (%d) -----------------------\n",
		  _SC_EXEC_SRV_ID, n);

	for (i = 0; i < n; i++)
	    {io_printf(fp, "%s %s", _SC_EXEC_SRV_ID, log[i]);};

        io_printf(fp, "%s ----- server log -----------------------\n",
		  _SC_EXEC_SRV_ID);};

    state->is_stdout = FALSE;

    SFREE(log);

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SIGNAL_SERVER - process signals for the exec server
 *                   - this is a server side routine
 */

static void _SC_signal_server(int sig)
   {parstate *state;
    asyncstate *as;

/* if we have a SIGPIPE we cannot communicate - just exit with SC_EXIT_IO */
    if (sig == SIGPIPE)
       exit(SC_EXIT_IO);

    as    = &_SC_server_state;
    state = as->server;

    SC_block_file(stdin);

    if (as->active == TRUE)
       {switch (sig)
	   {case SIGINT :
	         LONGJMP(as->cpu, sig);

	    case SIGUSR1 :
	         _SC_server_show_log(state, stdout);
		 break;

	    case SIGTERM :
	         _SC_server_show_log(state, stdout);
	         LONGJMP(as->cpu, sig);

	    default :
	         LONGJMP(_SC.srv_rstrt, sig);};}
    else
       LONGJMP(_SC.srv_rstrt, sig);

    SC_unblock_file(stdin);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_HANDLER - handle signals for the exec server gracefully
 *                    - this is a server side routine
 */

static void _SC_server_handler(int sig)
   {PFSignal_handler si;
    asyncstate *as;
    parstate *state;

    as    = &_SC_server_state;
    state = as->server;

    SC_setup_sig_handlers(_SC_server_handler, FALSE);
    si = SC_signal(SC_SIGIO, SIG_IGN);

    _SC_exec_printf(as,
		    "EXEC SERVER: signal %s (%d) while doing %d\n",
		    SC_signal_name(sig), sig, state->doing);

/* attach or get retrace if it is sensible to do so */
    if ((sig != SIGINT) && (sig != SIGTERM) &&
	(sig != SIGPIPE) && (sig != SIGUSR1))

/* attach the debugger to this rascal if requested */
       {if (as->debug == TRUE)
	   SC_attach_dbg(-1);

/* do a call stack trace if it has died */
        else
	   SC_retrace_exe(NULL, -1, 120000);};

    _SC_signal_server(sig);

    SC_setup_sig_handlers(_SC_server_handler, TRUE);

    SC_signal(SC_SIGIO, si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_COMPLETE - do the work that needs to be done when
 *                     - a job is finished
 *                     - return TRUE iff the job is to be retried
 *                     - this is a server side routine
 */

static int _SC_server_complete(taskdesc *job, char *msg)
   {int rtry, pid, sgn, sts;
    char tag[MAXLINE];
    char *stm;
    double dt;
    asyncstate *as;
    parstate *state;
    jobinfo *inf;

    rtry = FALSE;

    if (job != NULL)
       {state = job->context;

	SC_START_ACTIVITY(state, JOB_COMPLETE);

	as   = &_SC_server_state;
	rtry = job->finish(job, as, state->server);
	if (rtry == FALSE)
	   {inf = &job->inf;
	    sts = inf->status;
	    sgn = inf->signal;
	    stm = inf->stop_time;
	    dt  = SC_wall_clock_time() - inf->tstart;
	
	    job->tag(job, tag, MAXLINE, stm);
	    state->print(state, "%s exit (%s) %d %d %.3f\n",
			 tag, msg, sgn, sts, dt);

	    pid = (state->tagio == TRUE) ? inf->id : -1;
	    if (pid != -1)
	       _SC_exec_printf(as, "[Job %2d %s]: %s %d %d %.3f\n",
			       pid, stm+11, msg, sgn, sts, dt);

	    job->remove(job);};

	SC_END_ACTIVITY(state);};

    return(rtry);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_CHECK_JOBS - return TRUE iff all jobs are complete
 *                       - return the overall status via PST
 *                       - return the number of running jobs via PNR
 *                       - return the number of complete jobs via PNC
 */

static int _SC_server_check_jobs(int *pst, int *pnr, int *pnc,
				 parstate *state)
   {int i, n, more;
    int rst, lex, ex, nr, nc;
    taskdesc **tsk, *job;
    jobinfo *inf;
    asyncstate *as;

    SC_START_ACTIVITY(state, SERVER_CHECK);

    as = &_SC_server_state;

    GET_TASKS(tsk, n, state);

    rst = 0;
    lex = 0;
    nr  = 0;
    nc  = 0;

    for (i = 0; i < n; i++)
        {job = tsk[i];

/* if there is no job it was finished prior to coming here */
	 if (job == NULL)
	    lex++;

	 else
	    {inf = &job->inf;

/* if finished accumulate the return statuses */
	     if (job->finished == TRUE)
	        {rst += abs(inf->status);
		 nc++;
		 lex++;}

/* if is has a process check its status */
	     else if (SC_process_alive(job->pp))
	        job->check(job, as, &nr, &nc);

/* otherwise get on with the next task in the job */
	     else
	        {more = job->exec(job, FALSE);
		 if (more == FALSE)
		    {rst += abs(inf->status);
		     nc++;
		     lex++;}
		 else
		    nr++;};};};

    REL_TASKS(tsk);

    if (pst != NULL)
       *pst = rst;
    if (pnr != NULL)
       *pnr = nr;
    if (pnc != NULL)
       *pnc = nc;

/* determine whether all jobs are finished */
    ex = (lex == i);
    if (state->done == FALSE)
       ex &= (n != 0);

#if 0
    if (ex == TRUE)
       _SC_server_printf(as, state, _SC_EXEC_INFO,
			 "all jobs done %d %d %d\n", nr, nc, rst);
#endif

    SC_END_ACTIVITY(state);

    return(ex);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_EXIT - if the server has been flagged as done
 *                 - check to make sure all running jobs are done 
 *                 - and exit only then
 */

static int _SC_server_exit(parstate *state, int fl)
   {int ex, nr, nc;
    asyncstate *as;

    SC_START_ACTIVITY(state, SERVER_EXIT);

    as = &_SC_server_state;
    if (fl == -1)
       ex = _SC_server_check_jobs(NULL, &nr, &nc, state);
    else
       ex = fl;

    if ((ex == TRUE) && (state->done == TRUE))
       {SC_setup_sig_handlers(SIG_DFL, TRUE);
	_SC_server_printf(as, state, _SC_EXEC_EXIT, "\n");
	_SC_signal_server(SIGINT);

	exit(SC_EXIT_SELF);};

    SC_END_ACTIVITY(state);

    return(ex);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SEND_HEARTBEAT - send the following information:
 *                    -    load average
 *                    -    fraction of memory available
 *                    -    # running according to the state
 *                    -    # complete according to the state
 *                    -    # running according to the processes (NR)
 *                    -    # complete according to the processes (NC)
 *                    - as a heartbeat message to the controlling parent
 *                    - this is a server side routine
 */

static void _SC_send_heartbeat(parstate *state, int nr, int nc)
   {double av[3], mem[2];
    double r;
    extern void _SC_kill_runaways(void);
    asyncstate *as;

    as = &_SC_server_state;

    if (state->done == FALSE)
       {_SC_kill_runaways();

	SC_load_ave(av);
	SC_free_mem(mem);

	r = mem[1]/mem[0];

	_SC_server_printf(as, state, _SC_EXEC_HEARTBEAT, 
			  "%.2f %9.3e %d %d %d %d\n",
			  av[0], r, state->n_running, state->n_complete,
			  nr, nc);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_HEARTBEAT - return TRUE iff all of the process have exited
 *                      - return the exit status in RV
 *                      - if processes are still at work send
 *                      -    load average
 *                      -    memory available
 *                      - as a heartbeat message
 *                      - this is a server side routine
 */

int _SC_server_heartbeat(int *prv, void *a)
   {int ex, rst;
    int nc, nr, ppid;
    int dth, dtw, thl, twl, tcur;
    parstate *state;
    asyncstate *as;

    as = &_SC_server_state;

/* if the parent PID changes - for example being orphaned
 * then we are done
 */
    ppid = getppid();
    if ((ppid < 2) || (ppid != as->ppid))
       LONGJMP(_SC.srv_rstrt, SIGSTOP);

/* do the main business */
    state = (parstate *) a;

    SC_START_ACTIVITY(state, SERVER_HEARTBEAT);

/* check the time */
    tcur = SC_wall_clock_time();
    if (_SC.elapsed == -1)
       {_SC.elapsed = tcur;
	dth     = 0;}
    else
       dth = tcur - _SC.elapsed;

    thl = state->heartbeat_dt;
    twl = state->wait_dt;

/* determine whether all jobs are finished */
    ex  = _SC_server_check_jobs(&rst, &nr, &nc, state);
    ex  = _SC_server_exit(state, ex);
    ex &= (state->server == FALSE);

/* if we have running jobs reset the timer */
    if (nr > 0)
       state->wait_ref = tcur;

    dtw = tcur - state->wait_ref;

/* if the wait time has passed then quit */
    if (dtw > twl)
       {_SC_server_printf(as, state, _SC_EXEC_INFO,
			  "quitting - no jobs and no messages for %d seconds\n",
			  twl);
	LONGJMP(_SC.srv_rstrt, SIGSTOP);}

/* if all are finished return the exit status */
    else if (ex == TRUE)
       *prv = rst;

/* send the heartbeat if it has been THL seconds */
    else if (dth > thl)
       {_SC.elapsed += dth;
	_SC_send_heartbeat(state, nr, nc);};

    SC_END_ACTIVITY(state);

    return(ex);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_ENV - process an environment variable specification
 *                - return TRUE iff T contains an environment variable spec
 *                - this is a server side routine
 */

static int _SC_server_env(parstate *state, char *t)
   {int rv, st;
    char *p, *var, *val, *evl, *nvl, *ex;
    asyncstate *as;

    SC_START_ACTIVITY(state, SERVER_ENV);

    as = &_SC_server_state;

    if (_SC.slog_env == TRUE)
       state->print(state, "<env> rcv %s", t);

    rv = (strchr(t, '=') != NULL);
    if (rv == TRUE)
       {ex = SC_strsavef(t, "_SC_SERVER_ENV:ex");
	SC_LAST_CHAR(ex) = '\0';

	nvl = SC_strsavef(ex, "_SC_SERVER_ENV:nvl");
	    
	var = SC_strtok(ex, " =", p);
	val = SC_strtok(NULL, "\n", p);
	evl = (var != NULL) ? getenv(var) : NULL;
	if ((evl == NULL) ||
	    (strcmp(var, "PATH") == 0) ||
	    (strcmp(var, "SHELL") == 0))
	   {st = SC_putenv(nvl);
	    _SC_server_printf(as, state, _SC_EXEC_ENV,
			      "%s (%d)\n", nvl, st);};

	SFREE(ex);
	SFREE(nvl);};

    SC_END_ACTIVITY(state);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_COMMAND - process an asynchronous command for the server
 *                    - this is a server side routine
 */

static void _SC_server_command(parstate *state, char *t)
   {int i, n, jid, rtry;
    char *p, *r;
    taskdesc **tsk, *job;
    jobinfo *inf;
    asyncstate *as;

    SC_START_ACTIVITY(state, SERVER_COMMAND);

    as = &_SC_server_state;

    GET_TASKS(tsk, n, state);

    p = NULL;

/* handle a kill job request */
    if (SC_EXEC_MSG_MATCH(r, t, _SC_EXEC_KILL))
       {t   = SC_strtok(r, " ", p);
        jid = SC_stoi(t);
	for (i = 0; i < n; i++)
	    {job = tsk[i];
	     if (job != NULL)
	        {inf = &job->inf;
		 if (inf->id == jid)
		    {if (state->finish != NULL)
		        rtry = state->finish(job, _SC_EXEC_KILLED);
		     break;};};};}

/* handle a show log request */
    else if (SC_EXEC_MSG_MATCH(r, t, _SC_EXEC_LOG))
       {_SC_server_show_log(state, stdout);
	_SC_exec_printf(as,
			"%s %s\n", _SC_EXEC_SRV_ID, _SC_EXEC_LOG);}

/* handle an exit request */
    else if (SC_EXEC_MSG_MATCH(r, t, _SC_EXEC_EXIT))
       {state->done = TRUE;
	_SC_server_exit(state, -1);}

/* handle a heartbeat notification - really a no-op */
    else if (SC_EXEC_MSG_MATCH(r, t, _SC_EXEC_HEARTBEAT))
       {};

    REL_TASKS(tsk);

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_JOB - launch/manage an asynchronous job for the server
 *                - this is a server side routine
 */

static void _SC_server_job(parstate *state, char *t)
   {int more, jid;
    char tag[MAXLINE];
    char *shell, *dir, *cmd, *p;
    taskdesc *job;
    jobinfo *inf;
    asyncstate *as;

    SC_START_ACTIVITY(state, SERVER_JOB);

    as = &_SC_server_state;
    p  = NULL;

    t = SC_strtok(t, ":", p);
    if (t == NULL)
       return;

    jid   = SC_stoi(t);
    shell = SC_strtok(NULL, ":", p);
    dir   = SC_strtok(NULL, ":", p);
    cmd   = SC_strtok(NULL, "\n", p);

    if (cmd != NULL)
       {if (shell == NULL)
	   shell = state->shell;

/* send acknowledgement */
	_SC_exec_printf(as,
			"[Job %2d]: %s %s -c 'cd %s ; %s'\n",
			jid, _SC_EXEC_JOB, shell, dir, cmd);

/* launch command */
	job = SC_make_taskdesc(state, jid, NULL, shell, dir, cmd);
	if (job != NULL)
	   {job->start(job, as, FALSE);
	    inf = &job->inf;

	    job->tag(job, tag, MAXLINE, NULL);
	    state->print(state, "%s command %s -c 'cd %s ; %s'\n",
			 tag, shell, dir, cmd);

	    job->add(job);

	    _SC_chg_dir(dir, &inf->directory);

	    more = job->exec(job, FALSE);};};

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_INPUT - get new job from stdin
 *                  - launch it and add it to the list
 *                  - of tasks
 *                  - a job is specified as:
 *                  -     "<id>:<shell>:<dir>:<cmnd>"
 *                  - where
 *                  -      <id>    a unique job id number
 *                  -              used to tag output for each job
 *                  -      <shell> the shell under which to run the command
 *                  -      <dir>   a directory in which <cmnd> is to be run
 *                  -      <cmnd>  the command to run
 *                  - this is a server side routine
 */

static void _SC_server_input(int fd, int mask, void *a)
   {int nce, ncj;
    char s[MAX_BFSZ+1];
    char *t;
    parstate *state;
    static int count = 0;

    state = (parstate *) a;

    SC_START_ACTIVITY(state, IN_ACCEPT);

    nce = strlen(_SC_EXEC_ENV);
    ncj = strlen(_SC_EXEC_JOB);

    while (TRUE)
       {t = io_gets(s, MAX_BFSZ, stdin);
	if (t != NULL)

/* if we receive input reset the timer */
	   {state->wait_ref = SC_wall_clock_time();
	    count = 0;

	    if (strncmp(t, _SC_EXEC_ENV, nce) == 0)
	       _SC_server_env(state, t + nce + 1);

	    else if (strncmp(t, _SC_EXEC_JOB, ncj) == 0)
	       _SC_server_job(state, t + ncj + 1);

	    else
	       _SC_server_command(state, t);}

        else
           break;};

    count++;

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SERVER_IN_REJECT - handle input matching the reject mask
 *                      - this is a server side routine
 */

static void _SC_server_in_reject(int fd, int mask, void *a)
   {int i, n, nzp;
    int err, hup, nvl;
    parstate *state;
    asyncstate *as;
    PROCESS *pp;
    taskdesc *job, **tsk;

    as    = &_SC_server_state;
    state = (parstate *) a;

    SC_START_ACTIVITY(state, IN_REJECT);

    GET_TASKS(tsk, n, state);

    for (i = 0; i < n; i++)
        {job = tsk[i];
	 if (job != NULL)
	    {pp = job->pp;
	     if (SC_process_alive(pp))
	        {if (pp->in == fd)
		    {err = ((mask & POLLERR)  != 0);
		     hup = ((mask & POLLHUP)  != 0);
		     nvl = ((mask & POLLNVAL) != 0);

		     _SC_exec_printf(as,
				     "_SC_SERVER_IN_REJECT: error\n");

		     nzp = job->nzip++;
		     if (nzp > 16)
		        {LONGJMP(_SC.srv_rstrt, SIGSTOP);};};};};};

    REL_TASKS(tsk);

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_SRV_CORE - do the core work of SC_exec_server */

static int _SC_exec_srv_core(char *shell, char *fname, int na,
			     int show, int ignore, int debug,
			     parstate *state, fspec *filter)
   {int np, pi, dt, sig, st, pid;
    SC_evlpdes *pe;
    asyncstate *as;

    SC_START_ACTIVITY(state, SERVER_CORE);

    as = &_SC_server_state;

/* re/set the signal handlers */
    SC_setup_sig_handlers(_SC_server_handler, TRUE);

    _SC_setup_async_state(as,
			  SC_get_sys_length_max(TRUE, FALSE));

    as->active = FALSE;
    as->debug  = debug;

/* create the event loop state */
    SC_io_interrupts_on = FALSE;

    shell = SC_get_shell(shell);
    pe    = SC_make_event_loop(NULL, NULL,
			       _SC_server_heartbeat, -1, -1, -1);

    _SC_exec_setup_state(state, shell, NULL,
			 na, show, TRUE, TRUE,
			 ignore, filter, pe,
			 _SC_process_output, _SC_process_out_reject,
			 _SC_server_complete);

    np  = SC_get_ncpu();
    pid = getpid();

    state->print(state, "<server> init:\n");
    state->print(state, "<server> PID    = %d\n", pid);
    state->print(state, "<server> # cpus = %d\n", np);
    state->print(state, "<server> shell  = %s\n", shell);
    if (fname == NULL)
       state->print(state, "<server> filter = none\n");
    else
       state->print(state, "<server> filter = %s\n", fname);

    _SC_exec_printf(as,
		    "%s %s %d\n",
		    _SC_EXEC_SRV_ID, _SC_EXEC_NCPU, np);

    pi = SC_register_event_loop_callback(pe, SC_FILE_I, stdin,
					 _SC_server_input,
					 _SC_server_in_reject,
					 -1);

    SC_unblock_file(stdin);

    as->log    = NULL;
    as->server = state;
    sig = SETJMP(as->cpu);
    if (sig == 0)
       {as->active = TRUE;

/* send an initial heartbeat */
	_SC_send_heartbeat(state, 0, 0);

/* wait for commands to complete
 * give a heartbeat every HEARTBEAT_DT seconds
 * NOTE: SC_event_loop takes time in milliseconds
 */
	dt = 1000*state->heartbeat_dt;
	dt = DEFAULT_WAIT;
	st = SC_event_loop(pe, state, dt);

	SC_block_file(stdin);

	state->print(state, "<server> fin(%d)\n", st);}

    else
       st = sig;

    as->active = FALSE;

/* cleanup */
    _SC_exec_free_state(state, TRUE);

    SC_END_ACTIVITY(state);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXEC_SERVER - execute each command in from stdin asynchronously
 *                - and with persistence on the local host
 *                - a NULL command causes no further commands to be accepted
 *                - use SHELL for commands requiring a shell
 *                - filtering the output using the filter file FNAME
 *                - SHOW governs how commands are displayed:
 *                -    0 do not echo command
 *                -    1 echo commands not begining with '@'
 *                -    2 echo all commands
 *                - IGNORE controls error behavior
 *                -    0 do not execute more commands after one fails
 *                -    1 execute all commands
 *                - the commands are attempted NA times with a random
 *                - time upto 30 seconds between attempts
 *                - if a command fails after NA attempts do not process
 *                - any remaining commands
 *                - if DEBUG is TRUE attach a debugger to the server if
 *                - a trapped signal is received
 *                - return TRUE iff all commands succeed
 */

int SC_exec_server(char *shell, char *fname, int na, int show, int ignore,
		   int debug)
   {int ia, st, rst;
    fspec *filter;
    parstate state;
    asyncstate *as;

    filter = NULL;
    st     = -1;
    as     = &_SC_server_state;

    as->ppid = getppid();

/* start/restart from here
 * on each restart do a fresh setjmp
 */
    for (ia = 0; ia < na; ia++)

/* set the signal handlers each time */
        {SC_setup_sig_handlers(_SC_server_handler, TRUE);

	 rst = SETJMP(_SC.srv_rstrt);
	 if (rst == 0)

/* do this here so that the error handler is in play */
            {if (ia == 0)
	        {memset(&state, 0, sizeof(parstate));
		 filter = _SC_read_filter(fname);};

	     st = _SC_exec_srv_core(shell, fname, na, show, ignore, debug,
				    &state, filter);
	     break;}

/* inform client of restart */
	 else
	    {if (ia < na-1)
	        _SC_exec_printf(as,
				"%s %s number %d after %s\n",
				_SC_EXEC_SRV_ID, _SC_EXEC_RESTART,
				ia+1, SC_signal_name(rst));
	     else
	        _SC_exec_printf(as,
				"%s %s giving up on attempt %d after %s\n",
				_SC_EXEC_SRV_ID, _SC_EXEC_RESTART,
				ia+1, SC_signal_name(rst));
	     _SC_exec_free_state(&state, FALSE);

	     if (rst == SIGSTOP)
	        {st = SC_EXIT_SELF;
		 break;};};};

    if (ia >= na)
       st = SC_EXIT_SELF;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_PUTS - do printing for SC_EXEC family of functions
 *               - print possibly tagged message to stdout
 *               - and to the AS log if it exists
 */

static int _SC_exec_puts(asyncstate *as, char *msg)
   {int ns, rv;

/* if we end up doing nothing the return value should be TRUE */
    ns = 1;

    if (as == NULL)
       ns = io_puts(msg, stdout);

    else
       {if (as->to_stdout == TRUE)
	   {ns = io_puts(msg, stdout);
	    if (as->server != NULL)
	       as->server->is_stdout = FALSE;};

	if (as->log != NULL)
	   ns = io_puts(msg, as->log);};

    rv = (ns >= 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_PRINTF - do printing for asyncstate instances
 *                 - this would be called exclusively as
 *                 -   as->print
 *                 - were it not for the case in which as is NULL
 */

int _SC_exec_printf(asyncstate *as, char *fmt, ...)
   {int rv;
    char *msg;

    SC_VDSNPRINTF(TRUE, msg, fmt);

    if (as == NULL)
       rv = io_puts(msg, stdout);
    else
       rv = _SC_exec_puts(as, msg);

    SFREE(msg);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SETUP_ASYNC_STATE - initialize an asyncstate AS */

void _SC_setup_async_state(asyncstate *as, int ln)
   {

    as->pool      = NULL;
    as->to_stdout = TRUE;
    as->nchar_max = ln;

    as->print     = _SC_exec_printf;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
