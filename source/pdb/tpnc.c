/*
 * TPCC.C - test connections child side
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

/* SEND_BINARY - do some binary data exchanges */

static void send_binary(long ni, char *type)
   {int did;
    long i;
    PDBfile *file;
    void *space;
    double *src;
    PROCESS *pp, *tty;

    PRINT(stdout, ":B\n");

    PRINT(stdout, "%s %ld\n", type, ni);

/* this is a testing mode check */
    tty = SC_get_terminal_process();
    if (tty == NULL)
       {char **argv;

        argv    = CMAKE_N(char *, 2);
        argv[0] = CSTRSAVE("VIF");

        tty = PN_mk_process(argv, "ab", SC_CHILD);
	for (i = 0; i < SC_N_IO_CH; i++)
	    tty->io[i] = i;};

    pp = tty;
    if (pp != NULL)
       {file = pp->vif;
	did  = SC_type_id(type, FALSE);

	src = (double *) _PD_alloc_entry(file, SC_DOUBLE_S, ni);
	for (i = 0L; i < ni; i++)
	    src[i] = (double) i;

	space = SC_convert_id(did, NULL, 0, 1,
			      SC_DOUBLE_I, src, 0, 1, ni, TRUE);

	if (SC_write(space, type, ni, pp))
	   printf("Binary data sent: %s %ld\n",
		  type, ni);

	else
	   printf("Binary data send failed: %s %ld\n",
		  type, ni);

	PRINT(stdout, ":A\n");

	CFREE(space);};

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
		      printf("\nUsage: tpcc [-b]\n");
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
