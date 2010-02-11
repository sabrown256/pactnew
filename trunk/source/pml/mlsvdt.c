/*
 * MLSVDT.C - test SVD solver
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

#define KL 25

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

/* MAIN - */

int main()
   {int i, j, k, n, ok;
    REAL ret, wc;
    REAL *a, *u, *w, *v;
    PM_matrix *ma, *mu, *mw, *mv, *one, *diff;

    PM_enable_fpe(TRUE, catch_fpe);

    ok = TRUE;
    for (n = 2; n < 5; n++)
        {printf("----------------------------------------------------------\n");
	 printf("N = %d\n", n);

	 ma = PM_create(n, n);
	 mu = PM_create(n, n);
	 mw = PM_create(n, n);
	 mv = PM_create(n, n);

	 one = PM_ident(PM_zero(PM_create(n, n)));

	 a = ma->array;
	 u = mu->array;
	 w = mw->array;
	 v = mv->array;

	 for (j = 0; j < n; j++)
	     {for (i = 0; i < n; i++)
		  {k    = i*n + j;
		   a[k] = i + j + 2;
		   u[k] = a[k];
		   w[k] = 0.0;};};

	 PM_svd_decompose(u, n, n, w, v);

	 printf("Condition number of W = %10.3e\n\n", condition_number(w, n));

	 printf("U (orthogonal)\n");
	 PM_print(mu);

/* convert vector to diagonal matrix */
	 for (i = 1; i < n; i++)
	     {k    = i*n + i;
	      wc   = w[i];
	      w[k] = wc;
	      w[i] = 0.0;};

	 printf("W (diagonal)\n");
	 PM_print(mw);

	 printf("V (orthogonal)\n");
	 PM_print(mv);

	 printf("Is U.tr(U) - 1 = 0?  ");
	 diff = PM_minus(PM_times(mu, PM_transpose(mu)), one);
         ok &= report_diff(diff);

	 PM_transpose(mu);

	 printf("Is V.tr(V) - 1 = 0?  ");
	 diff = PM_minus(PM_times(mv, PM_transpose(mv)), one);
         ok &= report_diff(diff);

	 PM_transpose(mv);

	 printf("Is A - U.W.V = 0?  ");
	 diff = PM_minus(ma, PM_times(mu, PM_times(mw, PM_transpose(mv))));
         ok &= report_diff(diff);

	 if (ok == TRUE)
	    printf("OK\n");
	 else
	    printf("NG\n");

         PM_destroy(one);
         PM_destroy(ma);
         PM_destroy(mu);
         PM_destroy(mw);
         PM_destroy(mv);};

    ret = !ok;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


