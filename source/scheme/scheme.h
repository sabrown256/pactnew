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

enum e_SS_io_logging
   {STDIN_ONLY,
    ALL,
    NO_LOG};

typedef enum e_SS_io_logging SS_io_logging;


/* Generic object definition */

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
typedef struct s_SS_psides SS_psides;
typedef struct s_object object;

struct s_object
   {char *print_name;            /* specific members */
    void *val;

    SS_eval_mode eval_type;      /* generic members */
    void (*print)(SS_psides *si, object *obj, object *strm);
    void (*release)(SS_psides *si, object *obj);};

FUNCTION_POINTER(object, (*PFObject));
FUNCTION_POINTER(object, *(*PFPObject));

typedef int (*PFPrChIn)(object *str, int ign_ws);
typedef void (*PFPrintErrMsg)(SS_psides *si, FILE *str, char *s, object *obj);
typedef object *(*PFPHand)(SS_psides *si, C_procedure *cp, object *argl);

#define SS_DEFINE_OBJECT(_si)                                               \
   {defstr *dp;                                                             \
    dp = PD_defstr(file, "object",                                          \
                   "char *print_name",                                      \
                   "char *val",                                             \
                   "int eval_type",                                         \
                   "function print",                                        \
                   "function release",                                      \
                   LAST);                                                   \
    if (dp == NULL)                                                         \
       SS_error(_si, "COULDN'T DEFINE OBJECT TO FILE - SS_DEFINE_OBJECT", \
                SS_null);}

struct s_SS_psides
   {int interactive;
    int trace_env;
    int know_env;
    int trap_error;
    int lines_page;
    int print_flag;
    int stat_flag;
    int bracket_flag;
    int nsave;
    int nrestore;
    int nsetc;
    int ngoc;
    int stack_size;
    int stack_mask;
    int cont_ptr;
    int err_cont_ptr;
    int errlev;
    int eox;
    int strict_c;

    char prompt[MAXLINE];
    char ans_prompt[MAXLINE];
    char *lex_text;

    continuation *continue_int;
    err_continuation *continue_err;

    SC_array *stack;

    hasharr *symtab;
    hasharr *types;

    SS_io_logging hist_flag;

    object *indev;
    object *outdev;
    object *histdev;
    object *this;
    object *val;
    object *unev;
    object *exn;
    object *argl;
    object *fun;
    object *env;
    object *global_env;
    object *err_state;
    object **err_stack;
    object *rdobj;
    object *evobj;
    object *character_stream;

    int (*pr_gets)(SS_psides *si, object *fp);
    int (*post_print)(SS_psides *si);
    void (*post_read)(SS_psides *si, object *fp);
    void (*post_eval)(SS_psides *si, object *fp);
    void (*name_reproc)(SS_psides *si, char *s, char *name);
    void (*get_arg)(SS_psides *si, object *obj, void *v, int type);
    object *(*read)(SS_psides *si, object *fp);
    object *(*call_arg)(SS_psides *si, int type, void *v);

    void (*pr_ch_out)(int c, object *fp);
    void (*pr_ch_un)(int c, object *fp);

    JMP_BUF cpu;};


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

#define SS_GET(_t, _o)          ((_t *) SS_OBJECT(_o))
#define SS_PP(_f, _memb)        ((procedure *) SS_OBJECT(_f))->_memb

/* OBJECT ACCESSORS */

#define SS_OBJECT_GC(_o)           SC_ref_count(_o)
#define SS_UNCOLLECT(_o)           SC_permanent(_o)
#define SS_OBJECT_TYPE(_o)         SC_arrtype(_o, -1)
#define SS_OBJECT_NAME(_o)         ((_o)->print_name)
#define SS_OBJECT(_o)              ((_o)->val)
#define SS_OBJECT_ETYPE(_o)        ((_o)->eval_type)
#define SS_OBJECT_FREE(_si, _o)       (*((_o)->release))(_si, _o)
#define SS_OBJECT_PRINT(_si, _o, _s)  (*((_o)->print))(_si, _o, _s)

#define SS_INQUIRE_OBJECT(_si, _o)                                           \
   ((object *) SC_hasharr_def_lookup((_si)->symtab, _o))

/* NUMBER ACCESSORS */

#define SS_INTEGER_VALUE(_o)       *((int64_t *) ((_o)->val))
#define SS_FLOAT_VALUE(_o)         *((double *) ((_o)->val))
#define SS_COMPLEX_VALUE(_o)       *((double _Complex *) ((_o)->val))
#define SS_QUATERNION_VALUE(_o)    *((quaternion *) ((_o)->val))

/* INPUT_PORT/OUTPUT_PORT ACCESSORS */

#define SS_OUTSTREAM(_o)                                                     \
    ((SS_outportp(_o)) ? (((output_port *) (_o)->val)->str) : stdout)
#define SS_OFILE_NAME(_o)                                                    \
    ((SS_outportp(_o)) ? (((output_port *) (_o)->val)->name) : NULL)

#define SS_INSTREAM(_o)          (((input_port *) (_o)->val)->str)
#define SS_BUFFER(_o)            (((input_port *) (_o)->val)->buffer)
#define SS_PTR(_o)               (((input_port *) (_o)->val)->ptr)
#define SS_LINE_NUMBER(_o)       (((input_port *) (_o)->val)->iln)
#define SS_CHAR_INDEX(_o)        (((input_port *) (_o)->val)->ichr)
#define SS_IFILE_NAME(_o)        (((input_port *) (_o)->val)->name)

/* CONS ACCESSORS */

#define SS_CAR_MACRO(_o)         (((cons *) (_o)->val)->car)
#define SS_CDR_MACRO(_o)         (((cons *) (_o)->val)->cdr)

/* VARIABLE ACCESSORS */

#define SS_VARIABLE_NAME(_o)     (((variable *) ((_o)->val))->name)
#define SS_VARIABLE_VALUE(_o)    (((variable *) ((_o)->val))->value)

/* BOOLEAN ACCESSORS */

#define SS_BOOLEAN_NAME(_o)      (((SS_boolean *) ((_o)->val))->name)
#define SS_BOOLEAN_VALUE(_o)     (((SS_boolean *) ((_o)->val))->value)

/* STRING ACCESSORS */

#define SS_STRING_TEXT(_o)       (((string *) ((_o)->val))->string)
#define SS_STRING_LENGTH(_o)     (((string *) ((_o)->val))->length)

/* PROCEDURE ACCESSORS */

#define SS_PROCEDURE_TYPE(_o)    (((procedure *) ((_o)->val))->type)
#define SS_PROCEDURE_NAME(_o)    (((procedure *) ((_o)->val))->name)
#define SS_PROCEDURE_DOC(_o)     (((procedure *) ((_o)->val))->doc)
#define SS_PROCEDURE_TRACEDP(_o) (((procedure *) ((_o)->val))->trace)
#define SS_PROCEDURE_PROC(_o)    (((procedure *) ((_o)->val))->proc)

/* C PROCEDURE ACCESSORS */

#define SS_GET_C_PROCEDURE(_o)   ((C_procedure *) SS_PROCEDURE_PROC(_o))

#define SS_C_PROCEDURE_HANDLER_PTR(_o)                                       \
   (((C_procedure *) SS_PROCEDURE_PROC(_o))->handler)
#define SS_C_PROCEDURE_HANDLER(_o)                                           \
   (*SS_C_PROCEDURE_HANDLER_PTR(_o))
#define SS_C_PROCEDURE_FUNCTION_PTR(_o)                                      \
   (((C_procedure *) SS_PROCEDURE_PROC(_o))->proc[0])
#define SS_C_PROCEDURE_FUNCTION(_o)                                          \
   (*SS_C_PROCEDURE_FUNCTION_PTR(_o))

/* COMPOUND PROCEDURE ACCESSORS */

#define SS_COMPOUND_PROCEDURE(_o)                                            \
   ((S_procedure *) SS_PROCEDURE_PROC(_o))

/* ESCAPE PROCEDURE ACCESSORS */

#define SS_ESCAPE_CONTINUATION(_o)                                           \
   (((Esc_procedure *) SS_PROCEDURE_PROC(_o))->cont)
#define SS_ESCAPE_STACK(_o)                                                  \
   (((Esc_procedure *) SS_PROCEDURE_PROC(_o))->stck)
#define SS_ESCAPE_ERROR(_o)                                                  \
   (((Esc_procedure *) SS_PROCEDURE_PROC(_o))->err)
#define SS_ESCAPE_TYPE(_o)                                                   \
   (((Esc_procedure *) SS_PROCEDURE_PROC(_o))->type)

#ifdef LARGE

/* VECTOR ACCESSORS */

#define SS_VECTOR_LENGTH(_o)  (((vector *) (_o)->val)->length)
#define SS_VECTOR_ARRAY(_o)   (((vector *) (_o)->val)->vect)

/* CHARACTER ACCESSOR */

#define SS_CHARACTER_VALUE(_o) *((int *) (_o)->val)

/* PROCESS ACCESSORS */

#define SS_PROCESS_VALUE(_o)     ((PROCESS *) (_o)->val)
#define SS_PROCESS_STATUS(_o)   (((PROCESS *) (_o)->val)->status)

#endif

/*--------------------------------------------------------------------------*/

/*                           MACRO PREDICATES                               */

/*--------------------------------------------------------------------------*/

/* SS_INTEGERP - return TRUE if the object is of type INTEGER 
 *             - return FALSE otherwise
 */

#define SS_integerp(_o) (SC_arrtype(_o, -1) == SC_INT_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_FLOATP - return TRUE if the object is of type FLOAT 
 *           - return FALSE otherwise
 */

#define SS_floatp(_o) (SC_arrtype(_o, -1) == SC_FLOAT_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_COMPLEXP - return TRUE if the object is of type COMPLEX
 *             - return FALSE otherwise
 */

#define SS_complexp(_o) (SC_arrtype(_o, -1) == SC_DOUBLE_COMPLEX_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_QUATERNIONP - return TRUE if the object is of type QUATERNION
 *                - return FALSE otherwise
 */

#define SS_quaterionp(_o) (SC_arrtype(_o, -1) == SC_QUATERNION_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_STRINGP - return TRUE if the object is of type STRING 
 *            - return FALSE otherwise
 */

#define SS_stringp(_o) (SC_arrtype(_o, -1) == SC_STRING_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CONSP - return TRUE if the object is of type CONS
 *          - return FALSE otherwise
 */

#define SS_consp(_o) (SC_arrtype(_o, -1) == SS_CONS_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_VARIABLEP - return TRUE if the object is of type VARIABLE
 *              - return FALSE otherwise
 */

#define SS_variablep(_o) (SC_arrtype(_o, -1) == SS_VARIABLE_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INPORTP - return TRUE if the object is of type INPUT_PORT
 *            - return FALSE otherwise
 */

#define SS_inportp(_o) (SC_arrtype(_o, -1) == SS_INPUT_PORT_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_OUTPORTP - return TRUE if the object is of type OUTPUT_PORT
 *             - return FALSE otherwise
 */

#define SS_outportp(_o) (SC_arrtype(_o, -1) == SS_OUTPUT_PORT_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_EOFOBJP - return TRUE if the object is of type EOF
 *            - return FALSE otherwise
 */

#define SS_eofobjp(_o) (SC_arrtype(_o, -1) == SS_EOF_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_NULLOBJP - return TRUE if the object is of type NULL
 *             - return FALSE otherwise
 */

#define SS_nullobjp(_o) (SC_arrtype(_o, -1) == SS_NULL_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PROCEDUREP - return TRUE if the object is of type PROC_OBJ
 *               - return FALSE otherwise
 */

#define SS_procedurep(_o) (SC_arrtype(_o, -1) == PROC_OBJ)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BOOLEANP - return TRUE if the object is of type BOOLEAN
 *             - return FALSE otherwise
 */

#define SS_booleanp(_o) (SC_arrtype(_o, -1) == SC_BOOL_I)

/*--------------------------------------------------------------------------*/

#ifdef LARGE

/*--------------------------------------------------------------------------*/

/* SS_HASHARRP - return TRUE if the object is of type HASH_ARRAY
 *             - return FALSE otherwise
 */

#define SS_hasharrp(_o) (SC_arrtype(_o, -1) == SS_HASHARR_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_HAELEMP - return TRUE if the object is of type HASH_ELEMENT
 *            - return FALSE otherwise
 */

#define SS_haelemp(_o) (SC_arrtype(_o, -1) == SS_HAELEM_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_CHAROBJP - return TRUE if the object is of type CHARACTER
 *             - return FALSE otherwise
 */

#define SS_charobjp(_o) (SC_arrtype(_o, -1) == SS_CHARACTER_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_VECTORP - return TRUE if the object is of type VECTOR
 *            - return FALSE otherwise
 */

#define SS_vectorp(_o) (SC_arrtype(_o, -1) == SS_VECTOR_I)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_PROCESSP - return TRUE if the object is of type PROCESS
 *             - return FALSE otherwise
 */

#define SS_processp(_o) (SC_arrtype(_o, -1) == SS_PROCESS_I)

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/*                        MEMORY MANAGEMENT MACROS                          */

/*--------------------------------------------------------------------------*/

/* SS_MARK - increment the reference count of the object */

#define SS_mark(_o)  SC_mark(_o, 1)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_ASSIGN - do C level variable assignment with GC
 *           - SS_mark before GC so that cdr'ing down lists works right
 *           - use temporary so that args are eval'd once only
 */

#ifdef SCHEME_DEBUG

#define SS_assign(_si, _o, _v)                                               \
    {object *_x;                                                             \
     _x = _v;                                                                \
     if ((_x->val == NULL) || (_x->eval_type == NO_EV))                      \
        SS_error(_si, "FREED OBJECT - SS_ASSIGN", SS_null);                  \
     SS_mark(_x);                                                            \
     SS_gc(_si, _o);                                                         \
     _o = _x;}

#else

#define SS_assign(_si, _o, _v)                                               \
    {object *_x;                                                             \
     _x = _v;                                                                \
     SS_mark(_x);                                                            \
     SS_gc(_si, _o);                                                         \
     _o = _x;}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_SAVE - save the object on the Scheme stack */

#ifdef STACK_FNC

extern void
 SS_save(SS_psides *si, object *obj);

#else

# ifdef SCHEME_DEBUG

#  define SS_save(_si, _o)                                                   \
   {if ((_o->val == NULL) || (_o->eval_type == NO_EV))                       \
       SS_error(_si, "FREED _OECT - SS_SAVE", SS_null);                      \
    (_si)->nsave++;                                                          \
    SS_mark(_o);                                                             \
    SC_array_push((_si)->stack, &_o);}

# else

#  define SS_save(_si, _o)                                                   \
   {(_si)->nsave++;                                                          \
    SS_mark(_o);                                                             \
    SC_array_push((_si)->stack, &_o);}

# endif
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_RESTORE - pop an object off the stack and release the stacken */

#ifdef STACK_FNC

extern void
 _SS_restore(SS_psides *si, object **px);

# define SS_restore(_si, _o) _SS_restore(_si, &_o)

#else

# ifdef SCHEME_DEBUG

#  define SS_restore(_si, _o)                                                \
   {(_si)->nrestore++;                                                       \
    SS_gc(_si, _o);                                                          \
    _o = *(object **) SC_array_pop((_si)->stack);                            \
    if ((_o->val == NULL) || (_o->eval_type == NO_EV))                       \
       SS_error(_si, "FREED OBJECT - SS_RESTORE", SS_null);}

# else

#  define SS_restore(_si, _o)                                                \
   {(_si)->nrestore++;                                                       \
    SS_gc(_si, _o);                                                          \
    _o = *(object **) SC_array_pop((_si)->stack);}

# endif
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_JUMP - can't bring myself to say goto */

#define SS_jump(_x)  goto _x

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_TRACEDP - returns the trace field in the procedure struct */

#define SS_tracedp(_o) (SS_GET(procedure, _o)->trace)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INT_VAL - return TRUE if the number has an integer value */

#define SS_int_val(_x) (floor(_x) == ceil(_x))

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INIT - setup certain universal things for applications, i.e.
 *         - interrupt handler
 *         - top level longjump
 *         - output buffering
 *         - this must be a macro to circumvent implementations that
 *         - won't let you LONGJMP back into function from which a
 *         - return has been done
 *         - Prototype arguments would be:
 *         -   char *_msg;
 *         -   void (*_ef)(int err);
 *         -   int _sh;
 *         -   void (*_sf)(int sig);
 *         -   int _bh;
 *         -   char *_bf;
 *         -   int _bsz;
 */

#define SS_init(_si, _msg, _ef, _sh, _sf, _bh, _bf, _bsz)                    \
   {void (*_lsf)(int sig);                                                   \
    static void (*_lef)(SS_psides *si, int err) = NULL;                      \
    switch (SETJMP(SC_gs.cpu))                                               \
       {case ABORT :                                                         \
             io_printf(STDOUT, "\n%s\n\n", _msg);                            \
             if (_lef != NULL)                                               \
                (*_lef)(_si, ABORT);                                         \
             exit(1);                                                        \
        case ERR_FREE :                                                      \
             if (_lef != NULL)                                               \
                (*_lef)(_si, ERR_FREE);                                      \
             exit(0);                                                        \
        default :                                                            \
             _lef = _ef;                                                     \
             break;};                                                        \
    if (_sh == TRUE)                                                         \
       {_lsf = _sf;                                                          \
        if (_lsf != NULL)                                                    \
           SC_signal_n(SIGINT, _lsf, _si);                                   \
        else                                                                 \
           SC_signal_n(SIGINT, SC_interrupt_handler, _si);};                 \
    if (_bh == TRUE)                                                         \
       {if ((_bf == NULL) || (_bsz <= 0))                                    \
           {SC_setbuf(stdout, NULL);}                                        \
        else                                                                 \
           {io_setvbuf(stdout, _bf, _IOFBF, _bsz);};};}

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
 *SS_exp_eval(SS_psides *si, object *obj),
 *SS_zargs(SS_psides *si, C_procedure *cp, object *argl),
 *SS_sargs(SS_psides *si, C_procedure *cp, object *argl),
 *SS_nargs(SS_psides *si, C_procedure *cp, object *argl),
 *SS_znargs(SS_psides *si, C_procedure *cp, object *argl),
 *SS_bound_name(SS_psides *si, char *name),
 *SS_mk_new_frame(SS_psides *si, object *name, hasharr *tab),
 *SS_lk_var_val(SS_psides *si, object *vr),
 *SS_bind_env(SS_psides *si, object *vr, object *penv);

extern char
 **SS_bound_vars(SS_psides *si, char *patt, object *penv);

extern void
 SS_save_registers(SS_psides *si, int vp),
 SS_restore_registers(SS_psides *si, int vp),
 SS_set_var(SS_psides *si, object *vr, object *vl, object *penv),
 SS_def_var(SS_psides *si, object *vr, object *vl, object *penv),
 SS_env_vars(SS_psides *si, char **vrs, object *penv);


/* SHEVAL.C declarations */

extern object
 *SS_eval(SS_psides *si, object *obj),
 *SS_params(SS_psides *si, object *fun),
 *SS_proc_body(SS_psides *si, object *fun),
 *SS_proc_env(SS_psides *si, object *fun),
 *SS_proc_name(SS_psides *si, object *fun),
 *SS_do_bindings(SS_psides *si, object *pp, object *argp);

extern int
 SS_true(object *obj);


/* SHINT.C declarations */

extern SS_psides
 *SS_get_current_scheme(int tid);

extern object
 *SS_define_constant(SS_psides *si, int n, ...),
 *SS_make_list(SS_psides *si, int first, ...),
 *SS_make_form(SS_psides *si, object *first, ...),
 *SS_eval_form(SS_psides *si, object *first, ...),
 *SS_call_scheme(SS_psides *si, char *func, ...);

extern int
 SS_args(SS_psides *si, object *s, ...),
 SS_run(SS_psides *si, char *s),
 SS_text_data_filep(char *fname, int cmnt),
 SS_load_scm(SS_psides *si, char *name),
 SS_define_argv(SS_psides *si, char *program, int c, char **v);

extern void
 SS_var_value(SS_psides *si, char *s, int type, void *vr, int flag);

extern void
 *SS_var_reference(SS_psides *si, char *s);

extern char
 *SS_exe_script(int c, char **v);


/* SHARR.C declarations */

extern object
 *SS_hash_dump(SS_psides *si, object *argl),
 *SS_mk_hasharr(SS_psides *si, hasharr *tb),
 *SS_mk_haelem(SS_psides *si, haelem *hp);


/* SHLARG.C declarations */

#ifdef LARGE

extern object
 *SS_vctlst(SS_psides *si, object *arg),
 *SS_lstvct(SS_psides *si, object *arg);

#endif


/* SHMM.C declarations */

extern object
 *SS_mk_proc_object(SS_psides *si, procedure *pp),
 *SS_mk_procedure(SS_psides *si, object *name,
		  object *lam_exp, object *penv),
 *SS_mk_esc_proc(SS_psides *si, int err, int type),
 *SS_mk_variable(SS_psides *si, char *n, object *v),
 *SS_mk_string(SS_psides *si, char *s),
 *SS_mk_inport(SS_psides *si, FILE *str, char *name),
 *SS_mk_outport(SS_psides *si, FILE *str, char *name),
 *SS_mk_integer(SS_psides *si, int64_t i),
 *SS_mk_float(SS_psides *si, double d),
 *SS_mk_complex(SS_psides *si, double _Complex d),
 *SS_mk_quaternion(SS_psides *si, quaternion d),
 *SS_mk_boolean(SS_psides *si, char *s, int v),
 *SS_mk_cons(SS_psides *si, object *ca, object *cd),
 *SS_mk_object(SS_psides *si,
	       void *np, int type, SS_eval_mode evt, char *pname,
	       void (*print)(SS_psides *si, object *obj, object *strm),
	       void (*release)(SS_psides *si, object *obj)),
 *SS_mk_char(SS_psides *si, int i),
 *SS_mk_vector(SS_psides *si, int l);

extern void
 SS_register_types(void),
 SS_install(SS_psides *si, char *pname, char *pdoc, PFPHand phand, ...),
 SS_rl_object(SS_psides *si, object *obj),
 SS_gc(SS_psides *si, object *obj);


/* SHPRM1.C declarations */

extern object
 *SS_list(SS_psides *si, object *argl);


/* SHPRM2.C declarations */

extern object
 *SS_unary_flt(SS_psides *si, C_procedure *cp, object *argl),
 *SS_unary_fix(SS_psides *si, C_procedure *cp, object *argl),
 *SS_binary_fix(SS_psides *si, C_procedure *cp, object *argl),
 *SS_binary_homogeneous(SS_psides *si, C_procedure *cp, object *argl),
 *SS_un_comp(SS_psides *si, C_procedure *cp, object *argl),
 *SS_bin_comp(SS_psides *si, C_procedure *cp, object *argl);


/* SHPRM3.C declarations */

extern int
 SS_length(SS_psides *si, object *obj),
 SS_numbp(object *obj);

extern object
 *SS_append(SS_psides *si, object *list1, object *list2),
 *SS_setcar(SS_psides *si, object *pair, object *car),
 *SS_setcdr(SS_psides *si, object *pair, object *cdr),
 *SS_list_tail(SS_psides *si, object *lst, int n),
 *SS_car(SS_psides *si, object *obj),
 *SS_cdr(SS_psides *si, object *obj),
 *SS_caar(SS_psides *si, object *obj),
 *SS_cadr(SS_psides *si, object *obj),
 *SS_cdar(SS_psides *si, object *obj),
 *SS_cddr(SS_psides *si, object *obj),
 *SS_caaar(SS_psides *si, object *obj),
 *SS_caadr(SS_psides *si, object *obj),
 *SS_cadar(SS_psides *si, object *obj),
 *SS_caddr(SS_psides *si, object *obj),
 *SS_cdaar(SS_psides *si, object *obj),
 *SS_cdadr(SS_psides *si, object *obj),
 *SS_cddar(SS_psides *si, object *obj),
 *SS_cdddr(SS_psides *si, object *obj),
 *SS_reverse(SS_psides *si, object *obj);


/* SHPRNT.C declarations */

extern void
 SS_set_prompt(SS_psides *si, char *fmt, ...),
 SS_print(SS_psides *si, object *strm, object *obj, char *begin, char *end),
 SS_wr_lst(SS_psides *si, object *obj, object *strm),
 SS_wr_proc(SS_psides *si, object *obj, object *strm),
 SS_wr_atm(SS_psides *si, object *obj, object *strm);

extern object
 *SS_trans_off(SS_psides *si),
 *SS_banner(SS_psides *si, object *obj);
 
extern int
 SS_puts(const char *s, FILE *fp, PFfputs put),
 SS_prim_des(SS_psides *si, object *strm, object *obj),
 SS_prim_apr(SS_psides *si, FILE *str, char *s),
 SS_get_display_flag(void),
 SS_set_display_flag(int flg);


/* SHREAD.C declarations */

extern object
 *SS_add_variable(SS_psides *si, char *name),
 *SS_gread(object *obj),
 *SS_read(SS_psides *si, object *str),
 *SS_load(SS_psides *si, object *argl);

extern void
 SS_add_parser(char *ext, object *(*prs)(SS_psides *si)),
 SS_set_parser(object *(*prs)(SS_psides *si));


/* SHSYNT.C declarations */

extern int
 SS_parse_error_synt(SS_psides *si, char *s, PFPObject fnc),
 SS_wrap_input_synt(void),
 SS_input_synt(SS_psides *si, char *ltxt);

extern void
 SS_unput_synt(SS_psides *si, int c),
 SS_name_map_synt(SS_psides *si, char *d, char *s);

extern object
 *SS_lookup_variable(SS_psides *si, char *txt, int verbose),
 *SS_mk_string_synt(SS_psides *si, char *s),
 *SS_add_type_synt(SS_psides *si, char *name);


/* SHTLEV.C declarations */

extern SS_psides
 *SS_init_scheme(char *code, char *vers);

extern PFPrintErrMsg
 SS_set_print_err_func(PFPrintErrMsg fnc, int dflt),
 SS_get_print_err_func(void);

extern object
 *SS_lookup_object(SS_psides *si, object *obj),
 *SS_pop_err(SS_psides *si, int n, int flag);

extern int
 SS_set_scheme_env(char *exepath, char *path),
 SS_err_catch(SS_psides *si, int (*fint)(SS_psides *si), PFInt errf);

extern char
 *SS_object_type_name(object *o, char *atype);

extern void
 SS_set_put_string(SS_psides *si, PFfputs ps),
 SS_set_put_line(SS_psides *si, int (*pf)(FILE *fp, char *fmt, ...)),
 SS_interrupt_handler(int sig),
 SS_scheme_path_err(char *path),
 SS_init_path(void),
 SS_repl(SS_psides *si),
 SS_end_scheme(SS_psides *si, int val),
 SS_expand_stack(SS_psides *si),
 SS_push_err(SS_psides *si, int flag, int type),
 SS_error(SS_psides *si, char *s, object *obj);


/* SHTTY.C declarations */

extern int
 SS_printf(FILE *fp, char *fmt, ...),
 SS_fputs(const char *s, FILE *fp),
 SS_get_ch(SS_psides *si, object *str, int ign_ws);

extern void
 SS_unget_ch(int c, object *str),
 SS_put_ch(int c, object *str);

extern char
 *SS_get_string(object *obj);


/* SHVAR.C declarations */

extern object
 *SS_install_cf(SS_psides *si, char *name, char *document, ...),
 *SS_install_cv(SS_psides *si, char *name, void *pval, int type),
 *SS_acc_double(SS_psides *si, C_procedure *cp, object *argl),
 *SS_acc_int(SS_psides *si, C_procedure *cp, object *argl),
 *SS_acc_long(SS_psides *si, C_procedure *cp, object *argl),
 *SS_acc_char(SS_psides *si, C_procedure *cp, object *argl),
 *SS_acc_string(SS_psides *si, C_procedure *cp, object *argl),
 *SS_acc_ptr(SS_psides *si, C_procedure *cp, object *argl);

#ifdef __cplusplus
}
#endif

#endif


