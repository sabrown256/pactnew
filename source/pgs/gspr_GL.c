/*
 * GSPR_GL.C - PGS GL primitive routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

/*--------------------------------------------------------------------------*/

/*                         STATE QUERY ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PG_GL_GET_TEXT_EXT - return the text extent in CS
 *                     - of the given string
 */

void _PG_GL_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p)
   {int len, dir, asc, dsc;
    XCharStruct overall;
    XFontStruct *fnt;
    Display *disp;

    disp = dev->display;
    if (disp == NULL)
       return;

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
 
/* _PG_GL_SET_LOGICAL_OP - set the logical operation */
 
void _PG_GL_set_logical_op(PG_device *dev, PG_logical_operation lop)
   {Display *disp;
    GC xgc;

    dev->logical_op = lop;

    disp = dev->display;
    if (disp == NULL)
       return;

    xgc = dev->gc;

    glEnable(GL_LOGIC_OP);

    switch (lop)
      {case GS_XOR :
	    glLogicOp(GL_XOR);
	    XSetFunction(disp, xgc, GXxor);
	    break;

       default      :
       case GS_COPY :
            glLogicOp(GL_COPY);     
	    glDisable(GL_LOGIC_OP);
	    XSetFunction(disp, xgc, GXcopy);
	    break;};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_SET_LINE_STYLE - set the line style */
 
void _PG_GL_set_line_style(PG_device *dev, int style)
   {GLushort patt;

    dev->line_style = style;

    glEnable(GL_LINE_STIPPLE);

    switch (style)
      {case LINE_DASHED :
	    patt = 0xF0F0;
	    break;

       case LINE_DOTTED :
	    patt = 0x8888;
	    break;

       case LINE_DOTDASHED :
	    patt = 0xD4D4;
	    break;

       default         :
       case LINE_SOLID :
	    patt = 0xFFFF;
	    glDisable(GL_LINE_STIPPLE);
	    break;};
 
    glLineStipple(1, patt);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_SET_LINE_WIDTH - set the line width */
 
void _PG_GL_set_line_width(PG_device *dev, double width)
   {GLdouble lwd;

    dev->line_width = width;

    lwd = 3.0*max(width, 0.3333333333);
    glLineWidth(lwd);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_SET_LINE_COLOR - set the line color */
 
void _PG_GL_set_line_color(PG_device *dev, int clr, int mapped)
   {

    dev->line_color = clr;
    if (mapped)
       clr = _PG_trans_color(dev, clr);

    glIndexi((GLint) dev->current_palette->pixel_value[clr]);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_SET_TEXT_COLOR - set the color of the text */
 
void _PG_GL_set_text_color(PG_device *dev, int clr, int mapped)
   {unsigned long *pi;
    Display *disp;

    dev->text_color = clr;

    disp = dev->display;
    if (disp == NULL)
       return;

    if (mapped)
       clr = _PG_trans_color(dev, clr);

    pi = dev->current_palette->pixel_value;
    glIndexi((GLint) pi[clr]);
    XSetForeground(disp, dev->gc, pi[clr]);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_SET_FILL_COLOR - sets current fill color */

void _PG_GL_set_fill_color(PG_device *dev, int clr, int mapped)
   {

    dev->fill_color = clr;
    if (mapped)
       glIndexi((GLint) dev->current_palette->pixel_value[clr]);
    else
       glIndexi((GLint) clr);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_SET_CHAR_SIZE - set the character size in CS */

void _PG_GL_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x)
   {double p[PG_SPACEDM];

    PG_trans_point(dev, nd, cs, x, NORMC, p);

    dev->char_height_s = p[0];
    dev->char_width_s  = p[1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_SET_CHAR_PATH - set the direction along which text will be written
 *                      - defaults to (1, 0)
 */

void _PG_GL_set_char_path(PG_device *dev, double x, double y)
   {

    dev->char_path[0] = x;
    dev->char_path[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_SET_CHAR_PRECISION - set the character precision
 *                           - fast and fixed size or
 *                           - slow and flexible
 */

void _PG_GL_set_char_precision(PG_device *dev, int p)
   {

    dev->char_precision = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_SET_CHAR_SPACE - set the space between characters */

void _PG_GL_set_char_space(PG_device *dev, double s)
   {

    dev->char_space = (double) s;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_SET_CHAR_UP - set the direction which is up for individual
 *                    - characters
 *                    - defaults to (0, 1)
 */

void _PG_GL_set_char_up(PG_device *dev, double x, double y)
   {

    dev->char_up[0] = x;
    dev->char_up[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_SET_CHAR_LINE - set the number characters per line */
 
void _PG_GL_set_char_line(PG_device *dev, int n)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_SET_CLIPPING - set clipping
 *                     - flag = FALSE  -->  clipping off
 *                     - flag = TRUE   -->  clipping on
 */

void _PG_GL_set_clipping(PG_device *dev, int flag)
   {int iw, ih;
    int pc[PG_BOXSZ];
    XRectangle cliprect[1];
    Display *disp;
    double wc[PG_BOXSZ];

    disp = dev->display;
    if (disp == NULL)
       return;

    _PG_find_clip_region(dev, pc, flag, FALSE);

    iw = pc[1] - pc[0] + 1;
    ih = pc[3] - pc[2] + 1;

    cliprect[0].x = 0;
    cliprect[0].y = 0;
    cliprect[0].width  = iw;
    cliprect[0].height = ih;
 
    PG_QUAD_FOUR_BOX(dev, pc);

    XSetClipRectangles(disp, dev->gc, pc[0], pc[2], cliprect, 1, Unsorted);

    glXMakeCurrent(dev->display, PG_X11_DRAWABLE(dev), dev->glxgc);
    glLoadIdentity();
    if (flag)
       {PG_log_space(dev, 2, TRUE, wc);
	PG_trans_box(dev, 2, WORLDC, wc, NORMC, wc);
	glOrtho(wc[0], wc[1], wc[2], wc[3], -1.0, 1.0);}

    else
       glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

    glViewport(pc[0], PG_window_height(dev) - ih - pc[2] + 1, iw, ih);

    dev->clipping = flag;

    return;}
 
/*--------------------------------------------------------------------------*/

/*                          MOVE AND DRAW ROUTINES                          */

/*--------------------------------------------------------------------------*/
 
/* _PG_GL_MOVE_GR_ABS - move the current graphics cursor position to the
 *                    - given absolute coordinates in WC
 */
 
void _PG_GL_move_gr_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_MOVE_TX_ABS - move the current text cursor position to the
 *                    - given coordinates in WC
 */
 
void _PG_GL_move_tx_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_abs(dev, p);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_MOVE_TX_REL - move the current text cursor position to the
 *                    - given relative coordinates in WC
 */
 
void _PG_GL_move_tx_rel(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_rel(dev, p);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_DRAW_TO_ABS - draw a line from current position to
 *                    - absolute position (x, y) in WC
 */
 
void _PG_GL_draw_to_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM], o[PG_SPACEDM];

    if (dev != NULL)
       {o[0] = dev->gcur[0];
	o[1] = dev->gcur[1];

	p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, o, NORMC, o);
	PG_trans_point(dev, 2, WORLDC, p, NORMC, p);

	PG_QUAD_FOUR_POINT(dev, o);
	PG_QUAD_FOUR_POINT(dev, p);

	glBegin(GL_LINE_STRIP);
	glVertex2d((GLdouble) o[0], (GLdouble) o[1]);
	glVertex2d((GLdouble) p[0], (GLdouble) p[1]);
	glEnd();};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_DRAW_TO_REL - draw a line from current position to
 *                    - relative position (x, y)
 *                    - in WC
 */
 
void _PG_GL_draw_to_rel(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM], o[PG_SPACEDM];

    if (dev != NULL)
       {o[0] = dev->gcur[0];
	o[1] = dev->gcur[1];

	p[0] = x;
	p[1] = y;

	_PG_move_gr_rel(dev, p);

	PG_trans_point(dev, 2, WORLDC, o, NORMC, o);
	PG_trans_point(dev, 2, WORLDC, p, NORMC, p);

	PG_QUAD_FOUR_POINT(dev, o);
	PG_QUAD_FOUR_POINT(dev, p);

	glBegin(GL_LINE_STRIP);
	glVertex2d(o[0], o[1]);
	glVertex2d(p[0], p[1]);
	glEnd();};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_DRAW_CURVE - plot the points from a PG_curve */
 
void _PG_GL_draw_curve(PG_device *dev, PG_curve *crv, int clip)
   {int n, j, xo, yo;
    int *x, *y;
    double p[PG_SPACEDM];
    GLdouble v[2];

    n  = crv->n;
    x  = crv->x;
    y  = crv->y;
    xo = crv->x_origin;
    yo = crv->y_origin;
    
    glBegin(GL_LINE_STRIP);
    for (j = 0; j < n; j++)
        {p[0] = xo + x[j];
	 p[1] = yo + y[j];

	 PG_QUAD_FOUR_POINT(dev, p);

	 PG_trans_point(dev, 2, PIXELC, p, NORMC, p);

	 v[0] = p[0];
	 v[1] = p[1];

	 glVertex2dv(v);};

    glEnd();
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_DRAW_DISJOINT_POLYLINE_2 - draws disjoint two dimensional
 *                                 - line segments specified in CS
 */

void _PG_GL_draw_disjoint_polyline_2(PG_device *dev, double **r,
				     long n, int flag, PG_coord_sys cs)
   {int j;
    double wc[PG_BOXSZ];
    double *px, *py;
    GLdouble v1[2], v2[2];

    if (n == 0)
       return;

    PG_trans_points(dev, 2*n, 2, cs, r, WORLDC, r);

    px = r[0];
    py = r[1];

    for (j = 0; j < n; j++)
        {wc[0] = *px++;
	 wc[2] = *py++;
	 wc[1] = *px++;
	 wc[3] = *py++;

	 if (cs == WORLDC)
	    PG_log_space(dev, 2, FALSE, wc);

	 PG_trans_box(dev, 2, WORLDC, wc, NORMC, wc);
 
         v1[0] = wc[0];
	 v1[1] = wc[2];
	 v2[0] = wc[1];
	 v2[1] = wc[3];

	 glBegin(GL_LINES);
	 glVertex2dv(v1);
	 glVertex2dv(v2);
	 glEnd();};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_SHADE_POLY - polygon shading routine */

void _PG_GL_shade_poly(PG_device *dev, int nd, int n, double **r)
   {int i;
    double ix[PG_SPACEDM];
    GLdouble v[2];
    GLenum mode;

    if (nd == 3)
       _PG_rst_shade_poly(dev, nd, n, r);

    else
       {n--;

/* GOTCHA: This only works for convex polygons - tessellation needed */

	if (n == 3)
	   mode = GL_TRIANGLES;
	else if (n == 4)
	   mode = GL_QUADS;
	else
	   mode = GL_POLYGON;

	glBegin(mode);

	for (i = 0; i < n; i++)
	    {ix[0] = r[0][i];
	     ix[1] = r[1][i];

	     PG_trans_point(dev, 2, WORLDC, ix, NORMC, ix);

	     v[0] = ix[0];
	     v[1] = ix[1];
	     glVertex2dv(v);};

	glEnd();};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_FILL_CURVE - fill closed curves */

void _PG_GL_fill_curve(PG_device *dev, PG_curve *crv)
   {int i, n;
    int *r[PG_SPACEDM];
    double ix[PG_SPACEDM];
    GLdouble v[2];
    GLenum mode;

    n     = crv->n - 1;
    r[0]  = crv->x;
    r[1]  = crv->y;
    
/* GOTCHA: This only works for convex polygons - tessellation needed */

    if (n == 3)
       mode = GL_TRIANGLES;
    else if (n == 4)
       mode = GL_QUADS;
    else
       mode = GL_POLYGON;

    glBegin(mode);
    for (i = 0; i < n; i++)
        {ix[0] = r[0][i];
	 ix[1] = r[1][i];

	 PG_QUAD_FOUR_POINT(dev, ix);
	 PG_trans_point(dev, 2, PIXELC, ix, NORMC, ix);

	 v[0] = ix[0];
	 v[1] = ix[1];

	 glVertex2dv(v);};

    glEnd();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_PUT_IMAGE - put the image on the screen
 *                  - the image buffer may be overwritten by the pseudo color
 *                  - mapping if it is needed!!
 */

void _PG_GL_put_image(PG_device *dev, unsigned char *bf,
		      int ix, int iy, int nx, int ny)
   {int i, k, l, n;
    int n_pal_colors, n_dev_colors;
    int pbx[PG_BOXSZ];
    unsigned long *pixel_value;
    unsigned char c, *pbf1, *pbf2;
    double p[PG_SPACEDM];
    GLdouble v[2];
    PG_palette *pal;
    RGB_color_map *pseudo_cm;

    pbx[0] = ix;
    pbx[1] = ix + nx;
    pbx[2] = iy;
    pbx[3] = iy + ny;

    pal          = dev->current_palette;
    n_dev_colors = dev->absolute_n_color;
    n_pal_colors = pal->n_pal_colors;
    pseudo_cm    = pal->pseudo_colormap;

    PG_invert_image_data(bf, nx, ny, 1);

    if ((n_dev_colors < n_pal_colors) && (pseudo_cm != NULL))
       {unsigned char *pbf;
	double rv;

        pbf = bf;
        PM_random(-1);
        for (l = 0; l < ny; l++)
            for (k = 0; k < nx; k++)
                {i      = *pbf;
                 rv     = 3.5*PM_random(1) - 0.83;
                 *pbf++ = (rv < pseudo_cm[i].red) ?
                          pseudo_cm[i].green : pseudo_cm[i].blue;};};

/* map the pixel values to the colors */
    n = nx*ny;
    pixel_value = pal->pixel_value;
    for (i = 0; i < n; i++)
        bf[i] = pixel_value[bf[i]];

/* turn image rightside up */
    pbf1 = bf;
    for (l = 0; l < (ny/2); l++)
        {pbf2 = bf + (ny - l - 1)*nx;
	 for (k = 0; k < nx; k++)
	     {c       = *pbf1;
	      *pbf1++ = *pbf2;
	      *pbf2++ = c;};};

    PG_QUAD_FOUR_BOX(dev, pbx);

#if 0
    iy = iy + ny - 1;
#endif

    p[0] = pbx[0];
    p[1] = pbx[2];
    PG_trans_point(dev, 2, PIXELC, p, NORMC, p);
    v[0] = p[0];
    v[1] = p[1];

    glRasterPos2dv(v);
    glDrawPixels(nx, ny, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_GET_IMAGE - get the image on the screen into
 *                  - the image buffer
 */

void _PG_GL_get_image(PG_device *dev, unsigned char *bf,
		      int ix, int iy, int nx, int ny)
   {

     glReadPixels(ix, iy, nx, ny, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, bf);
     
     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

