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

#undef MAKE
#undef MAKE_N
#undef REMAKE
#undef REMAKE_N
#undef SFREE
#undef SFREE_N

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/* MAKE - memory allocation and bookkeeping macro */

#define MAKE(_t)                                                             \
    ((_t *) (*SC_gs.mm.alloc)(1L, (long) sizeof(_t), NULL, FALSE))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_N - allocate a block of type _t and return a pointer to it */

#define MAKE_N(_t, n)                                                        \
    ((_t *) (*SC_gs.mm.alloc)((long) n, (long) sizeof(_t), NULL, FALSE))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FMAKE - memory allocation and bookkeeping macro */

#define FMAKE(_t, name)                                                      \
    ((_t *) (*SC_gs.mm.alloc)(1L, (long) sizeof(_t), name, FALSE))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FMAKE_N - allocate a block of type _t and return a pointer to it */

#define FMAKE_N(_t, n, name)                                                 \
    ((_t *) (*SC_gs.mm.alloc)((long) n, (long) sizeof(_t), name, FALSE))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REMAKE - memory reallocation and bookkeeping macro */

#define REMAKE(p, _t)                                                        \
   (p = (_t *) (*SC_gs.mm.realloc)((void *) p, 1L,                           \
				   (long) sizeof(_t), FALSE))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REMAKE_N - reallocate a block of type _t and return a pointer to it */

#define REMAKE_N(p, _t, n)                                                   \
   (p = (_t *) (*SC_gs.mm.realloc)((void *) p, (long) (n),                   \
				   (long) sizeof(_t), FALSE))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SFREE - release memory and do bookkeeping */

#define SFREE(x)                                                             \
   {(*SC_gs.mm.free)(x);                                                     \
    x = NULL;}

#define SFREE_N(x, n)  SFREE(x)

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


/* SCMEMC.C declarations */

extern int
 SC_zero_space(int flag),
 SC_zero_on_alloc(void);


#ifdef __cplusplus
}
#endif

#endif

