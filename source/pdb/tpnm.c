/*
 * TPCM.C - multiple children tests for PDBNet
 *
 */

#include "cpyright.h"

#include "pdb.h"
#include "scope_proc.h"
#include "scope_dp.h"

typedef struct s_descriptors descriptors;

struct s_descriptors
   {int quiet;
    int reason;
    int n_processes;
    char *name;
    PROCESS **processes;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHILD_HAS_TXT - handle text from the child process */

static void child_has_txt(int fd, int mask, void *a)
   {int i, n;
    char s[MAXLINE];
    PROCESS *p, **pp;
    descriptors *pd;

    pd = (descriptors *) a;
    n  = pd->n_processes;
    pp = pd->processes;

    p = NULL;
    for (i = 0; i < n; i++)
        if (pp[i] != NULL)
	   if (pp[i]->io[0] == fd)
	      {p = pp[i];
	       break;};

    if (p != NULL)
       while (SC_gets(s, MAXLINE, p) != NULL)
          PRINT(stdout, "\n  From #%d | %s", i + 1, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TTY_HAS_TXT - handle text from the tty */

static void tty_has_txt(int fd, int mask, void *a)
   {int i;
    char s[MAX_BFSZ], *t, *pt;
    PROCESS *p, **pp;
    descriptors *pd;

    pd = (descriptors *) a;
    pp = pd->processes;

    t = SC_fgets(s, MAX_BFSZ, stdin);
    if (t != NULL)
       {t = SC_strtok(s, " ", pt);
	i = SC_stoi(t);
	p = pp[i - 1];
	if (p == NULL)
	   {PRINT(stdout, "\nError: child #%d previously terminated\n\n", i);
	    exit(1);};

	t = SC_strtok(NULL, "", pt);
	SC_printf(p, "%s", t);

        PRINT(stdout, "\n  To   #%d | %s", i, t);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_END - return the number of process which have ended
 *             - do the SC_close now since the TTY may be in
 *             - RAW mode and we want the output to
 *             - look nice (PTY's do this)
 */

static int process_end(int *prv, void *a)
   {int i, n, status, reason, quiet, ret, rv;
    PROCESS *p, **pp;
    descriptors *pd;

    pd = (descriptors *) a;
    n     = pd->n_processes;
    pp    = pd->processes;
    quiet = pd->quiet;

    SC_check_children();

    rv  = 0;
    ret = 0;
    for (i = 0; i < n; i++)
        {p = pp[i];
	 if (p != NULL)
	    if (SC_status(p) != SC_RUNNING)
	       {status = p->status;
		reason = p->reason;

		pd->reason = reason;
		rv         = reason;

/* get anything remaining from the child */
		child_has_txt(p->io[0], 0, a);

		if (!quiet)
		   PRINT(stdout, "\n    Child #%d (%d) terminated (%d %d)\n",
			 i + 1, p->id, status, reason);

		SC_close(p);
		pp[i] = NULL;

		ret++;};};

    if (prv != NULL)
       *prv = rv;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_COUNT - return the number of active child processes */

static int process_count(void *a)
   {int i, n, ret;
    PROCESS **pp;
    descriptors *pd;

    pd = (descriptors *) a;
    n  = pd->n_processes;
    pp = pd->processes;

    ret = 0;
    for (i = 0; i < n; i++)
        if (pp[i] != NULL)
	   ret++;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEAN_UP - clean up this session */

static void clean_up(int sig)
   {

    SC_block_file(stdin);

    if (sig == SIGALRM)
       {PRINT(stdout, "Multiple child test timed out\n");
	exit(123);}
    else
       exit(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTERRUPT_MODE - handle the child and tty I/O via interrupts */

static int interrupt_mode(descriptors *pd)
   {int i, n, pi, rv;
    PROCESS **pp;
    SC_evlpdes *pe;

    n  = pd->n_processes;
    pp = pd->processes;

    SC_set_io_interrupts(TRUE);

/* create the event loop state */
    pe = SC_make_event_loop(NULL, NULL, process_end, -1, -1, -1);

/* register the I/O channels for the event loop to monitor */
    pi  = SC_register_event_loop_callback(pe, SC_FILE_I, stdin,
					  tty_has_txt, NULL, -1);
    for (i = 0; i < n; i++)
        {if (pp[i] != NULL)
	    pi &= SC_register_event_loop_callback(pe, G_PROCESS_I, pp[i],
						  child_has_txt, NULL, -1);};

/* if all channels are OK activate the interrupt handling */
    SC_set_io_interrupts(pi);
    if (pi == TRUE)
       SC_catch_event_loop_interrupts(pe, pi);

    rv = SC_event_loop(pe, pd, -1);

    SC_free_event_loop(pe);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POLL_MODE - poll the process and the tty */

static int poll_mode(descriptors *pd)
   {int i, n;
    char s[MAX_BFSZ];
    char *t, *pt;
    PROCESS *p, **pp;

    pp = pd->processes;
    n  = pd->n_processes;

    SC_signal_n(SIGINT, clean_up, NULL, 0);

    while (process_count(pd) > 0)
       {for (i = 0; i < n; i++)
            {p = pp[i];
	     if (p != NULL)
	        while (SC_gets(s, MAX_BFSZ, p) != NULL)
		   PRINT(stdout, "\n  From #%d | %s", i + 1, s);};

        if (process_end(NULL, pd))
           continue;

        SC_unblock_file(stdin);
        t = SC_fgets(s, MAX_BFSZ, stdin);
        SC_block_file(stdin);

        if (t != NULL)
	   {t = SC_strtok(s, " ", pt);
            i = SC_stoi(t);
	    p = pp[i - 1];
            if (p == NULL)
	       {PRINT(stdout, "\nError: child #%d previously terminated\n\n", i);
		exit(1);};

            t = SC_strtok(NULL, "", pt);
	    SC_printf(p, "%s", t);

            PRINT(stdout, "\n  To   #%d | %s", i, t);
	    sleep(1);};};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test PDBNet with multiple children */

int main(int argc, char **argv, char **envp)
   {int i, n, interrupts, ret, to;
    char mode[5], *prog, **argl;
    PROCESS *p, **pp;
    descriptors d;

/* process the command line arguments */
    interrupts  = TRUE;
    n           = 2;
    to          = DEFAULT_TIMEOUT;
    SC_strncpy(mode, 5, "wp", -1);
    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'c' : to = SC_stoi(argv[++i]);
                            break;
	         case 'h' :
		      printf("\nUsage: tpcm [-pst] [-c n] [-h] [-i] [-n <#>] [<prog> [<arg1> ...]]\n");
		      printf("   Options:\n");
		      printf("      c - timeout after n seconds\n");
		      printf("      h - print this help message\n");
		      printf("      i - poll explicitly instead of using system call\n");
		      printf("      n - # of copies to spawn\n");
		      printf("\n");
		      printf("      p - use pipes for communications\n");
		      printf("      s - use sockets for communications\n");
		      printf("      t - use pseudo terminals for communications\n");
		      printf("\n");
		      exit(1);

                 case 'i' :
                      interrupts = FALSE;
		      break;
                 case 'n' :
                      n = SC_stoi(argv[++i]);
		      break;
                 case 'p' :
                      SC_strncpy(mode, 5, "wp", -1);
		      break;
                 case 's' :
                      SC_strncpy(mode, 5, "ws", -1);
		      break;
                 case 't' :
                      SC_strncpy(mode, 5, "wt", -1);
		      break;};}
         else
            break;};

    SC_setbuf(stdout, NULL);

    argc -= i;
    if (argc < 1)
       {prog = CSTRSAVE("tpcc");
        argl = CMAKE_N(char *, 2);
        argl[0] = CSTRSAVE("tpcc");
        argl[1] = NULL;}
    else
       {prog = argv[i];
	argl = argv + i;};

/* print this before we potentially go into RAW mode (PTY's do this) */
    PRINT(stdout, "\nRunning process: %s\n", prog);
    if (to < DEFAULT_TIMEOUT)
       PRINT(stdout, "   Timing out in %d seconds\n", to);

    SC_unblock_file(stdin);

    pp = CMAKE_N(PROCESS *, n);

    for (i = 0; i < n; i++)
        {p = PN_open_process(argl, envp, mode);
	 if (p == NULL)
	    {PRINT(stdout, "\nError: failed to open copy #%d of %s\n\n",
		   i + 1, prog);
	     exit(1);};

	 pp[i] = p;

	 PRINT(stdout, "\n  Child #%d (%s) is %d", i + 1, prog, p->id);
	 SC_unblock(p);};

    PRINT(stdout, "\n");

    d.n_processes = n;
    d.processes   = pp;
    d.name        = prog;
    d.quiet       = FALSE;

/* set the alarm */
    SC_timeout(to, clean_up, NULL, 0);

    if (interrupts)
       ret = interrupt_mode(&d);
    else
       ret = poll_mode(&d);

/* reset the alarm */
    SC_timeout(0, clean_up, NULL, 0);

    PRINT(stdout, "\nProcess test %s ended\n\n", prog);

    SC_block_file(stdin);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
