/*
 * SHPRM3.C - Scheme primitive routines
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

/*--------------------------------------------------------------------------*/

/*                        LIST PRIMITIVES                                   */

/*--------------------------------------------------------------------------*/

/* _SS_ENDCONS - C version of endcons */

object *_SS_endcons(SS_psides *si, object *list, object *obj)
   {object *op, *mlist, *o;

    mlist = list;
    if (SS_nullobjp(list))
       o = SS_mk_cons(si, obj, SS_null);

    else if (!SS_consp(list))
       o = SS_mk_cons(si, list, SS_mk_cons(si, obj, SS_null));

    else
       {op = SS_mk_cons(si, obj, SS_null);
        while (SS_consp(mlist = SS_cdr(si, mlist)));
        SS_setcdr(si, mlist, op);
	o = list;};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CONS - gets the car and cadr of the arg list
 *           - puts them into a new object, and
 *           - returns a pointer to it
 *           - this is the Scheme version of cons
 */

static object *_SSI_cons(SS_psides *si, object *argl)
   {object *x1, *x2, *o;

    x1 = SS_car(si, argl);
    x2 = SS_cadr(si, argl);
    o  = SS_mk_cons(si, x1, x2);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SETCAR - set-car! at C level
 *           - return value is the new car
 */

object *SS_setcar(SS_psides *si, object *pair, object *car)
   {object *oldcar;

    oldcar = SS_car(si, pair);
    SS_mark(car);
    SS_CAR_MACRO(pair) = car;
    SS_gc(si, oldcar);

    return(car);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SETCDR - set-cdr! at C level
 *           - return value is the new cdr
 */

object *SS_setcdr(SS_psides *si, object *pair, object *cdr)
   {object *oldcdr;

    oldcdr = SS_cdr(si, pair);
    SS_mark(cdr);
    SS_CDR_MACRO(pair) = cdr;
    SS_gc(si, oldcdr);

    return(cdr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SETCAR - set-car! at Scheme level */

static object *_SSI_setcar(SS_psides *si, object *argl)
   {object *o;

    o = SS_setcar(si, SS_car(si, argl), SS_cadr(si, argl));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_SETCDR - set-cdr! at Scheme level */

static object *_SSI_setcdr(SS_psides *si, object *argl)
   {object *o;

    o = SS_setcdr(si, SS_car(si, argl), SS_cadr(si, argl));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CAR - return a pointer to the car of the object given */

object *SS_car(SS_psides *si, object *obj)
   {object *o;

    if (!SS_consp(obj))
       SS_error(si, "CAN'T TAKE CAR OF ATOM - CAR", obj);

    o = SS_CAR_MACRO(obj);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CDR - return a pointer to the cdr of the object given */

object *SS_cdr(SS_psides *si, object *obj)
   {object *o;

    if (!SS_consp(obj))
       SS_error(si, "CAN'T TAKE CDR OF ATOM - CDR", obj);

    o = SS_CDR_MACRO(obj);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CAAR - return a pointer to the caar of the object given */

object *SS_caar(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_car(si, obj));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CADR - return a pointer to the cadr of the object given */

object *SS_cadr(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_cdr(si, obj));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CDAR - return a pointer to the cdar of the object given */

object *SS_cdar(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_car(si, obj));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CDDR - return a pointer to the cddr of the object given */

object *SS_cddr(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_cdr(si, obj));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CAAAR - return a pointer to the caaar of the object given */

object *SS_caaar(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_car(si, SS_car(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CAADR - return a pointer to the caadr of the object given */

object *SS_caadr(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_car(si, SS_cdr(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CADAR - return a pointer to the cadar of the object given */

object *SS_cadar(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_cdr(si, SS_car(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CDAAR - return a pointer to the cdaar of the object given */

object *SS_cdaar(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_car(si, SS_car(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CADDR - return a pointer to the caddr of the object given */

object *SS_caddr(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_cdr(si, SS_cdr(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CDADR - return a pointer to the cdadr of the object given */

object *SS_cdadr(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_car(si, SS_cdr(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CDDAR - return a pointer to the cddar of the object given */

object *SS_cddar(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_cdr(si, SS_car(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CDDDR - return a pointer to the cdddr of the object given */

object *SS_cdddr(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_cdr(si, SS_cdr(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LIST_TAIL - return the tail of the list omitting the first n elements */

object *SS_list_tail(SS_psides *si, object *lst, int n)
   {int i, nl;
    object *h, *nlst;

    nlst = SS_null;

    if (!SS_nullobjp(lst))
       {nl = SS_length(si, lst);

	if (n < 0)
	   {n = nl + n;
	    if ((0 <= n) && (n <= nl))
	       {for (i = 0; i < n; i++)
		    {h    = SS_car(si, lst);
		     lst  = SS_cdr(si, lst);
		     nlst = SS_mk_cons(si, h, nlst);};
		nlst = SS_reverse(si, nlst);};}

	else if ((0 <= n) && (n < SS_length(si, lst)))
           {for (i = 0; i < n; i++, lst = SS_cdr(si, lst));
	    nlst = lst;};};

    return(nlst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LST_TAIL - return the tail of the list omitting the first n elements */

static object *_SSI_lst_tail(SS_psides *si, object *argl)
   {int n;
    object *lst, *o;

    SS_args(si, argl,
	    G_OBJECT_I, &lst,
	    G_INT_I, &n,
            0);

    o = SS_list_tail(si, lst, n);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LST_REF - return the nth element of the list */

static object *_SSI_lst_ref(SS_psides *si, object *argl)
   {int i, n, nl;
    object *lst, *o;

    SS_args(si, argl,
	    G_OBJECT_I, &lst,
	    G_INT_I, &n,
            0);

    o = SS_null;

    if (!SS_nullobjp(lst))
       {nl = SS_length(si, lst);

	if (n < 0)
	   {n = nl + n;
	    if ((0 <= n) && (n <= nl))
	       {for (i = 0; i <= n; i++, lst  = SS_cdr(si, lst))
                    o = SS_car(si, lst);};}

	else if ((0 <= n) && (n < SS_length(si, lst)))
           {for (i = 0; i < n; i++, lst = SS_cdr(si, lst));
	    o = SS_car(si, lst);};};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LAST - return a pointer to the last element of a list */

static object *_SSI_last(SS_psides *si, object *obj)
   {object *t, *lst, *o;

    t = obj;
    for (lst = obj; SS_consp(lst); lst = SS_cdr(si, lst))
        t = lst;

    if (!SS_consp(t))
       o = t;
    else if (SS_nullobjp(lst = SS_cdr(si, t)))
       o = SS_car(si, t);
    else
       o = lst;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_REVERSE - reverse the list in place and return it */

object *SS_reverse(SS_psides *si, object *obj)
   {object *ths, *nxt, *prv;

    ths = SS_null;

    if (SS_nullobjp(obj))
       ths = obj;

    else if (!SS_consp(obj))
       SS_error(si, "BAD LIST TO REVERSE", obj);

    else
       {for (ths = SS_null, nxt = obj; SS_consp(nxt); )
	    {prv = ths;
	     ths = nxt;
	     nxt = SS_cdr(si, nxt);
	     SS_CDR_MACRO(ths) = prv;
	     SS_mark(prv);
	     SC_mark(nxt, -1);};};

    return(ths);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_APPEND - append two lists together and return a pointer
 *           - to the first one
 *           - C version of append
 */

object *SS_append(SS_psides *si, object *list1, object *list2)
   {object *frst, *lst, *nxt, *cr;

    lst = SS_null;

    if (!SS_consp(list2) && !SS_nullobjp(list2))
       SS_error(si, "BAD SECOND LIST - SS_APPEND", list2);

    if (SS_nullobjp(list1))
       return(list2);

    else if (SS_consp(list1))
       {frst = SS_null;
	cr   = SS_null;
	SS_assign(si, cr, list1);
        while (SS_consp(cr))
           {nxt = SS_car(si, cr);
            SS_end_cons(si, frst, lst, nxt);
            SS_assign(si, cr, SS_cdr(si, cr));};

        SS_setcdr(si, lst, list2);

        return(frst);}

    else
       SS_error(si, "BAD FIRST LIST - SS_APPEND", list1);

    return(SS_null);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_APPEND - append two lists together and return a pointer
 *             - to the first one
 *             - Scheme version of append
 */

static object *_SSI_append(SS_psides *si, object *argl)
   {int n;
    object *o, *r, *rv;

    n = SS_length(si, argl);
    if (n < 1)
       rv = argl;

    else
       {rv = SS_car(si, argl);
	for (r = SS_cdr(si, argl); r != SS_null; r = SS_cdr(si, r))
	    {o  = SS_car(si, r);
	     rv = SS_append(si, rv, o);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_LENGTH - length primitive in Scheme */

static object *_SSI_length(SS_psides *si, object *obj)
   {object *o;

    obj = SS_car(si, obj);
    if (!SS_consp(obj))
       SS_error(si, "OBJECT NOT A LIST - LENGTH", obj);

    o = SS_mk_integer(si, SS_length(si, obj));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LENGTH - C usable version of length function in Scheme */

int SS_length(SS_psides *si, object *obj)
   {int i;

    for (i = 0; SS_consp(obj); i++)
        obj = SS_cdr(si, obj);

    return(i);}

/*--------------------------------------------------------------------------*/

/*                            PREDICATES                                    */

/*--------------------------------------------------------------------------*/

/* SS_numbp - number? predicate in C */

int SS_numbp(object *obj)
   {int rv;

    rv = (SS_integerp(obj) || SS_floatp(obj));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_NUMBERP - number? predicate in Scheme */

static object *_SSI_numberp(SS_psides *si, object *obj)
   {object *o;

    o = (SS_integerp(obj) || SS_floatp(obj)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_INTP - R4 compliant integer? function */

static object *_SSI_intp(SS_psides *si, object *obj)
   {double r;
    object *rv;

    rv = SS_f;
    if (SS_numbp(obj))
       {SS_args(si, obj,
		G_DOUBLE_I, &r,
		0);

	rv = (floor(r) == r) ? SS_t : SS_f;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_REALP - R4 compliant real? function */

static object *_SSI_realp(SS_psides *si, object *obj)
   {object *rv;

    rv = SS_numbp(obj) ? SS_t : SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_STRINGP - function version of SS_stringp macro */

static object *_SSI_stringp(SS_psides *si, object *obj)
   {object *o;

    o = SS_stringp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_VARP - function version of SS_variablep macro */

static object *_SSI_varp(SS_psides *si, object *obj)
   {object *o;

    o = SS_variablep(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_BOOLP - function version of SS_booleanp macro */

static object *_SSI_boolp(SS_psides *si, object *obj)
   {object *o;

    o = SS_booleanp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PAIRP - function version of SS_consp macro */

static object *_SSI_pairp(SS_psides *si, object *obj)
   {object *o;

    o = SS_consp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_PROCP - function version of SS_procedurep macro */

static object *_SSI_procp(SS_psides *si, object *obj)
   {object *o;

    o = SS_procedurep(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_FILEP - worker routine for file? and ascii-file? predicates */

static int _SS_filep(SS_psides *si, object *argl, const char *dtype)
   {int ret;
    char *name, *mode, *type, *scope;
    char **list;
    SC_udl *pu;

    ret   = FALSE;
    name  = NULL;
    mode  = NULL;
    type  = (char *) dtype;
    scope = NULL;

    SS_args(si, argl,
	    G_STRING_I, &name,
	    G_STRING_I, &mode,
	    G_STRING_I, &type,
	    G_STRING_I, &scope,
	    0);

    if (mode != NULL)
       if (strcmp(mode, "nil") == 0)
	  CFREE(mode);

    if (type != NULL)
       if (strcmp(type, "nil") == 0)
	  CFREE(type);

    if (scope == NULL)
       scope = CSTRSAVE("local");

    if (strcmp(scope, "local") == 0)
       list = NULL;
    else
       list = SC_get_search_path();

/* check for containers */
    pu = _SC_parse_udl(name);
    if (pu != NULL)
       {ret = (_SC_search_file(list, pu->path, mode, type) != NULL);

	_SC_rel_udl(pu);};

    CFREE(list);
    CFREE(name);
    CFREE(mode);
    CFREE(scope);
    if (type != dtype)
       CFREE(type);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_FILEP - file? predicate in Scheme */

static object *_SSI_filep(SS_psides *si, object *argl)
   {object *o;

    o = _SS_filep(si, argl, NULL) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_TEXT_FILEP - ascii-file? predicate in Scheme */

static object *_SSI_text_filep(SS_psides *si, object *argl)
   {object *o;

    o = _SS_filep(si, argl, "ascii") ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_EOFP - function version of SS_eofobjp macro */

static object *_SSI_eofp(SS_psides *si, object *obj)
   {object *o;

    o = SS_eofobjp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_NULLP - null? for Scheme */

static object *_SSI_nullp(SS_psides *si, object *obj)
   {object *o;

    o = SS_nullobjp(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_EQV - eqv? at C level */

static int _SS_eqv(SS_psides *si, object *o1, object *o2)
   {int ityp, rv;

    ityp = SC_arrtype(o1, -1);
    if (ityp != SC_arrtype(o2, -1))
       rv = FALSE;

    else if (SC_is_type_fix(ityp) == TRUE)
       rv = (SS_INTEGER_VALUE(o1) == SS_INTEGER_VALUE(o2));

    else if (SC_is_type_fp(ityp) == TRUE)
       rv = (SS_FLOAT_VALUE(o1) == SS_FLOAT_VALUE(o2));

    else if (SC_is_type_cx(ityp) == TRUE)
       rv = PM_cequal(SS_COMPLEX_VALUE(o1), SS_COMPLEX_VALUE(o2));

    else if (ityp == G_QUATERNION_I)
       rv = PM_qequal(SS_QUATERNION_VALUE(o1), SS_QUATERNION_VALUE(o2));

    else if (ityp == G_STRING_I)
       rv = (strcmp(SS_STRING_TEXT(o1), SS_STRING_TEXT(o2)) == 0);

    else if (ityp == SS_CHARACTER_I)
       rv = (SS_CHARACTER_VALUE(o1) == SS_CHARACTER_VALUE(o2));

    else if (ityp == G_SS_VARIABLE_I)
       rv = (strcmp(SS_VARIABLE_NAME(o1), SS_VARIABLE_NAME(o2)) == 0);

    else if (ityp == G_BOOL_I)
       rv = (SS_BOOLEAN_VALUE(o1) == SS_BOOLEAN_VALUE(o2));

    else if (ityp == G_SS_PROCEDURE_I)
       rv = (SS_PROCEDURE_PROC(o1) == SS_PROCEDURE_PROC(o2));

    else
       rv = (o1->val == o2->val);

   return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_EQUAL - equal? at C level */

static int _SS_equal(SS_psides *si, object *o1, object *o2)
   {int rv;

    rv = FALSE;

    if (SC_arrtype(o1, -1) == SC_arrtype(o2, -1))
       {if (SS_consp(o1))
	   {if (SS_length(si, o1) == SS_length(si, o2))
	       {while (TRUE)
		   {if (!SS_consp(o1))
		       {rv = _SS_eqv(si, o1, o2);
			break;}

		    else if (!_SS_equal(si, SS_car(si, o1), SS_car(si, o2)))
		      break;

		    else
		       {o1 = SS_cdr(si, o1);
			o2 = SS_cdr(si, o2);};};};}

        else if (SS_procedurep(o1))
	   {if (((SS_PROCEDURE_TYPE(o1) == SS_PROC) &&
		 (SS_PROCEDURE_TYPE(o2) == SS_PROC)) ||
		((SS_PROCEDURE_TYPE(o1) == SS_MACRO) &&
		 (SS_PROCEDURE_TYPE(o2) == SS_MACRO)))
	       rv = _SS_equal(si, SS_proc_body(si, o1),
			      SS_proc_body(si, o2));}

	else
	   rv = _SS_eqv(si, o1, o2);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_EQ - eq? in at C level */

static int _SS_eq(SS_psides *si, object *o1, object *o2)
   {int rv;

    rv = (o1->val == o2->val);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_EQ - eq? in Scheme */

static object *_SSI_eq(SS_psides *si, object *obj)
   {object *o;

    o = _SS_eq(si, SS_car(si, obj), SS_cadr(si, obj)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_EQV - eqv? in Scheme */

static object *_SSI_eqv(SS_psides *si, object *obj)
   {object *o;

    o = _SS_eqv(si, SS_car(si, obj), SS_cadr(si, obj)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_EQUAL - equal in Scheme */

static object *_SSI_equal(SS_psides *si, object *obj)
   {object *o;

    o = _SS_equal(si, SS_car(si, obj), SS_cadr(si, obj)) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MEMP - C level handler for memq, memv, and member */

static object *_SS_memp(SS_psides *si,
			int (*pred)(SS_psides *si, object *, object *),
			object *obj, object *lst)
   {object *o;

    while (TRUE)
       {if (!SS_consp(lst))
           break;

        o = SS_car(si, lst);
        if ((*pred)(si, obj, o))
           return(lst);

        lst = SS_cdr(si, lst);};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEMQ - Scheme version of memq */

static object *_SSI_memq(SS_psides *si, object *argl)
   {object *obj, *lst, *o;

    obj = SS_car(si, argl);
    lst = SS_cadr(si, argl);

    o = _SS_memp(si, _SS_eq, obj, lst);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEMV - Scheme version of memv */

static object *_SSI_memv(SS_psides *si, object *argl)
   {object *obj, *lst, *o;

    obj = SS_car(si, argl);
    lst = SS_cadr(si, argl);

    o = _SS_memp(si, _SS_eqv, obj, lst);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_MEMBER - Scheme version of member */

static object *_SSI_member(SS_psides *si, object *argl)
   {object *obj, *lst, *o;

    obj = SS_car(si, argl);
    lst = SS_cadr(si, argl);

    o = _SS_memp(si, _SS_equal, obj, lst);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_ASSP - C level handler for assq, assv, and assoc */

static object *_SS_assp(SS_psides *si,
			int (*pred)(SS_psides *si, object *, object *),
			object *obj, object *lst)
   {object *o, *rv;

    rv = SS_f;

    if (SS_consp(lst) == TRUE)
       {for ( ; SS_consp(lst); lst = SS_cdr(si, lst))
	    {o = SS_car(si, lst);
	     if ((*pred)(si, obj, SS_car(si, o)))
	        {rv = o;
		 break;};};}

    else if (_SS.assp_ext != NULL)
       rv = _SS.assp_ext(si, pred, obj, lst);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_ASSQ - Scheme version of assq */

static object *_SSI_assq(SS_psides *si, object *argl)
   {object *obj, *lst, *o;

    obj = SS_car(si, argl);
    lst = SS_cadr(si, argl);

    o = _SS_assp(si, _SS_eq, obj, lst);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_ASSV - Scheme version of assv */

static object *_SSI_assv(SS_psides *si, object *argl)
   {object *obj, *lst, *o;

    obj = SS_car(si, argl);
    lst = SS_cadr(si, argl);

    o = _SS_assp(si, _SS_eqv, obj, lst);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_ASSOC - Scheme version of assoc */

static object *_SSI_assoc(SS_psides *si, object *argl)
   {object *obj, *lst, *o;

    obj = SS_car(si, argl);
    lst = SS_cadr(si, argl);

    o = _SS_assp(si, _SS_equal, obj, lst);

    return(o);}

/*--------------------------------------------------------------------------*/

/*                           INTERPRETER BINDINGS                           */

/*--------------------------------------------------------------------------*/

/* _SSI_CAR - return a pointer to the car of the object given */

static object *_SSI_car(SS_psides *si, object *obj)
   {object *o;

    if (!SS_consp(obj))
       SS_error(si, "CAN'T TAKE CAR OF ATOM - CAR", obj);

    o = SS_CAR_MACRO(obj);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CDR - return a pointer to the cdr of the object given */

static object *_SSI_cdr(SS_psides *si, object *obj)
   {object *o;

    if (!SS_consp(obj))
       SS_error(si, "CAN'T TAKE CDR OF ATOM - CDR", obj);

    o = SS_CDR_MACRO(obj);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CAAR - return a pointer to the caar of the object given */

static object *_SSI_caar(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_car(si, obj));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CADR - return a pointer to the cadr of the object given */

static object *_SSI_cadr(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_cdr(si, obj));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CDAR - return a pointer to the cdar of the object given */

static object *_SSI_cdar(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_car(si, obj));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CDDR - return a pointer to the cddr of the object given */

static object *_SSI_cddr(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_cdr(si, obj));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CAAAR - return a pointer to the caaar of the object given */

static object *_SSI_caaar(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_car(si, SS_car(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CAADR - return a pointer to the caadr of the object given */

static object *_SSI_caadr(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_car(si, SS_cdr(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CADAR - return a pointer to the cadar of the object given */

static object *_SSI_cadar(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_cdr(si, SS_car(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CDAAR - return a pointer to the cdaar of the object given */

static object *_SSI_cdaar(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_car(si, SS_car(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CADDR - return a pointer to the caddr of the object given */

static object *_SSI_caddr(SS_psides *si, object *obj)
   {object *o;

    o = SS_car(si, SS_cdr(si, SS_cdr(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CDADR - return a pointer to the cdadr of the object given */

static object *_SSI_cdadr(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_car(si, SS_cdr(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CDDAR - return a pointer to the cddar of the object given */

static object *_SSI_cddar(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_cdr(si, SS_car(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_CDDDR - return a pointer to the cdddr of the object given */

static object *_SSI_cdddr(SS_psides *si, object *obj)
   {object *o;

    o = SS_cdr(si, SS_cdr(si, SS_cdr(si, obj)));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_REVERSE - reverse the list in place and return it */

static object *_SSI_reverse(SS_psides *si, object *obj)
   {object *o;

    o = SS_reverse(si, obj);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_PRM3 - install the Scheme primitives */

void _SS_inst_prm3(SS_psides *si)
   {

    SS_install(si, "append",
               "Return a new list made from a copy of the first and the second arguments",
               SS_nargs, 
               _SSI_append, SS_PR_PROC);

    SS_install(si, "ascii-file?",
               "Procedure: Returns #t if the object is an ascii file, and #f otherwise",
               SS_nargs,
               _SSI_text_filep, SS_PR_PROC);

    SS_install(si, "assoc",
               "Procedure: Returns the first list whose car is equal? to the first arg",
               SS_nargs,
               _SSI_assoc, SS_PR_PROC);

    SS_install(si, "assq",
               "Procedure: Returns the first list whose car is eq? to the first arg",
               SS_nargs,
               _SSI_assq, SS_PR_PROC);

    SS_install(si, "assv",
               "Procedure: Returns the first list whose car is eqv? to the first arg",
               SS_nargs,
               _SSI_assv, SS_PR_PROC);

    SS_install(si, "boolean?",
               "Returns #t if the object is a boolean, and #f otherwise",
               SS_sargs, 
               _SSI_boolp, SS_PR_PROC);

    SS_install(si, "car",
               "Return the car of the argument",
               SS_sargs, 
               _SSI_car, SS_PR_PROC);

    SS_install(si, "caaar",
               "Return the caaar of the argument",
               SS_sargs, 
               _SSI_caaar, SS_PR_PROC);

    SS_install(si, "caadr",
               "Return the caadr of the argument",
               SS_sargs, 
               _SSI_caadr, SS_PR_PROC);

    SS_install(si, "caar",
               "Return the caar of the argument",
               SS_sargs, 
               _SSI_caar, SS_PR_PROC);

    SS_install(si, "cadar",
               "Return the cadar of the argument",
               SS_sargs, 
               _SSI_cadar, SS_PR_PROC);

    SS_install(si, "caddr",
               "Return the caddr of the argument",
               SS_sargs, 
               _SSI_caddr, SS_PR_PROC);

    SS_install(si, "cadr",
               "Return the cadr of the argument",
               SS_sargs, 
               _SSI_cadr, SS_PR_PROC);

    SS_install(si, "cdar",
               "Return the cdar of the argument",
               SS_sargs, 
               _SSI_cdar, SS_PR_PROC);

    SS_install(si, "cdaar",
               "Return the cdaar of the argument",
               SS_sargs, 
               _SSI_cdaar, SS_PR_PROC);

    SS_install(si, "cdadr",
               "Return the cdadr of the argument",
               SS_sargs, 
               _SSI_cdadr, SS_PR_PROC);

    SS_install(si, "cddar",
               "Return the cddar of the argument",
               SS_sargs, 
               _SSI_cddar, SS_PR_PROC);

    SS_install(si, "cdddr",
               "Return the cdddr of the argument",
               SS_sargs, 
               _SSI_cdddr, SS_PR_PROC);

    SS_install(si, "cddr",
               "Return the cddr of the argument",
               SS_sargs, 
               _SSI_cddr, SS_PR_PROC);

    SS_install(si, "cdr",
               "Return the cdr of the argument",
               SS_sargs, 
               _SSI_cdr, SS_PR_PROC);

    SS_install(si, "cons",
               "Return a new cons whose car and cdr are the arguments",
               SS_nargs, 
               _SSI_cons, SS_PR_PROC);

    SS_install(si, "eof-object?",
               "Returns #t if the object is the EOF object, and #f otherwise",
               SS_sargs, 
               _SSI_eofp, SS_PR_PROC);

    SS_install(si, "eq?",
               "Procedure: Returns #t iff the two objects are identical",
               SS_nargs,
               _SSI_eq, SS_PR_PROC);

    SS_install(si, "equal?",
               "Procedure: Returns the result of recursively applying eqv? to thearguments",
               SS_nargs,
               _SSI_equal, SS_PR_PROC);

    SS_install(si, "eqv?",
               "Procedure: Returns #t iff the two objects are equivalent",
               SS_nargs,
               _SSI_eqv, SS_PR_PROC);

    SS_install(si, "file?",
               "Procedure: Returns #t if the object is a file, and #f otherwise",
               SS_nargs,
               _SSI_filep, SS_PR_PROC);

    SS_install(si, "integer?",
               "Returns #t if the object is an integer number, and #f otherwise",
               SS_sargs, 
               _SSI_intp, SS_PR_PROC);

    SS_install(si, "last",
               "Return the last element of a list or return any other object",
               SS_sargs, 
               _SSI_last, SS_PR_PROC);

    SS_install(si, "length",
               "Return the number of elements in the given list",
               SS_nargs, 
               _SSI_length, SS_PR_PROC);

    SS_install(si, "list-ref",
               "Return the nth element of the list (0 based)",
               SS_nargs, 
               _SSI_lst_ref, SS_PR_PROC);

    SS_install(si, "list-tail",
               "Return the tail of the list omitting the first n elements",
               SS_nargs, 
               _SSI_lst_tail, SS_PR_PROC);

    SS_install(si, "member",
               "Procedure: Returns the first sublist of the second arg whose car is equal? to the first arg",
               SS_nargs,
               _SSI_member, SS_PR_PROC);

    SS_install(si, "memq",
               "Procedure: Returns the first sublist of the second arg whose car is eq? to the first arg",
               SS_nargs,
               _SSI_memq, SS_PR_PROC);

    SS_install(si, "memv",
               "Procedure: Returns the first sublist of the second arg whose car is eqv? to the first arg",
               SS_nargs,
               _SSI_memv, SS_PR_PROC);

    SS_install(si, "null?",
               "Procedure: Returns #t iff the object is the empty list, ()",
               SS_sargs,
               _SSI_nullp, SS_PR_PROC);

    SS_install(si, "number?",
               "Procedure: Returns #t if the object is a number, and #f otherwise",
               SS_sargs,
               _SSI_numberp, SS_PR_PROC);

    SS_install(si, "pair?",
               "Returns #t if the object is a cons or list, and #f otherwise",
               SS_sargs, 
               _SSI_pairp, SS_PR_PROC);

    SS_install(si, "procedure?",
               "Returns #t if the object is a procedure object, and #f otherwise",
               SS_sargs, 
               _SSI_procp, SS_PR_PROC);

    SS_install(si, "real?",
               "Returns #t if the object is a real number, and #f otherwise",
               SS_sargs, 
               _SSI_realp, SS_PR_PROC);

    SS_install(si, "reverse",
               "Destructively reverse the list and return it",
               SS_sargs, 
               _SSI_reverse, SS_PR_PROC);

    SS_install(si, "set-car!",
               "Procedure: Replace the car of the first argument with the second and return the new car",
               SS_nargs,
               _SSI_setcar, SS_PR_PROC);

    SS_install(si, "set-cdr!",
               "Procedure: Replace the cdr of the first argument with the second and return the new cdr",
               SS_nargs,
               _SSI_setcdr, SS_PR_PROC);

    SS_install(si, "string?",
               "Returns #t if the object is a string, and #f otherwise",
               SS_sargs, 
               _SSI_stringp, SS_PR_PROC);

    SS_install(si, "symbol?",
               "Returns #t if the object is a variable, and #f otherwise",
               SS_sargs, 
               _SSI_varp, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
