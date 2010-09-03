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

char
 PM_error[MAXLINE];

#ifdef PCK_COMPLEX

double
 cx_ttr = 0.0,
 cx_tti = 0.0,
 cx_ttm = 0.0,
 cx_ttac = 0.0,
 cx_ttbd = 0.0;

complex
 cx_reg;

#endif

/* you cannot trust vendors to handle these declarations in <math.h>
 * some regard them as special extensions which must be specifically
 * asked for - phooey
 */

double
 j0(double x),
 j1(double x),
 jn(int n, double x),
 y0(double x),
 y1(double x),
 yn(int n, double x);

int
 *PM_fft_index(int n),
 _PM_fold(int sgn, int na, double *xa, double *ya, int nb,
	  double *xb, double *yb, double **pxf, double **pyf);

double
 _PM_jn(int n, double x),
 _PM_yn(int n, double x);

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
   {int ret;
    char type[MAXLINE], *token, *s = NULL;

    ret = FALSE;
    if (set != NULL)
       {ret = TRUE;

	strcpy(type, set->element_type);
	token = SC_strtok(type, " (*", s);

	if (token == NULL)
	   ret = FALSE;
	else if (strcmp(token, "double") == 0)
	   set->opers = PM_REAL_Opers;
	else if (strncmp(token, "int", 3) == 0)
	   set->opers = PM_Int_Opers;
	else if (strcmp(token, "long") == 0)
	   set->opers = PM_Long_Opers;
	else
	   ret = FALSE;};

    return(ret);}

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

/* PM_SET_VALUE - broadcast a value into an array */

void PM_set_value(double *p, int n, double f)
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
    double sc, tc, fc;

    ok = TRUE;
    for (i = 0; (i < n) && ok; i++)
        {sc = s[i];
	 tc = t[i];
	 fc = 2.0*(sc - tc)/(ABS(sc) + ABS(tc) + SMALL);
	 ok &= (ABS(fc) < tol);};

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

/*                          ADVANCED MATH FUNCTIONS                         */

/*--------------------------------------------------------------------------*/

/* PM_ROMBERG - Romberg integration routine
 *            - integrates func(x) from x0 to x1 to accuracy of tol
 *            - returned as y0.  Returns number of iterations
 *            - in n (2^(n-1) subdivisions) or -1 if integral has not
 *            - converged within 16 iterations (32768 subdivisions)
 */

double PM_romberg(double (*func)(double x), double x0, double x1, double tol)
   {double y0, toln, h, x;
    double a[15], b[15];
    int i, n, in;

    h = x1 - x0;
    x = x0;
    a[0] = h*((*func)(x0) + (*func)(x1))/2.0;
    toln = 2.0*tol;
    for (n = 2; (n < 16) || (toln > tol); n++)
        {h    = h/2.0;
         b[0] = a[0]/2.0;
         x    = x0 + h;
         in   = (int) POW(2.0, (n-2));
         for (i = 0; i < in; i++)
             {b[0] += h*func(x);
              x = x + 2.0*h;};

         for (i = 1; i < n; i++)
             b[i] = (POW(4.0, i)*b[i-1]-a[i-1])/(POW(4.0, i) - 1.0);

         y0   = b[n-1];

         toln = ABS((y0 - b[n-2])/y0);
         if (toln > tol)
            for (i = 0; i < n; i++)
                a[i] = b[i];};

      return(y0);}

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
    double x1, x2, y1, y2, yt;

    x1 = *fncx++;
    y1 = *fncy++;
    for (i = 1; i < n; i++)
        {x2 = *fncx++;
         y2 = *fncy++;
         *derx++ = 0.5*(x1 + x2);
         if (x2 != x1)
            *dery++ = (y2 - y1)/(x2 - x1);
         else
	    {yt = dery[-1];
	     *dery++ = yt;};
         x1 = x2;
         y1 = y2;};

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
    double x1, x2, y1, y2, xt, dydx, odydx;

    if (toler == HUGE)
       {toler = 10.0/((double) n);
        toler = min(toler, 0.1);};

    x1 = *fncx++;
    y1 = *fncy++;
    x2 = x1;
    y2 = y1;

    j  = 0;
    i0 = -1;

    odydx = -HUGE;
    for (i = 1; i < n; i++)
        {x2 = *fncx++;
         y2 = *fncy++;

         if (x2 != x1)
            dydx = (y2 - y1)/(x2 - x1);
         else
	    dydx = y2 > y1 ? HUGE : -HUGE;

         xt = 0.5*ABS(dydx - odydx)/(ABS(dydx) + ABS(odydx) + SMALL);
         if (xt > toler)
            {if (i0 < i-1)
                {thnx[j] = x1;
                 thny[j] = y1;
                 j++;};

	     odydx = dydx;
             i0    = i;

             thnx[j] = x2;
             thny[j] = y2;
             j++;};

         x1 = x2;
         y1 = y2;};

    if (x2 != thnx[j-1])
       {thnx[j] = x2;
        thny[j] = y2;
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
    area = FMAKE_N(double, n, "PM_THIN_1D_INT:area");
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

    SFREE(area);

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
   {int i, j, nh, nm, nu, jn, jx;
    double *ny, *py, *pc, yn;

    nh = nc >> 1;
    nm = nh*(3*nh + 1) >> 1;
    nu = n - nh;

    ny = FMAKE_N(double, n, "PM_FILTER_COEFF:ny");

    jn = 0;
    jx = 0;
    for (i = 0; i < n; i++)
        {if (i < nh)
	    {jx = nh + i + 1;
             pc = coef + jn;
             py = y;
             jn += jx;}

         else if (i < nu)
	    {jx = nc;
             pc = coef + jn;
             py = y + i - nh;
	     jn = nm;}

	 else
            {jn += jx;
	     jx = nh + n - i;
             pc = coef + jn;
             py = y + n - jx;};

         yn = 0.0;
         for (j = 0; j < jx; j++)
             yn += (*py++)*(*pc++);

         ny[i] = yn;};

    memcpy(y, ny, n*sizeof(double));
    SFREE(ny);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SMOOTH_INT_AVE - smooth an array Y of N values using PTS points
 *                   - the algorithm does a replacement by integral averages
 *                   - return TRUE iff successful
 */

int PM_smooth_int_ave(double *x, double *y, int n, int pts)
   {int i, j, jn, jx, nh, nt;
    double dx, Dx, ya, xo, xn, yo, yn;
    double *nx, *ny, *px, *py;

    pts = max(pts, 3);

    nh = pts >> 1;
    nt = n - 1;

    nx = FMAKE_N(double, n, "PM_SMOOTH_INT_AVE:nx");
    ny = FMAKE_N(double, n, "PM_SMOOTH_INT_AVE:ny");

    for (i = 0; i < n; i++)
        {jn = i - nh;
         jn = max(0, jn);

         jx = i + nh;
         jx = min(nt, jx);

         px = x + jn;
         xo = *px++;
         py = y + jn;
         yo = *py++;
         ya = 0.0;
         Dx = SMALL;
         for (j = jn; j < jx; j++)
             {xn  = *px++;
              dx  = ABS(xn - xo);
              Dx += dx;
              xo  = xn;

              yn  = *py++;
              ya += 0.5*(yn + yo)*dx;
              yo  = yn;};

         ny[i] = ya/Dx;
         nx[i] = ((jx - i)*x[jn] + (i - jn)*x[jx])/(jx - jn);};

    n *= sizeof(double);
    memcpy(x, nx, n);
    memcpy(y, ny, n);

    SFREE(nx);
    SFREE(ny);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SMOOTH_FFT - smooth an array Y of N values using PTS points
 *               - the algorithm uses FFT, filter, IFFT
 *               - return TRUE iff successful
 */

int PM_smooth_fft(double *x, double *y, int n, int pts,
		  void (*fnc)(complex *cx, int nt, double tol))
   {int i, j, nt, np, nh;
    double xa, xb, yb, ya, dt, idn;
    double *xo;
    complex *cx;

/* remove the linear trend */
    idn = 1.0/(n - 1);
    xb  = 0.0;
    ya  = idn*y[0];
    yb  = idn*y[n-1];
    for (i = 0; i < n; i++)
        y[i] -= (ya*(n - 1 - i) + yb*i);

/* do the Fourier transform of the data */
    nt = PM_fft_sc_real_data(&cx, &xo, x, y, n, x[0], x[n-1], TRUE);

    nh = nt >> 1;

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
		   xa, PM_REAL_C(cx[j]),
		   xb, PM_REAL_C(cx[j+1]));};

/* restore the linear trend */
    for (i = 0; i < n; i++)
        y[i] += (ya*(n - 1 - i) + yb*i);

    SFREE(xo);
    SFREE(cx);

    return(TRUE);}

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
             z[nh+i] = PM_TIMES_RC(mlt, z[nh+i]);
             z[nh-i] = PM_TIMES_RC(mlt, z[nh-i]);}
         else
            {z[nh+i] = PM_COMPLEX(0.0, 0.0);
             z[nh-i] = PM_COMPLEX(0.0, 0.0);};};

    mlt = (1.0 - 0.25*pts*pts);
    if (mlt < 0.0)
       mlt = 0.0;

    z[nh] = PM_COMPLEX(PM_REAL_C(z[nh]), mlt*PM_IMAGINARY_C(z[nh]));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FOLD - fundamental kernel for convolution or correlation integrals
 *          - each curve is assumed to have the same fixed dx
 */

int _PM_fold(int sgn, int na, double *xa, double *ya, int nb,
	     double *xb, double *yb, double **pxf, double **pyf)
   {int i, j, ja, jb, nf, nm, nmn, nt;
    double *xf, *yf, *yn;
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
    xf = *pxf = FMAKE_N(double, nf, "_PM_FOLD:xf");
    yf = *pyf = FMAKE_N(double, nf, "_PM_FOLD:yf");

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

    yn = FMAKE_N(double, nm, "_PM_FOLD:yn");

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
             yn[j] = ya[j + ja]*yb[j + jb];

/* integrate the product function */
         yt = 0.0;
         for (j = 1; j < nt; j++)
             yt += dxb*(yn[j] + yn[j-1]);

/* stash the value */
         yf[i] = yt;};

    SFREE(yn);

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
    SC_VSNPRINTF(PM_error, MAXLINE, fmt);
    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_VALUE_EQUAL - return TRUE iff X1 == X2 to tolerance TOL */

int PM_value_equal(double x1, double x2, double tol)
   {int rv;
    double x1a, x2a;

    if (tol < 0.0)
       tol = TOLERANCE;

    rv  = TRUE;
    x1a = ABS(x1);
    x2a = ABS(x2);
    if ((tol < x1a) || (tol < x2a))
       rv &= (2.0*ABS(x1 - x2)/(x1a + x2a + SMALL) < tol);

    return(rv);}

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

