/*
 * SHPRM5.C - quaternion arithmetic functions for Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MK_RECT - make-rectangularq for Scheme */

static object *_SSI_mk_rectq(object *argl)
   {quaternion q;
    object *o;

    q.s = 0.0;
    q.i = 0.0;
    q.j = 0.0;
    q.k = 0.0;
    SS_args(argl,
	    SC_DOUBLE_I, &q.s,
	    SC_DOUBLE_I, &q.i,
	    SC_DOUBLE_I, &q.j,
	    SC_DOUBLE_I, &q.k,
	    0);

    o = SS_mk_quaternion(q);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MK_POLARQ - make-polarq for Scheme
 *                - where
 *                -    s = m cos(th)
 *                -    i = m sin(th) cos(ph)
 *                -    j = m sin(th) sin(ph) cos(ch)
 *                -    k = m sin(th) sin(ph) sin(ch) 
 */

static object *_SSI_mk_polarq(object *argl)
   {double m, th, ph, ch;
    double ms, msp;
    quaternion q;
    object *o;

    m  = 0.0;
    th = 0.0;
    ph = 0.0;
    ch = 0.0;
    SS_args(argl,
	    SC_DOUBLE_I, &m,
	    SC_DOUBLE_I, &th,
	    SC_DOUBLE_I, &ph,
	    SC_DOUBLE_I, &ch,
	    0);

    ms  = m*sin(th);
    msp = ms*sin(ph);

    q.s = m*cos(th);
    q.i = ms*cos(ph);
    q.j = msp*cos(ch);
    q.k = msp*sin(ch);

    o = SS_mk_quaternion(q);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_REAL_PARTQ - real-partq for Scheme */

static object *_SSI_real_partq(object *argl)
   {quaternion q;
    object *o;

    q.s = 0.0;
    q.i = 0.0;
    q.j = 0.0;
    q.k = 0.0;
    SS_args(argl,
	    SC_QUATERNION_I, &q,
	    0);

    o = SS_mk_float(q.s);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_IMAG_PARTQ - imag-partq for Scheme */

static object *_SSI_imag_partq(object *argl)
   {quaternion q;
    object *o;

    q.s = 0.0;
    q.i = 0.0;
    q.j = 0.0;
    q.k = 0.0;
    SS_args(argl,
	    SC_QUATERNION_I, &q,
	    0);

    o = SS_mk_float(q.i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_JMAG_PART - jmag-partq for Scheme */

static object *_SSI_jmag_partq(object *argl)
   {quaternion q;
    object *o;

    q.s = 0.0;
    q.i = 0.0;
    q.j = 0.0;
    q.k = 0.0;
    SS_args(argl,
	    SC_QUATERNION_I, &q,
	    0);

    o = SS_mk_float(q.j);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_KMAG_PART - kmag-partq for Scheme */

static object *_SSI_kmag_partq(object *argl)
   {quaternion q;
    object *o;

    q.s = 0.0;
    q.i = 0.0;
    q.j = 0.0;
    q.k = 0.0;
    SS_args(argl,
	    SC_QUATERNION_I, &q,
	    0);

    o = SS_mk_float(q.k);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MAGNITUDEQ - magnitudeq for Scheme */

static object *_SSI_magnitudeq(object *argl)
   {double m;
    quaternion q;
    object *o;

    q.s = 0.0;
    q.i = 0.0;
    q.j = 0.0;
    q.k = 0.0;
    SS_args(argl,
	    SC_QUATERNION_I, &q,
	    0);

    m = PM_qnorm(q);
    o = SS_mk_float(m);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_ANGLEQ - angleq for Scheme
 *             - where
 *             -    ph = atan(sqrt(i*i + j*j + k*k)/s)
 */

static object *_SSI_angleq(object *argl)
   {double th;
    quaternion q;
    object *o;

    q.s = 0.0;
    q.i = 0.0;
    q.j = 0.0;
    q.k = 0.0;
    SS_args(argl,
	    SC_QUATERNION_I, &q,
	    0);

    th = atan2(sqrt(q.i*q.i + q.j*q.j + q.k*q.k), q.s);
    o  = SS_mk_float(th);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_COLATQ - colatitudeq for Scheme
 *             - where
 *             -    ph = atan(sqrt(j*j + k*k)/i)
 */

static object *_SSI_colatq(object *argl)
   {double ph;
    quaternion q;
    object *o;

    q.s = 0.0;
    q.i = 0.0;
    q.j = 0.0;
    q.k = 0.0;
    SS_args(argl,
	    SC_QUATERNION_I, &q,
	    0);

    ph = atan2(sqrt(q.j*q.j + q.k*q.k), q.i);
    o  = SS_mk_float(ph);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LONGQ - longitudeq for Scheme
 *            - where
 *            -    ch = atan(k/j)
 */

static object *_SSI_longq(object *argl)
   {double ch;
    quaternion q;
    object *o;

    q.s = 0.0;
    q.i = 0.0;
    q.j = 0.0;
    q.k = 0.0;
    SS_args(argl,
	    SC_QUATERNION_I, &q,
	    0);

    ch = atan2(q.k, q.j);
    o  = SS_mk_float(ch);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INSTALL_QUATERNION - install the SCHEME primitives for quaternion math */

void _SS_install_quaternion(void)
   {

    SS_install("make-rectangularq",
               "Procedure: make a quaternion from cartesian components",
               SS_nargs,
               _SSI_mk_rectq, SS_PR_PROC);

    SS_install("make-polarq",
               "Procedure: make a quaternion from a magnitude and angle",
               SS_nargs,
               _SSI_mk_polarq, SS_PR_PROC);

    SS_install("real-partq",
               "Procedure: return the S part of a quaternion",
               SS_sargs,
               _SSI_real_partq, SS_PR_PROC);

    SS_install("imag-partq",
               "Procedure: return the I part of a quaternion",
               SS_sargs,
               _SSI_imag_partq, SS_PR_PROC);

    SS_install("jmag-partq",
               "Procedure: return the J part of a quaternion",
               SS_sargs,
               _SSI_jmag_partq, SS_PR_PROC);

    SS_install("kmag-partq",
               "Procedure: return the K part of a quaternion",
               SS_sargs,
               _SSI_kmag_partq, SS_PR_PROC);

    SS_install("magnitudeq",
               "Procedure: return the magnitude of a quaternion",
               SS_sargs,
               _SSI_magnitudeq, SS_PR_PROC);

    SS_install("angleq",
               "Procedure: return the polar angle of a quaternion",
               SS_sargs,
               _SSI_angleq, SS_PR_PROC);

    SS_install("colatitudeq",
               "Procedure: return the colatitude of a quaternion",
               SS_sargs,
               _SSI_colatq, SS_PR_PROC);

    SS_install("longitudeq",
               "Procedure: return the longitude of a quaternion",
               SS_sargs,
               _SSI_longq, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
