/*
 * GSTX_X.C - raster text using X
 *
 */

#include "pgs_int.h"

#ifdef HAVE_X11

static Display
 *_PG_X_txt_display = NULL;

static char
 *backup_font[] = {"*-medium-r-*-12-*",
                   "*-r-*-12-*",
                   "*-r-*-13-*",
                   "*-r-*-14-*"};

extern int
 _PG_X_init_palette(PG_device *dev, int ndvc, unsigned long *fbc),
 _PG_X_setup_geom(PG_device *dev, double *xf, double *dxf,
		  int *dx, int *wx, int *wdx);

extern GC
 _PG_X_open_window(PG_device *dev, int *wx, int *wdx,
		   unsigned long *fbc, int map);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_TXT_CONNECT_SERVER - connect to the X server
 *                          - and register call backs for interrupt driven I/O
 *                          - on stdin and the connection to the server
 *                          - do this at most one time in a program
 */

static void _PG_X_txt_connect_server(PG_device *dev)
   {char *window_name;

    if (_PG_X_txt_display == NULL)

/* try to open the connection from the PGS device */
       {if (strcmp(dev->name, "WINDOW") != 0)
           _PG_X_txt_display = XOpenDisplay(dev->name);

/* try to open the connection from the DISPLAY variable */
        if (_PG_X_txt_display == NULL)
           {window_name       = getenv("DISPLAY");
            _PG_X_txt_display = XOpenDisplay(window_name);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_TXT_QUERY_SCREEN - query some physical device characteristics */

static void _PG_X_txt_query_screen(PG_device *dev, int *pdx, int *pnc)
   {int id, nc, screen, n_planes;
    int dx[PG_SPACEDM];

/* connect to X server once only */
    _PG_X_txt_connect_server(dev);

    if (_PG_X_txt_display != NULL)
       {dev->display = _PG_X_txt_display;
 
	screen   = DefaultScreen(_PG_X_txt_display);
	n_planes = DisplayPlanes(_PG_X_txt_display, screen);

	dx[0] = DisplayWidth(_PG_X_txt_display, screen);
	dx[1] = DisplayHeight(_PG_X_txt_display, screen);
	nc    = 1 << n_planes;}

    else
       {dx[0] = 0;
	dx[1] = 0;
	nc    = 0;};

    for (id = 0; id < 2; id++)
        pdx[id] = dx[id];

    *pnc = nc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_TXT_SETUP_FONT - load the named font for X if it exists */

static int _PG_X_txt_setup_font(PG_device *dev, char *bf)
   {char **names;
    int i, nf, ret;
    XFontStruct *xf;
    Display *disp;

    ret  = FALSE;
    disp = dev->display;
    if (disp != NULL)
       {names = XListFonts(disp, bf, 1024, &nf);
	for (i = 0; i < nf; i++)
	    {xf = XLoadQueryFont(disp, names[i]);
	     if (xf != NULL)
	        {XSetFont(disp, dev->gc, xf->fid);

		 if (dev->font_info != NULL)
		    XFreeFont(disp, dev->font_info);

		 dev->font_info = xf;

		 ret = TRUE;
		 break;};};

	XFreeFontNames(names);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_TXT_OPEN_SCREEN - initialize a drawable for doing text */
 
static PG_device *_PG_X_txt_open_screen(PG_device *dev,
					double dxfo, double dyfo)
   {int i, ndvc;
    int dx[PG_SPACEDM], wx[PG_SPACEDM], wdx[PG_SPACEDM];
    unsigned long fbc[2];
    double xf[PG_SPACEDM], dxf[PG_SPACEDM];
    PG_font_family *ff;
    XWindowAttributes windowattr;

    if (dev == NULL)
       return(NULL);

    dev->type_index = GRAPHIC_WINDOW_DEVICE;
    dev->quadrant   = QUAD_FOUR;

/* kind of by definition this must be TRUE - it is not always set
 * by default especially in applications using the non-C bindings
 */
    dev->use_pixmap = TRUE;

    PG_query_screen_n(dev, dx, &ndvc);
    if ((dx[0] == 0) && (dx[1] == 0) && (ndvc == 0))
       return(NULL);

    if (dev->display == NULL)
       return(NULL);
 
    xf[0] = 1.0e-8;
    xf[1] = 1.0e-8;

    dxf[0] = dxfo;
    dxf[1] = dyfo;

    _PG_X_setup_geom(dev, xf, dxf, dx, NULL, wdx);

    _PG_X_init_palette(dev, ndvc, fbc);

    dev->gc = _PG_X_open_window(dev, wx, wdx, fbc, FALSE);

/* the rest is like _PG_X_init_window - merge? */
    PG_dev_geometry *g;

    g = &dev->g;

    if (dev->use_pixmap)
       {int screen;
	unsigned int depth;

	screen = DefaultScreen(dev->display);
	depth  = XDefaultDepth(dev->display, screen);

	dev->pixmap = XCreatePixmap(dev->display, dev->window,
				    wdx[0], wdx[1], depth);};

/* compute the view port */
    _PG_default_viewspace(dev, TRUE);

    PG_init_viewspace(dev, TRUE);
 
/* get window width */
    XGetWindowAttributes(dev->display, dev->window, &windowattr);
 
/* set device pixel coordinate limits */
    g->cpc[0] = -16383 + dx[0];
    g->cpc[1] =  16383 - dx[0];
    g->cpc[2] = -16383 + dx[1];
    g->cpc[3] =  16383 - dx[1];
    g->cpc[4] = -16383;
    g->cpc[5] =  16383;

/* change the devices idea of window shape from NDC to pixels */
    g->hwin[0] = 0;
    g->hwin[1] = windowattr.width;
    g->hwin[2] = 0;
    g->hwin[3] = windowattr.height;
 
    SET_PC_FROM_HWIN(g);

/* set font */
    ff = PG_make_font_family(dev, "helvetica", NULL, 4,
                             "-helvetica-medium-r-",
                             "-helvetica-medium-o-",
                             "-helvetica-bold-r-",
                             "-helvetica-bold-o-");

    dev->font_family = ff;

    PG_fset_font(dev, "helvetica", "medium", 12);

    for (i = 0; dev->font_info == NULL; i++)
        {if (i > 3)
	    return(NULL);
	 _PG_X_txt_setup_font(dev, backup_font[i]);};

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_TXT_CLOSE_DEVICE - close a device */
 
void _PG_X_txt_close_device(void)
   {Display *disp;

    if (_PG.txtd != NULL)
       {disp = _PG.txtd->display;
	if (disp != NULL)
	   {if (_PG.txtd->window)
	       {XDestroyWindow(disp, _PG.txtd->window);
		XFlush(disp);};};

/* clean up the device */
	PG_rl_device(_PG.txtd);

	_PG.txtd = NULL;};

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_TXT_TRANS_COLOR - protect the color map */

static int _PG_X_txt_trans_color(PG_device *dev, int color)
   {int nc;

    nc = dev->ncolor - 1;

    color = max(0, color);
    color = min(nc, color);

    return(color);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_TXT_CLEAR_WINDOW - clear the screen */
 
static void _PG_X_txt_clear_window(PG_device *dev, int clr)
   {int color;
    unsigned int ipc[PG_BOXSZ];
    unsigned long *pi;
    Display *disp;

    disp = dev->display;
    if (disp != NULL)
       {ipc[0] = 0;
	ipc[1] = 0;
	ipc[2] = PG_window_width(dev);
	ipc[3] = PG_window_height(dev);

	clr   = _PG_X_txt_trans_color(dev, clr);
	pi    = dev->current_palette->pixel_value;
	color = pi[clr];

	_PG_X_fill_pixmap(dev, ipc, color);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_TXT_SET_FONT - set the character font */

static int _PG_X_txt_set_font(PG_device *dev, char *face,
			      char *style, int size)
   {int ret;
    extern int _PG_X_set_font(PG_device *dev,
			      char *face, char *style, int size);

    ret = _PG_X_set_font(dev, face, style, size);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_TXT_WRITE_TEXT - write out text to the appropriate device */
 
static void _PG_X_txt_write_text(PG_device *dev, char *s,
				 int color, int xpad, int *dx)
   {int ypad, st;
    int len, dir, asc, dsc;
    XCharStruct overall;
    XFontStruct *fnt;
    unsigned long *pi;
    Display *disp;

    disp = dev->display;
    if (disp == NULL)
       return;
 
/* setup the text color */
    color = _PG_X_txt_trans_color(dev, color);
    pi    = dev->current_palette->pixel_value;
    XSetForeground(disp, dev->gc, pi[color]);
 
/* get the text extent info in PC */
    dir = 0;
    asc = 0;
    dsc = 0;

    SC_MEM_INIT(XCharStruct, &overall);

    len = strlen(s);
    fnt = dev->font_info;
    XTextExtents(fnt, s, len, &dir, &asc, &dsc, &overall);
    
    dx[0] = overall.width;
    dx[1] = asc + dsc;

    ypad = asc;

    st = XDrawString(disp, dev->pixmap, dev->gc, xpad, ypad, s, len);
 
    SC_ASSERT(st == 0);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_TXT_PLACE_IMAGE - place the image from the drawable into IMG */

int _PG_X_txt_place_image(PG_device *src, int *dx,
			  PG_device *dst, int *xo, int bc, int pad)
   {int i, ix, iy, ok;
    int ri, gi, bi;
    int w, h, wow, bob;
    int pxss, bprs, ord;
    int ir[PG_SPACEDM], ip[PG_SPACEDM];
    unsigned long msk;
    unsigned char *sr, *dr, *dg, *db;
    double dc[PG_SPACEDM];
    XImage *xi;
    frame *img;
    PG_RAST_device *mdv;

    ok = FALSE;
    if ((src->display != NULL) && (dst->raster != NULL))
       {GET_RAST_DEVICE(dst, mdv);

	img = mdv->raster;

	w  = img->width;
	h  = img->height;

	SC_ASSERT(h != 0);

	dx[0] += 2*pad;

	if (dst->quadrant == QUAD_FOUR)
	   {xo[1] += (3*dx[1]/4);
	    PG_QUAD_FOUR_POINT(dst, xo);}
	else
	   xo[1] -= (3*dx[1]/4);

/* impose proper limits on XO and DX */
	for (i = 0; i < PG_SPACEDM; i++)
	    {dx[i] = max(dx[i], 1.01*src->g.pc[2*i]);
	     dx[i] = min(dx[i], 0.99*src->g.pc[2*i+1]);
	     xo[i] = max(xo[i], 0);};

	msk = AllPlanes;
	xi  = XGetImage(src->display, src->pixmap,
			0, 0, dx[0], dx[1], msk, ZPixmap);

	ord  = xi->byte_order;
	pxss = xi->bits_per_pixel >> 3;
	bprs = xi->bytes_per_line;

	PG_fget_char_path(dst, dc);

	for (iy = 0; iy < dx[1]; iy++)
	    {sr = (unsigned char *) (xi->data + bprs*iy);
	     if (ord)
	        {for (ix = 0; ix < dx[0]; ix++)
		     {ri  = sr[0];
		      gi  = sr[1];
		      bi  = sr[2];
		      sr += pxss;

		      ir[0] =  ix*dc[0] + iy*dc[1];
		      ir[1] = -ix*dc[1] + iy*dc[0];

		      ip[0] = xo[0] + ir[0];
		      ip[1] = xo[1] + ir[1];

		      dr = img->r + w*ip[1] + ip[0];
		      dg = img->g + w*ip[1] + ip[0];
		      db = img->b + w*ip[1] + ip[0];

		      img->n_on++;
		      *dr = ri;
		      *dg = gi;
		      *db = bi;};}
	     else
	        {for (ix = 0; ix < dx[0]; ix++)
		     {bi  = sr[0];
		      gi  = sr[1];
		      ri  = sr[2];
		      sr += pxss;

		      ir[0] =  ix*dc[0] + iy*dc[1];
		      ir[1] = -ix*dc[1] + iy*dc[0];

		      ip[0] = xo[0] + ir[0];
		      ip[1] = xo[1] + ir[1];

		      dr = img->r + w*ip[1] + ip[0];
		      dg = img->g + w*ip[1] + ip[0];
		      db = img->b + w*ip[1] + ip[0];

/* white on white - in spite of the name */
		      wow = ((bc == src->BLACK) &&
			     (ri == 0xff) && (gi == 0xff) && (bi == 0xff));

/* black on black - in spite of the name */
		      bob = ((bc == src->WHITE) &&
			     (ri == 0) && (gi == 0) && (bi == 0));

/* do not write pixels in background color if it is white or black */
		      if ((wow == FALSE) && (bob == FALSE))
			 {img->n_on++;
			  *dr = ri;
			  *dg = gi;
			  *db = bi;};};};};

	XDestroyImage(xi);

	ok = TRUE;};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_TXT_PLACE_TEXT - get the image from the drawable */

int _PG_X_txt_place_text(PG_device *src, int *dx,
			 PG_device *dst, int *xo, int bc, int pad)
   {int ok;
    int dir, asc, dsc;
    unsigned long msk;
    double dc[PG_SPACEDM];
    XCharStruct overall;
    XFontStruct *fnt;
    XImage *si;

    ok = FALSE;
    if ((src->display != NULL) && (dst->display != NULL))
       {fnt = dst->font_info;
	XTextExtents(fnt, "g", 1, &dir, &asc, &dsc, &overall);

	xo[1] -= asc;
	dx[0] += 2*pad;

	PG_fget_char_path(dst, dc);
	msk = AllPlanes;
	si  = XGetImage(src->display, src->pixmap,
			0, 0, dx[0], dx[1], msk, ZPixmap);

	ok = _PG_X_put_ximage(dst, si, bc, src->BLACK, src->WHITE,
			      xo, dx, dc[0], dc[1], FALSE);

	XDestroyImage(si);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_DRAW_TEXT - draw the specified text string in the specified raster
 *                 - gross cheat using X to draw and then retrieving the
 *                 - image
 */

int _PG_X_draw_text(PG_device *dev, char *s, double *x)
   {int id, nd;
    int ok, bc, fc, sz, pad;
    int ir[PG_SPACEDM], dx[PG_SPACEDM];
    double dc[PG_SPACEDM];
    char *face, *sty;

    nd = 2;
    ok = FALSE;

    for (id = 0; id < nd; id++)
        ir[id] = x[id];

    PG_fget_char_path(dev, dc);

    PG_QUAD_FOUR_POINT(dev, ir);

/* if the text path is horizontal left to right do the quick thing */
    if ((dc[0] == 1.0) && (dc[1] == 0.0))
       ok = XDrawString(dev->display, PG_X11_DRAWABLE(dev), dev->gc,
			ir[0], ir[1], s, strlen(s));

/* otherwise you asked for it */
    else
       {PG_fget_font(dev, &face, &sty, &sz);

	if (_PG.txtd == NULL)
	   {_PG.txtd = PG_make_device("SCREEN", "COLOR", "TextBuffer");
	    _PG_X_txt_open_screen(_PG.txtd, 0.3, 0.1);};

	if (_PG.txtd != NULL)
	   {pad = 2;

	    bc = dev->BLACK;
	    fc = PG_fget_text_color(dev);

	    _PG_X_txt_clear_window(_PG.txtd, bc);
	    _PG_X_txt_set_font(_PG.txtd, face, sty, sz);
	    _PG_X_txt_write_text(_PG.txtd, s, fc, pad, dx);
	    ok = _PG_X_txt_place_text(_PG.txtd, dx, dev, ir, bc, pad);};

	CFREE(face);
	CFREE(sty);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_DRAW_TEXT - draw the specified text string in the specified raster
 *                   - gross cheat using X to draw and then retrieving the
 *                   - image
 *                   -   (X,Y) - PC coordinate upper left corner origin
 *                   -   SZ    - font size in points
 *                   -   STY   - font style ("medium", "bold", "italic")
 *                   -   BC    - background color
 *                   -   FC    - foreground or text color
 */

int _PG_rst_draw_text(PG_device *dev, char *s)
   {int id, nd, bc, fc, sz;
    int ok, pad;
    int io[PG_SPACEDM], dx[PG_SPACEDM];
    double o[PG_SPACEDM];
    char *face, *sty;

    nd = 2;
    ok = FALSE;

    PG_fget_font(dev, &face, &sty, &sz);

    bc = dev->BLACK;
    fc = PG_fget_text_color(dev);

    for (id = 0; id < nd; id++)
        o[id] = dev->tcur[id];

    PG_trans_point(dev, 2, WORLDC, o, PIXELC, o);

    for (id = 0; id < nd; id++)
        io[id] = o[id];

    if (_PG.txtd == NULL)
       {_PG.txtd = PG_make_device("SCREEN", "COLOR", "TextBuffer");
	_PG_X_txt_open_screen(_PG.txtd, 0.3, 0.1);};

    if (_PG.txtd != NULL)
       {pad = 2;

	_PG_X_txt_clear_window(_PG.txtd, bc);
	_PG_X_txt_set_font(_PG.txtd, "helvetica", sty, sz);
	_PG_X_txt_write_text(_PG.txtd, s, fc, pad, dx);
	ok = _PG_X_txt_place_image(_PG.txtd, dx, dev, io, bc, pad);};

    CFREE(face);
    CFREE(sty);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RST_DRAW_TEXT - stub for when we do not have X11 */

int _PG_rst_draw_text(PG_device *dev, char *s)
   {int ok;

    ok = FALSE;

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

