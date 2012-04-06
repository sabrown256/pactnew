/*
 * TPDCORE4.C - core PDB test #4
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

struct s_st4
   {short a;
    char  b;};

typedef struct s_st4 st4;

static st4
 *vr4_w,
 *vr4_r;

static hasharr
 *tab4_r,
 *tab4_w;

static char
 *CHAR_S,
 *SHORT_S,
 *INT_S,
 *LONG_S,
 *FLOAT_S,
 *DOUBLE_S,
 *HASHEL_S;

/*--------------------------------------------------------------------------*/

/*                            TEST #4 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_4_DATA - prepare the test data */

static void prep_test_4_data(void)
   {int *pi;
    short *ps;
    long *pl;
    float *pf;
    double *pd;
    char *pc;
    haelem *hp;

    tab4_w = SC_make_hasharr(3, NODOC, SC_HA_NAME_KEY, 0);

    CHAR_S   = CSTRSAVE("char *");
    SHORT_S  = CSTRSAVE("short *");
    INT_S    = CSTRSAVE("int *");
    LONG_S   = CSTRSAVE("long *");
    FLOAT_S  = CSTRSAVE("float *");
    DOUBLE_S = CSTRSAVE("double *");
    HASHEL_S = CSTRSAVE("haelem *");

    pc  = CMAKE(char);
    *pc = 'A';   
    SC_hasharr_install(tab4_w, "pc", pc, CHAR_S, 3, -1);

    ps  = CMAKE(short);
    *ps = -1024;
    SC_hasharr_install(tab4_w, "ps", ps, SHORT_S, 3, -1);

    pi  = CMAKE(int);
    *pi = 16384;
    SC_hasharr_install(tab4_w, "pi", pi, INT_S, 3, -1);

    pl  = CMAKE(long);
    *pl = -1048576;
    SC_hasharr_install(tab4_w, "pl", pl, LONG_S, 3, -1);

    pf  = CMAKE(float);
    *pf = 3.141596;
    SC_hasharr_install(tab4_w, "pf", pf, FLOAT_S, 3, -1);

    pd  = CMAKE(double);
    *pd = -1.0e-30;
    hp = SC_hasharr_install(tab4_w, "pd", pd, DOUBLE_S, 3, -1);

/* mark to keep reference count right and valgrind clean */
    SC_mark(hp, 1);

    SC_hasharr_install(tab4_w, "ph", hp, HASHEL_S, 3, -1);

    tab4_r = NULL;

    vr4_w = CMAKE_N(st4, 3);

    vr4_w[0].a =  2048;
    vr4_w[0].b =  'G';
    vr4_w[1].a = -2048;
    vr4_w[1].b =  'H';
    vr4_w[2].a =  4096;
    vr4_w[2].b =  'I';

    vr4_r = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_HA_TEST_4 - free the haelem types which were read in not constant */

static int cleanup_ha_test_4(haelem *hp, void *a)
   {

    CFREE(hp->type);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_4 - free all known test data memory */

static void cleanup_test_4(void)
   {

    SC_free_hasharr(tab4_w, NULL, NULL);

    if (tab4_r != NULL)
       SC_free_hasharr(tab4_r, cleanup_ha_test_4, NULL);

    CFREE(CHAR_S);
    CFREE(SHORT_S);
    CFREE(INT_S);
    CFREE(LONG_S);
    CFREE(FLOAT_S);
    CFREE(DOUBLE_S);
    CFREE(HASHEL_S);

    CFREE(vr4_w);
    CFREE(vr4_r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_4_DATA - write out the data into the PDB file */

static void write_test_4_data(PDBfile *strm)
   {

    if (PD_write(strm, "tab4", "hasharr *", &tab4_w) == 0)
       error(1, STDOUT, "TAB4 WRITE FAILED - WRITE_TEST_4_DATA\n");

    if (PD_write(strm, "vr4", "st4 *", &vr4_w) == 0)
       error(1, STDOUT, "VR4 WRITE FAILED - WRITE_TEST_4_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_4_DATA - read the test data from the file */

static int read_test_4_data(PDBfile *strm)
   {int err;

    err = PD_read(strm, "tab4", &tab4_r);
    SC_hasharr_rekey(tab4_r, SC_HA_NAME_KEY);

    err = PD_read(strm, "vr4", &vr4_r);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_4_DATA - print it out to the file */

static void print_test_4_data(FILE *fp)
   {int i, ne, *pi;
    long *pl;
    short *ps;
    float *pf;
    double *pd;
    char *pc, **names;
    haelem *ph;

    ne    = SC_hasharr_get_n(tab4_r);
    names = SC_hasharr_dump(tab4_r, NULL, NULL, FALSE);
    for (i = 0; i < ne; i++)
        PRINT(fp, "%s\n", names[i]);
    SC_free_strings(names);

    PRINT(fp, "\n");

    pc = (char *) SC_hasharr_def_lookup(tab4_r, "pc");
    ps = (short *) SC_hasharr_def_lookup(tab4_r, "ps");
    pi = (int *) SC_hasharr_def_lookup(tab4_r, "pi");
    pl = (long *) SC_hasharr_def_lookup(tab4_r, "pl");
    pf = (float *) SC_hasharr_def_lookup(tab4_r, "pf");
    pd = (double *) SC_hasharr_def_lookup(tab4_r, "pd");
    ph = (haelem *) SC_hasharr_def_lookup(tab4_r, "ph");

/* mark to keep reference count right and valgrind clean */
    SC_mark(ph, 1);

    PRINT(fp, "Table values:\n");
    PRINT(fp, "   pc = %c %s\n", *pc, CHAR_S);
    PRINT(fp, "   ps = %d %s\n", *ps, SHORT_S);
    PRINT(fp, "   pi = %d %s\n", *pi, INT_S);
    PRINT(fp, "   pl = %ld %s\n", *pl, LONG_S);
    PRINT(fp, "   pf = %f %s\n", *pf, FLOAT_S);
    PRINT(fp, "   pd = %e %s\n", *pd, DOUBLE_S);

    PRINT(fp, "\n   ph : %s %s\n\n", ph->name, ph->type);

    PRINT(fp, "VR4[0]:\n");
    PRINT(fp, "VR4[0].A = %d\n", vr4_r[0].a);
    PRINT(fp, "VR4[0].B = %c\n", vr4_r[0].b);
    PRINT(fp, "VR4[1].A = %d\n", vr4_r[1].a);
    PRINT(fp, "VR4[1].B = %c\n", vr4_r[1].b);
    PRINT(fp, "VR4[2].A = %d\n", vr4_r[2].a);
    PRINT(fp, "VR4[2].B = %c\n", vr4_r[2].b);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_4_DATA - compare the test data */

static int compare_test_4_data(PDBfile *strm, FILE *fp)
   {int err, err_tot;
    long double fptol[N_PRIMITIVE_FP];
    char *pc_w, *pc_r;
    short *ps_w, *ps_r;
    int *pi_w, *pi_r;
    long *pl_w, *pl_r;
    float *pf_w, *pf_r;
    double *pd_w, *pd_r;
    haelem *ph_w, *ph_r;

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

    pc_r = (char *) SC_hasharr_def_lookup(tab4_r, "pc");
    ps_r = (short *) SC_hasharr_def_lookup(tab4_r, "ps");
    pi_r = (int *) SC_hasharr_def_lookup(tab4_r, "pi");
    pl_r = (long *) SC_hasharr_def_lookup(tab4_r, "pl");
    pf_r = (float *) SC_hasharr_def_lookup(tab4_r, "pf");
    pd_r = (double *) SC_hasharr_def_lookup(tab4_r, "pd");
    ph_r = (haelem *) SC_hasharr_def_lookup(tab4_r, "ph");

    pc_w = (char *) SC_hasharr_def_lookup(tab4_w, "pc");
    ps_w = (short *) SC_hasharr_def_lookup(tab4_w, "ps");
    pi_w = (int *) SC_hasharr_def_lookup(tab4_w, "pi");
    pl_w = (long *) SC_hasharr_def_lookup(tab4_w, "pl");
    pf_w = (float *) SC_hasharr_def_lookup(tab4_w, "pf");
    pd_w = (double *) SC_hasharr_def_lookup(tab4_w, "pd");
    ph_w = (haelem *) SC_hasharr_def_lookup(tab4_w, "ph");

/* compare offset and array */
    err  = TRUE;
    err &= (*pc_r == *pc_w);
    err &= (*ps_r == *ps_w);
    err &= (*pi_r == *pi_w);
    err &= (*pl_r == *pl_w);
    err &= FLOAT_EQUAL(*pf_r, *pf_w);
    err &= DOUBLE_EQUAL(*pd_r, *pd_w);

    if (err)
       PRINT(fp, "Hash tables compare\n");
    else
       PRINT(fp, "Hash tables differ\n");
    err_tot &= err;

/* compare haelem info */
    err  = TRUE;
    err &= (strcmp(ph_r->name, ph_w->name) == 0);
    err &= (strcmp(ph_r->type, ph_w->type) == 0);
    if (err)
       PRINT(fp, "Hashels compare\n");
    else
       PRINT(fp, "Hashels differ\n");
    err_tot &= err;

/* check the new alignments */
    err  = TRUE;
    err &= (vr4_w[0].a == vr4_r[0].a);
    err &= (vr4_w[0].b == vr4_r[0].b);
    err &= (vr4_w[1].a == vr4_r[1].a);
    err &= (vr4_w[1].b == vr4_r[1].b);
    err &= (vr4_w[2].a == vr4_r[2].a);
    err &= (vr4_w[2].b == vr4_r[2].b);

    if (err)
       PRINT(fp, "Alignments compare\n");
    else
       PRINT(fp, "Alignments differ\n");
    err_tot &= err;

    PRINT(fp, "\n");

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
