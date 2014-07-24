/*
 * MLMATH.C - random math routines which are missing in some
 *          - libraries or are just too useful not to have
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SET_OPERS - set the opers member of the set to correspond to
 *              - the element type
 *              - return TRUE is there is a known type of PM_field
 *              - to associate with the set
 *              - return FALSE otherwise
 *              - NOTE: so far this will work only for some standard
 *              -       types
 */

int PM_set_opers(PM_set *set)
   {int id, ret;
    char type[MAXLINE], *token, *s = NULL;

    ret = FALSE;
    if (set != NULL)
       {ret = TRUE;

	SC_strncpy(type, MAXLINE, set->element_type, -1);
	token = SC_strtok(type, " (*", s);

	if (token == NULL)
	   ret = FALSE;
	else
	   {id = SC_type_id(token, FALSE);
	    if (id == SC_INT_I)
	       set->opers = PM_gs.int_opers;
	    else if (id == SC_LONG_I)
	       set->opers = PM_gs.long_opers;
	    else if (id == SC_DOUBLE_I)
	       set->opers = PM_gs.fp_opers;
	    else
	       ret = FALSE;};};

    return(ret);}

/*--------------------------------------------------------------------------*/

/*                             SCALAR FUNCTIONS                             */

/*--------------------------------------------------------------------------*/

/* PM_VALUE_EQUAL - return TRUE iff X1 == X2 to tolerance TOL */

int PM_value_equal(double x1, double x2, double tol)
   {int ok;
    double x1a, x2a;

    if (tol < 0.0)
       tol = TOLERANCE;

    ok  = TRUE;
    x1a = ABS(x1);
    x2a = ABS(x2);
    if ((tol < x1a) || (tol < x2a))
       ok &= (2.0*ABS(x1 - x2)/(x1a + x2a + SMALL) < tol);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_VALUE_COMPARE - return 1 if X1 > X2 to tolerance TOL
 *                  -       -1 if X1 < X2 to tolerance
 *                  -        0 if X1 == X2 to tolerance
 */

int PM_value_compare(double x1, double x2, double tol)
   {int rv;
    double dx;

    if (tol < 0.0)
       tol = TOLERANCE;

    dx = (x1 - x2)/(ABS(x1) + ABS(x2) + SMALL);
    if (dx < -tol)
       rv = -1;
    else if (tol < dx)
       rv = 1;
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_QVALUE_COMPARE - return 1 if X1 > X2 to tolerance TOL
 *                   -       -1 if X1 < X2 to tolerance
 *                   -        0 if X1 == X2 to tolerance
 */

#ifdef HAVE_ANSI_FLOAT16

int PM_qvalue_compare(long double x1, long double x2, long double tol)
   {int rv;
    long double dx;

    if (tol < 0.0)
       tol = TOLERANCE;

    dx = (x1 - x2)/(ABS(x1) + ABS(x2) + SMALL);
    if (dx < -tol)
       rv = -1;
    else if (tol < dx)
       rv = 1;
    else
       rv = 0;

    return(rv);}

#else

/* GOTCHA: this version works for PGI who have no idea what to
 * do with long double - the compiler supports the syntax but they
 * do nothing right with the actual arithmetic
 */

int PM_qvalue_compare(long double x1, long double x2, long double tol)
   {int rv;
    double dx, lx1, lx2, ltol;

    lx1  = x1;
    lx2  = x2;
    ltol = tol;

    if (ltol < 0.0)
       ltol = TOLERANCE;

    dx = (lx1 - lx2)/(ABS(lx1) + ABS(lx2) + SMALL);
    if (dx < -ltol)
       rv = -1;
    else if (ltol < dx)
       rv = 1;
    else
       rv = 0;

    return(rv);}

#endif

/*--------------------------------------------------------------------------*/

/*                              ARRAY FUNCTIONS                             */

/*--------------------------------------------------------------------------*/

/* PM_ARRAY_SCALE - multiply all of the elements of an array
 *                - by a conversion factor
 */

void PM_array_scale(double *p, int n, double f)
   {int i;

    if (p != NULL)
       for (i = 0; i < n; i++)
           *(p++) *= f;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ARRAY_SHIFT - add a delta to all of the elements of an array */

void PM_array_shift(double *p, int n, double f)
   {int i;

    if (p != NULL)
       for (i = 0; i < n; i++)
           *(p++) += f;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ARRAY_SET - broadcast a value into an array */

void PM_array_set(double *p, int n, double f)
   {int i;

    if (p != NULL)
       for (i = 0; i < n; i++)
           *(p++) = f;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DARRLT - print the number of elements of P less than F */

long darrlt(double *p, int n, double f)
   {int i, nl;

    nl = 0;
	
    if (p != NULL)
       {for (i = 0; i < n; i++)
            nl += (*p++ < f);};

    return(nl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DARRGT - print the number of elements of P greater than F */

long darrgt(double *p, int n, double f)
   {int i, ng;

    ng = 0;
	
    if (p != NULL)
       {for (i = 0; i < n; i++)
            ng += (*p++ > f);};

    return(ng);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DARREQ - print the number of elements of P equal to F */

long darreq(double *p, int n, double f)
   {int i, ng;

    ng = 0;
	
    if (p != NULL)
       {for (i = 0; i < n; i++)
            ng += (*p++ == f);};

    return(ng);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ARRAY_EQUAL - return TRUE iff all N elements of each array are
 *                - equal to within tolerance
 */

int PM_array_equal(double *s, double *t, int n, double tol)
   {int i, ok;

    ok = TRUE;
    for (i = 0; (i < n) && ok; i++)
        ok &= PM_value_equal(s[i], t[i], tol);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ARRAY_COPY - copy one array to another
 *               - that is copy N double values from array T to array S
 */

void PM_array_copy(double *s, double *t, int n)
   {int i;

    if ((s != NULL) && (t != NULL))
       for (i = 0; i < n; i++)
           *(s++) = *(t++);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CURVE_LEN_2D - return the length of the given curve in 2 space */

double PM_curve_len_2d(double *x, double *y, int n)
   {int i;
    double s;
    double *px1, *px2, *py1, *py2;

    px1 = x;
    px2 = x + 1;
    py1 = y;
    py2 = y + 1;
    for (i = 1, s = 0.0; i < n; i++)
        s += PM_hypot(*px2++ - *px1++, *py2++ - *py1++);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CURVE_LEN_3D - return the length of the given curve in 3 space */

double PM_curve_len_3d(double *x, double *y, double *z, int n)
   {int i;
    double dx, dy, dz, s;
    double *px1, *px2, *py1, *py2, *pz1, *pz2;

    px1 = x;
    px2 = x + 1;
    py1 = y;
    py2 = y + 1;
    pz1 = z;
    pz2 = z + 1;
    for (i = 1, s = 0.0; i < n; i++)
        {dx = *px2++ - *px1++;
         dy = *py2++ - *py1++;
         dz = *pz2++ - *pz1++;
	 s += sqrt(dx*dx + dy*dx + dz*dz);};

    return(s);}

/*--------------------------------------------------------------------------*/

/*                           STATISTICS FUNCTIONS                           */

/*--------------------------------------------------------------------------*/

/* PM_STATS_MEAN - compute mean, median, mode, and standard deviation
 *
 * #bind PM_stats_mean fortran() scheme() python()
 */

void PM_stats_mean(int n, double *x, double *pmn, double *pmdn,
		   double *pmod, double *pstd)
   {int i, j, nh;
    double xc, xs, xsq, xmdn, xmn;

/* compute median */
    nh = n >> 1;

/* sort */
/*
    for (i = n-1; i >= 0; i--)
        {for (j = 0; j <= i; j++)
	     {if (x[j] >= x[j+1])
		 {xc     = x[j];
		  x[j]   = x[j+1];
		  x[j+1] = xc;};};};
*/
    if (n % 2 == 0)
       xmdn = 0.5*(x[nh] + x[nh-1]);
    else
       xmdn = x[nh];

/* compute the mean and standard deviation */
    xs  = 0.0;
    xsq = 0.0;
    for (i = 0; i < n; i++)
        {xc   = x[i];
	 xs  += xc;
	 xsq += xc*xc;};
        
    xmn = xs/((double) n);

/* the mean */
   if (pmn != NULL)
      *pmn = xmn;

/* the standard deviation */
    if (pstd != NULL)
       *pstd = sqrt((n*xsq - xs*xs)/(n*(n - 1.0)));

/* the median */
    if (pmdn != NULL)
       *pmdn = xmdn;

/* the mode */
    if (pmod != NULL)
       {int nt, nx;
	int *ni;
	double xcx;
        double *xt;

	xt = CMAKE_N(double, n);
	ni = CMAKE_N(int, n);

	for (i = 0; i < n; i++)
	    {xt[i] = x[i];
	     ni[i] = 0;};

	nt = n;
	for (i = 0; i < nt; i++)
	    {for (j = i+1; j < nt; j++)
	         {if (xt[i] == xt[j])
		     {ni[i]++;
		      nt--;
		      xt[j] = xt[nt];
		      j--;};};};
		 
	nx  = -1;
	xcx = 0.0;
	for (i = 0; i < nt; i++)
            {if (ni[i] > nx)
	        {nx  = ni[i];
		 xcx = xt[i];};};

	CFREE(ni);
	CFREE(xt);

       *pmod = xcx;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_HASHARR_STATS - compute statistics on the hash efficiency of HA
 *
 * #bind PM_hasharr_stats fortran() scheme() python()
 */

void PM_hasharr_stats(hasharr *ha,
		      double *pmn ARG(,out),
		      double *pmdn ARG(,out),
		      double *pmod ARG(,out),
		      double *pstd ARG(,out))
   {long i, sz, ni;
    double *x;
    haelem *hp, **tb;

    sz = ha->size;
    tb = ha->table;

    x = CMAKE_N(double, sz);

    for (i = 0; i < sz; i++)
        {for (hp = tb[i], ni = 0; hp != NULL; hp = hp->next, ni++)
	 x[i] = ni;};

    PM_stats_mean(sz, x, pmn, pmdn, pmod, pstd);

    CFREE(x);

    return;}

/*--------------------------------------------------------------------------*/

/*                          ADVANCED MATH FUNCTIONS                         */

/*--------------------------------------------------------------------------*/

/* PM_ROMBERG - Romberg integration routine
 *            - integrates func(x) from x0 to x1 to accuracy of tol
 *            - returned as y0.  Returns number of iterations
 *            - in n (2^(n-1) subdivisions) or -1 if integral has not
 *            - converged within 16 iterations (32768 subdivisions)
 */

double PM_romberg(double (*func)(double x), double x0, double x1, double tol)
   {double ly0, toln, h, x;
    double a[16], b[16];
    int i, n, in;

    h   = x1 - x0;
    x   = x0;
    ly0 = 0.0;

    memset(a, 0, sizeof(a));
    a[0] = h*((*func)(x0) + (*func)(x1))/2.0;
    toln = 2.0*tol;

    for (n = 2; (n < 16) && (toln > tol); n++)
        {h    = h/2.0;
         b[0] = a[0]/2.0;
         x    = x0 + h;
         in   = (int) POW(2.0, (n-2));
         for (i = 0; i < in; i++)
             {b[0] += h*func(x);
              x    += 2.0*h;};

         for (i = 1; i < n; i++)
             b[i] = (POW(4.0, i)*b[i-1] - a[i-1])/(POW(4.0, i) - 1.0);

         ly0 = b[n-1];

         toln = ABS((ly0 - b[n-2])/ly0);
         if (toln > tol)
            for (i = 0; i < n; i++)
                a[i] = b[i];};

    return(ly0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTEGRATE_TZR - do simple trapezoid rule integeration of a data set */

double PM_integrate_tzr(double xmn, double xmx, int *pn,
			double *fncx, double *fncy, double *intx, double *inty)
   {int i, k, n;
    double sum, lasty, xta;

    n   = *pn;
    i   = 0;
    sum = 0;

    xta = fncx[n-1];
    xmx = min(xmx, xta);

/* first point */
    if (xmn <= *fncx)
       {lasty   = *fncy;
        intx[0] = *fncx;

        i++;}

    else
       {for (; (fncx[i] < xmn) && (i < n); ++i);

        PM_interp(lasty, xmn, fncx[i-1], fncy[i-1], fncx[i], fncy[i]);

        intx[0] = xmn;};

    inty[0] = sum;

/* interior */
    for (k = 1; (i < (n - 1)) && (fncx[i] < xmx); i++, k++)
       {sum += 0.5*(fncy[i] + lasty)*(fncx[i] - intx[k-1]);

        inty[k] = sum;
        intx[k] = fncx[i];

        lasty   = fncy[i];};

/* last point */
    if (i < n)
       {PM_interp(inty[k], xmx, fncx[i-1], fncy[i-1], fncx[i], fncy[i]);
        sum += 0.5*(inty[k] + lasty)*(xmx - intx[k-1]);

        inty[k] = sum;
        intx[k] = xmx;

        k++;};

    *pn = k;

    return(sum);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DERIVATIVE - take df/dx from fncx and fnc and put the values in
 *               - derx and dery
 */

void PM_derivative(int n, double *fncx, double *fncy, double *derx, double *dery)
   {int i;
    double x1[2], x2[2], yt;

    x1[0] = *fncx++;
    x1[1] = *fncy++;
    for (i = 1; i < n; i++)
        {x2[0] = *fncx++;
         x2[1] = *fncy++;
         *derx++ = 0.5*(x1[0] + x2[0]);
         if (x2[0] != x1[0])
            *dery++ = (x2[1] - x1[1])/(x2[0] - x1[0]);
         else
	    {yt = dery[-1];
	     *dery++ = yt;};
         x1[0] = x2[0];
         x1[1] = x2[1];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_THIN_1D_DER - take a 1d data set and return a new set of points which
 *                - captures the essential features of the original set
 *                - but with fewer points
 *                - this uses a differential measure criterion
 */

int PM_thin_1d_der(int n, double *fncx, double *fncy, double *thnx, double *thny,
		   double toler)
   {int i, i0, j;
    double x1[2], x2[2], xt, dydx, odydx;

    if (toler == HUGE)
       {toler = 10.0/((double) n);
        toler = min(toler, 0.1);};

    x1[0] = *fncx++;
    x1[1] = *fncy++;
    x2[0] = x1[0];
    x2[1] = x1[1];

    j  = 0;
    i0 = -1;

    odydx = -HUGE;
    for (i = 1; i < n; i++)
        {x2[0] = *fncx++;
         x2[1] = *fncy++;

         if (x2[0] != x1[0])
            dydx = (x2[1] - x1[1])/(x2[0] - x1[0]);
         else
	    dydx = x2[1] > x1[1] ? HUGE : -HUGE;

         xt = 0.5*ABS(dydx - odydx)/(ABS(dydx) + ABS(odydx) + SMALL);
         if (xt > toler)
            {if (i0 < i-1)
                {thnx[j] = x1[0];
                 thny[j] = x1[1];
                 j++;};

	     odydx = dydx;
             i0    = i;

             thnx[j] = x2[0];
             thny[j] = x2[1];
             j++;};

         x1[0] = x2[0];
         x1[1] = x2[1];};

    if (x2[0] != thnx[j-1])
       {thnx[j] = x2[0];
        thny[j] = x2[1];
        j++;};

    return(j);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_THIN_1D_INT - take a 1d data set and return a new set of points which
 *                - captures the essential features of the original set
 *                - but with fewer points
 *                - this uses an integral measure criterion
 */

int PM_thin_1d_int(int n, double *fncx, double *fncy, double *thnx, double *thny,
		   double toler)
   {int i, nth, nc, iamn, iamna, iamnb;
    double xc, yc, sm, b, ht, amn;
    double *area;

    memcpy(thnx, fncx, n*sizeof(double));
    memcpy(thny, fncy, n*sizeof(double));

    nth = -1;
    xc  = 0.0;

/* compute the areas */
    area = CMAKE_N(double, n);
    for (i = 1; i < n-1; i++)
        {xc = thnx[i+1] - thnx[i-1];
         if (xc == 0.0)
            break;

         yc = thny[i+1] - thny[i-1];
         sm = yc/xc;
         b  = thny[i+1] - sm*thnx[i+1];
         ht = ABS(thny[i] - sm*thnx[i] - b)/sqrt(sm*sm + 1.0);

         area[i] = 0.5*ht*HYPOT(xc, yc);};

/* if there were no bad points proceed */
    if (xc != 0.0)
       {nth = (int) toler;
        for (nc = n; nth < nc; nc--)

/* find the smallest area */
            {iamn = 0;
             amn  = HUGE;
             for (i = 1; i < nc-1; i++)
                 if (area[i] < amn)
                    {amn  = area[i];
                     iamn = i;};
 
/* compress out point there */
             for (i = iamn; i < nc-1; i++)
                 {thnx[i] = thnx[i+1];
                  thny[i] = thny[i+1];
                  area[i] = area[i+1];};

/* recompute the areas around that point */
	     iamna = iamn - 1;
	     iamna = max(iamna, 1);
	     iamnb = iamn + 1;
	     iamnb = min(iamnb, nc-2);
             for (i = iamna; i < iamnb; i++)
                 {xc = thnx[i+1] - thnx[i-1];
                  yc = thny[i+1] - thny[i-1];
                  sm = yc/xc;
                  b  = thny[i+1] - sm*thnx[i+1];
                  ht = ABS(thny[i] - sm*thnx[i] - b)/sqrt(sm*sm + 1.0);
                  area[i] = 0.5*ht*HYPOT(xc, yc);};};};

    CFREE(area);

    return(nth);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FILTER_COEFF - replace the y values with new ones obtained by
 *                 - combining the old ones according to a set of supplied
 *                 - constant coefficients
 *                 -
 *                 -   NC is the number of coefficients in the general case
 *                 -      (it must be odd)
 *                 -   COEF is an array of nct coefficients where:
 *                 -
 *                 -   nh = nc/2
 *                 -   nt = total number of coefficients
 *                 -      = nc*[1 + 2*nh*(nh+1)/3]
 *                 -
 *                 -   COEF is layed out as follows:
 *                 -   ni = i*nh + 0.5*i*(i+1)
 *                 -   mi = nt - i*nh - 0.5*i*(i+1)
 *                 -   np = ni for i = nh
 *                 -
 *                 -   ynew[0] = c[n0]*y[0] + ... + c[n1-1]*y[nh]
 *                 -   ynew[1] = c[n1]*y[0] + ... + c[n2-1]*y[nh+1]
 *                 -   ynew[2] = c[n3]*y[0] + ... + c[n3-1]*y[nh+2]
 *                 -                        .
 *                 -                        .
 *                 -
 *                 -   ynew[i] = c[np]*y[i-nh] + ... + c[np+nh-1]*y[i+nh]
 *                 -                        .
 *                 -                        .
 *                 -
 *                 -   ynew[n-3] = c[m3]*y[n-nh-3] + ... + c[m2-1]*y[n-1]
 *                 -   ynew[n-2] = c[m2]*y[n-nh-2] + ... + c[m1-1]*y[n-1]
 *                 -   ynew[n-1] = c[m1]*y[n-nh-1] + ... + c[m0-1]*y[n-1]
 *                 -
 *                 - return TRUE iff successful
 */

int PM_filter_coeff(double *y, int n, double *coef, int nc)
   {int i, k, nh, nm, nu, kn, kx;
    double *ny, *py, *pc, lyn;

    nh = nc >> 1;
    nm = nh*(3*nh + 1) >> 1;
    nu = n - nh;

    ny = CMAKE_N(double, n);

    kn = 0;
    kx = 0;
    for (i = 0; i < n; i++)
        {if (i < nh)
	    {kx  = nh + i + 1;
             pc  = coef + kn;
             py  = y;
             kn += kx;}

         else if (i < nu)
	    {kx = nc;
             pc = coef + kn;
             py = y + i - nh;
	     kn = nm;}

	 else
            {kn += kx;
	     kx = nh + n - i;
             pc = coef + kn;
             py = y + n - kx;};

         lyn = 0.0;
         for (k = 0; k < kx; k++)
             lyn += (*py++)*(*pc++);

         ny[i] = lyn;};

    memcpy(y, ny, n*sizeof(double));
    CFREE(ny);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SMOOTH_INT_AVE - smooth an array Y of N values using PTS points
 *                   - the algorithm does a replacement by integral averages
 *                   - return TRUE iff successful
 */

int PM_smooth_int_ave(double *x, double *y, int n, int pts)
   {int i, k, kn, kx, nh, nt;
    double dx, Dx, ya, xo, xn, yo, lyn;
    double *nx, *ny, *px, *py;

    pts = max(pts, 3);

    nh = pts >> 1;
    nt = n - 1;

    nx = CMAKE_N(double, n);
    ny = CMAKE_N(double, n);

    for (i = 0; i < n; i++)
        {kn = i - nh;
         kn = max(0, kn);

         kx = i + nh;
         kx = min(nt, kx);

         px = x + kn;
         xo = *px++;
         py = y + kn;
         yo = *py++;
         ya = 0.0;
         Dx = SMALL;
         for (k = kn; k < kx; k++)
             {xn  = *px++;
              dx  = ABS(xn - xo);
              Dx += dx;
              xo  = xn;

              lyn = *py++;
              ya += 0.5*(lyn + yo)*dx;
              yo  = lyn;};

         ny[i] = ya/Dx;
         nx[i] = ((kx - i)*x[kn] + (i - kn)*x[kx])/(kx - kn);};

    n *= sizeof(double);
    memcpy(x, nx, n);
    memcpy(y, ny, n);

    CFREE(nx);
    CFREE(ny);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SMOOTH_FFT - smooth an array Y of N values using PTS points
 *               - the algorithm uses FFT, filter, IFFT
 *               - return TRUE iff successful
 */

int PM_smooth_fft(double *x, double *y, int n, int pts,
		  void (*fnc)(complex *cx, int nt, double tol))
   {int i, j, nt, np, nh, rv;
    double xa, xb, yb, ya, dt, idn;
    double *xo;
    complex *cx;

    rv = FALSE;

/* remove the linear trend */
    idn = 1.0/(n - 1);
    xb  = 0.0;
    ya  = idn*y[0];
    yb  = idn*y[n-1];
    for (i = 0; i < n; i++)
        y[i] -= (ya*(n - 1 - i) + yb*i);

/* do the Fourier transform of the data */
    nt = PM_fft_sc_real_data(&cx, &xo, x, y, n, x[0], x[n-1], TRUE);
    if (cx != NULL)
       {nh = nt >> 1;

/* filter the transform */
	if (fnc != NULL)
	   (*fnc)(cx, nt, (double) pts);

/* rearrange for inverse FFT */
	for (i = 0; i < nh; i++)
	    {PM_COMPLEX_SWAP(cx[i], cx[nh+i]);};

/* do the inverse transform */
	PM_fft_sc_complex(cx, nt-1, -1);

/* interpolate the smoothed curve back onto the old mesh */
	np = nt - 1;
	dt = (x[n-1] - x[0])/((double) (np - 1));
	for (i = 0; i < n; i++)
	    {for (j = 0, xa = x[0]; j < np; j++, xa = xb)
	         {xb = xa + dt;
		  if ((xa <= x[i]) && (x[i] < xb))
		     break;};
	     PM_interp(y[i], x[i],
		       xa, creal(cx[j]),
		       xb, creal(cx[j+1]));};

/* restore the linear trend */
	for (i = 0; i < n; i++)
	    y[i] += (ya*(n - 1 - i) + yb*i);

	CFREE(xo);
	CFREE(cx);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SMOOTH_FILTER - a generic smoothing filter for PM_smooth */

void PM_smooth_filter(complex *z, int n, double pts)
   {int i, nh;
    double c0, m0, mlt;

    nh = n >> 1;
    c0 = pts/nh;
    c0 = c0*c0;
    m0 = 1.0/(1.0 - c0);

/* do the low pass filter */
    for (mlt = 1.0, i = 0; i <= nh; i++)
        {if (mlt != 0.0)
            {mlt = m0*(1.0 - c0*i*i);
             mlt = min(mlt, 1.0);
             if (mlt < 0.0)
                mlt = 0.0;
             z[nh+i] *= mlt;
             z[nh-i] *= mlt;}
         else
            {z[nh+i] = CMPLX(0.0, 0.0);
             z[nh-i] = CMPLX(0.0, 0.0);};};

    mlt = (1.0 - 0.25*pts*pts);
    if (mlt < 0.0)
       mlt = 0.0;

    z[nh] = CMPLX(creal(z[nh]), mlt*cimag(z[nh]));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FOLD - fundamental kernel for convolution or correlation integrals
 *          - each curve is assumed to have the same fixed dx
 */

int _PM_fold(int sgn, int na, double *xa, double *ya, int nb,
	     double *xb, double *yb, double **pxf, double **pyf)
   {int i, j, ja, jb, nf, nm, nmn, nt;
    double *xf, *yf, *lyn;
    double Dxa, Dxb, dxa, dxb, xt, yt, xmn, xmx;

    Dxa = xa[na-1] - xa[0];
    Dxb = xb[nb-1] - xb[0];

/* quick feather count */
    dxa = Dxa/((double) (na - 1));
    dxb = Dxb/((double) (nb - 1));
    if (dxa != dxb)
       return(-1);

    if (sgn == 1)
       {xmn = xb[0] - xa[na - 1];
        xmx = xb[nb-1] - xa[0];}
    else
       {xmn = xa[0] - xb[nb-1];
        xmx = xa[na - 1] - xb[0];};

    nf = (xmx - xmn)/dxa + 1;
    xf = *pxf = CMAKE_N(double, nf);
    yf = *pyf = CMAKE_N(double, nf);

/* decide how many points at most are in the product function */
    nm  = 0;
    nmn = min(na, nb);
    for (xt = xmn, i = 0; i < nf; i++, xt += dxa)
        {if ((i < na) && (i < nb))
            nt = i;
         else if ((na <= i) && (nb <= i))
            nt = nf - 1 - i;
         else
            nt = nmn;

         nm = max(nm, nt);};

    lyn = CMAKE_N(double, nm);

    dxb *= 0.5;
    for (xt = xmn, i = 0; i < nf; i++, xt += dxa)
        {xf[i] = xt;

/* decide how many points are in the product function */
         if ((i < na) && (i < nb))
            nt = i;
         else if ((na <= i) && (nb <= i))
            nt = nf - 1 - i;
         else
            nt = nmn;

/* make the product function */
         if (sgn == 1)
            {ja = (i < (na - 1)) ? (na - 1 - i) : 0;
             jb = (i < na) ? 0 : i - na;}
         else
            {ja = (i < nb) ? 0 : i - nb;
             jb = (i < (nb - 1)) ? (nb - 1 - i) : 0;}

         for (j = 0; j < nt; j++)
             lyn[j] = ya[j + ja]*yb[j + jb];

/* integrate the product function */
         yt = 0.0;
         for (j = 1; j < nt; j++)
             yt += dxb*(lyn[j] + lyn[j-1]);

/* stash the value */
         yf[i] = yt;};

    CFREE(lyn);

    return(nf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MACHINE_PRECISION - return a decimal number representing the
 *                      - fractional precision of floating point arithmetic
 *                      - on the current machine
 */

double PM_machine_precision(void)
   {int i;
    double x, y;
    double val[4];

    val[0] = 3.0;
    val[1] = exp(1.0);
    val[2] = sqrt(2.0);
    val[3] = asin(1.0);

    x = 1.0;

/* scale by some non-trivial values */
    for (i = 0; i < 4; i++)
        x *= val[i];

/* restore the original by division */
    for (i = 0; i < 4; i++)
        x /= val[i];

/* what's left should be numerical noise and that's what
 * we want to know
 */
    x = ABS(1.0 - x);

/* on some platforms (LINUX - optimized) we need to clear the
 * state of the floating point unit because the preceeding has
 * possibly left an FPE hanging or about to trip
 */
    PM_clear_fpu();

/* figure out the order of magnitude and the fractional
 * value of the noise
 */
    y = log10(x);
    y = ceil(y);
    x = pow(10.0, y);

    return(x);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ERR - handle PML errors */

void PM_err(char *fmt, ...)
   {

    SC_VA_START(fmt);
    SC_VSNPRINTF(PM_gs.error, MAXLINE, fmt);
    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

