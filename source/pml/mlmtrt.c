/*
 * MLTEST.C - a main routine to test the matrix package
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

double
 a1[5][3] = { {0, 0, 1}, {1, 0, 1}, {0, 1, 1}, {.5, .5, 1}, {.7, .9, 1} },
 b1[5][1] = { {.5}, {1.4}, {.93589}, {1.16795}, {1.52230} };

/* double tstd[9] = {1, 4, 16,   1, 2, 4,   1, 1, 1}; */
double
 tstd[9] = {0.30, 0.59, 0.11,   0.60, -0.28, -0.32,   0.21, -0.52, 0.31};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* OPER_TEST - test basic matrix operations */

static int oper_test()
   {int nrow = 5, ncol = 3;
    PM_matrix *m, *t, *b, *a, *c;

    m = PM_create(nrow, ncol);
    SFREE(m->array);
    m->array = (double *) a1;

    b = PM_create(nrow, 1);
    SFREE(b->array);
    b->array = (double *) b1;
        
    PRINT(STDOUT, "\nMatrix M\n");
    PM_print(m);

    PRINT(STDOUT, "\nMatrix B\n");
    PM_print(b);

    PRINT(STDOUT, "\nMatrix tr(M)\n");
    PM_print(t = PM_transpose(m));

    PRINT(STDOUT, "\nA = tr(M).M (piecewise)\n");
    PM_print(a = PM_times(t, m));
    PM_destroy(t);

    PRINT(STDOUT, "\nT = tr(M).M (nested)\n");
    PM_print(c = PM_times(t = PM_transpose(m), m));
    PM_destroy(c);

    PRINT(STDOUT, "\nC = tr(M).B\n");
    PM_print(c = PM_times(t, b));
    PM_destroy(t);

    PRINT(STDOUT, "\nSolve A.X = C\n");
    PM_print(PM_solve(a, c));
    PM_destroy(a);
    PM_destroy(c);

    PRINT(STDOUT, "\nTest LU decomposition\n");
    a = PM_create(3, 3);
    SFREE(a->array);
    a->array = (double *) tstd;

    PRINT(STDOUT, "\nMatrix A\n");
    PM_print(a);
    PRINT(STDOUT, "\nc = (A)^-1\n");
    PM_print(c = PM_inverse(a));
    PRINT(STDOUT, "\nc.A\n");
    PM_print(t = PM_times(c, a));
    PM_destroy(t);
    PM_destroy(c);

    SFREE(a);
    SFREE(b);
    SFREE(m);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DET_TEST - test the determinant function */

static int det_test()
   {int i, n;
    double det;
    PM_matrix *m;

    n = 3;
    m = PM_create(n, n);
    n = n*n;

    for (i = 0; i < n; i++)
        m->array[i] = 0.0;
    PM_element(m, 1, 1) = 1.0;
    PM_element(m, 2, 3) = 1.0;
    PM_element(m, 3, 2) = 1.0;

    det = PM_determinant(m);

    PM_destroy(m);

    return(det == -1.0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - a sample program */

int main()
   {int err;
    long bytaa, bytfa, bytab, bytfb;

    SC_gs.mm_debug = TRUE;
    SC_zero_space_n(0, -2);

    SC_mem_stats(&bytab, &bytfb, NULL, NULL);

    err  = TRUE;
    err &= oper_test();
    if (!err)
       PRINT(STDOUT, "Matrix operation test failed\n");

    err &= det_test();
    if (!err)
       PRINT(STDOUT, "Matrix determinant test failed\n");

    SC_mem_stats(&bytaa, &bytfa, NULL, NULL);

    bytaa -= bytab;
    bytfa -= bytfb;

    if ((bytaa - bytfa) != 0)
       {PRINT(STDOUT, "\n\n\t\t   Allocated      Freed       Diff\n");
	PRINT(STDOUT, "\t\t   %9d  %9d  %9d\n\n",
	      bytaa, bytfa, bytaa - bytfa);};

    return(!err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

