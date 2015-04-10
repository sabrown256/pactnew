/*
 * SCBEOS.H - BEOS support header for PACT codes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef BEOS

#ifndef PCK_BEOS

#define PCK_BEOS

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                         STANDARD CONSTANT MACROS                         */

/*--------------------------------------------------------------------------*/

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

