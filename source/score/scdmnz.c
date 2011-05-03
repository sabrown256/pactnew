/*
 * SCDMNZ.C - daemonize a program
 *          - that is to say run it at a specified interval
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"
#include "scope_proc.h"

typedef struct s_info info;

struct s_info
   {int tc;
    int ti;
    int exit;
    long n_tries;
    long n_timeouts;
    long n_failures;
    FILE *fp;
    PROCESS *pp;
    JMP_BUF cpu;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXIT_DAEMON - finish off the session */

static void exit_daemon(info *facts, int status)
   {

    if ((facts->fp != stdout) && (facts->fp != stderr) && (facts->fp != NULL))
       io_close(facts->fp);

    exit(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSE_COMMAND - finish off the child command and clean up */

static void close_command(PROCESS *pp)
   {

    if (SC_process_alive(pp))
       SC_close(pp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTERRUPT_HANDLER - handle interrupt signals */

static void interrupt_handler(int sig)
   {info *facts;

    facts = SC_get_context(interrupt_handler);

    close_command(facts->pp);

    exit_daemon(facts, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT_HANDLER - handle report signals (SIGUSR1) */

static void report_handler(int sig)
   {char *name;
    FILE *lf;
    info *facts;

    facts = SC_get_context(report_handler);

    name = SC_dsnprintf(FALSE, "%s/.log.dmnz", getenv("HOME"));

    lf = io_open(name, "a");

    io_printf(lf, "----------------------------------------------------\n");
    io_printf(lf, "Time Interval: %d\n", facts->ti);
    io_printf(lf, "Timeout:       %d\n", facts->tc);
    io_printf(lf, "# runs:        %ld\n", facts->n_tries);
    io_printf(lf, "# timeouts:    %ld\n", facts->n_timeouts);
    io_printf(lf, "# failures:    %ld\n", facts->n_failures);

    io_close(lf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_HANDLER - handle I/O signals */

static void io_handler(int sig)
   {int n;
    char bf[1024];
    info *facts;

    facts = SC_get_context(io_handler);

    n = SC_read_sigsafe(0, bf, 1024);
    if (n > 0)
       {io_printf(stdout, "--> |%s|\n", bf);};

    io_printf(stdout, "I did too trap the SIGIO\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TIMEOUT_HANDLER - handle timeout signals
 *                 - we get here whether or not the command exited
 *                 - this is by design (to control timings)
 */

static void timeout_handler(int sig)
   {info *facts;

    facts = SC_get_context(timeout_handler);

    if (!facts->exit)
       facts->n_timeouts++;

    close_command(facts->pp);

    LONGJMP(facts->cpu, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_OUTPUT - handle output from the child process */

static void process_output(int fd, int mask, void *a)
   {char s[MAXLINE+1];
    PROCESS *pp;
    info *facts;
    FILE *fp;

    facts = (info *) a;
    pp = facts->pp;
    fp = facts->fp;

    while (SC_gets(s, MAXLINE, pp) != NULL)
       io_printf(fp, "%s", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_END - return TRUE iff the process has exited
 *             - return the exit status in RV
 */

static int process_end(int *prv, void *a)
   {int ex, rv;
    PROCESS *pp;
    info *facts;

    facts = (info *) a;
    pp    = facts->pp;

    ex = FALSE;
    if (SC_process_status(pp) != SC_RUNNING)
       {rv = pp->reason;

	facts->exit = TRUE;
        if (rv != 0)
           facts->n_failures++;

        *prv = rv;

	SC_sleep(100);
	process_output(0, 0, a);

	ex = TRUE;};

    return(ex);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTERRUPT_MODE - do interrupt driven I/O */

static int interrupt_mode(info *facts)
   {int rv;
    PROCESS *pp;

    rv = 0;

    pp = facts->pp;

    SC_unblock(pp);

    rv = SC_process_event_loop(pp, facts, process_end, process_output, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_COMMAND - run an instance of the command
 *             - allowing upto TO seconds
 */

static int run_command(info *facts, char **cmnd, int to, int na)
   {int rv;
    PROCESS *pp;

    SC_timeout(to, timeout_handler, facts);

    pp = SC_open(cmnd, NULL, "apo", NULL);
    if (SC_process_alive(pp))
       {facts->pp = pp;
	rv = interrupt_mode(facts);
	close_command(pp);}

    else
       rv = 1;

/* force a timeout so that we unconditionally hit the timeout
 * guaranteeing this phase to take TO seconds
 */
    if (na > 1)
       SC_sleep(1000*to);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHOW_HELP - show the usage and options */

static void show_help(void)
   {

    io_printf(stdout, "Run command periodically in the manner of a daemon\n\n");
    io_printf(stdout, "Usage: dmnz [-f <file>] [-h] [-i #] [-n #] [-t #] <command>\n\n");

    io_printf(stdout, "   f  file to receive all output\n");
    io_printf(stdout, "   h  this help message\n");
    io_printf(stdout, "   i  time limit in seconds for one instance to run\n");
    io_printf(stdout, "   n  number of times to the command (default is indefinite)\n");
    io_printf(stdout, "   t  time in seconds between invocations\n");
    io_printf(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, ia, na;
    int ti, tc, tr, rv;
    char **cmnd, *lf;
    info facts;
    FILE *fp;

    memset(&facts, 0, sizeof(facts));

    rv = 0;
    ti = 60;
    tc = ti;
    na = INT_MAX;
    lf = NULL;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'f':
                      lf = v[++i];
                      break;
		 case 'h':
                      show_help();
                      return(1);
                      break;
		 case 'i':
                      tc = SC_stoi(v[++i]);
                      break;
		 case 'n':
                      na = SC_stoi(v[++i]);
                      if (na < 0)
			 na = INT_MAX;
                      break;
		 case 't':
                      ti = SC_stoi(v[++i]);
                      break;
		 default:
		      io_printf(stdout, "Unknown option: %s\n", v[i]);
		      return(1);
                      break;};}
         else
	    break;};

    if (tc > ti)
       io_printf(stdout, "Timeout exceeds time interval (%d > %d) - resetting\n",
		 tc, ti);

    tc = min(tc, ti);
    tr = ti - tc;

/* start up the output logging */
    if (lf != NULL)
       fp = io_open(lf, "w");
    else
       fp = stdout;

    facts.tc         = tc;
    facts.ti         = ti;
    facts.n_tries    = 0;
    facts.n_timeouts = 0;
    facts.n_failures = 0;
    facts.fp         = fp;

    cmnd = v + i;

    SC_signal_n(SIGINT, interrupt_handler, &facts);
    SC_signal_n(SIGUSR1, report_handler, &facts);
    SC_signal_n(SC_SIGIO, io_handler, &facts);

    SC_setbuf(stdout, NULL);

    for (ia = 0; ia < na; ia++)
        {facts.n_tries++;
	 facts.exit = FALSE;

/* setup the non-local return in case of timeout */
	 if (SETJMP(facts.cpu) == 0)
	    rv = run_command(&facts, cmnd, tc, na);

/* sleep for TR seconds so that the next instance will be run
 * in exactly TI = (TC + TR) seconds
 */
	 if ((tr > 0) && (ia < na-1))
	    SC_sleep(1000*tr);};

    exit_daemon(&facts, rv);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
