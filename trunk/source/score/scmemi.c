/*
 * SCMEMI.C - config/init memory management functions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#include <stdio.h>
#include <stdint.h>

#define N_DOUBLES_MD 3

#include "scope_thread.h"
#include "scope_mem.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

int main(int c, char **v)
   {int na, nb, nd, ni;

    nb = sizeof(mem_descriptor);
    na = SC_MEM_ALIGN_SIZE;
    nd = sizeof(double);

/* compute how many SC_MEM_ALIGN_SIZE units cover the mem_descriptor */
    ni = (nb + na - 1)/na;

/* convert this to doubles */
    ni *= (na/nd);

    printf("#define N_DOUBLES_MD %d\n", ni);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

