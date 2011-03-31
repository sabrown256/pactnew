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
typedef struct s_descriptor descriptor;

struct s_info
   {int tc;
    int ti;
    int exit;
    long n_tries;
    long n_timeouts;
    long n_failures;};

struct s_descriptor
   {PROCESS *pp;
    info *facts;};

static FILE
 *fp = NULL;

static PROCESS
 *pp = NULL;

static JMP_BUF
 run_instance;

static info
 facts;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXIT_DAEMON - finish off the session */

static void exit_daemon(int status)
   {

    if ((fp != stdout) && (fp != stderr) && (fp != NULL))
       io_close(fp);

    exit(status);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLOSE_COMMAND - finish off the child command and clean up */

static void close_command(void)
   {

    if (SC_process_alive(pp))
       SC_close(pp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTERRUPT_HANDLER - handle interrupt signals */

static void interrupt_handler(int sig)
   {

    close_command();

    exit_daemon(1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT_HANDLER - handle report signals (SIGUSR1) */

static void report_handler(int sig)
   {char *name;
    FILE *lf;

    name = SC_dsnprintf(FALSE, "%s/.log.dmnz", getenv("HOME"));

    lf = io_open(name, "a");

    io_printf(lf, "----------------------------------------------------\n");
    io_printf(lf, "Time Interval: %d\n", facts.ti);
    io_printf(lf, "Timeout:       %d\n", facts.tc);
    io_printf(lf, "# runs:        %ld\n", facts.n_tries);
    io_printf(lf, "# timeouts:    %ld\n", facts.n_timeouts);
    io_printf(lf, "# failures:    %ld\n", facts.n_failures);

    io_close(lf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IO_HANDLER - handle I/O signals */

static void io_handler(int sig)
   {int n;
    char bf[1024];

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
   {

    if (!facts.exit)
       facts.n_timeouts++;

    close_command();

    LONGJMP(run_instance, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_OUTPUT - handle output from the child process */

static void process_output(int fd, int mask, void *a)
   {char s[MAXLINE+1];
    PROCESS *pp;
    descriptor *pd;

    pd = (descriptor *) a;
    pp = pd->pp;

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
    descriptor *pd;
    info *pf;

    pd = (descriptor *) a;
    pp = pd->pp;
    pf = pd->facts;

    ex = FALSE;
    if (SC_process_status(pp) != SC_RUNNING)
       {rv = pp->reason;

	pf->exit = TRUE;
        if (rv != 0)
           pf->n_failures++;

        *prv = rv;

	SC_sleep(100);
	process_output(0, 0, a);

	ex = TRUE;};

    return(ex);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTERRUPT_MODE - do interrupt driven I/O */

static int interrupt_mode(void)
   {int rv;
    descriptor d;

    rv = 0;

    SC_unblock(pp);

    d.pp    = pp;
    d.facts = &facts;

    rv = SC_process_event_loop(pp, &d, process_end, process_output, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_COMMAND - run an instance of the command
 *             - allowing upto TO seconds
 */

static int run_command(char **cmnd, int to, int na)
   {int rv;

    SC_timeout(to, timeout_handler);

    pp = SC_open(cmnd, NULL, "apo", NULL);
    if (SC_process_alive(pp))
       {rv = interrupt_mode();
	close_command();}

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

    facts.tc         = tc;
    facts.ti         = ti;
    facts.n_tries    = 0;
    facts.n_timeouts = 0;
    facts.n_failures = 0;

/* start up the output logging */
    if (lf != NULL)
       fp = io_open(lf, "w");
    else
       fp = stdout;

    cmnd = v + i;

    SC_signal(SIGINT, interrupt_handler);
    SC_signal(SIGUSR1, report_handler);
    SC_signal(SC_SIGIO, io_handler);

    SC_setbuf(stdout, NULL);

    for (ia = 0; ia < na; ia++)
        {facts.n_tries++;
	 facts.exit = FALSE;

/* setup the non-local return in case of timeout */
	 if (SETJMP(run_instance) == 0)
	    rv = run_command(cmnd, tc, na);

/* sleep for TR seconds so that the next instance will be run
 * in exactly TI = (TC + TR) seconds
 */
	 if ((tr > 0) && (ia < na-1))
	    SC_sleep(1000*tr);};

    exit_daemon(rv);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
