/*
 * MLSOLV.C - various matrix solver routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#define SETA(ma, a, j) (ma).array = (a) + (j)*nss
#define SETV(mu, u, j) (mu).array = (u) + (j)*ns

/*--------------------------------------------------------------------------*/

/*                    SERIAL DIRECT TRIDIAGONAL SOLVER                      */

/*--------------------------------------------------------------------------*/

/* PM_TRIDI - a tridiagonal solver that can be used for a whole patch
 *            without resolving solution over distinct lines of unknowns
 *          - from Numerical Recipes in C
 *          - matrix singularity check
 *          - solves for a vector u the tridiagonal linear set given by
 *            M.u = r
 *          - M is tridiagonal with 'a' pointing to the subdiagonal, 'b'
 *            pointing to the diagonal, and 'c' pointing to the superdiagonal
 *          - 'a[0]' and 'c[n-1]' are unused
 */

int PM_tridi(double *a, double *b, double *c, double *r, double *u, int n)
   {int j;
    double bc, *t;

    if (b[0] == 0.0)
       {PM_err("B[1] = 0 ERROR - PM_TRIDI");
        return(FALSE);};

    t = CMAKE_N(double, n);

/* decomposition and forward substitution sweep */
    bc   = b[0];
    u[0] = r[0]/bc;

    for (j = 1; j < n; j++)
        {t[j] = c[j-1]/bc;
         bc   = b[j] - a[j]*t[j];
         if (bc == 0.0)
            {PM_err("CAN'T DECOMPOSE THE MATRIX - PM_TRIDI");
             CFREE(t);
             return(FALSE);};

         u[j] = (r[j] - a[j]*u[j-1])/bc;};

/* backsubstitution sweep */
    for (j = n-2; j >= 0; j--)
        u[j] -= t[j+1]*u[j+1];

    CFREE(t);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                 SERIAL DIRECT BLOCK TRIDIAGONAL SOLVER                   */

/*--------------------------------------------------------------------------*/

/* PM_BLOCK_TRIDI - a block tridiagonal solver
 *                - from Numerical Recipes in C
 *                - solves for a vector u the tridiagonal linear set given by
 *                -    M.u = r
 *                - M is tridiagonal with a the sub diagonal, b the diagonal,
 *                - and c the super diagonal
 *                - a[0] and c[n-1] are unused
 */

int PM_block_tridi(double *a, double *b, double *c, double *u,
		   int ns, int nb)
   {int j, nss;
    PM_matrix ma, mb, mc, mu, *mx, *my, *mbi;
    extern PM_matrix *_PM_inverse(PM_matrix *b, PM_matrix *a);

    nss = ns*ns;

    ma.nrow = ns;
    ma.ncol = ns;
    ma.array = a;

    mb.nrow = ns;
    mb.ncol = ns;
    mb.array = b;

    mc.nrow = ns;
    mc.ncol = ns;
    mc.array = c;

    mu.nrow = ns;
    mu.ncol = 1;
    mu.array = u;

    mbi = PM_create(ns, ns);
    mx  = PM_create(ns, ns);
    my  = PM_create(ns, 1);

/* decomposition and forward substitution sweep */
    SETA(mb, b, 0);
    if (_PM_inverse(mbi, &mb) == NULL)
       {PM_err("CAN'T INVERT B[0] - PM_BLOCK_TRIDI");
        return(FALSE);};

    SETV(mu, u, 0);
    _PM_times(&mu, mbi, &mu);

    for (j = 1; j < nb; j++)

/* compute c[j-1] = bi*c[j-1] */
        {SETA(mc, c, j-1);
         _PM_times(&mc, mbi, &mc);

/* compute b[j] = b[j] - a[j]*c[j-1] */
	 SETA(ma, a, j);
         _PM_times(mx, &ma, &mc);
	 SETA(mb, b, j);
         _PM_minus(&mb, &mb, mx);

/* compute bi = b[j]^(-1) */
         if (_PM_inverse(mbi, &mb) == NULL)
            {PM_err("CAN'T INVERT B[J] - PM_BLOCK_TRIDI");
             return(FALSE);};

/* compute u[j] = bi*(u[j] - a[j]*u[j-1]) */
         _PM_times(my, &ma, &mu);
	 SETV(mu, u, j);
         _PM_minus(&mu, &mu, my);
         _PM_times(&mu, mbi, &mu);};

/* backsubstitution sweep */
    for (j = nb-2; j >= 0; j--)

/* compute u[j] = u[j] - c[j]*u[j+1] */
        {SETA(mc, c, j);
         _PM_times(my, &mc, &mu);
	 SETV(mu, u, j);
         _PM_minus(&mu, &mu, my);};

    PM_destroy(mbi);
    PM_destroy(mx);
    PM_destroy(my);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*         SERIAL/PARALLEL DIRECT BLOCK TRIDIAGONAL N-FORMER                */

/*--------------------------------------------------------------------------*/

/* PM_BLOCK_TRIDI_NF - a block tridiagonal N-former for a line of unknowns
 *                   - to be used with block size determined by number of
 *                     locally coupled unknowns
 * NF                - Number First, number of N-form matrix a-blocks in
 *                     first row of blocks of the N-form system 
 * NB                - Number of Blocks along diagonal of N-form system
 * NL                - Number Last, number of N-form matrix c-blocks in
 *                     last row of blocks of the N-form system 
 * NS                - Number of State variables which determines the size
 *                     of each block
 * LNBRS             - Line NeighBoRS
 * SETA              - SET Array
 * SETV              - SET Vector
 */

int PM_block_tridi_nf(double *af, double *a, double *b, double *c,
		      double *u, double *cl, int ns, int nb, int nf,
		      int nl, int *lnbrs)
   {int j, nss;
    PM_matrix maf, ma, mb, mc, mcl, mu, *mw, *mx, *my, *mbi;
    extern PM_matrix *_PM_inverse(PM_matrix *b, PM_matrix *a);

    nss = ns*ns;

    maf.nrow = ns;
    maf.ncol = ns;
    maf.array = af;

    ma.nrow = ns;
    ma.ncol = ns;
    ma.array = a;

    mb.nrow = ns;
    mb.ncol = ns;
    mb.array = b;

    mc.nrow = ns;
    mc.ncol = ns;
    mc.array = c;

    mcl.nrow = ns;
    mcl.ncol = ns;
    mcl.array = cl;

    mu.nrow = ns;
    mu.ncol = 1;
    mu.array = u;

    mbi = PM_create(ns, ns);
    mw  = PM_create(ns, ns);
    mx  = PM_create(ns, ns);
    my  = PM_create(ns, 1);

/* treat special case of one row of blocks */
    if (nb == 1)
       {SETA(mb, b, 0);
        _PM_inverse(mbi, &mb);
        if (_PM_inverse(mbi, &mb) == NULL)
           {PM_err("CAN'T INVERT B[0] - PM_BLOCK_TRIDI_NF");
            return(FALSE);};
        SETV(mu, u, 0);
        _PM_times(&mu, mbi, &mu);

        if (lnbrs[0] == TRUE)
/* compute a[0] = bi*a[0] */
           {SETA(ma, a, 0);
            _PM_times(&ma, mbi, &ma);

/* compute af[j] = bi*af[j] */
            for (j = 0; j < nf; j++)
              {SETA(maf, af, j);
               _PM_times(&maf, mbi, &maf);};};

        if (lnbrs[1] == TRUE)
/* compute c[0] = bi*c[0] */
           {SETA(mc, c, 0);
            _PM_times(&mc, mbi, &mc);

/* compute cl[j] = bi*cl[j] */
            for (j = 0; j < nl; j++)
              {SETA(mcl, cl, j);
               _PM_times(&mcl, mbi, &mcl);};};

        return(TRUE);};

/* forward elimination of subdiagonal blocks */
    if (lnbrs[0] == TRUE)

/* compute bi = b[1]^(-1) */
       {SETA(mb, b, 1);
        if (_PM_inverse(mbi, &mb) == NULL)
           {PM_err("CAN'T INVERT B[1] - PM_BLOCK_TRIDI_NF");
            return(FALSE);};
        SETA(ma, a, 1);
        _PM_times(&ma, mbi, &ma);
        SETV(mu, u, 1);
        _PM_times(&mu, mbi, &mu);

        for (j = 2; j < nb; j++)

/* compute c[j-1] = bi*c[j-1] */
            {SETA(mc, c, j-1);
             _PM_times(&mc, mbi, &mc);

/* compute w = a[j]*a[j-1] */
             PM_copy(mw, &ma);
             SETA(ma, a, j);
             _PM_times(mw, &ma, mw);

/* compute b[j] = b[j] - a[j]*c[j-1] */
             _PM_times(mx, &ma, &mc);
	     SETA(mb, b, j);
             _PM_minus(&mb, &mb, mx);

/* compute bi = b[j]^(-1) */
             if (_PM_inverse(mbi, &mb) == NULL)
                {PM_err("CAN'T INVERT B[J] - PM_BLOCK_TRIDI_NF");
                 return(FALSE);};

/* compute u[j] = bi*(u[j] - a[j]*u[j-1]) */
             _PM_times(my, &ma, &mu);
	     SETV(mu, u, j);
             _PM_minus(&mu, &mu, my);
             _PM_times(&mu, mbi, &mu); 

/* compute a[j] = -bi*a[j]*a[j-1], growing lower leg of N-form matrix */
             _PM_negative_times(&ma, mbi, mw);};}

    else
      
/* compute bi = b[0]^(-1) */
       {SETA(mb, b, 0);
        if (_PM_inverse(mbi, &mb) == NULL)
           {PM_err("CAN'T INVERT B[0] - PM_BLOCK_TRIDI_NF");
            return(FALSE);};
        SETV(mu, u, 0);
        _PM_times(&mu, mbi, &mu);
      
        for (j = 1; j < nb; j++)

/* compute c[j-1] = bi*c[j-1] */
            {SETA(mc, c, j-1);
             _PM_times(&mc, mbi, &mc);

/* compute b[j] = b[j] - a[j]*c[j-1] */
	     SETA(ma, a, j);
             _PM_times(mx, &ma, &mc);
	     SETA(mb, b, j);
             _PM_minus(&mb, &mb, mx);

/* compute bi = b[j]^(-1) */
             if (_PM_inverse(mbi, &mb) == NULL)
                {PM_err("CAN'T INVERT B[J] - PM_BLOCK_TRIDI_NF");
                 return(FALSE);};

/* compute u[j] = bi*(u[j] - a[j]*u[j-1]) */
             _PM_times(my, &ma, &mu);
             SETV(mu, u, j);
             _PM_minus(&mu, &mu, my);
             _PM_times(&mu, mbi, &mu);};};

    if (lnbrs[1] == TRUE)

/* compute c[nb-1] = bi*c[nb-1] */
       {SETA(mc, c, nb-1);
        _PM_times(&mc, mbi, &mc);

/* compute cl[j] = bi*cl[j] */
        for (j = 0; j < nl; j++)
          {SETA(mcl, cl, j);
           _PM_times(&mcl, mbi, &mcl);};

        SETA(mc, c, nb-2);
        SETV(mu, u, nb-2);

        if (lnbrs[0] == TRUE) 

           {SETA(ma, a, nb-2);

/* backward elimination of superdiagonal blocks */
            for (j = nb-3; j > 0; j--)

/* set w = c[j+1] */
                {PM_copy(mw, &mc);

/* compute u[j] = u[j] - c[j]*u[j+1] */
                 SETA(mc, c, j);
                 _PM_times(my, &mc, &mu);
                 SETV(mu, u, j);
                 _PM_minus(&mu, &mu, my);

/* compute a[j] = a[j] - c[j]*a[j+1] */
                 _PM_times(mx, &mc, &ma);
                 SETA(ma, a, j);
                 _PM_minus(&ma, &ma, mx);

/* compute c[j] = -c[j]*c[j+1], growing upper leg of N-form matrix */
                 _PM_negative_times(&mc, &mc, mw);};}

        else

/* backward elimination of superdiagonal blocks */
           {for (j = nb-3; j >= 0; j--)

/* set w = c[j+1] */
                {PM_copy(mw, &mc);

/* compute u[j] = u[j] - c[j]*u[j+1] */
                 SETA(mc, c, j);
                 _PM_times(my, &mc, &mu);
                 SETV(mu, u, j);
                 _PM_minus(&mu, &mu, my);

/* compute c[j] = -c[j]*c[j+1], growing upper leg of N-form matrix */
                 _PM_negative_times(&mc, &mc, mw);};};}

    else

       {SETV(mu, u, nb-1);

        if (lnbrs[0] == TRUE)

           {SETA(ma, a, nb-1);

/* backward elimination of superdiagonal blocks */
            for (j = nb-2; j > 0; j--)

/* compute u[j] = u[j] - c[j]*u[j+1] */
                {SETA(mc, c, j);
                 _PM_times(my, &mc, &mu);
                 SETV(mu, u, j);
                 _PM_minus(&mu, &mu, my);

/* compute a[j] = a[j] - c[j]*a[j+1] */
                 _PM_times(mx, &mc, &ma);
                 SETA(ma, a, j);
                 _PM_minus(&ma, &ma, mx);};}

        else

/* backward elimination of superdiagonal blocks */
           {for (j = nb-2; j >= 0; j--)

/* compute u[j] = u[j] - c[j]*u[j+1] */
                {SETA(mc, c, j);
                 _PM_times(my, &mc, &mu);
	         SETV(mu, u, j);
                 _PM_minus(&mu, &mu, my);};};};

    if (lnbrs[0] == TRUE)

       {if (nb == 2 && lnbrs[1])

/* compute bi = b[0]^(-1) */
           {SETA(mb, b, 0);
            if (_PM_inverse(mbi, &mb) == NULL)
               {PM_err("CAN'T INVERT B[0] - PM_BLOCK_TRIDI_NF");
                return(FALSE);};
            _PM_inverse(mbi, &mb);

/* compute a[0] = bi*a[0] */
            SETA(ma, a, 0);
            _PM_times(&ma, mbi, &ma);

/* compute af[j] = bi*af[j] */
            for (j = 0; j < nf; j++)
              {SETA(maf, af, j);
               _PM_times(&maf, mbi, &maf);};

/* compute c[0] = bi*c[0] */
            SETA(mc, c, 0);
            _PM_times(&mc, mbi, &mc);

/* compute u[0] = bi*u[0] */
            SETV(mu, u, 0);
            _PM_times(&mu, mbi, &mu);}

        else

/* compute b[0] = b[0] - c[0]*a[1] */
           {SETA(mc, c, 0);
            SETA(ma, a, 1);
            _PM_times(mx, &mc, &ma);
            SETA(mb, b, 0);
            _PM_minus(&mb, &mb, mx);

/* compute bi = b[0]^(-1) */
            _PM_inverse(mbi, &mb);

/* compute a[0] = bi*a[0] */
            SETA(ma, a, 0);
            _PM_times(&ma, mbi, &ma);

/* compute af[j] = bi*af[j] */
            for (j = 0; j < nf; j++)
              {SETA(maf, af, j);
               _PM_times(&maf, mbi, &maf);};

/* compute u[0] = bi*(u[0] - c[0]*u[1]) */
            SETV(mu, u, 1);
            _PM_times(my, &mc, &mu);
            SETV(mu, u, 0);
            _PM_minus(&mu, &mu, my);
            _PM_times(&mu, mbi, &mu);

            if (lnbrs[1] == TRUE)

/* compute c[0] = -bi*c[0]*c[1] */
               {SETA(mc, c, 1);
                PM_copy(mx, &mc);
                SETA(mc, c, 0);
                _PM_negative_times(&mc, &mc, mx); 
                _PM_times(&mc, mbi, &mc);};};};

/* block-tridiagonal system is now ready to be decoupled if necessary;
 * if it was not coupled, then it has been solved */

    PM_destroy(mbi);
    PM_destroy(mw);
    PM_destroy(mx);
    PM_destroy(my);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*              FUNCTION FOR REDUCED SYSTEM ELIMINATION                     */

/*--------------------------------------------------------------------------*/

/* PM_BLOCK_RSYS_ELIM - block tridiagonal reduced system skip-decoupling
 *                      solver
 *                    - for block tridiagonal systems spanning multiple
 *                      patches at a given level, this function can be
 *                      called at the beginning of the block tridiagonal
 *                      solution
 *                    - to be used with block size determined by number of
 *                      coupled unknowns
 *                    - ghost coefficient and unknown blocks are assumed to
 *                      exist in block locations [0] and [nb+1]
 * SETA               - SET Array
 * SETV               - SET Vector
 * NS                 - Number of State variables
 * NB                 - Number of Blocks
 * LNBRS              - Line NeighBoRS
 */

int PM_block_rsys_elim(double *a, double *b, double *c, double *u,
		       int ns, int nb, int *elim_flag)
   {int nss;
    PM_matrix ma, mb, mc, mu, *mx, *my, *mbi;
    extern PM_matrix *_PM_inverse(PM_matrix *b, PM_matrix *a);

    nss = ns*ns;

    ma.nrow = ns;
    ma.ncol = ns;
    ma.array = a;

    mb.nrow = ns;
    mb.ncol = ns;
    mb.array = b;

    mc.nrow = ns;
    mc.ncol = ns;
    mc.array = c;

    mu.nrow = ns;
    mu.ncol = 1;
    mu.array = u;

    mbi = PM_create(ns, ns);
    mx  = PM_create(ns, ns);
    my  = PM_create(ns, 1);

/* elimination of subdiagonal blocks in reduced system */
    if (elim_flag[0] == TRUE)
       {SETA(mc, c, 0);
        SETV(mu, u, 0);
        SETA(ma, a, 1);
        SETA(mb, b, 1);
        _PM_times(mx, &ma, &mc);
        _PM_times(my, &ma, &mu);
        PM_zero(&ma);
        _PM_minus(&mb, &mb, mx);
/* compute bi = b[1]^(-1) */
        if (_PM_inverse(mbi, &mb) == NULL)
           {PM_err("CAN'T INVERT B[1] - PM_BLOCK_RSYS_ELIM");
            return(FALSE);};
        PM_ident(&mb);
        SETA(mc, c, 1);
        _PM_times(&mc, mbi, &mc);
        SETV(mu, u, 1);
        _PM_minus(&mu, &mu, my);
        _PM_times(&mu, mbi, &mu);

        SETA(mc, c, 1);
        SETV(mu, u, 1);
        SETA(ma, a, nb-1);
        SETA(mb, b, nb-1);
        _PM_times(mx, &ma, &mc);
        _PM_times(my, &ma, &mu);
        PM_zero(&ma);
        _PM_minus(&mb, &mb, mx);
/* compute bi = b[nb-1]^(-1) */
        if (_PM_inverse(mbi, &mb) == NULL)
           {PM_err("CAN'T INVERT B[NB-1] - PM_BLOCK_RSYS_ELIM");
            return(FALSE);};
        PM_ident(&mb);
        SETA(mc, c, nb-1);
        _PM_times(&mc, mbi, &mc);
        SETV(mu, u, nb-1);
        _PM_minus(&mu, &mu, my);
        _PM_times(&mu, mbi, &mu);};

/* elimination of superdiagonal blocks in reduced system */
    if (elim_flag[1] == TRUE)
       {SETA(ma, a, nb);
        SETV(mu, u, nb);
        SETA(mb, b, nb-1);
        SETA(mc, c, nb-1);
        _PM_times(mx, &mc, &ma);
        _PM_times(my, &mc, &mu);
        PM_zero(&mc);
        _PM_minus(&mb, &mb, mx);
/* compute bi = b[nb-1]^(-1) */
        if (_PM_inverse(mbi, &mb) == NULL)
           {PM_err("CAN'T INVERT B[NB-1] - PM_BLOCK_RSYS_ELIM");
            return(FALSE);};
        PM_ident(&mb);
        SETA(ma, a, nb-1);
        _PM_times(&ma, mbi, &ma);
        SETV(mu, u, nb-1);
        _PM_minus(&mu, &mu, my);
        _PM_times(&mu, mbi, &mu);

        SETA(mc, a, nb-1);
        SETV(mu, u, nb-1);
        SETA(mb, b, 1);
        SETA(ma, c, 1);
        _PM_times(mx, &mc, &ma);
        _PM_times(my, &mc, &mu);
        PM_zero(&mc);
        _PM_minus(&mb, &mb, mx);
/* compute bi = b[1]^(-1) */
        if (_PM_inverse(mbi, &mb) == NULL)
           {PM_err("CAN'T INVERT B[1] - PM_BLOCK_RSYS_ELIM");
            return(FALSE);};
        PM_ident(&mb);
        SETA(ma, a, 1);
        _PM_times(&ma, mbi, &ma);
        SETV(mu, u, 1);
        _PM_minus(&mu, &mu, my);
        _PM_times(&mu, mbi, &mu);};

    PM_destroy(mbi);
    PM_destroy(mx);
    PM_destroy(my);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                  FUNCTION FOR N-FORM BACKSUBSTITUTION                    */

/*--------------------------------------------------------------------------*/

/* PM_BLOCK_NF_BACKSUB - do back substitution of reduced system unknowns */

int PM_block_nf_backsub(double *a, double *c, double *u,
			int ns, int nb, int *lnbrs)
   {int j, nss;
    PM_matrix ma, mc, mu, *my;

    nss = ns*ns;

    ma.nrow = ns;
    ma.ncol = ns;
    ma.array = a;

    mc.nrow = ns;
    mc.ncol = ns;
    mc.array = c;

    mu.nrow = ns;
    mu.ncol = 1;
    mu.array = u;

    my  = PM_create(ns, 1);

    if (lnbrs[0] == TRUE)

        if (lnbrs[1] == TRUE)

            for (j = 1; j < nb-1; j++)

/* compute u[j] = u[j] - a[j]*u[1] - c[j]*u[nb] */
                {SETA(ma, a, j);
	         SETV(mu, u, 0);
                 PM_copy(my, &mu);
                 _PM_times(my, &ma, my);
	         SETV(mu, u, j);
                 _PM_minus(&mu, &mu, my);

                 SETA(mc, c, j);
                 SETV(mu, u, nb-1);
                 PM_copy(my, &mu);
                 _PM_times(my, &mc, my);
                 SETV(mu, u, j);
                 _PM_minus(&mu, &mu, my); }
 
        else

            for (j = 1; j < nb-1; j++)

/* compute u[j] = u[j] - a[j]*u[1] */
                {SETA(ma, a, j);
	         SETV(mu, u, 0);
                 PM_copy(my, &mu);
                 _PM_times(my, &ma, my);
	         SETV(mu, u, j);
                 _PM_minus(&mu, &mu, my);}

    else

        if (lnbrs[1] == TRUE)

            for (j = 1; j < nb-1; j++)

/* compute u[j] = u[j] - c[j]*u[nb] */
                {SETA(mc, c, j);
                 SETV(mu, u, nb-1);
                 PM_copy(my, &mu);
                 _PM_times(my, &mc, my);
                 SETV(mu, u, j);
                 _PM_minus(&mu, &mu, my);}
           
    PM_destroy(my);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                          LU DECOMPOSE SOLVERS                            */

/*--------------------------------------------------------------------------*/

/* PM_SOLVE - solve systems of linear equations, A.X = B
 *          - A is an (NxN) matrix of coefficients,
 *          - B is matrix of values (typically NxM).
 *          - M is the number of systems to solve
 *          - Based on discussion in "Computer Solution of Linear Algebraic 
 *            Systems" Forsythe, Moler. p68 ff
 */

PM_matrix *PM_solve(PM_matrix *a, PM_matrix *b)
   {int *ips;
    int nr;
    PM_matrix *lu;

    if ((a == NULL) || (b == NULL))
       {PM_err("NULL MATRIX - PM_SOLVE");
        return(NULL);};
        
    if ((a->nrow) != (a->ncol))
       {PM_err("NON-SQUARE MATRIX - PM_SOLVE");
        return(NULL);};

/* allocate the pivot array */
    nr  = a->nrow;
    ips = CMAKE_N(int, nr);

/* do the lu decomposition */
    lu = PM_decompose(a, ips, FALSE);

/* solve all of the systems */
    if (lu != NULL)
       PM_sol(lu, b, ips, FALSE);

/* release the intermediate storage */
    CFREE(ips);

    return(b);}
                
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DECOMP - perform an LU decompostion on the matrix a and produce
 *           - the pivoting information in ips
 *           - the flag is used to control whether or not a new matrix
 *           - is made and returned (FALSE don't create new space)
 *           - return the number of pivots via PNC
 */

PM_matrix *PM_decomp(PM_matrix *a, int *ips, int flag, int *pnc)
   {int n, ipv, ns, nc;
    int j, i, k;
    double *ula, *ulb, *ulc, *uld;
    double *ala, *scales, big, size, alp, bet;
    PM_matrix *ul;

    n   = a->nrow;
    ipv = 0.0;
    nc  = 0;

/* if a new matrix is requested, allocate it and copy the original to it */
    if (flag == TRUE)
       {ul  = PM_create(n, n);
        ula = ul->array;
        ala = a->array;
        ns  = n*n;
        for (i = 0; i < ns; i++, *(ula++) = *(ala++));}
    else
       ul = a;

    scales = CMAKE_N(double, n);
    ula    = ul->array;

    for (i = 0; i < n; i++)
        {ulb = ula + i*n;
         big = 0.0;
         for (j = 0; j < n; j++)
             {size = ABS(ulb[j]);
              if (size > big)
                 big = size;};

         if (big == 0.0)
            {PM_err("SINGULAR MATRIX - PM_DECOMP");
             return(NULL);};

         scales[i] = 1.0/big;};

/* loop over columns */
    for (j = 0; j < n; j++)
        {ulb = ula + j;

/* begin super-diagonal element, beta(i,j) */
         for (i = 0; i < j; i++)
             {ulc = ula + i*n;
              bet = ulc[j];
              for (k = 0; k < i; k++)
                  bet -= ulc[k]*ulb[k*n];
              ulc[j] = bet;};

         big = 0.0;

/* begin the sub-diagonal element, alpha(i,j) */
         for (i = j; i < n; i++)
             {ulc = ula + i*n;
              alp = ulc[j];
              for (k = 0; k < j; k++)
                  alp -= ulc[k]*ulb[k*n];
              ulc[j] = alp;

/* check for the largest pivot element */
              size = scales[i]*ABS(alp);
              if (size >= big)
                 {big = size;
                  ipv = i;};};

/* pivot if necessary */
         if (j != ipv)
            {ulc = ula + ipv*n;
             uld = ula + j*n;
             for (k = 0; k < n; k++)
                 {size   = ulc[k];
                  ulc[k] = uld[k];
                  uld[k] = size;};

             scales[ipv] = scales[j];
	     nc++;};

         ips[j] = ipv;
         if (ulb[j*n] == 0.0)
            ulb[j*n] = SMALL;

/* normalize the sub-diagonal terms */
         if (j != n-1)
            {size = 1.0/(ulb[j*n]);
             for (i = j+1; i < n; i++)
                 ulb[i*n] *= size;};};

    *pnc = nc;

    CFREE(scales);

    return(ul);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DECOMPOSE - perform an LU decompostion on the matrix a and produce
 *              - the pivoting information in ips
 *              - the flag is used to control whether or not a new matrix
 *              - is made and returned (FALSE don't create new space)
 */

PM_matrix *PM_decompose(PM_matrix *a, int *ips, int flag)
   {int nc;
    PM_matrix *rv;

    rv = PM_decomp(a, ips, flag, &nc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DETERMINANT - return the value of the determinant of the given
 *                - square matrix
 */

double PM_determinant(PM_matrix *a)
   {int i, n, nc;
    int *ips;
    double det;
    PM_matrix *ul;

    n   = a->nrow;
    ips = CMAKE_N(int, n);

    ul = PM_decomp(a, ips, TRUE, &nc);
    if (ul == NULL)
       det = 0.0;

    else
       {det = (nc & 1) ? -1.0 : 1.0;
	for (i = 1; i <= n; i++)
	    det *= PM_element(ul, i, i);

	PM_destroy(ul);};

    CFREE(ips);

    return(det);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SOL - using the pivoting information complete the solution of
 *        - the linear system begun in PM_DECOMPOSE
 *        - the flag is used to control whether or not a new matrix
 *        - is made and returned (FALSE - no new array)
 */

PM_matrix *PM_sol(PM_matrix *ul, PM_matrix *b, int *ips, int flag)
   {int j, i, ih, ip, is, m, n, nh;
    double sum;
    double *xa, *ba, *ua, *ub, *px;
    PM_matrix *x;

    n  = ul->nrow;
    ua = ul->array;

/* NH is the number of RHS vectors or systems to solve */
    nh = b->ncol;

    if (flag == TRUE)
       {x  = PM_create(n, nh);
        xa = x->array;
        ba = b->array;
	m  = nh*n;
        for (i = 0; i < m; i++)
            xa[i] = ba[i];}
    else
       {x  = b;
        xa = b->array;};

/* solve all of the systems */
    for (ih = 0; ih < nh; ih++)
        {px = xa + ih*n;

	 is = -1;
	 for (i = 0; i < n; i++)
	     {ub     = ua + i*n;
	      ip     = ips[i];
	      sum    = px[ip];
	      px[ip] = px[i];
	      if (is >= 0)
		 for (j = is; j <= i-1; j++)
		     sum -= ub[j]*px[j];
	      else if (sum != 0.0)
		 is = i;

	      px[i] = sum;};

	 for (i = n-1; i >= 0; i--)
	     {ub  = ua + i*n;
	      sum = px[i];
	      for (j = i+1; j < n; j++)
		  sum -= ub[j]*px[j];

	      px[i] = sum/ub[i];};};

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_INVERSE - find B = A^-1 */

PM_matrix *_PM_inverse(PM_matrix *b, PM_matrix *a)
   {int j, i;
    int row, col;
    int *ips;
    PM_matrix *lu, *x, *t;

    row = a->nrow;
    col = a->ncol;
    if (row != col)
       {PM_err("NON-SQUARE MATRIX - _PM_INVERSE");
        return(NULL);};

    ips = CMAKE_N(int, row);
    lu  = PM_decompose(a, ips, TRUE);
    if (lu == NULL)
       {PM_err("SINGULAR MATRIX - _PM_INVERSE");
	CFREE(ips);
        return(NULL);};

    t = PM_create(row, 1);

    for (j = 1; j <= row; j++)
        {for (i = 1; i <= row; i++)
             PM_element(t, i, 1) = ((i == j) ? 1 : 0);

         x = PM_sol(lu, t, ips, FALSE);
         for (i = 1; i <= row; i++)
             PM_element(b, i, j) = PM_element(x, i, 1);};

/* release the temporary storage */
    PM_destroy(t);
    PM_destroy(lu);
    CFREE(ips);
        
    return(b);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INVERSE - invert the given matrix a */

PM_matrix *PM_inverse(PM_matrix *a)
   {int row, col;
    PM_matrix *rv;

    rv = NULL;

    row = a->nrow;
    col = a->ncol;
    if (row != col)
       PM_err("NON-SQUARE MATRIX - PM_INVERSE");

    else
       rv = _PM_inverse(PM_create(row, col), a);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_UPPER - return a matrix composed of the super diagonal part of a */

PM_matrix *PM_upper(PM_matrix *a)
   {int i, j, ncol, nrow;
    PM_matrix *u;

    ncol = a->ncol;
    nrow = a->nrow;

    u = PM_create(nrow, ncol);

    for (i = 1; i <= nrow; i++)
        for (j = 1; j <= ncol; j++)
            {if (j >= i)
                PM_element(u, i, j) = PM_element(a, i, j);
             else
                PM_element(u, i, j) = 0;};

    return(u);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LOWER - return a matrix composed of the sub diagonal part of a */

PM_matrix *PM_lower(PM_matrix *a)
   {int i, j, ncol, nrow;
    PM_matrix *l;

    ncol = a->ncol;
    nrow = a->nrow;

    l = PM_create(ncol, nrow);

    for (i = 1; i <= nrow; i++)
        for (j = 1; j <= ncol; j++)
            {if (j < i)
                PM_element(l, i, j) = PM_element(a, i, j);
             else if (j == i)
                PM_element(l, i, j) = 1;
             else
                PM_element(l, i, j) = 0;};

    return(l);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DECB - banded LU decomposer
 *
 * This subroutine constructs the LU decomposition of a band matrix a
 * in the form L*U = P*A , where P is a permutation matrix, L is a
 * unit lower triangular matrix, and U is an upper triangular matrix.
 * the matrix A is assumed here to be stored by rows in the array B.
 * mdim  =  the first dimension (column length) of the array B.
 *          mdim must be at least 2*ml+mu+1.
 * n     =  order of matrix.
 * ml,mu =  widths of the lower and upper parts of the band, not
 *          counting the main diagonal. total bandwidth is ml+mu+1.
 * B     =  mdim by n array containing the matrix A on input
 *          and its factored form on output.
 *          On input, A is stored by rows in B as an (ml+mu+1) by n
 *          array (the rows of A are the columns of B), with a column
 *          length of mdim for B.  Thus B(k,i) for 1 <= k <= ml+mu+1
 *          contains the part of the i-th row of A within the band.
 *          That is, B(j-i+ml+1,i) contains A(i,j) for -ml <= (j-i) <= mu.
 *          On output, B contains the L and U factors, with U in rows
 *          1 to ml+mu+1, and L in rows ml+mu+2 to 2*ml+mu+1.
 *          the diagonal of L is not stored, and the reciprocals
 *          of the diagonal elements of U are stored.
 * ip    =  array of length n containing pivot information on output.
 *
 * The input arguments are: n, ml, mu, and B.
 * The output arguments are: B, ip.
 *
 *  reference: A. C. Hindmarsh, Banded Linear Systems with Pivoting,
 *  Lawrence Livermore Laboratory Report UCID-30060, rev. 1, May 1976.
 */

PM_matrix *PM_decb(int n, int ml, int mu, PM_matrix *b, int *ip)
   {int ll, n1, k, lr, mx, np, kk;
    int ii, j, i, nr;
    double xm, xx;

    if (n != 1)
       {ll = ml + mu + 1;
        n1 = n - 1;

/* initialize storage. shift and zero out elements of b. */
        if (ml != 0)
           {for (i = 1; i <= ml; i++)
                {ii = mu + i;
                 k = ml + 1 - i;
                 for (j = 1; j <= ii; j++)
                     PM_element(b, j, i) = PM_element(b, j+k, i);
                 k = ii + 1;
                 for (j = k; j <= ll; j++)
                     PM_element(b, j, i) = 0.0;};};
        lr = ml;
        for (nr = 1; nr <= n1; nr++)
            {np = nr + 1;
             if (lr != n)
                lr++;

/*  find the pivot in column nr, within the band, at or below row nr. */
             mx = nr;
             xm = ABS(PM_element(b, 1, nr));
             if (ml != 0)
                for (i = np; i <= lr; i++)
                    {if (ABS(PM_element(b, 1, i)) <= xm)
                        continue;
                     mx = i;
                     xm = ABS(PM_element(b, 1, i));};
             ip[nr-1] = mx;

/* interchange rows nr and mx. */
             if (mx != nr)
                for (i = 1; i <= ll; i++)
                    {xx = PM_element(b, i, nr);
                     PM_element(b, i, nr) = PM_element(b, i, mx);
                     PM_element(b, i, mx) = xx;};
             xm = PM_element(b, 1, nr);

/* construct elements of l and u. */
             if (xm == 0.0)
                {PM_err("SINGULAR MATRIX - PM_DECB");
                 return(NULL);};

             PM_element(b, 1, nr) = 1.0/xm;
             if (ml != 0)
                {xm = -PM_element(b, 1, nr);
                 kk = min(n-nr, ll-1);
                 for (i = np; i <= lr; i++)
                     {j = ll + i - nr;
                      xx = PM_element(b, 1, i)*xm;
                      PM_element(b, j, nr) = xx;
                      for (ii = 1; ii <= kk; ii++)
                          PM_element(b, ii, i) = PM_element(b, ii+1, i) +
                                                xx*PM_element(b, ii+1, nr);
                      PM_element(b, ll, i) = 0.0;};};};};
    nr = n;
    if (PM_element(b, 1, n) == 0.0)
       {PM_err("ZERO ELEMENT SINGULAR MATRIX - PM_DECB");
        return(NULL);};

    PM_element(b, 1, n) = 1.0/PM_element(b, 1, n);

    return(b);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SOLB - banded solver with PM_DECB
 *
 *  this subroutine computes the solution of the banded linear system
 *  a*x = c , given the lu decomposition of the matrix a from decbr.
 *  y    =  right-hand vector c, of length n, on input,
 *       =  solution vector x on output.
 *  all other arguments are as described in decbr.
 *  all the arguments are input arguments.
 *  the output argument is  y.
 *
 *  reference.. a. c. hindmarsh, banded linear systems with pivoting,
 *  lawrence livermore laboratory report ucid-30060, rev. 1, may 1976.
 *
 */

PM_matrix *PM_solb(int n, int ml, int mu,
		   PM_matrix *b, PM_matrix *y, int *ips)
   {int n1, nrm1, idummy;
    int i, nr, nb, ll, ip, kk;
    double dp, xx;

    if (n != 1)
       {n1 = n - 1;
        ll = ml + mu + 1;
/*
 * if ml = 0, skip the first backsolution phase.
 * apply permutation and l multipliers to y.
 */
        if (ml != 0)
           {for (nr = 1; nr <= n1; nr++)
                {nrm1 = nr-1;
                 if (ips[nrm1] != nr)
                    {ip = ips[nrm1];
                     xx = PM_element(y, nr, 1);
                     PM_element(y, nr, 1) = PM_element(y, ip, 1);
                     PM_element(y, ip, 1) = xx;};
                 kk = min(n-nr, ml);
                 for (i = 1; i <= kk; i++)
                     PM_element(y, nr+i, 1) = PM_element(y, nr+i, 1) +
                                              PM_element(y, nr, 1) *
                                              PM_element(b, ll+i, nr);};};
        ll = ll - 1;

/* backsolve for x using u. */
        PM_element(y, n, 1) = PM_element(y, n, 1) * PM_element(b, 1, n);
        kk = 0;
        for (nb = 1; nb <= n1; nb++)
           {nr = n - nb;
            if (kk != ll)
               kk = kk + 1;
            dp = 0.0;
            if (ll == 0)
               continue;
            idummy = 0;
            for (i = 1; i <= kk; i++)
                {idummy = idummy + i*i;
                 dp = dp + PM_element(b, i+1, nr)*PM_element(y, nr+i, 1);};
            PM_element(y, nr, 1) = (PM_element(y, nr, 1) - dp) *
                                  PM_element(b, 1, nr);};
        return(y);};

    PM_element(y, 1, 1) = PM_element(y, 1, 1)*PM_element(b, 1, 1);

    return(y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_PRINT - print the given matrix a */

PM_matrix *PM_print(PM_matrix *a)
   {int i, j, ncol, nrow;

    if (a == NULL)
       {PM_err("NULL MATRIX - PM_PRINT");
        return(NULL);};

    ncol = a->ncol;
    nrow = a->nrow;

    for (i = 1; i <= nrow; i++)
        {for (j = 1; j <= ncol; j++)
             printf("%10.3e ", PM_element(a, i, j));
         printf("\n");};

    printf("\n");

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
