/*
 * SCMEMO.C - override primitive memory allocation functions
 *          - use dynamic linker to access system functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifdef LINUX

#define _GNU_SOURCE

#include "score_int.h"
#include "scope_mem.h"
#include <dlfcn.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_OVER - allocate NB bytes of memory */

void *_SC_alloc_over(size_t nb)
   {void *pr;
    static void *(*system_malloc)(size_t nb) = NULL;

    if (system_malloc == NULL)
       system_malloc = dlsym(RTLD_NEXT, "malloc");

    pr = system_malloc(nb);

    return(pr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_OVER - free memory allocated by _SC_ALLOC_OVER */

void _SC_free_over(void *p)
   {static void (*system_free)(void *p) = NULL;

    if (system_free == NULL)
       system_free = dlsym(RTLD_NEXT, "free");

    system_free(p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_OVER - reallocate to NB bytes of memory a pointer P */

void *_SC_realloc_over(void *p, size_t nb)
   {void *pr;
    static void *(*system_realloc)(void *p, size_t nb) = NULL;

    if (system_realloc == NULL)
       system_realloc = dlsym(RTLD_NEXT, "realloc");

    pr = system_realloc(p, nb);

    return(pr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_USE_OVER_MEM - setup to use override memory functions
 *
 * #bind SC_use_over_mem fortran() python()
 */

void SC_use_over_mem(int on)
   {

    if (on)
       {_SC.alloc   = _SC_alloc_over;
	_SC.realloc = _SC_realloc_over;
	_SC.free    = _SC_free_over;}
    else
       {_SC.alloc   = malloc;
	_SC.realloc = realloc;
	_SC.free    = free;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MALLOC - allocate NB bytes of memory */

void *malloc(size_t nb)
   {void *pr;

    SC_use_over_mem(TRUE);

#if 1
    pr = SC_alloc_n(nb, 1,
		    SC_MEM_ATTR_FUNC, "BARE_MALLOC",
		    0);
#else
    pr = CMAKE_N(char, nb);
#endif

    return(pr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CALLOC - allocate NB bytes of memory */

void *calloc(size_t ni, size_t bpi)
   {void *pr;

    SC_use_over_mem(TRUE);

#if 1
    pr = SC_alloc_n(ni, bpi,
		    SC_MEM_ATTR_FUNC, "BARE_CALLOC",
		    SC_MEM_ATTR_ZERO_SPACE, 2,
		    0);
#else
    size_t nb;
    
    nb = ni*bpi;
    pr = CMAKE_N(char, nb);
    memset(pr, 0, nb);
#endif

    return(pr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE - free memory allocated by malloc */

void free(void *p)
   {

    SC_use_over_mem(TRUE);

    CFREE(p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REALLOC - reallocate to NB bytes of memory a pointer P */

void *realloc(void *p, size_t nb)
   {void *pr;

    SC_use_over_mem(TRUE);

    CREMAKE(p, char, nb);

    return(pr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
