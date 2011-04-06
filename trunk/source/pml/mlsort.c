/*
 * MLSORT.C - sorting routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COMP - compare the values and return their relative order */

static int _PM_comp(double x1, double x2)
   {int rv;

    if (x1 < x2)
       rv = -1;
    else if (x1 > x2)
       rv = 1;
    else
       rv = 0;
        
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_EXCH - exchange two (x, y) pairs */

static void _PM_exch(double *x1, double *y1, double *x2, double *y2)
   {

    SC_SWAP_VALUE(double, *x1, *x2);
    SC_SWAP_VALUE(double, *y1, *y2);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_VAL_SORT - sort arrays containing points by x value */

void PM_val_sort(int n, double *xp, double *yp)
   {int gap, i, j;

    for (gap = n/2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i-gap; j >= 0; j -= gap)
                {if (_PM_comp(xp[j], xp[j+gap]) <= 0)
                    break;
                 _PM_exch(xp+j, yp+j, xp+j+gap, yp+j+gap);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_T_SORT - topological sort routine as described in Knuth,
 *           - "The Art of Computer Programming", Vol. 1, p 260-263
 *           - input is an array of indexes which is 2*n_dep long
 *           - containing n_dep pairs specifying the topology in terms
 *           - of relations, j < k, i.e. "j precedes k"
 *           - also n_dep the number of pairs and n_pts the number of
 *           - points to be ordered
 *           - the routine returns a pointer to a newly allocated list
 *           - of ordered indices if successful
 *           - NULL is returned on failure (loops present)
 */

int *PM_t_sort(int *in, int n_dep, int n_pts, int *ord)
   {int i, n, *pin, *out, *pout, *q;
    int r, f, ind, dep;
    sort_link *link, *nln;

    link = CMAKE_N(sort_link, n_dep+n_pts+1);
    q    = CMAKE_N(int, n_pts+1);

/* map the partial ordering into a structured list to do the sort */
    nln = link + n_pts + 1;
    pin = in;
    for (i = 0; i < n_dep; i++)
        {ind = *pin++;
         dep = *pin++;
         nln->count = dep;
         nln->next  = link[ind].next;
         link[dep].count++;
         link[ind].next = nln++;};

/* initialize the output queue */
    r = 0;
    for (i = 1; i <= n_pts; i++)
        if (link[i].count == 0)
           {q[r] = i;
            r    = i;};

/* determine what to do about output */
    if (ord == NULL)
       out = pout = CMAKE_N(int, n_pts);
    else
       out = pout = ord;

/* do the sort */
    n   = n_pts;
    f   = q[0];
    while (f != 0)
       {*pout++ = f;
        n--;

/* remove relations of the form "f < k" for some k of the system */
        for (nln = link[f].next; nln != NULL; nln = nln->next)
            if (--link[nln->count].count == 0)
               {q[r] = nln->count;
                r    = nln->count;};

        f = q[f];};

    CFREE(link);
    CFREE(q);

/* if n is non-zero there was a loop in the topology */
    if (n != 0)
       {CFREE(out);
        out = NULL;};

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SWAP - swap elements i and j in arrays v and ind */

void _PM_swap(double *v, int *ind, int i, int j)
   {

    SC_SWAP_VALUE(double, v[i], v[j]);
    SC_SWAP_VALUE(int, ind[i], ind[j]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_Q_SORT - quicksort, taken from "The C Programming Language",
 *            - 2nd Edition, by Kernighan and Ritchie, pp 87-88
 */

static void _PM_q_sort(double *v, int *ind, int il, int ir)
   {int i, in;

    if (il < ir)
       {_PM_swap(v, ind, il, (il+ir)/2);

	in = il;

	for (i = il+1; i <= ir; i++)
	    {if (v[i] < v[il])
	        _PM_swap(v, ind, ++in, i);};

	_PM_swap(v, ind, il, in);
	_PM_q_sort(v, ind, il, in-1);
	_PM_q_sort(v, ind, in+1, ir);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PM_Q_SORT - Sort an array of indexes on z; 
 *           - input is z, an array of doubles,
 *           -        ind, array of indexes
 *           -          n, number of entries
 *           - output is array z sorted and
 *           -           array ind sorted
 */

void PM_q_sort(double *z, int *ind, int n)
   {

    _PM_q_sort(z, ind, 0, n-1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

