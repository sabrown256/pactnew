/*
 * SHSYNT.C - support for alternate syntax parsing
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

typedef object *objp;

/*--------------------------------------------------------------------------*/

/*                       LEXICAL ANALYZER SUPPORT                           */

/*--------------------------------------------------------------------------*/

/* SS_INPUT_SYNT - get the next character from the stream for the lexer */

int SS_input_synt(SS_psides *si, char *ltxt)
   {int c;
    
    si->lex_text = ltxt;

    c = SS_get_ch(si, si->character_stream, FALSE);

    _SS.have_eof = (c == EOF);

#ifdef USE_FLEX
    if ((c != EOF) && (c < 0))
       c = 0;
#else
    if (c < 0)
       c = 0;
#endif

    return(c);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_PUSH_TOKEN - put string S back onto the stream for the lexer */

void _SS_push_token(SS_psides *si, char *s)
   {int i, n, c;

    n = strlen(s);
    for (i = n-1; i >= 0; i--)
        {c = s[i];
	 SS_unput_synt(si, c);
         s[i] = '\0';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_UNPUT_SYNT - put C back onto the stream for the lexer */

void SS_unput_synt(SS_psides *si, int c)
   {

    si->pr_ch_un(c, si->character_stream);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_WRAP_INPUT_SYNT - handle EOF for parser */

int SS_wrap_input_synt(void)
   {

    return(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MK_STRING_SYNT - make a proper string from what LEX gives
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

object *SS_mk_string_synt(SS_psides *si, char *s)
   {int i, m, n;
    char *pe, *pb;
    object *obj;

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

    obj = SS_mk_string(si, s+1);

    return(obj);}

/*--------------------------------------------------------------------------*/

/*                        SYMBOL AND TYPE ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* SS_NAME_MAP_SYNT - map a legal C identifier to a possible SCHEME one
 *                  - by changing all '_' to '-'
 *                  - this will get many names through the lookup
 *                  - this can be used for many but not all languages
 */

void SS_name_map_synt(SS_psides *si, char *d, char *s)
   {char *pd;

    strcpy(d, s);

    pd = d;
    for (pd = d; *pd != '\0'; pd++)
        {if (*pd == '_')
            *pd = '-';};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_LOOKUP_VARIABLE - lookup a variable for LEX */

object *SS_lookup_variable(SS_psides *si, char *txt, int verbose)
   {object *o;

    o = SS_add_variable(si, txt);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ADD_TYPE_SYNT - add a new data type */

object *SS_add_type_synt(SS_psides *si, char *name)
   {object *typ;

    if (name == NULL)
       typ = SS_null;

    else
       {typ = SS_mk_string(si, name);
        SC_hasharr_install(si->types, name, typ, SS_POBJECT_S, TRUE, TRUE);};

    return(typ);}

/*--------------------------------------------------------------------------*/

/*                            DIAGNOSTIC ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _SS_DIAGNOSTIC_SYNT - print diagnostics to monitor and debug the
 *                     - parser
 */

void _SS_diagnostic_synt(SS_psides *si, object *expr, char *msg, int diag)
   {char s[MAXLINE];

    if (diag)
       {snprintf(s, MAXLINE, "%s: ", msg);
        SS_print(si, si->outdev, expr, s, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_DIAGNOSE_RETURN_SYNT - print diagnostics for returns */

int _SS_diagnose_return_synt(SS_psides *si, int x, char *y, PFPInt fnc)
   {int dbg, *pdbg;

    pdbg = (*fnc)();
    dbg  = *pdbg;

    if (dbg)
       PRINT(stdout, "Token %s (%d) - %s\n", y, x, si->lex_text);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_UNSUPPORTED_SYNTAX - barf on unsupported syntax */

void _SS_unsupported_syntax(SS_psides *si, char *msg)
   {

    SS_error(si, "UNSUPPORTED SYNTAX", SS_mk_string(si, msg));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_ERROR_SYNT - error handler for parser */

int SS_parse_error_synt(SS_psides *si, char *s, PFPObject fnc)
   {object *tok, *obj;
    char msg[MAXLINE];

    tok = fnc();

    if (_SS.have_eof == TRUE)
       {_SS.have_eof = FALSE;
        LONGJMP(si->cpu, TRUE);}

    else if (si->lex_text[0] <= 0)
       LONGJMP(si->cpu, TRUE);

    else
       {snprintf(msg, MAXLINE, "BAD SYNTAX\n      TEXT:  %s", si->lex_text);
        obj = (tok == NULL) ? SS_f : tok;
        SS_error(si, msg, obj);};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DBGST - debug aid - print the parser stack */

void dbgst(SS_psides *si, objp *st)
   {int i;
    char msg[MAXLINE];

    if (si == NULL)
       si = SS_get_current_scheme(-1);       /* diagnostic default */

/* count the number of things on the stack */
    for (i = 0;
         (i > -20) && (st[i] != NULL) && (st[i] != (objp) 0x1000000);
         i--)
        {snprintf(msg, MAXLINE, "$%d = ", abs(i)+1);
         SS_print(si, si->outdev, st[i], msg, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
