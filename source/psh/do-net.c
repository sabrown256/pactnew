/*
 * DO-NET.C - C version of do-net
 *          - execute scripts around a list of networks and hosts
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 */

#include "common.h"
#include "libpsh.c"
#include "libtime.c"
#include "libasync.c"

/* exit states */
#undef WAITING
#undef PASSED
#undef FAILED
#undef TIMEDOUT
#define WAITING   10
#define PASSED    11
#define FAILED    12
#define TIMEDOUT  13

/* running states */
#define IDLE       20
#define PENDING    21
#define STARTED    22
#define RUNNING    23
#define DONE       24

#define BY_HOST  100
#define BY_NET   101

#define FLOG  0
#define VLOG  1
#define NLOG  2
#define QLOG  3

#define PH_SETUP        0
#define PH_BUILD        1
#define PH_NETINSTALL   2
#define PH_HOSTINSTALL  3
#define PH_CLEAN        4
#define PH_N_PHASES     5

#define N_AUX 6

#define INIT_PHASE(_p, _i, _s)                                                   \
   {(_p)->id     = _i;                                                           \
    (_p)->name   = _s;                                                           \
    (_p)->nsp    = 0;                                                            \
    (_p)->sp     = NULL;                                                         \
    (_p)->tlimit = 0;                                                            \
    memset((_p)->time, 0, 16);}

typedef struct s_hfspec hfspec;
typedef struct s_auxdes auxdes;
typedef struct s_phasedes phasedes;
typedef struct s_donetdes donetdes;

struct s_hfspec
   {char *delim;
    char *host;
    char *rawh;
    char rawwork[BFLRG];
    char workdir[BFLRG];
    char config[BFLRG];
    char *args[80];
    char logn[BFLRG];
    FILE *log;
    process *proc;
    int running;
    int exit;};

struct s_auxdes
   {char proper[BFLRG];
    char work[BFLRG];
    char perms[10];
    int permi;};

struct s_phasedes
   {int id;
    char *name;
    int nsp;
    hfspec *sp;
    int tlimit;
    char time[16];};

struct s_donetdes
   {int err;
    int finishing;
    int havetimeouts;
    int inintr;
    int check;
    int send;
    int build;
    int install;
    int installwdh;
    int verbose;
    int silent;
    int clearout;
    int reportprogress;
    int watchprogress;
    int watchout;
    int localinstall;
    int testhost;
    int trace;
    int watch;
    int usescp;
    int interact;
    int n_hosts;
    int n_nets;
    int vfyt;
    hfspec *hosts;
    hfspec *nets;
    phasedes phases[PH_N_PHASES];
    auxdes aux[N_AUX];
    char bindir[BFLRG];
    char incdir[BFLRG];
    char etcdir[BFLRG];
    char logdir[BFLRG];
    char lnetfn[BFLRG];
    char ssh[BFLRG];
    char dist[BFLRG];
    char hostfile[BFLRG];
    char do_code[BFLRG];
    char stamp[BFLRG];
    char uplog[BFLRG];
    char handtout[BFLRG];
    char *cargs;
    char *clargs;
    char *debug;
    char *hostonly;
    char *inacc;
    char *mailer;
    char *maillist;
    char *repo;
    char *run;
    char *scripts;
    char *shared;
    char *system;
    char *tag;
    char *usex;
    char *varspecs;
    char **argvcp;
    char *separator;};

static donetdes
 state;

static phasedes
 *current_phase = NULL;

static char
 *run_state[] = { "IDLE", "PENDING", "STARTED", "RUNNING", "DONE" };

static void
 finish(donetdes *st, double gti);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEPARATORV - emit a separator with constraints
 *            - this implements SeparatorV
 */

void separatorv(FILE *fp)
   {

    if (fp != NULL)
       {fprintf(fp, "DO-NET: \n");
	fprintf(fp, "DO-NET: %s\n", state.separator);
	fprintf(fp, "DO-NET: \n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NOTET - write messages preceded with "DO-NET:" and TAG
 *       - this implements NoteVT
 */

void notet(FILE *fp, char *tag, char *fmt, ...)
   {char bf[BFLRG];

    if (state.verbose == TRUE)
       {if (fp != NULL)
	   {VA_START(fmt);
	    VSNPRINTF(bf, BFLRG, fmt);
	    VA_END;

	    fprintf(fp, "DO-NET: (%s) %s\n", tag, bf);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NOTEN - write messages preceded with "DO-NET:"
 *       - this combines Note and NoteV
 */

void noten(FILE *fp, int vrb, char *fmt, ...)
   {char bf[BFLRG];

    if (vrb == TRUE)
       {if (fp != NULL)
	   {VA_START(fmt);
	    VSNPRINTF(bf, BFLRG, fmt);
	    VA_END;

	    fprintf(fp, "DO-NET: %s\n", bf);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NOTEJ - log job control messages in a consistent format */

static void notej(process *pp, char *fmt, ...)
   {int hr, mn, ti;
    double tc, sc;
    char bf[BFLRG];
    hfspec *sp;

    VA_START(fmt);
    VSNPRINTF(bf, BFLRG, fmt);
    VA_END;

    if (pp != NULL)
       {sp = (hfspec *) pp->a;

	tc = wall_clock_time();
	ti = tc;
	hr = ti / 3600;
	mn = (ti - 3600*hr) / 60;
	sc = tc - 60*(mn + 60*hr);

	fprintf(Log, "DO-NET: [Job %06d/%02d:%02d:%06.3f] %s(%s) %s\n",
		pp->id, hr, mn, sc,
		sp->delim, (sp->host != NULL) ? sp->host : sp->rawh,
		bf);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TRANSITION - note the transition in run state to the log */

static int transition(process *pp, int new)
   {int old;
    char *olds, *news;
    hfspec *sp;

    if (pp != NULL)
       {sp = (hfspec *) pp->a;

	old = sp->running;
	olds = run_state[old-IDLE];
	news = run_state[new-IDLE];

	if (new == DONE)
	   notej(pp, "transition from %s to %s (%s)",
		 olds, news, (sp->exit == PASSED) ? "ok" : "ng");
	else
	   notej(pp, "transition from %s to %s", olds, news);};

    return(new);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_HOST_FILE - get the separate elements of S which
 *               - is of the form [<host>:]<file>
 */

static void get_host_file(char *host, char *file, char *s)
   {char *p;

    strcpy(host, s);
    p = strchr(host, ':');
    if (p == NULL)
       {strcpy(file, host);
	host[0] = '\0';}
    else
       {*p++ = '\0';
	strcpy(file, p);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_FIELDS - assemble the fields to be passed as
 *            - an argument to do-local
 */

static void get_fields(char *f, int nc, hfspec *fs)
   {int j;
    char *arg;

    nstrncpy(f, nc, fs->rawh, -1);
    push_tok(f, nc, ' ', fs->rawwork);

    for (j = 0; TRUE; j++)
        {arg = fs->args[j];
	 if (arg == NULL)
	    break;
	 else
	    push_tok(f, nc, ' ', arg);};

    return;}
	   
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* START_TIME - start timer */

static double start_time(void)
   {double t;

    t = wall_clock_time();

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STOP_TIME - stop timer */

static char *stop_time(char *et, int nc, double ti)
   {double df;

    df = wall_clock_time() - ti;

    time_string(et, nc, TIME_HMS, df);

    return(et);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COUNT_SPECS - return the number of specs */

static int count_specs(hfspec *specs, size_t nsp)
   {int n;

    for (n = 0; (n < nsp) && (specs[n].running != -1); n++);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LRUN - execute a command and return the output in a string
 *      - echo the command to the log iff ECHO is TRUE
 *
 *   vlog -> lrun(f1, VLOG, ...)
 *   nlog -> lrun(f1, NLOG, ...)
 *   qlog -> lrun(f1, QLOG, ...)
 *   flog -> lrun(f1, FLOG, ...)
 *
 */

static char *lrun(FILE *f1, int tag, char *fmt, ...)
   {char s[BFLRG], cmd[BFLRG];
    char *p;
    FILE *fp;
    static char bf[100*BFLRG];

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    if (tag == FLOG)
       fprintf(f1, "Command: %s\n", s);
    else if ((state.verbose == TRUE) || (tag != VLOG))
       fprintf(f1, "DO-NET: %s\n", s);

    snprintf(cmd, BFLRG, "PATH=.:%s ; %s", lpath, s);

    bf[0] = '\0';

    fp = popen(cmd, "r");
    if (fp != NULL)
       {while (TRUE)
	   {p = fgets(s, BFLRG, fp);
	    if (p == NULL)
	       break;
	    nstrcat(bf, 100*BFLRG, s);};

	pclose(fp);};

    LAST_CHAR(bf) = '\0';

/* handle output to log */
    if (tag != QLOG)
       fputs(bf, f1);

    return(bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* AEXIT - chores to do when an asynchronous command completes
 *       - this is a JOB_WAIT callback
 */

static void aexit(process *pp)
   {int rv;
    char *st;
    hfspec *sp;

    sp = (hfspec *) pp->a;

    switch (pp->status)
       {case JOB_RUNNING :
	     st = "run";
	     break;
	case JOB_STOPPED :
	     st = "stop";
	     break;
	case JOB_CHANGED :
	     st = "chng";
	     break;
	case JOB_EXITED :
	     st = "exit";
	     break;
	case JOB_COREDUMPED :
	     st = "core";
	     break;
	case JOB_SIGNALED :
	     st = "sgnl";
	     break;
	case JOB_KILLED :
	     st = "kill";
	     break;
	case JOB_DEAD :
	     st = "dead";
	     break;
	default :
	     st = "unk";
	     break;};

    if (sp->running < STARTED)
       {notej(pp, "restart (%s/%d)", st, pp->reason);
	pp->status |= JOB_RESTART;}
    else
       notej(pp, "exited (%s/%d)", st, pp->reason);

/* drain text from the job - apoll will no longer check it after this */
    rv = job_read(-1, pp, pp->accept);
    ASSERT(rv == 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FINUP - finish up the job PP
 *       - this is an AFIN callback
 */

static void finup(process *pp, void *a)
   {int st, ok, err;
    hfspec *sp;

    sp = (hfspec *) a;

    job_read(-1, pp, pp->accept);
        
/* since we are done with it kill the job if it is still running */
    if (job_running(pp))
       {notej(pp, "terminated");
	job_done(pp, SIGTERM);

	st = TIMEDOUT;}

/* otherwise its exit status determines the state in sp */
    else
       {ok = ((pp != NULL) && (pp->reason == 0));
	if (sp->exit == WAITING)
	   st = (ok == TRUE) ? PASSED : FAILED;
	else
	   st = sp->exit;
	notej(pp, "completed (%s)", (ok == TRUE) ? "ok" : "ng");};

/* close the log */
    if (sp->log != NULL)
       {err = fclose(sp->log);
        if (err == 0)
	   notej(pp, "successfully closed log %s", sp->logn);
	else
	   notej(pp, "failed to close log %s", sp->logn);};

    sp->running = transition(pp, IDLE);
    sp->exit    = st;
    sp->log     = NULL;

    return;}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* BAD_BUILD - forge a build log for a bad build */

static void bad_build(hfspec *sp)
   {char fields[BFLRG];
    char *ltmp, *lcfg, *lopt, *file, *host;
    FILE *fp;

    host = sp->host;
    file = sp->logn;
    ltmp = sp->args[2];
    lcfg = sp->args[4];
    lopt = sp->args[5];
    get_fields(fields, BFLRG, sp);

    fp = open_file("w", file);

    note(fp, TRUE, "Starting build/test on %s (%s-%s) at `date`",
	 host,  host, lcfg);
    note(fp, TRUE, " ");
    note(fp, TRUE, "DO-NET: configure %s option %s %s",
	 lcfg, lopt, ltmp);
    note(fp, TRUE, "   Configuring from %s using %s",
	 lcfg, lopt);
    note(fp, TRUE, "   Removing ... OK");
    note(fp, TRUE, "   Building ... OK (0:00)");
    note(fp, TRUE, "   Testing .... OK (0:00)");
    note(fp, TRUE, " ");
    note(fp, TRUE, "Failed on %s (0:00)", host);

/* NOTE: this is a diagnostic to debug the above assignment of ltmp, lcfg, and lopt */
    note(fp, TRUE, " ");
    note(fp, TRUE, "HFields = |%s|", fields);

    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BAD_SECT - forge a log file for a bad SECT */

static void bad_sect(hfspec *sp, char *sect)
   {char *host, *file;
    FILE *fp;

    host = sp->host;
    file = sp->logn;

    fp = open_file("w", file);

    note(fp, TRUE, "   %s ................. FAILED (0:00)", host);
    note(fp, TRUE, "Failed on %s (0:00)", host);

    fclose(fp);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* LOG_IN - log input messages from the process */

static int log_in(process *pp, char *ar, char *s)
   {int rv;
    char *p;
    hfspec *sp;
    FILE *fo;

    rv = TRUE;

    sp = (hfspec *) pp->a;
    if (sp != NULL)
       {fo = sp->log;

	if (strstr(s, "Succeeded on ") != NULL)
	   {sp->exit    = PASSED;
	    sp->running = transition(pp, DONE);}

	else if (strstr(s, "Failed on ") != NULL)
	   {sp->exit    = FAILED;
	    sp->running = transition(pp, DONE);}

	else if (strncmp(s, "DO-NET: Exec do-local", 21) == 0)
	   sp->running = transition(pp, STARTED);

	else if (strncmp(s, "Starting", 8) == 0)
	   sp->running = transition(pp, RUNNING);

	else if (strncmp(s, "DO-NET: configure", 17) == 0)
	   {nstrncpy(sp->config, BFLRG, s+18, -1);
	    p = strchr(sp->config, ' ');
	    if (p != NULL)
	       *p = '\0';}

#if 0
	else if (strstr(s, "log/auto:") != NULL)
	   bad_build(sp);

	else if (strstr(s, "log/") != NULL)
	   {p = current_phase->name;
	    if (strncmp(s+4, p, strlen(p)) == 0)
	       bad_sect(sp, p);};
#endif

/* copy it to the log file if open */
	if (fo != NULL)
	   fputs(s, fo);

/* use the log file name as a buffer if no log file is open */
	else
           {if (LAST_CHAR(s) == '\n')
	       LAST_CHAR(s) = '\0';
	    notej(pp, "text(%s)", s);
	    if (sp->running < RUNNING)
	       sp->running = transition(pp, RUNNING);
	    nstrncpy(sp->logn, BFLRG, s, -1);
	    sp->logn[BFLRG-1] = '\0';};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REJ_IN - read from pipe in SPEC */

static int rej_in(int fd, process *pp, char *s)
   {int rv;

    rv = log_in(pp, "reject", s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ACC_IN - read from pipe in SPEC */

static int acc_in(int fd, process *pp, char *s)
   {int rv;

    rv = log_in(pp, "accept", s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ARUN - execute a command asynchronously */

static process *arun(hfspec *sp, char *fnm, void *a, char *fmt, ...)
   {char s[BFLRG], cmd[BFLRG];
    process *pp;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    if ((state.verbose == TRUE) && (Log != NULL))
       fprintf(Log, "DO-NET: %s\n", s);

    snprintf(cmd, BFLRG, "%s", s);

    pp = alaunch(-1, cmd, "r", sp, acc_in, rej_in, aexit);

    sp->running   = transition(pp, PENDING);
    sp->exit      = WAITING;
    sp->proc      = pp;
    sp->config[0] = '\0';

    if (fnm != NULL)
       {nstrncpy(sp->logn, BFLRG, fnm, -1);
        sp->log = open_file("w+", fnm);
	if (sp->log != NULL)
	   {setbuf(sp->log,  NULL);
	    notej(pp, "successfully opened log %s", fnm);}
	else
	   notej(pp, "failed to open log %s", fnm);}
    else
       {sp->log     = NULL;
	sp->logn[0] = '\0';};

    apoll(1);

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BRUN - execute a command in the background and
 *      - return the output in a string
 *      - no input expected
 */

static void brun(FILE *fo, char *fmt, ...)
   {char s[BFLRG], cmd[BFLRG];
    FILE *fp;

    VA_START(fmt);
    VSNPRINTF(s, BFLRG, fmt);
    VA_END;

    fprintf(fo, "DO-NET: %s\n", s);

    snprintf(cmd, BFLRG, "PATH=.:%s ; %s", lpath, s);

    fp = popen(cmd, "r");
    ASSERT(fp != NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IS_BACKGROUND - determine whether the process PID is
 *               - running in the background
 *               - if PID is -1 check the current process
 *               - the return value is a bit array
 *               -   set bit 1 if PID is different from the
 *               -   process group leader
 *               -   set bit 2 if PID is different from the
 *               -   terminal group ID
 */

int is_background(void)
   {int rv, pid, ppid, pgid, ptid;

    pid  = getpid();
    ppid = getppid();
    pgid = getpgrp();
    ptid = tcgetpgrp(fileno(stdin));

    rv = 0;

/* set bit 2 if terminal group id is different from the pid */
    if (pid != ptid)
       rv |= 2;

/* set bit 1 if the parent is init or the process group id
 * is different from the pid
 */
    if ((ppid == 1) || (pid != pgid))
       rv |= 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_TTY - check for messages from the tty
 *           - this is an AWAIT callback
 */

static int check_tty(char *sect)
   {int rv;
    char in[BFLRG];
    char *p;

    rv = FALSE;

    if (state.interact == TRUE)
       {if (is_background() == FALSE)

/* do a non-blocking read to see if a message is waiting */
	   {block_fd(fileno(stdin), FALSE);
            p = fgets(in, BFLRG, stdin);
	    block_fd(fileno(stdin), TRUE);

            if (p != NULL)
	       {LAST_CHAR(in) = '\0';
		if (strcmp(in, "done") == 0)
		   {printf("Phase %s declared done\n", sect);
		    printf("   %s ...", sect);
		    rv = TRUE;}

	        else if (strcmp(in, "help") == 0)
		   {printf("\n");
		    printf("Options:\n");
		    printf("  done   declare the current phase done\n");
		    printf("  help   this help message\n");
		    printf("\n");
		    printf("   %s ...", sect);}

		else
		   {printf("\n");
		    printf("Option ignored: %s\n", in);
		    printf("   %s ...", sect);};};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_LIST - print the list of items
 *            - so that they fit nicely on lines
 */

static void print_list(hfspec *sp, int nsp, int rptcfg, int run, int ex)
   {int i, j, ok;
    char *s;
    hfspec *lsp;

    for (i = 0, j = 0, lsp = sp; i < nsp; i++, lsp++)
        {if (((run == RUNNING) &&
	      (IDLE < lsp->running) && (lsp->running < DONE)) ||
	     (lsp->exit == ex))
	    {j++;
             ok = ((rptcfg == TRUE) && (IS_NULL(lsp->config) == FALSE));
	     s  = (ok == TRUE) ? lsp->config : lsp->host;
	     if (j % 4 == 0)
	        note(Log, FALSE, "\n   %-16s", s);
	     else
	        note(Log, FALSE, "   %-16s", s);};};

    note(Log, TRUE, "");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_REPORT -  report success and failure */

static int make_report(donetdes *st, hfspec *sp, int nsp, int rpt)
   {int i, ok, np, nf, nt, nl;
    char *prep, *noun;
    hfspec *lsp;
    static int rptcfg = -1;

    if (rptcfg == -1)
       rptcfg = (strcmp(cgetenv(FALSE, "ReportConfig"), "yes") == 0);

    if (rptcfg == TRUE)
       {prep = "for";
	noun = "configurations";}

    else
       {prep = "on";
	noun = "hosts";};

    np = 0;
    nf = 0;
    nt = 0;
    nl = 0;

/* gather information */
    for (i = 0, lsp = sp; i < nsp; i++, lsp++)
        {switch (lsp->exit)
	    {case PASSED:
	          np++;
		  break;

	     case FAILED:
		  nf++;
		  ok = FALSE;
		  break;

	     case TIMEDOUT:
		  nt++;
		  ok = FALSE;
		  break;

	     default:
		  nl++;
		  ok = FALSE;
		  break;};};

    ok = (np == nsp);

/* make the report if requested */
    if (rpt == TRUE)

/* report passed */
       {if (np != 0)
	   {if ((nl == 0) && (nt == 0) && (nf == 0))
	       note(Log, TRUE, "Succeeded %s ALL %s:", prep, noun);
	    else
	       note(Log, TRUE, "Succeeded %s %s:", prep, noun);

	    print_list(sp, nsp, rptcfg, DONE, PASSED);};

/* report timedout */
	if (nt != 0)
	   {if (strcmp(st->handtout, "pass") != 0)
	       {ok               = (np + nt == nsp);
		st->havetimeouts = TRUE;};

	    note(Log, TRUE, "Timed out %s %s:", prep, noun);
	    print_list(sp, nsp, rptcfg, DONE, TIMEDOUT);};

/* report failed */
        if (nf != 0)
	   {note(Log, TRUE, "Failed %s %s:", prep, noun);
	    print_list(sp, nsp, rptcfg, DONE, FAILED);};

/* report lost */
	if (nl != 0)
	   {note(Log, TRUE, "Lost %s:", noun);
	    print_list(sp, nsp, rptcfg, RUNNING, WAITING);};

	note(Log, TRUE, "");};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WAITUP - while waiting for jobs to finish gather input
 *        - this is an AWAIT callback
 */

static void waitup(int cyc, char *sect, void *a,
		   int nd, int np, int tc, int tf)
   {int pc;
    static int pcl;

    if (cyc == 0)
       pcl = 0;

/* do logging chores every 5% of the time interval tf */
    pc = 100*tc/tf;
    if (pc >= pcl)
       {pcl += 5;
	if (state.silent == FALSE)
	   printf(".");
	noten(Log, state.verbose,
	      "Jobs %3d of %3d  and  time %5d of %5d sec",
	      nd, np, tc, tf);};

/* NOTE: for many do-nets in the build phase there seems to be
 * enough work to keep the do-net using the entire CPU
 * so add delay here to keep it running at a low level
 */
    sleep(1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CWAIT - wait for parallel jobs to finish
 *       - Input:
 *       -    SP      the host or net specs
 *       -    NSP     the number of host or net specs
 *       -    SECT    the phase being monitored
 *       -    TF      the total time to wait before declaring timout
 */

static void cwait(donetdes *st, hfspec *sp, int nsp, char *sect, int tf)
   {int tc;
    char t[BFLRG];

    if (st->silent == FALSE)
       printf("   %s .", sect);

/* log the start */
    noten(Log, st->verbose, "Waiting on %s: Jobs %d  and  time %d sec",
	  sect, nsp, tf);

/* check the jobs until they are all done, or time is exhausted,
 * or we get an interrupt from the terminal
 */
    tc = await(tf, 1000, sect, check_tty, waitup, sp);

/* close out the jobs */
    afin(finup);

/* log the end */
    noten(Log, st->verbose,
	  "Waiting done on %s: %5d sec", sect, tc);

    if (st->silent == FALSE)
       printf(" done (%s)\n", time_string(t, BFLRG, TIME_HMS, tc));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHARSET - fill S with N CHRs */

static void charset(char *s, int nc, int chr, int n)
   {

    memset(s, chr, nc);

    if (n < 0)
       n = 0;

    s[n] = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FINDRUN - setup st->do_code and st->run */

static void findrun(donetdes *st)
   {char exed[BFLRG], locd[BFLRG];
    auxdes *pa;

    pa = st->aux;

/* check that the script exists */
    if (file_exists(st->do_code) == FALSE)
       {printf("\n");
	printf("The script %s specified in\n", st->do_code);
	printf("host file %s not found\n", st->hostfile);
	printf("\n");
	exit(4);};

    if (strcmp(st->shared, cgetenv(FALSE, "HOME")) == 0)
       {snprintf(locd, BFLRG, "./%s", pa[0].work);
	nstrncpy(exed, BFLRG, pa[1].work, -1);}

    else
       {snprintf(locd, BFLRG, "%s/%s", st->shared, pa[0].work);
	snprintf(exed, BFLRG, "%s/%s", st->shared, pa[1].work);};

    st->run = NULL;
    st->run = append_tok(st->run, ' ', locd);
    st->run = append_tok(st->run, ' ', exed);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SET_PHASES - set the hfspecs in phases */

static void set_phases(donetdes *st)
   {int ip, np;
    phasedes *ph;

    ph = st->phases;
    np = PH_N_PHASES;
    for (ip = 0; ip < np; ip++)
        {if (ph[ip].id == BY_NET)
	    {ph[ip].sp  = st->nets;
	     ph[ip].nsp = st->n_nets;}
	 else if (ph[ip].id == BY_HOST)
	    {ph[ip].sp  = st->hosts;
	     ph[ip].nsp = st->n_hosts;};};

    return;}

/*--------------------------------------------------------------------------*/

/*                           WATCH/PROGRESS ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* POP_TOK - pop N tokens off of S */

static char *pop_tok(char *s, int n, char *delim)
   {int i;
    char *p, *ps;

    p = NULL;

    for (i = 0, ps = s; i < n; i++, ps = NULL)
        p = strtok(ps, delim);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WATCH_BUILD - handle watch chores for build phase */

static int watch_build(char *plog, FILE *repf, char *pass, char *fail,
		       char *wrk, char *skip)
   {int cfgfail, running;
    char s[BFLRG], hst[BFLRG], rmh[BFLRG];
    char cfg[BFLRG], dbop[BFLRG], usetmp[BFLRG];
    char tss[BFLRG], fns[BFLRG], fnt[BFLRG];
    char blt[BFLRG], tst[BFLRG], rms[BFLRG], bls[BFLRG];
    char *p, *o;
    FILE *fp;

    running   = TRUE;
    cfgfail   = FALSE;
    strcpy(hst, "--- pending ---");
    strcpy(cfg, "---");
    rmh[0]    = '\0';
    dbop[0]   = '\0';
    usetmp[0] = '\0';
    tss[0]    = '\0';
    fns[0]    = '\0';
    fnt[0]    = '\0';
    blt[0]    = '\0';
    tst[0]    = '\0';
    rms[0]    = '\0';
    bls[0]    = '\0';

/* open the phase log file */
    fp = fopen(plog, "r");
    if (fp == NULL)
       return(running);

/* read the phase log file */
    while (TRUE)
      {p = fgets(s, BFLRG, fp);
       if (p == NULL)
	  break;
       else if (strstr(s, "Failed to configure") != NULL)
	  cfgfail = TRUE;
       else
	  {p = strtok(s, " \t\n");
	   if (p == NULL)
	      continue;
	   else if (strcmp(p, "Removing") == 0)
	      {p = pop_tok(NULL, 2, " \t\n");
	       if (p != NULL)
		  {nstrncpy(rmh, BFLRG, "have", -1);
		   nstrncpy(rms, BFLRG, p, -1);};}
	   else if (strcmp(p, "Building") == 0)
	      {p = pop_tok(NULL, 2, " \t\n");
	       if (p != NULL)
		  nstrncpy(bls, BFLRG, p, -1);
	       p = pop_tok(NULL, 1, " \t\n");
	       if (p != NULL)
		  nstrncpy(blt, BFLRG, p, -1);}
	   else if (strcmp(p, "Testing") == 0)
	      {p = pop_tok(NULL, 2, " \t\n");
	       if (p != NULL)
		  nstrncpy(tss, BFLRG, p, -1);
	       p = pop_tok(NULL, 1, " \t\n");
	       if (p != NULL)
		  nstrncpy(tst, BFLRG, p, -1);}
	   else if (strcmp(p, "Starting") == 0)
	      {p = pop_tok(NULL, 3, " \t\n");
	       if (p != NULL)
		  nstrncpy(hst, BFLRG, p, -1);}
	   else if (strcmp(p, "Succeeded") == 0)
	      {running = FALSE;
	       p = pop_tok(NULL, 3, " \t\n");
	       if (p != NULL)
		  {nstrncpy(fns, BFLRG, "Pass", -1);
		   nstrncpy(fnt, BFLRG, p, -1);};}
	   else if (strcmp(p, "Failed") == 0)
	      {running = FALSE;
	       p = pop_tok(NULL, 3, " \t\n");
	       if (p != NULL)
		  {nstrncpy(fns, BFLRG, "Fail", -1);
		   nstrncpy(fnt, BFLRG, p, -1);};}
	   else if (strcmp(p, "DO-NET:") == 0)
	      {p = pop_tok(NULL, 1, " \t\n");
               if ((p != NULL) && (strcmp(p, "Exec") == 0))
		  strcpy(hst, "--- started ---");
	       if ((p != NULL) && (strcmp(p, "configure") == 0))
		  {o = pop_tok(NULL, 1, " \t\n");
		   p = pop_tok(NULL, 1, " \t\n");
		   if ((p != NULL) && (strcmp(p, "option") == 0))
		      {if (o != NULL)
			  nstrncpy(cfg, BFLRG, o, -1);
		       p = pop_tok(NULL, 1, " \t\n");
		       if (p != NULL)
			  nstrncpy(dbop, BFLRG, p, -1);
		       p = pop_tok(NULL, 1, " \t\n");
		       if (p != NULL)
			  nstrncpy(usetmp, BFLRG, p, -1);};};};};};

    fclose(fp);

/* if the log file indicates the job is pending or has only started */
    if ((strcmp(hst, "--- pending ---") == 0) ||
	(strcmp(hst, "--- started ---") == 0))
       {nstrncpy(s, BFLRG, plog, -1);
	p = pop_tok(s, 2, ".");
	fprintf(repf,
		" %-12s %s       ---   ---  ---      ---\n",
		p, hst);}
       
/* if the log file indicates a running or complete job */
    else
       {if (strcmp(usetmp, "yes") == 0)
	   nstrcat(cfg, BFLRG, "*");

/* handle simple case of config failure */
	if (cfgfail == TRUE)
	   {nstrncpy(rms, BFLRG, skip, -1);
	    nstrncpy(bls, BFLRG, skip, -1);
	    nstrncpy(blt, BFLRG, skip, -1);
	    nstrncpy(tss, BFLRG, skip, -1);
	    nstrncpy(tst, BFLRG, skip, -1);}

	else

/* handle remove entry */
	   {if (strcmp(rmh, "have") == 0)
	       {if (strcasecmp(rms, "ok") == 0)
		   nstrncpy(rms, BFLRG, pass, -1);
	        else if (strcasecmp(rms, "failed") == 0)
		   nstrncpy(rms, BFLRG, fail, -1);
		else
		   nstrncpy(rms, BFLRG, wrk, -1);}
	    else
	       nstrncpy(rms, BFLRG, skip, -1);
   
/* handle build entry */
	    if (strcasecmp(bls, "ok") == 0)
	       nstrncpy(bls, BFLRG, pass, -1);
	    else if (strcasecmp(bls, "failed") == 0)
	       nstrncpy(bls, BFLRG, fail, -1);
	    else if ((strcmp(rms, pass) == 0) || (strcmp(rms, skip) == 0))
	       {nstrncpy(bls, BFLRG, wrk, -1);
		nstrncpy(blt, BFLRG, wrk, -1);};
   
/* handle test entry */
	    if (strcasecmp(tss, "ok") == 0)
	       nstrncpy(tss, BFLRG, pass, -1);
	    else if (strcasecmp(tss, "failed") == 0)
	       nstrncpy(tss, BFLRG, fail, -1);

/* allow the do-<specific> script to override test failure via PassTests
 * and denote it with the '*'
 */
	    else if (strcasecmp(tss, "failed*") == 0)
	       snprintf(tss, BFLRG, "%s*", fail);

	    else if (strcmp(bls, pass) == 0)
	       {nstrncpy(tss, BFLRG, wrk, -1);
		nstrncpy(tst, BFLRG, wrk, -1);}

	    else if (strcmp(bls, fail) == 0)
	       {nstrncpy(tss, BFLRG, skip, -1);
		nstrncpy(tst, BFLRG, skip, -1);};
   
	    if ((IS_NULL(tss) == TRUE) || (strcmp(tss, wrk) == 0))
	       {fns[0] = '\0';
		fnt[0] = '\0';};};
   
	fprintf(repf, " %-12s %-22s %-3s  %-4s ", hst, cfg, dbop, rms);
	fprintf(repf, "%-4s %7s   ", bls, trim(blt, BOTH, " ()"));
	fprintf(repf, "%-4s %7s   ", tss, trim(tst, BOTH, " ()"));
	fprintf(repf, "%-4s %7s\n",  fns, trim(fnt, BOTH, " ()"));};

    return(running);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WATCH_SECT - handle watch chores for phase SECT */

static int watch_sect(char *plog, FILE *repf, char *sect,
		      char *pass, char *fail, char *wrk, char *skip)
   {int i, running;
    char s[BFLRG], hst[BFLRG], sts[BFLRG], tim[BFLRG], fmt[BFLRG];
    char *p, *pf;
    FILE *fp;

    running = TRUE;

    nstrncpy(sts, BFLRG, skip, -1);
    nstrncpy(tim, BFLRG, skip, -1);

/* open the phase log file */
    fp = fopen(plog, "r");
    if (fp == NULL)
       return(running);

/* host derives from phase log name */
    p = strchr(plog, '.');
    nstrncpy(hst, BFLRG, p+1, -1);
    p = strchr(hst, '.');
    if (p != NULL)
       *p = '\0';

/* read the phase log file */
    while (TRUE)
      {p = fgets(s, BFLRG, fp);
       if (p == NULL)
	  break;
       else if (strncmp(s, "DO-NET: Exec do-local", 21) == 0)
	  {nstrncpy(sts, BFLRG, wrk, -1);
	   nstrncpy(tim, BFLRG, wrk, -1);}
       else if ((strncmp(s, "Succeeded", 9) == 0) ||
		(strncmp(s, "Failed", 6) == 0))
	  running = FALSE;
       else
	  {p = strtok(s, " \t\n");
	   if (p == NULL)
	      continue;

	   else if ((strcmp(p, "DO-NET:") != 0) &&
		    (strcmp(p, "Look") != 0))
	      {nstrncpy(hst, BFLRG, p, -1);
	       p = strtok(NULL, " \t\n");
	       p = strtok(NULL, " \t\n");
	       if (p != NULL)
		  nstrncpy(sts, BFLRG, p, -1);
	       p = strtok(NULL, " \t\n");
	       if (p != NULL)
		  nstrncpy(tim, BFLRG, trim(p, BOTH, " ()"), -1);};};};
    fclose(fp);

/* figure out the status and timing */
    if (strcasecmp(sts, "ok") == 0)
       nstrncpy(sts, BFLRG, pass, -1);
    else if (strcasecmp(sts, "failed") == 0)
       nstrncpy(sts, BFLRG, fail, -1);

/* figure out the format for printing */
    nstrncpy(fmt, BFLRG, " %-16s %-4s", -1);
    pf = fmt + strlen(fmt);
    for (i = strlen(sect) - 3; i >= 0; i--, *pf++ = ' ');
    *pf = '\0';
    nstrcat(fmt, BFLRG, "%7s\n");

    fprintf(repf, fmt, hst, sts, tim);

    return(running);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WATCH_PHASE - do the wath work for phase SECT
 *             - increment and return the number of running jobs
 */

static int watch_phase(donetdes *st, FILE *repf,
		       char *plog, int nr, char *sect,
		       char *pass, char *fail, char *skip, char *wrk)
   {

    if (strncmp(plog, sect, strlen(sect)) == 0)

/* process build phase in a special way */
       {if (strcmp(sect, "build") == 0)
	   nr += watch_build(plog, repf, pass, fail, wrk, skip);

/* process all other phases */
	else
	   nr += watch_sect(plog, repf, sect, pass, fail, wrk, skip);};

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WATCH_HEADER - emit the watch page header info */

static void watch_header(donetdes *st, int ip, FILE *repf, char *file,
			 int tr, char *shm, char *ehm, char *clr)
   {int nsp, visual;
    char hyphens[BFLRG], blanks[BFLRG], tm[BFLRG];
    char *tlft, *sect;

    sect = st->phases[ip].name;
    nsp  = st->phases[ip].nsp;

/* NOTE: this works on Linux at least */
    visual = TRUE;

    if (visual == TRUE)
       {nstrncpy(clr, 80, "[H[2J", -1);   /* clear screen */
	nstrncpy(shm, 80, "[4m", -1);       /* underline */
	nstrncpy(shm, 80, "[7m", -1);       /* standout */
	nstrncpy(shm, 80, "[1m", -1);       /* bold */
	nstrncpy(ehm, 80, "[m", -1);}
    else
       {nstrncpy(clr, 80, "", -1);
	nstrncpy(shm, 80, "", -1);
	nstrncpy(ehm, 80, "", -1);};

    if (tr >= 0)
       tlft = time_string(tm, BFLRG, TIME_HMS, tr);
    else
       tlft = "00:00";

/* setup the header of the watch page */
    note(repf,
	 TRUE, "%s Phase      Hosts  Time Left  Time Stamp   Host File%s",
	 shm, ehm);
    if (visual != TRUE)
       note(repf, TRUE, " -----   -----  ---------  ----------   ---------");

    note(repf, TRUE, " %-12s %3d   %8s  %-11s  %s",
	 sect, nsp, tlft, st->stamp, file);
    note(repf, TRUE, "");

    if (strcmp(sect, "build") == 0)
       {note(repf, TRUE,
	     "%s Host         Config                D/O Remove Build   Time   Test    Time   Final   Time%s",
	     shm, ehm);
	if (visual != TRUE)
	   note(repf, TRUE, " ----         ------                --- ------ ------------   ------------   ------------");}

    else
       {note(repf, TRUE, "%s Host             %s     Time%s", shm, sect, ehm);

	charset(blanks,  BFLRG, ' ', strlen(sect)-3);
	charset(hyphens, BFLRG, '-', strlen(sect)-7);

	if (visual != TRUE)
	  note(repf, TRUE, " ----       %s", hyphens);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WATCH_EMIT - emit the watch page */

static void watch_emit(donetdes *st, char *repfn, char *file,
		       char *shm, char *ehm, char *clr)
   {char s[BFLRG];
    char *p, *pe, *ps, *pc;
    FILE *htmlf, *fp;

    if (st->watchout == TEXT)
       {printf("%s\n", clr);
	cat(stdout, 0, -1, repfn);}

    else if (st->watchout == HTML)
       {htmlf = open_file("w", "%s/%s.html", st->shared, path_tail(file));
	note(htmlf, TRUE,  "<html>");
	note(htmlf, TRUE,  "<head>");
	note(htmlf, TRUE,  "<title>DO-NET %s</title>", path_tail(file));
	note(htmlf, TRUE,  "</head>");
	note(htmlf, TRUE,  "<body>");
	note(htmlf, TRUE,  "<pre>");

	fp = fopen(repfn, "r");
	if (fp == NULL)
	   {fclose(htmlf);
	    return;};

	while (TRUE)
	   {p = fgets(s, BFLRG, fp);
	    if (p == NULL)
	       break;
	    else
	       {ps = (IS_NULL(shm) == FALSE) ? strstr(s, shm) : NULL;
		pe = (IS_NULL(ehm) == FALSE) ? strstr(s, ehm) : NULL;
		pc = (IS_NULL(clr) == FALSE) ? strstr(s, clr) : NULL;

		if (ps != NULL)
		   subst(s, shm, "<b color=\"#0000ff\">", -1);
		if (pe != NULL)
		   subst(s, ehm, "</b>", -1);
		if (pc != NULL)
		   subst(s, clr, "", -1);

		note(htmlf, FALSE, s);};};
	fclose(fp);

	note(htmlf, TRUE,  "</pre>");
	note(htmlf, TRUE,  "</body>");
	note(htmlf, TRUE,  "</html>");
	fclose(htmlf);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WATCH - continuous monitor of do-net session */

static int watch(donetdes *st, int c, char **v)
   {int i, ip, tr, nr, ok;
    char file[BFLRG], upf[BFLRG], repfn[BFLRG], dir[BFLRG];
    char shm[80], ehm[80], clr[80];
    char *f, *rv, *phase;
    char *wrk, *skip, *pass, *fail;
    char **plog, **files;
    FILE *repf;

    full_path(file, BFLRG, NULL, v[1]);

/* find out the time stamp */
    if (c > 2)
       nstrncpy(st->stamp, BFLRG, v[2], -1);
    else
       {char lnm[BFLRG];

        st->stamp[0] = '\0';
        nstrncpy(lnm, BFLRG, path_tail(file), -1);
	files = ls("-t", "%s/*/update", st->logdir);
	if (files != NULL)
	   {for (i = 0; files[i] != NULL; i++)
	        {f  = files[i];
		 rv = run(FALSE, "grep \"Updating\" %s | awk '$2 == \"%s\" && $7 ~ /'%s'/ {print \"yes\"}'",
			  f, st->system, lnm);
		 if (strcmp(rv, "yes") == 0)
		    {nstrncpy(st->stamp, BFLRG, path_tail(path_head(f)), -1);
		     break;};};
	    free_strings(files);};

	if (IS_NULL(st->stamp) == TRUE)
	   {printf("\n");
	    printf("No update log matching %s and %s - exiting\n",
		   st->system, lnm);
	    printf("\n");
	    exit(1);};};

    wrk   = "...";
    skip = "---";
    pass  = "ok";
    fail  = "fail";

/* GOTCHA: st->uplog */
    snprintf(dir, BFLRG, "%s/%s", st->logdir, st->stamp);
    chdir(dir);

    Log = open_file("w", "watch");

    note(Log, TRUE, "Starting do-net watch using %s", file);
    note(Log, TRUE, "Timer is %s", st->aux[2].proper);

    if (st->send == TRUE)
       ip = PH_SETUP;
    else if (st->build == TRUE)
       ip = PH_BUILD;
    else if (st->install == TRUE)
       ip = PH_HOSTINSTALL;
    else
       ip = PH_CLEAN;
    phase = st->phases[ip].name;
    tr    = st->phases[ip].tlimit;

    snprintf(upf, BFLRG, "update");
    while (IS_NULL(phase) == FALSE)

/* wait for the update log to appear */
       {for (ok = FALSE; ok == FALSE; )
	    {if (file_exists(upf) == TRUE)
	        {files = ls("", "*");
		 if (files != NULL)
		    {for (i = 0; files[i] != NULL; i++)
		         {if (strstr(files[i], phase) != NULL)
			     {ok = TRUE;
			      break;};};
		     free_strings(files);};};
	     sleep(2);};
    
	snprintf(repfn, BFLRG, "%s.%s", cgetenv(FALSE, "USER"), phase);
	repf = open_file("w", repfn);

	watch_header(st, ip, repf, file, tr, shm, ehm, clr);

	nr = 0;

/* get the list of log files for this phase */
	plog = ls("", "%s.*", phase);

/* process the log files */
	if (plog != NULL)
	   {for (i = 0; plog[i] != NULL; i++)
	        nr = watch_phase(st, repf, plog[i], nr, phase,
				 pass, fail, skip, wrk);

	    free_strings(plog);};

	fclose(repf);

	watch_emit(st, repfn, file, shm, ehm, clr);

/* if none are left running advance to the next section */
	if (nr == 0)
	   {if (strcmp(phase, "setup") == 0)
	       {if (st->build == TRUE)
		   ip = PH_BUILD;
	        else if (st->install == TRUE)
		   ip = PH_HOSTINSTALL;
		else
		   ip = PH_CLEAN;}

	    else if (strcmp(phase, "build") == 0)
	       {if (st->install == TRUE)
		   ip = PH_HOSTINSTALL;
	        else
		   ip = PH_CLEAN;}

	    else if (strcmp(phase, "hostinstall") == 0)
	       ip = PH_NETINSTALL;

	    else
	       ip = PH_CLEAN;
	    
	    phase = st->phases[ip].name;
	    tr    = st->phases[ip].tlimit;}

	else if (tr > 600)
	   {sleep(10);
	    tr -= 10;}

	else if (tr > 60)
	   {sleep(5);
	    tr -= 5;}

	else
	   {sleep(1);
	    tr -= 1;};};

    if (st->watchout == TEXT)
       {note(Log, TRUE, "Continuing do-net watch until killed");
	while (1);};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* START_WATCH - launch a watch window process */

static void start_watch(donetdes *st, char *exe)
   {char fn[BFLRG], wargs[BFLRG];
    char *title, *geometry;
    FILE *fp;

    separatorv(Log);
    noten(Log, TRUE, "Start_watch");
    noten(Log, TRUE, "Launching watch process");
    noten(Log, TRUE, "Using xterm %s", cwhich("xterm"));

    title    = "DO-NET-WATCH";
    geometry = "90x15-10+10";

    wargs[0] = '\0';

    if (st->send == FALSE)
       push_tok(wargs, BFLRG, ' ', "-s");

    if (st->build == FALSE)
       push_tok(wargs, BFLRG, ' ', "-b");

    if (st->install == FALSE)
       push_tok(wargs, BFLRG, ' ', "-i");

    if (st->watchout == TEXT)
       push_tok(wargs, BFLRG, ' ', "+watch");
    else if (st->watchout == HTML)
       push_tok(wargs, BFLRG, ' ', "+hwatch");

    snprintf(fn, BFLRG, "%s/.do-watch-%s", st->shared, st->stamp);
    fp = open_file("w", fn);

    note(fp, TRUE, "#!/bin/csh -f");
    note(fp, TRUE, "%s %s %s %s", exe, wargs, st->hostfile, st->stamp);
    note(fp, TRUE, "rm -f %s", fn);
    note(fp, TRUE, "exit($status)");

    fclose(fp);

    chmod(fn, 0700);

    if (st->watchout == TEXT)
       brun(Log, "( xterm -geometry %s -T %s +sb -e %s & )",
	    geometry, title, fn);

    else if (st->watchout == HTML)
       brun(Log, "( %s & )", fn);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROGRESS - prepare a progress report for a specified do-net session
 *          - usage: do-net <hostfile> -p <stamp>
 *          - example: do-net foo:hosts/bar -p 07_26_18
 *          - GOTCHA: untested after conversion
 */

static int progress(donetdes *st)
   {int i, ip, np;
    char host[BFLRG], file[BFLRG], dir[BFLRG];
    char base[BFLRG], update[BFLRG];
    char *f, *s, **files;
    static char *phase[] = {"setup", "build", "netinstall",
			    "hostinstall", "interrupt"};

    get_host_file(host, file, st->hostfile);

    if (IS_NULL(host) == FALSE)
       run(FALSE, "%s %s do-net -p %s %s", st->ssh, host, st->stamp, file);

    else
/* GOTCHA: st->uplog */
       {snprintf(dir, BFLRG, "%s/%s", st->logdir, st->stamp);
	chdir(dir);

	printf("\n");
	cat(stdout, 0, -1, "update");
	printf("\n");

	np = sizeof(phase)/sizeof(char *);
	for (ip = 0; ip < np; ip++)
	    {snprintf(base, BFLRG, "%s", phase[ip]);
	     snprintf(update, BFLRG, "update");
	     touch("%s.t", base);

	     files = ls("", "%s.*", base);
	     if (files != NULL)
	        {for (i = 0; files[i] != NULL; i++)
		     {f = files[i];
		      if (strcmp(f+strlen(f)-2, ".t") == 0)
			 run(FALSE, "rm -f %s", f);

		      else if (strcmp(f, update) != 0)
			 {printf("%s\n", st->separator);
			  printf("%s\n", f);
			  printf("\n");
			  s = run(FALSE, "cat %s | awk '($1 != \"DO-NET:\") { print }'",
				  f);
			  printf("%s\n", s);};};

		 free_strings(files);};};};

   return(0);}

/*--------------------------------------------------------------------------*/

/*                          HOST CHECKING ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* VERIFYHOSTS - verify that each host in SP is available
 *             - if not NULL out the host field
 */

static int verifyhosts(donetdes *st, hfspec *sp, int nsp)
   {int i, j, tc;
    double tdi;
    char ahst[BFLRG], hserve[BFLRG], dt[16];
    char *hst;
    hfspec *lsp;

    nstrncpy(hserve, BFLRG, cwhich("hserve"), -1);

    tdi = start_time();

/* setup to run NSP jobs asynchronously */
    asetup(nsp, 3);

/* launch the jobs */
    for (i = 0, lsp = sp; i < nsp; i++, lsp++)
        {hst = lsp->rawh;
	 separatorv(Log);
	 noten(Log, st->verbose, "Verify %s", hst);
	 arun(lsp, NULL, lsp, "%s -m %s", hserve, hst);};

    separatorv(Log);

/* wait for the work to complete */
    tc = await(st->vfyt, 1000, "verify", check_tty, NULL, NULL);
    ASSERT(tc == TRUE);

/* close out the jobs */
    afin(finup);

    stop_time(dt, 16, tdi);

/* now check the results */
    for (i = 0; i < nsp; i++)
	   
/* convert host or system type to host */
        {FREE(sp[i].host);
	 nstrncpy(ahst, BFLRG, sp[i].logn, -1);

/* check for accessibility of real host */
	 if (strcmp(ahst, "-none-") != 0)
	    sp[i].host = STRSAVE(ahst);};

/* now squeeze out missing hosts */
    for (i = 0; i < nsp; i++)
	{if (IS_NULL(sp[i].host) == TRUE)
	    {for (j = i+1; j < nsp; j++)
	         sp[j-1] = sp[j];
             nsp--;
	     i--;};};

    return(nsp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECKLIST - check a list of specifications for hosts that are down */

static hfspec *checklist(donetdes *st, char *delim, hfspec *sp, int nsp)
   {int i, l;
    hfspec *hs;

    i  = 0;
    hs = MAKE_N(hfspec, nsp+1);
    if (hs != NULL)
       {nsp = verifyhosts(st, sp, nsp);

	for (l = 0; l < nsp; l++)
	    {if (sp[l].host == NULL)
	        st->inacc = append_tok(st->inacc, ' ', sp[l].rawh);

	     else
	        hs[i++] = sp[l];};

	for (; i <= nsp; i++)
	    {hs[i].delim   = NULL;
	     hs[i].rawh    = NULL;
	     hs[i].host    = NULL;
	     hs[i].running = -1;
	     hs[i].exit    = WAITING;};};

    return(hs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECKHOSTS - check to find out which hosts are down */

static void checkhosts(donetdes *st)
   {double ti;
    char tm[BFLRG];

    ti = start_time();

/* see which hosts are down */
    if (st->silent == FALSE)
       printf("   checking hosts ... ");

    st->inacc = NULL;

/* remove dead hosts from the host specs */
    st->hosts   = checklist(st, "host", st->hosts, st->n_hosts);
    st->n_hosts = count_specs(st->hosts, st->n_hosts);

/* remove dead hosts from the net specs */
    st->nets   = checklist(st, "net", st->nets, st->n_nets);
    st->n_nets = count_specs(st->nets, st->n_nets);

    set_phases(st);

    if (st->inacc != NULL)
       st->install = st->installwdh;

    if (st->silent == FALSE)
       printf("done (%s)\n", stop_time(tm, BFLRG, ti));

    if (st->inacc != NULL)
       {if (st->silent == FALSE)
	   printf("      %s (inaccessible)\n", st->inacc);
	note(Log, TRUE, "");
	note(Log, TRUE, "Inaccessible systems: %s", st->inacc);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNIQUEHOST - add specs from SSP to DSP if the host is unique to DSP */

static int uniquehost(hfspec *dsp, int nds, hfspec *ssp, int nss)
   {int i, j, add;
    char *host;

    for (i = 0; i < nss; i++)
        {host = ssp[i].rawh;
	 for (add = TRUE, j = 0; j < nds; j++)
	     {if (strcmp(host, dsp[j].rawh) == 0)
		 {add = FALSE;
		  break;};};

	 if (add == TRUE)
	    dsp[nds++] = ssp[i];};

    return(nds);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TESTHOSTS - check to see which hosts are down
 *           - usage: do-net -g <hostfile>
 *           - example: do-net -g foo:hosts/bar
 */

static int testhosts(donetdes *st, int c, char **v)
   {int i, nh, ns, downonly;
    char args[BFLRG], host[BFLRG], file[BFLRG];
    char *hfile, *uhst, *rhst, *fill;
    hfspec *hs;

    downonly = FALSE;
    args[0]  = '\0';

    hfile = "";
    for (i = 0; i < c; i++)
        {nstrcat(args, BFLRG, v[i]);
	 nstrcat(args, BFLRG, " ");
	 if (strcmp(v[i], "-d") == 0)
	    downonly = TRUE;
         else
	    hfile = v[i];};

    get_host_file(host, file, hfile);

    if (IS_NULL(host) == FALSE)
       run(FALSE, "%s %s do-net -g %s %s", st->ssh, host, file, args);

    else
       {nh = st->n_hosts + st->n_nets + 1;
	hs = MAKE_N(hfspec, nh);
	if (hs != NULL)

/* make a list of unique hosts */
	   {ns = uniquehost(hs, 0,  st->hosts, st->n_hosts);
	    ns = uniquehost(hs, ns, st->nets, st->n_nets);
	   
	    for (i = ns; i < nh; i++)
	        {hs[i].delim   = NULL;
		 hs[i].rawh    = NULL;
		 hs[i].host    = NULL;
		 hs[i].running = -1;
		 hs[i].exit    = WAITING;};

	    if (downonly == FALSE)
	       printf("Host status (%s):\n",
		      cgetenv(FALSE, "HOST_SERVER_DB"));

	    verifyhosts(st, hs, ns);

	    for (i = 0; i < ns; i++)
	        {uhst = hs[i].rawh;
		 rhst = hs[i].host;

		 fill = fill_string(uhst, 20);

		 if (downonly == TRUE)
		    {if (rhst == NULL)
		        printf("%s\n", uhst);}
		 else
		    {if (rhst != NULL)
		        printf("   %s up\n", fill);
		     else
		        printf("   %s down\n", fill);};};

	    FREE(hs);};};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPECLIST - check a list of specifications for hosts that are down */

static hfspec *speclist(char *delim, char *specs)
   {int i, j, nx;
    char t[BFLRG];
    char *host;
    hfspec *hs;

    i  = 0;
    nx = 10;
    hs = MAKE_N(hfspec, nx);

    if (hs != NULL)
       {FOREACH(s, specs, " \n")
	   if (strcmp(s, delim) == 0)
	      {NEXT(host, " \n");}
	   else
	      host = s;

	   if (host == NULL)
	      break;

	   hs[i].delim   = STRSAVE(delim);
	   hs[i].rawh    = STRSAVE(host);
	   hs[i].host    = NULL;
	   hs[i].running = IDLE;
	   hs[i].exit    = WAITING;

	   NEXT(s, " \n");
	   if (s != NULL)
	      {nstrncpy(hs[i].rawwork, BFLRG, s, -1);
	       full_path(t, BFLRG, cgetenv(FALSE, "HOME"), s);
	       nstrncpy(hs[i].workdir, BFLRG, t, -1);};

	   for (j = 0; TRUE; j++)
	       {NEXT(s, " \n");
		if ((s == NULL) || (strcmp(s, delim) == 0))
		   break;
		hs[i].args[j] = STRSAVE(s);};
  	   hs[i].args[j] = NULL;

	   i++;
	   if (i >= nx)
	      {nx += 10;
	       REMAKE(hs, hfspec, nx);
	       if (hs == NULL)
	          return(NULL);};

	ENDFOR;

	for (; i < nx; i++)
	    {hs[i].delim   = NULL;
	     hs[i].host    = NULL;
	     hs[i].rawh    = NULL;
	     hs[i].running = -1;
	     hs[i].exit    = WAITING;};};

    return(hs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_VAR - process a 'var' specification */

static void process_var(donetdes *st, char *s)
   {int iv, nv;
    char val[BFLRG];
    char *var, *pv;
    static char *varl[] = { "HOST_SERVER_DB", "SC_EXEC_TIME_VERIFY" };

    if (LAST_CHAR(s) == '\n')
       LAST_CHAR(s) = '\0';

    st->varspecs = append_tok(st->varspecs, ' ', s);

    nv = sizeof(varl)/sizeof(char *);
    for (iv = 0; iv < nv; iv++)
        {var = varl[iv];
	 if (strstr(s, var) != NULL)
	    {nstrncpy(val, BFLRG, s, -1);
	     pv = strtok(val, " \t");
	     pv = strtok(NULL, "\n");
	     if (IS_NULL(pv) == FALSE)
	        {csetenv(var, trim(pv, BOTH, " \t\n"));
		 break;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READHOST - read the host file */

static void readhost(donetdes *st, int log)
   {int tl;
    char linst[BFLRG], lssh[BFLRG], t[BFLRG], code[BFLRG];
    char hlst[BFMG], nlst[BFLRG];
    char *hf, *p, *pt;
    FILE *fp;

    hf = st->hostfile;

    st->varspecs = NULL;

    linst[0] = '\0';
    lssh[0]  = '\0';
    hlst[0]  = '\0';
    nlst[0]  = '\0';

/* read the host file */
    fp = fopen(hf, "r");
    if (fp != NULL)
       {while (TRUE)
	   {p = fgets(t, BFLRG, fp);
	    if (p == NULL)
	       break;
	    else if (strncmp(p, "script", 6) == 0)
	       nstrncpy(code, BFLRG, trim(p+6, BOTH, " \t\n"), -1);
	    else if (strncmp(p, "localinstall", 12) == 0)
	       nstrncpy(linst, BFLRG, trim(p+12, BOTH, " \t\n"), -1);
	    else if (strncmp(p, "mailprogram", 11) == 0)
	       {pt = trim(p+11, BOTH, " \t\n");
		FREE(st->mailer);
		st->mailer = STRSAVE(pt);}
	    else if (strncmp(p, "installtime", 11) == 0)
	       {tl = atoi(trim(p+11, BOTH, " \t\n"));
		st->phases[PH_NETINSTALL].tlimit  = tl;
	        st->phases[PH_HOSTINSTALL].tlimit = tl;}
	    else if (strncmp(p, "repository", 10) == 0)
	       {pt = trim(p+10, BOTH, " \t\n");
		FREE(st->repo);
		st->repo = STRSAVE(pt);}
	    else if (strncmp(p, "timelimit", 9) == 0)
	       {tl = atoi(trim(p+9, BOTH, " \t\n"));
		st->phases[PH_BUILD].tlimit = tl;}
	    else if (strncmp(p, "setuptime", 9) == 0)
	       {tl = atoi(trim(p+9, BOTH, " \t\n"));
		st->phases[PH_SETUP].tlimit = tl;}
	    else if (strncmp(p, "cleantime", 9) == 0)
	       {tl = atoi(trim(p+9, BOTH, " \t\n"));
		st->phases[PH_CLEAN].tlimit = tl;}
	    else if (strncmp(p, "timeout", 7) == 0)
	       nstrncpy(st->handtout, BFLRG, trim(p+7, BOTH, " \t\n"), -1);
	    else if (strncmp(p, "system", 6) == 0)
	       {pt = trim(p+6, BOTH, " \t\n");
		FREE(st->system);
		st->system = STRSAVE(pt);}
	    else if (strncmp(p, "logdir", 6) == 0)
	       nstrncpy(st->logdir, BFLRG, trim(p+6, BOTH, " \t\n"), -1);
	    else if (strncmp(p, "mail", 4) == 0)
	       {pt = trim(p+4, BOTH, " \t\n");
		FREE(st->maillist);
		st->maillist = STRSAVE(pt);}
	    else if (strncmp(p, "host", 4) == 0)
	       push_tok(hlst, BFMG, ' ', p);
	    else if (strncmp(p, "var", 3) == 0)
	       process_var(st, p+3);
	    else if (strncmp(p, "ssh", 3) == 0)
	       push_tok(lssh, BFLRG, ' ', p+3);
	    else if (strncmp(p, "net", 3) == 0)
	       push_tok(nlst, BFLRG, ' ', p);};

	fclose(fp);};

    if (cdefenv("SHARED") == TRUE)
       st->shared = STRSAVE(cgetenv(FALSE, "SHARED"));
    else
       st->shared = STRSAVE(cgetenv(FALSE, "HOME"));

/* NOTE: if set in host file the user is responsible for having
 * it on the path or giving a full path here
 */
    if (IS_NULL(lssh) == FALSE)
       snprintf(st->ssh, BFLRG,
		"%s -q %s -o BatchMode=yes -o StrictHostKeyChecking=no",
		lssh, st->usex);

/* the Stat variables are templates for use in comparing with the done list
 * the Specs variables contain the specifications for actual work
 */
    st->nets   = speclist("net", nlst);
    st->n_nets = count_specs(st->nets, -1);

/* GOTCHA: cleanup this clause */
   if (st->hostonly != NULL)
      {hlst[0] = '\0';
       FOREACH(s, st->hostonly, " ")
	  nstrncpy(t, BFLRG,
		   run(FALSE, "cat %s | awk '$1 == \"host\" && $0 ~ /%s / { print $0 }'", hf, s),
		   -1);
          if ((IS_NULL(t) == FALSE) && (strstr(t, s) != NULL))
	     {nstrcat(hlst, BFMG,
		      run(FALSE, "echo %s | awk '$1 == \"host\" { for (i = 1; i <= NF; i++) print $i }'", t));
	      nstrcat(hlst, BFMG, " ");};
       ENDFOR;};

    st->hosts   = speclist("host", hlst);
    st->n_hosts = count_specs(st->hosts, -1);

    set_phases(st);

/* get the various phase time limits */
    if (st->phases[PH_SETUP].tlimit == 0)
       st->phases[PH_SETUP].tlimit = 900;

    if (st->phases[PH_BUILD].tlimit == 0)
       st->phases[PH_BUILD].tlimit = 3600;

    if (st->phases[PH_HOSTINSTALL].tlimit == 0)
       st->phases[PH_HOSTINSTALL].tlimit = 120;

    if (st->phases[PH_NETINSTALL].tlimit == 0)
       st->phases[PH_NETINSTALL].tlimit = 120;

    if (st->phases[PH_CLEAN].tlimit == 0)
       st->phases[PH_CLEAN].tlimit = 60;

    if (IS_NULL(st->do_code) == TRUE)
       nstrncpy(st->do_code, BFLRG, code, -1);

/* check the script name and the version to be transmitted */
    if (IS_NULL(st->do_code) == TRUE)
       {printf("\n");
        printf("You must have a script specification in host file %s\n", hf);
        printf("\n");
        exit(4);};

    if (st->system == NULL)
       st->system = STRSAVE("code");

    if (IS_NULL(st->handtout) == TRUE)
       strcpy(st->handtout, "pass");

    if (strcmp(linst, "yes") == 0)
       st->localinstall = TRUE;

    if (IS_NULL(st->logdir) == TRUE)
       nstrncpy(st->logdir, BFLRG, cgetenv(TRUE, "HOME"), -1);
    full_path(st->logdir, BFLRG, cgetenv(FALSE, "HOME"), st->logdir);

    if (dir_exists(st->logdir) == FALSE)
       {printf("\n");
	printf("No log directory %s - exiting\n", st->logdir);
	printf("\n");
	exit(6);};

   if (IS_NULL(st->logdir) == FALSE)
      {st->cargs = append_tok(st->cargs, ' ', "-log");
       st->cargs = append_tok(st->cargs, ' ', st->logdir);};

   if (st->mailer == NULL)
      {char ml[BFLRG];

       nstrncpy(ml, BFLRG, cwhich("mailx"), -1);
       if (strcmp(ml, "none") == 0)
          {nstrncpy(ml, BFLRG, cwhich("mail"), -1);
	   if (strcmp(ml, "none") == 0)
	      {printf("\n");
	       printf("No mail program found - no mail sent\n");
	       printf("\n");
	       ml[0] = '\0';};};

       if (ml[0] != '\0')
	  st->mailer = STRSAVE(ml);};

/* setup the log files names */
    snprintf(st->uplog, BFLRG, "%s/%s", st->logdir, st->stamp);
    full_path(st->uplog, BFLRG, NULL, st->uplog);

    if (log == TRUE)
       {mkdir(st->uplog, 0770);
	snprintf(st->lnetfn, BFLRG, "%s/update", st->uplog);}
    else
       {getcwd(t, BFLRG);
	snprintf(st->lnetfn, BFLRG, "%s/.log.do-net", t);};

    Log = open_file("w+", st->lnetfn);
    if (Log == NULL)
       {printf("\n");
	printf("Cannot open log file %s - exiting\n", st->lnetfn);
	printf("\n");
	exit(7);};
       
    setbuf(Log, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT - initialize the main state of a do-net session */

static void init(donetdes *st, int *purepo, double *gti, char *uhost)
   {int smin, bmin, imin;
    char *rv;

    separatorv(Log);
    noten(Log, TRUE, "Init");

    *gti = start_time();

    if (st->silent == FALSE)
       printf("Beginning update of %s (%s)\n", st->system, st->stamp);

/* determine whether we are working from a distribution or
 * a source repository
 */
    if (IS_NULL(st->dist) == FALSE)
       {*purepo = FALSE;
	st->cargs = append_tok(st->cargs, ' ', "-dist");
	st->cargs = append_tok(st->cargs, ' ', path_tail(st->dist));}
    else if (st->repo != NULL)
       {*purepo = TRUE;
	st->cargs = append_tok(st->cargs, ' ', "-repo");
	st->cargs = append_tok(st->cargs, ' ', st->repo);
	st->cargs = append_tok(st->cargs, ' ', st->tag);}
    else if (st->send == TRUE)
       {note(Log, TRUE, "No means to get sources defined");
	exit(2);};

    note(Log, TRUE, "");
    note(Log, TRUE, "Updating %s from %s according to %s",
	 st->system, uhost, st->hostfile);
    if (st->clargs != NULL)
       note(Log, TRUE, "Command line args: %s", st->clargs);

    if (st->localinstall == TRUE)
       note(Log, TRUE, "Installs will be done on hosts which pass tests");

    note(Log, TRUE, "User: %s   Date: %s   Tag: %s",
	 cgetenv(TRUE, "USER"), run(FALSE, "date"), st->stamp);

    note(Log, FALSE, "Time limits in minutes are: ");
    smin = st->phases[PH_SETUP].tlimit / 60;
    note(Log, FALSE, "%d setup; ", smin);

    bmin = st->phases[PH_BUILD].tlimit / 60;
    note(Log, FALSE, "%d build; ", bmin);

    imin = st->phases[PH_HOSTINSTALL].tlimit / 60;
    note(Log, TRUE, "%d install", imin);

    noten(Log, TRUE, "state.dist           = |%s|", st->dist);
    noten(Log, TRUE, "state.check          = |%d|", st->check);
    noten(Log, TRUE, "state.send           = |%d|", st->send);
    noten(Log, TRUE, "state.build          = |%d|", st->build);
    noten(Log, TRUE, "state.install        = |%d|", st->install);
    noten(Log, TRUE, "state.installwdh     = |%d|", st->installwdh);
    noten(Log, TRUE, "state.verbose        = |%d|", st->verbose);
    noten(Log, TRUE, "state.silent         = |%d|", st->silent);
    noten(Log, TRUE, "state.clearout       = |%d|", st->clearout);
    noten(Log, TRUE, "state.reportprogress = |%d|", st->reportprogress);
    noten(Log, TRUE, "state.watchprogress  = |%d|", st->watchprogress);
    noten(Log, TRUE, "state.localinstall   = |%d|", st->localinstall);
    noten(Log, TRUE, "state.testhost       = |%d|", st->testhost);
    noten(Log, TRUE, "state.watch          = |%d|", st->watch);
    noten(Log, TRUE, "state.hostfile       = |%s|", st->hostfile);
    noten(Log, TRUE, "state.do_code        = |%s|", st->do_code);
    noten(Log, TRUE, "state.tag            = |%s|", st->tag);

    noten(Log, TRUE, "PATH = %s", cgetenv(FALSE, "PATH"));

/* dump the environment */
    rv = lrun(Log, QLOG, "/usr/bin/env");
    if (rv != NULL)
       {noten(Log, TRUE, "Environment:");
	FOREACH(s, rv, "\n")
	   noten(Log, TRUE, "   %s", s);
	ENDFOR;};   

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TRANSMIT_SCRIPT - send the scripts to HOST */

static int transmit_script(donetdes *st, char *host, char *csm)
   {int ia, na, ns, ok;
    char *rv, *p;

    ok = TRUE;
    ns = 0;
    na = 3;

    if (IS_NULL(host) == TRUE)
       {printf("\n");
	printf("No destination host for scripts - exiting\n");
	printf("\n");
	exit(8);};

    for (ia = 0; (ia < na) && (ns != N_AUX); ia++)
        {lrun(Log, NLOG, "scp %s %s:", st->scripts, host);

/* list the remote files */
	 rv = lrun(Log, QLOG, "ssh %s ls -l .do-*.%s", host, st->stamp);
	 if (rv == NULL)
	    noten(Log, TRUE, "Cannot list remote files");
         else
	    {noten(Log, TRUE, "%s files:", host);
	     FOREACH(s, rv, "\n")
	        noten(Log, TRUE, "   %s", s);
	     ENDFOR;};   

/* verify that they arrived in working shape */
	 rv = lrun(Log, QLOG, "ssh %s md5sum .do-*.%s", host, st->stamp);
	 if (rv == NULL)
	    noten(Log, TRUE, "Cannot checksum remote files");
	 else
	    {noten(Log, TRUE, "Checksum %s files:",  host);
	     FOREACH(s, rv, "\n")
	        noten(Log, TRUE, "   %s", s);
	     ENDFOR;

/* NOTE: CYGWIN md5sum prepends a '*' to the file names */
	     p = subst(rv, "*", " ", -1);

	     if (strcmp(csm, p) == 0)
	        {ns = N_AUX;
		 break;};};};

/* if after NA attempts they are not there - it is an error */
    if (ns != N_AUX)
       {printf("\n");
	printf("Unable to send scripts to %s - exiting", host);
	printf("\n");
	noten(Log, TRUE, "Unable to send scripts to %s - exiting", host);
	ok = FALSE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SENDSCRIPT - send the local script */

static int sendscript(donetdes *st, char *host)
   {int i, is, ok;
    char bin[BFLRG], csm[BFLRG], t[BFLRG];
    char *dst, *rv;
    auxdes *pa;

    separatorv(Log);
    noten(Log, TRUE, "Sendscript");

    nstrncpy(bin, BFLRG, path_head(cwhich("do-net")), -1);

    ok = TRUE;
    pa = st->aux;

    st->scripts = NULL;

/* prepare the auxilliary scripts */
    for (is = 0; is < N_AUX; is++)
        {dst = pa[is].work;
	 st->scripts = append_tok(st->scripts, ' ', dst);
	 copy(dst, pa[is].proper);
	 chmod(dst, pa[is].permi);};

/* list the local files */
    rv = lrun(Log, QLOG, "ls -l .do-*.%s", st->stamp);
    if (rv == NULL)
       noten(Log, TRUE, "Cannot list remote files");
    else
       {noten(Log, TRUE, "Local files:");
	FOREACH(s, rv, "\n")
	   noten(Log, TRUE, "   %s", s);
	ENDFOR;};   

/* checksum the local files */
    rv = lrun(Log, QLOG, "md5sum .do-*.%s", st->stamp);
    if (rv == NULL)
       noten(Log, TRUE, "Cannot checksum local files");
    else
       {noten(Log, TRUE, "Checksum local files:");
	FOREACH(s, rv, "\n")
	   noten(Log, TRUE, "   %s", s);
	ENDFOR;};   
    nstrncpy(csm, BFLRG, rv, -1);

/* copy the prepared files/scripts to one specified host */
    if (host != NULL)
       ok = transmit_script(st, host, csm);

/* copy the prepared files/scripts to all net hosts */
    else
       {ok = TRUE;
	for (i = 0; (i < st->n_nets) && (ok == TRUE); i++)
	    {host = st->nets[i].host;
	     ok   = transmit_script(st, host, csm);};};

/* remove the local copies if we are not in the home directory */
    getcwd(t, BFLRG);
    if (strcmp(t, cgetenv(FALSE, "HOME")) != 0)
       {FOREACH(s, st->scripts, " ")
	   unlink_safe(s);
	ENDFOR;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT - send the cleaned up report */

static void report(donetdes *st)
   {char rpt[BFLRG], s[BFLRG], tlog[BFLRG];
    char *p;
    FILE *fin, *fout, *frpt;

    separatorv(Log);
    noten(Log, TRUE, "Report");

    snprintf(rpt,  BFLRG, "%s/report", st->uplog);
    snprintf(tlog, BFLRG, "%s.tmp",    st->lnetfn);

    run(FALSE, "mv -f %s %s", st->lnetfn, tlog);

/* close the log while we operate on its contents */
    fclose(Log);

/* filter junk out of net log and write report */
    fin  = open_file("r", tlog);
    fout = open_file("w", st->lnetfn);
    frpt = open_file("w", rpt);
    while (TRUE)
       {p = fgets(s, BFLRG, fin);
	if (p == NULL)
	   break;

/* remove messages from stty */
	if (strncmp(s, "stty:", 5) == 0)
	   continue;

/* get rid of any message with "warning:"
 * we don't print any such thing ourselves but
 * ssh prints such warnings for compatibility modes
 */
	else if ((strncmp(s, "warning:", 8) == 0) &&
		 (strstr(s, "authentication") != NULL))
	   continue;

	else
	   {fputs(s, fout);

/* messages without "DO-NET:" go into the report */
	    if (strncmp(s, "DO-NET:", 7) != 0)
	       {fputs(s, frpt);

/* check for failure on any hosts */
		if (strstr(s, "Failed on") != NULL)
		   st->err = 10;};};};

    fclose(fout);
    fclose(frpt);
    fclose(fin);
    unlink_safe(tlog);

/* reopen the log for remaining activities */
    Log = open_file("a", st->lnetfn);

    if ((st->mailer != NULL) && (st->maillist != NULL))
       lrun(Log, VLOG, "( %s -s \"%s auto build log (%s)\" %s < %s )",
	    st->mailer, st->system, path_tail(st->hostfile),
	    st->maillist, rpt);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOCKOUT - lock out a competitor
 *         - this is NOT a distributed lock and perhaps it should be
 */

static void lockout(donetdes *st, char *host, char *uhost)
   {int i;
    char tdir[BFLRG], lock[BFLRG];
    char *nhst, *wdir;
    FILE *lf;

    for (i = 0; i < st->n_nets; i++)
        {nhst = st->nets[i].host;
	 wdir = st->nets[i].workdir;

	 if ((strcmp(nhst, host) == 0) || (strcmp(nhst, uhost) == 0))
	    {nstrncpy(tdir, BFLRG, path_tail(wdir), -1);
	     snprintf(lock, BFLRG, "%s/%s.%s.lock",
		      path_head(wdir), st->system, tdir);

	     if (file_exists(lock) == TRUE)
	        {noted(Log, "");
		 noted(Log, "Another do-net is running in directory %s", wdir);
		 noted(Log, "");
		 run(BOTH, "cat %s", lock);
		 noted(Log, "");

		 report(st);

		 exit(1);};

	     lf = fopen(lock, "w");
	     if (lf != NULL)
	        {note(lf, TRUE, "Host file = %s", st->hostfile);
		 note(lf, TRUE, "Command line arguments = %s", st->clargs);
		 note(lf, TRUE, "Date = %s", run(FALSE, "date"));
		 note(lf, TRUE, "User = %s", cgetenv(TRUE, "USER"));
		 fclose(lf);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RECOMMEND - make recommendations possibly for a retry */

static void recommend(donetdes *st)
   {int nc;
    char rs[BFLRG], cmd[BFLRG];
    char **v;

    note(Log, TRUE, "");
    note(Log, TRUE, "Making recommendations for follow up action");
    note(Log, TRUE, "");

/* look for retry hints */
    nstrncpy(rs, BFLRG,
	     run(FALSE, "awk '$1 == \"DO-NET:\" && $2 == \"(RETRY)\" { print $3 }' %s | sort | uniq",
		 st->lnetfn), -1);
    nstrncpy(rs, BFLRG, subst(rs, " ", ",", -1), -1);
    nstrncpy(rs, BFLRG, subst(rs, "\n", ",", -1), -1);

    nstrncpy(cmd, BFLRG, "do-net", -1);
    for (v = st->argvcp + 1; *v != NULL; v++)
        {nc = strlen(cmd);
	 snprintf(cmd+nc, BFLRG-nc, " %s", *v);};

   if (IS_NULL(rs) == FALSE)
      {if (st->silent == FALSE)
	  {noted(Log, "");
	   noted(Log, "Some hosts failed to complete their work successfully.");
	   noted(Log, "You may want to retry those hosts as follows:");
	   noted(Log, "    %s -o %s", cmd, rs);
	   noted(Log, "");}
       else
          {note(Log, TRUE, "");
	   note(Log, TRUE, "Some hosts failed to complete their work successfully.");
	   note(Log, TRUE, "You may want to retry those hosts as follows:");
	   note(Log, TRUE, "    %s -o %s", cmd, rs);
	   note(Log, TRUE, "");};};

    return;}

/*--------------------------------------------------------------------------*/

/*                             PHASE SUBROUTINES                            */

/*--------------------------------------------------------------------------*/

/* WORK - do the work for the specified phase */

static int work(donetdes *st, hfspec *sp, int nsp, int ip, int rpt)
   {int i, ok, nsec;
    double ti;
    char fields[BFLRG], fnm[BFLRG];
    char *host, *phase;
    hfspec *lsp;
    phasedes *ph;

    ok = FALSE;
    if (sp != NULL)
       {ti    = start_time();
	ph    = st->phases + ip;
	phase = ph->name;
	nsec  = ph->tlimit;

	current_phase = ph;

/* setup to run NSP jobs asynchronously */
	asetup(nsp, 3);

/* launch the jobs */
	for (i = 0, lsp = sp; i < nsp; i++, lsp++)
	    {host = lsp->host;
	     if (IS_NULL(host) == TRUE)
	        {printf("\n");
		 printf("No host for %s (%d/%d) in phase %s - exiting\n",
			lsp->rawh, i, nsp, phase);
		 printf("\n");
		 exit(9);};

	     get_fields(fields, BFLRG, lsp);

	     snprintf(fnm, BFLRG, "%s/%s.%s.%d", st->uplog, phase, host, i+1);

	     separatorv(Log);

	     noten(Log, st->verbose, "Dispatching %s on %s", phase, host);
	     arun(lsp, fnm, lsp,
		  "%s %s %s -%s %s -time_limit %d -host_vars %s -host_fields %s",
		  st->ssh, host, st->run, phase, st->cargs, nsec,
		  st->varspecs, fields);};

	separatorv(Log);

/* wait for the work to complete */
	cwait(st, sp, nsp, phase, nsec);

	current_phase = NULL;

	ok = make_report(st, sp, nsp, rpt);

	stop_time(ph->time, 16, ti);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP - dispatch the setup of the sources on the nets specified */

static void setup(donetdes *st, int ip, int urepo)
   {int i, ok, nsp;
    char t[BFLRG];
    char *host;
    hfspec *sp, *lsp;

    ok = TRUE;

    sp  = st->nets;
    nsp = st->n_nets;

    if (sp != NULL)
       {note(Log, FALSE, "For source setup using");
	if (urepo == FALSE)
	   {note(Log, TRUE, " distribution:");
	    note(Log, TRUE, "    %s", st->dist);}
	else
           {note(Log, TRUE, " repository:");
	    note(Log, TRUE, "    %s (tag %s)", st->repo, st->tag);};

	note(Log, TRUE, " ");

/* if no repo we better have a distribution file which we need to
 * send to all the net hosts now
 */
	if (urepo == FALSE)
	   {if (file_exists(st->dist) == TRUE)
	       {chmod(st->dist, 0770);
		getcwd(t, BFLRG);
		noten(Log, st->verbose, "Current directory: %s", t);

		for (i = 0, lsp = sp; i < nsp; i++, lsp++)
		    {host = lsp->host;

/* update sources from a distribution file
 * NOTE: see earlier note about KSH and SCP
 */
		     if (st->usescp == TRUE)
		        lrun(Log, QLOG, "scp %s %s:%s",
			     st->dist, host, path_tail(st->dist));
		     else
		        lrun(Log, QLOG, "( (sleep 2 ; cat %s) | ssh %s 'cat >| '%s )",
			     st->dist, host, path_tail(st->dist));};}
	    else
	       {noten(Log, st->verbose, "Distribution file %s does not exist",
		      st->dist);
		ok = FALSE;};};};

    if (ok == TRUE)
       ok = work(st, sp, nsp, ip, FALSE);

    st->build   &= ok;
    st->install &= ok;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BUILD - dispatch the build and test phase */

static void build(donetdes *st, int ip)
   {int ok, nsp;
    hfspec *sp;

    sp  = st->hosts;
    nsp = st->n_hosts;

    ok = work(st, sp, nsp, ip, TRUE);

    st->install &= ok;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HOSTINSTALL - dispatch the host install */

static void hostinstall(donetdes *st, int ip)
   {int ok, nsp;
    hfspec *sp;

    sp  = st->hosts;
    nsp = st->n_hosts;

    ok = work(st, sp, nsp, ip, FALSE);
    ASSERT(ok == 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NETINSTALL - dispatch the net install phase */

static void netinstall(donetdes *st, int ip)
   {int ok, nsp;
    hfspec *sp;

    sp  = st->nets;
    nsp = st->n_nets;

    ok = work(st, sp, nsp, ip, FALSE);
    ASSERT(ok == 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEAN - dispatch the cleanup phase of a do-net
 *       - by killing all descendants on all hosts
 *       - and removing locks
 */

static void clean(donetdes *st, hfspec *sp, int nsp, int ip, double *gti)
   {int ok;
    double ti;

    if (gti == NULL)
       ti = start_time();
    else
       ti = *gti;

    if (sp == NULL)
       {sp  = st->hosts;
	nsp = st->n_hosts;};

    ok = work(st, sp, nsp, ip, TRUE);
    ASSERT(ok == TRUE);

    noten(Log, st->verbose, "   All jobs completed");

/* if we were not already finishing up do so now */
   if (st->finishing == FALSE)
      {noten(Log, st->verbose, "   st->finishing");
       finish(st, ti);};

    noten(Log, st->verbose, "Leaving clean");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEAROUT - clear out the debris from an old do-net session
 *          - usage: do-net <hostfile> -c <stamp>
 *          - example: do-net foo:hosts/bar -c 07_26_18
 */

static int clearout(donetdes *st)
   {char host[BFLRG], file[BFLRG];

    get_host_file(host, file, st->hostfile);

    if (IS_NULL(host) == FALSE)
       run(FALSE, "%s %s do-net -c %s %s", st->ssh, host, st->stamp, file);

    else
       {clean(st, st->hosts, st->n_hosts, PH_CLEAN, NULL);

#if 0
/* remove the log files */
	printf("   Removing the log files %s/%s/*\n",
	       st->logdir, st->stamp);
        chdir(st->logdir);
        run(FALSE, "rm -f %s/*", st->stamp);
#endif
	};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SESSION - do the central do-net work
 *         - usage: do-net <hostfile>
 *         - example: do-net hosts/bar
 */

static int session(donetdes *st, char *exe, char *host, char *uhost)
   {int urepo, rv;
    double gti;

/* initialize the session */
    init(st, &urepo, &gti, uhost);

/* start a do-watch if possible and requested */
    if (st->watch == TRUE)
       start_watch(st, exe);

/* check to see which hosts are down */
    if (st->check == TRUE)
       checkhosts(st);

    separatorv(Log);
    noten(Log, TRUE, "Session");

    note(Log, TRUE, "");

/* report the following when verbose */
    noten(Log, st->verbose, "");
    noten(Log, st->verbose, "state.mailer    = |%s|",
	  (st->mailer != NULL) ? st->mailer : "none");
    noten(Log, st->verbose, "state.maillist  = |%s|",
	  (st->maillist != NULL) ? st->maillist : "none");
    if (st->inacc != NULL)
       noten(Log, st->verbose, "state.inacc     = |%s|", st->inacc);

/* lock out any competitors */
    lockout(st, host, uhost);

/* send the scripts where they need to be */
    rv = sendscript(st, NULL);
    if (rv == TRUE)

/* setup the sources on the nets specified */
       {if (st->send == TRUE)
	   setup(st, PH_SETUP, urepo);

/* dispatch the builds */
	if (st->build == TRUE)
	   build(st, PH_BUILD);

/* host install if successful */
	if ((st->install == TRUE) || (st->localinstall == TRUE))
	   hostinstall(st, PH_HOSTINSTALL);

/* net install if successful */
	if ((st->install == TRUE) || (st->localinstall == TRUE))
	   netinstall(st, PH_NETINSTALL);

/* gather the results into the log file */
	finish(st, gti);};

/* invert the sense of RV to be an exit status */
    rv = !rv;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEBUG - aid effort to debug do-local and do-specific scripts
 *       - usage: do-net -dbg <phase>,<cfg> <hostfile>
 *       - example: do-net -dbg setup,foo hosts/bar
 */

static int debug(donetdes *st)
   {int ip, is, nsec, nsp, rv, ok;
    char fields[BFLRG], s[BFLRG], phase[BFLRG], cfg[BFLRG];
    char host[BFLRG];
    char *t, **al;
    hfspec *sp, *lsp;
    phasedes *ph;

    phase[0] = '\0';
    cfg[0]   = '\0';

    nstrncpy(s, BFLRG, st->debug, -1);
    t = strtok(s, ",");
    if (t != NULL)
       nstrncpy(phase, BFLRG, t, -1);

    t = strtok(NULL, ",");
    if (t != NULL)
       nstrncpy(cfg, BFLRG, t, -1);

    for (ip = 0; ip < PH_N_PHASES; ip++)
        {ph = st->phases + ip;
	 if (strcmp(phase, ph->name) == 0)
	    break;};

    nsec = ph->tlimit;
    nsp  = ph->nsp;
    sp   = ph->sp;

    ok = FALSE;
    for (lsp = sp, is = 0; is < nsp; lsp++, is++)
        {if (strcmp(cfg, lsp->rawh) == 0)
	    ok = TRUE;
	 else
	    {al = lsp->args;
	     for (al = lsp->args; *al != NULL; al++)
	         if (strcmp(cfg, *al) == 0)
		    {ok = TRUE;
		     break;};};

         if (ok == TRUE)
	    break;};

    if (is >= nsp)
       {printf("No specifications matching %s\n", st->debug);
	rv = FALSE;}

    else
       {nstrncpy(host, BFLRG,
		 run(FALSE, "%s -m %s", cwhich("hserve"), lsp->rawh), -1);

	if (IS_NULL(host) == TRUE)
	   {printf("\n");
	    printf("No host for phase %s - exiting\n", phase);
	    printf("\n");
	    exit(10);};

/* send the scripts where they need to be */
	rv = sendscript(st, host);
	if (rv == TRUE)
	   {get_fields(fields, BFLRG, lsp);

	    noted(Log, "Dispatching %s on %s", phase, host);

	    t = run(TRUE,
		    "%s %s csh -vx %s -%s %s -time_limit %d -host_vars %s -host_fields %s",
		    st->ssh, host, st->run, phase, st->cargs, nsec,
		    st->varspecs, fields);

	    noted(Log, "");
	    noted(Log, "%s", t);
	    noted(Log, "");

	    run(TRUE, "%s %s rm -f %s", st->ssh, host, st->scripts);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_SECT - handle st->finishing logging chores for phases */

static void fin_sect(donetdes *st, hfspec *sp, int nsp,
		     FILE *slog, char *sect, char *etm)
   {int i, l, bld, nl;
    char file[BFLRG], s[BFLRG];
    char *host, *p;
    FILE *elog, *fin;

    bld = (strcmp(sect, "build") == 0);

    nl = 0;
    for (i = 0; i < nsp; i++)
        {host = sp[i].host;

	 snprintf(file, BFLRG, "%s/%s.%s.%d", st->uplog, sect, host, i+1);
	 fin = open_file("r", file);
	 if (fin != NULL)
	    {elog = open_file("a", "%s/log.%s", st->logdir, host);

	     for (l = 0; TRUE; l++)
	         {p = fgets(s, BFLRG, fin);
		  if (p == NULL)
		     break;
		  else if (strncmp(s, "DO-NET: (LOG) ", 14) == 0)
		     fputs(s+14, elog);
		  else if (strncmp(s, "DO-NET: (STAT) ", 15) == 0)
		     fputs(s+15, slog);
		  else if ((bld == TRUE) ||
			   ((strncmp(s, "Succeeded ", 10) != 0) &&
			    (strncmp(s, "Failed ", 7) != 0)))
		     {if (nl == 0)
		         {note(Log, TRUE, "%s\n", st->separator);
			  if (bld == FALSE)
			     note(Log, TRUE, "Starting %s", sect);}
		      else if ((l == 0) && (bld == TRUE))
		         {note(Log, TRUE, "");
			  note(Log, TRUE, "%s\n", st->separator);};
		      fputs(s, Log);
		      nl++;};};

	     fclose(elog);
	     fclose(fin);
	     unlink_safe(file);};};

    if ((nl > 0) && (bld == FALSE))
       {note(Log, TRUE, "Done with %s (%s)", sect, etm);
	if ((strcmp(sect, "setup") == 0) ||
	    (strcmp(sect, "hostinstall") == 0) ||
	    (strcmp(sect, "netinstall") == 0))
	   {notet(Log, "STAT", "%s (%s)", sect, etm);
	    note(Log, TRUE, " ");}
	else
	   run(FALSE, "rm -f %s/%s.*", st->uplog, sect);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FINISH - write up the report and call it a day */

static void finish(donetdes *st, double gti)
   {int i, ip, np, nsp;
    char etm[BFLRG], lock[BFLRG], ptime[BFLRG];
    char *host, *wdir, *sect, *time;
    hfspec *sp;
    FILE *slog;

    nstrncpy(ptime, BFLRG, cwhich("ptime"), -1);
    st->finishing = TRUE;

/* cleanup if there were timeouts */
    if (st->havetimeouts == TRUE)
       {note(Log, TRUE, "Some host timed out - cleaning up");

	st->havetimeouts = FALSE;

	clean(st, st->hosts, st->n_hosts, PH_CLEAN, &gti);};

/* we are really done so finish up */
    if (st->silent == FALSE)
       printf("   reporting ...");

/* report the elapsed time
 * technically this doesn't include the report and cleanup time
 * which is hopefully very small
 */
    stop_time(etm, BFLRG, gti);

    note(Log, TRUE, "Total time to completion %s", etm);

/* report do-stat version, domain (seconds since 1/1/2000), and total time */
    notet(Log, "STAT", "Version 5");
    notet(Log, "STAT", "Domain %s", run(FALSE, "%s -p 0", ptime));
    notet(Log, "STAT", "total (%s)", etm);

    note(Log, TRUE, "");

    lrun(Log, NLOG, "( rm -f %s/log.* >& /dev/null )", st->logdir);

    slog = open_file("a", "%s/stat", st->uplog);

    np = PH_N_PHASES;
    for (ip = 0; ip < np; ip++)
        {sect = st->phases[ip].name;
	 sp   = st->phases[ip].sp;
	 nsp  = st->phases[ip].nsp;
	 time = st->phases[ip].time;

	 fin_sect(st, sp, nsp, slog, sect, time);};

    fclose(slog);

/* remove empty log.<host> files */
    run(FALSE, "find . -size 0c -name \"log.*\" -exec rm {} \\;");

/* remove empty <stamp>.stat files */
    run(FALSE, "find . -size 0c -name \"*.stat\" -exec rm {} \\;");

    note(Log, TRUE, "%s\n", st->separator);

    if (st->silent == FALSE)
       printf(" done\n");

/* unlock the working directory */
    for (i = 0; i < st->n_nets; i++)
        {host = st->nets[i].host;
	 wdir = st->nets[i].workdir;

         snprintf(lock, BFLRG, "%s/%s.%s.lock",
		  path_head(wdir), st->system, path_tail(wdir));

	 unlink_safe(lock);};

/* notify the people on the mailing list */
    report(st);

/* make recommendations for proceeding */
    recommend(st);

/* delete the local scripts */
    for (i = 0; i < st->n_nets; i++)
        {host = st->nets[i].host;
	 if (IS_NULL(host) == TRUE)
	    {printf("\n");
	     printf("No host for finish - exiting\n");
	     printf("\n");
	     exit(11);};

	 lrun(Log, QLOG, "%s %s rm -f %s", st->ssh, host, st->scripts);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTERRUPT - cleanup properly after an interrupt */

static void interrupt(int sig)
   {int nsp;
    char *sect;
    hfspec *sp;
    void (*ohnd)(int sig);

   if (state.inintr == FALSE)
      {state.inintr = TRUE;

       ohnd = signal(SIGINT, interrupt);
       ASSERT(ohnd == 0);

/* the watch option process does not have Log defined and
 * does not need to cleanup
 */
       if ((Log != NULL) && (current_phase != NULL))
	  {sect = current_phase->name;
	   sp   = current_phase->sp;
	   nsp  = current_phase->nsp;
	   noted(Log, "\n>>> interrupted in %s", sect);
	   clean(&state, sp, nsp, PH_CLEAN, NULL);};};

    exit(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_AUX - initialize the auxilliary script info */

static void init_aux(donetdes *st)
   {char bin[BFLRG];
    auxdes *pa;

    nstrncpy(bin, BFLRG, path_head(cwhich("do-net")), -1);

/* make sure do_code is henceforth a full, absolute path */
    full_path(st->do_code, BFLRG, st->shared, st->do_code);

    pa = st->aux;

/* prepare the do-local script */
    snprintf(pa[0].proper, BFLRG, "%s/do-local", bin);
    snprintf(pa[0].work,   BFLRG, ".do-local.%s", st->stamp);
    nstrncpy(pa[0].perms, 10, "-rwx------", -1);
    pa[0].permi = 0700;

/* prepare the do-<code> file */
    nstrncpy(pa[1].proper, BFLRG, st->do_code, -1);
    snprintf(pa[1].work, BFLRG, ".%s.%s", path_tail(st->do_code), st->stamp);
    nstrncpy(pa[1].perms, 10, "-rw-------", -1);
    pa[1].permi = 0600;

/* prepare the timer script */
    nstrncpy(pa[2].proper, BFLRG, cwhich("timer"), -1);
    snprintf(pa[2].work, BFLRG, ".do-timer.%s", st->stamp);
    nstrncpy(pa[2].perms, 10, "-rwx------", -1);
    pa[2].permi = 0700;

/* prepare the nfsmon script */
    nstrncpy(pa[3].proper, BFLRG, cwhich("nfsmon"), -1);
    snprintf(pa[3].work, BFLRG, ".do-nfsmon.%s", st->stamp);
    nstrncpy(pa[3].perms, 10, "-rwx------", -1);
    pa[3].permi = 0700;

/* prepare the iopr script */
    nstrncpy(pa[4].proper, BFLRG, cwhich("iopr"), -1);
    snprintf(pa[4].work, BFLRG, ".do-iopr.%s", st->stamp);
    nstrncpy(pa[4].perms, 10, "-rwx------", -1);
    pa[4].permi = 0700;

/* prepare the csh-subr script */
    snprintf(pa[5].proper, BFLRG, "%s/csh-subroutines", st->etcdir);
    snprintf(pa[5].work, BFLRG, ".do-csh-subr.%s", st->stamp);
    nstrncpy(pa[5].perms, 10, "-rw-------", -1);
    pa[5].permi = 0600;

/* setup the do_code and run state variables */
    findrun(st);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - tell how to run do-net */

static void help(void)
   {

    printf("\n");
    printf("Usage: do-net [+/-b]\n");
    printf("              [-c <stamp>] [-ch]\n");
    printf("              [-d <file>] [-dt <sec>] [-e <script>] [-g]\n");
    printf("              [-h] [+/-i] [-int] [-I] [+/-m]\n");
    printf("              [-o <host>[,<host>]*] [-p <stamp>] [-q]\n");
    printf("              [+/-s] [-scp] [-t <tag>] [-v] [-watch] [+/-x]\n");
    printf("              <hostfile>\n");
    printf("              [-a ...]\n");
    printf("\n");
    printf("    a   - all following args are passed to the script\n");
    printf("    b   - do not build (-) or build only (+)\n");
    printf("    c   - clean up after aborted attempt\n");
    printf("    ch  - do not check whether hosts are up or down\n");
    printf("    d   - use distribution file\n");
    printf("    dt  - time in seconds to verify hosts (default 30)\n");
    printf("    e   - use the specified script\n");
    printf("    g   - test hosts for availability\n");
    printf("    h   - this help message\n");
    printf("    i   - do not install (-) or install only (+)\n");
    printf("    int - turn off interactive queries and commands\n");
    printf("    I   - installs may proceed even with a down host\n");
    printf("    l   - defer install decision to local hosts\n");
    printf("    m   - do setup in parallel (-) or serial (+) default is -m\n");
    printf("    o   - only do the comma delimited subset of hosts or configs\n");
    printf("          from the hostfile\n");
    printf("    p   - report progress\n");
    printf("    q   - don't print tty messages\n");
    printf("    s   - do not setup (-) or setup only (+)\n");
    printf("    scp - use SSH instead of SCP for file transport\n");
    printf("    t   - use version, <tag>, for repository update\n");
    printf("    v   - verbose mode\n");
    printf("    x   - do not forward X11 display (-) or ignore X (+)\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_ARGS - process the command line arguments */

static int process_args(donetdes *st, int c, char **v)
   {int i, l, ok;
    char *sh;

    if (c == 0)
       {help();
	return(1);};

    st->argvcp = v;

/* NOTE: you must have st->usescp FALSE when two bad things happen
 * First, the user's shell is KSH which is misfeatured wrt to login
 * environment - specifically no ~/.kshrc for users to overcome what
 * the sysadmins give the user (unhappy when you have bad sys admins)
 * Second, some admins cannot setup ssh worth shucks and consequently
 * the remote end of the SCP cannot find its side of ssh
 * the net effect is that you get a message like:
 * /bin/ksh: scp: no file or directory
 */
    sh = getenv("SHELL");
    if ((sh != NULL) && (strstr(sh, "ksh") != NULL))
       st->usescp = FALSE;

    ok = TRUE;
    l  = c;
    for (i = 1; (i < c) && (ok == TRUE); i++)
        {if (strcmp(v[i], "-ch") == 0)
            st->check = FALSE;

	 else if (strcmp(v[i], "-int") == 0)
            st->interact = FALSE;

	 else if (strcmp(v[i], "-scp") == 0)
            st->usescp = FALSE;

	 else if (strcmp(v[i], "-dbg") == 0)
	    {FREE(st->debug);
	     st->debug = STRSAVE(v[++i]);}

	 else if (strcmp(v[i], "-dt") == 0)
            st->vfyt = atoi(v[++i]);

	 else if (strcmp(v[i], "-watch") == 0)
            {st->watch    = TRUE;
	     st->watchout = TEXT;}

	 else if (strcmp(v[i], "-hwatch") == 0)
            {st->watch    = TRUE;
	     st->watchout = HTML;}

	 else if (strcmp(v[i], "+watch") == 0)
	    {l = i;
	     st->watchprogress = TRUE;
	     st->watchout      = TEXT;}

	 else if (strcmp(v[i], "+hwatch") == 0)
	    {l = i;
	     st->watchprogress = TRUE;
	     st->watchout      = HTML;}

	 else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'a':
		      for ( ; i < c; i++)
			  st->clargs = append_tok(st->clargs, ' ', v[i]);
		      st->cargs = append_tok(st->cargs, ' ', st->clargs);
                      ok = FALSE;
		      break;
                 case 'b':
                      st->build = FALSE;
                      break;
                 case 'c':
		      nstrncpy(st->stamp, BFLRG, v[++i], -1);
		      st->clearout = TRUE;
		      break;

		 case 'd':
		      full_path(st->dist, BFLRG, NULL, v[++i]);
		      break;
		 case 'e':
		      nstrncpy(st->do_code, BFLRG, v[++i], -1);
		      break;
		 case 'g':
                      l = i;
		      st->testhost = TRUE;
		      break;
		 case 'h':
		      help();
		      return(1);
		 case 'i':
		      st->install = FALSE;
		      break;
		 case 'I':
		      st->installwdh = TRUE;
		      break;
		 case 'l':
		      st->localinstall = TRUE;
		      break;
		 case 'o':
		      FREE(st->hostonly);
		      st->hostonly = STRSAVE(subst(v[++i], ",", " ", -1));
		      break;
		 case 'p':
		      nstrncpy(st->stamp, BFLRG, v[++i], -1);
		      st->reportprogress = TRUE;
		      break;
		 case 'q':
		      st->silent = TRUE;
		      break;
		 case 's':
		      st->send = FALSE;
		      break;
		 case 't':
		      st->tag = v[++i];
		      break;
		 case 'v':
/*
                      st->cargs = append_tok(st->cargs, ' ', v[i]);
		      st->verbose = TRUE;
 */
		      break;
		 case 'x':
		      st->usex = "";
		      break;
		 default:
                      break;};}

	 else if (v[i][0] == '+')
            {switch (v[i][1])
	        {case 'b':
		      st->send    = FALSE;
		      st->build   = TRUE;
		      st->install = FALSE;
		      break;
		 case 'i':
		      st->send    = FALSE;
		      st->build   = FALSE;
		      st->install = TRUE;
		      break;
		 case 's':
		      st->send    = TRUE;
		      st->build   = FALSE;
		      st->install = FALSE;
		      break;
		 case 'x':
		      strcpy(st->ssh, "ssh -q -o BatchMode=yes -o StrictHostKeyChecking=no");
		      break;};}

	 else if (IS_NULL(st->hostfile) == TRUE)
            full_path(st->hostfile, BFLRG, NULL, v[i]);

	 else
	    nstrncpy(st->stamp, BFLRG, v[i], -1);};

    return(l);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP - free up dynamic state */

static void cleanup(donetdes *st)
   {

    FREE(st->debug);
    FREE(st->hostonly);
    FREE(st->repo);
    FREE(st->maillist);
    FREE(st->mailer);
    FREE(st->system);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it out here */

int main(int c, char **v)
   {int l, logf, rv;
    char host[BFLRG], uhost[BFLRG], exe[BFLRG];
    char *p;

    signal(SIGINT, interrupt);

    umask(002);

/* find the directory with do-net */
    nstrncpy(exe, BFLRG, cwhich(v[0]), -1);
    nstrncpy(state.bindir, BFLRG, path_head(exe), -1);
    snprintf(state.incdir, BFLRG, "%s/include", path_head(state.bindir));
    snprintf(state.etcdir, BFLRG, "%s/etc", path_head(state.bindir));

    push_path(P_PREPEND, lpath, "/sbin");
    push_path(P_PREPEND, lpath, "/bin");
    push_path(P_PREPEND, lpath, "/usr/sbin");
    push_path(P_PREPEND, lpath, "/usr/bin");
    push_path(P_PREPEND, lpath, "/usr/local/bin");

/* locate the tools needed for subshells */
    build_path(NULL,
	       "sh", "rm", "ls", "mv", "find",
	       "grep", "cat", "awk", "date", "env",
	       "ssh", "scp", "hserve", "ptime",
	       "md5sum", "xterm",
	       NULL);

/* add the directory with do-net (and pact executables) to the path */
    push_path(P_PREPEND, lpath, state.bindir);

    INIT_PHASE(state.phases+PH_SETUP,       BY_NET,  "setup");
    INIT_PHASE(state.phases+PH_BUILD,       BY_HOST, "build");
    INIT_PHASE(state.phases+PH_NETINSTALL,  BY_NET,  "netinstall");
    INIT_PHASE(state.phases+PH_HOSTINSTALL, BY_HOST, "hostinstall");
    INIT_PHASE(state.phases+PH_CLEAN,       BY_HOST, "interrupt");

    state.n_hosts        = 0;
    state.n_nets         = 0;
    state.hosts          = NULL;
    state.nets           = NULL;
    state.err            = 0;
    state.finishing      = FALSE;
    state.havetimeouts   = FALSE;
    state.inintr         = FALSE;
    state.check          = TRUE;
    state.send           = TRUE;
    state.build          = TRUE;
    state.install        = TRUE;
    state.installwdh     = FALSE;
    state.verbose        = TRUE;
    state.silent         = FALSE;
    state.clearout       = FALSE;
    state.reportprogress = FALSE;
    state.watchprogress  = FALSE;
    state.watchout       = TEXT;
    state.localinstall   = FALSE;
    state.testhost       = FALSE;
    state.trace          = FALSE;
    state.watch          = FALSE;
    state.usescp         = TRUE;
    state.interact       = TRUE;
    state.vfyt           = 30;
    state.cargs          = NULL;
    state.clargs         = NULL;
    state.debug          = NULL;
    state.hostonly       = NULL;
    state.maillist       = NULL;
    state.mailer         = NULL;
    state.repo           = NULL;
    state.scripts        = NULL;
    state.system         = NULL;
    state.usex           = "-x";
    state.tag            = "none";
    state.separator      = "-----------------------------------------------------------------";

    strcpy(state.stamp, run(FALSE, "date +%%m_%%d_%%H_%%M"));
    unamef(host, BFLRG, "n");

    strcpy(uhost, host);
    p = strchr(uhost, '.');
    if (p != NULL)
       *p = '\0';

    state.finishing    = FALSE;
    state.havetimeouts = FALSE;
    state.inintr       = FALSE;

    strcpy(state.handtout, "pass");

    state.cargs = append_tok(state.cargs, ' ', "-v");
    state.cargs = append_tok(state.cargs, ' ', "-t");
    state.cargs = append_tok(state.cargs, ' ', state.stamp);

/* this can be overridden in the host file */
    csetenv("ReportConfig", "yes");

    l = process_args(&state, c, v);

    logf = ((state.reportprogress != TRUE) &&
	    (state.watchprogress != TRUE) &&
	    (state.testhost != TRUE) &&
	    (state.debug == NULL));
    readhost(&state, logf);

    init_aux(&state);

    snprintf(state.ssh, BFLRG,
	     "%s -q %s -o BatchMode=yes -o StrictHostKeyChecking=no",
	     cwhich("ssh"), state.usex);

    rv = 0;

/* debug downstream scripts */
    if (state.debug != NULL)
       rv = debug(&state);

/* let us know how we are doing - at the terminal */
    else if (state.reportprogress == TRUE)
       rv = progress(&state);

/* let us know how we are doing - in its own window */
    else if (state.watchprogress == TRUE)
       rv = watch(&state, c-l, v+l);

/* verify that all hosts specified in a host file are available */
    else if (state.testhost == TRUE)
       rv = testhosts(&state, c-l, v+l);

/* clean out the trash from an old or failed do-net */
    else if (state.clearout == TRUE)
       rv = clearout(&state);

    else if (IS_NULL(state.hostfile) == TRUE)
       {printf("\n");
	printf("Must specify a host file to describe actions\n");
	printf("\n");
	rv = 2;}

    else if (file_exists(state.hostfile) == FALSE)
       {printf("\n");
	printf("Host file %s does not exist", state.hostfile);
	printf("\n");
	rv = 3;}

/* run the session */
    else
       rv = session(&state, exe, host, uhost);

    if (Log != NULL)
       fclose(Log);

    cleanup(&state);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
