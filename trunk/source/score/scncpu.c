/*
 * SCNCPU.C - compute and return the number of processors
 *          - available for threaded operation
 *          - on the current node
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int np;

    np = SC_get_ncpu();

    printf("%d\n", np);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
