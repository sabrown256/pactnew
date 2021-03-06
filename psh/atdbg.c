/*
 * ATDBG.C - attach a debugger to one of the named jobs 
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"
#include "libtime.c"
#include "libasync.c"

#if 0
/* TV_CLI_1 refers to the CLI language used before about TV 8.11 */
#define HAVE_TV_CLI_1
#endif

/* TV_CLI_2 refers to the CLI language used from TV 8.11 to present */
#define HAVE_TV_CLI_2

#define ATTACH 10
#define TRACE  11

/* GDB actions */

#define QTHREAD 100
#define BTRACE  101

typedef struct s_atdbgdes atdbgdes;
typedef struct s_atproc atproc;
typedef struct s_dbgrsp dbgrsp;

struct s_atdbgdes
   {char os[BFLRG];
    char pname[BFLRG];
    char opt[BFLRG];
    char exe[BFLRG];
    char dbg[BFLRG];
    int qpid;                    /* PID from query */
    int spid;                    /* PID from command line */
    int mode;
    int verbose;
    int quiet;
    int whdbg;
    char **msgs;
    int nmsg;
    char logname[BFLRG];
    FILE *log;};

struct s_atproc
   {char name[BFLRG];
    int pid;};

struct s_dbgrsp
   {int action;
    char *prompt;
    char result[BFLRG];
    atdbgdes *st;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CANDIDATE_PROC - return an array of candidate processes
 *                - derived from NAME
 */

static atproc *candidate_proc(atdbgdes *st, const char *name)
   {int i, n;
    char pids[BFLRG];
    char *pl, *t;
    atproc *al;

    i  = 0;
    n  = 10;
    al = MAKE_N(atproc, n);
    if (al != NULL)

/* find the candidate process ids */
       {if ((IS_NULL(st->exe) == FALSE) && (st->spid > 0))
	   {al[i].pid = st->spid;
	    nstrncpy(al[i].name, BFLRG, st->exe, -1);
	    i++;}
	else
	   {nstrncpy(pids, BFLRG,
		     run(FALSE, "ls-jobs %s \"%s\" | sed '/atdbg /d' | sed '/sign /d' | awk '{print $2 \" \" $6}'",
			 st->opt, name), -1);
	    if (st->verbose == TRUE)
	       printf("%s\n", pids);

	    for (i = 0, pl = pids; TRUE; i++)
	        {t = strtok(pl, " \n");
		 if (t != NULL)
		    al[i].pid = atoi(t);
		 else
		    break;
		 pl = NULL;
		 t = strtok(pl, " \n");
		 if (t != NULL)
		    file_path(t, al[i].name, BFLRG);
/*		    nstrncpy(al[i].name, BFLRG,
			     run(FALSE, "rpath %s", t), -1); */
		 else
		    break;

		 if (i >= n-2)
		    {n += 10;
		     REMAKE(al, atproc, n);
		     if (al == NULL)
		        return(NULL);};};};

	 al[i].pid     = -1;
	 al[i].name[0] = '\0';};

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SELECT_PID - interactively select the appropriate process */

static atproc *select_pid(atdbgdes *st, atproc *al)
   {int i, ok;
    char resp[BFLRG];
    atproc *pal;

    ok = FALSE;
    for (i = 0, pal = al; pal->pid > 0; i++, pal++)
    
/* if no PID specified stop, present, and query */
	{if (st->spid > 0)
	    ok = (pal->pid == st->spid);

	 else
	    {printf("%s %s %d (y/n): ",
		    (st->mode == ATTACH) ? "attach" : "trace",
		    pal->name, pal->pid);

	     fgets(resp, BFLRG, stdin);
	     ok = (resp[0] == 'y');};

	 if (ok == TRUE)
	    {if (st->verbose == TRUE)
	        {printf("\n");
		 printf("Debugger:   %s\n", st->dbg);
		 printf("Executable: %s\n", pal->name);
		 printf("PID:        %d\n", pal->pid);};
	     break;};};

    if (ok == FALSE)
       pal = NULL;

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEND_MSG - prepare and send a message to PP */

static int send_msg(process *pp, int act, const char *fmt, ...)
   {int nl;
    char msg[BFLRG];
    char *p;
    dbgrsp *gr;
    atdbgdes *st;

    VA_START(fmt);

    if (fmt == NULL)
       p = NULL;
    else
       {VSNPRINTF(msg, BFLRG, fmt);
	p  = msg;};

    VA_END;

    gr = (dbgrsp *) pp->a;
    st = gr->st;
    gr->action = act;

/* log the message sent */
    if ((st->log != NULL) && (p != NULL))
       fprintf(st->log, "SND: %s\n", p);

    nl = job_response(pp, 30000, p);

    return(nl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CACHE_RSP - put the message S in the cache for later transmission */

static void cache_rsp(atdbgdes *st, const char *s)
   {

    if (st->msgs == NULL)
       {st->msgs = MAKE_N(char *, 100000);
        st->nmsg = 0;};

    if (st->msgs != NULL)
       st->msgs[st->nmsg++] = STRSAVE(s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEND_CACHED - now print the cached messages to stdout */

static void send_cached(atdbgdes *st)
   {int i, n;
    char r[BFLRG];
    char *s, **sa;

    n  = st->nmsg;
    sa = st->msgs;

/* print all the messages */
    for (i = 0; i < n; i++)
        {s = sa[i];

	 strclean(r, BFLRG, s, -1);

	 puts(r);

         FREE(s);};

/* reset */
    FREE(st->msgs);
    st->nmsg = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RSP_ERROR - handle errors such as timeouts or poll rejects */

static int rsp_error(int fd, process *pp, char *t)
   {int ok;
    dbgrsp *gr;
    atdbgdes *st;

    gr = (dbgrsp *) pp->a;
    st = gr->st;

    if (LAST_CHAR(t) == '\n')
       LAST_CHAR(t) = '\0';

    if (st->log != NULL)
       fprintf(st->log, "ERR: while handling '%s'\n", t);

    ok = FALSE;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RSP_TOTALVIEW - handle lines from the totalview CLI */

static int rsp_totalview(int fd, process *pp, char *t)
   {int ok, nc;
    char p[BFLRG];
    char *s, *pa, *pb, *pr;
    dbgrsp *gr;
    atdbgdes *st;

    ok = FALSE;

    if ((pp != NULL) && (t != NULL))
       {gr = (dbgrsp *) pp->a;
	st = gr->st;
	pr = gr->prompt;
	nc = strlen(pr);

	if (LAST_CHAR(t) == '\n')
	   LAST_CHAR(t) = '\0';

	if (st->log != NULL)
	   fprintf(st->log, "RCV: %s\n", t);

	ok = (strncmp(t, pr, nc) == 0);

	if (ok == FALSE)
	   {switch (gr->action)
	       {case QTHREAD:
		     if (IS_NULL(t) == FALSE)
		        nstrncpy(gr->result, BFLRG, t, -1);
		     break;

		case BTRACE:
		     if (strstr(t, "PC=") != NULL)
		        {nstrncpy(p, BFLRG, t, -1);
			 pa = strstr(p, "PC=");
			 pb = strchr(p, '[');
			 if ((pa != NULL) && (pb != NULL))
			    memmove(pa, pb, strlen(pb)+1);
			 s = p;
			 s = subst(s, "PC=", " ", 1);
			 s = subst(s, "> ", "  ", 1);
			 s = subst(s, "#", ":", 1);
			 cache_rsp(st, s);
			 if (strstr(s, "SC_retrace_exe") != NULL)
			    cache_rsp(st, "         -+---------------------------+-");};
		     break;};};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USE_TOTALVIEW - do the work with totalview CLI
 *               - NOTE: the function call arguments and values
 *               - are printed on separate lines which will require
 *               - some work to reassemble
 *               - for now just toss those lines
 */

static int use_totalview(atdbgdes *st, atproc *al)
   {int nl, rv, sig;
    char cmd[BFLRG], s[BFLRG];
    process *pp;
    dbgrsp gr;

    rv = TRUE;

    if (st->mode == TRACE)
       {gr.prompt = "d1.<> ";
	gr.st     = st;

	snprintf(cmd, BFLRG, "%scli", st->dbg);
	pp = job_launch(cmd, "a", NULL);
	if (pp != NULL)
	   {pp->accept = rsp_totalview;
	    pp->reject = rsp_error;
	    pp->a      = &gr;

	    send_msg(pp, 0, NULL);

#if defined(HAVE_TV_CLI_1)
/* TV versions < 8.11.0 */
	    send_msg(pp, 0, "dset LINES_PER_SCREEN 0");
	    send_msg(pp, 0, "dattach -no_attach_parallel %s %d",
		     al->name, al->pid);
	    send_msg(pp, 0, "dhalt");

	    send_msg(pp, QTHREAD, "dfocus p1 TV::thread get -all id");
	    FOREACH(thr, gr.result, " \n");
	       snprintf(s, BFLRG,
			"\n   ---------------- Thread %s ----------------\n", thr);
	       cache_rsp(st, s);
	       send_msg(pp, BTRACE, "dfocus %s {dwhere -a}", thr);
	    ENDFOR;

	    nl = send_msg(pp, 0, "exit -force");

#elif defined(HAVE_TV_CLI_2)

/* TV version >= 8.11.0 */
	    send_msg(pp, 0, "dattach %s %d", al->name, al->pid);
	    send_msg(pp, QTHREAD, "dfocus p1 TV::focus_threads");

	    FOREACH(thr, gr.result, " \n");
	       snprintf(s, BFLRG,
			"\n   ---------------- Thread %s ----------------\n", thr);
	       cache_rsp(st, s);
	       send_msg(pp, BTRACE, "dfocus %s {dhalt ; dwhere -a}", thr);
	    ENDFOR;

	    nl = send_msg(pp, 0, "exit -force");
#endif

	    sig = (nl == -1) ? SIGKILL : -1;
	    job_done(pp, sig);};}

    else
       run(FALSE, "%s %s -pid %d", st->dbg, al->name, al->pid);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RSP_GDB - handle lines from GDB */

static int rsp_gdb(int fd, process *pp, char *t)
   {int ok, nc;
    char p[BFLRG];
    char *s, *pr, *u;
    dbgrsp *gr;
    atdbgdes *st;

    gr = (dbgrsp *) pp->a;
    st = gr->st;
    pr = gr->prompt;
    nc = strlen(pr);

    if (LAST_CHAR(t) == '\n')
       LAST_CHAR(t) = '\0';

    if (st->log != NULL)
       fprintf(st->log, "RCV: %s\n", t);

    ok = (strncmp(t, pr, nc) == 0);

    if (ok == FALSE)
       {switch (gr->action)
	   {case QTHREAD:
	         u = strtok(t, " \t\n");
		 if ((u != NULL) && (*u == '*'))
		    u = strtok(NULL, " \t\n");
                 push_tok(gr->result, BFLRG, ' ', u);
	         break;

	    case BTRACE:
		 if (t[0] == '#')
		    {snprintf(p, BFLRG, "%s]", t+1);
		     s = subst(p, " (", "(", 1);
		     s = subst(s, " )", ")", 1);
		     s = subst(s, ") from ", ") [", 1);
		     s = subst(s, ") at ", ") [", 1);
		     s = subst(s, "../", "", -1);
		     s = subst(s, ", ", ",", -1);
		     s = subst(s, "<signal", "<----------signal", -1);
		     s = subst(s, "called>]", "called---------->", -1);
		     cache_rsp(st, s);
		     if (strstr(s, "SC_retrace_exe") != NULL)
		        cache_rsp(st, "         -+---------------------------+-");};
		 break;};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USE_GDB - do the work with GDB */

static int use_gdb(atdbgdes *st, const char *fname, atproc *al)
   {int rv, nl, sig;
    char cmd[BFLRG], s[BFLRG];
    process *pp;
    dbgrsp gr;

    rv = TRUE;

    if (st->mode == TRACE)
       {gr.prompt    = "(gdb) ";
	gr.result[0] = '\0';
	gr.st        = st;

	snprintf(cmd, BFLRG, "%s %s", st->dbg, al->name);
	pp = job_launch(cmd, "a", NULL);
	if (pp != NULL)
	   {pp->accept = rsp_gdb;
	    pp->reject = rsp_error;
	    pp->a      = &gr;

	    send_msg(pp, 0, NULL);
	    send_msg(pp, 0, "set width -1");
	    send_msg(pp, 0, "set height -1");
	    send_msg(pp, 0, "set confirm off");
	    send_msg(pp, 0, "attach %d", al->pid);
	    send_msg(pp, QTHREAD, "info threads");

	    FOREACH(thr, gr.result, " \n")
	       snprintf(s, BFLRG,
			"\n   ---------------- Thread %s ----------------\n", thr);
	       cache_rsp(st, s);
	       send_msg(pp, 0, "thread %s", thr);
	       send_msg(pp, BTRACE, "backtrace");
	    ENDFOR;

	    send_msg(pp, 0, "detach");

	    nl = send_msg(pp, 0, "quit");

	    sig = (nl == -1) ? SIGKILL : -1;
	    job_done(pp, sig);};}

    else
       run(FALSE, "xterm -sb -e %s %s %d", st->dbg, al->name, al->pid);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USE_DBX - do the work with DBX */

static int use_dbx(atdbgdes *st, const char *fname, atproc *al)
   {int rv;
    char *s;
    FILE *fp;

    rv = TRUE;

    if (st->mode == TRACE)
       {fp = fopen_safe(fname, "w");
	if (fp != NULL)
	   {fprintf(fp, "attach %d\n", al->pid);
	    fprintf(fp, "where\n");
	    fprintf(fp, "quit\n");
	    fclose_safe(fp);

	    FOREACH(t, run(FALSE, "%s %s -c %s", st->dbg, al->name, fname), "\n")
	       s = t;
	       s = subst(s, ">", " ", 1);
	       cache_rsp(st, s);
	    ENDFOR;

	    unlink_safe(fname);};}

    else
       run(FALSE, "xterm -sb -e %s %s -pid %d", st->dbg, al->name, al->pid);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SESSION - work the session */

static int session(atdbgdes *st)
   {int i, n, rv, zip;
    char fname[BFLRG], names[BFLRG];
    char *name, *pl, *lst[33];
    atproc *al, *pal;
    FILE *lg;

    rv = TRUE;

    if (st->whdbg == TRUE)
       {printf("%s\n", st->dbg);
        exit(0);};

/* open the log file if requested */
    if (IS_NULL(st->logname) == FALSE)
       {snprintf(fname, BFLRG, "%s.%d", st->logname, (int) getpid());
	lg = fopen_safe(fname, "w");
	if (lg != NULL)
	   setbuf(lg, NULL);
	st->log = lg;};

    if (st->quiet == FALSE)
       printf("\n");

    if (st->mode == TRACE)
       snprintf(fname, BFLRG, "/tmp/dbg.%d", (int) getpid());

    if ((IS_NULL(st->pname) == TRUE) &&
        ((IS_NULL(st->exe) == FALSE) || (st->spid >= 0)))
       snprintf(st->pname, BFLRG, "%d", st->spid);

/* make an array of process names */
    nstrncpy(names, BFLRG, st->pname, -1);
    for (n = 0, pl = names; n < 32; n++, pl = NULL)
        {name = strtok(pl, " \n");
	 if (name == NULL)
	    break;
	 lst[n] = STRSAVE(name);};
    lst[n] = NULL;

/* check each name for matching processes */
    for (i = 0; i < n; i++)
        {name = lst[i];
	 zip  = 0;
	 al   = candidate_proc(st, name);

/* act on the list of process ids */
	 if (al == NULL)
            {zip++;
	     if (st->quiet == FALSE)
	        printf("No processes found under '%s'\n", name);}

	 else
	    {pal = select_pid(st, al);
	     if (pal != NULL)
	        {if (strstr(st->dbg, "totalview") != NULL)
		    rv = use_totalview(st, pal);

		 else if (strstr(st->dbg, "gdb") != NULL)
		    rv = use_gdb(st, fname, pal);

		 else if (strstr(st->dbg, "dbx") != NULL)
		    rv = use_dbx(st, fname, pal);

		 send_cached(st);

		 break;};};

	 FREE(al);
         FREE(lst[i]);};

    if (st->quiet == FALSE)
       printf("\n");

/* close the log file if requested */
    if (st->log != NULL)
       fclose_safe(st->log);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT - setup the session */

static int init(atdbgdes *st)
   {int rv;
    char lst[BFLRG], t[BFLRG];

    rv = TRUE;

    unamef(st->os, BFLRG, "s");
    nstrncpy(st->opt, BFLRG, "-q", -1);
    st->pname[0] = '\0';
    st->exe[0]   = '\0';
    st->dbg[0]   = '\0';
    st->spid     = -1;
    st->msgs     = NULL;
    st->nmsg     = 0;
    st->mode     = ATTACH;
    st->verbose  = FALSE;
    st->quiet    = FALSE;
    st->whdbg    = FALSE;

/* init logging */
    st->log = NULL;
    if (cdefenv("ATDBG_LOG") == TRUE)
       nstrncpy(st->logname, BFLRG, cgetenv(TRUE, "ATDBG_LOG"), -1);
    else
       st->logname[0] = '\0';

/* determine which debugger to use */
    if (cdefenv("SC_DEBUGGER") == TRUE)
       {nstrncpy(t, BFLRG, cgetenv(TRUE, "SC_DEBUGGER"), -1);
        nstrncpy(lst, BFLRG, subst(t, ":", " ", BFLRG), -1);}
    else
       nstrncpy(lst, BFLRG, "totalview gdb dbx", -1);

    FOREACH(i, lst, " \n")
       nstrncpy(t, BFLRG, cwhich(i), -1);
       if (file_executable(t) == TRUE)
          {nstrncpy(st->dbg, BFLRG, t, -1);
           break;};
    ENDFOR;

    if (IS_NULL(st->dbg) == TRUE)
       {printf("There is no recognized debugger - exiting\n");
        rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - tell how to run atdbg */

static void help(void)
   {

    printf("\n");
    printf("Usage:  atdbg [-a] [-e <path>] [-h] [-l <file>] [-p <pid>] [-q] [-r] [-v] [-w] [name]*\n");
    printf("\n");
    printf("Present list of named jobs and attach debugger to selected one.\n");
    printf("\n");
    printf("        a    - search all jobs (i.e. ignore .ls-jobs-remove file)\n");
    printf("        e    - full path to the executable\n");
    printf("        h    - this help message\n");
    printf("        l    - log traffic to named file\n");
    printf("        p    - attach to specified PID\n");
    printf("        q    - quiet mode\n");
    printf("        r    - just show the current call stack\n");
    printf("        v    - verbose flag\n");
    printf("        w    - show which debugger is being used\n");
    printf("        name - executable name or command line pattern\n");
    printf("               used to identify processes\n");
    printf("\n");

    exit(1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_ARGS - process the command line arguments */

static int process_args(atdbgdes *st, int c, char **v)
   {int i;

    if (c == 0)
       help();

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-a") == 0)
            {push_tok(st->opt, BFLRG, ' ', "-r");
	     push_tok(st->opt, BFLRG, ' ', "none-");}

	 else if (strcmp(v[i], "-e") == 0)
            nstrncpy(st->exe, BFLRG, v[++i], -1);

	 else if (strcmp(v[i], "-h") == 0)
            help();

	 else if (strcmp(v[i], "-l") == 0)
            nstrncpy(st->logname, BFLRG, v[++i], -1);

	 else if (strcmp(v[i], "-p") == 0)
            st->spid = atoi(v[++i]);

	 else if (strcmp(v[i], "-q") == 0)
            st->quiet = TRUE;

	 else if (strcmp(v[i], "-r") == 0)
            st->mode = TRACE;
/*
	 else if (strcmp(v[i], "-u") == 0)
            USER = v[++i];
*/
	 else if (strcmp(v[i], "-v") == 0)
	    st->verbose = TRUE;

	 else if (strcmp(v[i], "-w") == 0)
	    st->whdbg = TRUE;

	 else if (v[i][0] == '-')
	    continue;

	 else
	    push_tok(st->pname, BFLRG, ' ', v[i]);};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int rv;
    char exe[BFLRG], bindir[BFLRG];
    atdbgdes state;

/* find the directory with atdbg */
    nstrncpy(exe, BFLRG, cwhich(v[0]), -1);
    nstrncpy(bindir, BFLRG, path_head(exe), -1);

/* locate the tools needed for subshells */
    build_path(NULL,
	       "sh", "xterm", "ls-jobs", "rpath",
	       NULL);

    push_path(P_PREPEND, lpath, bindir);

    rv = init(&state);
    if (rv == TRUE)
       {rv = process_args(&state, c, v);
	rv = session(&state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

