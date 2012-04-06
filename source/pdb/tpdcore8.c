/*
 * TPDCORE8.C - core PDB test #8
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "nat"

#define DOUBLE_EQUAL(d1, d2) (PM_value_compare(d1, d2, fptol[1]) == 0)

static double
 *d8_w,
 *d8a_r,
 *d8b_r,
 *d8c_r,
 *d8d_r;

/*--------------------------------------------------------------------------*/

/*                            TEST #8 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_8_DATA - prepare the test data */

static void prep_test_8_data(void)
   {int i, j, n, m;

    n = 10;
    m = 2*n;

    d8_w  = CMAKE_N(double, n);
    d8a_r = CMAKE_N(double, m);
    d8b_r = CMAKE_N(double, m);
    d8c_r = CMAKE_N(double, m);
    d8d_r = CMAKE_N(double, m);

    for (i = 0; i < n; i++)
        {j = i + 1;
	 d8_w[i] = (double) (j * j);};

    for (i = 0; i < m; i++)
        {d8a_r[i] = 0.0;
         d8b_r[i] = 0.0;
         d8c_r[i] = 0.0;
         d8d_r[i] = 0.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_8 - free all known test data memory */

static void cleanup_test_8(void)
   {

    CFREE(d8_w);
    CFREE(d8a_r);
    CFREE(d8b_r);
    CFREE(d8c_r);
    CFREE(d8d_r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_8_DATA - write out the data into the PDB file */

static void write_test_8_data(PDBfile *strm)
   {long ind[4];

    if (PD_write(strm, "d8a[10]", "double", d8_w) == 0)
       error(1, STDOUT, "D8A WRITE FAILED - WRITE_TEST_8_DATA\n");

    if (PD_defent(strm, "d8b[1,10]", "double") == 0)
       error(1, STDOUT, "D8B DEFENT FAILED - WRITE_TEST_8_DATA\n");

    if (PD_write(strm, "d8b[0,0:9]", "double", d8_w) == 0)
       error(1, STDOUT, "D8B WRITE FAILED - WRITE_TEST_8_DATA\n");

    if (PD_defent(strm, "d8c[0,10]", "double") == 0)
       error(1, STDOUT, "D8C DEFENT FAILED - WRITE_TEST_8_DATA\n");

    ind[0] = 1L;
    ind[1] = 0L;
    ind[2] = 0L;
    ind[3] = 9L;
    if (PD_defent_alt(strm, "d8d", "double", 2, ind) == 0)
       error(1, STDOUT, "D8D DEFENT FAILED - WRITE_TEST_8_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* APPEND_TEST_8_DATA - append to the data in the PDB file */

static void append_test_8_data(PDBfile *strm)
   {

    if (PD_append(strm, "d8a[10:19]", d8_w) == 0)
       error(1, STDOUT, "D8A APPEND FAILED - WRITE_TEST_8_DATA\n");

    if (PD_append(strm, "d8b[1:1,0:9]", d8_w) == 0)
       error(1, STDOUT, "D8B APPEND FAILED - WRITE_TEST_8_DATA\n");

    if (PD_append(strm, "d8c[0:0,0:9]", d8_w) == 0)
       error(1, STDOUT, "D8C APPEND FAILED - WRITE_TEST_8_DATA\n");

    if (PD_append(strm, "d8d[0:0,0:9]", d8_w) == 0)
       error(1, STDOUT, "D8D APPEND FAILED - WRITE_TEST_8_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_8_DATA - read the test data from the file */

static int read_test_8_data(PDBfile *strm)
   {int err;

    err = PD_read(strm, "d8a", d8a_r);
    err = PD_read(strm, "d8b", d8b_r);
    err = PD_read(strm, "d8c", d8c_r);
    err = PD_read(strm, "d8d", d8d_r);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_8_DATA - print it out to the file */

static void print_test_8_data(FILE *fp)
   {int i, n;

    n = 20;

    PRINT(fp, "\n");

    PRINT(fp, "D8A:\n");
    for (i = 0; i < n; i++)
        PRINT(fp, "%3d  %11.4e %11.4e %11.4e\n",
	      i, d8a_r[i], d8b_r[i], d8c_r[i]);
    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_8_DATA - compare the test data */

static int compare_test_8_data(PDBfile *strm, FILE *fp)
   {int i, n;
    int err, err_tot;
    double wc, arc, brc, crc, drc;
    long double fptol[N_PRIMITIVE_FP];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;
    err     = TRUE;
    n       = 10;

    for (i = 0; i < n; i++)
        {wc  = d8_w[i];
	 arc = d8a_r[i];
	 brc = d8b_r[i];
	 crc = d8c_r[i];
	 drc = d8d_r[i];

	 err &= DOUBLE_EQUAL(wc, arc);
	 err &= DOUBLE_EQUAL(wc, brc);
	 err &= DOUBLE_EQUAL(wc, crc);
	 err &= DOUBLE_EQUAL(wc, drc);};

    for (i = 0; i < n; i++)
        {wc  = d8_w[i];
	 arc = d8a_r[n+i];
	 brc = d8b_r[n+i];

	 err &= DOUBLE_EQUAL(wc, arc);
	 err &= DOUBLE_EQUAL(wc, brc);};

    err_tot &= err;

    PRINT(fp, "\n");

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
