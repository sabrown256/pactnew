/*
 * SHVAR.C - SCHEME interface to compiled variables
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "scheme_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_EXA_VAR - examine and print the contents of the variable VR */

static object *_SS_exa_var(SS_psides *si, void *vr, int type)
   {char cv, *sv, **pv;
    object *ret;

    ret = SS_f;

    if (si->interactive == TRUE)
       {char bf[MAXLINE];

	SC_ntos(bf, MAXLINE, type, vr, 0, 1);
	PRINT(stdout, "    %s", bf);};

/* character types (proper) */
    if (SC_is_type_char(type) == TRUE)
       {cv = *(char *) vr;
	ret = SS_mk_integer(si, cv);}

/* fixed point types (proper) */
    else if (SC_is_type_fix(type) == TRUE)
       {long long v;
	SC_convert_id(SC_LONG_LONG_I, &v, 0, 1, type, vr, 0, 1, 1, FALSE);
	ret = SS_mk_integer(si, v);}

/* floating point types (proper) */
    else if (SC_is_type_fp(type) == TRUE)
       {long double v;
	SC_convert_id(SC_LONG_DOUBLE_I, &v, 0, 1, type, vr, 0, 1, 1, FALSE);
	ret = SS_mk_float(si, v);}

/* complex floating point types (proper) */
    else if (SC_is_type_cx(type) == TRUE)
       {long double _Complex v;
	SC_convert_id(SC_LONG_DOUBLE_COMPLEX_I, &v, 0, 1,
		      type, vr, 0, 1, 1, FALSE);
	ret = SS_mk_complex(si, v);}

    else if (type == SC_STRING_I)
       {sv  = (char *) vr;
	ret = SS_mk_string(si, sv);}

    else if (type == SC_POINTER_I)
       {pv = (char **) vr;
	if (*pv != NULL)
	   ret = SS_mk_string(si, *pv);
	else
	   ret = SS_f;}

    else
       {if (si->interactive)
	   PRINT(stdout, "    Unknown type ...");};

    if (si->interactive)
       PRINT(stdout, "\n");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_SET_VAR - set the value of VR to the value of VL */

static object *_SS_set_var(SS_psides *si, void *vr, long nb,
			   object *vl, int type)
   {int ok;

    ok = _SS_object_to_numtype_id(type, vr, 0, vl);
    if (ok == FALSE)
       {long nc, lv;
	double dv;
	char *sv, bf[MAXLINE];

	lv = -HUGE_INT;
	dv = -HUGE;
	sv = NULL;

	if (SS_integerp(vl))
	   lv = SS_INTEGER_VALUE(vl);

        else if (SS_floatp(vl))
	   dv = SS_FLOAT_VALUE(vl);

	else
	   sv = SS_get_string(vl);

	if (type == SC_STRING_I)
	   {char *d;

	    nc = 0;
	    d  = (char *) vr;
	    if (sv != NULL)
	       {SC_strncpy(d, nb, sv, -1);
		nc = strlen(d);}

	    else if (lv != -HUGE_INT)
	       nc = snprintf(d, nb, "%ld", lv);

	    else if (dv != -HUGE)
	       nc = snprintf(d, nb, "%g", dv);

	    if (nc >= nb)
	       SS_error(si, "STRING OVERFLOW - _SS_SET_VAR", vl);}

	else if (type == SC_POINTER_I)
	   {char **d;

	    d = (char **) vr;
	    if (sv != NULL)
	       {*d = CSTRSAVE(sv);
		nc = strlen(*d);}

	    else if (lv != -HUGE_INT)
	       {nc = snprintf(bf, MAXLINE, "%ld", lv);
		*d = CSTRSAVE(bf);}

	    else if (dv != -HUGE)
	       {nc = snprintf(bf, MAXLINE, "%g", dv);
		*d = CSTRSAVE(bf);};}

	else
	   SS_error(si, "OBJECT HAS INCORRECT TYPE - _SS_SET_VAR", vl);};

    return(vl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INSTALL_CF - install a compiled code object by name
 *               - either a variable or a function may be installed
 *               - legal variable types must be accompanied by:
 *               -   char   :  SS_acc_char
 *               -   int    :  SS_acc_int
 *               -   long   :  SS_acc_long
 *               -   double :  SS_acc_double
 *               -   char * :  SS_acc_string
 *               -   void * :  SS_acc_ptr
 *               -
 *               - variables installed this way are used as follows:
 *               -
 *               -   (foo 3)  - changes value of foo to 3
 *               -   (foo)    - evaluates to value of foo
 */

object *SS_install_cf(SS_psides *si, char *name, char *doc, ...)
   {long nb;
    object *op, *vp;
    PFPHand handler;
    PFVoid fnc;
    SS_C_procedure *cp;
    SS_procedure *pp;

    nb = 0L;

    SC_VA_START(doc);
    handler = SC_VA_ARG(PFPHand);
    fnc     = SC_VA_ARG(PFVoid);
    if (handler == SS_acc_string)
       nb = SC_VA_ARG(long);
    SC_VA_END;

    cp = _SS_mk_C_proc_va(handler, 2, fnc, nb);
    pp = _SS_mk_scheme_proc(name, doc, SS_PR_PROC, cp);

    op = SS_mk_proc_object(si, pp);
    SS_UNCOLLECT(op);

    vp = SS_mk_variable(si, name, op);
    SS_UNCOLLECT(vp);

    SC_hasharr_install(si->symtab, name, vp, SS_POBJECT_S, 3, -1);

    return(vp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INSTALL_CV - put a compiled C variable into the Scheme table
 *               - variables installed this way are used as follows:
 *               -
 *               -   (set! foo 3)  - changes value of foo to 3
 *               -   foo           - evaluates to value of foo
 */

object *SS_install_cv(SS_psides *si, char *name, void *pval, int ityp)
   {char *typ;
    object *var;

    typ = SC_type_name(ityp);
    var = SS_mk_variable(si, name, SS_null);
    SS_UNCOLLECT(var);

/* character types (proper) */
    if (SC_is_type_char(ityp) == TRUE)
       {long long v;
	v = *(char *) pval;
	SS_def_var(si, var, SS_mk_integer(si, v), si->global_env);}

/* fixed point types (proper) */
    else if (SC_is_type_fix(ityp) == TRUE)
       {long long v;
	SC_convert_id(SC_LONG_LONG_I, &v, 0, 1, ityp, pval, 0, 1, 1, FALSE);
	SS_def_var(si, var, SS_mk_integer(si, v), si->global_env);}

/* floating point types (proper) */
    else if (SC_is_type_fp(ityp) == TRUE)
       {long double v;
	SC_convert_id(SC_LONG_DOUBLE_I, &v, 0, 1, ityp, pval, 0, 1, 1, FALSE);
	SS_def_var(si, var, SS_mk_float(si, v), si->global_env);}

/* complex floating point types (proper) */
    else if (SC_is_type_cx(ityp) == TRUE)
       {long double _Complex v;
	SC_convert_id(SC_LONG_DOUBLE_COMPLEX_I, &v, 0, 1,
		      ityp, pval, 0, 1, 1, FALSE);
	SS_def_var(si, var, SS_mk_complex(si, v), si->global_env);}

    else if (ityp == SC_STRING_I)
       {char *v;
	v = (char *) pval;
	SS_def_var(si, var, SS_mk_string(si, v), si->global_env);}

    else if (ityp == G_OBJECT_I)
       {SS_def_var(si, var, (object *) pval, si->global_env);
	typ = SS_POBJECT_S;}

    else
       SS_error(si, "BAD VARIABLE TYPE - SX_INSTALL_VARIABLE",
		  SS_null);

/* GOTCHA: the type, typ, is wrong! var is an object * and typ should be
 *         replaced by SS_POBJECT_S if one wants to conform to the rules
 *         for such things
 */
    if (SC_hasharr_install(si->symtab, name, var, typ, 3, -1) == NULL)
       SS_error(si, "INSTALL FAILED - SS_INSTALL_CV", var);

    return(var);}

/*--------------------------------------------------------------------------*/

/*                           VARIABLE HANDLERS                              */

/*--------------------------------------------------------------------------*/

/* _SS_ACC_VAR - the handler for accessing variables */

static object *_SS_acc_var(SS_psides *si, SS_C_procedure *cp,
			   object *argl, int type)
   {long nb;
    object *vl, *ret;
    SC_address ad;

    ad.funcaddr = (PFInt) cp->proc[0];
    if (cp->n > 1)
       nb = (long) cp->proc[1];
    else
       nb = 0L;

    if (SS_nullobjp(argl))
       ret = _SS_exa_var(si, ad.memaddr, type);

    else
       {vl = SS_car(si, argl);
        ret = _SS_set_var(si, ad.memaddr, nb, vl, type);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_DOUBLE - the handler for accessing double variables */

object *SS_acc_double(SS_psides *si, SS_C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(si, cp, argl, SC_DOUBLE_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_INT - the handler for accessing int variables */

object *SS_acc_int(SS_psides *si, SS_C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(si, cp, argl, SC_INT_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_LONG - the handler for accessing LONG variables */

object *SS_acc_long(SS_psides *si, SS_C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(si, cp, argl, SC_LONG_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_CHAR - the handler for accessing char variables */

object *SS_acc_char(SS_psides *si, SS_C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(si, cp, argl, SC_CHAR_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_STRING - the handler for accessing char * variables */

object *SS_acc_string(SS_psides *si, SS_C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(si, cp, argl, SC_STRING_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_PTR - the handler for accessing pointer variables */

object *SS_acc_ptr(SS_psides *si, SS_C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(si, cp, argl, SC_POINTER_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
