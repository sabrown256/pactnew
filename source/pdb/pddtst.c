/*
 * PDDTST.C - measure ASCII vs binary I/O performance
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define N       6
#define DATFILE "io"

typedef struct s_statedes statedes;
typedef int (*PFTest)(statedes *st);

struct s_statedes
   {int i;                /* test number */
    inti n;
    int debug_mode;
    int64_t nba0;
    int64_t nbf0;
    int64_t sz0;
    double t[N];           /* total time */
    double sz[N];};        /* file size */

/*--------------------------------------------------------------------------*/

/*                         GENERAL PURPOSE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* SHOW_STAT - show the resource stats */

static void show_stat(statedes *st, char *tag)
   {int i, ir;
    int64_t nba, nbf, sz, ov;
    double time, rs, rt;
    static int first = TRUE;

    SC_mem_statr(&nba, &nbf, NULL, NULL, &sz, &ov);
    time = SC_wall_clock_time();

    if (first == TRUE)
       {first = FALSE;
	PRINT(STDOUT, "\n");
	PRINT(STDOUT, "\tTest                 Ni   FileSize    Ratio       Time   SpeedUp\n");
	PRINT(STDOUT, "\t                          (kBytes)              (secs)\n");
};

    i  = st->i;
    ir = i & 1;

    if (i < 2)
       {rs = 1.0;
	rt = 1.0;}
    else
       {rs = st->sz[i]/st->sz[ir];
	rt = st->t[ir]/st->t[i];};

    printf("\t%-12s %10lld %10.2e %8.2f %10.2e %9.2e\n",
	   tag, (long long) st->n, 1.0e-3*st->sz[i], rs, st->t[i], rt);

    return;}

/*--------------------------------------------------------------------------*/

/*                            TEST #1 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_1 - write ASCII data */

static int test_1(statedes *st)
   {int npl, err;
    inti i, n;
    char fname[MAXLINE], fmt[3][20];
    float *fa;
    double time;
    double *da;
    long double *la;
    FILE *fp;

    err = TRUE;

    snprintf(fmt[0], 20, " %%15.7e");
    snprintf(fmt[1], 20, " %%23.16e");
    snprintf(fmt[2], 20, " %%41.33e");

    n   = st->n;
    npl = 4;
    snprintf(fname, MAXLINE, "%s-asc.dat", DATFILE);

    fa = CMAKE_N(float, n);
    da = CMAKE_N(double, n);
    la = CMAKE_N(long double, n);

    for (i = 0; i < n; i++)
        {fa[i] = i;
	 da[i] = i;
	 la[i] = i;};

    fp = fopen(fname, "w");

    time = SC_wall_clock_time();

/* float */
    for (i = 0; i < n; i++)
        {fprintf(fp, fmt[0], fa[i]);
         if ((i+1) % npl == 0)
	    fprintf(fp, "\n");};
    fprintf(fp, "\n");

/* double */
    for (i = 0; i < n; i++)
        {fprintf(fp, fmt[1], da[i]);
         if ((i+1) % npl == 0)
	    fprintf(fp, "\n");};
    fprintf(fp, "\n");

/* long double */
    for (i = 0; i < n; i++)
        {fprintf(fp, fmt[2], la[i]);
         if ((i+1) % npl == 0)
	    fprintf(fp, "\n");};
    fprintf(fp, "\n");

    st->t[st->i] += (SC_wall_clock_time() - time);

    CFREE(fa);
    CFREE(da);
    CFREE(la);

    st->sz[st->i] += ftell(fp);

    fclose(fp);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #2 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_2 - read ASCII data */

static int test_2(statedes *st)
   {int j, npl, err;
    inti i, n;
    char fname[MAXLINE], s[MAXLINE];
    char *p, *ps, *t, *r;
    float *fa;
    double time;
    double *da;
    long double *la;
    FILE *fp;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-asc.dat", DATFILE);

    n = st->n;

    fa = CMAKE_N(float, n);
    da = CMAKE_N(double, n);
    la = CMAKE_N(long double, n);

    for (i = 0; i < n; i++)
        {fa[i] = 0;
	 da[i] = 0;
	 la[i] = 0;};

    fp = fopen(fname, "r");

    time = SC_wall_clock_time();

    npl = 4;

/* float */
    for (i = 0; i < n; )
        {p = fgets(s, MAXLINE, fp);
	 if (p == NULL)
	    break;
	 for (ps = p, j = 0; j < npl; ps = NULL, j++)
	     {t = SC_strtok(ps, " \n", r);
	      fa[i++] = SC_stof(t);};};

/* double */
    for (i = 0; i < n; )
        {p = fgets(s, MAXLINE, fp);
	 if (p == NULL)
	    break;
	 for (ps = p, j = 0; j < npl; ps = NULL, j++)
	     {t = SC_strtok(ps, " \n", r);
	      fa[i++] = SC_stof(t);};};

/* long double */
    for (i = 0; i < n; )
        {p = fgets(s, MAXLINE, fp);
	 if (p == NULL)
	    break;
	 for (ps = p, j = 0; j < npl; ps = NULL, j++)
	     {t = SC_strtok(ps, " \n", r);
	      fa[i++] = SC_stof(t);};};

    st->t[st->i] += (SC_wall_clock_time() - time);

    CFREE(fa);
    CFREE(da);
    CFREE(la);

    st->sz[st->i] += ftell(fp);

    fclose(fp);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #3 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_3 - write binary data */

static int test_3(statedes *st)
   {int err;
    inti i, n, rv;
    char fname[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    FILE *fp;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-bin.dat", DATFILE);

    n = st->n;

    fa = CMAKE_N(float, n);
    da = CMAKE_N(double, n);
    la = CMAKE_N(long double, n);

    for (i = 0; i < n; i++)
        {fa[i] = i;
	 da[i] = i;
	 la[i] = i;};

    fp = fopen(fname, "wb");

    time = SC_wall_clock_time();

/* float */
    rv = fwrite(fa, sizeof(float), n, fp);

/* double */
    rv = fwrite(da, sizeof(double), n, fp);

/* long double */
    rv = fwrite(la, sizeof(long double), n, fp);

    st->t[st->i] += (SC_wall_clock_time() - time);

    CFREE(fa);
    CFREE(da);
    CFREE(la);

    st->sz[st->i] += ftell(fp);

    fclose(fp);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #4 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_4 - read binary data */

static int test_4(statedes *st)
   {int err;
    inti i, n, rv;
    char fname[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    FILE *fp;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-bin.dat", DATFILE);

    n = st->n;

    fa = CMAKE_N(float, n);
    da = CMAKE_N(double, n);
    la = CMAKE_N(long double, n);

    for (i = 0; i < n; i++)
        {fa[i] = 0;
	 da[i] = 0;
	 la[i] = 0;};

    fp = fopen(fname, "rb");

    time = SC_wall_clock_time();

/* float */
    rv = fread(fa, sizeof(float), n, fp);

/* double */
    rv = fread(da, sizeof(double), n, fp);

/* long double */
    rv = fread(la, sizeof(long double), n, fp);

    st->t[st->i] += (SC_wall_clock_time() - time);

    CFREE(fa);
    CFREE(da);
    CFREE(la);

    st->sz[st->i] += ftell(fp);

    fclose(fp);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #5 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_5 - write PDB data */

static int test_5(statedes *st)
   {int err;
    inti i, n, rv;
    long ind[3];
    char fname[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    PDBfile *fp;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-pdb.dat", DATFILE);

    n = st->n;
    ind[0] = 0;
    ind[1] = n-1;
    ind[2] = 1;

    fa = CMAKE_N(float, n);
    da = CMAKE_N(double, n);
    la = CMAKE_N(long double, n);

    for (i = 0; i < n; i++)
        {fa[i] = i;
	 da[i] = i;
	 la[i] = i;};

    fp = PD_open(fname, "w");

    time = SC_wall_clock_time();

/* float */
    rv = PD_write_alt(fp, "fa", "float", fa, 1, ind);

/* double */
    rv = PD_write_alt(fp, "da", "double", da, 1, ind);

/* long double */
    rv = PD_write_alt(fp, "la", "long_double", la, 1, ind);

    st->t[st->i] += (SC_wall_clock_time() - time);

    CFREE(fa);
    CFREE(da);
    CFREE(la);

    PD_close(fp);

    st->sz[st->i] += SC_file_length(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #6 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_6 - read PDB data */

static int test_6(statedes *st)
   {int err;
    inti i, n, rv;
    char fname[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    PDBfile *fp;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-pdb.dat", DATFILE);

    n = st->n;

    fa = CMAKE_N(float, n);
    da = CMAKE_N(double, n);
    la = CMAKE_N(long double, n);

    for (i = 0; i < n; i++)
        {fa[i] = 0;
	 da[i] = 0;
	 la[i] = 0;};

    fp = PD_open(fname, "r");

    time = SC_wall_clock_time();

/* float */
    rv = PD_read(fp, "fa", fa);

/* double */
    rv = PD_read(fp, "da", da);

/* long double */
    rv = PD_read(fp, "la", la);

    st->t[st->i] += (SC_wall_clock_time() - time);

    CFREE(fa);
    CFREE(da);
    CFREE(la);

    PD_close(fp);

    st->sz[st->i] += SC_file_length(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              DRIVER ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes
 *          - return the number of tests that fail
 */

static int run_test(statedes *st, PFTest test, char *tag)
   {int cs, fail;
    char msg[MAXLINE];
    static int dbg = 0;

/* NOTE: under the debugger set dbg to 1 or 2 for additional
 *       memory leak monitoring
 */
    cs = SC_mem_monitor(-1, dbg, "B", msg);

    fail = 0;

    st->t[st->i]  = 0.0;
    st->sz[st->i] = 0.0;

    if ((*test)(st) == FALSE)
       {PRINT(STDOUT, "Test #%d failed\n", st->i);
	fail++;};

    show_stat(st, tag);

    st->i++;

    cs = SC_mem_monitor(cs, dbg, "B", msg);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nPDDTST - measure ASCII vs binary I/O performance\n\n");
    PRINT(STDOUT, "Usage: pddtst [-b #] [-d] [-h] [-m] [-n #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       b    set buffer size (default no buffering)\n");
    PRINT(STDOUT, "       d    turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h    print this help message and exit\n");
    PRINT(STDOUT, "       m    use memory mapped files\n");
    PRINT(STDOUT, "       n    number of items per arrays (default 1000)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - measure ASCII vs binary I/O performance */

int main(int c, char **v)
   {int i, err;
    int use_mapped_files;
    int64_t bfsz;
    statedes st;

    PD_init_threads(0, NULL);

    SC_bf_set_hooks();
    SC_zero_space_n(1, -2);

    st.i             = 0;
    st.n             = 100000;
    st.debug_mode    = FALSE;
    
    bfsz             = -1;
    bfsz             = 100000;
    use_mapped_files = FALSE;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-n") == 0)
	    st.n = SC_stoi(v[++i]);
 	 else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      bfsz = SC_stoi(v[++i]);
		      break;
		 case 'd' :
		      st.debug_mode  = TRUE;
		      SC_gs.mm_debug = TRUE;
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'm' :
		      use_mapped_files = TRUE;
		      break;};}
         else
            break;};

    PD_set_io_hooks(use_mapped_files);

    PD_set_buffer_size(bfsz);

    SC_signal(SIGINT, SIG_DFL);

    err = 0;

    err += run_test(&st, test_1, "write-ascii");
    err += run_test(&st, test_2, "read-ascii");
    err += run_test(&st, test_5, "write-pdb");
    err += run_test(&st, test_6, "read-pdb");
    err += run_test(&st, test_3, "write-binary");
    err += run_test(&st, test_4, "read-binary");

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
