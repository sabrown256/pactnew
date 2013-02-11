/*
 * SCSYSD.C - handle job pools for SC_exec_async
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "score_int.h"
#include "scope_proc.h"

/* ERROR Exit Statuses
 *   signo    - range   1 <= 63
 *   internal - range  64 <= 71    (defined in score.h)
 *   exec     - range  72 <= 79    (some in sclppsx.c)
 *   future   - range  80 <= 127
 *   errno    - range 128 <= 255
 */
#define SC_CONN_EXHAUSTED  77
#define SC_NO_CONN         78

#define SC_ESC_KEY      0x1b
#define SC_CTL_D        0x04
#define SC_EOF          0xff

/* NOTE: these macros will give leverage to fix
 * if a suspected race condition is confirmed
 */

#define GET_CONNECTION(cp, ic)                                                   \
   *(connectdes **) SC_array_get(cp->pool, ic)

#define GET_TASK(pt, it)                                                         \
   *(contask **) SC_array_get(pt, it)

typedef struct s_connectdes connectdes;
typedef struct s_contask contask;

struct s_connectdes
   {int n_barrier;
    int n_running;
    int n_launched;
    int n_complete;
    int n_timeout;            /* number of jobs which timed out */
    int n_env_sent;           /* number of environment variables sent */
    int n_env_ack;            /* number of environment variables acknowledged */
    int n_srv_jobs_max;       /* max number of jobs possible for server */
    int n_srv_jobs_lmt;       /* max number of jobs currently for server */
    int n_srv_running;
    int n_srv_complete;
    int n_srv_restart;        /* number of server self restarts */
    int n_srv_attempt;        /* number of attempts at server self restarts */
    int srv_started;          /* TRUE once server has been launched */
    int srv_done;             /* TRUE for normal server completion */
    int cpu_factor;           /* CPU number use factor */
    int cpu_acc;              /* CPU recruiting accelaration factor */
    double load;              /* load average for connection */
    double load_min;          /* minimum load average for connection */
    double load_max;          /* maximum load average for connection */
    double memory;            /* memory load for connection */
    double dt;                /* average time taken for all tasks in connection */
    double heartbeat_time;    /* time last hearbeat was received */
    double time_contact;      /* number of seconds to assume contact lost */
    int na;                   /* number of attempts to run any task */
    char *system;
    char *host;
    char *shell;
    char **env;
    PROCESS *pp;
    SC_array *taska;
    SC_array *log;};

struct s_contask
   {jobinfo inf;
    int retry;
    int ack;                  /* acknowledge receipt of command */
    int killed;               /* server has reported job killed */
    int group;                /* group according to barrier number */
    double dt;};

static void
 _SC_reconnect_pool(conpool *cp, int ic, char *msg, int recon);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_CONTASK - allocate and initialize a contask instance */

static contask *_SC_make_contask(int jid, int na,
				 char *shell, char *dir, char *cmnd,
				 connectdes *pco)
   {contask *pto;

    pto = CMAKE(contask);

    pto->inf.id        = jid;
    pto->inf.ia        = 0;
    pto->inf.na        = na;
    pto->inf.shell     = CSTRSAVE(shell);
    pto->inf.directory = CSTRSAVE(dir);
    pto->inf.full      = CSTRSAVE(cmnd);
    pto->inf.signal    = -1;
    pto->inf.status    = NOT_FINISHED;
    pto->inf.tstart    = SC_wall_clock_time();

    pto->ack    = FALSE;
    pto->retry  = 0;
    pto->killed = FALSE;
    pto->group  = pco->n_barrier;
    pto->dt     = -1.0;

    return(pto);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_CONTASK - release a contask instance */

static int _SC_free_contask(void *a)
   {contask *pto;

    if (a != NULL)
       {pto = *(contask **) a;
	if (pto != NULL)
	   {CFREE(pto->inf.shell);
	    CFREE(pto->inf.directory);
	    CFREE(pto->inf.full);
	    CFREE(pto);};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_INIT_CONNECTION - initialize connection contents
 *                    - client side routine
 */

static void SC_init_connection(connectdes *pc, int na, int fl)
   {int nsa, cpuf, cpua;
    char *s;
    double tlst;

/* use the specified fraction of available CPUs - default 1/4 */
    s = getenv("SC_EXEC_CPU_FACTOR");
    if (s != NULL)
       cpuf = SC_stoi(s);
    else
       cpuf = 4;

/* use the specified acceleration factor for recruiting CPUs - -1 all at once */
    s = getenv("SC_EXEC_CPU_RECRUIT");
    if (s != NULL)
       cpua = SC_stoi(s);
    else
       cpua = -1;

/* check for specified number of server restart attempts */
    s = getenv("SC_EXEC_SERVER_N_RESTART");
    if (s != NULL)
       nsa = SC_stoi(s);
    else
       nsa = 3;

/* check for specified number of seconds to lost contact
 * NOTE: cp->heartbeat should be a multiple of DEFAULT_HEARTBEAT so
 * we add an extra to have some leeway
 * also add 40 seconds which is the maximum time to wait before a retry
 */
    s = getenv("SC_EXEC_TIME_LOST");
    if (s != NULL)
       tlst = SC_stoi(s);
    else
       tlst = 2.0*DEFAULT_HEARTBEAT + 40.0;

/* client side info */
    pc->n_barrier      = 0;
    pc->n_running      = 0;
    pc->n_launched     = 0;
    pc->n_complete     = 0;
    pc->n_timeout      = 0;
    pc->n_env_sent     = 0;
    pc->heartbeat_time = -1.0;

/* server side info */
    pc->n_env_ack      = 0;
    pc->n_srv_jobs_max = 1;    /* initial value - server will report later */
    pc->n_srv_jobs_lmt = 1;    /* initial value - performance will change this later */
    pc->n_srv_running  = 0;
    pc->n_srv_complete = 0;
    pc->n_srv_restart  = 0;
    pc->n_srv_attempt  = nsa;
    pc->srv_done       = FALSE;
    pc->na             = na;
    pc->memory         = 0;
    pc->load           = -1;
    pc->load_min       =  HUGE;
    pc->load_max       = -HUGE;
    pc->dt             = -1.0;

    pc->system         = NULL;
    pc->host           = NULL;
    pc->shell          = NULL;
    pc->env            = NULL;
    pc->pp             = NULL;

    pc->cpu_factor     = cpuf;
    pc->cpu_acc        = cpua;
    pc->time_contact   = tlst;

    if (fl == TRUE)
       {pc->taska = CMAKE_ARRAY(contask *, NULL, 0);
	pc->log   = SC_STRING_ARRAY();
        SC_array_resize(pc->taska, 512, -1.0);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_CONNECTDES - allocate and initialize a connectdes instance */

static connectdes *_SC_make_connectdes(int na, char *sys,
				       char *hst, char *shell, char **env)
   {connectdes *pc;

    pc = CMAKE(connectdes);

    SC_init_connection(pc, na, TRUE);

    pc->system      = CSTRSAVE(sys);
    pc->host        = CSTRSAVE(hst);
    pc->shell       = CSTRSAVE(shell);
    pc->env         = env;
    pc->srv_started = FALSE;

    return(pc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_CONNECTDES - release a connectdes instance */

static void _SC_free_connectdes(connectdes *pc)
   {

/* set these as a marginal precaution in case another pointer exists
 * for this space - it can infer that the space has been freed
 * it will be useless if/when the space is recycled
 */
    pc->memory = 0;
    pc->load   = -1;
    pc->dt     = -1.0;
    pc->env    = NULL;

    CFREE(pc->system);
    CFREE(pc->host);
    CFREE(pc->shell);

/* free the tasks */
    SC_free_array(pc->taska, _SC_free_contask);
    pc->taska = NULL;

/* free the logs */
    SC_free_array(pc->log, SC_array_free_n);
    pc->log = NULL;

    CFREE(pc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_KILL_RUNAWAYS - if the file ~/kill-pool exists exit now */

void _SC_kill_runaways(void)
   {

    if (_SC.die == NULL)
       _SC.die = SC_dsnprintf(TRUE, "%s/kill-pool", getenv("HOME"));

    if (SC_isfile(_SC.die) == TRUE)
       exit(SC_EXIT_SELF);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_TAG - return the tag with LBL and time */

static void _SC_pool_tag(char *tag, int nc, char *lbl)
   {char *tms, *tm;

    tm  = SC_datef();
    tms = tm + 11;

    snprintf(tag, nc, "[%s %s]", lbl, tms);

    CFREE(tm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_LOG - log I/O for connection pools
 *              - client side routine
 */

static void _SC_pool_log(connectdes *pc, char *lbl, char *fmt, ...)
   {char tag[MAXLINE];
    char *txt, *msg;

    SC_VDSNPRINTF(TRUE, txt, fmt);

    _SC_pool_tag(tag, MAXLINE, lbl);
    msg = SC_dsnprintf(TRUE, "%s %s", tag, txt);

    if (SC_LAST_CHAR(msg) == '\n')
       SC_LAST_CHAR(msg) = '\0';

    SC_array_string_add(pc->log, msg);

    CFREE(txt);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_PRINTF - log I/O for connection pools
 *                 - and send a message to the output stream
 *                 - client side routine
 */

static void _SC_pool_printf(asyncstate *as, char *tag,
			    connectdes *pc, char *lbl, char *fmt, ...)
   {char *txt;

    SC_VDSNPRINTF(TRUE, txt, fmt);

    _SC_exec_printf(as, "%s %s\n", tag, txt);

    if (pc != NULL)
       _SC_pool_log(pc, lbl, txt);

    CFREE(txt);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SHOW_POOL_LOGS - print the connection logs
 *                   - client side routine
 */

void SC_show_pool_logs(conpool *cp, int n)
   {int ic, it, nc, nt;
    char *s;
    connectdes *pco;
    asyncstate *as;

    as = cp->as;

    nc = SC_array_get_n(cp->pool);
    for (ic = 0; ic < nc; ic++)
        {if ((n >= 0) && (n != ic))
	    continue;

	 pco = GET_CONNECTION(cp, ic);
	 if (pco == NULL)
	    continue;

	 _SC_exec_printf(as, "\n");
	 _SC_exec_printf(as,
			 "Connection #%d (%s)\n", ic, pco->host);
	 _SC_exec_printf(as,
			 "-------------------------------------------------\n");

	 nt = SC_array_get_n(pco->log);
	 for (it = 0; it < nt; it++)
	     {s = *(char **) SC_array_get(pco->log, it);
	      _SC_exec_printf(as, "%2d> %s\n", ic, s);};

	 _SC_exec_printf(as,
			 "Load Ave: %.2f  Ave Free Mem: %.2f  Ave Job Time: %.2f\n",
			 pco->load, pco->memory, pco->dt);

	 _SC_exec_printf(as,
			 "# Launched: %d  # Complete: %d  # Running: %d\n",
			 pco->n_launched, pco->n_complete, pco->n_running);};

    _SC_exec_printf(as, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SHOW_SERVER_LOGS - tell the remote server(s) to show its logs
 *                      - client side routine
 */

static void _SC_show_server_logs(conpool *cp, int n)
   {int ic, nc;
    char s[MAX_BFSZ];
    char *end;
    PROCESS *pp;
    connectdes *pco;

    end = SC_dsnprintf(TRUE, "%s %s\n", _SC_EXEC_SRV_ID, _SC_EXEC_LOG);

    nc = SC_array_get_n(cp->pool);
    for (ic = 0; ic < nc; ic++)
        {if ((n >= 0) && (n != ic))
	    continue;

	 pco = GET_CONNECTION(cp, ic);
	 if (pco == NULL)
	    continue;

	 pp = pco->pp;

	 io_printf(stdout, "Connection #%d (%s)\n", ic, pco->host);
	 io_printf(stdout, "-------------------------------------------------\n");

	 SC_printf(pp, "%s\n", _SC_EXEC_LOG);

         while (SC_process_status(pp) == SC_RUNNING)
	    {if (SC_gets(s, MAX_BFSZ, pp) != NULL)
	        {if (strcmp(s, end) == 0)
		    break;
		 else
		    io_printf(stdout, "%s", s);};};};

    CFREE(end);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_CONNECTION_CLOSE - close a pool connection process
 *                           - client side routine
 */

static void _SC_pool_connection_close(conpool *cp, int vrb, char *tag,
				      int ic)
   {int id;
    SC_evlpdes *pe;
    asyncstate *as;
    connectdes *pco;
    PROCESS *pp;

    as = cp->as;
    pe = cp->loop;

    pco = GET_CONNECTION(cp, ic);

    pp = pco->pp;
    if (SC_process_alive(pp))
       {id = (pp == NULL) ? -1 : pp->id;

	if (vrb == TRUE)
	   _SC_pool_printf(as, "***>", NULL, tag,
			   "close connection %d, %s, for %s - process %d (%d %d)",
			   ic, pco->host, tag, id, pp->status, pp->reason);

	SC_remove_event_loop_callback(pe, SC_PROCESS_I, pp);

	SC_close(pp);

	pco->pp = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_POOL_JOB - send the command specified by PT to
 *                   - the host of connection PC
 *                   - client side routine
 */

static void _SC_exec_pool_job(conpool *cp, int ic, contask *pt)
   {int jid;
    char *shell, *dir, *cmd;
    PROCESS *pp;
    connectdes *pco;
    jobinfo *inf;

    pco = GET_CONNECTION(cp, ic);

    pp    = pco->pp;
    inf   = &pt->inf;
    jid   = inf->id;
    shell = inf->shell;
    dir   = inf->directory;
    cmd   = inf->full;

    if (cmd[0] == '@')
       cmd++;

    _SC_setup_output(inf, "_SC_POOL_JOB");

/* document the number of running jobs at launch time */
    _SC_pool_log(pco, "client",
		 "at launch nr(%d/%d) nl(%d) nc(%d)\n",
		 pco->n_running, pco->n_srv_jobs_max,
		 pco->n_launched, pco->n_complete);

/* submit the request */
    if (dir == NULL)
       SC_printf(pp, "%s %d:%s:.:%s\n", _SC_EXEC_JOB, jid, shell, cmd);
    else
       SC_printf(pp, "%s %d:%s:%s:%s\n", _SC_EXEC_JOB, jid, shell, dir, cmd);
	
    inf->ia++;
    pco->n_running++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_JOB - add a task instance to the connection and
 *              - send the command CMND to be run by SHELL in DIR
 *              - by the host of connection PC
 *              - client side routine
 */

static void _SC_pool_job(conpool *cp, int ic, int na,
			 char *shell, char *dir, char *cmnd)
   {int jid;
    contask *pto;
    connectdes *pco;

    jid = cp->n_jobs++;

    pco = GET_CONNECTION(cp, ic);

    pto = _SC_make_contask(jid, na, shell, dir, cmnd, pco);

    SC_array_push(pco->taska, &pto);

    _SC_exec_pool_job(cp, ic, pto);

    pco->n_launched++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FIND_BEST_CONNECTION - find the best connection for the next job
 *                          - best is least loaded
 *                          - iff successful return the index of the
 *                          - best connection otherwise
 *                          - return -2 if all connections
 *                          - are fully subscribed
 *                          - client side routine
 */

static int _SC_find_best_connection(conpool *cp)
   {int ic, nc, il, ir, it, rv, nr, nrn, nmx;
    double ld, dt, le, ldn, dtn;
    connectdes *pco, *pcr;
#ifdef DEBUG
    char msgs[20][MAXLINE];
#endif

    pco = GET_CONNECTION(cp, 0);
    nmx = INT_MAX;

    nc = SC_array_get_n(cp->pool);
    if (nc < 2)
       {if (pco == NULL)
	   rv = -2;
	else
	   rv = (pco->n_running < pco->n_srv_jobs_max) ? 0 : -2;}

    else
       {il  = -1;
	ir  = -1;
	it  = -1;
	ldn = HUGE;
	nrn = INT_MAX;
	dtn = HUGE;

#ifdef DEBUG
	snprintf(msgs[0], MAXLINE, "  #          host   nr  lmt  max      dt    load");
	snprintf(msgs[1], MAXLINE, "------------------------------------------------");
#endif

	for (ic = 0; ic < nc; ic++)
	    {pco = GET_CONNECTION(cp, ic);
	     if (pco == NULL)
	        continue;

	     nr  = pco->n_running;
	     nmx = pco->n_srv_jobs_lmt;

#ifdef DEBUG
	     snprintf(msgs[2+ic], MAXLINE, "%3d  %12s  %3d  %3d  %3d  %6.2f  %6.2f",
		     ic, pco->host, nr, pco->n_srv_jobs_lmt,
		     pco->n_srv_jobs_max, pco->dt, pco->load);
#endif

	     if (nr >= nmx)
	        continue;

	     ld = pco->load;
	     dt = pco->dt;

/* use a synthetic load figure which is the actual load average
 * plus the number of jobs running
 */
	     le = ld + nr;

/* find the vote for the one with the smallest load figure */
	     if (le < ldn)
	        {il  = ic;
		 ldn = le;};
	 
/* find the vote for the one with the smallest number of running jobs */
	     if (nr < nrn)
	        {ir  = ic;
		 nrn = nr;};

/* find the vote for the one with the smallest dt */
	     if ((0.0 < dt) && (dt < dtn))
	        {it  = ic;
		 dtn = dt;};};

	rv = il;

/* if load does not pick a connection try number of running jobs */
	if (rv < 0)
	   rv = ir;

/* if number of running jobs does not pick a connection try dt */
	if (rv < 0)
	   rv = it;

/* if dt does not pick a connection try the first one available */
	if (rv < 0)
	   {for (rv = 0; rv < nc; rv++)
	        {pcr = GET_CONNECTION(cp, rv);
		 if ((pcr != NULL) && (pcr->n_running < nmx))
		    break;};
	    if (rv >= nc)
	       rv = -2;};};

#ifdef DEBUG
    if (rv >= 0)
       {snprintf(msgs[ic+2], MAXLINE, "Using connection %d - %s", rv, pc[rv]->host);
        for (ic = 0; ic < nc+3; ic++)
	    io_printf(stdout, "%s\n", msgs[ic]);};
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRINT_TASK_OUTPUT - now that a task has finished
 *                       - print its output nicely
 *                       - client side routine
 */

static void _SC_print_task_output(conpool *cp, int ic, contask *pt)
   {char *t, **out;
    connectdes *pco;
    asyncstate *as;
    jobinfo *inf;

    as = cp->as;

    pco = GET_CONNECTION(cp, ic);

    inf = &pt->inf;
    t   = inf->full;

    _SC_show_command(as, t, cp->show);

    out = _SC_array_string_join(&inf->out);

    _SC_print_filtered(as, out, cp->filter, -1, pco->host);

    SC_free_strings(out);

    _SC_pool_log(pco, "client",
		 "%s [job %2d] %d %d %.2f",
		 _SC_EXEC_COMPLETE, inf->id,
		 inf->signal, inf->status, pt->dt);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_JOB_DONE - this job is done
 *                   - do the output and status settting
 *                   - client side routine
 */

static void _SC_pool_job_done(conpool *cp, int ic, contask *pt,
			      int retry, int sig, int st, double dt)
   {int jc, nc, it, nt, lmt, nmx, alg;
    double dtl, dtx;
    connectdes *pcj, *pco;
    jobinfo *inf;

    pco = GET_CONNECTION(cp, ic);
    if (pco != NULL)
       {inf = &pt->inf;

	inf->signal = sig;
	inf->status = st;

	pt->dt    = dt;
	pt->retry = retry;

	_SC_print_task_output(cp, ic, pt);

        nmx = pco->n_srv_jobs_max;
	lmt = pco->n_srv_jobs_lmt;
	alg = pco->cpu_acc;

/* find out which connection is slowest on average */
	dtx = pco->dt;
	dtx = (dtx < 0.0) ? dt : dtx;
	dtx = min(dt, dtx);
	it  = ic;
	nt  = 1;

	nc = SC_array_get_n(cp->pool);
	for (jc = 0; jc < nc; jc++)
	    {pcj = GET_CONNECTION(cp, jc);
	     if (pcj != NULL)
	        {dtl = pcj->dt;
		 if (dtx < dtl)
		    {dtx = dtl;
		     it  = jc;};
		 if (0.0 < dtl)
		   nt++;};};

/* if you have only 1 connection just give it the works */
	if (nc < 2)
	   lmt = nmx;

/* decrement limit just for being the slowest on average */
	else if ((it == ic) && (nt > 1))
	   lmt--;

/* increase limit just for finishing the job */
/*    else */
	switch (alg)
	   {case 1 :
	         lmt++;
		 break;
	    case 2 :
		 lmt *= 2;
		 break;
	    default :
	         lmt = nmx;
		 break;};

	lmt = min(lmt, nmx);
	lmt = max(lmt, 1);

	pco->n_srv_jobs_lmt = lmt;
	pco->n_running--;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_EXEC_MSG_MATCH - check to see if S begins with PTN
 *                    - if so return a pointer to the part of S
 *                    - after PTN
 *                    - otherwise return NULL
 */

char *_SC_exec_msg_match(char *s, char *ptn)
   {int nc;
    char *r;

    r = NULL;
    if (s != NULL)
       {nc = strlen(ptn);
	if (strncmp(s, ptn, nc) == 0)
	   r = s + nc;};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_SERVER_OUTPUT - handle output P from the server in
 *                           - connection IC of pool CP
 *                           - client side routine
 */

static void _SC_process_server_output(conpool *cp, int ic, char *p)
   {int nr, nc, np, npu, nrp, ncp;
    int ifrac;
    double la, mm;
    char *r, *s, *t;
    connectdes *pco;

    pco = GET_CONNECTION(cp, ic);

    if (_SC_exec_msg_match(p, _SC_EXEC_ENV) != NULL)
       pco->n_env_ack++;

    else if ((r = _SC_exec_msg_match(p, _SC_EXEC_EXIT)) != NULL)
       {pco->srv_done = TRUE;}

    else if ((r = _SC_exec_msg_match(p, _SC_EXEC_NCPU)) != NULL)
       {t  = SC_strtok(r, " \t\n", s);
	np = SC_stof(t);

/* use specified fraction of CPUs */
	ifrac = pco->cpu_factor;
        if (ifrac > 0)
	   npu = np/ifrac;
	else
	   npu = -np*ifrac;
	npu = max(npu, 1);

	pco->n_srv_jobs_max = npu;

	_SC_pool_log(pco, "server", "%s cpus(%d) jobs(%d)",
		     _SC_EXEC_NCPU, np, npu);}

    else if ((r = _SC_exec_msg_match(p, _SC_EXEC_HEARTBEAT)) != NULL)
       {t  = SC_strtok(r, " \t\n", s);
	la = SC_stof(t);

	t  = SC_strtok(NULL, " \t\n", s);
	mm = SC_stof(t);

	t  = SC_strtok(NULL, " \t\n", s);
	nr = SC_stoi(t);

	t  = SC_strtok(NULL, " \t\n", s);
	nc = SC_stoi(t);

	t   = SC_strtok(NULL, " \t\n", s);
	nrp = SC_stoi(t);

	t   = SC_strtok(NULL, " \t\n", s);
	ncp = SC_stoi(t);

	pco->load           = la;
	pco->load_min       = min(pco->load_min, la);
	pco->load_max       = max(pco->load_max, la);
	pco->memory         = mm;
        pco->n_srv_running  = nr;
	pco->n_srv_complete = nc;
	pco->heartbeat_time = SC_wall_clock_time();

	_SC_pool_log(pco, "server", "%s %.2f %.2f run(%d) cmpl(%d)",
		     _SC_EXEC_HEARTBEAT, la, mm, nrp, ncp);}

    else if ((r = _SC_exec_msg_match(p, _SC_EXEC_RESTART)) != NULL)
       {_SC_pool_log(pco, "server", "%s %d",
		     _SC_EXEC_RESTART, pco->n_srv_restart);
	_SC_reconnect_pool(cp, ic, "restart", TRUE);}

    else if (p != NULL)
       _SC_pool_log(pco, "server", "%s", p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PROCESS_TASK_OUTPUT - handle output P from task IT in connection IC
 *                         - of pool CP
 *                         - client side routine
 */

static void _SC_process_task_output(asyncstate *as, conpool *cp,
				    int ic, int it, char *p)
   {int sg, st, ok;
    double dt;
    char *r, *t, *tag;
    jobinfo *inf;
    connectdes *pco;
    contask *pto;

    pco = GET_CONNECTION(cp, ic);
    pto = *(contask **) SC_array_get(pco->taska, it);

    inf = &pto->inf;

/* log all messages that come here
 * label them with a tag "[job <dd> <hh:mm:ss.ssssss>]"
 */
    tag = SC_dsnprintf(TRUE, "job %2d", inf->id);
    _SC_pool_log(pco, tag, "%s", p);
    CFREE(tag);

    if (pto->killed == FALSE)
       {r = _SC_exec_msg_match(p, _SC_EXEC_COMPLETE);

/* EXEC_COMPLETE looks like "--done-- 4 0 0.061"
 * comes from _SC_server_complete via job->exec
 */
	if (r != NULL)
	   {t  = strtok(r, " \n");
	    sg = SC_stoi(t);
	    t  = strtok(NULL, " \n");
	    st = SC_stoi(t);
	    t  = strtok(NULL, " \n");
	    dt = SC_stof(t);

	    ok = _SC_decide_retry(as, inf, NULL, st);

	    _SC_pool_job_done(cp, ic, pto, ok, sg, st, dt);

	    if (ok == 1)
	       _SC_exec_pool_job(cp, ic, pto);
	    else
	       pco->n_complete++;}

/* EXEC_JOB looks like "--job--- /bin/sh -c 'cd <dir> ; <cmd>'"
 * comes from _SC_server_job
 */
	else if (_SC_exec_msg_match(p, _SC_EXEC_JOB) != NULL)
	   pto->ack = TRUE;

/* EXEC_KILLED looks like "--term-- 4 0 0.061"
 * comes from _SC_server_complete via _SC_server_command
 */
	else if (_SC_exec_msg_match(p, _SC_EXEC_KILLED) != NULL)
	   pto->killed = TRUE;

/* OUTPUT from server controlled job looks like "<text>"
 * comes from _SC_server_complete via job->finish and _SC_print_filtered
 */
	else if (inf->out->array != NULL)
	   SC_array_string_add_copy(inf->out, p);
				  
/* any other non-empty output is unexpected and logged as such
 * in order to get a clue as to its origin
 */
	else if (p != NULL)
	   {if (strlen(p) > 0)
	       _SC_exec_printf(as,
			       "%s[%d] %s", pco->host, inf->id, p);
	    else
	       {_SC_pool_printf(as, "***>", pco, "client",
				"unexpected empty message from job %d (%s)",
				inf->id, pco->host);
		_SC_pool_printf(as, "***>", pco, "client",
				"task %d   connection %d",
				it, ic);
		_SC_pool_printf(as, "***>", pco, "client",
				"attempt(%d) status(%d) signal(%d) ack(%d)",
				inf->ia, inf->status, inf->signal, pto->ack);
		_SC_pool_printf(as, "***>", pco, "client",
				"command(%s : %s : %s)",
				inf->shell, inf->directory, inf->full);};}

/* any empty output is unexpected and logged as such
 * in order to get a clue as to its origin
 */
	else
	   {_SC_pool_printf(as, "***>", pco, "client",
			    "unexpected NULL message from job %d (%s)",
			    inf->id, pco->host);
	    _SC_pool_printf(as, "***>", pco, "client",
			    "task %d   connection %d",
			    it, ic);
	    _SC_pool_printf(as, "***>", pco, "client",
			    "attempt(%d) status(%d) signal(%d) ack(%d)",
			    inf->ia, inf->status, inf->signal, pto->ack);
	    _SC_pool_printf(as, "***>", pco, "client",
			    "command(%s : %s : %s)",
			    inf->shell, inf->directory, inf->full);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_ASSIGN_OTHERS - distribute the tasks of connection IC to
 *                        - all of the other connections
 */

static void _SC_pool_assign_others(conpool *cp, int ic)
   {int it, lc, nc, nt;
    connectdes *pcl, *pco;
    contask *pto;
    jobinfo *inf;
    asyncstate *as;

    as = cp->as;

    pco = GET_CONNECTION(cp, ic);

    nt = SC_array_get_n(pco->taska);

/* relaunch any job which is not finished to one of the other connections */
    nc = SC_array_get_n(cp->pool);                                               \
    for (lc = 0, it = 0; it < nt; it++, lc++)
        {lc %= nc;
	 if (lc == ic)
	    lc = (lc + 1) % nc;

	 pto = GET_TASK(pco->taska, it);
	 inf = &pto->inf;
	 if ((inf->status == NOT_FINISHED) && (pto->killed == FALSE))
	    {pcl = GET_CONNECTION(cp, lc);

	     _SC_pool_log(pco, "client",
			  "relaunch job %d to connection %d, %s, as %d: %s",
			  inf->id, lc, pcl->host,
			  SC_array_get_n(pco->taska), inf->full);

	     pto->killed = TRUE;
	     pco->n_complete++;
	     pco->n_running--;

	     _SC_pool_job(cp, lc, inf->na, inf->shell,
			  inf->directory, inf->full);};};

    _SC_pool_printf(as, "***>", pco, "client",
		    "reassigned tasks from %s to other connections\n",
		    pco->host);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_ASSIGN_CURRENT - relaunch the tasks of connection IC
 *                         - presumably following some sort of recovery
 */

static void _SC_pool_assign_current(conpool *cp, int ic)
   {int it, nt;
    contask *pto;
    connectdes *pco;
    jobinfo *inf;

    pco = GET_CONNECTION(cp, ic);

    nt = SC_array_get_n(pco->taska);

/* relaunch any job which is not finished */
    for (it = 0; it < nt; it++)
        {pto = GET_TASK(pco->taska, it);
	 inf = &pto->inf;
	 if ((inf->status == NOT_FINISHED) && (pto->killed == FALSE))
	    {_SC_pool_log(pco, "client", "relaunch job %d as %d: %s",
			  inf->id, SC_array_get_n(pco->taska), inf->full);

	     pto->killed = TRUE;
	     pco->n_complete++;
	     pco->n_running--;

	     _SC_pool_job(cp, ic, inf->na, inf->shell,
			  inf->directory, inf->full);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_WHICH_POOL - return the connection in CP whose file descriptor is FD
 *                - client side routine
 */

int _SC_which_pool(conpool *cp, int fd)
   {int ic, nc;
    PROCESS *pp;
    connectdes *pco;

    nc = SC_array_get_n(cp->pool);
    for (ic = 0; ic < nc; ic++)
        {pco = GET_CONNECTION(cp, ic);
         if (pco == NULL)
	    continue;

	 pp = pco->pp;
	 if (pp->io[0] == fd)
	    break;};

    if (ic >= nc)
       ic = -1;

    return(ic);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_CONNECTION - release connection contents
 *                    - close the PROCESS of the connection
 *                    - if SIG is TRUE signal the server to exit
 *                    - client side routine
 */

static void SC_free_connection(conpool *cp, int ic, int sig)
   {PROCESS *pp;
    connectdes *pco;

    pco = GET_CONNECTION(cp, ic);

    pp = pco->pp;
    if ((sig == TRUE) && (SC_status(pp) == SC_RUNNING))
       {SC_printf(pp, "%s\n", _SC_EXEC_EXIT);

/* give the child a chance to get the message */
	SC_sleep(100);};

    _SC_pool_connection_close(cp, FALSE, "free", ic);

    _SC_free_connectdes(pco);

    SC_array_set(cp->pool, ic, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_DELETE_POOL_CONNECTION - remove a connection from the pool
 *                            - when there is no way to start a server
 *                            - process for it
 *                            - client side routine
 */

static int _SC_delete_pool_connection(conpool *cp, int ic)
   {int i, nc;
    connectdes *pco;

    SC_free_connection(cp, ic, FALSE);

/* remove the freed connection from the list */
    nc = SC_array_get_n(cp->pool);
    for (i = 0; i < nc; i++)
        {pco = GET_CONNECTION(cp, i);
	 if (pco == NULL)
	    {nc = SC_array_remove(cp->pool, i);
	     break;};};

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REMOVE_CONNECTION - remove connection IC and give its work
 *                       - to the remaining connections
 *                       - return the number of remaining connections
 *                       - client side routine
 */

static int _SC_remove_connection(conpool *cp, int ic, int assgn)
   {int nc;

/* move jobs to other connections if requested */
    if (assgn == TRUE)
       _SC_pool_assign_others(cp, ic);

/* now we may delete the connection */
    nc = _SC_delete_pool_connection(cp, ic);

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REMOVE_CONNECTION_DUP - remove all connections
 *                           - whose hosts are duplicates
 *                           - return the number of remaining connections
 *                           - client side routine
 */

static int _SC_remove_connection_dup(conpool *cp)
   {int i, j, nc;
    char *ha, *hb;
    connectdes **pc;

    nc = SC_array_get_n(cp->pool);
    pc = SC_array_array(cp->pool);

    for (i = 0; i < nc; i++)
        {ha = pc[i]->host;

	 for (j = i+1; j < nc; j++)
	     {hb = pc[j]->host;
	      if (strcmp(ha, hb) == 0)
		 {nc = _SC_remove_connection(cp, j, FALSE);
		  j--;};};};

    CFREE(pc);

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_SHIFT_POOL_CONNECTION - shift the connection IC to another
 *                           - host if possible
 *                           - return TRUE if the number of connections
 *                           - was unchanged
 */

static int _SC_shift_pool_connection(conpool *cp, int ic)
   {int jh, lc, nc, nh, ok;
    char **hsts, *hst;
    connectdes *pcl, *pco;
    asyncstate *as;

    as = cp->as;

    pco = GET_CONNECTION(cp, ic);

/* NOTE: do NOT try the same host twice
 * _SC_launch_pool_task is looping over the number of hosts
 * and a repeat may invalidate that count if the host is truly unavailable
 */
    nh   = cp->n_hosts;
    hsts = cp->hosts;

/* remove the bad host from the list */
    for (jh = 0; jh < nh; jh++)
        {hst = hsts[jh];
	 if ((hst != NULL) && (strcmp(hst, pco->host) == 0))
	    {CFREE(hsts[jh]);
	     break;};};

/* check for a new host */
    nc = SC_array_get_n(cp->pool);
    for (jh = 0; jh < nh; jh++)
        {hst = hsts[jh];
	 if (hst != NULL)
	    {ok  = TRUE;
	     for (lc = 0; lc < nc; lc++)
	         {pcl = GET_CONNECTION(cp, lc);
		  if (strcmp(hst, pcl->host) == 0)
		     {ok = FALSE;
		      break;};};
	     if (ok == TRUE)
	        break;};};

    if (jh >= nh)
       {_SC_pool_printf(as, "***>", pco, "client",
			"retiring connection %d, %s (exhausted all %d %s hosts)",
			ic, pco->host, nh, pco->system);
	_SC_remove_connection(cp, ic, TRUE);

	ok = FALSE;}
	
    else
       {_SC_pool_printf(as, "***>", pco, "client",
			"re-opening connection %d from %s on %s",
			ic, pco->host, hst);

	CFREE(pco->host);
	pco->host = CSTRSAVE(hst);

	ok = TRUE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RECOVER_CONNECTION - attempt to recover and salvage the session
 *                        - when connection IC has died in some way
 */

static void _SC_recover_connection(conpool *cp, int ic)
   {int ok;
    connectdes *pco;
    asyncstate *as;
    PROCESS *pp;

    as = cp->as;

    pco = GET_CONNECTION(cp, ic);

    pp = pco->pp;

    if (SC_process_alive(pp))
       _SC_pool_connection_close(cp, TRUE, "reject", ic);

    _SC_pool_printf(as, "***>", NULL, "client", 
		    "recover connection %d", ic);

/* shift the connection to another host since we are having problems
 * with this one
 */
    ok = _SC_shift_pool_connection(cp, ic);

/* re-establish the server on the new host */
    if (ok == TRUE)
       _SC_reconnect_pool(cp, ic, "reject", FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_REJECTED - when a connection has broken I/O
 *                   - take appropriate action to resume the session
 */

static void _SC_pool_rejected(asyncstate *as, conpool *cp, int ic,
			      PROCESS *pp)
   {int st;
    connectdes *pco;

    pco = GET_CONNECTION(cp, ic);

    SC_sleep(1000);
    st = SC_process_status(pp);

    _SC_pool_printf(as, "***>", NULL, "client", 
		    "damaged server on connection %d, %s (%d) status (%d)",
		    ic, pco->host, pp->id, st);

    _SC_recover_connection(cp, ic);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_OUTPUT - handle output from a job in a connection pool
 *                 - store the output from the child in
 *                 - a dynamic array of strings derived from A
 *                 - client side routine
 */

static void _SC_pool_output(int fd, int mask, void *a)
   {int i, ic, it, nb, nc, ne, nr, nt;
    int jid, count;
    char s[MAX_BFSZ+10];
    char *p, *t, *u, *ps;
    PROCESS *pp;
    conpool *cp;
    contask *pto;
    connectdes *pco;
    asyncstate *as;

    as = NULL;

    if (_SC.idln == 0)
       _SC.idln = strlen(_SC_EXEC_SRV_ID);

    cp = (conpool *) a;

    nc = SC_array_get_n(cp->pool);
    ic = _SC_which_pool(cp, fd);

    if ((ic != -1) && (ic < nc))
       {pco = GET_CONNECTION(cp, ic);
	nt  = SC_array_get_n(pco->taska);
	pp  = pco->pp;
	nb  = pp->n_read;

	count = 0;
	while (SC_gets(s, MAX_BFSZ, pp) != NULL)
	   {if (s[0] == '<')
	       {p  = s + 1;
		t  = strchr(p, '>');
		if (t != NULL)
		   {*t = '\0';
		    ne = SC_stoi(p);
		    ps = t + 1;}
		else
		   {ps = s;
		    ne = strlen(ps);};
		nr = strlen(ps);
	        if (ne != nr)
                   {io_printf(stdout, "--> recv %d  expected %d\n", nr, ne);
		    p = ps + nr;
		    for (i = nr; i < ne; i++)
		        p[i] = '#';
		    p[ne]   = '\n';
		    p[ne+1] = '\0';
		    io_printf(stdout, "--> |%s|\n", ps);};}
	    else
	       ps = s;

/* handle messages about the server */
	    if (strncmp(ps, _SC_EXEC_SRV_ID, _SC.idln) == 0)
	       {p = ps + _SC.idln + 1;
		_SC_process_server_output(cp, ic, p);}

/* handle messages about jobs on the server */
	    else if (strncmp(ps, "[Job ", 5) == 0)
	       {p = strchr(ps, ']');
		if (p == NULL)
		   _SC_pool_log(pco, "bad", "      ^^^");
		else
		   {p += 3;
		    t  = SC_strtok(ps+5, " ", u);
		    jid = SC_stoi(t);
		    for (it = 0; it < nt; it++)
		        {pto = GET_TASK(pco->taska, it);
		         if (pto->inf.id == jid)
			    {_SC_process_task_output(as, cp, ic, it, p);
			     break;};};};}

/* simply log any other message */
	    else
	       _SC_pool_log(pco, "unknwn", "%s", ps);

	    s[0] = '\0';
	    count++;};

	nb = pp->n_read - nb;

/* since we came here off a poll the connection must be dead if we
 * read 0 bytes
 */
	if (nb == 0)
	   _SC_pool_rejected(as, cp, ic, pp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_REJECT - handle reject IO from a job in a connection pool
 *                 - client side routine
 */

static void _SC_pool_reject(int fd, int mask, void *a)
   {int ic, nc;
    PROCESS *pp;
    conpool *cp;
    connectdes *pco;
    asyncstate *as;

    as = NULL;
    cp = (conpool *) a;

    nc = SC_array_get_n(cp->pool);
    for (ic = 0; ic < nc; ic++)
        {pco = GET_CONNECTION(cp, ic);
         if (pco == NULL)
	    continue;

	 pp = pco->pp;
	 if (SC_process_alive(pp) && (pp->io[0] == fd))
            {_SC_pool_rejected(as, cp, ic, pp);
	     break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PULSE_SERVERS - let the servers know that the client is still here
 *                   - this keeps them from timing out
 */

static void _SC_pulse_servers(conpool *cp)
   {int ic, nc, ok;
    PROCESS *pp;
    connectdes *pco;

    nc = SC_array_get_n(cp->pool);
    for (ic = 0; ic < nc; ic++)
        {pco = GET_CONNECTION(cp, ic);
         if (pco == NULL)
	    continue;

	 pp = pco->pp;

	 if (pp != NULL)
	    {ok = SC_printf(pp, "%s\n", _SC_EXEC_HEARTBEAT);
	     if (ok == FALSE)
	        _SC_recover_connection(cp, ic);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_OPEN_CONNECTION_POOL - open a pool of N connections to remote hosts
 *                         - of system type SYS
 *                         - so that work can be handed out to them
 *                         - under the specified SHELL
 *                         - client side routine
 */

conpool *SC_open_connection_pool(int n, char *sys, char *shell, char **env,
				 int na, int show, int ignore,
				 int th, fspec *filter)
   {int i, nh;
    char hst[MAXLINE];
    char **hsts;
    conpool *cp;
    connectdes *pc;

/* determine all possible hosts */
    if (n < 2)
       {nh   = 1;
	hsts = CMAKE_N(char *, nh);
	SC_hostname(hst, MAXLINE);
	hsts[0] = CSTRSAVE(hst);}

    else
       {nh   = SC_get_nhosts(sys);
	hsts = CMAKE_N(char *, nh);
	for (i = 0; i < nh; i++)
	    {if (n == 1)
	        SC_hostname(hst, MAXLINE);
	     else
	        SC_get_host_name(hst, MAXLINE, sys);

	     hsts[i] = CSTRSAVE(hst);};};

    n  = max(n, 1);
    cp = CMAKE(conpool);

/* take NFS stats before as a reference */
    SC_nfs_monitor(cp->ref_net, 8);

/*    SC_gs.io_interrupts_on = TRUE; */

    cp->as        = NULL;
    cp->show      = show;
    cp->ignore    = ignore;
    cp->na        = na;
    cp->n_jobs    = 0;
    cp->n_hosts   = nh;
    cp->hosts     = hsts;
    cp->loop      = SC_make_event_loop(NULL, NULL, NULL, -1, -1, -1);
    cp->heartbeat = th;
    cp->filter    = filter;

    cp->pool = CMAKE_ARRAY(connectdes *, NULL, 0);
    SC_array_resize(cp->pool, 512, -1.0);

    shell = SC_get_shell(shell);

    for (i = 0; i < n; i++)
        {pc = _SC_make_connectdes(na, sys, hsts[i], shell, env);

	 SC_array_push(cp->pool, &pc);};

/* weed out duplicate hosts so that there is only one connection
 * per host
 */
    _SC_remove_connection_dup(cp);

    return(cp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SHOW_POOL_STATS - report the statistics for all connections in pool CP
 *                    - client side routine
 */

void SC_show_pool_stats(conpool *cp, int n, int full)
   {int ic, it, nc, nt, st;
    connectdes *pco;
    contask *pto;
    jobinfo *inf;
    asyncstate *as;

    as = cp->as;

    nc = SC_array_get_n(cp->pool);
    for (ic = 0; ic < nc; ic++)
        {if ((n >= 0) && (n != ic))
	    continue;

	 pco = GET_CONNECTION(cp, ic);
	 if (pco == NULL)
	    continue;

	 nt = SC_array_get_n(pco->taska);

	 _SC_exec_printf(as, "\n");
	 _SC_exec_printf(as,
			 "Connection #%d on %s (max jobs %d) (clnt %d/%d jobs) (srv %d/%d jobs)\n",
			 ic, pco->host, pco->n_srv_jobs_max, pco->n_complete, pco->n_launched,
			 pco->n_srv_complete, pco->n_srv_running);
	 _SC_exec_printf(as,
			 "--------------------------------------------------------\n");

	 if (full == TRUE)
	    {_SC_exec_printf(as,
			     "     ID Grp Ack Stat    Time   Command\n");
	     for (it = 0; it < nt; it++)
	         {pto = GET_TASK(pco->taska, it);
		  inf = &pto->inf;
		  st  = inf->status;
		  if (pto->killed == TRUE)
		     _SC_exec_printf(as,
				     "   %4d %3d   x   -reissued-   %s\n",
				     inf->id, pto->group, inf->full);
		  else if (st == NOT_FINISHED)
		     _SC_exec_printf(as,
				     "   %4d %3d  %2d  ---     ---   %s\n",
				     inf->id, pto->group, pto->ack, inf->full);
		  else if (st == REISSUED)
		     _SC_exec_printf(as,
				     "   %4d %3d   x  ---     ---   %s\n",
				     inf->id, pto->group, inf->full);
		  else
		     _SC_exec_printf(as,
				     "   %4d %3d  %2d %4d %7.2f   %s\n",
				     inf->id, pto->group, pto->ack, inf->status,
				     pto->dt, inf->full);};};

/* load average info */
	 if (pco->load != -1.0)
	    {_SC_exec_printf(as,
			     "   Load Ave: %.2f <= %.2f <= %.2f",
			     pco->load_min, pco->load, pco->load_max);
	     _SC_exec_printf(as,
			     "  Free Memory: %6.2f%%", 100.0*pco->memory);
	     _SC_exec_printf(as,
			     "  Ave Job Time: %6.2f\n", pco->dt);}
	 else
	    _SC_exec_printf(as,
			    "   No load, memory, or job info yet\n");

	 _SC_exec_printf(as,
			 "   # Env sent: %d   # Env set: %d\n",
			 pco->n_env_sent, pco->n_env_ack);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SHOW_POOL_UNF - report the unfinished jobs for all connections
 *                  - in pool CP
 *                  - client side routine
 */

void SC_show_pool_unf(conpool *cp, int n, int full)
   {int ic, it, nc, nt, nu, st;
    connectdes *pco;
    contask *pto;
    jobinfo *inf;
    asyncstate *as;

    as = cp->as;

    nc = SC_array_get_n(cp->pool);
    for (ic = 0; ic < nc; ic++)
        {if ((n >= 0) && (n != ic))
	    continue;

	 pco = GET_CONNECTION(cp, ic);
	 if (pco == NULL)
	    continue;

	 nt = SC_array_get_n(pco->taska);
	 nu = 0;
	 for (it = 0; it < nt; it++)
	     {pto = GET_TASK(pco->taska, it);
	      inf = &pto->inf;
	      st  = inf->status;
	      nu += ((pto->killed == TRUE) ||
		     (st == NOT_FINISHED) ||
		     (st == REISSUED));};

	 if (nu != 0)
	    {_SC_exec_printf(as, "\n");
	     _SC_exec_printf(as,
			     "Connection #%d on %s (clnt %d/%d jobs) (srv %d/%d jobs)\n",
			     ic, pco->host, pco->n_complete, pco->n_launched,
			     pco->n_srv_complete, pco->n_srv_running);
	     _SC_exec_printf(as,
			     "--------------------------------------------------------\n");

	     _SC_exec_printf(as,
			     "     ID Grp Ack  Status        Command\n");
	     for (it = 0; it < nt; it++)
	         {pto = GET_TASK(pco->taska, it);
		  inf = &pto->inf;
		  st  = inf->status;
		  if (pto->killed == TRUE)
		     _SC_exec_printf(as,
				     "   %4d %3d   x   -killed-     %s\n",
				     inf->id, pto->group, inf->full);
		  else if (st == NOT_FINISHED)
		     {_SC_exec_printf(as,
				      "   %4d %3d  %2d  -not done-    %s\n",
				      inf->id, pto->group, pto->ack, inf->full);
		      _SC_exec_printf(as,
				      "                               %d/%d   %d/%d %f %s\n",
				      inf->ia, inf->na, inf->signal, inf->status,
				      inf->tstart, inf->stop_time);}
		  else if (st == REISSUED)
		     _SC_exec_printf(as,
				     "   %4d %3d   x   -reissued-   %s\n",
				     inf->id, pto->group, inf->full);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_POOL_CONNECTION_ENV - send the set of environment variables
 *                         - to the connection PC
 *                         - client side routine
 */

static void _SC_pool_connection_env(connectdes *pc)
   {int i, ne;
    char pth[MAXLINE];
    char **env, *s, *t, *ce;
    PROCESS *pp;

    pp  = pc->pp;
    env = pc->env;

/* get the current dmake to be the one used on remote hosts
 * to do this send the PATH that we need
 */
    strcpy(pth, pc->shell);
    t = strpbrk(pth, " \t");
    if (t != NULL)
       *t = '\0';

    SC_pop_path(pth);

    ce = getenv("PATH");
    t  = SC_dsnprintf(TRUE, "PATH=%s:%s", pth, ce);
    SC_printf(pp, "%s %s\n", _SC_EXEC_ENV, t);
    if (_SC.log_env == TRUE)
       _SC_pool_log(pc, "client", "%s send %s", _SC_EXEC_ENV, t);

    CFREE(t);

    ne = 1;

/* send the current environment to the connection */
    if (env != NULL)
       {for (i = 0; TRUE; i++)
	    {s = env[i];
	     if (s == NULL)
	        break;

/* handle PATH specially */
	     if (strncmp(s, "PATH=", 5) != 0)
	        {SC_printf(pp, "%s %s\n", _SC_EXEC_ENV, s);
		 if (_SC.log_env == TRUE)
		    _SC_pool_log(pc, "client", "%s send %s", _SC_EXEC_ENV, s);
		 ne++;};};};

    pc->n_env_sent = ne;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LAUNCH_POOL_CONNECTION - start the SSH session of the connection PC
 *                            - in the pool CP
 *                            - just start a shell if the connection host
 *                            - is the same as the current host
 *                            - client side routine
 */

static int _SC_launch_pool_connection(conpool *cp, int ic)
   {int pi, st;
    int est, sgn, pta, nms;
    char s[MAXLINE];
    char **env;
    PROCESS *pp;
    connectdes *pco;
    asyncstate *as;
    SC_evlpdes *pe;

    as = cp->as;

    pco = GET_CONNECTION(cp, ic);
    env = pco->env;

    pp = SC_open_remote(pco->host, pco->shell, NULL, env, "ap", NULL);
    if (SC_process_alive(pp))
       {_SC_pool_log(pco, "client",
		     "connected %d to %s (%d)",
		     ic, pco->host, pp->id);

	pco->srv_started = TRUE;
	pco->pp = pp;
	SC_unblock(pp);
	SC_set_attr(pp, SC_LINE, TRUE);

	pe = cp->loop;
	pi = SC_register_event_loop_callback(pe, SC_PROCESS_I, pp,
					     _SC_pool_output,
					     _SC_pool_reject,
					     -1);
	SC_ASSERT(pi >= 0);

	_SC_pool_connection_env(pco);}

    else
       {st = -(10 + SC_gs.errn)/1000000;
        sgn = (st / 10000) % 100;
        pta = (st / 100) % 100;
        nms = st % 100;
        est = st / 1000000;

	pco->srv_started = FALSE;

	_SC_pool_printf(as, "***>", NULL, "client", 
			"failed to launch connection %d on %s (status=%d  signal=%d  %%time=%d  Nmsg=%d)",
			ic, pco->host, est, sgn, pta, nms);
	if (st > 0)
	   {SC_error_explanation(st, s, MAXLINE);
	    _SC_pool_printf(as, "***>", NULL, "client", 
			    "%s", s);};
	_SC_shift_pool_connection(cp, ic);
	ic = -1;};

    return(ic);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHECK_POOL_CONNECTION - check the best available connection of CP and
 *                           - try to establish a running PROCESS there
 *                           - return -2 if all connections
 *                           - are fully subscribed
 *                           - return -1 iff the connection cannot sustain
 *                           - a process otherwise the connection index IC
 *                           - client side routine
 */

static int _SC_check_pool_connection(conpool *cp, int ic)
   {PROCESS *pp;
    connectdes *pco;

    if (ic < 0)
       ic = _SC_find_best_connection(cp);

    if (ic >= 0)
       {pco = GET_CONNECTION(cp, ic);
	pp  = pco->pp;

/* if there is no connection launch one */
	if (!SC_process_alive(pp))
	   {ic = _SC_launch_pool_connection(cp, ic);
	    if (ic >= 0)
	       {pco = GET_CONNECTION(cp, ic);
	        pp  = pco->pp;
		_SC_pool_log(pco, "client",
			     "launch connection %d, %s (%d)",
			     ic, pco->host, pp->id);};};

/* if the connection died off re-establish it */
	if (SC_process_alive(pp) && (SC_status(pp) != SC_RUNNING))
	   {_SC_pool_connection_close(cp, FALSE, "check", ic);

	    ic = _SC_launch_pool_connection(cp, ic);
	    if (ic >= 0)
	       {pco = GET_CONNECTION(cp, ic);
	        pp  = pco->pp;
		_SC_pool_log(pco, "client",
			     "relaunch connection %d, %s (%d)",
			     ic, pco->host, pp->id);};};};

    return(ic);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RECONNECT_POOL - restart connection and relaunch
 *                    - unfinished tasks from the connection
 *                    - client side routine
 */

static void _SC_reconnect_pool(conpool *cp, int ic, char *msg, int recon)
   {int ico;
    char *hno;
    PROCESS *pp;
    connectdes *pco;
    asyncstate *as;

    as = cp->as;

    pco = GET_CONNECTION(cp, ic);
    pco->n_env_sent = 0;
    pco->n_env_ack  = 0;

/* only try this the specified number of times */
    pco->n_srv_restart++;
    if (pco->n_srv_restart >= pco->n_srv_attempt)
       {_SC_pool_log(pco, "client", "reconnect attempts exhausted %d/%d\n",
		     pco->n_srv_restart, pco->n_srv_attempt);
        LONGJMP(cp->cpu, SC_CONN_EXHAUSTED);};

    ico = ic;
    hno = pco->host;
    pp  = pco->pp;
    if ((recon == TRUE) || (SC_status(pp) != SC_RUNNING))
       {_SC_pool_connection_close(cp, TRUE, msg, ic);

        ic = _SC_check_pool_connection(cp, ic);
        if (ic < 0)
	   {_SC_pool_log(pco, "client",
			 "no host to re-open connection - %d, %s\n",
			 ico, hno);
	    LONGJMP(cp->cpu, SC_NO_CONN);}
	else
	   {pco = GET_CONNECTION(cp, ic);
	    _SC_pool_printf(as, "***>", pco, "client",
			    "connection %d re-opened on %s (%d)",
			    ic, pco->host, pco->pp->id);};}
    else
       _SC_pool_log(pco, "client",
		    "no reconnection necessary for %d, %s (%d %d %d)",
		    ic, hno, recon, pp->status, pp->id);

    _SC_pool_assign_current(cp, ic);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LAUNCH_POOL_TASK - launch a job CMND in directory DIR
 *                      - in the connection pool CP
 *                      - a word about shells:
 *                      -    there is the connection shell and
 *                      -    there is the command SHELL
 *                      - they do not have to be the same
 *                      - and are not in general
 *                      - return -1 if a connection cannot be found
 *                      - with a running server process
 *                      - client side routine
 */

int _SC_launch_pool_task(conpool *cp, int na, int reset,
			 char *shell, char *dir, char *cmnd)
   {int ic, ih, nc, nh, rv;
    connectdes *pco;
    asyncstate *as;

    as = cp->as;

    nc = SC_array_get_n(cp->pool);
    nh = cp->n_hosts;

/* reset the average time for a job in each connection
 * after a barrier has been reached
 * also reset job counters
 */
    if (reset == TRUE)
       {for (ic = 0; ic < nc; ic++)
	    {pco = GET_CONNECTION(cp, ic);
	     if (pco == NULL)
	        continue;

	     pco->n_barrier++;
	     pco->dt         = -1.0;
/* GOTCHA: do we want this for proper accounting?
	     pco->n_running  = 0;
 */
	     pco->n_complete = 0;
	     pco->n_launched = 0;
	     _SC_pool_log(pco, "client", "reset connection %d at barrier",
			  ic);};};

/* look through no more than the number of hosts available for the
 * system type to make a connection
 */
    for (ih = 0; ih < nh; ih++)

/* launch the job we came here to do or
 * try again to find a better connection
 */
        {ic = _SC_check_pool_connection(cp, -1);
	 if (ic == -2)
	    break;
	 if (ic >= 0)
	    {_SC_pool_job(cp, ic, na, shell, dir, cmnd);
	     break;};};

/* there is no host up at all to do any work - just die now */
    if (ih >= nh)
       {_SC_pool_printf(as, "***>", NULL, "client",
			"failed to launch '%s' (%d conns/%d hosts)\n",
			cmnd, nc, nh);
	rv = -2;}

/* there is no host available to do any work - wait until later */
    else if (ic < 0)
       rv = -1;

/* we found a host for connection ic and the job is launched */
    else
       rv = ic;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LAUNCH_POOL_JOB - launch a job CMND in directory DIR
 *                    - in the connection pool CP
 *                    - if a job cannot be launched immediately wait
 *                    - (mainly until other jobs finish)
 *                    - then try again
 *                    - return -1 if there is no way to launch the job
 *                    - client side routine
 */

int SC_launch_pool_job(conpool *cp, int na, int reset,
		       char *shell, char *dir, char *cmnd)
   {int ncn, rv;
    asyncstate *as;

    as = cp->as;
    rv = -1;
    while (TRUE)

/* depending on faults NCN could change
 * so get the value each time through the loop
 */
       {ncn = SC_array_get_n(cp->pool);

/* might as well stop if you have no connections */
	if (ncn < 1)
	   {_SC_pool_printf(as, "***>", NULL, "client",
			    "no connections left in pool");
	    break;};

	rv = _SC_launch_pool_task(cp, na, reset, shell, dir, cmnd);
	if (rv == -1)
	   SC_wait_pool_job(cp, 1000);
	else if (rv == -2)
	   {rv = -1;
	    break;}
	else
	   break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_CHECK_JOB_TIME - watch out for jobs that need to be resubmitted
 *                    - client side routine
 */

static void _SC_check_job_time(conpool *cp)
   {int ic, nc, ncmp;
    double dt, tf, ndt, thl;
    double dta, dtamn, dtamx;
    connectdes *pco;
    PROCESS *pp;
    asyncstate *as;

    as = cp->as;

    nc = SC_array_get_n(cp->pool);
    tf = SC_wall_clock_time();

/* check each connection for timeouts (lost contact with server) */
    for (ic = 0; ic < nc; ic++)
        {pco = GET_CONNECTION(cp, ic);
	 if ((pco == NULL) || (pco->srv_started == FALSE))
	    continue;

	 thl = pco->time_contact;
	 dt  = tf - pco->heartbeat_time;
	 if (dt > thl)
	    {pco->heartbeat_time = tf;

	     pp = pco->pp;
	     if (pp == NULL)
	        _SC_pool_printf(as, "\n***>", pco, "client",
				"lost contact with connection %d, %s, (gone) after %.2f seconds",
				ic, pco->host, dt);
	     else
	        _SC_pool_printf(as, "\n***>", pco, "client",
				"lost contact with connection %d, %s, (%d %d) after %.2f seconds",
				ic, pco->host, pco->pp->id, pco->pp->status, dt);
	     _SC_reconnect_pool(cp, ic, "lost heartbeat", TRUE);};};

/* compute the average time taken in all connections */
    ncmp  = 0;
    ndt   = 0.0;
    dta   = 0.0;
    dtamn = HUGE;
    dtamx = 0.0;
    for (ic = 0; ic < nc; ic++)
        {pco = GET_CONNECTION(cp, ic);
	 if (pco == NULL)
	    continue;

	 dt = pco->dt;
         if (dt > 0.0)
	    {dta  += dt;
	     ndt  += 1.0;
	     dtamn = min(dt, dtamn);
	     dtamx = max(dt, dtamx);};

	 ncmp += pco->n_complete;};

    if (ndt > 0.0)
       dta /= ndt;

    if (dta <= 0.0)
       ncmp = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_WAIT_POOL_JOB - wait upto TO milliseconds for a
 *                  - job in the connection pool CP to finish
 *                  - return
 *                  -   -1 error in connection pool
 *                  -    0 if no connections are idle
 *                  -    1 if some connections are idle
 *                  -    2 if all connections are idle
 *                  - client side routine
 */

int SC_wait_pool_job(conpool *cp, int to)
   {int ic, it, nc, ng, nr, nrt, nfr, nt, rv, err, blck;
    char key[MAXLINE];
    char *p;
    double dt, ndt, dta;
    SC_evlpdes *pe;
    connectdes *pco;
    contask *pto;

/* kill runaways */
    _SC_kill_runaways();

/* send heartbeat to servers */
    _SC_pulse_servers(cp);

    pe = cp->loop;

/* check for command to enter interactive loop - Alt-C<CR> */
    blck = SC_isblocked_file(stdin);
    if (blck == TRUE)
       {SC_unblock_file(stdin);
	p = io_gets(key, MAXLINE, stdin);
        SC_block_file(stdin);}
    else
       p = io_gets(key, MAXLINE, stdin);
	  
    if (p != NULL)
       {if ((key[0] == SC_ESC_KEY) && (key[1] == 'c'))
	   {rv = SC_query_connection_pool(cp);
	    if (rv == TRUE)
	       SC_signal_async(SIGINT);};};

/* check the status of all the connection processes */
    nr = SC_running_children();

/* wait for commands to complete */
    err = SC_event_loop_poll(pe, cp, to);
    if (err < 0)
       return(-1);

/* compute average job info for each connection
 * to be used in assigning subsequent jobs to connections
 */
    nc = SC_array_get_n(cp->pool);
    if (nc < 1)
       return(-1);

    nfr = 0;
    nrt = 0;
    for (ic = 0; ic < nc; ic++)
        {pco = GET_CONNECTION(cp, ic);
	 if (pco == NULL)
	    continue;

	 nt = SC_array_get_n(pco->taska);

/* compute the average time taken by a task in the connection
 * in the current barrier group
 */
	 ng  = pco->n_barrier;
	 ndt = 0.0;
	 dta = 0.0;
	 for (it = 0; it < nt; it++)
	     {pto = GET_TASK(pco->taska, it);

	      dt = pto->dt;
	      if ((dt > 0.0) && (pto->group == ng))
		 {dta += dt;
		  ndt += 1.0;};};

	 if (ndt > 0.0)
	    dta /= ndt;

	 if (dta > 0.0)
	    pco->dt = dta;

/* compute the total number of running jobs and
 * the number of free connections
 */
	 nr = pco->n_running;

	 nrt += nr; 
	 nfr += (nr == 0);};

    if (nfr == 0)
       rv = 0;
    else if (nfr == nc)
       rv = 2;
    else
       rv = 1;

/* see if any job should be considered to have timed out */
    _SC_check_job_time(cp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CONNECTION_POOL_NET - take a measure of network activity
 *                        - since the pool started
 */

int SC_connection_pool_net(conpool *cp)
   {int i, st;
    int *a, *b, d[8];
    double ac, bc, dc, dnmo, dnmr;
    double pd[8];
    asyncstate *as;

    as = cp->as;

    b = cp->ref_net;
    a = cp->net;

    st = SC_nfs_monitor(cp->net, 8);
    if (st == TRUE)
       {for (i = 0; i < 8; i++)
	    {ac = a[i];
	     bc = b[i];
	     dc = ac - bc;

	     d[i] = max(dc, 1.0);};

	dnmr = 100.0/d[0];
	dnmo = 100.0/d[7];

	pd[1] = dnmr*d[1];
	for (i = 2; i < 7; i++)
	    pd[i] = dnmo*d[i];

	_SC_exec_printf(as, "\n");
	_SC_exec_printf(as, "NFS Traffic Summary\n");
	_SC_exec_printf(as, "-------------------\n");

	_SC_exec_printf(as, "     # RPC    Retrans      # Ops       Read");
	_SC_exec_printf(as, "      Write     Lookup    Getattr    Setattr\n");

/* increments */
	_SC_exec_printf(as,
			"%10d %10d %10d %10d %10d %10d %10d %10d\n",
			d[0], d[1], d[7], d[2], d[3], d[4], d[5], d[6]);

/* percent differences */
	_SC_exec_printf(as,
			"           %9.2f%%            %9.2f%% %9.2f%% %9.2f%% %9.2f%% %9.2f%%\n",
			pd[1], pd[2], pd[3], pd[4], pd[5], pd[6]);};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CONNECTION_POOL_STATUS - return the number of jobs in
 *                           - the connection pool with 
 *                           - non-zero exit status
 *                           - returns zero iff all succeed
 *                           - client side routine
 */

int SC_connection_pool_status(conpool *cp)
   {int ic, it, nc, nt;
    int st, stc;
    connectdes *pco;
    contask *pto;
    jobinfo *inf;

    nc = SC_array_get_n(cp->pool);
    st = 0;
    for (ic = 0; ic < nc; ic++)
        {pco = GET_CONNECTION(cp, ic);
	 if (pco == NULL)
	    continue;

	 nt = SC_array_get_n(pco->taska);

	 for (it = 0; it < nt; it++)
	     {pto = GET_TASK(pco->taska, it);
	      inf = &pto->inf;
	      if ((pto->killed != TRUE) && (pto->retry != 1))
		 {stc = inf->status;
		  st += (stc != 0);};};};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CONNECTION_POOL_ANOMALIES - return TRUE iff there are any
 *                              - any anomalies in the pool CP
 *                              - anomalies include:
 *                              -   any running children
 *                              -   n_launched != n_complete
 *                              -   n_killed != 0
 *                              -   signal == SC_EXIT_BAD
 */

int SC_connection_pool_anomalies(conpool *cp)
   {int ic, it, nc, nt, na;
    int st;
    connectdes *pco;
    contask *pto;
    jobinfo *inf;

    nc = SC_array_get_n(cp->pool);
    na = 0;

    for (ic = 0; ic < nc; ic++)
        {pco = GET_CONNECTION(cp, ic);
	 if (pco == NULL)
	    continue;

	 na += (pco->n_launched != pco->n_complete);

	 nt = SC_array_get_n(pco->taska);

	 for (it = 0; it < nt; it++)
	     {pto = GET_TASK(pco->taska, it);
	      inf = &pto->inf;

	      na += (inf->signal == SC_EXIT_BAD);
	      na += (pto->killed != 0);};};

    st = (na != 0);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CLOSE_CONNECTION_POOL - close a pool of connections to remote hosts
 *                          - if LOG is not 0 or SIGINT print the logs of the
 *                          - connections
 *                          - client side routine
 */

void SC_close_connection_pool(conpool *cp, int log, int sum)
   {int ic, ih, nc, nh, nr;
    char **hsts, *logf;
    asyncstate *as;

    as = cp->as;

/* if log file requested by environment variable only use it */
    logf = getenv("SC_EXEC_LOG_FILE");
    if ((logf != NULL) && (as->log == NULL))
       as->log = io_open(logf, "w");

/* if there are any anomalies then force log info out */
    if (SC_connection_pool_anomalies(cp) == TRUE)
       log = -1;

/* decide about full log info */
    if ((log != 0) && (log != SIGINT))
       {SC_show_pool_stats(cp, -1, TRUE);
	SC_show_pool_logs(cp, -1);
        if (log == -1)
	   _SC_show_server_logs(cp, -1);
	SC_connection_pool_net(cp);}

/* decide about summary log info */
    else if (sum != 0)
       {SC_show_pool_stats(cp, -1, FALSE);
	SC_connection_pool_net(cp);};

    nc = SC_array_get_n(cp->pool);

    for (ic = 0; ic < nc; ic++)
        SC_free_connection(cp, ic, TRUE);

/* check running children - should be none by now */
    nr = SC_running_children();
    if (nr != 0)
       {int ov;

	ov            = as->to_stdout;
	as->to_stdout = TRUE;

        _SC_exec_printf(as, "\n");
	_SC_exec_printf(as, "# running children = %d\n", nr);
        _SC_exec_printf(as, "\n");

        as->to_stdout = ov;};

/* close a log file */
    if ((as->log != NULL) && (as->log != stdout))
       io_close(as->log);

    SC_free_event_loop(cp->loop);

    SC_host_server_fin();

    SC_free_array(cp->pool, NULL);

    nh   = cp->n_hosts;
    hsts = cp->hosts;
    for (ih = 0; ih < nh; ih++)
        {CFREE(hsts[ih]);};
    CFREE(hsts);

    CFREE(cp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_QUERY_CONNECTION_POOL - respond to queries about the connection pool
 *                          - return TRUE to have caller exit session
 *                          - return FALSE to have caller resume session
 *                          - client side routine
 */

int SC_query_connection_pool(conpool *cp)
   {int n, ic, nc, st, c;
    char s[MAXLINE];
    char *ps, *p, *cmd, *val;
    connectdes *pco;

    st = FALSE;

    SC_setbuf(stdout, NULL);
    while (TRUE)
       {io_printf(stdout, "-> ");
	ps = io_gets(s, MAXLINE, stdin);
	c  = s[0];
	if ((ps == NULL) || (c == SC_CTL_D) || (c == SC_EOF))
	   break;

	cmd = SC_strtok(s, " \t\n", p);
	if (cmd == NULL)
	   continue;

	val = SC_strtok(NULL, " \t\n", p);
	if (val != NULL)
	   n = SC_stoi(val);
	else
	   n = -1;

	if (strcmp(cmd, "resume") == 0)
	   break;

	else if ((strcmp(cmd, "exit") == 0) ||
		 (strcmp(cmd, "quit") == 0) ||
		 (strcmp(cmd, "end") == 0))
	   {st = TRUE;
	    break;}

	else if (strcmp(cmd, "help") == 0)
	   {io_printf(stdout, "Commands:\n");
	    io_printf(stdout, "   end      - exit session\n");
	    io_printf(stdout, "   log [#]  - display the connection log(s)\n");
	    io_printf(stdout, "   load [#] - display the connection load info\n");
	    io_printf(stdout, "   net      - show network statistics\n");
	    io_printf(stdout, "   resume   - continue session\n");
	    io_printf(stdout, "   slog [#] - display the server log(s)\n");
	    io_printf(stdout, "   stat [#] - show the statistics for all connections\n");
	    io_printf(stdout, "   unf [#]  - show unfinished jobs for all connections\n");
	    io_printf(stdout, "\n");}

	else if (strcmp(cmd, "load") == 0)
	   {nc = SC_array_get_n(cp->pool);

	    io_printf(stdout, "   #         Host  Running  LoadAvg  FreeMem  TimeAvg\n");
	    for (ic = 0; ic < nc; ic++)
	        {pco = GET_CONNECTION(cp, ic);
	         if ((n > 0) && (n-1 != ic))
		    continue;
		 if (pco == NULL)
		    continue;

		 io_printf(stdout, "%4d %12s  %4d      %5.2f   %5.2f%%     %.2f\n",
			   ic+1, pco->host, pco->n_running,
			   pco->load, pco->memory, pco->dt);};

	    io_printf(stdout, "\n");}

	else if (strcmp(cmd, "log") == 0)
	   SC_show_pool_logs(cp, n);

	else if (strcmp(cmd, "net") == 0)
	   SC_connection_pool_net(cp);

	else if (strcmp(cmd, "slog") == 0)
	   _SC_show_server_logs(cp, n);

	else if (strcmp(cmd, "stat") == 0)
	   SC_show_pool_stats(cp, n, TRUE);

	else if (strcmp(cmd, "unf") == 0)
	   SC_show_pool_unf(cp, n, TRUE);};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
