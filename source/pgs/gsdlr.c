/*
 * GSDLR.C - PGS raster line drawing primitive routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static INLINE void _PG_set_pixel_2d(PG_device *dev, frame *fr, int *ir,
				    double lw, RGB_color_map *clr)
   {int i, j, m, lwd;
    int nx, ny, np;
    int lr[PG_SPACEDM];

    lwd = 3.0*max(lw, 0.0);
    nx  = fr->width;
    ny  = fr->height;
    np  = nx*ny;

    for (i = -lwd/2; i <= (lwd+1)/2; i++)
        {lr[0] = ir[0] + i;
         for (j = -lwd/2; j <= (lwd+1)/2; j++)
	     {lr[1] = ir[1] + j;
	      PG_QUAD_FOUR_POINT(dev, lr);

              m = lr[0] + lr[1]*nx;
              if ((0 <= m) && (m < np))
		 SET_RASTER_PIXEL(fr, clr, ir[0], ir[1], 0.0);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static INLINE void _PG_set_pixel_3d(PG_device *dev, frame *fr,
				    int *ir, double z,
				    double lw, RGB_color_map *clr)
   {int i, j, m, lwd;
    int nx, ny, np;
    int lr[PG_SPACEDM];

    lwd = 3.0*max(lw, 0.0);
    nx  = fr->width;
    ny  = fr->height;
    np  = nx*ny;

    for (i = -lwd/2; i <= (lwd+1)/2; i++)
        {lr[0] = ir[0] + i;
         for (j = -lwd/2; j <= (lwd+1)/2; j++)
	     {lr[1] = ir[1] + j;
	      PG_QUAD_FOUR_POINT(dev, lr);

              m = lr[0] + lr[1]*nx;
              if ((0 <= m) && (m < np) && (z > fr->zb[m]))
		 SET_RASTER_PIXEL(fr, clr, ir[0], ir[1], z);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_LINE_STYLE - return pixel level line style info */

static int _PG_rst_line_style(int *dl, int ls, double lw)
   {int i, nn, lwd;

/* make ZeroFault happy by initializing this here */
    for (i = 0; i < 4; i++)
        dl[0] = 0;

    lwd = 3.0*max(lw, 0.0);
    nn  = 0;
    switch (ls)
       {case LINE_DASHED:
            nn    = 2;
            dl[0] = 7;
            dl[1] = 3+lwd;
	    break;

        case LINE_DOTTED:
            nn    = 2;
            dl[0] = 3;
            dl[1] = 3+lwd;
	    break;

        case LINE_DOTDASHED:
            nn    = 4;
            dl[0] = 7;
            dl[1] = 3+lwd;
            dl[2] = 3;
            dl[3] = 3+lwd;
	    break;

        case LINE_SOLID:
        default:
	    break;};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_CLIP_LINE - clip the line X1 to X2
 *                   - to the box (xmn, ymn) to (xmx, ymx)
 *                   - return TRUE (visible) or FALSE (invisible)
 *                   - if visible, return new clipped points
 *                   - if not visible, no change to A and B.
 */

static int _PG_rst_clip_line(PG_device *dev, double *x1, double *x2)
   {int visible, count, i, sa, sb, inter, aorb;
    int ac[4], bc[4];
    int nchk;
    double m, x, y;
    double a[PG_SPACEDM], b[PG_SPACEDM], p[PG_SPACEDM];
    double pc[PG_BOXSZ], wc[PG_BOXSZ];

    if (dev->clipping == FALSE)
       return(TRUE);

    a[0] = x1[0];
    b[0] = x2[0];
    a[1] = x1[1];
    b[1] = x2[1];

    PG_get_viewspace(dev, WORLDC, wc);
    PG_log_space(dev, 2, TRUE, wc);
    PG_trans_box(dev, 2, WORLDC, wc, PIXELC, pc);

/* first end point A */
    ac[3] = (a[0] < pc[0]);
    ac[2] = (a[0] > pc[1]);
    ac[1] = (a[1] < pc[2]);
    ac[0] = (a[1] > pc[3]);

/* second end point B */
    bc[3] = (b[0] < pc[0]);
    bc[2] = (b[0] > pc[1]);
    bc[1] = (b[1] < pc[2]);
    bc[0] = (b[1] > pc[3]);

/* initialize the visibility flag, 
 * the drawing points (a[0], a[1]) and (b[0], b[1]),
 * and the counter
 */
    count   = 0;
    visible = TRUE;
    aorb    = 0;
    m       = DBL_MAX;   /* initialize to an infinite slope */

/* check for totally visible line */
    sa = 0;
    sb = 0;
    for (i = 0; i < 4; i++)
        {sa += ac[i];
	 sb += bc[i];};

    if ((sa == 0) && (sb == 0))
       return(visible);

/* line is not totally visible - check for trivial invisible case */

/* calculate intersection of A & B */
    inter = 0;
    for (i = 0; i < 4; i++)
        {inter += (ac[i] + bc[i])/2;
	 if (inter != 0)
	    {visible = FALSE;
	     return(visible);};};

/* line may be partially visible
 * check for first point inside window
 */
    nchk = FALSE;
    if (sa == 0)
       {count = 2;
	aorb  = 2;
	p[0]  = b[0];
	p[1]  = b[1];
	nchk  = TRUE;};

/* check for second point inside window */
    if (sb == 0)
       {count = 2;
	aorb  = 1;
	p[0]  = a[0];
	p[1]  = a[1];
	nchk  = TRUE;};

    while (visible == TRUE)

/* neither end point inside window */
       {if (nchk == FALSE)
	   {if (aorb == 1)
	       {a[0] = p[0];
		a[1] = p[1];};

	    if (aorb == 2)
	       {b[0] = p[0];
		b[1] = p[1];};

	    count++;
	    if (count > 2)
	       break;

	    p[0] = (count == 1) ? a[0] : b[0];
	    p[1] = (count == 1) ? a[1] : b[1];
	    aorb = (count == 1) ? 1 : 2;};

	nchk = FALSE;

/* non-vertical line */
	if ((b[0] - a[0]) != 0)

/* check the left-side intercept */
	   {m = (b[1] - a[1])/(b[0] - a[0]);
	    if (p[0] <= pc[0])
	       {y = m*(pc[0] - p[0]) + p[1];
	        if ((pc[2] <= y) && (y <= pc[3]))
		   {p[0] = pc[0];
		    p[1] = y;
		    continue;};};

/* check the right-side intercept */
	    if (pc[1] <= p[0])
	       {y = m*(pc[1] - p[0]) + p[1];
		if ((pc[2] <= y) && (y <= pc[3]))
		   {p[0] = pc[1];
		    p[1] = y;
		    continue;};};};

/* non-horizontal line */
	if (m != 0)

/* check the top edge intercept */
	   {if (pc[3] <= p[1])
	       {x = (1/m)*(pc[3] - p[1]) + p[0];
		if ((pc[0] <= x) && (x <= pc[1]))
		   {p[0] = x;
		    p[1] = pc[3];
		    continue;};};

/* check the bottom intercept */
	    x = (1/m)*(pc[2] - p[1]) + p[0];
	    if ((pc[0] <= x) && (x <= pc[1]))
	       {p[0] = x;
		p[1] = pc[2];
		continue;};

/* the line is doubly invisible */
	    visible = FALSE;};};

/* get clipping points P1 and P2 */
    if (visible == TRUE)
       {x1[0] = a[0];
	x1[1] = a[1];
	x2[0] = b[0];
	x2[1] = b[1];};

    return(visible);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_DRAW_LINE_2D - draw a line on the current frame buffer */

void _PG_rst_draw_line_2d(PG_device *dev, int *i1, int *i2)
   {int i, dxi, dyi, visible, rgb;
    int limit, il, icnt, lim1, nn, sp, dl[4];
    int ls, lc;
    int ir[PG_SPACEDM];
    double lw, dxr, dyr, rx, ry;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    RGB_color_map clr;
    PG_palette *pal;
    PG_RAST_device *mdv;
    frame *fr;

    x1[0] = i1[0];
    x1[1] = i1[1];
    x2[0] = i2[0];
    x2[1] = i2[1];

    visible = _PG_rst_clip_line(dev, x1, x2);
    if (visible == FALSE)
       return;

    ls  = dev->line_style;
    lc  = dev->line_color;
    lw  = dev->line_width;
    pal = dev->current_palette;

    GET_RAST_DEVICE(dev, mdv);

    rgb = mdv->rgb_mode;
    fr  = mdv->raster;

/* take negative LC to mean the background color - dev->BLACK */
    if (lc < 0)
       lc = dev->BLACK;

    nn = _PG_rst_line_style(dl, ls, lw);

    PG_rgb_color(&clr, lc, rgb, pal);

    dxi = i2[0] - i1[0];
    dyi = i2[1] - i1[1];

    if (abs(dxi) > abs(dyi))
       {dxr = (dxi > 0) ? 1.0 : -1.0 ;
        dyr = ((double) abs(dyi)) / ((double) abs(dxi));
        if (dyi < 0)
	   dyr = -dyr;}

    else if (abs(dyi) > abs(dxi))
       {dyr = (dyi > 0) ? 1.0 : -1.0 ;
        dxr = ((double) abs(dxi)) / ((double) abs(dyi));
        if (dxi < 0)
	   dxr = -dxr;}
    else
       {dxr = (dxi > 0) ? 1.0 : -1.0;
        dyr = (dyi > 0) ? 1.0 : -1.0;};

    il   = 1;
    icnt = 0;
    lim1 = dl[0];

/* should we use "+ 0.49" or not ? */
    limit = max(ABS(x1[0]-x2[0]), ABS(x1[1]-x2[1])) + 0.49;
    rx = x1[0];
    ry = x1[1];
    ir[0] = (int) (rx + 0.49);
    ir[1] = (int) (ry + 0.49);
    _PG_set_pixel_2d(dev, fr, ir, lw, &clr);

    for (i = 1; i < limit; i++)
        {rx += dxr;
         ry += dyr;

         ir[0] = (int) (rx + 0.49);
         ir[1] = (int) (ry + 0.49);

	 il++;
	 sp = FALSE;
	 switch (nn)
	    {case 0:                /* LINE_SOLID or default */
	          sp = TRUE;
	          break;
	     default:               /* DASHED, DOTTED or DOTDASHED */
		  sp = (((icnt % 2 == 0) && (il <= lim1)) || (il > lim1));
		  if (il > lim1)
		     {icnt++;
		      lim1 += dl[icnt % nn];};
		  break;};

	if (sp == TRUE)
	   _PG_set_pixel_2d(dev, fr, ir, lw, &clr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_DRAW_LINE_3D - draw a 3D line in PC on the current frame buffer
 *                      - the z coordinate determines who is on top
 */

void _PG_rst_draw_line_3d(PG_device *dev, int *i1, int *i2)
   {int i, id, dxi, dyi, visible, rgb;
    int limit, il, icnt, lim1, nn, sp, dl[4];
    int ls, lc, wox, woy;
    int ir[PG_SPACEDM];
    double lw, sf, dxr, dyr, dzr, rx, ry, rz;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    RGB_color_map clr;
    PG_palette *pal;
    PG_RAST_device *mdv;
    frame *fr;
    static long count = 0;

    count++;

    for (id = 0; id < 3; id++)
        {x1[id] = i1[id];
	 x2[id] = i2[id];};

    visible = _PG_rst_clip_line(dev, x1, x2);
    if (visible == FALSE)
       return;

    wox = dev->window_x[0];
    woy = dev->window_x[2];
    ls  = dev->line_style;
    lc  = dev->line_color;
    lw  = dev->line_width;
    pal = dev->current_palette;

    mdv = _PG_get_raster(dev, TRUE);

    rgb = mdv->rgb_mode;
    fr  = mdv->raster;

/* GOTCHA: why isn't this like the case in _PG_rst_draw_line_2d */
    if (lc < 0)
       lc = dev->BLACK;

    _PG_frame_z_buffer(fr);

    sf = 1.0/dev->resolution_scale_factor;

    nn = _PG_rst_line_style(dl, ls, lw);

    PG_rgb_color(&clr, lc, rgb, pal);

    dxi = i2[0] - i1[0];
    dyi = i2[1] - i1[1];
    dzr = i2[2] - i1[2];

    if (abs(dxi) > abs(dyi))
       {dxr = (dxi > 0) ? 1.0 : -1.0 ;
        dyr = ((double) abs(dyi)) / ((double) abs(dxi));
        if (dyi < 0)
	   dyr = -dyr;}

    else if (abs(dyi) > abs(dxi))
       {dyr = (dyi > 0) ? 1.0 : -1.0 ;
        dxr = ((double) abs(dxi)) / ((double) abs(dyi));
        if (dxi < 0)
	   dxr = -dxr;}
    else
       {dxr = (dxi > 0) ? 1.0 : -1.0;
        dyr = (dyi > 0) ? 1.0 : -1.0;};

    il   = 1;
    icnt = 0;
    lim1 = dl[0];

/* should we use "+ 0.49" or not ? */
    limit = max(ABS(x1[0]-x2[0]), ABS(x1[1]-x2[1])) + 0.49;
    if (limit > 1)
       dzr /= ((double) limit - 1.0);
    else
       dzr = 1.0;

    rx = i1[0] - wox;
    ry = i1[1] - woy;
    rz = i1[2];
    ir[0] = (int) (sf*rx + 0.49);
    ir[1] = (int) (sf*ry + 0.49);
    _PG_set_pixel_3d(dev, fr, ir, rz, lw, &clr);

    for (i = 1; i < limit; i++)
        {rx += dxr;
         ry += dyr;
         rz += dzr;
         ir[0]  = (int) (sf*rx + 0.49);
         ir[1]  = (int) (sf*ry + 0.49);

/* handle intervals for non-line_solid lines */
	 il++;
	 sp = FALSE;
	 switch (nn)
	    {case 0:                /* LINE_SOLID or default */
	          sp = TRUE;
	          break;
	     default:               /* DASHED, DOTTED or DOTDASHED */
		  sp = (((icnt % 2 == 0) && (il <= lim1)) || (il > lim1));
		  if (il > lim1)
		     {icnt++;
		      lim1 += dl[icnt % nn];};
		  break;};

	if (sp == TRUE)
	   _PG_set_pixel_3d(dev, fr, ir, rz, lw, &clr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DACRST - diagnostic print of number of red, green, and blue pixels
 *        - the raster frame FR
 */

void dacrst(frame *fr)
   {int l, ix, iy, nx, ny;
    int nr, ng, nb;
    unsigned int r, g, b;

    if (fr != NULL)
       {nx = fr->width;
        ny = fr->height;

	nr = 0;
	ng = 0;
	nb = 0;
	for (iy = 0; iy < ny; iy++)
	    {for (ix = 0; ix < nx; ix++)
	         {l = iy*nx + ix;

		  r = fr->r[l];
		  g = fr->g[l];
		  b = fr->b[l];

		  nr += (r != 255);
		  ng += (g != 255);
		  nb += (b != 255);};};

	io_printf(stdout, "# on = %d, # red = %d, # green = %d, # blue = %d\n",
		  fr->n_on, nr, ng, nb);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRST - do ASCII display of raster components
 *       - do not try this with a large image
 */

void dprst(frame *fr)
   {int i, j, l, w, h;
    char *s;
    unsigned char bc, *a;
    double *z;

    w = fr->width;
    h = fr->height;

    bc = '\377';

    s = FMAKE_N(char, w, "DPRRST:s");

    a = fr->r;
    io_puts("Red component:\n", stdout);
    for (j = 0; j < h; j++)
        {for (i = 0; i < w; i++)
	     {l = j*w + i;
	      s[i] = (a[l] != bc) ? '.' : ' ';};
	 if (strchr(s, '.') != NULL)
	    io_printf(stdout, "%5d: %s\n", j, s);};
    io_puts("\n", stdout);

    a = fr->g;
    io_puts("Green component:\n", stdout);
    for (j = 0; j < h; j++)
        {for (i = 0; i < w; i++)
	     {l = j*w + i;
	      s[i] = (a[l] != bc) ? '.' : ' ';};
	 if (strchr(s, '.') != NULL)
	    io_printf(stdout, "%5d: %s\n", j, s);};
    io_puts("\n", stdout);

    a = fr->b;
    io_puts("Blue component:\n", stdout);
    for (j = 0; j < h; j++)
        {for (i = 0; i < w; i++)
	     {l = j*w + i;
	      s[i] = (a[l] != bc) ? '.' : ' ';};
	 if (strchr(s, '.') != NULL)
	    io_printf(stdout, "%5d: %s\n", j, s);};
    io_puts("\n", stdout);

    z = fr->zb;
    io_puts("Z buffer:\n", stdout);
    for (j = 0; j < h; j++)
        {for (i = 0; i < w; i++)
	     {l = j*w + i;
	      s[i] = (z[l] != -HUGE) ? '.' : ' ';};
	 if (strchr(s, '.') != NULL)
	    io_printf(stdout, "%5d: %s\n", j, s);};
    io_puts("\n", stdout);

    SFREE(s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
