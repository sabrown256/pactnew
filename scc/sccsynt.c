/*
 * SCCSYNT.C - support for C parsing
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scc_int.h"

DEF_FUNCTION_PTR(expr *, PExpr);

/*--------------------------------------------------------------------------*/

/*                       LEXICAL ANALYZER SUPPORT                           */

/*--------------------------------------------------------------------------*/

/* CC_GET_CH - the do it right character reader
 *           - must get the next character from the given stream
 */

int CC_get_ch(FILE *fp, int ign_ws)
   {int c, ncx;
    decl *pd;

    c = getc(fp);

    if (_CC.cur == NULL)
       _CC.cur = CC_mk_decl(_CC.rloc.iln, MAX_BFSZ);

    pd = _CC.cur;

    if (_CC.ich >= pd->ncx - 2)
       {ncx     = _CC.ich + MAX_BFSZ;
        pd->ncx = ncx;
        CREMAKE(pd->text, char, ncx);};

    pd->text[_CC.ich++] = c;
    pd->text[_CC.ich]   = '\0';

    if (c == '\n')
       {_CC.rloc.iln++;
	_CC.vloc.iln++;};

    if (c == '{')
       _CC.ncbr++;

    if (c == '}')
       _CC.ncbr--;

    if (c == '[')
       _CC.nsbr++;

    if (c == ']')
       _CC.nsbr--;

    if (c == '(')
       _CC.nprn++;

    if (c == ')')
       _CC.nprn--;

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_INPUT_C - get the next character from the stream for the lexer */

int CC_input_c(const char *ltxt)
   {int c, eof;
    
    eof = (int) EOF;

    if (_CC.have_eof == TRUE)
       c = eof;

    else
       {_CC.lex_text = (char *) ltxt;

	c = CC_get_ch(_CC.fp, FALSE);

	_CC.have_eof = (c == eof);

#ifdef USE_FLEX
	if ((c != eof) && (c < 0))
	   c = eof;
#else
	if (c < 0)
	   c = eof;
#endif
	};

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_PUSH_TOKEN - put string S back onto the stream for the lexer */

void _CC_push_token(char *s)
   {int i, n, c;

    n = strlen(s);
    for (i = n-1; i >= 0; i--)
        {c = s[i];
	 CC_unput_c(c);
         s[i] = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_UNPUT_C - put C back onto the stream for the lexer */

void CC_unput_c(int c)
   {int rv;
    decl *pd;

    pd = _CC.cur;

    pd->text[--_CC.ich] = '\0';

    rv = ungetc(c, _CC.fp);
    SC_ASSERT(rv == c);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_WRAP_INPUT_C - handle EOF for parser */

int CC_wrap_input_c(void)
   {

    return(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_RECKON_LINE - track file and line number */

void CC_reckon_line(void)
   {int ln, st;
    char s[MAXLINE];
    char *t, *fn;
    struct stat sb;

    SC_strncpy(s, MAXLINE, _CC.lex_text, -1);
    t  = strtok(s, "# \t\n");
    ln = SC_stoi(t);

    fn = strtok(NULL, " \t\n\"");
    if (fn != NULL)
       {st = stat(fn, &sb);
	if (st == 0)
	   {CFREE(_CC.vloc.fname);

	    _CC.vloc.iln   = ln;
	    _CC.vloc.fname = CSTRSAVE(fn);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_MK_STRING_C - make a proper string from what LEX gives
 *                   - S may be of the form:
 *                   -   ["[c]*"[b]*]+
 *                   - where
 *                   -   c is any character
 *                   -   b is a whitespace character
 *                   - examples
 *                   -   "foo"
 *                   -   "foo"  "bar"
 *                   - this is here to handle C string literal
 *                   - concatenation rules
 *                   - FACTOR/PARSE
 */

expr *CC_mk_string_c(char *s)
   {int i, m, n;
    char *pe, *pb;
    expr *obj;

    n = strlen(s);

/* search for a closing quote */
    for (i = 1; i < n; i++)
        {switch (s[i])

/* skip escaped characters especially " */
	    {case '\\':
                  i++;
                  break;

	     case '"':
		  pb = s + i;

/* check for more quotes in case of concatenation */
		  pe = strchr(pb+1, '"');

/* if we are concatenating strings remove quotes
 * and intervening white space then copy the next string down
 * into its final position
 */
		  if (pe != NULL)
		     {m = strlen(pe+1);
		      memmove(pb, pe+1, m);
		      n    = i + m;
		      s[n] = '\0';}
		  else
		     {*pb = '\0';
		      break;};
		  break;
	     default:
		  break;};};

    obj = CC_mk_string(s+1);

    return(obj);}

/*--------------------------------------------------------------------------*/

/*                            DIAGNOSTIC ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _CC_DIAGNOSTIC_C - print diagnostics to monitor and debug the parser */

void _CC_diagnostic_c(expr *e, const char *msg, int diag)
   {char s[MAXLINE];

    if (diag)
       {CC_expr_str(s, MAXLINE, e);

	fprintf(stdout, "%s: %s\n", msg, s);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_DIAGNOSE_RETURN_C - print diagnostics for returns */

int _CC_diagnose_return_c(int x, const char *y, PFPInt fnc)
   {int dbg, *pdbg;

    pdbg = (*fnc)();
    dbg  = *pdbg;

    if (dbg)
       fprintf(stdout, "Token %s (%d) - %s\n", y, x, _CC.lex_text);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_ERROR - put string S back onto the stream for the lexer */

static void _CC_error(const char *s, char *e)
   {

    fprintf(stdout, "%s\n", s);

    _CC.n_error++;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _CC_UNSUPPORTED_C - barf on unsupported syntax */

void _CC_unsupported_c(const char *msg)
   {char s[MAXLINE];

    snprintf(s, MAXLINE, "UNSUPPORTED(%d/%d):  %s",
	     _CC.rloc.iln, _CC.ich, _CC.lex_text);
    _CC_error(s, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_PARSE_ERROR_C - error handler for parser */

int CC_parse_error_c(const char *s, PFPExpr fnc)
   {int n;
    char msg[MAXLINE];
    char *t;
    expr *tok;

    tok = fnc();
    SC_ASSERT(tok != NULL);

    if (_CC.have_eof == TRUE)
       {_CC.have_eof = FALSE;
        LONGJMP(_CC.cpu, TRUE);}

    else if (_CC.lex_text[0] <= 0)
       {t = _CC_clean_text(_CC.cur->text);
        n = _CC.ich - (t - _CC.cur->text);
	snprintf(msg, MAXLINE, "ERR(%d/%d): %s", _CC.rloc.iln, n, t);
        _CC_error(msg, NULL);
	LONGJMP(_CC.cpu, TRUE);}

    else
       {snprintf(msg, MAXLINE, "ERR(%d/%d):  %s",
		 _CC.rloc.iln, _CC.ich, _CC.lex_text);
        _CC_error(msg, NULL);};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBGSTK - debug aid - print the parser stack */

void dbgstk(exprp *st)
   {int i;
    char msg[MAXLINE];

/* count the number of things on the stack */
    for (i = 0;
         (i > -20) && (st[i] != NULL) && (st[i] != (exprp) 0x1000000);
         i--)
        {snprintf(msg, MAXLINE, "$%d = ", abs(i)+1);
         fprintf(stdout, "-> %s\n", msg);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
