/*
 * MLQFNC.C - quarternion math functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"
#include "scope_quaternion.h"
#include "scope_math.h"

/*
 * Quaternion q = ( q.s + q.i*I + q.j*J + q.k*K)
 * define     s = q.s
 *            V = (q.i*I + q.j*J + q.k*K);
 *            a = sqrt(q.i^2 + q.j^2 + q.k^2) = sqrt(V.V)
 *            U = V/a
 * Note that U.U = -1
 *
 * then       q  = s + aU
 *            q* = s - aU
 *
 *    e^q   = e^s*(cos(a) + U*sin(a))
 *    e^q*  = e^s*(cos(a) - U*sin(a))
 *
 * This is derived exactly the same way as for complex numbers
 * from the power series since both use the fact that i^2 = -1
 * or U^2 = -1
 *
 * The following are derived similarly with the complex numbers
 * using identities and power series
 *
 *   cos(aU) = cosh(a)
 *   sin(aU) = Usinh(a)
 *
 *   cos(q) = cos(s + aU) = cos(s)cos(aU) - sin(s)sin(aU)
 *          = cos(s)cosh(a) - Usin(s)sinh(a)
 *   sin(q) = sin(s + aU) = cos(s)sin(aU) + sin(s)cos(aU)
 *          = sin(s)cosh(a) + Ucos(s)sinh(a)
 *   tan(q) = sin(q)/cos(q)
 *
 *   cosh(q) = cosh(s)cos(a) + Usinh(s)sin(a)
 *   sinh(q) = sinh(s)cos(a) + Ucosh(s)sin(a)
 *   tanh(q) = sinh(q)/cosh(q)
 *
 *   acosh(q) = ln(q + sqrt(q^2 - 1))
 *   asinh(q) = ln(q + sqrt(q^2 + 1))
 *   atanh(q) = 0.5*ln((1 + q)/(1 - q))
 *
 *   acos(q) = -Uacosh(q)
 *   asin(q) = -Uasinh(qU)
 *   atan(q) = -Uatanh(qU)
 *
 */

#define PM_qabs PM_qnorm

quaternion
 QPHUGE = { HUGE, 0.0, 0.0, 0.0 },
 QMHUGE = { -HUGE, 0.0, 0.0, 0.0 },
 Qone   = { 1.0, 0.0, 0.0, 0.0 },
 Qzero  = { 0.0, 0.0, 0.0, 0.0 };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QUATERNION - return a quaternion constructed from components */

quaternion PM_quaternion(double s, double i, double j, double k)
   {quaternion q;

    q.s = s;
    q.i = i;
    q.j = j;
    q.k = k;

    return(q);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_PLUS_QQ - add quaternion A and B */

quaternion PM_plus_qq(quaternion a, quaternion b)
   {quaternion x;

    x.s = a.s + b.s;
    x.i = a.i + b.i;
    x.j = a.j + b.j;
    x.k = a.k + b.k;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_PLUS_RQ - add double R to quaternion A */

quaternion PM_plus_rq(double r, quaternion a)
   {quaternion x;

    x   = a;
    x.s = r + a.s;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MINUS_QQ - subtract quaternion B from A */

quaternion PM_minus_qq(quaternion a, quaternion b)
   {quaternion x;

    x.s = a.s - b.s;
    x.i = a.i - b.i;
    x.j = a.j - b.j;
    x.k = a.k - b.k;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_TIMES_QQ - multilply quaternion A by B */

quaternion PM_times_qq(quaternion a, quaternion b)
   {quaternion x;

    x.s = (a.s)*(b.s) - (a.i)*(b.i) - (a.j)*(b.j) - (a.k)*(b.k);
    x.i = (a.s)*(b.i) + (a.i)*(b.s) + (a.j)*(b.k) - (a.k)*(b.j);
    x.j = (a.s)*(b.j) - (a.i)*(b.k) + (a.j)*(b.s) + (a.k)*(b.i);
    x.k = (a.s)*(b.k) + (a.i)*(b.j) - (a.j)*(b.i) + (a.k)*(b.s);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_TIMES_RQ - multilply quaternion A by R */

quaternion PM_times_rq(double r, quaternion a)
   {quaternion x;

    x.s = r*a.s;
    x.i = r*a.i;
    x.j = r*a.j;
    x.k = r*a.k;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QCONJUGATE - return the conjugate A* of quaternion A */

quaternion PM_qconjugate(quaternion a)
   {quaternion x;

    x.s = a.s;
    x.i = -a.i;
    x.j = -a.j;
    x.k = -a.k;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QVNORM - return the norm of the imaginary part of A */

double PM_qvnorm(quaternion a)
   {double v;

    v = sqrt(a.i*a.i + a.j*a.j + a.k*a.k);

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QVUNIT - return the normalized imaginary part of A */

quaternion PM_qvunit(quaternion a)
   {double v;
    quaternion q;

    v = sqrt(a.i*a.i + a.j*a.j + a.k*a.k);

    q.s = 0.0;
    q.i = v*a.i;
    q.j = v*a.j;
    q.k = v*a.k;

    return(q);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QNORM - return the norm of quaternion A */

double PM_qnorm(quaternion a)
   {double d;

    d = sqrt(a.s*a.s + a.i*a.i + a.j*a.j + a.k*a.k);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QRECIP - return the reciprocal of quaternion A
 *           - defined to be (A*)/|A|^2
 *           - as opposed to the versor
 */

quaternion PM_qrecip(quaternion a)
   {double d;
    quaternion x;

    d = 1.0/(a.s*a.s + a.i*a.i + a.j*a.j + a.k*a.k);
    x = PM_times_rq(d, PM_qconjugate(a));

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QVERSOR - return the versor of quaternion A
 *            - defined to be A/|A|
 *            - as opposed to the reciprocal
 */

quaternion PM_qversor(quaternion a)
   {double d;
    quaternion x;

    d = 1.0/PM_qnorm(a);
    x = PM_times_rq(d, a);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DISTANCE_QQ - return the distance between quaternion A and B */

double PM_distance_qq(quaternion a, quaternion b)
   {double d;

    d = sqrt(PM_qnorm(PM_minus_qq(a, b)));

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LQUOTIENT_QQ - return the left quotient of quaternion A and B */

quaternion PM_lquotient_qq(quaternion a, quaternion b)
   {quaternion x;

    x = PM_times_qq(PM_qrecip(b), a);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_RQUOTIENT_QQ - return the right quotient of quaternion A and B */

quaternion PM_rquotient_qq(quaternion a, quaternion b)
   {quaternion x;

    x = PM_times_qq(a, PM_qrecip(b));

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QEXP - return the exp(A) */

quaternion PM_qexp(quaternion a)
   {double ea, cv, sv, vf, v;
    quaternion x;

    v = PM_qvnorm(a);

    ea = exp(a.s);
    cv = cos(v);
    sv = sin(v);
    vf = ea*sv/v;

    x.s = ea*cv;
    x.i = vf*a.i;
    x.j = vf*a.j;
    x.k = vf*a.k;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QLN - return the ln(A) */

quaternion PM_qln(quaternion a)
   {double ac, as, vf, v, d;
    quaternion x;

    v = PM_qvnorm(a);
    d = PM_qnorm(a);

    as = log(d);
    ac = acos(a.i/d);
    vf = ac/v;

    x.s = as;
    x.i = vf*a.i;
    x.j = vf*a.j;
    x.k = vf*a.k;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QLOG - quaternion log10 function */

quaternion PM_qlog(quaternion a)
   {quaternion r;

    if (_PM.ln10e == 0.0)
       _PM.ln10e = log10(exp(1.0));

    r = PM_qln(a);
    r = PM_times_rq(_PM.ln10e, r);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QSQR - quaternion sqr function */

quaternion PM_qsqr(quaternion a)
   {quaternion r;

    r = PM_times_qq(a, a);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QSQRT - quaternion sqrt function */

quaternion PM_qsqrt(quaternion a)
   {quaternion r;

    r = PM_qrpow(a, 0.5);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QRPOW - return the A^R */

quaternion PM_qrpow(quaternion a, double r)
   {quaternion x;

    x = PM_qexp(PM_times_rq(r, PM_qln(a)));

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QPOW - return the A^R */

quaternion PM_qpow(quaternion a, quaternion r)
   {quaternion x;

    x = PM_qexp(PM_times_qq(r, PM_qln(a)));

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QSIN - quaternion sin function */

quaternion PM_qsin(quaternion q)
   {double s, a, cs;
    quaternion r, u;

    s  = q.s;
    a  = PM_qvnorm(q);
    u  = PM_qvunit(q);
    cs = cos(s)*sinh(a);

    r.s = sin(s)*cosh(a);
    r.i = cs*u.i;
    r.j = cs*u.j;
    r.k = cs*u.k;

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QCOS - quaternion cos function */

quaternion PM_qcos(quaternion q)
   {double s, a, cs;
    quaternion r, u;

    s  = q.s;
    a  = PM_qvnorm(q);
    u  = PM_qvunit(q);
    cs = -sin(s)*sinh(a);

    r.s = cos(s)*cosh(a);
    r.i = cs*u.i;
    r.j = cs*u.j;
    r.k = cs*u.k;

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QTAN - quaternion tan function */

quaternion PM_qtan(quaternion q)
   {quaternion r;

    r = PM_rquotient_qq(PM_qsin(q), PM_qcos(q));

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QCOT - quaternion cot function */

quaternion PM_qcot(quaternion q)
   {quaternion r;

    r = PM_rquotient_qq(PM_qcos(q), PM_qsin(q));

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QSINH - quaternion sinh function */

quaternion PM_qsinh(quaternion q)
   {double s, a, cs;
    quaternion r, u;

    s  = q.s;
    a  = PM_qvnorm(q);
    u  = PM_qvunit(q);
    cs = cosh(s)*sin(a);

    r.s = sinh(s)*cos(a);
    r.i = cs*u.i;
    r.j = cs*u.j;
    r.k = cs*u.k;

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QCOSH - quaternion cosh function */

quaternion PM_qcosh(quaternion q)
   {double s, a, cs;
    quaternion r, u;

    s  = q.s;
    a  = PM_qvnorm(q);
    u  = PM_qvunit(q);
    cs = sinh(s)*sin(a);

    r.s = cosh(s)*cos(a);
    r.i = cs*u.i;
    r.j = cs*u.j;
    r.k = cs*u.k;

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QTANH - quaternion tanh function */

quaternion PM_qtanh(quaternion q)
   {quaternion r;

    r = PM_rquotient_qq(PM_qsinh(q), PM_qcosh(q));

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QCOTH - quaternion coth function */

quaternion PM_qcoth(quaternion q)
   {quaternion r;

    r = PM_rquotient_qq(PM_qcosh(q), PM_qsinh(q));

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QASIN - quaternion asin function
 *          -   asin(q) = -U*asinh(q)
 */

quaternion PM_qasin(quaternion q)
   {quaternion r, u;

    u = PM_qvunit(q);
    r = PM_times_rq(-1.0, PM_times_qq(u, PM_qasinh(q)));

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QACOS - quaternion acos function
 *          -   acos(q) = -U*acosh(qU)
 */

quaternion PM_qacos(quaternion q)
   {quaternion r, u;

    u = PM_qvunit(q);
    r = PM_times_rq(-1.0, PM_times_qq(u, PM_qacosh(PM_times_qq(q, u))));

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QATAN - quaternion atan function
 *          -   atan(q) = -Uatanh(qU)
 */

quaternion PM_qatan(quaternion q)
   {quaternion r, u;

    u = PM_qvunit(q);
    r = PM_times_rq(-1.0, PM_times_qq(u, PM_qatanh(PM_times_qq(q, u))));

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QASINH - quaternion asinh function
 *           -   asinh(q) = ln(q + sqrt(q*q + 1))	
 */

quaternion PM_qasinh(quaternion q)
   {quaternion r;

    r = PM_times_qq(q, q);
    r = PM_plus_rq(1.0, r);
    r = PM_qsqrt(r);
    r = PM_plus_qq(q, r);
    r = PM_qln(r);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QACOSH - quaternion acosh function
 *           -   acosh(q) = ln(q + sqrt(q*q - 1))
 */

quaternion PM_qacosh(quaternion q)
   {quaternion r;

    r = PM_times_qq(q, q);
    r = PM_plus_rq(-1.0, r);
    r = PM_qsqrt(r);
    r = PM_plus_qq(q, r);
    r = PM_qln(r);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QATANH - quaternion atanh function
 *           -    atanh(q) = ln((1 + q)/(1 - q))/2
 */

quaternion PM_qatanh(quaternion q)
   {quaternion r;
    quaternion q1, q2;

    q1 = PM_plus_rq(1.0, q);
    q2 = PM_plus_rq(-1.0, q);
    r  = PM_rquotient_qq(q1, q2);
    r  = PM_qln(r);
    r  = PM_times_rq(0.5, r);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QTCHN - Tchebyshev polynomial of order N */

quaternion PM_qtchn(quaternion x, double n)
   {int i, m;
    double r;
    quaternion ta, tb, tc;

    r = PM_qabs(x);
    if (1.0 < r)
       tc = QMHUGE;

    else
       {ta = Qone;
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
	         tc = QMHUGE;
	         for (i = m-1; i > 0; i--)
		     {tc = PM_times_qq(x, tb);
		      tc = PM_times_rq(2, tc);
		      tc = PM_minus_qq(tc, ta);
		      ta = tb;
		      tb = tc;};
		 break;};};

    return(tc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ROTATE_Q - rotate V about U by an angle THETA
 *             - the hyperspherical space of 3D rotations can be
 *             - parameterized by the three Euler angles
 *             - but any such parameterization is degenerate at
 *             - some points on the hypersphere
 *             - this can be avoided by using four Euclidean coordinates
 *             - w,x,y,z, with w^2 + x^2 + y^2 + z^2 = 1
 *             - the point (w,x,y,z) represents a rotation around the
 *             - axis defined by the vector (x,y,z) by an angle 
 *             - 
 *             -   theta = 2*acos(w) = 2*asin(sqrt(x^2 + y^2 + z^2))
 *             - 
 *             - define the quaternion
 *             - 
 *             -   q = w + xI + yJ + zK
 *             -     = w + (x,y,z)
 *             -     = cos(theta/2) + U*sin(theta/2)
 *             - 
 *             - where U is a unit vector
 *             - let V be an ordinary vector in 3 dimensional space,
 *             - considered as a quaternion with a real coordinate 0
 *             - then it can be shown that the quaternion product
 *             - 
 *             -   V' = q V q^-1
 *             - 
 *             - yields the vector rotated by theta about the axis U
 *             - the rotation is clockwise if the line of sight points
 *             - in the direction pointed by U
 *             - 
 *             - http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
 */

void PM_rotate_q(double *v, double *u, double th)
   {int i;
    double ct, st, vs;
    quaternion vp, vq, rq;

    vq.s = 0;
    vq.i = v[0];
    vq.j = v[1];
    vq.k = v[2];

    vs = 0.0;
    for (i = 0; i < 3 ; i++)
        vs += u[i]*u[i];
    st = sin(0.5*th)/sqrt(vs);
    ct = cos(0.5*th);

    rq.s = ct;
    rq.i = st*u[0];
    rq.j = st*u[1];
    rq.k = st*u[2];

    vp = PM_times_qq(rq, PM_times_qq(vq, PM_qrecip(rq)));

    v[0] = vp.i;
    v[1] = vp.j;
    v[2] = vp.k;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QHORNER - evaluate polynomial using Horner's rule
 *            -   y = Sum(Ci*X^i, mn <= i <= mx)
 */

quaternion PM_qhorner(quaternion x, double *c, int mn, int mx)
   {int i, in, iz, ix;
    quaternion r, rn, r0, rp, xi;

    in = min(mn, mx);
    ix = max(mn, mx);

    iz = max(in, 0);

/* grab the constant coefficient */
    if ((in <= iz) && (iz <= ix))
       r0 = PM_quaternion(c[iz-in], 0.0, 0.0, 0.0);
    else
       r0 = Qzero;

/* do the positive powers */
    rp = Qzero;
    for (i = ix; iz < i; i--)
        {rp = PM_plus_rq(c[i-in], rp);
	 rp = PM_times_qq(x, rp);};

/* do the negative powers */
    if (PM_qequal(x, Qzero) && (in < 0))
       rn = QMHUGE;

    else
       {xi = PM_qrecip(x);
	rn = Qzero;
	for (i = in; i < iz; i++)
	    {rn = PM_plus_rq(c[i-in], rn);
	     rn = PM_times_qq(xi, rn);};};

    r = PM_plus_qq(rn, rp);
    r = PM_plus_qq(r, r0);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QRANDOM - return unit random quaternion */

quaternion PM_qrandom(quaternion a)
   {double tp, x, y, cx, sx, v, vf;

    tp = 2.0*PI;

    a.i = PM_random(0.0);
    a.j = PM_random(0.0);
    a.k = PM_random(0.0);

    y = PM_random(0.0);
    x = tp*y;

    cx = cos(x);
    sx = sin(x);

    v  = PM_qvnorm(a);
    vf = sx/v;

    a.s = cx;
    a.i = vf*a.i;
    a.j = vf*a.j;
    a.k = vf*a.k;

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QEQUAL - return TRUE iff a == b */

int PM_qequal(quaternion a, quaternion b)
   {int eq;

    eq  = (a.s == b.s);
    eq &= (a.i == b.i);
    eq &= (a.j == b.j);
    eq &= (a.k == b.k);

    return(eq);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QCLOSE - return TRUE iff a == b to within machine precision */

int PM_qclose(quaternion a, quaternion b, double tol)
   {int eq;

    PM_CLOSETO_QUATERNION(eq, a, b, tol);

    return(eq);}

/*--------------------------------------------------------------------------*/

/*                      QUATERNION BESSEL FUNCTIONS                         */

/*--------------------------------------------------------------------------*/

/* PM_QJN - quaternion Bessel function of the first kind primitive
 *        - using algorithm from Numerical Recipies in C
 */

quaternion PM_qjn(quaternion x, double nd)
   {int j, jtot, m, n;
    double ax;
    quaternion s, z, bj, bjm, bjp, tot, tox, ret;
    quaternion xx, y, num, den, ct, st;

    n  = nd;
    ax = PM_qabs(x);
    if (n == 0)
       {if (ax < 8.0)
           {y   = PM_qsqr(x);
	    num = PM_qhorner(y, _PM_j0_coeff_na, 0, 5);
            den = PM_qhorner(y, _PM_j0_coeff_da, 0, 5);
            ret = PM_rquotient_qq(num, den);}

        else
           {z   = PM_qrecip(x);
	    s   = PM_qsqrt(PM_times_rq(two_ov_pi, z));
	    z   = PM_times_rq(8.0, z);
            y   = PM_qsqr(z);
            xx  = PM_plus_rq(-pi_1_4, x);

	    num = PM_qhorner(y, _PM_j0_coeff_nb, 0, 4);
	    den = PM_qhorner(y, _PM_j0_coeff_db, 0, 4);

	    ct = PM_qcos(xx);
	    ct = PM_times_qq(num, ct);

	    st = PM_qsin(xx);
	    st = PM_times_qq(den, st);
	    st = PM_times_qq(z, st);

	    ret = PM_minus_qq(ct, st);
	    ret = PM_times_qq(s, ret);};}
       
    else if (n == 1)
       {if (ax < 8.0)
           {y   = PM_qsqr(x);
	    num = PM_qhorner(y, _PM_j1_coeff_na, 0, 5);
	    den = PM_qhorner(y, _PM_j1_coeff_da, 0, 5);

            ret = PM_rquotient_qq(num, den);
            ret = PM_times_qq(x, ret);}

        else
           {z   = PM_qrecip(x);
	    s   = PM_qsqrt(PM_times_rq(two_ov_pi, z));
	    z   = PM_times_rq(8.0, z);
            y   = PM_qsqr(z);
            xx  = PM_plus_rq(-pi_3_4, x);

	    num = PM_qhorner(y, _PM_j1_coeff_nb, 0, 4);
	    den = PM_qhorner(y, _PM_j1_coeff_db, 0, 4);

	    ct = PM_qcos(xx);
	    ct = PM_times_qq(num, ct);

	    st = PM_qsin(xx);
	    st = PM_times_qq(den, st);
	    st = PM_times_qq(z, st);

	    ret = PM_minus_qq(ct, st);
	    ret = PM_times_qq(s, ret);};}

    else
       {if (ax == 0.0)
           return(Qzero);

        else if (ax > (double) n)
           {tox = PM_qrecip(x);
	    tox = PM_times_rq(2.0, tox);
            bjm = PM_qjn(x, 0.0);
            bj  = PM_qjn(x, 1.0);
            for (j = 1; j < n; j++)
                {bjp = PM_times_qq(tox, bj);
		 bjp = PM_times_rq(j, bjp);
		 bjp = PM_minus_qq(bjp, bjm);
                 bjm = bj;
                 bj  = bjp;};
            ret = bj;}

        else
           {tox = PM_qrecip(x);
	    tox = PM_times_rq(2.0, tox);

            m    = 2*((n + ((int) sqrt(BESS_ACC*n))) >> 1);
            jtot = 0;
            bjp  = Qzero;
            ret  = Qzero;
            tot  = Qzero;
            bj   = Qone;
            for (j = m; j > 0; j--)
                {bjm = PM_times_qq(tox, bj);
		 bjm = PM_times_rq(j, bjm);
		 bjm = PM_minus_qq(bjm, bjp);
                 bjp = bj;
                 bj  = bjm;
                 if (PM_qabs(bj) > HUGE)
                    {bj  = PM_times_rq(SMALL, bj);
                     bjp = PM_times_rq(SMALL, bjp);
                     ret = PM_times_rq(SMALL, ret);
                     tot = PM_times_rq(SMALL, tot);};

                 if (jtot)
                    tot = PM_plus_qq(tot, bj);
                 jtot = !jtot;
                 if (j == n)
                    ret = bjp;};

            tot = PM_times_rq(2.0, tot);
	    tot = PM_minus_qq(tot, bj);
            ret = PM_rquotient_qq(ret, tot);};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QJ0 - quaternion j0 */

quaternion PM_qj0(quaternion x)
   {quaternion r;

    r = PM_qjn(x, 0.0);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QJ1 - quaternion j1 */

quaternion PM_qj1(quaternion x)
   {quaternion r;

    r = PM_qjn(x, 1.0);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QYN - Bessel function of the second kind
 *        - algorithm from Numerical Recipes in C
 */

quaternion PM_qyn(quaternion x, double nd)
   {int j, n;
    double ax;
    quaternion by, bym, byp, ct, st, tox;
    quaternion z, s, jnc, lnc, rat, ret;
    quaternion xi, xx, y, num, den;

    n  = nd;
    ax = PM_qabs(x);
    if (n == 0)
       {if (ax < 8.0)
           {y   = PM_qsqr(x);
	    num = PM_qhorner(y, _PM_y0_coeff_na, 0, 5);
            den = PM_qhorner(y, _PM_y0_coeff_da, 0, 5);

            jnc = PM_qjn(x, 0.0);
	    lnc = PM_qln(x);
            rat = PM_rquotient_qq(num, den);

            ret = PM_times_qq(jnc, lnc);
            ret = PM_times_rq(two_ov_pi, ret);

            ret = PM_plus_qq(rat, ret);}

        else
           {z   = PM_qrecip(x);
	    s   = PM_qsqrt(PM_times_rq(two_ov_pi, z));
	    z   = PM_times_rq(8.0, z);
            y   = PM_qsqr(z);
            xx  = PM_plus_rq(-pi_1_4, x);

	    num = PM_qhorner(y, _PM_y0_coeff_nb, 0, 4);
            den = PM_qhorner(y, _PM_y0_coeff_db, 0, 4);

	    ct = PM_qcos(xx);
	    ct = PM_times_qq(den, ct);
	    ct = PM_times_qq(z, ct);

	    st = PM_qsin(xx);
	    st = PM_times_qq(st, num);

	    ret = PM_plus_qq(ct, st);
	    ret = PM_times_qq(s, ret);};}

    else if (n == 1)
       {if (ax < 8.0)
           {y   = PM_qsqr(x);
	    xi  = PM_qrecip(x);
	    num = PM_qhorner(y, _PM_y1_coeff_na, 0, 5);
            den = PM_qhorner(y, _PM_y1_coeff_da, 0, 6);

            jnc = PM_qjn(x, 1.0);
	    lnc = PM_qln(x);
            rat = PM_rquotient_qq(num, den);
            rat = PM_times_qq(x, rat);

            ret = PM_times_qq(jnc, lnc);
            ret = PM_minus_qq(ret, xi);
            ret = PM_times_rq(two_ov_pi, ret);

            ret = PM_plus_qq(rat, ret);}

        else
           {z   = PM_qrecip(x);
	    s   = PM_qsqrt(PM_times_rq(two_ov_pi, z));
	    z   = PM_times_rq(8.0, z);
            y   = PM_qsqr(z);
            xx  = PM_plus_rq(-pi_3_4, x);

	    num = PM_qhorner(y, _PM_y1_coeff_nb, 0, 4);
            den = PM_qhorner(y, _PM_y1_coeff_db, 0, 4);

	    ct = PM_qcos(xx);
	    ct = PM_times_qq(den, ct);
	    ct = PM_times_qq(z, ct);

	    st = PM_qsin(xx);
	    st = PM_times_qq(num, st);

	    ret = PM_plus_qq(ct, st);
	    ret = PM_times_qq(s, ret);};}

    else
       {tox = PM_qrecip(x);
	tox = PM_times_rq(2.0, tox);

        by  = PM_qyn(x, 1.0);
        bym = PM_qyn(x, 0.0);
        for (j = 1; j < n; j++)
            {byp = PM_times_qq(tox, by);
	     byp = PM_times_rq(j, byp);
             byp = PM_minus_qq(byp, bym);

             bym = by;
             by  = byp;};

        ret = by;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QY0 - y0 */

quaternion PM_qy0(quaternion x)
   {

    return(PM_qyn(x, 0.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QY1 - y1 */

quaternion PM_qy1(quaternion x)
   {

    return(PM_qyn(x, 1.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QIN - modified Bessel function of the first kind
 *        - algorithm from Numerical Recipes in C
 */

quaternion PM_qin(quaternion x, double nd)
   {int j, n, ns;
    double ax;
    quaternion bi, bim, bip, tox;
    quaternion z, y, ret, a1, a2;

    n  = nd;
    ax = PM_qabs(x);
    if (n == 0)
       {if (ax < 3.75)
           {z   = PM_times_rq(1.0/3.75, x);
	    y   = PM_qsqr(z);
	    ret = PM_qhorner(y, _PM_i0_coeff_na, 0, 6);}
        else
           {y   = PM_qrecip(x);
	    y   = PM_times_rq(3.75, y);
	    a1  = PM_qhorner(y, _PM_i0_coeff_nb, 0, 8);
            a2  = PM_qexp(x);
            a2  = PM_times_qq(a1, a2);
	    a1  = PM_qsqrt(x);
	    ret = PM_rquotient_qq(a2, a1);};}

    else if (n == 1)
       {if (ax < 3.75)
           {z   = PM_times_rq(1.0/3.75, x);
	    y   = PM_qsqr(z);
	    a1  = PM_qhorner(y, _PM_i1_coeff_na, 0, 6);
	    ret = PM_times_qq(x, a1);}
        else
           {y   = PM_qrecip(x);
	    y   = PM_times_rq(3.75, y);
	    a1  = PM_qhorner(y, _PM_i1_coeff_nb, 0, 8);
            a2  = PM_qexp(x);
            a2  = PM_times_qq(a1, a2);
	    a1  = PM_qsqrt(x);
	    ret = PM_rquotient_qq(a2, a1);};}

    else if (ax == 0.0)
      ret = x;

    else
       {tox = PM_qrecip(x);
	tox = PM_times_rq(2.0, tox);
        bi  = Qone;
	bip = Qzero;
        ret = Qzero;
	ns  = 2*(n + (int) sqrt(BESS_ACC*n));
	for (j = ns; 0 < j; j--)
	    {bim = PM_times_qq(tox, bi);
	     bim = PM_times_rq(j, bim);
	     bim = PM_plus_qq(bip, bim);
             bip = bi;
	     bi  = bim;
	     if (PM_qabs(bi) > HUGE)
	        {ret = PM_times_rq(SMALL, ret);
		 bi  = PM_times_rq(SMALL, bi);
		 bip = PM_times_rq(SMALL, bip);};
	     if (j == n)
	        ret = bip;};

	a1  = PM_qin(x, 0.0);
	a2  = PM_rquotient_qq(a1, bi);
	ret = PM_times_qq(ret, a2);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_qI0 - i0 */

quaternion PM_qi0(quaternion x)
   {

    return(PM_qin(x, 0.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_qI1 - i1 */

quaternion PM_qi1(quaternion x)
   {

    return(PM_qin(x, 1.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_qKN - modified Bessel function of the second kind
 *        - algorithm from Numerical Recipes in C
 */

quaternion PM_qkn(quaternion x, double nd)
   {int j, n;
    double ax;
    quaternion bk, bkm, bkp, tox;
    quaternion z, y, ret, a1, a2, a3;

    n  = nd;
    ax = PM_qabs(x);
    if (n == 0)
       {if (ax <= 2.0)
           {z   = PM_times_rq(0.5, x);
	    y   = PM_qsqr(z);
	    a1  = PM_qhorner(y, _PM_k0_coeff_na, 0, 6);
	    a2  = PM_qin(x, 0.0);
	    a3  = PM_qln(z);
	    a2  = PM_times_qq(a2, a3);
	    ret = PM_minus_qq(a1, a2);}
        else
           {y   = PM_qrecip(x);
            y   = PM_times_rq(2.0, y);
	    a1  = PM_qhorner(y, _PM_k0_coeff_nb, 0, 6);
            z   = PM_times_rq(-1.0, x);
            a2  = PM_qexp(z);
	    a3  = PM_qsqrt(x);
	    a2  = PM_rquotient_qq(a2, a3);
	    ret = PM_times_qq(a1, a2);};}

    else if (n == 1)
       {if (ax <= 2.0)
           {z   = PM_times_rq(0.5, x);
	    y   = PM_qsqr(z);
	    a1  = PM_qhorner(y, _PM_k1_coeff_na, 0, 6);
	    a1  = PM_rquotient_qq(a1, x);
	    a2  = PM_qln(z);
	    a3  = PM_qin(x, 1.0);
	    a2  = PM_times_qq(a2, a3);
	    ret = PM_plus_qq(a1, a2);}
        else
           {y   = PM_qrecip(x);
            y   = PM_times_rq(2.0, y);
	    a1  = PM_qhorner(y, _PM_k1_coeff_nb, 0, 6);
            z   = PM_times_rq(-1.0, x);
            a2  = PM_qexp(z);
	    a3  = PM_qsqrt(x);
	    a2  = PM_rquotient_qq(a2, a3);
	    ret = PM_times_qq(a1, a2);};}

    else
       {tox = PM_qrecip(x);
	tox = PM_times_rq(2.0, tox);

        bkm = PM_qkn(x, 0.0);
	bk  = PM_qkn(x, 1.0);
        ret = Qzero;
	for (j = 1; j < n; j++)
            {bkp = PM_times_qq(tox, bk);
	     bkp = PM_times_rq(j, bkp);
             bkp = PM_plus_qq(bkp, bkm);
             bkm = bk;
	     bk  = bkp;};

	ret = bk;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_qK0 - k0 */

quaternion PM_qk0(quaternion x)
   {

    return(PM_qkn(x, 0.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_qK1 - k1 */

quaternion PM_qk1(quaternion x)
   {

    return(PM_qkn(x, 1.0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
