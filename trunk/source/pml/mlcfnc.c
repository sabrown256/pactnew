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
     _x   = PM_REAL_C(_c);                                                  \
     _y   = PM_IMAGINARY_C(_c);                                             \
     _ey  = exp(_y);                                                        \
     _eyi = 1.0/_ey;                                                        \
     _cx  = cos(_x);                                                        \
     _sx  = sin(_x);}

#define MEXP_CMPH(_ex, _exi, _cy, _sy, _c)                                  \
    {double _x, _y;                                                         \
     _x   = PM_REAL_C(_c);                                                  \
     _y   = PM_IMAGINARY_C(_c);                                             \
     _ex  = exp(_x);                                                        \
     _exi = 1.0/_ex;                                                        \
     _cy  = cos(_y);                                                        \
     _sy  = sin(_y);}

#define MEXP(_rc, _c)                                                       \
    {xl = PM_REAL_C(_c);                                                    \
     yl = PM_IMAGINARY_C(_c);                                               \
     al = exp(xl);                                                          \
     cl = cos(yl);                                                          \
     sl = sin(yl);                                                          \
     _rc = PM_COMPLEX(al*cl, al*sl);}

#define MLOG(_rc, _c)                                                       \
    {x   = PM_REAL_C(_c);                                                   \
     y   = PM_IMAGINARY_C(_c);                                              \
     a   = log(HYPOT(x, y));                                                \
     b   = atan2(y, x);                                                     \
     _rc = PM_COMPLEX(a, b);}

#define MDIV(_rc, _ac, _bc)                                                 \
    {double ar, ai, br, bi, abr, abi, u, brovu, biovu, d;                   \
     ar = PM_REAL_C(_ac);                                                   \
     ai = PM_IMAGINARY_C(_ac);                                              \
     br = PM_REAL_C(_bc);                                                   \
     bi = PM_IMAGINARY_C(_bc);                                              \
     abr = ABS(br);                                                         \
     abi = ABS(bi);                                                         \
     u  = max(abr,abi);                                                     \
     brovu = br/u;                                                          \
     biovu = bi/u;                                                          \
     d     = 1.0/(br*brovu + bi*biovu);                                     \
     _rc   = PM_COMPLEX(d*(ar*brovu + ai*biovu), d*(ai*brovu - ar*biovu));}

#ifdef HAVE_ANSI_C9X_COMPLEX

complex
 CPHUGE = HUGE,
 CMHUGE = -HUGE,
 Czero  = 0.0;

/* the implementation here has superior accuracy to
 * several vendor libraries - so the no-brainer is
 * to continue to use these
# define USE_C99_FUNCTIONS
 */

#else

complex
 CPHUGE = {HUGE, 0.0},
 CMHUGE = {-HUGE, 0.0},
 Czero  = {0.0, 0.0};

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CHORNER - evaluate polynomial using Horner's rule
 *           -   y = Sum(Ci*X^i, mn <= i <= mx)
 */

complex PM_chorner(complex x, double *c, int mn, int mx)
   {int i, in, iz, ix;
    complex r, rn, r0, rp, xi;

    in = min(mn, mx);
    ix = max(mn, mx);

    iz = max(in, 0);

/* grab the constant coefficient */
    if ((in <= iz) && (iz <= ix))
       r0 = PM_COMPLEX(c[iz-in], 0.0);
    else
       r0 = Czero;

/* do the positive powers */
    rp = Czero;
    for (i = ix; iz < i; i--)
        {rp = PM_PLUS_RC(c[i-in], rp);
	 rp = PM_TIMES_CC(x, rp);};

/* do the negative powers */
    if (PM_cequal(x, Czero) && (in < 0))
       rn = CMHUGE;

    else
       {xi = PM_crecip(x);
	rn = Czero;
	for (i = in; i < iz; i++)
	    {rn = PM_PLUS_RC(c[i-in], rn);
	     rn = PM_TIMES_CC(xi, rn);};};

    r = PM_PLUS_CC(rn, rp);
    r = PM_PLUS_CC(r, r0);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CRANDOM - complex random function */

complex PM_crandom(complex c)
   {double tp, x, y, cx, sx;

    tp = 2.0*PI;

    y = PM_random(0.0);
    x = tp*y;

    cx = cos(x);
    sx = sin(x);

    c = PM_COMPLEX(cx, sx);

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

    xr = fabs(PM_REAL_C(x));
    xi = fabs(PM_IMAGINARY_C(x));

    if ( ( xr == 0.0 ) && ( xi == 0.0 ) )
       {modulus = 0.0;}
    else if ( xr >= xi )
       {ratio = PM_IMAGINARY_C(x)/PM_REAL_C(x);
	modulus = xr*sqrt( 1.0 + ratio*ratio );}
    else
       {ratio = PM_REAL_C(x)/PM_IMAGINARY_C(x);
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

    c = PM_COMPLEX(PM_REAL_C(c), -1.0*PM_IMAGINARY_C(c));

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

    x = PM_REAL_C(c);
    y = PM_IMAGINARY_C(c);

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

	r = PM_cconjugate(c);
	r = PM_TIMES_RC(x, r);};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CFLOOR - complex floor function
 *           - interpreted to mean the greatest integer less than
 *           - the magnitude of the complex number
 */

double PM_cfloor(complex c)
   {double r, x, y;

    x = PM_REAL_C(c);
    y = PM_IMAGINARY_C(c);
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

    x = PM_REAL_C(c);
    y = PM_IMAGINARY_C(c);
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

    a = PM_REAL_C(c);
    b = PM_IMAGINARY_C(c);
    if ((a == 0.0) && (b == 0.0))
       r = PM_COMPLEX(-HUGE, 0.0);

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

    r = PM_TIMES_RC(_PM.ln10e, r);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CSQR - complex sqr function */

complex PM_csqr(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = c*c;

#else

    r = PM_TIMES_CC(c, c);

#endif

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

    x = PM_REAL_C(c);
    y = PM_IMAGINARY_C(c);
    if ((x != 0.0) || (y != 0.0))
       {a  = sqrt(HYPOT(x, y));
	b  = atan2(y, x);
	cb = cos(b);

	x = a*sqrt(0.5*(1.0 + cb));
	y = a*sqrt(0.5*(1.0 - cb));};

    r = PM_COMPLEX(x, y);
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

    r = PM_COMPLEX(0.5*sx*(ey + eyi), 0.5*cx*(ey - eyi));

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

    r = PM_COMPLEX(0.5*cx*(ey + eyi), 0.5*sx*(eyi - ey));

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

    sc = PM_COMPLEX(sx*(ey + eyi), cx*(ey - eyi));
    cc = PM_COMPLEX(cx*(ey + eyi), sx*(eyi - ey));

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

    sc = PM_COMPLEX(sx*(ey + eyi), cx*(ey - eyi));
    cc = PM_COMPLEX(cx*(ey + eyi), sx*(eyi - ey));

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

    r = PM_COMPLEX(0.5*(ex - exi)*cy, 0.5*(ex + exi)*sy);

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

    r = PM_COMPLEX(0.5*(ex + exi)*cy, 0.5*(ex - exi)*sy);

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

    sc = PM_COMPLEX((ex - exi)*cy, (ex + exi)*sy);
    cc = PM_COMPLEX((ex + exi)*cy, (ex - exi)*sy);

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

    sc = PM_COMPLEX((ex - exi)*cy, (ex + exi)*sy);
    cc = PM_COMPLEX((ex + exi)*cy, (ex - exi)*sy);

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

    x = PM_REAL_C(c);
    y = PM_IMAGINARY_C(c);

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

    r = PM_COMPLEX(a, b);

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

    x = PM_REAL_C(c);
    y = PM_IMAGINARY_C(c);

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

    r = PM_COMPLEX(a, b);
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

    x = PM_REAL_C(c);
    y = PM_IMAGINARY_C(c);

    xs = x*x;
    yp = 1.0 + y;
    ym = 1.0 - y;

    a = 0.5*atan2(2.0*x, 1.0 - xs - y*y);
    b = 0.25*log((yp*yp + xs)/(ym*ym + xs));

    r = PM_COMPLEX(a, b);

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

    r = PM_TIMES_CC(c, c);
    r = PM_PLUS_RC(1.0, r);
    r = PM_csqrt(r);
    r = PM_PLUS_CC(c, r);
    r = PM_cln(r);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CACOSH - complex acosh function
 *           - acosh(z)	= ln(z + sqrt(z - 1)*sqrt(z + 1))	
 */

complex PM_cacosh(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = cacosh(c);

#else

    complex z1, z2;

    z1 = PM_PLUS_RC(1.0, c);
    z2 = PM_PLUS_RC(-1.0, c);

    z1 = PM_csqrt(z1);
    z2 = PM_csqrt(z2);

    r = PM_TIMES_CC(z1, z2);
    r = PM_PLUS_CC(c, r);
    r = PM_cln(r);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CATANH - complex atanh function
 *           - atanh(z)	= (ln(1 + z) - ln(1 - z))/2
 */

complex PM_catanh(complex c)
   {complex r;

#ifdef USE_C99_FUNCTIONS

    r = catanh(c);

#else

    complex z1, z2;

    z1 = PM_PLUS_RC(1.0, c);
    z2 = PM_PLUS_RC(-1.0, c);
    z2 = PM_TIMES_RC(-1.0, z2);

    z1 = PM_cln(z1);
    z2 = PM_cln(z2);

    r = PM_MINUS_CC(z1, z2);
    r = PM_TIMES_RC(0.5, r);

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
       {ta = PM_COMPLEX(1.0, 0.0);
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
		     {tc = PM_TIMES_CC(x, tb);
		      tc = PM_TIMES_RC(2, tc);
		      tc = PM_MINUS_CC(tc, ta);
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
            ret = PM_DIVIDE_CC(num, den);}

        else
           {z   = PM_crecip(x);
	    s   = PM_csqrt(PM_TIMES_RC(two_ov_pi, z));
	    z   = PM_TIMES_RC(8.0, z);
            y   = PM_csqr(z);
            xx  = PM_PLUS_RC(-pi_1_4, x);

	    num = PM_chorner(y, _PM_j0_coeff_nb, 0, 4);
	    den = PM_chorner(y, _PM_j0_coeff_db, 0, 4);

	    ct = PM_ccos(xx);
	    ct = PM_TIMES_CC(num, ct);

	    st = PM_csin(xx);
	    st = PM_TIMES_CC(den, st);
	    st = PM_TIMES_CC(z, st);

	    ret = PM_MINUS_CC(ct, st);
	    ret = PM_TIMES_CC(s, ret);};}
       
    else if (n == 1)
       {if (ax < 8.0)
           {y   = PM_csqr(x);
	    num = PM_chorner(y, _PM_j1_coeff_na, 0, 5);
	    den = PM_chorner(y, _PM_j1_coeff_da, 0, 5);

            ret = PM_DIVIDE_CC(num, den);
            ret = PM_TIMES_CC(x, ret);}

        else
           {z   = PM_crecip(x);
	    s   = PM_csqrt(PM_TIMES_RC(two_ov_pi, z));
	    z   = PM_TIMES_RC(8.0, z);
            y   = PM_csqr(z);
            xx  = PM_PLUS_RC(-pi_3_4, x);

	    num = PM_chorner(y, _PM_j1_coeff_nb, 0, 4);
	    den = PM_chorner(y, _PM_j1_coeff_db, 0, 4);

	    ct = PM_ccos(xx);
	    ct = PM_TIMES_CC(num, ct);

	    st = PM_csin(xx);
	    st = PM_TIMES_CC(den, st);
	    st = PM_TIMES_CC(z, st);

	    ret = PM_MINUS_CC(ct, st);
	    ret = PM_TIMES_CC(s, ret);};}

    else
       {if (ax == 0.0)
           return(Czero);

        else if (ax > (double) n)
           {tox = PM_crecip(x);
	    tox = PM_TIMES_RC(2.0, tox);
            bjm = PM_cjn(x, 0.0);
            bj  = PM_cjn(x, 1.0);
            for (j = 1; j < n; j++)
                {bjp = PM_TIMES_CC(tox, bj);
		 bjp = PM_TIMES_RC(j, bjp);
		 bjp = PM_MINUS_CC(bjp, bjm);
                 bjm = bj;
                 bj  = bjp;};
            ret = bj;}

        else
           {tox = PM_crecip(x);
	    tox = PM_TIMES_RC(2.0, tox);

            m    = 2*((n + ((int) sqrt(BESS_ACC*n))) >> 1);
            jtot = 0;
            bjp  = Czero;
            ret  = Czero;
            tot  = Czero;
            bj   = Czero;
            for (j = m; j > 0; j--)
                {bjm = PM_TIMES_CC(tox, bj);
		 bjm = PM_TIMES_RC(j, bjm);
		 bjm = PM_MINUS_CC(bjm, bjp);
                 bjp = bj;
                 bj  = bjm;
                 if (PM_cabs(bj) > HUGE)
                    {bj  = PM_TIMES_RC(SMALL, bj);
                     bjp = PM_TIMES_RC(SMALL, bjp);
                     ret = PM_TIMES_RC(SMALL, ret);
                     tot = PM_TIMES_RC(SMALL, tot);};

                 if (jtot)
                    tot = PM_PLUS_CC(tot, bj);
                 jtot = !jtot;
                 if (j == n)
                    ret = bjp;};

            tot = PM_TIMES_RC(2.0, tot);
	    tot = PM_MINUS_CC(tot, bj);
            ret = PM_DIVIDE_CC(ret, tot);};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CJ0 - complex j0 */

complex PM_cj0(complex x)
   {

    return(PM_cjn(x, 0.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CJ1 - complex j1 */

complex PM_cj1(complex x)
   {

    return(PM_cjn(x, 1.0));}

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
            rat = PM_DIVIDE_CC(num, den);

            ret = PM_TIMES_CC(jnc, lnc);
            ret = PM_TIMES_RC(two_ov_pi, ret);

            ret = PM_PLUS_CC(rat, ret);}

        else
           {z   = PM_crecip(x);
	    s   = PM_csqrt(PM_TIMES_RC(two_ov_pi, z));
	    z   = PM_TIMES_RC(8.0, z);
            y   = PM_csqr(z);
            xx  = PM_PLUS_RC(-pi_1_4, x);

	    num = PM_chorner(y, _PM_y0_coeff_nb, 0, 4);
            den = PM_chorner(y, _PM_y0_coeff_db, 0, 4);

	    ct = PM_ccos(xx);
	    ct = PM_TIMES_CC(den, ct);
	    ct = PM_TIMES_CC(z, ct);

	    st = PM_csin(xx);
	    st = PM_TIMES_CC(st, num);

	    ret = PM_PLUS_CC(ct, st);
	    ret = PM_TIMES_CC(s, ret);};}

    else if (n == 1)
       {if (ax < 8.0)
           {y   = PM_csqr(x);
	    xi  = PM_crecip(x);
	    num = PM_chorner(y, _PM_y1_coeff_na, 0, 5);
            den = PM_chorner(y, _PM_y1_coeff_da, 0, 6);

            jnc = PM_cjn(x, 1.0);
	    lnc = PM_cln(x);
            rat = PM_DIVIDE_CC(num, den);
            rat = PM_TIMES_CC(x, rat);

            ret = PM_TIMES_CC(jnc, lnc);
            ret = PM_MINUS_CC(ret, xi);
            ret = PM_TIMES_RC(two_ov_pi, ret);

            ret = PM_PLUS_CC(rat, ret);}

        else
           {z   = PM_crecip(x);
	    s   = PM_csqrt(PM_TIMES_RC(two_ov_pi, z));
	    z   = PM_TIMES_RC(8.0, z);
            y   = PM_csqr(z);
            xx  = PM_PLUS_RC(-pi_3_4, x);

	    num = PM_chorner(y, _PM_y1_coeff_nb, 0, 4);
            den = PM_chorner(y, _PM_y1_coeff_db, 0, 4);

	    ct = PM_ccos(xx);
	    ct = PM_TIMES_CC(den, ct);
	    ct = PM_TIMES_CC(z, ct);

	    st = PM_csin(xx);
	    st = PM_TIMES_CC(num, st);

	    ret = PM_PLUS_CC(ct, st);
	    ret = PM_TIMES_CC(s, ret);};}

    else
       {tox = PM_crecip(x);
	tox = PM_TIMES_RC(2.0, tox);

        by  = PM_cyn(x, 1.0);
        bym = PM_cyn(x, 0.0);
        for (j = 1; j < n; j++)
            {byp = PM_TIMES_CC(tox, by);
	     byp = PM_TIMES_RC(j, byp);
             byp = PM_MINUS_CC(byp, bym);

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
           {z   = PM_TIMES_RC(1.0/3.75, x);
	    y   = PM_csqr(z);
	    ret = PM_chorner(y, _PM_i0_coeff_na, 0, 6);}
        else
           {y   = PM_crecip(x);
	    y   = PM_TIMES_RC(3.75, y);
	    a1  = PM_chorner(y, _PM_i0_coeff_nb, 0, 8);
            a2  = PM_cexp(x);
            a2  = PM_TIMES_CC(a1, a2);
	    a1  = PM_csqrt(x);
	    ret = PM_DIVIDE_CC(a2, a1);};}

    else if (n == 1)
       {if (ax < 3.75)
           {z   = PM_TIMES_RC(1.0/3.75, x);
	    y   = PM_csqr(z);
	    a1  = PM_chorner(y, _PM_i1_coeff_na, 0, 6);
	    ret = PM_TIMES_CC(x, a1);}
        else
           {y   = PM_crecip(x);
	    y   = PM_TIMES_RC(3.75, y);
	    a1  = PM_chorner(y, _PM_i1_coeff_nb, 0, 8);
            a2  = PM_cexp(x);
            a2  = PM_TIMES_CC(a1, a2);
	    a1  = PM_csqrt(x);
	    ret = PM_DIVIDE_CC(a2, a1);};}

    else if (ax == 0.0)
      ret = x;

    else
       {tox = PM_crecip(x);
	tox = PM_TIMES_RC(2.0, tox);
        bi  = PM_COMPLEX(1.0, 0.0);
	bip = Czero;
        ret = Czero;
	ns  = 2*(n + (int) sqrt(BESS_ACC*n));
	for (j = ns; 0 < j; j--)
	    {bim = PM_TIMES_CC(tox, bi);
	     bim = PM_TIMES_RC(j, bim);
	     bim = PM_PLUS_CC(bip, bim);
             bip = bi;
	     bi  = bim;
	     if (PM_cabs(bi) > HUGE)
	        {ret = PM_TIMES_RC(SMALL, ret);
		 bi  = PM_TIMES_RC(SMALL, bi);
		 bip = PM_TIMES_RC(SMALL, bip);};
	     if (j == n)
	        ret = bip;};

	a1  = PM_cin(x, 0.0);
	a2  = PM_DIVIDE_CC(a1, bi);
	ret = PM_TIMES_CC(ret, a2);};

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
           {z   = PM_TIMES_RC(0.5, x);
	    y   = PM_csqr(z);
	    a1  = PM_chorner(y, _PM_k0_coeff_na, 0, 6);
	    a2  = PM_cin(x, 0.0);
	    a3  = PM_cln(z);
	    a2  = PM_TIMES_CC(a2, a3);
	    ret = PM_MINUS_CC(a1, a2);}
        else
           {y   = PM_crecip(x);
            y   = PM_TIMES_RC(2.0, y);
	    a1  = PM_chorner(y, _PM_k0_coeff_nb, 0, 6);
            z   = PM_TIMES_RC(-1.0, x);
            a2  = PM_cexp(z);
	    a3  = PM_csqrt(x);
	    a2  = PM_DIVIDE_CC(a2, a3);
	    ret = PM_TIMES_CC(a1, a2);};}

    else if (n == 1)
       {if (ax <= 2.0)
           {z   = PM_TIMES_RC(0.5, x);
	    y   = PM_csqr(z);
	    a1  = PM_chorner(y, _PM_k1_coeff_na, 0, 6);
	    a1  = PM_DIVIDE_CC(a1, x);
	    a2  = PM_cln(z);
	    a3  = PM_cin(x, 1.0);
	    a2  = PM_TIMES_CC(a2, a3);
	    ret = PM_PLUS_CC(a1, a2);}
        else
           {y   = PM_crecip(x);
            y   = PM_TIMES_RC(2.0, y);
	    a1  = PM_chorner(y, _PM_k1_coeff_nb, 0, 6);
            z   = PM_TIMES_RC(-1.0, x);
            a2  = PM_cexp(z);
	    a3  = PM_csqrt(x);
	    a2  = PM_DIVIDE_CC(a2, a3);
	    ret = PM_TIMES_CC(a1, a2);};}

    else
       {tox = PM_crecip(x);
	tox = PM_TIMES_RC(2.0, tox);

        bkm = PM_ckn(x, 0.0);
	bk  = PM_ckn(x, 1.0);
        ret = Czero;
	for (j = 1; j < n; j++)
            {bkp = PM_TIMES_CC(tox, bk);
	     bkp = PM_TIMES_RC(j, bkp);
             bkp = PM_PLUS_CC(bkp, bkm);
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

#ifdef USE_C99_FUNCTIONS

    c = a + b;

#else

    c = PM_PLUS_CC(a, b);

#endif

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CSUB - return a - b */

complex PM_csub(complex a, complex b)
   {complex c;

#ifdef USE_C99_FUNCTIONS

    c = a - b;

#else

    c = PM_MINUS_CC(a, b);

#endif

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CMLT - return a * b */

complex PM_cmlt(complex a, complex b)
   {complex c;

#ifdef USE_C99_FUNCTIONS

    c = a*b;

#else

    c = PM_TIMES_CC(a, b);

#endif

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CDIV - return a / b */

complex PM_cdiv(complex a, complex b)
   {complex c;

#ifdef USE_C99_FUNCTIONS

    c = a/b;

#else

    double br, bi, ratio, den, x, y; 

    br = fabs(PM_REAL_C(b));
    bi = fabs(PM_IMAGINARY_C(b));

    if (br >= bi)
       {ratio = PM_IMAGINARY_C(b)/PM_REAL_C(b);
	den = PM_REAL_C(b) + PM_IMAGINARY_C(b)*ratio;
	x = (PM_REAL_C(a) + PM_IMAGINARY_C(a)*ratio)/den;
	y = (PM_IMAGINARY_C(a) - PM_REAL_C(a)*ratio)/den;}
    else
       {ratio = PM_REAL_C(b)/PM_IMAGINARY_C(b);
	den = PM_REAL_C(b)*ratio + PM_IMAGINARY_C(b);
	x = (PM_REAL_C(a)*ratio + PM_IMAGINARY_C(a))/den;
	y = (PM_IMAGINARY_C(a)*ratio - PM_REAL_C(a))/den;}

    c = PM_COMPLEX(x, y);

#endif

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

    ar = PM_REAL_C(a);
    ai = PM_IMAGINARY_C(a);

    br = PM_REAL_C(b);
    bi = PM_IMAGINARY_C(b);

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
    nd = PM_TIMES_RC(nm, dl);

    ar = PM_REAL_C(nd);
    ai = PM_IMAGINARY_C(nd);

    ar = ABS(ar);
    ai = ABS(ai);

    eq = ((ar < tol) && (ai < tol));

    return(eq);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

