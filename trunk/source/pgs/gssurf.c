/*
 * GSSURF.C - surface plotting routines for PGS
 *          - THREADSAFE and THREADED
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

#define LABS(x) abs((int) (x))

typedef struct s_PG_scan_raster PG_scan_raster;
typedef struct s_PG_scan_line_data PG_scan_line_data;

typedef double *(*PFSurfMap)(double *f, double *x, double *y,
			     int mode, void *cnnct,
			     pcons *alist);

typedef void (*PFSurfScan)(PG_device *dev, PG_scan_line_data *par,
			   int *ip, int indx, int sweep);

struct s_PG_scan_raster
   {int aix;
    int aiy;
    int bix;
    int biy;
    int nx;
    int ny;
    unsigned char *bf;};

struct s_PG_scan_line_data
   {int color;
    int mesh;
    int nd;
    int ds;
    int np;
    int *iextr;
    double vmn;
    double **ir;
    double *dextr;
    double **f;
    double *fextr;
    PG_rendering type;
    PM_polygon *py;
    PFSurfScan fnc_scan;
    PG_device *dev;
    void *cnnct;
    pcons *alist;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static INLINE void _PG_set_srf_pixel(PG_device *dev, frame *fr,
				     int *ir, double z,
				     RGB_color_map *clr)
   {int m, nx, ny, np;
    int lr[PG_SPACEDM];

    nx = fr->width;
    ny = fr->height;
    np = nx*ny;

    lr[0] = ir[0];
    lr[1] = ir[1];
    PG_QUAD_FOUR_POINT(dev, lr);

    m = lr[0] + lr[1]*nx;
    if ((0 <= m) && (m < np))
       SET_RASTER_PIXEL(fr, clr, lr[0], lr[1], z);

    return;}

/*--------------------------------------------------------------------------*/

/*                           WORKER LEVEL ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _PG_FILL_SCAN_LINE - fill in the scan line between IXCA and IXCB
 *                    - IXCA <= IXCB is guaranteed
 */

static int _PG_fill_scan_line(PG_device *dev, int sweep, int ena, int enb,
			      long ixca, long ixcb, long idx,
			      long iyca, long iycb, long idy,
			      double zca, double zcb, double zmn, double zmx,
			      double fca, double fcb, double fmn, double fmx,
			      double *zbf, PG_rendering pty, int color, int mesh)
   {int clr, co, in;
    int ir[PG_SPACEDM];
    double dz, zc, df, fc, fs, fa, fb, fd;
    int cmx, backgr, rgb;
    RGB_color_map c;
    PG_palette *pal;
    PG_RAST_device *mdv;
    frame *fr;

    ir[0] = ixca;
    ir[1] = iyca;

    zc = zca;
    fc = fca;

    if ((zca <= -HUGE) || (HUGE <= zca) ||
	(zcb <= -HUGE) || (HUGE <= zcb))
       return(TRUE);

    if (idy == 0L)
       {dz = (zcb - zca)/(ixcb - ixca + SMALL);
        df = (fcb - fca)/(ixcb - ixca + SMALL);
        in = ir[0];}
    else
       {dz = (zcb - zca)/(iycb - iyca + SMALL);
        df = (fcb - fca)/(iycb - iyca + SMALL);
        in = ir[1];};

    pal = dev->current_palette;
    cmx = pal->n_pal_colors + 1;

    mdv = _PG_get_raster(dev, TRUE);
    rgb = mdv->rgb_mode;
    fr  = mdv->raster;

    if (POSTSCRIPT_DEVICE(dev) && !COLOR_POSTSCRIPT_DEVICE(dev))
       {co     = 2;
        backgr = cmx;}
    else
       {co     = (pty == PLOT_SURFACE) << 1;
        backgr = dev->BLACK;};

    if (ena && mesh && (zc >= zbf[in]))
       {zbf[in] = zc;
	if ((fmn <= fc) && (fc <= fmx))
	   {PG_rgb_color(&c, color, rgb, pal);
	    _PG_set_srf_pixel(dev, fr, ir, zc, &c);};
        in++;
        ir[0] += idx;
        ir[1] += idy;
        zc  += dz;
        fc  += df;};

/* fill the z-buffer with the contribution from this line's interior */
    switch (pty)
       {case PLOT_SURFACE :
	     fs = pal->n_pal_colors;
	     fd = fmx - fmn;
             fa = (fd == 0.0) ? 0.0 : fs/fd;
             fb = fmn*fa;
	     for (; (ir[0] < ixcb) && (ir[1] < iycb);
		  ir[0] += idx, ir[1] += idy, zc += dz, fc += df, in++)
	         if (zc >= zbf[in])
		    {zbf[in] = zc;
		     clr = fa*fc - fb + co;
		     PG_rgb_color(&c, clr, rgb, pal);
		     _PG_set_srf_pixel(dev, fr, ir, zc, &c);};
             break;

        case PLOT_WIRE_MESH : 
	     for (; (ir[0] < ixcb) && (ir[1] < iycb);
		  ir[0] += idx, ir[1] += idy, zc += dz, in++)
	         if (zc >= zbf[in])
		    {zbf[in] = zc;
		     PG_rgb_color(&c, backgr, rgb, pal);
		     _PG_set_srf_pixel(dev, fr, ir, zc, &c);};
	     break;

	default :
	     break;};

    if (enb && mesh && (zc >= zbf[in]))
       {zbf[in] = zc;
	if ((fmn <= fc) && (fc <= fmx))
	   {PG_rgb_color(&c, color, rgb, pal);
	    _PG_set_srf_pixel(dev, fr, ir, zc, &c);};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SCAN_TRIANGLE - scan convert a line crossing the triangle 1-2-3
 *                   - into the z-buffer
 *                   - the line connecting 1 and 2 is the only one visible
 *                   - this is intended to be the boundary of the
 *                   - partitioned polygon
 *                   - point 1 is below point 2 with respect to y
 *                   - sweep 0 is vertical sweep of horizontal lines
 *                   - sweep 1 is horizontal sweep of vertical lines
 */

static int _PG_scan_triangle(PG_device *dev, double *zbf,
			     int iyc, int ndr, int sweep, int *ip,
			     int ix1, int iy1, int ix2, int iy2,
			     int ix3, int iy3,
			     double z1, double z2, double z3,
			     double zmn, double zmx,
			     double *f1, double *f2, double *f3,
			     double *fextr,
			     int color, int mesh, PG_rendering pty)
   {int ena, enb;
    int crosses_12, crosses_23, crosses_31;
    int intersects_1, intersects_2, intersects_3;
    long dy1c, dy2c, dy3c;
    long ixca, ixcb, it;
    double dy12, dy23, dy31, fmn, fmx;
    double zca, zcb, zt, fca, fcb;

/* NOTE: short term hack which throws away the generality that has
 * been passed down to this level
 */
    f1 += (ndr - 1);
    f2 += (ndr - 1);
    f3 += (ndr - 1);
    fmn = fextr[2*(ndr - 1)];
    fmx = fextr[2*(ndr - 1) + 1];

    dy1c = iy1 - iyc;
    dy2c = iy2 - iyc;
    dy3c = iy3 - iyc;

    intersects_1 = (iy1 == iyc);
    intersects_2 = (iy2 == iyc);
    intersects_3 = (iy3 == iyc);

    crosses_12 = ((iy1 < iyc) && (iyc < iy2));
    crosses_31 = (((iy1 < iyc) && (iyc < iy3)) || ((iy3 < iyc) && (iyc < iy1)));
    crosses_23 = (((iy2 < iyc) && (iyc < iy3)) || ((iy3 < iyc) && (iyc < iy2)));

    ixca = 0;
    ixcb = 0;
    zcb  = 0;
    zca  = 0;
    fcb  = 0;
    fca  = 0;
    enb  = 0;
    ena  = 0;

/* check all cases in which the scan line crosses vertex #1 */
    if (intersects_1)
       {if (intersects_2)
           {if (ix1 <= ix2)
	       {ixca = ix1;
                zca  = z1;
                fca  = *f1;
		ena  = TRUE;
                ixcb = ix2;
                zcb  = z2;
                fcb  = *f2;
		enb  = TRUE;}
	    else
	       {ixca = ix2;
                zca  = z2;
                fca  = *f2;
		ena  = TRUE;
                ixcb = ix1;
                zcb  = z1;
                fcb  = *f1;
		enb  = TRUE;};}

        else if ((intersects_3) || (crosses_23))
	   {dy23 = 1.0/((double) (iy2 - iy3) + SMALL);
	    ixcb = (ix3*dy2c - ix2*dy3c)*dy23;
	    zcb  = (z3*dy2c - z2*dy3c)*dy23;
	    fcb  = (*f3*dy2c - *f2*dy3c)*dy23;
	    enb  = FALSE;

	    if (ix1 <= ixcb)
	       {ixca = ix1;
                zca  = z1;
                fca  = *f1;
		ena  = TRUE;}
	    else
	       {ixca = ixcb;
                zca  = zcb;
                fca  = fcb;
		ena  = enb;
                ixcb = ix1;
                zcb  = z1;
                fcb  = *f1;
		enb  = TRUE;};}

        else
	   {ixca = ix1;
	    zca  = z1;
	    fca  = *f1;
	    ena  = TRUE;
	    ixcb = ix1;
	    zcb  = z1;
	    fcb  = *f1;
	    enb  = FALSE;};}

/* check all cases in which the scan line crosses vertex #2 excluding above */
    else if (intersects_2)
       {if ((intersects_3) || (crosses_31))
	   {dy31 = 1.0/((double) (iy1 - iy3) + SMALL);
	    ixcb = (ix3*dy1c - ix1*dy3c)*dy31;
	    zcb  = (z3*dy1c - z1*dy3c)*dy31;
	    fcb  = (*f3*dy1c - *f1*dy3c)*dy31;
	    enb  = FALSE;

	    if (ix2 <= ixcb)
	       {ixca = ix2;
                zca  = z2;
                fca  = *f2;
		ena  = TRUE;}
	    else
	       {ixca = ixcb;
                zca  = zcb;
                fca  = fcb;
		ena  = enb;
                ixcb = ix2;
                zcb  = z2;
                fcb  = *f2;
		enb  = TRUE;};}
        else
	   {ixca = ix2;
	    zca  = z2;
	    fca  = *f2;
	    ena  = TRUE;
	    ixcb = ix2;
	    zcb  = z2;
	    fcb  = *f2;
	    enb  = FALSE;};}

/* check all cases in which the scan line crosses vertex #3 excluding above */
    else if (intersects_3)
       {if (crosses_12)
	   {dy12 = 1.0/((double) (iy1 - iy2) + SMALL);
	    ixcb = (ix2*dy1c - ix1*dy2c)*dy12;
	    zcb  = (z2*dy1c - z1*dy2c)*dy12;
	    fcb  = (*f2*dy1c - *f1*dy2c)*dy12;
	    enb  = TRUE;

	    if (ix3 <= ixcb)
	       {ixca = ix3;
                zca  = z3;
                fca  = *f3;
		ena  = FALSE;}
	    else
	       {ixca = ixcb;
                zca  = zcb;
                fca  = fcb;
		ena  = enb;
                ixcb = ix3;
                zcb  = z3;
                fcb  = *f3;
		enb  = FALSE;};}
        else
	   {ixca = ix3;
	    zca  = z3;
	    fca  = *f3;
	    ena  = FALSE;
	    ixcb = ix3;
	    zcb  = z3;
	    fcb  = *f3;
	    enb  = FALSE;};}

/* check all cases in which the scan line crosses line 1-2 excluding above */
    else if (crosses_12)
       {dy12 = 1.0/((double) (iy1 - iy2) + SMALL);
	ixcb = (ix2*dy1c - ix1*dy2c)*dy12;
	zcb  = (z2*dy1c - z1*dy2c)*dy12;
	fcb  = (*f2*dy1c - *f1*dy2c)*dy12;
	enb  = TRUE;

        if (crosses_23)
	   {dy23 = 1.0/((double) (iy2 - iy3) + SMALL);
	    ixca = (ix3*dy2c - ix2*dy3c)*dy23;
	    zca  = (z3*dy2c - z2*dy3c)*dy23;
	    fca  = (*f3*dy2c - *f2*dy3c)*dy23;
	    ena  = FALSE;
	    if (ixca > ixcb)
	       {it   = ixca;
		ixca = ixcb;
		ixcb = it;
	        zt   = zca;
		zca  = zcb;
		zcb  = zt;
	        zt   = fca;
		fca  = fcb;
		fcb  = zt;
		ena  = TRUE;
		enb  = FALSE;};}

        else if (crosses_31)
	   {dy31 = 1.0/((double) (iy1 - iy3) + SMALL);
	    ixca = (ix3*dy1c - ix1*dy3c)*dy31;
	    zca  = (z3*dy1c - z1*dy3c)*dy31;
	    fca  = (*f3*dy1c - *f1*dy3c)*dy31;
	    ena  = FALSE;
	    if (ixca > ixcb)
	       {it   = ixca;
		ixca = ixcb;
		ixcb = it;
	        zt   = zca;
		zca  = zcb;
		zcb  = zt;
	        zt   = fca;
		fca  = fcb;
		fcb  = zt;
		ena  = TRUE;
		enb  = FALSE;};}

        else
           return(FALSE);}

/* check all cases in which the scan line crosses line 2-3 excluding above */
    else if (crosses_23)
       {dy23 = 1.0/((double) (iy2 - iy3) + SMALL);
	ixcb = (ix3*dy2c - ix2*dy3c)*dy23;
	zcb  = (z3*dy2c - z2*dy3c)*dy23;
	fcb  = (*f3*dy2c - *f2*dy3c)*dy23;
	enb  = FALSE;
        if (crosses_31)
	   {dy31 = 1.0/((double) (iy1 - iy3) + SMALL);
	    ixca = (ix3*dy1c - ix1*dy3c)*dy31;
	    zca  = (z3*dy1c - z1*dy3c)*dy31;
	    fca  = (*f3*dy1c - *f1*dy3c)*dy31;
	    ena  = FALSE;
	    if (ixca > ixcb)
	       {it   = ixca;
		ixca = ixcb;
		ixcb = it;
	        zt   = zca;
		zca  = zcb;
		zcb  = zt;
	        zt   = fca;
		fca  = fcb;
		fcb  = zt;};}
        else
           return(FALSE);}

    else if (crosses_31)
       return(FALSE);

/* clip to the viewport limits */
    if (((ixca < ip[0]) && (ixcb < ip[0])) ||
	((ixca > ip[1]) && (ixcb > ip[1])))
       return(FALSE);

    if (ixca < ip[0])
       {zca += (zcb - zca)*((double) (ip[0] - ixca))/
	                   ((double) (ixcb - ixca));
        ixca = ip[0];};

    if (ip[1] < ixcb)
       {zcb += (zca - zcb)*((double) (ixcb - ip[1]))/
	                   ((double) (ixcb - ixca));
        ixcb = ip[1];};

/* printf("%d\t%d\t%d\t%d\t%d\n", ena, enb, iyc, ixca, ixcb); */
    color = _PG_gattrs.ref_mesh_color;
    if (sweep == 0)
       _PG_fill_scan_line(dev, sweep, ena, enb,
			  ixca, ixcb, 1L,
			  (long) iyc, iyc+1L, 0L,
			  zca, zcb, zmn, zmx,
			  fca, fcb, fmn, fmx,
			  zbf, pty, color, mesh);
    else
       _PG_fill_scan_line(dev, sweep, ena, enb,
			  (long) iyc, iyc+1L, 0L,
			  ixca, ixcb, 1L,
			  zca, zcb, zmn, zmx,
			  fca, fcb, fmn, fmx,
			  zbf, pty, color, mesh);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SCAN_TRIANGLE - scan convert a line crossing the triangle 1-c-2
 *                  - into the z-buffer ZBF
 *                  - the lines 1-c and c-2 are invisible
 *                  - this is intended to be in the interior of the
 *                  - partitioned polygon
 */

static void PG_scan_triangle(PG_device *dev, double *zbf,
			     int indx, int nd, int sweep, int *ip,
			     int ix1, int iy1, double a1, double *f1,
			     int ixc, int iyc, double ac, double *fc,
			     int ix2, int iy2, double a2, double *f2,
			     double amn, double amx, double *extr,
			     int color, int mesh, PG_rendering pty)
   {int iya, iyb;

    if (((ix1 < ip[0]) && (ixc < ip[0]) && (ix2 < ip[0])) ||
        ((ix1 > ip[1]) && (ixc > ip[1]) && (ix2 > ip[1])))
       return;

    if (iy1 <= iy2)
       {iya = max(iy2, iyc);
	iyb = min(iy1, iyc);
	if ((iyb <= indx) && (indx <= iya))
	   _PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
			     ix1, iy1, ix2, iy2,
			     ixc, iyc, a1, a2, ac, amn, amx,
			     f1, f2, fc, extr, color, mesh, pty);}
    else
       {iya = max(iy1, iyc);
	iyb = min(iy2, iyc);
	if ((iyb <= indx) && (indx <= iya))
	   _PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
			     ix2, iy2, ix1, iy1,
			     ixc, iyc, a2, a1, ac, amn, amx,
			     f2, f1, fc, extr, color, mesh, pty);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DO_SCAN_LINE - do a chunk of scan lines for the picture */

static void *_PG_do_scan_line(void *arg)
   {int i, nic, ok, p1, p2;
    int ds, iymn, iymx;
    int a[4];
    int *iextr;
    double u[4];
    double **xi;
    PM_polygon *py, *pl;
    PG_scan_line_data *par;
    PG_device *dev;
    PFSurfScan fnc_scan;
    void *rv;

#if 0
    int id;

    id = SC_current_thread();

/* GOTCHA: cause crash to test atdbg thread handling */
    if (id == 3)
       printf("-> %s\n", *((char **) -1));
#endif

    par = (PG_scan_line_data *) arg;

    ds       = par->ds;
    iextr    = par->iextr;
    py       = par->py;
    fnc_scan = par->fnc_scan;
    dev      = par->dev;

    SC_chunk_split(&iymn, &iymx, &rv);

    pl = PM_copy_polygon(py);
    PG_trans_points(dev, pl->nn, pl->nd, WORLDC, pl->x, PIXELC, pl->x);

/* do a vertical sweep of horizontal scan lines */
    for (i = iymn; i <= iymx; i++)

/* compute the imposed domain limits on the scan line */
	{u[0] = iextr[0]*ds;
	 u[1] = i*ds;
	 u[2] = iextr[1]*ds;
	 u[3] = u[1];

         ok = PM_intersect_line_polygon(&nic, &xi, NULL, u, u+2, pl, 3);
         if (ok == TRUE)
	    {if (nic < 2)
	        {p1 = PM_contains_nd(u, pl);
		 p2 = PM_contains_nd(u+2, pl);
		 if (nic == 1)
		    {a[0] = xi[0][0]/ds;
		     a[2] = xi[1][0]/ds;
		     if (p1 == 1)
		        {a[1] = u[0]/ds;
			 a[3] = u[1]/ds;}
		     else
		        {a[1] = u[2]/ds;
			 a[3] = u[3]/ds;};}

		 else if ((p1 == 1) && (p2 == 1))
		    {a[0] = u[0]/ds;
		     a[1] = u[2]/ds;
		     a[2] = u[1]/ds;
		     a[3] = u[3]/ds;};}

	     else
	        {a[0] = xi[0][0]/ds;
		 a[1] = xi[0][1]/ds;
		 a[2] = xi[1][0]/ds;
		 a[3] = xi[1][1]/ds;};

	     PM_free_vectors(2, xi);

/* ensure the points are left to right */
	     if (a[0] > a[1])
	        {SC_SWAP_VALUE(double, a[0], a[1]);
		 SC_SWAP_VALUE(double, a[2], a[3]);};

	     (*fnc_scan)(dev, par, a, i, 0);};};

    PM_free_polygon(pl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_SURFACE - draw a surface plot given a known mesh type, a
 *                  - suitable scanning function, and a scalar array
 *                  - rotate to view angle, plot with hidden line
 *                  - algorithm, and shade if requested
 */

static void _PG_draw_surface(PG_device *dev, int nd,
			     double **f, double *fextr,
			     double *x, double *y, int nn, double *dextr,
			     double width, int color, int mesh, int style,
			     PG_rendering pty, char *name, void *cnnct,
			     pcons *alist, PFSurfScan fnc_scan)
   {int i, np, mpts, ds;
    int jx, jy, lx, ly, nx, ny;
    int iext[4];
    double vext[PG_SPACEDM];
    double dext[PG_BOXSZ], sf[PG_SPACEDM], tr[PG_SPACEDM];
    double **ir, **r, **p, *t[PG_SPACEDM];
    PM_polygon *py;
    PG_scan_line_data par;

    if (dev == NULL)
       return;

    ds = dev->resolution_scale_factor;

    t[0] = x;
    t[1] = y;
    t[2] = f[0];

    mpts = max(nn, 12);

/* save and zero out the window offsets */
    for (i = 0; i < 3; i++)
        {tr[i] = dev->window_x[2*i];
	 dev->window_x[2*i] = 0;};

/* find the polygon which is the projection of the view space limits */
    p  = PG_get_space_box(dev, dextr, FALSE);
    py = PM_convex_hull(p[0], p[1], 8);
    PM_free_vectors(3, p);

    r = PM_copy_vectors(3, mpts, t);
    PG_rotate_vectors(dev, 3, nn, r);

/* transform to PC for efficiency */
    ir = PM_make_vectors(3, nn);
    PG_trans_points(dev, nn, 3, WORLDC, r, PIXELC, ir);

/* scale vectors to desired resolution */
    for (i = 0; i < 3; i++)
        sf[i] = 1.0/ds;

    PM_scale_vectors(3, nn, ir, sf);

/* find the final PC limits */
    PM_vector_extrema(3, nn, ir, dext);

    for (i = 0; i < 4; i++)
        iext[i] = dext[i];

    PG_set_clipping(dev, FALSE);

/* find mins and maxes for the integer mesh */
    PM_vector_extrema(1, nn, &r[2], vext);

    vext[0] -= 1.0e-3*(vext[1] - vext[0]);

    PG_get_viewport_size(dev, &jx, &jy, &nx, &ny);

    ny += 2;

/* adjust resolution downward if the image would be too big to fit in memory */
    if (!_PG_allocate_image_buffer(dev, NULL, &nx, &ny))
       return;

/* these values will be used to scale the scan lines */
    lx = iext[1] - iext[0] + 1;
    ly = iext[3] - iext[2] + 1;

/* NOTE: devices such as CGM insist on even values for image dimensions */
    nx = max(nx, lx);
    ny = max(ny, ly);
    if (nx & 1)
       {nx++;
        lx++;
        iext[1]++;};

    if (ny & 1)
       {ny++;
        ly++;
        iext[3]++;};

    if (POSTSCRIPT_DEVICE(dev) && !COLOR_POSTSCRIPT_DEVICE(dev))
       i = dev->current_palette->n_pal_colors + 1;
    else if (CGM_DEVICE(dev) && (dev->current_palette != dev->palettes))
       i = 1;
    else
       i = dev->BLACK;

    np = (max(PG_window_width(dev), PG_window_height(dev)))/ds;

    par.dev      = dev;
    par.fnc_scan = fnc_scan;
    par.f        = f;
    par.fextr    = fextr;
    par.dextr    = dextr;
    par.nd       = nd;
    par.iextr    = iext;
    par.ds       = ds;
    par.np       = np;
    par.ir       = ir;
    par.vmn      = vext[0];
    par.cnnct    = cnnct;
    par.alist    = alist;
    par.color    = color;
    par.mesh     = mesh;
    par.type     = pty;
    par.py       = py;

    SC_chunk_loop(_PG_do_scan_line, iext[2], iext[3], FALSE, &par);

/* restore the window offsets */
    for (i = 0; i < 3; i++)
        dev->window_x[2*i] = tr[i];

#if 0
    switch (_PG_gattrs.hl_clear_mode)
       {case CLEAR_SCREEN :
             PG_clear_window(dev);
             break;
        case CLEAR_VIEWPORT :
             PG_clear_viewport(dev);
             break;
        case CLEAR_FRAME :
             PG_clear_frame(dev);
             break;};
#endif

    PG_draw_box_n(dev, 3, WORLDC, dextr);

/* release the temporaries */
    PM_free_vectors(3, ir);
    PM_free_vectors(3, r);
    PM_free_polygon(py);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SURFACE_HAND - draw a surface plot */

static void PG_surface_hand(PG_device *dev, PG_graph *g, PG_rendering pty,
                            void *cnnct,
                            PFSurfMap fnc_map, PFSurfScan fnc_scan)
   {int i, n_nodes, npts, nd, same, sid;
    int color, mesh, style, rexfl;
    char bf[MAXLINE], *mtype, *s;
    double width;
    double va[PG_NANGLES], box[PG_BOXSZ], wc[PG_BOXSZ];
    double **r, **d, *dextr, *rextr, **afd;
    void **afs;
    PM_centering centering;
    PM_mapping *h;
    PM_set *domain, *range;
    pcons *alst;

    if ((dev == NULL) || (g == NULL))
       return;

    PG_get_fill_bound(dev, mesh);

    PG_get_attrs_graph(g, FALSE,
		       "THETA",      SC_DOUBLE_I,  &va[0], 0.0,
		       "PHI",        SC_DOUBLE_I,  &va[1], 0.0,
		       "CHI",        SC_DOUBLE_I,  &va[2], 0.0,
		       "DRAW-MESH",  SC_INT_I, &mesh,  _PG_gattrs.ref_mesh,
		       "LINE-COLOR", SC_INT_I, &color, dev->BLUE,
		       "LINE-STYLE", SC_INT_I, &style, LINE_SOLID,
		       "LINE-WIDTH", SC_DOUBLE_I,  &width, 0.0,
		       NULL);

    for (i = 0; i < PG_NANGLES; i++)
        va[i] *= DEG_RAD;

    mesh   = (pty == PLOT_WIRE_MESH) ? TRUE : mesh;
    h      = g->f;
    domain = h->domain;

    strcpy(bf, domain->element_type);
    mtype   = SC_strtok(bf, " *", s);
    npts    = domain->n_elements;
    r       = (double **) domain->elements;
    d       = PM_convert_vectors(2, npts, r, domain->element_type);

    n_nodes = npts;

    dextr = PM_array_real(domain->element_type, domain->extrema, 4, NULL);

    PG_get_viewspace(dev, WORLDC, wc);

    range = h->range;
    strcpy(bf, range->element_type);
    npts  = range->n_elements;
    nd    = range->dimension_elem;
    mtype = SC_strtok(bf, " *", s);
    sid   = SC_type_id(mtype, FALSE);
    same  = (sid == SC_DOUBLE_I);

    afd   = FMAKE_N(double *, nd, "PG_SURFACE_HAND:afd");
    afs   = (void **) range->elements;

/* find the range limits if any */
    rextr = PM_get_limits(range);

/* setup the range limits */
    rexfl = (rextr == NULL);
    if (same == TRUE)
       {if (rexfl)
           rextr = (double *) range->extrema;
        for (i = 0; i < nd; i++)
            afd[i] = (double *) afs[i];}
    else
       {for (i = 0; i < nd; i++)
            afd[i] = SC_convert_id(SC_DOUBLE_I, NULL, 0, 1,
				   sid, afs[i], 0, 1, npts, FALSE);

        if (rexfl)
           rextr = SC_convert_id(SC_DOUBLE_I, NULL, 0, 1,
				 sid, range->extrema, 0, 1, 2*nd, FALSE);};

    PG_register_range_extrema(dev, nd, rextr);

/* find the additional mapping information */
    centering = N_CENT;
    alst = PM_mapping_info(h,
			   "CENTERING", &centering,
			   NULL);

    PG_box_copy(2, box, wc);
    box[4] = rextr[0];
    box[5] = rextr[1];

/* this is done consistently with PG_draw_vector */
    switch (centering)
       {case Z_CENT :
             {double **afdp;

	      afdp = FMAKE_N(double *, nd, "PG_SURFACE_HAND:afdp");
	      for (i = 0; i < nd; i++)
                  afdp[i] = (*fnc_map)(afd[i], d[0], d[1],
				       _PG_gattrs.interp_flag,
				       cnnct, alst);

	      _PG_draw_surface(dev, nd, afdp, rextr, d[0], d[1], n_nodes,
			       box, width, color, mesh, style, pty,
			       h->name, cnnct, alst, fnc_scan);

	      for (i = 0; i < nd; i++)
		  SFREE(afdp[i]);
	      SFREE(afdp);};

	     break;

        case N_CENT :
             _PG_draw_surface(dev, nd, afd, rextr, d[0], d[1], n_nodes,
			      box, width, color, mesh, style, pty,
			      h->name, cnnct, alst, fnc_scan);
	     break;

        case F_CENT :
        case U_CENT :
        default     :
	     break;};

    PM_free_vectors(2, d);

    if (same == FALSE)
       {for (i = 0; i < nd; i++)
            SFREE(afd[i]);

        if (rexfl)
           SFREE(rextr);};

    SFREE(afd);
    SFREE(dextr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SETUP_PICTURE_SURFACE - setup a window for a surface rendering
 *                          - NOTE: no drawing of any kind is to be done here
 */

PG_picture_desc *PG_setup_picture_surface(PG_device *dev, PG_graph *data,
					  int save, int clear)
   {int nde, nre, change;
    double ndc[PG_BOXSZ];
    double *dpex, *ddex, *pdx, *rpex, *rdex, *prx;
    PG_rendering pty;
    PG_picture_desc *pd;
    PG_par_rend_info *pri;
    PG_device *dd;
    pcons *alst;

    if ((dev == NULL) || (data == NULL))
       return(NULL);

    change = !dev->supress_setup;

    pd = PG_get_rendering_properties(dev, data);

    PG_set_view_angle(dev, pd->va[0], pd->va[1], pd->va[2]);

    pd->legend_contour_fl = FALSE;
    pd->mesh_fl           = FALSE;

    pty  = pd->rendering;
    alst = pd->alist;
    pri  = dev->pri;
    if (pri != NULL)
       {dd = pri->dd;
	if (dd != NULL)
	   {dd->pri->alist  = alst;
	    dd->pri->render = pty;};};

/* setup the viewport */
    if (pty == PLOT_SURFACE)
       {if (change)
	   {ndc[0] = 0.175;
	    ndc[1] = 0.725 - _PG_gattrs.palette_width;
	    ndc[2] = 0.175;
	    ndc[3] = 0.825;

	    PG_set_viewspace(dev, 2, NORMC, ndc);};}
    else
	pd->legend_palette_fl = FALSE;

    if (change)

/* find the extrema for this frame */
       {PG_find_extrema(data, 0.0, &dpex, &rpex, &nde, &ddex, &nre, &rdex);

/* setup the range limits */
	prx = ((dev->autorange == TRUE) || (rpex == NULL)) ? rdex : rpex;
	PG_register_range_extrema(dev, nre, prx);

/* setup the domain limits */
	pdx = ((dev->autodomain == TRUE) || (dpex == NULL)) ? ddex : dpex;
	PG_set_viewspace(dev, 2, WORLDC, pdx);

	SFREE(ddex);
	SFREE(rdex);

/* GOTCHA: this isn't the only time we want 3d axes */
	if ((pd->va[0] != HUGE) || (nde == 3))
	   {pd->ax_type = CARTESIAN_3D;
	    PG_set_view_angle(dev, pd->va[0], pd->va[1], pd->va[2]);};

/* set surface plot attribute values */
	PG_set_clipping(dev, FALSE);};

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PG_SURFACE_CORE - surface plot skeleton routine */

static void _PG_surface_core(PG_device *dev, PG_graph *data, void *cnnct,
			     PFSurfMap fnc_map, PFSurfScan fnc_scan)
   {PG_graph *g;
    PG_picture_desc *pd;
	
    pd = PG_setup_picture(dev, data, TRUE, FALSE, TRUE);

    PG_adorn_before(dev, pd, data);

/* plot all of the current functions */
    for (g = data; g != NULL; g = g->next)
        PG_surface_hand(dev, g, g->rendering, cnnct, fnc_map, fnc_scan);

    PG_finish_picture(dev, data, pd);

    return;}

/*--------------------------------------------------------------------------*/

/*                       SCAN CONVERSION ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* PG_SCAN_CONVERT_LR - using a one line z-buffer
 *                    - scan convert each polygon intersecting the line
 *                    - flag is one of PLOT_WIRE_MESH or PLOT_SURFACE
 *                    - sweep 0 is vertical sweep of horizontal lines
 *                    - sweep 1 is horizontal sweep of vertical lines
 */

static void PG_scan_convert_lr(PG_device *dev, PG_scan_line_data *par,
			       int *ip, int indx, int sweep)
   {int i, j, l, l0, ln, lc, ic, nmap, im, jm, eflag;
    int nd, color, mesh, iline;
    int ix[5], iy[5];
    int *maxes, imax, jmax;
    double amn, amx;
    double u[5];
    double *rx[5], *ry[5], *ru[5], *ft;
    double **v, **ir, **f, *fextr;
    double *zbf;
    char *emap;
    void *cnnct;
    pcons *alist;
    frame *fr;
    PG_rendering pty;
    PG_RAST_device *mdv;

    mdv = _PG_get_raster(dev, TRUE);
    fr  = mdv->raster;

    if (dev->quadrant == QUAD_FOUR)
       iline = dev->g.pc[3] - indx;
    else
       iline = indx;

    zbf = fr->zb + iline*fr->width;

    color = par->color;
    mesh  = par->mesh;
    pty   = par->type;
    cnnct = par->cnnct;
    alist = par->alist;
    nd    = par->nd;
    ir    = par->ir;
    f     = par->f;
    fextr = par->fextr;

    maxes = (int *) cnnct;
    imax  = maxes[0];
    jmax  = maxes[1];
    nmap  = (imax - 1)*(jmax - 1);

    emap = PM_check_emap(&eflag, alist, nmap);

    amn = fextr[0];
    amx = fextr[1];

    PM_LOGICAL_ZONE(ir[0], rx[1], rx[2], rx[3], rx[4], imax);
    PM_LOGICAL_ZONE(ir[1], ry[1], ry[2], ry[3], ry[4], imax);
    PM_LOGICAL_ZONE(ir[2], ru[1], ru[2], ru[3], ru[4], imax);

    v = PM_make_vectors(5, nd);

    l0 = (nd < 2) ? 0 : 1;

    im = imax - 1;
    jm = jmax - 1;
    for (j = 0; j < jm; j++)
        for (i = 0; i < im; i++)
            {ln = j*imax + i;
             lc = j*im + i;
	     if (emap[lc] == 0)
	        continue;

             ix[0] = 0.0;
             iy[0] = 0.0;
             u[0]  = 0.0;

	     for (ic = 1; ic < 5; ic++)
	         {ix[ic] = rx[ic][ln];
		  iy[ic] = ry[ic][ln];
		  u[ic]  = ru[ic][ln];

		  ix[0] += ix[ic];
		  iy[0] += iy[ic];
		  u[0]  += u[ic];};

             ix[0] *= 0.25;
             iy[0] *= 0.25;
             u[0]  *= 0.25;

             for (l = l0; l < nd; l++)
                 {ft      = f[l];
                  v[1][l] = ft[ln+1];
                  v[2][l] = ft[ln+imax+1];
                  v[3][l] = ft[ln+imax];
                  v[4][l] = ft[ln];
                  v[0][l] = 0.25*(v[1][l] + v[2][l] + v[3][l] + v[4][l]);};

             if (sweep == 0)
	        {PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
				  ix[1], iy[1], u[1], v[1],
				  ix[0], iy[0], u[0], v[0],
				  ix[2], iy[2], u[2], v[2],
				  amn, amx, fextr,
				  color, mesh, pty);

		 PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
				  ix[2], iy[2], u[2], v[2],
				  ix[0], iy[0], u[0], v[0],
				  ix[3], iy[3], u[3], v[3],
				  amn, amx, fextr,
				  color, mesh, pty);

		 PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
				  ix[3], iy[3], u[3], v[3],
				  ix[0], iy[0], u[0], v[0],
				  ix[4], iy[4], u[4], v[4],
				  amn, amx, fextr,
				  color, mesh, pty);

		 PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
				  ix[4], iy[4], u[4], v[4],
				  ix[0], iy[0], u[0], v[0],
				  ix[1], iy[1], u[1], v[1],
				  amn, amx, fextr,
				  color, mesh, pty);}
	     else
	        {PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
				  iy[1], ix[1], u[1], v[1],
				  iy[0], ix[0], u[0], v[0],
				  iy[2], ix[2], u[2], v[2],
				  amn, amx, fextr,
				  color, mesh, pty);

		 PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
				  iy[2], ix[2], u[2], v[2],
				  iy[0], ix[0], u[0], v[0],
				  iy[3], ix[3], u[3], v[3],
				  amn, amx, fextr,
				  color, mesh, pty);

		 PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
				  iy[3], ix[3], u[3], v[3],
				  iy[0], ix[0], u[0], v[0],
				  iy[4], ix[4], u[4], v[4],
				  amn, amx, fextr,
				  color, mesh, pty);

		 PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
				  iy[4], ix[4], u[4], v[4],
				  iy[0], ix[0], u[0], v[0],
				  iy[1], ix[1], u[1], v[1],
				  amn, amx, fextr,
				  color, mesh, pty);};};

    PM_free_vectors(5, v);

    if (eflag)
        SFREE(emap);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SCAN_CONVERT_AC - using a one line z-buffer
 *                    - scan convert each polygon intersecting the line
 *                    - flag is one of PLOT_WIRE_MESH or PLOT_SURFACE
 *                    - sweep 0 is vertical sweep of horizontal lines
 *                    - sweep 1 is horizontal sweep of vertical lines
 */

static void PG_scan_convert_ac(PG_device *dev, PG_scan_line_data *par,
			       int *ip, int indx, int sweep)
   {int j, j0, is, os, iz, oz, is1, is2, in1, in2;
    int nd, color, mesh, iline;
    int ix[PG_SPACEDM], iy[PG_SPACEDM], ofs, ofz;
    int *nc, nz, *np, nzp, nsp;
    long **cells, *zones, *sides;
    double amn, amx, norm;
    double u[PG_SPACEDM];
    double **v, *ft, **f, **ir, *fextr;
    double *zbf;
    void *cnnct;
    frame *fr;
    PG_rendering pty;
    PG_RAST_device *mdv;
    PM_mesh_topology *mt;

    mdv = _PG_get_raster(dev, TRUE);
    fr  = mdv->raster;

    if (dev->quadrant == QUAD_FOUR)
       iline = dev->g.pc[3] - indx;
    else
       iline = indx;

    zbf = fr->zb + iline*fr->width;

    color = par->color;
    mesh  = par->mesh;
    pty  = par->type;
    cnnct = par->cnnct;
    nd    = par->nd;
    ir    = par->ir;
    f     = par->f;
    fextr = par->fextr;

    amn = fextr[0];
    amx = fextr[1];

    mt = (PM_mesh_topology *) cnnct;

    cells = mt->boundaries;
    zones = cells[2];
    sides = cells[1];

    nc = mt->n_cells;
    nz = nc[2] - 1;

    np  = mt->n_bound_params;
    nzp = np[2];
    nsp = np[1];

    v = PM_make_vectors(3, nd);

    j0 = (nd < 2) ? 0 : 1;

    ofz = (nzp < 2);
    ofs = (nsp < 2);
    for (iz = 0; iz <= nz; iz++)
        {oz  = iz*nzp;
	 is1 = zones[oz];
	 is2 = zones[oz+1] - ofz;

         if (nzp >= CENTER_CELL)
            {in1  = zones[oz + CENTER_CELL];
             u[0] = ir[2][in1];
	     for (j = j0; j < nd; j++)
	         {ft    = f[j];
		  v[0][j] = ft[in1];};
	     ix[0] = ir[0][in1];
	     iy[0] = ir[1][in1];}
         else
  	    {u[0] = 0.0;
	     for (j = j0; j < nd; j++)
  	         v[0][j] = 0.0;

	     ix[0] = 0;
  	     iy[0] = 0;
             for (is = is1; is <= is2; is++)
	         {os  = is*nsp;
		  in1 = sides[os];
                  ix[0] += ir[0][in1];
                  iy[0] += ir[1][in1];
		  u[0]  += ir[2][in1];
		  for (j = j0; j < nd; j++)
		      {ft     = f[j];
		       v[0][j] += ft[in1];};};

             norm  = 1.0/(is2 - is1 + 1);
	     u[0] *= norm;
	     for (j = j0; j < nd; j++)
	         {ft     = f[j];
		  v[0][j] *= norm;};
	     ix[0] *= norm;
	     iy[0] *= norm;};

	 for (is = is1; is <= is2; is++)
	     {os  = is*nsp;
	      in1 = sides[os];
	      in2 = (ofs && (is == is2)) ? sides[is1] : sides[os+1];

	      u[1] = ir[2][in1];
	      u[2] = ir[2][in2];
	      for (j = j0; j < nd; j++)
		  {ft    = f[j];
		   v[1][j] = ft[in1];
		   v[2][j] = ft[in2];};

	      ix[1] = ir[0][in1];
	      iy[1] = ir[1][in1];
	      ix[2] = ir[0][in2];
	      iy[2] = ir[1][in2];

              if (sweep == 0)
		 PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
				  ix[1], iy[1], u[1], v[1],
				  ix[0], iy[0], u[0], v[0],
				  ix[2], iy[2], u[2], v[2],
				  amn, amx, fextr,
				  color, mesh, pty);
	      else
		 PG_scan_triangle(dev, zbf, indx, nd, sweep, ip,
				  iy[1], ix[1], u[1], v[1],
				  iy[0], ix[0], u[0], v[0],
				  iy[2], ix[2], u[2], v[2],
				  amn, amx, fextr,
				  color, mesh, pty);};};

    PM_free_vectors(3, v);

    return;}

/*--------------------------------------------------------------------------*/

/*                             API LEVEL ROUTINES                           */

/*--------------------------------------------------------------------------*/
 
/* PG_SURFACE_PLOT - main surface plot control routine */

void PG_surface_plot(PG_device *dev, PG_graph *data, ...)
   {

    SC_VA_START(data);

    if (strcmp(data->f->category, PM_LR_S) == 0)
       _PG_surface_core(dev, data, (void *)data->f->domain->max_index,
			PM_z_n_lr_2d, PG_scan_convert_lr);

    else if (strcmp(data->f->category, PM_AC_S) == 0)
       _PG_surface_core(dev, data, data->f->domain->topology,
			PM_z_n_ac_2d, PG_scan_convert_ac);

    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_SURFACE_N - draw a surface plot given a known mesh type, a
 *                   - suitable scanning function, and a scalar array
 *                   - rotate to view angle, plot with hidden line
 *                   - algorithm, and shade if requested
 */

void PG_draw_surface_n(PG_device *dev, double *a1, double *a2, double *extr,
		       double **r, int nn, double *va, double width,
		       int color, int style, PG_rendering pty,
		       char *name, char *mesh_type, void *cnnct, pcons *alist)
   {int imx, jmx;
    int nrd;
    int *maxes;
    PM_set *domain, *range;
    PG_graph *g;
    pcons *alst;

    nrd = SC_ref_count(r[0]);

    domain = NULL;
    range  = NULL;

/* build the domain set */
    if (strcmp(mesh_type, PM_LR_S) == 0)
       {maxes  = (int *) cnnct;
	imx    = maxes[0];
	jmx    = maxes[1];
	domain = PM_make_set("{x,y}", SC_DOUBLE_S, FALSE, 2, imx, jmx,
			     2, r[0], r[1]);}

    else if (strcmp(mesh_type, PM_AC_S) == 0)
       domain = PM_make_ac_set("{x,y}", SC_DOUBLE_S, FALSE,
			       (PM_mesh_topology *) cnnct, 2, r[0], r[1]);

/* build the range set */
    if (a2 == NULL)
       range = PM_make_set("{a}", SC_DOUBLE_S, FALSE, 1, nn, 1, a1);
    else
       range = PM_make_set("{u,v}", SC_DOUBLE_S, FALSE, 1, nn, 2, a1, a2);

    alst = SC_copy_alist(alist);

    g = PG_make_graph_from_sets(name, domain, range, N_CENT,
				SC_PCONS_P_S, alst, 'A', NULL);

/* add the attributes */
    PG_set_attrs_graph(g,
		       "LINE-COLOR", SC_INT_I, FALSE, color,
		       "LINE-STYLE", SC_INT_I, FALSE, style,
		       "LINE-WIDTH", SC_DOUBLE_I,  FALSE, width,
		       "THETA",      SC_DOUBLE_I,  FALSE, va[0],
		       "PHI",        SC_DOUBLE_I,  FALSE, va[1],
		       "CHI",        SC_DOUBLE_I,  FALSE, va[2],
		       NULL);

    g->rendering = pty;

    PG_surface_plot(dev, g);

    PG_rl_graph(g, (nrd > 0), TRUE);

    return;}

/*--------------------------------------------------------------------------*/

/*                            FORTRAN API ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* PGPLSF - low level surface plot routine */

FIXNUM F77_FUNC(pgplsf, PGPLSF)(FIXNUM *devid, double *px, double *py, double *pz,
			     FIXNUM *pn, double *pxn, double *pxx,
			     double *pyn, double *pyx, double *pzn, double *pzx,
			     FIXNUM *pkx, FIXNUM *plx,
			     double *pth, double *pph, double *pch,
			     FIXNUM *ptyp, FIXNUM *pcol, double *pwid,
			     FIXNUM *psty, FIXNUM *pnc, F77_string label)
   {int sty, clr;
    int maxes[2];
    FIXNUM rv;
    double ext[PG_BOXSZ], va[PG_SPACEDM];
    double *r[PG_SPACEDM];
    char llabel[MAXLINE];
    PG_rendering pty;
    PG_device *dev;

    maxes[0] = (int) *pkx;
    maxes[1] = (int) *plx;

    clr = *pcol;
    sty = *psty;
    pty = (PG_rendering) *ptyp;

    ext[0] = *pzn;
    ext[1] = *pzx;
    ext[2] = *pzn;
    ext[3] = *pzx; 

    SC_FORTRAN_STR_C(llabel, label, *pnc);

    r[0] = px;
    r[1] = py;

    va[0] = *pth;
    va[1] = *pph;
    va[2] = *pch;

    dev = SC_GET_POINTER(PG_device, *devid);

    PG_draw_surface_n(dev, pz, pz, ext, r, *pn,
		      va, *pwid, clr, sty,
		      pty, llabel, PM_LR_S, maxes, NULL);

    rv = TRUE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

