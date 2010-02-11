/*
 * INTERACT.C - interactive test for PACT
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Function    : tSC_wall_clock_time
 * Usage       : Interactive tester for pact/score/scctl.c function
 *               SC_wall_clock_time
 *
 * Description : Displays time and instructions for user response.
 *               Prints name of function if test fails.
 *
 * Returns     : 1|0 for PASS|FAIL.
 *
 */

int tSC_wall_clock_time(void)
   {int cfail = 0;
    int time = 30;
    char reply[MAXLINE];                  /* MAXLINE defined in scstd.h */
    int i;
    int t0, t1;

    io_printf(STDOUT, "\n\n***** This is the test for SC_wall_clock_time ***** \n");

    t0 = SC_wall_clock_time();
    io_printf(STDOUT, "t0 = %d \n", t0);
    SC_sleep(1000*time);
    t1 = SC_wall_clock_time();
    io_printf(STDOUT, "t1 = %d \n", t1);
    io_printf(STDOUT, "Does %d = %d? \n",(t1-t0),time);

    for (i=0; i<time; i++)
        io_printf(STDOUT, "%d\n", SC_wall_clock_time());

    io_printf(STDOUT, "The starting time is: %11.3e \n", SC_wall_clock_time());
    io_printf(STDOUT, "(Taking %d second nap...zzz...) \n", time);
    SC_sleep(1000*time);
    io_printf(STDOUT, "The wake-up  time is: %11.3e \n", SC_wall_clock_time());
    io_printf(STDOUT, "\nIs that close enough?");
    io_printf(STDOUT, "TICKS_SECOND = %d", TICKS_SECOND);
    io_printf(STDOUT, "\nPlease reply y|n to set PASS|FAIL flag: ");
    if (io_gets(reply, MAXLINE, stdin) == NULL)
       io_printf(STDOUT, "\n Error reading response \n");

    else
       if (reply[0] != 'y')
          cfail++;

    return(cfail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*
 * Function    : tSC_banner
 * Usage       : Interactive tester for pact/score/scctl.c function
 *               SC_banner 
 *
 * Description : Displays banner and instructions for user response.
 *               Prints name of function if test fails.
 *
 * Returns     : 1|0 for PASS|FAIL.
 *
 */

int tSC_banner(void)
   {int bfail = 0;
    char reply[MAXLINE], bstr[MAXLINE];

    strcpy(bstr, "This message is brought to you by SC_banner.");

    io_printf(STDOUT, "\n\n***** This is the test for SC_banner ***** \n");
    io_printf(STDOUT, "You should see the message:\n %s \n", bstr);

    SC_banner(bstr);

    io_printf(STDOUT, "\nDo the messages match?");
    io_printf(STDOUT, "\nPlease reply y|n to set PASS|FAIL flag: ");
    if (io_gets(reply, MAXLINE, stdin) == NULL)
       io_printf(STDOUT, "\n Error reading response \n");

    else
       {if (reply[0] != 'y')
	   bfail++;};

    if (bfail != 0)
       io_printf(STDOUT, "\nFAIL SC_banner \n");

    return(bfail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Function    : tSC_pause 
 * Usage       : Interactive tester for pact/score/scctl.c function
 *               SC_pause
 *
 * Description : Causes pause for user until <CR> received.
 *               Prints name of function if test fails.
 *
 * Returns     : 1|0 for PASS|FAIL.
 *
 */

int tSC_pause(void)
   {int pfail = 0;
    char reply[MAXLINE];

    io_printf(STDOUT, "\n\n***** This is the test for SC_pause ***** \n");
    io_printf(STDOUT, "You should expect a pause until you press <CR>.\n");

    SC_pause();

    io_printf(STDOUT, "Was there a pause?\n");
    io_printf(STDOUT, "Please reply y|n to set PASS|FAIL flag: ");
    if (io_gets(reply, MAXLINE, stdin) == NULL)
       io_printf(STDOUT, "\n Error reading response \n");

    else
       {if (reply[0] != 'y')
	   pfail++;};

    if (pfail != 0)
       io_printf(STDOUT, "\nFAIL SC_pause \n");

    return(pfail);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* File        : interact.c 
 * Programmer  : Carolyn Sharp
 * Machines    : sun3, phoenix, Cray 2, RS6000
 *
 * Description : Interactive tester for pact - tests those routines
 *               which cannot be handled automatically. 
 * Notes       : Use Ctrl-C at any time to quit.
 * Caution     : Redirection of output -> terminal appears hung  but it's
 *               just waiting for your terminal input.
 */

int main(int c, char **v)
   {int ifail, rv;

    ifail = 0;
  
    ifail += tSC_wall_clock_time();
    ifail += tSC_pause();
    ifail += tSC_banner();

    if (ifail == 0)
       io_printf(STDOUT, "\nPASS Interactive Tester \n");
    else
       io_printf(stderr, "\nFAIL = %d in Interactive Tester \n", ifail);

    rv = ifail ? 1 : 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
