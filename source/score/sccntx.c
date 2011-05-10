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

#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_CONTEXT_TABLE - setup and return the context table */

static hasharr *_SC_init_context_table(void)
   {

    if (_SC.context_table == NULL)
       _SC.context_table = SC_make_hasharr(HSZSMINT, NODOC, SC_HA_ADDR_KEY, 3);

    return(_SC.context_table);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REGISTER_CONTEXT - register context A with function F
 *                     - when F is called it will lookup A
 *                     - by hashing on its own address
 */

int SC_register_context(void *f, void *a)
   {int rv;
    hasharr *ha;
    haelem *hp;

    rv = FALSE;
    ha = _SC_init_context_table();

    hp = SC_hasharr_install(ha, f, a, SC_POINTER_S, FALSE, TRUE);
    if (hp != NULL)
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_CONTEXT - return the context of F */

void *SC_get_context(void *f)
   {void *rv;
    hasharr *ha;

    ha = _SC_init_context_table();

    rv = SC_hasharr_def_lookup(ha, f);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
