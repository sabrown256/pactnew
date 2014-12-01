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
         lst = CMAKE_N(_t, nx);};                                            \
     lst[n++] = item;                                                        \
     if (n >= nx)                                                            \
        {nx += delta;                                                        \
         CREMAKE(lst, _t, nx);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define IS_BARRIER(_s)                                                       \
   ((_SC.barrier != NULL) &&                                                 \
    (strncmp(_s, _SC.barrier, strlen(_SC.barrier)) == 0))

#define CINIT_ARRAY(_a, _t, _init, _flags)                                   \
    _SC_init_array(_a, #_t, sizeof(_t), _init, (char *) __func__, _flags)

#define CMAKE_ARRAY(_t, _init, _flags)                                       \
    _SC_make_array(#_t, sizeof(_t), _init, (char *) __func__, _flags)

#define SC_STRING_ARRAY()    _SC_string_array((char *) __func__)

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_SC_array SC_array;

/* smart array structure
 *
 * #bind struct SC_array
 */

struct s_SC_array
  {char *name;
   char *type;                                   /* type of array elements */
   int bpi;                                              /* bytes per item */
   int memfl;                                              /* memory flags */
   long n;                                       /* max referenced element */
   long nx;                                /* number of elements allocated */
   double gf;                                             /* growth factor */
   void *array  MBR(type, type);                        /* the actual data */
   void (*init)(void *a);          /* initialize new elements when growing */
   void *(*access)(int oper, void *ra, int bpi, long n, void *v);};

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

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
 *_SC_make_array(char *type, int bpi,
		 void (*init)(void *a), char *name, int flags),
 *_SC_string_array(char *name),
 *SC_array_copy(SC_array *a),
 *SC_strings_array(int n, char **sa);

extern void
 _SC_init_array(SC_array *a, char *type, int bpi,
		void (*init)(void *a), char *name, int flags),
 SC_array_init(SC_array *a, long n),
 SC_free_array(SC_array *a, int (*rel)(void *a)),
 SC_array_string_add(SC_array *a, char *s),
 SC_array_string_add_copy(SC_array *a, char *s),
 SC_array_string_add_vcopy(SC_array *a, char *fmt, ...);

extern void
 *SC_array_done(SC_array *a),
 *SC_array_set(SC_array *a, long n, void *v),
 *SC_array_get(SC_array *a, long n),
 *SC_array_array(SC_array *a),
 *SC_array_push(SC_array *a, void *v),
 *SC_array_pop(SC_array *a);

extern char
 **_SC_array_string_join(SC_array **psa);

extern long
 SC_array_resize(SC_array *a, long n, double g),
 SC_array_frac_resize(SC_array *a, double f),
 SC_array_set_n(SC_array *a, long n),
 SC_array_inc_n(SC_array *a, long n, int wh),
 SC_array_dec_n(SC_array *a, long n, int wh),
 SC_array_get_n(SC_array *a),
 SC_array_remove(SC_array *a, long n);

extern int
 _SC_array_is_member(SC_array *a, char *s),
 SC_array_free_n(void *a),
 SC_array_string_append(SC_array *out, SC_array *in),
 SC_array_sort(SC_array *a, PFIntBin pred);


#ifdef __cplusplus
}
#endif

#endif

