/*
 * SCWIN32.H - WIN32 support header
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef WIN32

#ifndef PCK_WIN32

#define PCK_WIN32

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                        STANDARD CONFIGURATIONS                           */

/*--------------------------------------------------------------------------*/

/* NO_SHELL should be set for platforms lacking shell windows
 * this would include DOS and MAC (which is pre OS X) as well
 * as Windows
 */

#define NO_SHELL

#define directory_delim "\\"
#define directory_delim_c '\\'

/*--------------------------------------------------------------------------*/

/*                          STANDARD PROCEDURAL MACROS                      */

/*--------------------------------------------------------------------------*/

#define fileno(x) _fileno(x)

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                    STANDARD VARIABLE DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                    STANDARD FUNCTION DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif

#endif
