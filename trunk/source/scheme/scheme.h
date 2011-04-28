/*
 * SCHEME.H - header file supporting Scheme system
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCHEME

#include "cpyright.h"

#define PCK_SCHEME

/* #define SCHEME_DEBUG */

#ifndef CODE
# define CODE     "SCHEME V"
#endif

#include "pml.h"
#include "scope_quaternion.h"

#define LARGE

#ifdef LARGE
# include "ppc.h"
#endif

#if 1
# define PROC_OBJ        126
#endif

/* #define STACK_FNC */

/*--------------------------------------------------------------------------*/

/*                           STRUCTs and TYPEDEFs                           */

/*--------------------------------------------------------------------------*/

/* SCHEME level special form definers */

enum e_SS_form
   {SS_MACRO_EV,          /* define-ue */
    SS_MACRO,             /* define-macro */
    SS_ESC_PROC,
    SS_BEGIN,

/* C level special form definers */
    SS_EE_MACRO,          /* apply */
    SS_UE_MACRO,          /* let, let* */
    SS_UR_MACRO,          /* defined?, err-catch, ... */
    SS_DEFINE,            /* define */

    SS_SET,
    SS_COND,
    SS_IF,
    SS_AND,
    SS_OR,
    SS_PROC,
    SS_PR_PROC};

typedef enum e_SS_form SS_form;

enum e_SS_eval_mode
   {NO_EV,
    SELF_EV,
    VAR_EV,
    PROC_EV};

typedef enum e_SS_eval_mode SS_eval_mode;


/* Generic object definition */

typedef struct s_object object;

struct s_object
   {char *print_name;            /* specific members */
    void *val;

    SS_eval_mode eval_type;      /* generic members */
    void (*print)(object *obj, object *strm);
    void (*release)(object *obj);};

FUNCTION_POINTER(object, (*PFObject));
FUNCTION_POINTER(object, *(*PFPObject));

typedef enum e_SS_io_logging SS_io_logging;
typedef struct s_SS_input_port input_port;
typedef struct s_SS_output_port output_port;
typedef struct s_SS_cons cons;
typedef struct s_SS_variable variable;
typedef struct s_SS_boolean SS_boolean;
typedef struct s_SS_string string;
typedef struct s_SS_continuation continuation;
typedef struct s_SS_err_continuation err_continuation;
typedef struct s_SS_esc_proc Esc_procedure;
typedef struct s_SS_C_proc C_procedure;
typedef struct s_SS_S_proc S_procedure;
typedef struct s_SS_proc procedure;
typedef struct s_SS_vect vector;

typedef object *(*PFPHand)(C_procedure *cp, object *argl);
typedef void (*PFNameReproc)(char *s, char *name);
typedef void (*PFPostRead)(object *strm);
typedef void (*PFPostEval)(object *strm);
typedef int (*PFPostPrint)(void);
typedef int (*PFPrGetS)(object *str);
typedef int (*PFPrChIn)(object *str, int ign_ws);
typedef void (*PFPrChOut)(int c, object *str);
typedef void (*PFPrChUn)(int c, object *str);
typedef void (*PFPrintErrMsg)(FILE *str, char *s, object *obj);
typedef void (*PFExtractArg)(object *obj, void *v, int type);
typedef object *(*PFSSRead)(object *str);
typedef object *(*PFCallArg)(int type, void *v);

#define SS_DEFINE_OBJECT                                                    \
   {defstr *dp;                                                             \
    dp = PD_defstr(file, "object",                                          \
                   "char *print_name",                                      \
                   "char *val",                                             \
                   "int eval_type",                                         \
                   "function print",                                        \
                   "function release",                                      \
                   LAST);                                                   \
    if (dp == NULL)                                                         \
       SS_error("COULDN'T DEFINE OBJECT TO FILE - SS_DEFINE_OBJECT",        \
                SS_null);}

enum e_SS_io_logging
   {STDIN_ONLY,
    ALL,
    NO_LOG};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

struct s_SS_input_port
   {char *name;
    int iln;                       /* current line number in file */
    int ichr;                      /* 1 based char index in current line */
    FILE *str;
    char buffer[MAX_BFSZ];
    char *ptr;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

struct s_SS_output_port
   {char *name;
    FILE *str;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

struct s_SS_cons
   {object *car;
    object *cdr;};

#define SS_DEFINE_CONS \
    PD_defstr(file, "cons", \
              "object *car", \
              "object *cdr", \
              LAST)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

struct s_SS_variable
   {char *name;
    object *value;};

#define SS_DEFINE_VARIABLE        \
    PD_defstr(file, "variable",   \
              "char *name",       \
              "object *value",    \
              LAST)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

struct s_SS_boolean
   {char *name;
    int value;};

#define SS_DEFINE_BOOLEAN         \
    PD_defstr(file, "boolean",    \
              "char *name",       \
              "int value",        \
              LAST)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

struct s_SS_string
   {int length;
    char *string;};

#define SS_DEFINE_STRING          \
    PD_defstr(file, "string",     \
              "int length",       \
              "char *string",     \
              LAST)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

struct s_SS_continuation
   {object *signal;
    JMP_BUF cont;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

struct s_SS_err_continuation
   {int stack_ptr;
    int cont_ptr;
    object *signal;
    JMP_BUF cont;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ESCAPE PROCEDURE - used to implement call-with-current-continuation */

struct s_SS_esc_proc
   {int cont;
    int stck;
    int err;
    int type;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*  C PROCEDURE - primitive procedure or special form (written in C) */

struct s_SS_C_proc
   {int n;
    PFVoid *proc;
    PFPHand handler;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPOUND PROCEDURE - Scheme level procedure (written in Scheme) */

struct s_SS_S_proc
   {SS_form type;
    object *name;
    object *lambda;
    object *env;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

struct s_SS_proc
   {SS_form type;
    char *name;
    char *doc;
    short int trace;
    object *proc;};

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/*  Vector definition */
struct s_SS_vect
   {int length;
    object **vect;};

#define SS_DEFINE_VECTOR         \
    PD_defstr(file, "vector",    \
              "int length",      \
              "object **vect",   \
              LAST)

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/*                            PROCEDURAL MACROS                             */

/*--------------------------------------------------------------------------*/

#define SS_GET(typ, obj)        ((typ *) SS_OBJECT(obj))
#define SS_PP(fun, member)      ((procedure *) SS_OBJECT(fun))->member

#define SS_READ_EXPR(x)                                                      \
    ((_SS_si.read == NULL) ? SS_null : (*_SS_si.read)(x))


/* OBJECT ACCESSORS */

#define SS_INQUIRE_OBJECT(x)      ((object *) SC_hasharr_def_lookup(_SS_si.symtab, (x)))
#define SS_OBJECT_GC(x)           SC_ref_count(x)
#define SS_UNCOLLECT(x)           SC_permanent(x)
#define SS_OBJECT_TYPE(x)         SC_arrtype(x, -1)
#define SS_OBJECT_NAME(x)         ((x)->print_name)
#define SS_OBJECT(x)              ((x)->val)
#define SS_OBJECT_ETYPE(x)        ((x)->eval_type)
#define SS_OBJECT_PRINT(x, strm)  (*((x)->print))(x, strm)
#define SS_OBJECT_FREE(x)         (*((x)->release))(x)

/* NUMBER ACCESSORS */

#define SS_INTEGER_VALUE(x)       *((int64_t *) ((x)->val))
#define SS_FLOAT_VALUE(x)         *((double *) ((x)->val))
#define SS_COMPLEX_VALUE(x)       *((double _Complex *) ((x)->val))
#define SS_QUATERNION_VALUE(x)    *((quaternion *) ((x)->val))

/* INPUT_PORT/OUTPUT_PORT ACCESSORS */

#define SS_OUTSTREAM(x)                                                      \
    ((SS_outportp(x)) ? (((output_port *) (x)->val)->str) : stdout)
#define SS_OFILE_NAME(x)                                                     \
    ((SS_outportp(x)) ? (((output_port *) (x)->val)->name) : NULL)

#define SS_INSTREAM(x)          (((input_port *) (x)->val)->str)
#define SS_BUFFER(x)            (((input_port *) (x)->val)->buffer)
#define SS_PTR(x)               (((input_port *) (x)->val)->ptr)
#define SS_LINE_NUMBER(x)       (((input_port *) (x)->val)->iln)
#define SS_CHAR_INDEX(x)        (((input_port *) (x)->val)->ichr)
#define SS_IFILE_NAME(x)        (((input_port *) (x)->val)->name)

/* CONS ACCESSORS */

#define SS_CAR_MACRO(x)         (((cons *) (x)->val)->car)
#define SS_CDR_MACRO(x)         (((cons *) (x)->val)->cdr)

/* VARIABLE ACCESSORS */

#define SS_VARIABLE_NAME(x)     (((variable *) ((x)->val))->name)
#define SS_VARIABLE_VALUE(x)    (((variable *) ((x)->val))->value)

/* BOOLEAN ACCESSORS */

#define SS_BOOLEAN_NAME(x)      (((SS_boolean *) ((x)->val))->name)
#define SS_BOOLEAN_VALUE(x)     (((SS_boolean *) ((x)->val))->value)

/* STRING ACCESSORS */

#define SS_STRING_TEXT(x)       (((string *) ((x)->val))->string)
#define SS_STRING_LENGTH(x)     (((string *) ((x)->val))->length)

/* PROCEDURE ACCESSORS */

#define SS_PROCEDURE_TYPE(x)    (((procedure *) ((x)->val))->type)
#define SS_PROCEDURE_NAME(x)    (((procedure *) ((x)->val))->name)
#define SS_PROCEDURE_DOC(x)     (((procedure *) ((x)->val))->doc)
#define SS_PROCEDURE_TRACEDP(x) (((procedure *) ((x)->val))->trace)
#define SS_PROCEDURE_PROC(x)    (((procedure *) ((x)->val))->proc)

/* C PROCEDURE ACCESSORS */

#define SS_GET_C_PROCEDURE(x)   ((C_procedure *) SS_PROCEDURE_PROC(x))

#define SS_C_PROCEDURE_HANDLER_PTR(x)                                        \
   (((C_procedure *) SS_PROCEDURE_PROC(x))->handler)
#define SS_C_PROCEDURE_HANDLER(x)                                            \
   (*SS_C_PROCEDURE_HANDLER_PTR(x))
#define SS_C_PROCEDURE_FUNCTION_PTR(x)                                       \
   (((C_procedure *) SS_PROCEDURE_PROC(x))->proc[0])
#define SS_C_PROCEDURE_FUNCTION(x)                                           \
   (*SS_C_PROCEDURE_FUNCTION_PTR(x))

/* COMPOUND PROCEDURE ACCESSORS */

#define SS_COMPOUND_PROCEDURE(x)                                             \
   ((S_procedure *) SS_PROCEDURE_PROC(x))

/* ESCAPE PROCEDURE ACCESSORS */

#define SS_ESCAPE_CONTINUATION(x)                                           \
   (((Esc_procedure *) SS_PROCEDURE_PROC(x))->cont)
#define SS_ESCAPE_STACK(x)                                                  \
   (((Esc_procedure *) SS_PROCEDURE_PROC(x))->stck)
#define SS_ESCAPE_ERROR(x)                                                  \
   (((Esc_procedure *) SS_PROCEDURE_PROC(x))->err)
#define SS_ESCAPE_TYPE(x)                                                   \
   (((Esc_procedure *) SS_PROCEDURE_PROC(x))->type)

#ifdef LARGE

/* VECTOR ACCESSORS */

#define SS_VECTOR_LENGTH(x)  (((vector *) (x)->val)->length)
#define SS_VECTOR_ARRAY(x)   (((vector *) (x)->val)->vect)

/* CHARACTER ACCESSOR */

#define SS_CHARACTER_VALUE(x) *((int *) (x)->val)

/* PROCESS ACCESSORS */

#define SS_PROCESS_VALUE(x)     ((PROCESS *) (x)->val)
#define SS_PROCESS_STATUS(x)   (((PROCESS *) (x)->val)->status)

#endif

/*--------------------------------------------------------------------------*/

/*                           MACRO PREDICATES                               */

/*--------------------------------------------------------------------------*/

/* SS_INTEGERP - return TRUE if the object is of type INTEGER 
 *             - return FALSE otherwise
 */

#define SS_integerp(obj) (SC_arrtype(obj, -1) == SC_INT_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_FLOATP - return TRUE if the object is of type FLOAT 
 *           - return FALSE otherwise
 */

#define SS_floatp(obj) (SC_arrtype(obj, -1) == SC_FLOAT_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_COMPLEXP - return TRUE if the object is of type COMPLEX
 *             - return FALSE otherwise
 */

#define SS_complexp(obj) (SC_arrtype(obj, -1) == SC_DOUBLE_COMPLEX_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_QUATERNIONP - return TRUE if the object is of type QUATERNION
 *                - return FALSE otherwise
 */

#define SS_quaterionp(obj) (SC_arrtype(obj, -1) == SC_QUATERNION_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_STRINGP - return TRUE if the object is of type STRING 
 *            - return FALSE otherwise
 */

#define SS_stringp(obj) (SC_arrtype(obj, -1) == SC_STRING_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CONSP - return TRUE if the object is of type CONS
 *          - return FALSE otherwise
 */

#define SS_consp(obj) (SC_arrtype(obj, -1) == SS_CONS_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_VARIABLEP - return TRUE if the object is of type VARIABLE
 *              - return FALSE otherwise
 */

#define SS_variablep(obj) (SC_arrtype(obj, -1) == SS_VARIABLE_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INPORTP - return TRUE if the object is of type INPUT_PORT
 *            - return FALSE otherwise
 */

#define SS_inportp(obj) (SC_arrtype(obj, -1) == SS_INPUT_PORT_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_OUTPORTP - return TRUE if the object is of type OUTPUT_PORT
 *             - return FALSE otherwise
 */

#define SS_outportp(obj) (SC_arrtype(obj, -1) == SS_OUTPUT_PORT_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_EOFOBJP - return TRUE if the object is of type EOF
 *            - return FALSE otherwise
 */

#define SS_eofobjp(obj) (SC_arrtype(obj, -1) == SS_EOF_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_NULLOBJP - return TRUE if the object is of type NULL
 *             - return FALSE otherwise
 */

#define SS_nullobjp(obj) (SC_arrtype(obj, -1) == SS_NULL_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PROCEDUREP - return TRUE if the object is of type PROC_OBJ
 *               - return FALSE otherwise
 */

#define SS_procedurep(obj) (SC_arrtype(obj, -1) == PROC_OBJ)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BOOLEANP - return TRUE if the object is of type BOOLEAN
 *             - return FALSE otherwise
 */

#define SS_booleanp(obj) (SC_arrtype(obj, -1) == SC_BOOL_I)

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* SS_HASHARRP - return TRUE if the object is of type HASH_ARRAY
 *             - return FALSE otherwise
 */

#define SS_hasharrp(obj) (SC_arrtype(obj, -1) == SS_HASHARR_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_HAELEMP - return TRUE if the object is of type HASH_ELEMENT
 *            - return FALSE otherwise
 */

#define SS_haelemp(obj) (SC_arrtype(obj, -1) == SS_HAELEM_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CHAROBJP - return TRUE if the object is of type CHARACTER
 *             - return FALSE otherwise
 */

#define SS_charobjp(obj) (SC_arrtype(obj, -1) == SS_CHARACTER_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_VECTORP - return TRUE if the object is of type VECTOR
 *            - return FALSE otherwise
 */

#define SS_vectorp(obj) (SC_arrtype(obj, -1) == SS_VECTOR_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PROCESSP - return TRUE if the object is of type PROCESS
 *             - return FALSE otherwise
 */

#define SS_processp(obj) (SC_arrtype(obj, -1) == SS_PROCESS_I)

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/*                        MEMORY MANAGEMENT MACROS                          */

/*--------------------------------------------------------------------------*/

/* SS_GC - garbage collection, incremental
 *       - each object has number of pointers to itself (except #t, #f, etc)
 *       - when the number of pointers is 1 then garbage collection means
 *       - freeing the space associated with the object
 *       - when the number is greater than 1 garbage collection means
 *       - decrementing the number of pointers to itself
 */

#define SS_GC SS_gc

#if 0
#ifdef SCHEME_DEBUG

#define SS_GC(obj)                                                           \
   {object *x;                                                               \
    x = obj;                                                                 \
    if ((x->val == NULL) || (x->eval_type == NO_EV))                         \
       SS_error("FREED OBJECT - SS_GC", SS_null);                            \
    if (x != NULL)                                                           \
       {if ((SS_OBJECT_GC(x) != 1) || (x->val == NULL))                      \
           {CFREE(x);}                                                       \
        else                                                                 \
           x->release(x);};}

#else

#define SS_GC(obj)                                                           \
   {object *x;                                                               \
    x = obj;                                                                 \
    if (x != NULL)                                                           \
       {if ((SS_OBJECT_GC(x) != 1) || (x->val == NULL))                      \
           {CFREE(x);}                                                       \
        else                                                                 \
           x->release(x);};}

#endif
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_MARK - increment the reference count of the object */

#define SS_MARK(x)  SC_mark(x, 1)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ASSIGN - do C level variable assignment with GC
 *           - SS_MARK before GC so that cdr'ing down lists works right
 *           - use temporary so that args are eval'd once only
 */

#ifdef SCHEME_DEBUG

#define SS_Assign(obj, nval)                                                 \
    {object *x;                                                              \
     x = nval;                                                               \
     if ((x->val == NULL) || (x->eval_type == NO_EV))                        \
        SS_error("FREED OBJECT - SS_ASSIGN", SS_null);                       \
     SS_MARK(x);                                                             \
     SS_GC(obj);                                                             \
     obj = x;}

#else

#define SS_Assign(obj, val)                                                  \
    {object *x;                                                              \
     x = val;                                                                \
     SS_MARK(x);                                                             \
     SS_GC(obj);                                                             \
     obj = x;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SET_CONT - set up a new continuation, execute the jumps
 *             - and release the continuation on return
 */

#define SS_set_cont(to_go, to_return)                                        \
    _SS_si.nsetc++;                                                              \
    _SS_si.cont_ptr++;                                                           \
    if (_SS_si.cont_ptr >= _SS_si.stack_size)                                    \
       SS_expand_stack();                                                    \
    if (SETJMP(_SS_si.continue_int[_SS_si.cont_ptr].cont))                       \
       {SS_end_trace();                                                      \
        _SS_si.cont_ptr--;                                                       \
        _SS_si.ngoc++;                                                           \
        SS_jump(to_return);}                                                 \
    else                                                                     \
       {SS_jump(to_go);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_GO_CONT - go (LONGJMP) to the current continuation */

#define SS_go_cont LONGJMP(_SS_si.continue_int[_SS_si.cont_ptr].cont, TRUE)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SAVE - save the object on the Scheme stack */

#ifdef STACK_FNC

extern void
 SS_Save(object *obj);

#else

# ifdef SCHEME_DEBUG

#  define SS_Save(obj)                                                       \
   {if ((obj->val == NULL) || (obj->eval_type == NO_EV))                     \
       SS_error("FREED OBJECT - SS_SAVE", SS_null);                          \
    _SS_si.nsave++;                                                              \
    SS_MARK(obj);                                                            \
    SC_array_push(_SS_si.stack, &obj);}

# else

#  define SS_Save(obj)                                                       \
   {_SS_si.nsave++;                                                              \
    SS_MARK(obj);                                                            \
    SC_array_push(_SS_si.stack, &obj);}

# endif
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RESTORE - pop an object off the stack and release the stacken */

#ifdef STACK_FNC

extern void
 _SS_Restore(object **px);

# define SS_Restore(x) _SS_Restore(&x)

#else

# ifdef SCHEME_DEBUG

#  define SS_Restore(x)                                                      \
   {_SS_si.nrestore++;                                                           \
    SS_GC(x);                                                                \
    x = *(object **) SC_array_pop(_SS_si.stack);                             \
    if ((x->val == NULL) || (x->eval_type == NO_EV))                         \
       SS_error("FREED OBJECT - SS_RESTORE", SS_null);}                       

# else

#  define SS_Restore(x)                                                      \
   {_SS_si.nrestore++;                                                           \
    SS_GC(x);                                                                \
    x = *(object **) SC_array_pop(_SS_si.stack);}

# endif
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_JUMP - can't bring myself to say goto */

#define SS_jump(x)  goto x

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_TRACEDP - returns the trace field in the procedure struct */

#define SS_tracedp(x) (SS_GET(procedure, x)->trace)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INT_VAL - return TRUE if the number has an integer value */

#define SS_int_val(x) (floor(x) == ceil(x))

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern object
 *SS_anon_proc,
 *SS_anon_macro,
 *SS_block_proc,
 *SS_scheme_symtab,
 *SS_quoteproc,
 *SS_quasiproc,
 *SS_unqproc,
 *SS_unqspproc,
 *SS_setproc,
 *SS_null,
 *SS_eof,
 *SS_t,
 *SS_f,
 *SS_else;

extern char
 *SS_OBJECT_S,
 *SS_POBJECT_S;

extern int
 SS_OBJECT_I,
 SS_PROCEDURE_I,
 SS_CONS_I,
 SS_VARIABLE_I,
 SS_INPUT_PORT_I,
 SS_OUTPUT_PORT_I,
 SS_EOF_I,
 SS_NULL_I,
 SS_VECTOR_I,
 SS_CHARACTER_I,
 SS_ERROR_I;

#ifdef LARGE

extern int
 SS_HAELEM_I,
 SS_HASHARR_I,
 SS_PROCESS_I;

#endif

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* SHENVR.C declarations */

extern object
 *SS_zargs(C_procedure *cp, object *argl),
 *SS_sargs(C_procedure *cp, object *argl),
 *SS_nargs(C_procedure *cp, object *argl),
 *SS_znargs(C_procedure *cp, object *argl),
 *SS_bound_name(char *name),
 *SS_mk_new_frame(object *name, hasharr *tab),
 *SS_defp(object *vr),
 *SS_lk_var_val(object *vr, object *penv),
 *SS_bind_env(object *vr, object *penv);

extern char
 **SS_bound_vars(char *patt, object *penv);

extern void
 SS_save_registers(int vp),
 SS_restore_registers(int vp),
 SS_set_var(object *vr, object *vl, object *penv),
 SS_def_var(object *vr, object *vl, object *penv),
 SS_env_vars(char **vrs, object *penv);


/* SHEVAL.C declarations */

extern object
 *SS_exp_eval(object *obj),
 *SS_eval(object *obj),
 *SS_params(object *fun),
 *SS_proc_body(object *fun),
 *SS_proc_env(object *fun),
 *SS_proc_name(object *fun),
 *SS_do_bindings(object *pp, object *argp);

extern int
 SS_true(object *obj);


/* SHINT.C declarations */

extern object
 *SS_define_constant(int n, ...),
 *SS_make_list(int first, ...),
 *SS_make_form(object *first, ...),
 *SS_eval_form(object *first, ...),
 *SS_call_scheme(char *func, ...);

extern int
 SS_args(object *s, ...),
 SS_run(char *s),
 SS_text_data_filep(char *fname, int cmnt),
 SS_load_scm(char *name),
 SS_define_argv(char *program, int c, char **v);

extern void
 SS_var_value(char *s, int type, void *vr, int flag);

extern void
 *SS_var_reference(char *s);

extern char
 *SS_exe_script(int c, char **v);


/* SHARR.C declarations */

extern object
 *SS_hash_dump(object *argl),
 *SS_mk_hasharr(hasharr *tb),
 *SS_mk_haelem(haelem *hp);


/* SHLARG.C declarations */

#ifdef LARGE

extern object
 *SS_vctlst(object *arg),
 *SS_lstvct(object *arg);

#endif


/* SHMM.C declarations */

extern object
 *SS_mk_proc_object(procedure *pp),
 *SS_mk_procedure(object *name, object *lam_exp, object *penv),
 *SS_mk_esc_proc(int err, int type),
 *SS_mk_variable(char *n, object *v),
 *SS_mk_string(char *s),
 *SS_mk_inport(FILE *str, char *name),
 *SS_mk_outport(FILE *str, char *name),
 *SS_mk_integer(int64_t i),
 *SS_mk_float(double d),
 *SS_mk_complex(double _Complex d),
 *SS_mk_quaternion(quaternion d),
 *SS_mk_boolean(char *s, int v),
 *SS_mk_cons(object *ca, object *cd),
 *SS_mk_object(void *np, int type, SS_eval_mode evt, char *pname,
	       void (*print)(object *obj, object *strm),
	       void (*release)(object *obj)),
 *SS_mk_char(int i),
 *SS_mk_vector(int l),
 *SS_pr_obj_map(void);

extern void
 SS_register_types(void),
 SS_install(char *pname, char *pdoc, PFPHand phand, ...),
 SS_rl_object(object *obj),
 SS_gc(object *obj);


/* SHPRM1.C declarations */

extern object
 *SS_list(object *argl);

extern void
 SS_bgn_trace(object *pfun, object *pargl),
 SS_end_trace(void);


/* SHPRM2.C declarations */

extern object
 *SS_unary_flt(C_procedure *cp, object *argl),
 *SS_unary_fix(C_procedure *cp, object *argl),
 *SS_binary_fix(C_procedure *cp, object *argl),
 *SS_binary_homogeneous(C_procedure *cp, object *argl),
 *SS_un_comp(C_procedure *cp, object *argl),
 *SS_bin_comp(C_procedure *cp, object *argl);


/* SHPRM3.C declarations */

extern int
 SS_length(object *obj),
 SS_numbp(object *obj);

extern object
 *SS_append(object *list1, object *list2),
 *SS_setcar(object *pair, object *car),
 *SS_setcdr(object *pair, object *cdr),
 *SS_list_tail(object *lst, int n),
 *SS_car(object *obj),
 *SS_cdr(object *obj),
 *SS_caar(object *obj),
 *SS_cadr(object *obj),
 *SS_cdar(object *obj),
 *SS_cddr(object *obj),
 *SS_caaar(object *obj),
 *SS_caadr(object *obj),
 *SS_cadar(object *obj),
 *SS_caddr(object *obj),
 *SS_cdaar(object *obj),
 *SS_cdadr(object *obj),
 *SS_cddar(object *obj),
 *SS_cdddr(object *obj),
 *SS_reverse(object *obj);


/* SHPRNT.C declarations */

extern void
 SS_set_prompt(char *fmt, ...),
 SS_print(object *obj, char *begin, char *end, object *strm),
 SS_wr_lst(object *obj, object *strm),
 SS_wr_proc(object *obj, object *strm),
 SS_wr_atm(object *obj, object *strm);

extern object
 *SS_trans_off(void),
 *SS_banner(object *obj);
 
extern int
 SS_puts(char *s, FILE *fp, PFfputs put),
 SS_prim_des(object *strm, object *obj),
 SS_prim_apr(FILE *str, char *s),
 SS_get_display_flag(void),
 SS_set_display_flag(int flg);


/* SHREAD.C declarations */

extern object
 *SS_add_variable(char *name),
 *SS_read(object *str),
 *SS_gread(object *obj),
 *SS_load(object *argl);

extern void
 SS_add_parser(char *ext, PFPObject fnc),
 SS_set_parser(PFPObject op);


/* SHSYNT.C declarations */

extern int
 SS_parse_error_synt(char *s, PFPObject fnc),
 SS_wrap_input_synt(void),
 SS_input_synt(char *ltxt);

extern void
 SS_unput_synt(int c),
 SS_name_map_synt(char *d, char *s);

extern object
 *SS_lookup_variable(char *txt, int verbose),
 *SS_mk_string_synt(char *s),
 *SS_add_type_synt(char *name);


/* SHTLEV.C declarations */

extern PFPrintErrMsg
 SS_set_print_err_func(PFPrintErrMsg fnc, int dflt),
 SS_get_print_err_func(void);

extern object
 *SS_lookup_object(object *obj),
 *SS_pop_err(int n, int flag);

extern int
 SS_set_scheme_env(char *exepath, char *path),
 SS_err_catch(PFInt func, PFInt errf);

extern char
 *SS_object_type_name(object *o, char *atype);

extern void
 SS_scheme_path_err(char *path),
 SS_inst_prm(void),
 SS_repl(void),
 SS_end_scheme(int val),
 SS_init_scheme(char *code, char *vers),
 SS_init_path(void),
 SS_inst_const(void),
 SS_init_stack(void),
 SS_init_cont(void),
 SS_expand_stack(void),
 SS_interrupt_handler(int sig),
 SS_push_err(int flag, int type),
 SS_error(char *s, object *obj);


/* SHTTY.C declarations */

extern int
 SS_printf(FILE *fp, char *fmt, ...),
 SS_fputs(char *s, FILE *fp),
 SS_get_ch(object *str, int ign_ws);

extern void
 SS_unget_ch(int c, object *str),
 SS_put_ch(int c, object *str);

extern char
 *SS_get_string(object *obj);


/* SHVAR.C declarations */

extern object
 *SS_install_cf(char *name, char *document, ...),
 *SS_install_cv(char *name, void *pval, int type),
 *SS_acc_double(C_procedure *cp, object *argl),
 *SS_acc_int(C_procedure *cp, object *argl),
 *SS_acc_long(C_procedure *cp, object *argl),
 *SS_acc_char(C_procedure *cp, object *argl),
 *SS_acc_string(C_procedure *cp, object *argl),
 *SS_acc_ptr(C_procedure *cp, object *argl);

#ifdef __cplusplus
}
#endif

#endif


