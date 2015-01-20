/*
 * TPDNAN.C - test some NaN handling
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "nat"

/* #define N 6 */
#define N 2

typedef int (*PFTest)(char *base, char *tgt, int n);

static int
 debug_mode  = FALSE,
 native_only = FALSE;

static double
 dw[N],
 dr[N];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PREP_TEST_1_DATA - prepare the test data */

static void prep_test_1_data(void)
   {int i, le;
    char *ca;

    for (i = 0; i < N; i++)
        {dw[i] = 0.0;
	 dr[i] = 0.0;};

    i  = 1;
    ca = (char *) &i;
    le = (ca[0] == 1);

    ca = (char *) dw;

#if (N > 4)

/* plus and minus infinities
 *   positive double 0x7ff0000000000000
 *   negative double 0xfff0000000000000
 */
    if (le == TRUE)
       {ca[6] = 0xf0;
	ca[7] = 0x7f;
	ca += 8;
	ca[6] = 0xf0;
	ca[7] = 0xff;}
    else
       {ca[0] = 0x7f;
	ca[1] = 0xf0;
	ca += 8;
	ca[0] = 0xff;
	ca[1] = 0xf0;};

    ca += 8;

#endif

#if (N > 2)

/* plus and minus quiet NaNs
 *   positive double 0x7ff8000000000000 to 0x7fffffffffffffff
 *   negative double 0xfff8000000000000 to 0xffffffffffffffff
 */
    if (le == TRUE)
       {ca[5] = 0x08;
	ca[6] = 0xf8;
	ca[7] = 0x7f;
	ca += 8;
	ca[5] = 0x08;
	ca[6] = 0xf8;
	ca[7] = 0xff;}
    else
       {ca[0] = 0x7f;
	ca[1] = 0xf8;
	ca[2] = 0x08;
	ca += 8;
	ca[0] = 0xff;
	ca[1] = 0xf8;
	ca[2] = 0x08;};

    ca += 8;

#endif

/* plus and minus signalling NaNs
 *   positive double 0x7ff0000000000001 to 0x7ff7ffffffffffff
 *   negative double 0xfff0000000000001 to 0xfff7ffffffffffff
 */
    if (le == TRUE)
       {ca[1] = 0xff;
	ca[2] = 0xff;
	ca[3] = 0xff;
	ca[4] = 0xff;
	ca[5] = 0xff;
	ca[6] = 0xf7;
	ca[7] = 0x7f;
	ca += 8;
	ca[5] = 0xff;
	ca[6] = 0xf7;
	ca[7] = 0xff;}
    else
       {ca[0] = 0x7f;
	ca[1] = 0xf7;
	ca[2] = 0xff;
	ca += 8;
	ca[0] = 0xff;
	ca[1] = 0xf7;
	ca[2] = 0xff;};

    ca += 8;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_1_DATA - write out the data into the PDB file */

static void write_test_1_data(PDBfile *strm)
   {long ind[3];

    ind[0] = 0L;
    ind[1] = N - 1;
    ind[2] = 1L;

    if (PD_write_alt(strm, "d", "double", dw, 1, ind) == 0)
       error(1, STDOUT, "D WRITE FAILED - WRITE_TEST_1_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_1_DATA - read the test data from the file */

static int read_test_1_data(PDBfile *strm)
   {int err;

/* read the scalar data from the file */
    err = PD_read(strm, "d", &dr);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_1_DATA - compare the test data */

static int compare_test_1_data(PDBfile *strm, FILE *fp)
   {int i, n, err, err_tot;
    char *cw, *cr;

    cw = (char *) dw;
    cr = (char *) dr;

    n = N*sizeof(double);

    err_tot = TRUE;

/* compare scalars */
    err = TRUE;
    for (i = 0; i < n; i++)
        err &= (cr[i] == cw[i]);

    if (err)
       PRINT(fp, "NaNs compare\n");
    else
       PRINT(fp, "NaNs differ\n");
    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/

/*                            TEST #1 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_1 - test writing NaNs and reading them back
 *        - tests can be targeted
 */

int test_1(char *base, char *tgt, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

    prep_test_1_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = PD_create(datfile);
	if (strm == NULL)
	   error(1, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* write the test data */
	write_test_1_data(strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(1, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       error(1, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_1_data(strm);

/* compare the original data with that read in */
    err = compare_test_1_data(strm, fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes
 *          - return the number of tests that fail
 */

static int run_test(PFTest test, int n, char *host, int native)
   {int i, m, rv, fail;
    char *nm;
    tframe st;

    pre_test(&st, debug_mode);

    fail = 0;

    if (native == FALSE)
       {m = PD_target_n_platforms();
	for (i = 0; i < m; i++)

/* skip dos and vax which may be hopeless to test anymore */
	    {if ((i == 7) || (i == 11))
	        continue;

	     rv = PD_target_platform_n(i);
	     SC_ASSERT(rv == TRUE);

	     nm = PD_target_platform_name(i);
	     if ((*test)(host, nm, n) == FALSE)
	        {error(-1, STDOUT, "Test #%d %s failed\n", n, nm);
		 fail++;};};};

/* native test */
    if ((*test)(host, NULL, n) == FALSE)
       {error(-1, STDOUT, "Test #%d native failed\n", n);
	fail++;};

    post_test(&st, n);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, err;

    debug_mode  = FALSE;
    native_only = FALSE;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
	        {case 'd' :
		      debug_mode  = TRUE;
/*		      SC_gs.mm_debug = TRUE; */
		      break;
                 case 'h' :
                      printf("Usage: tpdnan [-h]\n");
		      return(1);
                 case 'n' :
		      native_only = TRUE;
		      break;};};};

    err  = 0;

    err += run_test(test_1, 1, DATFILE, native_only);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
