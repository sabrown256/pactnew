/*
 * SCBFTS.C - test buffered I/O
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - simple sequential write test
 *        - return TRUE iff successful
 */

static int test_1(void)
   {int i, n, st, bpi, ni, bu, cs;
    int *iv;
    char msg[MAXLINE];
    FILE *fp;
    static int dbg = 0;

    cs = SC_mem_monitor(-1, dbg, "T1", msg);

    st  = TRUE;
    n   = 1000;
    bpi = sizeof(int);
    bu  = 10;

    iv = CMAKE_N(int, n);
    for (i = 0; i < n; i++)
        iv[i] = i+1;

    fp = SC_bopen("ts1.dat", "w");

    io_setvbuf(fp, NULL, _IOFBF, bu*bpi);

    ni = 0;

/* write two contiguous blocks larger than the buffer size */
    ni += io_write(iv+ni, bpi, 11, fp);
    ni += io_write(iv+ni, bpi, 11, fp);

/* write two contiguous blocks smaller than the buffer size */
    ni += io_write(iv+ni, bpi, 9, fp);
    ni += io_write(iv+ni, bpi, 9, fp);

/* seek ahead out of buffer */
    io_seek(fp, bpi*ni+2, SEEK_SET);

/* write two contiguous blocks smaller than the buffer size */
    ni += io_write(iv+ni, bpi, 9, fp);
    ni += io_write(iv+ni, bpi, 9, fp);

/* write two contiguous blocks larger than the buffer size */
    ni += io_write(iv+ni, bpi, 12, fp);
    ni += io_write(iv+ni, bpi, 13, fp);

/* seek ahead in buffer */
    io_seek(fp, bpi*ni+2, SEEK_SET);

/* write two contiguous blocks larger than the buffer size */
    ni += io_write(iv+ni, bpi, 11, fp);
    ni += io_write(iv+ni, bpi, 11, fp);

/* write two contiguous blocks smaller than the buffer size */
    ni += io_write(iv+ni, bpi, 9, fp);
    ni += io_write(iv+ni, bpi, 9, fp);

    io_close(fp);

    CFREE(iv);

    st = (ni == 123);
    io_printf(stdout, "\t\t\twrite seq ................. %s\n",
	      (st == TRUE) ? "ok" : "ng");

    cs = SC_mem_monitor(cs, dbg, "T1", msg);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - simple sequential read test (using file from test_1)
 *        - return TRUE iff successful
 */

static int test_2(void)
   {int i, n, st, bpi, ni, bu, cs;
    int *iv;
    int64_t addr;
    char msg[MAXLINE];
    FILE *fp;
    static int dbg = 0;

    cs = SC_mem_monitor(-1, dbg, "T1", msg);

    st  = TRUE;
    n   = 1000;
    bpi = sizeof(int);
    bu  = 10;

    iv = CMAKE_N(int, n);
    for (i = 0; i < n; i++)
        iv[i] = 0;

    fp = SC_bopen("ts1.dat", "r");

    io_setvbuf(fp, NULL, _IOFBF, bu*bpi);

    addr = io_tell(fp);
    SC_ASSERT(addr == 0);

    ni = 0;

/* read two contiguous blocks larger than the buffer size */
    ni += io_read(iv+ni, bpi, 11, fp);
    ni += io_read(iv+ni, bpi, 11, fp);

/* read two contiguous blocks smaller than the buffer size */
    ni += io_read(iv+ni, bpi, 9, fp);
    ni += io_read(iv+ni, bpi, 9, fp);

/* seek ahead out of buffer */
    io_seek(fp, bpi*ni+2, SEEK_SET);

/* read two contiguous blocks smaller than the buffer size */
    ni += io_read(iv+ni, bpi, 9, fp);
    ni += io_read(iv+ni, bpi, 9, fp);

/* read two contiguous blocks larger than the buffer size */
    ni += io_read(iv+ni, bpi, 12, fp);
    ni += io_read(iv+ni, bpi, 13, fp);

/* seek ahead in buffer */
    io_seek(fp, bpi*ni+2, SEEK_SET);

/* read two contiguous blocks larger than the buffer size */
    ni += io_read(iv+ni, bpi, 11, fp);
    ni += io_read(iv+ni, bpi, 11, fp);

/* read two contiguous blocks smaller than the buffer size */
    ni += io_read(iv+ni, bpi, 9, fp);
    ni += io_read(iv+ni, bpi, 9, fp);

    io_close(fp);

/* check the results */
    st = TRUE;
    for (i = 0; i < ni; i++)
        st &= (iv[i] == i+1);

    io_printf(stdout, "\t\t\tread seq .................. %s\n",
	      (st == TRUE) ? "ok" : "ng");

    CFREE(iv);

    cs = SC_mem_monitor(cs, dbg, "T1", msg);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - test "random" writes
 *        - return TRUE iff successful
 */

static int test_3(void)
   {int i, n, st, bpi, ni, nw, nu, ad, bu, cs;
    int *iv;
    char msg[MAXLINE];
    FILE *fp;
    static long addr[] = { 8, 2, 5, 3, 9, 1, 7, 4, 6, 0 };
    static int dbg = 0;

    cs = SC_mem_monitor(-1, dbg, "T3", msg);

    st  = TRUE;
    n   = 1000;
    bpi = sizeof(int);
    bu  = 3;

    iv = CMAKE_N(int, n);
    for (i = 0; i < n; i++)
        iv[i] = i+1;

    ni = 0;
    nw = sizeof(addr)/sizeof(long);
    nu = 10;

    fp = SC_bopen("ts2.dat", "w");

    io_setvbuf(fp, NULL, _IOFBF, bu*bpi);

    for (i = 0; i < nw; i++)
        {ad = nu*addr[i];
	 io_seek(fp, ad*bpi, SEEK_SET);
	 ni += io_write(iv+ad, bpi, nu, fp);};

    io_close(fp);

    CFREE(iv);

    st = (ni == nu*nw);
    io_printf(stdout, "\t\t\twrite rnd ................. %s\n",
	      (st == TRUE) ? "ok" : "ng");

    cs = SC_mem_monitor(cs, dbg, "T3", msg);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test random reads from file of test_3
 *        - return TRUE iff successful
 */

static int test_4(void)
   {int i, n, st, bpi, ni, nw, nu, ad, bu, cs;
    int *iv;
    char msg[MAXLINE];
    FILE *fp;
    static long addr[] = { 4, 7, 6, 8, 2, 5, 9, 3, 0, 1 };
    static int dbg = 0;

    cs = SC_mem_monitor(-1, dbg, "T4", msg);

    st  = TRUE;
    n   = 1000;
    bpi = sizeof(int);
    bu  = 10;

    iv = CMAKE_N(int, n);
    for (i = 0; i < n; i++)
        iv[i] = 0;

    ni = 0;
    nw = sizeof(addr)/sizeof(long);
    nu = 7;

    fp = SC_bopen("ts2.dat", "r");

    io_setvbuf(fp, NULL, _IOFBF, bu*bpi);

    for (i = 0; i < nw; i++)
        {ad = nu*addr[i];
	 io_seek(fp, ad*bpi, SEEK_SET);
	 ni += io_read(iv+ad, bpi, nu, fp);};

    io_close(fp);

/* check the results */
    st = TRUE;
    for (i = 0; i < ni; i++)
        st &= (iv[i] == i+1);

    CFREE(iv);

    io_printf(stdout, "\t\t\tread rnd .................. %s\n",
	      (st == TRUE) ? "ok" : "ng");

    cs = SC_mem_monitor(cs, dbg, "T4", msg);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_5 - test backpedaling read from test_3 file
 *        - return TRUE iff successful
 */

static int test_5(void)
   {int i, n, st, bpi, ni, nw, ad, bu, cs;
    int *iv;
    char msg[MAXLINE];
    FILE *fp;
/*
    static long addr[] = { 0, 99, 79, 80 };
    static long nu[]   = { 8, 8, 8, 10 };
*/
    static long addr[] = { 0, 75, 62, 53 };
    static long nu[] = { 100, 14, 26, 34 };
    static int dbg = 0;

    cs = SC_mem_monitor(-1, dbg, "T5", msg);

    st  = TRUE;
    n   = 100;
    bpi = sizeof(int);
    bu  = 10;

    iv = CMAKE_N(int, n);
    for (i = 0; i < n; i++)
        iv[i] = 0;

    ni = 0;
    nw = sizeof(addr)/sizeof(long);

    fp = SC_bopen("ts2.dat", "r");

    io_setvbuf(fp, NULL, _IOFBF, bu*bpi);

    st = TRUE;

    for (i = 0; (i < nw) && (st == TRUE); i++)
        {ad = addr[i];
	 io_seek(fp, ad*bpi, SEEK_SET);
	 ni += io_read(iv+ad, bpi, nu[i], fp);
         st &= SC_check_file(fp);};

    io_close(fp);

/* check the results */
    for (i = 0; (i < n) && (st == TRUE); i++)
        st &= ((iv[i] == 0) || (iv[i] == i+1));

    CFREE(iv);

    io_printf(stdout, "\t\t\tread bck .................. %s\n",
	      (st == TRUE) ? "ok" : "ng");

    cs = SC_mem_monitor(cs, dbg, "T5", msg);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test mapped files */

int main(int c, char **v)
   {int i, rv, st;
    int ts[5];

    SC_setbuf(stdout, NULL);

    for (i = 0; i < 5; i++)
        ts[i] = TRUE;

/* process the command line arguments */
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case '1' :
		      ts[0] = FALSE;
		      break;
                 case '2' :
		      ts[1] = FALSE;
		      break;
                 case '3' :
		      ts[2] = FALSE;
		      break;
                 case '4' :
		      ts[3] = FALSE;
		      break;
                 case '5' :
		      ts[4] = FALSE;
		      break;

	         case 'h' :
		      printf("\nUsage: scbfts [-1] [-2] [-3] [-4] [-5] [-h]\n");
		      printf("   Options:\n");
		      printf("      1 - do not do seq write test\n");
		      printf("      2 - do not do seq read test\n");
		      printf("      3 - do not do rnd write test\n");
		      printf("      4 - do not do rnd read test\n");
		      printf("      4 - do not do bck read test\n");
		      printf("      h - print this help message\n");
		      printf("\n");
		      exit(1);};}
         else
            break;};

    st = TRUE;

    if (ts[0] == TRUE)
       st &= test_1();

    if (ts[1] == TRUE)
       st &= test_2();

    if (ts[2] == TRUE)
       st &= test_3();

    if (ts[3] == TRUE)
       st &= test_4();

    if (ts[4] == TRUE)
       st &= test_5();

    io_printf(stdout, "\n");

/* reverse the sense of the test to exit with 0 iff successful */
    rv = (st == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
