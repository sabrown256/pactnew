/*
 * GSCRV.C - 1D renderings for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_PICTURE_CURVE - setup a window for the 1D-1D renderings
 *                        - NOTE: no drawing of any kind is to be done here
 */

PG_picture_desc *PG_setup_picture_curve(PG_device *dev, PG_graph *data,
				        int save, int clear)
   {int n, plt, bare, datafl, change;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ], wc[PG_BOXSZ], ndc[PG_BOXSZ];
    double dy, tol, avg;
    double extr[2], *dextr, *rextr;
    double *vwprt, *rc, *tc;
    PM_mapping *pf;
    PM_set *domain, *range;
    PG_rendering pty;
    PG_graph *g;
    PG_picture_desc *pd;
    PG_par_rend_info *pri;
    PG_device *dd;
    pcons *alst;
    static double HC = 0.0;

    if ((dev == NULL) || (data == NULL))
       return(NULL);

    if (HC == 0.0)
       HC = HUGE_REAL*HUGE_REAL*HUGE_REAL;

    change = !dev->supress_setup;

    pd = PG_get_rendering_properties(dev, data);
    if (pd == NULL)
       return(NULL);

    pd->legend_palette_fl = FALSE;
    pd->legend_contour_fl = FALSE;

    pty  = pd->rendering;
    alst = pd->alist;
    pri  = dev->pri;
    if (pri != NULL)
       {dd = pri->dd;
	if (dd != NULL)
	   {dd->pri->alist  = alst;
	    dd->pri->render = PLOT_CURVE;};};

    plt = pd->pl_type;

    pd->legend_palette_fl = FALSE;
    pd->legend_contour_fl = FALSE;

/* setup the viewport */
    vwprt = pd->viewport;
    if (vwprt != NULL)
       {PG_box_copy(2, ndc, vwprt);
        bare = FALSE;}
    else
       {PG_box_init(2, ndc, 0.175, 0.825);
        ndc[1] = (plt == PLOT_POLAR) ? 0.825 : 0.925;
        bare   = dev->print_labels;};

    if (change)
       PG_set_viewspace(dev, 2, NORMC, ndc);

    pd->legend_curve_fl = bare;

/* find the data extrema */
    PG_box_init(2, dbx, HUGE_REAL, -HUGE_REAL);

    datafl = PG_render_data_type(data);
    if (datafl)
       {for (g = data; g != NULL; g = g->next)
	    {for (pf = g->f; pf != NULL; pf = pf->next)
	         {domain = pf->domain;
		  range = pf->range;
		  if (range == NULL)
		     continue;

		  PM_array_real(domain->element_type, domain->extrema, 2, extr);

		  wc[0] = extr[0];
		  wc[1] = extr[1];

		  PM_array_real(range->element_type, range->extrema, 2, extr);

		  wc[2] = extr[0];
		  wc[3] = extr[1];

		  if ((wc[0] > HC) || (wc[1] > HC) ||
		      (wc[2] > HC) || (wc[3] > HC) ||
		      (wc[0] < -HC) || (wc[1] < -HC) ||
		      (wc[2] < -HC) || (wc[3] < -HC))
		     continue;

		  dbx[0] = min(dbx[0], wc[0]);
		  dbx[1] = max(dbx[1], wc[1]);
		  dbx[2] = min(dbx[2], wc[2]);
		  dbx[3] = max(dbx[3], wc[3]);};};

	domain = data->f->domain;
	range  = data->f->range;

	if (pty == PLOT_LOGICAL)
	   {dbx[0] = 1.0;
	    dbx[1] = 1.0;
	    for (g = data; g != NULL; g = g->next)
	        {for (pf = g->f; pf != NULL; pf = pf->next)
		     {n    = pf->domain->n_elements;
		      dbx[1] = max(dbx[1], n);};};};

	dextr = PM_get_limits(domain);
	rextr = PM_get_limits(range);}

    else
       {PG_image_picture_info(data, NULL, dbx, rbx);
	dextr = NULL;
	rextr = NULL;};

    if (dbx[0] >= dbx[1])
       {dbx[0] = 0.0;
        dbx[1] = 1.0;};

    if ((dbx[2] == dbx[3]) && (dbx[2] != 0.0))
       {dy = 0.005*(dbx[2] + dbx[3]);
        dbx[2] -= dy;
        dbx[3] += dy;}

    else if (dbx[2] >= dbx[3])
       {dbx[2] = 0.0;
        dbx[3] = 1.0;}

/* set the plot limits and set the range and domain controls */
/*    if ((dev->autorange == TRUE) || (rextr == NULL)) */
    if (rextr == NULL)
       {wc[2] = dbx[2];
        wc[3] = dbx[3];}
    else
       {wc[2] = rextr[0];
        wc[3] = rextr[1];};

    if ((dev->autodomain == TRUE) || (dextr == NULL) || (pty == PLOT_LOGICAL))
       {wc[0] = dbx[0];
	wc[1] = dbx[1];}
    else
       {wc[0] = dextr[0];
        wc[1] = dextr[1];};

    if (datafl && (plt == PLOT_POLAR))
       {PG_box_init(2, dbx, HUGE_REAL, -HUGE_REAL);
        for (g = data; g != NULL; g = g->next)
	    {for (pf = g->f; pf != NULL; pf = pf->next)
	         {domain = pf->domain;
		  range  = pf->range;

/* GOTCHA: we are not format converting here because it is not clear
 *         what the correct conversion strategy is wrt the conversion
 *         that must be done below. Do something later if necessary.
 */
		  tc = *((double **) domain->elements);
		  rc = *((double **) range->elements);
		  n  = domain->n_elements;
		  PG_polar_limits(dbx, tc, rc, n);};};

        wc[0] = dbx[0] - 0.0005*(dbx[1] - dbx[0]);
        wc[1] = dbx[1] + 0.0005*(dbx[1] - dbx[0]);
        wc[2] = dbx[2] - 0.0005*(dbx[3] - dbx[2]);
        wc[3] = dbx[3] + 0.0005*(dbx[3] - dbx[2]);};

    if (change)

/* setup the range limits */
       {extr[0] = wc[2];
	extr[1] = wc[3];
	PG_register_range_extrema(dev, 1, extr);

/* setup the window limits */
	avg = 0.5*(wc[0] + wc[1]);
	tol = (avg == 0.0) ? TOLERANCE : ABS(0.001*avg);
	if (wc[0] + tol > wc[1])
	   {wc[0] -= tol;
	    wc[1] += tol;};

	avg = 0.5*(wc[2] + wc[3]);
	tol = (avg == 0.0) ? TOLERANCE : ABS(0.001*avg);
	if (wc[2] + tol > wc[3])
	   {wc[2] -= tol;
	    wc[3] += tol;};

	PG_set_viewspace(dev, 2, WORLDC, wc);

	PG_fset_palette(dev, "standard");};

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_CURVE_PLOT - plot graphs with 1D domains and ranges */

void PG_curve_plot(PG_device *dev, PG_graph *data, ...)
   {int j, n;
    int color;
    double wc[PG_BOXSZ], dextr[2];
    double *x, *y, *ix;
    PM_set *domain, *range;
    PM_mapping *pf;
    PG_rendering pty, plt;
    PG_graph *g;
    PG_picture_desc *pd;
    pcons *info;

    if ((dev == NULL) || (data == NULL))
       return;

    data->rendering = PLOT_CURVE;

/* GOTCHA: this is incomplete or incorrect - the compiler
 *         complained (correctly) about not being initialized
 *         so now it is but probably not as intended
 *         the other modes are shut out by this
 */
    pty = data->rendering;

    pd = PG_setup_picture(dev, data, FALSE, TRUE, TRUE);

    PG_adorn_before(dev, pd, data);

    SC_VA_START(data);

/* plot all of the current curves */
    for (g = data; g != NULL; g = g->next)
        {for (pf = g->f; pf != NULL; pf = pf->next)
	     {domain = pf->domain;
	      range  = pf->range;

	      if ((pf != g->f) || (_PG_gattrs.overplot == TRUE))
                 {PM_array_real(domain->element_type, domain->extrema,
				2, dextr);

		  PG_get_viewspace(dev, WORLDC, wc);

		  wc[0] = max(wc[0], dextr[0]);
		  wc[1] = min(wc[1], dextr[1]);

		  PG_clear_region(dev, 2, WORLDC, wc, 0);};

	      n = domain->n_elements;
	      x = NULL;
	      y = PM_array_real(range->element_type,
				DEREF(range->elements), n, NULL);

	      PG_get_attrs_graph(g, FALSE,
				 "LINE-COLOR", G_INT_I, &color, dev->BLUE,
				 "PLOT-TYPE",  G_INT_I, &plt,   PLOT_CARTESIAN,
				 NULL);

	      info = PG_fget_render_info(g);
	      if (pty == PLOT_LOGICAL)
		 {ix = CMAKE_N(double, n);
		  for (j = 1; j <= n; j++)
		      ix[j-1] = (double) j;

		  PG_plot_curve(dev, ix, y, n, info, 1);

		  if ((dev->data_id == TRUE) && (plt != PLOT_INSEL))
		     {PG_fset_text_color(dev, color, TRUE);
		      PG_draw_data_ids(dev, ix, y,
				       (int) domain->n_elements,
				       g->identifier, info);
		      PG_fset_text_color(dev, dev->WHITE, TRUE);};

		  CFREE(ix);}

	      else
		 {x = PM_array_real(domain->element_type,
				    DEREF(domain->elements), n, NULL);

		  PG_plot_curve(dev, x, y, n, info, 1);

		  if ((dev->data_id == TRUE) && (plt != PLOT_INSEL))
		     {PG_fset_text_color(dev, color, TRUE);
		      PG_draw_data_ids(dev, x, y,
				       (int) domain->n_elements,
				       g->identifier, info);
		      PG_fset_text_color(dev, dev->WHITE, TRUE);};

		  CFREE(x);};

	      CFREE(y);

	      PG_draw_domain_boundary(dev, pf);};};

    SC_VA_END;

    pd->legend_curve_fl &= dev->print_labels;

    PG_finish_picture(dev, data, pd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_INTERP_POINT - interpolate to point at window boundary
 *                  - take into account possible log scales
 */

static void _PG_interp_point(PG_device *dev, double *yp, double x,
                             double *xa, double *xb)
   {double p[PG_SPACEDM];

#if 0
    p[0] = x;
    p[1] = 0.0;
    
    if (g->iflog[0])
       {xa[0] = log10(ABS(xa[0]) + SMALL);
        xb[0] = log10(ABS(xb[0]) + SMALL);
        p[0]  = log10(ABS(p[0])  + SMALL);};

    if (g->iflog[1])
       {PM_interp(p[1], p[0], xa[0], xa[1], xb[0], xb[1]);
        if (p[1] <= 0.0)
           {*yp = -HUGE;
            return;};

        xa[1] = log10(ABS(xa[1]) + SMALL);
        xb[1] = log10(ABS(xb[1]) + SMALL);};
#else
    p[0] = x;
    p[1] = 1.0;
    
    PG_log_point(dev, 2, xa);
    PG_log_point(dev, 2, xb);
    PG_log_point(dev, 2, p);
#endif

    PG_trans_point(dev, 2, WORLDC, xa, NORMC, xa);
    PG_trans_point(dev, 2, WORLDC, xb, NORMC, xb);
    PG_trans_point(dev, 2, WORLDC, p, NORMC, p);

    PM_interp(p[1], p[0], xa[0], xa[1], xb[0], xb[1]);

    PG_trans_point(dev, 2, NORMC, p, WORLDC, p);

    PG_lin_point(dev, 2, p);

/* coerce y values to be within the window (current domain/range)
 * if p[1] is near an axis (near zero) and flips sign (+/-) then
 * do not allow the sign flip or the data-id will not be drawn for the curve
 */
    if (((p[1] < 0.0) && (xa[1] > 0.0) && (xa[1] < 0.1*SMALL)) ||
	((p[1] > 0.0) && (xa[1] < 0.0) && (xa[1] > 0.1*SMALL)))
       {if (p[1] < 0.0) 
           p[1] = -1.0 * SMALL;
        else
           p[1] = SMALL;};
          
    *yp = p[1];

    return;}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* PG_DRAW_DATA_IDS_ALT - draw the curve data ids */
 
static void PG_draw_data_ids_alt(PG_device *dev, double *x, double *y,
				 int n, int label, pcons *info)
   {char mark[2];
    int i, j, k, j0, m, scatter;
    int imn, imx;
    int *ppt, *psc;
    double xp[PG_SPACEDM];
    double xa[0], xb[0], xa[1], xb[1], s1, s2;
    double dx, dy, dt;
    double s, ds, r, wc[PG_BOXSZ];
    double *ln;
    PG_rendering pty;

    PG_fset_clipping(dev, TRUE);
    PG_get_viewspace(dev, WORLDC, wc);
 
    mark[0] = label;
    mark[1] = '\0';

    m = 6;
    r = 0.5*(1.0 + PM_random(1));
    k = n/m;

    if (g->iflog[0])
       dx = POW(10.0, (log10(wc[1]/wc[0]))/((double) m));
    else
       dx = (wc[1] - wc[0])/((double) m);

    PG_get_attrs_alist(info,
		       "SCATTER",   G_INT_I, &scatter, FALSE,
		       "PLOT-TYPE", G_INT_I, &pty,     PLOT_CARTESIAN,
		       NULL);

    if (!scatter && ((pty == PLOT_CARTESIAN) || (pty == PLOT_HISTOGRAM)))
       {ln = CMAKE_N(double, n);

/* NOTE: the following assume that the arrays are sorted */
        for (imn = 0; imn < n; imn++)
	    {if ((wc[0] <= x[imn]) && (wc[2] <= y[imn]))
                break;};

        for (imx = n-1; imx >= 0; imx--)
	    {if ((x[imx] <= wc[1]) && (y[imx] <= wc[3]))
                break;};

/* find the lengths of each segment */
        xa[0] = x[0];
        xa[1] = y[0];
        for (i = 1; i < n; i++)
            {xb[0] = x[i];
             xb[1] = y[i];
	     ln[i] = PM_hypot(xb[0] - xa[0], xb[1] - xa[1]);
	     xa[0] = xb[0];
             xa[1] = xb[1];};

/* integrate to find the total curve length */
        for (i = 1, s = 0.0; i < n; i++)
            {s += ln[i];
             ln[i] = s;};

/* normalize to find the fractional curve length */
        for (i = 1, s = 1.0/ln[n-1]; i < n; i++)
            ln[i] *= s;

	if ((g->iflog[0]) || (g->iflog[1]))
	   {dx = wc[1] - wc[0];
	    dy = wc[3] - wc[2];
	    dt = max(dx, dy);
	    ds = log(dt);
	    ds = ABS(ds);
	    ds = min(ds, _PG_gattrs.axis_n_decades);
	    s  = ln[imx]*POW(ds, -((double) m));}
	else
	   {if (imn == imx)
               {if (imn == 0)
                   imx++;
                else
		   imn--;};

	    ds = (ln[imx] - ln[imn])/((double) m);
	    s  = ln[imn] + r*ds;};

	for (i = 0; i < m; i++)
	    {j = PM_find_index(ln, s, n);

             if ((imn <= j) && (j <= imx))
                {if (pty == PLOT_HISTOGRAM || scatter)
                    {xp[0] = x[j-1];
                     xp[1] = y[j-1];}
                 else
                    {xa[0] = x[j-1];
                     xa[1] = y[j-1];
                     xb[0] = x[j];
                     xb[1] = y[j];
                     s1 = ln[j-1];
                     s2 = ln[j];

                     if (xa[0] < wc[0])
                        {PM_interp(xa[1], wc[0], xa[0], xa[1], xb[0], xb[1]);
                         xa[0] = wc[0];}
                     if (xb[0] > wc[1])
                        {PM_interp(xb[1], wc[1], xa[0], xa[1], xb[0], xb[1]);
                         xb[0] = wc[1];}

		     xp[0] = xa[0] + (xb[0] - xa[0])*(s - s1)/(s2 - s1 + SMALL);
		     if ((g->iflog[0]) || (g->iflog[1]))
			s *= ds;
                     else
		        s += ds;

                     _PG_interp_point(dev, &xp[1], xp[0], xa, xb);};

                 if (PG_box_contains(2, wc, xp) == TRUE)
                    PG_write_n(dev, 2, WORLDC, xp, mark);};};

	CFREE(ln);}

    else if (pty == PLOT_POLAR)
       {double tc, rc, xc, yc;

	j0 = ((double) k)*r;
	j0 = abs(j0);

        for (i = 0; i < m; i++)
            {j  = j0 + k*i;
             tc = x[j];
             rc = y[j];
             xp[0] = rc*cos(tc);
             xp[1] = rc*sin(tc);
	     if (PG_box_contains(2, wc, xp) == TRUE)
	        PG_write_n(dev, 2, WORLDC, xp, mark);};};
 
    PG_fset_clipping(dev, FALSE);
 
    return;}
 
/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* PG_DRAW_DATA_IDS - draw the curve data ids */
 
void PG_draw_data_ids(PG_device *dev, double *x, double *y,
		      int n, int label, pcons *info)
   {int i, j, k, ja, m, scatter, irev;
    char mark[10];
    double r, dx, x0;
    double xa[PG_SPACEDM], xb[PG_SPACEDM];
    double wc[PG_BOXSZ], box[PG_BOXSZ], xp[PG_SPACEDM];
    double **xrv;
    PG_rendering pty;
    PG_dev_geometry *g;

    if (dev == NULL)
       return;

    g = &dev->g;

    irev = FALSE;

/* reverse the curve if it is "backwards" */
    if (x[0] > x[n-1])
       {xrv = PM_make_vectors(2, n);
        
        for (i = 0; i < n; i++)
            {xrv[0][i] = x[n-i-1];
             xrv[1][i] = y[n-i-1];}

        irev = TRUE;
        x = xrv[0];
        y = xrv[1];}

    PG_fset_clipping(dev, TRUE);
    PG_get_viewspace(dev, WORLDC, wc);

    if (wc[0] > wc[1])
       {SC_SWAP_VALUE(double, wc[0], wc[1]);};
    if (wc[2] > wc[3])
       {SC_SWAP_VALUE(double, wc[2], wc[3]);};

    PM_maxmin(x, &box[0], &box[1], n);
    PM_maxmin(y, &box[2], &box[3], n);

/* if the data is completely outside the viewport quit now */
    if ((box[1] < wc[0]) || (wc[1] < box[0]))
       {if (irev)
	   PM_free_vectors(2, xrv);
	return;};

    if ((box[3] < wc[2]) || (wc[3] < box[2]))
       {if (irev)
	   PM_free_vectors(2, xrv);
	return;};

/* find the overlap of the data with the viewport */
    box[0] = max(wc[0], box[0]);
    box[2] = max(wc[2], box[2]);
    box[1] = min(wc[1], box[1]);
    box[3] = min(wc[3], box[3]);
    if (box[2] == box[3])
       {box[2] -= 1.0;
	box[3] += 1.0;};

    if (_PG_gattrs.numeric_data_id == TRUE)
       snprintf(mark, 10, "%d", label);
    else if (label > 'Z')
       snprintf(mark, 10, "%d", label - 'A' + 1);
    else
       {mark[0] = label;
	mark[1] = '\0';};

    m = 6;
    r = 0.5*(1.0 + PM_random(1));

    if (g->iflog[0])
       {x0  = box[1]/(POW(10.0, _PG_gattrs.axis_n_decades));
        box[0] = max(box[0], x0);
	dx  = POW(10.0, (log10(box[1]/box[0]))/((double) m));
        x0  = box[0]*POW(dx, r);}
    else
       {dx = (box[1] - box[0])/((double) m);
        x0 = box[0] + r*dx;}

    PG_get_attrs_alist(info,
		       "SCATTER",   G_INT_I, &scatter, FALSE,
		       "PLOT-TYPE", G_INT_I, &pty,     PLOT_CARTESIAN,
		       NULL);

    if ((pty == PLOT_CARTESIAN) || (pty == PLOT_SCATTER) ||
	(pty == PLOT_HISTOGRAM))
       {for (i = 0; i < m; i++)
            {if (g->iflog[0])
                xp[0] = x0*POW(dx, i);
             else
                xp[0] = x0 + i*dx;

             j = PM_find_index(x, xp[0], n);
             if ((0 < j) && (j < n))
                {if (pty == PLOT_HISTOGRAM || scatter)
                    {xp[0] = x[j-1];
                     xp[1] = y[j-1];}
                 else
                    {xa[0] = x[j-1];
                     xa[1] = y[j-1];
                     xb[0] = x[j];
                     xb[1] = y[j];

                     if (xa[0] < box[0])
                        {PM_interp(xa[1], box[0], xa[0], xa[1], xb[0], xb[1]);
                         xa[0] = box[0];}
                     if (xb[0] > box[1])
                        {PM_interp(xb[1], box[1], xa[0], xa[1], xb[0], xb[1]);
                         xb[0] = box[1];}

/* skip over points outside the box in the y direction
 * necessary for log space
 */
		     if (((xa[1] < box[2]) && (xb[1] < box[2])) ||
			 ((xa[1] > box[3]) && (xb[1] > box[3])))
		        continue;

                     _PG_interp_point(dev, &xp[1], xp[0], xa, xb);};

                 if (PG_box_contains(2, box, xp) == TRUE)
                    PG_write_n(dev, 2, WORLDC, xp, mark);};};}

    else if (pty == PLOT_POLAR)
       {double tc, rc, xc[PG_SPACEDM];

	k  = n/m;
	ja = ((double) k)*r;
	ja = abs(ja);

        for (i = 0; i < m; i++)
            {j  = ja + k*i;
             tc = x[j];
             rc = y[j];
             xc[0] = rc*cos(tc);
             xc[1] = rc*sin(tc);
             if (PG_box_contains(2, box, xc) == TRUE)
                PG_write_n(dev, 2, WORLDC, xc, mark);};};
 
    PG_fset_clipping(dev, FALSE);

    if (irev)
       PM_free_vectors(2, xrv);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_ERROR_PLOT - user interface to draw a scatter plot with error bars */

static void PG_error_plot(PG_device *dev, double **x, int n, int lncol,
                          double lnwid, int lnsty, int scatter, int marker,
                          int l, pcons *info)
   {int i, tn;
    double *px, *py;
    double x0[PG_SPACEDM], x1[PG_SPACEDM], x2[PG_SPACEDM], dx[PG_SPACEDM];
    double wc[PG_BOXSZ];
    double *r[PG_SPACEDM];
    double *xp[PG_SPACEDM], *xm[PG_SPACEDM], **t;

    if (dev == NULL)
       return;

    PG_get_attrs_alist(info,
		       "DY-PLUS",  G_POINTER_I, &xp[1], NULL,
		       "DY-MINUS", G_POINTER_I, &xm[1], NULL,
		       "DX-PLUS",  G_POINTER_I, &xp[0], NULL,
		       "DX-MINUS", G_POINTER_I, &xm[0], NULL,
		       NULL);
    r[0] = x[0];
    r[1] = x[1];

    if (l == 0)
       {PG_set_limits_n(dev, 2, WORLDC, n, r, PLOT_CARTESIAN);
        PG_axis(dev, CARTESIAN_2D);};
 
    PG_fset_clipping(dev, TRUE);
    PG_fset_line_color(dev, lncol, TRUE);
    PG_fset_line_width(dev, lnwid);
    PG_fset_line_style(dev, lnsty);
 
    tn = 12*n*(((xp[0] != NULL) || (xm[0] != NULL)) +
               ((xp[1] != NULL) || (xm[1] != NULL)));

    t = PM_make_vectors(2, tn);

    if (xm[0] == NULL)
       xm[0] = xp[0];

    if (xm[1] == NULL)
       xm[1] = xp[1];

    PG_get_viewspace(dev, WORLDC, wc);
    dx[0] = _PG_gattrs.error_cap_size*(wc[1] - wc[0]);
    dx[1] = _PG_gattrs.error_cap_size*(wc[3] - wc[2]);

    px = t[0];
    py = t[1];
    for (i = 0, tn = 0; i < n; i++)
        {x0[0] = x[0][i];
         x0[1] = x[1][i];
         if (xp[0] != NULL)
	    {x1[0] = x0[0] - xm[0][i];
	     x2[0] = x0[0] + xp[0][i];

	     *px++ = x1[0];
	     *py++ = x0[1];
	     *px++ = x2[0];
	     *py++ = x0[1];
             tn++;
             if (_PG_gattrs.error_cap_size > 0.0)
                {x1[1] = x0[1] + dx[1];
                 x2[1] = x0[1] - dx[1];

		 *px++ = x1[0];
		 *py++ = x1[1];
		 *px++ = x1[0];
		 *py++ = x2[1];
		 tn++;

		 *px++ = x2[0];
		 *py++ = x1[1];
		 *px++ = x2[0];
		 *py++ = x2[1];
		 tn++;};};

	 if (xp[1] != NULL)
	    {x1[1] = x0[1] - xm[1][i];
	     x2[1] = x0[1] + xp[1][i];

	     *px++ = x0[0];
	     *py++ = x1[1];
	     *px++ = x0[0];
	     *py++ = x2[1];
             tn++;
             if (_PG_gattrs.error_cap_size > 0.0)
                {x1[0] = x0[0] + dx[0];
                 x2[0] = x0[0] - dx[0];

		 *px++ = x1[0];
		 *py++ = x1[1];
		 *px++ = x2[0];
		 *py++ = x1[1];
		 tn++;

		 *px++ = x1[0];
		 *py++ = x2[1];
		 *px++ = x2[0];
		 *py++ = x2[1];
		 tn++;};};};

    PG_draw_disjoint_polyline_n(dev, 2, WORLDC, tn, t, FALSE);
 
    PM_free_vectors(2, t);

    PG_fset_clipping(dev, FALSE);
    PG_fset_line_width(dev, 0.0);
    PG_fset_line_style(dev, LINE_SOLID);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_RECT_PLOT - make a Cartesian plot */

void PG_rect_plot(PG_device *dev, double *x, double *y, int n, int lncol,
                  double lnwid, int lnsty, int scatter, int marker, int l)
   {int tn, clp, st;
    double wd, ymn;
    double *r[PG_SPACEDM];
    double **t;
    
    if (dev == NULL)
       return;

    if (POSTSCRIPT_DEVICE(dev))
       io_printf(dev->file, "%s", "\n%\n% Begin CURVE\n%\n");
 
    r[0] = x;
    r[1] = y;

    if (l == 0)
       {PG_set_limits_n(dev, 2, WORLDC, n, r, PLOT_CARTESIAN);
        PG_axis(dev, CARTESIAN_2D);};
 
/* worst case scenario is every line between adjacent points
 * intersects domain/range boundary rectangle twice.
 */
    tn = 2*n + 3;
    t  = PM_make_vectors(2, tn);

    PG_clip_data(dev, t[0], t[1], &tn, x, y, n);

/* GOTCHA: should set clipping flag here */

/* fill the area under the curve - or a closed curve */
    if (dev->fill_color != dev->BLACK)
       {int np;

	np = tn;

/* if not closed - close it */
	if ((t[0][0] != t[0][n-1]) || (t[1][0] != t[1][n-1]))
	   {if (dev->g.iflog[1] == FALSE)
	       ymn = 0.0;
	    else
	       ymn = dev->g.wc[3]*POW(10.0, -_PG_gattrs.axis_n_decades);

	    t[0][np] = t[0][np-1];
	    t[1][np] = ymn;
	    np++;

	    t[0][np] = t[0][0];
	    t[1][np] = ymn;
	    np++;

	    t[0][np] = t[0][0];
	    t[1][np] = t[1][0];
	    np++;};

       PG_fill_polygon_n(dev, dev->fill_color, TRUE, 2, WORLDC, np, t);};
	
/* GOTCHA: should set clipping above but
 * this works around a bug in the polygon intersection code that
 * ruins memory map plots - see ultra/applications/memmap.scm and
 * the test_2.map from score/scmots.c
 */
    clp = PG_fget_clipping(dev);
    PG_fset_clipping(dev, TRUE);
 
    wd = PG_fget_line_width(dev);
    st = PG_fget_line_style(dev);
 
    PG_fset_line_color(dev, lncol, TRUE);
    PG_fset_line_width(dev, lnwid);
    PG_fset_line_style(dev, lnsty);

/* plot lines unless scatter set */
    if (tn > 0)
       {if (scatter != 1)
	   PG_draw_polyline_n(dev, 2, WORLDC, tn, t, FALSE);
        else
	   PG_draw_markers_n(dev, 2, WORLDC, tn, t, marker);};
 
    PM_free_vectors(2, t);

    PG_fset_clipping(dev, clp);
    PG_fset_line_width(dev, wd);
    PG_fset_line_style(dev, st);
 
    if (POSTSCRIPT_DEVICE(dev))
       io_printf(dev->file, "%s", "\n%\n% End CURVE\n%\n");
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_HISTOGRAM_PLOT - make a Cartesian histogram plot */

void PG_histogram_plot(PG_device *dev, double *x, double *y, int n, int lncol,
                       double lnwid, int lnsty, int scatter, int marker,
                       PM_direction start, int l)
   {int i, j, nl, nm, ocl, offs;
    double ymn;
    double *r[PG_SPACEDM];
    double **xl, *py;

    if (dev == NULL)
       return;

    if (POSTSCRIPT_DEVICE(dev))
       io_printf(dev->file, "%s", "\n%\n% Begin CURVE\n%\n");
 
    r[0] = x;
    r[1] = y;

    if (l == 0)
       {PG_set_limits_n(dev, 2, WORLDC, n, r, PLOT_CARTESIAN);
        PG_axis(dev, CARTESIAN_2D);};
 
    PG_fset_clipping(dev, TRUE);
    PG_fset_line_color(dev, lncol, TRUE);
    PG_fset_line_width(dev, lnwid);
    PG_fset_line_style(dev, lnsty);
 
/* transform data to histogram form */
    nm = n - 1;
    nl = 2*n;
    xl = PM_make_vectors(2, nl);
	
    ymn = dev->g.wc[2];

/* start with beginning curve point */
    xl[0][0] = x[0];
    xl[1][0] = ymn;

    offs = 0;
    switch (start)
       {case DIR_RIGHT :
             offs++;
	case DIR_LEFT :
	     py = y + offs;

	     xl[0][1] = x[0];
	     xl[1][1] = py[0];
	     for (i = 1, j = 2; i < nm; i++, j += 2)
	         {xl[0][j] = x[i];
		  xl[1][j] = xl[1][j-1];

		  xl[0][j+1] = x[i];
		  xl[1][j+1] = py[i];};
	     xl[0][j] = x[i];
	     xl[1][j] = py[i-1];
	     break;

	case DIR_CENTER :
	     py = y + 1;

	     xl[0][1] = x[0];
	     xl[1][1] = 0.5*(y[0] + y[1]);
	     for (i = 1, j = 2; i < nm; i++, j += 2)
	         {xl[0][j] = x[i];
		  xl[1][j] = 0.5*(y[i-1] + y[i]);

		  xl[0][j+1] = x[i];
		  xl[1][j+1] = 0.5*(y[i] + y[i+1]);};
	     xl[0][j] = x[i];
	     xl[1][j] = 0.5*(y[i-1] + y[i]);
	     break;

	default :
             return;
	     break;};

/* finish with ending curve point */
    xl[0][j+1] = x[i];
    xl[1][j+1] = ymn;

    ocl = dev->clipping;
    dev->clipping = FALSE;

    PG_rect_plot(dev, xl[0], xl[1], nl, lncol,
                 lnwid, lnsty, scatter, marker, l);

    dev->clipping = ocl;

    PM_free_vectors(2, xl);

    PG_fset_clipping(dev, FALSE);
    PG_fset_line_width(dev, 0.0);
    PG_fset_line_style(dev, LINE_SOLID);
 
    if (POSTSCRIPT_DEVICE(dev))
       io_printf(dev->file, "%s", "\n%\n% End CURVE\n%\n");
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_INSEL_PLOT - make an Inselberg plot */
 
void PG_insel_plot(PG_device *dev, double *x, double *y, int n, int lncol,
                   double lnwid, int lnsty, int l)
   {int i, ndim;
    double tn, o, xsep;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    double xc[PG_SPACEDM], wc[PG_BOXSZ], bnd[PG_BOXSZ];
    double xext, yext, wxext;
    double *r[PG_SPACEDM];

    if (dev == NULL)
       return;

    if (POSTSCRIPT_DEVICE(dev))
       io_printf(dev->file, "%s", "\n%\n% Begin CURVE\n%\n");
 
    r[0] = x;
    r[1] = y;

   if (l == 0)
       {PG_set_limits_n(dev, 2, WORLDC, n, r, PLOT_CARTESIAN);
        PG_axis(dev, INSEL);};
 
    PG_fset_clipping(dev, TRUE);
    PG_fset_line_color(dev, lncol, TRUE);
    PG_fset_line_width(dev, lnwid);
    PG_fset_line_style(dev, lnsty);
 
/* get window and viewport limits */
    PG_get_viewspace(dev, WORLDC, wc);
    PG_get_viewspace(dev, BOUNDC, bnd);

/* scale the x axis so the "domain" is compressed and centered */
    wxext = wc[1] - wc[0];
    xext  = bnd[1] - bnd[0];
    yext  = bnd[3] - bnd[2];

    tn    = yext/xext;
    o     = bnd[2] - tn*bnd[0];
    ndim  = 2;
    xsep  = wxext/((double) (++ndim));
    x1[0] = wc[0] + xsep;
    x2[0] = wc[0] + 2.0*xsep;
    for (i = 0; i < n; i++)
        {xc[0] = x[i];
         xc[1] = y[i];
         if (PG_box_contains(2, wc, xc) == TRUE)
            {x1[1] = tn*xc[0] + o;
	     x2[1] = xc[1];
             PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);};};
 
    PG_fset_clipping(dev, FALSE);
    PG_fset_line_width(dev, 0.0);
    PG_fset_line_style(dev, LINE_SOLID);
 
    if (POSTSCRIPT_DEVICE(dev))
       io_printf(dev->file, "%s", "\n%\n% End CURVE\n%\n");
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_POLAR_PLOT - make a polar plot */
 
void PG_polar_plot(PG_device *dev, double *x, double *y, int n, int lncol,
                   double lnwid, int lnsty, int scatter, int marker, int l)
   {int i;
    double rc, tc;
    double xc[PG_SPACEDM], xco[PG_SPACEDM];
    double wc[PG_BOXSZ], bnd[PG_BOXSZ];
    double *r[PG_SPACEDM];

    if (dev == NULL)
       return;

    if (POSTSCRIPT_DEVICE(dev))
       io_printf(dev->file, "%s", "\n%\n% Begin CURVE\n%\n");
 
    r[0] = x;
    r[1] = y;

    if (l == 0)
       {PG_set_limits_n(dev, 2, WORLDC, n, r, PLOT_CARTESIAN);
        PG_axis(dev, POLAR);};
 
    PG_fset_clipping(dev, TRUE);
    PG_fset_line_color(dev, lncol, TRUE);
    PG_fset_line_width(dev, lnwid);
    PG_fset_line_style(dev, lnsty);
 
/* get window and viewport limits */
    PG_get_viewspace(dev, WORLDC, wc);
    PG_get_viewspace(dev, BOUNDC, bnd);

/* scale the x axis so the "domain" is compressed and centered */
    if (scatter)
       {double **xd;
       int nm;

        xd = PM_make_vectors(2, n);
        nm = 0;
        for (i = 1; i < n; i++)
            {tc = x[i];
             rc = y[i];
             xc[0] = rc*cos(tc);
             xc[1] = rc*sin(tc);
	     if (PG_box_contains(2, wc, xc) == TRUE)
                {xd[0][nm] = xc[0];
                 xd[1][nm] = xc[1];
                 nm++;};};
        PG_draw_markers_n(dev, 2, WORLDC, nm, xd, marker);
	PM_free_vectors(2, xd);}

    else
       {tc = x[0];
        rc = y[0];

        xco[0] = rc*cos(tc);
        xco[1] = rc*sin(tc);
        for (i = 1; i < n; i++)
            {tc = x[i];
             rc = y[i];
             xc[0] = rc*cos(tc);
             xc[1] = rc*sin(tc);
	     if (PG_box_contains(2, wc, xc) == TRUE)
                PG_draw_line_n(dev, 2, WORLDC, xco, xc, dev->clipping);
             xco[0] = xc[0];
             xco[1] = xc[1];};};
 
    PG_fset_clipping(dev, FALSE);
    PG_fset_line_width(dev, 0.0);
    PG_fset_line_style(dev, LINE_SOLID);
 
    if (POSTSCRIPT_DEVICE(dev))
       io_printf(dev->file, "%s", "\n%\n% End CURVE\n%\n");
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PLOT_CURVE - dispatch to the desired curve plotting routine
 *
 * #bind PG_plot_curve fortran() scheme() python()
 */

void PG_plot_curve(PG_device *dev ARG(,,cls),
		   double *x, double *y, int n, pcons *info, int l)
   {int lncol, lnsty, scatter, marker, start, hiddn;
    int ofc, fcol;
    double lnwid;
    double *r[PG_SPACEDM];
    PG_rendering pty;

    r[0] = x;
    r[1] = y;

    PG_get_attrs_alist(info,
		       "PLOT-TYPE",    G_INT_I, &pty,     PLOT_CARTESIAN,
		       "HIST-START",   G_INT_I, &start,   _PG_gattrs.hist_start,
		       "MARKER-INDEX", G_INT_I, &marker,  0,
		       "FILL",         G_INT_I, &fcol,    -100,
		       "SCATTER",      G_INT_I, &scatter, _PG_gattrs.scatter_plot,
		       "HIDDEN",       G_INT_I, &hiddn,   FALSE,
		       "LINE-COLOR",   G_INT_I, &lncol,   dev->BLUE,
		       "LINE-STYLE",   G_INT_I, &lnsty,   LINE_SOLID,
		       "LINE-WIDTH",   G_DOUBLE_I,  &lnwid,   0.0,
		       NULL);

    ofc = dev->fill_color;
    if (fcol == -100)
       dev->fill_color = dev->BLACK;
    else
       dev->fill_color = fcol;

    if (!hiddn)
       switch (pty)
	  {case PLOT_INSEL :
	        PG_insel_plot(dev, x, y, n,
			      lncol, lnwid, lnsty, l);
		break;

	   case PLOT_POLAR :
	        PG_polar_plot(dev, x, y, n, lncol, lnwid, lnsty,
			      scatter, marker, l);
		break;

	   case PLOT_HISTOGRAM :
	        PG_histogram_plot(dev, x, y, n, lncol, lnwid, lnsty,
				  scatter, marker, start, l);
		break;

	   case PLOT_SCATTER   :
           case PLOT_CARTESIAN :
                PG_rect_plot(dev, x, y, n, lncol, lnwid, lnsty,
			     scatter, marker, l);
		break;

	   case PLOT_ERROR_BAR :
                PG_error_plot(dev, r, n, lncol, lnwid, lnsty,
			      scatter, marker, l, info);
		break;

	   default :
	        break;};

    dev->fill_color = ofc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

