/*
 * MLICCG.C - the ICCG solver for the matrix package
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MATMUL - computes w = M.x */
 
static void matmul(double *a0, double *a1, double *b0, double *b1,
		   double *bm1, double *x, double *w, int km, int lm)
   {int i, klm;
    double a10, bm10, b10;

    a10     = a1[-1];
    bm10    = bm1[-1];
    b10     = b1[-1];
    a1[-1]  = 0.0;
    bm1[-1] = 0.0;
    b1[-1]  = 0.0;
 
    klm = km*lm;

    w[0] = a0[0]*x[0] + a1[0]*x[1];
    for (i = 1; i < klm; i++)
        w[i] = a1[i-1]*x[i-1] + a0[i]*x[i] + a1[i]*x[i+1];

    klm = km*(lm-1);
    for (i = 0; i < klm; i++)
        w[i] += bm1[i-1]*x[km+i-1] + b0[i]*x[km+i] + b1[i]*x[km+i+1];
 
    w[km] += b0[0]*x[0] + bm1[0]*x[1];
    for (i = 1; i < klm; i++)
        w[km+i] += b1[i-1]*x[i-1] + b0[i]*x[i] + bm1[i]*x[i+1];
 
    a1[-1] = a10;
    bm1[-1] = bm10;
    b1[-1] = b10;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORWARD - solves L*w = y */

static void forward(double *d, double *a1, double *y, double *w,
		    int nblocks, int step, int km)
   {int i, j, lastbl;

/* pointer to beginning of last block to be processed */
    lastbl = (nblocks-1)*step;
 
/* do first row of every other block */
    for (j = 0; j <= lastbl; j += step)
        w[j] = d[j] * y[j];
 
/* do ith row of every other block */
    for (i = 1; i < km; i++)
        for (j = 0; j <= lastbl; j += step)
            w[i+j] = d[i+j] * (y[i+j] - a1[i+j-1]*w[i+j-1]);
 
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* BACKWARD - solves DLt*x = w */

static void backward(double *d, double *a1, double *w, double *x,
		     int nblocks, int step, int km)
   {int i, j, jmax, lastbl;

/* pointer to beginning of last block to be processed */
    lastbl = (nblocks-1)*step;
 
/* do last row of every other block */
    jmax = lastbl + km;
    for (j = km-1; j < jmax; j += step)
        x[j] = w[j];
 
/* do ith row of every other block, progressing backwards */
    for (i = km-2; i >= 0; i--)
        for (j = 0; j <= lastbl; j += step)
            x[i+j] = w[i+j] - a1[i+j]*d[i+j]*x[i+j+1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ALTWS - compute next level of w's, the wtildes, for the solve */
 
static void altws(double *wk, double *b0km1, double *b1km1, double *bm1km1,
		  double *tkm1, double *b0k, double *b1k, double *bm1k,
		  double *tkp1, double *wtilde, int nblkslo,
		  int nblkshi, int km)
   {int i, j, jj, kmt, lastnblo, lastnbhi;
    double b1km10, bm1k0;

/* pointers to beginning of last new block */
    kmt      = 2*km;
    lastnblo = (nblkslo-1)*km;
    lastnbhi = (nblkshi-1)*km;

    b1km10 = b1km1[-1];
    bm1k0  = bm1k[-1];

    b1km1[-1] = 0.0;
    bm1k[-1]  = 0.0;
 
/* do ith row of every other block */
    for (i = 0; i < km; i++)
        {for (j = 0, jj = 0; jj <= lastnblo; jj += km, j += kmt)
             wtilde[i+jj] = wk[i+j] - (b1km1[i+j-1]*tkm1[i+j-1] +
                                       b0km1[i+j]*tkm1[i+j] +
                                       bm1km1[i+j]*tkm1[i+j+1]);

         for (j = 0, jj = 0; jj <= lastnbhi; jj += km, j += kmt)
             wtilde[i+jj] -= (bm1k[i+j-1]*tkp1[i+j-1] +
                              b0k[i+j]*tkp1[i+j] + b1k[i+j]*tkp1[i+j+1]);};

    b1km1[-1] = b1km10;
    bm1k[-1] = bm1k0;
 
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MOVEXE - moves block k/2 of xtilde (=x at level kq+1) into block k
 *        - of xk (=x at level kq), for k even
 */
 
static void movexe(double *xk, double *xtilde, int nblocks, int km)
   {int i, j, jj, kmt, lastnewb;

/* pointer to beginning of last new block */
    kmt      = 2*km;
    lastnewb = (nblocks-1)*km;
 
/* do ith row of every other block */
    for (i = 0; i < km; i++)
        for (j = 0, jj = 0; jj <= lastnewb; jj += km, j += kmt)
            xk[i+j] = xtilde[i+jj];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORMT1 - compute intermediate term in backward solve */
 
static void formt1(double *b0k, double *b1k, double *bm1k, double *xkp1,
		   double *b0km1, double *b1km1, double *bm1km1,
		   double *xkm1, double *q, int nblocks, int km)
   {int i, j, lastbl, kmt;
    double b1km10, bm1k0;

/* pointer to beginning of last block */
    kmt    = 2*km;
    lastbl = (nblocks-1) * kmt;
 
/* do part valid for all blocks, ith row of every other block,
 * then do part not valid for first block
 */
    b1km10 = b1km1[-1];
    bm1k0  = bm1k[-1];

    b1km1[-1] = 0.0;
    bm1k[-1]  = 0.0;
 
    for (i = 0; i < km; i++)
        {for (j = 0; j < lastbl; j += kmt)
             q[i+j] = bm1k[i+j-1]*xkp1[i+j-1] + b0k[i+j]*xkp1[i+j] +
                      b1k[i+j]*xkp1[i+j+1];

         for (j = kmt; j < lastbl; j += kmt)
             q[i+j] += b1km1[i+j-1]*xkm1[i+j-1] +
                       b0km1[i+j]*xkm1[i+j] + bm1km1[i+j]*xkm1[i+j+1];};
 
    b1km1[-1] = b1km10;
    bm1k[-1] = bm1k0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TSSOLVE - solves LDLt*x = y
 *         - by (first) L*w = y
 *         - (then) DLt*x = w
 *         - note that w and x use same storage (c0).  they are
 *         - kept distinct here for purposes of clarity.
 */

static void tssolve(double *x, double *y, double *d, double *a1,
		    double *b0, double *b1, double *bm1, double *w, double *q,
		    int ks, int km, int lm)
   {int nob[15], neb[15], ibo[15], ibe[15], inl[15];
    int i, kq, nevenb, inextlev, levlen, noddb, ibegino, ibegine;
    int j, jmax, kmt, neq;

    kmt = 2*km;
    neq = km*lm;

/* begin forward sweep  L*w = y */
 
/* set w at kq = 0 level to y */
    for (i = 0; i < neq; i++)
        w[i] = y[i];
 
/* set parameters for first pass through forward sweep loop */
    nevenb   = lm;
    inextlev = 0;

/* begin loop */
    for (kq = 0; kq <= ks; kq++)
 
/* set parameters for this level */
        {levlen   = km*nevenb;
         noddb    = (nevenb + 1)/2;
         nevenb   = nevenb/2;
         ibegino  = inextlev;
         ibegine  = ibegino + km;
         inextlev = ibegino + levlen;
 
/* save parameters for this level for future use
 * in backward sweep (they are tricky to regenerate when moving backwards).
 * kershaw, instead, generates the neven's using a circular
 * shift right and mask, so the process is reversible.
 */
         nob[kq] = noddb;
         neb[kq] = nevenb;
         ibo[kq] = ibegino;
         ibe[kq] = ibegine;
         inl[kq] = inextlev;
 
/* perform forward solve on odd blocks, L*w = w */
         forward(d+ibegino, a1+ibegino, w+ibegino, w+ibegino, noddb, kmt, km);
 
/* generate (L)-t (D)-1 * w terms for odd blocks, i.e. DLt*q = w */
         backward(d+ibegino, a1+ibegino, w+ibegino, q, noddb, kmt, km);
 
/* create next level of w's */
         altws(w+ibegine, b0+ibegino, b1+ibegino, 
               bm1+ibegino, q,
               b0+ibegine, b1+ibegine, bm1+ibegine, 
               q+kmt, 
               w+inextlev, nevenb, noddb-1, km);};
 
/* do kq = ks+1 forward solve */
    forward(d+inextlev, a1+inextlev, w+inextlev, w+inextlev, nevenb, km, km);
 
/* end forward sweep
 * begin backward sweep
 */
 
/* do kq = ks+1 backward solve */
    backward(d+inextlev, a1+inextlev, w+inextlev, x+inextlev, nevenb, km, km);
 
/* begin loop */
    for (kq = ks; kq >= 0; kq--)
 
/* set parameters for this level */
        {noddb = nob[kq];
         nevenb = neb[kq];
         ibegino = ibo[kq];
         ibegine = ibe[kq];
         inextlev = inl[kq];
 
/* move even blocks of x to lower level */
         movexe(x+ibegine, x+inextlev, nevenb, km);
 
/* evaluate b(k)t*x(k+1) + b(k-1)*x(k-1) for odd k, store in q */
         formt1(b0+ibegino, b1+ibegino, bm1+ibegino, x+ibegine, 
                b0+ibegine-kmt, b1+ibegine-kmt, 
                bm1+ibegine-kmt, x+ibegine-kmt, 
                q, noddb, km);
 
/* perform forward solve on this, keep in q */
         forward(d+ibegino, a1+ibegino, q, q, noddb, kmt, km);
 
/* and subtract from w, still in q */
         jmax = (noddb-1)*kmt;
         for (i = 0; i < km; i++)
             for (j = 0; j <= jmax; j += kmt)
                 q[i+j] = w[ibegino+i+j] - q[i+j];

/* finally, perform backward solve into x */
         backward(d+ibegino, a1+ibegino, q, x+ibegino, noddb, kmt, km);};
 
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_GCG - do generalized conjugate gradient solution phase */

static double _PM_gcg(double *a0, double *a1, double *b0, double *b1, double *bm1,
		      double *x, double *y, double *w, double *d, double *r,
		      double *q, double *p, int km, int lm, int neq,
		      int ks, int maxit, double eps)
   {int i, iter, exflag;
    double yabsum, dotr, dotp, dotprev, aa, pnorm, rerr, xerr, b;
 
/* form product w = M.x */
    matmul(a0, a1, b0, b1, bm1, x, w, km, lm);
 
/* form residual r = y - M.x (this is r0) and
 * compute y norm for all relative error tests
 */
    for (yabsum = SMALL, i = 0; i < neq; i++)
        {r[i]    = y[i] - w[i];
         yabsum += ABS(y[i]);};

/* compute w = [(LLt)-1].r */
    tssolve(w, r, d, a1, b0, b1, bm1, w, q, ks, km, lm);
 
/* and set p0 to this */
    for (i = 0; i < neq; i++)
        p[i] = w[i];
 
/* set up previous dotr product for iteration */
    dotprev = PM_dot(r, w, neq);
 
/* begin conjugate gradient iteration loop */
    rerr = 0.0;
    for (exflag = FALSE, iter = 0; iter < maxit; iter++)
 
/* compute q = M.p
 * tssolve uses q array, but we'll be done with it by then
 */
        {matmul(a0, a1, b0, b1, bm1, p, q, km, lm);
 
/* find (p, mp) and exit if done. */
         dotp = PM_dot(p, q, neq);
         if (dotp == 0.0)
            {exflag = TRUE;
             break;};
 
/* compute aa, the ratio of old dotr product and (p, M.p) */
         aa = dotprev/dotp;
 
/* compute x = x + aa*p and r = r - aa*mp */
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

         rerr = ABS(rerr) / yabsum;
         xerr = ABS(aa) * sqrt(pnorm/xerr);
 
/* test if done */
         if ((xerr < eps) && (rerr < eps))
            break;
 
/* compute new dotr product (r, [(LLt)-1].r) */
         tssolve(w, r, d, a1, b0, b1, bm1, w, q, ks, km, lm);
         dotr = PM_dot(r, w, neq);
 
/* b is the ratio of old to new dotr prods - reset dotrprev */
         b = dotr/dotprev;
         dotprev = dotr;
 
/* p = [(LLt)-1].r + b.p */
         for (i = 0; i < neq; i++)
             p[i] = w[i] + b*p[i];};
 
/* if this exflag is TRUE, no convergence after maxit passes
 * set parameters to the values they must have on return
 */
    eps = rerr + (double) iter;
    if (exflag)
       eps *= -1.0;

    return(eps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TSDECOMP - note that the d's overwrite the odd a0's at each level
 *          - negative pivots are eliminated using absolute value.
 *          - no check is made for small denominators.
 */

static void tsdecomp(double *a0, double *a1, double *d, int nblocks,
		     int step, int km)
   {int i, j, lastbl;

/* pointer to beginning of last block to be processed */
    lastbl = (nblocks - 1) * step;
 
/* do first row of every step/km block (compute d's) */
    for (j = 0; j <= lastbl; j += step)
        d[j] = 1.0 / (ABS(a0[j]) + SMALL);
 
/* do ith row of every step/km block (compute d's) */
    for (i = 1; i < km; i++)
        for (j = 0; j <= lastbl; j += step)
            d[i+j] = 1.0 /
                     (ABS(a0[i+j]-a1[i+j-1]*a1[i+j-1]*d[i+j-1]) + SMALL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GENCEES - generate the arrays c0 and cm1 */

static void gencees(double *b0, double *b1, double *bm1,
		    double *a1, double *d, double *c0, double *cm1,
		    int nblocks, int km)
   {int i, j, lastbl, kmt;

    kmt = km*2;

/* test the number of blocks before entering loop */
    if (nblocks <= 0)
       return;
 
/* pointer to beginning of last block to be processed */
    lastbl = (nblocks - 1) * kmt;
 
/* do first row of every other block (compute c0's, c1's) */
    for (j = 0; j <= lastbl; j += kmt)
        {c0[j]  = b0[j];
         cm1[j] = bm1[j]- a1[j]*d[j]*c0[j];};
 
/* do ith row of every other block (compute c0's, c1's) */
    for (i = 1; i < km; i++)
        for (j = 0; j <= lastbl; j += kmt)
            {c0[i+j]  = b0[i+j] - a1[i+j-1]*d[i+j-1]*b1[i+j-1];
             cm1[i+j] = bm1[i+j] - a1[i+j]*d[i+j]*c0[i+j];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ALTEVENS - */

static void altevens(double *a0k, double *a1k, double *c0km1,
		     double *c1km1, double *cm1km1, double *dkm1,
		     double *c0k, double *c1k, double *cm1k,
		     double *dkp1, double *a0tild, double *a1tild,
		     int nbkm1, int nbkkp1, int km)
   {int i, j, jj, lastnewb, lastaltb, kmt;

    kmt = 2*km;

/* test before entering loop */
    if (nbkm1 <= 0)
       return;
 
/* pointer to beginning of last new block */
    lastnewb = (nbkm1-1)*km;

/* first set atilde to:  a - (k-1 term) */
 
/* do first row of every other block */
    for (j = 0, jj = 0; jj <= lastnewb; jj += km, j += kmt)
        {a0tild[jj] = a0k[j] - (c0km1[j]*c0km1[j]*dkm1[j]
                              + cm1km1[j]*cm1km1[j]*dkm1[j]);
         a1tild[jj] = a1k[j] - (c0km1[j]*c1km1[j]*dkm1[j]
                              + cm1km1[j]*c0km1[j+1]*dkm1[j+1]);};
 
/* do ith row of every other block */
    for (i = 1; i < km; i++)
        for (j = 0, jj = 0; jj <= lastnewb; jj += km, j += kmt)
            {a0tild[i+jj] = a0k[i+j] -
                            (c0km1[i+j]*c0km1[i+j]*dkm1[i+j] + 
                             cm1km1[i+j]*cm1km1[i+j]*dkm1[i+j+1] + 
                             c1km1[i+j-1]*c1km1[i+j-1]*dkm1[i+j-1]);
             a1tild[i+jj] = a1k[i+j] -
                            (c0km1[i+j]*c1km1[i+j]*dkm1[i+j] + 
                             cm1km1[i+j]*c0km1[i+j+1]*dkm1[i+j+1]);};
 
/* subtract off k/k+1 term to form true atilde.
 * this is done in a separate loop to avoid "overreach"
 * problems.  since the c pointers do not move, we can't
 * count on the last block of c's being zero.
 */
 
/* test before entering loop */
    if (nbkkp1 <= 0)
       return;
 
/* pointer to beginning of last new block to alter */
    lastaltb = (nbkkp1-1)*km;
 
/* do first row of every other block */
    for (j = 0, jj = 0; jj <= lastaltb; jj += km, j += kmt)
        {a0tild[jj] -= (c0k[j]*c0k[j]*dkp1[j] + c1k[j]*c1k[j]*dkp1[j+1]);
         a1tild[jj] -= (c0k[j]*cm1k[j]*dkp1[j] + c1k[j]*c0k[j+1]*dkp1[j+1]);};
 
/* do ith row of every other block */
    for (i = 1; i < km; i++)
        for (j = 0, jj = 0; jj <= lastaltb; jj += km, j += kmt)
            {a0tild[i+jj] -= (c0k[i+j]*c0k[i+j]*dkp1[i+j] +
                              c1k[i+j]*c1k[i+j]*dkp1[i+j+1] +
                              cm1k[i+j-1]*cm1k[i+j-1]*dkp1[i+j-1]);
             a1tild[i+jj] -= (c0k[i+j]*cm1k[i+j]*dkp1[i+j] +
                              c1k[i+j]*c0k[i+j+1]*dkp1[i+j+1]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GENBEES - generate the arrays b0tild, b1tild, and bm1tild */

static void genbees(double *c0kp1, double *c1kp1, double *cm1kp1,
		    double *c0k, double *c1k, double *cm1k,
		    double *dkp1, double *b0tild, double *b1tild,
		    double *bm1tild, int nblocks, int km)
   {int i, j, jj, lastnewb, kmt;

    kmt = 2*km;

/* test before entering loop */
    if (nblocks <= 0)
       return;

/* pointer to beginning of last new block */
    lastnewb = (nblocks-1)*km;
 
/* do first row of every other block */
    for (j = 0, jj = 0; jj <= lastnewb; jj += km, j += kmt)
        {b0tild[jj]  = - c0kp1[j]*c0k[j]*dkp1[j] - cm1kp1[j]*c1k[j]*dkp1[j+1];
         b1tild[jj]  = - c1kp1[j]*c0k[j]*dkp1[j] - c0kp1[j+1]*c1k[j]*dkp1[j+1];
         bm1tild[jj] = - c0kp1[j]*cm1k[j]*dkp1[j] -
                         cm1kp1[j]*c0k[j+1]*dkp1[j+1];};
 
/* do ith row of every block */
    for (i = 1; i < km; i++)
        for (j = 0, jj = 0; jj <= lastnewb; jj += km, j += kmt)
            {b0tild[i+jj]  = - c0kp1[i+j]*c0k[i+j]*dkp1[i+j] -
                               cm1kp1[i+j]*c1k[i+j]*dkp1[i+j+1] -
                               c1kp1[i+j-1]*cm1k[i+j-1]*dkp1[i+j-1];
             b1tild[i+jj]  = - c1kp1[i+j]*c0k[i+j]*dkp1[i+j] -
                               c0kp1[i+j+1]*c1k[i+j]*dkp1[i+j+1];
             bm1tild[i+jj] = - c0kp1[i+j]*cm1k[i+j]*dkp1[i+j] - 
                               cm1kp1[i+j]*c0k[i+j+1]*dkp1[i+j+1];};
 
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ICD_CR - do incomplete Cholesky decomposition by cyclic reduction */

static int _PM_icd_cr(double *a0, double *a1, double *b0, double *b1,
		      double *bm1, double *di, double *c1, double *cm1,
		      double *d, int km, int lm, int ks)
   {int j, kp, kmt;
    int nevenb, inextlev, kq, levlen, noddb, ibegino, ibegine;
    int noddcb, nevencb, nbb;

    kmt = 2*km;

/* compute kp (Kershaw's p, the maximum possible level of
 * cyclic reduction) - this is used as a check on input ks
 */
    for (kp = 0, j = 2; (j < lm) && (kp++ < 1000); j *= 2);
    ks = min(ks, kp-1);
 
/* set parameters for first pass through decomposition loop */
    nevenb   = lm;
    inextlev = 0;

/* begin incomplete cholesky decomposition
 *
 * note that the c's are overwritten, level by level,
 * and so the pointers to di, c1, cm1 do not move
 */
    for (kq = 0; kq <= ks; kq++) 

/* set parameters for this level */
        {levlen   = km*nevenb;
         noddb    = (nevenb + 1)/2;
         nevenb   = nevenb/2;
         ibegino  = inextlev;
         ibegine  = ibegino + km;
         inextlev = ibegino + levlen;
 
/* decompose upper left corner (calculate d's) */
         tsdecomp(a0+ibegino, a1+ibegino, d+ibegino, noddb, kmt, km);
 
/* generate odd c's */
         noddcb = nevenb;
         gencees(b0+ibegino, b1+ibegino, bm1+ibegino,
                 a1+ibegino, d+ibegino, di, cm1, noddcb, km);
 
/* generate even c's */
         nevencb = noddb - 1;
         gencees(b0+ibegine, bm1+ibegine, b1+ibegine, 
                 a1+ibegino+kmt, d+ibegino+kmt, di+km, 
                 c1+km, nevencb, km);
 
/* modify even diagonal arrays, lower right corner (calculate
 * atilde's) - note that c1odd = b1odd, cm1even = bm1even
 */
         altevens(a0+ibegine, a1+ibegine, di,
                  b1+ibegino, cm1, d+ibegino, 
                  di+km, c1+km, bm1+ibegine, 
                  d+ibegino+kmt, 
                  a0+inextlev, a1+inextlev, noddcb, nevencb, km);
 
/* calculate off-diagonal elements, lower right corner (btilde's) */
         nbb = nevenb - 1;
         genbees(di+kmt, b1+ibegino+kmt, cm1+kmt, 
                 di+km, c1+km, bm1+ibegine, 
                 d+ibegino+kmt, 
                 b0+inextlev, b1+inextlev, bm1+inextlev, nbb, km);};
 
/* do final level of tridiagonal sym. decomposition */
    tsdecomp(a0+inextlev, a1+inextlev, d+inextlev, nevenb, km, km);

    return(ks);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DOT - take the inner product of two vectors
 *        - this is outside of the matrix package style presently
 */

double PM_dot(double *x, double *y, int n)
   {int i;
    double dot;

    for (dot = 0.0, i = 0; i < n; i++)
        dot += x[i]*y[i];

    return(dot);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ICCG_V - the vectorizable version of ICCG */

double _PM_iccg_v(int km, int lm, double eps, int ks, int maxit,
		  double *a0, double *a1, double *b0, double *b1, double *bm1,
		  double *x, double *y)
   {int j, numaux, neq, nw;
    double *a0s, *d, *p, *r, *q;
    double *di, *c1, *cm1;

/* set common variables */
    neq = km*lm;
 
/* set pointers (note that many arrays share storage) */
    nw = 6*(neq + 1);
    di = CMAKE_N(double, nw);
    PM_array_set(di, nw, 0.0);

    c1  = di + neq;
    r   = c1 + neq;
    p   = r + neq;
    q   = p + neq;
    a0s = q + neq;
    d   = a0;
    cm1 = c1;
 
/* zero out work spaces */
    for (numaux = 2*neq, j = neq; j < numaux; j++)
        {a0[j]  = 0.0;
         a1[j]  = 0.0;
         b0[j]  = 0.0;
         b1[j]  = 0.0;
         bm1[j] = 0.0;
         x[j]   = 0.0;};
 
/* copy a0 into a0s to preserve it for matrix multiplies later */
    for (j = 0; j < neq; j++)
        a0s[j] = a0[j];
 
/* do incomplete Cholesky decomposition by cyclic reduction */
    ks = _PM_icd_cr(a0, a1, b0, b1, bm1, di, c1, cm1, d, km, lm, ks);

/* do generalized conjugate gradient solution */
    eps = _PM_gcg(a0s, a1, b0, b1, bm1, x, y, di, d,
                  r, q, p, km, lm, neq, ks, maxit, eps);
 
/* restore a0 array for user convenience */
    for (j = 0; j < neq; j++)
        a0[j] = a0s[j];
 
/* free temporary storage */
    CFREE(di);

    return(eps);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ICCG - incomplete cholesky - conjugate gradient method
 *         - written by alex friedman, llnl, 415-422-0827
 *         - rewritten in C by Stewart Brown, llnl, 415-423-4889
 *         - using algorithms of david kershaw, donald wolitzer.
 *
 *         - input arrays are all twice as long as needed to specify
 *         - the problem; the second half is workspace.  that is, the
 *         - input arrays should all have dimension at least
 *         - 2*km*lm. these arrays are: a0,a1,b0,b1,bm1,x,y.
 *
 *         - the C version allocates its own work space and releases it
 *
 *         - The matrix to be solved is symmetric, and has the structure:
 *
 *      a0
 *        1
 *
 *      a1   a0
 *        1    2
 *
 *           a1   a0
 *             2    3
 *
 *          :
 *          :
 *          :
 *          :
 *
 *                              a1    a0
 *                                km-2  km-1
 *
 *                                    a1    a0
 *                                      km-1  km
 *
 *      b0   bm1                                  a0
 *        1     1                                  km+1
 *
 *      b1    b0   bm1                            a1    a0
 *        1     2     2                             km+1  km+2
 *
 *            b1    b0   bm1                            a1    a0
 *              2     3     3                             km+2  km+3
 *
 *                   :                                        :
 *                   :                                        :
 *                   :                                        :
 *
 *    Note that elements a1(sub)km etc. are zero due to the block structure.
 *
 *    the correspondence with one convention is:
 *      a0 <--> a
 *      a1 <--> b
 *      b0 <--> g
 *      b1 <--> d
 *      bm1<--> e
 *    note that the first physical element of bm1 is bm1(1), in
 *    contrast with the one convention where it is bm1(2).
 *
 *    scalar input variables are:
 *    km - the "short" dimension of the physical system
 *         (k is the rapidly varying index of the mesh array).
 *    lm - the "long" dimension of the physical system
 *         (l is the slowly varying index of the mesh array).
 *    eps  - convergence criterion, l2 normalized to y vector.
 *           on return, the actual accuracy achieved.
 *    ks   - last complete level of cyclic reduction desired
 *           in loops (one more level is done outside loops).
 *           the minimum possible value of ks is 0.
 *           the maximum possible value of ks is kp-1, where
 *           kp is the highest power of 2 with 2**kp  <=  lm.
 *           a reasonable choice is ks=4 for "most" problems.
 *           ks <= 14 at present due to dimension (15)
 *           in routine tssolve, allows lm of 32k (big enough).
 *    maxit - the maximum number of c.g. iterations desired.
 *            on return, the number of iterations used.
 *
 *
 *    return value is actual number of iterations, iter,  plus the norm
 *    of the residual, rerr, times -1 iff (p, Mp) = 0, i.e.
 *
 *       ret = rerr + iter
 *
 *    failure of the algorithm then is indicated if ret is less than 0,
 *    or int(ret) >= maxit, or frac(ret) > eps.
 */
 
double PM_iccg(int km, int lm, double eps, int ks, int maxit,
	       double *a0, double *a1, double *b0, double *b1, double *bm1,
	       double *x, double *y)
   {double rv;

#ifdef CRAY
    rv = _PM_iccg_v(km, lm, eps, ks, maxit, a0, a1, b0, b1, bm1, x, y);
#else
    rv = _PM_iccg_s(km, lm, eps, maxit, a0, a1, b0, b1, bm1, x, y, km*lm, 0);
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

