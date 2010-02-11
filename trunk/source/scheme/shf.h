/*
 * SHF.H - support routines for Fortran syntax mode
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_F_SYNTAX
#define PCK_F_SYNTAX

#include "scheme_int.h"

/*--------------------------------------------------------------------------*/

/*                                   MACROS                                 */

/*--------------------------------------------------------------------------*/

#ifndef EXTERN_VAR
# define EXTERN_VAR extern
#endif

#undef IF
#undef ELSE
#undef CONTINUE
#undef RETURN
#undef CONST
#undef SIGNED
#undef ECHO

#define CAPTURE(x)       (SS_MARK(x), x)

#define DIAGNOSTIC(x, y) _SS_diagnostic_synt(x, y, SS_diagnostic_f)
#define SYNTAX_ERR(x)    _SS_unsupported_syntax(x)

/*--------------------------------------------------------------------------*/

/*                            STRUCT AND TYPEDEF                            */

/*--------------------------------------------------------------------------*/

typedef object *objp;

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

EXTERN_VAR int
 SS_diagnostic_f;

EXTERN_VAR object
 *_SS_f_defunc,
 *_SS_f_defvar,
 *_SS_f_defarr,
 *_SS_f_defstr,
 *_SS_f_set,
 *_SS_f_block,
 *_SS_f_if,
 *_SS_f_expt,
 *_SS_f_plus,
 *_SS_f_minus,
 *_SS_f_times,
 *_SS_f_divide,
 *_SS_f_modulo,
 *_SS_f_not,
 *_SS_f_and,
 *_SS_f_or,
 *_SS_f_equal,
 *_SS_f_lt,
 *_SS_f_gt,
 *_SS_f_le,
 *_SS_f_ge,
 *_SS_f_bitand,
 *_SS_f_bitor,
 *_SS_f_bitcmp,
 *_SS_f_lshft,
 *_SS_f_rshft,
 *_SS_f_declare,
 *_SS_f_struct,
 *_SS_f_union,
 *_SS_f_typedef,
 *_SS_f_derived,
 *_SS_f_aref,
 *_SS_f_mref,
 *_SS_f_preinc,
 *_SS_f_predec,
 *_SS_f_postinc,
 *_SS_f_postdec,
 *_SS_f_for,
 *_SS_f_while,
 *_SS_f_cond,
 *_SS_f_label,
 *_SS_f_sizeof,
 *_SS_f_cast,
 *_SS_f_continue,
 *_SS_f_return;

extern int
 SS_f_tokens[];

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

/* SHGRF.Y declarations */

extern int
 shgrf_parse(void);

extern object
 *_SS_make_funf(object *proto, object *body),
 *SS_syntax_f(object *str),
 *SS_parse_token_f(void),
 *SS_parse_token_val_f(void);

extern void
 SS_init_f_syntax_mode(void);

extern int
 SS_lookup_identifier_f(char *txt, object **lval),
 *SS_parse_debug_f(void);

#endif
