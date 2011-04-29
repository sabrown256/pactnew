/*
 * SHFSUP.C - support routines for Fortran syntax mode
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#define EXTERN_VAR

#include "shf.h"

/*--------------------------------------------------------------------------*/

/*                            GRAMMAR SUPPORT                               */

/*--------------------------------------------------------------------------*/

/* _SS_MAKE_FUNF - build the best function possible from the given
 *               - prototype and body
 */

object *_SS_make_funf(object *proto, object *body)
   {int extra;
    object *rest, *o;

    rest = SS_null;
    extra = FALSE;
    if (SS_consp(body))
       {if (SS_CAR_MACRO(body) == _SS_f_block)
           {rest  = SS_CDR_MACRO(body);
	    extra = (SS_consp(rest) &&
		     (SS_CAR_MACRO(rest) == SS_null));};};
            
    if (extra)
       {body = SS_CDR_MACRO(rest);
	o = SS_append(SS_make_form(_SS_f_defunc, proto, 0),
		      body);}
    else
       o = SS_make_form(_SS_f_defunc, proto, body, 0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LOOKUP_IDENTIFIER_F - lookup an identifier for LEX */

int SS_lookup_identifier_f(char *txt, object **lval)
   {int type;
    object *o;
    SS_psides *si;

    si = &_SS_si;

    *lval = SS_f;

    o = (object *) SC_hasharr_def_lookup(si->types, txt);
    if (o != NULL)
       {*lval = o;
	type  = SS_f_tokens[0];}

    else
       {*lval = SS_add_variable(txt);
	type  = SS_f_tokens[1];};

     return(type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SYNTAX_F - Fortran -> SCHEME syntax translation parser */

object *SS_syntax_f(object *str)
   {object *ret;
    SS_psides *si;

    si = &_SS_si;

    if (SETJMP(si->cpu))
       ret = SS_eof;

    else
       {si->character_stream = str;

	shgrf_parse();

	ret = SS_parse_token_val_f();};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_F_MODE - switch to Fortran syntax parsing */

static object *SS_f_mode(void)
   {SS_psides *si;

    si = &_SS_si;

    snprintf(si->prompt, MAXLINE, "F-> ");
    si->read        = SS_syntax_f;
    si->name_reproc = SS_name_map_synt;

    SS_set_parser(SS_f_mode);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INIT_F_SYNTAX_MODE - setup for parsing Fortran syntax */

void SS_init_f_syntax_mode(void)
   {int *ssdbg;
    SS_psides *si;

    si = &_SS_si;

    if (_SS.f_mode_init == FALSE)
       {_SS.f_mode_init = TRUE;

	SS_load_scm("fsynt.scm");

	_SS_f_defunc    = SS_lookup_variable("handle-f-function-declaration", TRUE);
	_SS_f_defvar    = SS_lookup_variable("handle-f-scalar-declaration", TRUE);
	_SS_f_defarr    = SS_lookup_variable("handle-f-array-declaration", TRUE);
	_SS_f_set       = SS_lookup_variable("handle-f-assignment", TRUE);
	_SS_f_block     = SS_lookup_variable("let*", TRUE);
	_SS_f_if        = SS_lookup_variable("if", TRUE);
	_SS_f_for       = SS_lookup_variable("for", TRUE);
	_SS_f_while     = SS_lookup_variable("while", TRUE);
	_SS_f_expt      = SS_lookup_variable("expt", TRUE);
	_SS_f_plus      = SS_lookup_variable("+", TRUE);
	_SS_f_minus     = SS_lookup_variable("-", TRUE);
	_SS_f_times     = SS_lookup_variable("*", TRUE);
	_SS_f_divide    = SS_lookup_variable("/", TRUE);
	_SS_f_modulo    = SS_lookup_variable("quotient", TRUE);
	_SS_f_bitand    = SS_lookup_variable("&", TRUE);
	_SS_f_bitor     = SS_lookup_variable("|", TRUE);
	_SS_f_bitcmp    = SS_lookup_variable("~", TRUE);
	_SS_f_lshft     = SS_lookup_variable("<<", TRUE);
	_SS_f_rshft     = SS_lookup_variable(">>", TRUE);
	_SS_f_not       = SS_lookup_variable(".not.", TRUE);
	_SS_f_and       = SS_lookup_variable(".and.", TRUE);
	_SS_f_or        = SS_lookup_variable(".or.", TRUE);
	_SS_f_equal     = SS_lookup_variable("=", TRUE);
	_SS_f_lt        = SS_lookup_variable(".lt.", TRUE);
	_SS_f_gt        = SS_lookup_variable(".gt.", TRUE);
	_SS_f_le        = SS_lookup_variable(".le.", TRUE);
        _SS_f_ge        = SS_lookup_variable(".ge.", TRUE);
	_SS_f_declare   = SS_lookup_variable("handle-f-declare", TRUE);
	_SS_f_aref      = SS_lookup_variable("handle-f-array-ref", TRUE);
	_SS_f_mref      = SS_lookup_variable("handle-f-member-ref", TRUE);
	_SS_f_preinc    = SS_lookup_variable("handle-f-pre-inc", TRUE);
	_SS_f_predec    = SS_lookup_variable("handle-f-pre-dec", TRUE);
	_SS_f_postinc   = SS_lookup_variable("handle-f-post-inc", TRUE);
	_SS_f_postdec   = SS_lookup_variable("handle-f-post-dec", TRUE);
	_SS_f_cond      = SS_lookup_variable("handle-f-switch", TRUE);
	_SS_f_label     = SS_lookup_variable("handle-f-label", TRUE);
	_SS_f_sizeof    = SS_lookup_variable("handle-f-sizeof", TRUE);

	_SS_f_return    = SS_lookup_variable("-return-", TRUE);
	_SS_f_continue  = SS_lookup_variable("-continue-", TRUE);

	SS_add_type_synt("integer");
	SS_add_type_synt("real");
	SS_add_type_synt("double precision");

	SS_add_parser(".f", (PFPObject) SS_f_mode);

	si->eox          = TRUE;
	SS_diagnostic_f = FALSE;

	ssdbg  = SS_parse_debug_f();
	*ssdbg = FALSE;

	SS_install("f-mode",
		   "Procedure: set Fortran syntax parsing mode",
		   SS_zargs,
		   SS_f_mode, SS_PR_PROC);

	SS_install_cf("diagnose-f-parse", 
		      "Variable: Flag to print detailed diagnostics of a Fortran parse\n     Usage: diagnose-f-parse [0|1]",
		      SS_acc_int,
		      ssdbg);
	SS_install_cf("diagnose-f-grammar", 
		      "Variable: Flag to print diagnostics of Fortran actions/reductions\n     Usage: diagnose-f-parse [0|1]",
		      SS_acc_int,
		      &SS_diagnostic_f);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
