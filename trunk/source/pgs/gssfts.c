/*
 * GSSFTS.C - test the surface drawing routines in PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

#define N_POINTS 10

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DRAW_SET - draw a complete set of plots */

static void draw_set(PG_device *SCR_dev, PG_device *PS_mdev,
		     PG_device *CGM_mdev, PG_device *PS_cdev,
		     PG_device *CGM_cdev,
		     double *rz, double *rs, double *ext, double *rx, double *ry,
		     int n_pts, double xmn, double xmx, double ymn, double ymx,
		     double theta, double phi, double width,
		     int color, int style, PG_rendering pty, int *maxes,
		     pcons *alist, int fast)
   {double va[N_ANGLES];
    double *r[PG_SPACEDM];

    r[0] = rx;
    r[1] = ry;

    va[0] = theta;
    va[1] = phi;
    va[2] = 0.0;

    PG_draw_surface_n(SCR_dev, rz, rs, ext, r, n_pts,
		      va, width, color, style, pty, "Test Data",
		      PM_LR_S, maxes, alist);

    if (!fast)
       return;

    PRINT(STDOUT, "Monochrome PS ... ");
    PG_draw_surface_n(PS_mdev, rz, rs, ext, r, n_pts,
		      va, width, color, style, pty, "Test Data",
		      PM_LR_S, maxes, alist);

    PRINT(STDOUT, "Monochrome CGM ... ");
    PG_draw_surface_n(CGM_mdev, rz, rs, ext, r, n_pts,
		      va, width, color, style, pty, "Test Data",
		      PM_LR_S, maxes, alist);

    PRINT(STDOUT, "Color PS ... ");
    PG_draw_surface_n(PS_cdev, rz, rs, ext, r, n_pts,
		      va, width, color, style, pty, "Test Data",
		      PM_LR_S, maxes, alist);

    PRINT(STDOUT, "Color CGM ... ");
    PG_draw_surface_n(CGM_cdev, rz, rs, ext, r, n_pts,
		      va, width, color, style, pty, "Test Data",
		      PM_LR_S, maxes, alist);

    PRINT(STDOUT, "done\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PRINT_HELP - print a help message */

static void print_help(void)
   {PRINT(STDOUT, "\nGSSFTS - test the surface plotting in PGS\n");
    PRINT(STDOUT, "\n");

    PRINT(STDOUT, "Usage: gssfts [-d #] [-p #] [-h] [-z #]\n");
    PRINT(STDOUT, "\n");

    PRINT(STDOUT, "       -d  delta phi in degrees for sequence\n");
    PRINT(STDOUT, "       -f  do hardcopy tests (for fast machines)\n");
    PRINT(STDOUT, "       -h  Print this help message and exit\n");
    PRINT(STDOUT, "       -p  palette 0 - spectrum, 1 - rainbow, 2 - bw\n");
    PRINT(STDOUT, "       -z  Use the specified number of zones on a side\n");

    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int argc, char **argv)
   {int i, k, l, nt, fast;
    int maxes[2], X_max, Y_max, n_pts, use_color, palette;
    double x, y, dx, dy, theta, phi, dp;
    char t[MAXLINE], *token, *emap;
    PG_rendering pty;
    PG_device *SCR_dev, *PS_mdev, *CGM_mdev, *PS_cdev, *CGM_cdev;
    double *rx, *ry, *rz, *rs, *px, *py, *pz, *ps, *ext;
    pcons *alist;
    char *palettes[] = {"spectrum", "rainbow", "bw"};

    X_max     = N_POINTS;
    Y_max     = N_POINTS;
    palette   = 0;
    use_color = FALSE;
    pty      = PLOT_WIRE_MESH;
    dp        = 0.0;
    nt        = 1;
    fast      = FALSE;
    for (i = 1; i < argc; i++)
        {if (argv[i][0] == '-')
            {switch (argv[i][1])
                {case 'c' :
		      use_color = TRUE;
		      break;
                 case 'd' :
		      dp = ATOF(argv[++i]);
		      break;
                 case 'f' :
		      fast = TRUE;
		      break;
                 case 'h' :
		      print_help();
		      return(1);
                 case 'n' :
		      nt = atoi(argv[++i]);
		      break;
                 case 'p' :
		      palette = atoi(argv[++i]);
		      break;
                 case 's' :
		      pty = PLOT_SURFACE;
		      break;
                 case 'w' :
		      pty = PLOT_WIRE_MESH;
		      break;
                 case 'z' :
		      X_max = Y_max = atoi(argv[++i]);
		      break;};}

         else
            break;};

    SC_init_threads(nt, NULL);

    maxes[0] = X_max;
    maxes[1] = Y_max;
    n_pts    = X_max*Y_max;

    emap = CMAKE_N(char, n_pts);
    memset(emap, 1, n_pts);
    alist = PG_set_attrs_alist(NULL,
			      "EXISTENCE", SC_CHAR_I, TRUE, emap,
			      NULL);

    PG_open_console("GSSFTS", "MONOCHROME", 1,
                    0.55, 0.1, 0.4, 0.8);

    if (use_color)
       SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Surface Test");
    else
       SCR_dev = PG_make_device("WINDOW", "MONOCHROME", "PGS Surface Test");
/*    PG_open_device(SCR_dev, 0.1, 0.1, 0.02, 0.02); */
    PG_open_device(SCR_dev, 0.1, 0.1, 0.4, 0.4);

    if (pty == PLOT_SURFACE)
       PG_set_palette(SCR_dev, palettes[palette]);

    if (fast)
       {PS_cdev  = PG_make_device("PS", "COLOR", "gssfts-c");
	CGM_cdev = PG_make_device("CGM", "COLOR", "gssfts-c");
	PS_mdev  = PG_make_device("PS", "MONOCHROME", "gssfts-m");
	CGM_mdev = PG_make_device("CGM", "MONOCHROME", "gssfts-m");

	PG_open_device(PS_cdev, 0.0, 0.0, 0.0, 0.0);
	PG_open_device(CGM_cdev, 0.0, 0.0, 0.0, 0.0);
	PG_open_device(PS_mdev, 0.0, 0.0, 0.0, 0.0);
	PG_open_device(CGM_mdev, 0.0, 0.0, 0.0, 0.0);

	if (pty == PLOT_SURFACE)
	   {PG_set_palette(PS_mdev, palettes[palette]);
	    PG_set_palette(CGM_mdev, palettes[palette]);
	    PG_set_palette(PS_cdev, palettes[palette]);
	    PG_set_palette(CGM_cdev, palettes[palette]);};};

    rx = px = CMAKE_N(double, n_pts);
    ry = py = CMAKE_N(double, n_pts);
    rz = pz = CMAKE_N(double, n_pts);

    if (pty == PLOT_SURFACE)
       rs = ps = CMAKE_N(double, n_pts);
    else
       rs = ps = pz;

    ext = CMAKE_N(double, 4);
    ext[0] = -1;
    ext[1] =  1;
    ext[2] = -1;
    ext[3] =  1;

/* draw the first image */
    dx = 2.0*PI/((double) (X_max - 1));
    dy = 2.0*PI/((double) (Y_max - 1));
    for (l = 0; l < Y_max; l++)
        {for (k = 0; k < X_max; k++)
             {*px++ = x = ((double) k)*dx;
              *py++ = y = ((double) l)*dy;
              *pz++ = cos(y);
	      if (pty == PLOT_SURFACE)
                 *ps++ = sin(x);};};

    if (dp == 0.0)
       {while (TRUE)
           {char *s;

	    PRINT(STDOUT, "Viewing Angle: ");
            GETLN(t, MAXLINE, stdin);

            if ((token = SC_strtok(t, " ,", s)) == NULL)
               break;
            theta = ATOF(token);

            if ((token = SC_strtok(NULL, " ,", s)) == NULL)
               break;
            phi = ATOF(token);

            draw_set(SCR_dev, PS_mdev, CGM_mdev, PS_cdev, CGM_cdev,
		     rz, rs, ext, rx, ry,
		     n_pts,
		     0.0, 2.0*PI, 0.0, 2.0*PI,
		     theta, phi,
		     0.0, SCR_dev->BLUE, LINE_SOLID,
		     pty, maxes, alist,
		     fast);};}

    else
       {for (phi = 0.0; phi <= 90.0; phi += dp)
            {draw_set(SCR_dev, PS_mdev, CGM_mdev, PS_cdev, CGM_cdev,
		      rz, rs, ext, rx, ry,
		      n_pts,
		      0.0, 2.0*PI, 0.0, 2.0*PI,
		      0.0, phi,
		      0.0, SCR_dev->BLUE, LINE_SOLID,
		      pty, maxes, alist,
		      fast);};};

    PG_close_device(SCR_dev);

    if (fast)
       {PG_close_device(PS_cdev);
	PG_close_device(CGM_cdev);
	PG_close_device(PS_mdev);
	PG_close_device(CGM_mdev);};

    CFREE(emap);

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
