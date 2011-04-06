/*
 * SXCONT.C - control interface routines for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

#define EOE(str) ((*SS_PTR(str) == '\n') || (*SS_PTR(str) == '\0'))

int
 SX_autorange,
 SX_autodomain,
 SX_autoplot,
 SX_background_color_white,
 SX_border_width,
 SX_data_id,
 *SX_data_index,
 SX_default_color,
 SX_default_npts,
 SX_gr_mode,
 SX_grid,
 SX_N_Curves,
 *SX_number,
 SX_plot_flag,
 SX_plot_type_size,
 SX_qflag = FALSE,
 SX_show_mouse_location,
 SX_log_scale[PG_SPACEDM];

double
 SX_chi,
 SX_marker_orientation,
 SX_phi,
 SX_show_mouse_location_x,
 SX_show_mouse_location_y,
 SX_theta;

char
 *SX_text_output_format,
 SX_data_directory[MAXLINE] = "",
 *SX_display_name,
 *SX_display_type,
 *SX_display_title,
 *SX_plot_type_style,
 *SX_promotion_type,
 *SX_smooth_method;

PG_rendering
 SX_render_def,
 SX_render_1d_1d,
 SX_render_2d_1d,
 SX_render_2d_2d,
 SX_render_3d_1d;

curve
 *SX_dataset;

object
 *SX_curfile;

SX_state
 _SX = { 3, 1.0e-15, 1.0e-3, 2.0, 'a', LITERAL, 1, 1, '@', "curves.a",
         TRUE, };

SX_file_action
 SX_file_exist_action = FAIL;

static char
 *SX_MAPPING_S = "PM_mapping *",
 *SX_IMAGE_S = "PG_image *",
 *SX_CURVE_S = "PG_curve";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ARGS - get a C level data item from a single Scheme object */

void _SX_args(object *obj, void *v, int type)
   {void **pv;
    char *s;

    pv = (void **) v;
    if (pv == NULL)
       return;

    else if (SS_nullobjp(obj) && (type != G_FILE))
       {*pv = NULL;
        return;};

    switch (type)
       {case G_FILE :
             if (!SS_nullobjp(obj) && !SX_FILEP(obj))
                SS_error("OBJECT NOT FILE - _SX_ARGS", obj);

             if (SS_nullobjp(obj))
                *pv = (void *) SX_gvif;
             else
                *pv = (void *) SS_GET(g_file, obj);
             break;

        case G_DEVICE :
             if (!SX_DEVICEP(obj))
                SS_error("OBJECT NOT PG_DEVICE - _SX_ARGS", obj);
             *pv = (void *) SS_GET(PG_device, obj);
             break;

        case G_SET :
             if (!SX_SETP(obj))
                SS_error("OBJECT NOT PM_SET - _SX_ARGS", obj);
             *pv = (void *) SS_GET(PM_set, obj);
             break;

        case G_MAPPING :
             if (SX_GRAPHP(obj))
                *pv = (void *) (SS_GET(PG_graph, obj)->f);
             else if (SX_MAPPINGP(obj))
                *pv = (void *) SS_GET(PM_mapping, obj);
             else
                SS_error("OBJECT NOT PM_MAPPING - _SX_ARGS", obj);
             break;

        case G_GRAPH :
             if (SX_GRAPHP(obj))
                *pv = (void *) SS_GET(PG_graph, obj);
             else
                SS_error("OBJECT NOT PG_GRAPH - _SX_ARGS", obj);
             break;

        case G_NUM_ARRAY :
             if (!SX_NUMERIC_ARRAYP(obj))
                SS_error("OBJECT NOT C_ARRAY - _SX_ARGS", obj);
             *pv = (void *) SS_GET(C_array, obj);
             break;

        case G_IMAGE :
             if (!SX_IMAGEP(obj))
                SS_error("OBJECT NOT PG_IMAGE - _SX_ARGS", obj);
             *pv = (void *) SS_GET(PG_image, obj);
             break;

        case G_PACKAGE :
             if (!SX_PACKAGEP(obj))
                {s   = SS_get_string(obj);
                 *pv = (void *) PA_INQUIRE_VARIABLE(s);
                 if (pv == NULL)
                    SS_error("BAD PACKAGE - _SX_ARGS", obj);}
             else
                *pv = (void *) SS_GET(PA_package, obj);
             break;

        case G_PANVAR :
             if (!SX_PANVARP(obj))
                {s   = CSTRSAVE(SS_get_string(obj));
                 *pv = (void *) PA_INQUIRE_VARIABLE(s);
                 if (pv == NULL)
                    SS_error("NOT PANACEA VARIABLE - _SX_ARGS",
                             obj);}
             else
                *pv = (void *) SS_GET(PA_variable, obj);
             break;

        case G_PDBDATA :
             if (!SX_PDBDATAP(obj))
                SS_error("NOT PDBDATA OBJECT - _SX_ARGS", obj);

             *pv = (void *) SS_GET(g_pdbdata, obj);
             break;

        case G_DEFSTR :
             if (!SX_DEFSTRP(obj))
                SS_error("NOT DEFSTR OBJECT - _SX_ARGS", obj);

             *pv = (void *) SS_GET(defstr, obj);
             break;

        case G_SYMENT   :
        case G_PLOT_MAP :
        case G_PLT_CRV  :
        case G_FUNCTION :

        case G_DEV_ATTRIBUTES   :
        case G_SOURCE_VARIABLE  :
        case G_IV_SPECIFICATION :
        case G_PLOT_REQUEST     :

        default :
             SS_error("BAD TYPE - _SX_ARGS", SS_mk_integer(type));};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_CALL_ARGS - make a SCHEME level object from a C level one */

object *_SX_call_args(int type, void *v)
   {object *obj;

    obj = SS_null;

    switch (type)
       {case G_FILE :
             if (v == NULL)
                obj = SX_ovif;
             else
                obj = SX_mk_gfile((g_file *) v);
             break;

        case G_DEVICE :
             if (v != NULL)
                obj = SX_mk_graphics_device((PG_device *) v);
             break;

        case G_SET :
             if (v != NULL)
                obj = SX_mk_set((PM_set *) v);
             break;

        case G_MAPPING :
             if (v != NULL)
                obj = SX_mk_mapping((PM_mapping *) v);
             break;

        case G_GRAPH :
             if (v != NULL)
                obj = SX_mk_graph((PG_graph *) v);
             break;

        case G_NUM_ARRAY :
             if (v != NULL)
                obj = SX_mk_C_array((C_array *) v);
             break;

        case G_IMAGE :
             if (v != NULL)
                obj = SX_mk_image((PG_image *) v);
             break;

        case G_PACKAGE :
             if (v != NULL)
                obj = SX_mk_package((PA_package *) v);
             break;

        case G_PANVAR :
             if (v != NULL)
                obj = SX_mk_variable((PA_variable *) v);
             break;

        case G_DEFSTR :
             if (v != NULL)
                obj = _SX_mk_gdefstr((defstr *) v);
             break;

        case G_SYMENT :
             if (v != NULL)
                obj = _SX_mk_gsyment((syment *) v);
             break;

        case G_PDBDATA  :
        case G_PLOT_MAP :
        case G_PLT_CRV  :
        case G_FUNCTION :

        case G_DEV_ATTRIBUTES   :
        case G_SOURCE_VARIABLE  :
        case G_IV_SPECIFICATION :
        case G_PLOT_REQUEST     :

        default                 :
             SS_error("BAD TYPE - _SX_CALL_ARGS", SS_mk_integer(type));};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_LIST_VOBJECTS - list the items from the specified list matching the
 *                   - given pattern
 */

static object *_SX_list_vobjects(char *patt, g_file *po, int type)
   {long i, n;
    int t;
    char *s;
    SX_menu_item *mitem;
    object *ret;

    ret = SS_null;

    n = SC_array_get_n(po->menu_lst);
    if (n > 0L)
       {PRINT(stdout, "\n");

	for (i = 0L; i < n; i++)
	    {mitem = SC_array_get(po->menu_lst, i);

	     s = mitem->label;
	     t = mitem->type[3];
	     if (((type == 0) || (t == type)) && SC_regx_match(s, patt))
	        {PRINT(stdout, " %ld  %c  %s\n", i+1, t, s);
		 SS_Assign(ret, SS_mk_cons(SS_mk_integer(i+1), ret));}};

	SS_Assign(ret, SS_reverse(ret));};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MENU - print a menu of images, curves, and graphs */

object *_SXI_menu(object *argl)
   {char *patt, *type;
    g_file *po;
    object *ret;

    ret = SS_null;

    po   = NULL;
    patt = NULL;
    type = NULL;
    SS_args(argl,
            G_FILE, &po,
            SC_STRING_I, &patt,
            SC_STRING_I, &type,
            0);

    if (strcmp(po->type, PDBFILE_S) != 0)
       return(ret);

    _SX_get_menu(po);

    if (type != NULL)
       ret = _SX_list_vobjects(patt, po, type[0]);
    else
       ret = _SX_list_vobjects(patt, po, 0);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SORT_LISTS - sort the menu according to the alphabetization
 *                - of the labels
 */

static void _SX_sort_lists(SX_menu_item *a, int n)
   {int gap, i, j;
    SX_menu_item temp;

    for (gap = n/2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i-gap; j >= 0; j -= gap)
                {if (strcmp(a[j].label, a[j+gap].label) <= 0)
                    break;
                 temp     = a[j];
                 a[j]     = a[j+gap];
                 a[j+gap] = temp;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GET_MENU - make lists of syments for the mappings, images, and curves 
 *              - in the current directory of the given file
 */

void _SX_get_menu(g_file *po)
   {int i, n;
    char *dir, **names;
    PDBfile *file;
    SX_menu_item *mitems;

    file = FILE_FILE(PDBfile, po);

/* because we don't know the state of the pointer lists
 * we did have at least one problem where the label strings
 * had been read and freed elsewhere
 */
    PD_reset_ptr_list(file);

    _SX_free_menu(po, TRUE);

    dir = (PD_has_directories(file)) ? PD_pwd(file) : NULL;

    names = PD_ls(file, dir, "PM_mapping *", &n);
    for (i = 0; i < n; i++)
        _SX_push_menu_item(po, names[i], SX_MAPPING_S);
    CFREE(names);

    names = PD_ls(file, dir, "PG_image *", &n);
    for (i = 0; i < n; i++)
        _SX_push_menu_item(po, names[i], SX_IMAGE_S);
    CFREE(names);

    names = PD_ls(file, dir, SC_CHAR_S, &n);
    for (i = 0; i < n; i++)
        if (SC_regx_match(names[i], "curve????"))
	   _SX_push_menu_item(po, names[i], SX_CURVE_S);
    CFREE(names);

    n      = SC_array_get_n(po->menu_lst);
    mitems = SC_array_array(po->menu_lst);

    _SX_sort_lists(mitems, n);

    CFREE(mitems);

/* let's give the next place a break */
    PD_reset_ptr_list(file);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_PUSH_MENU_ITEM - add the given symbol table entry to the file menu */

void _SX_push_menu_item(g_file *po, char *name, char *type)
   {SX_menu_item mitem;
    char s[MAXLINE];
    char *var, *lb;
    PDBfile *file;

    file = FILE_FILE(PDBfile, po);

    mitem.type = type;

    strcpy(s, _PD_fixname(file, name));
    mitem.vname = CSTRSAVE(s);

/* get mapping label */
    if (strcmp(type, SX_MAPPING_S) == 0)
       {var = SC_dsnprintf(FALSE, "%s[%d].name", s, file->default_offset);
        if (!PD_read(file, var, &lb))
 	   SS_error("FAILED TO READ LABEL - _SX_PUSH_MENU_ITEM", SS_null);

	mitem.label = CSTRSAVE(lb);

	SC_array_push(po->menu_lst, &mitem);}

/* get curve label */
    else if (strcmp(type, SX_CURVE_S) == 0)
       {char *u, t[MAXLINE], *pt;

        if (!PD_read(file, s, t))
           SS_error("FAILED TO READ CURVE HEADER - _SX_PUSH_MENU_ITEM",
		    SS_null);

/* extract the name of the label variable */
        lb = SC_strtok(t, "|", pt);
        if (lb == NULL)
           SS_error("BAD CURVE HEADER - _SX_PUSH_MENU_ITEM", SS_null);

/* get the label */
        else if (PD_has_directories(file))
           {u = strrchr(s, '/');
            u = (u == NULL) ? s : u + 1;
            strcpy(u, lb);}
        else
           strcpy(s, lb);

        if (!PD_read(file, s, t))
           SS_error("FAILED TO READ LABEL - _SX_PUSH_MENU_ITEM", SS_null);
        mitem.label = CSTRSAVE(t);

	SC_array_push(po->menu_lst, &mitem);}

/* get image label */
    else if (strcmp(type, SX_IMAGE_S) == 0)
       {var = SC_dsnprintf(FALSE, "%s[%d].label", s, file->default_offset);
        if (!PD_read(file, var, &lb))
           SS_error("FAILED TO READ LABEL - _SX_PUSH_MENU_ITEM", SS_null);
	mitem.label = CSTRSAVE(lb);

	SC_array_push(po->menu_lst, &mitem);}

    else
       SS_error("BAD OBJECT TYPE - _SX_PUSH_MENU_ITEM", SS_null);

    return;}

/*--------------------------------------------------------------------------*/

/*                     INPUT REPROCESSING ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _SX_ISODD - return TRUE iff the given number is odd */

static int _SX_isodd(int n)
   {int rv;

    rv = (2*(n/2) == n) ? FALSE : TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_NO_ARGSP - return TRUE iff the object is a procedure object which
 *              - may have zero arguments
 */

static int _SX_no_argsp(object *obj)
   {int ret;
    object *params;
    PFPHand hand;

    ret = FALSE;

    switch (SS_PROCEDURE_TYPE(obj))
       {case SS_MACRO : 
        case SS_PROC  :
	     params = SS_params(obj);
	     ret = !SS_consp(params) || (params == SS_null);
	     break;

        default :
	     hand = SS_C_PROCEDURE_HANDLER(obj);
	     ret  = (hand ==            SS_zargs)      ||
	            (hand ==            SS_znargs)     ||
		    (hand == (PFPHand) SS_acc_char)   ||
		    (hand == (PFPHand) SS_acc_int)    ||
		    (hand == (PFPHand) SS_acc_long)   ||
		    (hand == (PFPHand) SS_acc_double)   ||
		    (hand == (PFPHand) SS_acc_string) ||
		    (hand == (PFPHand) SS_acc_ptr);
	     break;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PARSE - determine whether or not to reprocess the input for SX
 *          - this is the real worker for the SS_post_eval_hook
 *          - since this SS_evobj is not the same as in SS_REPL
 *          - it should be SS_MARK'd as being an additional pointer to its
 *          - respective object
 */

void SX_parse(void (*replot)(void), char *(*reproc)(char *s),
	      object *strm)
   {char *t, s[MAXLINE], line[MAXLINE], *ptr;

    if (SS_procedurep(SS_evobj))
       {strcpy(s, SS_PP(SS_evobj, name));
        if (_SX_no_argsp(SS_evobj) || !EOE(strm))
           {ptr = SS_BUFFER(strm);
            if (_SX_isodd(SC_char_count(ptr, '\"')))
               {PRINT(stdout, "\nUNMATCHED QUOTE: %s\n\n", ptr);
                *SS_BUFFER(strm) = '\0';
                SS_PTR(strm) = SS_BUFFER(strm);}
            else
               {strcpy(line, ptr);
                while ((t = (*reproc)(line)) != NULL)
                  {strcpy(ptr, t);
                   SS_PTR(strm) = SS_BUFFER(strm);
                   SS_Assign(SS_rdobj, SS_read(strm));
                   SS_interactive = ON;
                   SX_plot_flag   = TRUE;
                   SS_Assign(SS_evobj, SS_eval(SS_rdobj));
                   SS_interactive = OFF;};

                if (SX_plot_flag && (strcmp(s, "replot") != 0) &&
                    (SX_autoplot == ON) &&
                    (replot != NULL))
                   (*replot)();};};};

    if (PG_console_device != NULL)
       PG_console_device->gprint_flag = TRUE;

/*    for (t = SS_BUFFER(strm); *t; t++); */
    for (t = SS_PTR(strm); *t == '\n'; t++);
    SS_PTR(strm) = t;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_WRAP_PAREN - enclose second arg by first and third args */

char *SX_wrap_paren(char *open, char *form, char *close, size_t ln)
   {char tmp[MAXLINE];

   strcpy(tmp, form);

   snprintf(form, ln, "%s%s%s", open, tmp, close);

   return(form);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ISTHROUGH - return pointer if :range found, otherwise NULL */

static char *_SX_isthrough(char *s)
   {char *sp;

    for (sp = s; *sp != '\0'; sp++)

/* skip quoted strings */
        {if (*sp == '\"')
            for (sp++; *sp != '\"'; sp++);

/* skip index expressions */
	 if (*sp == '[')
            for (sp++; *sp != ']'; sp++);

         if (*sp == ':')

/* do not stop on a UDL */
	    {if (_SC_udlp(sp) == FALSE)
	        return(sp);};};

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_EXPAND_FIRST - expand the first missing menu/lst object */

static char *_SX_expand_first(char *s, char *sp)
   {char s1[MAXLINE], s2[MAXLINE], temp[MAXLINE];
    char *tp, *lexpr, *rexpr;
    int unbalanced;

    *sp = '\0';

/* break the portion of the command before the colon into two strings:
 * the expression immediately preceding the colon (lexpr) and the rest.
 */
    strcpy(s1, s);
    for (tp = s1 + (sp - s) - 1; *tp == ' '; tp--);

    if (*tp == ')')
       {*(tp + 2) = '\0';
        *(tp + 1) = ')';
        for (unbalanced = 1; unbalanced; tp--)
            {*tp = *(tp - 1);
             if (*tp == ')')
                unbalanced++;
             else if (*tp == '(')
                unbalanced--;}
        *tp = '\0';
        lexpr = tp + 1;}
    else
       lexpr = SC_lasttok(s1, " ,");

/* break the portion of the command after the colon into two strings:
 * the expression immediately following the colon (rexpr) and the rest.
 */
    for (tp = sp + 1; *tp == ' '; tp++);
    strcpy(s2, tp);

    if (*tp == '(')
       {rexpr = tp++;
        strcpy(temp, rexpr);
        rexpr = temp;
        for (unbalanced = 1; unbalanced; tp++)
            {if (*tp == '(')
                unbalanced++;
             else if (*tp == ')')
                unbalanced--;}
        strcpy(s2, tp);
        *tp = '\0';}
    else
       rexpr = SC_firsttok(s2, " ,");

/* build a new expanded command from all the parts.
 * removing the blank after the ')' causes multiple ranges to give an error.
 * the blank before the '(' improves the aesthetics of command log output.
 */
    sprintf(s, "%s (thru %s %s) %s", s1, lexpr, rexpr, s2);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_EXPAND_EXPR - expand object lists of the form x:y 
 *                - which means all object from x through y inclusive
 */

int SX_expand_expr(char *s)
   {char *sp;
    int ret;

/* something wrong in input if s is NULL */
    if (s == NULL)
       return(FALSE);

    sp = _SX_isthrough(s);

/* if nothing to expand - termination condition for loop */
    if (sp == NULL)
       ret = TRUE;

    else

/* expand the first missing object */
       {s = _SX_expand_first(s, sp);

/* loop through to find any more instances */
        ret = SX_expand_expr(s);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SPLIT_COMMAND - return the next command from the string in
 *                  - the given buffer
 *                  - return FALSE if there are no more commands
 */

int SX_split_command(char *cmd, char *lst)
   {int ok, rv;
    char *pc, *pl;

    ok = FALSE;
    rv = FALSE;
    for (pl = lst, pc = cmd; ok == FALSE; )
        {switch (*pc++ = *pl++)
            {case '\"' :
	          while ((*pc++ = *pl++) != '\"');
		  break;

             case '\r' :
             case '\n' :
             case ';'  :
	          pc--;
		  *pc = '\0';

/* copy the remainder of lst down to the beginning of lst */
		  for (pc = lst; (*pc++ = *pl++) != '\0'; );
		  ok = TRUE;
		  rv = TRUE;
		  break;

             case '\0' :
		  ok = TRUE;
		  rv = FALSE;
		  break;

             default :
	          break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*_SX_NO_DEVICE_SUPPORT - handler to support non-existence devices
 *                      - return zero to emulate the existence flag
 *                      - for a device that actually exists
 */

static object *_SX_no_device_support(void)
   {object *rv;

    rv = SS_mk_integer(0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_GET_DEVICE - given an index to a device
 *               - return the out_device corresponding to the index
 */

out_device *SX_get_device(int idev)
   {out_device *out;

    out = NULL;
    if ((0 <= idev) && (idev < N_OUTPUT_DEVICES))
       out = _SX.output_devices + idev;

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MATCH_DEVICE - given a PG_device
 *                 - return the out_device containing it
 */

out_device *SX_match_device(PG_device *dev)
   {int i;
    out_device *out;

    for (i = 0; i < N_OUTPUT_DEVICES; i++)
        {out = _SX.output_devices + i;
	 if (out == NULL)
	    continue;
    
	 if (out->dev == dev)
	    break;};

    if (i >= N_OUTPUT_DEVICES)
       out = NULL;

    return(out);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INSTALL_DEVICE_VARS - install the device variables in the symtab */

static void SX_install_device_vars(void)
   {int i;
    char dupp[MAXLINE];
    char *dname, *name, *doc;
    out_device *out;
    
    for (i = 0; i < N_OUTPUT_DEVICES; i++)
        {out = _SX.output_devices + i;
	 if (out == NULL)
	    continue;
    
	 dname = out->dname;
	 strcpy(dupp, dname);
	 SC_str_upper(dupp);

	 if (out->exist)
	    {out->fname = CSTRSAVE("plots");
             if (strcmp(dupp, "PS") == 0)
	        out->type = CSTRSAVE("monochrome");
             else
	        out->type = CSTRSAVE("RGB");

	     doc  = SC_dsnprintf(TRUE, "Variable: When TRUE print will generate a plot in a %s file",
				 dupp);
	     name = SC_dsnprintf(FALSE, "%s-flag", dname);
	     SS_install_cf(name, doc, SS_acc_int, &out->active);

	     doc  = SC_dsnprintf(TRUE, "Variable: The background color to use for a %s file",
				 dupp);
	     name = SC_dsnprintf(FALSE, "%s-background-color-flag", dname);
	     SS_install_cf(name, doc, SS_acc_int, &out->background_color);

	     doc  = SC_dsnprintf(TRUE, "Variable: %s file name\n     Usage: %s-name <string>",
				 dupp, dname);
	     name = SC_dsnprintf(FALSE, "%s-name", dname);
	     SS_install_cf(name, doc, SS_acc_ptr, &out->fname);

             if (strcmp(dupp, "PS") == 0)
	        doc = SC_dsnprintf(TRUE,  "Variable: %s file type (monochrome or color)\n     Usage: %s-type <string>",
				   dupp, dname);
             else
	        doc = SC_dsnprintf(TRUE,  "Variable: %s file type (monochrome or rgb)\n     Usage: %s-type <string>",
				   dupp, dname);
	     name = SC_dsnprintf(FALSE, "%s-type", dname);
	     SS_install_cf(name, doc, SS_acc_ptr, &out->type);

	     doc  = SC_dsnprintf(TRUE, "Variable: %s window height in fraction of screen width\n     Usage: window-height-%s <real>",
				 dupp, dname);
	     name = SC_dsnprintf(FALSE, "window-height-%s", dname);
	     SS_install_cf(name, doc, SS_acc_double, &out->height);

	     doc  = SC_dsnprintf(TRUE,  "Variable: %s X comp of window origin (frac of screen width)\n     Usage: window-origin-x-%s <real>",
				 dupp, dname);
	     name = SC_dsnprintf(FALSE, "window-origin-x-%s", dname);
	     SS_install_cf(name, doc, SS_acc_double, &out->x0);

	     doc  = SC_dsnprintf(TRUE, "Variable: %s Y comp of window origin (frac of screen width)\n     Usage: window-origin-y-%s <real>",
				 dupp, dname);
	     name = SC_dsnprintf(FALSE, "window-origin-y-%s", dname);
	     SS_install_cf(name, doc, SS_acc_double, &out->y0);

	     doc  = SC_dsnprintf(TRUE, "Variable: %s window width in fraction of screen width\n     Usage: window-width-%s <real>",
				 dupp, dname);
	     name = SC_dsnprintf(FALSE, "window-width-%s", dname);
	     SS_install_cf(name, doc, SS_acc_double, &out->width);}

	 else
	    {doc  = SC_dsnprintf(TRUE,  "%s-flag is not supported as configured",
				 dname);
	     name = SC_dsnprintf(FALSE, "%s-flag", dname);
	     SS_install_cf(name, doc, SS_zargs,
			   _SX_no_device_support, SS_PR_PROC);

	     doc  = SC_dsnprintf(TRUE,  "%s-name is not supported as configured",
				 dname);
	     name = SC_dsnprintf(FALSE, "%s-name", dname);
	     SS_install_cf(name, doc, SS_zargs,
			   _SX_no_device_support, SS_PR_PROC);

	     doc  = SC_dsnprintf(TRUE,  "%s-type is not supported as configured",
				 dname);
	     name = SC_dsnprintf(FALSE, "%s-type", dname);
	     SS_install_cf(name, doc, SS_zargs,
			   _SX_no_device_support, SS_PR_PROC);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INIT_DEVICE_VARS - set initial values for device variables */

void SX_init_device_vars(int idev, double xf, double yf, double wd, double hg)
   {out_device *out;
    
    out = SX_get_device(idev);
    if (out != NULL)
       {out->x0     = xf;
	out->y0     = yf;
	out->width  = wd;
	out->height = hg;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_REGISTER_DEVICES - register output devices for general use */

void SX_register_devices(void)
   {int i, nd;
    char lname[MAXLINE], uname[MAXLINE];
    double xf, yf, wd, hg;
    PG_device *dev;
    out_device *out;
    static char *devs[] = {"ps", "png", "jpeg", "cgm", "mpeg"};
    
/* setup the markers before the raster devices try to get in on the
 * low numbered ones
 */
    PG_setup_markers();

    nd = sizeof(devs)/sizeof(char *);
    for (i = 0; i < nd; i++)
        {out = SX_get_device(i);

	 strcpy(lname, devs[i]);
	 strcpy(uname, devs[i]);
	 SC_str_upper(uname);

	 out->dname = SC_strsavef(lname, "PERM|char*:SX_REGISTER_DEVICES:dname");
	 out->dupp  = SC_strsavef(uname, "PERM|char*:SX_REGISTER_DEVICES:dupp");

	 dev = PG_make_device(uname, "COLOR", "tmp");
	 out->exist = (dev != NULL);
	 PG_close_device(dev);

	 if (out->exist)
	    {xf = 0.0;
	     yf = 0.0;
	     if ((strcmp(devs[i], "ps") == 0) ||
		 (strcmp(devs[i], "cgm") == 0))
	        {wd = 0.0;
		 hg = 0.0;}
	     else
	        {wd = 512.0;
		 hg = 512.0;};
	     SX_init_device_vars(i, xf, yf, wd, hg);};

	 out->background_color = -1;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_LOAD_RC - load runtime file FFN and
 *            - one of the init files IFNA or IFNB
 */

void SX_load_rc(char *ffn, int ldrc, char *ifna, char *ifnb)
   {char *s;

/* figure out the runtime file */
    if (SC_query_file(ffn, "r", "ascii"))
       s = CSTRSAVE(ffn);

    else
       {s = SC_search_file(NULL, ffn);
	if (s == NULL)
	   {PRINT(stderr, "Can't find %s\n", ffn);
	    PRINT(stderr, "Check SCHEME environment variable\n\n");
	    SX_quit(ABORT);};};

/* load the runtime file */
    SS_load_scm(s);

    CFREE(s);

/* figure out the init file */
    if (ldrc == TRUE)
       {s = SC_search_file(NULL, ifna);
	if ((s == NULL) || !SC_query_file(s, "r", "ascii"))
	   {CFREE(s);

	    s = SC_search_file(NULL, ifnb);
	    if (s == NULL)
	       {PRINT(stderr, "Can't find %s or %s\n", ifna, ifnb);
		PRINT(stderr, "Check SCHEME environment variable\n\n");
		SX_quit(ABORT);};

	    if (!SC_query_file(s, "r", "ascii"))
	       {PRINT(stderr, "Can't open %s or %s\n", ifna, ifnb);
		SX_quit(ABORT);};};

/* load the init file */
	SS_load_scm(s);

	CFREE(s);};

    return;}

/*--------------------------------------------------------------------------*/

/*                         INSTALLATION ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* SX_INSTALL_GLOBAL_VARS - install the global variables */

void SX_install_global_vars(void)
   {int i;
    int *sqzlab, *refm, *refmc, *hider;
    int *hsts, *nlev, *scat, *cmtex, *intf, *mrki;
    int *lgcp, *ndi, *pltlb, *plttl, *pltdt, *palor;
    int *labcf, *labts, *labln, *lnclr, *lnsty, *txclr;
    int *rstvp;
    double *axsca, *axscs, *axsgs, *axslb;
    double *axsls, *axslw, *axsmts, *axsmjt, *axsmnt, *axndec;
    double *axsmxt, *axsmyt, *axson, *axstt, *axsty;
    double *cntrat, *psdpi, *lnwid;
    double *errcsz, *labsp, *labyo, *mrks;
    char **axslxf, **axslyf, **axstf, **txtfm;
    PG_rendering *ppty;

    PG_setup_attrs_glb();

    axsca     = PG_ptr_attr_glb("axis-char-angle");
    axscs     = PG_ptr_attr_glb("axis-char-size");
    axsgs     = PG_ptr_attr_glb("axis-grid-style");
    axslb     = PG_ptr_attr_glb("axis-labels");
    axslxf    = PG_ptr_attr_glb("axis-label-x-format");
    axslyf    = PG_ptr_attr_glb("axis-label-y-format");
    axsls     = PG_ptr_attr_glb("axis-line-style");
    axslw     = PG_ptr_attr_glb("axis-line-width");
    axsmts    = PG_ptr_attr_glb("axis-major-tick-size");
    axsmjt    = PG_ptr_attr_glb("axis-max-major-ticks");
    axndec    = PG_ptr_attr_glb("axis-n-decades");
    axsmnt    = PG_ptr_attr_glb("axis-n-minor-ticks");
    axsmxt    = PG_ptr_attr_glb("axis-n-minor-x-ticks");
    axsmyt    = PG_ptr_attr_glb("axis-n-minor-y-ticks");
    axson     = PG_ptr_attr_glb("axis-on");
    axstt     = PG_ptr_attr_glb("axis-tick-type");
    axsty     = PG_ptr_attr_glb("axis-type");
    axstf     = PG_ptr_attr_glb("axis-type-face");
    cmtex     = PG_ptr_attr_glb("color-map-to-extrema");
    cntrat    = PG_ptr_attr_glb("contour-ratio");
    nlev      = PG_ptr_attr_glb("contour-n-levels");
    errcsz    = PG_ptr_attr_glb("error-cap-size");
    hider     = PG_ptr_attr_glb("hide-rescale");
    hsts      = PG_ptr_attr_glb("hist-start");
    intf      = PG_ptr_attr_glb("interp-flag");
    labcf     = PG_ptr_attr_glb("label-color-flag");
    labln     = PG_ptr_attr_glb("label-length");
    labsp     = PG_ptr_attr_glb("label-space");
    labts     = PG_ptr_attr_glb("label-type-size");
    labyo     = PG_ptr_attr_glb("label-yoffset");
    lnclr     = PG_ptr_attr_glb("line-color");
    lnsty     = PG_ptr_attr_glb("line-style");
    lnwid     = PG_ptr_attr_glb("line-width");
    lgcp      = PG_ptr_attr_glb("logical-plot");
    mrki      = PG_ptr_attr_glb("marker-index");
    mrks      = PG_ptr_attr_glb("marker-scale");
    ndi       = PG_ptr_attr_glb("numeric-data-id");
    palor     = PG_ptr_attr_glb("palette-orientation");
    pltdt     = PG_ptr_attr_glb("plot-date");
    pltlb     = PG_ptr_attr_glb("plot-labels");
    plttl     = PG_ptr_attr_glb("plot-title");
    ppty      = PG_ptr_attr_glb("plot-type");
    psdpi     = PG_ptr_attr_glb("ps-dots-inch");
    refm      = PG_ptr_attr_glb("ref-mesh");
    refmc     = PG_ptr_attr_glb("ref-mesh-color");
    rstvp     = PG_ptr_attr_glb("restore-viewport");
    scat      = PG_ptr_attr_glb("scatter-plot");
    sqzlab    = PG_ptr_attr_glb("squeeze-labels");
    txclr     = PG_ptr_attr_glb("text-color");
    txtfm     = PG_ptr_attr_glb("text-format");

    *sqzlab = FALSE;
    *labcf  = FALSE;
    *labln  = 25;
    *labsp  = 0.0;
    *labts  = 8;
    *labyo  = 0.0;
    *axslxf = CSTRSAVE("%10.2g");
    *axslyf = CSTRSAVE("%10.2g");
    *axstf  = CSTRSAVE("helvetica");

    SX_autodomain             = TRUE;
    SX_autoplot               = ON;
    SX_autorange              = TRUE;
    SX_border_width           = 2;
    SX_chi                    = 0.0;
    SX_data_id                = TRUE,
    SX_default_color          = -1;
    SX_GRI_x                  = 0.5;
    SX_GRI_y                  = 0.01;
    SX_GRI_dx                 = 0.0;
    SX_GRI_dy                 = 0.0;
    SX_grid                   = OFF;
    SX_marker_orientation     = 0.0;

/*    SX_palette              = CSTRSAVE("spectrum"); */
    SX_phi                    = 0.0;
    SX_plot_type_size         = 12;
    SX_plot_type_style        = CSTRSAVE("medium");
    SX_promotion_type         = CSTRSAVE("none");
    SX_render_def             = PLOT_MESH;
    SX_render_1d_1d           = *ppty;
    SX_render_2d_1d           = PLOT_CONTOUR;
    SX_render_2d_2d           = PLOT_VECTOR;
    SX_render_3d_1d           = PLOT_MESH;
    SX_show_mouse_location    = FALSE;
    SX_show_mouse_location_x  = 0.025;
    SX_show_mouse_location_y  = 0.955;
    SX_smooth_method          = CSTRSAVE("averaging");
    SX_theta                  = 0.0;

    SX_GRI_title            = CSTRSAVE("PDBView Controls");
    SX_GRI_type_face        = CSTRSAVE("helvetica");
    SX_GRI_type_style       = CSTRSAVE("medium");
    SX_GRI_type_size        = 12;

    SS_interactive = FALSE;
    SS_print_flag  = FALSE;
    SS_stat_flag   = FALSE;

    for (i = 0; i < PG_SPACEDM; i++)
        SX_log_scale[i] = FALSE;

/* WC to BND offset */
    for (i = 0; i < PG_BOXSZ; i++)
        SX_gpad[i] = 0.01;

/* A */

    SS_install_cf("answer-prompt",
                  "Variable: A string printed before the return value\n     Usage: answer-prompt <string>",
                  SS_acc_string,
                  SS_ans_prompt);

    SS_install_cf("ascii-output-format",
                  "Variable: Controls format for ASCII output of floating point numbers\n     Usage: ascii-output-format <format>",
                  SS_acc_ptr,
                  &SX_text_output_format);

    SS_install_cf("autorange",
                  "Variable: Turns on or off autoranging\n     Usage: autorange on | off",
                  SS_acc_int,
                  &SX_autorange);

    SS_install_cf("autodomain",
                  "Variable: Turns on or off autodomain\n     Usage: autodomain on | off",
                  SS_acc_int,
                  &SX_autodomain);

    SS_install_cf("autoplot",
                  "Variable: Controls autoploting of graphs\n     Usage: autoplot on | off",
                  SS_acc_int,
                  &SX_autoplot);

    SS_install_cf("axis",
                  "Variable: Controls drawing the axes\n     Usage: axis on | off ",
                  SS_acc_int,
		  axson);

    SS_install_cf("axis-max-major-ticks",
                  "Variable: Controls maximum number of major ticks\nUsage: axis-max-major-ticks <integer> ",
                  SS_acc_int,
		  axsmjt);

    SS_install_cf("axis-number-minor-ticks",
                  "Variable: Controls number of minor axis ticks\nUsage: axis-number-minor-ticks <integer> ",
                  SS_acc_int,
		  axsmnt);

    SS_install_cf("axis-number-minor-x-ticks",
                  "Variable: Controls number of minor X axis ticks\nUsage: axis-number-minor-x-ticks <integer> ",
                  SS_acc_int,
		  axsmxt);

    SS_install_cf("axis-number-minor-y-ticks",
                  "Variable: Controls number of minor Y axis ticks\nUsage: axis-number-minor-y-ticks <integer> ",
                  SS_acc_int,
		  axsmyt);

    SS_install_cf("axis-n-decades",
                  "Variable: Controls maximum number of log axis decades\nUsage: axis-n-decades <real> ",
                  SS_acc_double,
                  axndec);

    SS_install_cf("axis-tick-type",
                  "Variable: Controls location of axis ticks\nUsage: axis-tick-type left-of-axis | right-of-axis | straddle-axis ",
                  SS_acc_int,
		  axstt);

    SS_install_cf("axis-char-size",
                  "Variable: Controls character size for the axes\n     Usage: axis-char-size <integer> ",
                  SS_acc_int,
		  axscs);

    SS_install_cf("axis-char-angle",
                  "Variable: Controls angle at which characters are drawn\n     Usage: axis-char-angle <real> ",
                  SS_acc_double,
		  axsca);

    SS_install_cf("axis-grid-style",
                  "Variable: Controls line style for the axis grid\n     Usage: axis-grid-style solid | dotted | dashed | dotdashed ",
                  SS_acc_int,
		  axsgs);

    SS_install_cf("axis-labels",
                  "Variable: Controls axis labelling\n     Usage: axis-labels on | off",
                  SS_acc_int,
		  axslb);

    SS_install_cf("axis-line-style",
                  "Variable: Controls line style for the axes\n     Usage: axis-line-style solid | dotted | dashed | dotdashed ",
                  SS_acc_int,
		  axsls);

    SS_install_cf("axis-line-width",
                  "Variable: Controls line width for the axes\n     Usage: axis-line-width [<real>] ",
                   SS_acc_double,
                   axslw);

    SS_install_cf("axis-tick-size",
                  "Variable: Controls major tick size for the axes\n     Usage: axis-tick-size <real>",
                  SS_acc_double,
		  axsmts);

    SS_install_cf("axis-type",
                  "Variable: Controls axis type\n     Usage: axis-type cartesian | polar | insel",
                  SS_acc_int,
		  axsty);

    SS_install_cf("axis-x-format",
                  "Variable: Controls format for x axis labels\n     Usage: axis-x-format <format>",
                  SS_acc_ptr,
                  axslxf);

    SS_install_cf("axis-y-format",
                  "Variable: Controls format for y axis labels\n     Usage: axis-y-format <format>",
                  SS_acc_ptr,
                  axslyf);

/* BCD */

    SS_install_cf("background-color-flag",
                  "Variable: Background color - white or black\n     Usage: background-color-flag black | white",
                  SS_acc_int,
                  &SX_background_color_white);

    SS_install_cf("border-width",
                  "Variable: Window border width in pixels\n     Usage: border-width <integer>",
                  SS_acc_int,
                  &SX_border_width);

    SS_install_cf("botspace",
                  "Variable: Fractional space at bottom of screen\n     Usage: botspace <real>",
                  SS_acc_double,
                  &SX_gpad[2]);

    SS_install_cf("bracket-flag",
                  "Variable: Remove blanks within square bracket enclosed fields\n     Usage: bracket-flag on | off",
                  SS_acc_int,
                  &SS_bracket_flag);

    SS_install_cf("chi",
                  "Variable: Default chi view angle\n     Usage: chi <real>",
                  SS_acc_double,
                  &SX_chi);

    SS_install_cf("console-type",
                  "Variable: Controls console mode\n     Usage: console-type <string>",
                  SS_acc_ptr,
                  &SX_console_type);

    SS_install_cf("console-origin-x",
                  "Variable: X comp of console origin (frac of screen width)\n     Usage: console-origin-x <real>",
                  SS_acc_double,
                  &SX_console_x);

    SS_install_cf("console-origin-y",
                  "Variable: Y comp of console origin (frac of screen width)\n     Usage: console-origin-y <real>",
                  SS_acc_double,
                  &SX_console_y);

    SS_install_cf("console-width",
                  "Variable: Console width in fraction of screen width\n     Usage: console-width <real>",
                  SS_acc_double,
                  &SX_console_width);

    SS_install_cf("console-height",
                  "Variable: Console height in fraction of screen width\n     Usage: console-height <real>",
                  SS_acc_double,
                  &SX_console_height);

    SS_install_cf("contour-n-levels",
                  "Variable: Default number of contour levels to plot\n     Usage: contour-n-levels <int>",
                  SS_acc_int,
                  nlev);

    SS_install_cf("contour-ratio",
                  "Variable: Default ratio for conntour levels spacing\n     Usage: contour-ratio <real>",
                  SS_acc_double,
                  cntrat);

    SS_install_cf("data-directory",
                  "Variable: Sets the default data directory\n     Usage: data-directory <path-name>",
                  SS_acc_string,
                  SX_data_directory);

    SS_install_cf("data-id",
                  "Variable: Controls display of curve identifiers on graph\n     Usage: data-id on | off",
                  SS_acc_int,
                  &SX_data_id);

    SS_install_cf("default-color",
                  "Variable: The default line color\n     Usage: default-color <color>",
                  SS_acc_int,
                  &SX_default_color);

    SS_install_cf("default-npts",
                  "Variable: The default number of points used to make new curves.\n     Used by SPAN, LINE, etc.",
                  SS_acc_int,
                  &SX_default_npts);

    SS_install_cf("display-name",
                  "Variable: Display-name - <host>:<display>.<screen>\n     Usage: display-name <string>",
                  SS_acc_ptr,
                  &SX_display_name);

    SS_install_cf("display-type",
                  "Variable: Controls display mode\n     Usage: display-type <string>",
                  SS_acc_ptr,
                  &SX_display_type);

    SS_install_cf("display-title",
                  "Variable: Controls display title\n     Usage: display-title <string>",
                  SS_acc_ptr,
                  &SX_display_title);

/* EFGHIJ */

    SS_install_cf("error-bar-cap-size",
                  "Variable: Fractional size of error bar caps\n     Usage: error-bar-cap-size <real>",
                  SS_acc_double,
                  errcsz);

    SS_install_cf("file-exist-action",
                  "Variable: Control action when opening existing files for append\n     Usage: file-exist-action <integer>",
                  SS_acc_int,
                  (int *) &SX_file_exist_action);

    SS_install_cf("graphics-mode",
                  "Variable: Graphics mode\n     Usage: graphics-mode [on|off]",
                  SS_acc_int,
                  &SX_gr_mode);

    SS_install_cf("gri-title",
                  "Variable: Graphical interface window title\n     Usage: gri-title <string>",
                  SS_acc_ptr,
                  &SX_GRI_type_face);

    SS_install_cf("gri-type-face",
                  "Variable: Graphical interface type face\n     Usage: gri-type-face medium | italic | bold | bold-italic",
                  SS_acc_ptr,
                  &SX_GRI_type_face);

    SS_install_cf("gri-type-size",
                  "Variable: Graphical interface type size\n     Usage: gri-type-size [<integer>]",
                  SS_acc_int,
                  &SX_GRI_type_size);

    SS_install_cf("gri-type-style",
                  "Variable: Graphical interface type style\n     Usage: gri-type-style medium | italic | bold | bold-italic",
                  SS_acc_ptr,
                  &SX_GRI_type_style);

    SS_install_cf("gri-x",
                  "Variable: Graphical interface window x origin\n     Usage: gri-x <real>",
                  SS_acc_double,
                  &SX_GRI_x);

    SS_install_cf("gri-y",
                  "Variable: Graphical interface window y origin\n     Usage: gri-y <real>",
                  SS_acc_double,
                  &SX_GRI_y);

    SS_install_cf("gri-width",
                  "Variable: Graphical interface window width\n     Usage: gri-width <real>",
                  SS_acc_double,
                  &SX_GRI_dx);

    SS_install_cf("gri-height",
                  "Variable: Graphical interface window height\n     Usage: gri-height <real>",
                  SS_acc_double,
                  &SX_GRI_dy);

    SS_install_cf("grid",
                  "Variable: Controls display of grid lines in graph\n     Usage: grid on | off",
                  SS_acc_int,
                  &SX_grid);

    SS_install_cf("hide-rescale",
                  "Variable: Allows plot to rescale when hiding curves\n     Usage: hide-rescale on | off",
                  SS_acc_int,
                  hider);

    SS_install_cf("histogram-start",
                  "Variable: Starting point for histogram plots\n     Usage: histogram-start left | center | right | off",
                  SS_acc_int,
                  hsts);

    SS_install_cf("interpolation-flag",
                  "Variable: Control going from zone centered to node centered mappings\n     Usage: interpolation-flag on | off",
                  SS_acc_int,
                  intf);

    SS_install_cf("interpolation-power",
                  "Variable: exponent which defines distance measure used in interpolation\n     Usage: interpolation-power #",
                  SS_acc_double,
                  &SX_interp_power);

    SS_install_cf("interpolation-scale",
                  "Variable: multiplier on scale ratio used in interpolation\n     Usage: interpolation-scale #",
                  SS_acc_double,
                  &SX_interp_scale);

    SS_install_cf("interpolation-strength",
                  "Variable: power of distance used in interpolation\n     Usage: interpolation-strength #",
                  SS_acc_double,
                  &SX_interp_strength);

/* KLMN */
    SS_install_cf("labels",
                  "Variable: Controls drawing plot labels\n    Usage: labels on | off ",
                  SS_acc_int,
		  pltlb);

    SS_install_cf("label-color-flag",
                  "Variable: When TRUE print curve label same color as curve",
                  SS_acc_int,
                  labcf);

    SS_install_cf("label-length",
                  "Variable: The length of the label shown by lst et al\n     Usage: label-length <integer> ",
                  SS_acc_int,
		  labln);

    SS_install_cf("label-space",
                  "Variable: Fractional space for curve labels\n     Usage: label-space <real>",
                  SS_acc_double,
                  labsp);

    SS_install_cf("label-type-size",
                  "Variable: Label type size for plot\n     Usage: label-type-size [<integer>]",
                  SS_acc_int,
                  labts);

    SS_install_cf("label-yoffset",
                  "Variable: Fractional offset to start of curve labels\n     Usage: label-yoffset <real>",
                  SS_acc_double,
		  labyo);

    SS_install_cf("leftspace",
                  "Variable: Fractional space at left of screen\n     Usage: leftspace <real>",
                  SS_acc_double,
                  &SX_gpad[0]);

    SS_install_cf("line-color",
                  "Variable: Default line color for new curves\n     Usage: line-color [<integer>] ",
                   SS_acc_int,
                   lnclr);

    SS_install_cf("line-style",
                  "Variable: Default line style for new curves\n     Usage: line-style [<integer>] ",
                   SS_acc_int,
                   lnsty);

    SS_install_cf("line-width",
                  "Variable: Default line width for new curves\n     Usage: line-width [<real>] ",
                   SS_acc_double,
                   lnwid);

    SS_install_cf("logical-plot",
                  "Variable: Controls plotting ranges vs indexes\n     Usage: logical-plot <on | off>",
                  SS_acc_int,
                  lgcp);

    SS_install_cf("map-color-to-extrema",
                  "Variable: Map out of bounds values to max or min\n  Usage: map-color-to-extrema <on | off>",
                  SS_acc_int,
		  cmtex);

    SS_install_cf("marker-index",
                  "Variable: Reference the nth marker\n     Usage: marker-index <integer>",
                  SS_acc_int,
                  mrki);

    SS_install_cf("marker-scale",
                  "Variable: The marker scale factor\n     Usage: marker-scale <real>",
                  SS_acc_double,
                  mrks);

    SS_install_cf("marker-orientation",
                  "Variable: The marker orientation angle\n     Usage: marker-orientation <real>",
                  SS_acc_double,
                  &SX_marker_orientation);

    SS_install_cf("n-curves",
                  "Variable: Total number of curves in the system\n     Usage: n-curves",
                  SS_acc_int,
                  &SX_N_Curves);

    SS_install_cf("n-curves-read",
                  "Variable: Total number of curves read from files\n     Usage: n-curves-read",
                  SS_acc_int,
                  &SX_n_curves_read);

    SS_install_cf("numeric-data-ids",
                  "Variable: Flag for drawing numeric data ids\n     Usage: numeric-data-ids",
                  SS_acc_int,
                  ndi);


/* OPQR */

    SS_install_cf("palette-orientation",
                  "Variable: Default palette orientation\n     Usage: palette-orientation horizontal | vertical",
                  SS_acc_int,
                  palor);

    SS_install_cf("phi",
                  "Variable: Default phi view angle\n     Usage: phi <real>",
                  SS_acc_double,
                  &SX_phi);

    SS_install_cf("plot-date",
                  "Variable: Print date and time on hardcopy if TRUE\n     Usage: plot-date on | off",
                  SS_acc_int,
		  pltdt);

    SS_install_cf("plot-flag",
                  "Variable: Flag that controls whether or not a handler plots\n     Usage: plot-flag on | off",
                  SS_acc_int,
                  &SX_plot_flag);

    SS_install_cf("plot-labels",
                  "Variable: Print curve labels if TRUE\n     Usage: plot-labels on | off",
                  SS_acc_int,
                  pltlb);

    SS_install_cf("plot-title",
                  "Variable: Print plot title if TRUE\n     Usage: plot-title on | off",
                  SS_acc_int,
		  plttl);

    SS_install_cf("plot-type",
                  "Variable: Controls plot type\n     Usage: plot-type cartesian | polar | insel",
                  SS_acc_int,
                  ppty);

    SS_install_cf("print-flag",
                  "Variable: Controls the interpreter value output\n     Usage: print-flag on | off",
                  SS_acc_int,
                  &SS_print_flag);

    SS_install_cf("print-stats",
                  "Variable: Controls the interpreter statistics output\n     Usage: print-flag on | off",
                  SS_acc_int,
                  &SS_stat_flag);

    SS_install_cf("prompt",
                  "Variable: The prompt\n     Usage: prompt <string>",
                  SS_acc_string,
                  SS_prompt);

    SS_install_cf("promotion-type",
                  "Variable: Data type for promotion of sets and arrays (default none)\n     Usage: promotion-type <string>",
                  SS_acc_ptr,
                  &SX_promotion_type);

    SS_install_cf("ps-dots-inch",
                  "Variable: PostScript dots/inch for 8.5 x 11 page\n     Usage: ps-dots-inch <real>",
                  SS_acc_double,
                  psdpi);

    SS_install_cf("ref-mesh",
                  "Variable: Controls drawing a reference mesh\n     Usage: ref-mesh on | off ",
                  SS_acc_int,
                  refm);

    SS_install_cf("ref-mesh-color",
                  "Variable: Controls line color of a reference mesh\n     Usage: ref-mesh <int>",
                  SS_acc_int,
                  refmc);

    SS_install_cf("render-1d-1d",
                  "Variable: Render 1D vs 1D plots\n     Usage: render-1d-1d <int>",
                  SS_acc_int,
                  &SX_render_1d_1d);

    SS_install_cf("render-2d-1d",
                  "Variable: Render 2D vs 1D plots\n     Usage: render-2d-1d <int>",
                  SS_acc_int,
                  &SX_render_2d_1d);

    SS_install_cf("render-2d-2d",
                  "Variable: Render 2D vs 2D plots\n     Usage: render-2d-2d <int>",
                  SS_acc_int,
                  &SX_render_2d_2d);

    SS_install_cf("render-3d-1d",
                  "Variable: Render 3D vs 1D plots\n     Usage: render-3d-1d <int>",
                  SS_acc_int,
                  &SX_render_3d_1d);

    SS_install_cf("restore-viewport",
                  "Variable: Restore viewport after plots\n     Usage: restore-viewport <int>",
                  SS_acc_int,
                  rstvp);

    SS_install_cf("rightspace",
                  "Variable: Fractional space at right of screen\n     Usage: rightspace <real>",
                  SS_acc_double,
                  &SX_gpad[1]);


/* STUV */

    SS_install_cf("scatter-plot",
                  "Variable: Controls scatter plotting\n     Usage: scatter-plot on | off",
                  SS_acc_int,
                  scat);

    SS_install_cf("show-mouse-location",
                  "Variable: Controls display of mouse posision\n     Usage: show-mouse-location on | off",
                  SS_acc_int,
                  &SX_show_mouse_location);

    SS_install_cf("show-mouse-location-x",
                  "Variable: Controls location of mouse posision display\n     Usage: show-mouse-location-x <real>",
                  SS_acc_double,
                  &SX_show_mouse_location_x);

    SS_install_cf("show-mouse-location-y",
                  "Variable: Controls location of mouse posision display\n     Usage: show-mouse-location-y <real>",
                  SS_acc_double,
                  &SX_show_mouse_location_y);

    SS_install_cf("smooth-method",
                  "Variable: Method for smooth functions\n     Usage: smooth-method \"fft\" | \"averaging\" | \"tchebyshev\" | \"least-sqr\"",
                  SS_acc_ptr,
                  &SX_smooth_method);

    SS_install_cf("squeeze-labels",
                  "Variable: Controls squeezing of extra blanks from labels\n     Usage: squeeze-labels on | off",
                  SS_acc_int,
                  sqzlab);

    SS_install_cf("text-color",
                  "Variable: Default text color\n     Usage: text-color [<integer>] ",
                   SS_acc_int,
                   txclr);

    SS_install_cf("text-format",
                  "Variable: Controls format for various text\n     Usage: text-format <format>",
                  SS_acc_ptr,
		  txtfm);

    SS_install_cf("theta",
                  "Variable: Default theta view angle\n     Usage: theta <real>",
                  SS_acc_double,
                  &SX_theta);

    SS_install_cf("topspace",
                  "Variable: Fractional space at top of screen\n     Usage: topspace <real>",
                  SS_acc_double,
                  &SX_gpad[3]);

    SS_install_cf("type-face",
                  "Variable: Font type face for plot\n     Usage: type-face helvetica | times | courier",
                  SS_acc_ptr,
                  axstf);

    SS_install_cf("type-size",
                  "Variable: Font type size for plot\n     Usage: type-size <integer>",
                  SS_acc_int,
                  &SX_plot_type_size);

    SS_install_cf("type-style",
                  "Variable: Font type style for plot\n     Usage: type-style medium | italic | bold | bold-italic",
                  SS_acc_ptr,
                  &SX_plot_type_style);

    SS_install_cf("view-aspect",
                  "Variable: Viewport aspect ratio\n     Usage: view-aspect <real>",
                  SS_acc_double,
                  &SX_view_aspect);

    SS_install_cf("view-height",
                  "Variable: Viewport height in fraction of screen height\n     Usage: view-height <real>",
                  SS_acc_double,
                  &SX_view_height);

    SS_install_cf("view-origin-x",
                  "Variable: X comp of viewport origin (frac of window width)\n     Usage: view-origin-x <real>",
                  SS_acc_double,
                  &SX_view_x[0]);

    SS_install_cf("view-origin-y",
                  "Variable: Y comp of viewport origin (frac of window width)\n     Usage: view-origin-y <real>",
                  SS_acc_double,
                  &SX_view_x[2]);

    SS_install_cf("view-width",
                  "Variable: Viewport width in fraction of screen width\n     Usage: view-width <real>",
                  SS_acc_double,
                  &SX_view_width);


/* UVWXYZ */

    SS_install_cf("window-height",
                  "Variable: Window height in fraction of screen width\n     Usage: window-height <real>",
                  SS_acc_double,
                  &SX_window_height);

    SS_install_cf("window-origin-x",
                  "Variable: X comp of window origin (frac of screen width)\n     Usage: window-origin-x <real>",
                  SS_acc_double,
                  &SX_window_x[0]);

    SS_install_cf("window-origin-y",
                  "Variable: Y comp of window origin (frac of screen width)\n     Usage: window-origin-y <real>",
                  SS_acc_double,
                  &SX_window_x[1]);

    SS_install_cf("window-width",
                  "Variable: Window width in fraction of screen width\n     Usage: window-width <real>",
                  SS_acc_double,
                  &SX_window_width);

    SS_install_cf("x-log-scale",
                  "Variable: Controls log scale on x axis\n     Usage: x-log-scale on | off",
                  SS_acc_int,
                  &SX_log_scale[0]);

    SS_install_cf("y-log-scale",
                  "Variable: Controls log scale on y axis\n     Usage: y-log-scale on | off",
                  SS_acc_int,
                  &SX_log_scale[1]);

    SX_register_devices();
    SX_install_device_vars();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
