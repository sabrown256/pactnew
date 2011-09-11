/*
 * PDPTST.C - test for the PDB pointer handling scaling and performance
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

#define DATFILE "nat"

#define N_DOUBLE 3
#define N_BOOL   2
#define N_FLOAT  4

#define FLOAT_EQUAL(d1, d2)  (PM_value_compare(d1, d2, fptol[0]) == 0)
#define DOUBLE_EQUAL(d1, d2) (PM_value_compare(d1, d2, fptol[1]) == 0)
#define QUAD_EQUAL(d1, d2)   (PM_qvalue_compare(d1, d2, fptol[2]) == 0)

typedef struct s_statedes statedes;
typedef int (*PFTest)(statedes *st, char *base, char *tgt, int n);

struct s_statedes
   {int na;
    int ni;
    int debug_mode;
    int read_only;
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

/* TEST_TARGET - set up the target for the data file */

static void test_target(char *base, int n, char *fname, char *datfile)
   {

    sprintf(fname, "%s-nat.rs%d", base, n);
    sprintf(datfile, "%s-nat.db%d", base, n);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHOW_STAT - show the resource stats */

static void show_stat(statedes *st, char *tag)
   {int64_t nba, nbf, dna, dnf;
    double time, dt, sz, dsz;
    SC_rusedes ru;

    SC_mem_stats(&nba, &nbf, NULL, NULL);
    SC_resource_usage(&ru, -1);
    sz   = ru.maxrss;
    time = SC_wall_clock_time();

    if (tag != NULL)
       {dna   = nba  - st->nba0;
	dnf   = nbf  - st->nbf0;
	dsz   = sz   - st->sz0;
	dt    = time - st->time0;

/* memory in kBytes */
	PRINT(STDOUT,
	      "\t\t     %3s  %10.2e %10.2e      %.2g\n",
	      tag, 1.0e-3*(dna - dnf), dsz, dt);};

    st->nba0  = nba;
    st->nbf0  = nbf;
    st->sz0   = sz;
    st->time0 = time;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DUMP_TEST_SYMBOL_TABLE - dump the symbol table */

static void dump_test_symbol_table(statedes *st, FILE *fp,
				   hasharr *tab, int n)
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
/*--------------------------------------------------------------------------*/

/* ERROR - get out on an error */

static void error(int n, FILE *fp, char *fmt, ...)
   {char t[MAXLINE];
        
    SC_VA_START(fmt);
    SC_VSNPRINTF(t, MAXLINE, fmt);
    SC_VA_END;

    io_printf(fp, "%s", t);

    exit(n);

    return;}

/*--------------------------------------------------------------------------*/

/*                            TEST #1 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_1_DATA - prepare the test data */

static void prep_test_1_data(statedes *st)
   {int i, l, ni, na;
    double **a;

    na = st->na;
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

static void cleanup_test_1(statedes *st)
   {int na;
    double **a_w, **a_r;

    na  = st->na;
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

static int compare_test_1_data(statedes *st, PDBfile *strm, FILE *fp)
   {int i, l, ni, na, err, err_tot;
    long double fptol[N_PRIMITIVE_FP];
    double **a_r, **a_w;

    PD_fp_toler(strm, fptol);

    na  = st->na;
    ni  = st->ni;
    a_r = st->a_r;
    a_w = st->a_w;

    err_tot = TRUE;

/* compare arrays */
    err = TRUE;

    for (i = 0; i < na; i++)
        {for (l = 0; l < ni; l++);
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

static int test_1(statedes *st, char *base, char *tgt, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    double time;
    PDBfile *strm;
    FILE *fp;

    show_stat(st, NULL);

    time = SC_wall_clock_time();

/* target the file as asked */
    test_target(base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_1_data(st);

    if (st->read_only == FALSE)

/* create the named file */
       {strm = PD_create(datfile);
	if (strm == NULL)
	   error(1, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* write the test data */
	write_test_1_data(st, strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(1, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);};

    show_stat(st, "wr");

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       error(1, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* dump the symbol table */
    dump_test_symbol_table(st, fp, strm->symtab, 1);

/* read the data from the file */
    read_test_1_data(st, strm);

/* compare the original data with that read in */
    err = compare_test_1_data(st, strm, fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_1_data(st, fp);

    show_stat(st, "rd");

/* free known test data memory */
    cleanup_test_1(st);

    if (st->debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    show_stat(st, "fin");

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              DRIVER ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes
 *          - return the number of tests that fail
 */

static int run_test(statedes *st, PFTest test, int n, char *host)
   {int cs, fail;
    char msg[MAXLINE];
    static int dbg = 0;

/* NOTE: under the debugger set dbg to 1 or 2 for additional
 *       memory leak monitoring
 */
    cs = SC_mem_monitor(-1, dbg, "B", msg);

    fail = 0;

    if ((*test)(st, host, NULL, n) == FALSE)
       {PRINT(STDOUT, "Test #%d failed\n", n);
	fail++;};

    cs = SC_mem_monitor(cs, dbg, "B", msg);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nPDPTST - run pointer scaling and efficiency tests\n\n");
    PRINT(STDOUT, "Usage: pdctst [-b #] [-c] [-d] [-h] [-n] [-r] [-v #] [-1]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       b  - set buffer size (default no buffering)\n");
    PRINT(STDOUT, "       c  - verify low level writes\n");
    PRINT(STDOUT, "       d  - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       n  - run native mode test only\n");
    PRINT(STDOUT, "       r  - read only (assuming files from other run exist)\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "       1  - do NOT run test #1\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, err;
    int test_one;
    int use_mapped_files, check_writes;
    int64_t bfsz;
    statedes st;

    PD_init_threads(0, NULL);

    SC_bf_set_hooks();
    SC_zero_space_n(1, -2);

    st.na            = 10;
    st.ni            = 1000;
    st.debug_mode    = FALSE;
    st.read_only     = FALSE;
    
    bfsz             = -1;
    bfsz             = 100000;
    check_writes     = FALSE;
    use_mapped_files = FALSE;
    test_one         = TRUE;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-na") == 0)
	    st.na = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-ni") == 0)
	    st.ni = SC_stoi(v[++i]);
 	 else if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      bfsz = SC_stoi(v[++i]);
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
                 case 'r' :
		      st.read_only = TRUE;
		      break;
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;
                 case '1' :
		      test_one = FALSE;
		      break;};}
         else
            break;};

    PD_set_io_hooks(use_mapped_files);
    PD_verify_writes(check_writes);

    PD_set_buffer_size(bfsz);

    SC_signal(SIGINT, SIG_DFL);

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "\t\t                  Memory            Time\n");
    PRINT(STDOUT, "\t\t                 (kBytes)          (secs)\n");
    PRINT(STDOUT, "\t\tMeasured   Allocated   Resource\n");

    err = 0;

    if (test_one)
       err += run_test(&st, test_1, 1, DATFILE);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
