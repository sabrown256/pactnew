/*
 * TPDGS.C - gather/scatter test for the PDB I/O
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "gst"

typedef int (*PFTest)(char *base, char *tgt, int n);

int
 native_only = FALSE,
 debug_mode = FALSE;

int
 ia_w[30],
 ib_w[6],
 ib_r[6];

double
 da_w[12],
 db_w[12],
 db_r[12];

/*--------------------------------------------------------------------------*/

/*                          TEST GATHER ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_GATHER_DATA - prepare the test data */

static void prep_test_gather_data(void)
   {int i, j, k, l;
    int imx, jmx, kmx;

/* set int array */
    imx = 2;
    jmx = 3;
    kmx = 5;
    for (i = 0; i < imx; i++)
        for (j = 0; j < jmx; j++)
	    for (k = 0; k < kmx; k++)
	        {l = k + kmx*(j + jmx*i);
		 ia_w[l] = 100*i + 10*j + k;};

    for (i = 0; i < 6; i++)
         ib_r[i] = 0;;

/* set double array */
    imx = 4;
    jmx = 3;
    for (i = 0; i < imx; i++)
        for (j = 0; j < jmx; j++)
	    {l = j + jmx*i;
	     da_w[l] = 10.0*i + j;
	     db_r[l] = 0.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_GATHER - free all known test data memory */

static void cleanup_test_gather(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_GATHER_DATA - write out the data into the PDB file */

static void write_test_gather_data(PDBfile *strm)
   {

/* write primitive arrays into the file */
    if (!PD_write(strm, "ia(2,3,5)", "integer", ia_w))
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_GATHER_DATA\n");
        exit(1);};

    if (!PD_write(strm, "da(4,3)", "double", da_w))
       {PRINT(STDOUT, "DA WRITE FAILED - WRITE_TEST_GATHER_DATA\n");
        exit(1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_GATHER_DATA - read the test data from the file */

static int read_test_gather_data(PDBfile *strm)
   {int err;
    long sind[9], dind[8];

/* gather ia(1,0:2,3) into ib(1,0:2) */
    sind[0] = 1;
    sind[1] = 1;
    sind[2] = 1;

    sind[3] = 0;
    sind[4] = 2;
    sind[5] = 1;

    sind[6] = 3;
    sind[7] = 3;
    sind[8] = 1;

    dind[0] = 1;
    dind[1] = 1;
    dind[2] = 1;
    dind[3] = 2;

    dind[4] = 0;
    dind[5] = 2;
    dind[6] = 1;
    dind[7] = 3;

    err = PD_gather(strm, "ia", &ib_r, sind, 2, dind);

/* gather da(0:3,1) into db(0:3,0) */
    sind[0] = 0;
    sind[1] = 3;
    sind[2] = 1;

    sind[3] = 1;
    sind[4] = 1;
    sind[5] = 1;

    dind[0] = 0;
    dind[1] = 3;
    dind[2] = 1;
    dind[3] = 4;

    dind[4] = 0;
    dind[5] = 0;
    dind[6] = 1;
    dind[7] = 3;

    err = PD_gather(strm, "da", &db_r, sind, 2, dind);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_GATHER_DATA - print it out to STDOUT */

static void print_test_gather_data(FILE *fp)
   {int i;

/* print int array */
    PRINT(fp, "integer array:\n");
    for (i = 0; i < 6; i++)
        PRINT(fp, "  ib[%d] = %d\n", i, ib_r[i]);

/* print double array */
    PRINT(fp, "double array:\n");
    for (i = 0; i < 12; i++)
        PRINT(fp, "  db[%d] = %14.6e\n", i, db_r[i]);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_GATHER_DATA - compare the test data */

static int compare_test_gather_data(PDBfile *strm, FILE *fp)
   {int i, err, err_tot;
    int ia, ib, mo, rmj;
    long double fptol[N_PRIMITIVE_FP];

    if (strm == NULL)
       return(FALSE);

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

    mo  = PD_get_major_order(strm);
    rmj = (mo == ROW_MAJOR_ORDER);

/* compare ia(1,0:2,3) with ib(1,0:2) */
    err = TRUE;
    for (i = 0; i < 3; i++)
        {if (rmj)
	    {ia = 15*1 + 5*i + 3;
	     ib = 3*1 + i;}
	 else
	    {ia = 1 + 2*i + 3*6;
	     ib = 1 + 2*i;};
	 err &= (ia_w[ia] == ib_r[ib]);};

/* compare da(0:3,1) with db(0:3,0) */
    for (i = 0; i < 4; i++)
        {if (rmj)
	    {ia = 3*i + 1;
	     ib = 3*i + 0;}
	 else
	    {ia = 4*1 + i;
	     ib = 4*0 + i;};
	 err &= (da_w[ia] == db_r[ib]);};

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");

    err_tot &= err;

    return(err_tot);}

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

    prep_test_gather_data();

/* write the test data */
    write_test_gather_data(strm);

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
    read_test_gather_data(strm);

/* compare the original data with that read in */
    err = compare_test_gather_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_gather_data(fp);

/* free known test data memory */
    cleanup_test_gather();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test the PDBLib gather functions in COLUMN_MAJOR order
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

    prep_test_gather_data();

/* write the test data */
    write_test_gather_data(strm);

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
    read_test_gather_data(strm);

/* compare the original data with that read in */
    err = compare_test_gather_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_gather_data(fp);

/* free known test data memory */
    cleanup_test_gather();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                         TEST SCATTER ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_SCATTER_DATA - prepare the test data */

static void prep_test_scatter_data(void)
   {int i, j, k, l;
    int imx, jmx, kmx;

/* set int array */
    imx = 2;
    jmx = 3;
    kmx = 5;
    for (i = 0; i < imx; i++)
        for (j = 0; j < jmx; j++)
	    for (k = 0; k < kmx; k++)
	        {l = k + kmx*(j + jmx*i);
		 ia_w[l] = 100*i + 10*j + k;};

    for (i = 0; i < 6; i++)
        {ib_r[i] = 0;
	 ib_w[i] = i;};

/* set double array */
    imx = 4;
    jmx = 3;
    for (i = 0; i < imx; i++)
        for (j = 0; j < jmx; j++)
	    {l = j + jmx*i;
	     da_w[l] = 10.0*i + j;
	     db_r[l] = 0.0;
	     db_w[l] = 50.0*i + j;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_SCATTER - free all known test data memory */

static void cleanup_test_scatter(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_SCATTER_DATA - write out the data into the PDB file */

static void write_test_scatter_data(PDBfile *strm)
   {

/* write primitive arrays into the file */
    if (!PD_write(strm, "ib(2,3)", "integer", ib_w))
       {PRINT(STDOUT, "IB WRITE FAILED - WRITE_TEST_SCATTER_DATA\n");
        exit(1);};

    if (!PD_write(strm, "db(4,3)", "double", db_w))
       {PRINT(STDOUT, "DA WRITE FAILED - WRITE_TEST_SCATTER_DATA\n");
        exit(1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_SCATTER_DATA - read the test data from the file */

static int read_test_scatter_data(PDBfile *strm)
   {int err;
    long sind[12], dind[6];

/* scatter ia(1,0:2,3) into ib(1,0:2) */
    sind[0]  = 1;
    sind[1]  = 1;
    sind[2]  = 1;
    sind[3]  = 2;

    sind[4]  = 0;
    sind[5]  = 2;
    sind[6]  = 1;
    sind[7]  = 3;

    sind[8]  = 3;
    sind[9]  = 3;
    sind[10] = 1;
    sind[11] = 5;

    dind[0] = 1;
    dind[1] = 1;
    dind[2] = 1;

    dind[3] = 0;
    dind[4] = 2;
    dind[5] = 1;

    err  = PD_scatter(strm, "ib", "integer", &ia_w, 3, sind, 2, dind);
    err &= PD_read(strm, "ib", &ib_r);

/* scatter da(0:3,1) into db(0:3,0) */
    sind[0] = 0;
    sind[1] = 3;
    sind[2] = 1;
    sind[3] = 4;

    sind[4] = 1;
    sind[5] = 1;
    sind[6] = 1;
    sind[7] = 3;

    dind[0] = 0;
    dind[1] = 3;
    dind[2] = 1;

    dind[3] = 0;
    dind[4] = 0;
    dind[5] = 1;

    err &= PD_scatter(strm, "db", "double", &da_w, 2, sind, 2, dind);
    err &= PD_read(strm, "db", &db_r);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_SCATTER_DATA - print it out to STDOUT */

static void print_test_scatter_data(FILE *fp)
   {int i;

/* print int array */
    PRINT(fp, "integer array:\n");
    for (i = 0; i < 6; i++)
        PRINT(fp, "  ib[%d] = %d\n", i, ib_r[i]);

/* print double array */
    PRINT(fp, "double array:\n");
    for (i = 0; i < 12; i++)
        PRINT(fp, "  db[%d] = %14.6e\n", i, db_r[i]);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_SCATTER_DATA - compare the test data */

static int compare_test_scatter_data(PDBfile *strm, FILE *fp)
   {int i, err, err_tot;
    int ia, ib, mo, rmj;
    long double fptol[3];

    if (strm == NULL)
       return(FALSE);

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

    mo  = PD_get_major_order(strm);
    rmj = (mo == ROW_MAJOR_ORDER);

/* compare ia(1,0:2,3) with ib(1,0:2) */
    err = TRUE;
    for (i = 0; i < 3; i++)
        {if (rmj)
	    {ia = 15*1 + 5*i + 3;
	     ib = 3*1 + i;}
	 else
	    {ia = 1 + 2*i + 3*6;
	     ib = 1 + 2*i;};
	 err &= (ia_w[ia] == ib_r[ib]);};

/* compare da(0:3,1) with db(0:3,0) */
    for (i = 0; i < 4; i++)
        {if (rmj)
	    {ia = 3*i + 1;
	     ib = 3*i + 0;}
	 else
	    {ia = 4*1 + i;
	     ib = 4*0 + i;};
	 err &= (da_w[ia] == db_r[ib]);};

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");

    err_tot &= err;

    return(err_tot);}

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

    prep_test_scatter_data();

/* write the test data */
    write_test_scatter_data(strm);

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
    read_test_scatter_data(strm);

/* compare the original data with that read in */
    err = compare_test_scatter_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_scatter_data(fp);

/* free known test data memory */
    cleanup_test_scatter();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

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

    prep_test_scatter_data();

/* write the test data */
    write_test_scatter_data(strm);

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
    read_test_scatter_data(strm);

/* compare the original data with that read in */
    err = compare_test_scatter_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_scatter_data(fp);

/* free known test data memory */
    cleanup_test_scatter();

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
	        {PRINT(STDOUT, "Test #%d %s failed\n", n, nm);
		 fail++;};};};

/* native test */
    if ((*test)(host, NULL, n) == FALSE)
       {PRINT(STDOUT, "Test #%d native failed\n", n);
	fail++;};

    post_test(&st, n);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDGS - run PDB scatter/gather test suite\n\n");
    PRINT(STDOUT, "Usage: tpdgs [-d] [-h] [-n] [-v #] [-1] [-2] [-3] [-4]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       n - run native mode test only\n");
    PRINT(STDOUT, "       v - use specified format version (default 2)\n");
    PRINT(STDOUT, "       1 - do NOT run test #1\n");
    PRINT(STDOUT, "       2 - do NOT run test #2\n");
    PRINT(STDOUT, "       3 - do NOT run test #3\n");
    PRINT(STDOUT, "       4 - do NOT run test #4\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the gather/scatter functionality */

int main(int c, char **v)
   {int i, n, err;
    int use_mapped_files;
    int ton[5];

    PD_init_threads(0, NULL);

    SC_zero_space_n(1, -2);
    debug_mode       = FALSE;
    native_only      = FALSE;
    use_mapped_files = FALSE;

    for (i = 0; i < 5; i++)
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

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
