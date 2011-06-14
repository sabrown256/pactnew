/*
 * GSATST.C - test of basic PGS drawing
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

#define TIMEOUT_GRAPHICS      180
#define TIMEOUT_FIN_PARALLEL  10

#define N       100
#define N_TEST  12
#define N_DEV   6

#define PAL(i) palettes[(i+palette)%(sizeof(palettes)/sizeof(char *))]

typedef int (*gtest)(PG_device *dev);
typedef int (*dtest)(char *name, int color, int debug_mode,
		     int nt, gtest *tst);

static int
 trm          = 0,
 mode         = 0,
 debug_mode   = 0,
 single_frame = 0,
 ids          = 0,
 palette      = 0,
 strategy     = 0;

static int
 sty[] = {LINE_SOLID, LINE_DOTTED, LINE_DASHED, LINE_DOTDASHED};

static char
 *palettes[] = {"rainbow", "bw", "iron", "contrast", "cym"},
 *clrs[] = {"BLACK", "WHITE", "GRAY", "DARK_GRAY",
            "BLUE", "GREEN", "CYAN", "RED", "MAGENTA", "BROWN",
            "DARK_BLUE", "DARK_GREEN", "DARK_CYAN", "DARK_RED",
            "YELLOW", "DARK_MAGENTA"};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPUTE_2D_DATA - compute data for 2D-1D rendering tests */

static void compute_2d_data(double *f, double *x, double *y,
			    int kmax, int lmax)
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

/* COMPUTE_2D_AC_MAPPING_2 - compute AC mapping for 2D-1D rendering tests
 *                         - using a 1 parameter boundary form
 *                         - this takes up more space but each cell
 *                         - is completely self-contained
 */

static PM_mapping *compute_2d_ac_mapping_2(double *x, double *y,
					   int kmax, int lmax)
   {int k, l, i, j, km, lm, io, ord, nd;
    int i1, i2, i3, i4;
    int id, ne, nc, jo[5];
    int *bnp, *bnc;
    long **bnd, *zone, *edge;
    double xmin, xmax, ymin, ymax;
    double dx, dy, tol, t, r, fv, xa, ya;
    double *f;
    PM_mesh_topology *mt;
    PM_set *dom, *ran;
    PM_mapping *h;

    km = kmax - 1;
    lm = lmax - 1;

    jo[0] = 0;
    jo[1] = 1;
    jo[2] = kmax + 1;
    jo[3] = kmax;
    jo[4] = 0;

    xmin = -5.0;
    xmax =  5.0;
    ymin = -5.0;
    ymax =  5.0;
    SC_ASSERT(ymax > ymin);

    dx = (xmax - xmin)/((double) kmax - 1.0);
    dy = (xmax - xmin)/((double) kmax - 1.0);
    nc = km*lm;

    tol = 1.0e-5;

    ord = 2;
    nd  = 2;

/* setup the number of boundary parameters array */
    bnp    = CMAKE_N(int, nd+1);
    bnp[0] = 1;
    for (id = 1; id <= nd; id++)
        bnp[id] = ord;

/* setup the number of cells array */
    bnc = CMAKE_N(int, nd+1);
    bnc[0] = kmax*lmax;
    bnc[1] = 4*nc;
    bnc[2] = nc;

/* allocate the boundary arrays */
    bnd    = CMAKE_N(long *, nd+1);
    bnd[0] = NULL;
    for (id = 1; id <= nd; id++)
        bnd[id] = CMAKE_N(long, ord*bnc[id]);

/* fill the zone list */
    zone = bnd[2];
    ne   = 0;
    for (i = 0; i < nc; i++)
        {zone[ord*i]     = ne;
	 zone[ord*i + 1] = ne + 3;
         ne += 4;}

/* fill the edge list */
    edge = bnd[1];
    for (k = 0; k < km; k++)
        for (l = 0; l < lm; l++)
	    {i  = k + l*km;
	     io = k + l*kmax;
	     for (j = 0; j < 4; j++)
	         {edge[ord*(4*i + j)]     = io + jo[j];
		  edge[ord*(4*i + j) + 1] = io + jo[j+1];};};

    mt = PM_make_topology(nd, bnp, bnc, bnd);

/* fill the points */
    for (k = 0; k < kmax; k++)
        for (l = 0; l < lmax; l++)
            {i    = l*kmax + k;
             x[i] = k*dx + xmin;
             y[i] = l*dy + ymin;};

    f = CMAKE_N(double, nc);
    for (i = 0; i < nc; i++)
        {i1 = edge[ord*(4*i + 0)];
         i2 = edge[ord*(4*i + 1)];
         i3 = edge[ord*(4*i + 2)];
         i4 = edge[ord*(4*i + 3)];

	 xa   = 0.25*(x[i1] + x[i2] + x[i3] + x[i4]);
	 ya   = 0.25*(y[i1] + y[i2] + y[i3] + y[i4]);
	 r    = sqrt(xa*xa + ya*ya);
	 t    = 5.0*atan(ya/(xa + SMALL));
	 r    = POW(r, 0.125);
	 fv   = exp(-r)*(1.0 + 0.1*cos(t));
	 f[i] = floor(fv/tol)*tol;};

/* put it all together */
    dom = PM_make_ac_set("Dac", SC_DOUBLE_S, FALSE, mt, nd, x, y);
    ran = PM_make_set("R", SC_DOUBLE_S, FALSE, 1, bnc[2], 1, f);
    h   = PM_make_mapping("Dac->R", PM_AC_S, dom, ran, Z_CENT, NULL);

    return(h);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPUTE_2D_AC_MAPPING_1 - compute AC mapping for 2D-1D rendering tests
 *                         - using a 1 parameter boundary form
 *                         - this takes up less space but each cell
 *                         - is partly defined by the cell that follows
 *                         - the next cell's indices mark the end of the
 *                         - current cell's index range
 *                         - NOTE: key point that in the 1 parameter form
 *                         - the edge list must formally close a face
 *                         - for example, a rectangular face will have
 *                         - 5 points on the edge list: a b c d a
 */

static PM_mapping *compute_2d_ac_mapping_1(double *x, double *y,
					   int kmax, int lmax)
   {int k, l, i, j, km, lm, io, nd;
    int i1, i2, i3, i4;
    int id, ne, nc, jo[5];
    int *bnp, *bnc;
    long **bnd, *zone, *edge;
    double xmin, xmax, ymin, ymax;
    double dx, dy, tol, t, r, fv, xa, ya;
    double *f;
    PM_mesh_topology *mt;
    PM_set *dom, *ran;
    PM_mapping *h;

    km = kmax - 1;
    lm = lmax - 1;

    jo[0] = 0;
    jo[1] = 1;
    jo[2] = kmax + 1;
    jo[3] = kmax;
    jo[4] = 0;

    xmin = -5.0;
    xmax =  5.0;
    ymin = -5.0;
    ymax =  5.0;
    SC_ASSERT(ymax > ymin);

    dx = (xmax - xmin)/((double) kmax - 1.0);
    dy = (xmax - xmin)/((double) kmax - 1.0);
    nc = km*lm;

    tol = 1.0e-5;
    nd  = 2;

/* setup the number of boundary parameters array */
    bnp    = CMAKE_N(int, nd+1);
    bnp[0] = 1;
    for (id = 1; id <= nd; id++)
        bnp[id] = 1;

/* setup the number of cells array */
    bnc = CMAKE_N(int, nd+1);
    bnc[0] = kmax*lmax;
    bnc[1] = 5*nc;
    bnc[2] = nc;

/* allocate the boundary arrays */
    bnd    = CMAKE_N(long *, nd+1);
    bnd[0] = NULL;
    for (id = 1; id <= nd; id++)
        bnd[id] = CMAKE_N(long, bnc[id]+1);

/* fill the zone list */
    zone = bnd[2];
    ne   = 0;
    for (i = 0; i < nc; i++)
        {zone[i]     = ne;
	 zone[i + 1] = ne + 4;
         ne += 5;}

/* fill the edge list */
    edge = bnd[1];
    for (k = 0; k < km; k++)
        for (l = 0; l < lm; l++)
	    {i  = k + l*km;
	     io = k + l*kmax;
	     for (j = 0; j < 5; j++)
	         edge[5*i + j] = io + jo[j];};

    mt = PM_make_topology(nd, bnp, bnc, bnd);

/* fill the points */
    for (k = 0; k < kmax; k++)
        for (l = 0; l < lmax; l++)
            {i    = l*kmax + k;
             x[i] = k*dx + xmin;
             y[i] = l*dy + ymin;};

    f = CMAKE_N(double, nc);
    for (i = 0; i < nc; i++)
        {i1 = edge[5*i + 0];
         i2 = edge[5*i + 1];
         i3 = edge[5*i + 2];
         i4 = edge[5*i + 3];

	 xa   = 0.25*(x[i1] + x[i2] + x[i3] + x[i4]);
	 ya   = 0.25*(y[i1] + y[i2] + y[i3] + y[i4]);
	 r    = sqrt(xa*xa + ya*ya);
	 t    = 5.0*atan(ya/(xa + SMALL));
	 r    = POW(r, 0.125);
	 fv   = exp(-r)*(1.0 + 0.1*cos(t));
	 f[i] = floor(fv/tol)*tol;};

/* put it all together */
    dom = PM_make_ac_set("Dac", SC_DOUBLE_S, FALSE, mt, nd, x, y);
    ran = PM_make_set("R", SC_DOUBLE_S, FALSE, 1, bnc[2], 1, f);
    h   = PM_make_mapping("Dac->R", PM_AC_S, dom, ran, Z_CENT, NULL);

    return(h);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_1 - line drawing test 
 *        - Modes: cartesian, polar, inselberg, histogram
 */

static int test_1(PG_device *dev)
   {int i, n, ip, np, imn, imx, ok;
    double *x, *y;
    PG_graph *data;

    if (dev == NULL)
       return(FALSE);

    n = N;

    x = CMAKE_N(double, n);
    y = CMAKE_N(double, n);

/* prepare the data */
    ip = PG_get_processor_number();
    np = PG_get_number_processors();
    n  /= np;
    imn = ip*n;
    imx = imn + n;

    for (i = imn; i < imx; i++)
        {x[i] = -(i+1)/10.0;
         y[i] = 6.022e23/x[i];};

    ok = TRUE;

    data = PG_make_graph_1d('A', "Test Data #1", FALSE, n,
                            x, y, "X Values", "Y Values");

    PG_set_attrs_glb(TRUE,
		     "plot-labels", TRUE,
		     NULL);

/* set up the hard copy device */
    PG_turn_data_id(dev, ids);

    data->info = (void *) PG_set_line_info(data->info, PLOT_CARTESIAN, CARTESIAN_2D,
					   LINE_SOLID, FALSE, 0, dev->BLUE,
					   0, 0.0);
    PG_draw_graph(dev, data);

    data->info = (void *) PG_set_line_info(data->info, PLOT_POLAR, POLAR,
					   LINE_SOLID, FALSE, 0, dev->GREEN,
					   0, 0.0);
    PG_draw_graph(dev, data);

    data->info = (void *) PG_set_line_info(data->info, PLOT_INSEL, INSEL,
					   LINE_SOLID, FALSE, 0, dev->RED,
					   0, 0.0);
    PG_draw_graph(dev, data);

    data->info = (void *) PG_set_line_info(data->info, PLOT_HISTOGRAM, CARTESIAN_2D,
					   LINE_SOLID, FALSE, 0, dev->WHITE,
					   0, 0.0);
    PG_draw_graph(dev, data);

    PG_rl_graph(data, TRUE, TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - line drawing test 
 *        - Modes: cartesian, polar, inselberg, histogram
 *        - with grids on
 */

static int test_2(PG_device *dev)
   {int i, n, ip, np, imn, imx, ok;
    double dx;
    double *x, *y;
    PG_graph *data;

    if (dev == NULL)
       return(FALSE);

    n = N;

    x = CMAKE_N(double, n);
    y = CMAKE_N(double, n);

/* prepare the data */
    ip = PG_get_processor_number();
    np = PG_get_number_processors();
    n  /= np;
    imn = ip*n;
    imx = imn + n;

    dx = 0.1*((double) (imx - imn))/((double) N - 1.0);

    for (i = imn; i < imx; i++)
        {x[i] = i*dx;
         y[i] = cos(x[i]);};

    ok = TRUE;

    data = PG_make_graph_1d('B', "Test Data #2", FALSE, n,
                            x, y, "X Values", "Y Values");

    PG_set_attrs_glb(TRUE,
		     "plot-labels", TRUE,
		     NULL);

    PG_turn_grid(dev, ON);

    data->info = (void *) PG_set_line_info(data->info, PLOT_HISTOGRAM, CARTESIAN_2D,
					   LINE_SOLID, FALSE, 0, dev->MAGENTA,
					   1, 0.0);
    PG_draw_graph(dev, data);

    data->info = (void *) PG_set_line_info(data->info, PLOT_POLAR, POLAR,
					   LINE_SOLID, FALSE, 0, dev->CYAN,
					   0, 0.0);
    PG_draw_graph(dev, data);

    data->info = (void *) PG_set_line_info(data->info, PLOT_INSEL, INSEL,
					   LINE_SOLID, FALSE, 0, dev->YELLOW,
					   0, 0.0);
    PG_draw_graph(dev, data);

    data->info = (void *) PG_set_line_info(data->info, PLOT_CARTESIAN, CARTESIAN_2D,
					   LINE_SOLID, FALSE, 0, dev->DARK_GRAY,
					   0, 0.0);

    PG_draw_graph(dev, data);

    PG_rl_graph(data, TRUE, TRUE);

    PG_turn_grid(dev, OFF);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - image drawing tests */

static int test_3(PG_device *dev)
   {int i, j, w, h, ok;
    unsigned char *bf;
    double dx, dy, xv, yv, sf;
    double dbx[PG_BOXSZ], rbx[PG_BOXSZ];
    PG_palette *pl;
    PG_image *calc_im;

    if (dev == NULL)
       return(FALSE);

    ok = TRUE;

/* prepare the data */
    w  = 250;
    h  = 250;
    sf = 255;

    PG_box_init(2, dbx, 0.0, 0.0);
    PG_box_init(1, rbx, 0.0, sf);

    calc_im = PG_make_image_n("Test Image", SC_CHAR_S, NULL,
			      2, WORLDC, dbx, rbx, w, h, 4, NULL);
    if (calc_im == NULL)
       {PRINT(STDOUT, "CAN'T ALLOCATE IMAGE");
	exit(1);};

    pl = PG_set_palette(dev, PAL(1));
    SC_ASSERT(pl != NULL);

/* compute the first image */
    bf = calc_im->buffer;
    for (j = 0; j < h; j++)
        {for (i = 0; i < w; i++)
	     *bf++ = sf*((double) i)*((double) j)/
	                ((double) (h - 1)*(w - 1));};

/* draw the first image */
    PG_draw_image(dev, calc_im, "Test Data A", NULL);

    pl = PG_set_palette(dev, PAL(2));
    SC_ASSERT(pl != NULL);

/* compute the second image */
    dx = 2.0*PI/((double) w);
    dy = 2.0*PI/((double) h);
    bf = calc_im->buffer;
    for (j = 0; j < h; j++)
        {for (i = 0; i < w; i++)
	     {xv = ((double) i)*dx;
	      yv = ((double) (h - j - 1))*dy;

	      *bf++ = sf*(0.5 + 0.5*sin(xv)*cos(yv));};};

    PG_set_attrs_glb(TRUE,
		     "plot-labels", TRUE,
		     NULL);

/* draw the second image */
    PG_draw_image(dev, calc_im, "Test Data B", NULL);

    PG_rl_image(calc_im);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SF_DT - set the font and draw the text */

static void sf_dt(PG_device *dev, double x1, double y1,
		  char *face, char *style, int size)
   {double bx[PG_BOXSZ], dx[PG_SPACEDM], p[PG_SPACEDM];

    PG_fset_font(dev, face, style, size);

    PG_get_text_ext_n(dev, 2, WORLDC, "foo", dx);

    bx[0] = x1;
    bx[1] = x1 + dx[0];
    bx[2] = y1;
    bx[3] = y1 + dx[1];

/* write some text and draw a box around it */
    p[0] = bx[0];
    p[1] = bx[2];
    PG_write_n(dev, 2, WORLDC, p, "%s", "foo");

    PG_draw_box_n(dev, 2, WORLDC, bx);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4_AUX - draw lots of text */

static void test_4_aux(PG_device *dev)
   {

    if (dev == NULL)
       return;

    PG_fset_line_color(dev, dev->WHITE, TRUE);
    PG_fset_text_color(dev, dev->WHITE, TRUE);

    sf_dt(dev, 0.1, 0.9, "helvetica", "medium", 12);
    sf_dt(dev, 0.1, 0.8, "helvetica", "italic", 12);
    sf_dt(dev, 0.1, 0.7, "helvetica", "bold", 12);
    sf_dt(dev, 0.1, 0.6, "helvetica", "bold-italic", 12);

    sf_dt(dev, 0.1, 0.50, "helvetica", "medium", 10);
    sf_dt(dev, 0.1, 0.45, "helvetica", "italic", 10);
    sf_dt(dev, 0.1, 0.40, "helvetica", "bold", 10);
    sf_dt(dev, 0.1, 0.35, "helvetica", "bold-italic", 10);

    sf_dt(dev, 0.1, 0.30, "helvetica", "medium", 8);
    sf_dt(dev, 0.1, 0.25, "helvetica", "italic", 8);
    sf_dt(dev, 0.1, 0.20, "helvetica", "bold", 8);
    sf_dt(dev, 0.1, 0.15, "helvetica", "bold-italic", 8);

    sf_dt(dev, 0.3, 0.9, "times", "medium", 12);
    sf_dt(dev, 0.3, 0.8, "times", "italic", 12);
    sf_dt(dev, 0.3, 0.7, "times", "bold", 12);
    sf_dt(dev, 0.3, 0.6, "times", "bold-italic", 12);

    sf_dt(dev, 0.3, 0.50, "times", "medium", 10);
    sf_dt(dev, 0.3, 0.45, "times", "italic", 10);
    sf_dt(dev, 0.3, 0.40, "times", "bold", 10);
    sf_dt(dev, 0.3, 0.35, "times", "bold-italic", 10);

    sf_dt(dev, 0.3, 0.30, "times", "medium", 8);
    sf_dt(dev, 0.3, 0.25, "times", "italic", 8);
    sf_dt(dev, 0.3, 0.20, "times", "bold", 8);
    sf_dt(dev, 0.3, 0.15, "times", "bold-italic", 8);

    sf_dt(dev, 0.5, 0.9, "courier", "medium", 12);
    sf_dt(dev, 0.5, 0.8, "courier", "italic", 12);
    sf_dt(dev, 0.5, 0.7, "courier", "bold", 12);
    sf_dt(dev, 0.5, 0.6, "courier", "bold-italic", 12);

    sf_dt(dev, 0.5, 0.50, "courier", "medium", 10);
    sf_dt(dev, 0.5, 0.45, "courier", "italic", 10);
    sf_dt(dev, 0.5, 0.40, "courier", "bold", 10);
    sf_dt(dev, 0.5, 0.35, "courier", "bold-italic", 10);

    sf_dt(dev, 0.5, 0.30, "courier", "medium", 8);
    sf_dt(dev, 0.5, 0.25, "courier", "italic", 8);
    sf_dt(dev, 0.5, 0.20, "courier", "bold", 8);
    sf_dt(dev, 0.5, 0.15, "courier", "bold-italic", 8);

    PG_update_vs(dev);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - text tests */

static int test_4(PG_device *dev)
   {int i, ok;
    double an;
    double x0[PG_SPACEDM], ndc[PG_BOXSZ], wc[PG_BOXSZ];

    if (dev == NULL)
       return(FALSE);

/* connect the I/O functions */
/*    PG_open_console("PGS Test", "COLOR", TRUE, 0.1, 0.7, 0.5, 0.3); */

    ok = TRUE;

/* do simple text test */

    PG_clear_screen(dev);

    ndc[0] = 0.1;
    ndc[1] = 0.9;
    ndc[2] = 0.2;
    ndc[3] = 0.8;
    PG_set_viewspace(dev, 2, NORMC, ndc);
    PG_set_viewspace(dev, 2, WORLDC, NULL);

    PG_box_init(2, wc, -0.02, 1.02);
    PG_draw_box_n(dev, 2, WORLDC, wc);

/*    PG_expose_device(PG_console_device); */

    test_4_aux(dev);

/* reset for following tests */
    PG_fset_font(dev, "helvetica", "medium", 10);

    PG_finish_plot(dev);

/* do oriented text test */
    PG_clear_screen(dev);
    wc[0] = -3.0;
    wc[1] =  3.0;
    wc[2] = -3.0;
    wc[3] =  3.0;
    PG_set_viewspace(dev, 2, WORLDC, wc);

    for (i = 0; i < 8; i++)
        {an = i*PI/4.0;
         x0[0] = cos(an);
         x0[1] = sin(an);
         PG_fset_char_path(dev, x0);
         PG_write_n(dev, 2, WORLDC, x0, "Sample");};

/* reset for following tests */
    x0[0] = 1.0;
    x0[1] = 0.0;
    PG_fset_char_path(dev, x0);

    PG_finish_plot(dev);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_5 - contour plot test */

static int test_5(PG_device *dev)
   {int kmax, lmax, kxl;
    int id, lncolor, lnstyle;
    double *x, *y, *f, lnwidth;
    double fmin, fmax;
    PM_centering centering;
    PG_graph *data;

    if (dev == NULL)
       return(FALSE);

/* set up data */
    kmax      = 20;
    lmax      = 20;
    kxl       = kmax*lmax;
    x         = CMAKE_N(double, kxl);
    y         = CMAKE_N(double, kxl);
    f         = CMAKE_N(double, kxl);
    id        = 'A';
    lncolor   = dev->BLUE;
    lnwidth   = 0.0;
    lnstyle   = LINE_SOLID;
    centering = N_CENT;

    compute_2d_data(f, x, y, kmax, lmax);

    PG_iso_limit(f, kxl, &fmin, &fmax);

    data = PG_make_graph_r2_r1(id, "{x, y}->f", FALSE,
                               kmax, lmax, centering, x, y, f, "xy", "f");

    data->info = (void *) PG_set_tds_info(data->info, PLOT_CONTOUR, CARTESIAN_2D,
					  lnstyle, lncolor, 10, 1.0,
					  lnwidth, 0.0, 0.0, 0.0, HUGE);

    PG_set_attrs_glb(TRUE,
		     "plot-labels", FALSE,
		     NULL);

    PG_turn_data_id(dev, OFF);

    PG_contour_plot(dev, data);

    PG_turn_data_id(dev, ON);

    PG_rl_graph(data, TRUE, TRUE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_6 - vector plot test */

static int test_6(PG_device *dev)
   {int i, k, l, kmax, lmax, kxl;
    int id, lncolor, lnstyle;
    double km, lm, lnwidth;
    double *x, *y, *u, *v;
    PM_centering centering;
    PM_set *domain, *range;
    PG_graph *data;

    if (dev == NULL)
       return(FALSE);

/* set up data */
    kmax      = 20;
    lmax      = 20;
    id        = 'A';
    lncolor   = dev->BLUE;
    lnwidth   = 0.0;
    lnstyle   = LINE_SOLID;
    centering = N_CENT;

    kxl = kmax*lmax;
    x   = CMAKE_N(double, kxl);
    y   = CMAKE_N(double, kxl);
    u   = CMAKE_N(double, kxl);
    v   = CMAKE_N(double, kxl);

    km = 2.0*PI/((double) kmax - 1.0);
    lm = 2.0*PI/((double) lmax - 1.0);
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

    data->info = (void *) PG_set_tdv_info(data->info, PLOT_VECTOR, CARTESIAN_2D,
					  lnstyle, lncolor, lnwidth);

    PG_set_attrs_glb(TRUE,
		     "plot-labels", TRUE,
		     NULL);

    PG_vector_plot(dev, data);

    PG_rl_graph(data, TRUE, TRUE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_7 - fill-poly plot test */

static int test_7(PG_device *dev)
   {int kmax, lmax, kxl, id;
    double *x, *y, *f;
    PM_centering centering;
    PG_graph *data;

    if (dev == NULL)
       return(FALSE);

/* set up data */
    centering = N_CENT;
    kmax      = 20;
    lmax      = 20;
    kxl       = kmax*lmax;
    x         = CMAKE_N(double, kxl);
    y         = CMAKE_N(double, kxl);
    f         = CMAKE_N(double, kxl);
    id        = 'A';

    compute_2d_data(f, x, y, kmax, lmax);

    data = PG_make_graph_r2_r1(id, "{x, y}->f", FALSE,
			       kmax, lmax, centering, x, y, f, "xy", "f");

    PG_set_attrs_glb(TRUE,
		     "plot-labels", FALSE,
		     NULL);

    PG_set_palette(dev, PAL(3));
    PG_poly_fill_plot(dev, data);

    PG_rl_graph(data, TRUE, TRUE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_8 - surface plot test */

static int test_8(PG_device *dev)
   {int id, k, l, kmax, lmax, kxl;
    int maxes[2];
    char *emap;
    double theta, phi, chi;
    double dtheta, dphi, dchi;
    double va[PG_NANGLES];
    double **r, *rs, *pz, *ps, *ext;
    pcons *alist;

    if (dev == NULL)
       return(FALSE);

    kmax = 10;
    lmax = 10;
    kxl  = kmax*lmax;

/* make the data */
    r = PM_make_vectors(3, kxl);
    pz = r[2];
    rs = ps = CMAKE_N(double, kxl);

    for (id = 0; id < 3; id++)
        SC_mark(r[id], 1);
    SC_mark(rs, 1);

/* load the data */
    compute_2d_data(r[2], r[0], r[1], kmax, lmax);

    for (l = 0; l < lmax; l++)
        for (k = 0; k < kmax; k++)
	    *ps++ = exp(*pz++);

    maxes[0] = kmax;
    maxes[1] = lmax;

    emap = CMAKE_N(char, kxl);
    memset(emap, 1, kxl);
    alist = PG_set_attrs_alist(NULL,
			      "EXISTENCE", SC_CHAR_I, TRUE, emap,
			      NULL);

    ext = CMAKE_N(double, 4);
    ext[0] = -1;
    ext[1] =  1;
    ext[2] = -1;
    ext[3] =  1;

    if (single_frame)
       {dtheta = HUGE;
	dphi   = HUGE;;
	dchi   = HUGE;;}
    else
       {dtheta = 45.0;
	dphi   = 45.0;
	dchi   = 30.0;};

    PG_set_attrs_glb(TRUE,
		     "plot-labels", FALSE,
		     NULL);

/* draw the wire frames */
    PG_set_palette(dev, "standard");

    for (theta = 0.0; theta <= 90.0; theta += dtheta)
        for (phi = 0.0; phi <= 90.0; phi += dphi)
	    for (chi = 30.0; chi <= 60.0; chi += dchi)
	        {va[0] = theta;
		 va[1] = phi;
		 va[2] = chi;
		 PG_clear_screen(dev);
		 PG_draw_surface_n(dev, r[2], r[2], ext, r, kxl,
				   va, 0.0, dev->BLUE, LINE_SOLID,
				   PLOT_WIRE_MESH, "Test Data",
				   PM_LR_S, maxes, alist);};

/* draw the surface frames */
    if (!single_frame)
       {PG_set_palette(dev, PAL(4));

	for (theta = 0.0; theta <= 90.0; theta += dtheta)
	    for (phi = 0.0; phi <= 90.0; phi += dphi)
	        for (chi = 30.0; chi <= 60.0; chi += dchi)
		    {va[0] = theta;
		     va[1] = phi;
		     va[2] = chi;
		     PG_clear_screen(dev);
		     PG_draw_surface_n(dev, r[2], rs, ext, r, kxl,
				       va, 0.0, dev->BLUE, LINE_SOLID,
				       PLOT_SURFACE, "Test Data",
				       PM_LR_S, maxes, alist);};};

    PM_free_vectors(3, r);
    CFREE(rs);
    CFREE(ext);
    CFREE(emap);
    SC_free_alist(alist, 3);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_9 - mesh plot test */

static int test_9(PG_device *dev)
   {int i, k, l, m, kmax, lmax, mmax, kxl;
    int id, lncolor, lnstyle;
    double xmin, xmax, ymin, ymax, dx, dy;
    double lnwidth, theta, phi, chi;
    double *x, *y, *z, *f;
    PM_centering centering;
    PM_set *domain, *range;
    PG_graph *data;

    if (dev == NULL)
       return(FALSE);

/* set up data */
    id        = 'A';
    lncolor   = dev->BLUE;
    lnwidth   = 0.0;
    lnstyle   = LINE_SOLID;
    centering = N_CENT;

/* construct 2d mesh */
    kmax      = 5;
    lmax      = 5;
    xmin      = -5.0;
    xmax      = 5.0;
    ymin      = -5.0;
    ymax      = 5.0;
    kxl       = kmax*lmax;
    x         = CMAKE_N(double, kxl);
    y         = CMAKE_N(double, kxl);
    f         = CMAKE_N(double, kxl);

    SC_ASSERT(ymax > ymin);

    dx = (xmax - xmin)/((double) kmax - 1.0);
    dy = (xmax - xmin)/((double) kmax - 1.0);

    for (k = 0; k < kmax; k++)
        for (l = 0; l < lmax; l++)
            {i    = l*kmax + k;
             x[i] = k*dx + xmin;
             y[i] = l*dy + ymin;
             f[i] = 0.0;};

    domain = PM_make_set("{x, y}", SC_DOUBLE_S, FALSE,
			 2, kmax, lmax, 2, x, y);

    range = PM_make_set("f", SC_DOUBLE_S, FALSE,
			2, kmax, lmax, 1, f);

    data = PG_make_graph_from_sets("{x, y}->f", domain, range,
				   centering,
				   SC_PCONS_P_S, NULL, id, NULL);

    data->info = (void *) PG_set_tds_info(data->info, PLOT_MESH, CARTESIAN_2D,
					  lnstyle, lncolor, 10, 1.0,
					  lnwidth, 0.0, 0.0, 0.0, HUGE);

    PG_set_attrs_glb(TRUE,
		     "plot-labels", FALSE,
		     NULL);

    PG_mesh_plot(dev, data);

    PG_rl_graph(data, TRUE, TRUE);

/* construct 3d mesh */
    kmax      = 5;
    lmax      = 6;
    mmax      = 7;
    xmin      = -5.0;
    xmax      = 3.0;
    ymin      = -3.0;
    ymax      = 5.0;
    kxl       = kmax*lmax*mmax;
    x         = CMAKE_N(double, kxl);
    y         = CMAKE_N(double, kxl);
    z         = CMAKE_N(double, kxl);
    f         = CMAKE_N(double, kxl);

    dx = (xmax - xmin)/((double) kmax - 1.0);
    dy = (xmax - xmin)/((double) kmax - 1.0);

    for (k = 0; k < kmax; k++)
        for (l = 0; l < lmax; l++)
	    for (m = 0; m < mmax; m++)
	        {i    = (m*lmax + l)*kmax + k;
		 x[i] = k*dx + xmin;
		 y[i] = l*dy + ymin;
		 z[i] = m;
		 f[i] = 0.0;};

    domain = PM_make_set("{x, y, z}", SC_DOUBLE_S, FALSE,
			 3, kmax, lmax, mmax, 3, x, y, z);

    range = PM_make_set("f", SC_DOUBLE_S, FALSE,
			3, kmax, lmax, mmax, 1, f);

    data = PG_make_graph_from_sets("{x, y, z}->f", domain, range,
				   centering,
				   SC_PCONS_P_S, NULL, id, NULL);

    theta = 30.0;
    phi   = 45.0;
    chi   = 60.0;

    data->info = (void *) PG_set_tds_info(data->info, PLOT_MESH, CARTESIAN_2D,
					  lnstyle, lncolor, 10, 1.0, lnwidth,
					  theta, phi, chi,
					  HUGE);

    PG_mesh_plot(dev, data);

    PG_rl_graph(data, TRUE, TRUE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_10 - line width/color/style test */

static int test_10(PG_device *dev)
   {int i, j, n;
    double y, dy, w;
    double ndc[PG_BOXSZ];
    double x1[PG_SPACEDM], x2[PG_SPACEDM], p[PG_SPACEDM];

    if (dev == NULL)
       return(FALSE);

    ndc[0] = 0.05;
    ndc[1] = 0.95;
    ndc[2] = 0.05;
    ndc[3] = 0.95;
    PG_set_viewspace(dev, 2, NORMC, ndc);
    PG_set_viewspace(dev, 2, WORLDC, NULL);
    PG_set_palette(dev, "standard");

/* colors */
    PG_clear_screen(dev);

    x1[0] = 0.0;
    x2[0] = 0.5;

    n  = 16;
    dy = 1.0/(n + 1.0);
    y  = 0.5*dy;
    for (i = 0; i < n; i++)
        {PG_fset_line_color(dev, i, TRUE);
	 x1[1] = y;
	 x2[1] = y;
	 PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);

         PG_fset_text_color(dev, i, TRUE);

	 p[0] = 0.6;
	 p[1] = y;
         PG_write_n(dev, 2, WORLDC, p, "%d %s", i, clrs[i]);

         y += dy;};

    PG_finish_plot(dev);

/* widths and styles */
    PG_fset_line_color(dev, dev->WHITE, TRUE);
    PG_clear_screen(dev);

    n  = 10;
    dy = 1.0/(n + 1.0);
    y  = 0.5*dy;
    for (i = 0; i < n; i++)
        {w = 0.3*i;
	 PG_fset_line_width(dev, w);
	 for (j = 0; j < 4; j++)
	     {x1[0] = 0.25*j + 0.05;
	      x1[1] = y;
	      x2[0] = 0.25*(j+1) - 0.05;
	      x2[1] = y;

	      PG_fset_line_style(dev, sty[j]);
	      PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);};

         y += dy;};

    PG_finish_plot(dev);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_11 - plotting AC data sets */

static int test_11(PG_device *dev)
   {int kmax, lmax, kxl;
    double *x, *y;
    PM_mapping *f;

    if (dev == NULL)
       return(FALSE);

/* set up data */
    kmax = 11;
    lmax = 11;
    kxl  = kmax*lmax;
    x    = CMAKE_N(double, kxl);
    y    = CMAKE_N(double, kxl);

    f = compute_2d_ac_mapping_2(x, y, kmax, lmax);

    PG_turn_data_id(dev, OFF);

    PG_set_attrs_mapping(f,
			 "PALETTE", SC_CHAR_I, TRUE, "rainbow",
			 NULL);

    PG_set_attrs_glb(TRUE,
		     "plot-labels", FALSE,
		     NULL);

    PG_draw_picture(dev, f, PLOT_CONTOUR,
		    TRUE, dev->BLUE, LINE_SOLID, 0.4,
		    TRUE, dev->RED, LINE_DOTTED, 0.1);

    PG_draw_picture(dev, f, PLOT_FILL_POLY,
		    TRUE, dev->BLUE, LINE_SOLID, 0.4,
		    TRUE, dev->RED, LINE_DOTTED, 0.1);

    PG_draw_picture(dev, f, PLOT_SURFACE,
		    TRUE, dev->BLUE, LINE_SOLID, 0.4,
		    TRUE, dev->RED, LINE_DOTTED, 0.1);

    PG_draw_picture(dev, f, PLOT_WIRE_MESH,
		    TRUE, dev->BLUE, LINE_SOLID, 0.4,
		    TRUE, dev->RED, LINE_DOTTED, 0.1);

    PG_draw_picture(dev, f, PLOT_MESH,
		    TRUE, dev->BLUE, LINE_SOLID, 0.4,
		    TRUE, dev->RED, LINE_DOTTED, 0.1);

    PM_rel_mapping(f, TRUE, TRUE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_12 - plotting AC data sets */

static int test_12(PG_device *dev)
   {int kmax, lmax, kxl;
    double *x, *y;
    PM_mapping *f;

    if (dev == NULL)
       return(FALSE);

/* set up data */
    kmax = 11;
    lmax = 11;
/*
    kmax = 3;
    lmax = 3;
*/
    kxl = kmax*lmax;
    x   = CMAKE_N(double, kxl);
    y   = CMAKE_N(double, kxl);

    f = compute_2d_ac_mapping_1(x, y, kmax, lmax);

    PG_set_attrs_mapping(f,
			 "PALETTE", SC_CHAR_I, TRUE, "rainbow",
			 NULL);

    PG_set_attrs_glb(TRUE,
		     "plot-labels", FALSE,
		     NULL);

    PG_draw_picture(dev, f, PLOT_CONTOUR,
		    TRUE, dev->BLUE, LINE_SOLID, 0.4,
		    TRUE, dev->RED, LINE_DOTTED, 0.1);

    PG_draw_picture(dev, f, PLOT_FILL_POLY,
		    TRUE, dev->BLUE, LINE_SOLID, 0.4,
		    TRUE, dev->RED, LINE_DOTTED, 0.1);

    PG_draw_picture(dev, f, PLOT_SURFACE,
		    TRUE, dev->BLUE, LINE_SOLID, 0.4,
		    TRUE, dev->RED, LINE_DOTTED, 0.1);

    PG_draw_picture(dev, f, PLOT_WIRE_MESH,
		    TRUE, dev->BLUE, LINE_SOLID, 0.4,
		    TRUE, dev->RED, LINE_DOTTED, 0.1);

    PG_draw_picture(dev, f, PLOT_MESH,
		    TRUE, dev->BLUE, LINE_SOLID, 0.4,
		    TRUE, dev->RED, LINE_DOTTED, 0.1);

    PM_rel_mapping(f, TRUE, TRUE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_TEST - run the sequence of tests for the specified device */

static int run_test(PG_device *dev, int nt, gtest *tst)
   {int i, err;
    gtest t;

    err = TRUE;

    for (i = 0; i < nt; i++)
        {t = tst[i];
	 if (t != NULL)
	    err &= (*t)(dev);};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_CHECK - begin/end memory checks for each test */

static long test_check(char *name, long which, int debug_mode)
   {long bytaa, bytfa, rv;
    char msg[MAXLINE];
    static long bytab, bytfb;

    rv = 0;

    if (debug_mode)
       rv = SC_mem_monitor(which, 2, "G", msg);

    if (which == -1L)
       SC_mem_stats(&bytab, &bytfb, NULL, NULL);

    else if (which != -1L)
       {SC_mem_stats(&bytaa, &bytfa, NULL, NULL);

	bytaa -= bytab;
	bytfa -= bytfb;

	ONCE_SAFE(TRUE, NULL)
	   PRINT(STDOUT, "\n\n\t\t   Allocated      Freed       Diff\n");
	END_SAFE;

	PRINT(STDOUT, "\t%s\t   %9d  %9d  %9d\n",
	      name, bytaa, bytfa, bytaa - bytfa);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* REGISTER_DEVICES - register the devices needed for the tests */

static void register_devices(void)
   {

#if 0
    PG_register_device("JPEG",  PG_setup_jpeg_device);
    PG_register_device("PNG",   PG_setup_png_device);
#endif

    PG_register_device("IMAGE", PG_setup_image_device);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCR_TEST - test the SCREEN device */

static int scr_test(char *name, int color, int debug_mode, int nt,
		    gtest *tst)
   {int ok;
    long sc;
    PG_device *dev;

    ok = TRUE;

    sc = test_check(name, -1L, debug_mode);

    register_devices();

    if (color)
       dev = PG_make_device("SCREEN", "COLOR", name);
    else
       dev = PG_make_device("SCREEN", "MONOCHROME", name);

    PG_open_device(dev, 0.01, 0.01, 0.3, 0.3);

    ok &= run_test(dev, nt, tst);

    PG_close_device(dev);

    PG_rl_all();

    sc = test_check(name, 1L, debug_mode);
    SC_ASSERT(sc == TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PS_TEST - test the PostScript device */

static int ps_test(char *name, int color, int debug_mode, int nt,
		   gtest *tst)
   {int ok;
    long sc;
    PG_device *dev;

    ok = TRUE;

    sc = test_check(name, -1L, debug_mode);

    register_devices();

    if (color)
       {if (mode == PORTRAIT_MODE)
	   dev = PG_make_device("PS", "COLOR", name);
	else
	   dev = PG_make_device("PS", "COLOR LANDSCAPE", name);}
    else
       {if (mode == PORTRAIT_MODE)
	   dev = PG_make_device("PS", "MONOCHROME PORTRAIT", name);
	else
	   dev = PG_make_device("PS", "MONOCHROME LANDSCAPE", name);};

    PG_open_device(dev, 0.0, 0.0, 0.0, 0.0);

    ok &= run_test(dev, nt, tst);

    PG_close_device(dev);

    PG_rl_all();

    sc = test_check(name, 1L, debug_mode);
    SC_ASSERT(sc == TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CGM_TEST - test the CGM device */

static int cgm_test(char *name, int color, int debug_mode, int nt,
		    gtest *tst)
   {int ok;
    long sc;
    PG_device *dev;

    ok = TRUE;

    sc = test_check(name, -1L, debug_mode);

    register_devices();

    if (color)
       dev = PG_make_device("CGM", "COLOR", name);
    else
       dev = PG_make_device("CGM", "MONOCHROME", name);

    PG_open_device(dev, 0.0, 0.0, 0.0, 0.0);

    ok &= run_test(dev, nt, tst);

    PG_close_device(dev);

    PG_rl_all();

    sc = test_check(name, 1L, debug_mode);
    SC_ASSERT(sc == TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JPEG_TEST - test the JPEG device */

static int jpeg_test(char *name, int color, int debug_mode, int nt,
		     gtest *tst)
   {int ok;
    long sc;
    PG_device *dev;

    ok = TRUE;
    sc = test_check(name, -1L, debug_mode);

    register_devices();

    if (color)
       dev = PG_make_device("JPEG", "COLOR", name);
    else
       dev = PG_make_device("JPEG", "MONOCHROME", name);

    if (dev != NULL)
       {PG_open_device(dev, 0.0, 0.0, 400.0, 400.0);

	ok &= run_test(dev, nt, tst);

	PG_close_device(dev);

	PG_rl_all();

	sc = test_check(name, 1L, debug_mode);
	SC_ASSERT(sc == TRUE);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PNG_TEST - test the PNG device */

static int png_test(char *name, int color, int debug_mode, int nt,
		    gtest *tst)
   {int ok;
    long sc;
    PG_device *dev;

    ok = TRUE;
    sc = test_check(name, -1L, debug_mode);

    register_devices();

    if (color)
       dev = PG_make_device("PNG", "COLOR", name);
    else
       dev = PG_make_device("PNG", "MONOCHROME", name);

    if (dev != NULL)
       {PG_open_device(dev, 0.0, 0.0, 400.0, 400.0);

	ok &= run_test(dev, nt, tst);

	PG_close_device(dev);

	PG_rl_all();

	sc = test_check(name, 1L, debug_mode);
	SC_ASSERT(sc == TRUE);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* IMAGE_TEST - test the IMAGE device */

static int image_test(char *name, int color,int debug_mode,int nt,
		      gtest *tst)
   {int ok, np;
    long sc;
    PG_device *dev;

    ok = TRUE;
    np = PG_get_number_processors();
    if (np == 1)
       {sc = test_check(name, -1L, debug_mode);

	register_devices();

	if (color)
	   dev = PG_make_device("IMAGE", "COLOR", name);
	else
	   dev = PG_make_device("IMAGE", "MONOCHROME", name);

	if (dev != NULL)
	   {PG_open_device(dev, 0.0, 0.0, 400.0, 400.0);

	    ok &= run_test(dev, nt, tst);

	    PG_close_device(dev);

	    PG_rl_all();

	    sc = test_check(name, 1L, debug_mode);
	    SC_ASSERT(sc == TRUE);};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print the help message */

void help(void)
   {

    PRINT(STDOUT, "Usage: gsatst [-cdhilABCDEF123456789] [-n #] [-p #] [-r #] [-s] [-t #]\n");
    PRINT(STDOUT, "   -c  do all tests in COLOR mode\n");
    PRINT(STDOUT, "   -d  debug memory usage\n");
    PRINT(STDOUT, "   -h  this help message\n");
    PRINT(STDOUT, "   -i  display data-ids in plots\n");
    PRINT(STDOUT, "   -n  number of processors to use/simulate\n");
    PRINT(STDOUT, "   -p  index of palette to be used\n");
    PRINT(STDOUT, "   -r  PS dots/inch\n");
    PRINT(STDOUT, "   -t  number of threads to use\n");
    PRINT(STDOUT, "   -s  use thread pool strategy\n");
    PRINT(STDOUT, "   -l  landscape mode for PS\n");
    PRINT(STDOUT, "   -A  don't test SCREEN device\n");
    PRINT(STDOUT, "   -B  don't test PS device\n");
    PRINT(STDOUT, "   -C  don't test CGM device\n");
    PRINT(STDOUT, "   -D  don't test JPEG device\n");
    PRINT(STDOUT, "   -E  don't test PNG device\n");
    PRINT(STDOUT, "   -F  don't test IMAGE device\n");
    PRINT(STDOUT, "   -1  don't do test #1 (first line test)\n");
    PRINT(STDOUT, "   -2  don't do test #2 (second line test)\n");
    PRINT(STDOUT, "   -3  don't do test #3 (image test)\n");
    PRINT(STDOUT, "   -4  don't do test #4 (rotated text test)\n");
    PRINT(STDOUT, "   -5  don't do test #5 (contour test)\n");
    PRINT(STDOUT, "   -6  don't do test #6 (vector test)\n");
    PRINT(STDOUT, "   -7  don't do test #7 (fill-poly test)\n");
    PRINT(STDOUT, "   -8  don't do test #8 (surface test)\n");
    PRINT(STDOUT, "   -9  don't do test #9 (mesh plot test)\n");
    PRINT(STDOUT, "   -10 don't do test #10 (line color/width/style)\n");
    PRINT(STDOUT, "   -11 don't do test #11 (2 parameter AC)\n");
    PRINT(STDOUT, "   -12 don't do test #12 (1 parameter AC)\n");
    PRINT(STDOUT, "\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PARALLEL - do all the parallel initializations now */

static int init_parallel(int c, char **v)
   {int i, nth, trm;

/* must find number of threads first */
    nth = 1;
    for (i = 1; i < c; i++)
        {if (strcmp(v[i], "-t") == 0)
	    nth = atoi(v[++i]);

	 else if (strcmp(v[i], "-s") == 0)
	    strategy = 1;};

    if (nth > 1)
       {if (strategy == 1)
	   {PRINT(STDOUT, "\n        Use thread pool with %d threads", nth);
	    SC_init_tpool(nth, NULL);}

        else
	   {PRINT(STDOUT, "\n        Use new threads with %d threads", nth);
	    SC_init_threads(nth, NULL);};};

/* now we can initialize distributed parallel */
    trm = PG_init_parallel(v, NULL);

    return(trm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TIMEOUT_FINPAR - handle timeout for parallel termination */

static void timeout_finpar(int sig)
   {int st;

    if (sig == SIGALRM)
       {PRINT(stdout, "\n        Timed out in parallel termination\n");
	st = 255;}
    else
       st = 1;

    exit(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIN_PARALLEL - do all parallel wrap ups */

static void fin_parallel(int sig)
   {int to;

    to = TIMEOUT_FIN_PARALLEL;

/* set the alarm */
    SC_timeout(to, timeout_finpar, NULL);

    if (strategy == 1)
       SC_fin_tpool();

    PG_fin_parallel(sig);

/* reset the alarm */
    SC_timeout(0, timeout_finpar, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TIMEOUT_GRAPHICS - handle timeout for graphics */

static void timeout_graphics(int sig)
   {int st;

    if (sig == SIGALRM)
       {PRINT(stdout, "\n        Timed out in renderings\n");
	st = 255;}
    else
       st = 1;

    if (strategy == 1)
       SC_fin_tpool();

    exit(st);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIG_HANDLER - handle signals gracefully */

static void sig_handler(int sig)
   {

    SC_setup_sig_handlers(sig_handler, NULL, FALSE);

    SC_retrace_exe(NULL, -1, 60000);

    SC_block_file(stdin);

    exit(sig);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int main(int c, char **v)
   {int i, in, nd, nt, color, to;
    char *tok, *n, tc;
    dtest d;
    gtest tst[N_TEST];
    dtest dev[N_DEV];
    char *name[N_DEV];

    SC_setup_sig_handlers(sig_handler, NULL, TRUE);

/* let's hear it for non-ANSI compilers */
    tst[0]  = test_1;
    tst[1]  = test_2;
    tst[2]  = test_3;
    tst[3]  = test_4;
    tst[4]  = test_5;
    tst[5]  = test_6;
    tst[6]  = test_7;
    tst[7]  = test_8;
    tst[8]  = test_9;
    tst[9]  = test_10;
    tst[10] = test_11;
    tst[11] = test_12;

    dev[0] = scr_test;
    dev[1] = ps_test;
    dev[2] = cgm_test;
    dev[3] = jpeg_test;
    dev[4] = png_test;
    dev[5] = image_test;

    name[0] = "gsscrts";
    name[1] = "gspsts";
    name[2] = "gscgmt";
    name[3] = "gsjpgt";
    name[4] = "gspngt";
    name[5] = "gsimts";

    trm = init_parallel(c, v);

    SC_zero_space_n(0, -2);

    nt = N_TEST;
    nd = N_DEV;

    color        = FALSE;
    mode         = PORTRAIT_MODE;
    ids          = FALSE;
    debug_mode   = FALSE;
    single_frame = FALSE;
    to           = TIMEOUT_GRAPHICS;

    PG_set_attrs_glb(TRUE,
		     "plot-date", FALSE,
		     NULL);

    for (i = 1; i < c; i++)
        if (v[i][0] == '-')
           {tok = v[i] + 1;
	    tc  = tok[0];
	    if (SC_intstrp(tok, 10))
	       {in      = SC_stoi(tok) - 1;
		tst[in] = NULL;}

	    else if (('A' <= tc) && (tc <= 'Z'))
	       {in      = tc - 'A';
		in      = min(in, N_DEV-1);
		dev[in] = NULL;}

	    else
	       {switch (tok[0])
		   {case 'a' :
		         color = TRUE;
			 name[0] = "gsscrta";
			 name[1] = "gspsta";
			 name[2] = "gscgmta";
			 name[3] = "gsjpgta";
			 name[4] = "gspngta";
			 name[5] = "gsimta";
			 break;
		    case 'c' :
		         color = TRUE;
			 break;
		    case 'd' :
		         debug_mode = TRUE;
			 break;
		    case 'f' :
		         single_frame = TRUE;
			 break;
		    case 'i' :
		         ids = TRUE;
			 break;
		    case 'l' :
		         mode = LANDSCAPE_MODE;
			 break;
		    case 'n' :
		         SC_gs.comm_size = SC_stoi(v[++i]);
			 PG_set_attrs_glb(TRUE,
					  "parallel-simulate", TRUE,
					  "parallel-graphics", TRUE,
					  NULL);
			 break;
		    case 'p' :
		         palette = SC_stoi(v[++i]);
			 break;
		    case 'r' :
			 PG_set_attrs_glb(TRUE,
					  "ps-dots-inch", SC_stof(v[++i]),
					  NULL);
			 break;
		    case 's' :
		    case 't' :
			 break;
		    default  :
		    case 'h' :
		         help();
			 fin_parallel(trm);
			 return(1);
			 break;};};};

/* set the alarm */
    SC_timeout(to, timeout_graphics, NULL);

    for (i = 0; i < nd; i++)
        {d = dev[i];
	 n = name[i];
	 if (d != NULL)
	    (*d)(n, color, debug_mode, nt, tst);};

/* reset the alarm */
    SC_timeout(0, timeout_graphics, NULL);

    fin_parallel(trm);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
