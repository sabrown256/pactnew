%{

/*
 * SCF77LR.L - partial set of lexical rules for FORTRAN 77
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                            DEFINITIONS SECTION                           */

/*--------------------------------------------------------------------------*/

#include "sclexan.h"

#define scf77lr_lex F77_FUNC(f77lxr, F77LXR)

/*--------------------------------------------------------------------------*/

/*                         END OF DEFINITIONS SECTION                       */

/*--------------------------------------------------------------------------*/

%}

WSPC   [ \t\n\r\f]
D      [0-9]
OD     [0-7]
HD     [0-9A-Fa-f]
E      [DEde][-+]?{D}+
IDENT  [A-Za-z][A-Za-z0-9]*
PRED   (\.(gt|ge|lt|le|eq|ne)\.)
OPER   ("-"|"+"|"*"|"/"|"="|"**")
STRNG  "(.(\\\")?)*"
KEY    ("if"|"then"|"else"|"elseif"|"endif"|"for"|"continue"|"common")

%%

{WSPC}+   ;

^"c".*    COMMENT;

[()]      DELIMITER;

{STRNG}   STRING;

{OD}+b    OCT_INTEGER;

#{HD}+    HEX_INTEGER;

{D}+                  DEC_INTEGER;
{D}+"."{D}*({E})? |
{D}*"."{D}+({E})? |
{D}+{E}               DEC_REAL;

{OPER}    OPERATOR;

{KEY}     KEYWORD;

{PRED}    PREDICATE;

{IDENT}   IDENTIFIER;

{D}+[hH]  {int i, nc;
           char s[10];

           strncpy(s, (CONST char *) yytext, 10);
           nc = strlen(s);
           s[nc-1] = '\0';

           nc = ATOL(s);
           for (i = 0; i < nc; i++)
               yytext[yyleng++] = SC_INPUT();
           yytext[yyleng++] = '\0';

           SC_lex_push_token(SC_HOLLER_TOK, yytext);}

%%

/*--------------------------------------------------------------------------*/

/*                               EXTRAS SECTION                             */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*                            END OF EXTRAS SECTION                         */

/*--------------------------------------------------------------------------*/

