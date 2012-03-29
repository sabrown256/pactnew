/*
 * GSOLD.C - old style PGS API routines which are deprecated
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_BOUND - return the WC limits of the viewport boundary */

void PG_get_bound(PG_device *dev,
		  double *xmn, double *xmx, double *ymn, double *ymx)
   {PG_dev_geometry *g;

    DEPRECATED(2009-10-08);

    g = &dev->g;

    *xmn = g->bnd[0];
    *xmx = g->bnd[1];
    *ymn = g->bnd[2];
    *ymx = g->bnd[3];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_WINDOW - return the WC limits of the viewport */

void PG_get_window(PG_device *dev,
		   double *xmn, double *xmx, double *ymn, double *ymx)
   {PG_dev_geometry *g;

    DEPRECATED(2009-10-08);

    g = &dev->g;

    *xmn = g->wc[0];
    *xmx = g->wc[1];
    *ymn = g->wc[2];
    *ymx = g->wc[3];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_FRAME - return the frame limits */

void PG_get_frame(PG_device *dev,
                  double *xmn, double *xmx, double *ymn, double *ymx)
   {PG_dev_geometry *g;

    DEPRECATED(2009-10-08);

    g = &dev->g;

    *xmn = g->fr[0];
    *xmx = g->fr[1];
    *ymn = g->fr[2];
    *ymx = g->fr[3];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_VIEWPORT - return the 2d view port limits */

void PG_get_viewport(PG_device *dev,
                     double *xmn, double *xmx, double *ymn, double *ymx)
   {double frm[PG_BOXSZ];

    DEPRECATED(2009-10-08);

    PG_viewport_frame(dev, 2, frm);

    *xmn = frm[0];
    *xmx = frm[1];
    *ymn = frm[2];
    *ymx = frm[3];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SET_BOUND - set the boundary limits for the viewport in WC
 *              - the axes are drawn on the boundary
 *              - and almost nobody else should use this routine
 */
 
void PG_set_bound(PG_device *dev,
		  double xmn, double xmx, double ymn, double ymx)
   {PG_dev_geometry *g;

    DEPRECATED(2009-10-08);

    g = &dev->g;

    g->bnd[0] = xmn;
    g->bnd[1] = xmx;
    g->bnd[2] = ymn;
    g->bnd[3] = ymx;
 
/* set transformations */
    _PG_fix_wtos(dev, FALSE);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SET_WINDOW - set up the window in WC:
 *               - this is the WC version of the viewport
 *               - when clipping is on it is possible to draw to those
 *               - portions of the screen or window for which
 *               -
 *               -   xmn <= x <= xmx
 *               -   ymn <= y <= ymx
 */
 
void PG_set_window(PG_device *dev,
                   double xmn, double xmx, double ymn, double ymx)
  {double wc[PG_BOXSZ];

    DEPRECATED(2009-10-08);

    wc[0] = xmn;
    wc[1] = xmx;
    wc[2] = ymn;
    wc[3] = ymx;

    PG_set_viewspace(dev, 2, WORLDC, wc);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SET_FRAME - set the frame for the device */
 
void PG_set_frame(PG_device *dev,
                  double x1, double x2, double y1, double y2)
   {double frm[PG_BOXSZ];

    DEPRECATED(2009-10-08);

    frm[0] = x1;
    frm[1] = x2;
    frm[2] = y1;
    frm[3] = y2;

    PG_set_viewspace(dev, 2, FRAMEC, frm);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SET_VIEWPORT - set the viewport for the device
 *                 - the viewport is that part of the window or screen which
 *                 - can be drawn on when clipping is on
 *                 - that is the viewport and clipping window are synonomous
 *                 - view port is specified in NDC
 */
 
void PG_set_viewport(PG_device *dev,
                     double x1, double x2, double y1, double y2)
   {double ndc[PG_BOXSZ];

    DEPRECATED(2009-10-08);

    ndc[0] = x1;
    ndc[1] = x2;
    ndc[2] = y1;
    ndc[3] = y2;

    PG_set_viewspace(dev, 2, NORMC, ndc);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DRAW_LINE - draw a line seqment between the specified points
 *              - in WC
 *              - with log plots this is the cleanest way to handle it
 */
 
void PG_draw_line(PG_device *dev,
                  double x1, double y1, double x2, double y2)
   {double p1[PG_SPACEDM], p2[PG_SPACEDM];

    DEPRECATED(2009-10-08);

    if (dev != NULL)
       {p1[0] = x1;
	p1[1] = y1;
	p2[0] = x2;
	p2[1] = y2;

	if (PG_clip_line_seg(dev, p1, p2))
	   {PG_move_gr_abs_n(dev, p1);
	    PG_draw_to_abs_n(dev, p2);};};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DRAW_POLYLINE - draw a polyline in WC */
 
void PG_draw_polyline(PG_device *dev, double *x, double *y, int n, int clip)
   {int id;
    double xo[PG_SPACEDM];
    double *r[PG_SPACEDM];
    PG_curve *crv;

    DEPRECATED(2009-10-08);

    if (dev == NULL)
       return;

    if (n > 0)
       {for (id = 0; id < 2; id++)
	    xo[id] = 0.0;

        PG_trans_point(dev, 2, PIXELC, xo, WORLDC, xo);
    
	r[0] = x;
	r[1] = y;

	crv = PG_make_curve(dev, WORLDC, FALSE, n, xo, r);

	PG_draw_curve(dev, crv, clip);

	PG_release_curve(crv);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DRAW_BOX_NDC - draw a box around the viewport (NDC) */
 
void PG_draw_box_ndc(PG_device *dev,
                     double xmin, double xmax, double ymin, double ymax)
   {double x[5], y[5];
    double *r[PG_SPACEDM];

    DEPRECATED(2009-10-08);

    x[0] = xmin;
    y[0] = ymin;
    x[1] = xmin;
    y[1] = ymax;
    x[2] = xmax;
    y[2] = ymax;
    x[3] = xmax;
    y[3] = ymin;
    x[4] = xmin;
    y[4] = ymin;

    r[0] = x;
    r[1] = y;

    PG_draw_polyline_n(dev, 2, NORMC, 5, r, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DRAW_BOX - draw a box around the viewport (WC) */
 
void PG_draw_box(PG_device *dev,
                 double xmin, double xmax, double ymin, double ymax)
   {double x[5], y[5];
    double *r[PG_SPACEDM];

    DEPRECATED(2009-10-08);

    x[0] = xmin;
    y[0] = ymin;
    x[1] = xmin;
    y[1] = ymax;
    x[2] = xmax;
    y[2] = ymax;
    x[3] = xmax;
    y[3] = ymin;
    x[4] = xmin;
    y[4] = ymin;

    r[0] = x;
    r[1] = y;

    PG_draw_polyline_n(dev, 2, WORLDC, 5, r, TRUE);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_AXIS_3D - draw a set of axes for a surface plot */

void PG_axis_3d(PG_device *dev, double *px, double *py, double *pz,
		int np, double theta, double phi, double chi,
	        double xmn, double xmx, double ymn, double ymx,
		double zmn, double zmx, int norm)
   {double extr[PG_BOXSZ];
    double *p[PG_SPACEDM];

    DEPRECATED(2009-10-08);

    PG_fset_view_angle(dev, TRUE, &theta, &phi, &chi);

    p[0] = px;
    p[1] = py;
    p[2] = pz;

    extr[0] = xmn;
    extr[1] = xmx;
    extr[2] = ymn;
    extr[3] = ymx;
    extr[4] = zmn;
    extr[5] = zmx;

    PG_axis_3(dev, p, np, extr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_SURFACE - draw a surface plot given a known mesh type, a
 *                 - suitable scanning function, and a scalar array
 *                 - rotate to view angle, plot with hidden line
 *                 - algorithm, and shade if requested
 */

void PG_draw_surface(PG_device *dev, double *a1, double *a2, double *aext,
		     double *x, double *y, int nn,
		     double xmn, double xmx, double ymn, double ymx,
		     double *va, double width,
		     int color, int style, int type, char *name,
		     char *mesh_type, void *cnnct, pcons *alist)
   {double *r[PG_SPACEDM];
    PG_rendering pty;

    DEPRECATED(2009-10-08);

    r[0] = x;
    r[1] = y;

    pty = (PG_rendering) type;

    PG_draw_surface_n(dev, a1, a2, aext, r, nn, va,
		      width, color, style, pty, name,
		      mesh_type, cnnct, alist);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_PALETTE - display the palette next to the image */

void PG_draw_palette(PG_device *dev,
		     double xmn, double ymn, double xmx, double ymx,
		     double zmn, double zmx, double wid)
   {double dbx[PG_BOXSZ], rbx[PG_BOXSZ];

    DEPRECATED(2009-10-08);

    dbx[0] = xmn;
    dbx[1] = xmx;
    dbx[2] = ymn;
    dbx[3] = ymx;
    rbx[0] = zmn;
    rbx[1] = zmx;

    PG_draw_palette_n(dev, dbx, rbx, wid, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_IMAGE - initialize an image */

PG_image *PG_make_image(char *label, char *type, void *z,
			double xmn, double xmx,
			double ymn, double ymx,
			double zmn, double zmx,
			int w, int h, int bpp, PG_palette *palette)
   {double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    PG_image *im;

    DEPRECATED(2009-10-08);

    dbx[0] = xmn;
    dbx[1] = xmx;
    dbx[2] = ymn;
    dbx[3] = ymx;
    rbx[0] = zmn;
    rbx[1] = zmx;

    im = PG_make_image_n(label, type, z, 2, WORLDC,
			 dbx, rbx, w, h, bpp, palette);

    return(im);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_AXIS - draw and label a plot axis */

PG_axis_def *PG_draw_axis(PG_device *dev, double x1, double y1,
			  double x2, double y2,
			  double t1, double t2, double v1, double v2,
			  double sc, char *format,
			  int tick_type, int label_type,
			  int flag)
   {double tn[2], vw[2];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];
    PG_axis_def *ad;

    DEPRECATED(2009-10-08);

    xl[0] = x1;
    xl[1] = y1;
    xr[0] = x2;
    xr[1] = y2;
    tn[0] = t1;
    tn[1] = t2;
    vw[0] = v1;
    vw[1] = v2;

    ad = PG_draw_axis_n(dev, xl, xr, tn, vw, sc, format,
			tick_type, label_type, flag, 0);

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WRITE_NDC - print formatted output
 *              - at the specified coordinates in NDC
 */

int PG_write_NDC(PG_device *dev, double x, double y, char *fmt, ...)
   {int id, logflag;
    int iflg[PG_SPACEDM], nflg[PG_SPACEDM];
    double p[PG_SPACEDM];
    char *s;
    PG_dev_geometry *g;

    DEPRECATED(2009-10-08);

    if (dev == NULL)
       return(FALSE);

    g = &dev->g;

    p[0] = x;
    p[1] = y;

    if (dev->gprint_flag)
       {SC_VDSNPRINTF(TRUE, s, fmt);

	if ((s != NULL) && (*s != '\0'))
	   {logflag = (g->iflog[0] || g->iflog[1]);
	    if (logflag)
	       {PG_fget_axis_log_scale(dev, 2, iflg);
		for (id = 0; id < 2; id++)
		    nflg[id] = FALSE;
		PG_fset_axis_log_scale(dev, 2, nflg);}
       
	    PG_trans_point(dev, 2, NORMC, p, WORLDC, p);

	    PG_move_tx_abs_n(dev, p);
	    PG_write_text(dev, PG_gs.stdscr, s);

	    if (logflag)
	       PG_fset_axis_log_scale(dev, 2, iflg);

#if DEBUG_TEXT
	    {double wc[PG_BOXSZ], dx[PG_SPACEDM];

	     PG_get_text_ext_n(dev, 2, WORLDC, s, dx);
	     wc[0] = p[0];
	     wc[1] = p[0] + dx[0];
	     wc[2] = p[1];
	     wc[3] = p[1] + dx[1];
	     PG_draw_box_n(dev, 2, WORLDC, wc);};
#endif
	   };

	CFREE(s);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WRITE_WC - print formatted output
 *             - at the specified coordinates in WC
 */

int PG_write_WC(PG_device *dev, double x, double y, char *fmt, ...)
   {int rv;
    char *s;
    double p[PG_SPACEDM];

    DEPRECATED(2009-10-08);

    rv = FALSE;

    if ((dev != NULL) && (dev->gprint_flag))
       {SC_VDSNPRINTF(TRUE, s, fmt);

	p[0] = x;
	p[1] = y;
        PG_move_tx_abs_n(dev, p);
        PG_write_text(dev, PG_gs.stdscr, s);

#if DEBUG_TEXT
	{double wc[PG_BOXSZ], dx[PG_SPACEDM];

	 PG_get_text_ext_n(dev, 2, WORLDC, s, dx);
	 wc[0] = x;
	 wc[1] = x + dx[0];
	 wc[2] = y;
	 wc[3] = y + dx[1];
	 PG_draw_box_n(dev, 2, WORLDC, wc);};
#endif

        CFREE(s);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_TEXT_EXT - return the text extent in WC of the given string */

void PG_get_text_ext(PG_device *dev, char *s, double *px, double *py)
   {double t[PG_SPACEDM];
    PG_dev_geometry *g;

    DEPRECATED(2009-10-08);

    PG_get_text_ext_n(dev, 2, NORMC, s, t);

    if (dev != NULL)
       {g = &dev->g;

	t[0] *= g->nd_w[1];
	t[1] *= g->nd_w[3];

/* since this was not given to PG_set_viewspace as logs
 * don't return it that way
 */
	PG_lin_point(dev, 2, t);};

    *px = t[0];
    *py = t[1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_CHAR_SIZE_NDC - return the character size in NDC */

void PG_get_char_size_NDC(PG_device *dev, double *pw, double *ph)
   {

    DEPRECATED(2009-10-08);

    *ph = dev->char_height_s;
    *pw = dev->char_width_s;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_CHAR_SIZE - return the character size in WC */

void PG_get_char_size(PG_device *dev, double *pw, double *ph)
   {

    DEPRECATED(2009-10-08);

    *pw = (dev->char_width_s)*(dev->g.nd_w[1]);
    *ph = (dev->char_height_s)*(dev->g.nd_w[3]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_MARKERS - draw 2d markers in WC */

void PG_draw_markers(PG_device *dev, int n, double *x, double *y, int marker)
   {double *r[PG_SPACEDM];

    DEPRECATED(2009-10-08);

    r[0] = x;
    r[1] = y;

    PG_draw_markers_n(dev, 2, WORLDC, n, r, marker);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_MARKERS_3 - draw 3d markers in WC */

void PG_draw_markers_3(PG_device *dev, int n, double **r, int marker)
   {

    DEPRECATED(2009-10-08);

    PG_draw_markers_n(dev, 3, WORLDC, n, r, marker);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_VCT - draw 2d vectors in WC */

void _PG_draw_vct(PG_device *dev, double **x, double **u, int npts)
   {

    DEPRECATED(2009-10-08);

    PG_draw_vector_n(dev, 2, WORLDC, npts, x, u);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_LIMITS - draw 2d vectors in WC */

void PG_set_limits(PG_device *dev, double *x, double *y, int n, int type)
   {double *r[PG_SPACEDM];
    PG_rendering pty;

    DEPRECATED(2009-10-08);

    r[0] = x;
    r[1] = y;

    pty = (PG_rendering) type;

    PG_set_limits_n(dev, 2, WORLDC, n, r, pty);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FILL_POLYGON - draw a polygon (X,Y) with N nodes
 *                 - and fill the interior with COLOR
 */

void PG_fill_polygon(PG_device *dev, int color, int mapped,
		     double *x, double *y, int n)
   {double *r[2];

    DEPRECATED(2009-10-08);

    r[0] = x;
    r[1] = y;

    PG_fill_polygon_n(dev, color, mapped, 2, WORLDC, n, r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_GET_CLIPPING - return the state of the clipping */

void PG_get_clipping(PG_device *dev, int *flag)
   {

    DEPRECATED(2011-06-12);

    *flag = dev->clipping;

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_CHAR_PATH - return the direction in which text is to be written
 *                  - defaults to (1, 0)
 */

void PG_get_char_path(PG_device *dev, double *px, double *py)
   {

    DEPRECATED(2011-06-12);

    *px = dev->char_path[0];
    *py = dev->char_path[1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_CHAR_UP - inquire about the direction which constitutes up for
 *                - the characters
 *                - defaults to (0, 1)
 */

void PG_get_char_up(PG_device *dev, double *px, double *py)
   {

    DEPRECATED(2011-06-12);

    *px = dev->char_up[0];
    *py = dev->char_up[1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_CHAR_PRECISION - inquire about the character precision
 *                       - fast and fixed size or
 *                       - slow and flexible
 */

void PG_get_char_precision(PG_device *dev, int *pcp)
   {

    DEPRECATED(2011-06-12);

    *pcp = dev->char_precision;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_CHAR_SIZE_N - return the character size in CS */

void PG_get_char_size_n(PG_device *dev, int nd, PG_coord_sys cs, double *p)
   {

    DEPRECATED(2011-06-12);

    p[0] = dev->char_height_s;
    p[1] = dev->char_width_s;

    PG_trans_point(dev, nd, NORMC, p, cs, p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_CHAR_SPACE - inquire about the space
 *                   - to be used between characters
 */

void PG_get_char_space(PG_device *dev, double *pcsp)
   {

    DEPRECATED(2011-06-12);

    *pcsp = dev->char_space;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_FONT - return the current font info */

void PG_get_font(PG_device *dev, char **pf, char **pst, int *psz)
   {

    DEPRECATED(2011-06-12);

    *pf  = CSTRSAVE(dev->type_face);
    *pst = CSTRSAVE(dev->type_style);
    *psz = dev->type_size;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_LOGICAL_OP - return the logical operation */

void PG_get_logical_op(PG_device *dev, PG_logical_operation *plop)
   {

    DEPRECATED(2011-06-12);

    *plop = dev->logical_op;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_LINE_STYLE - return the line style */

void PG_get_line_style(PG_device *dev, int *pl)
   {

    DEPRECATED(2011-06-12);

    *pl = dev->line_style;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_LINE_WIDTH - return the width of a line */

void PG_get_line_width(PG_device *dev, double *plw)
   {

    DEPRECATED(2011-06-12);

    *plw = dev->line_width;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_LINE_COLOR - inquire about the current line color */

void PG_get_line_color(PG_device *dev, int *pcl)
   {

    DEPRECATED(2011-06-12);

    *pcl = dev->line_color;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_TEXT_COLOR - inquire about the text color */

void PG_get_text_color(PG_device *dev, int *pcl)
   {

    DEPRECATED(2011-06-12);

    *pcl = dev->text_color;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_FILL_COLOR - inquire about the fill color */

void PG_get_fill_color(PG_device *dev, int *pcl)
   {

    DEPRECATED(2011-06-12);

    *pcl = dev->fill_color;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

