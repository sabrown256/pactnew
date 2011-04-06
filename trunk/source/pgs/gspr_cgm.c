/*
 * GSPR_CGM.C - PGS CGM primitive routines
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

/* _PG_CGM_GET_TEXT_EXT - return the text extent in CS
 *                      - of the given string
 */

void _PG_CGM_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p)
   {

    p[0] = strlen(s)*(dev->char_width_s + dev->char_space_s) -
           dev->char_space_s;
    p[1] = dev->char_height_s;
    p[2] = 0.0;

    p[0] *= 1.6;
    p[1] *= 2.0;

    PG_trans_interval(dev, nd, NORMC, p, cs, p);

    return;}

/*--------------------------------------------------------------------------*/

/*                          STATE CHANGE ROUTINES                           */

/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_SET_LOGICAL_OP - set the logical operation */
 
void _PG_CGM_set_logical_op(PG_device *dev, PG_logical_operation lop)
   {

    dev->logical_op = lop;

/* GOTCHA: Put something appropriate here */
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_SET_LINE_STYLE - set the line style */
 
void _PG_CGM_set_line_style(PG_device *dev, int style)
   {

    dev->line_style = style;

    if (dev->cgm_page_set)
       {if (style > 4)
           style = 1;

        PG_CGM_command(dev, LINE_TYPE, style);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_SET_LINE_WIDTH - set the line width */
 
void _PG_CGM_set_line_width(PG_device *dev, double width)
   {int lw;
 
    dev->line_width = width;

    if (dev->cgm_page_set)
       {lw = max(1.0, 160.0*width);
        PG_CGM_command(dev, LINE_WIDTH, lw);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_SET_LINE_COLOR - set the line color */
 
void _PG_CGM_set_line_color(PG_device *dev, int color, int mapped)
   {

    dev->line_color = color;

    if (mapped)
       color = _PG_trans_color(dev, color);

    if (dev->cgm_page_set)
       PG_CGM_command(dev, LINE_COLOUR, color);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_SET_TEXT_COLOR - set the color of the text */
 
void _PG_CGM_set_text_color(PG_device *dev, int color, int mapped)
   {

    dev->text_color = color;

    if (mapped)
       color = _PG_trans_color(dev, color);

    if (dev->cgm_page_set)
       PG_CGM_command(dev, TEXT_COLOUR, color);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_SET_FILL_COLOR - sets current fill color */

void _PG_CGM_set_fill_color(PG_device *dev, int color, int mapped)
   {

    dev->fill_color = color;

    if (dev->cgm_page_set)
       {if (mapped)
           PG_CGM_command(dev, FILL_COLOUR, color+8);
        else
           PG_CGM_command(dev, FILL_COLOUR, color);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_SET_FONT - set the character font */

int _PG_CGM_set_font(PG_device *dev, char *face, char *style, int size)
   {int nfont, nstyle, ih, dx, dy, nc;
    char *font_name;
    double sx, sy, scale;

    if (dev == NULL)
       return(FALSE);

    if (!PG_setup_font(dev, face, style, size, &font_name, &nfont, &nstyle))
       return(FALSE);

    scale = 0.77;
    PG_query_screen(dev, &dx, &dy, &nc);

    sx = 1200.0*PG_window_width(dev)/((double) dx);
    sy = 1200.0*PG_window_height(dev)/((double) dy);

    dev->char_width_s  = scale*size/sx;
    dev->char_height_s = scale*size/sy;

    ih = _PG_gattrs.cgm_text_mag*dev->char_height_s*PG_window_height(dev);
    if (dev->cgm_page_set)
       {PG_CGM_command(dev, TEXT_FONT_INDEX, nfont+1);
        PG_CGM_command(dev, CHARACTER_HEIGHT, ih);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_SET_CHAR_SIZE - set the character size in CS */

void _PG_CGM_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x)
   {int ih;
    double p[PG_SPACEDM];

    PG_trans_point(dev, nd, cs, x, NORMC, p);

    dev->char_height_s = p[0];
    dev->char_width_s  = p[1];

    ih = dev->char_height_s*PG_window_height(dev);
    PG_CGM_command(dev, CHARACTER_HEIGHT, ih);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_SET_CHAR_PATH - set the direction along which text will be written
 *                       - defaults to (1, 0)
 */

void _PG_CGM_set_char_path(PG_device *dev, double x, double y)
   {

    dev->char_path[0] = x;
    dev->char_path[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_SET_CHAR_PRECISION - set the character precision
 *                            - fast and fixed size or
 *                            - slow and flexible
 */

void _PG_CGM_set_char_precision(PG_device *dev, int p)
   {

    dev->char_precision = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_SET_CHAR_SPACE - set the space between characters */

void _PG_CGM_set_char_space(PG_device *dev, double s)
   {

    dev->char_space = (double) s;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_SET_CHAR_UP - set the direction which is up for individual
 *                     - characters
 *                - defaults to (0, 1)
 */

void _PG_CGM_set_char_up(PG_device *dev, double x, double y)
   {

    dev->char_up[0] = x;
    dev->char_up[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_SET_CHAR_LINE - set the number characters per line */
 
void _PG_CGM_set_char_line(PG_device *dev, int n)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_SET_CLIPPING - set clipping
 *                      - flag = FALSE  -->  clipping off
 *                      - flag = TRUE   -->  clipping on
 */

void _PG_CGM_set_clipping(PG_device *dev, int flag)
   {int pc[PG_BOXSZ], pts[PG_BOXSZ];

    if (dev->cgm_page_set)
       {if (flag)
	   {_PG_find_clip_region(dev, pc, flag, TRUE);

	    pts[0] = pc[0];
	    pts[3] = pc[2];
	    pts[2] = pc[1];
	    pts[1] = pc[3];

            PG_CGM_command(dev, CLIP_RECTANGLE, pts);};

        PG_CGM_command(dev, CLIP_INDICATOR, flag);};
 
    dev->clipping = flag;

    return;}
 
/*--------------------------------------------------------------------------*/

/*                          MOVE AND DRAW ROUTINES                          */

/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_MOVE_GR_ABS - move the current graphics cursor position to the
 *                     - given absolute coordinates in WC
 */
 
void _PG_CGM_move_gr_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_MOVE_TX_ABS - move the current text cursor position to the
 *                     - given coordinates in WC
 */
 
void _PG_CGM_move_tx_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_abs(dev, p);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_MOVE_TX_REL - move the current text cursor position to the
 *                     - given relative coordinates in WC
 */
 
void _PG_CGM_move_tx_rel(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_rel(dev, p);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_DRAW_TO_ABS - draw a line from current position to
 *                     - absolute position (x, y)
 *                     - in WC
 */
 
void _PG_CGM_draw_to_abs(PG_device *dev, double x, double y)
   {int pts[4];
    double o[PG_SPACEDM], n[PG_SPACEDM];

    if (dev == NULL)
       {if (dev->clipping == TRUE)
	   {o[0] = dev->gcur[0];
	    o[1] = dev->gcur[1];
	    n[0] = x;
	    n[1] = y;

	    if (PG_clip_line_seg(dev, o, n))
	       {if (dev->cgm_page_set)
		   {if (dev->clipping == FALSE)
		       {o[0] = dev->gcur[0];
			o[1] = dev->gcur[1];}

		    PG_trans_point(dev, 2, WORLDC, o, PIXELC, o);
		    pts[0] = o[0];
		    pts[1] = o[1];

		    _PG_move_gr_abs(dev, n);

		    PG_trans_point(dev, 2, WORLDC, n, PIXELC, n);
		    pts[2] = n[0];
		    pts[3] = n[1];

		    PG_CGM_command(dev, POLYLINE(4), pts);};};};};
        
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_DRAW_TO_REL - draw a line from current position to
 *                     - relative position (x, y)
 *                     - in WC
 */
 
void _PG_CGM_draw_to_rel(PG_device *dev, double x, double y)
   {int pts[4];
    double o[PG_SPACEDM], n[PG_SPACEDM];

    if (dev == NULL)
       {if (dev->cgm_page_set)
	   {o[0] = dev->gcur[0];
	    o[1] = dev->gcur[1];

	    PG_trans_point(dev, 2, WORLDC, o, PIXELC, o);
	    pts[0] = o[0];
	    pts[1] = o[1];

	    n[0] = x;
	    n[1] = y;

	    _PG_move_gr_rel(dev, n);

	    PG_trans_point(dev, 2, WORLDC, n, PIXELC, n);
	    pts[2] = n[0];
	    pts[3] = n[1];
 
	    PG_CGM_command(dev, POLYLINE(4), pts);};};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_DRAW_CURVE - draw a PG_curve */
 
void _PG_CGM_draw_curve(PG_device *dev, PG_curve *crv, int clip)
   {int i, flag, n, xo, yo, ix, iy, ixo, iyo, ix1, iy1;
    int pc[PG_BOXSZ];
    int *x, *y, *pts;

    if (dev->cgm_page_set)
       {flag = TRUE;
	
	_PG_find_clip_region(dev, pc, dev->clipping, TRUE);

	n  = crv->n;
	x  = crv->x;
	y  = crv->y;
        xo = crv->x_origin;
        yo = crv->y_origin;

	pts = CMAKE_N(int, 2*n);
    
	if (pc[2] > pc[3])
	   {SC_SWAP_VALUE(int, pc[2], pc[3]);};

        ix1 = 0;
        iy1 = 0;
    
	for (i = 0, ix = 0, iy = 1; i < n; i++)
	    {ixo = ix1;
	     iyo = iy1;
	     ix1 = x[i] + xo;
	     iy1 = y[i] + yo;
       
	     if (i == 0)
	        continue;

	     if ((ix1 == ixo) && (iy1 == iyo))
	        continue;

	     if (clip && dev->clipping)
	        {if (((ix1 < pc[0]) && (ixo < pc[0])) ||
		     ((ix1 > pc[1]) && (ixo > pc[1])))
		    {flag = TRUE;
		     continue;}

		 if (((iy1 < pc[2]) && (iyo < pc[2])) ||
		     ((iy1 > pc[3]) && (iyo > pc[3])))
		    {flag = TRUE;
		     continue;};};
/*
	     if (flag)
	        {dev->gcur[0] = ixo;
		 dev->gcur[1] = iyo;
		 flag = FALSE;}

	     if (ix == 0)
*/
	     if (flag)
	        {pts[ix] = ixo;
		 pts[iy] = iyo;

		 ix +=2;
		 iy +=2;
	         flag = FALSE;}

	     pts[ix] = ix1;
	     pts[iy] = iy1;

	     ix += 2;
	     iy += 2;};
        
        PG_CGM_command(dev, POLYLINE(ix), pts);

        CFREE(pts);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_DRAW_DISJOINT_POLYLINE_2 - draws disjoint two dimensional
 *                                  - line segments specified in CS
 */

void _PG_CGM_draw_disjoint_polyline_2(PG_device *dev, double **r,
                                      long n, int flag, PG_coord_sys cs)
   {int i, ix, iy, *pts;
    int ix1, ix2, iy1, iy2;
    int pc[PG_BOXSZ];
    double *px, *py;
    double box[PG_BOXSZ];

    if (dev == NULL)
       return;

    PG_trans_points(dev, 2*n, 2, cs, r, WORLDC, r);

    px = r[0];
    py = r[1];

    _PG_find_clip_region(dev, pc, dev->clipping, TRUE);

    pts = CMAKE_N(int, 4*n);
    for (i = 0, ix = 0, iy = 1; i < n; i++)
        {box[0] = *px++;
         box[2] = *py++;
         box[1] = *px++;
         box[3] = *py++;
         
         if (cs == WORLDC)
	    PG_log_space(dev, 2, FALSE, box);

	 PG_trans_box(dev, 2, WORLDC, box, PIXELC, box);

	 ix1 = box[0];
	 iy1 = box[2];
	 ix2 = box[1];
	 iy2 = box[3];

         if ((ix1 == ix2) && (iy1 == iy2))
	    continue;

/* do simple clipping */
	 if (dev->clipping &&
	     (((ix1 < pc[0]) && (ix2 < pc[0])) ||
	      ((ix1 > pc[1]) && (ix2 > pc[1])) ||
	      ((iy1 < pc[2]) && (iy2 < pc[2])) ||
	      ((iy1 > pc[3]) && (iy2 > pc[3]))))
	    continue;

	 pts[ix]   = ix1;
	 pts[iy]   = iy1;
	 pts[ix+2] = ix2;
	 pts[iy+2] = iy2;

         ix += 4;
         iy += 4;};

    PG_CGM_command(dev, DISJOINT_POLYLINE(ix), pts);

    CFREE(pts);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_SHADE_POLY - polygon shading routine */

void _PG_CGM_shade_poly(PG_device *dev, int nd, int n, double **r)
   {int i, ix, iy, *pts;
    double x[PG_SPACEDM], p[PG_SPACEDM];

    if (nd == 3)
       _PG_rst_shade_poly(dev, nd, n, r);

    else if (dev->cgm_page_set)
       {if (n > 0)     
           {pts = CMAKE_N(int, 2*n);
            for (i = 0, ix = 0, iy = 1; i < n; i++, ix += 2, iy += 2)
                {x[0] = r[0][i];
                 x[1] = r[1][i];

		 PG_trans_point(dev, 2, WORLDC, x, PIXELC, p);
		 pts[ix] = p[0];
		 pts[iy] = p[1];};
 
            PG_CGM_command(dev, POLYGON(2*n), pts);

            CFREE(pts);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_CGM_FILL_CURVE - fill a closed PG_curve */

void _PG_CGM_fill_curve(PG_device *dev, PG_curve *crv)
   {int i, n, ix, iy, xo, yo, *pts;
    int *x, *y;

    n  = crv->n;
    x  = crv->x;
    y  = crv->y;
    xo = crv->x_origin;
    yo = crv->y_origin;
    
    if (dev->cgm_page_set)
       {pts = CMAKE_N(int, 2*n);
        for (i = 0, ix = 0, iy = 1; i < n; i++, ix += 2, iy += 2)
            {pts[ix] = x[i] + xo;
             pts[iy] = y[i] + yo;};
 
        PG_CGM_command(dev, POLYGON(2*n), pts);

        PG_CGM_command(dev, LINE_COLOUR, dev->fill_color);
        PG_CGM_command(dev, POLYLINE(2*n), pts);
        PG_CGM_command(dev, LINE_COLOUR, dev->line_color);

        CFREE(pts);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_CGM_PUT_IMAGE - put the image on the screen
 *                   - the image buffer may be overwritten by the pseudo
 *                   - color mapping if it is needed!!
 */

void _PG_CGM_put_image(PG_device *dev, unsigned char *bf,
                       int ix, int iy, int nx, int ny)
   {int params[10], dx, dy, n, i, k, l;
    double rv;
    int n_pal_colors, n_dev_colors;
    unsigned char *pbf;
    PG_palette *pal;
    RGB_color_map *pseudo_cm;

    pal          = dev->current_palette;
    n_dev_colors = dev->absolute_n_color;
    n_pal_colors = pal->n_pal_colors;
    pseudo_cm    = pal->pseudo_colormap;

    PG_invert_image_data(bf, nx, ny, 1);

/* shift the pixels by the bottom eight mandatory colors
 * this gets into the expected palette range
 */
    if (dev->current_palette != dev->palettes)
       {n   = nx*ny;
	pbf = bf;
	for (i = 0; i < n; i++)
	    *pbf++ += 8;};

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

    n  = nx*ny;
    dx = nx*dev->resolution_scale_factor;
    dy = ny*dev->resolution_scale_factor;

/*
    params[0] = ix;
    params[1] = iy;
    params[2] = ix + dx;
    params[3] = iy - dy;
    params[4] = ix + dx;
    params[5] = iy;
*/
    params[0] = ix;
    params[1] = iy + dy;
    params[2] = ix + dx;
    params[3] = iy;
    params[4] = ix + dx;
    params[5] = iy + dy;

    params[6] = nx;
    params[7] = ny;
    params[8] = 0;
    params[9] = 1;
    PG_CGM_command(dev, CELL_ARRAY(params, n), bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

