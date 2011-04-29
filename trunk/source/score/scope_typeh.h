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

#define REAL double
#define HUGE_REAL 1.0e100

#ifndef HAVE_FIXED_WIDTH_FLOAT
typedef float       float32_t;
typedef double      float64_t;
typedef long double float128_t;
#endif

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

#define N_PRIMITIVE_CHAR  2
#define N_PRIMITIVE_FIX   5
#define N_PRIMITIVE_FP    3
#define N_PRIMITIVE_CPX   3
#define N_PRIMITIVE_QUT   1
#define N_PRIMITIVES     18   /* up through SC_POINTER_I */
#define N_TYPES          20   /* up through SC_STRING_I */

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define SC_TYPEOF(_t)        SC_type_id(#_t, FALSE)

#define SC_TYPE_CHAR(_i)     ((_i) - SC_CHAR_I)
#define SC_TYPE_FIX(_i)      ((_i) - SC_INT8_I)
#define SC_TYPE_FP(_i)       ((_i) - SC_FLOAT_I)
#define SC_TYPE_CPX(_i)      ((_i) - SC_FLOAT_COMPLEX_I)

#define SC_TYPE_CHAR_ID(_i)  ((_i) + SC_CHAR_I)
#define SC_TYPE_FIX_ID(_i)   ((_i) + SC_INT8_I)
#define SC_TYPE_FP_ID(_i)    ((_i) + SC_FLOAT_I)
#define SC_TYPE_CPX_ID(_i)   ((_i) + SC_FLOAT_COMPLEX_I)

/* SC_VA_ARG_FETCH - convert a variable arg item to an item of type index _DID
 *                 - NOTE: the variable arg item is read into a char array
 *                 - which is merely a buffer to temporarily hold 
 *                 - the contents of the char array which are fetched
 *                 - by the SC_VA_ARG_ID macro 
 */

#define SC_VA_ARG_FETCH(_did, _d, _sid)                                      \
    {char _v[MAX_PRSZ];                                                      \
     SC_VA_ARG_ID(_sid, _v, 0);                                              \
     SC_convert_id(_did, _d, 0, 1, _sid, _v, 0, 1, 1, FALSE);}

/* SC_VA_ARG_STORE - store a value _S of type _ID to a the space pointed
 *                 - to by a pointer grabbed from SC_VA_ARG_ID
 */

#define SC_VA_ARG_STORE(_id, _s)                                             \
    {void *_v;                                                               \
     _v = SC_VA_ARG(void *);                                                 \
     SC_convert_id(_id, _v, 0, 1, _id, _s, 0, 1, 1, FALSE);}

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

enum e_SC_type_method
   {SC_TYPE_NONE = 0,
    SC_TYPE_INIT,
    SC_TYPE_FREE};

typedef enum e_SC_type_method SC_type_method;

enum e_SC_kind
   {KIND_CHAR = 0,
    KIND_BOOL,
    KIND_INT,
    KIND_FLOAT,
    KIND_COMPLEX,
    KIND_QUATERNION,
    KIND_POINTER,
    KIND_STRUCT,
    KIND_OTHER};

typedef enum e_SC_kind SC_kind;

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
    SC_kind kind;              /* type kind */
    int bpi;                   /* bytes per item */
    void (*init)(void *x);     /* function to initialize instance */
    void (*free)(void *x);};   /* function to free instance */

struct s_precisionfp
   {int digits;
    long double tolerance;};

struct s_SC_type_manager
   {int nprimitive;
    int chr[3];                  /* fixed point type info    [n, first, last] */
    int fix[3];                  /* fixed point type info    [n, first, last] */
    int fp[3];                   /* floating point type info [n, first, last] */
    int cpx[3];                  /* complex type info        [n, first, last] */
    int qut[3];                  /* quaternion type info     [n, first, last] */

    int max_digits;
    int fix_precision[N_PRIMITIVE_FIX];
    precisionfp fp_precision[N_PRIMITIVE_FP];

    char *chrtyp[N_PRIMITIVE_CHAR];
    char *fixtyp[N_PRIMITIVE_FIX];
    char *fptyp[N_PRIMITIVE_FP];
    char *cpxtyp[N_PRIMITIVE_CPX];
    char *quttyp[N_PRIMITIVE_QUT];

    char *formats[N_TYPES];
    char *formata[N_TYPES];

    char *user_formats[N_TYPES];
    char *user_formata[N_TYPES];

    char *suppress_member;

    void *typ;};

/* initialization of type manager */

#define _SC_CHAR_1        3
#define _SC_CHAR_N        (_SC_CHAR_1 + N_PRIMITIVE_CHAR)
#define _SC_FIX_1         (_SC_CHAR_N + 1)
#define _SC_FIX_N         (_SC_FIX_1 + N_PRIMITIVE_FIX)
#define _SC_FP_1          (_SC_FIX_N + 1)
#define _SC_FP_N          (_SC_FP_1 + N_PRIMITIVE_FP)
#define _SC_CPX_1         (_SC_FP_N + 1)
#define _SC_CPX_N         (_SC_CPX_1 + N_PRIMITIVE_CPX)
#define _SC_QUT_1         (_SC_CPX_N + 1)
#define _SC_QUT_N         (_SC_QUT_1 + N_PRIMITIVE_QUT)

#define TYPE_STATE_INIT                                                       \
   { N_PRIMITIVES,                                                            \
     {N_PRIMITIVE_CHAR, _SC_CHAR_1, _SC_CHAR_N},                              \
     {N_PRIMITIVE_FIX,  _SC_FIX_1,  _SC_FIX_N},                               \
     {N_PRIMITIVE_FP,   _SC_FP_1,   _SC_FP_N},                                \
     {N_PRIMITIVE_CPX,  _SC_CPX_1,  _SC_CPX_N},                               \
     {N_PRIMITIVE_QUT,  _SC_QUT_1,  _SC_QUT_N},                               \
     1000,                                                                    \
     {0, 0, 0, 0},                                                            \
     {{0, 0.0}, {0, 0.0}, {0, 0.0}},                                          \
     { "char", "wchar" },                                                     \
     { "int8_t", "short", "integer", "long", "long_long" },                   \
     { "float", "double", "long_double" },                                    \
     { "float_complex", "double_complex", "long_double_complex" },            \
     { "quaternion" },                                                        \
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
 SC_WCHAR_I,
 SC_SHORT_I,
 SC_INT_I,
 SC_LONG_I,
 SC_LONG_LONG_I,
 SC_INT8_I,
 SC_INT16_I, 
 SC_INT32_I,
 SC_INT64_I,
 SC_FLOAT_I,
 SC_DOUBLE_I,
 SC_LONG_DOUBLE_I,
 SC_FLOAT32_I,
 SC_FLOAT64_I,
 SC_FLOAT128_I,
 SC_FLOAT_COMPLEX_I,
 SC_DOUBLE_COMPLEX_I,
 SC_LONG_DOUBLE_COMPLEX_I,
 SC_COMPLEX32_I,
 SC_COMPLEX64_I,
 SC_COMPLEX128_I,
 SC_QUATERNION_I,
 SC_STRING_I,
 SC_POINTER_I,
 SC_BOOL_P_I,
 SC_SHORT_P_I,
 SC_INT_P_I,
 SC_LONG_P_I,
 SC_LONG_LONG_P_I,
 SC_INT8_P_I,
 SC_INT16_P_I, 
 SC_INT32_P_I,
 SC_INT64_P_I,
 SC_FLOAT_P_I,
 SC_DOUBLE_P_I,
 SC_LONG_DOUBLE_P_I,
 SC_FLOAT32_P_I,
 SC_FLOAT64_P_I,
 SC_FLOAT128_P_I,
 SC_FLOAT_COMPLEX_P_I,
 SC_DOUBLE_COMPLEX_P_I,
 SC_LONG_DOUBLE_COMPLEX_P_I,
 SC_COMPLEX32_P_I,
 SC_COMPLEX64_P_I,
 SC_COMPLEX128_P_I,
 SC_QUATERNION_P_I,
 SC_VOID_I,
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
 *SC_WCHAR_S,
 *SC_SHORT_S,
 *SC_INT_S,
 *SC_LONG_S,
 *SC_LONG_LONG_S,
 *SC_INT8_S,
 *SC_INT16_S,
 *SC_INT32_S,
 *SC_INT64_S,
 *SC_FLOAT_S,
 *SC_DOUBLE_S,
 *SC_LONG_DOUBLE_S,
 *SC_FLOAT32_S,
 *SC_FLOAT64_S,
 *SC_FLOAT128_S,
 *SC_FLOAT_COMPLEX_S,
 *SC_DOUBLE_COMPLEX_S,
 *SC_LONG_DOUBLE_COMPLEX_S,
 *SC_COMPLEX32_S,
 *SC_COMPLEX64_S,
 *SC_COMPLEX128_S,
 *SC_QUATERNION_S,
 *SC_STRING_S,
 *SC_POINTER_S,
 *SC_BOOL_P_S,
 *SC_SHORT_P_S,
 *SC_INT_P_S,
 *SC_LONG_P_S,
 *SC_LONG_LONG_P_S,
 *SC_INT8_P_S,
 *SC_INT16_P_S,
 *SC_INT32_P_S,
 *SC_INT64_P_S,
 *SC_FLOAT_P_S,
 *SC_DOUBLE_P_S,
 *SC_LONG_DOUBLE_P_S,
 *SC_FLOAT32_P_S,
 *SC_FLOAT64_P_S,
 *SC_FLOAT128_P_S,
 *SC_FLOAT_COMPLEX_P_S,
 *SC_DOUBLE_COMPLEX_P_S,
 *SC_LONG_DOUBLE_COMPLEX_P_S,
 *SC_COMPLEX32_P_S,
 *SC_COMPLEX64_P_S,
 *SC_COMPLEX128_P_S,
 *SC_QUATERNION_P_S,
 *SC_VOID_S,
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
 SC_fix_lmt(int nb, int64_t *pmn, int64_t *pmx, int64_t *pumx),
 SC_unpack_bits(char *out, char *in, int ityp, int nbits,
		int padsz, int fpp, long nitems, long offs),
 SC_sizeof(char *s);

extern long
 SC_extract_field(char *in, int offs, int nbi, int nby, int *ord);


/* SCTYP.C declarations */

extern int
 SC_type_register(char *name, SC_kind kind, int bpi, ...),
 SC_type_alias(char *name, int id),
 SC_type_id(char *name, int unsg),
 SC_type_ptr_id(int id),
 SC_fix_type_id(char *name, int unsg),
 SC_fp_type_id(char *name),
 SC_cx_type_id(char *name),
 SC_is_type_char(int id),
 SC_is_type_fix(int id),
 SC_is_type_fp(int id),
 SC_is_type_cx(int id),
 SC_is_type_qut(int id),
 SC_is_type_num(int id),
 SC_is_type_ptr(int id),
 SC_is_type_prim(int id),
 SC_is_type_struct(int id),
 SC_type_size_i(int id),
 SC_type_size_a(char *name),
 SC_type_container_size(SC_kind kind, int nb),
 SC_type_match_size(SC_kind kind, int nb),
 SC_deref_id(char *name, int base);

extern size_t
 SC_copy_primitive(void *d, void *s, long n, int id);

extern char
 *SC_type_name(int id),
 *SC_dereference(char *s),
 *SC_ntos(char *t, int nc, int id, void *s, long n, int mode);

extern void
 *SC_convert_id(int did, void *d, long od, long sd,
		int sid, void *s, long os, long ss,
		long n, int flag),
 *SC_convert(char *dtype, void *d, long od, long sd,
	     char *stype, void *s, long os, long ss,
	     long n, int flag);

extern void
 SC_fin_type_manager(void),
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
