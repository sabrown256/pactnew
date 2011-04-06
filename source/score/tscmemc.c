/*
 * TSCMEMC.C - test SCMEMC.C routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h" 

#define MAXTOK 32

#define NUMTESTS 12

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Function    : tSC_firsttok
 * Test for    :  SC_firsttok
 * Input       : tokfil = file of test tokens and expected results
 *               debug = flag set in tokfil
 * Output      : prints (failures/everything) if debug flag = (0/1)
 * Externals   : fopen, fscanf, fclose, printf 
 * Returns     : total number of failures detected
 *
 * Description : This function loops through a file of tokens 
 *
 */

int tSC_firsttok(void)
   {int fail, failsum, i;
    char *first, *expect;
    char *str[NUMTESTS];
    static char *tok[] = {NULL, NULL, "*", "*", "*",
			  "*abc", "*", "*abc",
			  "x", "x", "x", "x"};

    str[0]  = CSTRSAVE("");
    str[1]  = CSTRSAVE(" ");
    str[2]  = CSTRSAVE("  *");
    str[3]  = CSTRSAVE(" *");
    str[4]  = CSTRSAVE("* ");
    str[5]  = CSTRSAVE("*abc");
    str[6]  = CSTRSAVE("* abc");
    str[7]  = CSTRSAVE("*abc ");
    str[8]  = CSTRSAVE("xyz");
    str[9]  = CSTRSAVE("xyz*");
    str[10] = CSTRSAVE("xyz *");
    str[11] = CSTRSAVE("xyz * ");

    failsum  = 0;

    io_printf(STDOUT, "\nTokenize by \" y\\n\"\n");
    io_printf(STDOUT, "Input   \tFirst\tExpect\tRest\tStatus\n");
    for (i = 0; i < NUMTESTS; i++)
        {io_printf(STDOUT, "|%s|      \t",str[i]);

         first  = SC_firsttok(str[i], " y\n");
         expect = tok[i];

         if (first == NULL)
	    io_printf(STDOUT, "|nil|\t");
	 else
	    io_printf(STDOUT, "|%s|\t", first);

	 if (expect == NULL)
	    io_printf(STDOUT, "|nil|\t");
	 else
	    io_printf(STDOUT, "|%s|\t", expect);

	 io_printf(STDOUT, "|%s|\t", str[i]);

	 if ((first == NULL) && (expect == NULL))
	    fail = 0;
	 else if ((first == NULL) || (expect == NULL))
	    fail = 1;
	 else
	    fail = strcmp(first, expect);

	 io_printf(STDOUT, "%s\n", fail ? " Fail" : "  OK");

	 failsum += fail;};

/* report failures */
    if (failsum)
       io_printf(STDOUT, "\n%d failure%s in SC_regx_match \n", failsum,
		 ((failsum>1) ? "s" : "") );

    io_printf(STDOUT, "\n");

    return(failsum);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* File        : tscmemc.c - Test program for pact/score/scmemc.c
 * Functions   : tSC_firsttok
 * Externals   : pact/score
 * Machines    : phoenix.ocf
 *
 * Programmer  : Carolyn Sharp
 * Created     : February 12, 1992
 * Revised     : 
 *
 * Description : Test program for all the string and token functions
 *               in scmemc.c .
 * Notes       : See tmm.c for the tests of memory management functions.
 */

int main(int c, char **v)
   {int fail, rv;

    fail = tSC_firsttok();

    rv = fail ? 1 : 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
