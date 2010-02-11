/*
 * SCARTS.C - test SC_array functionality
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

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
    long da, db;

    time = SC_wall_clock_time();
    SC_mem_stats(NULL, NULL, &db, NULL);

    err = (*tst)();

    time = SC_wall_clock_time() - time;
    SC_mem_stats(NULL, NULL, &da, NULL);

    io_printf(STDOUT, "\t\t\t%2d\t%8ld\t%.2f\t%d\n", n, da - db, time, err);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TS_FREE - free a ts1 */

static int ts_free(void *a)
   {ts1 *t;

    if (a != NULL)
       {t = (ts1 *) a;
	SFREE(t->c);};

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

    ac = SC_MAKE_ARRAY("TEST_1", char, NULL);
    ai = SC_MAKE_ARRAY("TEST_1", int, NULL);
    ad = SC_MAKE_ARRAY("TEST_1", double, NULL);

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
    
    cb = SC_array_array(ac, 0);
    ib = SC_array_array(ai, 0);
    db = SC_array_array(ad, 0);

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
    
    SC_free_array(ac, NULL);
    SC_free_array(ai, NULL);
    SC_free_array(ad, NULL);

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

    na = 3;
    for (i = 0; i < na; i++)
        {ca[i] = FMAKE(char, "TEST_2:ca[i]");
	 ia[i] = FMAKE(int, "TEST_2:ia[i]");
	 da[i] = FMAKE(double, "TEST_2:da[i]");

	 *ca[i] = 'a' + i;
	 *ia[i] = 10 + i;
	 *da[i] = -10.3 + i;};

    ac = SC_MAKE_ARRAY("TEST_2", char *, NULL);
    ai = SC_MAKE_ARRAY("TEST_2", int *, NULL);
    ad = SC_MAKE_ARRAY("TEST_2", double *, NULL);

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
    
    cb = SC_array_array(ac, 0);
    ib = SC_array_array(ai, 0);
    db = SC_array_array(ad, 0);

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
    
    SC_free_array(ac, SC_array_free_n);
    SC_free_array(ai, SC_array_free_n);
    SC_free_array(ad, SC_array_free_n);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_3 - test static struct */

static int test_3(void)
   {int i, na, ns, err;
    ts1 *sb;
    SC_array *as;
    static ts1 sa[] = { {1, "hello"}, {2, " "}, {3, "world"} };

    as = SC_MAKE_ARRAY("TEST_3", ts1, NULL);

    na = 3;

/* test array set */
    for (i = 0; i < na; i++)
        SC_array_set(as, i, &sa[i]);
    
/* test array push */
    for (i = 0; i < na; i++)
        SC_array_push(as, &sa[i]);
    
    ns = SC_array_get_n(as);
    sb = SC_array_array(as, 0);

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
    
    SC_free_array(as, NULL);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_4 - test static struct with dynamic members */

static int test_4(void)
   {int i, na, ns, err;
    ts1 *sb;
    SC_array *as;
    static ts1 sa[] = { {1, NULL}, {2, NULL}, {3, NULL} };

    na = 3;

    sa[0].c = SC_strsavef("hello", "char*:TEST_4:c");
    sa[1].c = SC_strsavef(" ", "char*:TEST_4:c");
    sa[2].c = SC_strsavef("world", "char*:TEST_4:c");

    as = SC_MAKE_ARRAY("TEST_4", ts1, NULL);

/* test array set */
    for (i = 0; i < na; i++)
        SC_array_set(as, i, &sa[i]);
    
/* test array push */
    for (i = 0; i < na; i++)
        SC_array_push(as, &sa[i]);
    
    ns = SC_array_get_n(as);
    sb = SC_array_array(as, 0);

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
    
    SC_free_array(as, ts_free);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_5 - test dynamic array of structs */

static int test_5(void)
   {int i, na, ns, err;
    ts1 **sb, **sa;
    SC_array *as;

    na = 3;
    sa = FMAKE_N(ts1 *, na, "TEST_5:sa");

    for (i = 0; i < na; i++)
        {sa[i] = FMAKE(ts1, "TEST_5:sa[i]");
	 sa[i]->n = i;};

    sa[0]->c = SC_strsavef("hello", "char*:TEST_5:c");
    sa[1]->c = SC_strsavef(" ", "char*:TEST_5:c");
    sa[2]->c = SC_strsavef("world", "char*:TEST_5:c");

    as = SC_MAKE_ARRAY("TEST_5", ts1 *, NULL);

/* test array set */
    for (i = 0; i < na; i++)
        SC_array_set(as, i, &sa[i]);
    
/* test array push */
    for (i = 0; i < na; i++)
        SC_array_push(as, &sa[i]);
    
    ns = SC_array_get_n(as);
    sb = SC_array_array(as, 0);

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
    for (i = 0; i < na; i++)
        {SFREE(sa[i]->c);
         SFREE(sa[i]);};
    SFREE(sa);

    SC_free_array(as, ts_free);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* MAIN - start here */

int main(int c, char **v)
   {int i, err, debug;
    long sc;
    char msg[MAXLINE];

    debug = 0;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-m") == 0)
            debug = 2;
	 else if (strcmp(v[i], "-h") == 0)
	    {printf("Usage: scarts [-m] [-h]\n");
             return(1);};};

    sc = SC_mem_monitor(-1, debug, "A", msg);

    io_printf(STDOUT, "\t\t\tTest\tLeaked Memory\tTime\tErrors\n");

    err = 0;

    err += run_test(1, test_1);
    err += run_test(2, test_2);
    err += run_test(3, test_3);
    err += run_test(4, test_4);
    err += run_test(5, test_5);

    io_printf(STDOUT, "\n");

    sc = SC_mem_monitor(sc, debug, "A", msg);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
