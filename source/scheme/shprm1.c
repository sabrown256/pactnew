/*
 * SHPRM1.C - Scheme Primitives
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

#define SS_Unquoted(x) ((x) == SS_unqproc)
#define Unqsplicing(x) ((x) == SS_unqspproc)

static object
 *_SS_quasiq(object *obj, int nestl);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_NEWSYM - make-new-symbol, generate a new uninterned symbol */

static object *_SSI_newsym(object *obj)
   {char *token, name[MAXLINE];
    object *o;

    token = NULL;

    if (SS_stringp(obj))
       token = SS_STRING_TEXT(obj);
    else if (SS_variablep(obj))
       token = SS_VARIABLE_NAME(obj);
    else
       SS_error("BAD OBJECT - _SSI_NEWSYM", obj);

    snprintf(name, MAXLINE, "%s%-d", token, _SS.nsym++);

    o = SS_mk_variable(name, SS_null);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_QUOTE - quote macro in Scheme */

static object *_SSI_quote(object *obj)
   {

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_UNQUOTE - does the real work of the unquote macro */

static object *_SS_unquote(object *x)
   {SS_psides *si;

    si = &_SS_si;

    x = SS_exp_eval(si, SS_cadr(x));

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_SPLICE - given  a return budding return list, an object, and the
 *            - rest of the original list
 *            - bash it all together and return the finished return list
 */

static object *_SS_splice(object *ncns, object *item,
			  object *lst, object *tcns)
   {object *rest;

/* process the rest of the list */
    rest = _SS_quasiq(lst, _SS.nest_level);

/* check the cases */
    if (SS_nullobjp(ncns))
       ncns = SS_mk_cons(item, rest);

    else
/* if there are no remaining elements */
       {if (SS_nullobjp(rest))
           ncns = SS_append(ncns, item);

/* if the remainder is not a list */
        else if (!SS_consp(rest))

/* if the item is null */
           {if (SS_nullobjp(item))
               SS_setcdr(tcns, rest);

/* if the item is non-null */
            else
               ncns = SS_append(ncns, SS_mk_cons(item, rest));}

/* if the remainder is a list */
        else
           ncns = SS_append(ncns, SS_append(item, rest));};

    return(ncns);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_QUASIQ - quasiquote macro at C level (sort of)
 *            - SS_quasiproc, name of quasiquote procedure
 *            - SS_unqproc, name of unquote procedure
 *            - SS_unqspproc,name of unquote-splicing procedure
 *            - NOTE: this crock of a version doesn't nest
 */

static object *_SS_quasiq(object *obj, int nestl)
   {object *lst, *car, *tcns, *ncns, *y;

/* handle forms like (quasiquote atom) */
    if (!SS_consp(obj))
       return(obj);

/* handle forms like (quasiquote (unqote expr)) */
    if (!SS_consp(tcns = SS_car(obj)))
       {if (SS_Unquoted(tcns))
           {ncns = _SS_unquote(obj);
            return(ncns);}
        else if (Unqsplicing(tcns))
           SS_error("MUST BE IMBEDDED IN LIST - _SS_QUASIQ", obj);};

/* look down a quasiquoted list */
    ncns = SS_null;
    y   = SS_null;
    for (lst = obj; SS_consp(lst); lst = SS_cdr(lst))
        {tcns = SS_car(lst);

/* if this element is a list */
         if (SS_consp(tcns))
            {car = SS_car(tcns);

/* is it an unquote form? */
             if (SS_Unquoted(car))
                {SS_end_cons(ncns, y, _SS_unquote(tcns));}

/* is it an unquote-splicing form? */
             else if (Unqsplicing(car))
                {ncns = _SS_splice(ncns, _SS_unquote(tcns), SS_cdr(lst), y);
                 break;}

/* any other list should be searched */
             else
                {SS_end_cons(ncns, y, _SS_quasiq(tcns, nestl));};}

/* any non-list element should be added */
         else
            {SS_end_cons(ncns, y, tcns);};};

    return(ncns);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_QUASIQ - quasiquote macro in Scheme
 *             - SS_quasiproc, name of quasiquote procedure
 *             - SS_unqproc, name of unquote procedure
 *             - SS_unqspproc, name of unquote-splicing procedure
 */

static object *_SSI_quasiq(object *obj)
   {SS_psides *si;

    si = &_SS_si;

    _SS.nest_level++;

    SS_Assign(si->val, _SS_quasiq(obj, _SS.nest_level));

    _SS.nest_level--;

    return(si->val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_UNQUOTE - unquote macro in Scheme */

static object *_SSI_unquote(object *obj)
   {object *o;
    SS_psides *si;

    si = &_SS_si;

    o = SS_exp_eval(si, obj);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_UNQ_SPL - unquote-splicing macro in Scheme */

static object *_SSI_unq_spl(object *obj)
   {object *o;
    SS_psides *si;

    si = &_SS_si;

    o = SS_exp_eval(si, obj);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LAMBDA - lambda, the procedure making macro in Scheme */

static object *_SSI_lambda(object *argl)
   {object *lambda;
    SS_psides *si;

    si = &_SS_si;

    lambda = SS_mk_procedure(SS_anon_proc, argl, si->env);

    return(lambda);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LAMBDAM - lambda-macro, the macro making macro in Scheme */

static object *_SSI_lambdam(object *argl)
   {object *lambda;
    SS_psides *si;

    si = &_SS_si;

    lambda = SS_mk_procedure(SS_anon_macro, argl, si->env);

/* NOTE: without this mark the environment ends up being GC'd
 * inappropriately
 * this came up in connection with c/decl-spec in csynt.scm
 */
    SC_mark(si->env, 1);

    SS_PROCEDURE_TYPE(lambda) = SS_MACRO;

    CFREE(SS_PROCEDURE_NAME(lambda));
    SS_PROCEDURE_NAME(lambda) = CSTRSAVE("lambda-macro");

    return(lambda);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LET - transform a let special form into a lambda application
 *        - to be handed back to eval
 */

static object *SS_let(object *let)
   {object *vlpair, *vr, *vl, *lst;
    object *prml, *argl, *bdy, *lambda;
    SS_psides *si;

    si = &_SS_si;

    if (SS_nullobjp(let) || !SS_consp(let))
       SS_error("BAD LET FORM", let);

/* convert the locals of the let into a list of formal parameters
 * for the lambda and a list of arguments for the call
 */
    prml = SS_null;
    argl = SS_null;
    for (lst = SS_car(let); !SS_nullobjp(lst); lst = SS_cdr(lst))
        {vlpair = SS_car(lst);
         if (!SS_consp(vlpair))
	    {vr = vlpair;
             vl = SS_null;}
         else
            {vr = SS_car(vlpair);
             vl = SS_cadr(vlpair);};

         prml = SS_mk_cons(vr, prml);
         argl = SS_mk_cons(vl, argl);};

/* now make the procedure */
    bdy    = SS_cdr(let);
    bdy    = SS_mk_cons(prml, bdy);
    lambda = SS_mk_procedure(SS_block_proc, bdy, si->env);

/* now make the procedure call */
    SS_Assign(si->exn, SS_mk_cons(lambda, argl));

    return(si->exn);}

/*--------------------------------------------------------------------------*/

#if 0

/* NOTE: you would like _SSI_letstr to look like this version
 *       but it leaks memory
 */

/*--------------------------------------------------------------------------*/

/* _SSI_LETSTR - transform a let* special form into a let form
 *             - to be handed back to eval
 */

static object *_SSI_letstr(object *lets)
   {object *vlpair, *vr, *vl, *lst;
    object *prml, *asgn, *asnl, *bdy, *let, *this;
    SS_psides *si;

    si = &_SS_si;

    if (SS_nullobjp(lets) || !SS_consp(lets))
       SS_error("BAD LET* FORM", lets);

/* convert the locals of the let* into a list of formal parameters
 * for the lambda and a list of assignment statements to be prepended
 * to the body of the let*
 */
    this = SS_null;
    asnl = SS_null;
    prml = SS_null;
    for (lst = SS_car(lets); !SS_nullobjp(lst); lst = SS_cdr(lst))
        {vlpair = SS_car(lst);
         if (!SS_consp(vlpair))
            {vr = vlpair;
             vl = SS_null;}
         else
            {vr = SS_car(vlpair);
             vl = SS_cadr(vlpair);};

         asgn = SS_make_form(SS_setproc, vr, vl, LAST);
         SS_end_cons_macro(asnl, this, asgn);
         prml = SS_mk_cons(vr, prml);};

/* append the assignment list to the body */
    bdy = SS_append(asnl, SS_cdr(lets));

/* complete the let form */
    let = SS_mk_cons(prml, bdy);

/* process the let form */
    SS_Assign(si->exn, SS_let(let));

    return(si->exn);}

/*--------------------------------------------------------------------------*/

#else

#if 1

/* NOTE: this is part of the way between the two versions
 *       and does not leak memory
 */

/*--------------------------------------------------------------------------*/

/* _SSI_LETSTR - transform a let* special form into a let form
 *             - to be handed back to eval
 */

static object *_SSI_letstr(object *lets)
   {object *vlpair, *vr, *vl, *lst, *frm;
    object *prml, *bdy, *asgn;
    SS_psides *si;

    si = &_SS_si;

    if (SS_nullobjp(lets) || !SS_consp(lets))
       SS_error("BAD LET* FORM", lets);

    SS_Save(si, si->this);
    SS_Save(si, si->argl);
    SS_Save(si, si->fun);

/* transform into a functionally equivalent let form */
    SS_Assign(si->this, SS_null);
    SS_Assign(si->argl, SS_null);
    prml = SS_null;
    for (lst = SS_car(lets); !SS_nullobjp(lst); lst = SS_cdr(lst))
        {if (!SS_consp(vlpair = SS_car(lst)))
            {vr = vlpair;
             vl = SS_null;}
         else
            {vr = SS_car(vlpair);
             vl = SS_cadr(vlpair);};

         asgn = SS_make_form(SS_setproc, vr, vl, LAST);
         SS_end_cons_macro(si->argl, si->this, asgn);
         prml = SS_mk_cons(vr, prml);};

/* append the assignment list to the body */
    bdy = SS_append(si->argl, SS_cdr(lets));

/* complete the transformation */
    frm = SS_mk_cons(prml, bdy);
    SS_Assign(si->fun, frm);

/* process the let form */
    SS_Assign(si->exn, SS_let(si->fun));

/* clean up the mess */
    SS_Restore(si, si->fun);
    SS_Restore(si, si->argl);
    SS_Restore(si, si->this);

    return(si->exn);}

/*--------------------------------------------------------------------------*/

#else

/* NOTE: it used to look like this
 *       and this version does not leak memory
 */

/*--------------------------------------------------------------------------*/

/* _SSI_LETSTR - transform a let* special form into a let form
 *             - to be handed back to eval
 */

static object *_SSI_letstr(object *letr)
   {object *vlpair, *vr, *vl, *lst, *frm;
    SS_psides *si;

    si = &_SS_si;

    if (SS_nullobjp(letr) || !SS_consp(letr))
       SS_error("BAD LET* FORM", letr);

    SS_Save(si, si->this);
    SS_Save(si, si->unev);
    SS_Save(si, si->argl);
    SS_Save(si, si->fun);
    SS_Save(si, si->val);

/* transform into a functionally equivalent let form */
    SS_Assign(si->this, SS_null);
    SS_Assign(si->argl, SS_null);
    SS_Assign(si->unev, SS_null);
    for (lst = SS_car(letr); !SS_nullobjp(lst); lst = SS_cdr(lst))
        {if (!SS_consp(vlpair = SS_car(lst)))
            {vr = vlpair;
             vl = SS_null;}
         else
            {vr = SS_car(vlpair);
             vl = SS_cadr(vlpair);};

         SS_Assign(si->val,
		   SS_make_form(SS_setproc, vr, vl, LAST));
         SS_end_cons_macro(si->argl, si->this, si->val);
         SS_Assign(si->unev, SS_mk_cons(vr, si->unev));};

/* complete the transformation */
    frm = SS_mk_cons(si->unev, SS_append(si->argl, SS_cdr(letr)));
    SS_Assign(si->fun, frm);

/* process the let form */
    SS_Assign(si->exn, SS_let(si->fun));

/* clean up the mess */
    SS_Restore(si, si->val);
    SS_Restore(si, si->fun);
    SS_Restore(si, si->argl);
    SS_Restore(si, si->unev);
    SS_Restore(si, si->this);

    return(si->exn);}

/*--------------------------------------------------------------------------*/

#endif
#endif

/*--------------------------------------------------------------------------*/

/* SS_LST_MAP - prepare the args to map or for-each for procedure calls
 *            - cdr down the ARGL's
 *            - replace the ARGL with the cdr'd version
 *            - return the ARGS for the application
 */

static object *SS_lst_map(object *argl, int *Ex_flag)
   {object *args, *arg_nxt, *rest, *rest_nxt, *lst, *o;

    arg_nxt  = SS_null;
    rest_nxt = SS_null;

    for (args = SS_null, rest = SS_null; SS_consp(argl); argl = SS_cdr(argl))
        {lst = SS_car(argl);

/* taking the car of each arg LST make up the list of ARGS for proc */
         SS_end_cons(args, arg_nxt, SS_car(lst));

/* cons up a list with the REST of the argument lists */
         if (!SS_nullobjp(lst = SS_cdr(lst)))
            {SS_end_cons(rest, rest_nxt, lst);};

/* if we're at the end of any arg LST signal to exit the loop */
         if (!SS_consp(lst))
            *Ex_flag = TRUE;};

/* if there are no more ARG Lists reset ARGL to the REST of the arguments */
    o = SS_mk_cons(args, rest);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MAP - map for Scheme */

static object *_SSI_map(object *obj)
   {object *proc, *argl, *expr, *args, *vl, *ret, *ret_nxt;
    int exf;
    SS_psides *si;

    si = &_SS_si;

    ret_nxt = SS_null;

    proc = SS_car(obj);
    argl = SS_cdr(obj);
    if (!SS_consp(argl))
       return(proc);

    SS_MARK(argl);
    expr = SS_null;
    vl   = SS_null;
    args = SS_null;
    ret  = SS_null;
    for (exf = FALSE; !exf; )
        {SS_Assign(vl, SS_lst_map(argl, &exf));
         if (SS_consp(SS_caar(vl)))
            {SS_Assign(args, SS_mk_cons(SS_quoteproc, SS_car(vl)));
             SS_Assign(expr, SS_mk_cons(proc, SS_mk_cons(args, SS_null)));}
         else
            {SS_Assign(args, SS_car(vl));
             SS_Assign(expr, SS_mk_cons(proc, args));};
         SS_Assign(argl, SS_cdr(vl));
         SS_Save(si, si->env);
         SS_end_cons(ret, ret_nxt, SS_exp_eval(si, expr));
         SS_Restore(si, si->env);};

/* clean up the mess */
    SS_GC(expr);
    SS_GC(argl);
    SS_GC(args);
    SS_GC(vl);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_FOREACH - for-each for Scheme */

static object *_SSI_foreach(object *obj)
   {int exf;
    object *proc, *argl, *expr, *args, *vl;
    SS_psides *si;

    si = &_SS_si;

    proc = SS_car(obj);
    argl = SS_cdr(obj);
    if (!SS_consp(argl))
       return(proc);

    SS_MARK(argl);
    expr = SS_null;
    vl   = SS_null;
    args = SS_null;
    for (exf = FALSE; !exf; )
        {SS_Assign(vl, SS_lst_map(argl, &exf));
         if (SS_consp(SS_caar(vl)))
            {SS_Assign(args, SS_mk_cons(SS_quoteproc, SS_car(vl)));
             SS_Assign(expr, SS_mk_cons(proc, SS_mk_cons(args, SS_null)));}
         else
            {SS_Assign(args, SS_car(vl));
             SS_Assign(expr, SS_mk_cons(proc, args));};
         SS_Assign(argl, SS_cdr(vl));
         SS_Save(si, si->env);
         SS_exp_eval(si, expr);
         SS_Restore(si, si->env);};

/* clean up the mess */
    SS_GC(expr);
    SS_GC(argl);
    SS_GC(args);
    SS_GC(vl);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_NOT - not for Scheme */

static object *_SSI_not(object *obj)
   {object *o;

    o = (SS_true(obj)) ? SS_f : SS_t;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LIST - list in Scheme */

object *SS_list(object *argl)
   {

    return(argl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_WATCH - set a watch variable */

static object *_SSI_watch(object *obj)
   {

    if (SS_variablep(obj))
       {_SS.watch_var = obj;
	_SS.watch_val = SS_VARIABLE_VALUE(obj);}
    else
       SS_error("OBJECT NOT A VARIABLE - _SSI_WATCH", obj);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_UNWATCH - unset a watch variable */

static object *_SSI_unwatch(object *obj)
   {

    _SS.watch_var = NULL;
    _SS.watch_val = NULL;

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_TRACE - set the trace field in the procedure structs to TRUE */

static object *_SSI_trace(object *argl)
   {object *lst, *car;

    lst = argl;
    while ((car = SS_car(lst)) != NULL)
       {if (SS_procedurep(car))
           SS_PROCEDURE_TRACEDP(car) = TRUE;
        if (SS_nullobjp(lst = SS_cdr(lst)))
           break;};

    return(argl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_UNTRACE - set the trace field in the procedure structs to FALSE */

static object *_SSI_untrace(object *argl)
   {object *lst, *car;

    lst = argl;
    while ((car = SS_car(lst)) != NULL)
       {if (SS_procedurep(car))
           SS_PROCEDURE_TRACEDP(car) = FALSE;
        if (SS_nullobjp(lst = SS_cdr(lst)))
           break;};

    return(argl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHECK_ALL_OBJECTS - take a look at all objects for debugging */

static void _SS_check_all_objects(char *name, char *addr,
				  long length, int count, int type)
   {char atype[MAXLINE];
    char *on, *p;
    object *o;

    if (strcmp(name, "SS_MK_OBJECT:op") == 0)
       {o  = (object *) addr;
        on = SS_OBJECT_NAME(o);
	
	if (on == NULL)
	   on = "none";

	p = SS_object_type_name(o, atype);
	if (p == NULL)
	   snprintf(atype, MAXLINE, "%d", type);

	if (count == UNCOLLECT)
	   PRINT(stdout, "%8lx  perm %15s   %s\n", o, atype, on);
	else
	   PRINT(stdout, "%8lx %5d %15s   %s\n", o, count, atype, on);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_CHECK_BAD_NAMES - take a look at all objects with suspect
 *                     - print names for debugging
 */

static void _SS_check_bad_names(char *name, char *addr,
				long length, int count, int type)
   {char atype[MAXLINE];
    char *on, *p;
    object *o;

    if (strcmp(name, "SS_MK_OBJECT:op") == 0)
       {o  = (object *) addr;
        on = SS_OBJECT_NAME(o);
	
	if (on != NULL)
	   {if (!SS_stringp(o) && !SC_print_charsp(on, FALSE))
	       {p = SS_object_type_name(o, atype);
		if (p == NULL)
		   snprintf(atype, MAXLINE, "%d", type);

		if (count == UNCOLLECT)
		   PRINT(stdout, "%8lx  perm %15s   %s\n",
			 o, atype, on);
		else
		   PRINT(stdout, "%8lx %5d %15s   %s\n",
			 o, count, atype, on);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CHECK_OBJECTS - debugging object trace */

static object *_SSI_check_objects(object *obj)
   {int kind;
    long nm;
    object *ret;

    kind = -1;
    SS_args(obj,
	    SC_INT_I, &kind,
	    0);

    nm = 0L;

    switch (kind)
       {case 1 :
	     nm = SC_mem_object_trace(sizeof(object), -1,
				      _SS_check_all_objects);
	     break;

        case 2 :
	     nm = SC_mem_object_trace(sizeof(object), -1,
				      _SS_check_bad_names);
	     break;};

    ret = SS_mk_integer(nm);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DOT - debugging object trace */

long dot(int kind)
   {long nm;

    nm = 0L;

    switch (kind)
       {case 1 :
	     nm = SC_mem_object_trace(sizeof(object), -1,
				      _SS_check_all_objects);
	     break;

        case 2 :
	     nm = SC_mem_object_trace(sizeof(object), -1,
				      _SS_check_bad_names);
	     break;};

    return(nm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_DO_WATCH - watch a variable for a change in value */

static void SS_do_watch(object *pfun, char *msg)
   {object *vl;
    FILE *fp;
    SS_psides *si;

    si = &_SS_si;

    if (_SS.watch_var != NULL)
       {vl = SS_VARIABLE_VALUE(_SS.watch_var);
	if (vl != _SS.watch_val)
	   {fp = SS_OUTSTREAM(si->outdev);
	    SC_ASSERT(fp != NULL);

	    SS_print(_SS.watch_var, "Value of ", " changed", si->outdev);
	    SS_print(pfun, msg, "\n", si->outdev);
	    SS_print(_SS.watch_val, "   Old Value = ", "\n", si->outdev);
	    SS_print(vl,            "   New Value = ", "\n", si->outdev);

	    _SS.watch_val = vl;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BGN_TRACE - start a trace if the procedure is to be traced */

void SS_bgn_trace(object *pfun, object *pargl)
   {SS_psides *si;

    si = &_SS_si;

    SS_do_watch(pfun, "entering");

    switch (SS_PROCEDURE_TYPE(pfun))
       {case SS_PROC     :
        case SS_EE_MACRO :
        case SS_PR_PROC  :
	     if (SS_tracedp(pfun))
	        {PRINT(SS_OUTSTREAM(si->outdev),
		       "Entering procedure %s with:\n  ",
		       SS_PROCEDURE_NAME(pfun));
		 SS_print(pargl, "", "\n", si->outdev);
		 SS_Assign(si->continue_int[si->cont_ptr].signal,
			   pfun);};
	     break;

        case SS_MACRO_EV :
	     if (SS_tracedp(pfun))
	        {PRINT(SS_OUTSTREAM(si->outdev),
		       "Entering macro-ev %s with:\n  ",
		       SS_PROCEDURE_NAME(pfun));
		 SS_print(pargl, "", "\n", si->outdev);
		 SS_Assign(si->continue_int[si->cont_ptr].signal,
			   pfun);};
	     break;

        case SS_BEGIN    :
        case SS_COND     :
        case SS_IF       :
        case SS_AND      :
        case SS_OR       : 
        case SS_DEFINE   :
        case SS_MACRO    :
        case SS_UE_MACRO :
        case SS_UR_MACRO :
	     if (SS_tracedp(pfun))
	        {PRINT(SS_OUTSTREAM(si->outdev),
		       "Entering macro %s with:\n  ",
		       SS_PROCEDURE_NAME(pfun));
		 SS_print(pargl, "", "\n", si->outdev);
		 SS_Assign(si->continue_int[si->cont_ptr].signal,
			   pfun);};
	default :
	     break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_END_TRACE - check a continuation on its return for a traced procedure */

void SS_end_trace(void)
   {object *pfun;
    SS_psides *si;

    si = &_SS_si;

    pfun = si->continue_int[si->cont_ptr].signal;

    SS_do_watch(pfun, "leaving");

    if (SS_procedurep(pfun))
       {switch (SS_PROCEDURE_TYPE(pfun))
            {case SS_PROC     :
             case SS_EE_MACRO :
             case SS_PR_PROC  :
	          if (SS_tracedp(pfun))
		     {PRINT(SS_OUTSTREAM(si->outdev),
			    "Leaving procedure %s with:\n  ",
			    SS_PROCEDURE_NAME(pfun));
		      SS_print(si->val, "", "\n", si->outdev);};
		  break;

	     case SS_MACRO_EV :
	          if (SS_tracedp(pfun))
		     {PRINT(SS_OUTSTREAM(si->outdev),
			    "Leaving macro-ev %s with:\n  ",
			    SS_PROCEDURE_NAME(pfun));
		      SS_print(si->val, "", "\n", si->outdev);};
		  break;

             case SS_BEGIN    :
             case SS_COND     :
             case SS_IF       :
             case SS_AND      :
             case SS_OR       :
             case SS_DEFINE   :
             case SS_MACRO    :
             case SS_UE_MACRO :
             case SS_UR_MACRO :
	          if (SS_tracedp(pfun))
		     {PRINT(SS_OUTSTREAM(si->outdev),
			    "Leaving macro %s with:\n  ",
			    SS_PROCEDURE_NAME(pfun));
		      SS_print(si->val, "", "\n", si->outdev);};

	     default :
	          break;};

        SS_Assign(si->continue_int[si->cont_ptr].signal, SS_null);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CATCH - the entry part of call-with-current-continuation */

static object *_SSI_catch(object *obj)
   {object *escape, *ret, *lst;
    SS_psides *si;

    si = &_SS_si;

    escape = SS_mk_esc_proc(si->errlev, SS_PROCEDURE_I);

    lst    = SS_make_list(SS_OBJECT_I, obj,
			  SS_OBJECT_I, escape,
			  0);

/* Use the fact that SS_Assign(si->exn, obj) is done in SS_eval to provide the
 * only reference to lst so that GC is done properly
 */
    ret = SS_exp_eval(si, lst);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CATCH_ERR - the entry part of err-catch */

static object *_SSI_catch_err(object *argl)
   {object *err_proc, *proc_call, *err_ev, *ret;
    object *esc;
    SS_psides *si;

    si = &_SS_si;

    ret = SS_null;

    _SS.oph = SS_get_print_err_func();

    err_proc  = SS_null;
    proc_call = SS_null;
    SS_args(argl,
	    SS_OBJECT_I, &err_proc,
	    SS_OBJECT_I, &proc_call,
	    0);

/* set the stage for error handling at the SCHEME level */
    SS_Assign(si->err_state, SS_null);
    SS_set_print_err_func(NULL, FALSE);

    si->cont_ptr++;
    SS_push_err(FALSE, SS_ERROR_I);
    switch (SETJMP(si->continue_int[si->cont_ptr].cont))
       {case ABORT :
	     SS_set_print_err_func(_SS.oph, FALSE);

	     err_ev = SS_mk_cons(err_proc, si->err_state);
	     ret    = SS_exp_eval(si, err_ev);
	     break;

        case RETURN_OK :
	     ret = si->val;
	     break;

        default :
	     ret = SS_exp_eval(si, proc_call);

        case ERR_FREE :
	     esc = SS_pop_err(si->errlev - 1, FALSE);
	     SS_GC(esc);
	     ret = SS_null;};

    si->cont_ptr--;

    SS_set_print_err_func(_SS.oph, FALSE);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_TIME - return the time from Scheme as
 *           - (yy mm dd hh mm ss) all are ints
 */

static object *_SSI_time(void)
   {struct tm safe, *loct;
    time_t syst;
    object *o;

    syst = time(NULL);
    loct = SC_localtime(&syst, &safe);

/* make the month right */
    loct->tm_mon++;

    o = SS_make_list(SC_INT_I, &loct->tm_year,
		     SC_INT_I, &loct->tm_mon,
		     SC_INT_I, &loct->tm_mday,
		     SC_INT_I, &loct->tm_hour,
		     SC_INT_I, &loct->tm_min,
		     SC_INT_I, &loct->tm_sec,
		     0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_ETIME - return the time since the given time as
 *            - usage: (time-elapsed <ref> <fl>)
 *            -   if <fl> is #t the time is returned in seconds
 *            -   if <fl> is #f the time is returned as (ww dd hh mm ss)
 *            -           where all are ints and
 *            -           maximum elapsed time measured in weeks
 */

static object *_SSI_etime(object *argl)
   {int i;
    long r, dt[5];
    double since;
    struct tm ref;
    time_t now, then;
    object *obj, *rv, *fl;
    static long secsper[5] = {7*24*3600, 24*3600, 3600, 60, 1};

    SC_MEM_INIT(struct tm, &ref);

    ref.tm_mday  = 1;        /* day of the month */
    ref.tm_year  = 100;      /* year */
    ref.tm_isdst = -1;       /* daylight saving time */

    obj = SS_null;
    fl  = SS_t;
    SS_args(argl,
	    SS_OBJECT_I, &obj,
	    SS_OBJECT_I, &fl,
	    0);

    SS_args(obj,
	    SC_INT_I, &ref.tm_year,
	    SC_INT_I, &ref.tm_mon,
	    SC_INT_I, &ref.tm_mday,
	    SC_INT_I, &ref.tm_hour,
	    SC_INT_I, &ref.tm_min,
	    SC_INT_I, &ref.tm_sec,
	    0);

/* make the month right */
    ref.tm_mon--;

    then  = mktime(&ref);
    now   = time(NULL);

    if (SS_true(fl))
       {since = now - then;
	rv = SS_mk_float(since);}

    else
       {r = now - then;
	for (i = 0; i < 5; i++)
	    {dt[i] = r / secsper[i];
	     r    %= secsper[i];};

	rv = SS_make_list(SC_INT_I, &dt[0],
			  SC_INT_I, &dt[1],
			  SC_INT_I, &dt[2],
			  SC_INT_I, &dt[3],
			  SC_INT_I, &dt[4],
			  0);};
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_UNLINK - unlink the specified file
 *             - Usage: (unlink <file-name>)
 *             - return #t iff successful
 */

static object *_SSI_unlink(object *argl)
   {int ok;
    char *path;
    object *rv;

    path = NULL;
    SS_args(argl,
	    SC_STRING_I, &path,
	    0);

    ok = -1;
    if ((path != NULL) && (strcmp(path, "nil") != 0))
       ok = unlink(path);

    rv = (ok == 0) ? SS_t : SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_PRM1 - install the Scheme primitives */

void _SS_inst_prm1(void)
   {

    SS_install("call-with-cc",
               "Procedure: pass an escape procedure to the argument, a procedure of one argument",
               SS_sargs,
               _SSI_catch, SS_PR_PROC);

    SS_install("check-objects",
               "Procedure: check objects: 1 - all, 2 - bad names",
               SS_nargs,
               _SSI_check_objects, SS_PR_PROC);

    SS_install("err-catch",
               "Special Form: Calls the given error procedure if the procedure call fails",
               SS_nargs,
               _SSI_catch_err, SS_UR_MACRO);

    SS_install("for-each",
               "Special Form: applies a procedure over a set of lists",
               SS_nargs,
               _SSI_foreach, SS_PR_PROC);

    SS_install("lambda",
               "Special Form: (lambda (<parameters>) <exp1> ... <expn>)",
               SS_nargs,
               _SSI_lambda, SS_UR_MACRO);

    SS_install("lambda-macro",
               "Special Form: (lambda-macro (<parameters>) <exp1> ... <expn>)",
               SS_nargs,
               _SSI_lambdam, SS_UR_MACRO);

    SS_install("let",
               "Special Form: define local variables using lambda binding",
               SS_nargs,
               SS_let, SS_UE_MACRO);

    SS_install("let*",
               "Special Form: define local variables using lambda binding and maximal scoping",
               SS_nargs,
               _SSI_letstr, SS_UE_MACRO);

    SS_install("make-new-symbol",
               "Procedure: generate a new symbol",
               SS_sargs,
               _SSI_newsym, SS_PR_PROC);

    SS_install("map",
               "Special Form: maps a procedure over a set of lists",
               SS_nargs,
               _SSI_map, SS_PR_PROC);

    SS_install("not",
               "Procedure: Returns #t if object is #f and #f for any other object",
               SS_sargs,
               _SSI_not, SS_PR_PROC);

    SS_install("quasiquote",
               "Special Form: Like quote except that unquote and unquote-splicing forms are eval'd",
               SS_sargs,
               _SSI_quasiq, SS_UR_MACRO);

    SS_install("quote",
               "Special Form: (quote x) -> x",
               SS_sargs,
               _SSI_quote, SS_UR_MACRO);

    SS_install("time",
               "Procedure: return current time as (yy mm dd hh mm ss)",
               SS_zargs,
               _SSI_time, SS_PR_PROC);

    SS_install("time-elapsed",
               "Procedure: return time elapsed since the given time",
               SS_nargs,
               _SSI_etime, SS_PR_PROC);

    SS_install("trace",
               "Procedure: 'trace's calls to the procedures in the list of arguments",
               SS_nargs,
               _SSI_trace, SS_PR_PROC);

    SS_install("unlink",
               "Procedure: remove or unlink the named file",
               SS_nargs,
               _SSI_unlink, SS_PR_PROC);

    SS_install("unquote",
               "Special Form: In a quasiquote'd form inserts the result of evaluating\n its argument",
               SS_sargs,
               _SSI_unquote, SS_UR_MACRO);

    SS_install("unquote-splicing",
               "Special Form: In a quasiquote'd form splices the eval'd list into the quoted form",
               SS_sargs,
               _SSI_unq_spl, SS_UR_MACRO);

    SS_install("untrace",
               "Procedure: removes the 'trace' from the procedures in the argument list",
               SS_nargs,
               _SSI_untrace, SS_PR_PROC);

    SS_install("unwatch",
               "Procedure: remove a watch on the specified variable",
               SS_sargs,
               _SSI_unwatch, SS_PR_PROC);

    SS_install("watch",
               "Procedure: put a watch on the specified variable",
               SS_sargs,
               _SSI_watch, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
