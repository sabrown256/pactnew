/*
 * GSDV_X.C - PGS X routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
  
#include "pgs_int.h"

static Display
 *_PG_X_display = NULL;

static char
 _PG_X_display_name[MAXLINE] = "";

static char
 *backup_font[] = {"*-medium-r-*", "*-r-*", "*-r-*", "*-r-*"};

static int
 backup_size[] = { 12, 12, 13, 14 };

SC_array
 *_PG_X_point_list;

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


/* from the corresponding TX file */

extern int
 _PG_X_draw_text(PG_device *dev, char *s, double *x);


/* from the corresponding PR file */

extern void
 _PG_X_draw_disjoint_polyline_2(PG_device *dev, double **r,
				long n, int clip, int coord),
 _PG_X_draw_curve(PG_device *dev, PG_curve *crv, int clip),
 _PG_X_draw_to_abs(PG_device *dev, double x, double y),
 _PG_X_draw_to_rel(PG_device *dev, double x, double y),
 _PG_X_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p),
 _PG_X_set_clipping(PG_device *dev, int flag),
 _PG_X_set_char_line(PG_device *dev, int n),
 _PG_X_set_char_path(PG_device *dev, double x, double y),
 _PG_X_set_char_precision(PG_device *dev, int p),
 _PG_X_set_char_space(PG_device *dev, double s),
 _PG_X_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x),
 _PG_X_set_char_up(PG_device *dev, double x, double y),
 _PG_X_set_fill_color(PG_device *dev, int color, int mapped),
 _PG_X_set_line_color(PG_device *dev, int color, int mapped),
 _PG_X_set_line_style(PG_device *dev, int style),
 _PG_X_set_line_width(PG_device *dev, double width),
 _PG_X_set_logical_op(PG_device *dev, PG_logical_operation lop),
 _PG_X_set_text_color(PG_device *dev, int color, int mapped),
 _PG_X_shade_poly(PG_device *dev, int nd, int n, double **r),
 _PG_X_fill_curve(PG_device *dev, PG_curve *crv),
 _PG_X_move_gr_abs(PG_device *dev, double x, double y),
 _PG_X_move_tx_abs(PG_device *dev, double x, double y),
 _PG_X_move_tx_rel(PG_device *dev, double x, double y),
 _PG_X_get_image(PG_device *dev, unsigned char *bf, int ix, int iy,
		 int nx, int ny),
 _PG_X_put_image(PG_device *dev, unsigned char *bf, int ix, int iy,
		 int nx, int ny);


static int
 _PG_X_open_console(char *title, char *type, int bckgr,
		    double xf, double yf, double dxf, double dyf);

static PG_device
 *_PG_X_get_event_device(PG_event *ev);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_EVENTS_PENDING - return number of pending events */

static int _PG_X_events_pending(PG_device *dev)
   {int n;

    n = 0;

    if (_PG_X_display != NULL)
       n = XEventsQueued(_PG_X_display, QueuedAfterFlush);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GET_EVENT_CORE - respond to an events
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

/* _PG_X_GET_FONT_FIELD - find the Nth '-' delimited field in S
 *                      - copy it into D and return a pointer to D
 */

char *_PG_X_get_font_field(char *d, char *s, int n)
   {int i, nc;
    char *ps, *pt;

    ps = s + 1;
    for (i = 1; (i < n) && (ps != NULL); i++)
        ps = strchr(ps, '-') + 1;

    pt = strchr(ps, '-');
    nc = pt - ps;
    strncpy(d, ps, nc);
    d[nc] = '\0';

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_SETUP_FONT - load the named font for X if it exists */

static int _PG_X_setup_font(PG_device *dev, char *face, int size)
   {int i, nf, ret;
    int fsz, lsz;
    char **names;
    char bf[MAXLINE], p[MAXLINE];
    XFontStruct *xf;
    Display *disp;

    ret  = FALSE;
    disp = dev->display;
    if (disp == NULL)
       return(ret);

/* allow the oddball fonts */
    if (size < 4)
       snprintf(bf, MAXLINE, "*%s*", face);
    else
       snprintf(bf, MAXLINE, "*%s*-*-*", face);

    names = XListFonts(disp, bf, 1024, &nf);
    lsz   = -100;
    for (i = 0; i < nf; i++)
        {_PG_X_get_font_field(p, names[i], 7);
	 fsz = SC_stoi(p);
	 if (fsz < size)
	    lsz = fsz;
	 else
	    {if (size-lsz < fsz-size)
	        i--;
	     break;};};
    if (i >= nf)
       i = nf-1;
         
    xf = XLoadQueryFont(disp, names[i]);
    if (xf != NULL)
       {XSetFont(disp, dev->gc, xf->fid);

	if (dev->font_info != NULL)
	   XFreeFont(disp, dev->font_info);

	dev->font_info = xf;
	
	ret = TRUE;};

    XFreeFontNames(names);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_OPEN_IMBEDDED_SCREEN - initialize a "window" on a display screen */
 
PG_device *_PG_X_open_imbedded_screen(PG_device *dev, Display *display,
				      Window window, GC gc,
				      double xf, double yf,
				      double dxf, double dyf)
   {unsigned long bck_color, for_color, valuemask;
    unsigned int icon_width, icon_height, depth;
    int i, Lightest, Light, Light_Gray, Dark_Gray, Dark, Darkest;
    int screen, win_x, win_y, win_width, win_height;
    int display_width, display_height, min_dim;
    int n_dev_colors;
    double intensity;
    XWindowAttributes windowattr;
    PG_font_family *ff;
    PG_dev_geometry *g;

    if (dev == NULL)
       return(NULL);

    PG_setup_markers();

    g = &dev->g;

/* connect to X server once only */
    dev->display = _PG_X_display = display;

    _PG.dsp_intr = SC_io_callback_pid(ConnectionNumber(_PG_X_display),
				      _PG_get_event, NULL, -2);

/* don't let interrupts in while we're setting up the window */
    PG_catch_interrupts(FALSE);

    dev->type_index = GRAPHIC_WINDOW_DEVICE;
    dev->quadrant   = QUAD_FOUR;

    valuemask = 0;

    PG_query_screen(dev, &display_width, &display_height, &n_dev_colors);
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
    if ((xf == 0.0) && (yf == 0.0))
       {xf = 0.5;
        yf = 0.1;};

    if ((dxf == 0.0) && (dyf == 0.0))
       {dxf = 0.5;
        dyf = 0.5;};

/* the following calculations used to use display_width to
 * set up the default size for a square window.  We now
 * use min(display_width, display_height) to make things
 * work better for two screen systems configured as one
 * logical screen
 */
    min_dim = min(display_width, display_height);

    win_x = xf*min_dim;
    win_y = yf*min_dim;

    win_width  = dxf*min_dim;
    win_height = dyf*min_dim;

/* window manager hints */
    icon_width  = 16;
    icon_height = 16;

    _PG_X_point_list = SC_MAKE_ARRAY("_PG_X_OPEN_IMBEDDED_SCREEN", XPoint, NULL);

/* decide on the overall color layout and choose WHITE or BLACK background */
    dev->absolute_n_color = n_dev_colors;
    intensity = dev->max_intensity*MAXPIX;
    if (dev->background_color_white)
       {if (n_dev_colors == 2)
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
        for_color  = BlackPixel(dev->display, screen);}
    else
       {if (n_dev_colors == 2)
           {Color_Map(dev, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            dev->ncolor = 2;}
        else
           {Color_Map(dev, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                           10, 11, 12, 13, 14, 15);
            dev->ncolor = N_COLORS;};
        Lightest   = intensity;
        Light      = intensity;
        Light_Gray = 0.8*intensity;
        Dark_Gray  = 0.5*intensity;
        Dark       = 0;
        Darkest    = 0;
        bck_color  = BlackPixel(dev->display, screen);
        for_color  = WhitePixel(dev->display, screen);};

/* Note: This flag was added in order to get PG_define_region and
 *       PG_move_object to work properly. Whether the GS_XOR logical
 *       operation doublely has anything to do with it is uncertain.
 */
    dev->xor_parity = (bck_color == 0);

/* attach the window */
    dev->window = window;

    XSelectInput(dev->display, dev->window,
                 StructureNotifyMask |
                 ExposureMask |
/*
                 EnterWindowMask |
                 LeaveWindowMask |
*/
                 PointerMotionMask |
/*
                 ButtonMotionMask |
                 Button1MotionMask |
                 Button2MotionMask |
                 Button3MotionMask |
*/
                 KeyPressMask |
                 KeyReleaseMask |
                 ButtonPressMask |
                 ButtonReleaseMask);

    if (dev->use_pixmap)
       dev->pixmap = XCreatePixmap(dev->display, dev->window,
				   win_width, win_height, depth);

/* compute the view port */
    _PG_default_viewspace(dev, TRUE);

    PG_init_viewspace(dev, TRUE);
 
/* attach graphics context */
    dev->gc = gc;

/* get window width */
    XGetWindowAttributes(dev->display, dev->window, &windowattr);
 
/* change the devices idea of window shape from NDC to pixels */
    g->hwin[0] = xf*min_dim;
    g->hwin[1] = g->hwin[0] + windowattr.width;
    g->hwin[2] = yf*min_dim;
    g->hwin[3] = g->hwin[2] + windowattr.height;
 
    SET_PC_FROM_HWIN(g);

/* set font */
    ff = PG_make_font_family(dev, "helvetica", NULL, 4,
                             "-helvetica-medium-r-",
                             "-helvetica-medium-o-",
                             "-helvetica-bold-r-",
                             "-helvetica-bold-o-");

    ff = PG_make_font_family(dev, "times", ff, 4,
                             "-times-medium-r-",
                             "-times-medium-i-",
                             "-times-bold-r-",
                             "-times-bold-i-");

    ff = PG_make_font_family(dev, "courier", ff, 4,
                             "-courier-medium-r-",
                             "-courier-medium-o-",
                             "-courier-bold-r-",
                             "-courier-bold-o-");

    dev->font_family = ff;

    PG_set_font(dev, "helvetica", "medium", 12);
    for (i = 0; dev->font_info == NULL; i++)
        {if (i > 3)
	    return(NULL);
	 _PG_X_setup_font(dev, backup_font[i], backup_size[i]);};

/* put in the default palettes */
    PG_setup_standard_palettes(dev, 64,
			       Light, Dark,
			       Light_Gray, Dark_Gray,
			       Lightest, Darkest);

/* remember this device for event handling purposes */
    _PG_push_device(dev);

/* turn interrupt handling back on */
    PG_catch_interrupts(TRUE);

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_OPEN_SCREEN - initialize a "window" on a display screen */
 
static PG_device *_PG_X_open_screen(PG_device *dev,
				    double xf, double yf,
				    double dxf, double dyf)
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
    XEvent report;
    GC xgc;
    PG_font_family *ff;
    PG_dev_geometry *g;

    if (dev == NULL)
       return(NULL);

    g = &dev->g;

    PG_setup_markers();

/* don't let interrupts in while we're setting up the window */
    PG_catch_interrupts(FALSE);

    dev->type_index = GRAPHIC_WINDOW_DEVICE;
    dev->quadrant   = QUAD_FOUR;

    valuemask = 0;

    PG_query_screen(dev, &display_width, &display_height, &n_dev_colors);
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
    if ((xf == 0.0) && (yf == 0.0))
       {xf = 0.5;
        yf = 0.1;};

    if ((dxf == 0.0) && (dyf == 0.0))
       {dxf = 0.5;
        dyf = 0.5;};

/* the following calculations used to use display_width to */
/* set up the default size for a square window.  We now    */
/* use min(display_width, display_height) to make things   */
/* work better for two screen systems configured as one    */
/* logical screen.                                         */

    min_dim = min(display_width, display_height);

    win_x = xf*min_dim;
    win_y = yf*min_dim;

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

    if (_PG_X_point_list == NULL)
       _PG_X_point_list = SC_MAKE_ARRAY("PERM|_PG_X_OPEN_SCREEN", XPoint, NULL);

/* GOTCHA: if we do this it will be a memory leak
    window_name = SC_strsavef(dev->title,
                              "char*:_PG_X_OPEN_SCREEN:name");
*/
    Xargc = 0;
    Xargv = NULL;
 
/* decide on the overall color layout and choose WHITE or BLACK background */
    dev->absolute_n_color = n_dev_colors;
    intensity = dev->max_intensity*MAXPIX;
    if (dev->background_color_white)
       {if (n_dev_colors == 2)
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
        for_color  = BlackPixel(dev->display, screen);}
    else
       {if (n_dev_colors == 2)
           {Color_Map(dev, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
            dev->ncolor = 2;}
        else
           {Color_Map(dev, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                           10, 11, 12, 13, 14, 15);
            dev->ncolor = N_COLORS;};
        Lightest   = intensity;
        Light      = intensity;
        Light_Gray = 0.8*intensity;
        Dark_Gray  = 0.5*intensity;
        Dark       = 0;
        Darkest    = 0;
        bck_color  = BlackPixel(dev->display, screen);
        for_color  = WhitePixel(dev->display, screen);};

/* NOTE: This flag was added in order to get PG_define_region and
 *       PG_move_object to work properly. Whether the GS_XOR logical
 *       operation doublely has anything to do with it is uncertain.
 */
    dev->xor_parity = (bck_color == 0);

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
/*
                 EnterWindowMask |
                 LeaveWindowMask |
*/
                 PointerMotionMask |
/*
                 ButtonMotionMask |
                 Button1MotionMask |
                 Button2MotionMask |
                 Button3MotionMask |
*/
                 KeyPressMask |
                 KeyReleaseMask |
                 ButtonPressMask |
                 ButtonReleaseMask);

    XMapWindow(dev->display, dev->window);
    XNextEvent(dev->display, &report);
 
    if (dev->use_pixmap)
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
/*
    g->hwin[0] = windowattr.x;
    g->hwin[2] = windowattr.y;
*/
    g->hwin[0] = xf*min_dim;
    g->hwin[2] = yf*min_dim;
/*    g->hwin[2] = yf*display_height; */

    g->hwin[1] = g->hwin[0] + windowattr.width;
    g->hwin[3] = g->hwin[2] + windowattr.height;
 
    SET_PC_FROM_HWIN(g);

/* set font */
    ff = PG_make_font_family(dev, "helvetica", NULL, 4,
                             "-helvetica-medium-r-",
                             "-helvetica-medium-o-",
                             "-helvetica-bold-r-",
                             "-helvetica-bold-o-");

    ff = PG_make_font_family(dev, "times", ff, 4,
                             "-times-medium-r-",
                             "-times-medium-i-",
                             "-times-bold-r-",
                             "-times-bold-i-");

    ff = PG_make_font_family(dev, "courier", ff, 4,
                             "-courier-medium-r-",
                             "-courier-medium-o-",
                             "-courier-bold-r-",
                             "-courier-bold-o-");

    dev->font_family = ff;

    PG_set_font(dev, "helvetica", "medium", 12);
    for (i = 0; dev->font_info == NULL; i++)
        {if (i > 3)
	    return(NULL);
	 _PG_X_setup_font(dev, backup_font[i], backup_size[i]);};

    npc = (n_dev_colors == 256) ? 64 : _PG_gattrs.max_color_factor;

/* put in the default palettes */
    PG_setup_standard_palettes(dev, npc,
			       Light, Dark,
			       Light_Gray, Dark_Gray,
			       Lightest, Darkest);

/* remember this device for event handling purposes */
    _PG_push_device(dev);

/* force a clear screen, i.e. a clear pixmap */
    if (dev->use_pixmap)
       PG_clear_screen(dev);

/* turn interrupt handling back on */
    PG_catch_interrupts(TRUE);

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_IO_ERROR - handle I/O errors for X */

static int _PG_X_io_error(Display *disp)
   {int fd;
    
    fd = ConnectionNumber(disp);

    PG_remove_callback(&fd);

    XCloseDisplay(disp);
    _PG_X_display_name[0] = '\0';
    _PG_X_display         = NULL;

    return(100);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_IO_REJECT - handle reject message from poll */

static void _PG_X_io_reject(int fd, int mask, void *a)
   {
    
    PG_remove_callback(&fd);

    XCloseDisplay(_PG_X_display);
    _PG_X_display_name[0] = '\0';
    _PG_X_display         = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_CONNECT_SERVER - connect to the X server
 *                      - and register call backs for interrupt driven I/O
 *                      - on stdin and the connection to the server
 *                      - do this at most one time in a program
 */

static void _PG_X_connect_server(PG_device *dev)
   {int fid, nc;
    char *wn;

/* ignore interrupts while opening the console */
    PG_catch_interrupts(FALSE);

    _PG.tty_intr = PG_init_event_handling();

/* decide on the source of the display name */
    if (SC_regx_match(dev->name, "*:*.*"))
       wn = dev->name;
    else
       wn = getenv("DISPLAY");

/* if the name of the display has changed close the existing one
 * this should help applications whose display may change in the middle
 */
    if ((_PG_X_display != NULL) &&
	(wn != NULL) && (strcmp(_PG_X_display_name, wn) != 0))
       {fid = ConnectionNumber(_PG_X_display);
	PG_remove_callback(&fid);
	XCloseDisplay(_PG_X_display);
        _PG_X_display_name[0] = '\0';
	_PG_X_display         = NULL;};

/* open the display if it is not already open */
    if (_PG_X_display == NULL)
       {if (wn != NULL)
	   _PG_X_display = XOpenDisplay(wn);

/* if successful register the call back on the connection */
        if (_PG_X_display != NULL)
	   {nc = strlen(wn);
	    SC_strncpy(_PG_X_display_name, MAXLINE, wn, nc);

	    fid          = ConnectionNumber(_PG_X_display);
	    _PG.dsp_intr = PG_loop_callback(0, &fid,
					    _PG_get_event,
					    _PG_X_io_reject,
					    -2);
	    XSetIOErrorHandler(_PG_X_io_error);}

/* otherwise we failed to make a connection to the server
 * we ought to be able to continue without having to can the interrupts
 * but debugging gets to be horrendous, so ...
 */
	else
	   {PG_IO_INTERRUPTS(FALSE);
	    _PG.tty_intr = FALSE;};};

/* if interrupts were on before, turn them back on again */
    PG_catch_interrupts(SC_io_interrupts_on);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_QUERY_SCREEN - query some physical device characteristics */

static void _PG_X_query_screen(PG_device *dev, int *pdx, int *pdy, int *pnc)
   {int dx, dy, nc;
    int screen, n_planes;

    if (dev->g.phys_width == -1)

/* connect to X server once only */
       {_PG_X_connect_server(dev);

	dx = 0;
	dy = 0;
	nc = 0;
	if (_PG_X_display != NULL)
	   {dev->display = _PG_X_display;
 
	    screen   = DefaultScreen(_PG_X_display);
	    n_planes = DisplayPlanes(_PG_X_display, screen);

	    dx = DisplayWidth(_PG_X_display, screen);
	    dy = DisplayHeight(_PG_X_display, screen);
	    nc = 1 << n_planes;};

	dev->g.phys_width  = dx;
	dev->g.phys_height = dy;
	dev->phys_n_colors = nc;};

    *pdx = dev->g.phys_width;
    *pdy = dev->g.phys_height;
    *pnc = dev->phys_n_colors;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_MAP_TO_COLOR_TABLE - map the PGS palette to host
 *                          - color table values
 */

static void _PG_X_map_to_color_table(PG_device *dev, PG_palette *pal)
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

    pi     = FMAKE_N(unsigned long, n_pal_colors,
                     "_PG_X_MAP_TO_COLOR_TABLE:pi");
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
    for (j = 0, i = 0; i < n_dev_colors; i++)
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

/* _PG_X_MATCH_RGB_COLORS - map the RGB references in the palette to
 *                        - valid colors from the root color table
 */

static void _PG_X_match_rgb_colors(PG_device *dev, PG_palette *pal)
   {int i, j, npc;
    unsigned long *pi, *pir;
    RGB_color_map *true_cm;

/* add two for the black and white colors at the top */
    npc     = pal->n_pal_colors + 2;
    true_cm = pal->true_colormap;

    pi  = FMAKE_N(unsigned long, npc,
                  "_PG_X_MATCH_RGB_COLORS:pi");
    pir = dev->color_table->pixel_value;

    for (i = 0; i < npc; i++)
        {j = PG_rgb_index(dev, true_cm++);
         pi[i] = pir[j];}

    pal->pixel_value = pi;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_SET_FONT - set the character font */

static int _PG_X_set_font(PG_device *dev, char *face, char *style, int size)
   {int nfont, nstyle, rv;
    char *font_name;

    if (!PG_setup_font(dev, face, style, size, &font_name, &nfont, &nstyle))
       rv = FALSE;
    else
       rv = _PG_X_setup_font(dev, font_name, size);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_OPEN_CONSOLE - open up a special window to be used as a console or
 *                    - terminal window for systems which can't do this for
 *                    - themselves (i.e. MacIntosh) or the SINGLE SCREEN
 *                    - devices
 */
 
static int _PG_X_open_console(char *title, char *type, int bckgr,
			      double xf, double yf, double dxf, double dyf)
   {int dx, dy, nc;
    double tx[PG_SPACEDM];

    if (PG_console_device == NULL)
       {PG_setup_markers();

	PG_console_device = PG_make_device("TEXT", type, title);
	if (PG_console_device == NULL)
	   return(FALSE);

	PG_query_screen(PG_console_device, &dx, &dy, &nc);
	if ((dx == 0) && (dy == 0) && (nc == 0))
	   return(FALSE);

	PG_console_device->background_color_white = bckgr;

/* initialize text size info */
	tx[0] = 1.0/TXSPAN;
	tx[1] = PG_console_device->txt_ratio*tx[0];
	PG_set_char_size_n(PG_console_device, 2, NORMC, tx);

	SC_setbuf(stdout, NULL);

	SC_set_get_line(PG_wind_fgets);
	SC_set_put_line(PG_fprintf);
	SC_set_put_string(PG_fputs);

	_PG_push_device(PG_console_device);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_FLUSH_EVENTS - flush all pending events on the device */
 
static void _PG_X_flush_events(PG_device *dev)
   {Display *disp;

    disp = dev->display;
    if (disp != NULL)
       XSync(disp, True);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_CLOSE_DEVICE - close a device */
 
static void _PG_X_close_device(PG_device *dev)
   {Display *disp;

    disp = dev->display;
    if (disp == NULL)
       return;

    if (dev->window)
       {XDestroyWindow(disp, dev->window);
        XFlush(disp);};

    _PG_remove_device(dev);

    if (SC_array_get_n(_PG.devlst) == 0)
       SC_free_array(_PG_X_point_list, NULL);

/* clean up the device */
    PG_rl_device(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_CLOSE_CONSOLE - close the console device */
 
static void _PG_X_close_console(void)
   {

    _PG_remove_device(PG_console_device);

    PG_console_device = NULL;

/* connect I/O to standard functions */
    SC_set_put_line(io_printf);
    SC_set_put_string(io_puts);
    SC_set_get_line(io_gets);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_GET_NEXT_EVENT - get the next event
 *                      - return TRUE iff successful
 */

static int _PG_X_get_next_event(PG_event *ev)
   {

    XNextEvent(_PG_X_display, ev);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_FIND_DEVICE - return the device from the device list
 *                   - which containing WINDOW
 */

PG_device *_PG_X_find_device(Window window)
   {int i, n;
    PG_device *dev;

    n = SC_array_get_n(_PG.devlst);
    for (i = 0; i < n; i++)
        {dev = *(PG_device **) SC_array_get(_PG.devlst, i);
         if ((dev != NULL) && (dev->window == window))
             break;};

    if (i >= n)
       dev = NULL;

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_GET_EVENT_DEVICE - return the device in which the given event
 *                        - occurred
 */

static PG_device *_PG_X_get_event_device(PG_event *ev)
   {Window window;
    PG_device *dev;

    window = ev->xany.window;
    dev    = _PG_X_find_device(window);

    return(dev);}

/*--------------------------------------------------------------------------*/

/*                           SCREEN STATE ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _PG_X_FILL_PIXMAP - helper to fill pixmap */

void _PG_X_fill_pixmap(PG_device *dev, unsigned int *ipc, int color)
  {int screen;
   unsigned long valuemask;
   Display *disp;
   XGCValues vreturn;
   GC xgc;

   if (dev->use_pixmap)
      {disp      = dev->display;
       screen    = DefaultScreen(disp);
       valuemask = 0;
       xgc       = XCreateGC(disp, dev->window, valuemask, &vreturn);

       XSetForeground(disp, xgc, color);
       XFillRectangle(disp, dev->pixmap, xgc, ipc[0], ipc[1], ipc[2], ipc[3]);
       XFreeGC(disp, xgc);};

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_CLEAR_DRAWABLE - clear the drawable in DEV */

static void _PG_X_clear_drawable(PG_device *dev)
  {int color, screen;
   unsigned int ipc[PG_BOXSZ];
   Display *disp;
   PG_dev_geometry *g;

   g    = &dev->g;
   disp = dev->display;

   if (disp != NULL)
      {XClearWindow(disp, dev->window);

       XFlush(disp);

       if (dev->use_pixmap)
	  {ipc[0] = 0;
	   ipc[1] = 0;
	   ipc[2] = PG_window_width(dev);
	   ipc[3] = PG_window_height(dev);
	   screen = DefaultScreen(disp);
	   color  = (dev->background_color_white) ?
	            WhitePixel(disp, screen) :
	            BlackPixel(disp, screen);

	   _PG_X_fill_pixmap(dev, ipc, color);};};

   PG_clear_raster_device(dev);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_CLEAR_WINDOW - clear the screen */

static void _PG_X_clear_window(PG_device *dev)
  {

   if (dev != NULL)
      {PG_make_device_current(dev);

       _PG_X_clear_drawable(dev);

       PG_release_current_device(dev);};

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_CLEAR_VIEWPORT - clear the viewport */
 
static void _PG_X_clear_viewport(PG_device *dev)
   {PG_dev_geometry *g;

    g = &dev->g;

    PG_clear_region(dev, 2, NORMC, g->ndc, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_CLEAR_REGION - clear the rectangular region specified in NDC */
 
static void _PG_X_clear_region(PG_device *dev, int nd, PG_coord_sys cs,
			       double *bx, int pad)
   {int iw, ih;
    int screen, color;
    unsigned int ipc[PG_BOXSZ];
    double pc[PG_BOXSZ];
    Display *disp;
    PG_dev_geometry *g;

    if (dev != NULL)
       {disp = dev->display;
	if (disp == NULL)
	   return;

	g = &dev->g;

	PG_trans_box(dev, 2, cs, bx, PIXELC, pc);

	iw = (pc[1] - pc[0] + 2*pad);
	ih = (pc[3] - pc[2] + 2*pad);
	pc[0] -= pad;
	pc[2] -= pad;

	pc[0] = max(pc[0], 0.0);
	pc[2] = max(pc[2], 0.0);

	ipc[0] = pc[0];
	ipc[1] = pc[2];
	ipc[2] = iw;
	ipc[3] = ih;
	    
	PG_make_device_current(dev);

	PG_QUAD_FOUR_POINT(dev, ipc);
	ipc[1] -= ih;

	if (dev->use_pixmap)
	   {screen = DefaultScreen(disp);
	    color  = (dev->background_color_white) ?
	             WhitePixel(disp, screen) :
		     BlackPixel(disp, screen);

	    _PG_X_fill_pixmap(dev, ipc, color);}

	else
	   XClearArea(disp, PG_X11_DRAWABLE(dev), ipc[0], ipc[1],
		       ipc[2], ipc[3], TRUE);
    
	PG_release_current_device(dev);};

    _PG_clear_raster_region(dev, nd, cs, bx, pad);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_UPDATE_VS - update the view surface for the given device */
 
static void _PG_X_update_vs(PG_device *dev)
   {Display *disp;

    if (dev == NULL)
       return;

    if (!dev->use_pixmap)
       {disp = dev->display;
	if (disp == NULL)
	   return;

	XFlush(disp);

	PG_release_current_device(dev);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_FINISH_PLOT - do what's necessary to finish up a graphical image
 *                   - and get the device updated with the image
 *                   - after this function nothing more can be added to
 *                   - the image
 */
 
static void _PG_X_finish_plot(PG_device *dev)
   {unsigned int width, height;
    Display *disp;

    if (dev == NULL)
       return;

    disp = dev->display;
    if (disp == NULL)
       return;

    if (dev->use_pixmap)
       {width  = PG_window_width(dev);
	height = PG_window_height(dev);

        XCopyArea(disp, dev->pixmap, dev->window,
		  dev->gc, 0, 0, width, height, 0, 0);};

    PG_make_device_current(dev);

    _PG_X_put_raster(dev, dev->BLACK, 0, 0, -1, -1, 1.0, 0.0);

    PG_update_vs(dev);

    PG_release_current_device(dev);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_CLEAR_PAGE - clear the current page
 *                  - and go to the line i on the screen
 */
 
static void _PG_X_clear_page(PG_device *dev, int i)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_EXPOSE_DEVICE - make this device the topmost one */
 
static void _PG_X_expose_device(PG_device *dev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_MAKE_DEVICE_CURRENT - make this device the current one for drawing
 *                           - purposes
 */
 
static void _PG_X_make_device_current(PG_device *dev)
   {Window window, root, parent, child, *children;
    Display *display;
    int x, y, root_x, root_y;
    unsigned int n, ow, oh;
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
        if (children != NULL)
	   XFree((caddr_t) children);

        XGetGeometry(display, window, &root,
                     &x, &y, &width, &height, &border_width, &depth);

	ow = PG_window_width(dev);
        oh = PG_window_height(dev);

	if (dev->use_pixmap &&
	    ((oh != height) || (ow != width)))
           {XFreePixmap(display, dev->pixmap);
	    dev->pixmap = XCreatePixmap(display, window,
					width, height, depth);

	    _PG_X_clear_drawable(dev);};

	XTranslateCoordinates(display, window, root,
			      0, 0, &root_x, &root_y, &child);

        g->hwin[0] = root_x;
        g->hwin[2] = root_y;
        g->hwin[3] = g->hwin[2] + height;
        g->hwin[1] = g->hwin[0] + width;

	SET_PC_FROM_HWIN(g);

	PG_set_viewspace(dev, 3, WORLDC, wc);};

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_RELEASE_CURRENT_DEVICE - make no device current */
 
static void _PG_X_release_current_device(PG_device *dev)
   {

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_WRITE_TEXT - write out text to the appropriate device */
 
static void _PG_X_write_text(PG_device *dev, FILE *fp, char *s)
   {int id, nd;
    double x[PG_SPACEDM];
    Display *disp;

    if (dev == NULL)
       return;

    if (fp == stdscr)
       {disp = dev->display;
        if (disp == NULL)
           return;

	nd = 2;

	for (id = 0; id < nd; id++)
	    x[id] = dev->tcur[id];

	PG_trans_point(dev, 2, WORLDC, x, PIXELC, x);

	_PG_X_draw_text(dev, s, x);}

    else
       io_printf(fp, "%s", s);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_X_NEXT_LINE - do a controlled newline */
 
static void _PG_X_next_line(PG_device *dev, FILE *fp)
   {

/* turn off SIGIO handler */
    PG_catch_interrupts(FALSE);

/* GOTCHA: what do we doublely want to do when it is the screen? */
    if (fp == stdscr)
       {}
    else
       putchar('\n');

/* turn on SIGIO handler */
    PG_catch_interrupts(SC_io_interrupts_on);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_WIND_FGETS - get and return a string from the window environment
 *                  - start with a clean event slate, and cycle the main
 *                  - event loop until some event sets
 *                  - PG_console_device->DoneFlag
 *                  - NOTE: there is a logical problem here in that
 *                  -       sometimes you just want to use PG_wind_fgets to
 *                  -       process events
 *                  -       so if str is NULL don't hang around for
 *                  -       a string
 */
 
static char *_PG_X_wind_fgets(char *str, int maxlen, FILE *fp)
   {char *p;

    p = NULL;
    if (fp != stdin)
       p = io_gets(str, maxlen, fp);

    else
       {PG_setup_input(str, maxlen);

	if (SETJMP(io_avail) == ERR_FREE)

/* turn off interrupts until the next call to _PG_X_wind_fgets */
	   {PG_catch_interrupts(FALSE);
	    p = _PG_gcont.input_bf;}

	else

/* don't catch interrupts until the SETJMP is successful */
           {PG_catch_interrupts(TRUE);
	    PG_poll(TRUE, 100);
	    PG_catch_interrupts(FALSE);};};

    return(p);}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

/* _PG_GET_EVENT_CHAR - handle key down events */

static void _PG_get_event_char(PG_device *dev, PG_event *ev)
   {char bf[80];
    int iev[PG_SPACEDM], mod;

    PG_key_event_info(dev, ev, iev, bf, 80, &mod);
    _PG.next_char = bf[0];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_GET_CHAR - get one char */

static int _PG_X_get_char(PG_device *dev)
   {PFEventHand old;

/* save the old key down handler */
    old = dev->key_down_event_handler.fnc;
    dev->key_down_event_handler.fnc = _PG_get_event_char;

    _PG.next_char = 0;
    while (!_PG.next_char)
       PG_poll(FALSE, -1);

/* restore the old key down handler */
    dev->key_down_event_handler.fnc = old;

    return(_PG.next_char);}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* _PG_X_EXPOSE_EVENT - handle an expose event */

static void _PG_X_expose_event(PG_device *dev, PG_event *ev)
   {
/*
    PRINT(stdout, "\nExpose Event: %s", event[ev->type]);
*/
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_X_UPDATE_EVENT - handle an update event */

static void _PG_X_update_event(PG_device *dev, PG_event *ev)
   {
/*
    PRINT(stdout, "\nUpdate Event: %s", event[ev->type]);
*/
    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_X_MOUSE_DOWN_EVENT - handle a mouse down event */

static void _PG_X_mouse_down_event(PG_device *dev, PG_event *ev)
   {
/*
    PRINT(stdout, "\nMouse Down Event: %s", event[ev->type]);
*/
    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_X_MOUSE_UP_EVENT - handle a mouse up event */

static void _PG_X_mouse_up_event(PG_device *dev, PG_event *ev)
   {
/*
    PRINT(stdout, "\nMouse Up Event: %s", event[ev->type]);
*/
    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_X_KEY_DOWN_EVENT - handle a key down event */

static void _PG_X_key_down_event(PG_device *dev, PG_event *ev)
   {
/*
    PRINT(stdout, "\nKey Down Event: %s", event[ev->type]);
*/
    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_X_KEY_UP_EVENT - handle a key up event */

static void _PG_X_key_up_event(PG_device *dev, PG_event *ev)
   {
/*
    PRINT(stdout, "\nKey Up Event: %s", event[ev->type]);
*/
    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_X_DEFAULT_EVENT - handle any event not already handled elsewhere */

static void _PG_X_default_event(PG_device *dev, PG_event *ev)
   {
/*
    PRINT(stdout, "\nUnknown Event: %s", event[ev->type]);
*/
    return;}

/*------------------------------------------------------------------------*/

#endif

/*------------------------------------------------------------------------*/

/* _PG_X_QUERY_POINTER - query the pointer for location and button status
 *                     - the button state is represented as follows
 *                     -   bit 0 - MOUSE_LEFT state
 *                     -   bit 1 - MOUSE_RIGHT state
 *                     -   bit 2 - MOUSE_MIDDLE state
 *                     -   bit 4 - KEY_SHIFT state
 *                     -   bit 5 - KEY_CNTL state
 *                     -   bit 6 - KEY_ALT state
 */

static void _PG_X_query_pointer(PG_device *dev, int *ir, int *pb, int *pq)
   {int btn, mod;
    int rx[PG_SPACEDM];
    unsigned int buttons;
    Window window, root, child;
    Display *display;

    display = dev->display;
    window  = dev->window;

    mod = XQueryPointer(display, window, &root, &child,
                        &rx[0], &rx[1],
                        &ir[0], &ir[1], &buttons);

    PG_QUAD_FOUR_POINT(dev, ir);

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

/* _PG_X_MOUSE_EVENT_INFO - return mouse event info for X */

static void _PG_X_mouse_event_info(PG_device *dev, PG_event *ev,
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
	case 4 :
	     eb = MOUSE_WHEEL_UP;
	     break;
	case 5 :
	     eb = MOUSE_WHEEL_DOWN;
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

/* _PG_X_KEY_EVENT_INFO - return keyboard event info for X */

static void _PG_X_key_event_info(PG_device *dev, PG_event *ev,
				 int *iev, char *bf, int n, int *peq)
   {int et, eq;
    KeySym key;
    XComposeStatus stat;

    XLookupString(&ev->xkey, bf, n, &key, &stat);

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

/* _PG_X_PUTS - put a string to the console window */

static void _PG_X_puts(char *bf)
   {

    puts(bf);
    FLUSH_ON(stdout, bf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_X11_DEVICE - do the device dependent device initialization
 *                     - for PG_make_device
 */

int PG_setup_x11_device(PG_device *d)
   {

    d->X_cap_style  = CapRound;
    d->X_line_style = LineSolid;
    d->X_join_style = JoinRound;
    d->X_line_width = 0;
    d->display      = NULL;
    d->font_info    = NULL;

    d->type_index   = GRAPHIC_WINDOW_DEVICE;
    d->is_visible   = TRUE;

    d->events_pending         = _PG_X_events_pending;
    d->query_pointer          = _PG_X_query_pointer;
    d->mouse_event_info       = _PG_X_mouse_event_info;
    d->key_event_info         = _PG_X_key_event_info;
    d->clear_page             = _PG_X_clear_page;
    d->clear_window           = _PG_X_clear_window;
    d->clear_viewport         = _PG_X_clear_viewport;
    d->clear_region           = _PG_X_clear_region;
    d->close_console          = _PG_X_close_console;
    d->close_device           = _PG_X_close_device;
    d->flush_events           = _PG_X_flush_events;
    d->draw_dj_polyln_2       = _PG_X_draw_disjoint_polyline_2;
    d->draw_curve             = _PG_X_draw_curve;
    d->draw_to_abs            = _PG_X_draw_to_abs;
    d->draw_to_rel            = _PG_X_draw_to_rel;
    d->expose_device          = _PG_X_expose_device;
    d->finish_plot            = _PG_X_finish_plot;
    d->get_char               = _PG_X_get_char;
    d->get_image              = _PG_X_get_image;
    d->get_text_ext           = _PG_X_get_text_ext;
    d->ggets                  = (PFfgets) _PG_X_wind_fgets;
    d->gputs                  = _PG_X_puts;
    d->make_device_current    = _PG_X_make_device_current;
    d->map_to_color_table     = _PG_X_map_to_color_table;
    d->match_rgb_colors       = _PG_X_match_rgb_colors;
    d->move_gr_abs            = _PG_X_move_gr_abs;
    d->move_tx_abs            = _PG_X_move_tx_abs;
    d->move_tx_rel            = _PG_X_move_tx_rel;
    d->next_line              = _PG_X_next_line;
    d->open_screen            = _PG_X_open_screen;
    d->put_image              = _PG_X_put_image;
    d->query_screen           = _PG_X_query_screen;
    d->release_current_device = _PG_X_release_current_device;
    d->set_clipping           = _PG_X_set_clipping;
    d->set_char_line          = _PG_X_set_char_line;
    d->set_char_path          = _PG_X_set_char_path;
    d->set_char_precision     = _PG_X_set_char_precision;
    d->set_char_size          = _PG_X_set_char_size;
    d->set_char_space         = _PG_X_set_char_space;
    d->set_char_up            = _PG_X_set_char_up;
    d->set_fill_color         = _PG_X_set_fill_color;
    d->set_font               = _PG_X_set_font;
    d->set_line_color         = _PG_X_set_line_color;
    d->set_line_style         = _PG_X_set_line_style;
    d->set_line_width         = _PG_X_set_line_width;
    d->set_logical_op         = _PG_X_set_logical_op;
    d->set_text_color         = _PG_X_set_text_color;
    d->shade_poly             = _PG_X_shade_poly;
    d->fill_curve             = _PG_X_fill_curve;
    d->update_vs              = _PG_X_update_vs;
    d->write_text             = _PG_X_write_text;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_X_SETUP_CTRLS_GLB - setup global control structures */

void PG_X_setup_ctrls_glb(void)
   {

    _PG_gcont.get_event    = _PG_X_get_next_event;
    _PG_gcont.event_device = _PG_X_get_event_device;
    _PG_gcont.open_console = _PG_X_open_console;
    _PG_gcont.setup_window = PG_setup_x11_device;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
