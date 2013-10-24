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

#ifdef __OPENCC__
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

#ifndef __cplusplus

# undef max
# define max(a, b) ((a) > (b) ? (a) : (b))

# undef min
# define min(a, b) ((a) < (b) ? (a) : (b))

#endif

#endif

