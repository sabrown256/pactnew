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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_END_SESSION - free all known permanent memory blocks
 *                - principally for codes wanting to check for leaks
 */

void SC_end_session(void)
   {

/* free hash tables */
    SC_free_hasharr(_SC.typ, NULL, NULL);
    SC_free_hasharr(_SC.mem_table, NULL, NULL);

/* free arrays */
    SC_free_array(_SC.ptr_lst, NULL);
    SC_free_array(_SC.wait_list, SC_array_free_n);
    SC_free_array(_SC.process_list, NULL);
    SC_free_array(_SC.eth_locks, NULL);
    SC_free_array(_SC.eth_keys, NULL);
    SC_free_array(_SC.eth_conds, NULL);

/* free strings */
    SFREE(_SC.ta);
    SFREE(BARRIER);
    SFREE(_SC.signame);
    SFREE(_SC.sqbf);
    SFREE(_SC.ecbf);

#if 0
/* environment variables - do not know how */
    t   = SC_strsavef(s, "PERM|char*:SC_PUTENV:s");

/* per thread data - do not know how */
    *pa = FMAKE_N(char, 4096, "PERM|char*:SC_VDSNPRINTF:pa");

/* what loop is this? */
    pe = FMAKE(SC_evlpdes, "PERM|SC_MAKE_EVENT_LOOP:pe");
    pe->fd     = SC_MAKE_ARRAY("PERM|SC_MAKE_EVENT_LOOP", SC_poll_desc, NULL);
    pe->faccpt = SC_MAKE_ARRAY("PERM|SC_MAKE_EVENT_LOOP", PFFileCallback, NULL);
    pe->frejct = SC_MAKE_ARRAY("PERM|SC_MAKE_EVENT_LOOP", PFFileCallback, NULL);
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
