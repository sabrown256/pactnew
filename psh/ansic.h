/*
 * ANSIC.H - header to provide ANSI C environment
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#ifndef PCK_ANSIC
#define PCK_ANSIC

#if defined(__OPENCC__)
# undef __GNUC__
#endif

#ifndef NO_STD_INCLUDES
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <stddef.h>
# include <stdarg.h>
# include <stdint.h>
# include <limits.h>
# include <math.h>
# include <stdbool.h>
# include <ctype.h>
# include <wctype.h>
# include <float.h>

# ifndef __cplusplus
#  ifdef HAVE_ANSI_C9X_COMPLEX
#   include <complex.h>

/* GOTCHA: if you have an old GCC - e.g. Solaris */
#   if !defined(__SUNPRO_C)
#    if (__GNUC__ < 4)
#       undef I
#       define I	(__extension__ 1.0iF)
#    endif
#   endif

#  endif
# endif

# ifndef S_SPLINT_S
#  include <signal.h>
# endif

# include <time.h>
# include <errno.h>
# include <setjmp.h>
# include <assert.h>

#endif

#undef TRUE
#undef FALSE

#define TRUE       1
#define FALSE      0

/* define a pseudo boolean type that works like a bool
 * but is an int
 * this allows all of the existing int based interfaces
 * to be fully backward compatible while gaining some of
 * the advantages of a designated boolean typ
 */

enum e_pboolean
   {B_F = 0, B_T};

typedef int pboolean;

#ifndef __cplusplus

# undef max
# define max(a, b) ((a) > (b) ? (a) : (b))

# undef min
# define min(a, b) ((a) < (b) ? (a) : (b))

#endif

#endif

