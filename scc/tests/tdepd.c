/*
 * TDEPD.C - dependent type test
 *         - tests struct declaration before definition
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */


struct s1;

typedef struct s1 {int m1;} t1[1];

typedef struct s1 t2[1];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void f1(void)
   {t2 a;

    a[0].m1 = 2;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
