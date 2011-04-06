/*
 * SCSMPB.C - shared memory parallel processing example and test
 *          - slightly higher level approach
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

typedef struct s_state state;

struct s_state
   {long niter;};

static state
  parallel = { 100000 };

SC_THREAD_LOCK(smp_lock);

int
 global_count;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void *mm_test(arg)
   void *arg;
   {long i;
    int n, index, count;
    double *arr;

    n = parallel.niter;

    for (i = 1; i <= n; ++i)
      {arr = CMAKE_N(double, 1);
       CFREE(arr);};

/* Make sure we get the correct key value back */
    index = 1;
    printf("Thread %d executing mm_test\n", index);

/* With sleep delay and without lock, the final global count will be wrong */
    SC_LOCKON(smp_lock);
    count = global_count;
    global_count = ++count;
    SC_LOCKOFF(smp_lock);

    return NULL;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a multithreading example that tests the SMP safety of the SCORE MM */

/* Note that the code below reduces to a single-threaded version if PACT was
 * not configured thread-safe (i.e. SC_THREAD_SAFE is not defined). In either
 * case, mm_test will be executed n times.
 */

int main(argc, argv)
   int argc;
   char **argv;
   {int n = 1, np = 4, ntotal;
    long i;
    PFPVoidAPV fnc;

    SC_zero_space_n(0, -2);
    SC_configure_mm(128L, 2000000L, 65536L, 1.2);
    SC_setbuf(STDOUT, NULL);

/* process the command line arguments */
    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-')
           {switch (argv[i][1])
               {case 'i' : parallel.niter = SC_stol(argv[++i]);
                           break;
                case 'n' : np = SC_stoi(argv[++i]);
                           break;};};

    SC_init_threads(np, NULL);

    fnc = mm_test;

    np = 6;
    SC_do_threads(n, &np, &fnc, NULL, NULL);

    np = 20;
    SC_do_threads(n, &np, &fnc, NULL, NULL);

    np = 3;
    SC_do_threads(n, &np, &fnc, NULL, NULL);

    ntotal = 29;

    if (global_count != ntotal)
       printf("\nError: Bad thread count = %d\n", global_count);

    printf("\n");

    return 0;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
