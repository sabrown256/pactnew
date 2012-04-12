/*
 * TPDRC.C - test PD_cast and PD_size_from
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbtfr.h"

#define DATFILE "cast"

typedef int (*PFTest)(char *base, char *tgt, int n);

typedef struct S_List List;
typedef struct S_BSTRUCT BSTRUCT;
typedef struct S_ASTRUCT ASTRUCT;

struct S_ASTRUCT
   {int val;};

struct S_BSTRUCT
   {float x;
    float y;};

struct S_List
   {char *type;
    void *data;
    struct S_List *next;};

typedef struct s_da2 da2;

struct s_da2
   {long n;
    char *a;};

typedef struct s_db2 db2;

struct s_db2
   {int i;
    long j;
    char *a;};

typedef struct s_da3 da3;

struct s_da3
   {long n[3];
    char *a;};

typedef struct s_db3 db3;

struct s_db3
   {long *n;
    char *a;};

static int
 debug_mode = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - recursive cast test */

int test_1(char *base, char *tgt, int n)
   {int i, rv;
    char datfile[MAXLINE], fname[MAXLINE];
    char *aname, *bname;
    PDBfile *file; 
    defstr *dp;
    List *head, *ptr, *nxt;
    ASTRUCT *aptr;
    BSTRUCT *bptr;

    rv = TRUE;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

    file = PD_open(datfile, "w"); 

    dp = PD_defstr(file,
		   "List",
		   "char *type",
		   "char *data", 
		   "List *next",
		   LAST);
    if (dp == NULL)
       PRINT(stdout, "PD_defstr List: %s\n", PD_get_error());
  
    dp = PD_defstr(file,
		   "ASTRUCT",
		   "int val",
		   LAST);
    if (dp == NULL)
       PRINT(stdout, "PD_defstr A: %s\n", PD_get_error());
    
    dp = PD_defstr(file,
		   "BSTRUCT",
		   "float x",
		   "float y",
		   LAST);
    if (dp == NULL)
       PRINT(stdout, "PD_defstr B: %s\n", PD_get_error());
  
    if (PD_cast(file, "List", "data", "type") == 0)
       PRINT(stdout, "PD_cast: %s\n", PD_get_error());

    head = CMAKE(List);
    ptr  = head;

    aname = CSTRSAVE("ASTRUCT *");
    bname = CSTRSAVE("BSTRUCT *");

    for (i = 0; i < 10; ++i)
        {if (i % 2 == 0)
	    {aptr      = CMAKE(ASTRUCT);
	     ptr->data = aptr;
	     ptr->type = aname;
	     aptr->val = i;}

	 else
	    {bptr      = CMAKE(BSTRUCT);
	     ptr->data = bptr;
	     ptr->type = bname;
	     bptr->x   = 1.2*i;
	     bptr->y   = 1.1*i;};
     
	 if (i != 9)
	    {ptr->next = CMAKE(List);
	     ptr       = ptr->next;}
	 else
	    ptr->next = NULL;};
    
    if (PD_write(file, "mylist", "List", head) == 0)
       PRINT(stdout, "PD_write: %s\n", PD_get_error());

    PD_close(file);

    for (ptr = head; ptr != NULL; ptr = nxt)
        {nxt = ptr->next;
	 CFREE(ptr->data);
	 CFREE(ptr);};
    CFREE(aname);
    CFREE(bname);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - test of PD_size_from */

int test_2(char *base, char *tgt, int n)
   {int i, l, m1, m2, ok, rv;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *file; 
    defstr *dp;
    da2 va_r, va_w;
    db2 wa_r, wa_w;

    rv = TRUE;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

/* write data */
    file = PD_open(datfile, "w");

    dp = PD_defstr(file, "da2",
		   "long n",
		   "char *a", 
		   LAST);
    if (dp == NULL)
       PRINT(stdout, "PD_defstr da2: %s\n", PD_get_error());
  
    ok = PD_size_from(file, "da2", "a", "n");
    if (ok == 0)
       PRINT(stdout, "PD_size_from: %s\n", PD_get_error());

    dp = PD_defstr(file, "db2",
		   "int i",
		   "long j",
		   "char *a", 
		   LAST);
    if (dp == NULL)
       PRINT(stdout, "PD_defstr db2: %s\n", PD_get_error());
  
    ok = PD_size_from(file, "db2", "a", "i,j");
    if (ok == 0)
       PRINT(stdout, "PD_size_from: %s\n", PD_get_error());

    m1 = 10;
    m2 = 2;

    va_w.n = m1;
    va_w.a = CMAKE_N(char, va_w.n+10);
    for (i = 0; i < m1; i++)
        va_w.a[i] = 'a' + i;
    va_w.a[i] = '\0';

    wa_w.i = m1;
    wa_w.j = m2;
    wa_w.a = CMAKE_N(char, m1*m2 + 1);
    for (i = 0; i < m1*m2; i++)
        wa_w.a[i] = 'a' + i;
    wa_w.a[i] = '\0';

    PD_write(file, "va", "da2", &va_w);
    PD_write(file, "wa", "db2", &wa_w);

    PD_close(file);

/* read data */
    file = PD_open(datfile, "r");

    ok = PD_read(file, "va", &va_r);
    ok = PD_read(file, "wa", &wa_r);

    PD_close(file);

/* compare data */
    rv = TRUE;

    rv &= (va_r.n == va_w.n);
    for (i = 0; i < va_r.n; i++)
        rv &= (va_r.a[i] == va_w.a[i]);

    l = wa_r.i * wa_r.j;
    rv &= (wa_r.i == wa_w.i);
    rv &= (wa_r.j == wa_w.j);
    for (i = 0; i < l; i++)
        rv &= (wa_r.a[i] == wa_w.a[i]);

    CFREE(va_w.a);
    CFREE(wa_w.a);
    CFREE(va_r.a);
    CFREE(wa_r.a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - test of PD_size_from with multi-dimension sizing member */

int test_3(char *base, char *tgt, int n)
   {int i, ok, rv;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *file; 
    defstr *dp;
    da3 va_r, va_w;

    rv = TRUE;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile);

/* write data */
    file = PD_open(datfile, "w");

    dp = PD_defstr(file, "da3",
		   "long n[3]",
		   "char *a", 
		   LAST);
    if (dp == NULL)
       PRINT(stdout, "PD_defstr da3: %s\n", PD_get_error());
  
    ok = PD_size_from(file, "da3", "a", "n");
    if (ok == 0)
       PRINT(stdout, "PD_size_from: %s\n", PD_get_error());

    for (i = 0; i < 3; i++)
        va_w.n[i] = i+2;

    n = 1;
    for (i = 0; i < 3; i++)
        n *= va_w.n[i];

    va_w.a = CMAKE_N(char, n+10);
    for (i = 0; i < n; i++)
        va_w.a[i] = 'a' + i;
    va_w.a[i] = '\0';

    PD_write(file, "va", "da3", &va_w);

    PD_close(file);

/* read data */
    file = PD_open(datfile, "r");

    ok = PD_read(file, "va", &va_r);

    PD_close(file);

/* compare data */
    rv = TRUE;

    n = 1;
    for (i = 0; i < 3; i++)
        n *= va_r.n[i];

    for (i = 0; i < 3; i++)
        rv &= (va_r.n[i] == va_w.n[i]);

    for (i = 0; i < n; i++)
        rv &= (va_r.a[i] == va_w.a[i]);

    CFREE(va_w.a);
    CFREE(va_r.a);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                              DRIVER ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* RUN_TEST - run a particular test through all targeting modes
 *          - return the number of tests that fail
 */

static int run_test(PFTest test, int n, char *host, int native)
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

void print_help(void)
   {

    PRINT(STDOUT, "\nTPDRC - run PDB cast tests\n\n");
    PRINT(STDOUT, "Usage: tpdrc [-d] [-h] [-n] [-r] [-v #] [-1] [-2] [-3]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d  - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       n  - run native mode test only\n");
    PRINT(STDOUT, "       r  - read only mode (assuming file from previous run)\n");
    PRINT(STDOUT, "       v  - use specified format version (default 2)\n");
    PRINT(STDOUT, "       1  - do NOT run test #1\n");
    PRINT(STDOUT, "       2  - do NOT run test #2\n");
    PRINT(STDOUT, "       3  - do NOT run test #3\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, n, err;
    int native_only;
    int ton[10];
  
    for (i = 0; i < 10; i++)
        ton[i] = TRUE;

    read_only   = FALSE;
    native_only = FALSE;
    for (i = 1; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
                {case 'd' :
		      debug_mode     = TRUE;
		      SC_gs.mm_debug = TRUE;
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'n' :
		      native_only = TRUE;
                      break;
                 case 'r' :
		      read_only = TRUE;
                      break;
                 case 'v' :
                      PD_set_fmt_version(SC_stoi(v[++i]));
		      break;
                 default:
                      n = -SC_stoi(v[i]);
		      ton[n] = FALSE;
		      break;};};};

    SC_signal(SIGINT, SIG_DFL);

    err = 0;

    if (ton[1])
       err += run_test(test_1, 1, DATFILE, native_only);
    if (ton[2])
       err += run_test(test_2, 2, DATFILE, native_only);
    if (ton[3])
       err += run_test(test_3, 3, DATFILE, native_only);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

