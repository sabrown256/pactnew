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

typedef int (*PFTest)(char *base, char *tgt, int n);

typedef struct 
   {float x_min;
    float x_max;
    float y_min;
    float y_max;} teststruct;
    
#define LARGE_MULTIPLE     2048
#define LARGE_NON_MULTIPLE 2000
#define STR_MULTIPLE       "2048"
#define STR_NON_MULTIPLE   "2000"
    
static long
 y[LARGE_MULTIPLE];

static float
 x;

static char
 *yname,
 *tsname;

static teststruct
 foo[LARGE_NON_MULTIPLE];

static int
 debug_mode = FALSE,
 native_only = FALSE,
 read_only  = FALSE;

int
 ia_w0[5],
 ia_w1[5],
 ia_w2[5],
 ia_r[15];

/*--------------------------------------------------------------------------*/

/*                              TEST 1 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PREP_TEST_1_DATA - setup data for test #1 */

static void prep_test_1_data(void)
   {int i, nc;

    x = 3.33;
     
    for (i = 0; i < LARGE_MULTIPLE; i++)
        y[i] = (long) i;
     
    for (i = 0; i < LARGE_NON_MULTIPLE; i++)
        {foo[i].x_min = (float) i;
	 foo[i].x_max = (float) i+1;
	 foo[i].y_min = (float) i+2;
	 foo[i].y_max = (float) i+3;};
     
    nc    = 3 + strlen(STR_MULTIPLE) + 1;
    yname = CMAKE_N(char, nc);
    snprintf(yname, LARGE_MULTIPLE, "y(%s)", STR_MULTIPLE);
     
    nc     = 5 + strlen(STR_NON_MULTIPLE) + 1;
    tsname = CMAKE_N(char, nc);

    snprintf(tsname, nc, "foo[%s]", STR_NON_MULTIPLE);
               
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_1_DATA - cleanup data for test #1 */

static void cleanup_test_1_data(void)
   {
               
    CFREE(yname);
    CFREE(tsname);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_1_DATA - write out the data into the PDB file */

static void write_test_1_data(PDBfile *strm)
   {

/* this also writes out an extra hidden checksum variable for x */
    if (!PD_write(strm, "x", "float", &x))
       PRINT(stdout, "%s", PD_get_error());
        
    if (!PD_write(strm, yname, "long", y))
       PRINT(stdout, "%s", PD_get_error());
        
    if (!PD_write(strm, tsname, "teststruct", foo))
       PRINT(stdout, "%s", PD_get_error());
                           
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_1_DATA - read in the data into the PDB file */

static int read_test_1_data(PDBfile *strm)
   {int rv, st;

    st = TRUE;

/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "x", &x); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	st = FALSE;};

/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "y", y); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	st = FALSE;};
         
/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "foo", foo); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	st = FALSE;};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test the PDBLib checksum functions
 *        - whole file only
 *        - tests can be targeted
 */

static int test_1(char *base, char *tgt, int n)
   {int err, old;
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
	    exit(1);};
	PRINT(fp, "File %s created\n", datfile);

	PD_defstr(strm, "teststruct", 
		  "float x_min",
		  "float x_max",
		  "float y_min",
		  "float y_max",
		  LAST);
               
/* turn on both file and variable (read/write) checksums */      
	old = PD_activate_cksum(strm, PD_MD5_FILE);
           
/* write the test data */
	write_test_1_data(strm);

/* close the file - this also calculates a checksum
 * for the entire file and adds it in
 */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    exit(1);};
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
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
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
/*    print_test_data(fp); */

/* free known test data memory */
    cleanup_test_1_data();

    if (debug_mode)
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

    x = 3.33;
     
    for (i = 0; i < LARGE_MULTIPLE; i++)
        y[i] = (long) i;
     
    for (i = 0; i < LARGE_NON_MULTIPLE; i++)
        {foo[i].x_min = (float) i;
	 foo[i].x_max = (float) i+1;
	 foo[i].y_min = (float) i+2;
	 foo[i].y_max = (float) i+3;};
     
    nc    = 3 + strlen(STR_MULTIPLE) + 1;
    yname = CMAKE_N(char, nc);
    snprintf(yname, LARGE_MULTIPLE, "y(%s)", STR_MULTIPLE);
     
    nc     = 5 + strlen(STR_NON_MULTIPLE) + 1;
    tsname = CMAKE_N(char, nc);
    snprintf(tsname, nc, "foo[%s]", STR_NON_MULTIPLE);
               
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEANUP_TEST_2_DATA - cleanup data for test #1 */

static void cleanup_test_2_data(void)
   {
               
    CFREE(yname);
    CFREE(tsname);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRITE_TEST_2_DATA - write out the data into the PDB file */

static void write_test_2_data(PDBfile *strm)
   {

/* this also writes out an extra hidden checksum variable for x */
    if (!PD_write(strm, "x", "float", &x))
       PRINT(stdout, "%s", PD_get_error());

    if (!PD_write(strm, yname, "long", y))
       PRINT(stdout, "%s", PD_get_error());

    if (!PD_write(strm, tsname, "teststruct", foo))
       PRINT(stdout, "%s", PD_get_error());

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_2_DATA - read in the data into the PDB file */

static int read_test_2_data(PDBfile *strm)
   {int rv, st;

    st = TRUE;

/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "x", &x); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	st = FALSE;};

/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "y", y); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	st = FALSE;};
         
/* determine if read failed or checksum failed: check error */     
    rv = PD_read(strm, "foo", foo); 
    if ((rv == FALSE) || (rv == -1))
       {PRINT(stdout, "%s", PD_get_error());  
	st = FALSE;};

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test the PDBLib checksum functions
 *        - tests can be targeted
 */

static int test_2(char *base, char *tgt, int n)
   {int err, old;
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
	    exit(1);};
	PRINT(fp, "File %s created\n", datfile);

	PD_defstr(strm, "teststruct", 
		  "float x_min",
		  "float x_max",
		  "float y_min",
		  "float y_max",
		  LAST);
               
/* turn on both file and variable (read/write) checksums */      
	old = PD_activate_cksum(strm, PD_MD5_FILE | PD_MD5_RW);
           
/* write the test data */
	write_test_2_data(strm);

/* read the data from the file */
	err = read_test_2_data(strm);
                           
/* close the file - this also calculates a checksum
 * for the entire file and adds it in
 */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    exit(1);};
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
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
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
/*    print_test_data(fp); */

/* free known test data memory */
    cleanup_test_2_data();

    if (debug_mode)
       SC_mem_map(STDOUT, FALSE);

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                              TEST 3 ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* WRITE_TEST_3_DATA - write out the data into the PDB file */

static void write_test_3_data(PDBfile *strm)
   {int old;

/* turn on variable checksums */      
    old = PD_activate_cksum(strm, PD_MD5_RW);
           
/* this also writes out an extra hidden checksum variable for x */
    if (!PD_write(strm, "x", "float", &x))
       PRINT(stdout, "%s", PD_get_error());
        
    if (!PD_write(strm, yname, "long", y))
       PRINT(stdout, "%s", PD_get_error());
        
/* turn off variable checksums */      
    old = PD_activate_cksum(strm, PD_MD5_OFF);
    SC_ASSERT(old == TRUE);
           
    if (!PD_write(strm, tsname, "teststruct", foo))
       PRINT(stdout, "%s", PD_get_error());
                           
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - checksum some variables but not others
 *        - tests can be targeted
 */

static int test_3(char *base, char *tgt, int n)
   {int err, old;
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
	    exit(1);};
	PRINT(fp, "File %s created\n", datfile);

	PD_defstr(strm, "teststruct", 
		  "float x_min",
		  "float x_max",
		  "float y_min",
		  "float y_max",
		  LAST);
               
/* write the test data */
	write_test_3_data(strm);

/* read the data from the file */
	err = read_test_2_data(strm);
                           
/* close the file - this also calculates a checksum
 * for the entire file and adds it in
 */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    exit(1);};
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
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
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
/*    print_test_data(fp); */

/* free known test data memory */
    cleanup_test_2_data();

    if (debug_mode)
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
        {ia_w0[i] = i;
	 ia_w1[i] = 10*i;
	 ia_w2[i] = 100*i;};

    for (i = 0; i < 15; i++)
        ia_r[i] = -1;

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

    err = PD_read(strm, "ia", &ia_r);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_TEST_4_DATA - print it out to STDOUT */

static void print_test_4_data(FILE *fp)
   {int i;

/* print int array */
    PRINT(fp, "integer array:\n");
    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,1) = %d\n", i, ia_r[i]);

    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,2) = %d\n", i, ia_r[i+5]);

    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,3) = %d\n", i, ia_r[i+10]);

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
        {err &= (ia_w0[i] == ia_r[i]);};

    for (i = 0; i < 5; i++)
        {err &= (ia_w1[i] == ia_r[i+5]);};

    for (i = 0; i < 5; i++)
        {err &= (ia_w2[i] == ia_r[i+10]);};

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

static void write_test_4_data(PDBfile *strm)
   {

/* write a primitive 2D array into the file in three blocks */

    if (!PD_write(strm, "ia(1,5)", "integer", ia_w0))
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_4_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(1:1,0:4)", ia_w1))
       {PRINT(STDOUT, "IA APPEND 1 FAILED - WRITE_TEST_4_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(2:2,0:4)", ia_w2))
       {PRINT(STDOUT, "IA APPEND 2 FAILED - WRITE_TEST_4_DATA\n");
        exit(1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test checksums and appends
 *        - tests can be targeted
 */

static int test_4(char *base, char *tgt, int n)
   {int err, old;
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
	    exit(1);};
	PRINT(fp, "File %s created\n", datfile);

/* turn on per variable checksums */      
	old = PD_activate_cksum(strm, PD_MD5_RW);
           
/* write the test data */
	write_test_4_data(strm);

/* close the file */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    exit(1);};
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "r");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
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
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_4_data(fp);

/* free known test data memory */
    cleanup_test_4_data();

    if (debug_mode)
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
        PRINT(fp, "  ia(%d,1) = %d\n", i, ia_r[i]);

    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,2) = %d\n", i, ia_r[i+5]);

    for (i = 0; i < 5; i++)
        PRINT(fp, "  ia(%d,3) = %d\n", i, ia_r[i+10]);

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
        {err &= (ia_w0[i] == ia_r[i]);};

    for (i = 1; i < 5; i++)
        {err &= (ia_w1[i] == ia_r[i+5]);};

    for (i = 0; i < 5; i++)
        {err &= (ia_w2[i] == ia_r[i+10]);};

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

static void write_test_5_data(PDBfile *strm)
   {

/* write a primitive 1D array into the file in three blocks */

    if (!PD_write(strm, "ia(5)", "integer", ia_w0))
       {PRINT(STDOUT, "IA WRITE FAILED - WRITE_TEST_5_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(5:9)", ia_w1))
       {PRINT(STDOUT, "IA APPEND 1 FAILED - WRITE_TEST_5_DATA\n");
        exit(1);};

    if (!PD_append(strm, "ia(10:14)", ia_w2))
       {PRINT(STDOUT, "IA APPEND 2 FAILED - WRITE_TEST_5_DATA\n");
        exit(1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHANGE_TEST_5_DATA - write out the data into the PDB file
 *                    - this is write, append, append
 *                    - row major order
 */

static void change_test_5_data(PDBfile *strm)
   {int ia_a[2];

    ia_a[0] = -1;
    ia_a[1] = -2;

/* change a value in the middle of the first block */
    if (!PD_write(strm, "ia(3)", "integer", ia_a))
       {PRINT(STDOUT, "IA WRITE FAILED - CHANGE_TEST_5_DATA\n");
        exit(1);};

/* change values in the first and second block
 * this tests writes spanning blocks
 */
    if (!PD_write(strm, "ia(4:5)", "integer", ia_a))
       {PRINT(STDOUT, "IA WRITE FAILED - CHANGE_TEST_5_DATA\n");
        exit(1);};

/* NOTE: we leave the third block alone to test handling of valid blocks */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* READ_TEST_5_DATA - read in bits and pieces of data from the PDB file */

static int read_test_5_data(PDBfile *strm)
   {int err, st;
    int ia_a[2];

    ia_a[0] = -1;
    ia_a[1] = -2;

    err = 0;

    st = PD_read(strm, "ia(3)", ia_a);
    if (st == 0)
       err++;

    st = PD_read(strm, "ia(9:10)", ia_a);
    if (st == 0)
       err++;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_5 - test checksums and appends
 *        - tests can be targeted
 */

static int test_5(char *base, char *tgt, int n)
   {int err, old;
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
	    exit(1);};
	PRINT(fp, "File %s created\n", datfile);

/* turn on per variable checksums */      
	old = PD_activate_cksum(strm, PD_MD5_RW);
           
/* write the test data */
	write_test_5_data(strm);

/* close the file */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    exit(1);};
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
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
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_5_data(fp);

/* free known test data memory */
    cleanup_test_5_data();

    if (debug_mode)
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
	    exit(1);};
	PRINT(fp, "File %s created\n", datfile);

/* turn on per variable checksums */      
	old = PD_activate_cksum(strm, PD_MD5_RW);
           
/* write the test data */
	write_test_5_data(strm);

/* close the file */
	if (!PD_close(strm))
	   {PRINT(fp, "Test couldn't close file %s\r\n", datfile);
	    exit(1);};
	PRINT(fp, "File %s closed\n", datfile);};

    if (read_only == FALSE)
       change_test_6_data(datfile);

/* reopen the file */
    strm = PD_open(datfile, "a");
    if (strm == NULL)
       {PRINT(fp, "Test couldn't open file %s\r\n", datfile);
        exit(1);};
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
        exit(1);};
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_5_data(fp);

/* free known test data memory */
    cleanup_test_5_data();

    if (debug_mode)
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
	        {PRINT(STDOUT, "Test #%d %s failed\n", n, nm);
		 fail++;};};};

/* native test */
    if ((*test)(host, NULL, n) == FALSE)
       {PRINT(STDOUT, "Test #%d native failed\n", n);
	fail++;};

    post_test(&st, n);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDCS - run PDB defent/append test suite\n\n");
    PRINT(STDOUT, "Usage: tpdcs [-d] [-h] [-n] [-r] [-v #] [-1] [-2] [-3] [-4] [-5] [-6]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h - print this help message and exit\n");
    PRINT(STDOUT, "       n - run native mode test only\n");
    PRINT(STDOUT, "       r - read only\n");
    PRINT(STDOUT, "       v - use specified format version (default 2)\n");
    PRINT(STDOUT, "       1 - do NOT run test #1\n");
    PRINT(STDOUT, "       2 - do NOT run test #2\n");
    PRINT(STDOUT, "       3 - do NOT run test #3\n");
    PRINT(STDOUT, "       4 - do NOT run test #4\n");
    PRINT(STDOUT, "       5 - do NOT run test #5\n");
    PRINT(STDOUT, "       6 - do NOT run test #6\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the various append scenarios */

int main(int c, char **v)
   {int i, err;
    int test_one, test_two, test_three, test_four, test_five, test_six;
    int use_mapped_files;

    PD_init_threads(0, NULL);

    SC_zero_space_n(1, -2);
    debug_mode       = FALSE;
    native_only      = FALSE;
    read_only        = FALSE;
    use_mapped_files = FALSE;
    test_one         = TRUE;
    test_two         = TRUE;
    test_three       = TRUE;
    test_four        = TRUE;
    test_five        = TRUE;
    test_six         = TRUE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'd' :
		      debug_mode  = TRUE;
		      SC_gs.mm_debug = TRUE;
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'm' :
		      use_mapped_files = TRUE;
		      break;
                 case 'n' :
		      native_only = TRUE;
		      break;
                 case 'r' :
		      read_only = TRUE;
		      break;
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;
                 case '1' :
		      test_one = FALSE;
		      break;
                 case '2' :
		      test_two = FALSE;
		      break;
                 case '3' :
		      test_three = FALSE;
		      break;
                 case '4' :
		      test_four = FALSE;
		      break;
                 case '5' :
		      test_five = FALSE;
		      break;
                 case '6' :
		      test_six = FALSE;
		      break;};}
         else
            break;};

    PD_set_io_hooks(use_mapped_files);

    SC_signal(SIGINT, SIG_DFL);

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "\t\t                      Memory                Time\n");
    PRINT(STDOUT, "\t\t                     (bytes)               (secs)\n");
    PRINT(STDOUT, "\t\t     Test  Allocated     Freed      Diff\n");

    err = 0;

    if (test_one)
       err += run_test(test_1, 1, DATFILE);
    if (test_two)
       err += run_test(test_2, 2, DATFILE);
    if (test_three)
       err += run_test(test_3, 3, DATFILE);
    if (test_four)
       err += run_test(test_4, 4, DATFILE);
    if (test_five)
       err += run_test(test_5, 5, DATFILE);
    if (test_six)
       err += run_test(test_6, 6, DATFILE);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
