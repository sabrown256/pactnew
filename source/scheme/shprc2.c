/*
 * SHPRC2.C - C and Scheme Process control routines
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

#include "shell/common.h"
#include "shell/libpsh.c"
#include "shell/libdb.c"
#include "shell/libpgrp.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SSI_GEXEC - gexec wrapper/access for SCHEME */

object *_SSI_gexec(SS_psides *si, object *argl)
   {object *o;

    o = SS_null;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
