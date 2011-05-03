/*
 * SHPRM4.C - complex arithmetic functions for Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MK_RECT - make-rectangular for Scheme */

static object *_SSI_mk_rect(SS_psides *si, object *argl)
   {double r, i;
    double _Complex z;
    object *o;

    r = 0.0;
    i = 0.0;
    SS_args(si, argl,
	    SC_DOUBLE_I, &r,
	    SC_DOUBLE_I, &i,
	    0);

    z = r + i*I;
    o = SS_mk_complex(si, z);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MK_POLAR - make-polar for Scheme */

static object *_SSI_mk_polar(SS_psides *si, object *argl)
   {double r, i, m, a;
    double _Complex z;
    object *o;

    m = 0.0;
    a = 0.0;
    SS_args(si, argl,
	    SC_DOUBLE_I, &m,
	    SC_DOUBLE_I, &a,
	    0);

    r = m*cos(a);
    i = m*sin(a);
    z = r + i*I;
    o = SS_mk_complex(si, z);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_REAL_PART - real-part for Scheme */

static object *_SSI_real_part(SS_psides *si, object *argl)
   {double r;
    double _Complex z;
    object *o;

    z = 0.0;
    SS_args(si, argl,
	    SC_DOUBLE_COMPLEX_I, &z,
	    0);

    r = creal(z);
    o = SS_mk_float(si, r);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_IMAG_PART - imag-part for Scheme */

static object *_SSI_imag_part(SS_psides *si, object *argl)
   {double r;
    double _Complex z;
    object *o;

    z = 0.0;
    SS_args(si, argl,
	    SC_DOUBLE_COMPLEX_I, &z,
	    0);

    r = cimag(z);
    o = SS_mk_float(si, r);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MAGNITUDE - magnitude for Scheme */

static object *_SSI_magnitude(SS_psides *si, object *argl)
   {double r, i, m;
    double _Complex z;
    object *o;

    z = 0.0;
    SS_args(si, argl,
	    SC_DOUBLE_COMPLEX_I, &z,
	    0);

    r = creal(z);
    i = cimag(z);
    m = sqrt(r*r + i*i);
    o = SS_mk_float(si, m);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_ANGLE - angle for Scheme */

static object *_SSI_angle(SS_psides *si, object *argl)
   {double r, i, a;
    double _Complex z;
    object *o;

    z = 0.0;
    SS_args(si, argl,
	    SC_DOUBLE_COMPLEX_I, &z,
	    0);

    r = creal(z);
    i = cimag(z);
    a = atan2(i, r);
    o = SS_mk_float(si, a);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INSTALL_COMPLEX - install the SCHEME primitives for complex math */

void _SS_install_complex(SS_psides *si)
   {

    SS_install(si, "make-rectangular",
               "Procedure: make a cartesian complex number from a real and imaginary part",
               SS_nargs,
               _SSI_mk_rect, SS_PR_PROC);

    SS_install(si, "make-polar",
               "Procedure: make a complex number from a magnitude and angle",
               SS_nargs,
               _SSI_mk_polar, SS_PR_PROC);

    SS_install(si, "real-part",
               "Procedure: return the real part of a complex number",
               SS_sargs,
               _SSI_real_part, SS_PR_PROC);

    SS_install(si, "imag-part",
               "Procedure: return the imaginary part of a complex number",
               SS_sargs,
               _SSI_imag_part, SS_PR_PROC);

    SS_install(si, "magnitude",
               "Procedure: return the magnitude of a complex number",
               SS_sargs,
               _SSI_magnitude, SS_PR_PROC);

    SS_install(si, "angle",
               "Procedure: return the polar angle of a complex number",
               SS_sargs,
               _SSI_angle, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
