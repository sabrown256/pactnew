/*
 * TPCCH.C - small program to report process info
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "score_int.h"
#include "scope_term.h"
#include "scope_proc.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int rv, pid, ppid, pgid, ptid, ib;

    rv = 0;

    pid  = getpid();
    ppid = getppid();
    pgid = getpgrp();
    ptid = tcgetpgrp(0);
    ib   = SC_is_background_process(pid);

    printf("\n");
    printf("  ppid(%d)", ppid);
    printf("  ptid(%d)", ptid);
    printf("  pgid(%d)", pgid);
    printf("  pid(%d)", pid);
    printf("  background(%d)", ib);
    printf("\n");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
