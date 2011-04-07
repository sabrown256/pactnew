/*
 * SCOLD.C - for backward compatibility
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_mem.h"

void
 *(*SC_make_hook)(long nitems, long bpi, char *name, int na) = SC_alloc_na,
 *(*SC_remake_hook)(void *p, long nitems, long bpi, int na)  = SC_realloc_na;

int
 (*SC_sfree_hook)(void *p) = SC_free;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ALLOC - add a layer of control over the C level memory management
 *          - system to store extra information about the allocated spaces
 */

void *SC_alloc(long nitems, long bpi, char *name)
   {void *rv;
    SC_mem_opt opt;

    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.fnc  = name;
    opt.file = NULL;
    opt.line = -1;

    rv = SC_alloc_nzt(nitems, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ALLOC_NA - add a layer of control over the C level memory management
 *             - system to store extra information about the allocated spaces
 *             - iff NA TRUE fudge the accounting so that this block
 *             - will not show up in the bytes allocated count
 */

void *SC_alloc_na(long nitems, long bpi, char *name, int na)
   {void *p;
    SC_mem_opt opt;

    opt.na  = na;
    opt.zsp = -1;
    opt.typ = -1;
    opt.fnc  = name;
    opt.file = NULL;
    opt.line = -1;

    p = SC_alloc_nzt(nitems, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REALLOC - add a layer of control over the C level memory management
 *            - system to store extra information about the allocated spaces
 */

void *SC_realloc(void *p, long nitems, long bpi)
   {void *rv;
    SC_mem_opt opt;

    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.fnc  = NULL;
    opt.file = NULL;
    opt.line = -1;

    rv = SC_realloc_nzt(p, nitems, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REALLOC_NA - add a layer of control over the C level memory management
 *               - system to store extra information about the allocated spaces
 *               - iff NA TRUE fudge the accounting so that this block
 *               - will not show up in the bytes allocated count
 */

void *SC_realloc_na(void *p, long nitems, long bpi, int na)
   {void *rv;
    SC_mem_opt opt;

    opt.na   = na;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.fnc  = NULL;
    opt.file = NULL;
    opt.line = -1;

    rv = SC_realloc_nzt(p, nitems, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE - the complementary routine for SC_alloc_nzt
 *         - free all the space including the counter
 *         - return TRUE if successful and FALSE otherwise
 */

int SC_free(void *p)
   {int rv;
    SC_mem_opt opt;

    opt.na   = -1;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.fnc  = NULL;
    opt.file = NULL;
    opt.line = -1;

    rv = SC_free_nzt(p, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ZERO_SPACE - set zero space flag globally
 *               - return the original global value
 */

int SC_zero_space(int flag)
   {int rv;

    rv = _SC.zero_space;
    if ((0 <= flag) && (flag <= 5))
       {SC_zero_space_n(flag, -2);
	_SC.zero_space = flag;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ZERO_ON_ALLOC - return TRUE iff zero_space is 1 or 2 */

int SC_zero_on_alloc(void)
   {int rv;

    rv = SC_zero_on_alloc_n(-2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

