/*
 * SCMEMC.C - core memory management functions
 *          - WARNING: in the multi-threaded version threads are
 *          - assumed to NOT share allocated spaces
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#define SC_DEFINE

#include "score_int.h"
#include "scope_mem.h"

SC_mem_fnc
 SC_mem_hook = {SC_nalloc_na, SC_alloc_na, SC_realloc_na, SC_free};

SC_thread_lock
 SC_mm_lock = SC_LOCK_INIT_STATE,
 SC_mc_lock = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/

/*                     MEMORY STATISTICS ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SC_MEM_STATS_ACC - change the memory usage by A and F
 *                   - takes pthread id as argument
 */

static void _SC_mem_stats_acc(long a, long f, SC_heap_des *ph)
   {

    SC_SP_ALLOC(ph) += a;
    SC_SP_FREE(ph)  += f;

    SC_SP_DIFF(ph) = SC_SP_ALLOC(ph) - SC_SP_FREE(ph);
    SC_SP_MAX(ph)  = (SC_SP_MAX(ph) > SC_SP_DIFF(ph)) ?
                   SC_SP_MAX(ph) : SC_SP_DIFF(ph);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATS_ACC - change the memory usage by A and F */

void SC_mem_stats_acc(long a, long f)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    _SC_mem_stats_acc(a, f, ph);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATB - return memory usage statistics */

void SC_mem_statb(uint64_t *al, uint64_t *fr, uint64_t *df, uint64_t *mx)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    if (al != NULL)
       *al = SC_SP_ALLOC(ph);

    if (fr != NULL)
       *fr = SC_SP_FREE(ph);

    if (df != NULL)
       *df = SC_SP_DIFF(ph);

    if (mx != NULL)
       *mx = SC_SP_MAX(ph);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATB_SET - set the memory usage to A and F */

void SC_mem_statb_set(uint64_t a, uint64_t f)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    SC_SP_ALLOC(ph) = a;
    SC_SP_FREE(ph)  = f;

    SC_SP_DIFF(ph) = SC_SP_ALLOC(ph) - SC_SP_FREE(ph);
    SC_SP_MAX(ph)  = (SC_SP_MAX(ph) > SC_SP_DIFF(ph)) ?
                     SC_SP_MAX(ph) : SC_SP_DIFF(ph);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATS - return memory usage statistics */

void SC_mem_stats(long *al, long *fr, long *df, long *mx)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    if (al != NULL)
       *al = SC_SP_ALLOC(ph);

    if (fr != NULL)
       *fr = SC_SP_FREE(ph);

    if (df != NULL)
       *df = SC_SP_DIFF(ph);

    if (mx != NULL)
       *mx = SC_SP_MAX(ph);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATS_SET - set the memory usage to A and F */

void SC_mem_stats_set(long a, long f)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    SC_SP_ALLOC(ph) = a;
    SC_SP_FREE(ph)  = f;

    SC_SP_DIFF(ph) = SC_SP_ALLOC(ph) - SC_SP_FREE(ph);
    SC_SP_MAX(ph)  = (SC_SP_MAX(ph) > SC_SP_DIFF(ph)) ?
                     SC_SP_MAX(ph) : SC_SP_DIFF(ph);

    return;}

/*--------------------------------------------------------------------------*/

/*                           MEMORY HEAP ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* SC_CONFIGURE_MM - configure the memory manager bin structure
 *                 - bin sizes increase linearly up to MXL and
 *                 - exponentially up to the maximum managed size MXM
 *                 - for efficiency memory will be requested from the
 *                 - system in units of BSZ for bins with size <= BSZ
 *                 - and broken up into appropriately sized units to
 *                 - be added to the free lists
 *                 - for bins with size > BSZ single units will be
 *                 - requested from the system in order to keep applications
 *                 - from grabbing too much memory that may not be used
 *                 -   MXL - maximum block size for linear region
 *                 -   MXM - maximum managed block size
 *                 -   BSZ - maximum block size requested from the system
 *                 -   R   - bin size ratio in exponential region
 */

void SC_configure_mm(long mxl, long mxm, long bsz, double r)
   {long l, n, szbn;
    size_t nb;

    ONCE_SAFE(_SC_init_emu_threads == TRUE, &SC_mc_lock)

       _SC_mem_align_size = SC_MEM_ALIGN_SIZE;
       _SC_mem_align_pad  = _SC_mem_align_size - 1;
       _SC_mem_align_expt = 0;
       for (n = _SC_mem_align_size; 1L < n; n >>= 1L)
	   _SC_mem_align_expt++;

       _SC_block_size = bsz;

/* find the number of bins */
       if (mxm > 0L)

/* compute the number of linear bins */
	  {n    = (mxl >> _SC_mem_align_expt);
	   szbn = (n << _SC_mem_align_expt);

/* count the logarithmic bins */
	   for ( ; szbn < mxm; n++)
	       {l    = (long) (r*((double) szbn));
		szbn = (((l + _SC_mem_align_pad) >> _SC_mem_align_expt) <<
			_SC_mem_align_expt);};
	   _SC_n_bins = n;}

       else
	  _SC_n_bins = 1;

       nb = sizeof(long)*_SC_n_bins;
       _SC_mm_bins = (long *) malloc(nb);

       assert(_SC_mm_bins != NULL);

/* fill the linear region */
       for (n = 1L; _SC_mem_align_size*n <= mxl; n++)
	   _SC_mm_bins[n-1] = (n << _SC_mem_align_expt);

/* fill the exponential region */
       for (--n; n < _SC_n_bins; n++)
	   {l = (long) (r*((double) _SC_mm_bins[n-1]));
	    _SC_mm_bins[n] = (((l + _SC_mem_align_pad) >>
			       _SC_mem_align_expt) <<
			      _SC_mem_align_expt);};

    END_SAFE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIN_INDEX - hash a byte size to an index */

long _SC_bin_index(long n)
   {long m, imn, imx, rv;

    m = -1L;
    if (n > _SC_mm_bins[0])
       {m = _SC_n_bins - 1L;

	if (n < _SC_mm_bins[m])
	   {imn = 0L;
	    imx = m;
	    for (m = (imn + imx) >> 1L; m > imn; m = (imn + imx) >> 1L)
	        {if (n <= _SC_mm_bins[m])
		    imx = m;
		 else
		    imn = m;};};};

    rv = m + 1L;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ASSIGN_BLOCK - set the info in SPACE that is needed for
 *                  - a functioning active memory block
 */

static void INLINE _SC_assign_block(SC_heap_des *ph, mem_header *space,
				    long nb, char *func, char *file, int line)
   {mem_descriptor *desc;
    mem_header *prev, *next, *lb;

    desc = &space->block;

    ASSIGN_ID(desc);
    SET_HEAP(desc, ph);
    desc->ref_count = 0;
    desc->type      = 0;
    desc->length    = nb;
    desc->func      = func;
    desc->file      = file;
    desc->line      = line;

    lb = ph->latest_block;
    if (lb != NULL)
       {next = lb->block.next;
        prev = lb;
        next->block.prev = space;
        prev->block.next = space;
        desc->next = next;
        desc->prev = prev;
        ph->latest_block = space;}

    else
       {ph->latest_block             = space;
        ph->latest_block->block.prev = space;
        ph->latest_block->block.next = space;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static INLINE void _SC_deassign_block(SC_heap_des *ph, mem_descriptor *desc,
				      void *addr)
   {

    ASSIGN_ID(desc);

    SET_HEAP(desc, ph);

    desc->ref_count = SC_MEM_MFA;
    desc->type      = SC_MEM_MFB;
    desc->length    = 0L;

    desc->prev = NULL;
    desc->next = NULL;
    desc->func = (char *) addr;
    desc->file = NULL;
    desc->line = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_UNASSIGN_BLOCK - splice a block DESC out of an active heap PH */

static INLINE void _SC_unassign_block(SC_heap_des *ph, mem_header *space)
    {mem_header *prev, *next;
     mem_descriptor *desc;

     desc = &space->block;

/* splice the block out */
     prev = desc->prev;
     next = desc->next;

     if (space == next)
        ph->latest_block = NULL;

     if (prev != NULL)
        prev->block.next = next;

     if (next != NULL)
        next->block.prev = prev;

     if (space == ph->latest_block)
        ph->latest_block = next;

/* deassign the block */
     _SC_deassign_block(ph, desc, NULL);

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_INIT_HEAP - initialize an SC_heap instance */

void _SC_init_heap(SC_heap_des *ph, int id)
   {int i;
    size_t nb;
    mem_descriptor **lst;

    assert(ph != NULL);

    ONCE_SAFE(_SC_init_emu_threads == TRUE, &SC_mm_lock)
       SC_configure_mm(128L, 4096L, 4096L, 1.1);
    END_SAFE;

    _SC_thread_error(id, "initializing heap");

    nb  = _SC_n_bins*sizeof(mem_descriptor *);
    lst = (mem_descriptor **) malloc(nb);

    assert(lst != NULL);

    for (i = 0; i < _SC_n_bins; i++)
        lst[i] = NULL;

    SC_FREE_LIST(ph)        = lst;
    SC_HEAP_TID(ph)         = id;
    SC_MAJOR_BLOCK_LIST(ph) = NULL;
    SC_N_MAJOR_BLOCKS(ph)   = 0L;
    SC_NX_MAJOR_BLOCKS(ph)  = 0L;
    SC_MAX_MEM_BLOCKS(ph)   = 0L;
    SC_N_MEM_BLOCKS(ph)     = 0L;
/*    SC_HDR_SIZE_MAX(ph)     = (1L << NBITS) - 1; */
    SC_HDR_SIZE_MAX(ph)     = LONG_MAX;
    SC_HDR_SIZE(ph)         = sizeof(mem_header);
    SC_LATEST_BLOCK(ph)     = NULL;

    SC_SP_ALLOC(ph) = 0L;
    SC_SP_FREE(ph)  = 0L; 
    SC_SP_DIFF(ph)  = 0L;
    SC_SP_MAX(ph)   = 0L;

    SC_HEAP_INIT(ph) = TRUE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TID_MM - get the ID for the current thread
 *            - and be sure that there is an initialized heap instance
 *            - for this thread
 */

SC_heap_des *_SC_tid_mm(void)
   {int id;
    SC_heap_des *ph;

    id = SC_current_thread();
    ph = _SC_get_heap(id);

    return(ph);}

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

    us = SC_HDR_SIZE(ph) + SC_BIN_SIZE(j);
    us = ((us + _SC_mem_align_pad) >> _SC_mem_align_expt) <<
         _SC_mem_align_expt;
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
    if (SC_MAJOR_BLOCK_LIST(ph) == NULL)
       {SC_NX_MAJOR_BLOCKS(ph) = BLOCKS_UNIT_DELTA;
	SC_N_MAJOR_BLOCKS(ph)  = 0;

	tnb = mblsz*SC_NX_MAJOR_BLOCKS(ph);
	pm  = (major_block_des *) malloc(tnb);

	assert(pm != NULL);

	SC_MAJOR_BLOCK_LIST(ph) = pm;};

/* add the new major block */
    SC_MAJOR_BLOCK_LIST(ph)[SC_N_MAJOR_BLOCKS(ph)++] = mbl;

/* expand the major block list if necessary */
    if (SC_N_MAJOR_BLOCKS(ph) >= SC_NX_MAJOR_BLOCKS(ph))
       {SC_NX_MAJOR_BLOCKS(ph) += BLOCKS_UNIT_DELTA;

	tnb = mblsz*SC_NX_MAJOR_BLOCKS(ph);

	pm = SC_MAJOR_BLOCK_LIST(ph);
	pm = (major_block_des *) realloc(pm, tnb);

	assert(pm != NULL);

	SC_MAJOR_BLOCK_LIST(ph) = pm;};

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

    nb = nbp - SC_HDR_SIZE(ph);
    j  = SC_BIN_INDEX(nb);

/* if this chunk size is within SCORE managed space handle it here */
    if (j < _SC_n_bins)
       {md = SC_FREE_LIST(ph)[j];

/* if we are out of free chunks get a block of them from the system */
        if (md == NULL)
           md = _SC_make_blocks(ph, j);

	p = NULL;
	if (md != NULL)

/* attach the new chunks to the free list */
	   {SC_FREE_LIST(ph)[j] = (mem_descriptor *) (md->func);

/* take the top of the free list for this size chunk */
	    md->initialized = FALSE;
	    p = (void *) md;};}

/* otherwise go out to the system for memory
 * get a doubles worth of extra bytes to guarantee that
 * _SC_prim_memset won't clobber anything
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

    nb = nbp - SC_HDR_SIZE(ph);
    j  = SC_BIN_INDEX(nb);
    if (j < _SC_n_bins)
       {ths = (mem_descriptor *) p;
        lst = SC_FREE_LIST(ph)[j];

	_SC_deassign_block(ph, ths, lst);

/* NOTE: this block is no longer in the active list and
 * MUST NOT point to blocks in the active list
 * the active blocks this could point to may be freed and this
 * block would not be updated with that information
 */
	ths->next = NULL;
	ths->prev = NULL;

        SC_FREE_LIST(ph)[j] = ths;}

    else
       _SC_FREE(p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRIM_MEMSET - efficiently zero out a memory block */

static void _SC_prim_memset(void *p, long nb)
   {long i, ni, nd;
    double *d;

    nd = sizeof(double);
    ni = (nb + nd - 1L) / nd;

    d = (double *) p;
    for (i = 0L; i < ni; i++, *d++ = 0.0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_SCORE_SPACE - return TRUE iff the given pointer P
 *                   - points to space allocated by SCORE
 */

int SC_is_score_space(void *p, mem_header **psp, mem_descriptor **pds)
   {int ok;
    mem_header *space;
    mem_descriptor *desc;

    space = NULL;
    desc  = NULL;
    ok    = FALSE;
    if (p != NULL)
       {space = ((mem_header *) p) - 1;
	desc  = &space->block;
	ok    = SCORE_BLOCK_P(desc);};

    if (psp != NULL)
       *psp = space;

    if (pds != NULL)
       *pds = desc;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ALLOC_NZT - add a layer of control over the C level memory management
 *              - system to store the byte length of allocated spaces
 *              - a space EXTRA_WORD_SIZE greater than requested is allocated
 *              - the length in bytes is written into the first EXTRA_WORD_SIZE
 *              - bytes with a 4 bit marker in the high bits and a pointer
 *              - to the next byte is returned
 *              - if the maximum size is exceeded a NULL pointer is returned
 *              - iff NA TRUE fudge the accounting so that this block
 *              - will not show up in the bytes allocated count
 *              - set the type field of the descriptor to TYP
 */

void *SC_alloc_nzt(long nitems, long bpi, void *arg)
   {int na, zsp, typ, line;
    long nb, nbp;
    uint64_t a, f;
    char *func, *file;
    SC_mem_opt *opt;
    SC_heap_des *ph;
    mem_header *space;
    void *rv;

    ph = _SC_tid_mm();

    if (arg == NULL)
       {na   = FALSE;
	zsp  = _SC.zero_space;
	typ  = -1;
        func = NULL;
        func = NULL;
        line = -1;}
    else
       {opt  = (SC_mem_opt *) arg;
	na   = opt->na;
	zsp  = (opt->zsp == -1) ? _SC.zero_space : opt->zsp;
	typ  = opt->typ;
        func = (char *) opt->fnc;
        file = (char *) opt->file;
        line = opt->line;};

    nb  = nitems*bpi;
    nbp = nb + SC_HDR_SIZE(ph);

    if ((nb <= 0) || ((unsigned long) nb > SC_HDR_SIZE_MAX(ph)))
       return(NULL);

    SC_LOCKON(SC_mm_lock);

    if (na == TRUE)
       SC_mem_statb(&a, &f, NULL, NULL);

    if (SC_mm_debug == TRUE)
       space = (mem_header *) _SC_ALLOC((size_t) nbp);
    else
       space = (mem_header *) _SC_prim_alloc((size_t) nbp, ph, zsp);

    if (space != NULL)
       {mem_descriptor *desc;

        _SC_assign_block(ph, space, nb, func, file, line);

	_SC_mem_stats_acc((long) nb, 0L, ph);
    
	SC_MAX_MEM_BLOCKS(ph)++;
	SC_N_MEM_BLOCKS(ph)++;

	BLOCK_TYPE(&space->block) = typ;

        desc = (mem_descriptor *) space;
        space++;

/* zero out the space */
	if ((zsp == 1) || (zsp == 2) || (zsp == 5))
	   {if (SC_mm_debug == TRUE)
	       memset(space, 0, nb);
	    else if (desc->initialized == FALSE)
               _SC_prim_memset(space, nb);};

/* log this entry if doing memory history */
	if (_SC_mem_hst_hook != NULL)
	   (*_SC_mem_hst_hook)(SC_MEM_ALLOC, desc);};

    if (na == TRUE)
       SC_mem_statb_set(a, f);

    SC_LOCKOFF(SC_mm_lock);

    rv = space;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REALLOC_NZ - add a layer of control over the C level memory management
 *               - system to store the byte length of allocated spaces
 *               - a space EXTRA_WORD_SIZE greater than requested is reallocated
 *               - the length in bytes is written into the first EXTRA_WORD_SIZE
 *               - bytes with a 4 bit marker in the high bits and a pointer to
 *               - the next byte is returned
 *               - if the maximum size implied by the EXTRA_WORD_SIZE - 4 is
 *               - exceeded a NULL pointer is returned
 *               - iff NA TRUE fudge the accounting so that this block
 *               - will not show up in the bytes allocated count
 */

void *SC_realloc_nz(void *p, long nitems, long bpi, int na, int zsp)
   {long nb, ob, db, nbp, obp, a, f;
    mem_header *space, *tmp;
    mem_header *prev, *next, *osp;
    mem_descriptor *desc;
    SC_heap_des *ph;
    void *rv;

    space = NULL;
    
    if (SC_is_score_space(p, &space, &desc))
       {ph  = GET_HEAP(desc);
	nb  = nitems*bpi;
	nbp = nb + SC_HDR_SIZE(ph);

	if ((nb <= 0) ||
	    ((unsigned long) nb > SC_HDR_SIZE_MAX(ph)))
	   {SFREE(p);
	    return(NULL);};

	ob = BLOCK_LENGTH(desc);
	if (nb < ob)
	   return(p);

	db = nb - ob;
    
	prev = space->block.prev;
	next = space->block.next;

	SC_LOCKON(SC_mm_lock);

	if (na == TRUE)
	   SC_mem_stats(&a, &f, NULL, NULL);

	SAVE_LINKS(desc);

	if (SC_mm_debug)
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

	    jn = SC_BIN_INDEX(nb);
	    jo = SC_BIN_INDEX(ob);
	    dn = 1;

	    if ((jo-dn >= jn) || (jn > jo) ||
		(jo >= _SC_n_bins) || (jn >= _SC_n_bins))
	       {tmp = (mem_header *) _SC_prim_alloc((size_t) nbp, ph, zsp);
		if (tmp == NULL)
		   space = NULL;
		else
		   {obp   = ob + SC_HDR_SIZE(ph);
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

/* reset the reference count - nobody is pointing to this space
 * GOTCHA: should we allow a realloc with multiple references
 */
	    desc->ref_count = 0;
	    BLOCK_LENGTH(desc) = nb;
	    _SC_mem_stats_acc((long) db, 0L, ph);

	    space++;

/* zero out the new space */
	    if ((db > 0) && ((zsp == 1) || (zsp == 2)))
	       memset(((char *) space + ob), 0, db);

/* log this entry if doing memory history */
	    if (_SC_mem_hst_hook != NULL)
	       (*_SC_mem_hst_hook)(SC_MEM_REALLOC, desc);};

	if (na == TRUE)
	   SC_mem_stats_set(a, f);

	SC_LOCKOFF(SC_mm_lock);};

    rv = space;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_Z - the complementary routine for SC_alloc
 *           - free all the space including the counter
 *           - return TRUE if successful and FALSE otherwise
 */

int SC_free_z(void *p, int zsp)
   {long nb, nbp;
    SC_heap_des *ph;
    mem_header *space;
    mem_descriptor *desc;

    if (p == NULL)
       return(TRUE);

    space = ((mem_header *) p) - 1;
    desc  = &space->block;
    if (!SCORE_BLOCK_P(desc))
       return(FALSE);

    if (REF_COUNT(desc) == UNCOLLECT)
       return(TRUE);

    if (FREE_SCORE_BLOCK_P(desc))
       return(TRUE);

    if (--REF_COUNT(desc) > 0)
       return(TRUE);

    ph = GET_HEAP(desc);
    if (ph == NULL)
       return(TRUE);

    zsp = (zsp == -1) ? _SC.zero_space : zsp;

/* log this entry if doing memory history */
    if (_SC_mem_hst_hook != NULL)
       (*_SC_mem_hst_hook)(SC_MEM_FREE, desc);

    nb  = BLOCK_LENGTH(desc);
    nbp = nb + SC_HDR_SIZE(ph);

    SC_LOCKON(SC_mm_lock);

    _SC_unassign_block(ph, space);

    _SC_mem_stats_acc(0L, (long) (nbp - SC_HDR_SIZE(ph)), ph);

    if ((zsp == 1) || (zsp == 3))
       _SC_prim_memset(space, nbp);
    else
       {_SC_deassign_block(ph, desc, NULL);};

    if (SC_mm_debug)
       _SC_FREE((void *) space);
    else
       _SC_prim_free((void *) space, nbp, ph);

    SC_N_MEM_BLOCKS(ph)--;

    SC_LOCKOFF(SC_mm_lock);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ALLOC_NZ - add a layer of control over the C level memory management
 *             - system to store the byte length of allocated spaces
 *             - a space EXTRA_WORD_SIZE greater than requested is allocated
 *             - the length in bytes is written into the first EXTRA_WORD_SIZE
 *             - bytes with a 4 bit marker in the high bits and a pointer
 *             - to the next byte is returned
 *             - if the maximum size is exceeded a NULL pointer is returned
 *             - iff NA TRUE fudge the accounting so that this block
 *             - will not show up in the bytes allocated count
 */

void *SC_alloc_nz(long nitems, long bpi, char *name, int na, int zsp)
   {void *p;
    SC_mem_opt opt;

    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;
    opt.fnc  = name;
    opt.file = NULL;
    opt.line = -1;

    p = SC_alloc_nzt(nitems, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_NALLOC_NA - add a layer of control over the C level memory management
 *              - system to store the byte length of allocated spaces
 *              - a space EXTRA_WORD_SIZE greater than requested is allocated
 *              - the length in bytes is written into the first EXTRA_WORD_SIZE
 *              - bytes with a 4 bit marker in the high bits and a pointer to the
 *              - next byte is returned
 *              - if the maximum size is exceeded a NULL pointer is returned
 *              - iff NA TRUE fudge the accounting so that this block
 *              - will not show up in the bytes allocated count
 */

void *SC_nalloc_na(long nitems, long bpi, int na,
		   const char *fnc, const char *file, int line)
   {void *p;
    SC_mem_opt opt;

    opt.na   = na;
    opt.zsp  = _SC.zero_space;
    opt.typ  = -1;
    opt.fnc  = fnc;
    opt.file = file;
    opt.line = line;

    p = SC_alloc_nzt(nitems, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ALLOC_NA - add a layer of control over the C level memory management
 *             - system to store the byte length of allocated spaces
 *             - a space EXTRA_WORD_SIZE greater than requested is allocated
 *             - the length in bytes is written into the first EXTRA_WORD_SIZE
 *             - bytes with a 4 bit marker in the high bits and a pointer to the
 *             - next byte is returned
 *             - if the maximum size is exceeded a NULL pointer is returned
 *             - iff NA TRUE fudge the accounting so that this block
 *             - will not show up in the bytes allocated count
 */

void *SC_alloc_na(long nitems, long bpi, char *name, int na)
   {void *p;
    SC_mem_opt opt;

    opt.na  = na;
    opt.zsp = _SC.zero_space;
    opt.typ = -1;
    opt.fnc  = name;
    opt.file = NULL;
    opt.line = -1;

    p = SC_alloc_nzt(nitems, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REALLOC_NA - add a layer of control over the C level memory management
 *               - system to store the byte length of allocated spaces
 *               - a space EXTRA_WORD_SIZE greater than requested is reallocated
 *               - the length in bytes is written into the first EXTRA_WORD_SIZE
 *               - bytes with a 4 bit marker in the high bits and a pointer to
 *               - the next byte is returned
 *               - if the maximum size implied by the EXTRA_WORD_SIZE - 4 is
 *               - exceeded a NULL pointer is returned
 *               - iff NA TRUE fudge the accounting so that this block
 *               - will not show up in the bytes allocated count
 */

void *SC_realloc_na(void *p, long nitems, long bpi, int na)
   {void *rv;

    rv = SC_realloc_nz(p, nitems, bpi, na, _SC.zero_space);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE - the complementary routine for SC_alloc
 *         - free all the space including the counter
 *         - return TRUE if successful and FALSE otherwise
 */

int SC_free(void *p)
   {int rv;

    rv = SC_free_z(p, _SC.zero_space);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ZERO_SPACE_N - set zero space flag on heap for thread TID
 *                 - thread action
 *                 -   -2 : all heaps
 *                 -   -1 : current heap
 *                 -  0-N : specified heap
 *                 - flag action
 *                 -   0 : don't zero
 *                 -   1 : zero on alloc and free
 *                 -   2 : zero on alloc only
 *                 -   3 : zero on free only
 *                 - return the original value
 */

int SC_zero_space_n(int flag, int tid)
   {int it, itn, itx, rv;
    SC_heap_des *ph;

    SC_LOCKON(SC_mm_lock);

    rv = 0;

    switch (tid)
       {case -2 :
	     itn = 0;
	     itx = SC_get_n_thread();
	     break;
        case -1 :
	     itn = SC_current_thread();
	     itx = itn+1;
	     break;
	default :
	     itn = tid;
	     itx = itn+1;
	     break;};

    for (it = itn; it < itx; it++)
        {ph = _SC_get_heap(it);
	 if (ph != NULL)
	    {rv = ph->zero_space;
	     if ((0 <= flag) && (flag <= 5))
	        ph->zero_space = flag;}};

    SC_LOCKOFF(SC_mm_lock);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ZERO_SPACE - set flag to zero space
 *               - 0 : don't zero
 *               - 1 : zero on alloc and free
 *               - 2 : zero on alloc only
 *               - 3 : zero on free only
 *               - return the original value
 */

int SC_zero_space(int flag)
   {int rv;

    rv = _SC.zero_space;
    if ((0 <= flag) && (flag <= 5))
       {SC_zero_space_n(flag, -2);
	_SC.zero_space = flag;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ZERO_ON_ALLOC - return TRUE iff zero_space is 1 or 2 */

int SC_zero_on_alloc(void)
   {int rv;

    rv = ((_SC.zero_space == 1) || (_SC.zero_space == 2));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCZRSP - fortran interface for SC_zero_space */

FIXNUM F77_FUNC(sczrsp, SCZRSP)(FIXNUM *pf)
   {FIXNUM zsp;

    zsp = SC_zero_space((int) *pf);

    return(zsp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
