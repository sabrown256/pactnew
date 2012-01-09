/*
 * TPDD.C - measure ASCII vs binary I/O performance
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"

#define N       12
#define DATFILE "io"

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
   {int i, ir, nt, nv;
    double nrm, rs, rt, tave, tref;
    static int first = TRUE;

    if (st->quiet == TRUE)
       first = FALSE;

    if (first == TRUE)
       {first = FALSE;
	PRINT(STDOUT, "\n");
	PRINT(STDOUT, "\tTest                   Ni   FileSize    Ratio       Time   SpeedUp\n");
	PRINT(STDOUT, "\t                            (kBytes)              (secs)\n");};

    i  = st->i;
    nv = st->nv;
    ir = i & 1;

    nt    = st->nt[i];
    tave  = st->t[i];
    nrm   = 1.0/(nt*nv);
    tave *= nrm;

    nt    = st->nt[ir];
    tref  = st->t[ir];
    nrm   = 1.0/(nt*nv);
    tref *= nrm;

    if (i < 2)
       {rs = 1.0;
	rt = 1.0;}
    else
       {rs = st->sz[i]/st->sz[ir];
	rt = tref/tave;};

    printf("\t%-14s %10lld %10.2e %8.2f %10.2e %9.2e\n",
	   tag, (long long) st->n, 1.0e-3*st->sz[i], rs, tave, rt);

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
/*--------------------------------------------------------------------------*/

/* GET_VAR_NAME - read to the next ASCII variable start
 *              - looking for line of form '# <var>[<n>]'
 */

static void get_var_name(statedes *st, FILE *fp)
   {char s[MAXLINE];
    char *p;

    if (st->meta == TRUE)
       {while ((p = fgets(s, MAXLINE, fp)) != NULL)
	   {if (*p == '#')
               break;};};

    return;}

/*--------------------------------------------------------------------------*/

/*                            WFA TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* EPRINTF - efficient ASCII fprintf */

static int eprintf(FILE *fp, char *fmt, ...)
   {static int ib = 0;
    static int64_t n = -10;
    static char *bf = NULL;

    if (n == -10)
       n = PD_get_buffer_size();
    if (n < 0)
       n = 4096;

    if (bf == NULL)
       bf = CMAKE_N(char, n);

    if (fmt == NULL)
       {fwrite(bf, 1, ib, fp);
	ib = 0;}

    else
       {SC_VA_START(fmt);
	ib += vsnprintf(bf+ib, n-ib, fmt, __a__);
	SC_VA_END;

	if (ib + 256 > n)
	   {fwrite(bf, 1, ib, fp);
	    ib = 0;};};

    return(ib);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_WFA - write ASCII data */

static int test_wfa(statedes *st, int iv, int it)
   {int npl, err;
    inti i, n;
    char fname[MAXLINE], fmt[3][20];
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    static FILE *fp = NULL;

    err = TRUE;

    SET_FORMAT(fmt);

    n   = st->n;
    npl = 4;
    snprintf(fname, MAXLINE, "%s-asc.dat", DATFILE);

    tr = setup_data(st, IWRITE, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == NULL)
       {fp = fopen(fname, "w");
	if (st->bfsz != -1)
	   setvbuf(fp, NULL, _IOFBF, st->bfsz);};

    time = SC_wall_clock_time();

/* float */
    if (st->meta == TRUE)
       eprintf(fp, "# float fa%08d[%ld]\n", iv, (long) n);
    for (i = 0; i < n; i++)
        {eprintf(fp, fmt[0], fa[i]);
         if ((i+1) % npl == 0)
	    eprintf(fp, "\n");};
    eprintf(fp, "\n");

/* double */
    if (st->meta == TRUE)
       eprintf(fp, "# double da%08d[%ld]\n", iv, (long) n);
    for (i = 0; i < n; i++)
        {eprintf(fp, fmt[1], da[i]);
         if ((i+1) % npl == 0)
	    eprintf(fp, "\n");};
    eprintf(fp, "\n");

/* long double */
    if (st->meta == TRUE)
       eprintf(fp, "# double la%08d[%ld]\n", iv, (long) n);
    for (i = 0; i < n; i++)
        {eprintf(fp, fmt[2], la[i]);
         if ((i+1) % npl == 0)
	    eprintf(fp, "\n");};
    eprintf(fp, "\n");

    eprintf(fp, NULL);

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {fclose(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = NULL;};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_WFAI - write ASCII data inefficiently */

static int test_wfai(statedes *st, int iv, int it)
   {int npl, err;
    inti i, n;
    char fname[MAXLINE], fmt[3][20];
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    static FILE *fp = NULL;

    err = TRUE;

    SET_FORMAT(fmt);

    n   = st->n;
    npl = 4;
    snprintf(fname, MAXLINE, "%s-asc.dat", DATFILE);

    tr = setup_data(st, IWRITE, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == NULL)
       {fp = fopen(fname, "w");
	if (st->bfsz != -1)
	   setvbuf(fp, NULL, _IOFBF, st->bfsz);};

    time = SC_wall_clock_time();

/* float */
    if (st->meta == TRUE)
       fprintf(fp, "# float fa%08d[%ld]\n", iv, (long) n);
    for (i = 0; i < n; i++)
        {fprintf(fp, fmt[0], fa[i]);
         if ((i+1) % npl == 0)
	    fprintf(fp, "\n");};
    fprintf(fp, "\n");

/* double */
    if (st->meta == TRUE)
       fprintf(fp, "# double da%08d[%ld]\n", iv, (long) n);
    for (i = 0; i < n; i++)
        {fprintf(fp, fmt[1], da[i]);
         if ((i+1) % npl == 0)
	    fprintf(fp, "\n");};
    fprintf(fp, "\n");

/* long double */
    if (st->meta == TRUE)
       fprintf(fp, "# double la%08d[%ld]\n", iv, (long) n);
    for (i = 0; i < n; i++)
        {fprintf(fp, fmt[2], la[i]);
         if ((i+1) % npl == 0)
	    fprintf(fp, "\n");};
    fprintf(fp, "\n");

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {fclose(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = NULL;};

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            RFA TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_RFA - read ASCII data */

static int test_rfa(statedes *st, int iv, int it)
   {int j, npl, err;
    inti i, n;
    char fname[MAXLINE], s[MAXLINE];
    char *p, *ps, *t, *r;
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    static FILE *fp = NULL;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-asc.dat", DATFILE);

    n = st->n;

    tr = setup_data(st, IREAD, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == NULL)
       fp = setup_file(st, fname, "r");

    time = SC_wall_clock_time();

    npl = 4;

/* float */
    get_var_name(st, fp);
    for (i = 0; i < n; )
        {p = fgets(s, MAXLINE, fp);
	 if (p == NULL)
	    break;
	 for (ps = p, j = 0; j < npl; ps = NULL, j++)
	     {t = SC_strtok(ps, " \n", r);
	      if (t == NULL)
		 break;
	      fa[i++] = SC_stof(t);};};

/* double */
    get_var_name(st, fp);
    for (i = 0; i < n; )
        {p = fgets(s, MAXLINE, fp);
	 if (p == NULL)
	    break;
	 for (ps = p, j = 0; j < npl; ps = NULL, j++)
	     {t = SC_strtok(ps, " \n", r);
	      if (t == NULL)
		 break;
	      da[i++] = SC_stof(t);};};

/* long double */
    get_var_name(st, fp);
    for (i = 0; i < n; )
        {p = fgets(s, MAXLINE, fp);
	 if (p == NULL)
	    break;
	 for (ps = p, j = 0; j < npl; ps = NULL, j++)
	     {t = SC_strtok(ps, " \n", r);
	      if (t == NULL)
		 break;
	      la[i++] = SC_stof(t);};};

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {fclose(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = NULL;};

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            WFB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_WFB - write binary data with fwrite */

static int test_wfb(statedes *st, int iv, int it)
   {int err;
    inti n, rv;
    char fname[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    static FILE *fp = NULL;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-binf.dat", DATFILE);

    n = st->n;

    tr = setup_data(st, IWRITE, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == NULL)
       fp = setup_file(st, fname, "wb");

    time = SC_wall_clock_time();

/* float */
    if (st->meta == TRUE)
       fprintf(fp, "# float fa%08d[%ld]\n", iv, (long) n);
    rv = fwrite(fa, sizeof(float), n, fp);

/* double */
    if (st->meta == TRUE)
       fprintf(fp, "# double da%08d[%ld]\n", iv, (long) n);
    rv = fwrite(da, sizeof(double), n, fp);

/* long double */
    if (st->meta == TRUE)
       fprintf(fp, "# double la%08d[%ld]\n", iv, (long) n);
    rv = fwrite(la, sizeof(long double), n, fp);

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {fclose(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = NULL;};

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            RFB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_RFB - read binary data with fread */

static int test_rfb(statedes *st, int iv, int it)
   {int err;
    inti n, rv;
    char fname[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    static FILE *fp = NULL;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-binf.dat", DATFILE);

    n = st->n;

    tr = setup_data(st, IREAD, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == NULL)
       fp = setup_file(st, fname, "rb");

    time = SC_wall_clock_time();

/* float */
    get_var_name(st, fp);
    rv = fread(fa, sizeof(float), n, fp);

/* double */
    get_var_name(st, fp);
    rv = fread(da, sizeof(double), n, fp);

/* long double */
    get_var_name(st, fp);
    rv = fread(la, sizeof(long double), n, fp);

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {fclose(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = NULL;};

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            WSB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_WSB - write binary data with write */

static int test_wsb(statedes *st, int iv, int it)
   {int err;
    inti n, rv;
    char fname[MAXLINE], s[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    static FILE *fp = NULL;
    static int fd = -1;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-bins.dat", DATFILE);

    n = st->n;

    tr = setup_data(st, IWRITE, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == NULL)
       {fp = setup_file(st, fname, "wb");
	fd = fileno(fp);};

    time = SC_wall_clock_time();

/* float */
    if (st->meta == TRUE)
       {snprintf(s, MAXLINE, "# float fa%08d[%ld]\n", iv, (long) n);
	rv = write(fd, s, strlen(s));};
    rv = write(fd, fa, n*sizeof(float));

/* double */
    if (st->meta == TRUE)
       {snprintf(s, MAXLINE, "# double da%08d[%ld]\n", iv, (long) n);
	rv = write(fd, s, strlen(s));};
    rv = write(fd, da, n*sizeof(double));

/* long double */
    if (st->meta == TRUE)
       {snprintf(s, MAXLINE, "# double la%08d[%ld]\n", iv, (long) n);
	rv = write(fd, s, strlen(s));};
    rv = write(fd, la, n*sizeof(long double));

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {fclose(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = NULL;
	fd = -1;};

    SC_ASSERT(rv > 0);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            RSB TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_RSB - read binary data with read */

static int test_rsb(statedes *st, int iv, int it)
   {int err;
    inti n, rv;
    char fname[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    static FILE *fp = NULL;
    static int fd = -1;

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-bins.dat", DATFILE);

    n = st->n;

    tr = setup_data(st, IREAD, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == NULL)
       {fp = setup_file(st, fname, "rb");
	fd = fileno(fp);};

    time = SC_wall_clock_time();

/* float */
    get_var_name(st, fp);
    rv = read(fd, fa, n*sizeof(float));

/* double */
    get_var_name(st, fp);
    rv = read(fd, da, n*sizeof(double));

/* long double */
    get_var_name(st, fp);
    rv = read(fd, la, n*sizeof(long double));

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {fclose(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = NULL;
	fd = -1;};

    SC_ASSERT(rv > 0);

    return(err);}

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

/*                           WHDF TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

#ifdef HAVE_HDF5

#include <hdf5.h>

/* TEST_WHDF - write HDF5 data */

static int test_whdf(statedes *st, int iv, int it)
   {int err;
    inti n;
    hsize_t ind[3];
    char fname[MAXLINE], s[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    herr_t rv;
    static hid_t fp = -1;
    extern int _H5_write_data(hid_t fid, char *fullpath,
			      int nd, hsize_t *dims,
			      hid_t htyp, void *data);

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-hdf5.dat", DATFILE);

    n = st->n;
    ind[0] = n;

    tr = setup_data(st, IWRITE, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == -1)
       fp = H5Fcreate(fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    time = SC_wall_clock_time();

/* float */
    snprintf(s, MAXLINE, "/fa%08d", iv);
    rv = _H5_write_data(fp, s, 1, ind, H5T_NATIVE_FLOAT, fa);

/* double */
    snprintf(s, MAXLINE, "/da%08d", iv);
    rv = _H5_write_data(fp, s, 1, ind, H5T_NATIVE_DOUBLE, fa);

/* long double */
    snprintf(s, MAXLINE, "/la%08d", iv);
    rv = _H5_write_data(fp, s, 1, ind, H5T_NATIVE_LDOUBLE, fa);

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {rv = H5Fclose(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = -1;};

    return(err);}

/*--------------------------------------------------------------------------*/

/*                           RHDF TEST ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* TEST_RHDF - read HDF5 data */

static int test_rhdf(statedes *st, int iv, int it)
   {int err;
    inti n;
    char fname[MAXLINE], s[MAXLINE];
    float *fa;
    double time;
    double *da;
    long double *la;
    triplet *tr;
    herr_t rv;
    static hid_t fp = -1;
    extern int _H5_read_data(hid_t fid, const char *fullpath, hid_t htyp, void *vr);

    err = TRUE;

    snprintf(fname, MAXLINE, "%s-hdf5.dat", DATFILE);

    n = st->n;

    tr = setup_data(st, IREAD, iv);
    fa = tr->fa;
    da = tr->da;
    la = tr->la;

    if (fp == -1)
       fp = H5Fopen(fname, H5F_ACC_RDONLY, H5P_DEFAULT);

    time = SC_wall_clock_time();

/* float */
    snprintf(s, MAXLINE, "/fa%08d", iv);
    rv = _H5_read_data(fp, s, H5T_NATIVE_FLOAT, fa);

/* double */
    snprintf(s, MAXLINE, "/da%08d", iv);
    rv = _H5_read_data(fp, s, H5T_NATIVE_DOUBLE, da);

/* long double */
    snprintf(s, MAXLINE, "/la%08d", iv);
    rv = _H5_read_data(fp, s, H5T_NATIVE_LDOUBLE, la);

    st->t[st->i] += (SC_wall_clock_time() - time);

    setup_data(st, IFREE, iv);

    if (iv+1 == st->nv)
       {rv = H5Fclose(fp);
	st->sz[st->i] = SC_file_length(fname);
	fp = -1;};

    return(err);}

#endif

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
    nt = 100000;
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

    PRINT(STDOUT, "\nTPDD - measure ASCII vs binary I/O performance\n\n");
    PRINT(STDOUT, "Usage: tpdd [-asc] [-b #] [-bin] [-d] [-h] [-hdf] [-m] [-md]\n");
    PRINT(STDOUT, "            [-n #] [-nv #] [-pdb] [-q] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       asc  do not do ASCII set\n");
    PRINT(STDOUT, "       b    set buffer size (default no buffering)\n");
    PRINT(STDOUT, "       bin  do not do binary set\n");
    PRINT(STDOUT, "       d    turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h    print this help message and exit\n");
    PRINT(STDOUT, "       hdf  do not do HDF5 set\n");
    PRINT(STDOUT, "       m    use memory mapped files\n");
    PRINT(STDOUT, "       md   do not write metadata for C std I/O methods\n");
    PRINT(STDOUT, "       n    number of items per arrays (default 1000)\n");
    PRINT(STDOUT, "       nv   number of triplets (default 1)\n");
    PRINT(STDOUT, "       pdb  do not do PDB set\n");
    PRINT(STDOUT, "       q    quiet mode\n");
    PRINT(STDOUT, "       v    PDB format version (default 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - measure ASCII vs binary I/O performance */

int main(int c, char **v)
   {int i, err, asc, bin, eff, pdb;
    statedes st;

#ifdef HAVE_HDF5
    int hdf;

    hdf = TRUE;
#endif

    PD_init_threads(0, NULL);

    SC_bf_set_hooks();
    SC_zero_space_n(1, -2);

    memset(&st, 0, sizeof(st));
    st.nv            = 1;
    st.i             = 0;
    st.n             = 100000;
    st.meta          = TRUE;
    st.quiet         = FALSE;
    st.debug_mode    = FALSE;
    st.bfsz          = -1;
    st.mmap          = FALSE;
    
    eff              = FALSE;
    asc              = TRUE;
    bin              = TRUE;
    pdb              = TRUE;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-asc") == 0)
	    asc = FALSE;
	 else if (strcmp(v[i], "-bin") == 0)
	    bin = FALSE;
#ifdef HAVE_HDF5
	 else if (strcmp(v[i], "-hdf") == 0)
	    hdf = FALSE;
#endif
	 else if (strcmp(v[i], "-md") == 0)
	    st.meta = FALSE;
	 else if (strcmp(v[i], "-n") == 0)
	    st.n = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-nv") == 0)
	    st.nv = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-pdb") == 0)
	    pdb = FALSE;
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

    if (asc == TRUE)
       {err += run_test(&st, test_wfai, "write-asci");
	err += run_test(&st, test_rfa,  "read-asci");};

    if (eff == TRUE)
       {err += run_test(&st, test_wfa, "write-asce");
	err += run_test(&st, test_rfa, "read-asce");};

#ifdef HAVE_HDF5
    if (hdf == TRUE)
       {err += run_test(&st, test_whdf, "write-hdf5");
	err += run_test(&st, test_rhdf, "read-hdf5");};
#endif

    if (pdb == TRUE)
       {err += run_test(&st, test_wpdb, "write-pdb");
	err += run_test(&st, test_rpdb, "read-pdb");};

    if (eff == TRUE)
       {err += run_test(&st, test_wfb, "fwrite-binary");
	err += run_test(&st, test_rfb, "fread-binary");};

    if (bin == TRUE)
       {err += run_test(&st, test_wsb, "write-binary");
	err += run_test(&st, test_rsb, "read-binary");};

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
