/*
 * SXHAND.C - handlers for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

typedef double (*PFDoubleR)(double x);
typedef double (*PFDoubleRR)(double x, double y);
typedef PM_mapping *(*PF_PPM_mapping_1)(SS_psides *si, 
					PM_mapping *f, object *argl);
typedef PM_mapping *(*PF_PPM_mapping_2)(SS_psides *si,
					PM_mapping *f, object **argl);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_HAVE_DISPLAY_LIST - return TRUE iff PDBView function display-mapping*
 *                      - has been defined
 *                      - this is taken as condition for
 *                      - plotting mappings made by the handlers
 */

int SX_have_display_list(SS_psides *si)
   {object *var, *pl;
    int plf;

    plf = FALSE;

    var = (object *) SC_hasharr_def_lookup(si->symtab, "display-mapping*");
    if (var != NULL)
       {pl  = SS_lk_var_val(si, var);
	plf = SS_procedurep(pl);};

    return(plf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_DISPLAY_MAP - do a display-mapping* on the mapping object MO */

object *SX_display_map(SS_psides *si, object *mo)
   {PM_mapping *h;
    PG_graph *g;

    if (SX_gs.plot_flag && SX_gs.autoplot)
       {h  = SS_GET(PM_mapping, mo);
	g  = PG_make_graph_from_mapping(h,
					SC_PCONS_P_S, NULL,
					1, NULL);
	mo = SX_mk_graph(si, g);

	SS_mark(mo);
	SS_call_scheme(si, "display-mapping*",
		       SS_OBJECT_I, mo,
		       0);};

    return(mo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RESOLVE_MAPPING - resolve the first thing on the given list into
 *                     - something that can be tested as a mapping
 */

static object *_SX_resolve_mapping(SS_psides *si, object *argl)
   {object *obj, *fo, *var, *fnc;

    SS_save(si, si->argl);

    obj = SS_car(si, argl);
    fo  = obj;

    var = (object *) SC_hasharr_def_lookup(si->symtab, "map-resolve");
    if (var != NULL)
       {fnc = SS_lk_var_val(si, var);
	if (SS_procedurep(fnc))

/* NOTE: the way this is called argl will end up in si->this and will be
 * freed if this path is taken, so add the mark that tells it that
 * someone else it pointing at it
 */
	   {SS_mark(argl);

	    fo = SS_call_scheme(si, "map-resolve",
				SS_OBJECT_I, obj,
				0);};};

    SS_restore(si, si->argl);

    return(fo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_DETERMINE_DRW_OBJ - extract a PM_mapping from the list
 *                      - and cdr the list
 *                      - if item is a graph extract the mapping from it
 *                      - if there is an interpretative function to resolve
 *                      - an object into a mapping call it
 */

void SX_determine_drw_obj(SS_psides *si, PM_mapping **pf,
			  object **po, object **pargl)
   {PM_mapping *f;
    PG_graph *g;
    object *argl, *obj;

    argl = *pargl;

    obj  = _SX_resolve_mapping(si, argl);
    argl = SS_cdr(si, argl);

    if (pf != NULL)
       {f = NULL;
	if (SX_GRAPHP(obj))
	   {g = SS_GET(PG_graph, obj);
	    f = g->f;}

	else if (SX_MAPPINGP(obj))
	   f = SS_GET(PM_mapping, obj);

       *pf = f;};

    if (po != NULL)
       *po = obj;

    *pargl = argl;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_DETERMINE_MAPPING - extract a PM_mapping from the list
 *                      - and cdr the list
 *                      - if item is a graph extract the mapping from it
 *                      - if there is an interpretative function to resolve
 *                      - an object into a mapping call it
 */

void SX_determine_mapping(SS_psides *si, PM_mapping **pf, object **pargl)
   {object *obj;

    SX_determine_drw_obj(si, pf, &obj, pargl);
/*
    if (*pf == NULL)
       SS_error(si, "BAD MAPPING OR GRAPH - SX_DETERMINE_MAPPING",
		obj);
*/
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*_SX_UNOP - apply a unary operator FN to data D of type T */

static void _SX_unop(PFDoubleR fn, char *t, void *d,
                     int n, double *w)
   {int i, rel, did;
    char type[MAXLINE];
    char *s;

    SC_strncpy(type, MAXLINE, t, -1);
    SC_strtok(type, " *", s);
    did = SC_type_id(type, FALSE);

/* make sure that we have a working array */
    if (w == NULL)
       {w   = CMAKE_N(double, n);
	rel = TRUE;}
    else
       rel = FALSE;

/* fill the working array with double versions of the contents of D */
    PM_array_real(type, d, n, w);

/* operate on the data */

#pragma omp parallel for
    for (i = 0; i < n; i++)
        w[i] = fn(w[i]);

/* convert the data back to the correct type in D */
    SC_convert_id(did, d, 0, 1, SC_DOUBLE_I, w, 0, 1, n, FALSE);

    if (rel)
       CFREE(w);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MH_U_S - SX Math Handler for Unary Scalar operators
 *            - first cut will be to apply operator to scalar elements
 *            - of the range set of the given list of mappings
 *            - or elements of set
 *            - or elements of C_array
 */

object *_SX_mh_u_s(SS_psides *si, C_procedure *cp, object *argl)
   {object *ret, *first, *obj;
    int id, ne, nde;
    char *type;
    double **elem, *pd, *d;
    PFDoubleR fn;
    PM_set *set;

    SX_gs.plot_flag = TRUE;

    ret   = SS_null;
    first = _SX_resolve_mapping(si, argl);
    if (SS_numbp(first))
       ret = SS_unary_flt(si, cp, argl);

    else
       {fn = (PFDoubleR) cp->proc[0];
	SS_assign(si, ret, argl);

	while (SS_consp(argl))
	   {obj  = _SX_resolve_mapping(si, argl);
	    argl = SS_cdr(si, argl);
	    set  = NULL;
	    if (SX_NUMERIC_ARRAYP(obj))
	       {pd   = (double *) NUMERIC_ARRAY_DATA(obj);
		ne   = NUMERIC_ARRAY_LENGTH(obj);
		type = NUMERIC_ARRAY_TYPE(obj);

		_SX_unop(fn, type, (void *) pd, ne, NULL);

		continue;}

	    else if (SX_GRAPHP(obj))
	       set = SS_GET(PG_graph, obj)->f->range;
	    else if (SX_MAPPINGP(obj))
	       set = SS_GET(PM_mapping, obj)->range;
	    else if (SX_SETP(obj))
	       set = SS_GET(PM_set, obj);
	    else
	       SS_error(si, "BAD TYPE OBJECT - _SX_MH_U_S", obj);

	    if (set != NULL)
	       {type = set->element_type;
		ne   = set->n_elements;
		nde  = set->dimension_elem;
		elem = (double **) set->elements;
		d    = CMAKE_N(double, ne);
		for (id = 0; id < nde; id++)
		    {pd = elem[id];
		     _SX_unop(fn, type, (void *) pd, ne, d);};

		CFREE(d);
		PM_find_extrema(set);};};

	SX_prep_ret(si, ret);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_M11_X - math handler applies the given unary function to the x values
 *           - specific to mappings with 1d ranges and domains
 *           - this was UL_uopxc in ULTRA II
 */

object *_SX_m11_x(SS_psides *si, C_procedure *cp, object *argl)
   {int i, n;
    double *xp;
    PFDoubleR fn;
    object *obj, *ret;
    PM_set *set;
    PM_mapping *f;

    SX_gs.plot_flag = TRUE;

    ret = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(si, argl);
        SX_determine_mapping(si, &f, &argl);
        if (f != NULL)
           {set = f->domain;
            xp = *(double **) set->elements;
            n  = set->n_elements;
            fn = (PFDoubleR) cp->proc[0];

#pragma omp parallel for
            for (i = 0; i < n; i++)
                xp[i] = fn(xp[i]);

/* for later
            SX_gs.dataset[j].modified = TRUE;
 */

            SS_assign(si, ret, SS_mk_cons(si, obj, ret));
            PM_find_extrema(set);};};
         
    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_M11_B_MRO - performs unary operations on range values of mappings
 *               - using a single object
 */

object *_SX_m11_b_mro(SS_psides *si, C_procedure *cp, object *argl)
   {PM_mapping *f;
    PF_PPM_mapping_1 op;
    object *al, *obj, *ret;
        
    SX_gs.plot_flag = TRUE;

    op   = (PF_PPM_mapping_1) cp->proc[0];
    al   = SS_car(si, argl);
    argl = SS_cdr(si, argl);

    ret = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(si, argl);
        SX_determine_mapping(si, &f, &argl);
        if (f != NULL)
           op(si, f, al);

	SS_assign(si, ret, SS_mk_cons(si, obj, ret));};
         
    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_CMP_B_SET - performs binary operations on set values
 *               - using a single scalar A and operates on
 *               - component CMP unless CMP is -1 in which case
 *               - it operates on all components
 */

void _SX_cmp_b_set(PFVoid oper, PM_set *set, double a, int cmp)
   {int i, n, id, nde;
    double *yp, **ya;
    PFDoubleRR fnc;

    n   = set->n_elements;
    nde = set->dimension_elem;
    ya  = (double **) set->elements;
    for (id = 0; id < nde; id++)
        {if ((cmp >= 0) && (cmp != id))
	    continue;

         fnc = (PFDoubleRR) oper;
	 yp = ya[id];

#pragma omp parallel for
	 for (i = 0; i < n; i++)
	     yp[i] = fnc(yp[i], a);};

    PM_find_extrema(set);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_M11_B_MRS - performs binary operations on range values of mappings
 *               - using a single scalar
 */

object *_SX_m11_b_mrs(SS_psides *si, C_procedure *cp, object *argl)
   {double a;
    PM_set *set;
    PM_mapping *f;
    object *obj, *ret;
        
    SX_gs.plot_flag = TRUE;

    a = HUGE;
    SS_args(si, argl,
	    SC_DOUBLE_I, &a,
	    0);
    if (a == HUGE)
       SS_error(si, "BAD NUMBER - SX_M11_B_MDS ", argl);

    argl = SS_cdr(si, argl);
    ret  = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(si, argl);
        SX_determine_mapping(si, &f, &argl);
        if (f != NULL)
           {set = f->range;
	    _SX_cmp_b_set(cp->proc[0], set, a, -1);

/* for later
            SX_gs.dataset[j].modified = TRUE;
 */
            SS_assign(si, ret, SS_mk_cons(si, obj, ret));
            PM_find_extrema(set);};};
         
    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_M11_B_MDS - performs binary operations on domains values of mappings
 *               - using a single scalar
 */

object *_SX_m11_b_mds(SS_psides *si, C_procedure *cp, object *argl)
   {double a;
    object *obj, *ret;
    PM_set *set;
    PM_mapping *f;
        
    SX_gs.plot_flag = TRUE;

    a = HUGE;
    SS_args(si, argl,
	    SC_DOUBLE_I, &a,
	    0);
    if (a == HUGE)
       SS_error(si, "BAD NUMBER - SX_M11_B_MDS ", argl);

    argl = SS_cdr(si, argl);
    ret  = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(si, argl);
        SX_determine_mapping(si, &f, &argl);
        if (f != NULL)
           {set = f->domain;
	    _SX_cmp_b_set(cp->proc[0], set, a, -1);

/* for later
            SX_gs.dataset[j].modified = TRUE;
 */
            SS_assign(si, ret, SS_mk_cons(si, obj, ret));
            PM_find_extrema(set);};};
         
    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MIJ_B_MRS - performs binary operations on range values of mappings
 *               - using a single scalar
 *               - the component(s) is specified
 */

object *_SX_mij_b_mrs(SS_psides *si, C_procedure *cp, object *argl)
   {int i;
    double a;
    PM_set *set;
    PM_mapping *f;
    object *obj, *ret;
        
    SX_gs.plot_flag = TRUE;

    i = -1;
    a = HUGE;
    SS_args(si, argl,
	    SC_INT_I, &i,
	    SC_DOUBLE_I, &a,
	    0);
    if (a == HUGE)
       SS_error(si, "BAD NUMBER - SX_MIJ_B_MDS ", argl);

/* NOTE: user specifies 1 for the first component so shitf */
    i--;

    argl = SS_cdr(si, argl);
    ret  = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(si, argl);
        SX_determine_mapping(si, &f, &argl);
        if (f != NULL)
           {set = f->range;
	    _SX_cmp_b_set(cp->proc[0], set, a, i);

/* for later
            SX_gs.dataset[j].modified = TRUE;
 */
            SS_assign(si, ret, SS_mk_cons(si, obj, ret));
            PM_find_extrema(set);};};
         
    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MIJ_B_MDS - performs binary operations on domains values of mappings
 *               - using a single scalar
 *               - the component(s) is specified
 */

object *_SX_mij_b_mds(SS_psides *si, C_procedure *cp, object *argl)
   {int i;
    double a;
    object *obj, *ret;
    PM_set *set;
    PM_mapping *f;
        
    SX_gs.plot_flag = TRUE;

    i = -1;
    a = HUGE;
    SS_args(si, argl,
	    SC_INT_I, &i,
	    SC_DOUBLE_I, &a,
	    0);
    if (a == HUGE)
       SS_error(si, "BAD NUMBER - SX_MIJ_B_MDS ", argl);

/* NOTE: user specifies 1 for the first component so shitf */
    i--;

    argl = SS_cdr(si, argl);
    ret  = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(si, argl);
        SX_determine_mapping(si, &f, &argl);
        if (f != NULL)
           {set = f->domain;
	    _SX_cmp_b_set(cp->proc[0], set, a, i);

/* for later
            SX_gs.dataset[j].modified = TRUE;
 */
            SS_assign(si, ret, SS_mk_cons(si, obj, ret));
            PM_find_extrema(set);};};
         
    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MH_U_M - handler for unary operation acting on a set of mappings */

object *_SX_mh_u_m(SS_psides *si, C_procedure *cp, object *argl)
   {int plf;
    PM_mapping *f, *h;
    PF_PPM_mapping_2 op;
    object *ret, *mo;

    SX_gs.plot_flag = TRUE;

    op  = (PF_PPM_mapping_2) cp->proc[0];
    plf = SX_have_display_list(si);
    ret = SS_null;
    while (SS_consp(argl))
       {SX_determine_mapping(si, &f, &argl);
        if (f != NULL)
            {h  = op(si, f, &argl);
	     mo = SX_mk_mapping(si, h);

	     if (plf)
	        mo = SX_display_map(si, mo);

	     SS_assign(si, ret, SS_mk_cons(si, mo, ret));};};
         
    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MF_INSTALL - install SX math functions in the SX function table
 *               - a separate function table is necessary to tell SCHEME
 *               - which version of "sin" to use, for example
 */

void SX_mf_install(SS_psides *si)
   {

    _SX_mf_inst_g(si);

    SS_install(si, "hyper-plane",
               "Generate a hyper-plane sum(i=0,n; ci*xi)\nFORM hyper-plane c0 (c1 x1min x1max np1) ...)",
               SS_nargs,
	       SX_plane, SS_PR_PROC);

/* Functions under the _SX_MH_U_S handler */

    SS_install(si, "log10",
               "Math base 10 log of range values\nFORM log10 <graph-list>",
               _SX_mh_u_s,
	       PM_log, SS_PR_PROC);
    SS_install(si, "exp",
               "Math exponential of range values y=e^y\nFORM exp <graph-list>",
               _SX_mh_u_s,
	       exp, SS_PR_PROC);
    SS_install(si, "random",
               "Generates random range values between -1 and 1 for the curve\nFORM random <graph-list>",
               _SX_mh_u_s,
	       PM_random, SS_PR_PROC);
    SS_install(si, "cos",
               "Math cosine of y \nFORM cos <graph-list>",
               _SX_mh_u_s,
	       cos, SS_PR_PROC);
    SS_install(si, "acos",
               "Math ArcCos of y\nFORM acos <graph-list>",
               _SX_mh_u_s,
	       acos, SS_PR_PROC);
    SS_install(si, "cosh",
               "Math hyperbolic cosine of y\nFORM cosh <graph-list>",
               _SX_mh_u_s,
	       cosh, SS_PR_PROC);
    SS_install(si, "sin",
               "Math sine of y\nFORM sin <graph-list>",
               _SX_mh_u_s,
	       sin, SS_PR_PROC);
    SS_install(si, "asin",
               "Math ArcSin of y\nFORM asin <graph-list>",
               _SX_mh_u_s,
	       asin, SS_PR_PROC);
    SS_install(si, "sinh",
               "Math hyperbolic sine of y\nFORM sinh <graph-list>",
               _SX_mh_u_s,
	       sinh, SS_PR_PROC);
    SS_install(si, "tan",
               "Math tangent of range values\nFORM tan <graph-list>",
               _SX_mh_u_s,
	       tan, SS_PR_PROC);
    SS_install(si, "atan",
               "Math ArcTan of range values\nFORM atan <graph-list>",
               _SX_mh_u_s,
	       atan, SS_PR_PROC);
    SS_install(si, "tanh",
               "Math hyperbolic tangent on range values\nFORM tanh <graph-list>",
               _SX_mh_u_s,
	       tanh, SS_PR_PROC);

    SS_install(si, "i0",
               "Math zero-th order modified Bessel function of the first kind on range values\nFORM i0 <graph-list>",
               _SX_mh_u_s,
	       PM_i0, SS_PR_PROC);
    SS_install(si, "i1",
               "Math first order modified Bessel function of the first kind on range values\nFORM i1 <graph-list>",
               _SX_mh_u_s,
	       PM_i1, SS_PR_PROC);

    SS_install(si, "j0",
               "Math zero-th order Bessel function of the first kind on range values\nFORM j0 <graph-list>",
               _SX_mh_u_s,
	       PM_j0, SS_PR_PROC);
    SS_install(si, "j1",
               "Math first order Bessel function of the first kind on range values\nFORM j1 <graph-list>",
               _SX_mh_u_s,
	       PM_j1, SS_PR_PROC);

    SS_install(si, "k0",
               "Math zero-th order modified Bessel function of the second kind on range values\nFORM k0 <graph-list>",
               _SX_mh_u_s,
	       PM_k0, SS_PR_PROC);
    SS_install(si, "k1",
               "Math first order modified Bessel function of the second kind on range values\nFORM k1 <graph-list>",
               _SX_mh_u_s,
	       PM_k1, SS_PR_PROC);

    SS_install(si, "y0",
               "Math zero-th order Bessel function of the second kind on range values\nFORM y0 <graph-list>",
               _SX_mh_u_s,
	       PM_y0, SS_PR_PROC);
    SS_install(si, "y1",
               "Math first order Bessel function of the second kind on range values\nFORM y1 <graph-list>",
               _SX_mh_u_s,
	       PM_y1, SS_PR_PROC);

    SS_install(si, "tchn",
               "Math nth order Tchebyshev function on range values\nFORM tchn <graph-list> <n>",
               _SX_mh_u_s,
	       PM_tchn, SS_PR_PROC);
    SS_install(si, "sqr",
               "Math square of range values\nFORM sqr <graph-list>",
               _SX_mh_u_s,
	       PM_sqr, SS_PR_PROC);
    SS_install(si, "ln",
               "Math natural log of range values\nFORM ln <graph-list>",
               _SX_mh_u_s,
	       PM_ln, SS_PR_PROC);
    SS_install(si, "sqrt",
               "Math square root of range values\nFORM sqrt <graph-list>",
               _SX_mh_u_s,
	       PM_sqrt, SS_PR_PROC);
    SS_install(si, "recip",
               "Math reciprocal of y\nFORM recip <graph-list>",
               _SX_mh_u_s,
	       PM_recip, SS_PR_PROC);

    SS_install(si, "abs",
               "Math absolute value of range values\nFORM abs <graph-list>",
               _SX_mh_u_s,
	       ABS, SS_PR_PROC);

/* Functions under the _SX_MH_U_M handler */

    SS_install(si, "refine",
               "Refine the domain of a mapping\nFORM refine [<mapping> '((<n-pts1> <n-pts2> ...)]*",
               _SX_mh_u_m,
	       _SXI_refine_mapping, SS_PR_PROC);
    SS_install(si, "interpolation-weights",
               "Find the interpolation weights for the given mapping\nFORM interpolation-weights [<mapping> '((<n-pts1> <n-pts2> ...)]*",
               _SX_mh_u_m,
	       _SXI_interp_mapping, SS_PR_PROC);

/* Functions under the _SX_MH_B_S handler */

    SS_install(si, "m+",
               "Math sum of mappings\nFORM m+ <graph-list>",
               _SX_mh_b_s,
	       PM_fplus, SS_PR_PROC);
    SS_install(si, "m-",
               "Math difference of mappings\nFORM m- <graph-list>",
               _SX_mh_b_s,
	       PM_fminus, SS_PR_PROC);
    SS_install(si, "m*",
               "Math product of mappings\nFORM m* <graph-list>",
               _SX_mh_b_s,
	       PM_ftimes, SS_PR_PROC);
    SS_install(si, "m/",
               "Math quotient of mappings\nFORM m/ <a> <b>",
               _SX_mh_b_s,
	       PM_fdivide, SS_PR_PROC);
    SS_install(si, "m==",
               "Math equal element mappings\nFORM m== <graph-list>",
               _SX_mh_b_s,
	       PM_ffeq, SS_PR_PROC);
    SS_install(si, "m!=",
               "Math not equal element mappings\nFORM m!= <graph-list>",
               _SX_mh_b_s,
	       PM_ffneq, SS_PR_PROC);
    SS_install(si, "m<",
               "Math less than element mappings\nFORM m< <graph-list>",
               _SX_mh_b_s,
	       PM_fflt, SS_PR_PROC);
    SS_install(si, "m<=",
               "Math less than or equal element mappings\nFORM m<= <graph-list>",
               _SX_mh_b_s,
	       PM_ffle, SS_PR_PROC);
    SS_install(si, "m>",
               "Math greater than element mappings\nFORM m> <graph-list>",
               _SX_mh_b_s,
	       PM_ffgt, SS_PR_PROC);
    SS_install(si, "m>=",
               "Math greater than or equal element mappings\nFORM m>= <graph-list>",
               _SX_mh_b_s,
	       PM_ffge, SS_PR_PROC);
    SS_install(si, "hypot",
               "Math calculates sqrt(a^2+b^2)\nFORM hypot <a> <b>",
               _SX_mh_b_s,
	       HYPOT, SS_PR_PROC);

/* Functions under the _SX_M11_X handler */

    SS_install(si, "lnx",
               "Math natural log of domain values\nFORM lnx <graph-list>",
               _SX_m11_x,
	       PM_ln, SS_PR_PROC);
    SS_install(si, "log10x",
               "Math base 10 log of domain values\nFORM log10x <graph-list>",
               _SX_m11_x,
	       PM_log, SS_PR_PROC);
    SS_install(si, "expx",
               "Math exponential of domain values x=e^x\nFORM expx <graph-list>",
               _SX_m11_x,
	       exp, SS_PR_PROC);
    SS_install(si, "sqrtx",
               "Math square root of domain values\nFORM sqrtx <graph-list>",
               _SX_m11_x,
	       PM_sqrt, SS_PR_PROC);
    SS_install(si, "sqrx",
               "Math square of domain values\nFORM sqrx <graph-list>",
               _SX_m11_x,
	       PM_sqr, SS_PR_PROC);
    SS_install(si, "absx",
               "Math absolute value of x\nFORM absx <graph-list>",
               _SX_m11_x,
	       ABS, SS_PR_PROC);
    SS_install(si, "recipx",
               "Math reciprocal of x\nFORM recipx <graph-list>",
               _SX_m11_x,
	       PM_recip, SS_PR_PROC);
    SS_install(si, "cosx",
               "Math cosine of x\nFORM cosx <graph-list>",
               _SX_m11_x,
	       cos, SS_PR_PROC);
    SS_install(si, "acosx",
               "Math ArcCos of x\nFORM acosx <graph-list>",
               _SX_m11_x,
	       acos, SS_PR_PROC);
    SS_install(si, "coshx",
               "Math hyperbolic cosine of x\nFORM coshx <graph-list>",
               _SX_m11_x,
	       cosh, SS_PR_PROC);
    SS_install(si, "sinx",
               "Math sine of x\nFORM sinx <graph-list>",
               _SX_m11_x,
	       sin, SS_PR_PROC);
    SS_install(si, "asinx",
               "Math ArcSin of x\nFORM asinx <graph-list>",
               _SX_m11_x,
	       asin, SS_PR_PROC);
    SS_install(si, "sinhx",
               "Math hyperbolic sine of x\nFORM sinhx <graph-list>",
               _SX_m11_x,
	       sinh, SS_PR_PROC);
    SS_install(si, "tanx",
               "Math tangent of domain values\nFORM tanx <graph-list>",
               _SX_m11_x,
	       tan, SS_PR_PROC);
    SS_install(si, "atanx",
               "Math ArcTan of domain values\nFORM atanx <graph-list>",
               _SX_m11_x,
	       atan, SS_PR_PROC);
    SS_install(si, "tanhx",
               "Math hyperbolic tangent on domain values\nFORM tanhx <graph-list>",
               _SX_m11_x,
	       tanh, SS_PR_PROC);

    SS_install(si, "i0x",
               "Math zero-th order modified Bessel function of the first kind on domain values\nFORM i0x <graph-list>",
               _SX_m11_x,
	       PM_i0, SS_PR_PROC);
    SS_install(si, "i1x",
               "Math first order modified Bessel function of the first kind on domain values\nFORM i1x <graph-list>",
               _SX_m11_x,
	       PM_i1, SS_PR_PROC);
    SS_install(si, "inx",
               "Math nth order modified Bessel function of the first kind on domain values\nFORM inx <graph-list> <n>",
               _SX_m11_b_mds,
	       PM_in, SS_PR_PROC);

    SS_install(si, "j0x",
               "Math zero-th order Bessel function of the first kind on domain values\nFORM j0x <graph-list>",
               _SX_m11_x,
	       PM_j0, SS_PR_PROC);
    SS_install(si, "j1x",
               "Math first order Bessel function of the first kind on domain values\nFORM j1x <graph-list>",
               _SX_m11_x,
	       PM_j1, SS_PR_PROC);
    SS_install(si, "jnx",
               "Math nth order Bessel function of the first kind on domain values\nFORM jnx <graph-list> <n>",
               _SX_m11_b_mds,
	       PM_jn, SS_PR_PROC);

    SS_install(si, "k0x",
               "Math zero-th order modified Bessel function of the second kind on domain values\nFORM k0x <graph-list>",
               _SX_m11_x,
	       PM_k0, SS_PR_PROC);
    SS_install(si, "k1x",
               "Math first order modified Bessel function of the second kind on domain values\nFORM k1x <graph-list>",
               _SX_m11_x,
	       PM_k1, SS_PR_PROC);
    SS_install(si, "knx",
               "Math nth order modified Bessel function of the second kind on domain values\nFORM knx <graph-list> <n>",
               _SX_m11_b_mds,
	       PM_kn, SS_PR_PROC);

    SS_install(si, "ynx",
               "Math nth order Bessel function of the second kind on domain values\nFORM yn <graph-list> <n>",
               _SX_m11_b_mds,
	       PM_yn, SS_PR_PROC);
    SS_install(si, "y0x",
               "Math zero-th order Bessel function of the second kind on domain values\nFORM y0x <graph-list>",
               _SX_m11_x,
	       PM_y0, SS_PR_PROC);
    SS_install(si, "y1x",
               "Math first order Bessel function of the second kind on domain values\nFORM y1x <graph-list>",
               _SX_m11_x,
	       PM_y1, SS_PR_PROC);


/* _SX_M11_B_MDS and _SX_M11_B_MRS functions */

    SS_install(si, "powrx",
               "Math raise domain values to a power x=x^a\nFORM powrx <a> <graph-list>",
               _SX_m11_b_mds,
	       POW, SS_PR_PROC);
    SS_install(si, "powx",
               "Math raise domain values to a power x=x^a\nFORM powx <a> <graph-list>",
               _SX_m11_b_mds,
	       POW, SS_PR_PROC);
    SS_install(si, "powax",
                "Math raise a to the power of the x value\nx=a^x\nFORM powax <a> <graph-list>",
               _SX_m11_b_mds,
	       PM_pow, SS_PR_PROC);
    SS_install(si, "powr",
               "Math raise range values to a power y=y^a\nFORM powr <a> <graph-list>",
               _SX_m11_b_mrs,
	       POW, SS_PR_PROC);
    SS_install(si, "pow",
               "Math raise range values to a power y=y^a\nFORM pow <a> <graph-list>",
               _SX_m11_b_mrs,
	       POW, SS_PR_PROC);
    SS_install(si, "powa",
               "Math raise a to the power of the y value\ny=a^y\nFORM powa <a> <graph-list>",
               _SX_m11_b_mrs,
	       PM_pow, SS_PR_PROC);

    SS_install(si, "jn",
               "Math nth order Bessel function of the first kind on range values\nFORM jn <n> <graph-list>",
               _SX_m11_b_mrs,
	       PM_jn, SS_PR_PROC);
    SS_install(si, "yn",
               "Math nth order Bessel function of the second kind on range values\nFORM yn <n> <graph-list>",
               _SX_m11_b_mrs,
	       PM_yn, SS_PR_PROC);
    SS_install(si, "in",
               "Math nth order modified Bessel function of the first kind on range values\nFORM in <n> <graph-list>",
               _SX_m11_b_mrs,
	       PM_in, SS_PR_PROC);
    SS_install(si, "kn",
               "Math nth order modified Bessel function of the second kind on range values\nFORM kn <n> <graph-list>",
               _SX_m11_b_mrs,
	       PM_in, SS_PR_PROC);

    SS_install(si, "dx",
               "Shift domain values of curve by a constant\nFORM dx <value> <graph-list>",
               _SX_m11_b_mds,
	       PM_fplus, SS_PR_PROC);
    SS_install(si, "dy",
               "Shift range values of curve by a constant\nFORM dy <value> <graph-list>",
               _SX_m11_b_mrs,
	       PM_fplus, SS_PR_PROC);
    SS_install(si, "mx",
               "Scale domain values of curve by a constant\nFORM mx <value> <graph-list>",
               _SX_m11_b_mds,
	       PM_ftimes, SS_PR_PROC);
    SS_install(si, "my",
               "Scale range values of curve by a constant\nFORM my <value> <graph-list>",
               _SX_m11_b_mrs,
	       PM_ftimes, SS_PR_PROC);
    SS_install(si, "divx",
               "Divide domain values by a constant\nFORM divx <value> <graph-list>",
               _SX_m11_b_mds,
	       PM_fdivide, SS_PR_PROC);
    SS_install(si, "divy",
               "Divide range values by a constant\nFORM divy <value> <graph-list>",
               _SX_m11_b_mrs,
	       PM_fdivide, SS_PR_PROC);

/* _SX_MIJ_B_MDS and _SX_MIJ_B_MRS functions */

    SS_install(si, "powrxc",
               "Math raise domain component values to a power x=x^a\nFORM powrxc <cmp> <a> <graph-list>",
               _SX_mij_b_mds,
	       POW, SS_PR_PROC);
    SS_install(si, "powxc",
               "Math raise domain component values to a power x=x^a\nFORM powxc <cmp> <a> <graph-list>",
               _SX_mij_b_mds,
	       POW, SS_PR_PROC);
    SS_install(si, "powaxc",
                "Math raise a to the power of the x value\nx=a^x\nFORM powaxc <cmp> <a> <graph-list>",
               _SX_mij_b_mds,
	       PM_pow, SS_PR_PROC);
    SS_install(si, "powrc",
               "Math raise range component values to a power y=y^a\nFORM powrc <cmp> <a> <graph-list>",
               _SX_mij_b_mrs,
	       POW, SS_PR_PROC);
    SS_install(si, "powc",
               "Math raise range component values to a power y=y^a\nFORM powc <cmp> <a> <graph-list>",
               _SX_mij_b_mrs,
	       POW, SS_PR_PROC);
    SS_install(si, "powac",
               "Math raise a to the power of the y value\ny=a^y\nFORM powac <cmp> <a> <graph-list>",
               _SX_mij_b_mrs,
	       PM_pow, SS_PR_PROC);
    SS_install(si, "jnc",
               "Math nth order Bessel function of the first kind on range component values\nFORM jnc <cmp> <n> <graph-list>",
               _SX_mij_b_mrs,
	       PM_jn, SS_PR_PROC);
    SS_install(si, "ync",
               "Math nth order Bessel function of the second kind on range component values\nFORM ync <cmp> <n> <graph-list>",
               _SX_mij_b_mrs,
	       PM_yn, SS_PR_PROC);
    SS_install(si, "dxc",
               "Shift domain component values of curve by a constant\nFORM dxc <cmp> <value> <graph-list>",
               _SX_mij_b_mds,
	       PM_fplus, SS_PR_PROC);
    SS_install(si, "dyc",
               "Shift range component values of curve by a constant\nFORM dyc <cmp> <value> <graph-list>",
               _SX_mij_b_mrs,
	       PM_fplus, SS_PR_PROC);
    SS_install(si, "mxc",
               "Scale domain component values of curve by a constant\nFORM mxc <cmp> <value> <graph-list>",
               _SX_mij_b_mds,
	       PM_ftimes, SS_PR_PROC);
    SS_install(si, "myc",
               "Scale range component values of curve by a constant\nFORM myc <cmp> <value> <graph-list>",
               _SX_mij_b_mrs,
	       PM_ftimes, SS_PR_PROC);
    SS_install(si, "divxc",
               "Divide domain component values by a constant\nFORM divxc <cmp> <value> <graph-list>",
               _SX_mij_b_mds,
	       PM_fdivide, SS_PR_PROC);
    SS_install(si, "divyc",
               "Divide range component values by a constant\nFORM divyc <cmp> <value> <graph-list>",
               _SX_mij_b_mrs,
	       PM_fdivide, SS_PR_PROC);

/* _SX_M11_B_MRO functions */

    SS_install(si, "xmm",
               "Excerpts a part of a mapping\nFORM xmm (<low-lim1> <high-lim1> ...) <graph-list>",
               _SX_m11_b_mro,
	       _SXI_extract_mapping, SS_PR_PROC);

#if 0

/* _SX_MH_U_M functions */

    SS_install(si, "rev",
               "Swaps x and range values for a curve\nYou may want to sort after this\nFORM rev <graph-list>",
               _SX_mh_u_m,
	       reverse, SS_PR_PROC);
    SS_install(si, "sort",
               "Sorts a curve into ascending order based on domain values\nFORM sort <graph-list>",
               _SX_mh_u_m,
	       sort, SS_PR_PROC);

/* BCXL functions */

    SS_install(si, "compose",
               "Functional composition f(g(x))\nFORM compose <f> <g>",
               bcxl,
	       compose, SS_PR_PROC);

/* ULAUXF.C functions */
    SS_install(si, "convolve",
               "Procedure: Convolve the two functions\n     Usage: convolve <curve> <curve> [<dt>]",
               SS_nargs,
               _ULI_convlv, SS_PR_PROC);

    SS_install(si, "edit",
               "Procedure: Graphically edit a curve\n     Usage: edit <curve>",
               _ULI_uc,
               _ULI_edit, SS_PR_PROC);

    SS_install(si, "error-bar",
               "Procedure: Do a scatter plot with error bars on the given curve\n     Usage: error-bar <curve> <y+curve> <y-curve> [<x+curve> [<x-curve>]]",
               SS_nargs,
               _ULI_error_plot, SS_PR_PROC);

    SS_install(si, "get-label",
               "Procedure: Return the given curve's label\n     Usage: get-label <curve>",
               SS_sargs,
               _ULI_crv_label, SS_PR_PROC);

    SS_install(si, "get-domain",
               "Procedure: Return the given curve's domain\n     Usage: get-domain <curve>",
               SS_sargs,
               _ULI_crv_domain, SS_PR_PROC);

    SS_install(si, "get-range",
               "Procedure: Return the given curve's range\n     Usage: get-range <curve>",
               SS_sargs,
               _ULI_crv_range, SS_PR_PROC);

    SS_install(si, "get-number-points",
               "Procedure: Return the given curve's number of points\n     Usage: get-number-points <curve>",
               SS_sargs,
               _ULI_crv_npts, SS_PR_PROC);

    SS_install(si, "get-attributes",
               "Procedure: Return the given curve's attributes, (color width style)\n     Usage: get-attributes <curve>",
               SS_sargs,
               _ULI_crv_attr, SS_PR_PROC);



    SS_install_cf(si, "fft",
                  "Procedure: Compute Fast Fourier Transform of double curve. Return double and imaginary parts.\n     Usage: fft <curve>",
                  _ULI_uc, 
                  _ULI_fft);
    SS_install_cf(si, "getx",
                  "Procedure: Return x values for a given y\n     Usage: getx <curve-list> <value>",
                  _ULI_bltocnp, 
                  _ULI_getx);
    SS_install_cf(si, "gety",
                  "Procedure: Return y values for a given x\n     Usage: gety <curve-list> <value>",
                  _ULI_bltocnp, 
                  _ULI_gety);
    SS_install_cf(si, "fit",
                  "Procedure: Find least-squares fit to the specified curves for a polynomial of order n\n     Usage: fit <curve-list> n",
                  _ULI_bltocnp, 
                  _ULI_fit);
    SS_install_cf(si, "dupx",
                  "Procedure: Duplicate x values so that y = x for each of the specified curves\n     Usage: dupx <curve-list>",
                  _ULI_uc, 
                  _ULI_dupx);
    SS_install_cf(si, "disp",
                  "Procedure: Display actual values in specified curves between min and max points\n     Usage: disp <curve-list> <xmin> <xmax>",
                  _ULI_ul2tocnp, 
                  _ULI_disp);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

