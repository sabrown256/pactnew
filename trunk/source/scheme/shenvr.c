/*
 * SHENVR.C - environment and eval control routines for Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

#define GET_FRAME(_n, _t, _e)                                               \
   {object *_l, *_v;                                                        \
    _l = SS_car(_e);                                                        \
    _v = SS_car(_l);                                                        \
    _t = SS_GET(hasharr, SS_cadr(_l));                                      \
    _n = NULL;                                                              \
    if (SS_variablep(_v))                                                   \
       {_n = SS_VARIABLE_NAME(_v);                                          \
        SC_ASSERT(_n != NULL);}                                             \
    else if (SS_stringp(_v))                                                \
       {_n = SS_STRING_TEXT(_v);                                            \
        SC_ASSERT(_n != NULL);};}

typedef object *(*PFZargs)(SS_psides *si);
typedef object *(*PFSargs)(SS_psides *si, object *argl);

char
 *SS_OBJECT_P_S = "object *";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPROC - print out the given compound procedure object
 *       - this is a debugging aide solely
 */

void dproc(SS_psides *si, object *pp)
   {object *name, *params, *penv, *bdy;

    if (si == NULL)
       si = &_SS_si;       /* diagnostic default */

    if (SS_procedurep(pp))
       {name   = SS_proc_name(pp);
	params = SS_params(pp);
	penv   = SS_proc_env(pp);
	bdy    = SS_proc_body(pp);

	SC_ASSERT(penv != NULL);

	SS_print(si, si->outdev, name,   "Name: ", "\n");
	SS_print(si, si->outdev, params, "Params: ", "\n");
	SS_print(si, si->outdev, bdy,    "Body: ", "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPENV - print out the entire environment frame stack given
 *       - this is a debugging aide solely
 */

void dpenv(SS_psides *si, object *penv)
   {int i, ie, n, nx;
    char pre[MAXLINE];
    char *fname, **lines;
    hasharr *tab;
    object *b;

    if (si == NULL)
       si = &_SS_si;       /* diagnostic default */

    if (penv == NULL)
       penv = si->env;

    for (ie = 1; !SS_nullobjp(penv); penv = SS_cdr(penv), ie++)
        {GET_FRAME(fname, tab, penv);

	 if ((fname != NULL) && (strcmp(fname, "global-environment") != 0))
	    {lines = SC_hasharr_dump(tab, NULL, NULL, TRUE);

	     if (lines != NULL)

/* subtract 1 for the NULL entry in lines signalling the end */
	        {SC_ptr_arr_len(nx, lines);

		 PRINT(stdout, "----------------------------------------\n");
		 PRINT(stdout, "Frame %d: %s\n", ie, fname);

		 n = min(nx, si->trace_env);
		 for (i = 0; i < n; i++)
		     {if ((i != (n-1)) || (lines[i] != NULL))
			 {b = (object *) SC_hasharr_def_lookup(tab, lines[i]);
			  snprintf(pre, MAXLINE, "%4d\t%s\t", i+1, lines[i]);
			  SS_print(si, si->outdev, b, pre, "\n");};};

		 if (n != nx)
		    {PRINT(stdout, "\t...\n");
		     b = (object *) SC_hasharr_def_lookup(tab, lines[nx-1]);
		     snprintf(pre, MAXLINE, "%4d\t%s\t", nx, lines[nx-1]);
		     SS_print(si, si->outdev, b, pre, "\n");};

		 CFREE(lines);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_EXP_EVAL - make an explicit interpreter level call to eval */

object *SS_exp_eval(SS_psides *si, object *obj)
   {

    SS_Assign(si->exn, obj);

    _SS_eval(si);

    return(si->val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_EVAL - make an implicit C level call to eval */

object *SS_eval(SS_psides *si, object *obj)
   {

    SS_Assign(si->exn, obj);
    SS_Assign(si->env, si->global_env);

    _SS_eval(si);

    return(si->val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SAVE_REGISTERS - save the Scheme register set on the stack */

void SS_save_registers(SS_psides *si, int vp)
   {

    if (vp)
       SS_Save(si, si->val);

    SS_Save(si, si->exn);
    SS_Save(si, si->env);
    SS_Save(si, si->fun);
    SS_Save(si, si->this);
    SS_Save(si, si->unev);
    SS_Save(si, si->argl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RESTORE_REGISTERS - restore the Scheme register set from the stack */

void SS_restore_registers(SS_psides *si, int vp)
   {

    SS_Restore(si, si->argl);
    SS_Restore(si, si->unev);
    SS_Restore(si, si->this);
    SS_Restore(si, si->fun);
    SS_Restore(si, si->env);
    SS_Restore(si, si->exn);

    if (vp)
       SS_Restore(si, si->val);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_TRUE - return TRUE if object is not boolean #f or () */

int SS_true(object *obj)
   {int rv;

    if (SS_booleanp(obj))
       rv = SS_BOOLEAN_VALUE(obj);
    else if (SS_nullobjp(obj))
       rv = FALSE;
    else
       rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                               HANDLERS                                   */

/*--------------------------------------------------------------------------*/

/* SS_ZARGS - zero argument macro/procedure handler */

object *SS_zargs(SS_psides *si, C_procedure *cp, object *argl)
   {object *o;
    PFZargs f;

    f = (PFZargs) cp->proc[0];
    o = f(si);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SARGS - single argument macro/procedure handler */

object *SS_sargs(SS_psides *si, C_procedure *cp, object *argl)
   {object *o;
    PFSargs f;

    f = (PFSargs) cp->proc[0];
    o = f(si, SS_car(argl));

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_NARGS - n argument macro/procedure handler */

object *SS_nargs(SS_psides *si, C_procedure *cp, object *argl)
   {object *o;
    PFSargs f;

    f = (PFSargs) cp->proc[0];
    o = f(si, argl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ZNARGS - zero or more arguments macro/procedure handler */

object *SS_znargs(SS_psides *si, C_procedure *cp, object *argl)
   {object *o;
    PFSargs f;

    f = (PFSargs) cp->proc[0];
    o = f(si, argl);

    return(o);}

/*--------------------------------------------------------------------------*/

/*                         PROCEDURE ACCESSORS                              */

/*--------------------------------------------------------------------------*/

/* SS_PROC_NAME - returns the name of the procedure */

object *SS_proc_name(object *fun)
   {object *rv;
    S_procedure *sp;

    sp = SS_COMPOUND_PROCEDURE(fun);
    rv = sp->name;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PROC_ENV - returns the environment frame index of the procedure */

object *SS_proc_env(object *fun)
   {object *rv;
    S_procedure *sp;

    sp = SS_COMPOUND_PROCEDURE(fun);
    rv = sp->env;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARAMS - returns a list of the formal parameters of the procedure */

object *SS_params(object *fun)
   {object *le, *rv;
    S_procedure *sp;

    sp = SS_COMPOUND_PROCEDURE(fun);
    le = sp->lambda;
    rv = SS_car(le);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PROC_BODY - returns a list of the body or the procedure */

object *SS_proc_body(object *fun)
   {object *le, *rv;
    S_procedure *sp;

    sp = SS_COMPOUND_PROCEDURE(fun);
    le = sp->lambda;
    rv = SS_cdr(le);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_NEW_FRAME - make a new environmental frame */

object *SS_mk_new_frame(SS_psides *si, object *name, hasharr *tab)
   {object *pt, *fr;

    if (tab == NULL)
       tab = SC_make_hasharr(HSZHUGE, NODOC, SC_HA_NAME_KEY);

    pt = SS_mk_hasharr(si, tab);

    fr = SS_make_form(si, name, pt, LAST);

    return(fr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ADD_TO_FRAME - install a variable in the environment frame hash table
 *                 - if its name begins with "$" and we are being aware
 *                 - of OS environment variables add it to the OS environment
 */

static void SS_add_to_frame(SS_psides *si, char *vr, object *vl, hasharr *tab)
   {int nc, ok;
    char *s, *t;

    SC_hasharr_install(tab, vr, vl, SS_OBJECT_P_S, TRUE, TRUE);

    if ((si->know_env) && (vr[0] == '$'))
       {t = _SS_sprintf(si, "%s", vl);

	nc = strlen(vr) + strlen(t) + 2;
	s  = SC_permanent(CMAKE_N(char, nc));
	snprintf(s, nc, "%s=%s", vr+1, t);

	ok = SC_putenv(s);
	SC_ASSERT(ok == TRUE);

	CFREE(s);
	CFREE(t);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MK_FRAME - make a new environmental frame */

static object *_SS_mk_frame(SS_psides *si, object *fnm,
			    object *vrs, object *vls)
   {object *fr, *var, *val;
    char *name;
    hasharr *tab;

    tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);

    if (SS_variablep(vrs))
       {name = SS_VARIABLE_NAME(vrs);
	SS_add_to_frame(si, name, vls, tab);}

    else
       {for (; SS_consp(vrs); vrs = SS_cdr(vrs))
            {if (SS_consp(vls))
                {val = SS_car(vls);
                 vls = SS_cdr(vls);}
             else
                val = SS_null;

             var  = SS_car(vrs);
	     name = SS_VARIABLE_NAME(var);

	     SS_add_to_frame(si, name, val, tab);};

/* if end of vrs is not NULL we had dotted arg list */
        if (!SS_nullobjp(vrs))
	   {name = SS_VARIABLE_NAME(vrs);
	    SS_add_to_frame(si, name, vls, tab);};};

    fr = SS_mk_new_frame(si, fnm, tab);

    return(fr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_XTND_ENV - A and S environment control */

static object *_SS_xtnd_env(SS_psides *si, object *fnm,
			    object *vrs, object *vls, object *base)
   {object *env;
    
    env = SS_mk_cons(si, _SS_mk_frame(si, fnm, vrs, vls), base);

    return(env);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_DO_BINDINGS - bind the parameters to the values in a new frame
 *                - and return the resulting environment
 */

object *SS_do_bindings(SS_psides *si, object *pp, object *argp)
   {object *params, *penv, *nenv, *name;

    if (!SS_procedurep(pp))
       SS_error(si, "BAD PROCEDURE TO DO-BINDINGS", pp);

    name   = SS_proc_name(pp);
    params = SS_params(pp);
    penv   = SS_proc_env(pp);
    nenv   = _SS_xtnd_env(si, name, params, argp, penv);

    if ((si->trace_env > 0) && (!SS_nullobjp(params)))
       dpenv(si, nenv);

    return(nenv);}

/*--------------------------------------------------------------------------*/

/*                     ENVIRONMENT CONTROL ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* SS_BOUND_VARS - return an array of the names of variable in PENV
 *               - which match PATT
 */

char **SS_bound_vars(SS_psides *si, char *patt, object *penv)
   {int i;
    char *fname, **vrs;
    hasharr *tab;

    if (penv == NULL)
       penv = si->env;

    GET_FRAME(fname, tab, penv);

    vrs = SC_hasharr_dump(tab, patt, NULL, TRUE);
    for (i = 0; vrs[i] != NULL; i++)
        vrs[i] = CSTRSAVE(vrs[i]);

    return(vrs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BOUND_NAME - lookup an object by NAME in the symbol table
 *               - or the current environment
 *               - return it if found and NULL otherwise
 *               - return the name under which it is found in UNDER
 */

object *SS_bound_name(SS_psides *si, char *name)
   {object *obj;

    obj = _SS_bind_envc(si, name, si->env);
    if (obj == NULL)
       obj = (object *) SC_hasharr_def_lookup(si->symtab, name);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BOUND_VAR - lookup an object by NAME in TAB
 *               - return it if found and NULL otherwise
 *               - return the name under which it is found in UNDER
 */

static object *_SS_bound_var(SS_psides *si, char *name,
			     hasharr *tab, char *under)
   {char s[MAXLINE], t[MAXLINE];
    object *b;

    if (under == NULL)
       under = t;

    strcpy(under, name);
    b = (object *) SC_hasharr_def_lookup(tab, under);

/* NOTE: get a second chance if one of the syntax modes has defined a
 * name mapping scheme
 * this allows a definition to overwrite an existing one
 * even if posed in a different syntax mode
 * so a C mode define of foo_bar could replace an existing foo-bar
 */
    if ((b == NULL) && (si->name_reproc != NULL))
       {(*si->name_reproc)(si, s, name);
	strcpy(under, s);
	b = (object *) SC_hasharr_def_lookup(tab, under);};

    return(b);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_GET_VARIABLE - traverse the list of environment frames PENV
 *                  - looking for a variable object matching NAME
 *                  - we just want the variable not what it is bound to
 */

object *_SS_get_variable(char *name, object *penv)
   {int i;
    char *fname;
    hasharr *tab;
    object *o;

    GET_FRAME(fname, tab, penv);

    o = NULL;
    if (name != NULL)
       {for (i = 0; !SS_nullobjp(penv); penv = SS_cdr(penv), i++)
	    {GET_FRAME(fname, tab, penv);
	     o = (object *) SC_hasharr_def_lookup(tab, name);
	     if (o != NULL)
	        break;};};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_SEARCH_FRAMES - traverse the list of environment frames PENV
 *                   - looking for an entry matching NAME
 *                   - NAME may have to be mapped by syntax modes
 *                   - so if found return the actual name UNDER
 *                   - which it was installed
 *                   - return either the hash table in which it was found
 *                   - or the first hash table in the list of frames
 *                   - via the argument PTAB
 *                   - return the found object if any in A
 */

static object *_SS_search_frames(SS_psides *si, char *name, object *penv,
				 char *under, hasharr **ptab)
   {int i;
    char *fname;
    hasharr *tab;
    object *b;

    if (ptab != NULL)
       {GET_FRAME(fname, tab, penv);
	*ptab = tab;};

    b = NULL;
    if (name != NULL)
       {for (i = 0; !SS_nullobjp(penv); penv = SS_cdr(penv), i++)
	    {GET_FRAME(fname, tab, penv);
	     b = _SS_bound_var(si, name, tab, under);
	     if (b != NULL)
	        {if (ptab != NULL)
		    *ptab = tab;
		 break;};};};

    return(b);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BIND_ENVC - return the binding of the NAME'd variable
 *               - in the given environment
 */

object *_SS_bind_envc(SS_psides *si, char *name, object *penv)
   {object *b;

    b = _SS_search_frames(si, name, penv, NULL, NULL);

    return(b);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_VAR - set the specified variable to the given value
 *            - in the specified environment
 */

void SS_set_var(SS_psides *si, object *vr, object *vl, object *penv)
   {char under[MAXLINE];
    char *name;
    hasharr *tab;
    object *b;

    name = SS_VARIABLE_NAME(vr);

    b = _SS_search_frames(si, name, penv, under, &tab);

    if (b != NULL)
       {SS_gc(b);
	SS_add_to_frame(si, under, vl, tab);}

    else
       SS_error(si, "UNBOUND VARIABLE - SS_SET_VAR", vr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_REM_VARC - remove the specified variable
 *              - in the specified environment
 *              - NOTE: this is used in the C syntax mode if nowhere else
 */

void _SS_rem_varc(SS_psides *si, char *name, object *penv)
   {char under[MAXLINE];
    hasharr *tab;
    object *b;

    b = _SS_search_frames(si, name, penv, under, &tab);

    if (b != NULL)
       {SS_gc(b);
	SC_hasharr_remove(tab, under);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_DEF_VARC - define the specified variable to the given value
 *              - in the specified environment
 */

static void _SS_def_varc(SS_psides *si, char *name, object *vl, object *penv)
   {char under[MAXLINE];
    object *b;
    hasharr *tab;

    b = _SS_search_frames(si, name, penv, under, &tab);

    if (b != NULL)
       {SS_gc(b);
	SS_add_to_frame(si, under, vl, tab);}

    else
       SS_add_to_frame(si, name, vl, tab);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BIND_ENV - return the binding of the variable VR
 *             - in the given environment
 */

object *SS_bind_env(SS_psides *si, object *vr, object *penv)
   {char *name;
    object *o;

    name = SS_VARIABLE_NAME(vr);

    o = _SS_bind_envc(si, name, penv);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_DEFP - defined? predicate in SCHEME */

object *_SSI_defp(SS_psides *si, object *vr)
   {object *b, *o;

    b = SS_bind_env(si, vr, si->env);
    o = (b == NULL) ? SS_f : SS_t;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_LK_VAR_VALC - look up the variable in the environment */

object *_SS_lk_var_valc(SS_psides *si, char *name, object *penv)
   {object *b, *obj;

    obj = SS_null;
    b   = _SS_bind_envc(si, name, penv);
    if (b != NULL)
       obj = b;

/* look in the symbol table - it may be a variable holding a primitive
 * procedure object
 */
    else
       {b = SS_INQUIRE_OBJECT(si, name);
        if (b != NULL)
           obj = SS_VARIABLE_VALUE(b);

        if (SS_nullobjp(obj))
           return(NULL);};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LK_VAR_VAL - look up the variable in the environment */

object *SS_lk_var_val(SS_psides *si, object *vr)
   {char *name;
    char s[MAXLINE];
    object *obj, *penv;

    penv = si->env;
    name = SS_VARIABLE_NAME(vr);
    obj  = _SS_lk_var_valc(si, name, penv);

/* NOTE: get a second chance if one of the syntax modes has defined a
 * name mapping scheme
 * for example while for-each is a perfectly good name in SCHEME
 * it's poison for C so in C-mode we could map the legal
 * for_each into for-each and do the lookup
 */
    if ((obj == NULL) && (si->name_reproc != NULL))
       {(*si->name_reproc)(si, s, name);
	obj = _SS_lk_var_valc(si, s, penv);};

/* if there is no variable complain */
    if (obj == NULL)
       SS_error(si, "UNBOUND VARIABLE - SS_LK_VAR_VAL", vr);

    else if ((obj->val == NULL) || (obj->eval_type == NO_EV))
       SS_error(si, "FREED OBJECT - SS_LK_VAR_VAL", SS_null);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_GET_PRINT_NAME - return a good printing name for the object O */

char *_SS_get_print_name(object *o)
   {char *s, *rv;
    procedure *pp;

    rv = NULL;
    s  = NULL;

    if (o != NULL)
       {if (SS_procedurep(o))
	   {pp = SS_GET(procedure, o);
	    switch (pp->type)
	       {case SS_MACRO :
		case SS_PROC  :
		     s = SS_proc_name(o)->print_name;
		     break;

		case SS_PR_PROC  :
		case SS_UR_MACRO :
		case SS_EE_MACRO :
		case SS_UE_MACRO :
		case SS_ESC_PROC :
		     s = SS_PROCEDURE_NAME(o);
		     break;
		default :
		     s = "procedure";
		     break;};}

        else if (o->print_name != NULL)
	   s = o->print_name;
	else
	   s = "- no print name -";

	rv = CSTRSAVE(s);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_DEF_VAR - define the specified variable to the given value
 *            - in the specified environment
 */

void SS_def_var(SS_psides *si, object *vr, object *vl, object *penv)
   {char *name;

    name = SS_VARIABLE_NAME(vr);

    _SS_def_varc(si, name, vl, penv);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ENV_VARS - add the environment variables defined in VRS
 *             - to the environment frame PENV
 */

void SS_env_vars(SS_psides *si, char **vrs, object *penv)
   {int i, nc;
    char *s, *p, *vr;
    object *vl;

    if (vrs == NULL)
       vrs = SC_get_env();

    if (vrs != NULL)
       {if (penv == NULL)
	   penv = si->global_env;

	for (i = 0; vrs[i] != NULL; i++)
	    {s = vrs[i];
	     p = strchr(s, '=');
	     if (p != NULL)
	        {*p = '\0';

		 nc = strlen(s) + 2;
		 vr = CMAKE_N(char, nc);
		 snprintf(vr, nc, "$%s", s);

		 vl = SS_mk_string(si, p+1);

		 _SS_def_varc(si, vr, vl, penv);

		 *p  = '=';
		 CFREE(vr);};};};

    si->know_env = TRUE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INIT_THREAD - init state SA for thread ID */

static void _SS_init_thread(SS_smp_state *sa, int id)
   {SS_psides *si;

    si = &_SS_si;

    sa->parser = _SSI_scheme_mode;

    _SSI_scheme_mode(si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_GET_STATE - return the SS state for thread ID
 *               - use current thread if ID == -1
 */

SS_smp_state *_SS_get_state(int id)
   {size_t bpi;
    SS_smp_state *sa;

    if (_SS.ita < 0)
       {bpi     = sizeof(SS_smp_state);
	_SS.ita = SC_register_thread_data("scheme-state", "SS_smp_state",
					  1, bpi, (PFTinit) _SS_init_thread);};

    if (id < 0)
       id = SC_current_thread();

    sa = (SS_smp_state *) SC_get_thread_element(id, _SS.ita);

    return(sa);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
