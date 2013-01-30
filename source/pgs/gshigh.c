/*
 * GSHIGH.C - Portable Graphics System
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

#define PAD_FACTOR   0.01

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CENTER_LABEL - print a label centered on a line at a specified
 *                 - y value given in screen coordinates
 *
 * #bind PG_center_label fortran() scheme(pg-center-label) python()
 */

void PG_center_label(PG_device *dev ARG(,,cls),
		     double sy, char *label)
   {double p[PG_SPACEDM];
    PG_dev_geometry *g;

    if ((dev != NULL) && (_PG_gattrs.plot_title == TRUE))
       {g = &dev->g;

	PG_get_text_ext_n(dev, 2, NORMC, label, p);

	p[0] = g->fr[0] + 0.5*(g->fr[1] - g->fr[0] - p[0]);
	p[1] = g->fr[2] + sy*(g->fr[3] - g->fr[2]);

	PG_write_n(dev, 2, NORMC, p, "%s", label);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PRINT_LABEL_SET - print a set of labels at the bottom
 *                    - of the screen/page
 *                    - arguments are:
 *                    -    DEV       the device
 *                    -    NLABS     number of labels to draw
 *                    -    LABELS    the labels
 *                    -    FILES     files associated with the labels
 *                    -    DATAID    dataid associated with the labels
 *                    -    MODIFIED  modified flag associated with the labels
 *                    -    CLR       label color
 *                    -    EXTR      X/Y extrema
 *                    -    CTG       category (e.g. CURVE or MAPPING)
 *                    -    LLF       label length
 *                    -    LCF       label color flag
 *                    -    SLF       squeeze label flag
 *                    -    LTS       label type size
 */

void PG_print_label_set(double *pyo, PG_device *dev, int nlabs,
			char **labels, char **files,
			int *dataid, int *modified, int *clr,
			double *extr, char *ctg,
		        int llf, int lcf, int slf, int lts)
   {int i, color, size, id, md, clp;
    int ne, tc, hcd;
    int oflg[PG_SPACEDM], nflg[PG_SPACEDM];
    double xhlbl, xolbl, xofil, dxtfile, dsxf;
    double ctxpos, overlap;
    double dsx, dsy, yo;
    double dxt[PG_SPACEDM], dxtx[PG_SPACEDM], xb[PG_SPACEDM], p[PG_SPACEDM];
    double bnd[PG_BOXSZ], wc[PG_BOXSZ];
    double xo[PG_BOXSZ], sx[PG_BOXSZ];
    char label[MAXLINE], bf[MAXLINE], *face, *style, *s;
    PG_palette *pl;
    PG_dev_geometry *g;
    static double tol = HUGE;

    if (tol == HUGE)
       tol = 10.0*PM_machine_precision();

    if (dev == NULL)
       return;

    memset(xb, 0, sizeof(xb));

    g   = &dev->g;
    hcd = HARDCOPY_DEVICE(dev);

    yo = *pyo;
    pl = dev->current_palette;

    clp = PG_fget_clipping(dev);
    PG_fget_font(dev, &face, &style, &size);

    color = PG_fget_text_color(dev);
    PG_fset_text_color(dev, dev->WHITE, TRUE);
/*
    PG_fset_palette(dev, "standard");
*/

    for (id = 0; id < 2; id++)
        nflg[id] = FALSE;

    PG_fget_axis_log_scale(dev, 2, oflg);
    PG_fset_axis_log_scale(dev, 2, nflg);
    PG_fset_clipping(dev, FALSE);

    PG_get_viewspace(dev, BOUNDC, bnd);

    if (hcd == TRUE)
       {xb[0] = 0;
	xb[1] = 0;
	PG_trans_point(dev, 2, PIXELC, xb, WORLDC, xb);
        if (_PG_gattrs.plot_date == TRUE)
           {PG_fset_font(dev, face, style, 6);
            memset(label, '\0', MAXLINE);
	    s = SC_date();
            SC_strncpy(label, MAXLINE, s, -1);
	    CFREE(s);

#if defined(HAVE_POSIX_SYS)
            label[strlen(label)] = ' ';
            gethostname(&label[strlen(label)], MAXLINE - strlen(label));
#endif

            PG_get_text_ext_n(dev, 2, WORLDC, label, dxt);

	    p[0] = bnd[1] - dxt[0];
	    p[1] = bnd[3] + 3.0*dxt[1];
            PG_write_n(dev, 2, WORLDC, p, label);};}

    PG_get_viewspace(dev, WORLDC, wc);
    PG_fset_font(dev, face, style, lts);

    sx[0] = 0.0;
    sx[1] = 0.992;
    if (hcd)
       {sx[0] = 0.03;
        sx[1] = 0.96;}
    sx[2] = 0.0;
    sx[3] = 0.0;

    for (i = 0, dxt[0] = 0, dxtfile = 0; i < nlabs; i++)
        {if (files[i] != NULL)
	    {PG_get_text_ext_n(dev, 2, NORMC, files[i], dxt);
	     dxtfile = max(dxtfile, dxt[0]);};};

    if (nlabs <= 0)
       {PG_fset_clipping(dev, clp);
        PG_fset_text_color(dev, color, TRUE);
        PG_fset_axis_log_scale(dev, 2, oflg);
        PG_fset_font(dev, face, style, size);
        return;};

    PG_get_text_ext_n(dev, 2, NORMC, "FILE", dxt);
    dsxf   = dxt[0]/2;
    dxt[0] = max(dxtfile, dxt[0]);
    xofil  = sx[1];
    if (dxtfile > 0)
       xofil -= dxt[0];

    p[0] = sx[0];
    p[1] = sx[2];
    PG_trans_point(dev, 2, NORMC, p, WORLDC, p);
    sx[0] = p[0];
    sx[2] = p[1];

    p[0] = xofil;
    p[1] = sx[3];
    PG_trans_point(dev, 2, NORMC, p, WORLDC, p);
    xofil = p[0];
    sx[3] = p[1];

    dxtfile /= dev->g.w_nd[1];
    dsxf    /= dev->g.w_nd[1];

    PG_get_text_ext_n(dev, 2, WORLDC, " -0.00e+00", dxt);
    dsx   = dxt[0]/2 - dsxf;
    xo[3] = xofil - dxt[0];
    if (dxtfile > 0)
       xo[3] -= dsxf/2;
    xo[2] = xo[3] - dxt[0];
    xo[1] = xo[2] - dxt[0];
    xo[0] = xo[1] - dxt[0];

    PG_get_text_ext_n(dev, 2, WORLDC, ctg, dxt);
    xhlbl = sx[0] + dxt[0] + dsxf/2;

    if ((_PG_gattrs.numeric_data_id == TRUE) || (nlabs > 26))
       {snprintf(bf, MAXLINE, " @%d M ", nlabs);
        PG_get_text_ext_n(dev, 2, WORLDC, bf, dxt);}
    else
       PG_get_text_ext_n(dev, 2, WORLDC, "   M  ", dxt);

    xolbl = sx[0] + dxt[0] + dsxf/2;

/* print the header if this is the initial label */
    if (yo == HUGE)
       {dsy = (wc[3] - wc[2])*(1.0 + g->pad[2] + g->pad[3]);
	yo  = wc[2] - g->pad[2]*(wc[3] - wc[2]) - 0.16*dsy - (wc[3] - wc[2])*_PG_gattrs.label_yoffset;
	if (hcd)
	   yo = wc[2] - g->pad[2]*(wc[3] - wc[2]) - 0.11*dsy - (wc[3] - wc[2])*_PG_gattrs.label_yoffset;

	p[1] = yo;

/* print the header */
	p[0] = sx[0] + dsxf/2;
	PG_write_n(dev, 2, WORLDC, p, ctg);

	p[0] = xhlbl + dsx;
	PG_write_n(dev, 2, WORLDC, p, " LABEL  ");
        
	if ((llf < 40) || slf)
	   {PG_get_text_ext_n(dev, 2, WORLDC, " LABEL    ", dxtx);
            ctxpos = xhlbl + dsx + dxtx[0];

/* adjust label spacing to avoid overlap, let them overflow to right */
            if (ctxpos >= xo[0]+dsx)
               {overlap = ctxpos - (xo[0]+dsx);
                if (hcd)
                   overlap += 0.03;

                xo[0] += overlap + SMALL;
                xo[1] += overlap + SMALL;
                xo[2] += overlap + SMALL;
                xo[3] += overlap + SMALL;
                xofil  = xo[3] + 2*dsx;}

	    p[0] = xo[0] + dsx;
            PG_write_n(dev, 2, WORLDC, p, "XMIN");

	    p[0] = xo[1] + dsx;
	    PG_write_n(dev, 2, WORLDC, p, "XMAX");

	    p[0] = xo[2] + dsx;
	    PG_write_n(dev, 2, WORLDC, p, "YMIN");

	    p[0] = xo[3] + dsx;
	    PG_write_n(dev, 2, WORLDC, p, "YMAX");

	    if (dxtfile > 0)
	       {p[0] = xofil+max((0.5*dxtfile) - dsxf, 0.0);
		PG_write_n(dev, 2, WORLDC, p, "FILE");};};

	yo -= 2*dxt[1];};

    ne = 0;
    for (i = 0; i < nlabs; i++)
        {if (hcd && (yo <= xb[1]))
	    if (!JPG_DEVICE(dev))
 	       break;

	 s = labels[i];
	 if (s == NULL)
	    continue;

	 id = (dataid == NULL) ? 0 : dataid[i];
	 md = (modified == NULL) ? FALSE : modified[i];

	 if (clr != NULL)
	    {tc = clr[i];
	     if (lcf == TRUE)
	        tc = (tc == -1) ? dev->GRAY : tc;
	     else
	        tc = (tc == -1) ? dev->GRAY : dev->WHITE;

	     PG_fset_text_color(dev, tc, TRUE);};

/* prep the label text */
	 strcpy(label, s);
	 if (slf)
	    SC_squeeze_blanks(label);
	 if (strlen(label) > llf)
	    label[llf] = '\0';

/* print the data id */
	 p[0] = sx[0];
	 p[1] = yo;
	 if (_PG_gattrs.numeric_data_id == TRUE)
	     PG_write_n(dev, 2, WORLDC, p, "    @%d", id);
	 else if (id > 'Z')
	    PG_write_n(dev, 2, WORLDC, p, "    @%d", id - 'A' + 1);
	 else
	    PG_write_n(dev, 2, WORLDC, p, "    %c", id);

/* print the modified flag */
	 if (md)
	    {p[0] = sx[0] + dxt[0];
	     PG_write_n(dev, 2, WORLDC, p, "*");};

/* print the label text */
	 p[0] = xolbl;
	 PG_write_n(dev, 2, WORLDC, p, " %s", label);

	 if (((llf < 40) || slf) && (extr != NULL))
	    {double mn, mx, dm;

/* print the X extrema */
	     mn = extr[ne++];
	     mx = extr[ne++];
             dm = mx - mn;
             if (ABS(mn) < tol*ABS(dm))
	        mn = 0.0;
             if (ABS(mx) < tol*ABS(dm))
	        mx = 0.0;

	     p[0] = xo[0];
	     PG_write_n(dev, 2, WORLDC, p, "%10.2e", mn);
	     p[0] = xo[1];
	     PG_write_n(dev, 2, WORLDC, p, "%10.2e", mx);

/* print the Y extrema */
	     mn = extr[ne++];
	     mx = extr[ne++];
             dm = mx - mn;
             if (ABS(mn) < tol*ABS(dm))
	        mn = 0.0;
             if (ABS(mx) < tol*ABS(dm))
	        mx = 0.0;

	     p[0] = xo[2];
	     PG_write_n(dev, 2, WORLDC, p, "%10.2e", mn);
	     p[0] = xo[3];
	     PG_write_n(dev, 2, WORLDC, p, "%10.2e", mx);

/* print the file name */
	     if (files[i] != NULL)
	        {p[0] = xofil;
		 PG_write_n(dev, 2, WORLDC, p, "%s", files[i]);};};

	 if (JPG_DEVICE(dev))
	    yo -= 0.5*dxt[1];

	 yo -= dxt[1];};

    PG_fset_clipping(dev, clp);
    PG_fset_text_color(dev, color, TRUE);
    PG_fset_axis_log_scale(dev, 2, oflg);
    PG_fset_font(dev, face, style, size);

    CFREE(face);
    CFREE(style);

    dev->current_palette = pl;

    *pyo = yo;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PRINT_LABELS - print the mapping labels at the bottom
 *                  - of the screen/page
 *                  - arguments:
 *                  -
 *                  -   DEV    the device drawn to
 *                  -   DATA   the link list of graphs plotted
 */

void _PG_print_labels(PG_device *dev, PG_graph *data)
   {int tc, ne;
    int ndd, ndr, nlabs, datafl;
    char **labels, **files;
    int *dataid, *modified, *clr;
    double dextr[32], rextr[32];
    double *extr;
    pcons *info;
    PM_set *domain, *range;
    PM_mapping *pf;
    PG_graph *g;
    PG_image *im;

    if ((_PG_gattrs.plot_labels == TRUE) &&
	(HARDCOPY_DEVICE(dev) || (_PG_gattrs.label_space > 0.0)))
       {datafl = PG_render_data_type(data);
	if (datafl)
	   {nlabs = 0;
	    for (g = data; g != NULL; g = g->next)
	        {for (pf = g->f; pf != NULL; pf = pf->next)
		   nlabs++;};}
	else
	   nlabs = 1;

	labels   = CMAKE_N(char *, nlabs);
	files    = CMAKE_N(char *, nlabs);
	dataid   = CMAKE_N(int, nlabs);
	modified = CMAKE_N(int, nlabs);
	clr      = CMAKE_N(int, nlabs);
	extr     = CMAKE_N(double, 4*nlabs);

	ne = 0;
	if (datafl)
	   {nlabs = 0;
	    for (g = data; g != NULL; g = g->next)
	        {for (pf = g->f; pf != NULL; pf = pf->next)
		     {domain = pf->domain;
		      range  = pf->range;
		      info   = (pcons *) g->info;

		      tc = dev->WHITE;
		      if (_PG_gattrs.label_color_flag == TRUE)
			 PG_get_attrs_alist(info,
					    "LINE-COLOR", SC_INT_I, &tc, dev->WHITE,
					    NULL);

		      dataid[nlabs]   = g->identifier;
		      modified[nlabs] = FALSE;
		      labels[nlabs]   = pf->name;
		      files[nlabs]    = pf->file;
		      clr[nlabs]      = tc;

		      if ((_PG_gattrs.label_length < 40) || _PG_gattrs.squeeze_labels)
			 {ndd = domain->dimension_elem;
			  PM_array_real(domain->element_type, domain->extrema,
					2*ndd, dextr);

			  if (range == NULL)
			     {rextr[0] = 0.0;
			      rextr[1] = 0.0;}
			  else
			     {ndr = range->dimension_elem;
			      PM_array_real(range->element_type, range->extrema,
					    2*ndr, rextr);};

			  if (ndd == 1)
			     {extr[ne++] = dextr[0];
			      extr[ne++] = dextr[1];
			      extr[ne++] = rextr[0];
			      extr[ne++] = rextr[1];}
			  else
			     {extr[ne++] = dextr[0];
			      extr[ne++] = dextr[1];
			      extr[ne++] = dextr[2];
			      extr[ne++] = dextr[3];};}

		      nlabs++;};};}
	else
	   {im = (PG_image *) data->f;

	    dataid[0]   = 1;
	    modified[0] = FALSE;
	    labels[0]   = im->label;
	    files[0]    = NULL;
	    clr[0]      = dev->WHITE;

	    extr[ne++] = im->xmin;
	    extr[ne++] = im->xmax;
	    extr[ne++] = im->ymin;
	    extr[ne++] = im->ymax;};

	PG_print_label_set(&_PG_gattrs.label_position_y,
			   dev, nlabs, labels, files,
			   dataid, modified, clr, extr, "MAP",
			   _PG_gattrs.label_length,
			   _PG_gattrs.label_color_flag,
			   _PG_gattrs.squeeze_labels,
			   _PG_gattrs.label_type_size);

	CFREE(labels);
	CFREE(files);
	CFREE(dataid);
	CFREE(modified);
	CFREE(clr);
	CFREE(extr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_POLAR_LIMITS - find the polar limits of the arrays X and Y */

void PG_polar_limits(double *bx, double *x, double *y, int n)
   {int i;
    double rco, tco, xco, yco;

    for (i = 0; i < n; i++)
        {rco = y[i];
	 tco = x[i];

	 xco = rco*cos(tco);
	 yco = rco*sin(tco);

	 bx[0] = min(bx[0], xco);
	 bx[1] = max(bx[1], xco);
	 bx[2] = min(bx[2], yco);
	 bx[3] = max(bx[3], yco);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SET_LIMITS_N - find limits of x and y arrays */
 
void PG_set_limits_n(PG_device *dev, int nd, PG_coord_sys cs, long n,
		     double **x, PG_rendering pty)
   {int l, id;
    double wc[PG_BOXSZ];

    switch (pty)
       {case PLOT_INSEL :
	     PM_vector_extrema(nd, n, x, wc);
             wc[0] = 0.0;
             wc[1] = 1.5;
             break;

        case PLOT_POLAR :
	     PG_box_init(2, wc, HUGE, -HUGE);
	     PG_polar_limits(wc, x[0], x[1], n);
 	     break;

        case PLOT_HISTOGRAM :
        case PLOT_CARTESIAN :
        default        :
	     PM_vector_extrema(nd, n, x, wc);
             break;};
        
/* force non-empty box */
    for (id = 0; id < nd; id++)
        {l = 2*id;
	 if ((wc[l+1] - wc[l]) == 0)
	    wc[l+1] = wc[l] + 1;};

    PG_set_viewspace(dev, nd, cs, wc);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_PLOT_TYPE - set the plot and axis type */

pcons *PG_set_plot_type(pcons *info, PG_rendering pty, int axs)
   {

    if (pty == PLOT_INSEL)
       axs = INSEL;

    else if (axs == INSEL)
       axs = pty;

    info = PG_set_attrs_alist(info,
			      "PLOT-TYPE", SC_INT_I, FALSE, pty,
			      "AXIS-TYPE", SC_INT_I, FALSE, axs,
			      NULL);

    return(info);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLIP_DATA - clip the data points to the boundary region */

void PG_clip_data(PG_device *dev, double *tx, double *ty, int *ptn,
		  double *x, double *y, int n)
   {int i, tp, flag;
    double bnd[PG_BOXSZ];
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    PG_dev_geometry *g;

    if (dev != NULL)
       {g = &dev->g;

	PG_get_viewspace(dev, BOUNDC, bnd);

	if (bnd[1] < bnd[0])
	   {SC_SWAP_VALUE(double, bnd[0], bnd[1]);};

	if (bnd[3] < bnd[2])
	   {SC_SWAP_VALUE(double, bnd[2], bnd[3]);};

	tp = 0;

	if (n == 1)
	   {x1[0] = x[0];
	    x1[1] = y[0];
	    if (PG_box_contains(2, bnd, x1) == TRUE)
	       {tx[tp] = x1[0];
		ty[tp] = x1[1];
		tp++;};}

	else
	   {for (i = 1; i < n; i++)
	        {x1[0] = x[i-1];
		 x2[0] = x[i];
		 x1[1] = y[i-1];
		 x2[1] = y[i];

		 flag = 0;
		 PG_clip_line(&flag, x1, x2, bnd, g->iflog);

		 if (((tp == 0) || (flag & 1)) && flag)
		    {tx[tp] = x1[0];
		     ty[tp] = x1[1];
		     tp++;};

		 if (flag & 2)
		    {tx[tp] = x2[0];
		     ty[tp] = x2[1];
		     tp++;};};};

	*ptn = tp;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLIP_LINE - clip the line X1 = (x1, y1) to X2 = (x2, y2) to the box
 *              - (xmn, ymn) to (xmx, ymx)
 *              - return FLAG value
 *              -    0 - take no end points
 *              -    1 - take the left end point
 *              -    2 - take the right end point
 *              -    3 - take both end points;
 */

void PG_clip_line(int *pfl, double *x1, double *x2, double *wc, int *lgf)
   {double lrgr, smllr;
    double tx[PG_SPACEDM], dx[PG_SPACEDM], dxt[PG_SPACEDM];
    double lwc[PG_BOXSZ];

    PG_box_copy(3, lwc, wc);

/* NOTE: it is possible that the box can be inverted - e.g. tgslax
 * but for clipping purposes make the box have the standard
 * orientation
 */
    if (lwc[0] > lwc[1])
       SC_SWAP_VALUE(double, lwc[0], lwc[1]);

    if (lwc[2] > lwc[3])
       SC_SWAP_VALUE(double, lwc[2], lwc[3]);

    lrgr  = 1.0 + PAD_FACTOR;
    smllr = 1.0/lrgr;

    dx[0] = PAD_FACTOR*ABS(lwc[1] - lwc[0]);
    dx[1] = PAD_FACTOR*ABS(lwc[3] - lwc[2]);

    tx[0] = 0.0;
    tx[1] = 0.0;;
	
/* if X1 and X2 are inside */
    if ((PG_box_contains(2, lwc, x1) == TRUE) &&
        (PG_box_contains(2, lwc, x2) == TRUE))
       *pfl |= 2;

/* if both X1 -> X2 doesn't cross trivially */
    else if (((x1[0] < lwc[0]) && (x2[0] < lwc[0])) ||
             ((x1[0] > lwc[1]) && (x2[0] > lwc[1])) ||
             ((x1[1] < lwc[2]) && (x2[1] < lwc[2])) ||
             ((x1[1] > lwc[3]) && (x2[1] > lwc[3])))
       *pfl = 0;

/* if X1 is inside and X2 is outside we're leaving */
    else if (((lwc[0] <= x1[0]) && (x1[0] <= lwc[1])) &&
             ((lwc[2] <= x1[1]) && (x1[1] <= lwc[3])))
       {if (x2[0] < lwc[0])
           {dxt[0] = lwc[0] - x2[0];
	    dx[0]  = min(dx[0], dxt[0]);

	    if (lgf[0])
	       tx[0] = smllr*lwc[0];
	    else
	       tx[0] = lwc[0] - dx[0];

            PM_interp(tx[1], tx[0], x1[0], x1[1], x2[0], x2[1]);
	    x2[1] = tx[1];
	    x2[0] = tx[0];}

        else if (lwc[1] < x2[0])
           {dxt[0] = x2[0] - lwc[1];
	    dx[0]  = min(dx[0], dxt[0]);

	    if (lgf[0])
	       tx[0] = lrgr*lwc[1];
	    else
	       tx[0] = lwc[1] + dx[0];

            PM_interp(tx[1], tx[0], x1[0], x1[1], x2[0], x2[1]);
            x2[1] = tx[1];
	    x2[0] = tx[0];};

        if (x2[1] < lwc[2])
           {dxt[1] = lwc[2] - x2[1];
	    dx[1]  = min(dx[1], dxt[1]);

	    if (lgf[1])
	       tx[1] = smllr*lwc[2];
	    else
	       tx[1] = lwc[2] - dx[1];

            PM_interp(tx[0], tx[1], x1[1], x1[0], x2[1], x2[0]);}

        else if (lwc[3] < x2[1])
           {dxt[1] = x2[1] - lwc[3];
	    dx[1]  = min(dx[1], dxt[1]);

	    if (lgf[1])
	       tx[1] = lrgr*lwc[3];
	    else
	       tx[1] = lwc[3] + dx[1];

            PM_interp(tx[0], tx[1], x1[1], x1[0], x2[1], x2[0]);};

        x2[0] = tx[0];
        x2[1] = tx[1];

        *pfl |= 2;}

/* if X1 is outside and X2 is inside we're entering */
    else if (((lwc[0] <= x2[0]) && (x2[0] <= lwc[1])) &&
             ((lwc[2] <= x2[1]) && (x2[1] <= lwc[3])))
       {if (x1[0] < lwc[0])
           {dxt[0] = lwc[0] - x1[0];
	    dx[0]  = min(dx[0], dxt[0]);

	    if (lgf[0])
	       tx[0] = smllr*lwc[0];
	    else
	       tx[0] = lwc[0] - dx[0];

            PM_interp(tx[1], tx[0], x1[0], x1[1], x2[0], x2[1]);
	    x1[1] = tx[1];
	    x1[0] = tx[0];}

        else if (lwc[1] < x1[0])
           {dxt[0] = x1[0] - lwc[1];
	    dx[0]  = min(dx[0], dxt[0]);

	    if (lgf[0])
	       tx[0] = lrgr*lwc[1];
	    else
	       tx[0] = lwc[1] + dx[0];

            PM_interp(tx[1], tx[0], x1[0], x1[1], x2[0], x2[1]);
            x1[1] = tx[1];
	    x1[0] = tx[0];};

        if (x1[1] < lwc[2])
           {dxt[1] = lwc[2] - x1[1];
	    dx[1]  = min(dx[1], dxt[1]);

	    if (lgf[1])
	       tx[1] = smllr*lwc[2];
	    else
	       tx[1] = lwc[2] - dx[1];

            PM_interp(tx[0], tx[1], x1[1], x1[0], x2[1], x2[0]);}

        else if (lwc[3] < x1[1])
           {dxt[1] = x1[1] - lwc[3];
	    dx[1]  = min(dx[1], dxt[1]);

	    if (lgf[1])
	       tx[1] = lrgr*lwc[3];
	    else
	       tx[1] = lwc[3] + dx[1];

            PM_interp(tx[0], tx[1], x1[1], x1[0], x2[1], x2[0]);};

        x1[0] = tx[0];
        x1[1] = tx[1];

        *pfl |= 3;}

/* if X1 -> X2 crosses x = lwc[0] going left to right interpolate and recurse */
    else if ((x1[0] < lwc[0]) && (lwc[0] <= x2[0]))
       {tx[0] = lwc[0];
        PM_interp(tx[1], tx[0], x1[0], x1[1], x2[0], x2[1]);
       
        x1[0] = tx[0];
        x1[1] = tx[1];

        *pfl |= 1;

        PG_clip_line(pfl, x1, x2, lwc, lgf);}

/* if X1 -> X2 crosses x = lwc[0] going right to left interpolate and recurse */
    else if ((x2[0] < lwc[0]) && (lwc[0] <= x1[0]))
       {tx[0] = lwc[0];
        PM_interp(tx[1], tx[0], x1[0], x1[1], x2[0], x2[1]);
       
        x2[0] = tx[0];
        x2[1] = tx[1];

        *pfl |= 2;

        PG_clip_line(pfl, x1, x2, lwc, lgf);}

/* if X1 -> X2 crosses x = lwc[1] going left to right interpolate and recurse */
    else if ((x1[0] <= lwc[1]) && (lwc[1] < x2[0]))
       {tx[0] = lwc[1];
        PM_interp(tx[1], tx[0], x1[0], x1[1], x2[0], x2[1]);
       
        x2[0] = tx[0];
        x2[1] = tx[1];

        *pfl |= 2;

        PG_clip_line(pfl, x1, x2, lwc, lgf);}

/* if X1 -> X2 crosses x = lwc[1] going right to left interpolate and recurse */
    else if ((x2[0] <= lwc[1]) && (lwc[1] < x1[0]))
       {tx[0] = lwc[1];
        PM_interp(tx[1], tx[0], x1[0], x1[1], x2[0], x2[1]);
       
        x1[0] = tx[0];
        x1[1] = tx[1];

        *pfl |= 1;

        PG_clip_line(pfl, x1, x2, lwc, lgf);}

/* if X1 -> X2 crosses y = lwc[2] going bottom to top interpolate and recurse */
    else if ((x1[1] < lwc[2]) && (lwc[2] <= x2[1]))
       {tx[1] = lwc[2];
        PM_interp(tx[0], tx[1], x1[1], x1[0], x2[1], x2[0]);
       
        x1[0] = tx[0];
        x1[1] = tx[1];

        *pfl |= 1;

        PG_clip_line(pfl, x1, x2, lwc, lgf);}

/* if X1 -> X2 crosses y = lwc[2] going top to bottom interpolate and recurse */
    else if ((x2[1] < lwc[2]) && (lwc[2] <= x1[1]))
       {tx[1] = lwc[2];
        PM_interp(tx[0], tx[1], x1[1], x1[0], x2[1], x2[0]);
       
        x2[0] = tx[0];
        x2[1] = tx[1];

        *pfl |= 2;

        PG_clip_line(pfl, x1, x2, lwc, lgf);}

/* if X1 -> X2 crosses y = lwc[3] going bottom to top interpolate and recurse */
    else if ((x1[1] <= lwc[3]) && (lwc[3] < x2[1]))
       {tx[1] = lwc[3];
        PM_interp(tx[0], tx[1], x1[1], x1[0], x2[1], x2[0]);
       
        x2[0] = tx[0];
        x2[1] = tx[1];

        *pfl |= 2;

        PG_clip_line(pfl, x1, x2, lwc, lgf);}

/* if X1 -> X2 crosses y = lwc[3] going top to bottom interpolate and recurse */
    else if ((x2[1] <= lwc[3]) && (lwc[3] < x1[1]))
       {tx[1] = lwc[3];
        PM_interp(tx[0], tx[1], x1[1], x1[0], x2[1], x2[0]);
       
        x1[0] = tx[0];
        x1[1] = tx[1];

        *pfl |= 1;

        PG_clip_line(pfl, x1, x2, lwc, lgf);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLIP_LINE_SEG - clip a simple line segment
 *                  - return TRUE iff there is something to draw
 */

int PG_clip_line_seg(PG_device *dev, double *x1, double *x2)
   {int flag, rv;
    double bnd[PG_BOXSZ];
    PG_dev_geometry *g;

    flag = 0;

    if (dev != NULL)
       {g = &dev->g;

/* prepare to clip the line to the window */
	PG_get_viewspace(dev, BOUNDC, bnd);

	if (bnd[1] < bnd[0])
	   {SC_SWAP_VALUE(double, bnd[0], bnd[1]);};

	if (bnd[3] < bnd[2])
	   {SC_SWAP_VALUE(double, bnd[2], bnd[3]);};

	PG_clip_line(&flag, x1, x2, bnd, g->iflog);};

    rv = (flag != 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLIP_BOX - clip a box BX to some WC limits */

int PG_clip_box(double *bx, double *wc, int *lgf)
   {int rv;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];

    x1[0] = bx[0];
    x2[0] = bx[1];
    x1[1] = bx[2];
    x2[1] = bx[3];
    x1[2] = bx[4];
    x2[2] = bx[5];

    rv = 0;
    PG_clip_line(&rv, x1, x2, wc, lgf);

    bx[0] = x1[0];
    bx[1] = x2[0];
    bx[2] = x1[1];
    bx[3] = x2[1];
    bx[4] = x1[2];
    bx[5] = x2[2];

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLIP_POLYGON - clip the data points to the boundary region
 *                 - in general many polygons can result
 *                 - return an array of polygons
 */

SC_array *PG_clip_polygon(PG_device *dev, PM_polygon *py)
   {double bnd[PG_BOXSZ];
    PM_polygon *pb;
    SC_array *a;

/* setup the viewport boundary as a polygon */
    PG_get_viewspace(dev, BOUNDC, bnd);

    if (bnd[1] < bnd[0])
       {SC_SWAP_VALUE(double, bnd[0], bnd[1]);};

    if (bnd[3] < bnd[2])
       {SC_SWAP_VALUE(double, bnd[2], bnd[3]);};

    pb = PM_polygon_box(2, bnd);

    a = PM_intersect_polygons(NULL, py, pb);

    PM_free_polygon(pb);

    return(a);}
                      
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

