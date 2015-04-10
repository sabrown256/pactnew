/*
 * MLFPE-HPUX.C - SIGFPE routines for HPUX and PA RISC
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PM_enable_fpe(int flg, PFSignal_handler hnd)

#if defined(HAS_ANSI_FENV)

    fesettrapenable(FE_OVERFLOW | FE_DIVBYZERO | FE_INVALID);

#else

    fpsetdefaults();

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

