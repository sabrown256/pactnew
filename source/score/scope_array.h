/*
 * SCOPE_ARRAY.H - support dynamic arrays
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCOPE_ARRAY

#define PCK_SCOPE_ARRAY

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

/* SC_REMEMBER - add an item to an array of items */

#define SC_REMEMBER(_t, item, lst, n, nx, delta)                             \
    {if (lst == NULL)                                                        \
        {nx = delta;                                                         \
         n  = 0;                                                             \
         lst = FMAKE_N(_t, nx, "SC_REMEMBER:lst");};                         \
     lst[n++] = item;                                                        \
     if (n >= nx)                                                            \
        {nx += delta;                                                        \
         REMAKE_N(lst, _t, nx);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SC_dynamic_array macros - deprecated as of 01/2010 */

#define SC_RELEASE_DYNAMIC(_a)                                               \
    SC_da_rel(&(_a))

/* cleaned up, partially functionalized dynamic array macros */

#define SC_START_DYNAMIC_ARRAY(_a, _t, _d, _name)                            \
    SC_da_init(&(_a), sizeof(_t), #_t, _d, _name)

#define SC_ARRAY_DYNAMIC(_t, _a)          ((_t *) (_a).array)
#define SC_GET_NTH_DYNAMIC(_t, _a, n)     (((_t *) (_a).array)[n])
#define SC_N_DYNAMIC(_a)                  ((_a).n)

/* SC_array macros */

#define IS_BARRIER(_s)                                                       \
   ((BARRIER != NULL) &&                                                     \
    (strncmp(_s, BARRIER, strlen(BARRIER)) == 0))

#define SC_INIT_ARRAY(_a, _n, _t, _init)                                     \
    _SC_init_array(_a, _n, #_t, sizeof(_t), _init)

#define SC_MAKE_ARRAY(_n, _t, _init)                                         \
    SC_make_array(_n, #_t, sizeof(_t), _init)

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* SC_dynamic_array macros - not used */

#define SC_REMEMBERF(_t, _item, _lst, _n, _nx, _delta, _name)                \
    {if (_lst == NULL)                                                       \
        {_nx = _delta;                                                       \
         _n  = 0;                                                            \
         _lst = FMAKE_N(_t, _nx, _name);};                                   \
     _lst[_n++] = _item;                                                     \
     if (_n >= _nx)                                                          \
        {_nx += _delta;                                                      \
         REMAKE_N(_lst, _t, _nx);};}

#define SC_INIT_DYNAMIC_ARRAY_F(_a, _t, _tn, _d, _name)                      \
    SC_da_init(&(_a), sizeof(_t), _tn, _d, _name)

#define SC_REMEMBER_DYNAMIC_F(_t, item, _a, name)                            \
    SC_da_remember(&(_a), &(item))

#define SC_REMEMBER_STRING(_s, _a)                                           \
    SC_remember_string(_s, &(_a))

#define SC_REMEMBER_STRING_COPY(_s, _a)                                      \
    SC_remember_string_copy(_s, &(_a))

#define SC_SET_NTH_DYNAMIC(_t, _a, n, v)  ((_t *) (_a).array)[(n)] = (v)

#define SC_SIZE_DYNAMIC(_t, _a, m)        SC_da_grow(&(_a), m)

#define SC_SHRINK_DYNAMIC(_t, _a, _n)     SC_da_shrink(&(_a), _n)

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_SC_dynamic_array SC_dynamic_array;

struct s_SC_dynamic_array
  {char *type;
   void *array;
   int n;
   int nx;
   int bpi;
   int delta;};

#define PD_DEFINE_DYNAMIC_ARRAY(_f)                                \
    {PD_defstr(_f, "SC_dynamic_array",                             \
                    "char *type",                                  \
                    "char *array",                                 \
                    "int n",                                       \
                    "int nx",                                      \
                    "int bpi",                                     \
                    "int delta",                                   \
                    LAST);                                         \
     PD_cast(_f, "SC_dynamic_array", "array", "type");}

typedef struct s_SC_array SC_array;

struct s_SC_array
  {char *name;
   char *type;                                   /* type of array elements */
   int bpi;                                              /* bytes per item */
   long n;                                       /* max referenced element */
   long nx;                                /* number of elements allocated */
   int nref;
   double gf;                                             /* growth factor */
   void *array;                                         /* the actual data */
   void (*init)(void *a);          /* initialize new elements when growing */
   void *(*set)(void *ra, int bpi, int oper, long n, void *v);};

#define PD_DEFINE_SMART_ARRAY(_f)                                  \
    {PD_defstr(_f, "SC_array",                                     \
                    "char *name",                                  \
                    "char *type",                                  \
                    "int bpi",                                     \
                    "long n",                                      \
                    "long nx",                                     \
                    "int nref",                                    \
                    "double gf",                                   \
                    "char *array",                                 \
                    "function init",                               \
                    "function set",                                \
                    LAST);                                         \
     PD_cast(_f, "SC_array", "array", "type");}

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern char
 *BARRIER;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/* SCARRD.C declarations */

extern void
 SC_da_clear(SC_dynamic_array *a, int d, int bpi),
 SC_da_init(SC_dynamic_array *a, int bpi, char *tn, int d, char *name),
 *SC_da_done(SC_dynamic_array *a),
 SC_da_free(SC_dynamic_array *a),
 SC_da_rel(SC_dynamic_array *a),
 SC_da_grow(SC_dynamic_array *a, int m),
 SC_da_shrink(SC_dynamic_array *a, int n),
 SC_da_reset(SC_dynamic_array *daa, int elem),
 SC_da_clean(SC_dynamic_array *daa, int elem),
 SC_da_release(SC_dynamic_array *daa),
 SC_da_release_pointees(SC_dynamic_array *daa, int elem),
 SC_da_release_all(SC_dynamic_array *daa),
 SC_dada_reset(SC_dynamic_array *dada, int elem),
 SC_dada_clean(SC_dynamic_array *dada, int elem),
 SC_dada_release(SC_dynamic_array *dada);

extern SC_dynamic_array
 *SC_da_copy(SC_dynamic_array *a);

extern int
 _SC_is_member(SC_dynamic_array *a, char *s),
 SC_da_remember(SC_dynamic_array *a, void *item),
 SC_append_strings(SC_dynamic_array *out, SC_dynamic_array *in);

extern char
 **_SC_join_lines(SC_dynamic_array *strs);

extern void
 _SC_da_alloc(SC_dynamic_array *a, char *name),
 _SC_da_extend(SC_dynamic_array *a, double pad),
 SC_remember_string(char *s, SC_dynamic_array *a),
 SC_remember_string_copy(char *s, SC_dynamic_array *a);


/* SCARRS.C declarations */

extern SC_array
 *SC_make_array(char *name, char *type, int bpi, void (*init)(void *a)),
 *SC_string_array(char *name),
 *SC_array_copy(SC_array *a),
 *SC_strings_array(int n, char **sa);

extern void
 _SC_init_array(SC_array *a, char *name, char *type, int bpi,
		void (*init)(void *a)),
 SC_array_init(SC_array *a, long n),
 SC_free_array(SC_array *a, int (*rel)(void *a)),
 SC_array_string_add(SC_array *a, char *s),
 SC_array_string_add_copy(SC_array *a, char *s),
 SC_array_unarray(SC_array *a, int flag);

extern void
 *SC_array_done(SC_array *a),
 *SC_array_set(SC_array *a, long n, void *v),
 *SC_array_get(SC_array *a, long n),
 *SC_array_array(SC_array *a, int flag),
 *SC_array_push(SC_array *a, void *v),
 *SC_array_pop(SC_array *a);

extern char
 **_SC_array_string_join(SC_array **psa);

extern long
 SC_array_resize(SC_array *a, long n, double g),
 SC_array_set_n(SC_array *a, long n),
 SC_array_inc_n(SC_array *a, long n, int wh),
 SC_array_dec_n(SC_array *a, long n, int wh),
 SC_array_get_n(SC_array *a);

extern int
 _SC_array_is_member(SC_array *a, char *s),
 SC_array_free_n(void *a),
 SC_array_string_append(SC_array *out, SC_array *in),
 SC_array_sort(SC_array *a, PFIntBin pred);


#ifdef __cplusplus
}
#endif

#endif

