/*
 * SCOSX.H - MACOSX support header for PACT codes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

/* MACOSX predefines __APPLE__ */
#ifdef __APPLE__

#if !defined(PCK_MACOSX)

#define PCK_MACOSX

#include "cpyright.h"

/* GOTCHA: mid 10.6 there was a patch concerning POSIX compliance
 * which broke the build
 * this work around needs to be re-analyzed from scratch with 10.7
 */
#ifndef SIGPOLL
# define SIGPOLL 7
#endif

/*--------------------------------------------------------------------------*/

/*                         STANDARD CONSTANT MACROS                         */

/*--------------------------------------------------------------------------*/

#undef HAVE_ALT_LARGE_FILE

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

#ifdef __cplusplus
}
#endif

#endif

#endif

