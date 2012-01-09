/*
 * TPDLF.C - large file test
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define N       12
#define DATFILE "lf"

#define SET_FORMAT(fmt)                                                     \
    snprintf(fmt[0], 20, " %%14.7e");                                       \
    snprintf(fmt[1], 20, " %%23.16e");                                      \
    snprintf(fmt[2], 20, " %%41.34e")

enum
   {IREAD, IWRITE, IFREE};

typedef struct s_statedes statedes;
typedef struct s_triplet triplet;
typedef int (*PFTest)(statedes *st, int iv, int it);

struct s_triplet
   {inti n;
    float *fa;
    double *da;
    long double *la;};

struct s_statedes
   {int i;                /* test number */
    int nv;               /* number of triplets to write */
    int meta;             /* include metadata for std I/O */
    int quiet;
    inti n;
    int debug_mode;
    int mmap;             /* use memory mapped files */
    int64_t bfsz;         /* buffer size */
    int64_t nba0;
    int64_t nbf0;
    int64_t sz0;
    int nt[N];            /* number of iterations */
    double t[N];          /* total time */
    double sz[N];         /* file size */
    triplet tr;};

/*--------------------------------------------------------------------------*/

/*                         GENERAL PURPOSE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* SHOW_STAT - show the resource stats */

static void show_stat(statedes *st, char *tag)
   {int i;
    double dt;
    static int first = TRUE;

    if (st->quiet == TRUE)
       first = FALSE;

    if (first == TRUE)
       {first = FALSE;
	PRINT(STDOUT, "\n");
	PRINT(STDOUT, "\t\tTest                   Ni   FileSize     Time\n");
	PRINT(STDOUT, "\t\t                            (MBytes)    (secs)\n");};

    i  = st->i;
    dt = st->t[i];

    printf("\t\t%-14s %10lld %10.2e %10.2e\n",
	   tag, (long long) st->n, 1.0e-6*st->sz[i], dt);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_FILE - setup standard I/O files */

FILE *setup_file(statedes *st, char *fname, char *mode)
   {int rv;
    char s[MAXLINE];
    FILE *fp;

    fp = fopen(fname, mode);
    if (st->bfsz != -1)
       setvbuf(fp, NULL, _IOFBF, st->bfsz);

/* emulate reading metadata from file end */
    if (mode[0] == 'r')
       {rv = fseek(fp, -100, SEEK_END);
	fgets(s, MAXLINE, fp);
	rv = fseek(fp, 0, SEEK_SET);

	SC_ASSERT(rv == 0);};

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_DATA - setup the data for writing/reading */

triplet *setup_data(statedes *st, int wh, int iv)
   {inti i, n;
    float *fa;
    double *da;
    long double *la;
    triplet *tr;

    n  = st->n;
    tr = &st->tr;

    if (wh == IWRITE)
       {fa = CMAKE_N(float, n);
	da = CMAKE_N(double, n);
	la = CMAKE_N(long double, n);

	for (i = 0; i < n; i++)
	    {fa[i] = i + 0.01*iv;
	     da[i] = i + 0.01*iv;;
	     la[i] = i + 0.01*iv;};}

    else if (wh == IREAD)
       {fa = CMAKE_N(float, n);
	da = CMAKE_N(double, n);
	la = CMAKE_N(long double, n);

	for (i = 0; i < n; i++)
	    {fa[i] = 0.0;
	     da[i] = 0.0;
	     la[i] = 0.0;};}

    else if (wh == IFREE)
       {n  = 0;
	fa = tr->fa;
	da = tr->da;
	la = tr->la;
	CFREE(fa);
	CFREE(da);
	CFREE(la);};

    tr->n  = n;
    tr->fa = fa;
    tr->da = da;
    tr->la = la;

    return(tr);}

/*--------------------------------------------------------------------------*/

/*                           WPDB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_WPDB - write PDB data */

static int test_wpdb(statedes *st, int iv, int it)
   {int err;
    inti n, rv;
    char fname[MAXLINE], s[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    static PDBfile *fp = NULL;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-pdb.dat", DATFILE);

    n = st->n;

    tr = setup_data(st, IWRITE, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == NULL)
       {PD_set_io_hooks(st->mmap);
	PD_set_buffer_size(st->bfsz);

	fp = PD_open(fname, "w");};

    time = SC_wall_clock_time();

/* float */
    snprintf(s, MAXLINE, "fa%08d[%ld]", iv, (long) n);
    rv = PD_write(fp, s, "float", fa);

/* double */
    snprintf(s, MAXLINE, "da%08d[%ld]", iv, (long) n);
    rv = PD_write(fp, s, "double", da);

/* long double */
    snprintf(s, MAXLINE, "la%08d[%ld]", iv, (long) n);
    rv = PD_write(fp, s, "long_double", la);

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {PD_close(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = NULL;};

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                           RPDB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_RPDB - read PDB data */

static int test_rpdb(statedes *st, int iv, int it)
   {int err;
    inti rv;
    char fname[MAXLINE], s[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    static PDBfile *fp = NULL;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-pdb.dat", DATFILE);

    tr = setup_data(st, IREAD, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == NULL)
       {PD_set_io_hooks(st->mmap);
	PD_set_buffer_size(st->bfsz);

	fp = PD_open(fname, "r");};

    time = SC_wall_clock_time();

/* float */
    snprintf(s, MAXLINE, "fa%08d", iv);
    rv = PD_read(fp, s, fa);

/* double */
    snprintf(s, MAXLINE, "da%08d", iv);
    rv = PD_read(fp, s, da);

/* long double */
    snprintf(s, MAXLINE, "la%08d", iv);
    rv = PD_read(fp, s, la);

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {PD_close(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = NULL;};

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              DRIVER ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes
 *          - return the number of tests that fail
 */

static int run_test(statedes *st, PFTest test, char *tag)
   {int i, it, iv, nt, nv, cs, fail, dbg;
    char msg[MAXLINE];
    double th, tr, dt;

    dbg = (st->debug_mode == TRUE) ? -2 : 0;

/* NOTE: under the debugger set dbg to 1 or 2 for additional
 *       memory leak monitoring
 */
    cs = SC_mem_monitor(-1, dbg, "B", msg);

    fail = 0;

    th = 4.0;
    tr = 0.0;
    nt = 1;
    nv = st->nv;
    i  = st->i;
    st->sz[i] = 0.0;

/* iterate to smooth out timiing noise */
    st->t[i] = 0.0;
    for (it = 0; it < nt; it++)
        {dt = SC_wall_clock_time();

	 for (iv = 0; iv < nv; iv++)
	     {if ((*test)(st, iv, it) == FALSE)
		 {PRINT(STDOUT, "Test #%d failed\n", st->i);
		  fail++;};};
       
	 tr += (SC_wall_clock_time() - dt);

	 st->nt[i]++;

/* iterations stop when accumulated I/O time passes threshold value
 * or total wall clock time for iterations passes threshold value
 */
         if ((st->t[i] > th) || (tr > th))
	    break;};

    show_stat(st, tag);

    st->i++;

    cs = SC_mem_monitor(cs, dbg, "B", msg);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDLF - large file test\n\n");
    PRINT(STDOUT, "Usage: tpdlf [-b #] [-d] [-h] [-m]\n");
    PRINT(STDOUT, "            [-n #] [-nv #] [-q] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       b    set buffer size (default no buffering)\n");
    PRINT(STDOUT, "       d    turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h    print this help message and exit\n");
    PRINT(STDOUT, "       m    use memory mapped files\n");
    PRINT(STDOUT, "       n    number of items per arrays (default 100000000)\n");
    PRINT(STDOUT, "       nv   number of triplets (default 2)\n");
    PRINT(STDOUT, "       q    quiet mode\n");
    PRINT(STDOUT, "       v    PDB format version (default 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - measure ASCII vs binary I/O performance */

int main(int c, char **v)
   {int i, err;
    char fname[MAXLINE];
    statedes st;

    PD_init_threads(0, NULL);

    SC_bf_set_hooks();
    SC_zero_space_n(1, -2);

    memset(&st, 0, sizeof(st));
    st.nv            = 2;
    st.i             = 0;
    st.n             = 100000000;
    st.meta          = TRUE;
    st.quiet         = FALSE;
    st.debug_mode    = FALSE;
    st.bfsz          = -1;
    st.mmap          = FALSE;
    
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-md") == 0)
	    st.meta = FALSE;
	 else if (strcmp(v[i], "-n") == 0)
	    st.n = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-nv") == 0)
	    st.nv = SC_stoi(v[++i]);
 	 else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      st.bfsz = SC_stoi(v[++i]);
		      break;
		 case 'd' :
		      st.debug_mode  = TRUE;
		      SC_gs.mm_debug = TRUE;
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'm' :
		      st.mmap = TRUE;
		      break;
                 case 'q' :
		      st.quiet = TRUE;
		      break;
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;};}
         else
            break;};

    SC_signal(SIGINT, SIG_DFL);

    err = 0;

    err += run_test(&st, test_wpdb, "write-pdb");
    err += run_test(&st, test_rpdb, "read-pdb");

    snprintf(fname, MAXLINE, "%s-pdb.dat", DATFILE);
    unlink(fname);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
