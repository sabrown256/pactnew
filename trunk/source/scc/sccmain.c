/*
 * SCCMAIN.C - the SCC main
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scc_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_END_SCC - gracefully exit from SCC */

void CC_end_scc(int val)
   {int ev;

    ev = val;

    exit(ev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_INTERRUPT_HANDLER - handle interrupts for SCC */

void CC_interrupt_handler(int sig)
   {

    SC_signal_n(SIGINT, CC_interrupt_handler, NULL);

    exit(1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USAGE - print usage message */

static void usage(void)
   {

    PRINT(stdout, "\n");
    PRINT(stdout, "Usage: scc [-check] [-grm] [-h] [-mem] [-prs] <arg>*\n");
    PRINT(stdout, "\n");
    PRINT(stdout, "Special options:\n");
    PRINT(stdout, "   check - check each declaration only\n");
    PRINT(stdout, "   grm   - show verbose trace of the grammar actions\n");
    PRINT(stdout, "   h     - this help message\n");
    PRINT(stdout, "   mem   - check for memory leaks\n");
    PRINT(stdout, "   prs   - show verbose trace of lexer tokens\n");
    PRINT(stdout, "   <arg> - argument for compiler\n");
    PRINT(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start SCC here */

int main(int c, char **v, char **env)
   {int i, j, l, ret;
    int cs, dbg;
    char msg[MAXLINE];
    char *cmp, **args, **files;

    SC_init("Aborting with error", CC_end_scc,
            TRUE, CC_interrupt_handler, NULL,
            TRUE, NULL, 0);

/* find memory check option early */
    dbg = -1;
    for (i = 1, j = 0, l = 0; i < c; i++)
        {if (strcmp(v[i], "-mem") == 0)
	    {dbg = 2;
	     break;};};

    if (dbg > 0)	
       cs = SC_mem_monitor(-1, dbg, "CC", msg);

    CC_init_c_syntax_mode();

    cmp = USE_COMPILER;
    ret = TRUE;

    args  = CMAKE_N(char *, c+1);
    files = CMAKE_N(char *, c+1);

/* process the command line arguments */
    for (i = 1, j = 0, l = 0; i < c; i++)
        {if (strcmp(v[i], "-grm") == 0)
	    _CC.diagnose_grammar = TRUE;

	 else if (strcmp(v[i], "-prs") == 0)
	    CC_diagnose_parse(TRUE);

	 else if (strcmp(v[i], "-check") == 0)
	    _CC.check_decls = TRUE;

	 else if (strcmp(v[i], "-mem") == 0)
	    dbg = 2;

	 else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'h' :
		      usage();
                      return(1);
		      break;};
	     args[j++] = v[i];}
         else
	    files[l++] = v[i];};

    args[j++]  = NULL;
    files[l++] = NULL;

    ret = TRUE;
    for (i = 0; files[i] != NULL; i++)
        ret &= CC_compile_file(files[i], cmp, args);

    CC_finish();

    CFREE(args);
    CFREE(files);

    if (dbg > 0)	
       cs = SC_mem_monitor(cs, dbg, "CC", msg);

    ret = (ret != TRUE);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
