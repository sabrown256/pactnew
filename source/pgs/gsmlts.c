/*
 * GSMLTS.C - test the poly segment drawing capability in PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

#define N_POINTS 10

#define FAST

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
   {int i, k, l, m;
    int X_max, Y_max, Z_max;
    long n_seg, n_pts;
    double x, y, z, dx, dy, dz, theta, phi;
    double *r[PG_SPACEDM], *px, *py, *pz;
    char t[MAXLINE], *token, *pt;
    PG_device *SCR_dev, *SCR_dew, *PS_dev, *CGM_dev;

    if (argc > 1)
       X_max = Y_max = Z_max = atoi(argv[1]);
    else
       {X_max = N_POINTS;
        Y_max = N_POINTS;
        Z_max = N_POINTS;};

    n_seg = (X_max - 1)*Y_max*Z_max + X_max*(Y_max - 1)*Z_max +
            X_max*Y_max*(Z_max - 1);
    n_pts = 2*n_seg;

    PG_open_console("GSMLTS", "MONOCHROME", 1,
                    0.55, 0.1, 0.4, 0.8);

    SCR_dev = PG_make_device("WINDOW", "MONOCHROME", "PGS Poly Segment 2D Test");
    PG_open_device(SCR_dev, 0.09, 0.1, 0.4, 0.4);

    SCR_dew = PG_make_device("WINDOW", "MONOCHROME", "PGS Poly Segment 3D Test");
    PG_open_device(SCR_dew, 0.51, 0.1, 0.4, 0.4);

#ifdef FAST

    PS_dev = PG_make_device("PS", "MONOCHROME", "gsmlts");
    PG_open_device(PS_dev, 0.0, 0.0, 0.0, 0.0);

    CGM_dev = PG_make_device("CGM", "MONOCHROME", "gsmlts");
    PG_open_device(CGM_dev, 0.0, 0.0, 0.0, 0.0);

#endif

    for (i = 0; i < 3; i++)
        r[i] = FMAKE_N(double, n_pts, "GSMLTS.C:r[i]");

    px = r[0];
    py = r[1];
    pz = r[2];

/* compute a lattice */
    dx = 1.0/((double) X_max);
    dy = 2.0/((double) Y_max);
    dz = 3.0/((double) Y_max);
    for (k = 0; k < X_max; k++)
        {x = ((double) k)*dx;
         for (l = 0; l < Y_max; l++)
             {y = ((double) l)*dy;
              for (m = 0; m < Z_max - 1; m++)
                  {z     = ((double) m)*dz;
                   *px++ = x;
                   *py++ = y;
                   *pz++ = z;

                   *px++ = x;
                   *py++ = y;
                   *pz++ = z + dz;};};};

    for (m = 0; m < Z_max; m++)
        {z = ((double) m)*dz;
         for (k = 0; k < X_max; k++)
             {x = ((double) k)*dx;
              for (l = 0; l < Y_max - 1; l++)
                  {y     = ((double) l)*dy;
                   *px++ = x;
                   *py++ = y;
                   *pz++ = z;

                   *px++ = x;
                   *py++ = y + dy;
                   *pz++ = z;};};};

    for (l = 0; l < Y_max; l++)
        {y = ((double) l)*dy;
         for (m = 0; m < Z_max; m++)
             {z = ((double) m)*dz;
              for (k = 0; k < X_max - 1; k++)
                  {x     = ((double) k)*dx;
                   *px++ = x;
                   *py++ = y;
                   *pz++ = z;

                   *px++ = x + dx;
                   *py++ = y;
                   *pz++ = z;};};};

/* draw a cubic lattice */
    PG_set_clipping(SCR_dev, TRUE);
    PG_set_clipping(SCR_dew, TRUE);
    PG_set_clipping(PS_dev, TRUE);
    PG_set_clipping(CGM_dev, TRUE);

    PG_turn_autorange(SCR_dev, TRUE);
    PG_turn_autorange(SCR_dew, TRUE);
    PG_turn_autorange(PS_dev, TRUE);
    PG_turn_autorange(CGM_dev, TRUE);

    for (i = 1; TRUE; i++)
       {PRINT(STDOUT, "Viewing Angle: ");
        GETLN(t, MAXLINE, stdin);

        if ((token = SC_strtok(t, " ,", pt)) == NULL)
           break;
        theta = ATOF(token);

        if ((token = SC_strtok(NULL, " ,", pt)) == NULL)
           break;
        phi = ATOF(token);

	PG_set_view_angle(SCR_dev, theta, phi, 0.0);
	PG_set_view_angle(SCR_dew, theta, phi, 0.0);
	PG_set_view_angle(PS_dev, theta, phi, 0.0);
	PG_set_view_angle(CGM_dev, theta, phi, 0.0);

        PG_set_color_line(SCR_dev, i, TRUE);
        PG_set_color_line(SCR_dew, i, TRUE);
        PG_set_color_line(PS_dev, i, TRUE);
        PG_set_color_line(CGM_dev, i, TRUE);

        PG_clear_window(SCR_dev);
        PG_clear_window(PS_dev);
        PG_clear_window(CGM_dev);

        PG_draw_disjoint_polyline_n(SCR_dev, 2, WORLDC, n_seg, r, TRUE);
        PG_draw_disjoint_polyline_n(PS_dev, 2, WORLDC, n_seg, r, TRUE);
        PG_draw_disjoint_polyline_n(CGM_dev, 2, WORLDC, n_seg, r, TRUE);

        PG_finish_plot(SCR_dew);
        PG_finish_plot(PS_dev);
        PG_finish_plot(CGM_dev);

        PG_clear_window(SCR_dew);
        PG_clear_window(PS_dev);
        PG_clear_window(CGM_dev);

        PG_draw_disjoint_polyline_n(SCR_dev, 3, WORLDC, n_seg, r, TRUE);
        PG_draw_disjoint_polyline_n(PS_dev, 3, WORLDC, n_seg, r, TRUE);
        PG_draw_disjoint_polyline_n(CGM_dev, 3, WORLDC, n_seg, r, TRUE);

        PG_finish_plot(SCR_dew);
        PG_finish_plot(PS_dev);
        PG_finish_plot(CGM_dev);};

    PG_close_device(SCR_dev);
    PG_close_device(SCR_dew);

#ifdef FAST

    PG_close_device(PS_dev);
    PG_close_device(CGM_dev);

#endif

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
