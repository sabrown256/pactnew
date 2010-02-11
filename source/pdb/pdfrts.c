/*
 * PDFRTS.C - test PD_free
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb.h"

#define DATFILE "nat"

#define N_DOUBLE 3
#define N_INT    5

typedef struct s_lev1 lev1;
typedef struct s_lev2 lev2;

struct s_lev2
   {char **s;
    int type;};

struct s_lev1
   {int *a;
    double *b;
    lev2 *c;};

typedef int (*PFTest)(char *base, char *tgt, int n);

static lev1
 *tar_r,
 *tar_w;

static int
 write_only  = FALSE,
 read_only   = FALSE;

/* variables for partial and member reads in test #2 */

static int
 *ap1,
 *ap2;

static double
 *bp1,
 *bp2;

static lev2
 *cp1,
 *cp2,
 ca[4];

static char
 **sp1,
 **sp2,
 *tp1,
 *tp2;

/*--------------------------------------------------------------------------*/

/*                         GENERAL PURPOSE ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* TEST_TARGET - set up the target for the data file */

static void test_target(char *tgt, char *base, int n,
		        char *fname, char *datfile)
   {

    sprintf(fname, "%s-nat.rs%d", base, n);
    sprintf(datfile, "%s-nat.db%d", base, n);

    return;}

/*--------------------------------------------------------------------------*/

/*                            TEST #2 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_1_DATA - prepare the test data */

static void prep_test_1_data(void)
   {int i, *p1, *p2;
    double *p3, *p4;

    tar_w = FMAKE_N(lev1, 2, "PREP_TEST_1_DATA:tar_w");

    p1 = tar_w[0].a = FMAKE_N(int, N_INT, "PREP_TEST_1_DATA:tar_w[0].a");
    p2 = tar_w[1].a = FMAKE_N(int, N_INT, "PREP_TEST_1_DATA:tar_w[1].a");
    for (i = 0; i < N_INT; i++)
        {p1[i] = i;
         p2[i] = i + 10;};

    p3 = tar_w[0].b = FMAKE_N(double, N_DOUBLE,
                              "PREP_TEST_1_DATA:tar_w[0].b");
    p4 = tar_w[1].b = FMAKE_N(double, N_DOUBLE,
                              "PREP_TEST_1_DATA:tar_w[1].b");
    for (i = 0; i < N_DOUBLE; i++)
        {p3[i] = exp((double) i);
         p4[i] = log(1.0 + (double) i);};

    tar_w[0].c = FMAKE_N(lev2, 2, "PREP_TEST_1_DATA:tar_w[0].c");
    tar_w[1].c = FMAKE_N(lev2, 2, "PREP_TEST_1_DATA:tar_w[0].c");

    tar_w[0].c[0].s    = FMAKE_N(char *, 2,
                                 "PREP_TEST_1_DATA:tar_w[0].c[0].s");
    tar_w[0].c[0].s[0] = SC_strsavef("Hello",
                           "char*:PREP_TEST_1_DATA:Hello");
    tar_w[0].c[0].s[1] = SC_strsavef(" ", "char*:PREP_TEST_1_DATA:blank");
    tar_w[0].c[1].s    = FMAKE_N(char *, 2,
                                 "PREP_TEST_1_DATA:tar_w[0].c[1].s");
    tar_w[0].c[1].s[0] = SC_strsavef("world",
                           "char*:PREP_TEST_1_DATA:world");
    tar_w[0].c[1].s[1] = SC_strsavef("!", "char*:PREP_TEST_1_DATA:!");

    tar_w[1].c[0].s    = FMAKE_N(char *, 2,
                                 "PREP_TEST_1_DATA:tar_w[1].c[0].s");
    tar_w[1].c[0].s[0] = SC_strsavef("Foo", "char*:PREP_TEST_1_DATA:Foo");
    tar_w[1].c[0].s[1] = SC_strsavef(" ", "char*:PREP_TEST_1_DATA:blank");
    tar_w[1].c[1].s    = FMAKE_N(char *, 2,
                                 "PREP_TEST_1_DATA:tar_w[1].c[1].s");
    tar_w[1].c[1].s[0] = SC_strsavef("Bar", "char*:PREP_TEST_1_DATA:Bar");
    tar_w[1].c[1].s[1] = SC_strsavef("!!!", "char*:PREP_TEST_1_DATA:!!!");

    tar_w[0].c[0].type = 1;
    tar_w[0].c[1].type = 2;
    tar_w[1].c[0].type = 3;
    tar_w[1].c[1].type = 4;

    tar_r = NULL;

    ap1 = NULL;
    ap2 = NULL;

    bp1 = NULL;
    bp2 = NULL;

    cp1 = NULL;
    cp2 = NULL;
    for (i = 0; i < 4; i++)
        {ca[i].s    = NULL;
         ca[i].type = 0;};

    sp1 = NULL;
    sp2 = NULL;

    tp1 = NULL;
    tp2 = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_1 - free all know test data memory */

static int cleanup_test_1(PDBfile *strm)
   {int i, rv;

    rv = TRUE;

    if (tar_w != NULL)
       rv &= (PD_free(strm, "lev1", tar_w) == TRUE);

    if (tar_r != NULL)
       rv &= (PD_free(strm, "lev1 *" , &tar_r) == TRUE);

    rv &= (PD_free(strm, "integer", ap1) == TRUE);
    rv &= (PD_free(strm, "integer *", &ap2) == TRUE);

    rv &= (PD_free(strm, "double", bp1) == TRUE);
    rv &= (PD_free(strm, "double *", &bp2) == TRUE);

    if (cp1 != NULL)
       rv &= (PD_free(strm, "lev2", cp1) == TRUE);

    if (cp2 != NULL)
       rv &= (PD_free(strm, "lev2 *", &cp2) == TRUE);

    for (i = 0; i < 4; i++)
        rv &= (PD_free(strm, "char *", ca[i].s) == TRUE);

    if (sp1 != NULL)
       rv &= (PD_free(strm, "char *", sp1) == TRUE);

    if (sp2 != NULL)
       rv &= (PD_free(strm, "char **", &sp2) == TRUE);

    rv &= (PD_free(strm, "char", tp1) == TRUE);
    rv &= (PD_free(strm, "char *", &tp2) == TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_1_DATA - write out the data into the PDB file */

static void write_test_1_data(PDBfile *strm)
   {

    if (!PD_write(strm, "tar", "lev1 *",  &tar_w))
       {PRINT(STDOUT, "TAR WRITE FAILED - WRITE_TEST_1_DATA\n");
        exit(1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_1_DATA - read the test data from the file */

static int read_test_1_data(PDBfile *strm)
   {int err;

    err = PD_read(strm, "tar", &tar_r);

    err = PD_read(strm, "tar(0).a", &ap1);
    err = PD_read(strm, "tar(1).a", &ap2);

    err = PD_read(strm, "tar(0).b", &bp1);
    err = PD_read(strm, "tar(1).b", &bp2);

    err = PD_read(strm, "tar(0).c", &cp1);
    err = PD_read(strm, "tar(1).c", &cp2);

    err = PD_read(strm, "tar(0).c(0)", &ca[0]);
    err = PD_read(strm, "tar(0).c(1)", &ca[1]);
    err = PD_read(strm, "tar(1).c(0)", &ca[2]);
    err = PD_read(strm, "tar(1).c(1)", &ca[3]);

    err = PD_read(strm, "tar(0).c(0).s", &sp1);
    err = PD_read(strm, "tar(0).c(1).s", &sp2);

    err = PD_read(strm, "tar(0).c(0).s(0)", &tp1);
    err = PD_read(strm, "tar(0).c(0).s(1)", &tp2);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the PDBLib functions handling indirections
 *        -
 *        - read and write structures with pointers
 *        - set default array base indexes
 *        - read structure members
 *        - read parts of arrays
 *        -
 *        - tests can be targeted
 */

static int test_1(char *base, char *tgt, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm, *vif;
    FILE *fp;

/* target the file is asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_1_data();

    vif = PD_open_vif("test_1");

    PD_defstr(vif, "lev2",
	      "char **s",
	      "integer type",
	      LAST);

    PD_defstr(vif, "lev1",
	      "integer *a",
	      "double *b",
	      "lev2 *c",
	      LAST);

    if (read_only == FALSE)

/* create the named file */
       {strm = PD_create(datfile);
	if (strm == NULL)
	   {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
	    exit(2);};
	PRINT(fp, "File %s created\n", datfile);

/* make a few defstructs */
	PD_defstr(strm, "lev2",
		  "char **s",
		  "integer type",
		  LAST);

	PD_defstr(strm, "lev1",
		  "integer *a",
		  "double *b",
		  "lev2 *c",
		  LAST);

/* write the test data */
	write_test_1_data(strm);

/* close the file */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    exit(2);};
	PRINT(fp, "File %s closed\n", datfile);};

    if (write_only == FALSE)

/* reopen the file */
       {strm = PD_open(datfile, "r");
	if (strm == NULL)
	   {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
	    exit(2);};
	PRINT(fp, "File %s opened\n", datfile);

/* read the structs */
	read_test_1_data(strm);

/* close the file */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    exit(2);};
	PRINT(fp, "File %s closed\n", datfile);};

/* cleanup test data memory */
    err = cleanup_test_1(vif);

    PD_close(vif);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              DRIVER ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes
 *          - return the number of tests that fail
 */

static int run_test(PFTest test, int n, char *host, int dbg)
   {int cs, fail;
    long bytaa, bytfa, bytab, bytfb;
    char msg[MAXLINE];
    double time;

    cs = SC_mem_monitor(-1, dbg, "B", msg);

    SC_mem_stats(&bytab, &bytfb, NULL, NULL);

    time = SC_wall_clock_time();

    fail = 0;

    if ((*test)(host, NULL, n) == 0)
       {PRINT(STDOUT, "Test #%d failed\n", n);
	fail++;};

    SC_mem_stats(&bytaa, &bytfa, NULL, NULL);

    bytaa -= bytab;
    bytfa -= bytfb;
    time   = SC_wall_clock_time() - time;

    cs = SC_mem_monitor(cs, dbg, "B", msg);

    PRINT(STDOUT,
          "\t\t     %3d     %7d   %7d   %7d     %.2g\n",
          n, bytaa, bytfa, bytaa - bytfa, time);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nPDFRTS - test PD_free\n\n");
    PRINT(STDOUT, "Usage: pdfrts [-d #] [-h] [-r] [-v #] [-w] [-1]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d - memory debug level (default is 1)\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       r - do read only test\n");
    PRINT(STDOUT, "       v - use format version # (default is 2)\n");
    PRINT(STDOUT, "       w - do write only test\n");
    PRINT(STDOUT, "       1 - do NOT run test #1\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, err;
    int test_one, mem_dbg;

    PD_init_threads(0, NULL);

    SC_zero_space(0);
    mem_dbg   = 1;
    read_only = FALSE;
    test_one  = TRUE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'd' :
		      mem_dbg = SC_stoi(v[++i]);
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'r' :
		      read_only = TRUE;
		      break;
                 case 'v' :
                      PD_set_format_version(SC_stoi(v[++i]));
		      break;
                 case 'w' :
		      write_only = TRUE;
		      break;
                 case '1' :
		      test_one = FALSE;
		      break;};}
         else
            break;};

    SC_signal(SIGINT, SIG_DFL);

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "\t\t                      Memory                Time\n");
    PRINT(STDOUT, "\t\t                     (bytes)               (secs)\n");
    PRINT(STDOUT, "\t\t     Test  Allocated     Freed      Diff\n");

    err = 0;

    if (test_one)
       err += run_test(test_1, 1, DATFILE, mem_dbg);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
