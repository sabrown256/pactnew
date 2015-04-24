/*
 * TPDCORE6.C - core PDB test #6
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "nat"

#define FLOAT_EQUAL(d1, d2)  (PM_value_compare(d1, d2, fptol[0]) == 0)

struct s_st61
   {int n;
    double a[10];};

typedef struct s_st61 st61;

struct s_st62
   {int n;
    double *a;};

typedef struct s_st62 st62;

static st61
 *d61_w = NULL;

static st62
 *d62_w = NULL;

static float
 d61_a[10],
 d62_a[10],
 d62_s[8];


/*--------------------------------------------------------------------------*/

/*                            TEST #6 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_6_DATA - prepare the test data */

static void prep_test_6_data(void)
   {int i, n;
    double *p1, *p2;

    n = 10;

    d61_w    = CMAKE(st61);
    d61_w->n = n;

    d62_w    = CMAKE(st62);
    d62_w->n = n;
    d62_w->a = CMAKE_N(double, n);

    p1 = d61_w->a;
    p2 = d62_w->a;
    for (i = 0; i < n; i++)
        {p1[i] = 0.1*((double) i);
         p2[i] = -0.1*((double) i);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_6 - free all known test data memory */

static void cleanup_test_6(void)
   {

    if (d61_w != NULL)
       CFREE(d61_w);

    if (d62_w != NULL)
       {CFREE(d62_w->a);
	CFREE(d62_w);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_6_DATA - write out the data into the PDB file */

static void write_test_6_data(PDBfile *strm)
   {

    if (PD_write(strm, "d61", "st61", d61_w) == 0)
       error(1, STDOUT, "D61 WRITE FAILED - WRITE_TEST_6_DATA\n");

    if (PD_write(strm, "d62", "st62", d62_w) == 0)
       error(1, STDOUT, "D62 WRITE FAILED - WRITE_TEST_6_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_6_DATA - read the test data from the file */

static int read_test_6_data(PDBfile *strm)
   {int err;

    err = PD_read_as_dwim(strm, "d61.a", "float", 10, d61_a);
    err = PD_read_as_dwim(strm, "d62.a", "float", 10, d62_a);
    err = PD_read_as_dwim(strm, "d62.a", "float",  8, d62_s);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_6_DATA - print it out to the file */

static void print_test_6_data(FILE *fp)
   {int i, n;

    n = d61_w->n;

    PRINT(fp, "\n");

    PRINT(fp, "D61 struct member A:\n");
    for (i = 0; i < n; i++)
        PRINT(fp, "%3d %5.2f\n", i, d61_a[i]);
    PRINT(fp, "\n");

    PRINT(fp, "D62 struct member A:\n");
    for (i = 0; i < n; i++)
        PRINT(fp, "%3d %5.2f\n", i, d62_a[i]);
    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_6_DATA - compare the test data */

static int compare_test_6_data(PDBfile *strm, FILE *fp)
   {int i, n;
    int err, err_tot;
    float fc1, fc2;
    double *p1w, *p2w;
    long double fptol[N_PRIMITIVE_FP];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

    err = TRUE;
    p1w = d61_w->a;
    p2w = d62_w->a;

    n = d61_w->n;

    for (i = 0; i < n; i++)
        {fc1 = p1w[i];
	 fc2 = p2w[i];
	 err &= FLOAT_EQUAL(fc1, d61_a[i]);
	 err &= FLOAT_EQUAL(fc2, d62_a[i]);};

    err_tot &= err;

    n -= 2;
    for (i = 0; i < n; i++)
        {fc2 = p2w[i];
	 err &= FLOAT_EQUAL(fc2, d62_s[i]);};

    err_tot &= err;

    PRINT(fp, "\n");

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
