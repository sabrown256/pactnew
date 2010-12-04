/*
 * SCMM.C - a modified version of the MM in K&R
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#define TEST

#define NALLOC 65535
/* #define NALLOC 65536 */

#define NEXT(p) (p->s.ptr)
#define SIZE(p) (p->s.size)

#define ENCLOSES(p, bp) ((p < bp) && (bp < NEXT(p)))
#define INCLUDES(p, bp) ((p < bp) && (bp <= NEXT(p)))
#define OFF_ENDS(p, bp) ((p >= NEXT(p)) && ((p < bp) || (bp < NEXT(p))))
#define AT_ENDS(p, bp) ((p >= NEXT(p)) && ((p < bp) || (bp <= NEXT(p))))

typedef struct s_mem_block mem_block;
typedef union u_Header Header;

struct s_mem_block
   {Header *ptr;
    unsigned int size;};

union u_Header
   {mem_block s;
    double x;};

static Header
 base,
 *freep = NULL;

char
 **mm_lst = NULL;

long
 total_bytes = 0L,
 mm_lst_len = 0L,
 mm_lst_max = 0L,
 max_block_size = 100000;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MALLOC_S - SCORE mm */

static void *_SC_malloc_s(unsigned int nbytes)
   {void *rv;

    rv = SC_alloc_nzt(nbytes, 1L, NULL, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_S - SCORE mm */

static void _SC_free_s(void *ap)
   {

    SC_free(ap);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MORECORE - ask the system for more memory
 *              - this is where the machine and OS dependency lie
 */

static Header *_SC_morecore(unsigned int nu, void (*fp)(void *p))
   {char *cp;
    int size;
    Header *up;

    if (mm_lst == NULL)
       {mm_lst_max = 50L;
        mm_lst = calloc(mm_lst_max, sizeof(char *));
        mm_lst_len = 0L;};

    if (mm_lst == NULL)
       exit(1);

    if (nu < NALLOC)
       nu = NALLOC;

    size = nu*sizeof(Header);

    cp = malloc(size);
    if (cp == NULL)
       {io_printf(STDOUT,
		  "\nMemory exhausted after %ld bytes in %ld blocks allocated\n",
		  total_bytes, mm_lst_len);
        exit(1);};

    total_bytes += size;
    mm_lst[mm_lst_len++] = cp;
    if (mm_lst_len >= mm_lst_max)
       {mm_lst_max += 50L;
        mm_lst = realloc(mm_lst, mm_lst_max);};

    up = (Header *) cp;
    SIZE(up) = nu;
    (*fp)(up + 1);

    return(freep);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_RELEASE_CORE - release big blocks grabbed by _SC_morecore */

static void _SC_release_core(void)
   {int i;

    if (mm_lst == NULL)
       return;

    for (i = 0; i < mm_lst_len; i++)
        if (mm_lst[i] != NULL)
           free(mm_lst[i]);

    mm_lst_len  = 0L;
    total_bytes = 0L;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_E - put block back into free list
 *            - corresponds to _SC_malloc_e
 */

static void _SC_free_e(void *ap)
   {Header *bp, *p, *np;

/* get the pointer to the block header */
    bp = (Header *) ap - 1;

/* find freed block at start or end of arena */
    for (p = freep; !ENCLOSES(p, bp); p = NEXT(p))
        {if (OFF_ENDS(p, bp))
            break;};

/* join to upper neighbor */
    np = NEXT(p);
    if (bp + SIZE(bp) == np)
       {SIZE(bp) += SIZE(np);
        NEXT(bp)  = NEXT(np);}
    else
       NEXT(bp) = NEXT(p);

/* join to lower neighbor */
    if (p + SIZE(p) == bp)
       {SIZE(p) += SIZE(bp);
        NEXT(p)  = NEXT(bp);}
    else
       NEXT(p) = bp;

    freep = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MALLOC_E - a general purpose memory manager that is portable
 *              - this is K&R taking memory from the end of the block
 */

static void *_SC_malloc_e(unsigned int nbytes)
   {unsigned int nunits;
    Header *p, *prevp;
    void *rv;

    nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    if ((prevp = freep) == NULL)
       {base.s.ptr  = freep = prevp = &base;
        base.s.size = 0;};

    p = NEXT(prevp);
    while (TRUE)

/* if big enough look further */
        {if (SIZE(p) >= nunits)

/* if exactly enough, grab it all */
            {if (SIZE(p) == nunits)
                NEXT(prevp) = NEXT(p);

/* else allocate tail end */
             else
                {SIZE(p) -= nunits;
                 p       += SIZE(p);
                 SIZE(p)  = nunits;};

             freep = prevp;
             rv    = (void *) (p+1);

             return(rv);};

/* wrapped around free list - get more memory from system */
         if (p == freep)
            if ((p = _SC_morecore(nunits, _SC_free_e)) == NULL)
               break;

         prevp = p;
         p = NEXT(p);};

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_FREE_B - put block back into free list
 *            - corresponds to _SC_malloc_b
 */

static void _SC_free_b(void *ap)
   {Header *bp, *p, *np;

/* get the pointer to the block header */
    bp = (Header *) ap - 1;

/* find freed block at start or end of arena */
    for (p = freep; !INCLUDES(p, bp); p = NEXT(p))
        if (AT_ENDS(p, bp))
           break;

/* join to upper neighbor */
    np = NEXT(p);
    if (bp + SIZE(bp) == np)
       {SIZE(bp) += SIZE(np);
        NEXT(bp)  = NEXT(np);}
    else
       NEXT(bp) = NEXT(p);

/* join to lower neighbor */
    if (p + SIZE(p) == bp)
       {SIZE(p) += SIZE(bp);
        NEXT(p)  = NEXT(bp);}
    else
       NEXT(p) = bp;

    freep = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MALLOC_B - a general purpose memory manager that is portable
 *              - this is K&R but taking memory from the beginning
 *              - of the block
 */

static void *_SC_malloc_b(unsigned int nbytes)
   {unsigned int nunits;
    Header *p, *np, *prevp;
    void *rv;

    nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
    if ((prevp = freep) == NULL)
       {base.s.ptr  = freep = prevp = &base;
        base.s.size = 0;};

    p = NEXT(prevp);
    while (TRUE)

/* if big enough look further */
        {if (SIZE(p) >= nunits)
	    {np = 0;

/* if exactly enough, grab it all */
	     if (SIZE(p) == nunits)
                NEXT(prevp) = NEXT(p);

/* else allocate front end */
             else
                {np          = p + nunits;
                 SIZE(np)    = SIZE(p) - nunits;
                 NEXT(np)    = NEXT(p);
                 NEXT(prevp) = np;};

/*             freep = prevp; */
             freep = np;
             rv    = (void *) (p+1);

             return(rv);};

/* wrapped around free list - get more memory from system */
         if (p == freep)
            if ((p = _SC_morecore(nunits, _SC_free_b)) == NULL)
               break;

         prevp = p;
         p = NEXT(p);};

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MM_TRACE - count the number of free blocks and
 *          - the number of bytes in free storage
 */

static void mm_trace(int *pn, long *pm)
    {int n;
     long m;
     Header *p;

     n = 0;
     m = 0L;
     if (freep != NULL)
        for (p = NEXT(freep); p != freep; p = NEXT(p))
            {n++;
             m += SIZE(p);};

     *pn = n;
     *pm = m;

     return;}

/*--------------------------------------------------------------------------*/

#ifdef TEST

/*--------------------------------------------------------------------------*/

/* TEST_MM - test a memory manager */

static void test_mm(void *(*am)(), void (*fm)(), int n, FILE *fp,
		    double *pt, double *pm, int *pnf)
   {int i, j, sz, nbl, nb, nfree;
    long mb;
    char **p;
    double t;
    static int test = 1;
    static unsigned int seed = 1;

    if (fp != NULL)
       io_printf(fp, "\n\nStart test #%d\n\n", test++);

    srand(seed);
    t = SC_cpu_time();

    nfree = 0;
    freep = NULL;
    nbl   = n*sizeof(char *);

    p  = (char **) (*am)(nbl);
    if (p == NULL)
       {*pt = -1.0;
        *pm = -1.0;
        return;};

    if (fp != NULL)
       {io_printf(fp, "\n\n\nBlock\t  Length\t Address\n\n");
        io_printf(fp, "A\t%8ld\t%8lx\tList\n", nbl, (long) p);};

    memset(p, 0, nbl);

    sz = 31;
    for (i = 0; i < n; i++)
        {sz = SC_rand(seed) % max_block_size;

         p[i] = (*am)(sz);
         if (p[i] == NULL)
            {if (fp != NULL)
                io_printf(fp, "Allocation failed for %ld bytes\n", sz);}
         else
            {if (fp != NULL)
                io_printf(fp, "A\t%8ld\t%8lx\n", sz, (long) p[i]);};

         j = sz % n;
         if (p[j] != NULL)
            {if (fp != NULL)
                io_printf(fp, "F\t\t\t%8lx\n", (long) p[j]);
             (*fm)(p[j]);
             nfree++;
             p[j] = NULL;};};

    mm_trace(&nb, &mb);
    *pm  = ((double) mb)/((double) n);
    *pnf = nfree;

    for (j = 0; j < n; j++)
        if (p[j] != NULL)
           {if (fp != NULL)
               io_printf(fp, "F\t\t\t%8lx\tCleanup\n", (long) p[j]);
            (*fm)(p[j]);
            p[j] = NULL;};

    (*fm)(p);

    _SC_release_core();

    if (fp != NULL)
       {io_printf(fp, "F\t\t\t%8lx\tList\n", (long) p);
        fflush(fp);};

    *pt = SC_cpu_time() - t;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the MM */

int main(int argc, char **argv)
   {int n, nf, nm;
    double time, mb;
    FILE *fp;

    if (argc < 2)
       {io_printf(STDOUT, "\n\n");
        io_printf(STDOUT,
		  "Usage: scmm <n-max> [<max_block_size>]\n");
        io_printf(STDOUT,
		  "       Perform a series of tests allocating n blocks of\n");
        io_printf(STDOUT,
		  "       random size memory up to a specified maximum size\n");
        io_printf(STDOUT,
		  "       (and randomly deallocating some of them) using two\n");
        io_printf(STDOUT,
		  "       variants of the K&R memory manager and the system\n");
        io_printf(STDOUT,
		  "       memory manager.\n\n");
        io_printf(STDOUT,
		  "       n-max          - The maximum number of allocations\n");
        io_printf(STDOUT,
		  "                      - to perform. Testing goes from 16\n");
        io_printf(STDOUT,
		  "                      - to n-max by doubling.\n");
        io_printf(STDOUT,
		  "                      - For example, 64 would imply tests\n");
        io_printf(STDOUT,
		  "                      - of 16, 32, and 64 allocations.\n\n");
        io_printf(STDOUT,
		  "       max_block_size - The upper limit on the size of\n");
        io_printf(STDOUT,
		  "                      - memory blocks being allocated.\n");
        io_printf(STDOUT,
		  "                      - Default is 100000.\n\n");
        return(1);};

    nm = atoi(argv[1]);
    if (argc == 3)
       max_block_size = atoi(argv[argc-1]);

/* make Klocworks happy */
    nm = min(nm, INT_MAX);

    fp = fopen("logfile", "w");

    fp = NULL;

    io_printf(STDOUT, "\n\nMax Block Size: %ld\n\n", max_block_size);
    for (n = 16; n < nm; n *= 2)
        {io_printf(STDOUT, "\nAllocations: %d\n", n);
         io_printf(STDOUT,
		   "Test\t# frees\t   Time(sec)\tFrac(bytes/free block)\n\n");

         test_mm(_SC_malloc_s, _SC_free_s, n, fp, &time, &mb, &nf);
         io_printf(STDOUT, "%s\t%6d\t   %.3e\t   %.2e\n", "SCORE", nf, time, mb);

         test_mm(_SC_malloc_e, _SC_free_e, n, fp, &time, &mb, &nf);
         io_printf(STDOUT, "%s\t%6d\t   %.3e\t   %.2e\n", "KR-E", nf, time, mb);

         test_mm(_SC_malloc_b, _SC_free_b, n, fp, &time, &mb, &nf);
         io_printf(STDOUT, "%s\t%6d\t   %.3e\t   %.2e\n", "KR-B", nf, time, mb);

         test_mm(malloc, free, n, fp, &time, &mb, &nf);
         io_printf(STDOUT, "%s\t%6d\t   %.3e\t   %.2e\n", "SMM", nf, time, mb);}

/*
    fclose(fp);
*/
    return(0);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

