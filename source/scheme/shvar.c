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
    int iv;
    long lv;
    double rv;
    object *ret;

    ret = SS_f;
    if (type == SC_CHAR_I)
       {cv = *(char *) vr;
	if (SS_interactive)
	   PRINT(stdout, "    %c", cv);
	ret = SS_mk_integer((BIGINT) cv);}

    else if (type == SC_INT_I)
       {iv = *(int *) vr;
	if (SS_interactive)
	   PRINT(stdout, "    %d", iv);
	ret = SS_mk_integer((BIGINT) iv);}

    else if (type == SC_LONG_I)
       {lv = *(long *) vr;
	if (SS_interactive)
	   PRINT(stdout, "    %ld", lv);
	ret = SS_mk_integer((BIGINT) lv);}

    else if (type == SC_DOUBLE_I)
       {rv = *(double *) vr;
	if (SS_interactive)
	   PRINT(stdout, "    %g", rv);
	ret = SS_mk_float((double) rv);}

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

/* NOTE: when this routine was in ULTRA this line was:
 *    UL_pause(FALSE);
 */
    if (SS_interactive)
       PRINT(stdout, "\n");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_SET_VAR - set the value of the given variable */

static object *_SS_set_var(void *vr, object *vl, int type)
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

    if (type == SC_CHAR_I)
       {if (lv != -HUGE_INT)
	   *((char *) vr) = (char) lv;

        else if (dv != -HUGE)
	   *((char *) vr) = (char) dv;

	else
	   SS_error("OBJECT NOT CHAR - SS_ACC_VAR", vl);}

    else if (type == SC_INT_I)
       {if (lv != -HUGE_INT)
	   *((int *) vr) = (int) lv;

        else if (dv != -HUGE)
	   *((int *) vr) = (int) dv;

	else
	   SS_error("OBJECT NOT INTEGER - SS_ACC_VAR", vl);}

    else if (type == SC_LONG_I)
       {if (lv != -HUGE_INT)
	   *((long *) vr) = lv;

        else if (dv != -HUGE)
	   *((long *) vr) = (long) dv;

	else
	   SS_error("OBJECT NOT LONG INT - SS_ACC_VAR", vl);}

    else if (type == SC_DOUBLE_I)
       {if (dv != -HUGE)
	   *((double *) vr) = dv;

        else if (lv != -HUGE_INT)
	   *((double *) vr) = (double) lv;

	else
	   SS_error("OBJECT NOT REAL NUMBER - SS_ACC_VAR", vl);}

    else if (type == SC_STRING_I)
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
       SS_error("OBJECT HAS INCORRECT TYPE - SS_ACC_VAR", vl);

    return(vl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INSTALL_CF - install a compiled code object by name
 *               - either a variable or a function may be installed
 *               - legal variable types must be accompanied by:
 *               -   char   :  SS_acc_char
 *               -   int    :  SS_acc_int
 *               -   long   :  SS_acc_long
 *               -   double :  SS_acc_REAL
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
    fnc = SC_VA_ARG(PFVoid);
    SC_VA_END;

    cp = FMAKE(C_procedure, "SS_INSTALL_CF:cp");
    cp->proc    = fnc;
    cp->handler = handler;

    pp = FMAKE(procedure, "SS_INSTALL_CF:pp");
    pp->type  = SS_PR_PROC;
    pp->trace = FALSE;
    pp->proc  = (object *) cp;
    pp->doc   = doc;
    pp->name  = SC_strsavef(name, "char*:SS_INSTALL_CF:name");

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

object *SS_install_cv(char *name, void *pval, int type)
   {object *var;
    char *sval, *typ = NULL;
    long lval;
    double dval;

    var = SS_mk_variable(name, SS_null);
    SS_UNCOLLECT(var);

    if (type == SC_CHAR_I)
       {lval = *(char *) pval;
	SS_def_var(var, SS_mk_integer((BIGINT) lval), SS_Global_Env);
	typ = SC_CHAR_S;}

    else if (type == SC_STRING_I)
       {sval = (char *) pval;
	SS_def_var(var, SS_mk_string(sval), SS_Global_Env);
	typ = SC_STRING_S;}

/* fixed point types */
    else if (type == SC_SHORT_I)
       {lval = *(short *) pval;
	SS_def_var(var, SS_mk_integer((BIGINT) lval), SS_Global_Env);
	typ = SC_SHORT_S;}

    else if (type == SC_INT_I)
       {lval = *(int *) pval;
	SS_def_var(var, SS_mk_integer((BIGINT) lval), SS_Global_Env);
	typ = SC_INT_S;}

    else if (type == SC_LONG_I)
       {lval = *(long *) pval;
	SS_def_var(var, SS_mk_integer((BIGINT) lval), SS_Global_Env);
	typ = SC_LONG_S;}

/* floating point types */
    else if (type == SC_FLOAT_I)
       {dval = *(float *) pval;
	SS_def_var(var, SS_mk_float(dval), SS_Global_Env);
	typ = SC_FLOAT_S;}

    else if (type == SC_DOUBLE_I)
       {dval = *(double *) pval;
	SS_def_var(var, SS_mk_float(dval), SS_Global_Env);
	typ = SC_DOUBLE_S;}

    else if (type == SS_OBJECT_I)
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

/* SS_ACC_REAL - the handler for accessing REAL variables */

object *SS_acc_REAL(void *vr, object *argl)
   {object *ret;

    ret = SS_acc_var(vr, argl, SC_DOUBLE_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_INT - the handler for accessing int variables */

object *SS_acc_int(void *vr, object *argl)
   {object *ret;

    ret = SS_acc_var(vr, argl, SC_INT_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_LONG - the handler for accessing LONG variables */

object *SS_acc_long(void *vr, object *argl)
   {object *ret;

    ret = SS_acc_var(vr, argl, SC_LONG_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_CHAR - the handler for accessing char variables */

object *SS_acc_char(void *vr, object *argl)
   {object *ret;

    ret = SS_acc_var(vr, argl, SC_CHAR_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_STRING - the handler for accessing char * variables */

object *SS_acc_string(void *vr, object *argl)
   {object *ret;

    ret = SS_acc_var(vr, argl, SC_STRING_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_PTR - the handler for accessing pointer variables */

object *SS_acc_ptr(void *vr, object *argl)
   {object *ret;

    ret = SS_acc_var(vr, argl, SC_POINTER_I);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ACC_VAR - the handler for accessing variables */

object *SS_acc_var(void *vr, object *argl, int type)
   {object *vl, *ret;

    if (SS_nullobjp(argl))
       ret = _SS_exa_var(vr, type);

    else
       {vl = SS_car(argl);
        ret = _SS_set_var(vr, vl, type);};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
