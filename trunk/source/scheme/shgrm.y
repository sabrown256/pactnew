%{

/*
 * SHGRM.Y - grammar for BASIS syntax parsing in SCHEME
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                           DEFINITIONS SECTION                            */

/*--------------------------------------------------------------------------*/

#include "shm.h"

#ifdef ANSI
# define YY_USE_PROTOS
#endif

#define YYSTYPE     objp
#define YYDEBUG     1

#define shgrm_lex   shlrm_lex
#define shgrm_lval  shlrm_lval

#define yyerror(x)  SS_parse_error_synt(SI, x, SS_parse_token_m)

/* assign X to yyval and _SS_m_val because in bison yyval is
 * a local variable in yyparse whereas in yacc it is file static
 */

#define SS_GR_VAL(_x)  {yyval = _x; _SS_m_val = yyval;}

extern int
 yydebug;

static object
 *_SS_m_val = NULL;

/*--------------------------------------------------------------------------*/

/*                       END OF DEFINITIONS SECTION                         */

/*--------------------------------------------------------------------------*/

%}

%token TYPE_SPEC
%token NUMBER
%token INTEGER
%token FLOAT
%token STRING
%token FCOMSTR
%token VARNAME
%token COMMAND

%token IF
%token ELSE
%token ELSEIF
%token THEN
%token ENDIF
%token WHILE
%token ENDWHILE
%token UNTIL
%token DO
%token ENDDO
%token FOR
%token ENDFOR
%token NEXT
%token BREAK
%token RETURN
%token GOTO
%token CALL

%token EQ
%token NEQ
%token LT
%token LE
%token GE
%token GT
%token AND
%token OR
%token NOT
%token ASGN

%token BOOLT
%token BOOLF

%token SUBR
%token FUNC
%token ENDF

%token INPUTOP
%token OUTPUTOP

%token GROUPNAME
%token EXPT
%token WS
%token EOS

%left '+' '-'
%left '*' '/' '%'
%right UMINUS

%%

/*--------------------------------------------------------------------------*/

/* PARSE - parse a FORTRAN sequence */

parse :
    stmt eos
      {yylval = $1;
       DIAGNOSTIC(yylval, "parse stmt");
       YYACCEPT;}

  | funcspec
      {yylval = $1;
       DIAGNOSTIC(yylval, "parse funcspec");
       YYACCEPT;}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STLIST - from BASIS grammar */

stlist :
    stmt eos
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "stlist stmt");}

  | stlist stmt eos
      {if (SS_nullobjp($2))
	  {SS_GR_VAL(CAPTURE($1));}
       else if (SS_nullobjp($1))
	  {SS_GR_VAL(CAPTURE($2));}
       else if (SS_consp($1) && SS_consp(SS_CAR_MACRO($1)))
	  {SS_GR_VAL(SS_mk_cons(SI, $2, $1));}
       else
          {SS_GR_VAL(SS_mk_cons(SI, $2, SS_mk_cons(SI, $1, SS_null)));};
       DIAGNOSTIC($$, "stlist stlist");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NONNULLSTLIST - from BASIS grammar */

nonnullstlist :
    nonnullstmt eos stlist
      {SS_GR_VAL(SS_mk_cons(SI, $1, $3));
       DIAGNOSTIC($$, "nonnullstmt");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EOS - from BASIS grammar */

eos :
    EOS
      {DIAGNOSTIC(SS_null, "eos");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIMPLESTMT - from BASIS grammar */

simplestmt :
    constant ';'
      {DIAGNOSTIC($1, "constant");}

  | assign
      {DIAGNOSTIC($1, "assign");}

  | opassign
      {DIAGNOSTIC($1, "opassign");}

  | append
      {DIAGNOSTIC($1, "append");}

  | list
      {DIAGNOSTIC($1, "list");}

  | display
      {DIAGNOSTIC($1, "display");}

  | misc
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "misc");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCTSTMT - from BASIS grammar */

structstmt :
    dostmt
      {DIAGNOSTIC($1, "dostmt");}

  | forstmt
      {DIAGNOSTIC($1, "forstmt");}

  | whilestmt
      {DIAGNOSTIC($1, "whilestmt");}

  | ifstmt
      {DIAGNOSTIC($1, "ifstmt");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NONNULLSTMT - from BASIS grammar */

nonnullstmt :
    simplestmt
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "simplestmt");}

  | structstmt
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "structstmt");}

  | funcspec
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "funcspec");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STMT - from BASIS grammar */

stmt :
    nonnullstmt
      {DIAGNOSTIC($1, "stmt nonnullstmt");}

  | '{' '}'
      {SS_GR_VAL(SS_null);
       DIAGNOSTIC($$, "stmt {}");}  /* Zane doesn't even know */

  | 
      {SS_GR_VAL(SS_null);}

  | '^'
      {SS_GR_VAL(SS_null);
       DIAGNOSTIC($$, "stmt ^");}   /* used in macro processor */
  ;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION SYNTAX                               */

/*--------------------------------------------------------------------------*/

/* FUNCSPEC - function definition from BASIS grammar */

funcspec :
    FUNC funcdes eos stlist ENDF
      {SS_GR_VAL(_SS_make_fun_m(SI, $2, SS_reverse($4)));
       DIAGNOSTIC($$, "function");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FUNCDES - from BASIS grammar */

funcdes :
    dname
      {DIAGNOSTIC($$, "funcdes dname");}

  | dname '(' paramlist ')'
      {if (SS_consp($3))
          {SS_GR_VAL(SS_mk_cons(SI, $1, $3));}
       else
          {SS_GR_VAL(SS_mk_cons(SI, $1, SS_mk_cons(SI, $3, SS_null)));};
       DIAGNOSTIC($$, "funcdes dname(paramlist)");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARAMLIST - from BASIS grammar */

paramlist :
    VARNAME
      {DIAGNOSTIC($$, "VARNAME");}

  | eos VARNAME
      {DIAGNOSTIC($$, "eos VARNAME");}

  | paramlist eos VARNAME
      {DIAGNOSTIC($$, "paramlist eos VARNAME");}

  | paramlist ',' VARNAME
      {DIAGNOSTIC($$, "paramlist,VARNAME");}
  ;

/*--------------------------------------------------------------------------*/

/*                               DO SYNTAX                                  */

/*--------------------------------------------------------------------------*/

/* DOSTMT - do statement from BASIS grammar */

dostmt :
    DO dospec eos stlist ENDDO
      {object *body;

       body = _SS_make_comp_stmt_m(SI, $4);
       $$   = SS_append(SI,
			SS_mk_cons(SI, _SS_m_for, $2),
			SS_mk_cons(SI, body, SS_null));
		       
       DIAGNOSTIC($$, "do dospec eos stlist enddo");}

  | DO eos stlist docontrol
      {object *body;

       body = _SS_make_comp_stmt_m(SI, $3);
       $$   = SS_make_form(_SS_m_for, SS_null, SS_null, SS_null, body, 0);

       DIAGNOSTIC($$, "do eos stlist docontrol");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DOSPEC - do statement support from BASIS grammar */

dospec :
    lhs ASGN first ',' last incr
      {object *init, *test, *incr, *iter, *val;

       init = SS_make_form(_SS_m_set, $1, $3, 0);
       test = SS_make_form(_SS_m_loop_test, $3, $1, $5, 0);

       if (SS_nullobjp($6))
	  val = SS_mk_integer(SI, 1);
       else
	  val = $6;

       incr = SS_make_form(_SS_m_plus, $1, val, 0);
       iter = SS_make_form(_SS_m_set, $1, incr, 0);
       $$   = SS_make_form(init, test, iter, 0);

       DIAGNOSTIC($$, "dospec");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIRST - do statement support from BASIS grammar */

first :
    exp
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LAST - do statement support from BASIS grammar */

last :
    exp
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INCR - do statement support from BASIS grammar */

incr :
    ',' exp
      {SS_GR_VAL(CAPTURE($2));}

  |
      {SS_GR_VAL(SS_null);}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DOCONTROL - do statement support from BASIS grammar */

docontrol :
    ENDDO
      {SS_GR_VAL(SS_null);}

  | UNTIL '(' lexp ')'
      {SS_GR_VAL(CAPTURE($3));
       DIAGNOSTIC($$, "until");}
  ;

/*--------------------------------------------------------------------------*/

/*                               FOR SYNTAX                                 */

/*--------------------------------------------------------------------------*/

/* To avoid a serious LR-conflict, only the null statement or a list
 * of assignments may be included in the initialization of a "for."
 * This is not a serious restriction, since any other "initializations"
 * can be written as separate statements prior to the "for."
 */

/* FORSTMT - for statement from BASIS grammar */

forstmt :
    FOR '(' forinit ',' forout ',' cstmt ')' stlist ENDFOR
      {SS_GR_VAL(SS_make_form(_SS_m_for, _SS_make_comp_stmt_m(SI, $3),
			 $5, _SS_make_comp_stmt_m(SI, $7),
			 _SS_make_comp_stmt_m(SI, $9), 0));
       DIAGNOSTIC($$, "for");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORINIT - support for statment from BASIS grammar */

forinit :
    assign
      {SS_GR_VAL(CAPTURE($1));}

  | assign eos forinit
      {if (SS_consp(SS_CAR_MACRO($1)))
	  {SS_GR_VAL(SS_mk_cons(SI, $3, $1));}
       else
	  {SS_GR_VAL(SS_make_form($3, $1, 0));}
       DIAGNOSTIC($$, "forinit list");}

  |
      {SS_GR_VAL(SS_null);}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FOROUT - support for statment from BASIS grammar */

forout :
    lexp
      {SS_GR_VAL(CAPTURE($1));}

  | eos lexp
      {SS_GR_VAL(CAPTURE($2));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CSTMT - support for statment from BASIS grammar */

cstmt :
    stmt
      {SS_GR_VAL(CAPTURE($1));}

  | eos stmt
      {SS_GR_VAL(CAPTURE($2));}

  | cstmt eos stmt
      {if (SS_nullobjp($3))
	  {SS_GR_VAL(CAPTURE($1));}
       else if (SS_nullobjp($1))
	  {SS_GR_VAL(CAPTURE($3));}
       else if (SS_consp($1) && SS_consp(SS_CAR_MACRO($1)))
	  {SS_GR_VAL(SS_mk_cons(SI, $3, $1));}
       else
          {SS_GR_VAL(SS_mk_cons(SI, $3, SS_mk_cons(SI, $1, SS_null)));}

       DIAGNOSTIC($$, "cstmt stmt");}
  ;

/*--------------------------------------------------------------------------*/

/*                              WHILE SYNTAX                                */

/*--------------------------------------------------------------------------*/

/* WHILESTMT - support while statment from BASIS grammar */

whilestmt :
    WHILE '(' lexp ')' stlist ENDWHILE
      {SS_GR_VAL(SS_make_form(_SS_m_while,
			      SS_f, $3, _SS_make_comp_stmt_m(SI, $5), 0));
       DIAGNOSTIC($$, "whilestmt");}
  ;

/*--------------------------------------------------------------------------*/

/*                                IF SYNTAX                                 */

/*--------------------------------------------------------------------------*/

/* A logical IF must control a single non-null statement (which need
 * not be on the same line). It can not have an ELSE clause or an ENDIF.
 * It is distinguished from the structured IF by the absence of the
 * reserved word THEN.
 */

/* IFSTMT - if statement from BASIS grammar */

ifstmt :
    IF ifexp lalt
      {SS_GR_VAL(SS_make_form(_SS_m_if, $2, $3, 0));
       DIAGNOSTIC($$, "ifstmt");}

  | IF ifexp thenlist optelse ENDIF
      {if (SS_nullobjp($4))
	  {SS_GR_VAL(SS_make_form(_SS_m_if, $2, $3, 0));}
       else
	  {SS_GR_VAL(SS_make_form(_SS_m_if, $2, $3, $4, 0));};
       DIAGNOSTIC($$, "ifendif");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IFEXP - support if statement from BASIS grammar */

ifexp :
    '(' lexp ')'
      {SS_GR_VAL(CAPTURE($2));
       DIAGNOSTIC($$, "ifexp");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LALT - support if statement from BASIS grammar */

lalt :
     eos nonnullstmt
      {SS_GR_VAL(CAPTURE($2));
       DIAGNOSTIC($$, "lalt eos");}

  |  nonnullstmt
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "lalt");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* THENLIST - support if statement from BASIS grammar */

thenlist :
    eos THEN stlist
      {SS_GR_VAL(_SS_make_comp_stmt_m(SI, $3));
       DIAGNOSTIC($$, "then eos");}

  | THEN stlist
      {SS_GR_VAL(_SS_make_comp_stmt_m(SI, $2));
       DIAGNOSTIC($$, "then stlist");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPTELSE - support if statement from BASIS grammar
 *         - ELSEIF must be a single word. If spelled ELSE IF, it will
 *         - be interpreted as an ELSE clause containing a nested IF statement
 *         - (which is OK, if that's what the user wants).
 *         - THEN can be omitted after ELSEIF without penalty
 *         - except that the THEN clause must be non-null if it is. This is
 *         - done to avoid a very troublesome LR-conflict.
 */

optelse :
    ELSE stlist
      {SS_GR_VAL(_SS_make_comp_stmt_m(SI, $2));
       DIAGNOSTIC($$, "optelse else");}

  | ELSEIF ifexp alt2 optelse
      {if (SS_nullobjp($4))
	  {SS_GR_VAL(SS_make_form(_SS_m_if, $2, $3, 0));}
       else
	  {SS_GR_VAL(SS_make_form(_SS_m_if, $2, $3, $4, 0));};
       DIAGNOSTIC($$, "optelse elsif");}

  |
      {SS_GR_VAL(SS_null);
       DIAGNOSTIC($$, "optelse");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ALT2 - support if statement from BASIS grammar */

alt2 :
    nonnullstlist
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "alt2");}

  | eos nonnullstlist
      {SS_GR_VAL(CAPTURE($2));
       DIAGNOSTIC($$, "alt2 eos");}

  | eos THEN stlist
      {SS_GR_VAL(_SS_make_comp_stmt_m(SI, $3));
       DIAGNOSTIC($$, "alt2 eos then");}

  | THEN stlist
      {SS_GR_VAL(_SS_make_comp_stmt_m(SI, $2));
       DIAGNOSTIC($$, "alt2 then");}
  ;

/*--------------------------------------------------------------------------*/

/*                               MISC SYNTAX                                */

/*--------------------------------------------------------------------------*/

/* MISC - miscellaneous syntax from BASIS grammar */

misc :
    'read' filename
      {SS_GR_VAL(SS_mk_cons(SI, _SS_m_load, SS_mk_cons(SI, $2, SS_null)));
       DIAGNOSTIC($$, "misc read filename");}

  | 'forget' forgetlist
      {DIAGNOSTIC($1, "misc forget forgetlist");}

  | 'forget'
      {DIAGNOSTIC($1, "misc forget");}

  | CALL factor
      {SS_GR_VAL(_SS_strip_call(SI, $2, TRUE));

       DIAGNOSTIC($$, "misc call factor");}

  | id COMMAND comlist
      {if (SS_consp($3))
	  {SS_GR_VAL(SS_mk_cons(SI, $1, SS_reverse($3)));}
       else
	  {SS_GR_VAL(SS_mk_cons(SI, $1, SS_mk_cons(SI, $3, SS_null)));};
       DIAGNOSTIC($$, "misc id command comlist");}

  | scope type varlist
      {SS_GR_VAL(SS_mk_cons(SI, _SS_m_defvar, $3));
       DIAGNOSTIC($$, "misc scope type varlist");}

  | BREAK optlevel
      {SS_GR_VAL(SS_make_form(_SS_m_continue, SS_f, 0));
       DIAGNOSTIC($$, "misc break optlevel");}

  | NEXT optlevel
      {SS_GR_VAL(SS_make_form(_SS_m_continue, SS_t, 0));
       DIAGNOSTIC($$, "misc next optlevel");}

  | indevice inlist
      {object *lst;
       SS_psides *si;

       si = SI;

       if (SS_consp($2) && !SS_procedurep(SS_eval(si, SS_CAR_MACRO($2))))
	  lst = SS_reverse($2);
       else
	  lst = SS_mk_cons(SI, $2, SS_null);
	 
       SS_GR_VAL(SS_make_form(_SS_m_input, $1, lst, 0));
       DIAGNOSTIC($$, "misc indevice inlist");}

  | outdevice outlist
      {object *lst;
       SS_psides *si;

       si = SI;

       if (SS_consp($2) && !SS_procedurep(SS_eval(si, SS_CAR_MACRO($2))))
	  lst = SS_mk_cons(SI, _SS_m_list, SS_reverse($2));
       else
	  lst = SS_mk_cons(SI, _SS_m_list, SS_mk_cons(SI, $2, SS_null));
	 
       SS_GR_VAL(SS_make_form(_SS_m_output, $1, lst, 0));
       DIAGNOSTIC($$, "misc outdevice outlist");}

  | RETURN optval
      {SS_GR_VAL(CAPTURE($2));
       DIAGNOSTIC($2, "misc return optval");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCOPE - miscellaneous syntax from BASIS grammar */

scope :
    dname
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "scope dname");}

  |
      {SS_GR_VAL(SS_null);
       DIAGNOSTIC($$, "scope");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INDEVICE - miscellaneous syntax from BASIS grammar */

indevice :

      {SS_GR_VAL(SS_null);
       DIAGNOSTIC($$, "indevice");}

  | exp
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "indevice exp");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OUTDEVICE - miscellaneous syntax from BASIS grammar */

outdevice :
    exp
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($1, "outdevice exp");}

  |
      {SS_GR_VAL(SS_null);
       DIAGNOSTIC($$, "outdevice");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OUTLIST - miscellaneous syntax from BASIS grammar */

outlist :
    OUTPUTOP lexp
      {SS_GR_VAL(CAPTURE($2));
       DIAGNOSTIC($$, "outlist << lexp");}

  | OUTPUTOP RETURN
      {DIAGNOSTIC($1, "outlist << return");}

  | outlist OUTPUTOP lexp
      {if (SS_consp($1))
	  {if (SS_consp(SS_CAR_MACRO($1)))
	      {SS_GR_VAL(SS_mk_cons(SI, $3, SS_mk_cons(SI, $1, SS_null)));}
	   else
	      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));};}
       else
	  {SS_GR_VAL(SS_mk_cons(SI, $3, SS_mk_cons(SI, $1, SS_null)));};

       DIAGNOSTIC($$, "outlist outlist << lexp");}

  | outlist OUTPUTOP RETURN
      {DIAGNOSTIC($1, "outlist outlist << return");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INLIST - miscellaneous syntax from BASIS grammar */

inlist :
    INPUTOP lhs
      {SS_GR_VAL(CAPTURE($2));
       DIAGNOSTIC($$, "inlist >>");}

  | INPUTOP RETURN
      {DIAGNOSTIC($1, "inlist >> return");}

  | inlist INPUTOP lhs
      {if (SS_consp($1))
	  {if (SS_consp(SS_CAR_MACRO($1)))
	      {SS_GR_VAL(SS_mk_cons(SI, $3, SS_mk_cons(SI, $1, SS_null)));}
	   else
	      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));};}
       else
	  {SS_GR_VAL(SS_mk_cons(SI, $3, SS_mk_cons(SI, $1, SS_null)));};

       DIAGNOSTIC($$, "inlist inlist >>");}

  | inlist INPUTOP RETURN
      {DIAGNOSTIC($1, "inlist inlist >> return");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASSIGN - miscellaneous syntax from BASIS grammar */

assign :
    lhs assigneq lexp
      {SS_GR_VAL(SS_make_form(_SS_m_set, _SS_strip_call(SI, $1, FALSE), $3, 0));
       DIAGNOSTIC($$, "lhs assigneq lexp");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPASSIGN - miscellaneous syntax from BASIS grammar */

opassign :
    lhs '|=' lexp
      {DIAGNOSTIC($1, "lhs |= lexp");}

/* NOTE: yacc is happy with these, bison is not
 * none of this is tested so we can't tell who to believe
  | lhs '&=' lexp
      {DIAGNOSTIC($1, "lhs &= lexp");}

  | lhs '+=' lexp
      {DIAGNOSTIC($1, "lhs += lexp");}

  | lhs '-=' lexp
      {DIAGNOSTIC($1, "lhs -= lexp");}

  | lhs '*=' lexp
      {DIAGNOSTIC($1, "lhs *= lexp");}

  | lhs '/=' lexp
      {DIAGNOSTIC($1, "lhs /= lexp");}

  | lhs '**=' lexp
      {DIAGNOSTIC($1, "lhs **= lexp");}
*/
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASSIGNEQ - miscellaneous syntax from BASIS grammar */

assigneq :
    ASGN
      {DIAGNOSTIC($1, "assigneq =");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* APPEND - miscellaneous syntax from BASIS grammar */

append :
    lhs appendeq lexp
      {DIAGNOSTIC($1, "append lhs:=lexp");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* APPENDEQ - miscellaneous syntax from BASIS grammar */

appendeq :
    ':='
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LIST - miscellaneous syntax from BASIS grammar */

list :
    'list'
      {DIAGNOSTIC($1, "list");}

  | 'list' listspec
      {DIAGNOSTIC($1, "list listspec");}
  ;

/*--------------------------------------------------------------------------*/

/*                      STATEMENT AUXILLIARY SYNTAX                         */

/*--------------------------------------------------------------------------*/

/* OPTLEVEL - auxilliary support from BASIS grammar */

optlevel :
    level
      {DIAGNOSTIC($1, "optlevel level");}

  |
      {DIAGNOSTIC($$, "optlevel");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LEVEL - auxilliary support from BASIS grammar */

level :
    INTEGER
      {DIAGNOSTIC($1, "level integer");}

  | '(' INTEGER ')'
      {DIAGNOSTIC($1, "level (integer)");}
  ;

/*--------------------------------------------------------------------------*/

/*                               LIST SYNTAX                                */

/*--------------------------------------------------------------------------*/

/* VARLIST - list support from BASIS grammar
 *         - this covers variable declarations
 *         - including dimensions and initializations
 */

varlist :
    dvar
      {SS_GR_VAL(SS_mk_cons(SI, _SS_strip_call(SI, $1, FALSE), SS_null));
       DIAGNOSTIC($$, "varlist dvar");}

  | dvar ASGN lexp
      {SS_GR_VAL(SS_mk_cons(SI, SS_make_form($1, $3, 0),
		       SS_null));
       DIAGNOSTIC($$, "varlist dvar = lexp");}

  | varlist ',' dvar
      {SS_GR_VAL(SS_mk_cons(SI, _SS_strip_call(SI, $3, FALSE), $1));
       DIAGNOSTIC($$, "varlist varlist, dvar");}

  | varlist ',' dvar ASGN lexp
      {SS_GR_VAL(SS_mk_cons(SI, SS_make_form($3, $5, 0), $1));
       DIAGNOSTIC($$, "varlist varlist, dvar = lexp");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DISPLAY - list support from BASIS grammar */

display :
    ditem
      {if (SS_procedurep($1))   /* not the right test */
	  {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));}
       else
          {SS_GR_VAL(CAPTURE($1));};

       DIAGNOSTIC($$, "display ditem");}

  | display ',' ditem
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));
       DIAGNOSTIC($$, "display display,ditem");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FORGETLIST - list support from BASIS grammar */

forgetlist :
    id
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($1, "forgetlist id");}

  | forgetlist ',' id
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));
       DIAGNOSTIC($1, "forgetlist forgetlist,id");}
  ;

/*--------------------------------------------------------------------------*/

/*                             OTHER DEFINITIONS                            */

/*--------------------------------------------------------------------------*/

/* DVAR - definitions supporting BASIS grammar
 *      - this is for variable declarations only
 */

dvar :
    dname
      {SS_GR_VAL(CAPTURE($1));}

  | dname args
      {if (SS_consp($1))
	  {SS_GR_VAL(SS_append(SI, $1, SS_mk_cons(SI, $2, SS_null)));}
       else
	  {SS_GR_VAL(SS_make_form($1, SS_mk_cons(SI, _SS_m_defarr, $2), 0));};

       DIAGNOSTIC($1, "dvar dname args");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DNAME - definitions supporting BASIS grammar */

dname :
    VARNAME
      {SS_GR_VAL(CAPTURE($1));}

  | '`' rterm '`'
      {DIAGNOSTIC($2, "dname `rterm`");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DITEM - definitions supporting BASIS grammar */

ditem :
    exp
      {SS_GR_VAL(CAPTURE($1));}

  | group
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LISTSPEC - definitions supporting BASIS grammar */

listspec :
    listelt
      {DIAGNOSTIC($1, "listspec listelt");}

  | listspec ',' listelt
      {DIAGNOSTIC($1, "listspec listspec,listelt");}

  | listspec listelt
      {DIAGNOSTIC($1, "listspec listspec listelt");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LISTELT - definitions supporting BASIS grammar */

listelt :
    id
      {DIAGNOSTIC($1, "listelt id");}

  | group
      {DIAGNOSTIC($1, "listelt group");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPTVAL - definitions supporting BASIS grammar */

optval :
    lexp
      {DIAGNOSTIC($1, "optval lexp");}

  |
      {DIAGNOSTIC($$, "optval");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TYPE - types from BASIS grammar */

type :
    TYPE_SPEC
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($1, "TYPE-SPEC");}

/*
    'integer'
      {DIAGNOSTIC($1, "integer");}

  | 'real'
      {DIAGNOSTIC($1, "real");}

  | 'logical'
      {DIAGNOSTIC($1, "logical");}

  | 'complex'
      {DIAGNOSTIC($1, "complex");}

  | 'double complex'
      {DIAGNOSTIC($1, "double complex");}

  | 'double'
      {DIAGNOSTIC($1, "double");}

  | 'chameleon'
      {DIAGNOSTIC($1, "chameleon");}

  | 'character *' rterm
      {DIAGNOSTIC($1, "character*");}

  | 'character'
      {DIAGNOSTIC($1, "character");}

  | 'range'
      {DIAGNOSTIC($1, "range");}

  | 'real8'
      {DIAGNOSTIC($1, "real8");}

  | 'complex8'
      {DIAGNOSTIC($1, "complex8");}

  | 'indirect'
      {DIAGNOSTIC($1, "indirect");}
*/
  ;

/*--------------------------------------------------------------------------*/

/*                             EXPRESSION SYNTAX                            */

/*--------------------------------------------------------------------------*/

/* note that by having a separate set of productions
 * for logical expressions, we not only impose the
 * correct operator precedences (logicals, then
 * relationals, then arithmetic), but also make
 * certain type restrictions implicit in the grammar.
 */

/* EXPLIST - term from BASIS grammar */

explist :
    lexp
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));}

  | lexp ',' explist
      {SS_GR_VAL(SS_mk_cons(SI, $1, $3));
       DIAGNOSTIC($$, "explist , lexp");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LEXP - term from BASIS grammar */

lexp :
    lexp OR lterm
      {SS_GR_VAL(SS_make_form(_SS_m_or, $1, $3, 0));
       DIAGNOSTIC($$, "lexp .or. lterm");}

  | lterm
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LTERM - term from BASIS grammar */

lterm :
    lterm AND lprimary
      {SS_GR_VAL(SS_make_form(_SS_m_and, $1, $3, 0));
       DIAGNOSTIC($$, "lterm .and. lprimary");}

  | lprimary
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LPRIMARY - term from BASIS grammar */

lprimary :
    lfactor
      {SS_GR_VAL(CAPTURE($1));}

  | NOT lfactor
      {SS_GR_VAL(SS_make_form(_SS_m_not, $2, 0));
       DIAGNOSTIC($$, "lprimary ~ lfactor");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LFACTOR - term from BASIS grammar */

lfactor :
    exp
      {SS_GR_VAL(CAPTURE($1));}

  | exp LT exp
      {SS_GR_VAL(SS_make_form(_SS_m_lt, $1, $3, 0));
       DIAGNOSTIC($$, "lfactor exp LT exp");}

  | exp LE exp
      {SS_GR_VAL(SS_make_form(_SS_m_le, $1, $3, 0));
       DIAGNOSTIC($$, "lfactor exp LE exp");}

  | exp GT exp
      {SS_GR_VAL(SS_make_form(_SS_m_gt, $1, $3, 0));
       DIAGNOSTIC($$, "lfactor exp GT exp");}

  | exp GE exp
      {SS_GR_VAL(SS_make_form(_SS_m_ge, $1, $3, 0));
       DIAGNOSTIC($$, "lfactor exp GE exp");}

  | exp EQ exp
      {SS_GR_VAL(SS_make_form(_SS_m_equal, $1, $3, 0));
       DIAGNOSTIC($$, "lfactor exp EQ exp");}

  | exp NEQ exp
      {SS_GR_VAL(SS_make_form(_SS_m_not, SS_make_form(_SS_m_equal, $1, $3, 0)));
       DIAGNOSTIC($$, "lfactor exp NEQ exp");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXP - term from BASIS grammar */

exp :
    rterm
      {SS_GR_VAL(CAPTURE($1));}

/* NOTE: yacc is happy with these, bison is not
 * none of this is tested so we can't tell who to believe
  | ':'
      {SS_GR_VAL(SS_make_form(SS_null, SS_null, 0));
       DIAGNOSTIC($$, "exp :");}

  | rexp ':' rexp
      {SS_GR_VAL(SS_make_form($1, $3, 0));
       DIAGNOSTIC($$, "exp rexp:rexp");}

  | rexp ':' rexp ':' rexp
      {SS_GR_VAL(SS_make_form($1, $3, $5, 0));
       DIAGNOSTIC($$, "exp rexp:rexp:rexp");}
*/
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REXP - term from BASIS grammar */

rexp :
    rterm
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RTERM - term from BASIS grammar */

rterm :
    rterm '+' term
      {SS_GR_VAL(SS_make_form(_SS_m_plus, $1, $3, 0));
       DIAGNOSTIC($$, "rterm + term");}

  | rterm '-' term
      {SS_GR_VAL(SS_make_form(_SS_m_minus, $1, $3, 0));
       DIAGNOSTIC($$, "rterm - term");}

  | '+' term
      {SS_GR_VAL(SS_make_form(_SS_m_plus, $2, 0));
       DIAGNOSTIC($$, "rterm +term");}

  | '-' term
      {SS_GR_VAL(SS_make_form(_SS_m_minus, $2, 0));
       DIAGNOSTIC($$, "rterm -term");}

  | term
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TERM - term from BASIS grammar */

term :
    term '*' primry
      {SS_GR_VAL(SS_make_form(_SS_m_times, $1, $3, 0));
       DIAGNOSTIC($$, "term * primry");}

  | term '*' '!' primry
      {DIAGNOSTIC($$, "term *! primry");}

  | term '/' primry
      {SS_GR_VAL(SS_make_form(_SS_m_divide, $1, $3, 0));
       DIAGNOSTIC($$, "term / primry");}

  | term '!' primry
      {DIAGNOSTIC($$, "term ! primry");}

  | term '/' '/' primry
      {DIAGNOSTIC($$, "term // primry");}

  | term '/' '!' primry
      {DIAGNOSTIC($$, "term /! primry");}

  | primry
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRIMRY - primry from BASIS grammar */

primry :
    factor EXPT primry
      {SS_GR_VAL(SS_make_form(_SS_m_expt, $1, $3, 0));
       DIAGNOSTIC($$, "primry factor **");}

  | factor
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FACTOR - factor from BASIS grammar */

factor :
    lhs
      {SS_GR_VAL(CAPTURE($1));}

  | constant
      {SS_GR_VAL(CAPTURE($1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LHS - lhs from BASIS grammar */

lhs :
    id
      {SS_GR_VAL(CAPTURE($1));}

  | '(' lexp ')'
      {SS_GR_VAL(CAPTURE($2));}

  | lhs args
      {SS_GR_VAL(SS_make_form(_SS_m_arr_call, $1, $2, 0));
       DIAGNOSTIC($$, "lhs args");}

  | '[' explist ']'
      {SS_GR_VAL(CAPTURE($2));
       DIAGNOSTIC($$, "lhs [explist]");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONSTANT - constants from BASIS grammar */
/*
constant :
    string
    real-constant
    double-constant
    complex-constant
    double-complex-constant
    integer-constant
    hex-constant
    octal-constant
*/

constant :
    INTEGER
      {DIAGNOSTIC($1, "INTEGER");}

  | STRING
      {DIAGNOSTIC($1, "STRING");}

  | FLOAT
      {DIAGNOSTIC($1, "FLOAT");}
  ;

/*--------------------------------------------------------------------------*/

/*                            ARGUMENT SYNTAX                               */

/*--------------------------------------------------------------------------*/

/* ARGS - argument support from BASIS grammar */

args :
    '(' arglist ')'
      {SS_GR_VAL(CAPTURE($2));
       DIAGNOSTIC($$, "args (arglist)");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ARGLIST - argument support from BASIS grammar */

arglist :
    argitem
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
/*       DIAGNOSTIC($$, "arglist argitem"); */}

  | argitem ',' arglist
      {SS_GR_VAL(SS_mk_cons(SI, $1, $3));
/*       DIAGNOSTIC($$, "arglist argitem,arglist"); */}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMLIST - command support from BASIS grammar */

comlist :
    comitem
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "comlist comitem");}

  | comitem ',' comlist
      {SS_GR_VAL(SS_mk_cons(SI, $1, $3));
       DIAGNOSTIC($$, "comlist comitem,comlist");}

  | comitem '@' comlist
      {SS_GR_VAL(SS_mk_cons(SI, $1, $3));
       DIAGNOSTIC($$, "comlist comitem@comlist");}

  | comitem WS comlist
      {SS_GR_VAL(SS_mk_cons(SI, $1, $3));
       DIAGNOSTIC($$, "comlist comitem WS comlist");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMITEM - argument support from BASIS grammar */

comitem :
    filename
      {DIAGNOSTIC($$, "comitem filename");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FILENAME - argument support from BASIS grammar */

filename :
    FCOMSTR
      {DIAGNOSTIC($$, "filename fcomstr");}

  | argitem
      {DIAGNOSTIC($$, "filename argitem");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ARGITEM - argument support from BASIS grammar */

argitem :
    dexp
      {SS_GR_VAL(CAPTURE($1));
/*       DIAGNOSTIC($$, "argitem dexp"); */}

  | '&' lhs
      {DIAGNOSTIC($$, "argitem & lhs");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEXP - argument support from BASIS grammar */

dexp :

      {DIAGNOSTIC($$, "dexp");}

  | lexp
      {SS_GR_VAL(CAPTURE($1));
/*       DIAGNOSTIC($$, "dexp lexp"); */}
  ;

/*--------------------------------------------------------------------------*/

/*                            GROUP/ID SYNTAX                               */

/*--------------------------------------------------------------------------*/

/* ID - id from BASIS grammar */

id :
    dname
      {SS_GR_VAL(CAPTURE($1));
/*       DIAGNOSTIC($$, "id dname"); */}

  | dname '.' dname
      {DIAGNOSTIC($$, "id dname.dname");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GROUP - group from BASIS grammar */

group :
    GROUPNAME
      {DIAGNOSTIC($$, "group Groupname");}

  | dname '.' GROUPNAME
      {DIAGNOSTIC($$, "group dname . Groupname");}
  ;

/*--------------------------------------------------------------------------*/

%%

/*--------------------------------------------------------------------------*/

/*                             EXTRAS SECTION                               */

/*--------------------------------------------------------------------------*/

#undef YYACCEPT

int
 SS_m_tokens[] = { TYPE_SPEC, VARNAME, GROUPNAME };

#include "shlrm.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_TOKEN_M - return YYVAL portably */

object *SS_parse_token_m()
   {

    return(_SS_m_val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_TOKEN_VAL_M - return YYLVAL portably */

object *SS_parse_token_val_m()
   {

    return(yylval);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_DEBUG_M - return a pointer to YYDEBUG portably */

int *SS_parse_debug_m()
   {

    return(&yydebug);}

/*--------------------------------------------------------------------------*/

/*                         END OF EXTRAS SECTION                            */

/*--------------------------------------------------------------------------*/

