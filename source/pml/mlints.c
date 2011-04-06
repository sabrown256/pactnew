/*
 * MLINTS.C - spline interpolation routines (N-dimensional and otherwise)
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

/*--------------------------------------------------------------------------*/

/*                       SPLINE INTERPOLATION ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* PM_COMPUTE_SPLINES - set up cubic spline interpolation coefficients
 *                    - for the N point function Y(X)
 */

double *PM_compute_splines(double *x, double *y, int n,
			   double dyl, double dyr)
   {int i, k, nm;
    double p, qn, sig, un, *u;
    double *dy;

    dy = CMAKE_N(double, n);

    nm = n - 1;
    u  = CMAKE_N(double, nm);
    if (dyl == HUGE)
       dy[0] = u[0] = 0.0;
    else
       {dy[0] = -0.5;
	u[0]  = (3.0/(x[1] - x[0]))*((y[1] - y[0])/(x[1] - x[0]) - dyl);};

    for (i = 1; i < nm; i++)
        {sig   = (x[i] - x[i-1])/(x[i+1] - x[i-1]);
	 p     = sig*dy[i-1] + 2.0;
	 dy[i] = (sig - 1.0)/p;
	 u[i]  = (y[i+1] - y[i])/(x[i+1] - x[i]) -
	         (y[i] - y[i-1])/(x[i] - x[i-1]);
	 u[i]  = (6.0*u[i]/(x[i+1] - x[i-1]) - sig*u[i-1])/p;};

    if (dyr == HUGE)
       qn = un = 0.0;
    else
       {qn = 0.5;
	un = (3.0/(x[nm] - x[nm-1]))*
             (dyr - (y[nm] - y[nm-1])/(x[nm] - x[nm-1]));};

    dy[nm] = (un - qn*u[nm-1])/(qn*dy[nm-1] + 1.0);
    for (k = nm-1; k >= 0; k--)
        dy[k] = dy[k]*dy[k+1] + u[k];

    CFREE(u);

    return(dy);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CUBIC_SPLINE_INT - find the value of the function represented in
 *                     - F(X) at the point XC and return it
 *                     - adapted from Numerical Recipes in C
 */

double PM_cubic_spline_int(double *x, double *f, double *df, int n, double xc)
   {int k0, kn, k;
    double h, b, a, fc;

/* find the appropriate bin */
    k0 = 0;
    kn = n - 1;
    while (kn-k0 > 1)
       {k = (kn + k0) >> 1;
	if (x[k] > xc)
	   kn = k;
	else
	   k0 = k;};

    h = x[kn] - x[k0];
    if (h == 0.0)
       fc = -HUGE;

    else
       {a = (x[kn] - xc)/h;
	b = (xc - x[k0])/h;

	h  = h*h/6.0;
	fc = a*(f[k0] + (a*a - 1.0)*df[k0]*h) +
	     b*(f[kn] + (b*b - 1.0)*df[kn]*h);};

    return(fc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LINEAR_INT - find the value of the function represented in
 *               - F(X) at the point XC and return it
 */

double PM_linear_int(double *x, double *f, int n, double xc)
   {int k0, kn, k;
    double fc;

/* find the appropriate bin */
    k0 = 0;
    kn = n - 1;
    while (kn-k0 > 1)
       {k = (kn + k0) >> 1;
	if (x[k] > xc)
	   kn = k;
	else
	   k0 = k;};

    PM_interp(fc, xc, x[k0], f[k0], x[kn], f[kn]);

    return(fc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
