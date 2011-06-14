/*
 * GSPR_PS.C - PGS PostScript primitive routines
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

/* _PG_PS_GET_TEXT_EXT - return the text extent in CS
 *                     - of the given string
 */

void _PG_PS_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p)
   {

    p[0] = strlen(s)*(dev->char_width_s + dev->char_space_s) -
           dev->char_space_s;
    p[1] = dev->char_height_s;
    p[2] = 0.0;

/* probably wrong factor for fixed width font */
    p[0] *= 0.55*_PG_gattrs.ps_dots_inch/600.0;
    p[1] *= 1.1*_PG_gattrs.ps_dots_inch/600.0;

    PG_trans_interval(dev, nd, NORMC, p, cs, p);

    return;}

/*--------------------------------------------------------------------------*/

/*                          STATE CHANGE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PG_PS_SET_FONT - set the character font */

int _PG_PS_set_font(PG_device *dev, char *face, char *style, int size)
   {int nfont, nstyle, nc, rv;
    int dx[PG_SPACEDM];
    double sx, sy, sd, sc;
    char *font_name;

    rv = FALSE;

    if ((dev != NULL) &&
	PG_setup_font(dev, face, style, size, &font_name, &nfont, &nstyle))
       {PG_query_screen_n(dev, dx, &nc);

	sx = PG_window_width(dev)/((double) dx[0]);
	sy = PG_window_height(dev)/((double) dx[1]);
	sd = _PG_gattrs.ps_dots_inch/600.0;

	dev->char_width_s  = size/(612*sx*sd);
	dev->char_height_s = size/(792*sy*sd);

	sc = (100.0*size)/(3.0*dx[0]);
	_PG_set_raster_text_scale(dev, sc);

	nc = (int) (size*_PG_gattrs.ps_dots_inch/72.0);
	io_printf(dev->file, "%d /%s SF\n", nc, font_name);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_SET_LOGICAL_OP - set the logical operation */
 
void _PG_PS_set_logical_op(PG_device *dev, PG_logical_operation lop)
   {

    dev->logical_op = lop;

    switch (lop)
       {case GS_XOR :
/* GOTCHA: Put something appropriate here */
             break;

        default      :
        case GS_COPY :
/* GOTCHA: Put something appropriate here */
             break;};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_SET_LINE_STYLE - set the line style */
 
void _PG_PS_set_line_style(PG_device *dev, int style)
   {int lv, sv;

    dev->line_style = style;

    sv = (int) (_PG_gattrs.ps_dots_inch/24.0);
    lv = sv << 1;
    switch (style)
       {case LINE_DASHED :
             io_printf(dev->file, "[%d %d] 0 d\n", lv, sv);
             break;
        case LINE_DOTTED :
             io_printf(dev->file, "[%d %d] 0 d\n", sv, sv);
             break;
        case LINE_DOTDASHED :
             io_printf(dev->file, "[%d %d %d %d] 0 d\n", lv, sv, sv, sv);
             break;
        case LINE_SOLID :
        default         :
             io_printf(dev->file, "[] 0 d\n");
             break;};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_SET_LINE_WIDTH - set the line width */
 
void _PG_PS_set_line_width(PG_device *dev, double width)
   {double w;
 
    dev->line_width = width;
    w = _PG_gattrs.ps_dots_inch*width/24.0;

    io_printf(dev->file, "%7.3f w\n", w);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_SET_LINE_COLOR - set the line color */
 
void _PG_PS_set_line_color(PG_device *dev, int color, int mapped)
   {

    dev->line_color     = color;
    dev->map_line_color = mapped;

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_SET_TEXT_COLOR - set the color of the text */
 
void _PG_PS_set_text_color(PG_device *dev, int color, int mapped)
   {

    dev->text_color     = color;
    dev->map_text_color = mapped;

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_SET_FILL_COLOR - sets current fill color */

void _PG_PS_set_fill_color(PG_device *dev, int color, int mapped)
   {

    dev->fill_color     = color;
    dev->map_fill_color = mapped;

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_SET_CHAR_SIZE - set the character size in CS */

void _PG_PS_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x)
   {double p[PG_SPACEDM];

    PG_trans_point(dev, nd, cs, x, NORMC, p);

    dev->char_height_s = p[0];
    dev->char_width_s  = p[1];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_SET_CHAR_PATH - set the direction along which text will be written
 *                      - defaults to (1, 0)
 */

void _PG_PS_set_char_path(PG_device *dev, double x, double y)
   {

    dev->char_path[0] = x;
    dev->char_path[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_SET_CHAR_PRECISION - set the character precision
 *                           - fast and fixed size or
 *                           - slow and flexible
 */

void _PG_PS_set_char_precision(PG_device *dev, int p)
   {

    dev->char_precision = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_SET_CHAR_SPACE - set the space between characters */

void _PG_PS_set_char_space(PG_device *dev, double s)
   {

    dev->char_space = (double) s;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_SET_CHAR_UP - set the direction which is up for individual
 *                    - characters
 *                    - defaults to (0, 1)
 */

void _PG_PS_set_char_up(PG_device *dev, double x, double y)
   {

    dev->char_up[0] = x;
    dev->char_up[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_SET_CHAR_LINE - set the number characters per line */
 
void _PG_PS_set_char_line(PG_device *dev, int n)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_SET_CLIPPING - set clipping
 *                     - flag = FALSE  -->  clipping off
 *                     - flag = TRUE   -->  clipping on
 */

void _PG_PS_set_clipping(PG_device *dev, int flag)
   {int idx[PG_SPACEDM];
    int pc[PG_BOXSZ];

    if (flag)
       {if (dev->clipping)
           {_PG_PS_set_dev_color(dev, PG_NOPRIM, TRUE);
            io_printf(dev->file, "\n%% Clipping OFF\n");
            io_printf(dev->file, "Cf\n");};

        _PG_find_clip_region(dev, pc, flag, TRUE);

	idx[0] = pc[1] - pc[0];
	idx[1] = pc[3] - pc[2];

        io_printf(dev->file, "\n%% Clipping ON\n");
        io_printf(dev->file, "%d %d %d %d Cn\n",
		  idx[0], idx[1], pc[0], pc[2]);}
    
    else if (dev->clipping)
       {_PG_PS_set_dev_color(dev, PG_NOPRIM, TRUE);
        io_printf(dev->file, "\n%% Clipping OFF\n");
        io_printf(dev->file, "Cf\n");};
 
    dev->clipping = flag;

    return;}
 
/*--------------------------------------------------------------------------*/

/*                          MOVE AND DRAW ROUTINES                          */

/*--------------------------------------------------------------------------*/
 
/* _PG_PS_MOVE_GR_ABS - move the current graphics cursor position to the
 *                    - given absolute coordinates in WC
 */
 
void _PG_PS_move_gr_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	io_printf(dev->file, "%d %d m\n", (int) p[0], (int) p[1]);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_MOVE_TX_ABS - move the current text cursor position to the
 *                    - given coordinates in WC
 */
 
void _PG_PS_move_tx_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	io_printf(dev->file, "1 0 0 1 %d %d 0 Tp\n",
		  (int) p[0], (int) p[1]);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_MOVE_TX_REL - move the current text cursor position to the
 *                    - given relative coordinates in WC
 */
 
void _PG_PS_move_tx_rel(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_rel(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	io_printf(dev->file, "1 0 0 1 %d %d 0 Tp\n",
		  (int) p[0], (int) p[1]);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_DRAW_TO_ABS - draw a line from current position to
 *                    - absolute position (x, y)
 *                    - in WC
 */
 
void _PG_PS_draw_to_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	_PG_PS_set_dev_color(dev, PG_LINE, TRUE);

	io_printf(dev->file, "%d %d L S\n", (int) p[0], (int) p[1]);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_DRAW_TO_REL - draw a line from current position to
 *                    - relative position (x, y)
 *                    - in WC
 */
 
void _PG_PS_draw_to_rel(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_rel(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);

	_PG_PS_set_dev_color(dev, PG_LINE, TRUE);

	io_printf(dev->file, "%d %d rlineto S\n",
		  (int) p[0], (int) p[1]);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_DRAW_CURVE - draw a PG_curve */
 
void _PG_PS_draw_curve(PG_device *dev, PG_curve *crv, int clip)
   {int i, flag, n, xo, yo, ix, ixo, iy, iyo, nl;
    int pc[PG_BOXSZ];
    int *x, *y;
    
    flag = TRUE;

    _PG_PS_set_dev_color(dev, PG_LINE, FALSE);

    _PG_find_clip_region(dev, pc, dev->clipping, TRUE);
    
    nl = 0;
    n  = crv->n;
    x  = crv->x;
    y  = crv->y;
    xo = crv->x_origin;
    yo = crv->y_origin;

    if (pc[2] > pc[3])
       {i   = pc[2];
        pc[2] = pc[3];
        pc[3] = i;};
    
/* make ZeroFault happy by initializing this here */
    ixo = 0;
    iyo = 0;

    for (i = 0; i < n; i++)
        {ixo = ix;
         iyo = iy;
         ix  = x[i] + xo;
         iy  = y[i] + yo;
       
         if (i == 0)
            continue;
    
         if ((ix == ixo) && (iy == iyo))
            continue;

         if (clip && dev->clipping)
	    {if (((ix < pc[0]) && (ixo < pc[0])) ||
		 ((ix > pc[1]) && (ixo > pc[1])))
	       {flag = TRUE;
		continue;}

	     if (((iy < pc[2]) && (iyo < pc[2])) ||
		 ((iy > pc[3]) && (iyo > pc[3])))
	       {flag = TRUE;
		continue;};}

	 if (flag)
	    {io_printf(dev->file, "%d %d m\n", ixo, iyo);
	     flag = FALSE;}

         io_printf(dev->file, "%d %d L\n", ix, iy);

         if (++nl >= 256)
            {nl = 0;
	     io_printf(dev->file, "S\n");
             io_printf(dev->file, "%d %d m\n", ix, iy);};};

    io_printf(dev->file, "S\n");

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_DRAW_DISJOINT_POLYLINE_2 - draws disjoint two dimensional
 *                                 - line segments specified in CS
 */

void _PG_PS_draw_disjoint_polyline_2(PG_device *dev, double **r,
                                     long n, int clip, PG_coord_sys cs)
   {int i;
    int ix1[PG_SPACEDM], ix2[PG_SPACEDM], pc[PG_BOXSZ];
    double *px, *py;
    double box[PG_BOXSZ];

    if (dev == NULL)
       return;

    _PG_PS_set_dev_color(dev, PG_LINE, TRUE);

    PG_trans_points(dev, 2*n, 2, cs, r, WORLDC, r);

    px = r[0];
    py = r[1];

    _PG_find_clip_region(dev, pc, dev->clipping, TRUE);

    for (i = 0; i < n; i++)
        {box[0] = *px++;
         box[2] = *py++;
         box[1] = *px++;
         box[3] = *py++;
         
         if (cs == WORLDC)
            PG_log_space(dev, 2, FALSE, box);

	 PG_trans_box(dev, 2, WORLDC, box, PIXELC, box);

	 ix1[0] = box[0];
	 ix1[1] = box[2];
	 ix2[0] = box[1];
	 ix2[1] = box[3];

         if ((ix1[0] == ix2[0]) && (ix1[1] == ix2[1]))
            continue;

/* do simple clipping */
         if ((dev->clipping == TRUE) &&
             (((ix1[0] < pc[0]) && (ix2[0] < pc[0])) ||
              ((ix1[0] > pc[1]) && (ix2[0] > pc[1])) ||
              ((ix1[1] < pc[2]) && (ix2[1] < pc[2])) ||
              ((ix1[1] > pc[3]) && (ix2[1] > pc[3]))))
            continue;

         io_printf(dev->file, "%d %d m %d %d L S\n",
		   ix1[0], ix1[1], ix2[0], ix2[1]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_SHADE_POLY - polygon shading routine */

void _PG_PS_shade_poly(PG_device *dev, int nd, int n, double **r)
   {int i;
    double x[PG_SPACEDM], ix[PG_SPACEDM];

    if (nd == 3)
       _PG_rst_shade_poly(dev, nd, n, r);

    else
       {_PG_PS_set_dev_color(dev, PG_FILL, TRUE);

	x[0] = r[0][0];
	x[1] = r[1][0];

	PG_trans_point(dev, 2, WORLDC, x, PIXELC, ix);

	io_printf(dev->file, "newpath\n");
	io_printf(dev->file, "%d %d m\n", (int) ix[0], (int) ix[1]);

	for (i = 1; i < n; i++)
	    {x[0] = r[0][i];
	     x[1] = r[1][i];
	     PG_trans_point(dev, 2, WORLDC, x, PIXELC, ix);

	     io_printf(dev->file, "%d %d L\n", (int) ix[0], (int) ix[1]);};

	io_printf(dev->file, "closepath\n");

	io_printf(dev->file, "fill\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_PS_FILL_CURVE - fill a closed PG_curve */

void _PG_PS_fill_curve(PG_device *dev, PG_curve *crv)
   {int i, n, xo, yo;
    int *x, *y;

    _PG_PS_set_dev_color(dev, PG_FILL, TRUE);

    n  = crv->n;
    x  = crv->x;
    y  = crv->y;
    xo = crv->x_origin;
    yo = crv->y_origin;

    io_printf(dev->file, "newpath\n");
    io_printf(dev->file, "%d %d m\n", x[0]+xo, y[0]+yo);

    for (i = 1; i < n; i++)
        io_printf(dev->file, "%d %d L\n", x[i]+xo, y[i]+yo);

    io_printf(dev->file, "closepath\n");
    io_printf(dev->file, "fill\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_PUT_IMAGE - put the image on the screen
 *                  - the image buffer may be overwritten by the pseudo
 *                  - color mapping if it is needed!!
 */

void _PG_PS_put_image(PG_device *dev, unsigned char *bf,
                      int ix, int iy, int nx, int ny)
   {int l, k, i, bpp, cd, tr;
    int n_pal_colors, n_dev_colors;
    unsigned int color, a, r, g, b, y;
    double sf, rv, scale;
    unsigned char *pbf;
    PG_palette *pal;
    RGB_color_map *pseudo_cm, *true_cm;
    FILE *fp;

    pal          = dev->current_palette;
    n_dev_colors = dev->absolute_n_color;
    n_pal_colors = pal->n_pal_colors;
    pseudo_cm    = pal->pseudo_colormap;
    true_cm      = pal->true_colormap;

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

    pbf = bf;
    bpp = log((double) (dev->absolute_n_color))/log(2.0) + 0.5;
    fp  = dev->file;
    if (bpp == 1)
       nx = _PG_byte_bit_map(bf, nx, ny, TRUE);

    sf = dev->resolution_scale_factor;

    cd = COLOR_POSTSCRIPT_DEVICE(dev);
    tr = dev->ps_transparent;

    io_printf(fp, "\n%% Image\n");
    io_printf(fp, "%d %d %d %d %d %f ImB\n",
	      nx, ny, bpp, ix, iy, sf);
    
/* color */
    if (cd == TRUE)
       {scale = 255.0/((double) MAXPIX);

        for (l = 0; l < ny; l++)
            {pbf = bf + l*nx;
             for (k = 0; k < nx; k++)
                 {color = *pbf++;
                  r = scale*true_cm[color].red;
                  g = scale*true_cm[color].green;
                  b = scale*true_cm[color].blue;

		  a = 0;
		  r = min(r, 255);
		  g = min(g, 255);
		  b = min(b, 255);

		  if (tr == TRUE)
		     io_printf(fp, "%02x%02x%02x%02x", a, r, g, b);
	          else
		     io_printf(fp, "%02x%02x%02x", r, g, b);};

	     io_printf(fp, "\n");};}

/* monochrome */
    else
       {scale = 255.0/((double) (n_pal_colors - 1));

        for (l = 0; l < ny; l++)
            {pbf = bf + l*nx;
             for (k = 0; k < nx; k++)
	         {y = scale*(*pbf++ - 2);
		  a = 0;
		  if (tr == TRUE)
		     io_printf(fp, "%02x%02x", a, y);
		  else
		     io_printf(fp, "%02x", y);};

	     io_printf(fp, "\n");};};

    if (dev->ps_level >= 3.0)
       io_printf(fp, ">\n");
    io_printf(fp, "ImE\n");
    io_printf(fp, "%% End of Image\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_PUT_RASTER - put the raster on the screen
 *                   - the raster buffer may be overwritten by the pseudo
 *                   - color mapping if it is needed!!
 */

int _PG_PS_put_raster(PG_device *dev,
		      int xo, int yo, int nx, int ny)
   {int ix, iy, bpp, l, cd, tr;
    int xmn, xmx, ymx, ymn;
    unsigned int a, r, g, b, y;
    double sf;
    FILE *fp;
    PG_RAST_device *mdv;
    frame *fr;
    RGB_color_map c;

    GET_RAST_DEVICE(dev, mdv);
    if (mdv == NULL)
       return(FALSE);

    fr = mdv->raster;
    if (fr == NULL)
       return(FALSE);

    if (nx < 0)
       nx = fr->width;
    if (ny < 0)
       ny = fr->height;

/* for consistency with _PG_clear_raster_box where pixels
 * are cleared to dev->BLACK
 */
    c = fr->bc;

    bpp = log((double) (dev->absolute_n_color))/log(2.0) + 0.5;
    fp  = dev->file;

    sf = dev->resolution_scale_factor;

    cd = COLOR_POSTSCRIPT_DEVICE(dev);
    tr = dev->ps_transparent;

/* write the raster if it is valid */
    if ((fr->box[0] < fr->box[1]) && (fr->box[2] < fr->box[3]))
       {xmn = fr->box[0] + 0.5;
	xmx = fr->box[1] + 0.5;
	ymn = fr->box[2] + 0.5;
	ymx = fr->box[3] + 0.5;

	xo = sf*(xo + xmn) + dev->window_x[0];
	yo = sf*(yo + ymn) + dev->window_x[2];

	io_printf(fp, "\n%% Raster\n");
	io_printf(fp, "%d %d %d %d %d %f ImB\n",
		  xmx - xmn + 1, ymx - ymn + 1, bpp, xo, yo, sf);
    
	for (iy = ymn; iy <= ymx; iy++)
	    {for (ix = xmn; ix <= xmx; ix++)
	         {l = iy*nx + ix;

		  r = fr->r[l];
		  g = fr->g[l];
		  b = fr->b[l];

		  a = 0;
		  r = min(r, 255);
		  g = min(g, 255);
		  b = min(b, 255);

		  if ((r == c.red) && (g == c.green) && (b == c.blue))
		     {a = 255;
		      r = 255;
		      g = 255;
		      b = 255;};
		      
/* color */
		  if (cd == TRUE)
		     {if (tr == TRUE)
			 io_printf(fp, "%02x%02x%02x%02x", a, r, g, b);
		      else
			 io_printf(fp, "%02x%02x%02x", r, g, b);}

/* monochrome */
		  else
		     {y = PG_luminance(r, g, b);
		      y = min(y, 255);
		      if (tr == TRUE)
			 io_printf(fp, "%02x%02x", a, y);
		      else
			 io_printf(fp, "%02x", y);};};

	     io_printf(fp, "\n");};

	if (dev->ps_level >= 3.0)
	   io_printf(fp, ">\n");
	io_printf(fp, "ImE\n");
	io_printf(fp, "%% End of Raster\n");};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PS_SET_DEV_COLOR - */

void _PG_PS_set_dev_color(PG_device *dev, int prim, int check)
   {int color, rgb;
    PG_palette *pal;

    if (dev == NULL)
       return;

    color = -1;

    switch (prim)
       {case PG_TEXT :
	     rgb   = dev->text_color;
             color = (dev->map_text_color) ?
	             _PG_trans_color(dev, rgb) : rgb;
             break;

        case PG_LINE :
	     rgb   = dev->line_color;
             color = (dev->map_line_color) ?
		     _PG_trans_color(dev, rgb) : rgb;
             break;

        case PG_FILL :
	     rgb   = dev->fill_color;
             color = (dev->map_fill_color) ?
	             _PG_trans_color(dev, rgb) : rgb;
             break;

        case PG_NOPRIM :
	     _PG.last_prim = prim;
             return;};

    if (check && (dev == _PG.last_dev) && (prim == _PG.last_prim) &&
	(color == _PG.last_color))
       return;

    else
       {_PG.last_dev   = dev;
        _PG.last_prim  = prim;
        _PG.last_color = color;};
 
    if (PG_is_true_color(rgb) == TRUE)
       pal = dev->color_table;
    else
       pal = dev->current_palette;

    if (COLOR_POSTSCRIPT_DEVICE(dev))
       {RGB_color_map *true_cm;
        double r, g, b, scale;

        scale = 1.0/((double) MAXPIX);

        true_cm = pal->true_colormap;
        r = ABS(scale*true_cm[color].red);
        g = ABS(scale*true_cm[color].green);
        b = ABS(scale*true_cm[color].blue);

        io_printf(dev->file, "%.2f %.2f %.2f Srgb\n", r, g, b);}

    else
       {double val;

/* NOTE: in a line oriented application such as ULTRA you want MONOCHROME
 * to mean black on white NOT a grayscale
 * (we've gotten user complaints about that)
 * this test is specifically checking for the standard palette
 * and that the colors have been setup via Color_Map for the MONOCHROME
 * case
 */
	if (strcmp(pal->name, "standard") == 0)
	   val = (color == dev->BLACK) ? 1.0 : 0.0;

	else
	   val = ((double) (color)) / ((double) (pal->n_pal_colors - 1));

        io_printf(dev->file, "%7.3f setgray\n", val);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

