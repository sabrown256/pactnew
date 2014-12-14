/*
 * TPDC.C - test for the PDB I/O for C99 types
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "nat"

#define N_DOUBLE 3
#define N_BOOL   2
#define N_FLOAT  4

#define FLOAT_EQUAL(d1, d2)  (PM_value_compare(d1, d2, fptol[0]) == 0)
#define DOUBLE_EQUAL(d1, d2) (PM_value_compare(d1, d2, fptol[1]) == 0)
#define QUAD_EQUAL(d1, d2)   (PM_qvalue_compare(d1, d2, fptol[2]) == 0)

typedef int (*PFTest)(char *base, char *tgt, int n);

extern long _PD_lookup_size(char *s, hasharr *tab);

static int
 debug_mode  = FALSE,
 native_only = FALSE;

static bool
 bs_w,
 bs_r,
 ba_w[N_BOOL],
 ba_r[N_BOOL];

static float _Complex
 fs_w,
 fs_r,
 fa_w[N_FLOAT],
 fa_r[N_FLOAT],
 fa2_r[N_FLOAT];

static double _Complex
 ds_w,
 ds_r,
 da_w[N_FLOAT],
 da_r[N_FLOAT];

static long double _Complex
 qs_w,
 qs_r,
 qa_w[N_FLOAT],
 qa_r[N_FLOAT];

/*--------------------------------------------------------------------------*/

/*                            TEST #0 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PRINT_TEST_0_DATA - print it out to STDOUT */

static void print_test_0_data(PDBfile *strm, FILE *fp)
   {int nbp;

/* print scalars */
    nbp = _PD_lookup_size("bool", strm->chart);
    PRINT(fp, "size(bool) = %d\n", nbp);

    nbp = _PD_lookup_size("float_complex", strm->chart);
    PRINT(fp, "size(float complex) = %d\n", nbp);

    nbp = _PD_lookup_size("double_complex", strm->chart);
    PRINT(fp, "size(double complex) = %d\n", nbp);

    nbp = _PD_lookup_size("long_double_complex", strm->chart);
    PRINT(fp, "size(long double complex) = %d\n", nbp);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_0_DATA - compare the test data */

static int compare_test_0_data(PDBfile *strm, FILE *fp)
   {int nbp, nbc, err, err_tot;

    err     = TRUE;
    err_tot = TRUE;

/* compare primitive types */
    nbp  = _PD_lookup_size("bool", strm->host_chart);
    nbc  = sizeof(bool);
    err &= (nbp == nbc);

    nbp  = _PD_lookup_size("float_complex", strm->host_chart);
    nbc  = sizeof(float _Complex);
    err &= (nbp == nbc);

    nbp  = _PD_lookup_size("double_complex", strm->host_chart);
    nbc  = sizeof(double _Complex);
    err &= (nbp == nbc);

    nbp  = _PD_lookup_size("long_double_complex", strm->host_chart);
    nbc  = sizeof(long double _Complex);
    err &= (nbp == nbc);

    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_0 - test PDB calculation of type and struct sizes */

static int test_0(char *base, char *tgt, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       error(1, fp, "Test couldn't create file %s\r\n", datfile);
    PRINT(fp, "File %s created\n", datfile);

/* compare the original data with that read in */
    err = compare_test_0_data(strm, fp);

/* print it out to STDOUT */
    print_test_0_data(strm, fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

    io_close(fp);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #1 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_1_DATA - prepare the test data */

static void prep_test_1_data(void)
   {int i;

/* set scalars */
    bs_r    = 0;
    fs_r    = 0.0;
    ds_r    = 0.0;
    qs_r    = 0.0;

    bs_w    = true;
    fs_w    = 3.14159;
    ds_w    = exp(1.0);
    qs_w    = sqrt(2.0);

/* set bool array */
    ba_w[0] = true;
    ba_w[1] = false;
    ba_r[0] = false;
    ba_r[1] = false;

/* set float complex array */
    for (i = 0; i < N_FLOAT; i++)
        {fa_w[i] = POW((double) (i+1), (double) (i+1)) + i*I;
	 fa_r[i] = 0.0;};

/* set double complex array */
    for (i = 0; i < N_FLOAT; i++)
        {da_w[i] = POW(ds_w, (double) (i+1)) + i*I;
         da_r[i] = 0.0;};

/* set long double complex array */
    for (i = 0; i < N_FLOAT; i++)
        {qa_w[i] = POW(ds_w, (double) (i+1)) + i*I;
         qa_r[i] = 0.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_1 - free all known test data memory */

static void cleanup_test_1(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_1_DATA - write out the data into the PDB file */

static void write_test_1_data(PDBfile *strm)
   {long ind[6];

/* write scalars into the file */
    if (PD_write(strm, "bs", "bool",   &bs_w) == 0)
       error(1, STDOUT, "BS WRITE FAILED - WRITE_TEST_1_DATA\n");
    if (PD_write(strm, "fs", "float_complex", &fs_w) == 0)
       error(1, STDOUT, "FS WRITE FAILED - WRITE_TEST_1_DATA\n");
    if (PD_write(strm, "ds", "double_complex",   &ds_w) == 0)
       error(1, STDOUT, "DS WRITE FAILED - WRITE_TEST_1_DATA\n");
    if (PD_write(strm, "qs", "long_double_complex",  &qs_w) == 0)
       error(1, STDOUT, "QS WRITE FAILED - WRITE_TEST_1_DATA\n");

/* write primitive arrays into the file */
    ind[0] = 0L;
    ind[1] = N_BOOL - 1;
    ind[2] = 1L;
    if (PD_write_alt(strm, "ba", "bool", ba_w, 1, ind) == 0)
       error(1, STDOUT, "BA WRITE FAILED - WRITE_TEST_1_DATA\n");

    ind[0] = 0L;
    ind[1] = N_FLOAT - 1;
    ind[2] = 1L;
    if (PD_write_alt(strm, "fa", "float_complex", fa_w, 1, ind) == 0)
       error(1, STDOUT, "FA WRITE FAILED - WRITE_TEST_1_DATA\n");

    ind[0] = 0L;
    ind[1] = N_FLOAT - 1;
    ind[2] = 1L;
    if (PD_write_alt(strm, "da", "double_complex", da_w, 1, ind) == 0)
       error(1, STDOUT, "DA WRITE FAILED - WRITE_TEST_1_DATA\n");

    ind[0] = 0L;
    ind[1] = N_FLOAT - 1;
    ind[2] = 1L;
    if (PD_write_alt(strm, "qa", "long_double_complex", qa_w, 1, ind) == 0)
       error(1, STDOUT, "QA WRITE FAILED - WRITE_TEST_1_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_1_DATA - read the test data from the file */

static int read_test_1_data(PDBfile *strm)
   {int err;
    long ind[6];

/* read the scalar data from the file */
    err = PD_read(strm, "bs", &bs_r);
    err = PD_read(strm, "fs", &fs_r);
    err = PD_read(strm, "ds", &ds_r);
    err = PD_read(strm, "qs", &qs_r);

/* read the primitive arrays from the file */
    err = PD_read(strm, "ba", ba_r);
    err = PD_read(strm, "fa", fa_r);
    err = PD_read(strm, "da", da_r);
    err = PD_read(strm, "qa", qa_r);

/* partial array read */
    ind[0] = 1;
    ind[1] = 2;
    ind[2] = 1;
    err = PD_read_alt(strm, "fa", fa2_r, ind);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_1_DATA - print it out to STDOUT */

static void print_test_1_data(FILE *fp)
   {int i;

/* print scalars */
    PRINT(fp, "bool scalar:                bs = %d\n", bs_r);
    PRINT(fp, "float complex scalar:       fs = %14.6e + %14.6e*I\n",
	  crealf(fs_r), cimagf(fs_r));
    PRINT(fp, "double complex scalar:      ds = %14.6e + %14.6e*I\n",
	  creal(ds_r), cimag(ds_r));
    PRINT(fp, "long double complex scalar: qs = %14.6e + %14.6e*I\n",
	  CREALL(qs_r), CIMAGL(qs_r));

    PRINT(fp, "\n");

/* print bool array */
    PRINT(fp, "bool array:\n");
    for (i = 0; i < N_BOOL; i++)
        PRINT(fp, "  ba[%d] = %d\n", i, ba_r[i]);

/* print float complex array */
    PRINT(fp, "float complex array:\n");
    for (i = 0; i < N_FLOAT; i++)
        PRINT(fp, "  fa[%d] = %14.6e + %14.6e*I\n",
	      i, crealf(fa_r[i]), cimagf(fa_r[i]));

/* print double complex array */
    PRINT(fp, "double complex array:\n");
    for (i = 0; i < N_FLOAT; i++)
        PRINT(fp, "  da[%d] = %14.6e + %14.6e*I\n",
	      i, creal(da_r[i]), cimag(da_r[i]));

/* print long double complex array */
    PRINT(fp, "long double complex array:\n");
    for (i = 0; i < N_FLOAT; i++)
        PRINT(fp, "  qa[%d] = %14.6le + %14.6le*I\n",
	      i, CREALL(qa_r[i]), CIMAGL(qa_r[i]));

    PRINT(fp, "\n");

/* partial read arrays */
    PRINT(fp, "partial read float complex array:\n");
    for (i = 0; i < N_FLOAT; i++)
        PRINT(fp, "  fa_p[%d] = %14.6e + %14.6e*I\n",
	      i, crealf(fa2_r[i]), cimagf(fa2_r[i]));

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_1_DATA - compare the test data */

static int compare_test_1_data(PDBfile *strm, FILE *fp)
   {int i, err, err_tot;
    long double fptol[N_PRIMITIVE_FP];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

/* compare scalars */
    err  = TRUE;
    err &= (bs_r == bs_w);
    err &= FLOAT_EQUAL(fs_r, fs_w);
    err &= DOUBLE_EQUAL(ds_r, ds_w);
    err &= QUAD_EQUAL(qs_r, qs_w);

    if (err)
       PRINT(fp, "Scalars compare\n");
    else
       PRINT(fp, "Scalars differ\n");
    err_tot &= err;

/* compare arrays */
    err = TRUE;

/* compare bool array */
    for (i = 0; i < N_BOOL; i++)
        err &= (ba_r[i] == ba_w[i]);

/* compare float complex array */
    for (i = 0; i < N_FLOAT; i++)
        err &= FLOAT_EQUAL(fa_r[i], fa_w[i]);

/* compare double complex array */
    for (i = 0; i < N_FLOAT; i++)
        err &= DOUBLE_EQUAL(da_r[i], da_w[i]);

/* compare long double complex array */
    for (i = 0; i < N_FLOAT; i++)
        err &= QUAD_EQUAL(qa_r[i], qa_w[i]);

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");
    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the fundamental PDBLib functions
 *        -
 *        - read and write scalars of all primitive types
 *        - read and write structures with no indirections
 *        - append data to a file and read it back
 *        - read structure members
 *        - read parts of arrays
 *        -
 *        - tests can be targeted
 */

static int test_1(char *base, char *tgt, int n)
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

/* dump the symbol table */
    dump_test_symbol_table(fp, strm->symtab, 1);

/* read the data from the file */
    read_test_1_data(strm);

/* compare the original data with that read in */
    err = compare_test_1_data(strm, fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_1_data(fp);

/* free known test data memory */
    cleanup_test_1();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #2 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_2_DATA - prepare the test data */

static void prep_test_2_data(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_2 - free all know test data memory */

static void cleanup_test_2(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_2_DATA - write out the data into the PDB file */

static void write_test_2_data(PDBfile *strm)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_2_DATA - read the test data from the file */

static int read_test_2_data(PDBfile *strm)
   {int err;

    err = TRUE;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_2_DATA - print it out to the file */

static void print_test_2_data(FILE *fp)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_2_DATA - compare the test data */

static int compare_test_2_data(PDBfile *strm, FILE *fp)
   {int err_tot;

    err_tot = TRUE;

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test the PDBLib functions handling indirections
 *        -
 *        - read and write structures with pointers
 *        - set default array base indexes
 *        - read structure members
 *        - read parts of arrays
 *        -
 *        - tests can be targeted
 */

static int test_2(char *base, char *tgt, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file is asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

    prep_test_2_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = PD_create(datfile);
	if (strm == NULL)
	   error(2, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* write the test data */
	write_test_2_data(strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(2, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       error(2, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* dump the symbol table */
    dump_test_symbol_table(fp, strm->symtab, 2);

/* read the structs */
    read_test_2_data(strm);

/* compare the original data with that read in */
    err = compare_test_2_data(strm, fp);

/* print out the results */
    print_test_2_data(fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(2, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* cleanup test data memory */
    cleanup_test_2();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              DRIVER ROUTINES                             */

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
	    {rv = PD_target_platform_n(i);
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

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDC - run C99 primitive tests\n\n");
    PRINT(STDOUT, "Usage: tpdc [-b #] [-c] [-d] [-h] [-n] [-r] [-v #] [-0] [-1] [-2] [-3] [-4] [-5] [-6] [-7] [-8] [-9] [-10]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       b  - set buffer size (default no buffering)\n");
    PRINT(STDOUT, "       c  - verify low level writes\n");
    PRINT(STDOUT, "       d  - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       n  - run native mode test only\n");
    PRINT(STDOUT, "       r  - read only (assuming files from other run exist)\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "       0  - do NOT run test #0\n");
    PRINT(STDOUT, "       1  - do NOT run test #1\n");
    PRINT(STDOUT, "       2  - do NOT run test #2\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, n, err;
    int ton[3];
    int use_mapped_files, check_writes;
    int64_t bfsz;

    PD_init_threads(0, NULL);

    SC_bf_set_hooks();
    SC_zero_space_n(1, -2);

    bfsz             = -1;
    bfsz             = SC_OPT_BFSZ;
    check_writes     = FALSE;
    debug_mode       = FALSE;
    native_only      = FALSE;
    read_only        = FALSE;
    use_mapped_files = FALSE;

    for (i = 0; i < 3; i++)
        ton[i] = TRUE;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      bfsz = SC_stol(v[++i]);
		      break;
                 case 'c' :
		      check_writes = TRUE;
		      break;
		 case 'd' :
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
                 case 'r' :
		      read_only = TRUE;
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
    PD_verify_writes(check_writes);

    PD_set_buffer_size(bfsz);

    SC_signal(SIGINT, SIG_DFL);

    test_header();

    err = 0;

    if (ton[0])
       err += run_test(test_0, 0, DATFILE, TRUE);
    if (ton[1])
       err += run_test(test_1, 1, DATFILE, native_only);
    if (ton[2])
       err += run_test(test_2, 2, DATFILE, native_only);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
