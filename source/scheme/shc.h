/*
 * SHC.H - support routines for C syntax mode
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_C_SYNTAX
#define PCK_C_SYNTAX

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

#define SI               (&_SS_si)

#define CAPTURE(x)       (SS_MARK(x), x)

#define DIAGNOSTIC(x, y) _SS_diagnostic_synt(SI, x, y, _SS_cps.diagnose_grammar)
#define SYNTAX_ERR(x)    _SS_unsupported_syntax(SI, x)

/*--------------------------------------------------------------------------*/

/*                            STRUCT AND TYPEDEF                            */

/*--------------------------------------------------------------------------*/

typedef object *objp;
typedef struct s_SS_c_parse_state SS_c_parse_state;

struct s_SS_c_parse_state
   {int mode_init;            /* has the C mode been initialized */
    int cpp_define;           /* processing a #define */
    int cpp_directive;        /* parsing a CPP directive */
    int diagnose_grammar;};   /* debug flag - print parse results */

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern SS_c_parse_state
 _SS_cps;

EXTERN_VAR object
 *_SS_c_decl,
 *_SS_c_defunc,
 *_SS_c_defmac,
 *_SS_c_defvar,
 *_SS_c_defarr,
 *_SS_c_set,
 *_SS_c_block,
 *_SS_c_if,
 *_SS_c_plus,
 *_SS_c_minus,
 *_SS_c_times,
 *_SS_c_divide,
 *_SS_c_expt,
 *_SS_c_modulo,
 *_SS_c_not,
 *_SS_c_and,
 *_SS_c_or,
 *_SS_c_equal,
 *_SS_c_lt,
 *_SS_c_gt,
 *_SS_c_le,
 *_SS_c_ge,
 *_SS_c_bitand,
 *_SS_c_bitor,
 *_SS_c_xor,
 *_SS_c_xor_pow,
 *_SS_c_bitcmp,
 *_SS_c_lshft,
 *_SS_c_rshft,
 *_SS_c_defined,
 *_SS_c_struct,
 *_SS_c_union,
 *_SS_c_enum,
 *_SS_c_derived,
 *_SS_c_aref,
 *_SS_c_mref,
 *_SS_c_preinc,
 *_SS_c_predec,
 *_SS_c_postinc,
 *_SS_c_postdec,
 *_SS_c_for,
 *_SS_c_while,
 *_SS_c_cond,
 *_SS_c_label,
 *_SS_c_sizeof,
 *_SS_c_cast,
 *_SS_c_continue,
 *_SS_c_load,
 *_SS_c_catvars,
 *_SS_c_strvar,
 *_SS_c_strapp,
 *_SS_c_goto,
 *_SS_c_return;

extern int
 SS_c_tokens[];

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

/* SHGRC.Y declarations */

extern int
 shgrc_parse(void);

extern object
 *_SS_make_func(SS_psides *si, object *proto, object *body),
 *_SS_del_var(SS_psides *si, object *var),
 *_SS_make_decl(SS_psides *si, object *ds, object *idl),
 *_SS_make_macr(object *proto, object *body),
 *_SS_make_mac_decl(SS_psides *si, object *ds, object *idl),
 *_SS_make_body(object *dcls, object *stmnts),
 *_SS_make_stmnt_lst(SS_psides *si, object *s, object *sl),
 *_SS_make_cmpnd_stmnt(SS_psides *si, object *dcl, object *sl),
 *_SS_make_cast(SS_psides *si, object *type, object *expr),
 *SS_syntax_c(SS_psides *si, object *str),
 *SS_parse_token_c(void),
 *SS_parse_token_val_c(void);

extern void
 SS_init_c_syntax_mode(SS_psides *si);

extern int
 SS_lookup_identifier_c(SS_psides *si, char *txt, object **lval),
 *SS_parse_debug_c(void);

#endif
