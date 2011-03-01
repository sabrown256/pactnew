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

static void _PG_X_txt_query_screen(PG_device *dev, int *pdx, int *pdy,
				   int *pnc)
   {int screen, n_planes;

/* connect to X server once only */
    _PG_X_txt_connect_server(dev);

    if (_PG_X_txt_display != NULL)
       {dev->display = _PG_X_txt_display;
 
	screen   = DefaultScreen(_PG_X_txt_display);
	n_planes = DisplayPlanes(_PG_X_txt_display, screen);

	*pdx = DisplayWidth(_PG_X_txt_display, screen);
	*pdy = DisplayHeight(_PG_X_txt_display, screen);
	*pnc = 1 << n_planes;}

    else
       {*pdx = 0;
	*pdy = 0;
	*pnc = 0;};

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
 
static PG_device *_PG_X_txt_open_screen(PG_device *dev, double dxf, double dyf)
   {unsigned long bck_color, for_color, valuemask;
    unsigned int icon_width, icon_height, depth;
    int i, Lightest, Light, Light_Gray, Dark_Gray, Dark, Darkest;
    int Xargc, screen, win_x, win_y, win_width, win_height;
    int display_width, display_height, min_dim;
    int n_dev_colors, npc;
    char **Xargv, *window_name;
    double intensity;
    XSetWindowAttributes setwindattr;
    XWindowAttributes windowattr;
    XTextProperty wintp;
    XSizeHints size_hints;
    XGCValues values;
    GC xgc;
    PG_font_family *ff;
    PG_dev_geometry *g;

    if (dev == NULL)
       return(NULL);

    g = &dev->g;

    dev->type_index = GRAPHIC_WINDOW_DEVICE;
    dev->quadrant   = QUAD_FOUR;

    valuemask = 0;

    _PG_X_txt_query_screen(dev, &display_width, &display_height, &n_dev_colors);
    if ((display_width == 0) && (display_height == 0) &&
        (n_dev_colors == 0))
       return(NULL);

    if (dev->display == NULL)
       return(NULL);
 
    screen = DefaultScreen(dev->display);
    depth  = XDefaultDepth(dev->display, screen);

/* set device pixel coordinate limits */
    dev->g.cpc[0] = -16383 + display_width;
    dev->g.cpc[1] =  16383 - display_width;
    dev->g.cpc[2] = -16383 + display_height;
    dev->g.cpc[3] =  16383 - display_height;
    dev->g.cpc[4] = -16383;
    dev->g.cpc[5] =  16383;
 
/* get the window shape in NDC */
    min_dim = min(display_width, display_height);

    win_x = 0;
    win_y = 0;

    win_width  = dxf*min_dim;
    win_height = dyf*min_dim;

/* window manager hints */
    icon_width  = 16;
    icon_height = 16;

/* size hints */
    size_hints.flags      = USPosition | USSize | PMinSize;
    size_hints.x          = win_x;
    size_hints.y          = win_y;
    size_hints.width      = win_width;
    size_hints.height     = win_height;
    size_hints.min_width  = 0.25*win_width;
    size_hints.min_height = 0.25*win_height;
 
    window_name = dev->title;

    Xargc = 0;
    Xargv = NULL;
 
/* decide on the overall color layout and choose WHITE or BLACK background */
    dev->absolute_n_color = n_dev_colors;
    intensity = dev->max_intensity*MAXPIX;

    if (n_dev_colors == 2)
       {Color_Map(dev, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
	dev->ncolor = 2;}
    else
       {Color_Map(dev, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9,
		  10, 11, 12, 13, 14, 15);
	dev->ncolor = N_COLORS;};
    Lightest   = 0;
    Light      = intensity;
    Light_Gray = 0.8*intensity;
    Dark_Gray  = 0.5*intensity;
    Dark       = 0;
    Darkest    = intensity;
    bck_color  = WhitePixel(dev->display, screen);
    for_color  = BlackPixel(dev->display, screen);

/* create the window and map it to the screen */
    dev->window = XCreateSimpleWindow(dev->display,
                                      RootWindow(dev->display, screen), 
                                      win_x, win_y,
                                      win_width, win_height,
                                      dev->border_width, 
                                      for_color,
                                      bck_color);

/* turn on backing store */
    setwindattr.backing_store = WhenMapped;
    XChangeWindowAttributes(dev->display, dev->window,
                            CWBackingStore, &setwindattr);

    XStringListToTextProperty(&window_name, 1, &wintp);
    XSetWMProperties(dev->display, dev->window, &wintp, &wintp,
		     Xargv, Xargc, &size_hints, NULL, NULL);

    XSelectInput(dev->display, dev->window,
                 StructureNotifyMask |
                 ExposureMask |
                 PointerMotionMask |
                 KeyPressMask |
                 KeyReleaseMask |
                 ButtonPressMask |
                 ButtonReleaseMask);

    dev->pixmap = XCreatePixmap(dev->display, dev->window,
				win_width, win_height, depth);

/* compute the view port */
    _PG_default_viewspace(dev, TRUE);

    PG_init_viewspace(dev, TRUE);
 
/* create graphics context */
    xgc = XCreateGC(dev->display, dev->window, valuemask, &values);

    XSetForeground(dev->display, xgc, for_color);
    XSetBackground(dev->display, xgc, bck_color);

    dev->gc = xgc;

/* get window width */
    XGetWindowAttributes(dev->display, dev->window, &windowattr);
 
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

    PG_set_font(dev, "helvetica", "medium", 12);
    for (i = 0; dev->font_info == NULL; i++)
        {if (i > 3)
	    return(NULL);
	 _PG_X_txt_setup_font(dev, backup_font[i]);};

    npc = (n_dev_colors == 256) ? 64 : _PG_gattrs.max_color_factor;

/* put in the default palettes */
    PG_setup_standard_palettes(dev, npc,
			       Light, Dark,
			       Light_Gray, Dark_Gray,
			       Lightest, Darkest);

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

static int _PG_X_txt_set_font(PG_device *dev, char *face, char *style, int size)
   {int i, nf, ret, nfont, nstyle;
    int fsz, lsz;
    char *pm, *font_name, **names;
    char bf[MAXLINE], p[MAXLINE];
    XFontStruct *xf;
    Display *disp;

    if (!PG_setup_font(dev, face, style, size, &font_name, &nfont, &nstyle))
       return(FALSE);

/* allow the oddball fonts */
    if (size < 4)
      sprintf(bf, "*%s*", font_name);
    else
      sprintf(bf, "*%s*-*-*", font_name);

/* setup the font */
    ret  = FALSE;
    disp = dev->display;
    if (disp != NULL)
       {names = XListFonts(disp, bf, 1024, &nf);
	lsz   = -100;
	for (i = 0; i < nf; i++)
	    {pm = SC_strstr(names[i], "--");
	     if (pm != NULL)
	        {strcpy(p, pm);
		 fsz = SC_stoi(strtok(p, "- \t\n"));
		 if (fsz < size)
		    lsz = fsz;
		 else
		    {if (size-lsz < fsz-size)
		        i--;
		     break;};};};
	if (i >= nf)
	   i = nf-1;
         
	xf = XLoadQueryFont(disp, names[i]);
	if (xf != NULL)
	   {XSetFont(disp, dev->gc, xf->fid);

	    if (dev->font_info != NULL)
	       XFreeFont(disp, dev->font_info);

	    dev->font_info = xf;
	
	    ret = TRUE;};

	XFreeFontNames(names);};

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
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_TXT_PLACE_IMAGE - place the image from the drawable into IMG */

int _PG_X_txt_place_image(PG_device *src, int *dx,
			  PG_device *dst, int *xo, int bc, int pad)
   {int ix, iy, ok;
    int ri, gi, bi;
    int w, h, wow, bob;
    int pxss, bprs, ord;
    int ir[PG_SPACEDM], ip[PG_SPACEDM];
    unsigned long msk;
    unsigned char *sr, *dr, *dg, *db;
    double ca, sa;
    XImage *xi;
    frame *img;
    PG_RAST_device *mdv;

    ok = FALSE;
    if ((src->display != NULL) && (dst->raster != NULL))
       {GET_RAST_DEVICE(dst, mdv);

	img = mdv->raster;

	w  = img->width;
	h  = img->height;

	dx[0] += 2*pad;

	msk = AllPlanes;
	xi  = XGetImage(src->display, src->pixmap,
			0, 0, dx[0], dx[1], msk, ZPixmap);

	ord  = xi->byte_order;
	pxss = xi->bits_per_pixel >> 3;
	bprs = xi->bytes_per_line;

	if (dst->quadrant == QUAD_FOUR)
	   {xo[1] += (3*dx[1]/4);
	    PG_QUAD_FOUR_POINT(dst, xo);}
	else
	   xo[1] -= (3*dx[1]/4);

	PG_get_char_path(dst, &ca, &sa);

	for (iy = 0; iy < dx[1]; iy++)
	    {sr = (unsigned char *) (xi->data + bprs*iy);
	     if (ord)
	        {for (ix = 0; ix < dx[0]; ix++)
		     {ri  = sr[0];
		      gi  = sr[1];
		      bi  = sr[2];
		      sr += pxss;

		      ir[0] =  ix*ca + iy*sa;
		      ir[1] = -ix*sa + iy*ca;

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

		      ir[0] =  ix*ca + iy*sa;
		      ir[1] = -ix*sa + iy*ca;

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
    double ca, sa;
    XCharStruct overall;
    XFontStruct *fnt;
    XImage *si;

    ok = FALSE;
    if ((src->display != NULL) && (dst->display != NULL))
       {fnt = dst->font_info;
	XTextExtents(fnt, "g", 1, &dir, &asc, &dsc, &overall);

	xo[1] -= asc;
	dx[0] += 2*pad;

	PG_get_char_path(dst, &ca, &sa);
	msk = AllPlanes;
	si  = XGetImage(src->display, src->pixmap,
			0, 0, dx[0], dx[1], msk, ZPixmap);

	ok = _PG_X_put_ximage(dst, si, bc, src->BLACK, src->WHITE,
			      xo, dx, ca, sa, FALSE);

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
    double ca, sa;
    char *face, *sty;

    nd = 2;

    for (id = 0; id < nd; id++)
        ir[id] = x[id];

    PG_get_char_path(dev, &ca, &sa);

    PG_QUAD_FOUR_POINT(dev, ir);

/* if the text path is horizontal left to right do the quick thing */
    if ((ca == 1.0) && (sa == 0.0))
       ok = XDrawString(dev->display, PG_X11_DRAWABLE(dev), dev->gc,
			ir[0], ir[1], s, strlen(s));

/* otherwise you asked for it */
    else
       {PG_get_font(dev, &face, &sty, &sz);

	if (_PG.txtd == NULL)
	   {_PG.txtd = PG_make_device("SCREEN", "COLOR", "TextBuffer");
	    _PG_X_txt_open_screen(_PG.txtd, 0.3, 0.1);};

	pad = 2;

	bc = dev->BLACK;
	PG_get_text_color(dev, &fc);

	_PG_X_txt_clear_window(_PG.txtd, bc);
	_PG_X_txt_set_font(_PG.txtd, face, sty, sz);
	_PG_X_txt_write_text(_PG.txtd, s, fc, pad, dx);
	ok = _PG_X_txt_place_text(_PG.txtd, dx, dev, ir, bc, pad);

	SFREE(face);
	SFREE(sty);};

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

    PG_get_font(dev, &face, &sty, &sz);

    bc = dev->BLACK;
    PG_get_text_color(dev, &fc);

    for (id = 0; id < nd; id++)
        o[id] = dev->tcur[id];

    PG_trans_point(dev, 2, WORLDC, o, PIXELC, o);

    for (id = 0; id < nd; id++)
        io[id] = o[id];

    if (_PG.txtd == NULL)
       {_PG.txtd = PG_make_device("SCREEN", "COLOR", "TextBuffer");
	_PG_X_txt_open_screen(_PG.txtd, 0.3, 0.1);};

    pad = 2;

    _PG_X_txt_clear_window(_PG.txtd, bc);
    _PG_X_txt_set_font(_PG.txtd, "helvetica", sty, sz);
    _PG_X_txt_write_text(_PG.txtd, s, fc, pad, dx);
    ok = _PG_X_txt_place_image(_PG.txtd, dx, dev, io, bc, pad);

    SFREE(face);
    SFREE(sty);

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

