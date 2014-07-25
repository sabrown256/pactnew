/*
 * SCOPE_LEX.H - define LEX scope
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_LEX

#define PCK_SCOPE_LEX

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                              CONSTANT MACROS                             */

/*--------------------------------------------------------------------------*/

/* relevant languages require/support these token types */

enum e_SC_token_type
   {SC_WSPC_TOK,      /* whitespace token */
    SC_DELIM_TOK,     /* delimiter token */
    SC_IDENT_TOK,     /* identifier token */
    SC_DINT_TOK,      /* decimal integer number token */
    SC_REAL_TOK,      /* decimal real number token */
    SC_OINT_TOK,      /* octal integer number token */
    SC_HINT_TOK,      /* hexidecimal integer number token */
    SC_OPER_TOK,      /* operator token */
    SC_STRING_TOK,    /* string token */
    SC_KEY_TOK,       /* keyword token */
    SC_PRED_TOK,      /* predicate token */
    SC_CMMNT_TOK,     /* comment token */
    SC_HOLLER_TOK};   /* hollerith token */

typedef enum e_SC_token_type SC_token_type;

/*--------------------------------------------------------------------------*/

/*                             PROCEDURAL MACROS                            */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                             TYPEDEFS AND STRUCTS                         */

/*--------------------------------------------------------------------------*/

typedef union u_SC_token_value SC_token_value;
typedef struct s_SC_lexical_token SC_lexical_token;
typedef struct s_SC_lexical_stream SC_lexical_stream;

union u_SC_token_value
   {long l;
    double d;
    char *s;
    PFInt f;};

struct s_SC_lexical_token
   {int type;
    SC_token_value val;};

struct s_SC_lexical_stream
   {int i_token;
    int n_tokens;
    int n_tokens_max;
    int nbo;                  /* in_bf and out_bf size */
    int nbs;                  /* str_bf size */
    char *name;
    FILE *file;
    SC_lexical_token *tokens;
    char *in_bf;
    char *in_ptr;
    char *out_bf;
    char *out_ptr;
    char *str_bf;
    char *str_ptr;
    PFInt scan;
    PFInt inp;
    int (*out)(int c);
    int (*unp)(int c);
    PFInt wrap;
    PFInt more;
    PFInt less;};

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern SC_lexical_stream
 *SC_current_lexical_stream;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/


/* SCLEXS.C declarations */

extern int
 SC_scan(SC_lexical_stream *str, int rd);

extern void
 SC_close_lexical_stream(SC_lexical_stream *str),
 SC_lex_push_token(int type, ...);

extern SC_lexical_stream
 *SC_open_lexical_stream(char *name, int inbfsz, int strbfsz,
			 PFInt scan, PFInt input, int (*output)(int c),
			 int (*unput)(int c), PFInt wrap, PFInt more, PFInt less);

extern SC_lexical_token
 *SC_get_next_lexical_token(SC_lexical_stream *str);


#ifdef __cplusplus
}
#endif

#endif

