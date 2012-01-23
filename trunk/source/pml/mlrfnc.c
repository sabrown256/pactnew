/*
 * MLRFNC.C - real math functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"
#include "scope_math.h"

#ifdef NO_BESSEL_FUNCTIONS
# define BESSEL_JN
# define BESSEL_YN
#endif

double
 _PM_j0_coeff_na[6] = {  57568490574.0, -13362590354.0,  651619640.7,
		        -11214424.18,    77392.33017,   -184.9052456 },
 _PM_j0_coeff_da[6] = { 57568490411.0, 1029532985.0, 9494680.718,
		        59272.64853,   267.8532712,  1.0 },

 _PM_j0_coeff_nb[5] = {  1.0,             -0.1098628627e-2, 0.2734510407e-4,
		        -0.2073370639e-5,  0.2093887211e-6 },
 _PM_j0_coeff_db[5] = { -0.1562499995e-1,  0.1430488765e-3, -0.6911147651e-5,
			 0.7621095161e-6, -0.934935152e-7 },

 _PM_j1_coeff_na[6] = {  72362614232.0, -7895059235.0,  242396853.1,
			-2972611.439,    15704.48260,  -30.16036606 },
 _PM_j1_coeff_da[6] = { 144725228442.0, 2300535178.0, 18583304.74,
			99447.43394,    376.9991397,  1.0 },

 _PM_j1_coeff_nb[5] = { 1.0,              0.183105e-2,   -0.3516396496e-4,
			0.2457520174e-5, -0.240337019e-6 },
 _PM_j1_coeff_db[5] = {  0.04687499995, -0.2002690873e-3, 0.8449199096e-5,
			-0.88228987e-6,  0.105787412e-6 },

 _PM_y0_coeff_na[6] = { -2957821389.0,  7062834065.0, -512359803.6,
			 10879881.29,  -86327.92757,   228.4622733 },
 _PM_y0_coeff_da[6] = { 40076544269.0, 745249964.8, 7189466.438,
			47447.26470,   226.1030244, 1.0 },

 _PM_y0_coeff_nb[5] = {  1.0,             -0.1098628627e-2, 0.2734510407e-4,
			-0.2073370639e-5,  0.2093887211e-6 },
 _PM_y0_coeff_db[5] = { -0.1562499995e-1,  0.1430488765e-3, -0.6911147651e-5,
			 0.7621095161e-6, -0.934945152e-7 },

 _PM_y1_coeff_na[6] = { -0.4900604943e13,  0.1275274390e13, -0.5153438139e11,
			 0.7349264551e9,  -0.4237922726e7,   0.8511937935e4 },
 _PM_y1_coeff_da[7] = { 0.2499580570e14, 0.4244419664e12, 0.3733650367e10,
			0.2245904002e8,  0.1020426050e6,  0.3549632885e3, 1.0},

 _PM_y1_coeff_nb[5] = { 1.0,              0.183105e-2,   -0.3516396496e-4,
			0.2457520174e-5, -0.240337019e-6 },
 _PM_y1_coeff_db[5] = {  0.04687499995, -0.2002690873e-3, 0.8449199096e-5,
			-0.88228987e-6,  0.105787412e-6 },

 _PM_i0_coeff_na[7] = {  1.0,          3.5156229,     3.0899424,  1.2067492,
		         0.2659732,    0.360768e-1,   0.45813e-2 },
 _PM_i0_coeff_nb[9] = { 0.39894228,    0.1328592e-1,  0.225319e-2,
                       -0.157565e-2,   0.916281e-2,  -0.2057706e-1,
                        0.2635537e-1, -0.1647633e-1,  0.392377e-2 },

 _PM_i1_coeff_na[7] = {  0.5,          0.87890594,    0.51498869,
                         0.15084934,   0.2658733e-1,  0.301532e-2,
                         0.32411e-3 },
 _PM_i1_coeff_nb[9] = {  0.39894228,  -0.3988024e-1, -0.362018e-2,
                         0.163801e-2, -0.1031555e-1,  0.2282967e-1,
                        -0.2895312e-1, 0.1787654e-1, -0.420059e-2},

 _PM_k0_coeff_na[7] = { -0.57721566,    0.42278420,    0.23069756,
			 0.3488590e-1,  0.262698e-2,   0.10750e-3,
			 0.74e-5},
 _PM_k0_coeff_nb[7] = {  1.25331414,   -0.7832358e-1,  0.2189568e-1,
			-0.1062446e-1,  0.587872e-2,  -0.251540e-2,
                         0.53208e-3},

 _PM_k1_coeff_na[7] = {  1.0,           0.15443144,   -0.67278579, 
                        -0.18156897,   -0.1919402e-1, -0.110404e-2,
                        -0.4686e-4},
 _PM_k1_coeff_nb[7] = {  1.25331414,    0.23498619,   -0.3655620e-1,
                         0.1504268e-1, -0.780353e-2,   0.325614e-2,
                        -0.68245e-3};

double
 two_ov_pi = 0.636619772,
 pi_1_4    = 0.785398164,
 pi_3_4    = 2.356194491;

int
 _PM_use_pml_bessel = TRUE;

/* you cannot trust vendors to handle these declarations in <math.h>
 * some regard them as special extensions which must be specifically
 * asked for - phooey
 */

double
 j0(double x),
 j1(double x),
 jn(int n, double x),
 y0(double x),
 y1(double x),
 yn(int n, double x);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_HORNER - evaluate polynomial using Horner's rule
 *           -   y = Sum(Ci*X^i, mn <= i <= mx)
 */

double PM_horner(double x, double *c, int mn, int mx)
   {int i, in, iz, ix;
    double r, rn, r0, rp, xi;

    in = min(mn, mx);
    ix = max(mn, mx);

    iz = max(in, 0);

/* grab the constant coefficient */
    if ((in <= iz) && (iz <= ix))
       r0 = c[iz-in];
    else
       r0 = 0.0;

/* do the positive powers */
    rp = 0.0;
    for (i = ix; iz < i; i--)
        {rp += c[i-in];
	 rp *= x;};

/* do the negative powers */
    if (x == 0.0)
       rn = (in < 0) ? -HUGE : 0.0;

    else
       {xi = 1.0/x;
	rn = 0.0;
	for (i = in; i < iz; i++)
	    {rn += c[i-in];
	     rn *= xi;};};

    r = rn + r0 + rp;

    return(r);}

/*--------------------------------------------------------------------------*/

/*                          REAL UNARY FUNCTIONS                            */

/*--------------------------------------------------------------------------*/
 
/* PM_SQR - fast version of sqr? */
 
double PM_sqr(double x)
   {double rv;

    rv = x*x;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SQRT - a safe square root */

double PM_sqrt(double x)
   {double rv;

    rv = sqrt(ABS(x));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LN - safe natural logarithm */

double PM_ln(double x)
   {double r;

    if (x > 0)
       r = log(x);
    else
       r = -HUGE;

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LOG - safe logarithm base 10 */

double PM_log(double x)
   {double r;

    if (x > 0)
       r = log10(x);
    else
       r = -HUGE;

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COT - safe cot routine */

double PM_cot(double x)
   {double r, rv;

    r = tan(x);

    rv = (r == 0.0) ? HUGE : 1.0/r;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COTH - safe coth routine */

double PM_coth(double x)
   {double r, rv;

    r = tanh(x);

    rv = (r == 0.0) ? HUGE : 1.0/r;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ACOS - safe acos routine */

double PM_acos(double x)
   {double o, r, rs, rb;

    o = 1.0;
    if (fabs(x) > 1.000001)
       return(-HUGE);

    rs = min(1.0, fabs(x));
    rb = SIGNF(o, x);
    r  = acos(rb*rs);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ASIN - safe asin routine */

double PM_asin(double x)
   {double o, r, rs, rb;

    o = 1.0;
    if (fabs(x) > 1.000001)
       return(-HUGE);

    rs = min(1.0, fabs(x));
    rb = SIGNF(o, x);
    r  = asin(rb*rs);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ATAN - return the arc tangent of Y/X in radians
 *         - using the signs of X and Y to determine the quadrant
 *         - the range is -PI < ang <= PI
 */

double PM_atan(double x, double y)
   {double a, r;

    if ((x == 0.0) && (y == 0.0))
       a = 0.0;

    else if (PM_LESS_LESS(x, y))
       a = (y > 0.0) ? PI/2.0 : -PI/2.0;

    else if (PM_LESS_LESS(y, x))
       a = (x > 0.0) ? 0.0 : PI;

    else
       {r = y/x;
	a = atan(r);

	if (x < 0.0)
           a += (r > 0.0) ? -PI : PI;};

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FLOOR - floor routine (largest integer less than x) */

double PM_floor(double x)
   {double r;

    r = floor(x);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SIGN2 - sign routine */

double PM_sign2(double x, double y)
   {double r;

    r = SIGNF(x, y);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_RECIP - a callable, safe inverse function */

double PM_recip(double x)
   {double rv;

    if (ABS(x) > SMALL)
       rv = 1.0/x;
    else
       rv = HUGE;

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PM_POW - smarter verson of pow */
 
double PM_pow(double x, double y)
   {double rv;

/* do not trust implementations (valgrind reveals)
 * handle trivial special cases
 */
    if (x == 0.0)
       rv = 0.0;
    else if (x == 1.0)
       rv = 1.0;
    else if (y == 0.0)
       rv = 1.0;
    else if (y == 1.0)
       rv = x;
    else
       rv = pow(ABS(x), y);

    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PM_IPOW - integer version of pow where n >= 0 */
 
long PM_ipow(int m, int n)
   {int nh;
    long mi;
 
    if (n == 0)
       mi = 1;

    else
       {nh = n >> 1;
	mi = PM_ipow(m, nh);
	mi = ((n % 2) == 0) ? mi*mi : mi*mi*m;};
 
    return(mi);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PM_POWER - fast version of pow for integer exponent */
 
double PM_power(double x, int p)
   {double y, t;
 
    if (p == 0)
       y = 1.0;

/* check for odd */
    else if (p & 1)
       {if (p > 0)
           y = x*PM_power(x, p-1);
        else
           y = PM_power(x, p+1)/x;}

    else
       {t = PM_power(x, (p >> 1));
	y = t*t;};
 
    return(y);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PM_SIGN - return the sign */
 
double PM_sign(double x)
   {double rv;

    if (x != 0.0)
       rv = ABS(x)/x;
    else
       rv = 1.0;
 
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PM_ROUND - round off the given number */
 
double PM_round(double x)
   {double rv;
    extern double round(double x);

/*    rv = PM_sign(x)*floor(ABS(x) + 0.5); */
    rv = round(x);
 
    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PM_FRAC - return the fractional part of the given number */
 
double PM_frac(double x)
   {double frac, rv;
 
    frac = 1.0;

    rv = fmod(x, frac);
 
    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FIX - truncates the fractional part of a double
 *        - the difference between this function and the C library's FLOOR is
 *        -
 *        - floor(-2.3)  = -3.0
 *        - PM_fix(-2.3) = -2.0
 */

double PM_fix(double v)
   {long iv;
    double rv;
    static double maxlv = (double) LONG_MAX;

    if (v < 0.0)
       {v  = max(v, -maxlv);
        iv = (long) ABS(v);
        rv = -((double) iv);}
    else
       {v  = min(v, maxlv);
        iv = (long) v;
        rv = (double) iv;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SGN - transfers the sign of sign to value */

double PM_sgn(double sign, double v)
   {

    v = ABS(v);
    if (sign < 0.0)
       v = -v;

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_HYPOT - return the root mean square value of the arguments */

double PM_hypot(double x, double y)
   {double r, rv;

    if (x == 0.0)
       rv = y;

    else if (y == 0.0)
       rv = x;

    else if (x < y)
       {r  = x/y;
	rv = y*sqrt(1.0 + r*r);}
       
    else
       {r  = y/x;
	rv = x*sqrt(1.0 + r*r);};
       
    rv = ABS(rv);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                              SPECIAL FUNCTIONS                           */

/*--------------------------------------------------------------------------*/

/* _PM_JN - Bessel function of the first kind primitive
 *        - using algorithm from Numerical Recipies in C
 */

static double _PM_jn(int n, double x)
   {int j, jtot, m;
    double ax, z, bj, bjm, bjp, tot, tox, ret;
    double xx, y, num, den, ct, st;

    ax = ABS(x);
    if (n == 0)
       {if (ax < 8.0)
           {y   = x*x;
	    num = PM_horner(y, _PM_j0_coeff_na, 0, 5);
            den = PM_horner(y, _PM_j0_coeff_da, 0, 5);
            ret = num/den;}
        else
           {z   = 8.0/ax;
            y   = z*z;
            xx  = ax - pi_1_4;
	    num = PM_horner(y, _PM_j0_coeff_nb, 0, 4);
	    den = PM_horner(y, _PM_j0_coeff_db, 0, 4);
            ct  = cos(xx)*num;
	    st  = z*sin(xx)*den;
            ret = sqrt(two_ov_pi/ax)*(ct - st);};}
       
    else if (n == 1)
       {if (ax < 8.0)
           {y   = x*x;
	    num = PM_horner(y, _PM_j1_coeff_na, 0, 5);
	    den = PM_horner(y, _PM_j1_coeff_da, 0, 5);
            ret = x*num/den;}
        else
           {z   = 8.0/ax;
            y   = z*z;
            xx  = ax - pi_3_4;
	    num = PM_horner(y, _PM_j1_coeff_nb, 0, 4);
	    den = PM_horner(y, _PM_j1_coeff_db, 0, 4);
            ct  = cos(xx)*num;
            st  = z*sin(xx)*den;
            ret = sqrt(two_ov_pi/ax)*(ct - st);
            if (x < 0.0)
               ret = -ret;};}

    else
       {if (ax == 0.0)
           return(0.0);

        else if (ax > (double) n)
           {tox = 2.0/ax;
            bjm = _PM_jn(0, ax);
            bj  = _PM_jn(1, ax);
            for (j = 1; j < n; j++)
                {bjp = j*tox*bj - bjm;
                 bjm = bj;
                 bj  = bjp;};
            ret = bj;}

        else
           {tox  = 2.0/ax;
            m    = 2*((n + ((int) sqrt(BESS_ACC*n))) >> 1);
            jtot = 0;
            bjp  = 0.0;
            ret  = 0.0;
            tot  = 0.0;
            bj   = 1.0;
            for (j = m; j > 0; j--)
                {bjm = j*tox*bj - bjp;
                 bjp = bj;
                 bj  = bjm;
                 if (ABS(bj) > HUGE)
                    {bj  *= SMALL;
                     bjp *= SMALL;
                     ret *= SMALL;
                     tot *= SMALL;};

                 if (jtot)
                    tot +=  bj;
                 jtot = !jtot;
                 if (j == n)
                    ret = bjp;};

            tot  = 2.0*tot - bj;
            ret /= tot;};

        ret = ((x < 0.0) && ((n % 2) == 1)) ? -ret : ret;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_YN - Bessel function of the second kind
 *        - algorithm from Numerical Recipes in C
 */

double _PM_yn(int n, double x)
   {int j;
    double by, bym, byp, tox;
    double z, ct, st, jnc, rat;
    double xx, y, ret, num, den;

    if (n == 0)
       {if (x < 8.0)
           {y   = x*x;
	    num = PM_horner(y, _PM_y0_coeff_na, 0, 5);
            den = PM_horner(y, _PM_y0_coeff_da, 0, 5);
            jnc = two_ov_pi*_PM_jn(0, x)*log(x);
            rat = num/den;
            ret = rat + jnc;}
        else
           {z   = 8.0/x;
            y   = z*z;
            xx  = x - pi_1_4;
	    num = PM_horner(y, _PM_y0_coeff_nb, 0, 4);
            den = PM_horner(y, _PM_y0_coeff_db, 0, 4);
            ct  = z*cos(xx)*den;
            st  = sin(xx)*num;
            ret = sqrt(two_ov_pi/x)*(st + ct);};}

    else if (n == 1)
       {if (x < 8.0)
           {y   = x*x;
	    num = PM_horner(y, _PM_y1_coeff_na, 0, 5);
            den = PM_horner(y, _PM_y1_coeff_da, 0, 6);
            jnc = two_ov_pi*(_PM_jn(1, x)*log(x) - 1.0/x);
            rat = x*(num/den);
            ret = rat + jnc;}
        else
           {z   = 8.0/x;
            y   = z*z;
            xx  = x - pi_3_4;
	    num = PM_horner(y, _PM_y1_coeff_nb, 0, 4);
            den = PM_horner(y, _PM_y1_coeff_db, 0, 4);

            ct  = z*cos(xx)*den;
            st  = sin(xx)*num;

            ret = sqrt(two_ov_pi/x)*(st + ct);};}

    else
       {tox = 2.0/x;
        by  = _PM_yn(1, x);
        bym = _PM_yn(0, x);
        for (j = 1; j < n; j++)
            {byp = j*tox*by - bym;
             bym = by;
             by  = byp;};

        ret = by;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_J0 - j0 */

double PM_j0(double x)
   {double r;

#ifdef BESSEL_JN

    r = _PM_jn(0, x);

#else

    if (_PM_use_pml_bessel)
       r = _PM_jn(0, x);
    else
       r = j0(x);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_J1 - j1 */

double PM_j1(double x)
   {double r;

#ifdef BESSEL_JN

    r = _PM_jn(1, x);

#else

    if (_PM_use_pml_bessel)
       r = _PM_jn(1, x);
    else
       r = j1(x);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_JN - Bessel function of 1rst kind */

double PM_jn(double x, double n)
   {double r;

#ifdef BESSEL_JN

    r = _PM_jn(n, x);

#else

    if (_PM_use_pml_bessel)
       r = _PM_jn(n, x);
    else
       r = jn((int) n, x);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_Y0 - y0 */

double PM_y0(double x)
   {double r;

#ifdef BESSEL_YN

    r = _PM_yn(0, x);

#else

    if (_PM_use_pml_bessel)
       r = _PM_yn(0, x);
    else
       r = y0(x);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_Y1 - y1 */

double PM_y1(double x)
   {double r;

#ifdef BESSEL_YN

    r = _PM_yn(1, x);

#else

    if (_PM_use_pml_bessel)
       r = _PM_yn(1, x);
    else
       r = y1(x);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_YN - Bessel function of 2nd kind */

double PM_yn(double x, double n)
   {double r;

#ifdef BESSEL_YN

    r = _PM_yn((int) n, x);

#else

    if (_PM_use_pml_bessel)
       r = _PM_yn((int) n, x);
    else
       r = yn((int) n, x);

#endif

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_IN - modified Bessel function of the first kind
 *       - algorithm from Numerical Recipes in C
 */

double PM_in(double x, double nd)
   {int j, n, ns;
    double bi, bim, bip, tox, ax;
    double z, y, ret, a1;

    n = nd;

    if (n == 0)
       {ax = ABS(x);
	if (ax < 3.75)
           {z   = x/3.75;
	    y   = z*z;
	    ret = PM_horner(y, _PM_i0_coeff_na, 0, 6);}
        else
           {y   = 3.75/ax;
	    a1  = PM_horner(y, _PM_i0_coeff_nb, 0, 8);
            ret = a1*exp(ax)/sqrt(ax);};}

    else if (n == 1)
       {ax = ABS(x);
	if (ax < 3.75)
           {z   = x/3.75;
	    y   = z*z;
	    ret = ax*PM_horner(y, _PM_i1_coeff_na, 0, 6);}
        else
           {y   = 3.75/ax;
	    a1  = PM_horner(y, _PM_i1_coeff_nb, 0, 8);
            ret = a1*exp(ax)/sqrt(ax);};}

    else if (x == 0.0)
      ret = x;

    else
       {tox = 2.0/ABS(x);
        bi  = 1.0;
	bip = 0.0;
        ret = 0.0;
	ns  = 2*(n + (int) sqrt(BESS_ACC*n));
	for (j = ns; 0 < j; j--)
	    {bim = bip + j*tox*bi;
             bip = bi;
	     bi  = bim;
	     if (ABS(bi) > HUGE)
	        {ret *= SMALL;
		 bi  *= SMALL;
		 bip *= SMALL;};
	     if (j == n)
	        ret = bip;};

	ret *= (PM_in(x, 0.0)/bi);
	ret  = ((x < 0.0) && (n % 2 == 1)) ? -ret : ret;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_I0 - i0 */

double PM_i0(double x)
   {double r;

    r = PM_in(x, 0.0);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_I1 - i1 */

double PM_i1(double x)
   {double r;

    r = PM_in(x, 1.0);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_KN - modified Bessel function of the second kind
 *       - algorithm from Numerical Recipes in C
 */

double PM_kn(double x, double nd)
   {int j, n;
    double bk, bkm, bkp, tox;
    double z, y, ret, a1, a2;

    n = nd;

    if (n == 0)
       {if (x <= 2.0)
           {z   = x/2.0;
	    y   = z*z;
	    a1  = PM_horner(y, _PM_k0_coeff_na, 0, 6);
	    a2  = log(z)*PM_in(x, 0.0);
	    ret = a1 - a2;}
        else
           {y   = 2.0/x;
	    a1  = PM_horner(y, _PM_k0_coeff_nb, 0, 6);
            a2  = exp(-x)/sqrt(x);
            ret = a1*a2;};}

    else if (n == 1)
       {if (x <= 2.0)
           {z   = x/2.0;
	    y   = z*z;
	    a1  = PM_horner(y, _PM_k1_coeff_na, 0, 6);
	    a2  = log(z)*PM_in(x, 1.0);
	    ret = a1/x + a2;}
        else
           {y   = 2.0/x;
	    a1  = PM_horner(y, _PM_k1_coeff_nb, 0, 6);
            a2  = exp(-x)/sqrt(x);
            ret = a1*a2;};}

    else
       {tox = 2.0/ABS(x);
        bkm = PM_kn(x, 0.0);
	bk  = PM_kn(x, 1.0);
        ret = 0.0;
	for (j = 1; j < n; j++)
	    {bkp = bkm + j*tox*bk;
             bkm = bk;
	     bk  = bkp;};

	ret = bk;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_K0 - k0 */

double PM_k0(double x)
   {double r;

    r = PM_kn(x, 0.0);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_K1 - k1 */

double PM_k1(double x)
   {double r;

    r = PM_kn(x, 1.0);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_TCHN - Tchebyshev polynomial of order N */

double PM_tchn(double x, double n)
   {int i, m;
    double ta, tb, tc;

    if ((x < -1.0) || (1.0 < x))
       tc = -HUGE;

    else
       {ta = 1.0;
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
	         tc = -HUGE;
	         for (i = m-1; i > 0; i--)
		     {tc = 2*x*tb - ta;
		      ta = tb;
		      tb = tc;};
		 break;};};

    return(tc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

