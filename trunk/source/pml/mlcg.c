/*
 * MLCG.C - Conjugate Gradient solver for symmetric sparse linear systems (PM_dscg)
 *        - and stabilized Bi-Conjugate Gradient method (PM_bicg) for
 *        - unsymmetric sparse linear systems
 *        - contributed by Mike Lambert of CASC at LLNL
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pml_int.h"

/* TODO:
 * 1) redo the transpose to save the space of effectively two
 *    copies of the array
 * 2) improve the performance of the decomposition for ICCG
 * 3) add Jacobi preconditioner
 * 4) parallelize the CG solvers
 */

#define LINEAR_COMB(_y, _x, _a, _p, _n)                                     \
    {int _i;                                                                \
     for (_i = 0; _i < _n; _i++)                                            \
         _y[_i] = _x[_i] + _a*_p[_i];}

#define INNER_PROD(_v, _x, _y, _n)                                          \
    {int _i;                                                                \
     _v = 0.0;                                                              \
     for (_i = 0; _i < _n; _i++)                                            \
         _v += _x[_i]*_y[_i];}

#define COPY(_x, _y, _n)                                                    \
    {int _i;                                                                \
     for (_i = 0; _i < _n; _i++)                                            \
         _x[_i] = _y[_i];}

#define DPRINT(_w, _n)                                                      \
    {PRINT(stdout, "----------------\n");                                   \
     for (i = 0; i < _n; i++)                                               \
         PRINT(stdout, #_w "[%d] = %12.6e\n", i, _w[i]);}

#define SP_SELEMENT(_s, _i, _js) (((double *) (_s)->aj[_i].array)[_js])
#define SP_COL_IND(_s, _i, _js)  (((int *) (_s)->j[_i].array)[_js])
#define SP_FINDEX(_s, _i, _j)    ((_s)->indx[_i][_j])
#define SP_N_ROW(_s, _i)         ((_s)->j[_i].n)

#define SP_FELEMENT(_a, _s, _i, _j)                                         \
   {int _js;                                                                \
    _js = SP_FINDEX(_s, _i, _j);                                            \
    _a  = (_js == -1) ? 0.0 : SP_COL_IND(_s, _i, _js);}

#define SP_ROW_ELEM(_ljk, _j, _k, _in, _sls, _sdl)                          \
    {int _ks, _nks;                                                         \
     _ljk = _in;                                                            \
     _nks = SP_N_ROW(_sls, _k);                                             \
     for (_ks = 0; _ks < _nks; _ks++)                                       \
         {if (_j == SP_COL_IND(_sls, _k, _ks))                              \
	     {_ljk = SP_SELEMENT(_sdl, _k, _ks);                            \
	      break;};};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SORT_SP - sort the rows so that the column indeces
 *             - are increasing order
 *             - also fill an array of the indeces of the diagonal elements
 */

static void _PM_sort_sp(PM_sp_lin_sys *sls)
   {int i, n, js, njs;
    int lh, l1, l2, tj1, tj2, gap;
    int *pj, *diag;
    double ta;
    double *pa;
    SC_array *jj, *aj; 

    if (sls == NULL)
       return;

    n = sls->n_ups;

    if (n == 0)
       return;

    jj = sls->j;
    aj = sls->aj;

/* sort the rows */
    for (i = 0; i < n; i++)
        {njs = jj[i].n;
	 pj  = jj[i].array;
	 pa  = aj[i].array;

         for (gap = njs >> 1; gap > 0; gap >>= 1)
	     for (lh = gap; lh < njs; lh++)
	         for (l1 = lh-gap; l1 >= 0; l1 -= gap)
		     {l2 = l1 + gap;

		      tj1 = pj[l1];
		      tj2 = pj[l2];
		      if (tj1 <= tj2)
			 break;

		      pj[l1] = tj2;
		      pj[l2] = tj1;

		      ta     = pa[l1];
		      pa[l1] = pa[l2];
		      pa[l2] = ta;};};

/* find the diagonals */
    diag = sls->diagonal;
    if (diag == NULL)
       diag = CMAKE_N(int, n);

    for (i = 0; i < n; i++)
        {njs = jj[i].n;
	 pj  = jj[i].array;
	 for (js = 0; js < njs; js++)
	     {if (i == pj[js])
		 {diag[i] = js;
		  break;};};};

    sls->diagonal = diag;

    return;}

/*--------------------------------------------------------------------------*/

/*                              METHODS FOR CG                              */

/*--------------------------------------------------------------------------*/

/* _PM_CG_CMP_AP - compute A.p for a sparse system */

static void _PM_cg_cmp_Ap(double *Ax, PM_sp_lin_sys *sls, double *x)
   {int i, j, js, n, njs;
    double aij;

    n = sls->n_ups;

    PM_set_value(Ax, n, 0.0);

    if (sls->symmetric)
       {for (i = 0; i < n; i++)
	    {njs = SP_N_ROW(sls, i);
	     for (js = 0; js < njs; js++)
	         {j      = SP_COL_IND(sls, i, js);
		  aij    = SP_SELEMENT(sls, i, js);
		  Ax[i] += aij*x[j];
		  if (i != j)
		     Ax[j] += aij*x[i];};};}

    else
       {for (i = 0; i < n; i++)
	    {njs = SP_N_ROW(sls, i);
	     for (js = 0; js < njs; js++)
	         {j      = SP_COL_IND(sls, i, js);
		  aij    = SP_SELEMENT(sls, i, js);
		  Ax[i] += aij*x[j];};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_CG_CMP_LR - compute L.r for a sparse system */

static void _PM_cg_cmp_Lr(double *Lr, PM_sp_lin_sys *sls, double *r)
   {int i, js, n, njs;
    double aij;

    n = sls->n_ups;

    for (i = 0; i < n; i++)
        {njs = SP_N_ROW(sls, i);
	 for (js = 0; js < njs; js++)
	     {if (i == SP_COL_IND(sls, i, js))
		 {aij   = SP_SELEMENT(sls, i, js);
		  Lr[i] = r[i]/aij;};};};

    return;}

/*--------------------------------------------------------------------------*/

/*                             METHODS FOR ICCG                             */

/*--------------------------------------------------------------------------*/

/* PM_ICCG_PRE - compute the incomplete Cholesky decomposition
 *             - this routine computes Lij as the upper triangular matrix
 *             - so Lij != 0 iff j >= i
 */

void PM_iccg_pre(PM_sp_lin_sys *sls)
   {int i, j, k, n, n_ods;
    int js, njs;
    double lij, ljk, lik, li, lk;
    double *d;
    PM_sp_lin_sys *sdl;

    n     = sls->n_ups;
    n_ods = 7;

    sdl = PM_mk_sp_lin_sys(n, 1, n_ods, TRUE, TRUE, NULL, NULL);
    if (sdl != NULL)
       {d   = sdl->b[0];
	li  = 0;

	for (i = 0L; i < n; i++)
	    {njs = SP_N_ROW(sls, i);
	     for (js = 0; js < njs; js++)
	         {j = SP_COL_IND(sls, i, js);

/* compute only upper triangular part */
		  if (j < i)
		     continue;

/* compute Lij = Aij - sum(0 <= k < i, Ljk*Lik/Lkk) */
		  lij = SP_SELEMENT(sls, i, js);
		  for (k = 0; k < i; k++)
		      {SP_ROW_ELEM(lk, k, k, HUGE, sls, sdl);

		       SP_ROW_ELEM(ljk, j, k, 0.0, sls, sdl);
		       if (ljk == 0.0)
			  continue;

		       SP_ROW_ELEM(lik, i, k, 0.0, sls, sdl);
		       if (lik == 0.0)
			  continue;

		       lij -= ljk*lik/lk;};

		  PM_set_sls_coef(sdl, i, j, lij);
		  if (i == j)
		     li = lij;};

	     d[i] = li;};

/* sort the rows and form the diagonal for L */
	_PM_sort_sp(sdl);

	sls->next = sdl;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ICCG_CMP_LR - compute L.R for CG with incomplete Cholesky decomposition
 *                - that is solve L.R = (Ut)(E)(U).R 
 *                - where A  ~ LDLt
 *                -       U  = L^(-1)
 *                -       Ut = trans(U)
 *                -       E  = D^(-1)
 *                - the matrix U has the same sparsity pattern as the original
 *                - matrix A (by the ICCG construction)
 */

void PM_iccg_cmp_Lr(double *Lr, PM_sp_lin_sys *sls, double *r)
   {int i, j, n, is, js, njs;
    int *diag, *pj, *pjt;
    double ri, li, lij, lji;
    double *d, *w, *pa, *pat;
    PM_sp_lin_sys *sdl;
    SC_array *jj, *aj, *jtj, *atj; 

    sdl = sls->next;

    n = sdl->n_ups;
    d = sdl->b[0];
    w = sdl->x[0];

/* setup access to the diagonal, Lii */
    diag = sdl->diagonal;

/* setup access to the decomposiiton, Lij */
    jj  = sdl->j;
    aj  = sdl->aj;

/* setup access to the transpose, Lji */
    jtj = sdl->jt;
    atj = sdl->atj;

/* forward substitution sweep
 *    Wi = Ri - sum(0 <= j < i, Wj*Lji)
 */
    for (i = 0; i < n; i++)
        {pa = aj[i].array;

	 is = diag[i];
	 li = pa[is];

/* do logical loop: 0 <= j < i */
	 ri = r[i];
	 njs = jtj[i].n;
	 pjt = jtj[i].array;
	 pat = atj[i].array;
	 for (js = 0; js < njs; js++)
             {j   = pjt[js];
	      if (i <= j)
		 break;
	      lji = pat[js];
	      ri -= lji*w[j];};

	 w[i] = ri/li;};

/* diagonal scaling sweep */
    for (i = 0; i < n; i++)
        w[i] *= d[i];

/* back substitution sweep
 *   (L.R)i = Wi - sum(i+1 <= j < n, Lij*(L.R)j)
 */
    for (i = n-1; i >= 0; i--)
        {pa  = aj[i].array;
	 pj  = jj[i].array;
	 njs = jj[i].n;

	 is = diag[i];
	 li = pa[is];

/* do logical loop: i+1 <= j < n */
	 ri = w[i];
	 for (js = 1; js < njs; js++)
	     {j   = pj[js];
	      lij = pa[js];
	      ri -= lij*Lr[j];};

	 Lr[i] = ri/li;};

    return;}

/*--------------------------------------------------------------------------*/

/*                          SPARSE SYSTEM MANAGEMENT                        */

/*--------------------------------------------------------------------------*/

/* PM_MK_SP_LIN_SYS - make sparse linear system
 *                  -  N_UPS  number of unknowns per linear systems
 *                  -  N_RHS  number of RHS to solve for
 *                  -  N_ODS  number of neighbors an unknown has
 *                  -         seeds the growth of the linear system
 */

PM_sp_lin_sys *PM_mk_sp_lin_sys(int n_ups, int n_rhs, int n_ods,
				int sym, int trans,
				void (*pre)(PM_sp_lin_sys *sls),
				void (*clr)(double *Lr, PM_sp_lin_sys *sls,
					    double *r))
   {int i;
    PM_sp_lin_sys *sls;

    if (n_ups == 0)
       return(NULL);

    if (clr == NULL)
       clr = _PM_cg_cmp_Lr;

    sls = CMAKE(PM_sp_lin_sys);

    sls->ilo       = 0;
    sls->ihi       = 0;
    sls->n_ups     = n_ups;
    sls->n_rhs     = n_rhs;
    sls->symmetric = sym;
    sls->transpose = trans;
    sls->diagonal  = NULL;

    sls->j  = CMAKE_N(SC_array, n_ups);
    sls->aj = CMAKE_N(SC_array, n_ups);

    for (i = 0; i < n_ups; i++)
        {CINIT_ARRAY((sls->j  + i), int,  NULL, 0);
	 CINIT_ARRAY((sls->aj + i), double, NULL, 0);};

    if (trans)
       {sls->jt  = CMAKE_N(SC_array, n_ups);
	sls->atj = CMAKE_N(SC_array, n_ups);

	for (i = 0; i < n_ups; i++)
	    {CINIT_ARRAY((sls->jt  + i), int,  NULL, 0);
	     CINIT_ARRAY((sls->atj + i), double, NULL, 0);};};


    sls->b      = PM_make_vectors(n_rhs, n_ups);
    sls->x      = PM_make_vectors(n_rhs, n_ups);
    sls->next   = NULL;
    sls->pre    = pre;
    sls->cmp_Ap = _PM_cg_cmp_Ap;
    sls->cmp_Lr = clr;

    return(sls);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SET_SLS_COEF - load coefficients for sparse linear system solves
 *                 - insert A(i,j) into AJ and
 *                 - insert j      into JJ
 */

void PM_set_sls_coef(PM_sp_lin_sys *sls, int i, int j, double a)
   {int is, js, nis, njs;
    int *pj;
    double *pa;
    SC_array *jj, *aj, *jtj, *atj; 

    if (!sls->symmetric || (i <= j))
       {jj = sls->j;
	aj = sls->aj;

	njs = jj[i].n;
	pj  = jj[i].array;
	pa  = aj[i].array;
	for (js = 0; js < njs; js++)
	    {if (j == pj[js])
	        {pa[js] = a;
		 break;};};

	if (js >= njs)
	   {SC_array_push(jj+i, &j);
	    SC_array_push(aj+i, &a);};

/* lay in the transpose */
	if (sls->transpose)
	   {jtj = sls->jt;
	    atj = sls->atj;

	    nis = jtj[j].n;
	    pj  = jtj[j].array;
	    pa  = atj[j].array;
	    for (is = 0; is < nis; is++)
	        {if (i == pj[is])
		    {pa[is] = a;
		     break;};};

	    if (is >= nis)
	       {SC_array_push(jtj+j, &i);
		SC_array_push(atj+j, &a);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_RL_SP_LIN_SYS - free sparse linear system */

void PM_rl_sp_lin_sys(PM_sp_lin_sys *sls)
   {int n_rhs;

    n_rhs = sls->n_rhs;

    CFREE(sls->diagonal);

    SC_free_array(sls->j, NULL);
    SC_free_array(sls->aj, NULL);

    if (sls->transpose)
       {SC_free_array(sls->jt, NULL);
	SC_free_array(sls->atj, NULL);};

    PM_free_vectors(n_rhs, sls->b);
    PM_free_vectors(n_rhs, sls->x);

    if (sls->next != NULL)
       PM_rl_sp_lin_sys(sls->next);

    CFREE(sls);
    
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SLS_SYM_CHK - determines whether the given sparse linear system has
 *                - a symmetric matrix
 *                - return 1 if successful
 *                - return 0 if SLS bad
 *                - return -(# bad rows)
 */

int PM_sls_sym_chk(PM_sp_lin_sys *sls)
   {int i, j, is, js, n, nis, njs;
    int elems, rows, unsym, count, rv;
    double vij, vji;

    if (sls == NULL)
       return(FALSE);

    n = sls->n_ups;

    if (n == 0)
       return(FALSE);

    rv    = TRUE;
    elems = 0;
    rows  = 0;
    for (i = 0; i < n; i++)
        {unsym = FALSE;

	 njs = SP_N_ROW(sls, i);
	 for (js = 0; js < njs; js++)
	     {j   = SP_COL_IND(sls, i, js);
	      vij = SP_SELEMENT(sls, i, js);

	      count = 0;
	      nis   = SP_N_ROW(sls, j);
	      for (is = 0; is < nis; is++)
		  {if (i == SP_COL_IND(sls, j, is))
		      {vji = SP_SELEMENT(sls, j, is);
		       count += (vij == vji);};};

	      if (count != 1)
		 {unsym = TRUE;
		  elems++;};};

	 if (unsym)
	    rows++;};

    if (rows)
       rv = -rows;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SLS_PAT_CHK - determines whether the given sparse linear system has
 *                - a matrix with a symmetric population
 *                - return 1 if successful
 *                - return 0 if SLS bad
 *                - return -(# bad rows)
 */

int PM_sls_pat_chk(PM_sp_lin_sys *sls)
   {int i, j, is, js, n, njs, nis;
    int elems, rows, unsym, count, rv;

    if (sls == NULL)
       return(FALSE);

    n = sls->n_ups;

    if (n == 0)
       return(FALSE);

    rv    = TRUE;
    elems = 0;
    rows  = 0;
    for (i = 0; i < n; i++)
        {unsym = FALSE;

	 njs   = SP_N_ROW(sls, i);
	 for (js = 0; js < njs; js++)
	     {j = SP_COL_IND(sls, i, js);

	      count = 0;
	      nis   = SP_N_ROW(sls, j);
	      for (is = 0; is < nis; is++)
		  {if (i == SP_COL_IND(sls, j, is))
		      count++;};

	      if (count != 1)
		 {unsym = TRUE;
		  elems++;};};

	 if (unsym)
	    rows++;};

    if (rows)
       rv = -rows;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SLS_COL_CHK - determines whether any row of the sparse linear system
 *                - has more than one entry for a given column of the matrix
 *                - return 1 if successful
 *                - return 0 if SLS bad
 *                - return -(# bad rows)
 */

int PM_sls_col_chk(PM_sp_lin_sys *sls)
   {int i, j, k, js, ks, n, njs;
    int rows, count, unsym, rv;

    if (sls == NULL)
       return(FALSE);

    n = sls->n_ups;

    if (n == 0)
       return(FALSE);

    rv   = TRUE;
    rows = 0;
    for (i = 0; i < n; i++)
        {unsym = FALSE;

	 njs = SP_N_ROW(sls, i);
	 for (js = 0; js < njs; js++)
	     {j = SP_COL_IND(sls, i, js);

	      count = 0;
	      for (ks = js; ks < njs; ks++)
		  {k = SP_COL_IND(sls, i, ks);
		   if (k == j)
		      count++;};

	      if (count != 1)
		 unsym = TRUE;};

	 if (unsym)
            rows++;};

    if (rows)
       rv = -rows;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SLS_B_CHK - determines whether b in Ax = b, has valid values
 *              - return  1 if successful
 *              - return  0 SLS bad
 *              - return -1 B bad
 *              - return -2 X bad
 */

int PM_sls_b_chk(PM_sp_lin_sys *sls)
   {int n, i, rv;
    int goodb, goodx0;
    double *b, *x0;

    if (sls == NULL)
       return(FALSE);

    n = sls->n_ups;
    if (n == 0)
       return(FALSE);

    b  = sls->b[0];
    x0 = sls->x[0];

    rv     = TRUE;
    goodb  = TRUE;
    goodx0 = TRUE;

    for (i = 0; i < n; i++)
        {goodb  = goodb  && (b[i]*b[i] < HUGE);
	 goodx0 = goodx0 && (x0[i]*x0[i] < HUGE);};

    if (!goodb)
       rv = -1;

    if (!goodx0)
       rv = -2;

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         BICONJUGATE GRADIENT SOLVER                      */

/*--------------------------------------------------------------------------*/

/* PM_DSBICG - performs serial diagonally scaled BiCG-STAB on a linear
 *           - system of equations Ax = b, loaded into a globally
 *           - declared PM_sp_lin_sys structure
 *           - A is definite but not necessarily symmetric
 *           - return -1 on failure
 *           - return number of iterations iff successful
 */

int PM_dsbicg(PM_sp_lin_sys *sls, int no, int *pit, double *ptol)
   {int i, j, n, js, njs, its, itmx, zs;
    double ibn, bn, tol, mtol, aij;
    double alpha, beta, omega, omhat, rho, rho_old, rn, sn, rtv, tts, ttt;
    double *b, *x0, *x, *Ax, *p, *phat, *r, *rtil, *s, *shat, *t, *v, *w;

    if (sls == NULL)
       return(-1);

    n = sls->n_ups;
    if (n == 0)
       return(-1);

    tol  = *ptol;
    itmx = *pit;

    _PM_sort_sp(sls);

    b  = sls->b[no];
    x0 = sls->x[no];

    zs = SC_zero_space_n(2, -1);

    x    = CMAKE_N(double, n);
    Ax   = CMAKE_N(double, n);
    p    = CMAKE_N(double, n);
    phat = CMAKE_N(double, n);
    r    = CMAKE_N(double, n);
    rtil = CMAKE_N(double, n);
    s    = CMAKE_N(double, n);
    shat = CMAKE_N(double, n);
    t    = CMAKE_N(double, n);
    v    = CMAKE_N(double, n);
    w    = CMAKE_N(double, n);

/* set iterate to initial guess x0 */
    COPY(x, x0, n);

/* calculate initial residual r = b - A.x */
    for (i = 0; i < n; i++)
        {njs   = SP_N_ROW(sls, i);
	 Ax[i] = 0.0;
	 for (js = 0; js < njs; js++)
	     {j      = SP_COL_IND(sls, i, js);
	      aij    = SP_SELEMENT(sls, i, js);
	      Ax[i] += aij*x[j];};};

    LINEAR_COMB(r, b, -1.0, Ax, n);
    COPY(rtil, r, n);

/* calculate norm of right-hand side */
    INNER_PROD(bn, b, b, n);
    bn  = sqrt(bn);
    ibn = 1.0/bn;

/* calculate residual norm */
    INNER_PROD(rn, r, r, n);
    rn = sqrt(rn);
    rn *= ibn;

    mtol = PM_machine_precision();
    tol  = max(tol, mtol);

/* main loop */
    for (its = 0; (rn > tol) && (its < itmx); its++)

/* calculate residual, residual-tilde inner product */
        {INNER_PROD(rho, rtil, r, n);
	 if (rho == 0.0)
	    return(FALSE);

	 if (its == 0)
            {COPY(p, r, n);}
	 else
	    {beta = (rho/rho_old)*(alpha/omega);
	     LINEAR_COMB(w, p, -omega, v, n);
	     LINEAR_COMB(p, r, beta, w, n);};
         
/* find phat, preconditioning with diag(A) */
	 for (i = 0; i < n; i++)
	     {njs = SP_N_ROW(sls, i);
	      for (js = 0; js < njs; js++)
	          {if (i == SP_COL_IND(sls, i, js))
		      {aij = SP_SELEMENT(sls, i, js);
		       phat[i] = p[i]/aij;};};};

/* compute v = Aphat */
	 for (i = 0; i < n; i++)
	     {v[i] = 0.0;
	      njs  = SP_N_ROW(sls, i);
	      for (js = 0; js < njs; js++)
	          {j     = SP_COL_IND(sls, i, js);
		   aij   = SP_SELEMENT(sls, i, js);
		   v[i] += aij*phat[j];};};

	 INNER_PROD(rtv, rtil, v, n);

	 alpha = rho/rtv;

	 LINEAR_COMB(s, r, -alpha, v, n);

/* calculate norm of s */
	 INNER_PROD(sn, s, s, n);
	 sn = sqrt(sn);

	 if (sn < 1.0e-10*tol)
	    {LINEAR_COMB(x, x, alpha, phat, n);
	     break;};

/* find shat, preconditioning with diag(A) */
	 for (i = 0; i < n; i++)
	     {njs = SP_N_ROW(sls, i);
	      for (js = 0; js < njs; js++)
	          {if (i == SP_COL_IND(sls, i, js))
		      {aij     = SP_SELEMENT(sls, i, js);
		       shat[i] = s[i]/aij;};};};

/* compute t = Ashat */
	 for (i = 0; i < n; i++)
	     {njs  = SP_N_ROW(sls, i);
	      t[i] = 0.0;
	      for (js = 0; js < njs; js++)
	          {j     = SP_COL_IND(sls, i, js);
		   aij   = SP_SELEMENT(sls, i, js);
		   t[i] += aij*shat[j];};};

/* compute tts */
	 INNER_PROD(tts, t, s, n);

/* compute ttt */
	 INNER_PROD(ttt, t, t, n);

/* compute omega */
	 omega = tts/ttt;

/* correct answer */
	 omhat = omega/alpha;
	 LINEAR_COMB(w, phat, omhat, shat, n);
	 LINEAR_COMB(x, x, alpha, w, n);

/* correct residual */
	 LINEAR_COMB(r, s, -omega, t, n);

/* calculate residual norm */
	 INNER_PROD(rn, r, r, n);
	 rn  = sqrt(rn);
	 rn *= ibn;

	 if (omega == 0.0)
	    {PRINT(stdout, "OMEGA IS ZERO - PM_DSBICG\n");
	     break;};
       
	 rho_old = rho;};

    *ptol = rn;
    *pit  = its;

/* put answer into sls.x[no] */
    COPY(x0, x, n);

    CFREE(v);
    CFREE(shat);
    CFREE(s);
    CFREE(rtil);
    CFREE(r);
    CFREE(phat);
    CFREE(p);
    CFREE(Ax);
    CFREE(x);
    CFREE(w);

    zs = SC_zero_space_n(zs, -1);

    return(its);}

/*--------------------------------------------------------------------------*/

/*                          CONJUGATE GRADIENT SOLVER                       */

/*--------------------------------------------------------------------------*/

/* PM_DSCG - performs serial diagonally-scaled CG on globally declared
 *         - PM_sp_lin_sys type sparse linear systems
 *         - works only on symmetric definite linear systems (since CG)
 *         -  b resides in sls.b[no]
 *         - x0 resides in sls.x[no]
 *         - return -1 on failure
 *         - return number of iterations iff successful
 *         - definitions:
 *         -    L = (diag A)^-1
 */

int PM_dscg(PM_sp_lin_sys *sls, int no, int *pit, double *ptol)
   {int n, its, itmx;
    double a, c, cstar, fac, ptAp, ibn, bn, rn, tol, mtol;
    double *b, *x0, *p, *r, *x, *Lr, *Ap, *Ax;

    if (sls == NULL)
       return(-1);

    n = sls->n_ups;
    if (n == 0)
       return(-1);

    itmx = *pit;
    tol  = *ptol;

    _PM_sort_sp(sls);

    b  = sls->b[no];
    x0 = sls->x[no];

    p   = CMAKE_N(double, n);
    r   = CMAKE_N(double, n);
    x   = CMAKE_N(double, n);
    Lr  = CMAKE_N(double, n);
    Ap  = CMAKE_N(double, n);
    Ax  = CMAKE_N(double, n);

/* set iterate to initial guess x0 */
    COPY(x, x0, n);

/* use a preconditioner if there is one */
    if (sls->pre != NULL)
       (*sls->pre)(sls);

/* calculate initial residual */
    (*sls->cmp_Ap)(Ax, sls, x);
    LINEAR_COMB(r, b, -1.0, Ax, n);

    INNER_PROD(bn, b, b, n);
    bn  = sqrt(bn);
    ibn = 1.0/bn;

    INNER_PROD(rn, r, r, n);
    rn  = sqrt(rn);
    rn *= ibn;

    mtol = PM_machine_precision();
    tol  = max(tol, mtol);

    cstar = 0.0;
    if (rn > tol)

/* find Lr = L.r */
       {(*sls->cmp_Lr)(Lr, sls, r);
	COPY(p, Lr, n);
	INNER_PROD(cstar, Lr, r, n);};

/* main loop */
    for (its = 0; (rn > tol) && (its < itmx); its++)

/* compute Ap */
        {(*sls->cmp_Ap)(Ap, sls, p);

/* compute a = cstar/ptAp */
	 INNER_PROD(ptAp, p, Ap, n);
	 a = cstar/ptAp;

/* compute x = x + ap */
	 LINEAR_COMB(x, x, a, p, n);

/* compute r = r - aAp */
	 LINEAR_COMB(r, r, -a, Ap, n);

/* find Lr = L.r */
	 (*sls->cmp_Lr)(Lr, sls, r);

	 INNER_PROD(rn, r, r, n);
	 rn  = sqrt(rn);
	 rn *= ibn;

	 if (rn > tol)

/* compute c = ztr */
	    {COPY(p, Lr, n);
	     INNER_PROD(c, Lr, r, n);

	     fac = c/cstar;

/* compute p = Lr + (c/cstar)p */
	     LINEAR_COMB(p, Lr, fac, p, n);

	     cstar = c;};};

    *ptol = rn;
    *pit  = its;

/* put answer into sls.x[no] */
    COPY(x0, x, n);

    CFREE(p);
    CFREE(r);
    CFREE(x);
    CFREE(Lr);
    CFREE(Ap);
    CFREE(Ax);

    return(its);}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
