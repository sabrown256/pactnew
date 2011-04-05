/*
 * PDAPTS.C - append tests for the PDB I/O
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

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

/* TEST_TARGET - set up the target for the data file */

static void test_target(char *tgt, char *base, int n,
		        char *fname, char *datfile)
   {int rv;

    if (tgt != NULL)
       {rv = PD_target_platform(tgt);
	SC_ASSERT(rv == TRUE);

        sprintf(fname, "%s-%s.rs%d", base, tgt, n);
        sprintf(datfile, "%s-%s.db%d", base, tgt, n);}

    else
       {sprintf(fname, "%s-nat.rs%d", base, n);
        sprintf(datfile, "%s-nat.db%d", base, n);};

    return;}

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
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_1_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(1:1,0:4)", ia_w1))
       {PRINT(STDOUT, "IA APPEND 1 FAILED - WRITE_TEST_1_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(2:2,0:4)", ia_w2))
       {PRINT(STDOUT, "IA APPEND 2 FAILED - WRITE_TEST_1_DATA\n");
        exit(1);};

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
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s created\n", datfile);

    prep_test_data();

/* write the test data */
    write_test_1_data(strm);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
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
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_2_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(0:4,1:1)", ia_w1))
       {PRINT(STDOUT, "IA APPEND 1 FAILED - WRITE_TEST_2_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(0:4,2:2)", ia_w2))
       {PRINT(STDOUT, "IA APPEND 2 FAILED - WRITE_TEST_2_DATA\n");
        exit(1);};

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
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s created\n", datfile);

    PD_set_major_order(strm, COLUMN_MAJOR_ORDER);

    prep_test_data();

/* write the test data */
    write_test_2_data(strm);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
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
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_3_DATA\n");
        exit(1);};

    if (!PD_write(strm, "ia(0:0,0:4)", "int", ia_w0))
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_3_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(1:1,0:4)", ia_w1))
       {PRINT(STDOUT, "IA APPEND 1 FAILED - WRITE_TEST_3_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(2:2,0:4)", ia_w2))
       {PRINT(STDOUT, "IA APPEND 2 FAILED - WRITE_TEST_3_DATA\n");
        exit(1);};

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
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s created\n", datfile);

    prep_test_data();

/* write the test data */
    write_test_3_data(strm);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
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
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_4_DATA\n");
        exit(1);};

    if (!PD_write(strm, "ia(0:4,0:0)", "int", ia_w0))
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_4_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(0:4,1:1)", ia_w1))
       {PRINT(STDOUT, "IA APPEND 1 FAILED - WRITE_TEST_4_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(0:4,2:2)", ia_w2))
       {PRINT(STDOUT, "IA APPEND 2 FAILED - WRITE_TEST_4_DATA\n");
        exit(1);};

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
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s created\n", datfile);

    PD_set_major_order(strm, COLUMN_MAJOR_ORDER);

    prep_test_data();

/* write the test data */
    write_test_4_data(strm);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
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
       {PRINT(STDOUT, "IA DEFENT 1 FAILED - WRITE_TEST_5_DATA\n");
        exit(1);};

    if (!PD_write(strm, "ia(0:0,0:4)", "int", ia_w0))
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_5_DATA\n");
        exit(1);};

    if (!PD_defent(strm, "ia(1:2,0:4)", "int"))
       {PRINT(STDOUT, "IA DEFENT 2 FAILED - WRITE_TEST_5_DATA\n");
        exit(1);};

    if (!PD_write(strm, "ia(1:1,0:4)", "int", ia_w1))
       {PRINT(STDOUT, "IA APPEND 1 FAILED - WRITE_TEST_5_DATA\n");
        exit(1);};

    if (!PD_write(strm, "ia(2:2,0:4)", "int", ia_w2))
       {PRINT(STDOUT, "IA APPEND 2 FAILED - WRITE_TEST_5_DATA\n");
        exit(1);};

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
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s created\n", datfile);

    prep_test_data();

/* write the test data */
    write_test_5_data(strm);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
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
       {PRINT(STDOUT, "IA DEFENT 1 FAILED - WRITE_TEST_6_DATA\n");
        exit(1);};

    if (!PD_write(strm, "ia(0:4,0:0)", "int", ia_w0))
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_6_DATA\n");
        exit(1);};

    if (!PD_defent(strm, "ia(0:4,1:2)", "int"))
       {PRINT(STDOUT, "IA DEFENT 2 FAILED - WRITE_TEST_6_DATA\n");
        exit(1);};

    if (!PD_write(strm, "ia(0:4,1:1)", "int", ia_w1))
       {PRINT(STDOUT, "IA APPEND 1 FAILED - WRITE_TEST_6_DATA\n");
        exit(1);};

    if (!PD_write(strm, "ia(0:4,2:2)", "int", ia_w2))
       {PRINT(STDOUT, "IA APPEND 2 FAILED - WRITE_TEST_6_DATA\n");
        exit(1);};

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
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s created\n", datfile);

    PD_set_major_order(strm, COLUMN_MAJOR_ORDER);

    prep_test_data();

/* write the test data */
    write_test_6_data(strm);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s opened\n", datfile);

/* read the data from the file */
    read_test_data(strm);

/* compare the original data with that read in */
    err = compare_test_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
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
   {int i, m, rv, cs, fail;
    long bytaa, bytfa, bytab, bytfb;
    char msg[MAXLINE];
    char *nm;
    double time;
    static int debug = 0;

/* NOTE: under the debugger set debug to 1 or 2 for additional
 *       memory leak monitoring
 */
    cs = SC_mem_monitor(-1, debug, "B", msg);

    SC_mem_stats(&bytab, &bytfb, NULL, NULL);

    time = SC_wall_clock_time();

    fail = 0;

    if (native_only == FALSE)
       {m = PD_target_n_platforms();
	for (i = 0; i < m; i++)
	    {rv = PD_target_platform_n(i);
	     SC_ASSERT(rv == TRUE);

	     nm = PD_target_platform_name(i);
	     if ((*test)(host, nm, n) == FALSE)
	        {PRINT(STDOUT, "Test #%d %s failed\n", n, nm);
		 fail++;};};};

/* native test */
    if ((*test)(host, NULL, n) == FALSE)
       {PRINT(STDOUT, "Test #%d native failed\n", n);
	fail++;};

    SC_mem_stats(&bytaa, &bytfa, NULL, NULL);

    bytaa -= bytab;
    bytfa -= bytfb;
    time   = SC_wall_clock_time() - time;

    cs = SC_mem_monitor(cs, debug, "B", msg);

    PRINT(STDOUT,
          "\t\t     %3d     %7d   %7d   %7d     %.2g\n",
          n, bytaa, bytfa, bytaa - bytfa, time);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nPDAPTS - run PDB defent/append test suite\n\n");
    PRINT(STDOUT, "Usage: pdapts [-d] [-h] [-n] [-v #] [-1] [-2] [-3] [-4] [-5] [-6]\n");
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
   {int i, err;
    int test_one, test_two, test_three, test_four, test_five, test_six;
    int use_mapped_files;

    PD_init_threads(0, NULL);

    SC_zero_space_n(0, -2);
    debug_mode       = FALSE;
    native_only      = FALSE;
    use_mapped_files = FALSE;
    test_one         = TRUE;
    test_two         = TRUE;
    test_three       = TRUE;
    test_four        = TRUE;
    test_five        = TRUE;
    test_six         = TRUE;
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
                      PD_set_format_version(SC_stoi(v[++i]));
		      break;
                 case '1' :
		      test_one = FALSE;
		      break;
                 case '2' :
		      test_two = FALSE;
		      break;
                 case '3' :
		      test_three = FALSE;
		      break;
                 case '4' :
		      test_four = FALSE;
		      break;
                 case '5' :
		      test_five = FALSE;
		      break;
                 case '6' :
		      test_six = FALSE;
		      break;};}
         else
            break;};

    PD_set_io_hooks(use_mapped_files);

    SC_signal(SIGINT, SIG_DFL);

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "\t\t                      Memory                Time\n");
    PRINT(STDOUT, "\t\t                     (bytes)               (secs)\n");
    PRINT(STDOUT, "\t\t     Test  Allocated     Freed      Diff\n");

    err = 0;

    if (test_one)
       err += run_test(test_1, 1, DATFILE);
    if (test_two)
       err += run_test(test_2, 2, DATFILE);
    if (test_three)
       err += run_test(test_3, 3, DATFILE);
    if (test_four)
       err += run_test(test_4, 4, DATFILE);
    if (test_five)
       err += run_test(test_5, 5, DATFILE);
    if (test_six)
       err += run_test(test_6, 6, DATFILE);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
