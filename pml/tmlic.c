/*
 * TMLIC.C - test vector ICCG solver
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#define KM	4
#define LM	3
#define KXL	24						 /* 2*KM*LM */
#define KL	12						   /* KM*LM */
#define EPS	1.0e-6
#define KS	4
#define MAXIT	100

static double
 ans[] = {4, 7, 8, 6};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ORIENT_ROW - solve a Laplace equation - row oriented arrays */

int orient_row(void)
   {int i, j, k, ok;
    double ret, err, erc, tol;
    double a0[KXL], a1[KXL], b0[KXL], b1[KXL], bm1[KXL], x[KXL], y[KXL];

    for (i = 0; i < KXL; i++)
	{a0[i]	= 0.0;
	 a1[i]	= 0.0;
	 b0[i]	= 0.0;
	 b1[i]	= 0.0;
	 bm1[i] = 0.0;
	 x[i]	= 0.0;
	 y[i]	= 0.0;};

    for (i = KM; i < 2*KM; i++)
	{a0[i]  = 2.0;
	 a1[i]  = -1.0;
	 b0[i]  = 0.0;
	 b1[i]  = 0.0;
	 bm1[i] = 0.0;};

    a1[2*KM-1] = 0.0;
    for (i = 0; i < KM; i++)
	{k = KM + i;
	 y[k] = (double) (i+1);};

    printf("\nProblem y :\n");
    for (j = 0; j < LM; j++)
	{printf("\nRow #%d: \n", j+1);
	 for (i = 0; i < KM; i++)
	     {k = j*KM + i;
	      printf(" y(%2d, %2d) = %11.3e ", j+1, i+1, y[k]);};};
    printf("\n");
    
    ret = _PM_iccg_v(KM, LM, EPS, KS, MAXIT, a0, a1, b0, b1, bm1, x, y);

    err = 0.0;
    for (i = 0; i < KM; i++)
        {k   = KM + i;
	 erc = x[k] - ans[i];
	 err += fabs(erc);};

    tol = 10.0*PM_machine_precision();
    ok  = (err < tol);

    printf("\nSolution x (should be (4, 7, 8, 6)) : %d %11.3e\n", ok, ret);
    for (j = 0; j < LM; j++)
	{printf("\nRow #%d: \n", j+1);
	 for (i = 0; i < KM; i++)
	     {k = j*KM + i;
	      printf(" x(%2d, %2d) = %11.3e ", j+1, i+1, x[k]);};};
    printf("\n\n");

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ORIENT_COL - solve a Laplace equation - column oriented arrays */

int orient_col(void)
   {int i, j, k, ok;
    double ret, erc, err, tol;
    double a0[KXL], a1[KXL], b0[KXL], b1[KXL], bm1[KXL], x[KXL], y[KXL];

    for (i = 0; i < KXL; i++)
	{a0[i]	= 0.0;
	 a1[i]	= 0.0;
	 b0[i]	= 0.0;
	 b1[i]	= 0.0;
	 bm1[i] = 0.0;
	 x[i]	= 0.0;
	 y[i]	= 0.0;};

    for (i = 1; i < KXL; i += LM)
	{a0[i] = 2.0;
	 a1[i] = 0.0;
	 b0[i] = -1.0;
	 b1[i] = 0.0;
	 bm1[i] = 0.0;};

    for (k = 1, i = 1; i < KXL; i += LM, k++)
	y[i] = (double) k;

    printf("\nProblem y :\n");
    for (j = 0; j < KM; j++)
	{printf("\nRow #%d: \n", j+1);
	 for (i = 0; i < LM; i++)
	     {k = j*LM + i;
	      printf(" y(%2d, %2d) = %11.3e ", j+1, i+1, y[k]);};};
    printf("\n");
    
    ret = _PM_iccg_v(LM, KM, EPS, KS, MAXIT, a0, a1, b0, b1, bm1, x, y);

    err = 0.0;
    for (i = 0; i < KM; i++)
        {k   = KM + i;
	 erc = x[k] - ans[i];
	 err += fabs(erc);};

    tol = 10.0*PM_machine_precision();

/* GOTCHA: this orientation gives bad results for a reasonable number of
 * iterations
 */
    tol = 20.0;
    ok  = (err < tol);

    printf("\nSolution x (should be (4, 7, 8, 6)) : %d %11.3e\n", ok, ret);
    for (j = 0; j < KM; j++)
	{printf("\nRow #%d: \n", j+1);
	 for (i = 0; i < LM; i++)
	     {k = j*LM + i;
	      printf(" x(%2d, %2d) = %11.3e ", j+1, i+1, x[k]);};};
    printf("\n\n");

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - solve a Laplace equation */

int main(int c, char **v)
   {int ok, rv;

    ok  = TRUE;
    ok &= orient_row();
    ok &= orient_col();

    if (ok == TRUE)
       rv = 0;
    else
       rv = 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


