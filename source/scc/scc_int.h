/*
 * SCC_INT.H - internal support for SCC
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_SCC_INT
#define PCK_SCC_INT

#include "scc.h"

/*--------------------------------------------------------------------------*/

/*                                   MACROS                                 */

/*--------------------------------------------------------------------------*/

#define DIAGNOSTIC(x, y) _CC_diagnostic_c(x, y, _CC.diagnose_grammar)

/*--------------------------------------------------------------------------*/

/*                            STRUCT AND TYPEDEF                            */

/*--------------------------------------------------------------------------*/

enum e_qtype
   {CC_UNDEF,
    CC_TYPEQ,
    CC_STORAGE,
    CC_EXTENSION,
    CC_IDENT,
    CC_DECL,
    CC_DEF,
    CC_ARG,
    CC_VAR,
    CC_TYP,
    CC_FNC,

/* storage classes */
    CC_AUTO,
    CC_EXTERN,
    CC_REGISTER,
    CC_STATIC,
    CC_TYPEDEF,

/* type qualifiers */
    CC_CONST,
    CC_VOLATILE,

    CC_PRIMITIVE,
    CC_DERIVED,
    CC_STRUCT,
    CC_UNION,
    CC_ENUM};

typedef enum e_qtype qtype;

enum e_vtype
   {CC_INT,
    CC_FLOAT,
    CC_CHAR,
    CC_STRING,
    CC_VARIABLE,
    CC_PARAMETER,
    CC_MEMBER,
    CC_TYPE,
    CC_FUNCTION};

typedef enum e_vtype vtype;

typedef struct s_CC_scope_private CC_scope_private;
typedef struct s_decl decl;
typedef struct s_expr expr;
typedef struct s_file_loc file_loc;
typedef expr *exprp;

struct s_file_loc
   {int iln;                  /* file line number */
    char *fname;};            /* file name */

struct s_expr
   {qtype declared;
    vtype cat;
    char *type;
    char *name;
    void *ptr;};

struct s_decl
   {int lb;
    int le;
    int vlb;
    int ncx;
    qtype kind;        /* variable or function */
    qtype dec;         /* definition or declaration */
    qtype qual;        /* type qualifier */
    qtype stor;        /* storage class */
    qtype cat;         /* struct, union, enum, or primitive */
    char *type;
    char *name;
    char *out;
    char *text;
    int *dep;
    hasharr *refobjs;};

struct s_CC_scope_private
   {int mode_init;            /* has the C mode been initialized */
    int cpp_define;           /* processing a #define */
    int cpp_directive;        /* parsing a CPP directive */
    int diagnose_grammar;     /* debug flag - print parse results */
    int check_decls;          /* check declarations only */
    int have_eof;
    int n_error;

    int ich;                  /* current character in declaration */
    char *lex_text;
    FILE *fp;
    file_loc rloc;            /* real location */
    file_loc vloc;            /* virtual location */

    int ncbr;                  /* depth of nested {} */
    int nsbr;                  /* depth of nested [] */
    int nprn;                  /* depth of nested () */

    int nd;
    int ndx;
    decl *cur;
    decl **all;

    hasharr *consts;
    hasharr *types;           /* all types encountered in parse */
    hasharr *primitives;      /* primitive C types */
    hasharr *depend;          /* objects on which current decl depends */

    JMP_BUF cpu;};

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern CC_scope_private
 _CC;

extern int
 CC_c_tokens[];

extern expr
 *CC_struct,
 *CC_union,
 *CC_enum,
 *CC_anon,
 *CC_eof,
 *CC_null,
 *CC_t,
 *CC_f;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern expr
 *CC_parse_token_c(void),
 *CC_parse_token_val_c(void);


/* SCCHK.C declarations */

extern int
 CC_check(char *cmp, char **v);


/* SCCEM.C declarations */

extern int
 CC_preprocess(char *s, char *cmp, char **v),
 CC_compile(char *s, char *cmp, char **v),
 CC_emit(char *cmp, char **v, int cfl);


/* SHGRC.Y declarations */

extern int
 sccgr_parse(void);

extern int
 *CC_parse_debug_c(void);


/* SCCINF.C declarations */

extern int
 _CC_var_def(expr *e, int d),
 CC_compile_file(char *name, char *cmp, char **v);

extern void
 _CC_rem_varc(char *name),
 _CC_def_type(expr *e),
 _CC_obj_decl(expr *e, int force),
 _CC_do_decl(expr *e),
 _CC_do_fnc(expr *e),
 CC_expr_str(char *s, int nc, expr *e);

extern char
 *_CC_clean_text(char *t),
 *CC_variable_name(expr *e);

extern expr
 *_CC_type_decl(expr *t),
 *_CC_name_decl(expr *t, expr *v, qtype k),
 *_CC_add_var(expr *l, expr *v),
 *_CC_var_decl(expr *t, expr *v, qtype wh),
 *_CC_der_decl(expr *t, expr *v),
 *_CC_mem_decl(expr *t, expr *v),
 *_CC_fnc_decl(expr *e),
 *_CC_stor_decl(expr *e),
 *_CC_qual_decl(expr *e),
 *CC_add_depend(char *name, qtype t),
 *CC_add_type(char *name, qtype t),
 *CC_add_const(expr *e),
 *CC_lookup_variable(char *txt, qtype q, int verbose),
 *CC_copy_expr(expr *e),
 *CC_mk_char(char *s),
 *CC_mk_string(char *s),
 *CC_mk_type(char *t),
 *CC_mk_variable(char *type, char *name, qtype declared),
 *CC_mk_integer(int64_t l),
 *CC_mk_float(double d);


/* SCCSUP.C declarations */

extern decl
 *CC_mk_decl(int ln, int nc);

extern void
 CC_diagnose_parse(int sw),
 CC_init_c_syntax_mode(void),
 _CC_free_exprs(hasharr *vt),
 CC_rl_decl(decl *pd),
 CC_rl_expr(expr *e),
 CC_finish(void);

extern int
 CC_parse(FILE *fp);


/* SCCSYNT.C declarations */

extern expr
 *CC_mk_string_c(char *s);

extern int
 CC_input_c(char *ltxt);

extern void
 CC_unput_c(int c),
 CC_reckon_line(void);

#endif
