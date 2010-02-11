/*
 * CSIZER.C - help characterize fortran types
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include <stdlib.h>
#include <stdio.h>

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

#define HAVE_LONG_LONG    1
#define HAVE_LONG_DOUBLE  1

#define MAX_SIZES 40

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

static int 
  counter,
  f90_sizes[MAX_SIZES];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* EXTERNAL_WORKER - find out how f90 sees C externals */

static void external_worker(char *s)
   {int i;
    FILE *out;

    out = fopen("from_c", "w");

    fprintf(out, "#define F90_ID(x, X)  %s\n", s);

/* write out sizes of C types */

    fprintf(out, "integer signed_char %ld\n", (long) sizeof(signed char));
    fprintf(out, "integer short       %ld\n", (long) sizeof(short));
/*   fprintf(out, "integer unsigned    %ld\n", (long) sizeof(unsigned)); */
    fprintf(out, "integer int         %ld\n", (long) sizeof(int));
    fprintf(out, "integer long        %ld\n", (long) sizeof(long));
#ifdef HAVE_LONG_LONG
    fprintf(out, "integer long_long   %ld\n", (long) sizeof(long long));
#endif

    fprintf(out, "real    float       %ld\n", (long) sizeof(float));
    fprintf(out, "real    double      %ld\n", (long) sizeof(double));
#ifdef HAVE_LONG_DOUBLE
    fprintf(out, "real    long_double %ld\n", (long) sizeof(long double));
#endif

    fprintf(out, "pointer pointer     %ld\n", (long) sizeof(void *));

/* write out sizes of fortran types */
    for (i = 0; i < counter; i++)
        fprintf(out, "%d\n", f90_sizes[i]);

    fclose(out);

    return;}

void f90external()  { external_worker("x"); }
void f90external_() { external_worker("x ## _"); }
void F90EXTERNAL()  { external_worker("X"); }

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void f90sizeofinit()  { counter = 0; }
void f90sizeofinit_() { counter = 0; }
void F90SIZEOFINIT()  { counter = 0; }

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* F90SIZEOF - compute the difference between two addresses
 *           - this is real useful when you're trying to find the byte size of
 *           - a f90 derived type in a portable manner
 */

int f90sizeof(char *addr1, char *addr2)
   {

    if (counter >= MAX_SIZES)
       {printf("counter too large\n");
	exit(1);};

    f90_sizes[counter++] = addr2 - addr1;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int f90sizeof_(char *addr1, char *addr2)
   {

    if (counter >= MAX_SIZES)
       {printf("counter too large\n");
	exit(1);};

    f90_sizes[counter++] = addr2 - addr1;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int F90SIZEOF(char *addr1, char *addr2)
   {

    if (counter >= MAX_SIZES)
       {printf("counter too large\n");
	exit(1);};

    f90_sizes[counter++] =  addr2 - addr1;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


