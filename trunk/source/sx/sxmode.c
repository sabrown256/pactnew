/*
 * SXMODE.C - the mode routines for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SET_GRAPHICS_STATE - map the PDBView graphics state variables onto the
 *                       - given device before initialization or plotting
 */

static void SX_set_graphics_state(PG_device *d)
   {int i;
    double mrks;
    char *axstf;
    out_device *out;
    PG_dev_geometry *g;

    if (d == NULL)
       return;

    g = &d->g;

    d->autodomain             = SX_autodomain;
    d->autoplot               = SX_autoplot;
    d->autorange              = SX_autorange;
    d->background_color_white = SX_background_color_white;

    out = SX_match_device(d);
    if (out != NULL)
       {if (out->background_color != -1)
	   d->background_color_white = out->background_color;};

    d->border_width = SX_border_width;
    d->data_id      = SX_data_id;
    d->gprint_flag  = TRUE;
    d->grid         = SX_grid;

/* view width and height are set by users so push it
 * into the view_x box
 */
    SX_view_x[1] = SX_view_x[0] + SX_view_width;
    SX_view_x[3] = SX_view_x[2] + SX_view_height;

    d->view_aspect  = SX_view_aspect;

    for (i = 0; i < PG_BOXSZ; i++)
        {d->view_x[i] = SX_view_x[i];

/* viewport limits in WC */
         g->wc[i] = SX_gwc[i];

/* WC to BND pad */
	 g->pad[i] = SX_gpad[i];};

    PG_get_attrs_glb(TRUE,
		     "axis-type-face", &axstf,
                     "marker-scale",   &mrks,
		     NULL);

    PG_fset_axis_log_scale(d, 2, SX_log_scale);
    PG_fset_font(d, axstf, SX_plot_type_style, SX_plot_type_size);
    PG_fset_marker_scale(d, mrks);
    PG_fset_marker_orientation(d, SX_marker_orientation);

    if (!POSTSCRIPT_DEVICE(d))
       {g->hwin[0] = SX_window_P[0];
        g->hwin[2] = SX_window_P[1];};

    g->hwin[1] = g->hwin[0] + SX_window_width_P;
    g->hwin[3] = g->hwin[2] + SX_window_height_P;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_EXPAND_PREFIX - expand prefix expressions in referring to menu items
 *                   - Because of possible user defined synonyms, this must
 *                   - be done for every command, not just select. This step
 *                   - could come later and be applied only to number lists,
 *                   - if range expansion were moved to the handlers.
 *
 * GOTCHA - Data references fail if the file name contains '.' (e.g. file.pdb)
 */

static void _SX_expand_prefix(char *s, int nc)
   {int ns, index, more_tokens;
    char t[MAXLINE], bf[MAXLINE], token[MAXLINE];
    char *tp, *fp, *ip, *pt;

    strcpy(t, s);
    ns = 0;

    more_tokens = TRUE;
    
    while (more_tokens)
       {token[0] = '\0';
        if (*t == '\"')
           {tp = SC_firsttok(t, "\"");
	    if (tp == NULL)
	       {token[0] = '\"';
		token[1] = '\"';
		token[2] = '\0';}
	    else
	       {token[0] = '\"';
		strcpy(token + 1, tp);
		SC_strcat(token, MAXLINE, "\"");};}

        else if ((tp = SC_firsttok(t, " \t\n\r")) != NULL)
	   strcpy(token, tp);
	else
	   more_tokens = FALSE;

        if (strlen(token) > 0)
           {if ((tp != NULL) && isalpha((int) *tp))
               {strcpy(bf, tp);
                fp = SC_strtok(bf, ".()[]", pt);
	        if ((fp != NULL) && (*(token + strlen(fp)) == '.'))
	           {ip = SC_strtok(NULL, "()[]", pt);
	            if ((ip != NULL) && (SC_intstrp(ip, 10)))
	               {index = ATOL(ip);
		        snprintf(token, MAXLINE, "(data-reference %d %s)%s",
				 index, fp,
				 tp + strlen(fp) + 1 + strlen(ip));};};}

	    snprintf(s+ns, nc-ns, "%s ", token);
	    ns += strlen(token) + 1;};};
 
    ns = min(ns, nc-1);
    *(s + ns) = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_REPROC_IN - reprocess the contents of the input buffer
 *               - treat the contents of the buffer as an implicit list
 *               - by effectively wrapping parens about the contents of
 *               - the buffer
 */

static char *_SX_reproc_in(SS_psides *si, char *line)
   {char *rv;

    if (!SX_split_command(_SX.command, line))
       rv = NULL;

    else
       {if (!SX_expand_expr(_SX.command))
           SS_error(si, "SYNTAX ERROR - _SX_REPROC_IN", SS_null);

        _SX_expand_prefix(_SX.command, MAXLINE);

        SX_wrap_paren("(", _SX.command, ")", MAXLINE);

        if (SX_command_log != NULL)
           PRINT(SX_command_log, "%s\n", _SX.command);

        rv = _SX.command;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PLOT - replot the requesite frames */

object *SX_plot(SS_psides *si)
   {object *var, *fnc;

    var = (object *) SC_hasharr_def_lookup(si->symtab, "viewport-update");
    if (var != NULL)
       {if (SS_bind_env(si, var, si->env) != NULL)
	   {fnc = SS_lk_var_val(si, var);
	    if (SS_procedurep(fnc))
	       SS_call_scheme(si, "viewport-update", 0);};};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PARSE - determine whether or not to reprocess the input for PDBView
 *           - this is the real worker for the si->post_eval
 */

static void _SX_parse(SS_psides *si, object *strm)
   {

    SX_parse(si, SX_plot, _SX_reproc_in, strm);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_READ - this is the si->post_read function for PDBView
 *          - trap implicit requests to print or change file variable
 *          - values and make them explicit
 */

static void _SX_read(SS_psides *si, object *strm)
   {char *s, *ptr, *t, *bf;
    g_file *po;
    object *o;
    PDBfile *file;

    s = NULL;
    o = si->rdobj;
    SS_args(si, o,
            SC_STRING_I, &s,
            0);

    bf = NULL;

/* anything without a print name generates a replot */
    if ((s != NULL) && strcmp(s, "- no print name -") == 0)
       {t = SS_BUFFER(strm);

        bf = SC_dsnprintf(FALSE, "(wu)");}

/* check for variables and special actions for certain cases depending
 * on the name and the namespace it is found in
 */
    else if (SS_variablep(o))
       {SS_var_value(si, "current-file", G_FILE, &po, TRUE);
	if (po == NULL)
	   file = SX_vif;
        else
	   file = FILE_FILE(PDBfile, po);

	t   = SS_BUFFER(strm);
	ptr = SS_PTR(strm);

/* look for file variable expression */
	if (SS_bind_env(si, o, si->env) == NULL)
	   {if (_SX_file_varp(file, s, FALSE))

/* find case */
	       {if ((*ptr != '\0') && (strchr("!<>=", *ptr) != NULL))
		   bf = SC_dsnprintf(FALSE, "varfind %s", t);

/* change case */
		else if ((*ptr != '\0') && (*ptr != '\n'))
		   bf = SC_dsnprintf(FALSE, "varset! %s", t);

/* print case */
		else
		   bf = SC_dsnprintf(FALSE, "varprint %s", t);};}

/* look for type name - define case */
	else if ((strcmp(s, "struct") != 0) && (SC_type_id(s, FALSE) != -1))
	   bf = SC_dsnprintf(FALSE, "vardef %s", t);};

    if (bf != NULL)
       {strcpy(t, bf);
        SS_PTR(strm) = t;
        SS_assign(si, si->rdobj, SS_read(si, strm));};

    CFREE(s);

    return;}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PRINT - the si->post_print function for PDBView */

int _SX_print(SS_psides *si)
   {

    if (PG_console_device != NULL)
       PG_console_device->gprint_flag = TRUE;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MODE_TEXT - go into text mode and
 *              - return "#t" if text mode was already in effect
 *              - else return "#f"
 */

object *SX_mode_text(SS_psides *si)
   {object *ret;

    if (PG_console_device == NULL)
       PG_open_console("PDBView", SX_console_type, SX_background_color_white,
                       SX_console_x, SX_console_y,
                       SX_console_width, SX_console_height);

    if (SX_graphics_device != NULL)
       {PG_clear_window(SX_graphics_device);
        PG_close_device(SX_graphics_device);
        SX_graphics_device = NULL;

/* give default values to the lisp package interface variables */
        si->post_read  = NULL;
        si->post_eval  = NULL;
        si->post_print = NULL;
	si->pr_gets    = _SX_get_input;
        si->pr_ch_un   = SS_unget_ch;
        si->pr_ch_out  = SS_put_ch;
        si->post_read  = _SX_read;
        si->post_eval  = _SX_parse;
        si->post_print = _SX_print;

#ifdef NO_SHELL
        SS_set_put_line(si, SX_fprintf);
        SS_set_put_string(si, SX_fputs);
        SC_set_get_line(PG_wind_fgets);
#else
        SS_set_put_line(si, SS_printf);
        SS_set_put_string(si, SS_fputs);
        SC_set_get_line(io_gets);
#endif

        SX_gr_mode   = FALSE;
        SX_plot_flag = FALSE;

        ret = SS_t;}
    else
        ret = SS_f;

    SS_set_prompt(si, "SX-> ");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SETUP_VIEWSPACE - setup the viewspace for DEV accounting for
 *                    - label space
 */

void SX_setup_viewspace(PG_device *dev, double mh)
   {double labsp, nvh, nvoy;
    static double obx[PG_BOXSZ] = { -HUGE, -HUGE, -HUGE, -HUGE, -HUGE, -HUGE};
    static int traditional = TRUE;

/* remember the original view height in order to be able to
 * reference label-space against that value
 * otherwise label-space 0.5 followed by label-space 0.0 goes bad
 */
    if (obx[0] == -HUGE)
       PG_box_copy(3, obx, SX_view_x);

    PG_get_attrs_glb(TRUE,
		     "label-space", &labsp,
		     NULL);

/* this way respects the user controls in ULTRA and PDBView
 * such as view-height and label-space
 * it does this by preserving SX_view_x
 */
    if (traditional == TRUE)
       {nvh  = SX_view_height/(1.0 + labsp);
	nvoy = (obx[2] + labsp)/(1.0 + labsp);

	SX_view_x[2] = nvoy;
	SX_view_x[3] = nvoy + nvh;

	dev->view_x[2] = nvoy;
	dev->view_x[3] = nvoy + nvh;

/* set the old school state */
/*        SX_view_height    = nvh; */
	SX_window_height *= mh;}

/* this way make better use of space as the window is
 * made larger or smaller
 * it does this by modifying SX_view_x
 */
    else
       {nvh  = (obx[3] - obx[2])/(1.0 + labsp);
	nvoy = (obx[2] + labsp)/(1.0 + labsp);

	SX_view_x[2] = nvoy;
	SX_view_x[3] = nvoy + nvh;

	dev->view_x[2] = nvoy;
	dev->view_x[3] = nvoy + nvh;

/* set the old school state */
	SX_view_height    = nvh;
	SX_window_height *= mh;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MODE_GRAPHICS - go into graphics mode
 *                  - return "#t" if graphics mode was already in effect
 *                  - else return "#f"
 */

object *SX_mode_graphics(SS_psides *si)
   {object *ret;

    if (PG_console_device == NULL)
       {if (!PG_open_console("PDBView", SX_console_type,
                             SX_background_color_white,
                             SX_console_x, SX_console_y,
                             SX_console_width, SX_console_height))
           {if (!SX_qflag)
               PRINT(STDOUT, "\nCannot connect to display\n");};}

    if (SX_graphics_device == NULL)
       {SS_set_prompt(si, "\n-> ");
        strcpy(si->ans_prompt, "");

        si->post_read  = _SX_read;
        si->post_eval  = _SX_parse;
        si->post_print = _SX_print;
	si->pr_gets    = _SX_get_input;

	SS_set_put_line(si, SX_fprintf);
	SS_set_put_string(si, SX_fputs);
	if (PG_console_device == NULL)
	   SC_set_get_line(io_gets);
	else
	   SC_set_get_line(PG_wind_fgets);

        SX_gr_mode         = TRUE;
        SX_graphics_device = PG_make_device(SX_display_name, SX_display_type,
                                            SX_display_title);

/* map the PDBView graphics state onto the device */
	if (SX_graphics_device != NULL)
	   {SX_set_graphics_state(SX_graphics_device);

	    SX_setup_viewspace(SX_graphics_device, 1.0);

	    PG_make_device_current(SX_graphics_device);
	    PG_set_viewspace(SX_graphics_device, 2, WORLDC, NULL);
	    PG_release_current_device(SX_graphics_device);

	    PG_set_default_event_handler(SX_graphics_device,
					 SX_default_event_handler);

	    PG_set_motion_event_handler(SX_graphics_device,
					SX_motion_event_handler);

	    PG_set_mouse_down_event_handler(SX_graphics_device,
					    SX_mouse_event_handler);

	    PG_set_mouse_up_event_handler(SX_graphics_device,
					  SX_mouse_event_handler);

	    PG_set_expose_event_handler(SX_graphics_device,
					SX_expose_event_handler);

	    PG_set_update_event_handler(SX_graphics_device,
					SX_update_event_handler);

	    SC_REGISTER_CONTEXT(SX_default_event_handler, si);
	    SC_REGISTER_CONTEXT(SX_motion_event_handler,  si);
	    SC_REGISTER_CONTEXT(SX_mouse_event_handler,   si);
	    SC_REGISTER_CONTEXT(SX_expose_event_handler,  si);
	    SC_REGISTER_CONTEXT(SX_update_event_handler,  si);

/* remember the window size and position in pixels */
	    SX_window_height_P = PG_window_height(SX_graphics_device);
	    SX_window_width_P  = PG_window_width(SX_graphics_device);
	    SX_window_P[0]     = SX_graphics_device->g.hwin[0];
	    SX_window_P[1]     = SX_graphics_device->g.hwin[2];
	    
	    if (PG_console_device != NULL)
	       PG_expose_device(PG_console_device);};

        ret = SS_t;}
    else
       ret = SS_f;

    PG_make_device_current(PG_console_device);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/