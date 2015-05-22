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

/* PM_VCT_EQUAL - return TRUE if X == V[I] to tolerance TOL */

int PM_vct_equal(int nd, const double *x, double **v, int i, double tol)
   {int id, ok;

    ok = TRUE;
    for (id = 0; (id < nd) && (ok == TRUE); id++)
        ok &= PM_value_equal(x[id], v[id][i], tol);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LNNORM - calculate the ln norm of a vector */

double PM_lnnorm(const double *v, const double *x, int n, int order)
   {double norm;
    int i;

    norm = 0.0;

/* special case: Infinity norm */
    if (order < 0)
       {norm = fabs(v[0]);
        for (i = 1; i < n; i++)
            norm = max(norm, fabs(v[i]));}

    else if (order == 1)
       {norm = 0.0;
        for (i = 0; i < n-1; i++)
            norm += fabs(v[i]) * (x[i+1] - x[i]);}

    else if (order > 1)
       {norm = 0.0;
        for (i = 0; i < n-1; i++)
            norm += pow(fabs(v[i]), order) * (x[i+1] - x[i]);
        norm = pow(norm, 1.0/((double) order));};

    return(norm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

