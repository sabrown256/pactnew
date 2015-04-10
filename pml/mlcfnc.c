/*
 * MLCFNC.C - complex math functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"
#include "scope_math.h"

#define MEXP_CMP(_cx, _sx, _ey, _eyi, _c)                                   \
    {double _x, _y;                                                         \
     _x   = creal(_c);                                                      \
     _y   = cimag(_c);                                                      \
     _ey  = exp(_y);                                                        \
     _eyi = 1.0/_ey;                                                        \
     _cx  = cos(_x);                                                        \
     _sx  = sin(_x);}

#define MEXP_CMPH(_ex, _exi, _cy, _sy, _c)                                  \
    {double _x, _y;                                                         \
     _x   = creal(_c);                                                      \
     _y   = cimag(_c);                                                      \
     _ex  = exp(_x);                                                        \
     _exi = 1.0/_ex;                                                        \
     _cy  = cos(_y);                                                        \
     _sy  = sin(_y);}

#define MEXP(_rc, _c)                                                       \
    {xl = creal(_c);                                                        \
     yl = cimag(_c);                                                        \
     al = exp(xl);                                                          \
     cl = cos(yl);                                                          \
     sl = sin(yl);                                                          \
     _rc = CMPLX(al*cl, al*sl);}

#define MLOG(_rc, _c)                                                       \
    {x   = creal(_c);                                                       \
     y   = cimag(_c);                                                       \
     a   = log(HYPOT(x, y));                                                \
     b   = atan2(y, x);                                                     \
     _rc = CMPLX(a, b);}

#define MDIV(_rc, _ac, _bc)                                                 \
    {double ar, ai, br, bi, abr, abi, u, brovu, biovu, d;                   \
     ar  = creal(_ac);                                                      \
     ai  = cimag(_ac);                                                      \
     br  = creal(_bc);                                                      \
     bi  = cimag(_bc);                                                      \
     abr = fabs(br);                                                        \
     abi = fabs(bi);                                                        \
     u   = max(abr,abi);                                                    \
     brovu = br/u;                                                          \
     biovu = bi/u;                                                          \
     d     = 1.0/(br*brovu + bi*biovu);                                     \
     _rc   = CMPLX(d*(ar*brovu + ai*biovu), d*(ai*brovu - ar*biovu));}

/* the implementation here has superior accuracy to
 * several vendor libraries - so the no-brainer is
 * to continue to use these
# define USE_C99_FUNCTIONS
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MAKE_COMPLEX - return a complex number with real part RP and
 *                  - imaginary par IP
 */

complex _PM_make_complex(double rp, double ip)
   {complex rv;

    rv  = rp;
    rv += ip*I;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_PLUS_CC - add complex A and B */

complex PM_plus_cc(complex b, complex c)
   {complex rv;

    rv = b + c;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MINUS_CC - subtract complex B from A */

complex PM_minus_cc(complex b, complex c)
   {complex rv;

    rv = b - c;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_TIMES_CC - multiply add complex A and B */

complex PM_times_cc(complex b, complex c)
   {complex rv;

    rv = b*c;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DIVIDE_CC - divide complex A by B */

complex PM_divide_cc(complex b, complex c)
   {complex rv;

    rv = b/c;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DISTANCE_CC - return the distance between complex A and B */

double PM_distance_cc(complex a, complex b)
   {double d;

    d = sqrt(PM_cabs(a - b));

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CHORNER - evaluate polynomial using Horner's rule
 *            -   y = Sum(Ci*X^i, mn <= i <= mx)
 */

complex PM_chorner(complex x, double *c, int mn, int mx)
   {int i, in, iz, ix;
    complex r, rn, r0, rp, xi;

    in = min(mn, mx);
    ix = max(mn, mx);

    iz = max(in, 0);

/* grab the constant coefficient */
    if ((in <= iz) && (iz <= ix))
       r0 = CMPLX(c[iz-in], 0.0);
    else
       r0 = Czero;

/* do the positive powers */
    rp = Czero;
    for (i = ix; iz < i; i--)
        rp = x*(c[i-in] + rp);

/* do the negative powers */
    if (PM_cequal(x, Czero) && (in < 0))
       rn = CMHUGE;

    else
       {xi = PM_crecip(x);
	rn = Czero;
	for (i = in; i < iz; i++)
	    rn = xi*(c[i-in] + rn);};

    r = rn + rp + r0;

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CRANDOM - return unit random complex */

complex PM_crandom(complex c)
   {double tp, x, y, cx, sx;

    tp = 2.0*PI;

    y = PM_random(0.0);
    x = tp*y;

    cx = cos(x);
    sx = sin(x);

    c = CMPLX(cx, sx);

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CABS - complex abs function */

double PM_cabs(complex x)
   {double modulus;

#ifdef USE_C99_FUNCTIONS

    modulus = cabs(x);

#else

    double xr, xi, ratio;

    xr = fabs(creal(x));
    xi = fabs(cimag(x));

    if ( ( xr == 0.0 ) && ( xi == 0.0 ) )
       {modulus = 0.0;}
    else if ( xr >= xi )
       {ratio   = cimag(x)/creal(x);
	modulus = xr*sqrt( 1.0 + ratio*ratio );}
    else
       {ratio   = creal(x)/cimag(x);
	modulus = xi*sqrt( 1.0 + ratio*ratio );}

#endif

    return(modulus);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CCONJUGATE - complex conjugate function */

complex PM_cconjugate(complex c)
   {

#ifdef USE_C99_FUNCTIONS

    c = conj(c);

#else

    c = CMPLX(creal(c), -1.0*cimag(c));

#endif

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CARG - complex arg function */

double PM_carg(complex c)
   {double r;

#ifdef USE_C99_FUNCTIONS

    r = carg(c);

#else

    double x, y;

    x = creal(c);
    y = cimag(c);

    r = PM_atan(x, y);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CRECIP - complex reciprocal function */

complex PM_crecip(complex c)
   {double x;
    complex r;

    x = PM_cabs(c);
    if (x == 0.0)
       r = CPHUGE;

    else
       {x = 1.0/(x*x);

	r = x*PM_cconjugate(c);};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CFLOOR - complex floor function
 *           - interpreted to mean the greatest integer less than
 *           - the magnitude of the complex number
 */

double PM_cfloor(complex c)
   {double r, x, y;

    x = creal(c);
    y = cimag(c);
    r = HYPOT(x, y);

    r = floor(r);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CROUND - complex round function
 *           - interpreted to mean the nearest integer to
 *           - the magnitude of the complex number
 */

double PM_cround(complex c)
   {double r, x, y;

    x = creal(c);
    y = cimag(c);
    r = HYPOT(x, y);

    r = floor(r + 0.5);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CEXP - complex exp function */

complex PM_cexp(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = cexp(c);

#else

    double xl, yl, al, cl, sl;

    MEXP(r, c);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CLN - complex log function */

complex PM_cln(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = clog(c);

#else

    double x, y, a, b;

    a = creal(c);
    b = cimag(c);
    if ((a == 0.0) && (b == 0.0))
       r = CMPLX(-HUGE, 0.0);

    else
       {MLOG(r, c);};

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CLOG - complex log10 function */

complex PM_clog(complex c)
   {complex r;
    double x, y, a, b;

    if (_PM.ln10e == 0.0)
       _PM.ln10e = log10(exp(1.0));

    MLOG(r, c);

    r = _PM.ln10e * r;

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CSQR - complex sqr function */

complex PM_csqr(complex c)
   {complex r;

    r = c*c;

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CSQRT - complex sqrt function */

complex PM_csqrt(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = csqrt(c);

#else

    double x, y, a, b, cb;

    x = creal(c);
    y = cimag(c);
    if ((x != 0.0) || (y != 0.0))
       {a  = sqrt(HYPOT(x, y));
	b  = atan2(y, x);
	cb = cos(b);

	x = a*sqrt(0.5*(1.0 + cb));
	y = copysign(a*sqrt(0.5*(1.0 - cb)), b);};

    r = CMPLX(x, y);
#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CSIN - complex sin function */

complex PM_csin(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = csin(c);

#else

    double cx, sx, ey, eyi;

    MEXP_CMP(cx, sx, ey, eyi, c);

    r = CMPLX(0.5*sx*(ey + eyi), 0.5*cx*(ey - eyi));

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CCOS - complex cos function */

complex PM_ccos(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = ccos(c);

#else

    double cx, sx, ey, eyi;

    MEXP_CMP(cx, sx, ey, eyi, c);

    r = CMPLX(0.5*cx*(ey + eyi), 0.5*sx*(eyi - ey));

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CTAN - complex tan function */

complex PM_ctan(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = ctan(c);

#else

    complex sc, cc;
    double cx, sx, ey, eyi;

    MEXP_CMP(cx, sx, ey, eyi, c);

    sc = CMPLX(sx*(ey + eyi), cx*(ey - eyi));
    cc = CMPLX(cx*(ey + eyi), sx*(eyi - ey));

    MDIV(r, sc, cc);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CCOT - complex cot function */

complex PM_ccot(complex c)
   {complex r, sc, cc;
    double cx, sx, ey, eyi;

    MEXP_CMP(cx, sx, ey, eyi, c);

    sc = CMPLX(sx*(ey + eyi), cx*(ey - eyi));
    cc = CMPLX(cx*(ey + eyi), sx*(eyi - ey));

    MDIV(r, cc, sc);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CSINH - complex sinh function */

complex PM_csinh(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = csinh(c);

#else

    double ex, exi, cy, sy;

    MEXP_CMPH(ex, exi, cy, sy, c);

    r = CMPLX(0.5*(ex - exi)*cy, 0.5*(ex + exi)*sy);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CCOSH - complex cosh function */

complex PM_ccosh(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = ccosh(c);

#else

    double ex, exi, cy, sy;

    MEXP_CMPH(ex, exi, cy, sy, c);

    r = CMPLX(0.5*(ex + exi)*cy, 0.5*(ex - exi)*sy);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CTANH - complex tanh function */

complex PM_ctanh(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = ctanh(c);

#else

    complex sc, cc;
    double ex, exi, cy, sy;

    MEXP_CMPH(ex, exi, cy, sy, c);

    sc = CMPLX((ex - exi)*cy, (ex + exi)*sy);
    cc = CMPLX((ex + exi)*cy, (ex - exi)*sy);

    MDIV(r, sc, cc);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CCOTH - complex coth function */

complex PM_ccoth(complex c)
   {complex r, sc, cc;
    double ex, exi, cy, sy;

    MEXP_CMPH(ex, exi, cy, sy, c);

    sc = CMPLX((ex - exi)*cy, (ex + exi)*sy);
    cc = CMPLX((ex + exi)*cy, (ex - exi)*sy);

    MDIV(r, cc, sc);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CASIN - complex asin function
 *          - asin(z) = -i*ln(i*z + sqrt(1 - z*z))
 */

complex PM_casin(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = casin(c);

#else

    double a, b, x, y, xp, xm, pq, as, ac, rx;

    x = creal(c);
    y = cimag(c);

    xp = 1.0 + x;
    xm = 1.0 - x;
    pq = 0.5*(HYPOT(xp, y) + HYPOT(xm, y));

    rx = x/pq;
    if (PM_CLOSETO_REL(rx, 1.0))
       rx = min(rx, 1.0);

    if (PM_CLOSETO_REL(rx, -1.0))
       rx = max(rx, -1.0);

    as = asin(rx);
    ac = acosh(pq);

    a = as;
    b = (y > 0.0) ? ac : -ac;

    r = CMPLX(a, b);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CACOS - complex acos function
 *          - acos(z) = PI/2 + i*ln(i*z + sqrt(1 - z*z))
 */

complex PM_cacos(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = cacos(c);

#else

    double a, b, x, y, xp, xm, pq, as, ac, rx;

    x = creal(c);
    y = cimag(c);

    xp = 1.0 + x;
    xm = 1.0 - x;
    pq = 0.5*(HYPOT(xp, y) + HYPOT(xm, y));

    rx = x/pq;
    if (PM_CLOSETO_REL(rx, 1.0))
       rx = min(rx, 1.0);

    if (PM_CLOSETO_REL(rx, -1.0))
       rx = max(rx, -1.0);

    as = acos(rx);
    ac = acosh(pq);

    a = as;
/*    b = (y >= 0.0) ? ac : -ac; */
    b = (y > 0.0) ? -ac : ac;

    r = CMPLX(a, b);
#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CATAN - complex atan function
 *          - atan(z) = i*(ln(1 - i*z) - ln(1 + i*z))/2
 *          - NOTE: the volatile qualifier on the local doubles
 *          - are necessary because of problems with optimization
 *          - on the opteron
 */

complex PM_catan(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = catan(c);

#else

    double x, y, xs, yp, ym, a;
    volatile double b;

    x = creal(c);
    y = cimag(c);

    xs = x*x;
    yp = 1.0 + y;
    ym = 1.0 - y;

    a = 0.5*atan2(2.0*x, 1.0 - xs - y*y);
    b = 0.25*log((yp*yp + xs)/(ym*ym + xs));

    r = CMPLX(a, b);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CASINH - complex asinh function
 *           - asinh(z)	= ln(z + sqrt(z*z + 1))	
 */

complex PM_casinh(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = casinh(c);

#else

    r = 1.0 + c*c;
    r = c + PM_csqrt(r);
    r = PM_cln(r);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CACOSH - complex acosh function
 *           - acosh(z)	= ln(z + sqrt(z*z - 1))
 */

complex PM_cacosh(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = cacosh(c);

#else

    r = c*c - 1.0;
    r = c + PM_csqrt(r);
    r = PM_cln(r);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CATANH - complex atanh function
 *           - atanh(z)	= ln((1 + z)/(1 - z))/2
 */

complex PM_catanh(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = catanh(c);

#else

    complex z1, z2;

    z1 = c + 1.0;
    z2 = c - 1.0;
    r  = z1/z2;
    r  = 0.5*PM_cln(r);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CTCHN - Tchebyshev polynomial of order N */

complex PM_ctchn(complex x, double n)
   {int i, m;
    double r;
    complex ta, tb, tc;

    r = PM_cabs(x);
    if (1.0 < r)
       tc = CMHUGE;

    else
       {ta = CMPLX(1.0, 0.0);
	tb = x;
	m  = (int) n;

	switch (m)
	   {case 0 :
	         tc = ta;
		 break;

	    case 1 :
	         tc = tb;
		 break;

	    default :
	         tc = CMHUGE;
	         for (i = m-1; i > 0; i--)
		     {tc = 2*(x*tb) - ta;
		      ta = tb;
		      tb = tc;};
		 break;};};

    return(tc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CJN - complex Bessel function of the first kind primitive
 *        - using algorithm from Numerical Recipies in C
 */

complex PM_cjn(complex x, double nd)
   {int j, jtot, m, n;
    double ax;
    complex s, z, bj, bjm, bjp, tot, tox, ret;
    complex xx, y, num, den, ct, st;

    n  = nd;
    ax = PM_cabs(x);
    if (n == 0)
       {if (ax < 8.0)
           {y   = PM_csqr(x);
	    num = PM_chorner(y, _PM_j0_coeff_na, 0, 5);
            den = PM_chorner(y, _PM_j0_coeff_da, 0, 5);
            ret = num/den;}

        else
           {z   = PM_crecip(x);
	    s   = PM_csqrt(two_ov_pi*z);
	    z   = 8.0*z;
            y   = PM_csqr(z);
            xx  = x - pi_1_4;

	    num = PM_chorner(y, _PM_j0_coeff_nb, 0, 4);
	    den = PM_chorner(y, _PM_j0_coeff_db, 0, 4);

	    ct = num*PM_ccos(xx);
	    st = den*z*PM_csin(xx);

	    ret = s*(ct - st);};}
       
    else if (n == 1)
       {if (ax < 8.0)
           {y   = PM_csqr(x);
	    num = PM_chorner(y, _PM_j1_coeff_na, 0, 5);
	    den = PM_chorner(y, _PM_j1_coeff_da, 0, 5);

            ret = x*(num/den);}

        else
           {z   = PM_crecip(x);
	    s   = PM_csqrt(two_ov_pi*z);
	    z   = 8.0*z;
            y   = PM_csqr(z);
            xx  = x - pi_3_4;

	    num = PM_chorner(y, _PM_j1_coeff_nb, 0, 4);
	    den = PM_chorner(y, _PM_j1_coeff_db, 0, 4);

	    ct = num*PM_ccos(xx);
	    st = den*z*PM_csin(xx);

	    ret = s*(ct - st);};}

    else
       {if (ax == 0.0)
           return(Czero);

        else if (ax > (double) n)
           {tox = 2.0*PM_crecip(x);
            bjm = PM_cjn(x, 0.0);
            bj  = PM_cjn(x, 1.0);
            for (j = 1; j < n; j++)
                {bjp = j*(tox*bj) - bjm;
                 bjm = bj;
                 bj  = bjp;};
            ret = bj;}

        else
           {tox = 2.0*PM_crecip(x);

            m    = 2*((n + ((int) sqrt(BESS_ACC*n))) >> 1);
            jtot = 0;
            bjp  = Czero;
            ret  = Czero;
            tot  = Czero;
            bj   = Cone;
            for (j = m; j > 0; j--)
                {bjm = j*(tox*bj) - bjp;
                 bjp = bj;
                 bj  = bjm;
                 if (PM_cabs(bj) > HUGE)
                    {bj  = SMALL*bj;
                     bjp = SMALL*bjp;
                     ret = SMALL*ret;
                     tot = SMALL*tot;};

                 if (jtot)
                    tot += bj;
                 jtot = !jtot;
                 if (j == n)
                    ret = bjp;};

            tot  = 2.0*tot - bj;
            ret /= tot;};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CJ0 - complex j0 */

complex PM_cj0(complex x)
   {complex r;

    r = PM_cjn(x, 0.0);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CJ1 - complex j1 */

complex PM_cj1(complex x)
   {complex r;

    r = PM_cjn(x, 1.0);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CYN - Bessel function of the second kind
 *        - algorithm from Numerical Recipes in C
 */

complex PM_cyn(complex x, double nd)
   {int j, n;
    double ax;
    complex by, bym, byp, ct, st, tox;
    complex z, s, jnc, lnc, rat, ret;
    complex xi, xx, y, num, den;

    n  = nd;
    ax = PM_cabs(x);
    if (n == 0)
       {if (ax < 8.0)
           {y   = PM_csqr(x);
	    num = PM_chorner(y, _PM_y0_coeff_na, 0, 5);
            den = PM_chorner(y, _PM_y0_coeff_da, 0, 5);

            jnc = PM_cjn(x, 0.0);
	    lnc = PM_cln(x);
            rat = num/den;

            ret = two_ov_pi*(jnc*lnc) + rat;}

        else
           {z   = PM_crecip(x);
	    s   = PM_csqrt(two_ov_pi*z);
	    z   = 8.0*z;
            y   = PM_csqr(z);
            xx  = x - pi_1_4;

	    num = PM_chorner(y, _PM_y0_coeff_nb, 0, 4);
            den = PM_chorner(y, _PM_y0_coeff_db, 0, 4);

	    ct = den*z*PM_ccos(xx);
	    st = num*PM_csin(xx);

	    ret = s*(ct + st);};}

    else if (n == 1)
       {if (ax < 8.0)
           {y   = PM_csqr(x);
	    xi  = PM_crecip(x);
	    num = PM_chorner(y, _PM_y1_coeff_na, 0, 5);
            den = PM_chorner(y, _PM_y1_coeff_da, 0, 6);

            jnc = PM_cjn(x, 1.0);
	    lnc = PM_cln(x);
            rat = x*(num/den);

            ret = two_ov_pi*(jnc*lnc - xi) + rat;}

        else
           {z   = PM_crecip(x);
	    s   = PM_csqrt(two_ov_pi*z);
	    z   = 8.0*z;
            y   = PM_csqr(z);
            xx  = x - pi_3_4;

	    num = PM_chorner(y, _PM_y1_coeff_nb, 0, 4);
            den = PM_chorner(y, _PM_y1_coeff_db, 0, 4);

	    ct = den*z*PM_ccos(xx);
	    st = num*PM_csin(xx);

	    ret = s*(ct + st);};}

    else
       {tox = 2.0*PM_crecip(x);
        by  = PM_cyn(x, 1.0);
        bym = PM_cyn(x, 0.0);
        for (j = 1; j < n; j++)
            {byp = j*(tox*by) - bym;
             bym = by;
             by  = byp;};

        ret = by;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CY0 - y0 */

complex PM_cy0(complex x)
   {

    return(PM_cyn(x, 0.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CY1 - y1 */

complex PM_cy1(complex x)
   {

    return(PM_cyn(x, 1.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CIN - modified Bessel function of the first kind
 *        - algorithm from Numerical Recipes in C
 */

complex PM_cin(complex x, double nd)
   {int j, n, ns;
    double ax;
    complex bi, bim, bip, tox;
    complex z, y, ret, a1, a2;

    n  = nd;
    ax = PM_cabs(x);
    if (n == 0)
       {if (ax < 3.75)
           {z   = (1.0/3.75)*x;
	    y   = PM_csqr(z);
	    ret = PM_chorner(y, _PM_i0_coeff_na, 0, 6);}
        else
           {y   = 3.75*PM_crecip(x);
	    a1  = PM_chorner(y, _PM_i0_coeff_nb, 0, 8);
            a2  = PM_cexp(x);
	    ret = (a1*a2)/PM_csqrt(x);};}

    else if (n == 1)
       {if (ax < 3.75)
           {z   = (1.0/3.75)*x;
	    y   = PM_csqr(z);
	    a1  = PM_chorner(y, _PM_i1_coeff_na, 0, 6);
	    ret = x*a1;}
        else
           {y   = 3.75*PM_crecip(x);
	    a1  = PM_chorner(y, _PM_i1_coeff_nb, 0, 8);
            a2  = PM_cexp(x);
	    ret = (a1*a2)/PM_csqrt(x);};}

    else if (ax == 0.0)
      ret = x;

    else
       {tox = 2.0*PM_crecip(x);
        bi  = Cone;
	bip = Czero;
        ret = Czero;
	ns  = 2*(n + (int) sqrt(BESS_ACC*n));
	for (j = ns; 0 < j; j--)
	    {bim = j*(tox*bi) + bip;
             bip = bi;
	     bi  = bim;
	     if (PM_cabs(bi) > HUGE)
	        {ret = SMALL*ret;
		 bi  = SMALL*bi;
		 bip = SMALL*bip;};
	     if (j == n)
	        ret = bip;};

	a1  = PM_cin(x, 0.0);
	a2  = a1/bi;
	ret *= a2;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CI0 - i0 */

complex PM_ci0(complex x)
   {

    return(PM_cin(x, 0.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CI1 - i1 */

complex PM_ci1(complex x)
   {

    return(PM_cin(x, 1.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CKN - modified Bessel function of the second kind
 *        - algorithm from Numerical Recipes in C
 */

complex PM_ckn(complex x, double nd)
   {int j, n;
    double ax;
    complex bk, bkm, bkp, tox;
    complex z, y, ret, a1, a2, a3;

    n  = nd;
    ax = PM_cabs(x);
    if (n == 0)
       {if (ax <= 2.0)
           {z   = 0.5*x;
	    y   = PM_csqr(z);
	    a1  = PM_chorner(y, _PM_k0_coeff_na, 0, 6);
	    a2  = PM_cin(x, 0.0);
	    a3  = PM_cln(z);
	    ret = a1 - a2*a3;}
        else
           {y   = 2.0*PM_crecip(x);
	    a1  = PM_chorner(y, _PM_k0_coeff_nb, 0, 6);
            z   = -x;
            a2  = PM_cexp(z);
	    a3  = PM_csqrt(x);
	    ret = a1*(a2/a3);};}

    else if (n == 1)
       {if (ax <= 2.0)
           {z   = 0.5*x;
	    y   = PM_csqr(z);
	    a1  = PM_chorner(y, _PM_k1_coeff_na, 0, 6);
	    a1  = a1/x;
	    a2  = PM_cln(z);
	    a3  = PM_cin(x, 1.0);
	    ret = a1 + a2*a3;}
        else
           {y   = 2.0*PM_crecip(x);
	    a1  = PM_chorner(y, _PM_k1_coeff_nb, 0, 6);
            z   = -x;
            a2  = PM_cexp(z);
	    a3  = PM_csqrt(x);
	    ret = a1*(a2/a3);};}

    else
       {tox = 2.0*PM_crecip(x);

        bkm = PM_ckn(x, 0.0);
	bk  = PM_ckn(x, 1.0);
        ret = Czero;
	for (j = 1; j < n; j++)
            {bkp = j*(tox*bk) + bkm;
             bkm = bk;
	     bk  = bkp;};

	ret = bk;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CK0 - k0 */

complex PM_ck0(complex x)
   {

    return(PM_ckn(x, 0.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CK1 - k1 */

complex PM_ck1(complex x)
   {

    return(PM_ckn(x, 1.0));}

/*--------------------------------------------------------------------------*/

/*                           BINARY COMPLEX FUNCTIONS                       */

/*--------------------------------------------------------------------------*/

/* PM_CADD - return a + b */

complex PM_cadd(complex a, complex b)
   {complex c;

    c = a + b;

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CSUB - return a - b */

complex PM_csub(complex a, complex b)
   {complex c;

    c = a - b;

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CMLT - return a * b */

complex PM_cmlt(complex a, complex b)
   {complex c;

    c = a*b;

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CDIV - return a / b */

complex PM_cdiv(complex a, complex b)
   {complex c;

    c = a/b;

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CPOW - return a^b */

complex PM_cpow(complex a, complex b)
   {complex c;

#ifdef USE_C99_FUNCTIONS

    c = cpow(a, b);

#else

    complex l, x;

    l = PM_cln(a);
    x = PM_cmlt(b, l);
    c = PM_cexp(x);

#endif

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CEQUAL - return TRUE iff a == b */

int PM_cequal(complex a, complex b)
   {int eq;

#ifdef USE_C99_FUNCTIONS

    eq = a == b;

#else

    double ar, ai, br, bi;

    ar = creal(a);
    ai = cimag(a);

    br = creal(b);
    bi = cimag(b);

    eq = ((ar == br) && (ai == bi));

#endif

    return(eq);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CCLOSE - return TRUE iff a == b to within machine precision */

int PM_cclose(complex a, complex b, double tol)
   {int eq;
    double ar, ai;
    double nm;
    complex dl, nd;

    dl = PM_csub(a, b);
    nm = 0.5*(PM_cabs(a) + PM_cabs(b) + SMALL);
    nd = nm*dl;

    ar = creal(nd);
    ai = cimag(nd);

    ar = fabs(ar);
    ai = fabs(ai);

    eq = ((ar < tol) && (ai < tol));

    return(eq);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

