/*
 * MLVND.C - Vandermonde matrix solver
 *         - following Numerical Recipes in C
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#define TOL 1.0e-5

#define SIGN(a, b) ((b) >= 0.0 ? ABS(a) : -ABS(a))

#define INDEX(i, j, m)  i*m + j

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_VANDERMONDE - solve the Vandermonde linear system
 *                - sum(i=1,N  Wi*Xi^(k-1)) = Qk for k = 1, ..., N
 *                - input is:
 *                -   X(N)
 *                -   Q(N)
 *                - return W(N)
 */

double *PM_vandermonde(int n, double *x, double *q)
   {int i, j, k, ko;
    double bc, s, t, xc;
    double *c, *w;

    c = CMAKE_N(double, n);
    w = CMAKE_N(double, n);

    if (n == 1)
       w[0] = q[0];

    else
       {PM_set_value(c, n, 0.0);

	c[n-1] = -x[0];
	for (i = 1; i < n; i++)
	    {xc = -x[i];
	     for (j = (n-i); j < (n-1); j++)
	         c[j] += xc*c[j+1];

	     c[n-1] += xc;};

/* do each subfactor */
	for (i = 0; i < n; i++)
	    {xc = x[i];
	     t  = 1.0;
	     bc = 1.0;
	     s  = q[n-1];
	     k  = n-1;

/* synthetic division of subfactor */
	     for (j = 1; j < n; j++)
	         {ko = k - 1;
		  bc  = c[k] + xc*bc;

/* multiplied by RHS */
		  s += q[ko]*bc;
		  t  = xc*t + bc;
		  k  = ko;};

/* divide by the denominator */
	     w[i] = s/t;};};

    CFREE(c);

    return(w);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

