/*
 * MLICGS.C - a scalar version of ICCG (after Bob Tipton)
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ICMATM - matrix multiply  Y = M*X */

static void icmatm(double *a0, double *a1,
		   double *b0, double *b1, double *bm1,
                   double *x, double *y, int km, int lm)
   {int i, n;
    double *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8;
    double *x1, *x2, *x3, *x4, *x5, *x6, *x7, *x8;

/* set pointers for left/right up/down ... etc */
    x1 = x - km + 1;
    x2 = x + 1;
    x3 = x + km + 1;
    x4 = x + km;
    x5 = x + km - 1;
    x6 = x - 1;
    x7 = x - km - 1;
    x8 = x - km;
 
/* using Alex Friedman's offset convention */
    c1 = bm1 - km;
    c2 = a1;
    c3 = b1;
    c4 = b0;
    c5 = bm1 - 1;
    c6 = a1 - 1;
    c7 = b1 - km - 1;
    c8 = b0 - km;
 
/* do Y = M*X */
    n = km*lm;
 
/* do first zone */
    y[0] = a0[0]*x[0] + c2[0]*x2[0] + c3[0]*x3[0] + c4[0]*x4[0];

/* do first row of zones */
    for (i = 1; i < km - 1; i++)
        y[i] = a0[i]*x[i] + c2[i]*x2[i] + c3[i]*x3[i] + c4[i]*x4[i] +
               c5[i]*x5[i] + c6[i]*x6[i];

/* do second corner */
    i = km - 1;
    y[i] = a0[i]*x[i] + c4[i]*x4[i] + c5[i]*x5[i] + c6[i]*x6[i];

/* do special case of i = km */
    i = km;
    y[i] = a0[i]*x[i] + c1[i]*x1[i] + c2[i]*x2[i] + c3[i]*x3[i] +
           c4[i]*x4[i] + c8[i]*x8[i];

    for (i = km + 1; i < n - km - 1; i++)
        y[i] = a0[i]*x[i] + c1[i]*x1[i] + c2[i]*x2[i] + c3[i]*x3[i] +
               c4[i]*x4[i] + c5[i]*x5[i] + c6[i]*x6[i] + c7[i]*x7[i] +
               c8[i]*x8[i];

/* do special case of i = n - km - 1 */
    i = n - km - 1;
    y[i] = a0[i]*x[i] + c1[i]*x1[i] + c2[i]*x2[i] + c4[i]*x4[i] +
           c5[i]*x5[i] + c6[i]*x6[i] + c7[i]*x7[i] + c8[i]*x8[i];

/* do last row of zones */
    for (i = n - km; i < n - 1; i++)
        y[i] = a0[i]*x[i] + c1[i]*x1[i] + c2[i]*x2[i] + c6[i]*x6[i] +
               c7[i]*x7[i] + c8[i]*x8[i];

/* do last zone */
    i    = n - 1;
    y[i] = a0[i]*x[i] + c6[i]*x6[i] + c7[i]*x7[i] + c8[i]*x8[i];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ICDLDU - performs incomplete LDU decomposition of M */
 
static void icdldu(double *a0, double *a1,
		   double *b0, double *b1, double *bm1,
                   double *di, double *la1,
		   double *lb0, double *lb1, double *lbm1,
                   double *z, int km, int lm, int neq, int method)
   {int i, n;
    double *c1, *c2, *c3, *c4;
    double *l1, *l2, *l3, *l4;
    double *l5, *l6, *l7, *l8;
    double *z1, *z2, *z3, *z4;
    double *d1, *d2, *d3, *d4;
 
    n = km*lm;
 
/* if method = 1 use point jacobi method */
    if (method == 1)
       for (i = 0; i < neq; i++)
           di[i] = 1.0/(a0[i] + SMALL);

/* do ICCG(0)  method */
    else

/* initialize pointers - Alex Friedman's convention */
       {c1 = b1 - km - 1;
        c2 = b0 - km;
        c3 = bm1 - km;
        c4 = a1 - 1;
 
        l1 = lb1 - km - 1;
        l2 = lb0 - km;
        l3 = lbm1 - km;
        l4 = la1 - 1;

        l5 = la1 - km - 1;
        l6 = la1 - km;
        l7 = lb0 - km - 1;
        l8 = lbm1 - km - 1;
 
        z1 = z - km - 1;
        z2 = z - km;
        z3 = z - km + 1;
        z4 = z - 1;

        d1 = di - km - 1;
        d2 = di - km;
        d3 = di - km + 1;
        d4 = di - 1;
 
/* do first row of zones */
        z[0]  = a0[0];
        di[0] = 1.0/(a0[0] + SMALL);

        for (i = 1; i <= km; i++)
            {l4[i] = c4[i]*d4[i];
             z[i]  = a0[i] - c4[i]*c4[i]*d4[i];
             di[i] = 1.0/(z[i] + SMALL);};

/* do special i = km case */
        i     = km;
        l2[i] = c2[i]*d2[i];
        l3[i] = (c3[i] - l2[i]*l6[i]*z2[i])*d3[i];
        z[i]  = a0[i] - l2[i]*l2[i]*z2[i] -
                        l3[i]*l3[i]*z3[i];
        di[i] = 1.0/(z[i] + SMALL);

        for (i = km + 1; i < n; i++)
            {l1[i] = c1[i]*d1[i];
             l2[i] = (c2[i] - l1[i]*l5[i]*z1[i])*d2[i];
             l3[i] = (c3[i] - l2[i]*l6[i]*z2[i])*d3[i];
             l4[i] = (c4[i] - l1[i]*l7[i]*z1[i] -
                              l2[i]*l8[i]*z2[i])*d4[i];
             z[i]  = a0[i] - l1[i]*l1[i]*z1[i] -
                             l2[i]*l2[i]*z2[i] -
                             l3[i]*l3[i]*z3[i] -
                             l4[i]*l4[i]*z4[i];
             di[i] = 1.0/(z[i] + SMALL);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ICILDU - solves LDU*X = Y for X given Y */
 
static void icildu(double *di, double *la1,
		   double *lb0, double *lb1, double *lbm1,
                   double *x, double *y, int km, int lm, int neq, int method)
   {int i, n;
    double *x1, *x2, *x3, *x4;
    double *c1, *c2, *c3, *c4;

    n = km*lm;

/* if method = 1 do point-jocobi method */
    if (method == 1)
       for (i = 0; i < neq; i++)
           x[i] = di[i]*y[i];

/* do ICCG(0) method */
    else
       {
/* initialize x to y */
        for (i = 0; i < n; i++)
            x[i] = y[i];
 
/* forward sweep */

/* set up offsets */
        x1 = x - km - 1;
        x2 = x - km;
        x3 = x - km + 1;
        x4 = x - 1;
 
/* Alex Freidman's convention */
        c1 = lb1 - km - 1;
        c2 = lb0 - km;
        c3 = lbm1 - km;
        c4 = la1 - 1;
 
/* do first row */
        for (i = 1; i < km; i++)
            x[i] -= c4[i]*x4[i];
 
/* do special case of i = km */
           i = km;
           x[i] -= c2[i]*x2[i] + c3[i]*x3[i];

/* do rest */
        for (i = km + 1; i < n; i++)
            x[i] -= c1[i]*x1[i] + c2[i]*x2[i] + c3[i]*x3[i] + c4[i]*x4[i];
 
/* diagonal sweep */
        for (i = 0; i < n; i++)
            x[i] *= di[i];
 
/* backward sweep */

/* set up offsets */
        x1 = x + 1;
        x2 = x + km - 1;
        x3 = x + km;
        x4 = x + km + 1;
 
/* Alex Freidman's convention */
        c1 = la1;
        c2 = lbm1 - 1;
        c3 = lb0;
        c4 = lb1;
 
/* do last row */
        for (i = n - 2; i > n - km - 1; i--)
            x[i] -= c1[i]*x1[i];
 
/* do special case of i = n - km - 1 */
        i = n - km - 1;
        x[i] -= c1[i]*x1[i] + c2[i]*x2[i] + c3[i]*x3[i];

/* do rest of mesh */
        for (i = n - km - 2; i >= 0; i--)
           x[i] -= c1[i]*x1[i] + c2[i]*x2[i] + c3[i]*x3[i] + c4[i]*x4[i];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_GCG_S - generalized conjugate gradient */

static double _PM_gcg_s(double *a0, double *a1,
			double *b0, double *b1, double *bm1,
			double *x, double *y, double *di,
			double *la1, double *lb0,
			double *lb1, double *lbm1, double *r,
			double *q, double *p,
			int km, int lm, int neq, int method, int maxit,
			double eps)
   {int i, iter, exflag;
    double yabsum, dotprev, dotp, dotr, aa, rerr, xerr, b, pnorm;

/* form product r = M.x */
    icmatm(a0, a1, b0, b1, bm1, x, r, km, lm);
 
/* form residual r = y - M.x (this is r0) and
 * compute y norm for all relative error tests
 */

    for (yabsum = SMALL, i = 0; i < neq; i++)
        {r[i]    = y[i] - r[i];
         yabsum += fabs(y[i]);};
 
/* solve R = [(LDLt)].Q */
    icildu(di, la1, lb0, lb1, lbm1, q, r, km, lm, neq, method);
 
/* set p0 to q */

    for (i = 0; i < neq; i++)
        p[i] = q[i];
 
/* set up previous dotr product for iteration */
    dotprev = PM_dot(r, q, neq);
 
/* begin conjugate gradient iteration loop */
    rerr = 0.0;
    for (exflag = FALSE, iter = 0; iter < maxit; iter++)
 
/* compute Q = M.p */
        {icmatm(a0, a1, b0, b1, bm1, p, q, km, lm);

/* find (p, Mp) and exit if done */
         dotp = PM_dot(p, q, neq);
         if (dotp == 0.0)
            {exflag = TRUE;
             break;};
 
/* compute aa, the ratio of old dotr product and (P, M.P) */
         aa = dotprev/dotp;

/* compute x = x + aa*p and r = r - aa*M.p */
         for (i = 0; i < neq; i++)
             {x[i] += aa*p[i];
              r[i] -= aa*q[i];};

/* compute error measure */
         rerr  = 0.0;
         pnorm = 0.0;
         xerr  = 0.0;
         for (i = 0; i < neq; i++)
             {rerr  += r[i];
              pnorm += p[i]*p[i];
              xerr  += x[i]*x[i];};

         rerr = fabs(rerr)/yabsum;
         xerr = fabs(aa)*sqrt(pnorm/xerr);
 
/* test if done */
         if ((xerr < eps) && (rerr < eps))
            break;
 
/* compute new dotr product (R, [(LDLt)-1].R) */
         icildu(di, la1, lb0, lb1, lbm1, q, r, km, lm, neq, method);
         dotr = PM_dot(r, q, neq);

/* b is the ratio of old to new dotr prods - reset dotprev */
         b       = dotr/dotprev;
         dotprev = dotr;

/* p = [(LLt)-1].r + b.p */
         for (i = 0; i < neq; i++)
             p[i] = q[i] + b*p[i];};
 
/* if this point reached, no convergence after maxit passes */

/* end generalized conjugate gradient */
 
/* set parameters to the values they must have on return */
    eps = rerr + (double) iter;
    if (exflag)
       eps *= -1.0;

    return(eps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PM_ICCG_S - Solves the matrix equation M * X = Y   for X given Y and M
 *            - using the the Incomplete Cholesky - Conjugate Gradient
 *            - method (ICCG).  M is assumed to be a sparse matrix
 *            - consisting of a regular 8 point coupling pattern plus the
 *            - self coupling of the diagonal.
 *            - 
 *            - This package uses a recursive scalar algorithm appropriate
 *            - for scalar machines.  On vector machines you may want to
 *            - use _PM_iccg_v which uses cyclic reduction to vecorize ICCG,
 *            - but involves more operations.  Hence _PM_iccg_v will run
 *            - faster than _PM_iccg_s on vector machines, but slower on
 *            - scalar machines.
 *            - 
 *            - This routine calls icdldu, icmatm, icildu
 *            - 
 *            - km      - k max  k is the rapidly varying index
 *            - lm      - l max  l is the slowly  varying index
 *            - eps     - convergence criterion, L2 normalized to y
 *            - maxit   - the maximun number of c.g. iterations
 *            - a0      - the diagonal coeficient                 len = km*lm
 *            - a1      - coupling between (k, l) and (k+1, l)    len = km*lm
 *            - b0      - coupling between (k, l) and (k, l+1)    len = km*lm
 *            - b1      - coupling between (k, l) and (k+1, l+1)  len = km*lm
 *            - bm1     - coupling between (k, l+1) and (k+1, l)  len = km*lm
 *            - x       - result vector                           len = km*lm
 *            - y       - source vector                           len = km*lm
 *            - neq     - total number of equations
 *            - method  - method = 0  produces ICCG(0)
 *            -           method = 1  produces Point-Jacobi method
 *
 *    return value is actual number of iterations, iter,  plus the norm
 *    of the residual, rerr, times -1 iff (p, M.p) = 0, i.e.
 *
 *       ret = rerr + iter
 *
 *    failure of the algorithm then is indicated if ret is less than 0,
 *    or int(ret) >= maxit, or frac(ret) > eps.
 */ 
 
double _PM_iccg_s(int km, int lm, double eps, int maxit,
		  double *a0, double *a1,
		  double *b0, double *b1, double *bm1, double *x, double *y,
		  int neq, int method)
   {int nkl;
    double *di, *la1, *lb0, *lb1, *lbm1, *p, *q, *r, *w;

/* assign temporary memory */
    nkl  = km*lm;

    w    = CMAKE_N(double, 4*(neq+nkl));
    di   = w;
    la1  = di + neq;
    lb0  = la1 + nkl;
    lb1  = lb0 + nkl;
    lbm1 = lb1 + nkl;
    p    = lbm1 + nkl;
    q    = p + neq;
    r    = q + neq;
 
/* perform incomplete cholesky decomposition */
    icdldu(a0, a1, b0, b1, bm1, di, la1, lb0, lb1, lbm1, r, km, lm, 
           neq, method);
 
/* do generalized conjugate gradient */
    eps = _PM_gcg_s(a0, a1, b0, b1, bm1, x, y, di, la1, lb0, lb1, lbm1,
                    r, q, p, km, lm, neq, method, maxit, eps);
 
/* free temporary storage */
    CFREE(w);

    return(eps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
