/*
 * MLEIGEN.C - eigen value/vector routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_EIGENVALUES - return a PM_matrix of eigenvalues of real
 *                - symmetric matrix A
 */

PM_matrix *PM_eigenvalue(PM_matrix *a)
   {int nr;
    double *ap;
    PM_matrix *mp;

    mp = FMAKE(PM_matrix, "PM_EIGENVALUES:mp");

    nr = a->nrow;

    mp->nrow = nr;
    mp->ncol = 1;
    
    ap = FMAKE_N(double, nr, "PM_EIGENVALUES:ap");
    mp->array = ap;

    return(mp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_EIGENVECTORS - return a PM_matrix of right eigenvectors of real
 *                 - symmetric matrix A
 */

PM_matrix *PM_eigenvectors(PM_matrix *a)
   {int nr;
    double *ap;
    PM_matrix *mp;

    mp = FMAKE(PM_matrix, "PM_EIGENVECTORS:mp");

    nr = a->nrow;

    mp->nrow = nr;
    mp->ncol = nr;
    
    ap = FMAKE_N(double, nr*nr, "PM_EIGENVECTORS:ap");
    mp->array = ap;

    return(mp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
