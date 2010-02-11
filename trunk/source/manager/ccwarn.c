
#include <stdlib.h>
#include <stdio.h>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char *fa(unsigned char *a)
   {char *b;

    b = a;

    return(b);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int rv;
    double a, b;
    char *ss;
    unsigned char *us;
    const char *cs;

    rv = 0;

    a = 1.0;

/* warn about statement with no effect */
    b == a;

    cs = "abcd";

/* warn about const qualifier */
    ss = cs;

/* warn about signed/unsigned mismatch */
    us = fa(ss);

/* warn about signed/unsigned mismatch */
    ss = us;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

