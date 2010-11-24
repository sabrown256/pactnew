/*
 * SCOPE_TYPEH.H - support type handling in SCORE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_TYPEH

#define PCK_SCOPE_TYPEH

#include "cpyright.h"

#ifndef NO_VA_ARG_ID
# include <scarg.h>
#endif

# define REAL double
# define HUGE_REAL 1.0e100
FUNCTION_POINTER(double, *(*PFPREAL));

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

#define N_PRIMITIVE_FIX   4
#define N_PRIMITIVE_FP    3
#define N_PRIMITIVE_CPX   3
#define N_PRIMITIVE_QUT   1
#define N_PRIMITIVES     16

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define SC_TYPEOF(_t)     SC_type_id(#_t, FALSE)

/* SC_VA_ARG_FETCH - convert a variable arg item to an item of type index _DID
 *                 - NOTE: the variable arg item is read into a char array
 *                 - which is merely a buffer to temporarily hold 
 *                 - the contents of the char array which are fetched
 *                 - by the SC_VA_ARG_ID macro 
 */

#define SC_VA_ARG_FETCH(_did, _d, _sid)                                      \
    {char _v[MAXLINE];                                                       \
     SC_VA_ARG_ID(_sid, _v, 0);                                              \
     SC_convert_id(_did, _d, 0, _sid, _v, 0, 1, 1, FALSE);}

/* SC_VA_ARG_STORE - store a value _S of type _ID to a the space pointed
 *                 - to by a pointer grabbed from SC_VA_ARG_ID
 */

#define SC_VA_ARG_STORE(_id, _s)                                             \
    {void *_v;                                                               \
     _v = SC_VA_ARG(void *);                                                 \
     SC_convert_id(_id, _v, 0, _id, _s, 0, 1, 1, FALSE);}

/* SC_VA_ARG_NTOS - convert a variable arg item to a string
 *                - NOTE: the variable arg item is read into a char array
 *                - which is merely a buffer to temporarily hold 
 *                - the contents of the char array which are fetched
 *                - by the SC_VA_ARG_ID macro 
 */

#define SC_VA_ARG_NTOS(_s, _nc, _id)                                         \
    {char _v[MAXLINE];                                                       \
     SC_VA_ARG_ID(_id, _v, 0);                                               \
     SC_ntos(_s, _nc, _id, _v, 0, 1);}


/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_SC_type SC_type;
typedef struct s_precisionfp precisionfp;
typedef struct s_SC_type_manager SC_type_manager;

/* SC_type
 *    - make a struct to encapsulate basic type information
 *    - to facilitate working with types as first class objects
 *    - also a place for further type related development
 *    - this is complementary to the PDB type system in that
 *    - in PDB the emphasis is in data conversion in file I/O
 *    - while here it is about working with data types in
 *    - programming
 */

struct s_SC_type
   {int id;                    /* integer type id */
    char *type;                /* type name */
    int bpi;                   /* bytes per item */
    void (*free)(void *x);};   /* function to free instance */

struct s_precisionfp
   {int digits;
    long double tolerance;};

struct s_SC_type_manager
   {int nprimitive;
    int fix[3];                  /* fixed point type info    [n, first, last] */
    int fp[3];                   /* floating point type info [n, first, last] */
    int cpx[3];                  /* complex type info        [n, first, last] */
    int qut[3];                  /* quaternion type info     [n, first, last] */

    int max_digits;
    int fix_precision[N_PRIMITIVE_FIX];
    precisionfp fp_precision[N_PRIMITIVE_FP];

    char *fixtyp[N_PRIMITIVE_FIX];
    char *fptyp[N_PRIMITIVE_FP];
    char *cpxtyp[N_PRIMITIVE_FP];
    char *quttyp[N_PRIMITIVE_FP];

    char *formats[N_PRIMITIVES];
    char *formata[N_PRIMITIVES];

    char *user_formats[N_PRIMITIVES];
    char *user_formata[N_PRIMITIVES];

    char *suppress_member;

    void *typ;};

/* initialization of type manager */

#define TYPE_STATE_INIT                                                       \
   { N_PRIMITIVES,                                                            \
     {N_PRIMITIVE_FIX,  4,  4+N_PRIMITIVE_FIX-1},                             \
     {N_PRIMITIVE_FP,   8,  8+N_PRIMITIVE_FP-1},                              \
     {N_PRIMITIVE_CPX, 11, 13+N_PRIMITIVE_CPX-1},                             \
     {N_PRIMITIVE_QUT, 14, 14+N_PRIMITIVE_QUT-1},                             \
     1000,                                                                    \
     {0, 0, 0, 0},                                                            \
     {{0, 0.0}, {0, 0.0}, {0, 0.0}},                                          \
     { "short", "integer", "long", "long_long" },                             \
     { "float", "double", "long_double" },                                    \
     { "float_complex", "double_complex", "long_double_complex" },            \
     { "float_quaternion", "double_quaternion", "long_double_quaternion" },   \
   }

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern int
 SC_UNKNOWN_I,
 SC_BIT_I,
 SC_BOOL_I,
 SC_CHAR_I,
 SC_SHORT_I,
 SC_INT_I,
 SC_LONG_I,
 SC_LONG_LONG_I,
 SC_FLOAT_I,
 SC_DOUBLE_I,
 SC_LONG_DOUBLE_I,
 SC_FLOAT_COMPLEX_I,
 SC_DOUBLE_COMPLEX_I,
 SC_LONG_DOUBLE_COMPLEX_I,
 SC_QUATERNION_I,
 SC_STRING_I,
 SC_POINTER_I,
 SC_BOOL_P_I,
 SC_SHORT_P_I,
 SC_INT_P_I,
 SC_LONG_P_I,
 SC_LONG_LONG_P_I,
 SC_FLOAT_P_I,
 SC_DOUBLE_P_I,
 SC_LONG_DOUBLE_P_I,
 SC_FLOAT_COMPLEX_P_I,
 SC_DOUBLE_COMPLEX_P_I,
 SC_LONG_DOUBLE_COMPLEX_P_I,
 SC_QUATERNION_P_I,
 SC_VOID_I,
 SC_CHAR_8_I,
 SC_STRUCT_I,
 SC_PCONS_I,
 SC_PROCESS_I,
 SC_FILE_I,
 SC_PCONS_P_I,
 SC_ENUM_I,
 SC_INTEGER_I,
 SC_REAL_I,
 SC_REAL_P_I;

extern char
 *SC_UNKNOWN_S,
 *SC_BIT_S,
 *SC_BOOL_S,
 *SC_CHAR_S,
 *SC_SHORT_S,
 *SC_INT_S,
 *SC_LONG_S,
 *SC_LONG_LONG_S,
 *SC_FLOAT_S,
 *SC_DOUBLE_S,
 *SC_LONG_DOUBLE_S,
 *SC_FLOAT_COMPLEX_S,
 *SC_DOUBLE_COMPLEX_S,
 *SC_LONG_DOUBLE_COMPLEX_S,
 *SC_QUATERNION_S,
 *SC_STRING_S,
 *SC_POINTER_S,
 *SC_BOOL_P_S,
 *SC_SHORT_P_S,
 *SC_INT_P_S,
 *SC_LONG_P_S,
 *SC_LONG_LONG_P_S,
 *SC_FLOAT_P_S,
 *SC_DOUBLE_P_S,
 *SC_LONG_DOUBLE_P_S,
 *SC_FLOAT_COMPLEX_P_S,
 *SC_DOUBLE_COMPLEX_P_S,
 *SC_LONG_DOUBLE_COMPLEX_P_S,
 *SC_QUATERNION_P_S,
 *SC_VOID_S,
 *SC_CHAR_8_S,
 *SC_STRUCT_S,
 *SC_PCONS_S,
 *SC_PROCESS_S,
 *SC_FILE_S,
 *SC_PCONS_P_S,
 *SC_ENUM_S,
 *SC_INTEGER_S,
 *SC_REAL_S,
 *SC_REAL_P_S;

extern int
 (*SC_sizeof_hook)(char *s);                     /* string driven size hook */

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/* SCFNCA.C declarations */

extern char
 *SC_dereference(char *s);

extern int
 SC_fix_lmt(int nb, BIGINT *pmn, BIGINT *pmx, BIGINT *pumx),
 SC_unpack_bits(char *out, char *in, int ityp, int nbits,
		int padsz, int fpp, long nitems, long offs),
 SC_sizeof(char *s);

extern long
 SC_extract_field(char *in, int offs, int nbi, int nby, int *ord);


/* SCTYP.C declarations */

extern int
 SC_register_type(char *name, int bpi, ...),
 SC_type_alias(char *name, int id),
 SC_type_id(char *name, int unsg),
 SC_fix_type_id(char *name, int unsg),
 SC_fp_type_id(char *name),
 SC_cx_type_id(char *name),
 SC_is_type_fix(int id),
 SC_is_type_fp(int id),
 SC_is_type_cx(int id),
 SC_is_type_num(int id),
 SC_is_type_ptr(int id),
 SC_type_size_i(int id),
 SC_type_size_a(char *name),
 SC_deref_id(char *name, int base);

extern size_t
 SC_copy_primitive(void *d, void *s, long n, int id);

extern char
 *SC_type_name(int id),
 *SC_dereference(char *s),
 *SC_ntos(char *t, int nc, int id, void *s, long n, int mode);

extern void
 *SC_convert_id(int did, void *d, long od,
		int sid, void *s, long os, long ss,
		long n, int flag),
 *SC_convert(char *dtype, void *d, long od,
	     char *stype, void *s, long os, long ss,
	     long n, int flag);

extern void
 SC_type_free_i(int id, void *x),
 SC_type_free_a(char *name, void *x),
 SC_init_base_types(void),
 _SC_set_user_defaults(void),
 _SC_set_user_formats(void),
 _SC_set_format_defaults(void);

#ifdef __cplusplus
}
#endif

#endif

