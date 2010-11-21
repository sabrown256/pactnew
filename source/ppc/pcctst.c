/*
 * PCCTST.C - test connections child side
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "ppc.h"
#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEND_BINARY - do some binary data exchanges */

static void send_binary(long nitems, char *type)
   {int did;
    long i;
    PDBfile *file;
    void *space;
    double *src;
    PROCESS *pp, *tty;

    PRINT(stdout, ":B\n");

    PRINT(stdout, "%s %ld\n", type, nitems);

/* this is a testing mode check */
    tty = SC_get_terminal_process();
    if (tty == NULL)
       {char **argv;

        argv    = FMAKE_N(char *, 2, "SEND_BINARY:argv");
        argv[0] = SC_strsavef("VIF", "char*:SEND_BINARY:argv0");

        tty      = PC_mk_process(argv, "ab", SC_CHILD);
        tty->in  = 0;
        tty->out = 1;};

    pp = tty;
    if (pp != NULL)
       {file = pp->vif;
	did  = SC_type_id(type, FALSE);

	src = (double *) _PD_alloc_entry(file, SC_DOUBLE_S, nitems);
	for (i = 0L; i < nitems; i++)
	    src[i] = (double) i;

	space = SC_convert_id(did, NULL, 0, SC_DOUBLE_I, src, 0, nitems, TRUE);

	if (PC_write(space, type, nitems, pp))
	   printf("Binary data sent: %s %ld\n",
		  type, nitems);

	else
	   printf("Binary data send failed: %s %ld\n",
		  type, nitems);

	PRINT(stdout, ":A\n");

	SFREE(space);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int argc, char **argv)
   {int i, binary;
    char s[MAXLINE+2];

    SC_setbuf(stdout, NULL);

    binary = FALSE;
    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'b' :
                      binary = TRUE;
		      break;
		 case 'h' :
		      printf("\nUsage: pcctst [-b]\n");
                      printf("   Simple program which prints prompts and echos input\n");
                      printf("   In binary mode sends 100 longs and 10 doubles first\n");
		      printf("   Options:\n");
		      printf("      b - run in binary mode\n");
		      break;};};};

    if (binary)
       {send_binary(100, SC_LONG_S);
        send_binary(10,  SC_DOUBLE_S);};

    while (TRUE)
       {PRINT(stdout, "-> ");
        if (SC_fgets(s, MAXLINE, stdin) != NULL)
           PRINT(stdout, "%s", s);

        if (strcmp(s, "end\n") == 0)
           break;};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
