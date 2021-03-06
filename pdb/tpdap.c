/*
 * TPDAP.C - append tests for the PDB I/O
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "apd"

typedef int (*PFTest)(char *base, char *tgt, int n);

int
 native_only = FALSE,
 debug_mode = FALSE;

int
 ia_w0[5],
 ia_w1[5],
 ia_w2[5],
 ia_r[15];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PREP_TEST_DATA - prepare the test data */

static void prep_test_data(void)
   {int i;

/* set int array */
    for (i = 0; i < 5; i++)
        {ia_w0[i] = i;
	 ia_w1[i] = 10*i;
	 ia_w2[i] = 100*i;};

    for (i = 0; i < 15; i++)
        ia_r[i] = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST - free all known test data memory */

static void cleanup_test(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_DATA - read the test data from the file */

static int read_test_data(PDBfile *strm)
   {int err;

    err = PD_read(strm, "ia", &ia_r);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_DATA - print it out to STDOUT */

static void print_test_data(FILE *fp)
   {int i;

/* print int array */
    PRINT(fp, "integer array:\n");
    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,1) = %d\n", i, ia_r[i]);

    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,2) = %d\n", i, ia_r[i+5]);

    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,3) = %d\n", i, ia_r[i+10]);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_DATA - compare the test data */

static int compare_test_data(PDBfile *strm, FILE *fp)
   {int i, err, err_tot;

    err_tot = TRUE;

/* compare ia */
    err = TRUE;
    for (i = 0; i < 5; i++)
        {err &= (ia_w0[i] == ia_r[i]);};

    for (i = 0; i < 5; i++)
        {err &= (ia_w1[i] == ia_r[i+5]);};

    for (i = 0; i < 5; i++)
        {err &= (ia_w2[i] == ia_r[i+10]);};

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");

    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/

/*                              TEST 1 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* WRITE_TEST_1_DATA - write out the data into the PDB file
 *                   - this is write, append, append
 *                   - row major order
 */

static void write_test_1_data(PDBfile *strm)
   {

/* write primitive arrays into the file */
    if (!PD_write(strm, "ia(1,5)", "int", ia_w0))
       error(-1, stdout, "IA WRITE FAILED - WRITE_TEST_1_DATA\n");

    if (!PD_append(strm, "ia(1:1,0:4)", ia_w1))
       error(-1, stdout, "IA APPEND 1 FAILED - WRITE_TEST_1_DATA\n");

    if (!PD_append(strm, "ia(2:2,0:4)", ia_w2))
       error(-1, stdout, "IA APPEND 2 FAILED - WRITE_TEST_1_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the PDBLib gather functions in ROW_MAJOR order
 *        - tests can be targeted
 */

static int test_1(char *base, char *tgt, int n)
   {PDBfile *strm;
    char datfile[MAXLINE], fname[MAXLINE];
    int err;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       error(-1, fp, "Test couldn't create file %s\r\n", datfile);
    PRINT(fp, "File %s created\n", datfile);

    prep_test_data();

/* write the test data */
    write_test_1_data(strm);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       error(-1, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_data(fp);

/* free known test data memory */
    cleanup_test();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 2 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* WRITE_TEST_2_DATA - write out the data into the PDB file
 *                   - this is write, append, append
 *                   - column major order
 */

static void write_test_2_data(PDBfile *strm)
   {

/* write primitive arrays into the file */
    if (!PD_write(strm, "ia(5,1)", "int", ia_w0))
       error(-1, stdout, "IA WRITE FAILED - WRITE_TEST_2_DATA\n");

    if (!PD_append(strm, "ia(0:4,1:1)", ia_w1))
       error(-1, stdout, "IA APPEND 1 FAILED - WRITE_TEST_2_DATA\n");

    if (!PD_append(strm, "ia(0:4,2:2)", ia_w2))
       error(-1, stdout, "IA APPEND 2 FAILED - WRITE_TEST_2_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test the PDBLib append functions in COLUMN_MAJOR order
 *        - tests can be targeted
 */

static int test_2(char *base, char *tgt, int n)
   {PDBfile *strm;
    char datfile[MAXLINE], fname[MAXLINE];
    int err;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       error(-1, fp, "Test couldn't create file %s\r\n", datfile);
    PRINT(fp, "File %s created\n", datfile);

    PD_set_major_order(strm, COLUMN_MAJOR_ORDER);

    prep_test_data();

/* write the test data */
    write_test_2_data(strm);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       error(-1, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_data(fp);

/* free known test data memory */
    cleanup_test();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 3 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* WRITE_TEST_3_DATA - write out the data into the PDB file
 *                   - this is defent, write, append, append
 *                   - row major order
 */

static void write_test_3_data(PDBfile *strm)
   {

/* write primitive arrays into the file */
    if (!PD_defent(strm, "ia(1,5)", "int"))
       error(-1, stdout, "IA WRITE FAILED - WRITE_TEST_3_DATA\n");

    if (!PD_write(strm, "ia(0:0,0:4)", "int", ia_w0))
       error(-1, stdout, "IA WRITE FAILED - WRITE_TEST_3_DATA\n");

    if (!PD_append(strm, "ia(1:1,0:4)", ia_w1))
       error(-1, stdout, "IA APPEND 1 FAILED - WRITE_TEST_3_DATA\n");

    if (!PD_append(strm, "ia(2:2,0:4)", ia_w2))
       error(-1, stdout, "IA APPEND 2 FAILED - WRITE_TEST_3_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - test the PDBLib scatter functions in ROW_MAJOR order
 *        - tests can be targeted
 */

static int test_3(char *base, char *tgt, int n)
   {PDBfile *strm;
    char datfile[MAXLINE], fname[MAXLINE];
    int err;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       error(-1, fp, "Test couldn't create file %s\r\n", datfile);
    PRINT(fp, "File %s created\n", datfile);

    prep_test_data();

/* write the test data */
    write_test_3_data(strm);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       error(-1, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_data(fp);

/* free known test data memory */
    cleanup_test();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 4 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* WRITE_TEST_4_DATA - write out the data into the PDB file
 *                   - this is defent, write, append, append
 *                   - column major order
 */

static void write_test_4_data(PDBfile *strm)
   {

/* write primitive arrays into the file */
    if (!PD_defent(strm, "ia(5,1)", "int"))
       error(-1, stdout, "IA WRITE FAILED - WRITE_TEST_4_DATA\n");

    if (!PD_write(strm, "ia(0:4,0:0)", "int", ia_w0))
       error(-1, stdout, "IA WRITE FAILED - WRITE_TEST_4_DATA\n");

    if (!PD_append(strm, "ia(0:4,1:1)", ia_w1))
       error(-1, stdout, "IA APPEND 1 FAILED - WRITE_TEST_4_DATA\n");

    if (!PD_append(strm, "ia(0:4,2:2)", ia_w2))
       error(-1, stdout, "IA APPEND 2 FAILED - WRITE_TEST_4_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test the PDBLib scatter functions in COLUMN_MAJOR order
 *        - tests can be targeted
 */

static int test_4(char *base, char *tgt, int n)
   {PDBfile *strm;
    char datfile[MAXLINE], fname[MAXLINE];
    int err;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       error(-1, fp, "Test couldn't create file %s\r\n", datfile);
    PRINT(fp, "File %s created\n", datfile);

    PD_set_major_order(strm, COLUMN_MAJOR_ORDER);

    prep_test_data();

/* write the test data */
    write_test_4_data(strm);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       error(-1, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_data(fp);

/* free known test data memory */
    cleanup_test();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 5 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* WRITE_TEST_5_DATA - write out the data into the PDB file
 *                   - this is defent, write, defent, write, write
 *                   - row major order
 */

static void write_test_5_data(PDBfile *strm)
   {

/* write primitive arrays into the file */
    if (!PD_defent(strm, "ia(1,5)", "int"))
       error(-1, stdout, "IA DEFENT 1 FAILED - WRITE_TEST_5_DATA\n");

    if (!PD_write(strm, "ia(0:0,0:4)", "int", ia_w0))
       error(-1, stdout, "IA WRITE FAILED - WRITE_TEST_5_DATA\n");

    if (!PD_defent(strm, "ia(1:2,0:4)", "int"))
       error(-1, stdout, "IA DEFENT 2 FAILED - WRITE_TEST_5_DATA\n");

    if (!PD_write(strm, "ia(1:1,0:4)", "int", ia_w1))
       error(-1, stdout, "IA APPEND 1 FAILED - WRITE_TEST_5_DATA\n");

    if (!PD_write(strm, "ia(2:2,0:4)", "int", ia_w2))
       error(-1, stdout, "IA APPEND 2 FAILED - WRITE_TEST_5_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_5 - test the PDBLib scatter functions in COLUMN_MAJOR order
 *        - tests can be targeted
 */

static int test_5(char *base, char *tgt, int n)
   {PDBfile *strm;
    char datfile[MAXLINE], fname[MAXLINE];
    int err;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       error(-1, fp, "Test couldn't create file %s\r\n", datfile);
    PRINT(fp, "File %s created\n", datfile);

    prep_test_data();

/* write the test data */
    write_test_5_data(strm);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       error(-1, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_data(fp);

/* free known test data memory */
    cleanup_test();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 6 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* WRITE_TEST_6_DATA - write out the data into the PDB file
 *                   - this is defent, write, defent, write, write
 *                   - column major order
 */

static void write_test_6_data(PDBfile *strm)
   {

/* write primitive arrays into the file */
    if (!PD_defent(strm, "ia(5,1)", "int"))
       error(-1, stdout, "IA DEFENT 1 FAILED - WRITE_TEST_6_DATA\n");

    if (!PD_write(strm, "ia(0:4,0:0)", "int", ia_w0))
       error(-1, stdout, "IA WRITE FAILED - WRITE_TEST_6_DATA\n");

    if (!PD_defent(strm, "ia(0:4,1:2)", "int"))
       error(-1, stdout, "IA DEFENT 2 FAILED - WRITE_TEST_6_DATA\n");

    if (!PD_write(strm, "ia(0:4,1:1)", "int", ia_w1))
       error(-1, stdout, "IA APPEND 1 FAILED - WRITE_TEST_6_DATA\n");

    if (!PD_write(strm, "ia(0:4,2:2)", "int", ia_w2))
       error(-1, stdout, "IA APPEND 2 FAILED - WRITE_TEST_6_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_6 - test the PDBLib scatter functions in COLUMN_MAJOR order
 *        - tests can be targeted
 */

static int test_6(char *base, char *tgt, int n)
   {PDBfile *strm;
    char datfile[MAXLINE], fname[MAXLINE];
    int err;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       error(-1, fp, "Test couldn't create file %s\r\n", datfile);
    PRINT(fp, "File %s created\n", datfile);

    PD_set_major_order(strm, COLUMN_MAJOR_ORDER);

    prep_test_data();

/* write the test data */
    write_test_6_data(strm);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       error(-1, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       error(-1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_data(fp);

/* free known test data memory */
    cleanup_test();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes */

static int run_test(PFTest test, int n, char *host)
   {int i, m, rv, fail;
    char *nm;
    tframe st;

    pre_test(&st, debug_mode);

    fail = 0;

    if (native_only == FALSE)
       {m = PD_target_n_platforms();
	for (i = 0; i < m; i++)
	    {rv = PD_target_platform_n(i);
	     SC_ASSERT(rv == TRUE);

	     nm = PD_target_platform_name(i);
	     if ((*test)(host, nm, n) == FALSE)
	        {error(-1, stdout, "Test #%d %s failed\n", n, nm);
		 fail++;};};};

/* native test */
    if ((*test)(host, NULL, n) == FALSE)
       {error(-1, stdout, "Test #%d native failed\n", n);
	fail++;};

    post_test(&st, n);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDAP - run PDB defent/append test suite\n\n");
    PRINT(STDOUT, "Usage: tpdap [-d] [-h] [-n] [-v #] [-1] [-2] [-3] [-4] [-5] [-6]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       n - run native mode test only\n");
    PRINT(STDOUT, "       v - use specified format version (default 2)\n");
    PRINT(STDOUT, "       1 - do NOT run test #1\n");
    PRINT(STDOUT, "       2 - do NOT run test #2\n");
    PRINT(STDOUT, "       3 - do NOT run test #3\n");
    PRINT(STDOUT, "       4 - do NOT run test #4\n");
    PRINT(STDOUT, "       5 - do NOT run test #5\n");
    PRINT(STDOUT, "       6 - do NOT run test #6\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the various append scenarios */

int main(int c, char **v)
   {int i, n, err;
    int ton[7];
    int use_mapped_files;

    PD_init_threads(0, NULL);

    SC_zero_space_n(1, -2);
    debug_mode       = FALSE;
    native_only      = FALSE;
    use_mapped_files = FALSE;

    for (i = 0; i < 7; i++)
        ton[i] = TRUE;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'd' :
		      debug_mode  = TRUE;
		      SC_gs.mm_debug = TRUE;
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'm' :
		      use_mapped_files = TRUE;
		      break;
                 case 'n' :
		      native_only = TRUE;
		      break;
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;
                 default :
		      n = -SC_stoi(v[i]);
		      ton[n] = FALSE;
		      break;};}
         else
            break;};

    PD_set_io_hooks(use_mapped_files);

    SC_signal(SIGINT, SIG_DFL);

    test_header();

    err = 0;

    if (ton[1])
       err += run_test(test_1, 1, DATFILE);
    if (ton[2])
       err += run_test(test_2, 2, DATFILE);
    if (ton[3])
       err += run_test(test_3, 3, DATFILE);
    if (ton[4])
       err += run_test(test_4, 4, DATFILE);
    if (ton[5])
       err += run_test(test_5, 5, DATFILE);
    if (ton[6])
       err += run_test(test_6, 6, DATFILE);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
