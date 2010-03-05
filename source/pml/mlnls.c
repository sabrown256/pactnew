/*
 * MLNLS.C - non-linear Newton solver for PML
 *         - courtesy of Jeff Painter
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SETTOL - set the tolerances */

static void _PM_settol(int neqns, double *tol, double *x,
		       double atol, double rtol)
   {int i;

    for (i = 0; i < neqns; i++)
        tol[i] = 1.0 / (atol + rtol*ABS(x[i]));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_WVNORM - weighted vector norm */

static double _PM_wvnorm(int neqns, double *x, double *tol)
   {int i;
    double val, t;

    val = 0.0;
    for (i = 0; i < neqns; i++)
        {t    = tol[i]*x[i];
         val += t*t;};

    return(sqrt(val));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NEWTONDL - a simple nonlinear solver designed to be conveniently called
 *             - by Alpal-generated codes.  This uses Newton's method to
 *             - solve f(Y)=0, with Y an array of length NEQNS.  On input,
 *             - NEQNS and the maximum number of iterations MAXITER need to
 *             - be set along with ATOL and RTOL which are the absolute and
 *             - relative error tolerance parameters (good values are 1.0e-4
 *             - and 1.0e-3 respectively).  Initial y values come in through Y
 *             - and the solution values are returned in Y.
 *             - One subroutine must be passed in through LINSOLV.
 *             - LINSOLV(x, y, iter) computes the Jacobian and the right-hand
 *             - side in the equation:
 *             -
 *             -     J.dy = -f(y1)     where J = (df/dy) at y1
 *             -     y1 = y[iter-1]
 *             -     dy = y[iter] - y1
 *             -
 *             - and returns the solution of the equation.  Iter is the Newton
 *             - iteration index. 
 *             -
 *             - Returns 0 iff we converged.
 *             - The convergence criterion is simple minded:
 *             -
 *             -     Norm(dy)/(atol + rtol*abs(y2[i])) < 1
 *             -
 *             - where, dy is the correction to the solution found in
 *             - the last iteration.
 */

double PM_newtondl(int neqns, double *y, double *dy, double *tol,
		   int maxiter, double atol, double rtol,
                   void (*linsolv)(int neqns, double *dy, double *y, int iter, void *arg),
		   void *arg)
   {int i, n, iter, rv;
    double err;

    iter = 0;
    err  = 2.0;
    n    = neqns*sizeof(double);

    while ((err > 1.0) && (iter++ < maxiter))

/* initial iterate */
       {memset(dy, 0, n);

/* solve J.dy = -f(y) */
        linsolv(neqns, dy, y, iter, arg);

/* update y */
        for (i = 0; i < neqns; i++)
            y[i] += dy[i];

        _PM_settol(neqns, tol, y, atol, rtol);

        err = _PM_wvnorm(neqns, dy, tol);};

    rv = (err <= 1.0) ? 0.0 : err;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NEWTONUL - a simple nonlinear solver designed to be conveniently called
 *             - by Alpal-generated codes.  This uses Newton's method to
 *             - solve f(Y)=0, with Y an array of length NEQNS.  On input,
 *             - NEQNS and the maximum number of iterations MAXITER need to
 *             - be set along with ATOL and RTOL which are the absolute and
 *             - relative error tolerance parameters (good values are 1.0e-4
 *             - and 1.0e-3 respectively).  Initial y values come in through Y2
 *             - and the solution values are returned in Y2.
 *             - One subroutine must be passed in through LINSOLV.
 *             - LINSOLV(x, y, iter) computes the Jacobian and the right-hand
 *             - side in the equation:
 *             -
 *             -     J.y2 = J.y1 - f(y1)   where J = (df/dy) at y1
 *             -     y1 = y[iter-1]
 *             -     y2 = y[iter]
 *             -
 *             - and returns the solution of the equation.  Iter is the Newton
 *             - iteration index. 
 *             -
 *             - Returns 0 iff we converged.
 *             - The convergence criterion is simple minded:
 *             -
 *             -     Norm(delta - y1[i])/(atol + rtol*abs(y2[i])) < 1
 *             -
 *             - where, delta - y1 is the correction to the solution found in
 *             - the last iteration.
 */

double PM_newtonul(int neqns, double *y2, double *y1, double *tol, int maxiter,
		   double atol, double rtol,
                   void (*linsolv)(int neqns, double *dy, double *y, int iter, void *arg),
                   void *arg)
   {int i, iter, n;
    double err, rv;

    iter = 0;
    err  = 2.0;
    n    = neqns*sizeof(double);

    while ((err > 1.0) && (iter++ < maxiter))

/* initial iterate */
       {memset(y1, 0, n);

/* solve J.y1 = -f(y2) */
        linsolv(neqns, y1, y2, iter, arg);

/* update y2 and set y1 to the correction made in this iteration
 * use tol for temporary storage to allow vectorization
 */
        for (i = 0; i < neqns; i++)
            tol[i] = y1[i] - y2[i];

        for (i = 0; i < neqns; i++)
            y2[i] = y1[i];

        for (i = 0; i < neqns; i++)
            y1[i] = tol[i];

        _PM_settol(neqns, tol, y2, atol, rtol);

        err = _PM_wvnorm(neqns, y1, tol);};

    rv = (err <= 1.0) ? 0.0 : err;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                       MONOTONE NON-LINEAR SOLVER                         */

/*--------------------------------------------------------------------------*/

/* _PM_COMPUTE_JACOBIAN - compute an effective Jacobian for the next
 *                      - non-linear iteration
 */

static double _PM_compute_jacobian(double xm, double x1, double xp,
				   double ym, double y1, double yp,
				   int meth)
   {double da, db, dc, b, c, dsc, xs, x1s;
    double ep, em, up, um, uep, uem, ie;
    double jsn, rat;

    da = 0.0;
    db = 0.0;
    dc = 0.0;
    uep = 0.0;
    uem = 0.0;

    ep = xp - x1;
    em = xm - x1;
    up = yp - y1;
    um = ym - y1;
	
    if ((ep == 0.0) || (em == 0.0) || (em == ep))
       meth = 0;

    else
       {uep = up/ep;
	uem = um/em;};
	
    rat = (um == 0.0) ? 1.0 : -up/um;

/* use the closest two points for the Jacobian */
    if (((1.0 < rat) || (up == 0.0)) && (um != 0.0))
       jsn = em/um;
    else
       jsn = ep/up;

/* if a quadratic extrapolation is requested instead of the
 * canonical linear one
 * make an effective linear Jacobian
 */
    if (meth)
       {ie = 1.0/(ep - em);

	switch (meth)

/* compute 2nd order Taylor expansion */
	   {case 1 :
	         da = (uem - uep)*ie;
		 db = 2.0/jsn;
		 dc = 2.0*y1;
		 break;

/* fit quadratic to the three points */
	    case 2 :
		 x1s = x1*x1;
		 b   = (ep*uem - em*uep)*ie;

		 da = (uep - uem)*ie;
		 db = b - 2*da*x1;
		 dc = y1 + da*x1s - b*x1;

		 break;};

/* proceed if quadratic coefficient is non-zero */
	if (da != 0.0)
	   {b   = 0.5*db/da;
	    c   = dc/da;
	    dsc = b*b - c;

/* proceed if roots are real */
	    if (dsc >= 0.0)
	       {dsc = sqrt(dsc);
		xs  = -b - dsc;
		if ((xs < xm) || (xp < xs))
		   xs = -b + dsc;

		jsn = -(xs - x1)/y1;};};};

    return(jsn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NLS_MONOTONE - use Newton's method to solve y(x) = 0
 *                 - in the interval (XM, XP)
 *                 - y(x) is assumed to be monotone
 *                 - a higher level routine could be used to 
 *                 - bracket crossings and call this routine
 *                 - in case the function FNC is expensive to evaluate
 *                 - and you have some prior info you can also
 *                 - supply (YM, YP) which should be (y(XM), y(XP))
 *                 - NOTE: YM <= 0.0 and YP >= 0.0
 *                 -
 *                 - The convergence criterion is simple minded:
 *                 -
 *                 -     errx = ABS(dx) < XTOL
 *                 - or
 *                 -     erry = ABS(y[i]) < YTOL
 *                 -
 *                 - Input:
 *                 -
 *                 -   PX        the initial guess for x
 *                 -   PY        the known value of y
 *                 -   PITX      the maximum number of iterations allowed
 *                 -   (XM, XP)  the x interval to confine the search
 *                 -   (YM, YP)  the y interval to confine the search
 *                 -             give (-HUGE, HUGE) to have them evaled
 *                 -   XTOL      the convergence tolerance for x
 *                 -   YTOL      the convergence tolerance for y
 *                 -   METH      the iteration method:
 *                 -               0  linear Newton iteration
 *                 -               1  quadratic Newton iteration
 *                 -                  using 2nd order Taylor expansion
 *                 -               2  quadratic fits of 3 points
 *                 -   ASYMM     the limit the asymmetry between the
 *                 -             left and right sides of zero crossing
 *                 -             before attempting to subdivide the
 *                 -             large side iteratively to accelerate
 *                 -             convergence
 *                 -   FNC       the function y
 *                 -   ARG       additional data required by FNC
 *                 -             bundled up and pointed to by ARG
 *                 -
 *                 - Return values:
 *                 -
 *                 -   PX   the best value for x
 *                 -   PY   the value y(PX)
 *                 -   PITX the number of iterations taken
 *                 -        if positive there was convergence on x or y
 *                 -        if negative the dx = 0 condition occured
 *                 -   PERR if convergence occured on y or dx = 0 take erry
 *                 -        if convergence occured on x take -errx
 */

void PM_nls_monotone(double *px, double *py, int *pitx, double *perr,
		     double xm, double xp, double ym, double yp,
		     double ytol, double xtol, int meth, double asymm,
		     double (*fnc)(double x, void *data), void *arg)
   {int it, itx;
    double err, yerr, xerr, jsn;
    double xt, dx, dxa, x1, y1;
    double rat, x2, y2, iasymm, fltol;

    yerr = 0.0;
    xerr = 0.0;

    if (_PM.precision == -1.0)
       _PM.precision = PM_machine_precision();

    x1  = *px;
    y1  = *py;
    itx = *pitx;

    iasymm = 1.0/asymm;
    fltol  = 10.0*_PM.precision;
    if (ABS(x1) > 1.0)
       fltol *= ABS(x1);

    fltol = max(fltol, xtol);

    if (ym <= -HUGE)
       ym = (*fnc)(xm, arg);

    if (yp >= HUGE)
       yp = (*fnc)(xp, arg);

/* if we happen to have nailed it on the plus side we're done */
    if ((yp < ytol) && (yp == y1))
       {y1   = yp;
	x1   = xp;
	it   = 0;
        yerr = yp;}

/* if we happen to have nailed it on the minus side we're done */
    else if ((-ym < ytol) && (ym == y1))
       {y1   = ym;
	x1   = xm;
	it   = 0;
	yerr = -ym;}

/* iterate it we must */
    else
       {dx = _PM.mlt*(xp - xm);
	dx = (x1 == xm) ? dx : -dx;

/* do a first pass with the linear method 
 * with luck this gets a second good point on the other
 * side of 0 from x1 - convergence goes ultra fast then
 */
	x1 += dx;
	y1  = (*fnc)(x1, arg);
	jsn = _PM_compute_jacobian(xm, x1, xp, ym, y1, yp, 0);

	dx = -y1*jsn;
	xt = x1 + dx;
	if ((xt <= xm) || (xp <= xt))
	   dx = (yp*xm - ym*xp)/(yp - ym) - x1;

	if (ABS(dx) < _PM.precision)
	   {it   = -1;
	    itx  = 0;
	    yerr = ABS(y1);};

	for (it = 1; it < itx; it++)
	    {x1 += dx;
	     y1 = (*fnc)(x1, arg);

	     yerr = ABS(y1);
	     xerr = ABS(dx);
	     if ((yerr < ytol) || (xerr < fltol))
	        break;

/* handle some special cases in which it is possible
 * to spend hundreds of iterations instead of
 * the relative few required to get the algorithm
 * into an optimal convergence path
 */

/* Case #1
 * handle the case the high and low sides are too different
 * in magnitude since this situation slows convergence
 * considerably
 */
	     rat = (ym == y1) ? 1.0 : (yp - y1)/(y1 - ym);

/* if ym is closer to zero by a lot than yp
 * iteratively chop the way too high side down
 */
	     if ((rat > asymm) && (y1 < 0.0))
                {for (; it < itx; it++)
		     {x2 = x1 + 0.1*(xp - x1);
		      dx = x2 - x1;
		      if (ABS(dx) < fltol)
			 break;

		      y2 = (*fnc)(x2, arg);
		      if (y2 > 0.0)
		         {xp = x2;
			  yp = y2;}
		      else
		         {x1 = x2;
			  y1 = y2;
			  break;};};}

/* yp is closer to zero by a lot than ym
 * iteratively chop the way too low side down
 */
	     else if ((rat < iasymm) && (y1 > 0.0))
                {for (; it < itx; it++)
		     {x2 = x1 - 0.1*(x1 - xm);
		      dx = x2 - x1;
		      if (ABS(dx) < fltol)
			 break;

		      y2 = (*fnc)(x2, arg);
		      if (y2 < 0.0)
		         {xm = x2;
			  ym = y2;}
		      else
		         {x1 = x2;
			  y1 = y2;
			  break;};};};
	       
/* Case #2
 * handle non-monotone regions if that's what fnc is
 */

/* if we have a dip/flat on the low side move past it */
	     if ((ym >= y1) && (xm != x1))
                {for (; it < itx; it++)
		     {x2 = 0.5*(x1 + xp);
		      dx = x2 - x1;
		      if (ABS(dx) < fltol)
			 break;

		      y2 = (*fnc)(x2, arg);
		      if (y2 <= ym)
		         {x1 = x2;
			  y1 = y2;}
		      else if (y2 > 0.0)
		         {xp = x2;
			  yp = y2;}
		      else
		         {x1 = x2;
			  y1 = y2;
			  break;};};}

/* if we have a bump/flat on the high side move past it */
	     else if ((yp <= y1) && (xp != x1))
                {for (; it < itx; it++)
		     {x2 = 0.5*(x1 + xm);
		      dx = x2 - x1;
		      if (ABS(dx) < fltol)
			 break;

		      y2 = (*fnc)(x2, arg);
		      if (y2 >= yp)
		         {x1 = x2;
			  y1 = y2;}
		      else if (y2 < 0.0)
		         {xm = x2;
			  ym = y2;}
		      else
		         {x1 = x2;
			  y1 = y2;
			  break;};};}

	     yerr = ABS(y1);
	     xerr = ABS(dx);
	     if ((yerr < ytol) || (xerr < fltol))
	        break;

	     jsn = _PM_compute_jacobian(xm, x1, xp, ym, y1, yp, meth);

/* improve the bracketed interval on the top or bottom */
	     if (y1 < 0.0)
	        {xm = x1;
		 ym = y1;}
	     else
	        {xp = x1;
		 yp = y1;};

/* compute the change in x for the next iteration */
	     dx = -y1*jsn;
	     xt = x1 + dx;

/* if the slope takes us outside of the known interval
 * interpolate between the interval limits
 */
	     if ((xt <= xm) || (xp <= xt))
	        dx = (yp*xm - ym*xp)/(yp - ym) - x1;

/* if we aren't changing stop */
	     dxa = 2.0*dx/(xp + xm);
	     if (ABS(dxa) < _PM.precision)
	        {it = -it;
		 break;};};};

/* if we got out on y convergence or dx == 0
 * take err to be positive
 */
    if ((yerr < ytol) || (it < 0))
       err = yerr;

/* if we got out on the convergence of x take err to be negative */
    else
       err = -xerr;

    *py   = y1;
    *px   = x1;
    *perr = err;
    *pitx = it;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

