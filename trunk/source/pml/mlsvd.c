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

#define SIGN(a, b) ((b) >= 0.0 ? ABS(a) : -ABS(a))

#define INDEX(i, j, m)  i*m + j

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SVD_SOLVE - do the back substitution step in an SVD solve */

int PM_svd_solve(double *u, double *w, double *v, int m, int n,
		 double *b, double *x)
   {int i, j, k, ij, jk;
    double s;
    double *t;

    t = FMAKE_N(double, n, "PM_SVD_SOLVE:t");

/* compute tr(U).B */
    for (j = 0; j < n;j++)
        {s = 0.0;
	 if (w[j] != 0.0)
	    {for (i = 0; i < m; i++)
	         {ij = INDEX(i, j, m);
		  s += u[ij]*b[i];};
	     s /= w[j];};

	 t[j] = s;};

/* multiply by V */
    for (j = 0; j < n; j++)
        {s = 0.0;
	 for (k = 0; k < n; k++)
	     {jk = INDEX(j, k, m);
	      s += v[jk]*t[k];};

	 x[j] = s;};

    SFREE(t);

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

    rv1 = FMAKE_N(double, n, "_PM_SVD_REDUCE:rv1");
    for (i = 0; i < n; i++)
        {l = i + 1;

	 rv1[i] = sc*g;
	 g      = 0.0;
	 s      = 0.0;
	 sc     = 0.0;
	 if (i < m)
	    {for (k = i; k < m; k++)
	         {ki  = INDEX(k, i, m);
		  sc += ABS(a[ki]);};

	     if (sc != 0.0)
		{for (k = i; k < m; k++)
		     {ki     = INDEX(k, i, m);
		      a[ki] /= sc;
		      s     += a[ki]*a[ki];};

		 ii = INDEX(i, i, m);
		 f  = a[ii];
		 g  = -SIGN(sqrt(s), f);
		 h  = f*g - s;

		 a[ii] = f - g;

		 if (i != n-1)
		    {for (j = l; j < n; j++)
			 {s = 0.0;
			  for (k = i; k < m; k++)
			      {ki = INDEX(k, i, m);
			       kj = INDEX(k, j, m);
			       s += a[ki]*a[kj];};

			  f = s/h;
			  for (k = i; k < m; k++)
			      {ki = INDEX(k, i, m);
			       kj = INDEX(k, j, m);
			       a[kj] += f*a[ki];};};};

		 for (k = i; k < m; k++)
		     {ki     = INDEX(k, i, m);
		      a[ki] *= sc;};};};

	 w[i]  = sc*g;
	 g     = 0.0;
	 s     = 0.0;
	 sc    = 0.0;
	 if ((i < m) && (i != n-1))
	    {for (k = l; k < n; k++)
	         {ik  = INDEX(i, k, m);
		  sc += ABS(a[ik]);};

	     if (sc != 0.0)
		{for (k = l; k < n; k++)
		     {ik     = INDEX(i, k, m);
		      a[ik] /= sc;
		      s     += a[ik]*a[ik];};

	         il = INDEX(i, l, m);
		 f  = a[il];
		 g  = -SIGN(sqrt(s), f);
		 h  = f*g - s;

		 a[il] = f - g;

		 for (k = l; k < n; k++)
		     {ik     = INDEX(i, k, m);
		      rv1[k] = a[ik]/h;};

		 if (i != m-1)
		    {for (j = l; j < m; j++)
			 {s = 0.0;
			  for (k = l; k < n; k++)
			      {jk = INDEX(j, k, m);
			       ik = INDEX(i, k, m);
			       s += a[jk]*a[ik];};

			  for (k = l; k < n; k++)
			      {jk     = INDEX(j, k, m);
			       a[jk] += s*rv1[k];};};};

		 for (k = l; k < n; k++)
		     {ik     = INDEX(i, k, m);
		      a[ik] *= sc;};};};

	 bnrm = ABS(w[i]) + ABS(rv1[i]);
	 anrm = max(anrm, bnrm);};

    *panrm = anrm;
    *prv1  = rv1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SVD_V - compute V the right-hand transformations */

static void _PM_svd_v(double *v, double *a, int m, int n, double *rv1)
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
		     {ji = INDEX(j, i, m);
		      ij = INDEX(i, j, m);
		      il = INDEX(i, l, m);
		      v[ji] = (a[ij]/a[il])/g;};

		 for (j = l; j < n; j++)
		     {s = 0.0;
		      for (k = l; k < n; k++)
			  {ik = INDEX(i, k, m);
			   kj = INDEX(k, j, m);
			   s += a[ik]*v[kj];};

		      for (k = l; k < n; k++)
			  {kj = INDEX(k, j, m);
			   ki = INDEX(k, i, m);
			   v[kj] += s*v[ki];};};};

	     for (j = l; j < n;j++)
	         {ij = INDEX(i, j, m);
		  ji = INDEX(j, i, m);
		  v[ij] = 0.0;
		  v[ji] = 0.0;};};

	 ii    = INDEX(i, i, m);
	 v[ii] = 1.0;
	 g     = rv1[i];
	 l     = i;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SVD_U - compute U the left-hand transformations
 *           - return them in A
 */

static void _PM_svd_u(double *a, double *w, int m, int n)
   {int i, j, k, l, ii, ij, ki, kj, ji;
    double f, g, s;

    for (i = n-1; i >= 0; i--)
        {l = i+1;
	 g = w[i];
	 if (i < n-1)
	    for (j = l; j < n; j++)
	        {ij = INDEX(i, j, m);
		 a[ij] = 0.0;};

	 if (g != 0.0)
	    {g = 1.0/g;
	     if (i != n-1)
		{for (j = l; j < n; j++)
		     {s = 0.0;
		      for (k = l; k < m; k++)
			  {ki = INDEX(k, i, m);
			   kj = INDEX(k, j, m);
			   s += a[ki]*a[kj];};

		      ii = INDEX(i, i, m);
		      f  = (s/a[ii])*g;
		      for (k = i; k < m; k++)
			  {ki = INDEX(k, i, m);
			   kj = INDEX(k, j, m);
			   a[kj] += f*a[ki];};};};

	     for (j = i; j < m; j++)
	         {ji     = INDEX(j, i, m);
		  a[ji] *= g;};}

	 else
	    {for (j = i; j < m; j++)
	         {ji    = INDEX(j, i, m);
		  a[ji] = 0.0;};};

	 ii     = INDEX(i, i, m);
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
    int ji, jj, jn, jk;
    double c, f, h, s, x, y, z;
    double anrm, g, wc;
    double *rv1;

     if (m < n)
        {PM_err("AUGMENT A WITH ZERO ROWS - PM_SVD_DECOMPOSE");
         return(FALSE);};

     PM_set_value(w, n, 0.0);
     PM_set_value(v, n*n, 0.0);

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
                    wc = (l > 0) ? w[l-1] : 0.0;
		    if (ABS(rv1[l]) + anrm == anrm)
		       {flag = 0;
			break;};

		    if (ABS(wc) + anrm == anrm)
		       break;};

	       if (flag != 0)
		  {c = 0.0;
		   s = 1.0;
		   for (i = 0; i < k; i++)
		       {f = s*rv1[i];
			if (ABS(f) != 0.0)
			   {g = w[i];
			    h = HYPOT(f, g);
			    w[i] = h;
			    h = 1.0/h;
			    c = g*h;
			    s = -f*h;
			    for (j = 0; j < m; j++)
			        {jn = INDEX(j, nm, m);
				 ji = INDEX(j, i, m);
				 y  = a[jn];
				 z  = a[ji];
				 a[jn] = y*c + z*s;
				 a[ji] = z*c - y*s;};};};};

/* convergence */
	       z = w[k];
	       if (l == k)

/* singular value is made nonnegative */
		  {if (z < 0.0)
		      {w[k] = -z;
		       for (j = 0; j < n; j++)
			   {jk    = INDEX(j, k, m);
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
		        {jj = INDEX(jp, j, m);
			 ji = INDEX(jp, i, m);
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
		        {jj = INDEX(jp, j, m);
			 ji = INDEX(jp, i, m);
			 y  = a[jj];
			 z  = a[ji];
			 a[jj] = y*c + z*s;
			 a[ji] = z*c - y*s;};};

	       rv1[l] = 0.0;
	       rv1[k] = f;
	       w[k]   = x;};};

    SFREE(rv1);

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

int PM_svd_fit(double *x, double *y, double *sig, int np,
	       double *a, int ma,
	       double *u, double *v, double *w, double *pcs,
	       void (*fnc)(double x, double *f, int n))
   {int i, j, ij;
    double wmax, c, wl, afs, cs;
    double *b, *f;

    b = FMAKE_N(double, np, "PM_SVD_FIT:b");
    f = FMAKE_N(double, ma, "PM_SVD_FIT:f");

    for (i = 0; i < np; i++)
        {(*fnc)(x[i], f, ma);

	 c = 1.0/sig[i];
	 for (j = 0; j < ma; j++)
	     {ij    = INDEX(i, j, ma);
	      u[ij] = f[j]*c;};

	 b[i] = y[i]*c;};

    PM_svd_decompose(u, np, ma, w, v);

    wmax = 0.0;
    for (j = 0; j < ma; j++)
        if (w[j] > wmax)
	   wmax = w[j];

    wl = TOL*wmax;
    for (j = 0; j < ma; j++)
        if (w[j] < wl)
           w[j] = 0.0;

    PM_svd_solve(u, w, v, np, ma, b, a);

    cs = 0.0;
    for (i = 0; i < np; i++)
        {(*fnc)(x[i], f, ma);

	 afs = 0.0;
	 for (j = 0; j < ma; j++)
	     afs += a[j]*f[j];

	 c   = (y[i] - afs)/sig[i];
	 cs += c*c;};

    SFREE(f);
    SFREE(b);

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

int PM_svd_covariance(double **v, int ma, double *w, double **cvm)
   {int i, j, k;
    double c, *wti;

    wti = FMAKE_N(double, ma, "PM_SVD_COVARIANCE:wti");

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

    SFREE(wti);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

