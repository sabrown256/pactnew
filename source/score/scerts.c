/*
 * SCERTS.C - test error handling routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - simplest, single level trap */

int test_1(int err)
   {int ok;

    ok = SC_ERR_TRAP();
    if (ok != 0)
       return(FALSE);

    if (err == TRUE)
       SC_error(-1, "TEST_1 caught");

    SC_ERR_UNTRAP();
    
    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - two level trap
 *        - escape from inner level
 */

int test_2(int err)
   {int ok;

/* enter first level */
    ok = SC_ERR_TRAP();
    if (ok != 0)
       return(FALSE);

/* enter second level */
    ok = SC_ERR_TRAP();
    if (ok != 0)
       return(FALSE);

/* trap to second level */
    if (err == TRUE)
       SC_error(-1, "TEST_2 caught");

/* leave second level */
    SC_ERR_UNTRAP();
    
/* leave first level */
    SC_ERR_UNTRAP();
    
    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - two level trap
 *        - escape from outer level
 */

int test_3(int err)
   {int ok;

/* enter first level */
    ok = SC_ERR_TRAP();
    if (ok != 0)
       return(FALSE);

/* enter second level */
    ok = SC_ERR_TRAP();
    if (ok != 0)
       return(FALSE);

/* leave second level */
    SC_ERR_UNTRAP();
    
/* trap to first level */
    if (err == TRUE)
       SC_error(-1, "TEST_3 caught");

/* leave first level */
    SC_ERR_UNTRAP();
    
    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_IT - run a test with and without error */

int run_it(int d, int (*tst)(int err))
   {int pe, me, rv;

    me = (*tst)(FALSE);
    pe = (*tst)(TRUE);
    
    if ((me == TRUE) && (pe == FALSE))
       {io_printf(STDOUT, "\t\t\t      # %d     ok\n", d);
	rv = TRUE;}
    else
       {io_printf(STDOUT, "\t\t\t      # %d     failed\n", d);
	rv = FALSE;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int ok, rv;

    ok = TRUE;

    ok &= run_it(1, test_1);
    ok &= run_it(2, test_2);
    ok &= run_it(3, test_3);
    
    if (ok == TRUE)
       rv = 0;
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
