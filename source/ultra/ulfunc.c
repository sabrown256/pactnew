/*
 * ULFUNC.C - basic functions for Ultra
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "ultra.h"

#define UL_COPY_CURVE(x)     UL_copy_curve(SX_get_crv_index_i(x));
#define advance(j)           (j < (SX_dataset[l].n-1)) ? j++ : j 

#ifndef TOO_MANY_POINTS
#define TOO_MANY_POINTS 1000000
#endif

char
 macro_line[MAXLINE];

/*--------------------------------------------------------------------------*/

/*                          RUDAMENTARY FUNCTIONS                           */

/*--------------------------------------------------------------------------*/

/* UL_SELECT - get the designated curve from the menu if it exists */

static object *UL_select(object *s)
   {int i, j;
    object *ret;

    j = -1;
    SS_args(s,
            SC_INT_I, &j,
            0);

    if ((j < 1) || (j > SX_n_curves_read))
       return(SS_t);

    i = SX_number[j];

/* this indicates that there is no curve */
    if (i == -1)
       return(SS_t);

    if (SX_dataset[i].n < 2)
       SS_error("CURVE HAS < 2 POINTS - UL_SELECT", s);

/* make a copy of the curve and read the data into the copy */
    ret = UL_copy_curve(i);
    i   = SX_get_crv_index_i(ret);

/* fetch the curve data out of the cache wherever and however it is done */
    SX_uncache_curve(&SX_dataset[i]);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_PRINT_LABEL - print one curve label out to the terminal
 *                 - see print_curve_labels
 */

static void _UL_print_label(int i, int j, int md, char *s, int id_flag,
			    FILE *fp, char f[], int id)
   {int k, sqzlab, ndi, labln;
    char label[MAXLINE];

    PG_get_attrs_glb(TRUE,
		     "label-length", &labln,
		     "numeric-data-id", &ndi,
		     "squeeze-labels", &sqzlab,
		     NULL);

/* prep the label text */
    memset(label, '\0', MAXLINE);
    if (sqzlab == TRUE)
       SC_squeeze_blanks(s);

    SC_strncpy(label, MAXLINE, s, labln);
    for (k = 0; k < labln; k++)
        {if (label[k] == '\0')
	    label[k] = ' ';};

    switch (id_flag)
       {case 1 :
	     PRINT(fp, "%4d", j);
	     break;
	case 2 :
	     if (md)
                if ((ndi) ||
                    (id > 'Z'))
	           PRINT(fp, "* @%d", id - 'A' + 1);
                else
                   PRINT(fp, "* %c", id);
	     else
                if ((ndi) ||
                    (id > 'Z'))
	           PRINT(fp, "  @%d", id - 'A' + 1);
                else
                   PRINT(fp, "  %c", id);
	     break;
	case 3 :
	     if (md)
                if ((ndi) ||
                    (id > 'Z'))
	           PRINT(fp, "%4d * @%d", j, id - 'A' + 1);
                else
	           PRINT(fp, "%4d * %c", j, id);
	     else
                if ((ndi) ||
                    (id > 'Z'))
	           PRINT(fp, "%4d   @%d", j, id - 'A' + 1);
                else
	           PRINT(fp, "%4d   %c", j, id);
	default :
	     break;};

    PRINT(fp, " %s", label);
    if (labln < 40)
       PRINT(fp, " %10.2e %10.2e %10.2e %10.2e %s",
	     SX_dataset[i].wc[0], SX_dataset[i].wc[1],
	     SX_dataset[i].wc[2], SX_dataset[i].wc[3], f);

    PRINT(fp, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_MENUI - display the menu of the selected curves */

static object *_ULI_menui(object *s)
   {int i, j, md, id, id_flag;
    char *ss, f[MAXLINE];
    FILE *fp;

    j = -1;
    SS_args(s,
            SC_INT_I, &j,
            0);

    if ((0 < j) && (j <= SX_n_curves_read))
       {i = SX_number[j];

/* this indicates that there is no curve */
	if (i != -1)
	   {id_flag = 1;
	    fp = stdout;
	    f[0] = '\0';

	    if (SX_dataset[i].file != NULL)
	       strcpy(f, SX_dataset[i].file);

	    id = SX_dataset[i].id;
	    md = SX_dataset[i].modified;
	    ss = SX_dataset[i].text;

	    _UL_print_label(i, j, md, ss, id_flag, fp, f, id);};};

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_COMPRESS_NUMBERS - after killing curves compress the numbers to be
 *                     - in contiguous sequence from 1
 */

static object *UL_compress_numbers(void)
   {int i, lasti;

    lasti = -1;
    for (i = 1; i < SX_N_Curves; i++)
        {if ((SX_number[i] != -1) && (lasti != -1))
            {SX_number[lasti] = SX_number[i];
             SX_number[i] = -1;
             for (; (SX_number[lasti] != -1) && (lasti < SX_N_Curves);
                  lasti++);}

         else if ((SX_number[i] == -1) && (lasti == -1))
            lasti = i;};

    _SX_next_available_number = max(1, lasti);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_EXPUNGE - delete the given curve from the menu */

static int UL_expunge(int j)
   {int i;

/* if the requested curve number is zero, kill them all */
    if (j == 0)
       {SX_close_open_files();
        for (j = 0; j < SX_N_Curves; j++)
            {i = SX_number[j];
             if (i != -1)
                {if (SX_dataset[i].x[0] != NULL)
                    {CFREE(SX_dataset[i].x[0]);};
                 if (SX_dataset[i].x[1] != NULL)
                    {CFREE(SX_dataset[i].x[1]);};

                 SX_zero_curve(i);
                 SX_number[j] = -1;};};
	SX_n_curves_read = 0;
        SX_reset_prefix();}

    else
       {i = SX_number[j];
        if (i != -1)
           {if (SX_dataset[i].x[0] != NULL)
               {CFREE(SX_dataset[i].x[0]);};
            if (SX_dataset[i].x[1] != NULL)
               {CFREE(SX_dataset[i].x[1]);};

            SX_zero_curve(i);
            SX_number[j] = -1;
            SX_n_curves_read--;};}

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_EXPUNGE_MACRO - executes SIMPLE_EXPUNGE and COMPRESS_NUMBERS to
 *                    - give the expunge that you really want.
 */

static object *_ULI_expunge_macro(object *argl)
   {object *s, *t;
    int limit, j;

    SX_prep_arg(argl);

    limit = SX_n_curves_read;
    if (!SS_nullobjp(SS_car(argl)))
       {for (s = argl;
             SS_consp(s) && !SS_nullobjp(SS_car(s));
             s = SS_cdr(s))
	    {t = SS_car(s);
             SS_args(t,
		     SC_INT_I, &j,
		     0);
	     if ((0 <= j) && (j <= limit))
	        {if (!UL_expunge(j))
		    {UL_compress_numbers();
		     SS_error("INVALID CURVE NUMBER - _ULI_EXPUNGE_MACRO",
			      argl);};};};

        UL_compress_numbers();}

    SS_Assign(argl, SS_null);

    return(argl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_DELETE - delete the curve from the curve list (but not the menu)
 *           - and rebind the variable whose name is a lower case version
 *           - of the curve label to itself
 */

object *UL_delete(object *s)
   {int i;
    object *o;

    o = SS_null;
    if (!SS_nullobjp(s))
       {if (!SX_curvep_a(s))
	   SS_error("BAD CURVE - UL_DELETE", s);

	i = SX_curve_id(s);
	o = SX_rl_curve(i);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_ERASE - erase all the currently displayed curves */

static object *_ULI_erase(void)
   {int j;

    for (j = 0; j < SX_N_Curves; j++)
        {if (SX_data_index[j] != -1)
            SX_rl_curve(j);};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_COLOR - set the color of the specified curve */

static object *UL_color(object *obj, object *color)
   {int i;

    i = SX_get_crv_index_i(obj);

    SC_CHANGE_VALUE_ALIST(SX_dataset[i].info, int, SC_INT_P_S,
			  "LINE-COLOR", SS_INTEGER_VALUE(color));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_SCATTER - set the scatter attribute of the specified curve */

static object *UL_scatter(object *obj, object *flag)
   {int i;

    i = SX_get_crv_index_i(obj);

    SC_CHANGE_VALUE_ALIST(SX_dataset[i].info, int, SC_INT_P_S,
			  "SCATTER", SS_INTEGER_VALUE(flag));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_FILL - set the fill attribute of the specified curve */

static object *UL_fill(object *obj, object *flag)
   {int i;

    i = SX_get_crv_index_i(obj);

    SC_CHANGE_VALUE_ALIST(SX_dataset[i].info, int, SC_INT_P_S,
			  "FILL", SS_INTEGER_VALUE(flag));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_TOGGLE_LOGICAL_OP - toggle the logical op of the device */

static object *UL_toggle_logical_op(void)
   {int i;
    PG_logical_operation lop;
    PG_device *dev;
    out_device *out;

    for (i = -1; i < N_OUTPUT_DEVICES; i++)
        {if (i == -1)
	    dev = SX_graphics_device;
	 else
	    {out = SX_get_device(i);
	     if (out->active)
	        dev = out->dev;
	     else
	        dev = NULL;};

	 if (dev != NULL)
	    {PG_get_logical_op(dev, &lop);

	     lop = (lop == GS_COPY) ? GS_XOR : GS_COPY;

	     PG_set_logical_op(dev, lop);};};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_MARKER - set the marker of the specified curve */

static object *UL_marker(object *obj, object *flag)
   {int i, mrk, mi;

    i = SX_get_crv_index_i(obj);

    PG_get_attrs_glb(TRUE,
		     "marker-index", &mi,
		     NULL);

    mrk  = SS_INTEGER_VALUE(flag);
    if ((mrk < 0) || (mrk >= mi))
       SS_error("BAD MARKER VALUE - UL_MARKER", flag);

    SC_CHANGE_VALUE_ALIST(SX_dataset[i].info, int, SC_INT_P_S,
			  "MARKER-INDEX", mrk);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_HIST - set UL_plot the curve as a histogram */

static object *UL_hist(object *obj, object *flag)
   {int i, fl, side;
    pcons *info;
    PG_rendering pty;

    fl = SS_INTEGER_VALUE(flag);
    if (fl == HIST_LEFT)
       {pty  = PLOT_HISTOGRAM;
        side = 0;}

    else if (fl == HIST_RIGHT)
       {pty  = PLOT_HISTOGRAM;
        side = 1;}

    else if (fl == HIST_CENTER)
       {pty  = PLOT_HISTOGRAM;
        side = 2;}

    else
       {pty  = PLOT_CARTESIAN;
        side = 0;}

    i = SX_get_crv_index_i(obj);

    info = SX_dataset[i].info;
    info = PG_set_plot_type(info, pty, pty);

    SC_CHANGE_VALUE_ALIST(SX_dataset[i].info, int, SC_INT_P_S,
			  "HIST-START", side);

    SX_dataset[i].info = info;

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_LNWIDTH - set line width of specified curve */

static object *UL_lnwidth(object *obj, object *width)
   {int i;
    double wd;

    i = SX_get_crv_index_i(obj);

    wd = 0.0;
    SS_args(width,
	    SC_DOUBLE_I, &wd,
	    0);

    SC_CHANGE_VALUE_ALIST(SX_dataset[i].info, double, SC_DOUBLE_P_S,
			  "LINE-WIDTH", wd);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_LNSTYLE - set line style of specified curve */

static object *UL_lnstyle(object *obj, object *style)
   {int i;

    i = SX_get_crv_index_i(obj);

    SC_CHANGE_VALUE_ALIST(SX_dataset[i].info, int, SC_INT_P_S,
			  "LINE-STYLE", SS_INTEGER_VALUE(style));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_GET_SOME_DEVICE - any device will do
 *                     - just get one
 */

static PG_device *_UL_get_some_device(void)
   {int i;
    PG_device *dev;
    out_device *out;

    dev = NULL;
    if (SX_graphics_device != NULL)
       dev = SX_graphics_device;
    else
       {for (i = 0; i < N_OUTPUT_DEVICES; i++)
	    {out = SX_get_device(i);
	     if (out->exist && out->active)
	        {dev = out->dev;
		 break;};};};

    return(dev);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_RANGE - set the range of the plot */

static object *_ULI_range(object *argl)
   {double t;
    double wc[PG_BOXSZ];
    PG_device *dev;
    object *s, *rv;

    SX_prep_arg(argl);

    dev = _UL_get_some_device();

    wc[2] = HUGE;
    wc[3] = -HUGE;

    if (SS_nullobjp(argl))
       UL_plot_limits(dev, FALSE, wc);

    else
       {s = SS_car(argl);

	if (!SS_numbp(s))
	   {if (SC_str_icmp(SS_get_string(s), "de") == 0)
	       {SX_autorange = TRUE;
		UL_plot_limits(dev, FALSE, wc);}
	    else
	       SS_error("BAD ARGUMENTS - _ULI_RANGE", s);}
	else
	   {SS_args(argl,
		    SC_DOUBLE_I, &wc[2],
		    SC_DOUBLE_I, &wc[3],
		    0);
	    if (wc[2] == HUGE)
	       SS_error("BAD NUMBER LOWER LIMIT - _ULI_RANGE", argl);

	    if (wc[3] == -HUGE)
	       SS_error("BAD NUMBER UPPER LIMIT - _ULI_RANGE", argl);

	    if (wc[2] == wc[3])
	       SS_error("LOWER LIMIT EQUALS UPPER LIMIT - _ULI_RANGE", argl);

	    if (wc[3] < wc[2])
	       {t = wc[2];
		wc[2] = wc[3];
		wc[3] = t;};

	    SX_gwc[2] = wc[2];
	    SX_gwc[3] = wc[3];
	    SX_autorange = FALSE;};};

    rv = SS_mk_cons(SS_mk_float(wc[2]), SS_mk_float(wc[3]));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_DOMAIN - set the domain of the plot */

static object *_ULI_domain(object *argl)
   {double t;
    double wc[PG_BOXSZ];
    PG_device *dev;
    object *s, *rv;

    SX_prep_arg(argl);

    dev = _UL_get_some_device();

    wc[0] = HUGE;
    wc[1] = -HUGE;
    wc[2] = HUGE;
    wc[3] = -HUGE;
	    
    if (SS_nullobjp(argl))
       UL_plot_limits(dev, FALSE, wc);

    else
       {s = SS_car(argl);

	if (!SS_numbp(s))
	   {if (SC_str_icmp(SS_get_string(s), "de") == 0)
	       {SX_autodomain = TRUE;
		UL_plot_limits(dev, FALSE, wc);}
	    else
	       SS_error("BAD ARGUMENTS - _ULI_DOMAIN", s);}
	else
	   {SS_args(argl,
		    SC_DOUBLE_I, &wc[0],
		    SC_DOUBLE_I, &wc[1],
		    0);
	    if (wc[0] == HUGE)
	       SS_error("BAD NUMBER LOWER LIMIT - _ULI_DOMAIN", argl);

	    if (wc[1] == -HUGE)
	       SS_error("BAD NUMBER UPPER LIMIT - _ULI_DOMAIN", argl);

	    if (wc[0] == wc[1])
	       SS_error("LOWER LIMIT EQUALS UPPER LIMIT - _ULI_DOMAIN", argl);

	    if (wc[1] < wc[0])
	       {t = wc[0];
		wc[0] = wc[1];
		wc[1] = t;};

	    SX_gwc[0] = wc[0];
	    SX_gwc[1] = wc[1];
	    SX_autodomain = FALSE;};};

    rv = SS_mk_cons(SS_mk_float(wc[0]), SS_mk_float(wc[1]));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_OPEN_DEVICE - open the graphics window for ULTRA
 *                  - arguments:
 *                  -    DEVICE_NAME - X display name or one of the PGS names
 *                  -    DEVICE_TYPE - primarily "COLOR" or "MONOCHROME"
 *                  -    DISPLAY_TITLE - a title for identification purposes
 */

static object *_ULI_open_device(object *argl)
   {int i;
    char *name, *type, *title;
    out_device *out;

    SX_prep_arg(argl);

    name  = NULL;
    type  = NULL;
    title = NULL;
    SS_args(argl,
            SC_STRING_I, &name,
            SC_STRING_I, &type,
            SC_STRING_I, &title,
            0);

    SC_str_upper(name);
    SC_str_upper(type);

    for (i = 0; i < N_OUTPUT_DEVICES; i++)
        {out = SX_get_device(i);
	 if (out->exist && out->active)
	    {if (strcmp(name, out->dupp) == 0)
	        {CFREE(out->type);
		 out->type = type;

		 CFREE(out->fname);   
		 out->fname = title;

		 CFREE(name);
		 break;};};};

    if (i >= N_OUTPUT_DEVICES)
       {CFREE(SX_display_type);
        SX_display_type = type;

        CFREE(SX_display_title);   
        SX_display_title = title;

        CFREE(SX_display_name);
        SX_display_name = name;

        if (SX_gr_mode)
           UL_mode_graphics();};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_CLOSE_DEVICE - close the named graphics device for ULTRA */

static object *_ULI_close_device(object *arg)
   {int i;
    char *name;
    out_device *out;

    name = CSTRSAVE(SS_get_string(arg));

    SC_str_upper(name);
    for (i = 0; i < N_OUTPUT_DEVICES; i++)
        {out = SX_get_device(i);
	 if (out->exist && out->active)
	    {if (strcmp(name, out->dupp) == 0)
	        {CFREE(out->type);
		 CFREE(out->fname);   
		 if (out->dev != NULL)
		    {PG_close_device(out->dev);
		     out->dev    = NULL;
		     out->active = FALSE;
		     break;};};};};

    if ((strcmp(name, "WINDOW") == 0) || (strcmp(name, "SCREEN") == 0))
       {if (SX_graphics_device != NULL)
           {PG_close_device(SX_graphics_device);
            SX_graphics_device = NULL;};};

    CFREE(name);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_QUIT - gracefully exit from Ultra */

void _UL_quit(int sts)
   {int i;
    out_device *out;

    UL_mode_text();

/* check the need to close the command log */
    if (SX_command_log != NULL)
       io_close(SX_command_log);

/* check the need to close the ascii output file */
    if (SX_out_text != NULL)
       io_close(SX_out_text);

/* check the need to close the binary output file */
    if (SX_out_bin != NULL)
       io_close(SX_out_bin);

/* check the need to close the PDB output file */
    if (SX_out_pdb != NULL)
       PD_close(SX_out_pdb);

/* close the cache file and any open data files */
    SX_close_open_files();

/* check the need to close the output devices */
    for (i = 0; i < N_OUTPUT_DEVICES; i++)
        {out = SX_get_device(i);
	 if (out->dev != NULL)
	    {PG_close_device(out->dev);
	     out->dev = NULL;};};

    PC_exit_all();

    exit(sts);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_QUIT - gracefully exit from Ultra */

static object *_ULI_quit(object *arg)
   {int exit_val;

    exit_val = 0;
    SS_args(arg,
            SC_INT_I, &exit_val,
            0);

    _UL_quit(exit_val);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_PRINT_LABELS - print the curve labels out to the terminal
 *                 - see print_curve_labels
 */

static object *UL_print_labels(int *indx, int nc,
			       char *regx, char *file, int id_flag,
			       char *name, int silent)
   {int i, j, np, id, md, nmore, nlp;
    char bf[10];
    char f[MAXLINE], *s;
    FILE *fp;
    object *ret;

    ret = SS_null;

    if (name != NULL)
       {fp = io_open(name, "w");
        if (fp == NULL)
           SS_error("CANNOT OPEN FILE - UL_PRINT_LABELS", SS_null);
        PRINT(fp, "\n\n");}
    else
       fp = stdout;

    if (_SS_si.lines_page == 0)
       nlp = INT_MAX;
    else
       nlp = max(26, _SS_si.lines_page);

    np    = 0;
    nmore = 0;
    for (j = 0; j < nc; j++)
        {i = indx[j];
         if (i >= 0)
            {id = SX_dataset[i].id;
             md = SX_dataset[i].modified;
             s  = SX_dataset[i].text;
             if (SX_dataset[i].file != NULL)
                strcpy(f, SX_dataset[i].file);
             else
                f[0] = '\0';
             if (regx != NULL)
                {if (!SC_regx_match(s, regx))
                    continue;};
             if (file != NULL)
                {if (!SC_regx_match(f, file))
                    continue;};

             if (id_flag == 1)
                {SS_Assign(ret, SS_mk_cons(SS_mk_integer(j), ret));}
             else if (id_flag == 2)
                {SS_Assign(ret, SS_mk_cons(SX_dataset[i].obj, ret));}

             if ((silent == FALSE) &&
		 ((_SS_si.interactive == ON) || (fp != stdout)))

/* prep the label text */
	        {_UL_print_label(i, j, md, s, id_flag, fp, f, id);
		 if (fp == stdout)
		    {nmore++;
		     if (nmore >= nlp)
		        {PRINT(fp, "More ... (n to stop)");
			 GETLN(bf, 10, stdin);
			 if (bf[0] == 'n')
			    break;
			 nmore = 0;};};

		 if (name != NULL)
		    {np++;
		     if (np >= nlp)
		        {PRINT(fp, "\014\n\n");
			 np = 0;};};};};};

    if (name != NULL)
       io_close(fp);

    SS_Assign(ret, SS_reverse(ret));

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_MENU - display the menu of the available curves */

static object *_ULI_menu(object *argl)
   {char *pr, *pf, *pn;
    object *ret;

    if (!SS_nullobjp(argl))
       {SX_prep_arg(argl);};

    pr = NULL;
    pf = NULL;
    pn = NULL;
    SS_args(argl,
            SC_STRING_I, &pr,
            SC_STRING_I, &pf,
            SC_STRING_I, &pn,
            0);

    ret = UL_print_labels(SX_number, SX_N_Curves,
			  pr, pf, 1, pn, FALSE);
    SX_plot_flag = FALSE;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_PREFIX - set or display menu prefixes */

static object *_ULI_prefix(object *argl)
   {object *arg1;
    object *arg2;
    object *ret;
    char pre, prefix[MAXLINE];
    int  mindex, i;
    char *fname, *s;

    ret    = SS_null;
    fname  = "";
    mindex = 0;

    if (SS_consp(argl))
       {arg1  = SS_car(argl);
        argl = SS_cdr(argl);
        strcpy(prefix, SS_get_string(arg1));
        if (strlen(prefix) != 1)
           SS_error("BAD PREFIX - _ULI_PREFIX", arg1);

        pre = tolower((int) prefix[0]);
        if ((pre < 'a') || (pre > 'z'))
           SS_error("BAD PREFIX - _ULI_PREFIX", arg1);

        if (SS_consp(argl))
           {arg2 = SS_car(argl);
            s = SS_get_string(arg2);
            if (SC_intstrp(s, 10))
               mindex = atoi(s);
            else
               {if (strcmp(s, "off") != 0)
                   SS_error("BAD INDEX - _ULI_PREFIX", arg2);};

	    mindex = max(mindex, 0);
	    mindex = min(mindex, SX_n_curves_read - 1);
	    fname  = SX_dataset[SX_number[mindex]].file;
            SX_prefix_list[pre - 'a'] = mindex;}

        else
           {if ((mindex = SX_prefix_list[pre - 'a']) > 0)
               {if (mindex <= SX_n_curves_read)
                    fname = SX_dataset[SX_number[mindex]].file;
                if (_SS_si.interactive == ON)
                   PRINT(stdout, " %c%6d    %s\n", pre, mindex, fname);}
            else
               {if (_SS_si.interactive == ON)
                   PRINT(stdout, " Prefix %c is not assigned\n", pre);};}

        {SS_Assign(ret,
                   SS_mk_cons(arg1,
                              SS_mk_cons(SS_mk_integer(mindex),
                                         SS_mk_cons(SS_mk_string(fname),
                                                    ret))));};}
    else
       {for (i = 0; i < NPREFIX; i++)
            if ((mindex = SX_prefix_list[i]) > 0)
               {pre = 'a' + i;
                fname = "";
                if (mindex <= SX_n_curves_read)
                   fname = SX_dataset[SX_number[mindex]].file;
                if (_SS_si.interactive == ON)
                   PRINT(stdout, " %c%6d    %s\n", pre, mindex, fname);
                arg1 = SS_mk_char((int) pre);
                {SS_Assign(ret,
                           SS_mk_cons(
                                      SS_mk_cons(arg1,
                                                 SS_mk_cons(SS_mk_integer(mindex),
                                                            SS_mk_cons(SS_mk_string(fname),
                                                                       SS_null))),
                                      ret));};}
        {SS_Assign(ret, SS_reverse(ret));};}

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_LIST_CURVES - display the curve list */

static object *_ULI_list_curves(object *argl)
   {char *pr, *pf;
    object *ret;

    if (!SS_nullobjp(argl))
       {SX_prep_arg(argl);};

    pr = NULL;
    pf = NULL;
    SS_args(argl,
            SC_STRING_I, &pr,
            SC_STRING_I, &pf,
            0);

    ret = UL_print_labels(SX_data_index, SX_N_Curves,
			  pr, pf, 2, NULL, FALSE);
    SX_plot_flag = FALSE;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_GET_CRV_LIST - return a list of currently plotted curves */

object *UL_get_crv_list(void)
   {object *ret;

    ret = UL_print_labels(SX_data_index, SX_N_Curves,
			  NULL, NULL, 2, NULL, TRUE);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_POW - a callable version of pow (why?) */

static double _UL_pow(double x, double a)
   {double rv;

    rv = POW(a, x);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_COMPOSE - a functional composition of curves */

static double UL_compose(double a, int i)
   {int j, n;
    double val;
    double *x, *y;

    n = SX_dataset[i].n;
    x = SX_dataset[i].x[0];
    y = SX_dataset[i].x[1];
    j = PM_find_index(x, a, n);
    if (j == 0)
       {PM_interp(val, a, x[0], y[0], x[1], y[1]);}

    else if (j == n)
       {PM_interp(val, a, x[n-2], y[n-2], x[n-1], y[n-1]);}

    else
       {PM_interp(val, a, x[j-1], y[j-1], x[j], y[j]);};

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_DERIVATIVE - take the derivative of a list of curves */

static object *UL_derivative(int j)
   {int n;
    double *x[PG_SPACEDM];
    char *lbl;
    object *ch;

    n  = SX_dataset[j].n;
    x[0] = SX_dataset[j].x[0];
    x[1] = SX_dataset[j].x[1];
    UL_buf1x = CMAKE_N(double, n+5);
    UL_buf1y = CMAKE_N(double, n+5);

    PM_derivative(n, x[0], x[1], UL_buf1x, UL_buf1y);

    if ((SX_dataset[j].id >= 'A') &&
        (SX_dataset[j].id <= 'Z'))
       {lbl = SC_dsnprintf(FALSE, "Derivative %c", SX_dataset[j].id);}
    else
       {lbl = SC_dsnprintf(FALSE, "Derivative @%d", SX_dataset[j].id);}

    if (n == 2)
       {n = 3;
	UL_buf1x[0] = x[0][0];
	UL_buf1x[1] = x[0][1];
	UL_buf1y[1] = UL_buf1y[0];};

    ch = SX_mk_curve(n-1, UL_buf1x, UL_buf1y, lbl, NULL, (PFVoid) UL_plot);

    CFREE(UL_buf1x);
    CFREE(UL_buf1y);      

    return(ch);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_THIN - thin a curve */

static object *UL_thin(int j, object *argl)
   {int n, m;
    double *x[PG_SPACEDM];
    double toler;
    char *type, *lbl;
    object *ch;

    type  = NULL;
    toler = 0.02;
    SS_args(argl,
            SC_STRING_I, &type,
            SC_DOUBLE_I, &toler,
            0);

    n  = SX_dataset[j].n;
    x[0] = SX_dataset[j].x[0];
    x[1] = SX_dataset[j].x[1];
    UL_buf1x = CMAKE_N(double, n);
    UL_buf1y = CMAKE_N(double, n);
    toler = (toler < n) ? toler : n;

    if (strncmp(type, "int", 3) == 0)
       m = PM_thin_1d_int(n, x[0], x[1], UL_buf1x, UL_buf1y, toler);
    else
       m = PM_thin_1d_der(n, x[0], x[1], UL_buf1x, UL_buf1y, toler);

    if (m < 1)
       SS_error("THIN FAILED - UL_THIN", argl);

    if ((SX_dataset[j].id >= 'A') &&
        (SX_dataset[j].id <= 'Z'))
        {lbl = SC_dsnprintf(FALSE, "Thinned %c", SX_dataset[j].id);}
    else
        {lbl = SC_dsnprintf(FALSE, "Thinned @%d", SX_dataset[j].id);}

    ch = SX_mk_curve(m, UL_buf1x, UL_buf1y, lbl, NULL, (PFVoid) UL_plot);

    CFREE(UL_buf1x);
    CFREE(UL_buf1y);      

    return(ch);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_FILTER - filter out points from curve that fail domain or range test */

static object *UL_filter(int j, object *argl)
   {int i, k, n;
    double xt;
    double wc[PG_BOXSZ];
    double *x[PG_SPACEDM];
    object *dom_pred, *ran_pred, *xexpr, *yexpr, *o;

    dom_pred = SS_null;
    ran_pred = SS_null;

    SS_args(argl,
            SS_OBJECT_I, &dom_pred,
            SS_OBJECT_I, &ran_pred,
            0);

    if (SS_nullobjp(dom_pred))
       SS_error("BAD DOMAIN PREDICATE ARGUMENT - UL_FILTER", argl);
    if (SS_nullobjp(ran_pred))
       SS_error("BAD RANGE PREDICATE ARGUMENT - UL_FILTER", argl);

    n  = SX_dataset[j].n;
    x[0] = SX_dataset[j].x[0];
    x[1] = SX_dataset[j].x[1];
    UL_buf1x = CMAKE_N(double, n);
    UL_buf1y = CMAKE_N(double, n);

    xexpr = SS_null;
    yexpr = SS_null;
    for (i = 0, k = 0; i < n; i++)
        {xt = x[0][i];
	 SS_Assign(xexpr, SS_make_list(SS_OBJECT_I, dom_pred,
				       SC_DOUBLE_I, &xt,
				       0));
	 xt = x[1][i];
         SS_Assign(yexpr, SS_make_list(SS_OBJECT_I, ran_pred,
				       SC_DOUBLE_I, &xt,
				       0));
         if ((SS_true(SS_exp_eval(xexpr))) && (SS_true(SS_exp_eval(yexpr))))
            {UL_buf1x[k] = x[0][i];
             UL_buf1y[k] = x[1][i];
             k++;};};
    SS_Assign(xexpr, SS_null);
    SS_Assign(yexpr, SS_null);

    if (k < 2)
       {CFREE(UL_buf1x);
        CFREE(UL_buf1y);
        SS_error("FEWER THAN TWO POINTS REMAIN - UL_FILTER", SS_null);};

    SX_dataset[j].n  = k;
    SX_dataset[j].x[0] = UL_buf1x;
    SX_dataset[j].x[1] = UL_buf1y;
    PM_maxmin(SX_dataset[j].x[0], &wc[0], &wc[1], k);
    PM_maxmin(SX_dataset[j].x[1], &wc[2], &wc[3], k);
    SX_dataset[j].wc[0] = wc[0];
    SX_dataset[j].wc[1] = wc[1];
    SX_dataset[j].wc[2] = wc[2];
    SX_dataset[j].wc[3] = wc[3];
        
    CFREE(x[0]);
    CFREE(x[1]);

    o = (object *) SX_dataset[j].obj;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_INTEGRATE - integrate a curve between the specified limits */

static object *UL_integrate(int j, double d1, double d2)
   {int n;
    double *x[PG_SPACEDM];
    char *lbl;
    object *ch;

    n = SX_dataset[j].n;

    UL_buf1x = CMAKE_N(double, n+5);
    UL_buf1y = CMAKE_N(double, n+5);

    x[0] = SX_dataset[j].x[0];
    x[1] = SX_dataset[j].x[1];

/* take care of some bad cases */
    if ((SX_dataset[j].wc[0] >= d2) || (SX_dataset[j].wc[1] <= d1))
       SS_error("XMIN GREATER THAN XMAX - UL_INTEGRATE", SS_null);

    PM_integrate_tzr(d1, d2, &n, x[0], x[1], UL_buf1x, UL_buf1y);

    if ((SX_dataset[j].id >= 'A') &&
        (SX_dataset[j].id <= 'Z'))
        {lbl = SC_dsnprintf(FALSE, "Integrate %c", SX_dataset[j].id);}
    else
        {lbl = SC_dsnprintf(FALSE, "Integrate @%d", SX_dataset[j].id);}

    ch = SX_mk_curve(n, UL_buf1x, UL_buf1y, lbl, NULL, (PFVoid) UL_plot);

    CFREE(UL_buf1x);
    CFREE(UL_buf1y);
        
    return(ch);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_LABEL - change the label of the given curve */

static object *_ULI_label(object *argl)
   {int j;
    char *labl;
    object *o;

    SX_prep_arg(argl);

    j    = -1;
    labl = NULL;
    SS_args(argl,
            UL_CURVE_INDEX_I, &j,
            SC_STRING_I, &labl,
            0);

    if (j < 0)
       SS_error("BAD CURVE ARGUMENT - _ULI_LABEL", argl);

    if (labl == NULL)
       SS_error("BAD LABEL ARGUMENT - _ULI_LABEL", argl);

    CFREE(SX_dataset[j].text);

    SX_dataset[j].text     = labl;
    SX_dataset[j].modified = FALSE;

    o = SS_car(argl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_PLOT_OFF - turn off autoplotting */

static void UL_plot_off(void)
   {

    _SX.old_autoplot = SX_autoplot;
    SX_autoplot  = OFF;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_RESTORE_PLOT - turn on autoplotting */

static void UL_restore_plot(void)
   {

    SX_autoplot = _SX.old_autoplot;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_AVERAGE - return a new curve containing the average of
 *              - the curves specified
 */

static object *_ULI_average(object *s)
   {int i;
    char *t, *lbl;
    object *c, *numtoks, *rv;
    C_procedure *cpp, *cpd;

    SX_prep_arg(s);
    UL_plot_off();

    rv      = SS_null;
    numtoks = SS_mk_integer(SS_length(s));

    cpp = _SS_mk_C_proc_va(UL_bc, 1, PM_fplus);
    c   = UL_bc(cpp, s);
    if (SS_true(c))
       {cpd = _SS_mk_C_proc_va(UL_opyc, 1, PM_fdivide);
        c   = UL_opyc(cpd,
		      SS_make_list(SS_OBJECT_I, c,
				   SS_OBJECT_I, numtoks,
				   0));
        i = SX_get_crv_index_i(c);

        SC_strtok(SX_dataset[i].text, " ", t);
        lbl = SC_dsnprintf(FALSE, "Append %s", SC_strtok(NULL, "\n", t));
        CFREE(SX_dataset[i].text);

        SX_dataset[i].text = CSTRSAVE(lbl);
        UL_restore_plot();

	_SS_rl_C_proc(cpd);

        rv = c;};

    _SS_rl_C_proc(cpp);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_SYSTEM - exec a process */

static object *_ULI_system(object *s)
   {char local[MAXLINE];
   
    strcpy(local, SS_get_string(s));
    SYSTEM(local);

    UL_pause(FALSE);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_SYSCMND - exec and process and return its output in a list */

static object *_ULI_syscmnd(object *s)
   {char local[MAXLINE];
    int i;
    char **output = NULL;
    object *lst;
   
    strcpy(local, SS_get_string(s));
    output = SC_syscmnd(local);
    lst = SS_null;

    if (output != NULL)
       {i = 0;
        while (output[i] != NULL)
           {SS_Assign(lst, SS_mk_cons(SS_mk_string(output[i]), lst));
            CFREE(output[i]);
            i++;}
        SS_Assign(lst, SS_reverse(lst));
        CFREE(output);}

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_XMM - extract a portion of the given curve */

static object *UL_xmm(int j, double d1, double d2)
   {int i, l, k, n, decreasing;
    double tempx, tempy;
    double *x[PG_SPACEDM], *xrev, *yrev;
    char *lbl;
    object *ch;

    i = 0;
    k = 0;
    decreasing = FALSE;

    UL_buf1x = CMAKE_N(double, SX_dataset[j].n+1);
    UL_buf1y = CMAKE_N(double, SX_dataset[j].n+1);
    x[0]       = SX_dataset[j].x[0];
    x[1]       = SX_dataset[j].x[1];

/* take care of dumb case */
    if ((SX_dataset[j].wc[0] >= d2) || (SX_dataset[j].wc[1] <= d1))
       SS_error("XMIN GREATER THAN XMAX - UL_XMM", SS_null);

/* check to see if x is decreasing */
    if (x[0][0] > x[0][1])
       {xrev = CMAKE_N(double, SX_dataset[j].n);
        yrev = CMAKE_N(double, SX_dataset[j].n);
        n    = SX_dataset[j].n;
        for (l = 0; l < n; l++)
            {xrev[l] = x[0][n-(l+1)];
             yrev[l] = x[1][n-(l+1)];};
        x[0]   = xrev;
        x[1]   = yrev;
        decreasing = TRUE;}

/* first point */
    if (d1 < *x[0])
       {UL_buf1x[k] = *x[0];
        UL_buf1y[k++] = *x[1];
        i++;}
    else
       {while ((x[0][i] <= d1) && (i < SX_dataset[j].n))
           ++i;
        UL_buf1x[k] = d1;
        PM_interp(UL_buf1y[k++], d1, x[0][i-1], x[1][i-1], x[0][i], x[1][i]);};

/* all the rest */
    while ((i < SX_dataset[j].n) && (x[0][i] < d2))
       {UL_buf1x[k] = x[0][i];
        UL_buf1y[k++] = x[1][i];
        i++;};

    if ((d2 <= x[0][i]) && (i < SX_dataset[j].n))
       {UL_buf1x[k] = d2;
        PM_interp(UL_buf1y[k++], d2, x[0][i-1], x[1][i-1], x[0][i], x[1][i]);};

    if ((SX_dataset[j].id >= 'A') &&
        (SX_dataset[j].id <= 'Z'))
        {lbl = SC_dsnprintf(FALSE, "Extract %c", SX_dataset[j].id);}
    else
        {lbl = SC_dsnprintf(FALSE, "Extract @%d", SX_dataset[j].id);}

/* reverse points if decreasing */
    if (decreasing)
       {for (l = 0; l < k/2; l++)
            {tempx           = UL_buf1x[l];
             tempy           = UL_buf1y[l];
             UL_buf1x[l]     = UL_buf1x[k-l-1];
             UL_buf1y[l]     = UL_buf1y[k-l-1];
             UL_buf1x[k-l-1] = tempx;
             UL_buf1y[k-l-1] = tempy;};}
             
    ch = SX_mk_curve(k, UL_buf1x, UL_buf1y, lbl, NULL, (PFVoid) UL_plot);

    CFREE(UL_buf1x);
    CFREE(UL_buf1y);      

    if (decreasing)
       {CFREE(xrev);
        CFREE(yrev);}

    return(ch);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_FILTER_COEF - the n point filter routine */

static object *UL_filter_coef(int l, object *argl)
   {int ntimes;
    C_array *arr;
    object *o;

    arr    = NULL;
    ntimes = 1;
    SS_args(argl,
            G_NUM_ARRAY, &arr,
	    SC_INT_I, &ntimes,
	    0);

    SX_filter_coeff(SX_dataset[l].x[1], SX_dataset[l].n, arr, ntimes);

    UL_lmt(SX_dataset[l].x[1], SX_dataset[l].n,
	   &SX_dataset[l].wc[2], &SX_dataset[l].wc[3]);

    o = (object *) SX_dataset[l].obj;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_LNNORM - Calculate Ln norm of input curve */

static object *_ULI_lnnorm(object *argl)
   {int j, order, n;
    double lnnorm;
    object *o;

    j     = -1;
    order = 1;
    SS_args(argl,
            UL_CURVE_INDEX_I, &j,
	    SC_INT_I, &order,
	    0);

    n = SX_dataset[j].n;
    lnnorm = PM_lnnorm(SX_dataset[j].x[1], SX_dataset[j].x[0], n, order);

    o = SS_mk_float(lnnorm);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_SMOOTH - the n point smoothing routine */

static object *UL_smooth(int l, object *argl)
   {int i, n, pts, ntimes;
    char *bf;
    object *obj;

    pts = 3;
    ntimes = 1;

    SS_args(argl,
	    SC_INT_I, &pts,
	    SC_INT_I, &ntimes,
	    0);

    n = SX_dataset[l].n;

    if (SC_str_icmp(SX_smooth_method, "fft") == 0)

/* in the future let the user specify the filter function */
       {for (i = 0; i < ntimes; i++)
            PM_smooth_fft(SX_dataset[l].x[0], SX_dataset[l].x[1],
                          n, pts, PM_smooth_filter);}

    else if (SC_str_icmp(SX_smooth_method, "averaging") == 0)
       {for (i = 0; i < ntimes; i++)
            PM_smooth_int_ave(SX_dataset[l].x[0], SX_dataset[l].x[1],
                              n, pts);}

    else
       {C_array *arr;

        obj = SS_INQUIRE_OBJECT(SX_smooth_method);
        if (obj == NULL)
           {bf = SC_dsnprintf(FALSE, "NO FILTER NAMED %s EXISTS - UL_SMOOTH",
			      SX_smooth_method);
	    SS_error(bf, SS_null);};

        SS_args(SS_lk_var_val(obj, _SS_si.env),
                G_NUM_ARRAY, &arr,
		0);

        if (arr == NULL)
           {bf = SC_dsnprintf(FALSE, "%s IS NOT A FILTER - UL_SMOOTH",
			      SX_smooth_method);
	    SS_error(bf, SS_null);};

	SX_filter_coeff(SX_dataset[l].x[1], SX_dataset[l].n, arr, ntimes);};

    UL_lmt(SX_dataset[l].x[0], n, &SX_dataset[l].wc[0], &SX_dataset[l].wc[1]);
    UL_lmt(SX_dataset[l].x[1], n, &SX_dataset[l].wc[2], &SX_dataset[l].wc[3]);

    obj = (object *) SX_dataset[l].obj;

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_REVERSE - interchange the x and y data for the given curve */

static object *UL_reverse(int j)
   {object *rv;

    SC_SWAP_VALUE(double *, SX_dataset[j].x[0], SX_dataset[j].x[1]);

/* switch limits */
    SC_SWAP_VALUE(double, SX_dataset[j].wc[0], SX_dataset[j].wc[2]);
    SC_SWAP_VALUE(double, SX_dataset[j].wc[1], SX_dataset[j].wc[3]);

    rv = (object *) SX_dataset[j].obj;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_SORT - order the points in a curve */

object *UL_sort(int k)
   {object *o;

    PM_val_sort(SX_dataset[k].n, SX_dataset[k].x[0], SX_dataset[k].x[1]);

    o = (object *) SX_dataset[k].obj;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_SMP_APPEND - simple append only concatenates curves it does
 *               - not concern itself with overlaps
 */

static object *UL_smp_append(object *a, object *b)
   {int i, j, n, na, nb, l;
    double *x[PG_SPACEDM];
    char *lbl;
    object *c;

    i = SX_get_crv_index_i(a);
    j = SX_get_crv_index_i(b);

    na = SX_dataset[i].n;
    nb = SX_dataset[j].n;
    n  = na + nb;

    UL_buf1x = CMAKE_N(double, n);
    UL_buf1y = CMAKE_N(double, n);

/* insert the first curve */
    x[0] = SX_dataset[i].x[0];
    x[1] = SX_dataset[i].x[1];
    for (l = 0, n = 0; l < na; l++, n++)
        {UL_buf1x[n] = *x[0]++;
	 UL_buf1y[n] = *x[1]++;};

/* insert the second curve */
    x[0] = SX_dataset[j].x[0];
    x[1] = SX_dataset[j].x[1];
    for (l = 0; l < nb; l++, n++)
        {UL_buf1x[n] = *x[0]++;
	 UL_buf1y[n] = *x[1]++;};

    if ((SX_dataset[i].id >= 'A') &&
        (SX_dataset[i].id <= 'Z'))
       {lbl = SC_dsnprintf(FALSE, "Append %c %c", SX_dataset[i].id, SX_dataset[j].id);}
    else
       {lbl = SC_dsnprintf(FALSE, "Append @%d @%d", SX_dataset[i].id, SX_dataset[j].id);}

    c = SX_mk_curve(n, UL_buf1x, UL_buf1y, lbl, NULL, (PFVoid) UL_plot);

    CFREE(UL_buf1x);
    CFREE(UL_buf1y);      

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_PR_APPEND - primitive append is the binary append operation
 *              - it appends two curves together and returns a
 *              - newly created curve
 */

static object *UL_pr_append(object *a, object *b)
   {int i, j, k, n, na, nb, nc, l;
    double xmn, xmx, xv, yv;
    double *x[PG_SPACEDM];
    char *lbl;
    object *c, *tmp;

    tmp = SS_make_list(SS_OBJECT_I, a,
                       SS_OBJECT_I, b,
                       0);
    SS_MARK(tmp);
    c = _ULI_average(tmp);
    SS_GC(tmp);

/* no overlap of curves */
    if (!SX_curvep_a(c))
       c = UL_COPY_CURVE(a);

    i = SX_get_crv_index_i(a);
    j = SX_get_crv_index_i(b);
    k = SX_get_crv_index_i(c);

    na  = SX_dataset[i].n;
    nb  = SX_dataset[j].n;
    nc  = SX_dataset[k].n;

    xmn = SX_dataset[k].wc[0];
    xmx = SX_dataset[k].wc[1];

    n = na + nb + nc + 10;
    UL_buf1x = CMAKE_N(double, n);
    UL_buf1y = CMAKE_N(double, n);

/* insert region of A less than averaged region */
    x[0] = SX_dataset[i].x[0];
    x[1] = SX_dataset[i].x[1];
    for (n = 0; *x[0] < xmn; n++)
        {UL_buf1x[n] = *x[0]++;
	 UL_buf1y[n] = *x[1]++;};

/* insert region of B less than averaged region */
    x[0] = SX_dataset[j].x[0];
    x[1] = SX_dataset[j].x[1];
    for (; *x[0] < xmn; n++)
        {UL_buf1x[n] = *x[0]++;
	 UL_buf1y[n] = *x[1]++;};

/* insert averaged region */
    x[0] = SX_dataset[k].x[0];
    x[1] = SX_dataset[k].x[1];
    for (l = 0; l < nc; l++, n++)
        {UL_buf1x[n] = *x[0]++;
         UL_buf1y[n] = *x[1]++;};

/* insert region of A greater than averaged region */
    x[0] = SX_dataset[i].x[0];
    x[1] = SX_dataset[i].x[1];
    for (l = 0; l < na; l++)
        {xv = *x[0]++;
         yv = *x[1]++;
	 if (xv > xmx)
            {UL_buf1x[n] = xv;
             UL_buf1y[n] = yv;
             n++;};};

/* insert region of B greater than averaged region */
    x[0] = SX_dataset[j].x[0];
    x[1] = SX_dataset[j].x[1];
    for (l = 0; l < nb; l++)
        {xv = *x[0]++;
         yv = *x[1]++;
	 if (xv > xmx)
            {UL_buf1x[n] = xv;
             UL_buf1y[n] = yv;
             n++;};};

    UL_delete(c);

    if ((SX_dataset[i].id >= 'A') &&
        (SX_dataset[i].id <= 'Z'))
       {lbl = SC_dsnprintf(FALSE, "Append %c %c", SX_dataset[i].id, SX_dataset[j].id);}
    else
       {lbl = SC_dsnprintf(FALSE, "Append @%d @%d", SX_dataset[i].id, SX_dataset[j].id);}

    c = SX_mk_curve(n, UL_buf1x, UL_buf1y, lbl, NULL, (PFVoid) UL_plot);

    CFREE(UL_buf1x);
    CFREE(UL_buf1y);      

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_APPEND - sticks curves together recursively
 *             - where they overlap, it averages otherwise takes from
 *             - either curve
 */

static object *_ULI_append(object *argl)
   {int i, id;
    char local[MAXLINE];
    char *lbl;
    object *b, *acc, *target, *tmp;

    SX_prep_arg(argl);
    SX_autoplot = OFF;

    if (SS_nullobjp(argl))
       {SX_autoplot = ON;
        return(SS_null);};

/* get out the first curve and make two copies:
 * the first for the ultimate return curve; and
 * the second so that later deletions don't kill this first curve.
 */
    acc = SS_car(argl);
    if (!SX_curvep_a(acc))
       SS_error("BAD FIRST CURVE -  _ULI_APPEND", acc);
    strcpy(local, "Append");
    target = UL_COPY_CURVE(acc);
    acc = UL_COPY_CURVE(acc);
    argl = SS_cdr(argl);

    for ( ; SS_consp(argl); argl = SS_cdr(argl))
        {b = SS_car(argl);

         id = SX_dataset[SX_get_crv_index_i(b)].id;
         if ((id >= 'A') && (id <= 'Z'))
             {lbl = SC_dsnprintf(FALSE, "%s %c", local, id);}
         else
             {lbl = SC_dsnprintf(FALSE, "%s @%d", local, id);}

         if (SX_curvep_a(b))
            {if (UL_simple_append)
	        tmp = UL_smp_append(acc, b);
             else
	        tmp = UL_pr_append(acc, b);
             UL_delete(acc);
             acc = tmp;};};

    UL_delete(target);
    target = UL_COPY_CURVE(acc);
    UL_delete(acc);

    i = SX_get_crv_index_i(target);
    CFREE(SX_dataset[i].text);
    SX_dataset[i].text = CSTRSAVE(lbl);

    SX_autoplot = ON;

    return(target);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_HIDE - suppress (hide) the plotting of the given curve */

static object *UL_hide(int j)
   {object *o;

    SC_CHANGE_VALUE_ALIST(SX_dataset[j].info, int, SC_INT_P_S,
			  "LINE-COLOR", -1);

    o = (object *) SX_dataset[j].obj;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_SHOW - undo a HIDE operation on the given curve */

static object *UL_show(int j)
   {object *o;

    SC_CHANGE_VALUE_ALIST(SX_dataset[j].info, int, SC_INT_P_S,
			  "LINE-COLOR",
			  SX_next_color(SX_graphics_device));

    o = (object *) SX_dataset[j].obj;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_MAKE_LN - given a slope, intercept, xmin, xmax, and number of points
 *             - return a new curve
 */

object *_UL_make_ln(double slope, double interc,
		    double first, double last, int n)
   {double *x[PG_SPACEDM];
    double step, xo;
    object *ch;
    int i;
   
    step = (last - first)/(n-1);
    UL_buf1x = CMAKE_N(double, n);
    UL_buf1y = CMAKE_N(double, n);
    x[0] = UL_buf1x;
    x[1] = UL_buf1y;
    for (xo = first, i = 0; i < n; i++, xo += step, x[0]++, x[1]++)
        {*x[0] = xo;
         *x[1] = slope*xo + interc;};

/* without the next two lines, the final point isn't exactly at last */
    *(x[0] - 1) = last;
    *(x[1] - 1) = slope*last + interc;

    ch = SX_mk_curve(i, UL_buf1x, UL_buf1y, "Straight line", NULL,
		     (PFVoid) UL_plot);

    CFREE(UL_buf1x);
    CFREE(UL_buf1y);      

    return(ch);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_MAKE_LN - draw a line with given slope and intercept */

static object *_ULI_make_ln(object *argl)
   {int n;
    double first, last, slope, interc;
    object *o;

    slope  = 1.0;
    interc = 0.0;
    first  = 0.0;
    last   = 1.0;
    n      = SX_default_npts;
    SS_args(argl,
            SC_DOUBLE_I, &slope,
            SC_DOUBLE_I, &interc,
            SC_DOUBLE_I, &first,
            SC_DOUBLE_I, &last,
            SC_INT_I, &n,
            0);

    o = _UL_make_ln(slope, interc, first, last, n);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_MK_CURVE - make an ULTRA curve out of two lists of numbers */

static object *_ULI_mk_curve(object *argl)
   {int n;
    char *labls;
    double *x[PG_SPACEDM];
    object *xo, *yo, *xvals, *yvals;
    object *ch;

    labls = "Curve";
    SS_args(argl,
            SS_OBJECT_I, &xvals,
            SS_OBJECT_I, &yvals,
            SC_STRING_I, &labls,
            0);

    if (!SS_consp(xvals))
       SS_error("BAD LIST OF X-VALUES - _ULI_MK_CURVE", xvals);

    if (!SS_consp(yvals))
       SS_error("BAD LIST OF Y-VALUES - _ULI_MK_CURVE", yvals);

    n = min(SS_length(xvals), SS_length(yvals));

    UL_buf1x = CMAKE_N(double, n);
    UL_buf1y = CMAKE_N(double, n);
    for (x[0] = UL_buf1x, x[1] = UL_buf1y;
         SS_consp(xvals) && SS_consp(yvals);
         xvals = SS_cdr(xvals), yvals = SS_cdr(yvals))
        {xo = SS_car(xvals);
         if (!SS_numbp(xo))
            SS_error("BAD X-VALUE - _ULI_MK_CURVE", xo);
         if (SS_integerp(xo))
            *x[0]++ = SS_INTEGER_VALUE(xo);
         else
            *x[0]++ = SS_FLOAT_VALUE(xo);

         yo = SS_car(yvals);
         if (!SS_numbp(yo))
            SS_error("BAD Y-VALUE - _ULI_MK_CURVE", yo);
         if (SS_integerp(yo))
            *x[1]++ = SS_INTEGER_VALUE(yo);
         else
            *x[1]++ = SS_FLOAT_VALUE(yo);};

    ch = SX_mk_curve(n, UL_buf1x, UL_buf1y, labls, NULL, (PFVoid) UL_plot);

    CFREE(UL_buf1x);
    CFREE(UL_buf1y);

    return(ch);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_CURVE_LIST - make a list of the list of x values and the list of 
 *                 - y values from a given ULTRA curve
 */

static object *_ULI_curve_list(object *arg)
   {int i, l, n;
    double *x[PG_SPACEDM];
    object *xvals, *yvals, *o;

    i = SX_get_crv_index_i(arg);
    if (i < 0)
       SS_error("BAD CURVE - _ULI_CURVE_LIST", arg);

    n  = SX_dataset[i].n;
    x[0] = SX_dataset[i].x[0];
    x[1] = SX_dataset[i].x[1];
    xvals = SS_null;
    yvals = SS_null;
    for (l = 0; l < n; l++)
        {xvals = SS_mk_cons(SS_mk_float(x[0][l]), xvals);
         yvals = SS_mk_cons(SS_mk_float(x[1][l]), yvals);};

    o = SS_make_list(SS_OBJECT_I, SS_reverse(xvals),
		     SS_OBJECT_I, SS_reverse(yvals),
		     0);

    return(o);}

/*--------------------------------------------------------------------------*/

/*                            INSTALL FUNCTIONS                             */

/*--------------------------------------------------------------------------*/

/* UL_INSTALL_SCHEME_FUNCS - install some Ultra functions directly in the 
 *                         - Scheme hash table
 */

void UL_install_scheme_funcs(void)
   {extern void _SX_install_pgs_primitives(void);

    SX_install_global_funcs();
    _SX_install_pgs_primitives();

    SS_install("curve->list",
               "Prodedure: Given a curve return a list of the lists of x and y values\n     Usage: curve->list <curve>",
               SS_sargs,
               _ULI_curve_list, SS_PR_PROC);

    SS_install("menu",
               "Macro: List the available curves\n     Usage: menu [<label-pattern> [<file-pattern>]]",
               SS_znargs,
               _ULI_menu, SS_UR_MACRO);

    SS_install("menu*",
               "Procedure: List the available curves\n     Usage: menu* [<label-pattern> [<file-pattern>]]",
               SS_znargs,
               _ULI_menu, SS_PR_PROC);

    SS_install("lst",
               "Macro: Display curves in list\n     Usage: lst [<label-pattern> [<file-pattern>]]",
               SS_znargs,
               _ULI_list_curves, SS_UR_MACRO);

    SS_install("lst*",
               "Procedure: Display curves in list\n     Usage: lst* [<label-pattern> [<file-pattern>]]",
               SS_znargs,
               _ULI_list_curves, SS_PR_PROC);

    SS_install("prefix",
               "Macro: List or set menu prefixes\n     Usage: prefix [a | b | ... | z [<menu-index> | off]]",
               SS_znargs,
               _ULI_prefix, SS_UR_MACRO);

    SS_install("prefix*",
               "Procedure: List or set menu prefixes\n     Usage: prefix* [a | b | ... | z [<menu-index> | off]]",
               SS_znargs,
               _ULI_prefix, SS_PR_PROC);

    SS_install("thru",
               "Procedure: Return an ex[0]anded list of curves\n     Usage: thru <first-curve> <last-curve>",
               SS_nargs,
               _ULI_thru, SS_PR_PROC);

    SS_install("lnnorm",
               "Procedure: Calculate the Ln norm of curve\n     Usage: lnnorm <curve> [order]",
               SS_nargs,
               _ULI_lnnorm, SS_PR_PROC);


    SS_install("open-device*",
               "Procedure: Open graphics display device\n     Usage: open-device* <name> <tx[1]e> <title>",
               SS_nargs,
               _ULI_open_device, SS_PR_PROC);

    SS_install("open-device",
               "Macro: Open graphics device\n     Usage: open-device <name> <tx[1]e> <title>",
               SS_nargs,
               _ULI_open_device, SS_UR_MACRO);

    SS_install("close-device*",
               "Procedure: Close graphics device\n     Usage: close-device* ps | png | cgm | mpeg | jpeg | <name>",
               SS_sargs,
               _ULI_close_device, SS_PR_PROC);

    SS_install("close-device",
               "Macro: Close graphics device\n     Usage: close-device ps | png | cgm | mpeg | jpeg | <name>",
               SS_sargs,
               _ULI_close_device, SS_UR_MACRO);

    SS_install("domain",
               "Procedure: Set the domain for plotting\n     Usage: domain <low-lim> <high-lim> or\n     Usage: domain de",
               SS_znargs,
               _ULI_domain, SS_PR_PROC);

    SS_install("range",
               "Procedure: Set the range for plotting\n     Usage: range <low-lim> <high-lim> or\n     Usage: range de",
               SS_znargs,
               _ULI_range, SS_PR_PROC);

    SS_install("label",
               "Procedure: Change the label displayed for a curve by lst command\n     Usage: label <curve> <new-label>",
               SS_nargs,
               _ULI_label, SS_PR_PROC);

    SS_install("set-id",
               "Procedure: Change the data-id of a curve\n     Usage: set-id <curve> a | b | ... | z",
               SS_nargs,
               _ULI_set_id, SS_PR_PROC);

/* these are the no argument functions */
    SS_install("end",
               "Macro: Exit ULTRA with optional exit status value\n     Usage: end [<integer>]",
               SS_znargs,
               _ULI_quit, SS_PR_PROC);

    SS_install("erase",
               "Macro: Erases all curves on the screen but leaves the limits untouched\n     Usage: erase",
               SS_zargs,
               _ULI_erase, SS_UR_MACRO);

    SS_install("extract-curve",
               "Procedure: Extracts a curve with the specified points\n     Usage: extract-curve <curve> <x-start> <x-stop> <x-step>",
               SS_nargs,
               _ULI_extract_curve, SS_PR_PROC);

    SS_install("get-data-domain",
               "Procedure: Return the union domain for all curves in the list\n     Usage: get-data-domain <curve-list>",
               SS_nargs,
               SX_get_data_domain, SS_PR_PROC);

    SS_install("get-data-range",
               "Procedure: Return the union range for all curves in the list\n     Usage: get-data-range <curve-list>",
               SS_nargs,
               SX_get_data_range, SS_PR_PROC);

    SS_install("replot",
               "Macro: Replot curves in list\n     Usage: replot",
               SS_zargs,
               UL_plot, SS_UR_MACRO);

    SS_install("text",
               "Macro: Enter text mode - no graphics are available\n     See graphics\n     Usage: text",
               SS_zargs,
               UL_mode_text, SS_UR_MACRO);

    SS_install("graphics",
               "Macro: Enter graphics mode\n     To enter text mode tx[1]e: text\n     Usage: graphics",
               SS_zargs,
               UL_mode_graphics, SS_UR_MACRO);

    SS_install("print",
               "Macro: Print the current plot.\n     Usage: print",
               SS_zargs,
               _ULI_printscr, SS_UR_MACRO);

    SS_install("re-id",
               "Macro: Reset data-ids to A, B, C, ... no skipping\n     Usage: re-id",
               SS_zargs,
               SX_re_id, SS_UR_MACRO);

/* these are the former Ultra macros */
    SS_install("kill",
               "Procedure: Delete entries from the menu.\n     Usage: kill <number-list> | all",
               SS_nargs,
               _ULI_expunge_macro, SS_PR_PROC);

    SS_install("rd",
               "Macro: Read curve data file\n     Usage: rd <file-name>",
               SS_sargs,
               SX_read_data, SS_UR_MACRO);

    SS_install("rd1",
               "Macro: Read ULTRA I curve data file\n     Usage: rd1 <file-name>",
               SS_sargs,
               SX_read_ver1, SS_UR_MACRO);

    SS_install("open",
               "Procedure: Read curve data file\n     Usage: open <file-name>",
               SS_sargs,
               SX_read_data, SS_PR_PROC);

    SS_install("open1",
               "Procedure: Read ULTRA I curve data file\n     Usage: open1 <file-name>",
               SS_sargs,
               SX_read_ver1, SS_PR_PROC);

    SS_install("wrt",
               "Procedure: Save curves to file\n     Usage: wrt [<tx[1]e>] <file-name> <curve-list>",
               SS_nargs,
               SX_write_data, SS_PR_PROC);

    SS_install("save",
               "Macro: Save curves to file\n     Usage: save [<tx[1]e>] <file-name> <curve-lst>",
               SS_nargs,
               SX_write_data, SS_UR_MACRO);

    SS_install("average",
               "Procedure: Average curves\n     Usage: average <curve-list>",
               SS_nargs,
               _ULI_average, SS_PR_PROC);

    SS_install("system",
               "Procedure: Pass command to the operating system\n     Usage: system <unix-command>",
               SS_sargs,
               _ULI_system, SS_PR_PROC);

    SS_install("syscmnd",
               "Procedure: Pass command to the operating system\n     Usage: syscmnd <unix-command>",
               SS_sargs,
               _ULI_syscmnd, SS_PR_PROC);

    SS_install("append-curves",
               "Procedure: Merge curves over union of their domains \n     Where they overlap, take the average\n     Usage: append <curve-list>",
               SS_nargs,
               _ULI_append, SS_PR_PROC);

    SS_install("describe*",
               "Procedure: Describe an Ultra function or variable\n     Usage: describe* <function-list> <variable-list>",
               SS_nargs,
               _ULI_describe, SS_PR_PROC);

    SS_install("describe",
               "Macro: Describe an Ultra function or variable\n     Usage: describe <function-list> <variable-list>",
               SS_nargs,
               _ULI_describe, SS_UR_MACRO);

    SS_install("apropos",
               "Macro: List all functions or variables containing the designated substring\n     Usage: apropos <string>",
               SS_sargs,
               _ULI_apropos, SS_UR_MACRO);

    SS_install("make-curve*",
               "Procedure: Make an Ultra curve from two lists of numbers\n     Usage: make-curve* '(<list of x-values>) '(<list of y-values>)",
               SS_nargs,
               _ULI_mk_curve, SS_PR_PROC);

    SS_install("make-curve",
               "Macro: Make an Ultra curve from two lists of numbers\n     Usage: make-curve (<list of x-values>) (<list of y-values>)",
               SS_nargs,
               _ULI_mk_curve, SS_UR_MACRO);

    SS_install("make-filter",
               "Procedure: make an array of filter coefficients\n     Usage: make-filter <val1> ...",
               SS_nargs,
               SX_list_array, SS_PR_PROC);

    SS_install("line",
               "Procedure: Generate a curve with y = mx + b\n     Usage: line <m> <b> <low-lim> <high-lim> [<no.-pts>]",
               SS_nargs,
               _ULI_make_ln, SS_PR_PROC);

    SS_install("read-table*",
               "Procedure: Read nth table that starts at or after specified line in ASCII file\n     Usage: read-table* <file> [<n> [<label-line> [<line>]]]",
               SS_nargs,
               SX_read_text_table, SS_PR_PROC);

    SS_install("read-table",
               "Macro: Read nth table that starts at or after specified line in ASCII file\n     Usage: read-table <file> [<n> [<label-line> [<line>]]]",
               SS_nargs,
               SX_read_text_table, SS_UR_MACRO);

    SS_install("table-curve",
               "Procedure: Extract curve from current table\n     Usage: table-curve <num-points> [<y-off> [<y-stride> [<x-off> [<x-stride>]]]]",
               SS_nargs,
               SX_table_curve, SS_PR_PROC);

    SS_install("table-attributes",
               "Procedure: Return the attributes of the current table\n     Usage: table-attributes",
               SS_zargs,
               SX_table_attr, SS_PR_PROC);

    SS_install("toggle-logical-op",
               "Procedure: Switch the drawing state between XOR and COPY\n     Usage: toggle-logical-op",
               SS_zargs,
               UL_toggle_logical_op, SS_PR_PROC);

    SS_install("file-info",
               "Macro: Print the descriptive information for an ULTRA file\n     Usage: file-info <name>",
               SS_sargs,
               SX_crv_file_info, SS_UR_MACRO);

    SS_install("file-info*",
               "Procedure: Print the descriptive information for an ULTRA file\n     Usage: file-info* <name>",
               SS_sargs,
               SX_crv_file_info, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INSTALL_FUNCS - install Ultra functions in the Ultra hash table */

void UL_install_funcs(void)
   {

    UL_install_aux_funcs();

/* US handled functions */

    SS_install_cf("select",
                  "Procedure: Select curves from the menu for plotting\n     Usage: select <list-of-menu-numbers>",
                  UL_us, 
                  UL_select);
    SS_install_cf("menui",
                  "Procedure: List selected curves\n     Usage: menui <list-of-menu-numbers>",
                  UL_us, 
                  _ULI_menui);
    SS_install_cf("del",
                  "Procedure: Delete curves from list\n     Usage: del <curve-list>",
                  UL_us, 
                  UL_delete);

/* OPXC, OPYC handled functions */


    SS_install_cf("beta",
                  "Procedure: Take the beta function of y values of curves\n     Usage: beta <curve-list> <k>",
                  UL_opyc, 
                  PM_beta);
    SS_install_cf("betax",
                  "Procedure: Take the beta function of x values of curves\n     Usage: betax <curve-list> <k>",
                  UL_opxc, 
                  PM_beta);

    SS_install_cf("cei3",
		  "Procedure: Take the complete elliptic integral of the third kind of y values of curves\n     Usage: cei3 <curve-list> <k>",
                  UL_opyc, 
		  PM_elliptic_integral_c3);
    SS_install_cf("cei3x",
		  "Procedure: Take the complete elliptic integral of the third kind of x values of curves\n     Usage: cei3x <curve-list> <k>",
                  UL_opxc, 
		  PM_elliptic_integral_c3);

    SS_install_cf("cn",
		  "Procedure: Take the Jacobian elliptic function cn of y values of curves\n     Usage: cn <curve-list> <k>",
                  UL_opyc, 
		  PM_cn);
    SS_install_cf("cnx",
		  "Procedure: Take the Jacobian elliptic function cn of x values of curves\n     Usage: cnx <curve-list> <k>",
                  UL_opxc, 
		  PM_cn);
    SS_install_cf("dn",
		  "Procedure: Take the Jacobian elliptic function dn of y values of curves\n     Usage: dn <curve-list> <k>",
                  UL_opyc, 
		  PM_dn);
    SS_install_cf("dnx",
		  "Procedure: Take the Jacobian elliptic function dn of x values of curves\n     Usage: dnx <curve-list> <k>",
                  UL_opxc, 
		  PM_dn);
    SS_install_cf("dx",
                  "Procedure: Shift x values of curves by a constant\n     Usage: dx <curve-list> <value>",
                  UL_opxc,
                  PM_fplus);
    SS_install_cf("dy",
                  "Procedure: Shift y values of curves by a constant\n     Usage: dy <curve-list> <value>",
                  UL_opyc, 
                  PM_fplus);

    SS_install_cf("igamma-p",
		  "Procedure: Take the incomplete gamma function P of y values of curves\n     Usage: igamma-p <curve-list> <k>",
                  UL_opyc, 
		  PM_igamma_p);
    SS_install_cf("igamma-px",
		  "Procedure: Take the incomplete gamma function P of x values of curves\n     Usage: igamma-px <curve-list> <k>",
                  UL_opxc, 
		  PM_igamma_p);

    SS_install_cf("igamma-q",
		  "Procedure: Take the incomplete gamma function P of y values of curves\n     Usage: igamma-p <curve-list> <k>",
                  UL_opyc, 
		  PM_igamma_q);
    SS_install_cf("igamma-qx",
		  "Procedure: Take the incomplete gamma function P of x values of curves\n     Usage: igamma-px <curve-list> <k>",
                  UL_opxc, 
		  PM_igamma_q);

    SS_install_cf("mx",
                  "Procedure: Scale x values of curves by a constant\n     Usage: mx <curve-list> <value>",
                  UL_opxc, 
                  PM_ftimes);
    SS_install_cf("my",
                  "Procedure: Scale y values of curves by a constant\n     Usage: my <curve-list> <value>",
                  UL_opyc, 
                  PM_ftimes);
    SS_install_cf("divx",
                  "Procedure: Divide x values of curves by a constant\n     Usage: divx <curve-list> <value>",
                  UL_opxc, 
                  PM_fdivide);
    SS_install_cf("divy",
                  "Procedure: Divide y values of curves by a constant\n     Usage: divy <curve-list> <value>",
                  UL_opyc, 
                  PM_fdivide);
    SS_install_cf("in",
                  "Procedure: Take nth order modified Bessel function of the first kind of y values of curves\n     Usage: in <curve-list> <n>",
                  UL_opyc, 
                  PM_in);
    SS_install_cf("inx",
                  "Procedure: Take nth order modified Bessel function of the first kind of x values of curves\n     Usage: inx <curve-list> <n>",
                  UL_opxc, 
                  PM_in);
    SS_install_cf("jn",
                  "Procedure: Take nth order Bessel function of the first kind of y values of curves\n     Usage: jn <curve-list> <n>",
                  UL_opyc, 
                  PM_jn);
    SS_install_cf("jnx",
                  "Procedure: Take nth order Bessel function of the first kind of x values of curves\n     Usage: jnx <curve-list> <n>",
                  UL_opxc, 
                  PM_jn);
    SS_install_cf("kn",
                  "Procedure: Take nth order modified Bessel function of the second kind of y values of curves\n     Usage: kn <curve-list> <n>",
                  UL_opyc, 
                  PM_kn);
    SS_install_cf("knx",
                  "Procedure: Take nth order modified Bessel function of the second kind of x values of curves\n     Usage: knx <curve-list> <n>",
                  UL_opxc, 
                  PM_kn);
    SS_install_cf("lei1",
		  "Procedure: Take the Legendre elliptic integral of the first kind of y values of curves\n     Usage: lei1 <curve-list> <k>",
                  UL_opyc, 
		  PM_elliptic_integral_l1);
    SS_install_cf("lei1x",
		  "Procedure: Take the Legendre elliptic integral of the first kind of x values of curves\n     Usage: lei1x <curve-list> <k>",
                  UL_opxc, 
		  PM_elliptic_integral_l1);
    SS_install_cf("lei2",
		  "Procedure: Take the Legendre elliptic integral of the second kind of y values of curves\n     Usage: lei2 <curve-list> <k>",
                  UL_opyc, 
		  PM_elliptic_integral_l2);
    SS_install_cf("lei2x",
		  "Procedure: Take the Legendre elliptic integral of the second kind of x values of curves\n     Usage: lei2x <curve-list> <k>",
                  UL_opxc, 
		  PM_elliptic_integral_l2);

    SS_install_cf("powr",
                  "Procedure: Raise y values of curves to a power, y=y^a\n     Usage: powr <curve-list> <a>",
                  UL_opyc, 
                  POW);
    SS_install_cf("powrx",
                  "Procedure: Raise x values of curves to a power, x=x^a\n     Usage: powrx <curve-list> <a>",
                  UL_opxc, 
                  POW);
    SS_install_cf("powa",
                  "Procedure: Raise a to the power of the y values of curves, y=a^y\n     Usage: powa <curve-list> <a>",
                  UL_opyc, 
                  _UL_pow);
    SS_install_cf("powax",
                  "Procedure: Raise a to the power of the x values of curves, x=a^x\n     Usage: powax <curve-list> <a>",
                  UL_opxc, 
                  _UL_pow);
    SS_install_cf("sn",
		  "Procedure: Take the Jacobian elliptic function sn of y values of curves\n     Usage: sn <curve-list> <k>",
                  UL_opyc, 
		  PM_sn);
    SS_install_cf("snx",
		  "Procedure: Take the Jacobian elliptic function sn of x values of curves\n     Usage: snx <curve-list> <k>",
                  UL_opxc, 
		  PM_sn);
    SS_install_cf("tchn",
                  "Procedure: Take nth order Tchebyshev function of y values of curves\n     Usage: in <curve-list> <n>",
                  UL_opyc, 
                  PM_tchn);
    SS_install_cf("tchnx",
                  "Procedure: Take nth order Tchebyshev function of x values of curves\n     Usage: in <curve-list> <n>",
                  UL_opxc,
                  PM_tchn);
    SS_install_cf("yn",
                  "Procedure: Take nth order Bessel function of the second kind of y values of curves\n     Usage: yn <curve-list> <n>",
                  UL_opyc, 
                  PM_yn);
    SS_install_cf("ynx",
                  "Procedure: Take nth order Bessel function of the second kind of x values of curves\n     Usage: yn <curve-list> <n>",
                  UL_opxc, 
                  PM_yn);

/* UOPXC, UOPYC handled functions */

    SS_install_cf("cei1",
		  "Procedure: Take the complete elliptic integral of the first kind of y values of curves\n     Usage: cei1 <curve-list>",
                  UL_uopyc, 
		  PM_elliptic_integral_c1);
    SS_install_cf("cei1x",
		  "Procedure: Take the complete elliptic integral of the first kind of x values of curves\n     Usage: cei1x <curve-list>",
                  UL_uopxc, 
		  PM_elliptic_integral_c1);

    SS_install_cf("cei2",
		  "Procedure: Take the complete elliptic integral of the second kind of y values of curves\n     Usage: cei2 <curve-list>",
                  UL_uopyc, 
		  PM_elliptic_integral_c2);
    SS_install_cf("cei2x",
		  "Procedure: Take the complete elliptic integral of the second kind of x values of curves\n     Usage: cei2x <curve-list>",
                  UL_uopxc, 
		  PM_elliptic_integral_c2);

    SS_install_cf("erf",
                  "Procedure: Take the error function of y values of curves\n     Usage: erf <curve-list>",
                  UL_uopyc, 
                  PM_erf);
    SS_install_cf("erfx",
                  "Procedure: Take the error function of x values of curves\n     Usage: erfx <curve-list>",
                  UL_uopxc, 
                  PM_erf);

    SS_install_cf("erfc",
                  "Procedure: Take the complementary error function of y values of curves\n     Usage: erfc <curve-list>",
                  UL_uopyc, 
                  PM_erfc);
    SS_install_cf("erfcx",
                  "Procedure: Take the complementary error function of x values of curves\n     Usage: erfcx <curve-list>",
                  UL_uopxc, 
                  PM_erfc);

    SS_install_cf("ln",
                  "Procedure: Take natural log of y values of curves\n     Usage: ln <curve-list>",
                  UL_uopyc, 
                  PM_ln);
    SS_install_cf("lnx",
                  "Procedure: Take natural log of x values of curves\n     Usage: lnx <curve-list>",
                  UL_uopxc, 
                  PM_ln);

    SS_install_cf("lngamma",
                  "Procedure: Take log of the gamma function of y values of curves\n     Usage: lngamma <curve-list>",
                  UL_uopyc, 
                  PM_ln_gamma);
    SS_install_cf("lngammax",
                  "Procedure: Take log of the gamma function of x values of curves\n     Usage: lngammax <curve-list>",
                  UL_uopxc, 
                  PM_ln_gamma);

    SS_install_cf("log10",
                  "Procedure: Take base 10 log of y values of curves\n     Usage: log10 <curve-list>",
                  UL_uopyc, 
                  PM_log);
    SS_install_cf("log10x",
                  "Procedure: Take base 10 log of x values of curves\n     Usage: log10x <curve-list>",
                  UL_uopxc, 
                  PM_log);
    SS_install_cf("exp",
                  "Procedure: Take exponential of y values of curves, y=e^y\n     Usage: exp <curve-list>",
                  UL_uopyc, 
                  exp);
    SS_install_cf("expx",
                  "Procedure: Take exponential of x values of curves, x=e^x\n     Usage: expx <curve-list>",
                  UL_uopxc, 
                  exp);
    SS_install_cf("sqrt",
                  "Procedure: Take square root of y values of curves\n     Usage: sqrt <curve-list>",
                  UL_uopyc, 
                  PM_sqrt);
    SS_install_cf("sqrtx",
                  "Procedure: Take square root of x values of curves\n     Usage: sqrtx <curve-list>",
                  UL_uopxc, 
                  PM_sqrt);
    SS_install_cf("sqr",
                  "Procedure: Square y values of curves\n     Usage: sqr <curve-list>",
                  UL_uopyc, 
                  PM_sqr);
    SS_install_cf("sqrx",
                  "Procedure: Square x values of curves\n     Usage: sqrx <curve-list>",
                  UL_uopxc, 
                  PM_sqr);
    SS_install_cf("abs",
                  "Procedure: Take absolute value of y values of curves\n     Usage: abs <curve-list>",
                  UL_uopyc, 
                  ABS);
    SS_install_cf("absx",
                  "Procedure: Take absolute value of x values of curves\n     Usage: absx <curve-list>",
                  UL_uopxc, 
                  ABS);
    SS_install_cf("recip",
                  "Procedure: Take reciprocal of y values of curves\n     Usage: recip <curve-list>",
                  UL_uopyc, 
                  PM_recip);
    SS_install_cf("recipx",
                  "Procedure: Take reciprocal of x values of curves\n     Usage: recipx <curve-list>",
                  UL_uopxc, 
                  PM_recip);
    SS_install_cf("random",
                  "Procedure: Generate random y values between -1 and 1 for curves\n     Usage: random <curve-list>",
                  UL_uopyc, 
                  PM_random);
    SS_install_cf("cos",
                  "Procedure: Take cosine of y values of curves\n     Usage: cos <curve-list>",
                  UL_uopyc, 
                  cos);
    SS_install_cf("cosx",
                  "Procedure: Take cosine of x values of curves\n     Usage: cosx <curve-list>",
                  UL_uopxc, 
                  cos);
    SS_install_cf("acos",
                  "Procedure: Take ArcCos of y values of curves\n     Usage: acos <curve-list>",
                  UL_uopyc, 
                  acos);
    SS_install_cf("acosx",
                  "Procedure: Take ArcCos of x values of curves\n     Usage: acosx <curve-list>",
                  UL_uopxc, 
                  acos);
    SS_install_cf("cosh",
                  "Procedure: Take hx[1]erbolic cosine of y values of curves\n     Usage: cosh <curve-list>",
                  UL_uopyc, 
                  cosh);
    SS_install_cf("coshx",
                  "Procedure: Take hx[1]erbolic cosine of x values of curves\n     Usage: coshx <curve-list>",
                  UL_uopxc, 
                  cosh);
    SS_install_cf("sin",
                  "Procedure: Take sine of y values of curves\n     Usage: sin <curve-list>",
                  UL_uopyc, 
                  sin);
    SS_install_cf("sinx",
                  "Procedure: Take sine of x values of curves\n     Usage: sinx <curve-list>",
                  UL_uopxc, 
                  sin);
    SS_install_cf("asin",
                  "Procedure: Take ArcSin of y values of curves\n     Usage: asin <curve-list>",
                  UL_uopyc, 
                  asin);
    SS_install_cf("asinx",
                  "Procedure: Take ArcSin of x values of curves\n     Usage: asinx <curve-list>",
                  UL_uopxc, 
                  asin);
    SS_install_cf("sinh",
                  "Procedure: Take hx[1]erbolic sine of y values of curves\n     Usage: sinh <curve-list>",
                  UL_uopyc, 
                  sinh);
    SS_install_cf("sinhx",
                  "Take hx[1]erbolic sine of x values of curves\n     Usage: sinhx <curve-list>",
                  UL_uopxc, 
                  sinh);
    SS_install_cf("tan",
                  "Procedure: Take tangent of y values of curves\n     Usage: tan <curve-list>",
                  UL_uopyc, 
                  tan);
    SS_install_cf("tanx",
                  "Procedure: Take tangent of x values of curves\n     Usage: tanx <curve-list>",
                  UL_uopxc, 
                  tan);
    SS_install_cf("atan",
                  "Procedure: Take ArcTan of y values of curves\n     Usage: atan <curve-list>",
                  UL_uopyc, 
                  atan);
    SS_install_cf("atanx",
                  "Procedure: Take ArcTan of x values of curves\n     Usage: atanx <curve-list>",
                  UL_uopxc, 
                  atan);
    SS_install_cf("tanh",
                  "Procedure: Take hx[1]erbolic tangent of y values of curves\n     Usage: tanh <curve-list>",
                  UL_uopyc, 
                  tanh);
    SS_install_cf("tanhx",
                  "Procedure: Take hx[1]erbolic tangent of x values of curves\n     Usage: tanhx <curve-list>",
                  UL_uopxc, 
                  tanh);

    SS_install_cf("i0",
                  "Procedure: Take zeroth order modified Bessel function of the first kind of y values of curves\n     Usage: i0 <curve-list>",
                  UL_uopyc, 
                  PM_i0);
    SS_install_cf("i0x",
                  "Procedure: Take zeroth order modified Bessel function of the first kind of x values of curves\n     Usage: i0x <curve-list>",
                  UL_uopxc, 
                  PM_i0);
    SS_install_cf("i1",
                  "Procedure: Take first order modified Bessel function of the first kind of y values of curves\n     Usage: i1 <curve-list>",
                  UL_uopyc, 
                  PM_i1);
    SS_install_cf("i1x",
                  "Procedure: Take first order modified Bessel function of the first kind of x values of curves\n     Usage: i1x <curve-list>",
                  UL_uopxc, 
                  PM_i1);

    SS_install_cf("j0",
                  "Procedure: Take zeroth order Bessel function of the first kind of y values of curves\n     Usage: j0 <curve-list>",
                  UL_uopyc, 
                  PM_j0);
    SS_install_cf("j0x",
                  "Procedure: Take zeroth order Bessel function of the first kind of x values of curves\n     Usage: j0x <curve-list>",
                  UL_uopxc, 
                  PM_j0);
    SS_install_cf("j1",
                  "Procedure: Take first order Bessel function of the first kind of y values of curves\n     Usage: j1 <curve-list>",
                  UL_uopyc, 
                  PM_j1);
    SS_install_cf("j1x",
                  "Procedure: Take first order Bessel function of the first kind of x values of curves\n     Usage: j1x <curve-list>",
                  UL_uopxc, 
                  PM_j1);

    SS_install_cf("k0",
                  "Procedure: Take zeroth order modified Bessel function of the second kind of y values of curves\n     Usage: k0 <curve-list>",
                  UL_uopyc, 
                  PM_k0);
    SS_install_cf("k0x",
                  "Procedure: Take zeroth order modified Bessel function of the second kind of x values of curves\n     Usage: k0x <curve-list>",
                  UL_uopxc, 
                  PM_k0);
    SS_install_cf("k1",
                  "Procedure: Take first order modified Bessel function of the second kind of y values of curves\n     Usage: k1 <curve-list>",
                  UL_uopyc, 
                  PM_k1);
    SS_install_cf("k1x",
                  "Procedure: Take first order modified Bessel function of the second kind of x values of curves\n     Usage: k1x <curve-list>",
                  UL_uopxc, 
                  PM_k1);


    SS_install_cf("y0",
                  "Procedure: Take zeroth order Bessel function of the second kind of y values of curves\n     Usage: y0 <curve-list>",
                  UL_uopyc, 
                  PM_y0);
    SS_install_cf("y0x",
                  "Procedure: Take zeroth order Bessel function of the second kind of x values of curves\n     Usage: y0x <curve-list>",
                  UL_uopxc, 
                  PM_y0);
    SS_install_cf("y1",
                  "Procedure: Take first order Bessel function of the second kind of y values of curves\n     Usage: y1 <curve-list>",
                  UL_uopyc, 
                  PM_y1);
    SS_install_cf("y1x",
                  "Procedure: Take first order Bessel function of the second kind of x values of curves\n     Usage: y1x <curve-list>",
                  UL_uopxc, 
                  PM_y1);


/* BC handled functions */

    SS_install_cf("+",
                  "Procedure: Take sum of curves\n     Usage: + <curve-list>",
                  UL_bc, 
                  PM_fplus);
    SS_install_cf("-",
                  "Procedure: Take difference of curves\n     Usage: - <curve-list>",
                  UL_bc, 
                  PM_fminus);
    SS_install_cf("*",
                  "Procedure: Take product of curves\n     Usage: * <curve-list>",
                  UL_bc, 
                  PM_ftimes);
    SS_install_cf("/",
                  "Procedure: Take quotient of curves\n     Usage: / <curve-list>",
                  UL_bc, 
                  PM_fdivide);
    SS_install_cf("^",
		  "Procedure: Take the y values of the curve to the power a\n     Usage: ^ <curve> <a>",
                  UL_bc,
		  POW);
    SS_install_cf("min",
                  "Procedure: Construct curve from minima of y values of curves\n     Usage: min <curve-list>",
                  UL_bc, 
                  PM_fmin);
    SS_install_cf("max",
                  "Procedure: Construct curve from maxima of y values of curves\n     Usage: max <curve-list>",
                  UL_bc, 
                  PM_fmax);
    SS_install_cf("hypot",
                  "Procedure: Calculate harmonic average of two curves, sqrt(a^2+b^2)\n     Usage: hypot <a> <b>",
                  UL_bc, 
                  HYPOT);

/* BLTOC handled functions */

    SS_install_cf("color",
                  "Procedure: Set the color of curves\n     Usage: color <curve-list> <color-number>",
                  UL_bltoc, 
                  UL_color);
    SS_install_cf("fill",
                  "Procedure: Fill the area under curves with the specified color\n     Usage: fill <curve-list> <color-number>",
                  UL_bltoc, 
                  UL_fill);
    SS_install_cf("scatter",
                  "Procedure: Plot curves as scatter plots\n     Usage: scatter <curve-list> on | off",
                  UL_bltoc, 
                  UL_scatter);
    SS_install_cf("marker",
                  "Procedure: Set the marker of curves\n     Usage: marker <curve-list> plus | star | triangle",
                  UL_bltoc, 
                  UL_marker);
    SS_install_cf("histogram",
                  "Procedure: Plot curves as histograms\n     Usage: histogram <curve-list> off | left | right | center",
                  UL_bltoc, 
                  UL_hist);
    SS_install_cf("lnwidth",
                  "Procedure: Set the line widths of curves\n     Usage: lnwidth <curve-list> <width-number>",
                  UL_bltoc, 
                  UL_lnwidth);
    SS_install_cf("lnstyle",
                  "Procedure: Set the line styles of curves\n     Usage: lnstyle <curve-list> solid | dotted | dashed | dotdashed",
                  UL_bltoc, 
                  UL_lnstyle);

/* UL2TOC handled functions */

    SS_install_cf("integrate",
                  "Procedure: Integrate curves\n     Usage: integrate <curve-list> <low-lim> <high-lim>",
                  UL_ul2toc, 
                  UL_integrate);
    SS_install_cf("xmm",
                  "Procedure: Excerpt part of curves\n     Usage: xmm <curve-list> <low-lim> <high-lim>",
                  UL_ul2toc, 
                  UL_xmm);

/* ULNTOC handled functions */

    SS_install_cf("filter",
                  "Procedure: Return a filtered curve\n     Usage: filter <curve-list> <dom-pred> <ran-pred>",
                  UL_ulntoc,
                  UL_filter);
    SS_install_cf("filter-coef",
                  "Procedure: Return a curve filtered through coefficents\n     Usage: filter-coef <curve-list> <coeff-array> <ntimes>",
                  UL_ulntoc,
                  UL_filter_coef);
    SS_install_cf("smoothn",
                  "Procedure: Smooth curves using user specified smooth-method\n     Usage: smoothn <curve-list> <n> <ntimes>",
                  UL_ulntoc, 
                  UL_smooth);

/* UC handled functions */

    SS_install_cf("derivative",
                  "Procedure: Take derivative of curves\n     Usage: derivative <curve-list>",
                  UL_uc, 
                  UL_derivative);
    SS_install_cf("rev",
                  "Procedure: Swap x and y values for curves\n     You may want to sort after this\n     Usage: rev <curve-list>",
                  UL_uc, 
                  UL_reverse);
    SS_install_cf("copy",
                  "Procedure: Copy curves\n     Usage: copy <curve-list>",
                  UL_uc, 
                  UL_copy_curve);
    SS_install_cf("xindex",
                  "Procedure: Create curves with y values vs integer index values\n     Usage: xindex <curve-list>",
                  UL_uc, 
                  UL_xindex_curve);
    SS_install_cf("sort",
                  "Procedure: Sort curves' points into ascending order based on x values\n     Usage: sort <curve-list>",
                  UL_uc, 
                  UL_sort);
    SS_install_cf("compose",
                  "Functional composition f(g(x))\n     Usage: compose <f> <g>",
                  UL_bcxl, 
                  UL_compose);
    SS_install_cf("hide",
                  "Procedure: Hide curves from view\n     Usage: hide <curve-list>",
                  UL_uc, 
                  UL_hide);
    SS_install_cf("show",
                  "Procedure: Reveal curves hidden by hide command\n     Usage: show <curve-list>",
                  UL_uc, 
                  UL_show);
    SS_install_cf("thin",
                  "Procedure: Represent a curve with fewer points\n     Usage: thin <curve-list> <mode> <val>",
                  UL_ulntoc,
                  UL_thin);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
