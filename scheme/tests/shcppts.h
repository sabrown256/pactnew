/*
 * SHCPPTS.H - part of CPP syntax test for C syntax mode
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 *  #include "cpyright.h"
 */

/* test defining constants */

# define FOO
#define BAZ 3

/* NOTE: the procedural macros parse correctly but do not
 *       work correctly because of implementation defficiencies
 */

#define FOO1(x)   x = 3

#define FOO2(x)   FOO1(x)

#define BLAT1(x)  {x = 3;}

#define BLAT2(x, y)   \
   {int z;            \
    z = y;            \
    x = z;}

int
 x;
