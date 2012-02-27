/*
 * MLMATR.C - matrix operations including
 *          - create a matrix, destroy a matrix,
 *          - addition, subtraction, and multiplication,
 *          - transpose, invert, lu decompose, solve,
 *          - take upper and lower diagonal parts,
 *          - print
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CREATE - create a matrix with NR rows and NC columns */

PM_matrix *PM_create(int nr, int nc)
   {PM_matrix *mp;
    double *ap;

    mp = CMAKE(PM_matrix);

    mp->nrow = nr;
    mp->ncol = nc;
    
    ap = CMAKE_N(double, nr*nc);
    mp->array = ap;

    return(mp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DESTROY - release the storage associated with the given matrix */

int PM_destroy(PM_matrix *mp)
   {

    if (mp == NULL)
       return(FALSE);

    CFREE(mp->array);
    CFREE(mp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MATRIX_DONE - free the matrix but return the array */

double *PM_matrix_done(PM_matrix *mp)
   {double *rv;

    rv = NULL;

    if (mp != NULL)
       {rv = mp->array;
	CFREE(mp);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ZERO - zero a matrix with NR rows and NC columns */

PM_matrix *PM_zero(PM_matrix *a)
   {int n, nr, nc;

    nr = a->nrow;
    nc = a->ncol;
    n  = nr*nc;
    PM_array_set(a->array, n, 0.0);

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_IS_ZERO - return TRUE iff all of the elements of matrix A
 *            - are within machine precision of 0.0
 */

int PM_is_zero(PM_matrix *a)
   {int i, n, nr, nc, ok;
    double tol;
    double *ar;

    tol = 100.0*PM_machine_precision();

    nr = a->nrow;
    nc = a->ncol;
    ar = a->array;
    n  = nr*nc;

    ok = TRUE;
    for (i = 0; i < n; i++)
        ok &= (ABS(ar[i]) <= tol);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_IDENT - set diagonals of a matrix to unity */

PM_matrix *PM_ident(PM_matrix *a)
   {int i, nr, nc;

    nr = a->nrow;
    nc = a->ncol;

    if (nr > nc)
       nr = nc;

#pragma omp parallel for
    for (i = 1; i <= nr; i++)
        PM_element(a, i, i) = 1.0;

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NEGATIVE - replace a matrix of NR rows and NC columns with the
 *             - negative of the matrix
 */

PM_matrix *PM_negative(PM_matrix *m, PM_matrix *a)
   {int i, n, nr, nc;
    double *ar, *mr;

    nr = a->nrow;
    nc = a->ncol;
    ar = a->array;
    mr = m->array;
    n  = nr*nc;

#pragma omp parallel for
    for (i = 0; i < n; i++)
        mr[i] = -ar[i];

    return(m);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COPY - copy a matrix of NR rows and NC columns S to a matrix
 *         - of the same size D
 */

PM_matrix *PM_copy(PM_matrix *d, PM_matrix *s)
   {int i, n, nr, nc;
    double *sr, *dr;

    nr = s->nrow;
    nc = s->ncol;
    sr = s->array;
    dr = d->array;
    n  = nr*nc;

#pragma omp parallel for
    for (i = 0; i < n; i++)
        dr[i] = sr[i];

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_TRANSPOSE - transpose the given matrix */

PM_matrix *_PM_transpose(PM_matrix *m, PM_matrix *a)
   {int i, j, nc, nr;

    if ((m == NULL) || (a == NULL))
       {PM_err("NULL MATRIX - _PM_TRANSPOSE");
        return(NULL);};

    nr = a->nrow;
    nc = a->ncol;

    for (i = 1; i <= nr; i++)
        {

#pragma omp parallel for
         for (j = 1; j <= nc; j++)
             PM_element(m, j, i) = PM_element(a, i, j);

        };

    return(m);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_TRANSPOSE - transpose the given matrix */

PM_matrix *PM_transpose(PM_matrix *a)
   {int nc, nr;
    PM_matrix *c;

    if (a == NULL)
       {PM_err("NULL MATRIX - PM_TRANSPOSE");
        return(NULL);};

    nr = a->nrow;
    nc = a->ncol;

    c = _PM_transpose(PM_create(nc, nr), a);

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_TIMES - multiply two matrices M = A.B */

PM_matrix *_PM_times(PM_matrix *m, PM_matrix *a, PM_matrix *b)
   {int mr, mc, ar, ac, br, bc;
    double sum, ce, *va, *vb;
    int k, j, i;

    if ((m == NULL) || (a == NULL) || (b == NULL))
       {PM_err("NULL MATRIX - _PM_TIMES");
        return(NULL);};
        
    mr = m->nrow;
    mc = m->ncol;
    ar = a->nrow;
    ac = a->ncol;
    br = b->nrow;
    bc = b->ncol;

/* handle error */
    if ((ac != br) || (mr != ar) || (mc != bc))
       {PM_err("ROW COLUMN MISMATCH - _PM_TIMES");
        return(NULL);};

    for (i = 1; i <= ar; i++)
        {for (j = 1; j <= bc; j++)
	     {va = a->array + (i - 1)*ac;
	      vb = b->array + j - 1;
              sum = 0.0;
	      for (k = 0; k < ac; k++, va++, vb += bc)
		  {ce   = (*va)*(*vb);
		   sum += ce;};

	      PM_element(m, i, j) = sum;};};

    return(m);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_TIMES - multiply two matrices */

PM_matrix *PM_times(PM_matrix *a, PM_matrix *b)
   {int ar, bc;
    PM_matrix *c;

    if ((a == NULL) || (b == NULL))
       {PM_err("BAD MATRIX - PM_TIMES");
        return(NULL);};
        
    ar = a->nrow;
    bc = b->ncol;

    c = _PM_times(PM_create(ar, bc), a, b);

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_NEGATIVE_TIMES - return negative product of two matrices, M = -AB  */

PM_matrix *_PM_negative_times(PM_matrix *m, PM_matrix *a, PM_matrix *b)
   {int mr, mc, ar, ac, br, bc;
    double sum, ce, *va, *vb;
    int k, j, i;

    if ((m == NULL) || (a == NULL) || (b == NULL))
       {PM_err("NULL MATRIX - _PM_NEGATIVE_TIMES");
        return(NULL);};
        
    mr = m->nrow;
    mc = m->ncol;
    ar = a->nrow;
    ac = a->ncol;
    br = b->nrow;
    bc = b->ncol;

/* handle error */
    if ((ac != br) || (mr != ar) || (mc != bc))
       {PM_err("ROW COLUMN MISMATCH - _PM_NEGATIVE_TIMES");
        return(NULL);};

    for (i = 1; i <= ar; i++)
        {for (j = 1; j <= bc; j++)
	     {va = a->array + (i - 1)*ac;
	      vb = b->array + j - 1;
              sum = 0.0;
	      for (k = 0; k < ac; k++, va++, vb += bc)
		  {ce   = (*va)*(*vb);
		   sum += ce;};

	      PM_element(m, i, j) = -sum;};};

    return(m);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NEGATIVE_TIMES - multiply two matrices */

PM_matrix *PM_negative_times(PM_matrix *a, PM_matrix *b)
   {int ar, bc;
    PM_matrix *c;

    if ((a == NULL) || (b == NULL))
       {PM_err("BAD MATRIX - PM_NEGATIVE_TIMES");
        return(NULL);};
        
    ar = a->nrow;
    bc = b->ncol;

    c = _PM_negative_times(PM_create(ar, bc), a, b);

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_PLUS - add the given matrices C = A + B */

PM_matrix *_PM_plus(PM_matrix *c, PM_matrix *a, PM_matrix *b)
   {int i, n, nr, nc;
    double *ar, *br, *cr;

    nr = a->nrow;
    nc = a->ncol;
    if ((nr != b->nrow) || (nc != b->ncol))
       {PM_err("ROW COLUMN MISMATCH - _PM_PLUS");
        return(NULL);};

    ar = a->array;
    br = b->array;
    cr = c->array;
    n = nr*nc;

#pragma omp parallel for
    for (i = 0; i < n; i++)
        cr[i] = ar[i] + br[i];

    return(c);}
                
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_PLUS - add the given matrices */

PM_matrix *PM_plus(PM_matrix *a, PM_matrix *b)
   {int nr, nc;
    PM_matrix *c;

    nr = a->nrow;
    nc = a->ncol;
    if ((nr != b->nrow) || (nc != b->ncol))
       {PM_err("ROW COLUMN MISMATCH - PM_PLUS");
        c = NULL;}

    else
       c = _PM_plus(PM_create(nr, nc), a, b);
                
    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MINUS - subtract the given matrices C = A - B */

PM_matrix *_PM_minus(PM_matrix *c, PM_matrix *a, PM_matrix *b)
   {int i, n, nr, nc;
    double *ar, *br, *cr;

    nr = a->nrow;
    nc = a->ncol;
    if ((nr != b->nrow) || (nc != b->ncol))
       {PM_err("ROW COLUMN MISMATCH - _PM_MINUS");
        return(NULL);};

    ar = a->array;
    br = b->array;
    cr = c->array;
    n = nr*nc;

#pragma omp parallel for
    for (i = 0; i < n; i++)
        cr[i] = ar[i] - br[i];

    return(c);}
                
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MINUS - subtract the given matrices */

PM_matrix *PM_minus(PM_matrix *a, PM_matrix *b)
   {int nr, nc;
    PM_matrix *c;

    nr = a->nrow;
    nc = a->ncol;
    if ((nr != b->nrow) || (nc != b->ncol))
       {PM_err("ROW COLUMN MISMATCH - PM_MINUS");
        c = NULL;}

    else
       c = _PM_minus(PM_create(nr, nc), a, b);

    return(c);}
                
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DEL_COL - delete the specified column from the matrix 
 *            - assumes 0 based column numbering
 *            - input array col is indexes of columns to
 *            - be deleted.  col is assumed to be sorted in
 *            - ascending order and to have no duplicate entries. 
 */


int PM_del_col(PM_matrix *a, long *col, long ncol)
   {int i, j, nr, nc;
    long *dcolptr, ndcol;
    double *new, *nptr, *optr;

    nr = a->nrow;
    nc = a->ncol;


    if ((col[ncol-1] >= nc) || (col[0] < 0))
       return(FALSE);

    nptr  = CMAKE_N(double, nr*(nc - ncol));
    new   = nptr;
    optr  = a->array;
    ndcol = 0;

    for (i = 0; i < nr; i++)
        for (j = 0, dcolptr = col, ndcol = 0; j < nc; j++)
            {if (j != *dcolptr)
                *nptr++ = *optr;
             else
                {while ((*dcolptr <= j) && (ndcol < ncol))
                    {dcolptr++;
                     ndcol++;};}
             optr++;};

    CFREE(a->array);
    a->array = new;
    a->ncol  = nc - ndcol;

    return(TRUE);}
                
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_GET_COL - return the specified column from the input matrix
 *             - assumes 0 based column numbering
 */


double *_PM_get_col(PM_matrix *a, int col)
   {int i, nr, nc;
    double *get, *nptr, *optr;

    nr = a->nrow;
    nc = a->ncol;

    if ((col >= nc) || (col < 0))
       return(NULL);

    get  = nptr = CMAKE_N(double, nr);
    optr = a->array + col;

    for (i = 0; i < nr; i++, nptr++)
        {*nptr = *optr;
         optr  += nc;}

    return(get);}

 /*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_PUT_COL - replace the values in the specified column with the input
 *             - array.
 *             - assumes 0 based column numbering
 */


int _PM_put_col(PM_matrix *a, int col, double *put)
   {int i, nr, nc;
    double *optr;

    nr = a->nrow;
    nc = a->ncol;

    if ((col >= nc) || (col < 0))
       return(FALSE);

    optr = a->array + col;

    for (i = 0; i < nr; i++, put++)
        {*optr = *put;
         optr += nc;}

    return(TRUE);}
               
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SORT_ON_COL - sort the input matrix on the specified column
 *                - assumes 0 based column numbering
 */


int PM_sort_on_col(PM_matrix *a, int col)
   {int i, j, nr, nc;
    int *ind;
    double *rcol, *temp;

    nr = a->nrow;
    nc = a->ncol;

    if ((col >= nc) || (col < 0))
       return(FALSE);

    rcol = _PM_get_col(a, col);

    ind = CMAKE_N(int, nr);
    for (i = 0; i < nr; ind[i] = i, i++);

    PM_q_sort(rcol, ind, nr);

    _PM_put_col(a, col, rcol);

    temp = CMAKE_N(double, nr);
    
    for (i = 0; i < nc; i++)
        {if (i != col)
            {rcol = _PM_get_col(a, i);
	     if (rcol != NULL)
	        {for (j = 0; j < nr; j++)
		     temp[j] = rcol[ind[j]];
		 _PM_put_col(a, i, temp);};};};

    CFREE(rcol);
    CFREE(ind);
    CFREE(temp);

    return(TRUE);}
                
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
