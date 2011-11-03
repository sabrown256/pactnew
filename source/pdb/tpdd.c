/*
 * TPDD.C - measure ASCII vs binary I/O performance
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define N       10
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
    double rs, rt;
    static int first = TRUE;

    if (first == TRUE)
       {first = FALSE;
	PRINT(STDOUT, "\n");
	PRINT(STDOUT, "\tTest                   Ni   FileSize    Ratio       Time   SpeedUp\n");
	PRINT(STDOUT, "\t                            (kBytes)              (secs)\n");
};

    i  = st->i;
    ir = i & 1;

    if (i < 2)
       {rs = 1.0;
	rt = 1.0;}
    else
       {rs = st->sz[i]/st->sz[ir];
	rt = st->t[ir]/st->t[i];};

    printf("\t%-14s %10lld %10.2e %8.2f %10.2e %9.2e\n",
	   tag, (long long) st->n, 1.0e-3*st->sz[i], rs, st->t[i], rt);

    return;}

/*--------------------------------------------------------------------------*/

/*                            WFA TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_WFA - write ASCII data */

static int test_wfa(statedes *st)
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

/*                            RFA TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_RFA - read ASCII data */

static int test_rfa(statedes *st)
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

/*                            WFB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_WFB - write binary data with fwrite */

static int test_wfb(statedes *st)
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

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            RFB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_RFB - read binary data with fread */

static int test_rfb(statedes *st)
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

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            WSB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_WSB - write binary data with write */

static int test_wsb(statedes *st)
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

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            RSB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_RSB - read binary data with read */

static int test_rsb(statedes *st)
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

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                           WPDB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_WPDB - write PDB data */

static int test_wpdb(statedes *st)
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

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                           RPDB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_RPDB - read PDB data */

static int test_rpdb(statedes *st)
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

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                           WHDF TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

#ifdef HAVE_HDF5

#include <hdf5.h>
#include <hdf5_hl.h>
#include "H5LTpublic.h"

/* TEST_WHDF - write HDF5 data */

static int test_whdf(statedes *st)
   {int err;
    inti i, n;
    hsize_t ind[3];
    char fname[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    hid_t fp;
    herr_t rv;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-hdf5.dat", DATFILE);

    n = st->n;
    ind[0] = n;

    fa = CMAKE_N(float, n);
    da = CMAKE_N(double, n);
    la = CMAKE_N(long double, n);

    for (i = 0; i < n; i++)
        {fa[i] = i;
	 da[i] = i;
	 la[i] = i;};

    fp = H5Fcreate(fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    time = SC_wall_clock_time();

/* float */
    rv = H5LTmake_dataset(fp, "/fa", 1, ind, H5T_NATIVE_FLOAT, fa);

/* double */
    rv = H5LTmake_dataset(fp, "/da", 1, ind, H5T_NATIVE_DOUBLE, fa);

/* long double */
    rv = H5LTmake_dataset(fp, "/la", 1, ind, H5T_NATIVE_LDOUBLE, fa);

    st->t[st->i] += (SC_wall_clock_time() - time);

    CFREE(fa);
    CFREE(da);
    CFREE(la);

    rv = H5Fclose(fp);

    st->sz[st->i] += SC_file_length(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                           RHDF TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_RHDF - read HDF5 data */

static int test_rhdf(statedes *st)
   {int err;
    inti i, n;
    char fname[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    hid_t fp;
    herr_t rv;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-hdf5.dat", DATFILE);

    n = st->n;

    fa = CMAKE_N(float, n);
    da = CMAKE_N(double, n);
    la = CMAKE_N(long double, n);

    for (i = 0; i < n; i++)
        {fa[i] = 0;
	 da[i] = 0;
	 la[i] = 0;};

    fp = H5Fopen(fname, H5F_ACC_RDONLY, H5P_DEFAULT);

    time = SC_wall_clock_time();

/* float */
    rv = H5LTread_dataset(fp, "/fa", H5T_NATIVE_FLOAT, fa);

/* double */
    rv = H5LTread_dataset(fp, "/da", H5T_NATIVE_DOUBLE, da);

/* long double */
    rv = H5LTread_dataset(fp, "/la", H5T_NATIVE_LDOUBLE, la);

    st->t[st->i] += (SC_wall_clock_time() - time);

    CFREE(fa);
    CFREE(da);
    CFREE(la);

    rv = H5Fclose(fp);

    st->sz[st->i] += SC_file_length(fname);

    return(err);}

#endif

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

    PRINT(STDOUT, "\nTPDD - measure ASCII vs binary I/O performance\n\n");
    PRINT(STDOUT, "Usage: tpdd [-b #] [-d] [-h] [-m] [-n #]\n");
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

    err += run_test(&st, test_wfa, "write-ascii");
    err += run_test(&st, test_rfa, "read-ascii");

    err += run_test(&st, test_wpdb, "write-pdb");
    err += run_test(&st, test_rpdb, "read-pdb");

#ifdef HAVE_HDF5
    err += run_test(&st, test_whdf, "write-hdf5");
    err += run_test(&st, test_rhdf, "read-hdf5");
#endif

    err += run_test(&st, test_wfb, "fwrite-binary");
    err += run_test(&st, test_rfb, "fread-binary");

    err += run_test(&st, test_wsb, "write-binary");
    err += run_test(&st, test_rsb, "read-binary");

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
