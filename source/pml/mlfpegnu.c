/*
 * MLFPEGNU.C - GNU SIGFPE routine for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifdef __GNUC__

#define GCC_FPE_X86
#include <score.h>

#define __USE_GNU

#include <fenv.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ENABLE_FPE_GNU - enable software trapping of floating point
 *                   - exceptions using GNU extentions
 */

void PM_enable_fpe_gnu(int flg)
   {

#if !defined(BEOS)
    if (flg == TRUE)
       feenableexcept(FE_DIVBYZERO | FE_OVERFLOW | FE_INVALID);
    else
       fedisableexcept(FE_DIVBYZERO | FE_OVERFLOW | FE_INVALID);
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

