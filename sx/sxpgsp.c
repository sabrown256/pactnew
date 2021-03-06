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
 
/* _SXI_CLEAR_REGION - clear the region */

static object *_SXI_clear_region(SS_psides *si, object *argl)
   {int n, nd, pad;
    double ndc[PG_BOXSZ];
    PG_device *dev;

    dev = NULL;
    PG_box_init(3, ndc, 0.0, 1.0);

    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &ndc[0],
            G_DOUBLE_I, &ndc[1],
            G_DOUBLE_I, &ndc[2],
            G_DOUBLE_I, &ndc[3],
            G_DOUBLE_I, &ndc[4],
            G_DOUBLE_I, &ndc[5],
            G_INT_I, &pad,
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
 
/* _SXI_DEF_MRK - define a new marker character
 *              - a marker is defined by a set of line segments (4 numbers)
 *              - in the box -1 <= x <= 1 , -1 <= y <= 1
 *              - form (define-marker <x1a> <y1a> <x2a> <y2a> ...)
 */

static object *_SXI_def_mrk(SS_psides *si, object *argl)
   {int i, ns, indx;
    double *x1[2], *x2[2];
    object *obj;

    ns = SS_length(si, argl) >> 2;

    x1[0] = CMAKE_N(double, ns);
    x1[1] = CMAKE_N(double, ns);
    x2[0] = CMAKE_N(double, ns);
    x2[1] = CMAKE_N(double, ns);

    for (i = 0; i < ns; i++)
        {SX_GET_FLOAT_FROM_LIST(si, x1[0][i], argl,
	                        "CAN'T GET FIRST X VALUE - _SXI_DEF_MRK");
         SX_GET_FLOAT_FROM_LIST(si, x1[1][i], argl,
	                        "CAN'T GET FIRST Y VALUE - _SXI_DEF_MRK");
         SX_GET_FLOAT_FROM_LIST(si, x2[0][i], argl,
	                        "CAN'T GET SECOND X VALUE - _SXI_DEF_MRK");
         SX_GET_FLOAT_FROM_LIST(si, x2[1][i], argl,
	                        "CAN'T GET SECOND Y VALUE - _SXI_DEF_MRK");};

    indx = PG_def_marker(ns, x1[0], x1[1], x2[0], x2[1]);

    CFREE(x1[0]);
    CFREE(x1[1]);
    CFREE(x2[0]);
    CFREE(x2[1]);

    obj = SS_mk_integer(si, indx);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRW_MRK - draw markers at the specified points
 *              - form (pg-draw-markers dev nd cs marker xlst ylst)
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

#pragma omp parallel for
    for (id = 0; id < PG_SPACEDM; id++)
        x[id] = SS_null;

    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
	    G_INT_I, &nd,
	    G_ENUM_I, &cs,
	    G_INT_I, &mrk,
	    G_OBJECT_I,  &x[0],
	    G_OBJECT_I,  &x[1],
	    G_OBJECT_I,  &x[2],
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
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &xl[0],
            G_DOUBLE_I, &xl[1],
            G_DOUBLE_I, &xr[0],
            G_DOUBLE_I, &xr[1],
            G_DOUBLE_I, &tn[0],
            G_DOUBLE_I, &tn[1],
            G_DOUBLE_I, &vw[0],
            G_DOUBLE_I, &vw[1],
            G_DOUBLE_I, &sc,
            G_STRING_I, &fmt,
            G_INT_I, &tt,
            G_INT_I, &lt,
            G_INT_I, &td,
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
            G_PG_DEVICE_I, &dev,
            G_ENUM_I, &cs,
            G_DOUBLE_I, &bx[0],
            G_DOUBLE_I, &bx[1],
            G_DOUBLE_I, &bx[2],
            G_DOUBLE_I, &bx[3],
            G_DOUBLE_I, &bx[4],
            G_DOUBLE_I, &bx[5],
            0);

    n  = SS_length(si, argl) - 1;
    nd = n >> 1;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_BOX", SS_null);

    PG_fset_line_color(dev, dev->line_color, TRUE);
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
            G_PG_DEVICE_I, &dev,
            G_INT_I, &nd,
            G_ENUM_I, &cs,
            G_INT_I, &clip,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DDPN", SS_car(si, argl));

    n = 0;
    memset(x, 0, sizeof(x));

    argl = SS_cddr(si, SS_cddr(si, argl));
    if (nd == 2)
       _SX_args_arr_2(si, argl, &n, &x[0], &x[1]);
    else if (nd == 3)
       _SX_args_arr_3(si, argl, &n, &x[0], &x[1], &x[2]);

    PG_fset_line_color(dev, dev->line_color, TRUE);

    PG_draw_disjoint_polyline_n(dev, nd, cs, (long) n/2, x, clip);

    for (i = 0; i < nd; i++)
        CFREE(x[i]);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DPPN - project and draw a set of disjoint nd line segments */

static object *_SXI_dppn(SS_psides *si, object *argl)
   {int i, n, nd, clip;
    double *x[PG_SPACEDM];
    PG_coord_sys cs;
    PG_device *dev;

    dev  = NULL;
    nd   = 0;
    clip = TRUE;
    cs   = WORLDC;
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            G_INT_I, &nd,
            G_ENUM_I, &cs,
            G_INT_I, &clip,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DPPN", SS_car(si, argl));

    n = 0;
    memset(x, 0, sizeof(x));

    argl = SS_cddr(si, SS_cddr(si, argl));
    if (nd == 2)
       _SX_args_arr_2(si, argl, &n, &x[0], &x[1]);
    else if (nd == 3)
       _SX_args_arr_3(si, argl, &n, &x[0], &x[1], &x[2]);

    PG_fset_line_color(dev, dev->line_color, TRUE);

    PG_draw_projected_polyline_n(dev, nd, cs, (long) n/2, x, clip);

    for (i = 0; i < nd; i++)
        CFREE(x[i]);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_DRAW_LINE - draw a line segment */

static object *_SXI_draw_line(SS_psides *si, object *argl)
   {int n, nd;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    PG_device *dev;

    dev = NULL;
    PM_array_set(x1, PG_SPACEDM, 0.0);
    PM_array_set(x2, PG_SPACEDM, 1.0);
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &x1[0],
            G_DOUBLE_I, &x1[1],
            G_DOUBLE_I, &x2[0],
            G_DOUBLE_I, &x2[1],
            G_DOUBLE_I, &x1[2],
            G_DOUBLE_I, &x2[2],
            0);

    n  = SS_length(si, argl) - 1;
    nd = n >> 1;

    if (nd == 3)
       {SC_SWAP_VALUE(double, x1[2], x2[0]);
	SC_SWAP_VALUE(double, x2[1], x2[0]);};

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_LINE", SS_null);

    PG_fset_line_color(dev, dev->line_color, TRUE);
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
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &dbx[0],
            G_DOUBLE_I, &dbx[1],
            G_DOUBLE_I, &dbx[2],
            G_DOUBLE_I, &dbx[3],
            G_DOUBLE_I, &rbx[0],
            G_DOUBLE_I, &rbx[1],
            G_DOUBLE_I, &w,
            G_INT_I, &ex,
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
            G_PG_DEVICE_I,  &dev,
            G_INT_I,  &nd,
            G_ENUM_I, &cs,
            G_INT_I,  &clip,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_POLYLINE", SS_car(si, argl));

    argl = SS_cddr(si, SS_cddr(si, argl));
    o    = SS_car(si, argl);
    if (SX_POLYGONP(o))
       {py = SS_GET(PM_polygon, o);
	PG_draw_polyline_n(dev, nd, cs, py->nn, py->x, clip);}

    else
       {n = 0;
	memset(x, 0, sizeof(x));

	if (nd == 2)
	   _SX_args_arr_2(si, argl, &n, &x[0], &x[1]);
        else if (nd == 3)
	   _SX_args_arr_3(si, argl, &n, &x[0], &x[1], &x[2]);

	PG_fset_line_color(dev, dev->line_color, TRUE);

	PG_draw_polyline_n(dev, nd, cs, n, x, clip);

	for (i = 0; i < nd; i++)
	    CFREE(x[i]);};

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
            G_PG_DEVICE_I, &dev,
            G_INT_I, &nd,
            G_ENUM_I,    &cs,
            G_DOUBLE_I,  &p[0],
            G_DOUBLE_I,  &p[1],
            G_STRING_I,  &txt,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_DRAW_TEXT", SS_null);

    PG_fset_text_color(dev, dev->text_color, TRUE);
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
            G_PG_DEVICE_I, &dev,
            G_INT_I, &clr,
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

/* GOTCHA: freed by PM_free_vectors called in PG_fill_polygon_n
	CFREE(r[0]);
	CFREE(r[1]);
 */
        };

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GCSS - return the character size in NDC */

static object *_SXI_gcss(SS_psides *si, object *argl)
   {double x[PG_SPACEDM];
    PG_device *dev;
    object *o;

    dev = NULL;
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GCSS", SS_null);

    PG_fget_char_size_n(dev, 2, NORMC, x);

    o = SS_make_list(si, G_DOUBLE_I, &x[0],
		     G_DOUBLE_I, &x[1],
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
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GCLP", SS_null);

    clp = PG_fget_clipping(dev);

    o = clp ? SS_t : SS_f;

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
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GMXI", SS_null);

    i = PG_fget_max_intensity(dev);
    r = PG_fget_max_red_intensity(dev);
    g = PG_fget_max_green_intensity(dev);
    b = PG_fget_max_blue_intensity(dev);

    o = SS_make_list(si, G_DOUBLE_I, &i,
		     G_DOUBLE_I, &r,
		     G_DOUBLE_I, &g,
		     G_DOUBLE_I, &b,
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
            G_PG_DEVICE_I, &dev,
            G_INT_I, &nd,
            G_ENUM_I, &cs,
            G_STRING_I, &s,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GTEW", SS_null);

    PG_get_text_ext_n(dev, nd, cs, s, x);

    CFREE(s);

    o = SS_make_list(si, G_DOUBLE_I, &x[0],
		     G_DOUBLE_I, &x[1],
		     G_DOUBLE_I, &x[2],
		     0);

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
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GTXF", SS_null);

    PG_fget_font(dev, &face, &style, &size);

    ret = SS_make_list(si, G_STRING_I, face,
                       G_STRING_I, style,
                       G_INT_I, &size,
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
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GET_VIEWPORT", SS_null);

    PG_get_viewspace(dev, NORMC, ndc);

    ret = SS_make_list(si, G_DOUBLE_I, &ndc[0],
                       G_DOUBLE_I, &ndc[1],
                       G_DOUBLE_I, &ndc[2],
                       G_DOUBLE_I, &ndc[3],
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
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GET_FRAME", SS_null);

    PG_get_viewspace(dev, FRAMEC, frm);

    ret = SS_make_list(si, G_DOUBLE_I, &frm[0],
                       G_DOUBLE_I, &frm[1],
                       G_DOUBLE_I, &frm[2],
                       G_DOUBLE_I, &frm[3],
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
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GET_WINDOW", SS_null);

    PG_get_viewspace(dev, WORLDC, wc);

    ret = SS_make_list(si, G_DOUBLE_I, &wc[0],
                       G_DOUBLE_I, &wc[1],
                       G_DOUBLE_I, &wc[2],
                       G_DOUBLE_I, &wc[3],
                       0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_TRANS_BOX - translate a box between coordinate systems */

static object *_SXI_trans_box(SS_psides *si, object *argl)
   {int nd;
    PG_coord_sys ics, ocs;
    double x[PG_BOXSZ];
    PG_device *dev;
    object *rv;

    dev = NULL;
    x[0] = 0.0;
    x[1] = 0.0;
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            G_INT_I, &nd,
            G_ENUM_I, &ics,
            G_ENUM_I, &ocs,
            G_DOUBLE_I, &x[0],
            G_DOUBLE_I, &x[1],
            G_DOUBLE_I, &x[2],
            G_DOUBLE_I, &x[3],
            G_DOUBLE_I, &x[4],
            G_DOUBLE_I, &x[5],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_TRANS_BOX", SS_null);

    else
       PG_trans_box(dev, nd, ics, x, ocs, x);

    rv = SS_make_list(si, G_DOUBLE_I, &x[0],
		      G_DOUBLE_I, &x[1],
		      G_DOUBLE_I, &x[2],
		      G_DOUBLE_I, &x[3],
		      G_DOUBLE_I, &x[4],
		      G_DOUBLE_I, &x[5],
		      0);

    return(rv);}

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
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &x[0],
            G_DOUBLE_I, &x[1],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_PTOS", SS_null);

    else
       PG_trans_point(dev, 2, PIXELC, x, NORMC, x);

    rv = SS_make_list(si, G_DOUBLE_I, &x[0],
		      G_DOUBLE_I, &x[1],
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
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_QDEV", SS_null);

    PG_query_device(dev, &dx, &dy, &nc);

    ret = SS_make_list(si, G_INT_I, &dx,
                       G_INT_I, &dy,
                       G_INT_I, &nc,
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
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_QWIN", SS_null);

    PG_query_device(dev, &dx, &dy, &nc);

    ret = SS_make_list(si, G_INT_I, &dx,
                       G_INT_I, &dy,
                       0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SMXI - set the maximum intensities */

static object *_SXI_smxi(SS_psides *si, object *argl)
   {PG_device *dev;
    double i, r, g, b;

    i = r = g = b = 1.0;
    dev = NULL;
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &i,
            G_DOUBLE_I, &r,
            G_DOUBLE_I, &g,
            G_DOUBLE_I, &b,
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
            G_PG_DEVICE_I, &dev,
            G_STRING_I, &txt,
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SPAL", SS_null);

    if (!SX_OK_TO_DRAW(dev))
       o = SS_f;
    else
       o = (PG_fset_palette(dev, txt) == NULL) ? SS_f : SS_t;

    CFREE(txt);

    return(o);}

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
            G_PG_DEVICE_I, &dev,
            G_STRING_I, &face,
            G_STRING_I, &style,
            G_INT_I, &size,
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
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &ndc[0],
            G_DOUBLE_I, &ndc[1],
            G_DOUBLE_I, &ndc[2],
            G_DOUBLE_I, &ndc[3],
            G_DOUBLE_I, &ndc[4],
            G_DOUBLE_I, &ndc[5],
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
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &frm[0],
            G_DOUBLE_I, &frm[1],
            G_DOUBLE_I, &frm[2],
            G_DOUBLE_I, &frm[3],
            G_DOUBLE_I, &frm[4],
            G_DOUBLE_I, &frm[5],
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
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &wc[0],
            G_DOUBLE_I, &wc[1],
            G_DOUBLE_I, &wc[2],
            G_DOUBLE_I, &wc[3],
            G_DOUBLE_I, &wc[4],
            G_DOUBLE_I, &wc[5],
            0);

    n  = SS_length(si, argl) - 1;
    nd = n >> 1;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_SET_WINDOW", SS_null);

    PG_set_viewspace(dev, nd, WORLDC, wc);

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
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &x[0],
            G_DOUBLE_I, &x[1],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_STOP", SS_null);

    else
       PG_trans_point(dev, 2, NORMC, x, PIXELC, x);

    xi[0] = x[0];
    xi[1] = x[1];

    rv = SS_make_list(si, G_INT_I, &xi[0],
		      G_INT_I, &xi[1],
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
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &x[0],
            G_DOUBLE_I, &x[1],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_STOW", SS_null);
    else
       PG_trans_point(dev, 2, NORMC, x, WORLDC, x);

    rv = SS_make_list(si, G_DOUBLE_I, &x[0],
		      G_DOUBLE_I, &x[1],
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
            G_PG_DEVICE_I, &dev,
            G_DOUBLE_I, &x[0],
            G_DOUBLE_I, &x[1],
            0);

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_WTOS", SS_null);
    else
       PG_trans_point(dev, 2, WORLDC, x, NORMC, x);

    rv = SS_make_list(si,
		      G_DOUBLE_I, &x[0],
		      G_DOUBLE_I, &x[1],
		      0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_GETVA - get the view angle */

static object *_SXI_getva(SS_psides *si, object *argl)
   {PG_device *dev;
    double va[PG_SPACEDM];
    object *rv;

    dev   = NULL;
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            0);

    rv = SS_f;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_GETVA", SS_car(si, argl));

    else
       {PG_fget_view_angle(dev, TRUE, &va[0], &va[1], &va[2]);
        rv = SS_make_list(si,
			  G_DOUBLE_I, &va[0],
			  G_DOUBLE_I, &va[1],
			  G_DOUBLE_I, &va[2],
			  0);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SETVA - set the view angle */

static object *_SXI_setva(SS_psides *si, object *argl)
   {double va[PG_SPACEDM];
    PG_device *dev;

    dev = NULL;
    if (SX_DEVICEP(SS_car(si, argl)) == TRUE)
       {SS_args(si, argl,
		G_PG_DEVICE_I, &dev,
		0);
        argl = SS_cdr(si, argl);};

    SS_args(si, argl,
	    G_DOUBLE_I, &SX_gs.view_angle[0],
	    G_DOUBLE_I, &SX_gs.view_angle[1],
	    G_DOUBLE_I, &SX_gs.view_angle[2],
	    0);

    if (dev != NULL)
       {va[0] = SX_gs.view_angle[0];
	va[1] = SX_gs.view_angle[1];
	va[2] = SX_gs.view_angle[2];
	PG_fset_view_angle(dev, TRUE, &va[0], &va[1], &va[2]);};

    return(argl);}

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
            G_PG_DEVICE_I, &dev,
            G_STRING_I, &name,
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
	     lst   = SS_append(si, SS_make_list(si, G_DOUBLE_I, &red,
					    G_DOUBLE_I, &green,
					    G_DOUBLE_I, &blue,
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
            G_PG_DEVICE_I, &dev,
            G_STRING_I, &name,
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
            G_PG_DEVICE_I, &dev,
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
            G_PG_DEVICE_I, &dev,
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
            G_PG_DEVICE_I, &dev,
            G_STRING_I, &name,
            G_INT_I, &wbck,
            G_INT_I, &ndims,
            G_INT_I, &dims[0],
            G_INT_I, &dims[1],
            G_INT_I, &dims[2],
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
            G_PG_DEVICE_I, &dev,
            G_STRING_I, &name,
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
            G_PG_DEVICE_I, &dev,
            G_STRING_I, &pname,
            G_STRING_I, &fname,
            0);

    pal = PG_fget_palette(dev, pname);

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
            G_PG_DEVICE_I, &dev,
            0);

    
    for (lst = SS_cdr(si, argl); lst != SS_null; lst = SS_cddr(si, lst))
        {SS_args(si, lst,
                 G_INT_I, &attr,
                 G_DOUBLE_I, &val,
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
            G_PG_DEVICE_I, &dev,
            G_STRING_I, &dev_type,
            G_STRING_I, &color,
            0);

    PG_set_color_type(dev, dev_type, color);

/* now do the output devices */
    for (i = 0; i < N_OUTPUT_DEVICES; i++)
        {out = SX_get_device(i);
	 if (out->exist && out->active)
	    {if ((out->type == NULL) || (strcmp(out->type, color) != 0))
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
		 G_STRING_I, &name,
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
	     if (id == G_INT_I)
	        {o = SS_mk_integer(si, *(int *) pvo);
		 rv = SS_mk_cons(si, o, rv);}
	     else if (id == G_DOUBLE_I)
	        {o  = SS_mk_float(si, *(double *) pvo);
		 rv = SS_mk_cons(si, o, rv);}
	     else if (id == G_STRING_I)
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
		 G_STRING_I, &name,
		 G_INT_I, &ptr,
		 0);
	 argl = SS_cddr(si, argl);

	 pc = SC_assoc_entry(alst, name);
	 if (pc == NULL)
	    continue;

	 SC_strncpy(t, MAXLINE, pc->cdr_type, -1);
	 typ = PD_dereference(t);
	 o   = _SS_numtype_to_object(si, typ, pc->cdr, 0);
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
	    G_PG_GRAPH_I, &g,
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
	    G_PM_MAPPING_I, &f,
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
	    G_PM_SET_I, &s,
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
		 G_STRING_I, &name,
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
	     pvi = NULL;
	     id  = SC_type_id(typ, FALSE);
	     if (id == G_INT_I)
	        {iv = 0;
		 SS_args(si, argl,
			 G_INT_I, &iv,
			 0);
		 argl = SS_cdr(si, argl);
		 pvi  = &iv;}
	     else if (id == G_DOUBLE_I)
	        {dv = 0.0;
		 SS_args(si, argl,
			 G_DOUBLE_I, &dv,
			 0);
		 argl = SS_cdr(si, argl);
		 pvi  = &dv;}
	     else if (id == G_STRING_I)
	        {sv = NULL;
		 SS_args(si, argl,
			 G_STRING_I, &sv,
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
		 G_STRING_I, &name,
		 G_INT_I, &typ,
		 G_INT_I, &ptr,
		 0);
	 argl = SS_cdddr(si, argl);

/* GOTCHA: none of the PTR cases will work because the variables
 * are static - have to dynamically allocate them eventually
 */

/* character types (proper) */
	 if (SC_is_type_char(typ) == TRUE)
	    {SS_args(si, argl,
		     G_INT_I, &lv,
		     0);
	     argl = SS_cdr(si, argl);
	     if (ptr)
	        {pv   = &lv;
		 alst = SC_change_alist(alst, name, G_STRING_S, pv);}
	     else
	        {iv = lv;
		 SC_CHANGE_VALUE_ALIST(alst, char, G_STRING_S,
				       name, iv);};}

/* fixed point types (proper) */
	 else if (SC_is_type_fix(typ) == TRUE)
	    {SS_args(si, argl,
		     G_LONG_I, &lv,
		     0);
	     argl = SS_cdr(si, argl);
	     if (ptr)
	        {pv   = &lv;
		 alst = SC_change_alist(alst, name, G_LONG_P_S, pv);}
	     else
	        {SC_CHANGE_VALUE_ALIST(alst, long, G_LONG_P_S,
				       name, lv);};}

/* floating point types (proper) */
	 else if (SC_is_type_fp(typ) == TRUE)
	    {SS_args(si, argl,
		     G_DOUBLE_I, &dv,
		     0);
	     argl = SS_cdr(si, argl);
	     if (ptr)
	        {pv   = &dv;
		 alst = SC_change_alist(alst, name, G_DOUBLE_P_S, pv);}
	     else
	        {SC_CHANGE_VALUE_ALIST(alst, double, G_DOUBLE_P_S,
				       name, dv);};}

	 else if (typ == G_POINTER_I)
	    {SS_args(si, argl,
		     G_INT_I, &lv,
		     0);
	     argl = SS_cdr(si, argl);
	     if (ptr)
	        {pv   = &lv;
		 alst = SC_change_alist(alst, name, G_POINTER_S, pv);}
	     else
	        {pv = &lv;
		 SC_CHANGE_VALUE_ALIST(alst, void *, G_POINTER_S,
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
	    G_PG_GRAPH_I, &g,
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
	    G_PM_MAPPING_I, &f,
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
	    G_PM_SET_I, &s,
	    0);
    argl = SS_cdr(si, argl);

    s->info = _SX_set_attrs_alist(si, s->info, argl);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SXI_SLOGSC - wrapper for PG_fset_log_scale */

static object *_SXI_slogsc(SS_psides *si, object *argl)
   {PG_device *dev;
    int lsc[PG_SPACEDM];

    dev    = NULL;
    lsc[0] = 0;
    lsc[1] = 0;
    SS_args(si, argl,
	    G_PG_DEVICE_I, &dev,
	    G_INT_I, &lsc[0],
	    G_INT_I, &lsc[1],
	    0);

    PG_fset_axis_log_scale(dev, 2, lsc);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_INSTALL_PGS_PRIMITIVES - install the PGS primitives */

void _SX_install_pgs_primitives(SS_psides *si)
   {

    SS_install(si, "pg-character-size-ndc",
               "Return the character size in NDC for the given device",
               SS_nargs,
               _SXI_gcss, SS_PR_PROC);

    SS_install(si, "pg-clipping?",
               "Return #t iff clipping is on in the given device",
               SS_nargs,
               _SXI_gclp, SS_PR_PROC);

    SS_install(si, "pg-clear-region",
               "Clear the specified rectangular region of the given device",
               SS_nargs,
               _SXI_clear_region, SS_PR_PROC);

    SS_install(si, "pg-current-palette",
               "Return the current palette for the given device",
               SS_nargs,
               _SXI_current_pal, SS_PR_PROC);

    SS_install(si, "pg-define-marker",
               "Define a new marker character",
               SS_nargs,
               _SXI_def_mrk, SS_PR_PROC);

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

    SS_install(si, "pg-draw-projected-polyline-n",
               "Project and draw a set of disjoint ND line segments",
               SS_nargs,
               _SXI_dppn, SS_PR_PROC);

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

    SS_install(si, "pg-draw-text",
               "Draw text at the given point on the given device",
               SS_nargs,
               _SXI_draw_text, SS_PR_PROC);

    SS_install(si, "pg-fill-polygon",
               "Draw and fill a polygon on the specified device",
               SS_nargs,
               _SXI_fply, SS_PR_PROC);

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

    SS_install(si, "list->pg-palette",
               "Convert a list of values into a palette and register it in the device",
               SS_nargs,
               _SXI_list_pal, SS_PR_PROC);

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

    SS_install(si, "pg-set-log-scale!",
               "Set the log scale attributes for the given device",
               SS_nargs,
               _SXI_slogsc, SS_PR_PROC);

    SS_install(si, "pg-set-maximum-intensity!",
               "Set the maximum fractional intensity for colors (0.0 to 1.0) and optionally for RGB too",
               SS_nargs,
               _SXI_smxi, SS_PR_PROC);

    SS_install(si, "pg-set-palette!",
               "Set the palette for the given device",
               SS_nargs,
               _SXI_spal, SS_PR_PROC);

    SS_install(si, "pg-set-text-font!",
               "Set the text type face, type style, and point size on the given device",
               SS_nargs,
               _SXI_stxf, SS_PR_PROC);

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

    SS_install(si, "pg-read-palette",
               "Read a palette from the given file",
               SS_nargs,
               _SXI_rd_pal, SS_PR_PROC);

    SS_install(si, "pg-write-palette",
               "Write a palette to the given file",
               SS_nargs,
               _SXI_wr_pal, SS_PR_PROC);

    SS_install(si, "pg-text-extent",
               "Return a pair of numbers specifying the WC extent of the given string",
               SS_nargs,
               _SXI_gtew, SS_PR_PROC);

    SS_install(si, "pg-text-font",
               "Return a list of current font specifications on the given device",
               SS_nargs,
               _SXI_gtxf, SS_PR_PROC);

    SS_install(si, "pg-trans-box",
               "Return the given box in input coordinate system in output coordinate system",
               SS_nargs,
               _SXI_trans_box, SS_PR_PROC);

    SS_install(si, "pg-viewport",
               "Return a list of numbers defining the extent of the viewport in NDC",
               SS_nargs,
               _SXI_get_viewport, SS_PR_PROC);

    SS_install(si, "pg-view-angle",
               "Return a list of Euler angles defining the view angle",
               SS_nargs,
               _SXI_getva, SS_PR_PROC);

    SS_install(si, "pg-set-view-angle!",
               "Set the Euler angles defining the view angle",
               SS_nargs,
               _SXI_setva, SS_PR_PROC);

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

