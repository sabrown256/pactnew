/*
 * SCATST.C - test hashing and alist functionality
 *          - they share the following functionality
 *          -    install (SC_hasharr_install, SC_change_alist)
 *          -    lookup  (SC_hasharr_lookup, SC_hasharr_def_lookup, SC_assoc, SC_assoc_entry)
 *          -    remove  (SC_hasharr_remove, SC_rem_alist)
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* RUN_TEST - run a test and report the time and space taken */

static int run_test(int n, PFInt tst)
   {int ret, sc;
    char msg[MAXLINE];
    double time;
    long da, db;
    static int debug = 0;

    sc = SC_mem_monitor(-1, debug, "A", msg);

    time = SC_wall_clock_time();
    SC_mem_stats(NULL, NULL, &db, NULL);

    ret  = (*tst)();

    time  = SC_wall_clock_time() - time;
    SC_mem_stats(NULL, NULL, &da, NULL);

    io_printf(STDOUT, "\t\t\t%2d\t%8ld\t%.2f\n", n, da - db, time);

    sc = SC_mem_monitor(sc, debug, "A", msg);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_1_HASH - test 1 hash table */

static int test_1_hash(void)
   {int err;
    char *sa, *sb;
    char *ta, *tb;
    hasharr *tab;
    haelem *hpa, *hpb;

    err = 0;

    sa = CSTRSAVE("string-a");
    sb = CSTRSAVE("string-b");

    tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
    hpa = SC_hasharr_install(tab, "a", sa, SC_STRING_S, TRUE, TRUE);
    hpb = SC_hasharr_install(tab, "b", sb, SC_STRING_S, TRUE, TRUE);

    err += (hpa == NULL);
    err += (hpb == NULL);

    err += (hpa != SC_hasharr_lookup(tab, "a"));
    err += (hpb != SC_hasharr_lookup(tab, "b"));
    
    ta = (char *) SC_hasharr_def_lookup(tab, "a");
    tb = (char *) SC_hasharr_def_lookup(tab, "b");

    err += (ta == NULL);
    err += (tb == NULL);

    err += (strcmp(sa, ta) != 0);
    err += (strcmp(sb, tb) != 0);

    err += (SC_hasharr_remove(tab, "a") != TRUE);
    err += (SC_hasharr_remove(tab, "b") != TRUE);

    SC_free_hasharr(tab, NULL, NULL);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_1_HA - test 1 hasharr */

static int test_1_ha(void)
   {int err;
    char *sa, *sb;
    char *ta, *tb;
    hasharr *ha;
    haelem *hpa, *hpb;

    err = 0;

    sa = CSTRSAVE("string-a");
    sb = CSTRSAVE("string-b");

    ha = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
    hpa = SC_hasharr_install(ha, "a", sa, SC_STRING_S, TRUE, TRUE);
    hpb = SC_hasharr_install(ha, "b", sb, SC_STRING_S, TRUE, TRUE);

    err += (hpa == NULL);
    err += (hpb == NULL);

    err += (hpa != SC_hasharr_lookup(ha, "a"));
    err += (hpb != SC_hasharr_lookup(ha, "b"));
    
    ta = (char *) SC_hasharr_def_lookup(ha, "a");
    tb = (char *) SC_hasharr_def_lookup(ha, "b");

    err += (ta == NULL);
    err += (tb == NULL);

    err += (strcmp(sa, ta) != 0);
    err += (strcmp(sb, tb) != 0);

    err += (SC_hasharr_remove(ha, "a") != TRUE);
    err += (SC_hasharr_remove(ha, "b") != TRUE);

    SC_free_hasharr(ha, NULL, NULL);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_1_AL - test 1 association list */

static int test_1_al(void)
   {int err;
    char *sa, *sb;
    char *ta, *tb;
    pcons *lsa, *lsb;

    err = 0;

    sa = CSTRSAVE("string-a");
    sb = CSTRSAVE("string-b");

    lsa = SC_add_alist(NULL, "a", SC_STRING_S, sa);
    lsb = SC_add_alist(lsa,  "b", SC_STRING_S, sb);
    err += (lsa == NULL);
    err += (lsb == NULL);

    err += (lsa->car != SC_assoc_entry(lsa, "a"));
    err += (lsb->car != SC_assoc_entry(lsb, "b"));
    
    ta = (char *) SC_assoc(lsb, "a");
    tb = (char *) SC_assoc(lsb, "b");
    err += (strcmp(sa, ta) != 0);
    err += (strcmp(sb, tb) != 0);

    lsb = SC_rem_alist(lsb, "a");
    lsb = SC_rem_alist(lsb, "b");
    err += (lsb != NULL);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_1 - test shared functionality
 *        -   install
 *        -   lookup
 *        -   def-lookup
 *        -   remove
 */

static int test_1(void)
   {int err;

    err = 0;

/* hash table */
    err += test_1_hash();

/* hasharr */
    err += test_1_ha();

/* alist */
    err += test_1_al();

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_2 - test hash only functionality */

static int test_2(void)
   {

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* TEST_3 - test alist only functionality */

static int test_3(void)
   {int err;
    char *sa, *sb;
    char *ta, *tb;
    pcons *lsa, *lsb, *lsc;

    err = 0;

    sa = CSTRSAVE("string-a");
    sb = CSTRSAVE("string-b");

    lsa = NULL;
    lsa = SC_add_alist(lsa, "a1", SC_STRING_S, sa);
    lsa = SC_add_alist(lsa, "a2", SC_STRING_S, sb);
    err += (lsa == NULL);

    lsb = NULL;
    lsb = SC_add_alist(lsb, "b1", SC_STRING_S, sa);
    lsb = SC_add_alist(lsb, "b2", SC_STRING_S, sb);
    err += (lsb == NULL);

    lsc = SC_append_alist(NULL, lsa);
    lsc = SC_append_alist(lsc, lsb);
    err += (lsc == NULL);

    ta = (char *) SC_assoc(lsc, "a1");
    tb = (char *) SC_assoc(lsc, "b2");
    err += (strcmp(sa, ta) != 0);
    err += (strcmp(sb, tb) != 0);

    lsc = SC_rem_alist(lsc, "a1");
    lsc = SC_rem_alist(lsc, "a2");
    lsc = SC_rem_alist(lsc, "b1");
    lsc = SC_rem_alist(lsc, "b2");
    err += (lsc != NULL);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4_CALLBACK - test iterating over a NULL hash table */

static int test_4_callback(haelem *hp, void *arg)
   {

    return(-1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test iterating over a NULL hash table */

static int test_4(void)
   {int err;
    hasharr *tab;

    err = 0;

    tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);

    err += (tab == NULL);
    err += (SC_hasharr_foreach(tab, test_4_callback, NULL) < 0);

    SC_free_hasharr(tab, NULL, NULL);
   
    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_5_CALLBACK - test deleting while iterating over
 *                 - a hash table as linked list
 */

static int test_5_callback(haelem *hp, void *arg)
   {int ok, rv;
    void *v;

    rv = TRUE;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &v);
    SC_ASSERT(ok == TRUE);

    if (v != NULL)
       CFREE(v);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_5 - test deleting while iterating over a hash table as linked list */

static int test_5(void)
   {int err;
    char *type;
    hasharr *tab;

    SC_zero_space_n(1, -2);

    err = 0;

    type = CSTRSAVE("char *");

/* table size of 1 forces a linked list */
    tab = SC_make_hasharr(1, NODOC, SC_HA_NAME_KEY, 0);
    err += (tab == NULL);

    err += (SC_hasharr_install(tab, type, "foo", NULL, TRUE, TRUE) == NULL);
    err += (SC_hasharr_install(tab, type, "bar", NULL, TRUE, TRUE) == NULL);

    err += (SC_hasharr_foreach(tab, test_5_callback, NULL) < 0);

    SC_free_hasharr(tab, NULL, NULL);
    CFREE(type);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* MAIN - start here */

int main(int argc, char **argv)
   {int err;

    io_printf(STDOUT, "\n\t\t\tTest\tLeaked Memory\tTime\n");

    err = 0;

    err += run_test(1, test_1);
    err += run_test(2, test_2);
    err += run_test(3, test_3);
    err += run_test(4, test_4);
    err += run_test(5, test_5);

    io_printf(STDOUT, "\n");

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
