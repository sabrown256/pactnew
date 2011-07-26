/*
 * GSMM.C - memory management routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

PG_state
 _PG = { TRUE, TRUE, 1.0, 22.5, 0.05, 2, -1, -1, };

extern void
 _PG_rl_markers(void);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RL_FONT_FAMILY - release the memory associated with a font family */

static void _PG_rl_font_family(PG_font_family *f)
   {int i, n;
    char **fs;

    CFREE(f->type_face);

    n  = f->n_styles;
    fs = f->type_styles;
    for (i = 0; i < n; i++)
        CFREE(fs[i]);
    CFREE(fs);

    f->next = NULL;

    CFREE(f);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RL_DEVICE_TABLE - release memory associated with the device table */

static void _PG_rl_device_table(void)
   {haelem *np;

    if (_PG.device_table != NULL)
       {np = SC_hasharr_lookup(_PG.device_table, "PS");
	if (np != NULL)
	   CFREE(np->type);

	SC_free_hasharr(_PG.device_table, NULL, NULL);

        _PG.device_table = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RL_ALL - release all memory allocated internally by pgs */

void PG_rl_all(void)
   {

#ifdef HAVE_X11
    extern void _PG_X_txt_close_device(void);

    _PG_X_txt_close_device();
#endif

    CFREE(_PG_gattrs.text_format);
    CFREE(_PG_gattrs.axis_label_x_format);
    CFREE(_PG_gattrs.axis_label_y_format);
    CFREE(_PG_gattrs.axis_type_face);

    _PG_rl_rst_fonts();
    _PG_rl_device_table();
    _PG_rl_markers();
    _PG_rl_attrs_glb();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_GRAPH_1D - assemble a bunch of stuff together in a graph struct
 *                  - which will describe a 1d graph and return a pointer
 *                  - to it
 *
 * #bind PG_make_graph_1d fortran() scheme() python()
 */

PG_graph *PG_make_graph_1d(int id, char *label, int cp, int n,
			   double *x, double *y, char *xname, char *yname)
   {PG_graph *g;
    pcons *info;
    PM_set *domain, *range;

/* build the domain set */
    domain = PM_make_set(xname, SC_DOUBLE_S, cp, 1, n, 1, x);

/* build the range set */
    range = PM_make_set(yname, SC_DOUBLE_S, cp, 1, n, 1, y);

/* set default info attributes */
    info = PG_set_line_info(NULL, PLOT_CARTESIAN, CARTESIAN_2D,
			    LINE_SOLID, FALSE,
			    0, 1, 0, 0.0);
    g    = PG_make_graph_from_sets(label, domain, range, N_CENT,
				   SC_PCONS_P_S, info, id, NULL);

    return(g);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_GRAPH_R2_R1 - assemble a bunch of stuff together in a graph struct
 *                     - which will describe a mapping from R2 to R1 
 *                     - and return a pointer to it
 *
 * #bind PG_make_graph_r2_r1 fortran() scheme() python()
 */

PG_graph *PG_make_graph_r2_r1(int id, char *label, int cp,
			      int imx, int jmx, PM_centering centering,
			      double *x, double *y, double *r,
			      char *dname, char *rname)
   {PG_graph *g;
    PM_set *domain, *range;

/* build the domain set */
    domain = PM_make_set(dname, SC_DOUBLE_S, cp, 2, imx, jmx, 2, x, y);

/* build the range set */
    range = PM_make_set(rname, SC_DOUBLE_S, cp, 2, imx, jmx, 1, r);

    g = PG_make_graph_from_sets(label, domain, range, centering,
                                SC_PCONS_P_S, NULL, id, NULL);

    return(g);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_GRAPH_R3_R1 - assemble a bunch of stuff together in a graph struct
 *                     - which will describe a mapping from R3 to R1 
 *                     - and return a pointer to it
 */

PG_graph *PG_make_graph_r3_r1(int id, char *label, int cp,
			      int imx, int jmx, int kmx, PM_centering centering,
			      double *x, double *y, double *z, double *r,
			      char *dname, char *rname)
   {PG_graph *g;
    PM_set *domain, *range;

/* build the domain set */
    domain = PM_make_set(dname, SC_DOUBLE_S, cp, 3, imx, jmx, kmx, 3, x, y, z);

/* build the range set */
    range = PM_make_set(rname, SC_DOUBLE_S, cp, 3, imx, jmx, kmx, 1, r);

    g = PG_make_graph_from_sets(label, domain, range, centering,
                                SC_PCONS_P_S, NULL, id, NULL);

    return(g);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_LINE_INFO - set information describing how to
 *                  - render line plots
 */

pcons *PG_set_line_info(pcons *info, PG_rendering pty, int axis_type, int style,
			int scatter, int marker, int color, int start,
			double width)
   {

    info = PG_set_plot_type(info, pty, axis_type);

    info = PG_set_attrs_alist(info,
			      "HIST-START",   SC_INT_I,    FALSE, start,
			      "SCATTER",      SC_INT_I,    FALSE, scatter,
			      "MARKER-INDEX", SC_INT_I,    FALSE, marker,
			      "LINE-COLOR",   SC_INT_I,    FALSE, color,
			      "LINE-STYLE",   SC_INT_I,    FALSE, style,
			      "LINE-WIDTH",   SC_DOUBLE_I, FALSE, width,
			      NULL);
    return(info);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_TDS_INFO - set information describing how to
 *                 - render 2D scalar plots
 */

pcons *PG_set_tds_info(pcons *info, PG_rendering pty, int axis_type, int style,
		       int color, int nlev, double ratio, double width,
		       double theta, double phi, double chi, double d)
   {

    info = PG_set_plot_type(info, pty, axis_type);

    info = PG_set_attrs_alist(info,
			      "N-LEVELS",   SC_INT_I,    FALSE, nlev,
			      "LINE-COLOR", SC_INT_I,    FALSE, color,
			      "LINE-STYLE", SC_INT_I,    FALSE, style,
			      "LINE-WIDTH", SC_DOUBLE_I, FALSE, width,
			      "RATIO",      SC_DOUBLE_I, FALSE, ratio,
			      "THETA",      SC_DOUBLE_I, FALSE, theta,
			      "PHI",        SC_DOUBLE_I, FALSE, phi,
			      "CHI",        SC_DOUBLE_I, FALSE, chi,
			      "DISTANCE",   SC_DOUBLE_I, FALSE, d,
			      NULL);

    return(info);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_TDV_INFO - set information describing how to
 *                 - render 2D vector plots
 */

pcons *PG_set_tdv_info(pcons *info, PG_rendering pty, int axis_type,
		       int style, int color, double width)
   {

    info = PG_set_plot_type(info, pty, axis_type);

    info = PG_set_attrs_alist(info,
			      "LINE-COLOR",   SC_INT_I,    FALSE, color,
			      "LINE-STYLE",   SC_INT_I,    FALSE, style,
			      "LINE-WIDTH",   SC_DOUBLE_I, FALSE, width,
			      NULL);
    return(info);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_GRAPH_FROM_SETS - given a domain, range, and attributes bind them
 *                         - all together in a freshly allocated PG_graph
 *                         - and return a pointer to it
 */

PG_graph *PG_make_graph_from_sets(char *name, PM_set *domain, PM_set *range,
				  PM_centering centering, char *info_type,
				  void *info, int id, PG_graph *next)
   {PM_mapping *f;
    PG_graph *g;

/* build the mapping */
    if (domain->topology == NULL)
       f = PM_make_mapping(name, PM_LR_S, domain, range, centering, NULL);
    else
       f = PM_make_mapping(name, PM_AC_S, domain, range, centering, NULL);

    g = PG_make_graph_from_mapping(f, info_type, info, id, NULL);
    g->next = next;

    return(g);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_GRAPH_FROM_MAPPING - given a mapping and attributes bind them
 *                            - all together in a freshly allocated PG_graph
 *                            - and return a pointer to it
 */

PG_graph *PG_make_graph_from_mapping(PM_mapping *f, char *info_type,
				     void *info, int id, PG_graph *next)
   {PG_graph *g;

/* build the graph */
    g             = CMAKE(PG_graph);
    g->f          = f;
    g->info_type  = info_type;
    g->info       = info;
    g->identifier = id;
    g->use        = NULL;
    g->render     = NULL;
    g->mesh       = FALSE;
    g->next       = next;

    return(g);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RL_GRAPH - free a graph */

void PG_rl_graph(PG_graph *g, int rld, int rlr)
   {pcons *inf;

    if (g->info_type != NULL)
       {if (strcmp(g->info_type, SC_PCONS_P_S) == 0)
	   {inf = (pcons *) g->info;
	    if (inf != NULL)
	       SC_free_alist(inf, 3);};};

    if (g->f != NULL)
       PM_rel_mapping(g->f, rld, rlr);

    CFREE(g);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_REGISTER_DEVICE - let users add their own devices
 *                    - to do so they must register a setup function
 *                    - under the device name
 *
 * #bind PG_register_device fortran() scheme() python()
 */

void PG_register_device(char *name, PFRDev fnc)
   {void *f;
    static char *type = NULL;

    PG_setup_attrs_glb();

    if (_PG.device_table == NULL)
       {type = CSTRSAVE("PFVoid");
	_PG.device_table = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);

/* register the default set of devices */
	PG_register_device("SCREEN", PG_setup_window_device);

#if defined(HAVE_QD)
	PG_register_device("QD",     PG_setup_qd_device);
        strncpy(_PG_gcont.default_window_device, "QD", MAXLINE);
#elif defined(WIN32)
	PG_register_device("WIN32",  PG_setup_win32_device);
        strncpy(_PG_gcont.default_window_device, "WIN32", MAXLINE);
#endif
#if defined(HAVE_OGL)
	PG_register_device("GL",     PG_setup_gl_device);
        strncpy(_PG_gcont.default_window_device, "GL", MAXLINE);
#endif
#if defined(HAVE_X11)
	PG_register_device("X11",    PG_setup_x11_device);
        strncpy(_PG_gcont.default_window_device, "X11", MAXLINE);
#endif
	PG_register_device("PS",     PG_setup_ps_device);
	PG_register_device("PNG",    PG_setup_png_device);
	PG_register_device("JPEG",   PG_setup_jpeg_device);
	PG_register_device("MPEG",   PG_setup_mpeg_device);
	PG_register_device("CGM",    PG_setup_cgm_device);};

    if ((name != NULL) && (fnc != NULL))
       {f = (void *) fnc;
	SC_hasharr_install(_PG.device_table, name, f, type, FALSE, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_INIT_DEVICE_GEOMETRY - initialize a the geometry member of DEV */

static void PG_init_device_geometry(PG_dev_geometry *g)
   {int id, l;

    for (id = 0; id < PG_SPACEDM; id++)
        {l = 2*id;

	 g->iflog[id] = FALSE;

/* window location/size in PC */
	 g->hwin[l]   = 0;
	 g->hwin[l+1] = 0;

/* fractional offset from WC to BND */
	 g->pad[l]   = 0.01;
	 g->pad[l+1] = 0.01;

/* frame setup */
	 g->fr[l]   = 0.0;
	 g->fr[l+1] = 1.0;

/* boundary limits in WC */
	 g->bnd[l]   = 0.0;
	 g->bnd[l+1] = 1.0;

/* conversion limits in PC */
	 g->cpc[l]   = 0;
	 g->cpc[l+1] = 0;

/* text window in PC */
	 g->tx[l]   = 0;
	 g->tx[l+1] = 1;

/* window limits in PC */
	 g->pc[l]   = 0;
	 g->pc[l+1] = 0;

/* viewspace limits in NDC */
	 g->ndc[l]   = 0.0;
	 g->ndc[l+1] = 1.0;

/* viewspace limits in WC */
	 g->wc[l]   = 0.0;
	 g->wc[l+1] = 1.0;};

    for (id = 0; id < PG_NANGLES; id++)
        g->view_angle[id] = 0.0;

/* physical device properties */
    g->phys_height = -1;
    g->phys_width  = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_RAW_DEVICE - initialize a graphical device to lowest order */

static PG_device *PG_make_raw_device(char *name, char *type, char *title,
				     PG_device *dd, int dp)
   {int id, ok, lnclr, lnsty, txclr;
    double lnwid;
    char bf[MAXLINE], *pttl, *pname;
    PG_device *d;
    PFRDev fnc;

    PG_setup_attrs_glb();

    d = CMAKE(PG_device);

    if (d == NULL)
       return(NULL);

    PG_get_attrs_glb(TRUE,
		     "line-color", &lnclr,
		     "line-style", &lnsty,
		     "line-width", &lnwid,
		     "text-color", &txclr,
		     NULL);

#if 0
    snprintf(bf, MAXLINE, "%s [PROC:%d]", title, dp);
    pttl = bf;
#else
    pttl = title;
#endif

/* parallel graphics info */
    d->pri = _PG_init_dev_parallel(dd, dp);

/* generic device info */

    d->absolute_n_color        = 256;
    d->autodomain              = TRUE;
    d->autorange               = TRUE;

    d->background_color_white  = TRUE;
    d->border_width            = 1;

/* character controls */
    d->char_frac               = 0.015;
    d->char_height_s           = 0.0;
    d->char_precision          = 1;
    d->char_space              = 0.0;
    d->char_space_s            = 0.0;
    d->char_width_s            = 0.0;

    d->clipping                = FALSE;
    d->color_table             = NULL;
    d->current_palette         = NULL;
    d->data_id                 = 0;
    d->draw_fill_bound         = FALSE;
    d->file                    = NULL;
    d->fill_color              = 0;
    d->finished                = TRUE;

    d->font_family             = NULL;

    d->gprint_flag             = TRUE;
    d->grid                    = OFF;

/* TRUE for hard copy devices */
    d->hard_copy_device        = FALSE;

    d->iobjs                   = CMAKE_ARRAY(PG_interface_object *, NULL, 0);
    d->is_visible              = TRUE;
    d->line_style              = lnsty;
    d->line_color              = lnclr;
    d->line_width              = lnwid;
    d->logical_op              = GS_COPY;

    d->marker_scale            = _PG_gattrs.marker_scale;
    d->marker_orientation      = 0.0;
    d->match_rgb_colors        = NULL;

    d->max_intensity           = 1.0;
    d->max_red_intensity       = 1.0;
    d->max_green_intensity     = 1.0;
    d->max_blue_intensity      = 1.0;

/* color mapping flags */
    d->map_text_color          = TRUE;
    d->map_line_color          = TRUE;
    d->map_fill_color          = TRUE;

    d->mode                    = -1;
    d->name                    = CSTRSAVE(name);
    d->ncolor                  = 16;
    d->palettes                = NULL;
    d->print_labels            = FALSE;
    d->ps_transparent          = TRUE;
    d->quadrant                = 0;
    d->range_n_extrema         = 0;
    d->range_extrema           = NULL;
    d->raster                  = NULL;
    d->re_open                 = FALSE;
    d->resolution_scale_factor = 1;
    d->scatter                 = 0;
    d->supress_setup           = FALSE;
    d->text_color              = txclr;
    d->title                   = CSTRSAVE(pttl);
    d->use_pixmap              = PG_fget_use_pixmap();

    d->txt_ratio               = 1.0;
    d->type                    = SC_str_upper(CSTRSAVE(type));
    d->type_face               = CSTRSAVE("helvetica");
    d->type_style              = CSTRSAVE("medium");
    d->type_size               = 12;

/* physical device properties */
    d->phys_n_colors           = -1;

/* viewport location/size in window in NDC */
#ifdef HAVE_WINDOW_DEVICE
    d->window                  = 0;
#endif
    d->view_aspect             = 1.0;

    d->xor_parity              = 0;

    for (id = 0; id < PG_SPACEDM; id++)
        {d->gcur[id]       = 0.0;
	 d->tcur[id]       = 0.0;
	 d->char_path[id]  = 0.0;
         d->char_up[id]    = 0.0;};

    PG_box_init(3, d->view_x, 0.0, 0.0);
    PG_box_init(3, d->window_x, 0.0, 0.0);

    d->char_path[0] = 1.0;
    d->char_up[1]   = 1.0;

    for (id = 0; id < 2; id++)
        d->light_angle[id] = 0.0;

    d->expose_event_handler.fnc      = NULL;
    d->expose_event_handler.lang     = _C_LANG;

    d->update_event_handler.fnc      = NULL;
    d->update_event_handler.lang     = _C_LANG;

    d->mouse_down_event_handler.fnc  = NULL;
    d->mouse_down_event_handler.lang = _C_LANG;

    d->mouse_up_event_handler.fnc    = NULL;
    d->mouse_up_event_handler.lang   = _C_LANG;

    d->motion_event_handler.fnc      = NULL;
    d->motion_event_handler.lang     = _C_LANG;

    d->key_down_event_handler.fnc    = NULL;
    d->key_down_event_handler.lang   = _C_LANG;

    d->key_up_event_handler.fnc      = NULL;
    d->key_up_event_handler.lang     = _C_LANG;

    d->default_event_handler.fnc     = NULL;
    d->default_event_handler.lang    = _C_LANG;

    if (_PG.device_table == NULL)
       PG_register_device(NULL, NULL);

    strcpy(bf, name);
    pname = SC_firsttok(bf, " \t\r\n");
    fnc   = SC_HASHARR_LOOKUP_FUNCTION(_PG.device_table, PFRDev, pname);
    ok    = FALSE;
    if (fnc != NULL)
       ok = (*fnc)(d);

    else
       {fnc = SC_HASHARR_LOOKUP_FUNCTION(_PG.device_table, PFRDev, "SCREEN");
	if (fnc != NULL)
	   ok = (*fnc)(d);};

    PG_init_device_geometry(&d->g);

    if (ok == FALSE)
       {PG_rl_device(d);
	d = NULL;}

/* by default make world coordinates be the same as NDC */
    else
       {PG_set_viewspace(d, 3, NORMC, NULL);
	PG_set_viewspace(d, 3, WORLDC, NULL);};

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_DEVICE - initialize a graphical device
 *                - NAME - a desriptive name of the device
 *                -        WINDOW - display screen window
 *                -        PS     - PostScript
 *                -        CGM    - CGM device
 *                -        RASTER - raster device
 *                - TYPE - a qualifier for the device
 *                -        MONOCHROME - black and white display
 *                -        COLOR      - color display
 *
 * #bind PG_make_device fortran() scheme() python()
 */

PG_device *PG_make_device(char *name, char *type, char *title)
   {int ip, np;
    char *fname;
    PG_device *d, *dd;

    dd    = NULL;
    fname = name;

    np = PG_get_number_processors();
    if (np > 1)
       {ip = PG_get_processor_number();
	if (ip == _PG.dp)
	   dd = PG_make_raw_device(name, type, title, NULL, _PG.dp);

	fname = "IMAGE";}
	
    d      = PG_make_raw_device(fname, type, title, dd, _PG.dp);
    _PG.dp = (_PG.dp + 1) % np;

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RL_DEVICE - release the memory associated with a graphical device */

void PG_rl_device(PG_device *dev)
   {PG_font_family *f, *nxt_f;
    PG_palette *pal, *nxt_p;
    PG_device *dd;
    PG_par_rend_info *pri;

    CFREE(dev->name);
    CFREE(dev->range_extrema);
    CFREE(dev->title);
    CFREE(dev->type);
    CFREE(dev->type_face);
    CFREE(dev->type_style);

/* free any raster device */
    PG_free_raster_device(dev->raster);

/* free any parallel rendering info */
    pri = dev->pri;
    if (pri != NULL)
       {dd = pri->dd;

/* NOTE: leave pri->pp it is doublely a global guy at this time */
	if (dd != NULL)
	   PG_close_device(dd);

	CFREE(pri->map);
	CFREE(pri->label);
        CFREE(pri);};

/* free any font info */
    for (f = dev->font_family; f != NULL; f = nxt_f)
	{nxt_f = f->next;
	 _PG_rl_font_family(f);};

/* free interface objects */
    PG_free_interface_objects(dev);

/* free palettes and color tables */
    if (dev->palettes != NULL)
       {for (pal = dev->palettes; pal->next != NULL; pal = nxt_p)
	    {nxt_p = pal->next;
	     PG_rl_palette(pal);
	     if (nxt_p == dev->palettes)
	        break;};};

    if (dev->color_table != NULL)
       PG_rl_palette(dev->color_table);

    CFREE(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_FONT_FAMILY - encapsulate the information for a family of 
 *                     - text fonts
 */

PG_font_family *PG_make_font_family(PG_device *dev, char *name,
                                    PG_font_family *next, int n, ...)
   {int i;
    char *ft, **fs;
    PG_font_family *f;

    f  = CMAKE(PG_font_family);
    fs = CMAKE_N(char *, n);

    SC_VA_START(n);

    for (i = 0; i < n; i++)
        {ft    = SC_VA_ARG(char *);
	 fs[i] = CSTRSAVE(ft);};

    SC_VA_END;

    f->type_face   = CSTRSAVE(name);
    f->n_styles    = n;
    f->type_styles = fs;
    f->next        = next;

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_VIEW_ATTRIBUTES - initialize and set a view attributes struct */

PG_view_attributes *PG_make_view_attributes(PG_device *dev)
   {PG_view_attributes *d;

    d = CMAKE(PG_view_attributes);
    PG_save_view_attributes(d, dev);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SAVE_VIEW_ATTRIBUTES - save the view attributes */

void PG_save_view_attributes(PG_view_attributes *d, PG_device *dev)
   {int id;

    d->finished = dev->finished;
    d->g        = dev->g;

    for (id = 0; id < PG_SPACEDM; id++)
        {d->gcur[id] = dev->gcur[id];
	 d->tcur[id] = dev->tcur[id];};

    for (id = 0; id < PG_BOXSZ; id++)
        d->view_x[id] = dev->view_x[id];

/* viewport location/size in window in NDC */
    d->view_aspect = dev->view_aspect;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RESTORE_VIEW_ATTRIBUTES - restore the PG_view_attributes to the
 *                            - graphical device
 *
 * #bind PG_restore_view_attributes fortran() scheme() python()
 */

void PG_restore_view_attributes(PG_device *dev ARG(,,cls),
				PG_view_attributes *d)
   {int id;

    dev->finished = d->finished;
    dev->g        = d->g;

    for (id = 0; id < PG_SPACEDM; id++)
        {dev->gcur[id] = d->gcur[id];
	 dev->tcur[id] = d->tcur[id];};

    for (id = 0; id < PG_BOXSZ; id++)
        dev->view_x[id] = d->view_x[id];

/* viewport location/size in window in NDC */
    dev->view_aspect = d->view_aspect;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_CURVE - allocate and initialize a curve
 *               - CS is one of: WORLDC, NORMC, PIXELC
 */

PG_curve *PG_make_curve(PG_device *dev, PG_coord_sys cs, int closed, int n,
			double *xo, double **r)
   {int i;
    int *xi[PG_SPACEDM];
    double p[PG_SPACEDM], ixo[PG_SPACEDM], ix[PG_SPACEDM];
    PG_curve *crv;

    crv = NULL;

    if (dev != NULL)
       {p[0] = xo[0];
        p[1] = xo[1];
          
	if (cs == WORLDC)
	   PG_log_point(dev, 2, p);
	PG_trans_point(dev, 2, cs, p, PIXELC, ixo);

	crv   = CMAKE(PG_curve);
	xi[0] = CMAKE_N(int, n);
	xi[1] = CMAKE_N(int, n);

	for (i = 0; i < n; i++)
	    {p[0] = r[0][i];
	     p[1] = r[1][i];
          
	     if (cs == WORLDC)
	        PG_log_point(dev, 2, p);
	     PG_trans_point(dev, 2, cs, p, PIXELC, ix);

	     xi[0][i] = ix[0] - ixo[0];
	     xi[1][i] = ix[1] - ixo[1];};

	crv->closed    = closed;
	crv->n         = n;
	crv->x         = xi[0];
	crv->y         = xi[1];
	crv->x_origin  = ixo[0];
	crv->y_origin  = ixo[1];
	crv->rx_origin = xo[0];
	crv->ry_origin = xo[1];};

    return(crv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_RELEASE_CURVE - free a curve */

void PG_release_curve(PG_curve *crv)
   {

    CFREE(crv->x);
    CFREE(crv->y);
    CFREE(crv);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_MAKE_BOX_CURVE - make a curve from a rectangle description */

PG_curve *PG_make_box_curve(PG_device *dev, PG_coord_sys cs, double *co, double *cbx)
   {double px[5], py[5];
    double *r[PG_SPACEDM];
    PG_curve *crv;

    px[0] = cbx[0];
    py[0] = cbx[2];

    px[1] = cbx[1];
    py[1] = cbx[2];

    px[2] = cbx[1];
    py[2] = cbx[3];

    px[3] = cbx[0];
    py[3] = cbx[3];

    px[4] = cbx[0];
    py[4] = cbx[2];

    r[0] = px;
    r[1] = py;

    crv = PG_make_curve(dev, cs, TRUE, 5, co, r);

    return(crv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_COPY_CURVE - copy a curve for another device */

PG_curve *PG_copy_curve(PG_curve *icv, PG_device *odv, PG_device *idv)
   {int i, n;
    int *xo, *yo, *xi, *yi;
    double p[PG_SPACEDM], t[PG_SPACEDM];
    double ixo[PG_SPACEDM], oxo[PG_SPACEDM];
    PG_curve *ocv;

    ocv = CMAKE(PG_curve);

    *ocv = *icv;

/* convert the origin */
    ixo[0] = icv->x_origin;
    ixo[1] = icv->y_origin;

    PG_trans_point(idv, 2, PIXELC, ixo, NORMC, p);
    PG_trans_point(odv, 2, NORMC, p, PIXELC, oxo);
 
    n  = icv->n;
    xi = icv->x;
    yi = icv->y;

    xo = CMAKE_N(int, n);
    yo = CMAKE_N(int, n);

/* convert the curve points */
    for (i = 0; i < n; i++)
        {t[0] = ixo[0] + *xi++;
	 t[1] = ixo[1] + *yi++;
          
	 PG_trans_point(idv, 2, PIXELC, t, NORMC, t);
	 PG_trans_point(odv, 2, NORMC, t, PIXELC, t);
 
	 xo[i] = t[0] - oxo[0];
	 yo[i] = t[1] - oxo[1];};

    ocv->x         = xo;
    ocv->y         = yo;
    ocv->x_origin  = oxo[0];
    ocv->y_origin  = oxo[1];
    ocv->rx_origin = p[0];
    ocv->ry_origin = p[1];

    return(ocv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
