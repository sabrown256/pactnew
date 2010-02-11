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

/* SS_SAVE - save the object on the Scheme stack */

#ifdef STACK_FNC

void SS_Save(object *obj)
   {

    SS_nsave++;
    SS_MARK(obj);

    SC_array_push(SS_stack, &obj);

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RESTORE - pop an object off the stack and release the stacken */

#ifdef STACK_FNC

void _SS_Restore(object **px)
   {object *x;

    x = *px;

    SS_nrestore++;
    SS_GC(x);

    *px = *(object **) SC_array_pop(SS_stack);

    return;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_EVAL - entry point at C level for eval */

void _SS_eval(void)
   {char msg[MAXLINE];
    char *s;
    C_procedure *cp;
    procedure *pf, *pg;
    SS_form pty;
    PFPHand ph;
    PFVoid  pp;
    
    SS_set_cont(eval_disp, ret_val);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EVAL_DISP - eval-dispatch the real starting point for the evaluator */

eval_disp:
    switch (SS_Exn->eval_type)
       {case NO_EV :
             SS_error("ILLEGAL EVALUATION TYPE", SS_Exn);

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
    SS_Assign(SS_Val, SS_Exn);
    SS_go_cont;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VAR_EV - evaluate variables */

var_ev:
    SS_Assign(SS_Val, SS_lk_var_val(SS_Exn, SS_Env));
    SS_go_cont;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PROC_EV - evaluate a procedure/list object */

proc_ev:
    SS_Assign(SS_Unev, SS_cdr(SS_Exn));
    SS_Assign(SS_Exn, SS_car(SS_Exn));
    SS_Save(SS_Unev);
    SS_Save(SS_Env);
    SS_set_cont(eval_disp, ev_args);

ev_args:
    SS_Restore(SS_Env);
    SS_Restore(SS_Unev);
    SS_Assign(SS_Fun, SS_Val);
    if (!SS_procedurep(SS_Fun))
       SS_error("ILLEGAL PROCEDURE OBJECT", SS_Fun);

    pf  = SS_GET(procedure, SS_Fun);
    pty = pf->type;
    switch (pty)
       {case SS_ESC_PROC :

/* procedure with evaluated args and simple return */
        case SS_PROC     : 
        case SS_PR_PROC  :

/* procedure with evaluated args and post-evaluated results */
        case SS_EE_MACRO :
	     SS_Save(SS_Fun);
	     SS_Assign(SS_This, SS_null);
	     SS_Assign(SS_Argl, SS_null);
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
	     SS_error("BAD PROCEDURE TYPE", SS_Fun);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET - set! macro in Scheme */

ev_set:
    SS_Assign(SS_Exn, SS_cadr(SS_Unev));
    SS_Assign(SS_Unev, SS_car(SS_Unev));

    if (!SS_variablep(SS_Unev))
       SS_error("CAN'T SET NON-VARIABLE OBJECT - SET", SS_Unev);

    SS_Save(SS_Unev);
    SS_Save(SS_Env);
    SS_set_cont(eval_disp, ev_seta);

ev_seta:
    SS_Restore(SS_Env);
    SS_Restore(SS_Unev);
    SS_set_var(SS_Unev, SS_Val, SS_Env);
    SS_Assign(SS_Val, SS_Unev);
    SS_go_cont;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_DEFINE - define macro in Scheme */

ev_define:
    SS_bgn_trace(SS_Fun, SS_Unev);
    SS_Assign(SS_Exn, SS_cdr(SS_Unev));
    SS_Assign(SS_Unev, SS_car(SS_Unev));

/* define a compound procedure */
    if (SS_consp(SS_Unev))
       {SS_Assign(SS_Exn, SS_mk_cons(SS_cdr(SS_Unev), SS_Exn));
        SS_Assign(SS_Unev, SS_car(SS_Unev));
        SS_Assign(SS_Val, SS_mk_procedure(SS_Unev, SS_Exn, SS_Env));

	pf = SS_GET(procedure, SS_Fun);
	pg = SS_GET(procedure, SS_Val);

        if (strcmp(pf->name, "define-macro") == 0)
           pg->type = SS_MACRO;

        else if (strcmp(pf->name, "define-macro-ev") == 0)
           pg->type = SS_MACRO_EV;

        s = SS_VARIABLE_NAME(SS_Unev);

        SFREE(pg->name);
        pg->name = SC_strsavef(s, "char*:SS_DEFINE:name");}

/* define a variable */
    else if (SS_variablep(SS_Unev))
       {SS_Save(SS_Unev);
        SS_Save(SS_Env);
        SS_Assign(SS_Exn, SS_car(SS_Exn));
        SS_set_cont(eval_disp, ev_def);

ev_def:
        SS_Restore(SS_Env);
        SS_Restore(SS_Unev);}

    else
       SS_error("CAN'T DEFINE NON-VARIABLE OBJECT - DEFINE", SS_Unev);

    SS_def_var(SS_Unev, SS_Val, SS_Env);
    SS_Assign(SS_Val, SS_Unev);
    SS_end_trace();
    SS_go_cont;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PR_APPLY - jump on primitive (i.e. C level) procedure applications */

pr_apply:
    cp = SS_GET_C_PROCEDURE(SS_Fun);
    ph = cp->handler;
    pp = cp->proc;

    SS_Assign(SS_Val, (*ph)(pp, SS_Argl));

    SS_go_cont;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MACRO_UR - jump on special forms which require unevaled args
 *          - and simply return the result (e.g. define)
 */

macro_ur:
    cp = SS_GET_C_PROCEDURE(SS_Fun);
    ph = cp->handler;
    pp = cp->proc;

    SS_bgn_trace(SS_Fun, SS_Unev);
    SS_Assign(SS_Val, (*ph)(pp, SS_Unev));

    SS_go_cont;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MACRO_UE - jump on special forms which require unevaled args
 *          - and eval the result (e.g. let)
 */

macro_ue:
    cp = SS_GET_C_PROCEDURE(SS_Fun);
    ph = cp->handler;
    pp = cp->proc;

    SS_bgn_trace(SS_Fun, SS_Unev);
    SS_Assign(SS_Exn, (*ph)(pp, SS_Unev));

    SS_jump(eval_disp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EVA_LOOP - eval the args */

eva_loop:
    if (SS_nullobjp(SS_Unev))
       {SS_Restore(SS_Fun);
        SS_Assign(SS_Argl, SS_null);
        SS_jump(apply_dis);};

    SS_Save(SS_Argl);
    SS_Assign(SS_Exn, SS_car(SS_Unev));
    if (SS_nullobjp(SS_cdr(SS_Unev)))
       {SS_Save(SS_This);
        SS_set_cont(eval_disp, acc_last);

acc_last:
        SS_Restore(SS_This);
        SS_Restore(SS_Argl);
        SS_Restore(SS_Fun);
        SS_end_cons_macro(SS_Argl, SS_This, SS_Val);
        SS_Assign(SS_This, SS_null);            /* done with SS_This for now */
        SS_jump(apply_dis);}
    else
       {SS_Save(SS_Unev);
        SS_Save(SS_This);
        SS_Save(SS_Env);
        SS_set_cont(eval_disp, acc_arg);

acc_arg:
        SS_Restore(SS_Env);
        SS_Restore(SS_This);
        SS_Restore(SS_Unev);
        SS_Restore(SS_Argl);
        SS_end_cons_macro(SS_Argl, SS_This, SS_Val);
        SS_Assign(SS_Unev, SS_cdr(SS_Unev));
        SS_jump(eva_loop);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* APPLY_DIS - jump on primitive (C level) or compound (Scheme level)
 *           - procedures
 */

apply_dis:
    SS_bgn_trace(SS_Fun, SS_Argl);

    pf  = SS_GET(procedure, SS_Fun);
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
	     SS_error("UNKNOWN PROCEDURE TYPE - APPLY-DISP",
		      SS_Fun);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MACRO_EE - jump on special forms which require evaled args
 *          - and eval the result (e.g. apply)
 */

macro_ee:
    SS_Assign(SS_Unev, SS_Argl);
    SS_Assign(SS_Fun, SS_car(SS_Unev));

    pf  = SS_GET(procedure, SS_Fun);
    pty = pf->type;
    switch (pty)
       {case SS_PROC     :
	case SS_EE_MACRO :
	case SS_ESC_PROC :
	case SS_PR_PROC  :
	     SS_Assign(SS_Argl, SS_cadr(SS_Unev));
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
	     SS_Assign(SS_Exn,
		       SS_mk_cons(SS_Fun, SS_cadr(SS_Unev)));
	     SS_jump(eval_disp);

        default :
 	     snprintf(msg, MAXLINE,
		      "UNKNOWN PROCEDURE TYPE %d (%c) - MACRO_EE",
		     pty, pty);
	     SS_error(msg, SS_Fun);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_MACRO - set up the evaluation of a composite macro */

ev_macro:
    SS_bgn_trace(SS_Fun, SS_Unev);
    SS_Assign(SS_Env, SS_do_bindings(SS_Fun, SS_Unev));
    SS_Assign(SS_Unev, SS_proc_body(SS_Fun));
    SS_jump(ev_begin);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_MACRO_EV - set up the evaluation of a composite macro with post eval */

ev_macro_ev:
    SS_bgn_trace(SS_Fun, SS_Unev);
    SS_Save(SS_Env);
    SS_Save(SS_Fun);
    SS_Assign(SS_Env, SS_do_bindings(SS_Fun, SS_Unev));
    SS_Assign(SS_Unev, SS_proc_body(SS_Fun));
    SS_set_cont(ev_begin, ev_macro_evb);

ev_macro_evb:
    SS_Restore(SS_Fun);
    SS_Restore(SS_Env);
    SS_end_trace();
    SS_Assign(SS_Exn, SS_Val);
    SS_jump(eval_disp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMP_APP - compound apply (i.e. Scheme level) procedure applications */

comp_app:
    SS_Assign(SS_Env, SS_do_bindings(SS_Fun, SS_Argl));
    SS_Assign(SS_Unev, SS_proc_body(SS_Fun));
    SS_jump(ev_begin);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PR_THROW - the underlying return part of an escape procedure
 *          - there are a few GC issues to be addressed here
 *          - this is a first cut
 */

pr_throw:
    if (SS_ESCAPE_TYPE(SS_Fun) == SS_PROCEDURE_I)
       _SS_restore_state(SS_Fun);
    else
       {SS_GC(SS_Fun);};

/* assign the return value */
    SS_Assign(SS_Val, SS_car(SS_Argl));

    SS_go_cont;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_BEGIN - R3RS version of progn */

ev_begin:
    SS_Assign(SS_Exn, SS_car(SS_Unev));
    if (SS_nullobjp(SS_cdr(SS_Unev)))
       SS_jump(eval_disp);
    SS_Save(SS_Unev);
    SS_Save(SS_Env);
    SS_set_cont(eval_disp, evb_cont);

evb_cont:
    SS_Restore(SS_Env);
    SS_Restore(SS_Unev);
    SS_Assign(SS_Unev, SS_cdr(SS_Unev));
    SS_jump(ev_begin);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_AND - the and special form */

ev_and:
    if (SS_nullobjp(SS_Unev))
       {SS_Assign(SS_Val, SS_t);
        SS_go_cont;};

eva_iter:
    SS_Assign(SS_Exn, SS_car(SS_Unev));
    SS_Assign(SS_Unev, SS_cdr(SS_Unev));

    SS_Save(SS_Unev);
    SS_Save(SS_Env);
    SS_set_cont(eval_disp, eva_dec);

eva_dec:
    SS_Restore(SS_Env);
    SS_Restore(SS_Unev);
    if (!SS_true(SS_Val))
       SS_go_cont;
    else
       {if (SS_nullobjp(SS_Unev))
          SS_go_cont;
        else
          SS_jump(eva_iter);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_OR - the or special form */

ev_or:
    if (SS_nullobjp(SS_Unev))
       {SS_Assign(SS_Val, SS_f);
        SS_go_cont;};

    SS_Assign(SS_Exn, SS_car(SS_Unev));
    SS_Assign(SS_Unev, SS_cdr(SS_Unev));
    if (SS_nullobjp(SS_Exn))
       {SS_Assign(SS_Val, SS_f);
        SS_go_cont;};

    SS_Save(SS_Unev);
    SS_Save(SS_Env);
    SS_set_cont(eval_disp, evo_dec);

evo_dec:
    SS_Restore(SS_Env);
    SS_Restore(SS_Unev);
    if (SS_true(SS_Val))
       SS_go_cont;
    else
       SS_jump(ev_or);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_COND - cond, the conditional special form in Scheme */

ev_cond:
    if (SS_nullobjp(SS_Unev))
       {SS_Assign(SS_Val, SS_f);
        SS_go_cont;};

    SS_Assign(SS_Exn, SS_car(SS_Unev));
    if (SS_nullobjp(SS_Exn))
       {SS_Assign(SS_Val, SS_f);
        SS_go_cont;};

    SS_Assign(SS_Exn, SS_car(SS_Exn));
    SS_Save(SS_Unev);
    SS_Save(SS_Env);
    SS_set_cont(eval_disp, evc_dec);

evc_dec:
    SS_Restore(SS_Env);
    SS_Restore(SS_Unev);
    if (SS_true(SS_Val))
       {SS_Assign(SS_Unev, SS_cdar(SS_Unev));
        SS_jump(ev_begin);}
    else
       {SS_Assign(SS_Unev, SS_cdr(SS_Unev));
        SS_jump(ev_cond);};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EV_IF - the if special form */

ev_if:
    if (SS_nullobjp(SS_Unev))
       SS_error("BAD IF FORM", SS_Unev);

    SS_Assign(SS_Exn, SS_car(SS_Unev));
    SS_Assign(SS_Unev, SS_cdr(SS_Unev));

    SS_Save(SS_Unev);
    SS_Save(SS_Env);
    SS_set_cont(eval_disp, evi_dec);

evi_dec:
    SS_Restore(SS_Env);
    SS_Restore(SS_Unev);
    if (!SS_true(SS_Val))
       {SS_Assign(SS_Unev, SS_cdr(SS_Unev));
        if (SS_nullobjp(SS_Unev))
           {SS_Assign(SS_Val, SS_f);
            SS_go_cont;};};

    SS_Assign(SS_Exn, SS_car(SS_Unev));
    SS_jump(eval_disp);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RET_VAL - finish up an eval */

ret_val:
/*    SS_print(SS_Val, "   ", "\n", SS_outdev); */
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_IDENT - dummy function necessary to make special forms work */

static object *_SS_ident(object *obj)
   {

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INST_EVAL - install the Scheme special forms */

void _SS_inst_eval(void)
   {

    SS_install("and",
               "Special Form: (and <form> ... <form>), evaluates forms until FALSE is returned",
               SS_sargs,
               _SS_ident, SS_AND);

    SS_install("apply",
               "Special Form: cons the procedure onto the args and eval the result",
               SS_sargs,
               _SS_ident, SS_EE_MACRO);

    SS_install("begin",
               "Special Form: Evaluate a list of forms and returns the value of the last one",
               SS_sargs,
               _SS_ident, SS_BEGIN);

    SS_install("cond",
               "Special Form: (cond (<clause1>) ... (<clausen>))",
               SS_sargs,
               _SS_ident, SS_COND);

    SS_install("define",
               "Special Form: defines variables and procedures in the current environment",
               SS_sargs,
               _SS_ident, SS_DEFINE);

    SS_install("define-macro",
               "Special Form: defines special forms in the current environment",
               SS_sargs,
               _SS_ident, SS_DEFINE);

    SS_install("define-macro-ev",
               "Special Form: defines special forms which are post-evaluated in the current environment",
               SS_sargs,
               _SS_ident, SS_DEFINE);

    SS_install("eval",
               "Procedure: Evaluates the given form and returns the value",
               SS_sargs,
               SS_exp_eval, SS_PR_PROC);

    SS_install("if",
               "Special Form: (if <pred> <consequent> <alternate>)",
               SS_sargs,
               _SS_ident, SS_IF);

    SS_install("or",
               "Special Form: (or <form> ... <form>), evaluates forms until TRUE is returned",
               SS_sargs,
               _SS_ident, SS_OR);

    SS_install("set!",
               "Special Form: binds variable to value in current environment",
               SS_sargs,
               _SS_ident, SS_SET);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

