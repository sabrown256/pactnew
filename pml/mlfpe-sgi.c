/*
 * MLFPE-SGI.C - SIGFPE routines for SGI
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

/* NOTE: SGI dramatically changed the API around IRIX 6.3
 *       The old argument was an unsigned long and the
 *       new is unsigned int **
 *       Both are advertised to be "a pointer to the address
 *       of the instruction which caused the exception"
 */

#include <sigfpe.h>

#ifndef SGI_FPE_HAND_TYPE
# define SGI_FPE_HAND_TYPE  unsigned int **
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PM_enable_fpe(int flg, PFSignal_handler hnd)
   {

# ifndef HAVE_NO_SIGFPE

    if (flg)
       {sigfpe_[_UNDERFL].repls = _ZERO;

	handle_sigfpes(_ON,
		       _EN_OVERFL | _EN_DIVZERO | _EN_INVALID,
		       NULL, _REPLACE_HANDLER_ON_ERROR,
		       (void (*)(SGI_FPE_HAND_TYPE)) hnd);}
    else
       {handle_sigfpes(_OFF,
		       _EN_OVERFL | _EN_DIVZERO | _EN_INVALID,
		       NULL, _REPLACE_HANDLER_ON_ERROR,
		       (void (*)(SGI_FPE_HAND_TYPE)) hnd);};

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
