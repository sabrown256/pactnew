/*
 * GSCTST.C - test of contour routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

#define SPHERE       1
#define HYPERBOLOID  2
#define LINEAR       3

static PG_device
 *SCR_dev,
 *PS_dev,
 *CGM_dev;

static double
 theta = 70.0,
 phi   = 10.0;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2D_SIMPLE - test the simple 2d contour plotting facility */

static void test_2d_simple(void)
   {int i, k, l, imx, jmx, ne;
    int id, lncolor, lnstyle;
    double *x, *y, *f, r, t, lnwidth;
    double xmn, xmx, ymn, ymx, fmn, fmx;
    PM_centering centering;
    PG_graph *data;

/* set up data */
    imx       = 20;
    jmx       = 20;
    xmn       = -5.0;
    xmx       = 5.0;
    ymn       = -5.0;
    ymx       = 5.0;
    ne        = imx*jmx;
    x         = CMAKE_N(double, ne);
    y         = CMAKE_N(double, ne);
    f         = CMAKE_N(double, ne);
    id        = 'A';
    lncolor   = SCR_dev->BLUE;
    lnwidth   = 0.0;
    lnstyle   = LINE_SOLID;
    centering = N_CENT;

    for (k = 0; k < imx; k++)
        for (l = 0; l < jmx; l++)
            {i    = l*imx + k;
             x[i] = k/10.0 - 5.0;
             y[i] = l/10.0 - 5.0;
             r    = x[i]*x[i] + y[i]*y[i];
             t    = 5.0*atan(y[i]/(x[i] + SMALL));
             r    = POW(r, 0.125);
             f[i] = exp(-r)*(1.0 + 0.1*cos(t));};

    PG_iso_limit(f, ne, &fmn, &fmx);

    data = PG_make_graph_r2_r1(id, "{x, y}->f", FALSE,
                               imx, jmx, centering, x, y, f, "xy", "f");

    data->info = PG_set_tds_info(data->info, PLOT_CONTOUR, CARTESIAN_2D,
				 lnstyle, lncolor, 10, 1.0,
				 lnwidth, 0.0, 0.0, 0.0, HUGE);

    PG_contour_plot(SCR_dev, data);
    PG_contour_plot(PS_dev, data);
    PG_contour_plot(CGM_dev, data);

    PRINT(stdout, "Hit enter to continue: ");
    SC_pause();

    PG_rl_graph(data, TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2D_NEW - test the new 2d contour plotting facility
 *             - METHOD = 2  color coded levels
 *             - METHOD = 3  filled contours
 */

static void test_2d_new(int method)
   {int i, j, l, imx, jmx, ne, nlev, mshp;
    int id, lncolor, lnstyle;
    double r, t, lnwidth, levels[2];
    double idn, jdn, fmn, fmx;
    double *x, *y, *f;
    PM_centering centering;
    PG_graph *data;

/* set up data */
#if 1
    imx       = 20;
    jmx       = 20;
#else
    imx       = 3;
    jmx       = 3;
#endif
    ne        = imx*jmx;
    x         = CMAKE_N(double, ne);
    y         = CMAKE_N(double, ne);
    f         = CMAKE_N(double, ne);
    id        = 'A';
    lncolor   = SCR_dev->BLUE;
    lnwidth   = 0.0;
    lnstyle   = LINE_SOLID;
    centering = N_CENT;

    idn = 2.0/((double) imx - 1.0);
    jdn = 2.0/((double) jmx - 1.0);

    for (i = 0; i < imx; i++)
        for (j = 0; j < jmx; j++)
            {l    = j*imx + i;
             x[l] = i*idn - 5.0;
             y[l] = j*idn - 5.0;
             r    = x[l]*x[l] + y[l]*y[l];
             t    = 5.0*atan(y[l]/(x[l] + SMALL));
             r    = POW(r, 0.125);
             f[l] = exp(-r)*(1.0 + 0.1*cos(t));};

    PG_iso_limit(f, ne, &fmn, &fmx);

    data = PG_make_graph_r2_r1(id, "{x, y}->f", FALSE,
                               imx, jmx, centering, x, y, f, "xy", "f");

    mshp = TRUE;

#if 1
    nlev  = 1;
    levels[0] = 0.2;
#else
    nlev  = 2;
    levels[0] = 0.19;
    levels[1] = 0.21;
#endif

/* setup the mapping data */
    PG_set_attrs_mapping(data->f,
			 "CONTOUR-METHOD", SC_INT_I, FALSE, method,
			 NULL);

/* setup the global graph data */
    data->info = PG_set_tds_info(data->info, PLOT_CONTOUR, CARTESIAN_2D,
				 lnstyle, lncolor, 10, 1.0,
				 lnwidth, 0.0, 0.0, 0.0, HUGE);

    PG_set_attrs_graph(data,
		       "DRAW-MESH", SC_INT_I, FALSE, mshp,
/*
		       "N-LEVELS",  SC_INT_I, FALSE, nlev,
		       "LEVELS",    SC_DOUBLE_I, TRUE,  levels,
*/
		       NULL);

    PG_contour_plot(SCR_dev, data);
    PG_contour_plot(PS_dev, data);
    PG_contour_plot(CGM_dev, data);

    PRINT(stdout, "Hit enter to continue: ");
    SC_pause();

    PG_rl_graph(data, TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3D_NEW - test the new 3d contour plotting facility */

static void test_3d_new(void)
   {int i, j, k, l, imx, jmx, kmx, ne, sz;
    int id, lncolor, lnstyle, nlev, count, fnc;
    double xc, yc, zc, fc, r, t, lnwidth, thl, phl;
    double idn, jdn, kdn, fmn, fmx;
    double levels[4];
    double *x, *y, *z, *f;
    char s[MAXLINE];
    char *ps;
    PM_centering centering;
    PG_rendering pty;
    PG_graph *data;

    sz = 20;

/* set up data */
    imx       = sz;
    jmx       = sz;
    kmx       = sz;
    ne        = imx*jmx*kmx;
    x         = CMAKE_N(double, ne);
    y         = CMAKE_N(double, ne);
    z         = CMAKE_N(double, ne);
    f         = CMAKE_N(double, ne);
    id        = 'A';
    lncolor   = SCR_dev->BLUE;
    lnwidth   = 0.0;
    lnstyle   = LINE_SOLID;
    centering = N_CENT;

    idn = 1.0/((double) imx - 1.0);
    jdn = 1.0/((double) jmx - 1.0);
    kdn = 1.0/((double) kmx - 1.0);

    fnc = SPHERE;
    fnc = LINEAR;
    fnc = HYPERBOLOID;

    for (i = 0; i < imx; i++)
        for (j = 0; j < jmx; j++)
	    for (k = 0; k < kmx; k++)
	        {l  = (k*jmx + j)*imx + i;
		 xc = i*idn;
		 yc = j*jdn;
		 zc = k*kdn;

		 if (fnc == SPHERE)
		    {r  = xc*xc + yc*yc + zc*zc;
		     fc = sqrt(r);}

		 else if (fnc == HYPERBOLOID)
		    {r  = zc*zc - (xc*xc + yc*yc);
		     if (r < 0.0)
		        fc = 0.0;
		     else
		        {t  = 4.0*atan(yc/(xc + SMALL));
			 r  = POW(r, 0.25);
			 fc = r*(1.0 + 0.1*cos(t));};}

		 else if (fnc == LINEAR)
		    {r  = zc - xc - 0.1*yc;
		     fc = r;};

		 x[l] = xc;
		 y[l] = yc;
		 z[l] = zc;
		 f[l] = fc;};

    PG_iso_limit(f, ne, &fmn, &fmx);

    data = PG_make_graph_r3_r1(id, "{x, y, z}->f", FALSE,
                               imx, jmx, kmx, centering, x, y, z, f,
			       "xyz", "f");

    pty = PLOT_SURFACE;
    thl = 45.0;
    phl = 45.0;

#if 0
 theta = 10.0,
 phi   = 135.0;

    nlev  = 1;
    levels[0] = 0.95;
/*    levels[0] = 0.75; */
#else
    nlev  = 4;
    levels[0] = 0.25;
    levels[1] = 0.50;
    levels[2] = 0.80;
    levels[3] = 0.95;
#endif

/* setup the global graph data */
    data->info = PG_set_tds_info(data->info, PLOT_CONTOUR, CARTESIAN_2D,
				 lnstyle, lncolor, 10, 1.0, lnwidth,
				 theta, phi, 0.0, HUGE);

    PG_set_attrs_graph(data,
		       "N-LEVELS",  SC_INT_I, FALSE, nlev,
		       "LEVELS",    SC_INT_I, TRUE,  levels,
		       NULL);

/* setup the mapping data */
    PG_set_attrs_mapping(data->f,
			 "PLOT-TYPE", SC_INT_I, FALSE, pty,
			 "THETA",     SC_DOUBLE_I,  FALSE, theta,
			 "PHI",       SC_DOUBLE_I,  FALSE, phi,
			 "CHI",       SC_DOUBLE_I,  FALSE, 0.0,
			 NULL);

    for (count = 0; TRUE; count++)
        {if (count > 0)
	    {PRINT(stdout, "Contour> ");
	     if (GETLN(s, MAXLINE, stdin) == NULL)
	        break;

	     ps = strtok(s, "\n");
	     if ((ps == NULL) || (ps[0] == '\0'))
	        break;
	     strcpy(s, ps);

	     switch (s[0])
	        {case 'a':
		      ps    = strtok(s+1, ", \t");
		      theta = SC_stof(ps);
		      ps    = strtok(NULL, ", \t");
		      phi   = SC_stof(ps);
		      break;
		 case 'l':
		      ps  = strtok(s+1, ", \t");
		      thl = SC_stof(ps);
		      ps  = strtok(NULL, ", \t");
		      phl = SC_stof(ps);
		      break;
	         case 'm':
		      pty = PLOT_WIRE_MESH;
		      break;
	         case 'n':
		      ps   = strtok(s+1, ", \t");
		      nlev = SC_stoi(ps);
		      break;
		 case 's':
		      pty = PLOT_SURFACE;
		      break;

		 case '?':
		      PRINT(stdout, "Commands:\n");
		      PRINT(stdout, "  a <theta> <phi>   set view angle\n");
		      PRINT(stdout, "  l <theta> <phi>   set lighting angle\n");
		      PRINT(stdout, "  m                 wire frame mesh\n");
		      PRINT(stdout, "  n <# levels>      set number of levels\n");
		      PRINT(stdout, "  s                 shaded surface\n");
		      PRINT(stdout, "  ?                 this help message\n");
		      break;

 	 	 default:
		      PRINT(stdout, "Bad command: %s\n", s);};};

/* setup the global graph data */
	 PG_set_attrs_graph(data,
			    "N-LEVELS",  SC_INT_I, FALSE, nlev,
			    "LEVELS",    SC_INT_I, TRUE,  levels,
			    NULL);

/* setup the mapping data */
	 PG_set_attrs_mapping(data->f,
			      "PLOT-TYPE",   SC_INT_I, FALSE, pty,
			      "THETA",       SC_DOUBLE_I,  FALSE, theta,
			      "PHI",         SC_DOUBLE_I,  FALSE, phi,
			      "CHI",         SC_DOUBLE_I,  FALSE, 0.0,
			      "THETA-LIGHT", SC_DOUBLE_I,  FALSE, thl,
			      "PHI-LIGHT",   SC_DOUBLE_I,  FALSE, phl,
			      NULL);

	 PG_contour_plot(SCR_dev, data);};

    PG_contour_plot(PS_dev, data);
    PG_contour_plot(CGM_dev, data);

    PG_rl_graph(data, TRUE, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAIN - start it off here */

int main(int c, char **v)
   {int i, sc;
    int fl2d, fl2ds, fl3d;
    char msg[MAXLINE];

    SC_setbuf(stdout, NULL);

    sc = SC_mem_monitor(-1, 2, "cont3d", msg);

    fl2d  = TRUE;
    fl2ds = TRUE;
    fl3d  = TRUE;

    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-2") == 0)
	    fl2d = FALSE;

	 else if (strcmp(v[i], "-s") == 0)
	    fl2ds = FALSE;

	 else if (strcmp(v[i], "-3") == 0)
	    fl3d = FALSE;

	 else if (strcmp(v[i], "-t") == 0)
	    {theta = SC_stof(v[++i]);
	     fl2d  = FALSE;
	     fl2ds = FALSE;}

	 else if (strcmp(v[i], "-p") == 0)
	    {phi   = SC_stof(v[++i]);
	     fl2d  = FALSE;
	     fl2ds = FALSE;};};

/* set up the graphics window */
    SCR_dev = PG_make_device("WINDOW", "COLOR", "PGS Contour Test");

    PG_white_background(SCR_dev, TRUE);
    PG_set_viewport_pos(SCR_dev, 0.250, 0.150);
    PG_set_viewport_shape(SCR_dev, 0.500, 0.0, 0.500/0.333);
    PG_set_border_width(SCR_dev, 5);

    PG_open_device(SCR_dev, 0.78, 0.002, 0.45, 0.45);

    PS_dev = PG_make_device("PS", "MONOCHROME", "gsctst");
    PG_open_device(PS_dev, 0.0, 0.0, 0.0, 0.0);

    CGM_dev = PG_make_device("CGM", "MONOCHROME", "gsctst");
    PG_open_device(CGM_dev, 0.0, 0.0, 0.0, 0.0);

    PG_set_pixmap_flag(SCR_dev, FALSE);

    if (fl2ds)
       test_2d_simple();

    if (fl2d)
       {test_2d_new(2);
	test_2d_new(3);};

    if (fl3d)
       test_3d_new();

    PG_close_device(SCR_dev);
    PG_close_device(PS_dev);
    PG_close_device(CGM_dev);

    PG_rl_all();

    sc = SC_mem_monitor(sc, 2, "cont3d", msg);

    exit(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
