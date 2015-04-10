/*
 * MLLSQ.C - least squares fitting routines
 *         - following Numerical Recipes in C
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_LSQ - find least squares fit */

PM_matrix *_PM_lsq(PM_matrix *a, PM_matrix *ay)
   {PM_matrix *at, *b, *by;

    at = PM_transpose(a);
    b  = PM_times(at, a);
    by = PM_times(at, ay);
        
/* solution is returned in BY */
    PM_solve(b, by);

/* clean up the mess */
    PM_destroy(at);
    PM_destroy(b);

    return(by);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LSQ_FIT - return least squares fitting coefficients
 *            - of N ND points X on domain DEXTR
 *            - ORDER is the order of the polynomial fit
 */

double *PM_lsq_fit(int nd, int n, double **x, double *dextr, int order)
   {int i, k, aord;
    double acc, xc;
    PM_matrix *a, *ay, *cf;
    double *rv;

    rv = NULL;

    if ((order >= 0) || (dextr[0]*dextr[1] > 0.0))
       {aord = abs(order) + 1;
	ay   = PM_create(n, 1);
	a    = PM_create(n, aord);

	for (i = 1; i <= n; i++)
	    {xc  = (order < 0) ? 1.0/x[0][i-1] : x[0][i-1];
	     acc = 1;
	     PM_element(ay, i, 1) = x[1][i-1];
	     for (k = 1; k <= aord; k++)
	         {PM_element(a, i, k) = acc;
		  acc *= xc;};};
        
	cf = _PM_lsq(a, ay);
	rv = PM_matrix_done(cf);

	PM_destroy(a);
	PM_destroy(ay);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LSQ_POLYNOMIAL - return a vector of N points
 *                   - defined as a polynomial of order ORD generated from
 *                   - the set of coefficients CF
 *                   - on the domain defined by DEXTR
 */

double **PM_lsq_polynomial(int n, int ord, double *cf, double *dextr)
   {int i, ic, nc;
    double xc, dxc, xpi, acc;
    double **p;

    p = PM_make_vectors(2, n);
 
    nc = abs(ord) + 1;

    dxc = (dextr[1] - dextr[0])/(n - 1);
    for (i = 0; i < n; i++)
        {xpi     =  dextr[0] + i*dxc;
	 xc      = (ord < 0) ? 1.0/xpi : xpi;
         p[0][i] = xpi;
         p[1][i] = 0;

         acc = 1;
         for (ic = 0; ic < nc; ic++)
             {p[1][i] += cf[ic]*acc;
              acc     *= xc;};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

