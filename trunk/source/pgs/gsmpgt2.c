/*
 * GSMPGTST2.c - 2nd mpeg test for color patterns
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

#define N_MPG  4
#define N      100
#define PAL(i) palettes[(i+palette)%(sizeof(palettes)/sizeof(char *))]

int
 palette = 0;

char
 *palettes[] = {"bw", "rainbow", "iron", "contrast", "cym"};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPUTE_2D_DATA - compute data for 2D-1D rendering tests */

void compute_2d_data(double *f, double *x, double *y, int kmax, int lmax)
   {int k, l, i;
    double xmin, xmax, ymin, ymax;
    double dx, dy, tol, t, r, fv;

    xmin = -5.0;
    xmax =  5.0;
    ymin = -5.0;
    ymax =  5.0;

    SC_ASSERT(ymax > ymin);

    dx = (xmax - xmin)/((double) kmax - 1.0);
    dy = (xmax - xmin)/((double) kmax - 1.0);

    tol = 1.0e-5;

    for (k = 0; k < kmax; k++)
        for (l = 0; l < lmax; l++)
            {i    = l*kmax + k;
             x[i] = k*dx + xmin;
             y[i] = l*dy + ymin;
             r    = x[i]*x[i] + y[i]*y[i];
             t    = 5.0*atan(y[i]/(x[i] + SMALL));
             r    = POW(r, 0.125);
             fv   = exp(-r)*(1.0 + 0.1*cos(t));
             f[i] = floor(fv/tol)*tol;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print the help message */

void help(void)
   {

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "Usage: gsmpgt2 [-h] [-nf #\n");
    PRINT(STDOUT, "   -h  this help message\n");
    PRINT(STDOUT, "   -nf number of frames to produce\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, n, kmax, lmax, kxl, nx;
    int label;
    double *x, *y, *f;
    PM_centering centering;
    PG_device *dev;
    PG_graph *data;

    nx = N_MPG;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-nf") == 0)
	    nx = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-h") == 0)
            {help();
	     return(1);};};

    dev = PG_make_device("MPEG", "COLOR", "gsmpg2");
    PG_open_device(dev, 0.0, 0.0, 400.0, 400.0);

/* set up data */
    centering = N_CENT;
    kmax      = 20;
    lmax      = 20;
    kxl       = kmax*lmax;
    x         = CMAKE_N(double, kxl);
    y         = CMAKE_N(double, kxl);
    f         = CMAKE_N(double, kxl);
    label     = 'A';

    data = NULL;
    for (n = 0; n < nx; n++)
        {compute_2d_data(f, x, y, kmax, lmax);
	 data = PG_make_graph_r2_r1(label, "{x, y}->f", FALSE, 
				    kmax, lmax, centering,
				    x, y, f, "xy", "f");

	 PG_fset_palette(dev, PAL(n));
	 PG_poly_fill_plot(dev, data);}

    if (data != NULL)
       PG_rl_graph(data, TRUE, TRUE);

    PG_close_device(dev);
    PG_rl_all();

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
