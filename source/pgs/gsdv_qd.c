/*
 * GSDV_QD.C - PGS Quickdraw routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
  
#include "scope_quickdraw.h"

#define WIDTH_MIN  40
#define HEIGHT_MIN 40

#define SCROLL_BAR_WIDTH    16
#define ACTIVE                0
#define INACTIVE            255

extern PFVoid
 PG_lookup_callback(char *name);

static int
 _PG_mess_flag;
 
static WindowPtr
 _PG_actv,
 _PG_whch;
 
static Boolean
 _PG_nil = 0;

static CursHandle
 ibeamHdl;


/* from or for corresponding PR file */

int
 _PG_qd_set_font(PG_device *dev, char *face, char *style, int size);

void
 _PG_qd_draw_disjoint_polyline_2(PG_device *dev, double **r,
                                 long n, int clip, int coord),
 _PG_qd_draw_curve(PG_device *dev, PG_curve *crv, int clip),
 _PG_qd_draw_to_abs(PG_device *dev, double x, double y),
 _PG_qd_draw_to_rel(PG_device *dev, double x, double y),
 _PG_qd_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p),
 _PG_qd_set_clipping(PG_device *dev, int flag),
 _PG_qd_set_char_line(PG_device *dev, int n),
 _PG_qd_set_char_path(PG_device *dev, double x, double y),
 _PG_qd_set_char_precision(PG_device *dev, int p),
 _PG_qd_set_char_space(PG_device *dev, double s),
 _PG_qd_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x),
 _PG_qd_set_char_up(PG_device *dev, double x, double y),
 _PG_qd_set_fill_color(PG_device *dev, int color, int mapped),
 _PG_qd_set_line_color(PG_device *dev, int color, int mapped),
 _PG_qd_set_line_style(PG_device *dev, int style),
 _PG_qd_set_line_width(PG_device *dev, double width),
 _PG_qd_set_logical_op(PG_device *dev, PG_logical_operation lop),
 _PG_qd_set_text_color(PG_device *dev, int color, int mapped),
 _PG_qd_shade_poly(PG_device *dev, int nd, int n, double **r),
 _PG_qd_fill_curve(PG_device *dev, PG_curve *crv),
 _PG_qd_move_gr_abs(PG_device *dev, double x, double y),
 _PG_qd_move_tx_abs(PG_device *dev, double x, double y),
 _PG_qd_move_tx_rel(PG_device *dev, double x, double y),
 _PG_qd_get_image(PG_device *dev, unsigned char *bf, int ix, int iy,
                  int nx, int ny),
 _PG_qd_put_image(PG_device *dev, unsigned char *bf, int ix, int iy,
                  int nx, int ny),
 _PG_set_text_font_mac(PG_device *dev, int index),
 _PG_set_text_size_mac(PG_device *dev, int size_index, int flag);
 

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_OPEN_SCREEN - initialize a "window" on a display screen */
 
PG_device *_PG_qd_open_screen(PG_device *dev, double xf, double yf,
			      double dxf, double dyf)
   {int bck_color, for_color, idx, idy;
    double intensity;
    unsigned int Lightest, Light, Light_Gray, Dark_Gray, Dark, Darkest;
    PG_font_family *ff;
    Rect physical_screen_shape;

#ifdef MAC_COLOR
    RGB_color_map *colormap;
#endif

    if (dev == NULL)
       return(NULL);

    PG_setup_markers();

    _PG_qd_init_mac();
 
    dev->quadrant = QUAD_FOUR;

/* get the window shape in NDC */
    if ((xf == 0.0) && (yf == 0.0))
       {xf = 0.5;
        yf = 0.1;};

    if ((dxf == 0.0) && (dyf == 0.0))
       {dxf = 0.5;
        dyf = 0.5;};

/* open appropriate window type */
    if (strcmp(dev->name, "TEXT") == 0)
       _PG_qd_open_text_window(dev, TRUE, xf, yf, dxf, dyf);
    else
       _PG_qd_open_graphics_window(dev, xf, yf, dxf, dyf);

    intensity = dev->max_intensity*MAXPIX;
    if (dev->background_color_white)
       {if (dev->ncolor == 2)
           {Color_Map(dev, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);}
        else
           {Color_Map(dev, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9,
                           10, 11, 12, 13, 14, 15);};
        Lightest   = 0;
        Light      = intensity;
        Light_Gray = 0.8*intensity;
        Dark_Gray  = 0.5*intensity;
        Dark       = 0;
        Darkest    = intensity;}
    else
       {if (dev->ncolor == 2)
           {Color_Map(dev, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);}
        else
           {Color_Map(dev, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                           10, 11, 12, 13, 14, 15);};
        Lightest   = intensity;
        Light      = intensity;
        Light_Gray = 0.8*intensity;
        Dark_Gray  = 0.5*intensity;
        Dark       = 0;
        Darkest    = 0;};

    bck_color  = dev->BLACK;
    for_color  = dev->WHITE;

/* compute the view port */
    _PG_default_viewspace(dev, TRUE);

    PG_init_viewspace(dev, TRUE);
 
/* convert the window shape information from NDC to pixels */
    physical_screen_shape = SCREEN_BITS.bounds;
    idx = physical_screen_shape.right - physical_screen_shape.left;
    idy = physical_screen_shape.bottom - physical_screen_shape.top -
          MENU_HEIGHT;

/* initialize text size info */
    ff = PG_make_font_family(dev, "helvetica", NULL, 4,
                             "Helvetica",
                             "Helvetica",
                             "Helvetica",
                             "Helvetica");

    ff = PG_make_font_family(dev, "times", ff, 4,
                             "Times",
                             "Times",
                             "Times",
                             "Times");

    ff = PG_make_font_family(dev, "courier", ff, 4,
                             "Courier",
                             "Courier",
                             "Courier",
                             "Courier");

    dev->font_family = ff;

    PG_set_font(dev, "helvetica", "medium", 10);
 
/* set MAC colors - the order here matters because devices with fewer planes
 * will have the color map entries overwritten possibly many times
 */

#ifdef MAC_COLOR

/* put in the default palettes */
    PG_setup_standard_palettes(dev, 64,
                   Light, Dark,
                   Light_Gray, Dark_Gray,
                   Lightest, Darkest);

    colormap = dev->current_palette->true_colormap;
    RGBBackColor(&colormap[bck_color]);
    EraseRect(&dev->window->portRect);

#endif

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_QUERY_SCREEN - query some physical device characteristics */

void _PG_qd_query_screen(PG_device *dev, int *pdx, int *pdy, int *pnc)
   {int dx, dy, nc;
    Rect physical_screen_shape;
    PG_dev_geometry *g;

    g = &dev->g;

    if (g->phys_width == -1)
       {physical_screen_shape = SCREEN_BITS.bounds;

	dx = physical_screen_shape.right - physical_screen_shape.left;
	dy = physical_screen_shape.bottom - physical_screen_shape.top -
	     MENU_HEIGHT;

	if (strcmp(dev->type, "MONOCHROME") == 0)
	   nc = 2;
	else
	   nc = 256;
        
	g->phys_width    = dx;
	g->phys_height   = dy;
	dev->phys_n_colors = nc;};

    *pdx = g->phys_width;
    *pdy = g->phys_height;
    *pnc = dev->phys_n_colors;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_MAP_TO_COLOR_TABLE - map the PGS palette to host
 *                           - color table values
 */

void _PG_qd_map_to_color_table(PG_device *dev, PG_palette *pal)
   {return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_MATCH_RGB_COLORS - map the RGB references in the palette to
 *                         - valid colors from the root color table
 */

static void _PG_qd_match_rgb_colors(PG_device *dev, PG_palette *pal)
   {int i, j, npc;
    PaletteHandle hp;
    RGB_color_map *true_cm;

    npc     = pal->n_pal_colors;
    true_cm = pal->true_colormap;
    hp      = pal->hpalette;

/* add two for the black and white colors at the top */
    npc += 2;
    for (i = 0; i < npc; i++)
        j = PG_rgb_index(dev, &true_cm[i]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_OPEN_CONSOLE - open up a special window to be used as a console or
 *                     - terminal window for systems which can't do this for
 *                     - themselves (i.e. MacIntosh) or the SINGLE SCREEN
 *                     - devices
 */
 
static int _PG_qd_open_console(char *title, char *type, int bckgr,
			       double xf, double yf, double dxf, double dyf)
   {int rv;

    rv = _PG_qd_open_cons(title, type, bckgr, TRUE, xf, yf, dxf, dyf);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_OPEN_CONS - open up a special window to be used as a console or
 *                  - terminal window for systems which can't do this for
 *                  - themselves (i.e. MacIntosh) or the SINGLE SCREEN
 *                  - devices
 */
 
int _PG_qd_open_cons(char *title, char *type, int bckgr, int vis,
		     double xf, double yf, double dxf, double dyf)
   {

    PG_setup_markers();

    PG_console_device = PG_make_device("TEXT", type, title);
    _PG_qd_open_text_window(PG_console_device, vis, xf, yf, dxf, dyf);

    SC_set_put_line(PG_wind_fprintf);
    SC_set_put_string(PG_wind_fputs);
    SC_set_get_line(_PG_qd_wind_fgets);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_CLOSE_DEVICE - close a device */
 
void _PG_qd_close_device(PG_device *dev)
   {int i, j;

    CloseWindow(dev->window);

    _PG_remove_device(dev);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_CLOSE_CONSOLE - close the console device */
 
void _PG_qd_close_console(void)
   {int i, j;

    CloseWindow(PG_console_device->window);

    _PG_remove_device(PG_console_device);

    PG_console_device = NULL;

/* connect I/O to standard functions */
    SC_set_put_line(io_printf);
    SC_set_put_string(io_puts);
    SC_set_get_line(io_gets);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_GET_NEXT_EVENT - get the next event
 *                       - return TRUE iff successful
 */

static int _PG_qd_get_next_event(PG_event *ev)
   {int ret;
    EventRecord sys;

    ret = GetNextEvent(everyEvent, &sys);
    if (!ret || (sys.what == nullEvent))
       {Point pt, ptGlobal;
        int wx, wy;
        WindowPtr wnd;
        PG_device *dev;
        static WindowPtr ownd;
        static int owx = 0, owy = 0;

        GetMouse(&pt);
        wx = pt.h;
        wy = pt.v;

/* convert pt to global coordinates */
        ptGlobal = pt;
        LocalToGlobal (&ptGlobal);

        dev = _PG_qd_find_point(ptGlobal);
        if (dev != NULL)
           {wnd = dev->window;
            if ((owx != wx) || (owy != wy) || (ownd != wnd))
               {owx  = wx;
                owy  = wy;
                ownd = wnd;

                sys.where     = ptGlobal;
                sys.what      = MOTION_EVENT;
                sys.message   = 0L;
                sys.when      = 0L;
                sys.modifiers = 0;

                ret = TRUE;};};};

/* NOTE: this will be replaced if the left or right arrow keys were pressed */

    ev->type = sys.what;

/* MAC says RIGHT and MIDDLE mouse events are KEY events
 * NOTE: remember that the original MAC mouse has a single button
 */
    if (sys.what == KEY_DOWN_EVENT)
       {char c;

        c = BitAnd(sys.message, charCodeMask);
	if (c == 0x1d)
           {ev->type = MOUSE_DOWN_EVENT;
	    sys.what = MOUSE_RIGHT;}

	else if (c == 0x1c)
           {ev->type = MOUSE_DOWN_EVENT;
	    sys.what = MOUSE_MIDDLE;};};

    ev->sys_event = sys;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_FIND_DEVICE - return the device from the device list
 *                    - which containing WINDOW
 */

static PG_device *_PG_qd_find_device(WindowPtr window)
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

/* _PG_QD_FIND_POINT - return the device containing the given point */

static PG_device *_PG_qd_find_point(Point pt)
   {int inpart;
    WindowPtr window;
    PG_device *dev;

    inpart = FindWindow(pt, &window);
    dev    = _PG_qd_find_device(window);

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_GET_EVENT_DEVICE - return the device in which the given event
 *                         - occurred
 */

static PG_device *_PG_qd_get_event_device(PG_event *ev)
   {int type;
    WindowPtr window;
    PG_device *dev;

    type = PG_EVENT_TYPE(*ev);
    if ((type == EXPOSE_EVENT) || (type == UPDATE_EVENT))
       {window = (WindowPtr) ev->sys_event.message;
	dev    = _PG_qd_find_device(window);}
    else
        dev = _PG_qd_find_point(ev->sys_event.where);

    return(dev);}

/*--------------------------------------------------------------------------*/

/*                           SCREEN STATE ROUTINES                          */

/*--------------------------------------------------------------------------*/
 
/* _PG_QD_CLEAR_WINDOW - clear the screen */
 
void _PG_qd_clear_window(PG_device *dev)
   {

    if (dev != NULL)
       {SetPort(dev->window);

	EraseRect(&dev->window->portRect);

	PG_release_current_device(dev);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_CLEAR_VIEWPORT - clear the viewport */
 
void _PG_qd_clear_viewport(PG_device *dev)
   {PG_dev_geometry *g;

    g = &dev->g;

    PG_clear_region(dev, 2, NORMC, g->ndc, 1);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_CLEAR_REGION - clear the rectangular region specified in NDC */
 
static void _PG_qd_clear_region(PG_device *dev, int nd, PG_coord_sys cs,
				double *bx, int pad)
   {double ix[PG_BOXSZ];
    Rect sh;

    if (dev != NULL)
       {SetPort(dev->window);

	PG_trans_box(dev, 2, cs, bx, PIXELC, ix);

	ix[0] -= pad;
	ix[2] -= pad;
	ix[1] += pad;
	ix[3] += pad;

	SETRECT(&sh, ix[0], ix[2], ix[1], ix[3]);
	EraseRect(&sh);
 
	PG_release_current_device(dev);};
 
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_UPDATE_VS - update the view surface for the given device */
 
void _PG_qd_update_vs(PG_device *dev)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_FINISH_PLOT - do what's necessary to finish up a graphical image
 *                    - and get the device updated with the image
 *                    - after this function nothing more can be added to
 *                    - the image
 */
 
void _PG_qd_finish_plot(PG_device *dev)
   {
/*
    PG_make_device_current(dev);

    PG_update_vs(dev);

    PG_release_current_device(dev);
*/
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_CLEAR_PAGE - clear the current page
 *                   - and go to the line i on the screen
 */
 
void _PG_qd_clear_page(PG_device *dev, int i)
   {return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_EXPOSE_DEVICE - make this device the topmost one */
 
void _PG_qd_expose_device(PG_device *dev)
   {

    if (dev->title[0] != '\0')
        SelectWindow(dev->window);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_MAKE_DEVICE_CURRENT - make this device the current one for drawing
 *                            - purposes
 */
 
void _PG_qd_make_device_current(PG_device *dev)
   {WindowPtr window;

    window = dev->window;

    if (window != NULL)
       {if (dev->title[0] != '\0')
            SetPort(window);

        BeginUpdate(window);
        EndUpdate(window);};

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_RELEASE_CURRENT_DEVICE - make no device current */
 
void _PG_qd_release_current_device(PG_device *dev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_WRITE_TEXT - write out text to the appropriate device */
 
void _PG_qd_write_text(PG_device *dev, FILE *fp, char *s)
   {

    SetPort(dev->window);
    _PG_textdraw(dev, s);
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_QD_NEXT_LINE - do a controlled newline */
 
void _PG_qd_next_line(PG_device *dev)
   {_PG_textdraw(dev, "\n");

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_WIND_FGETS - get and return a string from the window environment
 *                   - start with a clean event slate, and cycle the main
 *                   - event loop until some event sets
 *                   - PG_console_device->DoneFlag
 */
 
static char *_PG_qd_wind_fgets(char *str, int maxlen, FILE *fp)
   {int type, oldval, newval, lineHght, Hght;
    PG_device *dev, *devC;
    PG_interface_object *iob;
    PG_text_box *b;
    static PG_text_box *b0 = NULL;

    if (PG_console_device == NULL)
       _PG_qd_open_cons("", "COLOR", TRUE, FALSE, 0.0, 0.0, 0.1, 0.1);

    PG_make_device_current(PG_console_device);
    PG_expose_device(PG_console_device);

    if (fp != stdin)
       return(io_gets(str, maxlen, fp));

    PG_setup_input(str, maxlen);

    if (SETJMP(io_avail) == ERR_FREE)

/* turn off interrupts until the next call to _PG_X_wind_fgets */
       {SC_catch_io_interrupts(FALSE);
        return(_PG_gcont.input_bf);};

/* don't catch interrupts until the SETJMP is successful */
    SC_catch_io_interrupts(TRUE);

    _PG_mess_flag = FALSE;
    _PG_whch      = PG_console_device->window;

/* put a line or two below where the typing will be done */
    oldval = GetControlValue(PG_console_device->scroll_bar);
    newval = (*PG_console_device->text)->nLines;
    SetControlMaximum(PG_console_device->scroll_bar,
              (*PG_console_device->text)->nLines);
    SetControlValue(PG_console_device->scroll_bar, newval);

    lineHght = (*PG_console_device->text)->lineHeight;
    Hght     = (oldval - newval - 2)*lineHght;
    TEPinScroll(0, Hght, PG_console_device->text);

/* update the display rect */
    _PG_update_display_rect(PG_console_device);
        
/* the event loop */
    while (TRUE)

/* main event tasks: */
       {SystemTask();
        _PG_actv = FrontWindow();        /* used often, avoid repeated calls */

/* things to do on each pass throught the event loop
 * when we are the active window:
 *        [1] Track the mouse, and set the cursor appropriately:
 *            (IBeam if in content region, THE_ARROW if outside)
 *        [2] TEIdle our textedit window, so the insertion bar blinks.
 */
        if (PG_console_device->window == _PG_actv)
           {GetMouse(&PG_console_device->mouse);
            TEIdle(PG_console_device->text);};
           
/* handle the next event */
        if (!PG_get_next_event(NULL)) 
           continue;

        type = PG_EVENT_TYPE(_PG_gcont.current_event);

        dev = PG_get_event_device(NULL);
        if (dev == NULL)
           {switch (type)
              {case EXPOSE_EVENT :
                    break;

               case UPDATE_EVENT :
		    break;

               case MOUSE_DOWN_EVENT :
		    devC = PG_console_device;
		    iob = PG_get_object_event(devC, &_PG_gcont.current_event);
		    if ((iob != NULL) && (iob->action != NULL))
		       (*iob->action)(iob, &_PG_gcont.current_event);
		    else if (devC->mouse_down_event_handler.fnc != NULL)
                       PG_CALL_HANDLER(devC->mouse_down_event_handler, devC, _PG_gcont.current_event);
		    break;

                case MOUSE_UP_EVENT :
		     break;

                case KEY_DOWN_EVENT :
		     break;

                case KEY_UP_EVENT :
		     break;

                default :
		     break;};

           continue;};

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
                if (b != b0)
                   {if (b0 != NULL)
                       _PG_draw_cursor(b0, FALSE);
                       _PG_draw_cursor(b, TRUE);
                       b0 = b;};}

            else if (b0 != NULL)
               {_PG_draw_cursor(b0, FALSE);
                b0 = NULL;}

            else if (dev->motion_event_handler.fnc != NULL)
               PG_CALL_HANDLER(dev->motion_event_handler, dev, _PG_gcont.current_event);}

        else if (dev->default_event_handler.fnc != NULL)
           {PG_CALL_HANDLER(dev->default_event_handler, dev, _PG_gcont.current_event);};

       if (_PG_mess_flag)
          return(str);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MOUSE_DOWN_HANDLER - handle a mouseDown event */

void _PG_qd_mouse_down_handler(PG_device *dev, PG_event *ev)
   {int inpart;
    EventRecord sys;

    sys = ev->sys_event;

    inpart = FindWindow(sys.where, &_PG_whch);
    switch (inpart) 
       {case inSysWindow  : SystemClick(&sys, _PG_whch);
                            break;
        case inMenuBar    : _PG_menu_command(dev, 
                                             MenuSelect(sys.where));
                            break;
        case inDrag       : DragWindow(_PG_whch, sys.where,
                                       &dev->drag_bound_shape);
                            break;
        case inContent    : _PG_mouse_content(dev, ev);
                            break;
        case inGrow       :
        case inZoomIn     : 
        case inZoomOut    : _PG_resize_window(dev, ev, inpart);
                            break;
        case inGoAway     : break;
        default           : break;};

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_QD_KEY_DOWN_HANDLER - handle a keyDown event */

void _PG_qd_key_down_handler(PG_device *dev, PG_event *ev)
   {char c;
    EventRecord sys;
    static count = 0;
    static char *ptr = NULL;

    sys = ev->sys_event;

    if (dev->window == _PG_actv)
       {c = sys.message & charCodeMask;
        if (sys.modifiers & cmdKey)
           if (c == '.')
              raise (SIGINT);
           else
              _PG_menu_command(dev, MenuKey(c));

        else if (c == '\3')
           raise (SIGINT);

        else if (dev->text != NULL)
           {if (ptr == NULL)
               ptr = _PG_gcont.input_bf;

            if ((c == '\010') || (c == '\177'))
               {count--;
                if (count > -1)
                   {ptr--;
                    *ptr = '\0';
                    TEKey(c, dev->text);};

                count = max(count, 0);}

            else if (++count < _PG_gcont.input_ncx)
               {*ptr++ = c;
                *ptr   = '\0';
                TEKey(c, dev->text);};

            if ((c == '\n') || (c == '\r'))
               {_PG_mess_flag = TRUE;

                *(ptr - 1) = '\r';
                *ptr       = '\0';
                TEKey(c, dev->text);

                count = 0;
                ptr   = NULL;};};};

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_QD_EXPOSE_HANDLER - handle an EXPOSE_EVENT */

void _PG_qd_expose_handler(PG_device *dev, PG_event *ev)
   {int inc, maxTop;
    EventRecord sys;

    sys = ev->sys_event;

    if ((WindowPtr) (sys.message) == dev->window) 
       {SetPort(dev->window);
        if (sys.modifiers & activeFlag)
           {if (dev->text != NULL)
               {DrawGrowIcon(dev->window);
                TEActivate(dev->text);
                _PG_text_update(dev, dev->text);
                inc = (dev->text_shape.bottom - dev->text_shape.top) / (*dev->text)->lineHeight;
                maxTop = (*dev->text)->nLines - inc;

/* turn scroll bar on if we have more lines of text than the window will hold */
                if (maxTop <= 0)
                   {maxTop = 0;
                    HiliteControl(dev->scroll_bar, INACTIVE);}
                else
                    HiliteControl(dev->scroll_bar, ACTIVE);}
               DrawControls(dev->window);
               DisableItem(dev->menu[editMenu], undoCommand);}
       else 
          {if (dev->text != NULL)
             {TEDeactivate(dev->text);
              HiliteControl(dev->scroll_bar, INACTIVE);}
           EnableItem(dev->menu[editMenu], undoCommand);};};
 
    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_QD_UPDATE_HANDLER - handle an UPDATE_EVENT */

void _PG_qd_update_handler(PG_device *dev, PG_event *ev)
   {

    if ((WindowPtr) (ev->sys_event.message) == dev->window)
       _PG_update_display_rect(dev);

    return;}

/*------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_QUERY_POINTER - query the pointer for location and button status */

void _PG_qd_query_pointer(PG_device *dev, int *ir, int *pb, int *pq)
   {int ret, btn, mod, i;
    Point pt;
    KeyMap pk;

    GetMouse(&pt);
    GET_KEYS(pk);
    ret = Button();

    ir[0] = pt.h;
    ir[1] = pt.v;

    PG_QUAD_FOUR_POINT(dev, ir);

/* map MAC buttons into PGS buttons */
    btn = 0;
    btn |= (ret)      ? MOUSE_LEFT   : 0;

    i = ((pk[3]) >> 3) & ~(~0 << 1);
    btn |= (i)        ? MOUSE_MIDDLE : 0;

    i = ((pk[3]) >> 4) & ~(~0 << 1);
    btn |= (i)        ? MOUSE_RIGHT  : 0;
   

/*    btn |= (pk[0x63]) ? MOUSE_MIDDLE : 0;
    btn |= (pk[0x64]) ? MOUSE_RIGHT  : 0;
    btn |= (pk[0x7b]) ? MOUSE_MIDDLE : 0;
    btn |= (pk[0x7c]) ? MOUSE_RIGHT  : 0;
*/
    
    mod = 0;

/* little MAC's */
#if 0
    mod |= (pk[56]) ? KEY_SHIFT : 0;
    mod |= (pk[55]) ? KEY_CNTL  : 0;
    mod |= (pk[58]) ? KEY_ALT   : 0;
#endif

    i = (pk[1]) & ~(~0 << 1);
    mod |= (i)  ? KEY_SHIFT : 0;

    i = ((pk[1]) >> 3) & ~(~0 << 1);
    mod |= (i)  ? KEY_CNTL  : 0;

    i = ((pk[1]) >> 2) & ~(~0 << 1);
    mod |= (i)  ? KEY_ALT   : 0;

    *pb = btn;
    *pq = mod;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_MOUSE_EVENT_INFO - return mouse event info for QD */

static void _PG_qd_mouse_event_info(PG_device *dev, PG_event *ev,
				    int *iev, PG_mouse_button *peb, int *peq)
   {int et, eq;
    EventRecord sys;

    sys = ev->sys_event;

/* location of mouse */
    iev[0] = sys.where.h;
    iev[1] = sys.where.v;

    PG_QUAD_FOUR_POINT(dev, iev);

/* which button */
    *peb = sys.what;

/* button modifiers */
    et  = sys.modifiers;
    eq  = (et & 0x0200) ? KEY_SHIFT : 0;
    eq |= (et & 0x0400) ? KEY_LOCK  : 0;
    eq |= (et & 0x1000) ? KEY_CNTL  : 0;
    eq |= (et & 0x0800) ? KEY_ALT   : 0;

    *peq = eq;

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_QD_KEY_EVENT_INFO - return keyboard event info for QD */

static void _PG_qd_key_event_info(PG_device *dev, PG_event *ev,
				  int *iev, char *bf, int n, int *peq)
   {int et;
    EventRecord sys;

    sys = (ev)->sys_event;

/* location */
    iev[0] = sys.where.h;
    iev[1] = sys.where.v;

    bf[0] = BitAnd(sys.message, charCodeMask);

/* key modifers */
    et  = sys.modifiers;
    eq  = (et & 0x0200) ? KEY_SHIFT : 0;
    eq |= (et & 0x0400) ? KEY_LOCK  : 0;
    eq |= (et & 0x1000) ? KEY_CNTL  : 0;
    eq |= (et & 0x0800) ? KEY_ALT   : 0;

    return;}

/*------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_QD_PUTS - put a string to the console window */

static void _PG_qd_puts(char *bf)
   {

    if (PG_console_device != NULL)
       {SetPort(PG_console_device->window);
        _PG_textdraw(PG_console_device, bf);};

    return;}

/*--------------------------------------------------------------------------*/

/*                            INTERNAL ROUTINES                             */

/*------------------------------------------------------------------------*/

/* _PG_QD_INIT_MAC - initialize the MacIntosh window environment for PACT
 *                 - applications
 */

static void _PG_qd_init_mac(void)
   {

    ONCE_SAFE(TRUE, NULL)

       InitGraf(&THE_PORT);
       InitFonts();
       FlushEvents(everyEvent, 0);
       InitWindows();
       InitMenus();
       TEInit();
       InitDialogs(NULL);
       InitCursor();

    END_SAFE;
    
    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_QD_OPEN_TEXT_WINDOW - set up a MacIntosh window for text type
 *                         - applications
 */

static void _PG_qd_open_text_window(PG_device *dev, int vis,
				    double xfrac, double yfrac,
				    double dxfrac, double dyfrac)
   {int idx, idy, ixul, iyul, ixlr, iylr;
    int font_size, n_colors;
    double ratio;
    Rect physical_screen_shape, *window_shape, port_shape;
    FontInfo font_data;
    PG_dev_geometry *g;

    if (dev == NULL)
       return;

    g = &dev->g;

    _PG_qd_init_mac();
 
    dev->type_index = TEXT_WINDOW_DEVICE;

/* initialize the menus */
    _PG_init_mac_menu(dev);

/* setup the text window */
    window_shape = &dev->window_shape;
    physical_screen_shape = SCREEN_BITS.bounds;

    PG_query_screen(dev, &idx, &idy, &n_colors);

    dev->ncolor           = n_colors;
    dev->absolute_n_color = n_colors;
        
/* NOTE: since window height is expressed as a fraction of screen WIDTH, in order
 *       to calculate window height in pixels, multiply by WIDTH, not height!
 */
    g->hwin[0] = idx*xfrac;
    g->hwin[1] = g->hwin[0] + idx*dxfrac;
    g->hwin[2] = idx*yfrac;
    g->hwin[3] = g->hwin[2] + idx*dyfrac;

    SET_PC_FROM_HWIN(g);

    ixul = xfrac*idx;
    iyul = yfrac*idx + 2*MENU_HEIGHT;
    ixlr = (xfrac + dxfrac)*idx;
    iylr = (yfrac + dyfrac)*idx + MENU_HEIGHT;
    SETRECT(window_shape, ixul, iyul, ixlr, iylr);

/* create window */
    NEW_WINDOW(dev->window, 0L, window_shape, dev->title, vis, zoomDocProc,
               (WindowPtr)-1, true, 0);

    SetPort(dev->window);

/* set up the limits within which this window may be dragged around */
    ixul = physical_screen_shape.left;
    iyul = physical_screen_shape.top + MENU_HEIGHT;
    ixlr = physical_screen_shape.right;
    iylr = physical_screen_shape.bottom;
    SETRECT(&(dev->drag_bound_shape), ixul, iyul, ixlr, iylr);
    
/* set up the scroll bars */
    port_shape = dev->window->portRect;
    SETRECT(&dev->scroll_bar_shape, port_shape.right - SCROLL_BAR_WIDTH + 1,
                                    port_shape.top,
                                    port_shape.right + 1,
                                    port_shape.bottom - SCROLL_BAR_WIDTH + 2);

    dev->scroll_bar = NEW_CONTROL(dev->window, &dev->scroll_bar_shape,
                                  "", TRUE, 1, 1, 2,
                                  scrollBarProc, 0);

/* Create a TextEdit record with the destRect and viewRect set
 * to window's portRect (offset by 4 pixels on the left and right
 * sides so that text doesn't jam up against the window frame).
 * Set the text destination and view rectangle to allow for the scroll bar
 */
    SETRECT(&dev->display_shape, port_shape.left,
                                 port_shape.top,
                                 port_shape.right - SCROLL_BAR_WIDTH + 1,
                                 port_shape.bottom);
    SETRECT(&dev->text_shape, port_shape.left,
                              port_shape.top,
                              port_shape.right - SCROLL_BAR_WIDTH + 1,
                              port_shape.bottom);
    InsetRect(&dev->text_shape, 4, 0);
    dev->text = TENew(&dev->text_shape, &dev->text_shape);
    TEAutoView(TRUE, dev->text);
    (*dev->text)->txSize = 12;

    ibeamHdl = GetCursor(iBeamCursor);           /* Grab this for use later */
    SetCursor(&THE_ARROW);
    
/* set up the font */
    GetFontInfo(&font_data);
    ratio = dxfrac*idx/(80*font_data.widMax);
    font_size = DEFAULT_FONT_SIZE;
    if (ratio < 1.0)
       {font_size = 0.5*(ratio*font_size + 0.5);
        font_size *= 2;
        font_size = max(MIN_FONT_SIZE, font_size);};
    TextSize(font_size);

/* update the display */
    _PG_update_display_rect(dev);
    _PG_adjust_scrollbar(dev);

    _PG_push_device(dev);

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_QD_OPEN_GRAPHICS_WINDOW - set up a MacIntosh window for graphical
 *                             - applications
 */

void _PG_qd_open_graphics_window(PG_device *dev,
				 double xfrac, double yfrac,
				 double dxfrac, double dyfrac)
   {int idx, idy, ixul, iyul, ixlr, iylr;
    int font_size, n_colors;
    double ratio;
    Rect physical_screen_shape, *window_shape;
    FontInfo font_data;
    PG_dev_geometry *g;
   
    if (dev == NULL)
       return;

    g = &dev->g;

    _PG_qd_init_mac();
 
    dev->type_index = GRAPHIC_WINDOW_DEVICE;

/* setup the graphics window */
    window_shape    = &dev->window_shape;
    physical_screen_shape = SCREEN_BITS.bounds;

    PG_query_screen(dev, &idx, &idy, &n_colors);

/* GOTCHA: The following pixel coordinate limits may be incorrect */
/* set device pixel coordinate limits */
    g->cnv[0] = INT_MIN + idx;
    g->cnv[1] = INT_MAX - idx;
    g->cpc[2] = INT_MIN + idy;
    g->cpc[3] = INT_MAX - idy;
    g->cpc[4] = INT_MIN;
    g->cpc[5] = INT_MAX;

    dev->ncolor           = n_colors;
    dev->absolute_n_color = n_colors;

/* NOTE: since window height is expressed as a fraction of screen WIDTH, in order
 *       to calculate window height in pixels, multiply by WIDTH, not height!
 */
    g->hwin[0] = idx*xfrac;
    g->hwin[1] = g->hwin[0] + idx*dxfrac;
    g->hwin[2] = idx*yfrac;
    g->hwin[3] = g->hwin[2] + idx*dyfrac;

    SET_PC_FROM_HWIN(g);

    ixul = xfrac*idx;
    iyul = yfrac*idx + 2*MENU_HEIGHT;
    ixlr = (xfrac + dxfrac)*idx;
    iylr = (yfrac + dyfrac)*idx + MENU_HEIGHT;
    SETRECT(window_shape, ixul, iyul, ixlr, iylr);

/* create window */

#ifdef MAC_COLOR
    if (dev->ncolor > 2)
       {NEW_C_WINDOW(dev->window, 0L, window_shape, dev->title, true, documentProc,
                     (WindowPtr)-1, true, 0);}
    else
       {NEW_WINDOW(dev->window, 0L, window_shape, dev->title, true, documentProc,
                   (WindowPtr)-1, true, 0);};
#else
    NEW_WINDOW(dev->window, 0L, window_shape, dev->title, true, documentProc,
               (WindowPtr)-1, true, 0);
#endif

    SetPort(dev->window);
    SetOrigin(0, 0);

/* set up the limits within which this window may be dragged around */
    ixul = physical_screen_shape.left;
    iyul = physical_screen_shape.top + MENU_HEIGHT;
    ixlr = physical_screen_shape.right;
    iylr = physical_screen_shape.bottom;
    SETRECT(&(dev->drag_bound_shape), ixul, iyul, ixlr, iylr);
    
    SetCursor(&THE_ARROW);

/* set up the font */
    GetFontInfo(&font_data);
    ratio = dxfrac*idx/(80*font_data.widMax);
    font_size = DEFAULT_FONT_SIZE;
    if (ratio < 1.0)
       {font_size = 0.5*(ratio*font_size + 0.5);
        font_size *= 2;
        font_size = max(MIN_FONT_SIZE, font_size);};
    TextSize(font_size);

    _PG_push_device(dev);

    return;}

/*------------------------------------------------------------------------*/

/*                         INTERNAL ROUTINES                              */

/*------------------------------------------------------------------------*/

/* _PG_SCROLL_DELAY - scroll by the given amount, wait for an
 *                  - exponentially decreasing time, and scroll again
 *                  - until the mouse button is released
 */

static void _PG_scroll_delay(PG_device *dev, int size, int count)
   {int n, inc, val;
   
    if (dev->text == NULL)
       return;

/* compute the size in lines and get the current scroll bar value */
    inc   = size / (*dev->text)->lineHeight;
    val   = GetControlValue(dev->scroll_bar);

/* scroll at an ever increasing rate and keep track of the scroll bar */
    do
      {TEPinScroll(0, size, dev->text);
       val -= inc;
       SetControlValue(dev->scroll_bar, val);
       if (count > 1)
          {for (n = count; n > 0; n--);
           count /= 2;};}
    while (StillDown());
    
/* redraw the scroll bar and leave */
    DrawControls(dev->window);

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_TEXT_UPDATE - do the update for text edit */

static void _PG_text_update(PG_device *dev, TEHandle txt)
   {Rect winrect;
    WindowPtr win;

    if (dev->title[0] != '\0')
       {win     = dev->window;
        winrect = win->portRect;
        InvalRect(&(dev->display_shape));
        BeginUpdate(win);
        TEUpdate(&winrect, txt);
        EndUpdate(win);}

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_TEXTDRAW - however you do it put some text out */

static void _PG_textdraw(PG_device *dev, char *s)
   {int n, inc, maxTop;
    char *t, *pt, c;
    static Boolean scrollBarOff = TRUE;

    if (dev->type_index == TEXT_WINDOW_DEVICE)
       {t = SC_dsnprintf(FALSE, "%s", s);
        for (pt = t; (c = *pt); pt++)
            if (c == '\n')
               *pt = '\r';

        n = strlen(t);   
        if (dev->text != NULL)
           {TESetSelect (-1, -1, dev->text);
            TEInsert(t, n, dev->text);

/* is it time to turn scroll bars on yet? */
            if (scrollBarOff)
               {inc    = (dev->text_shape.bottom - dev->text_shape.top) /
		         (*dev->text)->lineHeight;
                maxTop = (*dev->text)->nLines - inc;
                if (maxTop <= 0)
                   {maxTop = 0;
                    HiliteControl(dev->scroll_bar, INACTIVE);}
                else
                   {HiliteControl(dev->scroll_bar, ACTIVE);
                    scrollBarOff = FALSE;};};};}

    else if (dev->type_index == GRAPHIC_WINDOW_DEVICE)
       {DRAW_STRING(s);};
   
    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_ADJUST_SCROLLBAR - adjust scroll bar to current length of text */

static void _PG_adjust_scrollbar(PG_device *dev)
   {int inc, maxTop;

    inc = (dev->text_shape.bottom - dev->text_shape.top) / (*dev->text)->lineHeight;
    maxTop = (*dev->text)->nLines - inc;

/* turn scroll bar off if text is smaller than window */
/* make sure top line is in view before we turn scroll bars off! */
    if (maxTop <= 0)
       {maxTop = 0;
        TESetSelect(0,0,dev->text);
        TESelView(dev->text);
        HiliteControl(dev->scroll_bar, INACTIVE);}
    else
        HiliteControl(dev->scroll_bar, ACTIVE);

/* adjust range of scroll bar; put bottom line at bottom of screen */
/*  SetControlMaximum(dev->scroll_bar, maxTop);*/
    SetControlMinimum(dev->scroll_bar, inc);
    SetControlValue(dev->scroll_bar, (*dev->text)->selStart);
    TESetSelect (-1, -1, dev->text);

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_MOUSE_CONTENT - handle a mouseDown event in the content region
 *                   - of the window
 */

static void _PG_mouse_content(PG_device *dev, PG_event *ev)
   {int Contind, lineHght, pageHght, Hght, oldval, newval;
    ControlHandle whchCont;
    Point *whchPoint;

    if (_PG_whch != _PG_actv)
       SelectWindow(_PG_whch);

    else if (_PG_whch == dev->window) 
       {whchPoint = &(ev->sys_event.where);
        GlobalToLocal(whchPoint);
    
        if (dev->text != NULL)
           {Contind = FindControl(*whchPoint, dev->window, &whchCont);
            switch (Contind)
               {case kControlUpButtonPart   :
                     lineHght = (*dev->text)->lineHeight;
                     _PG_scroll_delay(dev, lineHght, 16384);
                     break;
                case kControlDownButtonPart :
                     lineHght = (*dev->text)->lineHeight;
                     _PG_scroll_delay(dev, -lineHght, 16384);
                     break;
                case kControlPageUpPart     :
                     pageHght = dev->text_shape.bottom - 
                                dev->text_shape.top;
                     _PG_scroll_delay(dev, pageHght, 16384);
                     break;
                case kControlPageDownPart   :
                     pageHght = dev->text_shape.bottom - 
                                dev->text_shape.top;
                     _PG_scroll_delay(dev, -pageHght, 16384);
                     break;
                case kControlIndicatorPart      :
                     lineHght = (*dev->text)->lineHeight;
                     oldval = GetControlValue(whchCont);
                     TrackControl(whchCont, *whchPoint, NULL);
/*                   TrackControl(whchCont, *whchPoint, _PG_nil); */
                     newval = GetControlValue(whchCont);
                     Hght = (oldval - newval)*lineHght;
                     TEPinScroll(0, Hght, dev->text);
                     break;
                default           :
                     TEClick(*whchPoint,
                             (ev->sys_event.modifiers & shiftKey) != 0,
                             dev->text);
                     break;};};};

    return;}
    
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_RESIZE_WINDOW - resize the window either by virtue of Zoom or Grow */

static void _PG_resize_window(PG_device *dev, PG_event *ev, int inpart)
   {int hr, hl, hlp, vt, vb, vbp;
    int width, height, dx, dy;
    long new_size;
    Rect port_shape, grow_limit, physical_screen_shape;

    SetPort(dev->window);

/* get the new size */
    width  = 0;
    height = 0;
    if (inpart == inGrow)
       {physical_screen_shape = SCREEN_BITS.bounds;
        dx = physical_screen_shape.right - physical_screen_shape.left;
        dy = physical_screen_shape.bottom -
             physical_screen_shape.top - MENU_HEIGHT;
        SETRECT(&grow_limit, WIDTH_MIN, HEIGHT_MIN, dx, dy);
        new_size = GrowWindow(dev->window, ev->sys_event.where, &grow_limit);
        width    = LoWord(new_size);
        height   = HiWord(new_size);};

    EraseRect(&(dev->window->portRect));
    if ((inpart == inZoomIn) || (inpart == inZoomOut))
       ZoomWindow(dev->window, inpart, TRUE);
    else
       SizeWindow(dev->window, width, height, TRUE);

/* get the new port shape information */
    port_shape = dev->window->portRect;
    hlp        = port_shape.left;
    hl         = port_shape.right - SCROLL_BAR_WIDTH + 1;
    vt         = port_shape.top;
    hr         = port_shape.right;
    vb         = port_shape.bottom;
    vbp        = vb - SCROLL_BAR_WIDTH + 1;

    SETRECT(&dev->display_shape, hlp, vt, hl, vb);

/* reset the text view and destination rectangles */
    if (dev->text != NULL)
       {SETRECT(&dev->text_shape, hlp, vt, hl, vb);
        InsetRect(&dev->text_shape, 4, 0);
        (*dev->text)->destRect = dev->text_shape;
        (*dev->text)->viewRect = dev->text_shape;
        TECalText(dev->text);
        _PG_text_update(dev, dev->text);

/* reset up the scroll bars */
        SETRECT(&dev->scroll_bar_shape, hl, vt, hr, vbp);
        MoveControl(dev->scroll_bar, hl, vt);
        SizeControl(dev->scroll_bar, hr-hl+1, vbp-vt+1);
        _PG_adjust_scrollbar(dev);};

    DrawControls(dev->window);
    DrawGrowIcon(dev->window);

    return;}
                            
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_INIT_MAC_MENU - set up the Apple, File, and Edit menus */

static void _PG_init_mac_menu(PG_device *dev)
   {MenuHandle *p;

/* Set up the desk accessories menu
 * then append the desk accessory names from the 'DRVR' resources.
 */
    dev->menu[appleMenu] = NewMenu(appleID, "\p\024");
    dev->menu[fileMenu]  = NewMenu(fileID, "\pFile");
    dev->menu[editMenu]  = NewMenu(editID, "\pEdit");
    dev->menu[fontMenu]  = NewMenu(fontID, "\pFonts");
    dev->menu[sizeMenu]  = NewMenu(sizeID, "\pSize");

    AppendMenu(dev->menu[appleMenu],
               "\pAbout...");
    AppendMenu(dev->menu[fileMenu],
               "\pQuit/Q");
    AppendMenu(dev->menu[editMenu],
               "\pUndo/Z;(-;(Cut/X;Copy/C;(Paste/V;(Clear");
    AppendMenu(dev->menu[sizeMenu],
               "\p6 Point;8 Point;10 Point;12 Point");
    
/* get the resources for the menus */
    AppendResMenu(dev->menu[appleMenu], (ResType) 'DRVR');
    AppendResMenu(dev->menu[fontMenu], (ResType) 'FONT');

/* Now insert all of the application menus in the menu bar */
    for (p = dev->menu; p < &dev->menu[menuCount]; ++p)
        InsertMenu(*p, 0);

    DrawMenuBar();

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_INFO_DIAL - display the information dialog from the DLOG resource
 *               - insert two static text items in the DLOG:
 *               -         author name
 *               -         source language
 *               - wait until the OK button is clicked before returning
 */
 
static void _PG_info_dial(void)
   {GrafPtr orig;
    DialogPtr dial;
    short type;
    Handle hand;
    Rect coord;
    short resp;

    GetPort(&orig);
    dial = GetNewDialog(aboutMeDLOG, NULL, (WindowPtr) -1);
    SetPort(dial);

    GetDialogItem(dial, versionItem, &type, &hand, &coord);
    SET_I_TEXT(hand, VERSION);

    do
       {ModalDialog(NULL, &resp);}
    while (resp != okButton);

    CloseDialog(dial);
    SetPort(orig);

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
                                       
/* _PG_BASH_SCRAP - add the scrap to the current message which Windgets
 *                - is preparing (fill this one in someday)
 */
 
static void _PG_bash_scrap(char **ps)
    {struct scrptype {long length; char *txt;} txScrap;

     return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_MENU_COMMAND - process menu commands */

static void _PG_menu_command(PG_device *dev, long mResult)
   {int cmenu, citem;
    unsigned char name[256];
    GrafPtr orig;
    Rect *Uprect;
    PFVoid func;

    citem = LOWORD(mResult);
    cmenu = HIWORD(mResult);        /* This is the resource ID */

/* switch on the current menu */
    switch (cmenu)
       {case appleID : if (citem == aboutMeCommand)
                          _PG_info_dial();
                       else
                          {GET_ITEM(dev->menu[appleMenu], citem, name);
                           GetPort(&orig);
                           OPEN_DESK_ACC((char *) name);
                           SetPort(orig);};
                       break;

        case fileID  : switch (citem) 
                          {case quitCommand : func = (PFVoid) PG_lookup_callback ("End");
                                              (*func) (PG_console_device, NULL);
                                              break;
                           default          : break;};
                       break;

/* SystemEdit will return FALSE if it's not a system window
 * Otherwise, handle Cut/Copy/Paste properly
 */
        case editID  : if ((citem <= clearCommand) && SystemEdit(citem-1)) 
                          {}
                       else
                          {switch (citem)
                             {case undoCommand  : break;         /* can't undo */
                              case cutCommand   :
                              case copyCommand  : if (citem == cutCommand)
                                                     TECut(dev->text);
                                                  else
                                                     TECopy(dev->text);
                                                  ZeroScrap();
                                                  TEToScrap();
                                                  break;
                              case pasteCommand : _PG_bash_scrap(&_PG_gcont.input_bf);
                                                  TEFromScrap();
                                                  TEPaste(dev->text);
                                                  break;
                              case clearCommand : TEDelete(dev->text);
                                                  break;
                              default           : break;};};
                       break;

        case fontID  : _PG_set_text_font_mac(dev, citem);
                       Uprect = &((*dev->text)->viewRect);
                       InvalRect(Uprect);
                       _PG_text_update(dev, dev->text);
                       _PG_adjust_scrollbar(dev);
                       break;

        case sizeID  : _PG_set_text_size_mac(dev, citem, TRUE);
                       Uprect = &((*dev->text)->viewRect);
                       InvalRect(Uprect);
                       TECalText(dev->text);
                       _PG_text_update(dev, dev->text);
                       _PG_adjust_scrollbar(dev);
                       break;

        default      : break;};

    HiliteMenu(0);
    DrawControls(dev->window);

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_SET_TEXT_FONT_MAC - set the font for the MAC */

void _PG_set_text_font_mac(PG_device *dev, int index)
   {short num;
    unsigned char name[256];

    if (dev->menu[fontMenu] != NULL)
       {GET_ITEM(dev->menu[fontMenu], index, name);
        PtoCstr(name);
        GET_FNUM((char *)name, &num);}
    else
       num = index;

    TextFont(num);
    if (dev->text != NULL)
       (*dev->text)->txFont = num;

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_SET_TEXT_SIZE_MAC - change the text size in MAC OS */

void _PG_set_text_size_mac(PG_device *dev, int size_index, int flag)
   {int cycle, place;
    float oldsize, height, ascent, scale;
    short newsize;
    FontInfo fInfo;

    if (flag)
       {cycle   = (int) POW(2.0, (double) ((size_index / 5) + 1));
        place   = (size_index % 5);
        newsize = (short) (cycle*(2 + place));}
    else
       newsize = size_index;

    if (dev->text != NULL)
       {oldsize = (float) (*dev->text)->txSize;
        height  = (float) (*dev->text)->lineHeight;
        ascent  = (float) (*dev->text)->fontAscent;

        TextSize(newsize);

        GetFontInfo(&fInfo);

        (*dev->text)->txSize     = newsize;
        (*dev->text)->lineHeight = fInfo.ascent + fInfo.descent + fInfo.leading;
        (*dev->text)->fontAscent = fInfo.ascent;};

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_UPDATE_DISPLAY_RECT - clear the display rect part of a TEXT device */

static void _PG_update_display_rect(PG_device *dev)
   {

    SetPort(dev->window);
    BeginUpdate(dev->window);
    InvalRect(&(dev->display_shape));
    DrawControls(dev->window);
    DrawGrowIcon(dev->window);
    EndUpdate(dev->window);

    if (dev->text != NULL)
       _PG_text_update(dev, dev->text);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_QD_DEVICE - do the device dependent device initialization
 *                    - for PG_make_device
 */

int PG_setup_qd_device(PG_device *d)
   {

    if (SC_strstri(d->name, "TEXT") != NULL)
       d->type_index = TEXT_WINDOW_DEVICE;
    else
       d->type_index = GRAPHIC_WINDOW_DEVICE;

    d->is_visible = TRUE;

    d->events_pending         = NULL;
    d->query_pointer          = _PG_qd_query_pointer;
    d->mouse_event_info       = _PG_qd_mouse_event_info;
    d->key_event_info         = _PG_qd_key_event_info;
    d->clear_page             = _PG_qd_clear_page;
    d->clear_window           = _PG_qd_clear_window;
    d->clear_viewport         = _PG_qd_clear_viewport;
    d->clear_region           = _PG_qd_clear_region;
    d->close_console          = _PG_qd_close_console;
    d->close_device           = _PG_qd_close_device;
    d->flush_events           = NULL;
    d->draw_dj_polyln_2       = _PG_qd_draw_disjoint_polyline_2;
    d->draw_curve             = _PG_qd_draw_curve;
    d->draw_to_abs            = _PG_qd_draw_to_abs;
    d->draw_to_rel            = _PG_qd_draw_to_rel;
    d->expose_device          = _PG_qd_expose_device;
    d->finish_plot            = _PG_qd_finish_plot;
    d->get_char               = NULL;
    d->get_image              = _PG_qd_get_image;
    d->get_text_ext           = _PG_qd_get_text_ext;
    d->ggets                  = (PFfgets) _PG_qd_wind_fgets;
    d->gputs                  = _PG_qd_puts;
    d->make_device_current    = _PG_qd_make_device_current;
    d->map_to_color_table     = _PG_qd_map_to_color_table;
    d->match_rgb_colors       = _PG_qd_match_rgb_colors;
    d->move_gr_abs            = _PG_qd_move_gr_abs;
    d->move_tx_abs            = _PG_qd_move_tx_abs;
    d->move_tx_rel            = _PG_qd_move_tx_rel;
    d->next_line              = _PG_qd_next_line;
    d->open_screen            = _PG_qd_open_screen;
    d->put_image              = _PG_qd_put_image;
    d->query_screen           = _PG_qd_query_screen;
    d->release_current_device = _PG_qd_release_current_device;
    d->set_clipping           = _PG_qd_set_clipping;
    d->set_char_line          = _PG_qd_set_char_line;
    d->set_char_path          = _PG_qd_set_char_path;
    d->set_char_precision     = _PG_qd_set_char_precision;
    d->set_char_size          = _PG_qd_set_char_size;
    d->set_char_space         = _PG_qd_set_char_space;
    d->set_char_up            = _PG_qd_set_char_up;
    d->set_fill_color         = _PG_qd_set_fill_color;
    d->set_font               = _PG_qd_set_font;
    d->set_line_color         = _PG_qd_set_line_color;
    d->set_line_style         = _PG_qd_set_line_style;
    d->set_line_width         = _PG_qd_set_line_width;
    d->set_logical_op         = _PG_qd_set_logical_op;
    d->set_text_color         = _PG_qd_set_text_color;
    d->shade_poly             = _PG_qd_shade_poly;
    d->fill_curve             = _PG_qd_fill_curve;
    d->update_vs              = _PG_qd_update_vs;
    d->write_text             = _PG_qd_write_text;
    
    d->expose_event_handler.fnc     = _PG_qd_expose_handler;
    d->update_event_handler.fnc     = _PG_qd_update_handler;
    d->mouse_down_event_handler.fnc = _PG_qd_mouse_down_handler;
    d->mouse_up_event_handler.fnc   = NULL;
    d->key_down_event_handler.fnc   = _PG_qd_key_down_handler;
    d->key_up_event_handler.fnc     = NULL;
    d->default_event_handler.fnc    = NULL;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QD_SETUP_CTRLS_GLB - setup global control structures */

void PG_qd_setup_ctrls_glb(void)
   {

    _PG_gcont.get_event    = _PG_qd_get_next_event;
    _PG_gcont.event_device = _PG_qd_get_event_device;
    _PG_gcont.open_console = _PG_qd_open_console;
    _PG_gcont.setup_window = PG_setup_qd_device;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

