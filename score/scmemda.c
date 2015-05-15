/*
 * SCMEMDA.C - memory management diagnostic functions part A
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h"
#include "scope_mem.h"

typedef struct s_chkdes chkdes;

struct s_chkdes
   {int na;
    int nf;
    int nr;
    int nab;
    int nfb;
    int nrb;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NALLOC_STD - use the C std library malloc */

static void *_SC_nalloc_std(long ni, long bpi, int na, int zsp,
			    const char *fnc, const char *file, int line)
   {void *space;

    space = malloc(ni*bpi);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_STD - use the C std library malloc */

static void *_SC_alloc_std(long ni, long bpi, char *name, int na, int zsp)
   {void *space;

    space = malloc(ni*bpi);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_STD - use the C std library realloc */

static void *_SC_realloc_std(void *p, long ni, long bpi, int na, int zsp)
   {void *space;

    space = realloc(p, ni*bpi);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_STD - use the C std library free */

static int _SC_free_std(void *p, int zsp)
   {

    free(p);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRLEN_STD - use the C std library free */


static int64_t _SC_arrlen_std(const void *p)
   {int64_t rv;

    rv = -1;

#if defined(LINUX) || defined(CYGWIN)
    rv = malloc_usable_size((void *) p);
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NALLOC_CHK - wrap a check for a specific pointer value
 *                - around the _SC_ALLOC_N call
 *                - part of a usable API for memory debugging
 */

static void *_SC_nalloc_chk(long ni, long bpi, int na, int zsp,
			    const char *fnc, const char *file, int line)
   {void *space;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;

    opt.where.pfunc = fnc;
    opt.where.pfile = file;
    opt.where.line  = line;

    space = _SC_ALLOC_N(ni, bpi, &opt);

    if (space == _SC_ms.trap_ptr)
       raise(_SC_ms.trap_sig);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_CHK - wrap a check for a specific pointer value
 *               - around the _SC_ALLOC_N call
 *               - part of a usable API for memory debugging
 */

static void *_SC_alloc_chk(long ni, long bpi, char *name, int na, int zsp)
   {void *space;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na  = na;
    opt.zsp = zsp;
    opt.typ = -1;

    opt.where.pfunc = name;
    opt.where.pfile = NULL;
    opt.where.line  = -1;

    space = _SC_ALLOC_N(ni, bpi, &opt);

    if (space == _SC_ms.trap_ptr)
       raise(_SC_ms.trap_sig);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_CHK - wrap a check for a specific pointer value
 *                 - around the _SC_realloc_w call
 *                 - part of a usable API for memory debugging
 */

static void *_SC_realloc_chk(void *p, long ni, long bpi, int na, int zsp)
   {void *space;

    if (p == _SC_ms.trap_ptr)
       raise(_SC_ms.trap_sig);

    space = SC_gs.mm.realloc(p, ni, bpi, na, zsp);

    if (space == _SC_ms.trap_ptr)
       raise(_SC_ms.trap_sig);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_CHK - wrap a check for a specific pointer value
 *              - around an CFREE
 *              - part of a usable API for memory debugging
 */

static int _SC_free_chk(void *p, int zsp)
   {int rv;

    if (p == _SC_ms.trap_ptr)
       raise(_SC_ms.trap_sig);

    rv = SC_gs.mm.free(p, zsp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ARRLEN_CHK - wrap a check for a specific pointer value
 *                - around an SC_arrlen
 *                - part of a usable API for memory debugging
 */

static int64_t _SC_arrlen_chk(const void *p)
   {int64_t rv;

    if (p == _SC_ms.trap_ptr)
       raise(_SC_ms.trap_sig);

    rv = SC_gs.mm.arrlen(p);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                        MEMORY MANAGER COLLECTIONS                        */

/*--------------------------------------------------------------------------*/

/* SC_USE_SCORE_MM - use the SCORE memory manager */

SC_mem_fnc SC_use_score_mm(void)
   {SC_mem_fnc rv;

#ifdef USE_FULL_MM
    rv = SC_use_full_mm();
#else
    rv = SC_use_reduced_mm();
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_USE_C_MM - use the C memory manager */

SC_mem_fnc SC_use_c_mm(void)
   {SC_mem_fnc rv;

    rv = SC_gs.mm;

    SC_gs.mm.nalloc  = _SC_nalloc_std;
    SC_gs.mm.alloc   = _SC_alloc_std;
    SC_gs.mm.realloc = _SC_realloc_std;
    SC_gs.mm.free    = _SC_free_std;
    SC_gs.mm.arrlen  = _SC_arrlen_std;

/* GOTCHA: add these */
    SC_gs.mm.alloc_n   = NULL;
    SC_gs.mm.realloc_n = NULL;
    SC_gs.mm.free_n    = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_USE_MM - use the supplied set of memory manager functions */

SC_mem_fnc SC_use_mm(SC_mem_fnc *mf)
   {SC_mem_fnc rv;

    rv = SC_gs.mm;

    if (mf != NULL)
       {SC_gs.mm.nalloc  = mf->nalloc;
	SC_gs.mm.alloc   = mf->alloc;
	SC_gs.mm.realloc = mf->realloc;
	SC_gs.mm.free    = mf->free;
	SC_gs.mm.arrlen  = mf->arrlen;

	SC_gs.mm.alloc_n   = mf->alloc_n;
	SC_gs.mm.realloc_n = mf->realloc_n;
	SC_gs.mm.free_n    = mf->free_n;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TRAP_POINTER - arrange for the memory manager to raise signal SIG when
 *                 - address P is allocated, reallocated, or freed
 */

SC_mem_fnc SC_trap_pointer(void *p, int sig)
   {SC_mem_fnc rv;

    rv = SC_gs.mm;

    _SC_ms.trap_ptr = p;
    _SC_ms.trap_sig = sig;

    SC_gs.mm.nalloc  = _SC_nalloc_chk;
    SC_gs.mm.alloc   = _SC_alloc_chk;
    SC_gs.mm.realloc = _SC_realloc_chk;
    SC_gs.mm.free    = _SC_free_chk;
    SC_gs.mm.arrlen  = _SC_arrlen_chk;

/* GOTCHA: add these */
    SC_gs.mm.alloc_n   = NULL;
    SC_gs.mm.realloc_n = NULL;
    SC_gs.mm.free_n    = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_UNTRAP_POINTER - undo the trap on a pointer setup
 *                   - by a call to SC_trap_pointer
 */

void SC_untrap_pointer(void *p)
   {

    _SC_ms.trap_ptr = NULL;
    _SC_ms.trap_sig = -1;

    SC_use_score_mm();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_COPY_ITEM - copy a bit of memory */

void *SC_copy_item(const void *in)
   {int len;
    void *out;

    if (in == NULL)
       return NULL;

    len = SC_arrlen(in);
    if (len == 0)
       return NULL;

    out = CMAKE_N(char, len);
    if (out == NULL)
       return NULL;

    memcpy(out, in, len);

    return(out);}

/*--------------------------------------------------------------------------*/

/*                            HEAP ANALYZER ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* _SC_BLOCK_NAME - return the name associated with the object P */

char *_SC_block_name(const mem_descriptor *desc)
   {char *rv;
    static char name[MAXLINE];

    rv = NULL;
    if (desc != NULL)
       {if (FREE_SCORE_BLOCK_P(desc))
	   {snprintf(name, MAXLINE, "-- free --");
	    rv = name;}
        else
	   rv = _SC_format_loc(name, MAXLINE, &desc->desc.where, TRUE, TRUE);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRINT_BLOCK_INFO - print out the descriptive info about a
 *                      - memory block
 */

void _SC_print_block_info(FILE *fp, SC_heap_des *ph,
			  const void *ptr, int flag)
   {int j, nc, nr, nf, acc;
    long nb;
    char bf[MAXLINE], c;
    char *name, *ps, *pc;
    mem_descriptor *desc;
    mem_header *space;
    mem_inf *info;

    space = (mem_header *) ptr;
    desc  = &space->block;
    info  = &desc->desc.info;
    nb    = desc->length;
    nr    = info->ref_count;
    name  = _SC_block_name(desc);
    if (name == NULL)
       {if (flag == 0)
	   io_printf(fp, "   0x%012lx %9ld\t(no name)\n",
		     space+1, nb);}

    else if (!SC_pointer_ok(name))
       {io_printf(fp, "   Block: 0x%012lx (corrupted active memory block ",
		  space+1);
	io_printf(fp, "- %d\n", ph->tid);}

    else
       {if (FF_NAME(desc))
	   {nc = strlen(name);
	    ps = strchr(name, ' ');
	    if (ps != NULL)
	       {nf = ps - name;
		nc = min(nc, nf);};

	    SC_strncpy(bf, MAXLINE, name, nc);
	    name = bf;};
		     
	acc  = ((flag & 8) || (info->ref_count != UNCOLLECT));
	if ((name != NULL) && (acc == TRUE))
	   {if (sizeof(char *) == 4)
	       io_printf(fp, "   0x%012lx %9ld %3d\t%s",
			 space+1, nb, nr, name);
	    else
	       io_printf(fp, "   0x%012lx %9ld %3d\t%s",
			 space+1, nb, nr, name);

	    if ((strncmp(name, "char*:", 6) == 0) ||
		(info->type == G_STRING_I))
	       {io_printf(fp, " = \"");
		pc = (char *) (space + 1);
		for (j = 0; j < nb; j++)
		    {c = *pc++;
		     if (SC_is_print_char(c, 0))
		        io_printf(fp, "%c", c);
		     else
		        io_printf(fp, "\\%03o", c);};

		io_printf(fp, "\"");};

	    io_printf(fp, "\n");};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_INFO - return the mem_descriptor info for the given pointer
 *             -    PL  byte length
 *             -    PT  data type index
 *             -    PR  reference count
 *             -    PN  allocated name
 *             - return TRUE iff P is valid
 *
 * #bind SC_mem_info fortran() python()
 */

int SC_mem_info(const void *p, long *pl, int *pt, int *pr, char **pn)
   {int rv, ty, rf;
    long ln;
    char *nm;
    mem_descriptor *desc;
    mem_header *space;
    mem_inf *info;
    static char *fr = "-- free block --";

    rv = FALSE;

    if (p != NULL)
       {space = ((mem_header *) p) - 1;
	desc  = &space->block;
	info  = &desc->desc.info;
	if (SCORE_BLOCK_P(desc))
	   {if (FREE_SCORE_BLOCK_P(desc))
	       {ln = 0L;
		ty = -1;
		rf = 0;
		nm = fr;}
	    else
	       {ln = desc->length;
		ty = info->type;
		rf = info->ref_count;
		nm = _SC_block_name(desc);};
	    
	    if (pl != NULL)
	       *pl = ln;

	    if (pt != NULL)
	       *pt = ty;

	    if (pr != NULL)
	       *pr = rf;

	    if (pn != NULL)
	       *pn = nm;

	    rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REG_MEM - register a piece of memory for the purpose
 *            - of memory accounting
 *
 * #bind SC_reg_mem fortran()
 */

int SC_reg_mem(const void *p, long length, const char *name)
   {int rv;
    SC_heap_des *ph;
    mem_descriptor *desc;
    mem_inf *info;
    haelem *hp;

    if (_SC.mem_table == NULL)
       _SC.mem_table = SC_make_hasharr(1001, FALSE, SC_HA_ADDR_KEY, 0);

    ph = _SC_tid_mm();

    desc = CPMAKE(mem_descriptor, 3);
    info = &desc->desc.info;

    desc->heap      = ph;
    desc->length    = length;
    desc->prev      = NULL;
    desc->next      = (mem_header *) p;

    info->pfunc     = strdup(name);
    info->pfile     = NULL;
    info->line      = -1;
    info->ref_count = 1;
    info->type      = 0;

    SC_SET_BLOCK_ID(desc, SC_MEM_ID);

    hp = SC_hasharr_install(_SC.mem_table, p, desc, "mem_descriptor", 3, -1);
    if (hp != NULL)
       {SC_mem_stats_acc(length, 0L);
	rv = TRUE;}
    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_DEREG_MEM - deregister a piece of memory for the purpose
 *              - of memory accounting
 *
 * #bind SC_dereg_mem fortran()
 */

int SC_dereg_mem(const void *p)
   {int rv;
    long nb;
    mem_descriptor *desc;

    desc = SC_hasharr_def_lookup(_SC.mem_table, p);
    if (desc != NULL)
       {free((char *) desc->desc.where.pfunc);
	nb = desc->length;}
    else
       nb = 0;

    rv = SC_hasharr_remove(_SC.mem_table, p);
    if (rv == TRUE)
       SC_mem_stats_acc(0L, nb);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_REG_MEM_TABLE - release the registered memory table
 *                       - intended for cleanup at end of session
 */

void SC_free_reg_mem_table(void)
   {

    SC_free_hasharr(_SC.mem_table, NULL, NULL);

    _SC.mem_table = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_LOOKUP - given a pointer return the name of the block */

char *SC_mem_lookup(const void *p)
   {int i, n;
    char *name;
    void *ps;
    SC_heap_des *ph;
    mem_header *space;
    mem_descriptor *desc;

    ph = _SC_tid_mm();

    SC_LOCKON(SC_mm_lock);

    name  = NULL;
    space = ph->latest_block;
    if (space != NULL)
       {n = ph->nx_mem_blocks + BLOCKS_UNIT_DELTA;

	for (i = 0; (i < n) && (space != NULL); i++)
	    {ps = (void *) (space + 1);
	     if (ps == p)
	        {desc = &space->block;
		 name = _SC_block_name(desc);
		 break;};

	     space = space->block.next;};};

    SC_LOCKOFF(SC_mm_lock);

    return(name);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_CHK_BLOCK - check a memory block for correctness */

static int _SC_mem_chk_block(SC_heap_des *ph, mem_descriptor *desc,
			     mem_kind wh, void *a, long i, long j)
   {int ok;
    char *p;
    chkdes *st;
    mem_inf *info;

    ok = TRUE;
    st = (chkdes *) a;

    info = &desc->desc.info;

    switch (wh)
       {case MEM_BLOCK_ACTIVE:
	     if (SCORE_BLOCK_P(desc))
	        st->na++;
	     else
	        {_SC_print_block_info(stdout, ph, (void *) desc, FALSE);
		 st->nab++;
	         ok = FALSE;};
	     break;

	case MEM_BLOCK_FREE:
 	     if ((SC_pointer_ok((char *) desc) == TRUE) &&
		 (info->ref_count == SC_MEM_MFA) && (info->type == SC_MEM_MFB))
	        st->nf++;

	     else
	        {io_printf(stdout,
			   "   Block: %12lx (corrupted free memory block - %d,%ld,%ld)\n",
			   ((mem_header *) desc + 1), ph->tid, i, j);
		 st->nfb++;
	         ok = FALSE;};
	     break;

	case MEM_BLOCK_REG:
	     p = (char *) desc->next;
	     if (SC_pointer_ok(p) == TRUE)
	        st->nr++;
	     else
	        {io_printf(stdout,
			   "   Block: %12lx (corrupted registered memory block ",
			   p);

		 io_printf(stdout,	"- %ld)\n", i);
		 st->nrb++;};
	     break;

	default:
	     break;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_CHK - check out all aspects of managed memory
 *            - if bit #1 of TYP is 1 include the allocated memory
 *            - if bit #2 of TYP is 1 include the freed memory
 *            - if bit #3 of TYP is 1 include the registered memory
 *            - return values
 *            -   > 0  total blocks in requested memory pools
 *            -  -1    corrupt active pool
 *            -  -2    corrupt free pool
 *            -  -3    both pools corrupt
 *
 * #bind SC_mem_chk fortran() scheme(memory-check) python()
 */

long SC_mem_chk(int typ)
   {long nb;
    chkdes st;

    nb = 0L;

    SC_mem_over_mark(1);

    st.na  = 0;
    st.nf  = 0;
    st.nr  = 0;
    st.nab = 0;
    st.nfb = 0;
    st.nrb = 0;

    SC_mem_map_f(typ, _SC_mem_chk_block, &st);

    if (typ & 1)
       {if (st.nab > 0)
	   nb = -1L;
	else
	   nb += st.na;};

    if (typ & 2)
       {if (st.nfb > 0)
	   nb = (nb < 0L) ? -3L : -2L;
	else if (nb >= 0L)
	   nb += st.nf;};

    if (typ & 4)
       {if (st.nrb > 0)
	   nb = (nb < 0L) ? -3L : -2L;
	else if (nb >= 0L)
	   nb += st.nr;};

    SC_mem_over_mark(-1);

    return(nb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_TRACE - return the number of active chunks of memory managed
 *              - by the system
 *              - the given pointer must have been allocated by _SC_ALLOC_N
 *              - return -1 if the forward and backward counts differ
 *              - return -2 if a NULL pointer occurs in the chain
 *              - return -3 if the link count exceeds the number of blocks
 *              - return -4 if a corrupted header is detected
 *              -           also prints name of block allocated
 *
 * #bind SC_mem_trace fortran() scheme(memory-trace) python()
 */

int SC_mem_trace(void)
   {int ret;

    ret = SC_mem_chk(1);

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                           MEMORY INFO ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* SC_IS_SCORE_PTR - return TRUE iff the given pointer P
 *                 - points to space allocated by SCORE
 *
 * #bind SC_is_score_ptr fortran() python()
 */

int SC_is_score_ptr(const void *p)
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

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRLEN - return the length of an array which was allocated
 *           - with _SC_ALLOC_N
 *           - return -1L on error
 *
 * #bind SC_arrlen fortran() python()
 */

long SC_arrlen(const void *p)
   {long rv;

    if (SC_gs.mm.arrlen != NULL)
       rv = SC_gs.mm.arrlen(p);
    else
       rv = -1L;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MARK - change the reference count by n
 *
 * #bind SC_mark fortran() python()
 */

int SC_mark(const void *p, int n)
   {mem_descriptor *desc;
    mem_inf *info;

    if (SC_is_active_space(p, NULL, &desc))
       {info = &desc->desc.info;

	if (info->ref_count < UNCOLLECT)
           info->ref_count += n;

        n = info->ref_count;}
    else
       n = -1;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_COUNT - set the reference count of the given object */

int SC_set_count(const void *p, int n)
   {mem_descriptor *desc;
    mem_inf *info;

    if (SC_is_active_space(p, NULL, &desc))
       {info = &desc->desc.info;
	info->ref_count = n;}
    else
       n = -1;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PERMANENT - set the reference count of the given object
 *              - to make it uncollectable
 *              - pass the pointer P through
 */

const void *SC_permanent(const void *p)
   {mem_descriptor *desc;
    mem_inf *info;

    if (SC_is_active_space(p, NULL, &desc))
       {info = &desc->desc.info;
	info->ref_count = UNCOLLECT;};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_ATTRS - set various attribute on the memory block P */

const void *SC_mem_attrs(const void *p, int attr)
   {mem_descriptor *desc;
    mem_inf *info;

    if (SC_is_active_space(p, NULL, &desc))
       {info = &desc->desc.info;

/* mark the block uncollectable */
	if (attr & 1)
	   info->ref_count = UNCOLLECT;

/* remove this block from the memory accounting
 * decrement the total bytes allocated by the size of the block
 * this is not the same as doing the allocation with NA == TRUE
 * the difference and maximum difference could be different
 */
	if (attr & 2)
	   {u_int64_t a, f, nb;

	    SC_mem_statb(&a, &f, NULL, NULL);
	    nb = desc->length;
	    a -= nb;
	    SC_mem_stats_set(a, f);};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SAFE_TO_FREE - return TRUE iff the reference count of the space
 *                 - is 0 or 1 indicating that an active space has only
 *                 - a single reference
 */

int SC_safe_to_free(const void *p)
   {int rc, ok;

    rc = SC_ref_count(p);
    ok = ((rc == 0) || (rc == 1));

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REF_COUNT - return the reference count of the given object */

int SC_ref_count(const void *p)
   {int n;
    mem_descriptor *desc;
    mem_inf *info;

    n = -1;

    if (SC_is_active_space(p, NULL, &desc))
       {info = &desc->desc.info;
	n = info->ref_count;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRTYPE - return the type index of the object */

int SC_arrtype(const void *p, int type)
   {int n;
    mem_descriptor *desc;
    mem_inf *info;

    n = -1;

    if (SC_is_active_space(p, NULL, &desc))
       {info = &desc->desc.info;
	if (type > 0)
	   info->type = type;

	n = info->type;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRNAME - return the name associated with the object P */

char *SC_arrname(const void *p)
   {char *rv;
    mem_descriptor *desc;

    rv = NULL;
    if (SC_is_active_space(p, NULL, &desc))
       rv = _SC_block_name(desc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_ARRTYPE - set the type index of the object
 *                - return the original type
 */

int SC_set_arrtype(const void *p, int type)
   {int n;
    mem_descriptor *desc;
    mem_inf *info;

    n = -1;

    if (SC_is_active_space(p, NULL, &desc))
       {info = &desc->desc.info;
	n          = info->type;
	info->type = type;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRBLK - print info about memory block */

void dprblk(void *p)
   {mem_header *space;
    mem_descriptor *desc;
    mem_inf *info;

    if (p != NULL)
       {space = ((mem_header *) p) - 1;
	desc  = &space->block;
	info  = &desc->desc.info;
	if (!SCORE_BLOCK_P(desc))
           printf("%p is not a SCORE allocated block\n", p);
	else
	   {printf("Address: %p\n", p);
            if (FREE_SCORE_BLOCK_P(desc) == TRUE)
               printf("Free Block\n");
	    else
	       {printf("Name: %s\n",       _SC_block_name(desc));
		printf("Length: %ld\n",    desc->length);
		printf("Type: %d\n",       info->type);
		printf("References: %d\n", info->ref_count);};

	    printf("Next Block: %p\n",     desc->next);
	    printf("Previous Block: %p\n", desc->prev);
	    printf("Heap: %d\n",           desc->heap->tid);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
