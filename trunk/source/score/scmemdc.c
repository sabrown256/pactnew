/*
 * SCMEMDC.C - memory mappers
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

/* _SC_MEM_MAP_ACT - map F over all active memory blocks
 *
 * #bind SC_mem_map_act fortran() scheme(memory-trace) python()
 */

static void _SC_mem_map_act(SC_heap_des *ph, PFMemMap f, void *a)
   {int ok;
    long i, n;
    mem_header *space, *first;
    mem_descriptor *md;

    SC_LOCKON(SC_mm_lock);

    first = ph->latest_block;
    if (first != NULL)

/* NOTE: if F allocates or frees then N would be changed and
 * the latest_block pointer would be changed too
 * the loop would miss a number of blocks (allocates) or
 * process some blocks multiple times (frees)
 */
       {n = ph->n_mem_blocks;
	for (space = first, i = 0L, ok = TRUE;
	     (i < n) && (ok == TRUE) && (space != NULL);
	     space = space->block.next, i++)
	    {md = &space->block;
	     ok = f(ph, md, MEM_BLOCK_ACTIVE, a, i, -1);};};

    SC_LOCKOFF(SC_mm_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_MAP_FR - map F over all free blocks */

static void _SC_mem_map_fr(SC_heap_des *ph, PFMemMap f, void *a)
   {int ok;
    long i, j;
    mem_descriptor *md;

    for (j = 0L; j < _SC_ms.n_bins; j++)
        {for (i = 0L, md = ph->free_list[j];
	      md != NULL;
	      i++, md = (mem_descriptor *) md->where.pfunc)
	     {ok = f(ph, md, MEM_BLOCK_FREE, a, j, i);
	      if (ok == FALSE)
	         break;};};

    SC_ASSERT(i >= 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_MAP_REG - map F over all registered blocks */

static void _SC_mem_map_reg(SC_heap_des *ph, PFMemMap f, void *a)
   {long i;
    mem_descriptor *md;

    if (_SC.mem_table != NULL)
       {for (i = 0;
	     SC_hasharr_next(_SC.mem_table, &i, NULL, NULL, (void **) &md);
	     i++)
	    f(ph, md, MEM_BLOCK_REG, a, i, -1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_MAP_F - map F over memory sectors specified by TYP
 *              - if bit #1 of TYP is 1 include the allocated memory
 *              - if bit #2 of TYP is 1 include the freed memory
 *              - if bit #3 of TYP is 1 include the registered memory
 *
 * #bind SC_mem_map_f fortran() scheme(memory-check) python()
 */

void SC_mem_map_f(int typ, PFMemMap f, void *a)
   {int ok;
    SC_heap_des *ph;

    ph = _SC_tid_mm();

    SC_mem_over_mark(1);

    SC_LOCKON(SC_mm_lock);

/* this forces allocations which will come off the current
 * free lists at the first invocation
 * MUST be done before the ones in the loops over the free lists
 */
    ok = SC_pointer_ok(ph);
    SC_ASSERT(ok == TRUE);

    if (typ & 1)
       _SC_mem_map_act(ph, f, a);

    if (typ & 2)
       _SC_mem_map_fr(ph, f, a);

    if (typ & 4)
       _SC_mem_map_reg(ph, f, a);

    SC_LOCKOFF(SC_mm_lock);

    SC_mem_over_mark(-1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
