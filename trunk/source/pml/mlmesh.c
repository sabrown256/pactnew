/*
 * MLMESH.C - 2D logical-rectangular mesh generation routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pml_int.h"

#define vecset4(v,v1,v2,v3,v4)                                               \
   v2 = v;                                                                   \
   v3 = v2 - 1;                                                              \
   v4 = v3 - kbnd;                                                           \
   v1 = v4 + 1

#define NODE_OF(_i, _j, _di) (((_j) - 1)*_di + (_i) - 1)

#define PM_CG      1
#define PM_DECSOL  2
#define PM_ICCG    3
#define PM_BICG    4
#define PM_HYPRE   5

typedef struct s_solver_method solver_method;

struct s_solver_method
   {int n_unk;
    int n_rhs;
    int nd;
    int *lmt;
    double **x;
    double **b;
    PM_matrix *lapl;
    PM_matrix **mb;
    PM_sp_lin_sys *axb;
    int strategy;
    void (*set)(solver_method *sv, int i, int j, double a);
    void (*solve)(solver_method *sv, PM_matrix *n_map);};

/*--------------------------------------------------------------------------*/

/*                            DECSOL ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _PM_DECSOL_SET - set the element A(I,J) = V */

static void _PM_decsol_set(solver_method *sv, int i, int j, double v)
   {PM_matrix *lapl;

    lapl = sv->lapl;

    PM_element(lapl, i+1, j+1) = v;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_DECSOL_LAPL - solve NA systems B = LAPL.X
 *                 - return the solution in X
 *                 - AR is work space to hold the solutions temporarily
 *                 - NOTE: this should be done with a multi-block tridi
 *                 - approach rather than LU decomposition
 */

static void _PM_decsol_lapl(solver_method *sv, PM_matrix *n_map)
   {int n, j, i, k, na, nd, nt, nh;
    int *ips;
    double **x, **b;
    PM_matrix *lapl, **mb;
   
    nd   = sv->nd;
    na   = sv->n_rhs;
    x    = sv->x;
    b    = sv->b;
    mb   = sv->mb;
    lapl = sv->lapl;

    nt = PM_ipow(3, nd);
    nh = (nt >> 1) + 1;

    n   = n_map->nrow;
    ips = CMAKE_N(int, n);

/* do the lu decomposition */
    PM_decompose(lapl, ips, FALSE);

/* do the sol part */
    for (k = 0; k < na; k++)
        {PM_sol(lapl, mb[k], ips, FALSE);};

    for (j = 0; j < n; j++)
        {i = PM_element(n_map, j+1, nh) - 1;
	 if (i >= 0)
	    {for (k = 0; k < na; k++)
	         x[k][i] = b[k][j];};};

    CFREE(ips);

    return;}

/*--------------------------------------------------------------------------*/

/*                              CG ROUTINES                                 */

/*--------------------------------------------------------------------------*/

/* _PM_CG_SET - set the element A(I,J) = V */

static void _PM_cg_set(solver_method *sv, int i, int j, double v)
   {PM_sp_lin_sys *axb;

    axb = sv->axb;

    PM_set_sls_coef(axb, i, j, v);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_CG_LAPL - solve NA systems B = LAPL.X
 *             - return the solution in X
 */

static void _PM_cg_lapl(solver_method *sv, PM_matrix *n_map)
   {int n, j, i, k, na, nd, nt, nh, ni, in, itmx;
    int strategy;
    double tol, itol;
    double **x, *pxi, *pxo;
    PM_sp_lin_sys *axb;
   
    nd       = sv->nd;
    na       = sv->n_rhs;
    x        = sv->x;
    axb      = sv->axb;
    strategy = sv->strategy;

    nt = PM_ipow(3, nd);
    nh = (nt >> 1) + 1;

    ni   = 0;
    tol  = 1.0e8*PM_machine_precision();
    itmx = 1000;

    if ((strategy == PM_CG) || (strategy == PM_ICCG))
       {for (i = 0; i < na; i++)
	    {itol = tol;
	     in   = itmx;
	     ni  += PM_dscg(axb, i, &in, &itol);};}

    else if (strategy == PM_BICG)
       {for (i = 0; i < na; i++)
	    {itol = tol;
	     in   = itmx;
	     ni  += PM_dsbicg(axb, i, &in, &itol);};};

     n = n_map->nrow;

     for (k = 0; k < na; k++)
         {pxi = axb->x[k];
	  pxo = x[k];
	  for (j = 0; j < n; j++)
	      {i = PM_element(n_map, j+1, nh) - 1;
	       pxo[i] = pxi[j];};};

    return;}

/*--------------------------------------------------------------------------*/

/*                             HYPRE ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _PM_HYPRE_LAPL - solve NA systems B = LAPL.X
 *                - return the solution in X
 *                - AR is work space to hold the solutions temporarily
 *                - NOTE: this should be done with a multi-block tridi
 *                - approach rather than LU decomposition
 */

static void _PM_hypre_lapl(solver_method *sv, PM_matrix *n_map)
   {

#ifdef HAVE_HYPRE

/* initiate the hypre solver */

/* give the rows to hypre */

/* let hypre solve it */

/* get the answer out */

/* cleanup */
        call HYPRE_IJMatrixDestroy(hypre_A, ierr)

#else
    
    _PM_cg_lapl(sv, n_map);

#endif

    return;}

/*--------------------------------------------------------------------------*/

/*                      SETUP LINEAR SOLVER ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* _PM_MK_LIN_SYS - setup the linear system solver storage */

static solver_method *_PM_mk_lin_sys(int n_unk, int n_rhs, int nd,
				     int *lmt, double **x, int strategy)
   {int j, ns;
    solver_method *sv;
    double **b;
    PM_matrix *lapl, **mb;
    PM_sp_lin_sys *axb;

    sv = CMAKE(solver_method);

    sv->n_unk    = n_unk;
    sv->n_rhs    = n_rhs;
    sv->nd       = nd;
    sv->x        = x;
    sv->lmt      = lmt;
    sv->lapl     = NULL;
    sv->mb       = NULL;
    sv->axb      = NULL;
    sv->strategy = strategy;

    if ((strategy == PM_CG) || (strategy == PM_BICG))
       {ns  = 2*nd + 1;
	axb = PM_mk_sp_lin_sys(n_unk, n_rhs, ns,
			       (strategy == PM_CG), FALSE,
			       NULL, NULL);

	sv->axb   = axb;
	sv->b     = axb->b;
        sv->set   = _PM_cg_set;
        sv->solve = _PM_cg_lapl;}

    else if (strategy == PM_ICCG)
       {ns  = 2*nd + 1;
	axb = PM_mk_sp_lin_sys(n_unk, n_rhs, ns, TRUE, TRUE,
			       PM_iccg_pre, PM_iccg_cmp_Lr);
	sv->axb   = axb;
	sv->b     = axb->b;
        sv->set   = _PM_cg_set;
        sv->solve = _PM_cg_lapl;}

    else if (strategy == PM_HYPRE)
       {ns  = 2*nd + 1;
	axb = PM_mk_sp_lin_sys(n_unk, n_rhs, ns, TRUE, TRUE,
			       NULL, NULL);
	sv->axb   = axb;
	sv->b     = axb->b;
        sv->set   = _PM_cg_set;
        sv->solve = _PM_hypre_lapl;}

    else
       {lapl = PM_create(n_unk, n_unk);

	b  = CMAKE_N(double *, n_rhs);
	mb = CMAKE_N(PM_matrix *, n_rhs);
	for (j = 0; j < n_rhs; j++)
	    {mb[j] = PM_create(n_unk, 1);
	     b[j]  = mb[j]->array;};

	PM_set_value(lapl->array, n_unk*n_unk, 0.0);

	sv->lapl  = lapl;
	sv->mb    = mb;
	sv->b     = b;
        sv->set   = _PM_decsol_set;
        sv->solve = _PM_decsol_lapl;};

    return(sv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_RL_LIN_SYS - release the linear system solver storage */

static void _PM_rl_lin_sys(solver_method *sv)
   {int j, n_rhs;

    n_rhs = sv->n_rhs;

    if (sv->axb != NULL)
       {PM_rl_sp_lin_sys(sv->axb);}

    else if (sv->lapl != NULL)
       {PM_destroy(sv->lapl);

/* release the intermediate storage */
	for (j = 0; j < n_rhs; j++)
	    PM_destroy(sv->mb[j]);

	CFREE(sv->b);
	CFREE(sv->mb);}

    else
       PM_free_vectors(4*n_rhs+4, sv->b);

    CFREE(sv);

    return;}

/*--------------------------------------------------------------------------*/

/*                           SERVICE ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* _PM_FILL_MAP - fill out the map array with nearest neighbor information */

static PM_matrix *_PM_fill_map(double *nodet, int *unm, int *str,
			       int n0, int nd, int *lmt)
   {int m, j, i, l, n, nt;
    int imx, jmx;
    PM_matrix *n_map;

    imx = lmt[0];
    jmx = lmt[1];

    nt = PM_ipow(3, nd);

    n_map = PM_create(n0, nt);

/* put the nodes into the n_map array */
    n = 0;
    for (j = 1; j < jmx; j++)
        for (i = 1; i < imx; i++)
            {m = NODE_OF(i, j, imx);
             if (nodet[m] == -1.0)
                {unm[m] = n;
		 n++;
		 for (l = 0; l < nt; l++)
		     PM_element(n_map, n, l+1) = m + str[l] + 1;};};

/* mark the neighbors that are already fixed */
    for (j = 1; j <= n; j++)
        for (l = 0; l < nt; l++)
	    {m = PM_element(n_map, j, l+1) - 1;
	     if (nodet[m] != -1.0)
	        PM_element(n_map, j, l+1) = -(m + 1);};

/* mark the selected nodes by resetting to 0 */
    for (j = 1; j < jmx; j++)
        for (i = 1; i < imx; i++)
            {m = NODE_OF(i, j, imx);
             if (nodet[m] == -1.0)
                nodet[m] = 0.0;};

    return(n_map);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FIN_SOL - map the solutions in tx and ty back into the mesh */

static void _PM_fin_sol(PM_matrix *n_map, int m, int *reg_map, double *nodet)
   {int i, j, n, nh;

    n  = n_map->nrow;
    nh = 5;
    for (j = 1; j <= n; j++)
        {i          = PM_element(n_map, j, nh) - 1;
         reg_map[i] = m;
         nodet[i]   = 1.0;};

    return;}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* _PM_MESH_CONVERGED - return TRUE iff the coordinates have converged */

static int _PM_mesh_converged(PM_matrix *xn, PM_matrix *yn,
			      PM_matrix *xo, PM_matrix *yo, int n)
   {int i;
    double *pxo, *pyo, *pxn, *pyn;
    double s, dx, dy, rx1, ry1, rx2, ry2, ax, ay;

    pxo = xo->array;
    pyo = yo->array;
    pxn = xn->array;
    pyn = yn->array;

    s = 0.0;
    for (i = 0; i < n; i++)
        {rx1 = *pxo++;
         rx2 = *pxn++;
         ry1 = *pyo++;
         ry2 = *pyn++;
         dx  = rx2 - rx1;
         dy  = ry2 - ry1;
         ax  = 0.5*(rx2 + rx1);
         ay  = 0.5*(ry2 + ry1);
         s  += (dx*dx + dy*dy)/(ax*ax + ay*ay + SMALL);};
    s = sqrt(s/((double) n));

    return((s < 0.0001) ? TRUE : FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_CARTESIAN_REGION - return TRUE iff the region described by the part
 *                      - has straight line side all the way around
 */

static int _PM_cartesian_region(PM_part *ipart)
   {int ret;
    PM_conic_curve *crv;
    PM_side *ib;

    ret = TRUE;
    for (ib = ipart->leg; TRUE; )
        {crv = ib->crve;
         ret &= ((crv->xx == 0.0) && (crv->yy == 0.0) && (crv->xy == 0.0));
         if ((ib = ib->next) == ipart->leg)
            break;};

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_CURVATURE - compute the curvature at J of the coordinate contour
 *               - implied by the stride, S
 */

static double _PM_curvature(int j, int s)
   {double dx1, dy1, dx2, dy2, dx3, dy3, dxf, dyf;
    double d0, t0, r0;

    dx1 = rx[j] - rx[j-s];
    dy1 = ry[j] - ry[j-s];
    dx2 = rx[j+s] - rx[j];
    dy2 = ry[j+s] - ry[j];
    dx3 = 0.5*(rx[j+s] - rx[j-s]);
    dy3 = 0.5*(ry[j+s] - ry[j-s]);
    d0  = dx1*dy2 - dy1*dx2;
    if (d0 == 0.0)
       r0 = SMALL;
    else
       {t0  = (dx3*dx2 + dy3*dy2)/d0;
        dxf = 0.5*dx1 + t0*dy1;
        dyf = 0.5*dy1 - t0*dx1;
        r0  = 1.0/(sqrt(dxf*dxf + dyf*dyf) + SMALL);};

    r0 = min(r0, param[9]);
    r0 = max(r0, param[8]);

    return(r0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COPY_SOL - copy the coordinate solutions */

static void _PM_copy_sol(PM_matrix *xo, PM_matrix *xn,
			 PM_matrix *yo, PM_matrix *yn, PM_matrix *n_map)
   {int i, j, n;
    double *pxo, *pyo, *pxn, *pyn;

    n   = xo->nrow;
    pxo = xo->array;
    pyo = yo->array;
    pxn = xn->array;
    pyn = yn->array;
    for (i = 0; i < n; i++)
        {*pxo++ = *pxn++;
         *pyo++ = *pyn++;};

    for (j = 1; j <= n; j++)
        {i     = PM_element(n_map, j, nh) - 1;
         rx[i] = PM_element(xn, j, 1);
         ry[i] = PM_element(yn, j, 1);};

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _PM_LOAD_LIN_SYS - load up the sparse linear system representation
 *                  - to solve NA systems of A.Xi = Bi
 *                  - where 0 < i < NA
 */

static void _PM_load_lin_sys(solver_method *sv,
			     int i, PM_matrix *n_map,
			     int n, double *av, int *unm, int *str)
   {int j, k, l, m, na;
    double **x, **b;
    void (*a)(solver_method *sv, int i, int j, double v);

    na = sv->n_rhs;
    b  = sv->b;
    x  = sv->x;
    a  = sv->set;

    for (k = 0; k < na; k++)
        b[k][i] = 0.0;

    for (l = 0; l < n; l++)
        {m = PM_element(n_map, i+1, l+1);
	 if (m > 0)
	    {j = unm[m-1];
	     (*a)(sv, i, j, av[l]);}
	 else
	    {for (k = 0; k < na; k++)
	         b[k][i] -= av[l]*x[k][-m-1];};};

    return;}

/*--------------------------------------------------------------------------*/

/*                            LAPLACE SOLVER                                */

/*--------------------------------------------------------------------------*/

/* _PM_FILL_LAPL_OP - set up the laplacian operator and the boundary arrays
 *                  - for the 2D laplacian solver
 *                  - this operator uses only the tangential
 *                  - spacing ratios
 *                  - if KRA or LRA are NULL or CRF is TRUE, use constant
 *                  - ratios KRC and LRC
 */

static void _PM_fill_lapl_op(solver_method *sv,
			     double *kra, double *lra,
			     PM_matrix *n_map, int *unm, int *str,
			     double ts, double krc, double lrc, int crf)
   {int i, j, n, nd, nt, nh;
    double sr, sl, sb, st;
    double pnt, cnt;
    double a[9];

    nd = sv->nd;
    n  = n_map->nrow;
    nt = PM_ipow(3, nd);
    nh = (nt >> 1) + 1;

    pnt = min(ts, 1.0);
    pnt = max(pnt, 0.0);
    cnt = 2.0 - pnt;

    for (i = 0; i < n; i++)
        {j = PM_element(n_map, i+1, nh) - 1;

         if (crf || (kra == NULL) || (lra == NULL))
            {sr = 1.0/(1.0 + krc);
             sl = krc*sr;
             st = 1.0/(1.0 + lrc);
             sb = lrc*st;}
         else
            {sr = 1.0/(1.0 + kra[j]);
             sl = kra[j]*sr;
             st = 1.0/(1.0 + lra[j]);
             sb = lra[j]*st;};

	 a[0] = -pnt*sl*sb;
	 a[1] = sb;
	 a[2] = -pnt*sr*sb;
	 a[3] = sl;
	 a[4] = -cnt;
	 a[5] = sr;
	 a[6] = -pnt*sl*st;
	 a[7] = st;
	 a[8] = -pnt*sr*st;

	 _PM_load_lin_sys(sv, i, n_map, nt, a, unm, str);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_LAPLACE_SOL - compute the coordinates using the correct laplacian
 *                 - this operator uses only the tangential
 *                 - spacing ratios
 */

static void _PM_laplace_sol(int n, int nd, int na, int *lmt, double **x,
			    double *kra, double *lra,
			    PM_matrix *n_map, int *unm, int *str,
			    double ts, double krc, double lrc,
			    int crf, int strategy)
   {solver_method *sv;

    sv = _PM_mk_lin_sys(n, na, nd, lmt, x, strategy);

/* fill in the linear system */
    _PM_fill_lapl_op(sv, kra, lra, n_map, unm, str, ts, krc, lrc, crf);

/* solve the system */
    (*sv->solve)(sv, n_map);

    _PM_rl_lin_sys(sv);

    return;}

/*--------------------------------------------------------------------------*/

/*                           LAPLACE SOLVER A                               */

/*--------------------------------------------------------------------------*/

/* _PM_COMPUTE_A_BND - compute the a quantities on the given side */

static void _PM_compute_a_bnd(double as, double xs, double ae, double xe,
			      double *v, int *lmt,
			      int kmn, int kmx, int lmn, int lmx)
   {int i, j, n, nt, sdk, sdl;
    int kbnd;
    double ps, pe, dk, dl;

    kbnd = lmt[0];

    if (as < 0.0)
       {ps  = -1.0;
	as *= -1.0;}
    else
       ps = 1.0;

    if (ae < 0.0)
       {pe  = -1.0;
	ae *= -1.0;}
    else
       pe = 1.0;

    dk = kmx - kmn;
    dl = lmx - lmn;
    n  = sqrt(dk*dk + dl*dl);
    nt = n/2;

    sdk = (dk > 0);
    sdl = (dl > 0);

    if (as != 0.0)
       {for (j = 0; j < nt; j++)
            {i    = NODE_OF(kmn + sdk*(j + 1), lmn + sdl*(j + 1), kbnd);
             v[i] = pow((1.0 + as*exp(-j*xs)), ps);};};

    if (ae != 0.0)
       {for (j = 0; j < nt; j++)
            {i    = NODE_OF(kmx - sdk*j, lmx - sdl*j, kbnd);
             v[i] = pow((1.0 + ae*exp(-j*xe)), pe);};};

    if (2*nt != n)
       {i    = NODE_OF(kmn + sdk*(nt + 1), lmn + sdl*(nt + 1), kbnd);
        v[i] = 1.0;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COMPUTE_A - compute the a quantities */

static void _PM_compute_a(double *apk, double *apl, double *kra, double *lra,
			  int *lmt, int kmn, int kmx, int lmn, int lmx,
			  double ask, double xsk, double aek, double xek,
			  double asl, double xsl, double ael, double xel,
			  int constr)
   {int j, j0, k, l;
    int kbnd, lbnd, nz;
    double u, dk, dl, dkl;
    double *x1, *x2, *x3, *x4, *s, *t;

    kbnd = lmt[0];
    lbnd = lmt[1];

    nz = kbnd*lbnd;

/* extrapolate lra to the undetermined boundary */
    if (lmx - lmn < 3)
       {for (k = kmn; k <= kmx; k++)
            {j      = NODE_OF(k, lmx, kbnd);
             lra[j] = lra[j-kbnd];
             j      = NODE_OF(k, lmn, kbnd);
             lra[j] = lra[j+kbnd];};}
    else
       {for (k = kmn; k <= kmx; k++)
            {j      = NODE_OF(k, lmx, kbnd);
             lra[j] = 2.0*lra[j-kbnd] - lra[j-2*kbnd];
             j      = NODE_OF(k, lmn, kbnd);
             lra[j] = 2.0*lra[j+kbnd] - lra[j+2*kbnd];};};

/* extrapolate kra to the undetermined boundary */
    if (kmx - kmn < 3)
       {for (l = lmn; l <= lmx; l++)
            {j      = NODE_OF(kmx, l, kbnd);
             kra[j] = kra[j-1];
             j      = NODE_OF(kmn, l, kbnd);
             kra[j] = kra[j+1];};}
    else
       {for (l = lmn; l <= lmx; l++)
            {j      = NODE_OF(kmx, l, kbnd);
             kra[j] = 2.0*kra[j-1] - kra[j-2];
             j      = NODE_OF(kmn, l, kbnd);
             kra[j] = 2.0*kra[j+1] - kra[j+2];};};

    _PM_compute_a_bnd(ask, xsk, aek, xek, apk,
		      lmt, kmn, kmx, lmx, lmx);

    _PM_compute_a_bnd(asl, xsl, ael, xel, apl,
		      lmt, kmx, kmx, lmn, lmx);

    t = CMAKE_N(double, nz);
    s = CMAKE_N(double, nz);

/* compute apl */
    PM_set_value(s, nz, 0.0);
    PM_set_value(t, nz, 0.0);

    vecset4(lra, x1, x2, x3, x4);
    for (l = lmn+1; l <= lmx; l++)
        {j0 = NODE_OF(kmx, l, kbnd);

         if (constr)

/* sweep to the right */
            {for (k = kmn+1; k <= kmx; k++)
                 {j   = NODE_OF(k, l, kbnd);
                  u   = 4.0/(x1[j] + x2[j] + x3[j] + x4[j]);
                  dk  = 0.5*(x2[j] - x3[j] - x4[j] + x1[j]);
                  dl  = 0.5*(x2[j] + x3[j] - x4[j] - x1[j]);
                  dkl = 0.25*(x2[j] - x3[j] + x4[j] - x1[j]);

                  s[j] = s[j-1] + 2.0*dl + dk*u*u - dkl*u;};

/* sweep to the left */
             for (k = kmx-1; k >= kmn; k--)
                 {j   = NODE_OF(k, l, kbnd);
                  u   = 4.0/(x1[j] + x2[j] + x3[j] + x4[j]);
                  dk  = 0.5*(x2[j] - x3[j] - x4[j] + x1[j]);
                  dl  = 0.5*(x2[j] + x3[j] - x4[j] - x1[j]);
                  dkl = 0.25*(x2[j] - x3[j] + x4[j] - x1[j]);

                  t[j] = t[j+1] - 2.0*dl - dk*u*u + dkl*u;};};

         for (k = kmn+1; k <= kmx; k++)
             {j      = NODE_OF(k, l, kbnd);
              apl[j] = 0.5*(s[j] + t[j]) + apl[j0];};};

/* compute apk */
    PM_set_value(s, nz, 0.0);
    PM_set_value(t, nz, 0.0);

    vecset4(kra, x1, x2, x3, x4);
    for (k = kmn+1; k <= kmx; k++)
        {j0 = NODE_OF(k, lmx, kbnd);

         if (constr)

/* sweep up */
            {for (l = lmn+1; l <= lmx; l++)
                 {j   = NODE_OF(k, l, kbnd);
                  u   = 4.0/(x1[j] + x2[j] + x3[j] + x4[j]);
                  dk  = 0.5*(x2[j] - x3[j] - x4[j] + x1[j]);
                  dl  = 0.5*(x2[j] + x3[j] - x4[j] - x1[j]);
                  dkl = 0.25*(x2[j] - x3[j] + x4[j] - x1[j]);

                  s[j] = s[j-kbnd] + 2.0*dk + dl*u*u - dkl*u;};

/* sweep down */
             for (l = lmx-1; l > lmn; l--)
                 {j   = NODE_OF(k, l, kbnd);
                  u   = 4.0/(x1[j] + x2[j] + x3[j] + x4[j]);
                  dk  = 0.5*(x2[j] - x3[j] - x4[j] + x1[j]);
                  dl  = 0.5*(x2[j] + x3[j] - x4[j] - x1[j]);
                  dkl = 0.25*(x2[j] - x3[j] + x4[j] - x1[j]);

                  t[j] = t[j+kbnd] - 2.0*dk - dl*u*u + dkl*u;};};

         for (l = lmn+1; l <= lmx; l++)
             {j   = NODE_OF(k, l, kbnd);
              apk[j] = 0.5*(s[j] + t[j]) + apk[j0];};};

    CFREE(s);
    CFREE(t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FILL_LAPL_OPA - set up the laplacian operator and the boundary arrays
 *                   - for the 2D laplacian solver
 *                   - this operator uses both the normal and tangential
 *                   - spacing ratios
 *                   - if KRA, LRA, APK, or APL are NULL or CRF is TRUE,
 *                   - use constant ratios KRC and LRC
 */

static void _PM_fill_lapl_opa(solver_method *sv, int *lmt,
			      double *kra, double *lra, double *apk, double *apl,
			      PM_matrix *n_map, int *unm, int *str,
			      double ts, double krc, double lrc,
			      int crf, double theta)
   {int i, j, n, kbnd;
    int nd, nt, nh;
    double alpha, beta;
    double s1, s2, s3, s4, sr, sl, sb, st;
    double akp, akm, alp, alm;
    double ckp, ck, ckm, clp, cl, clm;
    double bkp, bk, bkm, blp, bl, blm;
    double pnt, cnt;
    double a[9];

    nd = sv->nd;

    theta *= 0.5*PI;
    alpha = cos(theta);
    beta  = sin(theta);
    pnt   = alpha + beta;
    alpha /= pnt;
    beta  /= pnt;

    kbnd = lmt[0];

    n  = n_map->nrow;
    nt = PM_ipow(3, nd);
    nh = (nt >> 1) + 1;

    pnt = min(ts, 1.0);
    pnt = max(pnt, 0.0);
    cnt = 2.0 - pnt;

    for (i = 0; i < n; i++)
        {j = PM_element(n_map, i+1, nh) - 1;

         if ((kra == NULL) || (lra == NULL) ||
             (apk == NULL) || (apl == NULL) ||
             crf)
            {sr = 1.0/(1.0 + krc);
             sl = krc*sr;
             st = 1.0/(1.0 + lrc);
             sb = lrc*st;

             s1 = -pnt*sr*sb;
             s2 = -pnt*sr*st;
             s3 = -pnt*sl*st;
             s4 = -pnt*sl*sb;}

         else
            {akp = apk[j+1];
             akm = 1.0/apk[j];
             alp = apl[j+kbnd];
             alm = 1.0/apl[j];

             ckp = 1.0/(1.0 + kra[j+kbnd]);
             ck  = 1.0/(1.0 + kra[j]);
             ckm = 1.0/(1.0 + kra[j-kbnd]);
             clp = 1.0/(1.0 + lra[j+1]);
             cl  = 1.0/(1.0 + lra[j]);
             clm = 1.0/(1.0 + lra[j-1]);

             bkp = ckp*kra[j+kbnd];
             bk  = ck*kra[j];
             bkm = ckm*kra[j-kbnd];
             blp = clp*lra[j+1];
             bl  = cl*lra[j];
             blm = clm*lra[j-1];

             sr = (alpha + beta*akp)*ck;
             sl = (alpha + beta*akm)*bk;
             st = (alpha*alp + beta)*cl;
             sb = (alpha*alm + beta)*bl;

             s1 = -pnt*(alpha*alm*bl*ckm + beta*akp*ck*blp);
             s2 = -pnt*(alpha*alp*cl*ckp + beta*akp*ck*clp);
             s3 = -pnt*(alpha*alp*cl*bkp + beta*akm*bk*clm);
             s4 = -pnt*(alpha*alm*bl*bkm + beta*akm*bk*blm);};

	 a[0] = s4;
	 a[1] = sb;
	 a[2] = s1;
	 a[3] = sl;
	 a[4] = -cnt;
	 a[5] = sr;
	 a[6] = s3;
	 a[7] = st;
	 a[8] = s2;

	 _PM_load_lin_sys(sv, i, n_map, nt, a, unm, str);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_LAPLACE_SOLA - compute the coordinates using the correct laplacian
 *                  - this operator uses both the normal and tangential
 *                  - spacing ratios
 */

static void _PM_laplace_sola(int n, int nd, int *lmt, int na, double **x,
			     double *kra, double *lra, double *apk, double *apl,
			     PM_matrix *n_map, int *unm, int *str,
			     double ts, double krc, double lrc,
			     int crf, int strategy, double theta)
   {solver_method *sv;

    sv = _PM_mk_lin_sys(n, na, nd, lmt, x, strategy);

/* fill in the linear system */
    _PM_fill_lapl_opa(sv, lmt, kra, lra, apk, apl,
		      n_map, unm, str,
		      ts, krc, lrc, crf, theta);

/* solve the system */
    (*sv->solve)(sv, n_map);

    _PM_rl_lin_sys(sv);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MESH_PART - find the rest of the nodes in the block
 *              - and compute the mesh in the block
 *              - Arguments:
 *              -   RX       space for x coordinate values (i/o)
 *              -   RY       space for y coordinate values (i/o)
 *              -   NODET    node type array (i)
 *              -   REG_MAP  material region map array (i)
 *              -   N        number of nodes in this part of the mesh
 *              -   (K,L)    a logical point in this part of the mesh
 *              -   KMN      patch minimum k value
 *              -   KMX      patch maximum k value
 *              -   LMN      patch minimum l value
 *              -   LMX      patch maximum l value
 *              -   KMAX     global mesh maximum k value
 *              -   LMAX     global mesh maximum l value
 *              -   M        material region number (i)
 *              -   KR       patch wide k-ratio (i)
 *              -   LR       patch wide l-ratio (i)
 *              -   KRA      k-ratio array (i)
 *              -   LRA      l-ratio array (i)
 *              -   APK      k-product array method 2 only (i)
 *              -   APL      l-product array method 2 only (i)
 *              -   ASK      k-product magnitude start method 2 only (i)
 *              -   XSK      k-product exponent start method 2 only (i)
 *              -   AEK      k-product magnitude end method 2 only (i)
 *              -   XEK      k-product exponent end method 2 only (i)
 *              -   ASL      l-product magnitude start method 2 only (i)
 *              -   XSL      l-product exponent start method 2 only (i)
 *              -   AEL      l-product magnitude end method 2 only (i)
 *              -   XEL      l-product exponent end method 2 only (i)
 *              -   STRATEGY solver method: 1 - DSCG
 *              -                           2 - DECSOL
 *              -                           3 - ICCG
 *              -                           4 - BICG
 *              -                          -1 - default, we pick
 *              -   METHOD   generation method: 1) ratios only;
 *              -                               2) products and ratios
 *              -   CONSTR   impose mesh generation constraint
 *              -   DSPAT    spatial differencing: 0.0 -> pure 5 point
 *              -                                  1.0 -> pure 9 point
 *              -   DRAT     ratio differencing: 0.0 -> pure 5 point
 *              -                                1.0 -> pure 9 point
 *              -   ORIENT   orientation: 0.0 -> pure K orientation
 *              -                         1.0 -> pure L orientation
 */

void PM_mesh_part(double *rx, double *ry, double *nodet,
		  int *reg_map, int n, int k, int l,
		  int kmn, int kmx, int lmn, int lmx, 
		  int kmax, int lmax, int m,
		  double kr, double lr,
		  double *kra, double *lra, double *apk, double *apl,
		  double ask, double xsk, double aek, double xek,
		  double asl, double xsl, double ael, double xel,
		  int strategy, int method, int constr,
		  double dspat, double drat, double orient)
   {int na, nd, nn, zs;
    int str[9], lmt[2];
    int *unm;
    double **x;
    PM_matrix *n_map;

    zs = SC_zero_space_n(2, -1);

    nd = 2;

    lmt[0] = kmax + 1;
    lmt[1] = lmax + 1;

    str[0] = -lmt[0] - 1;
    str[1] = -lmt[0];
    str[2] = -lmt[0] + 1;
    str[3] = -1;
    str[4] = 0;
    str[5] = 1;
    str[6] = lmt[0] - 1;
    str[7] = lmt[0];
    str[8] = lmt[0] + 1;

    nn = lmt[0]*lmt[1];

/* NOTE: emperical tests show that ICCG wins big for under 4000 cells
 * and BICG wins big over 4000 cells
 */
    if (strategy == -1)
       {if (nn > 4000)
	   strategy = PM_BICG;
	else
	   strategy = PM_ICCG;};

    unm   = CMAKE_N(int, nn);
    n_map = _PM_fill_map(nodet, unm, str, n, nd, lmt);

    switch (method)

/* this method uses only the tangential spacing ratios */
       {case 1 :

	     na = 2;
	     x = CMAKE_N(double *, na);

	     x[0] = kra;
	     x[1] = lra;
	     _PM_laplace_sol(n, nd, na, lmt, x, NULL, NULL,
			     n_map, unm, str, drat, kr, lr,
			     TRUE, strategy);

	     x[0] = rx;
	     x[1] = ry;
	     _PM_laplace_sol(n, nd, na, lmt, x, kra, lra,
			     n_map, unm, str, dspat, 1.0, 1.0,
			     FALSE, strategy);
	     break;

/* this method uses both the normal and tangential spacing ratios */
        case 2 :

	     na = 4;
	     x  = CMAKE_N(double *, na);

	     x[0] = kra;
	     x[1] = lra;
	     x[2] = apk;
	     x[3] = apl;
	     _PM_laplace_sola(n, nd, lmt, na, x,
			      NULL, NULL, NULL, NULL,
			      n_map, unm, str, drat,
			      kr, lr, TRUE, strategy, orient);

	     _PM_compute_a(apk, apl, kra, lra, lmt,
			   kmn, kmx, lmn, lmx,
			   ask, xsk, aek, xek, asl, xsl, ael, xel,
			   constr);

	     na   = 2;
	     x[0] = rx;
	     x[1] = ry;
	     _PM_laplace_sola(n, nd, lmt, na, x,
			      kra, lra, apk, apl,
			      n_map, unm, str, dspat,
			      1.0, 1.0, FALSE, strategy, orient);
	     break;

        default :
	     break;};

/* map xn and yn into rx and ry with n_map */
    _PM_fin_sol(n_map, m, reg_map, nodet);

    PM_destroy(n_map);
    CFREE(unm);

    zs = SC_zero_space_n(zs, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
