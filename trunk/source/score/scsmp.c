/*
 * SCSMP.C - shared memory parallel processing example and test
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

typedef struct s_state state;

struct s_state
   {int niter;};

static state
  parallel = { 100 };

SC_ATTACHED_THREAD(smp_attr);
SC_THREAD_LOCK(smp_lock);

SC_thread_key
 index_key;

int
 m = FALSE,
 s = TRUE,
 p = FALSE,
 global_count;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MM_TEST - worker for parallel MM test */

void mm_test(void *x)
   {int i, n, id, index;
    double *arr;

    n = parallel.niter;

/* here we set the key to the thread index */
    SC_SET_KEY(int, index_key, x);

    id = SC_current_thread();

    if (s)
       for (i = 1; i <= n; ++i)
           {arr = FMAKE_N(double, i, "MM_TEST:arr");  
            if (p)
               printf("(%d,%d) ",
		      (int) SC_MEM_GET_N(double, arr),
		      id);
            SFREE(arr);};

    if (m)
       for (i = 1; i <= n; ++i)
           {arr = malloc(sizeof(double)*i);
            if (p)
               printf("(%d,%d) ", i, id);
            free(arr);};

/* make sure we get the correct key value back */
    index = SC_GET_KEY(int, index_key);
    if (index != *((int *) x))
       printf("\nKey value error: in = %d, out = %d\n", *((int *) x), index);

    SC_LOCKON(smp_lock);
    global_count++;
    SC_LOCKOFF(smp_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a multithreading example that tests the SMP safety of the SCORE MM
 * Note that the code below reduces to a single-threaded version if PACT was
 * not thread configured (i.e. HAVE_THREADS is not defined). In either case,
 * mm_test will be executed n times. If PACT was thread configured the main
 * thread does not call mm_test (it doesn't do any work). This gives better
 * performance on some systems.
 */

int main(int argc, char **argv)
   {int i, n;
    int *indexes;
    SC_thread *threads;

    n = 4;

    SC_setbuf(STDOUT, NULL);

/* process the command line arguments */
    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-')
           {switch (argv[i][1]) 
               {case 'i' : parallel.niter = SC_stoi(argv[++i]);
		           break;
                case 'm' : m = TRUE;
                           s = FALSE;
                           break;
                case 'n' : n = SC_stoi(argv[++i]);
                           break;
                case 'p' : p = TRUE;
                           break;
                case 's' : s = TRUE;
                           m = FALSE;
                           break;};};

/* make this call exactly once for each key */
    SC_CREATE_KEY(index_key, NULL);

    i = 0;
    SC_SET_KEY(int, index_key, &i);

/* this must appear before any SC_GET_KEYs */
    indexes    = FMAKE_N(int, n + 1, "MAIN:threads");
    indexes[0] = 0;
    SC_SET_KEY(int, index_key, &indexes[0]);

    threads  = FMAKE_N(SC_thread, n + 1, "MAIN:threads");

    for (i = 1; i <= n; i++)
        {indexes[i] = i;
	 SC_THREAD_CREATE(threads[i], smp_attr, mm_test, &indexes[i]);};

    for (i = 1; i <= n; i++)
        SC_THREAD_JOIN(threads[i], NULL);

    if (global_count != n)
       printf("\nError: Bad thread count = %d\n", global_count);

    printf("\n");

    SFREE(threads);

/* this must appear after last key reference */
    SFREE(indexes);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
