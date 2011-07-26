/*
 * GSPR.C - PGS host independent primitive routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_TEXT_EXT_N - return the text extent in NDC of the given string
 *
 * #bind PG_get_text_ext_n fortran() scheme() python()
 */

void PG_get_text_ext_n(PG_device *dev ARG(,,cls), int nd,
		       PG_coord_sys cs ARG(NORMC), char *s, double *p)
   {

    p[0] = 0.0;
    p[1] = 0.0;
    if (dev != NULL)
       {if (dev->get_text_ext != NULL)
           {(*dev->get_text_ext)(dev, nd, cs, s, p);
/*	    PG_trans_interval(dev, nd, NORMC, p, cs, p); */
	    if (cs == WORLDC)
	       PG_lin_point(dev, 2, p);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_GET_MARKER_SCALE - get the correct marker scale
 *                      - especially for raster devices
 */

static double _PG_get_marker_scale(PG_device *dev)
   {double sc;
    PG_RAST_device *mdv;

    sc = PG_fget_marker_scale(dev);
    
    GET_RAST_DEVICE(dev, mdv);
    if (mdv != NULL)
       sc *= mdv->text_scale;   

    return(sc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SET_RASTER_TEXT_SCALE - set correct text scale for stroked text
 *                           - in raster devices
 */

void _PG_set_raster_text_scale(PG_device *dev, double sc)
   {PG_RAST_device *mdv;

    GET_RAST_DEVICE(dev, mdv);
    if (mdv != NULL)
       mdv->text_scale = sc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_ATTRIBUTES - return the graphical attributes of the device in a
 *                   - structure
 *                   - the attributes returned are:
 *                   -
 *                   - struct s_PG_dev_attributes
 *                   -    {int clipping;
 *                   -     int char_font;
 *                   -     double char_frac;
 *                   -     int char_precision;
 *                   -     double char_space;
 *                   -     char charsz[2];
 *                   -     double char_up[PG_SPACEDM];
 *                   -     int fill_color;
 *                   -     int line_color;
 *                   -     int line_style;
 *                   -     double line_width;
 *                   -     int logical_op;
 *                   -     int text_color;
 *                   -     PG_palette *palette;};
 *                   -
 *                   - typedef struct s_PG_dev_attributes PG_dev_attributes
 *                   -
 */

PG_dev_attributes *PG_get_attributes(PG_device *dev)
   {int id;
    PG_dev_attributes *attr;

    attr = CMAKE(PG_dev_attributes);

    attr->clipping       = dev->clipping;
    attr->char_frac      = dev->char_frac;
    attr->char_precision = dev->char_precision;
    attr->char_space     = dev->char_space;
    attr->fill_color     = dev->fill_color;
    attr->line_color     = dev->line_color;
    attr->line_style     = dev->line_style;
    attr->line_width     = dev->line_width;
    attr->logical_op     = dev->logical_op;
    attr->text_color     = dev->text_color;
    attr->palette        = dev->current_palette;

    for (id = 0; id < PG_SPACEDM; id++)
        attr->char_up[id] = dev->char_up[id];

    return(attr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_FONT - map the given font name to the host font name and
 *               - perform other generic font setup
 *               - return TRUE iff successful
 */

int PG_setup_font(PG_device *dev, char *face, char *style, int size, 
                  char **pfn, int *pnf, int *pns)
   {int l, fn, nfont;
    PG_font_family *ff;
    char **fs;

    if (dev->type_face == NULL)
       dev->type_face = CSTRSAVE(face);

    else if (strcmp(dev->type_face, face) != 0)
       {CFREE(dev->type_face);
	dev->type_face = CSTRSAVE(face);};

    if (dev->type_style == NULL)
       dev->type_style = CSTRSAVE(style);

    else if (strcmp(dev->type_style, style) != 0)
       {CFREE(dev->type_style);
	dev->type_style = CSTRSAVE(style);};

    dev->type_size  = size;

    nfont = 0;
    for (ff = dev->font_family; ff != NULL; ff = ff->next)
        {if (strcmp(ff->type_face, face) == 0)
           break;
         nfont += ff->n_styles;};

    if (ff == NULL)
       return(FALSE);

    fs = ff->type_styles;
    fn = ff->n_styles;
    l  = 0;
    if (strcmp(style, "medium") == 0)
       l = 0;
    else if (strcmp(style, "italic") == 0)
       l = 1;
    else if (strcmp(style, "bold") == 0)
       l = 2;
    else if (strcmp(style, "bold-italic") == 0)
       l = 3;

    if (l >= fn)
       return(FALSE);

    *pfn = fs[l];

    nfont += l;
    *pnf   = nfont;
    *pns   = l;

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                          STATE CHANGE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* PG_SET_EXPOSE_EVENT_HANDLER - set the handler to the new FNC and return
 *                             - the old one
 */

PFEventHand PG_set_expose_event_handler(PG_device *dev, PFEventHand fnc)
   {PFEventHand oldfnc;

    if (dev != NULL)
       {oldfnc = dev->expose_event_handler.fnc;
	dev->expose_event_handler.fnc = fnc;}
    else
       oldfnc = NULL;

    return(oldfnc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_UPDATE_EVENT_HANDLER - set the handler to the new FNC and return
 *                             - the old one
 */

PFEventHand PG_set_update_event_handler(PG_device *dev, PFEventHand fnc)
   {PFEventHand oldfnc;

    if (dev != NULL)
       {oldfnc = dev->update_event_handler.fnc;
	dev->update_event_handler.fnc = fnc;}
    else
       oldfnc = NULL;

    return(oldfnc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_MOUSE_DOWN_EVENT_HANDLER - set the handler to the new FNC
 *                                 - and return the old one
 */

PFEventHand PG_set_mouse_down_event_handler(PG_device *dev, PFEventHand fnc)
   {PFEventHand oldfnc;

    if (dev != NULL)
       {oldfnc = dev->mouse_down_event_handler.fnc;
	dev->mouse_down_event_handler.fnc = fnc;}
    else
       oldfnc = NULL;

    return(oldfnc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_MOUSE_UP_EVENT_HANDLER - set the handler to the new FNC
 *                               - and return the old one
 */

PFEventHand PG_set_mouse_up_event_handler(PG_device *dev, PFEventHand fnc)
   {PFEventHand oldfnc;

    if (dev != NULL)
       {oldfnc = dev->mouse_up_event_handler.fnc;
	dev->mouse_up_event_handler.fnc = fnc;}
    else
       oldfnc = NULL;

    return(oldfnc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_MOTION_EVENT_HANDLER - set the handler to the new FNC
 *                             - and return the old one
 */

PFEventHand PG_set_motion_event_handler(PG_device *dev, PFEventHand fnc)
   {PFEventHand oldfnc;

    if (dev != NULL)
       {oldfnc = dev->motion_event_handler.fnc;
	dev->motion_event_handler.fnc = fnc;}
    else
       oldfnc = NULL;

    return(oldfnc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_KEY_DOWN_EVENT_HANDLER - set the handler to the new FNC
 *                               - and return the old one
 */

PFEventHand PG_set_key_down_event_handler(PG_device *dev, PFEventHand fnc)
   {PFEventHand oldfnc;

    if (dev != NULL)
       {oldfnc = dev->key_down_event_handler.fnc;
	dev->key_down_event_handler.fnc = fnc;}
    else
       oldfnc = NULL;

    return(oldfnc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_KEY_UP_EVENT_HANDLER - set the handler to the new FNC
 *                             - and return the old one
 */

PFEventHand PG_set_key_up_event_handler(PG_device *dev, PFEventHand fnc)
   {PFEventHand oldfnc;

    if (dev != NULL)
       {oldfnc = dev->key_up_event_handler.fnc;
	dev->key_up_event_handler.fnc = fnc;}
    else
       oldfnc = NULL;

    return(oldfnc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_DEFAULT_EVENT_HANDLER - set the handler to the new FNC
 *                              - and return the old one
 */

PFEventHand PG_set_default_event_handler(PG_device *dev, PFEventHand fnc)
   {PFEventHand oldfnc;

    if (dev != NULL)
       {oldfnc = dev->default_event_handler.fnc;
	dev->default_event_handler.fnc = fnc;}
    else
       oldfnc = NULL;

    return(oldfnc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_ATTRIBUTES - set the graphical attributes of the device to 
 *                   - those stored in the PG_dev_attribute structure
 *                   - the attributes which were saved are:
 *                   -
 *                   - struct s_PG_dev_attributes
 *                   -    {int clipping;
 *                   -     double char_frac;
 *                   -     int char_precision;
 *                   -     double char_space;
 *                   -     char charsz[2];
 *                   -     double char_up[PG_SPACEDM];
 *                   -     int fill_color;
 *                   -     int line_color;
 *                   -     int line_style;
 *                   -     double line_width;
 *                   -     int logical_op;
 *                   -     int text_color;
 *                   -     PG_palette *palette;};
 *                   -
 *                   - typedef struct s_PG_dev_attributes PG_dev_attributes
 *                   -
 */

void PG_set_attributes(PG_device *dev, PG_dev_attributes *attr)
   {int id;

    if ((dev != NULL) && (attr != NULL))
       {PG_fset_clipping(dev, attr->clipping);

	dev->char_frac       = attr->char_frac;
	dev->char_precision  = attr->char_precision;
	dev->char_space      = attr->char_space;
	dev->fill_color      = attr->fill_color;
	dev->line_color      = attr->line_color;
	dev->line_style      = attr->line_style;
	dev->logical_op      = attr->logical_op;
	dev->text_color      = attr->text_color;
	dev->current_palette = attr->palette;

	for (id = 0; id < PG_SPACEDM; id++)
	    dev->char_up[id] = attr->char_up[id];

/* because of the line width mapping this cannot be done as the other are */
	PG_fset_line_width(dev, attr->line_width);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_MARKERS - set up a set of common markers */

void PG_setup_markers(void)
   {double x1[20], y1[20], x2[20], y2[20];

    if (_PG.marker_list != NULL)
       return;

/* define '+' marker */
    x1[0] = -1.0;
    y1[0] =  0.0;

    x2[0] =  1.0;
    y2[0] =  0.0;

    x1[1] =  0.0;
    y1[1] = -1.0;

    x2[1] =  0.0;
    y2[1] =  1.0;

    PG_def_marker(2, x1, y1, x2, y2);

/* define '*' marker */
    x1[2] = -0.707;
    y1[2] = -0.707;

    x2[2] =  0.707;
    y2[2] =  0.707;

    x1[3] = -0.707;
    y1[3] =  0.707;

    x2[3] =  0.707;
    y2[3] = -0.707;

    PG_def_marker(4, x1, y1, x2, y2);

/* define triangle marker */
    x1[0] = -1.0;
    y1[0] = -1.0;

    x2[0] =  1.0;
    y2[0] = -1.0;

    x1[1] =  1.0;
    y1[1] = -1.0;

    x2[1] =  0.0;
    y2[1] =  1.0;

    x1[2] =  0.0;
    y1[2] =  1.0;

    x2[2] = -1.0;
    y2[2] = -1.0;

    PG_def_marker(3, x1, y1, x2, y2);

/* define circle marker */
    x1[0] = -0.0833;
    y1[0] = 0.4167;

    x2[0] = -0.2500;
    y2[0] = 0.3333;

    x1[1] = -0.2500;
    y1[1] = 0.3333;

    x2[1] = -0.4167;
    y2[1] = 0.1667;

    x1[2] = -0.4167;
    y1[2] = 0.1667;

    x2[2] = -0.5000;
    y2[2] = -0.0833;

    x1[3] = -0.5000;
    y1[3] = -0.0833;

    x2[3] = -0.5000;
    y2[3] = -0.2500;

    x1[4] = -0.5000;
    y1[4] = -0.2500;

    x2[4] = -0.4167;
    y2[4] = -0.5000;

    x1[5] = -0.4167;
    y1[5] = -0.5000;

    x2[5] = -0.2500;
    y2[5] = -0.6667;

    x1[6] = -0.2500;
    y1[6] = -0.6667;

    x2[6] = -0.0833;
    y2[6] = -0.7500;

    x1[7] = -0.0833;
    y1[7] = -0.7500;

    x2[7] = 0.1667;
    y2[7] = -0.7500;

    x1[8] = 0.1667;
    y1[8] = -0.7500;

    x2[8] = 0.3333;
    y2[8] = -0.6667;

    x1[9] = 0.3333;
    y1[9] = -0.6667;

    x2[9] = 0.5000;
    y2[9] = -0.5000;

    x1[10] = 0.5000;
    y1[10] = -0.5000;

    x2[10] = 0.5833;
    y2[10] = -0.2500;

    x1[11] = 0.5833;
    y1[11] = -0.2500;

    x2[11] = 0.5833;
    y2[11] = -0.0833;

    x1[12] = 0.5833;
    y1[12] = -0.0833;

    x2[12] = 0.5000;
    y2[12] = 0.1667;

    x1[13] = 0.5000;
    y1[13] = 0.1667;

    x2[13] = 0.3333;
    y2[13] = 0.3333;

    x1[14] = 0.3333;
    y1[14] = 0.3333;

    x2[14] = 0.1667;
    y2[14] = 0.4167;

    x1[15] = 0.1667;
    y1[15] = 0.4167;

    x2[15] = -0.0833;
    y2[15] = 0.4167;

    PG_def_marker(16, x1, y1, x2, y2);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RL_MARKERS - release memory associated with markers */

void _PG_rl_markers(void)
   {int i;
    PG_marker *mrk;

    for (i = 0; i < _PG_gattrs.marker_index; i++)
        {mrk = &_PG.marker_list[i];
	 CFREE(mrk->x1);
	 CFREE(mrk->y1);
	 CFREE(mrk->x2);
	 CFREE(mrk->y2);};

    CFREE(_PG.marker_list);

    _PG_gattrs.marker_index = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DEF_MARKER - define a marker character and return an index into
 *               - an internal list of markers
 *               - a marker is defined in terms of a list of line segments
 *               - in a box (-1, -1) to (1, 1)
 *               - a scale factor may be separately specified
 *               - the marker made contains copies of the input arrays
 *
 * #bind PG_def_marker fortran() scheme() python()
 */

int PG_def_marker(int n, double *x1, double *y1, double *x2, double *y2)
   {int ne, nm;
    PG_marker *mrk;
    double *x1a, *y1a, *x2a, *y2a;

    if (_PG.marker_list == NULL)
       {_PG.marker_max = 10;
        _PG.marker_list = CMAKE_N(PG_marker, _PG.marker_max);};

    x1a = CMAKE_N(double, n);    
    y1a = CMAKE_N(double, n);    
    x2a = CMAKE_N(double, n);    
    y2a = CMAKE_N(double, n);    

    ne = n*sizeof(double);
    memcpy(x1a, x1, ne);
    memcpy(y1a, y1, ne);
    memcpy(x2a, x2, ne);
    memcpy(y2a, y2, ne);

    mrk        = &_PG.marker_list[_PG_gattrs.marker_index++];
    mrk->x1    = x1a;
    mrk->y1    = y1a;
    mrk->x2    = x2a;
    mrk->y2    = y2a;
    mrk->n_seg = n;

    if (_PG_gattrs.marker_index >= _PG.marker_max)
       {_PG.marker_max += 10;
        CREMAKE(_PG.marker_list, PG_marker, _PG.marker_max);};

    nm = _PG_gattrs.marker_index - 1;

    return(nm);}

/*--------------------------------------------------------------------------*/

/*                          MOVE AND DRAW ROUTINES                          */

/*--------------------------------------------------------------------------*/
 
/* _PG_MOVE_GR_ABS - move the current graphics cursor position to the
 *                 - given absolute coordinates in WC
 */
 
void _PG_move_gr_abs(PG_device *dev, double *p)
   {

    if (dev != NULL)
       {PG_log_point(dev, 2, p);
 
	dev->gcur[0] = p[0];
	dev->gcur[1] = p[1];

	p[0] = dev->gcur[0];
	p[1] = dev->gcur[1];};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_MOVE_GR_REL - move the current graphics cursor position to the
 *                 - given relative coordinates in WC
 */
 
void _PG_move_gr_rel(PG_device *dev, double *p)
   {

    if (dev != NULL)
       {PG_log_point(dev, 2, p);
 
	dev->gcur[0] += p[0];
	dev->gcur[1] += p[1];
 
	p[0] = dev->gcur[0];
	p[1] = dev->gcur[1];};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_MOVE_TX_ABS - move the current text cursor position to the
 *                 - given coordinates in WC
 */
 
void _PG_move_tx_abs(PG_device *dev, double *p)
   {

    if (dev != NULL)
       {PG_log_point(dev, 2, p);
 
	dev->tcur[0] = p[0];
	dev->tcur[1] = p[1];

	p[0] = dev->tcur[0];
	p[1] = dev->tcur[1];};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_MOVE_TX_REL - move the current text cursor position to the
 *                 - given relative coordinates in WC
 */
 
void _PG_move_tx_rel(PG_device *dev, double *p)
   {

    if (dev != NULL)
       {PG_log_point(dev, 2, p);
 
	dev->tcur[0] += p[0];
	dev->tcur[1] += p[1];
 
	p[0] = dev->tcur[0];
	p[1] = dev->tcur[1];};
 
    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_DRAW_MARKERS_2 - draw N 2D markers at CS points R */
 
static void _PG_draw_markers_2(PG_device *dev, PG_coord_sys cs,
			       int n, double **r, int marker)
   {int i, j, l, id, na, nd, nm;
    double x0[2], dx[2], sc;
    double bnd[PG_BOXSZ];
    double *xa[2], *xb[2];
    double **rd;
    PG_marker *mrk;

    if ((dev == NULL) || (n <= 0))
       return;

    sc = _PG_get_marker_scale(dev);

    mrk   = &_PG.marker_list[marker];
    xa[0] = mrk->x1;
    xb[0] = mrk->x2;
    xa[1] = mrk->y1;
    xb[1] = mrk->y2;
    nm    = mrk->n_seg;

    nd = 2;
    rd = PM_make_vectors(nd, 2*nm*n);

    PG_get_viewspace(dev, BOUNDC, bnd);

/* if clipping is off push the limits off a factor of 10
 * this will get the text and prevent possible problems with
 * arithmetic over and underflows
 */
    if (dev->clipping == FALSE)
       {for (id = 0; id < nd; id++)
	    {l = 2*id;
	     dx[id]    = 10.0*(bnd[l+1] - bnd[l]);
	     bnd[l]   -= dx[id];
	     bnd[l+1] += dx[id];};};

    l  = 0;
    na = 0;
    for (i = 0; i < n; i++)
        {x0[0] = r[0][i];
         x0[1] = r[1][i];

/* take only points that are in bounds */
	 if (PG_box_contains(2, bnd, x0) == TRUE)
	    {PG_log_point(dev, nd, x0);
	     PG_trans_point(dev, 2, WORLDC, x0, NORMC, x0);

	     for (j = 0; j < nm; j++)
	         {for (id = 0; id < nd; id++)
		      {rd[id][l]   = x0[id] + sc*xa[id][j];
		       rd[id][l+1] = x0[id] + sc*xb[id][j];};
		  l += 2;};

	     na++;};};

    PG_draw_disjoint_polyline_n(dev, 2, NORMC, na*nm, rd, FALSE);

    PM_free_vectors(nd, rd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_DRAW_MARKERS_3 - draw N 3D markers at CS points R */
 
static void _PG_draw_markers_3(PG_device *dev, PG_coord_sys cs,
			       int n, double **r, int marker)
   {int i, j, l, id, nd, nm;
    double x0[PG_SPACEDM], dx[PG_SPACEDM], sc;
    double bx[PG_BOXSZ];
    double *xa[2], *xb[2];
    double **rd;
    PG_marker *mrk;
    PG_dev_geometry *g;

    if ((dev == NULL) || (n <= 0))
       return;

    g  = &dev->g;
    nd = PG_SPACEDM;

    sc = _PG_get_marker_scale(dev);

    mrk   = &_PG.marker_list[marker];
    xa[0] = mrk->x1;
    xb[0] = mrk->x2;
    xa[1] = mrk->y1;
    xb[1] = mrk->y2;
    nm    = mrk->n_seg;

    rd = PM_make_vectors(nd, 2*nm*n);

/* if clipping is off push the limits off a factor of 10
 * this will get the text and prevent possible problems with
 * arithmetic over and underflows
 */
    if (dev->clipping == FALSE)
       {for (id = 0; id < nd; id++)
	    {l       = 2*id;
	     dx[id]  = 10.0*(g->wc[l+1] - g->wc[l]);
	     bx[l]   = g->wc[l] - dx[id];
	     bx[l+1] = g->wc[l+1] + dx[id];};}
    else
       {for (id = 0; id < nd; id++)
	    {l       = 2*id;
	     bx[l]   = g->wc[l];
	     bx[l+1] = g->wc[l+1];};};

    l = 0;
    for (i = 0; i < n; i++)
        {for (id = 0; id < nd; id++)
	     x0[id] = r[id][i];

/* take only points that are in bounds */
	 if (PG_box_contains(nd, bx, x0) == TRUE)
	    {PG_log_point(dev, nd, x0);
	     PG_trans_point(dev, 2, WORLDC, x0, NORMC, x0);

	     for (j = 0; j < nm; j++)
	         {for (id = 0; id < 2; id++)
		      {rd[id][l]   = x0[id] + sc*xa[id][j];
		       rd[id][l+1] = x0[id] + sc*xb[id][j];};
		  rd[id][l]   = x0[id];
		  rd[id][l+1] = x0[id];
		  l += 2;};};};

    PG_trans_points(dev, l, 2, NORMC, rd, WORLDC, rd);

    PG_draw_disjoint_polyline_n(dev, nd, WORLDC, l/2, rd, FALSE);

    PM_free_vectors(nd, rd);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DRAW_MARKERS_N - draw N ND markers at CS points R
 *
 * #bind PG_draw_markers_n fortran() scheme() python()
 */
 
void PG_draw_markers_n(PG_device *dev ARG(,,cls),
		       int nd, PG_coord_sys cs,
		       int n, double **r, int marker)
   {

    if ((dev != NULL) && (0 < n))
       {switch (nd)
	   {case 3 :
	         _PG_draw_markers_3(dev, cs, n, r, marker);
		 break;
	    case 2  :
	    default :
		   _PG_draw_markers_2(dev, cs, n, r, marker);
		 break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DRAW_BOX_N - draw the BOX in the specified coordinates
 *
 * #bind PG_draw_box_n fortran() scheme() python()
 */

void PG_draw_box_n(PG_device *dev ARG(,,cls),
		   int nd, PG_coord_sys cs, double *box)
   {double x[5], y[5], bx[PG_BOXSZ];
    double **p, *r[PG_SPACEDM];
    PM_polygon *py;

    PG_trans_box(dev, nd, cs, box, WORLDC, bx);

    switch (nd)
       {default :
	case 2  :
	     x[0] = bx[0];
	     y[0] = bx[2];
	     x[1] = bx[0];
	     y[1] = bx[3];
	     x[2] = bx[1];
	     y[2] = bx[3];
	     x[3] = bx[1];
	     y[3] = bx[2];
	     x[4] = bx[0];
	     y[4] = bx[2];

	     r[0] = x;
	     r[1] = y;

	     PG_draw_polyline_n(dev, 2, WORLDC, 5, r, TRUE);
	     break;

	case 3 :
	     p  = PG_get_space_box(dev, bx, FALSE);
	     py = PM_convex_hull(p[0], p[1], 8);

	     PG_draw_polygon(dev, py, FALSE);

	     PM_free_polygon(py);
	     PM_free_vectors(3, p);
	     break;};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_FILL_POLYGON_N - draw an ND polygon with N nodes R in CS
 *                   - and fill the interior with COLOR
 *
 * #bind PG_fill_polygon_n fortran() scheme() python()
 */

void PG_fill_polygon_n(PG_device *dev ARG(,,cls), int color ARG(1),
		       int mapped ARG(TRUE),
		       int nd, PG_coord_sys cs ARG(WORLDC),
		       long n, double **r)
   {int ip, np;
    double **cr;
    PM_polygon *pc, *py;
    SC_array *a;
    static int count = 0;

    if ((dev != NULL) && (n > nd))
       {PG_trans_points(dev, n, nd, cs, r, WORLDC, r);

	py = PM_make_polygon(nd, n, r[0], r[1], r[2]);

	if (dev->clipping)
	   {a = PG_clip_polygon(dev, py);

	    np = SC_array_get_n(a);
	    for (ip = 0; ip < np; ip++)
	        {pc = PM_polygon_get(a, ip);
		 cr = pc->x;
		 n  = pc->nn;
       
		 PG_fset_fill_color(dev, color, mapped);
		 PG_shade_poly_n(dev, nd, n, cr);};

	    PM_free_polygons(a, TRUE);}

	else
	   {cr = py->x;
	    PG_fset_fill_color(dev, color, mapped);
	    PG_shade_poly_n(dev, nd, n, cr);};

	PM_free_polygon(py);

	count++;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

