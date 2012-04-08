/*
 * TPDCORE5.C - core PDB test #5
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

#define DOUBLE_EQUAL(d1, d2) (PM_value_compare(d1, d2, fptol[1]) == 0)

struct s_lev25
   {char **s;
    int type;};

typedef struct s_lev25 lev25;

struct s_lev15
   {int *a;
    double *b;
    lev25 *c;};

typedef struct s_lev15 lev15;

static lev15
 tar5_t[4],
 tar5_r[4],
 *tar5_w;

static int
 p_w[N_INT],
 p_r[N_INT];

/*--------------------------------------------------------------------------*/

/*                            TEST #5 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_5_DATA - prepare the test data */

static void prep_test_5_data(void)
   {int i, *p1, *p2;
    double *p3, *p4;

    for (i = 0; i < N_INT; i++)
        {p_w[i] = i;
         p_r[i] = 0;};

    tar5_w = CMAKE_N(lev15, 2);

    p1 = tar5_w[0].a = CMAKE_N(int, N_INT);
    p2 = tar5_w[1].a = CMAKE_N(int, N_INT);
    for (i = 0; i < N_INT; i++)
        {p1[i] = i;
         p2[i] = i + 10;};

    p3 = tar5_w[0].b = CMAKE_N(double, N_DOUBLE);
    p4 = tar5_w[1].b = CMAKE_N(double, N_DOUBLE);
    for (i = 0; i < N_DOUBLE; i++)
        {p3[i] = exp((double) i);
         p4[i] = log(1.0 + (double) i);};

    tar5_w[0].c = NULL;
    tar5_w[1].c = NULL;

    memset(tar5_t, 0, 4*sizeof(lev15));
    memset(tar5_r, 0, 4*sizeof(lev15));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_5 - free all known test data memory */

static void cleanup_test_5(void)
   {int l;

    if (tar5_w != NULL)
       {CFREE(tar5_w[0].a);
        CFREE(tar5_w[1].a);

        CFREE(tar5_w[0].b);
        CFREE(tar5_w[1].b);

        CFREE(tar5_w);};

    for (l = 0; l < 4; l++)
       {CFREE(tar5_t[l].a);
        CFREE(tar5_t[l].b);

        CFREE(tar5_r[l].a);
        CFREE(tar5_r[l].b);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_5_DATA - write out the data into the PDB file */

static void write_test_5_data(PDBfile *strm)
   {

    if (PD_write(strm, "tar(2)", "lev15", tar5_w) == 0)
       error(1, STDOUT, "TAR WRITE FAILED - WRITE_TEST_5_DATA\n");

    if (PD_append(strm, "tar(2:3)", tar5_w) == 0)
       error(1, STDOUT, "TAR APPEND FAILED - WRITE_TEST_5_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_5_DATA - read the test data from the file */

static int read_test_5_data(PDBfile *strm)
   {int err;

    err = PD_read(strm, "tar",    tar5_t);
    if (partial_read == TRUE)
       {err = PD_read(strm, "tar(0)", tar5_r);
	err = PD_read(strm, "tar(1)", tar5_r+1);
	err = PD_read(strm, "tar(2)", tar5_r+2);
	err = PD_read(strm, "tar(3)", tar5_r+3);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_5_DATA - print it out to the file */

static void print_test_5_data(FILE *fp)
   {int i;
    int *p0, *p1, *p2, *p3;
    double *p4, *p5, *p6, *p7;

    PRINT(fp, "\n");

    p0 = tar5_t[0].a;
    p1 = tar5_t[1].a;
    p2 = tar5_t[2].a;
    p3 = tar5_t[3].a;
    PRINT(fp, "TAR struct member A:\n");
    PRINT(fp, "    TAR[0].A    TAR[1].A    TAR[2].A    TAR[3].A\n");
    for (i = 0; i < N_INT; i++)
        PRINT(fp, "        %d          %d        %d          %d\n",
	      p0[i], p1[i], p2[i], p3[i]);
    PRINT(fp, "\n");

    p4 = tar5_t[0].b;
    p5 = tar5_t[1].b;
    p6 = tar5_t[2].b;
    p7 = tar5_t[3].b;
    PRINT(fp, "TAR struct member B:\n");
    PRINT(fp, "    TAR[0].B    TAR[1].B    TAR[2].B    TAR[3].B\n");
    for (i = 0; i < N_DOUBLE; i++)
        PRINT(fp, "    %f    %f    %f    %f\n",
	      p4[i], p5[i], p6[i], p7[i]);
    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_5_DATA - compare the test data */

static int compare_test_5_data(PDBfile *strm, FILE *fp)
   {int i, l, *p1w, *p2w, *p1r, *p2r;
    int err, err_tot;
    double *p3w, *p4w, *p3r, *p4r;
    long double fptol[N_PRIMITIVE_FP];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

    err = TRUE;
    if (partial_read == TRUE)
       {p1w = tar5_w[0].a;
	p2w = tar5_w[1].a;
	p3w = tar5_w[0].b;
	p4w = tar5_w[1].b;

	for (l = 0; l < 2; l++)
	    {p1r = tar5_t[2*l].a;
	     p2r = tar5_t[2*l+1].a;
	     for (i = 0; i < N_INT; i++)
	         {err &= (p1w[i] == p1r[i]);
		  err &= (p2w[i] == p2r[i]);};

	     p3r = tar5_t[2*l].b;
	     p4r = tar5_t[2*l+1].b;
	     for (i = 0; i < N_DOUBLE; i++)
	         {err &= DOUBLE_EQUAL(p3w[i], p3r[i]);
		  err &= DOUBLE_EQUAL(p4w[i], p4r[i]);};};

	for (l = 0; l < 2; l++)
	    {p1r = tar5_r[2*l].a;
	     p2r = tar5_r[2*l+1].a;
	     for (i = 0; i < N_INT; i++)
	         {err &= (p1w[i] == p1r[i]);
		  err &= (p2w[i] == p2r[i]);};

	     p3r = tar5_r[2*l].b;
	     p4r = tar5_r[2*l+1].b;
	     for (i = 0; i < N_DOUBLE; i++)
	         {err &= DOUBLE_EQUAL(p3w[i], p3r[i]);
		  err &= DOUBLE_EQUAL(p4w[i], p4r[i]);};};};

    PRINT(fp, "\n");

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
