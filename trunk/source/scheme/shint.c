/*
 * SHINT.C - C interface routines for SCHEME
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GET_TYPE_METHOD - fetch the methods associated with TYPE */

int SS_get_type_method(int type, ...)
   {int i, ok, rv;
    char *mn;
    pcons *alst;
    PFPVoid *pmf;
    SC_type *ty;

    SC_VA_START(type);

    ty = _SC_get_type_id(type);

/* if the type is found set all the function pointers */
    if (ty != NULL)
       {alst = ty->a;

	ok = TRUE;
	for (i = 0; ok == TRUE; i++)
	    {mn = SC_VA_ARG(char *);
	     if (mn == NULL)
	        ok = FALSE;
	     else
	        {pmf  = SC_VA_ARG(PFPVoid *);
		 *pmf = SC_assoc(alst, mn);};};

	rv = TRUE;}

/* if the type is not found NULL out all the function pointers */
    else
       {ok = TRUE;
	for (i = 0; ok == TRUE; i++)
	    {mn = SC_VA_ARG(char *);
	     if (mn == NULL)
	        ok = FALSE;
	     else
	        {pmf  = SC_VA_ARG(PFPVoid *);
		 *pmf = NULL;};};

	rv = FALSE;};

    SC_VA_END;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_TYPE_METHOD - set the methods associated with TYPE */

int SS_set_type_method(int type, ...)
   {int i, ok, rv;
    char *tn, *mn;
    pcons *alst;
    PFPVoid mf;
    SC_type *ty;

    SC_VA_START(type);

    ty = _SC_get_type_id(type);
    if (ty != NULL)
       {tn   = ty->type;
	alst = ty->a;

	ok = TRUE;
	for (i = 0; ok == TRUE; i++)
	    {mn = SC_VA_ARG(char *);
	     if (mn == NULL)
	        ok = FALSE;
	     else
	        {mf   = SC_VA_ARG(PFPVoid);
		 alst = SC_add_alist(alst, mn, tn, mf);};};

	ty->a = alst;

	rv = TRUE;}
    else
       rv = FALSE;

    SC_VA_END;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LOAD_BF - load the temporary string buffer */

static char *_SS_load_bf(char *s)
   {int nc;

    nc = strlen(s);
    if (nc >= _SS.ncx)
       {_SS.ncx = nc + 1;
	if (_SS.ibf != NULL)
	   {CFREE(_SS.ibf);};

	_SS.ibf = CMAKE_N(char, _SS.ncx);};

    SC_strncpy(_SS.ibf, _SS.ncx, s, strlen(s));

    return(_SS.ibf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_FIX_ARG - get a C level fix point data item from
 *             - a single Scheme object
 */

static void _SS_fix_arg(SS_psides *si, object *obj, void *v, int type)
   {int ok;

    ok = _SS_object_to_numtype_id(type, v, 0, obj);
    if (ok == FALSE)
       {int64_t l;

	l = 0L;

	if (SS_nullobjp(obj))
	   l = 0;

	else if (obj == SS_t)
	   l = TRUE;

        else if (obj == SS_f)
	   l = FALSE;

	else if (SS_procedurep(obj))
	   {PFVoid hand;
	    SC_address u;

	    switch (SS_PROCEDURE_TYPE(obj))
	       {case SS_MACRO : 
                case SS_PROC  :
		     SS_error(si, "CAN'T MAKE VALUE - _SS_FIX_ARG", obj);
		     break;

		default :
		     hand       = (PFVoid) SS_C_PROCEDURE_HANDLER_PTR(obj);
		     u.funcaddr = (PFInt) SS_C_PROCEDURE_FUNCTION_PTR(obj);
		     if (hand == (PFVoid) SS_acc_char)
		        l = *(char *) u.memaddr;
		     else if (hand == (PFVoid) SS_acc_int)
		        l = *(int *) u.memaddr;
		     else if (hand == (PFVoid) SS_acc_long)
		        l = *(long *) u.memaddr;
		     else if (hand == (PFVoid) SS_acc_double)
		        l = *(double *) u.memaddr;
		     else
		        SS_error(si, "BAD VARIABLE TYPE - _SS_FIX_ARG", obj);};}

	else
	   SS_error(si, "BAD OBJECT - _SS_FIX_ARG", obj);

	SC_convert_id(type, v, 0, 1, SC_LONG_LONG_I, &l, 0, 1, 1, FALSE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_FLOAT_ARG - get a C level floating point data item from
 *               - a single Scheme object
 */

static void _SS_float_arg(SS_psides *si, object *obj, void *v, int type)
   {int ok;

    ok = _SS_object_to_numtype_id(type, v, 0, obj);
    if (ok == FALSE)
       {long double d;

	d = 0.0L;

	if (SS_nullobjp(obj))
	   d = 0.0L;

	else if (SS_procedurep(obj))
	   {PFVoid hand;
	    SC_address u;

	    switch (SS_PROCEDURE_TYPE(obj))
	       {case SS_MACRO : 
		case SS_PROC  :
		     SS_error(si, "CAN'T MAKE VALUE - _SS_FLOAT_ARG", obj);
		     break;

		default :
		     hand       = (PFVoid) SS_C_PROCEDURE_HANDLER_PTR(obj);
		     u.funcaddr = (PFInt) SS_C_PROCEDURE_FUNCTION_PTR(obj);
		     if (hand == (PFVoid) SS_acc_char)
		        d = *(char *) u.memaddr;
		     else if (hand == (PFVoid) SS_acc_int)
		        d = *(int *) u.memaddr;
		     else if (hand == (PFVoid) SS_acc_long)
		        d = *(long *) u.memaddr;
		     else if (hand == (PFVoid) SS_acc_double)
		        d = *(double *) u.memaddr;
		     else
		        SS_error(si, "BAD VARIABLE TYPE - _SS_FLOAT_ARG", obj);};}

	else
	   SS_error(si, "BAD OBJECT - _SS_FLOAT_ARG", obj);

	SC_convert_id(type, v, 0, 1, SC_LONG_DOUBLE_I, &d, 0, 1, 1, FALSE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_COMPLEX_ARG - get a C level complex floating point data item from
 *                 - a single Scheme object
 */

static void _SS_complex_arg(SS_psides *si, object *obj, void *v)
   {int ok;

    ok = _SS_object_to_numtype_id(SC_DOUBLE_COMPLEX_I, v, 0, obj);
    if (ok == FALSE)
       {double _Complex z, *zp;

	z = 0.0;

	if (SS_nullobjp(obj))
	   z = 0.0;

	else if (SS_procedurep(obj))
	   {PFVoid hand;
	    SC_address u;

	    switch (SS_PROCEDURE_TYPE(obj))
	       {case SS_MACRO : 
		case SS_PROC  :
		     SS_error(si, "CAN'T MAKE VALUE - _SS_COMPLEX_ARG", obj);
		     break;

		default :
		     hand       = (PFVoid) SS_C_PROCEDURE_HANDLER_PTR(obj);
		     u.funcaddr = (PFInt) SS_C_PROCEDURE_FUNCTION_PTR(obj);
		     if (hand == (PFVoid) SS_acc_char)
		        z = *(char *) u.memaddr;
		     else if (hand == (PFVoid) SS_acc_int)
		        z = *(int *) u.memaddr;
		     else if (hand == (PFVoid) SS_acc_long)
		        z = *(long *) u.memaddr;
		     else if (hand == (PFVoid) SS_acc_double)
		        z = *(double *) u.memaddr;
		     else
		        SS_error(si, "BAD VARIABLE TYPE - _SS_COMPLEX_ARG", obj);};}

	else
	   SS_error(si, "BAD OBJECT - _SS_COMPLEX_ARG", obj);

	zp  = (double _Complex *) v;
	*zp = z;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_QUATERNION_ARG - get a C level quaternion data item from
 *                    - a single Scheme object
 */

static void _SS_quaternion_arg(SS_psides *si, object *obj, void *v)
   {quaternion q, *qp;
    double _Complex z;

    q.s = 0.0;
    q.i = 0.0;
    q.j = 0.0;
    q.k = 0.0;

    if (SS_quaternionp(obj))
       q = SS_QUATERNION_VALUE(obj);

    else if (SS_complexp(obj))
       {z   = SS_COMPLEX_VALUE(obj);
	q.s = creal(z);
	q.i = cimag(z);}

    else if (SS_integerp(obj))
       q.s = SS_INTEGER_VALUE(obj);

    else if (SS_floatp(obj))
       q.s = SS_FLOAT_VALUE(obj);

    else if (SS_charobjp(obj))
       q.s = SS_CHARACTER_VALUE(obj);

    else if (SS_procedurep(obj))
       {PFVoid hand;
        SC_address u;

        switch (SS_PROCEDURE_TYPE(obj))
           {case SS_MACRO : 
            case SS_PROC  :
                 SS_error(si, "CAN'T MAKE VALUE - _SS_QUATERNION_ARG", obj);
                 break;

            default :
                 hand       = (PFVoid) SS_C_PROCEDURE_HANDLER_PTR(obj);
                 u.funcaddr = (PFInt) SS_C_PROCEDURE_FUNCTION_PTR(obj);
                 if (hand == (PFVoid) SS_acc_char)
                    q.s = *(char *) u.memaddr;
                 else if (hand == (PFVoid) SS_acc_int)
                    q.s = *(int *) u.memaddr;
                 else if (hand == (PFVoid) SS_acc_long)
                    q.s = *(long *) u.memaddr;
                 else if (hand == (PFVoid) SS_acc_double)
                    q.s = *(double *) u.memaddr;
                 else
                    SS_error(si, "BAD VARIABLE TYPE - _SS_QUATERNION_ARG", obj);};}

    else if (SS_nullobjp(obj))
       q.s = 0.0;

    else
       SS_error(si, "BAD OBJECT - _SS_QUATERNION_ARG", obj);

    qp  = (quaternion *) v;
    *qp = q;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_ARGS - get a C level data item from a single SCHEME object */

static void _SS_args(SS_psides *si, object *obj, void *v, int type)
   {void **pv;
    char *s;
    void *(*f)(SS_psides *si, object *o);
    SS_procedure *pp;

    pv = (void **) v;

/* if the object has been GC'd along the line NULL out the C level item */
    if (obj->val == NULL)
       {DEREF(v) = NULL;
        return;};

    if (SC_is_type_char(type) == TRUE)
       _SS_fix_arg(si, obj, v, type);

    else if ((SC_is_type_fix(type) == TRUE) ||
	     (type == SC_BOOL_I))
       _SS_fix_arg(si, obj, v, type);

    else if (SC_is_type_fp(type) == TRUE)
       _SS_float_arg(si, obj, v, type);

    else if (SC_is_type_cx(type) == TRUE)
       _SS_complex_arg(si, obj, v);

    else if (type == SC_QUATERNION_I)
       _SS_quaternion_arg(si, obj, v);

    else if (type == SC_STRING_I)
       {s = _SS_get_print_name(si, obj);
	DEREF(v) = s;}

    else if (type == G_OBJECT_I)
       *pv = (void *) obj;

    else if (type == G_SS_PROCEDURE_I)
       {pp  = (SS_procedure *) obj->val;
	*pv = (void *) pp->proc;}

    else if (type == G_SS_INPUT_PORT_I)
       *pv = (void *) SS_INSTREAM(obj);

    else if (type == G_SS_OUTPUT_PORT_I)
       *pv = (void *) SS_OUTSTREAM(obj);

#ifdef LARGE

    else if (type == G_HAELEM_I)
       {if (!SS_haelemp(obj))
	   SS_error(si, "OBJECT NOT HASH_ELEMENT - _SS_ARGS", obj);
	*pv = obj->val;}

    else if (type == G_HASHARR_I)
       {if (!SS_nullobjp(obj))
	   {if (!SS_hasharrp(obj))
	       SS_error(si, "OBJECT NOT HASH_ARRAY - _SS_ARGS", obj);
	    *pv = obj->val;};}

    else if (type == G_PROCESS_I)
       {if (!SS_processp(obj))
	   SS_error(si, "OBJECT NOT PROCESS - _SS_ARGS", obj);
	*pv = obj->val;}

    else if (type == G_SS_VECTOR_I)
       {if (!SS_vectorp(obj))
	   SS_error(si, "OBJECT NOT VECTOR - _SS_ARGS", obj);
	*pv = obj->val;}

#endif

    else
       {SS_get_type_method(type, "Scheme->C", &f, NULL);
	if (f != NULL)
	   *pv = f(si, obj);
	else if (si->get_arg != NULL)
	   si->get_arg(si, obj, v, type);
        else
	   *pv = obj->val;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ARGS - break out the C level data analogs from a Scheme
 *         - object of list of objects
 *         - this is for ease of use
 *         - return the number of Scheme arguments
 */

int SS_args(SS_psides *si, object *s, ...)
   {object *obj;
    void *v;
    int type, len;

    if (SS_nullobjp(s))
       return(0);

    SC_VA_START(s);

    if (!SS_consp(s))
       {len  = 1;
        obj  = s;
        type = SC_VA_ARG(int);
        if (type != 0)
           {v = SC_VA_ARG(void *);
            _SS_args(si, obj, v, type);};}
    else
       {len = SS_length(si, s);
        while (SS_consp(s))
           {obj = SS_car(si, s);
            s   = SS_cdr(si, s);

            type = SC_VA_ARG(int);
            if (type == 0)
               break;

            v = SC_VA_ARG(void *);
            if (v == (void *) LAST)
               break;

            _SS_args(si, obj, v, type);};};

    SC_VA_END;

    return(len);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_DEFINE_CONSTANT - bind a given compiled value as
 *                    - a SCHEME variable of a specified type
 */

object *SS_define_constant(SS_psides *si, int n, ...)
   {int type;
    char *name;
    object *vr, *val;

    vr  = SS_null;
    val = SS_null;

    SC_VA_START(n);

    while ((name = SC_VA_ARG(char *)) != NULL)
      {type = SC_VA_ARG(int);

/* character types (proper) */
       if (SC_is_type_char(type) == TRUE)
	  {long long v;
	   SC_VA_ARG_FETCH(SC_LONG_LONG_I, &v, type);
	   val = SS_mk_integer(si, v);}

/* fixed point types (proper) */
       else if (SC_is_type_fix(type) == TRUE)
	  {long long v;
	   SC_VA_ARG_FETCH(SC_LONG_LONG_I, &v, type);
	   val = SS_mk_integer(si, v);}

/* floating point types (proper) */
       else if (SC_is_type_fp(type) == TRUE)
	  {long double v;
	   SC_VA_ARG_FETCH(SC_LONG_DOUBLE_I, &v, type);
	   val = SS_mk_float(si, v);}

/* complex floating point types (proper) */
       else if (SC_is_type_cx(type) == TRUE)
	  {long double _Complex v;
	   SC_VA_ARG_FETCH(SC_LONG_DOUBLE_COMPLEX_I, &v, type);
	   val = SS_mk_complex(si, v);}

       else if (type == SC_STRING_I)
	  {char *v;
	   v   = SC_VA_ARG(char *);
	   val = SS_mk_string(si, v);}

       else if (type == G_OBJECT_I)
	  val = SC_VA_ARG(object *);

       else
	  SS_error(si, "UNSUPPORTED TYPE - SS_DEFINE_CONSTANT",
		   SS_null);

       vr = SS_mk_variable(si, name, SS_null);
       SS_UNCOLLECT(vr);

       SC_hasharr_install(si->symtab, name, vr, SS_OBJECT_P_S, 3, -1);
       SS_def_var(si, vr, val, si->global_env);};

    SC_VA_END;

    return(vr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_VAR_VALUE - given the name of a current SCHEME object which
 *              - has correspondence with a C or FORTRAN variable
 *              - make the given pointer point to its value
 *              - if flag is TRUE and the object is a variable return its
 *              - binding in the current environment
 */

void SS_var_value(SS_psides *si, char *s, int type, void *vr, int flag)
   {object *obj;

    obj = SS_INQUIRE_OBJECT(si, s);

    if (flag && SS_variablep(obj))
       obj = SS_lk_var_val(si, obj);

    if ((obj == NULL) || SS_nullobjp(obj))
       DEREF(vr) = NULL;

    else
       SS_args(si, obj, type, vr, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_VAR_REFERENCE - given the name of a current SCHEME object which
 *                  - has correspondence with a C or FORTRAN variable
 *                  - return a pointer to its value
 */

void *SS_var_reference(SS_psides *si, char *s)
   {object *obj;
    void *vr;

    vr = NULL;

    obj = SS_INQUIRE_OBJECT(si, s);
    if (SS_variablep(obj))
       obj = SS_lk_var_val(si, obj);

    if (SS_integerp(obj) || SS_floatp(obj) || SS_charobjp(obj))
       vr = SS_OBJECT(obj);

    else if (SS_stringp(obj))
       vr = SS_STRING_TEXT(obj);

    else if (SS_procedurep(obj))
       {PFVoid hand;
        SC_address u;

        switch (SS_PROCEDURE_TYPE(obj))
           {case SS_MACRO : 
            case SS_PROC  :
                 SS_error(si, "CAN'T MAKE VALUE - _SS_FLOAT_ARG", obj);
                 break;

            default       :
                 hand       = (PFVoid) SS_C_PROCEDURE_HANDLER_PTR(obj);
                 u.funcaddr = (PFInt) SS_C_PROCEDURE_FUNCTION_PTR(obj);
                 if ((hand == (PFVoid) SS_acc_char) ||
                     (hand == (PFVoid) SS_acc_int) ||
                     (hand == (PFVoid) SS_acc_long) ||
                     (hand == (PFVoid) SS_acc_double))
                    vr = u.memaddr;
                 else
                    SS_error(si, "BAD VARIABLE TYPE - SS_VAR_REFERENCE", obj);};}
    else
       SS_error(si, "BAD OBJECT - SS_VAR_REFERENCE", obj);

    return(vr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAKE_LIST - make a SCHEME list from a C arg list */

object *_SS_make_list(SS_psides *si, int n, int *type, void **ptr)
   {int i, c, ityp;
    char *s;
    void *vl;
    object *o, *lst;

    lst = SS_null;
    for (i = 0; i < n; i++)
        {ityp = type[i];
	 vl   = ptr[i];

/* fixed point types (proper) */
	 if (SC_is_type_fix(ityp) == TRUE)
	    {long long v;

	     SC_convert_id(SC_LONG_LONG_I, &v, 0, 1,
			   ityp, vl, 0, 1, 1, FALSE);
	     lst = SS_mk_cons(si, SS_mk_integer(si, v), lst);}

/* floating point types (proper) */
	 else if (SC_is_type_fp(ityp) == TRUE)
	    {long double v;

	     SC_convert_id(SC_LONG_DOUBLE_I, &v, 0, 1,
			   ityp, vl, 0, 1, 1, FALSE);
	     lst = SS_mk_cons(si, SS_mk_float(si, v), lst);}

/* complex floating point types (proper) */
	 else if (SC_is_type_cx(ityp) == TRUE)
	    {long double _Complex v;

	     SC_convert_id(SC_LONG_DOUBLE_COMPLEX_I, &v, 0, 1,
			   ityp, vl, 0, 1, 1, FALSE);
	     lst = SS_mk_cons(si, SS_mk_complex(si, v), lst);}
 
	 else if (ityp == SC_STRING_I)
	    {s   = (char *) vl;
	     lst = SS_mk_cons(si, SS_mk_string(si, s), lst);}
 
	 else if (ityp == G_OBJECT_I)
	    {o   = (object *) vl;
	     lst = SS_mk_cons(si, o, lst);}
 
#ifdef LARGE
 
/* character types (proper) */
	 else if (SC_is_type_char(ityp) == TRUE)
	    {c   = *(int *) vl;
	     lst = SS_mk_cons(si, SS_mk_char(si, c), lst);}
 
	 else if (ityp == G_HAELEM_I)
	    {haelem *hp;
	     hp  = (haelem *) vl;
	     lst = SS_mk_cons(si, SS_mk_haelem(si, hp), lst);}
 
	 else if (ityp == G_HASHARR_I)
	    {hasharr *ht;
	     ht  = (hasharr *) vl;
	     lst = SS_mk_cons(si, SS_mk_hasharr(si, ht), lst);}

	 else if (ityp == G_PROCESS_I)
	    {PROCESS *pp;
	     pp  = (PROCESS *) vl;
	     lst = SS_mk_cons(si, SS_mk_process(si, pp), lst);}

#endif
 
	 else
	    {if (si->call_arg != NULL)
	        {o   = si->call_arg(si, type[i], vl);
		 lst = SS_mk_cons(si, o, lst);}
	     else
	        lst = SS_mk_cons(si, SS_null, lst);};};

    o = SS_reverse(si, lst);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MAKE_LIST - make a SCHEME list at the C level */

object *SS_make_list(SS_psides *si, int first, ...)
   {int i, type[MAXLINE];
    void *ptr[MAXLINE];
    object *o;

    SC_VA_START(first);

    type[0] = first;
    ptr[0]  = SC_VA_ARG(void *);

    for (i = 1; i < MAXLINE; i++)
        {type[i] = SC_VA_ARG(int);
         if (type[i] == 0)
            break;

         ptr[i]  = SC_VA_ARG(void *);
         if (ptr[i] == (void *) LAST)
            break;};

    SC_VA_END;

    o = _SS_make_list(si, i, type, ptr);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MAKE_FORM - make a SCHEME form which is a list of objects */

object *SS_make_form(SS_psides *si, object *first, ...)
   {int i, type[MAXLINE];
    void *ptr[MAXLINE];
    object *expr;

    type[0] = G_OBJECT_I;
    ptr[0]  = first;

    SC_VA_START(first);

    for (i = 1; i < MAXLINE; i++)
        {type[i] = G_OBJECT_I;
         ptr[i]  = SC_VA_ARG(void *);
         if ((ptr[i] == (void *) LAST) || (ptr[i] == NULL))
            break;};

    SC_VA_END;

    expr = _SS_make_list(si, i, type, ptr);

    return(expr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_EVAL_FORM - make and evaluate a SCHEME form
 *              - which is a list of objects
 */

object *SS_eval_form(SS_psides *si, object *first, ...)
   {int i, type[MAXLINE];
    void *ptr[MAXLINE];
    object *expr, *res;

    type[0] = G_OBJECT_I;
    ptr[0]  = first;

    SC_VA_START(first);

    for (i = 1; i < MAXLINE; i++)
        {type[i] = G_OBJECT_I;
         ptr[i]  = SC_VA_ARG(void *);
         if ((ptr[i] == (void *) LAST) || (ptr[i] == NULL))
            break;};

    SC_VA_END;

    expr = SS_null;
    SS_assign(si, expr, _SS_make_list(si, i, type, ptr));
    res = SS_exp_eval(si, expr);
    SS_assign(si, expr, SS_null);

    return(res);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CALL_SCHEME - make a C level call to a SCHEME level function
 *                - return the SCHEME result object
 *
 * #bind SS_call_scheme  fortran()
 */

object *SS_call_scheme(SS_psides *si, char *func, ...)
   {int i, type[MAXLINE];
    object *fnc, *expr, *rv;
    void *ptr[MAXLINE];

    fnc = _SS_lk_var_valc(si, func, si->env);
/*    fnc = (object *) SC_hasharr_def_lookup(si->symtab, func); */
    if ((fnc == NULL) || (SS_procedurep(fnc) == FALSE))
       {rv = SS_f;
        io_printf(stdout,
		  "Unknown procedure '%s' to SS_call_scheme - ignored\n",
		  func);}
    else
       {SC_VA_START(func);

        for (i = 0; i < MAXLINE; i++)
	    {type[i] = SC_VA_ARG(int);
	     if (type[i] == 0)
	        break;

	     ptr[i] = SC_VA_ARG(void *);
	     if (ptr[i] == (void *) LAST)
	        break;};

	SC_VA_END;

	SC_mem_stats_set(0L, 0L);

	expr = SS_mk_cons(si, fnc, _SS_make_list(si, i, type, ptr));
	SC_mark(expr, 1);

	SS_eval(si, expr);

	SS_gc(si, expr);

	SS_assign(si, si->env, si->global_env);
	SS_assign(si, si->this, SS_null);
	SS_assign(si, si->exn, SS_null);
	SS_assign(si, si->unev, SS_null);
	SS_assign(si, si->argl, SS_null);
	SS_assign(si, si->fun, SS_null);

	rv = si->val;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RUN - evaluate a single form which is given on the command line 
 *         - as a string
 */

static int _SS_run(SS_psides *si)
   {int iret;
    object *port, *ret;

    port = SS_mk_inport(si, stdin, "stdin");
    SC_strncpy(SS_BUFFER(port), MAX_BFSZ, _SS.ibf, strlen(_SS.ibf));
    SS_PTR(port) = SS_BUFFER(port);

    ret  = SS_eval(si, SS_read(si, port));
    iret = FALSE;
    if (SS_numbp(ret))
       SS_args(si, ret,
	       SC_INT_I, &iret,
	       0);

    else
       iret = SS_true(ret);

    return(iret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RUN - evaluate a single form which is given on the command line 
 *        - as a string
 */

int SS_run(SS_psides *si, char *s)
   {int rv;

    while (strchr(" \t\n\r\f", *s++) != NULL);
    _SS_load_bf(--s);

    rv = SS_err_catch(si, _SS_run, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LOAD_SCM - load an SCHEME file with error protection */

static int _SS_load_scm(SS_psides *si)
   {

    SS_call_scheme(si, "load",
                   SC_STRING_I, _SS.ibf,
                   G_OBJECT_I, SS_t,
                   0);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LOAD_SCM - load a SCHEME file with error protection */

int SS_load_scm(SS_psides *si, char *fmt, ...)
   {int rv;
    char fn[BFLRG];
    char *s;

    SC_VA_START(fmt);
    SC_VSNPRINTF(fn, BFLRG, fmt);
    SC_VA_END;

    s = SC_search_file(NULL, fn);
    if ((s == NULL) || !SC_query_file(s, "r", "ascii"))
       rv = FALSE;
    else
       {_SS_load_bf(s);
	rv = SS_err_catch(si, _SS_load_scm, NULL);};

    CFREE(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_TEXT_DATA_FILEP - test for numeric text data files
 *                    - look for comment lines beginning with CMNT
 *                    - all other non-blank lines should have number
 *                    - return TRUE if FP conforms to this pattern
 */

int SS_text_data_filep(char *fname, int cmnt)
   {int ok, nb;
    char *bf, *s, *p, *token;
    FILE *fp;

    fp = io_open(fname, "r");

    bf = NULL;
    nb = 0;

    for (ok = TRUE; ok == TRUE; )
        {bf = SC_dgets(bf, &nb, fp);
	 if (bf == NULL)
	    break;

	 if (bf[0] == cmnt)
            continue;

	 for (s = bf; TRUE; s = NULL)
	     {token = SC_strtok(s, " \t\f\n\r", p);
	      if (token == NULL)
	         break;
	      else if (SC_numstrp(token) == FALSE)
	         {ok = FALSE;
		  break;};};};

    io_close(fp);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_EXE_SCRIPT - return the name of a file if the command line indicates
 *               - running a SCHEME script
 *               - determine whether or not this is a script
 *               - by looking at the first argument which is not a flag,
 *               - not a -l <file> combination, and names an ASCII file
 *               - beginning with '#!'
 */

char *SS_exe_script(int c, char **v)
   {int i, ok;
    char bf[MAXLINE];
    char *p, *script;
    FILE *fp;

    script = NULL;
    for (i = 1; (i < c) && (script == NULL); i++)
        {if ((v[i][0] != '-') && SC_isfile_text(v[i]) &&
	     (strcmp(v[i-1], "-l") != 0))
	    {fp = io_open(v[i], "r");
	     p  = io_gets(bf, MAXLINE, fp);
	     io_close(fp);

/* if file begins with "#!" it is a script */
	     ok = ((p != NULL) && (p[0] == '#') && (p[1] == '!'));
	     if (ok == TRUE)
		script = v[i];};};

    return(script);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_DEFINE_ARGV - save the command line args in a list that is visible
 *                - in the interpreter
 *                - this is to facilitate/enable writing executable SCHEME
 *                - scripts
 *                - if invoked as a script the shell transforms:
 *                -    foo <args>
 *                - to
 *                -    scheme foo <args>
 *                - assume all args are to be processed by the compiled
 *                - command line processing in the "main" routine
 */

int SS_define_argv(SS_psides *si, int c, char **v, int go)
   {int i, n, rv, t, after;
    int type[MAXLINE];
    long *lp;
    double *dp;
    char *s, *script;
    void *p, *ptr[MAXLINE];
    object *argl;

    rv    = FALSE;
    after = FALSE;

    script = SS_exe_script(c, v);
    if (script != NULL)

/* set the exit in case the script exits */
       {switch (SETJMP(SC_gs.cpu))
	   {case ABORT :
		 SS_end_scheme(si, ABORT);
		 exit(1);
	    case ERR_FREE :
	         SS_end_scheme(si, ERR_FREE);
		 exit(0);};

/* process the command line arguments into a list */
	for (n = 0, i = 0; i < c; i++)
	    {s = v[i];
	     if (s == NULL)
	        break;

	     if (s == script)
                {after = TRUE;
		 continue;};

	     if (after == FALSE)
	        continue;

	     if (s[0] == '-')
	        {t = SC_STRING_I;
		 p = s;}
	     else if (SC_intstrp(s, 10))
	        {t   = SC_LONG_I;
		 lp  = CMAKE(long);
		 *lp = SC_stol(s);
		 p   = lp;}
	     else if (SC_fltstrp(s))
	        {t   = SC_DOUBLE_I;
		 dp  = CMAKE(double);
		 *dp = SC_stof(s);
		 p   = dp;}
	     else
	        {t = SC_STRING_I;
		 p = s;}

	     type[n] = t;
	     ptr[n]  = p;
	     n++;};

/* make the arg list available in interpreted code */
	argl = _SS_make_list(si, n, type, ptr);

	SS_define_constant(si, 1,
			   "argv", G_OBJECT_I, argl,
			   NULL);

/* load the full Scheme */
	SS_load_scm(si, "runt.scm");

/* load the script file - this might not return */
	if (go == TRUE)
	   SS_load_scm(si, script);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
