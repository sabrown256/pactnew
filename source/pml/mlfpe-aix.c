/*
 * MLFPE-AIX.C - SIGFPE routines for AIX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include <fptrap.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PM_enable_fpe(int flg, PFSignal_handler hnd)
   {int ret;

    if (flg)
       {ret = fp_trap(FP_TRAP_FASTMODE);
        if (ret == FP_TRAP_ERROR)
           PRINT(stdout, "FP_TRAP CALLED WITH INVALID PARAMETER - _PM_ENABLE_FPE");
        else if (ret == FP_TRAP_UNIMPL)
           PRINT(stdout, "FP_TRAP_FASTMODE NOT SUPPORTED - _PM_ENABLE_FPE");
        fp_enable(TRP_INVALID |
	          TRP_DIV_BY_ZERO |
	          TRP_OVERFLOW);}
    else
       {ret = fp_trap(FP_TRAP_OFF);
        if (ret == FP_TRAP_ERROR)
           PRINT(stdout, "COULD NOT TURN OFF FPE TRAPPING - _PM_ENABLE_FPE");
        fp_disable_all();};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

