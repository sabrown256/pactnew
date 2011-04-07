/*
 * SCORE_OLD.H - include file for deprecated SCORE functionality
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCORE_OLD

#define PCK_SCORE_OLD

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#undef MAKE
#undef MAKE_N
#undef REMAKE
#undef REMAKE_N
#undef SFREE
#undef SFREE_N

#define MAKE(_t)             CMAKE(_t)
#define MAKE_N(_t, n)        CMAKE_N(_t, n)
#define REMAKE(p, _t)        CREMAKE(p, _t, 1)
#define REMAKE_N(p, _t, n)   CREMAKE(p, _t, n)
#define SFREE(x)             CFREE(x)
#define SFREE_N(x, n)        CFREE(x)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FMAKE - memory allocation and bookkeeping macro */

#define FMAKE(_t, name)                                                      \
    ((_t *) (*SC_gs.mm.alloc)(1L, (long) sizeof(_t), name, FALSE, -1))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FMAKE_N - allocate a block of type _t and return a pointer to it */

#define FMAKE_N(_t, n, name)                                                 \
    ((_t *) (*SC_gs.mm.alloc)((long) n, (long) sizeof(_t), name, FALSE, -1))

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* former SCMEMC.C declarations */

extern int
 SC_zero_space(int flag),
 SC_zero_on_alloc(void);


/* former SCMEMDA.C declarations */

extern int
 SC_free(void *p);

extern void
 *SC_alloc(long nitems, long bpi, char *name),
 *SC_alloc_na(long nitems, long bpi, char *name, int na),
 *SC_realloc(void *p, long nitems, long bpi),
 *SC_realloc_na(void *p, long nitems, long bpi, int na);


#ifdef __cplusplus
}
#endif

#endif

