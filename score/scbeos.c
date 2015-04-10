/*
 * SCCYG.C - CYGWIN support for PACT codes
 *         - complex support in the compiler is there
 *         - headers and library functions are missing
 *         - fill in what we need
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h" 

#ifdef PCK_BEOS

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CREALL - missing in C99 complex library function */

long double creall(long double _Complex z)
   {long double r, *pr;
    
    pr = (long double *) &z;
    r  = pr[0];

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CIMAGL - missing in C99 complex library function */

long double cimagl(long double _Complex z)
   {long double r, *pr;
    
    pr = (long double *) &z;
    r  = pr[1];

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CREAL - missing in C99 complex library function */

double creal(double _Complex z)
   {double r, *pr;
    
    pr = (double *) &z;
    r  = pr[0];

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CIMAG - missing in C99 complex library function */

double cimag(double _Complex z)
   {double r, *pr;
    
    pr = (double *) &z;
    r  = pr[1];

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CREALF - missing in C99 complex library function */

float crealf(float _Complex z)
   {float r, *pr;
    
    pr = (float *) &z;
    r  = pr[0];

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CIMAGF - missing in C99 complex library function */

float cimagf(float _Complex z)
   {float r, *pr;
    
    pr = (float *) &z;
    r  = pr[1];

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POWL - missing in C99 complex library function */

long double powl(long double x, long double y)
   {double xd, yd, r;

    xd = x;
    yd = y;
    r  = pow(xd, yd);

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

