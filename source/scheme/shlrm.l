%{

/*
 * SHLRM.L - lexical rules for BASIS langauge parses
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                           DEFINITIONS SECTION                            */

/*--------------------------------------------------------------------------*/

#undef yywrap
#define yywrap SS_wrap_input_synt

#define DIAG_RETURN(x)                                                       \
    return(_SS_diagnose_return_synt(x, #x, SS_parse_debug_m))

#define IDENT_RETURN(x)                                                      \
    return(_SS_diagnose_return_synt(x, "IDENTIFIER", SS_parse_debug_m))

#ifdef FLEX_SCANNER

#undef YY_INPUT
#define YY_INPUT(bf, res, szx)                                               \
   {int c;                                                                   \
    c     = SS_input_synt((char *) yytext);                                  \
    res   = (c != EOF);                                                      \
    bf[0] = (char) c;}

#else

#undef input
#undef unput

#define input()  SS_input_synt((char *) yytext)
#define unput(c) SS_unput_synt(c)

#endif

/*--------------------------------------------------------------------------*/

/*                       END OF DEFINITIONS SECTION                         */

/*--------------------------------------------------------------------------*/

%}

BD            [01]
OD            [0-7]
DD            [0-9]
HD            [0-9a-fA-F]

WS            [ \t\f]
EOSS          [\n\r;]
FCMTS         ^([c!]).*
MCMTS         #.*

EXP           [Ee]("+"|"-")?{DD}+
BIN_INTEGER   [bB]{BD}+
OCT_INTEGER   [oO]{OD}+
HEX_INTEGER   [zZ]{HD}+
DEC_INTEGER   {DD}+

COMMA         "command_("{COMMQ}+")"
COMMB         "command_"{COMMQ}+
IDENT         [$%a-z][$%a-zA-Z_0-9_]*
GROUPNS       [A-Z][$%a-zA-Z_0-9]*
COMMQ         [sSeEwWcCqQaA0-9]

STR           \"[^"]*\"

IOPS          \>\>
OOPS          \<\<

EQOS          \.eq\.
NEQOS         \.ne\.
LTOS          \.lt\.
LEOS          \.le\.
GEOS          \.ge\.
GTOS          \.gt\.

EQNS          \=\=
NEQNS         \<\>
LTNS          \<
LENS          \<\=
GENS          \>\=
GTNS          \>

ASGNS         \=

ANDS          \.and\.
ORS           \.or\.
NOTS          \.not\.

BOOLTS        \.true\.
BOOLFS        \.false\.

EXPTS         \*\*

SELCASS       "select case"
ENDSELS       "end select"

%p 3000
%a 2200

%%

if           {DIAG_RETURN(IF);}
else         {DIAG_RETURN(ELSE);}
elseif       {DIAG_RETURN(ELSEIF);}
then         {DIAG_RETURN(THEN);}
endif        {DIAG_RETURN(ENDIF);}

while        {DIAG_RETURN(WHILE);}
endwhile     {DIAG_RETURN(ENDWHILE);}

do           {DIAG_RETURN(DO);}
enddo        {DIAG_RETURN(ENDDO);}

for          {DIAG_RETURN(FOR);}
endfor       {DIAG_RETURN(ENDFOR);}

next         {DIAG_RETURN(NEXT);}
break        {DIAG_RETURN(BREAK);}
return       {DIAG_RETURN(RETURN);}
goto         {DIAG_RETURN(GOTO);}

subroutine   {DIAG_RETURN(SUBR);}
function     {DIAG_RETURN(FUNC);}
endf         {DIAG_RETURN(ENDF);}

call         {DIAG_RETURN(CALL);}

{OCT_INTEGER}              {yylval = SS_mk_integer((BIGINT) STRTOL(yytext, NULL, 8));
                            DIAG_RETURN(INTEGER);}

{DEC_INTEGER}              {yylval = SS_mk_integer((BIGINT) STRTOL(yytext, NULL, 10));
                            DIAG_RETURN(INTEGER);}

{HEX_INTEGER}              {yylval = SS_mk_integer((BIGINT) STRTOL(yytext, NULL, 10));
                            DIAG_RETURN(INTEGER);}

{DD}+\.?{DD}*({EXP})? |
{DD}*\.?{DD}+({EXP})?      {yylval = SS_mk_float(STRTOD(yytext, NULL));
                            DIAG_RETURN(FLOAT);}

{STR}      {yylval = SS_mk_string_synt((char *) yytext);
            DIAG_RETURN(STRING);}

{EXPTS}    {DIAG_RETURN(EXPT);}

{IOPS}     {DIAG_RETURN(INPUTOP);}
{OOPS}     {DIAG_RETURN(OUTPUTOP);}

{ASGNS}    {DIAG_RETURN(ASGN);}

{EQNS}     {DIAG_RETURN(EQ);}
{NEQNS}    {DIAG_RETURN(NEQ);}
{LTNS}     {DIAG_RETURN(LT);}
{LENS}     {DIAG_RETURN(LE);}
{GENS}     {DIAG_RETURN(GE);}
{GTNS}     {DIAG_RETURN(GT);}

{EQOS}     {DIAG_RETURN(EQ);}
{NEQOS}    {DIAG_RETURN(NEQ);}
{LTOS}     {DIAG_RETURN(LT);}
{LEOS}     {DIAG_RETURN(LE);}
{GEOS}     {DIAG_RETURN(GE);}
{GTOS}     {DIAG_RETURN(GT);}

{NOTS}     {DIAG_RETURN(NOT);}
{ORS}      {DIAG_RETURN(OR);}
{ANDS}     {DIAG_RETURN(AND);}

{BOOLTS}   {DIAG_RETURN(BOOLT);}
{BOOLFS}   {DIAG_RETURN(BOOLF);}

{EOSS}     {DIAG_RETURN(EOS);}

{COMMA}    {DIAG_RETURN(COMMAND);}
{COMMB}    {DIAG_RETURN(COMMAND);}

{WS}                    ;
{FCMTS}                 ;
{MCMTS}                 ;

{IDENT}    {IDENT_RETURN(SS_lookup_identifier_m((char *) yytext, &yylval));}
{GROUPNS}  {DIAG_RETURN(GROUPNAME);}

.          {return(yytext[0]);}

%%
