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

/* SC_ALLOC - add a layer of control over the C level memory management
 *          - system to store the byte length of allocated spaces
 *          - a space EXTRA_WORD_SIZE greater than requested is allocated
 *          - the length in bytes is written into the first EXTRA_WORD_SIZE
 *          - bytes with a 4 bit marker in the high bits and a pointer to the
 *          - next byte is returned
 *          - if the maximum size is exceeded a NULL pointer is returned
 */

void *SC_alloc(long nitems, long bpi, char *name)
   {void *rv;
    SC_mem_opt opt;

    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.fnc  = name;
    opt.file = NULL;
    opt.line = -1;

    rv = SC_alloc_nzt(nitems, bpi, &opt);

    return(rv);}

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
    opt.zsp = -1;
    opt.typ = -1;
    opt.fnc  = name;
    opt.file = NULL;
    opt.line = -1;

    p = SC_alloc_nzt(nitems, bpi, &opt);

    return(p);}

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
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.fnc  = fnc;
    opt.file = file;
    opt.line = line;

    p = SC_alloc_nzt(nitems, bpi, &opt);

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REALLOC - add a layer of control over the C level memory management
 *            - system to store the byte length of allocated spaces
 *            - a space EXTRA_WORD_SIZE greater than requested is reallocated
 *            - the length in bytes is written into the first EXTRA_WORD_SIZE
 *            - bytes with a 4 bit marker in the high bits and a pointer to
 *            - the next byte is returned
 *            - if the maximum size implied by the EXTRA_WORD_SIZE - 4 is
 *            - exceeded a NULL pointer is returned
 */

void *SC_realloc(void *p, long nitems, long bpi)
   {void *rv;
    SC_mem_opt opt;

    opt.na   = FALSE;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.fnc  = NULL;
    opt.file = NULL;
    opt.line = -1;

    rv = SC_realloc_nzt(p, nitems, bpi, &opt);

    return(rv);}

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
    SC_mem_opt opt;

    opt.na   = na;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.fnc  = NULL;
    opt.file = NULL;
    opt.line = -1;

    rv = SC_realloc_nzt(p, nitems, bpi, &opt);

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
   {void *rv;
    SC_mem_opt opt;

    opt.na   = na;
    opt.zsp  = zsp;
    opt.typ  = -1;
    opt.fnc  = NULL;
    opt.file = NULL;
    opt.line = -1;

    rv = SC_realloc_nzt(p, nitems, bpi, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE - the complementary routine for SC_alloc
 *         - free all the space including the counter
 *         - return TRUE if successful and FALSE otherwise
 */

int SC_free(void *p)
   {int rv;
    SC_mem_opt opt;

    opt.na   = -1;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.fnc  = NULL;
    opt.file = NULL;
    opt.line = -1;

    rv = SC_free_nzt(p, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_FREE_Z - the complementary routine for SC_alloc
 *           - free all the space including the counter
 *           - return TRUE if successful and FALSE otherwise
 */

int SC_free_z(void *p, int zsp)
   {int rv;
    SC_mem_opt opt;

    opt.na   = -1;
    opt.zsp  = zsp;
    opt.typ  = -1;
    opt.fnc  = NULL;
    opt.file = NULL;
    opt.line = -1;

    rv = SC_free_nzt(p, &opt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NALLOC_STD - use the C std library malloc */

static void *_SC_nalloc_std(long nitems, long bpi, int na,
			    const char *fnc, const char *file, int line)
   {void *space;

    space = malloc(nitems*bpi);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_STD - use the C std library malloc */

static void *_SC_alloc_std(long nitems, long bpi, char *name, int na)
   {void *space;

    space = malloc(nitems*bpi);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_STD - use the C std library realloc */

static void *_SC_realloc_std(void *p, long nitems, long bpi, int na)
   {void *space;

    space = realloc(p, nitems*bpi);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_STD - use the C std library free */

static int _SC_free_std(void *p)
   {

    free(p);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_NALLOC_CHK - wrap a check for a specific pointer value
 *                - around the SC_alloc_na call
 *                - part of a usable API for memory debugging
 */

static void *_SC_nalloc_chk(long nitems, long bpi, int na,
			    const char *fnc, const char *file, int line)
   {void *space;
    SC_mem_opt opt;

    opt.na   = na;
    opt.zsp  = -1;
    opt.typ  = -1;
    opt.fnc  = fnc;
    opt.file = file;
    opt.line = line;

    space = SC_alloc_nzt(nitems, bpi, &opt);

    if (space == _SC_trap_ptr)
       raise(_SC_trap_sig);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_ALLOC_CHK - wrap a check for a specific pointer value
 *               - around the SC_alloc_na call
 *               - part of a usable API for memory debugging
 */

static void *_SC_alloc_chk(long nitems, long bpi, char *name, int na)
   {void *space;
    SC_mem_opt opt;

    opt.na  = na;
    opt.zsp = -1;
    opt.typ = -1;
    opt.fnc  = name;
    opt.file = NULL;
    opt.line = -1;

    space = SC_alloc_nzt(nitems, bpi, &opt);

    if (space == _SC_trap_ptr)
       raise(_SC_trap_sig);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_REALLOC_CHK - wrap a check for a specific pointer value
 *                 - around the SC_alloc_na call
 *                 - part of a usable API for memory debugging
 */

static void *_SC_realloc_chk(void *p, long nitems, long bpi, int na)
   {void *space;

    if (p == _SC_trap_ptr)
       raise(_SC_trap_sig);

    space = SC_realloc_na(p, nitems, bpi, na);

    if (space == _SC_trap_ptr)
       raise(_SC_trap_sig);

    return(space);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_CHK - wrap a check for a specific pointer value
 *              - around the SC_free
 *              - part of a usable API for memory debugging
 */

static int _SC_free_chk(void *p)
   {int rv;

    if (p == _SC_trap_ptr)
       raise(_SC_trap_sig);

    rv = SC_free(p);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                        MEMORY MANAGER COLLECTIONS                        */

/*--------------------------------------------------------------------------*/

/* SC_USE_SCORE_MM - use the SCORE memory manager */

SC_mem_fnc SC_use_score_mm(void)
   {SC_mem_fnc rv;

    rv = SC_gs.mm;

    SC_gs.mm.nalloc  = SC_nalloc_na;
    SC_gs.mm.alloc   = SC_alloc_na;
    SC_gs.mm.realloc = SC_realloc_na;
    SC_gs.mm.free    = SC_free;

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

    _SC_trap_ptr = p;
    _SC_trap_sig = sig;

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

    _SC_trap_ptr = NULL;
    _SC_trap_sig = -1;

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

    out = FMAKE_N(char, len, "SC_COPY_ITEM:out");
    if (out == NULL)
       return NULL;

    memcpy(out, in, len);

    return(out);}

/*---------------------------------------------------------------------------*/

/*                             HEAP ANALYZER ROUTINES                        */

/*---------------------------------------------------------------------------*/

/* _SC_NAME_OK - filter names for PACT function names */

int _SC_name_ok(char *name, int flag)
   {int i, n;
    char t[MAXLINE];
    static char *prefixes[] = { "_SC_", "SC_", "_PM_", "PM_", "_PD_", "PD_",
				"_PC_", "PC_", "_PG_", "PG_", "_PA_", "PA_",
				"_SH_", "SH_", "_SX_", "SX_", "_UL_", "ULA_",
				"PAAREC", "PABREC", "PAGRID", "PATRNL",
				"PATRNN", "PAMRGN"};

    if (name == NULL)
       return(FALSE);

    n = sizeof(prefixes)/sizeof(char *);
    if (flag & 1)
       {for (i = 0; i < n; i++)
	    {if (strncmp(name, prefixes[i], strlen(prefixes[i])) == 0)
	        return(FALSE);

	     snprintf(t, MAXLINE, "char*:%s", prefixes[i]);
	     if (strncmp(name, t, strlen(t)) == 0)
	        return(FALSE);};};

    if (flag & 2)
       {if (strncmp(name, "PERM|", 5) == 0)
	   return(FALSE);};

    return(TRUE);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _SC_BLOCK_NAME - return the name associated with the object P */

char *_SC_block_name(mem_descriptor *desc)
   {char *p, *rv;
    static char name[MAXLINE];

    rv = NULL;
    if (desc != NULL)
       {if (desc->file != NULL)
	   {p = strrchr(desc->file, '/');
	    if (p == NULL)
	       p = desc->file;
	    else
	       p++;
	    snprintf(name, MAXLINE, "%s(%s:%d)", desc->func, p, desc->line);}
	else if (desc->func != NULL)
	   SC_strncpy(name, MAXLINE, desc->func, -1);
	else
	   SC_strncpy(name, MAXLINE, "-none-", -1);

	rv = name;};

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _SC_PRINT_BLOCK_INFO - print out the descriptive info about a
 *                      - memory block
 */

void _SC_print_block_info(FILE *fp, SC_heap_des *ph, void *ptr, int flag)
   {int j, nc, nr, nf;
    long nb;
    char bf[MAXLINE], c;
    char *name, *ps, *pc;
    mem_descriptor *desc;
    mem_header *space;

    space = (mem_header *) ptr;
    desc  = &space->block;
    nb    = BLOCK_LENGTH(desc);
    nr    = REF_COUNT(desc);
    name  = _SC_block_name(desc);
    if (name == NULL)
       {if (flag == 0)
	   io_printf(fp, "   0x%012lx %9ld\t(no name)\n",
		     space+1, nb);}

    else if (!SC_pointer_ok(name))
       {io_printf(fp, "   Block: 0x%012lx (corrupted active memory block ",
		  space+1);
	io_printf(fp, "- %d\n", SC_HEAP_TID(ph));}

    else
       {if (FTN_NAME(desc))
	   {name = SC_F77_C_STRING((F77_string) name);
	    nc   = strlen(name);

	    ps = strchr(name, ' ');
	    if (ps != NULL)
	       {nf = ps - name;
		nc = min(nc, nf);};

	    SC_strncpy(bf, MAXLINE, name, nc);
	    name = bf;};
		     
	if (_SC_name_ok(name, flag))
	   {if (sizeof(char *) == 4)
	       io_printf(fp, "   0x%012lx %9ld %3d\t%s",
			 space+1, nb, nr, name);
	    else
	       io_printf(fp, "   0x%012lx %9ld %3d\t%s",
			 space+1, nb, nr, name);

/* GOTCHA: what do we do with "PERM|char*:..."? */
	    if (strncmp(name, "char*:", 6) == 0)
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

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SC_MEM_INFO - return the mem_descriptor info for the given pointer
 *             -    PL  byte length
 *             -    PT  data type index
 *             -    PR  reference count
 *             -    PN  allocated name
 *             - return TRUE iff P is valid
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

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SC_MEM_TRACE - return the number of active chunks of memory managed
 *              - by the system
 *              - the given pointer must have been allocated by SC_alloc
 *              - return -1 if the forward and backward counts differ
 *              - return -2 if a NULL pointer occurs in the chain
 *              - return -3 if the link count exceeds the number of blocks
 *              - return -4 if a corrupted header is detected
 *              -           also prints name of block
 *              - allocated
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
    if (SC_LATEST_BLOCK(ph) != NULL)
       {n    = SC_MAX_MEM_BLOCKS(ph) + BLOCKS_UNIT_DELTA;
	n_mf = 1;
	n_mb = 1;

	ok = TRUE;
	for (space = SC_LATEST_BLOCK(ph), i = 0L;
	     i < n;
	     space = space->block.next, n_mf++, i++)
	    {desc = &space->block;
	     if (!SCORE_BLOCK_P(desc))
	        {_SC_print_block_info(stdout, ph, (void *) space, FALSE);
		 ok = FALSE;
		 break;};

	     if ((desc->next == SC_LATEST_BLOCK(ph)) ||
		 (space == NULL))
	        break;};

	if (!ok)
	   ret = -4;

	else if ((i >= n) && (SC_MAX_MEM_BLOCKS(ph) != 0))
	   ret = -3;

	else if (space == NULL)
	   ret = -2;

	else
	   {for (space = SC_LATEST_BLOCK(ph), i = 0L;
		 i < n;
		 space = space->block.prev, n_mb++, i++)
	        if ((space == NULL) ||
		    (space->block.prev == SC_LATEST_BLOCK(ph)))
		   break;

	    if ((i >= n) && (SC_MAX_MEM_BLOCKS(ph) != 0))
	       ret = -3;

	    else if (space == NULL)
	       ret = -2;

	    else if (n_mf != n_mb)
	       ret = -1;

	    else
	       ret = n_mf;};};

    SC_LOCKOFF(SC_mm_lock);

    return(ret);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SC_REG_MEM - register a piece of memory for the purpose
 *            - of memory accounting
 */

int SC_reg_mem(void *p, long length, char *name)
   {int rv;
    SC_heap_des *ph;
    mem_descriptor *pd;
    haelem *hp;

    if (_SC.mem_table == NULL)
       _SC.mem_table = SC_make_hasharr(1001, FALSE, SC_HA_ADDR_KEY);

    ph = _SC_tid_mm();

    pd = FMAKE(mem_descriptor, "PERM|SC_REG_MEM:pd");

    SET_HEAP(pd, ph);

    pd->func      = name;
    pd->file      = NULL;
    pd->line      = -1;
    pd->ref_count = 1;
    pd->type      = 0;
    pd->length    = length;
    pd->prev      = NULL;
    pd->next      = (mem_header *) p;

    hp = SC_hasharr_install(_SC.mem_table, p, pd, "mem_descriptor", TRUE, TRUE);
    rv = (hp != NULL);

    return(rv);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SC_DEREG_MEM - deregister a piece of memory for the purpose
 *              - of memory accounting
 */

int SC_dereg_mem(void *p)
   {int rv;

    rv = SC_hasharr_remove(_SC.mem_table, p);

    return(rv);}

/*---------------------------------------------------------------------------*/
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
    space = SC_LATEST_BLOCK(ph);
    if (space != NULL)
       {n = SC_MAX_MEM_BLOCKS(ph) + BLOCKS_UNIT_DELTA;

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
/*---------------------------------------------------------------------------*/

/* DPRFREE - print the free memory lists */

void dprfree(void)
   {long i, j;
    SC_heap_des *ph;
    mem_descriptor *md;

    ph = _SC_tid_mm();

    io_printf(stdout, "Bin  Max  Blocks\n");
    for (j = 0L; j < _SC_n_bins; j++)
        {io_printf(stdout, "%3ld %4ld ", j, SC_BIN_SIZE(j));
         for (md  = SC_FREE_LIST(ph)[j], i = 0L;
	      md != NULL;
	      md  = (mem_descriptor *) md->func, i++)
             {io_printf(stdout, " %lx", md);
	      fflush(stdout);};
	 io_printf(stdout, "\n");};

    return;}

/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DFLPR - show the free list for the specified bin */

void dflpr(int j)
   {SC_heap_des *ph;
    mem_descriptor *md;

    ph = _SC_tid_mm();
    
    io_printf(stdout, "Free chunks of size %ld to %ld bytes\n",
	  (j < 1) ? 1L : SC_BIN_SIZE(j-1) + 1L,
	  SC_BIN_SIZE(j));
	  
    for (md = SC_FREE_LIST(ph)[j];
	 md != NULL;
	 md = (mem_descriptor *) md->func)
        io_printf(stdout, "%8lx\n", md);

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* _SC_FLCHK - return the total number of free blocks */

static long _SC_flchk(void)
   {int ok, bad;
    long j, k, nf;
    SC_heap_des *ph;
    mem_descriptor *md, *hd;

    ph = _SC_tid_mm();

    nf  = 0L;
    bad = 0;
    for (j = 0L; j < _SC_n_bins; j++)
        {hd = SC_FREE_LIST(ph)[j];
         for (k = 0L, md = hd;
	      md != NULL;
	      k++, md = (mem_descriptor *) md->func)
	     {ok = SC_pointer_ok(md);
	      if (!ok)
		 {io_printf(stdout,
			    "   Block: Head of free list %d,%ld corrupted\n",
			    ph->tid, j);

		  bad++;
		  break;};
		
	      ok = SC_pointer_ok(md->func);
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

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* SC_MEM_CHK - check out all aspects of managed memory
 *            - if bit #1 of TYP is 1 include the allocated memory
 *            - if bit #2 of TYP is 1 include the freed memory
 *            - if bit #3 of TYP is 1 include the registered memory
 *            - return values
 *            -   > 0  total blocks in requested memory pools
 *            -  -1    corrupt active pool
 *            -  -2    corrupt free pool
 *            -  -3    both pools corrupt
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

/*---------------------------------------------------------------------------*/

/*                            MEMORY INFO ROUTINES                           */

/*---------------------------------------------------------------------------*/

/* SC_IS_SCORE_PTR - return TRUE iff the given pointer P
 *                 - points to space allocated by SCORE
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
 *           - with SC_alloc
 *           - return -1L on error
 */

long SC_arrlen(void *p)
   {long nb, rv;
    mem_descriptor *desc;

    rv = -1L;

    if (SC_is_score_space(p, NULL, &desc))
       {nb = BLOCK_LENGTH(desc);
	rv = (nb < 0L) ? -1L : nb;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MARK - change the reference count by n */

int SC_mark(void *p, int n)
   {mem_descriptor *desc;

    if (SC_is_score_space(p, NULL, &desc))
       {if (REF_COUNT(desc) < UNCOLLECT)
           {REF_COUNT(desc) += n;}

        n = REF_COUNT(desc);}
    else
       {n = -1;}

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SET_COUNT - set the reference count of the given object */

int SC_set_count(void *p, int n)
   {mem_descriptor *desc;

    if (SC_is_score_space(p, NULL, &desc))
       {REF_COUNT(desc) = n;}

    else
       n = -1;

    return(n);}

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
       {n = REF_COUNT(desc);};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_PERMANENT - make an object unfreeable */

int SC_permanent(void *p)
   {int n;
    mem_descriptor *desc;

    n = -1;

    if (SC_is_score_space(p, NULL, &desc))
       {REF_COUNT(desc) = UNCOLLECT;

	n = REF_COUNT(desc);};

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
		printf("Length: %ld\n",    BLOCK_LENGTH(desc));
		printf("Type: %d\n",       BLOCK_TYPE(desc));
		printf("References: %d\n", REF_COUNT(desc));};

	    printf("Next Block: %p\n",     desc->next);
	    printf("Previous Block: %p\n", desc->prev);
	    printf("Heap: %d\n",           desc->heap->tid);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
