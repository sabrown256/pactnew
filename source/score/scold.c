/*
 * SCOLD.C - for backward compatibility
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

void
 *(*SC_make_hook)(long nitems, long bpi, char *name, int na) = SC_alloc_na,
 *(*SC_remake_hook)(void *p, long nitems, long bpi, int na)  = SC_realloc_na;

int
 (*SC_sfree_hook)(void *p) = SC_free;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

