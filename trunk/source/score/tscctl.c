/*
 * TSCCTL.C - test SCCTL.C routines
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

/* Function    : tSC_regx_match
 * Test for    :  SC_regx_match
 * Input       : regxfil = file of strings, patterns and expected results
 * Output      : prints (failures/everything) if debug flag = (0/1)
 * Externals   : fopen, fscanf, fclose, printf 
 * Returns     : total number of failures detected
 *
 * Description : This function loops through a file of strings and
 *               patterns which have been chosen to match or not
 *               using regular expression specifiers * and ? .
 *
 * Notes       : In case SC_regx_match is extended, the input file
 *               needs to be expanded but the test function does not
 *               need to be changed.
 */

int tSC_regx_match(void)
   {FILE *fp;                         /* file pointer */
    char name[MAX];                   /* name of file of strings */
    char s[MAX];                      /* string to search */
    char patt[MAX];                   /* pattern to be matched */
    int truth;                        /* expected result (T=1, F=0) */
    int retval;                       /* actual result */
    int fail, failsum = 0;

    strcpy(name, "regxfil");

/* process input file */
    fp = SC_fopen_safe(name, "r");
    if (fp != NULL)
       {io_printf(STDOUT, "\nString\tPattern\tExpect\tMeasured Status\n");
	while (fscanf(fp,"%10s %10s %d", s, patt, &truth) != EOF)
	   {retval = SC_regx_match(s, patt);
	    fail = ((retval == truth) ? 0 : 1);
	    failsum += fail;
	    io_printf(STDOUT, "%s\t%s\t%5d\t%5d\t  %s\n",
		      s, patt, truth, retval,
		      (fail ? "Fail" : " OK"));};

	SC_fclose_safe(fp);};

/* report failures */
    if (failsum)
       io_printf(STDOUT, "\n%d failure%s in SC_regx_match\n\n", failsum,
		 ((failsum>1) ? "s" : ""));

    return(failsum);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Function    : tSC_xxxstrp
 * Test for    :  SC_numstrp, SC_intstrp, SC_fltstrp
 * Input       : 
 * Output      : prints (failures/everything) if debug flag = (0/1)
 * Externals   : fopen, fscanf, fclose, printf, strchr, strcmp, strlen,
 *               SC_strtol, SC_strtod 
 * Returns     : total number of failures detected
 *
 * Description : This function loops through files of integers and floats
 *               to check the score predicates that return true/false
 *               depending on the sort of input string encountered.
 *
 * Notes       : These functions depend on SC_strtol and SC_strtod and
 *               so cannot succeed in case their tests fail (tscstr.c).
 *             : Restriction: base = 10 for one digit integers, which
 *               ignores a..Z --why?
 *               also, radix = 10 wired for most cases --to match floats?
 *             : Leading '+' n/a.
 *             : Not double counting int|flt and num failure but should?
 */

int tSC_xxxstrp(void)
   {int base;                         /* 2-36 allowed, score always 10? */
    int truth;                        /* actual true/false value */
    int retval;                       /* actual result */
    int fail;                         /* set to 1 if case fails */
    int ifailsum;                     /* running sum of intstrp failures*/
    int ffailsum;                     /* running sum of fltstrp failures*/
    int rv;
    char name[MAX];
    char xxx[MAX];                    /* string to search */
    double fval;
    FILE *fp;

    ifailsum = 0;
    ffailsum = 0;

/* process input file of integers */
    strcpy(name, "intfil");
    fp = SC_fopen_safe(name, "r");
    if (fp != NULL)
       {io_printf(STDOUT, "\nString\tBase\tExpect\tMeasured Status\n");
	while (fscanf(fp, "%31s %d %*d %d", xxx, &base, &truth) != EOF)
	   {retval = SC_intstrp(xxx, base);
	    fail   = (retval != truth);
	    ifailsum += fail;

	    io_printf(STDOUT, "%s\t%2d\t%5d\t%5d\t  %s\n",
		      xxx, base, truth, retval,
		      (fail ? "Fail" : " OK"));};

	SC_fclose_safe(fp);};

    if (ifailsum)
       io_printf(STDOUT, "\n%d failure%s in SC_intstrp\n",
		 ifailsum, ((ifailsum > 1) ? "s" : ""));

/* process input file of floats */
    strcpy(name, "fltfil");
    fp = SC_fopen_safe(name, "r");

    if (fp != NULL)
       {io_printf(STDOUT, "\nString\tExpect\tMeasured Status\n");
	while (fscanf(fp, "%31s %lg %d", xxx, &fval, &truth) != EOF)
	   {retval = SC_fltstrp(xxx);
	    fail   = (retval != truth);
	    ffailsum += fail;

	    io_printf(STDOUT, "%s\t%5d\t%5d\t  %s\n",
		      xxx, truth, retval,
		      (fail ? "Fail" : " OK"));};

	SC_fclose_safe(fp);};

/* report failures */
    if (ffailsum)
       io_printf(STDOUT, "\n%d failure%s in SC_fltstrp \n",
		 ffailsum, ((ffailsum > 1) ? "s" : ""));

    rv = ifailsum + ffailsum;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* File        : tscctl.c - Test program for pact/score/scctl.c
 * Functions   : tSC_regx_match, tSC_xxxstrp
 * Externals   : regxfil, intfil, fltfil, pact/score
 * Programmer  : Carolyn Sharp
 * Created     : November 21, 1991
 * Revised     : 
 *
 * Description : Test program for all the non-interactive functions
 *               in scctl.c .
 * Notes       : See interact.c for the tests for the rest.
 */

int main(int c, char **v)
   {int fail, rv;

    fail = 0;

    fail += tSC_regx_match();
    fail += tSC_xxxstrp();

    io_printf(STDOUT, "\n");

    rv = fail ? 1 : 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
