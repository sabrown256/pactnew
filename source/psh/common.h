/*
 * COMMON.H - header for simply portable C programs
 *          - which do the same kind of work as shell scripts
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

#define BFSML        256
#define BFMDM        512
#define BFLRG        4096
#define BFMG         1048576
#define N_STACK      32

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

/* COMMON.H */
       P_APPEND = 100,
       P_PREPEND,

/* LIBPSH.C */

       TEXT,
       HTML,

#ifdef NEWWAY
       FRONT,
       BACK,
       BOTH,
#endif

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
       TIME_HMS,

     };

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
