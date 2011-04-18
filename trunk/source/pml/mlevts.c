/*
 * MLEVST.C - test eigen value/vector routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

/* number of test matrices to run */
#define NM 3

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_MATRIX - return a test matrix */

PM_matrix *test_matrix(int n)
   {int i, j, l;
    double f;
    PM_matrix *a;

    switch (n)

/* trivial diagonal matrix with eigenvalues 1, -1 */
       {case 0:
	     a = PM_create(2, 2);
	     PM_zero(a);
	     PM_element(a, 1, 1) = 1.0;
	     PM_element(a, 2, 2) = -1.0;
             break;

/* fairly trivial matrix with eigenvalues 1, 2, 4 */
        case 1:
	     a = PM_create(3, 3);
	     PM_zero(a);
	     PM_element(a, 1, 1) = 2.0;
	     PM_element(a, 2, 2) = 2.0;
	     PM_element(a, 3, 3) = 3.0;
	     PM_element(a, 2, 3) = sqrt(2.0);
	     PM_element(a, 3, 2) = sqrt(2.0);
	     break;

/* arbitrarily chosen dense matrix */
        case 2:
	     a = PM_create(5, 5);
	     PM_zero(a);
             for (i = 1; i <= 5; i++)
	         for (j = i; j <= 5; j++)
		     {f = 3.14*i + 2.71*j*j;
                      l = floor(f);
		      l %= 7;
		      f -= l;
		      PM_element(a, i, j) = f;
		      PM_element(a, j, i) = f;};
	     break;};

    printf("Matrix A:\n");
    PM_print(a);

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CHECK_MATRIX - verify that VAL = Tr(A) . B . A
 *              - return TRUE iff successful
 *              - if VAL is NULL return TRUE iff Tr(A) . B . A is diagonal
 */

static int check_matrix(PM_matrix *a, PM_matrix *b, PM_matrix *val)
   {int i, j, n, ok;
    double va, ve;
    PM_matrix *c, *d;

    c = PM_times(b, a);
    d = PM_times(PM_transpose(a), c);

    n  = a->nrow;
    ok = TRUE;
    for (i = 1; i <= n; i++)
        for (j = 1; j <= n; j++)
            {va = PM_element(d, i, j);
	     if (i == j)
	        {if (val != NULL)
		    {ve = PM_element(val, i, 1);
		     ok &= PM_CLOSETO_REL(va, ve);};}
	     else
	        ok &= PM_CLOSETO_ABS(va, 0.0);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TST1 - test eigenvalues */

int tst1(int n)
   {int rv;
    PM_matrix *a, *x;

    printf("Test #1\n");

    rv = TRUE;

    a = test_matrix(n);

/* find the eigenvalues */
    x = PM_eigenvalue(a);

    printf("Eigenvalues for A:\n");
    PM_print(x);

    PM_destroy(x);
    PM_destroy(a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TST2 - test eigenvectors */

int tst2(int n)
   {int rv;
    PM_matrix *a, *b;

    printf("Test #2\n");

    a = test_matrix(n);
    b = PM_create(a->nrow, a->ncol);
    PM_copy(b, a);

/* find the right eigenvectors */
    rv = PM_eigenvectors(a);

    if (rv == TRUE)
       {printf("Eigenvectors for A:\n");
	PM_print(a);};

/* verify */
    rv &= check_matrix(a, b, NULL);

    PM_destroy(a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TST3 - test both */

int tst3(int n)
   {int ok;
    PM_matrix *a, *b, *val;

    printf("Test #3\n");

    a = test_matrix(n);
    b = PM_create(a->nrow, a->ncol);
    PM_copy(b, a);

/* find eigenvalues and eigenvectors */
    val = PM_eigensys(a);

    printf("Eigenvalues for A:\n");
    PM_print(val);

    printf("Eigenvectors for A:\n");
    PM_print(a);

/* verify */
    ok = check_matrix(a, b, val);

    PM_destroy(val);
    PM_destroy(a);
    PM_destroy(b);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start here */

int main(int c, char **v)
   {int i, n, nt, rv, ok;
    int tf[] = { TRUE, TRUE, TRUE };
    int (*tst[])(int n) = { tst1, tst2, tst3 };

    nt = sizeof(tf)/sizeof(int);

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-h") == 0)
            {printf("Usage: mlevts [-h] [-1] [-2] [-3]\n");
	     printf("   h   this help message\n");
	     printf("   1   omit eigenvalue test\n");
	     printf("   2   omit eigenvector test\n");
	     printf("   3   omit eigensystem test\n");}
	 else if (strcmp(v[i], "-1") == 0)
	    tf[0] = FALSE;
	 else if (strcmp(v[i], "-2") == 0)
	    tf[1] = FALSE;
	 else if (strcmp(v[i], "-3") == 0)
	    tf[2] = FALSE;};

    rv = TRUE;
    for (n = 0; n < NM; n++)
        {printf("----------------------------------------------------------\n");
	 printf(" Test matrix #%d\n", n+1);
	 printf("\n");

	 ok = TRUE;
	 for (i = 0; i < nt; i++)
	     {if (tf[i] == TRUE)
		 ok &= tst[i](n);};

	 printf(" Result = %s\n", (ok == TRUE) ? "ok" : "ng");
	 printf("\n");

         rv &= ok;};

/* reverse the sense for exit status */
    rv = (rv != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
