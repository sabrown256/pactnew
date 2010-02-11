/*
 * TDEPA.C - dependent type test
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

typedef int t1;

typedef struct s_t2 t2;
struct s_t2 {int m1;};

struct s_t3 {double m1;};

char v1 = 'a';
char v2 = '\n';
char v3 = '\002';

t1 v4;

t2 v5 = { 1 };

int main(int c, char *v)
   {struct s_t3 lv1;
    lv1.m1 = c;
    v4     = v5.m1;
    return(v4);}

