/*
 * SCMEME.C - memory management cleanup functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h"
#include "scope_mem.h"
#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_END_SESSION - free all known permanent memory blocks
 *                - principally for codes wanting to check for leaks
 */

void SC_end_session(void)
   {

/* free hash tables */
    SC_free_hasharr(_SC.types.typ, NULL, NULL);
    SC_free_hasharr(_SC.mem_table, NULL, NULL);

/* free arrays */
    SC_free_array(_SC.ptr_lst, NULL);
    SC_free_array(_SC.eth_locks, NULL);
    SC_free_array(_SC.eth_keys, NULL);
    SC_free_array(_SC.eth_conds, NULL);

    SC_free_array(_SC_ps.wait_list, SC_array_free_n);
    SC_free_array(_SC_ps.process_list, NULL);

/* free strings */
    CFREE(_SC.ta);
    CFREE(_SC.barrier);
    CFREE(_SC.signame);
    CFREE(_SC.sqbf);
    CFREE(_SC.ecbf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
