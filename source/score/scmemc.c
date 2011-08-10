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

SC_thread_lock
 SC_mm_lock = SC_LOCK_INIT_STATE,
 SC_mc_lock = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/

/*                     MEMORY STATISTICS ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SC_MEM_STATS_ACC - change the memory usage by A and F
 *                   - takes pthread id as argument
 */

static void _SC_mem_stats_acc(SC_heap_des *ph, long a, long f)
   {

    ph->sp_alloc += a;
    ph->sp_free  += f;

    ph->sp_diff = ph->sp_alloc - ph->sp_free;
    ph->sp_max  = (ph->sp_max > ph->sp_diff) ?
                     ph->sp_max : ph->sp_diff;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATS_ACC - change the memory usage by A and F */

void SC_mem_stats_acc(long a, long f)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    _SC_mem_stats_acc(ph, a, f);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATB - return memory usage statistics */

void SC_mem_statb(uint64_t *al, uint64_t *fr, uint64_t *df, uint64_t *mx)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    if (al != NULL)
       *al = ph->sp_alloc;

    if (fr != NULL)
       *fr = ph->sp_free;

    if (df != NULL)
       *df = ph->sp_diff;

    if (mx != NULL)
       *mx = ph->sp_max;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATB_SET - set the memory usage to A and F */

void SC_mem_statb_set(uint64_t a, uint64_t f)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    ph->sp_alloc = a;
    ph->sp_free  = f;

    ph->sp_diff = ph->sp_alloc - ph->sp_free;
    ph->sp_max  = (ph->sp_max > ph->sp_diff) ?
                     ph->sp_max : ph->sp_diff;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATS - return memory usage statistics
 *
 * #bind SC_mem_stats fortran() scheme(memory-usage) python()
 */

void SC_mem_stats(long *al, long *fr, long *df, long *mx)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    if (al != NULL)
       *al = ph->sp_alloc;

    if (fr != NULL)
       *fr = ph->sp_free;

    if (df != NULL)
       *df = ph->sp_diff;

    if (mx != NULL)
       *mx = ph->sp_max;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATS_SET - set the memory usage to A and F */

void SC_mem_stats_set(long a, long f)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    ph->sp_alloc = a;
    ph->sp_free  = f;

    ph->sp_diff = ph->sp_alloc - ph->sp_free;
    ph->sp_max  = (ph->sp_max > ph->sp_diff) ?
                     ph->sp_max : ph->sp_diff;

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
 *
 * #bind SC_configure_mm fortran() scheme(sc-configure-mm) python()
 */

void SC_configure_mm(long mxl, long mxm, long bsz, double r)
   {long l, n, szbn;
    size_t nb;

    ONCE_SAFE(_SC_init_emu_threads == TRUE, &SC_mc_lock)

       _SC_ms.mem_align_size = SC_MEM_ALIGN_SIZE;
       _SC_ms.mem_align_pad  = _SC_ms.mem_align_size - 1;
       _SC_ms.mem_align_expt = 0;
       for (n = _SC_ms.mem_align_size; 1L < n; n >>= 1L)
	   _SC_ms.mem_align_expt++;

       _SC_ms.block_size = bsz;

/* find the number of bins */
       if (mxm > 0L)

/* compute the number of linear bins */
	  {n    = (mxl >> _SC_ms.mem_align_expt);
	   szbn = (n << _SC_ms.mem_align_expt);

/* count the logarithmic bins */
	   for ( ; szbn < mxm; n++)
	       {l    = (long) (r*((double) szbn));
		szbn = (((l + _SC_ms.mem_align_pad) >> _SC_ms.mem_align_expt) <<
			_SC_ms.mem_align_expt);};
	   _SC_ms.n_bins = n;}

       else
	  _SC_ms.n_bins = 1;

       nb = sizeof(long)*_SC_ms.n_bins;
       _SC_ms.bins = (long *) malloc(nb);

       assert(_SC_ms.bins != NULL);

/* fill the linear region */
       for (n = 1L; _SC_ms.mem_align_size*n <= mxl; n++)
	   _SC_ms.bins[n-1] = (n << _SC_ms.mem_align_expt);

/* fill the exponential region */
       for (--n; n < _SC_ms.n_bins; n++)
	   {l = (long) (r*((double) _SC_ms.bins[n-1]));
	    _SC_ms.bins[n] = (((l + _SC_ms.mem_align_pad) >>
			       _SC_ms.mem_align_expt) <<
			      _SC_ms.mem_align_expt);};

    END_SAFE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_BIN_INDEX - hash a byte size to an index */

long _SC_bin_index(long n)
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

/* _SC_ASSIGN_BLOCK - set the info in SPACE that is needed for
 *                  - a functioning active memory block
 */

static void INLINE _SC_assign_block(SC_heap_des *ph, mem_header *space,
				    long nb, char *func, char *file, int line)
   {mem_descriptor *desc;
    mem_header *prev, *next, *lb;

    desc = &space->block;

    desc->id         = SC_MEM_ID;
    desc->heap       = ph;
    desc->ref_count  = 0;
    desc->type       = 0;
    desc->length     = nb;
    desc->where.pfunc = func;
    desc->where.pfile = file;
    desc->where.line  = line;

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

    desc->id        = SC_MEM_ID;
    desc->heap      = ph;
    desc->ref_count = SC_MEM_MFA;
    desc->type      = SC_MEM_MFB;
    desc->length    = 0L;

    desc->prev = NULL;
    desc->next = NULL;
    desc->where.pfunc = (char *) addr;
    desc->where.pfile = NULL;
    desc->where.line  = -1;

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
   {int i, zsp;
    size_t nb;
    mem_descriptor **lst;

    assert(ph != NULL);

    ONCE_SAFE(_SC_init_emu_threads == TRUE, &SC_mm_lock)
       SC_configure_mm(128L, 4096L, 4096L, 1.1);
    END_SAFE;

    _SC_thread_error(id, "initializing heap");

    nb  = _SC_ms.n_bins*sizeof(mem_descriptor *);
    lst = (mem_descriptor **) malloc(nb);

    assert(lst != NULL);

    for (i = 0; i < _SC_ms.n_bins; i++)
        lst[i] = NULL;

/* new heaps inherit the zero_space flag from heap 0 */
    if (id > 0)
       {SC_heap_des *phz;

	phz = _SC_get_heap(0);
	zsp = phz->zero_space;}
    else
       zsp = 0;

    ph->init             = TRUE;
    ph->tid              = id;
    ph->zero_space       = zsp;
    ph->free_list        = lst;
    ph->latest_block     = NULL;
    ph->major_block_list = NULL;
    ph->n_major_blocks   = 0L;
    ph->nx_major_blocks  = 0L;
    ph->n_mem_blocks     = 0L;
    ph->nx_mem_blocks    = 0L;
    ph->hdr_size         = sizeof(mem_header);
    ph->hdr_size_max     = LONG_MAX;
    ph->sp_alloc         = 0L;
    ph->sp_free          = 0L; 
    ph->sp_diff          = 0L;
    ph->sp_max           = 0L;
    ph->ih               = 0L;
    ph->nh               = 0L;
    ph->ring             = NULL;

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
    j  = SC_BIN_INDEX(nb);

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

    nb = nbp - ph->hdr_size;
    j  = SC_BIN_INDEX(nb);
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

/* _SC_ALLOC_N - add a layer of control over the C level memory management
 *             - system to store the byte length of allocated spaces
 *             - attributes arriving in ARG are:
 *             -
 *             -    SC_MEM_ATTR_NO_ACCOUNT   memory block not include in totals
 *             -    SC_MEM_ATTR_ZERO_SPACE   set zero_space behavior
 *             -    SC_MEM_ATTR_TYPE         data type index
 *             -    SC_MEM_ATTR_FUNC         function containing call
 *             -    SC_MEM_ATTR_FILE         file containing function
 *             -    SC_MEM_ATTR_LINE         line number in file
 */

void *_SC_alloc_n(long ni, long bpi, void *arg)
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
		   _SC_prim_memset(space, nb);};

/* log this entry if doing memory history */
	    if (_SC.mem_hst != NULL)
	       _SC.mem_hst(SC_MEM_ALLOC, desc);};

	if (na == TRUE)
	   SC_mem_statb_set(a, f);

	SC_LOCKOFF(SC_mm_lock);

	rv = space;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ALLOC_N - add a layer of control over the C level memory management
 *            - system to store extra information about the allocated spaces
 *            - if the maximum size is exceeded a NULL pointer is returned
 *            - options are:
 *            -
 *            -    SC_MEM_ATTR_NO_ACCOUNT   memory block not include in totals
 *            -    SC_MEM_ATTR_ZERO_SPACE   set zero_space behavior
 *            -    SC_MEM_ATTR_TYPE         data type index
 *            -    SC_MEM_ATTR_FUNC         function containing call
 *            -    SC_MEM_ATTR_FILE         file containing function
 *            -    SC_MEM_ATTR_LINE         line number in file
 *            -
 *            - Example:
 *            -
 *            -  p = SC_alloc_n(10, sizeof(int),
 *            -                 SC_MEM_ATTR_ZERO_SPACE, 2,
 *            -                 SC_MEM_ATTR_FUNC, "my_func",
 *            -                 SC_MEM_ATTR_FILE, "my_file.c",
 *            -                 SC_MEM_ATTR_LINE, 384,
 *            -                 0);
 */

void *SC_alloc_n(long ni, long bpi, ...)
   {int is, ok;
    void *rv;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = -1;

    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    SC_VA_START(bpi);
    for (ok = TRUE; ok == TRUE; )
        {is = SC_VA_ARG(int);
         switch (is)
            {case SC_MEM_ATTR_PERMANENT :
                  opt.perm = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_NO_ACCOUNT :
                  opt.na = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_ZERO_SPACE :
                  opt.zsp = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_TYPE :
                  opt.typ = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_FUNC :
                  opt.where.pfunc = SC_VA_ARG(char *);
                  break;
             case SC_MEM_ATTR_FILE :
                  opt.where.pfile = SC_VA_ARG(char *);
                  break;
             case SC_MEM_ATTR_LINE :
                  opt.where.line = SC_VA_ARG(int);
                  break;
	     default :
                  ok = FALSE;
	          break;};};

    SC_VA_END;

    rv = _SC_alloc_n(ni, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_N - add a layer of control over the C level memory management
 *               - system to store the byte length of allocated spaces
 *               - attributes arriving in ARG are:
 *               -
 *               -    SC_MEM_ATTR_NO_ACCOUNT   memory block not include in totals
 *               -    SC_MEM_ATTR_ZERO_SPACE   set zero_space behavior
 *               -    SC_MEM_ATTR_TYPE         data type index
 *               -    SC_MEM_ATTR_FUNC         function containing call
 *               -    SC_MEM_ATTR_FILE         file containing function
 *               -    SC_MEM_ATTR_LINE         line number in file
 */

void *_SC_realloc_n(void *p, long ni, long bpi, void *arg)
   {int na, zsp, prm;
    long nb, ob, db, nbp, obp, a, f;
    mem_header *space, *tmp;
    mem_header *prev, *next, *osp;
    mem_descriptor *desc;
    SC_mem_opt *opt;
    SC_heap_des *ph;
    void *rv;

    space = NULL;
    
    if (SC_is_score_space(p, &space, &desc))
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

	    jn = SC_BIN_INDEX(nb);
	    jo = SC_BIN_INDEX(ob);
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

/* SC_REALLOC_N - add a layer of control over the C level memory management
 *              - system to store extra information about the allocated spaces
 *              - if the maximum size is exceeded a NULL pointer is returned
 *              - options are:
 *              -
 *              -    SC_MEM_ATTR_NO_ACCOUNT   memory block not include in totals
 *              -    SC_MEM_ATTR_ZERO_SPACE   set zero_space behavior
 *              -    SC_MEM_ATTR_TYPE         data type index
 *              -    SC_MEM_ATTR_FUNC         function containing call
 *              -    SC_MEM_ATTR_FILE         file containing function
 *              -    SC_MEM_ATTR_LINE         line number in file
 *              -
 *              - Example:
 *              -
 *              -  p = SC_realloc_n(p, 10, sizeof(int),
 *              -                   SC_MEM_ATTR_ZERO_SPACE, 2,
 *              -                   SC_MEM_ATTR_FUNC, "my_func",
 *              -                   SC_MEM_ATTR_FILE, "my_file.c",
 *              -                   SC_MEM_ATTR_LINE, 384,
 *              -                   0);
 */

void *SC_realloc_n(void *p, long ni, long bpi, ...)
   {int is, ok;
    void *rv;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = -1;

    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    SC_VA_START(bpi);
    for (ok = TRUE; ok == TRUE; )
        {is = SC_VA_ARG(int);
         switch (is)
            {case SC_MEM_ATTR_PERMANENT :
                  opt.perm = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_NO_ACCOUNT :
                  opt.na = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_ZERO_SPACE :
                  opt.zsp = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_TYPE :
                  opt.typ = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_FUNC :
                  opt.where.pfunc = SC_VA_ARG(char *);
                  break;
             case SC_MEM_ATTR_FILE :
                  opt.where.pfile = SC_VA_ARG(char *);
                  break;
             case SC_MEM_ATTR_LINE :
                  opt.where.line = SC_VA_ARG(int);
                  break;
	     default :
                  ok = FALSE;
	          break;};};

    SC_VA_END;

    rv = _SC_realloc_n(p, ni, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_N - the complementary routine for _SC_alloc_n
 *            - free all the space including the counter
 *            - return TRUE if successful and FALSE otherwise
 */

int _SC_free_n(void *p, void *arg)
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

    _SC_mem_stats_acc(ph, 0L, nbp - ph->hdr_size);

    if ((zsp == 1) || (zsp == 3))
       _SC_prim_memset(space, nbp);
    else
       {_SC_deassign_block(ph, desc, NULL);};

    if (SC_gs.mm_debug)
       _SC_FREE((void *) space);
    else
       _SC_prim_free((void *) space, nbp, ph);

    ph->n_mem_blocks--;

    SC_LOCKOFF(SC_mm_lock);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_N - add a layer of control over the C level memory management
 *           - system to store extra information about the allocated spaces
 *           - options are:
 *           -
 *           -    SC_MEM_ATTR_NO_ACCOUNT   memory block not include in totals
 *           -    SC_MEM_ATTR_ZERO_SPACE   set zero_space behavior
 *           -    SC_MEM_ATTR_TYPE         data type index
 *           -    SC_MEM_ATTR_FUNC         function containing call
 *           -    SC_MEM_ATTR_FILE         file containing function
 *           -    SC_MEM_ATTR_LINE         line number in file
 *           -
 *           - Example:
 *           -
 *           -  ok = SC_free_n(p,
 *           -                 SC_MEM_ATTR_ZERO_SPACE, 1,
 *           -                 SC_MEM_ATTR_FUNC, "my_func",
 *           -                 SC_MEM_ATTR_FILE, "my_file.c",
 *           -                 SC_MEM_ATTR_LINE, 384,
 *           -                 0);
 */

int SC_free_n(void *p, ...)
   {int is, ok, rv;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = -1;

    opt.where.pfunc = NULL;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    SC_VA_START(p);
    for (ok = TRUE; ok == TRUE; )
        {is = SC_VA_ARG(int);
         switch (is)
            {case SC_MEM_ATTR_PERMANENT :
                  opt.perm = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_NO_ACCOUNT :
                  opt.na = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_ZERO_SPACE :
                  opt.zsp = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_TYPE :
                  opt.typ = SC_VA_ARG(int);
                  break;
             case SC_MEM_ATTR_FUNC :
                  opt.where.pfunc = SC_VA_ARG(char *);
                  break;
             case SC_MEM_ATTR_FILE :
                  opt.where.pfile = SC_VA_ARG(char *);
                  break;
             case SC_MEM_ATTR_LINE :
                  opt.where.line = SC_VA_ARG(int);
                  break;
	     default :
                  ok = FALSE;
	          break;};};

    SC_VA_END;

    rv = _SC_free_n(p, &opt);

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
 *                 -   5 : use calloc to get zeroed memory in _SC_prim_alloc
 *                 - return the original value
 *
 * #bind SC_zero_space_n fortran() scheme(set-zero-space-n!) python()
 */

int SC_zero_space_n(int flag, int tid)
   {int it, itn, itx, rv;
    SC_heap_des *ph;

    switch (tid)
       {case -2 :
	     itn = 0;
	     itx = SC_get_n_thread();
	     itx = max(itx, SC_n_threads);
	     itx = max(itx, 1);
	     break;
        case -1 :
	     itn = SC_current_thread();
	     itx = itn+1;
	     break;
	default :
	     itn = tid;
	     itx = itn+1;
	     break;};

    rv = 0;

    SC_LOCKON(SC_mm_lock);

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

/* SC_ZERO_ON_ALLOC_N - return TRUE iff zero_space is 1 or 2
 *                    - for specified thread/heap
 */

int SC_zero_on_alloc_n(int tid)
   {int it, itn, itx, rv;
    SC_heap_des *ph;

    switch (tid)
       {case -2 :
	     itn = 0;
	     itx = SC_get_n_thread();
	     itx = max(itx, 1);
	     break;
        case -1 :
	     itn = SC_current_thread();
	     itx = itn+1;
	     break;
	default :
	     itn = tid;
	     itx = itn+1;
	     break;};

    rv = TRUE;

    SC_LOCKON(SC_mm_lock);

    for (it = itn; it < itx; it++)
        {ph = _SC_get_heap(it);
	 if (ph != NULL)
	    rv &= ((ph->zero_space == 1) || (ph->zero_space == 2));};

    SC_LOCKOFF(SC_mm_lock);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_OVER_MARK - increment guard counter by N (negative to decrement)
 *                  - use system malloc when guard counter > 0
 */

int SC_mem_over_mark(int n)
   {int rv;

    rv = _SC_mf.sys;

    _SC_mf.sys += n;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
