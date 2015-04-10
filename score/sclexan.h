/*
 * SCLEXAN.H - include file for LEXical ANalyzer support
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_LEXAN

#define PCK_LEXAN                                 /* LEXAN package resident */

#include "cpyright.h"

#undef input
#undef output
#undef unput

#include "score.h"

#define YY_SKIP_YYWRAP
#define TOKENIZER scf77lr_text

#define COMMENT        SC_lex_push_token(SC_CMMNT_TOK, TOKENIZER)
#define DEC_INTEGER    SC_lex_push_token(SC_DINT_TOK, ATOL(TOKENIZER))
#define HEX_INTEGER    SC_lex_push_token(SC_HINT_TOK, HTOL(TOKENIZER))
#define OCT_INTEGER    SC_lex_push_token(SC_OINT_TOK, OTOL(TOKENIZER))
#define DEC_REAL       SC_lex_push_token(SC_REAL_TOK, ATOF(TOKENIZER))
#define STRING         SC_lex_push_token(SC_STRING_TOK, TOKENIZER)
#define IDENTIFIER     SC_lex_push_token(SC_IDENT_TOK, TOKENIZER)
#define KEYWORD        SC_lex_push_token(SC_KEY_TOK, TOKENIZER)
#define OPERATOR       SC_lex_push_token(SC_OPER_TOK, TOKENIZER)
#define PREDICATE      SC_lex_push_token(SC_PRED_TOK, TOKENIZER)
#define DELIMITER      SC_lex_push_token(SC_DELIM_TOK, TOKENIZER)
#define WHITESPACE     SC_lex_push_token(SC_WSPC_TOK, TOKENIZER)

#define SC_INPUT  (*(SC_current_lexical_stream->inp))
#define SC_OUTPUT (*(SC_current_lexical_stream->out))
#define SC_UNPUT  (*(SC_current_lexical_stream->unp))

#ifndef FLEX_SCANNER

#undef input
#undef output
#undef unput

#define input  SC_INPUT
#define output SC_OUTPUT
#define unput  SC_UNPUT

#endif

#undef scf77lr_wrap
#define scf77lr_wrap (*(SC_current_lexical_stream->wrap))

#undef ECHO
#define ECHO NULL

#define YY_INPUT(buf, result, max_size)                                     \
    {int c;                                                                 \
     c = SC_INPUT();                                                        \
     result = ((c != '\0') && (c != EOF) && (c != '\n') && (c != '\r'));    \
     buf[0] = (char) c;}

#endif

