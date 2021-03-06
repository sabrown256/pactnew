/*
 * SXGRI.C - graphical interface for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PG_INTERFACE_OBJECT - handle BLANG binding related operations */

void *_SX_opt_PG_interface_object(PG_interface_object *x, bind_opt wh, void *a)
   {int rc;
    void *rv;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     break;

        case BIND_LABEL :
        case BIND_PRINT :
	     rv = x->name;
             break;

        case BIND_FREE :
	     rc = SC_mark(x, -1);
	     if (rc < 1)
	        SX_rem_iob(x, TRUE);
             break;

        case BIND_ALLOC :
	     SC_mark(x, 1);
             break;

	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_END_PROG - end the program */

void SX_end_prog(void *d, PG_event *ev)
   {PG_device *dev;

    dev = PG_handle_button_press(d, ev);
    if (dev != NULL)
       {PG_close_device(dev);
        SX_quit(0);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_CLEAR_WINDOW - clear the screen */

static void _SX_clear_window(void *d, PG_event *ev)
   {PG_device *dev;

    dev = PG_handle_button_press(d, ev);
    if (dev != NULL)
       {PG_clear_window(dev);
        PG_draw_interface_objects(dev);

        PG_update_vs(dev);};
/*        PG_finish_plot(dev);}; */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SAVE_GRI - save the interface state */

static void _SX_save_gri(void *d, PG_event *ev)
   {PG_device *dev;

    dev = PG_handle_button_press(d, ev);
    if ((dev != NULL) && (SX_gs.pui_file != NULL))
       PG_write_interface(dev, SX_gs.pui_file);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SET_EDIT_MODE - turn on editing of interface objects */

static void _SX_set_edit_mode(void *d, PG_event *ev)
   {PG_device *dev;

    dev = PG_handle_button_press(d, ev);
    if (dev != NULL)
       PG_set_attrs_glb(TRUE, "edit-mode", TRUE, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MOUSE_EVENT_HANDLER - handle mouse down events */

static void _SX_mouse_event_handler(PG_device *dev, PG_event *ev)
   {PG_interface_object *iob;

    iob = PG_get_object_event(dev, ev);
    if (iob != NULL)
       {PRINT(stdout, "\n%s\n", (char *) iob->obj);}

#ifdef HAVE_WINDOW_DEVICE

    else
       {int mod;
	int iev[PG_SPACEDM];
	PG_mouse_button btn;

        PG_mouse_event_info(dev, ev, iev, &btn, &mod);};

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_ANNOT_ACTION - define the behavior of events wrt to annotations */

static void SX_annot_action(void *d, PG_event *ev)
   {

#ifdef HAVE_WINDOW_DEVICE

    int type;

    if (ev != NULL)
       {type = PG_EVENT_TYPE(*ev);

/* mouse events are handled first since PG_handle_key_press also looks for
 * some mouse events
 */
	if (type == MOUSE_DOWN_EVENT)
           {int btn, mod;
	    int ir[PG_SPACEDM];
	    double bx[PG_BOXSZ];
	    PG_device *dev;
	    PG_interface_object *iob;

	    iob = (PG_interface_object *) d;
	    dev = iob->device;
	    if (dev != NULL)
	       {PG_query_pointer(dev, ir, &btn, &mod);
		switch (btn)
		   {case MOUSE_LEFT :
		         PG_box_init(2, bx, 0, 10000);
		         PG_move_object(iob, bx, TRUE);
			 break;

		    case MOUSE_RIGHT :
			 PG_fset_fill_color(dev, dev->BLACK, TRUE);
			 PG_fill_curve(dev, iob->curve);
			 PG_fset_line_color(dev, dev->BLACK, FALSE);
			 PG_draw_curve(dev, iob->curve, FALSE);
			 PG_fset_line_color(dev, dev->WHITE, TRUE);

			 SX_rem_iob(iob, TRUE);

			 break;};

		PG_update_vs(dev);};}

        else if (type == KEY_DOWN_EVENT)
	   PG_handle_key_press(d, ev);};

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ADD_TEXT_ANN - register a TEXT interface object with the device
 *                  - this is intended for annotations of plots
 */

static PG_interface_object *_SX_add_text_ann(PG_device *dev, double *ndc,
					     char *s, PG_textdes *td)
   {int clr, flags[3];
    double xo[PG_SPACEDM];
    PG_interface_object *iob;
    PG_curve *crv;
    PG_text_box *b;

    iob = NULL;

    if (dev != NULL)
       {ONCE_SAFE(TRUE, NULL)
	   PG_register_callback("SX_annot_action", SX_annot_action);
	END_SAFE;

/* make them visible, selectable, and active */
	flags[0] = TRUE;
	flags[1] = TRUE;
	flags[2] = TRUE;

	if (s == NULL)
	   s = CSTRSAVE("                        ");

	xo[0] = ndc[0];
	xo[1] = ndc[2];

	crv = PG_make_box_curve(dev, NORMC, xo, ndc);

/* make sure color is visible */
	clr = _PG_trans_color(dev, td->color);

	iob = PG_make_interface_object_n(dev, s, PG_TEXT_OBJECT_S, NULL,
					 crv, flags, clr, dev->BLACK,
					 td, NULL, NULL,
					 "SX_annot_action", NULL);

	PG_register_interface_object(dev, iob);

	b = (PG_text_box *) iob->obj;
	b->border = 0.0;

	PG_draw_interface_object(iob);
	PG_update_vs(dev);};

    return(iob);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_COPY_IOB - copy interface objects from one device to another
 *               - Usage:
 *               -    (copy-interface-objects dvd dvs rm)
 */

static object *_SXI_copy_iob(SS_psides *si, object *argl)
   {int rm;
    PG_device *dvd, *dvs;

    dvd = NULL;
    dvs = NULL;
    rm  = FALSE;
    SS_args(si, argl,
            G_PG_DEVICE_I, &dvd,
            G_PG_DEVICE_I, &dvs,
	    G_INT_I, &rm,
            0);

    if ((dvd == NULL) || (dvs == NULL))
       SS_error(si, "BAD DEVICE - _SXI_COPY_IOB", argl);

    PG_copy_interface_objects(dvd, dvs, rm);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ADD_ANNOT - add an annotation to the given device */

static object *_SXI_add_annot(SS_psides *si, object *argl)
   {char *s;
    double ndc[PG_BOXSZ];
    PG_textdes td;
    PG_interface_object *iob;
    PG_device *dev;
    object *rv;

    PG_box_init(2, ndc, 0.0, 0.0);

    td.color = GREEN;
    td.size  = 12;
    td.style = NULL;
    td.face  = NULL;
    td.angle = 0.0;
    td.align = DIR_CENTER;
    td.other = NULL;

    rv  = SS_null;
    dev = NULL;
    s   = NULL;
    SS_args(si, argl,
            G_PG_DEVICE_I,    &dev,
            G_STRING_I, &s,
	    G_INT_I,    &td.color,
	    G_DOUBLE_I, &ndc[0],
	    G_DOUBLE_I, &ndc[1],
	    G_DOUBLE_I, &ndc[2],
	    G_DOUBLE_I, &ndc[3],
	    G_ENUM_I,   &td.align,
	    G_DOUBLE_I, &td.angle,
	    G_INT_I,    &td.size,
	    G_STRING_I, &td.style,
	    G_STRING_I, &td.face,
	    G_STRING_I, &td.other,
            0);

    if (dev == NULL)
       dev = SX_gs.graphics_device;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_ADD_ANNOT", SS_null);

    if ((SS_length(si, argl) < 7) || (ndc[0] == ndc[1]))
       PG_define_region(dev, NORMC, ndc);

    td.angle *= DEG_RAD;

    iob = _SX_add_text_ann(dev, ndc, s, &td);

    if (iob != NULL)
       rv = SX_make_pg_interface_object(si, iob);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FREE_IOBS - remove all interface objects from the specified device */

static object *_SXI_free_iobs(SS_psides *si, object *argl)
   {PG_device *dev;

    dev = NULL;
    SS_args(si, argl,
            G_PG_DEVICE_I, &dev,
            0);

    if (dev == NULL)
       dev = SX_gs.graphics_device;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_FREE_IOBS", argl);

    PG_free_interface_objects(dev);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_REM_IOB - remove an interface object from the device
 *            - if FLAG is TRUE remove the objects children as well
 */

void SX_rem_iob(PG_interface_object *iob, int flag)
   {int i, niobs;
    PG_interface_object *diob;
    PG_device *dev;

    dev   = iob->device;
    niobs = SC_array_get_n(dev->iobjs);
    for (i = 0; i < niobs; i++)
        {diob = IOB(dev->iobjs, i);
	 if (diob == iob)
	    {niobs = SC_array_remove(dev->iobjs, i);
             _PG_rl_interface_object(iob, flag);
             break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_IOBP - function version of SX_INTERFACE_OBJECTP macro */

static object *_SXI_iobp(SS_psides *si, object *obj)
   {object *o;

    o = SX_INTERFACE_OBJECTP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_TOGGLE_GRI - start the graphical interface window */

static object *_SXI_toggle_gri(SS_psides *si, object *toggle)
   {int i, flag, nc;
    int sdx[PG_SPACEDM];
    int *clrmd, *nlev, *mrki, *labcf, *labln, *labts;
    int *axslc, *axsls, *lnclr, *lnsty;
    double ndc[PG_BOXSZ];
    double *axsca, *axslw, *axsmts, *axsmjt, *axsmnt;
    double *axson, *axndec, *cntrat, *labsp, *mrks;
    double *lnwid;
    char *s, *name, **axstf, **axslxf, **axslyf;
    out_device *out;

    if (PG_gs.console == NULL)
       return(SS_f);

    SS_args(si, toggle,
	    G_INT_I, &flag,
	    0);

    if (flag)
       {SC_set_io_interrupts(TRUE);

	axsca  = PG_ptr_attr_glb("axis-char-angle");
	axslxf = PG_ptr_attr_glb("axis-label-x-format");
	axslyf = PG_ptr_attr_glb("axis-label-y-format");
	axslc  = PG_ptr_attr_glb("axis-line-color");
	axsls  = PG_ptr_attr_glb("axis-line-style");
	axslw  = PG_ptr_attr_glb("axis-line-width");
	axsmts = PG_ptr_attr_glb("axis-major-tick-size");
	axsmjt = PG_ptr_attr_glb("axis-max-major-ticks");
	axsmnt = PG_ptr_attr_glb("axis-n-minor-ticks");
	axndec = PG_ptr_attr_glb("axis-n-decades");
	axson  = PG_ptr_attr_glb("axis-on");
	axstf  = PG_ptr_attr_glb("axis-type-face");
	nlev   = PG_ptr_attr_glb("contour-n-levels");
	cntrat = PG_ptr_attr_glb("contour-ratio");
	clrmd  = PG_ptr_attr_glb("hl-clear-mode");
	labcf  = PG_ptr_attr_glb("label-color-flag");
	labln  = PG_ptr_attr_glb("label-length");
	labsp  = PG_ptr_attr_glb("label-space");
	labts  = PG_ptr_attr_glb("label-type-size");
	lnclr  = PG_ptr_attr_glb("line-color");
	lnsty  = PG_ptr_attr_glb("line-style");
	lnwid  = PG_ptr_attr_glb("line-width");
	mrki   = PG_ptr_attr_glb("marker-index");
	mrks   = PG_ptr_attr_glb("marker-scale");

/* setup the euler angles for viewing */
        _SX.ea_mn[0] =    0.0;
        _SX.ea_mx[0] =  180.0;
        _SX.ea_mn[1] = -180.0;
        _SX.ea_mx[1] =  180.0;
        _SX.ea_mn[2] = -180.0;
        _SX.ea_mx[2] =  180.0;

	if (SX_gs.gri_type_face == NULL)
	   SX_gs.gri_type_face = CSTRSAVE("helvetica");

	if (SX_gs.gri_type_style == NULL)
	   SX_gs.gri_type_style = CSTRSAVE("medium");

	if (SX_gs.gri_title == NULL)
	   SX_gs.gri_title = CSTRSAVE("PDBView Controls");

	if (*axslxf == NULL)
	   *axslxf = CSTRSAVE("%10.2g");

	if (*axslyf == NULL)
	   *axslyf = CSTRSAVE("%10.2g");

	if (*axstf == NULL)
	   *axstf = CSTRSAVE("helvetica");

/* connect the I/O functions */
	_SX.gri = PG_make_device("WINDOW", "COLOR", SX_gs.gri_title);

	PG_query_screen_n(PG_gs.console, sdx, &nc);
        if (SX_gs.gri_dx[0] == 0.0)
           SX_gs.gri_dx[0] = 30.0*SX_gs.gri_type_size/((double) sdx[0]);

        if (SX_gs.gri_dx[1] == 0.0)
           SX_gs.gri_dx[1] = 15.0*SX_gs.gri_type_size/((double) sdx[1]);

	PG_open_device(_SX.gri, SX_gs.gri_x[0], SX_gs.gri_x[1], SX_gs.gri_dx[0], SX_gs.gri_dx[1]);

	ndc[0] = 0.01;
	ndc[1] = 0.99;
	ndc[2] = 0.1;
	ndc[3] = 0.99;

	PG_set_viewspace(_SX.gri, 2, NORMC, ndc);
	PG_set_viewspace(_SX.gri, 2, WORLDC, NULL);

	PG_set_mouse_down_event_handler(_SX.gri, _SX_mouse_event_handler);

	SC_REGISTER_CONTEXT(_SX_mouse_event_handler, SS_psides, si);

	PG_register_callback("Edit", _SX_set_edit_mode);
	PG_register_callback("End", SX_end_prog);
	PG_register_callback("Save", _SX_save_gri);
	PG_register_callback("ReDraw", _SX_clear_window);

/* axis controls */
	PG_register_variable("Axis", G_INT_S,
			     axson, NULL, NULL);
	PG_register_variable("Grid", G_INT_S,
			     &SX_gs.grid, NULL, NULL);
	PG_register_variable("Max Major Ticks", G_INT_S,
			     axsmjt, NULL, NULL);
	PG_register_variable("# Minor Ticks", G_INT_S,
			     axsmnt, NULL, NULL);
	PG_register_variable("# Decades", G_DOUBLE_S,
			     axndec, NULL, NULL);
	PG_register_variable("axis_char_angle", G_DOUBLE_S,
			     axsca, NULL, NULL);
	PG_register_variable("Line Color", G_INT_S,
			     axslc, NULL, NULL);
	PG_register_variable("Line Style", G_INT_S,
			     axsls, NULL, NULL);
	PG_register_variable("Line Width", G_DOUBLE_S,
			     axslw, NULL, NULL);
	PG_register_variable("Tick Size", G_DOUBLE_S,
			     axsmts, NULL, NULL);
	PG_register_variable("X Axis", G_STRING_S,
			     axslxf, NULL, NULL);
	PG_register_variable("Y Axis", G_STRING_S,
			     axslyf, NULL, NULL);

	PG_register_variable("Left Offset", G_DOUBLE_S,
			     &SX_gs.gpad[0], NULL, NULL);
	PG_register_variable("Right Offset", G_DOUBLE_S,
			     &SX_gs.gpad[1], NULL, NULL);
	PG_register_variable("Bottom Offset", G_DOUBLE_S,
			     &SX_gs.gpad[2], NULL, NULL);
	PG_register_variable("Top Offset", G_DOUBLE_S,
			     &SX_gs.gpad[3], NULL, NULL);

/* font controls */
	PG_register_variable("Type Face", G_STRING_S,
			     axstf, NULL, NULL);
	PG_register_variable("Type Size", G_INT_S,
			     &SX_gs.plot_type_size, NULL, NULL);
	PG_register_variable("Type Style", G_STRING_S,
			     &SX_gs.plot_type_style, NULL, NULL);

/* label controls */
	PG_register_variable("Label Color Flag", G_INT_S,
			     labcf, NULL, NULL);
	PG_register_variable("Label Length", G_INT_S,
			     labln, NULL, NULL);
	PG_register_variable("Label Type Size", G_INT_S,
			     labts, NULL, NULL);
	PG_register_variable("Label Space", G_DOUBLE_S,
			     labsp, NULL, NULL);

/* marker controls */
	PG_register_variable("Marker Index", G_INT_S,
			     mrki, NULL, NULL);
	PG_register_variable("Orientation", G_DOUBLE_S,
			     &SX_gs.marker_orientation, NULL, NULL);
	PG_register_variable("Marker Scale", G_DOUBLE_S,
			     mrks, NULL, NULL);

/* math controls */
	PG_register_variable("Smooth Method", G_STRING_S,
			     &SX_gs.smooth_method, NULL, NULL);

/* mouse location controls */
	PG_register_variable("Mouse", G_INT_S, 
			     &SX_gs.show_mouse_location, NULL, NULL);
	PG_register_variable("X Location", G_DOUBLE_S,
			     &SX_gs.show_mouse_x[0], NULL, NULL);
	PG_register_variable("Y Location", G_DOUBLE_S,
			     &SX_gs.show_mouse_x[1], NULL, NULL);

/* output controls */
        for (i = 0; i < N_OUTPUT_DEVICES; i++)
	    {out = SX_get_device(i);

	     s = SC_dsnprintf(FALSE, "%s Flag", out->dupp);
	     PG_register_variable(s, G_INT_S,
				  &out->active, NULL, NULL);
	     s = SC_dsnprintf(FALSE, "%s Name", out->dupp);
	     PG_register_variable(s, G_STRING_S,
				  &out->fname, NULL, NULL);
	     s = SC_dsnprintf(FALSE, "%s Type", out->dupp);
	     PG_register_variable(s, G_STRING_S,
				  &out->type, NULL, NULL);};

/* rendering controls */
	PG_register_variable("1D->1D", G_INT_S,
			     &SX_gs.render_1d_1d, NULL, NULL);
	PG_register_variable("2D->1D", G_INT_S,
			     &SX_gs.render_2d_1d, NULL, NULL);
	PG_register_variable("2D->2D", G_INT_S,
			     &SX_gs.render_2d_2d, NULL, NULL);
	PG_register_variable("# Contour Levels", G_INT_S,
			     nlev, NULL, NULL);
	PG_register_variable("Contour Ratio", G_DOUBLE_S,
			     cntrat, NULL, NULL);
	PG_register_variable("Chi", G_DOUBLE_S,
			     &SX_gs.view_angle[2], &_SX.ea_mn[2], &_SX.ea_mx[2]);
	PG_register_variable("Phi", G_DOUBLE_S,
			     &SX_gs.view_angle[1], &_SX.ea_mn[1], &_SX.ea_mx[1]);
	PG_register_variable("Theta", G_DOUBLE_S,
			     &SX_gs.view_angle[0], &_SX.ea_mn[0], &_SX.ea_mx[0]);
	PG_register_variable("Default Color", G_INT_S,
			     lnclr, NULL, NULL);

	PG_register_variable("Default Style", G_INT_S,
			     lnsty, NULL, NULL);

	PG_register_variable("Default Width", G_DOUBLE_S,
			     lnwid, NULL, NULL);

/* window controls */
	PG_register_variable("Border Width", G_INT_S,
			     &SX_gs.border_width, NULL, NULL);
	PG_register_variable("Clear Mode", G_INT_S,
			     clrmd, NULL, NULL);

/* tty output controls */
	PG_register_variable("answer_prompt", G_STRING_S,
			     si->ans_prompt, NULL, NULL);
	PG_register_variable("prompt", G_STRING_S,
			     si->prompt, NULL, NULL);

	name = SC_search_file(NULL, SX_gs.pui_file);
	if (name == NULL)
	   {PRINT(stderr, "Can't find %s\n", SX_gs.pui_file);};

	PG_read_interface(_SX.gri, name);

	CFREE(name);

	_SX_clear_window((void *) _SX.gri, NULL);}

    else
       PG_close_device(_SX.gri);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _SX_INSTALL_PGS_IOB - install the PGS interface object functions */
 
void _SX_install_pgs_iob(SS_psides *si)
   {

    SS_install(si, "pg-interface-object?",
               "Returns #t if the object is a PGS interface object, and #f otherwise",
               SS_sargs,
               _SXI_iobp, SS_PR_PROC);

    SS_install(si, "copy-interface-objects",
               "Procedure: Copy the interface objects from one device to another\n     Usage: copy-interface-objects dst src [rm]",
               SS_nargs,
               _SXI_copy_iob, SS_PR_PROC);

    SS_install(si, "graphical-interface",
               "Procedure: Toggle the graphical interface\n     Usage: graphical-interface [on | off]",
               SS_sargs,
               _SXI_toggle_gri, SS_PR_PROC);

    SS_install(si, "add-annotation",
               "Procedure: Add textual annotation to the specified device\n     Usage: add-annotation <device> [<text> [<color> [<xmin> <xmax> <ymin> <ymax> [<angle>]]]]",
               SS_nargs,
               _SXI_add_annot, SS_PR_PROC);

    SS_install(si, "rem-annotations",
               "Procedure: Remove all annotations\n     Usage: rem-annotations <device>",
               SS_sargs,
               _SXI_free_iobs, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
