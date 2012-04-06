/*
 * TPDCORE1.C - core PDB test #1
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

struct s_fr1
   {float x_min;
    float x_max;
    float y_min;
    float y_max;};

typedef struct s_fr1 fr1;

struct s_plot1
   {float x_axis[N_CHAR];
    float y_axis[N_CHAR];
    int npts;
    char *label;
    fr1 view;};

typedef struct s_plot1 plot1;

static char
 cs_w,
 cs_r,
 ca_w[N_CHAR],
 ca_r[N_CHAR],
 *cap_w[N_DOUBLE],
 *cap_r[N_DOUBLE];

static short
 ss_w,
 ss_r,
 sa_w[N_INT],
 sa_r[N_INT];

static int
 is_w,
 is_r,
 ia_w[N_INT],
 ia_r[N_INT],
 len;

static float
 fs_w,
 fs_r,
 fs_app_w,
 fs_app_r,
 fs_p1_r,
 fs_p2_r,
 fs_p3_r,
 fa1_r[N_FLOAT],
 fa2_w[N_FLOAT][N_DOUBLE],
 fa2_r[N_FLOAT][N_DOUBLE],
 fa2_app_w[N_FLOAT][N_DOUBLE],
 fa2_app_r[N_FLOAT][N_DOUBLE];

static double
 ds_w,
 ds_r,
 da_w[N_FLOAT],
 da_r[N_FLOAT];

static plot1
 graph_w,
 graph_r;

static fr1
 view_w,
 view_r;

/*--------------------------------------------------------------------------*/

/*                            TEST #1 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_1_DATA - prepare the test data */

static void prep_test_1_data(void)
   {int i, k;

/* set scalars */
    cs_r    = 0;
    ss_r    = 0;
    is_r    = 0;
    fs_r    = 0.0;
    fs_p1_r = 0.0;
    fs_p2_r = 0.0;
    fs_p3_r = 0.0;
    ds_r    = 0.0;

    cs_w     = 'Q';
    ss_w     = -514;
    is_w     =  10;
    fs_w     =  3.14159;
    fs_app_w = -3.14159;
    ds_w     =  exp(1.0);

/* set char array */
    for (i = 0; i < N_CHAR; i++)
        {ca_w[i] = '\0';
         ca_r[i] = '\0';};

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
            {fa2_w[i][k]     = POW((double) (i+1), (double) (k+1));
             fa2_app_w[i][k] = POW((double) (k+1), (double) (i+1));
             fa2_r[i][k]     = 0.0;
             fa2_app_r[i][k] = 0.0;};

/* set double array */
    for (i = 0; i < N_FLOAT; i++)
        {da_w[i] = POW(ds_w, (double) (i+1));
         da_r[i] = 0.0;};

/* set strings */
    strcpy(ca_w, "Hi there!");
    len = strlen(ca_w) + 1;

    cap_w[0] = CSTRSAVE("lev1");
    cap_w[1] = CSTRSAVE("lev2");
    cap_w[2] = CSTRSAVE("tar fu blat");
    cap_r[0] = NULL;
    cap_r[1] = NULL;
    cap_r[2] = NULL;

/* set structures */
    for (i = 0; i < N_CHAR; i++)
        {graph_w.x_axis[i] = ((float) i)/10.0;
         graph_w.y_axis[i] = 0.5 - graph_w.x_axis[i];
         graph_r.x_axis[i] = -1000.0;
         graph_r.y_axis[i] = -1000.0;};

    view_w.x_min = 0.1;
    view_w.x_max = 1.0;
    view_w.y_min = -0.5;
    view_w.y_max =  0.5;

    view_r.x_min = -1.e-10;
    view_r.x_max = -1.e-10;
    view_r.y_min = -1.e-10;
    view_r.y_max = -1.e-10;

    graph_w.npts  = N_CHAR;
    graph_w.label = CSTRSAVE("test graph");
    graph_w.view  = view_w;

    graph_r.npts  = 0;
    graph_r.label = NULL;

    graph_r.view.x_min = -1.e-10;
    graph_r.view.x_max = -1.e-10;
    graph_r.view.y_min = -1.e-10;
    graph_r.view.y_max = -1.e-10;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_1 - free all known test data memory */

static void cleanup_test_1(void)
   {

    CFREE(cap_w[0]);
    CFREE(cap_w[1]);
    CFREE(cap_w[2]);

    CFREE(cap_r[0]);
    CFREE(cap_r[1]);
    CFREE(cap_r[2]);

    CFREE(graph_w.label);
    CFREE(graph_r.label);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_1_DATA - write out the data into the PDB file */

static void write_test_1_data(PDBfile *strm)
   {long ind[6];

/* write scalars into the file */
    if (PD_write(strm, "cs", "char",    &cs_w) == 0)
       error(1, STDOUT, "CS WRITE FAILED - WRITE_TEST_1_DATA\n");
    if (PD_write(strm, "ss", "short",   &ss_w) == 0)
       error(1, STDOUT, "SS WRITE FAILED - WRITE_TEST_1_DATA\n");
    if (PD_write(strm, "is", "integer", &is_w) == 0)
       error(1, STDOUT, "IS WRITE FAILED - WRITE_TEST_1_DATA\n");
    if (PD_write(strm, "fs", "float",   &fs_w) == 0)
       error(1, STDOUT, "FS WRITE FAILED - WRITE_TEST_1_DATA\n");
    if (PD_write(strm, "ds", "double",  &ds_w) == 0)
       error(1, STDOUT, "DS WRITE FAILED - WRITE_TEST_1_DATA\n");

/* write primitive arrays into the file */
    ind[0] = 0L;
    ind[1] = len - 1;
    ind[2] = 1L;
    if (PD_write_alt(strm, "ca", "char", ca_w, 1, ind) == 0)
       error(1, STDOUT, "CA WRITE FAILED - WRITE_TEST_1_DATA\n");
    if (PD_write(strm, "sa(5)", "short", sa_w) == 0)
       error(1, STDOUT, "SA WRITE FAILED - WRITE_TEST_1_DATA\n");
    if (PD_write(strm, "ia(5)", "integer", ia_w) == 0)
       error(1, STDOUT, "IA WRITE FAILED - WRITE_TEST_1_DATA\n");

    ind[0] = 0L;
    ind[1] = N_FLOAT - 1;
    ind[2] = 1L;
    ind[3] = 0L;
    ind[4] = N_DOUBLE - 1;
    ind[5] = 1L;
    if (PD_write_alt(strm, "fa2", "float", fa2_w, 2, ind) == 0)
       error(1, STDOUT, "FA2 WRITE FAILED - WRITE_TEST_1_DATA\n");

    ind[0] = 0L;
    ind[1] = N_FLOAT - 1;
    ind[2] = 1L;
    if (PD_write_alt(strm, "da", "double", da_w, 1, ind) == 0)
       error(1, STDOUT, "DA WRITE FAILED - WRITE_TEST_1_DATA\n");

    ind[0] = 0L;
    ind[1] = N_DOUBLE - 1;
    ind[2] = 1L;
    if (PD_write_alt(strm, "cap", "char *", cap_w, 1, ind) == 0)
       error(1, STDOUT, "CAP WRITE FAILED - WRITE_TEST_1_DATA\n");

/* write structures into the file */
    if (PD_write(strm, "view", "fr1", &view_w) == 0)
       error(1, STDOUT, "VIEW WRITE FAILED - WRITE_TEST_1_DATA\n");

    if (PD_write(strm, "graph", "plot1", &graph_w) == 0)
       error(1, STDOUT, "GRAPH WRITE FAILED - WRITE_TEST_1_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* APPEND_TEST_1_DATA - append some test data to the file */

static void append_test_1_data(PDBfile *strm)
   {long ind[6];

    if (PD_write(strm, "fs_app", "float", &fs_app_w) == 0)
       error(1, STDOUT, "FS_APP WRITE FAILED - APPEND_TEST_1_DATA\n");

    ind[0] = 0L;
    ind[1] = N_FLOAT - 1;
    ind[2] = 1L;
    ind[3] = 0L;
    ind[4] = N_DOUBLE - 1;
    ind[5] = 1L;
    if (PD_write_alt(strm, "fa2_app", "float", fa2_app_w, 2, ind) == 0)
       error(1, STDOUT, "FA2_APP WRITE FAILED - APPEND_TEST_1_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_1_DATA - read the test data from the file */

static int read_test_1_data(PDBfile *strm)
   {int err;
    long ind[6];

/* read the scalar data from the file */
    err = PD_read(strm, "cs", &cs_r);
    err = PD_read(strm, "ss", &ss_r);
    err = PD_read(strm, "is", &is_r);
    err = PD_read(strm, "fs", &fs_r);
    err = PD_read(strm, "ds", &ds_r);

/* read the primitive arrays from the file */
    err = PD_read(strm, "ca",  ca_r);
    err = PD_read(strm, "sa",  sa_r);
    err = PD_read(strm, "ia",  ia_r);
    err = PD_read(strm, "fa2", fa2_r);
    err = PD_read(strm, "da",  da_r);
    err = PD_read(strm, "cap", cap_r);

/* read the entire structures from the file */
    err = PD_read(strm, "view",  &view_r);
    err = PD_read(strm, "graph", &graph_r);

/* read the appended data from the file */
    err = PD_read(strm, "fs_app",  &fs_app_r);
    err = PD_read(strm, "fa2_app", fa2_app_r);

/* partial array read */
    ind[0] = 1;
    ind[1] = 2;
    ind[2] = 1;
    ind[3] = 0;
    ind[4] = 1;
    ind[5] = 1;
    err = PD_read_alt(strm, "fa2", fa1_r, ind);

/* struct member test */
    ind[0] = 8;
    ind[1] = 8;
    ind[2] = 1;
    err = PD_read_alt(strm, "graph.x_axis",     &fs_p1_r, ind);
    err = PD_read(    strm, "graph.view.x_max", &fs_p2_r);
    err = PD_read(    strm, "view.y_max",       &fs_p3_r);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_1_DATA - print it out to STDOUT */

static void print_test_1_data(FILE *fp)
   {int i, k;

/* print scalars */
    PRINT(fp, "short scalar:        ss = %d\n", ss_r);
    PRINT(fp, "integer scalar:      is = %d\n", is_r);
    PRINT(fp, "float scalar:        fs = %14.6e\n", fs_r);
    PRINT(fp, "double scalar:       ds = %14.6e\n", ds_r);

    PRINT(fp, "\n");

/* print char array */
    PRINT(fp, "static char array:\n  ca = %s\n", ca_r);

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

/* print character pointer array */
    PRINT(fp, "string array:\n");
    for (i = 0; i < N_DOUBLE; i++)
        PRINT(fp, "  cap[%d] = %s\n", i, cap_r[i]);

    PRINT(fp, "\n");

/* print appended scalars */
    PRINT(fp, "appended float scalar: fs_app = %14.6e\n", fs_app_r);

/* print float array */
    PRINT(fp, "appended float array:\n");
    for (i = 0; i < N_FLOAT; i++)
        for (k = 0; k < N_DOUBLE; k++)
            PRINT(fp, "  fa2_app[%d][%d] = %14.6e\n",
                    i, k, fa2_app_r[i][k]);

    PRINT(fp, "\n");

/* whole struct test */
    PRINT(fp, "struct view:\n");
    PRINT(fp, "  x-min = %14.6e\n", view_r.x_min);
    PRINT(fp, "  x-max = %14.6e\n", view_r.x_max);
    PRINT(fp, "  y-min = %14.6e\n", view_r.y_min);
    PRINT(fp, "  y-max = %14.6e\n", view_r.y_max);

    PRINT(fp, "\n");

    PRINT(fp, "struct graph:\n");
    PRINT(fp, "  #pts  = %d\n",     graph_r.npts);
    PRINT(fp, "  x-min = %14.6e\n", graph_r.view.x_min);
    PRINT(fp, "  x-max = %14.6e\n", graph_r.view.x_max);
    PRINT(fp, "  y-min = %14.6e\n", graph_r.view.y_min);
    PRINT(fp, "  y-max = %14.6e\n", graph_r.view.y_max);

    PRINT(fp, "      x values             y values\n");
    for (i = 0; i < N_CHAR; i++)
        PRINT(fp, "   %14.6e       %14.6e\n",
              graph_r.x_axis[i], graph_r.y_axis[i]);

    PRINT(fp, "\n");

/* partial read single elements */
    PRINT(fp, "\npartial read scalars:\n");
    PRINT(fp, "  graph.x_axis[8]  = %14.6e\n", fs_p1_r);
    PRINT(fp, "  graph.view.x_max = %14.6e\n", fs_p2_r);
    PRINT(fp, "  view.y_max       = %14.6e\n", fs_p3_r);

    PRINT(fp, "\n");

/* partial read arrays */
    PRINT(fp, "partial read float array:\n");
    for (i = 0; i < N_FLOAT; i++)
        PRINT(fp, "  fa2_p[%d] = %14.6e\n", i, fa1_r[i]);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_1_DATA - compare the test data */

static int compare_test_1_data(PDBfile *strm, FILE *fp)
   {int i, k, err, err_tot;
    long double fptol[N_PRIMITIVE_FP];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

/* compare scalars */
    err  = TRUE;
    err &= (cs_r == cs_w);
    err &= (ss_r == ss_w);
    err &= (is_r == is_w);
    err &= FLOAT_EQUAL(fs_r, fs_w);
    err &= FLOAT_EQUAL(fs_app_r, fs_app_w);
    err &= DOUBLE_EQUAL(ds_r, ds_w);

    if (err)
       PRINT(fp, "Scalars compare\n");
    else
       PRINT(fp, "Scalars differ\n");
    err_tot &= err;

/* compare char array */
    err = TRUE;
    for (i = 0; i < N_CHAR; i++)
        err &= (ca_r[i] == ca_w[i]);

/* compare short array */
    for (i = 0; i < N_INT; i++)
        err &= (sa_r[i] == sa_w[i]);

/* compare int array */
    for (i = 0; i < N_INT; i++)
        err &= (ia_r[i] == ia_w[i]);

/* compare float array */
    for (i = 0; i < N_FLOAT; i++)
        for (k = 0; k < N_DOUBLE; k++)
            {err &= FLOAT_EQUAL(fa2_r[i][k], fa2_w[i][k]);
             err &= FLOAT_EQUAL(fa2_app_r[i][k], fa2_app_w[i][k]);}

/* compare double array */
    for (i = 0; i < N_FLOAT; i++)
        err &= DOUBLE_EQUAL(da_r[i], da_w[i]);

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");
    err_tot &= err;

/* compare strings */
    err = TRUE;
    for (i = 0; i < N_DOUBLE; i++)
        err &= (strcmp(cap_r[i], cap_w[i]) == 0);

    if (err)
       PRINT(fp, "Strings compare\n");
    else
       PRINT(fp, "Strings differ\n");
    err_tot &= err;

/* compare structures */
    err = TRUE;
    for (i = 0; i < N_CHAR; i++)
        {err &= FLOAT_EQUAL(graph_r.x_axis[i], graph_w.x_axis[i]);
         err &= FLOAT_EQUAL(graph_r.y_axis[i], graph_w.y_axis[i]);};

    err &= FLOAT_EQUAL(view_r.x_min, view_w.x_min);
    err &= FLOAT_EQUAL(view_r.x_max, view_w.x_max);
    err &= FLOAT_EQUAL(view_r.y_min, view_w.y_min);
    err &= FLOAT_EQUAL(view_r.y_max, view_w.y_max);

    err &= (graph_r.npts == graph_w.npts);
    err &= (strcmp(graph_r.label, graph_w.label) == 0);

    err &= FLOAT_EQUAL(graph_r.view.x_min, graph_w.view.x_min);
    err &= FLOAT_EQUAL(graph_r.view.x_max, graph_w.view.x_max);
    err &= FLOAT_EQUAL(graph_r.view.y_min, graph_w.view.y_min);
    err &= FLOAT_EQUAL(graph_r.view.y_max, graph_w.view.y_max);

    if (err)
       PRINT(fp, "Structs compare\n");
    else
       PRINT(fp, "Structs differ\n");
    err_tot &= err;

/* compare partial read results */
    err  = TRUE;
    err &= FLOAT_EQUAL(fs_p1_r, graph_w.x_axis[8]);
    err &= FLOAT_EQUAL(fs_p2_r, graph_w.view.x_max);
    err &= FLOAT_EQUAL(fs_p3_r, view_w.y_max);

    if (err)
       PRINT(fp, "Partial reads compare\n");
    else
       PRINT(fp, "Partial reads differ\n");
    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
