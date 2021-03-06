/*
 * SCPMAN.C - process manager routines for process control
 *          - in SCORE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

#include "scope_proc.h"

#define N_PROC_MNG  32

typedef struct s_sigchld_rec sigchld_rec;

struct s_sigchld_rec
   {int pid;            /* process id */
    int condition;      /* signalled, stopped, exited, ... */
    int exit;};         /* exit status */

extern void
 dstatelog(const char *fmt, ...);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DRPROC - report the managed processes
 *        - strictly a degugging aid
 */

void drproc(int ev, int pid, int tid)
   {int i, np;
    char idx[10], prc[10], st[80];
    PROCESS *pp;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(tid);

    np = SC_array_get_n(ps->process_list);

    switch (ev)
       {case SC_PROC_EXEC :
	     SC_strncpy(st, 80, "exec of", -1);
	     break;
	case SC_PROC_SIG :
	     SC_strncpy(st, 80, "signal to", -1);
	     break;
	case SC_PROC_IO :
	     SC_strncpy(st, 80, "close io to", -1);
	     break;
	case SC_PROC_RM :
	     SC_strncpy(st, 80, "interrupt from", -1);
	     break;
        default :
	     SC_strncpy(st, 80, "unknown", -1);
	     break;};

    io_printf(stdout, "Managed process list after %s %d (%d/%d)\n",
	      st, pid, np, ps->process_list->nx);

    io_printf(stdout, "   #  Index       Address       PID  State  # Ref\n");
    for (i = 0; i < np; i++)
        {pp = *(PROCESS **) SC_array_get(ps->process_list, i);
	 if (pp == NULL)
	    continue;

	 st[0] = '\0';
	 if (pp->flags & SC_PROC_EXEC)
	    SC_strcat(st, 80, "e");
	 if (pp->flags & SC_PROC_SIG)
	    SC_strcat(st, 80, "t");
	 if (pp->flags & SC_PROC_IO)
	    SC_strcat(st, 80, "i");
	 if (pp->flags & SC_PROC_RM)
	    SC_strcat(st, 80, "f");

	 switch (pp->index)
	    {case SC_PROC_FREE :
		  snprintf(idx, 10, "free ");
		  break;
	     case SC_PROC_ALLOC :
		  snprintf(idx, 10, "alloc");
		  break;
	     case SC_PROC_DELETED :
		  snprintf(idx, 10, " del ");
		  break;
	     case SC_PROC_CLOSED :
		  snprintf(idx, 10, " cls ");
		  break;
	     default :
		  snprintf(idx, 10, "%3d  ", pp->index);
		  break;};

	 snprintf(prc, 10, "%8d", pp->id);

	 if (!SC_process_alive(pp))
	    io_printf(stdout, "PROCESS LIST CORRUPT\n\n");
	 else
	    io_printf(stdout, " %3d  %5s  %12p  %8s  %5s  %3d\n",
		      i, idx, pp, prc, st, SC_ref_count(pp));};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DRWAIT - report the list of processes which the OS tells us have finished
 *        - strictly a degugging aid
 */

void drwait(int tid)
   {int i, np, nx, ic;
    char *cnd;
    sigchld_rec *sr;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(tid);

    np  = SC_array_get_n(ps->wait_list);
    nx  = ps->wait_list->nx;
    cnd = "none";

    io_printf(stdout, "Finished process list (%d/%d)\n", np, nx);
    io_printf(stdout, "    #      PID   Exit Status\n");
    for (i = 0; i < np; i++)
        {sr = *(sigchld_rec **) SC_array_get(ps->wait_list, i);
	 ic = sr->condition;
	 if (ic & SC_SIGNALED)
	    cnd = "signaled";
	 else if (ic & SC_KILLED)
	    cnd = "killed";
	 else if (ic & SC_DEAD)
	    cnd = "dead";
	 else if (ic & SC_STOPPED)
	    cnd = "stopped";
	 else if (ic & SC_EXITED)
	    cnd = "ok";

         io_printf(stdout, " %4d  %8d  %3d  %s\n",
		   i, sr->pid, sr->exit, cnd);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LOG_PROC - log process management */

static void _SC_log_proc(int tid, const char *fmt, ...)
   {char t[MAXLINE];
    static int dbg = FALSE;

    if (dbg == TRUE)
       {SC_VA_START(fmt);
	SC_VSNPRINTF(t, MAXLINE, fmt);
	SC_VA_END;

	fprintf(stderr, "%2d>>> %s\n", tid, t);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_THR_PROCESSES - initialize an SC_thread_proc instance */

void _SC_init_thr_processes(SC_thread_proc *ps, int id)
   {

    assert(ps != NULL);

    memset(ps, 0, sizeof(SC_thread_proc));

    ps->tid                     = id;
    ps->current_flushed_process = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_GET_THR_PROCESSES - return the process list for thread ID
 *                       - use current thread if ID == -1
 */

SC_thread_proc *_SC_get_thr_processes(int id)
   {SC_thread_proc *ps;
    SC_smp_state *pa;

    pa = _SC_get_state(id);
    ps = pa->proc;

    return(ps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIN_THR_PROCESSES - cleanup the SC_thread_proc instances */

void _SC_fin_thr_processes(void)
   {int i, nt;
    SC_thread_proc *ps;

    nt = max(SC_n_threads, 1);
    for (i = 0; i < nt; i++)
        {ps = _SC_get_thr_processes(i);

	 SC_free_array(ps->wait_list, SC_array_free_n);
	 SC_free_array(ps->process_list, NULL);}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TID_PROC - get the ID for the current thread
 *              - and be sure that there is an initialized
 *              - SC_thread_proc instance for this thread
 */

SC_thread_proc *_SC_tid_proc(void)
   {int id;
    SC_thread_proc *ps;

    id = SC_current_thread();
    ps = _SC_get_thr_processes(id);

    return(ps);}

/*--------------------------------------------------------------------------*/

/*                      SIGCHLD MANAGEMENT ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _SC_MAKE_WAIT - initialize a sigchld_rec */

static void _SC_make_wait(void *a)
   {sigchld_rec **v;

    v = (sigchld_rec **) a;
    if (v != NULL)
       *v = CPMAKE(sigchld_rec, 3);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_WAIT - initialize the wait_list array of PS
 *               - this is called out so that the list can be initialized
 *               - in a thread safe execution region
 *               - NOTE: on LINUX boxes if the child completes while
 *               - the parent is still in the fork we can hit a thread
 *               - lock if we have to allocate any memory
 */

static void _SC_init_wait(SC_thread_proc *ps)
   {

/* if never initialized do it now */
    if (ps->wait_list == NULL)
       {ps->wait_list = CMAKE_ARRAY(sigchld_rec *, _SC_make_wait, 3);
	SC_array_resize(ps->wait_list, N_PROC_MNG, -1.0);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RECORD_WAIT - record the process PID in the list
 *                 - of processes for which a waitpid has succeeded
 */

static void _SC_record_wait(SC_thread_proc *ps, int pid, int cnd, int sts)
   {sigchld_rec sr, **ra;
    
    _SC_init_wait(ps);

    if (pid > 0)
       {sr.pid       = pid;
	sr.condition = cnd;
	sr.exit      = sts;

	SC_array_inc_n(ps->wait_list, 1L, 1);
	ra = SC_array_get(ps->wait_list, -1);

	**ra = sr;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LOOKUP_EXITED_CHILD - lookup the process PID in the PS list
 *                         - of processes for which a waitpid has succeeded
 *                         - return TRUE iff found
 *                         - if found return the condition, PCND,
 *                         - and status, PSTS
 */

static int _SC_lookup_exited_child(SC_thread_proc *ps, int pid,
				   int *pcnd, int *psts)
   {int i, n, rv;
    sigchld_rec *sr;

    rv = FALSE;
    if (pid > 0)
       {n = SC_array_get_n(ps->wait_list);

	for (i = n-1; 0 <= i; i--)
	    {sr = *(sigchld_rec **) SC_array_get(ps->wait_list, i);
	     if (sr->pid == pid)
	        {rv = TRUE;

		 if (pcnd != NULL)
		    *pcnd = sr->condition;

		 if (psts != NULL)
		    *psts = sr->exit;

		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MARK_EXITED_CHILD - mark the PID in the list of processes
 *                       - telling us that the PROCESS which had this
 *                       - PID is done and gone
 */

static void _SC_mark_exited_child(PROCESS *pp)
   {int i, n, pid;
    sigchld_rec *sr;
    SC_thread_proc *ps;

    ps = pp->tstate;

    if (SC_process_alive(pp))
       {n = SC_array_get_n(ps->wait_list);

	pid = pp->id;
	for (i = n-1; 0 <= i; i--)
	    {sr = *(sigchld_rec **) SC_array_get(ps->wait_list, i);
	     if (sr->pid == pid)
	        {_SC_log_proc(ps->tid,
			      "mark as exited process %d", sr->pid);
		 sr->pid *= -1;
                 break;};};};

    return;}

/*--------------------------------------------------------------------------*/

/*                      PROCESS MANAGEMENT ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _SC_MANAGE_PROCESS - record the process in the list
 *                    - of managed processes
 */

void _SC_manage_process(PROCESS *pp)
   {SC_thread_proc *ps;

    ps = pp->tstate;

    if (ps->process_list == NULL)
       {ps->process_list = CMAKE_ARRAY(PROCESS *, NULL, 3);
	SC_array_resize(ps->process_list, N_PROC_MNG, -1.0);};

    pp->index = SC_array_get_n(ps->process_list);

    SC_array_push(ps->process_list, &pp);

    SC_mark(pp, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LOOKUP_PROCESS - given a PID return the managed process
 *                    - which contains it or NULL
 */

static PROCESS *_SC_lookup_process(int pid, int tid)
   {int i, n;
    PROCESS *pp;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(tid);

    n = SC_array_get_n(ps->process_list);
    for (i = 0; i < n; i++)
        {pp = *(PROCESS **) SC_array_get(ps->process_list, i);
	 if (pp->id == pid)
	    break;};

    if (i >= n)
       pp = NULL;

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LOOKUP_PROCESS - given a PID return the managed process
 *                   - which contains it or NULL
 */

PROCESS *SC_lookup_process(int pid)
   {int it, nt;
    PROCESS *pp;

    pp = NULL;

    nt = max(SC_n_threads, 1);
    for (it = 0; (it < nt) && (pp == NULL); it++)
        pp = _SC_lookup_process(pid, it);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DELETE_PID - remove the process with PID from the list
 *                - of managed processes
 *                - at this point the PROCESS will have a child that
 *                - was exec'd (SC_PROC_EXEC) and we have received the
 *                - SIGCHILD (the SC_PROC_RM will be added here)
 *                - it may or may not have been closed (SC_PROC_SIG)
 *                - closed only means that the parent sent a kill signal
 *                - the child may have terminated on its own
 *                - the I/O may or may not have been checked (SC_PROC_IO)
 */

int _SC_delete_pid(int pid)
   {int i, it, n, na, nt, fl, fr, ok;
    PROCESS *pp;
    SC_thread_proc *ps;

    fr = FALSE;

    na = 0;
    ok = TRUE;
    nt = max(SC_n_threads, 1);
    for (it = 0; (it < nt) && (ok == FALSE); it++)
        {ps = _SC_get_thr_processes(it);

	 n  = SC_array_get_n(ps->process_list);
	 na = max(n, na);

	 for (i = 0; i < n; i++)
	     {pp = *(PROCESS **) SC_array_get(ps->process_list, i);

/* do not remove the process if the I/O has not been finished
 * someone might still come looking for it
 */
	      fl = pp->flags & SC_PROC_IO;

	      if ((pp->id == pid) && (fl != 0))
		 {SC_rl_io_callback(pp->io[0]);
		  SC_rl_io_callback(pp->io[1]);

		  _SC_mark_exited_child(pp);

		  fr = SC_process_state(pp, SC_PROC_RM | SC_PROC_SIG);

		  n = SC_array_remove(ps->process_list, i);
		  if ((i < n) && (fr == FALSE))
		     pp->index = i;

		  ok = FALSE;
		  break;};};

	 SC_array_set_n(ps->process_list, n);};

    if (na == 0)
       SC_reset_terminal();

    return(fr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RL_PROCESS - release a PROCESS structure
 *                - hard to know who may still be pointing to this
 *                - so ream out the state thoroughly
 */

static void _SC_rl_process(PROCESS *pp)
   {

    if (SC_safe_to_free(pp))
       {if ((pp->vif != NULL) && (pp->rl_vif != NULL))
	   (*pp->rl_vif)(pp->vif);

	CFREE(pp->start_time);
	CFREE(pp->stop_time);
	CFREE(pp->spty);

	PS_ring_clear(&pp->ior);

        pp->status    = -1;
        pp->reason    = -1;
        pp->id        = -1;
        pp->root      = -1;
        pp->type      = -1;
        pp->io[0]     = -1;
        pp->io[1]     = -1;
        pp->io[2]     = -1;
        pp->data      = -1;
        pp->ischild   = -1;
        pp->flags     = -1;
        pp->data_type = SC_UNKNOWN;
        pp->medium    = NO_IPC;
        pp->line_sep  = -1;
        pp->line_mode = -1;

	pp->n_zero    = -1;
	pp->n_read    = -1;
	pp->n_write   = -1;

	pp->on_exit  = NULL;
	pp->exit_arg = NULL;

        pp->release  = NULL;
	pp->exec     = NULL;
	pp->statusp  = NULL;
	pp->close    = NULL;
	pp->flush    = NULL;
	pp->read     = NULL;
	pp->write    = NULL;
	pp->printf   = NULL;
	pp->gets     = NULL;
	pp->in_ready = NULL;
	pp->lost     = NULL;
	pp->setup    = NULL;

	CFREE(pp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PROCESS_STATE - add EV to the PP flags member
 *                  - if all of the criteria are met then
 *                  - and only then free the PROCESS
 *                  - return TRUE iff PROCESS released
 *                  - NOTE: always free a child PROCESS
 */

int SC_process_state(PROCESS *pp, int ev)
   {int fl, fr, chld, rv;
    SC_thread_proc *ps;

    ps = pp->tstate;

    pp->flags |= ev;

    fr   = FALSE;
    fl   = pp->flags;
    chld = pp->ischild;

    if (chld == FALSE)
       {if (pp->id != 0)
	   _SC_log_proc(ps->tid, "add process #%d (%d)",
			pp->index, pp->id);

/* if we have closed the I/O there is no further possibility of
 * communication with this process so remove it from
 * the managed process list
 * and assign an exit status for the process
 */
	if (ev == SC_PROC_IO)

/* if we killed it count it as a normal termination */
	   {if (fl & SC_PROC_SIG)
	       _SC_set_process_status(pp, SC_EXITED | SC_CHANGED, 0, NULL);

/* otherwise mark this in a recognizable way */
	    else
	       _SC_set_process_status(pp, 0, -100, NULL);

	    fr = _SC_delete_pid(pp->id);};

	if (ps->debug_proc)
	   drproc(ev, pp->id, ps->tid);};

    rv = fr;
    if ((fr == FALSE) &&
	((fl == 0xf) || ((fl & SC_PROC_EXEC) == 0) || (chld == TRUE)))
       {rv = TRUE;
       _SC_rl_process(pp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SET_PROCESS_STATUS - set the status, reason, and stop_time
 *                        - of the given process PP
 */

void _SC_set_process_status(PROCESS *pp, int sts, int rsn, char *tm)
   {SC_thread_proc *ps;

    if (tm == NULL)

#if defined(SGI) || defined(SOLARIS) || defined(MACOSX)
       tm = NULL;
#else
       tm = SC_datef();
#endif

    CFREE(pp->stop_time);

    pp->status    = sts;
    pp->reason    = rsn;
    pp->stop_time = tm;

    if (pp->id != 0)
       {ps = pp->tstate;
	_SC_log_proc(ps->tid,
		     "change status on process #%d (%d) to %d/%d",
		     pp->index, pp->id, sts, rsn);};
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHILD_EXIT - given the wait results W
 *                - return the exit condition and status
 *                - via PCND and PSTS respectively
 */

static void _SC_child_exit(int w, int *pcnd, int *psts)
   {int cnd, sts;

    cnd = -1;
    sts = -1;

    if (WIFEXITED(w))
       {cnd = SC_EXITED | SC_CHANGED;
	sts = WEXITSTATUS(w);}

    else if (WIFSIGNALED(w))
       {cnd = SC_SIGNALED | SC_CHANGED;
	sts = WTERMSIG(w);}

    else if (WIFSTOPPED(w))
       {cnd = SC_STOPPED | SC_CHANGED;
	sts = WSTOPSIG(w);};

    if (pcnd != NULL)
       *pcnd = cnd;

    if (psts != NULL)
       *psts = sts;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHILD_CHECK - do a non-blocking waitpid on PID and return
 *                 -    -2   on error
 *                 -    -1   if child is unknown (never existed)
 *                 -     0   if child is still running
 *                 -    PID  if child is known and has completed
 *                 - a value of -1 is acceptable for PID in which case
 *                 - any one child that is finished will be reported
 *                 - if PID was indeed exec'd and has exited
 *                 - it will be returned with condition and status info
 *                 - NOTE: this is like waitpid except that error and
 *                 - non-existent process have been separated into
 *                 - distinct cases
 *                 - NOTE: it is unlike waitpid in that the condition
 *                 - and status info is saved so that future calls to
 *                 - SC_child_status will always return the PID, condition,
 *                 - and status info (waitpid is like a delta function
 *                 - and SC_child_status is like a step function)
 */

static int _SC_child_check(SC_thread_proc *ps, int pid)
   {int st, w, cnd, sts;

    SC_signal_block(NULL, SC_ALL_SIGNALS, -1);

    st = waitpid(pid, &w, WNOHANG);
    if (st > 0)
       {cnd = -1;
	sts = -1;

	_SC_child_exit(w, &cnd, &sts);
	_SC_record_wait(ps, st, cnd, sts);}

    else if ((st == -1) && (errno != ECHILD))
       st = -2;

    SC_signal_unblock(NULL, SC_ALL_SIGNALS, -1);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHILD_KILL - kill a child
 *               - do not take no for an answer
 *               - return TRUE iff successful
 */

int SC_child_kill(int pid)
   {int i, na, rv, w, wst, kst;
    int sa[2] = {SIGTERM, SIGKILL};

    rv = FALSE;
    na = sizeof(sa)/sizeof(int);
    for (i = 0; i < na; i++)

/* send the signal */
        {kst = kill(pid, sa[i]);
	 if (kst == 0)
	    {wst = 0;

	     SC_signal_block(NULL, SC_ALL_SIGNALS, -1);
	     SC_signal_unblock(NULL, SIGALRM, -1);

/* wait at most a second for the child to die */
	     if (SC_time_allow(1) == 0)
	        {wst = waitpid(pid, &w, 0);
		 SC_time_allow(0);};

	     SC_signal_unblock(NULL, SC_ALL_SIGNALS, -1);

/* if the child is gone we are done */
	     if (wst == pid)
	        {rv = TRUE;
		 break;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SAVE_EXITED_CHILDREN - check on the state of child processes
 *                         - each child process which completes is
 *                         - recorded along with condition and status info
 *                         - for later retrieval by _SC_lookup_exited_child
 */

void SC_save_exited_children(void)
   {int st;
    SC_thread_proc *ps;

    ps = _SC_tid_proc();

    _SC_init_wait(ps);

/* look for any processes finishing up */
    while (TRUE)
       {st = _SC_child_check(ps, -1);
	if (st <= 0)
	   break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHILD_STATUS - check on the state of a child process and return
 *                 -    -2   on error
 *                 -    -1   if child is unknown (never existed)
 *                 -     0   if child is still running
 *                 -     PID if child is known and has completed
 *                 - the child process id is specified by PID
 *                 - a value of -1 is acceptable for PID in which case
 *                 - any one child that is finished will be reported
 *                 - if PID was indeed exec'd and has exited
 *                 - it will be returned with condition and status info
 *                 - NOTE: this is like waitpid except that error and
 *                 - non-existent process have been separated into
 *                 - distinct cases
 *                 - NOTE: it is unlike waitpid in that the condition
 *                 - and status info is saved so that future calls to
 *                 - SC_child_status will always return the PID, condition,
 *                 - and status info (waitpid is like a delta function
 *                 - and SC_child_status is like a step function)
 *                 -
 *                 - Examples:
 *                 -    SC_child_status(5218, NULL, NULL);
 *                 -    SC_child_status(5218, NULL, &exit);
 *                 -    SC_child_status(-1, &sig, &exit);
 */

int SC_child_status(int pid, int *pcnd, int *psts)
   {int it, nt, st, ok;
    SC_thread_proc *ps;

    nt = max(SC_n_threads, 1);
    for (it = 0; it < nt; it++)
        {ps = _SC_get_thr_processes(it);

	 _SC_init_wait(ps);

	 st = _SC_child_check(ps, pid);

/* now lookup the requested process */
	 ok = _SC_lookup_exited_child(ps, pid, pcnd, psts);
	 if (ok == TRUE)
	    st = pid;

	 if (st == -1)
	    continue;
	 else
	    break;};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PROCESS_INIT_RUSAGE - initialize a resource usage descriptor instance */

SC_process_rusedes *SC_process_init_rusage(void)
   {SC_process_rusedes *pru;

    pru = CMAKE(SC_process_rusedes);

    if (pru != NULL)
       {pru->wcr   = SC_wall_clock_time();
	pru->wct   = 0.0;
        pru->since = 0.0;
	pru->idlet = 0.0;
	pru->usrt  = 0.0;
	pru->syst  = 0.0;
	pru->mem   = 0.0;
	pru->dusrt = 0.0;
	pru->dsyst = 0.0;
	pru->dmem  = 0.0;};

    return(pru);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PROCESS_FREE_RUSAGE - release a resource usage descriptor instance */

void SC_process_free_rusage(SC_process_rusedes *pru)
   {

    CFREE(pru);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PROCESS_RUSAGE - update the resource usage of the process PP
 *                   - info include total resource consumption and
 *                   - current rate of consumption
 *                   - return TRUE if the process is progressing
 */

void SC_process_rusage(PROCESS *pp)
   {int rv;
    double t, dt, idt;
    double dut, dst, dmem;
    SC_rusedes ru;
    SC_process_rusedes *pru;

    if (pp != NULL)
       {pru = pp->pru;

/* initialize if necessary */
	if (pru == NULL)
	   {pru = SC_process_init_rusage();
	    pp->pru = pru;};

	if (pru != NULL)
	   {rv = SC_resource_usage(&ru, pp->id);
	    if (rv == TRUE)
	       {t  = SC_wall_clock_time() - pru->wcr;
		dt = t - pru->wct;

/* compute the rates */
		if (dt != 0.0)
		   {idt  = 1.0/dt;
		    dut  = (ru.ut - pru->usrt)*idt;
		    dst  = (ru.st - pru->syst)*idt;
		    dmem = (ru.maxrss - pru->mem)*idt;}
		else
		   {dut  = 0.0;
		    dst  = 0.0;
		    dmem = 0.0;};

		if ((dut != 0.0) || (dst != 0.0))
		   pru->since = 0.0;
		else
		   {pru->since += dt;
		    pru->idlet += dt;};

/* update the rates */
		pru->dusrt = dut;
		pru->dsyst = dst;
		pru->dmem  = dmem;

/* update the resources */
		pru->usrt = ru.ut;
		pru->syst = ru.st;
		pru->mem  = ru.maxrss;
		pru->wct  = t;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PROCESS_STATUS - check on the state of a process PP and return
 *                   - return the process status which will be
 *                   -   a) SC_RUNNING if the process is running
 *                   -   b) a combination of SC_EXITED, SC_STOPPED,
 *                   -      SC_SIGNALED, SC_KILLED, SC_DEAD, SC_LOST,
 *                   -      or SC_CHANGED
 *                   -      if the process has exited
 *                   -   c) -1 on error or no process
 */

int SC_process_status(PROCESS *pp)
   {int rv, ex, pid;
    int cnd, sts;

    rv  = -1;
    cnd = 0;
    sts = 0;
    if (SC_process_alive(pp))

/* if it was running up til now check for change */
       {if (SC_status(pp) == SC_RUNNING)
	   {pid = pp->id;

	    SC_process_rusage(pp);

/* we lost track of the process or it is completely stalled */
	    if ((pp->lost != NULL) && ((*pp->lost)(pp) == TRUE))
	       {ex = -3;
		dstatelog("idle process %d (%d/%d)\n",
			  pid, SC_LOST, SC_EXIT_TIMEOUT);
		_SC_set_process_status(pp, SC_EXITED | SC_LOST, SC_EXIT_BAD,
				       NULL);}
	    else
	       ex = SC_child_status(pid, &cnd, &sts);

	    SC_ASSERT(ex >= 0);

/* if it just finished record the condition and status */
	    if (ex == pid)
	       {_SC_set_process_status(pp, cnd, sts, NULL);
	        if (pp->on_exit != NULL)
		   (*pp->on_exit)(pp, pp->exit_arg);

/* we promise to do this at most once */
		pp->on_exit  = NULL;
		pp->exit_arg = NULL;}

/* the system lost the process */
	    else if (ex == -1)
	       {dstatelog("process lost %d (%d/%d)\n", pid, cnd, sts);
		_SC_set_process_status(pp, SC_EXITED | SC_DEAD, SC_EXIT_BAD,
				       NULL);}

/* other system error regarding the process
 * we should never get here, so the real job is to figure out how we
 * managed to do it anyway
 */
	    else if (ex == -2)
	       {dstatelog("bad process %d (%d/%d)\n", pid, cnd, sts);
		_SC_set_process_status(pp, SC_EXITED | SC_DEAD, SC_EXIT_BAD,
				       NULL);};};

	rv = SC_status(pp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHECK_CHILDREN - check each running child on the managed list
 *                    - and update its run/exit status
 *                    - return the number of children who finished
 *                    - during this call
 */

static int _SC_check_children(int tid)
   {int i, n, np;
    PROCESS *pp;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(tid);

    n = SC_array_get_n(ps->process_list);

    np = 0;
    for (i = 0; i < n; i++)
        {pp = *(PROCESS **) SC_array_get(ps->process_list, i);
	 if (SC_process_alive(pp) && (SC_status(pp) == SC_RUNNING))
	    {if (SC_process_status(pp) != SC_RUNNING)
	        np++;};};

    return(np);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CHECK_CHILDREN - check each running child on the managed list
 *                   - and update its run/exit status
 *                   - return the number of children who finished
 *                   - during this call
 */

int SC_check_children(void)
   {int it, np, nt;

    np = 0;

    nt = max(SC_n_threads, 1);
    for (it = 0; it < nt; it++)
        np += _SC_check_children(it);

    return(np);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RUNNING_CHILDREN - check each running child on the managed list
 *                      - and update its run/exit status
 *                      - return the number of children who
 *                      - are still running
 *                      - return -1 if there is a lost child
 */

static int _SC_running_children(int tid)
   {int i, n, nr, st;
    PROCESS *pp;
    SC_thread_proc *ps;

    ps = _SC_get_thr_processes(tid);

    n = SC_array_get_n(ps->process_list);

    nr = 0;
    for (i = 0; i < n; i++)
        {pp = *(PROCESS **) SC_array_get(ps->process_list, i);
	 if (SC_process_alive(pp))
	    {st = SC_process_status(pp);
	     if (st == 0)
	        nr++;
	     else if ((st & (SC_DEAD | SC_KILLED | SC_SIGNALED)) != 0)
	        {nr = -1;
		 break;};};};

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_RUNNING_CHILDREN - check the running children on the managed list
 *                     - of each thread
 *                     - and update its run/exit status
 *                     - return the number of children who
 *                     - are still running
 *                     - return -1 if there is a lost child
 */

int SC_running_children(void)
   {int it, n, nr, nt;

    nr = 0;

    nt = max(SC_n_threads, 1);
    for (it = 0; (it < nt) && (nr != -1); it++)
        {n = _SC_running_children(it);
	 if (n >= 0)
	    nr += n;
	 else
	    nr = -1;};

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_HANDLE_SIGCHLD - on receipt of a signal that a child status has
 *                   - changed record the new status and the reason for
 *                   - the change
 */

void SC_handle_sigchld(int signo)
   {

#ifdef HAVE_PROCESS_CONTROL

    int old_errno;

    old_errno = errno;

    errno = old_errno;

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

