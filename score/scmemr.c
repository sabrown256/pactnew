/*
 * SCMEMR.C - replace malloc et. al. for good debugging
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "scope_mem.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MALLOC - allocate NB bytes of memory which is guarded */

void *malloc(size_t nb)
   {void *p;

    p = _SC_alloc_guarded(nb);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CALLOC - allocate NB bytes of memory which is guarded */

void *calloc(size_t ni, size_t bpi)
   {size_t nb;
    void *p;

    nb = ni*bpi;
    p  = _SC_alloc_guarded(nb);

    _SC_mem_exhausted(p == NULL);

    if (p != NULL)
       memset(p, 0, nb);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FREE - free memory allocated by malloc, calloc */

void free(void *p)
   {

    _SC_free_guarded(p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REALLOC - reallocate to NB bytes of memory a pointer P */

void *realloc(void *p, size_t nb)
   {char *t;

    t = _SC_realloc_guarded(p, nb);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
