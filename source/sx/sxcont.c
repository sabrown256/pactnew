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

typedef void *(*PFPVPsiObj)(SS_psides *si, object *o);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_ARGS - get a C level data item from a single Scheme object */

void _SX_args(SS_psides *si, object *obj, void *v, SC_type *td)
   {int type;
    void **pv;
    pcons *alst;
    PFPVPsiObj f;

    pv = (void **) v;
    if (pv != NULL)
       {type = td->id;
	if (SS_nullobjp(obj) && (type != G_SX_FILE_I))
	   *pv = NULL;

	else
	   {alst = td->a;
	    f    = (PFPVPsiObj) SC_assoc(alst, "Scheme->C");
	    if (f != NULL)
	       *pv = f(si, obj);

/* G_SX_FILE_I is for any file object wrapped in a SX_file */
	    else if (type == G_SX_FILE_I)
	       {if (!SS_nullobjp(obj) && !SX_FILEP(obj))
		   SS_error(si, "OBJECT NOT FILE - _SX_ARGS", obj);

		if (SS_nullobjp(obj))
		   *pv = (void *) SX_gs.gvif;
		else
		   *pv = (void *) SS_GET(SX_file, obj);}

	    else if (type == G_SX_PDBDATA_I)
	       {if (!SX_PDBDATAP(obj))
		   SS_error(si, "NOT PDBDATA OBJECT - _SX_ARGS", obj);

		*pv = (void *) SS_GET(SX_pdbdata, obj);}

	    else if (obj->val != NULL)
	       *pv = obj->val;

	    else
	       SS_error(si, "BAD TYPE - _SX_ARGS",
			SS_mk_integer(si, type));};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_CALL_ARGS - make a SCHEME level object from a C level one */

object *_SX_call_args(SS_psides *si, SC_type *td, void *v)
   {int type;
    object *obj;
    object *(*f)(SS_psides *si, void *a);

    obj  = SS_null;
    type = td->id;

    SS_get_type_method(td, "C->Scheme", &f, NULL);
    if (f != NULL)
       obj = f(si, v);

    else if (type == G_SX_FILE_I)
       {if (v == NULL)
	   obj = SX_gs.ovif;
	else
	   obj = SX_mk_gfile(si, (SX_file *) v);}

    else
       SS_error(si, "BAD TYPE - _SX_CALL_ARGS",
		SS_mk_integer(si, type));

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_LIST_VOBJECTS - list the items from the specified list matching the
 *                   - given pattern
 */

static object *_SX_list_vobjects(SS_psides *si, char *patt,
				 SX_file *po, int type)
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
		 SS_assign(si, ret, SS_mk_cons(si,
					       SS_mk_integer(si, i+1),
					       ret));}};

	SS_assign(si, ret, SS_reverse(si, ret));};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_GENERIC - handle BLANG binding related operations
 *                 - for a generic object if there is no
 *                 - specific method for its type
 */

void *_SX_opt_generic(void *x, bind_opt wh, void *a)
   {int ity;
    void *rv;
    object *o;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o   = (object *) a;
	     ity = SS_OBJECT_TYPE(o);
             if (SS_nullobjp(o))
                rv = NULL;

/* GOTCHA: figure out reasonable limits here */
	     else if ((0 < ity) && (ity < 100))
	        rv = SS_GET(void, o);
	     else
	        rv = _SX.unresolved;
	     break;

	case BIND_LABEL :
        case BIND_PRINT :
        case BIND_ALLOC :
        case BIND_FREE :
        default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MENU - print a menu of images, curves, and graphs */

object *_SXI_menu(SS_psides *si, object *argl)
   {char *patt, *type;
    SX_file *po;
    object *ret;

    ret = SS_null;

    po   = NULL;
    patt = NULL;
    type = NULL;
    SS_args(si, argl,
            G_SX_FILE_I, &po,
            G_STRING_I, &patt,
            G_STRING_I, &type,
            0);

    if (strcmp(po->type, G_PDBFILE_S) != 0)
       return(ret);

    _SX_get_menu(si, po);

    if (type != NULL)
       ret = _SX_list_vobjects(si, patt, po, type[0]);
    else
       ret = _SX_list_vobjects(si, patt, po, 0);

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
        {

#pragma omp parallel for private(j, temp)
	 for (i = gap; i < n; i++)
             {for (j = i-gap; j >= 0; j -= gap)
                  {if (strcmp(a[j].label, a[j+gap].label) <= 0)
		      break;
		   temp     = a[j];
		   a[j]     = a[j+gap];
		   a[j+gap] = temp;};};

        };

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GET_MENU - make lists of syments for the mappings, images, and curves 
 *              - in the current directory of the given file
 */

void _SX_get_menu(SS_psides *si, SX_file *po)
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
    if (names != NULL)
       {for (i = 0; i < n; i++)
	    _SX_push_menu_item(si, po, names[i], G_PM_MAPPING_P_S);
	CFREE(names);};

    names = PD_ls(file, dir, "PG_image *", &n);
    if (names != NULL)
       {for (i = 0; i < n; i++)
	    _SX_push_menu_item(si, po, names[i], G_PG_IMAGE_P_S);
	CFREE(names);};

    names = PD_ls(file, dir, G_CHAR_S, &n);
    if (names != NULL)
       {for (i = 0; i < n; i++)
	    if (SC_regx_match(names[i], "curve????"))
	       _SX_push_menu_item(si, po, names[i], G_PG_CURVE_S);
	CFREE(names);};

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

void _SX_push_menu_item(SS_psides *si, SX_file *po, char *name, char *type)
   {int nc;
    char s[MAXLINE];
    char *var, *lb;
    PDBfile *file;
    SX_menu_item mitem;

    file = FILE_FILE(PDBfile, po);

    mitem.type = type;

    SC_strncpy(s, MAXLINE, _PD_fixname(file, name), -1);
    mitem.vname = CSTRSAVE(s);

/* get mapping label */
    if (strcmp(type, G_PM_MAPPING_P_S) == 0)
       {var = SC_dsnprintf(FALSE, "%s[%d].name", s, file->default_offset);
        if (!PD_read(file, var, &lb))
 	   SS_error(si, "FAILED TO READ LABEL - _SX_PUSH_MENU_ITEM",
		      SS_null);

	mitem.label = CSTRSAVE(lb);

	SC_array_push(po->menu_lst, &mitem);}

/* get curve label */
    else if (strcmp(type, G_PG_CURVE_S) == 0)
       {char *u, t[MAXLINE], *pt;

        if (!PD_read(file, s, t))
           SS_error(si, "FAILED TO READ CURVE HEADER - _SX_PUSH_MENU_ITEM",
		      SS_null);

/* extract the name of the label variable */
        lb = SC_strtok(t, "|", pt);
        if (lb == NULL)
           SS_error(si, "BAD CURVE HEADER - _SX_PUSH_MENU_ITEM", SS_null);

/* get the label */
        else if (PD_has_directories(file))
           {u = strrchr(s, '/');
	    if (u == NULL)
	       {u  = s;
		nc = MAXLINE;}
	    else
	       {u++;
		nc = MAXLINE - (u - s);};
            SC_strncpy(u, nc, lb, -1);}

        else
           SC_strncpy(s, MAXLINE, lb, -1);

        if (!PD_read(file, s, t))
           SS_error(si, "FAILED TO READ LABEL - _SX_PUSH_MENU_ITEM",
		      SS_null);
        mitem.label = CSTRSAVE(t);

	SC_array_push(po->menu_lst, &mitem);}

/* get image label */
    else if (strcmp(type, G_PG_IMAGE_P_S) == 0)
       {var = SC_dsnprintf(FALSE, "%s[%d].label", s, file->default_offset);
        if (!PD_read(file, var, &lb))
           SS_error(si, "FAILED TO READ LABEL - _SX_PUSH_MENU_ITEM",
		      SS_null);
	mitem.label = CSTRSAVE(lb);

	SC_array_push(po->menu_lst, &mitem);}

    else
       SS_error(si, "BAD OBJECT TYPE - _SX_PUSH_MENU_ITEM", SS_null);

    return;}

/*--------------------------------------------------------------------------*/

/*                     INPUT REPROCESSING ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _SX_ISODD - return TRUE iff the given number is odd */

int _SX_isodd(int n)
   {int rv;

    rv = (2*(n/2) == n) ? FALSE : TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_NO_ARGSP - return TRUE iff the object is a procedure object which
 *              - may have zero arguments
 */

int _SX_no_argsp(SS_psides *si, object *obj)
   {int ret;
    object *params;
    PFPHand hand;

    ret = FALSE;

    switch (SS_PROCEDURE_TYPE(obj))
       {case SS_MACRO_EV : 
        case SS_MACRO    : 
        case SS_PROC     :
	     params = SS_params(si, obj);
	     ret    = (!SS_consp(params) || (params == SS_null));
	     break;

        default :
	     hand = SS_C_PROCEDURE_HANDLER(obj);
	     ret  = (hand == SS_zargs)      ||
	            (hand == SS_znargs)     ||
		    (hand == SS_acc_char)   ||
		    (hand == SS_acc_int)    ||
		    (hand == SS_acc_long)   ||
		    (hand == SS_acc_double) ||
		    (hand == SS_acc_string) ||
		    (hand == SS_acc_ptr);
	     break;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PARSE - determine whether or not to reprocess the input for SX
 *          - this is the real worker for the si->post_eval
 *          - since this si->evobj is not the same as in SS_REPL
 *          - it should be SS_mark'd as being an additional pointer to its
 *          - respective object
 */

void SX_parse(SX_reparsed *pd, object *strm)
   {int i, nl;
    char s[MAXLINE];
    char *t, *ptr, **sa;
    SS_psides *si;

    si = pd->si;
    
    if (SS_procedurep(si->evobj))
       {SC_strncpy(s, MAXLINE, SS_PP(si->evobj, name), -1);
        if (_SX_no_argsp(si, si->evobj) || !EOE(strm))
           {ptr = SS_BUFFER(strm);
            if (_SX_isodd(SC_char_count(ptr, '\"')))
               {PRINT(stdout, "\nUNMATCHED QUOTE: %s\n\n", ptr);
                *SS_BUFFER(strm) = '\0';
                SS_PTR(strm) = SS_BUFFER(strm);}
            else
	       {sa = PS_tokenize(ptr, ";\n", 0);
		nl = PS_lst_length(sa);
		for (i = 0; i < nl; i++)
		    {t = pd->reproc(pd, sa[i]);
		     if (t != NULL)
		        {SC_strncpy(ptr, MAXLINE, t, -1);
			 SS_PTR(strm) = SS_BUFFER(strm);
			 SS_assign(si, si->rdobj, SS_read(si, strm));
			 si->interactive = ON;
			 SX_gs.plot_flag = TRUE;
			 SS_assign(si, si->evobj, SS_eval(si, si->rdobj));
			 si->interactive = OFF;};};
		    
		PS_free_strings(sa);

                if (SX_gs.plot_flag && (strcmp(s, "replot") != 0) &&
                    (SX_gs.autoplot == ON) && (pd->replot != NULL))
                   pd->replot(si);};};};

    if (PG_gs.console != NULL)
       PG_gs.console->gprint_flag = TRUE;

    for (t = SS_PTR(strm); *t == '\n'; t++);
    SS_PTR(strm) = t;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_WRAP_PAREN - enclose second arg by first and third args */

char *SX_wrap_paren(char *opn, char *form, char *cls, size_t ln)
   {char tmp[MAXLINE];

   SC_strncpy(tmp, MAXLINE, form, -1);

   snprintf(form, ln, "%s%s%s", opn, tmp, cls);

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

static char *_SX_expand_first(char *d, int nb, char *sp)
   {int np;
    char s1[MAXLINE], s2[MAXLINE], t[MAXLINE];
    char *tp, *lp, *rp;

    *sp = '\0';

/* break the portion of the command before the colon into two strings:
 * the expression immediately preceding the colon (lp) and the rest.
 */
    SC_strncpy(s1, MAXLINE, d, -1);
    for (tp = s1 + (sp - d) - 1; *tp == ' '; tp--);

    if (*tp == ')')
       {tp[2] = '\0';
        tp[1] = ')';
        for (np = 1; np != 0; tp--)
            {*tp = *(tp - 1);
             if (*tp == ')')
                np++;
             else if (*tp == '(')
                np--;}
        *tp = '\0';
        lp = tp + 1;}
    else
       lp = SC_lasttok(s1, " ,");

/* break the portion of the command after the colon into two strings:
 * the expression immediately following the colon (rp) and the rest.
 */
    for (tp = sp + 1; *tp == ' '; tp++);
    SC_strncpy(s2, MAXLINE, tp, -1);

    if (*tp == '(')
       {rp = tp++;
        SC_strncpy(t, MAXLINE, rp, -1);
        rp = t;
        for (np = 1; np != 0; tp++)
            {if (*tp == '(')
                np++;
             else if (*tp == ')')
                np--;}
        SC_strncpy(s2, MAXLINE, tp, -1);
        *tp = '\0';}
    else
       rp = SC_firsttok(s2, " ,");

/* build a new expanded command from all the parts.
 * removing the blank after the ')' causes multiple ranges to give an error.
 * the blank before the '(' improves the aesthetics of command log output.
 */
    snprintf(d, nb, "%s (thru %s %s) %s", s1, lp, rp, s2);

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_EXPAND_EXPR - expand object lists of the form x:y 
 *                - which means all object from x through y inclusive
 */

int SX_expand_expr(char *s, int nb)
   {int ret;
    char *sp;

/* something wrong in input if s is NULL */
    if (s == NULL)
       return(FALSE);

    sp = _SX_isthrough(s);

/* if nothing to expand - termination condition for loop */
    if (sp == NULL)
       ret = TRUE;

    else

/* expand the first missing object */
       {s = _SX_expand_first(s, nb, sp);

/* loop through to find any more instances */
        ret = SX_expand_expr(s, nb);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SPLIT_COMMAND - return the next command from the string in
 *                  - the given buffer
 *                  - return FALSE if there are no more commands
 */

int SX_split_command(char *cmd, int nc, char *lst)
   {int nb, rv;
    char *pc, *pl;

    nb = PS_strcpy_tok(cmd, nc, lst, -1, NULL, ";", 0);

    for (pc = lst, pl = pc+nb; (*pc++ = *pl++) != '\0'; );

    rv = (nb != 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*_SX_NO_DEVICE_SUPPORT - handler to support non-existence devices
 *                      - return zero to emulate the existence flag
 *                      - for a device that actually exists
 */

static object *_SX_no_device_support(SS_psides *si)
   {object *rv;

    rv = SS_mk_integer(si, 0);

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

/* _SX_INSTALL_DEVICE_VARS - install the device variables in the symtab */

static void _SX_install_device_vars(SS_psides *si)
   {int i;
    char dupp[MAXLINE];
    char *dname, *name, *doc;
    out_device *out;
    
    for (i = 0; i < N_OUTPUT_DEVICES; i++)
        {out = _SX.output_devices + i;
	 if (out == NULL)
	    continue;
    
	 dname = out->dname;
	 SC_strncpy(dupp, MAXLINE, dname, -1);
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
	     SS_install_cf(si, name, doc, SS_acc_int, &out->active);

	     doc  = SC_dsnprintf(TRUE, "Variable: The background color to use for a %s file",
				 dupp);
	     name = SC_dsnprintf(FALSE, "%s-background-color-flag", dname);
	     SS_install_cf(si, name, doc, SS_acc_int, &out->background_color);

	     doc  = SC_dsnprintf(TRUE, "Variable: %s file name\n     Usage: %s-name <string>",
				 dupp, dname);
	     name = SC_dsnprintf(FALSE, "%s-name", dname);
	     SS_install_cf(si, name, doc, SS_acc_ptr, &out->fname);

             if (strcmp(dupp, "PS") == 0)
	        doc = SC_dsnprintf(TRUE,  "Variable: %s file type (monochrome or color)\n     Usage: %s-type <string>",
				   dupp, dname);
             else
	        doc = SC_dsnprintf(TRUE,  "Variable: %s file type (monochrome or rgb)\n     Usage: %s-type <string>",
				   dupp, dname);
	     name = SC_dsnprintf(FALSE, "%s-type", dname);
	     SS_install_cf(si, name, doc, SS_acc_ptr, &out->type);

	     doc  = SC_dsnprintf(TRUE, "Variable: %s window height in fraction of screen width\n     Usage: window-height-%s <real>",
				 dupp, dname);
	     name = SC_dsnprintf(FALSE, "window-height-%s", dname);
	     SS_install_cf(si, name, doc, SS_acc_double, &out->dx0[1]);

	     doc  = SC_dsnprintf(TRUE,  "Variable: %s X comp of window origin (frac of screen width)\n     Usage: window-origin-x-%s <real>",
				 dupp, dname);
	     name = SC_dsnprintf(FALSE, "window-origin-x-%s", dname);
	     SS_install_cf(si, name, doc, SS_acc_double, &out->x0[0]);

	     doc  = SC_dsnprintf(TRUE, "Variable: %s Y comp of window origin (frac of screen width)\n     Usage: window-origin-y-%s <real>",
				 dupp, dname);
	     name = SC_dsnprintf(FALSE, "window-origin-y-%s", dname);
	     SS_install_cf(si, name, doc, SS_acc_double, &out->x0[1]);

	     doc  = SC_dsnprintf(TRUE, "Variable: %s window width in fraction of screen width\n     Usage: window-width-%s <real>",
				 dupp, dname);
	     name = SC_dsnprintf(FALSE, "window-width-%s", dname);
	     SS_install_cf(si, name, doc, SS_acc_double, &out->dx0[0]);}

	 else
	    {doc  = SC_dsnprintf(TRUE,  "%s-flag is not supported as configured",
				 dname);
	     name = SC_dsnprintf(FALSE, "%s-flag", dname);
	     SS_install_cf(si, name, doc, SS_zargs,
			   _SX_no_device_support, SS_PR_PROC);

	     doc  = SC_dsnprintf(TRUE,  "%s-name is not supported as configured",
				 dname);
	     name = SC_dsnprintf(FALSE, "%s-name", dname);
	     SS_install_cf(si, name, doc, SS_zargs,
			   _SX_no_device_support, SS_PR_PROC);

	     doc  = SC_dsnprintf(TRUE,  "%s-type is not supported as configured",
				 dname);
	     name = SC_dsnprintf(FALSE, "%s-type", dname);
	     SS_install_cf(si, name, doc, SS_zargs,
			   _SX_no_device_support, SS_PR_PROC);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INIT_DEVICE_VARS - set initial values for device variables */

void SX_init_device_vars(int idev, double *xf, double *dxf)
   {out_device *out;
    
    out = SX_get_device(idev);
    if (out != NULL)
       {out->x0[0]  = xf[0];
	out->x0[1]  = xf[1];
	out->dx0[0] = dxf[0];
	out->dx0[1] = dxf[1];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_REGISTER_DEVICES - register output devices for general use */

void SX_register_devices(void)
   {int i, nd;
    char lnm[MAXLINE], unm[MAXLINE];
    double xf[PG_SPACEDM], dxf[PG_SPACEDM];
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

	 SC_strncpy(lnm, MAXLINE, devs[i], -1);
	 SC_strncpy(unm, MAXLINE, devs[i], -1);
	 SC_str_upper(unm);

	 out->dname = CSTRDUP(lnm, 3);
	 out->dupp  = CSTRDUP(unm, 3);

	 dev = PG_make_device(unm, "COLOR", "tmp");
	 out->exist = (dev != NULL);
	 PG_close_device(dev);

	 if (out->exist)
	    {xf[0] = 0.0;
	     xf[1] = 0.0;
	     if ((strcmp(devs[i], "ps") == 0) ||
		 (strcmp(devs[i], "cgm") == 0))
	        {dxf[0] = 0.0;
		 dxf[1] = 0.0;}
	     else
	        {dxf[0] = 512.0;
		 dxf[1] = 512.0;};
	     SX_init_device_vars(i, xf, dxf);};

	 out->background_color = -1;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_LOAD_RC - load runtime file FFN and
 *            - one of the init files IFNA or IFNB
 */

void SX_load_rc(SS_psides *si, char *ffn, int ldrc, char *ifna, char *ifnb)
   {int st;

/* figure out the runtime file */
    st = SS_load_scm(si, ffn);
    if (st == FALSE)
       {PRINT(stderr, "Can't find %s\n", ffn);
	PRINT(stderr, "Check SCHEME environment variable\n\n");
	SX_quit(ABORT);};

/* figure out the init file */
    if (ldrc == TRUE)
       {st = SS_load_scm(si, ifna);
	if (st == FALSE)
	   {st = SS_load_scm(si, ifnb);
	    if (st == FALSE)
	       {PRINT(stderr, "Can't find %s or %s\n", ifna, ifnb);
		PRINT(stderr, "Check SCHEME environment variable\n\n");
		SX_quit(ABORT);};};};

    return;}

/*--------------------------------------------------------------------------*/

/*                         INSTALLATION ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* SX_INSTALL_GLOBAL_VARS - install the global variables */

void SX_install_global_vars(SS_psides *si)
   {int i;
    int *axslc, *axsls;
    int *sqzlab, *refm, *refmc, *hider;
    int *hsts, *nlev, *scat, *cmtex, *intf, *mrki;
    int *lgcp, *ndi, *pltlb, *plttl, *pltdt, *palor;
    int *labcf, *labts, *labln, *lnclr, *lnsty, *txclr;
    int *rstvp;
    double *axsca, *axscs, *axsgs, *axslb;
    double *axslw, *axsmts, *axsmjt, *axsmnt, *axndec;
    double *axsmxt, *axsmyt, *axson, *axstt, *axsty;
    double *cntrat, *psdpi, *lnwid;
    double *errcsz, *labsp, *labyo, *mrks;
    char **axslxf, **axslyf, **axstf, **txtfm;
    PG_rendering *ppty;

/* change default color to rotating */
    PG_set_attrs_glb(TRUE, "line-color", ROTATING, NULL);

    axsca     = PG_ptr_attr_glb("axis-char-angle");
    axscs     = PG_ptr_attr_glb("axis-char-size");
    axsgs     = PG_ptr_attr_glb("axis-grid-style");
    axslb     = PG_ptr_attr_glb("axis-labels");
    axslxf    = PG_ptr_attr_glb("axis-label-x-format");
    axslyf    = PG_ptr_attr_glb("axis-label-y-format");
    axslc     = PG_ptr_attr_glb("axis-line-color");
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

    SX_gs.autodomain             = TRUE;
    SX_gs.autoplot               = ON;
    SX_gs.autorange              = TRUE;
    SX_gs.border_width           = 2;
    SX_gs.view_angle[2]          = 0.0;
    SX_gs.data_id                = TRUE,
    SX_gs.gri_x[0]               = 0.5;
    SX_gs.gri_x[1]               = 0.01;
    SX_gs.gri_dx[0]              = 0.0;
    SX_gs.gri_dx[1]              = 0.0;
    SX_gs.grid                   = OFF;
    SX_gs.marker_orientation     = 0.0;

/*    SX_palette                 = CSTRSAVE("spectrum"); */
    SX_gs.view_angle[1]          = 0.0;
    SX_gs.plot_type_size         = 12;
    SX_gs.plot_type_style        = CSTRSAVE("medium");
    SX_gs.promotion_type         = CSTRSAVE("none");
    SX_gs.render_def             = PLOT_MESH;
    SX_gs.render_1d_1d           = *ppty;
    SX_gs.render_2d_1d           = PLOT_CONTOUR;
    SX_gs.render_2d_2d           = PLOT_VECTOR;
    SX_gs.render_3d_1d           = PLOT_MESH;
    SX_gs.show_mouse_location    = FALSE;
    SX_gs.show_mouse_x[0]        = 0.025;
    SX_gs.show_mouse_x[1]        = 0.955;
    SX_gs.smooth_method          = CSTRSAVE("averaging");
    SX_gs.view_angle[0]          = 0.0;

    SX_gs.gri_title            = CSTRSAVE("PDBView Controls");
    SX_gs.gri_type_face        = CSTRSAVE("helvetica");
    SX_gs.gri_type_style       = CSTRSAVE("medium");
    SX_gs.gri_type_size        = 12;

    si->interactive = FALSE;
    si->print_flag  = FALSE;
    si->stat_flag   = FALSE;

#pragma omp parallel for
    for (i = 0; i < PG_SPACEDM; i++)
        SX_gs.log_scale[i] = FALSE;

/* WC to BND offset */

#pragma omp parallel for
    for (i = 0; i < PG_BOXSZ; i++)
        SX_gs.gpad[i] = 0.01;

/* A */

    SS_install_cf(si, "answer-prompt",
                  "Variable: A string printed before the return value\n     Usage: answer-prompt <string>",
                  SS_acc_string,
                  si->ans_prompt, MAXLINE);

    SS_install_cf(si, "autorange",
                  "Variable: Turns on or off autoranging\n     Usage: autorange on | off",
                  SS_acc_int,
                  &SX_gs.autorange);

    SS_install_cf(si, "autodomain",
                  "Variable: Turns on or off autodomain\n     Usage: autodomain on | off",
                  SS_acc_int,
                  &SX_gs.autodomain);

    SS_install_cf(si, "autoplot",
                  "Variable: Controls autoploting of graphs\n     Usage: autoplot on | off",
                  SS_acc_int,
                  &SX_gs.autoplot);

    SS_install_cf(si, "axis",
                  "Variable: Controls drawing the axes\n     Usage: axis on | off ",
                  SS_acc_int,
		  axson);

    SS_install_cf(si, "axis-max-major-ticks",
                  "Variable: Controls maximum number of major ticks\nUsage: axis-max-major-ticks <integer> ",
                  SS_acc_int,
		  axsmjt);

    SS_install_cf(si, "axis-number-minor-ticks",
                  "Variable: Controls number of minor axis ticks\nUsage: axis-number-minor-ticks <integer> ",
                  SS_acc_int,
		  axsmnt);

    SS_install_cf(si, "axis-number-minor-x-ticks",
                  "Variable: Controls number of minor X axis ticks\nUsage: axis-number-minor-x-ticks <integer> ",
                  SS_acc_int,
		  axsmxt);

    SS_install_cf(si, "axis-number-minor-y-ticks",
                  "Variable: Controls number of minor Y axis ticks\nUsage: axis-number-minor-y-ticks <integer> ",
                  SS_acc_int,
		  axsmyt);

    SS_install_cf(si, "axis-n-decades",
                  "Variable: Controls maximum number of log axis decades\nUsage: axis-n-decades <real> ",
                  SS_acc_double,
                  axndec);

    SS_install_cf(si, "axis-tick-type",
                  "Variable: Controls location of axis ticks\nUsage: axis-tick-type left-of-axis | right-of-axis | straddle-axis ",
                  SS_acc_int,
		  axstt);

    SS_install_cf(si, "axis-char-size",
                  "Variable: Controls character size for the axes\n     Usage: axis-char-size <integer> ",
                  SS_acc_int,
		  axscs);

    SS_install_cf(si, "axis-char-angle",
                  "Variable: Controls angle at which characters are drawn\n     Usage: axis-char-angle <real> ",
                  SS_acc_double,
		  axsca);

    SS_install_cf(si, "axis-grid-style",
                  "Variable: Controls line style for the axis grid\n     Usage: axis-grid-style [solid | dotted | dashed | dotdashed]",
                  SS_acc_int,
		  axsgs);

    SS_install_cf(si, "axis-labels",
                  "Variable: Controls axis labelling\n     Usage: axis-labels on | off",
                  SS_acc_int,
		  axslb);

    SS_install_cf(si, "axis-line-color",
                  "Variable: Controls line color for the axes\n     Usage: axis-line-color [<integer>]",
                  SS_acc_int,
		  axslc);

    SS_install_cf(si, "axis-line-style",
                  "Variable: Controls line style for the axes\n     Usage: axis-line-style [solid | dotted | dashed | dotdashed]",
                  SS_acc_int,
		  axsls);

    SS_install_cf(si, "axis-line-width",
                  "Variable: Controls line width for the axes\n     Usage: axis-line-width [<real>] ",
                   SS_acc_double,
                   axslw);

    SS_install_cf(si, "axis-tick-size",
                  "Variable: Controls major tick size for the axes\n     Usage: axis-tick-size <real>",
                  SS_acc_double,
		  axsmts);

    SS_install_cf(si, "axis-type",
                  "Variable: Controls axis type\n     Usage: axis-type cartesian | polar | insel",
                  SS_acc_int,
		  axsty);

    SS_install_cf(si, "axis-x-format",
                  "Variable: Controls format for x axis labels\n     Usage: axis-x-format <format>",
                  SS_acc_ptr,
                  axslxf);

    SS_install_cf(si, "axis-y-format",
                  "Variable: Controls format for y axis labels\n     Usage: axis-y-format <format>",
                  SS_acc_ptr,
                  axslyf);

/* BCD */

    SS_install_cf(si, "background-color-flag",
                  "Variable: Background color - white or black\n     Usage: background-color-flag black | white",
                  SS_acc_int,
                  &SX_gs.background_color_white);

    SS_install_cf(si, "border-width",
                  "Variable: Window border width in pixels\n     Usage: border-width <integer>",
                  SS_acc_int,
                  &SX_gs.border_width);

    SS_install_cf(si, "botspace",
                  "Variable: Fractional space at bottom of screen\n     Usage: botspace <real>",
                  SS_acc_double,
                  &SX_gs.gpad[2]);

    SS_install_cf(si, "bracket-flag",
                  "Variable: Remove blanks within square bracket enclosed fields\n     Usage: bracket-flag on | off",
                  SS_acc_int,
                  &si->bracket_flag);

    SS_install_cf(si, "chi",
                  "Variable: Default chi view angle\n     Usage: chi <real>",
                  SS_acc_double,
                  &SX_gs.view_angle[2]);

    SS_install_cf(si, "console-type",
                  "Variable: Controls console mode\n     Usage: console-type <string>",
                  SS_acc_ptr,
                  &SX_gs.console_type);

    SS_install_cf(si, "console-origin-x",
                  "Variable: X comp of console origin (frac of screen width)\n     Usage: console-origin-x <real>",
                  SS_acc_double,
                  &SX_gs.console_x[0]);

    SS_install_cf(si, "console-origin-y",
                  "Variable: Y comp of console origin (frac of screen width)\n     Usage: console-origin-y <real>",
                  SS_acc_double,
                  &SX_gs.console_x[1]);

    SS_install_cf(si, "console-width",
                  "Variable: Console width in fraction of screen width\n     Usage: console-width <real>",
                  SS_acc_double,
                  &SX_gs.console_dx[0]);

    SS_install_cf(si, "console-height",
                  "Variable: Console height in fraction of screen width\n     Usage: console-height <real>",
                  SS_acc_double,
                  &SX_gs.console_dx[1]);

    SS_install_cf(si, "contour-n-levels",
                  "Variable: Default number of contour levels to plot\n     Usage: contour-n-levels <int>",
                  SS_acc_int,
                  nlev);

    SS_install_cf(si, "contour-ratio",
                  "Variable: Default ratio for conntour levels spacing\n     Usage: contour-ratio <real>",
                  SS_acc_double,
                  cntrat);

    SS_install_cf(si, "data-directory",
                  "Variable: Sets the default data directory\n     Usage: data-directory <path-name>",
                  SS_acc_string,
                  SX_gs.data_directory, MAXLINE);

    SS_install_cf(si, "data-id",
                  "Variable: Controls display of curve identifiers on graph\n     Usage: data-id on | off",
                  SS_acc_int,
                  &SX_gs.data_id);

    SS_install_cf(si, "default-npts",
                  "Variable: The default number of points used to make new curves.\n     Used by SPAN, LINE, etc.",
                  SS_acc_int,
                  &SX_gs.default_npts);

    SS_install_cf(si, "display-name",
                  "Variable: Display-name - <host>:<display>.<screen>\n     Usage: display-name <string>",
                  SS_acc_ptr,
                  &SX_gs.display_name);

    SS_install_cf(si, "display-type",
                  "Variable: Controls display mode\n     Usage: display-type <string>",
                  SS_acc_ptr,
                  &SX_gs.display_type);

    SS_install_cf(si, "display-title",
                  "Variable: Controls display title\n     Usage: display-title <string>",
                  SS_acc_ptr,
                  &SX_gs.display_title);

/* EFGHIJ */

    SS_install_cf(si, "error-bar-cap-size",
                  "Variable: Fractional size of error bar caps\n     Usage: error-bar-cap-size <real>",
                  SS_acc_double,
                  errcsz);

    SS_install_cf(si, "file-exist-action",
                  "Variable: Control action when opening existing files for append\n     Usage: file-exist-action <integer>",
                  SS_acc_int,
                  (int *) &_SX.file_exist_action);

    SS_install_cf(si, "graphics-mode",
                  "Variable: Graphics mode\n     Usage: graphics-mode [on|off]",
                  SS_acc_int,
                  &SX_gs.gr_mode);

    SS_install_cf(si, "gri-title",
                  "Variable: Graphical interface window title\n     Usage: gri-title <string>",
                  SS_acc_ptr,
                  &SX_gs.gri_type_face);

    SS_install_cf(si, "gri-type-face",
                  "Variable: Graphical interface type face\n     Usage: gri-type-face medium | italic | bold | bold-italic",
                  SS_acc_ptr,
                  &SX_gs.gri_type_face);

    SS_install_cf(si, "gri-type-size",
                  "Variable: Graphical interface type size\n     Usage: gri-type-size [<integer>]",
                  SS_acc_int,
                  &SX_gs.gri_type_size);

    SS_install_cf(si, "gri-type-style",
                  "Variable: Graphical interface type style\n     Usage: gri-type-style medium | italic | bold | bold-italic",
                  SS_acc_ptr,
                  &SX_gs.gri_type_style);

    SS_install_cf(si, "gri-x",
                  "Variable: Graphical interface window x origin\n     Usage: gri-x <real>",
                  SS_acc_double,
                  &SX_gs.gri_x[0]);

    SS_install_cf(si, "gri-y",
                  "Variable: Graphical interface window y origin\n     Usage: gri-y <real>",
                  SS_acc_double,
                  &SX_gs.gri_x[1]);

    SS_install_cf(si, "gri-width",
                  "Variable: Graphical interface window width\n     Usage: gri-width <real>",
                  SS_acc_double,
                  &SX_gs.gri_dx[0]);

    SS_install_cf(si, "gri-height",
                  "Variable: Graphical interface window height\n     Usage: gri-height <real>",
                  SS_acc_double,
                  &SX_gs.gri_dx[1]);

    SS_install_cf(si, "grid",
                  "Variable: Controls display of grid lines in graph\n     Usage: grid on | off",
                  SS_acc_int,
                  &SX_gs.grid);

    SS_install_cf(si, "hide-rescale",
                  "Variable: Allows plot to rescale when hiding curves\n     Usage: hide-rescale on | off",
                  SS_acc_int,
                  hider);

    SS_install_cf(si, "histogram-start",
                  "Variable: Starting point for histogram plots\n     Usage: histogram-start left | center | right | off",
                  SS_acc_int,
                  hsts);

    SS_install_cf(si, "interpolation-flag",
                  "Variable: Control going from zone centered to node centered mappings\n     Usage: interpolation-flag on | off",
                  SS_acc_int,
                  intf);

    SS_install_cf(si, "interpolation-method",
                  "Variable: interpolation method: inverse distance or multi-quadric\n     Usage: interpolation-method idw | mq",
                  SS_acc_int,
                  &SX_gs.interp_method);

    SS_install_cf(si, "interpolation-power",
                  "Variable: exponent which defines distance measure used in interpolation\n     Usage: interpolation-power #",
                  SS_acc_double,
                  &SX_gs.interp_power);

    SS_install_cf(si, "interpolation-scale",
                  "Variable: multiplier on scale ratio used in interpolation\n     Usage: interpolation-scale #",
                  SS_acc_double,
                  &SX_gs.interp_scale);

    SS_install_cf(si, "interpolation-strength",
                  "Variable: power of distance used in interpolation\n     Usage: interpolation-strength #",
                  SS_acc_double,
                  &SX_gs.interp_strength);

    SS_install_cf(si, "interpolation-mq-scale",
                  "Variable: multiplier on scale factor used in MQ interpolation\n     Usage: interpolation-mq-scale #",
                  SS_acc_double,
                  &SX_gs.interp_mq_scale);

/* KLMN */
    SS_install_cf(si, "labels",
                  "Variable: Controls drawing plot labels\n    Usage: labels on | off ",
                  SS_acc_int,
		  pltlb);

    SS_install_cf(si, "label-color-flag",
                  "Variable: When TRUE print curve label same color as curve",
                  SS_acc_int,
                  labcf);

    SS_install_cf(si, "label-length",
                  "Variable: The length of the label shown by lst et al\n     Usage: label-length <integer> ",
                  SS_acc_int,
		  labln);

    SS_install_cf(si, "label-space",
                  "Variable: Fractional space for curve labels\n     Usage: label-space <real>",
                  SS_acc_double,
                  labsp);

    SS_install_cf(si, "label-type-size",
                  "Variable: Label type size for plot\n     Usage: label-type-size [<integer>]",
                  SS_acc_int,
                  labts);

    SS_install_cf(si, "label-yoffset",
                  "Variable: Fractional offset to start of curve labels\n     Usage: label-yoffset <real>",
                  SS_acc_double,
		  labyo);

    SS_install_cf(si, "leftspace",
                  "Variable: Fractional space at left of screen\n     Usage: leftspace <real>",
                  SS_acc_double,
                  &SX_gs.gpad[0]);

    SS_install_cf(si, "line-color",
                  "Variable: Default line color for new curves\n     Usage: line-color [<integer>] ",
                   SS_acc_int,
                   lnclr);

    SS_install_cf(si, "line-style",
                  "Variable: Default line style for new curves\n     Usage: line-style [<integer>] ",
                   SS_acc_int,
                   lnsty);

    SS_install_cf(si, "line-width",
                  "Variable: Default line width for new curves\n     Usage: line-width [<real>] ",
                   SS_acc_double,
                   lnwid);

    SS_install_cf(si, "logical-plot",
                  "Variable: Controls plotting ranges vs indexes\n     Usage: logical-plot <on | off>",
                  SS_acc_int,
                  lgcp);

    SS_install_cf(si, "map-color-to-extrema",
                  "Variable: Map out of bounds values to max or min\n  Usage: map-color-to-extrema <on | off>",
                  SS_acc_int,
		  cmtex);

    SS_install_cf(si, "marker-index",
                  "Variable: Reference the nth marker\n     Usage: marker-index <integer>",
                  SS_acc_int,
                  mrki);

    SS_install_cf(si, "marker-scale",
                  "Variable: The marker scale factor\n     Usage: marker-scale <real>",
                  SS_acc_double,
                  mrks);

    SS_install_cf(si, "marker-orientation",
                  "Variable: The marker orientation angle\n     Usage: marker-orientation <real>",
                  SS_acc_double,
                  &SX_gs.marker_orientation);

    SS_install_cf(si, "n-curves",
                  "Variable: Total number of curves in the system\n     Usage: n-curves",
                  SS_acc_int,
                  &SX_gs.n_curves);

    SS_install_cf(si, "n-curves-read",
                  "Variable: Total number of curves read from files\n     Usage: n-curves-read",
                  SS_acc_int,
                  &SX_gs.n_curves_read);

    SS_install_cf(si, "numeric-data-ids",
                  "Variable: Flag for drawing numeric data ids\n     Usage: numeric-data-ids",
                  SS_acc_int,
                  ndi);


/* OPQR */

    SS_install_cf(si, "palette-orientation",
                  "Variable: Default palette orientation\n     Usage: palette-orientation horizontal | vertical",
                  SS_acc_int,
                  palor);

    SS_install_cf(si, "phi",
                  "Variable: Default phi view angle\n     Usage: phi <real>",
                  SS_acc_double,
                  &SX_gs.view_angle[1]);

    SS_install_cf(si, "plot-date",
                  "Variable: Print date and time on hardcopy if TRUE\n     Usage: plot-date on | off",
                  SS_acc_int,
		  pltdt);

    SS_install_cf(si, "plot-flag",
                  "Variable: Flag that controls whether or not a handler plots\n     Usage: plot-flag on | off",
                  SS_acc_int,
                  &SX_gs.plot_flag);

    SS_install_cf(si, "plot-labels",
                  "Variable: Print curve labels if TRUE\n     Usage: plot-labels on | off",
                  SS_acc_int,
                  pltlb);

    SS_install_cf(si, "plot-title",
                  "Variable: Print plot title if TRUE\n     Usage: plot-title on | off",
                  SS_acc_int,
		  plttl);

    SS_install_cf(si, "plot-type",
                  "Variable: Controls plot type\n     Usage: plot-type cartesian | polar | insel",
                  SS_acc_int,
                  ppty);

    SS_install_cf(si, "print-flag",
                  "Variable: Controls the interpreter value output\n     Usage: print-flag on | off",
                  SS_acc_int,
                  &si->print_flag);

    SS_install_cf(si, "print-stats",
                  "Variable: Controls the interpreter statistics output\n     Usage: print-flag on | off",
                  SS_acc_int,
                  &si->stat_flag);

    SS_install_cf(si, "prompt",
                  "Variable: The prompt\n     Usage: prompt <string>",
                  SS_acc_string,
                  si->prompt, MAXLINE);

    SS_install_cf(si, "promotion-type",
                  "Variable: Data type for promotion of sets and arrays (default none)\n     Usage: promotion-type <string>",
                  SS_acc_ptr,
                  &SX_gs.promotion_type);

    SS_install_cf(si, "ps-dots-inch",
                  "Variable: PostScript dots/inch for 8.5 x 11 page\n     Usage: ps-dots-inch <real>",
                  SS_acc_double,
                  psdpi);

    SS_install_cf(si, "ref-mesh",
                  "Variable: Controls drawing a reference mesh\n     Usage: ref-mesh on | off ",
                  SS_acc_int,
                  refm);

    SS_install_cf(si, "ref-mesh-color",
                  "Variable: Controls line color of a reference mesh\n     Usage: ref-mesh <int>",
                  SS_acc_int,
                  refmc);

    SS_install_cf(si, "render-1d-1d",
                  "Variable: Render 1D vs 1D plots\n     Usage: render-1d-1d <int>",
                  SS_acc_int,
                  &SX_gs.render_1d_1d);

    SS_install_cf(si, "render-2d-1d",
                  "Variable: Render 2D vs 1D plots\n     Usage: render-2d-1d <int>",
                  SS_acc_int,
                  &SX_gs.render_2d_1d);

    SS_install_cf(si, "render-2d-2d",
                  "Variable: Render 2D vs 2D plots\n     Usage: render-2d-2d <int>",
                  SS_acc_int,
                  &SX_gs.render_2d_2d);

    SS_install_cf(si, "render-3d-1d",
                  "Variable: Render 3D vs 1D plots\n     Usage: render-3d-1d <int>",
                  SS_acc_int,
                  &SX_gs.render_3d_1d);

    SS_install_cf(si, "restore-viewport",
                  "Variable: Restore viewport after plots\n     Usage: restore-viewport <int>",
                  SS_acc_int,
                  rstvp);

    SS_install_cf(si, "rightspace",
                  "Variable: Fractional space at right of screen\n     Usage: rightspace <real>",
                  SS_acc_double,
                  &SX_gs.gpad[1]);


/* STUV */

    SS_install_cf(si, "scatter-plot",
                  "Variable: Controls scatter plotting\n     Usage: scatter-plot on | off",
                  SS_acc_int,
                  scat);

    SS_install_cf(si, "show-mouse-location",
                  "Variable: Controls display of mouse posision\n     Usage: show-mouse-location on | off",
                  SS_acc_int,
                  &SX_gs.show_mouse_location);

    SS_install_cf(si, "show-mouse-location-x",
                  "Variable: Controls location of mouse posision display\n     Usage: show-mouse-location-x <real>",
                  SS_acc_double,
                  &SX_gs.show_mouse_x[0]);

    SS_install_cf(si, "show-mouse-location-y",
                  "Variable: Controls location of mouse posision display\n     Usage: show-mouse-location-y <real>",
                  SS_acc_double,
                  &SX_gs.show_mouse_x[1]);

    SS_install_cf(si, "smooth-method",
                  "Variable: Method for smooth functions\n     Usage: smooth-method \"fft\" | \"averaging\" | \"tchebyshev\" | \"least-sqr\"",
                  SS_acc_ptr,
                  &SX_gs.smooth_method);

    SS_install_cf(si, "squeeze-labels",
                  "Variable: Controls squeezing of extra blanks from labels\n     Usage: squeeze-labels on | off",
                  SS_acc_int,
                  sqzlab);

    SS_install_cf(si, "text-color",
                  "Variable: Default text color\n     Usage: text-color [<integer>] ",
                   SS_acc_int,
                   txclr);

    SS_install_cf(si, "text-format",
                  "Variable: Controls format for various text\n     Usage: text-format <format>",
                  SS_acc_ptr,
		  txtfm);

    SS_install_cf(si, "theta",
                  "Variable: Default theta view angle\n     Usage: theta <real>",
                  SS_acc_double,
                  &SX_gs.view_angle[0]);

    SS_install_cf(si, "topspace",
                  "Variable: Fractional space at top of screen\n     Usage: topspace <real>",
                  SS_acc_double,
                  &SX_gs.gpad[3]);

    SS_install_cf(si, "type-face",
                  "Variable: Font type face for plot\n     Usage: type-face helvetica | times | courier",
                  SS_acc_ptr,
                  axstf);

    SS_install_cf(si, "type-size",
                  "Variable: Font type size for plot\n     Usage: type-size <integer>",
                  SS_acc_int,
                  &SX_gs.plot_type_size);

    SS_install_cf(si, "type-style",
                  "Variable: Font type style for plot\n     Usage: type-style medium | italic | bold | bold-italic",
                  SS_acc_ptr,
                  &SX_gs.plot_type_style);

    SS_install_cf(si, "view-aspect",
                  "Variable: Viewport aspect ratio\n     Usage: view-aspect <real>",
                  SS_acc_double,
                  &SX_gs.view_aspect);

    SS_install_cf(si, "view-height",
                  "Variable: Viewport height in fraction of screen height\n     Usage: view-height <real>",
                  SS_acc_double,
                  &SX_gs.view_dx[1]);

    SS_install_cf(si, "view-origin-x",
                  "Variable: X comp of viewport origin (frac of window width)\n     Usage: view-origin-x <real>",
                  SS_acc_double,
                  &SX_gs.view_x[0]);

    SS_install_cf(si, "view-origin-y",
                  "Variable: Y comp of viewport origin (frac of window width)\n     Usage: view-origin-y <real>",
                  SS_acc_double,
                  &SX_gs.view_x[2]);

    SS_install_cf(si, "view-width",
                  "Variable: Viewport width in fraction of screen width\n     Usage: view-width <real>",
                  SS_acc_double,
                  &SX_gs.view_dx[0]);


/* UVWXYZ */

    SS_install_cf(si, "window-height",
                  "Variable: Window height in fraction of screen width\n     Usage: window-height <real>",
                  SS_acc_double,
                  &SX_gs.window_dx[1]);

    SS_install_cf(si, "window-origin-x",
                  "Variable: X comp of window origin (frac of screen width)\n     Usage: window-origin-x <real>",
                  SS_acc_double,
                  &SX_gs.window_x[0]);

    SS_install_cf(si, "window-origin-y",
                  "Variable: Y comp of window origin (frac of screen width)\n     Usage: window-origin-y <real>",
                  SS_acc_double,
                  &SX_gs.window_x[1]);

    SS_install_cf(si, "window-width",
                  "Variable: Window width in fraction of screen width\n     Usage: window-width <real>",
                  SS_acc_double,
                  &SX_gs.window_dx[0]);

    SS_install_cf(si, "x-log-scale",
                  "Variable: Controls log scale on x axis\n     Usage: x-log-scale on | off",
                  SS_acc_int,
                  &SX_gs.log_scale[0]);

    SS_install_cf(si, "y-log-scale",
                  "Variable: Controls log scale on y axis\n     Usage: y-log-scale on | off",
                  SS_acc_int,
                  &SX_gs.log_scale[1]);

    SX_register_devices();
    _SX_install_device_vars(si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
