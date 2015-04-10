/*
 * MLSMP.C - SMP parallel support routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_INIT_THREAD - init state MA for thread ID */

static void _PM_init_thread(PM_smp_state *ma, int id)
   {double *pd, *pm;
    unsigned short *pl;

/* search state */
    ma->last = 1L;

/* random number state */
    pd = ma->dseed;
    pm = ma->dmult;
    pl = ma->multplr;

    pd[0] = 16555217.0;
    pd[1] = 9732691.0;

    pm[0] = 15184245.0;
    pm[1] = 2651554.0;

    pl[0] = 0xb175;
    pl[1] = 0xa2e7;
    pl[2] = 0x2875;

    ma->dadder = 0.0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_GET_STATE - return the PM state for thread ID
 *               - use current thread if ID == -1
 */

PM_smp_state *_PM_get_state(int id)
   {size_t bpi;
    PM_smp_state *ma;

    if (_PM.ita < 0)
       {bpi     = sizeof(PM_smp_state);
	_PM.ita = SC_register_thread_data("pml-state", "PM_smp_state",
				      1, bpi, (PFTinit) _PM_init_thread);};

    if (id < 0)
       id = SC_current_thread();

    ma = (PM_smp_state *) SC_get_thread_element(id, _PM.ita);

    return(ma);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

