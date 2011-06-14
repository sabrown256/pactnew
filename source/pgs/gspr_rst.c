/*
 * GSPR_RST.C - PGS raster device primitive routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

typedef struct s_edgedes edgedes;

struct s_edgedes
   {double x[PG_SPACEDM];
    double dx[PG_SPACEDM];
    edgedes *next;};

#define SMALLNO   1.0e-9
#define NODIFF    1.0e-8

/*--------------------------------------------------------------------------*/

/*                         STATE QUERY ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PG_RST_GET_TEXT_EXT - return the text extent in CS
 *                      - of the given string
 */

void _PG_rst_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p)
   {int i;

    p[0] = strlen(s)*(dev->char_width_s + dev->char_space_s) -
           dev->char_space_s;
    p[1] = dev->char_height_s;
    p[2] = 0.0;

/* clip the box to safe limits in NORMC */
    for (i = 0; i < PG_SPACEDM; i++)
        {p[i] = min(p[i], 0.99);
	 p[i] = max(p[i], 0.01);};

    PG_trans_interval(dev, nd, NORMC, p, cs, p);

    return;}

/*--------------------------------------------------------------------------*/

/*                          STATE CHANGE ROUTINES                           */

/*--------------------------------------------------------------------------*/
 
/* _PG_RST_SET_LOGICAL_OP - set the logical operation */
 
void _PG_rst_set_logical_op(PG_device *dev, PG_logical_operation lop)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_SET_LINE_STYLE - set the line style */
 
void _PG_rst_set_line_style(PG_device *dev, int style)
   {

    dev->line_style = style;
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_SET_LINE_WIDTH - set the line width */
 
void _PG_rst_set_line_width(PG_device *dev, double width)
   {

    dev->line_width = width;
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_SET_LINE_COLOR - set the line color */
 
void _PG_rst_set_line_color(PG_device *dev, int color, int mapped)
   {

    if (mapped)
       dev->line_color = _PG_trans_color(dev, color);
    else
       dev->line_color = color;
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_SET_TEXT_COLOR - set the color of the text */
 
void _PG_rst_set_text_color(PG_device *dev, int color, int mapped)
   {

    if (mapped)
       dev->text_color = _PG_trans_color(dev, color);
    else
       dev->text_color = color;
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_SET_FILL_COLOR - sets current fill color */

void _PG_rst_set_fill_color(PG_device *dev, int color, int mapped)
   {

    if (mapped)
       dev->fill_color = _PG_trans_color(dev, color);
    else
       dev->fill_color = color;
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_SET_FONT - set the character font */

int _PG_rst_set_font(PG_device *dev, char *face, char *style, int size)
   {int nfont, nstyle, nc, rv;
    int dx[PG_SPACEDM];
    double scale, sc;
    char *font_name;

    rv = FALSE;

    if ((dev != NULL) &&
	PG_setup_font(dev, face, style, size, &font_name, &nfont, &nstyle))
       {PG_query_screen_n(dev, dx, &nc);

	scale = 0.6;

	dev->char_width_s  = scale*((double) size)/((double) dx[0]);
	dev->char_height_s = scale*((double) size)/((double) dx[1]);
	dev->char_space_s  = 0.001 * dev->char_width_s;

	sc = (100.0*size)/(3.0*dx[0]);
	_PG_set_raster_text_scale(dev, sc);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_SET_CHAR_SIZE - set the character size in CS */

void _PG_rst_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x)
   {double p[PG_SPACEDM];

    PG_trans_point(dev, nd, cs, x, NORMC, p);

    dev->char_height_s = p[0];
    dev->char_width_s  = p[1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_SET_CHAR_PATH - set the direction along which text will be written
 *                       - defaults to (1, 0)
 */

void _PG_rst_set_char_path(PG_device *dev, double x, double y)
   {

    dev->char_path[0] = x;
    dev->char_path[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_SET_CHAR_PRECISION - set the character precision
 *                            - fast and fixed size or
 *                            - slow and flexible
 */

void _PG_rst_set_char_precision(PG_device *dev, int p)
   {

    dev->char_precision = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_SET_CHAR_SPACE - set the space between characters */

void _PG_rst_set_char_space(PG_device *dev, double s)
   {

    dev->char_space = (double) s;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_SET_CHAR_UP - set the direction which is up for individual
 *                     - characters
 *                     - defaults to (0, 1)
 */

void _PG_rst_set_char_up(PG_device *dev, double x, double y)
   {

    dev->char_up[0] = x;
    dev->char_up[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_SET_CHAR_LINE - set the number characters per line */
 
void _PG_rst_set_char_line(PG_device *dev, int n)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_SET_CLIPPING - set clipping
 *                      - flag = FALSE  -->  clipping off
 *                      - flag = TRUE   -->  clipping on
 */

void _PG_rst_set_clipping(PG_device *dev, int flag)
   {double wc[PG_BOXSZ];

    PG_get_viewspace(dev, WORLDC, wc);
 
    dev->clipping = flag;

    return;}
 
/*--------------------------------------------------------------------------*/

/*                          MOVE AND DRAW ROUTINES                          */

/*--------------------------------------------------------------------------*/
 
/* _PG_RST_MOVE_GR_ABS - move the current graphics cursor position to the
 *                     - given absolute coordinates in WC
 */
 
void _PG_rst_move_gr_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_MOVE_TX_ABS - move the current text cursor position to the
 *                     - given coordinates in WC
 */
 
void _PG_rst_move_tx_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_abs(dev, p);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_MOVE_TX_REL - move the current text cursor position to the
 *                     - given relative coordinates in WC
 */
 
void _PG_rst_move_tx_rel(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_rel(dev, p);};
     
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_DRAW_TO_ABS - draw a line from current position to
 *                     - absolute position (x, y)
 *                     - in WC
 */

void _PG_rst_draw_to_abs(PG_device *dev, double x, double y)
   {int i1[PG_SPACEDM], i2[PG_SPACEDM];
    double o[PG_SPACEDM], p[PG_SPACEDM];

    if (dev != NULL)
       {o[0] = dev->gcur[0];
	o[1] = dev->gcur[1];

	p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, o, PIXELC, o);
	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);

	i1[0] = o[0];
	i1[1] = o[1];
	i2[0] = p[0];
	i2[1] = p[1];

	_PG_rst_draw_line_2d(dev, i1, i2);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_DRAW_TO_REL - draw a line from current position to
 *                     - relative position (x, y)
 *                     - in WC
 */
 
void _PG_rst_draw_to_rel(PG_device *dev, double x, double y)
   {int i1[PG_SPACEDM], i2[PG_SPACEDM];
    double o[PG_SPACEDM], p[PG_SPACEDM];

    if (dev != NULL)
       {o[0] = dev->gcur[0];
	o[1] = dev->gcur[1];

	p[0] = x;
	p[1] = y;

	_PG_move_gr_rel(dev, p);

	PG_trans_point(dev, 2, WORLDC, o, PIXELC, o);
	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);

	i1[0] = o[0];
	i1[1] = o[1];
	i2[0] = p[0];
	i2[1] = p[1];

	_PG_rst_draw_line_2d(dev, i1, i2);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_DRAW_CURVE - draw a PG_curve */
 
void _PG_rst_draw_curve(PG_device *dev, PG_curve *crv, int clip)
   {int n, j, xo, yo;
    int i1[PG_SPACEDM], i2[PG_SPACEDM];
    int *px, *py;
    
    n  = crv->n;
    px = crv->x;
    py = crv->y;
    xo = crv->x_origin;
    yo = crv->y_origin;
    
    i1[0] = *px++ + xo;
    i1[1] = *py++ + yo;

    for (j = 1; j < n; j++)
        {i2[0] = *px++ + xo;
	 i2[1] = *py++ + yo;

	 _PG_rst_draw_line_2d(dev, i1, i2);

	 i1[0] = i2[0];
	 i1[1] = i2[1];};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_DRAW_DISJOINT_POLYLINE_2 - draws disjoint two dimensional
 *                                  - line segments specified in CS
 */

void _PG_rst_draw_disjoint_polyline_2(PG_device *dev, double **r,
                                      long n, int flag, PG_coord_sys cs)
   {int i, ix1[PG_SPACEDM], ix2[PG_SPACEDM];
    double box[PG_BOXSZ];
    double *px, *py;

    if ((dev != NULL) && (n > 0))
       {px = r[0];
	py = r[1];

	for (i = 0; i < n; i++)
	    {box[0] = *px++;
	     box[2] = *py++;
	     box[1] = *px++;
	     box[3] = *py++;

	     if (cs == WORLDC)
	        PG_log_space(dev, 2, FALSE, box);

	     PG_trans_box(dev, 2, cs, box, PIXELC, box);

	     ix1[0] = box[0];
	     ix1[1] = box[2];
	     ix2[0] = box[1];
	     ix2[1] = box[3];

	     if ((ix1[0] != ix2[0]) || (ix1[1] != ix2[1]))
	        _PG_rst_draw_line_2d(dev, ix1, ix2);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_DRAW_DISJOINT_POLYLINE_3 - draws disjoint three dimensional
 *                                  - line segments in WC
 *                                  - if CPY is TRUE operate on a copy of R
 *                                  - because R should not be changed
 */

void _PG_rst_draw_disjoint_polyline_3(PG_device *dev, PG_coord_sys cs, long n,
				      double **r, int clip, int cpy)
   {int i, m;
    int ix1[PG_SPACEDM], ix2[PG_SPACEDM];
    double **ix, **rx;

    PG_make_device_current(dev);

    m = n << 1;

    if (cpy == TRUE)
       rx = PM_copy_vectors(3, m, r);
    else
       rx = r;

    PG_rotate_vectors(dev, 3, m, rx);

    ix = PM_make_vectors(3, m);

    PG_trans_points(dev, m, 3, cs, rx, PIXELC, ix);

    for (i = 0; i < m; i += 2)
        {ix1[0] = ix[0][i];
	 ix1[1] = ix[1][i];
	 ix1[2] = ix[2][i];

	 ix2[0] = ix[0][i+1];
	 ix2[1] = ix[1][i+1];
	 ix2[2] = ix[2][i+1];

         _PG_rst_draw_line_3d(dev, ix1, ix2);};

    PM_free_vectors(3, ix);

    if (cpy == TRUE)
       PM_free_vectors(3, rx);

    PG_release_current_device(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DESC_EDGE - make and return an edgedes for segment IA to IB of IR */

static INLINE edgedes *_PG_desc_edge(int nd, double **ir,
				     int ia, int ib, double yc)
   {int id;
    int ix1[PG_SPACEDM], ix2[PG_SPACEDM];
    double dx[PG_SPACEDM];
    edgedes *ei;

    ei = CMAKE(edgedes);

    for (id = 0; id < nd; id++)
        {ix1[id] = ir[id][ia];
	 ix2[id] = ir[id][ib];
	 dx[id] = ix1[id] - ix2[id];};

    for (id = 0; id < nd; id++)
        {if (id == 1)
	    {ei->dx[1] = (ix1[1] > ix2[1]) ? dx[1] : -dx[1];
	     ei->next  = NULL;}
	 else
	    {if (ix1[id] == ix2[id])
	        {ei->x[id]  = ix1[id];
		 ei->dx[id] = 0.0;}
	     else
	        {ei->x[id]  = ix1[id] + (yc - ix1[1])*(dx[id]/dx[1]);
		 ei->dx[id] = -(dx[id]/dx[1]);};};};

    return(ei);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_MAKE_EDGE_TABLE - make and fill an edge table */

static edgedes **_PG_make_edge_table(PG_device *dev, int nd,
				     int n, double **r,
				     int iymx, int iymn)
   {int i, j, iya, iyb, narr;
    int *flags;
    double yc;
    double **ir;
    edgedes **et, *frst, *ths, *ei;

    et = NULL;

    if (n > 0)
       {narr  = iymx - iymn;
	et    = CMAKE_N(edgedes *, narr);
	flags = CMAKE_N(int, n-1);
	SC_MEM_INIT_N(int, flags, n-1);

/* transform to PC for efficiency */
	ir = PM_make_vectors(nd, n);
	PG_trans_points(dev, n, nd, WORLDC, r, PIXELC, ir);

	for (i = iymx-1; i >= iymn; i--)
	    {yc   = (double) i + 0.5;
	     frst = NULL;
	     ths  = NULL;
	     for (j = 0; j < n-1; j++)
	         {iya = ir[1][j];
		  iyb = ir[1][j+1];
		  if (flags[j] == FALSE)
		     {if (((yc < iya) || (yc < iyb)) && (iya != iyb))
			 {flags[j] = TRUE;

			  ei = _PG_desc_edge(nd, ir, j, j+1, yc);

			  if (frst == NULL)
			     frst = ei;
			  else
			     ths->next = ei;

			  ths = ei;};};};

	     et[i-iymn] = frst;};

	PM_free_vectors(nd, ir);

	CFREE(flags);};

    return(et);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FREE_EDGE_TABLE - free up space in the edge table */

static void _PG_free_edge_table(edgedes **et, int iymn, int iymx)
   {int i;
    edgedes *ei, *nxt;

    for (i = iymn; i < iymx; i++)
        for (ei = et[i-iymn]; ei != NULL; ei = nxt)
	    {nxt = ei->next;
	     CFREE(ei);};

    CFREE(et);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FIND_ACTIVE - fill the active edge list AE from EI
 *                 - return the new number of active edges
 */

static int _PG_find_active(edgedes **ae, int n, edgedes *ei)
   {int k, l;
    edgedes *pei, *pa;

/* try for the next scanline */
    for (k = 0; k < n; k++)
        {pa = ae[k];

	 pa->x[0] += pa->dx[0];
	 pa->x[2] += pa->dx[2];

	 pa->dx[1]--;
	 if (pa->dx[1] == 0)
	    {for (l = k; l < n-1; l++)
		 ae[l] = ae[l+1];
	     n--;
	     k--;};};

    for (pei = ei; pei != NULL; pei = pei->next)
	ae[n++] = pei;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_BSORT_X - bubble sort is best for this problem */

static void _PG_bsort_x(edgedes **ae, int n)
   {int i, sorted;

    sorted = FALSE;
    while (sorted == FALSE)
       {sorted = TRUE;
	for (i = 0; i < n-1; i++)
	    if (ae[i]->x[0] > ae[i+1]->x[0])
	       {sorted  = FALSE;
		SC_SWAP_VALUE(edgedes *, ae[i], ae[i+1]);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_EDGE_POINT - return the coordinate vector IP of edge EI at IY */

static INLINE void _PG_edge_point(int *ip, int nd, edgedes *ei, int iy)
   {int id;
    double x;

    ip[1] = iy;

    for (id = 0; id < nd; id++)
        {if (id != 1)
	    {x      = ei->x[id] + 0.5*ei->dx[id] + SMALLNO;
	     ip[id] = x;
	     if (ABS(x - (double) ip[id]) > NODIFF)
	        ip[id] = ceil(x);

	     else if (ei->x[id] > (double) ip[id])
	        ip[id] = floor(x);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_FILL_RST_LINE - fill in the part of line IY defined by AE */

static void _PG_fill_rst_line(PG_device *dev, edgedes **ae,
			      int nd, int ne, int iy)
   {int i, ix1[PG_SPACEDM], ix2[PG_SPACEDM];

/* force NE to be even - hack for tortured polygons
 * prevents reference off the end of AE in loop below
 */
    ne = (ne >> 1) << 1;

    for (i = 0; i < ne; i += 2)
        {_PG_edge_point(ix1, nd, ae[i],   iy);
	 _PG_edge_point(ix2, nd, ae[i+1], iy);

	 if (nd < 3)
	    _PG_rst_draw_line_2d(dev, ix1, ix2);
	 else
	    _PG_rst_draw_line_3d(dev, ix1, ix2);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_SHADE_POLY - polygon shading routine
 *                    - N points (X,Y) are in WC
 */

void _PG_rst_shade_poly(PG_device *dev, int nd, int n, double **r)
   {int i, iymn, iymx, ne, lclr, fclr;
    double lwd;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    edgedes **ae, **et;

    if ((dev != NULL) && (n > nd))

/* get the PC y limits for this polygon */
       {PM_maxmin(r[1], &x1[1], &x2[1], n);

	x1[0] = 0.0;
	PG_trans_point(dev, 2, WORLDC, x1, PIXELC, x1);
	iymn = x1[1];

	x2[0] = 0.0;
	PG_trans_point(dev, 2, WORLDC, x2, PIXELC, x2);
	iymx = x2[1];

	lclr = PG_fget_line_color(dev);
	fclr = PG_fget_fill_color(dev);
	PG_fset_line_color(dev, fclr, FALSE);

	lwd = PG_fget_line_width(dev);
	PG_fset_line_width(dev, _PG_gattrs.line_width);

	et = _PG_make_edge_table(dev, nd, n, r, iymx, iymn);
	if (et != NULL)
	   {ae = CMAKE_N(edgedes *, n-1);

	    ne = 0;
	    for (i = iymx-1; i >= iymn; i--)
	        {ne = _PG_find_active(ae, ne, et[i-iymn]);
		 _PG_bsort_x(ae, ne);
		 _PG_fill_rst_line(dev, ae, nd, ne, i);};

	    CFREE(ae);
	    _PG_free_edge_table(et, iymn, iymx);};

	PG_fset_line_width(dev, lwd);
	PG_fset_line_color(dev, lclr, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_SHADE_POLY_3 - polygon shading routine
 *                      - N points (X,Y) are in WC
 */

void _PG_rst_shade_poly_3(PG_device *dev, int n, double **r)
   {

    _PG_rst_shade_poly(dev, 3, n, r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_RST_FILL_CURVE - fill a closed PG_curve */

void _PG_rst_fill_curve(PG_device *dev, PG_curve *crv)
   {int i, n, xo, yo;
    int *r[PG_SPACEDM];
    double x[PG_SPACEDM], ix[PG_SPACEDM];
    double **w;

    n    = crv->n;
    r[0] = crv->x;
    r[1] = crv->y;
    xo   = crv->x_origin;
    yo   = crv->y_origin;
    
/* the fill poly routine requires WC - sigh - so convert */
    w = PM_make_vectors(2, n);

    for (i = 0; i < n; i++)
        {ix[0] = xo + r[0][i];
	 ix[1] = yo + r[1][i];

	 PG_trans_point(dev, 2, PIXELC, ix, WORLDC, x);

	 w[0][i] = x[0];
	 w[1][i] = x[1];};

    _PG_rst_shade_poly(dev, 2, n, w);

    PM_free_vectors(2, w);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_PUT_IMAGE - put the image on the screen
 *                   - the image buffer may be overwritten by the pseudo color
 *                   - mapping if it is needed!!
 */

void _PG_rst_put_image(PG_device *dev, unsigned char *bf,
                       int ix, int iy, int nx, int ny)
   {int bpp, l, k, m, dx, dy, sz, rgb, pc;
    int color, red_fl, green_fl, blue_fl, ok;
    int ir[PG_SPACEDM];
    unsigned long *pv;
    unsigned char *pbf, *r, *g, *b;
    double scale;
    PG_palette *pal;
    RGB_color_map *true_cm;
    PG_RAST_device *mdv;
    frame *fr;

    pal     = dev->current_palette;
    true_cm = pal->true_colormap;
    pv      = pal->pixel_value;

    pbf = bf;
    bpp = log((double) (dev->absolute_n_color))/log(2.0) + 0.5;
    if (bpp == 1)
       nx = _PG_byte_bit_map(bf, nx, ny, TRUE);

    GET_RAST_DEVICE(dev, mdv);

    rgb = mdv->rgb_mode;
    fr  = mdv->raster;
    GET_RGB(fr, r, g, b);

    dx = fr->width;
    dy = fr->height;
    sz = dx*dy;

    ok = TRUE;
    if (rgb)
       {scale = 255.0/((double) MAXPIX);

	for (l = 0; l < ny; l++)
	    {ir[1] = l + iy;
	     PG_QUAD_FOUR_POINT(dev, ir);

	     pbf = bf + l*nx;
	     for (k = 0; k < nx; k++)
	         {color    = *pbf++;
		  red_fl   = scale*true_cm[color].red;
		  green_fl = scale*true_cm[color].green;
		  blue_fl  = scale*true_cm[color].blue;
		  if ((red_fl < 0) || (255 < red_fl))
		     {if (ok)
			 {PRINT(stdout,
				"Bad Red Value for index %d: %d\n",
				color, red_fl);
			  ok = FALSE;};
		      continue;};

		  if ((green_fl < 0) || (255 < green_fl))
		     {if (ok)
			 {PRINT(stdout,
				"Bad Green Value for index %d: %d\n",
				color, green_fl);
			  ok = FALSE;};
		      continue;};

		  if ((blue_fl < 0) || (255 < blue_fl))
		     {if (ok)
			 {PRINT(stdout,
				"Bad Blue Value for index %d: %d\n",
				color, blue_fl);
			  ok = FALSE;};
		      continue;};

		  ir[0] = k + ix;

		  m = ir[0] + ir[1]*dx;

/* clip out of range pixels */
		  if ((m < 0) || (m >= sz))
		     break;

		  r[m] = red_fl;
		  g[m] = green_fl;
		  b[m] = blue_fl;};};}
    else
       {for (l = 0; l < ny; l++)
	    {ir[1] = l + iy;
	     PG_QUAD_FOUR_POINT(dev, ir);

	     pbf = bf + l*nx;
	     for (k = 0; k < nx; k++)
	         {ir[0] = k + ix;

		  m = ir[0] + ir[1]*dx;
		  if ((m < 0) || (m >= sz))
		     {if (ok)
			 {PRINT(stdout,
				"Bad index %d < 0 or %d >= %d (bw)\n",
				m, m, sz);
			  ok = FALSE;};};

		  pc   = *pbf++;
		  r[m] = (pv != NULL) ? pv[pc] : pc;};};};

    fr->n_on += nx*ny;

    if (SC_mem_trace() < 0)
       {PRINT(stdout, "MEMORY CORRUPTED - _PG_RST_PUT_IMAGE\n");
	exit(2);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_GET_IMAGE - get the image on the screen into
 *                   - the image buffer
 */

void _PG_rst_get_image(PG_device *dev, unsigned char *bf,
                       int ix, int iy, int nx, int ny)
   {int k, l, mo, mi, mx, my;
    unsigned char *r, *g, *b;
    frame *fr;
    PG_RAST_device *mdv;

    GET_RAST_DEVICE(dev, mdv);

    fr = mdv->raster;
    mx = mdv->width;
    my = mdv->height;

    SC_ASSERT(my != 0);

    GET_RGB(fr, r, g, b);

    SC_ASSERT(r != NULL);
    SC_ASSERT(g != NULL);
    SC_ASSERT(b != NULL);

    for (l = 0; l < ny; l++)
        {for (k = 0; k < nx; k++)
	     {mo = k + l*nx;
	      mi = (k + ix) + (l + iy)*mx;

	      bf[mo] = r[mi];};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRANOM - diagnostic print to find anomalies in raster image
 *         - list lines with various anomalies
 */

void dpranom(PG_device *dev)
   {int i, j, ic, nx, ny;
    int imn, imx, jmn, jmx, dx;
    int na, ac;
    unsigned char *c[3], *cc, *cl, *cu;
    frame *fr;
    PG_RAST_device *mdv;

    GET_RAST_DEVICE(dev, mdv);

    fr = mdv->raster;
    nx = mdv->width;
    ny = mdv->height;

    SC_ASSERT(ny != 0);

    imn = fr->box[0];
    imx = fr->box[1];
    jmn = fr->box[2];
    jmx = fr->box[3];

    dx = imx - imn;

    GET_RGB(fr, c[0], c[1], c[2]);

/* find lines whose neighbors are the same but different from the line */
    for (j = jmn+1; j < jmx-1; j++)
        {na = 0;
	 for (i = imn; i < imx; i++)
	     {ac = 0;
	      for (ic = 0; ic < 3; ic++)
		  {cc = c[ic] + j*nx + i;
		   cl = cc - nx;
		   cu = cc + nx;
		   ac += ((cl[0] == cu[0]) && (cc[0] != cl[0]));};

	      na += ac;};

	 if (10*na > dx)
	    io_printf(stdout, " %4d  diff neighbor %4d/%4d\n", j, na, dx);};
		   
/* find lines which differ from the line above */
    for (j = jmn+1; j < jmx-1; j++)
        {na = 0;
	 for (i = imn; i < imx; i++)
	     {ac = 0;
	      for (ic = 0; ic < 3; ic++)
		  {cc = c[ic] + j*nx + i;
		   cu = cc + nx;
		   ac += (cc[0] != cu[0]);};

	      na += ac;};

	 if (10*na > dx)
	    io_printf(stdout, " %4d  diff above %4d/%4d\n", j, na, dx);};
		   
/* find lines which differ from the line below */
    for (j = jmn+1; j < jmx-1; j++)
        {na = 0;
	 for (i = imn; i < imx; i++)
	     {ac = 0;
	      for (ic = 0; ic < 3; ic++)
		  {cc = c[ic] + j*nx + i;
		   cl = cc - nx;
		   ac += (cc[0] != cl[0]);};

	      na += ac;};

	 if (10*na > dx)
	    io_printf(stdout, " %4d  diff below %4d/%4d\n", j, na, dx);};
		   
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
