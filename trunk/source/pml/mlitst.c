/*
 * MLITST.C - driver to study the interpolation routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPUTE_F - interpolate the mapping from voltages to positions
 *           - into one from positions to voltages
 */

double **compute_f(double **r, int ni, double **x, double **f,
		   int nx, int ny)
   {double **vals, **fncs;
    PM_lagrangian_mesh *grid;

    grid       = FMAKE(PM_lagrangian_mesh, "COMPUTE_F:grid");
    grid->x    = f[0];
    grid->y    = f[1];
    grid->kmax = nx;
    grid->lmax = ny;

    fncs    = FMAKE_N(double *, 4, "COMPUTE_F:fncs");
    fncs[0] = x[0];
    fncs[1] = x[1];
    fncs[2] = f[0];
    fncs[3] = f[1];

    vals = PM_interpol(grid, r, ni, fncs, 4);

    return(vals);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DUMP - make an ASCII dump of a map */

void dump(char *s, double **p, int n, FILE *fp)
   {int i;
    double *px, *py;

    px = p[0];
    py = p[1];

    fprintf(fp, "\n\n%s\n\n", s);

    for (i = 0; i < n; i++)
        fprintf(fp, "(%6.3f, %6.3f) ", *px++, *py++);

    fprintf(fp, "\n\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - test 2d interpolation on LR mesh */

int test_1(void)
   {int i, j, l, ni, np, nx, ny, rv;
    double inx, iny;
    double **f, **x, **r, **vals;
    FILE *out;

/* interupt handler */
    SC_signal(SIGINT, SIG_DFL);

    ni = 20;
    nx = 10;
    ny = 15;
    np = nx*ny;

    inx = 1.0/(nx - 1.0);
    iny = 1.0/(ny - 1.0);

    x = PM_make_vectors(2, np);
    f = PM_make_vectors(2, np);

/* get in the number of interpolation points */
    r = PM_make_vectors(2, ni);

    for (j = 0; j < ny; j++)
        for (i = 0; i < nx; i++)
	    {l = j*nx + i;

/* setup the coordinates of the logical values */
	     x[0][l] = i*inx;
	     x[1][l] = j*iny;

/* setup the logical values */
	     f[0][l] = i*inx + (ny - 1.0 - j)*iny;
	     f[1][l] = (nx - 1.0 - i)*inx + j*iny;};

/* get in the list of interpolation points */
    for (i = 0; i < ni; i++)
        {r[0][i] = i*inx + cos(PI*inx*i);
         r[1][i] = i*inx + sin(PI*inx*i);};

    vals = compute_f(r, ni, f, x, nx, ny);

    out = fopen("test_1.out", "w");
    if (out == NULL)
       {printf("CAN'T OPEN TEST_1.OUT\n");
        rv = FALSE;}

    else
       {dump("LOGICAL VALUES - F", f, np, out);
	dump("X", x, np, out);
	dump("INTERPOLATION POINTS - R", r, ni, out);
	dump("F(R)", vals, ni, out);
	dump("R", vals + 2, ni, out);

	fclose(out);

	rv = TRUE;};

    PM_free_vectors(2, x);
    PM_free_vectors(2, f);
    PM_free_vectors(2, r);
    PM_free_vectors(2, vals);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
   {int rv;

    rv = TRUE;
    rv &= test_1();

/* invert rv for exit status */
    rv = (rv == FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
