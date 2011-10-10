/*
 * SCMEMDB.C - memory management diagnostic functions part B
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h"
#include "scope_mem.h"

#define ENTRY_SIZE          128
#define MAP_ENTRY(_a, _i)   ((_a) + ENTRY_SIZE*(_i))

typedef void (*PFOTrace)(char *name, char *addr, long length,
			 int count, int type);
typedef struct s_mapdes mapdes;

struct s_mapdes
   {int flag;
    int show;
    int reg;
    int nbl;
    long nc;
    char *arr;};

typedef struct s_otrdes otrdes;

struct s_otrdes
   {long nm;
    int ty;
    long nb;
    PFOTrace fn;};

typedef struct s_ngbdes ngbdes;

struct s_ngbdes
   {int flag;
    mem_descriptor *a;       /* nearest block below */
    mem_descriptor *b;       /* the candidate block */
    mem_descriptor *c;};     /* nearest block above */

/*--------------------------------------------------------------------------*/

/*                             HEAP ANALYZER ROUTINES                       */

/*--------------------------------------------------------------------------*/

/* _SC_COUNT_TAGGED - count the number of bytes of memory
 *                  - not suppressed by FLAG
 */

static long _SC_count_tagged(int flag)
   {int nc, nf, acc;
    long i, n, nb, nbt;
    char bf[MAXLINE];
    char *name, *ps;
    SC_heap_des *ph;
    mem_header *space;
    mem_descriptor *desc;
    mem_inf *info;

    ph = _SC_tid_mm();

    SC_LOCKON(SC_mm_lock);

    nbt = 0;
    if (ph->latest_block != NULL)
       {n = ph->nx_mem_blocks + BLOCKS_UNIT_DELTA;
	for (space = ph->latest_block, i = 0L;
	     i < n;
	     space = space->block.next, i++)
	    {desc = &space->block;
	     if (SCORE_BLOCK_P(desc))
	        {info = &desc->desc.info;
	         acc  = ((flag & 8) || (info->ref_count != UNCOLLECT));
		 if (acc == TRUE)
		    {nb   = desc->length;
		     name = _SC_block_name(desc);
		     if (name == NULL)
		        nbt += nb;
		     else
		        {if (FF_NAME(desc))
			    {nc = strlen(name);
			     ps = strchr(name, ' ');
			     if (ps != NULL)
			        {nf = ps - name;
				 nc = min(nc, nf);};

			     SC_strncpy(bf, MAXLINE, name, nc);
			     name = bf;};
		     
			 if (name != NULL)
			    nbt += nb;};};};

	     if ((desc->next == ph->latest_block) ||
		 (space == NULL))
	        break;};};

    SC_LOCKOFF(SC_mm_lock);

    return(nbt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LIST_BLOCK_INFO - print out the descriptive info about a
 *                     - memory block matching FLAG
 *                     - FLAG follows the SC_mem_chk convention
 *                     - return TRUE if the block matches FLAG
 */

static int _SC_list_block_info(char *s, SC_heap_des *ph, void *ptr,
			       long sz, int flag, int show, int reg)
   {int rv, valid;
    int idok, nmok, prok, nxok, ty, nc, nr;
    long mad, nb;
    char t[MAXLINE];
    char *name;
    mem_header *space, *prev, *next;
    mem_descriptor *desc;
    mem_inf *info;
    SC_address ad;

    rv = FALSE;

    space      = (mem_header *) ptr;
    ad.memaddr = (char *) (space + 1);
    mad        = ad.mdiskaddr;

    desc = &space->block;
    info = &desc->desc.info;

    name = _SC_block_name(desc);
    prev = desc->prev;
    next = desc->next;
    nb   = desc->length;
    nr   = info->ref_count;
    ty   = info->type;
	      
    nmok = SC_pointer_ok(name);

/* info for registered block */
    if (reg == TRUE)
       {if (prev == NULL)
	   {idok = TRUE;
	    prok = TRUE;
	    nxok = TRUE;}
	else
	   {idok = FALSE;
	    prok = FALSE;
	    nxok = FALSE;};}

/* info for managed block */
    else
       {idok = SCORE_BLOCK_P(desc);
        prok = SC_pointer_ok(prev);
	nxok = SC_pointer_ok(next);};

/* expect an active block if length is greater than 0 */
    valid = TRUE;
    if (idok && (nb > 0) && prok && nxok)
       {if (flag & 1)
	   {if (name == NULL)
	       name = "-- no name --";
	    else if (!nmok)
	       {name = "-- corrupt active block --";
		nb   = sz;
		ty   = -2;
		nr   = -2;}
	    else if (FREE_SCORE_BLOCK_P(desc))
	       {nb = (nb == 0) ? sz : nb;
		ty = -2;
		nr = -2;};}
	else if (flag & 4)
	   {if (name == NULL)
	       name = "-- no name --";
	    else if (!nmok)
	       {name = "-- corrupt registered block --";
		nb   = sz;
		ty   = -2;
		nr   = -2;}
	    else if (FREE_SCORE_BLOCK_P(desc))
	       {nb = (nb == 0) ? sz : nb;
		ty = -2;
		nr = -2;};}
	else
	   valid = FALSE;}

/* expect a free block if desc is setup right */
    else if (idok && FREE_SCORE_BLOCK_P(desc) && prok && nxok)
       {if (flag & 2)
	   {if (!nmok)
	       {name = "-- corrupt free block --";
		ty   = -2;
		nr   = -2;}
	    else
	       {name = "-- free --";
		ty   = -1;
		nr   = 0;};
	    nb = sz;}
        else
	   valid = FALSE;}

/* otherwise a corrupted block */
    else
       {if ((name == NULL) || (!nmok))
	   name = "-- corrupt --";
	nb = sz;
        ty = -2;
        nr = -2;};

/* proceed if the block is valid relative to the testing done on it */
    if (valid == TRUE)
       {if ((ty == -2) && (nr == -2))
	   nc = snprintf(t, ENTRY_SIZE, "   0x%012lx %9ld ***  ***  %s",
			 mad, nb, name);
        else if (nr == UNCOLLECT)
	   nc = snprintf(t, ENTRY_SIZE, "   0x%012lx %9ld %3d PERM  %s",
			 mad, nb, ty, name);
	else
	   nc = snprintf(t, ENTRY_SIZE, "   0x%012lx %9ld %3d  %3d  %s",
			 mad, nb, ty, nr, name);

	nc = ENTRY_SIZE - nc - 1;
	SC_ASSERT(nc > 0);

	if ((nr != UNCOLLECT) || (show == TRUE))
	   {strcpy(s, t);
	    rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_BLOCK_INF - get the memory block info for a memory map */

static int _SC_mem_block_inf(SC_heap_des *ph, mem_descriptor *md,
			     mem_kind wh, void *a, long i, long j)
   {int ok, nbl;
    long nb;
    char *arr;
    mapdes *st;

    ok  = TRUE;
    st  = (mapdes *) a;
    nb  = md->length;
    nbl = st->nbl;
    arr = MAP_ENTRY(st->arr, nbl);

    st->nbl += _SC_list_block_info(arr, ph, md, nb,
				   st->flag, st->show, st->reg);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_N_BLOCKS - return the number of blocks contained in PH */

int _SC_n_blocks(SC_heap_des *ph, int flag)
   {int i, n, nmj;
    long nt, ntc, ntx;
    major_block_des *mbl;

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

/* add blocks tied up in major blocks - essentially the free blocks
 * this double counts some of active blocks
 */
    if (flag & 2)
       {nmj = ph->n_major_blocks;
	mbl = ph->major_block_list;
	for (i = 0; i < nmj; i++)
	    n += mbl[i].nunits;};

/* add registered block count */
    if ((flag & 4) && (_SC.mem_table != NULL))
       n += _SC.mem_table->size;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_LIST - return an array of text containing a memory map
 *              - each entry is ENTRY_SIZE characters long
 *              - done this way to limit memory allocations in case
 *              - the heap is in trouble
 *              - argument FLAG is a bit map determining which
 *              - sectors are reported
 *              -    1  active blocks
 *              -    2  free blocks
 *              -    4  registered blocks
 *              -    8  non-accountable blocks reported
 *              - this follows the convention of SC_mem_chk
 *              - if SHOW is TRUE include active UNCOLLECT'able blocks
 */

static void _SC_mem_list(int flag, int show, char **parr, int *pnbl)
   {int n, nbl;
    char *arr;
    SC_heap_des *ph;

    ph = _SC_tid_mm();

    SC_mem_over_mark(1);

    n = _SC_n_blocks(ph, flag);

/* NOTE: do not use the SCORE memory manager to allocate memory when
 * inspecting the state of the SCORE memory manager
 * use direct malloc call instead
 */
    nbl = 0;
    arr = malloc(n*ENTRY_SIZE);
    if (arr != NULL)
       {mapdes st;

	st.flag = flag;
	st.show = show;
	st.reg  = FALSE;
	st.nbl  = 0;
	st.arr  = arr;

	SC_mem_map_f(flag, _SC_mem_block_inf, &st);

/* sort the entries now */
        nbl = st.nbl;
	SC_text_sort(arr, nbl, ENTRY_SIZE);

	arr[ENTRY_SIZE*nbl] = '\0';};

    if (parr != NULL)
       *parr = arr;

    if (pnbl != NULL)
       *pnbl = nbl;

    SC_mem_over_mark(-1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_LIST - return an array of text containing a memory map
 *             - each entry is ENTRY_SIZE characters long
 *             - done this way to limit memory allocations in case
 *             - the heap is in trouble
 *             - argument FLAG is a bit map determining which
 *             - sectors are reported
 *             -    1  active blocks
 *             -    2  free blocks
 *             -    4  registered blocks
 *             -    8  non-accountable blocks reported
 *             - this follows the convention of SC_mem_chk
 *             - if SHOW is TRUE include active UNCOLLECT'able blocks
 */

char *SC_mem_list(int flag, int show)
   {char *arr;

    SC_LOCKON(SC_mm_lock);

    _SC_mem_list(flag, show, &arr, NULL);

    SC_LOCKOFF(SC_mm_lock);

    return(arr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_CORRUPT - list corrupted memory blocks and their immediate
 *                - nearest neighbors to the terminal
 *                - return the number of corrupted blocks
 */

int SC_mem_corrupt(int flag)
   {int i, nbl, nc, last;
    char *arr, *pa;

    SC_LOCKON(SC_mm_lock);

    _SC_mem_list(flag, TRUE, &arr, &nbl);

    last = TRUE;
    nc   = 0;
    for (i = 0; i < nbl; i++)
        {pa = arr + i*ENTRY_SIZE;
         if (strstr(pa, "***  ***") != NULL)
	    {nc++;

	     if ((i != 0) && (last == TRUE))
	        io_printf(stdout, "%8d %s\n", i-1, pa-ENTRY_SIZE);
	     io_printf(stdout, "%8d %s\n", i, pa);
	     last = FALSE;}

	 else if (last == FALSE)
	    {io_printf(stdout, "%8d %s\n", i, pa);
	     last = TRUE;};};

    SC_LOCKOFF(SC_mm_lock);

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_MAP - print out a memory map to the given FILE
 *             - WARNING: do not use io_printf here because it
 *             - may allocate memory and cause a deadlock
 */

int _SC_mem_map(FILE *fp, int flag, int show, int lineno)
   {int i, nbl;
    int64_t a, f, d;
    char *s, *arr;
    SC_heap_des *ph;

    ph = _SC_tid_mm();

    SC_LOCKON(SC_mm_lock);

    SC_mem_over_mark(1);

    nbl = 0;

    if (ph->latest_block != NULL)
       {if (fp == NULL)
	   fp = stdout;

/* report the active memory blocks */
	SC_mem_stats(&a, &f, &d, NULL);
	fprintf(fp, "\nMemory Map (%8lld %8lld %8lld)\n",
		(long long) a, (long long) f, (long long) d);

	_SC_mem_list(flag, show, &arr, &nbl);
        if (arr == NULL)
	   {fprintf(fp, "\nMemory too corrupt to map out\n");}
	else
	   {fprintf(fp, "   Block       Address         Bytes Type Refs Name\n");

	    if (lineno == TRUE)
	       {for (i = 0; TRUE; i++)
		    {s = MAP_ENTRY(arr, i);
		     if (s[0] == '\0')
		        break;
		     else
		        fprintf(fp, "%8d %s\n", i+1, s);};}
	    else
	       {for (i = 0; TRUE; i++)
		    {s = MAP_ENTRY(arr, i);
		     if (s[0] == '\0')
		        break;
		     else
		        fprintf(fp, "%s\n", s);};};

/*	    CFREE(arr); */
	    free(arr);

	    nbl = i;};

	fprintf(fp, "\n");};

    SC_mem_over_mark(-1);

    SC_LOCKOFF(SC_mm_lock);

    return(nbl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_MAP - print out a memory map to the given FILE
 *            - argument FLAG is a bit map determining which
 *            - sectors are reported
 *            -    1  active blocks
 *            -    2  free blocks
 *            -    4  registered blocks
 *            -    8  non-accountable blocks reported
 *
 * #bind SC_mem_map fortran() scheme(memory-map) python()
 */

int SC_mem_map(FILE *fp, int flag)
   {int nbl;

    nbl = _SC_mem_map(fp, flag, ((flag & 8) != 0), TRUE);

    return(nbl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_SS - print out memory map snapshots to a family of files
 *           - argument FLAG is a bit map determining which
 *            - sectors are reported
 *            -    1  active blocks
 *            -    2  free blocks
 *            -    4  registered blocks
 *            -    8  non-accountable blocks reported
 *
 * #bind SC_mem_ss fortran() scheme(memory-snapshot) python()
 */

int SC_mem_ss(char *base, int flag)
   {int nbl;
    char s[MAXLINE];
    FILE *fp;
    static int idx = 0;
    static char root[MAXLINE] = "mem-ss";

    if (base != NULL)
       {SC_strncpy(root, MAXLINE, base, -1);
	idx = 0;};

    snprintf(s, MAXLINE, "%s.%03d.map", root, idx++);
    fp = fopen(s, "w");

    nbl = _SC_mem_map(fp, flag, ((flag & 8) != 0), TRUE);

    fclose(fp);

    return(nbl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_MONITOR - Monitor memory leaks.
 *                - A pair calls lets you track memory that leaks in or
 *                - out of the region between the calls.
 *                - Arguments:
 *                -    OLD   byte count from previous call
 *                -          -1 to initialize
 *                -    LEV   level of monitoring
 *                -          0 - off
 *                -          1 - total byte measure only
 *                -          2 - give detailed map of leaked blocks
 *                -          4 - non-accountable blocks included
 *                -    ID    identifier for temporary files
 *                -    MSG   user allocated space to return error message
 *
 * #bind SC_mem_monitor fortran() scheme(memory-monitor) python()
 */

long SC_mem_monitor(int old, int lev, char *id, char *msg)
   {int leva, actfl, prmfl, show, pid, st;
    long d;
    char base[MAXLINE], ta[MAXLINE], tb[MAXLINE];
    char sd[MAXLINE], td[MAXLINE], cd[MAXLINE];
    FILE *fp;

    leva  = abs(lev);
    show  = FALSE;
    actfl = 5;        /* look at active and registered blocks */
    prmfl = 2;

    if (leva == 0)
       return(-1);

    SC_mem_over_mark(1);

    pid = SC_get_processor_number();

    snprintf(base, MAXLINE, "mem-%d-%s", pid, id);
    snprintf(tb, MAXLINE, "%s.before", base);
    snprintf(ta, MAXLINE, "%s.after", base);
    snprintf(td, MAXLINE, "%s.diff", base);
    snprintf(sd, MAXLINE, "LD_PRELOAD= ; diff %s %s > %s", tb, ta, td);
    snprintf(cd, MAXLINE, "LD_PRELOAD= ; cat %s", td);

    if (leva & 4)
       {show   = TRUE;
	actfl |= 8;
	prmfl |= 8;};

    d = _SC_count_tagged(prmfl);
/*    SC_mem_stats(NULL, NULL, &d, NULL); */

    if (old == -1)
       {if (leva > 1)
	   {fp = fopen(tb, "w");
	    _SC_mem_map(fp, actfl, show, FALSE);
	    fclose(fp);};}

    else
       {if (leva > 1)
	   {fp = fopen(ta, "w");
	    _SC_mem_map(fp, actfl, show, FALSE);
	    fclose(fp);

	    REMOVE(td);

	    if (old != d)
	       {st = SYSTEM(sd);
		SC_ASSERT(st == 0);

		st = SYSTEM(cd);
		SC_ASSERT(st == 0);};

	    REMOVE(tb);
	    REMOVE(ta);};

	if (old != d)
	   sprintf(msg,
		   "LEAKED %ld BYTES MEMORY - SC_MEM_MONITOR",
		   d - old);
	else
	  *msg = '\0';};

    SC_mem_over_mark(-1);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_NGB - determine whether MD is nearest to the B block in ST */

static int _SC_mem_ngb(SC_heap_des *ph, mem_descriptor *desc,
		       mem_kind wh, void *a, long i, long j)
   {int ok, skip;
    char *ae, *bs, *be, *cs, *ts, *te;
    mem_descriptor *ma, *mb, *mc;
    mem_inf *info;
    ngbdes *st;

    ok = TRUE;
    st = (ngbdes *) a;

    info = &desc->desc.info;
    skip = ((info->ref_count == UNCOLLECT) && ((st->flag & 8) == 0));

    if (SCORE_BLOCK_P(desc) && (skip == FALSE))
       {ma = st->a;
	if (ma == NULL)
	   ae = NULL;
	else
	   ae = ((char *) ma) + ma->length + ph->hdr_size;

	mb = st->b;
	bs = (char *) mb;
	be = bs + mb->length + ph->hdr_size;

	mc = st->c;
	if (mc == NULL)
	   cs = (char *) LONG_MAX;
	else
	   cs = (char *) mc;

	ts = (char *) desc;
	te = ts + desc->length + ph->hdr_size;

/* if DESC is between A and B make it the new A */
	if ((ae - ts < 0) && (te - bs <= 0))
	   st->a = desc;

/* if DESC is between B and C make it the new C */
	if ((be - ts < 0) && (te - cs <= 0))
	   st->c = desc;

	ok = SC_pointer_ok((void *) info->pfunc);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_NEIGHBOR - return pointers to the managed spaces physically
 *                 - before, B, and after, A, the managed space P
 *                 - return TRUE if at least one neighbor was found
 *                 - this is to be used to help identify spaces that
 *                 - were over-indexed (PPA) or under-indexed (PPB) to
 *                 - clobber the space P
 */

int SC_mem_neighbor(void *p, int flag, void *b, void *a)
   {int rv;
    mem_header *space, *pa, *pb;
    mem_header **ppa;
    ngbdes st;
	
    rv = FALSE;
    pa = NULL;
    pb = NULL;
    if (p != NULL)
       {space = ((mem_header *) p) - 1;

	st.flag = flag;
	st.a    = NULL;
	st.b    = &space->block;
	st.c    = NULL;

	SC_mem_map_f(flag, _SC_mem_ngb, &st);

	pb = (mem_header *) st.a;
	pa = (mem_header *) st.c;};

    ppa  = (mem_header **) a;
    *ppa = (pa == NULL) ? NULL : pa + 1;

    ppa  = (mem_header **) b;
    *ppa = (pb == NULL) ? NULL : pb + 1;

    SC_mem_over_mark(-1);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MEM_OBJ_TRACE - perform object trace on DESC if it meets critera */

static int _SC_mem_obj_trace(SC_heap_des *ph, mem_descriptor *desc,
			     mem_kind wh, void *a, long i, long j)
   {int it, type, ok;
    long ib, nb;
    char *name;
    mem_header *space;
    mem_inf *info;
    PFOTrace fn;
    otrdes *st;

    ok = TRUE;

    st   = (otrdes *) a;
    type = st->ty;
    nb   = st->nb;
    fn   = st->fn;

    info = &desc->desc.info;

    ib = desc->length;
    it = info->type;

    if (((type == -1) || (type == it)) && (nb == ib))
       {st->nm++;
	space = (mem_header *) desc;
	name  = _SC_block_name(desc);
	fn(name, (char *) (space + 1), ib, info->ref_count, it);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_OBJECT_TRACE - report all objects matching size and type */

long SC_mem_object_trace(long nb, int type,
			 void (*f)(char *name, char *addr, long length,
				   int count, int type))
   {long nm;
    otrdes st;
	
    nm = 0;
    if ((nb > 0) && (f != NULL))
       {st.nb = nb;
	st.ty = type;
	st.nm = 0;
        st.fn = f;

	SC_mem_map_f(7, _SC_mem_obj_trace, &st);

	nm = st.nm;};

    return(nm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_PRINT - print the mem_descriptor info for the given pointer
 *
 * #bind SC_mem_print fortran() python()
 */

void SC_mem_print(void *p)
   {long id;
    mem_descriptor *desc;
    mem_header *space;
    mem_inf *info;

    if (p == NULL)
       return;

    space = ((mem_header *) p) - 1;
    desc  = &space->block;
    info  = &desc->desc.info;

    id = SC_GET_BLOCK_ID(desc);

    if (FREE_SCORE_BLOCK_P(desc))
       {io_printf(stdout, "  Address        : 0x%lx\n", p);
	io_printf(stdout, "  Active         : free\n");
	io_printf(stdout, "  Number of bytes: %ld\n",   desc->length);
	io_printf(stdout, "  Type index     : %x\n",    info->type);
	io_printf(stdout, "  Reference count: %x\n",    info->ref_count);
	io_printf(stdout, "  Block id       : %lx\n",   id);
	io_printf(stdout, "  Next block     : 0x%lx\n", info->pfunc);}

    else if (SCORE_BLOCK_P(desc))
       {io_printf(stdout, "  Address        : 0x%lx\n", p);
	io_printf(stdout, "  Active         : yes\n");
	io_printf(stdout, "  Associated name: %s\n",    _SC_block_name(desc));
	io_printf(stdout, "  Number of bytes: %ld\n",   desc->length);
	io_printf(stdout, "  Type index     : %d\n",    info->type);
	io_printf(stdout, "  Reference count: %d\n",    info->ref_count);
	io_printf(stdout, "  Block id       : %lx\n",   id);
	io_printf(stdout, "  Previous block : 0x%lx\n", desc->prev);
	io_printf(stdout, "  Next block     : 0x%lx\n", desc->next);}

    else
       io_printf(stdout,
		 "Space at %lx was not allocated by the SCORE memory manager\n",
		 p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_DIFF - report changes to a block of memory between two calls
 *             - in the first call one of the two input pointers A and B
 *             - must be NULL and a copy of the non-NULL one is returned
 *             - in the second call the original non-NULL pointer is
 *             - passed in as A along with the copy returned by the
 *             - first call as B
 *             - the differences are reported via FP and the copy is released
 */

void *SC_mem_diff(FILE *fp, void *a, void *b, size_t nb)
   {int is, id, nd, first;
    size_t i;
    char *p, *s, *pa, *pb;

    if (a == NULL)
       p = b;
    else if (b == NULL)
       p = a;
    else
       p = NULL;

/* first call - make a copy of the non-NULL argument */
    if (p != NULL)
       {s = CMAKE_N(char, nb);
	memcpy(s, p, nb);}

/* second call report the differences */
    else
       {is = -1;
	id = -1;
	pa = (char *) a;
	pb = (char *) b;
	nd = 0;

	first = TRUE;
	for (i = 0; i < nb; i++)
	    {if (pa[i] == pb[i])
	        {if ((is < id) && (0 <= id))
		    {if (first == TRUE)
		        {first = FALSE;
			 io_printf(fp, "\n");
			 io_printf(fp, " Diff    # bytes     IStart      IEnd         StartA        EndA\n");
			 io_printf(fp, " ----    -------     ------      ----         ------        ----\n");};
		     io_printf(fp,
			       "%5d   %8d   %8d  %8d     0x%08lx  0x%08lx\n",
			       nd, id-is, is, id,
			       (long) pa+is, (long) pa+id);};
		 is = i;}
	     else
	        {id = i;
		 if (id - is == 1)
		    nd++;};};

	CFREE(b);
	s = NULL;};

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_FRAG - compute and return the memory fragmentation
 *             - the measure of memory fragmenation is:
 *             -    (adx - adn - nb)/nb
 *             - where
 *             -     adx  the highest address of any byte of managed memory
 *             -     adn  the lowest address of any byte of managed memory
 *             -     nb   the number of active bytes of managed memory
 *             - thread action on TID
 *             -   -2 : all heaps
 *             -   -1 : current heap
 *             -  0-N : specified heap
 *             - NDE is the number of MSB to use as the basis for binning
 *             - the memory
 */

double SC_mem_frag(int tid, int nde)
   {int it, itn, itx, idv, ndv, nf, ns;
    long i, n, nb, adr, amx, lmx;
    long *nbt, *nbl;
    double fr, nbf, blf;
    double *adn, *adx, *bpb, *frb;
    SC_heap_des *ph;
    SC_address ad;
    mem_header *space;
    mem_descriptor *desc;

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

    SC_LOCKON(SC_mm_lock);

/* initialize the bins */
    ndv = 1 << nde;
    adn = CMAKE_N(double, ndv);
    adx = CMAKE_N(double, ndv);
    bpb = CMAKE_N(double, ndv);
    frb = CMAKE_N(double, ndv);
    nbt = CMAKE_N(long, ndv);
    nbl = CMAKE_N(long, ndv);
    for (i = 0; i < ndv; i++)
        {adn[i] = HUGE;
	 adx[i] = -HUGE;
	 bpb[i] = 0.0;
	 frb[i] = 0.0;
	 nbt[i] = 0;
	 nbl[i] = 0;};

/* find the largest address to compute ns */
    amx = LONG_MIN;
    for (it = itn; it < itx; it++)
        {ph = _SC_get_heap(it);
	 if ((ph != NULL) && (ph->latest_block != NULL))
	    {n = ph->nx_mem_blocks + BLOCKS_UNIT_DELTA;
	     for (space = ph->latest_block, i = 0L;
		  i < n;
		  space = space->block.next, i++)
	         {desc = &space->block;
		  nb   = ph->hdr_size + desc->length;

		  ad.memaddr = (char *) desc;
                  adr        = ad.diskaddr;

		  amx = max(amx, adr + nb);

		  if ((desc->next == ph->latest_block) || (space == NULL))
		     break;};};};

    for (i = 0; SC_hasharr_next(_SC.mem_table, &i, NULL, NULL, (void **) &space); i++)
        {desc = &space->block;
	 nb   = desc->length;

	 ad.memaddr = (char *) desc;
	 adr        = ad.diskaddr;

	 amx = max(amx, adr + nb);};

    for (ns = 0, lmx = amx; lmx > 0; ns++, lmx >>= 1);
    ns -= nde;

/* loop over the specified thread heaps */
    for (it = itn; it < itx; it++)
        {ph = _SC_get_heap(it);
	 if ((ph != NULL) && (ph->latest_block != NULL))
	    {n = ph->nx_mem_blocks + BLOCKS_UNIT_DELTA;
	     for (space = ph->latest_block, i = 0L;
		  i < n;
		  space = space->block.next, i++)
	         {desc = &space->block;
		  nb   = ph->hdr_size + desc->length;

		  ad.memaddr = (char *) desc;
                  adr        = ad.diskaddr;
		  idv        = adr >> ns;

		  adn[idv]  = min(adn[idv], adr);
		  adx[idv]  = max(adx[idv], adr + nb);
		  nbt[idv] += nb;
		  nbl[idv]++;

		  if ((desc->next == ph->latest_block) || (space == NULL))
		     break;};};};

    for (i = 0; SC_hasharr_next(_SC.mem_table, &i, NULL, NULL, (void **) &space); i++)
        {desc = &space->block;
	 nb   = desc->length;

	 ad.memaddr = (char *) desc;
	 adr        = ad.diskaddr;
	 idv        = adr >> ns;

	 adn[idv]  = min(adn[idv], adr);
	 adx[idv]  = max(adx[idv], adr + nb);
	 nbt[idv] += nb;
	 nbl[idv]++;};

/* compute average fragmentation over bins */
    fr = 0.0;
    nf = 0;
    for (i = 0; i < ndv; i++)
        {nbf = nbt[i];
	 if (nbf > 0.0)
	    {blf    = nbl[i];
	     bpb[i] = nbf/blf;
	     frb[i] = (adx[i] - adn[i] - nbf)/nbf;

	     fr += frb[i];
	     nf++;};};

    fr *= 1.0/((double) nf);

    CFREE(adn);
    CFREE(adx);
    CFREE(bpb);
    CFREE(frb);
    CFREE(nbt);
    CFREE(nbl);

    SC_LOCKOFF(SC_mm_lock);

    return(fr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRSTATS - print memory stats for specified heap */

void dprstats(int tid, int ifmt)
   {SC_heap_des *ph;

    ph = _SC_get_heap(tid);

    if (ifmt == TRUE)
       {printf("Allocated: %ld\n", (long) ph->sp_alloc);
	printf("Freed:     %ld\n", (long) ph->sp_free);
	printf("Diff:      %ld\n", (long) ph->sp_diff);
	printf("Max Diff:  %ld\n", (long) ph->sp_max);}
    else
       {printf("Allocated: %10.3e\n", (double) ph->sp_alloc);
	printf("Freed:     %10.3e\n", (double) ph->sp_free);
	printf("Diff:      %10.3e\n", (double) ph->sp_diff);
	printf("Max Diff:  %10.3e\n", (double) ph->sp_max);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
