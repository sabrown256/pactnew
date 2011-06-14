/*
 * GSACC.C - struct accessors for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_BUFFER_SIZE - get the default I/O buffer size
 *
 * #bind PG_fget_buffer_size fortran() scheme()
 */

int64_t PG_fget_buffer_size(void)
   {int64_t rv;
    
    rv = _PG.buffer_size;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_BUFFER_SIZE - set the default I/O buffer size to SZ
 *                     - return the old value
 *                     - -1 turns off default buffering optimization
 *                     - which happens on file open
 *
 * #bind PG_fset_buffer_size fortran() scheme()
 */

int64_t PG_fset_buffer_size(int64_t sz)
   {int64_t rv;
    
    rv = _PG.buffer_size;
    
    _PG.buffer_size = sz;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CLEAR_MODE - get the high level clear mode
 *
 * #bind PG_fget_clear_mode fortran() scheme()
 */

int PG_fget_clear_mode(void)
   {int i;

    PG_get_attrs_glb(TRUE, "hl-clear-mode", &i, NULL);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CLEAR_MODE - set the high level clear mode
 *
 * #bind PG_fset_clear_mode fortran() scheme()
 */

int PG_fset_clear_mode(int i)
   {

    PG_set_attrs_glb(TRUE, "hl-clear-mode", i, NULL);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CLIPPING - get the device clipping flag
 *
 * #bind PG_fget_clipping fortran() scheme()
 */

int PG_fget_clipping(PG_device *dev)
   {int rv;
 
    rv = (dev == NULL) ? FALSE : dev->clipping;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CLIPPING - set the device clipping flag
 *
 * #bind PG_fset_clipping fortran() scheme()
 */

int PG_fset_clipping(PG_device *dev, int flag)
   {int rv;

    rv = flag;

    if (dev != NULL)
       {if (dev->set_clipping != NULL)
           (*dev->set_clipping)(dev, flag);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CHAR_PATH - get the device char_path
 *                   - defaults to (1, 0)
 *
 * #bind PG_fget_char_path fortran() scheme()
 */

void PG_fget_char_path(PG_device *dev, double *x)
   {

    if (dev != NULL)
       {x[0] = dev->char_path[0];
	x[1] = dev->char_path[1];}
    else
       {x[0] = 1.0;
	x[1] = 0.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CHAR_PATH - set the device char_path
 *
 * #bind PG_fset_char_path fortran() scheme()
 */

void PG_fset_char_path(PG_device *dev, double *x)
   {

    if (dev != NULL)
       {if (dev->set_char_path != NULL)
           (*dev->set_char_path)(dev, x[0], x[1]);}

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CHAR_UP - inquire about the direction which constitutes up for
 *                 - the characters
 *                 - defaults to (0, 1)
 *
 * #bind PG_fget_char_up fortran() scheme()
 */

void PG_fget_char_up(PG_device *dev, double *x)
   {

    if (dev != NULL)
       {x[0] = dev->char_up[0];
	x[1] = dev->char_up[1];}
    else
       {x[0] = 0.0;
	x[1] = 1.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CHAR_UP - set the direction which constitutes up for
 *                 - the characters
 *
 * #bind PG_fset_char_up fortran() scheme()
 */

void PG_fset_char_up(PG_device *dev, double *x)
   {

    if (dev != NULL)
       {if (dev->set_char_up != NULL)
           (*dev->set_char_up)(dev, x[0], x[1]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CHAR_PRECISION - return the device character precision
 *                        - defaults to 1
 *
 * #bind PG_fget_char_precision fortran() scheme()
 */

int PG_fget_char_precision(PG_device *dev)
   {int rv;

    if (dev != NULL)
       rv = dev->char_precision;
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CHAR_PRECISION - set the device character precision
 *
 * #bind PG_fset_char_precision fortran() scheme()
 */

int PG_fset_char_precision(PG_device *dev, int p)
   {

    if (dev != NULL)
       {if (dev->set_char_precision != NULL)
           (*dev->set_char_precision)(dev, p);};

    return(p);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CHAR_SIZE_N - return the character size in CS
 *
 * #bind PG_fget_char_size_n fortran() scheme()
 */

void PG_fget_char_size_n(PG_device *dev, int nd, PG_coord_sys cs, double *p)
   {

    if (dev != NULL)
       {p[0] = dev->char_height_s;
	p[1] = dev->char_width_s;}
    else
       {p[0] = 0.0;
	p[1] = 0.0;};

    PG_trans_point(dev, nd, NORMC, p, cs, p);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CHAR_SIZE_N - return the character size in CS
 *
 * #bind PG_fset_char_size_n fortran() scheme()
 */

void PG_fset_char_size_n(PG_device *dev, int nd, PG_coord_sys cs, double *p)
   {

    if (dev != NULL)
       {if (dev->set_char_size != NULL)
           (*dev->set_char_size)(dev, nd, cs, p);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_CHAR_SPACE - inquire about the space
 *                    - to be used between characters
 *                    - defaults to 0.0
 *
 * #bind PG_fget_char_space fortran() scheme()
 */

double PG_fget_char_space(PG_device *dev)
   {double d;

    if (dev != NULL)
       d = dev->char_space;
    else
       d = 0.0;

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_CHAR_SPACE - set the space
 *                    - to be used between characters
 *
 * #bind PG_fset_char_space fortran() scheme()
 */

double PG_fset_char_space(PG_device *dev, double d)
   {

    if (dev != NULL)
       {if (dev->set_char_space != NULL)
           (*dev->set_char_space)(dev, d);};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_FILL_BOUND - return the device draw_fill_bound flag
 *                    - defaults to FALSE
 *
 * #bind PG_fget_fill_bound fortran() scheme()
 */

int PG_fget_fill_bound(PG_device *dev)
   {int rv;

    if (dev != NULL)
       rv = dev->draw_fill_bound;
    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_FILL_BOUND - set the device draw_fill_bound flag
 *
 * #bind PG_fset_fill_bound fortran() scheme()
 */

int PG_fset_fill_bound(PG_device *dev, int v)
   {

    if (dev != NULL)
       dev->draw_fill_bound = v;

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_FINISH_STATE - return the device finished flag
 *                      - defaults to TRUE
 *
 * #bind PG_fget_finish_state fortran() scheme()
 */

int PG_fget_finish_state(PG_device *dev)
   {int rv;

    if (dev != NULL)
       rv = dev->finished;
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_FINISH_STATE - set the device finished flag
 *
 * #bind PG_fset_finish_state fortran() scheme()
 */

int PG_fset_finish_state(PG_device *dev, int v)
   {

    if (dev != NULL)
       dev->finished = v;

    return(v);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_FONT - return the current font info
 *
 * #bind PG_fget_font fortran() scheme()
 */

void PG_fget_font(PG_device *dev, char **of, char **ost, int *osz)
   {

    *of  = CSTRSAVE(dev->type_face);
    *ost = CSTRSAVE(dev->type_style);
    *osz = dev->type_size;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_FONT - set the current font info
 *
 * #bind PG_fset_font fortran() scheme()
 */

void PG_fset_font(PG_device *dev, char *face, char *style, int sz)
   {

    if (dev != NULL)
       {if (dev->set_font != NULL)
           (*dev->set_font)(dev, face, style, sz);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_IDENTIFIER - return the identifier of graph G
 *
 * #bind PG_fget_identifier fortran() scheme()
 */

int PG_fget_identifier(PG_graph *g)
   {int rv;

    if (g != NULL)
       rv = g->identifier;
    else
       rv = 'A';

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_IDENTIFIER - set the identifier of graph G
 *                    - return the old value
 *
 * #bind PG_fset_identifier fortran() scheme()
 */

int PG_fset_identifier(PG_graph *g, int id)
   {int rv;

    if (g != NULL)
       {rv = g->identifier;
	g->identifier = id;}
    else
       rv = 'A';
       
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LOGICAL_OP - return the device logical operation flag
 *                    - default is GS_COPY
 *
 * #bind PG_fget_logical_op fortran() scheme()
 */

PG_logical_operation PG_fget_logical_op(PG_device *dev)
   {PG_logical_operation lop;

    if (dev != NULL)
       lop = dev->logical_op;
    else
       lop = GS_COPY;

    return(lop);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LOGICAL_OP - set the device logical operation flag
 *                    - return the old value
 *
 * #bind PG_fset_logical_op fortran() scheme()
 */

PG_logical_operation PG_fset_logical_op(PG_device *dev,
					PG_logical_operation lop)
   {PG_logical_operation rv;

    if ((dev != NULL) && (dev->set_logical_op != NULL))
       (*dev->set_logical_op)(dev, lop);
    else
       rv = GS_COPY;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_STYLE - return the device line style
 *                    - default is LINE_SOLID
 *
 * #bind PG_fget_line_style fortran() scheme()
 */

int PG_fget_line_style(PG_device *dev)
   {int st;

    if (dev != NULL)
       st = dev->line_style;
    else
       st = LINE_SOLID;

    return(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_STYLE - set the device line style
 *                    - return the old value
 *
 * #bind PG_fset_line_style fortran() scheme()
 */

int PG_fset_line_style(PG_device *dev, int st)
   {int rv;

    if ((dev != NULL) && (dev->set_line_style != NULL))
       {rv = dev->line_style;
	(*dev->set_line_style)(dev, st);}
    else
       rv = LINE_SOLID;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_WIDTH - return the device line width
 *                    - default is 0.1
 *
 * #bind PG_fget_line_width fortran() scheme()
 */

double PG_fget_line_width(PG_device *dev)
   {double wd;

    if (dev != NULL)
       wd = dev->line_width;
    else
       wd = 0.1;

    return(wd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_WIDTH - set the device line width
 *                    - return the old value
 *
 * #bind PG_fset_line_width fortran() scheme()
 */

double PG_fset_line_width(PG_device *dev, double wd)
   {double owd;

    PG_get_attrs_glb(TRUE, "line-width", &owd, NULL);
    if (wd <= 0.0)
       wd = owd;

    if ((dev != NULL) && (dev->set_line_width != NULL))
       {owd = dev->line_width;
	(*dev->set_line_width)(dev, wd);};

    return(owd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MARKER_ORIENTATION - return the device marker orientation
 *                            - default is 0.0
 *
 * #bind PG_fget_marker_orientation fortran() scheme()
 */

double PG_fget_marker_orientation(PG_device *dev)
   {double rv;

    if (dev != NULL)
       rv = dev->marker_orientation;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MARKER_ORIENTATION - set the device marker orientation
 *                            - return the old value
 *
 * #bind PG_fset_marker_orientation fortran() scheme()
 */

double PG_fset_marker_orientation(PG_device *dev, double a)
   {double rv;

    if (dev != NULL)
       {rv = dev->marker_orientation;
	dev->marker_orientation = a;}
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MARKER_SCALE - return the device marker scale
 *                      - default is 0.0
 *
 * #bind PG_fget_marker_scale fortran() scheme()
 */

double PG_fget_marker_scale(PG_device *dev)
   {double rv;

    if (dev != NULL)
       rv = dev->marker_scale;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MARKER_SCALE - set the device marker scale
 *                      - return the old value
 *
 * #bind PG_fset_marker_scale fortran() scheme()
 */

double PG_fset_marker_scale(PG_device *dev, double s)
   {double rv;

    if (dev != NULL)
       {rv = dev->marker_scale;
	dev->marker_scale = max(s, 0.0);}
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MAX_INTENSITY - return the device maximum intensity
 *                       - default is 0.0
 *
 * #bind PG_fget_max_intensity fortran() scheme()
 */

double PG_fget_max_intensity(PG_device *dev)
   {double rv;

    if (dev != NULL)
       rv = dev->max_intensity;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MAX_INTENSITY - set the device maximum intensity
 *                       - return the old value
 *
 * #bind PG_fset_max_intensity fortran() scheme()
 */

double PG_fset_max_intensity(PG_device *dev, double i)
   {double rv;

    if (dev != NULL)
       {rv = dev->max_intensity;
	dev->max_intensity = min(i, 1.0);}
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MAX_RED_INTENSITY - return the device maximum red intensity
 *                           - default is 0.0
 *
 * #bind PG_fget_max_red_intensity fortran() scheme()
 */

double PG_fget_max_red_intensity(PG_device *dev)
   {double rv;

    if (dev != NULL)
       rv = dev->max_red_intensity;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MAX_RED_INTENSITY - set the device maximum red intensity
 *                           - return the old value
 *
 * #bind PG_fset_max_red_intensity fortran() scheme()
 */

double PG_fset_max_red_intensity(PG_device *dev, double i)
   {double rv;

    if (dev != NULL)
       {rv = dev->max_red_intensity;
	dev->max_red_intensity = min(i, 1.0);}
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MAX_GREEN_INTENSITY - return the device maximum green intensity
 *                             - default is 0.0
 *
 * #bind PG_fget_max_green_intensity fortran() scheme()
 */

double PG_fget_max_green_intensity(PG_device *dev)
   {double rv;

    if (dev != NULL)
       rv = dev->max_green_intensity;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MAX_GREEN_INTENSITY - set the device maximum green intensity
 *                             - return the old value
 *
 * #bind PG_fset_max_green_intensity fortran() scheme()
 */

double PG_fset_max_green_intensity(PG_device *dev, double i)
   {double rv;

    if (dev != NULL)
       {rv = dev->max_green_intensity;
	dev->max_green_intensity = min(i, 1.0);}
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_MAX_BLUE_INTENSITY - return the device maximum blue intensity
 *                            - default is 0.0
 *
 * #bind PG_fget_max_blue_intensity fortran() scheme()
 */

double PG_fget_max_blue_intensity(PG_device *dev)
   {double rv;

    if (dev != NULL)
       rv = dev->max_blue_intensity;
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_MAX_BLUE_INTENSITY - set the device maximum blue intensity
 *                       - return the old value
 *
 * #bind PG_fset_max_blue_intensity fortran() scheme()
 */

double PG_fset_max_blue_intensity(PG_device *dev, double i)
   {double rv;

    if (dev != NULL)
       {rv = dev->max_blue_intensity;
	dev->max_blue_intensity = min(i, 1.0);}
    else
       rv = 0.0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_PIXMAP_FLAG - return the device pixmap flag
 *                     - default is 0
 *
 * #bind PG_fget_pixmap_flag fortran() scheme()
 */

int PG_fget_pixmap_flag(PG_device *dev)
   {int fl;

    if (dev != NULL)
       fl = dev->use_pixmap;
    else
       fl = 0;

    return(fl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_PIXMAP_FLAG - set the device pixmap flag
 *                     - return the old value
 *
 * #bind PG_fset_pixmap_flag fortran() scheme()
 */

int PG_fset_pixmap_flag(PG_device *dev, int fl)
   {int rv;

    if (dev != NULL)
       {rv = dev->use_pixmap;
	dev->use_pixmap = fl;}
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_PS_DOTS_INCH - return the device dots per inch
 *                      - default is 0
 *
 * #bind PG_fget_ps_dots_inch fortran() scheme()
 */

double PG_fget_ps_dots_inch(void)
   {double dpi;

    PG_get_attrs_glb(TRUE, "ps-dots-inch", &dpi, NULL);

    return(dpi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_PS_DOTS_INCH - set the device dots per inch
 *                     - return the old value
 *
 * #bind PG_fset_ps_dots_inch fortran() scheme()
 */

double PG_fset_ps_dots_inch(double dpi)
   {

    PG_set_attrs_glb(TRUE, "ps-dots-inch", dpi, NULL);

    return(dpi);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_RENDER_INFO - get the graph render_info
 *                     - defaults to NULL
 *
 * #bind PG_fget_render_info fortran() scheme()
 */

pcons *PG_fget_render_info(PG_graph *g)
   {pcons *rv;

    rv = NULL;
    if ((g != NULL) && (g->info_type != NULL))
       {if (strcmp(g->info_type, SC_PCONS_P_S) == 0)
	   rv = (pcons *) g->info;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_RENDER_INFO - set the graph render_info
 *
 * #bind PG_fset_render_info fortran() scheme()
 */

pcons *PG_fset_render_info(PG_graph *g, pcons *a)
   {pcons *rv;

    if (g != NULL)
       {rv = g->info;
	g->info = a;}
    else
       rv = NULL;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_USE_PIXMAP - get the high level clear mode
 *
 * #bind PG_fget_use_pixmap fortran() scheme()
 */

int PG_fget_use_pixmap(void)
   {int i;

    PG_get_attrs_glb(TRUE, "use-pixmap", &i, NULL);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_USE_PIXMAP - set the high level clear mode
 *
 * #bind PG_fset_use_pixmap fortran() scheme()
 */

int PG_fset_use_pixmap(int i)
   {

    PG_set_attrs_glb(TRUE, "use-pixmap", i, NULL);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_RES_SCALE_FACTOR - get the device resolution scale factor
 *                          - defaults to 1
 *
 * #bind PG_fget_res_scale_factor fortran() scheme()
 */

int PG_fget_res_scale_factor(PG_device *dev)
   {int rv;

    if (dev != NULL)
       rv = dev->resolution_scale_factor;
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_RES_SCALE_FACTOR - set the device resolution scale factor
 *
 * #bind PG_fset_res_scale_factor fortran() scheme()
 */

int PG_fset_res_scale_factor(PG_device *dev, int s)
   {int rv;

    if (dev != NULL)
       {rv = dev->resolution_scale_factor;
	dev->resolution_scale_factor = s;}
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_BORDER_WIDTH - get the device border width
 *                      - defaults to 1
 *
 * #bind PG_fget_border_width fortran() scheme()
 */

int PG_fget_border_width(PG_device *dev)
   {int rv;

    if (dev != NULL)
       rv = dev->border_width;
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_BORDER_WIDTH - set the device border width
 *
 * #bind PG_fset_border_width fortran() scheme()
 */

int PG_fset_border_width(PG_device *dev, int w)
   {int rv;

    if (dev != NULL)
       {rv = dev->border_width;
	dev->border_width = w;}
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_VIEWPORT_POS - return the current viewport position
 *
 * #bind PG_fget_viewport_pos fortran() scheme()
 */

void PG_fget_viewport_pos(PG_device *dev, double *x)
   {

    if (dev != NULL)
       {x[0] = dev->view_x[0];
	x[1] = dev->view_x[2];}
    else
       {x[0] = 0.0;
	x[1] = 0.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_VIEWPORT_POS - set the current viewport position
 *
 * #bind PG_fset_viewport_pos fortran() scheme()
 */

void PG_fset_viewport_pos(PG_device *dev, double *x)
   {

    if (dev != NULL)
       {dev->view_x[0] = x[0];
	dev->view_x[2] = x[1];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_VIEWPORT_SHAPE - return the current viewport shape
 *
 * #bind PG_fget_viewport_shape fortran() scheme()
 */

void PG_fget_viewport_shape(PG_device *dev, double *dx, double *pa)
   {

    if (dev != NULL)
       {dx[0] = dev->view_x[1] - dev->view_x[0];
	dx[1] = dev->view_x[3] - dev->view_x[2];
        *pa   = dev->view_aspect;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_VIEWPORT_SHAPE - set the current viewport shape
 *
 * #bind PG_fset_viewport_shape fortran() scheme()
 */

void PG_fset_viewport_shape(PG_device *dev, double *dx, double asp)
   {

    if (dev != NULL)
       {dev->view_x[1]   = dev->view_x[0] + dx[0];
        dev->view_x[3]   = dev->view_x[2] + dx[1];
        dev->view_aspect = asp;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_LINE_COLOR - inquire about the current line color
 *
 * #bind PG_fget_line_color fortran() scheme()
 */

int PG_fget_line_color(PG_device *dev)
   {int rv;

    rv = dev->line_color;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_LINE_COLOR - set the current line color
 *
 * #bind PG_fset_line_color fortran() scheme()
 */

int PG_fset_line_color(PG_device *dev, int clr, int mapped)
   {int rv;

    if ((dev != NULL) && (dev->set_line_color != NULL))
       {rv = dev->line_color;
	(*dev->set_line_color)(dev, clr, mapped);}
    else
       PG_get_attrs_glb(TRUE, "line-color", &rv, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_TEXT_COLOR - inquire about the current text color
 *
 * #bind PG_fget_text_color fortran() scheme()
 */

int PG_fget_text_color(PG_device *dev)
   {int rv;

    rv = dev->text_color;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_TEXT_COLOR - set the current text color
 *
 * #bind PG_fset_text_color fortran() scheme()
 */

int PG_fset_text_color(PG_device *dev, int clr, int mapped)
   {int rv;

    if ((dev != NULL) && (dev->set_text_color != NULL))
       {rv = dev->text_color;
	(*dev->set_text_color)(dev, clr, mapped);}
    else
       PG_get_attrs_glb(TRUE, "text-color", &rv, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FGET_FILL_COLOR - inquire about the current fill color
 *
 * #bind PG_fget_fill_color fortran() scheme()
 */

int PG_fget_fill_color(PG_device *dev)
   {int rv;

    rv = dev->fill_color;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FSET_FILL_COLOR - set the current fill color
 *
 * #bind PG_fset_fill_color fortran() scheme()
 */

int PG_fset_fill_color(PG_device *dev, int clr, int mapped)
   {int rv;

    if ((dev != NULL) && (dev->set_fill_color != NULL))
       {rv = dev->fill_color;
	(*dev->set_fill_color)(dev, clr, mapped);}
    else
       PG_get_attrs_glb(TRUE, "fill-color", &rv, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_WHITE_BACKGROUND - set the device background color white flag
 *                     - return the old value
 *
 * #bind PG_white_background fortran() scheme()
 */

int PG_white_background(PG_device *dev, int t)
   {int rv;

    if (dev != NULL)
       {rv = dev->background_color_white;
	dev->background_color_white = t;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_GRID - set the device grid flag
 *              - return the old value
 *
 * #bind PG_turn_grid fortran() scheme()
 */

int PG_turn_grid(PG_device *dev, int t)
   {int rv;

    if (dev != NULL)
       {rv = dev->grid;
	dev->grid = t;}
    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_DATA_ID - set the device data_id flag
 *                 - return the old value
 *
 * #bind PG_turn_data_id fortran() scheme()
 */

int PG_turn_data_id(PG_device *dev, int t)
   {int rv;

    if (dev != NULL)
       {rv = dev->data_id;
	dev->data_id = t;}
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_SCATTER - set the device scatter flag
 *                 - return the old value
 *
 * #bind PG_turn_scatter fortran() scheme()
 */

int PG_turn_scatter(PG_device *dev, int t)
   {int rv;

    if (dev != NULL)
       {rv = dev->scatter;
	dev->scatter = t;}
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_AUTODOMAIN - set the device autodomain flag
 *                    - return the old value
 *
 * #bind PG_turn_autodomain fortran() scheme()
 */

int PG_turn_autodomain(PG_device *dev, int t)
   {int rv;

    if (dev != NULL)
       {rv = dev->autodomain;
	dev->autodomain = t;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_AUTORANGE - set the device autorange flag
 *                   - return the old value
 *
 * #bind PG_turn_autorange fortran() scheme()
 */

int PG_turn_autorange(PG_device *dev, int t)
   {int rv;

    if (dev != NULL)
       {rv = dev->autorange;
	dev->autorange = t;}
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_TURN_AUTOPLOT - set the device autoplot flag
 *                  - return the old value
 *
 * #bind PG_turn_autoplot fortran() scheme()
 */

int PG_turn_autoplot(PG_device *dev, int t)
   {int rv;

    if (dev != NULL)
       {rv = dev->autoplot;
	dev->autoplot = t;}
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_N_EVENTS_PENDING - return the number of events pending on device
 *
 * #bind PG_n_events_pending fortran() scheme()
 */

int PG_n_events_pending(PG_device *dev)
   {int rv;

    if (dev != NULL)
       rv = (*(dev->events_pending))(dev);
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_EXPOSE_EVENT - handle an expose event
 *
 * #bind PG_handle_expose_event fortran() scheme()
 */

void PG_handle_expose_event(PG_device *dev, PG_event *ev)
   {

    if ((dev != NULL) && (dev->expose_event_handler.fnc != NULL))
       (*(dev->expose_event_handler.fnc))(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_UPDATE_EVENT - handle an update event
 *
 * #bind PG_handle_update_event fortran() scheme()
 */

void PG_handle_update_event(PG_device *dev, PG_event *ev)
   {

    if ((dev != NULL) && (dev->update_event_handler.fnc != NULL))
       (*(dev->update_event_handler.fnc))(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_MOUSE_DOWN_EVENT - handle an mouse_down event
 *
 * #bind PG_handle_mouse_down_event fortran() scheme()
 */

void PG_handle_mouse_down_event(PG_device *dev, PG_event *ev)
   {

    if ((dev != NULL) && (dev->mouse_down_event_handler.fnc != NULL))
       (*(dev->mouse_down_event_handler.fnc))(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_MOUSE_UP_EVENT - handle an mouse_up event
 *
 * #bind PG_handle_mouse_up_event fortran() scheme()
 */

void PG_handle_mouse_up_event(PG_device *dev, PG_event *ev)
   {

    if ((dev != NULL) && (dev->mouse_up_event_handler.fnc != NULL))
       (*(dev->mouse_up_event_handler.fnc))(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_KEY_DOWN_EVENT - handle an key_down event
 *
 * #bind PG_handle_key_down_event fortran() scheme()
 */

void PG_handle_key_down_event(PG_device *dev, PG_event *ev)
   {

    if ((dev != NULL) && (dev->key_down_event_handler.fnc != NULL))
       (*(dev->key_down_event_handler.fnc))(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_KEY_UP_EVENT - handle an key_up event
 *
 * #bind PG_handle_key_up_event fortran() scheme()
 */

void PG_handle_key_up_event(PG_device *dev, PG_event *ev)
   {

    if ((dev != NULL) && (dev->key_up_event_handler.fnc != NULL))
       (*(dev->key_up_event_handler.fnc))(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_HANDLE_DEFAULT_EVENT - handle an default event
 *
 * #bind PG_handle_default_event fortran() scheme()
 */

void PG_handle_default_event(PG_device *dev, PG_event *ev)
   {

    if ((dev != NULL) && (dev->default_event_handler.fnc != NULL))
       (*(dev->default_event_handler.fnc))(dev, ev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QUERY_POINTER - report the pointer state
 *
 * #bind PG_query_pointer fortran() scheme()
 */

void PG_query_pointer(PG_device *dev, int *ir, int *pb, int *pq)
   {

    if ((dev != NULL) && (dev->query_pointer != NULL))
       (*dev->query_pointer)(dev, ir, pb, pq);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOUSE_EVENT_INFO - return info on mouse events
 *
 * #bind PG_mouse_event_info fortran() scheme()
 */

void PG_mouse_event_info(PG_device *dev, PG_event *ev,
			 int *iev, PG_mouse_button *peb, int *peq)
   {

    if ((dev != NULL) && (dev->mouse_event_info != NULL))
       (*dev->mouse_event_info)(dev, ev, iev, peb, peq);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_KEY_EVENT_INFO - return info on key events
 *
 * #bind PG_key_event_info fortran() scheme()
 */

void PG_key_event_info(PG_device *dev, PG_event *ev, int *iev,
		       char *bf, int n, int *peq)
   {

    if ((dev != NULL) && (dev->key_event_info != NULL))
       (*dev->key_event_info)(dev, ev, iev, bf, n, peq);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_OPEN_SCREEN - open the device DEV at the NDC box
 *                - of the physical device
 *
 * #bind PG_open_screen fortran() scheme()
 */

void PG_open_screen(PG_device *dev, double *ndc)
   {double xf, yf, dxf, dyf;

    if ((dev != NULL) && (dev->open_screen != NULL))
       {xf = ndc[0];
	yf = ndc[1];
	dxf = ndc[2] - xf;
	dyf = ndc[3] - yf;
	(*dev->open_screen)(dev, xf, yf, dxf, dyf);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_QUERY_SCREEN_N - query the device DEV
 *                   - return size and number of colors
 *
 * #bind PG_query_screen_n fortran() scheme()
 */

void PG_query_screen_n(PG_device *dev, int *dx, int *pnc)
   {int ldx, ldy;

    if ((dev != NULL) && (dev->query_screen != NULL))
       {(*dev->query_screen)(dev, &ldx, &ldy, pnc);
	dx[0] = ldx;
	dx[1] = ldy;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_DEVICE_CURRENT - make the device DEV the current one for drawing
 *
 * #bind PG_make_device_current fortran() scheme()
 */

void PG_make_device_current(PG_device *dev)
   {

    if ((dev != NULL) && (dev->make_device_current != NULL))
       (*dev->make_device_current)(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RELEASE_DEVICE_CURRENT - make the device DEV NOT
 *                           - the current one for drawing
 *
 * #bind PG_release_device_current fortran() scheme()
 */

void PG_release_current_device(PG_device *dev)
   {

    if ((dev != NULL) && (dev->release_current_device != NULL))
       (*dev->release_current_device)(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_UPDATE_VS - update the view surface of the device DEV
 *
 * #bind PG_update_vs fortran() scheme()
 */

void PG_update_vs(PG_device *dev)
   {

    if ((dev != NULL) && (dev->update_vs != NULL))
       (*dev->update_vs)(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FINISH_PLOT - declare the picture in the view surface of the device DEV
 *                - finished
 *
 * #bind PG_finish_plot fortran() scheme()
 */

void PG_finish_plot(PG_device *dev)
   {

    if ((dev != NULL) && (dev->finish_plot != NULL))
       (*dev->finish_plot)(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_EXPOSE_DEVICE - bring the device DEV to the top
 *
 * #bind PG_expose_device fortran() scheme()
 */

void PG_expose_device(PG_device *dev)
   {

    if ((dev != NULL) && (dev->expose_device != NULL))
       (*dev->expose_device)(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLOSE_DEVICE - close the device DEV
 *
 * #bind PG_close_device fortran() scheme()
 */

void PG_close_device(PG_device *dev)
   {

    if ((dev != NULL) && (dev->close_device != NULL))
       (*dev->close_device)(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLOSE_CONSOLE - close the console device
 *
 * #bind PG_close_console fortran() scheme()
 */

void PG_close_console(void)
   {

    if ((PG_console_device != NULL) &&
	(PG_console_device->close_console != NULL))
       (*PG_console_device->close_console)();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_FRAME - clear the current frame
 *
 * #bind PG_clear_frame fortran() scheme()
 */

void PG_clear_frame(PG_device *dev)
   {

    if (dev != NULL)
       PG_clear_region(dev, 2, NORMC, dev->g.fr, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_WINDOW - clear the current window
 *
 * #bind PG_clear_window fortran() scheme()
 */

void PG_clear_window(PG_device *dev)
   {

    if ((dev != NULL) && (dev->clear_window != NULL))
       {PG_set_attrs_glb(TRUE, "label-position-y", HUGE, NULL);
	(*dev->clear_window)(dev);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_VIEWPORT - clear the current viewport
 *
 * #bind PG_clear_viewport fortran() scheme()
 */

void PG_clear_viewport(PG_device *dev)
   {

    if ((dev != NULL) && (dev->clear_viewport != NULL))
       (*dev->clear_viewport)(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_REGION - clear the region specified by NDC
 *
 * #bind PG_clear_region fortran() scheme()
 */

void PG_clear_region(PG_device *dev, int nd, PG_coord_sys cs,
		     double *ndc, int pad)
   {

    if ((dev != NULL) && (dev->clear_region != NULL))
       (*dev->clear_region)(dev, nd, cs, ndc, pad);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CLEAR_PAGE - clear the current page
 *
 * #bind PG_clear_page fortran() scheme()
 */

void PG_clear_page(PG_device *dev, int i)
   {

    if ((dev != NULL) && (dev->clear_page != NULL))
       (*dev->clear_page)(dev, i);

    return;}

/*--------------------------------------------------------------------------*/

/* PG_WRITE_TEXT - write S to FP and/or DEV
 *
 * #bind PG_write_text fortran() scheme()
 */

void PG_write_text(PG_device *dev, FILE *fp, char *s)
   {

    if ((dev != NULL) && (dev->write_text != NULL))
       (*dev->write_text)(dev, fp, s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_NEXT_LINE - move to next line on FP and/or DEV
 *
 * #bind PG_next_line fortran() scheme()
 */

void PG_next_line(PG_device *dev, FILE *fp)
   {

    if ((dev != NULL) && (dev->next_line != NULL))
       (*dev->next_line)(dev, fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SHADE_POLY_N - draw and fill polygon R
 *
 * #bind PG_shade_poly_n fortran() scheme()
 */

void PG_shade_poly_n(PG_device *dev, int nd, int n, double **r)
   {

    if ((dev != NULL) && (dev->shade_poly != NULL))
       (*dev->shade_poly)(dev, nd, n, r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FILL_CURVE - draw curve CRV and fill underneath it
 *
 * #bind PG_fill_curve fortran() scheme()
 */

void PG_fill_curve(PG_device *dev, PG_curve *crv)
   {

    if ((dev != NULL) && (dev->fill_curve != NULL))
       (*dev->fill_curve)(dev, crv);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_CURVE - draw curve CRV
 *
 * #bind PG_draw_curve fortran() scheme()
 */

void PG_draw_curve(PG_device *dev, PG_curve *crv, int clip)
   {

    if ((dev != NULL) && (dev->draw_curve != NULL))
       (*dev->draw_curve)(dev, crv, clip);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_PALETTE_CURRENT - make P the current palette of DEV
 *
 * #bind PG_make_palette_current fortran() scheme()
 */

void PG_make_palette_current(PG_device *dev, PG_palette *p)
   {

    if (dev != NULL)
       dev->current_palette = p;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_IMAGE_N - copy the region IRG of DEV into BF
 *
 * #bind PG_get_image_n fortran() scheme()
 */

void PG_get_image_n(PG_device *dev, unsigned char *bf,
		    PG_coord_sys cs, double *irg)
   {double ix[PG_SPACEDM], nx[PG_SPACEDM], pc[PG_BOXSZ];

    if ((dev != NULL) && (dev->get_image != NULL))
       {PG_trans_box(dev, 2, cs, irg, PIXELC, pc);
	ix[0] = pc[0];
	ix[1] = pc[2];
	nx[0] = pc[1] - pc[0];
	nx[1] = pc[3] - pc[2];
	(*dev->get_image)(dev, bf, ix[0], ix[1], nx[0], nx[1]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PUT_IMAGE_N - copy the BF into region IRG of DEV
 *
 * #bind PG_put_image_n fortran() scheme()
 */

void PG_put_image_n(PG_device *dev, unsigned char *bf,
		    PG_coord_sys cs, double *irg)
   {double ix[PG_SPACEDM], nx[PG_SPACEDM], pc[PG_BOXSZ];

    if ((dev != NULL) && (dev->put_image != NULL))
       {PG_trans_box(dev, 2, cs, irg, PIXELC, pc);
	ix[0] = pc[0];
	ix[1] = pc[2];
	nx[0] = pc[1] - pc[0];
	nx[1] = pc[3] - pc[2];
	(*dev->put_image)(dev, bf, ix[0], ix[1], nx[0], nx[1]);};

    return;}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

#define PG_fgetc(stream)
    (((PG_console_device != NULL) && (PG_console_device->ggetc != NULL)) ?
     (*PG_console_device->ggetc)(stream) :
     EOF)

#define PG_fgets(buffer, maxlen, stream)
    (((PG_console_device != NULL) && (PG_console_device->ggets != NULL)) ?
     (*PG_console_device->ggets)(buffer, maxlen, stream) :
     NULL)

#define PG_puts(bf)
    if ((PG_console_device != NULL) && (PG_console_device->gputs != NULL))
       (*PG_console_device->gputs)(bf)

/*--------------------------------------------------------------------------*/

/* PG_MOVE_GR_ABS_N - move the current point of DEV to X
 *
 * #bind PG_move_gr_abs_n fortran() scheme()
 */

void PG_move_gr_abs_n(PG_device *dev, double *x)
   {

    if ((dev != NULL) && (dev->move_gr_abs != NULL))
       (*dev->move_gr_abs)(dev, x[0], x[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_TX_ABS_N - move the current text point of DEV to X
 *
 * #bind PG_move_tx_abs_n fortran() scheme()
 */

void PG_move_tx_abs_n(PG_device *dev, double *x)
   {

    if ((dev != NULL) && (dev->move_tx_abs != NULL))
       (*dev->move_tx_abs)(dev, x[0], x[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MOVE_TX_REL_N - move the current text point of DEV by X
 *
 * #bind PG_move_tx_rel_n fortran() scheme()
 */

void PG_move_tx_rel_n(PG_device *dev, double *x)
   {

    if ((dev != NULL) && (dev->move_tx_rel != NULL))
       (*dev->move_tx_rel)(dev, x[0], x[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_TO_ABS_N - draw from the current point of DEV to X
 *
 * #bind PG_draw_to_abs_n fortran() scheme()
 */

void PG_draw_to_abs_n(PG_device *dev, double *x)
   {

    if ((dev != NULL) && (dev->draw_to_abs != NULL))
       (*dev->draw_to_abs)(dev, x[0], x[1]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_TO_REL_N - draw from the current point of DEV by X
 *
 * #bind PG_draw_to_rel_n fortran() scheme()
 */

void PG_draw_to_rel_n(PG_device *dev, double *x)
   {

    if ((dev != NULL) && (dev->draw_to_rel != NULL))
       (*dev->draw_to_rel)(dev, x[0], x[1]);

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
