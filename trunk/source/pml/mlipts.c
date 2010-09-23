/*
 * MLIPTS.C - intersecting polygon performance test
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml.h"

/*--------------------------------------------------------------------------*/

/*                             TEST_1 ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* TEST_1 - test simple quads
 *        - make a mesh, make a shifted and rotated copy
 *        - intersect the cells (N squared)
 *        - most will have zero intersection
 */

int test_1(void)
   {int i, j, l, nx, ny, nc, nn, ns, rv;
    double xc, yc, ca, sa, inx, iny;
    double t0, dt;
    double rb[4], tb[4];
    double **r, **t;
    SC_array **a;
    PM_polygon **ry, **ty;

    rv = TRUE;

    nx = 10;
    ny = 12;
    nc = nx*ny;
    nn = (nx+1)*(ny+1);

    r = PM_make_vectors(2, nn);
    t = PM_make_vectors(2, nn);

    inx = 1.0/((double) nx);
    iny = 1.0/((double) ny);

    ca = cos(0.1);
    sa = sin(0.1);

    for (j = 0; j <= ny; j++)
        for (i = 0; i <= nx; i++)
            {l = j*(nx + 1) + i;
	     r[0][l] = i*inx;
	     r[1][l] = j*iny;

	     xc = ca*r[0][l] + sa*r[1][l] + 0.01;
	     yc = -sa*r[0][l] + ca*r[1][l] - 0.02;

	     t[0][l] = xc;
	     t[1][l] = yc;};

    ns = nc*nc;
    ry = FMAKE_N(PM_polygon *, nc, "TEST_1:ry");
    ty = FMAKE_N(PM_polygon *, nc, "TEST_1:ty");
    a  = FMAKE_N(SC_array *, ns, "TEST_1:a");

    for (j = 0; j < nc; j++)
        {for (i = 0; i < nc; i++)
	     {l = j*nc + i;

	      rb[0] = r[0][i];
	      rb[1] = r[0][i+1];
	      rb[2] = r[1][j];
	      rb[3] = r[1][j+1];
	      ry[i] = PM_polygon_box(rb);

	      tb[0] = t[0][i];
	      tb[1] = t[0][i+1];
	      tb[2] = t[1][j];
	      tb[3] = t[1][j+1];
	      ty[i] = PM_polygon_box(tb);};};

    t0 = SC_wall_clock_time();

    for (j = 0; j < nc; j++)
        {for (i = 0; i < nc; i++)
	     {l = j*nc + i;
	      a[l] = PM_intersect_polygons(NULL, ry[i], ty[j]);};};

    dt = SC_wall_clock_time() - t0;

    printf("  %8d    %.2f   %11.3e\n", ns, dt, dt/((double) ns));

    for (j = 0; j < nc; j++)
        {for (i = 0; i < nc; i++)
	     {l = j*nc + i;
	      PM_free_polygons(a[l], TRUE);};
	 PM_free_polygon(ry[j]);
	 PM_free_polygon(ty[j]);};

    SFREE(ry);
    SFREE(ty);

    PM_free_vectors(2, r);
    PM_free_vectors(2, t);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
   {int rv;

/* interupt handler */
    SC_signal(SIGINT, SIG_DFL);

    rv = TRUE;

/* simple quads */
    rv &= test_1();

/* invert rv for exit status */
    rv = (rv == FALSE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
