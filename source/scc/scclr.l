%{

/*
 * SCCLR.L - lexical rules for C language for SCC
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
#define yywrap CC_wrap_input_c

#define DIAG_RETURN(x)                                                       \
    return(_CC_diagnose_return_c(x, #x, CC_parse_debug_c))

#define IDENT_RETURN(x)                                                      \
    return(_CC_diagnose_return_c(x, "IDENTIFIER", CC_parse_debug_c))

#ifdef FLEX_SCANNER

#undef YY_INPUT
#define YY_INPUT(bf, res, szx)                                               \
   {int c;                                                                   \
    c     = CC_input_c((char *) yytext);                                     \
    res   = (c != EOF);                                                      \
    bf[0] = (char) c;}

#else

#undef input
#undef unput

#define input()  CC_input_c((char *) yytext)
#define unput(c) CC_unput_c(c)

#endif

/*--------------------------------------------------------------------------*/

/*                       END OF DEFINITIONS SECTION                         */

/*--------------------------------------------------------------------------*/

/*
 * OD   - octal digits
 * DD   - decimal digits
 * HD   - hexadecimal digits
 *
 * WS   - "all" whitespace
 * BLNK - space and tabs
 * NLS  - newlines
 * ENLS - escaped newline
 *
 */

%}

OD            [0-7]
DD            [0-9]
HD            [0-9a-fA-F]

WS            [ \t\n\r\f]
BLNK          [ \t]
NLS           [\n]
ENLS          \\\n
QUT           \"

EXP           [Ee]("+"|"-")?{DD}+
OCT_INTEGER   0{OD}+
HEX_INTEGER   0x{HD}+
DEC_INTEGER   {DD}+(L)?

CHR_CONST     '.'|'\\.'|'\\{OD}{OD}{OD}'

STOR_CLAS     (auto|register|static|extern|typedef)
TYPE_QUALS    (const|volatile)

EXTENS        (__const|__restrict|__inline__|__inline|__extension__|__builtin_va_list)
ATTRS         __attribute__\ \([ \t]*\(.*\)[ \t]*\)
ASMS          __asm__\ \(.*\)

TYPES         [a-zA-Z_][a-zA-Z0-9_]*{WS}*[\*]+
IDENT         [a-zA-Z_][a-zA-Z0-9_]*
DERS          #[a-zA-Z0-9_@;/ \t\"\'\-\(\)\[\]\{\}\<\>\~\.\+\%\^\&\*\!\?\:]*
DOTS          \.

EQS           ==
NEQS          !=
LES           <=
GES           >=
ANDS          &&
ORS           \|\|

RSS           >>
LSS           <<

ACCS          ->
INCS          \+\+
DECS          --

PLUSEQS       \+=
MINUSEQS      \-=
TIMESEQS      \*=
DIVEQS        \/=
MODEQS        \%=

RSEQS         >>=
LSEQS         <<=

ANDEQS        \&=
OREQS         \|=
XOREQS        \^=

CAROTS        \^

%s CMMNTS
BEGCMT        {WS}*"/*"
ENDCMT        "*/"

%n 1000
%p 6000
%a 5000

%%

include        {DIAG_RETURN(INCLUDE);}
define         {_CC.cpp_define = TRUE;
                DIAG_RETURN(DEFINE);}
undef          {DIAG_RETURN(UNDEF);}
ifdef          {DIAG_RETURN(IFDEF);}
ifndef         {DIAG_RETURN(IFNDEF);}
endif          {DIAG_RETURN(ENDIF);}
pragma         {_CC.cpp_define = TRUE;
                DIAG_RETURN(PRAGMA);}

if             {DIAG_RETURN(IF);}
else           {DIAG_RETURN(ELSE);}
while          {DIAG_RETURN(WHILE);}
do             {DIAG_RETURN(DO);}
for            {DIAG_RETURN(FOR);}
switch         {DIAG_RETURN(SWITCH);}
case           {DIAG_RETURN(CASE);}
default        {DIAG_RETURN(DEFAULT);}
continue       {DIAG_RETURN(CONTINUE);}
break          {DIAG_RETURN(BREAK);}
return         {DIAG_RETURN(RETURN);}
goto           {DIAG_RETURN(GOTO);}
sizeof         {DIAG_RETURN(SIZOF);}

struct         {DIAG_RETURN(STRUCT);}
union          {DIAG_RETURN(UNION);}
enum           {DIAG_RETURN(ENUM);}
\.\.\.         {DIAG_RETURN(DOTDOT);}

{OCT_INTEGER}  {yylval = CC_mk_integer(STRTOL(yytext, NULL, 8));
                DIAG_RETURN(INTEGER);}

{DEC_INTEGER}  {yylval = CC_mk_integer(STRTOL(yytext, NULL, 10));
                DIAG_RETURN(INTEGER);}

{HEX_INTEGER}  {yylval = CC_mk_integer(STRTOL(yytext, NULL, 10));
                DIAG_RETURN(INTEGER);}

{CHR_CONST}    {yylval = CC_mk_char(yytext);
                DIAG_RETURN(INTEGER);}

{DD}+\.?{DD}*({EXP})? |
{DD}*\.?{DD}+({EXP})?      {yylval = CC_mk_float(STRTOD(yytext, NULL));
                            DIAG_RETURN(FLOAT);}

{QUT}          {yylval = CC_parse_string_c();
                DIAG_RETURN(STRING);}

{DERS}         {CC_reckon_line();}
{NLS}          {if (_CC.cpp_directive == TRUE)
                   {_CC.cpp_directive = FALSE;
                    DIAG_RETURN(NL);};}
{ENLS}         {}

{ATTRS}        {}
{ASMS}         {}
{EXTENS}       {}

{RSS}          {DIAG_RETURN(RS);}
{LSS}          {DIAG_RETURN(LS);}

{DOTS}         {DIAG_RETURN(DOT);}
{ACCS}         {DIAG_RETURN(ACC);}
{INCS}         {DIAG_RETURN(INC);}
{DECS}         {DIAG_RETURN(DEC);}

{EQS}          {DIAG_RETURN(EQ);}
{NEQS}         {DIAG_RETURN(NEQ);}
{LES}          {DIAG_RETURN(LE);}
{GES}          {DIAG_RETURN(GE);}
{ORS}          {DIAG_RETURN(OR);}
{ANDS}         {DIAG_RETURN(AND);}

{PLUSEQS}      {DIAG_RETURN(PLUSEQ);}
{MINUSEQS}     {DIAG_RETURN(MINUSEQ);}
{TIMESEQS}     {DIAG_RETURN(TIMESEQ);}
{DIVEQS}       {DIAG_RETURN(DIVEQ);}
{MODEQS}       {DIAG_RETURN(MODEQ);}

{RSEQS}        {DIAG_RETURN(RSEQ);}
{LSEQS}        {DIAG_RETURN(LSEQ);}

{ANDEQS}       {DIAG_RETURN(ANDEQ);}
{OREQS}        {DIAG_RETURN(OREQ);}
{XOREQS}       {DIAG_RETURN(XOREQ);}

{CAROTS}       {DIAG_RETURN(XOR);}

{TYPE_QUALS}   {char func[MAXLINE];
                sprintf(func, "%s", (char *) yytext);
                yylval = CC_lookup_variable(func, CC_TYPEQ, TRUE);
                DIAG_RETURN(TYPE_QUALIFIER);}

{STOR_CLAS}    {char func[MAXLINE];
                sprintf(func, "%s", (char *) yytext);
                yylval = CC_lookup_variable(func, CC_STORAGE, TRUE);
                DIAG_RETURN(STORAGE_CLASS);}

{TYPES}        {IDENT_RETURN(CC_lookup_identifier_c((char *) yytext, &yylval));}
{IDENT}        {IDENT_RETURN(CC_lookup_identifier_c((char *) yytext, &yylval));}

{WS}                    ;
{BEGCMT}.*{ENDCMT}      ;
{BEGCMT}                { BEGIN CMMNTS; }
<CMMNTS>.*"*/\n"        { BEGIN 0; }
<CMMNTS>.*\n            ;

.              {return(yytext[0]);}

%%
