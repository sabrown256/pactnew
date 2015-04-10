/*
 * TPCB.C - test binary connections - parent side
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"
#include "scope_proc.h"
#include "scope_dp.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROCESS_BINARY - do some binary data exchanges */

static void process_binary(PROCESS *pp)
   {char s[MAXLINE+2], type[80], *msg, *t;
    long ni, number;
    PDBfile *file;
    void *space;

    if (pp != NULL)
       {file = pp->vif;

	printf("P> Processing binary transmission:\n");

	SC_gets(s, MAXLINE, pp);
	msg = SC_strtok(s, " \n", t);
	if (msg == NULL)
	   {printf("P> Binary data receive failed: no data\n");
	    return;};

	SC_strncpy(type, 80, msg, -1);
	ni = SC_stol(SC_strtok(NULL, " \n", t));

	space = _PD_alloc_entry(file, type, ni);

	printf("P> Reading binary data ... ");
	number = SC_read(space, type, ni, pp);
	printf("done\n");

	while (SC_gets(s, MAXLINE, pp) != NULL)
	   printf("C> %s", s);

	if (number == ni)
	   printf("P> Binary data received: %s %ld\n",
		  type, number);

	else
	   printf("P> Binary data receive failed: %s %ld/%ld\n",
		  type, number, ni);

	CFREE(space);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ERROR_HANDLER - do some binary data exchanges */

static void error_handler(int sig)
   {

    SC_block_file(stdin);

    if (sig == SIGALRM)
       {PRINT(stdout, "Binary test timed out\n");
	exit(123);}
    else
       exit(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int argc, char **argv)
   {PROCESS *pp;
    int to;
    char s[MAXLINE+1], **argl, *prog, *msg;

    prog = argv[0];
    to   = 30;
    if (argc < 2)
       {printf("\nRunning TPCC by default\n\n");
        argl = CMAKE_N(char *, 3);
        argl[0] = CSTRSAVE("tpcc");
        argl[1] = CSTRSAVE("-b");
        argl[2] = NULL;}
    else
       argl = argv + 1;

    SC_signal_n(SIGINT, error_handler, NULL, 0);

    SC_setbuf(stdout, NULL);

    pp = PN_open_process(argl, NULL, "wb");
    if (pp == NULL)
       {printf("\nFailed to open: %s\n\n", prog);
        return(1);};

    printf("\nRunning process: %s\n\n", prog);

    SC_unblock_file(stdin);

    SC_unblock(pp);

/* set the alarm */
    SC_timeout(to, error_handler, NULL, 0);

    while (TRUE)
       {while (SC_gets(s, MAXLINE, pp) != NULL)
           {if (strcmp(s, ":B\n") == 0)
               {SC_set_attr(pp, SC_LINE, TRUE);
                process_binary(pp);}
            else if (strcmp(s, ":A\n") == 0)
               SC_set_attr(pp, SC_LINE, FALSE);
            else
               printf("%s", s);};

        if (SC_status(pp) != SC_RUNNING)
           {printf("\nProcess %s terminated (%d %d)\n\n",
                   prog, pp->status, pp->reason);
            break;};

        if (SC_fgets(s, MAXLINE, stdin) != NULL)
           SC_printf(pp, "%s", s);

	msg = SC_error_msg();
        if (msg[0] != '\0')
           {printf("%s\n\n", msg);
            break;};};
        
/* reset the alarm */
    SC_timeout(0, error_handler, NULL, 0);

    SC_block_file(stdin);

    SC_close(pp);

    printf("\nProcess test %s ended\n\n", prog);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
