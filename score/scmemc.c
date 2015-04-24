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

SC_scope_mem
 _SC_ms = { -1, NULL, 0, 0, 0, 0L, 0L, NULL,
            SC_LOCK_INIT_STATE, SC_LOCK_INIT_STATE };

SC_memfncs
  _SC_mf = { FALSE, NULL, NULL, NULL };

/*--------------------------------------------------------------------------*/

/*                     MEMORY STATISTICS ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SC_MEM_STATS_ACC - change the memory usage by A and F
 *                   - takes pthread id as argument
 */

void _SC_mem_stats_acc(SC_heap_des *ph, int64_t a, int64_t f)
   {int64_t d;

    ph->sp_alloc += a;
    ph->sp_free  += f;

    d = ph->sp_alloc - ph->sp_free;

    ph->sp_diff = max(d, 0);
    ph->sp_max  = max(ph->sp_max, ph->sp_diff);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATB_SET - set the memory usage to A and F */

void SC_mem_statb_set(u_int64_t a, u_int64_t f)
   {int64_t d;
    SC_heap_des *ph;

    ph = _SC_tid_mm();

    ph->sp_alloc = a;
    ph->sp_free  = f;

    d = ph->sp_alloc - ph->sp_free;

    ph->sp_diff = max(d, 0);
    ph->sp_max  = max(ph->sp_max, ph->sp_diff);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATS_SET - set the memory usage to A and F */

void SC_mem_stats_set(int64_t a, int64_t f)
   {int64_t d;
    SC_heap_des *ph;

    ph = _SC_tid_mm();

    ph->sp_alloc = a;
    ph->sp_free  = f;

    d = ph->sp_alloc - ph->sp_free;

    ph->sp_diff = max(d, 0);
    ph->sp_max  = max(ph->sp_max, ph->sp_diff);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATS_ACC - change the memory usage by A and F */

void SC_mem_stats_acc(int64_t a, int64_t f)
   {SC_heap_des *ph;

    ph = _SC_tid_mm();

    _SC_mem_stats_acc(ph, a, f);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_STATB - return memory usage statistics */

void SC_mem_statb(u_int64_t *al, u_int64_t *fr, u_int64_t *df, u_int64_t *mx)
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

/* SC_MEM_STATS - return memory usage statistics
 *
 * #bind SC_mem_stats fortran() scheme(memory-usage) python()
 */

void SC_mem_stats(int64_t *al ARG([*],out), int64_t *fr ARG([*],out),
		  int64_t *df ARG([*],out), int64_t *mx ARG([*],out))
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

/*                           MEMORY HEAP ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SC_MEM_EXHAUSTED - if CND is TRUE then system memory is exhausted
 *                   - and we must break the cardinal rules:
 *                   -    1) print an error message
 *                   -    2) exit the application
 *                   - attempts at graceful shutdown will likely
 *                   - fail because printf will not work if there
 *                   - is no memory available
 */

void _SC_mem_exhausted(int cnd)
   {char *msg = "\nFATAL - MEMORY EXHAUSTED, NO RECOVERY POSSIBLE - EXITING\n\n";

    if (cnd == TRUE)
       {write(2, msg, strlen(msg));
	exit(80);};

    return;}

/*--------------------------------------------------------------------------*/
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

    ONCE_SAFE(_SC_ts.init_emu == TRUE, &SC_mc_lock)

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

       _SC_mem_exhausted(_SC_ms.bins == NULL);

       _SC_ms.bins[0] = 8;

/* fill the linear region */
       for (n = 1L; _SC_ms.mem_align_size*n <= mxl; n++)
	   _SC_ms.bins[n-1] = (n << _SC_ms.mem_align_expt);

       n = max(n, 2);

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

/* _SC_ASSIGN_BLOCK - set the info in SPACE that is needed for
 *                  - a functioning active memory block
 */

void _SC_assign_block(SC_heap_des *ph, mem_header *space, long nb,
		      const char *func, const char *file, int line)
   {mem_descriptor *desc;
    mem_header *prev, *next, *lb;
    mem_inf *info;

    desc = &space->block;
    info = &desc->desc.info;

    desc->heap        = ph;
    desc->length      = nb;

    info->pfunc     = func;
    info->pfile     = file;
    info->line      = line;
    info->ref_count = 0;
    info->type      = 0;

    SC_SET_BLOCK_ID(desc, SC_MEM_ID);

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

/* _SC_DEASSIGN_BLOCK - reset the block DESC to free state */

void _SC_deassign_block(SC_heap_des *ph, mem_descriptor *desc,
			void *addr)
   {mem_inf *info;

    info = &desc->desc.info;

    desc->prev   = NULL;
    desc->next   = NULL;
    desc->heap   = ph;
    desc->length = 0L;

    info->pfunc       = (char *) addr;
    info->pfile       = NULL;
    info->line        = -1;
    info->initialized = FALSE;
    info->ref_count   = SC_MEM_MFA;
    info->type        = SC_MEM_MFB;

    SC_SET_BLOCK_ID(desc, SC_MEM_ID);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_UNASSIGN_BLOCK - splice a block DESC out of an active heap PH */

void _SC_unassign_block(SC_heap_des *ph, mem_header *space)
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

    ONCE_SAFE(_SC_ts.init_emu == TRUE, &SC_mm_lock)
       SC_configure_mm(128L, 4096L, 4096L, 1.1);
    END_SAFE;

    _SC_thread_error(id, "initializing heap");

    nb  = _SC_ms.n_bins*sizeof(mem_descriptor *);
    lst = (mem_descriptor **) malloc(nb);

    _SC_mem_exhausted(lst == NULL);

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

/* SC_IS_SCORE_SPACE - return TRUE iff the given pointer P
 *                   - points to space allocated by SCORE
 */

int SC_is_score_space(const void *p, mem_header **psp, mem_descriptor **pds)
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

/* SC_IS_FREE_SPACE - return TRUE iff the given pointer P
 *                  - points to space allocated by SCORE which is free
 */

int SC_is_free_space(const void *p, mem_header **psp, mem_descriptor **pds)
   {int ok;
    mem_header *space;
    mem_descriptor *desc;

    space = NULL;
    desc  = NULL;
    ok    = FALSE;
    if (p != NULL)
       {space = ((mem_header *) p) - 1;
	desc  = &space->block;
	ok    = (SCORE_BLOCK_P(desc) && FREE_SCORE_BLOCK_P(desc));};

    if (psp != NULL)
       *psp = space;

    if (pds != NULL)
       *pds = desc;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_IS_ACTIVE_SPACE - return TRUE iff the given pointer P
 *                    - points to space allocated by SCORE which is active
 */

int SC_is_active_space(const void *p, mem_header **psp, mem_descriptor **pds)
   {int ok;
    mem_header *space;
    mem_descriptor *desc;

    space = NULL;
    desc  = NULL;
    ok    = FALSE;
    if (p != NULL)
       {space = ((mem_header *) p) - 1;
	desc  = &space->block;
	ok    = (SCORE_BLOCK_P(desc) && !FREE_SCORE_BLOCK_P(desc));};

    if (psp != NULL)
       *psp = space;

    if (pds != NULL)
       *pds = desc;

    return(ok);}

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

    rv = _SC_ALLOC_N(ni, bpi, &opt);

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

    rv = _SC_REALLOC_N(p, ni, bpi, &opt);

    return(rv);}

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

    rv = _SC_FREE_N(p, &opt);

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

/* SC_MEM_TRIM - hand memory pages back to system
 *             - return TRUE if memory is returned to the system
 */

int SC_mem_trim(size_t pad)
   {int rv;

    rv = FALSE;

#if defined(LINUX) || defined(CYGWIN)
    SC_LOCKON(SC_mm_lock);

    rv = malloc_trim(pad);

    SC_LOCKOFF(SC_mm_lock);
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
