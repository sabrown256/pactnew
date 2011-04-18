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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_VANDERMONDE - solve the Vandermonde linear system
 *                - sum(i=1,N  Ai*Xi^(k-1)) = Bk for k = 1, ..., N
 *                - input is:
 *                -   X(N)
 *                -   B(N)
 *                - return A(N)
 */

double *PM_vandermonde(int n, double *x, double *b)
   {int i, j, k, ko;
    double bc, nm, dn, xc;
    double *c, *a;

    c = CMAKE_N(double, n);
    a = CMAKE_N(double, n);

    if (n == 1)
       a[0] = b[0];

    else
       {PM_set_value(c, n, 0.0);

	c[n-1] = -x[0];
	for (i = 1; i < n; i++)
	    {xc = -x[i];
	     for (j = n - i; j < n - 1; j++)
	         c[j] += xc*c[j+1];

	     c[n-1] += xc;};

/* do each subfactor */
	for (i = 0; i < n; i++)
	    {k  = n - 1;
	     xc = x[i];
	     bc = 1.0;
	     dn = 1.0;
	     nm = b[k];

/* synthetic division of subfactor */
	     for (j = 1; j < n; j++)
	         {ko = k - 1;
		  bc = c[k] + xc*bc;

/* multiplied by RHS */
		  nm += b[ko]*bc;
		  dn  = xc*dn + bc;
		  k   = ko;};

/* divide by the denominator */
	     a[i] = nm/dn;};};

    CFREE(c);

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

