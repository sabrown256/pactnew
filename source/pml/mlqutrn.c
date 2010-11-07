/*
 * MLQUTRN.C - quarternion math functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scope_quaternion.h"

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

/* PM_CONJUGATE_Q - return the conjugate A* of quaternion A */

quaternion PM_conjugate_q(quaternion a)
   {quaternion x;

    x.s = a.s;
    x.i = -a.i;
    x.j = -a.j;
    x.k = -a.k;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NORM_Q - return the norm of quaternion A */

double PM_norm_q(quaternion a)
   {double d;

    d = a.s*a.s + a.i*a.i + a.j*a.j + a.k*a.k;
    d = sqrt(d);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_RECIP_Q - return the reciprocal of quaternion A
 *            - defined to be A*/|A|^2
 *            - as opposed to the versor
 */

quaternion PM_recip_q(quaternion a)
   {double d;
    quaternion x;

    d = 1.0/(a.s*a.s + a.i*a.i + a.j*a.j + a.k*a.k);
    x = PM_times_rq(d, PM_conjugate_q(a));

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_VERSOR_Q - return the versor of quaternion A
 *             - defined to be A/|A|
 *             - as opposed to the reciprocal
 */

quaternion PM_versor_q(quaternion a)
   {double d;
    quaternion x;

    d = 1.0/PM_norm_q(a);
    x = PM_times_rq(d, a);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DISTANCE_QQ - return the distance between quaternion A and B */

double PM_distance_qq(quaternion a, quaternion b)
   {double d;

    d = sqrt(PM_norm_q(PM_minus_qq(a, b)));

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LQUOTIENT_QQ - return the left quotient of quaternion A and B */

quaternion PM_lquotient_qq(quaternion a, quaternion b)
   {quaternion x;

    x = PM_times_qq(PM_recip_q(b), a);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_RQUOTIENT_QQ - return the right quotient of quaternion A and B */

quaternion PM_rquotient_qq(quaternion a, quaternion b)
   {quaternion x;

    x = PM_times_qq(a, PM_recip_q(b));

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_EXP_Q - return the exp(A) */

quaternion PM_exp_q(quaternion a)
   {double ea, cv, sv, vf, v;
    quaternion x;

    v = sqrt(a.i*a.i + a.j*a.j + a.k*a.k);

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

/* PM_LN_Q - return the ln(A) */

quaternion PM_ln_q(quaternion a)
   {double ac, as, vf, v, d;
    quaternion x;

    v = sqrt(a.i*a.i + a.j*a.j + a.k*a.k);
    d = PM_norm_q(a);

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

/* PM_POW_Q - return the A^R */

quaternion PM_pow_q(quaternion a, double r)
   {quaternion x;

    x = PM_exp_q(PM_times_rq(r, PM_ln_q(a)));

    return(x);}

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

    vp = PM_times_qq(rq, PM_times_qq(vq, PM_recip_q(rq)));

    v[0] = vp.i;
    v[1] = vp.j;
    v[2] = vp.k;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
