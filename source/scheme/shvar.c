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

/* _SS_EXA_VAR - examine and print the contents of the variable */

static object *_SS_exa_var(void *vr, int type)
   {char cv, *sv, **pv;
    object *ret;

    ret = SS_f;

    if (type == SC_CHAR_I)
       {cv = *(char *) vr;
	if (SS_interactive)
	   PRINT(stdout, "    %c", cv);
	ret = SS_mk_integer(cv);}

/* fixed point types (proper) */
    else if (SC_is_type_fix(type) == TRUE)
       {long long v;
	SC_convert_id(SC_LONG_LONG_I, &v, 0, type, vr, 0, 1, 1, FALSE);
	if (SS_interactive)
	   PRINT(stdout, "    %lld", v);
	ret = SS_mk_integer(v);}

/* floating point types (proper) */
    else if (SC_is_type_fp(type) == TRUE)
       {long double v;
	SC_convert_id(SC_LONG_DOUBLE_I, &v, 0, type, vr, 0, 1, 1, FALSE);
	if (SS_interactive)
	   PRINT(stdout, "    %lg", v);
	ret = SS_mk_float(v);}

/* complex floating point types (proper) */
    else if (SC_is_type_cx(type) == TRUE)
       {long double _Complex v;
	SC_convert_id(SC_LONG_DOUBLE_COMPLEX_I, &v, 0, type, vr, 0, 1, 1, FALSE);
	if (SS_interactive)
	   PRINT(stdout, "    %lg + %lg*I", creall(v), cimagl(v));
	ret = SS_mk_complex(v);}

    else if (type == SC_STRING_I)
       {sv = (char *) vr;
	if (SS_interactive)
	   PRINT(stdout, "    %s", sv);
	ret = SS_mk_string(sv);}

    else if (type == SC_POINTER_I)
       {pv = (char **) vr;
	if (*pv != NULL)
	   {if (SS_interactive)
	       PRINT(stdout, "    %s", *pv);
	    ret = SS_mk_string(*pv);}
	else
	   {if (SS_interactive)
	       PRINT(stdout, "    \"\"");
	    ret = SS_f;};}

    else
       {if (SS_interactive)
	   PRINT(stdout, "    Unknown type ...");};

    if (SS_interactive)
       PRINT(stdout, "\n");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_SET_VAR - set the value of VR to the value of VL */

static object *_SS_set_var(void *vr, object *vl, int type)
   {int ok;

    ok = _SS_object_to_numtype_id(type, vr, 0, vl);
    if (ok == FALSE)
       {char *sv, bf[MAXLINE];
	long lv;
	double dv;

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
	   {if (sv != NULL)
	       strcpy((char *) vr, sv);

	    else if (lv != -HUGE_INT)
	       sprintf((char *) vr, "%ld", lv);

	    else if (dv != -HUGE)
	       sprintf((char *) vr, "%g", dv);}

	else if (type == SC_POINTER_I)
	   {if (sv != NULL)
	       *((char **) vr) = SC_strsavef(sv, "char*:_SS_SET_VAR:sv");

	    else if (lv != -HUGE_INT)
	       {snprintf(bf, MAXLINE, "%ld", lv);
		*((char **) vr) = SC_strsavef(bf, "char*:_SS_SET_VAR:bf");}

	    else if (dv != -HUGE)
	       {snprintf(bf, MAXLINE, "%g", dv);
		*((char **) vr) = SC_strsavef(bf, "char*:_SS_SET_VAR:bf");};}

	else
	   SS_error("OBJECT HAS INCORRECT TYPE - _SS_SET_VAR", vl);};

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

object *SS_install_cf(char *name, char *doc, ...)
   {object *op, *vp;
    PFPHand handler;
    PFVoid fnc;
    C_procedure *cp;
    procedure *pp;

    SC_VA_START(doc);
    handler = SC_VA_ARG(PFPHand);
    fnc     = SC_VA_ARG(PFVoid);
    SC_VA_END;

    cp = _SS_mk_C_proc_va(handler, 1, fnc);
    pp = _SS_mk_scheme_proc(name, doc, SS_PR_PROC, cp);

    op = SS_mk_proc_object(pp);
    SS_UNCOLLECT(op);

    vp = SS_mk_variable(name, op);
    SS_UNCOLLECT(vp);

    SC_hasharr_install(SS_symtab, name, vp, SS_POBJECT_S, TRUE, TRUE);

    return(vp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INSTALL_CV - put a compiled C variable into the Scheme table
 *               - variables installed this way are used as follows:
 *               -
 *               -   (set! foo 3)  - changes value of foo to 3
 *               -   foo           - evaluates to value of foo
 */

object *SS_install_cv(char *name, void *pval, int ityp)
   {char *typ;
    object *var;

    typ = SC_type_name(ityp);
    var = SS_mk_variable(name, SS_null);
    SS_UNCOLLECT(var);

    if (ityp == SC_CHAR_I)
       {long long v;
	v = *(char *) pval;
	SS_def_var(var, SS_mk_integer(v), SS_Global_Env);}

    else if (ityp == SC_STRING_I)
       {char *v;
	v = (char *) pval;
	SS_def_var(var, SS_mk_string(v), SS_Global_Env);}

/* fixed point types (proper) */
    else if (SC_is_type_fix(ityp) == TRUE)
       {long long v;
	SC_convert_id(SC_LONG_LONG_I, &v, 0, ityp, pval, 0, 1, 1, FALSE);
	SS_def_var(var, SS_mk_integer(v), SS_Global_Env);}

/* floating point types (proper) */
    else if (SC_is_type_fp(ityp) == TRUE)
       {long double v;
	SC_convert_id(SC_LONG_DOUBLE_I, &v, 0, ityp, pval, 0, 1, 1, FALSE);
	SS_def_var(var, SS_mk_float(v), SS_Global_Env);}

/* complex floating point types (proper) */
    else if (SC_is_type_cx(ityp) == TRUE)
       {long double _Complex v;
	SC_convert_id(SC_LONG_DOUBLE_COMPLEX_I, &v, 0, ityp, pval, 0, 1, 1, FALSE);
	SS_def_var(var, SS_mk_complex(v), SS_Global_Env);}

    else if (ityp == SS_OBJECT_I)
       {SS_def_var(var, (object *) pval, SS_Global_Env);
	typ = SS_POBJECT_S;}

    else
       SS_error("BAD VARIABLE TYPE - SX_INSTALL_VARIABLE",
		SS_null);

/* GOTCHA: the type, typ, is wrong! var is an object * and typ should be
 *         replaced by SS_POBJECT_S if one wants to conform to the rules
 *         for such things
 */
    if (SC_hasharr_install(SS_symtab, name, var, typ, TRUE, TRUE) == NULL)
       SS_error("INSTALL FAILED - SS_INSTALL_CV", var);

    return(var);}

/*--------------------------------------------------------------------------*/

/*                           VARIABLE HANDLERS                              */

/*--------------------------------------------------------------------------*/

/* _SS_ACC_VAR - the handler for accessing variables */

static object *_SS_acc_var(C_procedure *cp, object *argl, int type)
   {object *vl, *ret;
    SC_address ad;

    ad.funcaddr = (PFInt) cp->proc[0];

    if (SS_nullobjp(argl))
       ret = _SS_exa_var(ad.memaddr, type);

    else
       {vl = SS_car(argl);
        ret = _SS_set_var(ad.memaddr, vl, type);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_DOUBLE - the handler for accessing double variables */

object *SS_acc_double(C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(cp, argl, SC_DOUBLE_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_INT - the handler for accessing int variables */

object *SS_acc_int(C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(cp, argl, SC_INT_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_LONG - the handler for accessing LONG variables */

object *SS_acc_long(C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(cp, argl, SC_LONG_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_CHAR - the handler for accessing char variables */

object *SS_acc_char(C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(cp, argl, SC_CHAR_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_STRING - the handler for accessing char * variables */

object *SS_acc_string(C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(cp, argl, SC_STRING_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_PTR - the handler for accessing pointer variables */

object *SS_acc_ptr(C_procedure *cp, object *argl)
   {object *ret;

    ret = _SS_acc_var(cp, argl, SC_POINTER_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
