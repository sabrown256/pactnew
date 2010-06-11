/*
 * MATS.C - test regular expression comparisons for PCO
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * include "cpyright.h"
 *
 */

#include "common.h"
#include "libpsh.c"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, n, ok;
    char *s, *p;
    static char *str[] = { "fooabcade", "fooabcade",
			   "3.2.1", "4.2.1", "5.2.1",
			   "joe", "john" };
    static char *pat[] = { "*ae", "*ac",
			   "4.*", "4.*", "4.*",
			   "john", "joe" };

    n = sizeof(str)/sizeof(char *);
    for (i = 0; i < n; i++)
        {s = str[i];
	 p = pat[i];

	 ok = match(s, p);
	 if (ok < 0)
	    printf("-> %s < %s\n", s, p);
	 else if (ok == 0)
	    printf("-> %s = %s\n", s, p);
	 else if (ok > 0)
	    printf("-> %s > %s\n", s, p);};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

