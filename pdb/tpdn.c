/*
 * TPDN.C - test for PDB net I/O
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

struct s_l_frame
   {float x_min;
    float x_max;
    float y_min;
    float y_max;};

typedef struct s_l_frame l_frame;

struct s_plot
   {float x_axis[N_CHAR];
    float y_axis[N_CHAR];
    int npts;
    char *label;
    l_frame view;};

typedef struct s_plot plot;

struct s_lev2
   {char **s;
    int type;};

typedef struct s_lev2 lev2;

struct s_lev1
   {int *a;
    double *b;
    lev2 *c;};

typedef struct s_lev1 lev1;

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

struct s_st4
   {short a;
    char  b;};

typedef struct s_st4 st4;

typedef int (*PFTest)(char *base, char *tgt, int n);

static st4
  *vr4_w,
  *vr4_r;

static st3
 vr1_w,
 vr1_r;

static lev1
 *tar_r,
 *tar_w;

static hasharr
 *tab4_r,
 *tab4_w;

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
 debug_mode  = FALSE,
 native_only = FALSE,
 mlt_cnnct   = TRUE,
 is_w,
 is_r,
 ia_w[N_INT],
 ia_r[N_INT],
 do_r,
 do_w,
 p_w[N_INT],
 p_r[N_INT],
 len;

static float
 fs_w,
 fs_r,
 fs_p1_r,
 fs_p2_r,
 fs_p3_r,
 fa_w[N_FLOAT][N_DOUBLE],
 fa_r[N_FLOAT][N_DOUBLE];

static double
 ds_w,
 ds_r,
 da_w[N_FLOAT],
 da_r[N_FLOAT];

static plot
 graph_w,
 graph_r;

static l_frame
 view_w,
 view_r;

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
	        {error(-1, stdout, "Test #%d %s failed\n", n, nm);
		 fail++;};};};

/* native test */
    if ((*test)(host, NULL, n) == FALSE)
       {error(-1, stdout, "Test #%d native failed\n", n);
	fail++;};

    post_test(&st, n);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IN_OUT - write and read an object into a buffer for later comparison */

long in_out(PDBfile *file, char *type, long n, void *vi, void *vo)
   {long nb, nba;
    char *bo, *bi;
    PDBfile *inf, *outf;

    nb = PD_sizeof(file, type, n, vi);

/* make the space too big to turn up in the intermediate allocations
 * this saves the test on some 32 bit platforms
 */
    nba = nb + 100;

    bo = CMAKE_N(char, nba);
    bi = CMAKE_N(char, nba);

    memset(bo, 0, nba);
    memset(bi, 0, nba);

/* write in to the buffer */
    outf = PN_open(file, bo);
    PN_write(outf, type, n, vi);
    PN_close(outf);

/* move the buffer contents thus invalidating all ITAG addresses */
    memcpy(bi, bo, nb);
    CFREE(bo);

/* read out of the buffer */
    inf = PN_open(file, bi);
    PN_relocate(inf, type, n);

    PN_read(inf, type, n, vo);
    PN_close(inf);

    CFREE(bi);

    return(nb);}

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
            {fa_w[i][k]     = POW((double) (i+1), (double) (k+1));
             fa_r[i][k]     = 0.0;};

/* set double array */
    for (i = 0; i < N_FLOAT; i++)
        {da_w[i] = POW(ds_w, (double) (i+1));
         da_r[i] = 0.0;};

/* set strings */
    SC_strncpy(ca_w, N_CHAR, "Hi there!", -1);
    len = strlen(ca_w) + 1;

    cap_w[0] = CSTRSAVE("lev1");
    cap_w[1] = CSTRSAVE("lev2");
    if (mlt_cnnct == TRUE)
       {cap_w[2] = cap_w[0];
	SC_mark(cap_w[0], 1);}
    else
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

/* SEND_TEST_1_DATA - write/read the data for test 1 */

static void send_test_1_data(PDBfile *strm)
   {

    in_out(strm, "char", 1L, &cs_w, &cs_r);
    in_out(strm, "short", 1L, &ss_w, &ss_r);
    in_out(strm, "int", 1L, &is_w, &is_r);
    in_out(strm, "float", 1L, &fs_w, &fs_r);
    in_out(strm, "double", 1L, &ds_w, &ds_r);

    in_out(strm, "char", len, &ca_w, &ca_r);
    in_out(strm, "short", 5L, &sa_w, &sa_r);
    in_out(strm, "int", 5L, &ia_w, &ia_r);
    in_out(strm, "float", N_FLOAT*N_DOUBLE, &fa_w, &fa_r);
    in_out(strm, "double", N_FLOAT, &da_w, &da_r);

    in_out(strm, "char *", N_DOUBLE, &cap_w, &cap_r);

    in_out(strm, "l_frame", 1L, &view_w, &view_r);
    in_out(strm, "plot",    1L, &graph_w, &graph_r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_1 - free all known test data memory */

static void cleanup_test_1(void)
   {

    CFREE(cap_w[0]);
    CFREE(cap_w[1]);
    if (mlt_cnnct == FALSE)
       CFREE(cap_w[2]);

    CFREE(cap_r[0]);
    CFREE(cap_r[1]);
    CFREE(cap_r[2]);

    CFREE(graph_w.label);
    CFREE(graph_r.label);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_1_DATA - print it out to STDOUT */

static void print_test_1_data(FILE *fp)
   {int i, k;

/* print scalars */
    PRINT(fp, "short scalar:   ss = %d\n", ss_r);
    PRINT(fp, "integer scalar: is = %d\n", is_r);
    PRINT(fp, "float scalar:   fs = %14.6e\n", fs_r);
    PRINT(fp, "double scalar:  ds = %14.6e\n", ds_r);

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
            PRINT(fp, "  fa[%d][%d] = %14.6e\n", i, k, fa_r[i][k]);

/* print double array */
    PRINT(fp, "double array:\n");
    for (i = 0; i < N_FLOAT; i++)
        PRINT(fp, "  da[%d] = %14.6e\n", i, da_r[i]);

/* print character pointer array */
    PRINT(fp, "string array:\n");
    for (i = 0; i < N_DOUBLE; i++)
        PRINT(fp, "  cap[%d] = %s\n", i, cap_r[i]);

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

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_1_DATA - compare the test data */

static int compare_test_1_data(PDBfile *strm, FILE *fp)
   {int i, err, err_tot;
    long double fptol[N_PRIMITIVE_FP];

    PD_fp_toler(strm, fptol);

    err_tot = TRUE;

/* compare scalars */
    err  = TRUE;
    err &= (cs_r == cs_w);
    err &= (ss_r == ss_w);
    err &= (is_r == is_w);
    err &= FLOAT_EQUAL(fs_r, fs_w);
    err &= DOUBLE_EQUAL(ds_r, ds_w);

    if (err)
       PRINT(fp, "Scalars compare\n");
    else
       PRINT(fp, "Scalars differ\n");
    err_tot &= err;

/* compare char array */
    err  = TRUE;
    for (i = 0; i < N_CHAR; i++)
        err &= (ca_r[i] == ca_w[i]);

/* compare short array */
    for (i = 0; i < N_INT; i++)
        err &= (sa_r[i] == sa_w[i]);

/* compare int array */
    for (i = 0; i < N_INT; i++)
        err &= (ia_r[i] == ia_w[i]);

/* compare double array */
    for (i = 0; i < N_FLOAT; i++)
        err &= DOUBLE_EQUAL(da_r[i], da_w[i]);

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");
    err_tot &= err;

/* compare strings */
    err  = TRUE;

    for (i = 0; i < N_DOUBLE; i++)
        err &= (strcmp(cap_r[i], cap_w[i]) == 0);

    if (err)
       PRINT(fp, "Strings compare\n");
    else
       PRINT(fp, "Strings differ\n");
    err_tot &= err;

/* compare structures */
    err  = TRUE;
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

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the fundamental PDBLib functions
 *        -
 *        - read and write scalars of all primitive types
 *        - read and write structures with no indirections
 *        - append data to a file and read it back
 *        - read structure members
 *        - read parts of arrays
 *        -
 *        - tests can be targeted
 */

static int test_1(char *base, char *tgt, int n)
   {int err;
    PDBfile *strm;
    char datfile[MAXLINE], fname[MAXLINE];
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_open_vif(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(1);};
    PRINT(fp, "File %s created\n", datfile);

    prep_test_1_data();

/* make a few defstructs */
    PD_defstr(strm, "l_frame",
              " float x_min",
              "float x_max",
              "float y_min",
              "float y_max",
              LAST);
    PD_defstr(strm, "plot",
              "float x_axis(10)",
              "float y_axis(10)",
              "int npts", 
              "char * label",
              "l_frame view",
               LAST);

    send_test_1_data(strm);

/* compare the original data with that read in */
    err = compare_test_1_data(strm, fp);

/* print it out to STDOUT */
    print_test_1_data(fp);

/* free known test data memory */
    cleanup_test_1();

    PD_close(strm);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #2 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_2_DATA - prepare the test data */

void prep_test_2_data(void)
   {int i, *p1, *p2;
    double *p3, *p4;

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

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_2 - free all know test data memory */

void cleanup_test_2(void)
   {

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

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEND_TEST_2_DATA - write/read the data for test 2 */

static void send_test_2_data(PDBfile *strm)
   {

    do_r = strm->default_offset;

    in_out(strm, "lev1 *",  1, &tar_w, &tar_r);
    in_out(strm, "int", N_INT, p_w, p_r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_2_DATA - print it out to the file */

void print_test_2_data(FILE *fp)
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

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_2_DATA - compare the test data */

static int compare_test_2_data(PDBfile *strm, FILE *fp)
   {int i, *p1w, *p2w, *p1r, *p2r;
    double *p3w, *p4w, *p3r, *p4r;
    int err, err_tot;
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

    PRINT(fp, "\n");

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test the PDBLib functions handling indirections
 *        -
 *        - read and write structures with pointers
 *        - set default array base indexes
 *        - read structure members
 *        - read parts of arrays
 *        -
 *        - tests can be targeted
 */

static int test_2(char *base, char *tgt, int n)
   {PDBfile *strm;
    char datfile[MAXLINE], fname[MAXLINE];
    int err;
    FILE *fp;

/* target the file is asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

    prep_test_2_data();

/* create the named file */
    strm = PD_open_vif(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(2);};
    PRINT(fp, "File %s created\n", datfile);

/* set the default offset */
    strm->default_offset = do_w;

/* make a few defstructs */
    PD_defstr(strm, "lev2",
              "char **s",
	      "int type",
              LAST);

    PD_defstr(strm, "lev1",
              "int *a",
	      "double *b",
	      "lev2 *c",
              LAST);

    send_test_2_data(strm);

/* compare the original data with that read in */
    err = compare_test_2_data(strm, fp);

/* print out the results */
    print_test_2_data(fp);

/* cleanup test data memory */
    cleanup_test_2();

    PD_close(strm);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #3 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_3_DATA - prepare the test data */

void prep_test_3_data(void)
   {vr1_w.a    = 'A';
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

void cleanup_test_3(void)
   {

    CFREE(vr1_w.j);
    CFREE(vr1_r.j);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEND_TEST_3_DATA - write/read the data for test 3 */

static void send_test_3_data(PDBfile *strm)
   {

    in_out(strm, "st3", 1, &vr1_w, &vr1_r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_3_DATA - print it out to the file */

void print_test_3_data(FILE *fp)
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

/* TEST_3 - test the PDBLib functions handling indirections
 *        -
 *        - read and write structures with alignment difficulties
 *        -
 *        - tests can be targeted
 */

static int test_3(char *base, char *tgt, int n)
   {PDBfile *strm;
    char datfile[MAXLINE], fname[MAXLINE];
    int err;
    FILE *fp;

/* target the file is asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_open_vif(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(2);};
    PRINT(fp, "File %s created\n", datfile);

    prep_test_3_data();

/* make a few defstructs */
    PD_defstr(strm, "st3",
              "char a",
              "short b",
              "char c[2]",
              "int d",
              "char e[3]",
              "float f",
              "char g[4]",
              "double h",
              "char i[5]",
              "char *j",
              "char k[6]",
              LAST);

    send_test_3_data(strm);

/* compare the original data with that read in */
    err = compare_test_3_data(strm, fp);

/* print out the results */
    print_test_3_data(fp);

/* clean up test data memory */
    cleanup_test_3();

    PD_close(strm);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

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

    pc  = CMAKE(char);
    *pc = 'A';   
    SC_hasharr_install(tab4_w, "pc", pc, G_STRING_S, 3, -1);

    ps  = CMAKE(short);
    *ps = -1024;
    SC_hasharr_install(tab4_w, "ps", ps, G_SHORT_P_S, 3, -1);

    pi  = CMAKE(int);
    *pi = 16384;
    SC_hasharr_install(tab4_w, "pi", pi, G_INT_P_S, 3, -1);

    pl  = CMAKE(long);
    *pl = -1048576;
    SC_hasharr_install(tab4_w, "pl", pl, G_LONG_P_S, 3, -1);

    pf  = CMAKE(float);
    *pf = 3.141596;
    SC_hasharr_install(tab4_w, "pf", pf, G_FLOAT_P_S, 3, -1);

    pd  = CMAKE(double);
    *pd = -1.0e-30;
    hp = SC_hasharr_install(tab4_w, "pd", pd, G_DOUBLE_P_S, 3, -1);

/* mark to keep reference count right and valgrind clean */
    SC_mark(hp, 1);

    SC_hasharr_install(tab4_w, "ph", hp, G_HAELEM_P_S, 3, -1);

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

    CFREE(vr4_w);
    CFREE(vr4_r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SEND_TEST_4_DATA - write/read the data for test 4 */

static void send_test_4_data(PDBfile *strm)
   {

    in_out(strm, "hasharr *", 1, &tab4_w, &tab4_r);
    SC_hasharr_rekey(tab4_r, SC_HA_NAME_KEY);

    in_out(strm, "st4 *", 1, &vr4_w, &vr4_r);

    return;}

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
    PRINT(fp, "   pc = %c %s\n", *pc, G_STRING_S);
    PRINT(fp, "   ps = %d %s\n", *ps, G_SHORT_P_S);
    PRINT(fp, "   pi = %d %s\n", *pi, G_INT_P_S);
    PRINT(fp, "   pl = %ld %s\n", *pl, G_LONG_P_S);
    PRINT(fp, "   pf = %f %s\n", *pf, G_FLOAT_P_S);
    PRINT(fp, "   pd = %e %s\n", *pd, G_DOUBLE_P_S);

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
#if 0
/* GOTCHA: this used to work but now type "double *" is read
 * back as "d"
 */
    err &= (strcmp(ph_r->type, ph_w->type) == 0);
#endif
    if (err)
       PRINT(fp, "Haelems compare\n");
    else
       PRINT(fp, "Haelems differ\n");
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

/* TEST_4 - test the PDBLib functions handling indirections
 *        - read and write structures with alignment difficulties
 *        - tests can be targeted
 */

static int test_4(char *base, char *tgt, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

/* create the named file */
    strm = PD_open_vif(datfile);
    if (strm == NULL)
       {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
        exit(2);};
    PRINT(fp, "File %s created\n", datfile);

    prep_test_4_data();

    strm->previous_file = CSTRSAVE(base);

/* make a few defstructs */
    PD_def_hash_types(strm, 3);

    PD_defstr(strm, "st4",
              "short a",
              "char b",
              LAST);

    PD_cast(strm, "haelem", "def", "type");

    send_test_4_data(strm);

/* compare the original data with that read in */
    err = compare_test_4_data(strm, fp);

/* print out the results */
    print_test_4_data(fp);

/* clean up test data memory */
    cleanup_test_4();

    PD_close(strm);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                         GENERAL PURPOSE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

void print_help(void)
   {

    PRINT(STDOUT, "\nTPDN - run PDB net test suite\n\n");
    PRINT(STDOUT, "Usage: tpdn [-d] [-h] [-n] [-v #] [-1] [-2] [-3] [-4]\n");
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

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, n, err;
    int ton[5];

    PD_init_threads(0, NULL);

    SC_zero_space_n(1, -2);
    debug_mode  = FALSE;
    native_only = FALSE;

    for (i = 0; i < 5; i++)
        ton[i] = TRUE;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'd' :
		      debug_mode = TRUE;
		      break;
                 case 'h' :
		      print_help();
		      return(1);
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
