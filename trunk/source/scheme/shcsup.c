/*
 * SHCSUP.C - support routines for C syntax mode
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#define EXTERN_VAR

#include "shc.h"

SS_c_parse_state
 _SS_cps = { FALSE, FALSE, FALSE, FALSE };

/* figure out what to do with a newline
 * in general newlines are ignored
 * however if attempting to interpret preprocessing
 * directives they are necessary to render the
 * the grammar unambiguous
 * for example:
 * 
 *   #define FOO
 *   int x;
 * 
 * and
 * 
 *   #define FOO 3
 * 
 * provide a shift/reduce conflict as the grammar is
 * currently defined
 * no sense can be made of this case unlike the if/else
 * shift/reduce conflict
 *
 * use cpp_define member of SS_c_parse_state as control
 * on the parse to resolve the conflict
 */

SS_psides
 *_SS_isc;

/*--------------------------------------------------------------------------*/

/*                            GRAMMAR SUPPORT                               */

/*--------------------------------------------------------------------------*/

/* _SS_MAKE_FUNC - build the best function possible from the given
 *               - prototype and body
 */

object *_SS_make_func(SS_psides *si, object *proto, object *body)
   {int extra;
    object *rest, *frm;

    rest  = SS_null;
    extra = FALSE;
    if (SS_consp(body))
       {if (SS_CAR_MACRO(body) == _SS_c_block)
           {rest  = SS_CDR_MACRO(body);
	    extra = (SS_consp(rest) &&
		     (SS_CAR_MACRO(rest) == SS_null));};};
            
    if (extra)
       {body = SS_CDR_MACRO(rest);
        frm  = SS_append(si,
			 SS_make_form(si, _SS_c_defunc, proto, LAST),
			 body);}
    else
       frm = SS_make_form(si, _SS_c_defunc, proto, body, LAST);

    return(frm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAKE_MACR - build the best macro possible from the given
 *               - prototype and body
 *               - NOTE: this is for CPP extention #define
 */

object *_SS_make_macr(SS_psides *si, object *proto, object *body)
   {object *frm;

    if (SS_variablep(proto) && (proto == body))
       frm = SS_make_form(si, _SS_c_defmac, proto, SS_null, LAST);

    else
       frm = SS_make_form(si, _SS_c_defmac, proto, body, LAST);

    return(frm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAKE_STMNT_LST - build up a list of statements
 *                    - by adding statement S to the statement list SL
 */

object *_SS_make_stmnt_lst(SS_psides *si, object *s, object *sl)
   {object *frm;

    if (SS_nullobjp(s))
       frm = sl;

    else if (SS_consp(SS_CAR_MACRO(s)))
       frm = SS_append(si, s, sl);

    else
       frm = SS_mk_cons(si, s, sl);

    return(frm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAKE_CMPND_STMNT - build a compound statement
 *                      - from a list of declarations and
 *                      - a list of statements
 */

object *_SS_make_cmpnd_stmnt(SS_psides *si, object *dcl, object *sl)
   {object *frm, *bdy, *d, *dcls, *lst, *proto;

    if (SS_nullobjp(sl))
       dcls = SS_cdr(dcl);

    else
       {dcls = SS_null;
	for (lst = dcl; !SS_nullobjp(lst); lst = SS_cdr(lst))
	    {d    = SS_cdar(lst);
	     dcls = SS_append(si, dcls, d);};

        dcls = SS_reverse(si, dcls);};

    bdy   = SS_reverse(si, sl);
    proto = SS_make_form(si, _SS_c_block, dcls, LAST);
    frm   = SS_append(si, proto, bdy);

    return(frm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_DEL_VAR - delete or undefine a variable
 *             - NOTE: this is for CPP extention #undef
 */

object *_SS_del_var(SS_psides *si, object *var)
   {char *name;

    name = SS_VARIABLE_NAME(var);

    _SS_rem_varc(si, name, si->env);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAKE_DECL - make a declaration
 *               - from the 'declaration' production
 *               -   DS  the declaration_specifiers
 *               -       this is a SCHEME declaration structure
 *               -   IDL the init_declarator_list
 *               -       these are of the form (<var> <init>) | <var>
 */

object *_SS_make_decl(SS_psides *si, object *ds, object *idl)
   {object *decl;

    decl = SS_mk_cons(si, ds, idl);

    return(decl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAKE_MAC_DECL - make a macro declaration
 *                   - from the 'macro_declarator' production
 *                   -   DS  the macro_declarator
 *                   -   IDL the identifier_list
 */

object *_SS_make_mac_decl(SS_psides *si, object *ds, object *idl)
   {object *f, *args, *decl;

    args = SS_reverse(si, idl);
    if (SS_consp(ds))
       {f = SS_CAR_MACRO(ds);
	if (SS_consp(f))
	   decl = SS_mk_cons(si, SS_append(si, f, args), SS_null);
	else
	   decl = SS_append(si, ds, args);}
    else
       decl = SS_mk_cons(si, ds, args);

    return(decl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MAKE_CAST - make a cast from TYPE and EXPR */

object *_SS_make_cast(SS_psides *si, object *type, object *expr)
   {char tname[MAXLINE];
    char *t, *p;
    object *typ, *cast;

    SS_args(si, type,
	    SC_STRING_I, &t,
	    LAST);
    strncpy(tname, t, MAXLINE);
    p = strstr(tname, "define-");
    t = (p != NULL) ? p + 7 : tname;
       
    typ  = SS_mk_string(si, t);
    cast = SS_make_form(si, _SS_c_cast, typ, expr, LAST);

    return(cast);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LOOKUP_IDENTIFIER_C - lookup an identifier for LEX */

int SS_lookup_identifier_c(SS_psides *si, char *txt, object **lval)
   {int type;
    object *o;

    *lval = SS_f;

    o = (object *) SC_hasharr_def_lookup(si->types, txt);
    if (o != NULL)
       {*lval = o;
	type  = SS_c_tokens[0];}

    else if (txt[strlen(txt)-1] == '*')
       {*lval = SS_mk_string(si, txt);
	type  = SS_c_tokens[0];}

    else
       {*lval = SS_add_variable(si, txt);
	type  = SS_c_tokens[1];};

    return(type);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SYNTAX_C - C -> SCHEME syntax translation parser */

object *SS_syntax_c(SS_psides *si, object *str)
   {char *s;
    object *ret;

    s = SS_BUFFER(str);
    SC_ASSERT(s != NULL);

    if (SETJMP(si->cpu))
       ret = SS_eof;

    else
       {si->character_stream   = str;
	_SS_cps.cpp_directive = FALSE;
       
	_SS_isc = si;

	shgrc_parse();

	ret = SS_parse_token_val_c();};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_C_MODE - switch to C syntax parsing */

static object *SS_c_mode(SS_psides *si)
   {

    snprintf(si->prompt, MAXLINE, "C-> ");
    si->read        = SS_syntax_c;
    si->name_reproc = SS_name_map_synt;

    SS_set_parser(SS_c_mode);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_C_ADD_TYPE - add a new C type (via typedef) */

static object *SS_c_add_type(SS_psides *si, object *argl)
   {char *name;
    object *typ;

    name = NULL;
    SS_args(si, argl,
	    SC_STRING_I, &name,
	    0);

    typ = SS_add_type_synt(si, name);

    return(typ);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INIT_C_SYNTAX_MODE - setup for parsing C syntax */

void SS_init_c_syntax_mode(SS_psides *si)
   {int *ssdbg;

    if (_SS_cps.mode_init == FALSE)
       {_SS_cps.mode_init = TRUE;

	SS_load_scm(si, "csynt.scm");

        _SS_c_decl      = SS_lookup_variable(si, "c/decl-spec", TRUE);
	_SS_c_defunc    = SS_lookup_variable(si, "c/defun", TRUE);
	_SS_c_defvar    = SS_lookup_variable(si, "c/scalar", TRUE);
	_SS_c_defarr    = SS_lookup_variable(si, "c/array", TRUE);
	_SS_c_set       = SS_lookup_variable(si, "c/set!", TRUE);
	_SS_c_block     = SS_lookup_variable(si, "let*", TRUE);
	_SS_c_if        = SS_lookup_variable(si, "c/if", TRUE);
	_SS_c_for       = SS_lookup_variable(si, "for", TRUE);
	_SS_c_while     = SS_lookup_variable(si, "while", TRUE);
	_SS_c_plus      = SS_lookup_variable(si, "+", TRUE);
	_SS_c_minus     = SS_lookup_variable(si, "-", TRUE);
	_SS_c_times     = SS_lookup_variable(si, "*", TRUE);
	_SS_c_divide    = SS_lookup_variable(si, "/", TRUE);
	_SS_c_expt      = SS_lookup_variable(si, "expt", TRUE);
	_SS_c_modulo    = SS_lookup_variable(si, "quotient", TRUE);
	_SS_c_bitand    = SS_lookup_variable(si, "&", TRUE);
	_SS_c_bitor     = SS_lookup_variable(si, "|", TRUE);
	_SS_c_xor       = SS_lookup_variable(si, "xor", TRUE);
	_SS_c_xor_pow   = SS_lookup_variable(si, "^", TRUE);
	_SS_c_bitcmp    = SS_lookup_variable(si, "~", TRUE);
	_SS_c_lshft     = SS_lookup_variable(si, "<<", TRUE);
	_SS_c_rshft     = SS_lookup_variable(si, ">>", TRUE);
	_SS_c_not       = SS_lookup_variable(si, "not", TRUE);
	_SS_c_and       = SS_lookup_variable(si, "and", TRUE);
	_SS_c_or        = SS_lookup_variable(si, "or", TRUE);
	_SS_c_equal     = SS_lookup_variable(si, "eqv?", TRUE);
	_SS_c_lt        = SS_lookup_variable(si, "<", TRUE);
	_SS_c_gt        = SS_lookup_variable(si, ">", TRUE);
	_SS_c_le        = SS_lookup_variable(si, "<=", TRUE);
        _SS_c_ge        = SS_lookup_variable(si, ">=", TRUE);
        _SS_c_defined   = SS_lookup_variable(si, "defined?", TRUE);
	_SS_c_struct    = SS_lookup_variable(si, "c/struct", TRUE);
        _SS_c_union     = SS_lookup_variable(si, "c/union", TRUE);
        _SS_c_enum      = SS_lookup_variable(si, "c/enum", TRUE);
        _SS_c_derived   = SS_lookup_variable(si, "c/derived", TRUE);
	_SS_c_aref      = SS_lookup_variable(si, "c/[]", TRUE);
	_SS_c_mref      = SS_lookup_variable(si, "c/->", TRUE);
	_SS_c_preinc    = SS_lookup_variable(si, "c/++x", TRUE);
	_SS_c_predec    = SS_lookup_variable(si, "c/--x", TRUE);
	_SS_c_postinc   = SS_lookup_variable(si, "c/x++", TRUE);
	_SS_c_postdec   = SS_lookup_variable(si, "c/x--", TRUE);
	_SS_c_cond      = SS_lookup_variable(si, "c/switch", TRUE);
	_SS_c_label     = SS_lookup_variable(si, "c/label", TRUE);
	_SS_c_sizeof    = SS_lookup_variable(si, "c/sizeof", TRUE);
	_SS_c_cast      = SS_lookup_variable(si, "c/cast", TRUE);
	_SS_c_goto      = SS_lookup_variable(si, "c/goto", TRUE);
	_SS_c_strapp    = SS_lookup_variable(si, "string-append", TRUE);

	_SS_c_return    = SS_lookup_variable(si, "-return-", TRUE);
	_SS_c_continue  = SS_lookup_variable(si, "-continue-", TRUE);

/* CPP syntax procedures */
	_SS_c_load      = SS_lookup_variable(si, "load", TRUE);
	_SS_c_defmac    = SS_lookup_variable(si, "c/macro", TRUE);
	_SS_c_catvars   = SS_lookup_variable(si, "c/cat-vars", TRUE);
	_SS_c_strvar    = SS_lookup_variable(si, "c/str-var", TRUE);

/* GOTHCA: work out adding C99 types
 * the space in types like "long double" will have to be sorted out
 */
	SS_add_type_synt(si, SC_CHAR_S);
	SS_add_type_synt(si, SC_SHORT_S);
	SS_add_type_synt(si, SC_INT_S);
	SS_add_type_synt(si, SC_LONG_S);
	SS_add_type_synt(si, SC_FLOAT_S);
	SS_add_type_synt(si, SC_DOUBLE_S);
	SS_add_type_synt(si, SC_VOID_S);

	SS_add_parser(".c", SS_c_mode);
	SS_add_parser(".h", SS_c_mode);

	si->eox                   = TRUE;
	_SS_cps.diagnose_grammar = FALSE;

	ssdbg  = SS_parse_debug_c();
	*ssdbg = FALSE;

	SS_install(si, "c-mode",
		   "Procedure: set C syntax parsing mode",
		   SS_zargs,
		   SS_c_mode, SS_PR_PROC);

	SS_install(si, "c-add-type",
		   "Procedure: add a new C type",
		   SS_sargs,
		   SS_c_add_type, SS_PR_PROC);

	SS_install_cf(si, "diagnose-c-parse", 
		      "Variable: Flag to print detailed diagnostics of a C parse\n     Usage: diagnose-c-parse [0|1]",
		      SS_acc_int,
		      ssdbg);
	SS_install_cf(si, "diagnose-c-grammar", 
		      "Variable: Flag to print diagnostics of C actions/reductions\n     Usage: diagnose-c-parse [0|1]",
		      SS_acc_int,
		      &_SS_cps.diagnose_grammar);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
