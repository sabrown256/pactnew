/*
 * SHCTST.C - test of C syntax mode for SCHEME
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 *  #include "cpyright.h"
 */

/* test some global declarations */

int
 x = 3,
 y,
 z = 2;

double
 a,
 b = 4.0,
 c;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_RETURN - test return statements in various configurations */

int test_return(int n)
   {int i;

    printf(NULL, "\nRETURN Test\n");
    if (n < 0)
       {printf(NULL, "Negative branch - %d\n", n);
	return(-1);}

    else if (n > 0)
       {printf(NULL, "Positive branch - %d\n", n);
	return(1);}

    else
       {printf(NULL, "Zero branch - %d\n", n);
	return(0);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_SWITCH - test switch statements in various configurations */

int test_switch(int n)
   {int i;

/* test #1 switch(exp) */
    printf(NULL, "\nSWITCH Test\n");
    switch (n)
       {case 0 :
	     i = n;
             printf(NULL, "   case 0 : %s\n", i);
	case 3 :
	     i = 2*n;
             printf(NULL, "   case 3 : %s\n", i);
	     break;
	default :
	     i = 3*n;
             printf(NULL, "   default : %s\n", i);};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_WHILE - test while loops in various configurations */

int test_while(int n)
   {int i;
    int j;
    double x;
    double y;

/* test #1 while(exp) */
    printf(NULL, "\nWHILE Test #1 - while (exp) statement\n");
    x = 0.0;
    i = 0;
    while (i < n)
       {x += 1.1*i;
	printf(NULL, "   %d %g\n", i, x);
        i++;};

/* test #2 do {} while (exp) */
    printf(NULL, "\nWHILE Test #2 - do statement while (exp)\n");
    x = 0.0;
    i = 0;
    do {x += 2.2*i;
	printf(NULL, "   %d %g\n", i, x);
        i++;}
    while (i < n);

/* test #3 compound for clauses */
    printf(NULL, "\nWHILE Test #3 - with compound exp\n");
    x = 0.0;
    y = 0.0;
    i = 0;
    j = 0;
    while ((i < n) && (j > -n))
       {x = i - j;
	y += sin(x, n);
	printf(NULL, "   %d %d %g\n", i, j, y);
        i++;
	j--;};

/* test #4 while with break */
    printf(NULL, "\nWHILE Test #4 - with break\n");
    i = 0;
    while (1)
       {i++;
	printf(NULL, "   %d\n", i);
        if (i > n)
	   break;};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_FOR - test for loops in various configurations */

int test_for(int n)
   {int i;
    int j;
    double x;
    double y;

/* test #1 for(exp; exp; exp) */
    printf(NULL, "\nFOR Test #1 - for (exp; exp; exp) statement\n");
    x = 0.0;
    for (i = 0; i < n; i++)
        {x += 1.1*i;
         printf(NULL, "   %d %g\n", i, x);};

/* test #2 for(; exp; exp) */
    printf(NULL, "\nFOR Test #2 - for ( ; exp; exp) statement\n");
    x = 0.0;
    for (; i < 2*n; i++)
        {x += 2.2*i;
         printf(NULL, "   %d %g\n", i, x);};

/* test #3 for(exp; ; exp) */
    printf(NULL, "\nFOR Test #3 - for (exp; ; exp) statement\n");
    x = 0.0;
    for (i = 0; ; i++)
        {x += 3.3*i;
         printf(NULL, "   %d %g\n", i, x);
         if (i > n)
	    break;};

/* test #4 for(exp; exp;) */
    printf(NULL, "\nFOR Test #4 - for (exp; exp; ) statement\n");
    x = 0.0;
    for (i = 0; i < n;)
        {x += 4.4*i;
         printf(NULL, "   %d %g\n", i, x);
         i++;};

/* test #5 for(exp; ;) */
    printf(NULL, "\nFOR Test #5 - for (exp; ; ) statement\n");
    x = 0.0;
    for (i = 0; ; )
        {x += 5.5*i;
         printf(NULL, "   %d %g\n", i, x);
         i++;
         if (i > n)
	    break;};

/* test #6 for(; exp;) */
    printf(NULL, "\nFOR Test #6 - for ( ; exp; ) statement\n");
    x = 0.0;
    for (; i < 2*n;)
        {x += 6.6*i;
         printf(NULL, "   %d %g\n", i, x);
         i++;};

/* test #7 for( ; ; exp) */
    printf(NULL, "\nFOR Test #7 - for ( ; ; exp) statement\n");
    x = 0.0;
    i = 0;
    for (; ; i++)
        {x += 7.7*i;
         printf(NULL, "   %d %g\n", i, x);
         if (i > n)
	    break;};

/* test #8 compound for clauses */
    printf(NULL, "\nFOR Test #8 - with compound clauses\n");
    y = 0.0;
    for (i = 0, j = -1; ((i < n) && (j > -n)); i++, j--)
        {x = i - j;
	 y += sin(n, x);
         printf(NULL, "   %d %d %g\n", i, j, y);};

/* test #9 for with continue */
    printf(NULL, "\nFOR Test #9 - with continue\n");
    x = 0.0;
    for (i = 0; i < n; i++)
        {x += 9.9*i;

         if (i == 1)
            continue;

         printf(NULL, "   %d %g\n", i, x);};

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_INC - test the pre and post fix increment and decrement */

int test_inc(int n)
   {

    printf(NULL, "\nIncrement Test\n");
    printf(NULL, "   %s\n", n);
    printf(NULL, "   %s\n", n++);
    printf(NULL, "   %s\n", ++n);

    printf(NULL, "\nDeccrement Test\n");
    printf(NULL, "   %s\n", n);
    printf(NULL, "   %s\n", --n);
    printf(NULL, "   %s\n", n--);

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_SIZEOF - test the sizeof operator */

void test_sizeof()
  {

   printf(NULL, "\nSIZEOF Test\n");

   printf(NULL, "   int    %d\n", sizeof(int));
   printf(NULL, "   short  %d\n", sizeof(short));
   printf(NULL, "   float  %d\n", sizeof(float));
   printf(NULL, "   double %d\n", sizeof(double));

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_CAST - test the supported casts */

void test_cast()
   {

    printf(NULL, "\nCAST Test\n");

    printf(NULL, "   int 3 -> double %5.2f\n", (double) 3);
    printf(NULL, "   double 3.1 -> int %d\n", (int) 3.1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_ARRAY - test some array capability */

int test_array()
   {int i, a[4];

    printf(NULL, "\nARRAY Test\n");

    for (i = 0; i < 4; i++)
        {a[i] = i;};

    i = a[0] + a[1] + a[2] + a[3];

    printf(NULL, "   sum = %d\n", i);

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test the ability to have expressions of the form:
 *     if (<expr>) <statement>
 * interleaved with external declarations
 */

printf(NULL, "Interleaved if and declaration test\n");

char
 *a = "foo";

printf(NULL, "-> before a = %s\n", a);

if (1)
   a = "bar";

int
 b = 1;

if (b == 1)
   {printf(NULL, "-> intermediate a = %s\n", a);
    a = "baz";};

double
 c = 2.0;

printf(NULL, "-> after a = %s\n", a);
printf(NULL, "-> after b = %d\n", b);
printf(NULL, "-> after c = %3.1f\n", c);

printf(NULL, "done\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* describe(test_return); */
test_return(-2);
test_return(15);
test_return( 0);

/* describe(test_switch); */
test_switch(0);
test_switch(3);
test_switch(5);

/* describe(test_while); */
test_while(5);

/* describe(test_for); */
test_for(5);

/* describe(test_inc); */
test_inc(3);

/* describe(test_sizeof); */
test_sizeof();

/* describe(test_cast); */
test_cast();

/* describe(test_array); */
test_array();

quit();

