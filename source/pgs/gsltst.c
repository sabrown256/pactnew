/*
 * GSLTST.C - test of graphics library
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

#define N 50

#define DASHED     2
#define DOTTED     3
#define DOTDASHED  4

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DRAW_SET_1 - draw the first data set */

static void draw_set_1(double *x, double *y,
		       PG_device *SCR_dev, PG_device *SCR_dew,
		       PG_device *PS_dev, PG_device *CGM_dev)
   {int i;
    PG_graph *data;

    for (i = 0; i < N; i++)
        {x[i] = -(i+1)/8.0;
         y[i] = 6.022e23/x[i];};

    data = PG_make_graph_1d('A', "Test Data #1", FALSE, N,
                            x, y, "X Values", "Y Values");

    PG_turn_grid(SCR_dev, OFF);
    PG_turn_grid(SCR_dew, OFF);
    PG_turn_grid(PS_dev, OFF);
    PG_turn_grid(CGM_dev, OFF);

    PG_set_attrs_glb(TRUE,
		     "plot-labels", TRUE,
		     "axis-tick-type", AXIS_TICK_RIGHT,
		     NULL);

/* picture #1 */
    data->info = PG_set_line_info(data->info, PLOT_CARTESIAN, CARTESIAN_2D,
				  DASHED, FALSE, 0, 4, 0, 0.0);
    PG_draw_graph(SCR_dev, data);
    PG_draw_graph(PS_dev, data);
    PG_draw_graph(CGM_dev, data);
    SC_pause();

/* picture #2 */
    data->info = PG_set_line_info(data->info, PLOT_POLAR, POLAR,
				  DOTTED, FALSE, 0, 4, 0, 0.0);
    PG_draw_graph(SCR_dew, data);
    PG_draw_graph(PS_dev, data);
    PG_draw_graph(CGM_dev, data);
    SC_pause();

/* picture #3 */
    data->info = PG_set_line_info(data->info, PLOT_INSEL, INSEL,
				  LINE_SOLID, FALSE, 0, 4, 0, 0.0);
    PG_draw_graph(SCR_dev, data);
    PG_draw_graph(PS_dev, data);
    PG_draw_graph(CGM_dev, data);
    SC_pause();

    PG_set_attrs_glb(TRUE,
		     "axis-tick-type", AXIS_TICK_LEFT,
		     NULL);

/* picture #4 */
    data->info = PG_set_line_info(data->info, PLOT_HISTOGRAM, CARTESIAN_2D,
				  LINE_SOLID, FALSE, 0, 4, 0, 0.0);
    PG_draw_graph(SCR_dew, data);
    PG_draw_graph(PS_dev, data);
    PG_draw_graph(CGM_dev, data);
    SC_pause();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DRAW_SET_2 - draw the second set */

static void draw_set_2(double *x, double *y,
		       PG_device *SCR_dev, PG_device *SCR_dew,
		       PG_device *PS_dev, PG_device *CGM_dev)
   {int i;
    PG_graph *data;

    for (i = 0; i < N; i++)
        {x[i] = i/8.0;
         y[i] = cos(x[i]);};

    data = PG_make_graph_1d('B', "Test Data #2", FALSE, N,
                            x, y, "X Values", "Y Values");

    PG_turn_grid(SCR_dev, ON);
    PG_turn_grid(SCR_dew, ON);
    PG_turn_grid(PS_dev, ON);
    PG_turn_grid(CGM_dev, ON);

    PG_set_attrs_glb(TRUE,
		     "plot-labels", FALSE,
		     "axis-tick-type", AXIS_TICK_RIGHT,
		     NULL);

/* picture #5 */
    data->info = PG_set_line_info(data->info, PLOT_HISTOGRAM, CARTESIAN_2D,
				  LINE_SOLID, FALSE, 0, 5, 1, 0.0);
    PG_draw_graph(SCR_dev, data);
    PG_draw_graph(PS_dev, data);
    PG_draw_graph(CGM_dev, data);
    SC_pause();

/* picture #6 */
    data->info = PG_set_line_info(data->info, PLOT_INSEL, INSEL,
				  LINE_SOLID, FALSE, 0, 5, 0, 0.0);
    PG_draw_graph(SCR_dew, data);
    PG_draw_graph(PS_dev, data);
    PG_draw_graph(CGM_dev, data);
    SC_pause();

/* picture #7 */
    data->info = PG_set_line_info(data->info, PLOT_POLAR, POLAR,
				  DOTDASHED, FALSE, 0, 5, 0, 0.0);
    PG_draw_graph(SCR_dev, data);
    PG_draw_graph(PS_dev, data);
    PG_draw_graph(CGM_dev, data);
    SC_pause();

    PG_set_attrs_glb(TRUE,
		     "axis-tick-type", AXIS_TICK_LEFT,
		     NULL);

/* picture #8 */
    data->info = PG_set_line_info(data->info, PLOT_CARTESIAN, CARTESIAN_2D,
				  LINE_SOLID, FALSE, 0, 5, 0, 0.0);
    PG_draw_graph(SCR_dew, data);
    PG_draw_graph(PS_dev, data);
    PG_draw_graph(CGM_dev, data);
    SC_pause();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
   {int i, use_color;
    double *x, *y;
    char *s;
    PG_device *SCR_dev, *SCR_dew, *PS_dev, *CGM_dev;

    use_color = FALSE;
    s = CSTRSAVE("WINDOW");
    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-')
           {switch (argv[i][1])
               {case 'c' : use_color = TRUE;
                           break;
                case 'h' : i++;
                           s = argv[i];
                           break;};};

    x = CMAKE_N(double, N);
    y = CMAKE_N(double, N);

/* set up the left window */
    SCR_dev = PG_make_device(s, "COLOR", "PGS Test A");
    PG_white_background(SCR_dev, TRUE);
    PG_turn_data_id(SCR_dev, ON);
    PG_open_device(SCR_dev, 0.05, 0.2, 0.45, 0.45);

/* set up the right window */
    SCR_dew = PG_make_device(s, "COLOR", "PGS Test B");
    PG_white_background(SCR_dew, FALSE);
    PG_turn_data_id(SCR_dew, ON);
    PG_fset_max_intensity(SCR_dew, 0.8);
    PG_open_device(SCR_dew, 0.5, 0.2, 0.45, 0.45);

/* set up the hard copy device */
    if (use_color)
       PS_dev = PG_make_device("PS", "COLOR", "gsltst 2.0 2.0");
    else
       PS_dev = PG_make_device("PS", "MONOCHROME", "gsltst 2.0 2.0");
    PG_turn_data_id(PS_dev, ON);
    PG_open_device(PS_dev, 0.0, 0.0, 0.0, 0.0);

/* set up the hard copy device */
    if (use_color)
       CGM_dev = PG_make_device("CGM", "COLOR", "gsltst");
    else
       CGM_dev = PG_make_device("CGM", "MONOCHROME", "gsltst");
    PG_turn_data_id(CGM_dev, ON);
    PG_fset_max_intensity(CGM_dev, 0.8);
    PG_open_device(CGM_dev, 0.0, 0.0, 0.0, 0.0);

    draw_set_1(x, y, SCR_dev, SCR_dew, PS_dev, CGM_dev);
    draw_set_2(x, y, SCR_dev, SCR_dew, PS_dev, CGM_dev);

    PG_close_device(SCR_dev);
    PG_close_device(SCR_dew);
    PG_close_device(PS_dev);
    PG_close_device(CGM_dev);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
