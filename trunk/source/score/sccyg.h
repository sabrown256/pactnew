/*
 * SCCYG.H - CYGWIN support header for PACT codes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef CYGWIN

#ifndef PCK_CYGWIN

#define PCK_CYGWIN

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                         STANDARD CONSTANT MACROS                         */

/*--------------------------------------------------------------------------*/

#ifdef __GNUC__

# undef HAVE_ALT_LARGE_FILE
# define _LDBL_EQ_DBL

# undef I

# define complex        _Complex
# define _Complex_I	(__extension__ 1.0iF)
# define I               _Complex_I

#endif

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                        STANDARD PROCEDURAL MACROS                        */

/*--------------------------------------------------------------------------*/


#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                      STANDARD VARIABLE DECLARATIONS                      */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                      STANDARD FUNCTION DECLARATIONS                      */

/*--------------------------------------------------------------------------*/

long double
 creall(long double _Complex z),
 cimagl(long double _Complex z);

double
 creal(double _Complex z),
 cimag(double _Complex z);

float
 crealf(float _Complex z),
 cimagf(float _Complex z);

#ifdef __cplusplus
}
#endif

#endif

#endif

