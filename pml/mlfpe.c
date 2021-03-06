/*
 * MLFPE.C - SIGFPE routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#define x86_64   100
#define ia32     200
#define x87      300
#define ppc      400
#define alpha    500

void
 _PM_enable_fpe(int flg, PFSignal_handler hnd);

SC_thread_lock
 PM_fpe_lock = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/

#if defined(HAVE_ANSI_C9X_FENV)

# if defined(HAVE_GNU_FENV)
#  undef __USE_GNU
#  define __USE_GNU
# endif

# include <fenv.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ENABLE_FPE_ANSI - ANSI standard floating point handling */

void _PM_enable_fpe_ansi(int flg)
   {int fe;
    fexcept_t flag;

    fe = FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW;

    if (flg)

/* fetch current FPU flags */
       {fegetexceptflag(&flag, FE_ALL_EXCEPT);

/* set FPU flags */
	fesetexceptflag(&flag, fe);}

    else
       feclearexcept(fe);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FPE_TRAPS - return and optionally print a report of
 *               - the FPEs that are being trapped
 */

int _PM_fpe_traps(int pfl)
   {int fe;

    fe = fetestexcept(FE_ALL_EXCEPT);

    if (pfl)
       {if (fe & FE_INVALID)
	   PRINT(stdout, "invalid operations\n");

#ifdef __FE_DENORM
	if (fe & __FE_DENORM)
	   PRINT(stdout, "denormalized numbers\n");
#endif

	if (fe & FE_DIVBYZERO)
	   PRINT(stdout, "division by zero\n");

	if (fe & FE_OVERFLOW)
	   PRINT(stdout, "overflows\n");

	if (fe & FE_UNDERFLOW)
	   PRINT(stdout, "underflows\n");

	if (fe & FE_INEXACT)
	   PRINT(stdout, "inexacts\n");};

    return(fe);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FPU_STATUS - return and optionally print a report of
 *                - the status of the floating point unit
 */

int _PM_fpu_status(int pfl)
   {int fs, fe;

/* setup to test the following FPE */
    fe = FE_INVALID   |        /* 0x01 - invalid operation */

#ifdef __FE_DENORM
         __FE_DENORM  |        /* 0x02 - denormalized number */
#endif

	 FE_DIVBYZERO |        /* 0x04 - devision by zero */
	 FE_OVERFLOW  |        /* 0x08 - result not representable due to overflow */
	 FE_UNDERFLOW |        /* 0x10 - result not representable due to underflow */
	 FE_INEXACT;           /* 0x20 - inexact result */

    fs = fetestexcept(fe);

    if (pfl)
       {if (fs & FE_INVALID)
	   PRINT(stdout, "An invalid operation has occured\n");

#ifdef __FE_DENORM
	if (fs & __FE_DENORM)
	   PRINT(stdout, "A denormalized number has occured\n");
#endif

	if (fs & FE_DIVBYZERO)
	   PRINT(stdout, "Division by zero has occured\n");

	if (fs & FE_OVERFLOW)
	   PRINT(stdout, "An overflow has occured\n");};

    return(fs);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

#if (FPU_TYPE == x87)

int fpu_type = 0;

#elif defined(MACOSX)

# include "mlfpe-osx.c"

int fpu_type = 3;

#elif (FPU_TYPE == ppc)

int fpu_type = 1;

#elif defined(HAVE_GNU_FENV)

# include "mlfpe-gnu.c"

int fpu_type = 2;

#elif defined(SOLARIS)

# include "mlfpe-sol.c"

int fpu_type = 4;

/* ----------------  these are moribund ----------------- */

#elif defined(AIX)

# include "mlfpe-aix.c"

int fpu_type = 5;

#elif defined(HPUX)

# include "mlfpe-hpux.c"

int fpu_type = 6;

/* ----------------  these are dead ----------------- */

#elif defined(SGI)

# include "mlfpe-sgi.c"

int fpu_type = 7;

#elif defined(OSF)

# include "mlfpe-osf.c"

int fpu_type = 8;

#elif (FPU_TYPE == alpha)

# include "mlfpe-alpha.c"

int fpu_type = 9;

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DETECT_FPU - detect the FPU and describe it */

void PM_detect_fpu(unsigned int *pf, unsigned int *pm)
   {

    ONCE_SAFE(FALSE, NULL)

#if (FPU_TYPE == x87)
       {extern void _PM_detect_fpu_x86(unsigned int *pf, unsigned int *pm);

        _PM_detect_fpu_x86(&_PM.features, &_PM.mask);}
#endif

    END_SAFE;

    if (pf != NULL)
       *pf = _PM.features;

    if (pm != NULL)
       *pm = _PM.mask;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ENABLE_FPE_N - enable software trapping of floating point exceptions
 *                 - sets the FPE handler on current thread
 */

void PM_enable_fpe_n(int flg, PFSignal_handler hnd, void *a)
   {

/* assign the signal handler to SIGFPE on each thread */
    switch (flg)
       {case TRUE :
	     if (hnd != NULL)
	        SC_signal_action_n(SIGFPE, hnd, a, -1, 0, -1);
	     break;
	case FALSE :
	     SC_signal_action_n(SIGFPE, SIG_DFL, NULL, 0, 0, -1);
	     break;
	default :
	     break;};

    if (flg != -1)
       {SC_LOCKON(PM_fpe_lock);

/* set the exceptions which should raise SIGFPE
 * does this have to be done on each thread?
 * seems like doing it once for the CPU/FPU as a whole is the right thing
 */
	_PM_enable_fpe(flg, hnd);

	_PM.fpe_enabled = (flg != FALSE);

	SC_LOCKOFF(PM_fpe_lock);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ENABLE_FPE_T - enable software trapping of floating point exceptions
 *                 - threaded version of PM_enable_fpe_n which
 *                 - sets the FPE handler on all threads
 */

void PM_enable_fpe_t(int flg, PFSignal_handler hnd, void *a, int nb)
   {

/* assign the signal handler to SIGFPE on each thread */
    switch (flg)
       {case TRUE :
	     if (hnd != NULL)
	        SC_signal_action_t(SIGFPE, hnd, a, nb, 0, -1);
	     break;
	case FALSE :
	     SC_signal_action_t(SIGFPE, SIG_DFL, NULL, 0, 0, -1);
	     break;
	default :
	     break;};

    SC_LOCKON(PM_fpe_lock);

/* set the exceptions which should raise SIGFPE
 * does this have to be done on each thread?
 * seems like doing it once for the CPU/FPU as a whole is the right thing
 */
    _PM_enable_fpe(flg, hnd);

    _PM.fpe_enabled = (flg != FALSE);

    SC_LOCKOFF(PM_fpe_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ENABLE_FPE - Change handling of floating point exceptions.
 *               - If FLG is TRUE and HND is non-NULL associate HND with
 *               - SIGFPE.  If FLG is TRUE enable hardware trapping of
 *               - floating point exceptions.  If FLG is FALSE associate
 *               - SIG_DFL with SIGFPE and disable hardware trapping of
 *               - floating point exceptions.
 *
 * #bind PM_enable_fpe fortran() python()
 */

void PM_enable_fpe(int flg, PFSignal_handler hnd)
   {

    PM_enable_fpe_n(flg, hnd, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FPE_TRAPS - return and optionally print a report of
 *              - the FPEs that are being trapped
 */

int PM_fpe_traps(int pfl)
   {int fs;

#ifdef HAVE_ANSI_C9X_FENV

    fs = _PM_fpe_traps(pfl);

#else

    fs = 0;

#endif

    return(fs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FPU_STATUS - return and optionally print a report of
 *               - the status of the floating point unit
 */

int PM_fpu_status(int pfl)
   {int fs;

#ifdef HAVE_ANSI_C9X_FENV

    fs = _PM_fpu_status(pfl);

#else

    fs = 0;

#endif

    return(fs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CLEAR_FPU - clear any bad FPU status indicators
 *              - this is to help control FPE handling
 */

void PM_clear_fpu(void)
   {

#ifdef HAVE_ANSI_C9X_FENV
    int fe;

/* setup to clear the following FPE */
    fe = FE_INVALID   |        /* 0x01 - invalid operation */

#ifdef __FE_DENORM
         __FE_DENORM  |        /* 0x02 - denormalized number */
#endif

	 FE_DIVBYZERO |        /* 0x04 - devision by zero */
	 FE_OVERFLOW  |        /* 0x08 - result not representable due to overflow */
	 FE_UNDERFLOW |        /* 0x10 - result not representable due to underflow */
	 FE_INEXACT;           /* 0x20 - inexact result */

    feclearexcept(fe);

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SAVE_FPU - allocate space and save the state of the FPU in that space
 *             - return it as a void pointer
 *             - NOTE: the space can be used any number of times to
 *             - restore a particular state hence it is the application's
 *             - responsibility to CFREE it
 */

void *PM_save_fpu(void)
   {void *a;

    a = NULL;

#if (FPU_TYPE == x87)
    {extern void *PM_save_fpu_x86(void);

     a = PM_save_fpu_x86();}
#endif

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_RESTORE_FPU - restore the state of the FPU from the supplied
 *                - state A which must have been returned from a prior
 *                - call to PM_save_fpu
 */

void PM_restore_fpu(void *a)
   {

#if (FPU_TYPE == x87)
    {extern void PM_restore_fpu_x86(void *a);

     PM_restore_fpu_x86(a);};
#endif

    return;}

/*--------------------------------------------------------------------------*/

/*                                NAN ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* PM_FIX_NAND - Replace all NaNs and Infs of type indicated by MASK
 *             - in the array of N double values D with V.
 *             - MASK is a bit array:
 *             -    bit   condition
 *             -      1  IEEE plus Infinity
 *             -      2  IEEE minus Infinity
 *             -      4  IEEE positive signalling NaN
 *             -      8  IEEE negative signalling NaN
 *             -     16  IEEE positive quiet NaN
 *             -     32  IEEE negative quiet NaN
 *             - Return the number of NaNs and Infs found.
 *
 * #bind PM_fix_nand fortran() scheme() python()
 */

long PM_fix_nand(long n, double *d, int mask, double v)
   {int ty;
    long i, nn;

/* find them and fix them */
    nn = 0;
    for (i = 0; i < n; i++)
        {ty = PM_fp_typed(d[i]);
	 ty = abs(ty);
	 if ((ty & mask) != 0)
	    {nn++;
	     d[i] = v;};};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COUNT_NAND - Count the NaNs and Infs in the N element double array D
 *               - which match specifications in MASK.
 *               - MASK is a bit array:
 *               -    bit   condition
 *               -      1  IEEE plus Infinity
 *               -      2  IEEE minus Infinity
 *               -      4  IEEE positive signalling NaN
 *               -      8  IEEE negative signalling NaN
 *               -     16  IEEE positive quiet NaN
 *               -     32  IEEE negative quiet NaN
 *               - Return the number of NaNs and Infs found.
 *
 * #bind PM_count_nand fortran() scheme() python()
 */

long PM_count_nand(long n, double *d, int mask)
   {int ty;
    long i, nn;

    nn = 0;
    for (i = 0; i < n; i++)
        {ty = PM_fp_typed(d[i]);
	 ty = abs(ty);
	 if ((ty & mask) != 0)
	    nn++;};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FIX_NANF - Replace all NaNs and Infs of type indicated by MASK
 *             - in the array of N float values F with V.
 *             - MASK is a bit array:
 *             -    bit   condition
 *             -      1  IEEE plus Infinity
 *             -      2  IEEE minus Infinity
 *             -      4  IEEE positive signalling NaN
 *             -      8  IEEE negative signalling NaN
 *             -     16  IEEE positive quiet NaN
 *             -     32  IEEE negative quiet NaN
 *             - Return the number of NaNs and Infs found.
 *
 * #bind PM_fix_nanf fortran() scheme() python()
 */

long PM_fix_nanf(long n, float *f, int mask, float v)
   {int ty;
    long i, nn;

    nn = 0;
    for (i = 0; i < n; i++)
        {ty = PM_fp_typef(f[i]);
	 ty = abs(ty);
	 if ((ty & mask) != 0)
	    {nn++;
	     f[i] = v;};};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COUNT_NANF - Count the NaNs and Infs in the N element float array F
 *               - which match specifications in MASK.
 *               - MASK is a bit array:
 *               -    bit   condition
 *               -      1  IEEE plus Infinity
 *               -      2  IEEE minus Infinity
 *               -      4  IEEE positive signalling NaN
 *               -      8  IEEE negative signalling NaN
 *               -     16  IEEE positive quiet NaN
 *               -     32  IEEE negative quiet NaN
 *               - Return the number of NaNs and Infs found.
 *
 * #bind PM_count_nanf fortran() scheme() python()
 */

long PM_count_nanf(long n, float *f, int mask)
   {int ty;
    long i, nn;

/* find them and fix them */
    nn = 0;
    for (i = 0; i < n; i++)
        {ty = PM_fp_typef(f[i]);
	 ty = abs(ty);
	 if ((ty & mask) != 0)
	    nn++;};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FP_TYPED - Return one of the following depending on what
 *             - kind of double value D is:
 *             -    0  a legal IEEE double
 *             -    1  IEEE plus Infinity
 *             -    2  IEEE minus Infinity
 *             -    4  IEEE positive signalling NaN
 *             -    8  IEEE negative signalling NaN
 *             -   16  IEEE positive quiet NaN
 *             -   32  IEEE negative quiet NaN
 *
 * #bind PM_fp_typed fortran() scheme() python()
 */

int PM_fp_typed(double d)
   {int rv, sg;
    u_int64_t ba, bm, bs, b1, l1, l2, l3;

    rv = 1;
    ba = *(u_int64_t *) &d;

    l1 = 0xf0;
    l2 = 0xf8;
    l3 = 1;
    l1 = l1 << 48;
    l2 = l2 << 48;
    l3 = (l3 << 56) - 1LL;

    bs = 1;
    b1 = 0xff;

    bm = (ba & l3);
    bs = (bs << 63);
    bs = (ba & bs);
    b1 = (b1 << 56);
    b1 = (ba & b1) >> (8*(sizeof(double) - 1));
    sg = (bs == 0) ? 1 : 2;

    if ((b1 != 0x7f) && (b1 != 0xff))
       rv = 0;

/* plus and minus infinities
 *   positive double  7f f0000000000000
 *   negative double  ff f0000000000000
 */
    else if (bm == l1)
       rv = 1*sg;

/* plus and minus signalling NaNs
 *   positive double  7f f0000000000001 - 7f f7ffffffffffff
 *   negative double  ff f0000000000001 - ff f7ffffffffffff
 */
    else if ((l1 < bm) && (bm < l2))
       rv = 4*sg;

/* plus and minus quiet NaNs
 *   positive double  7f f8000000000000 - 7f ffffffffffffff
 *   negative double  ff f8000000000000 - ff ffffffffffffff
 */
    else if ((l2 <= bm) && (bm <= l3))
       rv = 16*sg;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FP_TYPEF - Return one of the following depending on what
 *             - kind of float value F is:
 *             -    0  a legal IEEE double
 *             -    1  IEEE plus Infinity
 *             -    2  IEEE minus Infinity
 *             -    4  IEEE positive signalling NaN
 *             -    8  IEEE negative signalling NaN
 *             -   16  IEEE positive quiet NaN
 *             -   32  IEEE negative quiet NaN
 *
 * #bind PM_fp_typef fortran() scheme() python()
 */

int PM_fp_typef(float f)
   {int rv, sg;
    int ba, bm, bs, b1, l1, l2, l3;

    rv = 1;
    ba = *(int *) &f;

    l1 = 0x800000;
    l2 = 0xc00000;
    l3 = 0xffffff;

    bm = (ba & l3);
    bs = (ba & 0x80000000);
    b1 = (ba & 0xff000000) >> (8*(sizeof(float) - 1));
    sg = (bs == 0) ? 1 : 2;

    if ((b1 != 0x7f) && (b1 != 0xff))
       rv = 0;

/* plus and minus infinities
 *   positive float  7f 800000
 *   negative float  ff 800000
 */
    else if (bm == l1)
       rv = 1*sg;

/* plus and minus signalling NaNs
 *   positive float   7f 800001 - 7f bfffff
 *   negative float   ff 800001 - ff bfffff
 */
    else if ((l1 < bm) && (bm < l2))
       rv = 4*sg;

/* plus and minus quiet NaNs
 *   positive float   7f c00000 - 7f ffffff
 *   negative float   ff c00000 - ff ffffff
 */
    else if ((l2 <= bm) && (bm <= l3))
       rv = 16*sg;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
