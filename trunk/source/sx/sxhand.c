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
typedef PM_mapping *(*PF_PPM_mapping_1)(PM_mapping *f, object *argl);
typedef PM_mapping *(*PF_PPM_mapping_2)(PM_mapping *f, object **argl);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_HAVE_DISPLAY_LIST - return TRUE iff PDBView function display-mapping*
 *                      - has been defined
 *                      - this is taken as condition for
 *                      - plotting mappings made by the handlers
 */

int SX_have_display_list(void)
   {object *var, *pl;
    int plf;

    plf = FALSE;

    var = (object *) SC_hasharr_def_lookup(SS_symtab, "display-mapping*");
    if (var != NULL)
       {pl  = SS_lk_var_val(var, SS_Env);
	plf = SS_procedurep(pl);};

    return(plf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_DISPLAY_MAP - do a display-mapping* on the mapping object MO */

object *SX_display_map(object *mo)
   {PM_mapping *h;
    PG_graph *g;

    if (SX_plot_flag && SX_autoplot)
       {h  = SS_GET(PM_mapping, mo);
	g  = PG_make_graph_from_mapping(h,
					SC_PCONS_P_S, NULL,
					1, NULL);
	mo = SX_mk_graph(g);

	SS_MARK(mo);
	SS_call_scheme("display-mapping*",
		       SS_OBJECT_I, mo,
		       0);};

    return(mo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RESOLVE_MAPPING - resolve the first thing on the given list into
 *                     - something that can be tested as a mapping
 */

static object *_SX_resolve_mapping(object *argl)
   {object *obj, *fo, *var, *fnc;

    SS_Save(SS_Argl);

    obj = SS_car(argl);
    fo  = obj;

    var = (object *) SC_hasharr_def_lookup(SS_symtab, "map-resolve");
    if (var != NULL)
       {fnc = SS_lk_var_val(var, SS_Env);
	if (SS_procedurep(fnc))

/* NOTE: the way this is called argl will end up in SS_This and will be
 * freed if this path is taken, so add the mark that tells it that
 * someone else it pointing at it
 */
	   {SS_MARK(argl);

	    fo = SS_call_scheme("map-resolve",
				SS_OBJECT_I, obj,
				0);};};

    SS_Restore(SS_Argl);

    return(fo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_DETERMINE_DRW_OBJ - extract a PM_mapping from the list
 *                      - and cdr the list
 *                      - if item is a graph extract the mapping from it
 *                      - if there is an interpretative function to resolve
 *                      - an object into a mapping call it
 */

void SX_determine_drw_obj(PM_mapping **pf, object **po, object **pargl)
   {PM_mapping *f;
    PG_graph *g;
    object *argl, *obj;

    argl = *pargl;

    obj  = _SX_resolve_mapping(argl);
    argl = SS_cdr(argl);

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

void SX_determine_mapping(PM_mapping **pf, object **pargl)
   {object *obj;

    SX_determine_drw_obj(pf, &obj, pargl);
/*
    if (*pf == NULL)
       SS_error("BAD MAPPING OR GRAPH - SX_DETERMINE_MAPPING",
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

    strcpy(type, t);
    SC_strtok(type, " *", s);
    did = SC_type_id(type, FALSE);

/* make sure that we have a working array */
    if (w == NULL)
       {w   = FMAKE_N(double, n, "_SX_UNOP:w");
	rel = TRUE;}
    else
       rel = FALSE;

/* fill the working array with double versions of the contents of D */
    PM_array_real(type, d, n, w);

/* operate on the data */
    for (i = 0; i < n; i++)
        w[i] = (*fn)(w[i]);

/* convert the data back to the correct type in D */
    SC_convert_id(did, d, 0, SC_DOUBLE_I, w, 0, n, FALSE);

    if (rel)
       SFREE(w);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MH_U_S - SX Math Handler for Unary Scalar operators
 *            - first cut will be to apply operator to scalar elements
 *            - of the range set of the given list of mappings
 *            - or elements of set
 *            - or elements of C_array
 */

object *_SX_mh_u_s(C_procedure *cp, object *argl)
   {object *ret, *first, *obj;
    int id, ne, nde;
    char *type;
    double **elem, *pd, *d;
    PFDoubleR fn;
    PM_set *set;

    SX_plot_flag = TRUE;

    ret   = SS_null;
    first = _SX_resolve_mapping(argl);
    if (SS_numbp(first))
       ret = SS_unary_flt(cp, argl);

    else
       {fn = (PFDoubleR) cp->proc[0];
	SS_Assign(ret, argl);

	while (SS_consp(argl))
	   {obj  = _SX_resolve_mapping(argl);
	    argl = SS_cdr(argl);
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
	       SS_error("BAD TYPE OBJECT - _SX_MH_U_S", obj);

	    if (set != NULL)
	       {type = set->element_type;
		ne   = set->n_elements;
		nde  = set->dimension_elem;
		elem = (double **) set->elements;
		d    = FMAKE_N(double, ne, "_SX_MH_U_S:d");
		for (id = 0; id < nde; id++)
		    {pd = elem[id];
		     _SX_unop(fn, type, (void *) pd, ne, d);};

		SFREE(d);
		PM_find_extrema(set);};};

	SX_prep_ret(ret);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_M11_X - math handler applies the given unary function to the x values
 *           - specific to mappings with 1d ranges and domains
 *           - this was UL_uopxc in ULTRA II
 */

object *_SX_m11_x(C_procedure *cp, object *argl)
   {int i, n;
    double *xp;
    PFDoubleR fn;
    object *obj, *ret;
    PM_set *set;
    PM_mapping *f;

    SX_plot_flag = TRUE;

    ret = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(argl);
        SX_determine_mapping(&f, &argl);
        if (f != NULL)
           {set = f->domain;
            xp = *(double **) set->elements;
            n  = set->n_elements;
            fn = (PFDoubleR) cp->proc[0];
            for (i = 0; i < n; xp++, i++)
                *xp = (double) (*fn)(*xp);

/* for later
            SX_dataset[j].modified = TRUE;
 */

            SS_Assign(ret, SS_mk_cons(obj, ret));
            PM_find_extrema(set);};};
         
    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_M11_B_MRO - performs unary operations on range values of mappings
 *               - using a single object
 */

object *_SX_m11_b_mro(C_procedure *cp, object *argl)
   {PM_mapping *f;
    PF_PPM_mapping_1 op;
    object *al, *obj, *ret;
        
    SX_plot_flag = TRUE;

    op   = (PF_PPM_mapping_1) cp->proc[0];
    al   = SS_car(argl);
    argl = SS_cdr(argl);

    ret = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(argl);
        SX_determine_mapping(&f, &argl);
        if (f != NULL)
           (*op)(f, al);

	SS_Assign(ret, SS_mk_cons(obj, ret));};
         
    SX_prep_ret(ret);

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
	 for (i = 0; i < n; yp++, i++)
	     *yp = (double) (*fnc)(*yp, a);};

    PM_find_extrema(set);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_M11_B_MRS - performs binary operations on range values of mappings
 *               - using a single scalar
 */

object *_SX_m11_b_mrs(C_procedure *cp, object *argl)
   {double a;
    PM_set *set;
    PM_mapping *f;
    object *obj, *ret;
        
    SX_plot_flag = TRUE;

    a = HUGE;
    SS_args(argl,
	    SC_DOUBLE_I, &a,
	    0);
    if (a == HUGE)
       SS_error("BAD NUMBER - SX_M11_B_MDS ", argl);

    argl = SS_cdr(argl);
    ret  = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(argl);
        SX_determine_mapping(&f, &argl);
        if (f != NULL)
           {set = f->range;
	    _SX_cmp_b_set(cp->proc[0], set, a, -1);

/* for later
            SX_dataset[j].modified = TRUE;
 */
            SS_Assign(ret, SS_mk_cons(obj, ret));
            PM_find_extrema(set);};};
         
    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_M11_B_MDS - performs binary operations on domains values of mappings
 *               - using a single scalar
 */

object *_SX_m11_b_mds(C_procedure *cp, object *argl)
   {double a;
    object *obj, *ret;
    PM_set *set;
    PM_mapping *f;
        
    SX_plot_flag = TRUE;

    a = HUGE;
    SS_args(argl,
	    SC_DOUBLE_I, &a,
	    0);
    if (a == HUGE)
       SS_error("BAD NUMBER - SX_M11_B_MDS ", argl);

    argl = SS_cdr(argl);
    ret  = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(argl);
        SX_determine_mapping(&f, &argl);
        if (f != NULL)
           {set = f->domain;
	    _SX_cmp_b_set(cp->proc[0], set, a, -1);

/* for later
            SX_dataset[j].modified = TRUE;
 */
            SS_Assign(ret, SS_mk_cons(obj, ret));
            PM_find_extrema(set);};};
         
    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MIJ_B_MRS - performs binary operations on range values of mappings
 *               - using a single scalar
 *               - the component(s) is specified
 */

object *_SX_mij_b_mrs(C_procedure *cp, object *argl)
   {int i;
    double a;
    PM_set *set;
    PM_mapping *f;
    object *obj, *ret;
        
    SX_plot_flag = TRUE;

    i = -1;
    a = HUGE;
    SS_args(argl,
	    SC_INT_I, &i,
	    SC_DOUBLE_I, &a,
	    0);
    if (a == HUGE)
       SS_error("BAD NUMBER - SX_MIJ_B_MDS ", argl);

/* NOTE: user specifies 1 for the first component so shitf */
    i--;

    argl = SS_cdr(argl);
    ret  = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(argl);
        SX_determine_mapping(&f, &argl);
        if (f != NULL)
           {set = f->range;
	    _SX_cmp_b_set(cp->proc[0], set, a, i);

/* for later
            SX_dataset[j].modified = TRUE;
 */
            SS_Assign(ret, SS_mk_cons(obj, ret));
            PM_find_extrema(set);};};
         
    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MIJ_B_MDS - performs binary operations on domains values of mappings
 *               - using a single scalar
 *               - the component(s) is specified
 */

object *_SX_mij_b_mds(C_procedure *cp, object *argl)
   {int i;
    double a;
    object *obj, *ret;
    PM_set *set;
    PM_mapping *f;
        
    SX_plot_flag = TRUE;

    i = -1;
    a = HUGE;
    SS_args(argl,
	    SC_INT_I, &i,
	    SC_DOUBLE_I, &a,
	    0);
    if (a == HUGE)
       SS_error("BAD NUMBER - SX_MIJ_B_MDS ", argl);

/* NOTE: user specifies 1 for the first component so shitf */
    i--;

    argl = SS_cdr(argl);
    ret  = SS_null;
    while (SS_consp(argl))
       {obj = SS_car(argl);
        SX_determine_mapping(&f, &argl);
        if (f != NULL)
           {set = f->domain;
	    _SX_cmp_b_set(cp->proc[0], set, a, i);

/* for later
            SX_dataset[j].modified = TRUE;
 */
            SS_Assign(ret, SS_mk_cons(obj, ret));
            PM_find_extrema(set);};};
         
    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MH_U_M - handler for unary operation acting on a set of mappings */

object *_SX_mh_u_m(C_procedure *cp, object *argl)
   {int plf;
    PM_mapping *f, *h;
    PF_PPM_mapping_2 op;
    object *ret, *mo;

    SX_plot_flag = TRUE;

    op  = (PF_PPM_mapping_2) cp->proc[0];
    plf = SX_have_display_list();
    ret = SS_null;
    while (SS_consp(argl))
       {SX_determine_mapping(&f, &argl);
        if (f != NULL)
            {h  = (*op)(f, &argl);
	     mo = SX_mk_mapping(h);

	     if (plf)
	        mo = SX_display_map(mo);

	     SS_Assign(ret, SS_mk_cons(mo, ret));};};
         
    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MF_INSTALL - install SX math functions in the SX function table
 *               - a separate function table is necessary to tell SCHEME
 *               - which version of "sin" to use, for example
 */

void SX_mf_install(void)
   {

    _SX_mf_inst_g();

    SS_install("hyper-plane",
               "Generate a hyper-plane sum(i=0,n; ci*xi)\nFORM hyper-plane c0 (c1 x1min x1max np1) ...)",
               SS_nargs,
	       SX_plane, SS_PR_PROC);

/* Functions under the _SX_MH_U_S handler */

    SS_install("log10",
               "Math base 10 log of range values\nFORM log10 <graph-list>",
               _SX_mh_u_s,
	       PM_log, SS_PR_PROC);
    SS_install("exp",
               "Math exponential of range values y=e^y\nFORM exp <graph-list>",
               _SX_mh_u_s,
	       exp, SS_PR_PROC);
    SS_install("random",
               "Generates random range values between -1 and 1 for the curve\nFORM random <graph-list>",
               _SX_mh_u_s,
	       PM_random, SS_PR_PROC);
    SS_install("cos",
               "Math cosine of y \nFORM cos <graph-list>",
               _SX_mh_u_s,
	       cos, SS_PR_PROC);
    SS_install("acos",
               "Math ArcCos of y\nFORM acos <graph-list>",
               _SX_mh_u_s,
	       acos, SS_PR_PROC);
    SS_install("cosh",
               "Math hyperbolic cosine of y\nFORM cosh <graph-list>",
               _SX_mh_u_s,
	       cosh, SS_PR_PROC);
    SS_install("sin",
               "Math sine of y\nFORM sin <graph-list>",
               _SX_mh_u_s,
	       sin, SS_PR_PROC);
    SS_install("asin",
               "Math ArcSin of y\nFORM asin <graph-list>",
               _SX_mh_u_s,
	       asin, SS_PR_PROC);
    SS_install("sinh",
               "Math hyperbolic sine of y\nFORM sinh <graph-list>",
               _SX_mh_u_s,
	       sinh, SS_PR_PROC);
    SS_install("tan",
               "Math tangent of range values\nFORM tan <graph-list>",
               _SX_mh_u_s,
	       tan, SS_PR_PROC);
    SS_install("atan",
               "Math ArcTan of range values\nFORM atan <graph-list>",
               _SX_mh_u_s,
	       atan, SS_PR_PROC);
    SS_install("tanh",
               "Math hyperbolic tangent on range values\nFORM tanh <graph-list>",
               _SX_mh_u_s,
	       tanh, SS_PR_PROC);

    SS_install("i0",
               "Math zero-th order modified Bessel function of the first kind on range values\nFORM i0 <graph-list>",
               _SX_mh_u_s,
	       PM_i0, SS_PR_PROC);
    SS_install("i1",
               "Math first order modified Bessel function of the first kind on range values\nFORM i1 <graph-list>",
               _SX_mh_u_s,
	       PM_i1, SS_PR_PROC);

    SS_install("j0",
               "Math zero-th order Bessel function of the first kind on range values\nFORM j0 <graph-list>",
               _SX_mh_u_s,
	       PM_j0, SS_PR_PROC);
    SS_install("j1",
               "Math first order Bessel function of the first kind on range values\nFORM j1 <graph-list>",
               _SX_mh_u_s,
	       PM_j1, SS_PR_PROC);

    SS_install("k0",
               "Math zero-th order modified Bessel function of the second kind on range values\nFORM k0 <graph-list>",
               _SX_mh_u_s,
	       PM_k0, SS_PR_PROC);
    SS_install("k1",
               "Math first order modified Bessel function of the second kind on range values\nFORM k1 <graph-list>",
               _SX_mh_u_s,
	       PM_k1, SS_PR_PROC);

    SS_install("y0",
               "Math zero-th order Bessel function of the second kind on range values\nFORM y0 <graph-list>",
               _SX_mh_u_s,
	       PM_y0, SS_PR_PROC);
    SS_install("y1",
               "Math first order Bessel function of the second kind on range values\nFORM y1 <graph-list>",
               _SX_mh_u_s,
	       PM_y1, SS_PR_PROC);

    SS_install("tchn",
               "Math nth order Tchebyshev function on range values\nFORM tchn <graph-list> <n>",
               _SX_mh_u_s,
	       PM_tchn, SS_PR_PROC);
    SS_install("sqr",
               "Math square of range values\nFORM sqr <graph-list>",
               _SX_mh_u_s,
	       PM_sqr, SS_PR_PROC);
    SS_install("ln",
               "Math natural log of range values\nFORM ln <graph-list>",
               _SX_mh_u_s,
	       PM_ln, SS_PR_PROC);
    SS_install("sqrt",
               "Math square root of range values\nFORM sqrt <graph-list>",
               _SX_mh_u_s,
	       PM_sqrt, SS_PR_PROC);
    SS_install("recip",
               "Math reciprocal of y\nFORM recip <graph-list>",
               _SX_mh_u_s,
	       PM_recip, SS_PR_PROC);

    SS_install("abs",
               "Math absolute value of range values\nFORM abs <graph-list>",
               _SX_mh_u_s,
	       ABS, SS_PR_PROC);

/* Functions under the _SX_MH_U_M handler */

    SS_install("refine",
               "Refine the domain of a mapping\nFORM refine [<mapping> '((<n-pts1> <n-pts2> ...)]*",
               _SX_mh_u_m,
	       _SXI_refine_mapping, SS_PR_PROC);
    SS_install("interpolation-weights",
               "Find the interpolation weights for the given mapping\nFORM interpolation-weights [<mapping> '((<n-pts1> <n-pts2> ...)]*",
               _SX_mh_u_m,
	       _SXI_interp_mapping, SS_PR_PROC);

/* Functions under the _SX_MH_B_S handler */

    SS_install("m+",
               "Math sum of mappings\nFORM m+ <graph-list>",
               _SX_mh_b_s,
	       PM_fplus, SS_PR_PROC);
    SS_install("m-",
               "Math difference of mappings\nFORM m- <graph-list>",
               _SX_mh_b_s,
	       PM_fminus, SS_PR_PROC);
    SS_install("m*",
               "Math product of mappings\nFORM m* <graph-list>",
               _SX_mh_b_s,
	       PM_ftimes, SS_PR_PROC);
    SS_install("m/",
               "Math quotient of mappings\nFORM m/ <a> <b>",
               _SX_mh_b_s,
	       PM_fdivide, SS_PR_PROC);
    SS_install("m==",
               "Math equal element mappings\nFORM m== <graph-list>",
               _SX_mh_b_s,
	       PM_ffeq, SS_PR_PROC);
    SS_install("m!=",
               "Math not equal element mappings\nFORM m!= <graph-list>",
               _SX_mh_b_s,
	       PM_ffneq, SS_PR_PROC);
    SS_install("m<",
               "Math less than element mappings\nFORM m< <graph-list>",
               _SX_mh_b_s,
	       PM_fflt, SS_PR_PROC);
    SS_install("m<=",
               "Math less than or equal element mappings\nFORM m<= <graph-list>",
               _SX_mh_b_s,
	       PM_ffle, SS_PR_PROC);
    SS_install("m>",
               "Math greater than element mappings\nFORM m> <graph-list>",
               _SX_mh_b_s,
	       PM_ffgt, SS_PR_PROC);
    SS_install("m>=",
               "Math greater than or equal element mappings\nFORM m>= <graph-list>",
               _SX_mh_b_s,
	       PM_ffge, SS_PR_PROC);
    SS_install("hypot",
               "Math calculates sqrt(a^2+b^2)\nFORM hypot <a> <b>",
               _SX_mh_b_s,
	       HYPOT, SS_PR_PROC);

/* Functions under the _SX_M11_X handler */

    SS_install("lnx",
               "Math natural log of domain values\nFORM lnx <graph-list>",
               _SX_m11_x,
	       PM_ln, SS_PR_PROC);
    SS_install("log10x",
               "Math base 10 log of domain values\nFORM log10x <graph-list>",
               _SX_m11_x,
	       PM_log, SS_PR_PROC);
    SS_install("expx",
               "Math exponential of domain values x=e^x\nFORM expx <graph-list>",
               _SX_m11_x,
	       exp, SS_PR_PROC);
    SS_install("sqrtx",
               "Math square root of domain values\nFORM sqrtx <graph-list>",
               _SX_m11_x,
	       PM_sqrt, SS_PR_PROC);
    SS_install("sqrx",
               "Math square of domain values\nFORM sqrx <graph-list>",
               _SX_m11_x,
	       PM_sqr, SS_PR_PROC);
    SS_install("absx",
               "Math absolute value of x\nFORM absx <graph-list>",
               _SX_m11_x,
	       ABS, SS_PR_PROC);
    SS_install("recipx",
               "Math reciprocal of x\nFORM recipx <graph-list>",
               _SX_m11_x,
	       PM_recip, SS_PR_PROC);
    SS_install("cosx",
               "Math cosine of x\nFORM cosx <graph-list>",
               _SX_m11_x,
	       cos, SS_PR_PROC);
    SS_install("acosx",
               "Math ArcCos of x\nFORM acosx <graph-list>",
               _SX_m11_x,
	       acos, SS_PR_PROC);
    SS_install("coshx",
               "Math hyperbolic cosine of x\nFORM coshx <graph-list>",
               _SX_m11_x,
	       cosh, SS_PR_PROC);
    SS_install("sinx",
               "Math sine of x\nFORM sinx <graph-list>",
               _SX_m11_x,
	       sin, SS_PR_PROC);
    SS_install("asinx",
               "Math ArcSin of x\nFORM asinx <graph-list>",
               _SX_m11_x,
	       asin, SS_PR_PROC);
    SS_install("sinhx",
               "Math hyperbolic sine of x\nFORM sinhx <graph-list>",
               _SX_m11_x,
	       sinh, SS_PR_PROC);
    SS_install("tanx",
               "Math tangent of domain values\nFORM tanx <graph-list>",
               _SX_m11_x,
	       tan, SS_PR_PROC);
    SS_install("atanx",
               "Math ArcTan of domain values\nFORM atanx <graph-list>",
               _SX_m11_x,
	       atan, SS_PR_PROC);
    SS_install("tanhx",
               "Math hyperbolic tangent on domain values\nFORM tanhx <graph-list>",
               _SX_m11_x,
	       tanh, SS_PR_PROC);

    SS_install("i0x",
               "Math zero-th order modified Bessel function of the first kind on domain values\nFORM i0x <graph-list>",
               _SX_m11_x,
	       PM_i0, SS_PR_PROC);
    SS_install("i1x",
               "Math first order modified Bessel function of the first kind on domain values\nFORM i1x <graph-list>",
               _SX_m11_x,
	       PM_i1, SS_PR_PROC);
    SS_install("inx",
               "Math nth order modified Bessel function of the first kind on domain values\nFORM inx <graph-list> <n>",
               _SX_m11_b_mds,
	       PM_in, SS_PR_PROC);

    SS_install("j0x",
               "Math zero-th order Bessel function of the first kind on domain values\nFORM j0x <graph-list>",
               _SX_m11_x,
	       PM_j0, SS_PR_PROC);
    SS_install("j1x",
               "Math first order Bessel function of the first kind on domain values\nFORM j1x <graph-list>",
               _SX_m11_x,
	       PM_j1, SS_PR_PROC);
    SS_install("jnx",
               "Math nth order Bessel function of the first kind on domain values\nFORM jnx <graph-list> <n>",
               _SX_m11_b_mds,
	       PM_jn, SS_PR_PROC);

    SS_install("k0x",
               "Math zero-th order modified Bessel function of the second kind on domain values\nFORM k0x <graph-list>",
               _SX_m11_x,
	       PM_k0, SS_PR_PROC);
    SS_install("k1x",
               "Math first order modified Bessel function of the second kind on domain values\nFORM k1x <graph-list>",
               _SX_m11_x,
	       PM_k1, SS_PR_PROC);
    SS_install("knx",
               "Math nth order modified Bessel function of the second kind on domain values\nFORM knx <graph-list> <n>",
               _SX_m11_b_mds,
	       PM_kn, SS_PR_PROC);

    SS_install("ynx",
               "Math nth order Bessel function of the second kind on domain values\nFORM yn <graph-list> <n>",
               _SX_m11_b_mds,
	       PM_yn, SS_PR_PROC);
    SS_install("y0x",
               "Math zero-th order Bessel function of the second kind on domain values\nFORM y0x <graph-list>",
               _SX_m11_x,
	       PM_y0, SS_PR_PROC);
    SS_install("y1x",
               "Math first order Bessel function of the second kind on domain values\nFORM y1x <graph-list>",
               _SX_m11_x,
	       PM_y1, SS_PR_PROC);


/* _SX_M11_B_MDS and _SX_M11_B_MRS functions */

    SS_install("powrx",
               "Math raise domain values to a power x=x^a\nFORM powrx <a> <graph-list>",
               _SX_m11_b_mds,
	       POW, SS_PR_PROC);
    SS_install("powx",
               "Math raise domain values to a power x=x^a\nFORM powx <a> <graph-list>",
               _SX_m11_b_mds,
	       POW, SS_PR_PROC);
    SS_install("powax",
                "Math raise a to the power of the x value\nx=a^x\nFORM powax <a> <graph-list>",
               _SX_m11_b_mds,
	       PM_pow, SS_PR_PROC);
    SS_install("powr",
               "Math raise range values to a power y=y^a\nFORM powr <a> <graph-list>",
               _SX_m11_b_mrs,
	       POW, SS_PR_PROC);
    SS_install("pow",
               "Math raise range values to a power y=y^a\nFORM pow <a> <graph-list>",
               _SX_m11_b_mrs,
	       POW, SS_PR_PROC);
    SS_install("powa",
               "Math raise a to the power of the y value\ny=a^y\nFORM powa <a> <graph-list>",
               _SX_m11_b_mrs,
	       PM_pow, SS_PR_PROC);

    SS_install("jn",
               "Math nth order Bessel function of the first kind on range values\nFORM jn <n> <graph-list>",
               _SX_m11_b_mrs,
	       PM_jn, SS_PR_PROC);
    SS_install("yn",
               "Math nth order Bessel function of the second kind on range values\nFORM yn <n> <graph-list>",
               _SX_m11_b_mrs,
	       PM_yn, SS_PR_PROC);
    SS_install("in",
               "Math nth order modified Bessel function of the first kind on range values\nFORM in <n> <graph-list>",
               _SX_m11_b_mrs,
	       PM_in, SS_PR_PROC);
    SS_install("kn",
               "Math nth order modified Bessel function of the second kind on range values\nFORM kn <n> <graph-list>",
               _SX_m11_b_mrs,
	       PM_in, SS_PR_PROC);

    SS_install("dx",
               "Shift domain values of curve by a constant\nFORM dx <value> <graph-list>",
               _SX_m11_b_mds,
	       PM_fplus, SS_PR_PROC);
    SS_install("dy",
               "Shift range values of curve by a constant\nFORM dy <value> <graph-list>",
               _SX_m11_b_mrs,
	       PM_fplus, SS_PR_PROC);
    SS_install("mx",
               "Scale domain values of curve by a constant\nFORM mx <value> <graph-list>",
               _SX_m11_b_mds,
	       PM_ftimes, SS_PR_PROC);
    SS_install("my",
               "Scale range values of curve by a constant\nFORM my <value> <graph-list>",
               _SX_m11_b_mrs,
	       PM_ftimes, SS_PR_PROC);
    SS_install("divx",
               "Divide domain values by a constant\nFORM divx <value> <graph-list>",
               _SX_m11_b_mds,
	       PM_fdivide, SS_PR_PROC);
    SS_install("divy",
               "Divide range values by a constant\nFORM divy <value> <graph-list>",
               _SX_m11_b_mrs,
	       PM_fdivide, SS_PR_PROC);

/* _SX_MIJ_B_MDS and _SX_MIJ_B_MRS functions */

    SS_install("powrxc",
               "Math raise domain component values to a power x=x^a\nFORM powrxc <cmp> <a> <graph-list>",
               _SX_mij_b_mds,
	       POW, SS_PR_PROC);
    SS_install("powxc",
               "Math raise domain component values to a power x=x^a\nFORM powxc <cmp> <a> <graph-list>",
               _SX_mij_b_mds,
	       POW, SS_PR_PROC);
    SS_install("powaxc",
                "Math raise a to the power of the x value\nx=a^x\nFORM powaxc <cmp> <a> <graph-list>",
               _SX_mij_b_mds,
	       PM_pow, SS_PR_PROC);
    SS_install("powrc",
               "Math raise range component values to a power y=y^a\nFORM powrc <cmp> <a> <graph-list>",
               _SX_mij_b_mrs,
	       POW, SS_PR_PROC);
    SS_install("powc",
               "Math raise range component values to a power y=y^a\nFORM powc <cmp> <a> <graph-list>",
               _SX_mij_b_mrs,
	       POW, SS_PR_PROC);
    SS_install("powac",
               "Math raise a to the power of the y value\ny=a^y\nFORM powac <cmp> <a> <graph-list>",
               _SX_mij_b_mrs,
	       PM_pow, SS_PR_PROC);
    SS_install("jnc",
               "Math nth order Bessel function of the first kind on range component values\nFORM jnc <cmp> <n> <graph-list>",
               _SX_mij_b_mrs,
	       PM_jn, SS_PR_PROC);
    SS_install("ync",
               "Math nth order Bessel function of the second kind on range component values\nFORM ync <cmp> <n> <graph-list>",
               _SX_mij_b_mrs,
	       PM_yn, SS_PR_PROC);
    SS_install("dxc",
               "Shift domain component values of curve by a constant\nFORM dxc <cmp> <value> <graph-list>",
               _SX_mij_b_mds,
	       PM_fplus, SS_PR_PROC);
    SS_install("dyc",
               "Shift range component values of curve by a constant\nFORM dyc <cmp> <value> <graph-list>",
               _SX_mij_b_mrs,
	       PM_fplus, SS_PR_PROC);
    SS_install("mxc",
               "Scale domain component values of curve by a constant\nFORM mxc <cmp> <value> <graph-list>",
               _SX_mij_b_mds,
	       PM_ftimes, SS_PR_PROC);
    SS_install("myc",
               "Scale range component values of curve by a constant\nFORM myc <cmp> <value> <graph-list>",
               _SX_mij_b_mrs,
	       PM_ftimes, SS_PR_PROC);
    SS_install("divxc",
               "Divide domain component values by a constant\nFORM divxc <cmp> <value> <graph-list>",
               _SX_mij_b_mds,
	       PM_fdivide, SS_PR_PROC);
    SS_install("divyc",
               "Divide range component values by a constant\nFORM divyc <cmp> <value> <graph-list>",
               _SX_mij_b_mrs,
	       PM_fdivide, SS_PR_PROC);

/* _SX_M11_B_MRO functions */

    SS_install("xmm",
               "Excerpts a part of a mapping\nFORM xmm (<low-lim1> <high-lim1> ...) <graph-list>",
               _SX_m11_b_mro,
	       _SXI_extract_mapping, SS_PR_PROC);

#if 0

/* _SX_MH_U_M functions */

    SS_install("rev",
               "Swaps x and range values for a curve\nYou may want to sort after this\nFORM rev <graph-list>",
               _SX_mh_u_m,
	       reverse, SS_PR_PROC);
    SS_install("sort",
               "Sorts a curve into ascending order based on domain values\nFORM sort <graph-list>",
               _SX_mh_u_m,
	       sort, SS_PR_PROC);

/* BCXL functions */

    SS_install("compose",
               "Functional composition f(g(x))\nFORM compose <f> <g>",
               bcxl,
	       compose, SS_PR_PROC);

/* ULAUXF.C functions */
    SS_install("convolve",
               "Procedure: Convolve the two functions\n     Usage: convolve <curve> <curve> [<dt>]",
               SS_nargs,
               _ULI_convlv, SS_PR_PROC);

    SS_install("edit",
               "Procedure: Graphically edit a curve\n     Usage: edit <curve>",
               _ULI_uc,
               _ULI_edit, SS_PR_PROC);

    SS_install("error-bar",
               "Procedure: Do a scatter plot with error bars on the given curve\n     Usage: error-bar <curve> <y+curve> <y-curve> [<x+curve> [<x-curve>]]",
               SS_nargs,
               _ULI_error_plot, SS_PR_PROC);

    SS_install("get-label",
               "Procedure: Return the given curve's label\n     Usage: get-label <curve>",
               SS_sargs,
               _ULI_crv_label, SS_PR_PROC);

    SS_install("get-domain",
               "Procedure: Return the given curve's domain\n     Usage: get-domain <curve>",
               SS_sargs,
               _ULI_crv_domain, SS_PR_PROC);

    SS_install("get-range",
               "Procedure: Return the given curve's range\n     Usage: get-range <curve>",
               SS_sargs,
               _ULI_crv_range, SS_PR_PROC);

    SS_install("get-number-points",
               "Procedure: Return the given curve's number of points\n     Usage: get-number-points <curve>",
               SS_sargs,
               _ULI_crv_npts, SS_PR_PROC);

    SS_install("get-attributes",
               "Procedure: Return the given curve's attributes, (color width style)\n     Usage: get-attributes <curve>",
               SS_sargs,
               _ULI_crv_attr, SS_PR_PROC);



    SS_install_cf("fft",
                  "Procedure: Compute Fast Fourier Transform of double curve. Return double and imaginary parts.\n     Usage: fft <curve>",
                  _ULI_uc, 
                  _ULI_fft);
    SS_install_cf("getx",
                  "Procedure: Return x values for a given y\n     Usage: getx <curve-list> <value>",
                  _ULI_bltocnp, 
                  _ULI_getx);
    SS_install_cf("gety",
                  "Procedure: Return y values for a given x\n     Usage: gety <curve-list> <value>",
                  _ULI_bltocnp, 
                  _ULI_gety);
    SS_install_cf("fit",
                  "Procedure: Find least-squares fit to the specified curves for a polynomial of order n\n     Usage: fit <curve-list> n",
                  _ULI_bltocnp, 
                  _ULI_fit);
    SS_install_cf("dupx",
                  "Procedure: Duplicate x values so that y = x for each of the specified curves\n     Usage: dupx <curve-list>",
                  _ULI_uc, 
                  _ULI_dupx);
    SS_install_cf("disp",
                  "Procedure: Display actual values in specified curves between min and max points\n     Usage: disp <curve-list> <xmin> <xmax>",
                  _ULI_ul2tocnp, 
                  _ULI_disp);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

