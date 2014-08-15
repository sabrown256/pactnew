/*
 * GSDV_WIN32.C - PGS WIN32 routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
  
#include "scope_win32.h"

#define MAX_CONSOLE_LINES 500

SC_array
 *_PG_win32_point_list;

/* these next two are saved from parameters passed in to WinMain */

HINSTANCE
 _PG_hInstance;

int
 _PG_iCmdShow;

MSG
 _PG_msg;

/* from or for corresponding PR file */

int
 _PG_win32_set_font(PG_device *dev, char *face, char *style, int size);

void
 _PG_win32_draw_disjoint_polyline_2(PG_device *dev, double **r,
				    long n, int clip, int coord),
 _PG_win32_draw_curve(PG_device *dev, PG_curve *crv, int clip),
 _PG_win32_draw_to_abs(PG_device *dev, double x, double y),
 _PG_win32_draw_to_rel(PG_device *dev, double x, double y),
 _PG_win32_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p),
 _PG_win32_set_clipping(PG_device *dev, bool flag),
 _PG_win32_set_char_line(PG_device *dev, int n),
 _PG_win32_set_char_path(PG_device *dev, double x, double y),
 _PG_win32_set_char_precision(PG_device *dev, int p),
 _PG_win32_set_char_space(PG_device *dev, double s),
 _PG_win32_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x),
 _PG_win32_set_char_up(PG_device *dev, double x, double y),
 _PG_win32_set_fill_color(PG_device *dev, int color, int mapped),
 _PG_win32_set_line_color(PG_device *dev, int color, int mapped),
 _PG_win32_set_line_style(PG_device *dev, int style),
 _PG_win32_set_line_width(PG_device *dev, double width),
 _PG_win32_set_logical_op(PG_device *dev, PG_logical_operation lop),
 _PG_win32_set_text_color(PG_device *dev, int color, int mapped),
 _PG_win32_shade_poly(PG_device *dev, int nd, int n, double **r),
 _PG_win32_fill_curve(PG_device *dev, PG_curve *crv),
 _PG_win32_move_gr_abs(PG_device *dev, double x, double y),
 _PG_win32_move_tx_abs(PG_device *dev, double x, double y),
 _PG_win32_move_tx_rel(PG_device *dev, double x, double y),
 _PG_win32_get_image(PG_device *dev, unsigned char *bf, int ix, int iy,
		     int nx, int ny),
 _PG_win32_put_image(PG_device *dev, unsigned char *bf, int ix, int iy,
		     int nx, int ny),
 _PG_set_text_font_win32(PG_device *dev, int index),
 _PG_set_text_size_win32(PG_device *dev, int size_index, int flag);
 

/*--------------------------------------------------------------------------*/

/*                           INTERNAL ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PG_WIN32_INIT - initialize the WIN32 window environment for PACT
 *                 - applications
 */

static void _PG_win32_init(void)
   {

#if 0
    ONCE_SAFE(TRUE, NULL)

    END_SAFE;
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TEXTDRAW - however you do it put some text out */

static void _PG_textdraw(PG_device *dev, char *s)
   {int n, inc, maxTop;

    if (dev->type_index == TEXT_WINDOW_DEVICE)
       {PRINT(STDOUT, s);}

    else if (dev->type_index == GRAPHIC_WINDOW_DEVICE)
       {int len;
        double x[PG_SPACEDM], ix[PG_SPACEDM];
        HDC hdc;
   
        len  = strlen(s);
        x[0] = dev->tcur[0];
        x[1] = dev->tcur[1];

	PG_trans_point(dev, 2, WORLDC, x, PIXELC, ix);

        hdc = GetDC(dev->window);
        TextOut(hdc, ix[0], ix[1], s, len);
        ReleaseDC(dev->window, hdc);}
   
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WNDPROC - the window procedure */

LRESULT CALLBACK _PG_WndProc(HWND hwnd, UINT message,
			     WPARAM wParam, LPARAM lParam)
   {HDC hdc;
    LOGPEN logpen;
    LOGBRUSH logbrush;
    HPEN hPen;
    HBRUSH hBrush;

    switch (message)
       {case WM_CREATE  :

/* create a pen for later manipulation by the primitive routines.  */
/* this may be unnecessary, but the assumption is that the default */
/* pen is a stock object and should not be deleted.                */

             hdc  = GetDC(hwnd);

             hPen = GetCurrentObject(hdc, OBJ_PEN);
             GetObject(hPen, sizeof(LOGPEN), (LPVOID) &logpen);
             SelectObject(hdc, CreatePenIndirect(&logpen));

             hBrush = GetCurrentObject(hdc, OBJ_BRUSH);
             GetObject(hBrush, sizeof(LOGBRUSH), (LPVOID) &logbrush);
             SelectObject(hdc, CreateBrushIndirect(&logbrush));

             ReleaseDC(hwnd, hdc);
             return 0;}

    return(DefWindowProc(hwnd, message, wParam, lParam));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_CREATE_GWINDOW - create a graphics window
 *
 */

static HWND _PG_win32_create_gwindow(PG_device *dev)
   {int h, w;
    HWND hwnd;
    WNDCLASS  wndclass;
    PG_dev_geometry *g;
    static char szAppName[] = "PGS graphics window";

    wndclass.style          =  CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.lpfnWndProc    = _PG_WndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 0;
    wndclass.hInstance      = _PG_hInstance;  /* WinMain will store this in a global */
    wndclass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground  = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = szAppName;
    
    if (!RegisterClass (&wndclass))
       {PRINT(STDOUT, "Unable to create a graphics window--_PG_win32_create_gwindow\n");
        return((HWND) 0);}

    g = &dev->g;
    w = PG_window_width(dev);
    h = PG_window_height(dev);

    hwnd = CreateWindow(szAppName, dev->title,
                        WS_OVERLAPPEDWINDOW,
                        g->hwin[0], g->hwin[2], w, h,
                        NULL, NULL, _PG_hInstance, NULL);

    ShowWindow(hwnd, _PG_iCmdShow); /* have WinMain save _PG_iCmdShow in global */
    UpdateWindow(hwnd);
    
    return(hwnd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MOUSE_CONTENT - handle a mouseDown event in the content region
 *                   - of the window
 */

static void _PG_mouse_content(PG_device *dev, PG_event *ev)
   {
    return;}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RESIZE_WINDOW - resize the window either by virtue of Zoom or Grow */

static void _PG_resize_window(PG_device *dev, PG_event *ev, int inpart)
   {
    return;}
                            

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SET_TEXT_FONT_WIN32 - set the font for the WIN32 */

void _PG_set_text_font_win32(PG_device *dev, int index)
   {
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SET_TEXT_SIZE_WIN32 - change the text size in WIN32 OS */

void _PG_set_text_size_win32(PG_device *dev, int size_index, int flag)
   {
    return;}

/*--------------------------------------------------------------------------*/

/*                               PGS INTERNAL                               */

/*--------------------------------------------------------------------------*/

/* _PG_WIN32_QUERY_SCREEN - query some physical device characteristics */

static void _PG_win32_query_screen(PG_device *dev, int *pdx, int *pdy, int *pnc)
   {int dx, dy, nc;
    int nplanes, bitspixel;
    HDC hdc;

    if (dev->phys_width == -1)
       {hdc = GetDC(dev->window);
    
	dx        = GetDeviceCaps(hdc, HORZRES);
	dy        = GetDeviceCaps(hdc, VERTRES);
	nplanes   = GetDeviceCaps(hdc, PLANES);
	bitspixel = GetDeviceCaps(hdc, BITSPIXEL);
	nc        = 1 << (nplanes * bitspixel);

	ReleaseDC(dev->window, hdc);
        
	dev->phys_width    = dx;
	dev->phys_height   = dy;
	dev->phys_n_colors = nc;};

    *pdx = dev->phys_width;
    *pdy = dev->phys_height;
    *pnc = dev->phys_n_colors;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_MAP_TO_COLOR_TABLE - map the PGS palette to host
 *                              - color table values
 */

static void _PG_win32_map_to_color_table(PG_device *dev, PG_palette *pal)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_MATCH_RGB_COLORS - map the RGB references in the palette to
 *                            - valid colors from the root color table
 */

static void _PG_win32_match_rgb_colors(PG_device *dev, PG_palette *pal)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_CLOSE_DEVICE - close a device */
 
static void _PG_win32_close_device(PG_device *dev)
   {int i, j;

    DestroyWindow(dev->window);

    _PG_remove_device(dev);

#if 0
    if (_PG.n_devices == 0)
       SC_free_array(_PG_win32_point_list, NULL);
#else
    if (SC_array_get_n(_PG_devlst) == 0)
       SC_free_array(_PG_win32_point_list, NULL);
#endif

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_CLOSE_CONSOLE - close the console device */
 
static void _PG_win32_close_console(void)
   {int i, j;

    FreeConsole();

    _PG_remove_device(PG_gs.console);

    PG_gs.console = NULL;

/* connect I/O to standard functions */
    SC_set_put_line(io_printf_hook);
    SC_set_put_string(io_puts_hook);
    SC_set_get_line(io_gets_hook);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_GET_NEXT_EVENT - get the next event
 *                       - return TRUE iff successful
 */

static int _PG_win32_get_next_event(PG_event *ev)
   {int ret = TRUE;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_FIND_POINT - return the device containing the given point */

static PG_device *_PG_win32_find_point(POINT pt)
   {

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_GET_EVENT_DEVICE - return the device in which the given event
 *                         - occurred
 */

static PG_device *_PG_win32_get_event_device(PG_event *ev)
   {

    return(NULL);}

/*--------------------------------------------------------------------------*/

/*                           SCREEN STATE ROUTINES                          */

/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_CLEAR_WINDOW - clear the screen */
 
static void _PG_win32_clear_window(PG_device *dev)
   {int color;
    unsigned int width, height;
    HDC hdc;
    PG_dev_geometry *g;

    if (dev != NULL)
       {g = &dev->g;

	width  = PG_window_width(dev);
	height = PG_window_height(dev);

	hdc = GetDC(dev->window);
	Rectangle(hdc, 0, 0, width, height);
	ReleaseDC(dev->window, hdc);            

	PG_release_current_device(dev);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_CLEAR_VIEWPORT - clear the viewport */
 
static void _PG_win32_clear_viewport(PG_device *dev)
   {PG_dev_geometry *g;

    g = &dev->g;

    PG_clear_region(dev, 2, NORMC, g->ndc[0], 1);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_CLEAR_REGION - clear the rectangular region specified in NDC */
 
static void _PG_win32_clear_region(PG_device *dev, int nd, PG_coord_sys cs,
				   double *bx, int pad)
   {double pc[PG_BOXSZ];
    HDC hdc;

    if (dev != NULL)
       {PG_trans_box(dev, 2, cs, bx, PIXELC, pc);

	hdc = GetDC(dev->window);
	Rectangle(hdc, pc[0], pc[2], pc[1], pc[3]);
	ReleaseDC(dev->window, hdc);    

	PG_release_current_device(dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_UPDATE_VS - update the view surface for the given device */
 
static void _PG_win32_update_vs(PG_device *dev)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_FINISH_PLOT - do what's necessary to finish up a graphical image
 *                       - and get the device updated with the image
 *                       - after this function nothing more can be added to
 *                       - the image
 */
 
static void _PG_win32_finish_plot(PG_device *dev)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_CLEAR_PAGE - clear the current page
 *                      - and go to the line i on the screen
 */
 
static void _PG_win32_clear_page(PG_device *dev, int i)
   {

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_EXPOSE_DEVICE - make this device the topmost one */
 
static void _PG_win32_expose_device(PG_device *dev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_MAKE_DEVICE_CURRENT - make this device the current one for drawing
 *                               - purposes
 */
 
static void _PG_win32_make_device_current(PG_device *dev)
   {

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_RELEASE_CURRENT_DEVICE - make no device current */
 
static void _PG_win32_release_current_device(PG_device *dev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_WRITE_TEXT - write out text to the appropriate device */
 
static void _PG_win32_write_text(PG_device *dev, FILE *fp, char *s)
   {

    _PG_textdraw(dev, s);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_NEXT_LINE - do a controlled newline */
 
static void _PG_win32_next_line(PG_device *dev)
   {

    _PG_textdraw(dev, "\n");

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_PROCESS_MESSAGES - process pending messages */

static void _PG_win32_process_messages(void)
   {

    while (PeekMessage(&_PG_msg, NULL, 0, 0, PM_REMOVE))
       {TranslateMessage(&_PG_msg);
        DispatchMessage(&_PG_msg);}
    
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_WIND_FGETS - get and return a string from the window environment
 *                      - process pending messages, then get input from the
 *                      - console window
 */
 
static char *_PG_win32_wind_fgets(char *str, int maxlen, FILE *fp)
   {

/* process pending messages, then get input from the console */
    _PG_win32_process_messages();

    str = SC_fgets(str, maxlen, stdin);
    
    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MOUSE_DOWN_HANDLER - handle a mouseDown event */

static void _PG_win32_mouse_down_handler(PG_device *dev, PG_event *ev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_KEY_DOWN_HANDLER - handle a keyDown event */

static void _PG_win32_key_down_handler(PG_device *dev, PG_event *ev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_EXPOSE_HANDLER - handle an EXPOSE_EVENT */

static void _PG_win32_expose_handler(PG_device *dev, PG_event *ev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_UPDATE_HANDLER - handle an UPDATE_EVENT */

static void _PG_win32_update_handler(PG_device *dev, PG_event *ev)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_QUERY_POINTER - query the pointer for location and button status */

static void _PG_win32_query_pointer(PG_device *dev, int *ir,
				    int *pb, int *pq)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_WIN32_GRAB_POINTER - take over the pointer */

static int _PG_win32_grab_pointer(PG_device *dev)
   {int rv;

    rv = FALSE;

    return(rv);}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_WIN32_RELEASE_POINTER - release the pointer */

static int _PG_win32_release_pointer(PG_device *dev)
   {int rv;

    rv = FALSE;

    return(rv);}

/*------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_MOUSE_EVENT_INFO - return mouse event info for WIN32 */

static void _PG_win32_mouse_event_info(PG_device *dev, PG_event *ev,
				       int *iev, PG_mouse_button *peb,
				       int *peq)
   {

    return;}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

/* _PG_WIN32_KEY_EVENT_INFO - return keyboard event info for WIN32 */

static void _PG_win32_key_event_info(PG_device *dev, PG_event *ev,
				     int *iev, char *bf, int n, int *peq)
   {

    return;}

/*------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_PUTS - put a string to the console window */

static void _PG_win32_puts(char *bf)
   {

/* should we process pending messages before putting the string? */
    io_puts(bf, stdout);

    return;}

/*--------------------------------------------------------------------------*/

/*                            INTERNAL ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PG_WIN32_OPEN_TEXT_WINDOW - set up a window for text type
 *                            - applications
 */

static void _PG_win32_open_text_window(PG_device *dev, int vis,
				       double xfrac, double yfrac,
				       double dxfrac, double dyfrac)
   {int hConHandle;
    long lStdHandle;
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    FILE *fp;

    _PG_win32_init();
 
    dev->type_index = TEXT_WINDOW_DEVICE;

/* setup the text window */
    AllocConsole();

/* set the screen buffer to be big enough to let us scroll text */
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
                               &coninfo);
    coninfo.dwSize.Y = MAX_CONSOLE_LINES;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
                               coninfo.dwSize);

/* redirect unbuffered STDOUT to the console */
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);

/* redirect unbuffered STDIN to the console */
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "r");
    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);

/* redirect unbuffered STDERR to the console */
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stderr = *fp;
    setvbuf(stderr, NULL, _IONBF, 0);

    dev->ncolor           = 2;
    dev->absolute_n_color = 2;
        
    _PG_push_device(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_OPEN_GRAPHICS_WINDOW - set up a window for graphical
 *                                - applications
 */

static void _PG_win32_open_graphics_window(PG_device *dev,
					   double xfrac, double yfrac,
					   double dxfrac, double dyfrac)
   {int ixul, iyul, ixlr, iylr;
    int font_size, n_colors;
    int dx[PG_SPACEDM];
    double ratio;
    PG_dev_geometry *g;
   
    if (dev == NULL)
       return;

    g = &dev->g;

    _PG_win32_init();
 
    dev->type_index = GRAPHIC_WINDOW_DEVICE;
    dev->quadrant   = QUAD_FOUR;

    PG_query_screen_n(dev, dx, &n_colors);
    if ((dx[0] == 0) && (dx[1] == 0) &&
        (n_colors == 0))
       return;

/* GOTCHA: The following pixel coordinate limits may be incorrect
 * set device pixel coordinate limits
 */
    g->cnv[0] = INT_MIN + dx[0];
    g->cnv[1] = INT_MAX - dx[0];
    g->cpc[2] = INT_MIN + dx[1];
    g->cpc[3] = INT_MAX - dx[1];
    g->cpc[4] = INT_MIN;
    g->cpc[5] = INT_MAX;

    dev->ncolor           = n_colors;
    dev->absolute_n_color = n_colors;

/* NOTE: since window height is expressed as a fraction of screen WIDTH, in order
 *       to calculate window height in pixels, multiply by WIDTH, not height!
 */

    if ((xfrac == 0.0) && (yfrac == 0.0))
       {xfrac = 0.5;
        yfrac = 0.1;};

    if ((dxfrac == 0.0) && (dyfrac == 0.0))
       {dxfrac = 0.5;
        dyfrac = 0.5;};

    g->hwin[0] = dx[0]*xfrac;
    g->hwin[1] = g->hwin[0] + dx[0]*dxfrac;
    g->hwin[2] = dx[0]*yfrac;
    g->hwin[3] = g->hwin[2] + dx[0]*dyfrac;

    SET_PC_FROM_HWIN(g);

/* create window */
    dev->window = _PG_win32_create_gwindow(dev);

/* set up the font */
#if 0
 /* Mac coding: */

    GetFontInfo(&font_data);
    ratio = dxfrac*dx[0]/(80*font_data.widMax);
    font_size = DEFAULT_FONT_SIZE;
    if (ratio < 1.0)
       {font_size = 0.5*(ratio*font_size + 0.5);
        font_size *= 2;
        font_size = max(MIN_FONT_SIZE, font_size);};
    TextSize(font_size);
#endif

    _PG_push_device(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_OPEN_SCREEN - initialize a "window" on a display screen */
 
static PG_device *_PG_win32_open_screen(PG_device *dev,
					double xf, double yf,
					double dxf, double dyf)
   {int bck_color, for_color;
    int dx[PG_SPACEDM];
    double intensity;
    unsigned int Lightest, Light, Light_Gray, Dark_Gray, Dark, Darkest;
    PG_font_family *ff;

    if (dev == NULL)
       return(NULL);

    PG_setup_markers();

    _PG_win32_init();
 
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
       _PG_win32_open_text_window(dev, TRUE, xf, yf, dxf, dyf);
    else
       _PG_win32_open_graphics_window(dev, xf, yf, dxf, dyf);

    _PG_win23_point_list = CMAKE_ARRAY(POINT, NULL, 0);

    intensity = dev->max_intensity*MAXPIX;
    mono      = (dev->ncolor == 2);
    PG_color_map(dev, mono, FALSE, BLACK, WHITE);
    if (dev->background_color_white == TRUE)
       {Lightest   = 0;
        Light      = intensity;
        Light_Gray = 0.8*intensity;
        Dark_Gray  = 0.5*intensity;
        Dark       = 0;
        Darkest    = intensity;}
    else
       {Lightest   = intensity;
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

/*
    The X device has some coding to set the window size in pixels here, is
    it necessary here?
*/
 
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

    PG_fset_font(dev, "helvetica", "medium", 10);

/* put in the default palettes */
    PG_setup_standard_palettes(dev, 64,
                   Light, Dark,
                   Light_Gray, Dark_Gray,
                   Lightest, Darkest);

#if 0
/* Mac coding */
Set the background color????
    colormap = dev->current_palette->true_colormap;
    RGBBackColor(&colormap[bck_color]);
    EraseRect(&dev->window->portRect);

#endif

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_OPEN_CONS - open up a special window to be used as a console or
 *                     - terminal window for systems which can't do this for
 *                     - themselves (i.e. MacIntosh) or the SINGLE SCREEN
 *                     - devices
 */
 
static int _PG_win32_open_cons(char *title, char *type, int bckgr, int vis,
			       double xf, double yf, double dxf, double dyf)
   {

    PG_setup_markers();

    PG_gs.console = PG_make_device("TEXT", type, title);
    _PG_win32_open_text_window(PG_gs.console, vis, xf, yf, dxf, dyf);

    SC_set_put_line(PG_wind_fprintf);
    SC_set_put_string(PG_wind_fputs);
    SC_set_get_line(_PG_win32_wind_fgets);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_OPEN_CONSOLE - open up a special window to be used as a console or
 *                        - terminal window for systems which can't do this for
 *                        - themselves (i.e. MacIntosh) or the SINGLE SCREEN
 *                        - devices
 */
 
static int _PG_win32_open_console(char *title, char *type, int bckgr,
				  double xf, double yf,
				  double dxf, double dyf)
   {int rv;

    rv = _PG_win32_open_cons(title, type, bckgr, TRUE, xf, yf, dxf, dyf);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_SETUP_WIN32_DEVICE - do the device dependent device initialization
 *                       - for PG_make_device
 */

int PG_setup_win32_device(PG_device *d)
   {

    if (SC_strstri(d->name, "TEXT") != NULL)
       d->type_index = TEXT_WINDOW_DEVICE;
    else
       d->type_index = GRAPHIC_WINDOW_DEVICE;

    d->is_visible = TRUE;

    d->events_pending         = NULL;
    d->query_pointer          = _PG_win32_query_pointer;
    d->grab_pointer           = _PG_win32_grab_pointer;
    d->release_pointer        = _PG_win32_release_pointer;
    d->mouse_event_info       = _PG_win32_mouse_event_info;
    d->key_event_info         = _PG_win32_key_event_info;
    d->clear_page             = _PG_win32_clear_page;
    d->clear_window           = _PG_win32_clear_window;
    d->clear_viewport         = _PG_win32_clear_viewport;
    d->clear_region           = _PG_win32_clear_region;
    d->close_console          = _PG_win32_close_console;
    d->close_device           = _PG_win32_close_device;
    d->flush_events           = NULL;
    d->draw_dj_polyln_2       = _PG_win32_draw_disjoint_polyline_2;
    d->draw_curve             = _PG_win32_draw_curve;
    d->draw_to_abs            = _PG_win32_draw_to_abs;
    d->draw_to_rel            = _PG_win32_draw_to_rel;
    d->expose_device          = _PG_win32_expose_device;
    d->finish_plot            = _PG_win32_finish_plot;
    d->get_char               = NULL;
    d->get_image              = _PG_win32_get_image;
    d->get_text_ext           = _PG_win32_get_text_ext;
    d->ggetc                   = io_getc;
    d->ggets                  = (PFfgets) _PG_win32_wind_fgets;
    d->gputs                  = _PG_win32_puts;
    d->make_device_current    = _PG_win32_make_device_current;
    d->map_to_color_table     = _PG_win32_map_to_color_table;
    d->match_rgb_colors       = _PG_win32_match_rgb_colors;
    d->move_gr_abs            = _PG_win32_move_gr_abs;
    d->move_tx_abs            = _PG_win32_move_tx_abs;
    d->move_tx_rel            = _PG_win32_move_tx_rel;
    d->next_line              = _PG_win32_next_line;
    d->open_screen            = _PG_win32_open_screen;
    d->put_image              = _PG_win32_put_image;
    d->query_screen           = _PG_win32_query_screen;
    d->release_current_device = _PG_win32_release_current_device;
    d->set_clipping           = _PG_win32_set_clipping;
    d->set_char_line          = _PG_win32_set_char_line;
    d->set_char_path          = _PG_win32_set_char_path;
    d->set_char_precision     = _PG_win32_set_char_precision;
    d->set_char_size          = _PG_win32_set_char_size;
    d->set_char_space         = _PG_win32_set_char_space;
    d->set_char_up            = _PG_win32_set_char_up;
    d->set_fill_color         = _PG_win32_set_fill_color;
    d->set_font               = _PG_win32_set_font;
    d->set_line_color         = _PG_win32_set_line_color;
    d->set_line_style         = _PG_win32_set_line_style;
    d->set_line_width         = _PG_win32_set_line_width;
    d->set_logical_op         = _PG_win32_set_logical_op;
    d->set_text_color         = _PG_win32_set_text_color;
    d->shade_poly             = _PG_win32_shade_poly;
    d->fill_curve             = _PG_win32_fill_curve;
    d->update_vs              = _PG_win32_update_vs;
    d->write_text             = _PG_win32_write_text;
    
    d->expose_event_handler.fnc     = _PG_win32_expose_handler;
    d->update_event_handler.fnc     = _PG_win32_update_handler;
    d->mouse_down_event_handler.fnc = _PG_win32_mouse_down_handler;
    d->mouse_up_event_handler.fnc   = NULL;
    d->key_down_event_handler.fnc   = _PG_win32_key_down_handler;
    d->key_up_event_handler.fnc     = NULL;
    d->default_event_handler.fnc    = NULL;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WIN32_SETUP_CTRLS_GLB - setup global control structures */

void PG_win32_setup_ctrls_glb(void)
   {

    _PG_gcont.get_event    = _PG_win32_get_next_event;
    _PG_gcont.event_device = _PG_win32_get_event_device;
    _PG_gcont.open_console = _PG_win32_open_console;
    _PG_gcont.setup_window = PG_setup_win32_device;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

