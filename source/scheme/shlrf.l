%{

/*
 * SHLRF.L - lexical rules for algebraic parses
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
    return(_SS_diagnose_return_synt(x, #x, SS_parse_debug_f))

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

/* WS            [ \t\n\r\f] */
WS            [ \t\f]

EOSS          [\n\r]

EXP           [Ee]("+"|"-")?{DD}+
BIN_INTEGER   [bB]{BD}+
OCT_INTEGER   [oO]{OD}+
HEX_INTEGER   [zZ]{HD}+
DEC_INTEGER   {DD}+

IDENT         [a-zA-Z_][a-zA-Z0-9_]*

STR           \"[^"]*\"

EQOS          \.eq\.
EQNS          ==
NEQOS         \.ne\.
NEQNS         \<>\.
LTOS          \.lt\.
LTNS          \<\.
LEOS          \.le\.
LENS          \<=\.
GEOS          \.ge\.
GENS          \>=\.
GTOS          \.gt\.
GTNS          \>\.

ANDS          \.and\.
ORS           \.or\.

BOOLTS        \.true\.
BOOLFS        \.false\.

RSS           >>
LSS           <<

EXPTS         \*\*

SELCASS       "select case"
ENDSELS       "end select"
ENDIFS        "endif"

%s CMMNTS
BEGCMT        {WS}*"/*"
ENDCMT        "*/"

%p 3000
%a 5200

%%

if           {DIAG_RETURN(IF);}
then         {DIAG_RETURN(THEN);}
else         {DIAG_RETURN(ELSE);}
while        {DIAG_RETURN(WHILE);}
do           {DIAG_RETURN(DO);}
for          {DIAG_RETURN(FOR);}
switch       {DIAG_RETURN(SWITCH);}
case         {DIAG_RETURN(CASE);}
default      {DIAG_RETURN(DEFAULT);}
continue     {DIAG_RETURN(CONTINUE);}
break        {DIAG_RETURN(BREAK);}
return       {DIAG_RETURN(RETURN);}
goto         {DIAG_RETURN(GOTO);}
sizeof       {DIAG_RETURN(SIZOF);}
subroutine   {DIAG_RETURN(SUBR);}
function     {DIAG_RETURN(FUNC);}

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

{RSS}      {DIAG_RETURN(RS);}
{LSS}      {DIAG_RETURN(LS);}

{EQOS}     {DIAG_RETURN(EQ);}
{EQNS}     {DIAG_RETURN(EQ);}
{NEQOS}    {DIAG_RETURN(NEQ);}
{NEQNS}    {DIAG_RETURN(NEQ);}
{LTOS}     {DIAG_RETURN(LT);}
{LTNS}     {DIAG_RETURN(LT);}
{LEOS}     {DIAG_RETURN(LE);}
{LENS}     {DIAG_RETURN(LE);}
{GEOS}     {DIAG_RETURN(GE);}
{GENS}     {DIAG_RETURN(GE);}
{GTOS}     {DIAG_RETURN(GT);}
{GTNS}     {DIAG_RETURN(GT);}

{ORS}      {DIAG_RETURN(OR);}
{ANDS}     {DIAG_RETURN(AND);}

{BOOLTS}   {DIAG_RETURN(BOOLT);}
{BOOLFS}   {DIAG_RETURN(BOOLF);}

{SELCASS}  {DIAG_RETURN(SELECTCASE);}
{ENDSELS}  {DIAG_RETURN(EOS);}
{ENDIFS}   {DIAG_RETURN(EOS);}
{EOSS}     {DIAG_RETURN(EOS);}

{IDENT}    {DIAG_RETURN(SS_lookup_identifier_f((char *) yytext, &yylval));}

{WS}                    ;
{BEGCMT}.*{ENDCMT}      ;
{BEGCMT}                { BEGIN CMMNTS; }
<CMMNTS>.*"*/\n"        { BEGIN 0; }
<CMMNTS>.*\n            ;

.          {return(yytext[0]);}

%%
