/*
 * SCBSD.C - (Free)BSD support for PACT codes
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

#ifdef PCK_FREEBSD

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

