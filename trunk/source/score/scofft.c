/*
 * SCOFFT.C - test large file types
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - test large file types */

int main(int c, char **v)
   {int rv;

    io_printf(stdout, "\n");
#ifdef _LARGEFILE_SOURCE
    io_printf(stdout, "Using large file conditionalizations\n");
#else
    io_printf(stdout, "No large file conditionalizations\n");
#endif
    io_printf(stdout, "\n");

    io_printf(stdout, "Data type byte sizes:\n");
    io_printf(stdout, "   int      = %d\n", sizeof(int));
    io_printf(stdout, "   long     = %d\n", sizeof(long));
    io_printf(stdout, "   longlong = %d\n", sizeof(long long));
    io_printf(stdout, "   size_t   = %d\n", sizeof(size_t));
    io_printf(stdout, "   off_t    = %d\n", sizeof(off_t));
    io_printf(stdout, "   int64_t  = %d\n", sizeof(int64_t));
    io_printf(stdout, "\n");

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
