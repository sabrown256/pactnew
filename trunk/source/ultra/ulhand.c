/*
 * ULHAND.C - handlers for Ultra
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "ultra.h"

typedef object *(*PFSargs)(SS_psides *si, object *argl);
typedef object *(*PFSargsI)(SS_psides *si, int j);
typedef double (*PFDoubleR)(double x);
typedef double (*PFDoubleRR)(double x, double y);
typedef double (*PFDoubleRi)(double x, int i);
typedef void (*PFVoidis)(int j, char *s);
typedef object *(*PFPObjectidd)(SS_psides *si, int j, double d1, double d2);
typedef object *(*PFPObjectio)(SS_psides *si, int j, object *argl);
typedef object *(*PFPObjectoo)(SS_psides *si, object *s, object *tok);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_LMT - find the min and max of a double array */

void UL_lmt(double *pt, int n, double *mymin, double *mymax)
   {double *p;
    int i;

    *mymin =  HUGE;
    *mymax = -HUGE;
    for (p = pt, i = 0; i < n; p++, i++)
        {*mymin = min(*mymin, *p);
         *mymax = max(*mymax, *p);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_CURVE_STRP - return TRUE iff the given object is a string which
 *               - designates an active curve
 *               - used in building curve based procedures
 */

int UL_curve_strp(object *obj)
   {int rv;

    rv = FALSE;
    if (SS_stringp(obj))
       {if (SX_curvep(SS_STRING_TEXT(obj)))
           rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_CHECK_ORDER - check that the array is strictly increasing
 *                - or do a full sort
 */

void UL_check_order(SS_psides *si, double *p, int n, int i)
   {int j;
    double *p1, *p2;

    for (p1 = p, p2 = p+1, j = 1; j < n; p1++, p2++, j++)
        {if (*p1 > *p2)                             /* sort if out of order */
            {if (si->interactive == ON)
	        PRINT(stdout, "\nSorting curve %s\n", _UL_id_str(i, i));

             UL_sort(si, i);
             break;};};

    return;}
                        
/*--------------------------------------------------------------------------*/

/*                            FUNCTION HANDLERS                             */

/*--------------------------------------------------------------------------*/

/* UL_US - handler for unary operation acting on a set of curves
 *       - this handler passes a Scheme object to its functions and
 *       - expects Scheme objects back from them
 */

object *UL_us(SS_psides *si, C_procedure *cp, object *argl)
   {object *ret, *val, *t;
    PFSargs fun;

    fun = (PFSargs) cp->proc[0];
    SX_prep_arg(si, argl);

/* set plot flag on so that for example (select (menu)) causes replot */
    SX_gs.plot_flag = TRUE;

    ret = SS_null;
    for (t = argl; !SS_nullobjp(t); t = SS_cdr(si, t))
        {val = fun(si, SS_car(si, t));
         SS_assign(si, ret, SS_mk_cons(si, val, ret));};

    SS_assign(si, argl, SS_null);

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_UC - handler for unary operation acting on a set of curves
 *       - these funtions expect an integer curve index and
 *       - return a valid Scheme object
 */

object *UL_uc(SS_psides *si, C_procedure *cp, object *argl)
   {int i;
    object *ret, *t;
    PFSargsI fun;

    fun = (PFSargsI) cp->proc[0];

    SX_prep_arg(si, argl);

/* set plot flag on so that for example (copy (lst)) causes replot */
    SX_gs.plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; !SS_nullobjp(t); t = SS_cdr(si, t))
        {i = SX_get_crv_index_i(SS_car(si, t));
         if (i >= 0)
            {SS_assign(si, ret, SS_mk_cons(si, fun(si, i), ret));};};
         
    SS_assign(si, argl, SS_null);

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_OPXC - performs binary operations on x values of curves
 *         - using a single scalar
 */
    
object *UL_opxc(SS_psides *si, C_procedure *cp, object *argl)
   {object *tok, *ret, *t;
    double a;
    int i, l, n;
    double *xp;
    PFDoubleRR fun;

    fun = (PFDoubleRR) cp->proc[0];
        
    SX_last_arg(si, tok, argl);
    SX_prep_arg(si, argl);

    a = HUGE;
    SS_args(si, tok,
            SC_DOUBLE_I, &a,
            0);
    if (a == HUGE)
       SS_error(si, "BAD NUMBER - UL_OPXC ", tok);

    SS_assign(si, tok, SS_null);

/* set plot flag on so that for example (dx (lst)) causes replot */
    SX_gs.plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(si, t))
        {i = SX_get_crv_index_i(SS_car(si, t));
         if (i != -1)
            {n = SX_gs.dataset[i].n;
             for (xp = SX_gs.dataset[i].x[0], l = 0; l < n; xp++, l++)
                 *xp = fun(*xp, a);
             SX_gs.dataset[i].modified = TRUE;

             SS_assign(si, ret, SS_mk_cons(si, SX_gs.dataset[i].obj, ret));
             UL_lmt(SX_gs.dataset[i].x[0], n,
                    &SX_gs.dataset[i].wc[0],
                    &SX_gs.dataset[i].wc[1]);};};
         
    SS_assign(si, argl, SS_null);

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_OPYC - performs binary operations on y values of curves
 *         - using a single scalar
 */

object *UL_opyc(SS_psides *si, C_procedure *cp, object *argl)
   {object *tok, *ret, *t;
    double a;
    int i, l, n;
    double *yp;
    PFDoubleRR fun;

    fun = (PFDoubleRR) cp->proc[0];
        
    SX_last_arg(si, tok, argl);
    SX_prep_arg(si, argl);

    a = HUGE;
    SS_args(si, tok,
            SC_DOUBLE_I, &a,
            0);
    if (a == HUGE)
       SS_error(si, "BAD NUMBER - UL_OPYC ", tok);

    SS_assign(si, tok, SS_null);

/* set plot flag on so that for example (dy (lst)) causes replot */
    SX_gs.plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(si, t))
        {i = SX_get_crv_index_i(SS_car(si, t));
         if (i != -1)
            {n = SX_gs.dataset[i].n;
             for (yp = SX_gs.dataset[i].x[1], l = 0; l < n; yp++, l++)
                 *yp = fun(*yp, a);
             SX_gs.dataset[i].modified = TRUE;

             SS_assign(si, ret, SS_mk_cons(si, SX_gs.dataset[i].obj, ret));
             UL_lmt(SX_gs.dataset[i].x[1], n,
                    &SX_gs.dataset[i].wc[2],
                    &SX_gs.dataset[i].wc[3]);};};

    SS_assign(si, argl, SS_null);

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_UL2TOC - worker routine for UL_UL2TOC and UL_UL2TOCNP */

static object *_UL_ul2toc(SS_psides *si, C_procedure *cp,
			  object *argl, int replot_flag)
   {int i;
    double d1, d2;
    object *s, *t, *tok1, *tok2, *ret;
    PFPObjectidd fun;

    fun = (PFPObjectidd) cp->proc[0];

    tok1 = NULL;
    tok2 = NULL;

    SS_assign(si, argl, SS_reverse(si, argl));
    tok2 = SS_car(si, argl);
    if (SS_numbp(tok2))
       {SS_assign(si, argl, SS_cdr(si, argl));
	if (SS_consp(argl))
	   {tok1 = SS_car(si, argl);
	    if (SS_numbp(tok1))
	       {SS_assign(si, argl, SS_cdr(si, argl));}
	    else
	       {tok1 = tok2;
		tok2 = NULL;};}
	else
	   {tok1 = tok2;
	    tok2 = NULL;};}
    else
        tok2 = NULL;
    SS_assign(si, argl, SS_reverse(si, argl));

    SX_prep_arg(si, argl);

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(si, t))
        {s = SS_car(si, t);
         if (SX_curvep_a(s))
            {i = SX_get_crv_index_i(s);
	     if (tok1 != NULL)
	        SS_args(si, tok1,
			SC_DOUBLE_I, &d1,
			0);
	     else
	        d1 = SX_gs.dataset[i].wc[0];
	     if (tok2 != NULL)
	        SS_args(si, tok2,
			SC_DOUBLE_I, &d2,
			0);
	     else
	        d2 = SX_gs.dataset[i].wc[1];
             SS_assign(si, ret,
                       SS_mk_cons(si, fun(si, i, d1, d2), ret));};};

    SX_gs.plot_flag = replot_flag;

    SS_assign(si, argl, SS_null);

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_UL2TOC - unary applies last 2 args to curve */

object *UL_ul2toc(SS_psides *si, C_procedure *cp, object *argl)
   {object *o;

    o = _UL_ul2toc(si, cp, argl, TRUE);
   
    return(o);}
   
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_UL2TOCNP - unary applies last 2 args to curve
 *             - and does not replot
 */

object *UL_ul2tocnp(SS_psides *si, C_procedure *cp, object *argl)
   {object *o;

    o = _UL_ul2toc(si, cp, argl, FALSE);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_ULNTOC - unary applies last n args to curve */

object *UL_ulntoc(SS_psides *si, C_procedure *cp, object *argl)
   {int i;
    object *tok, *crvs, *ret, *t, *u;
    PFPObjectio fun;

    fun = (PFPObjectio) cp->proc[0];

    SX_prep_arg(si, argl);

    for (t = argl, crvs = SS_null; SS_consp(t); t = SS_cdr(si, t))
        {tok = SS_car(si, t);
         if (SX_curvep_a(tok))
            {SS_assign(si, crvs, SS_mk_cons(si, tok, crvs));}
         else
            break;};

/* set plot flag on so that for example (filter (lst) dp rp) causes replot */
    SX_gs.plot_flag = TRUE;

    for (ret = SS_null, u = crvs; SS_consp(u); u = SS_cdr(si, u))
        {tok = SS_car(si, u);
         if (SX_curvep_a(tok))
            {i = SX_get_crv_index_i(tok);
             SS_assign(si, ret,
                       SS_mk_cons(si, fun(si, i, t), ret));};};

    SS_assign(si, crvs, SS_null);
    SS_assign(si, argl, SS_null);
    SS_assign(si, ret, SS_reverse(si, ret));

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_UOPXC - math handler applies the given unary function to the x values */
    
object *UL_uopxc(SS_psides *si, C_procedure *cp, object *argl)
   {int i, l, n;
    double *xp;
    object *ret, *t;
    PFDoubleR fun;

    fun = (PFDoubleR) cp->proc[0];

    SX_prep_arg(si, argl);

/* set plot flag on so that for example (expx (lst)) causes replot */
    SX_gs.plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(si, t))
        {i = SX_get_crv_index_i(SS_car(si, t));
         if (i != -1)
            {n = SX_gs.dataset[i].n;
             for (xp = SX_gs.dataset[i].x[0], l = 0; l < n; xp++, l++)
                 *xp = fun(*xp);
             SX_gs.dataset[i].modified = TRUE;

             SS_assign(si, ret, SS_mk_cons(si, SX_gs.dataset[i].obj, ret));
             UL_lmt(SX_gs.dataset[i].x[0], n,
                    &SX_gs.dataset[i].wc[0],
                    &SX_gs.dataset[i].wc[1]);};};
         
    SS_assign(si, argl, SS_null);

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_UOPYC - math handler applies the given unary function to the y values */

object *UL_uopyc(SS_psides *si, C_procedure *cp, object *argl)
   {int i, l, n;
    double *yp, f;
    object *s, *ret, *tmp, *t;
    PFDoubleR fun;

    fun = (PFDoubleR) cp->proc[0];
	     
    SX_prep_arg(si, argl);

/* set plot flag on so that for example (expx (lst)) causes replot */
    SX_gs.plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(si, t))
        {s   = SS_car(si, t);
         i   = SX_get_crv_index_i(s);
	 tmp = SS_null;
         if (i != -1)
            {n = SX_gs.dataset[i].n;
             for (yp = SX_gs.dataset[i].x[1], l = 0; l < n; yp++, l++)
                 *yp = fun(*yp);
             SX_gs.dataset[i].modified = TRUE;

             tmp = SS_mk_cons(si, SX_gs.dataset[i].obj, ret);
             UL_lmt(SX_gs.dataset[i].x[1], n,
                    &SX_gs.dataset[i].wc[2], &SX_gs.dataset[i].wc[3]);}

         else if (SS_integerp(s))
	    {f   = (double) SS_INTEGER_VALUE(s);
             tmp = SS_mk_cons(si,
			      SS_mk_float(si, fun(f)),
			      ret);}

         else if (SS_floatp(s))
            {f   = SS_FLOAT_VALUE(s);
             tmp = SS_mk_cons(si,
			      SS_mk_float(si, fun(f)),
			      ret);};

         SS_assign(si, ret, tmp);};

    SS_assign(si, argl, SS_null);

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_BFTOC - unary operation on curves using first argument as
 *          - a parameter
 */

object *UL_bftoc(SS_psides *si, C_procedure *cp, object *argl)
   {int i;
    char *s1;
    object *s, *tok, *t;
    PFVoidis fun;

    fun = (PFVoidis) cp->proc[0];

    SX_prep_arg(si, argl);

/* set plot flag on so that for example (op param (lst)) causes replot */
    SX_gs.plot_flag = TRUE;

    tok  = SS_car(si, argl);
    argl = SS_cdr(si, argl);

    s1 = CSTRSAVE(SS_get_string(tok));

    for (t = argl ; SS_consp(t); t = SS_cdr(si, t))
        {s = SS_car(si, t);

         if (SX_curvep_a(s))
            {i = SX_get_crv_index_i(s);
	     fun(i, s1);};};

    CFREE(s1);

    SS_assign(si, argl, SS_null);

    return(tok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_BLTOC - unary operation applied to each curve using the last argument
 *          - as a parameter
 */

object *UL_bltoc(SS_psides *si, C_procedure *cp, object *argl)
   {object *s, *ret, *tok, *t;
    PFPObjectoo fun;

    fun = (PFPObjectoo) cp->proc[0];

    SX_last_arg(si, tok, argl);
    if (!SS_numbp(tok))
       SS_error(si, "BAD LAST ARGUMENT - UL_BLTOC", tok);

    SX_prep_arg(si, argl);

/* set plot flag on so that for example (color (lst) red) causes replot */
    SX_gs.plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(si, t))
        {s = SS_car(si, t);

         if (SX_curvep_a(s))
            SS_assign(si, ret, SS_mk_cons(si, fun(si, s, tok), ret));};

    SS_assign(si, tok, SS_null);
    SS_assign(si, argl, SS_null);

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_BLTOCNP - unary operation applied to each curve using the last argument
 *            - as a parameter
 *            - clears screen, prints name of function and
 *            - surpresses plotting
 */

object *UL_bltocnp(SS_psides *si, C_procedure *cp, object *argl)
   {char *r;
    object *s, *tok, *ret, *t;
    PFPObjectoo fun;

    fun = (PFPObjectoo) cp->proc[0];

    SX_last_arg(si, tok, argl);
    if (!SS_numbp(tok))
       SS_error(si, "BAD LAST ARGUMENT - BLTOCNP", tok);

    if (si->interactive == ON)
       {r  = SS_get_string(si->fun);
	*r = (char) toupper((int) *r);
	PRINT(stdout, "\n     %s\n", r);};

    SX_prep_arg(si, argl);

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(si, t))
        {s = SS_car(si, t);

         if (SX_curvep_a(s))
            SS_assign(si, ret, SS_mk_cons(si, fun(si, s, tok), ret));};
         
    UL_pause(si, FALSE);

    SS_assign(si, tok, SS_null);
    SS_assign(si, argl, SS_null);

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_BC_OPERATE - do the work of operating on two curves now that
 *                - they have been chosen and setup
 */

static int _UL_bc_operate(PFVoid basicf, double **xa,
			  double **xp1, double **xp2,
			  int n1, int n2)
   {int na, i, ic1, ic2;
    double ida, xv, d1, d2, yva, yvb;
    double *sx1, *sx2;
    PFDoubleRR fun;

    fun = (PFDoubleRR) basicf;

/* find starting point or bail out if no overlap */
    ic1 = 0;
    ic2 = 0;
    if (*xp1[0] < *xp2[0])
       {xv = *xp2[0];
        if (xv > xp1[0][n1-1])
           return(-1);

        for (; (*xp1[0] < xv) && (ic1 < n1); xp1[0]++, xp1[1]++, ic1++);}
    else
       {xv = *xp1[0];
        if (xv > xp2[0][n2-1])
           return(-1);

        for (; (*xp2[0] < xv) && (ic2 < n2); xp2[0]++, xp2[1]++, ic2++);};

/* select the x values for the accumulator */
    na  = 0;
    sx1 = xp1[0];
    sx2 = xp2[0];
    while ((ic1++ < n1) && (ic2++ < n2))
       {d1 = *xp1[0]++ - xv;
        d2 = *xp2[0]++ - xv;
        xv += (d2 < d1) ? d2 : d1;

        xa[0][na++] = xv;

        ida = 2.0*ABS(d2 - d1)/(d1 + d2 + SMALL);
        if (ida >= 0.2)
           {if (d2 < d1)
               {xp1[0]--;
                ic1--;}
            else
               {xp2[0]--;
                ic2--;};};};

/* get y values for the selected x values */
    xp1[0] = sx1;
    xp2[0] = sx2;
    for (i = 0; i < na; i++)
        {xv = xa[0][i];

	 for (; xv > *xp1[0]; xp1[0]++, xp1[1]++);
	 if (xv == *xp1[0])
            yva = *xp1[1];
         else
            {PM_interp(yva, xv, xp1[0][-1], xp1[1][-1], *xp1[0], *xp1[1]);};

	 for (; xv > *xp2[0]; xp2[0]++, xp2[1]++);
	 if (xv == *xp2[0])
            yvb = *xp2[1];
         else
            {PM_interp(yvb, xv, xp2[0][-1], xp2[1][-1], *xp2[0], *xp2[1]);};

         xa[1][i] = fun(yva, yvb);};

    return(na);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_BC - does binary operations on curves two at a time and
 *       - accumulates the result
 */

object *UL_bc(SS_psides *si, C_procedure *cp, object *argl)
   {int i, j, ic, id;
    int n1, n2, na;     /* number of elements in each array and accumulator */
    int temp_flag;                                /* flags for the pointers */
    double value, xt;
    double gxext[PG_SPACEDM];
    double *xp1[PG_SPACEDM], *xp2[PG_SPACEDM];
    double *xa[PG_SPACEDM], *af;                /* pointers for accumulator */
    char pbf2[MAXLINE];
    char *lbl;
    object *ch, *s, *tmp, *t;
    PFDoubleRR fun;

    fun = (PFDoubleRR) cp->proc[0];

    i = 0;

    pbf2[0] = '\0';

/* set up initial pointers */
    temp_flag = FALSE;
    SX_prep_arg(si, argl);

/* set plot flag on so that for example (+ (lst) red) causes replot */
    SX_gs.plot_flag = TRUE;

    if (SS_nullobjp(argl))
       return(argl);

    else if (SS_nullobjp(SS_cdr(si, argl)))
       {ch = SS_car(si, argl);
	return(ch);}

    else

/* search all curves in the list for the domain
 * this way it is not necessary that the screen be active
 */
       {gxext[0] =  HUGE;
        gxext[1] = -HUGE;
        for (ch = argl; !SS_nullobjp(ch); ch = SS_cdr(si, ch))
            {s = SS_car(si, ch);
             if (SX_curvep_a(s))
                {i = SX_get_crv_index_i(s);
                 for (id = 0; id < 2; id++)
		     {xt        = SX_gs.dataset[i].wc[id];
		      gxext[id] = min(gxext[id], xt);};};};
            
	value = 0.0;
        ch    = SS_null;
        t     = argl;
        while (TRUE)
           {s = SS_car(si, t);
            if (SS_numbp(s))
               {ch = SS_mk_cons(si, s, ch);
                t  = SS_cdr(si, t);}

/* the first non-number in the arg list */
            else
               {if (!SS_nullobjp(ch))
                   {s = SS_binary_homogeneous(si, cp, SS_reverse(si, ch));
                    if (SS_integerp(s))
                       value = (double) SS_INTEGER_VALUE(s);
                    else if (SS_floatp(s))
                       value = (double) SS_FLOAT_VALUE(s);

                    temp_flag = TRUE;
                    tmp = _UL_make_ln(si, 0.0, value,
                                      gxext[0],
                                      gxext[1],
                                      SX_gs.default_npts);

                    i   = SX_get_crv_index_i(tmp);
                    lbl = SC_dsnprintf(FALSE, "%s %g",
				       SS_get_string(si->fun),
				       value);}

                else if (SX_curvep_a(s))
                   {i   = SX_get_crv_index_i(s);
		    lbl = SC_dsnprintf(FALSE, "%s %s",
				       SS_get_string(si->fun),
				       _UL_id_str(i, i));}

                else
                   SS_error(si, "BAD ARGUMENT - BC", s);

                if (SS_nullobjp(ch))
                   t = SS_cdr(si, t);

                break;};

/* check for end of arg list */
            if (SS_nullobjp(t))
               {if (SS_nullobjp(ch))
                   ch = SS_null;
                else
		   ch = SS_binary_homogeneous(si, cp, SS_reverse(si, ch));

		return(ch);};};

        n1 = SX_gs.dataset[i].n;
	for (id = 0; id < 2; id++)
	    UL_gs.bfa[id] = CMAKE_N(double, n1);

/* copy the curve data into the buffer
 * this protects against hacking the curve
 */
	for (id = 0; id < 2; id++)
	    {xp1[id] = UL_gs.bfa[id];
	     xp2[id] = SX_gs.dataset[i].x[id];

	     if (id == 0)
	        UL_check_order(si, xp2[0], n1, i);

	     for (j = 0; j < n1; j++)
	         xp1[id][j] = *xp2[id]++;};};
        
    for (id = 0; id < 2; id++)
        UL_gs.bfb[id] = CMAKE_N(double, 1);

    na = n1;
    for (id = 0; id < 2; id++)
        xa[id] = xp1[id];

    af = NULL;
    SC_strncpy(pbf2, MAXLINE, lbl, -1);
    
    for ( ; SS_consp(t); t = SS_cdr(si, t))
        {s = SS_car(si, t);

/* combine a number with the accumulator */
         if (SS_numbp(s))
            {if (SS_integerp(s))
                value = (double) SS_INTEGER_VALUE(s);
             else if (SS_floatp(s))
                value = (double) SS_FLOAT_VALUE(s);
             for (xp1[1] = xa[1], ic = 0; ic < na; xp1[1]++, ic++)
                 *xp1[1] = fun(*xp1[1], value);
             lbl = SC_dsnprintf(FALSE, "%s %g", pbf2, value);
             SC_strncpy(pbf2, MAXLINE, lbl, -1);}

/* combine a curve with the accumulator */
         else if (SX_curvep_a(s))
            {j   = SX_get_crv_index_i(s);
	     lbl = SC_dsnprintf(FALSE, "%s %s",
				    pbf2, _UL_id_str(i, j));

             SC_strncpy(pbf2, MAXLINE, lbl, -1);

             n2 = SX_gs.dataset[j].n;
	     for (id = 0; id < 2; id++)
	         xp2[id] = SX_gs.dataset[j].x[id];

             UL_check_order(si, xp2[0], n2, j);

/* set/reset the accumulator */
             n1 = na;
             if (af == UL_gs.bfb[0])
	        {for (id = 0; id < 2; id++)
		     {xp1[id] = UL_gs.bfb[id];
		      CREMAKE(UL_gs.bfa[id], double, n1+2+n2);
		      xa[id] = UL_gs.bfa[id];};}

             else
	        {for (id = 0; id < 2; id++)
		     {xp1[id] = UL_gs.bfa[id];
		      CREMAKE(UL_gs.bfb[id], double, n1+2+n2);
		      xa[id] = UL_gs.bfb[id];};};

	     af = xa[0];

             na = _UL_bc_operate(cp->proc[0], xa, xp1, xp2, n1, n2);
             if (na == -1)
                return(SS_f);};};

    SS_assign(si, argl, SS_null);

/* delete the temporary curve if allocated */
    if (temp_flag)
       UL_delete(si, tmp);

/* create new curve with data in the accumulator */
    ch = SX_mk_curve(si, na, xa, lbl, NULL, UL_plot);

    for (id = 0; id < 2; id++)
        {CFREE(UL_gs.bfa[id]);
	 CFREE(UL_gs.bfb[id]);};

    return(ch);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_BCXL - binary operation on curves
 *         - uses x values from last curve in string and finds y
 *         - values for these x's.  This is primarily intended for
 *         - use in functional composition, but it may have other
 *         - applications.  cfm 2/18/87
 */

object *UL_bcxl(SS_psides *si, C_procedure *cp, object *argl)
   {int i, j, n;
    double *x[PG_SPACEDM];
    char local[MAXLINE], local2[MAXLINE];
    char *lbl;
    object *s, *ch, *t;
    PFDoubleRi fun;

    fun = (PFDoubleRi) cp->proc[0];
        
    SX_prep_arg(si, argl);
    argl = SS_reverse(si, argl);
    i    = -1;
    SS_args(si, argl,
            UL_CURVE_INDEX_I, &i,
            0);

    if (i < 0)
       SS_error(si, "BAD LAST ARGUMENT - UL_BCXL", SS_reverse(si, argl));

/* set plot flag on so that for example (compose (lst)) causes replot */
    SX_gs.plot_flag = TRUE;

    x[0] = SX_gs.dataset[i].x[0];
    x[1] = SX_gs.dataset[i].x[1];
    n  = SX_gs.dataset[i].n;

    SC_strncpy(local, MAXLINE, _UL_id_str(i, i), -1);

    UL_gs.bfa[0] = CMAKE_N(double, n);
    UL_gs.bfa[1] = CMAKE_N(double, n);
    for (j = 0; j < n; j++)                          /* copy to accumulator */
        {UL_gs.bfa[0][j] = x[0][j];
         UL_gs.bfa[1][j] = x[1][j];};
    x[0] = UL_gs.bfa[0];
    x[1] = UL_gs.bfa[1];
    SC_strncpy(local2, MAXLINE, local, -1);

    for (t = SS_cdr(si, argl); SS_consp(t); t = SS_cdr(si, t))
        {s = SS_car(si, t);
         if (!SX_curvep_a(s))
            SS_error(si, "BAD ARGUMENT - UL_BCXL", s);
         i = SX_get_crv_index_i(s);
	 snprintf(local, MAXLINE, "%s %s", local2, _UL_id_str(i, i));

         SC_strncpy(local2, MAXLINE, local, -1);

         for (j = 0; j < n; j++)
             {x[1][j] = fun(x[1][j], i);};};

    lbl = SC_dsnprintf(FALSE, "%s %s",
		       SS_get_string(si->fun), SC_strrev(local));
    ch = SX_mk_curve(si, n, x, lbl, NULL, UL_plot);

    SS_assign(si, argl, SS_null);

    CFREE(UL_gs.bfa[0]);
    CFREE(UL_gs.bfa[1]);           

    return(ch);}
                
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

