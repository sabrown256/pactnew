%{

/*
 * SHGRC.Y - grammar for C syntax parsing in SCHEME
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/* Here is what's NOT currently supported (wrt parsing):
 *
 *    CHAR's although char *'s are supported (primary_expressions)
 *    multi-dimensional arrays semantics (syntax is there)
 *    a.b syntax although a->b is supported (postfix_expression)
 *    &, * i.e. address of and dereference (unary_expression)
 */

/* Here is what's NOT currently supported from the
 * CPP syntax:
 *
 *    #elif
 *    #line   seems a shame to make line a reserved word
 *            just have to have the lexer switch modes when DER
 *            is found then line, error, and pragma could all
 *            be treated
 *    #error  seems a shame to make error a reserved word
 *    #pragma is limited only because I ran out of energy to
 *            implement a lexer mode in which tokens are returned
 *            so that the parser can ignore them
 *    #<var> has only limited support due to the extreme complexity
 *           of trying to do it interpretively at runtime
 */

/* Note on conflicts in this grammar
 * properly C has one shift/reduce conflict which is the usual if/else
 * there are 4 shift/reduce conflicts in this grammar
 *   1)   the canonical if/else
 *   2)   an additional if/declaration conflict to allow
 *        interleaved if expressions and variable declarations
 *        in interpreted C code
 *   3&4) folding in the CPP '#x', i.e. stringification and concatenation
 *
 * there are 2 reduce/reduce conflicts in this grammar
 *   1&2) adding procedural #define's introduces these 2 in connection
 *        with recognizing the end of the body with NL instead of ';'
 */

/*--------------------------------------------------------------------------*/

/*                           DEFINITIONS SECTION                            */

/*--------------------------------------------------------------------------*/

#include "shc.h"

#undef CONST
#undef SIGNED

#ifdef ANSI
#define YY_USE_PROTOS
#endif

#define YYSTYPE     objp
#define YYDEBUG     1

#define shgrc_lex   shlrc_lex
#define shgrc_lval  shlrc_lval

#define yyerror(x)  SS_parse_error_synt(SI, x, SS_parse_token_c);

/* assign X to yyval and _SS_c_val because in bison yyval is
 * a local variable in yyparse whereas in yacc it is file static
 */

#define SS_GR_VAL(_x)  {yyval = _x; _SS_c_val = yyval;}

extern int
 yydebug;

static object
 *_SS_c_val = NULL;

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
%token AUTO
%token REGISTER
%token STATIC
%token EXTERN

%token TYPE_QUALIFIER
%token CONST
%token VOLATILE

%token TYPE_SPEC
%token SIGNED
%token UNSIGNED

%token TYPEDEF
%token STRUCT
%token UNION
%token ENUM
%token DOTDOT

%token DER
%token CAT

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
%token LE
%token GE
%token AND
%token OR

%token RS
%token LS

%token DOT
%token ACC
%token INC
%token DEC

%token PLUSEQ
%token MINUSEQ
%token TIMESEQ
%token DIVEQ
%token MODEQ

%token RSEQ
%token LSEQ

%token ANDEQ
%token OREQ
%token XOREQ

%token XOR
%token EXPT

%token INCLUDE
%token DEFINE
%token UNDEF
%token IFDEF
%token IFNDEF
%token ENDIF
%token PRAGMA

%token NL

%left '+' '-'
%left '*' '/' '%'
%right UMINUS

%%

/*--------------------------------------------------------------------------*/

/* SEQUENCE - map a C-style block into a SCHEME let*
 *          - i.e. roughly speaking
 *          -    {<decls>; statement; ... } -> (let* (<locals>) expr ... )
 *          - single statements go over without the block
 *          -    statement;  ->  expr
 */

sequence :

    statement
      {yylval = $1;
       DIAGNOSTIC(yylval, "statement");
/*
       if (_SS_cps.cpp_directive == FALSE)
	  printf("\n");
*/
       YYACCEPT;}

  | external_declaration
      {yylval = $1;
       DIAGNOSTIC(yylval, "external-declaration");
/*       printf("\n"); */
       YYACCEPT;}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXTERNAL_DECLARATION - from ANSI C grammar in K&R 2nd */

external_declaration :
    function_definition
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "function-definition");}

  | declaration
      {SS_GR_VAL(SS_mk_cons(SI, _SS_c_defvar, $1));
       DIAGNOSTIC($$, "declaration");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FUNCTION_DEFINITION - from ANSI C grammar in K&R 2nd */

function_definition :
    declaration_specifiers declarator declaration_list compound_statement
      {SS_GR_VAL(_SS_make_func(SI, $2, $4));
       DIAGNOSTIC($$, "DDDC");}

  | declaration_specifiers declarator compound_statement
      {SS_GR_VAL(_SS_make_func(SI, $2, $3));
       DIAGNOSTIC($$, "DDC(2)");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATION - from ANSI C grammar in K&R 2nd */

declaration :
    declaration_specifiers init_declarator_list ';'
      {SS_GR_VAL(_SS_make_decl(SI, $1, $2));
       DIAGNOSTIC($$, "declaration-specifiers init-declarator-list");}

  | declaration_specifiers ';'
      {SS_GR_VAL(_SS_make_decl(SI, $1, SS_null));
       DIAGNOSTIC($$, "declaration-specifiers");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATION_LIST - from ANSI C grammar in K&R 2nd */

declaration_list :
    declaration
      {SS_GR_VAL(SS_make_form($1, LAST));
       DIAGNOSTIC($$, "declaration");}

  | declaration_list declaration
      {SS_GR_VAL(SS_mk_cons(SI, $2, $1));
       DIAGNOSTIC($$, "declaration-list declaration");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATION_SPECIFIERS - from ANSI C grammar in K&R 2nd */

declaration_specifiers :
    storage_class_specifier
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "storage-class-specifier");}

  | storage_class_specifier declaration_specifiers
      {SS_GR_VAL(SS_make_form($1, $2, LAST));
       DIAGNOSTIC($$, "storage-class-specifier declaration-specifiers");}

  | type_specifier
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "type-specifier");}

  | type_specifier declaration_specifiers
      {SS_GR_VAL(SS_make_form($1, $2, LAST));
       DIAGNOSTIC($$, "type-specifier declaration-specifiers");}

  | type_qualifier
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "type-qualifier");}

  | type_qualifier declaration_specifiers
      {SS_GR_VAL(SS_make_form($1, $2, LAST));
       DIAGNOSTIC($$, "type-qualifier declaration-specifiers");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STORAGE_CLASS_SPECIFIER - from ANSI C grammar in K&R 2nd */

storage_class_specifier :
    STORAGE_CLASS
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "STORAGE CLASS");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TYPE_SPECIFIER - from ANSI C grammar in K&R 2nd */

type_specifier :
    TYPE_SPEC
      {SS_GR_VAL(SS_eval_form(SI, _SS_c_decl, SS_null, $1, SS_null, LAST));
       DIAGNOSTIC($$, "TYPE-SPEC");}

  | struct_or_union_specifier
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "struct-union-specifier");}

  | enum_specifier
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "enum-specifier");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TYPE_QUALIFIER - from ANSI C grammar in K&R 2nd */

type_qualifier :
    TYPE_QUALIFIER
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "TYPE-QUALIFIER");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_OR_UNION_SPECIFIER - from ANSI C grammar in K&R 2nd */

struct_or_union_specifier :
    struct_or_union VARNAME '{' struct_declaration_list '}'
      {SS_GR_VAL(SS_eval_form(SI, _SS_c_decl, $1, $2, SS_reverse($4), LAST));
       DIAGNOSTIC($$, "struct-or-union VARNAME { struct-declaration-list }");}

  | struct_or_union '{' struct_declaration_list '}'
      {SS_GR_VAL(SS_eval_form(SI, _SS_c_decl, $1, SS_null, SS_reverse($3), LAST));
       DIAGNOSTIC($$, "struct-or-union { struct-declaration-list }");}

  | struct_or_union VARNAME
      {SS_GR_VAL(SS_eval_form(SI, _SS_c_decl, $1, $2, SS_null, LAST));
       DIAGNOSTIC($$, "struct-or-union VARNAME");}

  | struct_or_union TYPE_SPEC
      {SS_GR_VAL(SS_eval_form(SI, _SS_c_decl, $1, $2, SS_null, LAST));
       DIAGNOSTIC($$, "struct-or-union TYPE-SPEC");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_OR_UNION - from ANSI C grammar in K&R 2nd */

struct_or_union :
    STRUCT
      {SS_GR_VAL(_SS_c_struct);
       DIAGNOSTIC($$, "STRUCT");}

  | UNION
      {SS_GR_VAL(_SS_c_union);
       DIAGNOSTIC($$, "UNION");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_DECLARATION_LIST - from ANSI C grammar in K&R 2nd */

struct_declaration_list :
    struct_declaration
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($$, "struct-declaration");}

  | struct_declaration_list struct_declaration
      {SS_GR_VAL(SS_mk_cons(SI, $2, $1));
       DIAGNOSTIC($$, "struct-declaration-list struct-declaration");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_DECLARATOR_LIST - from ANSI C grammar in K&R 2nd */

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

/* INIT_DECLARATOR - from ANSI C grammar in K&R 2nd */

init_declarator :
    declarator
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "declarator");}

  | declarator '=' initializer
      {SS_GR_VAL(SS_make_form($1, $3, LAST));
       DIAGNOSTIC($$, "declarator = initializer");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_DECLARATION - from ANSI C grammar in K&R 2nd */

struct_declaration :
    specifier_qualifier_list struct_declarator_list ';'
      {SS_GR_VAL(SS_mk_cons(SI, $1, $2));
       DIAGNOSTIC($$, "specifier-qualifier-list");}

/* NOTE: GNU extension */
  | struct_or_union_specifier ';'
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "struct-or-union-specifier");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPECIFIER_QUALIFIER_LIST - from ANSI C grammar in K&R 2nd */

specifier_qualifier_list :
    type_specifier specifier_qualifier_list
      {SS_GR_VAL(SS_mk_cons(SI, $1, $2));
       DIAGNOSTIC($2, "type-specifier specifier-qualifier-list");}

  | type_specifier
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "type-specifier");}

  | type_qualifier specifier_qualifier_list
      {SS_GR_VAL(SS_mk_cons(SI, $1, $2));
       DIAGNOSTIC($$, "type-qualifier specifier-qualifier-list");}

  | type_qualifier
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "type-qualifier");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_DECLARATOR_LIST - from ANSI C grammar in K&R 2nd */

struct_declarator_list :
    struct_declarator
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($$, "struct-declarator");}

  | struct_declarator_list ',' struct_declarator
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));
       DIAGNOSTIC($$, "struct-declarator-list , struct-declarator");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_DECLARATOR - from ANSI C grammar in K&R 2nd */

struct_declarator :
    declarator
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "declarator");}

  | declarator ':' constant_expression
      {SYNTAX_ERR("declarator : const");}

  | ':' constant_expression
      {SYNTAX_ERR(": const");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENUM_SPECIFIER - from ANSI C grammar in K&R 2nd */

enum_specifier :
    ENUM VARNAME '{' enumerator_list '}'
      {SS_GR_VAL(SS_eval_form(SI, _SS_c_decl, _SS_c_enum, $2, SS_reverse($4), LAST));
       DIAGNOSTIC($$, "enum_specifier : VARNAME { enumerator_list }");}

  | ENUM '{' enumerator_list '}'
      {SS_GR_VAL(SS_eval_form(SI, _SS_c_decl, _SS_c_enum, SS_null, SS_reverse($3), LAST));
       DIAGNOSTIC($$, "enum_specifier : { enumerator_list }");}

  | ENUM VARNAME
      {SS_GR_VAL(SS_eval_form(SI, _SS_c_decl, _SS_c_enum, $2, SS_null, LAST));
       DIAGNOSTIC($$, "enum_specifier : VARNAME");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENUMERATOR_LIST - from ANSI C grammar in K&R 2nd */

enumerator_list :
    enumerator
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($$, "enumerator_list : enumerator");}

  | enumerator_list ',' enumerator
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));
       DIAGNOSTIC($$, "enumerator_list : enumerator_list , enumerator");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENUMERATOR - from ANSI C grammar in K&R 2nd */

enumerator :
    VARNAME
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($$, "enumerator : VARNAME");}

  | VARNAME '=' constant_expression
      {SS_GR_VAL(SS_mk_cons(SI, $1, $3));
       DIAGNOSTIC($$, "enumerator : VARNAME = constant_expression");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATOR - from ANSI C grammar in K&R 2nd */

declarator :
    pointer direct_declarator
      {if (SS_consp($1))
	  {if (SS_consp($2))
	      {SS_GR_VAL(SS_append(SI, $2, $1));}
           else
	      {SS_GR_VAL(SS_mk_cons(SI, $2, $1));};}
       else
	  {if (SS_consp($2))
	      {SS_GR_VAL(SS_append(SI, $2, SS_mk_cons(SI, $1, SS_null)));}
	   else
	      {SS_GR_VAL(CAPTURE($2));};};

       DIAGNOSTIC($$, "pointer direct-declarator");}

  | direct_declarator
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "direct-declarator");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIRECT_DECLARATOR - from ANSI C grammar in K&R 2nd */

direct_declarator :
    variable
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "variable");}

  | '(' declarator ')'
      {SS_GR_VAL(SS_mk_cons(SI, $2, SS_null));
       DIAGNOSTIC($$, "( declarator )");}

  | direct_declarator '[' constant_expression ']'
      {if (SS_consp($1))
	  {SS_GR_VAL(SS_append(SI, $1, SS_mk_cons(SI, $3, SS_null)));}
       else
	  {SS_GR_VAL(SS_make_form($1,
				  SS_make_form(_SS_c_defarr, $3, LAST),
				  LAST));};
       DIAGNOSTIC($$, "direct-declarator [ constant-expression ]");}

  | direct_declarator '[' ']'
      {SS_GR_VAL(SS_make_form($1, _SS_c_times, LAST));
       DIAGNOSTIC($$, "direct-declarator []");}

  | direct_declarator '(' parameter_type_list ')'
      {object *args;

       args = SS_reverse($3);
       if (SS_consp($1))
	  {object *f;

           f = SS_CAR_MACRO($1);
	   if (SS_consp(f))
	      {SS_GR_VAL(SS_mk_cons(SI, SS_append(SI, f, args), SS_null));}
	   else
	      {SS_GR_VAL(SS_append(SI, $1, args));};}
       else
	  {SS_GR_VAL(SS_mk_cons(SI, $1, args));};

       DIAGNOSTIC($$, "direct-declarator ( parameter-type-list )");}

  | direct_declarator '(' identifier_list ')'
      {object *args;

       args = SS_reverse($3);
       if (SS_consp($1))
	  {object *f;

           f = SS_CAR_MACRO($1);
	   if (SS_consp(f))
	      {SS_GR_VAL(SS_mk_cons(SI, SS_append(SI, f, args), SS_null));}
	   else
	      {SS_GR_VAL(SS_append(SI, $1, args));};}
       else
	  {SS_GR_VAL(SS_mk_cons(SI, $1, args));};

       DIAGNOSTIC($$, "direct-declarator ( identifier-list )");}

  | direct_declarator '(' ')'
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($$, "direct-declarator ()");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POINTER - from ANSI C grammar in K&R 2nd */

pointer :
    '*' type_qualifier_list
      {SS_GR_VAL(SS_mk_cons(SI, _SS_c_times, $2));
       DIAGNOSTIC($$, "* type-qualifier-list");}

  | '*'
      {SS_GR_VAL(_SS_c_times);
       DIAGNOSTIC($$, "*");}

  | '*' type_qualifier_list pointer
      {SS_GR_VAL(SS_make_form(_SS_c_times, $2, $3, LAST));
       DIAGNOSTIC($$, "* type-qualifier-list pointer");}

  | '*' pointer
      {SS_GR_VAL(SS_make_form(_SS_c_times, $2, LAST));
       DIAGNOSTIC($$, "* pointer");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TYPE_QUALIFIER_LIST - from ANSI C grammar in K&R 2nd */

type_qualifier_list :
    TYPE_QUALIFIER
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "TYPE-QUALIFIER");}

  | type_qualifier_list TYPE_QUALIFIER
      {SS_GR_VAL(SS_mk_cons(SI, $2, $1));
       DIAGNOSTIC($$, "type-qualifier-list TYPE-QUALIFIER");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARAMETER_TYPE_LIST - from ANSI C grammar in K&R 2nd */

parameter_type_list :
    parameter_list
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "parameter-list");}

  | parameter_list ',' DOTDOT
      {SYNTAX_ERR("parameter-list ...");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARAMETER_LIST - from ANSI C grammar in K&R 2nd */

parameter_list :
    parameter_declaration
      {if ($1 == SS_f)
	  {SS_GR_VAL(SS_null);}
       else
	  {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));};
       DIAGNOSTIC($$, "parameter-declaration");}

  | parameter_list ',' parameter_declaration
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));
       DIAGNOSTIC($$, "parameter-list , parameter-declaration");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARAMETER_DECLARATION - from ANSI C grammar in K&R 2nd */

parameter_declaration :
    declaration_specifiers declarator
      {SS_GR_VAL(CAPTURE($2));
       DIAGNOSTIC($$, "declaration-specifiers declator");}

  | declaration_specifiers abstract_declarator
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "declaratin-specifiers abstract-declarator");
       SYNTAX_ERR("declaration-specifiers abstract-declarator");}

  | declaration_specifiers
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "declaration-specifiers");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IDENTIFIER_LIST - from ANSI C grammar in K&R 2nd */

identifier_list :
    variable
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($$, "variable");}

  | identifier_list ',' variable
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));
       DIAGNOSTIC($$, "identifier-list , variable");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INITIALIZER - from ANSI C grammar in K&R 2nd */

initializer :
    assignment_expression
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "assignment-expression");}

  | '{' initializer_list '}'
      {SS_GR_VAL(SS_reverse($2));
       DIAGNOSTIC($$, "{initializer-list}");}

  | '{' initializer_list ',' '}'
      {SS_GR_VAL(SS_reverse($2));
       DIAGNOSTIC($$, "{initializer-list ,}");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INITIALIZER_LIST - from ANSI C grammar in K&R 2nd */

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

/* TYPE_NAME - from ANSI C grammar in K&R 2nd */

type_name :
    specifier_qualifier_list
      {DIAGNOSTIC($$, "specifier_qualifier_list");}

  | specifier_qualifier_list abstract_declarator
      {DIAGNOSTIC($$, "specifier-qualifier-list abstract-declarator");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ABSTRACT_DECLARATOR - from ANSI C grammar in K&R 2nd */

abstract_declarator :
    pointer
      {SYNTAX_ERR("pointer");}

  | pointer direct_abstract_declarator
      {SYNTAX_ERR("pointer direct-abst-decl");}

  | direct_abstract_declarator
      {SYNTAX_ERR("direct-abst-decl");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIRECT_ABSTRACT_DECLARATOR - from ANSI C grammar in K&R 2nd */

direct_abstract_declarator :
    '(' abstract_declarator ')'
      {SYNTAX_ERR("( abst-decl )");}

  | direct_abstract_declarator '[' constant_expression ']'
      {SYNTAX_ERR("direct-abst-decl [ ... ]");}

  | direct_abstract_declarator '[' ']'
      {SYNTAX_ERR("direct-abst-decl []");}

  | '[' constant_expression ']'
      {SYNTAX_ERR("[ const-expr ]");}

  | '[' ']'
      {SYNTAX_ERR("[]");}

  | direct_abstract_declarator '(' parameter_type_list ')'
      {SYNTAX_ERR("direct-abst-decl ( ... )");}

  | direct_abstract_declarator '(' ')'
      {SYNTAX_ERR("direct-abst-decl ()");}

  | '(' parameter_type_list ')'
      {SYNTAX_ERR("( ... )");}

  | '(' ')'
      {SYNTAX_ERR("()");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STATEMENT - from ANSI C grammar in K&R 2nd */

statement :
    labeled_statement
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "labeled-statement");}

  | expression_statement
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "expression-statement");}

  | compound_statement
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "compound-statement");}

  | selection_statement
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "selection-statement");}

  | iteration_statement
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "iteration-statement");}

  | jump_statement
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "jump-statement");}

/* NOTE: CPP extension */
  | preprocessor_directive
      {_SS_cps.cpp_directive = TRUE;
       SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "preprocessor-directive");}

  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LABELED_STATEMENT - from ANSI C grammar in K&R 2nd */

labeled_statement :
    variable ':' statement
      {SYNTAX_ERR("variable : statement");}

  | CASE constant_expression ':' statement
      {SS_GR_VAL(SS_mk_cons(SI, $4,
			    SS_mk_cons(SI, SS_make_form(_SS_c_label, $2, LAST),
				       SS_null)));
       DIAGNOSTIC($$, "CASE constant_expression : statement");}

  | DEFAULT ':' statement
      {SS_GR_VAL(SS_mk_cons(SI, $3,
			    SS_mk_cons(SI, SS_make_form(_SS_c_label, SS_t, LAST),
				       SS_null)));
       DIAGNOSTIC($$, "DEFAULT : statement");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPRESSION_STATEMENT - from ANSI C grammar in K&R 2nd */

expression_statement :
    expression ';'
      {if (SS_consp($1) && (SS_consp(SS_CAR_MACRO($1))))
	  {SS_GR_VAL(SS_mk_cons(SI, _SS_c_block,
				SS_mk_cons(SI, SS_null,
					   SS_reverse($1))));}
       else
	  {SS_GR_VAL(CAPTURE($1));};
       DIAGNOSTIC($$, "expression");}

  | ';'
      {SS_GR_VAL(SS_null);
       DIAGNOSTIC($$, ";");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPOUND_STATEMENT - from ANSI C grammar in K&R 2nd */

compound_statement :
    '{' declaration_list statement_list '}'
      {SS_GR_VAL(_SS_make_cmpnd_stmnt(SI, $2, $3));
       DIAGNOSTIC($$, "{ declaration-list statement-list }");}

  | '{' declaration_list '}'
      {SS_GR_VAL(_SS_make_cmpnd_stmnt(SI, $2, SS_null));
       DIAGNOSTIC($$, "{ declaration-list }");}

  | '{' statement_list '}'
      {SS_GR_VAL(_SS_make_cmpnd_stmnt(SI, SS_null, $2));
       DIAGNOSTIC($$, "{ statement-list }");}

  | '{' '}'
      {SS_GR_VAL(SS_f);
       DIAGNOSTIC($$, "{ }");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STATEMENT_LIST - from ANSI C grammar in K&R 2nd */

statement_list :
    statement
      {SS_GR_VAL(_SS_make_stmnt_lst(SI, $1, SS_null));
       DIAGNOSTIC($$, "statement");}

  | statement_list statement
      {SS_GR_VAL(_SS_make_stmnt_lst(SI, $2, $1));
       DIAGNOSTIC($$, "statement-list statement");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SELECTION_STATEMENT - from ANSI C grammar in K&R 2nd */

selection_statement :
    IF '(' expression ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_if, $3, $5, LAST));}

/* GOTCHA: this adds one shift/reduce conflict to the grammar
 * it is included to allow interpreted C to mix 'if (<e>) <s>'
 * forms with declarations
 */
  | IF '(' expression ')' statement TYPE_SPEC
      {SS_GR_VAL(SS_make_form(_SS_c_if, $3, $5, LAST));}

/* GOTCHA: this adds one shift/reduce conflict to the grammar
 * this is the canonical if/else conflict
 */
  | IF '(' expression ')' statement ELSE statement
      {SS_GR_VAL(SS_make_form(_SS_c_if, $3, $5, $7, LAST));}

  | SWITCH '(' expression ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_cond, $3, $5, LAST));
       DIAGNOSTIC($$, "SWITCH (expression) statement");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ITERATION_STATEMENT - from ANSI C grammar in K&R 2nd */

iteration_statement :
    WHILE '(' expression ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_while, SS_f, $3, $5, LAST));}

  | DO statement WHILE '(' expression ')' ';'
      {SS_GR_VAL(SS_make_form(_SS_c_while, SS_t, $5, $2, LAST));}

  | FOR '(' expression ';' expression ';' expression ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_for, $3, $5, $7, $9, LAST));}

  | FOR '(' ';' expression ';' expression ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_for, SS_null, $4, $6, $8, LAST));}

  | FOR '(' expression ';' ';' expression ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_for, $3, SS_null, $6, $8, LAST));}

  | FOR '(' expression ';' expression ';' ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_for, $3, $5, SS_null, $8, LAST));}

  | FOR '(' expression ';' ';' ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_for, $3, SS_null, SS_null, $7, LAST));}

  | FOR '(' ';' expression ';' ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_for, SS_null, $4, SS_null, $7, LAST));}

  | FOR '(' ';' ';' expression ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_for, SS_null, SS_null, $5, $7, LAST));}
  | FOR '(' ';' ';' ')' statement
      {SS_GR_VAL(SS_make_form(_SS_c_for, SS_null, SS_null, SS_null, $6, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JUMP_STATEMENT - from ANSI C grammar in K&R 2nd */

jump_statement :
    GOTO variable ';'
      {SS_GR_VAL(SS_make_form(_SS_c_goto, $2));
       DIAGNOSTIC($$, "GOTO variable");}

  | CONTINUE ';'
      {SS_GR_VAL(SS_make_form(_SS_c_continue, SS_t, LAST));
       DIAGNOSTIC($$, "CONTINUE");}

  | BREAK ';'
      {SS_GR_VAL(SS_make_form(_SS_c_continue, SS_f, LAST));
       DIAGNOSTIC($$, "BREAK");}

  | RETURN expression ';'
      {SS_GR_VAL(SS_make_form(_SS_c_return, $2, LAST));
       DIAGNOSTIC($$, "RETURN expression");}

  | RETURN ';'
      {SS_GR_VAL(SS_make_form(_SS_c_return, SS_null, LAST));
       DIAGNOSTIC($$, "RETURN");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPRESSION - expression of ANSI C grammar in K&R 2nd */

expression :
    assignment_expression
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "assignment-expression");}

  | expression ',' assignment_expression
      {if (SS_consp(SS_CAR_MACRO($1)))
	  {SS_GR_VAL(SS_mk_cons(SI, $3, $1));}
       else
	  {SS_GR_VAL(SS_make_form($3, $1, LAST));};
       DIAGNOSTIC($$, "expression , assignment-expression");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASSIGNMENT-EXPRESSION - from ANSI C grammar in K&R 2nd */

assignment_expression :
    conditional_expression

  | unary_expression '=' assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1, $3, LAST));}

  | unary_expression TIMESEQ assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1,
			      SS_make_form(_SS_c_times, $1, $3, LAST), LAST));}

  | unary_expression PLUSEQ assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1,
			      SS_make_form(_SS_c_plus, $1, $3, LAST), LAST));}

  | unary_expression MINUSEQ assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1,
			      SS_make_form(_SS_c_minus, $1, $3, LAST), LAST));}

  | unary_expression DIVEQ assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1,
			      SS_make_form(_SS_c_divide, $1, $3, LAST), LAST));}

  | unary_expression MODEQ assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1,
			      SS_make_form(_SS_c_modulo, $1, $3, LAST), LAST));}

  | unary_expression LSEQ assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1,
			      SS_make_form(_SS_c_lshft, $1, $3, LAST), LAST));}

  | unary_expression RSEQ assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1,
			      SS_make_form(_SS_c_rshft, $1, $3, LAST), LAST));}

  | unary_expression ANDEQ assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1,
			      SS_make_form(_SS_c_bitand, $1, $3, LAST), LAST));}

  | unary_expression XOREQ assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1,
			      SS_make_form(_SS_c_bitcmp, $1, $3, LAST), LAST));}

  | unary_expression OREQ assignment_expression
      {SS_GR_VAL(SS_make_form(_SS_c_set, $1,
			      SS_make_form(_SS_c_bitor, $1, $3, LAST), LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONDITIONAL_EXPRESSION - from ANSI C grammar in K&R 2nd */

conditional_expression :
    logical_or_expression

  | logical_or_expression '?' expression ':' conditional_expression
      {SS_GR_VAL(SS_make_form(_SS_c_if, $1, $3, $5, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONSTANT_EXPRESSION - from ANSI C grammar in K&R 2nd */

constant_expression :
    conditional_expression
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOGICAL_OR_EXPRESSION - from ANSI C grammar in K&R 2nd */

logical_or_expression :
    logical_and_expression

  | logical_or_expression OR logical_and_expression
      {SS_GR_VAL(SS_make_form(_SS_c_or, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOGICAL_AND_EXPRESSION - from ANSI C grammar in K&R 2nd */

logical_and_expression :
    inclusive_or_expression

  | logical_and_expression AND inclusive_or_expression
      {SS_GR_VAL(SS_make_form(_SS_c_and, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INCLUSIVE_OR_EXPRESSION - from ANSI C grammar in K&R 2nd */

inclusive_or_expression :
    exclusive_or_expression

  | inclusive_or_expression '|' exclusive_or_expression
      {SS_GR_VAL(SS_make_form(_SS_c_bitor, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXCLUSIVE_OR_EXPRESSION - modification of ANSI C grammar in K&R 2nd
 *                         - use ^ as exponentiation (e.g. pow) operator
 *                         - when arguments are not both integers
 *                         - remove from  here for proper precedence wrt
 *                         - arithmetic operations
 */

exclusive_or_expression :
    and_expression

  | exclusive_or_expression XOR and_expression
      {SS_GR_VAL(SS_make_form(_SS_c_xor, $1, $3, LAST));}

  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* AND_EXPRESSION - from ANSI C grammar in K&R 2nd */

and_expression :
    equality_expression

  | and_expression '&' equality_expression
      {SS_GR_VAL(SS_make_form(_SS_c_bitand, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EQUALITY_EXPRESSION - from ANSI C grammar in K&R 2nd */

equality_expression :
    relational_expression

  | equality_expression EQ relational_expression
      {SS_GR_VAL(SS_make_form(_SS_c_equal, $1, $3, LAST));}

  | equality_expression NEQ relational_expression
      {SS_GR_VAL(SS_make_form(_SS_c_not,
			      SS_make_form(_SS_c_equal, $1, $3, LAST), LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RELATIONAL_EXPRESSION - from ANSI C grammar in K&R 2nd */

relational_expression :
    shift_expression

  | relational_expression '<' shift_expression
      {SS_GR_VAL(SS_make_form(_SS_c_lt, $1, $3, LAST));}

  | relational_expression '>' shift_expression
      {SS_GR_VAL(SS_make_form(_SS_c_gt, $1, $3, LAST));}

  | relational_expression LE shift_expression
      {SS_GR_VAL(SS_make_form(_SS_c_le, $1, $3, LAST));}

  | relational_expression GE shift_expression
      {SS_GR_VAL(SS_make_form(_SS_c_ge, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHIFT_EXPRESSION - from ANSI C grammar in K&R 2nd */

shift_expression :
    additive_expression

  | shift_expression LS additive_expression
      {SS_GR_VAL(SS_make_form(_SS_c_lshft, $1, $3, LAST));}

  | shift_expression RS additive_expression
      {SS_GR_VAL(SS_make_form(_SS_c_rshft, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADDITIVE_EXPRESSION - from ANSI C grammar in K&R 2nd */

additive_expression :
    multiplicative_expression

  | additive_expression '+' multiplicative_expression
      {SS_GR_VAL(SS_make_form(_SS_c_plus, $1, $3, LAST));}

  | additive_expression '-' multiplicative_expression
      {SS_GR_VAL(SS_make_form(_SS_c_minus, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MULTIPLICATIVE_EXPRESSION - from ANSI C grammar in K&R 2nd */

multiplicative_expression :
    exponentiation_expression

  | multiplicative_expression '*' exponentiation_expression
      {SS_GR_VAL(SS_make_form(_SS_c_times, $1, $3, LAST));}

  | multiplicative_expression '/' exponentiation_expression
      {SS_GR_VAL(SS_make_form(_SS_c_divide, $1, $3, LAST));}

  | multiplicative_expression '%' exponentiation_expression
      {SS_GR_VAL(SS_make_form(_SS_c_modulo, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPONENTIATION_EXPRESSION - modification of ANSI C grammar in K&R 2nd
 *                           - use ^ as exponentiation (e.g. pow) operator
 *                           - when arguments are not both integers
 *                           - insert here for proper precedence wrt
 *                           - arithmetic operations
 */

exponentiation_expression :
    cast_expression

  | exponentiation_expression EXPT cast_expression
      {SS_GR_VAL(SS_make_form(_SS_c_expt, $1, $3, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CAST_EXPRESSION - from ANSI C grammar in K&R 2nd */

cast_expression :
    unary_expression

  | '(' type_name ')' cast_expression
      {SS_GR_VAL(_SS_make_cast(SI, $2, $4));
       DIAGNOSTIC($$, "cast");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNARY_EXPRESSION - from ANSI C grammar in K&R 2nd */

unary_expression :
    postfix_expression

  | INC unary_expression
      {SS_GR_VAL(SS_make_form(_SS_c_preinc, $2, LAST));}

  | DEC unary_expression
      {SS_GR_VAL(SS_make_form(_SS_c_predec, $2, LAST));}

  | SIZOF unary_expression
      {SS_GR_VAL(SS_make_form(_SS_c_sizeof, $2, SS_f, LAST));}

  | SIZOF '(' TYPE_SPEC ')'
      {SS_GR_VAL(SS_make_form(_SS_c_sizeof, $3, SS_t, LAST));}

  | '&' cast_expression
      {DIAGNOSTIC($$, "&cast-expr");}

  | '*' cast_expression
      {DIAGNOSTIC($$, "*cast-expr");}

  | '+' unary_expression
      {SS_GR_VAL(SS_make_form(_SS_c_plus, $2, LAST));}

  | '-' unary_expression
      {SS_GR_VAL(SS_make_form(_SS_c_minus, $2, LAST));}

  | '!' unary_expression
      {SS_GR_VAL(SS_make_form(_SS_c_not, $2, LAST));}

  | '~' unary_expression
      {SS_GR_VAL(SS_make_form(_SS_c_bitcmp, $2, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POSTFIX_EXPRESSION - from ANSI C grammar in K&R 2nd */

postfix_expression :
    primary_expression

  | postfix_expression '[' expression ']'
      {SS_GR_VAL(SS_make_form(_SS_c_aref, $1, $3, LAST));}

  | postfix_expression '(' argument_expression_list ')'
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_reverse($3)));}

  | postfix_expression '(' ')'
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));}

  | postfix_expression DOT variable
      {char t[MAXLINE];
       sprintf(t, "\"%s\"", SS_VARIABLE_NAME($3));
       SS_GR_VAL(SS_make_form(_SS_c_mref, $1, CSTRSAVE(t), LAST));}

  | postfix_expression ACC variable
      {char t[MAXLINE];
       sprintf(t, "\"%s\"", SS_VARIABLE_NAME($3));
       SS_GR_VAL(SS_make_form(_SS_c_mref, $1, CSTRSAVE(t), LAST));}

  | postfix_expression INC
      {SS_GR_VAL(SS_make_form(_SS_c_postinc, $1, LAST));}

  | postfix_expression DEC
      {SS_GR_VAL(SS_make_form(_SS_c_postdec, $1, LAST));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRIMARY_EXPRESSION - from ANSI C grammar in K&R 2nd */

primary_expression :
    variable
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "variable");}

  | constant
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "constant");}

  | string                
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "string");}

  | '(' expression ')'
      {SS_GR_VAL(CAPTURE($2));
       DIAGNOSTIC($$, "( expression )");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ARGUMENT_EXPRESSION_LIST - from ANSI C grammar in K&R 2nd */

argument_expression_list :
    assignment_expression
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));}

  | argument_expression_list ',' assignment_expression
      {SS_GR_VAL(SS_mk_cons(SI, $3, $1));}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VARIABLE - modify ANSI C grammar in K&R 2nd to include
 *          - CPP 'x ## y' syntax
 */

variable :
    VARNAME
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "VARNAME");}

/* NOTE: CPP extension */
  | VARNAME CAT VARNAME
      {if (_SS_cps.cpp_define == TRUE)
	  {SS_GR_VAL(SS_make_form(_SS_c_catvars, $1, $3, LAST));
	   DIAGNOSTIC($$, "VARNAME ## VARNAME");}
       else
          {SYNTAX_ERR("VARNAME ## VARNAME");};}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRING - modify ANSI C grammar in K&R 2nd to include
 *        - CPP '#x' syntax
 */

string :
    string_unit
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "string_unit");}

/* GOTCHA: this adds 2 shift/reduce conflicts
 *         the consequence are as yet unknown
 *         the desire here is to handle arbitrary sequences
 *         of literal strings and #x constructs
 *         the lexical analyzer concatenates sequences of
 *         space delimited literal strings
 */
  | string string_unit
      {if (_SS_cps.cpp_define == TRUE)
	  {SS_GR_VAL(SS_make_form(_SS_c_strapp, $1, $2, LAST));
	   DIAGNOSTIC($$, "string string_unit");}
       else
          {SYNTAX_ERR("string string_unit");};}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRING_UNIT - modify ANSI C grammar in K&R 2nd to handle
 *             - appending strings (either literal or #<var>)
 */

string_unit :
    STRING
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "STRING");}

/* NOTE: CPP extension */
  | pseudo_string
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "pseudo_string");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONSTANT - from ANSI C grammar in K&R 2nd */

constant :
    INTEGER
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "INTEGER");}

  | FLOAT
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "FLOAT");}
  ;

/*--------------------------------------------------------------------------*/

/*                           CPP EXTENSIONS                                 */

/*--------------------------------------------------------------------------*/

/* PSEUDO_STRING - CPP '#x' syntax
 */

pseudo_string :
    DER VARNAME
      {if (_SS_cps.cpp_define == TRUE)
	  {SS_GR_VAL(SS_make_form(_SS_c_strvar, $2, LAST));
	   DIAGNOSTIC($$, "# VARNAME");}
       else
          {SYNTAX_ERR("# VARNAME");};}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MACRO_DECLARATOR - jam CPP directives into the C grammar
 *                  - for the interpreter
 */

macro_declarator :
    VARNAME
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "VARNAME");}

  | macro_declarator '(' identifier_list ')'
      {SS_GR_VAL(_SS_make_mac_decl(SI, $1, $3));
       DIAGNOSTIC($$, "macro-declarator ( identifier-list )");}

  | macro_declarator '(' ')'
      {SS_GR_VAL(SS_mk_cons(SI, $1, SS_null));
       DIAGNOSTIC($$, "macro-declarator ()");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MACRO_BODY - jam CPP directives into the C grammar
 *            - for the interpreter
 */

macro_body :
    NL
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "macro-body NL");}

/* GOTCHA: the next two productions add 2 reduce/reduce conflicts
 *         to the grammar (each one adds 2, together they only add 2)
 *         but no adverse consequences have yet been exposed
 */

  |  expression NL
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "macro-body expression NL");}

  | expression_statement NL
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "macro-body expression-statement NL");}

  | compound_statement NL
      {SS_GR_VAL(CAPTURE($1));
       DIAGNOSTIC($$, "macro-body compound-statement NL");}

  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PREPROCESSOR_DIRECTIVE - extension to ANSI C grammar
 *                        - to interpret C preprocessor directives
 *                        - in a quasi-reasonable fashion
 */

preprocessor_directive :
    DER INCLUDE STRING
      {SS_GR_VAL(SS_make_form(_SS_c_load, $3, LAST));
       DIAGNOSTIC($$, "INCLUDE");}

  | DER DEFINE macro_declarator macro_body
      {SS_GR_VAL(_SS_make_macr($3, $4));
       _SS_cps.cpp_define = FALSE;
       DIAGNOSTIC($$, "DEFINE proc body");}

  | DER UNDEF VARNAME
      {SS_GR_VAL(_SS_del_var(SI, $3));
       DIAGNOSTIC($$, "UNDEF var");}

  | DER IF primary_expression statement DER ENDIF
      {SS_GR_VAL(SS_make_form(_SS_c_if, $3, $4, LAST));
       DIAGNOSTIC($$, "IF ENDIF");}

  | DER IF primary_expression statement DER ELSE statement DER ENDIF
      {SS_GR_VAL(SS_make_form(_SS_c_if, $3, $4, $7, LAST));
       DIAGNOSTIC($$, "IF ELSE ENDIF");}

  | DER IFDEF primary_expression statement DER ENDIF
      {SS_GR_VAL(SS_make_form(_SS_c_if,
			      SS_make_form(_SS_c_defined, $3, LAST), $4, LAST));
       DIAGNOSTIC($$, "IFDEF ENDIF");}

  | DER IFDEF primary_expression statement DER ELSE statement DER ENDIF
      {SS_GR_VAL(SS_make_form(_SS_c_if,
			      SS_make_form(_SS_c_defined, $3, LAST),
			      $4, $7,
			      LAST));
       DIAGNOSTIC($$, "IFDEF ELSE ENDIF");}

  | DER IFNDEF primary_expression statement DER ENDIF
      {SS_GR_VAL(SS_make_form(_SS_c_if,
			      SS_make_form(_SS_c_not,
					   SS_make_form(_SS_c_defined, $3, LAST),
					   LAST),
			      $4, LAST));
       DIAGNOSTIC($$, "IFNDEF ENDIF");}

  | DER IFNDEF primary_expression statement DER ELSE statement DER ENDIF
      {SS_GR_VAL(SS_make_form(_SS_c_if,
			      SS_make_form(_SS_c_not,
					   SS_make_form(_SS_c_defined, $3, LAST),
					   LAST),
			      $4, $7, LAST));
       DIAGNOSTIC($$, "IFNDEF ELSE ENDIF");}

  | DER PRAGMA macro_body
      {SS_GR_VAL(SS_null);
       _SS_cps.cpp_define = FALSE;
       DIAGNOSTIC($$, "PRAGMA");}
  ;

/*--------------------------------------------------------------------------*/

%%

/*--------------------------------------------------------------------------*/

/*                             EXTRAS SECTION                               */

/*--------------------------------------------------------------------------*/

#undef YYACCEPT

int
 SS_c_tokens[] = { TYPE_SPEC, VARNAME };

static object
 *SS_parse_string_c(SS_psides *si);

#include "shlrc.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_TOKEN_C - return YYVAL portably */

object *SS_parse_token_c(void)
   {

    return(_SS_c_val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_TOKEN_VAL_C - return YYLVAL portably */

object *SS_parse_token_val_c(void)
   {

    return(yylval);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_DEBUG_C - return a pointer to YYDEBUG portably */

int *SS_parse_debug_c(void)
   {

    return(&yydebug);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PARSE_STRING_C - return a string token portably
 *                   - it is perhaps impossible to get all of the
 *                   - vendor lex programs to do the right thing
 *                   - from a single set of rules for string parsing
 *                   - we have two constraints that together seem to
 *                   - shoot at least one lex scanner down:
 *                   -   1) we must handle imbedded literal quotes '"'
 *                   -      properly
 *                   -   2) we support string concatenation so the rules:
 *                   -      <string> := <seg>+
 *                   -      <seg>    := "<chars>*<frag>*"<blank>*
 *                   -      <chars>  := [^"]
 *                   -      <frag>   := \"<chars>*
 *                   -      <blank>  := [ \t\n\r\f]
 *                   -      must be followed
 */

object *SS_parse_string_c(SS_psides *si)
   {int i, in, done;
    char c;
    object *rv;
    static char *s = NULL;
    static int ns = 0L;

    if (s == NULL)
       {ns = MAX_BFSZ;
	s  = SC_permanent(CMAKE_N(char, ns));
	memset(s, 0, ns);};

    strcpy(s, "\"");
    in   = TRUE;
    done = FALSE;
    for (i = 1; (done == FALSE) && (i < ns); i++)
        {c = SS_input_synt(si, (char *) shlrc_text);
         switch (c)
            {case '\n':
             case '\r':
             case '\f':
             case '\t':
             case ' ':
                  s[i] = c;
                  break;
             default:
                  if (in == TRUE)
		     s[i] = c;
		  else
		     {SS_unput_synt(si, c);
		      done = TRUE;};
                  break;
             case '\\':
                  s[i++] = c;
                  s[i]   = SS_input_synt(si, (char *) shlrc_text);
                  break;
             case '"':
                  s[i] = c;
                  in   = !in;
                  break;};

	 if (i >= ns-1)
	    {ns += MAX_BFSZ;
	     REMAKE_N(s, char, ns);};};

    s[i-1] = '\0';
    rv     = SS_mk_string_synt(si, s);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         END OF EXTRAS SECTION                            */

/*--------------------------------------------------------------------------*/

