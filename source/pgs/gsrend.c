/*
 * GSREND.C - high level rendering control for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

/* Rendering Procedure:
 *
 *  1) Render a linked list of mappings with a single rendering serially
 *     (uses PG_draw_graph)
 *
 *        PG_make_device                                (allocate the device)
 *        PG_open_device                        (open the device for drawing)
 *        PG_setup_picture                          (setup viewport, WC, etc)
 *        loop over mappings
 *           render individual mapping           (control pixels in viewport)
 *        PG_finish_picture         (dress with axes, labels, etc, and flush)
 *
 *  2) Render a linked list of mappings with a individual renderings serially
 *     (uses PG_draw_picture)
 *
 *        PG_make_device                                (allocate the device)
 *        PG_open_device                        (open the device for drawing)
 *        PG_setup_picture                          (setup viewport, WC, etc)
 *           (set device flag to supress setup and finish in subsequent
 *            calls to PG_draw_graph)
 *        loop over mappings
 *           PG_draw_graph                       (control pixels in viewport)
 *        PG_finish_picture         (dress with axes, labels, etc, and flush)
 *           (reset device flag supressing setup and finish)
 *
 *  3) Render a linked list of mappings with a single rendering in parallel
 *     (uses PG_draw_graph)
 *
 *        PG_make_device                                (allocate the device)
 *           (select a node to open the requested drawing device
 *            all nodes open an IMAGE device and the selected
 *            node opens the requested device)
 *        PG_open_device                        (open the device for drawing)
 *           (all nodes open IMAGE device and selected node
 *            also opens requested device)
 *        PG_setup_picture                          (setup viewport, WC, etc)
 *           (call to PG_parallel_setup to communicate common
 *            viewport, WC, and extrema values in order to limit each
 *            node to settting only those pixels of the final
 *            picture for which it has data)
 *        loop over mappings
 *           render individual mapping to IMAGE device
 *        PG_finish_picture         (dress with axes, labels, etc, and flush)
 *           (after usual finish_picture all IMAGE devices
 *            transmit their raster to the selected node
 *            (_PG_transmit_images)
 *            which assembles them into a single raster image
 *            (_PG_display_image)
 *            and displays the raster image to the requested device
 *            with the rendering decorations of the selected type
 *            (PG_render_parallel)
 *
 *  4) Render a linked list of mappings with individual renderings in parallel
 *     (uses PG_draw_picture)
 *
 *     analogous to the relationship between (2) and (1) with special
 *     attention paid to the setup supression and parallel rendering issues
 *
 */

static double
 HC = 0.0;

/*--------------------------------------------------------------------------*/

/*                       HIGHEST LEVEL RENDERING CONTROL                    */

/*--------------------------------------------------------------------------*/

/* _PG_FIND_SET_EXTREMA - loop over all sets to find the extrema */

static int _PG_find_set_extrema(PM_set *set, double *sdet, double *sdex,
				int first)
   {int i, ia, ib, n, nde, rv;
    double mn, mx;
    double *lmt;
    pcons *alst;
    PM_set *p;

    rv = TRUE;
    for (p = set; p != NULL; p = p->next)
        {nde = p->dimension_elem;
	 PM_array_real(p->element_type, p->extrema, 2*nde, sdet);

	 PG_get_render_info(p, alst);

	 PG_get_attrs_alist(alst,
			    "LIMITS", SC_POINTER_I, &lmt, NULL,
			    NULL);
	 i = 0;
	 if (lmt != NULL)

/* take limits from the first mapping only */
	    {if (first)
	        {n = 2*nde;
		 for (i = 0; i < n; i++)
		     sdex[i] = lmt[i];};}

	 else
	    {for (i = 0; i < nde; i++)
	         {ia = 2*i;
		  ib = ia + 1;
		  mn = sdex[ia];
		  mx = sdex[ib];
		  if ((mn > HC) || (mx > HC) ||
		      (mn < -HC) || (mx < -HC))
		     break;

		  sdex[ia] = min(mn, sdet[ia]);
		  sdex[ib] = max(mx, sdet[ib]);};};

/* weed out mappings presenting overflow problems */
	 rv &= (i < nde);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FIND_EXTREMA - find the extrema of the data and the requested
 *                 - plotting limits and return it all
 *                 -    STDOFF an additional stand off for the data extrema
 *                 -    DPEX   domain plotting extrema
 *                 -    RPEX   range plotting extrema
 *                 -    DDEX   domain data extrema
 *                 -    RDEX   range data extrema
 *                 -    NDE    dimension of domain elements
 *                 -    NRE    dimension of range elements
 */

void PG_find_extrema(PG_graph *data, double stdoff, double **pdpex, double **prpex,
		     int *pnde, double **pddex, int *pnre, double **prdex)
   {int i, ia, ib, ne, nde, nre, datafl, first;
    double dmn, dmx, rmn, rmx, rm, sc, mn, mx;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    double *ddex, *ddet, *rdex, *rdet;
    PM_set *domain, *range;
    PM_mapping *f;
    PG_graph *g;

    if (HC == 0.0)
       HC = HUGE_REAL*HUGE_REAL*HUGE_REAL;

    datafl = PG_render_data_type(data);

/* allocate space for the data extrema */
    if (datafl)
       {domain = data->f->domain;
	range  = data->f->range;
	nde    = (domain == NULL) ? 0 : domain->dimension_elem;
	nre    = (range == NULL) ? 0 : range->dimension_elem;}
    else
       {nde = 2;
	nre = 1;};

    ddex = CMAKE_N(double, 2*nde);
    PG_box_init(nde, ddex, HUGE_REAL, -HUGE_REAL);

    rdex = CMAKE_N(double, 2*nre);
    PG_box_init(nre, rdex, HUGE_REAL, -HUGE_REAL);

/* fill in the data extrema info */
    if (datafl)
       {ddet = CMAKE_N(double, 2*nde);
	rdet = CMAKE_N(double, 2*nre);

	for (g = data; g != NULL; g = g->next)
	    {for (f = g->f; f != NULL; f = f->next)
	         {first = (f == g->f);

		  domain = f->domain;
		  if (domain != NULL)
		     if (_PG_find_set_extrema(domain, ddet, ddex, first))
		        continue;

		  range = f->range;
		  if (range != NULL)
		     if (_PG_find_set_extrema(range, rdet, rdex, first))
		        continue;};};

	CFREE(ddet);
	CFREE(rdet);

	if (pdpex != NULL)
	   {domain = data->f->domain;
	    *pdpex = PM_get_limits(domain);};
	if (prpex != NULL)
	   {range  = data->f->range;
	    *prpex = PM_get_limits(range);};}

    else
       {PG_image_picture_info(data, NULL, dbx, rbx);
        for (i = 0; i < 4; i++)
	    ddex[i] = dbx[i];
	for (i = 0; i < 2; i++)
	    rdex[i] = rbx[i];

	if (pdpex != NULL)
	   *pdpex = NULL;
	if (prpex != NULL)
	   *prpex = NULL;};

/* regularize the domain data extrema */
    for (i = 0; i < nde; i++)
        {ia = 2*i;
	 ib = ia + 1;
	 mn = ddex[ia];
	 mx = ddex[ib];
	 if (mn > mx)
	    {ddex[ia] = 0.0;
	     ddex[ib] = 1.0;};};

/* regularize the range data extrema */
    for (i = 0; i < nre; i++)
        {ia = 2*i;
	 ib = ia + 1;
	 mn = rdex[ia];
	 mx = rdex[ib];
	 if (mn > mx)
	    {rdex[ia] = 0.0;
	     rdex[ib] = 1.0;};};

/* add the standoff (vector plots use this) */
    if (stdoff != 0.0)
       {ne = min(nde, nre);
	for (i = 0; i < ne; i++)
	    {ia = 2*i;
	     ib = ia + 1;
	     dmn = ddex[ia];
	     dmx = ddex[ib];
	     rmn = rdex[ia];
	     rmx = rdex[ib];

	     rm = max(ABS(rmx), ABS(rmn));
	     sc = (rm == 0.0) ? 0.0 : stdoff*(dmx - dmn)/rm;

	     ddex[ia] += (min(0.0, sc*rmn));
	     ddex[ib] += (max(0.0, sc*rmx));};};

/* return the data extrema info */
    if (pnde != NULL)
       *pnde  = nde;
    if (pnre != NULL)
       *pnre  = nre;

    if (pddex != NULL)
       *pddex = ddex;
    else
       CFREE(ddex);

    if (prdex != NULL)
       *prdex = rdex;
    else
       CFREE(rdex);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GET_PLACE - find the actual limits of LI specified
 *               - by PL which is in (OFF, N) form
 *               - return in LO
 */

static void _PG_get_place(double *lo, int *pl, double *li)
   {int i, n;
    double alm[2], ind, dn, d;

    if (pl == NULL)
       {alm[0] = li[0];
	alm[1] = li[1];
	ind = 0.0;}

    else
       {i = pl[0];
	n = pl[1];

	d   = li[1] - li[0];
	dn  = d/((double) n);
	ind = 0.02*dn;

	alm[0] = li[0] + i*dn;
	alm[1] = alm[0] + dn;};

    lo[0] = alm[0] + ind;
    lo[1] = alm[1] - ind;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_EST_RENDERING - estimate a rendering mode based on the
 *                  - given graph
 */

PG_rendering PG_est_rendering(PG_graph *data)
   {int domain_dim, range_dim;
    PG_rendering pty;

    domain_dim = data->f->domain->dimension_elem;
    range_dim  = data->f->range->dimension_elem;

    pty = PLOT_DEFAULT;

    switch (domain_dim)
       {case 1 :
	     pty = PLOT_CURVE;
	     break;

        case 2 :
	     switch (range_dim)
	        {default :
		 case 1  :
		      pty = PLOT_CONTOUR;
                      break;
		 case 2  :
		      pty = PLOT_VECTOR;
		      break;};
	     break;

        case 3 :
	     pty = PLOT_MESH;
	     break;};

    PG_get_attrs_graph(data, FALSE,
		       "RENDERING-TYPE", SC_INT_I, &pty, pty,
		       NULL);

    return(pty);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_RENDERING_PROPERTIES - return the rendering properties from the
 *                             - specified graph
 */

PG_picture_desc *PG_get_rendering_properties(PG_device *dev, PG_graph *data)
   {int i, datafl, meshfl;
    double *vwprt;
    char *labl, *ppal;
    PG_rendering pty;
    PG_picture_desc *pd;
    PG_par_rend_info *pri;
    PG_palette *plo;
    pcons *alst;

    if ((dev == NULL) || (data == NULL))
       return(NULL);

    datafl = PG_render_data_type(data);
    meshfl = (data->mesh | (_PG_gattrs.ref_mesh == TRUE));
    pty    = data->rendering;

    pd = CMAKE(PG_picture_desc);

    PG_get_render_info(data, alst);

    if (data->f != NULL)
       labl = (datafl) ? data->f->name : ((PG_image *) data->f)->label;
    else
       labl = NULL;

    PG_get_attrs_graph(data, TRUE,
		       "AXIS-TYPE",      SC_INT_I,     &pd->ax_type,    CARTESIAN_2D,
		       "PLOT-TYPE",      SC_INT_I,     &pd->pl_type,    _PG_gattrs.plot_type,
		       "THETA",          SC_DOUBLE_I,  &pd->va[0],      0.0,
		       "PHI",            SC_DOUBLE_I,  &pd->va[1],      0.0,
		       "CHI",            SC_DOUBLE_I,  &pd->va[2],      0.0,
		       "DRAW-AXIS",      SC_INT_I,     &pd->axis_fl,    TRUE,
		       "DRAW-MESH",      SC_INT_I,     &pd->mesh_fl,    meshfl,
		       "PALETTE",        SC_POINTER_I, &ppal,           NULL,
		       "CONTOUR-METHOD", SC_INT_I,     &pd->lev_method, TRUE,
		       "VIEW-PORT",      SC_POINTER_I, &vwprt,          NULL,
		       "DRAW-LABEL",     SC_INT_I,     &pd->legend_label_fl,   TRUE,
		       "DRAW-LEGEND",    SC_INT_I,     &pd->legend_contour_fl, TRUE,
		       "DRAW-PALETTE",   SC_INT_I,     &pd->legend_palette_fl, TRUE,
                       "LEGEND-PLACE",   SC_POINTER_I, &pd->legend_place,      NULL,
		       NULL);

    pd->data       = data;
    pd->alist      = alst;
    pd->viewport   = vwprt;
    pd->rendering  = pty;
    pd->n_levels   = 0;
    pd->levels     = NULL;

    pd->legend_label    = labl;
    pd->legend_curve_fl = FALSE;
    if (ppal != NULL)
       {plo = PG_get_palette(dev, NULL);
	PG_set_palette(dev, ppal);
	pd->legend_palette = PG_get_palette(dev, NULL);
        PG_make_palette_current(dev, plo);}
    else
       pd->legend_palette = PG_get_palette(dev, NULL);

    for (i = 0; i < 4; i++)
        {pd->view[i] = 0.0;
	 pd->wc[i]   = 0.0;};

    if ((pd->va[0] != HUGE) &&
	((pty == PLOT_SURFACE) || (pty == PLOT_WIRE_MESH)))
       pd->ax_type = CARTESIAN_3D;

    pri = dev->pri;
    if (pri != NULL)
       {pri->render = pty;
	if (pri->label == NULL)
	   pri->label = CSTRSAVE(pd->legend_label);};

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_LEGEND_PALETTE - draw the palette for the picture */

static void _PG_legend_palette(PG_device *dev,	PG_picture_desc *pd,
			       PG_graph *data)
   {int nd, nde, hvf, axty, datafl;
    int *lpl;
    double wd;
    double ndc[PG_BOXSZ], dbx[PG_BOXSZ];
    double *extr;
    PG_rendering pty;
    PG_palette *cpl, *opl;

    if ((dev == NULL) || (pd == NULL))
       return;

    axty   = pd->ax_type;
    cpl    = pd->legend_palette;
    lpl    = pd->legend_place;
    wd     = _PG_gattrs.palette_width;
    datafl = PG_render_data_type(data);

    PG_viewport_frame(dev, 2, ndc);

    if (lpl == NULL)
       extr = dev->range_extrema;
    else
       extr = (double *) data->f->range->extrema;

    opl = PG_get_palette(dev, NULL);

    if (cpl != NULL)
       {PG_make_palette_current(dev, cpl);};

    if (axty == CARTESIAN_3D)
       {pty = data->rendering;
	nd  = (datafl) ? data->f->range->dimension_elem : 1;
	if (pty == PLOT_SURFACE)
	   {dbx[0] = 0.808;
	    dbx[1] = 0.808;
	    if (nd == 1)
	       {_PG_get_place(dbx+2, lpl, ndc+2);
		PG_draw_palette_n(dev, dbx, extr, wd, FALSE);}
	    else
	       {_PG_get_place(dbx+2, lpl, ndc+2);
		PG_draw_palette_n(dev, dbx, extr+2, wd, FALSE);};};}

    else
       {hvf = _PG_gattrs.palette_orientation;
	if (hvf == VERTICAL)
	   {_PG_get_place(dbx+2, lpl, ndc+2);
	    ndc[1] += 0.25*(1.0 - ndc[1]);

	    dbx[0] = ndc[1];
	    dbx[1] = ndc[1];
	    PG_draw_palette_n(dev, dbx, extr, wd, TRUE);}

	else if (hvf == HORIZONTAL)
	   {nde = dev->range_n_extrema >> 1;
	    if ((cpl->pal_dims == NULL) || (nde < 2))
	       {_PG_get_place(dbx, lpl, ndc);
		ndc[2] *= 0.4;

		dbx[2] = ndc[2];
		dbx[3] = ndc[2];
		PG_draw_palette_n(dev, dbx, extr, wd, TRUE);}

	    else
	       {dbx[0] = 0.325;
	        dbx[1] = 0.06;
		dbx[2] = 0.07;
		dbx[3] = 0.175;
		PG_draw_2dpalette(dev, dbx, extr, wd);};};};

    PG_make_palette_current(dev, opl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_LEGEND_CONTOURS - draw the legend for the picture
 *                     - do it in NDC to avoid problems with log scales
 */

static void _PG_legend_contours(PG_device *dev, PG_picture_desc *pd,
				PG_graph *data)
   {int i, id, did, di, nl, datafl, tclr, clr, idt;
    int oflg[PG_SPACEDM], nflg[PG_SPACEDM];
    int *lpl;
    double ml, dt;
    double dxt[PG_SPACEDM], ndc[PG_BOXSZ];
    double xf[PG_SPACEDM], xw[PG_SPACEDM], dxw[PG_SPACEDM], p[PG_SPACEDM];
    double **r;
    double *lv;
    PG_palette *opl, *spl;

    if (dev == NULL)
       return;

    lpl    = pd->legend_place;
    datafl = PG_render_data_type(data);
    opl    = PG_get_palette(dev, NULL);

    PG_set_palette(dev, "standard");
    spl = PG_get_palette(dev, NULL);

    PG_set_text_color(dev, dev->WHITE);

    did = (datafl) ? pd->data->identifier : 'A';
    lv  = pd->levels;
    nl  = pd->n_levels;

    if (nl > 0)
       {PG_viewport_frame(dev, 2, ndc);

	_PG_get_place(ndc+2, lpl, ndc+2);

	PG_get_axis_log_scale(dev, 2, oflg);

	for (id = 0; id < 2; id++)
	    nflg[id] = FALSE;

	PG_set_axis_log_scale(dev, 2, nflg);

	PG_set_char_line(dev, 100);
	PG_get_text_ext_n(dev, 2, NORMC, "XX ", dxt);

	dt = 1.02*dxt[1];
	xw[0] = ndc[1] + 0.2*(1.0 - ndc[1]);
	xw[1] = ndc[3] - dt;
	PG_frame_point(dev, 2, NORMC, xw, xf);
	PG_write_n(dev, 2, NORMC, xf, "%s", "Levels: ");
  
	ml = (ndc[3] - ndc[2])/dt;
	di = nl/ml;
	di = max(1, di);
	nl = min(nl, di*ml);

	did = max(did, 'A');

        switch (pd->lev_method)
	   {case 1  :
	         for (i = 0; i < nl; i += di)
		     {xw[1] -= dt;
		      idt = did + i;

		      PG_frame_point(dev, 2, NORMC, xw, xf);

/* check the range of the contour label and roll over to numbers
 * if too high - one application did
 */
		      if (('A' <= idt) && (idt <= 'Z'))
			 PG_write_n(dev, 2, NORMC, xf, "%c", idt);
		      else
			 PG_write_n(dev, 2, NORMC, xf, "%d", idt-'A'+1);

		      p[0] = xw[0] + dxt[0];
		      p[1] = xw[1];
		      PG_frame_point(dev, 2, NORMC, p, xf);
		      PG_write_n(dev, 2, NORMC, xf, "%.2e ", lv[i]);};
	         break;
             
/* color coded contour lines */
	    case 2 :
	         PG_get_text_color(dev, &tclr);
	         for (i = 0; i < nl; i += di)
		     {xw[1] -= dt;
		      clr = i + 4;
		      idt = did + i;

		      PG_set_text_color(dev, clr);

		      PG_frame_point(dev, 2, NORMC, xw, xf);

/* check the range of the contour label and roll over to numbers
 * if too high - one application did
 */
		      if (('A' <= idt) && (idt <= 'Z'))
			 PG_write_n(dev, 2, NORMC, xf, "%c", idt);
		      else
			 PG_write_n(dev, 2, NORMC, xf, "%d", idt-'A'+1);

		      PG_set_text_color(dev, dev->WHITE);

		      p[0] = xw[0] + dxt[0];
		      p[1] = xw[1];
		      PG_frame_point(dev, 2, NORMC, p, xf);
		      PG_write_n(dev, 2, NORMC, xf, "%.2e ", lv[i]);};

	         PG_set_text_color(dev, tclr);
	         break;
             
/* fill contours */
	    case 3 :
	    case 4 :
	         r = PM_make_vectors(2, 5);

		 dxw[0] = 0.7*(1.0 - ndc[1]);
		 dxw[1] = dxt[1];
	         PG_get_text_color(dev, &tclr);
	         for (i = 0; i < nl; i += di)
		     {xw[1] -= dt;

		      r[0][0] = xw[0];
		      r[1][0] = xw[1] - 0.02*dxw[1];
		      r[0][1] = xw[0] + dxw[0];
		      r[1][1] = xw[1] - 0.02*dxw[1];
		      r[0][2] = xw[0] + dxw[0];
		      r[1][2] = xw[1] + dxw[1];
		      r[0][3] = xw[0];
		      r[1][3] = xw[1] + dxw[1];
		      r[0][4] = xw[0];
		      r[1][4] = xw[1] - 0.02*dxw[1];

		      PG_frame_points(dev, 2, NORMC, 5, r, r);

		      PG_make_palette_current(dev, opl);
		      clr = _PG_get_fill_contour_color(dev, i, nl);
		      PG_fill_polygon_n(dev, clr, TRUE, 2, NORMC, 5, r);

		      PG_make_palette_current(dev, spl);
		      PG_set_text_color(dev, dev->WHITE);

		      p[0] = xw[0] + 0.1*dxw[0];
		      p[1] = xw[1] + 0.1*dxw[1];
		      PG_frame_point(dev, 2, NORMC, p, xf);
		      PG_write_n(dev, 2, NORMC, xf, "< %.2e ", lv[i]);};

	         PG_set_text_color(dev, tclr);
		 PM_free_vectors(2, r);
	         break;};
             
	PG_set_char_line(dev, 80);

	PG_set_axis_log_scale(dev, 2, oflg);};

    PG_make_palette_current(dev, opl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_LEGEND_LABEL - label the picture */

static void _PG_legend_label(PG_device *dev, PG_picture_desc *pd,
			     PG_graph *data)
   {int axty;
    double ndc[PG_BOXSZ];
    char *label, t[MAXLINE];
    PG_palette *opl;

    if (dev == NULL)
       return;

    opl = PG_get_palette(dev, NULL);
    PG_set_palette(dev, "standard");

    PG_set_text_color(dev, dev->WHITE);

    label = pd->legend_label;
    axty  = pd->ax_type;

    if (axty == CARTESIAN_3D)
       {snprintf(t, MAXLINE, "View Angle (%.2f, %.2f, %.2f)",
		 pd->va[0], pd->va[1], pd->va[2]);
	PG_center_label(dev, 0.1, t);};

    PG_viewport_frame(dev, 2, ndc);
    PG_center_label(dev, 0.5*(1.0 + ndc[3]), label);

    PG_make_palette_current(dev, opl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PICTURE_AXES - draw the axes for the picture */

static void _PG_picture_axes(PG_device *dev, PG_picture_desc *pd,
			     PG_graph *data)
   {int axty, datafl;
    double abx[PG_BOXSZ], rbx[PG_BOXSZ], ext[PG_BOXSZ], dbx[PG_BOXSZ];
    double *extr, *dextr;
    PM_set *domain;
    PG_palette *opl;

    if (dev == NULL)
       return;

    opl = PG_get_palette(dev, NULL);
    PG_set_palette(dev, "standard");

    PG_set_axis_attributes(dev, AXIS_LINECOLOR, dev->line_color,
                                AXIS_LABELCOLOR, dev->text_color, 0);

    PG_set_view_angle(dev, pd->va[0], pd->va[1], pd->va[2]);

    extr   = dev->range_extrema;
    axty   = pd->ax_type;
    datafl = PG_render_data_type(data);

    if (axty == CARTESIAN_3D)
       {domain = (datafl) ? data->f->domain : NULL;
	if (domain != NULL)
	   {PG_box_init(3, abx, 0.0, 1.0);
	    switch (domain->dimension_elem)

/* NOTE: assuming only type PLOT_SCATTER for this case */
	       {case 1 :

/* handle 2D shaded surface and wire-frame case */
		case 2 :
		     if (datafl)
		        {dextr = PM_array_real(domain->element_type,
					       domain->extrema, 4, ext);
			 PG_box_copy(2, abx, dextr);
			 abx[4] = extr[0];
			 abx[5] = extr[1];}
		     else
		        {PG_image_picture_info(data, NULL, abx, rbx);
			 abx[4] = rbx[0];
			 abx[5] = rbx[1];};
		     break;

/* handle 3D wire-frame case */
		case 3 :
		     if (datafl)
		        {dextr = PM_array_real(domain->element_type,
					       domain->extrema, 6, ext);
			 PG_box_copy(3, abx, dextr);}
		     else
		        {PG_image_picture_info(data, NULL, abx, rbx);
			 abx[4] = rbx[0];
			 abx[5] = rbx[1];};
		     break;};

	    PG_box_copy(3, dbx, abx);

	    PG_axis_3(dev, NULL, 0, NULL);};}
/*	    PG_axis_3(dev, NULL, 0, dbx);};} */

    else
       PG_axis(dev, axty);

    PG_make_palette_current(dev, opl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_ADORN_PICTURE - draw all of the picture adornments */

static void _PG_adorn_picture(PG_device *dev, PG_picture_desc *pd,
			      PG_graph *data, int wh)
   {int axty, clp, now;

    if ((dev == NULL) || (pd == NULL) || (data == NULL))
       return;

    axty = pd->ax_type;
    now  = (((axty == CARTESIAN_3D) && wh) ||
	    ((axty != CARTESIAN_3D) && !wh));

    if (now)
       {PG_get_clipping(dev, &clp);

/* draw the palette if needed */
	if (pd->legend_palette_fl)
	   _PG_legend_palette(dev, pd, data);

/* draw the axes if requested */
	if (pd->axis_fl)
	   _PG_picture_axes(dev, pd, data);

/* print a label for the picture if requested */
	if ((pd->legend_label_fl) && (_PG_gattrs.label_space == 0.0))
	   _PG_legend_label(dev, pd, data);

/* print the contour level values (legend) */
	if (pd->legend_contour_fl)
	   _PG_legend_contours(dev, pd, data);
  
/* print the drawable labels if requested */
	if (pd->legend_curve_fl || (_PG_gattrs.plot_labels == TRUE))
	   _PG_print_labels(dev, data);

	PG_set_clipping(dev, clp);

	PG_update_vs(dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_ADORN_BEFORE - adorn a picture before any datat has been rendered */

void PG_adorn_before(PG_device *dev, PG_picture_desc *pd, PG_graph *data)
   {PG_rendering pty;

    pty = data->rendering;
    if ((pty < PLOT_CURVE) || (PLOT_DEFAULT <= pty))
       pty = PG_est_rendering(data);

/* don't adorn the reference mesh plotting */
    if ((pty != PLOT_MESH) || (_PG_gattrs.ref_mesh == FALSE))
       _PG_adorn_picture(dev, pd, data, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_PICTURE - setup a window for the rendering implied by
 *                  - the specified graph DATA
 *                  - NOTE: no drawing of any kind is to be done here
 *                  -       for example, a contour plot may not have setup
 *                  -       the iso levels yet which would prevent the
 *                  -       contour legend from being drawn
 *                  -       each renderer MUST call PG_ADORN_BEFORE
 *                  -       explicitly subsequent to this call
 */

PG_picture_desc *PG_setup_picture(PG_device *dev, PG_graph *data,
				  int save, int clear, int par)
   {int i, change;
    double wc[PG_BOXSZ], ndc[PG_BOXSZ];
    PG_rendering pty;
    PG_picture_desc *pd;

    if (dev == NULL)
       return(NULL);

    PG_make_device_current(dev);

    change = !dev->supress_setup;

    if (par && change)
       dev->autorange = TRUE;

    if (save && change)
       {PG_viewport_frame(dev, 2, ndc);
	PG_get_viewspace(dev, WORLDC, wc);};

    if (clear && change)
       {switch (_PG_gattrs.hl_clear_mode)
	   {case CLEAR_SCREEN :
	         PG_clear_window(dev);
		 break;
	    case CLEAR_VIEWPORT :
	         PG_clear_viewport(dev);
		 break;
	    case CLEAR_FRAME :
	         PG_clear_frame(dev);
		 break;};};

    pd = NULL;

    pty = data->rendering;
    if ((pty < PLOT_CURVE) || (PLOT_DEFAULT <= pty))
       pty = PG_est_rendering(data);

    if (data->f != NULL)
       switch (pty)
         {case PLOT_CONTOUR :
	       pd = PG_setup_picture_contour(dev, data, save, clear);
	       break;

          case PLOT_CURVE:
	       pd = PG_setup_picture_curve(dev, data, save, clear);
	       break;

          case PLOT_DV_BND:
	       pd = PG_setup_picture_dv_bnd(dev, data, save, clear);
	       break;

          case PLOT_FILL_POLY:
	       pd = PG_setup_picture_fill_poly(dev, data, save, clear);
	       break;

          case PLOT_IMAGE:
	       pd = PG_setup_picture_image(dev, data, save, clear);
	       break;

          case PLOT_MESH:
	       pd = PG_setup_picture_mesh(dev, data, save, clear);
	       break;

          case PLOT_SCATTER:
	       pd = PG_setup_picture_scatter(dev, data, save, clear);
	       break;

          case PLOT_WIRE_MESH:
          case PLOT_SURFACE:
	       pd = PG_setup_picture_surface(dev, data, save, clear);
	       break;

          case PLOT_VECTOR :
	       pd = PG_setup_picture_vector(dev, data, save, clear);
	       break;

          default :
	       break;};

    if (save && change && (pd != NULL))
       {for (i = 0; i < 4; i++)
	    {pd->view[i] = ndc[i];
	     pd->wc[i]   = wc[i];};};

    if (par && change)
       PG_parallel_setup(dev, pd);

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_PICTURE - render a list of mappings all in one picture
 *                 - Argument:
 *                 -   PTYP   rendering type
 *                 -   BNDP   on/off flag to show patch boundaries
 *                 -   CBND   line color for patch boundaries
 *                 -   SBND   line style for patch boundaries
 *                 -   WBND   line width for patch boundaries
 *                 -   MSHP   on/off flag to show mesh lines
 *                 -   CMSH   line color for mesh lines
 *                 -   SMSH   line style for mesh lines
 *                 -   WMSH   line width for mesh lines
 */

void PG_draw_picture(PG_device *dev, PM_mapping *f,
		     PG_rendering ptyp, int bndp, int cbnd, int sbnd,
		     double wbnd, int mshp, int cmsh, int smsh,
		     double wmsh)
   {int fls, axs;
    int fin, save_pt;
    pcons *inf, *alst;
    PM_mapping *pf, *nxt;
    PG_rendering pty;
    PG_graph *g, data;
    PG_picture_desc *pd;
    PG_par_rend_info *pri;
    PG_device *dd;
    static int id = 'A';

    if ((dev == NULL) || (f == NULL))
       return;

/* save device attributes */
    PG_get_fill_bound(dev, fls);
    axs = _PG_gattrs.axis_on;

/* set device attributes */
    if (f != NULL)
       inf = SC_copy_alist((pcons *) f->map);
    else
       inf = NULL;

    pty = ptyp;

    g = &data;
    SC_MEM_INIT(PG_graph, g);

    data.info_type  = SC_PCONS_P_S;
    data.info       = (void *) inf;
    data.f          = f;
    data.rendering  = pty;
    data.mesh       = (mshp != 0);
    data.identifier = id;

    PG_set_attrs_graph(g,
		       "DRAW-MESH", SC_INT_I, FALSE, mshp,
		       NULL);

    fin = dev->finished;
    dev->finished = FALSE;

    pd = PG_setup_picture(dev, g, TRUE, TRUE, TRUE);
    dev->supress_setup = TRUE;

    save_pt = _PG_gattrs.plot_title;

/* loop over all mappings */
    for (pf = f; pf != NULL; pf = nxt)
        {nxt = pf->next;

	 pf->next = NULL;

/* suppress the label on all but the first linked list */
	 if (pf != f)
	    _PG_gattrs.plot_title = FALSE;

	 g = PG_make_graph_from_mapping(pf, SC_PCONS_P_S, NULL, id, NULL);

/* setup the graph's info list */
	 inf = SC_copy_alist((pcons *) pf->map);
	 inf = SC_append_alist((pcons *) g->info, inf);

/* finish up the graph */
	 g->info      = (void *) inf;
	 g->mesh      = (mshp != 0);
	 g->rendering = pty;

/* handle the patch boundary attribute */
	 if (bndp)
	    PG_set_attrs_set(pf->domain,
			     "DOMAIN-BORDER-WIDTH", SC_DOUBLE_I,  FALSE, wbnd,
			     "DOMAIN-BORDER-COLOR", SC_INT_I, FALSE, cbnd,
			     "DOMAIN-BORDER-STYLE", SC_INT_I, FALSE, sbnd,
			     NULL);

/* handle the mesh plotting attribute */
	 PG_set_attrs_graph(g,
			    "DRAW-MESH", SC_INT_I, FALSE, mshp,
			    NULL);

/* if the mapping states its rendering then do it! */
	 PG_get_attrs_set(pf->domain,
			  "PLOT-TYPE", SC_INT_I, &pty, ptyp,
			  NULL);

	 if (pf->range == NULL)
	    PG_mesh_plot(dev, g);
	 else
	    PG_draw_graph(dev, g);

/* carefully release the graph - but not the mapping */
/*	 SC_free_alist(inf, 3); */
	 SC_free_alist(g->info, 3);

	 CFREE(g);

	 if (id > 'Z')
	    id = 'A';

	 _PG_gattrs.axis_on = FALSE;

	 pf->next = nxt;};
	     
    pri = dev->pri;
    if (pri != NULL)
       {pri->render = ptyp;
	dd = pri->dd;
	if (dd != NULL)
	   dd->pri->render = ptyp;};

    if (_PG.picture != NULL)
       (*_PG.picture)(dev, &data, pd);

/* the free was moved after finish because it zeroed out parts of data
 * needed in finish and so caused problems plotting mesh lines to crash
 */
    dev->finished = fin;
    if (pd != NULL)
       {alst = pd->alist;
	PG_finish_picture(dev, &data, pd);
	SC_free_alist(alst, 3);};

    dev->supress_setup = FALSE;

/* restore device attributes */
    PG_set_fill_bound(dev, fls);
    _PG_gattrs.axis_on = axs;

    _PG_gattrs.plot_title = save_pt;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FINISH_PICTURE - draw the requested adornments and
 *                   - finish the plot frame if requested
 */

void PG_finish_picture(PG_device *dev, PG_graph *data, PG_picture_desc *pd)
   {double wc[PG_BOXSZ], ndc[PG_BOXSZ];
    PG_palette *cpl;

    if ((dev == NULL) || (pd == NULL) || (data == NULL))
       return;

    PG_set_clipping(dev, FALSE);

    cpl = pd->legend_palette;

    _PG_adorn_picture(dev, pd, data, TRUE);

/* draw a reference mesh if requested - must be last */
    if (pd->mesh_fl)
       {PG_get_viewspace(dev, NORMC, ndc);
	PG_ref_mesh(dev, data, 2, ndc, pd->viewport);};
       
/* do the final frame update */
    if (dev->finished)
       {PG_finish_plot(dev);}
    else
       {PG_update_vs(dev);};

/* restore saved state */
    if (cpl != NULL)
       {PG_make_palette_current(dev, cpl);};

    if (_PG_gattrs.restore_viewport == TRUE)
       {PG_box_copy(2, ndc, pd->view);
	if ((ndc[0] != ndc[1]) && (ndc[2] != ndc[3]))
	   PG_set_viewspace(dev, 2, NORMC, ndc);

	PG_box_copy(2, wc, pd->wc);
	if ((wc[0] != wc[1]) && (wc[2] != wc[3]))
	   PG_set_viewspace(dev, 2, WORLDC, wc);};

    CFREE(pd->levels);
    CFREE(pd);

/* release the device */
    PG_release_current_device(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DISPATCH_2D - dispatch 2d graphs to the requested rendering
 *                 - routine
 *                 - user supplied hooks should be inserted into this
 *                 - process at some point
 */

static void _PG_dispatch_2d(PG_device *dev, PG_graph *data, int range_dim)
   {PG_rendering pty;

/* get the rendering method for this graph */
    pty = data->rendering;

    switch (range_dim)
       {case 1 :
	     switch (pty)
	        {default           :
		 case PLOT_DEFAULT :
		 case PLOT_CONTOUR :
		      PG_contour_plot(dev, data, 2);
		      break;
	         case PLOT_IMAGE :
		      PG_image_plot(dev, data, 2);
		      break;
	         case PLOT_DV_BND :
		      PG_dv_bnd_plot(dev, data, 2);
		      break;
	         case PLOT_FILL_POLY :
		      PG_poly_fill_plot(dev, data, 2);
		      break;
	         case PLOT_WIRE_MESH :
		 case PLOT_SURFACE   :
		      PG_surface_plot(dev, data, 2);
		      break;
		 case PLOT_MESH :
		      PG_mesh_plot(dev, data, 2);
		      break;
		 case PLOT_SCATTER :
		      PG_scatter_plot(dev, data, 2);
		      break;};
	     break;

        case 2  :
	     switch (pty)
	        {case PLOT_DEFAULT :
		 case PLOT_VECTOR  :
                      PG_vector_plot(dev, data, 2);
                      break;
		 case PLOT_SURFACE :
		      PG_surface_plot(dev, data, 2);
		      break;
	         case PLOT_FILL_POLY :
		      PG_poly_fill_plot(dev, data, 2);
		      break;
	         case PLOT_SCATTER :
		      PG_scatter_plot(dev, data, 2);
		      break;
		 case PLOT_MESH :
		      PG_mesh_plot(dev, data, 2);
		      break;
	         default :
		      break;};
	     break;

        default :
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DISPATCH_3D - dispatch 3d graphs to the requested rendering
 *                 - routine
 *                 - user supplied hooks should be inserted into this
 *                 - process at some point
 */

static void _PG_dispatch_3d(PG_device *dev, PG_graph *data, int range_dim)
   {PG_rendering pty;

/* get the rendering method for this graph */
    pty = data->rendering;

    switch (pty)
       {default             :
	case PLOT_WIRE_MESH :
	case PLOT_SURFACE   :
	     PG_surface_plot(dev, data, 3);
	     break;
	case PLOT_CONTOUR :
	     PG_contour_plot(dev, data, 3);
	     break;
        case PLOT_MESH :
	     PG_mesh_plot(dev, data, 3);
	     break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DRAW_GRAPH - main graph plotting control routine */

void PG_draw_graph(PG_device *dev, PG_graph *data)
   {int domain_dim, range_dim;
    PM_mapping *f;

    if (_PG_gattrs.text_format == NULL)
       _PG_gattrs.text_format = SC_strsavef("%.0f",
					    "PERM|char*:PG_DRAW_GRAPH :format");

    PG_get_attrs_graph(data, FALSE,
		       "RENDERING-TYPE", SC_INT_I, &data->rendering, data->rendering,
		       NULL);

    f = data->f;
    if (f->range == NULL)
       PG_mesh_plot(dev, data);

    else
       {domain_dim = f->domain->dimension_elem;
        range_dim  = f->range->dimension_elem;

	switch (domain_dim)
	   {case 1 :
	         switch (range_dim)
		    {case 1 :
		          PG_curve_plot(dev, data);
			  break;
		     default :
		          break;};
		 break;

	    case 2 :
	         _PG_dispatch_2d(dev, data, range_dim);
		 break;

	    case 3 :
		 _PG_dispatch_3d(dev, data, range_dim);
		 break;

	    default :
	         break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_PICTURE_HOOK - set the picture hook */

void PG_set_picture_hook(void (*f)(PG_device *dev, PG_graph *data,
				   PG_picture_desc *pd))
   {

    _PG.picture = f;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
