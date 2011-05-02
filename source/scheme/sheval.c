/*
 * SHEVAL.C - the core eval for Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_CONT - set up a new continuation, execute the jumps
 *             - and release the continuation on return
 */

#define SS_set_cont(_si, _go, _return)                                       \
   {(_si)->nsetc++;                                                          \
    (_si)->cont_ptr++;                                                       \
    if ((_si)->cont_ptr >= (_si)->stack_size)                                \
       SS_expand_stack(si);                                                  \
    if (SETJMP((_si)->continue_int[(_si)->cont_ptr].cont))                   \
       {_SS_end_trace(_si);                                                  \
        (_si)->cont_ptr--;                                                   \
        (_si)->ngoc++;                                                       \
        SS_jump(_return);}                                                   \
    else                                                                     \
       {SS_jump(_go);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GO_CONT - go (LONGJMP) to the current continuation */

#define SS_go_cont(_si)                                                      \
    LONGJMP((_si)->continue_int[(_si)->cont_ptr].cont, TRUE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SAVE - save the object on the Scheme stack */

#ifdef STACK_FNC

void SS_Save(SS_psides *si, object *obj)
   {

    si->nsave++;
    SS_MARK(obj);

    SC_array_push(si->stack, &obj);

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RESTORE - pop an object off the stack and release the stacken */

#ifdef STACK_FNC

void _SS_Restore(SS_psides *si, object **po)
   {object *o;

    o = *po;

    si->nrestore++;
    SS_gc(o);

    *po = *(object **) SC_array_pop(si->stack);

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_EVAL - entry point at C level for eval */

void _SS_eval(SS_psides *si)
   {char msg[MAXLINE];
    char *s;
    C_procedure *cp;
    procedure *pf, *pg;
    SS_form pty;
    
    SS_set_cont(si, eval_disp, ret_val);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EVAL_DISP - eval-dispatch the real starting point for the evaluator */

eval_disp:
    switch (si->exn->eval_type)
       {case NO_EV :
             SS_error_n(si, "ILLEGAL EVALUATION TYPE", si->exn);

	case SELF_EV :
	     SS_jump(self_ev);

        case VAR_EV :
	     SS_jump(var_ev);

        case PROC_EV :
	     SS_jump(proc_ev);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SELF_EV - do self evaluating forms */

self_ev:
    SS_Assign(si->val, si->exn);
    SS_go_cont(si);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VAR_EV - evaluate variables */

var_ev:
    SS_Assign(si->val, SS_lk_var_val(si, si->exn));
    SS_go_cont(si);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROC_EV - evaluate a procedure/list object */

proc_ev:
    SS_Assign(si->unev, SS_cdr(si->exn));
    SS_Assign(si->exn, SS_car(si->exn));
    SS_Save(si, si->unev);
    SS_Save(si, si->env);
    SS_set_cont(si, eval_disp, ev_args);

ev_args:
    SS_Restore(si, si->env);
    SS_Restore(si, si->unev);
    SS_Assign(si->fun, si->val);
    if (!SS_procedurep(si->fun))
       SS_error_n(si, "ILLEGAL PROCEDURE OBJECT", si->fun);

    pf  = SS_GET(procedure, si->fun);
    pty = pf->type;
    switch (pty)
       {case SS_ESC_PROC :

/* procedure with evaluated args and simple return */
        case SS_PROC     : 
        case SS_PR_PROC  :

/* procedure with evaluated args and post-evaluated results */
        case SS_EE_MACRO :
	     SS_Save(si, si->fun);
	     SS_Assign(si->this, SS_null);
	     SS_Assign(si->argl, SS_null);
	     SS_jump(eva_loop);

/* macro with unevaluated args and post-evaluated results */
        case SS_MACRO_EV :
	     SS_jump(ev_macro_ev);

        case SS_UE_MACRO :
	     SS_jump(macro_ue);

/* macro with unevaluated args and simple return */
        case SS_MACRO    :
	     SS_jump(ev_macro);

        case SS_UR_MACRO :
	     SS_jump(macro_ur);

        case SS_BEGIN :
	     SS_jump(ev_begin);

        case SS_DEFINE :
	     SS_jump(ev_define);

        case SS_SET :
	     SS_jump(ev_set);

        case SS_IF :
	     SS_jump(ev_if);

        case SS_AND :
	     SS_jump(ev_and);

        case SS_OR :
	     SS_jump(ev_or);

        case SS_COND :
	     SS_jump(ev_cond);

        default :
	     SS_error_n(si, "BAD PROCEDURE TYPE", si->fun);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET - set! macro in Scheme */

ev_set:
    SS_Assign(si->exn, SS_cadr(si->unev));
    SS_Assign(si->unev, SS_car(si->unev));

    if (!SS_variablep(si->unev))
       SS_error_n(si, "CAN'T SET NON-VARIABLE OBJECT - SET", si->unev);

    SS_Save(si, si->unev);
    SS_Save(si, si->env);
    SS_set_cont(si, eval_disp, ev_seta);

ev_seta:
    SS_Restore(si, si->env);
    SS_Restore(si, si->unev);
    SS_set_var(si, si->unev, si->val, si->env);
    SS_Assign(si->val, si->unev);
    SS_go_cont(si);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_DEFINE - define macro in Scheme */

ev_define:
    _SS_bgn_trace(si, si->fun, si->unev);
    SS_Assign(si->exn, SS_cdr(si->unev));
    SS_Assign(si->unev, SS_car(si->unev));

/* define a compound procedure */
    if (SS_consp(si->unev))
       {SS_Assign(si->exn, SS_mk_cons(si, SS_cdr(si->unev), si->exn));
        SS_Assign(si->unev, SS_car(si->unev));
        SS_Assign(si->val, SS_mk_procedure(si, si->unev, si->exn, si->env));

	pf = SS_GET(procedure, si->fun);
	pg = SS_GET(procedure, si->val);

        if (strcmp(pf->name, "define-macro") == 0)
           pg->type = SS_MACRO;

        else if (strcmp(pf->name, "define-macro-ev") == 0)
           pg->type = SS_MACRO_EV;

        s = SS_VARIABLE_NAME(si->unev);

        CFREE(pg->name);
        pg->name = CSTRSAVE(s);}

/* define a variable */
    else if (SS_variablep(si->unev))
       {SS_Save(si, si->unev);
        SS_Save(si, si->env);
        SS_Assign(si->exn, SS_car(si->exn));
        SS_set_cont(si, eval_disp, ev_def);

ev_def:
        SS_Restore(si, si->env);
        SS_Restore(si, si->unev);}

    else
       SS_error_n(si, "CAN'T DEFINE NON-VARIABLE OBJECT - DEFINE", si->unev);

    SS_def_var(si, si->unev, si->val, si->env);
    SS_Assign(si->val, si->unev);
    _SS_end_trace(si);
    SS_go_cont(si);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PR_APPLY - jump on primitive (i.e. C level) procedure applications */

pr_apply:
    cp = SS_GET_C_PROCEDURE(si->fun);

    SS_Assign(si->val, cp->handler(si, cp, si->argl));

    SS_go_cont(si);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MACRO_UR - jump on special forms which require unevaled args
 *          - and simply return the result (e.g. define)
 */

macro_ur:
    cp = SS_GET_C_PROCEDURE(si->fun);

    _SS_bgn_trace(si, si->fun, si->unev);
    SS_Assign(si->val, cp->handler(si, cp, si->unev));

    SS_go_cont(si);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MACRO_UE - jump on special forms which require unevaled args
 *          - and eval the result (e.g. let)
 */

macro_ue:
    cp = SS_GET_C_PROCEDURE(si->fun);

    _SS_bgn_trace(si, si->fun, si->unev);
    SS_Assign(si->exn, cp->handler(si, cp, si->unev));

    SS_jump(eval_disp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EVA_LOOP - eval the args */

eva_loop:
    if (SS_nullobjp(si->unev))
       {SS_Restore(si, si->fun);
        SS_Assign(si->argl, SS_null);
        SS_jump(apply_dis);};

    SS_Save(si, si->argl);
    SS_Assign(si->exn, SS_car(si->unev));
    if (SS_nullobjp(SS_cdr(si->unev)))
       {SS_Save(si, si->this);
        SS_set_cont(si, eval_disp, acc_last);

acc_last:
        SS_Restore(si, si->this);
        SS_Restore(si, si->argl);
        SS_Restore(si, si->fun);
        SS_end_cons_macro(si->argl, si->this, si->val);
        SS_Assign(si->this, SS_null);         /* done with si->this for now */
        SS_jump(apply_dis);}
    else
       {SS_Save(si, si->unev);
        SS_Save(si, si->this);
        SS_Save(si, si->env);
        SS_set_cont(si, eval_disp, acc_arg);

acc_arg:
        SS_Restore(si, si->env);
        SS_Restore(si, si->this);
        SS_Restore(si, si->unev);
        SS_Restore(si, si->argl);
        SS_end_cons_macro(si->argl, si->this, si->val);
        SS_Assign(si->unev, SS_cdr(si->unev));
        SS_jump(eva_loop);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* APPLY_DIS - jump on primitive (C level) or compound (Scheme level)
 *           - procedures
 */

apply_dis:
    _SS_bgn_trace(si, si->fun, si->argl);

    pf  = SS_GET(procedure, si->fun);
    pty = pf->type;
    switch (pty)
       {case SS_PROC :
	     SS_jump(comp_app);

        case SS_EE_MACRO :
	     SS_jump(macro_ee);

        case SS_ESC_PROC :
	     SS_jump(pr_throw);

        case SS_PR_PROC :
	     SS_jump(pr_apply);

        default :
	     SS_error_n(si, "UNKNOWN PROCEDURE TYPE - APPLY-DISP",
			si->fun);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MACRO_EE - jump on special forms which require evaled args
 *          - and eval the result (e.g. apply)
 */

macro_ee:
    SS_Assign(si->unev, si->argl);
    SS_Assign(si->fun, SS_car(si->unev));

    pf  = SS_GET(procedure, si->fun);
    pty = pf->type;
    switch (pty)
       {case SS_PROC     :
	case SS_EE_MACRO :
	case SS_ESC_PROC :
	case SS_PR_PROC  :
	     SS_Assign(si->argl, SS_cadr(si->unev));
	     SS_jump(apply_dis);

	case SS_UE_MACRO :
        case SS_UR_MACRO :
        case SS_MACRO    :
        case SS_MACRO_EV :
        case SS_BEGIN    :
        case SS_DEFINE   :
        case SS_SET      :
        case SS_COND     :
        case SS_IF       :
        case SS_AND      :
        case SS_OR       :
	     SS_Assign(si->exn,
		       SS_mk_cons(si, si->fun, SS_cadr(si->unev)));
	     SS_jump(eval_disp);

        default :
 	     snprintf(msg, MAXLINE,
		      "UNKNOWN PROCEDURE TYPE %d (%c) - MACRO_EE",
		     pty, pty);
	     SS_error_n(si, msg, si->fun);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_MACRO - set up the evaluation of a composite macro */

ev_macro:
    _SS_bgn_trace(si, si->fun, si->unev);
    SS_Assign(si->env, SS_do_bindings(si, si->fun, si->unev));
    SS_Assign(si->unev, SS_proc_body(si->fun));
    SS_jump(ev_begin);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_MACRO_EV - set up the evaluation of a composite macro with post eval */

ev_macro_ev:
    _SS_bgn_trace(si, si->fun, si->unev);
    SS_Save(si, si->env);
    SS_Save(si, si->fun);
    SS_Assign(si->env, SS_do_bindings(si, si->fun, si->unev));
    SS_Assign(si->unev, SS_proc_body(si->fun));
    SS_set_cont(si, ev_begin, ev_macro_evb);

ev_macro_evb:
    SS_Restore(si, si->fun);
    SS_Restore(si, si->env);
    _SS_end_trace(si);
    SS_Assign(si->exn, si->val);
    SS_jump(eval_disp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMP_APP - compound apply (i.e. Scheme level) procedure applications */

comp_app:
    SS_Assign(si->env, SS_do_bindings(si, si->fun, si->argl));
    SS_Assign(si->unev, SS_proc_body(si->fun));
    SS_jump(ev_begin);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PR_THROW - the underlying return part of an escape procedure
 *          - there are a few GC issues to be addressed here
 *          - this is a first cut
 */

pr_throw:
    if (SS_ESCAPE_TYPE(si->fun) == SS_PROCEDURE_I)
       _SS_restore_state(si, si->fun);
    else
       {SS_gc(si->fun);};

/* assign the return value */
    SS_Assign(si->val, SS_car(si->argl));

    SS_go_cont(si);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_BEGIN - R3RS version of progn */

ev_begin:
    SS_Assign(si->exn, SS_car(si->unev));
    if (SS_nullobjp(SS_cdr(si->unev)))
       SS_jump(eval_disp);
    SS_Save(si, si->unev);
    SS_Save(si, si->env);
    SS_set_cont(si, eval_disp, evb_cont);

evb_cont:
    SS_Restore(si, si->env);
    SS_Restore(si, si->unev);
    SS_Assign(si->unev, SS_cdr(si->unev));
    SS_jump(ev_begin);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_AND - the and special form */

ev_and:
    if (SS_nullobjp(si->unev))
       {SS_Assign(si->val, SS_t);
        SS_go_cont(si);};

eva_iter:
    SS_Assign(si->exn, SS_car(si->unev));
    SS_Assign(si->unev, SS_cdr(si->unev));

    SS_Save(si, si->unev);
    SS_Save(si, si->env);
    SS_set_cont(si, eval_disp, eva_dec);

eva_dec:
    SS_Restore(si, si->env);
    SS_Restore(si, si->unev);
    if (!SS_true(si->val))
       SS_go_cont(si);
    else
       {if (SS_nullobjp(si->unev))
          SS_go_cont(si);
        else
          SS_jump(eva_iter);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_OR - the or special form */

ev_or:
    if (SS_nullobjp(si->unev))
       {SS_Assign(si->val, SS_f);
        SS_go_cont(si);};

    SS_Assign(si->exn, SS_car(si->unev));
    SS_Assign(si->unev, SS_cdr(si->unev));
    if (SS_nullobjp(si->exn))
       {SS_Assign(si->val, SS_f);
        SS_go_cont(si);};

    SS_Save(si, si->unev);
    SS_Save(si, si->env);
    SS_set_cont(si, eval_disp, evo_dec);

evo_dec:
    SS_Restore(si, si->env);
    SS_Restore(si, si->unev);
    if (SS_true(si->val))
       SS_go_cont(si);
    else
       SS_jump(ev_or);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_COND - cond, the conditional special form in Scheme */

ev_cond:
    if (SS_nullobjp(si->unev))
       {SS_Assign(si->val, SS_f);
        SS_go_cont(si);};

    SS_Assign(si->exn, SS_car(si->unev));
    if (SS_nullobjp(si->exn))
       {SS_Assign(si->val, SS_f);
        SS_go_cont(si);};

    SS_Assign(si->exn, SS_car(si->exn));
    SS_Save(si, si->unev);
    SS_Save(si, si->env);
    SS_set_cont(si, eval_disp, evc_dec);

evc_dec:
    SS_Restore(si, si->env);
    SS_Restore(si, si->unev);
    if (SS_true(si->val))
       {SS_Assign(si->unev, SS_cdar(si->unev));
        SS_jump(ev_begin);}
    else
       {SS_Assign(si->unev, SS_cdr(si->unev));
        SS_jump(ev_cond);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_IF - the if special form */

ev_if:
    if (SS_nullobjp(si->unev))
       SS_error_n(si, "BAD IF FORM", si->unev);

    SS_Assign(si->exn, SS_car(si->unev));
    SS_Assign(si->unev, SS_cdr(si->unev));

    SS_Save(si, si->unev);
    SS_Save(si, si->env);
    SS_set_cont(si, eval_disp, evi_dec);

evi_dec:
    SS_Restore(si, si->env);
    SS_Restore(si, si->unev);
    if (!SS_true(si->val))
       {SS_Assign(si->unev, SS_cdr(si->unev));
        if (SS_nullobjp(si->unev))
           {SS_Assign(si->val, SS_f);
            SS_go_cont(si);};};

    SS_Assign(si->exn, SS_car(si->unev));
    SS_jump(eval_disp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RET_VAL - finish up an eval */

ret_val:
/*    SS_print(si, si->outdev, si->val, "   ", "\n"); */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_IDENT - dummy function necessary to make special forms work */

static object *_SS_ident(SS_psides *si, object *obj)
   {

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_EXP_EVAL - make an explicit interpreter level call to eval */

static object *_SSI_exp_eval(SS_psides *si, object *obj)
   {object *rv;

    rv = SS_exp_eval(si, obj);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_EVAL - install the Scheme special forms */

void _SS_inst_eval(SS_psides *si)
   {

    SS_install(si, "and",
               "Special Form: (and <form> ... <form>), evaluates forms until FALSE is returned",
               SS_sargs,
               _SS_ident, SS_AND);

    SS_install(si, "apply",
               "Special Form: cons the procedure onto the args and eval the result",
               SS_sargs,
               _SS_ident, SS_EE_MACRO);

    SS_install(si, "begin",
               "Special Form: Evaluate a list of forms and returns the value of the last one",
               SS_sargs,
               _SS_ident, SS_BEGIN);

    SS_install(si, "cond",
               "Special Form: (cond (<clause1>) ... (<clausen>))",
               SS_sargs,
               _SS_ident, SS_COND);

    SS_install(si, "define",
               "Special Form: defines variables and procedures in the current environment",
               SS_sargs,
               _SS_ident, SS_DEFINE);

    SS_install(si, "define-macro",
               "Special Form: defines special forms in the current environment",
               SS_sargs,
               _SS_ident, SS_DEFINE);

    SS_install(si, "define-macro-ev",
               "Special Form: defines special forms which are post-evaluated in the current environment",
               SS_sargs,
               _SS_ident, SS_DEFINE);

    SS_install(si, "eval",
               "Procedure: Evaluates the given form and returns the value",
               SS_sargs,
               _SSI_exp_eval, SS_PR_PROC);

    SS_install(si, "if",
               "Special Form: (if <pred> <consequent> <alternate>)",
               SS_sargs,
               _SS_ident, SS_IF);

    SS_install(si, "or",
               "Special Form: (or <form> ... <form>), evaluates forms until TRUE is returned",
               SS_sargs,
               _SS_ident, SS_OR);

    SS_install(si, "set!",
               "Special Form: binds variable to value in current environment",
               SS_sargs,
               _SS_ident, SS_SET);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

