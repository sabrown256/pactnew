%{

/*
 * SHGRF.Y - grammar for pseudo Fortran syntax parsing in SCHEME
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                           DEFINITIONS SECTION                            */

/*--------------------------------------------------------------------------*/

#include "shf.h"

#ifdef ANSI
#define YY_USE_PROTOS
#endif

#define YYSTYPE     objp
#define YYDEBUG     1

#define shgrf_lex   shlrf_lex
#define shgrf_lval  shlrf_lval

#define yyerror(x)  SS_parse_error_synt(SI, x, SS_parse_token_f)

/* assign X to yyval and _SS_f_val because in bison yyval is
 * a local variable in yyparse whereas in yacc it is file static
 */

#define SS_GR_VAL(_x)  {yyval = _x; _SS_f_val = yyval;}

extern int
 yydebug;

static object
 *_SS_f_val = NULL;

/*--------------------------------------------------------------------------*/

/*                       END OF DEFINITIONS SECTION                         */

/*--------------------------------------------------------------------------*/

%}

%token FIX_REDUCE

%token NUMBER
%token INTEGER
%token FLOAT
%token STRING
%token VARNAME

%token STORAGE_CLASS
%token EXTERN

%token TYPE_SPEC
%token TYPEDEF

%token IF
%token ELSE
%token WHILE
%token DO
%token FOR
%token SWITCH
%token CASE
%token DEFAULT
%token CONTINUE
%token BREAK
%token RETURN
%token GOTO
%token SIZOF

%token EQ
%token NEQ
%token LT
%token LE
%token GE
%token GT
%token AND
%token OR

%token BOOLT
%token BOOLF

%token SUBR
%token FUNC
%token SELECTCASE
%token THEN
%token EOS

%token EXPT

%token RS
%token LS

%left '+' '-'
%left '*' '/' '%'
%right UMINUS

%%

/*--------------------------------------------------------------------------*/

/* SEQUENCE - map a Fortran-style block into a SCHEME let*
 *          - i.e. roughly speaking
 *          -    {<decls>; statement; ... } -> (let* (<locals>) expr ... )
 *          - single statements go over without the block
 *          -    statement;  ->  expr
 */

sequence :

    statement
      {yylval = $1;
       DIAGNOSTIC(yylval, "statement");
       printf("\n");
       YYACCEPT;}

  | external_declaration
      {yylval = $1;
       DIAGNOSTIC(yylval, "external-declaration");
       printf("\n");
       YYACCEPT;}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXTERNAL_DECLARATION - from ANSI F90 Appendix D */

external_declaration :
    function_definition
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "function-definition");}

  | declaration
      {SS_GR_VAL(SS_mk_cons(SI, _SS_f_defvar, $1));
       DIAGNOSTIC($$, "declaration");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FUNCTION_DEFINITION - from ANSI F90 Appendix D */

function_definition :
    SUBR declarator EOS compound_statement
      {SS_GR_VAL(_SS_make_funf(SI, $2, $4));
       DIAGNOSTIC($$, "SUBR");}

  | declaration_specifiers FUNC declarator EOS compound_statement
      {SS_GR_VAL(_SS_make_funf(SI, $2, $4));
       DIAGNOSTIC($$, "FUNC");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATION - from ANSI F90 Appendix D */

declaration :
    declaration_specifiers init_declarator_list ';'
      {if (SS_consp($1))
          {object *op, *type;

           op   = SS_CAR_MACRO($1);
           type = SS_cadr($1);
           if (op == _SS_f_typedef)
              {SS_GR_VAL(SS_make_form(SI, _SS_f_typedef,
				      SS_CAR_MACRO($2), type, LAST));}
	   else
	      {SS_GR_VAL(SS_append(SI, $1, $2));};}

       else
	  {SS_GR_VAL($2);};

       DIAGNOSTIC($$, "declaration-specifiers init-declarator-list ;");}

  | declaration_specifiers ';'
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "declaration-specifiers ;");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATION_LIST - from ANSI F90 Appendix D */

declaration_list :
    declaration
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "declaration");}

  | declaration_list declaration
      {SS_GR_VAL(SS_append(SI, $2, $1));
       DIAGNOSTIC($$, "declaration-list declaration");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATION_SPECIFIERS - from ANSI F90 Appendix D */

declaration_specifiers :
    storage_class_specifier
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($1, "storage-class-specifier");}

  | storage_class_specifier declaration_specifiers
      {SS_GR_VAL(SS_mk_cons(SI, $1, $2));
       DIAGNOSTIC($$, "storage-class-specifier declaration-specifiers");}

  | type_specifier
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($1, "type-specifier");}

  | type_specifier declaration_specifiers
      {SS_GR_VAL(SS_mk_cons(SI, $1, $2));
       DIAGNOSTIC($$, "type-specifier declaration-specifiers");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STORAGE_CLASS_SPECIFIER - from ANSI F90 Appendix D */

storage_class_specifier :
    STORAGE_CLASS
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($1, "STORAGE CLASS");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TYPE_SPECIFIER - from ANSI F90 Appendix D */

type_specifier :
    TYPE_SPEC
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($1, "TYPE-SPEC");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_DECLARATOR_LIST - from ANSI F90 Appendix D */

init_declarator_list :
    init_declarator
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));

       DIAGNOSTIC($$, "init-declarator");}

  | init_declarator_list ',' init_declarator
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));
       DIAGNOSTIC($$, "init-declarator , init-declarator");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_DECLARATOR - from ANSI F90 Appendix D */

init_declarator :
    declarator
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "declarator");}

  | declarator '=' initializer
      {SS_GR_VAL(SS_make_form(SI, $1, $3, LAST));
       DIAGNOSTIC($$, "declarator = initializer");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATOR - from ANSI F90 Appendix D */

declarator :
    VARNAME
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "VARNAME");}

  | declarator '(' identifier_list ')'
      {object *args;

       args = SS_reverse(SI, $3);
       if (SS_consp($1))
	  {object *f;

           f = SS_CAR_MACRO($1);
	   if (SS_consp(f))
	      {SS_GR_VAL(SS_mk_cons(SI, SS_append(SI, f, args), SS_null));}
	   else
	      {SS_GR_VAL(SS_append(SI, $1, args));};}
       else
	  {SS_GR_VAL(SS_mk_cons(SI, $1, args));};

       DIAGNOSTIC($$, "declarator ( identifier-list )");}

  | declarator '(' ')'
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($$, "declarator ()");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IDENTIFIER_LIST - from ANSI F90 Appendix D */

identifier_list :
    VARNAME
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($$, "VARNAME");}

  | identifier_list ',' VARNAME
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));
       DIAGNOSTIC($$, "identifier-list , VARNAME");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INITIALIZER - from ANSI F90 Appendix D */

initializer :
    assignment_expression
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "assignment-expression");}

  | '{' initializer_list '}'
      {SYNTAX_ERR("{ init-list }");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INITIALIZER_LIST - from ANSI F90 Appendix D */

initializer_list :
    initializer
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($$, "initializer");}

  | initializer_list ',' initializer
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));
       DIAGNOSTIC($$, "initializer-list , initializer");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STATEMENT - from ANSI F90 Appendix D */

statement :
    expression_statement
      {DIAGNOSTIC($1, "expression-statement");}

  | compound_statement
      {DIAGNOSTIC($1, "compound-statement");}

  | IF '(' expression ')' statement
      {SS_GR_VAL(SS_make_form(SI, _SS_f_if, $3, $5, LAST));}

  | IF '(' expression ')' THEN statement
      {SS_GR_VAL(SS_make_form(SI, _SS_f_if, $3, $5, LAST));}

  | IF '(' expression ')' THEN statement ELSE statement
      {SS_GR_VAL(SS_make_form(SI, _SS_f_if, $3, $5, $7, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPRESSION_STATEMENT - from ANSI F90 Appendix D */

expression_statement :
    expression
      {if (SS_consp($1) && (SS_consp(SS_CAR_MACRO($1))))
	  {SS_GR_VAL(SS_mk_cons(SI, _SS_f_block,
				SS_mk_cons(SI, SS_null,
					   SS_reverse(SI, $1))));}
       else
	  {SS_GR_VAL(CAPTURE($1));};
       DIAGNOSTIC($$, "expression");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPOUND_STATEMENT - from ANSI F90 Appendix D */

compound_statement :
    declaration_list statement_list
      {SS_GR_VAL(SS_mk_cons(SI, _SS_f_block,
			    SS_mk_cons(SI, SS_reverse(SI, $1),
				       SS_reverse(SI, $2))));
       DIAGNOSTIC($$, "{ declaration-list statement-list }");}

  | statement_list
      {SS_GR_VAL(SS_mk_cons(SI, _SS_f_block,
			    SS_mk_cons(SI, SS_null,
				       SS_reverse(SI, $1))));
       DIAGNOSTIC($$, "{ statement-list }");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STATEMENT_LIST - from ANSI F90 Appendix D */

statement_list :
    statement EOS
      {if (SS_consp(SS_CAR_MACRO($1)))
	  {SS_GR_VAL(CAPTURE($1));}
       else
	  {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));};
       DIAGNOSTIC($$, "statement");}

  | statement_list EOS statement
      {if (SS_consp(SS_CAR_MACRO($2)))
	  {SS_GR_VAL(SS_append(SI, $2, $1));}
       else
	  {SS_GR_VAL(SS_mk_cons(SI, $2, $1));};
       DIAGNOSTIC($$, "statement-list statement");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPRESSION - expression of ANSI F90 Appendix D */

expression :
    assignment_expression
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "assignment-expression");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASSIGNMENT-EXPRESSION - from ANSI F90 Appendix D */

assignment_expression :
    logical_or_expression

  | unary_expression '=' assignment_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_set, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOGICAL_OR_EXPRESSION - from ANSI F90 Appendix D */

logical_or_expression :
    logical_and_expression

  | logical_or_expression OR logical_and_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_or, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOGICAL_AND_EXPRESSION - from ANSI F90 Appendix D */

logical_and_expression :
    equality_expression

  | logical_and_expression AND equality_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_and, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EQUALITY_EXPRESSION - from ANSI F90 Appendix D */

equality_expression :
    relational_expression

  | equality_expression EQ relational_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_equal, $1, $3, LAST));}

  | equality_expression NEQ relational_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_not, SS_make_form(SI, _SS_f_equal, $1, $3, LAST), LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RELATIONAL_EXPRESSION - from ANSI F90 Appendix D */

relational_expression :
    shift_expression

  | relational_expression LT shift_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_lt, $1, $3, LAST));}

  | relational_expression GT shift_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_gt, $1, $3, LAST));}

  | relational_expression LE shift_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_le, $1, $3, LAST));}

  | relational_expression GE shift_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_ge, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHIFT_EXPRESSION - from ANSI F90 Appendix D */

shift_expression :
    additive_expression
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADDITIVE_EXPRESSION - from ANSI F90 Appendix D */

additive_expression :
    multiplicative_expression

  | additive_expression EXPT multiplicative_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_expt, $1, $3, LAST));}

  | additive_expression '+' multiplicative_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_plus, $1, $3, LAST));}

  | additive_expression '-' multiplicative_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_minus, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MULTIPLICATIVE_EXPRESSION - from ANSI F90 Appendix D */

multiplicative_expression :
    unary_expression

  | multiplicative_expression '*' unary_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_times, $1, $3, LAST));}

  | multiplicative_expression '/' unary_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_divide, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNARY_EXPRESSION - from ANSI F90 Appendix D */

unary_expression :
    postfix_expression

  | '+' unary_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_plus, $2, LAST));}

  | '-' unary_expression
      {SS_GR_VAL(SS_make_form(SI, _SS_f_minus, $2, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POSTFIX_EXPRESSION - from ANSI F90 Appendix D */

postfix_expression :
    primary_expression

  | postfix_expression '(' argument_expression_list ')'
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_reverse(SI, $3)));}

  | postfix_expression '(' ')'
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRIMARY_EXPRESSION - from ANSI F90 Appendix D */

primary_expression :
    VARNAME
      {DIAGNOSTIC($1, "VARNAME");}

  | constant
      {DIAGNOSTIC($1, "constant");}

  | STRING                
      {DIAGNOSTIC($1, "STRING");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ARGUMENT_EXPRESSION_LIST - from ANSI F90 Appendix D */

argument_expression_list :
    assignment_expression
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));}

  | argument_expression_list ',' assignment_expression
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONSTANT - from ANSI F90 Appendix D */

constant :
    INTEGER
      {DIAGNOSTIC($1, "INTEGER");}

  | FLOAT
      {DIAGNOSTIC($1, "FLOAT");}
  ;

/*--------------------------------------------------------------------------*/

%%

/*--------------------------------------------------------------------------*/

/*                             EXTRAS SECTION                               */

/*--------------------------------------------------------------------------*/

#undef YYACCEPT

int
 SS_f_tokens[] = { TYPE_SPEC, VARNAME };

#include "shlrf.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_TOKEN_F - return YYVAL portably */

object *SS_parse_token_f()
   {

    return(_SS_f_val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_TOKEN_VAL - return YYLVAL portably */

object *SS_parse_token_val_f()
   {

    return(yylval);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_DEBUG_F - return a pointer to YYDEBUG portably */

int *SS_parse_debug_f()
   {

    return(&yydebug);}

/*--------------------------------------------------------------------------*/

/*                         END OF EXTRAS SECTION                            */

/*--------------------------------------------------------------------------*/

