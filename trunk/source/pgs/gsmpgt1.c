/*
 * GSMPGTST1.c - 1st mpeg test, a moving sine curve
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

#define N_MPG 10
#define N 100
#define TWOPI 2.0*3.1415926535897931
#define XCON TWOPI/N
#define YCON TWOPI/N_MPG

int
 ids = 0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print the help message */

void help(void)
   {

    PRINT(STDOUT, "\n");
    PRINT(STDOUT, "Usage: gsmpgt1 [-h] [-nf #\n");
    PRINT(STDOUT, "   -h  this help message\n");
    PRINT(STDOUT, "   -nf number of frames to produce\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, n, nx, label;
    double *x, *y;
    PG_graph *data;
    PG_device *dev;

    nx = N_MPG;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-nf") == 0)
	    nx = SC_stoi(v[++i]);
	 else if (strcmp(v[i], "-h") == 0)
            {help();
	     return(1);};};

    dev = PG_make_device("MPEG", "COLOR", "gsmpg1");
    if (dev == NULL)
       return(1);

    PG_open_device(dev, 0.0, 0.0, 512.0, 512.0);

    x = FMAKE_N(double, N, "MPGTST:x");
    y = FMAKE_N(double, N, "MPGTST:y");
    label = 'A';

    data = NULL;
    for (n = 0; n < nx; n++)
        {for (i = 0; i < N; i++)
	     {x[i] = XCON*i;
	      y[i] = sin(x[i]+YCON*n);}

	 data = PG_make_graph_1d(label++, "Test Data #1", FALSE, N,
				 x, y, "X Values", "Y Values");
	 PG_turn_data_id(dev, ids);
	 data->info = (void *) PG_set_line_info(data->info, 
						PLOT_CARTESIAN, CARTESIAN_2D, 
						LINE_SOLID, FALSE, 0,
						dev->BLUE, 0, 0.0);
	 PG_draw_graph(dev, data);}

    if (data != NULL)
       PG_rl_graph(data, TRUE, TRUE);

    PG_close_device(dev);
    PG_rl_all();

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
