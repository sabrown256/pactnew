/*
 * SCMEMCL.C - full up memory manager with managed free lists
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

#define SC_BIN_UNITS(_n)  (((_n) < _SC_ms.block_size) ? _SC_ms.block_size/(_n) : 1)
# define SC_BIN_SIZE(_n)  (((_n) >= _SC_ms.n_bins) ? -1 : _SC_ms.bins[_n])


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRIM_MEMSET_L - efficiently zero out a memory block */

static void _SC_prim_memset_l(void *p, long nb)
   {long i, ni, nd;
    double *d;

    nd = sizeof(double);
    ni = (nb + nd - 1L) / nd;

    d = (double *) p;
    for (i = 0L; i < ni; i++, *d++ = 0.0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIN_INDEX - hash a byte size to an index */

static long _SC_bin_index(long n)
   {long m, imn, imx, rv;

    m = -1L;
    if (n > _SC_ms.bins[0])
       {m = _SC_ms.n_bins - 1L;

	if (n < _SC_ms.bins[m])
	   {imn = 0L;
	    imx = m;
	    for (m = (imn + imx) >> 1L; m > imn; m = (imn + imx) >> 1L)
	        {if (n <= _SC_ms.bins[m])
		    imx = m;
		 else
		    imn = m;};};};

    rv = m + 1L;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_BLOCKS - make blocks for the free list of heap PH, bin J */

static mem_descriptor *_SC_make_blocks(SC_heap_des *ph, long j)
   {int nu, us, ns, i;
    size_t tnb;
    char *pn;
    mem_descriptor *md, *ths;
    major_block_des mbl, *pm;
    static int mblsz = sizeof(major_block_des);

    us = ph->hdr_size + SC_BIN_SIZE(j);
    us = ((us + _SC_ms.mem_align_pad) >> _SC_ms.mem_align_expt) <<
         _SC_ms.mem_align_expt;
    nu = SC_BIN_UNITS(us);
    ns = nu*us;
    pn = _SC_ALLOC((size_t) ns);

    assert(pn != NULL);

/* setup the new major block */
    mbl.index  = j;
    mbl.nunits = nu;
    mbl.size   = us;
    mbl.block  = pn;

/* initialize the major block list if necessary */
    if (ph->major_block_list == NULL)
       {ph->nx_major_blocks = BLOCKS_UNIT_DELTA;
	ph->n_major_blocks  = 0;

	tnb = mblsz*ph->nx_major_blocks;

        SC_mem_over_mark(1);
	pm  = (major_block_des *) malloc(tnb);
        SC_mem_over_mark(-1);

	assert(pm != NULL);

	ph->major_block_list = pm;};

/* add the new major block */
    ph->major_block_list[ph->n_major_blocks++] = mbl;

/* expand the major block list if necessary */
    if (ph->n_major_blocks >= ph->nx_major_blocks)
       {ph->nx_major_blocks += BLOCKS_UNIT_DELTA;

	tnb = mblsz*ph->nx_major_blocks;

	pm = ph->major_block_list;
        SC_mem_over_mark(1);
	pm = (major_block_des *) realloc(pm, tnb);
        SC_mem_over_mark(-1);

	assert(pm != NULL);

	ph->major_block_list = pm;};

/* chain the new chunks on the block properly */
    md = (mem_descriptor *) pn;
    nu--;
    for (i = 0; i < nu; i++, pn += us)
        {ths = (mem_descriptor *) pn;
	 _SC_deassign_block(ph, ths, pn + us);
         ths->prev = NULL;
	 ths->next = NULL;};

    ths = (mem_descriptor *) pn;

    _SC_deassign_block(ph, ths, NULL);

    ths->prev = NULL;
    ths->next = NULL;

    return(md);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRIM_ALLOC - memory allocator that manages banks of small chunks
 *                - for efficiency
 */

static void *_SC_prim_alloc(size_t nbp, SC_heap_des *ph, int zsp)
   {long j, nb;
    mem_descriptor *md;
    void *p;

    if (nbp <= 0)
       return(NULL);

    nb = nbp - ph->hdr_size;
    j  = _SC_bin_index(nb);

/* if this chunk size is within SCORE managed space handle it here */
    if (j < _SC_ms.n_bins)
       {md = ph->free_list[j];

/* if we are out of free chunks get a block of them from the system */
        if (md == NULL)
           md = _SC_make_blocks(ph, j);

	p = NULL;
	if (md != NULL)

/* attach the new chunks to the free list */
	   {ph->free_list[j] = (mem_descriptor *) (md->where.pfunc);

/* take the top of the free list for this size chunk */
	    md->initialized = FALSE;
	    p = (void *) md;};}

/* otherwise go out to the system for memory
 * get a doubles worth of extra bytes to guarantee that
 * _SC_prim_memset_l won't clobber anything
 */
    else
       {if (zsp == 5)
           {p = calloc((size_t) 1, (size_t) (nbp + sizeof(double)));

	    assert(p != NULL);

	    md = (mem_descriptor *) p;
	    md->initialized = TRUE;}
        else
           {p = _SC_ALLOC((size_t) (nbp + sizeof(double)));

	    assert(p != NULL);

	    md = (mem_descriptor *) p;
	    md->initialized = FALSE;};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRIM_FREE - free small block counterpart to _SC_prim_alloc
 *               - for efficiency
 */

static void _SC_prim_free(void *p, long nbp, SC_heap_des *ph)
   {long j, nb;
    mem_descriptor *lst, *ths;

    if (p == NULL)
       return;

    nb = nbp - ph->hdr_size;
    j  = _SC_bin_index(nb);
    if (j < _SC_ms.n_bins)
       {ths = (mem_descriptor *) p;
        lst = ph->free_list[j];

	_SC_deassign_block(ph, ths, lst);

/* NOTE: this block is no longer in the active list and
 * MUST NOT point to blocks in the active list
 * the active blocks this could point to may be freed and this
 * block would not be updated with that information
 */
	ths->next = NULL;
	ths->prev = NULL;

        ph->free_list[j] = ths;}

    else
       _SC_FREE(p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_NL - add a layer of control over the C level memory management
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

void *_SC_alloc_nl(long ni, long bpi, void *arg)
   {int na, zsp, prm, typ, line;
    long nb, nbp;
    uint64_t a, f;
    char *func, *file;
    SC_mem_opt *opt;
    SC_heap_des *ph;
    mem_header *space;
    void *rv;

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

	if (SC_gs.mm_debug == TRUE)
	   space = (mem_header *) _SC_ALLOC((size_t) nbp);
	else
	   space = (mem_header *) _SC_prim_alloc((size_t) nbp, ph, zsp);

	if (space != NULL)
	   {mem_descriptor *desc;

	    _SC_assign_block(ph, space, nb, func, file, line);

	    _SC_mem_stats_acc(ph, nb, 0L);
    
	    ph->nx_mem_blocks++;
	    ph->n_mem_blocks++;

	    space->block.type = typ;

	    desc = (mem_descriptor *) space;
	    space++;

	    if (prm == TRUE)
	       desc->ref_count = UNCOLLECT;

/* zero out the space */
	    if ((zsp == 1) || (zsp == 2) || (zsp == 5))
	       {if (SC_gs.mm_debug == TRUE)
		   memset(space, 0, nb);
	        else if (desc->initialized == FALSE)
		   _SC_prim_memset_l(space, nb);};

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

/* _SC_REALLOC_NL - add a layer of control over the C level memory management
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

void *_SC_realloc_nl(void *p, long ni, long bpi, void *arg)
   {int na, zsp, prm;
    long nb, ob, db, nbp, obp;
    int64_t a, f;
    mem_header *space, *tmp;
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

	if (SC_gs.mm_debug)
	   {osp   = space;
	    space = (mem_header *) _SC_REALLOC((void *) osp, (size_t) nbp);
	    if (osp != prev)
	       prev->block.next = space;
	    else
	       space->block.next = space;
	    if (osp != next)
	       next->block.prev = space;
	    else
	       space->block.prev = space;}

	else
	   {int jo, jn, dn;

	    jn = _SC_bin_index(nb);
	    jo = _SC_bin_index(ob);
	    dn = 1;

	    if ((jo-dn >= jn) || (jn > jo) ||
		(jo >= _SC_ms.n_bins) || (jn >= _SC_ms.n_bins))
	       {tmp = (mem_header *) _SC_prim_alloc((size_t) nbp, ph, zsp);
		if (tmp == NULL)
		   space = NULL;
		else
		   {obp   = ob + ph->hdr_size;
		    osp   = space;
		    space = tmp;
		    memcpy(space, osp, min(obp, nbp));
		    if (osp != prev)
		       prev->block.next = space;
		    else
		       space->block.next = space;
		    if (osp != next)
		       next->block.prev = space;
		    else
		       space->block.prev = space;

		    _SC_prim_free((void *) osp, obp, ph);};};};
    
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

/* _SC_FREE_NL - the complementary routine for _SC_alloc_nl
 *             - free all the space including the counter
 *             - return TRUE if successful and FALSE otherwise
 */

int _SC_free_nl(void *p, void *arg)
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
       _SC_prim_memset_l(p, nb);

    if (SC_gs.mm_debug)
       _SC_FREE((void *) space);
    else
       _SC_prim_free((void *) space, nbp, ph);

    ph->n_mem_blocks--;

    SC_LOCKOFF(SC_mm_lock);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_WL - wrapper for _SC_alloc_nl */

void *_SC_alloc_wl(long ni, long bpi, char *name, int memfl, int zsp)
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

    p = _SC_alloc_nl(ni, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NALLOC_WL - wrapper for _SC_alloc_nl */

void *_SC_nalloc_wl(long ni, long bpi, int memfl, int zsp,
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

    p = _SC_alloc_nl(ni, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_WL - wrapper for _SC_realloc_nl */

void *_SC_realloc_wl(void *p, long ni, long bpi, int memfl, int zsp)
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

    rv = _SC_realloc_nl(p, ni, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_WL - wrapper for _SC_free_nl */

int _SC_free_wl(void *p, int zsp)
   {int rv;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = -1;
    opt.zsp  = zsp;
    opt.typ  = -1;

    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    rv = _SC_free_nl(p, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_USE_FULL_MM - use the SCORE memory manager */

SC_mem_fnc SC_use_full_mm(void)
   {SC_mem_fnc rv;

    rv = SC_gs.mm;

    SC_gs.mm.nalloc    = _SC_nalloc_wl;
    SC_gs.mm.alloc     = _SC_alloc_wl;
    SC_gs.mm.realloc   = _SC_realloc_wl;
    SC_gs.mm.free      = _SC_free_wl;

    SC_gs.mm.alloc_n   = _SC_alloc_nl;
    SC_gs.mm.realloc_n = _SC_realloc_nl;
    SC_gs.mm.free_n    = _SC_free_nl;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRUNE_MAJOR_BLOCKS - release empty major blocks
 *                        - return the number of blocks released
 */
 
int _SC_prune_major_blocks(void)
   {int i, j, nf, nbl, nmj;
    long nu, sz, nby;
    char *pn;
    mem_descriptor *desc, *md;
    major_block_des *mbl;
    SC_heap_des *ph;

    ph = _SC_tid_mm();

    nbl = 0;
    nby = 0;

    nmj = ph->n_major_blocks;
    mbl = ph->major_block_list;
    for (i = 0; i < nmj; i++)
        {pn = mbl[i].block;
	 nu = mbl[i].nunits;
	 sz = mbl[i].size;

         nf = 0;
	 for (j = 0; j < nu; j++, pn += sz)
	     {desc = (mem_descriptor *) pn;
	      nf += FREE_SCORE_BLOCK_P(desc);};

	 if (nf == nu)
	    {nbl++;
#if 0
	     printf("Major block %d is eligible to be released\n", i);
#else
	     nby += nu*sz;

             md = (mem_descriptor *) mbl[i].block;

             for (j = 0; j < _SC_ms.n_bins; j++)
	         {if (ph->free_list[j] == md)
                     {ph->free_list[j] = NULL;
		      break;};};

	     _SC_FREE(md);

	     mbl[i] = mbl[--nmj];
	     mbl[nmj].index  = -1;
	     mbl[nmj].nunits = 0;
	     mbl[nmj].size   = 0;
	     mbl[nmj].block  = NULL;
#endif
	    };};

    ph->n_major_blocks = nmj;

    return(nbl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRFREE - print the free memory lists */

void dprfree(long jmn, long jmx)
   {long i, j;
    SC_heap_des *ph;
    mem_descriptor *md;

    ph = _SC_tid_mm();

    if ((jmn < 0) || (jmx <= jmn))
       {jmn = 0L;
	jmx = _SC_ms.n_bins;};

    for (j = jmn; j < jmx; j++)
        {fprintf(stdout, "Bin %3ld   Max Size %4ld\n",
		 j, (long) SC_BIN_SIZE(j));
         for (md  = ph->free_list[j], i = 0L;
	      md != NULL;
	      md  = (mem_descriptor *) md->where.pfunc, i++)
             {fprintf(stdout, " %10p", md);
	      if (i % 6 == 5)
		 fprintf(stdout, "\n");
	      fflush(stdout);};
	 if (i % 6 != 0)
	    fprintf(stdout, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DFLPR - show the free list for the specified bin */

void dflpr(int j)
   {SC_heap_des *ph;
    mem_descriptor *md;

    ph = _SC_tid_mm();
    
    io_printf(stdout, "Free chunks of size %ld to %ld bytes\n",
	  (j < 1) ? 1L : SC_BIN_SIZE(j-1) + 1L,
	  SC_BIN_SIZE(j));
	  
    for (md = ph->free_list[j];
	 md != NULL;
	 md = (mem_descriptor *) md->where.pfunc)
        io_printf(stdout, "%8lx\n", md);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
