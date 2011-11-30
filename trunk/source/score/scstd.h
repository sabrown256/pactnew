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

#include <iso-c.h>
#include <scconfig.h>

#include <scope_thread.h>
#include <scunix.h>
#include <scwin32.h>
#include <scbgl.h>

#include <stdio.h>
#include <string.h>

#include <math.h>

#ifndef __cplusplus
# ifdef HAVE_ANSI_C9X_COMPLEX
#  include <complex.h>
# endif
#endif

#include <stdbool.h>
#include <ctype.h>
#include <wctype.h>
#include <float.h>

#ifndef S_SPLINT_S
# include <signal.h>
#endif

#include <setjmp.h>
#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>

#include <scdecls.h>

#include <scgnu.h>

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

#ifndef LLONG_MAX
# define LLONG_MAX    9223372036854775807LL
#endif

#ifndef LLONG_MIN
# define LLONG_MIN    (-LLONG_MAX - 1LL)
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

#undef MAX_PRSZ
#define MAX_PRSZ    4*sizeof(long double)

#undef MAXLINE
#define MAXLINE 255

#undef MAX_BFSZ
#define MAX_BFSZ     4096

#define ABS fabs

#ifndef POW
# define POW pow
#endif

#ifndef POWL
# ifdef COMPILER_PGI
#  define POWL pow
# else
#  define POWL powl
# endif
#endif

#ifdef COMPILER_PGI
# define CREALL creal
# define CIMAGL cimag
#else
# define CREALL creall
# define CIMAGL cimagl
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

/* this is a minor diagnostic that counts unexpected results
 * it will also silence warnings about variables being set but not used
 */
#define SC_ASSERT(_t)   {if ((_t) == FALSE) SC_gs.assert_fail++;}

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

#ifndef _LARGE_FILES

# define OFF_T_MAX LONG_MAX

#endif

FUNCTION_POINTER(void, (*PFVoid));

FUNCTION_POINTER(char, (*PFChar));
FUNCTION_POINTER(int, (*PFInt));
FUNCTION_POINTER(long, (*PFLong));
FUNCTION_POINTER(int64_t, (*PFInt64));
FUNCTION_POINTER(float, (*PFFloat));
FUNCTION_POINTER(double, (*PFDouble));
FUNCTION_POINTER(void, (*PFByte));

FUNCTION_POINTER(char, *(*PFPChar));
FUNCTION_POINTER(int, *(*PFPInt));
FUNCTION_POINTER(long, *(*PFPLong));
FUNCTION_POINTER(int64_t, *(*PFPInt64));
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


typedef struct s_SC_srcloc SC_srcloc;

struct s_SC_srcloc
   {const char *pfile;
    const char *pfunc;
    unsigned int line;};
    
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

/* QUATERNION - defined here so that it participates in type system */

typedef struct s_quaternion quaternion;

struct s_quaternion
   {double s;
    double i;
    double j;
    double k;};

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
 * NOTE: some of the bizarre coding is to avoid compiler warnings
 * deal with it
 */

#define ONCE_SAFE(_f, _l)                                                    \
   {static int _first = TRUE;                                                \
    if (_first == TRUE)                                                      \
       {int _use;                                                            \
        SC_thread_lock *_lck, *_lc;                                          \
        static SC_thread_lock _lock = SC_LOCK_INIT_STATE;                    \
        _lc  = _l;                                                           \
	_lck = (_lc == NULL) ? &_lock : _lc;                                 \
	_use = (_f);                                                         \
        if (_use == TRUE)                                                    \
           _SC_eth_lockon(_lck);                                             \
        if (_first == TRUE)                                                  \
           {_first = FALSE;

#define IF_SAFE(_f, _l, _t)                                                  \
   {if (_t)                                                                  \
       {int _use;                                                            \
        SC_thread_lock *_lck, *_lc;                                          \
        static SC_thread_lock _lock = SC_LOCK_INIT_STATE;                    \
        _lc  = _l;                                                           \
	_lck = (_lc == NULL) ? &_lock : _lc;                                 \
	_use = (_f);                                                         \
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

/* CMAKE - memory allocation and bookkeeping macro */

#define CMAKE(x)                                                             \
    (SC_mem_stats_acc((long) sizeof(x), 0L), (x *) calloc(1, sizeof(x)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CMAKE_N - allocate a block of type x and return a pointer to it */

#define CMAKE_N(x, n)                                                        \
    (SC_mem_stats_acc((long) n*sizeof(x), 0L), (x *) calloc(n, sizeof(x)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CREMAKE - reallocate a block of memory */

#define CREMAKE(p, x, n) ((x *) realloc(p, n*sizeof(x)))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CFREE - release memory and do bookkeeping */

#define CFREE(x)                                                             \
   {SC_mem_stats_acc(0L, (long) sizeof(*x));                                 \
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

/*                SCORE VARIABLE ARGUMENT LIST MACROS                       */

/*--------------------------------------------------------------------------*/

#define SC_VA_START(x)                                                       \
   {va_list __a__;                                                           \
    va_start(__a__, x)

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

