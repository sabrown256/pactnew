/*
 * SCOPE_COMPLEX.H - header for complex arithmetic
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#ifndef PCK_COMPLEX

#include "cpyright.h"

#define PCK_COMPLEX

#include "score.h"

/*--------------------------------------------------------------------------*/

/*                               TYPEDEFS                                   */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus

typedef REAL complex[2];

# define PM_REAL_C(x)       ((x)[0])
# define PM_IMAGINARY_C(x)  ((x)[1])

#else

struct s_complex
   {REAL real;
    REAL imag;};

typedef struct s_complex complex;

#  define PM_REAL_C(x)       x.real
#  define PM_IMAGINARY_C(x)  x.imag

#endif

/*--------------------------------------------------------------------------*/

/*                       STANDARD PROCEDURAL MACROS                         */

/*--------------------------------------------------------------------------*/

/* PM_COMPLEX - set the parts of a complex number */

#define PM_COMPLEX(_b, _c)                                                   \
   (PM_REAL_C(cx_reg) = (double) (_b),                                       \
    PM_IMAGINARY_C(cx_reg) = (double) (_c),                                  \
    cx_reg)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COMPLEX_CONJUGATE - return the complex conjugate of c */

#define PM_COMPLEX_CONJUGATE(_c)                                             \
   (PM_REAL_C(cx_reg)      =  PM_REAL_C(_c),                                 \
    PM_IMAGINARY_C(cx_reg) = -PM_IMAGINARY_C(_c),                            \
    cx_reg)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_PLUS_CC - binary complex addition, b + c */

#define PM_PLUS_CC(b, c)                                                     \
   (PM_REAL_C(cx_reg)      = PM_REAL_C(b) + PM_REAL_C(c),                    \
    PM_IMAGINARY_C(cx_reg) = PM_IMAGINARY_C(b) + PM_IMAGINARY_C(c),          \
    cx_reg)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_PLUS_RC - b + c where b is real and c is complex */

#define PM_PLUS_RC(b, c)                                                     \
   (PM_REAL_C(cx_reg)      = b + PM_REAL_C(c),                               \
    PM_IMAGINARY_C(cx_reg) = PM_IMAGINARY_C(c),                              \
    cx_reg)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MINUS_CC - binary complex subtraction, b - c */

#define PM_MINUS_CC(b, c)                                                    \
   (PM_REAL_C(cx_reg)      = PM_REAL_C(b) - PM_REAL_C(c),                    \
    PM_IMAGINARY_C(cx_reg) = PM_IMAGINARY_C(b) - PM_IMAGINARY_C(c),          \
    cx_reg)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_TIMES_CC - binary complex multiplication, b * c */

#define PM_TIMES_CC(b, c)                                                    \
   (cx_ttac = (PM_REAL_C(b)*PM_REAL_C(c)),                                   \
    cx_ttbd = (PM_IMAGINARY_C(b)*PM_IMAGINARY_C(c)),                         \
    cx_ttr  = (cx_ttac - cx_ttbd),                                           \
    cx_tti  = ((PM_REAL_C(b) + PM_IMAGINARY_C(b))*                           \
               (PM_REAL_C(c) + PM_IMAGINARY_C(c)) - cx_ttac - cx_ttbd),      \
    PM_REAL_C(cx_reg)      = cx_ttr,                                         \
    PM_IMAGINARY_C(cx_reg) = cx_tti,                                         \
    cx_reg)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_TIMES_RC - b * c where b is real and c is complex */

#define PM_TIMES_RC(b, c)                                                    \
   (PM_REAL_C(cx_reg)      = b*PM_REAL_C(c),                                 \
    PM_IMAGINARY_C(cx_reg) = b*PM_IMAGINARY_C(c),                            \
    cx_reg)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_TIMES_IC - b * c where b is the magnitude of a pure imaginary number
 *             - and c is complex */

#define PM_TIMES_IC(b, c)                                                    \
   (PM_REAL_C(cx_reg)      = -b*PM_IMAGINARY_C(c),                           \
    PM_IMAGINARY_C(cx_reg) =  b*PM_REAL_C(c),                                \
    cx_reg)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DIVIDE_CC - binary complex division, b / c */

#define PM_DIVIDE_CC(b, c) (PM_cdiv(b,c))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MODULUS_C - compute the complex modulus */

#define PM_MODULUS_C(x) (PM_cabs(x))

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#define PM_COMPLEX_SWAP(a, b)                                                \
    {SC_SWAP_VALUE(complex, a, b);}

/*--------------------------------------------------------------------------*/

/*                    STANDARD VARIABLE DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

extern double
 cx_ttr,
 cx_tti,
 cx_ttm,
 cx_ttac,
 cx_ttbd;

extern complex
 cx_reg;

/*--------------------------------------------------------------------------*/

/*                    STANDARD FUNCTION DECLARATIONS                        */

/*--------------------------------------------------------------------------*/


/* MLFFT.C declarations */

extern complex
 *PM_uniform_complex_y(int no, REAL *xo, int ni, REAL *xi, complex *yi),
 *PM_fft_sc_real(REAL *x, int n, int flag);

extern int
 PM_fft_sc_real_data(complex **pyo, REAL **pxo, REAL *xi, REAL *yi,
		     int ni, double xmn, double xmx, int ordr),
 PM_fft_sc_complex_data(complex **pyo, REAL **pxo, REAL *xi,
			complex *yi, int ni, double xmn, double xmx,
			int flag, int ordr),
 PM_fft_sc_complex(complex *x, int n, int flag);


/* MLCFNC.C declarations */

extern complex
 PM_csqr(complex c),
 PM_csqrt(complex c),
 PM_cln(complex c),
 PM_clog(complex c),
 PM_cexp(complex c),
 PM_csin(complex c),
 PM_ccos(complex c),
 PM_ctan(complex c),
 PM_ccot(complex c),
 PM_csinh(complex c),
 PM_ccosh(complex c),
 PM_ctanh(complex c),
 PM_ccoth(complex c),
 PM_casin(complex c),
 PM_cacos(complex c),
 PM_catan(complex c),
 PM_crandom(complex c),
 PM_cconjugate(complex c),
 PM_crecip(complex c),
 PM_ci0(complex x),
 PM_ci1(complex x),
 PM_cin(complex x, double n),
 PM_cj0(complex x),
 PM_cj1(complex x),
 PM_cjn(complex x, double n),
 PM_ck0(complex x),
 PM_ck1(complex x),
 PM_ckn(complex x, double n),
 PM_cy0(complex x),
 PM_cy1(complex x),
 PM_cyn(complex x, double n),
 PM_ctchn(complex x, double n),
 PM_cadd(complex a, complex b),
 PM_csub(complex a, complex b),
 PM_cmlt(complex a, complex b),
 PM_cdiv(complex a, complex b),
 PM_cpow(complex a, complex b),
 PM_chorner(complex x, double *c, int mn, int mx);

extern double
 PM_cabs(complex c),
 PM_carg(complex c),
 PM_cfloor(complex c),
 PM_cround(complex c);

extern int
  PM_cequal(complex x, complex y),
  PM_cclose(complex x, complex y, double tol);


/* MLSFNC.C declarations */

extern complex
 PM_Ylm(double theta, double phi, int l, int m);


/* MLMATH.C declarations */

extern void
 PM_smooth_filter(complex *z, int n, double pts);

extern int
 PM_smooth_fft(REAL *x, REAL *y, int n, int pts,
	       void (*fnc)(complex *cx, int nt, double tol));

#endif


