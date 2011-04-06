/*
 * GSRSTT.C - test of PGS raster device
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

#define N 50

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
   {int i, k, l, w, h, image_test, color, ids;
    char *s;
    unsigned char *bf;
    double dx, dy, xv, yv, sf;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    double *x, *y;
    PG_graph *data;
    PG_palette *pl;
    PG_image *calc_im;
    PG_device *PS_dev;

    s = CSTRSAVE("WINDOW");

    image_test = TRUE;
    color      = FALSE;
    ids        = FALSE;

    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-')
           {switch (argv[i][1])
               {case 'h' : i++;
                           s = argv[i];
                           break;
                case 'c' : color = TRUE;
                           break;
                case 'd' : ids = TRUE;
                           break;
                case 's' : image_test = FALSE;
                           break;};};

    x = CMAKE_N(double, N);
    y = CMAKE_N(double, N);

    for (i = 0; i < N; i++)
        {x[i] = -(i+1)/10.0;
         y[i] = 6.022e23/x[i];};

    data = PG_make_graph_1d('A', "Test Data #1", FALSE, N,
                            x, y, "X Values", "Y Values");

    if (color)
       PS_dev = PG_make_device("RASTER", "COLOR", "gspsts");
    else
       PS_dev = PG_make_device("RASTER", "MONOCHROME", "gspsts");

    if (PS_dev == NULL)
       return(1);

/* set up the hard copy device */
    PG_turn_data_id(PS_dev, ids);

    PG_open_device(PS_dev, 0.0, 0.0, 0.0, 0.0);

    data->info = PG_set_line_info(data->info, PLOT_CARTESIAN, CARTESIAN_2D,
				  LINE_SOLID, FALSE, 0, PS_dev->BLUE,
				  0, 0.0);
    PG_draw_graph(PS_dev, data);

    data->info = PG_set_line_info(data->info, PLOT_POLAR, POLAR,
				  LINE_SOLID, FALSE, 0, PS_dev->GREEN,
				  0, 0.0);
    PG_draw_graph(PS_dev, data);

    data->info = PG_set_line_info(data->info, PLOT_INSEL, INSEL,
				  LINE_SOLID, FALSE, 0, PS_dev->RED,
				  0, 0.0);
    PG_draw_graph(PS_dev, data);

    data->info = PG_set_line_info(data->info, PLOT_HISTOGRAM, CARTESIAN_2D,
				  LINE_SOLID, FALSE, 0, PS_dev->WHITE,
				  0, 0.0);
    PG_draw_graph(PS_dev, data);

    for (i = 0; i < N; i++)
        {x[i] = i/10.0;
         y[i] = cos(x[i]);};

    data = PG_make_graph_1d('B', "Test Data #2", FALSE, N,
                            x, y, "X Values", "Y Values");

    PG_turn_grid(PS_dev, ON);

    data->info = PG_set_line_info(data->info, PLOT_HISTOGRAM, CARTESIAN_2D,
				  LINE_SOLID, FALSE, 0, PS_dev->MAGENTA,
				  1, 0.0);
    PG_draw_graph(PS_dev, data);

    data->info = PG_set_line_info(data->info, PLOT_POLAR, POLAR,
				  LINE_SOLID, FALSE, 0, PS_dev->CYAN,
				  0, 0.0);
    PG_draw_graph(PS_dev, data);

    data->info = PG_set_line_info(data->info, PLOT_INSEL, INSEL,
				  LINE_SOLID, FALSE, 0, PS_dev->YELLOW,
				  0, 0.0);
    PG_draw_graph(PS_dev, data);

    data->info = PG_set_line_info(data->info, PLOT_CARTESIAN, CARTESIAN_2D,
				  LINE_SOLID, FALSE, 0, PS_dev->DARK_GRAY,
				  0, 0.0);
    PG_draw_graph(PS_dev, data);

    if (image_test)
       {w = 250;
        h = 250;

	PG_box_init(2, dbx, 0.0, 1.0);
	PG_box_init(1, rbx, -1.0, 1.0);

        calc_im = PG_make_image_n("Test Image", SC_CHAR_S, NULL,
				  2, WORLDC, dbx, rbx, w, h, 4, NULL);
        if (calc_im == NULL)
           {PRINT(STDOUT, "CAN'T ALLOCATE IMAGE");
            exit(1);};

/* draw the first image */
        pl = PG_set_palette(PS_dev, "rainbow");
        bf = calc_im->buffer;
        sf = pl->n_pal_colors;
        for (l = 0; l < h; l++)
            {for (k = 0; k < w; k++)
                 *bf++ = sf*((double) k)*((double) (h - l - 1))/
                               ((double) h*w);};

        PS_dev->grid  = OFF;

        PG_draw_image(PS_dev, calc_im, "Test Data A", NULL);

/* draw the second image */
        pl = PG_set_palette(PS_dev, "spectrum");
        dx = 2.0*PI/((double) w);
        dy = 2.0*PI/((double) h);
        bf = calc_im->buffer;
        sf = pl->n_pal_colors;
        for (l = 0; l < h; l++)
            {for (k = 0; k < w; k++)
                 {xv = ((double) k)*dx;
                  yv = ((double) (h - l - 1))*dy;

                 *bf++ = sf*(0.5 + 0.5*sin(xv)*cos(yv));};};

        PG_draw_image(PS_dev, calc_im, "Test Data B", NULL);

        PG_rl_image(calc_im);};

    PG_close_device(PS_dev);

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
