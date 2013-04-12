/*
 * TMLCG.C - test Conjugate Gradient solvers
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pml.h"

typedef void (*setter)(void *a, int i, int j, double v);
typedef void (*loader)(void *a, int nd, int *dim, setter set,
		       int n, double *b, double *x0);
typedef double *(*solver)(int n, int *pit, double *ptol,
			  int nd, int *dim, loader lt);

typedef struct s_operator operator;

struct s_operator
   {loader fnc;
    char *name;
    int nd_max;
    int it_max;};

/*--------------------------------------------------------------------------*/

/*                                   SETTERS                                */

/*--------------------------------------------------------------------------*/

/* SET_SLS - set the A(I,J) = V for a PM_sp_lin_sys */

static void set_sls(void *a, int i, int j, double v)
   {PM_sp_lin_sys *sls;

    sls = (PM_sp_lin_sys *) a;

    PM_set_sls_coef(sls, i, j, v);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SET_MAT - set the A(I,J) = V for a PM_matrix */

static void set_mat(void *a, int i, int j, double v)
   {PM_matrix *ma;

    ma = (PM_matrix *) a;

    PM_element(ma, i+1, j+1) = v;

    return;}

/*--------------------------------------------------------------------------*/

/*                                   LOADERS                                */

/*--------------------------------------------------------------------------*/

/* LOAD_LAPL - load up a Laplacian operator to test the solvers */

static void load_lapl(void *a, int nd, int *dim, setter set,
		      int n, double *b, double *x0)
   {int i, id, str;
    int lim, lmn, lmx;
    double cnt;

    cnt = -2.0*nd;

/* load up an ND Laplacian system */
    for (str = 1, lim = 1, id = 0; id < nd; id++)
        {lim *= dim[id];
	 for (i = 0; i < n; i++)
	     {lmn = (i / lim)*lim;
	      lmx = lmn + lim;
	      if (lmn <= i - str)
		 (*set)(a, i, i-str, 1.0);

	      (*set)(a, i, i, cnt);
	 
	      if (i+str < lmx)
		 (*set)(a, i, i+str, 1.0);};

	 str *= dim[id];};

/* load the right hand side and the initial guess */
    for (i = 0; i < n; i++)
        {b[i] = 1.0;
	 if (x0 != NULL)
	    x0[i] = 1.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOAD_BILPL - load up a bi-laplacian operator to test the solvers */

static void load_bilpl(void *a, int nd, int *dim, setter set,
		       int n, double *b, double *x0)
   {int i, j, il, jn, nt, str[9];
    int lim, lmn, lmx;
    double cnt, cnr, edg, vc, v[9];

    cnr = -0.25;
    edg = 0.5;
    cnt = -1.0;

    nt = PM_ipow(3, nd);

    switch (nd)
       {case 1 :
	     str[0] = -1;
	     str[1] =  0;
	     str[2] =  1;

	     v[0] = cnr;
	     v[1] = edg;
	     v[2] = cnr;
	     break;

        case 2 :
        default :
	     str[0] = -dim[0] - 1;
	     str[1] = -dim[0];
	     str[2] = -dim[0] + 1;
	     str[3] = -1;
	     str[4] =  0;
	     str[5] =  1;
	     str[6] =  dim[0] - 1;
	     str[7] =  dim[0];
	     str[8] =  dim[0] + 1;

	     v[0] = cnr;
	     v[1] = edg;
	     v[2] = cnr;
	     v[3] = edg;
	     v[4] = cnt;
	     v[5] = edg;
	     v[6] = cnr;
	     v[7] = edg;
	     v[8] = cnr;
	     break;};

/* load up a bi-Laplacian system */
    lim = dim[0];
    for (j = 0; j < nt; j++)
        for (i = 0; i < n; i++)
	    {lmn = (i / lim)*lim;
	     lmx = lmn + lim;
	     jn  = j % 9;
	     il  = i + str[jn];

/* GOTCHA: haven't worked out the coefficients for more than 2d */
	     vc  = v[jn];

	     if ((lmn <= il) && (il < lmx))
	        (*set)(a, i, il, vc);};

/* load the right hand side and the initial guess */
    for (i = 0; i < n; i++)
        {b[i] = 1.0;
	 if (x0 != NULL)
	    x0[i] = 1.0;};

    return;}

/*--------------------------------------------------------------------------*/

/*                               TEST SOLVERS                               */

/*--------------------------------------------------------------------------*/

/* TEST_DCSL - exercises a DEC/SOL direct solver
 *           - on a definite linear system of equations Ax = b loaded
 *           - into a PM_sp_lin_sys structure
 */

static double *test_dcsl(int n, int *pit, double *ptol, int nd, int *dim,
			 loader lt)
   {int *ips;
    PM_matrix *ma, *mx;
    double *x;
   
    ma = PM_create(n, n);
    mx = PM_create(n, 1);

/* load up the A matrix and B vector
 * NOTE: DEC/SOL solves in place to B and X are the same
 */
    (*lt)(ma, nd, dim, set_mat,
	  n, mx->array, NULL);

/* solve the system A.x = b */
    ips = CMAKE_N(int, n);

/* do the lu decomposition */
    PM_decompose(ma, ips, FALSE);

/* do the sol part */
    PM_sol(ma, mx, ips, FALSE);

/* get out the solution for return value */
    x = mx->array;

/* free the linear system space */
    mx->array = NULL;
    PM_destroy(ma);
    PM_destroy(mx);

    CFREE(ips);

    *pit  = 1;
    *ptol = 0.0;

    return(x);}

/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_DSBICG - exercises a Diagonally Scaled Bi-Conjugate Gradient solver
 *             - on a definite linear system of equations Ax = b loaded
 *             - into a PM_sp_lin_sys structure
 */

static double *test_dsbicg(int n, int *pit, double *ptol, int nd, int *dim,
			   loader lt)
   {PM_sp_lin_sys *sls;
    double *x;
   
/* make the sparse array skeleton */
    sls = PM_mk_sp_lin_sys(n, 1, 2*nd + 1, FALSE, FALSE, NULL, NULL);

/* load up the A matrix and B vector
 * sls->b[0] holds the RHS and sls->x[0] holds the initial guess
 */
    (*lt)(sls, nd, dim, set_sls,
	  sls->n_ups, sls->b[0], sls->x[0]);

/* solve the system A.x = b */
    PM_dsbicg(sls, 0, pit, ptol);

/* get out the solution for return value */
    x = sls->x[0];

/* free the sparse linear system */
    sls->x[0] = NULL;
    PM_rl_sp_lin_sys(sls);

    return(x);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* TEST_DSCG - exercise Diagonally Scaled Conjugate Gradient solver on a 
 *           - a symmetric positive definite linear system of equations
 *           - loaded into a PM_sp_lin_sys structure
 */

static double *test_dscg(int n, int *pit, double *ptol, int nd, int *dim,
			 loader lt)
   {PM_sp_lin_sys *sls;
    double *x;
   
    sls = PM_mk_sp_lin_sys(n, 1, 2*nd + 1, TRUE, FALSE, NULL, NULL);

/* load up the A matrix and B vector
 * sls->b[0] holds the RHS and sls->x[0] holds the initial guess
 */
    (*lt)(sls, nd, dim, set_sls,
	  sls->n_ups, sls->b[0], sls->x[0]);

/* solve the system A.x = b */
    PM_dscg(sls, 0, pit, ptol);

/* get out the solution for return value */
    x = sls->x[0];

/* free the sparse linear system */
    sls->x[0] = NULL;
    PM_rl_sp_lin_sys(sls);

    return(x);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* TEST_ICCG - exercise Incomplete Cholesky Conjugate Gradient solver on a 
 *           - a symmetric positive definite linear system of equations
 *           - loaded into a PM_sp_lin_sys structure
 */

static double *test_iccg(int n, int *pit, double *ptol, int nd, int *dim,
			 loader lt)
   {PM_sp_lin_sys *sls;
    double *x;
   
    sls = PM_mk_sp_lin_sys(n, 1, 2*nd + 1, TRUE, TRUE,
			   PM_iccg_pre, PM_iccg_cmp_Lr);

/* load up the A matrix and B vector
 * sls->b[0] holds the RHS and sls->x[0] holds the initial guess
 */
    (*lt)(sls, nd, dim, set_sls,
	  sls->n_ups, sls->b[0], sls->x[0]);

/* solve the system A.x = b */
    PM_dscg(sls, 0, pit, ptol);

/* get out the solution for return value */
    x = sls->x[0];

/* free the sparse linear system */
    sls->x[0] = NULL;
    PM_rl_sp_lin_sys(sls);

    return(x);}

/*---------------------------------------------------------------------------*/

/*                                  TOP LEVEL                                */

/*---------------------------------------------------------------------------*/

/* RUN_TEST - run the given test */

static double *run_test(solver f, char *s, int nd, int *dim,
			int *pit, double *ptol, loader lt)
   {int i, n;
    double time;
    double *x;

/* count the number of unknowns */
    n = 1;
    for (i = 0; i < nd; i++)
        n *= dim[i];

    time = SC_wall_clock_time();

    x = (*f)(n, pit, ptol, nd, dim, lt);

    time = SC_wall_clock_time() - time;

    PRINT(stdout,
	  " %s   %2d  %8d  %8d  %8.2e  %8.2e\n",
	  s, i, n, *pit, time, *ptol);
/*
    for (i = 0; i < n; i++)
        PRINT(stdout, "  X[%d] = %12.4e\n", i, x[i]);
*/

    return(x);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* PLOT_ANSWER - dump the answers for ultra */
 
static void plot_answers(FILE *fp, int nd, int *dim, char *label, double *x)
   {int i, n;

    n = 1;
    for (i = 0; i < nd; i++)
        n *= dim[i];

    PRINT(fp, "# %s %d\n", label, n);
    for (i = 0; i < n; i++)
        PRINT(fp, "%8d %12.6e\n", i, x[i]);

    return;}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* DIFF_ARRAY - diff two arrays
 *            - return the fractional difference
 */

static double diff_array(double *a, double *b, int nc, double tol)
   {int i;
    double ac, bc, nmr, den, fd;

    fd = 0.0;
    for (i = 0; i < nc; i++)
        {ac = a[i];
	 bc = b[i];

	 nmr = ac - bc;
	 nmr = ABS(nmr);
	 den = 0.5*(ABS(ac) + ABS(bc) + SMALL);

	 fd += (nmr/den);};

    fd /= ((double) nc);

    return(fd);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* COMPARE_ANSWER - compare the answers from the tests */

static int compare_answer(double *xs, double *xc, double *xi, double *xb,
			  int nd, int *dim, double tol)
   {int i, nc;
    int cc, ci, cb, err;
    double fdc, fdi, fdb, atol;

    nc = 1;
    for (i = 0; i < nd; i++)
        nc *= dim[i];

    cc = FALSE;
    ci = FALSE;
    cb = FALSE;

    atol = 1.5*tol;

    if ((xc != NULL) && (xs != NULL))
       {fdc = diff_array(xc, xs, nc, tol);
	cc  = (fdc > atol);
	if (cc)
	   PRINT(stdout,
		 "DSCG disagrees with direct solve by %8.2e (%8.2e)\n",
		 fdc, atol);};

    if ((xi != NULL) && (xs != NULL))
       {fdi = diff_array(xi, xs, nc, tol);
	ci  = (fdi > atol);
	if (ci)
	   PRINT(stdout,
		 "ICCG disagrees with direct solve by %8.2e (%8.2e)\n",
		 fdi, atol);};

    if ((xb != NULL) && (xs != NULL))
       {fdb = diff_array(xb, xs, nc, tol);
	cb  = (fdb > atol);
	if (cb)
	   PRINT(stdout,
		 "BICG disagrees with direct solve by %8.2e (%8.2e)\n",
		 fdb, atol);};

    err = (cc || ci || cb);

    CFREE(xs);
    CFREE(xc);
    CFREE(xi);
    CFREE(xb);

    return(err);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* MAIN - run the tests */

int main(int c, char **v)
   {int i, j, ndmx, nd, no, err, smll;
    int dscg, iccg, bicg;
    int itmx, its, itc, iti, itb;
    int *dim;
    char *oname;
    double tol, tos, toc, toi, tob;
    double *xs, *xc, *xi, *xb;
    loader lt;
    FILE *fp;
    static int dimh[] = {20, 30, 3};
/*    static int diml[] = {8, 7, 6, 5}; */
    static int diml[] = {20, 8, 3, 1};
    static int dims[] = {4, 3, 2};
    static operator opers[] = {{load_lapl, "laplacian", 100, 10000},
			       {load_bilpl, "bilaplacian", 2, 1000}};

    SC_zero_space_n(1, -2);

    PM_enable_fpe_n(TRUE, NULL, NULL);

    dscg = TRUE;
    iccg = TRUE;
    bicg = TRUE;
    fp   = NULL;

    tol  = 1.0e-10;

    dim  = diml;
    ndmx = sizeof(diml)/sizeof(int);
    no   = sizeof(opers)/sizeof(operator);
    smll = FALSE;

    for (i = 0; i < c; i++)
        {if (v[i][0] == '-')
            {switch (v[i][1])
		{case 'b':
		      bicg = FALSE;
		      break;
		 case 'c':
		      dscg = FALSE;
		      break;
		 case 'd':
		      fp = SC_fopen_safe("tmlcg.dat", "w");
		      break;
		 case 'h':
		      PRINT(stdout, "Usgae: tmlcg [-b] [-c] [-d] [-h] [-i] [-l] [-s]\n");
		      PRINT(stdout, "       b - suppress biconjugate gradient solve\n");
		      PRINT(stdout, "       c - suppress conjugate gradient solve\n");
                      PRINT(stdout, "       d - do debug plotting\n");
		      PRINT(stdout, "       h - this help message\n");
		      PRINT(stdout, "       i - suppress incomple cholesky conjugate gradient solve\n");
		      PRINT(stdout, "       l - do solves with large matrices\n");
		      PRINT(stdout, "       s - do solves with small matrices\n");
		      return(1);
		      break;
	         case 'i':
		      iccg = FALSE;
		      break;
		 case 'l':
		      dim  = dimh;
		      ndmx = sizeof(dimh)/sizeof(int);
		      break;
		 case 's':
		      dim  = dims;
		      ndmx = sizeof(dims)/sizeof(int);
		      smll = TRUE;
		      break;};};};

    if (smll)
       no = 1;

    err = 0;
    for (j = 0; j < no; j++)
        {lt    = opers[j].fnc;
	 oname = opers[j].name;
	 nd    = opers[j].nd_max + 1;
	 itmx  = opers[j].it_max;

	 PRINT(stdout, "\nTesting with %s operator\n\n", oname);

	 PRINT(stdout, "Solver   D     Cells      Iter    Time      Err\n");

	 nd = min(nd, ndmx);

	 if (smll)
	    nd = 2;

	 for (i = 1; i < nd; i++)
	     {xc = NULL;
	      xi = NULL;
	      xb = NULL;
	      xs = NULL;

	      its = itmx;
	      itc = itmx;
	      iti = itmx;
	      itb = itmx;

	      tos = tol;
	      toc = tol;
	      toi = tol;
	      tob = tol;

	      PRINT(stdout, "----------\n");

	      if (dscg)
		 xs = run_test(test_dcsl, "dsol", i, dim, &its, &tos, lt);

	      if (dscg)
		 xc = run_test(test_dscg, "dscg", i, dim, &itc, &toc, lt);

	      if (iccg)
		 xi = run_test(test_iccg, "iccg", i, dim, &iti, &toi, lt);

	      if (bicg)
		 xb = run_test(test_dsbicg, "bicg", i, dim, &itb, &tob, lt);

	      if (fp != NULL)
		 {plot_answers(fp, i, dim, "dsol", xs);
		  plot_answers(fp, i, dim, "dscg", xc);
		  plot_answers(fp, i, dim, "iccg", xi);
		  plot_answers(fp, i, dim, "bicg", xb);};

	      err += compare_answer((its < itmx) ? xs : NULL,
				    (itc < itmx) ? xc : NULL,
				    (iti < itmx) ? xi : NULL,
				    (itb < itmx) ? xb : NULL,
				    i, dim, tol);};};

    if (fp != NULL)
       fclose(fp);

    return(err);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
