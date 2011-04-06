/*
 * GSFPTS.C - test of polygon fill plot
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {

    PRINT(STDOUT, "\nGSFPTS - test the polygon fill plotting in PGS\n");
    PRINT(STDOUT, "\n");

    PRINT(STDOUT, "Usage: gsfpts [-h] [-p]\n");
    PRINT(STDOUT, "\n");

    PRINT(STDOUT, "       -h  Print this help message and exit\n");
    PRINT(STDOUT, "       -p  palette 0 - spectrum, 1 - rainbow, 2 - bw\n");

    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it off here */

int main(int argc, char **argv)
   {int i, k, l, kmax, lmax, kxl, nt;
    int id, lncolor, lnstyle, palette;
    double *x, *y, *f, r, t, lnwidth;
    double xmin, xmax, ymin, ymax;
    PM_centering centering;
    PG_device *SC_dev, *PS_dev;
    PG_graph *data;
    FILE *fp;
    char *palettes[] = {"spectrum", "rainbow", "bw"};

    nt = 1;

    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'p' :
                      palette = atoi(argv[++i]);
		      break;
                 case 't' :
		      nt = atoi(argv[++i]);
		      break;
                 case 'h' :
		      print_help();
		      return(1);};}
         else
            break;};

    SC_init_threads(nt, NULL);

/* set up data */
    palette = 1;
    kmax    = 20;
    lmax    = 20;
    xmin    = -5.0;
    xmax    = 5.0;
    ymin    = -5.0;
    ymax    = 5.0;
    kxl     = kmax*lmax;
    x       = CMAKE_N(double, kxl);
    y       = CMAKE_N(double, kxl);
    f       = CMAKE_N(double, kxl);
    id      = 'A';

    for (k = 0; k < kmax; k++)
        for (l = 0; l < lmax; l++)
            {i    = l*kmax + k;
             x[i] = k/10.0 - 5.0;
             y[i] = l/10.0 - 5.0;
             r    = x[i]*x[i] + y[i]*y[i];
             t    = 5.0*atan(y[i]/(x[i] + SMALL));
             r    = POW(r, 0.125);
             f[i] = exp(-r)*(1.0 + 0.1*cos(t));};

/* set up the graphics window */
    SC_dev = PG_make_device("WINDOW", "COLOR", "PGS Polygon Fill Test A");
    lncolor   = (SC_dev != NULL) ? SC_dev->BLUE : 4;
    lnwidth   = 0.0;
    lnstyle   = LINE_SOLID;
    PG_white_background(SC_dev, TRUE);
    PG_open_device(SC_dev, 0.05, 0.1, 0.4, 0.4);

    fp = STDOUT;
    if (fp != NULL)
       setbuf(fp, NULL);

/*    PG_register_device("JPEG", PG_setup_jpeg_device);*/
    PS_dev = PG_make_device("JPEG", "COLOR", "gsfpts");
    PG_open_device(PS_dev, 0.0, 0.0, 512.0, 512.0);

    centering = N_CENT;
    data = PG_make_graph_r2_r1(id, "{x, y}->f", FALSE,
                     kmax, lmax, centering, x, y, f, "xy", "f");

    PG_set_palette(SC_dev, palettes[palette]);
    PG_poly_fill_plot(SC_dev, data);

    PG_set_palette(PS_dev, palettes[palette]);
    PG_poly_fill_plot(PS_dev, data);
    PG_finish_plot(PS_dev);

    SC_pause();
    PG_close_device(SC_dev);
    PG_close_device(PS_dev);

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
