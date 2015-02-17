/*
 * SCC.H - support routines for PACT C pre-compiler
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_SCC
#define PCK_SCC

#include "score_int.h"

/*--------------------------------------------------------------------------*/

/*                                   MACROS                                 */

/*--------------------------------------------------------------------------*/

#ifndef EXTERN_VAR
# define EXTERN_VAR extern
#endif

#undef IF
#undef ELSE
#undef CONTINUE
#undef RETURN
#undef SIGNED
#undef ECHO

/*--------------------------------------------------------------------------*/

/*                            STRUCT AND TYPEDEF                            */

/*--------------------------------------------------------------------------*/

typedef struct s_CC_scope_public CC_scope_public;

struct s_CC_scope_public
   {int none;};

/*--------------------------------------------------------------------------*/

/*                            VARIABLE DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

extern CC_scope_public
 CC_gs;

/*--------------------------------------------------------------------------*/

/*                            FUNCTION DECLARATIONS                         */

/*--------------------------------------------------------------------------*/

#endif
