/*
 * GSDV_GL.C - PGS X routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
  
#include "pgs_int.h"

static Display
 *_PG_X_display = NULL;

static int
 attributeList[] = { None };

#ifndef HAVE_X11

char *X_event_name[] = 
   {"Event0",
    "Event1",
    "KeyPress",
    "KeyRelease",
    "ButtonPress",
    "ButtonRelease",
    "MotionNotify",
    "EnterNotify",
    "LeaveNotify",
    "FocusIn",
    "FocusOut",
    "KeymapNotify",
    "Expose",
    "GraphicsExpose",
    "NoExpose",
    "VisibilityNotify",
    "CreateNotify",
    "DestroyNotify",
    "UnmapNotify",
    "MapNotify",
    "MapRequest",
    "ReparentNotify",
    "ConfigureNotify",
    "ConfigureRequest",
    "GravityNotify",
    "ResizeRequest",
    "CirculateNotify",
    "CirculateRequest",
    "PropertyNotify",
    "SelectionClear",
    "SelectionRequest",
    "SelectionNotify",
    "ColormapNotify",
    "ClientMessage",
    "MappingNotify"};

#endif

static int
 _PG_GL_open_console(const char *title, const char *type, int bckgr,
		     double xf, double yf, double dxf, double dyf);

static PG_device
 *_PG_GL_get_event_device(PG_event *ev);

static void
 _PG_get_event(int fd, int mask, void *a);


/* from gsdv_X.c */

extern int
 _PG_X_init_palette(PG_device *dev, int ndvc, unsigned long *fbc),
 _PG_X_init_window(PG_device *dev, double *xf, int *dx, int *wdx,
		   int min_dim),
 _PG_X_setup_geom(PG_device *dev, double *xf, double *dxf,
		  int *dx, int *wx, int *wdx);

extern void
 _PG_X_select_events(PG_device *dev);

extern GC
 _PG_X_open_window(PG_device *dev, int *wx, int *wdx,
		   unsigned long *fbc, int map, int glx);


/* from the corresponding PR file */

extern void
 _PG_GL_draw_disjoint_polyline_2(PG_device *dev, double **r,
				 long n, int clip, int coord),
 _PG_GL_draw_curve(PG_device *dev, PG_curve *crv, int clip),
 _PG_GL_draw_to_abs(PG_device *dev, double x, double y),
 _PG_GL_draw_to_rel(PG_device *dev, double x, double y),
 _PG_GL_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs,
		     const char *s, double *p),
 _PG_GL_set_clipping(PG_device *dev, bool flag),
 _PG_GL_set_char_line(PG_device *dev, int n),
 _PG_GL_set_char_path(PG_device *dev, double x, double y),
 _PG_GL_set_char_precision(PG_device *dev, int p),
 _PG_GL_set_char_space(PG_device *dev, double s),
 _PG_GL_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x),
 _PG_GL_set_char_up(PG_device *dev, double x, double y),
 _PG_GL_set_fill_color(PG_device *dev, int color, int mapped),
 _PG_GL_set_line_color(PG_device *dev, int color, int mapped),
 _PG_GL_set_line_style(PG_device *dev, int style),
 _PG_GL_set_line_width(PG_device *dev, double width),
 _PG_GL_set_logical_op(PG_device *dev, PG_logical_operation log),
 _PG_GL_set_text_color(PG_device *dev, int color, int mapped),
 _PG_GL_shade_poly(PG_device *dev, int nd, int n, double **r),
 _PG_GL_fill_curve(PG_device *dev, PG_curve *crv),
 _PG_GL_move_gr_abs(PG_device *dev, double x, double y),
 _PG_GL_move_tx_abs(PG_device *dev, double x, double y),
 _PG_GL_move_tx_rel(PG_device *dev, double x, double y),
 _PG_GL_get_image(const PG_device *dev, unsigned char *bf, int ix, int iy,
		  int nx, int ny),
 _PG_GL_put_image(PG_device *dev, unsigned char *bf, int ix, int iy,
		  int nx, int ny);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_SETUP_FONT - load the named font for X if it exists */

static int _PG_GL_setup_font(PG_device *dev, char *bf)
   {char **names;
    int i, nf, ret;
    XFontStruct *xf;
    Display *disp;

    ret = FALSE;
    if (dev != NULL)
       {disp = dev->display;
	if (disp == NULL)
	   return(ret);

	names = XListFonts(disp, bf, 1024, &nf);
	for (i = 0; i < nf; i++)
	    {xf = XLoadQueryFont(disp, names[i]);
	     if (xf != NULL)
	        {XSetFont(disp, dev->gc, xf->fid);
		 dev->font_info = xf;

		 ret = TRUE;
		 break;};};

	XFreeFontNames(names);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_INIT_WINDOW - perform GL window initializations */

static PG_device *_PG_GL_init_window(PG_device *dev)
   {int screen;
    XVisualInfo *vi;

/* create OpenGL graphics context */
    if (!glXQueryExtension(dev->display, NULL, NULL))
       return(NULL);

    screen = DefaultScreen(_PG_X_display);
    vi     = glXChooseVisual(dev->display, screen, attributeList);

    dev->glxgc = glXCreateContext(dev->display, vi, 0, TRUE);

    glXMakeCurrent(dev->display, dev->window, dev->glxgc);
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

/* GOTCHA: it may be more efficient to leave pixel row alignment set to
 * its default value of 4 and pack image data accordingly
 */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (glGetError() != GL_NO_ERROR)
       dev = NULL;

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_OPEN_IMBEDDED_SCREEN - initialize a "window" on a display screen */
 
PG_device *_PG_GL_open_imbedded_screen(PG_device *dev, Display *display,
                                       Window window, GC gc,
				       double xfo, double yfo,
                                       double dxfo, double dyfo)
   {int min_dim, ok, ndvc;
    int dx[PG_SPACEDM], wdx[PG_SPACEDM];
    double xf[PG_SPACEDM], dxf[PG_SPACEDM];

    if ((dev == NULL) || (display == NULL))
       return(NULL);

/* connect to X server once only */
    dev->display = _PG_X_display = display;

    _PG.dsp_intr = SC_io_callback_pid(ConnectionNumber(_PG_X_display),
				      _PG_get_event, NULL, -2);

/* don't let interrupts in while we're setting up the window */
    PG_catch_interrupts(FALSE);

    dev->type_index = GRAPHIC_WINDOW_DEVICE;
    dev->quadrant   = QUAD_FOUR;

    PG_query_screen_n(dev, dx, &ndvc);
    if ((dx[0] == 0) && (dx[1] == 0) && (ndvc == 0))
       return(NULL);

    xf[0] = xfo;
    xf[1] = yfo;

    dxf[0] = dxfo;
    dxf[1] = dyfo;

    min_dim = _PG_X_setup_geom(dev, xf, dxf, dx, NULL, wdx);

    _PG_X_init_palette(dev, ndvc, NULL);

    dev = _PG_GL_init_window(dev);
    if (dev != NULL)

/* attach the window */
       {dev->window = window;

/* attach X graphics context */
	dev->gc = gc;

	_PG_X_select_events(dev);

	ok = _PG_X_init_window(dev, xf, dx, wdx, min_dim);
	if (ok != 0)
	   dev = NULL;};

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_OPEN_SCREEN - initialize a "window" on a display screen */
 
static PG_device *_PG_GL_open_screen(PG_device *dev, double xfo, double yfo,
				     double dxfo, double dyfo)
   {int ndvc, min_dim, ok;
    int dx[PG_SPACEDM], wx[PG_SPACEDM], wdx[PG_SPACEDM];
    unsigned long fbc[2];
    double xf[PG_SPACEDM], dxf[PG_SPACEDM];

    if (dev == NULL)
       return(NULL);

/* don't let interrupts in while we're setting up the window */
    PG_catch_interrupts(FALSE);

    dev->type_index = GRAPHIC_WINDOW_DEVICE;
    dev->quadrant   = QUAD_FOUR;

    PG_query_screen_n(dev, dx, &ndvc);
    if ((dx[0] == 0) && (dx[1] == 0) && (ndvc == 0))
       return(NULL);

    if (dev->display == NULL)
       return(NULL);

    xf[0] = xfo;
    xf[1] = yfo;

    dxf[0] = dxfo;
    dxf[1] = dyfo;

    min_dim = _PG_X_setup_geom(dev, xf, dxf, dx, NULL, wdx);

    _PG_X_init_palette(dev, ndvc, fbc);

    dev->gc = _PG_X_open_window(dev, wx, wdx, fbc, TRUE, TRUE);

    dev = _PG_GL_init_window(dev);
    if (dev != NULL)
       {ok = _PG_X_init_window(dev, xf, dx, wdx, min_dim);
	if (ok != 0)
	   dev = NULL;};

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_CONNECT_SERVER - connect to the X server
 *                       - and register call backs for interrupt driven I/O
 *                       - on stdin and the connection to the server
 *                       - do this at most one time in a program
 */

static void _PG_GL_connect_server(PG_device *dev)
   {char *window_name;

/* ignore interrupts while opening the console */
    PG_catch_interrupts(FALSE);

    _PG.tty_intr = PG_init_event_handling();

    if (_PG_X_display == NULL)

/* try to open the connection from the PGS device */
       {if (strcmp(dev->name, "WINDOW") != 0)
           _PG_X_display = XOpenDisplay(dev->name);

/* try to open the connection from the DISPLAY variable */
        if (_PG_X_display == NULL)
           {window_name   = getenv("DISPLAY");
            _PG_X_display = XOpenDisplay(window_name);};

/* if successful register the call back on the connection */
        if (_PG_X_display != NULL)
	   {int fid;

	    fid          = ConnectionNumber(_PG_X_display);
	    _PG.dsp_intr = PG_loop_callback(0, &fid,
					    _PG_get_event, NULL, -2);}

/* otherwise we faile to make a connection to the server
 * we ought to be able to continue without having to can the interrupts
 * but debugging gets to be horrendous, so ...
 */
	else
	   {SC_set_io_interrupts(FALSE);
	    _PG.tty_intr = FALSE;};};

/* if interrupts were on before, turn them back on again */
    PG_catch_interrupts(-1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_QUERY_SCREEN - query some physical device characteristics */

static void _PG_GL_query_screen(PG_device *dev, int *pdx, int *pnc)
   {int id, screen, n_planes;

    if (dev->g.phys_dx[0] == -1)

/* connect to server once only */
       {_PG_GL_connect_server(dev);

	if (_PG_X_display != NULL)
	   {dev->display = _PG_X_display;
 
	    screen   = DefaultScreen(_PG_X_display);
	    n_planes = DisplayPlanes(_PG_X_display, screen);

	    dev->g.phys_dx[0]  = DisplayWidth(_PG_X_display, screen);
	    dev->g.phys_dx[1]  = DisplayHeight(_PG_X_display, screen);
	    dev->phys_n_colors = 1 << n_planes;}

	else
	   {for (id = 0; id < 2; id++)
	        dev->g.phys_dx[id] = 0;

	    dev->phys_n_colors = 0;};};

    for (id = 0; id < 2; id++)
        pdx[id] = dev->g.phys_dx[id];

    *pnc = dev->phys_n_colors;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_MAP_TO_COLOR_TABLE - map the PGS palette to host
 *                           - color table values
 */

static void _PG_GL_map_to_color_table(PG_device *dev, PG_palette *pal)
   {XColor pixcolor;
    Display *disp;
    int i, status, screen, n_pal_colors;
    unsigned long *pi, last_color;
    Colormap clrmp;
    RGB_color_map *true_cm;

    n_pal_colors = pal->n_pal_colors;
    true_cm      = pal->true_colormap;

/* add two for the black and white colors at the top */
    n_pal_colors += 2;

    disp = dev->display;
    if (disp == NULL)
       return;

    pi     = CMAKE_N(unsigned long, n_pal_colors);
    screen = DefaultScreen(disp);
    clrmp  = DefaultColormap(disp, screen);

    pixcolor.flags = DoRed | DoGreen | DoBlue;
    pixcolor.pixel = 0;
    last_color     = 0;
    for (i = 0; i < n_pal_colors; i++)
        {pixcolor.red   = true_cm[i].red;
         pixcolor.green = true_cm[i].green;
         pixcolor.blue  = true_cm[i].blue;
   
         status = XAllocColor(disp, clrmp, &pixcolor);
         if (status != 0)
            last_color = pixcolor.pixel;

         pi[i] = last_color;};

/* this is the right thing to do when you want more colors than
 * the device supports and have to use the pseudo_cm
 *
    for (j = 0, i = 0; i < ndvc; i++)
        {pixcolor.red   = true_cm[i].red;
         pixcolor.green = true_cm[i].green;
         pixcolor.blue  = true_cm[i].blue;
   
         status = XAllocColor(disp,
                              DefaultColormap(disp, screen),
                              &pixcolor);
         if (status != 0)
            last_color = pixcolor.pixel;

         pi[j++] = last_color;
         if (j >= n_pal_colors)
            break;};
*/
    pal->pixel_value = pi;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_MATCH_RGB_COLORS - map the RGB references in the palette to
 *                         - valid colors from the root color table
 */

static void _PG_GL_match_rgb_colors(PG_device *dev, PG_palette *pal)
   {int i, j, npc;
    unsigned long *pi, *pir;
    RGB_color_map *true_cm;

/* add two for the black and white colors at the top */
    npc     = pal->n_pal_colors + 2;
    true_cm = pal->true_colormap;

    pi  = CMAKE_N(unsigned long, npc);
    pir = dev->color_table->pixel_value;

    for (i = 0; i < npc; i++)
        {j = PG_rgb_index(dev, true_cm++);
         pi[i] = pir[j];}

    pal->pixel_value = pi;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_SET_FONT - set the character font */

static int _PG_GL_set_font(PG_device *dev, const char *face,
			   const char *style, int size)
   {int nfont, nstyle;
    char *font_name;
    char bf[MAXLINE];

    if (!PG_setup_font(dev, face, style, size, &font_name, &nfont, &nstyle))
       return(FALSE);

/* allow the oddball fonts */
    if (size < 4)
      snprintf(bf, MAXLINE, "*%s*", font_name);
    else
      snprintf(bf, MAXLINE, "*%s*-%d-*", font_name, 10 * size);

    return(_PG_GL_setup_font(dev, bf));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_OPEN_CONSOLE - open up a special window to be used as a console or
 *                     - terminal window for systems which can't do this for
 *                     - themselves (i.e. MacIntosh) or the SINGLE SCREEN
 *                     - devices
 */
 
static int _PG_GL_open_console(const char *title, const char *type,
			       int bckgr,
			       double xf, double yf, double dxf, double dyf)
   {int nc;
    int dx[PG_SPACEDM];
    double tx[PG_SPACEDM];

    PG_setup_markers();

    PG_gs.console = PG_make_device("TEXT", type, title);
    if (PG_gs.console == NULL)
       return(FALSE);

    PG_query_screen_n(PG_gs.console, dx, &nc);
    if ((dx[0] == 0) && (dx[1] == 0) && (nc == 0))
       return(FALSE);

    PG_gs.console->background_color_white = bckgr;

/* initialize text size info */
    tx[0] = 1.0/TXSPAN;
    tx[1] = PG_gs.console->txt_ratio*tx[0];
    PG_fset_char_size_n(PG_gs.console, 2, NORMC, tx);

    SC_setbuf(stdout, NULL);

    SC_set_get_line(PG_wind_fgets);
    SC_set_put_line(PG_fprintf);
    SC_set_put_string(PG_fputs);

    _PG_push_device(PG_gs.console);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_FLUSH_EVENTS - flush all pending events on the device */
 
static void _PG_GL_flush_events(PG_device *dev)
   {Display *disp;

    disp = dev->display;
    if (disp != NULL)
       XSync(disp, True);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_CLOSE_DEVICE - close a device */
 
static void _PG_GL_close_device(PG_device *dev)
   {Display *disp;

    disp = dev->display;
    if (disp != NULL)
       {glXDestroyContext(disp, dev->glxgc);

	if (dev->window)
	   {XDestroyWindow(disp, dev->window);
	    XFlush(disp);};

	_PG_remove_device(dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_CLOSE_CONSOLE - close the console device */
 
static void _PG_GL_close_console(void)
   {

    _PG_remove_device(PG_gs.console);

    PG_gs.console = NULL;

/* connect I/O to standard functions */
    SC_set_put_line(io_printf);
    SC_set_put_string(io_puts);
    SC_set_get_line(io_gets);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_GET_NEXT_EVENT - get the next event
 *                       - return TRUE iff successful
 */

static int _PG_GL_get_next_event(PG_event *ev)
   {

    XNextEvent(_PG_X_display, ev);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_GET_EVENT_DEVICE - return the device in which the given event
 *                         - occurred
 */

static PG_device *_PG_GL_get_event_device(PG_event *ev)
   {Window window;
    PG_device *dev;
    extern PG_device *_PG_X_find_device(Window window);

    window = ev->xany.window;
    dev    = _PG_X_find_device(window);

    return(dev);}

/*--------------------------------------------------------------------------*/

/*                           SCREEN STATE ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _PG_GL_MAKE_DEVICE_CURRENT - make this device the current one for drawing
 *                            - purposes
 */
 
static void _PG_GL_make_device_current(PG_device *dev)
   {Window window, root, parent, child, *children;
    Display *display;
    int x, y, root_x, root_y;
    unsigned int n;
    unsigned int width, height, border_width, depth;
    double wc[PG_BOXSZ];
    PG_dev_geometry *g;

    if (dev == NULL)
       return;

    window  = dev->window;
    display = dev->display;

    if (window)
       {g = &dev->g;

	PG_get_viewspace(dev, WORLDC, wc);

        XQueryTree(display, window, &root, &parent, &children, &n);
	if (children)
	   XFree(children);

        XGetGeometry(display, window, &root,
                     &x, &y, &width, &height, &border_width, &depth);

        g->hwin[3] = g->hwin[2] + height;
        g->hwin[1] = g->hwin[0] + width;
/*	dev->border_width  = border_width; */

	XTranslateCoordinates(display, window, root,
			      0, 0, &root_x, &root_y, &child);

        g->hwin[0] = root_x;
        g->hwin[2] = root_y;

	SET_PC_FROM_HWIN(g);

	PG_set_viewspace(dev, 3, WORLDC, wc);

	glXMakeCurrent(dev->display, dev->window, dev->glxgc);
	glViewport(x, y, width, height);};

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_RELEASE_CURRENT_DEVICE - make no device current */
 
static void _PG_GL_release_current_device(PG_device *dev)
   {

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_CLEAR_WINDOW - clear the screen */
 
static void _PG_GL_clear_window(PG_device *dev)
   {Display *disp;

    if (dev != NULL)
       {PG_make_device_current(dev);

	disp = dev->display;
	if (disp != NULL)
	   XClearWindow(disp, dev->window);

	PG_clear_raster_device(dev);

	PG_release_current_device(dev);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_CLEAR_VIEWPORT - clear the viewport */
 
static void _PG_GL_clear_viewport(PG_device *dev)
   {PG_dev_geometry *g;

    g = &dev->g;

    PG_clear_region(dev, 2, NORMC, g->ndc, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_CLEAR_REGION - clear the ND box BX specified in CS */
 
static void _PG_GL_clear_region(PG_device *dev, int nd, PG_coord_sys cs,
				double *bx, int pad)
   {int iw, ih;
    double pc[PG_BOXSZ];
    Display *disp;

    if (dev != NULL)
       {disp = dev->display;
	if (disp == NULL)
	   return;

	PG_make_device_current(dev);

	PG_trans_box(dev, 2, cs, bx, PIXELC, pc);

	iw = (pc[1] - pc[0] + 2*pad);
	ih = (pc[3] - pc[2] + 2*pad);
	pc[0] -= pad;
	pc[2] -= pad;
	XClearArea(disp, dev->window, pc[0], pc[2],
		   (unsigned int) iw, (unsigned int) ih, TRUE);
    
	PG_release_current_device(dev);

	_PG_clear_raster_region(dev, nd, cs, bx, pad);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_UPDATE_VS - update the view surface for the given device */
 
static void _PG_GL_update_vs(PG_device *dev)
   {Display *disp;

    if (dev != NULL)
       {disp = dev->display;
	if (disp == NULL)
	   return;

	XFlush(disp);
	glXWaitX();
	glFlush();
	glXWaitGL();

	PG_release_current_device(dev);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_FINISH_PLOT - do what's necessary to finish up a graphical image
 *                    - and get the device updated with the image
 *                    - after this function nothing more can be added to
 *                    - the image
 */
 
static void _PG_GL_finish_plot(PG_device *dev)
   {

    PG_make_device_current(dev);

    _PG_X_put_raster(dev, dev->WHITE, 0, 0, -1, -1, 1.0, 0.0);

    PG_update_vs(dev);

    PG_release_current_device(dev);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_CLEAR_PAGE - clear the current page
 *                   - and go to the line i on the screen
 */
 
static void _PG_GL_clear_page(PG_device *dev, int i)
   {return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_EXPOSE_DEVICE - make this device the topmost one */
 
static void _PG_GL_expose_device(PG_device *dev)
   {return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_WRITE_TEXT - write out text to the appropriate device */
 
static void _PG_GL_write_text(PG_device *dev, FILE *fp, const char *s)
   {double x[PG_SPACEDM];
    Display *disp;

    if (fp == PG_gs.stdscr)
       {disp = dev->display;
        if (disp == NULL)
           return;

	x[0] = dev->tcur[0];
	x[1] = dev->tcur[1];
	PG_trans_point(dev, 2, WORLDC, x, PIXELC, x);

        XDrawString(disp, PG_X11_DRAWABLE(dev), dev->gc,
                    x[0], x[1], s, strlen(s));}
    else
       io_printf(fp, "%s", s);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_GL_NEXT_LINE - do a controlled newline */
 
static void _PG_GL_next_line(PG_device *dev, FILE *fp)
   {

    putchar('\n');
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_EVENTS_PENDING - return number of pending events */

static int _PG_GL_events_pending(PG_device *dev)
   {int n;

    n = 0;

    if (_PG_X_display != NULL)
       n = XEventsQueued(_PG_X_display, QueuedAfterFlush);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GET_EVENT_CORE - respond to events
 *                    - if check is TRUE then loop while there are events
 *                    - if check is FALSE loop forever
 */

static char *_PG_get_event_core(int check)
   {int type;
    PG_text_box *b;
    PG_device *dev;
    PG_interface_object *iob;

    if (_PG_X_display == NULL)
       return(NULL);

    while (!check || (XEventsQueued(_PG_X_display, QueuedAfterFlush) > 0))
       {if (!PG_get_next_event(NULL)) 
           continue;

        dev = PG_get_event_device(NULL);
        if (dev == NULL)
           continue;

/* a switch is precluded here because you need to check both the
 * event and handler at the same time so that a default handler
 * can do them all if its set up that way
 */
        type = PG_EVENT_TYPE(_PG_gcont.current_event);
        if ((type == EXPOSE_EVENT) &&
            (dev->expose_event_handler.fnc != NULL))
           {PG_CALL_HANDLER(dev->expose_event_handler, dev, _PG_gcont.current_event);}

        else if ((type == UPDATE_EVENT) &&
                 (dev->update_event_handler.fnc != NULL))
           {PG_CALL_HANDLER(dev->update_event_handler, dev, _PG_gcont.current_event);}

        else if (type == MOUSE_DOWN_EVENT)
           {iob = PG_get_object_event(dev, &_PG_gcont.current_event);
            if ((iob != NULL) && (iob->action != NULL))
               (*iob->action)(iob, &_PG_gcont.current_event);
            else if (dev->mouse_down_event_handler.fnc != NULL)
               PG_CALL_HANDLER(dev->mouse_down_event_handler, dev, _PG_gcont.current_event);}

        else if ((type == MOUSE_UP_EVENT) &&
                 (dev->mouse_up_event_handler.fnc != NULL))
           {PG_CALL_HANDLER(dev->mouse_up_event_handler, dev, _PG_gcont.current_event);}

        else if (type == KEY_DOWN_EVENT)
           {iob = PG_get_object_event(dev, &_PG_gcont.current_event);
            if ((iob != NULL) && (iob->action != NULL))
	       (*iob->action)(iob, &_PG_gcont.current_event);
	    else if (dev->key_down_event_handler.fnc != NULL)
               PG_CALL_HANDLER(dev->key_down_event_handler, dev, _PG_gcont.current_event);}

        else if ((type == KEY_UP_EVENT) &&
                 (dev->key_up_event_handler.fnc != NULL))
           {PG_CALL_HANDLER(dev->key_up_event_handler, dev, _PG_gcont.current_event);}

        else if (type == MOTION_EVENT)
           {iob = PG_get_object_event(dev, &_PG_gcont.current_event);
            if ((iob != NULL) && (strcmp(iob->type, PG_TEXT_OBJECT_S) == 0))
               {b = (PG_text_box *) iob->obj;
                if (b != _PG.b0)
		   {if (_PG.b0 != NULL)
		       _PG_draw_cursor(_PG.b0, FALSE);
		    _PG_draw_cursor(b, TRUE);
		    _PG.b0 = b;};}

            else if (_PG.b0 != NULL)
	       {_PG_draw_cursor(_PG.b0, FALSE);
		_PG.b0 = NULL;}

            else if (dev->motion_event_handler.fnc != NULL)
               PG_CALL_HANDLER(dev->motion_event_handler, dev, _PG_gcont.current_event);}

        else if (dev->default_event_handler.fnc != NULL)
           PG_CALL_HANDLER(dev->default_event_handler, dev, _PG_gcont.current_event);};

    return(_PG_gcont.input_bf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GET_EVENT - respond to an event available interrupt */

static void _PG_get_event(int fd, int mask, void *a)
   {

    _PG_get_event_core(TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_WIND_FGETS - get and return a string from the window environment
 *                   - start with a clean event slate, and cycle the main
 *                   - event loop until some event sets
 *                   - PG_gs.console->DoneFlag
 *                   - NOTE: there is a logical problem here in that
 *                   -       sometimes you just want to use PG_wind_fgets to
 *                   -       process events
 *                   -       so if str is NULL don't hang around for
 *                   -       a string
 */
 
static char *_PG_GL_wind_fgets(char *str, int maxlen, FILE *fp)
   {char *p;

    p = NULL;
    if (fp != stdin)
       p = io_gets(str, maxlen, fp);

    else
       {PG_setup_input(str, maxlen, -1);

	if (SETJMP(_PG.io_avail) == ERR_FREE)

/* turn off interrupts until the next call to _PG_GL_wind_fgets */
	   {PG_catch_interrupts(FALSE);
	    p = _PG_gcont.input_bf;}

	else

/* don't catch interrupts until the SETJMP is successful */
	   {PG_catch_interrupts(TRUE);

/* if they are both on interrupts or both off interrupts
 * go let the OS block on input from either source
 */
	    if (_PG.dsp_intr == _PG.tty_intr)
	       {PG_catch_interrupts(FALSE);

		PG_poll((_PG_gcont.input_bf != NULL), -1);

		PG_catch_interrupts(TRUE);}

/* if only the display is on interrupts, do a blocking read on the tty */
	    else if (_PG.dsp_intr)
	       p = io_gets(str, maxlen, fp);

/* if only the tty is on interrupts, do a XNextEvent on the display */
	    else if (_PG.tty_intr)
	       p = _PG_get_event_core(FALSE);};};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_WIND_FGETC - get and return a character from the window environment
 *                   - start with a clean event slate, and cycle the main
 *                   - event loop until some event sets
 *                   - PG_gs.console->DoneFlag
 */
 
static int _PG_GL_wind_fgetc(FILE *fp)
   {int c;
    char bf[10];

    c = 0;
    if (fp != stdin)
       c = io_getc(fp);

    else
       {PG_setup_input(bf, 10, 1);

	if (SETJMP(_PG.io_avail) == ERR_FREE)

/* turn off interrupts until the next call to _PG_GL_wind_fgetc */
	   {PG_catch_interrupts(FALSE);
	    c = _PG_gcont.input_bf[0];}

	else

/* don't catch interrupts until the SETJMP is successful */
	   {PG_catch_interrupts(TRUE);
	    PG_poll(TRUE, -1);
	    PG_catch_interrupts(TRUE);};};

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GET_EVENT_CHAR - handle key down events */

static void _PG_get_event_char(PG_device *dev, PG_event *ev)
   {char bf[80];
    int iev[PG_SPACEDM], mod;

    PG_key_event_info(dev, ev, iev, bf, 80, &mod);
    _PG.next_char = bf[0];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GL_GET_CHAR - get one char */

static int _PG_GL_get_char(PG_device *dev)
   {PFEventHand old;

    old = dev->key_down_event_handler.fnc;
    dev->key_down_event_handler.fnc = _PG_get_event_char;
    _PG.next_char = 0;

    while (!_PG.next_char)
       SC_poll_descriptors(-1);

    dev->key_down_event_handler.fnc = old;

    return(_PG.next_char);}

/*--------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_GL_QUERY_POINTER - query the pointer for location and button status
 *                      - the button state is represented as follows
 *                      -   bit 0 - MOUSE_LEFT state
 *                      -   bit 1 - MOUSE_RIGHT state
 *                      -   bit 2 - MOUSE_MIDDLE state
 *                      -   bit 4 - KEY_SHIFT state
 *                      -   bit 5 - KEY_CNTL state
 *                      -   bit 6 - KEY_ALT state
 */

static void _PG_GL_query_pointer(PG_device *dev, int *ir, int *pb, int *pq)
   {Window window, root, child;
    Display *display;
    int rx, ry, btn, mod;
    unsigned int buttons;

    display = dev->display;
    window  = dev->window;

    mod = XQueryPointer(display, window, &root, &child,
                        &rx, &ry,
                        &ir[0], &ir[1], &buttons);

/* map X buttons into PGS buttons */
    btn = 0;
    btn |= (buttons & Button1Mask) ? MOUSE_LEFT   : 0;
    btn |= (buttons & Button2Mask) ? MOUSE_MIDDLE : 0;
    btn |= (buttons & Button3Mask) ? MOUSE_RIGHT  : 0;

    mod = 0;
    mod |= (buttons & ShiftMask)   ? KEY_SHIFT : 0;
    mod |= (buttons & ControlMask) ? KEY_CNTL  : 0;
    mod |= (buttons & Mod1Mask)    ? KEY_ALT   : 0;

    *pb = btn;
    *pq = mod;

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_GL_GRAB_POINTER - take over the pointer */

static int _PG_GL_grab_pointer(PG_device *dev)
   {int rv;
    Display *disp;

    if (dev == NULL)
       disp = _PG_X_display;
    else
       disp = dev->display;

    rv = XGrabPointer(disp, XDefaultRootWindow(disp),
		      False, ButtonPressMask, GrabModeAsync, 
		      GrabModeAsync, None, None, CurrentTime);

    rv = (rv == GrabSuccess);

    return(rv);}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_GL_RELEASE_POINTER - release the pointer */

static int _PG_GL_release_pointer(PG_device *dev)
   {int rv;
    Display *disp;

    if (dev == NULL)
       disp = _PG_X_display;
    else
       disp = dev->display;

    rv = XUngrabPointer(disp, CurrentTime);

    rv = (rv == GrabSuccess);

    return(rv);}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_GL_MOUSE_EVENT_INFO - return mouse event info for GL */

static void _PG_GL_mouse_event_info(PG_device *dev, PG_event *ev,
				    int *iev, PG_mouse_button *peb, int *peq)
   {int et, eq;
    PG_mouse_button eb;

/* location of mouse */
    iev[0] = ev->xbutton.x;
    iev[1] = ev->xbutton.y;

    PG_QUAD_FOUR_POINT(dev, iev);

/* which button */
    switch (ev->xbutton.button)
       {case 1 :
	     eb = MOUSE_LEFT;
	     break;
        case 2 :
	     eb = MOUSE_MIDDLE;
	     break;
	case 3 :
	     eb = MOUSE_RIGHT;
	     break;
	default :
	     eb = MOUSE_NONE;
	     break;};
    *peb = eb;

/* button modifers */
    et  = ev->xbutton.state;
    eq  = (et & 1) ? KEY_SHIFT : 0;
    eq |= (et & 2) ? KEY_LOCK  : 0;
    eq |= (et & 4) ? KEY_CNTL  : 0;
    eq |= (et & 8) ? KEY_ALT   : 0;

    *peq = eq;

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_GL_KEY_EVENT_INFO - return keyboard event info for GL */

static void _PG_GL_key_event_info(PG_device *dev, PG_event *ev,
				 int *iev, char *bf, int n, int *peq)
   {int et, eq;
    KeySym key;
    XComposeStatus st;

    XLookupString(&ev->xkey, bf, n, &key, &st);

/* location */
    iev[0] = ev->xkey.x;
    iev[1] = ev->xkey.y;

/* key modifers */
    et = ev->xkey.state;
    eq  = (et & 1) ? KEY_SHIFT : 0;
    eq |= (et & 2) ? KEY_LOCK  : 0;
    eq |= (et & 4) ? KEY_CNTL  : 0;
    eq |= (et & 8) ? KEY_ALT   : 0;

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_GL_PUTS - put a string to the console window */

static void _PG_GL_puts(const char *bf)
   {

    puts(bf);
    FLUSH_ON(stdout, bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_GL_DEVICE - do the device dependent device initialization
 *                    - for PG_make_device
 */

int PG_setup_gl_device(PG_device *d)
   {

    d->X_cap_style  = CapRound;
    d->X_line_style = LineSolid;
    d->X_join_style = JoinRound;
    d->X_line_width = 0;
    d->display      = NULL;
    d->font_info    = NULL;

    d->type_index   = GRAPHIC_WINDOW_DEVICE;
    d->is_visible   = TRUE;

    d->events_pending         = _PG_GL_events_pending;
    d->query_pointer          = _PG_GL_query_pointer;
    d->grab_pointer           = _PG_GL_grab_pointer;
    d->release_pointer        = _PG_GL_release_pointer;
    d->query_pointer          = _PG_GL_query_pointer;
    d->mouse_event_info       = _PG_GL_mouse_event_info;
    d->key_event_info         = _PG_GL_key_event_info;
    d->clear_page             = _PG_GL_clear_page;
    d->clear_window           = _PG_GL_clear_window;
    d->clear_viewport         = _PG_GL_clear_viewport;
    d->clear_region           = _PG_GL_clear_region;
    d->close_console          = _PG_GL_close_console;
    d->close_device           = _PG_GL_close_device;
    d->flush_events           = _PG_GL_flush_events;
    d->draw_dj_polyln_2       = _PG_GL_draw_disjoint_polyline_2;
    d->draw_curve             = _PG_GL_draw_curve;
    d->draw_to_abs            = _PG_GL_draw_to_abs;
    d->draw_to_rel            = _PG_GL_draw_to_rel;
    d->expose_device          = _PG_GL_expose_device;
    d->finish_plot            = _PG_GL_finish_plot;
    d->get_char               = _PG_GL_get_char;
    d->get_image              = _PG_GL_get_image;
    d->get_text_ext           = _PG_GL_get_text_ext;
    d->ggetc                  = (PFfgetc) _PG_GL_wind_fgetc;
    d->ggets                  = (PFfgets) _PG_GL_wind_fgets;
    d->gputs                  = _PG_GL_puts;
    d->make_device_current    = _PG_GL_make_device_current;
    d->map_to_color_table     = _PG_GL_map_to_color_table;
    d->match_rgb_colors       = _PG_GL_match_rgb_colors;
    d->move_gr_abs            = _PG_GL_move_gr_abs;
    d->move_tx_abs            = _PG_GL_move_tx_abs;
    d->move_tx_rel            = _PG_GL_move_tx_rel;
    d->next_line              = _PG_GL_next_line;
    d->open_screen            = _PG_GL_open_screen;
    d->put_image              = _PG_GL_put_image;
    d->query_screen           = _PG_GL_query_screen;
    d->release_current_device = _PG_GL_release_current_device;
    d->set_clipping           = _PG_GL_set_clipping;
    d->set_char_line          = _PG_GL_set_char_line;
    d->set_char_path          = _PG_GL_set_char_path;
    d->set_char_precision     = _PG_GL_set_char_precision;
    d->set_char_size          = _PG_GL_set_char_size;
    d->set_char_space         = _PG_GL_set_char_space;
    d->set_char_up            = _PG_GL_set_char_up;
    d->set_fill_color         = _PG_GL_set_fill_color;
    d->set_font               = _PG_GL_set_font;
    d->set_line_color         = _PG_GL_set_line_color;
    d->set_line_style         = _PG_GL_set_line_style;
    d->set_line_width         = _PG_GL_set_line_width;
    d->set_logical_op         = _PG_GL_set_logical_op;
    d->set_text_color         = _PG_GL_set_text_color;
    d->shade_poly             = _PG_GL_shade_poly;
    d->fill_curve             = _PG_GL_fill_curve;
    d->update_vs              = _PG_GL_update_vs;
    d->write_text             = _PG_GL_write_text;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GL_SETUP_CTRLS_GLB - setup global control structures for GL */

void PG_GL_setup_ctrls_glb(void)
   {

    _PG_gcont.get_event    = _PG_GL_get_next_event;
    _PG_gcont.event_device = _PG_GL_get_event_device;
    _PG_gcont.open_console = _PG_GL_open_console;
    _PG_gcont.setup_window = PG_setup_gl_device;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
