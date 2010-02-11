/*
 * MLVECT.C - vector routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LNNORM - calculate the ln norm of a vector */

REAL PM_lnnorm(REAL *v, REAL *x, int n, int order)
   {REAL norm;
    int i;

    norm = 0.0;

/* special case: Infinity norm */
    if (order < 0)
       {norm = ABS(v[0]);
        for (i = 1; i < n; i++)
            norm = max(norm, ABS(v[i]));}

    else if (order == 1)
       {norm = 0.0;
        for (i = 0; i < n-1; i++)
            norm += ABS(v[i]) * (x[i+1] - x[i]);}

    else if (order > 1)
       {norm = 0.0;
        for (i = 0; i < n-1; i++)
            norm += pow(ABS(v[i]), order) * (x[i+1] - x[i]);
        norm = pow(norm, 1.0/((double) order));};

    return(norm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

