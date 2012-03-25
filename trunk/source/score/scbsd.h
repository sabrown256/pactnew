/*
 * SCBSD.H - (Free)BSD support header for PACT codes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#if defined(FREEBSD)

#ifndef PCK_FREEBSD

#define PCK_FREEBSD

#include "cpyright.h"

/* GOTCHA: SIGPOLL seems to have disappeared
 * this work around needs to be re-analyzed from scratch
 */
#ifndef SIGPOLL
# define SIGPOLL 7
#endif

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

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif

#endif

