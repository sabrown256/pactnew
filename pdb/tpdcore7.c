/*
 * TPDCORE7.C - core PDB test #7
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "nat"

#define FLOAT_EQUAL(d1, d2)  (PM_value_compare(d1, d2, fptol[0]) == 0)

struct s_st7
   {int n;
    double *a;};

typedef struct s_st7 st7;

static st7
 d71_w,
 d71_w_save,
 d72_w,
 d71_r,
 d72_r;

/*--------------------------------------------------------------------------*/

/*                            TEST #7 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_7_DATA - prepare the test data */

static void prep_test_7_data(void)
   {int i, n;

    n = 10;

    d71_w.a      = CMAKE_N(double, n);
    d71_w_save.a = CMAKE_N(double, n);
    d72_w.a      = CMAKE_N(double, n);
  
    d71_w.n      = n;
    d71_w_save.n = n;
    d72_w.n      = n;

    for (i = 0; i < n; i++)
        {d71_w.a[i] = d71_w_save.a[i] = (double)i;
         d72_w.a[i] = (double)i * 10.0;}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_7 - free all known test data memory */

static void cleanup_test_7(void)
   {

    CFREE(d71_w.a);
    CFREE(d71_w_save.a);
    CFREE(d72_w.a);
    CFREE(d71_r.a);
    CFREE(d72_r.a);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_7_DATA - write out the data into the PDB file */

static void write_test_7_data(PDBfile *strm)
   {int i;

    if (PD_write(strm, "d71", "st7", &d71_w) == 0)
       error(1, STDOUT, "D71 WRITE FAILED - WRITE_TEST_7_DATA\n");

/* reuse an area of memory to test PD_TRACK_POINTERS */
    for (i = 0; i < d71_w.n; i++)
        d71_w.a[i] = (double) i * 10.0;

    if (PD_write(strm, "d72", "st7", &d71_w) == 0)
       error(1, STDOUT, "D72 WRITE FAILED - WRITE_TEST_7_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_7_DATA - read the test data from the file */

static int read_test_7_data(PDBfile *strm)
   {int err;

    err = PD_read(strm, "d71", &d71_r);
    err = PD_read(strm, "d72", &d72_r);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_7_DATA - print it out to the file */

static void print_test_7_data(FILE *fp)
   {int i, n;

    n = d71_w.n;

    PRINT(fp, "\n");

    PRINT(fp, "D71 struct member A:\n");
    for (i = 0; i < n; i++)
        PRINT(fp, "%3d %5.2f\n", i, d71_r.a[i]);
    PRINT(fp, "\n");

    PRINT(fp, "D72 struct member A:\n");
    for (i = 0; i < n; i++)
        PRINT(fp, "%3d %5.2f\n", i, d72_r.a[i]);
    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_7_DATA - compare the test data */

static int compare_test_7_data(PDBfile *strm, FILE *fp)
   {int i, n;
    int err, err_tot;
    float fc1, fc2;
    double *p1w, *p2w;
    long double fptol[N_PRIMITIVE_FP];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

    err = TRUE;
    p1w = d71_w_save.a;
    p2w = d72_w.a;

    n = d71_w.n;

    for (i = 0; i < n; i++)
        {fc1 = p1w[i];
	 fc2 = p2w[i];
	 err &= FLOAT_EQUAL(fc1, d71_r.a[i]);
	 err &= FLOAT_EQUAL(fc2, d72_r.a[i]);};

    err_tot &= err;

    PRINT(fp, "\n");

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
