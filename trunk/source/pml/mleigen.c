/*
 * MLEIGEN.C - eigen value/vector routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#define SIGN(a, b) (((b) < 0.0) ? -fabs(a) : fabs(a))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_HRSYMATR - perform Householder reduction of a symmetric matrix A
 *              - to symmetric tridiagonal form
 *              - return R a 2 column matrix with
 *              - column 1 containing the diagonal
 *              - column 2 containing the sub-diagonal
 *              - if CMV is TRUE A is returned with coefficients to compute
 *              - the eigenvectors of the original A
 *              - following Numerical Recipes in C
 */

PM_matrix *_PM_hrsymatr(PM_matrix *a, int cmv)
   {int i, j, k, l, n;
    double f, g, h, nrm, inrm, u, v;
    PM_matrix *r;

    if (a->nrow != a->ncol)
       return(NULL);

    n = a->nrow;

    r = PM_create(n, 2);

    for (i = n; i >= 2; i--)
        {l = i - 1;

	 h   = 0.0;
	 nrm = 0.0;
	 if (l > 1)
	    {for (k = 1; k <= l; k++)
	         nrm += fabs(PM_element(a, i, k));

/* skip transformation */
	     if (nrm == 0.0)
	        PM_element(r, i, 2) = PM_element(a, i, l);

	     else
	        {inrm = 1.0/nrm;
		 for (k = 1; k <= l; k++)
		     {v  = PM_element(a, i, k);
		      v *= inrm;
		      h += v*v;
		      PM_element(a, i, k) = v;};

		 f  = PM_element(a, i, l);
		 g  = sqrt(h);
		 g *= ((f > 0.0) ? -1.0 : 1.0);
		 PM_element(r, i, 2) = nrm*g;

		 h -= f*g;
		 PM_element(a, i, l) = f - g;

		 f = 0.0;
		 for (j = 1; j <= l; j++)
		     {if (cmv == TRUE)
			 PM_element(a, j, i) = PM_element(a, i, j)/h;

		      g = 0.0;
		      for (k = 1; k <= j; k++)
			  g += PM_element(a, j, k)*PM_element(a, i, k);

		      for (k = j+1; k <= l; k++)
			  g += PM_element(a, k, j)*PM_element(a, i, k);

		      PM_element(r, j, 2) = g/h;

		      f += PM_element(r, j, 2)*PM_element(a, i, j);};

		 u = f/(h + h);
		 for (j = 1; j <= l; j++)
		     {f = PM_element(a, i, j);
		      g = PM_element(r, j, 2) - u*f;

		      PM_element(r, j, 2) = g;

		      for (k = 1; k <= j; k++)
			  PM_element(a, j, k) -= (f*PM_element(r, k, 2) +
						  g*PM_element(a, i, k));};};}

	 else
	    PM_element(r, i, 2) = PM_element(a, i, l);

	 PM_element(r, i, 1) = h;};
		      
    PM_element(r, 1, 2) = 0.0;

    if (cmv == TRUE)
       {PM_element(r, 1, 1) = 0.0;

        for (i = 1; i <= n; i++)
	    {l = i - 1;
	     if (PM_element(r, i, 1) != 0.0)
	        {for (j = 1; j <= l; j++)
		     {g =0.0;
		      for (k = 1; k <= l; k++)
			  g += PM_element(a, i, k)*PM_element(a, k, j);
		      for (k = 1; k <= l; k++)
			  PM_element(a, k, j) -= g*PM_element(a, k, i);};};

	     PM_element(r, i, 1) = PM_element(a, i, i);

	     PM_element(a, i, i) = 1.0;
	     for (j = 1; j <= l; j++)
	         {PM_element(a, j, i) = 0.0;
		  PM_element(a, i, j) = 0.0;};};}

    else
       {for (i = 1; i <= n; i++)
	    PM_element(a, i, i) = 1.0;};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_QL_TRI_EIGEN - tridiagonal QL eigenvalue/eigenvector solver
 *                  - R is a tridiagonal matrix with the diagonal in the
 *                  - first column and the subdiagnal in the second column
 *                  - the first row element of the subdiagonal is unused
 *                  - eigenvalues are returned in the first column of R
 *                  - right eigenvectors are returned if CMV is TRUE
 *                  - following Numerical Recipes in C
 */

static int _PM_ql_tri_eigen(PM_matrix *r, PM_matrix *z, int cmv)
   {int i, k, l, n, m, it;
    double b, c, d, f, g, p, s, t;

    n = r->nrow;

    for (i = 2; i <= n; i++)
        PM_element(r, i-1, 2) = PM_element(r, i, 2);

    PM_element(r, n, 2) = 0.0;

    for (l = 1; l <= n; l++)
        {it = 0;
	 do {for (m = l; m <= n - 1; m++)
	         {d = fabs(PM_element(r, m, 1)) + fabs(PM_element(r, m+1, 1));
		  if (fabs(PM_element(r, m, 2)) + d == d)
		     break;};

	     if (m != l)
	        {if (it++ == 30)
		    return(FALSE);

		 g = (PM_element(r, l+1, 1) - PM_element(r, l, 1))/
		     (2.0*PM_element(r, l, 2));
		 t = sqrt(g*g + 1.0);
		 g = PM_element(r, m, 1) - PM_element(r, l, 1) +
		     PM_element(r, l, 2)/(g + SIGN(t, g));
		 s = 1.0;
		 c = 1.0;
		 p = 0.0;
		 for (i = m-1; i >= l; i--)
		     {f = s*PM_element(r, i, 2);
		      b = c*PM_element(r, i, 2);
		      if (fabs(f) >= fabs(g))
			 {c = g/f;
			  t = sqrt(c*c + 1.0);
			  PM_element(r, i+1, 2) = f*t;
			  s  = 1.0/t;
			  c *= s;}
		      else
			 {s = f/g;
			  t = sqrt(s*s + 1.0);
			  PM_element(r, i+1, 2) = g*t;
			  c  = 1.0/t;
			  s *= c;};

		      g = PM_element(r, i+1, 1) - p;
		      t = (PM_element(r, i, 1) - g)*s + 2.0*c*b;
		      p = s*t;
		      PM_element(r, i+1, 1) = g + p;

		      g = c*t - b;
		      if (cmv == TRUE)
			 {for (k = 1; k <= n; k++)
			      {f = PM_element(z, k, i+1);
			       PM_element(z, k, i+1) = s*PM_element(z, k, i) + c*f;
			       PM_element(z, k, i)   = c*PM_element(z, k, i) - s*f;};};};

		 PM_element(r, l, 1) -= p;
		 PM_element(r, l, 2)  = g;
		 PM_element(r, m, 2)  = 0.0;};}
	 while (m != l);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_EIGENVALUES - return a PM_matrix of eigenvalues of real
 *                - symmetric matrix A
 *                - return NULL if A is not proper shape or NULL
 */

PM_matrix *PM_eigenvalue(PM_matrix *a)
   {int i, n;
    PM_matrix *r, *x;

    x = NULL;

    if (a != NULL)
       {if (a->nrow == a->ncol)
	   {n = a->nrow;

	    r = _PM_hrsymatr(a, TRUE);
    	    _PM_ql_tri_eigen(r, a, FALSE);

	    x = PM_create(n, 1);

	    for (i = 1; i <= n; i++)
	        PM_element(x, i, 1) = PM_element(r, i, 1);

	    PM_destroy(r);};};

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_EIGENVECTORS - return a PM_matrix of right eigenvectors of real
 *                 - symmetric matrix A
 */

int PM_eigenvectors(PM_matrix *a)
   {int rv;
    PM_matrix *r;

    rv = FALSE;

    if (a != NULL)
       {if (a->nrow == a->ncol)
	   {r = _PM_hrsymatr(a, TRUE);
    	    rv = _PM_ql_tri_eigen(r, a, TRUE);

	    PM_destroy(r);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_EIGENSYS - return a PM_matrix of eigenvalues and one of eigenvectors
 *             - for a real symmetric matrix A
 *             - eigenvectors are returned in A
 *             - eigenvalues are returned as the result of the call
 */

PM_matrix *PM_eigensys(PM_matrix *a)
   {int i, n, rv;
    PM_matrix *r, *x;

    x = NULL;

    if (a != NULL)
       {if (a->nrow == a->ncol)
	   {n = a->nrow;

	    r  = _PM_hrsymatr(a, TRUE);
    	    rv = _PM_ql_tri_eigen(r, a, TRUE);

	    if (rv == TRUE)
	       {x = PM_create(n, 1);
		for (i = 1; i <= n; i++)
		    PM_element(x, i, 1) = PM_element(r, i, 1);};

	    PM_destroy(r);};};

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
