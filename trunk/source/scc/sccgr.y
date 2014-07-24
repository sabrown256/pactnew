%{

/*
 * SCCGR.Y - grammar for C syntax parsing for SCC
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

/* Note on conflicts in this grammar
 * properly C has one shift/reduce conflict which is the usual if/else
 * there are 2 shift/reduce conflicts in this grammar
 *   1)   the canonical if/else
 *   2)   an additional if/declaration conflict to allow
 *        interleaved if expressions and variable declarations
 *        in interpreted C code
 */

/*--------------------------------------------------------------------------*/

/*                           DEFINITIONS SECTION                            */

/*--------------------------------------------------------------------------*/

#include "scc_int.h"

#undef CONST
#undef SIGNED

#ifdef ANSI
#define YY_USE_PROTOS
#endif

#define YYSTYPE     exprp
#define YYDEBUG     1

#define sccgr_lex   scclr_lex
#define sccgr_lval  scclr_lval

#define yyerror(x)  CC_parse_error_c(x, CC_parse_token_c);

/* assign X to yyval and _CC_c_val because in bison yyval is
 * a local variable in yyparse whereas in yacc it is file static
 */

#define CC_GR_VAL(_x)  {yyval = _x; _CC_c_val = yyval;}

extern int
 yydebug;

static expr
 *_CC_c_val = NULL;

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
       YYACCEPT;}

  | external_declaration
      {yylval = $1;
       DIAGNOSTIC(yylval, "external-declaration");
       YYACCEPT;}

/* NOTE: CPP extension */
  | directive
      {DIAGNOSTIC($$, "directive");
       YYACCEPT;}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXTERNAL_DECLARATION - from ANSI C grammar in K&R 2nd */

external_declaration :
    function_definition
      {DIAGNOSTIC($$, "function-definition");
       _CC_do_fnc($$);}

  | declaration
      {DIAGNOSTIC($$, "declaration");
       /* _CC_do_decl($$); */}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FUNCTION_DEFINITION - from ANSI C grammar in K&R 2nd */

function_definition :
    declaration_specifiers declarator declaration_list compound_statement
      {$$ = _CC_var_decl($1, $2, CC_FNC);
       DIAGNOSTIC($$, "DDDC");}

  | declaration_specifiers declarator compound_statement
      {$$ = _CC_var_decl($1, $2, CC_FNC);
       DIAGNOSTIC($$, "DDC(2)");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATION - from ANSI C grammar in K&R 2nd */

declaration :
    declaration_specifiers init_declarator_list ';'
      {$$ = _CC_var_decl($1, $2, CC_VAR);
       DIAGNOSTIC($$, "declaration-specifiers init-declarator-list");}

  | declaration_specifiers ';'
      {DIAGNOSTIC($$, "declaration-specifiers");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATION_LIST - from ANSI C grammar in K&R 2nd */

declaration_list :
    declaration
      {DIAGNOSTIC($$, "declaration");}

  | declaration_list declaration
      {DIAGNOSTIC($$, "declaration-list declaration");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATION_SPECIFIERS - from ANSI C grammar in K&R 2nd */

declaration_specifiers :
    storage_class_specifier
      {DIAGNOSTIC($$, "storage-class-specifier");}

  | storage_class_specifier declaration_specifiers
      {$$ = $2;
       DIAGNOSTIC($$, "storage-class-specifier declaration-specifiers");}

  | type_specifier
      {_CC_obj_decl($$, FALSE);
       DIAGNOSTIC($$, "type-specifier");}

  | type_specifier declaration_specifiers
      {$$ = $2;
       DIAGNOSTIC($$, "type-specifier declaration-specifiers");}

  | type_qualifier
      {DIAGNOSTIC($$, "type-qualifier");}

  | type_qualifier declaration_specifiers
      {$$ = $2;
       DIAGNOSTIC($$, "type-qualifier declaration-specifiers");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STORAGE_CLASS_SPECIFIER - from ANSI C grammar in K&R 2nd */

storage_class_specifier :
    STORAGE_CLASS
      {$$ = _CC_stor_decl($1);
       DIAGNOSTIC($$, "STORAGE CLASS");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TYPE_SPECIFIER - from ANSI C grammar in K&R 2nd */

type_specifier :
    TYPE_SPEC
      {$$ = _CC_type_decl($1);
       DIAGNOSTIC($$, "TYPE-SPEC");}

  | struct_or_union_specifier
      {$$ = _CC_type_decl($1);
       DIAGNOSTIC($$, "struct-union-specifier");}

  | enum_specifier
      {$$ = _CC_type_decl($1);
       DIAGNOSTIC($$, "enum-specifier");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TYPE_QUALIFIER - from ANSI C grammar in K&R 2nd */

type_qualifier :
    TYPE_QUALIFIER
      {$$ = _CC_qual_decl($1);
       DIAGNOSTIC($$, "TYPE-QUALIFIER");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_OR_UNION_SPECIFIER - from ANSI C grammar in K&R 2nd */

struct_or_union_specifier :
    struct_or_union VARNAME '{' struct_declaration_list '}'
      {$$ = _CC_der_decl($1, $2);
       DIAGNOSTIC($$, "struct-or-union VARNAME { struct-declaration-list }");}

  | struct_or_union '{' struct_declaration_list '}'
      {$$ = _CC_der_decl($1, CC_anon);
       DIAGNOSTIC($$, "struct-or-union { struct-declaration-list }");}

  | struct_or_union VARNAME
      {$$ = _CC_der_decl($1, $2);
       DIAGNOSTIC($$, "struct-or-union VARNAME");}

  | struct_or_union TYPE_SPEC
      {$$ = _CC_der_decl($1, $2);
       DIAGNOSTIC($$, "struct-or-union TYPE-SPEC");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_OR_UNION - from ANSI C grammar in K&R 2nd */

struct_or_union :
    STRUCT
      {$$ = CC_struct;
       DIAGNOSTIC($$, "STRUCT");}

  | UNION
      {$$ = CC_union;
       DIAGNOSTIC($$, "UNION");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_DECLARATION_LIST - from ANSI C grammar in K&R 2nd */

struct_declaration_list :
    struct_declaration
      {DIAGNOSTIC($$, "struct-declaration");}

  | struct_declaration_list struct_declaration
      {DIAGNOSTIC($$, "struct-declaration-list struct-declaration");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_DECLARATOR_LIST - from ANSI C grammar in K&R 2nd */

init_declarator_list :
    init_declarator
      {/* $$ = _CC_name_decl(NULL, $1, CC_VAR); */
       _CC_obj_decl($1, TRUE);
       DIAGNOSTIC($$, "init-declarator");}

  | init_declarator_list ',' init_declarator
      {/* $$ = _CC_add_var($1, $3); */
       _CC_obj_decl($3, TRUE);
       DIAGNOSTIC($$, "init-declarator , init-declarator");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_DECLARATOR - from ANSI C grammar in K&R 2nd */

init_declarator :
    declarator
      {DIAGNOSTIC($$, "declarator");}

  | declarator '=' initializer
      {DIAGNOSTIC($$, "declarator = initializer");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_DECLARATION - from ANSI C grammar in K&R 2nd */

struct_declaration :
    specifier_qualifier_list struct_declarator_list ';'
      {$$ = _CC_mem_decl($1, $2);
       DIAGNOSTIC($$, "specifier-qualifier-list");}

/* NOTE: GNU extension */
  | struct_or_union_specifier ';'
      {DIAGNOSTIC($$, "struct-or-union-specifier");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SPECIFIER_QUALIFIER_LIST - from ANSI C grammar in K&R 2nd */

specifier_qualifier_list :
    type_specifier specifier_qualifier_list
      {DIAGNOSTIC($2, "type-specifier specifier-qualifier-list");}

  | type_specifier
      {DIAGNOSTIC($$, "type-specifier");}

  | type_qualifier specifier_qualifier_list
      {DIAGNOSTIC($$, "type-qualifier specifier-qualifier-list");}

  | type_qualifier
      {DIAGNOSTIC($$, "type-qualifier");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_DECLARATOR_LIST - from ANSI C grammar in K&R 2nd */

struct_declarator_list :
    struct_declarator
      {DIAGNOSTIC($$, "struct-declarator");}

  | struct_declarator_list ',' struct_declarator
      {DIAGNOSTIC($$, "struct-declarator-list , struct-declarator");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRUCT_DECLARATOR - from ANSI C grammar in K&R 2nd */

struct_declarator :
    declarator
      {DIAGNOSTIC($$, "declarator");}

  | declarator ':' constant_expression
      {DIAGNOSTIC($$, "declarator : const");}

  | ':' constant_expression
      {DIAGNOSTIC($$, ": const");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENUM_SPECIFIER - from ANSI C grammar in K&R 2nd */

enum_specifier :
    ENUM VARNAME '{' enumerator_list '}'
      {$$ = _CC_der_decl(CC_enum, $2);
       DIAGNOSTIC($$, "enum_specifier : VARNAME { enumerator_list }");}

  | ENUM '{' enumerator_list '}'
      {$$ = _CC_der_decl(CC_enum, CC_anon);
       DIAGNOSTIC($$, "enum_specifier : { enumerator_list }");}

  | ENUM VARNAME '{' enumerator_list ',' '}'
      {$$ = _CC_der_decl(CC_enum, $2);
       DIAGNOSTIC($$, "enum_specifier : { enumerator_list }");}

  | ENUM '{' enumerator_list ',' '}'
      {$$ = _CC_der_decl(CC_enum, CC_anon);
       DIAGNOSTIC($$, "enum_specifier : { enumerator_list }");}

  | ENUM VARNAME
      {$$ = _CC_der_decl(CC_enum, $2);
       DIAGNOSTIC($$, "enum_specifier : VARNAME");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENUMERATOR_LIST - from ANSI C grammar in K&R 2nd */

enumerator_list :
    enumerator
      {DIAGNOSTIC($$, "enumerator_list : enumerator");}

  | enumerator_list ',' enumerator
      {DIAGNOSTIC($$, "enumerator_list : enumerator_list , enumerator");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ENUMERATOR - from ANSI C grammar in K&R 2nd */

enumerator :
    VARNAME
      {DIAGNOSTIC($$, "enumerator : VARNAME");}

  | VARNAME '=' constant_expression
      {DIAGNOSTIC($$, "enumerator : VARNAME = constant_expression");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DECLARATOR - from ANSI C grammar in K&R 2nd */

declarator :
    pointer direct_declarator
      {$$ = $2;
       DIAGNOSTIC($$, "pointer direct-declarator");}

  | direct_declarator
      {DIAGNOSTIC($$, "direct-declarator");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIRECT_DECLARATOR - from ANSI C grammar in K&R 2nd */

direct_declarator :
    variable
      {$$ = _CC_name_decl(NULL, $1, CC_IDENT);
       DIAGNOSTIC($$, "variable");}

  | '(' declarator ')'
      {$$ = $2;
       DIAGNOSTIC($$, "( declarator )");}

  | direct_declarator '[' constant_expression ']'
      {$$ = $1;
       DIAGNOSTIC($$, "direct-declarator [ constant-expression ]");}

  | direct_declarator '[' ']'
      {$$ = $1;
       DIAGNOSTIC($$, "direct-declarator []");}

  | direct_declarator '(' parameter_type_list ')'
      {$$ = _CC_fnc_decl($1);
       DIAGNOSTIC($$, "direct-declarator ( parameter-type-list )");}

  | direct_declarator '(' identifier_list ')'
      {$$ = $1;
       DIAGNOSTIC($$, "direct-declarator ( identifier-list )");}

  | direct_declarator '(' ')'
      {$$ = $1;
       DIAGNOSTIC($$, "direct-declarator ()");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POINTER - from ANSI C grammar in K&R 2nd */

pointer :
    '*' type_qualifier_list
      {DIAGNOSTIC($$, "* type-qualifier-list");}

  | '*'
      {DIAGNOSTIC($$, "*");}

  | '*' type_qualifier_list pointer
      {DIAGNOSTIC($$, "* type-qualifier-list pointer");}

  | '*' pointer
      {DIAGNOSTIC($$, "* pointer");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TYPE_QUALIFIER_LIST - from ANSI C grammar in K&R 2nd */

type_qualifier_list :
    TYPE_QUALIFIER
      {DIAGNOSTIC($$, "TYPE-QUALIFIER");}

  | type_qualifier_list TYPE_QUALIFIER
      {DIAGNOSTIC($$, "type-qualifier-list TYPE-QUALIFIER");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARAMETER_TYPE_LIST - from ANSI C grammar in K&R 2nd */

parameter_type_list :
    parameter_list
      {DIAGNOSTIC($$, "parameter-list");}

  | parameter_list ',' DOTDOT
      {DIAGNOSTIC($$, "parameter-list ...");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARAMETER_LIST - from ANSI C grammar in K&R 2nd */

parameter_list :
    parameter_declaration
      {DIAGNOSTIC($$, "parameter-declaration");}

  | parameter_list ',' parameter_declaration
      {DIAGNOSTIC($$, "parameter-list , parameter-declaration");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARAMETER_DECLARATION - from ANSI C grammar in K&R 2nd */

parameter_declaration :
    declaration_specifiers declarator
      {$$ = _CC_var_decl($1, $2, CC_ARG);
       DIAGNOSTIC($$, "declaration-specifiers declator");}

  | declaration_specifiers abstract_declarator
      {DIAGNOSTIC($$, "declaration-specifiers abstract-declarator");}

  | declaration_specifiers
      {DIAGNOSTIC($$, "declaration-specifiers");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IDENTIFIER_LIST - from ANSI C grammar in K&R 2nd */

identifier_list :
    variable
      {DIAGNOSTIC($$, "variable");}

  | identifier_list ',' variable
      {DIAGNOSTIC($$, "identifier-list , variable");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INITIALIZER - from ANSI C grammar in K&R 2nd */

initializer :
    assignment_expression
      {DIAGNOSTIC($$, "assignment-expression");}

  | '{' initializer_list '}'
      {DIAGNOSTIC($$, "{initializer-list}");}

  | '{' initializer_list ',' '}'
      {DIAGNOSTIC($$, "{initializer-list ,}");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INITIALIZER_LIST - from ANSI C grammar in K&R 2nd */

initializer_list :
    initializer
      {DIAGNOSTIC($$, "initializer");}

  | initializer_list ',' initializer
      {DIAGNOSTIC($$, "initializer-list , initializer");}
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
      {DIAGNOSTIC($$, "pointer");}

  | pointer direct_abstract_declarator
      {DIAGNOSTIC($$, "pointer direct-abst-decl");}

  | direct_abstract_declarator
      {DIAGNOSTIC($$, "direct-abst-decl");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DIRECT_ABSTRACT_DECLARATOR - from ANSI C grammar in K&R 2nd */

direct_abstract_declarator :
    '(' abstract_declarator ')'
      {DIAGNOSTIC($$, "( abst-decl )");}

  | direct_abstract_declarator '[' constant_expression ']'
      {DIAGNOSTIC($$, "direct-abst-decl [ ... ]");}

  | direct_abstract_declarator '[' ']'
      {DIAGNOSTIC($$, "direct-abst-decl []");}

  | '[' constant_expression ']'
      {DIAGNOSTIC($$, "[ const-expr ]");}

  | '[' ']'
      {DIAGNOSTIC($$, "[]");}

  | direct_abstract_declarator '(' parameter_type_list ')'
      {DIAGNOSTIC($$, "direct-abst-decl ( ... )");}

  | direct_abstract_declarator '(' ')'
      {DIAGNOSTIC($$, "direct-abst-decl ()");}

  | '(' parameter_type_list ')'
      {DIAGNOSTIC($$, "( ... )");}

  | '(' ')'
      {DIAGNOSTIC($$, "()");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STATEMENT - from ANSI C grammar in K&R 2nd */

statement :
    labeled_statement
      {DIAGNOSTIC($$, "labeled-statement");}

  | expression_statement
      {DIAGNOSTIC($$, "expression-statement");}

  | compound_statement
      {DIAGNOSTIC($$, "compound-statement");}

  | selection_statement
      {DIAGNOSTIC($$, "selection-statement");}

  | iteration_statement
      {DIAGNOSTIC($$, "iteration-statement");}

  | jump_statement
      {DIAGNOSTIC($$, "jump-statement");}

  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LABELED_STATEMENT - from ANSI C grammar in K&R 2nd */

labeled_statement :
    variable ':' statement
      {DIAGNOSTIC($$, "variable : statement");}

  | CASE constant_expression ':' statement
      {DIAGNOSTIC($$, "CASE constant_expression : statement");}

  | DEFAULT ':' statement
      {DIAGNOSTIC($$, "DEFAULT : statement");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPRESSION_STATEMENT - from ANSI C grammar in K&R 2nd */

expression_statement :
    expression ';'
      {DIAGNOSTIC($$, "expression");}

  | ';'
      {DIAGNOSTIC($$, ";");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPOUND_STATEMENT - from ANSI C grammar in K&R 2nd */

compound_statement :
    '{' declaration_list statement_list '}'
      {DIAGNOSTIC($$, "{ declaration-list statement-list }");}

  | '{' declaration_list '}'
      {DIAGNOSTIC($$, "{ declaration-list }");}

  | '{' statement_list '}'
      {DIAGNOSTIC($$, "{ statement-list }");}

  | '{' '}'
      {DIAGNOSTIC($$, "{ }");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STATEMENT_LIST - from ANSI C grammar in K&R 2nd */

statement_list :
    statement
      {DIAGNOSTIC($$, "statement");}

  | statement_list statement
      {DIAGNOSTIC($$, "statement-list statement");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SELECTION_STATEMENT - from ANSI C grammar in K&R 2nd */

selection_statement :
    IF '(' expression ')' statement
      {DIAGNOSTIC($$, "if (expr) statement");}

/* GOTCHA: this adds one shift/reduce conflict to the grammar
 * this is the canonical if/else conflict
 */
  | IF '(' expression ')' statement ELSE statement
      {DIAGNOSTIC($$, "if (expr) statement else statement");}

  | SWITCH '(' expression ')' statement
      {DIAGNOSTIC($$, "switch (expr) statement");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ITERATION_STATEMENT - from ANSI C grammar in K&R 2nd */

iteration_statement :
    WHILE '(' expression ')' statement
      {DIAGNOSTIC($$, "while (expr) statement");}

  | DO statement WHILE '(' expression ')' ';'
      {DIAGNOSTIC($$, "do statement while (expr)");}

  | FOR '(' expression ';' expression ';' expression ')' statement
      {DIAGNOSTIC($$, "for (expr; expr; expr) statement");}

  | FOR '(' ';' expression ';' expression ')' statement
      {DIAGNOSTIC($$, "for (; expr; expr) statement");}

  | FOR '(' expression ';' ';' expression ')' statement
      {DIAGNOSTIC($$, "for (expr; ; expr) statement");}

  | FOR '(' expression ';' expression ';' ')' statement
      {DIAGNOSTIC($$, "for (expr; expr;) statement");}

  | FOR '(' expression ';' ';' ')' statement
      {DIAGNOSTIC($$, "for (expr; ;) statement");}

  | FOR '(' ';' expression ';' ')' statement
      {DIAGNOSTIC($$, "for (; expr;) statement");}

  | FOR '(' ';' ';' expression ')' statement
      {DIAGNOSTIC($$, "for (; ; expr) statement");}

  | FOR '(' ';' ';' ')' statement
      {DIAGNOSTIC($$, "for (; ;) statement");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JUMP_STATEMENT - from ANSI C grammar in K&R 2nd */

jump_statement :
    GOTO variable ';'
      {DIAGNOSTIC($$, "goto variable");}

  | CONTINUE ';'
      {DIAGNOSTIC($$, "CONTINUE");}

  | BREAK ';'
      {DIAGNOSTIC($$, "BREAK");}

  | RETURN expression ';'
      {DIAGNOSTIC($$, "RETURN expression");}

  | RETURN ';'
      {DIAGNOSTIC($$, "RETURN");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXPRESSION - expression of ANSI C grammar in K&R 2nd */

expression :
    assignment_expression
      {DIAGNOSTIC($$, "assignment-expression");}

  | expression ',' assignment_expression
      {DIAGNOSTIC($$, "expression , assignment-expression");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASSIGNMENT-EXPRESSION - from ANSI C grammar in K&R 2nd */

assignment_expression :
    conditional_expression

  | unary_expression '=' assignment_expression
      {DIAGNOSTIC($$, "unary = expr");}

  | unary_expression TIMESEQ assignment_expression
      {DIAGNOSTIC($$, "unary *= expr");}

  | unary_expression PLUSEQ assignment_expression
      {DIAGNOSTIC($$, "unary += expr");}

  | unary_expression MINUSEQ assignment_expression
      {DIAGNOSTIC($$, "unary -= expr");}

  | unary_expression DIVEQ assignment_expression
      {DIAGNOSTIC($$, "unary /= expr");}

  | unary_expression MODEQ assignment_expression
      {DIAGNOSTIC($$, "unary %= expr");}

  | unary_expression LSEQ assignment_expression
      {DIAGNOSTIC($$, "unary <<= expr");}

  | unary_expression RSEQ assignment_expression
      {DIAGNOSTIC($$, "unary >>= expr");}

  | unary_expression ANDEQ assignment_expression
      {DIAGNOSTIC($$, "unary &= expr");}

  | unary_expression XOREQ assignment_expression
      {DIAGNOSTIC($$, "unary ^= expr");}

  | unary_expression OREQ assignment_expression
      {DIAGNOSTIC($$, "unary |= expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONDITIONAL_EXPRESSION - from ANSI C grammar in K&R 2nd */

conditional_expression :
    logical_or_expression

  | logical_or_expression '?' expression ':' conditional_expression
      {DIAGNOSTIC($$, "expr ? expr : expr");}
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
      {DIAGNOSTIC($$, "expr || expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOGICAL_AND_EXPRESSION - from ANSI C grammar in K&R 2nd */

logical_and_expression :
    inclusive_or_expression

  | logical_and_expression AND inclusive_or_expression
      {DIAGNOSTIC($$, "expr && expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INCLUSIVE_OR_EXPRESSION - from ANSI C grammar in K&R 2nd */

inclusive_or_expression :
    exclusive_or_expression

  | inclusive_or_expression '|' exclusive_or_expression
      {DIAGNOSTIC($$, "expr | expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXCLUSIVE_OR_EXPRESSION - modification of ANSI C grammar in K&R 2nd
 *                         - use ^ as exponentiation (e.g. pow) operator
 *                         - when arguments are not both integers
 *                         - remove from here for proper precedence wrt
 *                         - arithmetic operations
 */

exclusive_or_expression :
    and_expression

  | exclusive_or_expression XOR and_expression
      {DIAGNOSTIC($$, "expr ^ expr");}

  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* AND_EXPRESSION - from ANSI C grammar in K&R 2nd */

and_expression :
    equality_expression

  | and_expression '&' equality_expression
      {DIAGNOSTIC($$, "expr & expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EQUALITY_EXPRESSION - from ANSI C grammar in K&R 2nd */

equality_expression :
    relational_expression

  | equality_expression EQ relational_expression
      {DIAGNOSTIC($$, "expr == expr");}

  | equality_expression NEQ relational_expression
      {DIAGNOSTIC($$, "expr != expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RELATIONAL_EXPRESSION - from ANSI C grammar in K&R 2nd */

relational_expression :
    shift_expression

  | relational_expression '<' shift_expression
      {DIAGNOSTIC($$, "expr < expr");}

  | relational_expression '>' shift_expression
      {DIAGNOSTIC($$, "expr > expr");}

  | relational_expression LE shift_expression
      {DIAGNOSTIC($$, "expr <= expr");}

  | relational_expression GE shift_expression
      {DIAGNOSTIC($$, "expr >= expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SHIFT_EXPRESSION - from ANSI C grammar in K&R 2nd */

shift_expression :
    additive_expression

  | shift_expression LS additive_expression
      {DIAGNOSTIC($$, "expr << expr");}

  | shift_expression RS additive_expression
      {DIAGNOSTIC($$, "expr >> expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ADDITIVE_EXPRESSION - from ANSI C grammar in K&R 2nd */

additive_expression :
    multiplicative_expression

  | additive_expression '+' multiplicative_expression
      {DIAGNOSTIC($$, "expr + expr");}

  | additive_expression '-' multiplicative_expression
      {DIAGNOSTIC($$, "expr - expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MULTIPLICATIVE_EXPRESSION - from ANSI C grammar in K&R 2nd */

multiplicative_expression :
    exponentiation_expression

  | multiplicative_expression '*' exponentiation_expression
      {DIAGNOSTIC($$, "expr * expr");}

  | multiplicative_expression '/' exponentiation_expression
      {DIAGNOSTIC($$, "expr / expr");}

  | multiplicative_expression '%' exponentiation_expression
      {DIAGNOSTIC($$, "expr % expr");}
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
      {DIAGNOSTIC($$, "expr ^ expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CAST_EXPRESSION - from ANSI C grammar in K&R 2nd */

cast_expression :
    unary_expression
      {DIAGNOSTIC($$, "cast");}

  | '(' type_name ')' cast_expression
      {DIAGNOSTIC($$, "(type) cast_expression");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNARY_EXPRESSION - from ANSI C grammar in K&R 2nd */

unary_expression :
    postfix_expression

  | INC unary_expression
      {DIAGNOSTIC($$, "++expr");}

  | DEC unary_expression
      {DIAGNOSTIC($$, "--expr");}

  | SIZOF unary_expression
      {DIAGNOSTIC($$, "sizeof(expr)");}

  | SIZOF '(' type_name ')'
      {DIAGNOSTIC($$, "sizeof(type)");}

  | '&' cast_expression
      {DIAGNOSTIC($$, "&cast-expr");}

  | '*' cast_expression
      {DIAGNOSTIC($$, "*cast-expr");}

  | '+' cast_expression
      {DIAGNOSTIC($$, "+cast-expr");}

  | '-' cast_expression
      {DIAGNOSTIC($$, "-cast-expr");}

  | '!' cast_expression
      {DIAGNOSTIC($$, "!cast-expr");}

  | '~' cast_expression
      {DIAGNOSTIC($$, "~cast-expr");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POSTFIX_EXPRESSION - from ANSI C grammar in K&R 2nd */

postfix_expression :
    primary_expression

  | postfix_expression '[' expression ']'
      {DIAGNOSTIC($$, "[expr]");}

  | postfix_expression '(' argument_expression_list ')'
      {DIAGNOSTIC($$, "(argl)");}

  | postfix_expression '(' ')'
      {DIAGNOSTIC($$, "()");}

  | postfix_expression DOT variable
      {_CC_var_def($3, TRUE);
       DIAGNOSTIC($$, "expr.var");}

  | postfix_expression ACC variable
      {_CC_var_def($3, TRUE);
       DIAGNOSTIC($$, "expr->var");}

  | postfix_expression INC
      {DIAGNOSTIC($$, "expr++");}

  | postfix_expression DEC
      {DIAGNOSTIC($$, "expr--");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRIMARY_EXPRESSION - from ANSI C grammar in K&R 2nd */

primary_expression :
    variable
      {DIAGNOSTIC($$, "variable");}

  | constant
      {DIAGNOSTIC($$, "constant");}

  | string                
      {DIAGNOSTIC($$, "string");}

  | '(' expression ')'
      {DIAGNOSTIC($$, "( expression )");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ARGUMENT_EXPRESSION_LIST - from ANSI C grammar in K&R 2nd */

argument_expression_list :
    assignment_expression
      {DIAGNOSTIC($$, "assign-expression");}

  | argument_expression_list ',' assignment_expression
      {DIAGNOSTIC($$, "arg, assign-expression");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VARIABLE - from ANSI C grammar in K&R 2nd */

variable :
    VARNAME
      {DIAGNOSTIC($$, "VARNAME");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STRING - from ANSI C grammar in K&R 2nd */

string :
    STRING
      {DIAGNOSTIC($$, "STRING");}
  ;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONSTANT - from ANSI C grammar in K&R 2nd */

constant :
    INTEGER
      {DIAGNOSTIC($$, "INTEGER");}

  | FLOAT
      {DIAGNOSTIC($$, "FLOAT");}
  ;

/*--------------------------------------------------------------------------*/

/*                           CPP EXTENSIONS                                 */

/*--------------------------------------------------------------------------*/

/* DIRECTIVE - CPP '#x' syntax */

directive :
    DER NL
      {DIAGNOSTIC($$, "# directive");}
  ;

/*--------------------------------------------------------------------------*/

%%

/*--------------------------------------------------------------------------*/

/*                             EXTRAS SECTION                               */

/*--------------------------------------------------------------------------*/

#undef YYACCEPT

int
 CC_c_tokens[] = { TYPE_SPEC, VARNAME };

static expr
 *CC_parse_string_c();

#include "scclr.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_PARSE_TOKEN_C - return YYVAL portably */

expr *CC_parse_token_c(void)
   {

    return(_CC_c_val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_PARSE_TOKEN_VAL_C - return YYLVAL portably */

expr *CC_parse_token_val_c(void)
   {

    return(yylval);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_PARSE_DEBUG_C - return a pointer to YYDEBUG portably */

int *CC_parse_debug_c(void)
   {

    return(&yydebug);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CC_PARSE_STRING_C - return a string token portably
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

expr *CC_parse_string_c()
   {int i, in, done;
    char c;
    expr *rv;
    static char *s = NULL;
    static int ns = 0L;

    if (s == NULL)
       {ns = MAX_BFSZ;
	s  = CMAKE_N(char, ns);
	memset(s, 0, ns);};

    SC_strncpy(s, ns, "\"", -1);
    in   = TRUE;
    done = FALSE;
    for (i = 1; (done == FALSE) && (i < ns); i++)
        {c = CC_input_c((char *) scclr_text);
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
		     {CC_unput_c(c);
		      done = TRUE;};
                  break;
             case '\\':
                  s[i++] = c;
                  s[i]   = CC_input_c((char *) scclr_text);
                  break;
             case '"':
                  s[i] = c;
                  in   = !in;
                  break;};

	 if (i >= ns-1)
	    {ns += MAX_BFSZ;
	     REMAKE_N(s, char, ns);};};

    s[i-1] = '\0';
    rv     = CC_mk_string_c(s);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         END OF EXTRAS SECTION                            */

/*--------------------------------------------------------------------------*/

