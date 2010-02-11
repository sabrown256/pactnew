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

#define ATTACH 10
#define TRACE  11

/* GDB actions */

#define QTHREAD 100
#define BTRACE  101

typedef struct s_atdbgdes atdbgdes;
typedef struct s_atproc atproc;
typedef struct s_dbgrsp dbgrsp;

struct s_atdbgdes
   {char os[MAXLINE];
    char pname[MAXLINE];
    char opt[MAXLINE];
    char exe[MAXLINE];
    char dbg[MAXLINE];
    int qpid;                    /* PID from query */
    int spid;                    /* PID from command line */
    int mode;
    int verbose;
    int quiet;
    int whdbg;
    char **msgs;
    int nmsg;
    char logname[MAXLINE];
    FILE *log;};

struct s_atproc
   {char name[MAXLINE];
    int pid;};

struct s_dbgrsp
   {int action;
    char *prompt;
    char result[MAXLINE];
    atdbgdes *st;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CANDIDATE_PROC - return an array of candidate processes
 *                - derived from NAME
 */

static atproc *candidate_proc(atdbgdes *st, char *name)
   {int i, n;
    char pids[MAXLINE];
    char *pl, *t;
    atproc *al;

    i  = 0;
    n  = 10;
    al = MAKE_N(atproc, n);

/* find the candidate process ids */
    if ((st->exe[0] != '\0') && (st->spid > 0))
       {al        = MAKE_N(atproc, 2);
	al[i].pid = st->spid;
	nstrncpy(al[i].name, MAXLINE, st->exe, -1);
	i++;}
    else
       {nstrncpy(pids, MAXLINE,
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
	        nstrncpy(al[i].name, MAXLINE,
			 run(FALSE, "rpath %s", t), -1);
	     else
	        break;

	     if (i >= n-2)
	        {n += 10;
		 REMAKE(al, atproc, n);};};};

    al[i].pid     = -1;
    al[i].name[0] = '\0';

    return(al);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SELECT_PID - interactively select the appropriate process */

static atproc *select_pid(atdbgdes *st, atproc *al)
   {int i, ok;
    char resp[MAXLINE];
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

	     fgets(resp, MAXLINE, stdin);
	     ok = (resp[0] == 'y');};

	 if (ok == TRUE)
	    break;

       if (st->verbose == TRUE)
	  {printf("Debugger:   %s", st->dbg);
	   printf("Executable: %s", pal->name);
	   printf("PID:        %d", pal->pid);};};

    if (ok == FALSE)
       pal = NULL;

    return(pal);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEND_MSG - prepare and send a message to PP */

static int send_msg(process *pp, int act, char *fmt, ...)
   {int nl;
    char msg[MAXLINE];
    char *p;
    dbgrsp *gr;
    atdbgdes *st;

    VA_START(fmt);

    if (fmt == NULL)
       p = NULL;
    else
       {VSPRINTF(msg, fmt);
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

static void cache_rsp(atdbgdes *st, char *s)
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
    char r[MAXLINE];
    char *s, **sa;

    n  = st->nmsg;
    sa = st->msgs;

/* print all the messages */
    for (i = 0; i < n; i++)
        {s = sa[i];

	 strclean(r, MAXLINE, s, -1);

	 puts(r);

         FREE(s);};

/* reset */
    FREE(st->msgs);
    st->nmsg = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RSP_ERROR - handle errors such as timeouts or poll rejects */

static int rsp_error(process *pp, char *t)
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

static int rsp_totalview(process *pp, char *t)
   {int ok, nc;
    char p[MAXLINE];
    char *s, *pa, *pb, *pr;
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
                 if (t[0] != '\0')
		    nstrncpy(gr->result, MAXLINE, t, -1);
	         break;

	    case BTRACE:
		 if (strstr(t, "PC=") != NULL)
		    {nstrncpy(p, MAXLINE, t, -1);
		     pa = strstr(p, "PC=");
		     pb = strchr(p, '[');
		     memmove(pa, pb, strlen(pb)+1);
		     s = p;
		     s = subst(s, "PC=", " ", 1);
		     s = subst(s, "> ", "  ", 1);
		     s = subst(s, "#", ":", 1);
		     cache_rsp(st, s);
		     if (strstr(s, "SC_retrace_exe") != NULL)
		        cache_rsp(st, "         -+---------------------------+-");};
		 break;};};

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
    char cmd[MAXLINE], s[MAXLINE];
    process *pp;
    dbgrsp gr;

    rv = TRUE;

    if (st->mode == TRACE)
       {gr.prompt = "d1.<> ";
	gr.st     = st;

	snprintf(cmd, MAXLINE, "%scli", st->dbg);
	pp = job_launch(cmd, "a", NULL);

	pp->accept = rsp_totalview;
	pp->reject = rsp_error;
	pp->a      = &gr;

	send_msg(pp, 0, NULL);
#if 0
        gr.prompt = "tvcli> ";
	send_msg(pp, 0, "dset PROMPT {tvcli> }");
#endif
	send_msg(pp, 0, "dset LINES_PER_SCREEN 0");
	send_msg(pp, 0, "dattach -no_attach_parallel %s %d", al->name, al->pid);
	send_msg(pp, 0, "dhalt");
/*	send_msg(pp, 0, "dwait"); */
	send_msg(pp, QTHREAD, "f p1 TV::thread get -all id");

	FOREACH(thr, gr.result, " \n");
           snprintf(s, MAXLINE,
		    "\n   ---------------- Thread %s ----------------\n", thr);
	   cache_rsp(st, s);
	   send_msg(pp, BTRACE, "dfocus %s {dwhere -a}", thr);
        ENDFOR;

/*	send_msg(pp, 0, "ddetach"); */

	nl = send_msg(pp, 0, "exit -force");

	sig = (nl == -1) ? SIGKILL : -1;
	job_done(pp, sig);}

    else
       run(FALSE, "%s %s -pid %d", st->dbg, al->name, al->pid);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RSP_GDB - handle lines from GDB */

static int rsp_gdb(process *pp, char *t)
   {int ok, nc;
    char p[MAXLINE];
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
		 if (*u == '*')
		    u = strtok(NULL, " \t\n");
                 push_tok(gr->result, MAXLINE, ' ', u);
	         break;

	    case BTRACE:
		 if (t[0] == '#')
		    {snprintf(p, MAXLINE, "%s]", t+1);
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

static int use_gdb(atdbgdes *st, char *fname, atproc *al)
   {int rv, nl, sig;
    char cmd[MAXLINE], s[MAXLINE];
    process *pp;
    dbgrsp gr;

    rv = TRUE;

    if (st->mode == TRACE)
       {gr.prompt    = "(gdb) ";
	gr.result[0] = '\0';
	gr.st        = st;

	snprintf(cmd, MAXLINE, "%s %s", st->dbg, al->name);
	pp = job_launch(cmd, "a", NULL);

	pp->accept = rsp_gdb;
	pp->reject = rsp_error;
	pp->a      = &gr;

	send_msg(pp, 0, NULL);
	send_msg(pp, 0, "set width -1");
	send_msg(pp, 0, "set height -1");
	send_msg(pp, 0, "set confirm off");
	send_msg(pp, 0, "attach %d", al->pid);
	send_msg(pp, QTHREAD, "info threads");

	FOREACH(thr, gr.result, " \n")
           snprintf(s, MAXLINE,
		    "\n   ---------------- Thread %s ----------------\n", thr);
	   cache_rsp(st, s);
           send_msg(pp, 0, "thread %s", thr);
   	   send_msg(pp, BTRACE, "backtrace");
        ENDFOR;

	send_msg(pp, 0, "detach");

	nl = send_msg(pp, 0, "quit");

	sig = (nl == -1) ? SIGKILL : -1;
	job_done(pp, sig);}

    else
       run(FALSE, "xterm -sb -e %s %s %d", st->dbg, al->name, al->pid);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USE_DBX - do the work with DBX */

static int use_dbx(atdbgdes *st, char *fname, atproc *al)
   {int rv;
    char *s;
    FILE *fp;

    rv = TRUE;

    if (st->mode == TRACE)
       {fp = fopen(fname, "w");
	fprintf(fp, "attach %d\n", al->pid);
        fprintf(fp, "where\n");
        fprintf(fp, "quit\n");
	fclose(fp);

	FOREACH(t, run(FALSE, "%s %s -c %s", st->dbg, al->name, fname), "\n")
	   s = t;
	   s = subst(s, ">", " ", 1);
	   cache_rsp(st, s);
	ENDFOR;
#if 0
        $dbg $exe -c $Tmp  |&              \
        sed 's/>/ /'      |                \
        awk '$1 ~ /[0-9]+/ { print }';
#endif

	unlink(fname);}

    else
       run(FALSE, "xterm -sb -e %s %s -pid %d", st->dbg, al->name, al->pid);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SESSION - work the session */

static int session(atdbgdes *st)
   {int i, n, rv, zip;
    char fname[MAXLINE], names[MAXLINE];
    char *name, *pl, *lst[32];
    atproc *al, *pal;
    FILE *log;

    rv = TRUE;

    if (st->whdbg == TRUE)
       {printf("%s\n", st->dbg);
        exit(0);};

/* open the log file if requested */
    if (st->logname[0] != '\0')
       {snprintf(fname, MAXLINE, "%s.%d", st->logname, getpid());
	log = fopen(fname, "w");
	if (log != NULL)
	   setbuf(log, NULL);
	st->log = log;};

    if (st->quiet == FALSE)
       printf("\n");

    if (st->mode == TRACE)
       snprintf(fname, MAXLINE, "/tmp/dbg.%d", getpid());

    if ((st->pname[0] == '\0') &&
        ((st->exe[0] != '\0') || (st->spid >= 0)))
       snprintf(st->pname, MAXLINE, "%d", st->spid);

/* make an array of process names */
    nstrncpy(names, MAXLINE, st->pname, -1);
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
       fclose(st->log);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT - setup the session */

static int init(atdbgdes *st)
   {int rv;
    char lst[MAXLINE], t[MAXLINE];

    rv = TRUE;

    unamef(st->os, MAXLINE, "s");
    strcpy(st->opt, "-q");
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
       nstrncpy(st->logname, MAXLINE, cgetenv(TRUE, "ATDBG_LOG"), -1);
    else
       st->logname[0] = '\0';

/* determine which debugger to use */
    if (cdefenv("SC_DEBUGGER") == TRUE)
       {nstrncpy(t, MAXLINE, cgetenv(TRUE, "SC_DEBUGGER"), -1);
        nstrncpy(lst, MAXLINE, subst(t, ":", " ", MAXLINE), -1);}
    else
       nstrncpy(lst, MAXLINE, "totalview gdb dbx", -1);

    FOREACH(i, lst, " \n")
       nstrncpy(t, MAXLINE, cwhich(i), -1);
       if (file_executable(t) == TRUE)
          {nstrncpy(st->dbg, MAXLINE, t, -1);
           break;};
    ENDFOR;

    if (st->dbg[0] == '\0')
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
            {push_tok(st->opt, MAXLINE, ' ', "-r");
	     push_tok(st->opt, MAXLINE, ' ', "none-");}

	 else if (strcmp(v[i], "-e") == 0)
            nstrncpy(st->exe, MAXLINE, v[++i], -1);

	 else if (strcmp(v[i], "-h") == 0)
            help();

	 else if (strcmp(v[i], "-l") == 0)
            nstrncpy(st->logname, MAXLINE, v[++i], -1);

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
	    push_tok(st->pname, MAXLINE, ' ', v[i]);};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int rv;
    char exe[MAXLINE], bindir[MAXLINE];
    atdbgdes state;

#if 0
    setbuf(stdout, NULL);
#endif

/* find the directory with atdbg */
    strcpy(exe, cwhich(v[0]));
    strcpy(bindir, path_head(exe));

/* locate the tools needed for subshells */
    build_path(NULL,
	       "sh", "xterm", "ls-jobs", "rpath",
	       NULL);

    push_path(PREPEND, lpath, bindir);

    rv = init(&state);
    if (rv == TRUE)
       {rv = process_args(&state, c, v);
	rv = session(&state);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

