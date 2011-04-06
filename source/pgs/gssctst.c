/*
 * GSSCTST.C - C PGS contour plotting example
 *
 * Source Version: 3.0
 * Software Release: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
#include "pgs.h"

int main(int argc, char **argv)
  {int i, k, l, kmax, lmax, kxl;
   int id, lncolor, lnstyle;
   double *x, *y, *f, r, t, lnwidth;
   double xmin, xmax, ymin, ymax;
   PM_centering centering;
   PG_device *SCR_dev;
   PG_graph *dataset;

/* set up the graphics window */

   SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Contour Test");
   PG_open_device(SCR_dev, 0.05, 0.2, 0.45, 0.45);

   PG_set_viewport_pos(SCR_dev, 0.25, 0.15);
   PG_set_viewport_shape(SCR_dev, 0.5, 0.0, 0.5/0.3333);

   PG_white_background(SCR_dev, TRUE);
   PG_set_border_width(SCR_dev, 5);

/* set up data */
   kmax      = 20;
   lmax      = 20;
   xmin      = -5.0;
   xmax      = 5.0;
   ymin      = -5.0;
   ymax      = 5.0;
   kxl       = kmax*lmax;
   x         = CMAKE_N(double, kxl);
   y         = CMAKE_N(double, kxl);
   f         = CMAKE_N(double, kxl);
   id        = 'A';
   lncolor   = (SCR_dev != NULL) ? SCR_dev->BLUE : 4;
   lnwidth   = 0.0;
   lnstyle   = LINE_SOLID;
   centering = N_CENT;

   for (k = 0; k < kmax; k++)
       for (l = 0; l < lmax; l++)
           {i = l*kmax + k;
            x[i] = k/10.0 - 5.0;
            y[i] = l/10.0 - 5.0;
            r = x[i]*x[i] + y[i]*y[i];
            t = 5.0*atan(y[i]/(x[i] + SMALL));
            r = pow(r, 0.125);
            f[i] = exp(-r)*(1.0 + 0.1*cos(t));};

   dataset = PG_make_graph_r2_r1(id, "contour", FALSE,
                                 kmax, lmax, centering, x, y, f,
                                 "xy", "f");

   PG_turn_data_id(SCR_dev, 1);

   PG_contour_plot(SCR_dev, dataset);

   SC_pause();

   PG_close_device(SCR_dev);

   exit(0);}

