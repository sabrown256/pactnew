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

/* protect C++ programs from C99 complex which are incompatible */

#ifndef __cplusplus

/* clean the slate */
#undef complex

/*--------------------------------------------------------------------------*/

/*                       STANDARD PROCEDURAL MACROS                         */

/*--------------------------------------------------------------------------*/

/* Open64 compiler has optimizer bug with original CMPLX macro */
#if defined(__OPENCC__)

# undef CMPLX
# define CMPLX(_rp, _ip)  _PM_make_complex(_rp, _ip)

#endif

#if defined(__FreeBSD__)
# undef CMPLX
#endif

/* define the C11 complex constructor if not already defined */

#if !defined(CMPLX)
# define CMPLX(_b, _c)               ((_b) + (_c)*I)
#endif

/*--------------------------------------------------------------------------*/

#if defined(HAVE_ANSI_C9X_COMPLEX)

# define complex      double _Complex 

/* if we have C99 or later complex types and functions these
 * macros give older, pre-C99 complex support
 */

# define PM_PLUS_CC(_b, _c)            ((_b) + (_c))
# define PM_PLUS_RC(_b, _c)            ((_b) + (_c))
# define PM_MINUS_CC(_b, _c)           ((_b) - (_c))
# define PM_TIMES_CC(_b, _c)           ((_b) * (_c))
# define PM_TIMES_RC(_b, _c)           ((_b) * (_c))
# define PM_TIMES_IC(_b, _c)           ((_b) * (_c))
# define PM_DIVIDE_CC(_b, _c)          ((_b) / (_c))

# define PM_COMPLEX(_b, _c)            ((_b) + (_c)*I)
# define PM_REAL_C(_c)                 creal(_c)
# define PM_IMAGINARY_C(_c)            cimag(_c)
# define PM_MODULUS_C(_c)              cabs(_c)
# define PM_COMPLEX_CONJUGATE(_c)      conj(_c)

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/* if we have C89 or earlier provide complex type and functions
 * these macros emulate a degree of C99 complex support
 */

#ifndef CYGWIN
typedef struct s_complex complex;
#endif

struct s_complex
   {double real;
    double imag;};

# define PM_PLUS_CC                    PM_plus_cc
# define PM_PLUS_RC(_b, _c)            {(_b) + (_c).real, (_c).imag}
# define PM_MINUS_CC(_b, _c)           PM_minus_cc
# define PM_TIMES_CC(_b, _c)           PM_times_cc
# define PM_TIMES_RC(_b, _c)           {(_b)*(_c).real, (_b)*(_c).imag}
# define PM_TIMES_IC(_b, _c)           {(_b)*(_c).real, (_b)*(_c).imag}
# define PM_DIVIDE_CC(_b, _c)          PM_divide_cc

# define PM_COMPLEX(_b, _c)            {(_b), (_c)}
# define PM_REAL_C(_c)                 (_c).real
# define PM_IMAGINARY_C(_c)            (_c).imag
# define PM_COMPLEX_CONJUGATE(_c)      (-(_c).imag)
# define PM_MODULUS_C(_c)                                                   \
    {(_c).real * (_c).real + (_c).imag * (_c).imag}

# define cabs   PM_cabs
# define conj   PM_cconjugate
# define carg   PM_carg
# define cexp   PM_cexp
# define clog   PM_cln
# define csqrt  PM_csqrt
# define csin   PM_csin
# define ccos   PM_ccos
# define ctan   PM_ctan
# define csinh  PM_csinh
# define ccosh  PM_ccosh
# define ctanh  PM_ctanh
# define casin  PM_casin
# define cacos  PM_cacos
# define catan  PM_catan
# define casinh PM_casinh
# define cacosh PM_cacosh
# define catanh PM_catanh
# define cpow   PM_cpow

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

#define PM_COMPLEX_SWAP(_b, _c)       SC_SWAP_VALUE(complex, _b, _c)

#define PM_CLOSETO_COMPLEX(_ok, _x, _y, _tol)                                \
   {long double _del;                                                        \
    _del = (2.0*(_x - _y)/(ABS(_x) + ABS(_y) + SMALL));                      \
    _ok  = ((_del < -_tol) || (_tol < _del));}

/*--------------------------------------------------------------------------*/

/*                               TYPEDEFS                                   */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                    STANDARD VARIABLE DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                    STANDARD FUNCTION DECLARATIONS                        */

/*--------------------------------------------------------------------------*/

/* MLFFT.C declarations */

extern complex
 *PM_uniform_complex_y(int no, double *xo, int ni, double *xi, complex *yi),
 *PM_fft_sc_real(double *x, int n, int flag);

extern int
 PM_fft_sc_real_data(complex **pyo, double **pxo, double *xi, double *yi,
		     int ni, double xmn, double xmx, int ordr),
 PM_fft_sc_complex_data(complex **pyo, double **pxo, double *xi,
			complex *yi, int ni, double xmn, double xmx,
			int flag, int ordr),
 PM_fft_sc_complex(complex *x, int n, int flag);


/* MLCFNC.C declarations */

extern complex
 _PM_make_complex(double rp, double ip),
 PM_plus_cc(complex b, complex c),
 PM_minus_cc(complex b, complex c),
 PM_times_cc(complex b, complex c),
 PM_divide_cc(complex b, complex c),
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
 PM_casinh(complex c),
 PM_cacosh(complex c),
 PM_catanh(complex c),
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
 PM_cround(complex c),
 PM_distance_cc(complex a, complex b);

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
 PM_smooth_fft(double *x, double *y, int n, int pts,
	       void (*fnc)(complex *cx, int nt, double tol));

#endif

#endif

