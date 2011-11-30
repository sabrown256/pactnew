/*
 * MLFPE-SOL.C - SIGFPE routines for Solaris
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef __GNUC__
# include <sunmath.h>
#else

extern void
 nonstandard_arithmetic(void),
 standard_arithmetic(void);

extern long
 ieee_handler(const char *action, const char *exception,
	      sigfpe_handler_type hdl);
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PM_enable_fpe(int flg, PFSignal_handler hnd)
   {

#ifndef __GNUC__
    if (flg)
       {nonstandard_arithmetic();
	if (ieee_handler("set", "common", hnd) != 0)
           PRINT(stdout, "CAN'T SET FPE HANDLING - _PM_ENABLE_FPE");}
    else
       {standard_arithmetic();
	if (ieee_handler("clear", "common", NULL) != 0)
           PRINT(stdout, "CAN'T CLEAR FPE HANDLING - _PM_ENABLE_FPE");};
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
