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
#define N        3

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

typedef struct s_da4 da4;

struct s_da4
   {char *type;
    long *n;
    void *a;};

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
    test_target(tgt, base, n, fname, datfile, MAXLINE);

    file = PD_open(datfile, "w"); 

    dp = PD_defstr(file,
		   "List",
		   "char *type",
		   "char *data", 
		   "List *next",
		   LAST);
    if (dp == NULL)
       error(-1, stdout, "PD_defstr List: %s\n", PD_get_error());
  
    dp = PD_defstr(file,
		   "ASTRUCT",
		   "int val",
		   LAST);
    if (dp == NULL)
       error(-1, stdout, "PD_defstr A: %s\n", PD_get_error());
    
    dp = PD_defstr(file,
		   "BSTRUCT",
		   "float x",
		   "float y",
		   LAST);
    if (dp == NULL)
       error(-1, stdout, "PD_defstr B: %s\n", PD_get_error());
  
    if (PD_cast(file, "List", "data", "type") == 0)
       error(-1, stdout, "PD_cast: %s\n", PD_get_error());

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
       error(-1, stdout, "PD_write: %s\n", PD_get_error());

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
   {int i, l, m1, m2, nr, nw, ok, rv;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *file; 
    defstr *dp;
    da2 va_r[N], va_w[N];
    db2 wa_r[N], wa_w[N];

    rv = TRUE;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

/* write data */
    file = PD_open(datfile, "w");

    dp = PD_defstr(file, "da2",
		   "long n",
		   "char *a", 
		   LAST);
    if (dp == NULL)
       error(-1, stdout, "PD_defstr da2: %s\n", PD_get_error());
  
    ok = PD_size_from(file, "da2", "a", "n");
    if (ok == 0)
       error(-1, stdout, "PD_size_from: %s\n", PD_get_error());

    dp = PD_defstr(file, "db2",
		   "int i",
		   "long j",
		   "char *a", 
		   LAST);
    if (dp == NULL)
       error(-1, stdout, "PD_defstr db2: %s\n", PD_get_error());
  
    ok = PD_size_from(file, "db2", "a", "i,j");
    if (ok == 0)
       error(-1, stdout, "PD_size_from: %s\n", PD_get_error());

    m1 = 10;
    m2 = 2;
    nw = m1*m2;

    for (l = 0; l < N; l++)
        {va_w[l].n = m1 + 1;
	 va_w[l].a = CMAKE_N(char, va_w[l].n+10);
	 for (i = 0; i < m1; i++)
	     va_w[l].a[i] = 'a' + i + l;
	 va_w[l].a[i] = '\0';

	 wa_w[l].i = m1;
	 wa_w[l].j = m2;
	 wa_w[l].a = CMAKE_N(char, nw + 1);
	 for (i = 0; i < nw; i++)
	     wa_w[l].a[i] = 'a' + i + l;
	 wa_w[l].a[i] = '\0';};

    PD_write(file, "va[3]", "da2", va_w);
    PD_write(file, "wa[3]", "db2", wa_w);

    PD_close(file);

/* read data */
    file = PD_open(datfile, "r");

    ok = PD_read(file, "va", va_r);
    ok = PD_read(file, "wa", wa_r);

    PD_close(file);

/* compare data */
    rv = TRUE;

    for (l = 0; l < N; l++)
        {rv &= (va_r[l].n == va_w[l].n);
	 for (i = 0; i < va_r[l].n; i++)
	     rv &= (va_r[l].a[i] == va_w[l].a[i]);

	 nr = wa_r[l].i * wa_r[l].j;

	 rv &= (wa_r[l].i == wa_w[l].i);
	 rv &= (wa_r[l].j == wa_w[l].j);
	 for (i = 0; i < nr; i++)
	     rv &= (wa_r[l].a[i] == wa_w[l].a[i]);};

/* cleanup memory */
    for (l = 0; l < N; l++)
        {CFREE(va_w[l].a);
	 CFREE(wa_w[l].a);
	 CFREE(va_r[l].a);
	 CFREE(wa_r[l].a);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - test of PD_size_from with multi-dimension
 *        - and indirect sizing members
 */

int test_3(char *base, char *tgt, int n)
   {int i, l, nw, nr, ok, rv;
    char datfile[MAXLINE], fname[MAXLINE];
    PDBfile *file; 
    defstr *dp;
    da3 va_r[N], va_w[N];
    db3 wa_r[N], wa_w[N];

    rv = TRUE;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

/* open file for writing */
    file = PD_open(datfile, "w");

/* setup types */
    dp = PD_defstr(file, "da3",
		   "long n[3]",
		   "char *a", 
		   LAST);
    if (dp == NULL)
       error(-1, stdout, "PD_defstr da3: %s\n", PD_get_error());
  
    ok = PD_size_from(file, "da3", "a", "n");
    if (ok == 0)
       error(-1, stdout, "PD_size_from: %s\n", PD_get_error());

    dp = PD_defstr(file, "db3",
		   "long *n",
		   "char *a", 
		   LAST);
    if (dp == NULL)
       error(-1, stdout, "PD_defstr db3: %s\n", PD_get_error());
  
    ok = PD_size_from(file, "db3", "a", "n");
    if (ok == 0)
       error(-1, stdout, "PD_size_from: %s\n", PD_get_error());

/* setup data */
    for (l = 0; l < N; l++)
        {for (i = 0; i < 3; i++)
	     va_w[l].n[i] = i + l + 2;

	 nw = 1;
	 for (i = 0; i < 3; i++)
	     nw *= va_w[l].n[i];

	 va_w[l].a = CMAKE_N(char, nw+10);
	 for (i = 0; i < nw; i++)
	     va_w[l].a[i] = 'a' + i + l;
	 va_w[l].a[i] = '\0';

	 wa_w[l].n = CMAKE_N(long, 3);
	 for (i = 0; i < 3; i++)
	     wa_w[l].n[i] = i + l + 2;

	 nw = 1;
	 for (i = 0; i < 3; i++)
	     nw *= wa_w[l].n[i];

	 wa_w[l].a = CMAKE_N(char, nw+10);
	 for (i = 0; i < nw; i++)
	     wa_w[l].a[i] = 'a' + i + l;
	 wa_w[l].a[i] = '\0';};

/* write the data */
    PD_write(file, "va[3]", "da3", va_w);
    PD_write(file, "wa[3]", "db3", wa_w);

    PD_close(file);

/* read data */
    file = PD_open(datfile, "r");

    ok = PD_read(file, "va", va_r);
    ok = PD_read(file, "wa", wa_r);

    PD_close(file);

/* compare data */
    rv = TRUE;

    for (l = 0; l < N; l++)
        {nr = 1;
	 for (i = 0; i < 3; i++)
	     nr *= va_r[l].n[i];

	 for (i = 0; i < 3; i++)
	     rv &= (va_r[l].n[i] == va_w[l].n[i]);

	 for (i = 0; i < nr; i++)
	     rv &= (va_r[l].a[i] == va_w[l].a[i]);

	 nr = 1;
	 for (i = 0; i < 3; i++)
	     nr *= wa_r[l].n[i];

	 for (i = 0; i < 3; i++)
	     rv &= (wa_r[l].n[i] == wa_w[l].n[i]);

	 for (i = 0; i < nr; i++)
	     rv &= (wa_r[l].a[i] == wa_w[l].a[i]);};

/* cleanup memory */
    for (l = 0; l < N; l++)
        {CFREE(va_w[l].a);
	 CFREE(va_r[l].a);
	 CFREE(wa_w[l].n);
	 CFREE(wa_w[l].a);
	 CFREE(wa_r[l].n);
	 CFREE(wa_r[l].a);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test PD_size_from combined with PD_cast */

int test_4(char *base, char *tgt, int n)
   {int i, l, nr, nw, ok, rv;
    char datfile[MAXLINE], fname[MAXLINE];
    float *fr, *fw;
    PDBfile *file; 
    defstr *dp;
    da4 va_r[N], va_w[N];

    rv = TRUE;

/* target the file as asked */
    test_target(tgt, base, n, fname, datfile, MAXLINE);

/* write data */
    file = PD_open(datfile, "w");

    dp = PD_defstr(file, "da4",
                   "char *type",
		   "long *n",
		   "char *a", 
		   LAST);
    if (dp == NULL)
       error(-1, stdout, "PD_defstr da4: %s\n", PD_get_error());
  
    ok = PD_cast(file, "da4", "a", "type");
    if (ok == 0)
       error(-1, stdout, "PD_size_from: %s\n", PD_get_error());

    ok = PD_size_from(file, "da4", "a", "n");
    if (ok == 0)
       error(-1, stdout, "PD_size_from: %s\n", PD_get_error());

    for (l = 0; l < N; l++)
        {va_w[l].type = CSTRSAVE("float *");

	 va_w[l].n = CMAKE_N(long, 3);
	 for (i = 0; i < 3; i++)
	     va_w[l].n[i] = i + l + 2;

	 nw = 1;
	 for (i = 0; i < 3; i++)
	     nw *= va_w[l].n[i];

	 fr = CMAKE_N(float, nw);
	 for (i = 0; i < nw; i++)
	     fr[i] = 'a' + i + l;
	 va_w[l].a = fr;};

    PD_write(file, "va[3]", "da4", va_w);

    PD_close(file);

/* read data */
    file = PD_open(datfile, "r");

    ok = PD_read(file, "va", va_r);

    PD_close(file);

/* compare data */
    rv = TRUE;

    for (l = 0; l < N; l++)
        {nr = 1;
	 for (i = 0; i < 3; i++)
	     nr *= va_r[l].n[i];

	 for (i = 0; i < 3; i++)
	     rv &= (va_r[l].n[i] == va_w[l].n[i]);

	 fr = va_r[l].a;
	 fw = va_w[l].a;
	 for (i = 0; i < nr; i++)
	     rv &= (fr[i] == fw[i]);};

/* cleanup memory */
    for (l = 0; l < N; l++)
        {CFREE(va_w[l].type);
	 CFREE(va_w[l].n);
	 CFREE(va_w[l].a);
	 CFREE(va_r[l].type);
	 CFREE(va_r[l].n);
	 CFREE(va_r[l].a);};

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
	        {error(-1, stdout, "Test #%d %s failed\n", n, nm);
		 fail++;};};};

/* native test */
    if ((*test)(host, NULL, n) == FALSE)
       {error(-1, stdout, "Test #%d native failed\n", n);
	fail++;};

    post_test(&st, n);

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

void print_help(void)
   {

    PRINT(STDOUT, "\nTPDRC - run PDB cast tests\n\n");
    PRINT(STDOUT, "Usage: tpdrc [-d] [-h] [-n] [-r] [-v #] [-1] [-2] [-3] [-4]\n");
    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "       d  - turn on debug mode to display memory maps\n");
    PRINT(STDOUT, "       h  - print this help message and exit\n");
    PRINT(STDOUT, "       n  - run native mode test only\n");
    PRINT(STDOUT, "       r  - read only mode (assuming file from previous run)\n");
    PRINT(STDOUT, "       v  - use specified format version (default 2)\n");
    PRINT(STDOUT, "       1  - do NOT run test #1\n");
    PRINT(STDOUT, "       2  - do NOT run test #2\n");
    PRINT(STDOUT, "       3  - do NOT run test #3\n");
    PRINT(STDOUT, "       4  - do NOT run test #4\n");
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

    PRINT(STDOUT, "\n");

    err = 0;

    if (ton[1])
       err += run_test(test_1, 1, DATFILE, native_only);
    if (ton[2])
       err += run_test(test_2, 2, DATFILE, native_only);
    if (ton[3])
       err += run_test(test_3, 3, DATFILE, native_only);
    if (ton[4])
       err += run_test(test_4, 4, DATFILE, native_only);

    PRINT(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

