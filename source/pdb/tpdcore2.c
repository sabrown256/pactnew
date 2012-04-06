/*
 * TPDCORE2.C - core PDB test #2
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

#define DOUBLE_EQUAL(d1, d2) (PM_value_compare(d1, d2, fptol[1]) == 0)

struct s_lev2
   {char **s;
    int type;};

typedef struct s_lev2 lev2;

struct s_lev1
   {int *a;
    double *b;
    lev2 *c;};

typedef struct s_lev1 lev1;

static lev1
 *tar_r,
 *tar_w;

static int
 do_r,
 do_w,
 p_w[N_INT],
 p_r[N_INT];

/* variables for partial and member reads in test #2 */

static int
 *ap1,
 *ap2,
 aa[4];

static double
 *bp1,
 *bp2,
 ba[4];

static lev2
 *cp1,
 *cp2,
 ca[4];

static char
 **sp1,
 **sp2,
 **sp3,
 **sp4,
 *tp1,
 *tp2,
 *tp3,
 *tp4,
 *tp5,
 *tp6,
 *tp7,
 *tp8,
 ta[8];

/*--------------------------------------------------------------------------*/

/*                            TEST #2 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_2_DATA - prepare the test data */

static void prep_test_2_data(void)
   {int i, *p1, *p2;
    double *p3, *p4;

/*    do_w = -1; */

    for (i = 0; i < N_INT; i++)
        {p_w[i] = i;
         p_r[i] = 0;};

    tar_w = CMAKE_N(lev1, 2);

    p1 = tar_w[0].a = CMAKE_N(int, N_INT);
    p2 = tar_w[1].a = CMAKE_N(int, N_INT);
    for (i = 0; i < N_INT; i++)
        {p1[i] = i;
         p2[i] = i + 10;};

    p3 = tar_w[0].b = CMAKE_N(double, N_DOUBLE);
    p4 = tar_w[1].b = CMAKE_N(double, N_DOUBLE);
    for (i = 0; i < N_DOUBLE; i++)
        {p3[i] = exp((double) i);
         p4[i] = log(1.0 + (double) i);};

    tar_w[0].c = CMAKE_N(lev2, 2);
    tar_w[1].c = CMAKE_N(lev2, 2);

    tar_w[0].c[0].s    = CMAKE_N(char *, 2);
    tar_w[0].c[0].s[0] = CSTRSAVE("Hello");
    tar_w[0].c[0].s[1] = CSTRSAVE(" ");
    tar_w[0].c[1].s    = CMAKE_N(char *, 2);
    tar_w[0].c[1].s[0] = CSTRSAVE("world");
    tar_w[0].c[1].s[1] = CSTRSAVE("!");

    tar_w[1].c[0].s    = CMAKE_N(char *, 2);
    tar_w[1].c[0].s[0] = CSTRSAVE("Foo");
    tar_w[1].c[0].s[1] = CSTRSAVE(" ");
    tar_w[1].c[1].s    = CMAKE_N(char *, 2);
    tar_w[1].c[1].s[0] = CSTRSAVE("Bar");
    tar_w[1].c[1].s[1] = CSTRSAVE("!!!");

    tar_w[0].c[0].type = 1;
    tar_w[0].c[1].type = 2;
    tar_w[1].c[0].type = 3;
    tar_w[1].c[1].type = 4;

    tar_r = NULL;

    ap1 = NULL;
    ap2 = NULL;
    for (i = 0; i < 4; i++)
        aa[i] = 0;

    bp1 = NULL;
    bp2 = NULL;
    for (i = 0; i < 4; i++)
        ba[i] = 0.0;

    cp1 = NULL;
    cp2 = NULL;
    for (i = 0; i < 4; i++)
        {ca[i].s    = NULL;
         ca[i].type = 0;};

    sp1 = NULL;
    sp2 = NULL;
    sp3 = NULL;
    sp4 = NULL;

    tp1 = NULL;
    tp2 = NULL;
    tp3 = NULL;
    tp4 = NULL;
    tp5 = NULL;
    tp6 = NULL;
    tp7 = NULL;
    tp8 = NULL;
    for (i = 0; i < 4; i++)
        ta[i] = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_2 - free all know test data memory */

static void cleanup_test_2(void)
   {int i;

    if (tar_w != NULL)
       {CFREE(tar_w[0].c[0].s[0]);
        CFREE(tar_w[0].c[0].s[1]);
        CFREE(tar_w[0].c[1].s[0]);
        CFREE(tar_w[0].c[1].s[1]);

        CFREE(tar_w[1].c[0].s[0]);
        CFREE(tar_w[1].c[0].s[1]);
        CFREE(tar_w[1].c[1].s[0]);
        CFREE(tar_w[1].c[1].s[1]);

        CFREE(tar_w[0].c[0].s);
        CFREE(tar_w[0].c[1].s);
        CFREE(tar_w[1].c[0].s);
        CFREE(tar_w[1].c[1].s);

        CFREE(tar_w[0].c);
        CFREE(tar_w[1].c);

        CFREE(tar_w[0].a);
        CFREE(tar_w[1].a);

        CFREE(tar_w[0].b);
        CFREE(tar_w[1].b);

        CFREE(tar_w);};

    if (tar_r != NULL)
       {CFREE(tar_r[0].c[0].s[0]);
        CFREE(tar_r[0].c[0].s[1]);
        CFREE(tar_r[0].c[1].s[0]);
        CFREE(tar_r[0].c[1].s[1]);

        CFREE(tar_r[1].c[0].s[0]);
        CFREE(tar_r[1].c[0].s[1]);
        CFREE(tar_r[1].c[1].s[0]);
        CFREE(tar_r[1].c[1].s[1]);

        CFREE(tar_r[0].c[0].s);
        CFREE(tar_r[0].c[1].s);
        CFREE(tar_r[1].c[0].s);
        CFREE(tar_r[1].c[1].s);

        CFREE(tar_r[0].c);
        CFREE(tar_r[1].c);

        CFREE(tar_r[0].a);
        CFREE(tar_r[1].a);

        CFREE(tar_r[0].b);
        CFREE(tar_r[1].b);

        CFREE(tar_r);};

    CFREE(ap1);
    CFREE(ap2);

    CFREE(bp1);
    CFREE(bp2);

    if (cp1 != NULL)
       {if (cp1[0].s != NULL)
           {CFREE(cp1[0].s[0]);
            CFREE(cp1[0].s[1]);
            CFREE(cp1[0].s);};

        if (cp1[1].s != NULL)
           {CFREE(cp1[1].s[0]);
            CFREE(cp1[1].s[1]);
            CFREE(cp1[1].s);};

        CFREE(cp1);};

    if (cp2 != NULL)
       {if (cp2[0].s != NULL)
           {CFREE(cp2[0].s[0]);
            CFREE(cp2[0].s[1]);
            CFREE(cp2[0].s);};

        if (cp2[1].s != NULL)
           {CFREE(cp2[1].s[0]);
            CFREE(cp2[1].s[1]);
            CFREE(cp2[1].s);};

        CFREE(cp2);};

    for (i = 0; i < 4; i++)
        {CFREE(ca[i].s[0]);
	 CFREE(ca[i].s[1]);
	 CFREE(ca[i].s);};

    if (sp1 != NULL)
       {CFREE(sp1[0]);
        CFREE(sp1[1]);
        CFREE(sp1);};

    if (sp2 != NULL)
       {CFREE(sp2[0]);
        CFREE(sp2[1]);
        CFREE(sp2);};

    if (sp3 != NULL)
       {CFREE(sp3[0]);
        CFREE(sp3[1]);
        CFREE(sp3);};

    if (sp4 != NULL)
       {CFREE(sp4[0]);
        CFREE(sp4[1]);
        CFREE(sp4);};

    CFREE(tp1);
    CFREE(tp2);
    CFREE(tp3);
    CFREE(tp4);
    CFREE(tp5);
    CFREE(tp6);
    CFREE(tp7);
    CFREE(tp8);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_2_DATA - write out the data into the PDB file */

static void write_test_2_data(PDBfile *strm)
   {long ind[3];

    ind[0] = 1L;
    ind[1] = N_INT;
    ind[2] = 1L;
    if (PD_write_alt(strm, "p", "integer", p_w, 1, ind) == 0)
       error(1, STDOUT, "P WRITE FAILED - WRITE_TEST_2_DATA\n");

    if (PD_write(strm, "tar", "lev1 *",  &tar_w) == 0)
       error(1, STDOUT, "TAR WRITE FAILED - WRITE_TEST_2_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_2_DATA - read the test data from the file */

static int read_test_2_data(PDBfile *strm)
   {int err;

    do_r = strm->default_offset;

    err = PD_read(strm, "tar", &tar_r);
    err = PD_read(strm, "p", p_r);

    err = PD_read(strm, "tar(0).a", &ap1);
    err = PD_read(strm, "tar(1).a", &ap2);

    err = PD_read(strm, "tar(0).a(0)", &aa[0]);
    err = PD_read(strm, "tar(0).a(1)", &aa[1]);
    err = PD_read(strm, "tar(1).a(0)", &aa[2]);
    err = PD_read(strm, "tar(1).a(1)", &aa[3]);

    err = PD_read(strm, "tar(0).b", &bp1);
    err = PD_read(strm, "tar(1).b", &bp2);

    err = PD_read(strm, "tar(0).b(0)", &ba[0]);
    err = PD_read(strm, "tar(0).b(1)", &ba[1]);
    err = PD_read(strm, "tar(1).b(0)", &ba[2]);
    err = PD_read(strm, "tar(1).b(1)", &ba[3]);

    err = PD_read(strm, "tar(0).c", &cp1);
    err = PD_read(strm, "tar(1).c", &cp2);

    err = PD_read(strm, "tar(0).c(0)", &ca[0]);
    err = PD_read(strm, "tar(0).c(1)", &ca[1]);
    err = PD_read(strm, "tar(1).c(0)", &ca[2]);
    err = PD_read(strm, "tar(1).c(1)", &ca[3]);

    err = PD_read(strm, "tar(0).c(0).s", &sp1);
    err = PD_read(strm, "tar(0).c(1).s", &sp2);
    err = PD_read(strm, "tar(1).c(0).s", &sp3);
    err = PD_read(strm, "tar(1).c(1).s", &sp4);

    err = PD_read(strm, "tar(0).c(0).s(0)", &tp1);
    err = PD_read(strm, "tar(0).c(0).s(1)", &tp2);
    err = PD_read(strm, "tar(0).c(1).s(0)", &tp3);
    err = PD_read(strm, "tar(0).c(1).s(1)", &tp4);

    err = PD_read(strm, "tar(1).c(0).s(0)", &tp5);
    err = PD_read(strm, "tar(1).c(0).s(1)", &tp6);
    err = PD_read(strm, "tar(1).c(1).s(0)", &tp7);
    err = PD_read(strm, "tar(1).c(1).s(1)", &tp8);

    err = PD_read(strm, "tar(0).c(0).s(0)(2)", &ta[0]);
    err = PD_read(strm, "tar(0).c(0).s(1)(1)", &ta[1]);
    err = PD_read(strm, "tar(0).c(1).s(0)(3)", &ta[2]);
    err = PD_read(strm, "tar(0).c(1).s(1)(2)", &ta[3]);

    err = PD_read(strm, "tar(1).c(0).s(0)(2)", &ta[4]);
    err = PD_read(strm, "tar(1).c(0).s(1)(1)", &ta[5]);
    err = PD_read(strm, "tar(1).c(1).s(0)(3)", &ta[6]);
    err = PD_read(strm, "tar(1).c(1).s(1)(2)", &ta[7]);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_2_DATA - print it out to the file */

static void print_test_2_data(FILE *fp)
   {int i;
    int *p1, *p2;
    double *p3, *p4;

    PRINT(fp, "\n");

    p1 = tar_r[0].a;
    p2 = tar_r[1].a;
    PRINT(fp, "TAR struct member A:\n");
    PRINT(fp, "    TAR[0].A    TAR[1].A\n");
    for (i = 0; i < N_INT; i++)
        PRINT(fp, "        %d          %d\n", p1[i], p2[i]);
    PRINT(fp, "\n");

    p3 = tar_r[0].b;
    p4 = tar_r[1].b;
    PRINT(fp, "TAR struct member B:\n");
    PRINT(fp, "    TAR[0].B    TAR[1].B\n");
    for (i = 0; i < N_DOUBLE; i++)
        PRINT(fp, "    %f    %f\n", p3[i], p4[i]);
    PRINT(fp, "\n");


    PRINT(fp, "TAR struct member C:\n");
    PRINT(fp, "    TAR[0].C[0]         TAR[0].C[1]\n");
    PRINT(fp, "   S[0]    S[1]        S[0]    S[1]\n");
    PRINT(fp, "  %s      %s        %s      %s\n",
                tar_r[0].c[0].s[0], tar_r[0].c[0].s[1],
                tar_r[0].c[1].s[0], tar_r[0].c[1].s[1]);
    PRINT(fp, "\n       TYPE\n");
    PRINT(fp, "     %d       %d\n",
                tar_r[0].c[0].type, tar_r[0].c[1].type);


    PRINT(fp, "\n    TAR[1].C[0]         TAR[1].C[1]\n");
    PRINT(fp, "   S[0]    S[1]        S[0]    S[1]\n");
    PRINT(fp, "   %s     %s           %s      %s\n",
                tar_r[1].c[0].s[0], tar_r[1].c[0].s[1],
                tar_r[1].c[1].s[0], tar_r[1].c[1].s[1]);

    PRINT(fp, "       TYPE\n");
    PRINT(fp, "     %d       %d\n",
                tar_r[1].c[0].type, tar_r[1].c[1].type);

    PRINT(fp, "\n");

/* print the offset */
    PRINT(fp, "file array index offset: %d\n\n", do_r);

    PRINT(fp, "\n");

/* read the an array which has offsets */
    PRINT(fp, "array p: \n");
    for (i = 0; i < N_INT; i++)
        PRINT(fp, "  %d\n", p_r[i]);

    PRINT(fp, "\n");

/* print the partial and member read stuff */
    if (ap1 != NULL)
       {PRINT(fp, "member read    TAR[0].A    TAR[1].A\n");
        for (i = 0; i < N_INT; i++)
            PRINT(fp, "                  %d          %d\n", ap1[i], ap2[i]);
        PRINT(fp, "\n");};
        
    if (bp1 != NULL)
       {PRINT(fp, "member read    TAR[0].B    TAR[1].B\n");
        for (i = 0; i < N_DOUBLE; i++)
            PRINT(fp, "               %f    %f\n", bp1[i], bp2[i]);
        PRINT(fp, "\n");};

    if (cp1 != NULL)
       {PRINT(fp, "member read TAR[0].C:\n");
        PRINT(fp, "    TAR[0].C[0]         TAR[0].C[1]\n");
        PRINT(fp, "   S[0]    S[1]        S[0]    S[1]\n");
        PRINT(fp, "  %s      %s        %s      %s\n",
                    cp1[0].s[0], cp1[0].s[1],
                    cp1[1].s[0], cp1[1].s[1]);
        PRINT(fp, "\n       TYPE\n");
        PRINT(fp, "     %d       %d\n",
                    cp1[0].type, cp1[1].type);

        PRINT(fp, "\nmember read TAR[1].C:\n");
        PRINT(fp, "    TAR[1].C[0]         TAR[1].C[1]\n");
        PRINT(fp, "   S[0]    S[1]        S[0]    S[1]\n");
        PRINT(fp, "   %s     %s           %s      %s\n",
                    cp2[0].s[0], cp2[0].s[1],
                    cp2[1].s[0], cp2[1].s[1]);
        PRINT(fp, "\n       TYPE\n");
        PRINT(fp, "     %d       %d\n",
                    cp2[0].type, cp2[1].type);};

/*
    PRINT(fp, "member read TAR[0].C[0]:\n");
    PRINT(fp, "   S[0]    S[1]\n");
    PRINT(fp, "  %s      %s\n", ca[0].s[0], ca[0].s[1]);

    PRINT(fp, "member read TAR[0].C[1]:\n");
    PRINT(fp, "   S[0]    S[1]\n");
    PRINT(fp, "  %s      %s\n", ca[1].s[0], ca[1].s[1]);

    PRINT(fp, "member read TAR[1].C[1]:\n");
    PRINT(fp, "   S[0]    S[1]\n");
    PRINT(fp, "  %s      %s\n", ca[2].s[0], ca[2].s[1]);

    PRINT(fp, "member read TAR[1].C[1]:\n");
    PRINT(fp, "   S[0]    S[1]\n");
    PRINT(fp, "  %s      %s\n", ca[3].s[0], ca[3].s[1]);
*/
/* TAR.C.S read */

    if (sp1 != NULL)
       {PRINT(fp, "\nmember read TAR[0].C[0].S:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", sp1[0], sp1[1]);

        PRINT(fp, "\nmember read TAR[0].C[1].S:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", sp2[0], sp2[1]);

        PRINT(fp, "\nmember read TAR[1].C[0].S:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", sp3[0], sp3[1]);

        PRINT(fp, "\nmember read TAR[1].C[1].S:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", sp4[0], sp4[1]);};

/* TAR.C.S[i] read */

    if (tp1 != NULL)
       {PRINT(fp, "\nmember read TAR[0].C[0].S[0]:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", tp1, tp1);

        PRINT(fp, "\nmember read TAR[0].C[1].S[1]:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", tp2, tp2);

        PRINT(fp, "\nmember read TAR[0].C[1].S[0]:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", tp3, tp3);

        PRINT(fp, "\nmember read TAR[0].C[1].S[1]:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", tp4, tp4);

        PRINT(fp, "\nmember read TAR[1].C[0].S[0]:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", tp5, tp5);

        PRINT(fp, "\nmember read TAR[1].C[0].S[1]:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", tp6, tp6);

        PRINT(fp, "\nmember read TAR[1].C[1].S[0]:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", tp7, tp7);

        PRINT(fp, "\nmember read TAR[1].C[1].S[1]:\n");
        PRINT(fp, "   S[0]    S[1]\n");
        PRINT(fp, "  %s      %s\n", tp8, tp8);

/* TAR.C.S[i][j] read */

        PRINT(fp, "\nmember read from TAR[0]:\n");
        PRINT(fp, "C[0].S[0][2]  C[0].S[1][1]  C[1].S[0][3]  C[1].S[1][2]\n");
        PRINT(fp, "     %c             %c           %c             %c\n",
                    ta[0], ta[1], ta[2], ta[3]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_2_DATA - compare the test data */

static int compare_test_2_data(PDBfile *strm, FILE *fp)
   {int i, err, err_tot;
    int *p1w, *p2w, *p1r, *p2r;
    double *p3w, *p4w, *p3r, *p4r;
    long double fptol[N_PRIMITIVE_FP];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

/* compare offset and array */
    err  = TRUE;
    err &= (do_r == do_w);

    for (i = 0; i < N_INT; i++)
        err &= (p_w[i] == p_r[i]);

    if (err)
       PRINT(fp, "Offset compare\n");
    else
       PRINT(fp, "Offset differ\n");
    err_tot &= err;

    err = TRUE;
    p1w = tar_w[0].a;
    p2w = tar_w[1].a;
    p1r = tar_r[0].a;
    p2r = tar_r[1].a;
    for (i = 0; i < N_INT; i++)
        {err &= (p1w[i] == p1r[i]);
         err &= (p2w[i] == p2r[i]);};

    p3w = tar_w[0].b;
    p4w = tar_w[1].b;
    p3r = tar_r[0].b;
    p4r = tar_r[1].b;
    for (i = 0; i < N_DOUBLE; i++)
        {err &= DOUBLE_EQUAL(p3w[i], p3r[i]);
         err &= DOUBLE_EQUAL(p4w[i], p4r[i]);};

    err &= (strcmp(tar_w[0].c[0].s[0], tar_r[0].c[0].s[0]) == 0);
    err &= (strcmp(tar_w[0].c[0].s[1], tar_r[0].c[0].s[1]) == 0);
    err &= (strcmp(tar_w[0].c[1].s[0], tar_r[0].c[1].s[0]) == 0);
    err &= (strcmp(tar_w[0].c[1].s[1], tar_r[0].c[1].s[1]) == 0);

    err &= (strcmp(tar_w[1].c[0].s[0], tar_r[1].c[0].s[0]) == 0);
    err &= (strcmp(tar_w[1].c[0].s[1], tar_r[1].c[0].s[1]) == 0);
    err &= (strcmp(tar_w[1].c[1].s[0], tar_r[1].c[1].s[0]) == 0);
    err &= (strcmp(tar_w[1].c[1].s[1], tar_r[1].c[1].s[1]) == 0);

    err &= (tar_w[0].c[0].type == tar_r[0].c[0].type);
    err &= (tar_w[0].c[1].type == tar_r[0].c[1].type);
    err &= (tar_w[1].c[0].type == tar_r[1].c[0].type);
    err &= (tar_w[1].c[1].type == tar_r[1].c[1].type);

    if (err)
       PRINT(fp, "Indirects compare\n");
    else
       PRINT(fp, "Indirects differ\n");
    err_tot &= err;

    PRINT(fp, "\n");

    if (err)
       PRINT(fp, "Offset compare\n");
    else
       PRINT(fp, "Offset differ\n");
    err_tot &= err;

    err = TRUE;

    if (ap1 != NULL)
       {p1w = tar_w[0].a;
        p2w = tar_w[1].a;
        for (i = 0; i < N_INT; i++)
            {err &= (p1w[i] == ap1[i]);
             err &= (p2w[i] == ap2[i]);};};

    if (bp1 != NULL)
       {p3w = tar_w[0].b;
        p4w = tar_w[1].b;
        for (i = 0; i < N_DOUBLE; i++)
            {err &= DOUBLE_EQUAL(p3w[i], bp1[i]);
             err &= DOUBLE_EQUAL(p4w[i], bp2[i]);};};

    if (cp1 != NULL)
       {err &= (strcmp(tar_w[0].c[0].s[0], cp1[0].s[0]) == 0);
        err &= (strcmp(tar_w[0].c[0].s[1], cp1[0].s[1]) == 0);
        err &= (strcmp(tar_w[0].c[1].s[0], cp1[1].s[0]) == 0);
        err &= (strcmp(tar_w[0].c[1].s[1], cp1[1].s[1]) == 0);

        err &= (strcmp(tar_w[1].c[0].s[0], cp2[0].s[0]) == 0);
        err &= (strcmp(tar_w[1].c[0].s[1], cp2[0].s[1]) == 0);
        err &= (strcmp(tar_w[1].c[1].s[0], cp2[1].s[0]) == 0);
        err &= (strcmp(tar_w[1].c[1].s[1], cp2[1].s[1]) == 0);

        err &= (tar_w[0].c[0].type == cp1[0].type);
        err &= (tar_w[0].c[1].type == cp1[1].type);
        err &= (tar_w[1].c[0].type == cp2[0].type);
        err &= (tar_w[1].c[1].type == cp2[1].type);};
/*
    err &= (strcmp(tar_w[0].c[0].s[0], ca[0].s[0]) == 0);
    err &= (strcmp(tar_w[0].c[0].s[1], ca[0].s[1]) == 0);
    err &= (strcmp(tar_w[0].c[1].s[0], ca[1].s[0]) == 0);
    err &= (strcmp(tar_w[0].c[1].s[1], ca[1].s[1]) == 0);

    err &= (strcmp(tar_w[1].c[0].s[0], ca[2].s[0]) == 0);
    err &= (strcmp(tar_w[1].c[0].s[1], ca[2].s[1]) == 0);
    err &= (strcmp(tar_w[1].c[1].s[0], ca[3].s[0]) == 0);
    err &= (strcmp(tar_w[1].c[1].s[1], ca[3].s[1]) == 0);
*/
    if (sp1 != NULL)
       {err &= (strcmp(tar_w[0].c[0].s[0], sp1[0]) == 0);
        err &= (strcmp(tar_w[0].c[0].s[1], sp1[1]) == 0);
        err &= (strcmp(tar_w[0].c[1].s[0], sp2[0]) == 0);
        err &= (strcmp(tar_w[0].c[1].s[1], sp2[1]) == 0);

        err &= (strcmp(tar_w[1].c[0].s[0], sp3[0]) == 0);
        err &= (strcmp(tar_w[1].c[0].s[1], sp3[1]) == 0);
        err &= (strcmp(tar_w[1].c[1].s[0], sp4[0]) == 0);
        err &= (strcmp(tar_w[1].c[1].s[1], sp4[1]) == 0);};

    if (tp1 != NULL)
       {err &= (strcmp(tar_w[0].c[0].s[0], tp1) == 0);
        err &= (strcmp(tar_w[0].c[0].s[1], tp2) == 0);
        err &= (strcmp(tar_w[0].c[1].s[0], tp3) == 0);
        err &= (strcmp(tar_w[0].c[1].s[1], tp4) == 0);

        err &= (strcmp(tar_w[1].c[0].s[0], tp5) == 0);
        err &= (strcmp(tar_w[1].c[0].s[1], tp6) == 0);
        err &= (strcmp(tar_w[1].c[1].s[0], tp7) == 0);
        err &= (strcmp(tar_w[1].c[1].s[1], tp8) == 0);

        err &= (tar_w[0].c[0].s[0][2] == ta[0]);
        err &= (tar_w[0].c[0].s[1][1] == ta[1]);
        err &= (tar_w[0].c[1].s[0][3] == ta[2]);
        err &= (tar_w[0].c[1].s[1][2] == ta[3]);};

    if (err)
       PRINT(fp, "Indirects compare\n");
    else
       PRINT(fp, "Indirects differ\n");
    err_tot &= err;

    PRINT(fp, "\n");

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
