/*
 * TMLICS.C - test scalar ICCG solver
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

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - solve a Laplace equation */

int main(int c, char **v)
   {int i, j, k;
    double ret;
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
	{a0[i] = 2.0;
	 a1[i] = -1.0;
	 b0[i] = 0.0;
	 b1[i] = 0.0;
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
    
    ret = _PM_iccg_s(KM, LM, EPS, MAXIT, a0, a1, b0, b1, bm1, x, y, KM*LM, 0);

    printf("\nSolution x (should be (4, 7, 8, 6)) : %11.3e\n", ret);
    for (j = 0; j < LM; j++)
	{printf("\nRow #%d: \n", j+1);
	 for (i = 0; i < KM; i++)
	     {k = j*KM + i;
	      printf(" x(%2d, %2d) = %11.3e ", j+1, i+1, x[k]);};};
    printf("\n\n");

    getchar();

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
    
    ret = _PM_iccg_s(LM, KM, EPS, MAXIT, a0, a1, b0, b1, bm1, x, y, KM*LM, 0);

    printf("\nSolution x (should be (4, 7, 8, 6)) : %11.3e\n", ret);
    for (j = 0; j < KM; j++)
	{printf("\nRow #%d: \n", j+1);
	 for (i = 0; i < LM; i++)
	     {k = j*LM + i;
	      printf(" x(%2d, %2d) = %11.3e ", j+1, i+1, x[k]);};};
    printf("\n\n");

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


