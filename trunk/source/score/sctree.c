/*
 * SCTREE.C - N-tree routines after Knuth Vol III, p 455
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#define BALANCED    -1

#define ROTATE(_s, _a, _r, _b)                                               \
   {(_s)->branch[_a] = (_r)->branch[_b];                                     \
    (_s)->rank[_a]   = (_r)->rank[_b];                                       \
    (_r)->branch[_b] = (_s);                                                 \
    (_r)->rank[_b]   = 1 + (_s)->rank[_a] + (_s)->rank[_b];}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PRN - print the node key */

static void _prn(SC_tree_node *pn, int depth, int idp)
   {char c, *ca;

    c = 0;

    switch (pn->balance)
        {case -1:
	      c = 'b';
	      break;
	 case 0:
	      c = 'l';
	      break;
	 case 1:
	      c = 'r';
	      break;};

    ca = (char *) pn->key;
    io_printf(stdout, "%p: (%d %c %c %d %d) %s\n",
	      pn, depth, c,
	      idp ? '>' : '<',
	      pn->rank[0], pn->rank[1], ca);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRN - print the tree nodes
 *      - debugging aid
 */

void dprn(SC_ntree *bt)
   {

    io_printf(stdout, "\nTree:\n");
    SC_map_tree(bt, _prn);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAKE_TREE_NODE - allocate and initialize a new tree node */

static SC_tree_node *_SC_make_tree_node(void *k, void *v, int nd)
   {int i;
    int *pr;
    SC_tree_node *pn, **pb;

    pn = FMAKE(SC_tree_node, "_SC_MAKE_TREE_NODE:pn");
    pr = FMAKE_N(int, nd, "_SC_MAKE_TREE_NODE:branch");
    pb = FMAKE_N(SC_tree_node *, nd, "_SC_MAKE_TREE_NODE:branch");
    for (i = 0; i < nd; i++)
        {pr[i] = 0;
	 pb[i] = NULL;};

    pn->nd      = nd;
    pn->balance = BALANCED;
    pn->key     = k;
    pn->val     = v;
    pn->rank    = pr;
    pn->branch  = pb;

    return(pn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REL_TREE_NODE - recursively free the tree nodes starting at PN */

static void SC_rel_tree_node(SC_tree_node *pn)
   {int id, nd;
    SC_tree_node *p, **branch;

    nd     = pn->nd;
    branch = pn->branch;
    for (id = 0; id < nd; id++)
        {p = branch[id];
         if (p != NULL)
            SC_rel_tree_node(p);};

    SFREE(pn->rank);
    SFREE(pn->branch);
    SFREE(pn);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TREE_CMP - default node comparison predicate
 *              - returns the index of the child branch
 *              - or -1 for the node itself
 */

static int _SC_tree_cmp(void *a, void *b)
   {int relate;

    if (a < b)
       relate = 0;
    else if (a > b)
       relate = 1;
    else
       relate = -1;

    return(relate);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAKE_NTREE - allocate and initialize a new N-tree */

SC_ntree *SC_make_ntree(int nd, PFIntBin cmp)
   {SC_ntree *pt;

    pt = FMAKE(SC_ntree, "SC_MAKE_NTREE:pt");

    pt->nd                 = nd;
    pt->n_entry            = 0;
    pt->head               = _SC_make_tree_node(NULL, NULL, nd);
    pt->compare            = (cmp == NULL) ? _SC_tree_cmp : cmp;

    return(pt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_REL_NTREE - release the nodes of the tree and the tree itself
 *              - the values in the tree are not released
 */

void SC_rel_ntree(SC_ntree *tree)
   {int nd;
    SC_tree_node **branch;

    nd     = tree->head->nd;
    branch = tree->head->branch;

    SC_rel_tree_node(branch[nd-1]);

    SFREE(tree->head->branch);
    SFREE(tree->head->rank);
    SFREE(tree->head);
    SFREE(tree);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_PERMUTE_TERMINUS - permute a->b->c into a<-c->b if conditions
 *                      - are correct
 *                      - this is a significant special case that could
 *                      - otherwise be handled by a rotation
 */

static SC_tree_node *_SC_permute_terminus(SC_tree_node *t,
					  SC_tree_node *s, int a,
					  SC_tree_node *r, int b)
   {SC_tree_node *p;

    p = r->branch[b];
    if ((p->branch[a] == NULL) && (p->branch[b] == NULL))
       {p->branch[a] = r;
	p->branch[b] = s;
	s->branch[a] = NULL;
	r->branch[b] = NULL;
	s->rank[a]   = 0;
	r->rank[b]   = 0;
	p->rank[0]   = 1 + r->rank[a];
	p->rank[1]   = 1 + s->rank[b];

	r->balance = BALANCED;
	s->balance = BALANCED;

	if (s == t->branch[1])
	   t->branch[1] = p;
	else
	   t->branch[0] = p;}

    else
       p = s;

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_TREE_REBALANCE - rebalance the subtree S of TREE
 *                    - having just inserted Q
 */

static void _SC_tree_rebalance(SC_tree_node *q, SC_tree_node *t,
                               SC_tree_node *s, SC_ntree *tree)
   {int a, b, ib, nd;
    int dr, ds, dp;
    SC_tree_node *p, *r, **branch;
    void *k;
    PFIntBin compare;

    nd      = tree->head->nd;
    branch  = tree->head->branch;
    compare = tree->compare;
    k       = q->key;

    a = compare(k, s->key);
    a = (a == -1) ? nd-1 : a;
    b = 1 - a;

/* adjust balance factors */
    p = s->branch[a];
    r = p;

    while (p != q)
       {ib = compare(k, p->key);
        if (ib != -1)
           {p->balance = ib;
            p          = p->branch[ib];}
        else
           break;};

/* balancing act */
    if (s->balance == BALANCED)
       {s->balance = a;
        tree->head->branch[0]++;}

    else if (s->balance == b)
       s->balance = BALANCED;

    else if ((s->branch[b] == NULL) && (r->branch[a] == NULL))
       p = _SC_permute_terminus(t, s, a, r, b);

    else if (s->balance == a)

/* single rotation */
       {if (r->balance == b)
	   {p = r;
	    ROTATE(s, a, r, b);

            r->balance = BALANCED;
            s->balance = BALANCED;}

        else if (r->balance == a)

/* double rotation */
           {p = r->branch[b];
	    if (p != NULL)
	       {ROTATE(r, b, p, a);
		ROTATE(s, a, p, b);

	        dr = r->rank[1] - r->rank[0];
	        ds = s->rank[1] - s->rank[0];
	        dp = p->rank[1] - p->rank[0];

		if (dr == 0)
		   r->balance = BALANCED;
		else
		   r->balance = (dr > 0);

		if (ds == 0)
		   s->balance = BALANCED;
		else
		   s->balance = (ds > 0);

		if (dp == 0)
		   p->balance = BALANCED;
		else
		   p->balance = (dp > 0);
/*
		if (p->balance == a)
		   {s->balance = b;
		    r->balance = BALANCED;}

		else if (p->balance == BALANCED)
		   {s->balance = BALANCED;
		    r->balance = BALANCED;}

		else if (p->balance == b)
		   {s->balance = BALANCED;
		    r->balance = a;};

		p->balance = BALANCED;
 */
};};

/* finish */
	if (p != NULL)
	   {if (s == t->branch[1])
	       t->branch[1] = p;
	    else
	       t->branch[0] = p;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TREE_INSTALL - install VAL in the TREE under the key K
 *                 - return a pointer to VAL
 */

void *SC_tree_install(void *k, void *val, SC_ntree *tree)
   {int ib, ok, nd;
    SC_tree_node *t, *s, *p, *q, **branch;
    PFIntBin compare;

    t       = tree->head;
    compare = tree->compare;

    nd     = t->nd;
    branch = t->branch;

    s = branch[nd-1];
    if (s == NULL)
       branch[nd-1] = _SC_make_tree_node(k, val, nd);
       
    else
       {p  = s;
	ok = TRUE;
	while (ok)
	   {ib = compare(k, p->key);
	    if (ib >= 0)
	       {p->rank[ib]++;
		q = p->branch[ib];
		if (q == NULL)
		   {q             = _SC_make_tree_node(k, val, nd);
		    p->branch[ib] = q;
		    _SC_tree_rebalance(q, t, s, tree);
		    tree->n_entry++;
		    ok = FALSE;}

		else
		   {if (q->balance != BALANCED)
		       {t = p;
			s = q;};
		    p = q;};}

	    else
	       val = p->val;};};

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TREE_LOOKUP_KEY - lookup an entry under key K in tree TREE
 *                    - return the associated value if found and
 *                    - NULL otherwise
 */

SC_tree_node *SC_tree_lookup_key(void *k, SC_ntree *tree)
   {int ib, ok, nd;
    SC_tree_node *p, *q, **branch;
    PFIntBin compare;

    nd      = tree->head->nd;
    branch  = tree->head->branch;
    compare = tree->compare;
    p       = branch[nd-1];

    ok = TRUE;
    while (ok)
       {ib = compare(k, p->key);
        switch (ib)
           {case 0:
            case 1:
                 q = p->branch[ib];
                 if (q == NULL)
                    ok = FALSE;
                 else
                    p = q;
                 break;

            default:
                 ok = FALSE;
                 break;};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_TREE_LOOKUP_N - lookup the Nth element in TREE
 *                  - return the associated value if found and
 *                  - NULL otherwise
 */

SC_tree_node *SC_tree_lookup_n(int n, SC_ntree *tree)
   {int i, m, nd;
    SC_tree_node *p, **branch;

    if ((n < 0) || (n > tree->n_entry))
       return(NULL);

    nd     = tree->head->nd;
    branch = tree->head->branch;

    p = branch[nd-1];
    i = 0;
    m = n;
    while (TRUE)
       {i = p->rank[0];
        if ((m < i) && (p->branch[0] != NULL))
           p = p->branch[0];
        else if ((m > i) && (p->branch[1] != NULL))
           {m -= (p->rank[0] + 1);
            p  = p->branch[1];}
        else
           break;};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_MAP_TREE - recursively walk the tree and apply the function
 *              - given to each node
 *              - hand the DEPTH and index relative to parent IDP
 *              - down the the function
 */

static void _SC_map_tree(SC_tree_node *pn, int depth, int idp,
			 void (*f)(SC_tree_node *pn, int depth, int idp))
   {int id, nd;
    SC_tree_node *p, **branch;

    nd     = pn->nd;
    branch = pn->branch;
    for (id = 0; id < nd; id++)
        {p = branch[id];
         if (p != NULL)
            _SC_map_tree(p, depth+1, id, f);};

    (*f)(pn, depth, idp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_MAP_TREE - recursively walk the tree and apply the function
 *             - given to each node
 */

void SC_map_tree(SC_ntree *tr,
		 void (*f)(SC_tree_node *pn, int depth, int idp))
   {int nd;
    SC_tree_node **branch;

    nd     = tr->head->nd;
    branch = tr->head->branch;

    _SC_map_tree(branch[nd-1], 0, -1, f);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

