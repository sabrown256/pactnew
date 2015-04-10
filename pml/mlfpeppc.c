/*
 * MLFPEPPC.C - PPC SIGFPE routine for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifdef __GNUC__

#include <score.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PM_enable_fpe_ppc(int flg, PFSignal_handler hnd)
   {

    if (flg == TRUE)
       {unsigned int tmp[2] __attribute__ ((__aligned__(8)));

	tmp[0] = 0xFFF80000;     /* more-or-less arbitrary; this is a QNaN */
	tmp[1] = 0xd0;

/*	__asm__("lfd 0,%0; mtfsf 255,0" : : "m" (*tmp) : "fr0"); */

};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

