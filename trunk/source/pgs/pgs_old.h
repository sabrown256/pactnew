/*
 * PGS_OLD.H - header for deprecate functionality in PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_PGS_OLD

#include "cpyright.h"

#define PCK_PGS_OLD

#ifndef PGS_3_0_API

/*--------------------------------------------------------------------------*/

/*                            PROCEDURAL MACROS                             */

/*--------------------------------------------------------------------------*/

#define PG_query_screen(d, pdx, pdy, pnc)                                    \
    if ((d) != NULL)                                                         \
       {int _dx[2];                                                          \
        PG_query_screen_n(d, _dx, pnc);                                      \
	pdx = _dx[0];							     \
	pdy = _dx[1];};}

/*--------------------------------------------------------------------------*/

#define PG_fgetc(stream)                                                     \
    (((PG_gs.console != NULL) &&                                             \
      (PG_gs.console->ggetc != NULL)) ?                                      \
     (*PG_gs.console->ggetc)(stream) :                                       \
     EOF)

#define PG_fgets(buffer, maxlen, stream)                                     \
    (((PG_gs.console != NULL) &&                                             \
      (PG_gs.console->ggets != NULL)) ?                                      \
     (*PG_gs.console->ggets)(buffer, maxlen, stream) :                       \
     NULL)

#define PG_puts(bf)                                                          \
    if ((PG_gs.console != NULL) &&                                           \
        (PG_gs.console->gputs != NULL))                                      \
       (*PG_gs.console->gputs)(bf)

/*--------------------------------------------------------------------------*/

#define PG_move_gr_abs(d, x, y)                                              \
    if ((d) != NULL)                                                         \
       {if ((d)->move_gr_abs != NULL)                                        \
           (*(d)->move_gr_abs)(d, x, y);}

#define PG_move_tx_abs(d, x, y)                                              \
    if ((d) != NULL)                                                         \
       {if ((d)->move_tx_abs != NULL)                                        \
           (*(d)->move_tx_abs)(d, x, y);}

#define PG_move_tx_rel(d, x, y)                                              \
    if ((d) != NULL)                                                         \
       {if ((d)->move_tx_rel != NULL)                                        \
           (*(d)->move_tx_rel)(d, x, y);}

#define PG_draw_to_abs(d, x, y)                                              \
    if ((d) != NULL)                                                         \
       {if ((d)->draw_to_abs != NULL)                                        \
           (*(d)->draw_to_abs)(d, x, y);}

#define PG_draw_to_rel(d, x, y)                                              \
    if ((d) != NULL)                                                         \
       {if ((d)->draw_to_rel != NULL)                                        \
           (*(d)->draw_to_rel)(d, x, y);}

/*--------------------------------------------------------------------------*/

#define PG_set_axis_decades(_d) PG_set_attrs_glb(TRUE, "axis-n-decades", (double) _d, NULL)
#define PG_get_axis_decades(_d) PG_get_attrs_glb(TRUE, "axis-n-decades", &(_d), NULL)

#define PG_set_clear_mode(_i)   PG_set_attrs_glb(TRUE, "hl-clear-mode", (int) _i, NULL)
#define PG_get_clear_mode(_i)   PG_get_attrs_glb(TRUE, "hl-clear-mode", &(_i), NULL)

#define PG_set_clipping(d, flag)                                             \
    if ((d) != NULL)                                                         \
       {if ((d)->set_clipping != NULL)                                       \
           (*(d)->set_clipping)(d, flag);}

#define PG_set_char_path(d, x, y)                                            \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_path != NULL)                                      \
           (*(d)->set_char_path)(d, x, y);}

#define PG_set_char_up(d, x, y)                                              \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_up != NULL)                                        \
           (*(d)->set_char_up)(d, x, y);}

#define PG_get_fill_bound(d, v)                                              \
   (v = ((d) != NULL) ?  d->draw_fill_bound : 0)
#define PG_set_fill_bound(d, v)                                              \
   {if ((d) != NULL)                                                         \
       d->draw_fill_bound = (v);}

#define PG_get_finish_state(d, v)                                            \
   (v = ((d) != NULL) ? (d)->finished : 0)
#define PG_set_finish_state(d, v)                                            \
   {if ((d) != NULL)                                                         \
       d->finished = (v);}

#define PG_set_font(d, face, style, size)                                    \
    if ((d) != NULL)                                                         \
       {if ((d)->set_font != NULL)                                           \
           (*(d)->set_font)(d, face, style, size);}

#define PG_get_identifier(g, i)                                              \
   (((g) != NULL) ? i = (g)->identifier : 'A')
#define PG_set_identifier(g, i)                                              \
   {if ((g) != NULL)                                                         \
       (g)->identifier = (i);}

#define PG_set_logical_op(d, lop)                                            \
    if ((d) != NULL)                                                         \
       {if ((d)->set_logical_op != NULL)                                     \
           (*(d)->set_logical_op)(d, lop);}

#define PG_set_line_style(d, style)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->set_line_style != NULL)                                     \
           (*(d)->set_line_style)(d, style);}

#define PG_set_line_width(d, width)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->set_line_width != NULL)                                     \
	   {if (width > 0.0)                                                 \
	       (*(d)->set_line_width)(d, width);                             \
	    else                                                             \
               {double _lw;                                                  \
		PG_get_attrs_glb(TRUE, "line-width", &_lw, NULL);            \
		(*(d)->set_line_width)(d, _lw);};};}

#define PG_get_marker_orientation(d, v)                                      \
   (v = ((d) != NULL) ? (d)->marker_orientation : 0)
#define PG_set_marker_orientation(d, v)                                      \
   {if ((d) != NULL)                                                         \
       (d)->marker_orientation = (v);}

#define PG_get_marker_scale(d, v)                                            \
   (v = ((d) != NULL) ? (d)->marker_scale : 0.0)
#define PG_set_marker_scale(d, v)                                            \
   {if ((d) != NULL)                                                         \
       d->marker_scale = max(v, 0.0);}

#define PG_get_max_intensity(d, v)                                           \
   (v = ((d) != NULL) ? (d)->max_intensity : 0)
#define PG_set_max_intensity(d, v)                                           \
   {if ((d) != NULL)                                                         \
       d->max_intensity = min(v, 1.0);}

#define PG_get_max_red_intensity(d, v)                                       \
   (v = ((d) != NULL) ? (d)->max_red_intensity : 0)
#define PG_set_max_red_intensity(d, v)                                       \
   {if ((d) != NULL)                                                         \
       (d)->max_red_intensity = min(v, 1.0);}

#define PG_get_max_green_intensity(d, v)                                     \
   (v = ((d) != NULL) ? d->max_green_intensity : 0)
#define PG_set_max_green_intensity(d, v)                                     \
   {if ((d) != NULL)                                                         \
       (d)->max_green_intensity = min(v, 1.0);}

#define PG_get_max_blue_intensity(d, v)                                      \
   (v = ((d) != NULL) ? d->max_blue_intensity : 0)
#define PG_set_max_blue_intensity(d, v)                                      \
   {if ((d) != NULL)                                                         \
       (d)->max_blue_intensity = min(v, 1.0);}

#define PG_get_pixmap_flag(d, v)                                             \
   (v = ((d) != NULL) ? (d)->use_pixmap : 0)
#define PG_set_pixmap_flag(d, v)                                             \
   {if ((d) != NULL)                                                         \
       (d)->use_pixmap = v;}

#define PG_get_ps_dots_inch(_d) PG_get_attrs_glb(TRUE, "ps-dots-inch", &(_d), NULL)
#define PG_set_ps_dots_inch(_d) PG_set_attrs_glb(TRUE, "ps-dots-inch", (double) _d, NULL)

#define PG_get_render_info(_g, _a)                                           \
    {_a = NULL;                                                              \
     if (((_g) != NULL) && ((_g)->info_type != NULL))                        \
        {if (strcmp((_g)->info_type, SC_PCONS_P_S) == 0)                     \
	    _a = (pcons *) ((_g)->info);};}
#define PG_set_render_info(g, a)                                             \
   {if ((g) != NULL)                                                         \
       (g)->info = (char *) (a);}

#define PG_get_use_pixmap(_i)   PG_get_attrs_glb(TRUE, "use-pixmap", &(_i), NULL)
#define PG_set_use_pixmap(_i)   PG_set_attrs_glb(TRUE, "use-pixmap", (int) _i, NULL)

/*--------------------------------------------------------------------------*/

#define PG_set_res_scale_factor(d, t)                                        \
   {if ((d) != NULL)                                                         \
       (d)->resolution_scale_factor = (t);}

#define PG_set_border_width(d, t)                                            \
   {if ((d) != NULL)                                                         \
       (d)->border_width = (t);}

#define PG_set_viewport_pos(d, x, y)                                         \
   {if ((d) != NULL)                                                         \
       {(d)->view_x[0] = (x);                                                \
        (d)->view_x[2] = (y);};}

#define PG_set_viewport_shape(d, w, h, a)                                    \
   {if ((d) != NULL)                                                         \
       {(d)->view_x[1]   = (d)->view_x[0] + (w);                             \
        (d)->view_x[3]   = (d)->view_x[2] + (h);                             \
        (d)->view_aspect = (a);};}

#define PG_get_type_size(d, v)                                               \
    {v = -1;                                                                 \
     if ((d) != NULL)                                                        \
        v = (d)->type_size;}

#define PG_get_char(d)                                                       \
    ((((d) != NULL) && ((d)->get_char != NULL)) ? (*(d)->get_char)(d) : '\0')

#define PG_set_line_color(d, color)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->set_line_color != NULL)                                     \
           (*(d)->set_line_color)(d, color, TRUE);}

#define PG_set_color_line(d, color, mapped)                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->set_line_color != NULL)                                     \
           (*(d)->set_line_color)(d, color, mapped);}

#define PG_set_text_color(d, color)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->set_text_color != NULL)                                     \
           (*(d)->set_text_color)(d, color, TRUE);}

#define PG_set_color_text(d, color, mapped)                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->set_text_color != NULL)                                     \
           (*(d)->set_text_color)(d, color, mapped);}

#define PG_set_char_precision(d, p)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_precision != NULL)                                 \
           (*(d)->set_char_precision)(d, p);}

#define PG_set_char_space(d, s)                                              \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_space != NULL)                                     \
           (*(d)->set_char_space)(d, s);}

#define PG_set_char_size_n(d, nd, cs, x)                                     \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_size != NULL)                                      \
           (*(d)->set_char_size)(d, nd, cs, x);}

#define PG_set_char_line(d, n)                                               \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_line != NULL)                                      \
           (*(d)->set_char_line)(d, n);}

#define PG_set_fill_color(d, color)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->set_fill_color != NULL)                                     \
           (*(d)->set_fill_color)(d, color, TRUE);}

#define PG_set_color_fill(d, color, mapped)                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->set_fill_color != NULL)                                     \
           (*(d)->set_fill_color)(d, color, mapped);}

#define PG_shade_poly(d, x, y, n)                                            \
    if ((d) != NULL)                                                         \
       {if ((d)->shade_poly != NULL)                                         \
           {double *_r[2];                                                   \
	    _r[0] = x;                                                       \
	    _r[1] = y;                                                       \
	    (*(d)->shade_poly)(d, 2, n, _r);};}

#define PG_draw_disjoint_polyline_2(d, x, y, n, flag, coord)                 \
    if ((d) != NULL)                                                         \
       {if ((d)->draw_dj_polyln_2 != NULL)                                   \
           {double *_r[2];                                                   \
	    _r[0] = x;                                                       \
	    _r[1] = y;                                                       \
	    (*(d)->draw_dj_polyln_2)(d, _r, n, flag, coord);};}

#define PG_get_image(d, bf, ix, iy, nx, ny)                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->get_image != NULL)                                          \
           (*(d)->get_image)(d, bf, ix, iy, nx, ny);}

#define PG_put_image(d, bf, ix, iy, nx, ny)                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->put_image != NULL)                                          \
           (*(d)->put_image)(d, bf, ix, iy, nx, ny);}

/*--------------------------------------------------------------------------*/

#define PG_get_device_image(_d, _i)                                          \
    ((_i) = ((PG_image *) GET_RST_DEVICE(_d)->fp))

#define PG_put_device_image(_d, _i)                                          \
    (GET_RST_DEVICE(_d)->fp = (FILE *) (_i))

#define PG_set_color_type(dev, dev_type, color)                              \
    if (strcmp(dev_type, "PS") == 0)                                         \
       {CFREE(dev->type);                                                    \
        dev->type = CSTRSAVE(color);                                         \
        if (strcmp(color, "COLOR"))                                          \
	   dev->ps_color = TRUE;                                             \
	else                                                                 \
	   dev->ps_color = FALSE;}

/*--------------------------------------------------------------------------*/

/* GSOLD.C declarations - deprecated */

#define PG_write_abs          PG_write_WC

#define PG_set_char_size_NDC(d, x, y)                                        \
   {double _p[PG_SPACEDM];                                                   \
    _p[0] = x;                                                               \
    _p[1] = y;                                                               \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_size != NULL)                                      \
           (*(d)->set_char_size)(d, 2, NORMC, _p);};}
#define PG_get_text_ext_NDC(d, s, px, py)                                    \
   {double _p[PG_SPACEDM];                                                   \
    _p[0] = 0.0;                                                             \
    _p[1] = 0.0;                                                             \
    if ((d) != NULL)                                                         \
       {if ((d)->get_text_ext != NULL)                                       \
           (*(d)->get_text_ext)(d, 2, NORMC, s, _p);}                        \
    *px = _p[0];                                                             \
    *py = _p[1];}

#define WtoS(_d, _x, _y)                                                     \
   {double _p[PG_SPACEDM];                                                   \
    _p[0] = _x;                                                              \
    _p[1] = _y;                                                              \
    PG_trans_point(_d, 2, WORLDC, _p, NORMC, _p);                            \
    _x = _p[0];                                                              \
    _y = _p[1];}

#define StoW(_d, _x, _y)                                                     \
   {double _p[PG_SPACEDM];                                                   \
    _p[0] = _x;                                                              \
    _p[1] = _y;                                                              \
    PG_trans_point(_d, 2, NORMC, _p, WORLDC, _p);                            \
    _x = _p[0];                                                              \
    _y = _p[1];}

#define StoP(_d, _x, _y, _ix, _iy)                                           \
   {double _p[PG_SPACEDM];                                                   \
    _p[0] = _x;                                                              \
    _p[1] = _y;                                                              \
    PG_trans_point(_d, 2, NORMC, _p, PIXELC, _p);                            \
    _ix = _p[0];                                                             \
    _iy = _p[1];}

#define PtoS(_d, _ix, _iy, _x, _y)                                           \
   {double _p[PG_SPACEDM];                                                   \
    _p[0] = _ix;                                                             \
    _p[1] = _iy;                                                             \
    PG_trans_point(_d, 2, PIXELC, _p, NORMC, _p);                            \
    _x = _p[0];                                                              \
    _y = _p[1];}

#define PG_clear_region_NDC(d, xmn, xmx, ymn, ymx, pad)                      \
    if ((d) != NULL)                                                         \
       {if ((d)->clear_region != NULL)                                       \
           {double _ndc[PG_BOXSZ];                                           \
	    _ndc[0] = xmn;                                                   \
	    _ndc[1] = xmx;                                                   \
	    _ndc[2] = ymn;                                                   \
	    _ndc[3] = ymx;                                                   \
	    (*(d)->clear_region)(d, 2, NORMC, _ndc, pad);};}

/*--------------------------------------------------------------------------*/
 
/*                        TYPEDEF'S AND STRUCT'S                            */
 
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/* GSCLR.C declarations */

extern void
 PG_get_line_color(PG_device *dev, int *pcl),
 PG_get_text_color(PG_device *dev, int *pcl),
 PG_get_fill_color(PG_device *dev, int *pcl);


/* GSIOB.C declarations */

extern PG_interface_object
 *PG_make_interface_object(PG_device *dev,
			   char *name, char *type, void *obj,
			   PM_direction align, double ang,
			   PG_curve *crv, int *flags, int fc, int bc,
			   char *slct, char *draw, char *action,
			   PG_interface_object *parent);


/* GSPR.C declarations */

extern void
 PG_get_clipping(PG_device *dev, int *flag),
 PG_get_char_path(PG_device *dev, double *px, double *py),
 PG_get_char_up(PG_device *dev, double *px, double *py),
 PG_get_char_precision(PG_device *dev, int *pcp),
 PG_get_char_size_n(PG_device *dev, int nd, PG_coord_sys cs, double *p),
 PG_get_char_space(PG_device *dev, double *pcsp),
 PG_get_font(PG_device *dev, char **pf, char **pst, int *psz),
 PG_get_logical_op(PG_device *dev, PG_logical_operation *plop),
 PG_get_line_style(PG_device *dev, int *pls),
 PG_get_line_width(PG_device *dev, double *plw);


/* GSOLD.C declarations */

extern void
 PG_draw_line(PG_device *dev, double x1, double y1, double x2, double y2),
 PG_draw_polyline(PG_device *dev, double *x, double *y, int n, int clip),
 PG_draw_box(PG_device *dev, double xmn, double xmx,
	     double ymn, double ymx),
 PG_draw_box_ndc(PG_device *dev, double xmn, double xmx,
	        double ymn, double ymx),
 PG_get_bound(PG_device *dev,
	      double *xmn, double *xmx, double *ymn, double *ymx),
 PG_get_window(PG_device *dev,
	       double *xmn, double *xmx, double *ymn, double *ymx),
 PG_get_frame(PG_device *dev, double *xmn, double *xmx, 
	      double *ymn, double *ymx),
 PG_get_viewport(PG_device *dev,
		 double *xmn, double *xmx, double *ymn, double *ymx),
 PG_set_bound(PG_device *dev, double xmn, double xmx,
	      double ymn, double ymx),
 PG_set_window(PG_device *dev, double xmn, double xmx,
	       double ymn, double ymx),
 PG_set_frame(PG_device *dev, double x1, double x2, double y1, double y2),
 PG_set_viewport(PG_device *dev, double x1, double x2, double y1, double y2),
 PG_axis_3d(PG_device *dev, double *px, double *py, double *pz,
	    int n_pts, double theta, double phi, double chi,
	    double xmn, double xmx, double ymn, double ymx,
	    double zmn, double zmx, int norm),
 PG_draw_surface(PG_device *dev, double *a1, double *a2, double *aext,
		 double *x, double *y, int nn,
		 double xmn, double xmx, double ymn, double ymx,
		 double *va, double width,
		 int color, int style, int type, char *name,
		 char *mesh_type, void *cnnct, pcons *alist),
 PG_draw_palette(PG_device *dev, double xmn, double ymn,
		 double xmx, double ymx, double zmn, double zmx,
		 double wid),
 PG_get_text_ext(PG_device *dev, char *s, double *px, double *py),
 PG_get_char_size_NDC(PG_device *dev, double *pw, double *ph),
 PG_get_char_size(PG_device *dev, double *pw, double *ph),
 PG_draw_markers(PG_device *dev, int n, double *x, double *y, int marker),
 PG_draw_markers_3(PG_device *dev, int n, double **r, int marker),
 PG_set_limits(PG_device *dev, double *x, double *y, int n, int type),
 PG_fill_polygon(PG_device *dev, int color, int mapped,
		 double *x, double *y, int n);

extern int
 PG_write_NDC(PG_device *dev, double x, double y, char *fmt, ...),
 PG_write_WC(PG_device *dev, double x, double y, char *fmt, ...);

extern PG_image
 *PG_make_image(char *label, char *type, void *z, double xmn, double xmx,
		double ymn, double ymx, double zmn, double zmx,
		int w, int h, int bpp, PG_palette *palette);

extern PG_axis_def
 *PG_draw_axis(PG_device *dev, double xl, double yl, double xr, double yr,
	       double t1, double t2, double v1, double v2,
	       double sc, char *format, int tick_type, int label_type,
	       int flag);

#ifdef __cplusplus
}
#endif

#endif

#endif

