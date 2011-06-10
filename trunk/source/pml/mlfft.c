/*
 * MLFFT.C - Fast Fourier Transform routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_UNIFORM_REAL_X - compute evenly spaced x values for the given x array */

double *PM_uniform_real_x(int no, double xmin, double xmax, int flag)
   {int i, na;
    double step;
    double *x;

    x = CMAKE_N(double, no);

    na = (flag == 1) ? no - 1 : no;

    step = (xmax - xmin)/(na - 1);
    for (i = 0; i < na; i++)
        x[i] = xmin + step*i;

    x[no-1] = xmax;

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_UNIFORM_REAL_Y - compute evenly spaced real Y values
 *                   - for the given (X, Y) pairs
 *                   -   XI,YI  input x and y values
 *                   -   XO,YO  output evenly spaced x and y values
 *                   -   NI     number of points in input arrays
 *                   -   NO     number of points in output arrays
 */

double *PM_uniform_real_y(int no, double *xo,
			int ni, double *xi, double *yi)
   {int i, j;
    double dx, xa, xb;
    double *yo, *py;

    yo = CMAKE_N(double, no);

    if (xi == NULL)
       {xa = xo[0];
        if (xa == 0.0)
	   dx = (xo[no-1] - xa)/(ni - 1.0);
	else
	   dx = xo[1] - xa;
	xb = xa + dx;

	for (i = 0, j = 0; i < no; i++)
	    {while ((xo[i] > xb) && (j < ni - 2))
	        {j++;
		 xa += dx;
		 xb += dx;};
	     py = yi + j;
	     PM_interp(yo[i], xo[i], xa, py[0], xb, py[1]);};}

    else
       {xa = xi[0];
	xb = xi[1];

	for (i = 0, j = 0; i < no; i++)
	    {while (xo[i] > xb)
	        {j++;
		 xa = xi[j];
		 xb = xi[j+1];};
	     py = yi + j;
	     PM_interp(yo[i], xo[i], xa, py[0], xb, py[1]);};};

    return(yo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_UNIFORM_COMPLEX_Y - compute evenly spaced complex Y values
 *                      - for the given (X, Y) pairs
 *                      -   XI,YI  input x and y values
 *                      -   XO,YO  output evenly spaced x and y values
 *                      -   NI     number of points in input arrays
 *                      -   NO     number of points in output arrays
 */

complex *PM_uniform_complex_y(int no, double *xo,
			      int ni, double *xi, complex *yi)
   {int i;
    double *yti, *yto;
    complex *yo;

    yo  = CMAKE_N(complex, no);
    yti = CMAKE_N(double, ni);
    if ((yti != NULL) && (yo != NULL))

/* interpolate the real part of y */
       {for (i = 0; i < ni; i++)
	    yti[i] = PM_REAL_C(yi[i]);

	yto = PM_uniform_real_y(no, xo, ni, xi, yti);

	for (i = 0; i < no; i++)
	    yo[i] = PM_COMPLEX(yto[i], PM_IMAGINARY_C(yo[i]));

	CFREE(yto);

/* interpolate the imaginary part of y */
	for (i = 0; i < ni; i++)
	    yti[i] = PM_IMAGINARY_C(yi[i]);

	yto = PM_uniform_real_y(no, xo, ni, xi, yti);

	for (i = 0; i < no; i++)
	    yo[i] = PM_COMPLEX(PM_REAL_C(yo[i]), yto[i]);

	CFREE(yto);
	CFREE(yti);};

    return(yo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NEXT_EXP_TWO - return the largest M such that 2^M <= N
 *
 * #bind PM_next_exp_two fortran() scheme()
 */

int PM_next_exp_two(int n)
   {int i, m;

    for (m = 0, i = n; i > 1; i >>= 1, m++);

    return(m);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NEXT_POWER_TWO - return the smallest integer, P,  such that
 *                   - |P| >= |N| and |P| is an integer power of 2
 */

int PM_next_power_two(int n)
   {int sgn, p;

    sgn = 1;
    if (n != 0)
       {sgn  = abs(n)/n;
	n   *= sgn;};

    for (p = 1; n > p; p <<= 1);

    return(sgn*p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NEAR_POWER - return the number nearest N which is a power of A */

int PM_near_power(int n, int a)
   {int i, d;

    i = PM_round(log((double) n)/log((double) a));
    d = (int) (POW((double) a, (double) i) + .999);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FFT_FIN - complete an FFT including such things as:
 *             -   normalizing the frequency axis
 *             -   reordering the transform into intuitive order
 *             -   introducing a phase shift to get IFFT to match original
 */

int _PM_fft_fin(complex *y, double *x, int np, int flag, int ordr)
   {int i, n, nh, nr;
    double dt;

    n  = np - 1;
    nh = np >> 1;
    dt = 1.0/(x[n-1] - x[0]);

/* make proper frequency axis values */
    if (flag == 1)
       {for (i = 0; i <= nh; i++)
	    {x[nh+i] =  dt*i;
	     x[nh-i] = -dt*i;};

/* if the transform was requested in intuitive order, reorder the array */
	if (ordr)
	   {for (i = 0; i < nh; i++)
	        {PM_COMPLEX_SWAP(y[i], y[nh+i]);};
	    y[2*nh] = y[0];};

        nr = np;}

/* make proper spatial axis values */
    else
       {for (i = 0; i < n; i++)
            x[i] = dt*i;

        nr = n;};

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FFT_SC_REAL_HSP - perform an FFT on a scalar set of real data in
 *                     - Half the SPace required by PM_fft_sc_real
 *                     - used as helper for convolution routine
 *                     - unlikely to be useful in it own right since the
 *                     - ordering of the transforms is NOT user friendly
 *                     - adapted from Numerical Recipes in C
 */

static int _PM_fft_sc_real_hsp(double *f, int n, int sgn)
   {int i, i1, i2, i3, i4, na, nh;
    double c1, c2, h1r, h1i, h2r, h2i;
    double wr, wi, dwr, dwi, wt, dth;

    dth = PI/((double) n);
    if (sgn == 1)
       {PM_fft_sc_complex((complex *) f, n, sgn);
	c1 = 0.5;
	c2 = -0.25*n;}
    else
       {c1  = 1.0;
        c2  = 1.0;
	dth = -dth;};

    wr  = cos(dth);
    wi  = sin(dth);

    dwr = wr - 1.0;
    dwi = wi;

    na  = 2*n + 1;
    nh  = n >> 1;
    for (i = 1; i < nh; i++)
        {i1 = 2*i;
         i2 = i1 + 1;
         i3 = na - i2;
         i4 = i3 + 1;

	 h1r =  c1*(f[i1] + f[i3]);
	 h1i =  c1*(f[i2] - f[i4]);
	 h2r = -c2*(f[i2] + f[i4]);
	 h2i =  c2*(f[i1] - f[i3]);

	 f[i1] =  h1r + wr*h2r - wi*h2i;
	 f[i2] =  h1i + wr*h2i + wi*h2r;
	 f[i3] =  h1r - wr*h2r + wi*h2i;
	 f[i4] = -h1i + wr*h2i + wi*h2r;

	 wt = wr;
	 wr = wt*dwr - wi*dwi + wr;
	 wi = wi*dwr + wt*dwi + wi;};

    if (sgn == 1)
       {h1r  = f[0];
        c2   = 0.5*n;
        f[0] = c2*(h1r + f[1]);
	f[1] = c2*(h1r - f[1]);}

    else
       {h1r  = f[0];
        f[0] = h1r + f[1];
	f[1] = h1r - f[1];

	PM_fft_sc_complex((complex *) f, n, sgn);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFT_SC_COMPLEX - perform an FFT on a scalar set of complex data
 *                   - DATA is an array of N complex values
 *                   - if FLAG > 0 do FFT and do inv(FFT) otherwise
 *                   - return TRUE iff successful
 */

int PM_fft_sc_complex(complex *x, int n, int flag)
   {int m, mmax, istep;
    complex temp, w, dw;
    unsigned int i, j;
    double nrm, dth, th0;

    m = PM_next_exp_two(n);

/* permute series */
    for (i = 0; i <= n; i++)
        {j = SC_bit_reverse(i, m);
         if (i < j)
            {temp = x[j];
             x[j] = x[i];
             x[i] = temp;};};
        
    th0 = (flag > 0) ? PI : -PI;

/* transform series */
    mmax = 1;
    while (mmax < n)
       {istep = mmax << 1;
        dth   = th0/((double) mmax);
        dw    = PM_COMPLEX(cos(dth), sin(dth));
        w     = PM_COMPLEX(1.0, 0.0);
        for (m = 1; m <= mmax; m++)
            {for (i = m-1; i < n; i += istep)
                 {j = i + mmax;

                  temp = PM_TIMES_CC(w, x[j]);
                  x[j] = PM_MINUS_CC(x[i], temp);
                  x[i] = PM_PLUS_CC(x[i], temp);};

             w = PM_TIMES_CC(w, dw);};
        mmax = istep;};

    x[n] = x[0];

/* fix normalization */
    if (flag == 1)
       nrm = 2.0/((double) n);
    else
       nrm = 0.5;

    for (i = 0; i <= n; i++)
        x[i] = PM_TIMES_RC(nrm, x[i]);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFT_SC_REAL - perform an FFT on a scalar set of real data
 *                - X is an array of N (N must power of 2) evenly spaced
 *                - real values (use PM_fft_sc_real_data for standard data
 *                - sets encountered in practice)
 *                - if FLAG > 0 do FFT and do inv(FFT) otherwise
 *                - return array of complex values iff successful
 *                - return NULL otherwise
 *                - formerly in ULTRA
 */

complex *PM_fft_sc_real(double *x, int n, int flag)
   {int i;
    complex *cx;

    cx = CMAKE_N(complex, n + 1);
    if (cx == NULL)
       {PM_err("CAN'T ALLOCATE SPACE - PM_FFT_SC_REAL");
        return(NULL);};

    for (i = 0; i <= n; i++)
        cx[i] = PM_COMPLEX(x[i], 0.0);

    PM_fft_sc_complex(cx, n, flag);

    return(cx);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFT_SC_REAL_DATA - do FFT on set of real data which is not necessarily
 *                     - evenly spaced
 *                     - returned via the argument list are the frequency
 *                     - values from most negative to most positive
 *                     - and the complex transform values can be in the same
 *                     - order as the frequency array (this is counter to the
 *                     - standard FFT practice but is the way one thinks of
 *                     - the transform data (this is controlled by ORDR)
 *                     - return the number of points in the transform arrays
 *                     - if successful and 0 otherwise
 *
 * #bind PM_fft_sc_real_data fortran() scheme()
 */

int PM_fft_sc_real_data(complex **pyo, double **pxo, double *xi, double *yi,
			int ni, double xmn, double xmx, int ordr)
   {int n, np, nr;
    double *x, *y;
    complex *cy;

    n  = PM_near_power(ni, 2);
    np = n + 1;

/* generate evenly spaced x values for interpolation */
    x = PM_uniform_real_x(np, xmn, xmx, 1);

/* interpolate to find evenly spaced y's */
    y = PM_uniform_real_y(np, x, ni, xi, yi);

    cy = PM_fft_sc_real(y, n, 1);

    CFREE(y);

    nr = _PM_fft_fin(cy, x, np, 1, ordr);

    *pyo = cy;
    if (pxo == NULL)
       {CFREE(x);}
    else
       *pxo = x;

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFT_SC_COMPLEX_DATA - do FFT on set of complex data which is not
 *                        - necessarily evenly spaced
 *                        - FLAG is 1 for FFT and -1 for inv(FFT)
 *                        - returned via the argument list are the frequency
 *                        - values from most negative to most positive
 *                        - and the complex transform values can be in the
 *                        - same order as the frequency array (this is
 *                        - counter to the standard FFT practice but is
 *                        - the way one thinks of the transform data
 *                        - (this is controlled by ORDR)
 *                        - return the number of points in the transform
 *                        - arrays if successful and 0 otherwise
 *
 * #bind PM_fft_sc_complex_data fortran() scheme()
 */

int PM_fft_sc_complex_data(complex **pyo, double **pxo, double *xi,
			   complex *yi, int ni, double xmn, double xmx,
			   int flag, int ordr)
   {int i, n, nh, np, nr;
    double *x;
    complex *y, *ay;

    n  = PM_near_power(ni, 2);
    np = n + 1;
    nh = n >> 1;

/* generate evenly spaced x values for interpolation */
    x = PM_uniform_real_x(np, xmn, xmx, flag);

/* interpolate to find evenly spaced y's */
    y = PM_uniform_complex_y(np, x, ni, xi, yi);

/* reorder to high/low frequency order */
    if ((flag != 1) && (ordr != 0))
       {ay = CMAKE_N(complex, np);

	for (i = 0; i < np; i++)
	    ay[i] = y[i];

	for (i = 0; i <= nh; i++)
	    {y[i]   = ay[nh+i];   /* this one needs to be first */
	     y[n-i] = ay[nh-i];};

	CFREE(ay);};

    PM_fft_sc_complex(y, n, flag);
        
    nr = _PM_fft_fin(y, x, np, flag, ordr);

    *pyo = y;
    if (pxo == NULL)
       {CFREE(x);}
    else
       *pxo = x;

    return(nr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FFT_SC_REAL_SIMUL - do simultaneous FFT's on two real functions
 *                      - adapted from Numerical Recipes in C
 */

int PM_fft_sc_real_simul(double *fx1, double *fx2, double *fw1, double *fw2, int n)
   {int i, j, nb, na;
    double rep, rem, aip, aim, nrm;

    for (j = 0, i = 1; j < n; j++, i += 2)
        {fw1[i-1] = fx1[j];
	 fw1[i]   = fx2[j];};

    PM_fft_sc_complex((complex *) fw1, n, 1);

    na = n << 1;
    nb = na + 1;

/* fix normalization */
    nrm = n/2.0;

    fw2[0]  = nrm*fw1[1];
    fw1[0] *= nrm;
    fw1[1] = fw2[1] = 0.0;

    nrm *= 0.5;
    for (j = 2; j < n+1; j += 2)
        {rep = nrm*(fw1[j] + fw1[na-j]);
	 rem = nrm*(fw1[j] - fw1[na-j]);
	 aip = nrm*(fw1[j+1] + fw1[nb-j]);
	 aim = nrm*(fw1[j+1] - fw1[nb-j]);

	 fw1[j]    = rep;
	 fw1[j+1]  = aim;
	 fw1[na-j] = rep;
	 fw1[nb-j] = -aim;

	 fw2[j]    = aip;
	 fw2[j+1]  = -rem;
	 fw2[na-j] = aip;
	 fw2[nb-j] = rem;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                        FFT CONVOLUTION ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* PM_CONVOLVE - convolve two data sets regardless of their coordinate
 *             - spacings
 *             - for some users H is the response function
 *             - and G is the signal function
 *             - contributed by Charles McMillan 8/94
 */

int PM_convolve(double *gx, double *gy, int gn, double *hx, double *hy,
	        int hn, double dt, double **pxr, double **pyr, int *pnr)
   {int i, j, gtn, hin;
    double hdx, hxmn, hxmx;
    double gxmn, gxmx;
    double cxmn, cxmx;
    double igy, ihy, init_y, init_x, vx, nrm;
    double *gty, *hty, *xret, *yret;
    double *hix, *hiy;
    double *gdy, *hdy;
  
    gdy = PM_compute_splines(gx, gy, gn, HUGE, HUGE);
    if (gdy == NULL)
       return(FALSE);

    hdy = PM_compute_splines(hx, hy, hn, HUGE, HUGE);
    if (hdy == NULL) 
       return(FALSE);

/* setup limits, find number of points */
    gxmn = gx[0];
    gxmx = gx[gn-1];
    hxmn = hx[0];
    hxmx = hx[hn-1];
    hdx  = hxmx - hxmn;

    cxmn = gxmn - hdx;
    cxmx = gxmx + hdx;

    hin = hdx/dt;
    gtn = (cxmx - cxmn + 2*hdx)/dt;
    gtn = PM_next_power_two(gtn);

    xret = CMAKE_N(double, gtn);
    gty  = CMAKE_N(double, gtn);
    hty  = CMAKE_N(double, gtn);
    hix  = CMAKE_N(double, hin);
    hiy  = CMAKE_N(double, hin);

/* interpolate signal */
    init_y = gy[0];
    for (i = 0; i < gtn; i++)
        {vx = cxmn + i*dt;
         xret[i] = vx;
	 if ((gxmn <= vx) && (vx <= gxmx))
	    {gty[i]  = PM_cubic_spline_int(gx, gy, gdy, gn, vx);
	     gty[i] -= init_y;}

/* pad the ends of the array */
	 else
	    gty[i] = 0;};

/* interpolate response */
    for (i = 0; i < hin; i++)
        {vx = hxmn + i*dt;
         hix[i] = vx;
	 hiy[i] = PM_cubic_spline_int(hx, hy, hdy, hn, vx);};

/* reorder response for fft convolution */
    init_x = 0;
    for (i = 0; hix[i] < 0; i++);
    j = i;
    if (j == 0)
       init_x = hxmn;
    if (j == hin)
       init_x = hxmx;

/* arrays are born zero but its cheap insurance */
    for (i = 0; i < gtn; i++)
        hty[i] = 0;

    for (i = j; i < hin; i++)
        hty[i-j] = hiy[i];

    for (i = 0; i < j; i++)
        hty[gtn-j+i] = hiy[i];

/* calculate the integrals for normalization */
    igy = 0.0;
    ihy = 0.0;
    for (i = 0; i < gtn; i++)
	{igy += gty[i];
	 ihy += hty[i];};
    nrm = min(igy, ihy);
    nrm = 1.0/nrm;

/* call fft convolution */
    yret = CMAKE_N(double, 2*gtn);
    PM_convolve_logical(gty, gtn, hty, gtn, 1, yret);
  
    CFREE(hix);
    CFREE(hiy);
    CFREE(gty);
    CFREE(hty);

/* replace initial value */
    for (i = 0; xret[i] < cxmx; i++)
        {yret[i]  = nrm*yret[i] + init_y;
	 xret[i] += init_x;};

    *pnr = i;
    *pxr = xret;
    *pyr = yret;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CONVOLVE_LOGICAL - compute and return the convolution of G and H in CNV
 *                     - this is done on an implied grid of uniform spacing
 *                     - where the G and H are defined on possibly different
 *                     - parts of the same grid
 *                     - adapted from Numerical Recipes in C
 */

int PM_convolve_logical(double *g, int n, double *h, int m, int sgn, double *cnv)
   {int i, nh, mh;
    double t, mg, in, a, b;
    double *ft;

    if (sgn*sgn != 1)
       return(FALSE);

    ft = CMAKE_N(double, 2*n);

/* pad h in the middle */
    mh = (m - 1)/2;
    for (i = 0; i < mh; i++)
        h[n+1-i] = h[m+1-i];

    for (i = mh + 1; i < n - mh; i++)
        h[i] = 0.0;

    PM_fft_sc_real_simul(g, h, ft, cnv, n);

    nh = n/2;
    in = 1.0/nh;
    for (i = 1; i < n+2; i += 2)
        {t = cnv[i-1];
         if (sgn == 1)
	    {mg = in;
	     cnv[i-1] = (ft[i-1]*t - ft[i]*cnv[i])*mg;
	     cnv[i]   = (ft[i]*t + ft[i-1]*cnv[i])*mg;}
         else
	    {a = cnv[i];
             b = cnv[i-1];
	     mg = a*a + b*b;
	     if (mg == 0.0)
	        return(FALSE);
	     mg = in/mg;

	     cnv[i-1] = (ft[i-1]*t + ft[i]*cnv[i])*mg;
	     cnv[i]   = (ft[i]*t - ft[i-1]*cnv[i])*mg;};};

    cnv[1] = cnv[n];

    _PM_fft_sc_real_hsp(cnv, nh, -1);

    CFREE(ft);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
