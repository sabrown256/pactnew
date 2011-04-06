/*
 * SCLEXS.C - lexical scanner functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h" 

SC_lexical_stream
 *SC_current_lexical_stream = NULL;

/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEX_PUTC - a function to attach to the lexical scanner output macro */

static int _SC_lex_putc(int c)
   {

    *SC_current_lexical_stream->out_ptr++ = c;

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEX_GETC - a function to attach to the lexical scanner input macro */

static int _SC_lex_getc(void)
   {int rv;

    rv = *SC_current_lexical_stream->in_ptr++;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEX_PUSH - a function to attach to the lexical scanner unput macro */

static int _SC_lex_push(int c)
   {int nb;
    SC_lexical_stream *str;

    str = SC_current_lexical_stream;

    nb = str->in_ptr - str->in_bf;
    if (nb > 0)
       {*(--str->in_ptr) = c;};

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SC_LEX_WRAP - a default EOF handler for lexical scanners */

static int _SC_lex_wrap(void)
   {

    return(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_OPEN_LEXICAL_STREAM - open up a stream for lexical scanning
 *                        - Arguments:
 *                        -
 *                        -  NAME    - name of file or NULL for stdin
 *                        -  INBFSZ  - byte size of input buffer or
 *                        -            MAXLINE if 0
 *                        -  STRBFSZ - byte size of string buffer or
 *                        -            MAX_LEN_BUFFER if 0
 *                        -  SCAN    - scanning function such as yylex (int)
 *                        -  INPUT   - input function (int)
 *                        -            SC_lex_getc used if NULL
 *                        -  OUTPUT  - output function (int)
 *                        -            SC_lex_putc used if NULL
 *                        -  UNPUT   - unput function (int)
 *                        -            SC_lex_push used if NULL
 *                        -  WRAP    - wrap function (int)
 *                        -            _SC_wrap used if NULL
 *                        -  MORE    - more function (int)
 *                        -  LESS    - less function (int)
 */

SC_lexical_stream *SC_open_lexical_stream(char *name, int inbfsz,
					  int strbfsz,
					  PFInt scan, PFInt input,
					  int (*output)(int c), int (*unput)(int c),
					  PFInt wrap, PFInt more,
					  PFInt less)
   {SC_lexical_stream *str;

    str = CMAKE(SC_lexical_stream);

/* set up the file */
    if (name != NULL)
       {str->name = CSTRSAVE(name);
        str->file = io_open(name, "r");}
    else
       {str->name = NULL;
        str->file = stdin;};

/* allocate the token space */
    str->n_tokens_max = 50;
    str->n_tokens     = 0;
    str->tokens       = CMAKE_N(SC_lexical_token, 50);

/* allocate the buffers */
    if (inbfsz != 0)
       {str->in_bf  = CMAKE_N(char, inbfsz);
        str->out_bf = CMAKE_N(char, inbfsz);}
    else
       {str->in_bf  = CMAKE_N(char, MAXLINE);
        str->out_bf = CMAKE_N(char, MAXLINE);}

    if (strbfsz != 0)
       str->str_bf = CMAKE_N(char, strbfsz);
    else
       str->str_bf = CMAKE_N(char, MAX_BFSZ);

    str->str_ptr = str->str_bf;
    str->in_ptr  = str->in_bf;
    str->out_ptr = str->out_bf;

/* set up the assocaited functions */
    str->scan = scan;
    str->more = more;
    str->less = less;

    if (input != NULL)
       str->inp = input;
    else
       str->inp = _SC_lex_getc;

    if (output != NULL)
       str->out = output;
    else
       str->out = _SC_lex_putc;

    if (input != NULL)
       str->unp = unput;
    else
       str->unp = _SC_lex_push;

    if (wrap != NULL)
       str->wrap = wrap;
    else
       str->wrap = _SC_lex_wrap;

    return(str);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_CLOSE_LEXICAL_STREAM - close an SC_lexical_stream and clean up the
 *                         - memory
 */

void SC_close_lexical_stream(SC_lexical_stream *str)
   {

    if (str->name != NULL)
       {io_close(str->file);
        CFREE(str->name);};

    CFREE(str->tokens);
    CFREE(str->in_bf);
    CFREE(str->out_bf);
    CFREE(str->str_bf);

    CFREE(str);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_SCAN - using an automatically generated lexical scanner
 *         - scan the given input string and break it down into
 *         - its lexical tokens
 *         - STR is the SC_lexical_stream
 *         - if RD is TRUE, read a line from STR before scanning
 *         - return the number of tokens read and available
 *         - return -1 if RD is TRUE and EOF has been found
 */

int SC_scan(SC_lexical_stream *str, int rd)
   {int sz;
    char *in, *out;

    if (str->tokens == NULL)
       return(-1);

    memset(str->tokens, 0, str->n_tokens_max*sizeof(SC_lexical_token));

    in  = str->in_bf;
    out = str->out_bf;

    str->i_token  = 0;
    str->n_tokens = 0;
    str->str_ptr = str->str_bf;
    str->in_ptr  = in;
    str->out_ptr = out;

    if (rd)
       {sz = SC_arrlen(out);
        memset(out, 0, sz);

        sz = SC_arrlen(in);
        if (io_gets(in, sz, str->file) == NULL)
           return(-1);};

    SC_current_lexical_stream = str;

    SC_LEX_SCAN(str);

    SC_current_lexical_stream = NULL;

    return(str->n_tokens);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_GET_NEXT_LEXICAL_TOKEN - get the next lexical token from the
 *                           - given lexical stream
 *                           - return NULL if no tokens available
 *                           - NOTE: do we want to distinguish between
 *                           -       EOL, EOF, and an error or
 *                           -       should we leave that to the
 *                           -       lexical scanning rules?
 */

SC_lexical_token *SC_get_next_lexical_token(SC_lexical_stream *str)
   {SC_lexical_token *tok;

/* if no more tokens in the buffer, scan some more from the stream */
    if (str->i_token >= str->n_tokens)
       {SC_scan(str, TRUE);
        if (str->n_tokens == 0)
           return(NULL);};

    tok = &str->tokens[str->i_token++];

    return(tok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_LEX_PUSH_TOKEN - called by automatically generated lexical scanners
 *                   - such as FLEX to push a token onto an array
 */

void SC_lex_push_token(int type, ...)
   {int indx;
    char *s;
    SC_lexical_stream *str;

    SC_VA_START(type);

    str  = SC_current_lexical_stream;
    indx = str->n_tokens++;

    str->tokens[indx].type = type;

    switch (type)
       {case SC_CMMNT_TOK  :
        case SC_STRING_TOK :
        case SC_IDENT_TOK  :
        case SC_KEY_TOK    :
        case SC_OPER_TOK   :
        case SC_PRED_TOK   :
        case SC_DELIM_TOK  :
        case SC_WSPC_TOK   :
        case SC_HOLLER_TOK :
             s = SC_VA_ARG(char *);
	     strcpy(str->str_ptr, s);
             str->tokens[indx].val.s = str->str_ptr;
	     str->str_ptr += strlen(s) + 1;
             break;

        case SC_DINT_TOK   :
        case SC_HINT_TOK   :
        case SC_OINT_TOK   :
             str->tokens[indx].val.l = SC_VA_ARG(long);
             break;

        case SC_REAL_TOK   :
             str->tokens[indx].val.d = SC_VA_ARG(double);
             break;

        default            :
             break;};

    SC_VA_END;

    if (str->n_tokens >= str->n_tokens_max)
       {str->n_tokens_max += 50;
        CREMAKE(str->tokens, SC_lexical_token, str->n_tokens_max);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
