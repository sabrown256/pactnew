/*
 * SHMAIN.C - the function main which when loaded with the SCHEME library
 *          - package gives a working Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"
#include "syntax.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USAGE - print usage message */

static void usage(void)
   {

    PRINT(stdout, "\n");
    PRINT(stdout, "Usage: scheme [-e] [-h] [-l <file>]* [-q] [-r] <command\n");
    PRINT(stdout, "\n");
    PRINT(stdout, "Options:\n");
    PRINT(stdout, "   e - exit on error instead of continuing at next error level\n");
    PRINT(stdout, "   h - this help message\n");
    PRINT(stdout, "   l - load the specified <file> of Scheme code\n");
    PRINT(stdout, "   q - quiet, suppress the banner\n");
    PRINT(stdout, "   r - do not load the standard run time Scheme environment\n");
    PRINT(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a fairly vanilla place to start up a Scheme */

int main(int c, char **v, char **env)
   {int i, lsch, ret;
    int rfl, cfl, qfl, trerr;
    char *cmnd;
    SS_psides *si;

    si = &_SS_si;

    SS_init(si, "Aborting with error", SS_end_scheme,
            TRUE, SS_interrupt_handler,
            TRUE, NULL, 0);

    SS_set_scheme_env(v[0], NULL);

/* initialize the Scheme system */
    si = SS_init_scheme(CODE, VERSION);

    SS_set_prompt("Scheme-> ");
    si->trap_error = FALSE;

    SS_env_vars(si, env, NULL);

/* initialize the available syntax modes */
    DEF_SYNTAX_MODES(si);

    SS_define_argv(si, "scheme", c, v);

    cmnd  = NULL;
    qfl   = FALSE;
    lsch  = 0;
    rfl   = TRUE;
    cfl   = FALSE;
    trerr = TRUE;

#ifdef NO_SHELL
    rfl = FALSE;

/* connect the I/O functions */
    PG_open_console("PACT Scheme", "MONOCHROME", 1, 0.1, 0.1, 0.8, 0.8);

#endif

/* process the command line arguments */
    for (i = 1; i < c; i++)
        if (v[i][0] == '-')
           {switch (v[i][1])
               {case 'e' :
		     trerr = FALSE;
		     break;
                case 'h' :
		     usage();
                     return(1);
		     break;
                case 'l' :
		     lsch = ++i;            /* load Scheme file */
		     break;
                case 'q' :
		     qfl = TRUE;            /* quiet - no banner */
		     break;
                case 'r' :
		     rfl = FALSE;           /* don't load full Scheme */
		     break;};}
        else
           {cfl  = TRUE;
            cmnd = SC_dstrcpy(cmnd, " ");
            for ( ; i < c; i++)
                {cmnd = SC_dstrcat(cmnd, v[i]);
                 cmnd = SC_dstrcat(cmnd, " ");};};

    si->trap_error = trerr;

/* load the full Scheme */
    if (rfl)
       SS_load_scm("runt.scm");

/* read the optionally specified load file */ 
    if (lsch != 0)
       SS_load_scm(v[lsch]);

    si->nsave    = 0;
    si->nrestore = 0;
    si->nsetc    = 0;
    si->ngoc     = 0;

    SC_mem_stats_set(0L, 0L);

    if (cfl)
       {PRINT(STDOUT, "\n\n");
	ret = !SS_run(cmnd);
	PRINT(STDOUT, "\n\n");}

    else
       {if (qfl == FALSE)
	   SC_banner("");
        SS_repl(si);

        ret = 0;};

    CFREE(cmnd);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
