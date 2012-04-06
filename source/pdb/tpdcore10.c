/*
 * TPDCORE10.C - core PDB test #10
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "nat"

#define N_FLOAT  4

#ifdef HAVE_ANSI_FLOAT16

#define QUAD_EQUAL(q1, q2) (PM_qvalue_compare(q1, q2, fptol[2]) == 0)

static long double
 qs_w,
 qs_r,
 qa_w[N_FLOAT],
 qa_r[N_FLOAT];

/*--------------------------------------------------------------------------*/

/*                            TEST #10 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_10_DATA - prepare the test data */

static void prep_test_10_data(void)
   {int i;

/* set scalars */
    qs_r = 0.0L;
    qs_w = expl(1.0);
    qs_w = 2.0L;

/* set long double array */
    for (i = 0; i < N_FLOAT; i++)
        {qa_w[i] = POWL(qs_w, (long double) (i+1));
         qa_r[i] = 0.0L;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_10_DATA - write out the data into the PDB file */

static void write_test_10_data(PDBfile *strm)
   {long ind[6];

/* write scalars into the file */
    if (PD_write(strm, "qs", "long_double",  &qs_w) == 0)
       error(1, STDOUT, "QS WRITE FAILED - WRITE_TEST_10_DATA\n");

/* write primitive arrays into the file */
    ind[0] = 0L;
    ind[1] = N_FLOAT - 1;
    ind[2] = 1L;
    if (PD_write_alt(strm, "qa", "long_double", qa_w, 1, ind) == 0)
       error(1, STDOUT, "DA WRITE FAILED - WRITE_TEST_10_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_10_DATA - read the test data from the file */

static int read_test_10_data(PDBfile *strm)
   {int err;

/* read the scalar data from the file */
    err = PD_read(strm, "qs", &qs_r);

/* read the primitive arrays from the file */
    err = PD_read(strm, "qa",  qa_r);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_10_DATA - print it out to STDOUT */

static void print_test_10_data(FILE *fp)
   {int i;

    PRINT(fp, "long double scalar:  qs = %14.6Le\n", qs_r);

/* print long double array */
    PRINT(fp, "long double array:\n");
    for (i = 0; i < N_FLOAT; i++)
        PRINT(fp, "  qa[%d] = %14.6Le\n", i, qa_r[i]);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_10_DATA - compare the test data */

static int compare_test_10_data(PDBfile *strm, FILE *fp)
   {int i, err, err_tot;
    long double fptol[N_PRIMITIVE_FP];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

/* compare scalars */
    err  = TRUE;

    err &= QUAD_EQUAL(qs_r, qs_w);

    if (err)
       PRINT(fp, "Scalars compare\n");
    else
       PRINT(fp, "Scalars differ\n");
    err_tot &= err;

/* compare long double array */
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

/* TEST_10 - test the long double support in PDBLib
 *         - read and write scalars and arrays of long doubles
 *         - tests can be targeted
 */

int test_10(char *base, char *tgt, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, 100+n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_10_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = PD_create(datfile);
	if (strm == NULL)
	   error(1, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* write the test data */
	write_test_10_data(strm);

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
    read_test_10_data(strm);

/* compare the original data with that read in */
    err = compare_test_10_data(strm, fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_10_data(fp);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* TEST_10 - stub for when there is no long double support */

int test_10(char *base, char *tgt, int n)
   {

    return(TRUE);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
