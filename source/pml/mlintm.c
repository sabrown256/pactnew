/*
 * MLINTM.C - mesh interpolation routines (N-dimensional and otherwise)
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#define MSCALE     prm[0]
#define MSTRENGTH  prm[1]
#define MPOWER     prm[2]
#define MQSCALE    prm[3]

/* compute MQ basis function from the distance between
 * ND point XI[K] and XO[IP[1,...,ND]]
 *
 *   D = sqrt( D0 + SUM(ID, XI[ID][K] - XO[ID][IP[ID]]) )
 */

#define BASIS_MQ(_d, _nd, _xi, _k, _xo, _ip, _d0)                             \
   {int _l, _id;                                                              \
    double _dx;                                                               \
    double *_xic, *_xoc;                                                      \
    _d = _d0;                                                                 \
    for (_id = 0; _id < _nd; _id++)                                           \
        {_l   = _ip[_id];                                                     \
	 _xic = _xi[_id];                                                     \
	 _xoc = _xo[_id];                                                     \
	 _dx  = (_xic[_k] - _xoc[_l]);                                        \
	 _d  += _dx*_dx;};                                                    \
    _d = sqrt(_d);}

typedef struct s_weight weight;
typedef union u_dmask dmask;

struct s_weight
   {double w;
    int id;           /* index of destination point */
    int ids;};        /* index of source point on destination mesh */

union u_dmask
   {double d;
    int64_t l;};

/*--------------------------------------------------------------------------*/

/*                        DISTANCE WEIGHTED ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* DISTANCE - return the pseudo-cartesian distance between ND dimensional
 *          - points A[IA] and B[IB]
 *          - if PW == 2 then it is cartesian
 */

static INLINE double DISTANCE(double **a, int ia, double **b,
			      int ib, int nd, double pw)
   {int j;
    double xa, xb, dx, d;

    d = 0.0;

#pragma omp parallel for private(xa, xb, dx)
    for (j = 0; j < nd; j++)
        {xa = a[j][ia];
	 xb = b[j][ib];
	 dx = (xb - xa);
	 dx = ABS(dx);
	 d += POW(dx, pw);};

    d = POW(d, 1.0/pw);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FIND_OVERLAP_MARGIN - from the mesh scales compute an overlap
 *                         - factor that determines the size of the
 *                         - interpolation template
 */

static int *_PM_find_overlap_margin(int nc, PM_set *dd, PM_set *sd,
				    double *prm)
   {int id, dnde, snde, snd;
    int lc, tc;
    int *ddix, *sdix, *ta;
    char *sty, *dty;
    double sc, dc, rc, ic;
    double ssc[20], dsc[20];

    ta = CMAKE_N(int, nc);

    snd  = sd->dimension;
    snde = sd->dimension_elem;
    sdix = sd->max_index;
    sty  = sd->es_type;
    PM_array_real(sty, sd->scales, snde, ssc);

    dnde = dd->dimension_elem;
    ddix = dd->max_index;
    dty  = dd->es_type;
    PM_array_real(dty, dd->scales, dnde, dsc);

#pragma omp parallel for private(rc, dc, sc, ic, lc, tc)
    for (id = 0; id < dnde; id++)
        {rc = ssc[id]/dsc[id] - 1.0;
	 rc = max(rc, 0.0);
	 dc = HUGE;

         if ((sdix != NULL) && (ddix != NULL))

/* NOTE: this factor of 2 is to compensate for the factor
 *       of 2 divided out to form the half interval later
 *       it doesn't effect the ratio IC but does effect TC
 */
	    {sc = (snd == 1) ? 2*sdix[0] : 2*sdix[id];
	     dc = 2*ddix[id];
	     ic = dc/sc;
	     ic = max(ic, 0.0);

	     rc = max(rc, ic);};

	 rc = 1.0 + MSCALE*rc;

	 lc = rc + 0.5;
	 tc = (dnde == 1) ? lc : 2*lc;
	 tc = min(tc, dc);

	 ta[id] = tc;};

    return(ta);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FIND_OVERLAP_POINT - from the mesh scales compute an overlap
 *                        - factor that determines the size of the
 *                        - interpolation template
 */

static void _PM_find_overlap_point(int *tb, int *ta, PM_set *dd, double *ds,
				   double *prm)
   {int id, nd;
    int dc, gc, lc, tc;
    int *ddix;
    char *dty;
    double rc;
    double dsc[20];

    nd   = dd->dimension_elem;
    ddix = dd->max_index;
    dty  = dd->es_type;
    PM_array_real(dty, dd->scales, nd, dsc);

#pragma omp parallel for private(dc, rc, lc, gc, tc)
    for (id = 0; id < nd; id++)
        {dc = ddix[id];

	 rc = ds[id]/dsc[id];
	 rc = ABS(rc) - 1.0;
	 rc = max(rc, 0.0);
	 rc = 1.0 + MSCALE*rc;

	 lc = rc + 0.5;
	 tc = 2*lc;
	 tc = min(tc, dc);

	 gc = ta[id];
	 tc = max(tc, gc);

	 tb[id] = tc;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COORD_INTERP_PARAM - compute the linear interpolation coefficients
 *                        - AD and BD for of ND directions
 */

void _PM_coord_interp_param(double *ad, double *bd, int nd,
			    double *extr, int *idx)
   {int j, ld;
    double xn, xx, inx;

#pragma omp parallel for private(xn, xx, ld, inx)
    for (j = 0; j < nd; j++)
        {xn  = extr[2*j];
	 xx  = extr[2*j + 1];
	 ld  = idx[j] - 1;
         inx = 1.0/(xx - xn);

         ad[j] = ld*inx;
	 bd[j] = -ld*xn*inx;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_INV_DIST_WGT - compute indeces of the destination points nearest the
 *                  - source points (IDS) and the inverse distance
 *                  - weights WN in the neighborhood of each point
 */

static void _PM_inv_dist_wgt(SC_array *pwd, int *nof,
			     PM_set *dd, PM_set *sd,
			     double *ad, double *bd, int *ddix,
			     int *ta, double *prm)
   {int j, id, is, idc, im, in, nn;
    int isp, ism, ist;
    int snde, sne, skip, ld, dl;
    int *it, *dt, *tb, *to, *on;
    double ls, xs, d, w, oc;
    double dm, dp, dc;
    double ps, pw;
    double **dde, **sde, *ds;
    weight wt;
    double epsilon;

    snde = sd->dimension_elem;
    sne  = sd->n_elements;

    dde = PM_make_real_set_elements(dd);
    sde = PM_make_real_set_elements(sd);

    it = CMAKE_N(int, snde);
    dt = CMAKE_N(int, snde);
    tb = CMAKE_N(int, snde);
    to = CMAKE_N(int, snde);
    on = CMAKE_N(int, snde);
    ds = CMAKE_N(double, snde);

/* PS defines the strength of the pole obtained at zero distance
 * the relationships here are imperical and give better results
 * than the obvious values
 */
    if (MSTRENGTH == 0.0)
       ps = snde;
    else
       ps = MSTRENGTH;

/* PW defines the averaging of the components of a vector in the
 * distance measure - a value of 2 would give the standard harmonic
 * average sqrt(sum(squares))
 */
    if (MPOWER == 0.0)
       pw = PM_pow(ps, -ps);
    else
       pw = MPOWER;

    im = 0;

    for (is = 0; is < sne; is++)

/* compute the point ID nearest IS but closer to the index space origin */
	{idc = 0;
	 for (j = snde - 1; j >= 0; j--)
	     {xs = sde[j][is];
	      ls = ad[j]*xs + bd[j];
	      oc = ls - floor(ls + 0.5);

/* GOTCHA: this tolerance is just plain arbitrary */
	      on[j] = (ABS(oc) < 2.0e-10);

	      it[j] = ls + 0.5;
	      idc   = idc*ddix[j] + ls;};

	 isp = (is < sne-1) ? is + 1 : is;
	 ism = (is > 0) ? is - 1 : is;

	 dp = DISTANCE(sde, isp, sde, is, snde, 2.0);
	 dm = DISTANCE(sde, ism, sde, is, snde, 2.0);
	 
/* Epsilon attempts to force a predictable path through the following
 * "if" statement, in the face of mappings that vary only due to machine
 * precision.  The subsequent calculation of point neighborhoods is sensitive
 * to the particular branch taken, and this affects computation of norms
 * in ways not well-understood at present (3June2009 - LEB.)
 */
	 epsilon = 100.0 * PM_machine_precision();
	 if (dm <= epsilon)
	    {ist = isp;
	     dc  = dp;}
	 else if (dp <= epsilon)
	    {ist = ism;
	     dc  = dm;}
	 else if (dm < (dp - epsilon))
	    {ist = isp;
	     dc  = dp;}
	 else
	    {ist = ism;
	     dc  = dm;};
#if 1
	 SC_ASSERT(dc != 0.0);

	 for (j = 0; j < snde; j++)
	     ds[j] = (sde[j][ist] - sde[j][is]);
#else
	 for (j = 0; j < snde; j++)
	     ds[j] = dc;
#endif
	 _PM_find_overlap_point(tb, ta, dd, ds, prm);

	 nn = 1;
	 for (j = 0; j < snde; j++)
	     {to[j] = (tb[j] - 2) >> 1;
	      dt[j] = 0;
	      nn   *= tb[j];};

/* loop over neighborhood of idc */
	 nof[is] = im;
         for (in = 0; in < nn; in++)
	     {skip = FALSE;

/* find the index ID of DT in the template relative to IDC */
	      id = 0;
	      for (j = snde - 1; j >= 0; j--)
		  {dl    = dt[j] - to[j];
		   ld    = it[j] + dl;
		   id    = id*ddix[j] + ld;
		   skip |= ((ld < 0) || (ddix[j] <= ld));
		   skip |= (on[j] && (abs(dl) > to[j]));};

/* increment the template point DT */
              dt[0]++;
	      for (j = 0; j < snde; j++)
		  if (dt[j] >= tb[j])
                     {dt[j] = 0;
		      if (j+1 < snde)
			 dt[j+1]++;};

/* if this point ID is outside the mesh skip it */
	      if (skip)
		 continue;

/* compute the distances from each src point IS to the current
 * destination point ID
 */
	      d = DISTANCE(sde, is, dde, id, snde, pw);
	      if (d < SMALL) 
		 w = HUGE;

	      else
		 w = PM_pow(d, -ps);

	      wt.id  = id;
	      wt.ids = idc;
	      wt.w   = w;
	      SC_array_push(pwd, &wt);

	      im++;};};

    nof[is] = im;

    PM_rel_real_set_elements(dde);
    PM_rel_real_set_elements(sde);

    CFREE(it);
    CFREE(dt);
    CFREE(tb);
    CFREE(to);
    CFREE(on);
    CFREE(ds);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERPOLATE_MAPPING_ID - interpolate the range of the mapping SOURCE
 *                           - onto an intermediate vector of arrays TRE
 *                           - centered on the mapping DEST
 *                           - return TRE
 *                           - uses inverse distance weighted interpolation
 *                           - NOTE: works very well until the destination
 *                           - mesh is about 4 times finer than the source
 *                           - when the relative granularity of the source
 *                           - leaks into the interpolated values
 */

double **PM_interpolate_mapping_id(PM_mapping *dest, PM_mapping *source,
				   int wgtfl, double *prm)
   {int i, j, id, is, in, nn, ina, inb;
    int sne, dne, snde, snre, dnde;
    int *ddix, *nof, *ta;
    char *sty, *dty;
    double w, iwc, wgc, src, tc;
    double **sre, **tre;
    double **s, *sra, *trc, *wgt, *ddextr;
    double *ad, *bd;
    PM_set *sr, *sd, *dr, *dd;
    SC_array *wda;
    weight *wa;

    sd   = source->domain;
    snde = sd->dimension_elem;
    sne  = sd->n_elements;
    sty  = sd->es_type;

    sr   = source->range;
    sre  = (double **) sr->elements;
    sty  = sr->es_type;
    snre = sr->n_elements;

    dd     = dest->domain;
    dne    = dd->n_elements;
    dty    = dd->es_type;
    ddextr = dd->extrema;
    ddextr = PM_array_real(dty, dd->extrema, 2*snde, NULL);

    dr   = dest->range;
    dnde = dr->dimension_elem;

    ddix = dd->max_index;

    tre = PM_make_vectors(dnde, dne);

    ta = _PM_find_overlap_margin(dnde, dd, sd, prm);

    for (j = 0; j < dnde; j++)
        PM_array_set(tre[j], dne, 0.0);

    nn = 1;
    for (j = 0; j < snde; j++)
        nn *= ta[j];

    wda = CMAKE_ARRAY(weight, NULL, 0);
    nof = CMAKE_N(int, sne+1);

    ad = CMAKE_N(double, snde);
    bd = CMAKE_N(double, snde);

/* compute the coordinate interpolation parameters */
    _PM_coord_interp_param(ad, bd, snde, ddextr, ddix);

/* compute the inverse distance weights */
    _PM_inv_dist_wgt(wda, nof, dd, sd, ad, bd, ddix, ta, prm);

    wgt = CMAKE_N(double, dne);

    PM_array_set(wgt, dne, 0.0);

/* NOTE: by construction now the range is indexed the same as the domain */
    s = PM_convert_vectors(dnde, sne, sre, sty);

/* compute the weighted sums */
    for (i = 0; i < dnde; i++)
        {sra = s[i];
	 trc = tre[i];

	 for (is = 0; is < sne; is++)
	     {if (is >= snre)
                 return(NULL);

/* loop over neighborhood of id */
	      src = sra[is];
	      ina = nof[is];
	      inb = nof[is+1];
	      for (in = ina; in < inb; in++)
		  {wa = SC_array_get(wda, in);
		   w  = wa->w;
		   if (w != 0.0)
		      {id = wa->id;

		       trc[id] += src*w;
		       wgt[id] += w;};};};};

    PM_free_vectors(dnde, s);

/* invert the weights taking care with zero values */

#pragma omp parallel for private(wgc)
    for (i = 0; i < dne; i++)
        {wgc = wgt[i];
	 wgt[i] = (wgc == 0.0) ? 0.0 : 1.0/wgc;};

    for (id = 0; id < dnde; id++)
        {trc = tre[id];
	
/* just return the inverse weights */
	 if (wgtfl)
	    {

#pragma omp parallel for private(iwc)
	     for (i = 0; i < dne; i++)
	         {iwc    = wgt[i];
		  trc[i] = iwc;};

	     }

/* normalize the destination values by the inverse weights */
	 else
	    {

#pragma omp parallel for private(iwc, tc)
	     for (i = 0; i < dne; i++)
	         {iwc    = wgt[i];
		  tc     = trc[i]*iwc;
		  tc     = (ABS(tc) <= SMALL) ? 0.0 : tc;
		  trc[i] = tc;};

	    };};

    SC_free_array(wda, NULL);

    CFREE(ddextr);
    CFREE(ad);
    CFREE(bd);
    CFREE(wgt);
    CFREE(nof);
    CFREE(ta);

    return(tre);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERP_MESH_ID - interpolate NF meshless ND arrays FI at NI points XI
 *                   - to FO on a cartesian product mesh XO with
 *                   - component length MXO
 *                   - use inverse distance weighting method
 *                   -   PRM[0] = scale
 *                   -   PRM[1] = strength
 *                   -   PRM[2] = power
 *                   - return TRUE iff successful
 */

int PM_interp_mesh_id(int nd, int nf, int ni, double **xi, double **fi,
		      int *mxo, double **xo, double **fo,
		      double *prm)
   {int i, id, ne, nxo, rv;
    double xmn, xmx, dx, xc;
    double *xic, *xoc, *extr, *rat, *t;
    double **tre;
    C_array sa, da;
    PM_set *sr, *sd, *dr, *dd;
    PM_mapping *src, *dst;

    rv = TRUE;

    extr = CMAKE_N(double, 2*nd);
    rat  = CMAKE_N(double, nd);

    for (id = 0; id < nd; id++)
        xo[id] = CMAKE_N(double, mxo[id]);

/* compute the output cartesian product mesh */

#pragma omp parallel for private(nxo, xic, xmn, xmx, dx, xoc)
    for (id = 0; id < nd; id++)
        {nxo = mxo[id];
	 xic = xi[id];

	 xmn = HUGE;
	 xmx = -HUGE;
	 for (i = 0; i < ni; i++)
	     {xc  = xic[i];
	      xmn = min(xmn, xc);
	      xmx = max(xmx, xc);};

	 dx = (xmx - xmn)/(nxo - 1.0);

	 extr[2*id]   = xmn;
	 extr[2*id+1] = xmx;
	 rat[id]      = 1.0;

	 xoc = xo[id];
	 for (i = 0; i < nxo; i++)
	     xoc[i] = xmn + i*dx;};

    sd = PM_make_set_alt("sd", SC_DOUBLE_S, FALSE, 1, &ni, nd, (void **) xi);
    dd = PM_make_lr_index_domain("dd", SC_DOUBLE_S, nd, nd, mxo, extr, rat);

    ne = dd->n_elements;
    for (id = 0; id < nf; id++)
        {t = CMAKE_N(double, ne);
	 PM_array_set(t, ne, 0.0);
         fo[id] = t;};

    sr = PM_make_set_alt("sr", SC_DOUBLE_S, FALSE, 1, &ni, nf, (void **) fi);
    dr = PM_make_set_alt("dr", SC_DOUBLE_S, FALSE, nd, mxo, nf, (void **) fo);

    src = PM_make_mapping("src", PM_LR_S, sd, sr, N_CENT, NULL);
    dst = PM_make_mapping("dst", PM_LR_S, dd, dr, N_CENT, NULL);

    tre = PM_interpolate_mapping_id(dst, src, 0, prm);

    sa.length = ne;
    sa.type   = SC_DOUBLE_P_S;

    da.length = ne;
    da.type   = SC_DOUBLE_P_S;

    for (id = 0; id < nf; id++)
        {da.data = fo[id];
	 if (tre != NULL)
	    sa.data = tre[id];
	 else
	    sa.data = NULL;

	 PM_conv_array(&da, &sa, FALSE);};

    CFREE(extr);
    CFREE(rat);

    PM_free_vectors(nf, tre);

    PM_rel_mapping(src, TRUE, TRUE);
    PM_rel_mapping(dst, TRUE, FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                          MULTI-QUADRIC ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* MASK - mask off the low bits of D */

static INLINE void MASK(double *d)
   {
/*
    dmask m;

    m.d = *d;

    m.l &= 0xffffffffffffffff;

    *d = m.d;
 */
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MQ_COEF - calculate coefficients of Hardy's multiquadric fit
 *             - the input data is represented by the N Cartesian points
 *             - X at which the data values are F
 *             - RS is an input parameter, based on X and
 *             - COEF is the coefficient array
 *             - return TRUE iff successful
 */

static double *_PM_mq_coef(int nd, int n, double **x, double *f, double rs)
   {int i, j, id;
    double dc, dxc;
    double *xc, *coef;
    PM_matrix *a, *b;

    a = PM_create(n, n);
    b = PM_create(n, 1);

/* set up the real, symmetric matrix that is used in solving for coef */

#pragma omp parallel for private(i, dc, xc, dxc)
    for (j = 2; j <= n; j++)
        {for (i = 1; i <= j; i++)
	     {dc = 0.0;
	      for (id = 0; id < nd; id++)
		  {xc  = x[id];
		   dxc = xc[i-1] - xc[j-1];
		   dc += dxc*dxc;};

	      MASK(&dc);
	      dc = sqrt(dc + rs);

	      PM_element(a, i, j) = dc;
	      PM_element(a, j, i) = dc;};};

#pragma omp parallel for
    for (i = 1; i <= n; i++)
        {PM_element(a, i, i) = sqrt(rs);
	 PM_element(b, i, 1) = f[i-1];};

    PM_solve(a, b);

    coef = CMAKE_N(double, n);

#pragma omp parallel for
    for (i = 1; i <= n; i++)
        coef[i-1] = PM_element(b, i, 1);

    PM_destroy(a);
    PM_destroy(b);

    return(coef);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MQ_EVAL - evaluate an ND multiquadric interpolating function defined
 *             - by N data points XI, RS, and coefficients COEF[I]
 *             - at prod(MX) points XO, and return the values in F
 */

static void _PM_mq_eval(int nd, int n, double **xi, double rs, double *coef,
			int *mx, double **xo, double *f)
   {int l, lc, id, ic, io, is, no;
    int *ip, *str;
    double dc, fc;

    ip  = CMAKE_N(int, nd);
    str = CMAKE_N(int, nd+1);

    no = 1;
    for (id = 0; id < nd; id++)
        {ip[id]  = 0;
	 str[id] = no;
	 no     *= mx[id];};
    str[nd] = no;

    for (io = 0; io < no; io++)

/* get the index coordinates for io because xo is a Cartesian product mesh */
        {lc = io;
	 for (id = nd-1; id >= 0; id--)
	     {is = str[id];
	      ic = lc / is;
	      lc = lc - ic*is;
	      ip[id] = ic;};

/* evaluate the function at io */
	 fc = 0.0;

#pragma omp parallel for
	 for (l = 0; l < n; l++)
	     {BASIS_MQ(dc, nd, xi, l, xo, ip, rs);
	      fc += coef[l]*dc;};

	 f[io] = fc;};

    CFREE(ip);
    CFREE(str);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERP_MESH_MQ - interpolate NF meshless ND arrays FI at NI points XI
 *                   - to FO on a cartesian product mesh XO with
 *                   - component length MXO
 *                   - use multiquadric fit
 *                   - return TRUE iff successful
 */

int PM_interp_mesh_mq(int nd, int nf, int ni, double **xi, double **fi,
		      int *mxo, double **xo, double **fo,
		      double *prm)
   {int i, id, no, nxo, rv;
    double xc, dx, xmn, xmx, rs, rsc;
    double *xic, *xoc, *coef;

    rs = MQSCALE;

    no = 1;
    for (id = 0; id < nd; id++)
        no *= mxo[id];

    for (id = 0; id < nd; id++)
        xo[id] = CMAKE_N(double, mxo[id]);

/* compute the output cartesian product mesh
 * and the mesh based estimate of RS
 */
    rsc = 4.0/ni;

#pragma omp parallel for private(nxo, xic, xmn, xmx, xc, dx, xoc)
    for (id = 0; id < nd; id++)
        {nxo = mxo[id];
	 xic = xi[id];

	 xmn = HUGE;
	 xmx = -HUGE;
	 for (i = 0; i < ni; i++)
	     {xc  = xic[i];
	      xmn = min(xmn, xc);
	      xmx = max(xmx, xc);};

	 dx   = xmx - xmn;
	 rsc *= dx;
	 dx  /= (nxo - 1.0);

	 xoc = xo[id];
	 for (i = 0; i < nxo; i++)
	     xoc[i] = xmn + i*dx;};

/* use the value supplied if positive */
    if (rs > 0.0)
       rsc = rs;

    rv = FALSE;

    for (i = 0; i < nf; i++)
        {fo[i] = CMAKE_N(double, no);

	 coef = _PM_mq_coef(nd, ni, xi, fi[i], rsc);
	 if (coef != NULL)
	    {_PM_mq_eval(nd, ni, xi, rsc, coef, mxo, xo, fo[i]);
	     CFREE(coef);
	     rv = TRUE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERPOLATE_MAPPING_MQ - interpolate the range of the mapping SOURCE
 *                           - onto an intermediate vector of arrays TRE
 *                           - centered on the mapping DEST
 *                           - return TRE
 *                           - uses multi-quadric interpolation
 */

double **PM_interpolate_mapping_mq(PM_mapping *dest, PM_mapping *source,
				   int wgtfl, double *prm)
   {int j;
    int sne, dne, snde, dnde;
    int *ddix;
    double **sde, **sre, **tde, **tre;
    PM_set *sr, *sd, *dr, *dd;

    sd   = source->domain;
    sde  = (double **) sd->elements;
    snde = sd->dimension_elem;
    sne  = sd->n_elements;

    sr   = source->range;
    sre  = (double **) sr->elements;

    dd   = dest->domain;
    dne  = dd->n_elements;

    dr   = dest->range;
    dnde = dr->dimension_elem;

    ddix = dd->max_index;

    tre = PM_make_vectors(dnde, dne);

    for (j = 0; j < dnde; j++)
        PM_array_set(tre[j], dne, 0.0);

    tde = CMAKE_N(double *, snde);

    PM_interp_mesh_mq(snde, dnde, sne, sde, sre, ddix, tde, tre, prm);

    for (j = 0; j < snde; j++)
        CFREE(tde[j]);
    CFREE(tde);

    return(tre);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERPOLATE_MAPPING - interpolate the range of the mapping SOURCE
 *                        - onto an intermediate vector of arrays TRE
 *                        - centered on the mapping DEST
 *                        - return TRE
 *                        - the value of MTH determines method:
 *                        -   -N use IWD if more than N points
 *                        -      otherwise use MQ
 *                        -   -1 use MQ interpolation
 *                        -    0 use inverse distance weighting
 *                        -    1 return IWD weights
 */

double **PM_interpolate_mapping(PM_mapping *dest, PM_mapping *source,
				int mth, double *prm)
   {int np;
    double **tre;

/* -N option gives a way to automatically get away from O(N^2)
 * MQ method when the number of points gets big - user defined big at that
 */
    np = source->domain->n_elements;
    if (mth < -1)
       mth = (-mth < np) ? 0 : -1;

    if (mth < 0)
       tre = PM_interpolate_mapping_mq(dest, source, mth, prm);
    else
       tre = PM_interpolate_mapping_id(dest, source, mth, prm);

    return(tre);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

