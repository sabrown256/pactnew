/*
 * SHMSUP.C - support routines for BASIS language syntax mode
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#define EXTERN_VAR

#include "shm.h"

SS_m_parse_state
 _SS_mps = { FALSE, FALSE };

/*--------------------------------------------------------------------------*/

/*                            GRAMMAR SUPPORT                               */

/*--------------------------------------------------------------------------*/

/* _SS_MAKE_FUN_M - build the best function possible from the given
 *                - prototype and body
 */

object *_SS_make_fun_m(object *proto, object *body)
   {object *o;

    o = SS_append(SS_make_form(_SS_m_defunc, proto, 0), body);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAKE_COMP_STMT_M - turn a list of statments into 
 *                      - a (begin ... ) form
 */

object *_SS_make_comp_stmt_m(object *body)
   {object *cmp;

    if (SS_consp(body) && SS_consp(SS_CAR_MACRO(body)))
       cmp = SS_mk_cons(_SS_m_block, SS_reverse(body));
    else
       cmp = body;

    return(cmp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_STRIP_CALL - strip (handle-m-arr-call x (a1 a2 ...)) form down to
 *                - (x a1 a2 ...)
 */

object *_SS_strip_call(object *expr, int paren)
   {object *frm, *xpr, *fnc, *args;

    fnc = SS_CAR_MACRO(expr);
    if (fnc == _SS_m_arr_call)
       {xpr = SS_CDR_MACRO(expr);
	if (SS_consp(xpr))
	   {fnc  = SS_CAR_MACRO(xpr);
	    args = SS_CAR_MACRO(SS_CDR_MACRO(xpr));
	    frm  = SS_mk_cons(fnc, args);}
	else
	   frm = SS_mk_cons(xpr, SS_null);}

    else if (paren)
       frm = SS_mk_cons(expr, SS_null);

    else
       frm = expr;

    return(frm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LOOKUP_IDENTIFIER_M - lookup an identifier for LEX */

int SS_lookup_identifier_m(char *txt, object **lval)
   {int type;
    object *o;
    SS_psides *si;

    si = &_SS_si;

    *lval = SS_f;

    o = (object *) SC_hasharr_def_lookup(si->types, txt);
    if (o != NULL)
       {*lval = o;
	type  = SS_m_tokens[0];}

    else
       {*lval = SS_add_variable(txt);

/* group names begin with upper case letter */
	if (isupper((int) *txt))
	   type  = SS_m_tokens[2];

/* identifier names don't */
        else
	   type  = SS_m_tokens[1];};

     return(type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SYNTAX_M - BASIS -> SCHEME syntax translation parser */

object *SS_syntax_m(object *str)
   {object *ret;
    SS_psides *si;

    si = &_SS_si;

    if (SETJMP(si->cpu))
       ret = SS_eof;

    else
       {si->character_stream = str;

	for (ret = SS_null; ret == SS_null; )
	    {shgrm_parse();
	     ret = SS_parse_token_val_m();};};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_M_MODE - switch to BASIS syntax parsing */

static object *SS_m_mode(SS_psides *si)
   {

    snprintf(si->prompt, MAXLINE, "M-> ");
    si->read        = SS_syntax_m;
    si->name_reproc = SS_name_map_synt;

    SS_set_parser(SS_m_mode);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INIT_M_SYNTAX_MODE - setup for parsing BASIS syntax */

void SS_init_m_syntax_mode(void)
   {int *ssdbg;
    SS_psides *si;

    si = &_SS_si;

    if (_SS_mps.mode_init == FALSE)
       {_SS_mps.mode_init = TRUE;

	SS_load_scm("msynt.scm");

	_SS_m_defunc    = SS_lookup_variable("m/defun", TRUE);
	_SS_m_defvar    = SS_lookup_variable("m/scalar", TRUE);
	_SS_m_defarr    = SS_lookup_variable("m/array", TRUE);
	_SS_m_set       = SS_lookup_variable("m/set!", TRUE);
	_SS_m_loop_test = SS_lookup_variable("m/loop", TRUE);
	_SS_m_arr_call  = SS_lookup_variable("m/ac", TRUE);
	_SS_m_list      = SS_lookup_variable("list", TRUE);
	_SS_m_block     = SS_lookup_variable("begin", TRUE);
	_SS_m_if        = SS_lookup_variable("if", TRUE);
	_SS_m_for       = SS_lookup_variable("for", TRUE);
	_SS_m_while     = SS_lookup_variable("while", TRUE);
	_SS_m_expt      = SS_lookup_variable("expt", TRUE);
	_SS_m_plus      = SS_lookup_variable("+", TRUE);
	_SS_m_minus     = SS_lookup_variable("-", TRUE);
	_SS_m_times     = SS_lookup_variable("*", TRUE);
	_SS_m_divide    = SS_lookup_variable("/", TRUE);
	_SS_m_modulo    = SS_lookup_variable("quotient", TRUE);
	_SS_m_bitand    = SS_lookup_variable("&", TRUE);
	_SS_m_bitor     = SS_lookup_variable("|", TRUE);
	_SS_m_bitcmp    = SS_lookup_variable("~", TRUE);
	_SS_m_not       = SS_lookup_variable("not", TRUE);
	_SS_m_and       = SS_lookup_variable("and", TRUE);
	_SS_m_or        = SS_lookup_variable("or", TRUE);
	_SS_m_equal     = SS_lookup_variable("=", TRUE);
	_SS_m_lt        = SS_lookup_variable("<", TRUE);
	_SS_m_gt        = SS_lookup_variable(">", TRUE);
	_SS_m_le        = SS_lookup_variable("<=", TRUE);
        _SS_m_ge        = SS_lookup_variable(">=", TRUE);
	_SS_m_label     = SS_lookup_variable("m/label", TRUE);
	_SS_m_load      = SS_lookup_variable("ld", TRUE);
	_SS_m_output    = SS_lookup_variable("m/<<", TRUE);
	_SS_m_input     = SS_lookup_variable("m/>>", TRUE);

	_SS_m_return    = SS_lookup_variable("-return-", TRUE);
	_SS_m_continue  = SS_lookup_variable("-continue-", TRUE);

	SS_add_type_synt("integer");
	SS_add_type_synt("real");
	SS_add_type_synt("logical");
	SS_add_type_synt("complex");
	SS_add_type_synt("double complex");
	SS_add_type_synt("double");
	SS_add_type_synt("chameleon");
	SS_add_type_synt("character*");
	SS_add_type_synt("character");
	SS_add_type_synt("range");
	SS_add_type_synt("real8");
	SS_add_type_synt("complex8");
	SS_add_type_synt("indirect");
	SS_add_type_synt("double precision");

	SS_add_parser(".m", SS_m_mode);
	SS_add_parser(".v", SS_m_mode);

	si->eox                   = TRUE;
	_SS_mps.diagnose_grammar = FALSE;

	ssdbg  = SS_parse_debug_m();
	*ssdbg = FALSE;

	SS_install("m-mode",
		   "Procedure: set BASIS language parsing mode",
		   SS_zargs,
		   SS_m_mode, SS_PR_PROC);

	SS_install_cf("diagnose-m-parse", 
		      "Variable: Flag to print detailed diagnostics of a BASIS parse\n     Usage: diagnose-m-parse [0|1]",
		      SS_acc_int,
		      ssdbg);
	SS_install_cf("diagnose-m-grammar", 
		      "Variable: Flag to print diagnostics of BASIS actions/reductions\n     Usage: diagnose-m-parse [0|1]",
		      SS_acc_int,
		      &_SS_mps.diagnose_grammar);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
