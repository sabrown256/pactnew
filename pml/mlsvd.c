/*
 * MLSVD.C - singular value decomposition solver
 *         - following Numerical Recipes in C
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#define TOL 1.0e-5

#define SIGN(a, b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

#define INDEX(i, j, m)  i*m + j

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SVD_SOLVE - do the back substitution step in an SVD solve
 *              - completing the solution to A.X = B
 *              - where A = UWV - the SVD decomposition of A
 */

int PM_svd_solve(const double *u, const double *w, const double *v,
		 int m, int n, const double *b, double *x)
   {int i, j, k, ij, jk;
    double s;
    double *t;

    t = CMAKE_N(double, n);

/* compute tr(U).B */
    for (j = 0; j < n;j++)
        {s = 0.0;
	 if (w[j] != 0.0)
	    {for (i = 0; i < m; i++)
	         {ij = INDEX(i, j, n);
		  s += u[ij]*b[i];};
	     s /= w[j];};

	 t[j] = s;};

/* multiply by V */
    for (j = 0; j < n; j++)
        {s = 0.0;
	 for (k = 0; k < n; k++)
	     {jk = INDEX(j, k, n);
	      s += v[jk]*t[k];};

	 x[j] = s;};

    CFREE(t);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SVD_REDUCE - reduce A(M,N) to bidiagonal form */

static void _PM_svd_reduce(double **prv1, double *panrm,
			   double *a, double *w, int m, int n)
   {int i, j, l, k, ik, il, jk, ki, kj, ii;
    double anrm, bnrm, f, g, h, s, sc;
    double *rv1;

    anrm = 0.0;
    g    = 0.0;
    sc   = 0.0;

    rv1 = CMAKE_N(double, n);
    for (i = 0; i < n; i++)
        {l = i + 1;

	 rv1[i] = sc*g;
	 g      = 0.0;
	 s      = 0.0;
	 sc     = 0.0;
	 if (i < m)
	    {for (k = i; k < m; k++)
	         {ki  = INDEX(k, i, n);
		  sc += fabs(a[ki]);};

	     if (sc != 0.0)
		{for (k = i; k < m; k++)
		     {ki     = INDEX(k, i, n);
		      a[ki] /= sc;
		      s     += a[ki]*a[ki];};

		 ii = INDEX(i, i, n);
		 f  = a[ii];
		 g  = -SIGN(sqrt(s), f);
		 h  = f*g - s;

		 a[ii] = f - g;

		 if (i != n-1)
		    {for (j = l; j < n; j++)
			 {s = 0.0;
			  for (k = i; k < m; k++)
			      {ki = INDEX(k, i, n);
			       kj = INDEX(k, j, n);
			       s += a[ki]*a[kj];};

			  f = s/h;
			  for (k = i; k < m; k++)
			      {ki = INDEX(k, i, n);
			       kj = INDEX(k, j, n);
			       a[kj] += f*a[ki];};};};

		 for (k = i; k < m; k++)
		     {ki     = INDEX(k, i, n);
		      a[ki] *= sc;};};};

	 w[i]  = sc*g;
	 g     = 0.0;
	 s     = 0.0;
	 sc    = 0.0;
	 if ((i < m) && (i != n-1))
	    {for (k = l; k < n; k++)
	         {ik  = INDEX(i, k, n);
		  sc += fabs(a[ik]);};

	     if (sc != 0.0)
		{for (k = l; k < n; k++)
		     {ik     = INDEX(i, k, n);
		      a[ik] /= sc;
		      s     += a[ik]*a[ik];};

	         il = INDEX(i, l, n);
		 f  = a[il];
		 g  = -SIGN(sqrt(s), f);
		 h  = f*g - s;

		 a[il] = f - g;

		 for (k = l; k < n; k++)
		     {ik     = INDEX(i, k, n);
		      rv1[k] = a[ik]/h;};

		 if (i != m-1)
		    {for (j = l; j < m; j++)
			 {s = 0.0;
			  for (k = l; k < n; k++)
			      {jk = INDEX(j, k, n);
			       ik = INDEX(i, k, n);
			       s += a[jk]*a[ik];};

			  for (k = l; k < n; k++)
			      {jk     = INDEX(j, k, n);
			       a[jk] += s*rv1[k];};};};

		 for (k = l; k < n; k++)
		     {ik     = INDEX(i, k, n);
		      a[ik] *= sc;};};};

	 bnrm = fabs(w[i]) + fabs(rv1[i]);
	 anrm = max(anrm, bnrm);};

    *panrm = anrm;
    *prv1  = rv1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SVD_V - compute V the right-hand transformations */

static void _PM_svd_v(double *v, const double *a,
		      int m, int n, const double *rv1)
   {int i, j, k, l, ii, ij, ik, il, ji, ki, kj;
    double g, s;

    j = 0;
    k = 0;
    l = 0;
    g = 0.0;

    for (i = n-1; i >= 0; i--)
        {if (i < n-1)
	    {if (g != 0.0)
		{for (j = l; j < n; j++)
		     {ji = INDEX(j, i, n);
		      ij = INDEX(i, j, n);
		      il = INDEX(i, l, n);
		      v[ji] = (a[ij]/a[il])/g;};

		 for (j = l; j < n; j++)
		     {s = 0.0;
		      for (k = l; k < n; k++)
			  {ik = INDEX(i, k, n);
			   kj = INDEX(k, j, n);
			   s += a[ik]*v[kj];};

		      for (k = l; k < n; k++)
			  {kj = INDEX(k, j, n);
			   ki = INDEX(k, i, n);
			   v[kj] += s*v[ki];};};};

	     for (j = l; j < n;j++)
	         {ij = INDEX(i, j, n);
		  ji = INDEX(j, i, n);
		  v[ij] = 0.0;
		  v[ji] = 0.0;};};

	 ii    = INDEX(i, i, n);
	 v[ii] = 1.0;
	 g     = rv1[i];
	 l     = i;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SVD_U - compute U the left-hand transformations
 *           - return them in A
 */

static void _PM_svd_u(double *a, const double *w, int m, int n)
   {int i, j, k, l, ii, ij, ki, kj, ji;
    double f, g, s;

    for (i = n-1; i >= 0; i--)
        {l = i+1;
	 g = w[i];
	 if (i < n-1)
	    for (j = l; j < n; j++)
	        {ij = INDEX(i, j, n);
		 a[ij] = 0.0;};

	 if (g != 0.0)
	    {g = 1.0/g;
	     if (i != n-1)
		{for (j = l; j < n; j++)
		     {s = 0.0;
		      for (k = l; k < m; k++)
			  {ki = INDEX(k, i, n);
			   kj = INDEX(k, j, n);
			   s += a[ki]*a[kj];};

		      ii = INDEX(i, i, n);
		      f  = (s/a[ii])*g;
		      for (k = i; k < m; k++)
			  {ki = INDEX(k, i, n);
			   kj = INDEX(k, j, n);
			   a[kj] += f*a[ki];};};};

	     for (j = i; j < m; j++)
	         {ji     = INDEX(j, i, n);
		  a[ji] *= g;};}

	 else
	    {for (j = i; j < m; j++)
	         {ji    = INDEX(j, i, n);
		  a[ji] = 0.0;};};

	 ii     = INDEX(i, i, n);
	 a[ii] += 1.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SVD_DECOMPOSE - do the SVD decomposition step
 *                  - compute the singular value decomposition of A(M, N),
 *                  -    A = U.W.tr(V)
 *                  - U is MxN and returned in A
 *                  - W is a vector of length N and 
 *                  - contains the diagonal of the matrix of singular values
 *                  - V is NxN
 *                  - if M < N then A should be filled out to NxN with
 *                  - rows of zeros
 */

int PM_svd_decompose(double *a, int m, int n, double *w, double *v)
   {int flag, i, its, j, jp, k, l, nm;
    int ji, jj, jm, jk;
    double c, f, h, s, x, y, z;
    double anrm, g, wc;
    double *rv1;

     if (m < n)
        {PM_err("AUGMENT A WITH ZERO ROWS - PM_SVD_DECOMPOSE");
         return(FALSE);};

     PM_array_set(w, n, 0.0);
     PM_array_set(v, n*n, 0.0);

/* Householder reduction to bidiagonal form */
     _PM_svd_reduce(&rv1, &anrm, a, w, m, n);

/* compute right-hand transformations V */
     _PM_svd_v(v, a, m, n, rv1);

/* compute left-hand transformations U (in A) */
     _PM_svd_u(a, w, m, n);

/* diagonalization of the bidiagonal form */

/* loop over singular values */
     for (k = n-1; k >= 0; k--)

/* loop over allowed iterations */
         {for (its = 0; its < 30; its++)
	      {flag = 1;
	       for (l = k; l >= 0; l--)
		   {nm = l - 1;
                    wc = (l > 0) ? w[nm] : 0.0;
		    if (fabs(rv1[l]) + anrm == anrm)
		       {flag = 0;
			break;};

		    if (fabs(wc) + anrm == anrm)
		       break;};

	       if (flag != 0)
		  {c = 0.0;
		   s = 1.0;
		   for (i = 0; i < k; i++)
		       {f = s*rv1[i];
			if (fabs(f) != 0.0)
			   {g = w[i];
			    h = HYPOT(f, g);
			    w[i] = h;
			    h = 1.0/h;
			    c = g*h;
			    s = -f*h;
			    for (j = 0; j < m; j++)
			        {jm = INDEX(j, nm, n);
				 ji = INDEX(j, i, n);
				 y  = a[jm];
				 z  = a[ji];
				 a[jm] = y*c + z*s;
				 a[ji] = z*c - y*s;};};};};

/* convergence */
	       z = w[k];
	       if (l == k)

/* singular value is made nonnegative */
		  {if (z < 0.0)
		      {w[k] = -z;
		       for (j = 0; j < n; j++)
			   {jk    = INDEX(j, k, n);
			    v[jk] = -v[jk];};};
		   break;};

	       if (its == 30)
		  {PM_err("NO CONVERGENCE - PM_SVD_DECOMPOSE");
		   return(FALSE);};

	       x = w[l];
	       nm = k-1;
	       y = w[nm];
	       g = rv1[nm];
	       h = rv1[k];
	       f = ((y-z)*(y+z) + (g-h)*(g+h))/(2.0*h*y);
	       g = HYPOT(f, 1.0);
	       f = ((x-z)*(x+z) + h*((y/(f + SIGN(g, f))) - h))/x;

/* next QR transformation */
	       c = 1.0;
	       s = 1.0;
	       for (j = l; j <= nm; j++)
		   {i = j+1;
		    g = rv1[i];
		    y = w[i];
		    h = s*g;
		    g = c*g;
		    z = HYPOT(f, h);

		    rv1[j] = z;

		    c = f/(z + SMALL);
		    s = h/(z + SMALL);
		    f = x*c + g*s;
		    g = g*c - x*s;
		    h = y*s;
		    y = y*c;
		    for (jp = 0; jp < n; jp++)
		        {jj = INDEX(jp, j, n);
			 ji = INDEX(jp, i, n);
			 x  = v[jj];
			 z  = v[ji];
			 v[jj] = x*c + z*s;
			 v[ji] = z*c - x*s;};

		    z    = HYPOT(f, h);
		    w[j] = z;

/* rotation can be arbitary if z == 0 */
		    if (z != 0.0)
		       {z = 1.0/z;
			c = f*z;
			s = h*z;};

		    f = c*g + s*y;
		    x = c*y - s*g;
		    for (jp = 0; jp < m; jp++)
		        {jj = INDEX(jp, j, n);
			 ji = INDEX(jp, i, n);
			 y  = a[jj];
			 z  = a[ji];
			 a[jj] = y*c + z*s;
			 a[ji] = z*c - y*s;};};

	       rv1[l] = 0.0;
	       rv1[k] = f;
	       w[k]   = x;};};

    CFREE(rv1);

    return(TRUE);}
     
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
     
/* PM_SVD_FIT - perform a linear least squares fit using SVD
 *            - return the MA fitting coefficients A 
 *            - input:
 *            -   (X, Y)  data points
 *            -   SIG     individual standard deviations
 *            -   NP      number of data points
 *            -   FNC     computes the basis functions of the fit
 *            - output:
 *            -   (U,V,W) the SVD decomposition       
 *            -   A       the fitting coefficients
 *            -   CS      the Chi^2 of the fit
 */

int PM_svd_fit(const double *x, const double *y, const double *sig, int np,
	       double *a, int ma,
	       double *u, double *v, double *w, double *pcs,
	       void (*fnc)(double x, double *f, int n))
   {int i, j, ij;
    double wmax, c, wl, afs, cs;
    double *b, *f;

    b = CMAKE_N(double, np);
    f = CMAKE_N(double, ma);

/* construct the B matrix */
    for (i = 0; i < np; i++)
        {(*fnc)(x[i], f, ma);

	 c = 1.0/sig[i];
	 for (j = 0; j < ma; j++)
	     {ij    = INDEX(i, j, ma);
	      u[ij] = f[j]*c;};

	 b[i] = y[i]*c;};

    PM_svd_decompose(u, np, ma, w, v);

/* find the maximum eigenvalue */
    PM_minmax(w, ma, NULL, &wmax, NULL, NULL);

    wl = TOL*wmax;
    for (j = 0; j < ma; j++)
        if (w[j] < wl)
           w[j] = 0.0;

/* solve (UWV).A = B */
    PM_svd_solve(u, w, v, np, ma, b, a);

/* find the chi-squared value for the fit */
    cs = 0.0;
    for (i = 0; i < np; i++)
        {(*fnc)(x[i], f, ma);

	 afs = 0.0;
	 for (j = 0; j < ma; j++)
	     afs += a[j]*f[j];

	 c   = (y[i] - afs)/sig[i];
	 cs += c*c;};

    CFREE(f);
    CFREE(b);

    if (pcs != NULL)
       *pcs = cs;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SVD_COVARIANCE - compute the covariance matrix of the
 *                   - MA fit pararameters obtained in PM_svd_fit
 *                   - input:
 *                   -   (V,W)  decompostion from PM_svd_fit
 */

int PM_svd_covariance(const double **v, int ma,
		      const double *w, double **cvm)
   {int i, j, k;
    double c, *wti;

    wti = CMAKE_N(double, ma);

    for (i = 0; i < ma; i++)
        {wti[i] = 0.0;
	 if (w[i] != 0.0)
	    wti[i] = 1.0/(w[i]*w[i]);};

    for (i = 0; i < ma; i++)
        {for (j = 0; j < i; j++)
	     {c = 0.0;
	      for (k = 0; k < ma; k++)
		  c += v[i][k]*v[j][k]*wti[k];

	      cvm[j][i] = c;
	      cvm[i][j] = c;};};

    CFREE(wti);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

