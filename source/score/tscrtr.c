/*
 * TSCRTR.C - test SC_retrace_exe
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

    SC_setup_sig_handlers(sig_handler, NULL, FALSE);

    SC_LOCKON(tlock);

    err = SC_retrace_exe(NULL, -1, 60000);

    SC_ASSERT(err == TRUE);

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

    SC_ASSERT(c != -HUGE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK - serial worker routine */

static void *work(void *a)
   {int tid, nt;
    void *rv;

    tid = SC_current_thread();
    nt  = SC_get_n_thread();

    rv = NULL;
    if ((tid == nt-1) || (nt < 2))
       raise(SIGSEGV);
    else
       busy();

    return(rv);}

/*--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v, char **env)
   {int i, nt, ntmx, rv;

    SC_exe_init_api();

    nt = -1;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("\nUsage: tscrtr [-h] [-t #]\n");
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

    SC_setup_sig_handlers(sig_handler, NULL, TRUE);

    if (SETJMP(cpu) == 0)
       rv = driver(nt);
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
