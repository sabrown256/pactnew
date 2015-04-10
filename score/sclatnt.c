/*
 * SCLATNT.C - compute the latencies for various file systems
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
   {double tmp, hm, net, prc;

    SC_get_latencies(&tmp, &hm, &net, &prc);

    printf("/tmp    %10.3e      %6.2f\n", tmp, 1.0);
    printf("Home    %10.3e      %6.2f\n", hm, hm/tmp);
    printf("Net     %10.3e      %6.2f\n", net, net/tmp);
    printf("Proc    %10.3e      %6.2f\n", prc, prc/tmp);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
