/*
 * MLSVDT.C - test SVD solver
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

#define KL       25
#define N_TEST   3

typedef int (*PFTFnc)(int nx);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CATCH_FPE - */

void catch_fpe(int sig)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REPORT_DIFF - handle reporting the difference */

static int report_diff(PM_matrix *diff)
   {int ok;

    if (PM_is_zero(diff) == TRUE)
       {printf("yes\n");
	ok = TRUE;}
    else
       {printf("no\n");
	ok = FALSE;};

    PM_print(diff);
    PM_destroy(diff);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONDITION_NUMBER - report the condition number of W */

static double condition_number(double *w, int n)
   {double mn, mx, rv;

    PM_maxmin(w, &mn, &mx, n);

    if (mn != 0.0)
       rv = mx/mn;
    else
       rv = HUGE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SVD_KERNEL - solve and verify
 *            - return TRUE iff successful
 */

static int svd_kernel(PM_matrix *ma)
   {int i, j, k, m, n, nn, mx, ok;
    double wc;
    double *a, *u, *w, *v;
    PM_matrix *mu, *mw, *mv;
    PM_matrix *one, *diff;

    m = ma->nrow;
    n = ma->ncol;
    a = ma->array;

    mx = max(m, n);

    mu = PM_create(mx, n);
    mw = PM_create(n, n);
    mv = PM_create(n, n);

    u = mu->array;
    w = mw->array;
    v = mv->array;

    for (j = 0; j < m; j++)
        {for (i = 0; i < n; i++)
	     {k    = i*n + j;
	      u[k] = a[k];};};

/* pad out U with zero rows if M < N */
    for (; j < n; j++)
        {for (i = 0; i < n; i++)
	     {k    = i*n + j;
	      u[k] = 0.0;};};

    nn = n*n;
    for (k = 0; k < nn; k++)
        w[k] = 0.0;

    ok = PM_svd_decompose(u, mx, n, w, v);

    printf("Condition number of W = %10.3e\n\n", condition_number(w, n));

    printf("U (orthogonal)\n");
    PM_print(mu);

/* convert vector to diagonal matrix */
    for (i = 1; i < n; i++)
        {k    = i*n + i;
	 wc   = w[i];
	 w[k] = wc;
	 w[i] = 0.0;};

    one = PM_ident(PM_zero(PM_create(n, n)));

    printf("W (diagonal)\n");
    PM_print(mw);

    printf("V (orthogonal)\n");
    PM_print(mv);

    printf("Is tr(U).U - 1 = 0?  ");
    diff = PM_minus(PM_times(PM_transpose(mu), mu), one);
    ok &= report_diff(diff);

    PM_transpose(mu);

    printf("Is tr(V).V - 1 = 0?  ");
    diff = PM_minus(PM_times(mv, PM_transpose(mv)), one);
    ok &= report_diff(diff);

    printf("Is V.tr(V) - 1 = 0?  ");
    diff = PM_minus(PM_times(mv, PM_transpose(mv)), one);
    ok &= report_diff(diff);

    PM_transpose(mv);

    if (m == mx)
       {printf("Is A - U.W.tr(V) = 0?  ");
	diff = PM_minus(ma, PM_times(mu, PM_times(mw, PM_transpose(mv))));
	ok &= report_diff(diff);}
    else
       {mu->nrow = m;
	PM_print(ma);
	PM_print(PM_times(mu, PM_times(mw, PM_transpose(mv))));};

    PM_destroy(one);
    PM_destroy(mu);
    PM_destroy(mw);
    PM_destroy(mv);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - square matrix */

int test_1(int nx)
   {int i, j, k, n, ok;
    double *a;
    PM_matrix *ma;

    printf("\nSquare matrix test\n");

    ok = TRUE;
    for (n = 2; n < nx; n++)
        {printf("----------------------------------------------------------\n");
	 printf("N = %d\n", n);

	 ma = PM_create(n, n);
	 a  = ma->array;

	 for (j = 0; j < n; j++)
	     {for (i = 0; i < n; i++)
		  {k    = i*n + j;
		   a[k] = i + j + 2;};};

	 ok = svd_kernel(ma);
	 if (ok == TRUE)
	    printf("OK\n");
	 else
	    printf("NG\n");

         PM_destroy(ma);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - overdetermined matrix */

int test_2(int nx)
   {int i, j, k, n, m, ok;
    double *a;
    PM_matrix *ma;

    printf("\nOverdetermined system test\n");

    ok = TRUE;
    for (n = 1; n < nx; n++)
        {printf("----------------------------------------------------------\n");
	 m = 3*n;

	 printf("M = %d\n", m);
	 printf("N = %d\n", n);

	 ma = PM_create(m, n);
	 a  = ma->array;

	 for (j = 0; j < m; j++)
	     {for (i = 0; i < n; i++)
		  {k    = i*n + j;
		   a[k] = i + j + 2;};};

	 ok = svd_kernel(ma);
	 if (ok == TRUE)
	    printf("OK\n");
	 else
	    printf("NG\n");

         PM_destroy(ma);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - underdetermined matrix */

int test_3(int nx)
   {int i, j, k, n, m, ok;
    double *a;
    PM_matrix *ma;

    printf("\nUnderdetermined system test\n");

    ok = TRUE;
    for (n = 2; n < nx; n++)
        {printf("----------------------------------------------------------\n");
	 m = n/2;
	 m = max(m, 1);

	 printf("M = %d\n", m);
	 printf("N = %d\n", n);

	 ma = PM_create(m, n);
	 a  = ma->array;

	 for (j = 0; j < m; j++)
	     {for (i = 0; i < n; i++)
		  {k    = i*n + j;
		   a[k] = i + j + 2;};};

	 ok = svd_kernel(ma);
	 if (ok == TRUE)
	    printf("OK\n");
	 else
	    printf("NG\n");

         PM_destroy(ma);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print help message */

void help(void)
   {

    printf("\n");
    printf("Usage: mlsvdt [-1] [-2] [-3] [-h] [-n #]\n");
    printf("   1   do not run test 1\n");
    printf("   2   do not run test 2\n");
    printf("   3   do not run test 3\n");
    printf("   h   this help message\n");
    printf("   n   maximum number of unknowns (default 5)\n");
    printf("\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - */

int main(int c, char **v)
   {int i, nx, ok, rv;
    int tst[] = { TRUE, TRUE, TRUE };
    PFTFnc tf[] = { test_1, test_2, test_3 };

    PM_enable_fpe_n(TRUE, catch_fpe, NULL);

    nx = 5;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-1") == 0)
	    tst[0] = FALSE;
	 else if (strcmp(v[i], "-2") == 0)
	    tst[1] = FALSE;
	 else if (strcmp(v[i], "-3") == 0)
	    tst[2] = FALSE;
	 else if (strcmp(v[i], "-h") == 0)
	    {help();
	     return(1);}
	 else if (strcmp(v[i], "-n") == 0)
	    nx = SC_stoi(v[++i]);};

    ok = TRUE;
    for (i = 0; i < N_TEST; i++)
        {if (tst[i] == TRUE)
	    ok &= tf[i](nx);};

    rv = !ok;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


