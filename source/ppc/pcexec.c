/*
 * PNEXEC.C - PDBNet system exec utility
 *
 */

#include "cpyright.h"

#include "ppc_int.h"
#include "scope_proc.h"

typedef struct s_descriptors descriptors;

struct s_descriptors
   {int quiet;
    int reason;
    char *name;
    PROCESS *pp;
    SC_evlpdes *pe;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHILD_HAS_TXT - handle text from the child process */

static void child_has_txt(int fd, int mask, void *a)
   {char s[MAXLINE];
    PROCESS *pp;
    descriptors *pd;

    pd = (descriptors *) a;
    pp = pd->pp;

    while (SC_gets(s, MAXLINE, pp) != NULL)
       PRINT(stdout, "%s", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TTY_HAS_TXT - handle text from the tty */

static void tty_has_txt(int fd, int mask, void *a)
   {char s[MAX_BFSZ];
    PROCESS *pp;
    descriptors *pd;

    pd = (descriptors *) a;
    pp = pd->pp;

    SC_change_term_state(STDIN_FILENO, SC_TERM_RAW, FALSE, NULL);

    if (SC_fgets(s, MAX_BFSZ, stdin) != NULL)
       SC_printf(pp, "%s", s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_END - return TRUE if the process has ended
 *             - do the SC_close now since the TTY may be in
 *             - RAW mode and we want the output to
 *             - look nice (PTY's do this)
 */

static int process_end(int *prv, void *a)
   {int status, reason, quiet, ex, rv;
    char *name;
    PROCESS *pp;
    descriptors *pd;

    pd = (descriptors *) a;
    pp    = pd->pp;
    name  = pd->name;
    quiet = pd->quiet;

    SC_check_children();

    ex = FALSE;
    if ((pp != NULL) && (SC_status(pp) != SC_RUNNING))
       {status = pp->status;
        reason = pp->reason;

        pd->reason = reason;
	if (prv != NULL)
	   *prv = reason;

/* get anything remaining from the child */
        child_has_txt(pp->io[0], 0, a);

        rv = SC_close(pp);
	if (rv == FALSE)
	   CFREE(pp);

        if (!quiet)
           PRINT(stdout, "\nProcess %s terminated (%d %d)\n",
	         name, status, reason);

	ex = TRUE;};

    return(ex);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POLL_MODE - poll the process and the tty */

static int poll_mode(descriptors *pd)
   {int quiet, ok;
    char s[MAX_BFSZ];
    char *t, *msg;
    PROCESS *pp;

    quiet = pd->quiet;
    pp    = pd->pp;

    pd->reason = -1;
    while (TRUE)
       {ok = SC_process_status(pp);
	SC_ASSERT(ok == TRUE);

	while (SC_gets(s, MAX_BFSZ, pp) != NULL)
           PRINT(stdout, "%s", s);

        if (process_end(NULL, pd))
           break;

        SC_unblock_file(stdin);
        t = SC_fgets(s, MAX_BFSZ, stdin);
        SC_block_file(stdin);
        if (t != NULL)
           SC_printf(pp, "%s", s);

	msg = SC_error_msg();
        if (msg[0] != '\0')

/* close now since the TTY may be in RAW mode and we want the output to
 * look nice (PTY's do this)
 */
           {SC_close(pp);
	    if (!quiet)
               PRINT(stdout, "%s\n\n", msg);
            break;};};

    return(pd->reason);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTERRUPT_MODE - handle the child and tty I/O via interrupts */

static int interrupt_mode(descriptors *pd)
   {int pi, rv;
    int acc, rej;
    SC_evlpdes *pe;
    PROCESS *pp;

    pp = pd->pp;

    SC_set_io_interrupts(FALSE);

/* create the event loop state */
    pe      = SC_make_event_loop(NULL, NULL, process_end, -1, -1, -1);
    pe->raw = (pp->medium == USE_PTYS);

/* adjust the polling masks */
    SC_event_loop_get_masks(pe, &acc, &rej);
    rej |= POLLHUP;
    SC_event_loop_set_masks(pe, acc, rej);

/* register the I/O channels for the event loop to monitor */
    pi  = SC_register_event_loop_callback(pe, SC_FILE_I, stdin,
					  tty_has_txt, NULL, -1);
    pi &= SC_register_event_loop_callback(pe, SC_PROCESS_I, pp,
					  child_has_txt, NULL, -1);

/* if all channels are OK activate the interrupt handling */
    SC_set_io_interrupts(pi);
    if (pi == TRUE)
       SC_catch_event_loop_interrupts(pe, pi);

    SC_unblock_file(stdin);

    rv = SC_event_loop(pe, pd, -1);
    SC_ASSERT(rv == TRUE);

    SC_block_file(stdin);

    SC_free_event_loop(pe);

    return(pd->reason);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ERROR_HANDLER - trap various signals and restore the terminal */

static void error_handler(int sig)
   {int err, ev;

    SC_reset_terminal();

    err = SC_block_file(stdin);
    SC_ASSERT(err == TRUE);

    if (sig == SIGALRM)
       {PRINT(stdout, "PCEXEC timed out\n");
	ev = SC_EXIT_TIMEOUT;}
    else
       {PRINT(stdout, "PCEXEC exiting with signal %d\n", sig);
	ev = SC_EXIT_SELF;};

    exit(ev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USAGE - print out a usage message for PCEXEC */

static void usage(void)
   {

    printf("\nUsage: pcexec [-pst] [-c n] [-d] [-h] [-i] <prog> [<arg1> ...]\n");
    printf("       pcexec -f <host>\n");
/*    printf("       pcexec -r [<arg1> ...]\n"); */
    printf("\n");

    printf("   The first form runs <prog> as a child process.\n");
    printf("   The second form acts as a remote file access server.\n");
/*    printf("   The third form acts as a parallel communications server.\n"); */
    printf("\n");

    printf("   The forms for <prog> are:\n");
    printf("       <name>              - run <name> on local <host>\n");
    printf("       <host>:<name>       - run <name> on remote <host>\n");
/*
    printf("       <CPU>@<name>        - run <name> on processor <CPU>\n");
    printf("       <host>:<CPU>@<name> - run <name> on processor <CPU>\n");
    printf("   The last two are not yet completed.\n");
*/
    printf("\n");

    printf("   The full syntax for <host> is:\n");
    printf("       <hostname>                         or\n");
    printf("       <hostname>,<username>\n");
    printf("   Note: whitespace is NOT allowed\n");
    printf("\n");

    printf("   Options:\n");
    printf("      c    - timeout after n seconds\n");
    printf("      d    - print diagnostic details\n");
    printf("      h    - this help message\n");
    printf("      i    - poll explicitly instead of using system call\n");
    printf("      l    - when acting as a file server, log transactions to\n");
    printf("             log file in your home directory\n");
    printf("      p    - use pipes for communications\n");
    printf("      q    - print only messages from the child\n");
    printf("      s    - use sockets for communications\n");
    printf("      t    - use pseudo terminals for communications\n");
    printf("\n");
    
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test PPC */

int main(int argc, char **argv)
   {int i, interrupts, ret, quiet, access_file, lg, parallel, to, info;
    char mode[5];
    PROCESS *pp;
    descriptors d;

/* process the command line arguments */
    quiet       = FALSE;
    interrupts  = TRUE;
    parallel    = FALSE;
    access_file = FALSE;
    lg          = FALSE;
    info        = FALSE;
    to          = 1000000;
    SC_strncpy(mode, 5, "w", -1);
    if (argc < 2)
       {usage();
        return(1);};

    for (i = 1; argv[i] != NULL; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'c' :
		      to = SC_stoi(argv[++i]);
		      break;
                 case 'd' :
		      info = TRUE;
		      break;
                 case 'f' :
		      access_file = TRUE;
		      break;
                 case 'h' :
		      usage();
		      return(1);
		      break;
                 case 'i' :
		      interrupts = FALSE;
		      break;
                 case 'l' :
		      lg = TRUE;
		      break;
                 case 'r' :
		      parallel = TRUE;
		      i++;
		      break;
                 case 'p' :
		      SC_strncpy(mode, 5, "wp", -1);
		      break;
                 case 'q' :
		      quiet = TRUE;
		      break;
                 case 's' :
		      SC_strncpy(mode, 5, "ws", -1);
		      break;
                 case 't' :
		      SC_strncpy(mode, 5, "wt",  -1);
		      break;};}
         else
            break;};

/* trap the following signals to restore the terminal state */

#if defined(HAVE_POSIX_SYS)
    SC_signal_n(SIGSEGV, error_handler, NULL, 0);
    SC_signal_n(SIGABRT, error_handler, NULL, 0);
    SC_signal_n(SIGTERM, error_handler, NULL, 0);
    SC_signal_n(SIGQUIT, error_handler, NULL, 0);
    SC_signal_n(SIGILL, error_handler, NULL, 0);
    SC_signal_n(SIGINT, error_handler, NULL, 0);
#endif

    ret = 0;

    if (access_file)
       SC_file_access(lg);

    else if (parallel)
       ret = PN_process_access(argv, "rsb+");

    else
       {if ((strcmp(argv[i], "ftp") == 0) ||
	    (strcmp(argv[i], "telnet") == 0))
	   {if (strcmp(mode, "wt") != 0)
	       {if (!quiet)
		   PRINT(stdout,
			 "\nWarning: run with -t flag\n\n");};};

/* print this before we potentially go into RAW mode (PTY's do this) */
	if (!quiet)
	   PRINT(stdout, "\nRunning process: %s\n\n", argv[i]);

/* set the alarm */
        SC_timeout(to, error_handler, NULL, 0);

	pp = PN_open_process(argv+i, NULL, mode);
	if (pp == NULL)
	   {if (!quiet)
	       {PRINT(stdout, "%s\n", SC_error_msg());
		PRINT(stdout, "\nFailed to open: %s\n\n", argv[i]);};
	    error_handler(0);};

/* reset the alarm */
        SC_timeout(0, error_handler, NULL, 0);

	SC_setbuf(stdout, NULL);

/* set the alarm */
        SC_timeout(to, error_handler, NULL, 0);

	d.pp    = pp;
	d.name  = argv[i];
	d.quiet = quiet;

	if (interrupts)
	   ret = interrupt_mode(&d);

	else
	   ret = poll_mode(&d);

/* reset the alarm */
        SC_timeout(0, error_handler, NULL, 0);

	if (!quiet)
	   PRINT(stdout, "Process test %s ended\n\n", argv[i]);};

    if (info == TRUE)
       {PRINT(stdout, "\n");
	PRINT(stdout, "  ppid(%d)", getppid());
	PRINT(stdout, "  ptid(%d)", tcgetpgrp(0));
	PRINT(stdout, "  pgid(%d)", getpgrp());
	PRINT(stdout, "  pid(%d)",  getpid());
	PRINT(stdout, "  background(%d)", SC_is_background_process(-1));
	PRINT(stdout, "\n");};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
