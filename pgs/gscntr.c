/*
 * GSCNTR.C - contour plotting routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

typedef void (*PFCntrZC)(PG_device *dev, double *a,
                         int ndd, double **x, double *lev,
                         int nlev, int id, void *cnnct, pcons *alist);

typedef void (*PFCntrNC)(PG_device *dev, double *a,
                         int ndd, double **x, double *lev,
                         int nlev, int id, void *cnnct, pcons *alist);
 
#define ADD_INTERIOR_PT(_x, _y, _a)                                           \
    {if ((inp == 0) ||                                                        \
         (ilx[inp-1] != _x) || (ily[inp-1] != _y) || (ila[inp-1] != _a))      \
        {ilx[inp] = _x;                                                       \
         ily[inp] = _y;                                                       \
         ila[inp] = _a;                                                       \
         inp++;};}

#define ADD_EXTERIOR_PT(_x, _y, _a)                                           \
    {if ((onp == 0) ||                                                        \
	 (olx[onp-1] != _x) || (oly[onp-1] != _y) || (ola[onp-1] != _a))      \
        {olx[onp] = _x;                                                       \
         oly[onp] = _y;                                                       \
         ola[onp] = _a;                                                       \
         osd[onp] = onp;                                                      \
         onp++;};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GET_FILL_CONTOUR_COLOR - return a color index for the filled contour
 *                            - level L out of N
 */

int _PG_get_fill_contour_color(PG_device *dev, int l, int n)
   {int nc, clr;
    PG_palette *pal;

    pal = PG_fget_palette(dev, NULL);
    nc  = pal->n_pal_colors - 2;

    clr = l*nc/n + 2;

    return(clr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CONTOUR_LEVELS - Compute an array of NLEV contour levels with values
 *                   - between FMN and FMX and spaced with ratio R apart.
 *                   - Return the contour levels in LEV.
 *
 * #bind PG_contour_levels fortran() scheme() python()
 */

int PG_contour_levels(double *lev, int nlev, double fmn, double fmx,
		      double ratio)
   {int i;
    double delta, levn;

    if (nlev == 1)
       lev[0] = 0.5*(fmn + fmx);

    else
       {lev[0] = fmn;
	levn   = nlev - 1.0;
	if (ratio == 1.0)
	   delta = (fmx - fmn)/levn;
	else
	   delta = (fmx - fmn)*(1.0 - ratio)/
	           (ratio*(1.0 - POW(ratio, levn)));

	for (i = 1; i < nlev; i++)
	    {delta *= ratio;
	     lev[i] = lev[i-1] + delta;};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_ISO_LIMIT - find the extreme values of the array A which is NPTS long
 *              - return the min and max thru the pointers PMIN, PMAX
 */

void PG_iso_limit(double *a, int npts, double *pmin, double *pmax)
   {int i;
    double lmn, lmx;
 
    lmx = lmn = a[0];
    for (i = 0; i < npts; i++)
        {lmx = max(lmx, a[i]);
         lmn = min(lmn, a[i]);};

    *pmin = lmn;
    *pmax = lmx;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_ISO_LEVELS - setup a set of level values for a contour plot */

void PG_setup_iso_levels(PG_device *dev, PG_graph *data, PG_picture_desc *pd)
   {int nlev, datafl;
    double rlmn, rlmx, ratio;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    double *lv, *lev, *rextr;
    PM_set *range;
    pcons *alst;

    rlmn =  HUGE;
    rlmx = -HUGE;

    alst = pd->alist;
    PG_get_attrs_alist(alst,
		       "RATIO",    G_DOUBLE_I,    &ratio, _PG_gattrs.contour_ratio,
		       "N-LEVELS", G_INT_I, &nlev,  _PG_gattrs.contour_n_levels,
		       "LEVELS",   G_POINTER_I, &lv,    NULL,
		       NULL);

    if (nlev <= 0)
       nlev = 10;

    datafl = PG_render_data_type(data);

/* find the range limits if any */
    if (datafl)
       {range = data->f->range;
	rextr = PM_get_limits(range);}
    else
       {PG_image_picture_info(data, NULL, dbx, rbx);
	rextr = rbx;};

    if (rextr != NULL)
       {rlmn = rextr[0];
	rlmx = rextr[1];};

    CFREE(pd->levels);

    lev = CMAKE_N(double, nlev);
    if (lv == NULL)
       {if (dev->autorange || (rlmn == HUGE) || (rlmx == -HUGE))
	   {rextr = dev->range_extrema;
	    rlmn = rextr[0];
	    rlmx = rextr[1];};

/* compute the contour levels */
	PG_contour_levels(lev, nlev, rlmn, rlmx, ratio);}

    else
       PM_array_copy(lev, lv, nlev);

    pd->n_levels  = nlev;
    pd->levels    = lev;

/* GOTCHA: what is the point of the DRAW-LEGEND attribute if it is overridden?
    pd->legend_contour_fl = TRUE;
*/

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_POLY_FILL_CONTOUR - draw filled contours in a polygon */

static void _PG_poly_fill_contour(PG_device *dev, double *lev,
				  int ilev, int levmx,
				  double *a, double *px, double *py,
				  int is1, int is2, long *sides,
				  int nsp, int id, int method,
				  int *mcnt, int *mark,
				  double *xpt, double *ypt)
   {int inp, onp, ofs, clr;
    int is, os, in1, in2, ic, l1, l2;
    int icross[20];
    long osd[20];
    double fraca, fracb;
    double lv, a1, a2, xc, yc;
    double ilx[20], ily[20], ila[20];
    double olx[20], oly[20], ola[20];
    double x1[PG_SPACEDM], x2[PG_SPACEDM];

    if (dev == NULL)
       return;

    memset(ilx, 0, sizeof(ilx));
    memset(ily, 0, sizeof(ily));

    clr = _PG_get_fill_contour_color(dev, ilev, levmx);

    ofs = (nsp < 2);

/* recursively fill between contour levels */
    if (ilev < levmx)
       {lv = lev[ilev];

	inp = 0;
	onp = 0;
	ic  = 0;
	for (is = is1; is <= is2; is++)
	    {os  = is*nsp;
	     in1 = sides[os];
	     in2 = (ofs && (is == is2)) ? sides[is1] : sides[os+1];

	     a1 = a[in1];
	     a2 = a[in2];

	     if (a1 < lv)
		{ADD_INTERIOR_PT(px[in1], py[in1], a1);}
	     else if (lv < a1)
		{ADD_EXTERIOR_PT(px[in1], py[in1], a1);};

/* leaving or entering */
	     if (((a1 < lv) && (lv <= a2)) ||
		 ((a2 <= lv) && (lv < a1)))
		{fracb = (lv - a1)/(a2 - a1 + SMALL);
		 fraca = 1.0 - fracb;
		 xc    = fraca*px[in1] + fracb*px[in2];
		 yc    = fraca*py[in1] + fracb*py[in2];
		 icross[ic++] = inp;
		 ADD_INTERIOR_PT(xc, yc, lv);
		 ADD_EXTERIOR_PT(xc, yc, lv);};};

	if (inp > 2)
	   {ilx[inp] = ilx[0];
	    ily[inp] = ily[0];
	    PG_fill_polygon(dev, clr, TRUE, ilx, ily, inp+1);};

	if (onp > 2)
	   {osd[onp] = osd[0];
	    _PG_poly_fill_contour(dev, lev, ilev+1, levmx,
				  ola, olx, oly,
				  0, onp, osd, 1, id, method,
				  mcnt, mark, xpt, ypt);};

	if ((method == 3) && (ic == 2))
	   {l1 = icross[0];
	    l2 = icross[1];
	    PG_fset_line_color(dev, dev->WHITE, TRUE);
	    x1[0] = ilx[l1];
	    x1[1] = ily[l1];
	    x2[0] = ilx[l2];
	    x2[1] = ily[l2];
	    PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);};}

/* get anything outside the highest contour level */
    else
       {inp = 0;
	for (is = is1; is <= is2; is++)
	    {os  = is*nsp;
	     in1 = sides[os];
	     ilx[inp] = px[in1];
	     ily[inp] = py[in1];
	     inp++;};

	ilx[inp] = ilx[0];
	ily[inp] = ily[0];
	PG_fill_polygon(dev, clr, TRUE, ilx, ily, inp+1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_POLY_CONTOUR - draw contours lines in a polygon */

static void _PG_poly_contour(PG_device *dev, double *lev, int nlev,
			     double *a, double *px, double *py,
			     int is1, int is2, long *sides,
			     int nsp, int id,
			     int *mcnt, int *mark, double *xpt, double *ypt)
   {int i, ilev, npt, ok_ids, ofs;
    int is, os, in1, in2;
    double fraca, fracb, lv, a1, a2, fm;
    double x1[PG_SPACEDM], x2[PG_SPACEDM], ave[PG_SPACEDM];

    ok_ids = dev->data_id;
    ofs    = (nsp < 2);

    fm = 1.0 - TOLERANCE;

    for (ilev = 0; ilev < nlev; ilev++)
        {lv = lev[ilev];

	 npt = 0;
	 for (is = is1; is <= is2; is++)
	     {os  = is*nsp;
	      in1 = sides[os];
	      in2 = (ofs && (is == is2)) ? sides[is1] : sides[os+1];

	      a1 = a[in1];
	      a2 = a[in2];
	      if (((a1 < fm*lv) && (fm*lv <= a2)) ||
		  ((a2 < fm*lv) && (fm*lv <= a1)))
		 {fracb = (lv - a1)/(a2 - a1 + SMALL);
		  fraca = 1.0 - fracb;
		  xpt[npt] = fraca*px[in1] + fracb*px[in2];
		  ypt[npt] = fraca*py[in1] + fracb*py[in2];
		  npt++;};};

	 if (npt == 2)
	    {x1[0] = xpt[0];
	     x1[1] = ypt[0];
	     x2[0] = xpt[1];
	     x2[1] = ypt[1];
	     PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);
	     if (id && ok_ids)
	        {if (mcnt[ilev] > 9)
		    {mcnt[ilev] = 0;
		     ave[0] = 0.5*(xpt[0] + xpt[1]);
		     ave[1] = 0.5*(ypt[0] + ypt[1]);
		     PG_write_n(dev, 2, WORLDC, ave, "%c", mark[ilev]);}
		 else
		    mcnt[ilev]++;};}

	 else if (npt > 2)
	    {mcnt[ilev]++;

	     x1[0] = 0.0;
	     x1[1] = 0.0;
	     for (i = 0; i < npt; i++)
	         {x1[0] += xpt[0];
		  x1[1] += ypt[0];};
	     x1[0] /= (double) npt;
	     x1[1] /= (double) npt;
	     for (i = 0; i < npt; i++)
	         {x2[0] = xpt[i];
		  x2[1] = ypt[i];
		  PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_PICTURE_CONTOUR - setup a window for a contour rendering
 *                          - NOTE: no drawing of any kind is to be done here
 */

PG_picture_desc *PG_setup_picture_contour(PG_device *dev, PG_graph *data,
					  int save, int clear)
   {int nde, nre, change;
    double ndc[PG_BOXSZ];
    double *dpex, *ddex, *pdx, *rpex, *rdex, *prx, *vwprt;
    PM_set *domain;
    PG_picture_desc *pd;
    PG_par_rend_info *pri;
    PG_device *dd;
    pcons *alst;

    if (dev == NULL)
       return(NULL);

    change = !dev->supress_setup;

    pd = PG_get_rendering_properties(dev, data);
    if (pd != NULL)

/* we are drawing a contour plot so we do not want a fill-poly type legend */
       {pd->legend_palette_fl = FALSE;

	alst = pd->alist;
	pri  = dev->pri;
	if (pri != NULL)
	   {dd = pri->dd;
	    if (dd != NULL)
	       {dd->pri->alist  = alst;
		dd->pri->render = PLOT_CONTOUR;};};

/* setup the viewport */
	vwprt = pd->viewport;
	if (vwprt != NULL)
	   PG_box_copy(2, ndc, vwprt);
	else
	   {ndc[0] = 0.175;
	    ndc[1] = 0.735;
	    ndc[2] = 0.175;
	    ndc[3] = 0.825;};

	if (change)
	   {PG_set_viewspace(dev, 2, NORMC, ndc);

/* find the extrema for this frame */
	    PG_find_extrema(data, 0.0, &dpex, &rpex,
			    &nde, &ddex, &nre, &rdex);

/* setup the range limits */
	    prx = ((dev->autorange == TRUE) || (rpex == NULL)) ? rdex : rpex;
	    PG_register_range_extrema(dev, nre, prx);

/* setup the domain limits */
	    pdx = ((dev->autodomain == TRUE) || (dpex == NULL)) ? ddex : dpex;
	    PG_set_viewspace(dev, nde, WORLDC, pdx);

	    CFREE(ddex);
	    CFREE(rdex);

/* set up the drawing properties */
	    PG_fset_palette(dev, "standard");

	    PG_fset_line_color(dev, dev->WHITE, TRUE);
	    PG_fset_text_color(dev, dev->WHITE, TRUE);

	    PG_setup_iso_levels(dev, data, pd);

	    domain = data->f->domain;
	    if (domain->dimension_elem == 3)
	       {pd->ax_type = CARTESIAN_3D;
		pd->axis_fl = FALSE;};};};

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_ISO_NC_LR_2D - plot iso-contours for the function A on the
 *                  - Logical-Rectangular mesh specified by coordinates
 *                  - X and Y which is IMX by JMX and node centered
 *                  - plot NLEV contour levels specified in LEV
 *                  - if ID is non-zero label the contour levels starting
 *                  - with ID as the first character
 *                  - the mesh topology is always ignored and is there
 *                  - to be call compatible with the AC version
 */

static void _PG_iso_nc_lr_2d(PG_device *dev, double *a,
			     int ndd, double **x, double *lev,
			     int nlev, int id, void *cnnct, pcons *alist)
   {int i, im, j, jm, l, l1, ilev;
    int method, imx, jmx, nmap, eflag;
    int *maxes, *mcnt, *mark;
    long sides[5];
    double xpt[10], ypt[10];
    double *x1[2], *x2[2], *x3[2], *x4[2];
    double *a1, *a2, *a3, *a4;
    double px[4], py[4], pa[4];
    char *emap;

    maxes = (int *) cnnct;
    imx   = maxes[0];
    jmx   = maxes[1];
    nmap  = (imx - 1)*(jmx - 1);

    PG_get_attrs_alist(alist,
		       "CONTOUR-METHOD", G_INT_I, &method, 0,
		       NULL);

    emap = PM_check_emap(&eflag, alist, nmap);

    PM_LOGICAL_ZONE(x[0], x1[0], x2[0], x3[0], x4[0], imx);
    PM_LOGICAL_ZONE(x[1], x1[1], x2[1], x3[1], x4[1], imx);
    PM_LOGICAL_ZONE(a,    a1, a2, a3, a4, imx);

    mark = CMAKE_N(int, nlev);
    mcnt = CMAKE_N(int, nlev);

    for (ilev = 0; ilev < nlev; ilev++)
        {if (id)
            {mark[ilev] = id + ilev;
             mcnt[ilev] = 0;};};
  
    im = imx - 1;
    jm = jmx - 1;
    for (j = 0; j < jm; j++)
        for (i = 0; i < im; i++)
	    {l  = j*imx + i;
	     l1 = j*im + i;
	     if (emap[l1] == 0)
	        continue;

	     px[0] = x1[0][l];
	     px[1] = x2[0][l];
	     px[2] = x3[0][l];
	     px[3] = x4[0][l];

	     py[0] = x1[1][l];
	     py[1] = x2[1][l];
	     py[2] = x3[1][l];
	     py[3] = x4[1][l];

	     pa[0] = a1[l];
	     pa[1] = a2[l];
	     pa[2] = a3[l];
	     pa[3] = a4[l];

	     sides[0] = 0;
	     sides[1] = 1;
	     sides[2] = 2;
	     sides[3] = 3;
	     sides[4] = 0;

/* color filled contours with lines (3) and without lines (4) */
	     if ((method == 3) || (method == 4))
	        _PG_poly_fill_contour(dev, lev, 0, nlev, pa, px, py,
				      0, 4, sides, 1, id, method,
				      mcnt, mark, xpt, ypt);
/* just contour lines (1) */
	     else
	        _PG_poly_contour(dev, lev, nlev, pa, px, py,
				 0, 4, sides, 1, id,
				 mcnt, mark, xpt, ypt);};

    CFREE(mark);
    CFREE(mcnt);

    if (eflag)
        CFREE(emap);

    return;}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_ISO_NC_LR - Draw iso-contours for the mapping {X} -> {A} in
 *                   - the current frame of the device DEV.
 *                   - The remaining arguments are:
 *                   -    NDD    the dimension of the domain specified by X
 *                   -    LEV    the array of contour levels
 *                   -    NLEV   the number of contour levels
 *                   -    ID     an identifier for the set of contours
 *                   -           useful when plotting more than one set
 *                   -           of contours in a frame
 *                   -    CNNCT  connectivity information for
 *                   -           arbitrarily connected meshes
 *                   -    ALIST  assocation list of rendering
 *                   -           attributes and their values
 *
 * #bind PG_draw_iso_nc_lr fortran() scheme() python()
 */

void PG_draw_iso_nc_lr(PG_device *dev ARG(,,cls), double *a,
		       int ndd, double **x, double *lev,
		       int nlev, int id, void *cnnct, pcons *alist)
   {int method;

    PG_get_attrs_alist(alist,
		       "CONTOUR-METHOD", G_INT_I, &method, 0,
		       NULL);

    if (ndd == 2)
       {if (method == 2)
	   _PG_iso_nc_lr_3d(dev, a, ndd, x, lev, nlev, id,
			    cnnct, alist, method);
        else
	   _PG_iso_nc_lr_2d(dev, a, ndd, x, lev, nlev, id, cnnct, alist);}

    else if (ndd == 3)
       _PG_iso_nc_lr_3d(dev, a, ndd, x, lev, nlev, id,
			cnnct, alist, method);

    return;}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_ISO_ZC_LR - draw iso contour lines assuming a zone centered
 *                   - Logical-Rectangular mesh
 */

static void PG_draw_iso_zc_lr(PG_device *dev, double *a,
			      int ndd, double **x, double *lev,
			      int nlev, int id, void *cnnct, pcons *alist)
   {double *ap;

    if (ndd == 2)
       {ap = PM_z_n_lr_2d(a, x[0], x[1],
			  _PG_gattrs.interp_flag, cnnct, alist);

	PG_draw_iso_nc_lr(dev, ap, ndd, x, lev, nlev, id, cnnct, alist);

	CFREE(ap);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_ISO_NC_AC - plot iso-contours for the function A on the
 *                   - Arbitrarily-Connected mesh specified by coordinates
 *                   - X and Y which is IMX by JMX and node centered
 *                   - plot NLEV contour levels specified in LEV
 *                   - if ID is non-zero label the contour levels starting
 *                   - with the character ID
 */

static void PG_draw_iso_nc_ac(PG_device *dev, double *a,
			      int ndd, double **x, double *lev,
			      int nlev, int id, void *cnnct, pcons *alist)
   {int ilev, iz, oz, is1, is2;
    int nz, nzp, nsp, ofz, method;
    int *mcnt, *mark, *nc, *np;
    long **cells, *zones, *sides;
    double *xpt, *ypt, *px, *py;
    PM_mesh_topology *mt;

    PG_get_attrs_alist(alist,
		       "CONTOUR-METHOD", G_INT_I, &method, 0,
		       NULL);

    mt = (PM_mesh_topology *) cnnct;

    cells = mt->boundaries;
    zones = cells[2];
    sides = cells[1];

    nc = mt->n_cells;
    nz = nc[2] - 1;

    np  = mt->n_bound_params;
    nzp = np[2];
    nsp = np[1];

    px = x[0];
    py = x[1];

/* can compute this by looping over the sides */
    xpt = CMAKE_N(double, 10);
    ypt = CMAKE_N(double, 10);

    mark = CMAKE_N(int, nlev);
    mcnt = CMAKE_N(int, nlev);

    ofz = (nzp < 2);
    for (ilev = 0; ilev < nlev; ilev++)
        {if (id)
            {mark[ilev] = id + ilev;
             mcnt[ilev] = 0;};};
  
    for (iz = 0; iz <= nz; iz++)
        {oz  = iz*nzp;
	 is1 = zones[oz];
	 is2 = zones[oz+1] - ofz;

	 if (method == 3)
	    _PG_poly_fill_contour(dev, lev, 0, nlev, a, px, py,
				  is1, is2, sides, nsp, id, method,
				  mcnt, mark, xpt, ypt);
	 else
	    _PG_poly_contour(dev, lev, nlev, a, px, py,
			     is1, is2, sides,
			     nsp, id, mcnt, mark, xpt, ypt);};

    CFREE(mark);
    CFREE(mcnt);
    CFREE(xpt);
    CFREE(ypt);

    return;}
  
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_ISO_ZC_AC - draw iso contour lines assuming a zone centered
 *                   - Arbitrarily-Connected mesh
 */

static void PG_draw_iso_zc_ac(PG_device *dev, double *a,
			      int ndd, double **x, double *lev,
			      int nlev, int id, void *cnnct, pcons *alist)
   {double *ap;

    ap = PM_z_n_ac_2d(a, x[0], x[1],
		      _PG_gattrs.interp_flag, cnnct, alist);

    PG_draw_iso_nc_ac(dev, ap, ndd, x, lev, nlev, id, cnnct, alist);

    CFREE(ap);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_ISO_HAND - iso-contour plot handler
 *             - for the graph G on the device DEV
 *             - plot NLEV contour levels
 */

static void PG_iso_hand(PG_device *dev, PG_graph *g, PG_picture_desc *pd,
		        PFCntrZC fnc_zc, PFCntrNC fnc_nc)
   {int id, ndd, npts;
    int color, style, nlev;
    double width;
    double **x, *f, *lev, **r;
    void *cnnct;
    pcons *alst;
    PM_centering centering;
    PM_mapping *h;
    PM_set *domain, *range;

    if (dev == NULL)
       return;

    PG_fset_clipping(dev, TRUE);

    nlev = pd->n_levels;
    lev  = pd->levels;
    id   = g->identifier;
    id   = max(id, '@');

    for (h = g->f; h != NULL; h = h->next)
        {PG_get_attrs_graph(g, FALSE,
			    "LINE-COLOR", G_INT_I, &color, dev->BLUE,
			    "LINE-STYLE", G_INT_I, &style, LINE_SOLID,
			    "LINE-WIDTH", G_DOUBLE_I,  &width, 0.0,
			    NULL);

	 PG_fset_line_style(dev, style);
	 PG_fset_line_width(dev, width);

	 domain = h->domain;
	 npts  = domain->n_elements;
	 ndd   = domain->dimension;
	 r     = (double **) domain->elements;

	 x = PM_convert_vectors(ndd, npts, r, domain->element_type);

	 range = h->range;
	 npts  = range->n_elements;

	 f = PM_array_real(range->element_type, DEREF(range->elements), npts, NULL);

/* find the additional mapping information */
	 centering = N_CENT;
	 alst      = PM_mapping_info(h,
				     "CENTERING", &centering,
				     NULL);
	 cnnct     = PM_connectivity(h);

	 PG_fset_line_color(dev, color, TRUE);
	 switch (centering)
	    {case Z_CENT :
                  (*fnc_zc)(dev, f, ndd, x, lev, nlev, id, cnnct, alst);
                  break;

             case N_CENT :
                  (*fnc_nc)(dev, f, ndd, x, lev, nlev, id, cnnct, alst);
                  break;

             case F_CENT :
             case U_CENT :
             default     :
                  break;};

	 PG_draw_domain_boundary(dev, h);

	 PG_update_vs(dev);

	 PM_free_vectors(ndd, x);

	 CFREE(f);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CONTOUR_CORE - contour plot skeleton routine */

static void _PG_contour_core(PG_device *dev, PG_graph *data,
			     PFCntrZC fnc_zc, PFCntrNC fnc_nc)
   {PG_graph *g;
    PG_picture_desc *pd;

    if ((dev != NULL) && (data != NULL))
       {data->rendering = PLOT_CONTOUR;

	pd = PG_setup_picture(dev, data, TRUE, TRUE, TRUE);

/* this has to be done again here to be correct for parallel renderings */
	PG_setup_iso_levels(dev, data, pd);

	PG_adorn_before(dev, pd, data);

/* plot all of the current functions */
	for (g = data; g != NULL; g = g->next)
	    PG_iso_hand(dev, g, pd, fnc_zc, fnc_nc);

	PG_finish_picture(dev, data, pd);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_CONTOUR_PLOT - main contour plot control routine */

void PG_contour_plot(PG_device *dev, PG_graph *data, ...)
   {double ratio, *pr;
        
    PG_get_attrs_graph(data, FALSE,
		       "RATIO", G_DOUBLE_I, &ratio, 1.0,
		       NULL);
    if (ratio == 0.0)
       {SC_VA_START(data);
        pr  = CMAKE(double);
        *pr = SC_VA_ARG(double);
	PG_set_attrs_graph(data,
			   "RATIO", G_DOUBLE_I, TRUE, pr,
			   NULL);
        SC_VA_END;};

    if (strcmp(data->f->category, PM_LR_S) == 0)
       _PG_contour_core(dev, data, PG_draw_iso_zc_lr, PG_draw_iso_nc_lr);

    else if (strcmp(data->f->category, PM_AC_S) == 0)
       _PG_contour_core(dev, data, PG_draw_iso_zc_ac, PG_draw_iso_nc_ac);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
