/*
 * SCEXTS.C - test SC_exec_server
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_proc.h"

static int
 single = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHILD_HAS_TXT - handle text from the child process */

static int child_has_txt(int *prv, void *a)
   {int id;
    char s[MAXLINE];
    char *p, *t;
    PROCESS *pp;

    pp = (PROCESS *) a;

    while (SC_gets(s, MAXLINE, pp) != NULL)
       {if (strncmp(s, "Job[", 4) == 0)
	   {p  = strchr(s, ':');
	    p += 2;
	    t  = strtok(s+4, "]");
	    id = SC_stoi(t);
            io_printf(stdout, "[job %2d]: %s", id, p);}

        else if (strncmp(s, "Srv:", 4) == 0)
	   io_printf(stdout, "[server]: %s", s+5);

	else
	   io_printf(stdout, "[unknwn]: %s", s);};

    *prv = TRUE;

    return(single);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TTY_HAS_TXT - handle text from the tty */

static void tty_has_txt(int fd, int mask, void *a)
   {char s[MAX_BFSZ];
    char *shell, *t, *p, *msg, *dir;
    PROCESS *pp;
    static int count = 1;

    shell = getenv("SHELL");
    dir   = SC_getcwd();

    pp = (PROCESS *) a;

    t = SC_fgets(s, MAX_BFSZ, stdin);
    if (t != NULL)
       {t   = SC_strtok(s, "\n\r", p);
	msg = SC_dsnprintf(FALSE, "%s %d:%s:%s:%s", _SC_EXEC_JOB,
			   count++, shell, dir, t);

	SC_printf(pp, "%s", msg);};

    SFREE(dir);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_SRV - test SC_exec_server */

static int test_srv(char *mode, int dt)
   {int i, pi, st;
    char *ca[10];
    PROCESS *pp;
    SC_evlpdes *pe;

    i = 0;
    ca[i++] = "dmake";
    ca[i++] = "-srv";
    ca[i++] = NULL;

    pp = SC_open(ca, NULL, mode, NULL);

/* create the event loop */
    SC_gs.io_interrupts_on = FALSE;

    pe = SC_make_event_loop(NULL, NULL, child_has_txt, -1, -1, -1);
    pi = SC_register_event_loop_callback(pe, SC_FILE_I, stdin,
					 tty_has_txt,
					 NULL,
					 -1);

    SC_unblock_file(stdin);

/* wait for commands to complete
 * give a heartbeat every 30 seconds
 */
    st = SC_event_loop(pe, pp, DEFAULT_TIMEOUT);

    SC_free_event_loop(pe);

    SC_block_file(stdin);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_CHLD - test limits of fork/exec wrt shell */

static int test_chld(char **ca, char *mode, int dt)
   {int st;
    extern void _SC_exec_test(char **argv, char **envp, char *mode);

    st = TRUE;

    _SC_exec_test(ca, NULL, mode);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test SC_exec_server */

int main(int argc, char **argv, char **envp)
   {int i, st, to, opt;
    char mode[5];

    SC_setbuf(stdout, NULL);

/* process the command line arguments */
    to  = DEFAULT_TIMEOUT;
    opt = 1;

    strcpy(mode, "ap");
    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'c' :
		      to = SC_stoi(argv[++i]);
		      break;

	         case 'h' :
		      printf("\nUsage: scexts [-pst] [-c n] [-e] [-h]\n");
		      printf("   Options:\n");
		      printf("      c - time interval for status checking in milliseconds\n");
		      printf("      h - print this help message\n");
		      printf("      e - run exec test\n"); 
		      printf("      p - use pipes for communications\n");
		      printf("      s - use sockets for communications\n");
		      printf("      t - use pseudo terminals for communications\n");
		      printf("\n");
		      exit(1);

		 case 'e' :
		      st  = test_chld(argv + i + 1, mode, to);
                      return(st);
		      break;
                 case 'p' :
                      strcpy(mode, "ap");
		      break;
                 case 's' :
                      strcpy(mode, "as");
		      break;
                 case 't' :
                      strcpy(mode, "at");
		      break;};}
         else
            break;};

    switch (opt)
       {case 1:
	     st = test_srv(mode, to);
	     break;};

    io_printf(stdout, "\n");

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
