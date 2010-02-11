/*
 * TTYPDEF.C - typedef parse test
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

typedef int t1;
typedef int *t2;
typedef int (*t3)(void);
typedef int (*t4)(int a1, int a2);
typedef int (*t5)(int, int);
typedef int *(*t6)(void);

struct s_info
   {int m1;
    double m2;
    short m3;};

typedef struct s_info t7;
typedef struct {int m1; int m2;} t8;

