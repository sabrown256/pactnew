/*
 * SCTRTS.C - N-tree test routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

static int
 ai[3] = {1, 2, 3};

static float
 bf[2] = {10.2, 13.8};

static double
 cd[4] = {3.14159, 2.7183, 1.0, 0.0};

static SC_ntree
 *bt = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CMP - compare string keys 
 *     - returns the index of the child branch
 *     - or -1 for the node itself
 */

static int cmp(void *a, void *b)
   {int i, relate;
    char *ca, *cb;

    ca = (char *) a;
    cb = (char *) b;

    i = strcmp(ca, cb);
    if (i < 0)
       relate = 0;
    else if (i > 0)
       relate = 1;
    else
       relate = -1;

    return(relate);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test the ntree routines */

int main(int c, char **v)
   {int i, n, sc;
    char msg[MAXLINE];
    SC_tree_node *p;
    char *names[] = {"foo", "bar", "baz", "zed",
		     "mrf", "qrs", "abc", "pdq",
		     "mno", "tuv", "cde", "arf"};
    void *ptrs[]  = {ai, bf, cd, NULL,
		     NULL, NULL, NULL, NULL,
		     NULL, NULL, NULL, NULL};

    sc = SC_mem_monitor(-1, 2, "sctrts", msg);

    bt = SC_make_ntree(2, cmp);

    n = sizeof(names)/sizeof(char *);

    io_printf(stdout, "Installing by key\n");
    for (i = 0; i < n; i++)
        {SC_tree_install(names[i], ptrs[i], bt);
	 io_printf(stdout, "%4d %8s %p\n", i, names[i], ptrs[i]);};

    io_printf(stdout, "Lookup by key\n");
    for (i = n-1; i >= 0; i--)
        {p = SC_tree_lookup_key(names[i], bt);
	 io_printf(stdout, "%4d %8s %p\n", i, p->key, p->val);};

    io_printf(stdout, "Lookup by index\n");
    for (i = 0; i < n; i++)
        {p = SC_tree_lookup_n(i, bt);
	 io_printf(stdout, "%4d %8s %p\n", i, p->key, p->val);};

    SC_rel_ntree(bt);

    sc = SC_mem_monitor(sc, 2, "sctrts", msg);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
