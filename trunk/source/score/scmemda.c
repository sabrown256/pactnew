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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_W - wrapper for _SC_alloc_n */

void *_SC_alloc_w(long nitems, long bpi, char *name, int memfl, int zsp)
   {int prm, na;
    void *p;
    SC_mem_opt opt;

    prm = ((memfl & 1) != 0);
    na  = ((memfl & 2) != 0);

    opt.perm = prm;
    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;

    opt.where.func = name;
    opt.where.file = NULL;
    opt.where.line = -1;

    p = _SC_alloc_n(nitems, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NALLOC_W - wrapper for _SC_alloc_n */

void *_SC_nalloc_w(long nitems, long bpi, int memfl, int zsp,
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

    opt.where.func = fnc;
    opt.where.file = file;
    opt.where.line = line;

    p = _SC_alloc_n(nitems, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_W - wrapper for _SC_realloc_n */

void *_SC_realloc_w(void *p, long nitems, long bpi, int memfl, int zsp)
   {int prm, na;
    void *rv;
    SC_mem_opt opt;

    prm = ((memfl & 1) != 0);
    na  = ((memfl & 2) != 0);

    opt.perm = prm;
    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;

    opt.where.func = NULL;
    opt.where.file = NULL;
    opt.where.line = -1;

    rv = _SC_realloc_n(p, nitems, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_W - wrapper for _SC_free_n */

int _SC_free_w(void *p, int zsp)
   {int rv;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = -1;
    opt.zsp  = zsp;
    opt.typ  = -1;

    opt.where.func = NULL;
    opt.where.file = NULL;
    opt.where.line = -1;

    rv = _SC_free_n(p, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NALLOC_STD - use the C std library malloc */

static void *_SC_nalloc_std(long nitems, long bpi, int na, int zsp,
			    const char *fnc, const char *file, int line)
   {void *space;

    space = malloc(nitems*bpi);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_STD - use the C std library malloc */

static void *_SC_alloc_std(long nitems, long bpi, char *name, int na, int zsp)
   {void *space;

    space = malloc(nitems*bpi);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_STD - use the C std library realloc */

static void *_SC_realloc_std(void *p, long nitems, long bpi, int na, int zsp)
   {void *space;

    space = realloc(p, nitems*bpi);

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

/* _SC_NALLOC_CHK - wrap a check for a specific pointer value
 *                - around the _SC_alloc_n call
 *                - part of a usable API for memory debugging
 */

static void *_SC_nalloc_chk(long nitems, long bpi, int na, int zsp,
			    const char *fnc, const char *file, int line)
   {void *space;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;

    opt.where.func = fnc;
    opt.where.file = file;
    opt.where.line = line;

    space = _SC_alloc_n(nitems, bpi, &opt);

    if (space == _SC_ms.trap_ptr)
       raise(_SC_ms.trap_sig);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_CHK - wrap a check for a specific pointer value
 *               - around the _SC_alloc_n call
 *               - part of a usable API for memory debugging
 */

static void *_SC_alloc_chk(long nitems, long bpi, char *name, int na, int zsp)
   {void *space;
    SC_mem_opt opt;

    opt.perm = FALSE;
    opt.na  = na;
    opt.zsp = zsp;
    opt.typ = -1;

    opt.where.func = name;
    opt.where.file = NULL;
    opt.where.line = -1;

    space = _SC_alloc_n(nitems, bpi, &opt);

    if (space == _SC_ms.trap_ptr)
       raise(_SC_ms.trap_sig);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_CHK - wrap a check for a specific pointer value
 *                 - around the _SC_alloc_n call
 *                 - part of a usable API for memory debugging
 */

static void *_SC_realloc_chk(void *p, long nitems, long bpi, int na, int zsp)
   {void *space;

    if (p == _SC_ms.trap_ptr)
       raise(_SC_ms.trap_sig);

    space = _SC_realloc_w(p, nitems, bpi, na, zsp);

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

    rv = _SC_free_w(p, zsp);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                        MEMORY MANAGER COLLECTIONS                        */

/*--------------------------------------------------------------------------*/

/* SC_USE_SCORE_MM - use the SCORE memory manager */

SC_mem_fnc SC_use_score_mm(void)
   {SC_mem_fnc rv;

    rv = SC_gs.mm;

    SC_gs.mm.nalloc  = _SC_nalloc_w;
    SC_gs.mm.alloc   = _SC_alloc_w;
    SC_gs.mm.realloc = _SC_realloc_w;
    SC_gs.mm.free    = _SC_free_w;

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
	SC_gs.mm.free    = mf->free;};

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

void *SC_copy_item(void *in)
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

char *_SC_block_name(mem_descriptor *desc)
   {char *p, *rv;
    static char name[MAXLINE];

    rv = NULL;
    if (desc != NULL)
       {if (desc->where.file != NULL)
	   {p = strrchr(desc->where.file, '/');
	    if (p == NULL)
	       p = (char *) desc->where.file;
	    else
	       p++;
	    snprintf(name, MAXLINE, "%s(%s:%d)", desc->where.func, p, desc->where.line);}
	else if (desc->where.func != NULL)
	   SC_strncpy(name, MAXLINE, (char *) desc->where.func, -1);
	else
	   SC_strncpy(name, MAXLINE, "-none-", -1);

	rv = name;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PRINT_BLOCK_INFO - print out the descriptive info about a
 *                      - memory block
 */

void _SC_print_block_info(FILE *fp, SC_heap_des *ph, void *ptr, int flag)
   {int j, nc, nr, nf, acc;
    long nb;
    char bf[MAXLINE], c;
    char *name, *ps, *pc;
    mem_descriptor *desc;
    mem_header *space;

    space = (mem_header *) ptr;
    desc  = &space->block;
    nb    = desc->length;
    nr    = desc->ref_count;
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
		     
	acc  = ((flag & 8) || (desc->ref_count != UNCOLLECT));
	if ((name != NULL) && (acc == TRUE))
	   {if (sizeof(char *) == 4)
	       io_printf(fp, "   0x%012lx %9ld %3d\t%s",
			 space+1, nb, nr, name);
	    else
	       io_printf(fp, "   0x%012lx %9ld %3d\t%s",
			 space+1, nb, nr, name);

	    if ((strncmp(name, "char*:", 6) == 0) ||
		(desc->type == SC_STRING_I))
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

int SC_mem_info(void *p, long *pl, int *pt, int *pr, char **pn)
   {int rv, ty, rf;
    long ln;
    char *nm;
    mem_descriptor *desc;
    mem_header *space;
    static char *fr = "-- free block --";

    rv = FALSE;

    if (p != NULL)
       {space = ((mem_header *) p) - 1;
	desc  = &space->block;
	if (SCORE_BLOCK_P(desc))
	   {if (FREE_SCORE_BLOCK_P(desc))
	       {ln = 0L;
		ty = -1;
		rf = 0;
		nm = fr;}
	    else
	       {ln = desc->length;
		ty = desc->type;
		rf = desc->ref_count;
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

/* SC_MEM_TRACE - return the number of active chunks of memory managed
 *              - by the system
 *              - the given pointer must have been allocated by _SC_alloc_n
 *              - return -1 if the forward and backward counts differ
 *              - return -2 if a NULL pointer occurs in the chain
 *              - return -3 if the link count exceeds the number of blocks
 *              - return -4 if a corrupted header is detected
 *              -           also prints name of block
 *              - allocated
 *
 * #bind SC_mem_trace fortran() scheme(memory-trace) python()
 */

int SC_mem_trace(void)
   {int n_mf, n_mb, ret, ok;
    long i, n;
    SC_heap_des *ph;
    mem_header *space;
    mem_descriptor *desc;

    ph = _SC_tid_mm();

    SC_LOCKON(SC_mm_lock);

    ret = 0;
    if (ph->latest_block != NULL)
       {n    = ph->nx_mem_blocks + BLOCKS_UNIT_DELTA;
	n_mf = 1;
	n_mb = 1;

	ok = TRUE;
	for (space = ph->latest_block, i = 0L;
	     i < n;
	     space = space->block.next, n_mf++, i++)
	    {desc = &space->block;
	     if (!SCORE_BLOCK_P(desc))
	        {_SC_print_block_info(stdout, ph, (void *) space, FALSE);
		 ok = FALSE;
		 break;};

	     if ((desc->next == ph->latest_block) || (space == NULL))
	        break;};

	if (!ok)
	   ret = -4;

	else if ((i >= n) && (ph->nx_mem_blocks != 0))
	   ret = -3;

	else if (space == NULL)
	   ret = -2;

	else
	   {for (space = ph->latest_block, i = 0L;
		 i < n;
		 space = space->block.prev, n_mb++, i++)
	        if ((space == NULL) ||
		    (space->block.prev == ph->latest_block))
		   break;

	    if ((i >= n) && (ph->nx_mem_blocks != 0))
	       ret = -3;

	    else if (space == NULL)
	       ret = -2;

	    else if (n_mf != n_mb)
	       ret = -1;

	    else
	       ret = n_mf;};};

    SC_LOCKOFF(SC_mm_lock);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REG_MEM - register a piece of memory for the purpose
 *            - of memory accounting
 *
 * #bind SC_reg_mem fortran()
 */

int SC_reg_mem(void *p, long length, char *name)
   {int rv;
    SC_heap_des *ph;
    mem_descriptor *pd;
    haelem *hp;

    if (_SC.mem_table == NULL)
       _SC.mem_table = SC_make_hasharr(1001, FALSE, SC_HA_ADDR_KEY, 0);

    ph = _SC_tid_mm();

    pd = CPMAKE(mem_descriptor, 3);

    pd->heap      = ph;
    pd->ref_count = 1;
    pd->type      = 0;
    pd->length    = length;
    pd->prev      = NULL;
    pd->next      = (mem_header *) p;

    pd->where.func = strdup(name);
    pd->where.file = NULL;
    pd->where.line = -1;

    hp = SC_hasharr_install(_SC.mem_table, p, pd, "mem_descriptor", TRUE, TRUE);
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

int SC_dereg_mem(void *p)
   {int rv;
    long nb;
    mem_descriptor *pd;

    pd = SC_hasharr_def_lookup(_SC.mem_table, p);
    if (pd != NULL)
       {free((char *) pd->where.func);
	nb = pd->length;}
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

char *SC_mem_lookup(void *p)
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

/* DPRFREE - print the free memory lists */

void dprfree(void)
   {long i, j;
    SC_heap_des *ph;
    mem_descriptor *md;

    ph = _SC_tid_mm();

    io_printf(stdout, "Bin  Max  Blocks\n");
    for (j = 0L; j < _SC_ms.n_bins; j++)
        {io_printf(stdout, "%3ld %4ld ", j, SC_BIN_SIZE(j));
         for (md  = ph->free_list[j], i = 0L;
	      md != NULL;
	      md  = (mem_descriptor *) md->where.func, i++)
             {io_printf(stdout, " %lx", md);
	      fflush(stdout);};
	 io_printf(stdout, "\n");};

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
	 md = (mem_descriptor *) md->where.func)
        io_printf(stdout, "%8lx\n", md);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FLCHK - return the total number of free blocks */

static long _SC_flchk(void)
   {int ok, bad;
    long j, k, nf;
    SC_heap_des *ph;
    mem_descriptor *md, *hd;

    ph = _SC_tid_mm();

    nf  = 0L;
    bad = 0;
    for (j = 0L; j < _SC_ms.n_bins; j++)
        {hd = ph->free_list[j];
         for (k = 0L, md = hd;
	      md != NULL;
	      k++, md = (mem_descriptor *) md->where.func)
	     {ok = SC_pointer_ok(md);
	      if (!ok)
		 {io_printf(stdout,
			    "   Block: Head of free list %d,%ld corrupted\n",
			    ph->tid, j);

		  bad++;
		  break;};
		
	      ok = SC_pointer_ok((char *) md->where.func);
 	      if (ok &&
		  (md->ref_count == SC_MEM_MFA) &&
		  (md->type == SC_MEM_MFB))
	         nf++;

	      else
		 {io_printf(stdout,
			    "   Block: %12lx (corrupted free memory block ",
			    ((mem_header *) md + 1));
			
		  io_printf(stdout, "- %d,%ld,%ld)\n",
			    ph->tid, j, k);

		  bad++;
		  break;};};};

    if (bad > 0)
       nf = -1L;

    return(nf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RGCHK - return the total number of registered blocks */

static long _SC_rgchk(void)
   {int bad, ok;
    long i, nr;
    void *p;
    mem_descriptor *md;

    nr  = 0L;
    bad = 0;
    if (_SC.mem_table != NULL)
       {for (i = 0; SC_hasharr_next(_SC.mem_table, &i, NULL, NULL, (void **) &md); i++)
	    {p  = md->next;
	     ok = SC_pointer_ok(p);
	     if (ok)
	        nr++;
	     else
	        {io_printf(stdout,
			   "   Block: %12lx (corrupted registered memory block ",
			   p);

		 io_printf(stdout,	"- %ld)\n", i);

		 bad++;};};

	if (bad > 0)
	   nr = -1L;};

    return(nr);}

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
   {long nb, nt;

    nb = 0L;

    if (typ & 1)
       {nt = SC_mem_trace();
	if (nt < 0L)
	   nb = -1L;
	else
	   nb += nt;};

    if (typ & 2)
       {nt = _SC_flchk();
	if (nt < 0L)
	   nb = (nb < 0L) ? -3L : -2L;
	else if (nb >= 0L)
	   nb += nt;};

    if (typ & 4)
       {nt = _SC_rgchk();
	if (nt < 0L)
	   nb = (nb < 0L) ? -3L : -2L;
	else if (nb >= 0L)
	   nb += nt;};

    return(nb);}

/*--------------------------------------------------------------------------*/

/*                           MEMORY INFO ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* SC_IS_SCORE_PTR - return TRUE iff the given pointer P
 *                 - points to space allocated by SCORE
 *
 * #bind SC_is_score_ptr fortran() python()
 */

int SC_is_score_ptr(void *p)
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
 *           - with _SC_alloc_n
 *           - return -1L on error
 *
 * #bind SC_arrlen fortran() python()
 */

long SC_arrlen(void *p)
   {long nb, rv;
    mem_descriptor *desc;

    rv = -1L;

    if (SC_is_score_space(p, NULL, &desc))
       {nb = desc->length;
	rv = (nb < 0L) ? -1L : nb;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MARK - change the reference count by n
 *
 * #bind SC_mark fortran() python()
 */

int SC_mark(void *p, int n)
   {mem_descriptor *desc;

    if (SC_is_score_space(p, NULL, &desc))
       {if (desc->ref_count < UNCOLLECT)
           desc->ref_count += n;

        n = desc->ref_count;}
    else
       n = -1;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_COUNT - set the reference count of the given object */

int SC_set_count(void *p, int n)
   {mem_descriptor *desc;

    if (SC_is_score_space(p, NULL, &desc))
       desc->ref_count = n;
    else
       n = -1;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PERMANENT - set the reference count of the given object
 *              - to make it uncollectable
 *              - pass the pointer P through
 */

void *SC_permanent(void *p)
   {mem_descriptor *desc;

    if (SC_is_score_space(p, NULL, &desc))
       desc->ref_count = UNCOLLECT;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_ATTRS - set various attribute on the memory block P */

void *SC_mem_attrs(void *p, int attr)
   {mem_descriptor *desc;

    if (SC_is_score_space(p, NULL, &desc))

/* mark the block uncollectable */
       {if (attr & 1)
	   desc->ref_count = UNCOLLECT;

/* remove this block from the memory accounting
 * decrement the total bytes allocated by the size of the block
 * this is not the same as doing the allocation with NA == TRUE
 * the difference and maximum difference could be different
 */
	if (attr & 2)
	   {uint64_t a, f, nb;

	    SC_mem_statb(&a, &f, NULL, NULL);
	    nb = desc->length;
	    a -= nb;
	    SC_mem_statb_set(a, f);};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SAFE_TO_FREE - return TRUE iff the reference count of the space
 *                 - is 0 or 1 indicating that an active space has only
 *                 - a single reference
 */

int SC_safe_to_free(void *p)
   {int rc, ok;

    rc = SC_ref_count(p);
    ok = ((rc == 0) || (rc == 1));

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REF_COUNT - return the reference count of the given object */

int SC_ref_count(void *p)
   {int n;
    mem_descriptor *desc;

    n = -1;

    if (SC_is_score_space(p, NULL, &desc))
       n = desc->ref_count;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRTYPE - return the type index of the object */

int SC_arrtype(void *p, int type)
   {int n;
    mem_descriptor *desc;

    n = -1;

    if (SC_is_score_space(p, NULL, &desc))
       {if (type > 0)
	   desc->type = type;

	n = desc->type;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_ARRNAME - return the name associated with the object P */

char *SC_arrname(void *p)
   {char *rv;
    mem_descriptor *desc;

    rv = NULL;
    if (SC_is_score_space(p, NULL, &desc))
       rv = _SC_block_name(desc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_ARRTYPE - set the type index of the object
 *                - return the original type
 */

int SC_set_arrtype(void *p, int type)
   {int n;
    mem_descriptor *desc;

    n = -1;

    if (SC_is_score_space(p, NULL, &desc))
       {n          = desc->type;
	desc->type = type;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRBLK - print info about memory block */

void dprblk(void *p)
   {mem_header *space;
    mem_descriptor *desc;

    if (p != NULL)
       {space = ((mem_header *) p) - 1;
	desc  = &space->block;
	if (!SCORE_BLOCK_P(desc))
           printf("%p is not a SCORE allocated block\n", p);
	else
	   {printf("Address: %p\n", p);
            if (FREE_SCORE_BLOCK_P(desc) == TRUE)
               printf("Free Block\n");
	    else
	       {printf("Name: %s\n",       _SC_block_name(desc));
		printf("Length: %ld\n",    desc->length);
		printf("Type: %d\n",       desc->type);
		printf("References: %d\n", desc->ref_count);};

	    printf("Next Block: %p\n",     desc->next);
	    printf("Previous Block: %p\n", desc->prev);
	    printf("Heap: %d\n",           desc->heap->tid);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
