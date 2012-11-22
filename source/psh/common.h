/*
 * COMMON.H - header for simply portable C programs
 *          - which do the same kind of work as shell scripts
 *          - PSH is designed to be used in two very different ways:
 *          -   1) source files directly included:
 *          -      #include <libpsh.c>
 *          -   2) source files built into SCORE library
 *          - the first way allows cc -g foo.c -o foo type builds
 *          - without extra libraries or flags
 *          - the second way allows the routines in PSH to be
 *          - used in the rest of PACT - avoiding duplication
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#ifndef PCK_COMMON

# define PCK_COMMON

# include "posix.h"
# include "ansic.h"
# include "nonstd.h"

/*--------------------------------------------------------------------------*/

# ifndef SCOPE_SCORE_COMPILE

/*--------------------------------------------------------------------------*/

#undef MAKE
#undef MAKE_N
#undef REMAKE
#undef FREE
#undef STRSAVE

#define MAKE(_t)            ((_t *) malloc(sizeof(_t)))
#define MAKE_N(_t, _n)      ((_t *) calloc(_n, sizeof(_t)))
#define REMAKE(_p, _t, _n)  (_p = (_t *) realloc((void *) _p, (_n)*sizeof(_t)))
#define FREE(_p)            {if (_p != NULL) free(_p); _p = NULL;}
#define STRSAVE(_s)         nstrsave(_s)

#undef ASSERT
#define ASSERT(_t)        {if (_t) _assert_fail++;}

#define LST_FREE(_x)                                                         \
    {lst_free(_x);                                                           \
     _x = NULL;}

#define LAST_CHAR(_s)     ((_s)[last_char(_s)])

#define VA_START(x)                                                          \
   {va_list __a__;                                                           \
    va_start(__a__, x)

#define VA_ARG(x)      va_arg(__a__, x)

/* #define VSNPRINTF(_s, _n, _f) vsprintf(_s, _f, __a__) */
#define VSNPRINTF(_s, _n, _f) vsnprintf(_s, _n, _f, __a__)

#define VA_END                                                               \
    va_end(__a__);}

#define IS_NULL(_s)  (((_s) == NULL) || ((_s)[0] == '\0'))

extern int
 _assert_fail;

/*--------------------------------------------------------------------------*/

# endif

/*--------------------------------------------------------------------------*/

/* anonymous enumerated constants for PSH
 * rather than define'd constants which can collide
 * with applications */

#undef TEXT

enum {

      FRONT = 1,
      BACK  = 2,
      BOTH  = 3,

/* COMMON.H */
      P_APPEND = 100,
      P_PREPEND,


/* LIBPSH.C */
      TEXT,
      HTML,
      CMD_LINE,
      CMD_OUT,
      STACK_FILE,
      STACK_PROCESS,
      STACK_TOOL,
      STACK_GROUP,
      PHASE_READ,
      PHASE_ANALYZE,
      PHASE_WRITE,


/* LIBTIME.C */
      TIME_SEC,
      TIME_HMS};

enum e_array_size_constants
   {BFSML   = 256,
    BFMDM   = 512,
    BFLRG   = 4096,
    BFMG    = 1048576,
    N_STACK = 32};

typedef enum e_array_size_constants array_size_constants;

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* keep these out of PACT proper - only in PSH */

#ifndef SCOPE_SCORE

#define FOREACH(_i, _v, _delim)                                              \
   {char *_i, *_t, *_pt;                                                     \
    _t = STRSAVE(_v);                                                        \
    for (_pt = _t; TRUE; _pt = NULL)                                         \
        {_i = strtok(_pt, _delim);                                           \
	 if (_i == NULL)                                                     \
	     break;                                                          \
	 else                                                                \
            {

#define NEXT(_i, _delim)                                                     \
    {_pt = NULL;                                                             \
     _i  = strtok(_pt, _delim);}

#define ENDFOR                                                               \
      };};                                                                   \
   FREE(_t);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
