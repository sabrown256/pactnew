/*
 * SCTLTS.C - thread lock test
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#define memzsp 0

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

double *get_temp(int ni, char *name)
   {double *d;

    d = CMAKE_N(double, ni);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

double *rel_temp(double *d)
   {

    CFREE(d);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TIME_THREAD_LOCK - measure time to lock and unlock */

static int time_thread_lock(int nt, int nd, int ni)
   {int it, rv;
    double al, au, tl, tu, ta, tb, tc;
    double tt;
    SC_thread_lock lck = SC_LOCK_INIT_STATE;

    io_printf(stdout, "\t\t\ttiming ............ ");

    rv = TRUE;

    tl = 0.0;
    tu = 0.0;
    tt = SC_wall_clock_time();

    ni *= 200;
    for (it = 0; it < ni; it++)
        {ta = SC_wall_clock_time();
	 SC_LOCKON(lck);

	 tb = SC_wall_clock_time();

	 SC_LOCKOFF(lck);

	 tc = SC_wall_clock_time();

	 tl += (tb - ta);
	 tu += (tc - tb);};

    tt = SC_wall_clock_time() - tt;

    al = tl/((double) ni);
    au = tu/((double) ni);

    io_printf(stdout, "ok\n\n");
    io_printf(stdout, "\t\t    Iter      Ttot       Tlck      Tulck\n");
    io_printf(stdout, "\t\t%8d  %10.2e %10.2e %10.2e\n\n", it, tt, al, au);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_OMP - test omp thread locking */

static int test_omp(int nt, int nd, int ni)
   {int rv;

#ifdef PTHREAD_OMP
    int i, tid;
    char *mem;
    double **dptr;

    io_printf(stdout, "\t\t\tomp ............... ");

    tid = -1;
    SC_ASSERT(tid == -1);

/* allocate something trivial to initialize the heap for this thread */
    mem = CMAKE_N(char, 1);
    CFREE(mem);

    dptr = CMAKE_N(double *, 11);

    for (i = 0; i < ni; i++)
        {i++;

#pragma omp parallel sections private(tid) 
	 {
#pragma omp section
	  dptr[0] = get_temp(nd, "one");
#pragma omp section
	  dptr[1] = get_temp(nd, "two");
#pragma omp section
	  dptr[2] = get_temp(nd, "three");
#pragma omp section
	  dptr[3] = get_temp(nd, "four");
#pragma omp section
	  dptr[4] = get_temp(nd, "five");
#pragma omp section
	  dptr[5] = get_temp(nd, "six");
#pragma omp section
	  dptr[6] = get_temp(nd, "seven");
#pragma omp section
	  dptr[7] = get_temp(nd, "eight");
#pragma omp section
	  dptr[8] = get_temp(nd, "nine");
#pragma omp section
	  dptr[9] = get_temp(nd, "ten");
#pragma omp section
	  dptr[10] = get_temp(nd, "eleven");
	 };

	 {
#pragma omp barrier
	 };

#pragma omp parallel sections
	 {
#pragma omp section
	  dptr[10] = rel_temp(dptr[10]);
#pragma omp section
	  dptr[9] = rel_temp(dptr[9]);
#pragma omp section
	  dptr[8] = rel_temp(dptr[8]);
#pragma omp section
	  dptr[7] = rel_temp(dptr[7]);
#pragma omp section
	  dptr[6] = rel_temp(dptr[6]);
#pragma omp section
	  dptr[5] = rel_temp(dptr[5]);
#pragma omp section
	  dptr[4] = rel_temp(dptr[4]);
#pragma omp section
	  dptr[3] = rel_temp(dptr[3]);
#pragma omp section
	  dptr[2] = rel_temp(dptr[2]);
#pragma omp section
	  dptr[1] = rel_temp(dptr[1]);
#pragma omp section
	  dptr[0] = rel_temp(dptr[0]);
	 };

	 {
#pragma omp barrier
	 };
       };

    io_printf(stdout, "ok\n\n");

    CFREE(dptr);

#endif

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - show the usage and options */

static void help(void)
   {

    io_printf(stdout, "Usage: sctlts [-h] [-nd #] [-ni #] [-nt #]\n\n");

    io_printf(stdout, "   h   this help message\n");
    io_printf(stdout, "   nd  # of doubles to allocate\n");
    io_printf(stdout, "   ni  # of iterations to run\n");
    io_printf(stdout, "   nt  # of threads to use\n");
    io_printf(stdout, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, nd, ni, nt, rv;

    rv = TRUE;
    nd = 100;
    ni = 1000;
    nt = 4;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-nd") == 0)
	    nd = SC_stoi(v[++i]);
         else if (strcmp(v[i], "-ni") == 0)
	    ni = SC_stoi(v[++i]);
         else if (strcmp(v[i], "-nt") == 0)
	    nt = SC_stoi(v[++i]);
         else if (strcmp(v[i], "-h") == 0)
	    {help();
	     return(1);};};

/* NOTE: PGI OpenMP is bad overscheduling threads */
#ifdef COMPILER_PGI
    {int ncpu;

     ncpu = SC_get_ncpu();
     nt   = min(nt, ncpu);};
#endif

    SC_zero_space_n(1, -2);

    SC_init_threads(nt, NULL);

/*    SC_gs.mm_debug = 1; */
/*    SC_use_c_mm(); */
/*    SC_configure_mm(0, 0, 0, 1.5); */
/*    SC_configure_mm(128, 1048576, 1048576, 1.5); */

    rv &= test_omp(nt, nd, ni);
    rv &= time_thread_lock(nt, nd, ni);

    rv = (rv == FALSE);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

