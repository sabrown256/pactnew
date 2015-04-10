/*
 * TDEPB.C - dependent type test
 *         - tests types in struct members
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

typedef struct s_s1 s1;
typedef struct s_s2 s2;

struct s_s1
   {int m1;
    double m2;};

struct s_s2
   {int m1;
    double m2;
    s1 m3;};

s2
 is2;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int rv;

    is2.m3.m1 = 1;
    is2.m3.m2 = 2.0;

    rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


