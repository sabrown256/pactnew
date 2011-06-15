/*
 * SCAPI.C - helpers for language bindings
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h" 

SC_thread_lock
 SC_ptr_lock    = SC_LOCK_INIT_STATE,
 SC_stream_lock = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/

/*                        POINTER MANAGEMENT ROUTINES                       */

/*--------------------------------------------------------------------------*/

/* SC_STASH_POINTER - add a pointer to the managed array and return
 *                  - its index
 */

int64_t SC_stash_pointer(void *p)
   {int64_t i;

    i = -1;

#ifdef FF_INT_SIZE_PTR_DIFFER
    int np;
    void *pl;

    SC_LOCKON(SC_ptr_lock);

    if (_SC.ptr_lst == NULL)
       _SC.ptr_lst = CMAKE_ARRAY(void *, NULL, 3);

    np = SC_array_get_n(_SC.ptr_lst);

    for (; _SC.ip < np; _SC.ip++)
        {pl = *(void **) SC_array_get(_SC.ptr_lst, _SC.ip);
         if (pl == NULL)
            {SC_array_set(_SC.ptr_lst, _SC.ip, &p);
             i = ++_SC.ip;
             break;};}

    if (i == -1)
       {SC_array_push(_SC.ptr_lst, &p);
        i = ++_SC.ip;}

    SC_LOCKOFF(SC_ptr_lock);

#else

    i = _SC_to_number(p);

#endif

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_POINTER - return a pointer from the managed array */

void *SC_get_pointer(int64_t n)
   {void *p;


#ifdef FF_INT_SIZE_PTR_DIFFER
    int np;

    SC_LOCKON(SC_ptr_lock);

    p = NULL;

    np = SC_array_get_n(_SC.ptr_lst);

    if ((1 <= n) && (n <= np))
       {n--;
	p = *(void **) SC_array_get(_SC.ptr_lst, n);};

    SC_LOCKOFF(SC_ptr_lock);

#else

    p = _SC_to_address(n);

#endif

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_POINTER_INDEX - return the index of the stored pointer
 *
 */

int64_t SC_pointer_index(void *p)
   {int64_t i;

#ifdef FF_INT_SIZE_PTR_DIFFER
    int np;
    void **pl;

    SC_LOCKON(SC_ptr_lock);

    np = SC_array_get_n(_SC.ptr_lst);
    for (i = 0; i < np; i++)
        {pl = *(void **) SC_array_get(_SC.ptr_lst, i);
	 if (pl == p)
             break;}

    if (i >= np)
       i = -1;
    else
       i++;

    SC_LOCKOFF(SC_ptr_lock);

#else

    i = _SC_to_number(p);

#endif

    return(i);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DEL_POINTER - remove a pointer from the managed array
 *                - return the pointer so that somebody can free it maybe
 */

void *SC_del_pointer(int n)
   {void *p;
    
#ifdef FF_INT_SIZE_PTR_DIFFER
    int np;

    p = NULL;

    np = SC_array_get_n(_SC.ptr_lst);

    if ((1 <= n) && (n <= np))
       {n--;

	SC_LOCKON(SC_ptr_lock);

	p = *(void **) SC_array_get(_SC.ptr_lst, n);

/* NULL out this slot */
	SC_array_set(_SC.ptr_lst, n, NULL);

/* reset the first avaiable index */
	_SC.ip = min(_SC.ip, n);

	SC_LOCKOFF(SC_ptr_lock);};

#else

    p = _SC_to_address(n);

#endif

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_STASH - free the array of stashed pointers */

int SC_free_stash(void)
   {

    SC_free_array(_SC.ptr_lst, NULL);

    _SC.ptr_lst = NULL;
    _SC.ip      = 0;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

