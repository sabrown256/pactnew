/*
 * SHM.H - support routines for BASIS langauge syntax mode
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_M_SYNTAX
#define PCK_M_SYNTAX

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
#undef SIGNED
#undef ECHO
#undef SI
#undef DIAGNOSTIC

#define SI               _SS_ism

#define CAPTURE(x)       (SS_mark(x), x)

#define DIAGNOSTIC(x, y) _SS_diagnostic_synt(SI, x, y, _SS_mps.diagnose_grammar)
#define SYNTAX_ERR(x)    _SS_unsupported_syntax(SI, x)

/*--------------------------------------------------------------------------*/

/*                            STRUCT AND TYPEDEF                            */

/*--------------------------------------------------------------------------*/

typedef struct s_SS_m_parse_state SS_m_parse_state;

struct s_SS_m_parse_state
   {int mode_init;            /* has the Basis mode been initialized */
    int diagnose_grammar;};   /* debug flag - print parse results */

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern SS_m_parse_state
 _SS_mps;

EXTERN_VAR object
 *_SS_m_defunc,
 *_SS_m_defvar,
 *_SS_m_defarr,
 *_SS_m_set,
 *_SS_m_loop_test,
 *_SS_m_arr_call,
 *_SS_m_list,
 *_SS_m_block,
 *_SS_m_if,
 *_SS_m_for,
 *_SS_m_while,
 *_SS_m_expt,
 *_SS_m_plus,
 *_SS_m_minus,
 *_SS_m_times,
 *_SS_m_divide,
 *_SS_m_modulo,
 *_SS_m_bitand,
 *_SS_m_bitor,
 *_SS_m_bitcmp,
 *_SS_m_not,
 *_SS_m_and,
 *_SS_m_or,
 *_SS_m_equal,
 *_SS_m_lt,
 *_SS_m_gt,
 *_SS_m_le,
 *_SS_m_ge,
 *_SS_m_label,
 *_SS_m_load,
 *_SS_m_output,
 *_SS_m_input,
 *_SS_m_read,
 *_SS_m_continue,
 *_SS_m_return;

extern int
 SS_m_tokens[];

extern SS_psides
 *_SS_ism;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

/* SHGRM.Y declarations */

extern int
 shgrm_parse(void);

extern object
 *_SS_make_fun_m(SS_psides *si, object *proto, object *body),
 *_SS_make_comp_stmt_m(SS_psides *si, object *body),
 *_SS_strip_call(SS_psides *si, object *expr, int paren),
 *SS_syntax_m(SS_psides *si, object *str),
 *SS_parse_token_m(void),
 *SS_parse_token_val_m(void);

extern void
 SS_init_m_syntax_mode(SS_psides *si);

extern int
 SS_lookup_identifier_m(SS_psides *si, char *txt, object **lval),
 *SS_parse_debug_m(void);

#endif
