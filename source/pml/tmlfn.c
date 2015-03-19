/*
 * TMLFN.C - test the PML math functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"
#include "scope_math.h"

#define CHECK_RESULT(_r, _s)                                                \
   {double _rl, _sl, _al;                                                   \
    _rl = (_r);                                                             \
    _sl = (_s);                                                             \
    dl  = _rl - _sl;                                                        \
    _al = 0.5*(fabs(_rl) + fabs(_sl));                                      \
    dl  = fabs(dl);                                                         \
    if (_al > 1.0)                                                          \
       dl /= _al;                                                           \
    dlmx = max(dlmx, dl);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REAL_COMPLEX_TEST - test that complex functions with real arguments
 *                   - give same value as real functions
 */

static int real_complex_test(char *name,
			     double (*fr)(double x), complex (*fc)(complex x),
			     double amn, double amx, double mlt)
   {int i, n, ok;
    double mtol, dlmx, dl;
    double a, da, rc, sc;
    complex ci, cc;

    mtol = mlt*PM_machine_precision();

    n  = 100;
    da = (amx - amn)/((double) n);

    ok   = TRUE;
    dlmx = -HUGE;
    for (i = 0, a = amn; a <= amx; a += da, i++)
        {ci = CMPLX(a, 0.0);
	 rc = (*fr)(a);
	 cc = (*fc)(ci);

	 sc = creal(cc);

	 CHECK_RESULT(rc, sc);

	 if (dl > mtol)
	    ok = FALSE;};

    PRINT(stdout, "%20s ... %11.4e /%11.4e (%s)\n",
	  name, dlmx, mtol, (ok == TRUE) ? "ok" : "ng");

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* R_INVERSE_TEST - test a pair of real functions
 *                - which are inverses of each other
 */

static int r_inverse_test(char *name,
			  double (*f)(double x), double (*fi)(double x))
   {int n, ok;
    double mtol, dlmx, dl;
    double a, amn, amx, da, fc, ia;

    mtol = 10.0*PM_machine_precision();

    n = 10;

    amn = mtol;
    amx = PI/2.0 - mtol;
    da  = (amx - amn)/((double) n);

    ok   = TRUE;
    dlmx = -HUGE;
    for (a = amn; a <= amx; a += da)
        {fc = (*f)(a);
	 ia = (*fi)(fc);

	 CHECK_RESULT(a, ia);

	 if (dl > mtol)
	    ok = FALSE;};

    PRINT(stdout, "%20s ... %11.4e /%11.4e (%s)\n",
	  name, dlmx, mtol, (ok == TRUE) ? "ok" : "ng");

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_INVERSE_TEST - test a pair of complex functions
 *                - which are inverses of each other
 */

static int c_inverse_test(char *name,
			  complex (*f)(complex x), complex (*fi)(complex x),
			  double mlt)
   {int i, n, m, ok;
    double mtol, tol, dlmx, dl, rc, sc;
    double a, amn, amx, da;
    double b, bmn, bmx, ba;
    complex c, fc, ic;

    mtol = mlt*PM_machine_precision();
    tol  = 1.0e-15;

    n = 10;
    m = 10;

    amn = tol;
    amx = PI/2.0 - tol;
    da  = (amx - amn)/((double) n);

    bmn = -1.0 + mtol;
    bmx = 1.0 - mtol;
    ba  = (bmx - bmn)/((double) m);

    ok   = TRUE;
    dlmx = -HUGE;
    i    = 0;
    for (a = amn; a <= amx; a += da)
        for (b = bmn; b <= bmx; b += ba)
	    {c  = CMPLX(a, b);
	     fc = (*f)(c);
	     ic = (*fi)(fc);

	     rc = PM_cabs(c);
	     sc = PM_cabs(ic);

	     CHECK_RESULT(rc, sc);

	     if (dl > mtol)
	        ok = FALSE;

	     i++;};

    PRINT(stdout, "%20s ... %11.4e /%11.4e (%s)\n",
	  name, dlmx, mtol, (ok == TRUE) ? "ok" : "ng");

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_EXPT - test the exponential/log functions */

static int test_expt(void)
   {int ok;

    ok = TRUE;

/* test real functions */    
    ok &= r_inverse_test("exp", exp, PM_ln);
    ok &= r_inverse_test("sqr", PM_sqr, PM_sqrt);

/* test complex functions */
    ok &= c_inverse_test("exp", PM_cexp, PM_cln, 10.0);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_TRIG - test the trig functions */

static int test_trig(void)
   {int ok;

    ok = TRUE;

/* test real functions */    
    ok &= r_inverse_test("r sin", sin, PM_asin);
    ok &= r_inverse_test("r cos", cos, PM_acos);
/*    ok &= r_inverse_test("r tan", tan, PM_atan); */

/* test complex functions */
    ok &= c_inverse_test("c sin", PM_csin, PM_casin, 20.0);
    ok &= c_inverse_test("c cos", PM_ccos, PM_cacos, 20.0);
    ok &= c_inverse_test("c tan", PM_ctan, PM_catan, 20.0);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_REAL_VAL - test the some real limits functions */

static int test_real_val(void)
   {int ok;

    ok = TRUE;

    ok &= real_complex_test("ln", PM_ln, PM_cln,
			     0.0001, 10.0, 10.0);
    ok &= real_complex_test("log", PM_log, PM_clog,
			     0.0001, 10.0, 10.0);
    ok &= real_complex_test("recip", PM_recip, PM_crecip,
			     -10.1, 10.2, 10.0);
    ok &= real_complex_test("sqr", PM_sqr, PM_csqr,
			     -10.0, 10.0, 10.0);
    ok &= real_complex_test("sqrt", PM_sqrt,  PM_csqrt,
			     0.0001, 10.0, 10.0);

    ok &= real_complex_test("sin", sin, PM_csin, -10.0, 10.0, 10.0);
    ok &= real_complex_test("cos", cos, PM_ccos, -10.0, 10.0, 10.0);
    ok &= real_complex_test("tan", tan, PM_ctan, -10.0, 10.0, 10.0);

    ok &= real_complex_test("asin", PM_asin, PM_casin, -1.0, 1.0, 10.0);
    ok &= real_complex_test("acos", PM_acos, PM_cacos, -1.0, 1.0, 10.0);
    ok &= real_complex_test("atan", atan,    PM_catan, -PI,  PI,  10.0);

    ok &= real_complex_test("cot", PM_cot, PM_ccot, -0.9*PI, 0.9*PI, 10.0);

    ok &= real_complex_test("exp",  exp,     PM_cexp,  -2.0, 2.0, 10.0);
    ok &= real_complex_test("sinh", sinh,    PM_csinh, -2.0, 2.0, 10.0);
    ok &= real_complex_test("cosh", cosh,    PM_ccosh, -2.0, 2.0, 10.0);
    ok &= real_complex_test("tanh", tanh,    PM_ctanh, -2.0, 2.0, 10.0);
    ok &= real_complex_test("coth", PM_coth, PM_ccoth, -2.1, 2.0, 500.0);

    ok &= real_complex_test("asinh", asinh,  PM_casinh, -4.0, 4.0, 10.0);
    ok &= real_complex_test("acosh", acosh,  PM_cacosh,  1.0, 4.0, 10.0);
    ok &= real_complex_test("atanh", atanh,  PM_catanh, -0.9, 0.9, 10.0);

    _PM_use_pml_bessel = TRUE;

    ok &= real_complex_test("i0", PM_i0, PM_ci0, 0.0001, 10.0, 1.0e1);
    ok &= real_complex_test("i1", PM_i1, PM_ci1, 0.0001, 10.0, 1.0e1);
    ok &= real_complex_test("j0", PM_j0, PM_cj0, 0.0001, 10.0, 1.0e1);
    ok &= real_complex_test("j1", PM_j1, PM_cj1, 0.0001, 10.0, 1.0e1);
    ok &= real_complex_test("k0", PM_k0, PM_ck0, 0.0001, 10.0, 1.0e1);
    ok &= real_complex_test("k1", PM_k1, PM_ck1, 0.0001, 10.0, 1.0e1);
    ok &= real_complex_test("y0", PM_y0, PM_cy0, 0.0001, 10.0, 1.0e1);
    ok &= real_complex_test("y1", PM_y1, PM_cy1, 0.0001, 10.0, 1.0e1);

    _PM_use_pml_bessel = FALSE;

#ifndef SGI
    ok &= real_complex_test("i0'", PM_i0, PM_ci0, 0.0001, 10.0, 5.0e6);
    ok &= real_complex_test("i1'", PM_i1, PM_ci1, 0.0001, 10.0, 5.0e6);
    ok &= real_complex_test("j0'", PM_j0, PM_cj0, 0.0001, 10.0, 5.0e6);
    ok &= real_complex_test("j1'", PM_j1, PM_cj1, 0.0001, 10.0, 5.0e6);
    ok &= real_complex_test("k0'", PM_k0, PM_ck0, 0.0001, 10.0, 5.0e6);
    ok &= real_complex_test("k1'", PM_k1, PM_ck1, 0.0001, 10.0, 5.0e6);
    ok &= real_complex_test("y0'", PM_y0, PM_cy0, 0.0001, 10.0, 2.0e7);
    ok &= real_complex_test("y1'", PM_y1, PM_cy1, 0.0001, 10.0, 2.0e7);
#endif

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a sample program */

int main(int c, char **v)
   {int err, lerr;
    int64_t bytaa, bytfa, bytab, bytfb;

    SC_gs.mm_debug = TRUE;
    SC_zero_space_n(0, -2);

    SC_mem_stats(&bytab, &bytfb, NULL, NULL);

    PM_enable_fpe_n(TRUE, NULL, NULL);

    err = TRUE;

/* test real limits */
    PRINT(STDOUT, "   Real limit test ... \n");
    lerr = test_real_val();
    if (!lerr)
       PRINT(STDOUT, "   Real limit test ... failed\n");
    else
       PRINT(STDOUT, "   Real limit test ... passed\n");
    err &= lerr;

/* test trig functions */
    PRINT(STDOUT, "   Trig test ......... \n");
    lerr = test_trig();
    if (!lerr)
       PRINT(STDOUT, "   Trig test ......... failed\n");
    else
       PRINT(STDOUT, "   Trig test ......... passed\n");
    err &= lerr;

/* test exponential functions */
    PRINT(STDOUT, "   Expt test ......... \n");
    lerr = test_expt();
    if (!lerr)
       PRINT(STDOUT, "   Expt test ......... failed\n");
    else
       PRINT(STDOUT, "   Expt test ......... passed\n");
    err &= lerr;

    SC_mem_stats(&bytaa, &bytfa, NULL, NULL);

    bytaa -= bytab;
    bytfa -= bytfb;

    if ((bytaa - bytfa) != 0)
       {PRINT(STDOUT, "\n\n\t\t   Allocated      Freed       Diff\n");
	PRINT(STDOUT, "\t\t   %9d  %9d  %9d\n\n",
	      bytaa, bytfa, bytaa - bytfa);};

    return(!err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

