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
	        {acc = ((flag & 8) || (desc->ref_count != UNCOLLECT));
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
    mem_descriptor *desc;
    mem_header *space, *prev, *next;
    SC_address ad;

    rv = FALSE;

    space      = (mem_header *) ptr;
    ad.memaddr = (char *) (space + 1);
    mad        = ad.mdiskaddr;

    desc = &space->block;

    prev = desc->prev;
    next = desc->next;
    name = _SC_block_name(desc);
    nr   = desc->ref_count;
    ty   = desc->type;
    nb   = desc->length;
	      
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

/* _SC_N_BLOCKS - return the number of blocks contained in
 *              - major blocks
 */

static int _SC_n_blocks(SC_heap_des *ph)
   {int i, n, nmj;
    major_block_des *mbl;

    nmj = ph->n_major_blocks;
    mbl = ph->major_block_list;
    for (i = 0, n = 0; i < nmj; i++)
        n += mbl[i].nunits;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LIST_MAJOR_BLOCKS - list selected blocks in major blocks
 *                       - this is to be done carefully so as to detect
 *                       - corrupted memory
 *                       - return the number of blocks found
 */
 
static int _SC_list_major_blocks(char *arr, int nc, 
			         int flag, int show, SC_heap_des *ph)
   {int i, j, id, nbl, nmj;
    long nu, sz, nb;
    char *pn;
    mem_descriptor *desc;
    major_block_des *mbl;

    nbl = 0;

    nmj = ph->n_major_blocks;
    mbl = ph->major_block_list;
    for (i = 0; i < nmj; i++)
        {pn = mbl[i].block;
	 nu = mbl[i].nunits;
	 sz = mbl[i].size;
	 id = mbl[i].index;
	 nb = SC_BIN_SIZE(id);

	 for (j = 0; j < nu; j++, pn += sz)
	     {desc = (mem_descriptor *) pn;
	      nbl += _SC_list_block_info(MAP_ENTRY(arr, nbl), ph,
					 desc, nb, flag, show, FALSE);};};

    return(nbl);}

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
    long i, nb, nbthr, nt, ntc, ntx;
    char *arr;
    SC_heap_des *ph;
    mem_header *space, *nxt;
    mem_descriptor *desc;

    ph = _SC_tid_mm();

    SC_mem_over_mark(1);

    nbthr = SC_BIN_THRESHOLD();
    nbl   = 0;

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
 * this double count some of active blocks
 */
    if (flag & 2)
       n += _SC_n_blocks(ph);

/* add registered block count */
    if ((flag & 4) && (_SC.mem_table != NULL))
       n += _SC.mem_table->size;

/* NOTE: do not use the SCORE memory manager to allocate memory when
 * inspecting the state of the SCORE memory manager
 * use direct malloc call instead
 */
    arr = malloc(n*ENTRY_SIZE);
    if (arr != NULL)

/* handle active blocks that are too big to be in the major blocks lists */
       {if ((flag & 1) && (ph->latest_block != NULL))
	   {n     = ph->nx_mem_blocks + BLOCKS_UNIT_DELTA;
	    space = ph->latest_block;
	    for (i = 0; (i < n) && (space != NULL); i++, space = nxt)
	        {desc = &space->block;
		 if (!SC_pointer_ok(desc))
		    break;

		 nxt = desc->next;
		 nb  = desc->length;
		 if (nb > nbthr)
		    nbl += _SC_list_block_info(MAP_ENTRY(arr, nbl), ph,
					       space, nb, flag, show, FALSE);

		 if (nxt == ph->latest_block)
		    break;};};

/* handle blocks in the major blocks lists */
	nbl += _SC_list_major_blocks(MAP_ENTRY(arr, nbl), ENTRY_SIZE,
				     flag, show, ph);

/* report the registered memory blocks */
	if ((flag & 4) && (_SC.mem_table != NULL))
	   {for (i = 0; SC_hasharr_next(_SC.mem_table, &i, NULL, NULL, (void **) &space); i++)
	        {desc  = &space->block;
		 nb    = desc->length;
		 nbl  += _SC_list_block_info(MAP_ENTRY(arr, nbl), ph,
					     space, nb, flag, show, TRUE);};};

/* sort the entries now */
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
    long a, f, d;
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
	fprintf(fp, "\nMemory Map (%8ld %8ld %8ld)\n", a, f, d);

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

    nbl = _SC_mem_map(fp, flag, FALSE, TRUE);

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

/* SC_MEM_NEIGHBOR - return pointers to the managed spaces physically
 *                 - before, PPB, and after, PPA, the managed space P
 *                 - return TRUE if at least one neighbor was found
 *                 - this is to be used to help identify spaces that
 *                 - were over-indexed (PPA) or under-indexed (PPB) to
 *                 - clobber the space P
 */

int SC_mem_neighbor(void *p, void *a, void *b)
   {int rv;
    long i, nu, nmj, nb, sz;
    char *bs, *be, *pa, *pb, *pt;
    SC_heap_des *ph;
    major_block_des *mbl;
    mem_descriptor *md;
    mem_header *space;
    mem_header **ppa;

    ph = _SC_tid_mm();

    SC_LOCKON(SC_mm_lock);

    SC_mem_over_mark(1);

    rv = FALSE;
    pa = NULL;
    pb = NULL;
    if (p != NULL)
       {space = ((mem_header *) p) - 1;
	md    = &space->block;

	pt = (char *) md;

	if (SCORE_BLOCK_P(md))

/* check all the major blocks */
	   {nmj = ph->n_major_blocks;
	    mbl = ph->major_block_list;
	    for (i = 0; i < nmj; i++)
	        {bs = mbl[i].block;
		 nu = mbl[i].nunits;
		 sz = mbl[i].size;
		 nb = nu*sz;
		 be = bs + nb;

/* if the descriptor P is in the block compute the
 * address of the one before and the one after P
 */
		 if ((bs <= pt) && (pt < be))
		    {rv = TRUE;
		     pb = pt - sz;
		     if (pb < bs)
		        pb = NULL;
		     pa = pt + sz;
		     if (be <= pa)
		        pa = NULL;
		     break;};};};};

    ppa  = (mem_header **) a;
    *ppa = (pa == NULL) ? NULL : ((mem_header *) pa + 1);

    ppa  = (mem_header **) b;
    *ppa = (pb == NULL) ? NULL : ((mem_header *) pb + 1);

    SC_mem_over_mark(-1);

    SC_LOCKOFF(SC_mm_lock);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_OBJECT_TRACE - report all objects matching size and type */

long SC_mem_object_trace(long nb, int type,
			 void (*f)(char *name, char *addr, long length,
				   int count, int type))
   {int j, jt, it;
    long i, l, us, nu, nmj, nm, ib;
    char *pn, *name;
    SC_heap_des *ph;
    major_block_des *mbl;
    mem_descriptor *md;
    mem_header *space;

    ph = _SC_tid_mm();

    SC_LOCKON(SC_mm_lock);

    if ((nb > 0) && (f != NULL))
       {j  = SC_BIN_INDEX(nb);
	us = ph->hdr_size + SC_BIN_SIZE(j);
	us = ((us + _SC_ms.mem_align_pad) >> _SC_ms.mem_align_expt) <<
	     _SC_ms.mem_align_expt;
	nu = SC_BIN_UNITS(us);

/* check all the major blocks */
	nmj = ph->n_major_blocks;
	mbl = ph->major_block_list;
	nm  = 0L;
	for (i = 0; i < nmj; i++)
	    {md = (mem_descriptor *) mbl[i].block;

/* check if the blocks in this major block could match the size */
	     ib = md->length;
	     jt = SC_BIN_INDEX(ib);
	     if (j != jt)
	        continue;

/* scan the blocks for exact matches */
	     pn = (char *) md;
	     for (l = 0; l < nu; l++, pn += us)
	         {md = (mem_descriptor *) pn;
		  ib = md->length;
		  it = md->type;
		  if (((type == -1) || (type == it)) && (nb == ib))
		     {nm++;
		      space = (mem_header *) md;
		      name = _SC_block_name(md);
		      (*f)(name, (char *) (space + 1), ib,
			   (int) md->ref_count, (int) it);};};};};

    SC_LOCKOFF(SC_mm_lock);

    return(nm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MEM_PRINT - print the mem_descriptor info for the given pointer
 *
 * #bind SC_mem_print fortran() python()
 */

void SC_mem_print(void *p)
   {mem_descriptor *desc;
    mem_header *space;

    if (p == NULL)
       return;

    space = ((mem_header *) p) - 1;
    desc  = &space->block;
    if (FREE_SCORE_BLOCK_P(desc))
       {io_printf(stdout, "  Address        : 0x%lx\n", p);
	io_printf(stdout, "  Active         : free\n");
	io_printf(stdout, "  Number of bytes: %ld\n",   desc->length);
	io_printf(stdout, "  Type index     : %x\n",    desc->type);
	io_printf(stdout, "  Reference count: %x\n",    desc->ref_count);
	io_printf(stdout, "  Block id       : %lx\n",   desc->id);
	io_printf(stdout, "  Next block     : 0x%lx\n", desc->where.pfunc);}

    else if (SCORE_BLOCK_P(desc))
       {io_printf(stdout, "  Address        : 0x%lx\n", p);
	io_printf(stdout, "  Active         : yes\n");
	io_printf(stdout, "  Associated name: %s\n",    _SC_block_name(desc));
	io_printf(stdout, "  Number of bytes: %ld\n",   desc->length);
	io_printf(stdout, "  Type index     : %d\n",    desc->type);
	io_printf(stdout, "  Reference count: %d\n",    desc->ref_count);
	io_printf(stdout, "  Block id       : %lx\n",   desc->id);
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

void dprstats(int tid)
   {SC_heap_des *ph;

    ph = _SC_get_heap(tid);

    printf("Allocated: %ld\n", (long) ph->sp_alloc);
    printf("Freed:     %ld\n", (long) ph->sp_free);
    printf("Diff:      %ld\n", (long) ph->sp_diff);
    printf("Max Diff:  %ld\n", (long) ph->sp_max);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/