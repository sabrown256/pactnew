/*
 * ULCNTL.C - control structure functions for Ultra
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "ultra.h"
#include "scope_raster.h"

static void
 UL_draw_plot(PG_device *dev);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_ID_STR - return a string representation of data-id for curve I */

char *_UL_id_str(int i, int j)
   {static char idstr[80];

    if ((SX_gs.dataset[i].id >= 'A') &&	(SX_gs.dataset[i].id <= 'Z'))
       snprintf(idstr, 80, "%c", SX_gs.dataset[j].id);
    else
       snprintf(idstr, 80, "@%d", SX_gs.dataset[j].id);

    return(idstr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_INIT_HASH - set up the Ultra hash table */

void UL_init_hash(void)
   {

    return;}

/*--------------------------------------------------------------------------*/

/*                          DOCUMENTATION FUNCTIONS                         */

/*--------------------------------------------------------------------------*/

/* _ULI_DESCRIBE - the Ultra documentation function */

object *_ULI_describe(SS_psides *si, object *argl)
   {object *obj;

    SX_prep_arg(si, argl);

    for ( ; !SS_nullobjp(argl); argl = SS_cdr(si, argl))
        {obj = SS_car(si, argl);
         if (obj != NULL)
            {if (!SS_prim_des(si, si->outdev, obj))
                PRINT(stdout, " Unknown function\n");};};

    UL_pause(si, TRUE);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_APROPOS - finds all commands which contain the substring s in their 
 *              - name or description
 */

object *_ULI_apropos(SS_psides *si, object *obj)
   {int flag;
    char *s;

    s = SS_get_string(obj);
    SC_banner("APROPOS");
    PRINT(stdout, "Apropos: %s\n\n", s);

/* search SCHEME hash table */
    flag = SS_prim_apr(si, stdout, s);
    if (!flag)
       PRINT(stdout, "No documentation on %s\n", s);

    UL_pause(si, TRUE);

    return(SS_f);}

/*--------------------------------------------------------------------------*/

/*                         CURVE PLOTTING ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _UL_PRINT_OUT_DEVICE - print to the specified output device */

static void _UL_print_out_device(SS_psides *si, int idev)
   {int dx[PG_SPACEDM];
    PG_device *dev;
    PG_dev_geometry *g;
    out_device *out;

    out = SX_get_device(idev);

    if (out->active)
       {dev = out->dev;
	if (dev == NULL)
           {if ((out->fname == NULL) || (strlen(out->fname) == 0))
	       SS_error(si,
			  "OUTPUT FILE NAME IS NULL - _UL_PRINT_OUT_DEVICE",
			  SS_null);
            if ((out->type == NULL) || (strlen(out->type) == 0))
               SS_error(si,
			  "OUTPUT FILE TYPE IS NULL - _UL_PRINT_OUT_DEVICE",
			  SS_null);
            dev = PG_make_device(out->dupp, out->type, out->fname);
            UL_set_graphics_state(dev);
            dev = PG_open_device(dev, out->x0[0], out->x0[1],
				 out->dx0[0], out->dx0[1]);

            if (dev == NULL)
               SS_error(si,
			  "CAN'T OPEN OUTPUT FILE - _UL_PRINT_OUT_DEVICE",
			  SS_null);

	    out->dev = dev;};

/* the set graphics state operation is necessary after PG_open_device
 * because a set window is done then which overrides gymin, gymax, etc
 */
	if (dev != NULL)
	   {g = &dev->g;

	    dx[0] = PG_window_width(dev);
	    dx[1] = PG_window_height(dev);

	    UL_set_graphics_state(dev);

	    g->hwin[1] = g->hwin[0] + dx[0];
	    g->hwin[3] = g->hwin[2] + dx[1];

	    UL_draw_plot(dev);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_PRINTSCR - dump the screen to the printer */

object *_ULI_printscr(SS_psides *si)
   {int i;

    for (i = 0; i < N_OUTPUT_DEVICES; i++)
        _UL_print_out_device(si, i);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _UL_FIX_TEXT_BUF - fix discrepancy in text buffer size between
 *                  - screen box, SBX, and output file box, FBX
 */

static void _UL_fix_text_buf(PG_text_box *fbx, PG_text_box *sbx)
   {int i, nl, nc;
    char **bf;

    nl = (fbx->n_lines > sbx->n_lines) ?
         fbx->n_lines : sbx->n_lines;
    nc = (fbx->n_chars_line > sbx->n_chars_line) ?
         fbx->n_chars_line : sbx->n_chars_line;

/* allocate the space */
    bf = CMAKE_N(char *, nl);
    for (i = 0; i < nl; i++)
        bf[i] = CMAKE_N(char, nc);

/* copy the contents
 * don't copy more strings than we've got; free up old string space
 */
    for (i = 0; i < fbx->n_lines; i++)
        {SC_strncpy(bf[i], nc, fbx->text_buffer[i], -1);
	 CFREE(fbx->text_buffer[i]);}

    fbx->text_buffer  = bf;
    fbx->n_lines      = nl;
    fbx->n_chars_line = nc;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_PRINT_CURVE_LABELS - print the curve labels at the bottom
 *                        - of the screen/page
 */

static void _UL_print_curve_labels(PG_device *dev, int *mark)
   {int i, j, tc, ne;
    int nlabs, sqzlab, ndi, labcf, labln, labts;
    int *dataid, *modified, *clr;
    double labsp, yo;
    double *extr;
    curve *data;
    char **labels, **files;

    PG_get_attrs_glb(TRUE,
		     "label-color-flag", &labcf,
		     "label-length", &labln,
		     "label-space", &labsp,
		     "label-type-size", &labts,
		     "squeeze-labels", &sqzlab,
		     NULL);

    if (HARDCOPY_DEVICE(dev) || (labsp > 0.0))
       {labels   = CMAKE_N(char *, SX_gs.n_curves);
	files    = CMAKE_N(char *, SX_gs.n_curves);
	dataid   = CMAKE_N(int, SX_gs.n_curves);
	modified = CMAKE_N(int, SX_gs.n_curves);
	clr      = CMAKE_N(int, SX_gs.n_curves);

        extr     = CMAKE_N(double, 4 * SX_gs.n_curves);
                        
	data  = SX_gs.dataset;
	nlabs = 0;
	ne    = 0;
	yo    = HUGE;
	for (j = 0; j < SX_gs.n_curves; j++)
	    {if (mark[j] >= 0)
	        {i = mark[j];

		 PG_get_attrs_alist(data[i].info,
				    "LINE-COLOR", SC_INT_I, &tc, dev->WHITE,
				    NULL);
		 if (labcf == FALSE)
		    tc = (tc == -1) ? tc : dev->WHITE;

		 PG_get_attrs_glb(TRUE,
				  "numeric-data-id", &ndi,
				  NULL);

		 dataid[nlabs]   = (ndi == TRUE) ? data[i].id - 'A' + 1 : 
                                                   data[i].id;
		 modified[nlabs] = data[i].modified;
		 labels[nlabs]   = data[i].text;
		 files[nlabs]    = data[i].file;
		 clr[nlabs]      = tc;

		 if ((labln < 40) || (sqzlab == TRUE))
		    {extr[ne++] = data[i].wc[0];
		     extr[ne++] = data[i].wc[1];
		     extr[ne++] = data[i].wc[2];
		     extr[ne++] = data[i].wc[3];};

		 nlabs++;};};

	PG_print_label_set(&yo, dev, nlabs, labels, files,
			   dataid, modified, clr, extr,
			   "CURVE", labln, labcf,
			   sqzlab, labts);

	CFREE(labels);
	CFREE(files);
	CFREE(dataid);
	CFREE(modified);
	CFREE(clr);
        CFREE(extr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _UL_DRAW_PLOT - main plot control routine */

static void _UL_draw_plot(PG_device *dev)
   {int i, color, first;
    int ptyp, m;
    int pltlb, ndi, axsty;
    curve *data;
    pcons *info;
    double wc[PG_BOXSZ];
    PG_rendering pty;

    if (dev == NULL)
       return;

    PG_get_attrs_glb(TRUE,
		     "axis-type",       &axsty,
		     "numeric-data-id", &ndi,
		     "plot-labels",     &pltlb,
		     "plot-type",       &pty,
		     NULL);

    first = TRUE;

    PG_make_device_current(dev);
    PG_clear_window(dev);

/* reinitialize the view space to make user suplied viewspace effective */
    PG_init_viewspace(dev, FALSE);

    UL_plot_limits(dev, TRUE, wc);
    PG_set_viewspace(dev, 2, WORLDC, wc);

/* plot all of the current curves */
    data = SX_gs.dataset;
    for (i = 0; i < SX_gs.n_curves; i++)
        {if (data[i].id != ' ')
            {info = data[i].info;
	     PG_get_attrs_alist(info,
				"PLOT-TYPE",  SC_INT_I, &ptyp,  PLOT_CARTESIAN,
				"LINE-COLOR", SC_INT_I, &color, dev->BLUE,
				NULL);
             if (color >= 0)
                {if ((pty != PLOT_CARTESIAN) ||
		     ((ptyp != PLOT_HISTOGRAM) && (ptyp != PLOT_ERROR_BAR)))
		    {info = PG_set_plot_type(info, pty, axsty);};

                 if (first)
                    {first = FALSE;
                     PG_axis(dev, axsty);};

                 PG_plot_curve(dev, data[i].x[0], data[i].x[1], data[i].n,
                               info, 1);

                 if ((dev->data_id == TRUE) && (pty != PLOT_INSEL))
                    {PG_fset_text_color(dev, color, TRUE);
		     m = (ndi == TRUE) ? data[i].id - 'A' + 1 : data[i].id;
                     PG_draw_data_ids(dev, data[i].x[0], data[i].x[1],
                                      data[i].n, m, info);
                     PG_fset_text_color(dev, dev->WHITE, TRUE);};};};};


    if (first)
       {first = FALSE;
	PG_axis(dev, axsty);};

    if (pltlb == TRUE)
       _UL_print_curve_labels(dev, SX_gs.data_index);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_DRAW_PLOT - coordinate all of the plot drawing */

static void UL_draw_plot(PG_device *dev)
   {

    PG_fset_finish_state(dev, FALSE);

/* draw the curves */
    _UL_draw_plot(dev);

/* update the interface for a hardcopy device
 * the tricky part is mapping the coordinate systems of the different
 * devices - I got lazy and let the read/write interface routines
 * do it at the cost of a temporary file
 */
    if (dev != SX_gs.graphics_device)
       {int i, niobs;
	PG_interface_object *piob, *iob;
	PG_text_box *b, *pb;

/* remove any existing interface objects */
	PG_clear_interface_objects(dev);
        
/* GOTCHA: this has to be reworked for annot to work in -s mode */
        if (SX_gs.graphics_device != NULL)
	   {PG_copy_interface_objects(dev, SX_gs.graphics_device, FALSE);

	    niobs = SC_array_get_n(dev->iobjs);
	    for (i = 0; i < niobs; i++)
	        {iob  = IOB(dev->iobjs, i);
		 piob = IOB(SX_gs.graphics_device->iobjs, i);
                 if (strcmp(iob->type, PG_TEXT_OBJECT_S) == 0)
		    {b  = (PG_text_box *) iob->obj;
                     pb = (PG_text_box *) piob->obj;
                     b->border   = pb->border;
                     b->align    = pb->align;
                     b->angle    = pb->angle;
                     b->standoff = pb->standoff;
                     if ((b->n_lines  != pb->n_lines) ||
			 (b->n_chars_line != pb->n_chars_line))
                        _UL_fix_text_buf(b, pb);};};

	    PG_draw_interface_objects(dev);};}
    else
       PG_draw_interface_objects(dev);

    PG_finish_plot(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_PLOT - main plotting routine
 *         - check the current mode against mode and reinit if different
 *         - before plotting
 */

object *UL_plot(SS_psides *si)
   {double labsp;

    if (si == NULL)
       si = SS_get_current_scheme(-1);       /* diagnostic default */

    PG_get_attrs_glb(TRUE,
		     "label-space", &labsp,
		     NULL);

    if (SX_gs.graphics_device != NULL)
       {if (_SX.last_state != labsp)
           {UL_gs.window_height_factor = (1.0 + labsp)/(1.0 + _SX.last_state);
            _SX.last_state = labsp;
            UL_mode_text(si);
            UL_mode_graphics(si);};

        UL_set_graphics_state(SX_gs.graphics_device);
        UL_draw_plot(SX_gs.graphics_device);};

    PG_make_device_current(PG_gs.console);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* UL_PLOT_LIMITS - return the current plot limits */

void UL_plot_limits(PG_device *dev, int pflg, double *box)
   {int i, j, color, hider;
    curve *data;
    double wc[PG_BOXSZ], plr[PG_SPACEDM], crt[PG_SPACEDM];
    double *pc[PG_SPACEDM];
    double HC;
    pcons *info;
    PG_rendering pty;

    HC = HUGE_REAL*HUGE_REAL*HUGE_REAL;

    PG_box_init(2, box, HUGE_REAL, -HUGE_REAL);

    PG_get_attrs_glb(TRUE,
		     "hide-rescale", &hider,
		     "plot-type",    &pty,
		     NULL);

    data = SX_gs.dataset;
    for (i = 0; i < SX_gs.n_curves; i++)
        {if (data[i].id != ' ')
	    {info = (pcons *) data[i].info;
	     PG_get_attrs_alist(info,
				"LINE-COLOR", SC_INT_I, &color, 1,
				NULL);
             if ((color >= 0) || (hider != TRUE))
                {PG_box_copy(2, wc, data[i].wc);

                 if ((-HC <= wc[0]) && (wc[0] <= HC) &&
		     (-HC <= wc[1]) && (wc[1] <= HC) &&
		     (-HC <= wc[2]) && (wc[2] <= HC) &&
		     (-HC <= wc[3]) && (wc[3] <= HC))
		    {box[0] = min(box[0], wc[0]);
		     box[1] = max(box[1], wc[1]);
		     box[2] = min(box[2], wc[2]);
		     box[3] = max(box[3], wc[3]);};};};};

    if (box[0] >= box[1])
       {box[0] = 0.0;
        box[1] = 1.0;};

    if ((box[2] == box[3]) && (box[2] != 0.0))
       {double dy;

        dy = ABS(0.01*(box[2] + box[3]));
        box[2] -= dy;
        box[3] += dy;}

    else if (box[2] >= box[3])
       {box[2] = 0.0;
        box[3] = 1.0;}

    if (dev == NULL)
       {if (SX_gs.autorange == TRUE)
	   {wc[3] = box[3];
	    wc[2] = box[2];}
	else
	   {wc[3] = SX_gs.gwc[3];
	    wc[2] = SX_gs.gwc[2];};

	if (SX_gs.autodomain == TRUE)
	   {wc[1] = box[1];
	    wc[0] = box[0];}
	else
	   {wc[1] = SX_gs.gwc[1];
	    wc[0] = SX_gs.gwc[0];};}

    else
       {PG_get_viewspace(dev, WORLDC, wc);

	if (SX_gs.autorange == TRUE)
	   {wc[3] = box[3];
	    wc[2] = box[2];};

	if (SX_gs.autodomain == TRUE)
	   {wc[1] = box[1];
	    wc[0] = box[0];};};

    if (pflg && (pty == PLOT_POLAR))
       {PG_box_init(2, box, HUGE_REAL, -HUGE_REAL);

        for (i = 0; i < SX_gs.n_curves; i++)
            {if (data[i].id != ' ')
                {pc[1] = data[i].x[1];
                 pc[0] = data[i].x[0];
                 for (j = 0; j < data[i].n; j++)
                     {plr[1] = pc[1][j];
                      plr[0] = pc[0][j];

                      if (PG_box_contains(2, wc, plr) == TRUE)
			 {crt[0] = plr[1]*cos(plr[0]);
			  crt[1] = plr[1]*sin(plr[0]);

			  box[0] = min(box[0], crt[0]);
			  box[1] = max(box[1], crt[0]);
			  box[2] = min(box[2], crt[1]);
			  box[3] = max(box[3], crt[1]);};};};};

        wc[0] = box[0] - 0.005;
        wc[1] = box[1] + 0.005;
        wc[2] = box[2] - 0.005;
        wc[3] = box[3] + 0.005;};

    PG_box_copy(2, box, wc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_PAUSE - wait if apropriate before going on */

void UL_pause(SS_psides *si, int pf)
   {

    if (pf)
       SX_gs.plot_flag = FALSE;

    if (si->interactive == ON)
       PRINT(stdout, "\n");

    return;}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_INSTALL_GLOBAL_VARS - install the global variables */

void UL_install_global_vars(SS_psides *si)
   {

    SX_install_global_vars(si);

    SS_install_cf(si, "derivative-tolerance",
                  "Variable: Threshold for first derivative change.\n     Usage: derivative-tolerance <double>",
                  SS_acc_double,
                  &UL_gs.derivative_tolerance);

    SS_install_cf(si, "igamma-tolerance",
		  "Procedure: Set the tolerance to which incomplete gamma functions are computed\n     Usage: igamma-tolerance <tol>",
                  SS_acc_double, 
		  &_PM.igamma_tol);

/* KLMN */

/* OPQR */

/* STUV */

    SS_install_cf(si, "save-intermediate",
                  "Variable: Save intermediate curves iff ON\n     Usage: save-intermediate [ on | off]",
                  SS_acc_int,
                  &UL_gs.save_intermediate);

    SS_install_cf(si, "simple-append",
                  "Variable: Do simple concatenations with append-curves\n     Usage: simple-append [ on | off]",
                  SS_acc_int,
                  &UL_gs.simple_append);


/* WXYZ */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
