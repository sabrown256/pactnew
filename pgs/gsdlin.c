/*
 * GSDLIN.C - line drawing routines for PGS
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_ORIGIN_POINT - set XO to be the origin relative to PC */
 
void PG_origin_point(PG_device *dev, int nd, PG_coord_sys cs, double *xo)
   {int id;

    if (dev != NULL)
       {for (id = 0; id < nd; id++)
	    xo[id] = 0.0;

        PG_trans_point(dev, nd, PIXELC, xo, cs, xo);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DRAW_POLYGON - draw a polygon in WC */
 
void PG_draw_polygon(PG_device *dev, PM_polygon *py, int clip)
   {double xo[PG_SPACEDM];
    PG_curve *crv;

    if ((dev != NULL) && (py != NULL) && (py->nn > 0))
       {PG_origin_point(dev, 2, WORLDC, xo);

	crv = PG_make_curve(dev, WORLDC, FALSE, py->nn, xo, py->x);

	PG_draw_curve(dev, crv, clip);

	PG_release_curve(crv);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_MULTIPLE_LINE - draw multiple line segments.  Two lines are
 *                       - specified and the number of line segments to
 *                       - be drawn between them.  This routine is useful
 *                       - in drawing grid lines, cross hatching, and
 *                       - shading.  It is not limited to drawing parallel
 *                       - lines; the two delimiting lines may be at any
 *                       - orientation to each other.
 *                       - nlines - specifies the number of lines to
 *                                  draw between the two given
 *                                  lines.  If n=0, only the two
 *                                  given lines will be drawn
 *                       - X1, X2 - defines the two end points
 *                                  for the first line
 *                       - X3, X4 - defines the two end points
 *                                  for the second line
 */

void PG_draw_multiple_line(PG_device *dev, int nlines,
			   double *x1, double *x2, double *x3, double *x4,
			   double *dx)
   {double sp, dx13, dy13, dx24, dy24, inv_nlines;
    double **t, *px, *py;
    int i, n;

    if (nlines <= 0)
       nlines = 1;

    if (nlines > 1)
       inv_nlines = 1.0/((double) (nlines - 1));
    else
       inv_nlines = 1.0;

    dx13 = x3[0] - x1[0];
    dy13 = x3[1] - x1[1];
    dx24 = x4[0] - x2[0];
    dy24 = x4[1] - x2[1];

    n = nlines << 1;
    t = PM_make_vectors(2, n);

/* plot lines in between, if any */
    px = t[0];
    py = t[1];
    if (dx == NULL)
       {for (i = 0; i < nlines; ++i)
            {sp = ((double) i)*inv_nlines;
             *px++ = x1[0] + sp*dx13;
             *py++ = x1[1] + sp*dy13;
             *px++ = x2[0] + sp*dx24;
             *py++ = x2[1] + sp*dy24;};}
    else
       {for (i = 0; i < nlines; ++i)
            {sp = fabs(dx[i]);
             *px++ = x1[0] + sp*dx13;
             *py++ = x1[1] + sp*dy13;
             *px++ = x2[0] + sp*dx24;
             *py++ = x2[1] + sp*dy24;};};

    PG_draw_disjoint_polyline_n(dev, 2, WORLDC, nlines, t, FALSE);

    PM_free_vectors(2, t);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_LINE_N - Draw ND dimensional line segment (X1, X2) in 
 *                - coordinate system CS on current frame of device DEV.
 *
 * #bind PG_draw_line_n fortran() scheme() python()
 */

void PG_draw_line_n(PG_device *dev ARG(,,cls), int nd,
		    PG_coord_sys cs ARG(WORLDC),
		    double *x1 ARG([0.0,0.0,0.0]),
		    double *x2 ARG([1.0,1.0,1.0]),
		    int clip)
   {int id;
    double xi[PG_SPACEDM][2];
    double l1[PG_SPACEDM], l2[PG_SPACEDM];
    double *p[PG_SPACEDM];

    if (dev != NULL)
       {for (id = 0; id < nd; id++)
	    {xi[id][0] = x1[id];
	     xi[id][1] = x2[id];};

/* put in vector form for transformations */
	for (id = 0; id < PG_SPACEDM; id++)
	    p[id] = &xi[id][0];

/* draw */
	if (nd == 2)
	   {PG_trans_points(dev, 2, nd, cs, p, WORLDC, p);

/* make a copy so as to avoid clobbering X1 and X2 */
	    for (id = 0; id < nd; id++)
	        {l1[id] = xi[id][0];
		 l2[id] = xi[id][1];};

	    if (PG_clip_line_seg(dev, l1, l2))
	       {PG_move_gr_abs_n(dev, l1);
		PG_draw_to_abs_n(dev, l2);};}

	else
	   _PG_rst_draw_disjoint_polyline_3(dev, cs, 1, p, clip, FALSE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_DRAW_POLYLINE_N - Draw a ND polyline of N points X in coordinate
 *                    - system CS on current frame of device DEV.
 *
 * #bind PG_draw_polyline_n fortran() scheme() python()
 */
 
void PG_draw_polyline_n(PG_device *dev ARG(,,cls), int nd,
			PG_coord_sys cs ARG(WORLDC),
			long n, double **x,
			int clip ARG(TRUE))
   {int i, j, m, id;
    double xa, xb;
    double **rx, *px;

    if ((dev != NULL) && (n > 1))
       {if (nd == 2)
	   {PG_curve *crv;
	    double xo[PG_SPACEDM];

	    if (n > 0)
	       {PG_origin_point(dev, nd, cs, xo);
    
		crv = PG_make_curve(dev, cs, FALSE, n, xo, x);

		PG_draw_curve(dev, crv, clip);

		PG_release_curve(crv);};}

        else if (nd == 3)
	   {m  = (n - 1) << 1;
	    rx = PM_make_vectors(3, m);

/* convert polyline into disjoint polyline */
	    for (id = 0; id < 3; id++)
	        {xa = x[id][0];
		 px = rx[id];
		 for (i = 1; i < n; i++)
		     {j  = 2*(i - 1);
		      xb = x[id][i];
		      px[j]   = xa;
		      px[j+1] = xb;
		      xa = xb;};};

	    _PG_rst_draw_disjoint_polyline_3(dev, cs, n, rx, clip, FALSE);

	    PM_free_vectors(3, rx);};};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_PROJECTED_POLYLINE_N - In the current frame of the device DEV,
 *                              - rotate, project, and draw N disjoint line
 *                              - segments defined by the points X which
 *                              - are ND dimensional and specified in the
 *                              - coordinate system CS.  The rotation
 *                              - is by the current view_angle of DEV.
 *                              - The projection is done onto the X-Y plane.
 *                              - N is the number of segments hence half
 *                              - the number of points.
 *                              - X[2*i] is one endpoint of the ith segment
 *                              - and X[2*i+1] is the other endpoint.
 *                              - CLIP specifies wether the set is clipped
 *                              - to the viewport limits.
 *
 * #bind PG_draw_projected_polyline_n fortran() scheme() python()
 */

void PG_draw_projected_polyline_n(PG_device *dev ARG(,,cls), int nd,
				  PG_coord_sys cs ARG(WORLDC),
				  long n, double **x,
				  int clip ARG(TRUE))
   {int rel;
    long np;
    double **t;
    void (*f)(PG_device *dev, double **r, long n,
	      int clip, int coord);

    if ((dev != NULL) && (dev->draw_dj_polyln_2 != NULL))
       {f  = dev->draw_dj_polyln_2;
	np = 2*n;

	if (nd > 2)
	   {PG_rotate_vectors(dev, nd, np, x);
	    t   = PM_project_vectors(2, np, x);
	    rel = TRUE;}
	else
	   {t   = x;
	    rel = FALSE;};

/* if auto ranging or domaining is on the data will control the WC system */
	if (clip && (dev->autorange || dev->autodomain))
	   PG_set_limits_n(dev, nd, cs, np, t, PLOT_CARTESIAN);

	f(dev, t, n, clip, cs);

	if (rel == TRUE)
	   PM_free_vectors(2, t);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_DISJOINT_POLYLINE_N - In the current frame of the device DEV,
 *                             - draw N disjoint line segments defined by
 *                             - the points X which are ND dimensional and
 *                             - specified in the coordinate system CS.
 *                             - N is the number of segments hence half
 *                             - the number of points
 *                             - X[2*i] is one endpoint of the ith segment
 *                             - and X[2*i+1] is the other endpoint
 *                             - CLIP specifies wether the set is clipped
 *                             - to the viewport limits
 *
 * #bind PG_draw_disjoint_polyline_n fortran() scheme() python()
 */

void PG_draw_disjoint_polyline_n(PG_device *dev ARG(,,cls), int nd,
				 PG_coord_sys cs ARG(WORLDC),
				 long n, double **x,
				 int clip ARG(TRUE))
   {

    if (dev != NULL)

/* if auto ranging or domaining is on the data will control the WC system */
       {if (clip && (dev->autorange || dev->autodomain))
	   PG_set_limits_n(dev, nd, cs, 2*n, x, PLOT_CARTESIAN);

	if (nd == 2)
	   {if (dev->draw_dj_polyln_2 != NULL)
	       (*dev->draw_dj_polyln_2)(dev, x, n, clip, cs);}

	else
	   _PG_rst_draw_disjoint_polyline_3(dev, cs, n, x, clip, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_RAD - In the current frame of the device DEV,
 *             - draw a radial line from RMIN to RMAX along the
 *             - direction A given with respect to positive x axis
 *             - (increasing counter-clockwise) from central point (X, Y).
 *             - If UNIT is DEGREE the angle A is in degrees otherwise
 *             - it is taken to be in radians.
 *
 * #bind PG_draw_rad fortran() scheme(pg-draw-radius) python()
 */

void PG_draw_rad(PG_device *dev ARG(,,cls),
		 double rmin, double rmax, double a,
		 double x, double y,
		 int unit ARG(DEGREE))
   {double ca, sa;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];

    if (unit == DEGREE)
       a *= DEG_RAD;

    ca = cos(a);
    sa = sin(a);

    x1[0] = x + rmin*ca;
    x1[1] = y + rmin*sa;
    x2[0] = x + rmax*ca;
    x2[1] = y + rmax*sa;

    PG_draw_line_n(dev, 2, WORLDC, x1, x2, dev->clipping);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_ARC - In the current frame of the device DEV,
 *             - draw an arc with radius, R, from angles A1 to A2
 *             - given with respect to positive x axis
 *             - (increasing counter-clockwise) from central point (X, Y).
 *             - If UNIT is DEGREE the angles are in degrees otherwise
 *             - they are taken to be in radians.
 *
 * #bind PG_draw_arc fortran() scheme() python()
 */

void PG_draw_arc(PG_device *dev ARG(,,cls),
		 double r, double a1, double a2,
		 double x, double y, int unit)
   {double a, da, ca, sa;
    double xr[N_ANGLES], yr[N_ANGLES];
    double *p[PG_SPACEDM];
    int i;

    if (0.0 < r)
       {if (unit == DEGREE)
	   {a1 *= DEG_RAD;
	    a2 *= DEG_RAD;};

	da = (a2-a1)/(N_ANGLES-1);
	for (i = 0, a = a1; i < N_ANGLES; a += da, i++)
	    {ca = cos(a);
	     sa = sin(a);
	     xr[i] = x + r*ca;
	     yr[i] = y + r*sa;};

	p[0] = xr;
	p[1] = yr;

	PG_draw_polyline_n(dev, 2, WORLDC, N_ANGLES, p, TRUE);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

