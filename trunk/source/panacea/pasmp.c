/*
 * PASMP.C - SMP parallel routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "panacea_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CURRENT_THREAD - return the id of the current thread */

int PA_current_thread(void)
   {int tid;

    tid = SC_current_thread();
    tid = max(tid, 0);
    tid = (tid >= SC_n_threads) ? 0 : tid;

    return(tid);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INIT_THREADS - execute the given function FNC on NT threads
 *                 - serial execution follows from NT equal to 1
 */

void PA_init_threads(int nt, PFTid tid)
   {

    if (SC_n_threads == -1)
       SC_init_tpool(nt, tid);

    PA_set_n_threads(max(SC_n_threads, 1));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_N_THREADS - get the number of threads */

int PA_get_n_threads(void)
   {int n;

    n = _PA.nthreads;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SET_N_THREADS - set the number of threads and get the space for them */

void PA_set_n_threads(int n)
   {

    _PA.nthreads = n;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INIT_STATE - initialize the thread state PS */

void _PA_init_state(PA_thread_state *ps, int id)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_THREAD - return the thread state for the current thread */

PA_thread_state *PA_get_thread(int id)
   {PA_thread_state *ps;

    if (_PA.tsid < 0)
       _PA.tsid = SC_register_thread_data("panacea-state",
					     "PA_thread_state",
					     1, sizeof(PA_thread_state),
					     (PFTinit) _PA_init_state);

    if (id < 0)
       id  = PA_current_thread();

    ps = (PA_thread_state *) SC_get_thread_element(id, _PA.tsid);

    return(ps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DO_THREADS - execute the given function FNC on NT threads
 *               - serial execution follows from NT equal to 1
 */

void PA_do_threads(PFPVoid fnc, void **ret)
   {int nt;
    PFPVoidAPV lfnc;

    lfnc = (PFPVoidAPV) fnc;

    nt = PA_get_n_threads();

    SC_do_threads(1, &nt, &lfnc, NULL, ret);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
