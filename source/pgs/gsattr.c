/*
 * GSATTR.C - manage plotting attributes
 *          - THREADSAFE and THREADED
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

#define ATTR_STR(_n, _v) SC_hasharr_install(_PG.attr_table, _n, _v, SC_STRING_S, TRUE, TRUE)
#define ATTR_INT(_n, _v) SC_hasharr_install(_PG.attr_table, _n, _v, SC_INT_S,    TRUE, TRUE)
#define ATTR_DBL(_n, _v) SC_hasharr_install(_PG.attr_table, _n, _v, SC_DOUBLE_S, TRUE, TRUE)

gattrdes
 _PG_gattrs;

/*--------------------------------------------------------------------------*/

/*                              GET ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* _PG_GET_ATTRS_ALIST - retrieve the values of the specified attributes
 *                     - and assign them to the specified variables
 *                     - iff DFLT is TRUE use the given default value
 *                     - when the attribute does not exist on the alist
 *                     - specifications are quadruples:
 *                          <attr>, <type>, <var>, <default>
 *                     - where
 *                     -    <attr>    = string naming the attribute
 *                     -                (e.g. "LINE-COLOR")
 *                     -    <type>    = integer type code for
 *                     -              = char, short, int, long,
 *                     -              = float, double, or pointer
 *                     -    <var>     = the address of the variable
 *                     -              = to set
 *                     -    <default> = the default value to assign
 *                     -              = if the attribute is not present
 *                     - return the number of attributes processed
 */

static int _PG_get_attrs_alist(pcons *alst, int dflt, va_list __a__)
   {int i, typ;
    int *pi, iv;
    long *pl, lv;
    short *ps;
    double *pd, dv;
    REAL *pr;
    float *pf;
    char *pc;
    char **pt, *tv;
    char *name;
    void *pv;

    for (i = 0; TRUE; i++)
        {name = SC_VA_ARG(char *);
	 if (name == NULL)
	    break;

	 typ = SC_VA_ARG(int);
	 pv  = NULL;
	 SC_assoc_info(alst, name, &pv, NULL);

/* fixed point types */
	 if (typ == SC_SHORT_I)
	    {ps = SC_VA_ARG(short *);
	     iv = SC_VA_ARG(int);
	     if (pv != NULL)
	        *ps = *(short *) pv;
	     else if (dflt == TRUE)
	        *ps = iv;}

	 else if (typ == SC_INT_I)
	    {pi = SC_VA_ARG(int *);
	     iv = SC_VA_ARG(int);
	     if (pv != NULL)
	        *pi = *(int *) pv;
	     else if (dflt == TRUE)
	        *pi = iv;}

	 else if (typ == SC_LONG_I)
	    {pl = SC_VA_ARG(long *);
	     lv = SC_VA_ARG(long);
	     if (pv != NULL)
	        *pl = *(long *) pv;
	     else if (dflt == TRUE)
	        *pl = lv;}

	 else if (typ == SC_LONG_LONG_I)
	    {pl = SC_VA_ARG(long long *);
	     lv = SC_VA_ARG(long long);
	     if (pv != NULL)
	        *pl = *(long long *) pv;
	     else if (dflt == TRUE)
	        *pl = lv;}

/* floating point types */
	 else if (typ == SC_FLOAT_I)
	    {pf = SC_VA_ARG(float *);
	     dv = SC_VA_ARG(double);
	     if (pv != NULL)
	        *pf = *(float *) pv;
	     else if (dflt == TRUE)
	        *pf = dv;}

	 else if (typ == SC_DOUBLE_I)
	    {pd = SC_VA_ARG(double *);
	     dv = SC_VA_ARG(double);
	     if (pv != NULL)
	        *pd = *(double *) pv;
	     else if (dflt == TRUE)
	        *pd = dv;}

	 else if (typ == SC_LONG_DOUBLE_I)
	    {pd = SC_VA_ARG(long double *);
	     dv = SC_VA_ARG(long double);
	     if (pv != NULL)
	        *pd = *(long double *) pv;
	     else if (dflt == TRUE)
	        *pd = dv;}

/* complex floating point types */
	 else if (typ == SC_FLOAT_COMPLEX_I)
	    {pf = SC_VA_ARG(float _Complex *);
	     dv = SC_VA_ARG(float _Complex);
	     if (pv != NULL)
	        *pf = *(float *) pv;
	     else if (dflt == TRUE)
	        *pf = dv;}

	 else if (typ == SC_DOUBLE_COMPLEX_I)
	    {pd = SC_VA_ARG(double _Complex *);
	     dv = SC_VA_ARG(double _Complex);
	     if (pv != NULL)
	        *pd = *(double *) pv;
	     else if (dflt == TRUE)
	        *pd = dv;}

	 else if (typ == SC_LONG_DOUBLE_COMPLEX_I)
	    {pd = SC_VA_ARG(long double _Complex *);
	     dv = SC_VA_ARG(long double _Complex);
	     if (pv != NULL)
	        *pd = *(long double *) pv;
	     else if (dflt == TRUE)
	        *pd = dv;}

	 else if (typ == SC_CHAR_I)
	    {pc = SC_VA_ARG(char *);
	     iv = SC_VA_ARG(int);
	     if (pv != NULL)
	        *pc = *(char *) pv;
	     else if (dflt == TRUE)
	        *pc = iv;}

	 else if (typ == SC_POINTER_I)
	    {pt = SC_VA_ARG(char **);
	     tv = SC_VA_ARG(char *);
	     if (pv != NULL)
	        *pt = pv;
	     else if (dflt == TRUE)
	        *pt = tv;};};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_ATTRS_GRAPH - retrieve the values of the specified attributes
 *                    - from the given PG_graph
 */

int PG_get_attrs_graph(PG_graph *g, int all, ...)
   {int n;

    SC_VA_START(all);

    n = _PG_get_attrs_alist(g->info, TRUE, SC_VA_VAR);

    SC_VA_END;

    if (all == TRUE)
       {if ((g->f != NULL) && (g->use == NULL))
	   {if (g->f->map != NULL)
	       {SC_VA_START(all);
		n += _PG_get_attrs_alist(g->f->map, FALSE, SC_VA_VAR);
		SC_VA_END;};

	    if (g->f->domain != NULL)
	       {SC_VA_START(all);
		n += _PG_get_attrs_alist(g->f->domain->info, FALSE, SC_VA_VAR);
		SC_VA_END;};

	    if (g->f->range != NULL)
	       {SC_VA_START(all);
		n += _PG_get_attrs_alist(g->f->range->info, FALSE, SC_VA_VAR);
		SC_VA_END;};};};
	
    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_ATTRS_MAPPING - retrieve the values of the specified attributes
 *                      - from the given PM_mapping
 */

int PG_get_attrs_mapping(PM_mapping *f, ...)
   {int n;

    SC_VA_START(f);

    n = _PG_get_attrs_alist(f->map, TRUE, SC_VA_VAR);

    SC_VA_END;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_ATTRS_SET - retrieve the values of the specified attributes
 *                  - from the given PM_set
 */

int PG_get_attrs_set(PM_set *s, ...)
   {int n;

    SC_VA_START(s);

    n = _PG_get_attrs_alist(s->info, TRUE, SC_VA_VAR);

    SC_VA_END;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_ATTRS_ALIST - retrieve the values of the specified attributes
 *                    - from the given association list
 */

int PG_get_attrs_alist(pcons *alst, ...)
   {int n;

    SC_VA_START(alst);

    n = _PG_get_attrs_alist(alst, TRUE, SC_VA_VAR);

    SC_VA_END;

    return(n);}

/*--------------------------------------------------------------------------*/

/*                              SET ROUTINES                                */

/*--------------------------------------------------------------------------*/

/* _PG_SET_ATTRS_ALIST - set one or more graphics attribute
 *                     - specifications are quadruples:
 *                          <attr>, <type>, <ptr>, <val>
 *                     - where
 *                     -    <attr>    = string naming the attribute
 *                     -                (e.g. "LINE-COLOR")
 *                     -    <type>    = integer type code for
 *                     -              = char, short, int, long,
 *                     -              = float, double, or pointer
 *                     -    <ptr>     = TRUE iff the attribute is an array
 *                     -    <val>     = the attribute value
 *                     - return the resulting alist
 */

static pcons *_PG_set_attrs_alist(pcons *alst, va_list SC_VA_VAR)
   {int i, typ, ptr, iv;
    char *name;
    long lv;
    double dv;
    void *pv;

    for (i = 0; TRUE; i++)
        {name = SC_VA_ARG(char *);
	 if (name == NULL)
	    break;

	 typ = SC_VA_ARG(int);
	 ptr = SC_VA_ARG(int);

/* fixed point types */
	 if (typ == SC_SHORT_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(short *);
		 alst = SC_change_alist(alst, name, SC_SHORT_P_S, pv);}
	     else
	        {iv = SC_VA_ARG(int);
		 SC_CHANGE_VALUE_ALIST(alst, short, SC_SHORT_P_S,
				       name, iv);};}

	 else if (typ == SC_INT_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(int *);
		 alst = SC_change_alist(alst, name, SC_INT_P_S, pv);}
	     else
	        {iv = SC_VA_ARG(int);
		 SC_CHANGE_VALUE_ALIST(alst, int, SC_INT_P_S,
				       name, iv);};}

	 else if (typ == SC_LONG_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(long *);
		 alst = SC_change_alist(alst, name, SC_LONG_P_S, pv);}
	     else
	        {lv = SC_VA_ARG(long);
		 SC_CHANGE_VALUE_ALIST(alst, long, SC_LONG_P_S,
				       name, lv);};}

	 else if (typ == SC_LONG_LONG_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(long long *);
		 alst = SC_change_alist(alst, name, SC_LONG_LONG_P_S, pv);}
	     else
	        {lv = SC_VA_ARG(long long);
		 SC_CHANGE_VALUE_ALIST(alst, long, SC_LONG_LONG_P_S,
				       name, lv);};}

/* floating point types */
	 else if (typ == SC_FLOAT_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(float *);
		 alst = SC_change_alist(alst, name, SC_FLOAT_P_S, pv);}
	     else
	        {dv = SC_VA_ARG(double);
		 SC_CHANGE_VALUE_ALIST(alst, float, SC_FLOAT_P_S,
				       name, dv);};}

	 else if (typ == SC_DOUBLE_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(double *);
		 alst = SC_change_alist(alst, name, SC_DOUBLE_P_S, pv);}
	     else
	        {dv = SC_VA_ARG(double);
		 SC_CHANGE_VALUE_ALIST(alst, double, SC_DOUBLE_P_S,
				       name, dv);};}

	 else if (typ == SC_LONG_DOUBLE_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(long double *);
		 alst = SC_change_alist(alst, name, SC_LONG_DOUBLE_P_S, pv);}
	     else
	        {dv = SC_VA_ARG(long double);
		 SC_CHANGE_VALUE_ALIST(alst, double, SC_LONG_DOUBLE_P_S,
				       name, dv);};}

/* complex floating point types */
	 else if (typ == SC_FLOAT_COMPLEX_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(float _Complex *);
		 alst = SC_change_alist(alst, name, SC_FLOAT_COMPLEX_P_S, pv);}
	     else
	        {dv = SC_VA_ARG(float _Complex);
		 SC_CHANGE_VALUE_ALIST(alst, float, SC_FLOAT_COMPLEX_P_S,
				       name, dv);};}

	 else if (typ == SC_DOUBLE_COMPLEX_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(double _Complex *);
		 alst = SC_change_alist(alst, name, SC_DOUBLE_COMPLEX_P_S, pv);}
	     else
	        {dv = SC_VA_ARG(double _Complex);
		 SC_CHANGE_VALUE_ALIST(alst, double, SC_DOUBLE_COMPLEX_P_S,
				       name, dv);};}

	 else if (typ == SC_LONG_DOUBLE_COMPLEX_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(long double _Complex *);
		 alst = SC_change_alist(alst, name, SC_LONG_DOUBLE_COMPLEX_P_S, pv);}
	     else
	        {dv = SC_VA_ARG(long double _Complex);
		 SC_CHANGE_VALUE_ALIST(alst, double, SC_LONG_DOUBLE_COMPLEX_P_S,
				       name, dv);};}

	 else if (typ == SC_CHAR_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(char *);
		 alst = SC_change_alist(alst, name, SC_STRING_S, pv);}
	     else
	        {iv = SC_VA_ARG(int);
		 SC_CHANGE_VALUE_ALIST(alst, char, SC_STRING_S,
				       name, iv);};}

	 else if (typ == SC_POINTER_I)
	    {if (ptr)
	        {pv = SC_VA_ARG(void **);
		 alst = SC_change_alist(alst, name, SC_POINTER_S, pv);}
	     else
	        {pv = SC_VA_ARG(void *);
		 SC_CHANGE_VALUE_ALIST(alst, void *, SC_POINTER_S,
				       name, pv);};};};

    return(alst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_ATTRS_GRAPH - set the values of the specified attributes
 *                    - in the given PG_graph
 */

int PG_set_attrs_graph(PG_graph *g, ...)
   {

    SC_VA_START(g);

    g->info = _PG_set_attrs_alist(g->info, SC_VA_VAR);

    SC_VA_END;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_ATTRS_MAPPING - set the values of the specified attributes
 *                      - in the given PM_mapping
 */

int PG_set_attrs_mapping(PM_mapping *f, ...)
   {

    SC_VA_START(f);

    f->map = _PG_set_attrs_alist(f->map, SC_VA_VAR);

    SC_VA_END;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_ATTRS_SET - set the values of the specified attributes
 *                  - in the given PM_set
 */

int PG_set_attrs_set(PM_set *s, ...)
   {

    SC_VA_START(s);

    s->info = _PG_set_attrs_alist(s->info, SC_VA_VAR);

    SC_VA_END;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_ATTRS_ALIST - set the values of the specified attributes
 *                    - in the given association list
 */

pcons *PG_set_attrs_alist(pcons *alst, ...)
   {

    SC_VA_START(alst);

    alst = _PG_set_attrs_alist(alst, SC_VA_VAR);

    SC_VA_END;

    return(alst);}

/*--------------------------------------------------------------------------*/

/*                             REMOVE ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* _PG_REM_ATTRS_ALIST - remove one or more graphics attributes
 *                     - specifications are quadruples:
 *                          <attr>,
 *                     - where
 *                     -    <attr>    = string naming the attribute
 *                     - return the resulting alist
 */

static pcons *_PG_rem_attrs_alist(pcons *alst, va_list __a__)
   {int i;
    char *name;

    for (i = 0; TRUE; i++)
        {name = SC_VA_ARG(char *);
	 if (name == NULL)
	    break;

	 alst = SC_rem_alist(alst, name);};

    return(alst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REM_ATTRS_GRAPH - remove the values of the specified attributes
 *                    - in the given PG_graph
 */

int PG_rem_attrs_graph(PG_graph *g, ...)
   {

    SC_VA_START(g);

    g->info = _PG_rem_attrs_alist(g->info, SC_VA_VAR);

    SC_VA_END;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REM_ATTRS_MAPPING - remove the values of the specified attributes
 *                      - in the given PM_mapping
 */

int PG_rem_attrs_mapping(PM_mapping *f, ...)
   {

    SC_VA_START(f);

    f->map = _PG_rem_attrs_alist(f->map, SC_VA_VAR);

    SC_VA_END;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REM_ATTRS_SET - remove the values of the specified attributes
 *                  - in the given PM_set
 */

int PG_rem_attrs_set(PM_set *s, ...)
   {

    SC_VA_START(s);

    s->info = _PG_rem_attrs_alist(s->info, SC_VA_VAR);

    SC_VA_END;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REM_ATTRS_ALIST - remove the values of the specified attributes
 *                    - in the given association list
 */

pcons *PG_rem_attrs_alist(pcons *alst, ...)
   {

    SC_VA_START(alst);

    alst = _PG_rem_attrs_alist(alst, SC_VA_VAR);

    SC_VA_END;

    return(alst);}

/*--------------------------------------------------------------------------*/

/*                             STATE ATTRIBUTES                             */

/*--------------------------------------------------------------------------*/

/* PG_SETUP_ATTRS_GLB - one time initialization of global PGS attributes */

void PG_setup_attrs_glb(void)
   {

    if (_PG.attr_table == NULL)
       {_PG.attr_table = SC_make_hasharr(HSZLARGE, NODOC, SC_HA_NAME_KEY);

	_PG_gattrs.axis_char_size        = 1;
	_PG_gattrs.axis_grid_on          = FALSE;
	_PG_gattrs.axis_grid_style       = 3;
	_PG_gattrs.axis_labels           = TRUE;
	_PG_gattrs.axis_line_style       = LINE_SOLID;
	_PG_gattrs.axis_max_major_ticks  = 10;
	_PG_gattrs.axis_n_minor_ticks    = -1;
	_PG_gattrs.axis_n_minor_x_ticks  = -1;
	_PG_gattrs.axis_n_minor_y_ticks  = -1;
	_PG_gattrs.axis_on               = TRUE;
	_PG_gattrs.axis_tick_type        = AXIS_TICK_RIGHT;
	_PG_gattrs.axis_type             = CARTESIAN_2D;
	_PG_gattrs.color_map_to_extrema  = FALSE;
	_PG_gattrs.contour_n_levels      = 10;
	_PG_gattrs.edit_mode             = FALSE;
	_PG_gattrs.hide_rescale          = FALSE;
	_PG_gattrs.hist_start            = 0;
	_PG_gattrs.hl_clear_mode         = CLEAR_SCREEN;
	_PG_gattrs.interp_flag           = TRUE;
	_PG_gattrs.jpg_quality           = 100;
	_PG_gattrs.label_color_flag      = FALSE;
	_PG_gattrs.label_length          = 25;
	_PG_gattrs.label_type_size       = 8;
	_PG_gattrs.line_color            = 1;
	_PG_gattrs.line_style            = LINE_SOLID;
	_PG_gattrs.logical_plot          = FALSE;
	_PG_gattrs.marker_index          = 0;
	_PG_gattrs.marker_scale          = 0.01;
	_PG_gattrs.max_color_factor      = 128;
	_PG_gattrs.numeric_data_id       = FALSE;
	_PG_gattrs.overplot              = FALSE;
	_PG_gattrs.palette_orientation   = HORIZONTAL;
	_PG_gattrs.plot_date             = TRUE;
	_PG_gattrs.plot_labels           = TRUE;
	_PG_gattrs.plot_type             = PLOT_CARTESIAN;
	_PG_gattrs.parallel_graphics     = FALSE;
	_PG_gattrs.parallel_simulate     = FALSE;
	_PG_gattrs.plot_title            = TRUE;
	_PG_gattrs.ref_mesh_color        = 0;
	_PG_gattrs.ref_mesh              = FALSE;
	_PG_gattrs.restore_viewport      = TRUE;
	_PG_gattrs.scatter_plot          = FALSE;
	_PG_gattrs.stroke_raster_text    = TRUE;
	_PG_gattrs.squeeze_labels        = FALSE;
	_PG_gattrs.text_color            = 1;
	_PG_gattrs.use_pixmap            = TRUE;

	_PG_gattrs.axis_char_angle       = 0.0;
	_PG_gattrs.axis_label_x_standoff = 2.0;
	_PG_gattrs.axis_label_y_standoff = 2.0;
	_PG_gattrs.axis_line_width       = 0.1;
	_PG_gattrs.axis_major_tick_size  = 0.015;
	_PG_gattrs.axis_n_decades        = 8.0;
	_PG_gattrs.cgm_text_mag          = 1.6;
	_PG_gattrs.contour_ratio         = 1.0;
	_PG_gattrs.error_cap_size        = 0.018;
	_PG_gattrs.label_position_y      = HUGE;
	_PG_gattrs.label_space           = 0.0;
	_PG_gattrs.label_yoffset         = 0.0;
	_PG_gattrs.line_width            = 0.1;
	_PG_gattrs.palette_width         = 0.02;
	_PG_gattrs.ps_dots_inch          = 600.0;

 	_PG_gattrs.axis_label_x_format   = NULL;
 	_PG_gattrs.axis_label_y_format   = NULL;
	_PG_gattrs.axis_type_face        = NULL;
	_PG_gattrs.text_format           = NULL;

/* strings */
 	ATTR_STR("axis-label-x-format",   &_PG_gattrs.axis_label_x_format);
	ATTR_STR("axis-label-y-format",   &_PG_gattrs.axis_label_y_format);
	ATTR_STR("axis-type-face",        &_PG_gattrs.axis_type_face);
	ATTR_STR("text-format",           &_PG_gattrs.text_format);

/* integers */
	ATTR_INT("axis-char-size",        &_PG_gattrs.axis_char_size);
	ATTR_INT("axis-grid-on",          &_PG_gattrs.axis_grid_on);
	ATTR_INT("axis-grid-style",       &_PG_gattrs.axis_grid_style);
	ATTR_INT("axis-labels",           &_PG_gattrs.axis_labels);
	ATTR_INT("axis-line-style",       &_PG_gattrs.axis_line_style);
	ATTR_INT("axis-max-major-ticks",  &_PG_gattrs.axis_max_major_ticks);
	ATTR_INT("axis-n-minor-ticks",    &_PG_gattrs.axis_n_minor_ticks);
	ATTR_INT("axis-n-minor-x-ticks",  &_PG_gattrs.axis_n_minor_x_ticks);
	ATTR_INT("axis-n-minor-y-ticks",  &_PG_gattrs.axis_n_minor_y_ticks);
	ATTR_INT("axis-on",               &_PG_gattrs.axis_on);
	ATTR_INT("axis-tick-type",        &_PG_gattrs.axis_tick_type);
	ATTR_INT("axis-type",             &_PG_gattrs.axis_type);
	ATTR_INT("color-map-to-extrema",  &_PG_gattrs.color_map_to_extrema);
	ATTR_INT("contour-n-levels",      &_PG_gattrs.contour_n_levels);
	ATTR_INT("edit-mode",             &_PG_gattrs.edit_mode);
	ATTR_INT("hide-rescale",          &_PG_gattrs.hide_rescale);
	ATTR_INT("hist-start",            &_PG_gattrs.hist_start);
	ATTR_INT("hl-clear-mode",         &_PG_gattrs.hl_clear_mode);
	ATTR_INT("interp-flag",           &_PG_gattrs.interp_flag);
	ATTR_INT("jpg-quality",           &_PG_gattrs.jpg_quality);
	ATTR_INT("label-color-flag",      &_PG_gattrs.label_color_flag);
	ATTR_INT("label-length",          &_PG_gattrs.label_length);
	ATTR_INT("label-type-size",       &_PG_gattrs.label_type_size);
	ATTR_INT("line-color",            &_PG_gattrs.line_color);
	ATTR_INT("line-style",            &_PG_gattrs.line_style);
	ATTR_INT("logical-plot",          &_PG_gattrs.logical_plot);
	ATTR_INT("marker-index",          &_PG_gattrs.marker_index);
	ATTR_INT("max-color-factor",      &_PG_gattrs.max_color_factor);
	ATTR_INT("numeric-data-id",       &_PG_gattrs.numeric_data_id);
	ATTR_INT("overplot",              &_PG_gattrs.overplot);
	ATTR_INT("parallel-graphics",     &_PG_gattrs.parallel_graphics);
	ATTR_INT("parallel-simulate",     &_PG_gattrs.parallel_simulate);
	ATTR_INT("palette-orientation",   &_PG_gattrs.palette_orientation);
	ATTR_INT("plot-date",             &_PG_gattrs.plot_date);
	ATTR_INT("plot-labels",           &_PG_gattrs.plot_labels);
	ATTR_INT("plot-title",            &_PG_gattrs.plot_title);
	ATTR_INT("plot-type",             &_PG_gattrs.plot_type);
	ATTR_INT("ref-mesh-color",        &_PG_gattrs.ref_mesh_color);
	ATTR_INT("ref-mesh",              &_PG_gattrs.ref_mesh);
	ATTR_INT("restore-viewport",      &_PG_gattrs.restore_viewport);
	ATTR_INT("scatter-plot",          &_PG_gattrs.scatter_plot);
	ATTR_INT("squeeze-labels",        &_PG_gattrs.squeeze_labels);
	ATTR_INT("stroke-raster-text",    &_PG_gattrs.stroke_raster_text);
	ATTR_INT("text-color",            &_PG_gattrs.text_color);
	ATTR_INT("use-pixmap",            &_PG_gattrs.use_pixmap);

/* doubles */
	ATTR_DBL("axis-char-angle",       &_PG_gattrs.axis_char_angle);
	ATTR_DBL("axis-label-x-standoff", &_PG_gattrs.axis_label_x_standoff);
	ATTR_DBL("axis-label-y-standoff", &_PG_gattrs.axis_label_y_standoff);
	ATTR_DBL("axis-line-width",       &_PG_gattrs.axis_line_width);
	ATTR_DBL("axis-major-tick-size",  &_PG_gattrs.axis_major_tick_size);
	ATTR_DBL("axis-n-decades",        &_PG_gattrs.axis_n_decades);
	ATTR_DBL("cgm-text-mag",          &_PG_gattrs.cgm_text_mag);
	ATTR_DBL("contour-ratio",         &_PG_gattrs.contour_ratio);
	ATTR_DBL("error-cap-size",        &_PG_gattrs.error_cap_size);
	ATTR_DBL("label-position-y",      &_PG_gattrs.label_position_y);
	ATTR_DBL("label-space",           &_PG_gattrs.label_space);
	ATTR_DBL("label-yoffset",         &_PG_gattrs.label_yoffset);
	ATTR_DBL("line-width",            &_PG_gattrs.line_width);
	ATTR_DBL("marker-scale",          &_PG_gattrs.marker_scale);
	ATTR_DBL("palette-width",         &_PG_gattrs.palette_width);
	ATTR_DBL("ps-dots-inch",          &_PG_gattrs.ps_dots_inch);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RL_ATTRS_GLB - one time initialization of global PGS attributes */

void _PG_rl_attrs_glb(void)
   {

    if (_PG.attr_table != NULL)
       {SC_free_hasharr(_PG.attr_table, NULL, NULL);

	_PG.attr_table = NULL;

 	_PG_gattrs.axis_label_x_format = NULL;
 	_PG_gattrs.axis_label_y_format = NULL;
	_PG_gattrs.axis_type_face      = NULL;
	_PG_gattrs.text_format         = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RL_ATTRS_GLB - return a pointer to the attribute table
 *                  - this only intended for the PYTHON wrappers
 *                  - otherwise it totally breaks the hiding discipline
 *                  - no declaration even in pgs_int.h
 */

hasharr *_PG_get_attrs_table(void)
   {

    return(_PG.attr_table);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PTR_ATTR_ENTRY - return a pointer to a global PGS attribute entry */

haelem *PG_ptr_attr_entry(char *name)
   {haelem *hp;

    hp = SC_hasharr_lookup(_PG.attr_table, name);

    return(hp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PTR_ATTR_GLB - return a pointer to a global PGS attribute */

void *PG_ptr_attr_glb(char *name)
   {void *rv;
    haelem *hp;

    rv = NULL;
    hp = SC_hasharr_lookup(_PG.attr_table, name);
    if (hp != NULL)
       rv = hp->def;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_PTR_ATTR_SET - set the value PVO of type TYP to
 *                  - PVI also of type TYP
 */

void _PG_ptr_attr_set(char *typ, void *pvo, void *pvi)
   {

    if (typ == SC_INT_S)
       {int *iv;

	iv  = (int *) pvo;
	*iv = *(int *) pvi;}

    else if (typ == SC_DOUBLE_S)
       {double *dv;

	dv  = (double *) pvo;
	*dv = *(double *) pvi;}

    else if (typ == SC_STRING_S)
       {char **sv;

	sv  = (char **) pvo;
	*sv = *(char **) pvi;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_GET_ATTRS_GLB - get global PGS attributes */

int PG_get_attrs_glb(int dflt, ...)
   {int n;
    char *typ, *name;
    void *pvi, *pvo;
    haelem *hp;

    PG_setup_attrs_glb();

    SC_VA_START(dflt);

    for (n = 0; TRUE; n++)
        {name = SC_VA_ARG(char *);
	 if (name == NULL)
	    break;

	 hp = SC_hasharr_lookup(_PG.attr_table, name);
	 if (hp != NULL)
	    {typ = hp->type;
	     pvi = hp->def;
	     pvo = SC_VA_ARG(void *);
	     _PG_ptr_attr_set(typ, pvo, pvi);};};

    SC_VA_END;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_ATTRS_GLB - set global PGS attributes */

int PG_set_attrs_glb(int dflt, ...)
   {int n, iv;
    double dv;
    char *typ, *name, *sv;
    void *pvi, *pvo;
    haelem *hp;

    PG_setup_attrs_glb();

    SC_VA_START(dflt);

    for (n = 0; TRUE; n++)
        {name = SC_VA_ARG(char *);
	 if (name == NULL)
	    break;

	 hp = SC_hasharr_lookup(_PG.attr_table, name);
	 if (hp == NULL)
	    break;
	 else
	    {typ = hp->type;
	     pvo = hp->def;
	     if (typ == SC_INT_S)
	        {iv  = SC_VA_ARG(int);
		 pvi = &iv;}
	     else if (typ == SC_DOUBLE_S)
	        {dv  = SC_VA_ARG(double);
		 pvi = &dv;}
	     else if (typ == SC_STRING_S)
	        {sv  = SC_VA_ARG(char *);
		 pvi = &sv;};

	     _PG_ptr_attr_set(typ, pvo, pvi);};};

    SC_VA_END;

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
