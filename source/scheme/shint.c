/*
 * SHINT.C - C interface routines for SCHEME
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

PFExtractArg
 SS_arg_hook = NULL;

PFCallArg
 SS_call_arg_hook = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LOAD_BF - load the temporary string buffer */

static char *_SS_load_bf(char *s)
   {int nc;

    nc = strlen(s);
    if (nc >= _SS.ncx)
       {_SS.ncx = nc + 1;
	if (_SS.ibf != NULL)
	   {SFREE(_SS.ibf);};

	_SS.ibf = FMAKE_N(char, _SS.ncx, "chr*:_SS_LOAD_BF:_SS.ibf");};

    SC_strncpy(_SS.ibf, _SS.ncx, s, strlen(s));

    return(_SS.ibf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_FIX_ARG - get a C level fix point data item from
 *             - a single Scheme object
 */

static void _SS_fix_arg(object *obj, void *v, int type)
   {long long l;
    long long *llp;
    long *lp;
    int *ip;
    short *sp;
    char *cp;

    l = 0;

    if (SS_integerp(obj))
       l = SS_INTEGER_VALUE(obj);

    else if (SS_floatp(obj))
       l = PM_fix(SS_FLOAT_VALUE(obj));

    else if (SS_charobjp(obj))
       l = SS_CHARACTER_VALUE(obj);

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
                 SS_error("CAN'T MAKE VALUE - _SS_FIX_ARG", obj);
                 break;

            default       :
                 hand       = (PFVoid) SS_C_PROCEDURE_HANDLER_PTR(obj);
                 u.funcaddr = (PFInt) SS_C_PROCEDURE_FUNCTION_PTR(obj);
                 if (hand == (PFVoid) SS_acc_char)
                    l = *(char *) u.memaddr;
                 else if (hand == (PFVoid) SS_acc_int)
                    l = *(int *) u.memaddr;
                 else if (hand == (PFVoid) SS_acc_long)
                    l = *(long *) u.memaddr;
                 else if (hand == (PFVoid) SS_acc_REAL)
                    l = *(double *) u.memaddr;
                 else
                    SS_error("BAD VARIABLE TYPE - _SS_FIX_ARG", obj);};}

    else if (SS_nullobjp(obj))
       l = 0;

    else
       SS_error("BAD OBJECT - _SS_FIX_ARG", obj);

    if (type == SC_CHAR_I)
       {cp  = (char *) v;
	*cp = (char) l;}

    else if (type == SC_SHORT_I)
       {sp  = (short *) v;
	*sp = (short) l;}

    else if ((type == SC_INT_I) || (type == SC_ENUM_I))
       {ip  = (int *) v;
	*ip = (int) l;}

    else if (type == SC_LONG_I)
       {lp  = (long *) v;
	*lp = (long) l;}

    else if (type == SC_LONG_LONG_I)
       {llp  = (long long *) v;
	*llp =  l;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_FLOAT_ARG - get a C level floating point data item from
 *               - a single Scheme object
 */

static void _SS_float_arg(object *obj, void *v, int type)
   {int which;
    double d, *dp;
    float *fp;

     d = 0.0;

    if (SS_integerp(obj))
       d = SS_INTEGER_VALUE(obj);

    else if (SS_floatp(obj))
       d = SS_FLOAT_VALUE(obj);

    else if (SS_charobjp(obj))
       d = SS_CHARACTER_VALUE(obj);

    else if (SS_procedurep(obj))
       {PFVoid hand;
        SC_address u;

        switch (SS_PROCEDURE_TYPE(obj))
           {case SS_MACRO : 
            case SS_PROC  :
                 SS_error("CAN'T MAKE VALUE - _SS_FLOAT_ARG", obj);
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
                 else if (hand == (PFVoid) SS_acc_REAL)
                    d = *(double *) u.memaddr;
                 else
                    SS_error("BAD VARIABLE TYPE - _SS_FLOAT_ARG", obj);};}

    else if (SS_nullobjp(obj))
       d = 0.0;

    else
       SS_error("BAD OBJECT - _SS_FLOAT_ARG", obj);

    which = (sizeof(REAL) == sizeof(double));
    if (which)
       {if (type == SC_FLOAT_I)
	   {fp  = (float *) v;
	    *fp = (float) d;}
	else if ((type == SC_REAL_I) || (type == SC_DOUBLE_I))
	   {dp  = (double *) v;
	    *dp = d;};}

    else
       {if ((type == SC_FLOAT_I) || (type == SC_REAL_I))
	   {fp  = (float *) v;
	    *fp = (float) d;}
	else if (type == SC_DOUBLE_I)
	   {dp  = (double *) v;
	    *dp = d;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_ARGS - get a C level data item from a single SCHEME object */

static void _SS_args(object *obj, void *v, int type)
   {void **pv;
    char *s;
    procedure *pp;

    pv = (void **) v;

/* if the object has been GC'd along the line NULL out the C level item */
    if (obj->val == NULL)
       {DEREF(v) = NULL;
        return;};

    if ((type == SC_CHAR_I) ||
	(type == SC_SHORT_I) ||
	(type == SC_INT_I) ||
	(type == SC_ENUM_I) ||
	(type == SC_LONG_LONG_I) ||
	(type == SC_LONG_I))
       _SS_fix_arg(obj, v, type);

    else if ((type == SC_FLOAT_I) ||
	     (type == SC_REAL_I) ||
	     (type == SC_DOUBLE_I))
       _SS_float_arg(obj, v, type);

    else if (type == SC_STRING_I)
       {s = _SS_get_print_name(obj);
	DEREF(v) = s;}

    else if (type == SS_OBJECT_I)
       *pv = (void *) obj;

    else if (type == SS_PROCEDURE_I)
       {pp  = (procedure *) obj->val;
	*pv = (void *) pp->proc;}

    else if (type == SS_INPUT_PORT_I)
       *pv = (void *) SS_INSTREAM(obj);

    else if (type == SS_OUTPUT_PORT_I)
       *pv = (void *) SS_OUTSTREAM(obj);

#ifdef LARGE

    else if (type == SS_HAELEM_I)
       {if (!SS_haelemp(obj))
	   SS_error("OBJECT NOT HASH_ELEMENT - _SS_ARGS", obj);
	*pv = obj->val;}

    else if (type == SS_HASHARR_I)
       {if (!SS_nullobjp(obj))
	   {if (!SS_hasharrp(obj))
	       SS_error("OBJECT NOT HASH_ARRAY - _SS_ARGS", obj);
	    *pv = obj->val;};}

    else if (type == SS_PROCESS_I)
       {if (!SS_processp(obj))
	   SS_error("OBJECT NOT PROCESS - _SS_ARGS", obj);
	*pv = obj->val;}

    else if (type == SS_VECTOR_I)
       {if (!SS_vectorp(obj))
	   SS_error("OBJECT NOT VECTOR - _SS_ARGS", obj);
	*pv = obj->val;}

#endif

    else
       {if (SS_arg_hook != NULL)
	   (*SS_arg_hook)(obj, v, type);
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

int SS_args(object *s, ...)
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
            _SS_args(obj, v, type);};}
    else
       {len = SS_length(s);
        while (SS_consp(s))
           {obj = SS_car(s);
            s   = SS_cdr(s);

            type = SC_VA_ARG(int);
            if (type == 0)
               break;

            v = SC_VA_ARG(void *);
            if (v == (void *) LAST)
               break;

            _SS_args(obj, v, type);};};

    SC_VA_END;

    return(len);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_DEFINE_CONSTANT - bind a given compiled value as
 *                    - a SCHEME variable of a specified type
 */

object *SS_define_constant(int n, ...)
   {int type;
    long long il;
    double dc;
    char *name, *s;
    object *vr, *val;

    vr  = SS_null;
    val = SS_null;
    

    SC_VA_START(n);

    while ((name = SC_VA_ARG(char *)) != NULL)
      {type = SC_VA_ARG(int);
       if ((type == SC_CHAR_I) ||
	   (type == SC_SHORT_I) ||
	   (type == SC_INT_I) ||
	   (type == SC_ENUM_I))
	  {il  = SC_VA_ARG(int);
	   val = SS_mk_integer(il);}

       else if (type == SC_LONG_I)
	  {il  = SC_VA_ARG(long);
	   val = SS_mk_integer(il);}

       else if (type == SC_LONG_LONG_I)
	  {il  = SC_VA_ARG(long long);
	   val = SS_mk_integer(il);}

       else if ((type == SC_FLOAT_I) ||
		(type == SC_DOUBLE_I))
	  {dc  = SC_VA_ARG(double);
	   val = SS_mk_float(dc);}

       else if (type == SC_STRING_I)
	  {s   = SC_VA_ARG(char *);
	   val = SS_mk_string(s);}

       else if (type == SS_OBJECT_I)
	  val = SC_VA_ARG(object *);

       else
	  SS_error("UNSUPPORTED TYPE - SS_DEFINE_CONSTANT",
		   SS_null);

       vr = SS_mk_variable(name, SS_null);
       SS_UNCOLLECT(vr);

       SC_hasharr_install(SS_symtab, name, vr, SS_POBJECT_S, TRUE, TRUE);
       SS_def_var(vr, val, SS_Global_Env);};

    SC_VA_END;

    return(vr);}

/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_VAR_VALUE - given the name of a current SCHEME object which
 *              - has correspondence with a C or FORTRAN variable
 *              - make the given pointer point to its value
 *              - if flag is TRUE and the object is a variable return its
 *              - binding in the current environment
 */

void SS_var_value(char *s, int type, void *vr, int flag)
   {object *obj;

    obj = SS_INQUIRE_OBJECT(s);

    if (flag && SS_variablep(obj))
       obj = SS_lk_var_val(obj, SS_Env);

    if ((obj == NULL) || SS_nullobjp(obj))
       DEREF(vr) = NULL;

    else
       SS_args(obj, type, vr, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_VAR_REFERENCE - given the name of a current SCHEME object which
 *                  - has correspondence with a C or FORTRAN variable
 *                  - return a pointer to its value
 */

void *SS_var_reference(char *s)
   {object *obj;
    void *vr = NULL;

    obj = SS_INQUIRE_OBJECT(s);
    if (SS_variablep(obj))
       obj = SS_lk_var_val(obj, SS_Env);

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
                 SS_error("CAN'T MAKE VALUE - _SS_FLOAT_ARG", obj);
                 break;

            default       :
                 hand       = (PFVoid) SS_C_PROCEDURE_HANDLER_PTR(obj);
                 u.funcaddr = (PFInt) SS_C_PROCEDURE_FUNCTION_PTR(obj);
                 if ((hand == (PFVoid) SS_acc_char) ||
                     (hand == (PFVoid) SS_acc_int) ||
                     (hand == (PFVoid) SS_acc_long) ||
                     (hand == (PFVoid) SS_acc_REAL))
                    vr = u.memaddr;
                 else
                    SS_error("BAD VARIABLE TYPE - SS_VAR_REFERENCE", obj);};}
    else
       SS_error("BAD OBJECT - SS_VAR_REFERENCE", obj);

    return(vr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAKE_LIST - make a SCHEME list from a C arg list */

static object *_SS_make_list(int n, int *type, void **ptr)
   {int i, j, c, ityp;
    long l;
    long long ll;
    double d;
    char *s;
    object *o, *lst;

    lst = SS_null;
    for (i = 0; i < n; i++)
        {ityp = type[i];
	 if ((ityp == SC_SHORT_I) ||
	     (ityp == SC_INT_I) ||
	     (ityp == SC_ENUM_I))
	    {j   = *(int *) ptr[i];
	     lst = SS_mk_cons(SS_mk_integer((long long) j), lst);}
 
	 else if (ityp == SC_LONG_I)
	    {l   = *(long *) ptr[i];
	     lst = SS_mk_cons(SS_mk_integer((long long) l), lst);}

	 else if (ityp == SC_LONG_LONG_I)
	    {ll   = *(long long *) ptr[i];
	     lst = SS_mk_cons(SS_mk_integer(ll), lst);}

	 else if (ityp == SC_FLOAT_I)
	    {d   = *(float *) ptr[i];
	     lst = SS_mk_cons(SS_mk_float(d), lst);}

	 else if (ityp == SC_REAL_I)
	    {d   = *(double *) ptr[i];
	     lst = SS_mk_cons(SS_mk_float(d), lst);}

	 else if (ityp == SC_DOUBLE_I)
	    {d   = *(double *) ptr[i];
	     lst = SS_mk_cons(SS_mk_float(d), lst);}
 
	 else if (ityp == SC_STRING_I)
	    {s   = (char *) ptr[i];
	     lst = SS_mk_cons(SS_mk_string(s), lst);}
 
	 else if (ityp == SS_OBJECT_I)
	    {o   = (object *) ptr[i];
	     lst = SS_mk_cons(o, lst);}
 
#ifdef LARGE
 
	 else if (ityp == SC_CHAR_I)
	    {c   = *(int *) ptr[i];
	     lst = SS_mk_cons(SS_mk_char(c), lst);}
 
	 else if (ityp == SS_HAELEM_I)
	    {haelem *hp;
	     hp  = (haelem *) ptr[i];
	     lst = SS_mk_cons(SS_mk_haelem(hp), lst);}
 
	 else if (ityp == SS_HASHARR_I)
	    {hasharr *ht;
	     ht  = (hasharr *) ptr[i];
	     lst = SS_mk_cons(SS_mk_hasharr(ht), lst);}

#endif
 
	 else
	    {if (SS_call_arg_hook != NULL)
	        {o   = (*SS_call_arg_hook)(type[i], ptr[i]);
		 lst = SS_mk_cons(o, lst);}
	     else
	        lst = SS_mk_cons(SS_null, lst);};};

    o = SS_reverse(lst);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MAKE_LIST - make a SCHEME list at the C level */

object *SS_make_list(int first, ...)
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

    o = _SS_make_list(i, type, ptr);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MAKE_FORM - make a SCHEME form which is a list of objects */

object *SS_make_form(object *first, ...)
   {int i, type[MAXLINE];
    void *ptr[MAXLINE];
    object *expr;

    type[0] = SS_OBJECT_I;
    ptr[0]  = first;

    SC_VA_START(first);

    for (i = 1; i < MAXLINE; i++)
        {type[i] = SS_OBJECT_I;
         ptr[i]  = SC_VA_ARG(void *);
         if ((ptr[i] == (void *) LAST) || (ptr[i] == NULL))
            break;};

    SC_VA_END;

    expr = _SS_make_list(i, type, ptr);

    return(expr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_EVAL_FORM - make and evaluate a SCHEME form
 *              - which is a list of objects
 */

object *SS_eval_form(object *first, ...)
   {int i, type[MAXLINE];
    void *ptr[MAXLINE];
    object *expr, *res;

    type[0] = SS_OBJECT_I;
    ptr[0]  = first;

    SC_VA_START(first);

    for (i = 1; i < MAXLINE; i++)
        {type[i] = SS_OBJECT_I;
         ptr[i]  = SC_VA_ARG(void *);
         if ((ptr[i] == (void *) LAST) || (ptr[i] == NULL))
            break;};

    SC_VA_END;

    expr = SS_null;
    SS_Assign(expr, _SS_make_list(i, type, ptr));
    res = SS_exp_eval(expr);
    SS_Assign(expr, SS_null);

    return(res);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SSCHEM - do a Fortran version of SS_call_scheme */

FIXNUM F77_FUNC(sschem, SSCHEM)(FIXNUM *pnc, F77_string name, ...)
   {int i, type[MAXLINE];
    FIXNUM rv;
    void *ptr[MAXLINE];
    char func[80];
    SC_address ret;
    object *fnc, *expr;
    
    SC_FORTRAN_STR_C(func, name, *pnc);

    SC_VA_START(name);

    fnc = (object *) SC_hasharr_def_lookup(SS_symtab, func);
    if (fnc == NULL)
       SS_error("UNKNOWN PROCEDURE - SSCHEM", SS_mk_string(func));

    for (i = 0; i < MAXLINE; i++)
        {type[i] = *SC_VA_ARG(int *);
         if (type[i] == 0)
            break;

         ptr[i] = (void *) *SC_VA_ARG(char **);
         if (ptr[i] == (void *) LAST)
            break;};

    SC_VA_END;

    expr = SS_null;
    SS_Assign(expr, SS_mk_cons(fnc, _SS_make_list(i, type, ptr)));

    SS_eval(expr);

    SS_Assign(expr, SS_null);

    ret.memaddr = (char *) SS_Val;

    rv = ret.diskaddr;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CALL_SCHEME - make a C level call to a SCHEME level function
 *                - return the SCHEME result object
 */

object *SS_call_scheme(char *func, ...)
   {int i, type[MAXLINE];
    object *fnc, *expr;
    void *ptr[MAXLINE];

    SC_VA_START(func);

    fnc = _SS_lk_var_valc(func, SS_Env);
/*    fnc = (object *) SC_hasharr_def_lookup(SS_symtab, func); */
    if (fnc == NULL)
       SS_error("UNKNOWN PROCEDURE - SS_CALL_SCHEME", SS_mk_string(func));

    for (i = 0; i < MAXLINE; i++)
        {type[i] = SC_VA_ARG(int);
         if (type[i] == 0)
            break;

         ptr[i] = SC_VA_ARG(void *);
         if (ptr[i] == (void *) LAST)
            break;};

    SC_VA_END;

    SC_mem_stats_set(0L, 0L);

    expr = SS_mk_cons(fnc, _SS_make_list(i, type, ptr));
    SC_mark(expr, 1);

    SS_eval(expr);

    SS_GC(expr);

    SS_Assign(SS_Env, SS_Global_Env);
    SS_Assign(SS_This, SS_null);
    SS_Assign(SS_Exn, SS_null);
    SS_Assign(SS_Unev, SS_null);
    SS_Assign(SS_Argl, SS_null);
    SS_Assign(SS_Fun, SS_null);

    return(SS_Val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_RUN - evaluate a single form which is given on the command line 
 *         - as a string
 */

static int _SS_run(void)
   {int iret;
    object *port, *ret;
    
    port = SS_mk_inport(stdin, "stdin");
    SC_strncpy(SS_BUFFER(port), MAX_BFSZ, _SS.ibf, strlen(_SS.ibf));
    SS_PTR(port) = SS_BUFFER(port);

    ret  = SS_eval(SS_read(port));
    iret = FALSE;
    if (SS_numbp(ret))
       SS_args(ret,
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

int SS_run(char *s)
   {int rv;

    while (strchr(" \t\n\r\f", *s++) != NULL);
    _SS_load_bf(--s);

    rv = SS_err_catch(_SS_run, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LOAD_SCM - load an SCHEME file with error protection */

static int _SS_load_scm(void)
   {

    SS_call_scheme("load",
                   SC_STRING_I, _SS.ibf,
                   SS_OBJECT_I, SS_t,
                   0);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LOAD_SCM - load a SCHEME file with error protection */

int SS_load_scm(char *name)
   {int rv;

    _SS_load_bf(name);

    rv = SS_err_catch(_SS_load_scm, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_TEXT_DATA_FILEP - test for numeric text data files
 *                    - look for comment lines beginning with CMNT
 *                    - all other non-blank lines should have number
 *                    - return TRUE if FP conforms to this pattern
 */

int SS_text_data_filep(char *fname, int cmnt)
   {int ok;
    char bf[MAXLINE];
    char *s, *p, *token;
    FILE *fp;

    ok = TRUE;
    fp = io_open(fname, "r");

    while ((io_gets(bf, MAXLINE, fp) != NULL) && (ok == TRUE))
       {if (bf[0] == cmnt)
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

int SS_define_argv(char *program, int c, char **v)
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
       {for (n = 0, i = 0; i < c; i++)
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
		 lp  = FMAKE(long, "SS_DEFINE_ARGV:lp");
		 *lp = SC_stoi(s);
		 p   = lp;}
	     else if (SC_fltstrp(s))
	        {t   = SC_DOUBLE_I;
		 dp  = FMAKE(double, "SS_DEFINE_ARGV:dp");
		 *dp = SC_stof(s);
		 p   = dp;}
	     else
	        {t = SC_STRING_I;
		 p = s;}

	     type[n] = t;
	     ptr[n]  = p;
	     n++;};

/* make the arg list available in interpreted code */
	argl = _SS_make_list(n, type, ptr);

	SS_define_constant(1,
			   "argv", SS_OBJECT_I, argl,
			   NULL);

/* load the full Scheme */
	SS_load_scm("runt.scm");

/* load the script file - this might not return */
	SS_load_scm(script);

	rv = TRUE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
