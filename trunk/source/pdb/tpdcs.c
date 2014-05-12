/*
 * TPDCS.C - checksum test
 *
 * Source Version: 3.0
 * Software Release: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "csm"
#define ERR     0

typedef int (*PFTest)(char *base, char *tgt, int n);

typedef struct s_ts1 ts1;
typedef struct s_statedes statedes;

struct s_ts1
   {float x_min;
    float x_max;
    float y_min;
    float y_max;};
    
struct s_statedes
   {int lm;
    int ln;
    int debug_mode;
    int native_only;
    int ia_w0[5];
    int ia_w1[5];
    int ia_w2[5];
    int ia_r[15];
    long *y;
    float x;
    char *yname;
    char *tsname;
    ts1 *foo;};

static statedes
 st = { 2048, 2000, FALSE, FALSE, };

#define LARGE_MULTIPLE     2048
#define LARGE_NON_MULTIPLE 2000
    
/*--------------------------------------------------------------------------*/

/*                              TEST 1 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_1_DATA - setup data for test #1 */

static void prep_test_1_data(void)
   {int i, nc;

    st.x = 3.33;
     
    for (i = 0; i < st.lm; i++)
        st.y[i] = (long) i;
     
    for (i = 0; i < st.ln; i++)
        {st.foo[i].x_min = (float) i;
	 st.foo[i].x_max = (float) i+1;
	 st.foo[i].y_min = (float) i+2;
	 st.foo[i].y_max = (float) i+3;};
     
    nc    = log10(st.lm);
    nc   += 5;
    st.yname = CMAKE_N(char, nc);
    snprintf(st.yname, nc, "y(%d)", st.lm);
     
    nc     = log10(st.ln);
    nc    += 7;
    st.tsname = CMAKE_N(char, nc);

    snprintf(st.tsname, nc, "foo[%d]", st.ln);
               
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_1_DATA - cleanup data for test #1 */

static void cleanup_test_1_data(void)
   {
               
    CFREE(st.yname);
    CFREE(st.tsname);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_1_DATA - write out the data into the PDB file */

static int write_test_1_data(PDBfile *strm)
   {int rv;

    rv = TRUE;

/* this also writes out an extra hidden checksum variable for x */
    if (!PD_write(strm, "x", "float", &st.x))
       {PRINT(stdout, "%s", PD_get_error());
	rv = FALSE;};
        
    if (!PD_write(strm, st.yname, "long", st.y))
       {PRINT(stdout, "%s", PD_get_error());
	rv = FALSE;};
        
    if (!PD_write(strm, st.tsname, "ts1", st.foo))
       {PRINT(stdout, "%s", PD_get_error());
	rv = FALSE;};
                           
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_1_DATA - read in the data into the PDB file */

static int read_test_1_data(PDBfile *strm)
   {int rv, ok;

    ok = TRUE;

/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "x", &st.x); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	ok = FALSE;};

/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "y", st.y); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	ok = FALSE;};
         
/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "foo", st.foo); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	ok = FALSE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the PDBLib checksum functions
 *        - whole file only
 *        - tests can be targeted
 */

static int test_1(char *base, char *tgt, int n)
   {int err, ok, old;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_1_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = PD_open(datfile, "w"); 
	if (strm == NULL)
	   {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s created\n", datfile);

	PD_defstr(strm, "ts1", 
		  "float x_min",
		  "float x_max",
		  "float y_min",
		  "float y_max",
		  LAST);
               
/* turn on both file and variable (read/write) checksums */      
	old = PD_activate_cksum(strm, PD_MD5_FILE);
           
/* write the test data */
	ok = write_test_1_data(strm);
	if (ok == FALSE)
	   return(ERR);

/* close the file - this also calculates a checksum
 * for the entire file and adds it in
 */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s opened\n", datfile);

    old = PD_activate_cksum(strm, PD_MD5_FILE);
    SC_ASSERT(old == TRUE);

/* read the data from the file */
    err = read_test_1_data(strm);

/* compare the original data with that read in */
/*    err = compare_test_data(strm, fp); */

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
/*    print_test_data(fp); */

/* free known test data memory */
    cleanup_test_1_data();

    if (st.debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 2 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_2_DATA - setup data for test #1 */

static void prep_test_2_data(void)
   {int i, nc;

    st.x = 3.33;
     
    for (i = 0; i < st.lm; i++)
        st.y[i] = (long) i;
     
    for (i = 0; i < st.ln; i++)
        {st.foo[i].x_min = (float) i;
	 st.foo[i].x_max = (float) i+1;
	 st.foo[i].y_min = (float) i+2;
	 st.foo[i].y_max = (float) i+3;};
     
    nc    = log10(st.lm);
    nc   += 5;
    st.yname = CMAKE_N(char, nc);
    snprintf(st.yname, nc, "y(%d)", st.lm);
     
    nc     = log10(st.ln);
    nc    += 7;
    st.tsname = CMAKE_N(char, nc);
    snprintf(st.tsname, nc, "foo[%d]", st.ln);
               
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_2_DATA - cleanup data for test #1 */

static void cleanup_test_2_data(void)
   {
               
    CFREE(st.yname);
    CFREE(st.tsname);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_2_DATA - write out the data into the PDB file */

static int write_test_2_data(PDBfile *strm)
   {int rv;

    rv = TRUE;

/* this also writes out an extra hidden checksum variable for x */
    if (!PD_write(strm, "x", "float", &st.x))
       {PRINT(stdout, "%s", PD_get_error());
	rv = FALSE;};

    if (!PD_write(strm, st.yname, "long", st.y))
       {PRINT(stdout, "%s", PD_get_error());
	rv = FALSE;};

    if (!PD_write(strm, st.tsname, "ts1", st.foo))
       {PRINT(stdout, "%s", PD_get_error());
	rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_2_DATA - read in the data into the PDB file */

static int read_test_2_data(PDBfile *strm)
   {int rv, ok;

    ok = TRUE;

/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "x", &st.x); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	ok = FALSE;};

/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "y", st.y); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	ok = FALSE;};
         
/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "foo", st.foo); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	ok = FALSE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test the PDBLib checksum functions
 *        - tests can be targeted
 */

static int test_2(char *base, char *tgt, int n)
   {int err, ok, old;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_2_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = PD_open(datfile, "w"); 
	if (strm == NULL)
	   {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s created\n", datfile);

	PD_defstr(strm, "ts1", 
		  "float x_min",
		  "float x_max",
		  "float y_min",
		  "float y_max",
		  LAST);
               
/* turn on both file and variable (read/write) checksums */      
	old = PD_activate_cksum(strm, PD_MD5_FILE | PD_MD5_RW);
           
/* write the test data */
	ok = write_test_2_data(strm);
	if (ok == FALSE)
	   return(ERR);

/* read the data from the file */
	err = read_test_2_data(strm);
                           
/* close the file - this also calculates a checksum
 * for the entire file and adds it in
 */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s opened\n", datfile);

    old = PD_activate_cksum(strm, PD_MD5_FILE | PD_MD5_RW);
    SC_ASSERT(old == TRUE);

/* read the data from the file */
    err = read_test_2_data(strm);

/* compare the original data with that read in */
/*    err = compare_test_data(strm, fp); */

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
/*    print_test_data(fp); */

/* free known test data memory */
    cleanup_test_2_data();

    if (st.debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 3 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* WRITE_TEST_3_DATA - write out the data into the PDB file */

static int write_test_3_data(PDBfile *strm)
   {int rv, old;

    rv = TRUE;

/* turn on variable checksums */      
    old = PD_activate_cksum(strm, PD_MD5_RW);
           
/* this also writes out an extra hidden checksum variable for x */
    if (!PD_write(strm, "x", "float", &st.x))
       {PRINT(stdout, "%s", PD_get_error());
	rv = FALSE;};
        
    if (!PD_write(strm, st.yname, "long", st.y))
       {PRINT(stdout, "%s", PD_get_error());
	rv = FALSE;};
        
/* turn off variable checksums */      
    old = PD_activate_cksum(strm, PD_MD5_OFF);
    SC_ASSERT(old == TRUE);
           
    if (!PD_write(strm, st.tsname, "ts1", st.foo))
       {PRINT(stdout, "%s", PD_get_error());
	rv = FALSE;};
                           
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - checksum some variables but not others
 *        - tests can be targeted
 */

static int test_3(char *base, char *tgt, int n)
   {int err, ok, old;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_2_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = PD_open(datfile, "w"); 
	if (strm == NULL)
	   {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s created\n", datfile);

	PD_defstr(strm, "ts1", 
		  "float x_min",
		  "float x_max",
		  "float y_min",
		  "float y_max",
		  LAST);
               
/* write the test data */
	ok = write_test_3_data(strm);
	if (ok == FALSE)
	   return(ERR);

/* read the data from the file */
	err = read_test_2_data(strm);
                           
/* close the file - this also calculates a checksum
 * for the entire file and adds it in
 */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s opened\n", datfile);

    old = PD_activate_cksum(strm, PD_MD5_RW);
    SC_ASSERT(old == TRUE);

/* read the data from the file */
    err = read_test_2_data(strm);

/* compare the original data with that read in */
/*    err = compare_test_data(strm, fp); */

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
/*    print_test_data(fp); */

/* free known test data memory */
    cleanup_test_2_data();

    if (st.debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 4 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_4_DATA - prepare the test data */

static void prep_test_4_data(void)
   {int i;

/* set int array */
    for (i = 0; i < 5; i++)
        {st.ia_w0[i] = i;
	 st.ia_w1[i] = 10*i;
	 st.ia_w2[i] = 100*i;};

    for (i = 0; i < 15; i++)
        st.ia_r[i] = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_4_DATA - free all known test data memory */

static void cleanup_test_4_data(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_4_DATA - read the test data from the file */

static int read_test_4_data(PDBfile *strm)
   {int err;

    err = PD_read(strm, "ia", &st.ia_r);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_4_DATA - print it out to STDOUT */

static void print_test_4_data(FILE *fp)
   {int i;

/* print int array */
    PRINT(fp, "integer array:\n");
    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,1) = %d\n", i, st.ia_r[i]);

    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,2) = %d\n", i, st.ia_r[i+5]);

    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,3) = %d\n", i, st.ia_r[i+10]);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_4_DATA - compare the test data */

static int compare_test_4_data(PDBfile *strm, FILE *fp)
   {int i, err, err_tot;

    err_tot = TRUE;

/* compare ia */
    err = TRUE;
    for (i = 0; i < 5; i++)
        {err &= (st.ia_w0[i] == st.ia_r[i]);};

    for (i = 0; i < 5; i++)
        {err &= (st.ia_w1[i] == st.ia_r[i+5]);};

    for (i = 0; i < 5; i++)
        {err &= (st.ia_w2[i] == st.ia_r[i+10]);};

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");

    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_4_DATA - write out the data into the PDB file
 *                   - this is write, append, append
 */

static int write_test_4_data(PDBfile *strm)
   {int rv;

    rv = TRUE;

/* write a primitive 2D array into the file in three blocks */

    if (!PD_write(strm, "ia(1,5)", "integer", st.ia_w0))
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_4_DATA\n");
        rv = FALSE;};

    if (!PD_append(strm, "ia(1:1,0:4)", st.ia_w1))
       {PRINT(STDOUT, "IA APPEND 1 FAILED - WRITE_TEST_4_DATA\n");
        rv = FALSE;};

    if (!PD_append(strm, "ia(2:2,0:4)", st.ia_w2))
       {PRINT(STDOUT, "IA APPEND 2 FAILED - WRITE_TEST_4_DATA\n");
        rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test checksums and appends
 *        - tests can be targeted
 */

static int test_4(char *base, char *tgt, int n)
   {int err, ok, old;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_4_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = PD_create(datfile);
	if (strm == NULL)
	   {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s created\n", datfile);

/* turn on per variable checksums */      
	old = PD_activate_cksum(strm, PD_MD5_RW);
           
/* write the test data */
	ok = write_test_4_data(strm);
	if (ok == FALSE)
	   return(ERR);

/* close the file */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s opened\n", datfile);

/* turn on per variable checksums */      
    old = PD_activate_cksum(strm, PD_MD5_RW);
    SC_ASSERT(old == TRUE);
           
/* read the data from the file */
    read_test_4_data(strm);

/* compare the original data with that read in */
    err = compare_test_4_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_4_data(fp);

/* free known test data memory */
    cleanup_test_4_data();

    if (st.debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 5 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_5_DATA - free all known test data memory */

static void cleanup_test_5_data(void)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_5_DATA - print it out to STDOUT */

static void print_test_5_data(FILE *fp)
   {int i;

/* print int array */
    PRINT(fp, "integer array:\n");
    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,1) = %d\n", i, st.ia_r[i]);

    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,2) = %d\n", i, st.ia_r[i+5]);

    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,3) = %d\n", i, st.ia_r[i+10]);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_TEST_5_DATA - compare the test data */

static int compare_test_5_data(PDBfile *strm, FILE *fp)
   {int i, err, err_tot;

    err_tot = TRUE;

/* compare ia */
    err = TRUE;
    for (i = 0; i < 3; i++)
        {err &= (st.ia_w0[i] == st.ia_r[i]);};

    for (i = 1; i < 5; i++)
        {err &= (st.ia_w1[i] == st.ia_r[i+5]);};

    for (i = 0; i < 5; i++)
        {err &= (st.ia_w2[i] == st.ia_r[i+10]);};

    if (err)
       PRINT(fp, "Arrays compare\n");
    else
       PRINT(fp, "Arrays differ\n");

    err_tot &= err;

    return(err_tot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_5_DATA - write out the data into the PDB file
 *                   - this is write, append, append
 */

static int write_test_5_data(PDBfile *strm)
   {int rv;

    rv = TRUE;

/* write a primitive 1D array into the file in three blocks */

    if (!PD_write(strm, "ia(5)", "integer", st.ia_w0))
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_5_DATA\n");
        rv = FALSE;};

    if (!PD_append(strm, "ia(5:9)", st.ia_w1))
       {PRINT(STDOUT, "IA APPEND 1 FAILED - WRITE_TEST_5_DATA\n");
        rv = FALSE;};

    if (!PD_append(strm, "ia(10:14)", st.ia_w2))
       {PRINT(STDOUT, "IA APPEND 2 FAILED - WRITE_TEST_5_DATA\n");
        rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHANGE_TEST_5_DATA - write out the data into the PDB file
 *                    - this is write, append, append
 *                    - row major order
 */

static int change_test_5_data(PDBfile *strm)
   {int rv;
    int ia_a[2];

    rv = TRUE;

    ia_a[0] = -1;
    ia_a[1] = -2;

/* change a value in the middle of the first block */
    if (!PD_write(strm, "ia(3)", "integer", ia_a))
       {PRINT(STDOUT, "IA WRITE FAILED - CHANGE_TEST_5_DATA\n");
        rv = FALSE;};

/* change values in the first and second block
 * this tests writes spanning blocks
 */
    if (!PD_write(strm, "ia(4:5)", "integer", ia_a))
       {PRINT(STDOUT, "IA WRITE FAILED - CHANGE_TEST_5_DATA\n");
        rv = FALSE;};

/* NOTE: we leave the third block alone to test handling of valid blocks */

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_5_DATA - read in bits and pieces of data from the PDB file */

static int read_test_5_data(PDBfile *strm)
   {int err, lst;
    int ia_a[2];

    ia_a[0] = -1;
    ia_a[1] = -2;

    err = 0;

    lst = PD_read(strm, "ia(3)", ia_a);
    if (lst == 0)
       err++;

    lst = PD_read(strm, "ia(9:10)", ia_a);
    if (lst == 0)
       err++;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_5 - test checksums and appends
 *        - tests can be targeted
 */

static int test_5(char *base, char *tgt, int n)
   {int err, ok, old;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_4_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = PD_create(datfile);
	if (strm == NULL)
	   {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s created\n", datfile);

/* turn on per variable checksums */      
	old = PD_activate_cksum(strm, PD_MD5_RW);
           
/* write the test data */
	ok = write_test_5_data(strm);
	if (ok == FALSE)
	   return(ERR);

/* close the file */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s opened\n", datfile);

/* turn on per variable checksums */      
    old = PD_activate_cksum(strm, PD_MD5_RW);
    SC_ASSERT(old == TRUE);
           
/* modify the file data */
    if (read_only == FALSE)
       change_test_5_data(strm);

/* read the data from the file */
    read_test_4_data(strm);

/* read part of the data from the file */
    err = read_test_5_data(strm);

/* compare the original data with that read in */
    err = compare_test_5_data(strm, fp);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_5_data(fp);

/* free known test data memory */
    cleanup_test_5_data();

    if (st.debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 6 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* CHANGE_TEST_6_DATA - write out the data into the PDB file
 *                    - this is write, append, append
 *                    - row major order
 */

static void change_test_6_data(char *name)
   {size_t nw;
    int64_t addr;
    syment *ep;
    PDBfile *strm;
    FILE *fp;

    addr = 0;

/* find the address of IA */
    strm = PD_open(name, "r");
    if (strm != NULL)
       {ep   = PD_inquire_entry(strm, "/ia", FALSE, NULL);
	addr = PD_entry_address(ep);

	PD_close(strm);};

/* clobber part of IA */
    fp = io_open(name, "r+");
    io_seek(fp, addr+2, SEEK_SET);

    nw = io_write("    ", 1, 4, fp);
    SC_ASSERT(nw == 4);

    io_close(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_6 - test checksums by corrupting the file
 *        - tests can be targeted
 */

static int test_6(char *base, char *tgt, int n)
   {int ok, fail, old;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_4_data();

    ok = TRUE;

    if (read_only == FALSE)

/* create the named file */
       {strm = PD_create(datfile);
	if (strm == NULL)
	   {PRINT(fp, "Test couldn't create file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s created\n", datfile);

/* turn on per variable checksums */      
	old = PD_activate_cksum(strm, PD_MD5_RW);
           
/* write the test data */
	ok = write_test_5_data(strm);
	if (ok == FALSE)
	   return(ERR);

/* close the file */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    return(ERR);};
	PRINT(fp, "File %s closed\n", datfile);};

    if (read_only == FALSE)
       change_test_6_data(datfile);

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s opened\n", datfile);

/* turn on per variable checksums */      
    old = PD_activate_cksum(strm, PD_MD5_RW);
    SC_ASSERT(old == TRUE);
           
/* read the entire corrupted variable from the file */
    fail = read_test_4_data(strm);
    ok  &= (fail == 0);

/* read part of the corrupted variable from the file */
    fail = read_test_5_data(strm);
    ok  &= (fail == 1);

/* close the file */
    if (!PD_close(strm))
       {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
        return(ERR);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_5_data(fp);

/* free known test data memory */
    cleanup_test_5_data();

    if (st.debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (ok)
       REMOVE(fname);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes
 *          - return the number of tests that fail
 */

static int run_test(PFTest test, int n, char *host)
   {int i, m, rv, fail;
    char *nm;
    tframe ti;

    pre_test(&ti, st.debug_mode);

    fail = 0;

    if (st.native_only == FALSE)
       {m = PD_target_n_platforms();
	for (i = 0; i < m; i++)
	    {rv = PD_target_platform_n(i);
	     SC_ASSERT(rv == TRUE);

	     nm = PD_target_platform_name(i);
	     if ((*test)(host, nm, n) == FALSE)
	        {PRINT(STDOUT, "Test #%d %s failed\n", n, nm);
		 fail++;};};};

/* native test */
    if ((*test)(host, NULL, n) == FALSE)
       {PRINT(STDOUT, "Test #%d native failed\n", n);
	fail++;};

    post_test(&ti, n);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDCS - run PDB defent/append test suite\n\n");
    PRINT(STDOUT, "Usage: tpdcs [-b] [-d] [-db] [-h] [-lm #] [-ln #] [-n] [-r] [-v #] [-vw] [-1] [-2] [-3] [-4] [-5] [-6]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       b    set buffer size (default no buffering)\n");
    PRINT(STDOUT, "       d    turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       db   turn on buffer debugging\n");
    PRINT(STDOUT, "       h    print this help message and exit\n");
    PRINT(STDOUT, "       lm   power of 2 array size (default 2048)\n");
    PRINT(STDOUT, "       ln   non-power of 2 array size (default 2000)\n");
    PRINT(STDOUT, "       n    run native mode test only\n");
    PRINT(STDOUT, "       r    read only\n");
    PRINT(STDOUT, "       v    use specified format version (default 2)\n");
    PRINT(STDOUT, "       vw   verify low level writes\n");
    PRINT(STDOUT, "       1    do NOT run test #1\n");
    PRINT(STDOUT, "       2    do NOT run test #2\n");
    PRINT(STDOUT, "       3    do NOT run test #3\n");
    PRINT(STDOUT, "       4    do NOT run test #4\n");
    PRINT(STDOUT, "       5    do NOT run test #5\n");
    PRINT(STDOUT, "       6    do NOT run test #6\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the various append scenarios */

int main(int c, char **v)
   {int i, n, err;
    int ton[7];
    int use_mapped_files;
    int64_t bfsz;
    extern int SC_bio_debug(int lvl);

    PD_init_threads(0, NULL);

    SC_zero_space_n(1, -2);

    bfsz             = -1;
    bfsz             = SC_OPT_BFSZ;
    read_only        = FALSE;
    use_mapped_files = FALSE;

    st.debug_mode  = FALSE;
    st.native_only = FALSE;

    for (i = 0; i < 7; i++)
        ton[i] = TRUE;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {if (strcmp(v[i], "-db") == 0)
	        SC_bio_debug(3);
             else if (strcmp(v[i], "-lm") == 0)
	        st.lm = SC_stoi(v[++i]);
	     else if (strcmp(v[i], "-ln") == 0)
	        st.ln = SC_stoi(v[++i]);
	     else if (strcmp(v[i], "-vw") == 0)
	        PD_verify_writes(1);
	     else
                {switch (v[i][1])
		    {case 'b' :
		          bfsz = SC_stol(v[++i]);
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
		     case 'n' :
			  st.native_only = TRUE;
			  break;
		     case 'r' :
			  read_only = TRUE;
			  break;
		     case 'v' :
			  PD_set_fmt_version(SC_stoi(v[++i]));
			  break;
		     default :
		          n = -SC_stoi(v[i]);
			  ton[n] = FALSE;
			  break;};};}
         else
            break;};

    st.y   = CMAKE_N(long, st.lm);
    st.foo = CMAKE_N(ts1, st.ln);

    PD_set_io_hooks(use_mapped_files);

    PD_set_buffer_size(bfsz);

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
    if (ton[5])
       err += run_test(test_5, 5, DATFILE);
    if (ton[6])
       err += run_test(test_6, 6, DATFILE);

    PRINT(STDOUT, "\n");

    CFREE(st.y);
    CFREE(st.foo);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
