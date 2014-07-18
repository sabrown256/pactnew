/*
 * GSPR_WIN32.C - PGS WIN32 primitive routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "scope_win32.h"

extern SC_array
 *_PG_win32_point_list;

void
 _PG_set_text_font_win32(PG_device *dev, int index),
 _PG_set_text_size_win32(PG_device *dev, int size_index, int flag);
 
 static void _PG_win32_draw_polyline(PG_device *dev, double *x, double *y, int n);

/*--------------------------------------------------------------------------*/

/*                         STATE QUERY ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PG_WIN32_GET_TEXT_EXT - return the text extent in CS
 *                        - of the given string
 */

void _PG_win32_get_text_ext(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p)
   {HDC hdc;
    TEXTMETRIC tm;
    double x, y;
    int ix, iy;

    hdc = GetDC(dev->window);
    GetTextMetrics(hdc, &tm);
    ReleaseDC(dev->window, hdc);

    p[0] = strlen(s) * tm.tmAveCharWidth;
    p[1] = tm.tmHeight + tm.tmExternalLeading;
    p[2] = 0.0;

    PG_trans_interval(dev, nd, NORMC, p, cs, p);
        
    return;}

/*--------------------------------------------------------------------------*/

/*                          STATE CHANGE ROUTINES                           */

/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_SET_LOGICAL_OP - set the logical operation */
 
void _PG_win32_set_logical_op(PG_device *dev, PG_logical_operation lop)
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
 
/* _PG_WIN32_SET_LINE_STYLE - set the line style */
 
void _PG_win32_set_line_style(PG_device *dev, int style)
   {HDC hdc;
    LOGPEN logpen;
    HPEN   hPen;

    dev->line_style = style;
    
    hdc  = GetDC(dev->window);
    hPen = GetCurrentObject(hdc, OBJ_PEN);
    GetObject(hPen, sizeof(LOGPEN), (LPVOID) &logpen);

    switch (style)
       {case LINE_DASHED :
             logpen.lopnStyle = PS_DASH;
             SelectObject(hdc, CreatePenIndirect(&logpen));
             DeleteObject(hPen);
             break; 

        case LINE_DOTTED :
             logpen.lopnStyle = PS_DOT;
             SelectObject(hdc, CreatePenIndirect(&logpen));
             DeleteObject(hPen);
             break;

        case LINE_DOTDASHED :
             logpen.lopnStyle = PS_DASHDOT;
             SelectObject(hdc, CreatePenIndirect(&logpen));
             DeleteObject(hPen);
             break;

        case LINE_SOLID :
        default         :
             logpen.lopnStyle = PS_SOLID;
             SelectObject(hdc, CreatePenIndirect(&logpen));
             DeleteObject(hPen);
             break;};

    ReleaseDC(dev->window, hdc);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_SET_LINE_WIDTH - set the line width */
 
void _PG_win32_set_line_width(PG_device *dev, double width)
   {int lwd;
    HDC hdc;
    LOGPEN logpen;
    HPEN   hPen;

    dev->line_width = width;
    lwd = max(1, width);
    
    hdc  = GetDC(dev->window);
    hPen = GetCurrentObject(hdc, OBJ_PEN);
    GetObject(hPen, sizeof(LOGPEN), (LPVOID) &logpen);

    logpen.lopnWidth.x = lwd;

    SelectObject(hdc, CreatePenIndirect(&logpen));
    DeleteObject(hPen);
    ReleaseDC(dev->window, hdc);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_SET_LINE_COLOR - set the line color */
 
void _PG_win32_set_line_color(PG_device *dev, int color, int mapped)
   {HDC hdc;
    LOGPEN logpen;
    HPEN   hPen;
    RGB_color_map *cm;
    unsigned char red, green, blue;

    dev->line_color = color;

    if (mapped)
       color = _PG_trans_color(dev, color);

    hdc  = GetDC(dev->window);
    hPen = GetCurrentObject(hdc, OBJ_PEN);
    GetObject(hPen, sizeof(LOGPEN), (LPVOID) &logpen);

    cm    = dev->current_palette->true_colormap;
    red   = cm[color].red;
    green = cm[color].green;
    blue  = cm[color].blue;

    logpen.lopnColor = RGB(red, green, blue);

    SelectObject(hdc, CreatePenIndirect(&logpen));
    DeleteObject(hPen);
    ReleaseDC(dev->window, hdc);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_SET_TEXT_COLOR - set the color of the text */
 
void _PG_win32_set_text_color(PG_device *dev, int color, int mapped)
   {HDC hdc;
    RGB_color_map *cm;
    unsigned char red, green, blue;

    dev->text_color = color;

    if (mapped)
       color = _PG_trans_color(dev, color);

    hdc  = GetDC(dev->window);

    cm    = dev->current_palette->true_colormap;
    red   = cm[color].red;
    green = cm[color].green;
    blue  = cm[color].blue;

    SetTextColor(hdc, RGB(red, green, blue));

    ReleaseDC(dev->window, hdc);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_SET_FILL_COLOR - sets current fill color */

void _PG_win32_set_fill_color(PG_device *dev, int color, int mapped)
   {HDC hdc;
    HBRUSH hBrush;
    RGB_color_map *cm;
    unsigned char red, green, blue;

    hdc  = GetDC(dev->window);

    cm    = dev->current_palette->true_colormap;    
    red   = cm[color].red;
    green = cm[color].green;
    blue  = cm[color].blue;

    hBrush = CreateSolidBrush(RGB(red, green, blue));
    DeleteObject(SelectObject(hdc, hBrush));

    ReleaseDC(dev->window, hdc);
    
    dev->fill_color = color;
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_SET_FONT - set the character font */

int _PG_win32_set_font(PG_device *dev, char *face, char *style, int size)
   {int rv;

    rv = TRUE;

/* needs more */

#if 0
    if (!PG_setup_font(dev, face, style, size, &font_name, &nfont, &nstyle))
       rv = FALSE;
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_SET_CHAR_SIZE - set the character size in CS */

void _PG_win32_set_char_size(PG_device *dev, int nd, PG_coord_sys cs, double *x)
   {int size;
    double p[PG_SPACEDM];

    PG_trans_point(dev, nd, cs, x, NORMC, p);

    dev->char_height_s = p[0];
    dev->char_width_s  = p[1];

    size = dev->char_width_s*PG_window_width(dev);
    if (size > 4)
       _PG_set_text_size_win32(dev, size, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_SET_CHAR_PATH - set the direction along which text will be written
 *                         - defaults to (1, 0)
 */

void _PG_win32_set_char_path(PG_device *dev, double x, double y)
   {

    dev->char_path[0] = x;
    dev->char_path[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_SET_CHAR_PRECISION - set the character precision
 *                              - fast and fixed size or
 *                              - slow and flexible
 */

void _PG_win32_set_char_precision(PG_device *dev, int p)
   {

    dev->char_precision = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_SET_CHAR_SPACE - set the space between characters */

void _PG_win32_set_char_space(PG_device *dev, double s)
   {

    dev->char_space = (double) s;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_SET_CHAR_UP - set the direction which is up for individual
 *                       - characters
 *                       - defaults to (0, 1)
 */

void _PG_win32_set_char_up(PG_device *dev, double x, double y)
   {

    dev->char_up[0] = x;
    dev->char_up[1] = y;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_SET_CHAR_LINE - set the number characters per line */
 
void _PG_win32_set_char_line(PG_device *dev, int n)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_SET_CLIPPING - set clipping
 *                        - flag = FALSE  -->  clipping off
 *                        - flag = TRUE   -->  clipping on
 */

void _PG_win32_set_clipping(PG_device *dev, bool flag)
   {int pc[PG_BOXSZ];
    HDC hdc;
    HRGN hrgn;

    hrgn = NULL;
    if (flag == TRUE)
       {_PG_find_clip_region(dev, pc, flag, FALSE);

	PG_QUAD_FOUR_BOX(dev, pc);

        hrgn = CreateRectRgn(pc[0], pc[2], pc[1], pc[3]);};

/* if turning clipping on pass in a clip region
 * else pass in a NULL clip region
 */
    hdc = GetDC(dev->window);
    SelectClipRgn(hdc, hrgn);
    ReleaseDC(dev->window, hdc);

    if (flag == TRUE)
        DeleteObject(hrgn);

    dev->clipping = flag;

    return;}
 
/*--------------------------------------------------------------------------*/

/*                          MOVE AND DRAW ROUTINES                          */

/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_MOVE_GR_ABS - move the current graphics cursor position to the
 *                       - given absolute coordinates in WC
 */
 
void _PG_win32_move_gr_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];
    HDC hdc;

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	hdc = GetDC(dev->window);
	MoveToEx(hdc, p[0], p[1], NULL);
	ReleaseDC(dev->window, hdc);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_MOVE_TX_ABS - move the current text cursor position to the
 *                       - given coordinates in WC
 */
 
void _PG_win32_move_tx_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];
    HDC hdc;

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	hdc = GetDC(dev->window);
	MoveToEx(hdc, p[0], p[1], NULL);
	ReleaseDC(dev->window, hdc);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_MOVE_TX_REL - move the current text cursor position to the
 *                       - given relative coordinates in WC
 */
 
void _PG_win32_move_tx_rel(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];
    HDC hdc;

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_tx_rel(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	hdc = GetDC(dev->window);
	MoveToEx(hdc, p[0], p[1], NULL);
	ReleaseDC(dev->window, hdc);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_DRAW_TO_ABS - draw a line from current position to
 *                       - absolute position (x, y)
 *                       - in WC
 */
 
void _PG_win32_draw_to_abs(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];
    HDC hdc;

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_abs(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);
 
	PG_QUAD_FOUR_POINT(dev, p);

	hdc = GetDC(dev->window);
	LineTo(hdc, p[0], p[1]);
	ReleaseDC(dev->window, hdc);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_DRAW_TO_REL - draw a line from current position to
 *                       - relative position (x, y)
 *                       - in WC
 */
 
void _PG_win32_draw_to_rel(PG_device *dev, double x, double y)
   {double p[PG_SPACEDM];
    HDC hdc;

    if (dev != NULL)
       {p[0] = x;
	p[1] = y;

	_PG_move_gr_rel(dev, p);

	PG_trans_point(dev, 2, WORLDC, p, PIXELC, p);

	PG_QUAD_FOUR_POINT(dev, p);

	hdc = GetDC(dev->window);
	LineTo(hdc, p[0], p[1]);
	ReleaseDC(dev->window, hdc);};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_DRAW_CURVE - draw a PG_curve */
 
void _PG_win32_draw_curve(PG_device *dev, PG_curve *crv, int clip)
   {int i, n, xo, yo, ix, iy;
    int p[PG_SPACEDM];
    int *x, *y;
    HDC hdc;

    n  = crv->n;
    x  = crv->x;
    y  = crv->y;
    xo = crv->x_origin;
    yo = crv->y_origin;

    hdc = GetDC(dev->window);

    p[0] = x[0] + xo;
    p[1] = y[0] + yo;

    PG_QUAD_FOUR_POINT(dev, p);
    MoveToEx(hdc, p[0], p[1], NULL);

    for (i = 1; i < n; i++)
        {p[0] = x[i] + xo;
         p[1] = y[i] + yo;

	 PG_QUAD_FOUR_POINT(dev, p);
         LineTo(hdc, p[0], p[1]);};

    ReleaseDC(dev->window, hdc);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_DRAW_DISJOINT_POLYLINE_2 - draws disjoint two dimensional
 *                                    - line segments specified in CS
 */

void _PG_win32_draw_disjoint_polyline_2(PG_device *dev, double **r,
					long n, int flag, PG_coord_sys cs)
   {int i;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    double *px, *py;

    px = r[0];
    py = r[1];

    for (i = 0; i < n; i++)
        {x1[0] = *px++;
	 x1[1] = *py++;
	 x2[0] = *px++;
	 x2[1] = *py++;
	 PG_draw_line_n(dev, 2, cs, x1, x2, dev->clipping);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_SHADE_POLY - polygon shading routine */

void _PG_win32_shade_poly(PG_device *dev, int nd, int n, double **r)
   {HDC hdc;
    int i;
    double x[PG_SPACEDM];
    POINT p, *points;

    if (nd == 3)
       _PG_rst_shade_poly(dev, nd, n, r);

    else

/* assume the points are given in counter-clockwise order */
       {for (i = 0; i < n; i++)
	    {x[0] = r[0][i];
	     x[1] = r[1][i];
       
	     PG_log_point(dev, 2, x);
	     PG_trans_point(dev, 2, WORLDC, x, PIXELC, x);

	     PG_QUAD_FOUR_POINT(dev, x);

	     p.x = x[0];
	     p.y = x[1];
	     SC_array_push(_PG_win32_point_list, &p);};

	points = SC_array_array(_PG_win32_point_list);

	SC_array_set_n(_PG_win32_point_list, 0);

	hdc = GetDC(dev->window);

	Polygon(hdc, points, n);

	ReleaseDC(dev->window, hdc);

	CFREE(points);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_WIN32_FILL_CURVE - fill a closed PG_curve */

void _PG_win32_fill_curve(PG_device *dev, PG_curve *crv)
   {int i, n;
    int ir[PG_SPACEDM], xo[PG_SPACEDM];
    int *x[PG_SPACEDM];
    POINT p, *points;
    HDC hdc;

    n     = crv->n;
    x[0]  = crv->x;
    x[1]  = crv->y;
    xo[0] = crv->x_origin;
    xo[1] = crv->y_origin;

/* assume the points are given in counter-clockwise order */
    for (i = 0; i < n; i++)
        {ir[0] = x[0][i] + xo[0];
	 ir[1] = x[1][i] + xo[1];
	 PG_QUAD_FOUR_POINT(dev, ir);

	 p.x = ir[0];
	 p.y = ir[1];

	 SC_array_push(_PG_win32_point_list, &p);};

    points = SC_array_array(_PG_win32_point_list);

    SC_array_set_n(_PG_win32_point_list, 0);

    hdc = GetDC(dev->window);

    Polygon(hdc, points, n);

    ReleaseDC(dev->window, hdc);    

    CFREE(points);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_PUT_IMAGE - put the image on the screen
 *                     - the image buffer may be overwritten by the pseudo
 *                     - color mapping if it is needed!!
 */

void _PG_win32_put_image(PG_device *dev, unsigned char *bf,
			 int ix, int iy, int nx, int ny)
   {

    return;}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WIN32_GET_IMAGE - get the image on the screen into
 *                  - the image buffer
 */

void _PG_win32_get_image(PG_device *dev, unsigned char *bf,
			 int ix, int iy, int nx, int ny)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/















