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

/*--------------------------------------------------------------------------*/

#if defined(HAVE_ANSI_C9X_FENV)

# if defined(HAVE_GNU_FENV)
#  define __USE_GNU
# endif

# include <fenv.h>

# if defined(MACOSX)

/*--------------------------------------------------------------------------*/

/* _PM_ENABLE_FPE_ANSI - rotten MACOSX has non-standard declarations
 *                     - in some versions they are all void (e.g. 10.3.3)
 */

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
    fexcept_t fs;

/* setup to test the following FPE */
    fe = FE_INVALID   |
	 FE_DIVBYZERO |
	 FE_OVERFLOW  |
	 FE_UNDERFLOW |
	 FE_INEXACT;

/* fetch current FPU flags */
    fegetexceptflag(&fs, FE_ALL_EXCEPT);

    if (pfl)
       {if (fs & FE_INVALID)
	   PRINT(stdout, "invalid operations\n");

	if (fs & FE_DIVBYZERO)
	   PRINT(stdout, "division by zero\n");

	if (fs & FE_OVERFLOW)
	   PRINT(stdout, "overflows\n");

	if (fs & FE_UNDERFLOW)
	   PRINT(stdout, "underflows\n");

	if (fs & FE_INEXACT)
	   PRINT(stdout, "inexacts\n");};

    return(fs);}

/*--------------------------------------------------------------------------*/

# else

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
    fexcept_t fs;

/* setup to test the following FPE */
    fe = FE_INVALID   |

#ifdef __FE_DENORM
         __FE_DENORM  |
#endif

	 FE_DIVBYZERO |
	 FE_OVERFLOW  |
	 FE_UNDERFLOW |
	 FE_INEXACT;

/* fetch current FPU flags */
    fegetexceptflag(&fs, FE_ALL_EXCEPT);

    if (pfl)
       {if (fs & FE_INVALID)
	   PRINT(stdout, "invalid operations\n");

#ifdef __FE_DENORM
	if (fs & __FE_DENORM)
	   PRINT(stdout, "denormalized numbers\n");
#endif

	if (fs & FE_DIVBYZERO)
	   PRINT(stdout, "division by zero\n");

	if (fs & FE_OVERFLOW)
	   PRINT(stdout, "overflows\n");

	if (fs & FE_UNDERFLOW)
	   PRINT(stdout, "underflows\n");

	if (fs & FE_INEXACT)
	   PRINT(stdout, "inexacts\n");};

    return(fs);}

/*--------------------------------------------------------------------------*/

# endif

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

/* PM_ENABLE_FPE - enable software trapping of floating point exceptions */

void PM_enable_fpe(int flg, PFSignal_handler hnd)
   {

/* assign the signal handler to SIGFPE */
    if ((flg == TRUE) && (hnd != NULL))
       SC_signal(SIGFPE, hnd);
    else
       SC_signal(SIGFPE, SIG_DFL);

/* set the exceptions which should raise SIGFPE */
    _PM_enable_fpe(flg, hnd);

    _PM.fpe_enabled = flg;

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
 *             - responsibility to SFREE it
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

/* PM_FIX_NAND - replace all NaNs in D with the value V
 *            - return the number of NaNs found
 */

long PM_fix_nand(long n, double *d, int mask, double v)
   {int ty;
    long i, nn;

/* find them and fix them */
    nn = 0;
    for (i = 0; i < n; i++)
        {ty = PM_fp_typed(d[i]);
	 ty = ABS(ty);
	 if ((ty & mask) != 0)
	    {nn++;
	     d[i] = v;};};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COUNT_NAND - return the number of NaNs found in D */

long PM_count_nand(long n, double *d, int mask)
   {int ty;
    long i, nn;

    nn = 0;
    for (i = 0; i < n; i++)
        {ty = PM_fp_typed(d[i]);
	 ty = ABS(ty);
	 if ((ty & mask) != 0)
	    nn++;};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FIX_NANF - replace all NaNs in F with the value V
 *            - return the number of NaNs found
 */

long PM_fix_nanf(long n, float *f, int mask, float v)
   {int ty;
    long i, nn;

    nn = 0;
    for (i = 0; i < n; i++)
        {ty = PM_fp_typef(f[i]);
	 ty = ABS(ty);
	 if ((ty & mask) != 0)
	    {nn++;
	     f[i] = v;};};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COUNT_NANF - return the number of NaNs found in F */

long PM_count_nanf(long n, float *f, int mask)
   {int ty;
    long i, nn;

/* find them and fix them */
    nn = 0;
    for (i = 0; i < n; i++)
        {ty = PM_fp_typef(f[i]);
	 ty = ABS(ty);
	 if ((ty & mask) != 0)
	    nn++;};

    return(nn);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FP_TYPED - return one of the following depending on what
 *             - kind of floating point value D is
 *             -   0  D is a legal IEEE double
 *             -   1  D is IEEE plus Infinity
 *             -   2  D is IEEE minus Infinity
 *             -   4  D is an IEEE positive signalling NaN
 *             -   8  D is an IEEE negative signalling NaN
 *             -   16 D is an IEEE positive quiet NaN
 *             -   32 D is an IEEE negative quiet NaN
 */

int PM_fp_typed(double d)
   {int rv, le, sg;
    BIGUINT ba, bm, bs, b1, l1, l2, l3;
    char *ca;

    rv = 1;
    ca = (char *) &rv;
    le = (ca[0] == 1);
    ba = *(BIGUINT *) &d;

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

/* PM_FP_TYPEF - return one of the following depending on what
 *             - kind of floating point value F is
 *             -   0  F is a legal IEEE float
 *             -   1  F is IEEE plus Infinity
 *             -   2  F is IEEE minus Infinity
 *             -   4  F is an IEEE positive signalling NaN
 *             -   8  F is an IEEE negative signalling NaN
 *             -   16 F is an IEEE positive quiet NaN
 *             -   32 F is an IEEE negative quiet NaN
 */

int PM_fp_typef(float f)
   {int rv, le, sg;
    int ba, bm, bs, b1, l1, l2, l3;
    char *ca;

    rv = 1;
    ca = (char *) &rv;
    le = (ca[0] == 1);
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
