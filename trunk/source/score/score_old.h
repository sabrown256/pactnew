/*
 * SCORE_OLD.H - include file for deprecated SCORE functionality
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_SCORE_OLD

#define PCK_SCORE_OLD

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#ifndef SIGNAL_FUNCTION
# define SIGNAL_FUNCTION signal
#endif

#define SIGNAL(sig, fnc)  SIGNAL_FUNCTION(sig, fnc)

#undef MAKE
#undef MAKE_N
#undef REMAKE
#undef REMAKE_N
#undef SFREE
#undef SFREE_N

#define MAKE(_t)             CMAKE(_t)
#define MAKE_N(_t, n)        CMAKE_N(_t, n)
#define REMAKE(p, _t)        CREMAKE(p, _t, 1)
#define REMAKE_N(p, _t, n)   CREMAKE(p, _t, n)
#define SFREE(x)             CFREE(x)
#define SFREE_N(x, n)        CFREE(x)

#define FMAKE(_t, name)                                                      \
    ((_t *) (*SC_gs.mm.alloc)(1L, (long) sizeof(_t), name, FALSE, -1))

#define FMAKE_N(_t, n, name)                                                 \
    ((_t *) (*SC_gs.mm.alloc)((long) n, (long) sizeof(_t), name, FALSE, -1))

#define NMAKE(_t)                                                            \
    ((_t *) (*SC_gs.mm.alloc)(1L, (long) sizeof(_t),                         \
			      (char *) __func__, TRUE, -1))

#define NMAKE_N(_t, n)                                                       \
    ((_t *) (*SC_gs.mm.alloc)((long) n, (long) sizeof(_t),                   \
			      (char *) __func__, TRUE, -1))

#define NREMAKE(p, _t, n)                                                    \
   (p = (_t *) (*SC_gs.mm.realloc)((void *) p, (long) (n),                   \
				   (long) sizeof(_t), TRUE, -1))

#define SC_MAKE_ARRAY(_n, _t, _init)                                         \
    _SC_make_array(#_t, sizeof(_t), _init, _n, 0)

#define SC_INIT_ARRAY(_a, _n, _t, _init)                                     \
    _SC_init_array(_a, #_t, sizeof(_t), _init, _n, 0)


/* SC_dynamic_array macros - deprecated as of 01/2010 */

#define SC_RELEASE_DYNAMIC(_a)                                               \
    SC_da_rel(&(_a))

/* cleaned up, partially functionalized dynamic array macros */

#define SC_START_DYNAMIC_ARRAY(_a, _t, _d, _name)                            \
    SC_da_init(&(_a), sizeof(_t), #_t, _d, _name)

#define SC_ARRAY_DYNAMIC(_t, _a)          ((_t *) (_a).array)
#define SC_GET_NTH_DYNAMIC(_t, _a, n)     (((_t *) (_a).array)[n])
#define SC_N_DYNAMIC(_a)                  ((_a).n)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FF_ID - attempt a uniform naming of FORTRAN functions which 
 *       - gets around loader naming conventions
 *       - default definition - auto configured from scconfig.h
 */

#ifndef FF_ID

# ifdef ANSI_F77
#  define FF_ID(x, X)  X
# endif

# ifndef FF_ID
#  define FF_ID(x, X)  x ## _
# endif

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FCB_ID - attempt a uniform naming of FORTRAN common blocks which 
 *        - gets around compiler/loader naming conventions
 *        -
 *        -    FCB_ID(type, foo, FOO)(x, y, z)
 */

#ifndef FCB_ID
# ifdef INTEL_FC

#  define FCB_ID(x, X) ifl_ ## x ## _

# else    

#  define FCB_ID(x, X) FF_ID(x, X)

# endif
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* F77_ID - attempt a uniform naming of FORTRAN 77 functions which 
 *        - gets around loader naming conventions
 *        -
 *        - F77_ID(foo_, foo, FOO)(x, y, z)
 */

#ifndef F77_ID

# ifdef ANSI_F77
#  define F77_ID(x_, x, X)  X
# endif

# ifndef F77_ID
#  define F77_ID(x_, x, X)  x_
# endif

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* F77_STRING - historical, obviated by Fortran/C interoperability standard */

typedef char *F77_string;

# define SC_F77_C_STRING(_f) ((char *) _f)
# define SC_C_F77_STRING(_c) ((char *) _c)

#define SC_FORTRAN_STR_C(_cstr, _fstr, _nc)                                  \
   {strncpy(_cstr, (char *) (_fstr), _nc);                                   \
    _cstr[_nc] = '\0';}

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
         CREMAKE(_lst, _t, _nx);};}

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

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

/* former SCSIG.C declarations */

extern PFSignal_handler
 SC_signal(int sig, PFSignal_handler fnc);


/* former SCMEMC.C declarations */

extern int
 SC_zero_space(int flag),
 SC_zero_on_alloc(void);


/* former SCMEMDA.C declarations */

extern int
 SC_free(void *p),
 SC_free_z(void *p, int zsp);

extern void
 *SC_alloc(long nitems, long bpi, char *name),
 *SC_alloc_na(long nitems, long bpi, char *name, int na),
 *SC_alloc_nz(long nitems, long bpi, char *name, int na, int zsp),
 *SC_realloc(void *p, long nitems, long bpi),
 *SC_realloc_na(void *p, long nitems, long bpi, int na),
 *SC_realloc_nz(void *p, long nitems, long bpi, int na, int zsp);


/* former SCSTR.C declarations */

extern char
 *SC_strsave(char *s),
 *SC_strsavef(char *s, char *name),
 *SC_strsaven(char *s, char *name);


#ifdef __cplusplus
}
#endif

#endif
