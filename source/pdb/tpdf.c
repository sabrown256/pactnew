/*
 * TPDF.C - test PDB filter chains
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdb_int.h"
#include "pdbtfr.h"

#define DATFILE "nat"

#define CIRCULAR_SHIFT_RIGHT(_b, _n, _s)                                    \
  ((_b) >> ((_n) % (_s)) | ((_b) << (((_s) - (_n)) % (_s))))

#define CIRCULAR_SHIFT_LEFT(_b, _n, _s)                                     \
  ((_b) << ((_n) % (_s)) | ((_b) >> (((_s) - (_n)) % (_s))))

typedef int (*PFTest)(char *base, char *tgt, fltdes *fl, int n);

static int
 filter_file  = TRUE,
 partial_read = TRUE,
 debug_mode   = FALSE,
 native_only  = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCRAMBLE_FILE_IN - whole file unscrambling on input */

int scramble_file_in(PDBfile *file, fltdes *fl, fltdat *inf)
   {int j, s, rv;
    long bpi;
    int64_t i, ni, nir, nic, nbi, nbo;
    unsigned char b[2];
    unsigned char *bfi, *bfo;

    rv = TRUE;

    ni  = inf->ni[0];
    bpi = inf->bpi[0];
    bfi = inf->bf[0];

    nbi = ni*bpi;
    nir = nbi % 5;
    nic = nbi / 5;

    bfo = inf->bf[1];
    if (bfo == NULL)
       {nbo = 4*nic + nir;
	bfo = CMAKE_N(unsigned char, nbo);

	inf->ni[1]  = nbo;
	inf->bpi[1] = 1;
	inf->nb[1]  = nbo;
	inf->bf[1]  = bfo;};

    for (i = 0; i < nic; i++)
        {for (j = 0; j < 4; j++)
	     {s = (i + j) % 6 + 1;
	      b[0] = *bfi++;
	      b[1] = CIRCULAR_SHIFT_RIGHT(b[0], s, 8);
	      *bfo++ = b[1];};

	 bfi++;};

    for (i = 0; i < nir; i++)
        *bfo++ = *bfi++;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCRAMBLE_FILE_OUT - whole file scrambling on output */

int scramble_file_out(PDBfile *file, fltdes *fl, fltdat *inf)
   {int j, s, rv;
    long bpi;
    int64_t i, ni, nir, nic, nbi, nbo;
    unsigned char b[2];
    unsigned char *bfi, *bfo;

    rv = TRUE;

    ni  = inf->ni[0];
    bpi = inf->bpi[0];
    bfi = inf->bf[0];

    nbi = ni*bpi;
    nir = nbi % 4;
    nic = nbi / 4;

    nbo = 5*nic + nir;
    bfo = CMAKE_N(unsigned char, nbo);

    inf->ni[1]  = nbo;
    inf->bpi[1] = 1;
    inf->nb[1]  = nbo;
    inf->bf[1]  = bfo;

    for (i = 0; i < nic; i++)
        {for (j = 0; j < 4; j++)
	     {s = (i + j) % 6 + 1;
	      b[0] = *bfi++;
	      b[1] = CIRCULAR_SHIFT_LEFT(b[0], s, 8);
	      *bfo++ = b[1];};

	 *bfo++ = i % 256;};

    for (i = 0; i < nir; i++)
        *bfo++ = *bfi++;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHRINK_FILE_IN - whole file unshrinking on input */

int shrink_file_in(PDBfile *file, fltdes *fl, fltdat *inf)
   {int j, rv;
    long bpi;
    int64_t i, ni, nir, nic, nbi, nbo;
    unsigned char *bfi, *bfo;

    rv = TRUE;

    ni  = inf->ni[0];
    bpi = inf->bpi[0];
    bfi = inf->bf[0];

    nbi = ni*bpi;
    nir = nbi % 4;
    nic = nbi / 4;

    bfo = inf->bf[1];
    if (bfo == NULL)
       {nbo = 5*nic + nir;
	bfo = CMAKE_N(unsigned char, nbo);

	inf->ni[1]  = nbo;
	inf->bpi[1] = 1;
	inf->nb[1]  = nbo;
	inf->bf[1]  = bfo;};

    for (i = 0; i < nic; i++)
        {for (j = 0; j < 4; j++)
	     *bfo++ = *bfi++;

	 *bfo++ = i % 256;};

    for (i = 0; i < nir; i++)
        *bfo++ = *bfi++;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHRINK_FILE_OUT - whole file shrinking on output */

int shrink_file_out(PDBfile *file, fltdes *fl, fltdat *inf)
   {int j, rv;
    long bpi;
    int64_t i, ni, nir, nic, nbi, nbo;
    unsigned char *bfi, *bfo;

    rv = TRUE;

    ni  = inf->ni[0];
    bpi = inf->bpi[0];
    bfi = inf->bf[0];

    nbi = ni*bpi;
    nir = nbi % 5;
    nic = nbi / 5;

    nbo = 4*nic + nir;
    bfo = CMAKE_N(unsigned char, nbo);

    inf->ni[1]  = nbo;
    inf->bpi[1] = 1;
    inf->nb[1]  = nbo;
    inf->bf[1]  = bfo;

    for (i = 0; i < nic; i++)
        {for (j = 0; j < 4; j++)
	     *bfo++ = *bfi++;

	 bfi++;};

    for (i = 0; i < nir; i++)
        *bfo++ = *bfi++;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_CREATE_FILTERS - setup the filters for file NAME */

PDBfile *setup_create_filters(char *name, fltdes *fl)
   {PDBfile *file;

    if (filter_file == TRUE)
       {PD_filt_register_file(fl);
	file = PD_create(name);}
    else
       {file = PD_create(name);
	PD_filt_register_block(file, fl);};

    return(file);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_OPEN_FILTERS - setup the filters for file NAME */

PDBfile *setup_open_filters(char *name, char *mode, fltdes *fl)
   {PDBfile *file;

    if (filter_file == TRUE)
       {PD_filt_register_file(fl);
	file = PD_open(name, mode);}
    else
       {file = PD_open(name, mode);
	PD_filt_register_block(file, fl);};

    return(file);}

/*--------------------------------------------------------------------------*/

/*                            TEST #1 ROUTINES                              */

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

#include "tpdcore1.c"

int test_1(char *base, char *tgt, fltdes *fl, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_1_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = setup_create_filters(datfile, fl);
	if (strm == NULL)
	   error(1, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* make a few defstructs */
	PD_defstr(strm, "fr1",
		  " float x_min",
		  "float x_max",
		  "float y_min",
		  "float y_max",
		  LAST);
	PD_defstr(strm, "plot1",
		  "float x_axis(10)",
		  "float y_axis(10)",
		  "integer npts", 
		  "char * label",
		  "fr1 view",
		  LAST);

/* write the test data */
	write_test_1_data(strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(1, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);

/* reopen the file to append */
	strm = setup_open_filters(datfile, "a", fl);
	if (strm == NULL)
	   error(1, fp, "Test couldn't open file %s to append\r\n", datfile);
	PRINT(fp, "File %s opened to append\n", datfile);

	append_test_1_data(strm);

/* close the file after append */
	if (PD_close(strm) == FALSE)
	   error(1, fp, "Test couldn't close file %s after append\r\n",
		 datfile);
	PRINT(fp, "File %s closed after append\n", datfile);};

/* reopen the file */
    strm = setup_open_filters(datfile, "r", fl);
    if (strm == NULL)
       error(1, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* dump the symbol table */
    dump_test_symbol_table(fp, strm->symtab, 1);

/* read the data from the file */
    read_test_1_data(strm);

/* compare the original data with that read in */
    err = compare_test_1_data(strm, fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(1, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print it out to STDOUT */
    print_test_1_data(fp);

/* free known test data memory */
    cleanup_test_1();

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #2 ROUTINES                              */

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

#include "tpdcore2.c"

int test_2(char *base, char *tgt, fltdes *fl, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file is asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_2_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = setup_create_filters(datfile, fl);
	if (strm == NULL)
	   error(2, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* set the default offset */
	strm->default_offset = do_w;

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
	write_test_2_data(strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(2, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = setup_open_filters(datfile, "r", fl);
    if (strm == NULL)
       error(2, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* dump the symbol table */
    dump_test_symbol_table(fp, strm->symtab, 2);

/* read the structs */
    read_test_2_data(strm);

/* compare the original data with that read in */
    err = compare_test_2_data(strm, fp);

/* print out the results */
    print_test_2_data(fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(2, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* cleanup test data memory */
    cleanup_test_2();

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #3 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_3 - test the PDBLib functions handling indirections
 *        - read and write structures with alignment difficulties
 *        - tests can be targeted
 */

#include "tpdcore3.c"

int test_3(char *base, char *tgt, fltdes *fl, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file is asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_3_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = setup_create_filters(datfile, fl);
	if (strm == NULL)
	   error(2, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* make a few defstructs */
	PD_defstr(strm, "st3",
		  "char a",
		  "short b",
		  "char c[2]",
		  "integer d",
		  "char e[3]",
		  "float f",
		  "char g[4]",
		  "double h",
		  "char i[5]",
		  "char *j",
		  "char k[6]",
		  LAST);

/* write the test data */
	write_test_3_data(strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(2, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);

#ifdef LINUX
/* GOTCHA: Without this delay, the test fails about half the time,
 *         iff the files are being accessed across the network.
 */
	SC_sleep(10);
#endif
	};

/* reopen the file */
    strm = setup_open_filters(datfile, "r", fl);
    if (strm == NULL)
       error(2, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* dump the symbol table */
    dump_test_symbol_table(fp, strm->symtab, 3);

/* read the structs */
    read_test_3_data(strm);

/* compare the original data with that read in */
    err = compare_test_3_data(strm, fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(2, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* print out the results */
    print_test_3_data(fp);

/* clean up test data memory */
    cleanup_test_3();

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #4 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_4 - test the PDBLib functions handling indirections
 *        - read and write structures with alignment difficulties
 *        - tests can be targeted
 */

#include "tpdcore4.c"

int test_4(char *base, char *tgt, fltdes *fl, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_4_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = setup_create_filters(datfile, fl);
	if (strm == NULL)
	   error(2, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

	strm->previous_file = CSTRSAVE(base);

/* make a few defstructs */
	PD_def_hash_types(strm, 3);

	PD_defstr(strm, "st4",
		  "short a",
		  "char b",
		  LAST);

/* write the test data */
	write_test_4_data(strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(2, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = setup_open_filters(datfile, "r", fl);
    if (strm == NULL)
       error(2, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* dump the symbol table */
    dump_test_symbol_table(fp, strm->symtab, 4);

/* read the structs */
    read_test_4_data(strm);

/* compare the original data with that read in */
    err = compare_test_4_data(strm, fp);

/* print out the results */
    print_test_4_data(fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(2, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* clean up test data memory */
    cleanup_test_4();

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #5 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_5 - test the PDBLib functions handling indirections and appends
 *        - read and write structures with alignment difficulties
 *        - tests can be targeted
 */

#include "tpdcore5.c"

int test_5(char *base, char *tgt, fltdes *fl, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_5_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = setup_create_filters(datfile, fl);
	if (strm == NULL)
	   error(2, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

	strm->previous_file = CSTRSAVE(base);

/* make a few defstructs */
	PD_defstr(strm, "lev15",
		  "integer *a",
		  "double *b",
		  "char *c",
		  LAST);

/* write the test data */
	write_test_5_data(strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(2, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = setup_open_filters(datfile, "r", fl);
    if (strm == NULL)
       error(2, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the structs */
    read_test_5_data(strm);

/* compare the original data with that read in */
    err = compare_test_5_data(strm, fp);

/* print out the results */
    print_test_5_data(fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(2, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* clean up test data memory */
    cleanup_test_5();

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #6 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_6 - test the PDBLib function PD_read_as_dwim */

#include "tpdcore6.c"

int test_6(char *base, char *tgt, fltdes *fl, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_6_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = setup_create_filters(datfile, fl);
	if (strm == NULL)
	   error(2, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* make a few defstructs */
	PD_defstr(strm, "st61",
		  "int n",
		  "double a[10]",
		  LAST);

	PD_defstr(strm, "st62",
		  "int n",
		  "double *a",
		  LAST);

/* write the test data */
	write_test_6_data(strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(2, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = setup_open_filters(datfile, "r", fl);
    if (strm == NULL)
       error(2, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the structs */
    read_test_6_data(strm);

/* compare the original data with that read in */
    err = compare_test_6_data(strm, fp);

/* print out the results */
    print_test_6_data(fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(2, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* clean up test data memory */
    cleanup_test_6();

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #7 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_7 - test the PDBLib pointer tracking */

#include "tpdcore7.c"

int test_7(char *base, char *tgt, fltdes *fl, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_7_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = setup_create_filters(datfile, fl);
	if (strm == NULL)
	   error(2, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* make a few defstructs */
	PD_defstr(strm, "st7",
		  "int n",
		  "double *a",
		  LAST);

/* turn off pointer tracking */
	PD_set_track_pointers(strm, FALSE);

/* write the test data */
	write_test_7_data(strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(2, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = setup_open_filters(datfile, "r", fl);
    if (strm == NULL)
       error(2, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the structs */
    read_test_7_data(strm);

/* compare the original data with that read in */
    err = compare_test_7_data(strm, fp);

/* print out the results */
    print_test_7_data(fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(2, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* clean up test data memory */
    cleanup_test_7();

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #8 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_8 - test the PDBLib defent and append logic */

#include "tpdcore8.c"

int test_8(char *base, char *tgt, fltdes *fl, int n)
   {int err;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *strm;
    FILE *fp;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    fp = io_open(fname, "w");

    prep_test_8_data();

    if (read_only == FALSE)

/* create the named file */
       {strm = setup_create_filters(datfile, fl);
	if (strm == NULL)
	   error(2, fp, "Test couldn't create file %s\r\n", datfile);
	PRINT(fp, "File %s created\n", datfile);

/* write the test data */
	write_test_8_data(strm);

/* append the test data */
	append_test_8_data(strm);

/* close the file */
	if (PD_close(strm) == FALSE)
	   error(2, fp, "Test couldn't close file %s\r\n", datfile);
	PRINT(fp, "File %s closed\n", datfile);};

/* reopen the file */
    strm = setup_open_filters(datfile, "r", fl);
    if (strm == NULL)
       error(2, fp, "Test couldn't open file %s\r\n", datfile);
    PRINT(fp, "File %s opened\n", datfile);

/* read the structs */
    read_test_8_data(strm);

/* compare the original data with that read in */
    err = compare_test_8_data(strm, fp);

/* print out the results */
    print_test_8_data(fp);

/* close the file */
    if (PD_close(strm) == FALSE)
       error(2, fp, "Test couldn't close file %s\r\n", datfile);
    PRINT(fp, "File %s closed\n", datfile);

/* clean up test data memory */
    cleanup_test_8();

    io_close(fp);
    if (err)
       REMOVE(fname);

    return(err);}

/*--------------------------------------------------------------------------*/

/*                            TEST #9 ROUTINES                              */

/*--------------------------------------------------------------------------*/

#include "tpdcore9.c"

/*--------------------------------------------------------------------------*/

/*                            TEST #10 ROUTINES                             */

/*--------------------------------------------------------------------------*/

#include "tpdcore10.c"

/*--------------------------------------------------------------------------*/

/*                              DRIVER ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes
 *          - return the number of tests that fail
 */

static int run_test(PFTest test, int n, char *host, fltdes *fl, int native)
   {int i, m, rv, fail;
    char *nm;
    tframe st;

    pre_test(&st, debug_mode);

    fail = 0;

    if (native == FALSE)
       {m = PD_target_n_platforms();
	for (i = 0; i < m; i++)
	    {rv = PD_target_platform_n(i);
	     SC_ASSERT(rv == TRUE);

	     nm = PD_target_platform_name(i);
	     if ((*test)(host, nm, fl, n) == FALSE)
	        {PRINT(STDOUT, "Test #%d %s failed\n", n, nm);
		 fail++;};};};

/* native test */
    if ((*test)(host, NULL, fl, n) == FALSE)
       {PRINT(STDOUT, "Test #%d native failed\n", n);
	fail++;};

    post_test(&st, n);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORK - do the test work */

static int work(char *lbl, int *ton, fltdes *fl, int ff, int no)
   {int err;

    filter_file  = ff;
    partial_read = (filter_file == TRUE);

    PRINT(STDOUT, "\t\t%s\n", lbl);

    err = 0;

    if (ton[1])
       err += run_test(test_1, 1, DATFILE, fl, no);
    if (ton[2])
       err += run_test(test_2, 2, DATFILE, fl, no);
    if (ton[3])
       err += run_test(test_3, 3, DATFILE, fl, no);
    if (ton[4])
       err += run_test(test_4, 4, DATFILE, fl, no);
    if (ton[5])
       err += run_test(test_5, 5, DATFILE, fl, no);
#if 0
    if (ton[6])
       err += run_test(test_6, 6, DATFILE, fl, no);
    if (ton[7])
       err += run_test(test_7, 7, DATFILE, fl, no);
    if (ton[8])
       err += run_test(test_8, 8, DATFILE, fl, no);
#endif

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nTPDF - run basic PDB test suite\n\n");
    PRINT(STDOUT, "Usage: tpdf [-b #] [-c] [-c0] [-c1] [-c2] [-d] [-f] [-h] [-n]\n");
    PRINT(STDOUT, "            [-r] [-v #] [-1] [-2] [-3] [-4] [-5] [-6] [-7] [-8]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       b  - set buffer size (default no buffering)\n");
    PRINT(STDOUT, "       c  - verify low level writes\n");
    PRINT(STDOUT, "       c0 - no filter chain\n");
    PRINT(STDOUT, "       c1 - use single filter chain\n");
    PRINT(STDOUT, "       c2 - use double filter chain\n");
    PRINT(STDOUT, "       d  - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       f  - turn off file mode in favor of block mode\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       n  - run native mode test only\n");
    PRINT(STDOUT, "       r  - read only (assuming files from other run exist)\n");
    PRINT(STDOUT, "       v  - use format version # (default is 2)\n");
    PRINT(STDOUT, "       1  - do NOT run test #1\n");
    PRINT(STDOUT, "       2  - do NOT run test #2\n");
    PRINT(STDOUT, "       3  - do NOT run test #3\n");
    PRINT(STDOUT, "       4  - do NOT run test #4\n");
    PRINT(STDOUT, "       5  - do NOT run test #5\n");
    PRINT(STDOUT, "       6  - do NOT run test #6\n");
    PRINT(STDOUT, "       7  - do NOT run test #7\n");
    PRINT(STDOUT, "       8  - do NOT run test #8\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the PDB Library system */

int main(int c, char **v)
   {int i, err;
    int ton[8];
    int use_mapped_files, check_writes;
    int64_t bfsz;
    fltdes *fl1, *fl2, *fl;

    PD_init_threads(0, NULL);

    SC_bf_set_hooks();
    SC_zero_space_n(1, -2);

    fl1 = PD_filt_make_chain(NULL, "scramble",
			     scramble_file_in, scramble_file_out, NULL);

    fl2 = PD_filt_make_chain(NULL, "scramble",
			     scramble_file_in, scramble_file_out, NULL);
    fl2 = PD_filt_make_chain(fl2, "shrink",
			     shrink_file_in, shrink_file_out, NULL);

    fl = fl1;

    bfsz             = -1;
    bfsz             = 100000;
    check_writes     = FALSE;
    debug_mode       = FALSE;
    native_only      = FALSE;
    read_only        = FALSE;
    use_mapped_files = FALSE;

    for (i = 0; i < 8; i++)
        ton[i] = TRUE;

    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'b' :
		      bfsz = SC_stol(v[++i]);
		      break;
                 case 'c' :
                      switch (v[i][2])
                         {case '0' :
                               fl = NULL;
			       break;
			  case '1' :
                               fl = fl1;
			       break;
                          case '2' :
                               fl = fl2;
			       break;
                          default :
			       check_writes = TRUE;
			       break;};
		      break;
		 case 'd' :
		      debug_mode  = TRUE;
/*		      SC_gs.mm_debug = TRUE; */
		      break;

/* select block filtering and use the 2 filter chain because it
 * preserves the byte size of the items
 * which is currently necessary
 */
		 case 'f' :
		      filter_file  = FALSE;
		      partial_read = FALSE;
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
                 case '2' :
                 case '3' :
                 case '4' :
                 case '5' :
                 case '6' :
                 case '7' :
                 case '8' :
		      ton[v[i][1] - '0'] = FALSE;
		      break;};}
         else
            break;};

    if ((filter_file == FALSE) && (fl == fl1))
       fl = fl2;

    PD_set_io_hooks(use_mapped_files);
    PD_verify_writes(check_writes);

    PD_set_buffer_size(bfsz);

    SC_signal(SIGINT, SIG_DFL);

/* force allocation of permanent memory outside of memory monitors */
    PD_filt_register_file(fl);
    PD_open_vif("foo");

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "\t\t                      Memory                Time\n");
    PRINT(STDOUT, "\t\t                     (bytes)               (secs)\n");
    PRINT(STDOUT, "\t\t     Test  Allocated     Freed      Diff\n");

    err = 0;

/* whole file filtering */
/*    err += work("file 0", ton, NULL, TRUE, native_only); */
    err += work("file 1", ton, fl1,  TRUE, native_only);
    err += work("file 2", ton, fl2,  TRUE, native_only);

/* block filtering */
/*    err += work("block 0", ton, NULL, TRUE, native_only); */
    err += work("block 2", ton, fl2,  FALSE, native_only);

    PD_filt_free_chain(fl1);
    PD_filt_free_chain(fl2);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
