/*
 * MLFPETS.C - test SIGFPE routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

static JMP_BUF
 cpu;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HANDLER - handle SIGFPE */

static void handler(int sig)
   {

    PM_clear_fpu();
    PM_enable_fpe(TRUE, handler);

    LONGJMP(cpu, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, rv, verbose;
    volatile double a, x, y;

    verbose = FALSE;

    PM_enable_fpe(TRUE, handler);

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("\n");
	     printf("Usage: mlfptes [-h] [-v]\n");
	     printf("   h   this help message\n");
	     printf("   v   report FPE traps\n");
	     printf("\n");
	     return(1);}
	 else if (strcmp(v[i], "-v") == 0)
            verbose = TRUE;};

    if (verbose == TRUE)
       PM_fpe_traps(TRUE);

    rv = 0;
    x  = 1.0;
    y  = 0.0;

    PRINT(stdout, "   Trapping exceptions ... \n");

/* try divide by zero - should be trapped */
    if (SETJMP(cpu) == 0)
       {a = x/y;
	PRINT(stdout, "      divide by zero ... none\n");}
    else
       {rv |= 1;
	PRINT(stdout, "      divide by zero ... ok\n");};

/* try overflow - should be trapped */
    if (SETJMP(cpu) == 0)
       {for (i = 0, a = x; i < 100000; i++)
	    a *= 2.0;
#ifdef IBM_BGP
        rv |= 2;
	PRINT(stdout, "      overflow ......... ok\n");
#else
	PRINT(stdout, "      overflow ......... none\n");
#endif
        }
    else
       {rv |= 2;
	PRINT(stdout, "      overflow ......... ok\n");};

/* try underflow - should not be trapped */
    if (SETJMP(cpu) == 0)
       {for (i = 0, a = x; i < 100000; i++)
	    a *= 0.5;
	PRINT(stdout, "      underflow ........ none\n");}
    else
       {rv |= 4;
	PRINT(stdout, "      underflow ........ ok\n");};

    if (verbose == TRUE)
       PM_fpu_status(TRUE);

/* change correct result into correct exit status
 * we do not expect to get underflow FPE
 */
    rv = (rv != 3);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

