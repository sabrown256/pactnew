/*
 * SCOPE_TYPE.H - support type handling in SCORE
 *
 */

#ifndef PCK_SCOPE_TYPEH

#define PCK_SCOPE_TYPEH

#include "cpyright.h"

#if defined(BEOS)
typedef long long int64_t;
#endif

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define SC_TYPEOF(_t)        SC_type_id(#_t, FALSE)

#define SC_TYPE_CHAR(_i)     (((_i) - SC_CHAR_I) >> 1)
#define SC_TYPE_FIX(_i)      (((_i) - SC_INT8_I) >> 1)
#define SC_TYPE_FP(_i)       (((_i) - SC_FLOAT_I) >> 1)
#define SC_TYPE_CPX(_i)      (((_i) - SC_FLOAT_COMPLEX_I) >> 1)
#define SC_TYPE_QUT(_i)      (((_i) - SC_QUATERNION_I) >> 1)

#define SC_TYPE_CHAR_ID(_i)  (((_i) << 1) + SC_CHAR_I)
#define SC_TYPE_FIX_ID(_i)   (((_i) << 1) + SC_INT8_I)
#define SC_TYPE_FP_ID(_i)    (((_i) << 1) + SC_FLOAT_I)
#define SC_TYPE_CPX_ID(_i)   (((_i) << 1) + SC_FLOAT_COMPLEX_I)
#define SC_TYPE_QUT_ID(_i)   (((_i) << 1) + SC_QUATERNION_I)


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

/* these "link" to the libtyp originals */
typedef type_group SC_kind;
typedef type_desc SC_type;

typedef struct s_precisionfp precisionfp;
typedef struct s_SC_type_manager SC_type_manager;

struct s_precisionfp
   {int digits;
    long double tolerance;};

struct s_SC_type_manager
   {int nprimitive;
    int nstandard;

    int max_digits;
    int fix_precision[N_PRIMITIVE_FIX];
    precisionfp fp_precision[N_PRIMITIVE_FP];

    char *formats[N_TYPES];
    char *formata[N_TYPES];

    char *user_formats[N_TYPES];
    char *user_formata[N_TYPES];

    char *suppress_member;

    void *typ;};


/* initialization of type manager */

#define _SC_TYPE_STATE_INIT_                                                 \
   { N_PRIMITIVES, N_TYPES,                                                  \
     1000,                                                                   \
     {0, 0, 0, 0},                                                           \
     {{0, 0.0}, {0, 0.0}, {0, 0.0}},                                         \
   }

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/* SCFNCA.C declarations */

extern int
 SC_fix_lmt(int nb, int64_t *pmn, int64_t *pmx, int64_t *pumx),
 SC_unpack_bits(char *out, char *in, int ityp, int nbits,
		int padsz, int fpp, long ni, long offs),
 SC_sizeof(char *s);

extern long
 SC_extract_field(char *in, int offs, int nbi, int nby, int *ord);


/* SCTYP.C declarations */

extern SC_type
 *_SC_get_type_name(char *name),
 *_SC_get_type_id(int id),
 *SC_find_primitive(int id);

extern int
 SC_type_register(char *name, SC_kind kind, bool ptr, int bpi, ...),
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
 SC_is_type_ptr_a(char *name),
 SC_is_type_prim(int id),
 SC_is_type_struct(int id),
 SC_type_size_i(int id),
 SC_type_size_a(char *name),
 SC_type_container_size(SC_kind kind, int nb),
 SC_type_match_size(SC_kind kind, int nb),
 SC_deref_id(char *name, int base);

extern size_t
 SC_copy_primitive(void *d, void *s, long n, int id);

extern void
 *SC_type_info(int id);

extern char
 *SC_type_name(int id),
 *SC_type_ptr_name(int id),
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

