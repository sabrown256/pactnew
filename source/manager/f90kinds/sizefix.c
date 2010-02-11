/*
 * SIZEFIX.C - compute and report the appropriate C type for FIXNUM
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include <stdio.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int n;

    n = atol(v[1]);
    if (n == sizeof(long))
       printf("#define FIXNUM long\n");

    else if (n == sizeof(int))
       printf("#define FIXNUM int\n");

    else if (n == sizeof(short))
       printf("#define FIXNUM short\n");

/* default */
    else
       printf("#define FIXNUM int\n");

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
