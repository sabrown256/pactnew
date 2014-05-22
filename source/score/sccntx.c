/*
 * SCCNTX.C - state context routines
 *          - provide a mechanism to associate state with
 *          - things like signal handlers or event handlers
 *          - which are not passed
 *          - the state that they need to operate
 *          - the work around is to use global variables which
 *          - can create thread hazards
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_proc.h"
#include "scope_mem.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_CONTEXT_TABLE - setup and return the context table */

static hasharr *_SC_init_context_table(void)
   {hasharr *tb;
    SC_smp_state *pa;

    pa = _SC_get_state(-1);

    if (pa->context_table == NULL)
       pa->context_table = SC_make_hasharr(HSZSMINT, NODOC, SC_HA_ADDR_KEY, 3);

    tb = pa->context_table;

    return(tb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_CONTEXT_TABLE - release the context table */

void SC_free_context_table(void)
   {SC_smp_state *pa;

    pa = _SC_get_state(-1);

    if (pa->context_table != NULL)
       {SC_free_hasharr(pa->context_table, NULL, NULL);
	pa->context_table = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REGISTER_CONTEXT - register context A with function F
 *                     - when F is called it will lookup A
 *                     - by hashing on its own address
 */

int SC_register_context(void *f, void *a, int nb)
   {int rv;
    hasharr *ha;
    haelem *hp;
    SC_contextdes *cd;

    rv = FALSE;
    ha = _SC_init_context_table();

    if (f != NULL)
       {hp = SC_hasharr_lookup(ha, f);
	if (hp != NULL)
	   cd = hp->def;
	else
	   {cd = CPMAKE(SC_contextdes, 3);
	    if (cd != NULL)
	       {cd->f = (PFSignal_handler) f;
		cd->a = a;
	    cd->nb = nb;};};

	hp = SC_hasharr_install(ha, f, cd, SC_POINTER_S, 2, -1);
	if (hp != NULL)
	   rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LOOKUP_CONTEXT - return the context of F */

SC_contextdes SC_lookup_context(void *f)
   {SC_contextdes rv, *prv;
    hasharr *ha;

    ha = _SC_init_context_table();

    prv = (SC_contextdes *) SC_hasharr_def_lookup(ha, f);
    if (prv != NULL)
       rv = *prv;
    else
       memset(&rv, 0, sizeof(SC_contextdes));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_CONTEXT - return the context of F */

void *SC_get_context(void *f)
   {void *rv;
    SC_contextdes cd;

    cd = SC_lookup_context(f);
    rv = cd.a;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
