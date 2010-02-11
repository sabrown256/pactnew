/*
 * MLINTS.C - spline interpolation routines (N-dimensional and otherwise)
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/

/*                       SPLINE INTERPOLATION ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* _PM_SPLINE - set up cubic spline interpolation coefficients */

void _PM_spline(double *x, double *y, int n, double yp1, double ypn, double *d2y)
   {int i, k, nm;
    double p, qn, sig, un, *u;

    nm = n - 1;
    u  = FMAKE_N(double, nm, "_PM_SPLINE:u");
    if (yp1 == HUGE)
       d2y[0] = u[0] = 0.0;
    else
       {d2y[0] = -0.5;
	u[0]   = (3.0/(x[1] - x[0]))*((y[1] - y[0])/(x[1] - x[0]) - yp1);};

    for (i = 1; i < nm; i++)
        {sig    = (x[i] - x[i-1])/(x[i+1] - x[i-1]);
	 p      = sig*d2y[i-1] + 2.0;
	 d2y[i] = (sig - 1.0)/p;
	 u[i]   = (y[i+1] - y[i])/(x[i+1] - x[i]) -
	          (y[i] - y[i-1])/(x[i] - x[i-1]);
	 u[i]   = (6.0*u[i]/(x[i+1] - x[i-1]) - sig*u[i-1])/p;};

    if (ypn == HUGE)
       qn = un = 0.0;
    else
       {qn = 0.5;
	un = (3.0/(x[nm] - x[nm-1]))*
             (ypn - (y[nm] - y[nm-1])/(x[nm] - x[nm-1]));};

    d2y[nm] = (un - qn*u[nm-1])/(qn*d2y[nm-1] + 1.0);
    for (k = nm-1; k >= 0; k--)
        d2y[k] = d2y[k]*d2y[k+1] + u[k];

    SFREE(u);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CUBIC_SPLINE_INT - find the value of the function represented in
 *                     - FX and FY at the point X and return it in PY
 *                     - adapted from Numerical Recipes in C
 */

int PM_cubic_spline_int(double *fx, double *fy, double *d2y, int n,
                        double x, double *py)
   {int k0, kn, k;
    double h, b, a;

/* find the appropriate bin */
    k0 = 0;
    kn = n - 1;
    while (kn-k0 > 1)
       {k = (kn + k0) >> 1;
	if (fx[k] > x)
	   kn = k;
	else
	   k0 = k;};

    h = fx[kn] - fx[k0];
    if (h == 0.0)
       return(FALSE);

    a = (fx[kn] - x)/h;
    b = (x - fx[k0])/h;

    h   = h*h/6.0;
    *py = a*(fy[k0] + (a*a - 1.0)*d2y[k0]*h) +
          b*(fy[kn] + (b*b - 1.0)*d2y[kn]*h);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
