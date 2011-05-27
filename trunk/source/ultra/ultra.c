/*
 * ULTRA.C - the portable presentation, analysis and manipulation tool
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "ultra.h"
#include "scope_raster.h"
#include "scope_proc.h"

typedef void (*PFReplot)(void);

#ifdef DEBUG

int
 heapch;

#endif

double
 UL_window_height_factor,
 *UL_buf1x,
 *UL_buf1y,
 *UL_buf2x,
 *UL_buf2y;

double
 UL_derivative_tolerance;

object
 *UL_ann_lst,
 *UL_window;

static object
 *crva,
 *crvb;

static char
 _UL_bf[MAXLINE];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_RD_SCM - do a SCHEME level rd with error protection */

static int _UL_rd_scm(SS_psides *si)
   {

    SS_call_scheme(si, "rd",
                   SC_STRING_I, _UL_bf,
                   0);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_RD_SCM - do a SCHEME level rd with error protection */

static int UL_rd_scm(SS_psides *si, char *name)
   {int rv;

    strcpy(_UL_bf, name);

    rv = SS_err_catch(si, _UL_rd_scm, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_INIT_VIEW - initialize the plot parameters */

void UL_init_view(SS_psides *si)
   {int j;

    SX_default_npts      = 100;
    SX_gr_mode           = TRUE;
    SX_plot_flag         = TRUE;

    PG_box_init(3, SX_gwc, 0.0, 1.0);

    SX_view_x[0]       = 0.18;
    SX_view_x[1]       = 0.93;
    SX_view_x[2]       = 0.18;
    SX_view_x[3]       = 0.93;
    SX_view_x[4]       = 0.0;
    SX_view_x[5]       = 1.0;

    SX_view_width        = 0.75;
    SX_view_height       = 0.75;
    SX_view_aspect       = 1.0;
    SX_window_x[0]       = 0.5;
    SX_window_x[1]       = 0.1;
    SX_window_width      = 0.4;
    SX_window_height     = 0.4;
    SX_window_width_P    = 1.0;
    SX_window_height_P   = 1.0;

    SX_console_x         = 0.0;
    SX_console_y         = 0.0;
    SX_console_width     = 0.33;
    SX_console_height    = 0.33;

    SX_console_type     = CSTRSAVE("MONOCHROME");
    SX_text_output_format = CSTRSAVE("%13.6e");

    SX_display_name  = CSTRSAVE("WINDOW");
    SX_display_type  = CSTRSAVE("COLOR");
    SX_display_title = CSTRSAVE("ULTRA II");

    UL_derivative_tolerance = 2.0e-2;
    UL_window_height_factor = 1.0;

    si->interactive = FALSE;
    si->print_flag  = FALSE;
    si->stat_flag   = FALSE;

    SX_command_log_name = CSTRSAVE("ultra.log");

    for (j = 0; j < SX_N_Curves; j++)
        SX_data_index[j] = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_ARGS - get a C level data item from a single Scheme object */

static void _UL_args(SS_psides *si, object *obj, void *v, int type)
   {int *pi, len, i;
    char *s, *cptr;

    switch (type)
       {case UL_CURVE_INDEX_I :
             if (SX_curvep_a(obj))
                {pi  = (int *) v;
                 *pi = SX_get_crv_index_i(obj);}
             else if (SS_integerp(obj))
                {pi = (int *) v;
                 *pi = SX_number[*SS_GET(int, obj)];}
             else
                SS_error(si, "OBJECT NOT CURVE - _UL_ARGS", obj);
             break;

        case UL_CURVE_INDEX_J :
             if (SX_curvep_a(obj))
                {pi  = (int *) v;
                 *pi = SX_get_crv_index_j(obj);}
             else if (SS_integerp(obj))
                {pi = (int *) v;
                 *pi = SX_number[*SS_GET(int, obj)];}
             else
                SS_error(si, "OBJECT NOT CURVE - _UL_ARGS", obj);
             break;

        case UL_DATA_ID_I :
             s   = SS_get_string(obj);
             len = strlen(s);
             cptr = (char *) v;
             for (i = 0; i < len; i++)
                 cptr[i] = (char) toupper((int) s[i]);
             cptr[i] = '\0';
             break;
#if 0
             *(char *) v = (char) toupper((int) *SS_get_string(obj));
             break;
#endif
        default :
             _SX_args(si, obj, v, type);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_EXPAND_PREFIX - expand prefix expressions in referring to menu items
 *                   - Because of possible user defined synonyms, this must
 *                   - be done for every command, not just select. This step
 *                   - could come later and be applied only to number lists,
 *                   - if range expansion were moved to the handlers.
 */

static void _UL_expand_prefix(char *s)
   {int flag, index;
    char t[MAXLINE], token[MAXLINE];
    char *sp, *tp, *rp;

    strcpy(t, s);
    sp  = s;

    while ((tp = SC_firsttokq(t, " \t\n\r","\"")) != NULL)
       {if ((*tp >= 'a') && (*tp <= 'z')
            && (SX_prefix_list[*tp - 'a'] > 0)
            && (*(tp+1) == '.'))
           {if ((rp = strchr(tp+2, ')')))
               *rp = '\0';
            if ((flag = SC_intstrp(tp+2, 10)))
               index = atoi(tp+2);
            if (rp)
               *rp = ')';
            if (flag && rp)
               snprintf(token, MAXLINE, "(pre %c %d))", *tp, index);
            else if (flag)
               snprintf(token, MAXLINE, "(pre %c %d)", *tp, index);
            else
               strcpy(token, tp);}
        else
           strcpy(token, tp);

        sprintf(sp, "%s ", token);
        sp += strlen(token) + 1;}

    *(sp - 1) = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_REPROC_IN - reprocess the contents of the input buffer
 *               - treat the contents of the buffer as an implicit list
 *               - by effectively wrapping parens about the contents of
 *               - the buffer
 */

static char *_UL_reproc_in(SS_psides *si, char *line)
   {char *rv;
    static char command[MAXLINE];

    rv = NULL;

    if (SX_split_command(command, line))
       {if (!SX_expand_expr(command))
           SS_error(si, "SYNTAX ERROR - _UL_REPROC_IN", SS_null);

        _UL_expand_prefix(command);

/* if it's already a list tell the parser to do nothing - it's already
 * done everything necessary
 */
        if (command[0] != '(')
	   {SX_wrap_paren("(", command, ")", MAXLINE);

	    if (SX_command_log != NULL)
	       PRINT(SX_command_log, "%s\n", command);

	    rv = command;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_RE_ID_CRV - change the id of curve C to the next available one */

int _UL_re_id_crv(object *c)
   {int idn, ido;
    char s[2];

    s[1] = '\0';

    for (idn = 0; idn < SX_N_Curves; idn++)
        if (SX_dataset[idn].n == 0)
	   break;

    idn--;
    ido = SX_curve_id(c);
    if (ido != idn)
       {s[0] = 'A' + idn;
	SX_set_crv_id(ido, s);};

    return(idn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_DEL_INTERMEDIATE - delete intermediate result curves
 *                      - curves that are in CLA but not in any
 *                      - of the other arguments are intermediate results
 */

static void _UL_del_intermediate(SS_psides *si, object *cla, ...)
   {int i, na;
    object *lsta, *lstb;
    object *cb, *clb, *clr, **oa;

    na = SS_length(si, cla);
    oa = CMAKE_N(object *, na);

    for (i = 0, lsta = cla; i < na; i++, lsta = SS_cdr(si, lsta))
        oa[i] = SS_car(si, lsta);

    SC_VA_START(cla);

/* look thru all the lists and NULL out entries in oa which match
 * an entry in any other list
 */
    while (TRUE)
       {clr = SC_VA_ARG(object *);
	if (clr == NULL)
	   break;

	clb = clr;

	if (SS_consp(clb) == TRUE)
	   {for (lstb = clb; !SS_nullobjp(lstb); lstb = SS_cdr(si, lstb))
	        {cb = SS_car(si, lstb);
		 for (i = 0; i < na; i++)
		     {if (cb == oa[i])
			 {oa[i] = NULL;
			  break;};};};}

	else if (SX_curvep_a(clb) == TRUE)
	   {for (i = 0; i < na; i++)
	        {if (clb == oa[i])
		    {oa[i] = NULL;
		     break;};};};};


    SC_VA_END;

/* any non-NULL entry is an intermediate result which should now be deleteed */
    for (i = 0; i < na; i++)
        {if (oa[i] != NULL)
	    UL_delete(si, oa[i]);};

/* reassign the ids of the final results */
#if 0
    if (SS_consp(clr) == TRUE)
       {for (lstb = clr; !SS_nullobjp(lstb); lstb = SS_cdr(si, lstb))
	    _UL_re_id_crv(cb);}

    else
       _UL_re_id_crv(clr);
#endif

    CFREE(oa);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_PARSE - determine whether or not to reprocess the input for Ultra
 *           - this is the double worker for the si->post_eval
 *           - since this si->evobj is not the same as in SS_REPL
 *           - it should be SS_mark'd as being an additional pointer to its
 *           - respective object
 */

static void _UL_parse(SS_psides *si, object *strm)
   {int na, nb, nr;

    SX_parse(si, UL_plot, _UL_reproc_in, strm);

/* get the list of curves after evaluating the latest expression */
    SS_assign(si, crva, UL_get_crv_list(si));

/* identify intermediate results for elimination */
    if (UL_save_intermediate == OFF)
       {na = SS_length(si, crva);
	nb = SS_length(si, crvb);
	nr = SS_length(si, si->evobj);
	if (na > nb+nr)
	   {if (SS_consp(crvb) == TRUE)
	       _UL_del_intermediate(si, crva, crvb, si->val, NULL);
	    else
	       _UL_del_intermediate(si, crva, si->val, NULL);};};

/* free the curve lists */
    SS_assign(si, crva, SS_null);
    SS_assign(si, crvb, SS_null);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_READ - this is the si->post_read function for Ultra */

static void _UL_read(SS_psides *si, object *strm)
   {

/* get the list of curves before evaluating the latest expression */
    SS_assign(si, crvb, UL_get_crv_list(si));

    return;}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_PRINT - the si->post_print function for Ultra */

static int _UL_print(SS_psides *si)
   {

    if (PG_console_device != NULL)
       PG_console_device->gprint_flag = TRUE;

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                       CURVE MANAGEMENT ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* UL_INIT_CURVES - initialize the curve data and references */

void UL_init_curves(SS_psides *si)
   {

    SX_N_Curves = 0;
    SX_enlarge_dataset(si, (PFVoid) UL_curve_eval);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_SET_ID - change the data-id of the given curve */

object *_ULI_set_id(SS_psides *si, object *argl)
   {char id[MAXLINE];
    int jo;
    object *obj;

    SX_prep_arg(si, argl);

    jo    = -1;
    id[0] = '\0';
    SS_args(si, argl,
            UL_CURVE_INDEX_J, &jo,
            UL_DATA_ID_I, id,
            0);

    if (jo < 0)
       SS_error(si, "BAD CURVE ARGUMENT - _ULI_SET_ID", argl);

    if ((id[0] != '@') &&
        ((id[0] == '\0') || (id[0] < 'A') || (id[0] > 'Z')))
       SS_error(si, "BAD ID ARGUMENT - _ULI_SET_ID", argl);

    if (SX_curvep(id))
       SS_error(si, "SPECIFIED ID ALREADY IN USE - _ULI_SET_ID", argl);

    obj = SX_set_crv_id(jo, id);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char *_UL_next_dataid(char *id, int inc)
   {static char ret[MAXLINE];
    int temp;

    ret[0] = '\0';
    if (id[0] == '@')
       {temp = SC_stoi(id+1) + inc;
        if (temp >= 0)
           snprintf(ret, MAXLINE, "@%d", temp);}
    else if ((id[0] == 'Z') && (inc > 0))
        strcpy(ret, "@27");
    else if (!((id[0] == 'A') && (inc < 0)))
       {int ic = (int)id[0];
        ret[0] = (char)(ic + 1);
        ret[1] = '\0';}

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_DATAID_SEQ - sequence the dataids */

object *_UL_dataid_seq(SS_psides *si, char *first, char *last)
   {object *ret = SS_null;
    int i, ifirst, ilast, icur, num, inc;
    char next[MAXLINE], prev[MAXLINE];

    ifirst = SX_get_curve_id(first);
    ilast  = SX_get_curve_id(last);

    if (ifirst > ilast)
       {num = ifirst - ilast + 1;
        inc = -1;}
    else
       {num = ilast - ifirst + 1;
        inc = 1;}

    SS_assign(si, ret, SS_mk_cons(si,
			      SX_get_curve_obj(ifirst),
			      ret));
    strcpy(prev, first);

    for (i = 1; i < num; i++)
        {strcpy(next, _UL_next_dataid(prev, inc));
         if (SX_curvep(next))
            {icur = SX_get_curve_id(next);
             SS_assign(si, ret, SS_mk_cons(si,
				       SX_get_curve_obj(icur),
				       ret));}
         strcpy(prev, next);}

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_THRU - return an expanded list of curves */

object *_ULI_thru(SS_psides *si, object *argl)
   {object *ret;

    ret = SS_null;

    SX_prep_arg(si, argl);

    if (SS_numbp(SS_car(si, argl)))
       {int first = 0, last = 0, id;
        
        SS_args(si, argl,
                SC_INT_I, &first,
                SC_INT_I, &last,
                0);

        if (first < 1)
           SS_error(si,
		      "FIRST ARGUMENT NOT A VALID CURVE NUMBER - _ULI_THRU",
		      argl);

        if (last < 1)
           SS_error(si,
		      "SECOND ARGUMENT NOT A VALID CURVE NUMBER - _ULI_THRU",
		      argl);

        if (first <= last)
           {for (id = first; id <= last; id++)
                SS_assign(si, ret, SS_mk_cons(si,
					  SS_mk_integer(si, id),
					  ret));}
        else
           {for (id = first; id >= last; id--)
                SS_assign(si, ret, SS_mk_cons(si,
					  SS_mk_integer(si, id),
					  ret));};}

    else
       {char first[MAXLINE], last[MAXLINE];
        first[0] = '\0';
        last[0]  = '\0';

        SS_args(si, argl,
                UL_DATA_ID_I, first,
                UL_DATA_ID_I, last,
                0);

/* replace this with a macro or function test */
        if ((first[0] != '@') &&
            ((first[0] < 'A') || (first[0] > 'Z')))
           SS_error(si, "FIRST ARGUMENT NOT A VALID DATA-ID - _ULI_THRU",
		      argl);

        if ((last[0] != '@') &&
            ((last[0] < 'A') || (last[0] > 'Z')))
           SS_error(si, "SECOND ARGUMENT NOT A VALID DATA-ID - _ULI_THRU",
		      argl);

        ret = _UL_dataid_seq(si, first, last);}

    SX_prep_retl(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_COPY_CURVE - make a copy of the curve indexed by j
 *               - and return the objectified curve label
 */

object *UL_copy_curve(SS_psides *si, int j)
   {int i, k;
    double *xpi, *ypi, *xpj, *ypj;
    object *o;

    i = SX_next_space(si);

    SX_assign_next_id(si, i, UL_plot);

    SX_dataset[i].text      = CSTRSAVE(SX_dataset[j].text);
    SX_dataset[i].wc[0]      = SX_dataset[j].wc[0];
    SX_dataset[i].wc[1]      = SX_dataset[j].wc[1];
    SX_dataset[i].wc[2]      = SX_dataset[j].wc[2];
    SX_dataset[i].wc[3]      = SX_dataset[j].wc[3];
    SX_dataset[i].n         = SX_dataset[j].n;
    SX_dataset[i].file_info = SX_dataset[j].file_info;
    SX_dataset[i].file_type = SX_dataset[j].file_type;
    SX_dataset[i].modified  = FALSE;

    if (SX_dataset[j].file != NULL)
       SX_dataset[i].file = CSTRSAVE(SX_dataset[j].file);
    else
       SX_dataset[i].file = NULL;

    xpj = SX_dataset[j].x[0];
    ypj = SX_dataset[j].x[1];
    xpi = SX_dataset[i].x[0] = CMAKE_N(double, SX_dataset[j].n);
    ypi = SX_dataset[i].x[1] = CMAKE_N(double, SX_dataset[j].n);
    if (xpi == NULL || ypi == NULL)
       SS_error(si, "INSUFFICIENT MEMORY - UL_COPY_CURVE", SS_null);

/* copy data if it is already in memory */
    if ((xpj != NULL) && (ypj != NULL))
       for (k = 0; k < SX_dataset[j].n; k++)
           {*xpi++ = *xpj++;
            *ypi++ = *ypj++;};

    PG_set_line_info(SX_dataset[i].info, PLOT_CARTESIAN, CARTESIAN_2D,
                     LINE_SOLID,
		     FALSE, 0, SX_next_color(SX_graphics_device), 0, 0.0);

    o = SX_mk_curve_proc(i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _ULI_EXTRACT_CURVE - extract a curve from the given curve 
 *                    - from xstart to xstop by xstep
 */

object *_ULI_extract_curve(SS_psides *si, object *argl)
   {int i, j, k, l, n, irev;
    double xstart, xstop, xstep, xv, yv, ymn, ymx, tmp;
    double *xpi, *ypi, *xpj, *ypj, *xpjtmp, *ypjtmp;
    char s[MAXLINE];
    object *crv, *o;

    irev   = FALSE;
    crv    = NULL;
    xstart = 0.0;
    xstop  = 1.0;
    xstep  = 0.1;
    SS_args(si, argl,
            SS_OBJECT_I, &crv,
            SC_DOUBLE_I, &xstart,
            SC_DOUBLE_I, &xstop,
            SC_DOUBLE_I, &xstep,
            0);

    j = SX_get_crv_index_i(crv);
    i = SX_next_space(si);
    n = 1 + (1.0 + TOLERANCE)*ABS(xstop - xstart)/xstep;

    xpj = SX_dataset[j].x[0];
    ypj = SX_dataset[j].x[1];

    if ((SX_dataset[j].wc[0] == xpj[SX_dataset[j].n - 1]) &&
        (SX_dataset[j].wc[1] == xpj[0]))
       {xpjtmp = CMAKE_N(double, SX_dataset[j].n);
        ypjtmp = CMAKE_N(double, SX_dataset[j].n);
        for (l = 0; l < SX_dataset[j].n; l++)
            {xpjtmp[l] = xpj[SX_dataset[j].n - l - 1];
             ypjtmp[l] = ypj[SX_dataset[j].n - l - 1];}
        xpj = xpjtmp;
        ypj = ypjtmp;
        irev = TRUE;}

    xpi = SX_dataset[i].x[0] = CMAKE_N(double, n);
    ypi = SX_dataset[i].x[1] = CMAKE_N(double, n);
    if (xpi == NULL || ypi == NULL)
       SS_error(si, "INSUFFICIENT MEMORY - _ULI_EXTRACT_CURVE", SS_null);

    ymn = HUGE;
    ymx = -HUGE;

/* interpolate the new curve from the old one */
    if ((xpj != NULL) && (ypj != NULL))
       {for (k = 0, xv = xstart; k < n; k++, xv += xstep)
            {if (k == n - 1)
                xv = xstop;
             if ((k == 0) && (PM_CLOSETO_REL(xv, xpj[0])))
                {PM_interp(yv, xv, *xpj, *ypj, xpj[1], ypj[1]);}
             else
                {for (; xv > *xpj; xpj++, ypj++);
                 PM_interp(yv, xv, xpj[-1], ypj[-1], *xpj, *ypj);};

             ymn = min(ymn, yv);
             ymx = max(ymx, yv);

             *xpi++ = xv;
             *ypi++ = yv;};}

    if (irev)
       {xpi = SX_dataset[i].x[0];
        ypi = SX_dataset[i].x[1];

        for (l = 0; l < n/2; l++)
            {tmp = xpi[l];
             xpi[l] = xpi[n - l - 1];
             xpi[n - l - 1] = tmp;
             tmp = ypi[l];
             ypi[l] = ypi[n - l - 1];
             ypi[n - l - 1] = tmp;}

        CFREE(xpjtmp);
        CFREE(ypjtmp);}

    snprintf(s, MAXLINE, "Extract %c (%e to %e by %e)",
            SX_dataset[j].id, xstart, xstop, xstep);

    SX_assign_next_id(si, i, UL_plot);

    SX_dataset[i].text      = CSTRSAVE(s);
    SX_dataset[i].file      = NULL;
    SX_dataset[i].wc[0]      = xstart;
    SX_dataset[i].wc[1]      = xstop;
    SX_dataset[i].wc[2]      = ymn;
    SX_dataset[i].wc[3]      = ymx;
    SX_dataset[i].n         = n;
    SX_dataset[i].file_info = NULL;
    SX_dataset[i].file_type = SC_NO_FILE;
    SX_dataset[i].modified  = FALSE;

    PG_set_line_info(SX_dataset[i].info, PLOT_CARTESIAN, CARTESIAN_2D,
                     LINE_SOLID,
		     FALSE, 0, SX_next_color(SX_graphics_device), 0, 0.0);

    o = SX_mk_curve_proc(i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_XINDEX_CURVE - make a copy of the curve indexed by j
 *                 - with x values replaced by the index of the x values
 *                 - and return the objectified curve
 */

object *UL_xindex_curve(SS_psides *si, int j)
   {int i, k, n;
    double *xpi, *ypi, *xpj, *ypj;
    object *o;

    i = SX_next_space(si);
    n = SX_dataset[j].n;

    SX_assign_next_id(si, i, UL_plot);

    SX_dataset[i].text      = CSTRSAVE(SX_dataset[j].text);
    SX_dataset[i].wc[0]     = 1.0;
    SX_dataset[i].wc[1]     = (double) n;
    SX_dataset[i].wc[2]     = SX_dataset[j].wc[2];
    SX_dataset[i].wc[3]     = SX_dataset[j].wc[3];
    SX_dataset[i].n         = n;
    SX_dataset[i].file_info = SX_dataset[j].file_info;
    SX_dataset[i].file_type = SX_dataset[j].file_type;
    SX_dataset[i].modified  = FALSE;

    if (SX_dataset[j].file != NULL)
       SX_dataset[i].file = CSTRSAVE(SX_dataset[j].file);
    else
       SX_dataset[i].file = NULL;

    xpj = SX_dataset[j].x[0];
    ypj = SX_dataset[j].x[1];
    xpi = SX_dataset[i].x[0] = CMAKE_N(double, n);
    ypi = SX_dataset[i].x[1] = CMAKE_N(double, n);
    if (xpi == NULL || ypi == NULL)
       SS_error(si, "INSUFFICIENT MEMORY - UL_XINDEX_CURVE", SS_null);

/* copy data if it is already in memory */
    if ((xpj != NULL) && (ypj != NULL))
       for (k = 0; k < n; k++)
           {*xpi++ = (double) (k+1);
            *ypi++ = *ypj++;};

    PG_set_line_info(SX_dataset[i].info, PLOT_CARTESIAN, CARTESIAN_2D,
                     LINE_SOLID,
		     FALSE, 0, SX_next_color(SX_graphics_device), 0, 0.0);

    o = SX_mk_curve_proc(i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_PRINT_BANNER - put the banner nicely on the screen */

void UL_print_banner(void)
   {

    SC_banner("");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_INIT_ENV - setup the overall ULTRA environment */

static void UL_init_env(SS_psides *si)
   {int i;
    int *plot_type;

/* initialize the prefix list */
    for (i = 0; i < NPREFIX; i++)
        SX_prefix_list[i] = 0;

    SX_pui_file  = CSTRSAVE("ultra.pui");
    SX_GRI_title = CSTRSAVE("ULTRA Controls");

    PG_register_callback("Replot", UL_plot);

    plot_type = PG_ptr_attr_glb("plot-type");

    PG_register_variable("Plot Type", SC_INT_S,
			 plot_type, NULL, NULL);

/* add the SX annotation stuff */
    _SX_install_pgs_iob(si);

/* these lisp package special variables are initialized in all modes */
    SS_set_print_err_func(NULL, TRUE);
    si->get_arg  = _UL_args;
    SX_plot_hook = UL_plot;

    SC_gs.atof   = SC_atof;
    SC_gs.strtod = SC_strtod;

/* you doublely want to do interrupt based terminal I/O so as to not
 * have ULTRA hog the CPU while polling for input (especially on a CRAY)
 */
    PG_IO_INTERRUPTS(TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_MODE_TEXT - go into text mode and
 *              - return "#t" if text mode was already in effect
 *              - else return "#f"
 */

object *UL_mode_text(SS_psides *si)
   {object *ret;

    if (PG_console_device == NULL)
       PG_open_console("ULTRA II", SX_console_type, SX_background_color_white,
                       SX_console_x, SX_console_y,
                       SX_console_width, SX_console_height);

    if (SX_graphics_device != NULL)
       {PG_clear_window(SX_graphics_device);
        PG_close_device(SX_graphics_device);
        SX_graphics_device = NULL;

        SX_gr_mode   = FALSE;
        SX_plot_flag = FALSE;

        ret = SS_t;}
    else
        ret = SS_f;

/* give default values to the lisp package interface variables */
    si->post_read  = NULL;
    si->post_eval  = NULL;
    si->post_print = NULL;
    si->pr_ch_un   = SS_unget_ch;
    si->pr_ch_out  = SS_put_ch;

#ifdef NO_SHELL
    SS_set_put_line(si, SX_fprintf);
    SS_set_put_string(si, SX_fputs);
    SC_set_get_line(PG_wind_fgets);
#else
    SS_set_put_line(si, SS_printf);
    SS_set_put_string(si, SS_fputs);
    SC_set_get_line(io_gets);
#endif

    SS_set_prompt(si, "S-> ");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_MODE_GRAPHICS - go into graphics mode
 *                  - return "#t" if graphics mode was already in effect
 *                  - else return "#f"
 */

object *UL_mode_graphics(SS_psides *si)
   {object *ret;
    static object *scrwin = NULL;

    if (PG_console_device == NULL)
       {if (!PG_open_console("ULTRA II", SX_console_type,
                             SX_background_color_white,
                             SX_console_x, SX_console_y,
                             SX_console_width, SX_console_height))
           {PRINT(STDOUT, "\nCannot connect to display\n\n");};}

    if (SX_graphics_device == NULL)
       {SS_set_prompt(si, "U-> ");
        strcpy(si->ans_prompt, "");

        si->post_read  = _UL_read;
        si->post_eval  = _UL_parse;
        si->post_print = _UL_print;
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

	if (SX_graphics_device != NULL)
	   {if (scrwin == NULL)
	       {scrwin = SX_mk_graphics_device(si, SX_graphics_device);
		SS_install_cv(si, "screen-window", scrwin, SS_OBJECT_I);
		SS_UNCOLLECT(scrwin);}
 	    else
	       scrwin->val = (void *) SX_graphics_device;

/* map the ultra graphics state onto the device */
	    UL_set_graphics_state(SX_graphics_device);

	    SX_setup_viewspace(SX_graphics_device, UL_window_height_factor);

/* open the device now */
	    PG_open_device(SX_graphics_device,
			   SX_window_x[0], SX_window_x[1],
			   SX_window_width, SX_window_height);

            if (SX_current_palette != NULL)
               {SX_graphics_device->current_palette = 
                   PG_rd_palette(SX_graphics_device, SX_current_palette);}
            
	    PG_make_device_current(SX_graphics_device);
	    SX_border_width = SX_graphics_device->border_width;
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

/* UL_SET_GRAPHICS_STATE - map the ultra graphics state variables onto the
 *                       - given device before initialization or plotting
 */

void UL_set_graphics_state(PG_device *d)
   {int i, nc, width, height;
    double mrks;
    char *axstf;
    out_device *out;
    PG_dev_geometry *g;

    if (d != NULL)
       {g = &d->g;

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

	PG_set_axis_log_scale(d, 2, SX_log_scale);
	PG_set_font(d, axstf, SX_plot_type_style, SX_plot_type_size);
	PG_set_marker_scale(d, mrks);
	PG_set_marker_orientation(d, SX_marker_orientation);

	if (!POSTSCRIPT_DEVICE(d))
	   {g->hwin[0] = SX_window_P[0];
	    g->hwin[1] = g->hwin[0] + SX_window_width_P;
	    g->hwin[2] = SX_window_P[1];
	    g->hwin[3] = g->hwin[2] + SX_window_height_P;}
	else
	   {PG_query_screen(d, &width, &height, &nc);   
	    g->hwin[1] = g->hwin[0] + width;
	    g->hwin[3] = g->hwin[2] + height;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* USAGE - print help */

static void usage(void)
   {

    printf("\n");
    printf("Usage: ultra [-e] [-h] [-i] [-l <file>]* [-m] [-n] [-p] [-q] [-r] [-s] [-u] [-w]\n");
    printf("             <file> | (<scheme-command>)\n");
    printf("\n");

    printf("Options:\n");
    printf("    -e     abort on error instead of returning to the interpreter\n");
    printf("    -h     this help message\n");
    printf("    -i     do not use interrupt driven I/O\n");
    printf("    -l     load file containing commands (Scheme or C syntax)\n");
    printf("    -m     on X systems do drawing to pixmap and then blt\n");
    printf("    -n     do not load the standard interpreted command files\n");
    printf("    -p     add PID to the cache filename\n");
    printf("    -q     do not display the ULTRA banner\n");
    printf("    -r     do not load the .ultrarc file\n");
    printf("    -s     start up in command mode with no graphics window\n");
    printf("    -u     start up in graphics mode with a window (default)\n");
    printf("    -w     on X systems do all drawing directly to the window\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start up a fun filled session of ULTRA right here */

int main(int c, char **v, char **env)
   {int i, n, load_init, load_rc, zsp, rv;
    int commnd_flag, no_banner, n_files, tflag, track;
    SIGNED int order[4096];
    char commnd[MAXLINE];
    double evalt;
    SS_psides *si;

    SC_init_path(1, "ULTRA");

    si = SS_init_scheme(CODE, VERSION, c, v, env);

    SS_init(si, "Aborting with error", _UL_quit,
            TRUE, SS_interrupt_handler,
            FALSE, NULL, 0);

/* NOTE: be able to access remote files
 * this MUST be set before the PD_init_threads uses the current
 * io hooks to initialize the pio hooks
 */
    SC_io_connect(PC_REMOTE);

    PD_set_format_version(3);

    PD_init_threads(0, NULL);

/* start out with interrupts off until we are set
 * that is until we are ready to process the command line args
 */
    PG_IO_INTERRUPTS(FALSE);

    commnd_flag = FALSE;
    tflag       = FALSE;
    no_banner   = FALSE;
    track       = TRUE;

/* initialize the file order */
    n_files = 0;

#ifdef NO_SHELL
/* for debugging purposes */
    load_init = FALSE;
    load_rc   = FALSE;
#else
    load_init = TRUE;
    load_rc   = TRUE;
#endif

    PG_set_use_pixmap(FALSE);

    zsp = 2;

    SC_zero_space_n(zsp, -2);

/* ULTRA initializations not depending on scheme */
    UL_init_view(si);
    UL_init_hash();
    UL_install_global_vars(si);
    UL_install_funcs(si);

/* ULTRA initializations depending on scheme */
    UL_install_scheme_funcs(si);
    UL_init_curves(si);

    UL_init_env(si);

/* process the command line arguments */
    for (i = 1; i < c; i++)
        if (v[i][0] == '-')
           {switch (v[i][1])
               {case 'e' :
		     si->trap_error = FALSE;
		     break;
		case 'h' :
		     usage();
		     return(1);
		     break;
                case 'i' :                       /* IO not interrupt driven */
		     PG_IO_INTERRUPTS(FALSE);
                     break;
                case 'l' :                              /* load Scheme file */
                     order[n_files++] = ++i;
                     break;
                case 'm' :                          /* use X pixmap for drawing */
                     PG_set_use_pixmap(TRUE);
                     break; 
                case 'n' :                          /* don't load init file */
                     load_init = FALSE;
                     break;
                case 'p' :     
                     SX_cache_addpid();               /* curves-PID.a */
                     break;
                case 'q' :                      /* don't display the banner */
                     no_banner = TRUE;
                     break;
                case 'r' :                           /* don't load .rc file */
                     load_rc   = FALSE;
                     break;
                case 's' :                                   /* Scheme mode */
                     SX_gr_mode = FALSE;
		     PG_IO_INTERRUPTS(FALSE);
                     break;
                case 't' :                                /* time the loads */
                     tflag = TRUE;
                     break;
                case 'u' :                                    /* Ultra mode */
                     SX_gr_mode = TRUE;
                     break;
                case 'w' :                       /* use X window for drawing */
                                                 /* not pixmap               */
                     PG_set_use_pixmap(FALSE);
                     break;
               case 'x' :
                     track = FALSE;
                     break;
               case 'z' :                              
                     zsp = SC_stoi(v[++i]);
                     SC_zero_space_n(zsp, -2);
                     break;};}

        else if ((v[i][0] != '(') && !commnd_flag)
	   order[n_files++] = -i;

        else if (v[i][0] == '(')
           {commnd_flag = TRUE;
            strcpy(commnd, " ");
	    SC_concatenate(commnd, MAXLINE, c-i, v+i, " ", TRUE);};

#ifndef NO_SHELL
    if (!no_banner && !commnd_flag)
       UL_print_banner();
#endif

    SX_autoplot = OFF;
    if (load_init)
       SX_load_rc(si, "ultra.scm", load_rc, ".ultrarc", "ultra.ini");

    if (track)
       SC_send_tracker("ultra", VERSION, 0, NULL);

    SX_autoplot = ON;
    if (SX_gr_mode)
       UL_mode_graphics(si);
    else
       UL_mode_text(si);

#ifdef NO_SHELL
    if (!commnd_flag)
       UL_print_banner();
#endif

    PG_expose_device(PG_console_device);

/* if it is not a script then we have to process the files at this level */
    if (TRUE)

/* read the optionally specified data/scheme files in order */
       {for (i = 0; i < n_files; i++)
	    {n = order[i];
	     if (n < 0)
	        UL_rd_scm(si, v[-n]);
	     else
	        {evalt = SC_cpu_time();
		 SS_load_scm(si, v[n]);
		 evalt = SC_cpu_time() - evalt;

		 if (tflag)
		    PRINT(STDOUT,
			  "   %s load time: (%10.3e)\n",
			  v[n], evalt);};};};

    si->nsave    = 0;
    si->nrestore = 0;
    si->nsetc    = 0;
    si->ngoc     = 0;

    SC_mem_stats_set(0L, 0L);

    if (commnd_flag)
       rv = !SS_run(si, commnd);
    else 
       {SS_repl(si);
	rv = TRUE;};

    return(rv);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

