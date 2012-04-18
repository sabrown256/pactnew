/*
 * MLFPEX86.C - X86 SIGFPE routine for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

/* FPU Status Register
 *
 * Useful when debugging.  Using the GDB 'info reg fstat' command
 * you can see the 16 bit status word.  The bits are:
 * 
 *     0   Invalid operation
 *     1   Denormalized
 *     2   Zero Divide
 *     3   Overflow
 *     4   Underflow
 *     5   Precision
 *     6   Stack Fault
 *     7   Exception Flag
 *     8   C0
 *     9   C1
 *    10   C2
 *    11   
 *    12
 *    13
 *    14  C3
 *    15  Busy
 * 
 * 0-5 - exception flags
 * they appear in the same order as the exception masks in the control register
 * if the corresponding condition exists the bit is set
 * they do not depend on the exception masks in the control register
 * 
 *  6  - stack fault
 * a stack fault occurs when there is a stack overflow or underflow
 * if this bit is set the C1 condition code bit indicates a
 * stack overflow (C1=1) or stack underflow (C1=0)
 * 
 *  7 - the logical OR of bits 0-5
 * 
 */

#ifdef __GNUC__

/* too many codes define this - so get rid of it in this context */
#undef CPU

#define GCC_FPE_X86
#include <score.h>

#define x86_64   100
#define ia32     200

#undef X86_PREFIX
#if (CPU_TYPE == x86_64)
# define X86_PREFIX "r"
#else
# define X86_PREFIX "e"
#endif

#define fnstcw(_a)              __asm __volatile("fnstcw %0" : "=m" (*(_a)))
#define fnstsw(_a)              __asm __volatile("fnstsw %0" : "=m" (*(_a)))

typedef union u_fpu_state fpu_state;

union u_fpu_state
   {double a;
    char b[512];};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_DETECT_FPU_X86 - detect what kind of FPU is available
 *                    -   0x001   x87 fpu on chip
 *                    -   0x002   mmx technology
 *                    -   0x004   fxsave/fxrestor instructions present
 *                    -   0x008   sse extensions
 *                    -   0x010   sse2 extensions
 *                    -   0x020   sse3 extensions
 *                    -   0x040   daz supported
 *                    - developed by Dave Munro in Yorick
 */

void _PM_detect_fpu_x86(unsigned int *pf, unsigned int *pm)
   {unsigned int features, mask, x, y;

    features = 0;
    mask     = 0;
    x        = 0;
    y        = 0;

#if (MACOSX_VERSION >= 6)

/* Opteron result */
    features = 0x5f;
    mask     = 0x2ffff;

#else

    __asm __volatile ("pushf\n\t pop %%"X86_PREFIX"ax" : "=a" (x));

    y = x;          /* set x and y to original value of eflags */
    x ^= 0x200000;  /* flip bit 21, ID */

    __asm __volatile ("push %%"X86_PREFIX"ax\n\t popf" : : "a" (x));
    __asm __volatile ("pushf\n\t pop %%"X86_PREFIX"ax" : "=a" (x));

    if (x ^ y)
       {unsigned long ecx, edx;

/* this cpu has the cpuid instruction, restore original eflags */
	__asm __volatile ("push %%"X86_PREFIX"ax\n\t popf" : : "a" (y));

/* get mmx, sse related feature bits from cpuid */
	__asm __volatile ("mov %%"X86_PREFIX"bx, %%"X86_PREFIX"si \n\t"
			  "cpuid \n\t"
			  "xchg %%"X86_PREFIX"bx, %%"X86_PREFIX"si"
			  : "=c" (ecx), "=d" (edx) : "a" (1) : "si");

	features = (edx & 1) | ((edx & 0x7800000) >> 22) | ((ecx & 1) >> 5);
	if ((features & 0x004) && (features & 0x018))

/* get MXCSR mask to find out if DAZ supported */
	   {unsigned char pstate[528], *state;

	    memset(pstate, 0, 528);

/* get a 16-byte aligned state buffer for fxsave */
	    state = pstate + 15;
	    state = ((state - (unsigned char *) 0) & (~0xfL)) +
	            (unsigned char *) 0;

	    __asm __volatile ("fxsave %0" : : "m" (*state));

	    mask = *((unsigned int *) (state + 28));
	    if (!mask)
	       mask = 0xffbf;
	    else if (mask & 0x40)
	       features |= 0x040;};};
#endif

    if (pf != NULL)
       *pf = features;

    if (pm != NULL)
       *pm = mask;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

fpu_state *PM_save_fpu_x86(void)
   {fpu_state *fs;

    fs = CMAKE(fpu_state);

#ifdef CPU_ENABLE_SSE
    __asm __volatile("fxsave %0" : "=m" (*fs));
#else
    __asm __volatile("fnsave %0" : "=m" (*fs));
#endif

    return(fs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void PM_restore_fpu_x86(void *a)
   {fpu_state *fs;

    fs = (fpu_state *) a;

#ifdef CPU_ENABLE_SSE
    __asm("fxrstor %0" : : "m" (*fs));
#else
    __asm("frstor %0" : : "m" (*fs));
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ENABLE_FPE - enable software trapping of floating point
 *                - exceptions for IA32 architectures
 *                - FPU Control Register:
 *                -   Exception Masks Bits 0-5 (1 ignore, 0 trap)
 *                -      0 Invalid Operation
 *                -      1 Denormalized
 *                -      2 Zero Divide
 *                -      3 Overflow
 *                -      4 Underflow
 *                -      5 Inexact
 *                -  Precision Control Bits 8-9
 *                -      0 24 bit mantissa
 *                -      1 reserved
 *                -      2 53 bit mantissa
 *                -      3 64 bit mantissa
 *                -  Rounding Control Bits 10-11
 *                -      0 to nearest
 *                -      1 down
 *                -      2 up
 *                -      3 truncate
 *                -  Infinity Control Bit 12
 *                -      0 Affine      -Inf < +Inf
 *                -      1 Projective  -Inf == +Inf
 *
 *   bit meaning         x87      sse      yes=mask=1, no=deliver=0
 * flush to zero          --     0x8000       yes
 * infinity control     0x1000     --    (for 287 coprocessor compatibility)
 * rounding control     0x0c00   0x6000        0
 * precison control     0x0300     --    (0x0300 means 64-bit extended)
 * precision mask       0x0020   0x1000       yes
 * underflow mask       0x0010   0x0800       yes
 * overflow mask        0x0008   0x0400       no
 * zero divide mask     0x0004   0x0200       no
 * denormal op mask     0x0002   0x0100       yes
 * invalid op mask      0x0001   0x0080       no
 * denormals are zero     --     0x0040       yes
 */

void _PM_enable_fpe(int flg, PFSignal_handler hnd)
   {

#ifndef CYGWIN

    unsigned int cw, mxcsr;
    unsigned int mask;
    extern void PM_detect_fpu(unsigned int *pf, unsigned int *pm);

/* expected this to work but we get SIGFPE everytime we get here
 * after the first time
 */

#if defined(CYGWIN)

    __asm__("fclex" : : );

#endif

    PM_detect_fpu(NULL, &mask);

/* set the X87 FPU */
    if (flg)
       cw = 0x1372;
    else
       cw = 0x137f;

    __asm__("fldcw %0" : : "m" (cw));

/* set the SSE unit */
    if (flg)
       {if (mask != 0)
	   {mxcsr = 0x9940 & mask;

	    __asm__ ("ldmxcsr %0" : : "m" (mxcsr));};};

#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

