/*
 * GSPR_X.C - PGS X primitive routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

extern SC_array
 *_PG_X_point_list;

/*--------------------------------------------------------------------------*/

/*                         STATE QUERY ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PG_X_GET_TEXT_EXT - return the text extent in CS
 *                    - of the given string
 */

void _PG_X_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p)
   {int len, dir, asc, dsc;
    XCharStruct overall;
    XFontStruct *fnt;
    Display *disp;

    disp = dev->display;
    if (disp == NULL)
       return;

/* NOTE: these make ZeroFault happy */
    dir = 0;
    asc = 0;
    dsc = 0;
    SC_MEM_INIT(XCharStruct, &overall);
/*
    overall.lbearing   = 0;
    overall.rbearing   = 0;
    overall.width      = 0;
    overall.ascent     = 0;
    overall.descent    = 0;
    overall.attributes = 0;
*/
    len = strlen(s);
    fnt = dev->font_info;
    XTextExtents(fnt, s, len, &dir, &asc, &dsc, &overall);
    
    p[0] = overall.width;
    p[1] = asc + dsc;
    p[2] = 0.0;

    PG_trans_interval(dev, nd, PIXELC, p, cs, p);

    return;}

/*--------------------------------------------------------------------------*/

/*                          STATE CHANGE ROUTINES                           */

/*--------------------------------------------------------------------------*/
 
/* _PG_X_SET_LOGICAL_OP - set the logical operation */
 
void _PG_X_set_logical_op(PG_device *dev, PG_logical_operation lop)
   {Display *disp;
    GC xgc;

    dev->logical_op = lop;

    disp = dev->display;
    if (disp == NULL)
       return;

    xgc = dev->gc;

    switch (lop)
       {case GS_XOR :
             XSetFunction(disp, xgc, GXxor);
             break;

        default      :
	case GS_COPY :
	     XSetFunction(disp, xgc, GXcopy);
             break;};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_SET_LINE_STYLE - set the line style */
 
void _PG_X_set_line_style(PG_device *dev, int style)
   {int ls, n;
    char dl[32];
    Display *disp;
    GC xgc;

    dev->line_style = style;

    disp = dev->display;
    if (disp == NULL)
       return;

    xgc   = dev->gc;
    switch (style)
       {case LINE_DASHED    :
             ls = LineOnOffDash;
             n  = 2;
             dl[0] = 7;
             dl[1] = 3;
             XSetDashes(disp, xgc, 0, dl, n);
             break;

        case LINE_DOTTED    :
             ls = LineOnOffDash;
             n  = 2;
             dl[0] = 3;
             dl[1] = 3;
             XSetDashes(disp, xgc, 0, dl, n);
             break;

        case LINE_DOTDASHED :
             ls = LineOnOffDash;
             n  = 4;
             dl[0] = 7;
             dl[1] = 3;
             dl[2] = 3;
             dl[3] = 3;
             XSetDashes(disp, xgc, 0, dl, n);
             break;

       case LINE_SOLID     :
       default             :
             ls = LineSolid;
             break;};

    dev->X_line_style = ls;
    XSetLineAttributes(disp, xgc, dev->X_line_width, 
                       ls, dev->X_cap_style,
                       dev->X_join_style);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_SET_LINE_WIDTH - set the line width */
 
void _PG_X_set_line_width(PG_device *dev, double width)
   {unsigned int lwd;
    Display *disp;

    dev->line_width = width;

    disp = dev->display;
    if (disp != NULL)
       {lwd               = 3.0*max(width, 0.0);
	dev->X_line_width = lwd;
	XSetLineAttributes(disp, dev->gc, lwd,
			   dev->X_line_style,
			   dev->X_cap_style,
			   dev->X_join_style);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_SET_LINE_COLOR - set the line color */
 
void _PG_X_set_line_color(PG_device *dev, int color, int mapped)
   {unsigned long *pi;
    Display *disp;

    dev->line_color = color;

    disp = dev->display;
    if (disp != NULL)
       {if ((mapped == TRUE) || (PG_is_true_color(color) == TRUE))
	   color = _PG_trans_color(dev, color);

	pi = dev->current_palette->pixel_value;
	XSetForeground(disp, dev->gc, pi[color]);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_SET_TEXT_COLOR - set the color of the text */
 
void _PG_X_set_text_color(PG_device *dev, int color, int mapped)
   {unsigned long *pi;
    Display *disp;

    dev->text_color = color;

    disp = dev->display;
    if (disp != NULL)
       {if ((mapped == TRUE) || (PG_is_true_color(color) == TRUE))
	   color = _PG_trans_color(dev, color);

	pi = dev->current_palette->pixel_value;
	XSetForeground(disp, dev->gc, pi[color]);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_SET_FILL_COLOR - sets current fill color */

void _PG_X_set_fill_color(PG_device *dev, int color, int mapped)
   {int dc;
    unsigned long *pi;
    Display *disp;

    dev->fill_color = color;

    disp = dev->display;
    if (disp != NULL)
       {if (PG_is_true_color(color) == TRUE)
	   {color = _PG_trans_color(dev, color);
	    pi    = dev->color_table->pixel_value;
	    dc    = pi[color];}

	else if (mapped == TRUE)
	   {pi = dev->current_palette->pixel_value;
	    dc = pi[color];}
        else
	   dc = color;

	XSetForeground(disp, dev->gc, dc);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_SET_CHAR_SIZE - set the character size in CS */

void _PG_X_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x)
   {double p[PG_SPACEDM];

    if ((x != NULL) && (nd >= 2))
       {PG_trans_point(dev, nd, cs, x, NORMC, p);

	dev->char_height_s = p[0];
	dev->char_width_s  = p[1];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_SET_CHAR_PATH - set the direction along which text will be written
 *                     - defaults to (1, 0)
 */

void _PG_X_set_char_path(PG_device *dev, double x, double y)
   {

    dev->char_path[0] = x;
    dev->char_path[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_SET_CHAR_PRECISION - set the character precision
 *                          - fast and fixed size or
 *                          - slow and flexible
 */

void _PG_X_set_char_precision(PG_device *dev, int p)
   {

    dev->char_precision = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_SET_CHAR_SPACE - set the space between characters */

void _PG_X_set_char_space(PG_device *dev, double s)
   {

    dev->char_space = (double) s;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_SET_CHAR_UP - set the direction which is up for individual
 *                   - characters
 *                   - defaults to (0, 1)
 */

void _PG_X_set_char_up(PG_device *dev, double x, double y)
   {

    dev->char_up[0] = x;
    dev->char_up[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_SET_CHAR_LINE - set the number characters per line */
 
void _PG_X_set_char_line(PG_device *dev, int n)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_SET_CLIPPING - set clipping
 *                    - flag = FALSE  -->  clipping off
 *                    - flag = TRUE   -->  clipping on
 */

void _PG_X_set_clipping(PG_device *dev, bool flag)
   {int iw, ih;
    int pc[PG_BOXSZ];
    XRectangle cliprect[1];
    Display *disp;

    disp = dev->display;
    if (disp == NULL)
       return;

    _PG_find_clip_region(dev, pc, flag, FALSE);

    iw = pc[1] - pc[0] + 1;
    ih = pc[3] - pc[2] + 1;
      
    cliprect[0].x      = 0;
    cliprect[0].y      = 0;
    cliprect[0].width  = iw;
    cliprect[0].height = ih;
 
    PG_QUAD_FOUR_BOX(dev, pc);

    XSetClipRectangles(disp, dev->gc, pc[0], pc[2], cliprect, 1, Unsorted);
 
    dev->clipping = flag;

    return;}
 
/*--------------------------------------------------------------------------*/

/*                          MOVE AND DRAW ROUTINES                          */

/*--------------------------------------------------------------------------*/
 
/* _PG_X_MOVE_GR_ABS - move the current graphics cursor position to the
 *                   - given absolute coordinates in WC
 */
 
void _PG_X_move_gr_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_MOVE_TX_ABS - move the current text cursor position to the
 *                   - given coordinates in WC
 */
 
void _PG_X_move_tx_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_abs(dev, p);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_MOVE_TX_REL - move the current text cursor position to the
 *                   - given relative coordinates in WC
 */
 
void _PG_X_move_tx_rel(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_rel(dev, p);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_DRAW_TO_ABS - draw a line from current position to
 *                   - absolute position (x, y)
 *                   - in WC
 */
 
void _PG_X_draw_to_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM], o[PG_SPACEDM];
    Display *disp;

    if (dev != NULL)
       {disp = dev->display;
	if (disp == NULL)
	   return;
 
	o[0] = dev->gcur[0];
	o[1] = dev->gcur[1];

	p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, o, PIXELC, o);
	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	PG_QUAD_FOUR_POINT(dev, o);
	PG_QUAD_FOUR_POINT(dev, p);

	XDrawLine(disp, PG_X11_DRAWABLE(dev), dev->gc,
		  o[0], o[1], p[0], p[1]);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_DRAW_TO_REL - draw a line from current position to
 *                   - relative position (x, y)
 *                   - in WC
 */
 
void _PG_X_draw_to_rel(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM], o[PG_SPACEDM];
    Display *disp;

    if (dev != NULL)
       {disp = dev->display;
	if (disp == NULL)
	   return;

	o[0] = dev->gcur[0];
	o[1] = dev->gcur[1];

	p[0] = x;
	p[1] = y;

	_PG_move_gr_rel(dev, p);

	PG_trans_point(dev, 2, WORLDC, o, PIXELC, o);
	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);

	PG_QUAD_FOUR_POINT(dev, o);
	PG_QUAD_FOUR_POINT(dev, p);

	XDrawLine(disp, PG_X11_DRAWABLE(dev), dev->gc,
		  o[0], o[1], p[0], p[1]);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_DRAW_CURVE - plot the points from a PG_curve */
 
void _PG_X_draw_curve(PG_device *dev, PG_curve *crv, int clip)
   {int i, n, j, nmax;
    int p[PG_SPACEDM], xo[PG_SPACEDM];
    int *x[PG_SPACEDM];
    XPoint *pts;
    Display *disp;
    Window wind;
    GC gc;

    disp = dev->display;
    if (disp == NULL)
       return;

    wind = PG_X11_DRAWABLE(dev);
    gc   = dev->gc;

    n     = crv->n;
    x[0]  = crv->x;
    x[1]  = crv->y;
    xo[0] = crv->x_origin;
    xo[1] = crv->y_origin;
    
    nmax = (XMaxRequestSize(disp) - 3L) >> 1L;
    pts  = CMAKE_N(XPoint, nmax);
    for (i = 0; i < n; i += nmax)
        {if (i+nmax > n)
	    nmax = n - i;

	 for (j = 0; j < nmax; j++)
             {p[0] = *x[0]++ + xo[0];
	      p[1] = *x[1]++ + xo[1];

	      PG_QUAD_FOUR_POINT(dev, p);

	      pts[j].x = p[0];
	      pts[j].y = p[1];};

	 XDrawLines(disp, wind, gc, pts, nmax, CoordModeOrigin);};
    
    CFREE(pts);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_DRAW_DISJOINT_POLYLINE_2 - draws disjoint two dimensional
 *                                - line segments specified in CS
 */

void _PG_X_draw_disjoint_polyline_2(PG_device *dev, double **r, long n,
                                    int flag, PG_coord_sys cs)
   {int i, j, k, nmax, cflag;
    int *lgf;
    short ix1[PG_SPACEDM], ix2[PG_SPACEDM];
    double box[PG_BOXSZ], wc[PG_BOXSZ];
    double *px, *py;
    XSegment *segs;
    Display *disp;
    Window wind;
    GC gc;
    PG_dev_geometry *g;

    if (dev == NULL)
       return;

    g = &dev->g;

    disp = dev->display;
    if ((disp == NULL) || (n == 0))
       return;

    SC_MEM_INIT_V(box);
    SC_MEM_INIT_V(wc);

    nmax = (XMaxRequestSize(disp) - 3L) >> 1L;
    segs = CMAKE_N(XSegment, nmax);
    wind = PG_X11_DRAWABLE(dev);

    gc  = dev->gc;
    lgf = g->iflog;

    PG_trans_points(dev, 2*n, 2, cs, r, WORLDC, r);

    px = r[0];
    py = r[1];

    if (dev->clipping)
       PG_get_viewspace(dev, WORLDC, wc);
    else
       PG_get_limit(dev, WORLDC, wc);

    PG_log_space(dev, 2, FALSE, wc);

    for (i = 0; i < n; i += nmax)
        {if (i+nmax > n)
            nmax = n - i;
       
	 SC_MEM_INIT_N(XSegment, segs, nmax);

         for (k = 0, j = 0; j < nmax; j++)
             {box[0] = *px++;
              box[2] = *py++;
              box[1] = *px++;
              box[3] = *py++;

              if (cs == WORLDC)
		 PG_log_space(dev, 2, FALSE, box);

              cflag = PG_clip_box(box, wc, lgf);
              if (cflag == 0)
                 continue;

	      PG_trans_box(dev, 2, WORLDC, box, PIXELC, box);

	      ix1[0] = box[0];
	      ix1[1] = box[2];
	      ix2[0] = box[1];
	      ix2[1] = box[3];

	      PG_QUAD_FOUR_POINT(dev, ix1);
	      PG_QUAD_FOUR_POINT(dev, ix2);

	      if ((ix1[0] != ix2[0]) || (ix1[1] != ix2[1]))
		 {segs[k].x1 = ix1[0];
		  segs[k].y1 = ix1[1];
		  segs[k].x2 = ix2[0];
		  segs[k].y2 = ix2[1];
		  k++;};};
       
         XDrawSegments(disp, wind, gc, segs, k);
         XFlush(disp);};
    
    CFREE(segs);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_SHADE_POLY - polygon shading routine */

void _PG_X_shade_poly(PG_device *dev, int nd, int n, double **r)
   {int i;
    double ix[PG_SPACEDM];
    XPoint p, *points;
    Display *disp;

    disp = dev->display;
    if (disp == NULL)
       return;

    if (nd == 3)
       _PG_rst_shade_poly(dev, nd, n, r);

    else

/* assume the points are given in counter-clockwise order */
       {for (i = 0; i < n; i++)
	    {ix[0] = r[0][i];
	     ix[1] = r[1][i];
       
	     PG_trans_point(dev, 2, WORLDC, ix, PIXELC, ix);

	     PG_QUAD_FOUR_POINT(dev, ix);

	     p.x = ix[0];
	     p.y = ix[1];
	     SC_array_push(_PG_X_point_list, &p);};

	points = SC_array_array(_PG_X_point_list);

	SC_array_set_n(_PG_X_point_list, 0);

	XFillPolygon(disp, PG_X11_DRAWABLE(dev), dev->gc, points, n,
		     Nonconvex, CoordModeOrigin);

	CFREE(points);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_FILL_CURVE - fill closed curves */

void _PG_X_fill_curve(PG_device *dev, PG_curve *crv)
   {int i, n;
    int ir[PG_SPACEDM], xo[PG_SPACEDM];
    int *x[PG_SPACEDM];
    XPoint p, *points;
    Display *disp;

    disp = dev->display;
    if (disp == NULL)
       return;

    n     = crv->n;
    x[0]  = crv->x;
    x[1]  = crv->y;
    xo[0] = crv->x_origin;
    xo[1] = crv->y_origin;
    
/* assume the points are given in counter-clockwise order */
    for (i = 0; i < n; i++)
        {ir[0] = x[0][i] + xo[0];
	 ir[1] = x[1][i] + xo[1];
	 PG_QUAD_FOUR_POINT(dev, ir);

	 p.x = ir[0];
	 p.y = ir[1];

	 SC_array_push(_PG_X_point_list, &p);};

    points = SC_array_array(_PG_X_point_list);

    SC_array_set_n(_PG_X_point_list, 0);

    XFillPolygon(disp, PG_X11_DRAWABLE(dev), dev->gc, points, n,
                 Nonconvex, CoordModeOrigin);

    CFREE(points);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_PUT_IMAGE - put the image on the screen
 *                 - the image buffer may be overwritten by the pseudo color
 *                 - mapping if it is needed!!
 */

void _PG_X_put_image(PG_device *dev, unsigned char *bf,
                     int ix, int iy, int nx, int ny)
   {int i, j, k, l, n;
    int pbx[PG_BOXSZ];
    unsigned long cv;
    unsigned long *pixel_value;
    unsigned char *ibf;
    XImage *xim;
    Display *disp;
    double rv;
    int n_pal_colors, n_dev_colors;
    PG_palette *pal;
    RGB_color_map *pseudo_cm;

    disp = dev->display;
    if (disp == NULL)
       return;

    pbx[0] = ix;
    pbx[1] = ix + nx;
    pbx[2] = iy;
    pbx[3] = iy + ny;

    ibf          = NULL;
    pal          = dev->current_palette;
    n_dev_colors = dev->absolute_n_color;
    n_pal_colors = pal->n_pal_colors;
    pseudo_cm    = pal->pseudo_colormap;

    if ((n_dev_colors < n_pal_colors) && (pseudo_cm != NULL))
       {unsigned char *pbf;

        pbf = bf;
        PM_random(-1);
        for (l = 0; l < ny; l++)
            for (k = 0; k < nx; k++)
                {i      = *pbf;
                 rv     = 3.5*PM_random(1) - 0.83;
                 *pbf++ = (rv < pseudo_cm[i].red) ?
                          pseudo_cm[i].green : pseudo_cm[i].blue;};};

/* compress a zpixmap to a zbitmap for black and white systems - X bug */
    if (n_dev_colors < 3)
       {_PG_byte_bit_map(bf, nx, ny, FALSE);

        xim = XCreateImage(disp, DefaultVisual(disp, XDefaultScreen(disp)),
			   DisplayPlanes(disp, XDefaultScreen(disp)),
			   XYPixmap, 0,
			   (char *) bf,
			   (unsigned int) nx, (unsigned int) ny,
			   8, 0);}
    else
       {int id, pd, bw, bpi;
	long nb;
        
	pd  = XDefaultDepth(dev->display, DefaultScreen(dev->display));
	bw  = pd/8;
	id  = SC_type_container_size(KIND_INT, bw);
        bpi = SC_type_size_i(id);

        PG_invert_image_data(bf, nx, ny, 1);

/* map the pixel values to the colors */
        n = nx*ny;

        if (bw == 1)
           nb = n;
        else
	   nb = n*bpi;

	ibf = CMAKE_N(unsigned char, nb);

	xim = XCreateImage(disp, DefaultVisual(disp, XDefaultScreen(disp)),
			   pd,
			   ZPixmap, 0,
			   (char *) ibf,
			   (unsigned int) nx, (unsigned int) ny,
			   8, 0);
        
        pixel_value = pal->pixel_value;

        if (bw == 1)  /* 8 bit color */
           {for (i = 0; i < n; i++)
                ibf[i] = pixel_value[bf[i]];}
        else
           {k = 0;
            for (j = 0; j < ny; j++)           
	        for (i = 0; i < nx; i++)
		    {cv = pixel_value[bf[k++]];
		     XPutPixel(xim, i, j, cv);};};};

    PG_QUAD_FOUR_BOX(dev, pbx);

    XPutImage(disp, PG_X11_DRAWABLE(dev), dev->gc, xim, 0, 0,
	      pbx[0], pbx[2], nx, ny);
    XFlush(disp);
    xim->data = NULL;

    if (ibf)
       CFREE(ibf);

    XDestroyImage(xim);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_GET_IMAGE - get the image on the screen into
 *                 - the image buffer
 */

void _PG_X_get_image(PG_device *dev, unsigned char *bf,
                     int ix, int iy, int nx, int ny)
   {return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_PUT_RASTER - put (NX,NY) of the raster device frame on the screen
 *                  - at (X0,Y0) rotated by (CA,SA)
 */

int _PG_X_put_raster(PG_device *dev, int bc,
		     int xo, int yo, int nx, int ny,
		     double ca, double sa)
   {int ix, iy;
    int w, h, ok, bcp, nb, nv;
    int pxsd, bprd, l, ri, gi, bi;
    int ir[PG_SPACEDM];
    unsigned char *db, *dr;
    unsigned long msk;
    PG_RAST_device *mdv;
    RGB_color_map bcr;
    frame *fr;
    Display *disp;
    XImage *di;

    disp = dev->display;
    if (disp == NULL)
       return(FALSE);

    GET_RAST_DEVICE(dev, mdv);
    if (mdv == NULL)
       return(FALSE);

    fr = mdv->raster;
    if ((fr == NULL) || (fr->n_on < 1))
       return(FALSE);

    w = fr->width;
    h = fr->height;

    if (nx < 0)
       {if (fr->box[1] <= fr->box[0])
	   {xo = 0;
	    nx = w;}
	else
	   {xo = fr->box[0];
	    nx = fr->box[1] - xo + 1;};};
    if (ny < 0)
       {if (fr->box[3] <= fr->box[2])
	   {yo = 0;
	    ny = h;}
	else
	   {yo = fr->box[2];
	    ny = fr->box[3] - yo + 1;};};

    msk = AllPlanes;
    di  = XGetImage(disp, dev->window, 0, 0, w, h, msk, ZPixmap);

    pxsd = di->bits_per_pixel >> 3;
    bprd = di->bytes_per_line;
    db   = (unsigned char *) di->data;

    bcr = fr->bc;

    nb = 0;
    nv = 0;
    for (iy = 0; iy < ny; iy++)
        {for (ix = 0; ix < nx; ix++)
	     {l  = (yo + iy)*w + xo + ix;

	      ri = fr->r[l];
	      gi = fr->g[l];
	      bi = fr->b[l];

	      ir[0] = xo + ix*ca + iy*sa;
	      ir[1] = yo - ix*sa + iy*ca;

	      dr = db + bprd*ir[1] + pxsd*ir[0];

/* is pixel background color */
	      bcp = ((ri == bcr.red) && (gi == bcr.green) && (bi == bcr.blue));

	      nb += bcp;

/* do not write pixels in background color if it is white or black */
	      if (bcp == FALSE)
		 {nv++;
		  dr[0] = bi;
		  dr[1] = gi;
		  dr[2] = ri;};};};

    ok = XPutImage(disp, dev->window, dev->gc, di, 0, 0, 0, 0, w, h);

    XFlush(disp);
    XDestroyImage(di);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_PUT_XIMAGE - put (NX,NY) of the XImage on the screen
 *                  - at (X0,Y0) rotated by (CA,SA)
 *                  - if INV is TRUE invert the image
 *                  - images from other X windows (such as text)
 *                  - do not need to be inverted
 *                  - image from files generally do need to be inverted
 */

int _PG_X_put_ximage(PG_device *dev, void *psi, int bc, int swc, int sbc,
		     int *xo, int *nx, double ca, double sa, int inv)
   {int id, nd, ix, iy, ok;
    int wow, bob, ri, gi, bi;
    int pxss, bprs, pxsd, bprd;
    int ir[PG_SPACEDM], ex[PG_SPACEDM];
    unsigned long msk;
    unsigned char *sr, *dr, *sb, *db;
    XImage *di, *si;

    nd = 2;
    si = (XImage *) psi;

    ex[0] = PG_window_width(dev);
    ex[1] = PG_window_height(dev);

    for (id = 0; id < nd; id++)
        {if (nx[id] < 0)
	    nx[id] = ex[id];};

    msk = AllPlanes;
    di  = XGetImage(dev->display, PG_X11_DRAWABLE(dev),
		    0, 0, ex[0], ex[1], msk, ZPixmap);

    pxss = si->bits_per_pixel >> 3;
    bprs = si->bytes_per_line;
    sb   = (unsigned char *) si->data;

    pxsd = di->bits_per_pixel >> 3;
    bprd = di->bytes_per_line;
    db   = (unsigned char *) di->data;

    for (iy = 0; iy < nx[1]; iy++)
        {sr = sb + bprs*iy;
	 for (ix = 0; ix < nx[0]; ix++)
	     {bi = sr[0];
	      gi = sr[1];
	      ri = sr[2];

	      ir[0] = xo[0] + ix*ca + iy*sa;
	      ir[1] = xo[1] - ix*sa + iy*ca;

	      if (inv == TRUE)
		 PG_QUAD_FOUR_POINT(dev, ir);

	      dr = db + bprd*ir[1] + pxsd*ir[0];

/* white on white */
	      wow = ((bc == swc) &&
		     (ri == 0xff) && (gi == 0xff) && (bi == 0xff));

/* black on black */
	      bob = ((bc == sbc) &&
		     (ri == 0) && (gi == 0) && (bi == 0));

/* do not write pixels in background color if it is white or black */
	      if ((wow == FALSE) && (bob == FALSE))
		 {dr[0] = bi;
		  dr[1] = gi;
		  dr[2] = ri;};

	      sr += pxss;};};

    ok = XPutImage(dev->display, PG_X11_DRAWABLE(dev), dev->gc,
		   di, 0, 0, 0, 0, ex[0], ex[1]);

    XDestroyImage(di);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

