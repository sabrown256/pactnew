/*
 * SCARTS.C - test SC_array functionality
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#define SC_DEBUG

#include "score_int.h"

typedef struct s_ts1 ts1;

struct s_ts1
   {int n;
    char *c;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* RUN_TEST - run a test and report the time and space taken */

static int run_test(int n, PFInt tst)
   {int err;
    double time;
    int64_t da, db;

    SC_ENTERING;

    time = SC_wall_clock_time();
    SC_mem_stats(NULL, NULL, &db, NULL);

    err = (*tst)();

    time = SC_wall_clock_time() - time;
    SC_mem_stats(NULL, NULL, &da, NULL);

    io_printf(STDOUT, "\t\t\t%2d\t%8ld\t%.2f\t%d\n", n, da - db, time, err);

    SC_LEAVING;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TS_FREE - free a ts1 */

static int ts_free(void *a)
   {ts1 *t;

    SC_ENTERING;

    if (a != NULL)
       {t = (ts1 *) a;
	CFREE(t->c);};

    SC_LEAVING;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_1 - test static primitive */

static int test_1(void)
   {int i, na, nc, ni, nd, err;
    int *ib;
    char *cb;
    double *db;
    SC_array *ac, *ai, *ad;
    static char   ca[] = { 'a', 'b', '9' };
    static int    ia[] = { 1, 100, 100000 };
    static double da[] = { 0.0, 2.7183, 3.1415926 };

    SC_ENTERING;

    ac = CMAKE_ARRAY(char, NULL, 0);
    ai = CMAKE_ARRAY(int, NULL, 0);
    ad = CMAKE_ARRAY(double, NULL, 0);

    na = 3;

/* test array set */
    for (i = 0; i < na; i++)
        {SC_array_set(ac, i, &ca[i]);
	 SC_array_set(ai, i, &ia[i]);
         SC_array_set(ad, i, &da[i]);};
    
/* test array push */
    for (i = 0; i < na; i++)
        {SC_array_push(ac, &ca[i]);
	 SC_array_push(ai, &ia[i]);
         SC_array_push(ad, &da[i]);};
    
    cb = SC_array_array(ac);
    ib = SC_array_array(ai);
    db = SC_array_array(ad);

    nc = SC_array_get_n(ac);
    ni = SC_array_get_n(ai);
    nd = SC_array_get_n(ad);

/* now compare */
    err = 0;

    err += (nc != 2*na);
    err += (ni != 2*na);
    err += (nd != 2*na);

/* compare set results */
    for (i = 0; i < na; i++)
        {err += (cb[i] != ca[i]);
	 err += (ib[i] != ia[i]);
	 err += (db[i] != da[i]);};
    
/* compare push results */
    for (i = 0; i < na; i++)
        {err += (cb[na+i] != ca[i]);
	 err += (ib[na+i] != ia[i]);
	 err += (db[na+i] != da[i]);};
    
    CFREE(cb);
    CFREE(ib);
    CFREE(db);

    SC_free_array(ac, NULL);
    SC_free_array(ai, NULL);
    SC_free_array(ad, NULL);

    SC_LEAVING;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_2 - test pointer primitives */

static int test_2(void)
   {int i, na, nc, ni, nd, err;
    int **ib, *ia[3];
    char **cb, *ca[3];
    double **db, *da[3];
    SC_array *ac, *ai, *ad;

    SC_ENTERING;

    na = 3;
    for (i = 0; i < na; i++)
        {ca[i] = CMAKE(char);
	 ia[i] = CMAKE(int);
	 da[i] = CMAKE(double);

	 *ca[i] = 'a' + i;
	 *ia[i] = 10 + i;
	 *da[i] = -10.3 + i;};

    ac = CMAKE_ARRAY(char *, NULL, 0);
    ai = CMAKE_ARRAY(int *, NULL, 0);
    ad = CMAKE_ARRAY(double *, NULL, 0);

/* test array set */
    for (i = 0; i < na; i++)
        {SC_array_set(ac, na-i-1, &ca[i]);
	 SC_array_set(ai, na-i-1, &ia[i]);
         SC_array_set(ad, na-i-1, &da[i]);};
    
/* test array push */
    for (i = 0; i < na; i++)
        {SC_array_push(ac, &ca[i]);
	 SC_array_push(ai, &ia[i]);
         SC_array_push(ad, &da[i]);};
    
    cb = SC_array_array(ac);
    ib = SC_array_array(ai);
    db = SC_array_array(ad);

    nc = SC_array_get_n(ac);
    ni = SC_array_get_n(ai);
    nd = SC_array_get_n(ad);

/* now compare */
    err = 0;

    err += (nc != 2*na);
    err += (ni != 2*na);
    err += (nd != 2*na);

/* compare set results */
    for (i = 0; i < na; i++)
        {err += (*cb[na-i-1] != *ca[i]);
	 err += (*ib[na-i-1] != *ia[i]);
	 err += (*db[na-i-1] != *da[i]);};
    
/* compare push results */
    for (i = 0; i < na; i++)
        {err += (*cb[na+i] != *ca[i]);
	 err += (*ib[na+i] != *ia[i]);
	 err += (*db[na+i] != *da[i]);};
    
    CFREE(cb);
    CFREE(ib);
    CFREE(db);

    SC_free_array(ac, SC_array_free_n);
    SC_free_array(ai, SC_array_free_n);
    SC_free_array(ad, SC_array_free_n);

    SC_LEAVING;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_3 - test static struct */

static int test_3(void)
   {int i, na, ns, err;
    ts1 *sb;
    SC_array *as;
    static ts1 sa[] = { {1, "hello"}, {2, " "}, {3, "world"} };

    SC_ENTERING;

    as = CMAKE_ARRAY(ts1, NULL, 0);

    na = 3;

/* test array set */
    for (i = 0; i < na; i++)
        SC_array_set(as, i, &sa[i]);
    
/* test array push */
    for (i = 0; i < na; i++)
        SC_array_push(as, &sa[i]);
    
    ns = SC_array_get_n(as);
    sb = SC_array_array(as);

/* now compare */
    err = 0;

    err += (ns != 2*na);

/* compare set results */
    for (i = 0; i < na; i++)
        {err += (sb[i].n != sa[i].n);
	 err += (strcmp(sb[i].c, sa[i].c) != 0);}
    
/* compare push results */
    for (i = 0; i < na; i++)
        {err += (sb[i].n != sa[i].n);
	 err += (strcmp(sb[i].c, sa[i].c) != 0);}
    
    CFREE(sb);

    SC_free_array(as, NULL);

    SC_LEAVING;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_4 - test static struct with dynamic members */

static int test_4(void)
   {int i, na, ns, err;
    ts1 *sb;
    SC_array *as;
    static ts1 sa[] = { {1, NULL}, {2, NULL}, {3, NULL} };

    SC_ENTERING;

    na = 3;

    sa[0].c = CSTRSAVE("hello");
    sa[1].c = CSTRSAVE(" ");
    sa[2].c = CSTRSAVE("world");

    as = CMAKE_ARRAY(ts1, NULL, 0);

/* test array set */
    for (i = 0; i < na; i++)
        SC_array_set(as, i, &sa[i]);
    
/* test array push */
    for (i = 0; i < na; i++)
        SC_array_push(as, &sa[i]);
    
    ns = SC_array_get_n(as);
    sb = SC_array_array(as);

/* now compare */
    err = 0;

    err += (ns != 2*na);

/* compare set results */
    for (i = 0; i < na; i++)
        {err += (sb[i].n != sa[i].n);
	 err += (strcmp(sb[i].c, sa[i].c) != 0);}
    
/* compare push results */
    for (i = 0; i < na; i++)
        {err += (sb[i].n != sa[i].n);
	 err += (strcmp(sb[i].c, sa[i].c) != 0);}
    
    CFREE(sb);

    SC_free_array(as, ts_free);

    SC_LEAVING;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_5 - test dynamic array of structs */

static int test_5(void)
   {int i, na, ns, err;
    ts1 **sb, **sa;
    SC_array *as;

    SC_ENTERING;

    na = 3;
    sa = CMAKE_N(ts1 *, na);

    for (i = 0; i < na; i++)
        {sa[i] = CMAKE(ts1);
	 sa[i]->n = i;};

    sa[0]->c = CSTRSAVE("hello");
    sa[1]->c = CSTRSAVE(" ");
    sa[2]->c = CSTRSAVE("world");

    as = CMAKE_ARRAY(ts1 *, NULL, 0);

/* test array set */
    for (i = 0; i < na; i++)
        SC_array_set(as, i, &sa[i]);
    
/* test array push */
    for (i = 0; i < na; i++)
        SC_array_push(as, &sa[i]);
    
    ns = SC_array_get_n(as);
    sb = SC_array_array(as);

/* now compare */
    err = 0;

    err += (ns != 2*na);

/* compare set results */
    for (i = 0; i < na; i++)
        {err += (sb[i]->n != sa[i]->n);
	 err += (strcmp(sb[i]->c, sa[i]->c) != 0);}
    
/* compare push results */
    for (i = 0; i < na; i++)
        {err += (sb[i]->n != sa[i]->n);
	 err += (strcmp(sb[i]->c, sa[i]->c) != 0);}
    
/* cleanup */
    SC_free_array(as, NULL);

    for (i = 0; i < na; i++)
        {CFREE(sa[i]->c);
         CFREE(sa[i]);};
    CFREE(sa);

    CFREE(sb);

    SC_LEAVING;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_6_LESS - sorting test predicate */

static int test_6_less(void *a, void *b)
   {int ok;
    long la, lb;

    SC_ENTERING;

    la = *(long *) a;
    lb = *(long *) b;

    ok = (la < lb);

    SC_LEAVING;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_6 - test sorting dynamic arrays */

static int test_6(void)
   {int err;
    long i, l, m, na;
    SC_array *a;

    SC_ENTERING;

    na = 100;

    a = CMAKE_ARRAY(long, NULL, 0);
    for (i = 0; i < na; i++)
        {l = SC_random_int(1, na+1);
	 SC_array_push(a, &l);};

    SC_array_sort(a, test_6_less);

/* now compare */
    err = 0;

    for (i = 1; i < na; i++)
        {l = *(long *) SC_array_get(a, i);
	 m = *(long *) SC_array_get(a, i-1);
	 err += (m > l);};

    SC_free_array(a, NULL);

    SC_LEAVING;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_7_ADD - add N integers to IA */

static int *test_7_add(SC_array *ia, int n, int recy)
   {int i, ni;
    int *pi;

    SC_ENTERING;

    ni = ia->nx;

    for (i = 0; i < n; i++)
        SC_array_set(ia, i, &i);

/* attempt to recycle the original space from IA
 * this call will grow IA and this means the original
 * pointer will be available
 */
    pi = NULL;
    if (recy == TRUE)
       {pi = CMAKE_N(int, ni);
	for (i = 0; i < ni; i++)
	    pi[i] = 100;};

    SC_LEAVING;

    return(pi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_7 - test asynchronous race
 *        - this is an artificial way to emulate a race
 *        - and it will not valgrind happily - ignore valgrind complaints
 *        - about this test
 */

static int test_7(void)
   {int i, ni, err;
    int *pi, *npi;
    SC_array *ia;

    SC_ENTERING;

    ia = CMAKE_ARRAY(int, NULL, 0);
    test_7_add(ia, 10, FALSE);

    ni = SC_array_get_n(ia);
    pi = SC_array_array(ia);

    npi = NULL;
    for (i = 0; i < ni; i++)
        {pi[i] *= -1;

/* emulate an interrupt that adds to ia */
         if (i == 2)
	    npi = test_7_add(ia, 20, TRUE);};

/* now compare */
    err = 0;

    for (i = 0; i < ni; i++)
        err += (pi[i] != -i);

/* there should be 2 or 10 differences depending on the memory manager
 * NOTE: the full memory manager will get 10 errors
 * while the reduced one will get 2
 */
    err = ((err != 10) && (err != 2));

    CFREE(pi);
    CFREE(npi);

    SC_free_array(ia, NULL);

    SC_LEAVING;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_8 - test permanent arrays */

static int test_8(void)
   {int i, na, err, ocs, ncs;
    char msg[MAXLINE];
    double fc;
    double *dp;
    SC_array *da;

    SC_ENTERING;

    ocs = SC_mem_monitor(-1, 2, "T8", msg);

    na = 1000;

    da = CMAKE_ARRAY(double, NULL, 3);
    SC_array_resize(da, na, -1.0);

/* setting DA */
    for (i = 0; i < na; i++)
        {fc = i;
	 SC_array_set(da, i, &fc);};

/* fetching DA */
    dp = CMAKE_N(double, na);

    for (i = 0; i < na; i++)
        dp[i] = *(double *) SC_array_get(da, i);

    CFREE(dp);

    ncs = SC_mem_monitor(ocs, 2, "T8", msg);

    err = (ncs != ocs);

    SC_LEAVING;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_9 - performance test */

static int test_9(void)
   {int i, ms, mm, mg, ls, lm, lg, na, err;
    double fc, tm, ts, tg, tr, rs, rg;
    double *dp;
    SC_array *da;

    SC_ENTERING;

    err = 0;

    na = 1000000;

    da = CMAKE_ARRAY(double, NULL, 0);
    SC_array_resize(da, na, -1.0);

    dp = CMAKE_N(double, na);

/* time setting DP */
    lm = 0;
    tm = 0.0;
    for (mm = 0; TRUE; mm++)
        {tr = SC_wall_clock_time();

         for (i = 0; i < na; i++, lm++)
/*             {fc    = log10(1.0 + i); */
             {fc    = i;
              dp[i] = fc;};

	 tm += (SC_wall_clock_time() - tr);

	 if (tm > 2.0)
	    break;};

/* time setting DA */
    ls = 0;
    ts = 0.0;
    for (ms = 0; TRUE; ms++)
        {tr = SC_wall_clock_time();

         for (i = 0; i < na; i++, ls++)
/*             {fc    = log10(1.0 + i); */
             {fc    = i;
              SC_array_set(da, i, &fc);};

	 ts += (SC_wall_clock_time() - tr);

	 if (ts > 2.0)
	    break;};

/* time fetching DA */
    lg = 0;
    tg = 0.0;
    for (mg = 0; TRUE; mg++)
        {tr = SC_wall_clock_time();

         for (i = 0; i < na; i++, lg++)
             dp[i] = *(double *) SC_array_get(da, i);

	 tg += (SC_wall_clock_time() - tr);

	 if (tg > 2.0)
	    break;};

    rs = (ts*lm)/(tm*ls);
    rg = (tg*lm)/(tm*lg);

    printf("\n");
    printf("\t\t\t          Acesses      usec/access   ratio\n"); 
    printf("\t\t\tdirect   %9.2e   %11.3e   %11.3e\n",
	   (double) lm, 1.0e6*tm/((double) lm), 1.0);
    printf("\t\t\tset      %9.2e   %11.3e   %11.3e\n",
	   (double) ls, 1.0e6*ts/((double) ls), rs);
    printf("\t\t\tget      %9.2e   %11.3e   %11.3e\n",
	   (double) lg, 1.0e6*tg/((double) lg), rg);

    SC_free_array(da, NULL);
    CFREE(dp);

    SC_LEAVING;

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* MAIN - start here */

int main(int c, char **v)
   {int i, n, err, debug;
    long sc;
    char msg[MAXLINE];
    int tstf[] = { TRUE, TRUE, TRUE, TRUE, TRUE,
		   TRUE, TRUE, TRUE, TRUE, TRUE };
    PFInt tfnc[] = { NULL, test_1, test_2, test_3, test_4,
		     test_5, test_6, test_7, test_8, test_9 };

    debug = 0;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-1") == 0)
            tstf[1] = FALSE;
	 else if (strcmp(v[i], "-2") == 0)
            tstf[2] = FALSE;
	 else if (strcmp(v[i], "-3") == 0)
            tstf[3] = FALSE;
	 else if (strcmp(v[i], "-4") == 0)
            tstf[4] = FALSE;
	 else if (strcmp(v[i], "-5") == 0)
            tstf[5] = FALSE;
	 else if (strcmp(v[i], "-6") == 0)
            tstf[6] = FALSE;
	 else if (strcmp(v[i], "-7") == 0)
            tstf[7] = FALSE;
	 else if (strcmp(v[i], "-8") == 0)
            tstf[8] = FALSE;
	 else if (strcmp(v[i], "-9") == 0)
            tstf[9] = FALSE;
	 else if (strcmp(v[i], "-m") == 0)
            debug = 2;
	 else if (strcmp(v[i], "-h") == 0)
	    {printf("Usage: scarts [-1] [-2] [-3] [-4] [-5] [-6] [-7] [-8] [-9] [-m] [-h]\n");
             return(1);};};

    SC_zero_space_n(1, -2);
    sc = SC_mem_monitor(-1, debug, "A", msg);

    io_printf(STDOUT, "\t\t\tTest\tLeaked Memory\tTime\tErrors\n");

    err = 0;

    n = sizeof(tstf)/sizeof(int);
    for (i = 1; i <= n; i++)
        {if (tstf[i] == TRUE)
	    err += run_test(i, tfnc[i]);};

    io_printf(STDOUT, "\n");

    sc = SC_mem_monitor(sc, debug, "A", msg);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
