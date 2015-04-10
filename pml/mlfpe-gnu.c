/*
 * MLFPE-GNU.C - FPE handling with GNU extension
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include <score.h>

#define __USE_GNU

#include <fenv.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ENABLE_FPE - enable software trapping of floating point
 *                - exceptions using GNU extentions
 */

void _PM_enable_fpe(int flg, PFSignal_handler hnd)
   {

    if (flg == TRUE)
       feenableexcept(FE_DIVBYZERO | FE_OVERFLOW | FE_INVALID);
    else
       fedisableexcept(FE_DIVBYZERO | FE_OVERFLOW | FE_INVALID);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

