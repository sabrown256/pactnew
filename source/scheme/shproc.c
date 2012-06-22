/*
 * SHPROC.C - C and Scheme Process control routines
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"
#include "scope_proc.h"

enum e_SS_io_kind
   {SS_IO_IN, SS_IO_OUT, SS_IO_ERR};

typedef enum e_SS_io_kind SS_io_kind;

typedef struct s_SS_io SS_io;
typedef struct s_SS_job SS_job;

struct s_SS_io
   {SS_io_kind kind;        /* INPUT, OUTPUT, or ERROR - 0, 1, or 2 */
    int fd;                 /* file descriptor */
    int pid;                /* pipeline id */
    object *iex;            /* input expr */
    object *oex;};          /* output expr */

struct s_SS_job
   {int id;
    SS_io fd[3];
    char *cmd;
    int exit;
    SS_job *next;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_WR_PROCESS - print a process object */

static void _SS_wr_process(SS_psides *si, object *obj, object *strm)
   {PROCESS *pp;
    int flag;
    FILE *str;

    str = SS_OUTSTREAM(strm);

    pp = SS_PROCESS_VALUE(obj);
    PRINT(str, "<PROCESS|%d-%d-%d-%d-", pp->id, pp->index, pp->in, pp->out);

    flag = (pp->status & ~SC_CHANGED);
    switch (flag)
       {case SC_RUNNING :
	     PRINT(str, "Running");
	     break;
        case SC_STOPPED :
	     PRINT(str, "Stopped-");
	     break;
        case SC_EXITED :
	     PRINT(str, "Exited-");
	     break;
        case (SC_EXITED | SC_COREDUMPED) :
	     PRINT(str, "Exited-Core-Dumped-");
	     break;
        case SC_SIGNALED :
	     PRINT(str, "Signaled-");
	     break;
        case (SC_SIGNALED | SC_COREDUMPED) :
	     PRINT(str, "Signaled-Core-Dumped-");
	     break;
        default :
	     PRINT(str, "Unknown");
	     break;};

    if (flag != SC_RUNNING)
       switch (flag)
          {case SC_STOPPED :
           case SC_EXITED :
           case (SC_EXITED | SC_COREDUMPED) :
           case SC_SIGNALED :
           case (SC_SIGNALED | SC_COREDUMPED) :
	        PRINT(str, "%d", pp->reason);
           default :
	        break;};

    PRINT(str, ">");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RL_PROCESS - this is need for the garbage collector */

void _SS_rl_process(SS_psides *si, object *obj)
   {PROCESS *pp;

    pp = SS_PROCESS_VALUE(obj);
    SC_close(pp);

    CFREE(SS_OBJECT(obj));

    return;}

/*--------------------------------------------------------------------------*/

/*                           PROCESS PREDICATES                             */

/*--------------------------------------------------------------------------*/

/* _SSI_PRP - process? at Scheme level */

static object *_SSI_prp(SS_psides *si, object *obj)
   {object *o;

    o = SS_processp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PR_RUNP - process-running? in Scheme */

static object *_SSI_pr_runp(SS_psides *si, object *obj)
    {object *o;

     if (!SS_processp(obj))
        SS_error(si, "OBJECT NOT PROCESS - PROCESS-RUNNING?", obj);

     o = (SS_PROCESS_STATUS(obj) == SC_RUNNING) ? SS_t : SS_f;

     return(o);}

/*--------------------------------------------------------------------------*/

/*                           PROCESS PRIMITIVES                             */

/*--------------------------------------------------------------------------*/

/* _SSI_OPN_PR - process-open in Scheme */

static object *_SSI_opn_pr(SS_psides *si, object *argl)
   {int n, i;
    char **argv, *mode;
    PROCESS *pp;
    object *obj;

    obj = SS_null;

/* pull the mode off */
    mode = NULL;
    SS_args(si, argl,
            SC_STRING_I, &mode,
            0);

    if (strchr("rwa", mode[0]) == NULL)
       SS_error(si, "BAD MODE - _SSI_OPN_PR", SS_car(si, argl));

    argl = SS_cdr(si, argl);

/* the rest of the args constitute the command line */
    n = SS_length(si, argl);

/* we need one extra for a NULL argument to terminate argv */
    argv = CMAKE_N(char *, n+1);

    for (i = 0 ; i < n; argl = SS_cdr(si, argl))
        {obj = SS_car(si, argl);

         if (SS_stringp(obj))
            argv[i++] = SS_STRING_TEXT(obj);
         else if (SS_variablep(obj))
            argv[i++] = SS_VARIABLE_NAME(obj);
         else
            SS_error(si, "BAD OBJECT - PROCESS-OPEN", obj);};

    argv[i] = NULL;

    pp = PC_open(argv, NULL, mode);
    SC_block_file(stdin);   
    if (pp == NULL)
       SS_error(si, "CAN'T OPEN PROCESS - PROCESS-OPEN", obj);

    CFREE(argv);

    obj = SS_mk_object(si, pp, SS_PROCESS_I, SELF_EV, NULL,
		       _SS_wr_process, _SS_rl_process);

    SC_mark(pp, 1);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_BLCK_PR - process-block in Scheme */

static object *_SSI_blck_pr(SS_psides *si, object *argl)
   {PROCESS *pp;
    object *o, *r;

    pp = NULL;
    o  = NULL;
    SS_args(si, argl,
            SS_PROCESS_I, &pp,
            SS_OBJECT_I, &o,
            0);

    if (SS_true(o))
       {SC_block(pp);
	r = SS_t;}
    else
       {SC_unblock(pp);
	r = SS_f;};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CLS_PR - process-close in Scheme */

static object *_SSI_cls_pr(SS_psides *si, object *obj)
   {PROCESS *pp;

    pp = NULL;
    SS_args(si, obj,
            SS_PROCESS_I, &pp,
            0);

    SC_close(pp);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PR_RD_TRIES - process-read-tries in Scheme */

static object *_SSI_pr_rd_tries(SS_psides *si, object *argl)
   {object *o;

    if (!SS_nullobjp(argl))
       SS_args(si, argl,
               SC_INT_I, &_SS.n_tries,
               0);

    o = SS_mk_integer(si, (int64_t) _SS.n_tries);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PR_STAT - process-status at the Scheme level */

static object *_SSI_pr_stat(SS_psides *si, object *obj)
   {PROCESS *pp;
    int st, rs;
    object *ret;

    if (!SS_processp(obj))
       SS_error(si, "ARGUMENT NOT PROCESS - PROCESS-STATUS", obj);

    pp = SS_PROCESS_VALUE(obj);

    st = pp->status;
    if (SC_process_status(pp) == SC_RUNNING)
       rs = 0;
    else
       rs = pp->reason;

    ret = SS_make_list(si, SC_INT_I, &pp->id,
                       SC_INT_I, &pp->in,
                       SC_INT_I, &pp->out,
                       SC_INT_I, &st,
                       SC_INT_I, &rs,
                       0);

    return(ret);}    

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PR_RD_LINE - process-read-line at Scheme level */

static object *_SSI_pr_rd_line(SS_psides *si, object *obj)
   {int i;
    char bf[MAX_BFSZ];
    PROCESS *pp;
    object *rv;

    if (!SS_processp(obj))
       SS_error(si, "ARGUMENT NOT PROCESS - PROCESS-READ-LINE", obj);

    rv = SS_null;

    pp = SS_PROCESS_VALUE(obj);
    for (i = 0; i < _SS.n_tries; i++)
        {if (SC_gets(bf, MAX_BFSZ, pp) != NULL)
	    {rv = SS_mk_string(si, bf);
	     break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PR_SN_LINE - process-send-line at Scheme level */

static object *_SSI_pr_sn_line(SS_psides *si, object *argl)
   {int ret;
    char *s;
    PROCESS *pp;
    object *o;

    pp = NULL;
    s  = NULL;
    SS_args(si, argl,
            SS_PROCESS_I, &pp,
            SC_STRING_I, &s,
            0);

    ret = SC_printf(pp, "%s\n", s);
    o   = ret ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_SET_IO_SPEC - initialize an SS_io specification */

static void _SS_set_io_spec(SS_psides *si, SS_job *pj,
			    SS_io_kind kind, object *ios)
   {int fd, pid, n;
    char *type, *host;
    SS_io *io;
    object *iex, *oex;

    fd  = -1;
    pid = -1;
    iex = SS_null;
    oex = SS_null;

/* stdin, stdout, stderr, or /dev/null */
    if (SS_booleanp(ios) == TRUE)
       {if (SS_BOOLEAN_VALUE(ios) == FALSE)
	   fd  = -1;
	else
	   fd = kind;}

/* output to open file */
    else if ((SS_outportp(ios) == TRUE) && (kind != SS_IO_IN))
       fd = fileno(SS_OUTSTREAM(ios));

/* input from open file */
    else if ((SS_inportp(ios) == TRUE) && (kind == SS_IO_IN))
       fd = fileno(SS_INSTREAM(ios));

    else if (SS_consp(ios) == TRUE)
       {type = NULL;
	host = NULL;
	n    = -1;
	SS_args(si, ios,
		SC_STRING_I,  &type,
		SC_INTEGER_I, &n,
		SC_STRING_I,  &host,
		0);

/* process in pipeline relative to this process */
	if (strcmp(type, "piper") == 0)
	   pid = pj->id + n;

/* process in pipeline absolute position */
	else if (strcmp(type, "pipea") == 0)
	   pid = n;

/* file name */
	else if (strcmp(type, "file") == 0)
	   {if (host == NULL)
	       fd = n;
	    else
	       {if (kind == SS_IO_IN)
		   fd = open(host, O_RDONLY);

/* GOTCHA: worrry about append or create */
		else
		   fd = open(host, O_WRONLY);};}

/* socket connection */
	else if (strcmp(type, "socket") == 0)
	   {int to, fm;

	    to = 10000;         /* timeout after 10 seconds */
	    fm = FALSE;         /* fatal on timeout */

	    fd = _SC_tcp_connect(host, n, to, fm);}
      
/* expression to be evaluated for input or output */
        else
	   {if (kind == SS_IO_IN)
	       iex = ios;
	    else
	       oex = ios;
	    SS_mark(ios);};};

    io = pj->fd + kind;
    if (io != NULL)
       {io->kind = kind;
	io->fd   = fd;
        io->pid  = pid;
        io->iex  = iex;
        io->oex  = oex;};

    return;}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* _SS_MAKE_IO_SPEC - make and return an SS_io specification */

static SS_io *_SS_make_io_spec(SS_psides *si, SS_io_kind kind, object *ios)
   {SS_io *io;

    io = CMAKE(SS_io);
    if (io != NULL)
       _SS_set_io_spec(si, io, kind, ios);

    return(io);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_FREE_IO_SPEC - free an SS_io specification */

static void _SS_free_io_spec(SS_io *io)
   {

    CFREE(io);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SS_MAKE_PIPELINE - make a SS_job out of IS, OS, ES, and CMD
 *                   - add it to JOBS and return the result
 */

static SS_job *_SS_make_pipeline(SS_psides *si, SS_job *jobs, int id,
				 object *is, object *os, object *es,
				 char *cmd)
   {SS_job *pj;

    pj = CMAKE(SS_job);
    if (pj != NULL)
       {pj->id   = id;
	pj->cmd  = CSTRSAVE(cmd);
	pj->exit = -1;
	pj->next = jobs;

	_SS_set_io_spec(si, pj, SS_IO_IN,  is);
	_SS_set_io_spec(si, pj, SS_IO_OUT, os);
        _SS_set_io_spec(si, pj, SS_IO_ERR, es);};

    return(pj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_FREE_JOB - free the job PJ */

static void _SS_free_job(SS_job *pj)
   {

    if (pj != NULL)
       {CFREE(pj->cmd);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_FREE_PIPELINE - free the job pipeline JOBS */

static void _SS_free_pipeline(SS_job *jobs)
   {SS_job *pj, *nxt;

    for (pj = jobs; pj != NULL; pj = nxt)
        {nxt = pj->next;
	 _SS_free_job(pj);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RUN_PIPELINE - run the job pipeline JOBS
 *                  - return the list of exit statuses
 */

static object *_SS_run_pipeline(SS_psides *si, SS_job *jobs)
   {SS_job *pj, *nxt;
    object *o;

    o = SS_null;
    for (pj = jobs; pj != NULL; pj = nxt)
        {nxt = pj->next;
	 o = SS_mk_cons(si, SS_mk_integer(si, pj->exit), o);};

    _SS_free_pipeline(jobs);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PR_EXEC - exec at Scheme level
 *              - syntax: (exec [<in> <out> <err> <cmd>]*)
 *              -   <in>  := <io-spec>
 *              -   <out> := <io-spec>
 *              -   <err> := <io-spec>
 *              -   <cmd> := command line string
 *              -   <io-spec>   := #t | #f | <file> | <expr>
 *              -   <file>      := <file-name> | <file-port> |
 *                                 <file-descr> | <socket> | <pipe-id>
 *              -   <file-name> := file name string
 *              -   <file-port> := <input-port> | <output-port>
 *              -   <file-desc> := integer file descriptor
 *              -   <socket>    := (socket <port> <host>)
 *              -   <pipe-id>   := (piper <id>) | (pipea <id>)
 *              -   <id>        := piper - relative index <id>
 *              -                  pipea - absolute index <id>
 *              -   <expr>      := <var> | <function>
 *              -   <var>       := <scalar> | <array> | <list>
 *              -   <function>  := function call
 *              - an <io-spec> of #t is the default standard I/O
 *              - (i.e. stdin, stdout, or stderr)
 *              - an <io-spec> of #f is /dev/null
 *              - returns a list containing the exit status of
 *              - each process in the pipeline
 *
 *              - msh version would be:
 *              -   <cmd> [@<d><spec>]*
 *              -   <d>     := i (input) | o (output) | e (error) | b (o and e)
 *              -   <spec>  := <file> | a<id> | r<id> | <expr> | <blank>
 *              -   <id>    := absolute/relative position in pipeline
 *              -   <expr>  := <scalar> | <array> | <call> | ...
 *              -   <blank> := nothing - implies standard descriptor
 *
 *              - examples:
 *              -   cat @ifoo.bar @br1 grep "foo"
 *              -   echo $status -> 0 0
 *              -   cat @ifoo.bar @or1 @er2 grep "foo" @o grep "bar" @ojunk
 *              -   echo $status -> 0 0 1
 *              -   cat @ifoo.bar @ofoo.org:15000 @er1 grep "bar" @ojunk
 *              -   echo $status -> 0 0
 *              -   cat @ifoo.org:15000 grep "bar" @ojunk
 *              -   echo $status -> 0 0
 *
 */

static object *_SSI_pr_exec(SS_psides *si, object *argl)
   {int i, nl, nj;
    char *cmd;
    SS_job *jobs;
    object *o, *is, *os, *es, *t;

    o = SS_null;

    if (!SS_nullobjp(argl))
       {nl = SS_length(si, argl);

/* check that we have quadruples of jobs */
	if (nl % 4 == 0)
           {nj = nl >> 2;

            jobs = NULL;

	    for (i = 0; i < nj; i++)
	        {is   = SS_car(si, argl);
		 argl = SS_cdr(si, argl);

		 os   = SS_car(si, argl);
		 argl = SS_cdr(si, argl);

	         es   = SS_car(si, argl);
		 argl = SS_cdr(si, argl);

	         t    = SS_car(si, argl);
		 argl = SS_cdr(si, argl);
		 cmd  = NULL;
		 SS_args(si, t,
			 SC_STRING_I, &cmd,
			 0);

		 jobs = _SS_make_pipeline(si, jobs, i, is, os, es, cmd);};

	    o = _SS_run_pipeline(si, jobs);};};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GET_URL_FILE - grab a web page into a file */

static object *_SSI_get_url_file(SS_psides *si, object *argl)
   {int ret;
    char *url, *file, *vers;
    object *o;

    url  = NULL;
    file = NULL;
    vers = NULL;
    SS_args(si, argl,
            SC_STRING_I, &url,
            SC_STRING_I, &file,
            SC_STRING_I, &vers,
            0);

    ret = SC_http_url_file(url, file, vers);

    o = ret ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GET_HOST_NAME - given a system type SYS return an available host */

static object *_SSI_get_host_name(SS_psides *si, object *argl)
   {int ret;
    char hst[MAXLINE];
    char *sys;
    object *rv;

    sys  = NULL;
    SS_args(si, argl,
            SC_STRING_I, &sys,
            0);

    ret = SC_get_host_name(hst, MAXLINE, sys);

    CFREE(sys);

    if (ret)
       rv = SS_mk_string(si, hst);
    else
       rv = SS_null;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GET_HOST_TYPES - get the list of all available host types
 *                     - it is the clients responsibility to free
 *                     - array and the strings with SC_free_strings
 */

static object *_SSI_get_host_types(SS_psides *si, object *argl)
   {int i, all;
    char *t, *net, **strs;
    object *s, *strl;

    all = TRUE;
    net = NULL;
    SS_args(si, argl,
            SC_INT_I, &all,
            SC_STRING_I, &net,
            0);

    strs = SC_get_host_types(all, net);
    strl = SS_null;
    if (strs != NULL)
       {for (i = 0; TRUE; i++)
	    {t = strs[i];
	     if (t == NULL)
	        break;
	     else
	        {s = SS_mk_string(si, t);
		 SS_assign(si, strl, SS_mk_cons(si, s, strl));};};

	SC_free_strings(strs);

	strl = SS_reverse(si, strl);};

    return(strl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_RESOURCE_USAGE - return a list of available resource usage for PID */

static object *_SSI_resource_usage(SS_psides *si, object *argl)
   {int pid, err;
    SC_rusedes ru;
    object *o;

    pid = TRUE;
    SS_args(si, argl,
            SC_INT_I, &pid,
            0);

    if (pid == -1)
       pid = SYS_GETPID();

    err = SC_resource_usage(&ru, pid);
    if (err == TRUE)
       o = SS_make_list(si, SC_INT_I, &ru.pid,      /* process id */
			SC_INT_I,     &ru.ppid,     /* process parent id */
			SC_INT_I,     &ru.uid,      /* process user id */
			SC_INT_I,     &ru.priority, /* process scheduling priority */
			SC_STRING_I,  &ru.cmd,      /* command line */
			SC_DOUBLE_I,  &ru.ut,       /* total user time used (in sec) */
			SC_DOUBLE_I,  &ru.st,       /* total system time used (in sec) */
			SC_DOUBLE_I,  &ru.maxrss,   /* maximum resident set size (in MBy) */
			SC_DOUBLE_I,  &ru.idrss,    /* data segment memory used */
			SC_DOUBLE_I,  &ru.isrss,    /* stack memory used */
			SC_DOUBLE_I,  &ru.minflt,   /* soft page faults - reclaims */
			SC_DOUBLE_I,  &ru.majflt,   /* hard page faults - requiring I/O */
			SC_DOUBLE_I,  &ru.nswap,    /* times a process was swapped out of memory */
			SC_DOUBLE_I,  &ru.inblock,  /* block input operations via the file system */
			SC_DOUBLE_I,  &ru.outblock, /* block output operations via the file system */
			SC_DOUBLE_I,  &ru.msgsnd,   /* IPC messages sent */
			SC_DOUBLE_I,  &ru.msgrcv,   /* IPC messages received */
			SC_DOUBLE_I,  &ru.nsignals, /* signals delivered */
			SC_DOUBLE_I,  &ru.nvcsw,    /* voluntary context switches */
			SC_DOUBLE_I,  &ru.nivcsw,   /* involuntary context switches */
			SC_DOUBLE_I,  &ru.nsysc,    /* system calls */
			0);

    else
       o = SS_null;

    return(o);}

/*--------------------------------------------------------------------------*/

/*                              INSTALL FUNCTIONS                           */

/*--------------------------------------------------------------------------*/

/* _SS_INST_PROC - install the primitives for process control */

void _SS_inst_proc(SS_psides *si)
   {

    SS_install(si, "pipeline",
               "Procedure: Run job pipeline\nUsage: (pipeline [<in> <out> <err> <command>]*)",
               SS_nargs,
               _SSI_pr_exec, SS_PR_PROC);

    SS_install(si, "process?",
               "Procedure: Returns #t if the object is a PROCESS object",
               SS_sargs,
               _SSI_prp, SS_PR_PROC);

    SS_install(si, "process-block",
               "Procedure: Set process to be blocked if flag is #t and unblocked otherwise",
               SS_nargs,
               _SSI_blck_pr, SS_PR_PROC);

    SS_install(si, "process-close",
               "Procedure: Terminate a process",
               SS_sargs,
               _SSI_cls_pr, SS_PR_PROC);

    SS_install(si, "process-open",
               "Procedure: Exec a process and communicate with it",
               SS_nargs,
               _SSI_opn_pr, SS_PR_PROC);

    SS_install(si, "process-read-line",
               "Procedure: Returns a string recieved from a process",
               SS_sargs,
               _SSI_pr_rd_line, SS_PR_PROC);

    SS_install(si, "process-read-tries",
               "Procedure: Get/Set number of attempts to read from unblocked process",
               SS_nargs,
               _SSI_pr_rd_tries, SS_PR_PROC);

    SS_install(si, "process-running?",
               "Procedure: Returns #t if the process is still running",
               SS_sargs,
               _SSI_pr_runp, SS_PR_PROC);

    SS_install(si, "process-send-line",
               "Procedure: Send a string to a process",
               SS_nargs,
               _SSI_pr_sn_line, SS_PR_PROC);

    SS_install(si, "process-status",
               "Procedure: Returns a list of the process id, in, out, status, reason",
               SS_sargs,
               _SSI_pr_stat, SS_PR_PROC);

    SS_install(si, "url->file",
               "Procedure: Copy a web page to a file\nUsage: (url->file url [file] [vers])",
               SS_nargs,
               _SSI_get_url_file, SS_PR_PROC);

    SS_install(si, "get-host-name",
               "Procedure: given a system type string SYS return an available host\nUsage: (get-host-name <sys>)",
               SS_nargs,
               _SSI_get_host_name, SS_PR_PROC);

    SS_install(si, "get-host-types",
               "Procedure: return a list of all available hosts types\nUsage: (get-host-types)",
               SS_nargs,
               _SSI_get_host_types, SS_PR_PROC);

    SS_install(si, "resource-usage",
               "Procedure: return a list of resource usage info for the given pid\nUsage: (resource-usage pid)",
               SS_nargs,
               _SSI_resource_usage, SS_PR_PROC);

    _SS.n_tries = 100;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
