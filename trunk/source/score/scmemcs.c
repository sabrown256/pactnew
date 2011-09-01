/*
 * SCMEMCS.C - small memory manager
 *           - when you have a good, fast, malloc
 *           - adds mem_descriptor layer only to system routines
 *           - WARNING: in the multi-threaded version threads are
 *           - assumed to NOT share allocated spaces
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

/* _SC_ALLOC_NS - add a layer of control over the C level memory management
 *              - system to store the byte length of allocated spaces
 *              - attributes arriving in ARG are:
 *              -
 *              -    SC_MEM_ATTR_NO_ACCOUNT   memory block not include in totals
 *              -    SC_MEM_ATTR_ZERO_SPACE   set zero_space behavior
 *              -    SC_MEM_ATTR_TYPE         data type index
 *              -    SC_MEM_ATTR_FUNC         function containing call
 *              -    SC_MEM_ATTR_FILE         file containing function
 *              -    SC_MEM_ATTR_LINE         line number in file
 */

void *_SC_alloc_ns(long ni, long bpi, void *arg)
   {int na, zsp, prm, typ, line;
    long nb, nbp;
    uint64_t a, f;
    char *func, *file;
    SC_mem_opt *opt;
    SC_heap_des *ph;
    mem_header *space;
    mem_descriptor *desc;
    void *p, *rv;

    ph = _SC_tid_mm();

    if (arg == NULL)
       {func = NULL;
        file = NULL;
        line = -1;
	prm  = FALSE;
	na   = FALSE;
	zsp  = ph->zero_space;
	typ  = -1;}
    else
       {opt  = (SC_mem_opt *) arg;
        func = (char *) opt->where.pfunc;
        file = (char *) opt->where.pfile;
        line = opt->where.line;
	prm  = opt->perm;
	na   = opt->na;
	if ((line == -1) && (func != NULL) && (strncmp(func, "PERM|", 5) == 0))
	   {prm = TRUE;
	    na  = TRUE;};
	zsp  = (opt->zsp == -1) ? ph->zero_space : opt->zsp;
	typ  = opt->typ;};

    rv  = NULL;
    nb  = ni*bpi;
    nbp = nb + ph->hdr_size;

    if ((0 < nb) && ((unsigned long) nb <= ph->hdr_size_max))
       {SC_LOCKON(SC_mm_lock);

	if (na == TRUE)
	   SC_mem_statb(&a, &f, NULL, NULL);

	desc = NULL;
	if (nbp > 0)
	   {if (zsp == 5)
	       {p = calloc((size_t) 1, (size_t) (nbp + sizeof(double)));

		assert(p != NULL);

		desc = (mem_descriptor *) p;
		desc->initialized = TRUE;}
	    else
	       {p = _SC_ALLOC((size_t) (nbp + sizeof(double)));

		assert(p != NULL);

		desc = (mem_descriptor *) p;
		desc->initialized = FALSE;};};

	space = (mem_header *) desc;
	if (space != NULL)
	   {_SC_assign_block(ph, space, nb, func, file, line);

	    _SC_mem_stats_acc(ph, nb, 0L);
    
	    ph->nx_mem_blocks++;
	    ph->n_mem_blocks++;

	    space->block.type = typ;

	    space++;

	    if (prm == TRUE)
	       desc->ref_count = UNCOLLECT;

/* zero out the space */
	    if ((zsp == 1) || (zsp == 2) || (zsp == 5))
	       {if (SC_gs.mm_debug == TRUE)
		   memset(space, 0, nb);
	        else if (desc->initialized == FALSE)
		   _SC_prim_memset(space, nb);};

/* log this entry if doing memory history */
	    if (_SC.mem_hst != NULL)
	       _SC.mem_hst(SC_MEM_ALLOC, desc);};

	if (na == TRUE)
	   SC_mem_stats_set(a, f);

	SC_LOCKOFF(SC_mm_lock);

	rv = space;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_NS - add a layer of control over the C level memory management
 *                - system to store the byte length of allocated spaces
 *                - attributes arriving in ARG are:
 *                -
 *                -    SC_MEM_ATTR_NO_ACCOUNT   memory block not include in totals
 *                -    SC_MEM_ATTR_ZERO_SPACE   set zero_space behavior
 *                -    SC_MEM_ATTR_TYPE         data type index
 *                -    SC_MEM_ATTR_FUNC         function containing call
 *                -    SC_MEM_ATTR_FILE         file containing function
 *                -    SC_MEM_ATTR_LINE         line number in file
 */

void *_SC_realloc_ns(void *p, long ni, long bpi, void *arg)
   {int na, zsp, prm;
    long nb, ob, db, nbp;
    int64_t a, f;
    mem_header *space;
    mem_header *prev, *next, *osp;
    mem_descriptor *desc;
    SC_mem_opt *opt;
    SC_heap_des *ph;
    void *rv;

    space = NULL;
    
    if (SC_is_active_space(p, &space, &desc))
       {ph  = desc->heap;

	if (arg == NULL)
	   {prm = FALSE;
	    na  = FALSE;
	    zsp = ph->zero_space;}
	else
	   {opt = (SC_mem_opt *) arg;
	    prm = opt->perm;
	    na  = opt->na;
	    zsp = (opt->zsp == -1) ? ph->zero_space : opt->zsp;};

	prm |= (desc->ref_count == UNCOLLECT);

	nb  = ni*bpi;
	nbp = nb + ph->hdr_size;

	if ((nb <= 0) ||
	    ((unsigned long) nb > ph->hdr_size_max))
	   {CFREE(p);
	    return(NULL);};

	ob = desc->length;
	if (nb < ob)
	   return(p);

	db = nb - ob;
    
	prev = space->block.prev;
	next = space->block.next;

	SC_LOCKON(SC_mm_lock);

	if (na == TRUE)
	   SC_mem_stats(&a, &f, NULL, NULL);

/* get the block links */
	prev = desc->prev;
	next = desc->next;
	if (space == ph->latest_block)
	   ph->latest_block = next;

	osp   = space;
	space = (mem_header *) _SC_REALLOC((void *) osp, (size_t) nbp);
	if (osp != prev)
	   prev->block.next = space;
	else
	   space->block.next = space;
	if (osp != next)
	   next->block.prev = space;
	else
	   space->block.prev = space;

	if (space != NULL)
	   {desc = &space->block;

	    if (prm == TRUE)
	       desc->ref_count = UNCOLLECT;

/* reset the reference count - nobody is pointing to this space
 * GOTCHA: should we allow a realloc with multiple references
 */
	    else
	       {desc->ref_count = 0;
		_SC_mem_stats_acc(ph, db, 0L);};

	    desc->length = nb;

	    space++;

/* zero out the new space */
	    if ((db > 0) && ((zsp == 1) || (zsp == 2)))
	       memset(((char *) space + ob), 0, db);

/* log this entry if doing memory history */
	    if (_SC.mem_hst != NULL)
	       _SC.mem_hst(SC_MEM_REALLOC, desc);};

	if (na == TRUE)
	   SC_mem_stats_set(a, f);

	SC_LOCKOFF(SC_mm_lock);};

    rv = space;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_NS - the complementary routine for _SC_alloc_ns
 *             - free all the space including the counter
 *             - return TRUE if successful and FALSE otherwise
 */

int _SC_free_ns(void *p, void *arg)
   {int zsp;
    long nb, nbp;
    SC_mem_opt *opt;
    SC_heap_des *ph;
    mem_header *space;
    mem_descriptor *desc;

    if (p == NULL)
       return(TRUE);

    space = ((mem_header *) p) - 1;
    desc  = &space->block;
    if (!SCORE_BLOCK_P(desc))
       return(FALSE);

    if (desc->ref_count == UNCOLLECT)
       return(TRUE);

    if (FREE_SCORE_BLOCK_P(desc))
       return(TRUE);

    if (--(desc->ref_count) > 0)
       return(TRUE);

    ph = desc->heap;
    if (ph == NULL)
       return(TRUE);

    if (arg == NULL)
       zsp = ph->zero_space;
    else
       {opt = (SC_mem_opt *) arg;
	zsp = (opt->zsp == -1) ? ph->zero_space : opt->zsp;};

/* log this entry if doing memory history */
    if (_SC.mem_hst != NULL)
       _SC.mem_hst(SC_MEM_FREE, desc);

    nb  = desc->length;
    nbp = nb + ph->hdr_size;

    SC_LOCKON(SC_mm_lock);

    _SC_unassign_block(ph, space);

    _SC_mem_stats_acc(ph, 0L, nb);
    if ((zsp == 1) || (zsp == 3))
       _SC_prim_memset(p, nb);

    _SC_FREE((void *) space);

    ph->n_mem_blocks--;

    SC_LOCKOFF(SC_mm_lock);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_WS - wrapper for _SC_alloc_ns */

void *_SC_alloc_ws(long ni, long bpi, char *name, int memfl, int zsp)
   {int prm, na;
    void *p;
    SC_mem_opt opt;

    prm = ((memfl & 1) != 0);
    na  = ((memfl & 2) != 0);

    opt.perm = prm;
    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;

    opt.where.pfunc = name;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    p = _SC_alloc_ns(ni, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NALLOC_WS - wrapper for _SC_alloc_ns */

void *_SC_nalloc_ws(long ni, long bpi, int memfl, int zsp,
		    const char *fnc, const char *file, int line)
   {int prm, na;
    void *p;
    SC_mem_opt opt;

    prm = ((memfl & 1) != 0);
    na  = ((memfl & 2) != 0);

    opt.perm = prm;
    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;

    opt.where.pfunc = fnc;
    opt.where.pfile = file;
    opt.where.line  = line;

    p = _SC_alloc_ns(ni, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_WS - wrapper for _SC_realloc_ns */

void *_SC_realloc_ws(void *p, long ni, long bpi, int memfl, int zsp)
   {int prm, na;
    void *rv;
    SC_mem_opt opt;

    prm = ((memfl & 1) != 0);
    na  = ((memfl & 2) != 0);

    opt.perm = prm;
    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;

    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    rv = _SC_realloc_ns(p, ni, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_WS - wrapper for _SC_free_ns */

int _SC_free_ws(void *p, int zsp)
   {int rv;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = -1;
    opt.zsp  = zsp;
    opt.typ  = -1;

    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    rv = _SC_free_ns(p, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_USE_REDUCED_MM - use the reduced SCORE memory manager */

SC_mem_fnc SC_use_reduced_mm(void)
   {SC_mem_fnc rv;

    rv = SC_gs.mm;

    SC_gs.mm.nalloc  = _SC_nalloc_ws;
    SC_gs.mm.alloc   = _SC_alloc_ws;
    SC_gs.mm.realloc = _SC_realloc_ws;
    SC_gs.mm.free    = _SC_free_ws;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_N_BLOCKS_S - return the number of blocks */

int _SC_n_blocks_s(SC_heap_des *ph, int flag)
   {int n;
    long nt, ntc, ntx;

/* count the number of blocks we are going to have
 * this may significantly over count but this is for allocating space
 */
    n = 1;

/* add active block count */
    if ((flag & 1) && (ph->latest_block != NULL))
       {ntx = ph->nx_mem_blocks;
	ntc = ph->n_mem_blocks;
	nt  = min(ntx, 10*ntc);
	n  += nt + BLOCKS_UNIT_DELTA;};

/* add registered block count */
    if ((flag & 4) && (_SC.mem_table != NULL))
       n += _SC.mem_table->size;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
