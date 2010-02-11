/*
 * SHCPPTS.C - test CPP syntax as subset of C syntax mode
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 *  #include "cpyright.h"
 */

#include "shcppts.h"

/* statements to catch errors detecting the completion of #define */

int i;

printf(nil, "BAZ should be 3: %d\n", BAZ);

for (i = 0; i < BAZ; i++)
    printf(nil, "--> %d\n", i);

printf(nil, "\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check #ifdef/#endif */

#ifdef FOO
   printf(nil, "Passed #ifdef/#endif test #1\n");
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check #ifndef/#endif */

#ifndef BAR
   printf(nil, "Passed #ifndef/#endif test #1\n");
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check #if/#endif */

#if 1
    printf(nil, "Passed #if/#endif test #1\n");
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check #ifdef/#else/#endif */

#ifdef FOO
   printf(nil, "Passed #ifdef/#else/#endif test #1\n");
#else
   printf(nil, "Failed #ifdef/#else/#endif test #1\n");
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check #ifndef/#else/#endif */

#ifndef BAR
   printf(nil, "Passed #ifndef/#else/#endif test #1\n");
#else
   printf(nil, "Failed #ifndef/#else/#endif test #1\n");
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check #if/#else/#endif */

#if 0
    printf(nil, "Failed #if/#else/#endif test #1\n");
#else
    printf(nil, "Passed #if/#else/#endif test #1\n");
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check #if/#else/#endif */

#if (BAZ == 3)
    printf(nil, "Passed #if/#else/#endif test #2\n");
#else
    printf(nil, "Failed #if/#else/#endif test #2\n");
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check imbedded #ifdef/#else/#endif */

#ifdef FOO
# ifdef BAR
   printf(nil, "Failed imbedded #ifdef/#else/#endif test #1 (1)\n");
# else
   printf(nil, "Passed imbedded #ifdef/#else/#endif test #1\n");
# endif
#else
   printf(nil, "Failed imbedded #ifdef/#else/#endif test #1 (2)\n");
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* test procedural macro */

   printf(nil, "-----------------------------\n");

   int a;

   a = 0;
   FOO1(a);
   printf(nil, "FOO1(a) should set a to 3: %s\n", a);

   a = 0;
   FOO2(a);
   printf(nil, "FOO2(a) should set a to 3: %s\n", a);

   a = 0;
   BLAT1(a);
   printf(nil, "BLAT1(a) should set a to 3: %s\n", a);

   a = 0;
   BLAT2(a, 4);
   printf(nil, "BLAT2(a, 4) should set a to 4: %s\n", a);

   printf(nil, "-----------------------------\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check variable concatenation - ## */

   double c, d;

#define cat(x, y) x ## y
#define FOO cat(c, d)

   printf(nil, "cat(a, b) should expand to ab: %s\n", cat(a, b));
   printf(nil, "FOO should expand to cd: %s\n", FOO);

   printf(nil, "-----------------------------\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check variable conversion - #
 * NOTE: due to extreme complexity this syntax is fairly limited
 * compared to what the ANSI C usage is
 */

#define stringify(x)  #x

   printf(nil, "stringify(a) to 'a': %s\n", stringify(a));

   printf(nil, "-----------------------------\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* check #undef on macro FOO defined above */

#undef FOO
#ifdef FOO
   printf(nil, "Failed FOO still defined\n");
#else
   printf(nil, "Passed FOO is now undefined\n");
#endif

   printf(nil, "-----------------------------\n");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

   quit();
