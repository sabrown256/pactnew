/*
 * SCTHRT.C - shared memory parallel processing test
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

SC_ATTACHED_THREAD(smp_attr);
SC_THREAD_LOCK(smp_lock);

SC_thread_key
 tid_key,
 index_key;

int
 delay,
 mem,
 silent,
 global_count;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TID - return the id of the current thread */

static void tid(int *id)
   {int *tmp;

    tmp = SC_GET_PKEY(int, tid_key);
    if (tmp == NULL)
       *id = 0;

    else
       *id = *tmp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_WRK - worker for parallel test */

void *test_wrk(void *x)
   {int n, m, id, index;
    int l, t;
    int *pi;

    id = SC_current_thread();
    n  = *((int *) x);

/* here we set the key to the thread index */
    SC_SET_KEY(int, index_key, x);

    t = random();
    l = (t >> 1) + (t >> 2) + (t >> 3) + (t >> 5) + (t >> 7);
    m = (delay == 0) ? 0 : 1 + l % delay;
    SC_sleep(250*m);

    if (!silent)
       io_printf(stdout, "(%d|%d delay=%d) ", id+1, n, m);

/* make sure we get the correct key value back */
    pi = SC_GET_PKEY(int, index_key);
    if (pi != NULL)
       {index = *pi;
	if (index != n)
	   if (!silent)
	      io_printf(stdout, "\nKey value error: in = %d, out = %d\n",
			n, index);};

    SC_LOCKON(smp_lock);
    global_count++;
    SC_LOCKOFF(smp_lock);

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_MEM - worker for parallel memory test */

void *test_mem(void *x)
   {long i;
    int n, id;
    double *arr;

    id = SC_current_thread();
    n  = *((int *) x);

    for (i = 1; i <= n; ++i)
        {arr = FMAKE_N(double, 1, "TEST_MEM:arr");
/*         io_printf(stdout, "%d ", SC_arrlen(arr)/sizeof(double)); */
         SFREE(arr);};

/* make sure we get the correct key value back */
    io_printf(stdout, "Thread %d executing mm_test\n", id);

    SC_LOCKON(smp_lock);
    global_count++;
    SC_LOCKOFF(smp_lock);

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST - a multithreading example that tests SMP capabilities in SCORE */

int test(int n, int strategy)
   {int i, ok, bad, arg;
    int *indices;
    SC_thread *threads;
    void *pa[1];
    PFPVoidAPV fn[1];

    global_count = 0;

    if (!silent)
       io_printf(stdout, "\nInitial thread count = %d\n", global_count);

    if (mem > 0)
       {pa[0] = &mem;
	fn[0] = test_mem;}
    else
       {arg   = 2;
	pa[0] = &arg;
	fn[0] = test_wrk;};

    switch (strategy)
       {case 0 :

	     if (!silent)
	        io_printf(stdout, "Strategy: low level\n");

	     i = 0;
	     SC_CREATE_KEY(tid_key, NULL);
	     SC_SET_KEY(int, tid_key, &i);

	     SC_thread_set_tid(tid);

	     SC_n_threads = n + 1;

/* this must appear before any SC_GET_KEYs */
	     indices    = FMAKE_N(int, SC_n_threads, "MAIN:threads");
	     indices[0] = 0;

	     threads = FMAKE_N(SC_thread, SC_n_threads, "MAIN:threads");

/* make this call exactly once for each key */
	     SC_CREATE_KEY(index_key, NULL);
	     SC_SET_KEY(int, index_key, &indices[0]);

	     for (i = 1; i < SC_n_threads; i++)
	         {indices[i] = i;
		  if (mem > 0)
		     pa[0] = &mem;
		  else
		     pa[0] = &indices[i];

		  ok = SC_THREAD_CREATE(threads[i], smp_attr, fn[0], pa[0]);
		  if (ok != 0)
		     io_printf(stdout,
			       "THREAD CREATE FAILED %d (%d) - MAIN\n",
			       ok, i);};

	     for (i = 1; i < SC_n_threads; i++)
	         SC_THREAD_JOIN(threads[i], NULL);

	     SFREE(threads);

/* this must appear after last key reference */
	     SFREE(indices);
	     break;

	case 1 :

	     if (!silent)
	        io_printf(stdout, "Strategy: new threads\n");

	     SC_init_threads(n+1, NULL);

	     SC_do_threads(1, &n, fn, pa, NULL);

	     break;

	case 2 :

	     if (!silent)
	        io_printf(stdout, "Strategy: thread pool\n");

	     SC_init_tpool(n, NULL);

	     SC_do_threads(1, &n, fn, pa, NULL);

	     SC_fin_tpool();

	     break;};

    bad = (global_count != n);

    if (!silent)
       {if (bad)
	   io_printf(stdout, "\nError: Bad thread count = %d\n", global_count);
        else
	   io_printf(stdout, "\nFinal thread count = %d\n", global_count);

	io_printf(stdout, "\n");};

    return(bad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a multithreading example that tests SMP capabilities in SCORE */

int main(argc, argv)
   int argc;
   char **argv;
   {int i, m, n, to, bad;
    int strategy;

    SC_setbuf(STDOUT, NULL);

/* process the command line arguments */
    n        = 4;
    delay    = 7;
    mem      = 0;
    silent   = FALSE;
    strategy = 0;
    to       = 30;
    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-')
           {switch (argv[i][1]) 
               {case 'c' :
		     to = SC_stoi(argv[++i]);
		     break;
                case 'd' :
		     delay = SC_stoi(argv[++i]);
		     break;
                case 'h' :
		     printf("\n");
		     printf("Usage: scthrt [-c #] [-d #] [-h] [-m #] [-n #] [-p] [-s] [-t]\n");
		     printf("\n");
		     printf("     c  - timeout time in seconds\n");
		     printf("     d  - time delay factor\n");
		     printf("     h  - this help message\n");
		     printf("     m  - do memory test with # blocks allocated\n");
		     printf("     n  - number of threads to use\n");
		     printf("     p  - use thread pool instead of new\n");
		     printf("     s  - run silently\n");
		     printf("     t  - use new threads instead of pool\n");
                     return(1);
                case 'm' :
		     mem = SC_stoi(argv[++i]);
		     break;
                case 'n' :
		     n = SC_stoi(argv[++i]);
		     break;
                case 'p' :
		     strategy = 2;
		     break;
                case 's' :
		     silent = TRUE;
		     break;
                case 't' :
		     strategy = 1;
		     break;};};

    SC_timeout(to, NULL);

    m   = 8/n;
    m   = max(m, 1);
    bad = 0;
    for (i = 0; i < m; i++)
        bad |= test(n, strategy);

    SC_timeout(0, NULL);

    return(bad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
