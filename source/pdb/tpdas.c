/*
 * TPDAS.C - type definition tests for the PDB I/O
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "nat"

#define N_DOUBLE 3
#define N_INT    5
#define N_CHAR  10
#define N_FLOAT  4

#define FLOAT_EQUAL(d1, d2)  (PM_value_compare(d1, d2, fptol[0]) == 0)
#define DOUBLE_EQUAL(d1, d2) (PM_value_compare(d1, d2, fptol[1]) == 0)

typedef int (*PFTest)(char *base, char *tgt, int n);

static short
 ss_w,
 ss_r,
 sa_w[N_INT],
 sa_r[N_INT];

static int
 is_w,
 is_r,
 ia_w[N_INT],
 ia_r[N_INT];

static long
 ls_w,
 ls_r,
 la_w[N_INT],
 la_r[N_INT];

static float
 fs_w,
 fs_r,
 fa2_w[N_FLOAT][N_DOUBLE],
 fa2_r[N_FLOAT][N_DOUBLE];

static double
 ds_w,
 ds_r,
 da_w[N_FLOAT],
 da_r[N_FLOAT];

static int
 debug_mode  = FALSE,
 native_only = FALSE,
 ord_int8[]  = {1},
 ord_int24[] = {1, 3, 2};

/*--------------------------------------------------------------------------*/

/*                            TEST #1 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_1_DATA - prepare the test data */

static void prep_test_1_data(void)
   {int i, k;

/* set scalars */
    ss_r = 0;
    is_r = 0;
    fs_r = 0.0;
    ds_r = 0.0;

    ss_w = -51;
    is_w =  10;
    fs_w =  3.14159;
    ds_w =  exp(1.0);

/* set short array */
    for (i = 0; i < N_INT; i++)
        {sa_w[i] = 2 - i;
         sa_r[i] = 0;};

/* set int array */
    for (i = 0; i < N_INT; i++)
        {ia_w[i] = i - 2;
         ia_r[i] = 0;};

/* set float array */
    for (i = 0; i < N_FLOAT; i++)
        for (k = 0; k < N_DOUBLE; k++)
            {fa2_w[i][k] = (float)i;
             fa2_r[i][k] = 0.0;};

/* set double array */
    for (i = 0; i < N_FLOAT; i++)
        {da_w[i] = POW(ds_w, (double) (i+1));
         da_r[i] = 0.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_1_DATA - write out the data into the PDB file */

static void write_test_1_data(PDBfile *strm)
   {

/* write scalars into the file */
    PD_write_as(strm, "ss", "short",   "int8",  &ss_w);
    PD_write_as(strm, "is", "enum",    "int40", &is_w);
    PD_write_as(strm, "fs", "float",   "fp8",   &fs_w);
    PD_write_as(strm, "ds", "double",  "fp24",  &ds_w);

/* write primitive arrays into the file */
    PD_write_as(strm, "sa(5)",    "short",   "int40", sa_w);
    PD_write_as(strm, "ia(5)",    "integer", "int40", ia_w);
    PD_write_as(strm, "fa2(4,3)", "float",   "fp8",  fa2_w);
    PD_write_as(strm, "da(4)",    "double",  "fp24", da_w);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_1_DATA - read the test data from the file */

static void read_test_1_data(PDBfile *strm)
   {

/* read the scalar data from the file */
    PD_read_as(strm, "ss", "short",   &ss_r);
    PD_read_as(strm, "is", "integer", &is_r);
    PD_read_as(strm, "fs", "float",   &fs_r);
    PD_read_as(strm, "ds", "double",  &ds_r);

/* read the primitive arrays from the file */
    PD_read_as(strm, "sa",  "short",   sa_r);
    PD_read_as(strm, "ia",  "integer", ia_r);
    PD_read_as(strm, "fa2", "float",   fa2_r);
    PD_read_as(strm, "da",  "double",  da_r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_1_DATA - print it out to STDOUT */

static void print_test_1_data(FILE *fp)
   {int i, k;

/* print scalars */
    PRINT(fp, "short scalar:   ss = %d\n", ss_r);
    PRINT(fp, "integer scalar: is = %d\n", is_r);
    PRINT(fp, "float scalar:   fs = %14.6e\n", fs_r);
    PRINT(fp, "double scalar:  ds = %14.6e\n", ds_r);

    PRINT(fp, "\n");

/* print short array */
    PRINT(fp, "short array:\n");
    for (i = 0; i < N_INT; i++)
        PRINT(fp, "  sa[%d] = %d\n", i, sa_r[i]);

/* print int array */
    PRINT(fp, "integer array:\n");
    for (i = 0; i < N_INT; i++)
        PRINT(fp, "  ia[%d] = %d\n", i, ia_r[i]);

/* print float array */
    PRINT(fp, "float array:\n");
    for (i = 0; i < N_FLOAT; i++)
        for (k = 0; k < N_DOUBLE; k++)
            PRINT(fp, "  fa2[%d][%d] = %14.6e\n", i, k, fa2_r[i][k]);

/* print double array */
    PRINT(fp, "double array:\n");
    for (i = 0; i < N_FLOAT; i++)
        PRINT(fp, "  da[%d] = %14.6e\n", i, da_r[i]);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_1_DATA - compare the test data */

static int compare_test_1_data(PDBfile *strm, FILE *fp)
   {int i, k, err, err_tot;
    long double fptol[3];

    fptol[0] = 1.0e-2;
    fptol[1] = 1.0e-5;

    err_tot = TRUE;

/* compare scalars */
    err  = TRUE;
    err &= (ss_r == ss_w);
    err &= (is_r == is_w);
    err &= FLOAT_EQUAL(fs_r, fs_w);
    err &= DOUBLE_EQUAL(ds_r, ds_w);

    if (err)
       PRINT(fp, "Scalars compare\n");
    else
       PRINT(fp, "Scalars differ\n");
    err_tot &= err;

/* compare short array */
    for (i = 0; i < N_INT; i++)
        err &= (sa_r[i] == sa_w[i]);

/* compare int array */
    for (i = 0; i < N_INT; i++)
        err &= (ia_r[i] == ia_w[i]);

/* compare float array */
    for (i = 0; i < N_FLOAT; i++)
        for (k = 0; k < N_DOUBLE; k++)
            {err &= FLOAT_EQUAL(fa2_r[i][k], fa2_w[i][k]);};

/* compare double array */
    for (i = 0; i < N_FLOAT; i++)
        err &= DOUBLE_EQUAL(da_r[i], da_w[i]);

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");
    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the generality of the paramaterized data conversion
 *        - facility
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

    PD_defix(strm, "int8", 1, 1, NORMAL_ORDER);
    PD_defix(strm, "int40", 5, 4, REVERSE_ORDER);

    PD_defloat(strm, "fp8", 1L, 1, ord_int8, 2L, 5L, 5L, 6L, 0L, 0L, 1L);
    PD_defloat(strm, "fp24", 3L, 4, ord_int24, 7L, 16L, 0L, 1L, 8L, 0L, 0x3FL);

    PD_typedef(strm, "integer", "enum");

    prep_test_1_data();

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

/* dump the symbol table */
    dump_test_symbol_table(fp, strm->symtab, 1);

/* read the data from the file */
    read_test_1_data(strm);

/* compare the original data with that read in */
    err = compare_test_1_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_1_data(fp);

    io_close(fp);
    if (err)
       remove(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #2 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_2_DATA - prepare the test data */

static void prep_test_2_data(void)
   {int i;

    prep_test_1_data();

/* set scalars */
    ls_r = 0;

    ls_w =  10;

/* set int array */
    for (i = 0; i < N_INT; i++)
        {la_w[i] = i - 2;
         la_r[i] = 0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_2_DATA - write out the data into the PDB file */

static void write_test_2_data(PDBfile *strm)
   {

/* write scalars into the file */
    PD_write(strm, "ls", "int", &ls_w);

/* write primitive arrays into the file */
    PD_write(strm, "la(5)", "int", la_w);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_2_DATA - read the test data from the file */

static void read_test_2_data(PDBfile *strm)
   {

/* read the scalar data from the file */
    PD_read(strm, "ls", &ls_r);

/* read the primitive arrays from the file */
    PD_read(strm, "la", la_r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_2_DATA - print it out to STDOUT */

static void print_test_2_data(FILE *fp)
   {int i;

/* print scalars */
    PRINT(fp, "integer scalar: ls = %d\n", ls_r);

    PRINT(fp, "\n");

/* print int array */
    PRINT(fp, "integer array:\n");
    for (i = 0; i < N_INT; i++)
        PRINT(fp, "  la[%d] = %d\n", i, la_r[i]);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_2_DATA - compare the test data */

static int compare_test_2_data(PDBfile *strm, FILE *fp)
   {int i, err, err_tot;
    long double fptol[3];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

/* compare scalars */
    err  = TRUE;
    err &= (ls_r == ls_w);

    if (err)
       PRINT(fp, "Scalars compare\n");
    else
       PRINT(fp, "Scalars differ\n");
    err_tot &= err;

    err  = TRUE;

/* compare int array */
    for (i = 0; i < N_INT; i++)
        err &= (la_r[i] == la_w[i]);

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");
    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test PD_change_primitive */

static int test_2(char *base, char *tgt, int n)
   {int nb, err;
    PDBfile *strm;
    char datfile[MAXLINE], fname[MAXLINE];
    FILE *fp;

    nb = sizeof(long);

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_create(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s created\n", datfile);

    PD_change_primitive(strm, SC_INT_I, nb, nb, NULL, NULL);

    prep_test_2_data();

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

    PD_change_primitive(strm, SC_INT_I, nb, nb, NULL, NULL);

/* dump the symbol table */
    dump_test_symbol_table(fp, strm->symtab, 1);

/* read the data from the file */
    read_test_2_data(strm);

/* compare the original data with that read in */
    err = compare_test_2_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_2_data(fp);

    io_close(fp);
    if (err)
       remove(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                             MAIN DRIVERS                                 */

/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes */

static int run_test(PFTest test, int n, char *host)
   {int i, m, rv, fail;
    char *nm;
    tframe st;

    pre_test(&st, debug_mode);

    fail = 0;

/* native test */
    if ((*test)(host, NULL, n) == FALSE)
       {PRINT(STDOUT, "Test #%d native failed\n", n);
	fail++;};

    if (native_only == FALSE)
       {m = PD_target_n_platforms();
	for (i = 0; i < m; i++)
	    {rv = PD_target_platform_n(i);
	     SC_ASSERT(rv == TRUE);

	     nm = PD_target_platform_name(i);
	     if ((*test)(host, nm, n) == FALSE)
	        {PRINT(STDOUT, "Test #%d %s failed\n", n, nm);
		 fail++;};};};

    post_test(&st, n);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDAS - test user defined primitive types\n\n");
    PRINT(STDOUT, "Usage: tpdas [-d] [-h] [-n] [-v #] [-1] [-2]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       n - run native mode test only\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "       1 - do NOT run test #1\n");
    PRINT(STDOUT, "       2 - do NOT run test #1\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, err;
    int test_one, test_two;
    int use_mapped_files;

    PD_init_threads(0, NULL);

    SC_zero_space_n(1, -2);
    debug_mode       = FALSE;
    native_only      = FALSE;
    use_mapped_files = FALSE;
    test_one         = TRUE;
    test_two         = TRUE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'd' :
		      debug_mode  = TRUE;
		      native_only = TRUE;
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
                 case '1' :
		      test_one = FALSE;
		      break;
                 case '2' :
		      test_two = FALSE;
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

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
