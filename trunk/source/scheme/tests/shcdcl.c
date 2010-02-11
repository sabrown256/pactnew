/*
 * SHCDCL.C - test variable declaration and defining derived types
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 *  #include "cpyright.h"
 */

/*
diagnose_c_parse(1);
diagnose_c_grammar(1);
#include "foo.scm"
 */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test definition of primitive variables */

int
 f, g = 3, h;

printf(nil, "Defined primitive variables: f = %s, g = %s, h = %s\n",
       f, g, h);

/*--------------------------------------------------------------------------*/

printf(nil, "STRUCT Tests\n");

/*--------------------------------------------------------------------------*/

/* test struct definition */

struct s_foo
  {int a;
   double b;};

printf(nil, "   Defined 'struct NAME {body}'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test typdef of struct */

typedef struct s_foo foo;

printf(nil, "   Defined 'typdef struct NAME NAME'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test #1 definition of structured variables */

struct s_foo
 x1s, x2s;

printf(nil, "   Defined 'struct NAME var1, var2'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test #2 definition of structured variables */

struct erk
  {int a;
   double b;} x1d, x2d;

printf(nil, "   Defined 'struct NAME {body} var1, var2'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test #3 definition of structured variables */

struct
  {int x;
   float y;} a1d, a2d;

printf(nil, "   Defined 'struct {body} var1, var2'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test #1 definition of typdef'd variables */

foo
 x1t, x2t;

printf(nil, "   Defined '<typedef-name> var1, var2' (#1 struct typedef)\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test #2 definition of typdef'd variables */

typedef struct s_erk
  {int a;
   double b;} erk;

printf(nil, "   Defined 'typedef struct NAME {body} var'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test #3 definition of typdef'd variables */

erk
 e1;

printf(nil, "   Defined '<typedef-name>' var' (#2 struct typedef)\n");

/*--------------------------------------------------------------------------*/

printf(nil, "UNION Tests\n");

/*--------------------------------------------------------------------------*/

/* test definition of union */

union u_bar
  {int a;
   double b;};

printf(nil, "   Defined 'union NAME {body}'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test typdef of union */

typedef union u_bar bar;

printf(nil, "   Defined 'typdef union NAME NAME'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test #1 definition of union variables */

union u_bar
 yu;

printf(nil, "   Defined 'union NAME var'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test #1 definition of typdef'd union variable */

bar
 yt;

printf(nil, "   Defined '<typedef-name> var1, var2' (#1 union typedef)\n");

/*--------------------------------------------------------------------------*/

printf(nil, "ENUM Tests\n");

/*--------------------------------------------------------------------------*/

/* test definition of enum */

enum e_winter {december = 100, january, february};

printf(nil, "   Defined 'enum NAME {body}'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test typdef of enum */

typedef enum e_winter winter;

printf(nil, "   Defined 'typdef enum NAME NAME'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test #1 definition of enum variables */

enum e_winter
 ze;

printf(nil, "   Defined 'enum NAME var'\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test #1 definition of typdef'd enum variable */

winter
 zt;

printf(nil, "   Defined '<typedef-name> var1, var2' (#1 enum typedef)\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

quit();
