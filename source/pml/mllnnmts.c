/*
 * MLLNNMTS.C - test the ln norm function
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a sample program */

int main(int c, char **v)
   {double *x, *y, result, sign;
    int i;
    int npoints = 10;

    x = CMAKE_N(double, npoints);
    y = CMAKE_N(double, npoints);

    for (i = 0; i < npoints; i++)
        {x[i] = (double) i;
         y[i] = (double) i;};

/* L1 norm */
    result = PM_lnnorm(y, x, npoints, 1);
    PRINT(STDOUT, "L1 norm of        y = index(0:%d): %12.6e\n",
          npoints-1, result);

/* L2 norm */
    result = PM_lnnorm(y, x, npoints, 2);
    PRINT(STDOUT, "L2 norm of        y = index(0:%d): %12.6e\n",
          npoints-1, result);

/* Linfinity norm */
    result = PM_lnnorm(y, x, npoints, -1);
    PRINT(STDOUT, "Linfinity norm of y = index(0:%d): %12.6e\n",
          npoints-1, result);

/* alternating signs */
    sign = 1.0;
    for (i = 0; i < npoints; i++, sign *= -1.0)
        {y[i] = (double) i * sign;
         x[i] = (double) i;}

/* L1 norm */
    result = PM_lnnorm(y, x, npoints, 1);
    PRINT(STDOUT, "L1 norm of        y = index(0:%d) alternating sign: %12.6e\n",
          npoints-1, result);

/* L2 norm */
    result = PM_lnnorm(y, x, npoints, 2);
    PRINT(STDOUT, "L2 norm of        y = index(0:%d) alternating sign: %12.6e\n",
          npoints-1, result);

/* Linfinity norm */
    result = PM_lnnorm(y, x, npoints, -1);
    PRINT(STDOUT, "Linfinity norm of y = index(0:%d) alternating sign: %12.6e\n",
          npoints-1, result);

    CFREE(x);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

