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

/*                       FACTORIAL AND GAMMA FUNCTIONS                      */

/*--------------------------------------------------------------------------*/

/* PM_FACTORIAL - return N! as a double */

double PM_factorial(int n)
   {int i;
    double fct;

    fct = 1.0;
    for (i = 1; i <= n; i++)
        fct *= i;

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
    static double cf[6] = {76.18009173, -86.50532033,     24.01409822, 
	                   -1.231739516,  0.120858003e-2, -0.536382e-5};

    xc  = x - 1.0;
    tc  = xc + 5.5;
    tc -= (xc + 0.5)*log(tc);
    sc  = 1.0;
    for (j = 0; j <= 5; j++)
        {xc += 1.0;
	 sc += cf[j]/xc;};

    rv = -tc + log(2.50662827465*sc);

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

