/*
 * TPDP.C - test for the PDB pointer handling scaling and performance
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "nat"

#define N_DOUBLE 3
#define N_BOOL   2
#define N_FLOAT  4

#define FLOAT_EQUAL(d1, d2)  (PM_value_compare(d1, d2, fptol[0]) == 0)
#define DOUBLE_EQUAL(d1, d2) (PM_value_compare(d1, d2, fptol[1]) == 0)
#define QUAD_EQUAL(d1, d2)   (PM_qvalue_compare(d1, d2, fptol[2]) == 0)

typedef struct s_statedes statedes;
typedef int (*PFTest)(statedes *st, char *base, char *tgt, int n, int na);

struct s_statedes
   {int na;
    int ni;
    int dna;
    int dni;
    int debug_mode;
    int read_only;
    int track;
    int64_t nba0;
    int64_t nbf0;
    int64_t sz0;
    double time0;
    double **a_r;
    double **a_w;};

extern long _PD_lookup_size(char *s, hasharr *tab);

/*--------------------------------------------------------------------------*/

/*                         GENERAL PURPOSE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* TEST_TGT - set up the target for the data file */

static void test_tgt(char *base, int n, char *fname, char *datfile,
		     long nc)
   {

    snprintf(fname, nc, "%s-nat.rs%d", base, n);
    snprintf(datfile, nc, "%s-nat.db%d", base, n);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHOW_STAT - show the resource stats */

static void show_stat(statedes *st, char *tag, int na)
   {int64_t nba, nbf, dna, dnf, sz, ov;
    double ltm, dt, dnb, dsz;
    static int first = TRUE;

    SC_mem_statr(&nba, &nbf, NULL, NULL, &sz, &ov);
    ltm = SC_wall_clock_time();

    if (first == TRUE)
       {first = FALSE;
	PRINT(STDOUT, "\n");
	PRINT(STDOUT, "\t                                  Memory               Time\n");
	PRINT(STDOUT, "\tMeasured   Na        Ni   Allocated   Res  Over\n");
	PRINT(STDOUT, "\t                           (kBytes)                  (secs)\n");
};

    if (tag != NULL)
       {dna   = nba  - st->nba0;
	dnf   = nbf  - st->nbf0;
	dnb   = 1.0e-3*(dna - dnf);
	dsz   = 1.0e-3*(sz  - st->sz0);
	dt    = ltm - st->time0;

/* memory in kBytes */
	PRINT(STDOUT,
	      "\t%-3s  %8d  %8d  %10.2e %4.0f%% %4.0f%%      %.2g\n",
	      tag, na, st->ni, dnb, 100.0*dsz/dnb, 0.1*ov/dnb, dt);};

    st->nba0  = nba;
    st->nbf0  = nbf;
    st->sz0   = sz;
    st->time0 = ltm;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DUMP_TEST_SYMTAB - dump the symbol table */

static void dump_test_symtab(statedes *st, FILE *fp, hasharr *tab, int n)
   {int i, ne;
    char **names;

    PRINT(fp, "\nTest %d Symbol table:\n", n);

    ne    = SC_hasharr_get_n(tab);
    names = SC_hasharr_dump(tab, NULL, NULL, FALSE);
    for (i = 0; i < ne; i++)
        PRINT(fp, "%s\n", names[i]);
    SC_free_strings(names);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/

/*                            TEST #1 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_1_DATA - prepare the test data */

static void prep_test_1_data(statedes *st, int na)
   {int i, l, ni;
    double **a;

    ni = st->ni;

    a = PM_make_vectors(na, ni);

    for (i = 0; i < na; i++)
        {for (l = 0; l < ni; l++)
	     a[i][l] = i + l;};
	       
    st->a_w = a;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_1 - free all known test data memory */

static void cleanup_test_1(statedes *st, int na)
   {double **a_w, **a_r;

    a_r = st->a_r;
    a_w = st->a_w;

    PM_free_vectors(na, a_r);
    PM_free_vectors(na, a_w);
	       
    st->a_r = NULL;
    st->a_w = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_1_DATA - write out the data into the PDB file */

static void write_test_1_data(statedes *st, PDBfile *strm)
   {double **a_w;

    a_w = st->a_w;

/* write primitive arrays into the file */
    if (PD_write(strm, "a", "double **", &a_w) == 0)
       error(1, STDOUT, "A WRITE FAILED - WRITE_TEST_1_DATA\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_1_DATA - read the test data from the file */

static int read_test_1_data(statedes *st, PDBfile *strm)
   {int err;
    double **a_r;

/* read the scalar data from the file */
    err = PD_read(strm, "a", &a_r);

    st->a_r = a_r;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_1_DATA - print it out to STDOUT */

static void print_test_1_data(statedes *st, FILE *fp)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_1_DATA - compare the test data */

static int compare_test_1_data(statedes *st, PDBfile *strm, FILE *fp, int na)
   {int i, l, ni, err, err_tot;
    long double fptol[N_PRIMITIVE_FP];
    double **a_r, **a_w;

    PD_fp_toler(strm, fptol);

    ni  = st->ni;
    a_r = st->a_r;
    a_w = st->a_w;

    err_tot = TRUE;

/* compare arrays */
    err = TRUE;

    for (i = 0; i < na; i++)
        {for (l = 0; l < ni; l++)
	     err &= (a_r[i][l] == a_w[i][l]);};

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");
    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the memory usage of pointers */

static int test_1(statedes *st, char *base, char *tgt, int n, int na)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

    show_stat(st, NULL, na);

/* target the file as asked */
    test_tgt(base, n, fname, datfile, MAXLINE);

    fp = io_open(fname, "w");

    prep_test_1_data(st, na);

    if (st->read_only == FALSE)

/* create the named file */
       {strm = PD_create(datfile);
	if (strm == NULL)
	   error(1, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

	PD_set_track_pointers(strm, st->track);

/* write the test data */
	write_test_1_data(st, strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(1, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);};

    show_stat(st, "wr", na);

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       error(1, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

    PD_set_track_pointers(strm, st->track);

/* dump the symbol table */
    dump_test_symtab(st, fp, strm->symtab, 1);

/* read the data from the file */
    read_test_1_data(st, strm);

/* compare the original data with that read in */
    err = compare_test_1_data(st, strm, fp, na);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_1_data(st, fp);

    show_stat(st, "rd", na);

/* free known test data memory */
    cleanup_test_1(st, na);

    if (st->debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

/*    show_stat(st, "fin"); */

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              DRIVER ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes
 *          - return the number of tests that fail
 */

static int run_test(statedes *st, PFTest test, int n, char *host)
   {int ia, dna, cs, fail;
    char msg[MAXLINE];
    static int dbg = 0;

/* NOTE: under the debugger set dbg to 1 or 2 for additional
 *       memory leak monitoring
 */
    cs = SC_mem_monitor(-1, dbg, "B", msg);

    fail = 0;
    dna  = st->dna;

    for (ia = dna; ia <= st->na; ia *= dna)
        {if ((*test)(st, host, NULL, n, ia) == FALSE)
	    {error(-1, stdout, "Test #%d failed\n", n);
	     fail++;};};

    cs = SC_mem_monitor(cs, dbg, "B", msg);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDP - run pointer scaling and efficiency tests\n\n");
    PRINT(STDOUT, "Usage: tpdp [-b #] [-c] [-d] [-dna #] [-dni #] [-h] [-m] [-na #] [-ni #] [-r] [-v #]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       b    set buffer size (default no buffering)\n");
    PRINT(STDOUT, "       c    verify low level writes\n");
    PRINT(STDOUT, "       d    turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       dna  factor to scale NA each iteration (default 10)\n");
    PRINT(STDOUT, "       dni  factor to scale NI each iteration (default 10)\n");
    PRINT(STDOUT, "       h    print this help message and exit\n");
    PRINT(STDOUT, "       m    use memory mapped files\n");
    PRINT(STDOUT, "       na   number of dynamic arrays (default 10000)\n");
    PRINT(STDOUT, "       ni   number of items per arrays (default 1000)\n");
    PRINT(STDOUT, "       r    read only (assuming files from other run exist)\n");
    PRINT(STDOUT, "       v    use format version # (default is 2)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, err;
    int use_mapped_files, check_writes;
    long no;
    int64_t bfsz;
    statedes st;

    PD_init_threads(0, NULL);

    SC_bf_set_hooks();
    SC_zero_space_n(1, -2);

    st.na            = 10000;
    st.dna           = 10;
    st.ni            = 1000;
    st.dni           = 10;
    st.debug_mode    = FALSE;
    st.read_only     = FALSE;
    st.track         = TRUE;
    
    bfsz             = -1;
    bfsz             = SC_OPT_BFSZ;
    check_writes     = FALSE;
    use_mapped_files = FALSE;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-na") == 0)
	    st.na = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-dna") == 0)
	    st.dna = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-ni") == 0)
	    st.ni = SC_stol(v[++i]);
	 else if (strcmp(v[i], "-dni") == 0)
	    st.dni = SC_stol(v[++i]);
 	 else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      bfsz = SC_stol(v[++i]);
		      break;
                 case 'c' :
		      check_writes = TRUE;
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
		      break;
                 case 'p' :
		      st.track = FALSE;
		      break;
                 case 'r' :
		      st.read_only = TRUE;
		      break;
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;};}
         else
            break;};

    no = 1.013*st.na;
    PD_set_pointer_size(no);

    PD_set_io_hooks(use_mapped_files);
    PD_verify_writes(check_writes);

    PD_set_buffer_size(bfsz);

    SC_signal(SIGINT, SIG_DFL);

    err = 0;

    err += run_test(&st, test_1, 1, DATFILE);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
