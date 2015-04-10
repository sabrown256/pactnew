/*
 * SCSOL.H - SOLARIS support header for PACT codes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef SOLARIS

#ifndef PCK_SOLARIS

#define PCK_SOLARIS

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                         STANDARD CONSTANT MACROS                         */

/*--------------------------------------------------------------------------*/

#ifndef _LARGE_FILES
# define _LARGE_FILES
#endif

#ifndef _FILE_OFFSET_BITS
# define _FILE_OFFSET_BITS 64
#endif

#ifndef _LARGEFILE64_SOURCE
# define _LARGEFILE64_SOURCE 1
#endif

#ifndef _LARGEFILE_SOURCE
# define _LARGEFILE_SOURCE 1
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

#ifdef __cplusplus
}
#endif

#endif

#endif

