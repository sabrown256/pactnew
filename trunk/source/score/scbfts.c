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
   {int i, n, st, bpi, ni, cs;
    int *iv;
    char msg[MAXLINE];
    FILE *fp;
    static int dbg = 0;

    cs = SC_mem_monitor(-1, dbg, "T1", msg);

    st  = TRUE;
    n   = 1000;
    bpi = sizeof(int);

    iv = FMAKE_N(int, n, "TEST_1:iv");
    for (i = 0; i < n; i++)
        iv[i] = i;

    fp = SC_bopen("ts.dat", "w");

    io_setvbuf(fp, NULL, _IOFBF, 10*bpi);

    ni = 0;

/* write two contiguous blocks larger than the buffer size */
    ni += io_write(iv+ni, bpi, 11, fp);
    ni += io_write(iv+ni, bpi, 11, fp);

/* write two contiguous blocks smaller than the buffer size */
    ni += io_write(iv+ni, bpi, 9, fp);
    ni += io_write(iv+ni, bpi, 9, fp);

/* seek ahead out of buffer */
    io_seek(fp, ni+2, SEEK_SET);

/* write two contiguous blocks smaller than the buffer size */
    ni += io_write(iv+ni, bpi, 9, fp);
    ni += io_write(iv+ni, bpi, 9, fp);

/* write two contiguous blocks larger than the buffer size */
    ni += io_write(iv+ni, bpi, 12, fp);
    ni += io_write(iv+ni, bpi, 13, fp);

/* seek ahead in buffer */
    io_seek(fp, ni+2, SEEK_SET);

/* write two contiguous blocks larger than the buffer size */
    ni += io_write(iv+ni, bpi, 11, fp);
    ni += io_write(iv+ni, bpi, 11, fp);

/* write two contiguous blocks smaller than the buffer size */
    ni += io_write(iv+ni, bpi, 9, fp);
    ni += io_write(iv+ni, bpi, 9, fp);

    io_close(fp);

    SFREE(iv);

    cs = SC_mem_monitor(cs, dbg, "T1", msg);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - simple sequential read test (using file from test_1)
 *        - return TRUE iff successful
 */

static int test_2(void)
   {int i, n, st, bpi, ni, cs;
    int *iv;
    off_t addr;
    char msg[MAXLINE];
    FILE *fp;
    static int dbg = 0;

    cs = SC_mem_monitor(-1, dbg, "T1", msg);

    st  = TRUE;
    n   = 1000;
    bpi = sizeof(int);

    iv = FMAKE_N(int, n, "TEST_1:iv");
    for (i = 0; i < n; i++)
        iv[i] = 0;

    fp = SC_bopen("ts.dat", "r");

    io_setvbuf(fp, NULL, _IOFBF, 10*bpi);

    addr = io_tell(fp);

    ni = 0;

/* read two contiguous blocks larger than the buffer size */
    ni += io_read(iv+ni, bpi, 11, fp);
    ni += io_read(iv+ni, bpi, 11, fp);

/* read two contiguous blocks smaller than the buffer size */
    ni += io_read(iv+ni, bpi, 9, fp);
    ni += io_read(iv+ni, bpi, 9, fp);

/* seek ahead out of buffer */
    io_seek(fp, ni+2, SEEK_SET);

/* read two contiguous blocks smaller than the buffer size */
    ni += io_read(iv+ni, bpi, 9, fp);
    ni += io_read(iv+ni, bpi, 9, fp);

/* read two contiguous blocks larger than the buffer size */
    ni += io_read(iv+ni, bpi, 12, fp);
    ni += io_read(iv+ni, bpi, 13, fp);

/* seek ahead in buffer */
    io_seek(fp, ni+2, SEEK_SET);

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
        st &= (iv[i] == i);

    SFREE(iv);

    cs = SC_mem_monitor(cs, dbg, "T1", msg);

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - test editing memory mapped files
 *        - return TRUE iff successful
 */

static int test_3(void)
   {int st;

    st = TRUE;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test editing alternate memory mapped files (64 bit on 32 bit OS)
 *        - return TRUE iff successful
 */

static int test_4(void)
   {int st;

    st = TRUE;

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
		      printf("\nUsage: scbfts [-1] [-2] [-3] [-4] [-h]\n");
		      printf("   Options:\n");
		      printf("      1 - do not do simple write test\n");
		      printf("      2 - do not do simple read test\n");
		      printf("      3 - do not do mapped edit test\n");
		      printf("      4 - do not do alternate edit test\n");
		      printf("      h - print this help message\n");
		      printf("\n");
		      exit(1);};}
         else
            break;};

    st = TRUE;

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
/*--------------------------------------------------------------------------*/
