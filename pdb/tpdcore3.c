/*
 * TPDCORE3.C - core PDB test #3
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "nat"

#define FLOAT_EQUAL(d1, d2)  (PM_value_compare(d1, d2, fptol[0]) == 0)
#define DOUBLE_EQUAL(d1, d2) (PM_value_compare(d1, d2, fptol[1]) == 0)

struct s_st3
   {char a;
    short b;
    char c[2];
    int d;
    char e[3];
    float f;
    char g[4];
    double h;
    char i[5];
    char *j;
    char k[6];};

typedef struct s_st3 st3;

static st3
 vr1_w,
 vr1_r;

/*--------------------------------------------------------------------------*/

/*                            TEST #3 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_3_DATA - prepare the test data */

static void prep_test_3_data(void)
   {

    vr1_w.a    = 'A';
    vr1_w.b    = -10;
    vr1_w.c[0] = 'B';
    vr1_w.c[1] = 'a';
    vr1_w.d    = 32589;
    vr1_w.e[0] = 'C';
    vr1_w.e[1] = 'a';
    vr1_w.e[2] = 'b';
    vr1_w.f    = 1.523e-19;
    vr1_w.g[0] = 'D';
    vr1_w.g[1] = 'a';
    vr1_w.g[2] = 'b';
    vr1_w.g[3] = 'c';
    vr1_w.h    = 4.2782918323832554e30;
    vr1_w.i[0] = 'E';
    vr1_w.i[1] = 'a';
    vr1_w.i[2] = 'b';
    vr1_w.i[3] = 'c';
    vr1_w.i[4] = 'd';
    vr1_w.j    = CSTRSAVE("whoa there big fella!");
    vr1_w.k[0] = 'F';
    vr1_w.k[1] = 'a';
    vr1_w.k[2] = 'b';
    vr1_w.k[3] = 'c';
    vr1_w.k[4] = 'd';
    vr1_w.k[5] = 'e';

    vr1_r.a    = '\0';
    vr1_r.b    = 0;
    vr1_r.c[0] = '\0';
    vr1_r.c[1] = '\0';
    vr1_r.d    = 0;
    vr1_r.e[0] = '\0';
    vr1_r.e[1] = '\0';
    vr1_r.e[2] = '\0';
    vr1_r.f    = 0.0;
    vr1_r.g[0] = '\0';
    vr1_r.g[1] = '\0';
    vr1_r.g[2] = '\0';
    vr1_r.g[3] = '\0';
    vr1_r.h    = 0.0;
    vr1_r.i[0] = '\0';
    vr1_r.i[1] = '\0';
    vr1_r.i[2] = '\0';
    vr1_r.i[3] = '\0';
    vr1_r.i[4] = '\0';
    vr1_r.j    = NULL;
    vr1_r.k[0] = '\0';
    vr1_r.k[1] = '\0';
    vr1_r.k[2] = '\0';
    vr1_r.k[3] = '\0';
    vr1_r.k[4] = '\0';
    vr1_r.k[5] = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_3 - free all known test data memory */

static void cleanup_test_3(void)
   {

    CFREE(vr1_w.j);
    CFREE(vr1_r.j);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_3_DATA - write out the data into the PDB file */

static void write_test_3_data(PDBfile *strm)
   {

    if (PD_write(strm, "vr1", "st3", &vr1_w) == 0)
       error(1, STDOUT, "VR1 WRITE FAILED - WRITE_TEST_3_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_3_DATA - read the test data from the file */

static int read_test_3_data(PDBfile *strm)
   {int err;

    err = PD_read(strm, "vr1", &vr1_r);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_3_DATA - print it out to the file */

static void print_test_3_data(FILE *fp)
   {

    PRINT(fp, "struct vr1:\n");
    PRINT(fp, "   vr1.a    = %c\n", vr1_r.a);
    PRINT(fp, "   vr1.b    = %d\n", vr1_r.b);
    PRINT(fp, "   vr1.c[0] = %c\n", vr1_r.c[0]);
    PRINT(fp, "   vr1.c[1] = %c\n", vr1_r.c[1]);
    PRINT(fp, "   vr1.d    = %d\n", vr1_r.d);
    PRINT(fp, "   vr1.e[0] = %c\n", vr1_r.e[0]);
    PRINT(fp, "   vr1.e[1] = %c\n", vr1_r.e[1]);
    PRINT(fp, "   vr1.e[2] = %c\n", vr1_r.e[2]);
    PRINT(fp, "   vr1.f    = %14.7e\n", vr1_r.f);
    PRINT(fp, "   vr1.g[0] = %c\n", vr1_r.g[0]);
    PRINT(fp, "   vr1.g[1] = %c\n", vr1_r.g[1]);
    PRINT(fp, "   vr1.g[2] = %c\n", vr1_r.g[2]);
    PRINT(fp, "   vr1.g[3] = %c\n", vr1_r.g[3]);
    PRINT(fp, "   vr1.h    = %20.13e\n", vr1_r.h);
    PRINT(fp, "   vr1.i[0] = %c\n", vr1_r.i[0]);
    PRINT(fp, "   vr1.i[1] = %c\n", vr1_r.i[1]);
    PRINT(fp, "   vr1.i[2] = %c\n", vr1_r.i[2]);
    PRINT(fp, "   vr1.i[3] = %c\n", vr1_r.i[3]);
    PRINT(fp, "   vr1.i[4] = %c\n", vr1_r.i[4]);
    PRINT(fp, "   vr1.j    = %s\n", vr1_r.j);
    PRINT(fp, "   vr1.k[0] = %c\n", vr1_r.k[0]);
    PRINT(fp, "   vr1.k[1] = %c\n", vr1_r.k[1]);
    PRINT(fp, "   vr1.k[2] = %c\n", vr1_r.k[2]);
    PRINT(fp, "   vr1.k[3] = %c\n", vr1_r.k[3]);
    PRINT(fp, "   vr1.k[4] = %c\n", vr1_r.k[4]);
    PRINT(fp, "   vr1.k[5] = %c\n", vr1_r.k[5]);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_3_DATA - compare the test data */

static int compare_test_3_data(PDBfile *strm, FILE *fp)
   {int err, err_tot;
    long double fptol[N_PRIMITIVE_FP];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

/* compare offset and array */
    err  = TRUE;
    err &= (vr1_w.a == vr1_r.a);
    err &= (vr1_w.b == vr1_r.b);
    err &= (vr1_w.c[0] == vr1_r.c[0]);
    err &= (vr1_w.c[1] == vr1_r.c[1]);
    err &= (vr1_w.d == vr1_r.d);
    err &= (vr1_w.e[0] == vr1_r.e[0]);
    err &= (vr1_w.e[1] == vr1_r.e[1]);
    err &= (vr1_w.e[2] == vr1_r.e[2]);
    err &= FLOAT_EQUAL(vr1_w.f, vr1_r.f);
    err &= (vr1_w.g[0] == vr1_r.g[0]);
    err &= (vr1_w.g[1] == vr1_r.g[1]);
    err &= (vr1_w.g[2] == vr1_r.g[2]);
    err &= (vr1_w.g[3] == vr1_r.g[3]);
    err &= DOUBLE_EQUAL(vr1_w.h, vr1_r.h);
    err &= (vr1_w.i[0] == vr1_r.i[0]);
    err &= (vr1_w.i[1] == vr1_r.i[1]);
    err &= (vr1_w.i[2] == vr1_r.i[2]);
    err &= (vr1_w.i[3] == vr1_r.i[3]);
    err &= (vr1_w.i[4] == vr1_r.i[4]);
    err &= (strcmp(vr1_w.j, vr1_r.j) == 0);
    err &= (vr1_w.k[0] == vr1_r.k[0]);
    err &= (vr1_w.k[1] == vr1_r.k[1]);
    err &= (vr1_w.k[2] == vr1_r.k[2]);
    err &= (vr1_w.k[3] == vr1_r.k[3]);
    err &= (vr1_w.k[4] == vr1_r.k[4]);
    err &= (vr1_w.k[5] == vr1_r.k[5]);

    if (err)
       PRINT(fp, "Alignments compare\n");
    else
       PRINT(fp, "Alignments differ\n");
    err_tot &= err;

    PRINT(fp, "\n");

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
