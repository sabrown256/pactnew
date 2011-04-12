/*
 * MLPFTS.C - test the relative performance of long doubles and doubles
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"
#include "scope_math.h"

#ifdef HAVE_ANSI_FLOAT16

/* older compilers may lack these declarations and
 * still have the functions
 * GNU 3.3.3 for example
 */

extern long double
 expl(long double x),
 logl(long double x),
 sqrtl(long double x);

static JMP_BUF
 cpu;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TIMEOUT - handle timeout for iteration count */

static void timeout(int sig)
   {

    LONGJMP(cpu, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UNARY_TEST - test relative performance of double and long double functions */

static int unary_test(char *name, int to,
		      double (*fd)(double x), long double (*fl)(long double x),
		      double amn, double amx)
   {int n, id, il, ok;
    double ad, xd, da, rd, rl;
    long double al, xl;

    ok = TRUE;

    n  = 1000000000;
    da = (amx - amn)/((double) n);

/* do as many iterations as possible in TO seconds */
    if (SETJMP(cpu) == 0)
       {SC_timeout(to, timeout);

	for (id = 0; TRUE; id++)
	    {for (ad = amn; ad <= amx; ad += da, id++)
	         xd = fd(ad);};};

    rd = ((double) to)/((double) id);

/* do as many iterations as possible in TO seconds */
    if (SETJMP(cpu) == 0)
       {SC_timeout(to, timeout);

	for (il = 0; TRUE; il++)
	    {for (al = amn; al <= amx; al += da, il++)
	         xl = fl(al);};};

    rl = ((double) to)/((double) il);

    io_printf(stdout, "\t   %-10s %8d  %10.3e   %10.3e   %.2f\n",
	      name, id, rd, rl, rl/rd);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test double and long double performance */

int main(int c, char **v)
   {int err, rv;

    PM_enable_fpe(TRUE, NULL);

    err = TRUE;

    io_printf(stdout, "\n");
    io_printf(stdout, "\t   Function      calls      double   longdouble  ratio\n");

    err &= unary_test("exp",  1.0,  exp,  expl,   -2.0, 10.0);
    err &= unary_test("log",  1.0,  log,  logl, 1.0e-8, 1.0e4);
    err &= unary_test("sqrt", 1.0, sqrt, sqrtl, 1.0e-8, 1.0e4);

    io_printf(stdout, "\n");

    rv = (err != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a stub program */

int main(int c, char **v)
   {int rv;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
