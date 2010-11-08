/*
 * SCSTD.H - standard header for PACT codes
 *         - designed with portability, uniformity, and consistency
 *         - in mind
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_STD

#define PCK_STD

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                          STANDARD INCLUDES                               */

/*--------------------------------------------------------------------------*/

#include <scconfig.h>

#include <scope_thread.h>
#include <scunix.h>
#include <scwin32.h>
#include <scbgl.h>

#include <stdio.h>
#include <string.h>

#include <math.h>
#include <ctype.h>
#ifndef S_SPLINT_S
# include <signal.h>
#endif
#include <setjmp.h>
#include <time.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <float.h>
#include <assert.h>

#include <scdecls.h>

/*--------------------------------------------------------------------------*/

/*                           STANDARD PRAGMAS                               */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                      STANDARD CONSTANT MACROS                            */

/*--------------------------------------------------------------------------*/

#ifdef SC_DEFINE
#define SC_DECLARE_VAR(_t, _n, _v)    _t _n = _v
#define SC_DECLARE_VARU(_t, _n)       _t _n
#else
#define SC_DECLARE_VAR(_t, _n, _v)    extern _t _n
#define SC_DECLARE_VARU(_t, _n)       extern _t _n
#endif

#ifdef NO_CONST
# define const
#endif

#ifdef NO_VOLATILE
# define volatile
#endif

#ifndef TRUE
# define TRUE 1
#endif
#ifndef FALSE
# define FALSE 0
#endif
 
#ifndef ON
# undef OFF
# define ON  1
# define OFF 0
#endif

#define SMALL   1.0e-100
#undef HUGE
#define HUGE    1.0e100

#ifdef LONG_MAX
# define HUGE_INT LONG_MAX
#else
# define HUGE_INT ((1 << (sizeof(long)-1)) - 1)
#endif

#define SC_BITS_BYTE      8

#ifndef BINARY_MODE_R
# define BINARY_MODE_R         "rb"
#endif

#ifndef BINARY_MODE_W
# define BINARY_MODE_W         "wb"
#endif

#ifndef BINARY_MODE_A
# define BINARY_MODE_A         "ab"
#endif

#ifndef BINARY_MODE_RPLUS
# define BINARY_MODE_RPLUS     "r+b"
#endif

#ifndef BINARY_MODE_WPLUS
# define BINARY_MODE_WPLUS     "w+b"
#endif

#ifndef BINARY_MODE_APLUS
# define BINARY_MODE_APLUS     "a+b"
#endif

#ifndef SEEK_SET
# define SEEK_SET 0
# define SEEK_CUR 1
# define SEEK_END 2
#endif

#undef MAXLINE
#define MAXLINE 255

#undef MAX_BFSZ
#define MAX_BFSZ     4096

#define ABS fabs

#ifndef POW
# define POW pow
#endif

/* get the ticks per second for clock straight */

#ifndef TICKS_SECOND
# ifdef CLOCKS_PER_SEC
#  define TICKS_SECOND CLOCKS_PER_SEC
# else
#  ifdef CLK_TCK
#   define TICKS_SECOND CLK_TCK
#  else
#   define TICKS_SECOND 1000000
#  endif
# endif
#endif

#ifndef FIXNUM
# define FIXNUM long
#endif

#ifdef __cplusplus
# define FUNCTION_POINTER(t, n) typedef t n(...)
#else
# define FUNCTION_POINTER(t, n) typedef t n(void)
#endif

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

#ifdef _LARGE_FILES

# define SC_ATOADD SC_stoll
# define SC_STOADD SC_stoll

#else

# define SC_ATOADD atol
# define SC_STOADD SC_stol
# define OFF_T_MAX LONG_MAX

#endif

#ifdef NO_LONG_LONG
typedef long BIGINT;
typedef unsigned long BIGUINT;
#else
typedef long long BIGINT;
typedef unsigned long long BIGUINT;
#endif

FUNCTION_POINTER(void, (*PFVoid));

FUNCTION_POINTER(char, (*PFChar));
FUNCTION_POINTER(int, (*PFInt));
FUNCTION_POINTER(long, (*PFLong));
FUNCTION_POINTER(BIGINT, (*PFBIGINT));
FUNCTION_POINTER(float, (*PFFloat));
FUNCTION_POINTER(double, (*PFDouble));
FUNCTION_POINTER(void, (*PFByte));

FUNCTION_POINTER(char, *(*PFPChar));
FUNCTION_POINTER(int, *(*PFPInt));
FUNCTION_POINTER(long, *(*PFPLong));
FUNCTION_POINTER(BIGINT, *(*PFPBIGINT));
FUNCTION_POINTER(float, *(*PFPFloat));
FUNCTION_POINTER(double, *(*PFPDouble));
FUNCTION_POINTER(void, *(*PFPByte));
FUNCTION_POINTER(void, *(*PFPVoid));

FUNCTION_POINTER(char, **(*PFPPChar));

typedef void (*PFVoidAPV)(void *);
typedef void *(*PFPVoidAPV)(void *);
typedef int (*PFIntUn)(void *a);
typedef int (*PFIntBin)(void *a, void *b);

#ifdef ANSI
# ifndef SIGNAL_HANDLER_DEFINED
   typedef void (*PFSignal_handler)(int sig);
#  define SIGNAL_HANDLER_DEFINED
# endif
#endif

#ifndef SIGNAL_HANDLER_DEFINED
# ifdef __cplusplus
  typedef void (*PFSignal_handler)(int sig);
# else
  typedef void (*PFSignal_handler)();
# endif
#endif

#undef SIGNAL_HANDLER_DEFINED

typedef char *CONST * CONST_STRINGS;


/* Pcons definition
 *    - useful for linked lists, association lists and whatnot
 *    - the type fields are for bookkeeping that may be crucial to
 *    - other packages
 */

typedef struct s_pcons pcons;

struct s_pcons
   {char *car_type;
    void *car;
    char *cdr_type;
    void *cdr;};

/*--------------------------------------------------------------------------*/

/*                                SETJMP MACROS                             */

/*--------------------------------------------------------------------------*/

#ifndef JMP_BUF

#ifndef S_SPLINT_S
# define JMP_BUF            sigjmp_buf
# define SETJMP(_x)         sigsetjmp(_x, TRUE)
# define LONGJMP(_x, _v)    siglongjmp(_x, _v)
#else
# define JMP_BUF            jmp_buf
# define SETJMP(_x)         setjmp(_x)
# define LONGJMP(_x, _v)    longjmp(_x, _v)
#endif

#endif

/*--------------------------------------------------------------------------*/

/*                          STANDARD PROCEDURAL MACROS                      */

/*--------------------------------------------------------------------------*/

/* the idea it to guarantee and contain all the logic to ensure that a block
 * of code is executed just one time even in threaded regions
 *    _F   lock and unlock the block iff TRUE
 *    _L   use the supplied lock if non-NULL otherwise
 *         use a locally defined lock of block scope
 */

#define ONCE_SAFE(_f, _l)                                                    \
   {static int _first = TRUE;                                                \
    if (_first == TRUE)                                                      \
       {int _use;                                                            \
        SC_thread_lock *_lck;                                                \
        static SC_thread_lock _lock = SC_LOCK_INIT_STATE;                    \
	_lck = (_l == NULL) ? &_lock : _l;                                   \
	_use = ((_f) == TRUE);                                               \
        if (_use == TRUE)                                                    \
           _SC_eth_lockon(_lck);                                             \
        if (_first == TRUE)                                                  \
           {_first = FALSE;

#define IF_SAFE(_f, _l, _t)                                                  \
   {if (_t)                                                                  \
       {int _use;                                                            \
        SC_thread_lock *_lck;                                                \
        static SC_thread_lock _lock = SC_LOCK_INIT_STATE;                    \
	_lck = (_l == NULL) ? &_lock : _l;                                   \
	_use = ((_f) == TRUE);                                               \
        if (_use == TRUE)                                                    \
           _SC_eth_lockon(_lck);                                             \
        if (_t)                                                              \
           {

#define END_SAFE                                                             \
           };                                                                \
        if (_use == TRUE)                                                    \
           _SC_eth_lockoff(_lck);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DEREF - dereference a pointer */

#define DEREF(x) (*(char **) (x))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE - memory allocation and bookkeeping macro */

#define MAKE(x)                                                              \
    (SC_mem_stats_acc((long) sizeof(x), 0L), (x *) calloc(1, sizeof(x)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_N - allocate a block of type x and return a pointer to it */

#define MAKE_N(x, n)                                                         \
    (SC_mem_stats_acc((long) n*sizeof(x), 0L), (x *) calloc(n, sizeof(x)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REMAKE - memory reallocation and bookkeeping macro 
 *        - no book keeping here since we don't have sufficient info
 */

#define REMAKE(p, x) ((x *) realloc(p, (long) sizeof(x)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REMAKE_N - reallocate a block of type x and return a pointer to it
 *          - no book keeping here since we don't have sufficient info
 */

#define REMAKE_N(p, x, n) ((x *) realloc(p, n*sizeof(x)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SFREE - release memory and do bookkeeping */

#define SFREE(x)                                                             \
   {SC_mem_stats_acc(0L, (long) sizeof(*x));                                 \
    free(x);                                                                 \
    x = NULL;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SFREE_N - release memory and do bookkeeping on arrays */

#define SFREE_N(x, n)                                                        \
   {SC_mem_stats_acc(0L, (long) n*sizeof(*x));                               \
    free(x);                                                                 \
    x = NULL;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAX - the oft wanted maximum */

#ifndef __cplusplus
# ifndef max
#  define max(a, b) ((a) > (b) ? (a) : (b))
# endif
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MIN - companion to max */

#ifndef __cplusplus
# ifndef min
#  define min(a, b) ((a) < (b) ? (a) : (b))
# endif
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* F77_FUNC - attempt a uniform naming of FORTRAN functions which 
 *          - gets around loader naming conventions
 *          -
 *          - F77_FUNC(foo, FOO)(x, y, z)
 *
 *          - FF_ID is kept for backwards compatability with other codes
 */

#ifndef F77_FUNC

# ifdef ANSI_F77
#  define F77_FUNC(x, X)  X
#  define FF_ID(x, X)  X
# endif

# ifndef F77_FUNC
#  define F77_FUNC(x, X)  x ## _
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

#  define FCB_ID(x, X) F77_FUNC(x, X)

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

/* F77_STRING - get around the problem of C and F77 vendors who can't
 *            - agree on what a string is
 *            - NOTE: Scot Futral contributed info and code here
 */

/* NOTE: this case is preserved only to demonstrate why we keep this
 * abstraction of the Fortran string
 * the UNICOS to which we ported long ago is gone and if Cray keeps UNICOS
 * it will be a new port for us anyway
 */
#ifdef UNICOS

# ifndef __cplusplus
#  include <fortran.h>

typedef _fcd F77_string;

#  define SC_F77_C_STRING(_f) _fcdtocp((_f))
#  define SC_C_F77_STRING(_c) _cptofcd((_c), ((_c) ? strlen((_c)) : 0))

# else

typedef char *F77_string;

#  define SC_F77_C_STRING(_f) ((char *) _f)
#  define SC_C_F77_STRING(_c) ((char *) _c)

# endif

#else

typedef char *F77_string;

# define SC_F77_C_STRING(_f) ((char *) _f)
# define SC_C_F77_STRING(_c) ((char *) _c)

#endif

#define SC_FORTRAN_STR_C(_cstr, _fstr, _nc)                                  \
   {strncpy(_cstr, SC_F77_C_STRING(_fstr), _nc);                             \
    _cstr[_nc] = '\0';}

/*--------------------------------------------------------------------------*/

/*                SCORE VARIABLE ARGUMENT LIST MACROS                       */

/*--------------------------------------------------------------------------*/

#ifdef ANSI

#define SC_VA_START(x)                                                       \
   {va_list __a__;                                                           \
    va_start(__a__, x)

#endif

#define SC_VA_ARG(x)      va_arg(__a__, x)

#define SC_VSPRINTF(s, f) vsprintf(s, f, __a__)

#define SC_VSNPRINTF(s, n, f) (*SC_vsnprintf_hook)(s, n, f, __a__)

#define SC_VFPRINTF(s, f) vfprintf(s, f, __a__)

#define SC_VA_END                                                            \
    va_end(__a__);}

#define SC_VA_VAR __a__

#ifdef HAVE_VA_COPY

# define SC_VA_SAVE(x)                                                       \
    {va_list __a__;                                                          \
     va_copy(__a__, *(x))

# define SC_VA_RESTORE(x)                                                    \
     va_end(__a__);}

#endif

#if !defined(SC_VA_SAVE) && defined(NO_VA_LIST)

# define SC_VA_SAVE(x)                                                       \
    {va_list __a__;

# define SC_VA_RESTORE(x)                                                    \
     }

#endif

#if !defined(SC_VA_SAVE)

# define SC_VA_SAVE(x)                                                       \
    {va_list __a__;                                                          \
     __a__ = *(x)

# define SC_VA_RESTORE(x)                                                    \
     *(x) = __a__;}

#endif

#ifdef __GNUC__
#define _FORTIFY_SOURCE 2
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                    STANDARD VARIABLE DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

extern int
 *LAST;

extern int
 (*SC_vsnprintf_hook)(char *dst, size_t nc, const char *fmt, va_list a);

/*--------------------------------------------------------------------------*/

/*                    STANDARD FUNCTION DECLARATIONS                        */

/*--------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif

