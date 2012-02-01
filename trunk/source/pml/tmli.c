/*
 * TMLI.C - driver to study the interpolation routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_1D_POINTS - setup NL points at which functions will be evaluated */

double *setup_1d_points(int nl)
   {int l;
    double r0;
    double *r;

    r = CMAKE_N(double, nl);

    for (l = 0; l < nl; l++)
        {r0 = ((double) l)/(nl - 1.0);
	 r[l] = 0.2 + 0.5*(r0 + cos(PI*r0));};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SETUP_2D_POINTS - setup NL points at which functions will be evaluated */

double **setup_2d_points(int nl)
   {int l;
    double r0;
    double **r;

    r  = PM_make_vectors(2, nl);

    for (l = 0; l < nl; l++)
        {r0 = ((double) l)/(nl - 1.0);
	 r[0][l] = 0.2 + 0.5*(r0 + cos(PI*r0));
         r[1][l] = 0.2 + 0.5*(r0 + sin(PI*r0));};

    return(r);}

/*--------------------------------------------------------------------------*/

/*                             TEST_1 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_1 - test 1d linear spline interpolation */

int test_1(void)
   {int i, l, nl, nx, rv;
    double inx;
    double *f, *x, *y, *r;
    FILE *out;

    nl = 20;
    nx = 10;

    inx = 1.0/(nx - 1.0);

    x = CMAKE_N(double, nx);
    f = CMAKE_N(double, nx);

    for (i = 0; i < nx; i++)

/* setup the coordinates of the logical values */
        {x[i] = i*inx;

/* setup the logical values */
	 f[i] = exp(i*inx);};

/* setup the interpolation points */
    r = setup_1d_points(nl);
    y = CMAKE_N(double, nl);

/* compute Y(R) */
    for (l = 0; l < nl; l++)
        y[l] = PM_linear_int(x, f, nx, r[l]);

/* print the results */
    out = fopen("test_1.out", "w");
    if (out == NULL)
       {printf("CAN'T OPEN TEST_1.OUT\n");
        rv = FALSE;}

    else
       {fprintf(out, "#   I         X             F\n");
	for (i = 0; i < nx; i++)
	    fprintf(out, " %4d   %12.6e   %12.6e\n",
		    i, x[i], f[i]);

	fprintf(out, "#   I        R            FI\n");
	for (l = 0; l < nl; l++)
	    fprintf(out, " %4d   %12.6e   %12.6e\n",
		    l, r[l], y[l]);

	fclose(out);

	rv = TRUE;};

    CFREE(x);
    CFREE(f);
    CFREE(r);
    CFREE(y);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             TEST_2 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_2 - test 1d cubic spline interpolation */

int test_2(void)
   {int i, l, nl, nx, rv;
    double inx;
    double *df, *f, *x, *y, *r;
    FILE *out;

    nl = 20;
    nx = 10;

    inx = 1.0/(nx - 1.0);

    x = CMAKE_N(double, nx);
    f = CMAKE_N(double, nx);

    for (i = 0; i < nx; i++)

/* setup the coordinates of the logical values */
        {x[i] = i*inx;

/* setup the logical values */
	 f[i] = exp(i*inx);};

/* setup the interpolation points */
    r = setup_1d_points(nl);
    y = CMAKE_N(double, nl);

/* compute Y(R) */
    df = PM_compute_splines(x, f, nx, HUGE, HUGE);
    for (l = 0; l < nl; l++)
        y[l] = PM_cubic_spline_int(x, f, df, nx, r[l]);

/* print the results */
    out = fopen("test_2.out", "w");
    if (out == NULL)
       {printf("CAN'T OPEN TEST_2.OUT\n");
        rv = FALSE;}

    else
       {fprintf(out, "#   I         X             F\n");
	for (i = 0; i < nx; i++)
	    fprintf(out, " %4d   %12.6e   %12.6e\n",
		    i, x[i], f[i]);

	fprintf(out, "#   I        R            FI\n");
	for (l = 0; l < nl; l++)
	    fprintf(out, " %4d   %12.6e   %12.6e\n",
		    l, r[l], y[l]);

	fclose(out);

	rv = TRUE;};

    CFREE(x);
    CFREE(f);
    CFREE(df);
    CFREE(r);
    CFREE(y);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             TEST_3 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_3 - test 2d interpolation on LR mesh */

int test_3(void)
   {int i, j, l, nl, np, nx, ny, rv;
    double inx, iny;
    double **f, **x, **r, **vals, **fncs;
    PM_lagrangian_mesh *grid;
    FILE *out;

    nl = 20;
    nx = 10;
    ny = 15;
    np = nx*ny;

    inx = 1.0/(nx - 1.0);
    iny = 1.0/(ny - 1.0);

    x = PM_make_vectors(2, np);
    f = PM_make_vectors(2, np);

    for (j = 0; j < ny; j++)
        for (i = 0; i < nx; i++)
	    {l = j*nx + i;

/* setup the coordinates of the logical values */
	     x[0][l] = i*inx;
	     x[1][l] = j*iny;

/* setup the logical values */
	     f[0][l] = i*inx + (ny - 1.0 - j)*iny;
	     f[1][l] = (nx - 1.0 - i)*inx + j*iny;};

/* get in the number of interpolation points */
    r = setup_2d_points(nl);

    grid       = CMAKE(PM_lagrangian_mesh);
    grid->x    = x[0];
    grid->y    = x[1];
    grid->kmax = nx;
    grid->lmax = ny;

    fncs    = CMAKE_N(double *, 4);
    fncs[0] = x[0];
    fncs[1] = x[1];
    fncs[2] = f[0];
    fncs[3] = f[1];

    vals = PM_interpol(grid, r, nl, fncs, 4);

/* print the results */
    out = fopen("test_3.out", "w");
    if (out == NULL)
       {printf("CAN'T OPEN TEST_3.OUT\n");
        rv = FALSE;}

    else
       {fprintf(out, "#   I    J         X            Y             Fx           Fy\n");
	for (j = 0; j < ny; j++)
	    for (i = 0; i < nx; i++)
	        {l = j*nx + i;
		 fprintf(out, " %4d %4d   %12.6e %12.6e   %12.6e %12.6e\n",
			 i, j, x[0][l], x[1][l], f[0][l], f[1][l]);};

	fprintf(out, "#   I        Rx           Ry            FIx          FIy\n");
	for (l = 0; l < nl; l++)
	    fprintf(out, " %4d   %12.6e %12.6e   %12.6e %12.6e\n",
		    l, vals[2][l], vals[3][l], vals[0][l], vals[1][l]);

	fclose(out);

	rv = TRUE;};

    PM_free_vectors(2, x);
    PM_free_vectors(2, f);
    PM_free_vectors(2, r);
    PM_free_vectors(2, vals);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             TEST_4 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_4 - test 2d multi-quadric */

int test_4(void)
   {int i, j, l, nd, nf, nl, nx, ny, rv, ok;
    int mxo[2];
    double inl;
    double prm[4];
    double **f, **r, **fo, **xo;
    FILE *out;

    nd = 2;
    nx = 10;
    ny = 15;
    nl = 20;
    nf = 1;

    inl = 1.0/(nl - 1.0);

    f = PM_make_vectors(nf, nl);
    for (l = 0; l < nl; l++)
        f[0][l] = l*inl;

/* setup the interpolation points */
    nl = 20;
    r  = setup_2d_points(nl);

    xo = CMAKE_N(double *, nd);
    fo = CMAKE_N(double *, nf);

    mxo[0] = nx;
    mxo[1] = ny;
    prm[0] = 0.0;
    prm[1] = 0.0;
    prm[2] = 0.0;
    prm[3] = 0.0;
    ok = PM_interp_mesh_mq(nd, nf, nl, r, f, mxo, xo, fo, prm);
    SC_ASSERT(ok == TRUE);

/* print the results */
    out = fopen("test_4.out", "w");
    if (out == NULL)
       {printf("CAN'T OPEN TEST_4.OUT\n");
        rv = FALSE;}

    else
       {fprintf(out, "#   I        Rx           Ry            Fx\n");
	for (l = 0; l < nl; l++)
	    fprintf(out, " %4d   %12.6e %12.6e   %12.6e\n",
		    l, r[0][l], r[1][l], f[0][l]);

	fprintf(out, "#   I    J         X            Y             Fx\n");
	for (j = 0; j < ny; j++)
	    for (i = 0; i < nx; i++)
	        {l = j*nx + i;
		 fprintf(out, " %4d %4d   %12.6e %12.6e   %12.6e\n",
			 i, j, xo[0][i], xo[1][j], fo[0][l]);};

	fclose(out);

	rv = TRUE;};

    PM_free_vectors(nf, f);
    PM_free_vectors(nf, fo);
    PM_free_vectors(nd, r);
    PM_free_vectors(nd, xo);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             TEST_5 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_5 - test 2d inverse distance weighting */

int test_5(void)
   {int i, j, l, nd, nf, nl, nx, ny, rv, ok;
    int mxo[2];
    double inl;
    double prm[4];
    double **f, **r, **fo, **xo;
    FILE *out;

    nd = 2;
    nx = 10;
    ny = 15;
    nl = 20;
    nf = 1;

    inl = 1.0/(nl - 1.0);

    f = PM_make_vectors(nf, nl);
    for (l = 0; l < nl; l++)
        f[0][l] = l*inl;

/* setup the interpolation points */
    nl = 20;
    r  = setup_2d_points(nl);

    xo = CMAKE_N(double *, nd);
    fo = CMAKE_N(double *, nf);

    mxo[0] = nx;
    mxo[1] = ny;
    prm[0] = 0.2;
    prm[1] = 2.0;
    prm[2] = 2.0;
    prm[3] = 0.2;
    ok = PM_interp_mesh_id(nd, nf, nl, r, f, mxo, xo, fo, prm);
    SC_ASSERT(ok == TRUE);

/* print the results */
    out = fopen("test_5.out", "w");
    if (out == NULL)
       {printf("CAN'T OPEN TEST_5.OUT\n");
        rv = FALSE;}

    else
       {fprintf(out, "#   I        Rx           Ry            Fx\n");
	for (l = 0; l < nl; l++)
	    fprintf(out, " %4d   %12.6e %12.6e   %12.6e\n",
		    l, r[0][l], r[1][l], f[0][l]);

	fprintf(out, "#   I    J         X            Y             Fx\n");
	for (j = 0; j < ny; j++)
	    for (i = 0; i < nx; i++)
	        {l = j*nx + i;
		 fprintf(out, " %4d %4d   %12.6e %12.6e   %12.6e\n",
			 i, j, xo[0][i], xo[1][j], fo[0][l]);};

	fclose(out);

	rv = TRUE;};

    PM_free_vectors(nf, f);
    PM_free_vectors(nf, fo);
    PM_free_vectors(nd, r);
    PM_free_vectors(nd, xo);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
   {int rv;

/* interupt handler */
    SC_signal(SIGINT, SIG_DFL);

    rv = TRUE;

/* linear */
    rv &= test_1();

/* cubic spline */
    rv &= test_2();

/* 2d lagrangian mesh */
    rv &= test_3();

/* 2d multi-quadric */
    rv &= test_4();

/* 2d inverse-distance */
    rv &= test_5();

/* invert rv for exit status */
    rv = (rv == FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
