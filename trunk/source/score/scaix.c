/*
 * SCAIX.C - AIX specific routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "score_int.h" 

#ifdef AIX

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* __CREALL128 - missing in AIX 5.3 libraries */

long double __creall128(long double _Complex z)
   {long double r, *pr;
    
    pr = (long double *) &z;
    r  = pr[0];

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* __CIMAGL128 - missing in AIX 5.3 libraries */

long double __cimagl128(long double _Complex z)
   {long double r, *pr;
    
    pr = (long double *) &z;
    r  = pr[1];

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
