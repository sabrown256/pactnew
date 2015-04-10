/*
 * SCPCC.H - support header for PCC compiler
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifdef COMPILER_PCC

#ifndef PCK_PCC

#define PCK_PCC

#include "cpyright.h"

/*--------------------------------------------------------------------------*/

/*                      STANDARD CONSTANT MACROS                            */

/*--------------------------------------------------------------------------*/

#ifndef __cplusplus

# undef I
# define I 	(*_PM_i)

#endif

/*--------------------------------------------------------------------------*/

/*                           STANDARD TYPEDEFS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                          STANDARD PROCEDURAL MACROS                      */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                    STANDARD VARIABLE DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

#ifndef __cplusplus

extern long double _Complex
 *_PM_i;

#endif

/*--------------------------------------------------------------------------*/

/*                    STANDARD FUNCTION DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
