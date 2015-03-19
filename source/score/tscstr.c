/*
 * TSCSTR.C - test SCSTR.C routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

#define MAX 32

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Function    : checkd(...)
 * Programmer  : Carolyn Sharp
 * Created     : November 18, 1991
 * Revised     : November 25, 1991
 *               Moved debug prints in. 
 *
 * Description : Checks return value of named double function of one 
 *               argument against expected result.
 *               Prints arguments and function if test fails.
 *
 * Returns     : 0|1 for PASS|FAIL
 *
 */

int checkd(double dval, char *fcn_name, char xxx[MAX],
	   double expected, int pass)
   {int fail;
    double d, tolerance;

    tolerance = 1.0e-13;
    d         = 0.5*fabs(dval - expected)/(fabs(dval) + fabs(expected));
    fail      = (d > tolerance);
    io_printf(STDOUT, "%s       \t%s     \t%11e  %11e\t %s \n",
	      xxx, fcn_name, dval, expected,
	      (fail ? "Fail" : " OK"));

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Function    : checkl(...)
 * Programmer  : Carolyn Sharp
 * Created     : November 22, 1991
 * Revised     : November 25, 1991
 *               Moved debug prints in. 
 *
 * Description : Checks return value of named int function of two
 *               arguments against expected result.
 *               Prints arguments and function if test fails.
 *
 * Returns     : 0|1 for PASS|FAIL
 *
 */

int checkl(long mval, char *fcn_name, char xxx[MAX], long lval,
	   int lexpected)
   {int fail;

    fail = (mval != lval);
    io_printf(STDOUT, "%s\t%s     \t   %5ld\t %5d\t   %s\n",
	      xxx, fcn_name, mval, lexpected,
	      (fail ? "Fail" : " OK"));

    return(fail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* File        : tscstr.c - Test program for pact/score/scstr.c
 * Externals   : fltfil, intfil, pact/score
 * Machines    : sandpiper, phoenix.ocf
 *
 * Programmer  : Carolyn Sharp
 * Created     : October 30, 1991
 * Revised     : November 25, 1991
 *               Reorganized to match tscctl.c (no change in function).
 *             : November 26, 1991
 *               Added strtol tests.
 * Programmer  : Dennis Braddy
 *             : April 6, 1995
 *               Removed strtol and other system routine calls.
 *               The point of providing PACT routines is to replace
 *               sytem routines that don't work right!
 *
 * Description : Reads FILEs fltfil and intfil of assorted float/integer
 *               numbers and checks values returned by score functions
 *               SC_atof, SC_strtod, and SC_strtol.
 *
 */
 
int main(int c, char **v)
   {int base;                         /* 2-36 allowed, score always 10? */
    int lexpected;                    /* expected result (input, int) */
    int fail[3];                      /* number of failures per case */
    int failsum;                      /* total number of failures */
    int i, rv;
    long lval;
    char xxx[MAX];                    /* input string */
    char *endptr;                    /* pointer to remainder or NULL */
    double expected;                  /* expected result (input, float) */
    FILE *fp;                         /* file pointer */
    static char *casename[]  = {"SC_atof", "SC_strtod", "SC_strtol"};

/* process fltfil for atof/strtod functions */
    fp = SC_fopen_safe("fltfil", "r");

    io_printf(STDOUT, "\nString\t\tFunction\t  Measured       Expected\tStatus\n");

/* initialize failure counters */
    for (i = 0; i < 3; fail[i] = 0, i++);

    if (fp != NULL)
       {while (fscanf(fp, "%13s %lg %d", xxx, &expected, &lexpected) != EOF)
	   {fail[0] += checkd(SC_atof(xxx), "SC_atof",
			      xxx, expected, lexpected); 
	    fail[1] += checkd(SC_strtod(xxx, &endptr), "SC_strtod",
			      xxx, expected, lexpected);};

	SC_fclose_safe(fp);};
  
    io_printf(STDOUT, "\nString\tFunction\t  Measured     Expected   Status\n");

/* process intfil for strtol functions */
    fp = SC_fopen_safe("intfil", "r");

    if (fp != NULL)
       {while (fscanf(fp, "%10s %d %ld %d", xxx, &base, &lval, &lexpected) != EOF)
	   fail[2] += checkl(SC_strtol(xxx, &endptr, base), "SC_strtol", xxx,
			     lval, lexpected);

	SC_fclose_safe(fp);};
  
/* report failures */
    failsum = 0;
    for (i = 0; i < 3; i++)
        {if (fail[i])
	    {io_printf(STDOUT, "%d failure%s in %s \n", fail[i],
		       ((fail[i] > 1) ? "s" : "") , casename[i]);
	     failsum += fail[i];};};

    if (failsum)
       printf("%d = total \n", failsum);

    io_printf(STDOUT, "\n");

    rv = failsum ? 1 : 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
