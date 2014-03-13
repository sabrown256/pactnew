/*
 * MLMEXTR.C - logical-rectangular mesh extrapolation routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pml_int.h"

#define NODE_OF(k, l) (((l) - 1)*kmax + (k) - 1)

#define INDEX(_i, _j, _k, _di, _dj)   (((_k)*(_dj) + (_j))*(_di) + (_i))

#define SETUP_LIMITS(mn, mx, smn, smx, id, side, nd,                         \
		     nn, imn, imx, jmn, jmx, kmn, kmx, din, djn, dkn)        \
   {int oi, oj, ok;                                                          \
    imn = jmn = kmn = INT_MAX;                                               \
    imx = jmx = kmx = INT_MIN;                                               \
    nn  = PM_ipow(3, nd);                                                    \
    din = mx[0] - mn[0] + 1;                                                 \
    djn = mx[1] - mn[1] + 1;                                                 \
    dkn = mx[2] - mn[2] + 1;                                                 \
    oi  = mn[0];                                                             \
    oj  = (nd > 1) ? mn[1] : 0;                                              \
    ok  = (nd > 2) ? mn[2] : 0;                                              \
    switch (id)                                                              \
       {case 0 :                                                             \
	     imn = (side > 0) ? smn[0] : smx[0];                             \
	     imn -= side;                                                    \
	     imx  = imn;                                                     \
	     jmn = smn[1];                                                   \
	     jmx = smx[1];                                                   \
	     kmn = smn[2];                                                   \
	     kmx = smx[2];                                                   \
	     break;                                                          \
        case 1 :                                                             \
	     imn = smn[0];                                                   \
	     imx = smx[0];                                                   \
	     jmn = (side > 0) ? smn[1] : smx[1];                             \
	     jmn -= side;                                                    \
	     jmx  = jmn;                                                     \
	     kmn = smn[2];                                                   \
	     kmx = smx[2];                                                   \
	     break;                                                          \
        case 2 :                                                             \
	     imn = smn[0];                                                   \
	     imx = smx[0];                                                   \
	     jmn = smn[1];                                                   \
	     jmx = smx[1];                                                   \
	     kmn = (side > 0) ? smn[2] : smx[2];                             \
	     kmn -= side;                                                    \
	     kmx  = kmn;                                                     \
	     break;};                                                        \
    imn -= oi;                                                               \
    jmn -= oj;                                                               \
    kmn -= ok;                                                               \
    imx -= oi;                                                               \
    jmx -= oj;                                                               \
    kmx -= ok;                                                               \
    imx = max(imx, imn);                                                     \
    jmx = max(jmx, jmn);                                                     \
    kmx = max(kmx, kmn);                                                     \
    switch (nd)                                                              \
       {case 1 :                                                             \
             jmn = 0;                                                        \
	     jmx = 0;                                                        \
        case 2:                                                              \
             kmn = 0;                                                        \
	     kmx = 0;};}

#define vecset4(v,v1,v2,v3,v4)                                               \
   v2 = v;                                                                   \
   v3 = v2 - 1;                                                              \
   v4 = v3 - kmax;                                                           \
   v1 = v4 + 1

#define FIND_ELEMENT(off, i0)                                                \
   {i = PM_element(n_map, j, 1) + off;                                       \
    for (m = 1; m <= n; m++)                                                 \
        {if (PM_element(n_map, m, 1) == i)                                   \
            PM_element(n_map, j, i0) = m;};}

#define SET_LAPL(_a, _ia, _oa, _cf, _in, _of)                                \
   {_a  = _cf;                                                               \
    _ia = _in;                                                               \
    _oa = _of;}

static int
 debug = FALSE;

/*--------------------------------------------------------------------------*/

/*                            DEBUG ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* SHOW_MESH - lets see the mesh at this point */

static void show_mesh(double *x, int nd, int *mn, int *mx,
		      int idr, int side, int ig)
   {

#if 0

    int id, din, djn, nn;
    int maxes[2];
    char bf[MAXLINE];
    PM_set *dom;
    double **y;

#include <pgs.h>

    static PG_device *dev = NULL;

    if (dev == NULL)
       {dev = PG_make_device("SCREEN", "COLOR", "Mesh Debug");
	PG_open_device(dev, 0.01, 0.01, 0.4, 0.4);};

    din = mx[0] - mn[0] + 1;
    djn = mx[1] - mn[1] + 1;
    nn  = din*djn;

    maxes[0] = din;
    maxes[1] = djn;

    y = PM_make_vectors(nd, nn);
    for (id = 0; id < nd; id++)
        {y[id] = x + id*nn;
	 PM_array_copy(y[id], x+id*nn, nn);};

/* build the set */
    snprintf(bf, MAXLINE, "Dir = %d, Side = %d, I = %d", idr, side, ig);
    dom = PM_mk_set(bf, SC_DOUBLE_S, FALSE, nn, nd, nd,
		    maxes, y, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    PG_clear_screen(dev);

    PG_domain_plot(dev, dom, NULL);

    PM_rel_set(dom, FALSE);

    SC_pause();

#endif

    return;}

/*--------------------------------------------------------------------------*/

/*                           SERVICE ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _PM_LOAD_HIER_TRIDI - load up the hierarchical tridi array A
 *                     - and the vector of known values B
 *                     - to solve NA systems of A.Xi = Bi
 *                     - where 0 < i < NA
 */

static void _PM_load_hier_tridi(PM_sp_lin_sys *axb, int i, int na,
				PM_matrix *n_map,
				double **x, int n, double *av,
				int *unt, int *str)
   {int im, j, k, m, io, ih, ihm, jo, no, nr;
    double lij, ya;
    double *px, *py, *pb;

    im = i - 1;

    ih = n >> 1;
    no = ((n/3) >> 1) + 1;

/* place the contributions to Bk (RHS) */
    for (k = 0; k < na; k++)
        {px = x[k];
	 pb = axb->b[k];
	 py = axb->x[k];

	 m  = PM_element(n_map, i, ih+1);
	 ya = (m >= 0) ? px[m] : 0.0;

	 lij = 0.0;
	 jo  = 0;
	 for (j = 0; j < n; j++)

/* do simple extrapolation to make initial guess at solution */
	     {if (unt[j] == 0)
		 {jo++;
		  if (jo == no)
		     {ihm = (ih << 1) - j;
		      m   = PM_element(n_map, i, ihm+1);
		      ya  = 2.0*ya - px[m];};}

/* sum up the contributions from the known points for the RHS */
	      else
		 {m = PM_element(n_map, i, j+1);
		  if (m >= 0)
		     lij -= (av[j]*px[m]);};};

	 pb[im] = lij;
         py[im] = ya;};

/* place the contributions to A */
    m  = 0;
    nr = axb->n_ups;
    for (j = 0; j < n; j++)
        {if (unt[j] == 0)
	    {io = im + str[m];
	     if ((0 <= io) && (io < nr))
	        PM_set_sls_coef(axb, im, io, av[j]);

	     m++;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SOLV_LAPL - solve NA systems B = A.X
 *               - return the solution in X
 *               - B and A have been setup prior to this call which is
 *               - only to solve the matrix equation
 *               - NR is the index in N_MAP of the node whose value is
 *               - replaced by the solution
 *               - IPS is a pivot array
 *               - NOTE: this should be done with a multi-block tridi
 *               - approach rather than LU decomposition
 */

static void _PM_solv_lapl(double **x, PM_sp_lin_sys *axb, PM_matrix *n_map,
                          int nr, int na)
   {int n, i, j, k, ni, itmx;
    double tol;
    double *pxi, *pxo;

    ni   = 0;
    tol  = PM_machine_precision();
    itmx = 1000;

/*    if (PM_sls_sym_chk(axb)) */
    if (TRUE)
       {for (i = 0; i < na; i++)
	    ni += PM_dscg(axb, i, &itmx, &tol);}

     else
       {for (i = 0; i < na; i++)
	    ni += PM_dsbicg(axb, i, &itmx, &tol);};

     n = n_map->nrow;

     for (k = 0; k < na; k++)
         {pxi = axb->x[k];
	  pxo = x[k];
	  for (j = 1; j <= n; j++)
	      {i = PM_element(n_map, j, nr);
	       pxo[i] = pxi[j-1];};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FILL_MAP - fill out the map array with nearest neighbor information
 *              - MN, MX describe the entire patch
 *              - SMN, SMX describe the subset on which coordinates are
 *              - currently known
 */

static PM_matrix *_PM_fill_map(int id, int side, int nd,
                               int *smn, int *smx, int *mn, int *mx)
   {int i, j, k, l, n, m, jd, np, nn;
    int io, jo, ko;
    int imn, imx, jmn, jmx, kmn, kmx;
    int din, djn, dkn, dil, djl, dkl, off;
    PM_matrix *n_map;

    SETUP_LIMITS(mn, mx, smn, smx, id, side, nd,
		 nn, imn, imx, jmn, jmx, kmn, kmx, din, djn, dkn);

    np = 1;
    for (jd = 0; jd < nd; jd++)
        {if (jd != id)
	    np *= (smx[jd] - smn[jd] + 1);};

    n_map = PM_create(np, nn);

    dil = din - 1;
    djl = (nd > 1) ? djn - 1 : -1;
    dkl = (nd > 2) ? dkn - 1 : -1;

/* put the nodes into the n_map array */
    m = 1;
    for (k = kmn; k <= kmx; k++)
        for (j = jmn; j <= jmx; j++)
            for (i = imn; i <= imx; i++)
                {l = INDEX(i, j, k, din, djn);

		 for (n = 0; n < nn; n++)
		     {io = (n % 3) - 1;
		      jo = (nd > 1) ? ((n / 3) % 3) - 1 : 0;
		      ko = (nd > 2) ? (n % 9) - 1 : 0;

		      if (((i == 0) && (io < 0)) ||
			  ((i == dil) && (io > 0)) ||
			  ((j == 0) && (jo < 0)) ||
			  ((j == djl) && (jo > 0)) ||
			  ((k == 0) && (ko < 0)) ||
			  ((k == dkl) && (ko > 0)))
			 PM_element(n_map, m, n+1) = -1;

		      else
			 {off = io + din*(jo + djn*ko);
			  PM_element(n_map, m, n+1) = l + off;};};

		 m++;};

    return(n_map);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FILL_UNKNOWN_TEMPL - fill a template array for nearest neighbors
 *                        - value is 1 if function is known at that point
 *                        - and 0 otherwise
 *                        - also fill in an array of strides from the diagonal
 *                        - of the Laplacian operator
 */

static void _PM_fill_unknown_templ(int *unt, int *str,
                                   int nd, int id, int side)
   {int i, j, k, l, nn;
    int imn, imx, jmn, jmx, kmn, kmx;
    int din, djn;

    imn = 0;
    imx = 2;
    jmn = 0;
    jmx = (nd > 1) ? 2 : 0;
    kmn = 0;
    kmx = (nd > 2) ? 2 : 0;
    din = 3;
    djn = 3;
    nn  = PM_ipow(3, nd);
    switch (id)
       {case 0:
	     if (side > 0)
	        {imn = 2;
		 imx = 2;}
	     else
	        {imn = 0;
		 imx = 0;};
	     break;
        case 1:
	     if (side > 0)
	        {jmn = 2;
		 jmx = 2;}
	     else
	        {jmn = 0;
		 jmx = 0;};
	     break;
        case 2:
	     if (side > 0)
	        {kmn = 2;
		 kmx = 2;}
	     else
	        {kmn = 0;
		 kmx = 0;};
	     break;};

    for (i = 0; i < nn; i++)
        unt[i] = 1;

    for (k = kmn; k <= kmx; k++)
        for (j = jmn; j <= jmx; j++)
            for (i = imn; i <= imx; i++)
                {l = INDEX(i, j, k, din, djn);

                 unt[l] = 0;};

     switch (nd)
        {case 1 :
	      break;

         case 2 :
	      str[0] = -1;
	      str[1] =  0;
	      str[2] =  1;
	      break;

/* GOTCHA: this is wrong but a start in the right direction */
         case 3 :
	      str[0] = -1;
	      str[1] =  0;
	      str[2] =  1;
	      str[3] = -1;
	      str[4] =  0;
	      str[5] =  1;
	      str[6] = -1;
	      str[7] =  0;
	      str[8] =  1;
	      break;};

    return;}

/*--------------------------------------------------------------------------*/

/*                            LAPLACE SOLVER                                */

/*--------------------------------------------------------------------------*/

/* _PM_FILL_LAPL_OP - set up the laplacian operator and the boundary arrays
 *                  - for the 2D laplacian solver
 *                  - this operator uses only the tangential
 *                  - spacing ratios
 *                  - if KRA or LRA are NULL or CRF is TRUE, use constant
 *                  - ratios KRC and LRC
 */

static void _PM_fill_lapl_op(PM_sp_lin_sys *axb, int *unt, int *str,
                             int na, double **x, double *kra, double *lra,
                             PM_matrix *n_map, int no,
                             double ts, double krc, double lrc, int crf)
   {int n, j, i;
    double sr, sl, sb, st;
    double pnt, cnt;
    double a[9];

    n = n_map->nrow;

    pnt = min(ts, 1.0);
    pnt = max(pnt, 0.0);
    cnt = 2.0 - pnt;

    for (i = 1; i <= n; i++)
        {j = PM_element(n_map, i, no);

         if (crf || (kra == NULL) || (lra == NULL))
            {sr = 1.0/(1.0 + krc);
             sl = krc*sr;
             st = 1.0/(1.0 + lrc);
             sb = lrc*st;}
         else
            {sr = 1.0/(1.0 + kra[j]);
             sl = kra[j]*sr;
             st = 1.0/(1.0 + lra[j]);
             sb = lra[j]*st;};

	 a[0] = -pnt*sl*sb;
	 a[1] = sb;
	 a[2] = -pnt*sr*sb;
	 a[3] = sl;
	 a[4] = -cnt;
	 a[5] = sr;
	 a[6] = -pnt*sl*st;
	 a[7] = st;
	 a[8] = -pnt*sr*st;

	 _PM_load_hier_tridi(axb, i, na, n_map,
			     x, 9, a, unt, str);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_LAPLACE_SOL - compute the coordinates using the correct laplacian
 *                 - this operator uses only the tangential
 *                 - spacing ratios
 */

static void _PM_laplace_sol(int *unt, int *str,
                            int na, double **x, double *kra, double *lra,
			    PM_matrix *n_map, int no, int nr,
                            double ts, double krc, double lrc, int crf)
   {int n;
    PM_sp_lin_sys *axb;

    n   = n_map->nrow;
    axb = PM_mk_sp_lin_sys(n, na, 7, TRUE, FALSE, NULL, NULL);
    if (axb != NULL)

/* fill in the laplacian and b arrays */
       {_PM_fill_lapl_op(axb, unt, str, na, x, kra, lra,
			 n_map, no, ts, krc, lrc, crf);

/* solve the system */
	_PM_solv_lapl(x, axb, n_map, nr, na);

	PM_rl_sp_lin_sys(axb);};

    return;}

/*--------------------------------------------------------------------------*/

/*                           LAPLACE SOLVER A                               */

#if 0

/*--------------------------------------------------------------------------*/

/* _PM_COMPUTE_A_BND - compute the a quantities on the given side */

static void _PM_compute_a_bnd(double as, double xs, double ae, double xe,
			      double *v, int kmax, int lmax,
			      int kmn, int kmx, int lmn, int lmx)
   {int i, j, n, nt, sdk, sdl;
    double ps, pe, dk, dl;

    if (as < 0.0)
       {ps  = -1.0;
	as *= -1.0;}
    else
       ps = 1.0;

    if (ae < 0.0)
       {pe  = -1.0;
	ae *= -1.0;}
    else
       pe = 1.0;

    dk = kmx - kmn;
    dl = lmx - lmn;
    n  = sqrt(dk*dk + dl*dl);
    nt = n/2;

    sdk = (dk > 0);
    sdl = (dl > 0);

    if (as != 0.0)
       {for (j = 0; j < nt; j++)
            {i    = NODE_OF(kmn + sdk*(j + 1), lmn + sdl*(j + 1));
             v[i] = pow((1.0 + as*exp(-j*xs)), ps);};};

    if (ae != 0.0)
       {for (j = 0; j < nt; j++)
            {i    = NODE_OF(kmx - sdk*j, lmx - sdl*j);
             v[i] = pow((1.0 + ae*exp(-j*xe)), pe);};};

    if (2*nt != n)
       {i    = NODE_OF(kmn + sdk*(nt + 1), lmn + sdl*(nt + 1));
        v[i] = 1.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COMPUTE_A - compute the a quantities
 *               -   ASK      k-product magnitude start method 2 only (i)
 *               -   XSK      k-product exponent start method 2 only (i)
 *               -   AEK      k-product magnitude end method 2 only (i)
 *               -   XEK      k-product exponent end method 2 only (i)
 *               -   ASL      l-product magnitude start method 2 only (i)
 *               -   XSL      l-product exponent start method 2 only (i)
 *               -   AEL      l-product magnitude end method 2 only (i)
 *               -   XEL      l-product exponent end method 2 only (i)
 */

static void _PM_compute_a(double *apk, double *apl, double *kra, double *lra,
			  int kmax, int lmax,
			  int kmn, int kmx, int lmn, int lmx,
			  double ask, double xsk, double aek, double xek,
			  double asl, double xsl, double ael, double xel,
			  int constr)
   {int j, j0, k, l;
    int nz;
    double u, dk, dl, dkl;
    double *x1, *x2, *x3, *x4, *s, *t;

    nz = kmax*lmax;

/* extrapolate lra to the undetermined boundary */
    if (lmx - lmn < 3)
       {for (k = kmn; k <= kmx; k++)
            {j      = NODE_OF(k, lmx);
             lra[j] = lra[j-kmax];
             j      = NODE_OF(k, lmn);
             lra[j] = lra[j+kmax];};}
    else
       {for (k = kmn; k <= kmx; k++)
            {j      = NODE_OF(k, lmx);
             lra[j] = 2.0*lra[j-kmax] - lra[j-2*kmax];
             j      = NODE_OF(k, lmn);
             lra[j] = 2.0*lra[j+kmax] - lra[j+2*kmax];};};

/* extrapolate kra to the undetermined boundary */
    if (kmx - kmn < 3)
       {for (l = lmn; l <= lmx; l++)
            {j      = NODE_OF(kmx, l);
             kra[j] = kra[j-1];
             j      = NODE_OF(kmn, l);
             kra[j] = kra[j+1];};}
    else
       {for (l = lmn; l <= lmx; l++)
            {j      = NODE_OF(kmx, l);
             kra[j] = 2.0*kra[j-1] - kra[j-2];
             j      = NODE_OF(kmn, l);
             kra[j] = 2.0*kra[j+1] - kra[j+2];};};

    _PM_compute_a_bnd(ask, xsk, aek, xek, apk,
		      kmax, lmax, kmn, kmx, lmx, lmx);

    _PM_compute_a_bnd(asl, xsl, ael, xel, apl,
		      kmax, lmax, kmx, kmx, lmn, lmx);

    t = CMAKE_N(double, nz);
    s = CMAKE_N(double, nz);

/* compute apl */
    PM_array_set(s, nz, 0.0);
    PM_array_set(t, nz, 0.0);

    vecset4(lra, x1, x2, x3, x4);
    for (l = lmn+1; l <= lmx; l++)
        {j0 = NODE_OF(kmx, l);

         if (constr)

/* sweep to the right */
            {for (k = kmn+1; k <= kmx; k++)
                 {j   = NODE_OF(k, l);
                  u   = 4.0/(x1[j] + x2[j] + x3[j] + x4[j]);
                  dk  = 0.5*(x2[j] - x3[j] - x4[j] + x1[j]);
                  dl  = 0.5*(x2[j] + x3[j] - x4[j] - x1[j]);
                  dkl = 0.25*(x2[j] - x3[j] + x4[j] - x1[j]);

/* GOTCHA: dimensionally this is wrong!!! */
                  s[j] = s[j-1] + 2.0*dl + dk*u*u - dkl*u;};

/* sweep to the left */
             for (k = kmx-1; k >= kmn; k--)
                 {j   = NODE_OF(k, l);
                  u   = 4.0/(x1[j] + x2[j] + x3[j] + x4[j]);
                  dk  = 0.5*(x2[j] - x3[j] - x4[j] + x1[j]);
                  dl  = 0.5*(x2[j] + x3[j] - x4[j] - x1[j]);
                  dkl = 0.25*(x2[j] - x3[j] + x4[j] - x1[j]);

                  t[j] = t[j+1] - 2.0*dl - dk*u*u + dkl*u;};};

         for (k = kmn+1; k <= kmx; k++)
             {j      = NODE_OF(k, l);
              apl[j] = 0.5*(s[j] + t[j]) + apl[j0];};};

/* compute apk */
    PM_array_set(s, nz, 0.0);
    PM_array_set(t, nz, 0.0);

    vecset4(kra, x1, x2, x3, x4);
    for (k = kmn+1; k <= kmx; k++)
        {j0 = NODE_OF(k, lmx);

         if (constr)

/* sweep up */
            {for (l = lmn+1; l <= lmx; l++)
                 {j   = NODE_OF(k, l);
                  u   = 4.0/(x1[j] + x2[j] + x3[j] + x4[j]);
                  dk  = 0.5*(x2[j] - x3[j] - x4[j] + x1[j]);
                  dl  = 0.5*(x2[j] + x3[j] - x4[j] - x1[j]);
                  dkl = 0.25*(x2[j] - x3[j] + x4[j] - x1[j]);

                  s[j] = s[j-kmax] + 2.0*dk + dl*u*u - dkl*u;};

/* sweep down */
             for (l = lmx-1; l > lmn; l--)
                 {j   = NODE_OF(k, l);
                  u   = 4.0/(x1[j] + x2[j] + x3[j] + x4[j]);
                  dk  = 0.5*(x2[j] - x3[j] - x4[j] + x1[j]);
                  dl  = 0.5*(x2[j] + x3[j] - x4[j] - x1[j]);
                  dkl = 0.25*(x2[j] - x3[j] + x4[j] - x1[j]);

                  t[j] = t[j+kmax] - 2.0*dk - dl*u*u + dkl*u;};};

         for (l = lmn+1; l <= lmx; l++)
             {j   = NODE_OF(k, l);
              apk[j] = 0.5*(s[j] + t[j]) + apk[j0];};};

    CFREE(s);
    CFREE(t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FILL_LAPL_OPA - set up the laplacian operator and the boundary arrays
 *                   - for the 2D laplacian solver
 *                   - this operator uses both the normal and tangential
 *                   - spacing ratios
 *                   - if KRA, LRA, APK, or APL are NULL or CRF is TRUE,
 *                   - use constant ratios KRC and LRC
 */

static void _PM_fill_lapl_opa(PM_matrix *lapl, int kmax, int lmax, int na,
			      PM_matrix **ar,
			      double **dt, double *kra, double *lra,
			      double *apk, double *apl,
			      PM_matrix *n_map,
			      double ts, double krc, double lrc,
			      int crf, double theta)
   {int n, j, i;
    int indx[8], off[8];
    double alpha, beta;
    double s1, s2, s3, s4, sr, sl, sb, st;
    double akp, akm, alp, alm;
    double ckp, ck, ckm, clp, cl, clm;
    double bkp, bk, bkm, blp, bl, blm;
    double pnt, cnt;
    double a[8];

    theta *= 0.5*PI;
    alpha = cos(theta);
    beta  = sin(theta);
    pnt   = alpha + beta;
    alpha /= pnt;
    beta  /= pnt;

    n = n_map->nrow;

    PM_array_set(lapl->array, n*n, 0.0);

    pnt = min(ts, 1.0);
    pnt = max(pnt, 0.0);
    cnt = 2.0 - pnt;

    for (i = 1; i <= n; i++)
        {j = PM_element(n_map, i, 1);

         if ((kra == NULL) || (lra == NULL) ||
             (apk == NULL) || (apl == NULL) ||
             crf)
            {sr = 1.0/(1.0 + krc);
             sl = krc*sr;
             st = 1.0/(1.0 + lrc);
             sb = lrc*st;

             s1 = -pnt*sr*sb;
             s2 = -pnt*sr*st;
             s3 = -pnt*sl*st;
             s4 = -pnt*sl*sb;}

         else
            {akp = apk[j+1];
             akm = 1.0/apk[j];
             alp = apl[j+kmax];
             alm = 1.0/apl[j];

             ckp = 1.0/(1.0 + kra[j+kmax]);
             ck  = 1.0/(1.0 + kra[j]);
             ckm = 1.0/(1.0 + kra[j-kmax]);
             clp = 1.0/(1.0 + lra[j+1]);
             cl  = 1.0/(1.0 + lra[j]);
             clm = 1.0/(1.0 + lra[j-1]);

             bkp = ckp*kra[j+kmax];
             bk  = ck*kra[j];
             bkm = ckm*kra[j-kmax];
             blp = clp*lra[j+1];
             bl  = cl*lra[j];
             blm = clm*lra[j-1];

             sr = (alpha + beta*akp)*ck;
             sl = (alpha + beta*akm)*bk;
             st = (alpha*alp + beta)*cl;
             sb = (alpha*alm + beta)*bl;

             s1 = -pnt*(alpha*alm*bl*ckm + beta*akp*ck*blp);
             s2 = -pnt*(alpha*alp*cl*ckp + beta*akp*ck*clp);
             s3 = -pnt*(alpha*alp*cl*bkp + beta*akm*bk*clm);
             s4 = -pnt*(alpha*alm*bl*bkm + beta*akm*bk*blm);};

	 SET_LAPL(a[0], indx[0], off[0], sr, 2,  1);
	 SET_LAPL(a[1], indx[1], off[1], st, 3,  kmax);
	 SET_LAPL(a[2], indx[2], off[2], sl, 4, -1);
	 SET_LAPL(a[3], indx[3], off[3], sb, 5, -kmax);
	 SET_LAPL(a[4], indx[4], off[4], s2, 6,  kmax+1);
	 SET_LAPL(a[5], indx[5], off[5], s3, 7,  kmax-1);
	 SET_LAPL(a[6], indx[6], off[6], s4, 8, -kmax-1);
	 SET_LAPL(a[7], indx[7], off[7], s1, 9, -kmax+1);

	 _PM_load_hier_tridi(lapl, ar, i, na, kmax, cnt,
			     dt, 8, a, indx, off);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_LAPLACE_SOLA - compute the coordinates using the correct laplacian
 *                  - this operator uses both the normal and tangential
 *                  - spacing ratios
 */

static void _PM_laplace_sola(PM_matrix *lapl, int kmax, int lmax, int na,
			     PM_matrix **b, double **x,
			     double *kra, double *lra, double *apk, double *apl,
			     PM_matrix *n_map,
			     double ts, double krc, double lrc,
			     int crf, double theta)
   {

/* fill in the laplacian, bx, by and n_map arrays */
    _PM_fill_lapl_opa(lapl, kmax, lmax,
		      na, b, x, kra, lra, apk, apl, n_map,
		      ts, krc, lrc, crf, theta);

/* solve the system */
    _PM_solv_lapl(x, lapl, b, n_map, no, na);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* PM_MESH_EXWK - find the position of each node just outside the known
 *              - mesh for this block
 *              - Arguments:
 *              -   Y        coordinate arrays (i/o)
 *              -   N        number of nodes in this part of the mesh
 *              -   SMN      subpatch minimum index values
 *              -   SMX      subpatch maximum index values
 *              -   MN       patch minimum index values
 *              -   MX       patch maximum index values
 *              -   KRA      k-ratio array (i)
 *              -   LRA      l-ratio array (i)
 *              -   APK      k-product array method 2 only (i)
 *              -   APL      l-product array method 2 only (i)
 *              -   METHOD   generation method: 1) ratios only;
 *              -                               2) products and ratios
 *              -   CONSTR   impose mesh generation constraint
 *              -   DSPAT    spatial differencing: 0.0 -> pure 5 point
 *              -                                  1.0 -> pure 9 point
 *              -   DRAT     ratio differencing: 0.0 -> pure 5 point
 *              -                                1.0 -> pure 9 point
 *              -   ORIENT   orientation: 0.0 -> pure K orientation
 *              -                         1.0 -> pure L orientation
 *              -   UNT      template of unknowns: 0 unknown, 1 known
 */

void PM_mesh_exwk(double *y, int id, int side, int nd,
                  int *smn, int *smx, int *mn, int *mx,
                  double *kra, double *lra, double *apk, double *apl,
                  int *unt, int *str, int method, int constr,
                  double dspat, double drat, double orient)
   {int j, jd, n, na, no, nn, nr, sr;
    double **x;
    PM_matrix **b, *n_map, *lapl;

    n_map = _PM_fill_map(id, side, nd, smn, smx, mn, mx);

    no = 1 + (PM_ipow(3, nd) >> 1);

    sr = PM_ipow(3, id);
    if (side > 0)
       nr = no + sr;
    else
       nr = no - sr;

    n    = n_map->nrow;
    lapl = PM_create(n, n);

    na = 2;
    b  = CMAKE_N(PM_matrix *, na);
    x  = CMAKE_N(double *, na);
    for (j = 0; j < na; j++)
        b[j] = PM_create(n, 1);

    nn = 1;
    for (jd = 0; jd < nd; jd++)
        nn *= (mx[jd] - mn[jd] + 1);

    x[0] = y;
    x[1] = y + nn;

    switch (method)

/* this method uses only the tangential spacing ratios */
       {case 1 :

	     _PM_laplace_sol(unt, str, na, x, kra, lra,
			     n_map, no, nr, dspat, 1.0, 1.0, FALSE);

	     break;

/* this method uses both the normal and tangential spacing ratios */
        case 2 :

/*
	     _PM_laplace_sola(lapl, kmax, lmax,
			      na, b, x, kra, lra,
			      apk, apl,
			      n_map, dspat,
			      1.0, 1.0, FALSE, orient);
*/
	     break;

        default :
	     break;};

/* release the intermediate storage */
    PM_destroy(lapl);
    PM_destroy(n_map);
    for (j = 0; j < na; j++)
        PM_destroy(b[j]);

    CFREE(b);
    CFREE(x);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MESH_EXTR_1D - find the position of each node just outside the known
 *                  - mesh for this block
 *                  - Arguments:
 *                  -   X        coordinate arrays (i/o)
 *                  -   SMN      subpatch minimum index values
 *                  -   SMX      subpatch maximum index values
 *                  -   MN       patch minimum index values
 *                  -   MX       patch maximum index values
 */

static void _PM_mesh_extr_1d(double *x, int *smn, int *smx, int *mn, int *mx,
                             int ng, int side)
   {int ig, im, in;
    int ic, i0, i1, i2;
    double r1, r2, r3;

/* extrapolate one layer at a time */
    for (ig = 0; ig < ng; ig++)
        {im = (side < 0) ? 1 + smx[0]-- : smn[0]++;

	 in = im - mn[0];

	 i2 = in - side*2;
	 i1 = in - side;
	 i0 = in;
	 ic = in + side;

	 r1 = x[i2];
	 r2 = x[i1];
	 r3 = x[i0];
	 x[ic] = 3.0*r3 - 3.0*r2 + r1;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MESH_EXTR_ND - find the position of each node just outside the known
 *                  - mesh for this block
 *                  - Arguments:
 *                  -   X        coordinate arrays (i/o)
 *                  -   SMN      subpatch minimum index values
 *                  -   SMX      subpatch maximum index values
 *                  -   MN       patch minimum index values
 *                  -   MX       patch maximum index values
 *                  -   METHOD   generation method: 1) ratios only;
 *                  -                               2) products and ratios
 *                  -   CONSTR   impose mesh generation constraint
 *                  -   DSPAT    spatial differencing: 0.0 -> pure 5 point
 *                  -                                  1.0 -> pure 9 point
 *                  -   DRAT     ratio differencing: 0.0 -> pure 5 point
 *                  -                                1.0 -> pure 9 point
 *                  -   ORIENT   orientation: 0.0 -> pure K orientation
 *                  -                         1.0 -> pure L orientation
 */

static void _PM_mesh_extr_nd(double *x, int *smn, int *smx, int *mn, int *mx,
                             int ng, int nd, int id, int side,
                             int method, int constr,
                             double dspat, double drat, double orient)
   {int jd, ig, nn, nt;
    int *unt, *str;
    double *kra, *lra, *apk, *apl;

    nn = 1;
    for (jd = 0; jd < nd; jd++)
        nn *= (mx[jd] - mn[jd] + 1);

    kra = CMAKE_N(double, nn);
    lra = CMAKE_N(double, nn);
    apk = CMAKE_N(double, nn);
    apl = CMAKE_N(double, nn);

    PM_array_set(kra, nn, 1.0);
    PM_array_set(lra, nn, 1.0);
    PM_array_set(apk, nn, 1.0);
    PM_array_set(apl, nn, 1.0);

    nt  = PM_ipow(3, nd);
    unt = CMAKE_N(int, nt);
    str = CMAKE_N(int, nt);

    if (debug)
       {SC_mark(x, 1);
	show_mesh(x, nd, mn, mx, id, side, 0);};

    _PM_fill_unknown_templ(unt, str, nd, id, side);

/* extrapolate one layer at a time */
    for (ig = 0; ig < ng; ig++)
        {PM_mesh_exwk(x, id, side, nd, smn, smx, mn, mx,
		      kra, lra, apk, apl, unt, str,
		      method, constr, dspat, drat, orient);

	 if (debug)
	    show_mesh(x, nd, mn, mx, id, side, ig);

	 if (side > 0)
	    smn[id]++;
	 else
	    smx[id]--;};

    CFREE(kra);
    CFREE(lra);
    CFREE(apk);
    CFREE(apl);

    CFREE(unt);
    CFREE(str);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MESH_EXTR - find the position of each node just outside the known
 *              - mesh for this block
 *              - Arguments:
 *              -   X        coordinate arrays (i/o)
 *              -   SMN      subpatch minimum index values
 *              -   SMX      subpatch maximum index values
 *              -   MN       patch minimum index values
 *              -   MX       patch maximum index values
 *              -   METHOD   generation method: 1) ratios only;
 *              -                               2) products and ratios
 *              -   CONSTR   impose mesh generation constraint
 *              -   DSPAT    spatial differencing: 0.0 -> pure 5 point
 *              -                                  1.0 -> pure 9 point
 *              -   DRAT     ratio differencing: 0.0 -> pure 5 point
 *              -                                1.0 -> pure 9 point
 *              -   ORIENT   orientation: 0.0 -> pure K orientation
 *              -                         1.0 -> pure L orientation
 */

void PM_mesh_extr(double *x, int *smn, int *smx, int *mn, int *mx,
		  int ng, int nd, int id, int side,
		  int method, int constr,
		  double dspat, double drat, double orient)
   {

    if (nd == 1)
       _PM_mesh_extr_1d(x, smn, smx, mn, mx, ng, side);

    else
       _PM_mesh_extr_nd(x, smn, smx, mn, mx, ng, nd, id, side,
			method, constr, dspat, drat, orient);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
