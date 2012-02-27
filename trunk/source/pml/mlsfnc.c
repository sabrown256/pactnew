/*
 * MLSFNC.C - real special functions
 *          - following Numerical Recipes in C
 *          - and Abramowitz and Stegun
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#define CA 0.0003
#define CB 1.0e-9

#include "pml_int.h"
#include "scope_math.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CF_EVAL - evaluate a continued fraction sequence of N terms
 *            - to convergence TOL
 *            - the sequence is:
 *            -
 *            -               a[1]  a[2]  a[3]  a[4]
 *            -    f = b[0] + ----  ----  ----  ----   ...
 *            -               b[1]+ b[2]+ b[3]+ b[4]+
 *            -
 *            - A and B are N long and A[0] is ignored
 */

double PM_cf_eval(int n, double *a, double *b, double tol)
   {int it;
    double a0, a1, an, b0, b1, bn, f, fo, df;

    a0 = 1.0;
    a1 = b[0];
    b0 = 0.0;
    b1 = 1.0;

    fo = 0.0;
    f  = 0.0;

    for (it = 1; it < n; it++)
        {an = b[it]*a1 + a[it]*a0;
	 bn = b[it]*b1 + a[it]*b0;
	 if (bn != 0.0)
	    {f  = an/bn;
	     df = f - fo;
	     if (ABS(df) < tol*ABS(f))
	        break;};
	 fo = f;
	 a0 = a1;
	 b0 = b1;
	 a1 = an;
	 b1 = bn;};

    return(f);}

/*--------------------------------------------------------------------------*/

/*                       FACTORIAL AND GAMMA FUNCTIONS                      */

/*--------------------------------------------------------------------------*/

/* PM_FACTORIAL - return N! as a double */

double PM_factorial(int n)
   {int i;
    double fct;

    if (n > 20)
       fct = exp(PM_ln_gamma(n + 1.0));
    else
       {fct = 1.0;
	for (i = 1; i <= n; i++)
	    fct *= i;};

    return(fct);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_BINOMIAL - return N!/(K!*(N-K)!) as a double */

double PM_binomial(int n, int k)
   {double nf, kf, nkf, c;

    nf  = PM_factorial(n);
    kf  = PM_factorial(k);
    nkf = PM_factorial(n - k);

    c = nf/(kf*nkf);

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LN_GAMMA - compute ln(gamma(x)) */

double PM_ln_gamma(double x)
   {int j;
    double xc, tc, sc, rv;
    static double cf[7] = {2.50662827465,
			   76.18009173, -86.50532033,     24.01409822, 
	                   -1.231739516,  0.120858003e-2, -0.536382e-5};

    xc  = x - 1.0;
    tc  = xc + 5.5;
    tc -= (xc + 0.5)*log(tc);
    sc  = 1.0;
    for (j = 1; j < 7; j++)
        {xc += 1.0;
	 sc += cf[j]/xc;};

    rv = -tc + log(sc*cf[0]);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_BETA - compute beta(z, w) */

double PM_beta(double z, double w)
   {double ls, lw, lz, rv;

    lz = PM_ln_gamma(z);
    lw = PM_ln_gamma(w);
    ls = PM_ln_gamma(z + w);

    rv = exp(lz + lw - ls);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                      INCOMPLETE GAMMA FUNCTIONS                          */

/*--------------------------------------------------------------------------*/

/* PM_IGAMMA_TOLERANCE - set/get tolerance for computation of
 *                     - incomplete gamma functions
 *                     - return the old value
 *                     - if TOL <= 0 query only
 */

double PM_igamma_tolerance(double tol)
   {double rv;

    rv = _PM.igamma_tol;

    if (tol > 0.0)
       _PM.igamma_tol = tol;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_IGAMMA_SM - sum approximation to
 *               - incomplete gamma function
 *               - following Numerical Recipes algorithm
 */

static double _PM_igamma_sm(double a, double x)
   {int it, ni;
    double gs, gl, ap, ds, s;

    gs = 0.0;
    if (x > 0.0)
       {ni = 100;

	ap = a;
	ds = 1.0/a;
	s  = ds;
	for (it = 0; it < ni; it++)
	    {ap += 1.0;
	     ds *= x/ap;
	     s  += ds;
	     if (ABS(ds) < ABS(s*_PM.igamma_tol))
	        {gl = exp(-x + a*log(x) - PM_ln_gamma(a));
	         gs = s*gl;
		 break;};};};

    return(gs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_IGAMMA_CF - continued fraction approximation to
 *               - incomplete gamma function
 *               - following Numerical Recipes algorithm
 */

static double _PM_igamma_cf(double a, double x)
   {int it, ni;
    double gs, gl, a0, a1, b0, b1;
    double s, so, ds, ca, cb;

    ni = 100;

    gs = 0.0;
    so = 0.0;
    ds = 1.0;

/* set starting iteration values */
    a0 = 1.0;
    a1 = x;
    b0 = 0.0;
    b1 = 1.0;

    for (it = 1; it <= ni; it++)
        {ca = it - a;
	 cb = it*ds;
	 a0 = (a1 + a0*ca)*ds;
	 b0 = (b1 + b0*ca)*ds;
	 a1 = x*a0 + cb*a1;
	 b1 = x*b0 + cb*b1;
	 if (a1 != 0.0)
	    {ds = 1.0/a1;
	     s  = b1*ds;
	     if (ABS(s - so) < ABS(s*_PM.igamma_tol))
	        {gl = exp(-x + a*log(x) - PM_ln_gamma(a));
	         gs = s*gl;
		 break;};
	 so = s;};};

    return(gs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_IGAMMA_P - incomplete gamma function P(A, X)
 *             - using notation from Numerical Recipes in C
 *             - Note: the reversal of A and X is for ULTRA curve handling
 */

double PM_igamma_p(double x, double a)
   {double rv;

    if ((x < 0.0) || (a <= 0.0))
       rv = -HUGE;

    else if (x < (a + 1.0))
       rv = _PM_igamma_sm(a, x);

    else
       rv = 1.0 - _PM_igamma_cf(a, x);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_IGAMMA_Q - Q, the complement of incomplete gamma function P
 *             - using notation from Numerical Recipes in C
 *             - Note: the reversal of A and X is for ULTRA curve handling
 */

double PM_igamma_q(double x, double a)
   {double rv;

    if ((x < 0.0) || (a <= 0.0))
       rv = -HUGE;

    else if (x < (a + 1.0))
       rv = 1.0 - _PM_igamma_sm(a, x);

    else
       rv = _PM_igamma_cf(a, x);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            ERROR FUNCTIONS                               */

/*--------------------------------------------------------------------------*/

/* _PM_ERFC_A - rational polynomial approximation to ERFC(X)
 *            - about a factor of ~10 faster than iterative method
 */

static double _PM_erfc_a(double x)
   {double a, xa, rv;

    xa = ABS(x);
    rv = 0.0;

/* Abramowitz and Stegun 7.1.25 - fractional error < 9.25e-3 */
    if (_PM.igamma_tol > 9.25e-3)
       {a   = 1.0/(1.0 + 0.47047*xa);
	rv  = a*(0.3480242 + a*(-0.0958798 + a*0.7478556));
	rv *= exp(-x*x);}

/* rational polynomial fit to obtain fractional error < 3.0e-8
 * due to George Zimmerman
 */
    else if (_PM.igamma_tol > 3.0e-8)
       {a   = 1.0/(1.0 + xa*(2.46904246 +
			     xa*(2.62879275 +
				 xa*(1.53347072 +
				     xa*(0.502973642 +
					 xa*0.0793561593)))));

	rv  = a*(1.0 + xa*(1.34066148 +
			   xa*(0.842810482 +
			       xa*(0.283772040 +
				   xa*0.0447719189))));
	rv *= exp(-x*x);};

    if (x < 0.0)
       rv = 2.0 - rv;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ERFC - return the complementary error function ERFC(X) */

double PM_erfc(double x)
   {double rv;

    if (_PM.igamma_tol > 3.0e-8)
       rv = _PM_erfc_a(x);
    else
       rv = (x < 0.0) ? 1.0 + PM_igamma_p(x*x, 0.5) : PM_igamma_q(x*x, 0.5);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ERF - return the error function ERF(X) */

double PM_erf(double x)
   {double rv;

    if (_PM.igamma_tol > 3.0e-8)
       rv = 1.0 - _PM_erfc_a(x);
    else
       rv = (x < 0.0) ? -PM_igamma_p(x*x, 0.5) : PM_igamma_p(x*x, 0.5);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                          HARMONIC FUNCTIONS                              */

/*--------------------------------------------------------------------------*/

/* PM_LEGENDRE - return the value of the associated Legendre polynomial
 *             - Plm(x) where
 *             - M and L are integers and 0 <= M <= L
 *             - also -1.0 <= X <= 1.0
 */

double PM_legendre(double x, int l, int m)
   {int i, ll;
    double f, pl, pm, pp, sxs;

    pm = 1.0;
	
    if ((m < 0) || (m > l))
       PM_err("BAD SELECTION - PM_LEGENDRE");

    else if (ABS(x) > 1.0)
       PM_err("ARGUMENT OUT OF DOMAIN - PM_LEGENDRE");

    else
       {if (0 < m)
	   {sxs = sqrt((1.0 - x)*(1.0 + x));
	    f   = 1.0;
	    for (i = 0; i < m; i++)
	        {pm *= -f*sxs;
		 f  += 2.0;};};

	if (m < l)
	   {pp = x*(2*m+1)*pm;
	    if (l == (m+1))
	       pm = pp;
	    else
	       {pl = 0.0;                         /* make Klocworks happy */
		for (ll = (m+2); ll <= l; ll++)
		    {pl = (x*(2*ll-1)*pp - (ll+m-1)*pm)/(ll-m);
		     pm = pp;
		     pp = pl;};
		pm = pl;};};};

    return(pm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_YLM - return the value of the spherical harmonic Ylm(theta, phi)
 *        - M and L are integers and 0 <= M <= L
 *        - THETA and PHI are in degrees
 */

complex PM_Ylm(double theta, double phi, int l, int m)
   {double th, ph, ct, cp, sp, plm, nrm, yr, yi;
    complex ylm;

    th = DEG_RAD*theta;
    ph = DEG_RAD*phi;
    ct = cos(th);
    cp = cos(ph);
    sp = sin(ph);

    plm = PM_legendre(ct, l, m);

    nrm = (2*l + 1.0)*PM_factorial(l-m)/PM_factorial(l+m)/(4.0*PI);
    nrm = sqrt(nrm);

    yr = nrm*plm*cp;
    yi = nrm*plm*sp;

    ylm = PM_COMPLEX(yr, yi);

    return(ylm);}

/*--------------------------------------------------------------------------*/

/*                          ELLIPTICAL INTEGRALS                            */

/*--------------------------------------------------------------------------*/

/* PM_ELLIPTIC_INTEGRAL_G2 - compute the general elliptic integral
 *                         - of the 2nd kind
 *                         -
 *                         -                      (A + B*t^2)*dt
 *                         - int(0, X, ---------------------------------------)
 *                         -           (1 + t^2)*sqrt((1 + t^2)*(1 + (K*t)^2))
 *                         -
 */

double PM_elliptic_integral_g2(double x, double k, double a, double b)
   {int l;
    double ac, bc, c, d, e, f, g, em, h, p, kc, y, z;
    double eg;

    if (x == 0.0)
       eg = 0.0;

    else if (k != 0.0)
       {kc = k;
	ac = a;
	bc = b;
	c  = x*x;
	d  = 1.0 + c;
	p  = sqrt((1.0 + c*kc*kc)/d);
	d  = x/d;
	c  = d/(p + p);
	h  = ac;
	z  = h - bc;
	ac = 0.5*(ac + bc);
	y  = ABS(1.0/x);
	f  = 0.0;
	l  = 0;
	em = 1.0;
	kc = ABS(kc);
	while (TRUE)
	   {bc += (h*kc);
	    e   = em*kc;
	    g   = e/p;
	    d  += (f*g);
	    f   = c;
	    h   = ac;
	    p  += g;
	    c   = 0.5*(d/p + c);
	    g   = em;
	    em += kc;
	    ac  = 0.5*(bc/em + ac);
	    y  -= (e/y);
	    if (y == 0.0)
	       y = sqrt(e)*CB;
	    if (ABS(g-kc) <= CA*g)
	       break;
	    kc = sqrt(e)*2.0;
	    l *= 2;
	    if (y < 0.0)
	       l++;};

	if (y < 0.0)
	   l++;
	e = (atan(em/y) + PI*l)*ac/em;
	if (x < 0.0)
	   e = -e;

	eg = e + c*z;}

    else
       {PM_err("BAD QQC - PM_ELLIPTIC_INTEGRAL_G2");
	eg = -HUGE;};

    return(eg);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ELLIPTIC_INTEGRAL_GC - compute the general complete elliptic integral
 *                         -
 *                         -                       (A + B*t^2)*dt
 *                         - int(0, inf, ---------------------------------------)
 *                         -             (1 + P*t^2)*sqrt((1 + t^2)*(1 + (K*t)^2))
 */

double PM_elliptic_integral_gc(double k, double p, double a, double b)
   {double ac, bc, e, f, g, em, pc, q, kc;
    double gc;

    if (k == 0.0)
       {PM_err("BAD K - PM_ELLIPTIC_INTEGRAL_GC");
	gc = HUGE;}

    else
       {kc = ABS(k);
	ac = a;
	bc = b;
	pc = p;
	e  = kc;
	em = 1.0;
	if (pc > 0.0)
	   {pc  = sqrt(pc);
	    bc /= pc;}
	else
	   {f  = kc*kc;
	    q  = 1.0 - f;
	    g  = 1.0 - pc;
	    f -= pc;
	    q *= (bc - ac*pc);
	    pc = sqrt(f/g);
	    ac = (ac - bc)/g;
	    bc = -q/(g*g*pc) + ac*pc;};
	
	while (TRUE)
	   {f   = ac;
	    ac += (bc/pc);
	    g   = e/pc;
	    bc += (f*g);
	    bc += bc;
	    pc  = g + pc;
	    g   = em;
	    em += kc;
	    if (ABS(g-kc) <= g*CA)
	       break;
	    kc  = sqrt(e);
	    kc += kc;
	    e   = kc*em;};

	gc = 0.5*PI*(bc + ac*em)/(em*(em + pc));};

    return(gc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ELLIPTIC_INTEGRAL_L1 - compute the Legendre elliptic integral
 *                         - of the first kind, F(x, k)
 *                         -
 *                         -                       dt
 *                         - int(0, x, -----------------------------)
 *                         -           sqrt((1 + t^2)*(1 + (K*t)^2))
 *                         -
 */

double PM_elliptic_integral_l1(double x, double k)
   {double f;

    f = PM_elliptic_integral_g2(x, k, 1.0, 1.0);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ELLIPTIC_INTEGRAL_L2 - compute the Legendre elliptic integral
 *                         - of the second kind, E(x, k)
 *                         -
 *                         -             sqrt(1 + (K*t)^2)*dt
 *                         - int(0, X, -----------------------)
 *                         -           (1 + t^2)*sqrt(1 + t^2)
 *                         -
 */

double PM_elliptic_integral_l2(double x, double k)
   {double f;

    f = PM_elliptic_integral_g2(x, k, 1.0, k*k);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ELLIPTIC_INTEGRAL_C1 - compute the complete elliptic integral
 *                         - of the first kind, K(k)
 *                         -
 *                         -                       (1.0 + t^2)*dt
 *                         - int(0, inf, ---------------------------------------)
 *                         -             (1 + t^2)*sqrt((1 + t^2)*(1 + (K*t)^2))
 *                         -
 */

double PM_elliptic_integral_c1(double k)
   {double f;

    f = PM_elliptic_integral_gc(k, 1.0, 1.0, 1.0);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ELLIPTIC_INTEGRAL_C2 - compute the complete elliptic integral
 *                         - of the second kind, E(k)
 *                         -
 *                         -                       (1.0 + t^2)*dt
 *                         - int(0, inf, ---------------------------------------)
 *                         -             (1 + t^2)*sqrt((1 + t^2)*(1 + (K*t)^2))
 *                         -
 */

double PM_elliptic_integral_c2(double k)
   {double f;

    f = PM_elliptic_integral_gc(k, 1.0, 1.0, k*k);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ELLIPTIC_INTEGRAL_C3 - compute the complete elliptic integral
 *                         - of the third kind, Pi(n, k)
 *                         -
 *                         -                       (1 + t^2)*dt
 *                         - int(0, inf, ---------------------------------------)
 *                         -             (1 + P*t^2)*sqrt((1 + t^2)*(1 + (K*t)^2))
 */

double PM_elliptic_integral_c3(double k, double n)
   {double f;

    f = PM_elliptic_integral_gc(k, n+1.0, 1.0, 1.0);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_JACOBIAN_ELLIPTIC - worker to compute Jacobian elliptic functions
 *                       - sn(u, k), cn(u, k), and dn(u, k)
 *                       - return them via PSN, PCN, and PDN respectively
 *                       - foreach non-NULL pointer
 */

static void _PM_jacobian_elliptic(double u, double k,
				  double *psn, double *pcn, double *pdn)
   {int i, ii, l;
    double a, b, c, d, kcs, uc;
    double em[14], en[14];
    double sn, cn, dn;

    kcs = k*k;
    uc  = u;
    if (kcs != 0.0)
       {d = SMALL;
	if (kcs < 0.0)
	   {d    = 1.0 - kcs;
	    kcs /= -1.0/d;
	    d    = sqrt(d);
	    uc   *= d;};

	a  = 1.0;
	dn = 1.0;
	for (i = 1; i <= 13; i++)
	    {l = i;
	     em[i] = a;
	     kcs   = sqrt(kcs);
	     en[i] = kcs;
	     c     = 0.5*(a + kcs);
	     if (ABS(a - kcs) <= CA*a)
	        break;
	     kcs *= a;
	     a    = c;};

	uc *= c;
	sn  = sin(uc);
	cn  = cos(uc);
	if (sn != 0.0)
	   {a  = cn/sn;
	    c *= a;
	    for (ii = l; ii >= 1; ii--)
	        {b  = em[ii];
		 a *= c;
		 c *= dn;
		 dn = (en[ii] + a)/(b + a);
		 a  = c/b;};

	    a  = 1.0/sqrt(c*c + 1.0);
	    sn = (sn >= 0.0) ? a : -a;
	    cn = c*sn;};

	if (kcs < 0.0)
	   {a   = dn;
	    dn  = cn;
	    cn  = a;
	    sn /= d;};}

    else
       {cn = 1.0/cosh(uc);
	dn = cn;
	sn = tanh(uc);};

    if (pcn != NULL)
       *pcn = cn;

    if (psn != NULL)
       *psn = sn;

    if (pdn != NULL)
       *pdn = dn;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SN - compute Jacobian elliptic function sn(U, K) */

double PM_sn(double u, double k)
   {double f;

    _PM_jacobian_elliptic(u, k, &f, NULL, NULL);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CN - compute Jacobian elliptic function cn(U, K) */

double PM_cn(double u, double k)
   {double f;

    _PM_jacobian_elliptic(u, k, NULL, &f, NULL);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DN - compute Jacobian elliptic function dn(U, K) */

double PM_dn(double u, double k)
   {double f;

    _PM_jacobian_elliptic(u, k, NULL, NULL, &f);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

