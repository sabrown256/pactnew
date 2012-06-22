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
#define PCK_COMMON

#include "posix.h"
#include "ansic.h"
#include "nonstd.h"

#undef MAXLINE

#define MAXLINE    4096
#define LRG        4096
#define MEGA       1048576
#define N_STACK    32
#define APPEND     100
#define PREPEND    101

#undef MAKE
#undef MAKE_N
#undef REMAKE
#undef FREE
#undef STRSAVE

#define MAKE(_t)          ((_t *) malloc(sizeof(_t)))
#define MAKE_N(_t, _n)    ((_t *) calloc(_n, sizeof(_t)))
#define REMAKE(p, _t, n)  (p = (_t *) realloc((void *) p, (n)*sizeof(_t)))
#define FREE(_p)          {if (_p != NULL) free(_p); _p = NULL;}
#define STRSAVE(_s)       nstrsave(_s)

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

#define IS_NULL(_s)  (((_s) == NULL) || ((_s)[0] == '\0'))

int
 _assert_fail = 0;

#endif

