/*
 * TDEPC.C - dependent type test
 *         - tests types in arg lists
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

typedef unsigned int t1;
typedef long int t2;

struct s1 {int m1;};

typedef struct s_s2 s2;

struct s_s2
   {char *m1;
    int m2;};

extern char *d1(struct s1 *x);
extern char *d2(t2 *x, char *y);
extern t1 d3(char *x);

extern s2
 v1;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int f1(char *s)
   {int nc;

    nc = d3(s);
    nc = v1.m2;

    return(nc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
