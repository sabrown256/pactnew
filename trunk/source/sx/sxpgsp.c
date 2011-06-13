/*
 * SXPGSP.C - PGS extensions in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ARGS_ARR_2 - map a list pairwise to numbers */

static void _SX_args_arr_2(SS_psides *si, object *argl,
			   int *pn, double **px, double **py)
   {int i, n;
    double *x, *y;
    object *obj;

    n = SS_length(si, argl) >> 1;

    x = CMAKE_N(double, n);
    y = CMAKE_N(double, n);
    for (i = 0; !SS_nullobjp(argl); i++)
        {if (SS_consp(argl))
            SX_GET_FLOAT_FROM_LIST(si, x[i], argl,
                                   "CAN'T GET X VALUE - _SX_ARGS_ARR_2");
         if (SS_consp(argl))
            SX_GET_FLOAT_FROM_LIST(si, y[i], argl,
                                   "CAN'T GET Y VALUE - _SX_ARGS_ARR_2");};
    *px = x;
    *py = y;
    *pn = n;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ARGS_ARR_3 - map a list triple-wise to numbers */

static void _SX_args_arr_3(SS_psides *si, object *argl, int *pn,
                           double **px, double **py, double **pz)
   {int i, n;
    double *x, *y, *z;
    object *obj;

    n = SS_length(si, argl)/3;

    x = CMAKE_N(double, n);
    y = CMAKE_N(double, n);
    z = CMAKE_N(double, n);
    for (i = 0; !SS_nullobjp(argl); i++)
        {if (SS_consp(argl))
            SX_GET_FLOAT_FROM_LIST(si, x[i], argl,
                                   "CAN'T GET X VALUE - _SX_ARGS_ARR_3");
         if (SS_consp(argl))
            SX_GET_FLOAT_FROM_LIST(si, y[i], argl,
                                   "CAN'T GET Y VALUE - _SX_ARGS_ARR_3");
         if (SS_consp(argl))
            SX_GET_FLOAT_FROM_LIST(si, z[i], argl,
                                   "CAN'T GET Z VALUE - _SX_ARGS_ARR_3");};

    *px = x;
    *py = y;
    *pz = z;
    *pn = n;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_AXIS - draw a complete axis set */

static object *_SXI_axis(SS_psides *si, object *argl)
   {int type;
    PG_device *dev;

    dev  = NULL;
    type = CARTESIAN_2D;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &type,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_AXIS", SS_null);

    PG_axis(dev, type);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_CLEAR_WINDOW - clear the screen */

static object *_SXI_clear_window(SS_psides *si, object *argl)
   {PG_device *dev;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_CLEAR_WINDOW", SS_null);

    PG_clear_window(dev);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_CLEAR_VIEWPORT - clear the viewport */

static object *_SXI_clear_viewport(SS_psides *si, object *argl)
   {PG_device *dev;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_CLEAR_VIEWPORT", SS_null);

    PG_clear_viewport(dev);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SET_CLR_MODE - set the clear mode */

static object *_SXI_set_clr_mode(SS_psides *si, object *argl)
   {int mode;
    PG_device *dev;
    object *rv;

    dev  = NULL;
    mode = CLEAR_SCREEN;
    SS_args(si, argl,
            G_DEVICE, &dev,
	    SC_INT_I, &mode,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SET_CLR_MODE", SS_null);

    PG_fset_clear_mode(mode);

    rv = SS_mk_integer(si, mode);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_CLR_MODE - return the clear mode */

static object *_SXI_clr_mode(SS_psides *si, object *argl)
   {int mode;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_CLR_MODE", SS_null);

    mode = PG_fget_clear_mode();

    o = SS_mk_integer(si, mode);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_CLEAR_REGION - clear the region */

static object *_SXI_clear_region(SS_psides *si, object *argl)
   {int n, nd, pad;
    double ndc[PG_BOXSZ];
    PG_device *dev;

    dev = NULL;
    PG_box_init(3, ndc, 0.0, 1.0);

    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &ndc[0],
            SC_DOUBLE_I, &ndc[1],
            SC_DOUBLE_I, &ndc[2],
            SC_DOUBLE_I, &ndc[3],
            SC_DOUBLE_I, &ndc[4],
            SC_DOUBLE_I, &ndc[5],
            SC_INT_I, &pad,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_CLEAR_REGION", SS_null);

    n = SS_length(si, argl);
    nd = (n - 2) >> 1;

    if (nd == 2)
       pad = ndc[4];

    PG_clear_region(dev, nd, NORMC, ndc, pad);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_CENTER_LABEL - print a label centered in a line */

static object *_SXI_center_label(SS_psides *si, object *argl)
   {PG_device *dev;
    double sy;
    char *label;

    dev   = NULL;
    sy    = 0.0;
    label = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &sy,
            SC_STRING_I, &label,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_CENTER_LABEL", SS_null);

    PG_center_label(dev, sy, label);

    CFREE(label);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DEF_MRK - define a new marker character
 *              - a marker is defined by a set of line segments (4 numbers)
 *              - in the box -1 <= x <= 1 , -1 <= y <= 1
 *              - form (define-marker <x1a> <y1a> <x2a> <y2a> ...)
 */

static object *_SXI_def_mrk(SS_psides *si, object *argl)
   {int i, ns, indx;
    double *x1, *y1, *x2, *y2;
    object *obj;

    ns = SS_length(si, argl) >> 2;

    x1 = CMAKE_N(double, ns);
    y1 = CMAKE_N(double, ns);
    x2 = CMAKE_N(double, ns);
    y2 = CMAKE_N(double, ns);

    for (i = 0; i < ns; i++)
        {SX_GET_FLOAT_FROM_LIST(si, x1[i], argl,
	                        "CAN'T GET FIRST X VALUE - _SXI_DEF_MRK");
         SX_GET_FLOAT_FROM_LIST(si, y1[i], argl,
	                        "CAN'T GET FIRST Y VALUE - _SXI_DEF_MRK");
         SX_GET_FLOAT_FROM_LIST(si, x2[i], argl,
	                        "CAN'T GET SECOND X VALUE - _SXI_DEF_MRK");
         SX_GET_FLOAT_FROM_LIST(si, y2[i], argl,
	                        "CAN'T GET SECOND Y VALUE - _SXI_DEF_MRK");};

    indx = PG_def_marker(ns, x1, y1, x2, y2);

    CFREE(x1);
    CFREE(y1);
    CFREE(x2);
    CFREE(y2);

    obj = SS_mk_integer(si, indx);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRW_MRK - draw markers at the specified points
 *              - form (pg-draw-markers dev marker xlst ylst)
 */

static object *_SXI_drw_mrk(SS_psides *si, object *argl)
   {int i, id, ns, nd, mrk;
    double **r;
    object *x[PG_SPACEDM], *obj;
    PG_coord_sys cs;
    PG_device *dev;

    dev = NULL;
    mrk = 0;
    nd  = 2;
    cs  = WORLDC;
    for (id = 0; id < PG_SPACEDM; id++)
        x[id] = SS_null;

    SS_args(si, argl,
            G_DEVICE, &dev,
	    SC_INT_I, &nd,
	    SC_ENUM_I,    &cs,
	    SC_INT_I, &mrk,
	    SS_OBJECT_I,  &x[0],
	    SS_OBJECT_I,  &x[1],
	    SS_OBJECT_I,  &x[2],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRW_MRK", SS_null);

    ns = SS_length(si, x[0]);

    r = PM_make_vectors(nd, ns);

    for (i = 0; i < ns; i++)
        {for (id = 0; id < nd; id++)
	     {SX_GET_FLOAT_FROM_LIST(si, r[id][i], x[id],
				     "CAN'T GET COMPONENT - _SXI_DRW_MRK");};};

    PG_draw_markers_n(dev, nd, cs, ns, r, mrk);

    PM_free_vectors(nd, r);

    obj = SS_mk_integer(si, ns);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_MRK_ORNT - get the marker orientation */

static object *_SXI_mrk_ornt(SS_psides *si, object *argl)
   {double theta;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_MRK_ORNT", SS_null);

    theta = PG_fget_marker_orientation(dev);
    o     = SS_mk_float(si, theta);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_MRK_SCAL - get the marker scale */

static object *_SXI_mrk_scal(SS_psides *si, object *argl)
   {double sc;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_MRK_SCAL", SS_null);

    sc = PG_fget_marker_scale(dev);
    o  = SS_mk_float(si, sc);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SET_MRK_ORNT - set the marker orientation */

static object *_SXI_set_mrk_ornt(SS_psides *si, object *argl)
   {double theta;
    PG_device *dev;
    object *o;

    dev   = NULL;
    theta = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &theta,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SET_MRK_ORNT", SS_null);

    theta = PG_fset_marker_orientation(dev, theta);
    o     = SS_mk_float(si, theta);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SET_MRK_SCAL - set the marker scale */

static object *_SXI_set_mrk_scal(SS_psides *si, object *argl)
   {double sc;
    PG_device *dev;
    object *o;

    dev = NULL;
    sc  = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &sc,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SET_MRK_SCAL", SS_null);

    sc = PG_fset_marker_scale(dev, sc);
    o  = SS_mk_float(si, sc);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SHOW_MRK - show the current set of markers */

static object *_SXI_show_mrk(SS_psides *si)
   {int i, mi;
    PG_device *dev;
    double b, dx[PG_SPACEDM], x[PG_SPACEDM];
    double *r[PG_SPACEDM];

    dev = PG_make_device("WINDOW", "COLOR", "Markers");
    if (dev != NULL)
       {PG_white_background(dev, TRUE);
	PG_open_device(dev, 0.3, 0.3, 0.1, 0.2);

	PG_get_text_ext_n(dev, 2, WORLDC, "0", dx);
	dx[1] *= 0.5;

	PG_get_attrs_glb(TRUE,
			 "marker-index", &mi,
			 NULL);

	dev->marker_scale *= 3;
	b = 1.0/((double) (mi + 1));
	for (i = 0; i < mi; i++)
	    {x[0] = 0.3;
	     x[1] = b*(i + 1);
	     PG_write_n(dev, 2, WORLDC, x, "%d", i+1);
	     x[0] = 0.7;
	     x[1] += dx[1];
	     r[0] = x;
	     r[1] = x + 1;
	     PG_draw_markers_n(dev, 2, WORLDC, 1, r, i);};

	PRINT(stdout, "Press enter to continue\n");
	SC_pause();

	PG_close_device(dev);};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRAW_ARC - draw a portion of an arc */

static object *_SXI_draw_arc(SS_psides *si, object *argl)
   {int unit;
    double a1, a2, r, x, y;
    PG_device *dev;

    dev = NULL;
    x = y = r = 0.0;
    a1 = a2 = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &r,
            SC_DOUBLE_I, &a1,
            SC_DOUBLE_I, &a2,
            SC_DOUBLE_I, &x,
            SC_DOUBLE_I, &y,
            SC_INT_I, &unit,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_ARC", SS_null);

    if (unit == 1)
       unit = DEGREE;
    else
       unit = RADIAN;

    PG_set_line_color(dev, dev->line_color);
    PG_draw_arc(dev, r, a1, a2, x, y, unit);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRAW_AXIS - draw a axis */

static object *_SXI_draw_axis(SS_psides *si, object *argl)
   {int tt, lt, td;
    double sc;
    double tn[2], vw[2];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];
    char *fmt;
    PG_device *dev;

    dev = NULL;
    xl[0] = xl[1] = tn[0] = vw[0] = 0.0;
    xr[0] = xr[1] = tn[1] = vw[1] = 1.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &xl[0],
            SC_DOUBLE_I, &xl[1],
            SC_DOUBLE_I, &xr[0],
            SC_DOUBLE_I, &xr[1],
            SC_DOUBLE_I, &tn[0],
            SC_DOUBLE_I, &tn[1],
            SC_DOUBLE_I, &vw[0],
            SC_DOUBLE_I, &vw[1],
            SC_DOUBLE_I, &sc,
            SC_STRING_I, &fmt,
            SC_INT_I, &tt,
            SC_INT_I, &lt,
            SC_INT_I, &td,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_AXIS", SS_null);

    PG_draw_axis_n(dev, xl, xr, tn, vw,
		   sc, fmt, tt, lt, FALSE,
		   td, 0);

    CFREE(fmt);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRAW_BOX - draw a box */

static object *_SXI_draw_box(SS_psides *si, object *argl)
   {int n, nd;
    double bx[PG_BOXSZ];
    PG_coord_sys cs;
    PG_device *dev;

    dev = NULL;
    cs  = NORMC;
    PG_box_init(3, bx, 0.0, 1.0);

    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_ENUM_I, &cs,
            SC_DOUBLE_I, &bx[0],
            SC_DOUBLE_I, &bx[1],
            SC_DOUBLE_I, &bx[2],
            SC_DOUBLE_I, &bx[3],
            SC_DOUBLE_I, &bx[4],
            SC_DOUBLE_I, &bx[5],
            0);

    n  = SS_length(si, argl) - 1;
    nd = n >> 1;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_BOX", SS_null);

    PG_set_line_color(dev, dev->line_color);
    PG_draw_box_n(dev, nd, cs, bx);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DDPN - draw a set of disjoint nd line segments */

static object *_SXI_ddpn(SS_psides *si, object *argl)
   {int i, n, nd, clip;
    double *x[PG_SPACEDM];
    PG_coord_sys cs;
    PG_device *dev;

    dev  = NULL;
    nd   = 0;
    clip = TRUE;
    cs   = WORLDC;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &nd,
            SC_ENUM_I,    &cs,
            SC_INT_I, &clip,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DDP2", SS_car(si, argl));

    argl = SS_cddr(si, SS_cddr(si, argl));
    if (nd == 2)
       _SX_args_arr_2(si, argl, &n, &x[0], &x[1]);
    else if (nd == 3)
       _SX_args_arr_3(si, argl, &n, &x[0], &x[1], &x[2]);

    PG_set_line_color(dev, dev->line_color);

    PG_draw_disjoint_polyline_n(dev, nd, cs, (long) n/2, x, clip);

    for (i = 0; i < nd; i++)
        {CFREE(x[i]);};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRAW_LINE - draw a line segment */

static object *_SXI_draw_line(SS_psides *si, object *argl)
   {int n, nd;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    PG_device *dev;

    dev = NULL;
    PM_set_value(x1, PG_SPACEDM, 0.0);
    PM_set_value(x2, PG_SPACEDM, 1.0);
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &x1[0],
            SC_DOUBLE_I, &x1[1],
            SC_DOUBLE_I, &x2[0],
            SC_DOUBLE_I, &x2[1],
            SC_DOUBLE_I, &x1[2],
            SC_DOUBLE_I, &x2[2],
            0);

    n  = SS_length(si, argl) - 1;
    nd = n >> 1;

    if (nd == 3)
       {SC_SWAP_VALUE(double, x1[2], x2[0]);
	SC_SWAP_VALUE(double, x2[1], x2[0]);};

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_LINE", SS_null);

    PG_set_line_color(dev, dev->line_color);
    PG_draw_line_n(dev, nd, WORLDC, x1, x2, dev->clipping);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRAW_PALETTE - draw a palette */

static object *_SXI_draw_palette(SS_psides *si, object *argl)
   {int ex;
    double w;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    PG_device *dev;

    dev = NULL;
    w   = 0.0;
    ex  = FALSE;
    PG_box_init(2, dbx, 0.0, 1.0);
    PG_box_init(1, rbx, 0.0, 1.0);

    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &dbx[0],
            SC_DOUBLE_I, &dbx[1],
            SC_DOUBLE_I, &dbx[2],
            SC_DOUBLE_I, &dbx[3],
            SC_DOUBLE_I, &rbx[0],
            SC_DOUBLE_I, &rbx[1],
            SC_DOUBLE_I, &w,
            SC_INT_I, &ex,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_PALETTE", SS_null);

    PG_draw_palette_n(dev, dbx, rbx, w, ex);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRAW_POLYLINE - draw a set of nd line segments */

static object *_SXI_draw_polyline(SS_psides *si, object *argl)
   {int i, n, nd, clip;
    double *x[PG_SPACEDM];
    PM_polygon *py;
    PG_coord_sys cs;
    PG_device *dev;
    object *o;

    dev  = NULL;
    nd   = 0;
    clip = TRUE;
    cs   = WORLDC;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &nd,
            SC_ENUM_I, &cs,
            SC_INT_I, &clip,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_POLYLINE", SS_car(si, argl));

    argl = SS_cddr(si, SS_cddr(si, argl));
    o    = SS_car(si, argl);
    if (SX_POLYGONP(o))
       {py = SS_GET(PM_polygon, o);
	PG_draw_polyline_n(dev, nd, cs, py->nn, py->x, clip);}

    else
       {if (nd == 2)
	   _SX_args_arr_2(si, argl, &n, &x[0], &x[1]);
        else if (nd == 3)
	   _SX_args_arr_3(si, argl, &n, &x[0], &x[1], &x[2]);

	PG_set_line_color(dev, dev->line_color);

	PG_draw_polyline_n(dev, nd, cs, n, x, clip);

	for (i = 0; i < nd; i++)
	    {CFREE(x[i]);};};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRAW_RAD - draw a radial line segment */

static object *_SXI_draw_rad(SS_psides *si, object *argl)
   {int unit;
    double a, rn, rx, x, y;
    PG_device *dev;

    dev = NULL;
    x = y = 0.0;
    rn = rx = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &rn,
            SC_DOUBLE_I, &rx,
            SC_DOUBLE_I, &a,
            SC_DOUBLE_I, &x,
            SC_DOUBLE_I, &y,
            SC_INT_I, &unit,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_RAD", SS_null);

    PG_set_line_color(dev, dev->line_color);
    PG_draw_rad(dev, rn, rx, a, x, y, unit);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRAW_TEXT - draw a text string */

static object *_SXI_draw_text(SS_psides *si, object *argl)
   {int nd;
    double p[PG_SPACEDM];
    char *txt;
    PG_coord_sys cs;
    PG_device *dev;

    dev  = NULL;
    nd   = 2;
    cs   = NORMC;
    p[0] = 0.0;
    p[1] = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &nd,
            SC_ENUM_I,    &cs,
            SC_DOUBLE_I,  &p[0],
            SC_DOUBLE_I,  &p[1],
            SC_STRING_I,  &txt,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_TEXT_WC", SS_null);

    PG_set_text_color(dev, dev->text_color);
    PG_write_n(dev, nd, cs, p, "%s", txt);

    CFREE(txt);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_FPLY - fill a polygon */

static object *_SXI_fply(SS_psides *si, object *argl)
   {int n, clr;
    double *r[2];
    PM_polygon *py;
    PG_device *dev;
    object *o;

    dev = NULL;
    clr = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &clr,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_FPLY", SS_car(si, argl));

    argl = SS_cddr(si, argl);
    o    = SS_car(si, argl);
    if (SX_POLYGONP(o))
       {py = SS_GET(PM_polygon, o);
	PG_fill_polygon_n(dev, clr, TRUE, 2, WORLDC, py->nn, py->x);}
    else
       {_SX_args_arr_2(si, argl, &n, &r[0], &r[1]);

	PG_fill_polygon_n(dev, clr, TRUE, 2, WORLDC, n, r);

	CFREE(r[0]);
	CFREE(r[1]);};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_FINISH_PLOT - finish the frame */

static object *_SXI_finish_plot(SS_psides *si, object *argl)
   {PG_device *dev;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_FINISH_PLOT", SS_null);

    PG_finish_plot(dev);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GCPW - return the character path */

static object *_SXI_gcpw(SS_psides *si, object *argl)
   {double x[PG_SPACEDM];
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GCPW", SS_null);

    PG_fget_char_path(dev, x);

    o = SS_make_list(si, SC_DOUBLE_I, &x[0],
		     SC_DOUBLE_I, &x[1],
		     0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GCSS - return the character size in NDC */

static object *_SXI_gcss(SS_psides *si, object *argl)
   {double x[PG_SPACEDM];
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GCSS", SS_null);

    PG_fget_char_size_n(dev, 2, NORMC, x);

    o = SS_make_list(si, SC_DOUBLE_I, &x[0],
		     SC_DOUBLE_I, &x[1],
		     0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GCUW - return the character up direction */

static object *_SXI_gcuw(SS_psides *si, object *argl)
   {double x[PG_SPACEDM];
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GCUW", SS_null);

    PG_fget_char_up(dev, x);

    o = SS_make_list(si, SC_DOUBLE_I, &x[0],
		     SC_DOUBLE_I, &x[1],
		     0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GCLP - return the clipping state */

static object *_SXI_gclp(SS_psides *si, object *argl)
   {int clp;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GCLP", SS_null);

    clp = PG_fget_clipping(dev);

    o = clp ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GFIN - report the state of the finished flag */

static object *_SXI_gfin(SS_psides *si, object *argl)
   {int fl;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GFIN", SS_null);

    fl = PG_fget_finish_state(dev);

    o = SS_mk_integer(si, fl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GLNC - get the line color */

static object *_SXI_glnc(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GLNC", SS_null);

    PG_get_line_color(dev, &c);

    o = SS_mk_integer(si, c);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GLOP - get the logical operation */

static object *_SXI_glop(SS_psides *si, object *argl)
   {PG_device *dev;
    PG_logical_operation lop;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GLOP", SS_null);

    lop = PG_fget_logical_op(dev);

    o = SS_mk_integer(si, lop);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GLNS - get the line style */

static object *_SXI_glns(SS_psides *si, object *argl)
   {int s;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GLNS", SS_null);

    s = PG_fget_line_style(dev);
    o = SS_mk_integer(si, s);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GLNW - get the line width */

static object *_SXI_glnw(SS_psides *si, object *argl)
   {double w;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GLNW", SS_null);

    w = PG_fget_line_width(dev);

    o = SS_mk_float(si, w);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GMXI - get the maximum intensities */

static object *_SXI_gmxi(SS_psides *si, object *argl)
   {double i, r, g, b;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GMXI", SS_null);

    i = PG_fget_max_intensity(dev);
    r = PG_fget_max_red_intensity(dev);
    g = PG_fget_max_green_intensity(dev);
    b = PG_fget_max_blue_intensity(dev);

    o = SS_make_list(si, SC_DOUBLE_I, &i,
		     SC_DOUBLE_I, &r,
		     SC_DOUBLE_I, &g,
		     SC_DOUBLE_I, &b,
		     0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GTEW - return the text extent */

static object *_SXI_gtew(SS_psides *si, object *argl)
   {int nd;
    double x[PG_SPACEDM];
    char *s;
    PG_coord_sys cs;
    PG_device *dev;
    object *o;

    nd  = 0;
    cs  = NORMC;
    dev = NULL;
    SS_args(si, argl,
            G_DEVICE,     &dev,
            SC_INT_I, &nd,
            SC_ENUM_I,    &cs,
            SC_STRING_I,  &s,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GTEW", SS_null);

    PG_get_text_ext_n(dev, nd, cs, s, x);

    CFREE(s);

    o = SS_make_list(si, SC_DOUBLE_I, &x[0],
		     SC_DOUBLE_I, &x[1],
		     SC_DOUBLE_I, &x[2],
		     0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GTXC - get the text color */

static object *_SXI_gtxc(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GTXC", SS_null);

    PG_get_text_color(dev, &c);

    o = SS_mk_integer(si, c);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GTXF - get the text font */

static object *_SXI_gtxf(SS_psides *si, object *argl)
   {int size;
    char *face, *style;
    PG_device *dev;
    object *ret;

    dev  = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GTXF", SS_null);

    PG_fget_font(dev, &face, &style, &size);

    ret = SS_make_list(si, SC_STRING_I, face,
                       SC_STRING_I, style,
                       SC_INT_I, &size,
                       0);

    CFREE(face);
    CFREE(style);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GET_VIEWPORT - get the viewport */

static object *_SXI_get_viewport(SS_psides *si, object *argl)
   {PG_device *dev;
    double ndc[PG_BOXSZ];
    object *ret;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GET_VIEWPORT", SS_null);

    PG_get_viewspace(dev, NORMC, ndc);

    ret = SS_make_list(si, SC_DOUBLE_I, &ndc[0],
                       SC_DOUBLE_I, &ndc[1],
                       SC_DOUBLE_I, &ndc[2],
                       SC_DOUBLE_I, &ndc[3],
                       0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GET_FRAME - get the frame */

static object *_SXI_get_frame(SS_psides *si, object *argl)
   {double frm[PG_BOXSZ];
    PG_device *dev;
    object *ret;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GET_FRAME", SS_null);

    PG_get_viewspace(dev, FRAMEC, frm);

    ret = SS_make_list(si, SC_DOUBLE_I, &frm[0],
                       SC_DOUBLE_I, &frm[1],
                       SC_DOUBLE_I, &frm[2],
                       SC_DOUBLE_I, &frm[3],
                       0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GET_WINDOW - get the world coordinate system */

static object *_SXI_get_window(SS_psides *si, object *argl)
   {double wc[PG_BOXSZ];
    PG_device *dev;
    object *ret;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GET_WINDOW", SS_null);

    PG_get_viewspace(dev, WORLDC, wc);

    ret = SS_make_list(si, SC_DOUBLE_I, &wc[0],
                       SC_DOUBLE_I, &wc[1],
                       SC_DOUBLE_I, &wc[2],
                       SC_DOUBLE_I, &wc[3],
                       0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_MDVC - make the device current */

static object *_SXI_mdvc(SS_psides *si, object *argl)
   {PG_device *dev;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_MDVC", SS_null);

    PG_make_device_current(dev);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_PTOS - convert from pixel to screen coordinates */

static object *_SXI_ptos(SS_psides *si, object *argl)
   {double x[PG_SPACEDM];
    PG_device *dev;
    object *rv;

    dev = NULL;
    x[0] = 0.0;
    x[1] = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &x[0],
            SC_DOUBLE_I, &x[1],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_PTOS", SS_null);

    else
       PG_trans_point(dev, 2, PIXELC, x, NORMC, x);

    rv = SS_make_list(si, SC_DOUBLE_I, &x[0],
		      SC_DOUBLE_I, &x[1],
		      0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_QDEV - query the device */

static object *_SXI_qdev(SS_psides *si, object *argl)
   {int dx, dy, nc;
    PG_device *dev;
    object *ret;

    dev  = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_QDEV", SS_null);

    PG_query_device(dev, &dx, &dy, &nc);

    ret = SS_make_list(si, SC_INT_I, &dx,
                       SC_INT_I, &dy,
                       SC_INT_I, &nc,
                       0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_QWIN - query the window */

static object *_SXI_qwin(SS_psides *si, object *argl)
   {int dx, dy, nc;
    PG_device *dev;
    object *ret;

    dev  = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_QWIN", SS_null);

    PG_query_device(dev, &dx, &dy, &nc);

    ret = SS_make_list(si, SC_INT_I, &dx,
                       SC_INT_I, &dy,
                       0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_RDVC - release the device */

static object *_SXI_rdvc(SS_psides *si, object *argl)
   {PG_device *dev;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_RDVC", SS_null);

    PG_release_current_device(dev);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SADM - set the autodomain flag */

static object *_SXI_sadm(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SADM", SS_null);

    PG_turn_autodomain(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SAPL - set the autoplot flag */

static object *_SXI_sapl(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SAPL", SS_null);

    PG_turn_autoplot(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SARN - set the autorange flag */

static object *_SXI_sarn(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SARN", SS_null);

    PG_turn_autorange(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SBWD - set the border width in pixels */

static object *_SXI_sbwd(SS_psides *si, object *argl)
   {int c, oc;
    PG_device *dev;
    object *o;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SBWD", SS_null);

    oc = PG_fset_border_width(dev, c);
    o  = SS_mk_integer(si, oc);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SCPW - set the character path */

static object *_SXI_scpw(SS_psides *si, object *argl)
   {double x[PG_SPACEDM];
    PG_device *dev;

    dev  = NULL;
    x[0] = 0.0;
    x[1] = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &x[0],
            SC_DOUBLE_I, &x[1],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SCPW", SS_null);

    PG_fset_char_path(dev, x);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SCUW - set the character up direction */

static object *_SXI_scuw(SS_psides *si, object *argl)
   {double x[PG_SPACEDM];
    PG_device *dev;

    dev  = NULL;
    x[0] = 0.0;
    x[1] = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &x[0],
            SC_DOUBLE_I, &x[1],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SCUW", SS_null);

    PG_fset_char_path(dev, x);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SCLP - set the clipping state */

static object *_SXI_sclp(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SCLP", SS_null);

    PG_fset_clipping(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SDTI - set the data id flag */

static object *_SXI_sdti(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SDTI", SS_null);

    PG_turn_data_id(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SFIC - set the fill color */

static object *_SXI_sfic(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SFIC", SS_null);

    PG_set_fill_color(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SFIN - set the finish state */

static object *_SXI_sfin(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SFIN", SS_null);

    PG_fset_finish_state(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SGRD - set the grid flag */

static object *_SXI_sgrd(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SGRD", SS_null);

    PG_turn_grid(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SLNC - set the line color */

static object *_SXI_slnc(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SLNC", SS_null);

    PG_set_line_color(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SLOP - set the logical operation */

static object *_SXI_slop(SS_psides *si, object *argl)
   {PG_logical_operation lop;
    PG_device *dev;
    object *o;

    dev = NULL;
    lop = GS_COPY;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_ENUM_I, &lop,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SLOP", SS_null);

    lop = PG_fset_logical_op(dev, lop);
    o   = SS_mk_integer(si, lop);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SLNS - set the line style */

static object *_SXI_slns(SS_psides *si, object *argl)
   {int s, st;
    PG_device *dev;
    object *o;

    dev = NULL;
    s   = LINE_SOLID;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &s,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SLNS", SS_null);

    st = PG_fset_line_style(dev, s);
    o  = SS_mk_integer(si, st);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SLNW - set the line width */

static object *_SXI_slnw(SS_psides *si, object *argl)
   {double w;
    PG_device *dev;
    object *o;

    dev = NULL;
    w   = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &w,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SLNW", SS_null);

    w = PG_fset_line_width(dev, w);
    o = SS_mk_float(si, w);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SMXI - set the maximum intensities */

static object *_SXI_smxi(SS_psides *si, object *argl)
   {PG_device *dev;
    double i, r, g, b;

    i = r = g = b = 1.0;
    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &i,
            SC_DOUBLE_I, &r,
            SC_DOUBLE_I, &g,
            SC_DOUBLE_I, &b,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SMXI", SS_car(si, argl));

    i = max(0.0, i);
    i = min(1.0, i);

    r = max(0.0, r);
    r = min(1.0, r);

    g = max(0.0, g);
    g = min(1.0, g);

    b = max(0.0, b);
    b = min(1.0, b);

    PG_fset_max_intensity(dev, i);
    PG_fset_max_red_intensity(dev, r);
    PG_fset_max_green_intensity(dev, g);
    PG_fset_max_blue_intensity(dev, b);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SPAL - set the current palette */

static object *_SXI_spal(SS_psides *si, object *argl)
   {char *txt;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_STRING_I, &txt,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SPAL", SS_null);

    if (!SX_OK_TO_DRAW(dev))
       o = SS_f;
    else
       o = (PG_set_palette(dev, txt) == NULL) ? SS_f : SS_t;

    CFREE(txt);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SRES_SF - set the resolution scale factor */

static object *_SXI_sres_sf(SS_psides *si, object *argl)
   {int sf, rv;
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &sf,
            0);

    if (dev != NULL)
       rv = PG_fset_res_scale_factor(dev, sf);
    else
       rv = 1;

    o = SS_mk_integer(si, rv);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SSCT - set the scatter flag */

static object *_SXI_ssct(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev != NULL)
       PG_turn_scatter(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SWBK - set the white background flag */

static object *_SXI_swbk(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev != NULL)
       PG_white_background(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_STXC - set the text color */

static object *_SXI_stxc(SS_psides *si, object *argl)
   {int c;
    PG_device *dev;

    dev = NULL;
    c   = 1;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &c,
            0);

    if (dev != NULL)
       PG_set_text_color(dev, c);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_STXF - set the text font */

static object *_SXI_stxf(SS_psides *si, object *argl)
   {int size;
    char *face, *style;
    PG_device *dev;

    dev  = NULL;
    size = 12;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_STRING_I, &face,
            SC_STRING_I, &style,
            SC_INT_I, &size,
            0);

    if (dev != NULL)
       {if (face == NULL)
	   face = CSTRSAVE("helvetica");

	if (style == NULL)
	   style = CSTRSAVE("medium");

	PG_fset_font(dev, face, style, size);};

    CFREE(face);
    CFREE(style);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SET_VIEWPORT - set the viewport */

static object *_SXI_set_viewport(SS_psides *si, object *argl)
   {int n, nd;
    double ndc[PG_BOXSZ];
    PG_device *dev;

    dev = NULL;
    PG_box_init(3, ndc, 0.0, 1.0);

    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &ndc[0],
            SC_DOUBLE_I, &ndc[1],
            SC_DOUBLE_I, &ndc[2],
            SC_DOUBLE_I, &ndc[3],
            SC_DOUBLE_I, &ndc[4],
            SC_DOUBLE_I, &ndc[5],
            0);

    n  = SS_length(si, argl) - 1;
    nd = n >> 1;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SET_VIEWPORT", SS_null);

    PG_set_viewspace(dev, nd, NORMC, ndc);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SET_FRAME - set the frame */

static object *_SXI_set_frame(SS_psides *si, object *argl)
   {int n, nd;
    double frm[PG_BOXSZ];
    PG_device *dev;

    dev = NULL;
    PG_box_init(3, frm, 0.0, 1.0);

    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &frm[0],
            SC_DOUBLE_I, &frm[1],
            SC_DOUBLE_I, &frm[2],
            SC_DOUBLE_I, &frm[3],
            SC_DOUBLE_I, &frm[4],
            SC_DOUBLE_I, &frm[5],
            0);

    n  = SS_length(si, argl) - 1;
    nd = n >> 1;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SET_FRAME", SS_null);

    PG_set_viewspace(dev, nd, FRAMEC, frm);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SET_WINDOW - set the world coordinate system */

static object *_SXI_set_window(SS_psides *si, object *argl)
   {int n, nd;
    double wc[PG_BOXSZ];
    PG_device *dev;

    dev = NULL;
    PG_box_init(3, wc, 0.0, 1.0);

    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &wc[0],
            SC_DOUBLE_I, &wc[1],
            SC_DOUBLE_I, &wc[2],
            SC_DOUBLE_I, &wc[3],
            SC_DOUBLE_I, &wc[4],
            SC_DOUBLE_I, &wc[5],
            0);

    n  = SS_length(si, argl) - 1;
    nd = n >> 1;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SET_WINDOW", SS_null);

    PG_set_viewspace(dev, nd, WORLDC, wc);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_UPDATE_VS - update the view surface with a flush */

static object *_SXI_update_vs(SS_psides *si, object *argl)
   {PG_device *dev;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_UPDATE_VS", SS_null);

    PG_update_vs(dev);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_STOP - convert from screen to pixel coordinates */

static object *_SXI_stop(SS_psides *si, object *argl)
   {long xi[PG_SPACEDM];
    double x[PG_SPACEDM];
    PG_device *dev;
    object *rv;

    dev = NULL;
    x[0] = 0.0;
    x[1] = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &x[0],
            SC_DOUBLE_I, &x[1],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_STOP", SS_null);

    else
       PG_trans_point(dev, 2, NORMC, x, PIXELC, x);

    xi[0] = x[0];
    xi[1] = x[1];

    rv = SS_make_list(si, SC_INT_I, &xi[0],
		      SC_INT_I, &xi[1],
		      0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_STOW - convert from screen to world coordinates */

static object *_SXI_stow(SS_psides *si, object *argl)
   {double x[PG_SPACEDM];
    PG_device *dev;
    object *rv;

    dev = NULL;
    x[0] = 0.0;
    x[1] = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &x[0],
            SC_DOUBLE_I, &x[1],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_STOW", SS_null);
    else
       PG_trans_point(dev, 2, NORMC, x, WORLDC, x);

    rv = SS_make_list(si, SC_DOUBLE_I, &x[0],
		      SC_DOUBLE_I, &x[1],
		      0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_WTOS - convert from world to screen coordinates */

static object *_SXI_wtos(SS_psides *si, object *argl)
   {double x[PG_SPACEDM];
    PG_device *dev;
    object *rv;

    dev = NULL;
    x[0] = 0.0;
    x[1] = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &x[0],
            SC_DOUBLE_I, &x[1],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_WTOS", SS_null);
    else
       PG_trans_point(dev, 2, WORLDC, x, NORMC, x);

    rv = SS_make_list(si, SC_DOUBLE_I, &x[0],
		      SC_DOUBLE_I, &x[1],
		      0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SETVA - set the view angle */

static object *_SXI_setva(SS_psides *si, object *argl)
   {PG_device *dev;

    dev = NULL;
    if (SX_DEVICEP(SS_car(si, argl)) == TRUE)
       {SS_args(si, argl,
		G_DEVICE, &dev,
		0);
        argl = SS_cdr(si, argl);};

    SS_args(si, argl,
	    SC_DOUBLE_I, &SX_theta,
	    SC_DOUBLE_I, &SX_phi,
	    SC_DOUBLE_I, &SX_chi,
	    0);

    if (dev != NULL)
       PG_set_view_angle(dev, SX_theta, SX_phi, SX_chi);

    return(argl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GETVA - get the view angle */

static object *_SXI_getva(SS_psides *si, object *argl)
   {PG_device *dev;
    double theta, phi, chi;
    object *rv;

    dev   = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    rv = SS_f;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GETVA", SS_car(si, argl));

    else
       {PG_get_view_angle(dev, TRUE, &theta, &phi, &chi);
        rv = SS_make_list(si, SC_DOUBLE_I, &theta,
			  SC_DOUBLE_I, &phi,
			  SC_DOUBLE_I, &chi,
			  0);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SETLA - set the light angle */

static object *_SXI_setla(SS_psides *si, object *argl)
   {PG_device *dev;
    double theta, phi;

    dev   = NULL;
    theta = 0.0;
    phi   = 0.0;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_DOUBLE_I, &theta,
            SC_DOUBLE_I, &phi,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SETLA", SS_car(si, argl));

    else
       PG_set_light_angle(dev, theta, phi);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GETLA - get the light angle */

static object *_SXI_getla(SS_psides *si, object *argl)
   {PG_device *dev;
    double theta, phi;
    object *rv;

    dev   = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    rv = SS_f;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GETLA", SS_car(si, argl));

    else
       {PG_get_light_angle(dev, TRUE, &theta, &phi);
        rv = SS_make_list(si, SC_DOUBLE_I, &theta,
			  SC_DOUBLE_I, &phi,
			  0);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/*

SX_plcn(FIXNUM *devid, double *px, double *py, double *pa
                                 double *pl, FIXNUM *pkx, FIXNUM *plx,
                                 FIXNUM *pnl, FIXNUM *pli),
SX_plln(FIXNUM *devid, double *px, double *py
                                 FIXNUM *pn, FIXNUM *pmod, FIXNUM *paxt,
                                 FIXNUM *pcol, double *pwid, FIXNUM *psty,
                                 FIXNUM *psca, FIXNUM *psta, FIXNUM *pl),
SX_plim(FIXNUM *devid, FIXNUM *pnc
                                 char *name, FIXNUM *pnct, char *type,
                                 double *px, double *py, double *pz,
                                 FIXNUM *pk, FIXNUM *pl,
                                 double *pxn, double *pxx, double *pyn,
                                 double *pyx, double *pzn, double *pzx),
SX_plsf(FIXNUM *devid, double *px, double *py, double *pz
                                 FIXNUM *pn, double *pxn, double *pxx, double *pyn,
                                 double *pyx, double *pzn, double *pzx,
                                 FIXNUM *pkx, FIXNUM *plx, double *pth, double *pph,
                                 FIXNUM *ptyp, FIXNUM *pcol, double *pwid,
                                 FIXNUM *psty, FIXNUM *pnc, char *label),
SX_plvc(FIXNUM *devid, double *px, double *py
                                 double *pu, double *pv, FIXNUM *pn),
SX_rvpa(FIXNUM *devid, FIXNUM *vwatid)
SX_svpa(FIXNUM *devid, FIXNUM *pn)
*/

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PAL_LIST - return a list of RGB values for the named palette
 *               - RGB components will be normalized to unity
 */

static object *_SXI_pal_list(SS_psides *si, object *argl)
   {int i, nc;
    object *lst;
    char *name;
    PG_palette *pal;
    PG_device *dev;
    RGB_color_map *cm, cl;
    double imp, red, green, blue;

    name = NULL;
    dev  = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_STRING_I, &name,
            0);

    pal = dev->palettes;
    while (pal != NULL)
       {if (strcmp(name, pal->name) == 0)
           break;

        pal = pal->next;
        if (pal == dev->palettes)
           return(NULL);};

    lst = SS_null;
    if (pal == NULL)
       SS_error(si, "NO SUCH PALETTE FOR DEVICE - _SXI_PAL_LIST", argl);

    else
       {cm = pal->true_colormap;
	nc = pal->n_pal_colors;

	imp = 1.0/((double) MAXPIX);
	for (i = 0; i < nc; i++)
	    {cl = *cm++;
	     red   = imp*cl.red;
	     green = imp*cl.green;
	     blue  = imp*cl.blue;
	     lst   = SS_append(si, SS_make_list(si, SC_DOUBLE_I, &red,
					    SC_DOUBLE_I, &green,
					    SC_DOUBLE_I, &blue,
					    0),
			       lst);};};

    CFREE(name);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_LIST_PAL - install a palette in the given device
 *               - built from the list of RGB values specified
 *               - RGB components must be normalized to unity
 */

static object *_SXI_list_pal(SS_psides *si, object *argl)
   {int i, n_pal_colors, n_dev_colors;
    char *name;
    PG_palette *pal;
    PG_device *dev;
    RGB_color_map *cm, cl;
    object *colors;

    name = NULL;
    dev  = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_STRING_I, &name,
            0);

    colors       = SS_caddr(si, argl);
    n_pal_colors = SS_length(si, colors)/3;
    n_dev_colors = dev->absolute_n_color;

    pal = CMAKE(PG_palette);
    cm  = CMAKE_N(RGB_color_map, n_pal_colors);
    if (cm == NULL)
       return(SS_f);

/* the 256 max is to make it doable at all on a system with
 * 24 bits of color - nobody wants to wait a year for X to
 * allocate 16,000,000 colors
 */
    pal->true_colormap       = cm;
    pal->pseudo_colormap     = NULL;
    pal->max_red_intensity   = dev->max_red_intensity;
    pal->max_green_intensity = dev->max_green_intensity;
    pal->max_blue_intensity  = dev->max_blue_intensity;
    pal->n_pal_colors        = min(n_pal_colors, 256);
    pal->n_dev_colors        = min(n_dev_colors, 256);
    pal->name                = CSTRSAVE(name);

/* compute the color maps */
    for (i = 0; i < n_pal_colors; i++)
        {cl.red   = MAXPIX*SS_FLOAT_VALUE(SS_car(si, colors));
         colors   = SS_cdr(si, colors);
         cl.green = MAXPIX*SS_FLOAT_VALUE(SS_car(si, colors));
         colors   = SS_cdr(si, colors);
         cl.blue  = MAXPIX*SS_FLOAT_VALUE(SS_car(si, colors));
         colors   = SS_cdr(si, colors);

         *cm++    = cl;};

    PG_register_palette(dev, pal, TRUE);

    CFREE(name);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_CURRENT_PAL - return the current palette for the given device */

static object *_SXI_current_pal(SS_psides *si, object *argl)
   {object *obj;
    PG_device *dev;

    dev  = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       obj = SS_null;
    else
       obj = SS_mk_string(si, dev->current_palette->name);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PALS - return a list of palettes available for the given device */

static object *_SXI_pals(SS_psides *si, object *argl)
   {object *lst;
    PG_palette *pal;
    PG_device *dev;
    int first;

    dev  = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    lst   = SS_null;
    first = 0;
    for (pal = dev->palettes; TRUE; pal = pal->next)
        {first += (pal == dev->palettes);
         if (first > 1)
            break;
         lst = SS_mk_cons(si, SS_mk_string(si, pal->name), lst);};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SHOW_PAL - show the available palettes
 *               - a background color flag is optional
 */

static object *_SXI_show_pal(SS_psides *si, object *argl)
   {int wbck;
    PG_device *dev;

    dev  = NULL;
    wbck = TRUE;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_INT_I, &wbck,
            0);

    PG_show_palettes(dev, "WINDOW", wbck);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MK_PAL - graphically construct a palette
 *             - make it the current palette
 */

static object *_SXI_mk_pal(SS_psides *si, object *argl)
   {int ndims, wbck;
    int dims[5];
    char *name;
    PG_device *dev;

    dev  = NULL;
    name  = NULL;
    ndims = 1;
    dims[0] = 16;
    dims[1] = 1;
    dims[2] = 1;
    wbck = TRUE;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_STRING_I, &name,
            SC_INT_I, &wbck,
            SC_INT_I, &ndims,
            SC_INT_I, &dims[0],
            SC_INT_I, &dims[1],
            SC_INT_I, &dims[2],
            0);

    dev->current_palette = PG_make_ndim_palette(dev, name, ndims, 
						dims, wbck);

    CFREE(name);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RD_PAL - read a palette from the specified file
 *             - make it the current palette
 */

static object *_SXI_rd_pal(SS_psides *si, object *argl)
   {char *name;
    PG_device *dev;

    dev  = NULL;
    name = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_STRING_I, &name,
            0);

    dev->current_palette = PG_rd_palette(dev, name);

    CFREE(name);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_WR_PAL - write a palette to the specified file */

static object *_SXI_wr_pal(SS_psides *si, object *argl)
   {char *fname, *pname;
    PG_device *dev;
    PG_palette *pal;

    dev   = NULL;
    pname = NULL;
    fname = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_STRING_I, &pname,
            SC_STRING_I, &fname,
            0);

    pal = PG_get_palette(dev, pname);

    PG_wr_palette(dev, pal, fname);

    CFREE(pname);
    CFREE(fname);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_VECT_ATTR - set vector drawing attributes */

static object *_SXI_set_vect_attr(SS_psides *si, object *argl)
   {int attr;
    double val;
    PG_device *dev;
    object *lst, *o;

    dev  = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    
    for (lst = SS_cdr(si, argl); lst != SS_null; lst = SS_cddr(si, lst))
        {SS_args(si, lst,
                 SC_INT_I, &attr,
                 SC_DOUBLE_I, &val,
                 0);

         switch (attr)
            {case VEC_SCALE     :
             case VEC_ANGLE     :
             case VEC_HEADSIZE  :
             case VEC_FIXSIZE   :
             case VEC_MAXSIZE   :
             case VEC_LINETHICK :
                  PG_set_vec_attr(dev, attr, val, 0);
                  break;

             case VEC_LINESTYLE :
             case VEC_COLOR     :
             case VEC_FIXHEAD   :
                  PG_set_vec_attr(dev, attr, (int) val, 0);
                  break;

             default            : break;};};

    o = SS_cdr(si, argl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SET_COLOR_TYPE - set the color type for devices */

static object *_SXI_set_color_type(SS_psides *si, object *argl)
   {int i;
    char *dev_type, *color;
    PG_device *dev;
    out_device *out;

    dev      = NULL;
    dev_type = NULL;
    color    = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            SC_STRING_I, &dev_type,
            SC_STRING_I, &color,
            0);

    PG_set_color_type(dev, dev_type, color);

/* now do the output devices */
    for (i = 0; i < N_OUTPUT_DEVICES; i++)
        {out = SX_get_device(i);
	 if (out->exist && out->active)
	    {if (strcmp(out->type, color) != 0)
	        {CFREE(out->type);
		 out->type = CSTRSAVE(color);};};};

    CFREE(dev_type);
    CFREE(color);

    return(SS_t);}

/*--------------------------------------------------------------------------*/

/*                            ATTRIBUTE RETRIEVAL                           */

/*--------------------------------------------------------------------------*/
 
/* _SXI_GATGL - wrapper for PG_get_attrs_glb */

static object *_SXI_gatgl(SS_psides *si, object *argl)
   {int n, id;
    char *typ, *name;
    void *pvo;
    haelem *hp;
    object *o, *rv;

    PG_setup_attrs_glb();

    rv = SS_null;
    for (n = 0; !SS_nullobjp(argl); n++)
        {name = NULL;
	 SS_args(si, argl,
		 SC_STRING_I, &name,
		 0);
	 argl = SS_cdr(si, argl);

	 if (name == NULL)
	    break;

	 hp = PG_ptr_attr_entry(name);
	 if (hp == NULL)
	    break;
	 else
	    {typ = hp->type;
	     pvo = hp->def;
	     id  = SC_type_id(typ, FALSE);
	     if (id == SC_INT_I)
	        {o = SS_mk_integer(si, *(int *) pvo);
		 rv = SS_mk_cons(si, o, rv);}
	     else if (id == SC_DOUBLE_I)
	        {o  = SS_mk_float(si, *(double *) pvo);
		 rv = SS_mk_cons(si, o, rv);}
	     else if (id == SC_STRING_I)
	        {o  = SS_mk_string(si, (char *) pvo);
		 rv = SS_mk_cons(si, o, rv);};};

	 CFREE(name);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GET_ATTRS_ALIST - get one or more graphics attribute
 *                     - specifications are pairs:
 *                          <attr>, <ptr>
 *                     - where
 *                     -    <attr> = string naming the attribute
 *                     -             (e.g. "LINE-COLOR")
 *                     -    <ptr>  = TRUE iff the attribute is an array
 *                     - return the resulting list
 */

static object *_SX_get_attrs_alist(SS_psides *si, pcons *alst, object *argl)
   {int i, ptr;
    char t[MAXLINE];
    char *name, *typ;
    pcons *pc;
    object *o, *rv;

    rv = SS_null;
    for (i = 0; !SS_nullobjp(argl); i++)
        {name = NULL;
	 SS_args(si, argl,
		 SC_STRING_I, &name,
		 SC_INT_I, &ptr,
		 0);
	 argl = SS_cddr(si, argl);

	 pc = SC_assoc_entry(alst, name);
	 if (pc == NULL)
	    continue;

	 SC_strncpy(t, MAXLINE, pc->cdr_type, -1);
	 typ = PD_dereference(t);
	 o   = _SS_numtype_to_object(si, typ, pc->cdr, 1);
	 rv  = SS_mk_cons(si, o, rv);

	 CFREE(name);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GATGR - wrapper for PG_get_attrs_graph */

static object *_SXI_gatgr(SS_psides *si, object *argl)
   {PG_graph *g;
    object *rv;

    g = NULL;
    SS_args(si, argl,
	    G_GRAPH, &g,
	    0);
    argl = SS_cdr(si, argl);

    rv = _SX_get_attrs_alist(si, g->info, argl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GATMP - wrapper for PG_get_attrs_mapping */

static object *_SXI_gatmp(SS_psides *si, object *argl)
   {PM_mapping *f;
    object *rv;

    f = NULL;
    SS_args(si, argl,
	    G_MAPPING, &f,
	    0);
    argl = SS_cdr(si, argl);

    rv = _SX_get_attrs_alist(si, f->map, argl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GATST - wrapper for PG_get_attrs_set */

static object *_SXI_gatst(SS_psides *si, object *argl)
   {PM_set *s;
    object *rv;

    s = NULL;
    SS_args(si, argl,
	    G_SET, &s,
	    0);
    argl = SS_cdr(si, argl);

    rv = _SX_get_attrs_alist(si, s->info, argl);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            ATTRIBUTE SETTING                             */

/*--------------------------------------------------------------------------*/
 
/* _SXI_SATGL - wrapper for PG_set_attrs_glb */

static object *_SXI_satgl(SS_psides *si, object *argl)
   {int n, iv, id;
    double dv;
    char *typ, *name, *sv;
    void *pvi, *pvo;
    haelem *hp;

    PG_setup_attrs_glb();

    for (n = 0; !SS_nullobjp(argl); n++)
        {name = NULL;
	 SS_args(si, argl,
		 SC_STRING_I, &name,
		 0);
	 argl = SS_cdr(si, argl);

	 if (name == NULL)
	    break;

	 hp = PG_ptr_attr_entry(name);
	 if (hp == NULL)
	    break;
	 else
	    {typ = hp->type;
	     pvo = hp->def;
	     id  = SC_type_id(typ, FALSE);
	     if (id == SC_INT_I)
	        {iv = 0;
		 SS_args(si, argl,
			 SC_INT_I, &iv,
			 0);
		 argl = SS_cdr(si, argl);
		 pvi  = &iv;}
	     else if (id == SC_DOUBLE_I)
	        {dv = 0.0;
		 SS_args(si, argl,
			 SC_DOUBLE_I, &dv,
			 0);
		 argl = SS_cdr(si, argl);
		 pvi  = &dv;}
	     else if (id == SC_STRING_I)
	        {sv = NULL;
		 SS_args(si, argl,
			 SC_STRING_I, &sv,
			 0);
		 argl = SS_cdr(si, argl);
		 pvi  = &sv;};

	     _PG_ptr_attr_set(typ, pvo, pvi);};

	 CFREE(name);};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SET_ATTRS_ALIST - set one or more graphics attribute
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

static pcons *_SX_set_attrs_alist(SS_psides *si, pcons *alst, object *argl)
   {int i, typ, ptr, iv;
    char *name;
    long lv;
    double dv;
    void *pv;

    for (i = 0; !SS_nullobjp(argl); i++)
        {name = NULL;
	 SS_args(si, argl,
		 SC_STRING_I, &name,
		 SC_INT_I, &typ,
		 SC_INT_I, &ptr,
		 0);
	 argl = SS_cdddr(si, argl);

/* GOTCHA: none of the PTR cases will work because the variables
 * are static - have to dynamically allocate them eventually
 */

/* character types (proper) */
	 if (SC_is_type_char(typ) == TRUE)
	    {SS_args(si, argl,
		     SC_INT_I, &lv,
		     0);
	     argl = SS_cdr(si, argl);
	     if (ptr)
	        {pv   = &lv;
		 alst = SC_change_alist(alst, name, SC_STRING_S, pv);}
	     else
	        {iv = lv;
		 SC_CHANGE_VALUE_ALIST(alst, char, SC_STRING_S,
				       name, iv);};}

/* fixed point types (proper) */
	 else if (SC_is_type_fix(typ) == TRUE)
	    {SS_args(si, argl,
		     SC_LONG_I, &lv,
		     0);
	     argl = SS_cdr(si, argl);
	     if (ptr)
	        {pv   = &lv;
		 alst = SC_change_alist(alst, name, SC_LONG_P_S, pv);}
	     else
	        {SC_CHANGE_VALUE_ALIST(alst, long, SC_LONG_P_S,
				       name, lv);};}

/* floating point types (proper) */
	 else if (SC_is_type_fp(typ) == TRUE)
	    {SS_args(si, argl,
		     SC_DOUBLE_I, &dv,
		     0);
	     argl = SS_cdr(si, argl);
	     if (ptr)
	        {pv   = &dv;
		 alst = SC_change_alist(alst, name, SC_DOUBLE_P_S, pv);}
	     else
	        {SC_CHANGE_VALUE_ALIST(alst, double, SC_DOUBLE_P_S,
				       name, dv);};}

	 else if (typ == SC_POINTER_I)
	    {SS_args(si, argl,
		     SC_INT_I, &lv,
		     0);
	     argl = SS_cdr(si, argl);
	     if (ptr)
	        {pv   = &lv;
		 alst = SC_change_alist(alst, name, SC_POINTER_S, pv);}
	     else
	        {pv = &lv;
		 SC_CHANGE_VALUE_ALIST(alst, void *, SC_POINTER_S,
				       name, pv);};};

	 CFREE(name);};

    return(alst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SATGR - wrapper for PG_set_attrs_graph */

static object *_SXI_satgr(SS_psides *si, object *argl)
   {PG_graph *g;

    g = NULL;
    SS_args(si, argl,
	    G_GRAPH, &g,
	    0);
    argl = SS_cdr(si, argl);

    g->info = _SX_set_attrs_alist(si, g->info, argl);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SATMP - wrapper for PG_set_attrs_mapping */

static object *_SXI_satmp(SS_psides *si, object *argl)
   {PM_mapping *f;

    f = NULL;
    SS_args(si, argl,
	    G_MAPPING, &f,
	    0);
    argl = SS_cdr(si, argl);

    f->map = _SX_set_attrs_alist(si, f->map, argl);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SATST - wrapper for PG_set_attrs_set */

static object *_SXI_satst(SS_psides *si, object *argl)
   {PM_set *s;

    s = NULL;
    SS_args(si, argl,
	    G_SET, &s,
	    0);
    argl = SS_cdr(si, argl);

    s->info = _SX_set_attrs_alist(si, s->info, argl);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GBFSZ - wrapper for PG_fget_buffer_size */

static object *_SXI_gbfsz(SS_psides *si)
   {int64_t sz;
    object *rv;

    sz = PG_fget_buffer_size();
    rv = SS_mk_integer(si, sz);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SBFSZ - wrapper for PG_fset_buffer_size */

static object *_SXI_sbfsz(SS_psides *si, object *argl)
   {int64_t sz, osz;
    object *rv;

    sz = -1;
    SS_args(si, argl,
	    SC_LONG_LONG_I, &sz,
	    0);

    osz = PG_fset_buffer_size(sz);
    rv  = SS_mk_integer(si, osz);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_INSTALL_PGS_PRIMITIVES - install the PGS primitives */

void _SX_install_pgs_primitives(SS_psides *si)
   {

    SS_install(si, "pg-axis",
               "Draw a complete axis set on the given device",
               SS_nargs,
               _SXI_axis, SS_PR_PROC);

    SS_install(si, "pg-character-path",
               "Return a pair specifying the character path direction on the given device",
               SS_nargs,
               _SXI_gcpw, SS_PR_PROC);

    SS_install(si, "pg-character-size-ndc",
               "Return the character size in NDC for the given device",
               SS_nargs,
               _SXI_gcss, SS_PR_PROC);

    SS_install(si, "pg-character-up",
               "Return a pair specifying the character up direction on the given device",
               SS_nargs,
               _SXI_gcuw, SS_PR_PROC);

    SS_install(si, "pg-clear-mode",
               "Return the clear mode",
               SS_nargs,
               _SXI_clr_mode, SS_PR_PROC);

    SS_install(si, "pg-clipping?",
               "Return #t iff clipping is on in the given device",
               SS_nargs,
               _SXI_gclp, SS_PR_PROC);

    SS_install(si, "pg-clear-window",
               "Clear the screen and initialize a frame of the given device",
               SS_nargs,
               _SXI_clear_window, SS_PR_PROC);

    SS_install(si, "pg-clear-viewport",
               "Clear the current viewport of the given device",
               SS_nargs,
               _SXI_clear_viewport, SS_PR_PROC);

    SS_install(si, "pg-clear-region",
               "Clear the specified rectangular region of the given device",
               SS_nargs,
               _SXI_clear_region, SS_PR_PROC);

    SS_install(si, "pg-center-label",
               "Print a label string centered on a line at an NDC height",
               SS_nargs,
               _SXI_center_label, SS_PR_PROC);

    SS_install(si, "pg-current-palette",
               "Return the current palette for the given device",
               SS_nargs,
               _SXI_current_pal, SS_PR_PROC);

    SS_install(si, "pg-define-marker",
               "Define a new marker character",
               SS_nargs,
               _SXI_def_mrk, SS_PR_PROC);

    SS_install(si, "pg-draw-arc",
               "Draw a section of a circular arc on the given device",
               SS_nargs,
               _SXI_draw_arc, SS_PR_PROC);

    SS_install(si, "pg-draw-axis",
               "Draw a single axis on the given device",
               SS_nargs,
               _SXI_draw_axis, SS_PR_PROC);

    SS_install(si, "pg-draw-box",
               "Draw a box on the given device",
               SS_nargs,
               _SXI_draw_box, SS_PR_PROC);

    SS_install(si, "pg-draw-disjoint-polyline-n",
               "Draw a set of disjoint ND line segments",
               SS_nargs,
               _SXI_ddpn, SS_PR_PROC);

    SS_install(si, "pg-draw-line",
               "Draw a line on the given device",
               SS_nargs,
               _SXI_draw_line, SS_PR_PROC);

    SS_install(si, "pg-draw-markers",
               "Draw the specified marker at the given points",
               SS_nargs,
               _SXI_drw_mrk, SS_PR_PROC);

    SS_install(si, "pg-draw-palette",
               "Draw the current palette at the specified place on the given device",
               SS_nargs,
               _SXI_draw_palette, SS_PR_PROC);

    SS_install(si, "pg-draw-polyline-n",
               "Draw a set of connected ND line segments",
               SS_nargs,
               _SXI_draw_polyline, SS_PR_PROC);

    SS_install(si, "pg-draw-radius",
               "Draw a radial line segment on the given device",
               SS_nargs,
               _SXI_draw_rad, SS_PR_PROC);

    SS_install(si, "pg-draw-text",
               "Draw text at the given point on the given device",
               SS_nargs,
               _SXI_draw_text, SS_PR_PROC);

    SS_install(si, "pg-fill-polygon",
               "Draw and fill a polygon on the specified device",
               SS_nargs,
               _SXI_fply, SS_PR_PROC);

    SS_install(si, "pg-finish-plot",
               "Finish the plot frame on the given device",
               SS_nargs,
               _SXI_finish_plot, SS_PR_PROC);

    SS_install(si, "pg-finish-state",
               "Get the finish flag of the given device",
               SS_nargs,
               _SXI_gfin, SS_PR_PROC);

    SS_install(si, "pg-frame",
               "Return a list of numbers defining the extent of the frame in NDC",
               SS_nargs,
               _SXI_get_frame, SS_PR_PROC);

    SS_install(si, "pg-get-attrs-global",
               "Get the global graphics attributes",
               SS_nargs,
               _SXI_gatgl, SS_PR_PROC);

    SS_install(si, "pg-get-attrs-graph",
               "Get the specified attributes for the given graph",
               SS_nargs,
               _SXI_gatgr, SS_PR_PROC);

    SS_install(si, "pg-get-attrs-mapping",
               "Get the specified attributes for the given mapping",
               SS_nargs,
               _SXI_gatmp, SS_PR_PROC);

    SS_install(si, "pg-get-attrs-set",
               "Get the specified attributes for the given set",
               SS_nargs,
               _SXI_gatst, SS_PR_PROC);

    SS_install(si, "pg-get-buffer-size",
               "Get the I/O buffer size for graphics files",
               SS_zargs,
               _SXI_gbfsz, SS_PR_PROC);

    SS_install(si, "pg-line-color",
               "Get the line color on the given device",
               SS_nargs,
               _SXI_glnc, SS_PR_PROC);

    SS_install(si, "pg-logical-op",
               "Get the logical operation on the given device",
               SS_nargs,
               _SXI_glop, SS_PR_PROC);

    SS_install(si, "pg-line-style",
               "Get the line style on the given device",
               SS_nargs,
               _SXI_glns, SS_PR_PROC);

    SS_install(si, "pg-line-width",
               "Get the line width on the given device",
               SS_nargs,
               _SXI_glnw, SS_PR_PROC);

    SS_install(si, "list->pg-palette",
               "Convert a list of values into a palette and register it in the device",
               SS_nargs,
               _SXI_list_pal, SS_PR_PROC);

    SS_install(si, "pg-make-device-current",
               "Make the given device the current device for drawing",
               SS_nargs,
               _SXI_mdvc, SS_PR_PROC);

    SS_install(si, "pg-marker-orientation",
               "Get the orientation angle for marker characters",
               SS_nargs,
               _SXI_mrk_ornt, SS_PR_PROC);

    SS_install(si, "pg-marker-scale",
               "Get the scale for marker characters",
               SS_nargs,
               _SXI_mrk_scal, SS_PR_PROC);

    SS_install(si, "pg-maximum-intensity",
               "Return the maximum fractional intensities for colors (0.0 to 1.0)",
               SS_nargs,
               _SXI_gmxi, SS_PR_PROC);

    SS_install(si, "pg-normalized->pixel",
               "Return a pair containing the PC version of the given NDC point",
               SS_nargs,
               _SXI_stop, SS_PR_PROC);

    SS_install(si, "pg-normalized->world",
               "Return a pair containing the WC version of the given NDC point",
               SS_nargs,
               _SXI_stow, SS_PR_PROC);

    SS_install(si, "pg-palette->list",
               "Return the named palette as a list of values ( ... r g b ... )",
               SS_nargs,
               _SXI_pal_list, SS_PR_PROC);

    SS_install(si, "pg-palettes",
               "Return a list of palettes available for the given device",
               SS_nargs,
               _SXI_pals, SS_PR_PROC);

    SS_install(si, "pg-pixel->normalized",
               "Return a pair containing the NDC version of the given PC point",
               SS_nargs,
               _SXI_ptos, SS_PR_PROC);

    SS_install(si, "pg-query-device",
               "Return a list of device width, height, and number of colors",
               SS_nargs,
               _SXI_qdev, SS_PR_PROC);

    SS_install(si, "pg-query-window",
               "Return a list of window width and height",
               SS_nargs,
               _SXI_qwin, SS_PR_PROC);

    SS_install(si, "pg-release-current-device",
               "Release the given device as the current device",
               SS_nargs,
               _SXI_rdvc, SS_PR_PROC);

    SS_install(si, "pg-set-attrs-global!",
               "Set the global graphics attributes",
               SS_nargs,
               _SXI_satgl, SS_PR_PROC);

    SS_install(si, "pg-set-attrs-graph!",
               "Set the specified attributes for the given graph",
               SS_nargs,
               _SXI_satgr, SS_PR_PROC);

    SS_install(si, "pg-set-attrs-mapping!",
               "Set the specified attributes for the given mapping",
               SS_nargs,
               _SXI_satmp, SS_PR_PROC);

    SS_install(si, "pg-set-attrs-set!",
               "Set the specified attributes for the given set",
               SS_nargs,
               _SXI_satst, SS_PR_PROC);

    SS_install(si, "pg-set-autodomain!",
               "Set the autodomain flag of the given device",
               SS_nargs,
               _SXI_sadm, SS_PR_PROC);

    SS_install(si, "pg-set-autoplot!",
               "Set the autoplot flag of the given device",
               SS_nargs,
               _SXI_sapl, SS_PR_PROC);

    SS_install(si, "pg-set-autorange!",
               "Set the autorange flag of the given device",
               SS_nargs,
               _SXI_sarn, SS_PR_PROC);

    SS_install(si, "pg-set-border-width!",
               "Set the device border width in pixels",
               SS_nargs,
               _SXI_sbwd, SS_PR_PROC);

    SS_install(si, "pg-set-buffer-size!",
               "Set the I/O buffer size for graphics files",
               SS_sargs,
               _SXI_sbfsz, SS_PR_PROC);

    SS_install(si, "pg-set-char-path!",
               "Set the character path direction for the given device",
               SS_nargs,
               _SXI_scpw, SS_PR_PROC);

    SS_install(si, "pg-set-char-up!",
               "Set the character up direction for the given device",
               SS_nargs,
               _SXI_scuw, SS_PR_PROC);

    SS_install(si, "pg-set-clear-mode!",
               "Set the clear mode",
               SS_nargs,
               _SXI_set_clr_mode, SS_PR_PROC);

    SS_install(si, "pg-set-clipping!",
               "Set the clipping state of the given device",
               SS_nargs,
               _SXI_sclp, SS_PR_PROC);

    SS_install(si, "pg-set-data-id-flag!",
               "Set the data-id flag of the given device",
               SS_nargs,
               _SXI_sdti, SS_PR_PROC);

    SS_install(si, "pg-set-fill-color!",
               "Set the fill color on the given device",
               SS_nargs,
               _SXI_sfic, SS_PR_PROC);

    SS_install(si, "pg-set-finish-state!",
               "Set the finish flag of the given device",
               SS_nargs,
               _SXI_sfin, SS_PR_PROC);

    SS_install(si, "pg-set-grid-flag!",
               "Set the grid flag on the given device",
               SS_nargs,
               _SXI_sgrd, SS_PR_PROC);

    SS_install(si, "pg-set-line-color!",
               "Set the line color on the given device",
               SS_nargs,
               _SXI_slnc, SS_PR_PROC);

    SS_install(si, "pg-set-logical-op!",
               "Set the logical operation on the given device",
               SS_nargs,
               _SXI_slop, SS_PR_PROC);

    SS_install(si, "pg-set-line-style!",
               "Set the line style on the given device",
               SS_nargs,
               _SXI_slns, SS_PR_PROC);

    SS_install(si, "pg-set-line-width!",
               "Set the line width on the given device",
               SS_nargs,
               _SXI_slnw, SS_PR_PROC);

    SS_install(si, "pg-set-marker-orientation!",
               "Set the orientation angle for marker characters",
               SS_nargs,
               _SXI_set_mrk_ornt, SS_PR_PROC);

    SS_install(si, "pg-set-marker-scale!",
               "Set the scale for marker characters",
               SS_nargs,
               _SXI_set_mrk_scal, SS_PR_PROC);

    SS_install(si, "pg-set-maximum-intensity!",
               "Set the maximum fractional intensity for colors (0.0 to 1.0) and optionally for RGB too",
               SS_nargs,
               _SXI_smxi, SS_PR_PROC);

    SS_install(si, "pg-set-palette!",
               "Set the palette for the given device",
               SS_nargs,
               _SXI_spal, SS_PR_PROC);

    SS_install(si, "pg-set-resolution-scale-factor!",
               "Set the resolution scale factor for the given device",
               SS_nargs,
               _SXI_sres_sf, SS_PR_PROC);

    SS_install(si, "pg-set-scatter-flag!",
               "Set the scatter plot flag on the given device",
               SS_nargs,
               _SXI_ssct, SS_PR_PROC);

    SS_install(si, "pg-set-text-color!",
               "Set the text color on the given device",
               SS_nargs,
               _SXI_stxc, SS_PR_PROC);

    SS_install(si, "pg-set-text-font!",
               "Set the text type face, type style, and point size on the given device",
               SS_nargs,
               _SXI_stxf, SS_PR_PROC);

    SS_install(si, "pg-set-white-background!",
               "Set the white background flag on the given device",
               SS_nargs,
               _SXI_swbk, SS_PR_PROC);

    SS_install(si, "pg-set-frame!",
               "Set the frame for the given device",
               SS_nargs,
               _SXI_set_frame, SS_PR_PROC);

    SS_install(si, "pg-set-vector-attributes!",
               "Set the drawing properties for vectors",
               SS_nargs,
               _SXI_set_vect_attr, SS_PR_PROC);

    SS_install(si, "pg-set-viewport!",
               "Set the viewport for the given device",
               SS_nargs,
               _SXI_set_viewport, SS_PR_PROC);

    SS_install(si, "pg-set-world-coordinate-system!",
               "Set the world coordinate system for the given device",
               SS_nargs,
               _SXI_set_window, SS_PR_PROC);

    SS_install(si, "pg-show-markers",
               "Show the marker characters",
               SS_zargs,
               _SXI_show_mrk, SS_PR_PROC);

    SS_install(si, "pg-make-palette",
               "Graphically make a palette and make it the current palette",
               SS_nargs,
               _SXI_mk_pal, SS_PR_PROC);

    SS_install(si, "pg-show-palettes",
               "Show the available palettes and make selected palette the current one",
               SS_nargs,
               _SXI_show_pal, SS_PR_PROC);

    SS_install(si, "pg-read-palette",
               "Read a palette from the given file",
               SS_nargs,
               _SXI_rd_pal, SS_PR_PROC);

    SS_install(si, "pg-write-palette",
               "Write a palette to the given file",
               SS_nargs,
               _SXI_wr_pal, SS_PR_PROC);

    SS_install(si, "pg-text-color",
               "Return the current text color on the given device",
               SS_nargs,
               _SXI_gtxc, SS_PR_PROC);

    SS_install(si, "pg-text-extent",
               "Return a pair of numbers specifying the WC extent of the given string",
               SS_nargs,
               _SXI_gtew, SS_PR_PROC);

    SS_install(si, "pg-text-font",
               "Return a list of current font specifications on the given device",
               SS_nargs,
               _SXI_gtxf, SS_PR_PROC);

    SS_install(si, "pg-update-view-surface",
               "Update the view surface for the given device",
               SS_nargs,
               _SXI_update_vs, SS_PR_PROC);

    SS_install(si, "pg-viewport",
               "Return a list of numbers defining the extent of the viewport in NDC",
               SS_nargs,
               _SXI_get_viewport, SS_PR_PROC);

    SS_install(si, "pg-view-angle",
               "Return a list of Euler angles defining the view angle",
               SS_nargs,
               _SXI_getva, SS_PR_PROC);

    SS_install(si, "pg-light-angle",
               "Return a list of Euler angles defining the lighting angle",
               SS_nargs,
               _SXI_getla, SS_PR_PROC);

    SS_install(si, "pg-set-view-angle!",
               "Set the Euler angles defining the view angle",
               SS_nargs,
               _SXI_setva, SS_PR_PROC);

    SS_install(si, "pg-set-light-angle!",
               "Set the Euler angles defining the lighting angle",
               SS_nargs,
               _SXI_setla, SS_PR_PROC);

    SS_install(si, "pg-world-coordinate-system",
               "Return a list of numbers defining the WC system",
               SS_nargs,
               _SXI_get_window, SS_PR_PROC);

    SS_install(si, "pg-world->normalized",
               "Return a pair containing the NDC version of the given WC point",
               SS_nargs,
               _SXI_wtos, SS_PR_PROC);

    SS_install(si, "pg-set-color-type!",
               "Set the color type on the given device",
               SS_nargs,
               _SXI_set_color_type, SS_PR_PROC);
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

