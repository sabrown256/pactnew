/*
 * MLGEOM.C - geometry routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

typedef struct s_pt pt;

struct s_pt
   {int i;
    double x;
    double y;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_CROSS - test for the intersection of lines or parts of lines
 *           - LINEx has value 0, 1, 2 for segment, ray, or line
 *           - the lines are defined by the vectors: X1 and X2
 *           - and vectors: X3 and X4
 *           - for rays X1 and X3 are the terminations of the ray
 *           - (i.e. tail of vector)
 *           - and X2 and X4 are X1 - dXa and X3 - dXb (dXa and dXb define
 *           - the direction of the rays)
 *           - here vectors are defined as X = (x, y)
 *           -
 *           - the algorithm is as follows:
 *           -
 *           - (1) X1 + (X2-X1)t = X3 + (X4-X3)s   is the equation for the
 *           -                                     intersection
 *           -
 *           - if 0 <= t <= 1 and  0 <= s <= 1 then the segments intersect
 *           - the determinant of these two equations is
 *           - d = (X1-X2)x(X3-X4)
 *           -
 *           - if d = 0 then the lines are parallel and further tests
 *           - are:
 *           -
 *           - if the following condition is true:
 *           -
 *           - (2) (X2-X1)x(X3-X1) # 0
 *           -
 *           - then the lines do not intersect
 *           - otherwise, the intersection is the point given by the
 *           - smallest positive value for either t1 and t2 less than
 *           - unity. t1 and t2 satify the following:
 *           -
 *           - (3) X3 + (X4-X3)t1 = X1   and    X3 + (X4-X3)t2 = X2
 *           -                           or
 *           -     X1 + (X2-X1)t1 = X3   and    X1 + (X2-X1)t2 = X4
 */

int _PM_cross(double x1, double y1, double x2, double y2,
	      double x3, double y3, double x4, double y4,
	      double *px0, double *py0,
	      int line1, int line2)
   {int cross_flag;
    double a, b, dx21, dy21, dx43, dy43, dx13, dy13, idx, idy;
    double d, t1, t2;

/* assume they don't intersect */
    cross_flag = FALSE;

    t1 = -HUGE;
    t2 = -HUGE;

/* start far away */
    *px0 = HUGE;
    *py0 = HUGE;

    dx21 = x2 - x1;
    dy21 = y2 - y1;
    dx43 = x4 - x3;
    dy43 = y4 - y3;
    dx13 = x1 - x3;
    dy13 = y1 - y3;

/* find determinant for equation (1) */
    a = dx21*dy43;
    b = dy21*dx43;

/* check for parallel lines (i.e. d = 0 or a = b) */
    if (PM_CLOSETO_REL(a, b))

/* check the cross product (2) (i.e. cp = 0 or a = b) */
       {a = dx13*dy21;
	b = dx21*dy13;
	if (PM_CLOSETO_REL(a, b))

/* compute and test t1 and t2 */
           {if (x2 == x1)
               {idy = 1.0/dy43;
		t1  = dy13*idy;
                t2  = (y2 - y3)*idy;}
            else if (dx43 != 0.0)
               {idx = 1.0/dx43;
		t1  = dx13*idx;
                t2  = (x2 - x3)*idx;}
            else
               {idx = 1.0/dx21;
		t1  = -dx13*idx;
                t2  = (x4 - x1)*idx;};

	    *px0 = x3 + dx43*min(t1, t2);
	    *py0 = y3 + dy43*min(t1, t2);}}

/* compute the point of intersection of the segment and ray */
    else
       {d  = 1.0/(a - b);
        t1 = (dy13*dx43 - dx13*dy43)*d;
        t2 = (dy13*dx21 - dx13*dy21)*d;

	*px0 = x1 + dx21*t1;
	*py0 = y1 + dy21*t1;};

    switch (line1)
       {case 0 :
	     switch (line2)
	         {case 0 :
		       cross_flag = ((-TOLERANCE <= t1) &&
				     (t1 <= 1.0000000001) &&
				     (-TOLERANCE <= t2) &&
				     (t2 <= 1.0000000001));
		       break;
		  case 1 :
		       cross_flag = ((-TOLERANCE <= t1) &&
				     (t1 <= 1.0000000001) &&
				     (-TOLERANCE <= t2));
		       break;
		  case 2 :
		       cross_flag = ((-TOLERANCE <= t1) &&
				     (t1 <= 1.0000000001));
		       break;};
	     break;
	case 1 :
	     switch (line2)
	        {case 0 :
		      cross_flag = ((-TOLERANCE <= t1) &&
				    (-TOLERANCE <= t2) &&
					(t2 <= 1.0000000001));
		      break;
		 case 1 :
		      cross_flag = ((-TOLERANCE <= t1) &&
				    (-TOLERANCE <= t2));
		      break;
		 case 2 :
		      cross_flag = (-TOLERANCE <= t1);
		      break;};
	     break;
        case 2 :
	     switch (line2)
	        {case 0 :
		      cross_flag = ((-TOLERANCE <= t2) &&
				    (t2 <= 1.0000000001));
		      break;
		 case 1 :
		      cross_flag = (-TOLERANCE <= t2);
		      break;
		 case 2 :
		      cross_flag = TRUE;
		      break;};
	     break;};

    return(cross_flag);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CROSS - test for the intersection of a line segment and a ray */

int PM_cross(double x1, double y1, double x2, double y2,
	     double x3, double y3, double x4, double y4,
	     double *px0, double *py0)
   {int rv;

    rv = _PM_cross(x1, y1, x2, y2, x3, y3, x4, y4, px0, py0, 0, 1);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CROSS_SEG - test for the intersection of a two line segments */

int PM_cross_seg(double x1, double y1, double x2, double y2,
		 double x3, double y3, double x4, double y4,
		 double *px0, double *py0)
   {int rv;

    rv = _PM_cross(x1, y1, x2, y2, x3, y3, x4, y4, px0, py0, 0, 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CROSS_LINE_PLANE - test for the intersection of lines and a plane
 *                     - LINE has value 0, 1, 2 for segment, ray, or line
 *                     - the line is defined by the vectors: X1 and X2
 *                     - the plane is defined by the vectors: X3, X4, X5
 *                     - for rays X1 is the terminations of the ray
 *                     - (i.e. tail of vector)
 *                     - and X2 is X1 - dX (dX defines the direction of
 *                     - the ray)
 *                     - here vectors are defined as X = (x, y)
 *                     -
 *                     - the algorithm is as follows:
 *                     -
 *                     - (1) (X1 + t*(X2-X1) - X4).((X5-X4)x(X3-X4)) = 0
 *                     -     is the equation for the intersection
 *                     -     of a line and plane
 *                     -
 */

int PM_cross_line_plane(double x1, double y1, double z1,
			double x2, double y2, double z2,
			double *px, double *py, double *pz,
			double *px0, double *py0, double *pz0,
			int line)
   {int cross_flag;
    double a, b, t;
    double nx, ny, nz;
    double x0, y0, z0;
    double dx54, dy54, dz54, dx34, dy34, dz34;
    double dx14, dy14, dz14, dx21, dy21, dz21;

/* assume they don't intersect */
    cross_flag = FALSE;

    dx54 = px[0] - px[1];
    dy54 = py[0] - py[1];
    dz54 = pz[0] - pz[1];

    dx34 = px[2] - px[1];
    dy34 = py[2] - py[1];
    dz34 = pz[2] - pz[1];

    dx14 = x1 - px[1];
    dy14 = y1 - py[1];
    dz14 = z1 - pz[1];

    dx21 = x2 - x1;
    dy21 = y2 - y1;
    dz21 = z2 - z1;

    nx = dy54*dz34 - dz54*dy34;
    ny = dz54*dx34 - dx54*dz34;
    nz = dx54*dy34 - dy54*dx34;

    a = dx21*nx + dy21*ny + dz21*nz;
    b = dx14*nx + dy14*ny + dz14*nz;
    t = (a == 0.0) ? HUGE : -b/a;

    if (t == HUGE)
       {x0 = HUGE;
	y0 = HUGE;
	z0 = HUGE;
        cross_flag = FALSE;}
    else
       {x0 = x1 + dx21*t;
	y0 = y1 + dy21*t;
	z0 = z1 + dz21*t;
        cross_flag = TRUE;};

    switch (line)
       {case 0 :
	     cross_flag &= ((-TOLERANCE <= t) && (t <= 1.0000000001));
	     break;
	case 1 :
	     cross_flag &= (-TOLERANCE <= t);
	     break;
        case 2 :
	     cross_flag &= TRUE;
	     break;};

    *px0 = x0;
    *py0 = y0;
    *pz0 = z0;

    return(cross_flag);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_INSERT_POINT - insert I into the vector D */

static INLINE int _PM_insert_point(int i, int ln, int lx, int **pd, int *pn)
    {int l, n;
     int *d;

     n = *pn;
     d = *pd;

     lx++;

     if (n >= lx)
        {n += 10;
	 REMAKE_N(d, int, n);};

     for (l = lx-1; l > ln; l--)
         d[l] = d[l-1];

     d[l] = i;

     *pn = n;
     *pd = d;

     return(lx);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_BY_ANGLE - return TRUE iff angle of a < angle of b */

static int _PM_by_angle(void *a, void *b)
   {int ok;
    double aa, ab;
    pt *pa, *pb;

    pa = (pt *) a;
    pb = (pt *) b;

    aa = atan2(pa->y, pa->x);
    ab = atan2(pb->y, pb->x);

    if ((aa == 0.0) && (ab == 0.0))
       ok = (pa->x < pb->x);
    else
       ok = (aa < ab);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CONVEX_HULL - create the polygon which is the convex hull of the
 *                - given set of points
 *                - use the Graham scan algorithm
 */

PM_polygon *PM_convex_hull(double *p1, double *p2, int nh)
   {int i, imn, np;
    double xc, yc, xmn, ymn, cp;
    double *x, *y;
    pt *pa;
    SC_array *a;
    PM_polygon *py;

    a = SC_MAKE_ARRAY("PM_CONVEX_HULL", pt, NULL);
    SC_array_resize(a, nh+10, -1.0);
    SC_array_set_n(a, nh);

    pa = SC_array_array(a);

/* load the array and find the leftmost minimum y point */
    xmn = HUGE;
    ymn = HUGE;
    imn = -1;
    for (i = 0; i < nh; i++)
        {xc = p1[i];
	 yc = p2[i];
         pa[i].i = i;
	 pa[i].x = xc;
	 pa[i].y = yc;
         if (yc < ymn)
	    {xmn = xc;
	     ymn = yc;
	     imn = i;}
	 else if ((yc == ymn) && (xc < xmn))
	    {xmn = xc;
	     imn = i;};};

/* shift to minimum point */
    for (i = 0; i < nh; i++)
        {pa[i].x -= xmn;
	 pa[i].y -= ymn;};

/* make the first point be the minimum y point */
    SC_SWAP_VALUE(pt, pa[imn], pa[0]);
    SFREE(pa);

    SC_array_sort(a, _PM_by_angle);
    pa = SC_array_array(a);

    pa[nh] = pa[0];
    np = 1;
    for (i = np+1; i <= nh; i++)
        {for (cp = -1.0; (cp <= 0.0) && (np > 0); np--)
	     cp = PM_DELTA_CROSS_2D(pa[np-1].x, pa[np-1].y,
				    pa[np].x, pa[np].y,
				    pa[i].x, pa[i].y);
	 np += 2;
	 SC_SWAP_VALUE(pt, pa[np], pa[i]);};

/* make the return polygon */
    py = PM_init_polygon(2, np+1);
    x  = py->x[0];
    y  = py->x[1];

    for (i = 0; i < np; i++)
        {x[i] = pa[i].x + xmn;
         y[i] = pa[i].y + ymn;};

    x[np] = x[0];
    y[np] = y[0];

    py->nn = py->np;

    SFREE(pa);
    SC_free_array(a, NULL);

    return(py);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COLINEAR_2D - return TRUE iff the n points are colinear */

int PM_colinear_2d(double *px, double *py, int n)
   {int i, col;
    int ifx0, ify0, ifxi0, ifyi0;
    double x1, y1, dx, dy, dxi, dyi;
    double rx, ry;

    x1 = px[0];
    y1 = py[0];

    dx = px[1] - x1;
    dy = py[1] - y1;

    ifx0  = (ABS(dx) < TOLERANCE);
    ify0  = (ABS(dy) < TOLERANCE);

    for (i = 2, col = TRUE; (i < n) && col; i++)
        {dxi = px[i] - x1;
         dyi = py[i] - y1;

	 ifxi0 = (ABS(dxi) < TOLERANCE);
	 ifyi0 = (ABS(dyi) < TOLERANCE);

         if ((ifx0 && ifxi0) || (ify0 && ifyi0))
	    col = TRUE;
         else
            {rx  = (ifx0 ? HUGE : dxi/dx);
             ry  = (ify0 ? HUGE : dyi/dy);
             col = PM_CLOSETO_REL(rx, ry);};};

    return(col);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COLINEAR_3D - return TRUE iff the n points are colinear */

int PM_colinear_3d(double *px, double *py, double *pz, int n)
   {int i, col;
    int ifx0, ify0, ifz0, ifxi0, ifyi0, ifzi0;
    double rx, ry, rz;
    double x1, y1, z1, dx, dy, dz, dxi, dyi, dzi;

    x1 = px[0];
    y1 = py[0];
    z1 = pz[0];

    if (PM_CLOSETO_REL(x1, px[1]))
       dx = 0.0;
    else
       dx = px[1] - x1;

    if (PM_CLOSETO_REL(y1, py[1]))
       dy = 0.0;
    else
       dy = py[1] - y1;

    if (PM_CLOSETO_REL(z1, pz[1]))
       dz = 0.0;
    else
       dz = pz[1] - z1;

    ifx0  = (ABS(dx) < TOLERANCE);
    ify0  = (ABS(dy) < TOLERANCE);
    ifz0  = (ABS(dz) < TOLERANCE);

    for (i = 2, col = TRUE; (i < n) && col; i++)
        {ifxi0 = PM_CLOSETO_REL(x1, px[i]);
	 dxi   = ifxi0 ? 0.0 : px[i] - x1;
	 rx    = ifx0 ? HUGE : dxi/dx;

         ifyi0 = PM_CLOSETO_REL(y1, py[i]);
         dyi   = ifyi0 ? 0.0 : py[i] - y1;
	 ry    = ify0 ? HUGE : dyi/dy;

         ifzi0 = PM_CLOSETO_REL(z1, pz[i]);
         dzi   = ifzi0 ? 0.0 : pz[i] - z1;
	 rz    = ifz0 ? HUGE : dzi/dz;

         if (ifx0 && ifxi0)
            {if ((ify0 && ifyi0) || (ifz0 && ifzi0))
	        col = TRUE;
             else
	        col = PM_CLOSETO_REL(ry, rz);}
         else if (ify0 && ifyi0)
	    {if (ifz0 && ifzi0)
	        col = TRUE;
             else
	        col = PM_CLOSETO_REL(rz, rx);}
         else if (ifz0 && ifzi0)
	    col = PM_CLOSETO_REL(rx, ry);
         else
	    col = PM_CLOSETO_REL(rx, ry) && PM_CLOSETO_REL(ry, rz);};

    return(col);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CONVEX_CONTAINS_2D - check whether the point (Xc,Yc) is contained
 *                       - in the convex polygon Py
 *                       - return values:
 *                       -     2  in the interior
 *                       -     1  on the boundary
 *                       -     0  outside
 */

int PM_convex_contains_2d(double xc, double yc, PM_polygon *py)
   {int i, n, ni, rv;
    double cp, nrm, sx, sy, sc;
    double *x, *y;

    if (_PM.tol == -1.0)
       _PM.tol = 10.0*PM_machine_precision();

    x = py->x[0];
    y = py->x[1];
    n = py->np;

/* determine the length scale hence the absolute tolerance */
    nrm = 1.0/((double) n);
    sx  = xc*xc;
    sy  = yc*yc;
    for (i = 0; i < n; i++)
        {sx += x[i]*x[i];
	 sy += y[i]*y[i];};
    sc = _PM.tol*(sx + sy)*nrm;

/* check cross products with all sides */
    rv = 0;
    ni = 0;
    for (i = 1; i < n; i++)
        {cp = PM_DELTA_CROSS_2D(x[i-1], y[i-1], x[i], y[i], xc, yc);
	 if (cp < -sc)
            break;
         ni += (cp > sc);};

/* make final determination */
    if (i >= n)
       rv = (ni == n-1) ? 2 : 1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CONTAINS_2D - check whether the point (Xc, Yc) is contained
 *                - in the possibly non-convex polygon Py
 *                - return TRUE if the point is in the interior
 *                - return FALSE if on the boundary or in the exterior
 *                - a point is considered inside a non-convex polygon
 *                - iff it is contained in an odd number of the triangles
 *                - which are constructed from an arbitrary point in
 *                - space and the nodes of each edge of the polygon in turn
 */

int PM_contains_2d(double xc, double yc, PM_polygon *py)
   {int i, n, nm, ic, rv;
    double *x, *y, *xp, *yp;
    PM_polygon *pt;

    x = py->x[0];
    y = py->x[1];
    n = py->np;

/* initialize a triangle template */
    pt = PM_init_polygon(2, 4);
    xp = pt->x[0];
    yp = pt->x[1];

/* use the first node as the common point of the test triangle
 * since any point will do and this
 * means we get to loop from 2 to n-1 instead of 1 to n
 */
    xp[0] = x[0];
    yp[0] = y[0];
    xp[3] = x[0];
    yp[3] = y[0];

/* count the number of test triangles containing the point */
    ic = 0;
    nm = n - 1;
    for (i = 2; i < nm; i++)

/* do not check degenerate side */
        {if ((x[i-1] != x[i]) || (y[i-1] != y[i]))

/* complete the test triangle containing one side of the polygon */
	    {xp[1] = x[i-1];
	     yp[1] = y[i-1];
	     xp[2] = x[i];
	     yp[2] = y[i];
	     PM_orient_polygon(pt);

	     ic += PM_convex_contains_2d(xc, yc, pt);};};

/* if IC is odd the point is inside of Py */
    ic >>= 1;
    rv   = (ic & 1);

    PM_free_polygon(pt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CONTAINS_3D - check whether the 3D point is contained in (below) the
 *                - triangle defined by the ordered points (PX, PY, PZ)
 *                - if BND is TRUE return TRUE iff the point is in the
 *                - interior
 *                - otherwise return TRUE iff the point is on the boundary
 *                - or in the interior
 */

int PM_contains_3d(double x, double y, double z,
		   double *px, double *py, double *pz, int bnd)
   {int i, if1, if2, if3;
    double x1, y1, z1, x2, y2, z2;
    double dx1, dy1, dz1, dx2, dy2, dz2;
    double nx[3], ny[3], nz[3];
    double d1, d2, d3;

    x1 = px[2];
    y1 = py[2];
    z1 = pz[2];
    for (i = 0; i < 3; i++)
        {x2 = px[i];
	 y2 = py[i];
	 z2 = pz[i];

	 dx1 = x2 - x1;
	 dy1 = y2 - y1;
	 dz1 = z2 - z1;

         dx2 = x - x1;
         dy2 = y - y1;
         dz2 = z - z1;

         nx[i] = dy1*dz2 - dz1*dy2;
         ny[i] = dz1*dx2 - dx1*dz2;
         nz[i] = dx1*dy2 - dy1*dx2;

	 x1 = x2;
	 y1 = y2;
	 z1 = z2;};

    d1 = nx[0]*nx[1] + ny[0]*ny[1] + nz[0]*nz[1];
    d2 = nx[1]*nx[2] + ny[1]*ny[2] + nz[1]*nz[2];
    d3 = nx[2]*nx[0] + ny[2]*ny[0] + nz[2]*nz[0];

    if (bnd)
       {if1 = (d1 > -TOLERANCE);
	if2 = (d2 > -TOLERANCE);
	if3 = (d3 > -TOLERANCE);}
    else
       {if1 = (d1 > TOLERANCE);
	if2 = (d2 > TOLERANCE);
	if3 = (d3 > TOLERANCE);};

    return(if1 && if2 && if3);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_POLYGON_AREA - compute the area of the polygon PY */

double PM_polygon_area(PM_polygon *py)
   {int i, np;
    double x1, x2, x3, y1, y2, y3, ac;
    double *x, *y;

    x  = py->x[0];
    y  = py->x[1];
    np = py->np - 1;

    x1 = x[0];
    y1 = y[0];

    ac = 0.0;
    for (i = 1; i < np; i++)
        {x2 = x[i];
	 y2 = y[i];
	 x3 = x[i+1];
         y3 = y[i+1];

	 ac += (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1);};

    return(ac);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERSECT_LINE_POLYGON - compute the intersection points of the
 *                           - given line (Xmn,Ymn) -> (Xmx,Ymx)
 *                           - with the given polygon (Px,Py) of Np nodes
 *                           - return the number of intersection points PIC
 *                           - actually does clip to polygon
 *                           - return FALSE if the line segment is completely
 *                           - outside the polygon
 */

int PM_intersect_line_polygon(double *pxmn, double *pymn,
			      double *pxmx, double *pymx,
			      PM_polygon *py, int *pic)
   {int i, ic, np, p1, p2, rv;
    double x0, y0, x1, x2, x3, x4, y1, y2, y3, y4;
    double xr[10], yr[10];
    double *x, *y;

    x  = py->x[0];
    y  = py->x[1];
    np = py->np;

    x1 = *pxmn;
    y1 = *pymn;
    x2 = *pxmx;
    y2 = *pymx;

    for (i = 0; i < 10; i++)
        {xr[i] = 0.0;
	 yr[i] = 0.0;};

    ic = 0;
    for (i = 1; i < np; i++)
        {x3 = x[i];
         y3 = y[i];
         x4 = x[i-1];
         y4 = y[i-1];
         if (PM_cross_seg(x1, y1, x2, y2, x3, y3, x4, y4, &x0, &y0))
            {xr[ic] = x0;
	     yr[ic] = y0;
	     ic++;};};

    *pic = ic;

    p1 = FALSE;
    p2 = FALSE;
    if (ic < 2)
       {p1 = PM_contains_2d(x1, y1, py);
        p2 = PM_contains_2d(x2, y2, py);
        if (ic == 1)
	   {if (p1 == TRUE)
	       {xr[1] = x1;
                yr[1] = y1;}
	    else
	       {xr[1] = x2;
                yr[1] = y2;};}

	else if ((p1 == TRUE) && (p2 == TRUE))
	   {xr[0] = x1;
	    yr[0] = y1;
	    xr[1] = x2;
	    yr[1] = y2;};};

    *pxmn = xr[0];
    *pymn = yr[0];
    *pxmx = xr[1];
    *pymx = yr[1];

    rv = ((ic > 1) || p1 || p2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ADD_NODE - helper for PM_intersect_polygons */

static INLINE void _PM_add_node(PM_polygon *pd, double *x)
   {int n, id, nd;

    nd = pd->nd;
    n  = pd->nn++;

    for (id = 0; id < nd; id++)
        pd->x[id][n] = x[id];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_INTERSECT_POLY - traversing boundary of polygon PN
 *                    - accumlate points contained in polygon PP or boundary
 *                    - intersection points
 */

static void _PM_intersect_poly(PM_polygon *pc, PM_polygon *pn, PM_polygon *pp)
			      
   {int i, i1, i2, ic, id, ok, nd, ns, nn, nx;
    double x1[3], x2[3];
    double *xn, *yn;

    xn = pn->x[0];
    yn = pn->x[1];
    nn = pn->np;
    nd = pn->nd;

    nx = pc->np;
    ns = pc->nn;

    for (id = 0; id < nd; id++)
        x1[id] = pn->x[id][0];

    i1 = PM_contains_2d(x1[0], x1[1], pp);
    if (i1 == TRUE)
       _PM_add_node(pc, x1);

/* travserse the perimeter of P looking for entries and exits */
    for (i = 1; (i < nn) && (pp->nn < nx); i++)
        {for (id = 0; id < nd; id++)
	     {x1[id] = pn->x[id][i-1];
	      x2[id] = pn->x[id][i];};

	 i2 = PM_contains_2d(x2[0], x2[1], pp);
	 if (i2 == TRUE)

/* both points inside P */
	    {if (i1 == TRUE)
	        {_PM_add_node(pc, x2);}

/* entering P */
	     else if (i1 == FALSE)
	        {ok = PM_intersect_line_polygon(&x1[0], &x1[1],
						&x2[0], &x2[1], pp, &ic);
		 if ((ok == TRUE) && (ic > 0))
		    _PM_add_node(pc, x1);

		 _PM_add_node(pc, x2);};}

	 else if (i2 == FALSE)

/* exiting P */
	    {if (i1 == TRUE)
	        {ok = PM_intersect_line_polygon(&x1[0], &x1[1],
						&x2[0], &x2[1], pp, &ic);
		 if (ok == TRUE)
		    _PM_add_node(pc, x1);};};

	 i1 = i2;};

/* close the polygon if we end up on the outside */
    if ((i1 == FALSE) && (pc->nn > ns))
       {for (id = 0; id < nd; id++)
	    x1[id] = pc->x[id][ns];
        _PM_add_node(pc, x1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERSECT_POLYGONS - compute the intersection points of
 *                       - polygon Pa and Pb
 *                       - the polygons may be multiply connected,
 *                       - and non-convex but must be closed
 *                       - return the intersection polygon
 */

PM_polygon *PM_intersect_polygons(PM_polygon *pa, PM_polygon *pb)
   {int nx;
    PM_polygon *pc;

    nx = max(pa->np, pb->np);
    nx = 4*nx;
    pc = PM_init_polygon(pa->nd, nx);

/* force both polygons to have counter-clockwise (right-handed) orientation */
    PM_orient_polygon(pa);
    PM_orient_polygon(pb);

/* find the part of A contained in B */
    _PM_intersect_poly(pc, pa, pb);

/* find the part of B contained in A */
    _PM_intersect_poly(pc, pb, pa);

/* GOTCHA: we really have multiple polygons here in general
 * do we need to merge them?
 */

    return(pc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ORIENT_POLYGON - check that the given polygon is
 *                   - oriented so that the perimeter is traversed
 *                   - counter-clockwise
 */

void PM_orient_polygon(PM_polygon *p)
   {int i, id, j, n, nd, nh;
    double orient;
    double *x, *y;

    x  = p->x[0];
    y  = p->x[1];
    n  = p->np;
    nd = p->nd;

    orient = PM_DELTA_CROSS_2D(x[1], y[1], x[2], y[2], x[0], y[0]);
    if (orient < 0.0)
       {nh = n >> 1;
	for (i = 0; i < nh; i++)
	    {j  = n - i - 1;

	     for (id = 0; id < nd; id++)
	         {SC_SWAP_VALUE(double, p->x[id][i], p->x[id][j]);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_POLYGON_ORIENT - return the orientation of the polygon
 *                   -   1 if simply connected and counter-clockwise
 *                   -   0 if multiply connected
 *                   -  -1 if simply connected and clockwise
 *                   -  -2 error
 */

int PM_polygon_orient(PM_polygon *p)
   {int i, n, nm, np, rv;
    double cp;
    double *x, *y;

    x = p->x[0];
    y = p->x[1];
    n = p->np;

    nm = 0;
    np = 0;
    for (i = 2; i < n; i++)
        {cp = PM_DELTA_CROSS_2D(x[i-1], y[i-1], x[i], y[i], x[i-2], y[i-2]);
	 nm += (cp < 0.0);
	 np += (cp >= 0.0);};

    if ((nm != 0) && (np != 0))
       rv = 0;
    else if (nm == n-2)
       rv = -1;
    else if (np == n-2)
       rv = 1;
    else
       rv = -2;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_NEAREST_POINT - find the point in an array of points nearest
 *                  - a specified point
 */

void PM_nearest_point(PM_polygon *py, double xs, double ys,
		      double *pxt, double *pyt, int *pi)
   {int i, n, indx;
    double s, smn, dx, dy;
    double *x, *y;

    x = py->x[0];
    y = py->x[1];
    n = py->np;

    smn  = HUGE;
    indx = -1;
    for (i = 0; i < n; i++)
        {dx  = x[i] - xs;
         dy  = y[i] - ys;
         s   = HYPOT(dx, dy);
         smn = min(smn, s);
         if (s == smn)
            indx = i;};

    if (indx > -1)
       {*pi  = indx;
        *pxt = x[indx];
        *pyt = y[indx];};

    return;}

/*--------------------------------------------------------------------------*/

/*                            VOLUME ROUTINES                               */

/*--------------------------------------------------------------------------*/

/* PM_COMPUTE_QUAD_ROT_VOLUME - compute the a set of quadrilateral volumes
 *                            - of rotation
 *                            - NC is the number of quads
 *                            - INDX contains NC sets of 4 indeces into the
 *                            - 2 components of X defining the quads
 */

void PM_compute_quad_rot_volume(double *vol, int nc, int *indx, double **x)
   {int i, i1, i2, i3, i4;
    int *ri;
    double z1, z2, z3, z4;
    double r1, r2, r3, r4;
    double v1, v2;
    double *rz, *rr;

    PM_set_value(vol, nc, 0.0);

    ri = indx;

    rz = x[0];
    rr = x[1];

    for (i = 0; i < nc; i++)
        {i1 = *ri++;
	 i2 = *ri++;
	 i3 = *ri++;
	 i4 = *ri++;

         z1 = rz[i1];
	 z2 = rz[i2];
	 z3 = rz[i3];
	 z4 = rz[i4];

         r1 = rr[i1];
	 r2 = rr[i2];
	 r3 = rr[i3];
	 r4 = rr[i4];

	 PM_VOL_TRI_CYL(v1, 2, 1, 4);
	 PM_VOL_TRI_CYL(v2, 4, 3, 2);

	 vol[i] = v1 + v2;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COMPUTE_HEX_VOLUME - compute the cell volumes for a list of hexahedra
 *                       - NC is the number of cells
 *                       - INDX contains NC sets of 8 indeces into the
 *                       - 3 components of X defining the cells
 */

void PM_compute_hex_volume(double *vol, int nc, int *indx, double **x)
   {int i, i1, i2, i3, i4, i5, i6, i7, i8;
    int *ri;
    double vca, vcb;
    double v1, v2, v3, v4, v5, v6;
    double x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
    double x5, y5, z5, x6, y6, z6, x7, y7, z7, x8, y8, z8;
    double *rx, *ry, *rz;

    PM_set_value(vol, nc, 0.0);

    ri = indx;

    rx = x[0] - 1;
    ry = x[1] - 1;
    rz = x[2] - 1;

    for (i = 0; i < nc; i++)
        {i1 = *ri++;
	 i2 = *ri++;
	 i3 = *ri++;
	 i4 = *ri++;
	 i5 = *ri++;
	 i6 = *ri++;
	 i7 = *ri++;
	 i8 = *ri++;

         x1 = rx[1];
	 x2 = rx[2];
	 x3 = rx[3];
	 x4 = rx[4];
	 x5 = rx[5];
	 x6 = rx[6];
	 x7 = rx[7];
	 x8 = rx[8];

         y1 = ry[1];
	 y2 = ry[2];
	 y3 = ry[3];
	 y4 = ry[4];
	 y5 = ry[5];
	 y6 = ry[6];
	 y7 = ry[7];
	 y8 = ry[8];

         z1 = rz[1];
	 z2 = rz[2];
	 z3 = rz[3];
	 z4 = rz[4];
	 z5 = rz[5];
	 z6 = rz[6];
	 z7 = rz[7];
	 z8 = rz[8];

	 v1 = PM_DAX(1, 4, 3, 2);
	 v2 = PM_DAX(1, 2, 6, 5);
	 v3 = PM_DAX(1, 5, 8, 4);
	 v4 = PM_DAX(7, 8, 5, 6);
	 v5 = PM_DAX(7, 3, 8, 4);
	 v6 = PM_DAX(7, 6, 2, 3);
			  
	 vca = v1 + v2 + v3 + v4 + v5 + v6;

	 v1 = PM_VOLTET(1, 2, 4, 5);
	 v2 = PM_VOLTET(3, 4, 2, 7);
	 v3 = PM_VOLTET(6, 2, 5, 7);
	 v4 = PM_VOLTET(8, 7, 4, 5);
	 v5 = PM_VOLTET(4, 2, 7, 5);
			  
	 vcb = v1 + v2 + v3 + v4 + v5 + v6;

	 vol[i] = vca;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_VECTOR_EXTREMA - find the extrema of N ND vectors
 *                   - EXTR must by 2*ND
 */

void PM_vector_extrema(int nd, int n, double **x, double *extr)
   {int i, l, id;
    double mn, mx, xc;

    if (extr != NULL)
       {for (id = 0; id < nd; id++)
	    {l = 2*id;

	     mn =  HUGE_REAL;
	     mx = -HUGE_REAL;
	     for (i = 0; i < n; i++)
	         {xc = x[id][i];
		  mn = min(mn, xc);
		  mx = max(mx, xc);};

	     extr[l]   = mn;
	     extr[l+1] = mx;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_VECTOR_SELECT_EXTREMA - find the extrema of N ND vectors
 *                          - where MAP is non-zero
 *                          - EXTR must by 2*ND
 */

void PM_vector_select_extrema(int nd, int n, double **x,
			      char *map, double *extr)
   {int i, l, id;
    double mn, mx, xc;

    if (extr != NULL)
       {for (id = 0; id < nd; id++)
	    {l = 2*id;

	     mn =  HUGE_REAL;
	     mx = -HUGE_REAL;
	     for (i = 0; i < n; i++)
	         {if (map[i] != 0)
		     {xc = x[id][i];
		      mn = min(mn, xc);
		      mx = max(mx, xc);};};

	     extr[l]   = mn;
	     extr[l+1] = mx;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SCALE_VECTORS - scale N ND vectors X by S */

void PM_scale_vectors(int nd, int n, double **x, double *s)
   {int i, id;
    double sc;
    double *px;

    for (id = 0; id < nd; id++)
        {sc = s[id];
	 px = x[id];
	 for (i = 0; i < n; i++, *px++ *= sc);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_TRANSLATE_VECTORS - translate N ND vectors X by D */

void PM_translate_vectors(int nd, int n, double **x, double *d)
   {int i, id;
    double dc;
    double *px;

    for (id = 0; id < nd; id++)
        {dc = d[id];
	 px = x[id];
	 for (i = 0; i < n; i++, *px++ += dc);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ROTATE_2D - rotate the 2D vectors X about X0
 *              - by the angle, A (theta) in radians
 */

static void _PM_rotate_2d(int n, double **x, double *x0, double a)
   {int i;
    double ct, st, r11, r12, r21, r22;
    double xc, yc, xo[2];
    double *px, *py;

    if (x0 == NULL)
       {x0 = xo;
	for (i = 0; i < 2; i++)
	    xo[i] = 0.0;};

    ct = cos(a);
    st = sin(a);

    r11 =  ct;
    r12 =  st;
    r21 = -st;
    r22 =  ct;

    px = x[0];
    py = x[1];

    for (i = 0; i < n; i++)
        {xc = px[i] - xo[0];
         yc = py[i] - xo[1];

         px[i] = xo[0] + r11*xc + r12*yc;
         py[i] = xo[1] + r21*xc + r22*yc;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ROTATE_3D - rotate the vectors X about X0
 *               - by the angles, VA (theta, phi, and chi) in radians
 *               - rotations (clockwise and in radians) are:
 *               -    by phi about z axis from positive y axis
 *               -    by theta about x axis from positive z' axis 
 *               -    by chi about z axis from positive y'' axis
 */

static void _PM_rotate_3d(int n, double **x, double *xo,
			  double *a)
   {int i;
    double ct, st, cp, sp, cc, sc;
    double ctcp, stcp, ctsp, stsp;
    double r[3][3];
    double rc[3], xc[3], x0[3];
    double theta, phi, chi;

    if (xo == NULL)
       {xo = x0;
	for (i = 0; i < 3; i++)
	    x0[i] = 0.0;};

    theta = a[0];
    phi   = a[1];
    chi   = a[2];

    ct = cos(theta);
    st = sin(theta);
    cp = cos(phi);
    sp = sin(phi);
    cc = cos(chi);
    sc = sin(chi);

    ctcp = ct*cp;
    ctsp = ct*sp;
    stcp = st*cp;
    stsp = st*sp;

    r[0][0] =  cc*cp - sc*stsp;
    r[0][1] =  cc*sp + sc*ctcp;
    r[0][2] =  sc*st;
    r[1][0] = -sc*cp - cc*ctsp;
    r[1][1] = -sc*sp + cc*ctcp;
    r[1][2] =  cc*st;
    r[2][0] =  stsp;
    r[2][1] = -stcp;
    r[2][2] =  ct;

    for (i = 0; i < n; i++)
        {xc[0] = x[0][i] - xo[0];
         xc[1] = x[1][i] - xo[1];
         xc[2] = x[2][i] - xo[2];

         rc[0] = r[0][0]*xc[0] + r[0][1]*xc[1] + r[0][2]*xc[2];
         rc[1] = r[1][0]*xc[0] + r[1][1]*xc[1] + r[1][2]*xc[2];
         rc[2] = r[2][0]*xc[0] + r[2][1]*xc[1] + r[2][2]*xc[2];

         rc[0] += xo[0];
         rc[1] += xo[1];
         rc[2] += xo[2];

         x[0][i] = rc[0];
         x[1][i] = rc[1];
         x[2][i] = rc[2];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ROTATE_VECTORS - rotate N ND vectors X about X0
 *                   - by the Euler angles, A (theta, phi, and chi) in radians
 *                   - rotations (clockwise and in radians) are:
 *                   -    by phi about z axis from positive y axis
 *                   -    by theta about x axis from positive z' axis 
 *                   -    by chi about z axis from positive y'' axis
 */

void PM_rotate_vectors(int nd, int n, double **x, double *x0, double *a)
   {

    if (nd == 2)
       _PM_rotate_2d(n, x, x0, a[0]);

    else if (nd == 3)
       _PM_rotate_3d(n, x, x0, a);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CONVERT_VECTORS - convert N ND vectors X to type double */

double **PM_convert_vectors(int nd, int n, void *v, char *typ)
   {int id;
    double **t, **x;

    x = (double **) v;
    t = FMAKE_N(double *, nd, "PM_CONVERT_VECTORS:t");

    for (id = 0; id < nd; id++)
        t[id] = PM_array_real(typ, x[id], n, NULL);

    return(t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRVCT - diagnostic print of vectors */

void dprvct(int nd, int n, double **v)
   {int i, id;

    if (n < 0)
       n = SC_arrlen(v[0])/sizeof(double);

    for (i = 0; i < n; i++)
        {for (id = 0; id < nd; id++)
	     io_printf(stdout, "  %8.3e", v[id][i]);
	 io_printf(stdout, "\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DPRPLY - diagnostic print of polygons */

void dprply(PM_polygon *py)
   {int nn, nd;

    nd = py->nd;
    nn = py->nn;

    io_printf(stdout, "Dimensions = %d,   # Nodes = %d\n", nd, nn);
    dprvct(nd, nn, py->x);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

