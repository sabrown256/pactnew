/*
 * ULHAND.C - handlers for Ultra
 *
 * Source Version: 4.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "ultra.h"

typedef object *(*PFSargs)(object *argl);
typedef object *(*PFSargsI)(int j);
typedef double (*PFDoubleR)(double x);
typedef double (*PFDoubleRR)(double x, double y);
typedef double (*PFDoubleRi)(double x, int i);
typedef void (*PFVoidis)(int j, char *s);
typedef object *(*PFPObjectidd)(int j, double d1, double d2);
typedef object *(*PFPObjectio)(int j, object *argl);
typedef object *(*PFPObjectoo)(object *s, object *tok);

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

void UL_check_order(double *p, int n, int i)
   {double *p1, *p2;
    int j;

    for (p1 = p, p2 = p+1, j = 1; j < n; p1++, p2++, j++)
        {if (*p1 > *p2)                             /* sort if out of order */
            {if (SS_interactive == ON)
                {if ((SX_dataset[i].id >= 'A') &&
                     (SX_dataset[i].id <= 'Z'))
                    {PRINT(stdout, "\nSorting curve %c\n", SX_dataset[i].id);}
                 else
                    {PRINT(stdout, "\nSorting curve @%d\n", SX_dataset[i].id);};}
             UL_sort(i);
             break;};};

    return;}
                        
/*--------------------------------------------------------------------------*/

/*                            FUNCTION HANDLERS                             */

/*--------------------------------------------------------------------------*/

/* UL_US - handler for unary operation acting on a set of curves
 *       - this handler passes a Scheme object to its functions and
 *       - expects Scheme objects back from them
 */

object *UL_us(C_procedure *cp, object *argl)
   {object *ret, *val, *t;
    PFSargs fun;

    fun = (PFSargs) cp->proc[0];
    SX_prep_arg(argl);

/* set plot flag on so that for example (select (menu)) causes replot */
    SX_plot_flag = TRUE;

    ret = SS_null;
    for (t = argl; !SS_nullobjp(t); t = SS_cdr(t))
        {val = (*fun)(SS_car(t));
         SS_Assign(ret, SS_mk_cons(val, ret));};

    SS_Assign(argl, SS_null);

    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_UC - handler for unary operation acting on a set of curves
 *       - these funtions expect an integer curve index and
 *       - return a valid Scheme object
 */

object *UL_uc(C_procedure *cp, object *argl)
   {int i;
    object *ret, *t;

    SX_prep_arg(argl);

/* set plot flag on so that for example (copy (lst)) causes replot */
    SX_plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; !SS_nullobjp(t); t = SS_cdr(t))
        {i = SX_get_crv_index_i(SS_car(t));
         if (i >= 0)
            {SS_Assign(ret, SS_mk_cons((*(PFSargsI) cp->proc[0])(i), ret));};};
         
    SS_Assign(argl, SS_null);

    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_OPXC - performs binary operations on x values of curves
 *         - using a single scalar
 */
    
object *UL_opxc(C_procedure *cp, object *argl)
   {object *tok, *ret, *t;
    double a;
    int i, l, n;
    double *xp;
        
    SX_last_arg(tok, argl);
    SX_prep_arg(argl);

    a = HUGE;
    SS_args(tok,
            SC_DOUBLE_I, &a,
            0);
    if (a == HUGE)
       SS_error("BAD NUMBER - UL_OPXC ", tok);

    SS_Assign(tok, SS_null);

/* set plot flag on so that for example (dx (lst)) causes replot */
    SX_plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(t))
        {i = SX_get_crv_index_i(SS_car(t));
         if (i != -1)
            {n = SX_dataset[i].n;
             for (xp = SX_dataset[i].x[0], l = 0; l < n; xp++, l++)
                 *xp = (double) (*(PFDoubleRR) cp->proc[0])(*xp, a);
             SX_dataset[i].modified = TRUE;

             SS_Assign(ret, SS_mk_cons(SX_dataset[i].obj, ret));
             UL_lmt(SX_dataset[i].x[0], n,
                    &SX_dataset[i].wc[0],
                    &SX_dataset[i].wc[1]);};};
         
    SS_Assign(argl, SS_null);

    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_OPYC - performs binary operations on y values of curves
 *         - using a single scalar
 */

object *UL_opyc(C_procedure *cp, object *argl)
   {object *tok, *ret, *t;
    double a;
    int i, l, n;
    double *yp;
        
    SX_last_arg(tok, argl);
    SX_prep_arg(argl);

    a = HUGE;
    SS_args(tok,
            SC_DOUBLE_I, &a,
            0);
    if (a == HUGE)
       SS_error("BAD NUMBER - UL_OPYC ", tok);

    SS_Assign(tok, SS_null);

/* set plot flag on so that for example (dy (lst)) causes replot */
    SX_plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(t))
        {i = SX_get_crv_index_i(SS_car(t));
         if (i != -1)
            {n = SX_dataset[i].n;
             for (yp = SX_dataset[i].x[1], l = 0; l < n; yp++, l++)
                 *yp = (double) (*(PFDoubleRR) cp->proc[0])(*yp, a);
             SX_dataset[i].modified = TRUE;

             SS_Assign(ret, SS_mk_cons(SX_dataset[i].obj, ret));
             UL_lmt(SX_dataset[i].x[1], n,
                    &SX_dataset[i].wc[2],
                    &SX_dataset[i].wc[3]);};};

    SS_Assign(argl, SS_null);

    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_UL2TOC - unary applies last 2 args to curve */

object *UL_ul2toc(C_procedure *cp, object *argl)
   {object *o;

    o = _UL_ul2toc(cp, argl, TRUE);
   
    return(o);}
   
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_UL2TOCNP - unary applies last 2 args to curve
 *             - and does not replot
 */

object *UL_ul2tocnp(C_procedure *cp, object *argl)
   {object *o;

    o = _UL_ul2toc(cp, argl, FALSE);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_UL2TOC - worker routine for UL_UL2TOC and UL_UL2TOCNP */

object *_UL_ul2toc(C_procedure *cp, object *argl, int replot_flag)
   {int i;
    double d1, d2;
    object *s, *t, *tok1, *tok2, *ret;

    tok1 = NULL;
    tok2 = NULL;

    SS_Assign(argl, SS_reverse(argl));
    tok2 = SS_car(argl);
    if (SS_numbp(tok2))
       {SS_Assign(argl, SS_cdr(argl));
	if (SS_consp(argl))
	   {tok1 = SS_car(argl);
	    if (SS_numbp(tok1))
	       {SS_Assign(argl, SS_cdr(argl));}
	    else
	       {tok1 = tok2;
		tok2 = NULL;};}
	else
	   {tok1 = tok2;
	    tok2 = NULL;};}
    else
        tok2 = NULL;
    SS_Assign(argl, SS_reverse(argl));

    SX_prep_arg(argl);

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(t))
        {s = SS_car(t);
         if (SX_curvep_a(s))
            {i = SX_get_crv_index_i(s);
	     if (tok1 != NULL)
	        SS_args(tok1,
			SC_DOUBLE_I, &d1,
			0);
	     else
	        d1 = SX_dataset[i].wc[0];
	     if (tok2 != NULL)
	        SS_args(tok2,
			SC_DOUBLE_I, &d2,
			0);
	     else
	        d2 = SX_dataset[i].wc[1];
             SS_Assign(ret,
                       SS_mk_cons((*(PFPObjectidd) cp->proc[0])(i, d1, d2), ret));};};

    SX_plot_flag = replot_flag;

    SS_Assign(argl, SS_null);

    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_ULNTOC - unary applies last n args to curve */

object *UL_ulntoc(C_procedure *cp, object *argl)
   {int i;
    object *tok, *crvs, *ret, *t, *u;

    SX_prep_arg(argl);

    for (t = argl, crvs = SS_null; SS_consp(t); t = SS_cdr(t))
        {tok = SS_car(t);
         if (SX_curvep_a(tok))
            {SS_Assign(crvs, SS_mk_cons(tok, crvs));}
         else
            break;};

/* set plot flag on so that for example (filter (lst) dp rp) causes replot */
    SX_plot_flag = TRUE;

    for (ret = SS_null, u = crvs; SS_consp(u); u = SS_cdr(u))
        {tok = SS_car(u);
         if (SX_curvep_a(tok))
            {i = SX_get_crv_index_i(tok);
             SS_Assign(ret,
                       SS_mk_cons((*(PFPObjectio) cp->proc[0])(i, t), ret));};};

    SS_Assign(crvs, SS_null);
    SS_Assign(argl, SS_null);
    SS_Assign(ret, SS_reverse(ret));

    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_UOPXC - math handler applies the given unary function to the x values */
    
object *UL_uopxc(C_procedure *cp, object *argl)
   {int i, l, n;
    double *xp;
    object *ret, *t;

    SX_prep_arg(argl);

/* set plot flag on so that for example (expx (lst)) causes replot */
    SX_plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(t))
        {i = SX_get_crv_index_i(SS_car(t));
         if (i != -1)
            {n = SX_dataset[i].n;
             for (xp = SX_dataset[i].x[0], l = 0; l < n; xp++, l++)
                 *xp = (double) (*(PFDoubleR) cp->proc[0])(*xp);
             SX_dataset[i].modified = TRUE;

             SS_Assign(ret, SS_mk_cons(SX_dataset[i].obj, ret));
             UL_lmt(SX_dataset[i].x[0], n,
                    &SX_dataset[i].wc[0],
                    &SX_dataset[i].wc[1]);};};
         
    SS_Assign(argl, SS_null);

    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_UOPYC - math handler applies the given unary function to the y values */

object *UL_uopyc(C_procedure *cp, object *argl)
   {int i, l, n;
    double *yp, f;
    object *s, *ret, *tmp, *t;

    SX_prep_arg(argl);

/* set plot flag on so that for example (expx (lst)) causes replot */
    SX_plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(t))
        {s   = SS_car(t);
         i   = SX_get_crv_index_i(s);
	 tmp = SS_null;
         if (i != -1)
            {n = SX_dataset[i].n;
             for (yp = SX_dataset[i].x[1], l = 0; l < n; yp++, l++)
                 *yp = (double) (*(PFDoubleR) cp->proc[0])(*yp);
             SX_dataset[i].modified = TRUE;

             tmp = SS_mk_cons(SX_dataset[i].obj, ret);
             UL_lmt(SX_dataset[i].x[1], n,
                    &SX_dataset[i].wc[2], &SX_dataset[i].wc[3]);}

         else if (SS_integerp(s))
            {f   = (double) SS_INTEGER_VALUE(s);
             tmp = SS_mk_cons(SS_mk_float((*(PFDoubleR) cp->proc[0])(f)), ret);}

         else if (SS_floatp(s))
            {f   = SS_FLOAT_VALUE(s);
             tmp = SS_mk_cons(SS_mk_float((*(PFDoubleR) cp->proc[0])(f)), ret);};

         SS_Assign(ret, tmp);};

    SS_Assign(argl, SS_null);

    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_BFTOC - unary operation on curves using first argument as
 *          - a parameter
 */

object *UL_bftoc(C_procedure *cp, object *argl)
   {int i;
    char *s1;
    object *s, *tok, *t;

    SX_prep_arg(argl);

/* set plot flag on so that for example (op param (lst)) causes replot */
    SX_plot_flag = TRUE;

    tok  = SS_car(argl);
    argl = SS_cdr(argl);

    s1 = CSTRSAVE(SS_get_string(tok));

    for (t = argl ; SS_consp(t); t = SS_cdr(t))
        {s = SS_car(t);

         if (SX_curvep_a(s))
            {i = SX_get_crv_index_i(s);
             (*(PFVoidis) cp->proc[0])(i, s1);};};

    CFREE(s1);

    SS_Assign(argl, SS_null);

    return(tok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_BLTOC - unary operation applied to each curve using the last argument
 *          - as a parameter
 */

object *UL_bltoc(C_procedure *cp, object *argl)
   {object *s, *ret, *tok, *t;

    SX_last_arg(tok, argl);
    if (!SS_numbp(tok))
       SS_error("BAD LAST ARGUMENT - UL_BLTOC", tok);

    SX_prep_arg(argl);

/* set plot flag on so that for example (color (lst) red) causes replot */
    SX_plot_flag = TRUE;

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(t))
        {s = SS_car(t);

         if (SX_curvep_a(s))
            SS_Assign(ret, SS_mk_cons((*(PFPObjectoo) cp->proc[0])(s, tok), ret));};

    SS_Assign(tok, SS_null);
    SS_Assign(argl, SS_null);

    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_BLTOCNP - unary operation applied to each curve using the last argument
 *            - as a parameter
 *            - clears screen, prints name of function and
 *            - surpresses plotting
 */

object *UL_bltocnp(C_procedure *cp, object *argl)
   {object *s, *tok, *ret, *t;
    char *r;

    SX_last_arg(tok, argl);
    if (!SS_numbp(tok))
       SS_error("BAD LAST ARGUMENT - BLTOCNP", tok);

    if (SS_interactive == ON)
       {r  = SS_get_string(SS_Fun);
	*r = (char) toupper((int) *r);
	PRINT(stdout, "\n     %s\n", r);};

    SX_prep_arg(argl);

    ret = SS_null;
    for (t = argl ; SS_consp(t); t = SS_cdr(t))
        {s = SS_car(t);

         if (SX_curvep_a(s))
            SS_Assign(ret, SS_mk_cons((*(PFPObjectoo) cp->proc[0])(s, tok), ret));};
         
    UL_pause(FALSE);

    SS_Assign(tok, SS_null);
    SS_Assign(argl, SS_null);

    SX_prep_ret(ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _UL_BC_OPERATE - do the work of operating on two curves now that
 *                - they have been chosen and setup
 */

static int _UL_bc_operate(PFVoid basicf, double *xa, double *ya, 
			  double *xp1, double *xp2, double *yp1, double *yp2,
			  int n1, int n2)
   {int na, i, ic1, ic2;
    double ida, xv, d1, d2, yva, yvb;
    double *sx1, *sx2;

/* find starting point or bail out if no overlap */
    ic1 = 0;
    ic2 = 0;
    if (*xp1 < *xp2)
       {xv = *xp2;
        if (xv > xp1[n1-1])
           return(-1);

        for (; (*xp1 < xv) && (ic1 < n1); xp1++, yp1++, ic1++);}
    else
       {xv = *xp1;
        if (xv > xp2[n2-1])
           return(-1);

        for (; (*xp2 < xv) && (ic2 < n2); xp2++, yp2++, ic2++);};

/* select the x values for the accumulator */
    na  = 0;
    sx1 = xp1;
    sx2 = xp2;
    while ((ic1++ < n1) && (ic2++ < n2))
       {d1 = *xp1++ - xv;
        d2 = *xp2++ - xv;
        xv += (d2 < d1) ? d2 : d1;

        xa[na++] = xv;

        ida = 2.0*ABS(d2 - d1)/(d1 + d2 + SMALL);
        if (ida >= 0.2)
           {if (d2 < d1)
               {xp1--;
                ic1--;}
            else
               {xp2--;
                ic2--;};};};

/* get y values for the selected x values */
    xp1 = sx1;
    xp2 = sx2;
    for (i = 0; i < na; i++)
        {xv = xa[i];

	 for (; xv > *xp1; xp1++, yp1++);
	 if (xv == *xp1)
            yva = *yp1;
         else
            {PM_interp(yva, xv, xp1[-1], yp1[-1], *xp1, *yp1);};

	 for (; xv > *xp2; xp2++, yp2++);
	 if (xv == *xp2)
            yvb = *yp2;
         else
            {PM_interp(yvb, xv, xp2[-1], yp2[-1], *xp2, *yp2);};

         ya[i] = (double) (*(PFDoubleRR) basicf)(yva, yvb);};

    return(na);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_BC - does binary operations on curves two at a time and
 *       - accumulates the result
 */

object *UL_bc(C_procedure *cp, object *argl)
   {int i, j;
    int ic;
    int n1, n2, na;     /* number of elements in each array and accumulator */
    int temp_flag;                                /* flags for the pointers */
    double value, gxmin, gxmax, xt;
    double *xp1, *xp2, *yp1, *yp2;
    double *xa, *af, *ya;                         /* pointers for accumulator */
    char pbf2[MAXLINE];
    char *lbl;
    object *ch, *s, *tmp, *t;

    i = 0;

    pbf2[0] = '\0';

/* set up initial pointers */
    temp_flag = FALSE;
    SX_prep_arg(argl);

/* set plot flag on so that for example (+ (lst) red) causes replot */
    SX_plot_flag = TRUE;

    if (SS_nullobjp(argl))
       return(argl);

    else if (SS_nullobjp(SS_cdr(argl)))
       {ch = SS_car(argl);
	return(ch);}

    else

/* search all curves in the list for the domain
 * this way it is not necessary that the screen be active
 */
       {gxmin =  HUGE;
        gxmax = -HUGE;
        for (ch = argl; !SS_nullobjp(ch); ch = SS_cdr(ch))
            {s = SS_car(ch);
             if (SX_curvep_a(s))
                {i     = SX_get_crv_index_i(s);
                 xt    = SX_dataset[i].wc[0];
                 gxmin = min(gxmin, xt);
                 xt    = SX_dataset[i].wc[1];
                 gxmax = max(gxmax, xt);};};
            
	value = 0.0;
        ch    = SS_null;
        t     = argl;
        while (TRUE)
           {s = SS_car(t);
            if (SS_numbp(s))
               {ch = SS_mk_cons(s, ch);
                t  = SS_cdr(t);}

/* the first non-number in the arg list */
            else
               {if (!SS_nullobjp(ch))
                   {s = SS_binary_homogeneous(cp, SS_reverse(ch));
                    if (SS_integerp(s))
                       value = (double) SS_INTEGER_VALUE(s);
                    else if (SS_floatp(s))
                       value = (double) SS_FLOAT_VALUE(s);

                    temp_flag = TRUE;
                    tmp = _UL_make_ln(0.0, value,
                                      gxmin,
                                      gxmax,
                                      SX_default_npts);

                    i   = SX_get_crv_index_i(tmp);
                    lbl = SC_dsnprintf(FALSE, "%s %g",
				       SS_get_string(SS_Fun),
				       value);}

                else if (SX_curvep_a(s))
                   {i = SX_get_crv_index_i(s);
                    if ((SX_dataset[i].id >= 'A') &&
                        (SX_dataset[i].id <= 'Z'))
                       {lbl = SC_dsnprintf(FALSE, "%s %c",
					   SS_get_string(SS_Fun),
					   SX_dataset[i].id);}
                    else
                       {lbl = SC_dsnprintf(FALSE, "%s @%d",
					   SS_get_string(SS_Fun),
					   SX_dataset[i].id);};}
                else
                   SS_error("BAD ARGUMENT - BC", s);

                if (SS_nullobjp(ch))
                   t = SS_cdr(t);

                break;};

/* check for end of arg list */
            if (SS_nullobjp(t))
               {if (SS_nullobjp(ch))
                   ch = SS_null;
                else
		   ch = SS_binary_homogeneous(cp, SS_reverse(ch));

		return(ch);};};

        n1 = SX_dataset[i].n;
        UL_buf1x = CMAKE_N(double, n1);
        UL_buf1y = CMAKE_N(double, n1);

/* copy the curve data into the buffer
 * this protects against hacking the curve
 */
        xp1 = UL_buf1x;
        yp1 = UL_buf1y;
        xp2 = SX_dataset[i].x[0];
        yp2 = SX_dataset[i].x[1];
        UL_check_order(xp2, n1, i);
        for (j = 0; j < n1; j++)
            {*xp1++ = *xp2++;
             *yp1++ = *yp2++;};

        xp1 = UL_buf1x;
        yp1 = UL_buf1y;};
        
    UL_buf2x = CMAKE_N(double, 1);
    UL_buf2y = CMAKE_N(double, 1);

    na = n1;
    xa = xp1;
    ya = yp1;
    af = NULL;
    strcpy(pbf2, lbl);
    
    for ( ; SS_consp(t); t = SS_cdr(t))
        {s = SS_car(t);

/* combine a number with the accumulator */
         if (SS_numbp(s))
            {if (SS_integerp(s))
                value = (double) SS_INTEGER_VALUE(s);
             else if (SS_floatp(s))
                value = (double) SS_FLOAT_VALUE(s);
             for (yp1 = ya, ic = 0; ic < na; yp1++, ic++)
                 *yp1 = (double) (*(PFDoubleRR) cp->proc[0])(*yp1, value);
             lbl = SC_dsnprintf(FALSE, "%s %g", pbf2, value);
             strcpy(pbf2, lbl);}

/* combine a curve with the accumulator */
         else if (SX_curvep_a(s))
            {j = SX_get_crv_index_i(s);
             if ((SX_dataset[i].id >= 'A') &&
                 (SX_dataset[i].id <= 'Z'))
                {lbl = SC_dsnprintf(FALSE, "%s %c", pbf2, SX_dataset[j].id);}
             else
                {lbl = SC_dsnprintf(FALSE, "%s @%d", pbf2, SX_dataset[j].id);}

             strcpy(pbf2, lbl);

             xp2 = SX_dataset[j].x[0];
             yp2 = SX_dataset[j].x[1];
             n2  = SX_dataset[j].n;
             UL_check_order(xp2, n2, j);

/* set/reset the accumulator */
             n1 = na;
             if (af == UL_buf2x)
                {xp1 = UL_buf2x;
                 yp1 = UL_buf2y;
                 CREMAKE(UL_buf1x, double, n1+2+n2);
                 CREMAKE(UL_buf1y, double, n1+2+n2);
                 xa = af = UL_buf1x;
                 ya = UL_buf1y;}
             else
                {xp1 = UL_buf1x;

                 yp1 = UL_buf1y;
                 CREMAKE(UL_buf2x, double, n1+2+n2);
                 CREMAKE(UL_buf2y, double, n1+2+n2);
                 xa = af = UL_buf2x;
                 ya = UL_buf2y;};

             na = _UL_bc_operate(cp->proc[0], xa, ya, xp1, xp2, yp1, yp2, n1, n2);
             if (na == -1)
                return(SS_f);};};

    SS_Assign(argl, SS_null);

/* UL_delete the temporary curve if allocated */
    if (temp_flag)
       UL_delete(tmp);

/* create new curve with data in the accumulator */
    ch = SX_mk_curve(na, xa, ya, lbl, NULL, (PFVoid) UL_plot);

    CFREE(UL_buf1x);
    CFREE(UL_buf1y);
    CFREE(UL_buf2x);
    CFREE(UL_buf2y);

    return(ch);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UL_BCXL - binary operation on curves
 *         - uses x values from last curve in string and finds y
 *         - values for these x's.  This is primarily intended for
 *         - use in functional composition, but it may have other
 *         - applications.  cfm 2/18/87
 */

object *UL_bcxl(C_procedure *cp, object *argl)
   {int i, j, n;
    double *x[PG_SPACEDM];
    char local[MAXLINE], local2[MAXLINE];
    char *lbl;
    object *s, *ch, *t;
        
    SX_prep_arg(argl);
    argl = SS_reverse(argl);
    i    = -1;
    SS_args(argl,
            UL_CURVE_INDEX_I, &i,
            0);

    if (i < 0)
       SS_error("BAD LAST ARGUMENT - UL_BCXL", SS_reverse(argl));

/* set plot flag on so that for example (compose (lst)) causes replot */
    SX_plot_flag = TRUE;

    x[0] = SX_dataset[i].x[0];
    x[1] = SX_dataset[i].x[1];
    n  = SX_dataset[i].n;

    if ((SX_dataset[i].id >= 'A') &&
        (SX_dataset[i].id <= 'Z'))
       {snprintf(local, MAXLINE, "%c", SX_dataset[i].id);}
    else
       {snprintf(local, MAXLINE, "@%d", SX_dataset[i].id);}

    UL_buf1x = CMAKE_N(double, n);
    UL_buf1y = CMAKE_N(double, n);
    for (j = 0; j < n; j++)                          /* copy to accumulator */
        {UL_buf1x[j] = x[0][j];
         UL_buf1y[j] = x[1][j];};
    x[0] = UL_buf1x;
    x[1] = UL_buf1y;
    strcpy(local2, local);

    for (t = SS_cdr(argl); SS_consp(t); t = SS_cdr(t))
        {s = SS_car(t);
         if (!SX_curvep_a(s))
            SS_error("BAD ARGUMENT - UL_BCXL", s);
         i = SX_get_crv_index_i(s);
         if ((SX_dataset[i].id >= 'A') &&
             (SX_dataset[i].id <= 'Z'))
            {snprintf(local, MAXLINE, "%s %c", local2, SX_dataset[i].id);}
         else
            {snprintf(local, MAXLINE, "%s @%d", local2, SX_dataset[i].id);}

         strcpy(local2, local);

         for (j = 0; j < n; j++)
             {x[1][j] = (double) (*(PFDoubleRi) cp->proc[0])(x[1][j], i);};};

    lbl = SC_dsnprintf(FALSE, "%s %s",
		       SS_get_string(SS_Fun), SC_strrev(local));
    ch = SX_mk_curve(n, x[0], x[1], lbl, NULL, (PFVoid) UL_plot);

    SS_Assign(argl, SS_null);

    CFREE(UL_buf1x);
    CFREE(UL_buf1y);           

    return(ch);}
                
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

