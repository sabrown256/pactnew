/*
 * SCMEMG.C - guarded primitive memory allocation functions
 *          - memory allocated via mmap and mprotected
 *          - will give SIGSEGV is over/under written
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h"
#include "scope_mem.h"
#include "scope_proc.h"
#include <sys/mman.h>

#define NG  8

#ifndef PROT_NONE
# define PROT_NONE 0
#endif

/* allocated layout:
 *   |  page | double | requested space |
 *       ^        ^          ^
 *    protect  length     returned
 */

#define SC_MEM_G_PARAM(na, no, nb)                                           \
   {long _n, _m;                                                             \
    _m = sizeof(double) - 1;                                                 \
    _n = (nb + _m) & ~_m;                                                    \
    na = _n + sizeof(double) + _SC.page_size;                                \
    if (_SC.mem_guard_high)                                                  \
       {_m = _SC.page_size - 1;                                              \
        na = (na + _m) & ~_m;};                                              \
    no = na - _n;}

SC_thread_lock
 SC_mma_lock = SC_LOCK_INIT_STATE,
 SC_mmf_lock = SC_LOCK_INIT_STATE,
 SC_mmr_lock = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_GUARDED - allocate NB bytes of memory which is guarded
 *                   - at both ends
 */

void *_SC_alloc_guarded(size_t nb)
   {void *pr;

    pr = NULL;

    if (SC_n_threads != -1)
       SC_LOCKON(SC_mma_lock);

#ifdef HAVE_DEV_ZERO 
    {int err;
     size_t na, no;
     char *pc;
     long *pl;
     SC_address ad;
     static int fdz = -1;

     if (nb > 0)
        {if (_SC.page_size < 0)
	    _SC.page_size = getpagesize();

	 if (fdz == -1)
	    {fdz = open("/dev/zero", O_RDWR);
	     if (fdz < 0)
	        {io_printf(stdout,
			   "Cannot initialize Protected Score Memory Manager - exiting\n");
		 exit(SC_EXIT_ERRNO());};};

	 if (fdz >= 0)
	    {SC_MEM_G_PARAM(na, no, nb);

	     ad.diskaddr = 0;
	     ad.memaddr  = mmap(_SC.start, (int) na,
				PROT_READ | PROT_WRITE,
				MAP_PRIVATE,
				fdz, 0);

	     if (ad.diskaddr == -1)
	        pr = NULL;

	     else
	        {pc  = (char *) ad.memaddr;
		 pl  = (long *) (pc + no - sizeof(double));
		 *pl = nb;

		 err = mprotect(pc, _SC.page_size, PROT_NONE);
		 SC_ASSERT(err == 0);

		 _SC.start = pc + na;
		 pr    = pc + no;};};};};
#else

/* NOTE: infinite loop if this allocates memory */
    fputs("Cannot initialize Protected Score Memory Manager - exiting\n",
	  stdout);

    exit(SC_EXIT_INIT);

#endif

    if (SC_n_threads != -1)
       SC_LOCKOFF(SC_mma_lock);

    return(pr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_GUARDED - free memory allocated by _SC_ALLOC_GUARDED */

void _SC_free_guarded(void *p)
   {int err;
    size_t na, nb, no;
    char *pc;
    long *pl;

    if (SC_n_threads != -1)
       SC_LOCKON(SC_mmf_lock);

    if (p != NULL)
       {pc = (char *) p;
	pl = (long *) (pc - sizeof(double));
	nb = *pl;

	SC_MEM_G_PARAM(na, no, nb);

	pc -= no;
	err = mprotect(pc, _SC.page_size, PROT_READ);
	SC_ASSERT(err == 0);

	err = munmap(pc, na);
	SC_ASSERT(err == 0);};

    if (SC_n_threads != -1)
       SC_LOCKOFF(SC_mmf_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_GUARDED - reallocate to NB bytes of memory a pointer P */

void *_SC_realloc_guarded(void *p, size_t nb)
   {int err;
    size_t ns, na, no;
    char *s, *t, *pc;
    long *pl;

    if (SC_n_threads != -1)
       SC_LOCKON(SC_mmr_lock);

    if (p != NULL)
       {pc = (char *) p;
	pl = (long *) (pc - sizeof(double));
	ns = *pl;

	SC_MEM_G_PARAM(na, no, ns);

	s   = pc;
	pc -= no;

	err = mprotect(pc, _SC.page_size, PROT_READ);
	SC_ASSERT(err == 0);};

    t = _SC_alloc_guarded(nb);

    if ((p != NULL) && (t != NULL))
       {ns = min(ns, nb);

	memcpy(t, s, ns);

	_SC_free_guarded(s);};

    if (SC_n_threads != -1)
       SC_LOCKOFF(SC_mmr_lock);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_USE_GUARDED_MEM - setup to use mprotect'd memory
 *
 * #bind SC_use_guarded_mem fortran() python()
 */

void SC_use_guarded_mem(int on)
   {

    if (on)
       {_SC.alloc   = _SC_alloc_guarded;
	_SC.realloc = _SC_realloc_guarded;
	_SC.free    = _SC_free_guarded;}
    else
       {_SC.alloc   = malloc;
	_SC.realloc = realloc;
	_SC.free    = free;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_GUARD_HIGH - guard the upper memory bound if ON == TRUE
 *                   - otherwise guard the lower bound
 */

void SC_mem_guard_high(int on)
   {

     _SC.mem_guard_high = on;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
