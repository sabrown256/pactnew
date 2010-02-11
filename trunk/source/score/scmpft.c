/*
 * SCMPFT.C - test the memory manager performance
 *          - see scmtrt.c for correctness testing
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"
#include "scope_mem.h"

#undef SCORE
#undef SYS

#define N_ITER   5
#define N_ITEMS  3
#define SCORE    1
#define SYS      2
#define SM_MAX   4096                /* upper bound of small memory store */

#define ALLOC(_p, _t, _n, _s)                                              \
   {if (mm == SCORE)                                                       \
       {SC_mem_opt opt;                                                    \
        opt.na   = FALSE;                                                  \
        opt.zsp  = _SC_zero_space;                                         \
        opt.typ  = -1;                                                     \
        _p = SC_alloc_nzt(_n, sizeof(_t), _s, &opt);}                      \
    else                                                                   \
       _p = (_t *) malloc((_n)*sizeof(_t));}

#define REALLOC(_p, _sz)                                                   \
   {if (mm == SCORE)                                                       \
       _p = SC_realloc_nz(_p, _sz, 1, FALSE, _SC_zero_space);              \
    else                                                                   \
       _p = realloc(_p, (size_t) _sz);}

#define FREE(_p)                                                           \
   {if (mm == SCORE)                                                       \
       {SFREE(_p);}                                                        \
    else                                                                   \
       {free(_p);                                                          \
        _p = NULL;};}

typedef int (*PFTest)(double *tf, double *nif, int nir);

static int
 mm = SCORE;

double
 mcnv = -1.0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FAST_MEMSET - efficiently zero out a memory block */

static void fast_memset(void *p, long nb)
   {long i, ni, nd;
    double *d;

    if (p != NULL)
       {nd = sizeof(double);
	ni = (nb + nd - 1L) / nd;

	d = (double *) p;
	for (i = 0L; i < ni; i++, *d++ = 0.0);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DS_REALLOC - time downsizing reallocs 
 *            - the scenario here is to test the performance when
 *            - resizing to a smaller space
 *            - we allocate a space the size of the small block
 *            - limit (usually the page size)
 *            - we then realloc it down by 1 byte until it is nominally
 *            - one byte long
 *            - this looks for optimizations that the memory manager
 *            - makes when downsizing
 */

int ds_realloc(double *ptf, double *pnif, int nir)
   {int i, n, m, ni, err;
    char *a;
    double tf, tr;

    err = FALSE;

    ni = 0;
    tf = 0.0;

    for (m = 0; m < nir; m++)
        {n = SM_MAX;
	 ALLOC(a, char, n, "DS_REALLOC:a");

	 tr = SC_wall_clock_time();

	 for (i = n-1; i > 0; i--, ni++)
	     {REALLOC(a, i);};

	 tf += (SC_wall_clock_time() - tr);

	 FREE(a);

	 if (tf > 2.0)
	    break;};

    *ptf  = tf;
    *pnif = ni;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* US_REALLOC - time upsizing reallocs
 *            - the scenario here is to test the performance when
 *            - resizing to a larger space
 *            - we create an array of allocated spaces from 1 byte
 *            - to 3/4 the small block limit (usually the page size)
 *            - we then realloc every other block to be larger by
 *            - the minimum block size as determined by the alignment
 *            - requirement on the system (16 bytes covers most platforms)
 *            - this prevents the memory manager from trivially extending
 *            - the blocks and taking no time to do it
 */

int us_realloc(double *ptf, double *pnif, int nir)
   {int i, n, m, ni, err;
    char **p;
    double tf, tr;

    err = FALSE;

    n = (3*SM_MAX >> 2);
    p = (char **) malloc(n*sizeof(char *));
    if (p == NULL)
       return(TRUE);

    tf = 0.0;
    ni = 0;

    for (m = 0; m < nir; m++)
        {for (i = 1; i < n; i++, ni++)
	     {ALLOC(p[i], char, i, "US_REALLOC:p[i]");};

	 tr = SC_wall_clock_time();

	 for (i = 1; i < n; i += 2, ni++)
	     {REALLOC(p[i], i+16);};

	 tf += (SC_wall_clock_time() - tr);

	 for (i = 1; i < n; i++)
	     {FREE(p[i]);};

	 if (tf > 2.0)
	    break;};

    free(p);

    *ptf  = tf;
    *pnif = ni;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SM_ALLOC - time allocations of small chunks of memory
 *          - examine performance of small memory blocks
 *          - which are managed more efficiently in the SCORE
 *          - memory manager than large blocks
 */

int sm_alloc(double *ptf, double *pnif, int nir)
   {int i, n, m, ni, err, nb;
    char **p;
    double tf, tr;

    err = FALSE;

    p = (char **) malloc(SM_MAX*sizeof(char *));
    if (p == NULL)
       return(TRUE);

    tf = 0.0;
    ni = 0;

    for (m = 0; m < nir; m++)
        {n = SM_MAX;

	 tr = SC_wall_clock_time();

	 for (i = 1; i < n; i++, ni++)
	     {nb = SC_random_int(1, SM_MAX);
	      ALLOC(p[i], char, nb, "SM_ALLOC:p[i]");};

	 tf += (SC_wall_clock_time() - tr);

	 for (i = 1; i < n; i++)
	     {FREE(p[i]);};

	 if (tf > 2.0)
	    break;};

    free(p);

    *ptf  = tf;
    *pnif = ni;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SM_FREE - time free of small chunks of memory
 *          - examine performance of small memory blocks
 *          - which are managed more efficiently in the SCORE
 *          - memory manager than large blocks
 */

int sm_free(double *ptf, double *pnif, int nir)
   {int i, n, m, ni, nb, err;
    char **p;
    double tf, tr;

    err = FALSE;

    p = (char **) malloc(SM_MAX*sizeof(char *));
    if (p == NULL)
       return(TRUE);

    tf = 0.0;
    ni = 0;

    for (m = 0; m < nir; m++)
        {n = SM_MAX;

	 for (i = 1; i < n; i++)
	     {nb = SC_random_int(1, SM_MAX);
	      ALLOC(p[i], char, nb, "SM_FREE:p[i]");};

	 tr = SC_wall_clock_time();

	 for (i = 1; i < n; i++, ni++)
	     {FREE(p[i]);};

	 tf += (SC_wall_clock_time() - tr);

	 if (tf > 2.0)
	    break;};

    free(p);

    *ptf  = tf;
    *pnif = ni;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_RAND_ALLOC - allocate random sized large chunks of memory */

static void lr_rand_alloc(int *pl, int *pni, char **p, int n)
   {int l, nb, ni, full;
    BIGINT nt;
    static int sz = sizeof(double);

    full = TRUE;
    ni   = *pni;

    nt = 0;
    for (l = 1; (l < n) && (nt < 100000000); l++, ni++)
        {nb  = SC_random_int(SM_MAX+1, 1000000);
	 nb  = sz*((nb + sz - 1)/sz);
	 nt += nb;

	 ALLOC(p[l], char, nb, "LR_RAND_ALLOC:p[l]");

/* to defeat cheating lazy memory managers - zero out
 * the memory which will force them to actually allocate
 * the memory pages
 * unfortunately the actual allocation time is hopelessly
 * entangled with the time to set the allocated memory
 */
	 fast_memset(p[l], nb);};

    *pni = ni;
    *pl  = l;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_RAND_FREE - free the random chunks of memory in P */

static void lr_rand_free(char **p, int n)
   {int i;

    for (i = 1; i < n; i++)
        {FREE(p[i]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_ALLOC - time allocations of large chunks of memory
 *          - examine performance of large memory blocks
 *          - which are managed using the system memory manager
 */

int lr_alloc(double *ptf, double *pnif, int nir)
   {int l, m, n, ni, err;
    char **p;
    double tf, tr;

    err = FALSE;

    n = 1024;
    p = (char **) malloc(n*sizeof(char *));
    if (p == NULL)
       return(TRUE);

    tf = 0.0;
    ni = 0;

    for (m = 0; m < nir; m++)
        {tr = SC_wall_clock_time();

	 lr_rand_alloc(&l, &ni, p, n);

	 tf += (SC_wall_clock_time() - tr);

	 lr_rand_free(p, l);

	 if (tf > 2.0)
	    break;};

    free(p);

    *ptf  = tf;
    *pnif = ni;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_FREE - time free of large chunks of memory
 *         - examine performance of large memory blocks
 *         - which are managed using the system memory manager
 */

int lr_free(double *ptf, double *pnif, int nir)
   {int l, m, n, ni, err;
    char **p;
    double tfa, tff, tra, trf;

    err = FALSE;

    n = 1024;
    p = (char **) malloc(n*sizeof(char *));
    if (p == NULL)
       return(TRUE);

    tfa = 0.0;
    tff = 0.0;
    ni  = 0;

    for (m = 0; m < nir; m++)
        {tra = SC_wall_clock_time();

	 lr_rand_alloc(&l, &ni, p, n);

	 trf  = SC_wall_clock_time();
	 tfa += (trf - tra);

	 lr_rand_free(p, l);

	 tff += (SC_wall_clock_time() - trf);

	 if (tfa + tff > 2.0)
	    break;};

    free(p);

    *ptf  = tff;
    *pnif = ni;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CORE - time the test TST */

int core(int nir, PFTest tst, char *s, double *pdt)
   {int err;
    BIGUINT ab, fb, db, mb;
    BIGUINT aa, fa, da, ma;
    double tf, nif, nmby, dt;
    char *r;

    SC_mem_statb(&ab, &fb, &db, &mb);

    err = (*tst)(&tf, &nif, nir);

    SC_mem_statb(&aa, &fa, &da, &ma);

    nmby = (aa - ab);
    r    = " ";
    if (nmby > 1024.0)
       {nmby /= 1024.0;
	r     = "K";};
    if (nmby > 1024.0)
       {nmby /= 1024.0;
	r     = "M";};
    if (nmby > 1024.0)
       {nmby /= 1024.0;
	r     = "G";};
    if (nmby > 1024.0)
       {nmby /= 1024.0;
	r     = "T";};

    dt = tf/nif;

    io_printf(stdout, "\t\t%-18s %8d  %7.2f%s  %.2e\n",
	      s, (int) nif, nmby, r, dt);

    *pdt = dt;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print help message */

static void help(void)
   {

    io_printf(STDOUT, "\n");

    io_printf(STDOUT, "Usage: scmpft [-d #] [-h] [-l <# loops>] [-m #] [-s] [-z #]\n");
    io_printf(STDOUT, "   d    - run with memory history\n");
    io_printf(STDOUT, "   h    - this help message\n");
    io_printf(STDOUT, "   l    - # iterations\n");
    io_printf(STDOUT, "   m    - 1 for SCORE, 2 for system, 3 for both\n");
    io_printf(STDOUT, "   s    - no small block store for SCORE MM\n");
    io_printf(STDOUT, "   z    - reset memory\n");
    io_printf(STDOUT, "        - 0 none, 1 on both, 2 on alloc, 3 on free\n");

    io_printf(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the heap performance */

int main(int c, char **v)
   {int i, err, whc, nt;
    int nir, zsp;
    double tsc[6], tsy[6];
    static char *tn[]  = {"Small alloc", "Small free",
			  "Large alloc", "Large free",
			  "Downsizing realloc", "Upsizing realloc"};
    static PFTest tl[] = {sm_alloc, sm_free, lr_alloc, lr_free,
		          ds_realloc, us_realloc};

    whc = SCORE | SYS;
    nir = 1000;
    zsp = 0;
    nt  = sizeof(tl)/sizeof(PFTest);

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
	    {switch (v[i][1])
		{case 'd' :
		      SC_mem_history(SC_stoi(v[++i]));
		      break;

	         case 'h' :
                      help();
                      return(1);
		      break;

		 case 'l' :
		      nir = SC_stoi(v[++i]);
		      break;

		 case 'm' :
		      whc = SC_stoi(v[++i]);
		      break;

		 case 's' :
		      SC_configure_mm(0L, 0L, 0L, 1.0);
		      break;

		 case 't' :
		      tl[SC_stoi(v[++i])] = NULL;
		      break;

		 case 'z' :
		      zsp = SC_stoi(v[++i]);
		      break;};};};

/* turn this on to test the efficient memset replacement */
    SC_zero_space(zsp);

    io_printf(stdout, "\n");
    io_printf(stdout, "\t\tTest                # calls      Mem  Time/Call\n");

    err = FALSE;
    
    if (whc & SCORE)
       {mm = SCORE;

        io_printf(stdout, "\tScore MM\n");
        for (i = 0; i < nt; i++)
	    {if (tl[i] != NULL)
	        err |= core(nir, tl[i], tn[i], &tsc[i]);};

	io_printf(stdout, "\n");};

    if (whc & SYS)
       {mm = SYS;

        io_printf(stdout, "\tSystem MM\n");
        for (i = 0; i < nt; i++)
	    {if (tl[i] != NULL)
	        err |= core(nir, tl[i], tn[i], &tsy[i]);};

	io_printf(stdout, "\n");};

    if ((whc & SCORE) && (whc & SYS))
       {io_printf(stdout, "\tPerformance Comparison\n");
        io_printf(stdout, "\t\tTest\t\t       SCORE    System   Ratio\n");
        for (i = 0; i < nt; i++)
	    io_printf(stdout, "\t\t%-18s %9.2e %9.2e %7.2f\n",
		      tn[i], tsc[i], tsy[i], tsc[i]/tsy[i]);

	io_printf(stdout, "\n");};

    SC_mem_history_out(NULL, -1);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
