/*
 * GSVTST.C - test of vector routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it off here */

int main(int argc, char **argv)
   {int i, k, l, kmax, lmax, kxl;
    int id, lncolor, lnstyle;
    double xmin, xmax, ymin, ymax, km, lm;
    double xc[PG_SPACEDM];
    double *x, *y, *u, *v, lnwidth;
    PG_device *SCR_dev, *PS_dev, *CGM_dev;
    PM_centering centering;
    PM_set *domain, *range;
    PG_graph *data;

/* set up the graphics window */
    SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Vector Test");

    PG_white_background(SCR_dev, TRUE);

    xc[0] = 0.25;
    xc[1] = 0.15;
    PG_fset_viewport_pos(SCR_dev, x);

    xc[0] = 0.5;
    xc[1] = 0.0;
    PG_fset_viewport_shape(SCR_dev, xc, 0.500/0.333);

    PG_open_device(SCR_dev, 0.05, 0.2, 0.45, 0.45);

    PS_dev = PG_make_device("PS", "MONOCHROME", "gsctst");
    PG_open_device(PS_dev, 0.0, 0.0, 0.0, 0.0);

    CGM_dev = PG_make_device("CGM", "MONOCHROME", "gsctst");
    PG_open_device(CGM_dev, 0.0, 0.0, 0.0, 0.0);

/* set up data */
    kmax      = 20;
    lmax      = 20;
    xmin      = -5.0;
    xmax      = 5.0;
    ymin      = -5.0;
    ymax      = 5.0;
    id        = 'A';
    lncolor   = (SCR_dev != NULL) ? SCR_dev->BLUE : 4;
    lnwidth   = 0.0;
    lnstyle   = LINE_SOLID;
    centering = N_CENT;

    kxl = kmax*lmax;
    x   = CMAKE_N(double, kxl);
    y   = CMAKE_N(double, kxl);
    u   = CMAKE_N(double, kxl);
    v   = CMAKE_N(double, kxl);

    km = 2.0*PI/((double) (kmax - 1));
    lm = 2.0*PI/((double) (lmax - 1));
    for (k = 0; k < kmax; k++)
        for (l = 0; l < lmax; l++)
            {i    = l*kmax + k;
             x[i] = k*km;
             y[i] = l*lm;
             u[i] = sin(k*km);
             v[i] = sin(l*lm);};

/* build the domain set */
    domain = PM_make_set("{x, y}", SC_DOUBLE_S, FALSE,
			 2, kmax, lmax, 2, x, y);

/* build the range set */
    range = PM_make_set("{u, v}", SC_DOUBLE_S, FALSE,
			2, kmax, lmax, 2, u, v);

    data = PG_make_graph_from_sets("{x, y}->{u, v}", domain, range,
				   centering,
				   SC_PCONS_P_S, NULL, id, NULL);

    PG_set_tdv_info(data->info, PLOT_VECTOR, CARTESIAN_2D,
		    lnstyle, lncolor, lnwidth);

    PG_vector_plot(SCR_dev, data);
    PG_vector_plot(PS_dev, data);
    PG_vector_plot(CGM_dev, data);

    SC_pause();

    PG_close_device(SCR_dev);
    PG_close_device(PS_dev);
    PG_close_device(CGM_dev);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
