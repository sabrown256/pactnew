/*
 * MLFPE-OSX.C - SIGFPE routines for MAC OSX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifdef MACOSX

#if (FPU_TYPE == ppc)
# include <architecture/ppc/fp_regs.h>
#else
# include <architecture/i386/fpu.h>
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PM_enable_fpe(int flg, PFSignal_handler hnd)
   {

    if (flg)
       {ppc_fp_scr_t r;
	static volatile int looping = 0;

	r = get_fp_scr();

/* turn off exception bits to prevent immediate re-fault */
	r.fx      = r.fex    = r.vx     = r.ox      = 0;
	r.ux      = r.zx     = r.xx                 = 0;
	r.vx_snan = r.vx_isi = r.vx_idi = r.vx_zdz  = 0;
	r.vx_imz  = r.vx_xvc = r.vx_cvi = r.vx_soft = 0;

/* rsvd2 is actually vx_sqrt, set by fsqrt instruction
 * fsqrt is optional, not present on G4 and earlier Macs (but on G5)
 */
	r.rsvd2 = 0;

/* these only have to be set once, but may as well set anyway */
	r.ve = 1;  /* invalid */
	r.oe = 1;  /* overflow */
	r.ue = 0;  /* underflow */
	r.ze = 1;  /* zero divide */
	r.xe = 0;  /* inexact */
	if (looping == 0)
	   {looping |= 1;
	    set_fp_scr(r);
	    looping &= ~1;};

	looping = 0;};

    _PM_enable_fpe_ansi(flg);


    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

