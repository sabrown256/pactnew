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

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <poll.h>
#include "nonstd.h"

#define TRUE       1
#define FALSE      0
#define MAXLINE    512
#define LRG        4096
#define MEGA       1048576
#define N_STACK    32
#define APPEND     100
#define PREPEND    101

#define min(a, b)         ((a) < (b) ? (a) : (b))
#define max(a, b)         ((a) > (b) ? (a) : (b))
#define MAKE(_t)          ((_t *) malloc(sizeof(_t)))
#define MAKE_N(_t, _n)    ((_t *) calloc(_n, sizeof(_t)))
#define REMAKE(p, _t, n)  (p = (_t *) realloc((void *) p, (n)*sizeof(_t)))
#define FREE(_p)          {if (_p != NULL) free(_p); _p = NULL;}
#define STRSAVE(_s)       strdup(_s)

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

extern int
 sched_yield(void);

#endif

