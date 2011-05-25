/*
 * GSVECT.C - vector plot routines for Portable Graphics System
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

typedef void (*PFVectZC)(PG_device *dev, double **u, double **r,
                         void *cnnct,  pcons *alst);

typedef void (*PFVectNC)(PG_device *dev, double **u, double **r,
                         void *cnnct, pcons *alst);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_PICTURE_VECTOR - setup a window for a vector rendering
 *                         - NOTE: no drawing of any kind is to be done here
 */

PG_picture_desc *PG_setup_picture_vector(PG_device *dev, PG_graph *data, 
                                         int save, int clear)
   {int nde, nre, change;
    double ndc[PG_BOXSZ];
    double *dpex, *ddex, *pdx, *rpex, *rdex, *prx, *vwprt;
    PG_picture_desc *pd;
    PG_par_rend_info *pri;
    PG_device *dd;
    pcons *alst;

    if ((dev == NULL) || (data == NULL))
       return(NULL);

    change = !dev->supress_setup;

    pd = PG_get_rendering_properties(dev, data);

    pd->legend_palette_fl = FALSE;
    pd->legend_contour_fl = FALSE;

    alst = pd->alist;
    pri  = dev->pri;
    if (pri != NULL)
       {dd = pri->dd;
	if (dd != NULL)
	   {dd->pri->alist  = alst;
	    dd->pri->render = PLOT_VECTOR;};};

    vwprt = pd->viewport;
    if (vwprt != NULL)
       {ndc[0] = vwprt[0];
        ndc[1] = vwprt[1];
        ndc[2] = vwprt[2];
        ndc[3] = vwprt[3];}
    else
       {ndc[0] = 0.175;
        ndc[1] = 0.90;
        ndc[2] = 0.175;
        ndc[3] = 0.90;};

    if (change)
       {PG_set_viewspace(dev, 2, NORMC, ndc);

/* find the extrema for this frame */
	PG_find_extrema(data, 0.05, &dpex, &rpex, &nde, &ddex, &nre, &rdex);

/* setup the range limits */
	prx = ((dev->autorange == TRUE) || (rpex == NULL)) ? rdex : rpex;
	PG_register_range_extrema(dev, nre, prx);

/* setup the domain limits */
	pdx = ((dev->autodomain == TRUE) || (dpex == NULL)) ? ddex : dpex;
	PG_set_viewspace(dev, 2, WORLDC, pdx);

	CFREE(ddex);
	CFREE(rdex);

	PG_set_palette(dev, "standard");

	PG_set_color_line(dev, dev->WHITE, TRUE);
	PG_set_color_text(dev, dev->WHITE, TRUE);

/*        PG_set_vec_attr(dev, VEC_SCALE, scale, 0); */

	PG_set_clipping(dev, TRUE);};

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_VCT_HAND - plot arrows with heads (vectors)
 *             - The first argument is the device to draw to and
 *             - the second argument is the graph to draw.
 */

static void PG_vct_hand(PG_device *dev, PG_graph *g,
                        PFVectZC fnc_zc, PFVectNC fnc_nc)
   {int npts, color, style;
    double width;
    double **r, **d, **u, **v;
    PM_centering centering;
    PM_mapping *h;
    PM_set *domain, *range;
    PG_dev_attributes *attr;
    void *cnnct;
    pcons *alst;

    if ((dev == NULL) || (g == NULL))
       return;

    h = g->f;

    domain = h->domain;
    npts   = domain->n_elements;
    r      = (double **) domain->elements;
    d      = PM_convert_vectors(2, npts, r, domain->element_type);

    range = h->range;
    u     = (double **) range->elements;
    v     = PM_convert_vectors(2, npts, u, range->element_type);

/* save user's values for various attributes */
    attr = PG_get_attributes(dev);

/* set the default plotting attributes for vectors */
    PG_get_attrs_graph(g, FALSE,
		       "LINE-COLOR", SC_INT_I, &color, dev->BLUE,
		       "LINE-STYLE", SC_INT_I, &style, LINE_SOLID,
		       "LINE-WIDTH", SC_DOUBLE_I,    &width, 0.0,
		       NULL);

    PG_set_color_line(dev, color, TRUE);
    PG_set_line_style(dev, style);
    PG_set_line_width(dev, width);

/* find the additional mapping information */
    centering = N_CENT;
    alst = PM_mapping_info(h,
			   "CENTERING", &centering,
			   NULL);

    cnnct = PM_connectivity(h);

/* this is done consistently with PG_draw_contour */
    switch (centering)
       {case Z_CENT :
             (*fnc_zc)(dev, v, d, cnnct, alst);
	     break;

        case N_CENT :
             (*fnc_nc)(dev, v, d, cnnct, alst);
	     break;

        case F_CENT :
        case U_CENT :
        default     :
	     break;};

    PG_draw_domain_boundary(dev, h);

/* reset user's values for various attributes */
    PG_set_attributes(dev, attr);
    CFREE(attr);

    PM_free_vectors(2, d);
    PM_free_vectors(2, v);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_VCT_ZC_LR - draw zone centered logical rectangular vector plot */

static void _PG_draw_vct_zc_lr(PG_device *dev, double **u, double **r,
			       void *cnnct,  pcons *alst)
   {int i, j, nn, nmap, nz, kmax, lmax, km, lm, *maxes;
    int eflag;
    double *vx1, *vx2, *vx3, *vx4;
    double *vy1, *vy2, *vy3, *vy4;
    double **v;
    double valx, valy;
    char *emap;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];
    nn    = kmax*lmax;
    nmap  = (kmax - 1) * (lmax - 1);

    emap = PM_check_emap(&eflag, alst, nmap);

    v = PM_make_vectors(2, nn);

    PM_LOGICAL_ZONE(v[0], vx1, vx2, vx3, vx4, kmax);
    PM_LOGICAL_ZONE(v[1], vy1, vy2, vy3, vy4, kmax);

    km = kmax - 1;
    lm = lmax - 1;
    nz = km*lm;
    for (j = 0; j < nz; j++)
        {valx = 0.25*u[0][j];
	 valy = 0.25*u[1][j];
	 i    = j + j/km;
	 vx1[i] += valx;
	 vx2[i] += valx;
	 vx3[i] += valx;
	 vx4[i] += valx;
	 vy1[i] += valy;
	 vy2[i] += valy;
	 vy3[i] += valy;
	 vy4[i] += valy;};

    PG_draw_vector_n(dev, 2, WORLDC, nn, r, v);

    PM_free_vectors(2, v);

    if (eflag)
        CFREE(emap);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_VCT_NC_LR - draw node centered logical rectangular vector plot */

static void _PG_draw_vct_nc_lr(PG_device *dev, double **u, double **r,
			       void *cnnct, pcons *alst)
   {int nn, kmax, lmax, *maxes;

    maxes = (int *) cnnct;
    kmax  = maxes[0];
    lmax  = maxes[1];
    nn    = kmax*lmax;

    PG_draw_vector_n(dev, 2, WORLDC, nn, r, u);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_VCT_NC_AC - draw node centered arbitrarily connected vector plot */

static void _PG_draw_vct_nc_ac(PG_device *dev, double **u, double **r,
			       void *cnnct, pcons *alst)
   {int nn, *nc;
    PM_mesh_topology *mt;

    mt = (PM_mesh_topology *) cnnct;
    nc = mt->n_cells;
    nn = nc[0];

    PG_draw_vector_n(dev, 2, WORLDC, nn, r, u);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_VCT_ZC_AC - draw zone centered arbitrarily connected vector plot */

static void _PG_draw_vct_zc_ac(PG_device *dev, double **u, double **r,
			       void *cnnct, pcons *alst)
   {double *a[PG_SPACEDM];

    a[0] = PM_z_n_ac_2d(u[0], r[0], r[1],
			_PG_gattrs.interp_flag, cnnct, alst);
    a[1] = PM_z_n_ac_2d(u[1], r[0], r[1],
			_PG_gattrs.interp_flag, cnnct, alst);

    _PG_draw_vct_nc_ac(dev, a, r, cnnct, alst);

    CFREE(a[0]);
    CFREE(a[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_VECTOR_N - draw N ND vectors in CS
 *                  -   X base points of vectors
 *                  -   U vectors
 */

void PG_draw_vector_n(PG_device *dev, int nd, PG_coord_sys cs, long n,
		      double **x, double **u)
   {int i, l, id, il, iw, imx, jd, jl, nl, nsv, nw;
    double xc, xs, r, f, lscale, size, hs, sf;
    double rmx, rdm, tol, vecmax, angle, ca, sa, rc;
    double hd[PG_SPACEDM], sc[PG_SPACEDM];
    double dx[PG_SPACEDM], x0[PG_SPACEDM], x1[PG_SPACEDM];
    double xw[2][PG_SPACEDM], rw[2][PG_SPACEDM][PG_SPACEDM];
    double **t;
    PG_dev_geometry *g;

    if (dev == NULL)
       return;

    if (POSTSCRIPT_DEVICE(dev))
       {FILE *fp;

        fp = dev->file;
        io_printf(fp, "\n%% Begin Vectors\n\n");};

    g   = &dev->g;
    tol = 10.0*SMALL;

/* calculate the projections for the wings on the normalized vector */
    angle = _PG.hdangle*(atan(1.0)/45.0);
    ca    = cos(angle);
    sa    = sin(angle);

    SC_MEM_INIT_V(rw);

    rw[0][0][0] = ca;
    rw[0][0][1] = sa;
    rw[0][1][0] = -sa;
    rw[0][1][1] = ca;
    rw[0][2][1] = 1.0;

    rw[1][0][0] = ca;
    rw[1][0][1] = -sa;
    rw[1][1][0] = sa;
    rw[1][1][1] = ca;
    rw[1][2][1] = 1.0;

/* if a maximum size was specified, compute the max vector length */
    if (_PG.maxvsz != 0)
       {vecmax = 0.0;
        for (i = 0; i < n; i++)
	    {xs = 0.0;
	     for (id = 0; id < nd; id++)
	         {xc  = u[id][i];
		  xs += xc*xc;};
             size   = sqrt(xs);
             vecmax = max(vecmax, size);};
        lscale = _PG.sizemax/(vecmax + SMALL);}
    else
       lscale = _PG.scale;

/* compute a global scale factor to limit vectors size
 * in order to avoid integer conversion overflows later on
 * this happens with vectors whose magnitude is much bigger
 * than the mesh scales
 */
    for (id = 0; id < nd; id++)
        {l = 2*id + 1;
	 sc[id] = g->w_nd[l]*lscale;};

    rmx = 0.0;
    imx = -1;
    for (i = 0; i < n; ++i)
        {xs = 0.0;
	 for (id = 0; id < nd; id++)
	     {xc  = sc[id]*u[id][i];
	      xs += xc*xc;};
	 size = sqrt(xs) + SMALL;
         if (size > rmx)
            {rmx = size;
	     imx = i;};};

    SC_ASSERT(imx >= 0);

    xs = 0.0;
    for (id = 0; id < nd; id++)
        {l   = 2*id;
	 xc  = g->wc[l+1] - g->wc[l];
	 xs += xc*xc;
	 dx[id] = xc;};

    rdm = sqrt(xs);
    rmx = rdm/rmx;
    rmx = min(1.0, rmx);
    rdm *= 10.0;

/* n-segments-vector * n-vectors */
    nw  = 2;
    nsv = 1 + nw;
    nl  = nsv*n;
    t   = PM_make_vectors(nd, 2*nl);

/* compute the vectors */
    for (i = 0, il = 0; i < n; ++i, il += 2*nsv)
        {for (id = 0; id < nd; id++)
	     {x0[id] = x[id][i];
	      dx[id] = sc[id]*u[id][i];};

	 PG_trans_point(dev, nd, cs, x0, NORMC, x0);

	 xs = 0.0;
	 for (id = 0; id < nd; id++)
	     {xc  = dx[id];
	      xs += xc*xc;};
         size = sqrt(xs) + SMALL;

/* set the scale factor */
         if (_PG.ifix)
            r = _PG.fixsize;
         else
            r = rmx*size;

/* NOTE: limit vector lengths to 10 times the domain size
 * to eliminate integer overflows when converting to PC
 */
	 r = min(r, rdm);
	 if (r <= tol)
	    continue;

         if (_PG.ifixhead)
            {f  = _PG.headsize;
	     f *= (nd - 1)*(nd - 1);}
         else
            f = _PG.headsize*r;

	 sf = r/size;
	 hs = f/r;
	 for (id = 0; id < nd; id++)
	     {dx[id] = sf*dx[id];
	      hd[id] = hs*dx[id];
	      x1[id] = x0[id] + dx[id];};

/* compute wings */
	 for (iw = 0; iw < nw; iw++)
	     {for (id = 0; id < nd; id++)
		  {rc = 0.0;
		   for (jd = 0; jd < nd; jd++)
		       rc += rw[iw][id][jd]*hd[jd];
		   xw[iw][id] = x1[id] - rc;};};

	 for (id = 0; id < nd; id++)
	     {jl = il;

/* add shaft */
	      t[id][jl++] = x0[id];
	      t[id][jl++] = x1[id];

/* add wings */
	      for (iw = 0; iw < nw; iw++)
		  {t[id][jl++] = x1[id];
		   t[id][jl++] = xw[iw][id];};};};

    PG_trans_points(dev, 2*nl, nd, NORMC, t, WORLDC, t);
    PG_draw_disjoint_polyline_n(dev, nd, WORLDC, nl, t, FALSE);

    PM_free_vectors(nd, t);

    if (POSTSCRIPT_DEVICE(dev))
       {FILE *fp;

        fp = dev->file;
        io_printf(fp, "\n%% End Vectors\n\n");};

    return;}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* _PG_DRAW_VEC_DISTINCT - plots arrows (vectors) 
 *                       - guarantee all endpoints to be distinct
 *                       - X    - array of x coordinates
 *                       - Y    - array of y coordinates
 *                       - U    - array of velocity in x direction
 *                       - V    - array of velocity in y direction
 *                       - NPTS - number of elements in the arrays (arrows)
 *                       - NOTE: this is only not static to shut up compilers
 */

void _PG_draw_vec_distinct(PG_device *dev,
                           double *x, double *y, double *u, double *v, int npts)
   {int i;
    double r, f, lscale, aspect, size;
    double vecmax, angle, den;
    double cg, sg;
    double x0[PG_SPACEDM], x1[PG_SPACEDM], xw[0][PG_SPACEDM], xw[1][PG_SPACEDM];
    double hd[PG_SPACEDM], ca[PG_SPACEDM];
    double xp[PG_SPACEDM], dx[PG_SPACEDM], sx[PG_SPACEDM];
    double dp[PG_SPACEDM], wc[PG_BOXSZ];

    if (dev == NULL)
       return;

    PG_get_viewspace(dev, WORLDC, wc);
    aspect = (wc[3] - wc[2])/(wc[1] - wc[0] + SMALL);

    dp[0] = 1;
    dp[1] = 1;
    PG_trans_point(dev, 2, PIXELC, dp, WORLDC, dp);

/* if a maximum size was specified, compute the max vector length */
    if (_PG.maxvsz != 0)
       {vecmax = 0.0;
        for (i = 0; i < npts; i++)
            {xp[0]  = u[i];
             xp[1]  = v[i];
             size   = sqrt(xp[0]*xp[0] + xp[1]*xp[1]);
             vecmax = max(vecmax, size);};};

/* calculate the positions for the 2 wings on the normalized vector */
    angle = _PG.hdangle*(atan(1.0)/45.0);

    ca[0] = cos(angle);
    ca[1] = sin(angle);

    f   = _PG.headsize;
    den = sqrt(1.0 + f*(f - 2.0*ca[0])) + SMALL;
    sg  = f*ca[1]/den;
    cg  = (1.0 - f*ca[0])/den;

    if (_PG.maxvsz != 0)
       lscale = _PG.sizemax/(vecmax + SMALL);
    else
       lscale = _PG.scale;

/* compute the vectors */
    for (i = 0; i < npts; ++i)
        {x0[0] = x[i];
         x0[1] = y[i];

/* determine the cosine and sine of the angle the vector is to be
 * plotted at 
 */
         xp[0] = u[i];
         xp[1] = v[i];
         size  = sqrt(xp[0]*xp[0] + xp[1]*xp[1]) + SMALL;
         hd[0] = xp[0]/size;
         hd[1] = xp[1]/size;

/* set the x and y scale factor */
         if (_PG.ifix != 0)
            r = _PG.fixsize;
         else
            r = size*lscale;

         dx[0] = r*hd[0];
         dx[1] = r*hd[1];
         sx[0] = den*dx[0];
         sx[1] = den*dx[1];

/* compute new coordinates */
         x1[0] = x0[0] + dx[0];
         x1[1] = x0[1] + dx[1]*aspect;

         xw[0][0] = x0[0] + sx[0]*cg - sx[1]*sg;
         xw[0][1] = x0[1] + (sx[1]*cg + sx[0]*sg)*aspect;
         xw[1][0] = x0[0] + sx[0]*cg + sx[1]*sg;
         xw[1][1] = x0[1] + (sx[1]*cg - sx[0]*sg)*aspect;

         if ((x0[0] == x1[0]) && (x0[1] == x1[1]))
            {x1[0] += dp[0];
             x1[1] += dp[1];};
         if ((xw[0][0] == x1[0]) && (xw[0][1] == x1[1]))
            {xw[0][0] -= dp[0];
             xw[0][1] += dp[1];};
         if ((xw[1][0] == x1[0]) && (xw[1][1] == x1[1]))
            {xw[1][0] -= dp[0];
             xw[1][1] -= dp[1];};

/* draw the shaft */
         PG_move_gr_abs(dev, x0[0], x0[1]);
         PG_draw_to_abs(dev, x1[0], x1[1]);

/* draw the head */
         PG_move_gr_abs(dev, xw[0][0], xw[0][1]);
         PG_draw_to_abs(dev, x1[0], x1[1]);
         PG_draw_to_abs(dev, xw[1][0], xw[1][1]);};

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* PG_SET_VEC_ATTR - set the properties of the vectors for the
 *                 - next vector plot
 *                 - The parameters are paired, optional, and
 *                 - can be in any order.  For each pair, the first value
 *                 - describes the option, the second, the value.  The
 *                 - options are ints.  The values can be ints, doubles,
 *                 - or chars.  What  type the values are is determined
 *                 - by the option. The list must be ended  with a zero.
 *                 - 
 *                 -   VEC_SCALE     - a scale factor on vector lengths
 *                 -   VEC_ANGLE     - the angle between the arrow wings
 *                 -   VEC_FIXHEAD   - a fixed head size
 *                 -   VEC_HEADSIZE  - length of the wings
 *                 -   VEC_FIXSIZE   - a fixed vector length
 *                 -   VEC_MAXSIZE   - a maximum vector length
 *                 -   VEC_LINESTYLE - line style of vectors
 *                 -   VEC_LINETHICK - line width of vectors
 *                 -   VEC_COLOR     - line color of vectors
 */

void PG_set_vec_attr(PG_device *dev, ...)
   {int type, itemp;
    double diffvp, diffwd, temp;
    double wc[PG_BOXSZ];

/* get the current viewport & window -- this is used to go from NDC to
 * WC for specifying _PG.fixsize & _PG.maxvsz 
 */
    PG_get_viewspace(dev, NORMC, wc);
    diffvp = wc[1] - wc[0];
    PG_get_viewspace(dev, WORLDC, wc);
    diffwd = wc[1] - wc[0];

/* get the attributes */
    SC_VA_START(dev);
    while ((type = SC_VA_ARG(int)) != 0)
       {switch (type)
           {case VEC_SCALE :
	         _PG.scale = (double) SC_VA_ARG(double);
		 break;

            case VEC_ANGLE :
	         _PG.hdangle = (double) SC_VA_ARG(double);
		 break;

            case VEC_FIXHEAD :
	         _PG.ifixhead = SC_VA_ARG(int);
		 break;

            case VEC_HEADSIZE :
	         _PG.headsize = (double) SC_VA_ARG(double);
		 break;

            case VEC_FIXSIZE :
	         temp = SC_VA_ARG(double);
		 if (temp > 0.0)
		    {_PG.ifix = TRUE;
		     _PG.fixsize = temp*diffwd/diffvp;};
		 break;

            case VEC_MAXSIZE :
	         temp = SC_VA_ARG(double);
		 if (temp > 0.0)
		    {_PG.maxvsz = TRUE;
		     _PG.sizemax = temp*diffwd/diffvp;}
		 else
		    {_PG.maxvsz = FALSE;
		     _PG.sizemax = 0.0;};
		 break;

            case VEC_LINESTYLE :
	         itemp = SC_VA_ARG(int);
		 PG_set_line_style(dev, itemp);
		 break;

            case VEC_LINETHICK :
	         temp = SC_VA_ARG(double);
		 PG_set_line_width(dev, temp);
		 break;

            case VEC_COLOR :
	         itemp = SC_VA_ARG(int);
		 PG_set_color_line(dev, itemp, TRUE);
		 break;

            default :
	         break;};};

    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PGSVAT - set the properties of the vectors for the
 *        - next vector plot
 *        - (see the documentation for PD_SET_VEC_ATTR)
 */

void FF_ID(pgsvat, PGSVAT)(FIXNUM *sdid, ...)
   {int type, itemp;
    double diffvp, diffwd, temp;
    double wc[PG_BOXSZ];
    PG_device *dev;

    dev = *(PG_device **) sdid;

/* get the current viewport and window -- this is used to go from NDC to
 * WC for specifying _PG.fixsize and _PG.maxvsz 
 */
    PG_get_viewspace(dev, NORMC, wc);
    diffvp = wc[1] - wc[0];
    PG_get_viewspace(dev, WORLDC, wc);
    diffwd = wc[1] - wc[0];

/* get the attributes */
    SC_VA_START(sdid);
    while (TRUE)
       {type = *SC_VA_ARG(FIXNUM *);
        if (type == 0)
           break;

        switch (type)
           {case VEC_SCALE :
	         _PG.scale = (double) *SC_VA_ARG(double *);
		 break;

            case VEC_ANGLE :
	         _PG.hdangle = (double) *SC_VA_ARG(double *);
		 break;

            case VEC_FIXHEAD :
	         _PG.ifixhead = *SC_VA_ARG(FIXNUM *);
		 break;

            case VEC_HEADSIZE :
	         _PG.headsize = (double) *SC_VA_ARG(double *);
		 break;

            case VEC_FIXSIZE :
	         temp = *SC_VA_ARG(double *);
		 if (temp > 0.0)
		    {_PG.ifix = TRUE;
		     _PG.fixsize = temp*diffwd/diffvp;};
		 break;

            case VEC_MAXSIZE :
	         temp = *SC_VA_ARG(double *);
		 if (temp > 0.0)
		    {_PG.maxvsz = TRUE;
		     _PG.sizemax = temp*diffwd/diffvp;}
		 else
		    {_PG.maxvsz = FALSE;
		     _PG.sizemax = 0.0;};
		 break;

            case VEC_LINESTYLE :
	         itemp = *SC_VA_ARG(FIXNUM *);
		 PG_set_line_style(dev, itemp);
		 break;

            case VEC_LINETHICK :
	         temp = *SC_VA_ARG(double *);
		 PG_set_line_width(dev, temp);
		 break;

            case VEC_COLOR :
	         itemp = *SC_VA_ARG(FIXNUM *);
		 PG_set_color_line(dev, itemp, TRUE);
		 break;

            default :
	         break;};};

    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_VECTOR_CORE - vector plot skeleton routine */

static void _PG_vector_core(PG_device *dev, PG_graph *data,
                            PFVectZC fnc_zc, PFVectNC fnc_nc)
   {PG_graph *g;
    PG_picture_desc *pd;

    if ((dev != NULL) && (data != NULL))
       {data->rendering = PLOT_VECTOR;

	pd = PG_setup_picture(dev, data, TRUE, TRUE, TRUE);

	PG_adorn_before(dev, pd, data);

/* plot all of the current functions */
	for (g = data; g != NULL; g = g->next)
	    PG_vct_hand(dev, g, fnc_zc, fnc_nc);

	PG_finish_picture(dev, data, pd);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_VECTOR_PLOT - main vector plot control routine */

void PG_vector_plot(PG_device *dev, PG_graph *data, ...)
   {

    SC_VA_START(data);

    if (strcmp(data->f->category, PM_LR_S) == 0)
       _PG_vector_core(dev, data, _PG_draw_vct_zc_lr, _PG_draw_vct_nc_lr);

    else if (strcmp(data->f->category, PM_AC_S) == 0)
       _PG_vector_core(dev, data, _PG_draw_vct_zc_ac, _PG_draw_vct_nc_ac);

    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/

/*                            FORTRAN API ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* PGPLVC - low level vector plot routine */

FIXNUM FF_ID(pgplvc, PGPLVC)(FIXNUM *sdid,
			     double *ax, double *ay, double *au, double *av,
			     FIXNUM *sn, FIXNUM *said)
   {FIXNUM rv;
    double *x[PG_SPACEDM], *u[PG_SPACEDM];
    PG_device *dev;

    dev = SC_GET_POINTER(PG_device, *sdid);

    x[0] = ax;
    x[1] = ay;
    u[0] = au;
    u[1] = av;

    PG_draw_vector_n(dev, 2, WORLDC, *sn, x, u);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
