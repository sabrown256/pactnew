/*
 * MLFPE-OSF.C - SIGFPE routines for OSF on Alpha
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/* # include <machine/fpu.h> */

/* these are taken from <machine/fpu.h> which has illegal
 * constructs that render it uncompilable (@axposf.pa.dec.com)
 */

#define IEEE_TRAP_ENABLE_INV    0x000002   /* invalid operation */
#define IEEE_TRAP_ENABLE_DZE    0x000004   /* divide by 0 */
#define IEEE_TRAP_ENABLE_OVF    0x000008   /* overflow */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PM_enable_fpe(int flg, PFSignal_handler hnd)
    {unsigned long ost, nst;

     ost = ieee_get_fp_control();
     if (flg)
        {nst = /* ost |  */
               IEEE_TRAP_ENABLE_INV |
               IEEE_TRAP_ENABLE_DZE |
               IEEE_TRAP_ENABLE_OVF;
         ieee_set_fp_control(nst);
         if (nst != ieee_get_fp_control())
            PRINT(stdout, "CAN'T SET FPE HANDLING - _PM_ENABLE_FPE");}
     else
        {nst = /* ost &  */
               ~IEEE_TRAP_ENABLE_INV &
               ~IEEE_TRAP_ENABLE_DZE &
               ~IEEE_TRAP_ENABLE_OVF;
         ieee_set_fp_control(nst);
         if (nst != ieee_get_fp_control())
            PRINT(stdout, "CAN'T CLEAR FPE HANDLING - _PM_ENABLE_FPE");};

     return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

