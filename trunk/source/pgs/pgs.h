/*
 * PGS.H - header in support of Portable Graphics System
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_PGS

#include "cpyright.h"

#define PCK_PGS                               /* graphics package resident */

#define PGS_TOL 1.0e-10

#include "pdb.h"

#ifndef PACT_INCLUDES

# include <pdb.h>

#endif

#undef PACT_INCLUDES

#ifdef HAVE_OGL
# ifndef HAVE_X11
#  define HAVE_X11
# endif
# undef SIZEOF
# include <GL/glx.h>
# include <GL/gl.h>
# undef SIZEOF
# define SIZEOF (*SC_sizeof_hook)
#endif

#include "scope_x11.h"

/*--------------------------------------------------------------------------*/

/*                            DEFINED CONSTANTS                             */

/*--------------------------------------------------------------------------*/

#define PG_SPACEDM            3         /* maximum dimension of view space */
#define PG_BOXSZ              6         /* 2*PG_SPACEDM */
#define PG_NANGLES            3         /* number of Euler angles */

#define N_COLORS              16
#define N_IOB_FLAGS           3
#define N_ANGLES              180

#define TXSPAN                80.0

#define PG_IMAGE_VERSION      0

#ifndef MAXPIX
# define MAXPIX 65535
#endif

/*--------------------------------------------------------------------------*/

/*                            PROCEDURAL MACROS                             */

/*--------------------------------------------------------------------------*/

/* COLOR_POSTSCRIPT_DEVICE - TRUE iff dev is a color PostScript device */

#define COLOR_POSTSCRIPT_DEVICE(dev)                                         \
    (dev->ps_color == TRUE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MONOCHROME_POSTSCRIPT_DEVICE - TRUE iff dev is a monochrome PostScript device */

#define MONOCHROME_POSTSCRIPT_DEVICE(dev)                                    \
    (dev->ps_color == FALSE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POSTSCRIPT_DEVICE - TRUE iff dev is a PostScript device */

#define POSTSCRIPT_DEVICE(dev) (dev->type_index == PS_DEVICE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CGM_DEVICE - TRUE iff dev is a CGM device */

#define CGM_DEVICE(dev) (dev->type_index == CGMF_DEVICE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HARDCOPY_DEVICE - TRUE iff dev is a hard copy device */

#define HARDCOPY_DEVICE(dev) (dev->hard_copy_device)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COLOR_MAP - set up values for the 16 most common colors */

#define Color_Map(dev, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)       \
    dev->BLACK        = a;                                                   \
    dev->WHITE        = b;                                                   \
    dev->GRAY         = c;                                                   \
    dev->DARK_GRAY    = d;                                                   \
    dev->BLUE         = e;                                                   \
    dev->GREEN        = f;                                                   \
    dev->CYAN         = g;                                                   \
    dev->RED          = h;                                                   \
    dev->MAGENTA      = i;                                                   \
    dev->BROWN        = j;                                                   \
    dev->DARK_BLUE    = k;                                                   \
    dev->DARK_GREEN   = l;                                                   \
    dev->DARK_CYAN    = m;                                                   \
    dev->DARK_RED     = n;                                                   \
    dev->YELLOW       = o;                                                   \
    dev->DARK_MAGENTA = p

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_CALL_HANDLER - call the registered event handler with language
 *                   appropriate arguments
 */

#define PG_CALL_HANDLER(hndler, dvice, evnt)                                 \
    {if ((hndler).lang == _C_LANG)                                           \
       {(*(hndler).fnc)(dvice, &evnt);}                                      \
    else if ((hndler).lang == _FORTRAN_LANG)                                 \
       {PFFMouseHand ffcn = (PFFMouseHand) (hndler).fnc;                     \
        FIXNUM fdvice, fevnt;                                                \
        fdvice = SC_GET_INDEX(dvice);                                        \
        fevnt  = SC_ADD_POINTER(&evnt);                                      \
        (*ffcn)(&fdvice, &fevnt);}}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_ADJUST_LIMITS_3D - adjust limits for 3d plots and axes */

#define PG_ADJUST_LIMITS_3D(_bi, _bo)                                        \
    {int _i;                                                                 \
     double _d;                                                              \
     for (_i = 0; _i < 6; _i += 2)                                           \
         {_d = (_bi[_i+1] - _bi[_i])*TOLERANCE;                              \
          _bo[_i]   = _bi[_i] - _d;                                          \
	  _bo[_i+1] = _bi[_i+1] + _d;};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_WITH_CLIP - gather simple clipping info while copying points */

#define PG_SET_WITH_CLIP_INIT(_ok, _os)                                      \
   {_ok = FALSE;                                                             \
    _os = -2;}

#define PG_SET_WITH_CLIP(_xo, _xi, _ok, _os, _mn, _mx)                       \
   {int _s;                                                                  \
    double _v;                                                               \
    _v = _xi;                                                                \
    if (!_ok)                                                                \
       {if (_v < _mn)                                                        \
	   _s = -1;                                                          \
        else if (_mx < _v)                                                   \
	   _s = 1;                                                           \
	else                                                                 \
	   _s = 0;                                                           \
	_ok |= ((_s == 0) || ((_os != -2) && (_s != _os)));                  \
	_os  = _s;};                                                         \
    _xo = _v;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_MAPPING_INFO - extract info for Logical-Rectangular mappings */

#define LR_MAPPING_INFO(alst, npts)                                          \
   {emap   = NULL;                                                           \
    SC_assoc_info(alst,                                                      \
		  "EXISTENCE", &emap,                                        \
		  NULL);                                                     \
    eflag  = (emap == NULL);                                                 \
    if (eflag)                                                               \
       {emap = FMAKE_N(char, npts, "LR_MAPPING_INFO:emap");                  \
	memset(emap, 1, npts);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_PUSH_CHILD_IOB - add a child interface object to a parent's list */

#define PG_PUSH_CHILD_IOB(par, chld)                                         \
   {PG_interface_object *piob;                                               \
    piob = (par);                                                            \
    SC_array_push(piob->children, &chld);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef EVENT_DEFINED
# define PG_EVENT_TYPE(ev)   (ev).type
#endif

#define KEY_ACTION(b, bf, nc) PG_key_action(b, bf, nc)

#define PG_IO_INTERRUPTS(x)   SC_io_interrupts_on = (x)

#define PG_clear_screen       PG_clear_window

#define PG_set_axis_decades(_d) PG_set_attrs_glb(TRUE, "axis-n-decades", (double) _d, NULL)
#define PG_get_axis_decades(_d) PG_get_attrs_glb(TRUE, "axis-n-decades", &(_d), NULL)

#define PG_set_clear_mode(_i)   PG_set_attrs_glb(TRUE, "hl-clear-mode", (int) _i, NULL)
#define PG_get_clear_mode(_i)   PG_get_attrs_glb(TRUE, "hl-clear-mode", &(_i), NULL)

#define PG_set_ps_dots_inch(_d) PG_set_attrs_glb(TRUE, "ps-dots-inch", (double) _d, NULL)
#define PG_get_ps_dots_inch(_d) PG_get_attrs_glb(TRUE, "ps-dots-inch", &(_d), NULL)

#define PG_set_use_pixmap(_i)   PG_set_attrs_glb(TRUE, "use-pixmap", (int) _i, NULL)
#define PG_get_use_pixmap(_i)   PG_get_attrs_glb(TRUE, "use-pixmap", &(_i), NULL)


#define PG_set_render_info(g, a)                                             \
   {if ((g) != NULL)                                                         \
       (g)->info = (char *) (a);}
#define PG_get_render_info(_g, _a)                                           \
    {_a = NULL;                                                              \
     if (((_g) != NULL) && ((_g)->info_type != NULL))                        \
        {if (strcmp((_g)->info_type, SC_PCONS_P_S) == 0)                     \
	    _a = (pcons *) ((_g)->info);};}

#define PG_set_identifier(g, i)                                              \
   {if ((g) != NULL)                                                         \
       (g)->identifier = (i);}
#define PG_get_identifier(g, i)                                              \
   (((g) != NULL) ? i = (g)->identifier : 'A')

#define PG_white_background(d, t)                                            \
   {if ((d) != NULL)                                                         \
       (d)->background_color_white = (t);}
#define PG_set_res_scale_factor(d, t)                                        \
   {if ((d) != NULL)                                                         \
       (d)->resolution_scale_factor = (t);}
#define PG_set_border_width(d, t)                                            \
   {if ((d) != NULL)                                                         \
       (d)->border_width = (t);}

#define PG_turn_grid(d, t)                                                   \
   {if ((d) != NULL)                                                         \
       (d)->grid = (t);}
#define PG_turn_data_id(d, t)                                                \
   {if ((d) != NULL)                                                         \
       (d)->data_id = (t);}
#define PG_turn_scatter(d, t)                                                \
   {if ((d) != NULL)                                                         \
       (d)->scatter = (t);}
#define PG_turn_autodomain(d, t)                                             \
   {if ((d) != NULL)                                                         \
       (d)->autodomain = (t);}
#define PG_turn_autorange(d, t)                                              \
   {if ((d) != NULL)                                                         \
       (d)->autorange = (t);}
#define PG_turn_autoplot(d, t)                                               \
   {if ((d) != NULL)                                                         \
       (d)->autoplot = (t);}

#define PG_set_viewport_pos(d, x, y)                                         \
   {if ((d) != NULL)                                                         \
       {(d)->view_x[0] = (x);                                                \
        (d)->view_x[2] = (y);};}

#define PG_set_viewport_shape(d, w, h, a)                                    \
   {if ((d) != NULL)                                                         \
       {(d)->view_x[1]   = (d)->view_x[0] + (w);                             \
        (d)->view_x[3]   = (d)->view_x[2] + (h);                             \
        (d)->view_aspect = (a);};}

#define PG_clear_frame(d)                                                    \
   {if ((d) != NULL)                                                         \
       PG_clear_region(d, 2, NORMC, d->g.fr, 0);}

#define PG_set_fill_bound(d, v)                                              \
   {if ((d) != NULL)                                                         \
       d->draw_fill_bound = (v);}
#define PG_get_fill_bound(d, v)                                              \
   (v = ((d) != NULL) ?  d->draw_fill_bound : 0)

#define PG_set_finish_state(d, v)                                            \
   {if ((d) != NULL)                                                         \
       d->finished = (v);}
#define PG_get_finish_state(d, v)                                            \
   (v = ((d) != NULL) ? (d)->finished : 0)

#define PG_set_marker_scale(d, v)                                            \
   {if ((d) != NULL)                                                         \
       d->marker_scale = max(v, 0.0);}
#define PG_get_marker_scale(d, v)                                            \
   (v = ((d) != NULL) ? (d)->marker_scale : 0.0)
#define PG_set_marker_orientation(d, v)                                      \
   {if ((d) != NULL)                                                         \
       (d)->marker_orientation = (v);}
#define PG_get_marker_orientation(d, v)                                      \
   (v = ((d) != NULL) ? (d)->marker_orientation : 0)

#define PG_set_max_intensity(d, v)                                           \
   {if ((d) != NULL)                                                         \
       d->max_intensity = min(v, 1.0);}
#define PG_get_max_intensity(d, v)                                           \
   (v = ((d) != NULL) ? (d)->max_intensity : 0)

#define PG_set_max_red_intensity(d, v)                                       \
   {if ((d) != NULL)                                                         \
       (d)->max_red_intensity = min(v, 1.0);}
#define PG_get_max_red_intensity(d, v)                                       \
   (v = ((d) != NULL) ? (d)->max_red_intensity : 0)

#define PG_set_max_green_intensity(d, v)                                     \
   {if ((d) != NULL)                                                         \
       (d)->max_green_intensity = min(v, 1.0);}
#define PG_get_max_green_intensity(d, v)                                     \
   (v = ((d) != NULL) ? d->max_green_intensity : 0)

#define PG_set_max_blue_intensity(d, v)                                      \
   {if ((d) != NULL)                                                         \
       (d)->max_blue_intensity = min(v, 1.0);}
#define PG_get_max_blue_intensity(d, v)                                      \
   (v = ((d) != NULL) ? d->max_blue_intensity : 0)

#define PG_set_pixmap_flag(d, v)                                             \
   {if ((d) != NULL)                                                         \
       (d)->use_pixmap = v;}
#define PG_get_pixmap_flag(d, v)                                             \
   (v = ((d) != NULL) ? (d)->use_pixmap : 0)

#define PG_get_type_size(d, v)                                               \
    {v = -1;                                                                 \
     if ((d) != NULL)                                                        \
        v = (d)->type_size;}

#define PG_n_events_pending(d)                                               \
    (((d) != NULL) ? (*((d)->events_pending))(d) : 0)
#define PG_handle_expose_event(d, ev)                                        \
    {if ((d) != NULL)                                                        \
        (*((d)->expose_event_handler.fnc))(d, &ev);}
#define PG_handle_update_event(d, ev)                                        \
    {if ((d) != NULL)                                                        \
        (*((d)->update_event_handler.fnc))(d, &ev);}
#define PG_handle_mouse_down_event(d, ev)                                    \
    {if ((d) != NULL)                                                        \
        (*((d)->mouse_down_event_handler.fnc))(d, &ev);}
#define PG_handle_mouse_up_event(d, ev)                                      \
    {if ((d) != NULL)                                                        \
        (*((d)->mouse_up_event_handler.fnc))(d, &ev);}
#define PG_handle_key_down_event(d, ev)                                      \
    {if ((d) != NULL)                                                        \
        (*((d)->key_down_event_handler.fnc))(d, &ev);}
#define PG_handle_key_up_event(d, ev)                                        \
    {if ((d) != NULL)                                                        \
        (*((d)->key_up_event_handler.fnc))(d, &ev);}
#define PG_handle_default_event(d, ev)                                       \
    {if ((d) != NULL)                                                        \
        (*((d)->default_event_handler.fnc))(d, &ev);}

#define PG_query_pointer(d, ir, pb, pq)                                      \
    if ((d)->query_pointer != NULL)                                          \
       (*(d)->query_pointer)(d, ir, pb, pq)
#define PG_mouse_event_info(d, ev, iev, eb, eq)                              \
    if ((d)->mouse_event_info != NULL)                                       \
       (*(d)->mouse_event_info)(d, ev, iev, eb, eq)
#define PG_key_event_info(d, ev, iev, bf, n, eq)                             \
    if ((d)->key_event_info != NULL)                                         \
       (*(d)->key_event_info)(d, ev, iev, bf, n, eq)

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
#define PG_set_line_width(d, width)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->set_line_width != NULL)                                     \
	   {if (width > 0.0)                                                 \
	       (*(d)->set_line_width)(d, width);                             \
	    else                                                             \
               {double _lw;                                                  \
		PG_get_attrs_glb(TRUE, "line-width", &_lw, NULL);            \
		(*(d)->set_line_width)(d, _lw);};};}
#define PG_set_char_precision(d, p)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_precision != NULL)                                 \
           (*(d)->set_char_precision)(d, p);}
#define PG_set_char_space(d, s)                                              \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_space != NULL)                                     \
           (*(d)->set_char_space)(d, s);}
#define PG_set_char_up(d, x, y)                                              \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_up != NULL)                                        \
           (*(d)->set_char_up)(d, x, y);}
#define PG_set_char_size_n(d, nd, cs, x)                                     \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_size != NULL)                                      \
           (*(d)->set_char_size)(d, nd, cs, x);}
#define PG_set_char_path(d, x, y)                                            \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_path != NULL)                                      \
           (*(d)->set_char_path)(d, x, y);}
#define PG_set_font(d, face, style, size)                                    \
    if ((d) != NULL)                                                         \
       {if ((d)->set_font != NULL)                                           \
           (*(d)->set_font)(d, face, style, size);}
#define PG_set_char_line(d, n)                                               \
    if ((d) != NULL)                                                         \
       {if ((d)->set_char_line != NULL)                                      \
           (*(d)->set_char_line)(d, n);}
#define PG_set_clipping(d, flag)                                             \
    if ((d) != NULL)                                                         \
       {if ((d)->set_clipping != NULL)                                       \
           (*(d)->set_clipping)(d, flag);}
#define PG_shade_poly(d, x, y, n)                                            \
    if ((d) != NULL)                                                         \
       {if ((d)->shade_poly != NULL)                                         \
           {double *_r[2];                                                   \
	    _r[0] = x;                                                       \
	    _r[1] = y;                                                       \
	    (*(d)->shade_poly)(d, 2, n, _r);};}
#define PG_shade_poly_n(d, nd, n, r)                                         \
    if ((d) != NULL)                                                         \
       {if ((d)->shade_poly != NULL)                                         \
           (*(d)->shade_poly)(d, nd, n, r);}
#define PG_fill_curve(d, c)                                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->fill_curve != NULL)                                         \
           (*(d)->fill_curve)(d, c);}
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
#define PG_set_line_style(d, style)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->set_line_style != NULL)                                     \
           (*(d)->set_line_style)(d, style);}
#define PG_set_logical_op(d, lop)                                            \
    if ((d) != NULL)                                                         \
       {if ((d)->set_logical_op != NULL)                                     \
           (*(d)->set_logical_op)(d, lop);}
#define PG_set_fill_color(d, color)                                          \
    if ((d) != NULL)                                                         \
       {if ((d)->set_fill_color != NULL)                                     \
           (*(d)->set_fill_color)(d, color, TRUE);}
#define PG_set_color_fill(d, color, mapped)                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->set_fill_color != NULL)                                     \
           (*(d)->set_fill_color)(d, color, mapped);}
#define PG_draw_curve(d, c, clip)                                            \
    if ((d) != NULL)                                                         \
       {if ((d)->draw_curve != NULL)                                         \
           (*(d)->draw_curve)(d, c, clip);}
#define PG_draw_disjoint_polyline_2(d, x, y, n, flag, coord)                 \
    if ((d) != NULL)                                                         \
       {if ((d)->draw_dj_polyln_2 != NULL)                                   \
           {double *_r[2];                                                   \
	    _r[0] = x;                                                       \
	    _r[1] = y;                                                       \
	    (*(d)->draw_dj_polyln_2)(d, _r, n, flag, coord);};}
#define PG_clear_window(d)                                                   \
    if ((d) != NULL)                                                         \
       {if ((d)->clear_window != NULL)                                       \
	   {PG_set_attrs_glb(TRUE, "label-position-y", HUGE, NULL);          \
	    (*(d)->clear_window)(d);};}
#define PG_clear_viewport(d)                                                 \
    if ((d) != NULL)                                                         \
       {if ((d)->clear_viewport != NULL)                                     \
           (*(d)->clear_viewport)(d);}
#define PG_clear_region(d, nd, cs, bx, pad)                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->clear_region != NULL)                                       \
	   (*(d)->clear_region)(d, nd, cs, bx, pad);}
#define PG_update_vs(d)                                                      \
    if ((d) != NULL)                                                         \
       {if ((d)->update_vs != NULL)                                          \
           (*(d)->update_vs)(d);}
#define PG_finish_plot(d)                                                    \
    if ((d) != NULL)                                                         \
       {if ((d)->finish_plot != NULL)                                        \
           (*(d)->finish_plot)(d);}
#define PG_clear_page(d, i)                                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->clear_page != NULL)                                         \
           (*(d)->clear_page)(d, i);}
#define PG_expose_device(d)                                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->expose_device != NULL)                                      \
           (*(d)->expose_device)(d);}
#define PG_make_palette_current(d, p)                                        \
    if ((d) != NULL)                                                         \
       {(d)->current_palette = p;}
#define PG_make_device_current(d)                                            \
    if ((d) != NULL)                                                         \
       {if ((d)->make_device_current != NULL)                                \
           (*(d)->make_device_current)(d);}
#define PG_release_current_device(d)                                         \
    if ((d) != NULL)                                                         \
       {if ((d)->release_current_device != NULL)                             \
           (*(d)->release_current_device)(d);}
#define PG_get_image(d, bf, ix, iy, nx, ny)                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->get_image != NULL)                                          \
           (*(d)->get_image)(d, bf, ix, iy, nx, ny);}
#define PG_put_image(d, bf, ix, iy, nx, ny)                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->put_image != NULL)                                          \
           (*(d)->put_image)(d, bf, ix, iy, nx, ny);}
#define PG_write_text(d, fp, s)                                              \
    if ((d) != NULL)                                                         \
       {if ((d)->write_text != NULL)                                         \
           (*(d)->write_text)(d, fp, s);}
#define PG_next_line(d, fp)                                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->next_line != NULL)                                          \
           (*(d)->next_line)(d, fp);}
#define PG_open_screen(d, xf, yf, dxf, dyf)                                  \
    if ((d) != NULL)                                                         \
       {if ((d)->open_screen != NULL)                                        \
           (*(d)->open_screen)(d, xf, yf, dxf, dyf);}
#define PG_query_screen(d, pdx, pdy, pnc)                                    \
    if ((d) != NULL)                                                         \
       {if ((d)->query_screen != NULL)                                       \
           (*(d)->query_screen)(d, pdx, pdy, pnc);}
#define PG_close_device(d)                                                   \
    if ((d) != NULL)                                                         \
       {if ((d)->close_device != NULL)                                       \
           (*(d)->close_device)(d);}

#define PG_close_console                                                     \
    if ((PG_console_device != NULL) &&                                       \
        (PG_console_device->close_console != NULL))                          \
       (*PG_console_device->close_console)

#define PG_fgets(buffer, maxlen, stream)                                     \
    (((PG_console_device != NULL) && (PG_console_device->ggets != NULL)) ?   \
     (*PG_console_device->ggets)(buffer, maxlen, stream) :                   \
     NULL)

#define PG_puts(bf)                                                          \
    if ((PG_console_device != NULL) && (PG_console_device->gputs != NULL))   \
       (*PG_console_device->gputs)(bf)

#define PG_get_device_image(_d, _i)                                          \
    ((_i) = ((PG_image *) GET_RST_DEVICE(_d)->fp))

#define PG_put_device_image(_d, _i)                                          \
    (GET_RST_DEVICE(_d)->fp = (FILE *) (_i))

#define PG_set_color_type(dev, dev_type, color)                              \
    if (strcmp(dev_type, "PS") == 0)                                         \
       {SFREE(dev->type);                                                    \
        dev->type = SC_strsavef(color,                                       \
                                "char*:PG_SET_COLOR_TYPE:ps_type");          \
        if (strcmp(color, "COLOR"))                                          \
	   dev->ps_color = TRUE;                                             \
	else                                                                 \
	   dev->ps_color = FALSE;}

#define PG_window_width(_d)     abs((_d)->g.hwin[1] - (_d)->g.hwin[0])
#define PG_window_height(_d)    abs((_d)->g.hwin[3] - (_d)->g.hwin[2])

#define PG_luminance(_r, _g, _b) (0.2988*(_r) + 0.5869*(_g) + 0.1143*(_b) + 0.5)

#define PG_true_color(_r, _g, _b)                                            \
    ((0x7f) << 24 +                                                          \
     ((_r) & 0xff) << 16 +                                                   \
     ((_g) & 0xff) << 8 +                                                    \
     ((_b) & 0xff))

#define PG_is_true_color(_c)   (((0x7f << 24) & (_c)) != 0)

/*--------------------------------------------------------------------------*/
 
/*                        TYPEDEF'S AND STRUCT'S                            */
 
/*--------------------------------------------------------------------------*/

typedef struct s_PG_axis_tick_def PG_axis_tick_def;
typedef struct s_PG_axis_def PG_axis_def;
typedef struct s_PG_graph PG_graph;
typedef PG_graph *(*PFPPG_graph)(void);
typedef struct s_PG_marker PG_marker;
typedef struct s_PG_font_family PG_font_family;
typedef struct s_PG_curve PG_curve;
typedef struct s_PG_interface_object PG_interface_object;
typedef PG_interface_object *(*PFPPG_interface_object)(void);
typedef struct s_PG_device PG_device;
typedef PG_device *(*PFPPG_device)(void);
typedef struct s_PG_palette PG_palette;
typedef struct s_PG_picture_desc PG_picture_desc;
typedef struct s_PG_image PG_image;
typedef struct s_PG_par_rend_info PG_par_rend_info;
typedef struct s_PG_view_attributes PG_view_attributes;
typedef struct s_PG_dev_attributes PG_dev_attributes;
typedef struct s_PG_dev_geometry PG_dev_geometry;
typedef struct s_curve curve;
typedef struct s_PG_text_box PG_text_box;
typedef struct s_PG_event_handler PG_event_handler;
typedef int (*PFRDev)(PG_device *dev);

typedef void (*PFKeymap)(PG_text_box *b);

enum e_PG_coord_sys
   {WORLDC = 1,
    NORMC,
    PIXELC,
    BOUNDC,
    FRAMEC};

typedef enum e_PG_coord_sys PG_coord_sys;

enum e_PG_dev_type
   {TEXT_WINDOW_DEVICE = 128,             /* Quickdraw requires distinction */
    GRAPHIC_WINDOW_DEVICE,                                 /* Screen Window */
    PS_DEVICE,                                                /* PostScript */
    CGMF_DEVICE,                                                     /* CGM */
    PNG_DEVICE,                                                      /* PNG */
    JPEG_DEVICE,                                                    /* JPEG */
    MPEG_DEVICE,                                                    /* MPEG */
    HARD_COPY_DEVICE,                                /* undifferentiated HC */
    IMAGE_DEVICE,                                           /* IMAGE device */
    RASTER_DEVICE};                                        /* RASTER device */

typedef enum e_PG_dev_type PG_dev_type;

enum e_PG_rendering
   {PLOT_NONE = 0,
    PLOT_CURVE = 10,
    PLOT_CONTOUR,
    PLOT_IMAGE,
    PLOT_WIRE_MESH,
    PLOT_SURFACE,
    PLOT_VECTOR,
    PLOT_FILL_POLY,
    PLOT_MESH,
    PLOT_ERROR_BAR,
    PLOT_SCATTER,
    PLOT_DV_BND,
    PLOT_DEFAULT,
    PLOT_CARTESIAN,
    PLOT_POLAR,
    PLOT_INSEL,
    PLOT_HISTOGRAM,
    PLOT_LOGICAL};

typedef enum e_PG_rendering PG_rendering;

enum e_PG_axis_attr
   {AXIS_LINESTYLE = 100,
    AXIS_LINETHICK,
    AXIS_LINECOLOR,
    AXIS_LABELCOLOR,
    AXIS_LABELSIZE,
    AXIS_LABELFONT,
    AXIS_LABELPREC,
    AXIS_X_FORMAT,
    AXIS_Y_FORMAT,
    AXIS_TICKSIZE,
    AXIS_GRID_ON,
    AXIS_SIGNIF_DIGIT,
    AXIS_CHAR_ANGLE,
    AXIS_TICK_RIGHT,
    AXIS_TICK_LEFT,
    AXIS_TICK_STRADDLE,
    AXIS_TICK_ENDS,
    AXIS_TICK_MAJOR = 1,   /* these are used a flags which can be OR'd together */
    AXIS_TICK_MINOR = 2,
    AXIS_TICK_LABEL = 4,
    AXIS_TICK_NONE  = 8};

typedef enum e_PG_axis_attr PG_axis_attr;

enum e_PG_axis_type
   {CARTESIAN_2D = -1,
    CARTESIAN_3D = -2,
    POLAR        = -3,
    INSEL        = -4};

typedef enum e_PG_axis_type PG_axis_type;

enum e_PG_grid_attr
   {GRID_LINESTYLE = 1,
    GRID_LINETHICK,
    GRID_LINECOLOR};

typedef enum e_PG_grid_attr PG_grid_attr;

enum e_PG_line_attr
   {LINE_SOLID = 1,
    LINE_DASHED,
    LINE_DOTTED,
    LINE_DOTDASHED,
    LINE_NONE};

typedef enum e_PG_line_attr PG_line_attr;

enum e_PG_vector_attr
   {VEC_SCALE = 1,
    VEC_ANGLE,
    VEC_HEADSIZE,
    VEC_FIXSIZE,
    VEC_MAXSIZE,
    VEC_LINESTYLE,
    VEC_LINETHICK,
    VEC_COLOR,
    VEC_FIXHEAD};

typedef enum e_PG_vector_attr PG_vector_attr;

enum e_PG_mouse_button
   {MOUSE_NONE       = 0,
    MOUSE_LEFT       = 1,
    MOUSE_MIDDLE     = 2,
    MOUSE_RIGHT      = 4,
    MOUSE_WHEEL_UP   = 8,
    MOUSE_WHEEL_DOWN = 16,
    MOUSE_SOFTWARE   = 10,
    MOUSE_HARDWARE   = 11};

typedef enum e_PG_mouse_button PG_mouse_button;

enum e_PG_key_modifier
    {KEY_SHIFT = 0x10,
     KEY_CNTL  = 0x20,
     KEY_ALT   = 0x40,
     KEY_LOCK  = 0x80};

typedef enum e_PG_key_modifier PG_key_modifier;

enum e_PG_iob_tag
   {PG_IOB_NAME = 1,
    PG_IOB_CLR  = 2,
    PG_IOB_FLG  = 3,
    PG_IOB_DRW  = 4,
    PG_IOB_ACT  = 5,
    PG_IOB_SEL  = 6,
    PG_IOB_OBJ  = 7,
    PG_IOB_END  = 8};

typedef enum e_PG_iob_tag PG_iob_tag;

enum e_PG_iob_state
   {PG_IsVis = 0,
    PG_IsSel = 1,
    PG_IsAct = 2};

typedef enum e_PG_iob_state PG_iob_state;

enum e_PG_clear_mode
  {CLEAR_SCREEN   = -5,
   CLEAR_VIEWPORT = -6,
   CLEAR_FRAME    = -7};

typedef enum e_PG_clear_mode PG_clear_mode;

enum e_PG_orientation
   {HORIZONTAL     = -10,
    VERTICAL       = -11,
    PORTRAIT_MODE  =  10,
    LANDSCAPE_MODE =  11};

typedef enum e_PG_orientation PG_orientation;

enum e_PG_PS_primitive
   {PG_TEXT   = 1,
    PG_FILL   = 2,
    PG_LINE   = 3,
    PG_NOPRIM = -1};

typedef enum e_PG_PS_primitive PG_PS_primitive;

enum e_PG_text_alignment
   {LEFT,
    RIGHT,
    CENTER};

typedef enum e_PG_text_alignment PG_text_alignment;

enum e_PG_text_window_attr
   {MORE_WINDOW              = -1,
    SCROLLING_WINDOW         = -2,
    TEXT_INSERT              = -3,
    TEXT_OVERWRITE           = -4,
    TEXT_CHARACTER_PRECISION = 1};

typedef enum e_PG_text_window_attr PG_text_window_attr;

enum e_PG_quadrant
   {QUAD_ONE  = 1,
    QUAD_FOUR = 4};

typedef enum e_PG_quadrant PG_quadrant;

enum e_PG_angle_unit
   {DEGREE = 1,
    RADIAN = 2};

typedef enum e_PG_angle_unit PG_angle_unit;

enum e_PG_logical_operation
   {GS_COPY = 3,
    GS_XOR  = 6};

typedef enum e_PG_logical_operation PG_logical_operation;

enum e_PG_lang_binding
   {_C_LANG       = 1,
    _FORTRAN_LANG = 2};

typedef enum e_PG_lang_binding PG_lang_binding;

/*--------------------------------------------------------------------------*/

struct s_curve
   {char *text;                            /* text associated with the data */
    int modified;                          /* flag indicating changed curve */
    void *obj;                       /* pointer to the encapsulating object */
    double wc[PG_BOXSZ];
    int n;                                /* number of data points in array */
    double *x[PG_SPACEDM];                       /* point coordinate arrays */
    pcons *info;
    void *file_info;           /* file info - cast to some struct with info */
    SC_file_type file_type;                 /* file type ASCII, BINARY, PDB */
    char *file;                                      /* file name for curve */
    unsigned int id;};                                 /* letter identifier */
     
/*--------------------------------------------------------------------------*/


/* PG_AXIS_TICK_DEF - axis tick definition information */

struct s_PG_axis_tick_def
   {int n;
    int ndiv;        /* number of divisions - size of dx */
    int log_scale;
    double end;
    double start;
    double space;
    double vn;       /* the mimimum label value */
    double vx;       /* the maxmimum label value */
    double *dx;};    /* DX is ordered from min to max - normalized (0, 1) */


/* PG_AXIS_DEF - axis definition information */

struct s_PG_axis_def
   {int axis_type;
    int tick_type;
    int label_type;
    int log_scale[PG_SPACEDM];

    double x0[PG_SPACEDM];  /* axis vector origin */
    double dr;              /* axis vector length */
    double cosa;            /* axis vector direction cosines */
    double sina;

    double t1;     /* normalized interval along axis vector */
    double t2;     /* t1 < t2 means outward and t1 > t2 means inward */
    double vn;     /* minimum label value */
    double vx;     /* maximum label value */

    double scale;
    double scale_x[PG_SPACEDM];
    double eps_rel;

    PG_axis_tick_def tick[3];};

/*--------------------------------------------------------------------------*/

/* GRAPH - contains the information necessary to display a list of functions
 *       - in some fashion
 */

struct s_PG_graph
   {PM_mapping *f;
    PG_rendering rendering;
    int mesh;
    char *info_type;                       /* data type for the info member */
    void *info;        /* attribute information for the rendering algorithm */
    int identifier;
    char *use;                          /* volatile communicator internally */
    void (*render)(PG_device *dev, PG_graph *data, ...);
    struct s_PG_graph *next;};

/*--------------------------------------------------------------------------*/

/* MARKER - line segment description of a marker character */

struct s_PG_marker
   {int n_seg;
    double *x1;
    double *y1;
    double *x2;
    double *y2;};

/*--------------------------------------------------------------------------*/

/* FONT_FAMILY - info for a family of fonts
 *             - the canonical set of styles is (in order):
 *             -
 *             - Normal
 *             - Italic
 *             - Bold
 *             - Bold Italic
 *             -
 *             - others may be defined, but these should be here always
 */

struct s_PG_font_family
   {char *type_face;
    int n_styles;
    char **type_styles;
    struct s_PG_font_family *next;};

/*--------------------------------------------------------------------------*/

/* declare RGB color map */

struct s_RGB_color_map
   {double red;
    double green;
    double blue;};

typedef struct s_RGB_color_map RGB_color_map;

/*--------------------------------------------------------------------------*/

/* palette structure */
struct s_PG_palette
   {char *name;
    int max_pal_dims;                 /* number of different palette shapes */
    int **pal_dims;            /* shapes for 1, 2, ... dimensional palettes */
    int n_pal_colors;
    int n_dev_colors;
    double max_red_intensity;
    double max_green_intensity;
    double max_blue_intensity;
    RGB_color_map *pseudo_colormap;
    RGB_color_map *true_colormap;
    unsigned long *pixel_value;

#ifdef MAC_COLOR
    PaletteHandle hpalette;
#endif

    struct s_PG_palette *next;};

/*--------------------------------------------------------------------------*/

#ifndef EVENT_DEFINED

typedef struct s_PG_event PG_event;

struct s_PG_event
   {int type;};

#endif

/*--------------------------------------------------------------------------*/

typedef int (*PFGetEvent)(PG_event *ev);
typedef PG_device *(*PFEventDevice)(PG_event *ev);
typedef int (*PFOpenConsole)(char *title, char *type, int bckgr,
                             double xf, double yf, double dxf, double dyf);
typedef void (*PFEventHand)(PG_device *dev, PG_event *ev);
typedef void (*PFMouseHand)(PG_device *dev, PG_event *ev);
typedef void (*PFFMouseHand)(FIXNUM *fdvice, FIXNUM *fevnt);

typedef void (*PFEvCallback)(void *d, PG_event *ev);
typedef void (*PFIobDraw)(PG_interface_object *iob);
typedef void (*PFIobAction)(PG_interface_object *iob, PG_event *ev);
typedef PG_interface_object *(*PFIobSelect)(PG_interface_object *iob, PG_event *ev);


/*--------------------------------------------------------------------------*/

/* PG_CURVE - this is a structure containing a curve (line)
 *          - the points are in pixel coordinates which makes
 *          - plotting operations faster
 *          - it is useful for defining boundaries
 */

struct s_PG_curve
   {int n;
    int closed;
    int x_origin;
    int y_origin;
    int *x;
    int *y;
    double rx_origin;
    double ry_origin;};

/*--------------------------------------------------------------------------*/

struct s_PG_interface_object
   {PG_device *device;
    PG_curve *curve;
    char *name;
    char *type;
    void *obj;
    int foreground;
    int background;
    int is_visible;
    int is_active;
    int is_selectable;

    char *draw_name;
    char *action_name;
    char *select_name;

    PFIobDraw draw;
    PFIobAction action;
    PFIobSelect select;

    PG_interface_object *parent;
    SC_array *children;};

/*--------------------------------------------------------------------------*/

struct s_PG_text_box
   {char *name;
    int type;
    int active;
    PG_device *dev;
    PG_curve *bnd;
    int n_lines;
    int n_chars_line;
    int n_lines_page;
    int line;
    int col;
    int mode;
    PG_text_alignment align;
    double angle;
    int background;
    int foreground;
    double border;
    int standoff;
    char **text_buffer;
    PFKeymap *keymap;
    PG_text_box *next;

    char *(*backup)(PG_text_box *b, char *p, int n);
    char *(*gets)(PG_text_box *b, char *buffer, int maxlen, FILE *stream);

    void (*clear)(PG_text_box *b, int i);
    void (*write)(PG_text_box *b, char *s);
    void (*newline)(PG_text_box *b);};

/*--------------------------------------------------------------------------*/

struct s_PG_event_handler
   {PFEventHand fnc;
    int lang;};

/*--------------------------------------------------------------------------*/

struct s_PG_dev_geometry
   {int phys_height;          /* physical device properties in PIXELS */
    int phys_width;
    int hwin[4];              /* 2D window shape on device in PIXELS */
    int iflog[PG_SPACEDM];    /* log space for dimensions */
    int cpc[PG_BOXSZ];        /* conversion limits in PC */
    int tx[PG_BOXSZ];         /* text window in PC */
    double pad[PG_BOXSZ];     /* fractional offset from WC to BND */
    double nd_w[PG_BOXSZ];    /* NDC -> WC transform coefficients */
    double w_nd[PG_BOXSZ];    /* WC -> NDC transform coefficients */
    double fr[PG_BOXSZ];      /* frame limits in NDC */
    double bnd[PG_BOXSZ];     /* boundary limits in WC */
    double pc[PG_BOXSZ];      /* 3D window limits in PC */
    double ndc[PG_BOXSZ];     /* viewspace limits in NDC */
    double wc[PG_BOXSZ];      /* viewspace limits in WC */

    double view_angle[PG_NANGLES];};  /* Euler angles in radians */

/*--------------------------------------------------------------------------*/

/* PG_DEVICE - graphics device structure */

struct s_PG_device
   {

#ifdef HAVE_QUICKDRAW

    WindowPtr window;
    Rect window_shape;
    Rect scroll_bar_shape;
    Rect drag_bound_shape;
    Rect display_shape;  /* display rect which should be cleared */
    Rect text_shape;     /* display_shape inset by (4,0) */
    Point mouse;
    TEHandle text;
    ControlHandle scroll_bar;
    MenuHandle menu[menuCount];
    int next_palette_index;

#endif

#ifdef WIN32
    HWND window;
#endif

/* X11 variables */

#ifdef HAVE_X11

    int X_cap_style;
    int X_join_style;
    int X_line_style;
    unsigned int X_line_width;
    Display *display;
    Window window;
    Pixmap pixmap;
    GC gc;
# ifdef HAVE_OGL
    GLXContext glxgc;
# endif
    XGCValues values;
    XFontStruct *font_info;

#endif

/* PostScript device info */
    double ps_level;                 /* PostScript standard level */
    int ps_color;                    /* TRUE if a color PostScript printer */
    int ps_transparent;              /* use masked image drawing (for rasters) */
    int window_orientation;          /* PORTRAIT_MODE or LANDSCAPE_MODE */

/* CGM device info */
    int cgm_page_set;

/* generic device info */

    int absolute_n_color;
    int autodomain;             /* Sets the auto domain */
    int autoplot;
    int autorange;              /* Sets autoranging */

    int background_color_white;
    int border_width;

/* character controls */
    double char_frac;
    double char_height_s;
    int char_precision;
    double char_path[PG_SPACEDM];
    double char_space;
    double char_space_s;
    double char_up[PG_SPACEDM];
    double char_width_s;
    
    int clipping;

    int data_id;
    int draw_fill_bound;

    FILE *file;
    int fill_color;
    int finished;

    PG_font_family *font_family;

/* graphics cursor location in WC */
    double gcur[PG_SPACEDM];

    int gprint_flag;     /* print suppression flag (TRUE = ON, FALSE = OFF) */
    int grid;

    PG_dev_geometry g;

    int hard_copy_device;                     /* TRUE for hard copy devices */

    SC_array *iobjs;

    int is_visible;

    double light_angle[2];                    /* lighting angles in radians */

    int line_style;
    int line_color;
    double line_width;
    PG_logical_operation logical_op;

/* marker controls */
    double marker_orientation;
    double marker_scale;

    double max_intensity;
    double max_red_intensity;
    double max_green_intensity;
    double max_blue_intensity;

/* color mapping flags */
    int map_text_color;
    int map_line_color;
    int map_fill_color;

    int mode;                           /* _mode = -1 when graphics is off */
    char *name;
    int ncolor;
    int phys_n_colors;
    PG_par_rend_info *pri;              /* parallel rendering info */
    int print_labels;
    int quadrant;
    int range_n_extrema;
    double *range_extrema;

    void *raster;

    int re_open;
    int resolution_scale_factor;
    int scatter;
    int supress_setup;

/* text cursor location in WC */
    double tcur[PG_SPACEDM];

    int text_color;
    char *title; 

    PG_dev_type type_index;
    double txt_ratio;
    char *type;

    char *type_face;
    char *type_style;
    int  type_size;

    int use_pixmap;

/* viewport location/size in window in NDC */
    double view_aspect;
    double view_x[PG_BOXSZ];

/* window location/size in PIXELS */
    double window_x[PG_BOXSZ];

    int xor_parity;

    PG_palette *palettes;
    PG_palette *current_palette;
    PG_palette *color_table;

/* 16 standard colors */
    int BLACK;
    int WHITE;
    int GRAY;
    int DARK_GRAY;
    int BLUE;
    int GREEN;
    int CYAN;
    int RED;
    int MAGENTA;
    int BROWN;
    int DARK_BLUE;
    int DARK_GREEN;
    int DARK_CYAN;
    int DARK_RED;
    int YELLOW;
    int DARK_MAGENTA;

    void (*query_pointer)(PG_device *dev, int *ir, int *pb, int *pq);
    void (*clear_page)(PG_device *dev, int i);
    void (*clear_window)(PG_device *dev);
    void (*clear_viewport)(PG_device *dev);
    void (*clear_region)(PG_device *dev, int nd, PG_coord_sys cs, double *ndc, int pad);
    void (*close_console)(void);
    void (*close_device)(PG_device *dev);
    void (*flush_events)(PG_device *dev);
    void (*draw_dj_polyln_2)(PG_device *dev, double **r, long n,
			     int clip, int coord);
    void (*draw_curve)(PG_device *dev, PG_curve *crv, int clip);
    void (*draw_to_abs)(PG_device *dev, double x, double y);
    void (*draw_to_rel)(PG_device *dev, double x, double y);
    void (*expose_device)(PG_device *dev);
    void (*finish_plot)(PG_device *dev);
    int (*get_char)(PG_device *dev);
    void (*get_image)(PG_device *dev, unsigned char *bf, int ix, int iy,
		      int nx, int ny);
    void (*get_text_ext)(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *bx);
    PFfgets ggets;
    void (*gputs)(char *bf);
    void (*set_clipping)(PG_device *dev, int flag);
    void (*set_char_line)(PG_device *dev, int n);
    void (*set_char_path)(PG_device *dev, double x, double y);
    void (*set_char_precision)(PG_device *dev, int p);
    void (*set_char_space)(PG_device *dev, double s);
    void (*set_char_size)(PG_device *dev, int nd, PG_coord_sys cs, double *x);
    void (*set_char_up)(PG_device *dev, double x, double y);
    void (*set_fill_color)(PG_device *dev, int color, int mapped);
    int (*set_font)(PG_device *dev, char *face, char *style, int size);
    void (*set_line_color)(PG_device *dev, int color, int mapped);
    void (*set_line_style)(PG_device *dev, int style);
    void (*set_line_width)(PG_device *dev, double width);
    void (*set_logical_op)(PG_device *dev, PG_logical_operation lop);
    void (*set_text_color)(PG_device *dev, int color, int mapped);
    void (*shade_poly)(PG_device *dev, int nd, int n, double **r);
    void (*fill_curve)(PG_device *dev, PG_curve *crv);
    void (*make_device_current)(PG_device *dev);
    void (*map_to_color_table)(PG_device *dev, PG_palette *pal);
    void (*match_rgb_colors)(PG_device *dev, PG_palette *pal);
    void (*move_gr_abs)(PG_device *dev, double x, double y);
    void (*move_tx_abs)(PG_device *dev, double x, double y);
    void (*move_tx_rel)(PG_device *dev, double x, double y);
    void (*next_line)(PG_device *dev, FILE *fp);
    PG_device *(*open_screen)(PG_device *dev, double xf, double yf,
			      double dxf, double dyf);
    void (*put_image)(PG_device *dev, unsigned char *bf, int ix, int iy,
		      int nx, int ny);
    void (*query_screen)(PG_device *dev, int *pdx, int *pdy, int *pnc);
    void (*release_current_device)(PG_device *dev);
    void (*update_vs)(PG_device *dev);
    void (*write_text)(PG_device *dev, FILE *fp, char *s);

    int (*events_pending)(PG_device *dev);
    void (*mouse_event_info)(PG_device *dev, PG_event *ev,
			     int *iev, PG_mouse_button *peb, int *peq);
    void (*key_event_info)(PG_device *dev, PG_event *ev, int *iev,
			   char *bf, int n, int *peq);

    PG_event_handler expose_event_handler;
    PG_event_handler update_event_handler;
    PG_event_handler mouse_down_event_handler;
    PG_event_handler mouse_up_event_handler;
    PG_event_handler motion_event_handler;
    PG_event_handler key_down_event_handler;
    PG_event_handler key_up_event_handler;
    PG_event_handler default_event_handler;};
   
/*--------------------------------------------------------------------------*/

/* PG_IMAGE is PD_defstr'd in PDBX.C any changes here must be reflected
 * there!!
 */

struct s_PG_image
   {int version_id;    /* version id in case of change see PG_IMAGE_VERSION */
    char *label;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
    char *element_type;
    unsigned char *buffer;
    int kmax;
    int lmax;
    long size;
    int bits_pixel;
    PG_palette *palette;};


/*--------------------------------------------------------------------------*/

struct s_PG_par_rend_info
   {PG_device *dd;                   /* drawing device for parallel graphics */
    PROCESS *pp;                     /* process for parallel graphics */
    int ip;                          /* id of processor doing final output */
    int have_data;                   /* TRUE if current node has data */
    int *map;                        /* map of processors with data to plot */
    pcons *alist;                    /* picture attributes for parallel graphics */
    char *label;                     /* data label */
    PG_rendering render;             /* rendering mode for parallel graphics */
    int polar;};                     /* polar domain flag */

/*--------------------------------------------------------------------------*/

struct s_PG_view_attributes
   {int finished;

    PG_dev_geometry g;

/* graphics cursor location in WC */
    double gcur[PG_SPACEDM];

/* text cursor location in WC */
    double tcur[PG_SPACEDM];

/* viewport location/size in window in NDC */
    double view_aspect;
    double view_x[PG_BOXSZ];};

/*--------------------------------------------------------------------------*/

struct s_PG_dev_attributes
   {int clipping;
    double char_frac;
    int char_precision;
    double char_space;
    double char_up[PG_SPACEDM];
    int fill_color;
    int line_color;
    int line_style;
    double line_width;
    PG_logical_operation logical_op;
    int text_color;
    PG_palette *palette;};

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/
 
extern FILE
 *stdscr;              /* this is the effective file pointer for the screen */
 
extern char
 *PG_TEXT_OBJECT_S,
 *PG_VARIABLE_OBJECT_S,
 *PG_BUTTON_OBJECT_S,
 *PG_CONTAINER_OBJECT_S;

extern PG_device
 *PG_console_device;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* GSATTR.C declarations */

extern int
 PG_get_attrs_graph(PG_graph *g, int all, ...),
 PG_get_attrs_mapping(PM_mapping *f, ...),
 PG_get_attrs_set(PM_set *s, ...),
 PG_get_attrs_alist(pcons *alst, ...),
 PG_get_attrs_glb(int dflt, ...),
 PG_set_attrs_graph(PG_graph *g, ...),
 PG_set_attrs_mapping(PM_mapping *f, ...),
 PG_set_attrs_set(PM_set *s, ...),
 PG_set_attrs_glb(int dflt, ...),
 PG_rem_attrs_graph(PG_graph *g, ...),
 PG_rem_attrs_mapping(PM_mapping *f, ...),
 PG_rem_attrs_set(PM_set *s, ...);

extern pcons
 *PG_set_attrs_alist(pcons *alst, ...),
 *PG_rem_attrs_alist(pcons *alst, ...);

extern void
 PG_setup_attrs_glb(void),
 *PG_ptr_attr_glb(char *name);


/* GSAUX.C declarations */

extern unsigned char
 *PG_c_str_pascal(unsigned char *bf, char *s, long n, int pad);

extern int
 PG_CGM_command(PG_device *dev, int cat, int id, int nparams, ...);


/* GSAXIS.C declarations */

extern int
 PG_set_axis_attributes(PG_device *dev, ...);

extern void
 PG_axis(PG_device *dev, int axis_type),
 PG_axis_3(PG_device *dev, double **x, int n_pts, double *extr);

extern PG_axis_def
 *PG_draw_axis_n(PG_device *dev, double *xl, double *xr,
		 double t1, double t2, double v1, double v2,
		 double sc, char *format, int tick_type, int label_type,
		 int flag, ...);


/* GSCLR.C declarations */

extern int
 PG_rgb_index(PG_device *dev, RGB_color_map *cm),
 PG_select_color(PG_device *dev, int n, double *a, double *extr),
 PG_wr_palette(PG_device *dev, PG_palette *pal, char *fname);

extern void
 PG_get_fill_color(PG_device *dev, int *pcl),
 PG_get_line_color(PG_device *dev, int *pcl),
 PG_get_text_color(PG_device *dev, int *pcl),
 PG_register_palette(PG_device *dev, PG_palette *pal, int map),
 PG_setup_standard_palettes(PG_device *dev, int nc, int l1,
			    int l2, int l3, int l4,
			    int l5, int l6),
 PG_rl_palette(PG_palette *pal),
 PG_show_palettes(PG_device *sdev, char *type, int wbck),
 PG_show_colormap(char *type, int wbck),
 PG_dump_colormap(char *type, char *file),
 PG_rgb_color(RGB_color_map *clr, int lc, int rgb, PG_palette *pal);

extern PG_palette
 *PG_mk_palette(PG_device *dev, char *name, int nclr),
 *PG_get_palette(PG_device *dev, char *name),
 *PG_set_palette(PG_device *dev, char *name),
 *PG_make_palette(PG_device *tdev, char *name,
		  int nclr, int wbck),
 *PG_make_ndim_palette(PG_device *tdev, char *name,
		       int ndims, int *dims,  int wbck),
 *PG_rd_palette(PG_device *dev, char *fname);


/* GSCNTR.C declarations */

extern PG_picture_desc
 *PG_setup_picture_contour(PG_device *dev, PG_graph *data,
			   int save, int clear);

extern void
 PG_iso_limit(double *a, int npts, double *pmin, double *pmax),
 PG_contour_plot(PG_device *dev, PG_graph *data, ...);


/* GSDLIN.C declarations */

extern void
 PG_draw_polygon(PG_device *dev, PM_polygon *py, int clip),
 PG_draw_multiple_line(PG_device *dev, int nlines,
		       double *x1, double *x2, double *x3, double *x4,
		       double *dx),
 PG_draw_line_n(PG_device *dev, int nd, PG_coord_sys cs, double *x1, double *x2, int clip),
 PG_draw_polyline_n(PG_device *dev, int nd, PG_coord_sys cs,
		    long n, double **x, int clip),
 PG_draw_disjoint_polyline_n(PG_device *dev, int nd, PG_coord_sys cs,
			     long n, double **x, int clip),
 PG_draw_rad(PG_device *dev, double rmin, double rmax,
	     double a, double x, double y, int unit),
 PG_draw_arc(PG_device *dev, double r, double a1, double a2,
	     double x, double y, int unit);


/* GSDPLT.C declarations */

extern PG_picture_desc
 *PG_setup_picture_mesh(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_mesh_plot(PG_device *dev, PG_graph *data, ...),
 PG_domain_plot(PG_device *dev, PM_set *dom, PM_set *ran),
 PG_draw_domain_boundary(PG_device *dev, PM_mapping *f),
 PG_ref_mesh(PG_device *dev, PG_graph *data, int ndims,
	     double *bx, double *vwprt);


/* GSDV.C declarations */

extern void
 PG_setup_input(char *s, int nc),
 PG_reset_input(void),
 PG_device_filename(char *fname, char *raw, char *ext),
 PG_query_device(PG_device *dev, int *pdx, int *pdy, int *pnc),
 PG_query_window(PG_device *dev, int *pdx, int *pdy),
 PG_register_range_extrema(PG_device *dev, int nd, double *extr),
 PG_flush_events(PG_device *dev);

extern PG_device
 *PG_open_device(PG_device *dev, double xf, double yf, double dxf, double dyf);

extern char
 *PG_wind_fgets(char *str, int maxlen, FILE *stream);

extern int
 PG_setup_window_device(PG_device *dev),
 PG_fprintf(FILE *fp, char *fmt, ...),
 PG_fputs(char *s, FILE *fp),
 PG_write_n(PG_device *dev, int nd, PG_coord_sys cs, double *x, char *fmt, ...),
 PG_wind_fprintf(FILE *fp, char *fmt, ...),
 PG_wind_fputs(char *s, FILE *fp),
 PG_open_console(char *title, char *type, int bckgr,
		 double xf, double yf, double dxf, double dyf);

extern off_t
 PG_set_buffer_size(off_t sz),
 PG_get_buffer_size(void);


/* GSEVMN.C declarations */

extern void
 PG_remove_callback(int *pfd),
 PG_poll(int ind, int to),
 PG_catch_interrupts(int flg),
 PG_flush_events(PG_device *dev);

extern int
 PG_init_event_handling(void),
 PG_loop_callback(int type, void *p,
		  PFFileCallback acc, PFFileCallback rej, int pid),
 PG_check_events(int flag, int to),
 PG_get_next_event(PG_event *ev);

extern SC_evlpdes
 *PG_get_event_loop(void);

extern PG_device
 *PG_get_event_device(PG_event *ev);


/* GSFIA.C declarations */

extern PG_view_attributes
 *PG_view_attributes_pointer(int vwatid);


/* GSGROT.C declarations */

extern void
 PG_grotrian_plot(PG_device *dev, PG_graph *data, ...);


/* GSDV_CGM.C declarations */

extern int
 PG_setup_cgm_device(PG_device *d);


/* GSDV_GL.C declarations */


/* GSDV_IM.C declarations */

extern int
 PG_get_processor_number(void),
 PG_get_number_processors(void),
 PG_setup_image_device(PG_device *d);

extern void
 PG_parallel_setup(PG_device *dev, PG_picture_desc *pd);


/* GSDV_JPG.C declarations */

extern int
 PG_setup_jpeg_device(PG_device *d);


/* GSDV_MPG.C declarations */

extern int
 PG_setup_mpeg_device(PG_device *d);


/* GSDV_PNG.C declarations */

extern int
 PG_setup_png_device(PG_device *d);


/* GSDV_PS.C declarations */

extern int
 PG_setup_ps_device(PG_device *d);


/* GSDV_RST.C declarations */

extern int
 PG_setup_raster_device(PG_device *d);


/* GSIMAG.C declarations */

extern PG_picture_desc
 *PG_setup_picture_image(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_image_picture_info(PG_graph *data, int *pnd, double *dbx, double *rbx),
 PG_image_plot(PG_device *dev, PG_graph *data, ...),
 PG_draw_image_pixels(PG_device *dev, char *name, char *type,
		      void *f, double *dbx, double *ext,
		      void *cnnct, pcons *alist),
 PG_draw_palette_n(PG_device *dev, double *dbx, double *rbx,
		   double wid, int exact),
 PG_draw_2dpalette(PG_device *dev, double *frm, double *rex, double wid),
 PG_invert_image_data(unsigned char *bf,
		      int kx, int lx, int bpi),
 PG_invert_image(PG_image *im),
 PG_get_viewport_size(PG_device *dev, int *pix, int *piy, int *pnx, int *pny),
 PG_draw_image(PG_device *dev, PG_image *im, char *label,
	       pcons *alst),
 PG_rl_image(PG_image *im);

extern PG_image
 *PG_build_image(PG_device *dev, char *name, char *type, void *z,
		 int w, int h, int nd, PG_coord_sys cs, double *dbx, double *rbx),
 *PG_make_image_n(char *label, char *type, void *z,
		  int nd, PG_coord_sys cs, double *dbx, double *rbx,
		  int w, int h, int bpp, PG_palette *palette),
 *PG_extract_image(PG_device *dev, double *dbx, double *rbx);

extern int
 PG_render_data_type(PG_graph *data),
 PG_copy_image(PG_image *dim, PG_image *sim, int bck),
 PG_place_image(PG_image *dim, PG_image *sim, int scale),
 PG_shift_image_range(PG_image *a, PG_image *b, int off);


/* GSCRV.C declarations */

extern PG_picture_desc
 *PG_setup_picture_curve(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_curve_plot(PG_device *dev, PG_graph *data, ...),
 PG_plot_curve(PG_device *dev, double *x, double *y,
	       int n, pcons *info, int l),
 PG_draw_data_ids(PG_device *dev, double *x, double *y, int n,
		  int label, pcons *info),
 PG_rect_plot(PG_device *dev, double *x, double *y, int n, int lncol,
	      double lnwid, int lnsty, int scatter, int marker, int l),
 PG_histogram_plot(PG_device *dev, double *x, double *y, int n,
		   int lncol, double lnwid, int lnsty,
		   int scatter, int marker, int start, int l),
 PG_insel_plot(PG_device *dev, double *x, double *y, int n,
	       int lncol, double lnwid, int lnsty, int l),
 PG_polar_plot(PG_device *dev, double *x, double *y, int n, int lncol,
	       double lnwid, int lnsty, int scatter, int marker, int l);


/* GSHIGH.C declarations */

extern pcons
 *PG_set_plot_type(pcons *info, PG_rendering pty, int axs);

extern PM_polygon
 *PG_clip_polygon(PG_device *dev, PM_polygon *py);

extern void
 PG_clip_data(PG_device *dev, double *tx, double *ty, int *ptn,
	      double *x, double *y, int n),
 PG_clip_line(int *pfl, double *x1, double *x2, double *wc, int *lgf),
 PG_center_label(PG_device *dev, double sy, char *label),
 PG_polar_limits(double *bx, double *x, double *y, int n),
 PG_set_limits_n(PG_device *dev, int nd, PG_coord_sys cs, long n, double **x,
		 PG_rendering pty),
 PG_print_label_set(double *pyo, PG_device *dev, int nlabs,
		    char **labels, char **files,
		    int *dataid, int *modified, int *clr, double *extr, char *ctg,
		    int llf, int lcf, int slf, int lts);

extern int
 PG_clip_line_seg(PG_device *dev, double *x1, double *x2),
 PG_clip_box(double *bx, double *wc, int *lgf);


/* GSREND.C declarations */

extern PG_rendering
 PG_est_rendering(PG_graph *data);

extern void
 PG_find_extrema(PG_graph *data, double stdoff,
		 double **pdpex, double **prpex,
		 int *pnde, double **pddex,
		 int *pnre, double **prdex),
 PG_adorn_before(PG_device *dev, PG_picture_desc *pd, PG_graph *data),
 PG_draw_picture(PG_device *dev, PM_mapping *f, PG_rendering ptyp,
		 int bndp, int cbnd, int sbnd, double wbnd,
		 int mshp, int cmsh, int smsh, double wmsh),
 PG_finish_picture(PG_device *dev, PG_graph *data, PG_picture_desc *pd),
 PG_draw_graph(PG_device *dev, PG_graph *data),
 PG_set_picture_hook(void (*f)(PG_device *dev, PG_graph *data,
			       PG_picture_desc *pd));

extern PG_picture_desc
 *PG_setup_picture(PG_device *dev, PG_graph *data, int save,
		   int clear, int par),
 *PG_get_rendering_properties(PG_device *dev, PG_graph *data);


/* GSIOB.C declarations */

extern PG_device
 *PG_handle_button_press(void *d, PG_event *ev),
 *PG_handle_key_press(void *d, PG_event *ev);

extern PG_interface_object
 *PG_get_object_event(PG_device *dev, PG_event *ev),
 *PG_copy_interface_object(PG_device *dev, PG_interface_object *iob),
 *PG_find_object(PG_device *dev, char *s, PG_interface_object *parent);

extern haelem
 *PG_lookup_variable(char *name);

extern PFVoid
 PG_lookup_callback(char *name);

extern void
 PG_define_region(PG_device *dev, PG_coord_sys cs, double *bx),
 PG_print_pointer_location(PG_device *dev, double cx, double cy, int coord),
 PG_register_callback(char *name, ...),
 PG_register_variable(char *name, char *type, void *var, void *vn, void *vx),
 PG_move_object(PG_interface_object *iob, double *bx, int redraw),
 PG_draw_interface_object(PG_interface_object *iob),
 PG_draw_interface_objects(PG_device *dev),
 PG_key_action(PG_text_box *b, char *bf, int nc),
 PG_map_interface_object(PG_interface_object *iob, PFEvCallback fnc, void *a),
 PG_map_interface_objects(PG_device *dev, PFEvCallback fnc, void *a),
 PG_register_interface_object(PG_device *dev, PG_interface_object *iob),
 PG_add_text_obj(PG_device *dev, double *obx, char *s),
 PG_add_button(PG_device *dev, double *obx, char *s, PFEvCallback fnc),
 PG_free_interface_objects(PG_device *dev),
 PG_copy_interface_objects(PG_device *dvd, PG_device *dvs, int rm),
 PG_handle_keyboard_event(PG_interface_object *iob, PG_event *ev);


/* GSIOD.C declarations */

extern PG_interface_object
 *PG_add_iob(PG_device *dev, PG_interface_object *cnt,
	     double *bx, char *type, ...),
 *PG_iob_make_container(PG_device *dev, char *name, double *bx);

extern void
 PG_iob_register_toggle(PG_device *dev, PG_interface_object *cnt,
			char *name, char *abbrv, int *pv,
			double *bx, int on_clr, int off_clr),
 PG_iob_register_slider(PG_device *dev, PG_interface_object *cnt,
			char *name, char *abbrv, int *pv,
			double *bx, double dx, double dy);


/* GSMM.C declarations */

extern PG_graph
 *PG_make_graph_1d(int id, char *label, int cp, int n, double *x, double *y,
		   char *xname, char *yname),
 *PG_make_graph_r2_r1(int id, char *label, int cp, int imx, int jmx,
		      PM_centering centering, double *x, double *y,
		      double *r, char *dname, char *rname),
 *PG_make_graph_r3_r1(int id, char *label, int cp,
		      int imx, int jmx, int kmx, PM_centering centering,
		      double *x, double *y, double *z, double *r,
		      char *dname, char *rname),
 *PG_make_graph_from_sets(char *name, PM_set *domain, PM_set *range,
			  PM_centering centering, char *info_type, void *info,
			  int id, PG_graph *next),
 *PG_make_graph_from_mapping(PM_mapping *f, char *info_type, void *info,
			     int id, PG_graph *next);

extern PG_device
 *PG_make_device(char *name, char *type, char *title);

extern PG_view_attributes
 *PG_make_view_attributes(PG_device *dev);

extern PG_font_family
 *PG_make_font_family(PG_device *dev, char *name,
		      PG_font_family *next, int n, ...);

extern PG_interface_object
 *PG_make_interface_object(PG_device *dev, char *name, char *type, void *obj,
			   PG_text_alignment align, double ang,
			   PG_curve *crv, int *flags,
			   int fc, int bc,
			   char *select, char *draw, char *action,
			   PG_interface_object *parent);

extern PG_curve
 *PG_make_curve(PG_device *dev, PG_coord_sys cs, int closed, int n,
		double *xo, double **r),
 *PG_make_box_curve(PG_device *dev, PG_coord_sys cs, double *co, double *cbx),
 *PG_copy_curve(PG_curve *icv, PG_device *odv, PG_device *idv);

extern pcons
 *PG_set_line_info(pcons *info, PG_rendering pty, int axis_type,
		   int style, int scatter, int marker,
		   int color, int start, double width),
 *PG_set_tds_info(pcons *info, PG_rendering type, int axis_type,
		  int style, int color, int nlev, double ratio,
		  double width, double theta, double phi, double chi,
		  double d),
 *PG_set_tdv_info(pcons *info, PG_rendering type, int axis_type, 
		  int style, int color, double width);

extern void
 PG_rl_all(void),
 PG_register_device(char *name, PFRDev fnc),
 PG_rl_device(PG_device *dev),
 PG_rl_graph(PG_graph *g, int rld, int rlr),
 PG_save_view_attributes(PG_view_attributes *d, PG_device *dev),
 PG_restore_view_attributes(PG_device *dev, PG_view_attributes *d),
 PG_release_curve(PG_curve *reg);

extern int
 PG_init_parallel(char **argv, PROCESS *pp),
 PG_fin_parallel(int trm);


/* GSPR.C declarations */

extern PFEventHand
 PG_set_expose_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_update_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_mouse_down_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_mouse_up_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_motion_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_key_down_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_key_up_event_handler(PG_device *dev, PFEventHand fnc),
 PG_set_default_event_handler(PG_device *dev, PFEventHand fnc);

extern PG_dev_attributes
 *PG_get_attributes(PG_device *dev);

extern void
 PG_get_char_space(PG_device *dev, double *pcsp),
 PG_get_char_precision(PG_device *dev, int *pcp),
 PG_get_line_width(PG_device *dev, double *plw),
 PG_get_char_up(PG_device *dev, double *px, double *py),
 PG_get_clipping(PG_device *dev, int *flag),
 PG_get_font(PG_device *dev, char **pf, char **pst, int *psz),
 PG_get_char_size_n(PG_device *dev, int nd, PG_coord_sys cs, double *p),
 PG_get_line_style(PG_device *dev, int *pl),
 PG_get_logical_op(PG_device *dev, PG_logical_operation *plop),
 PG_get_char_path(PG_device *dev, double *px, double *py),
 PG_get_text_ext_n(PG_device *dev, int nd, PG_coord_sys cs, char *s, double *p),
 PG_set_attributes(PG_device *dev, PG_dev_attributes *attr),
 PG_setup_markers(void),
 PG_draw_markers_n(PG_device *dev, int nd, PG_coord_sys cs,
		   int n, double **r, int marker),
 PG_draw_box_n(PG_device *dev, int nd, PG_coord_sys cs, double *box),
 PG_fill_polygon_n(PG_device *dev, int color, int mapped,
		   int nd, PG_coord_sys cs, long n, double **r);

extern int
 PG_def_marker(int n, double *x1, double *y1, double *x2, double *y2),
 PG_setup_font(PG_device *dev, char *face, char *style, int size,
	       char **pfn, int *pnf, int *pns);


/* GSRWI.C declarations */

extern int
 PG_write_interface(PG_device *dev, char *name),
 PG_read_interface(PG_device *dev, char *name);


/* GSSCAT.C declarations */

extern PG_picture_desc
 *PG_setup_picture_scatter(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_scatter_plot(PG_device *dev, PG_graph *data, ...),
 PG_draw_scatter(PG_device *dev, double *a1, double *a2,
		 int nn, int ne, double **x,
		 double theta, double phi, double chi, double width,
		 int color, int style, int type, char *name,
		 pcons *alist);


/* GSSURF.C declarations */

extern PG_picture_desc
 *PG_setup_picture_surface(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_surface_plot(PG_device *dev, PG_graph *data, ...),
 PG_draw_surface_n(PG_device *dev, double *a1, double *a2, double *aext,
		   double **r, int nn, double *va, double width,
		   int color, int style, PG_rendering pty, char *name,
		   char *mesh_type, void *cnnct, pcons *alist);


/* GSPOLF.C declarations */

extern PG_picture_desc
 *PG_setup_picture_fill_poly(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_poly_fill_plot(PG_device *dev, PG_graph *data, ...),
 PG_fill_poly_zc_lr(PG_device *dev, int nd, double **a,
		    double *x, double *y, double *aext, void *cnnct,
		    pcons *alist),
 PG_fill_poly_nc_lr(PG_device *dev, int nd, double **a,
		    double *x, double *y, double *aext, void *cnnct,
		    pcons *alist),
 PG_fill_poly_zc_ac(PG_device *dev, int nd, double **a,
		    double *x, double *y, double *aext, void *cnnct,
		    pcons *alist),
 PG_fill_poly_nc_ac(PG_device *dev, int nd, double **a,
		    double *x, double *y, double *aext, void *cnnct,
		    pcons *alist);


/* GSDVB.C declarations */

extern PG_picture_desc
 *PG_setup_picture_dv_bnd(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_dv_bnd_plot(PG_device *dev, PG_graph *data, ...),
 PG_dv_bnd_zc_lr(PG_device *dev, int nd, int *a,
		 double *x, double *y, int npts, int *aext,
		 void *cnnct, pcons *alist),
 PG_dv_bnd_zc_ac(PG_device *dev, int nd, int *a,
		 double *x, double *y, int npts, int *aext,
		 void *cnnct, pcons *alist);


/* GSTXT.C declarations */

extern int
 PG_is_text_box(PG_interface_object *iob),
 PG_is_inactive_text_box(PG_interface_object *iob);

extern PG_text_box
 *PG_open_text_rect(PG_device *dev, char *name, int type,
		    PFKeymap *keymap, PG_curve *crv,
		    double brd, int ndrw),
 *PG_open_text_box(PG_device *dev, char *name, int type,
		   PFKeymap *keymap,
		   double xf, double yf,
		   double dxf, double dyf),
 *PG_copy_text_object(PG_text_box *ib,
		      PG_device *dvd, PG_device *dvs);

extern void
 PG_resize_text_box(PG_text_box *b, double *bx, int n_lines),
 PG_refresh_text_box(PG_text_box *b),
 PG_close_text_box(PG_text_box *b);


/* GSVECT.C declarations */

extern PG_picture_desc
 *PG_setup_picture_vector(PG_device *dev, PG_graph *data, int save, int clear);

extern void
 PG_draw_vector_n(PG_device *dev, int nd, PG_coord_sys cs, long n,
		  double **x, double **u),
 PG_vector_plot(PG_device *dev, PG_graph *data, ...),
 PG_draw_vector(PG_device *dev, PG_graph *g),
 PG_set_vec_attr(PG_device *dev, ...);


/* GSVSP.C declarations */

extern void
 PG_log_space(PG_device *dev, int nd, int dec, double *box),
 PG_log_point(PG_device *dev, int nd, double *p),
 PG_lin_space(PG_device *dev, int nd, double *box),
 PG_lin_point(PG_device *dev, int nd, double *p),
 PG_get_curve_extent(PG_device *dev, PG_curve *crv, PG_coord_sys cs, double *bx),
 PG_get_limit(PG_device *dev, PG_coord_sys cs, double *lim),
 PG_frame_points(PG_device *dev, int nd, PG_coord_sys cs, long n, double **xi,
		 double **xo),
 PG_frame_point(PG_device *dev, int nd, PG_coord_sys cs, double *xi, double *xo),
 PG_frame_interval(PG_device *dev, int nd, double *dxi, double *dxo),
 PG_frame_box(PG_device *dev, int nd, PG_coord_sys cs, double *box),
 PG_frame_viewport(PG_device *dev, int nd, double *ndc),
 PG_viewport_frame(PG_device *dev, int nd, double *ndc),
 PG_get_axis_log_scale(PG_device *dev, int nd, int *iflg),
 PG_set_axis_log_scale(PG_device *dev, int nd, int *iflg),
 PG_init_viewspace(PG_device *dev, int setw),
 PG_set_viewspace(PG_device *dev, int nd, PG_coord_sys cs, double *extr),
 PG_get_viewspace(PG_device *dev, PG_coord_sys cs, double *box),
 PG_set_view_angle(PG_device *dev, double theta, double phi, double chi),
 PG_get_view_angle(PG_device *dev, int cnv, double *pt, double *pp, double *pc),
 PG_set_light_angle(PG_device *dev, double theta, double phi),
 PG_get_light_angle(PG_device *dev, int cnv, double *pt, double *pp),
 PG_scale_points(PG_device *dev, int n, int nd, PG_coord_sys ics, double **xi,
		 PG_coord_sys ocs, double **xo),
 PG_trans_points(PG_device *dev, int n, int nd, PG_coord_sys ics, double **xi,
		 PG_coord_sys ocs, double **xo),
 PG_trans_point(PG_device *dev, int nd, PG_coord_sys ics, double *xi,
		PG_coord_sys ocs, double *xo),
 PG_trans_box(PG_device *dev, int nd, PG_coord_sys ics, double *bi,
	      PG_coord_sys ocs, double *bo),
 PG_trans_interval(PG_device *dev, int nd, PG_coord_sys ics, double *dxi,
		   PG_coord_sys ocs, double *dxo),
 PG_box_init(int nd, double *bx, double mn, double mx),
 PG_box_copy(int nd, double *d, double *s),
 PG_conform_view(PG_device *dev, int nd, PG_coord_sys cs, double *box,
		 int n, double **x),
 PG_rotate_vectors(PG_device *dev, int nd, int n, double **x);

extern int
 PG_box_contains(int nd, double *box, double *p);

extern double
 **PG_get_space_box(PG_device *dev, double *extr, int offs);


#ifndef PGS_3_0_API

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

#endif

#ifdef __cplusplus
}
#endif

#endif


