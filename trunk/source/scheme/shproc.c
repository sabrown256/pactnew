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
		 SS_Assign(strl, SS_mk_cons(si, s, strl));};};

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

    err = SC_resource_usage(&ru, pid);
    if (err == TRUE)
       o = SS_make_list(si, SC_INT_I, &ru.pid,      /* process id */
			SC_INT_I, &ru.ppid,     /* process parent id */
			SC_INT_I, &ru.uid,      /* process user id */
			SC_INT_I, &ru.priority, /* process scheduling priority */
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
