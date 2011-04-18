/*
 * SCRTRT.C - test SC_retrace_exe
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

static JMP_BUF
 cpu;

SC_THREAD_LOCK(tlock);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIG_HANDLER - handle signals gracefully */

static void sig_handler(int sig)
   {int err;

    SC_setup_sig_handlers(sig_handler, FALSE);

    SC_LOCKON(tlock);

    err = SC_retrace_exe(NULL, -1, 60000);

    LONGJMP(cpu, TRUE);

    SC_LOCKOFF(tlock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BUSY - do something to keep busy */

static void busy(void)
   {int i;
    double c;

    for (i = 0; i < 10000000; i++)
        c = cos((double) (i % 12));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK - serial worker routine */

static void *work(void *a)
   {int tid, nt;
    char *s;
    void *rv;

    tid = SC_current_thread();
    nt  = SC_get_n_thread();

    rv = NULL;
    s  = NULL;
    if ((tid == nt-1) || (nt < 2))
       raise(SIGSEGV);
/*       *s = '\0'; */
    else
       busy();

    return(rv);}

/*--------------------------------------------------------------------------*/

#if 1

/*--------------------------------------------------------------------------*/

/* DRIVER - test driver with NT threads
 *        - strategy: one entry per thread
 */

static int driver(int nt)
   {int i, rv, nto;
    int *npt;
    PFPVoidAPV *fnc;

    nto = SC_init_threads(nt, NULL);

    fnc = CMAKE_N(PFPVoidAPV, nto);
    npt = CMAKE_N(int, nto);

    for (i = 0; i < nto; i++)
        {fnc[i] = work;
         npt[i] = 1;};

    SC_do_threads(nto, npt, fnc, NULL, NULL);

    rv = 1;

    CFREE(fnc);
    CFREE(npt);

    return(rv);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* DRIVER - test driver with NT threads
 *        - strategy: single entry handed to multiple threads
 */

static int driver(int nt)
   {int rv, nto;
    int *npt;
    PFPVoidAPV *fnc;

    nto = SC_init_threads(nt, NULL);

    fnc = CMAKE_N(PFPVoidAPV, 1);
    npt = CMAKE_N(int, 1);

    fnc[0] = work;
    npt[0] = nto;

    SC_do_threads(1, npt, fnc, NULL, NULL);

    rv = 1;

    CFREE(fnc);
    CFREE(npt);

    return(rv);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v, char **env)
   {int i, nt, ntmx, rv;

    nt = -1;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("\nUsage: scrtrt [-h] [-t #]\n");
	     printf("    h  this help message\n");
	     printf("    t  number of threads to use\n");
	     printf("\n");
	     exit(1);}

	 else if (strcmp(v[i], "-t") == 0)
	    nt = SC_stoi(v[++i]);};

/*    ntmx = SC_get_ncpu(); */
    ntmx = 16;
    nt   = min(nt, ntmx);
    nt   = max(nt, 1);

    SC_setup_sig_handlers(sig_handler, TRUE);

    if (SETJMP(cpu) == 0)
       rv = driver(nt);
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
