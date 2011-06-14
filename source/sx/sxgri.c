/*
 * SXGRI.C - graphical interface for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

char
 *SX_pui_file = NULL,
 *SX_GRI_title = NULL,
 *SX_GRI_type_face = NULL,
 *SX_GRI_type_style = NULL;

int
 SX_GRI_type_size = 12;

double
 SX_GRI_x  = 0.5,
 SX_GRI_y  = 0.01,
 SX_GRI_dx = 0.0,
 SX_GRI_dy = 0.0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GIOB - print a g_interface_object */

static void _SX_wr_giob(SS_psides *si, object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<INTERFACE_OBJECT|%s>",
                              INTERFACE_OBJECT_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RL_GIOB - gc a interface_object */

static void _SX_rl_giob(SS_psides *si, object *obj)
   {int rc;
    PG_interface_object *iob;

    iob = INTERFACE_OBJECT(obj);
    rc  = SC_mark(iob, -1);
    if (rc < 1)
       SX_rem_iob(iob, TRUE);

    SS_rl_object(si, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_IOB - encapsulate a PG_interface_object as an object */

static object *_SX_mk_iob(SS_psides *si, PG_interface_object *iob)
   {object *op;

    op = SS_mk_object(si, iob, G_INTERFACE_OBJECT, SELF_EV, iob->name,
		      _SX_wr_giob, _SX_rl_giob);
    SC_mark(iob, 1);

    return(op);}

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
    if ((dev != NULL) && (SX_pui_file != NULL))
       PG_write_interface(dev, SX_pui_file);

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

/* SX_ADD_TEXT_ANN - register a TEXT interface object with the device
 *                 - this is intended for annotations of plots
 */

static PG_interface_object *SX_add_text_ann(PG_device *dev, double *ndc,
                                            char *s, int clr,
					    PG_text_alignment align,
                                            double ang)
   {int flags[3];
    double xo[PG_SPACEDM];
    PG_interface_object *iob;
    PG_curve *crv;
    PG_text_box *b;

    if (dev == NULL)
       return(NULL);

    ONCE_SAFE(TRUE, NULL)
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
    clr = _PG_trans_color(dev, clr);

    iob = PG_make_interface_object(dev, s, PG_TEXT_OBJECT_S, NULL, align, ang,
				   crv, flags, clr, dev->BLACK,
                                   NULL, NULL, "SX_annot_action",
                                   NULL);

    PG_register_interface_object(dev, iob);

    b = (PG_text_box *) iob->obj;
    b->border = 0.0;

    PG_draw_interface_object(iob);
    PG_update_vs(dev);

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
            G_DEVICE, &dvd,
            G_DEVICE, &dvs,
	    SC_INT_I, &rm,
            0);

    if ((dvd == NULL) || (dvs == NULL))
       SS_error(si, "BAD DEVICE - _SXI_COPY_IOB", argl);

    PG_copy_interface_objects(dvd, dvs, rm);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ADD_ANNOT - add an annotation to the given device */

static object *_SXI_add_annot(SS_psides *si, object *argl)
   {int clr;
    char *s;
    double ndc[PG_BOXSZ], ang;
    PG_text_alignment aln;
    PG_interface_object *iob;
    PG_device *dev;
    object *rv;

    PG_box_init(2, ndc, 0.0, 0.0);

    rv  = SS_null;
    dev = NULL;
    s   = NULL;
    clr = 5;
    aln = CENTER;
    ang = 0.0;
    SS_args(si, argl,
            G_DEVICE,     &dev,
            SC_STRING_I,  &s,
	    SC_INT_I, &clr,
	    SC_DOUBLE_I,  &ndc[0],
	    SC_DOUBLE_I,  &ndc[1],
	    SC_DOUBLE_I,  &ndc[2],
	    SC_DOUBLE_I,  &ndc[3],
	    SC_ENUM_I,    &aln,
	    SC_DOUBLE_I,  &ang,
            0);

    if (dev == NULL)
       dev = SX_graphics_device;

    if (dev == NULL)
       SS_error(si, "BAD DEVICE - _SXI_ADD_ANNOT", SS_null);

    if ((SS_length(si, argl) < 7) || (ndc[0] == ndc[1]))
       PG_define_region(dev, NORMC, ndc);

    iob = SX_add_text_ann(dev, ndc, s, clr, aln, ang*DEG_RAD);

    if (iob != NULL)
       rv = _SX_mk_iob(si, iob);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FREE_IOBS - remove all interface objects from the specified device */

static object *_SXI_free_iobs(SS_psides *si, object *argl)
   {PG_device *dev;

    dev = NULL;
    SS_args(si, argl,
            G_DEVICE, &dev,
            0);

    if (dev == NULL)
       dev = SX_graphics_device;

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
   {int i, flag, sdx, sdy, nc;
    int *clrmd, *nlev, *mrki, *labcf, *labln, *labts;
    double ndc[PG_BOXSZ];
    double *axsca, *axslw, *axsmts, *axsmjt, *axsmnt;
    double *axson, *axndec, *cntrat, *labsp, *mrks;
    char *s, *name, **axstf, **axslxf, **axslyf;
    out_device *out;

    if (PG_console_device == NULL)
       return(SS_f);

    SS_args(si, toggle,
	    SC_INT_I, &flag,
	    0);

    if (flag)
       {PG_IO_INTERRUPTS(TRUE);

	axsca  = PG_ptr_attr_glb("axis-char-angle");
	axslxf = PG_ptr_attr_glb("axis-label-x-format");
	axslyf = PG_ptr_attr_glb("axis-label-y-format");
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
	mrki   = PG_ptr_attr_glb("marker-index");
	mrks   = PG_ptr_attr_glb("marker-scale");

        _SX.chi_mn = -180.0;
        _SX.chi_mx =  180.0;
        _SX.phi_mn = -180.0;
        _SX.phi_mx =  180.0;
        _SX.th_mn  =    0.0;
        _SX.th_mx  =  180.0;

	if (SX_GRI_type_face == NULL)
	   SX_GRI_type_face = CSTRSAVE("helvetica");

	if (SX_GRI_type_style == NULL)
	   SX_GRI_type_style = CSTRSAVE("medium");

	if (SX_GRI_title == NULL)
	   SX_GRI_title = CSTRSAVE("PDBView Controls");

	if (*axslxf == NULL)
	   *axslxf = CSTRSAVE("%10.2g");

	if (*axslyf == NULL)
	   *axslyf = CSTRSAVE("%10.2g");

	if (*axstf == NULL)
	   *axstf = CSTRSAVE("helvetica");

/* connect the I/O functions */
	_SX.gri = PG_make_device("WINDOW", "COLOR", SX_GRI_title);

	PG_query_screen(PG_console_device, &sdx, &sdy, &nc);
        if (SX_GRI_dx == 0.0)
           SX_GRI_dx = 30.0*SX_GRI_type_size/((double) sdx);

        if (SX_GRI_dy == 0.0)
           SX_GRI_dy = 15.0*SX_GRI_type_size/((double) sdy);

	PG_open_device(_SX.gri, SX_GRI_x, SX_GRI_y, SX_GRI_dx, SX_GRI_dy);

	ndc[0] = 0.01;
	ndc[1] = 0.99;
	ndc[2] = 0.1;
	ndc[3] = 0.99;

	PG_set_viewspace(_SX.gri, 2, NORMC, ndc);
	PG_set_viewspace(_SX.gri, 2, WORLDC, NULL);

	PG_set_mouse_down_event_handler(_SX.gri, _SX_mouse_event_handler);

	SC_REGISTER_CONTEXT(_SX_mouse_event_handler, si);

	PG_register_callback("Edit", _SX_set_edit_mode);
	PG_register_callback("End", SX_end_prog);
	PG_register_callback("Save", _SX_save_gri);
	PG_register_callback("ReDraw", _SX_clear_window);

/* axis controls */
	PG_register_variable("Axis", SC_INT_S,
			     axson, NULL, NULL);
	PG_register_variable("Grid", SC_INT_S,
			     &SX_grid, NULL, NULL);
	PG_register_variable("Max Major Ticks", SC_INT_S,
			     axsmjt, NULL, NULL);
	PG_register_variable("# Minor Ticks", SC_INT_S,
			     axsmnt, NULL, NULL);
	PG_register_variable("# Decades", SC_DOUBLE_S,
			     axndec, NULL, NULL);
	PG_register_variable("axis_char_angle", SC_DOUBLE_S,
			     axsca, NULL, NULL);
	PG_register_variable("Line Width", SC_DOUBLE_S,
			     axslw, NULL, NULL);
	PG_register_variable("Tick Size", SC_DOUBLE_S,
			     axsmts, NULL, NULL);
	PG_register_variable("X Axis", SC_STRING_S,
			     axslxf, NULL, NULL);
	PG_register_variable("Y Axis", SC_STRING_S,
			     axslyf, NULL, NULL);

	PG_register_variable("Left Offset", SC_DOUBLE_S,
			     &SX_gpad[0], NULL, NULL);
	PG_register_variable("Right Offset", SC_DOUBLE_S,
			     &SX_gpad[1], NULL, NULL);
	PG_register_variable("Bottom Offset", SC_DOUBLE_S,
			     &SX_gpad[2], NULL, NULL);
	PG_register_variable("Top Offset", SC_DOUBLE_S,
			     &SX_gpad[3], NULL, NULL);

/* font controls */
	PG_register_variable("Type Face", SC_STRING_S,
			     axstf, NULL, NULL);
	PG_register_variable("Type Size", SC_INT_S,
			     &SX_plot_type_size, NULL, NULL);
	PG_register_variable("Type Style", SC_STRING_S,
			     &SX_plot_type_style, NULL, NULL);

/* label controls */
	PG_register_variable("Label Color Flag", SC_INT_S,
			     labcf, NULL, NULL);
	PG_register_variable("Label Length", SC_INT_S,
			     labln, NULL, NULL);
	PG_register_variable("Label Type Size", SC_INT_S,
			     labts, NULL, NULL);
	PG_register_variable("Label Space", SC_DOUBLE_S,
			     labsp, NULL, NULL);

/* marker controls */
	PG_register_variable("Marker Index", SC_INT_S,
			     mrki, NULL, NULL);
	PG_register_variable("Orientation", SC_DOUBLE_S,
			     &SX_marker_orientation, NULL, NULL);
	PG_register_variable("Marker Scale", SC_DOUBLE_S,
			     mrks, NULL, NULL);

/* math controls */
	PG_register_variable("Smooth Method", SC_STRING_S,
			     &SX_smooth_method, NULL, NULL);

/* mouse location controls */
	PG_register_variable("Mouse", SC_INT_S, 
			     &SX_show_mouse_location, NULL, NULL);
	PG_register_variable("X Location", SC_DOUBLE_S,
			     &SX_show_mouse_location_x, NULL, NULL);
	PG_register_variable("Y Location", SC_DOUBLE_S,
			     &SX_show_mouse_location_y, NULL, NULL);

/* output controls */
        for (i = 0; i < N_OUTPUT_DEVICES; i++)
	    {out = SX_get_device(i);

	     s = SC_dsnprintf(FALSE, "%s Flag", out->dupp);
	     PG_register_variable(s, SC_INT_S,
				  &out->active, NULL, NULL);
	     s = SC_dsnprintf(FALSE, "%s Name", out->dupp);
	     PG_register_variable(s, SC_STRING_S,
				  &out->fname, NULL, NULL);
	     s = SC_dsnprintf(FALSE, "%s Type", out->dupp);
	     PG_register_variable(s, SC_STRING_S,
				  &out->type, NULL, NULL);};

/* rendering controls */
	PG_register_variable("1D->1D", SC_INT_S,
			     &SX_render_1d_1d, NULL, NULL);
	PG_register_variable("2D->1D", SC_INT_S,
			     &SX_render_2d_1d, NULL, NULL);
	PG_register_variable("2D->2D", SC_INT_S,
			     &SX_render_2d_2d, NULL, NULL);
	PG_register_variable("# Contour Levels", SC_INT_S,
			     nlev, NULL, NULL);
	PG_register_variable("Contour Ratio", SC_DOUBLE_S,
			     cntrat, NULL, NULL);
	PG_register_variable("Chi", SC_DOUBLE_S,
			     &SX_chi, &_SX.chi_mn, &_SX.chi_mx);
	PG_register_variable("Phi", SC_DOUBLE_S,
			     &SX_phi, &_SX.phi_mn, &_SX.phi_mx);
	PG_register_variable("Theta", SC_DOUBLE_S,
			     &SX_theta, &_SX.th_mn, &_SX.th_mx);
	PG_register_variable("Default Color", SC_INT_S,
			     &SX_default_color, NULL, NULL);

/* window controls */
	PG_register_variable("Border Width", SC_INT_S,
			     &SX_border_width, NULL, NULL);
	PG_register_variable("Clear Mode", SC_INT_S,
			     clrmd, NULL, NULL);

/* tty output controls */
	PG_register_variable("answer_prompt", SC_STRING_S,
			     si->ans_prompt, NULL, NULL);
	PG_register_variable("prompt", SC_STRING_S,
			     si->prompt, NULL, NULL);

	name = SC_search_file(NULL, SX_pui_file);
	if (name == NULL)
	   {PRINT(stderr, "Can't find %s\n", SX_pui_file);};

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
