/*
 * SCMFTS.C - test mapped files
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#ifdef HAVE_MMAP

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test memory mapped files
 *        - return TRUE iff successful
 */

static int test_1(void)
   {int i, j, k, n, m, nj, st, lst;
    int64_t mnsz, extsz;
    size_t ni, nw, nis, nws;
    char *bf;
    FILE *fp;
    int64_t msa[] = {10, 100, 4096};
    int64_t esz[] = {10, 100, 4096};
    double mlt[] = {0.5, 0.89, 1.0, 1.31, 1.5};

    SC_mf_set_hooks();

    n  = sizeof(msa)/sizeof(int64_t);
    nj = sizeof(mlt)/sizeof(double);

    m  = 3*msa[n-1]/2;
    bf = CMAKE_N(char, m);

    io_printf(stdout, "                       Mapped write    ");

    st = TRUE;
    for (i = 0; i < n; i++)
        {mnsz  = msa[i];
	 extsz = esz[i];

	 SC_mf_set_size(mnsz, extsz);

/* do succession of files in which 5 blocks of data are written
 * and the block sizes are: 1/2, 1, 3/2 times the initial/extend
 * size
 * this will test writing and extending files
 */
	 for (j = 0; j < nj; j++)
	     {ni = mlt[j]*mnsz;

	      fp = io_open("mft.1", "w");

	      lst = TRUE;
	      nis = 0L;
	      nws = 0L;
	      for (k = 0; k < 5; k++)
		  {nw = io_write(bf, 1, ni, fp);
		   if (nw != ni)
		      lst = FALSE;

		   nws += nw;
		   nis += ni;};

              if ((lst == TRUE) && (nis == nws))
		 io_printf(stdout, ".");
	      else
		 {io_printf(stdout, "x");
		  st = FALSE;};

	      io_close(fp);};

	 io_printf(stdout, " ");};

    if (st == TRUE)
       io_printf(stdout, " OK\n");
    else
       io_printf(stdout, " FAIL\n");

    CFREE(bf);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test alternate memory mapped files (64 bit on 32 bit OS)
 *        - return TRUE iff successful
 */

static int test_2(void)
   {int st;

#ifdef HAVE_ALT_LARGE_FILE

    int i, j, k, n, m, nj, lst;
    int64_t mnsz, extsz;
    size_t ni, nw, nis, nws;
    char *bf;
    FILE *fp;
    int64_t msa[] = {10, 100, 4096};
    int64_t esz[] = {10, 100, 4096};
    double mlt[] = {0.5, 0.89, 1.0, 1.31, 1.5};

    SC_lmf_set_hooks();

    n  = sizeof(msa)/sizeof(int64_t);
    nj = sizeof(mlt)/sizeof(double);

    m  = 3*msa[n-1]/2;
    bf = CMAKE_N(char, m);

    io_printf(stdout, "                       Alternate write ");

    st = TRUE;
    for (i = 0; i < n; i++)
        {mnsz  = msa[i];
	 extsz = esz[i];

	 SC_mf_set_size(mnsz, extsz);

/* do succession of files in which 5 blocks of data are written
 * and the block sizes are: 1/2, 1, 3/2 times the initial/extend
 * size
 * this will test writing and extending files
 */
	 for (j = 0; j < nj; j++)
	     {ni = mlt[j]*mnsz;

	      fp = lio_open("mft.2", "w");

	      lst = TRUE;
	      nis = 0L;
	      nws = 0L;
	      for (k = 0; k < 5; k++)
		  {nw = lio_write(bf, 1, ni, fp);
		   if (nw != ni)
		      lst = FALSE;

		   nws += nw;
		   nis += ni;};

              if ((lst == TRUE) && (nis == nws))
		 io_printf(stdout, ".");
	      else
		 {io_printf(stdout, "x");
		  st = FALSE;};

	      lio_close(fp);};

	 io_printf(stdout, " ");};

    if (st == TRUE)
       io_printf(stdout, " OK\n");
    else
       io_printf(stdout, " FAIL\n");

    CFREE(bf);

#else

    st = TRUE;

#endif

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - test editing memory mapped files
 *        - return TRUE iff successful
 */

static int test_3(void)
   {int i, m, st;
    size_t nr, nw;
    int64_t od, oi, ln;
    char *src, *dst, *ps, *pd;
    FILE *fp;
    SC_file_block *bl;

    SC_mf_set_hooks();

    m   = 200;
    dst = CMAKE_N(char, m);
    src = CMAKE_N(char, m);
    for (i = 0; i < m; i++)
        src[i] = i + 1;

    io_printf(stdout, "                       Mapped edit    .................. ");

    st = TRUE;

    fp = io_open("mft.3", "w");
    nw = io_write(src, 1, m, fp);
    SC_ASSERT(nw == m);

    od = 20;
    oi = 100;
    ln = 10;

    bl = SC_mf_delete(fp, od, ln);
    SC_mf_insert(fp, bl, oi);

    io_seek(fp, 0, SEEK_SET);
    nr = io_read(dst, 1, m, fp);
    SC_ASSERT(nr == m);

    ps = src + od;
    pd = dst + oi;
    st = TRUE;
    for (i = 0; i < ln; i++)
        {if (ps[i] != pd[i])
	    st = FALSE;};

    io_close(fp);

    if (st == TRUE)
       io_printf(stdout, " OK\n");
    else
       io_printf(stdout, " FAIL\n");

    CFREE(src);
    CFREE(dst);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test editing alternate memory mapped files (64 bit on 32 bit OS)
 *        - return TRUE iff successful
 */

static int test_4(void)
   {int st;

#ifdef HAVE_ALT_LARGE_FILE

    int i, m;
    size_t nr, nw;
    int64_t od, oi, ln;
    char *src, *dst, *ps, *pd;
    FILE *fp;
    SC_file_block *bl;

    SC_lmf_set_hooks();

    m   = 200;
    dst = CMAKE_N(char, m);
    src = CMAKE_N(char, m);
    for (i = 0; i < m; i++)
        src[i] = i + 1;

    io_printf(stdout, "                       Alternate edit .................. ");

    st = TRUE;

    fp = lio_open("mft.4", "w");
    nw = lio_write(src, 1, m, fp);

    od = 20;
    oi = 100;
    ln = 10;

    bl = SC_mf_delete(fp, od, ln);
    SC_mf_insert(fp, bl, oi);

    lio_seek(fp, 0, SEEK_SET);
    nr = lio_read(dst, 1, m, fp);

    ps = src + od;
    pd = dst + oi;
    st = TRUE;
    for (i = 0; i < ln; i++)
        {if (ps[i] != pd[i])
	    st = FALSE;};

    lio_close(fp);

    if (st == TRUE)
       io_printf(stdout, " OK\n");
    else
       io_printf(stdout, " FAIL\n");

    CFREE(src);
    CFREE(dst);

#else

    st = TRUE;

#endif

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test mapped files */

int main(int c, char **v)
   {int i, rv, st;
    int t1, t2, t3, t4;

    SC_setbuf(stdout, NULL);

/* process the command line arguments */
    t1 = TRUE;
    t2 = TRUE;
    t3 = TRUE;
    t4 = TRUE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case '1' :
		      t1 = FALSE;
		      break;
                 case '2' :
		      t2 = FALSE;
		      break;
                 case '3' :
		      t3 = FALSE;
		      break;
                 case '4' :
		      t4 = FALSE;
		      break;

	         case 'h' :
		      printf("\nUsage: scmfts [-1] [-2] [-3] [-4] [-h]\n");
		      printf("   Options:\n");
		      printf("      1 - do not do mapped write test\n");
		      printf("      2 - do not do alternate write test\n");
		      printf("      3 - do not do mapped edit test\n");
		      printf("      4 - do not do alternate edit test\n");
		      printf("      h - print this help message\n");
		      printf("\n");
		      exit(1);};}
         else
            break;};

    st = TRUE;

    io_printf(stdout, "\n");
    io_printf(stdout, "                       Data type byte sizes:\n");
    io_printf(stdout, "                          int       = %d\n", sizeof(int));
    io_printf(stdout, "                          long      = %d\n", sizeof(long));
    io_printf(stdout, "                          long long = %d\n", sizeof(long long));
    io_printf(stdout, "                          size_t    = %d\n", sizeof(size_t));
    io_printf(stdout, "                          int64_t   = %d\n", sizeof(int64_t));
    io_printf(stdout, "\n");

#if defined(NEED_ALT_LARGE_FILE) && !defined(HAVE_ALT_LARGE_FILE)    
    io_printf(stdout, "\n");
    io_printf(stdout, "                       This platform does not support the alternate\n");
    io_printf(stdout, "                       memory mapped file scheme (mmap64)\n");
    io_printf(stdout, "\n");
#endif

    if (t1 == TRUE)
       st &= test_1();

    if (t2 == TRUE)
       st &= test_2();

    if (t3 == TRUE)
       st &= test_3();

    if (t4 == TRUE)
       st &= test_4();

    io_printf(stdout, "\n");

/* reverse the sense of the test to exit with 0 iff successful */
    rv = (st == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* MAIN - stub test mapped files */

int main(int c, char **v)
   {

    fprintf(stdout, "\n\t\t\tNot available on this platform\n\n");

    return(0);}

/*--------------------------------------------------------------------------*/

#endif
