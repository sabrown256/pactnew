/*
 * ULCMD.C - high level session support for ULTRA
 *
 */

#include "cpyright.h"

#include "scope_ultra.h"
#include "scope_raster.h"
#include "scope_proc.h"

typedef void (*PFReplot)(void);

UL_scope_public
 UL_gs = { FALSE, TRUE, };

UL_scope_private
 _UL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_INIT - initialize ULTRA */

SS_psides *UL_init(const char *code, const char *vers,
		   int c, char **v, char **env)
   {SS_psides *si;

    SC_init_path(1, "ULTRA");
    si = SS_init_scheme(code, VERSION, c, v, env, TRUE);

    _SX_install_generated(si);

    UL_CURVE_INDEX_I = SC_type_register("curve_i_space",
					KIND_ENUM, B_F, sizeof(int), 0);
    UL_CURVE_INDEX_J = SC_type_register("curve_j_space",
					KIND_ENUM, B_F, sizeof(int), 0);
    UL_DATA_ID_I     = SC_type_register("data_id",
					KIND_ENUM, B_F, sizeof(int), 0);

/* ULTRA initializations not depending on scheme */
    UL_init_view(si);
    UL_init_hash();
    UL_install_global_vars(si);
    UL_install_funcs(si);

/* ULTRA initializations depending on scheme */
    UL_install_scheme_funcs(si);
    UL_init_curves(si);

    UL_init_env(si);

    return(si);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_INIT_VIEW - initialize the plot parameters */

void UL_init_view(SS_psides *si)
   {int j;

    SX_gs.default_npts   = 100;
    SX_gs.gr_mode        = TRUE;
    SX_gs.plot_flag      = TRUE;
    SX_gs.sm             = SX_MODE_ULTRA;

    PG_box_init(3, SX_gs.gwc, 0.0, 1.0);

    SX_gs.view_x[0]      = 0.18;
    SX_gs.view_x[1]      = 0.93;
    SX_gs.view_x[2]      = 0.18;
    SX_gs.view_x[3]      = 0.93;
    SX_gs.view_x[4]      = 0.0;
    SX_gs.view_x[5]      = 1.0;

    SX_gs.view_dx[0]     = 0.75;
    SX_gs.view_dx[1]     = 0.75;
    SX_gs.view_aspect    = 1.0;
    SX_gs.window_x[0]    = 0.5;
    SX_gs.window_x[1]    = 0.1;
    SX_gs.window_dx[0]   = 0.4;
    SX_gs.window_dx[1]   = 0.4;
    SX_gs.window_dx_P[0] = 1.0;
    SX_gs.window_dx_P[1] = 1.0;

    SX_gs.console_x[0]   = 0.0;
    SX_gs.console_x[1]   = 0.0;
    SX_gs.console_dx[0]  = 0.33;
    SX_gs.console_dx[1]  = 0.33;

    SX_gs.console_type  = CSTRSAVE("MONOCHROME");
    SX_gs.display_name  = CSTRSAVE("WINDOW");
    SX_gs.display_type  = CSTRSAVE("COLOR");
    SX_gs.display_title = CSTRSAVE("ULTRA II");

    SS_gs.fmts[1]        = CSTRSAVE("%13.6Le");

    UL_gs.derivative_tolerance = 2.0e-2;
    UL_gs.window_height_factor = 1.0;

    si->interactive = FALSE;
    si->print_flag  = FALSE;
    si->stat_flag   = FALSE;

    SX_gs.command_log_name = CSTRSAVE("ultra.log");

    for (j = 0; j < SX_gs.n_curves; j++)
        SX_gs.data_index[j] = -1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_ARGS - get a C level data item from a single Scheme object */

static void _UL_args(SS_psides *si, object *obj, void *v, SC_type *td)
   {int type, len, i;
    int *pi;
    char *s, *cptr;

    type = td->id;

    if (type == UL_CURVE_INDEX_I)
       {if (SX_curvep_a(obj))
	   {pi  = (int *) v;
	    *pi = SX_get_crv_index_i(obj);}
        else if (SS_integerp(obj))
	   {pi = (int *) v;
	    *pi = SX_gs.number[*SS_GET(int, obj)];}
	else
	   SS_error(si, "OBJECT NOT CURVE - _UL_ARGS", obj);}

    else if (type == UL_CURVE_INDEX_J)
       {if (SX_curvep_a(obj))
	   {pi  = (int *) v;
	    *pi = SX_get_crv_index_j(obj);}
        else if (SS_integerp(obj))
	   {pi = (int *) v;
	    *pi = SX_gs.number[*SS_GET(int, obj)];}
	else
	   SS_error(si, "OBJECT NOT CURVE - _UL_ARGS", obj);}

    else if (type == UL_DATA_ID_I)
       {s   = SS_get_string(obj);
	len = strlen(s);
	cptr = (char *) v;
	for (i = 0; i < len; i++)
	    cptr[i] = (char) toupper((int) s[i]);
	cptr[i] = '\0';}

    else
       _SX_args(si, obj, v, td);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_EXPAND_PREFIX - expand prefix expressions in referring to menu items
 *                   - Because of possible user defined synonyms, this must
 *                   - be done for every command, not just select. This step
 *                   - could come later and be applied only to number lists,
 *                   - if range expansion were moved to the handlers.
 */

static void _UL_expand_prefix(char *s, int nb)
   {int flag, lind, nc, nr;
    char t[MAXLINE], token[MAXLINE];
    char *sp, *tp, *rp;

    SC_strncpy(t, MAXLINE, s, -1);
    sp  = s;
    nr  = nb;

    while ((tp = SC_firsttokq(t, " \t\n\r","\"")) != NULL)
       {if ((*tp >= 'a') && (*tp <= 'z')
            && (SX_gs.prefix_list[*tp - 'a'] > 0)
            && (*(tp+1) == '.'))
           {if ((rp = strchr(tp+2, ')')))
               *rp = '\0';
            if ((flag = SC_intstrp(tp+2, 10)))
               lind = atoi(tp+2);
            if (rp)
               *rp = ')';
            if (flag && rp)
               snprintf(token, MAXLINE, "(pre %c %d))", *tp, lind);
            else if (flag)
               snprintf(token, MAXLINE, "(pre %c %d)", *tp, lind);
            else
               SC_strncpy(token, MAXLINE, tp, -1);}
        else
	   SC_strncpy(token, MAXLINE, tp, -1);

        nc  = snprintf(sp, nr, "%s ", token);
        sp += nc;
        nr -= nc;}

    *(sp - 1) = '\0';

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_REPROC_IN - reprocess the contents of the input buffer
 *               - treat the contents of the buffer as an implicit list
 *               - by effectively wrapping parens about the contents of
 *               - the buffer
 */

static char *_UL_reproc_in(SX_reparsed *pd, char *line)
   {char *rv, *bf;
    SS_psides *si;

    rv = NULL;
    si = pd->si;
    bf = pd->bf;

    SC_strncpy(bf, BFLRG, line, -1);

    if (!SX_expand_expr(bf, BFLRG))
       SS_error(si, "SYNTAX ERROR - _UL_REPROC_IN", SS_null);

    _UL_expand_prefix(bf, BFLRG);

/* if it's already a list tell the parser to do nothing - it's already
 * done everything necessary
 */
    if (bf[0] != '(')
       {SX_wrap_paren("(", bf, ")", BFLRG);

	if (SX_gs.command_log != NULL)
	   PRINT(SX_gs.command_log, "%s\n", bf);

	rv = bf;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_RE_ID_CRV - change the id of curve C to the next available one */

int _UL_re_id_crv(object *c)
   {int idn, ido;
    char s[2];

    s[1] = '\0';

    for (idn = 0; idn < SX_gs.n_curves; idn++)
        if (SX_gs.dataset[idn].n == 0)
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
    SX_reparsed pd;

    pd.si     = si;
    pd.reproc = _UL_reproc_in;
    pd.replot = UL_plot;

    SX_parse(&pd, strm);

/* get the list of curves after evaluating the latest expression */
    SS_assign(si, _UL.crva, UL_get_crv_list(si));

/* identify intermediate results for elimination */
    if (UL_gs.save_intermediate == OFF)
       {na = SS_length(si, _UL.crva);
	nb = SS_length(si, _UL.crvb);
	nr = SS_length(si, si->evobj);
	if (na > nb+nr)
	   {if (SS_consp(_UL.crvb) == TRUE)
	       _UL_del_intermediate(si, _UL.crva, _UL.crvb, si->val, NULL);
	    else
	       _UL_del_intermediate(si, _UL.crva, si->val, NULL);};};

/* free the curve lists */
    SS_assign(si, _UL.crva, SS_null);
    SS_assign(si, _UL.crvb, SS_null);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_READ - this is the si->post_read function for Ultra */

static void _UL_read(SS_psides *si, object *strm)
   {

/* get the list of curves before evaluating the latest expression */
    SS_assign(si, _UL.crvb, UL_get_crv_list(si));

    return;}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_PRINT - the si->post_print function for Ultra */

static int _UL_print(SS_psides *si)
   {

    if (PG_gs.console != NULL)
       PG_gs.console->gprint_flag = TRUE;

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                       CURVE MANAGEMENT ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* UL_INIT_CURVES - initialize the curve data and references */

void UL_init_curves(SS_psides *si)
   {

    SX_gs.n_curves = 0;
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

char *_UL_next_dataid(const char *id, int inc)
   {static char ret[MAXLINE];
    int temp;

    ret[0] = '\0';
    if (id[0] == '@')
       {temp = SC_stoi(id+1) + inc;
        if (temp >= 0)
           snprintf(ret, MAXLINE, "@%d", temp);}

    else if ((id[0] == 'Z') && (inc > 0))
        SC_strncpy(ret, MAXLINE, "@27", -1);

    else if (!((id[0] == 'A') && (inc < 0)))
       {int ic;

	ic = (int) id[0];
        ret[0] = (char) (ic + 1);
        ret[1] = '\0';}

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_DATAID_SEQ - sequence the dataids */

object *_UL_dataid_seq(SS_psides *si, const char *first, const char *last)
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
    SC_strncpy(prev, MAXLINE, first, -1);

    for (i = 1; i < num; i++)
        {SC_strncpy(next, MAXLINE, _UL_next_dataid(prev, inc), -1);
         if (SX_curvep(next))
            {icur = SX_get_curve_id(next);
             SS_assign(si, ret, SS_mk_cons(si,
				       SX_get_curve_obj(icur),
				       ret));}
         SC_strncpy(prev, MAXLINE, next, -1);}

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
                G_INT_I, &first,
                G_INT_I, &last,
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

    SX_gs.dataset[i].text      = CSTRSAVE(SX_gs.dataset[j].text);
    SX_gs.dataset[i].wc[0]     = SX_gs.dataset[j].wc[0];
    SX_gs.dataset[i].wc[1]     = SX_gs.dataset[j].wc[1];
    SX_gs.dataset[i].wc[2]     = SX_gs.dataset[j].wc[2];
    SX_gs.dataset[i].wc[3]     = SX_gs.dataset[j].wc[3];
    SX_gs.dataset[i].n         = SX_gs.dataset[j].n;
    SX_gs.dataset[i].file_info = SX_gs.dataset[j].file_info;
    SX_gs.dataset[i].file_type = SX_gs.dataset[j].file_type;
    SX_gs.dataset[i].modified  = FALSE;

    SC_mark(SX_gs.dataset[i].file_info, 1);

    if (SX_gs.dataset[j].file != NULL)
       SX_gs.dataset[i].file = CSTRSAVE(SX_gs.dataset[j].file);
    else
       SX_gs.dataset[i].file = NULL;

    xpj = SX_gs.dataset[j].x[0];
    ypj = SX_gs.dataset[j].x[1];
    xpi = SX_gs.dataset[i].x[0] = CMAKE_N(double, SX_gs.dataset[j].n);
    ypi = SX_gs.dataset[i].x[1] = CMAKE_N(double, SX_gs.dataset[j].n);
    if (xpi == NULL || ypi == NULL)
       SS_error(si, "INSUFFICIENT MEMORY - UL_COPY_CURVE", SS_null);

/* copy data if it is already in memory */
    if ((xpj != NULL) && (ypj != NULL))
       for (k = 0; k < SX_gs.dataset[j].n; k++)
           {*xpi++ = *xpj++;
            *ypi++ = *ypj++;};

    PG_set_line_info(SX_gs.dataset[i].info, PLOT_CARTESIAN, CARTESIAN_2D,
                     LINE_SOLID,
		     FALSE, 0, SX_next_color(SX_gs.graphics_device), 0, 0.0);

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
            G_OBJECT_I, &crv,
            G_DOUBLE_I, &xstart,
            G_DOUBLE_I, &xstop,
            G_DOUBLE_I, &xstep,
            0);

    j = SX_get_crv_index_i(crv);
    i = SX_next_space(si);
    n = 1 + (1.0 + TOLERANCE)*fabs(xstop - xstart)/xstep;

    xpj = SX_gs.dataset[j].x[0];
    ypj = SX_gs.dataset[j].x[1];

    if ((SX_gs.dataset[j].wc[0] == xpj[SX_gs.dataset[j].n - 1]) &&
        (SX_gs.dataset[j].wc[1] == xpj[0]))
       {xpjtmp = CMAKE_N(double, SX_gs.dataset[j].n);
        ypjtmp = CMAKE_N(double, SX_gs.dataset[j].n);
        for (l = 0; l < SX_gs.dataset[j].n; l++)
            {xpjtmp[l] = xpj[SX_gs.dataset[j].n - l - 1];
             ypjtmp[l] = ypj[SX_gs.dataset[j].n - l - 1];}
        xpj = xpjtmp;
        ypj = ypjtmp;
        irev = TRUE;}

    xpi = SX_gs.dataset[i].x[0] = CMAKE_N(double, n);
    ypi = SX_gs.dataset[i].x[1] = CMAKE_N(double, n);
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
       {xpi = SX_gs.dataset[i].x[0];
        ypi = SX_gs.dataset[i].x[1];

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
            SX_gs.dataset[j].id, xstart, xstop, xstep);

    SX_assign_next_id(si, i, UL_plot);

    SX_gs.dataset[i].text      = CSTRSAVE(s);
    SX_gs.dataset[i].file      = NULL;
    SX_gs.dataset[i].wc[0]      = xstart;
    SX_gs.dataset[i].wc[1]      = xstop;
    SX_gs.dataset[i].wc[2]      = ymn;
    SX_gs.dataset[i].wc[3]      = ymx;
    SX_gs.dataset[i].n         = n;
    SX_gs.dataset[i].file_info = NULL;
    SX_gs.dataset[i].file_type = SC_NO_FILE;
    SX_gs.dataset[i].modified  = FALSE;

    PG_set_line_info(SX_gs.dataset[i].info, PLOT_CARTESIAN, CARTESIAN_2D,
                     LINE_SOLID,
		     FALSE, 0, SX_next_color(SX_gs.graphics_device), 0, 0.0);

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
    n = SX_gs.dataset[j].n;

    SX_assign_next_id(si, i, UL_plot);

    SX_gs.dataset[i].text      = CSTRSAVE(SX_gs.dataset[j].text);
    SX_gs.dataset[i].wc[0]     = 1.0;
    SX_gs.dataset[i].wc[1]     = (double) n;
    SX_gs.dataset[i].wc[2]     = SX_gs.dataset[j].wc[2];
    SX_gs.dataset[i].wc[3]     = SX_gs.dataset[j].wc[3];
    SX_gs.dataset[i].n         = n;
    SX_gs.dataset[i].file_info = SX_gs.dataset[j].file_info;
    SX_gs.dataset[i].file_type = SX_gs.dataset[j].file_type;
    SX_gs.dataset[i].modified  = FALSE;

    SC_mark(SX_gs.dataset[i].file_info, 1);

    if (SX_gs.dataset[j].file != NULL)
       SX_gs.dataset[i].file = CSTRSAVE(SX_gs.dataset[j].file);
    else
       SX_gs.dataset[i].file = NULL;

    xpj = SX_gs.dataset[j].x[0];
    ypj = SX_gs.dataset[j].x[1];
    xpi = SX_gs.dataset[i].x[0] = CMAKE_N(double, n);
    ypi = SX_gs.dataset[i].x[1] = CMAKE_N(double, n);
    if (xpi == NULL || ypi == NULL)
       SS_error(si, "INSUFFICIENT MEMORY - UL_XINDEX_CURVE", SS_null);

/* copy data if it is already in memory */
    if ((xpj != NULL) && (ypj != NULL))
       for (k = 0; k < n; k++)
           {*xpi++ = (double) (k+1);
            *ypi++ = *ypj++;};

    PG_set_line_info(SX_gs.dataset[i].info, PLOT_CARTESIAN, CARTESIAN_2D,
                     LINE_SOLID,
		     FALSE, 0, SX_next_color(SX_gs.graphics_device), 0, 0.0);

    o = SX_mk_curve_proc(i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_INIT_ENV - setup the overall ULTRA environment */

void UL_init_env(SS_psides *si)
   {int i;
    int *plot_type;

/* initialize the prefix list */
    for (i = 0; i < NPREFIX; i++)
        SX_gs.prefix_list[i] = 0;

    SX_gs.pui_file  = CSTRSAVE("ultra.pui");
    SX_gs.gri_title = CSTRSAVE("ULTRA Controls");

    PG_register_callback("Replot", UL_plot);

    plot_type = PG_ptr_attr_glb("plot-type");

    PG_register_variable("Plot Type", G_INT_S,
			 plot_type, NULL, NULL);

/* add the SX annotation stuff */
    _SX_install_pgs_iob(si);

/* these lisp package special variables are initialized in all modes */
    SS_set_print_err_func(NULL, TRUE);
    si->get_arg  = _UL_args;
    SX_gs.plot_hook = UL_plot;

    SC_gs.atof   = SC_atof;
    SC_gs.strtod = SC_strtod;

/* you doublely want to do interrupt based terminal I/O so as to not
 * have ULTRA hog the CPU while polling for input (especially on a CRAY)
 */
    SC_set_io_interrupts(TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_MODE_TEXT - go into text mode and
 *              - return "#t" if text mode was already in effect
 *              - else return "#f"
 */

object *UL_mode_text(SS_psides *si)
   {object *ret;

    ret = SX_mode_text(si);

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

    if (PG_gs.console == NULL)
       {if (!PG_open_console("ULTRA II", SX_gs.console_type,
                             SX_gs.background_color_white,
                             SX_gs.console_x[0], SX_gs.console_x[1],
                             SX_gs.console_dx[0], SX_gs.console_dx[1]))
           {PRINT(STDOUT, "\nCannot connect to display\n\n");};}

    if (SX_gs.graphics_device == NULL)
       {SS_set_prompt(si, "U-> ");
        SC_strncpy(si->ans_prompt, MAXLINE, "", -1);

        si->post_read  = _UL_read;
        si->post_eval  = _UL_parse;
        si->post_print = _UL_print;
	si->pr_gets    = _SX_get_input;

	SS_set_put_line(si, SX_fprintf);
	SS_set_put_string(si, SX_fputs);
	if (PG_gs.console == NULL)
	   SC_set_get_line(io_gets);
	else
	   SC_set_get_line(PG_wind_fgets);

        SX_gs.gr_mode         = TRUE;
        SX_gs.graphics_device = PG_make_device(SX_gs.display_name,
					       SX_gs.display_type,
					       SX_gs.display_title);

	if (SX_gs.graphics_device != NULL)
	   {if (scrwin == NULL)
	       {scrwin = SX_make_pg_device(si, SX_gs.graphics_device);
		SS_install_cv(si, "screen-window", scrwin, G_OBJECT_I);
		SS_UNCOLLECT(scrwin);}
 	    else
	       scrwin->val = (void *) SX_gs.graphics_device;

/* map the ultra graphics state onto the device */
	    UL_set_graphics_state(SX_gs.graphics_device);

	    SX_setup_viewspace(SX_gs.graphics_device,
			       UL_gs.window_height_factor);

/* open the device now */
	    PG_open_device(SX_gs.graphics_device,
			   SX_gs.window_x[0], SX_gs.window_x[1],
			   SX_gs.window_dx[0], SX_gs.window_dx[1]);

            if (SX_gs.current_palette != NULL)
               {SX_gs.graphics_device->current_palette = 
                   PG_rd_palette(SX_gs.graphics_device, SX_gs.current_palette);}
            
	    PG_make_device_current(SX_gs.graphics_device);
	    SX_gs.border_width = SX_gs.graphics_device->border_width;
	    PG_set_viewspace(SX_gs.graphics_device, 2, WORLDC, NULL);
	    PG_release_current_device(SX_gs.graphics_device);

	    PG_set_default_event_handler(SX_gs.graphics_device,
					 SX_default_event_handler);

	    PG_set_motion_event_handler(SX_gs.graphics_device,
					SX_motion_event_handler);
	    
	    PG_set_mouse_down_event_handler(SX_gs.graphics_device,
					    SX_mouse_event_handler);

	    PG_set_mouse_up_event_handler(SX_gs.graphics_device,
					  SX_mouse_event_handler);

	    PG_set_expose_event_handler(SX_gs.graphics_device,
					SX_expose_event_handler);

	    PG_set_update_event_handler(SX_gs.graphics_device,
					SX_update_event_handler);

	    SC_REGISTER_CONTEXT(SX_default_event_handler, SS_psides, si);
	    SC_REGISTER_CONTEXT(SX_motion_event_handler,  SS_psides, si);
	    SC_REGISTER_CONTEXT(SX_mouse_event_handler,   SS_psides, si);
	    SC_REGISTER_CONTEXT(SX_expose_event_handler,  SS_psides, si);
	    SC_REGISTER_CONTEXT(SX_update_event_handler,  SS_psides, si);

/* remember the window size and position in pixels */
	    SX_gs.window_dx_P[0] = PG_window_width(SX_gs.graphics_device);
	    SX_gs.window_dx_P[1] = PG_window_height(SX_gs.graphics_device);
	    SX_gs.window_P[0]    = SX_gs.graphics_device->g.hwin[0];
	    SX_gs.window_P[1]    = SX_gs.graphics_device->g.hwin[2];

	    if (PG_gs.console != NULL)
	       PG_expose_device(PG_gs.console);};

        ret = SS_t;}
    else
       ret = SS_f;

    PG_make_device_current(PG_gs.console);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_SET_GRAPHICS_STATE - map the ultra graphics state variables onto the
 *                       - given device before initialization or plotting
 */

void UL_set_graphics_state(PG_device *d)
   {int i, nc;
    int dx[PG_SPACEDM];
    double mrks;
    char *axstf;
    out_device *out;
    PG_dev_geometry *g;

    if (d != NULL)
       {g = &d->g;

	d->autodomain             = SX_gs.autodomain;
	d->autoplot               = SX_gs.autoplot;
	d->autorange              = SX_gs.autorange;
	d->background_color_white = SX_gs.background_color_white;

	out = SX_match_device(d);
	if (out != NULL)
	   {if (out->background_color != -1)
	       d->background_color_white = out->background_color;};

	d->border_width = SX_gs.border_width;
	d->data_id      = SX_gs.data_id;
	d->gprint_flag  = TRUE;
	d->grid         = SX_gs.grid;

	d->view_aspect  = SX_gs.view_aspect;

	for (i = 0; i < PG_BOXSZ; i++)
	    {d->view_x[i] = SX_gs.view_x[i];

/* viewport limits in WC */
	     g->wc[i] = SX_gs.gwc[i];

/* WC to BND pad */
	     g->pad[i] = SX_gs.gpad[i];};

	PG_get_attrs_glb(TRUE,
			 "axis-type-face", &axstf,
			 "marker-scale",   &mrks,
			 NULL);

	PG_fset_axis_log_scale(d, 2, SX_gs.log_scale);
	PG_fset_font(d, axstf, SX_gs.plot_type_style, SX_gs.plot_type_size);
	PG_fset_marker_scale(d, mrks);
	PG_fset_marker_orientation(d, SX_gs.marker_orientation);

	if (!POSTSCRIPT_DEVICE(d))
	   {g->hwin[0] = SX_gs.window_P[0];
	    g->hwin[1] = g->hwin[0] + SX_gs.window_dx_P[0];
	    g->hwin[2] = SX_gs.window_P[1];
	    g->hwin[3] = g->hwin[2] + SX_gs.window_dx_P[1];}
	else
	   {PG_query_screen_n(d, dx, &nc);   
	    g->hwin[1] = g->hwin[0] + dx[0];
	    g->hwin[3] = g->hwin[2] + dx[1];};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MODE_ULTRA - setup Ultra mode */

void SX_mode_ultra(SS_psides *si, int load_init, int load_rc, int track)
   {

    SX_gs.autoplot = OFF;
    if (load_init == TRUE)
       SX_load_rc(si, "ultra.scm", load_rc, ".ultrarc", "ultra.ini");

    if (track == TRUE)
       SC_send_tracker("ultra", VERSION, 0, NULL);

    SX_gs.autoplot = ON;
    if (SX_gs.gr_mode == TRUE)
       UL_mode_graphics(si);
    else
       UL_mode_text(si);

    PG_expose_device(PG_gs.console);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

