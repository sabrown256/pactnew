/*
 * SCSYSA.C - SCORE system comand equivalent
 *          - and support routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"

/* exec client activities */

#define EXEC_OUT_ACCEPT     100
#define EXEC_OUT_REJECT     101
#define EXEC_JOB_COMPLETE   102
#define EXEC_END            103
#define EXEC_LOG            104
#define EXEC_CORE           105

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SIGNAL_ASYNC - handle signal SIG for an SC_exec_async call
 *                 - this is meant to be called only from an interrupt handler
 *                 - return TRUE iff the caller should exit
 *                 - otherwise the caller will simply return from the
 *                 - interrupt
 *                 - this is a client side routine
 *                 - NOTE: SC_exec, SC_exec_commands may also use this handler
 */

int SC_signal_async(int sig)
   {int rv;
    conpool *cp;
    asyncstate *cs;

    GET_CLIENT_STATE(cs);

    rv = TRUE;
    cp = cs->pool;
    if (cp != NULL)
       {if (cp->active == TRUE)
	   {switch (sig)
	       {case SIGUSR1 :
		     sig = SIGINT;
		     rv  = FALSE;
		     SC_show_pool_logs(cp, -1);
		     break;

		case SIGUSR2 :
		     sig = SIGINT;
		     rv  = FALSE;
		     SC_show_pool_stats(cp, -1, TRUE);
		     break;};

	    if (rv == TRUE)
	       LONGJMP(cp->cpu, sig);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REJECTED_PROCESS - handle a process that has hit reject output */

static void _SC_rejected_process(asyncstate *as, parstate *state,
				 taskdesc *job, int mask)
   {

    if (job->nzip++ > 0)
       SC_sleep(10);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SETUP_OUTPUT - setup the output string arrays for INF */

void _SC_setup_output(jobinfo *inf, const char *name)
   {int ns;

/* initial guess at number of output strings needed by job
 * do not start too small or fatal thrashing on the dynamic
 * arrays can happen because of interrupt handling
 */
    ns = 512;

    inf->out = _SC_string_array(name);
    SC_array_resize(inf->out, ns, -1.0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_OUTPUT - handle output from the child process
 *                    - store the output from the child in
 *                    - a dynamic array of strings derived from A
 */

void _SC_process_output(int fd, int mask, void *a)
   {int i, n;
    long nb, nr;
    char s[MAX_BFSZ+1];
    char *ps;
    PROCESS *pp;
    taskdesc *job;
    parstate *state;
    asyncstate *as;

    GET_SERVER_STATE(as);

    state = (parstate *) a;

    SC_START_ACTIVITY(state, EXEC_OUT_ACCEPT);

    n = SC_array_get_n(state->tasks);

    for (i = 0; i < n; i++)
        {job = *(taskdesc **) SC_array_get(state->tasks, i);
	 if (job != NULL)
	    {pp = job->pp;
/*
fprintf(stderr, "%-16s: %4d %8d (%d/%d)\n", "process_out", SC_current_thread(), pp->id, pp->status, pp->reason);
*/
	     if (SC_process_alive(pp))
	        {if (pp->io[0] == fd)
		    {nb = pp->n_read;

		     while (SC_gets(s, MAX_BFSZ, pp) != NULL)
		        {ps = CSTRSAVE(s);

			 SC_trim_right(s, "\n");
			 job->print(job, as, "out  '%s'\n", s);

			 SC_array_string_add(job->inf.out, ps);};

/* something is wrong if we read no characters because we were called
 * following a poll which said we had characters to read
 * NOTE: not any more - this is called from other places too
 */
		     nr = pp->n_read - nb;
		     if (nr == 0)
		        _SC_rejected_process(as, state, job, mask);
		     else
		        job->nzip = 0;

		     break;};};};};

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_OUT_REJECT - handle rejected output events from
 *                        - the asynchronous child processes
 *                        - FACT: there is no OS that can let the job
 *                        - continue from here so report the job as complete
 */

void _SC_process_out_reject(int fd, int mask, void *a)
   {int i, n, nr;
    PROCESS *pp;
    taskdesc *job;
    parstate *state;
    asyncstate *as;

    GET_SERVER_STATE(as);

    state = (parstate *) a;

    SC_START_ACTIVITY(state, EXEC_OUT_REJECT);

    nr = SC_running_children();
    SC_ASSERT(nr >= 0);

    n = SC_array_get_n(state->tasks);
    for (i = 0; i < n; i++)
        {job = *(taskdesc **) SC_array_get(state->tasks, i);
	 if (job != NULL)
	    {pp = job->pp;
	     if (SC_process_alive(pp))
	        {
/*
fprintf(stderr, "%-16s: %4d %8d (%d/%d)\n", "out_reject", SC_current_thread(), pp->id, pp->status, pp->reason);
*/
                 if ((pp->io[0] == fd) && (SC_status(pp) == SC_RUNNING))
		    _SC_rejected_process(as, state, job, mask);};};};

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_JOB_COMPLETE - do the work that needs to be done when
 *                  - a job is finished
 *                  - return TRUE iff the job is to be retried
 *                  - this is a client side routine
 */

static int _SC_job_complete(taskdesc *job, const char *msg)
   {int rtry;
    asyncstate *as;
    parstate *state;

    state = job->context;

    SC_START_ACTIVITY(state, EXEC_JOB_COMPLETE);

    GET_CLIENT_STATE(as);

    rtry = job->finish(job, as, FALSE);

/* if successful completion */
    if (rtry == 0)
       job->finished = TRUE;

/* otherwise retry */
    else
       {rtry = job->launch(job, as);
        if (rtry == FALSE)
	   job->finished = TRUE;};

    SC_END_ACTIVITY(state);

    return(rtry);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_END - return TRUE iff all processes have exited
 *                 - return the exit status in RV
 */

static int _SC_process_end(int *prv, void *a)
   {int i, n, ex, rv;
    int nr, nc;
    double tl, dt;
    taskdesc *job;
    asyncstate *as;
    parstate *state;

    state = (parstate *) a;

    GET_SERVER_STATE(as);

    SC_START_ACTIVITY(state, EXEC_END);

    SC_save_exited_children();

    n = SC_array_get_n(state->tasks);
    
    nr = 0;
    nc = 0;

    for (i = 0; i < n; i++)
        {job = *(taskdesc **) SC_array_get(state->tasks, i);
	 if (job != NULL)
	    job->check(job, as, &nr, &nc);
	 else
	    nc++;};

/* determine whether all jobs are finished */
    ex = ((nc == i) && (state->done == TRUE));

/* if all jobs are finished compute the exit status as the sum
 * of all process exit statuses
 */
    if (ex == TRUE)
       {rv = 0;
	for (i = 0; i < n; i++)
	    {job = *(taskdesc **) SC_array_get(state->tasks, i);
	     if (job != NULL)
	        rv += job->inf.status;};
	*prv = rv;};

/* determine whether the time limit is exhausted */
    tl = 1.0e-3*((double) state->time_limit);
    if (tl > 0.0)
       {dt = SC_wall_clock_time() - state->tstart;
	if (dt > tl)
	   {ex = TRUE;
	    rv = SC_EXIT_TIMEOUT;};};

    SC_END_ACTIVITY(state);

    return(ex);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SHOW_STATE_LOG - print the log for the given state */

void SC_show_state_log(parstate *state)
   {int it, nt;
    char *s;

    SC_START_ACTIVITY(state, EXEC_LOG);

    nt = SC_array_get_n(state->log);
    for (it = 0; it < nt; it++)
        {s = *(char **) SC_array_get(state->log, it);
	 io_printf(stdout, "> %s", s);};

    io_printf(stdout, "\n");

    SC_END_ACTIVITY(state);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SHELL_NO_RC_CMD - return the "flag" to run a shell command
 *                     - without reading the rc file for the shell
 *                     - the CSH family and BASH are the only
 *                     - predictable ones
 *                     - /bin/sh and /bin/ksh vary from platform to platform
 *                     - so other shells all return blank
 */

char *_SC_shell_no_rc_cmd(const char *shell)
   {static char rv[80];

    SC_strncpy(rv, 80, " ", -1);

    if (shell != NULL)
       {if (strstr(shell, "csh") != NULL)
	   SC_strncpy(rv, 80, "-f", -1);

	else if (strstr(shell, "bash") != NULL)
	   SC_strncpy(rv, 80, "--noprofile", -1);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXEC_ASYNC - execute each command in CMNDS under SHELL asynchronously
 *               - and with persistence
 *               - a word about shells:
 *               -    there is the connection shell CONSH and
 *               -    there is the command SHELL
 *               - they do not have to be the same
 *               - and are not in general
 *               - CONENV contains the environment variables to
 *               - be set in the connection
 *               - send output to file LNAME as well as stdout
 *               - filtering the output using the filter file FNAME
 *               - SHOW governs how commands are displayed:
 *               -    0 do not echo command
 *               -    1 echo commands not begining with '@'
 *               -    2 echo all commands
 *               - IGNORE controls error behavior
 *               -    0 do not execute more commands after one fails
 *               -    1 execute all commands
 *               - the array of commands is terminated by a NULL entry
 *               - the commands are attempted NA times with a random
 *               - time upto 30 seconds between attempts
 *               - if a command fails after NA attempts do not process
 *               - any remaining commands
 *               - return TRUE iff all commands succeed
 *               - this version uses a command pool
 */

int SC_exec_async(const char *shell, char **cmnds, char **dirs,
		  const char *consh, char **conenv, int nc,
		  const char *lname, const char *fname,
		  int na, int show, int ignore, int lg)
   {int is, id, nd, ns, ja, st, sig;
    int nhst, reset, ioi;
    char *ldir[1];
    fspec *filter;
    conpool *cp;
    asyncstate *as;
    SC_contextdes hnd;

    as = CMAKE(asyncstate);
    SC_MEM_INIT(asyncstate, as);

    _SC_setup_async_state(as,
			  SC_get_host_length_max(NULL, TRUE, FALSE));
    as->debug = FALSE;

    if (lname != NULL)
       {as->log = io_open(lname, "a");
	SC_setbuf(as->log, NULL);}
    else
       as->log = NULL;

/* save old interrupt state */
    hnd = SC_which_signal_handler(SC_SIGIO);
    ioi = SC_set_io_interrupts(FALSE);
    SC_signal_n(SC_SIGIO, SIG_IGN, NULL, 0);

    filter = _SC_read_filter(fname);
    shell  = SC_get_shell(shell);

/* count commands */
    SC_ptr_arr_len(ns, cmnds);

/* count the directories */
    if (dirs == NULL)
       {ldir[0] = SC_getcwd();
	dirs    = ldir;
	nd      = 1;}
    else
       SC_ptr_arr_len(nd, dirs);

/* open the connection pool */
    nhst = SC_get_nhosts(PSY_Arch);
    nc   = min(nc, nhst);
    nc   = min(nc, ns);
    cp   = SC_open_connection_pool(nc, PSY_Arch, consh, conenv,
				   na, show, ignore,
				   DEFAULT_HEARTBEAT, filter);

    cp->as   = as;
    as->pool = cp;

    sig = SETJMP(cp->cpu);
    if (sig == 0)
       {cp->active = TRUE;

/* launch all commands */
	reset = TRUE;
        for (id = 0; id < nd; id++)
	    for (is = 0; is < ns; is++)
	        {if (IS_BARRIER(cmnds[is]))
		    {while (SC_wait_pool_job(cp, DEFAULT_WAIT) != 2);
		     reset = TRUE;}
		 
		 else
		    {ja = SC_launch_pool_job(cp, na, reset, shell,
					     dirs[id], cmnds[is]);
		     if (ja < 0)
		        {LONGJMP(cp->cpu, SC_EXIT_BAD);};

		     reset = FALSE;};};

/* wait for commands to complete */
	while (SC_wait_pool_job(cp, DEFAULT_WAIT) != 2);};

    cp->active = FALSE;
    as->pool   = NULL;

    st = SC_connection_pool_status(cp);

    if (lg == TRUE)
       {SC_close_connection_pool(cp, -1, TRUE);
	as->to_stdout = FALSE;}
    else
       {as->to_stdout = FALSE;
	SC_close_connection_pool(cp, sig, TRUE);};

    CFREE(as);

/* cleanup */
    _SC_free_filter(filter);

/* restore old interrupt state */
    SC_set_io_interrupts(ioi);
    SC_restore_signal_n(SC_SIGIO, hnd);

    if (sig == SC_EXIT_BAD)
       st = sig;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXEC_ASYNC_S - execute each command in CMNDS under SHELL asynchronously
 *                 - and with persistence on each system in SYSL
 *                 - filtering the output using the filter file FNAME
 *                 - SHOW governs how commands are displayed:
 *                 -    0 do not echo command
 *                 -    1 echo commands not begining with '@'
 *                 -    2 echo all commands
 *                 - IGNORE controls error behavior
 *                 -    0 do not execute more commands after one fails
 *                 -    1 execute all commands
 *                 - the array of commands is terminated by a NULL entry
 *                 - the commands are attempted NA times with a random
 *                 - time upto 30 seconds between attempts
 *                 - if a command fails after NA attempts do not process
 *                 - any remaining commands
 *                 - return TRUE iff all commands succeed
 *                 - this version does one SSH per command
 */

int SC_exec_async_s(const char *shell, char **env,
		    char **sysl, char **cmnds, char **dirs,
		    int to, char *fname, int na, int show, int ignore)
   {int ic, id, is, nc, nd, ns;
    int ok, st, jid, ioi;
    char hst[MAXLINE];
    char *ldir[1], *pc, *sys, *cm;
    SC_evlpdes *pe;
    fspec *filter;
    taskdesc *job;
    asyncstate *cs;
    parstate state;
    SC_contextdes hnd;

    GET_CLIENT_STATE(cs);

/* count commands */
    SC_ptr_arr_len(nc, cmnds);

/* count system types */
    SC_ptr_arr_len(ns, sysl);

/* count the directories */
    if (dirs == NULL)
       {ldir[0] = SC_getcwd();
	dirs    = ldir;
	nd      = 1;}
    else
       SC_ptr_arr_len(nd, dirs);

    _SC_setup_async_state(cs, SC_get_sys_length_max(TRUE, FALSE));

/* save old interrupt state */
    hnd = SC_which_signal_handler(SC_SIGIO);
    ioi = SC_set_io_interrupts(FALSE);
    SC_signal_n(SC_SIGIO, SIG_IGN, NULL, 0);

    state.time_limit = to;
    state.tstart     = SC_wall_clock_time();

    shell  = SC_get_shell(shell);
    filter = _SC_read_filter(fname);
    pe     = SC_make_event_loop(NULL, NULL, _SC_process_end, -1, -1, -1);

    _SC_exec_setup_state(&state, shell, env,
			 na, show, TRUE, FALSE,
			 ignore, filter, pe,
			 _SC_process_output, _SC_process_out_reject,
			 _SC_job_complete);

/* launch all commands */
    jid = 0;
    for (is = 0; is < ns; is++)
        {sys = sysl[is];
	 for (id = 0; id < nd; id++)
	     {for (ic = 0; ic < nc; ic++)
		  {SC_get_host_name(hst, MAXLINE, sys);
		   pc = _SC_show_command(NULL, cmnds[ic], FALSE);
		   if ((strncmp(pc, "echo", 4) == 0) &&
		       (strchr(" \t", pc[4]) != NULL))
		      st = system(pc);

		   else
		      {cm  = SC_dsnprintf(TRUE, "%s %s -c 'cd ~ ; cd %s ; %s'",
					  shell,
					  _SC_shell_no_rc_cmd(shell),
					  dirs[id], pc);

		       job = SC_make_taskdesc(&state, jid++,
					      hst, shell, NULL, cm);
		       if (job != NULL)
			  {ok = job->start(job, NULL, TRUE);
			   if (ok == TRUE)
			      SC_array_push(state.tasks, &job);};

		       CFREE(cm);};};};};

/* we have launched everything we have */
    state.done = TRUE;

/* wait for commands to complete */
    st = SC_event_loop(pe, &state, DEFAULT_WAIT);

/* cleanup */
    SC_free_event_loop(pe);
    _SC_free_filter(filter);

    state.free_tasks(&state);

/* restore old interrupt state */
    SC_set_io_interrupts(ioi);
    SC_restore_signal_n(SC_SIGIO, hnd);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXEC_ASYNC_H - execute each command in CMNDS under SHELL asynchronously
 *                 - and with persistence on each host in HSTL
 *                 - filtering the output using the filter file FNAME
 *                 - SHOW governs how commands are displayed:
 *                 -    0 do not echo command
 *                 -    1 echo commands not begining with '@'
 *                 -    2 echo all commands
 *                 - IGNORE controls error behavior
 *                 -    0 do not execute more commands after one fails
 *                 -    1 execute all commands
 *                 - the array of commands is terminated by a NULL entry
 *                 - the commands are attempted NA times with a random
 *                 - time upto 30 seconds between attempts
 *                 - if a command fails after NA attempts do not process
 *                 - any remaining commands
 *                 - return TRUE iff all commands succeed
 *                 - this version does one SSH per command
 */

int SC_exec_async_h(const char *shell, char **env,
		    char **hstl, char **cmnds, char **dirs,
		    int to, char *fname, int na, int show, int ignore)
   {int ic, id, ih, nc, nd, nh;
    int ok, st, jid, ioi, ln;
    char *ldir[1], *pc, *hst, *cm;
    SC_evlpdes *pe;
    fspec *filter;
    taskdesc *job;
    parstate state;
    asyncstate *cs;
    SC_contextdes hnd;

    GET_CLIENT_STATE(cs);

/* count commands */
    SC_ptr_arr_len(nc, cmnds);

/* count hosts and get the maximum string length of all hosts */
    ln = 0;
    if (hstl == NULL)
       nh = 0;
    else
       {for (nh = 0; hstl[nh] != NULL; nh++)
	    {ih = strlen(hstl[nh]);
	     ln = max(ln, ih);};};

/* count the directories */
    if (dirs == NULL)
       {ldir[0] = SC_getcwd();
	dirs    = ldir;
	nd      = 1;}
    else
       SC_ptr_arr_len(nd, dirs);

    _SC_setup_async_state(cs, ln);

/* save old interrupt state */
    hnd = SC_which_signal_handler(SC_SIGIO);
    ioi = SC_set_io_interrupts(FALSE);
    SC_signal_n(SC_SIGIO, SIG_IGN, NULL, 0);

    state.time_limit = to;
    state.tstart     = SC_wall_clock_time();

    shell  = SC_get_shell(shell);
    filter = _SC_read_filter(fname);
    pe     = SC_make_event_loop(NULL, NULL, _SC_process_end, -1, -1, -1);

    _SC_exec_setup_state(&state, shell, env,
			 na, show, TRUE, FALSE,
			 ignore, filter, pe,
			 _SC_process_output, _SC_process_out_reject,
			 _SC_job_complete);

/* launch all commands */
    jid = 0;
    for (ih = 0; ih < nh; ih++)
        {hst = hstl[ih];
	 for (id = 0; id < nd; id++)
	     {for (ic = 0; ic < nc; ic++)
		  {pc = _SC_show_command(NULL, cmnds[ic], FALSE);

		   if (strcmp(dirs[id], "~") == 0)
		      cm = SC_dsnprintf(TRUE, pc);
		   else
		      cm = SC_dsnprintf(TRUE, "cd %s ; %s",
					dirs[id], pc);

		   job = SC_make_taskdesc(&state, jid++,
					  hst, shell, NULL, cm);
		   if (job != NULL)
		      {ok = job->start(job, NULL, TRUE);
		       if (ok == TRUE)
			  SC_array_push(state.tasks, &job);};

                   CFREE(cm);};};};

/* we have launched everything we have */
    state.done = TRUE;

/* wait for commands to complete */
    st = SC_event_loop(pe, &state, DEFAULT_WAIT);

/* cleanup */
    SC_free_event_loop(pe);
    _SC_free_filter(filter);

    state.free_tasks(&state);

/* restore old interrupt state */
    SC_set_io_interrupts(ioi);
    SC_restore_signal_n(SC_SIGIO, hnd);

    return(st);}

/*--------------------------------------------------------------------------*/

/*                          SINGLE PROCESS EXECUTION                        */

/*--------------------------------------------------------------------------*/

/* _SC_EXEC - the SCORE equivalent of the system() command
 *          - use SHELL if non-NULL
 *          - otherwise use value of SHELL environment variable
 *          - wait no more than TO milliseconds if TO > 0
 *          - if TO == -1 wait as long as it takes
 *          - return command output in OUT
 *          - return the exit status
 */

static int _SC_exec(int i, SC_array *out, execdes *ed)
   {int ns, rv, ioi, sto, st, nf, ok;
    int to, toi, na, dbg;
    int *res;
    char *cmnd, *cwd, **env;
    const char *shell;
    SC_evlpdes *pe;
    taskdesc *job;
    asyncstate *as;
    parstate state;
    SC_contextdes osi;
    conpool *cpo;
    asyncstate *cs;

    GET_CLIENT_STATE(cs);

    to    = ed->to;
    na    = ed->na;
    dbg   = ed->dmp;
    cmnd  = ed->cmnds[i];
    shell = ed->shell;
    env   = ed->env;
    res   = ed->res;

    nf = SC_gs.assert_fail;

    as  = NULL;
    cpo = cs->pool;
    sto = cs->to_stdout;

    _SC_setup_async_state(cs, 0);

/* save old interrupt state */
    ioi = SC_set_io_interrupts(FALSE);
    osi = SC_signal_n(SC_SIGIO, SIG_IGN, NULL, 0);

    rv = FALSE;

    state.time_limit = to;
    state.tstart     = SC_wall_clock_time();

/* create the event loop state */
    pe = SC_make_event_loop(NULL, NULL, _SC_process_end, -1, -1, -1);

    _SC_exec_setup_state(&state, shell, env,
			 na, 0, FALSE, FALSE,
			 FALSE, NULL, pe,
			 _SC_process_output, _SC_process_out_reject,
			 NULL);

    SC_START_ACTIVITY(&state, EXEC_CORE);

    cwd = SC_getcwd();

/* launch command */
    job = SC_make_taskdesc(&state, 0, NULL, shell, NULL, cmnd);
    if (job != NULL)
       {ok = job->start(job, as, TRUE);

	SC_ASSERT(ok >= 0);

	state.done = TRUE;

	job->add(job);

	SC_catch_event_loop_interrupts(pe, TRUE);

	toi = (SC_n_threads > 1) ? 1 : DEFAULT_WAIT;
        toi = 10;

	rv = SC_event_loop(pe, &state, toi);
	ns = SC_array_string_append(out, job->inf.out);
	SC_ASSERT(ns >= 0);

	state.free_tasks(&state);};

    res[i] = rv;

    _SC_set_run_task_state(NULL);

    if (cwd != NULL)
       {st = chdir(cwd);
	SC_ASSERT(st == 0);

	CFREE(cwd);};

    if (dbg == TRUE)
       SC_show_state_log(&state);

    SC_END_ACTIVITY(&state);

    _SC_exec_free_state(&state, TRUE);

    SC_free_event_loop(pe);

/* restore old interrupt state */
    SC_set_io_interrupts(ioi);
    SC_restore_signal_n(SC_SIGIO, osi);

    cs->pool      = cpo;
    cs->to_stdout = sto;

    nf = SC_gs.assert_fail - nf;
    if (nf > 0)
       state.print(&state, "***> %d internal assertions violated", nf);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXEC - the SCORE equivalent of the system() command
 *         - use SHELL if non-NULL
 *         - otherwise use value of SHELL environment variable
 *         - wait no more than TO milliseconds if TO > 0
 *         - if TO == -1 wait as long as it takes
 *         - return command output in OUT and
 *         - no line in OUT will have a newline except at the very end
 *         - return the exit status
 */

int SC_exec(char ***out, char *cmnd, const char *shell, int to)
   {int st;
    SC_array *str;
    SC_thread_proc *ps;
    static int dbg = FALSE;

    ps = _SC_get_thr_processes(-1);

    ps->ed.n      = 1;
    ps->ed.to     = to;
    ps->ed.na     = 1;
    ps->ed.show   = FALSE;
    ps->ed.dmp    = dbg;
    ps->ed.ignore = FALSE;
    ps->ed.shell  = shell;
    ps->ed.cmnds  = &cmnd;
    ps->ed.env    = NULL;
    ps->ed.filter = NULL;
    ps->ed.res    = &st;

    str = SC_STRING_ARRAY();

    st = _SC_exec(0, str, &ps->ed);

/* return command output in good condition */
    *out = _SC_array_string_join(&str);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXECA - alternate form of SC_exec
 *          - the command line is derived from the variable arg list
 *          - use SHELL if non-NULL
 *          - otherwise use value of SHELL environment variable
 *          - wait no more than TO milliseconds if TO > 0
 *          - if TO == -1 wait as long as it takes
 *          - return command output in OUT and
 *          - no line in OUT will have a newline except at the very end
 *          - return the exit status
 */

int SC_execa(char ***out, const char *shell, int to, const char *fmt, ...)
   {int rv;
    char *cmd;

    *out = NULL;

    SC_VDSNPRINTF(TRUE, cmd, fmt);

    rv = SC_exec(out, cmd, shell, to);
    
    CFREE(cmd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXECS - alternate form of SC_exec
 *          - the command line is derived from the variable arg list
 *          - use SHELL if non-NULL
 *          - otherwise use value of SHELL environment variable
 *          - wait no more than TO milliseconds if TO > 0
 *          - if TO == -1 wait as long as it takes
 *          - a single line of output of no more than NC characters
 *          - is returned in OUT iff the command complete within TO seconds
 *          - if TO is less than 0 the timeout is infinite
 *          - return the exit status or -1 on timeout
 */

int SC_execs(char *out, int nc, const char *shell, int to, const char *fmt, ...)
   {int n, rv;
    char *s, *t, *p, *cmd, **res;

    SC_VDSNPRINTF(TRUE, cmd, fmt);

/* NOTE: this makes valgrind happy */
    memset(out, 0, nc);

    rv = SC_exec(&res, cmd, shell, to);
    if (rv == 0)
       {if (out != NULL)
	   {s = res[0];
	    t = SC_strtok(s, "\n\r\f", p);
	    if (t != NULL)
	       {n = strlen(t);
		SC_strncpy(out, nc, t, n);}
	    else
	       out[0] = '\0';};

        SC_free_strings(res);};
    
    CFREE(cmd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SYSTEM - wrapper around the system() command
 *           - to complain on systems lacking fork/exec
 *           - do NOT make this vararg because things like
 *           -  SC_system("date '+%Y'")
 *           - are too nasty to deal with given the prototype constraint
 */

int SC_system(const char *cmd)
   {int rv;

#ifdef HAVE_FORK_EXEC
    rv = system(cmd);
#else
    rv = -1;
#endif    

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SYSCMND - the SCORE equivalent of the system() command, returning
 *            - command output in char**
 *            - If flag is TRUE have sh parse the command
 */

char **SC_syscmnd(const char *fmt, ...)
   {int rv;
    char **icmnd, *cmd;

    SC_VDSNPRINTF(TRUE, cmd, fmt);

    rv = SC_exec(&icmnd, cmd, NULL, -1);
    SC_ASSERT(rv == 0);
    
    CFREE(cmd);

    return(icmnd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_ONE - execute one chunk of commands
 *              - thread worker for SC_exec_commands
 */

int _SC_exec_one(void **a, int *it)
   {int i, mn, mx, st, err;
    char *cm, **sa;
    execdes *ed;
    SC_array *out;
    asyncstate *cs;

    GET_CLIENT_STATE(cs);

    mn = it[0];
    mx = it[1];
    ed = *(execdes **) a;

    st = 0;

    for (i = mn; (i < mx) && ((st == 0) || (ed->ignore == TRUE)); i++)
        {out = SC_STRING_ARRAY();

	 cm           = ed->cmnds[i];
	 ed->cmnds[i] = _SC_put_command(out, cm, ed->show);

	 err = _SC_exec(i, out, ed);

	 ed->cmnds[i] = cm;

	 sa = SC_array_done(out);
	 if (sa != NULL)
	    _SC_print_filtered(cs, sa, ed->filter, -1, NULL);

	 SC_free_strings(sa);

	 st = (err != 0);
	 ed->res[i] = st;};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_EXEC_COMMANDS - execute each command in CMNDS with persistence
 *                  - using SHELL
 *                  - send output to file LNAME as well as stdout
 *                  - filtering the output using the filter file FNAME
 *                  - SHOW governs how commands are displayed:
 *                  -    0 do not echo command
 *                  -    1 echo commands not begining with '@'
 *                  -    2 echo all commands
 *                  - IGNORE controls error behavior
 *                  -    0 do not execute more commands after one fails
 *                  -    1 execute all commands
 *                  - DMP causes all internal logs to be dumped
 *                  - the array of commands is terminated by a NULL entry
 *                  - the commands are attempted NA times with a random
 *                  - time upto 30 seconds between attempts
 *                  - if a command fails after NA attempts do not process
 *                  - any remaining commands
 *                  - return TRUE iff all commands succeed
 */

int SC_exec_commands(const char *shell, char **cmnds, char **env, int to,
		     char *lname, char *fname, int na, int show,
		     int ignore, int dmp)
   {int i, n, st, sto;
    conpool *cpo;
    fspec *filter;
    asyncstate *cs;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(-1);

    GET_CLIENT_STATE(cs);

/* filter junk messages out of response */
    filter = _SC_read_filter(fname);

    cpo = cs->pool;
    sto = cs->to_stdout;

    _SC_setup_async_state(cs, 0);

    if (lname != NULL)
       {cs->log = io_open(lname, "a");
	SC_setbuf(cs->log, NULL);}
    else
       cs->log = NULL;

    SC_ptr_arr_len(n, cmnds);

    ps->ed.n      = n;
    ps->ed.to     = to;
    ps->ed.na     = na;
    ps->ed.show   = show;
    ps->ed.dmp    = dmp;
    ps->ed.ignore = ignore;
    ps->ed.cmnds  = cmnds;
    ps->ed.shell  = shell;
    ps->ed.env    = env;
    ps->ed.filter = filter;
    ps->ed.res    = CMAKE_N(int, n);
    for (i = 0; i < n; i++)
        ps->ed.res[i] = 0;

/* run each command until it succeeds or definitively fails
 * try to avoid failing on system fault type errors
 */
    st = 0;
    if (SC_n_threads > 1)
       {SC_do_tasks((PFInt) _SC_exec_one, &ps->ed, n, 0, TRUE);
	for (i = 0; i < n; i++)
	    st |= ps->ed.res[i];}
    else
       {int it[2];
	void *a;

	a     = &ps->ed;
	it[0] = 0;
	it[1] = n;
	st = _SC_exec_one(&a, it);
	for (i = 0; i < n; i++)
	    st |= ps->ed.res[i];};

    if (lname != NULL)
       io_close(cs->log);

    cs->pool      = cpo;
    cs->to_stdout = sto;

    _SC_free_filter(filter);
    CFREE(ps->ed.res);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
