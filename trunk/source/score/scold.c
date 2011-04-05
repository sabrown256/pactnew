/*
 * SCOLD.C - for backward compatibility
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"

void
 *(*SC_make_hook)(long nitems, long bpi, char *name, int na) = SC_alloc_na,
 *(*SC_remake_hook)(void *p, long nitems, long bpi, int na)  = SC_realloc_na;

int
 (*SC_sfree_hook)(void *p) = SC_free;

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

