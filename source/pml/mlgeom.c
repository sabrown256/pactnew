/*
 * MLGEOM.C - geometry routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#define ORIENTATION(_rv, _x1, _x2, _x3)                                      \
   {double _cp;                                                              \
    _cp = PM_DELTA_CROSS_2D(_x1[0], _x1[1], _x2[0], _x2[1], _x3[0], _x3[1]); \
    if (ABS(_cp) < TOLERANCE)                                                \
       _rv = 0;                                                              \
    else                                                                     \
       _rv = (_cp < 0) ? -1 : 1;}

typedef struct s_pt pt;

struct s_pt
   {int i;
    double x[PM_SPACEDM];};

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
 *           -
 *           - the determinant of these two equations is
 *           -    d = (X1-X2)x(X3-X4)
 *           -
 *           - if d = 0 then the lines are parallel and further tests
 *           - are:
 *           -
 *           - if the following condition is true:
 *           -
 *           - (2) (X2-X1)x(X3-X1) != 0
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

int _PM_cross(double *x1, double *x2, double *x3, double *x4,
	      double *x0, int line1, int line2)
   {int cross;
    double a, b, dx21, dy21, dx43, dy43, dx13, dy13, idx, idy;
    double d, t1, t2;

/* assume they don't intersect */
    cross = FALSE;

    t1 = -HUGE;
    t2 = -HUGE;

/* start far away */
    x0[0] = HUGE;
    x0[1] = HUGE;

    dx21 = x2[0] - x1[0];
    dy21 = x2[1] - x1[1];
    dx43 = x4[0] - x3[0];
    dy43 = x4[1] - x3[1];
    dx13 = x1[0] - x3[0];
    dy13 = x1[1] - x3[1];

/* if either vector is zero */
    if (((dx21 == 0.0) && (dy21 == 0.0)) ||
	((dx43 == 0.0) && (dy43 == 0.0)))
       return(cross);

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
           {if (x2[0] == x1[0])
               {idy = 1.0/dy43;
		t1  = dy13*idy;
                t2  = (x2[1] - x3[1])*idy;}
            else if (dx43 != 0.0)
               {idx = 1.0/dx43;
		t1  = dx13*idx;
                t2  = (x2[0] - x3[0])*idx;}
            else
               {idx = 1.0/dx21;
		t1  = -dx13*idx;
                t2  = (x4[0] - x1[0])*idx;};

	    x0[0] = x3[0] + dx43*min(t1, t2);
	    x0[1] = x3[1] + dy43*min(t1, t2);}}

/* compute the point of intersection of the segment and ray */
    else
       {d  = 1.0/(a - b);
        t1 = (dy13*dx43 - dx13*dy43)*d;
        t2 = (dy13*dx21 - dx13*dy21)*d;

	x0[0] = x1[0] + dx21*t1;
	x0[1] = x1[1] + dy21*t1;};

    switch (line1)
       {case 0 :
	     switch (line2)
	         {case 0 :
		       cross = ((-TOLERANCE <= t1) && (t1 <= 1.0000000001) &&
				(-TOLERANCE <= t2) && (t2 <= 1.0000000001));
		       break;
		  case 1 :
		       cross = ((-TOLERANCE <= t1) && (t1 <= 1.0000000001) &&
				(-TOLERANCE <= t2));
		       break;
		  case 2 :
		       cross = ((-TOLERANCE <= t1) && (t1 <= 1.0000000001));
		       break;};
	     break;
	case 1 :
	     switch (line2)
	        {case 0 :
		      cross = ((-TOLERANCE <= t1) &&
			       (-TOLERANCE <= t2) && (t2 <= 1.0000000001));
		      break;
		 case 1 :
		      cross = ((-TOLERANCE <= t1) && (-TOLERANCE <= t2));
		      break;
		 case 2 :
		      cross = (-TOLERANCE <= t1);
		      break;};
	     break;
        case 2 :
	     switch (line2)
	        {case 0 :
		      cross = ((-TOLERANCE <= t2) && (t2 <= 1.0000000001));
		      break;
		 case 1 :
		      cross = (-TOLERANCE <= t2);
		      break;
		 case 2 :
		      cross = TRUE;
		      break;};
	     break;};

    return(cross);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CROSS - test for the intersection of a line segment and a ray */

int PM_cross(double *x1, double *x2, double *x3, double *x4, double *x0)
   {int rv;

    rv = _PM_cross(x1, x2, x3, x4, x0, 0, 1);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CROSS_SEG - test for the intersection of a two line segments */

int PM_cross_seg(double *x1, double *x2, double *x3, double *x4, double *x0)
   {int rv;

    rv = _PM_cross(x1, x2, x3, x4, x0, 0, 0);

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

int PM_cross_line_plane(double *x1, double *x2,
			double **px, double *x0, int line)
   {int cross;
    double a, b, t;
    double nx[PM_SPACEDM];
    double dx54, dy54, dz54, dx34, dy34, dz34;
    double dx14, dy14, dz14, dx21, dy21, dz21;

/* assume they don't intersect */
    cross = FALSE;

    dx54 = px[0][0] - px[0][1];
    dy54 = px[1][0] - px[1][1];
    dz54 = px[2][0] - px[2][1];

    dx34 = px[0][2] - px[0][1];
    dy34 = px[1][2] - px[1][1];
    dz34 = px[2][2] - px[2][1];

    dx14 = x1[0] - px[0][1];
    dy14 = x1[1] - px[1][1];
    dz14 = x1[2] - px[2][1];

    dx21 = x2[0] - x1[0];
    dy21 = x2[1] - x1[1];
    dz21 = x2[2] - x1[2];

    nx[0] = dy54*dz34 - dz54*dy34;
    nx[1] = dz54*dx34 - dx54*dz34;
    nx[2] = dx54*dy34 - dy54*dx34;

    a = dx21*nx[0] + dy21*nx[1] + dz21*nx[2];
    b = dx14*nx[0] + dy14*nx[1] + dz14*nx[2];
    t = (a == 0.0) ? HUGE : -b/a;

    if (t == HUGE)
       {x0[0] = HUGE;
	x0[1] = HUGE;
	x0[2] = HUGE;
        cross = FALSE;}
    else
       {x0[0] = x1[0] + dx21*t;
	x0[1] = x1[1] + dy21*t;
	x0[2] = x1[2] + dz21*t;
        cross = TRUE;};

    switch (line)
       {case 0 :
	     cross &= ((-TOLERANCE <= t) && (t <= 1.0000000001));
	     break;
	case 1 :
	     cross &= (-TOLERANCE <= t);
	     break;
        case 2 :
	     cross &= TRUE;
	     break;};

    return(cross);}

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

    aa = atan2(pa->x[1], pa->x[0]);
    ab = atan2(pb->x[1], pb->x[0]);

    if ((aa == 0.0) && (ab == 0.0))
       ok = (pa->x[0] < pb->x[0]);
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
    double cp;
    double xc[PM_SPACEDM], xmn[PM_SPACEDM];
    double **x;
    pt *pa;
    SC_array *a;
    PM_polygon *py;

    a = SC_MAKE_ARRAY("PM_CONVEX_HULL", pt, NULL);
    SC_array_resize(a, nh+10, -1.0);
    SC_array_set_n(a, nh);

    pa = SC_array_array(a);

/* load the array and find the leftmost minimum y point */
    xmn[0] = HUGE;
    xmn[1] = HUGE;
    imn = -1;
    for (i = 0; i < nh; i++)
        {xc[0] = p1[i];
	 xc[1] = p2[i];
         pa[i].i = i;
	 pa[i].x[0] = xc[0];
	 pa[i].x[1] = xc[1];
         if (xc[1] < xmn[1])
	    {xmn[0] = xc[0];
	     xmn[1] = xc[1];
	     imn    = i;}
	 else if ((xc[1] == xmn[1]) && (xc[0] < xmn[0]))
	    {xmn[0] = xc[0];
	     imn    = i;};};

/* shift to minimum point */
    for (i = 0; i < nh; i++)
        {pa[i].x[0] -= xmn[0];
	 pa[i].x[1] -= xmn[1];};

/* make the first point be the minimum y point */
    SC_SWAP_VALUE(pt, pa[imn], pa[0]);
    SFREE(pa);

    SC_array_sort(a, _PM_by_angle);
    pa = SC_array_array(a);

    pa[nh] = pa[0];
    np = 1;
    for (i = np+1; i <= nh; i++)
        {for (cp = -1.0; (cp <= 0.0) && (np > 0); np--)
	     cp = PM_DELTA_CROSS_2D(pa[np-1].x[0], pa[np-1].x[1],
				    pa[np].x[0], pa[np].x[1],
				    pa[i].x[0], pa[i].x[1]);
	 np += 2;
	 SC_SWAP_VALUE(pt, pa[np], pa[i]);};

/* make the return polygon */
    py = PM_init_polygon(2, np+1);
    x  = py->x;

    for (i = 0; i < np; i++)
        {x[0][i] = pa[i].x[0] + xmn[0];
         x[1][i] = pa[i].x[1] + xmn[1];};

    x[0][np] = x[0][0];
    x[1][np] = x[1][0];

    py->nn = py->np;

    SFREE(pa);
    SC_free_array(a, NULL);

    return(py);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COLINEAR_2D - return TRUE iff the N points X are colinear */

static int _PM_colinear_2d(double **x, int n)
   {int i, id, nd, col;
    int ifx[PM_SPACEDM], ifxi[PM_SPACEDM];
    double dx[PM_SPACEDM], dxi[PM_SPACEDM];
    double rx[PM_SPACEDM], x1[PM_SPACEDM];

    nd = 2;

    for (id = 0; id < nd; id++)
        {x1[id]  = x[id][0];
	 dx[id]  = x[id][1] - x1[id];
	 ifx[id] = (ABS(dx[id]) < TOLERANCE);};

    for (i = 2, col = TRUE; (i < n) && col; i++)
        {for (id = 0; id < nd; id++)
	     {dxi[id]  = x[id][i] - x1[id];
	      ifxi[id] = (ABS(dxi[id]) < TOLERANCE);};

         if ((ifx[0] && ifxi[0]) || (ifx[1] && ifxi[1]))
	    col = TRUE;
         else
	    {for (id = 0; id < nd; id++)
	         rx[id] = (ifx[id] ? HUGE : dxi[id]/dx[id]);
             col = PM_CLOSETO_REL(rx[0], rx[1]);};};

    return(col);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COLINEAR_3D - return TRUE iff the N points X are colinear */

static int _PM_colinear_3d(double **x, int n)
   {int i, id, nd, col;
    int ifx[PM_SPACEDM], ifxi[PM_SPACEDM];
    double rx[PM_SPACEDM], dx[PM_SPACEDM], dxi[PM_SPACEDM];
    double x1[PM_SPACEDM];

    nd = 3;

    for (id = 0; id < nd; id++)
        {x1[id] = x[id][0];

         if (PM_CLOSETO_REL(x1[id], x[id][1]))
	    dx[id] = 0.0;
	 else
	    dx[id] = x[id][1] - x1[id];

        ifx[id] = (ABS(dx[0]) < TOLERANCE);};

    for (i = 2, col = TRUE; (i < n) && (col == TRUE); i++)
        {for (id = 0; id < nd; id++)
	     {ifxi[id] = PM_CLOSETO_REL(x1[id], x[id][i]);
	      dxi[id]  = ifxi[id] ? 0.0 : x[id][i] - x1[id];
	      rx[id]   = ifx[id] ? HUGE : dxi[id]/dx[id];};

         if (ifx[0] && ifxi[0])
            {if ((ifx[1] && ifxi[1]) || (ifx[2] && ifxi[2]))
	        col = TRUE;
             else
	        col = PM_CLOSETO_REL(rx[1], rx[2]);}

         else if (ifx[1] && ifxi[1])
	    {if (ifx[2] && ifxi[2])
	        col = TRUE;
             else
	        col = PM_CLOSETO_REL(rx[2], rx[0]);}

         else if (ifx[2] && ifxi[2])
	    col = PM_CLOSETO_REL(rx[0], rx[1]);

         else
	    col = PM_CLOSETO_REL(rx[0], rx[1]) && PM_CLOSETO_REL(rx[1], rx[2]);};

    return(col);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COLINEAR_ND - return TRUE iff the N points X are colinear */

int PM_colinear_nd(int nd, int n, double **x)
   {int col;

    if (nd == 2)
       col = _PM_colinear_2d(x, n);
    else if (nd == 3)
       col = _PM_colinear_3d(x, n);

    return(col);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CONVEX_CONTAINS_2D - check whether the point XC is contained
 *                       - in the convex polygon Py
 *                       - return values:
 *                       -     2  in the interior of positively oriented
 *                       -     1  on the boundary of positively oriented
 *                       -     0  outside
 *                       -    -1  on the boundary of negatively oriented
 *                       -    -2  in the interior of negatively oriented
 */

int PM_convex_contains_2d(double *xc, PM_polygon *py)
   {int i, n, nm, np, rv;
    double cp, nrm, sc;
    double sx[PM_SPACEDM];
    double **x;

    if (_PM.tol == -1.0)
       _PM.tol = 10.0*PM_machine_precision();

    x = py->x;
    n = py->nn;

/* determine the length scale hence the absolute tolerance */
    nrm = 1.0/((double) n);
    sx[0] = xc[0]*xc[0];
    sx[1] = xc[1]*xc[1];
    for (i = 0; i < n; i++)
        {sx[0] += x[0][i]*x[0][i];
	 sx[1] += x[1][i]*x[1][i];};
    sc = _PM.tol*(sx[0] + sx[1])*nrm;

/* check cross products with all sides */
    rv = 0;
    np = 0;
    nm = 0;
    for (i = 1; i < n; i++)
        {cp = PM_DELTA_CROSS_2D(x[0][i-1], x[1][i-1], x[0][i], x[1][i],
				xc[0], xc[1]);
	 nm += (cp < -sc);
	 np += (cp > sc);};

/* no negatively oriented sides - it is inside or on */
    if (nm == 0)
       rv = (np == n-1) ? 2 : 1;

/* no positively oriented sides - it is inside or on */
    else if (np == 0)
       rv = (nm == n-1) ? -2 : -1;

/* otherwise it is outside */
    else
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_CONTAINS_2D - check whether the point XC is contained
 *                 - in the possibly non-convex polygon Py
 *                 - if BND is TRUE return TRUE iff the point is in the
 *                 - interior
 *                 - otherwise return TRUE iff the point is on the boundary
 *                 - or in the interior
 *                 - a point is considered inside a non-convex polygon
 *                 - iff it is contained in an odd number of the triangles
 *                 - which are constructed from an arbitrary point in
 *                 - space and the nodes of each edge of the polygon in turn
 */

static int _PM_contains_2d(double *xc, PM_polygon *py, int bnd)
   {int i, n, nm, ic, rv;
    double **x, **xp;
    PM_polygon *pt;

    x = py->x;
    n = py->nn;

/* initialize a triangle template */
    pt = PM_init_polygon(2, 4);
    pt->nn = 4;
    xp = pt->x;

    rv = TRUE;

/* check for XC being a node when interior only is requested
 * stop immediately in that case to save flops
 */
    if (bnd == TRUE)
       rv = (PM_vct_equal(2, xc, py->x, 0, -1.0) == FALSE);

/* use the first node as the common point of the test triangle
 * since any point will do and this
 * means we get to loop from 2 to n-1 instead of 1 to n
 */
    if (rv == TRUE)
       {xp[0][0] = x[0][0];
	xp[1][0] = x[1][0];
	xp[0][3] = x[0][0];
	xp[1][3] = x[1][0];

/* count the number of test triangles containing the point */
	ic = 0;
	nm = n - 1;
	for (i = 2; (i < nm) && (rv == TRUE); i++)
	    {if (bnd == TRUE)
	        rv = (PM_vct_equal(2, xc, py->x, i-1, -1.0) == FALSE);

/* do not check degenerate side */
	     if ((rv == TRUE) &&
		 ((x[0][i-1] != x[0][i]) || (x[1][i-1] != x[1][i])))

/* complete the test triangle containing one side of the polygon */
	        {xp[0][1] = x[0][i-1];
		 xp[1][1] = x[1][i-1];
		 xp[0][2] = x[0][i];
		 xp[1][2] = x[1][i];

		 ic += PM_convex_contains_2d(xc, pt);};};

	if ((rv == TRUE) && (bnd == TRUE))
	   rv = (PM_vct_equal(2, xc, py->x, nm, -1.0) == FALSE);

/* if IC is odd the point is inside of Py */
	ic >>= 1;
	rv  &= (ic & 1);};

    PM_free_polygon(pt);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_CONTAINS_3D - check whether the 3D point is contained in the
 *                 - polygon PY
 *                 - if BND is TRUE return TRUE iff the point is in the
 *                 - interior
 *                 - otherwise return TRUE iff the point is on the boundary
 *                 - or in the interior
 */

static int _PM_contains_3d(double *xc, PM_polygon *py, int bnd)
   {int i, id, jd, nd, rv;
    double tol, ds1, ds2, dx1c, dx2c, nrm;
    double x1[PM_SPACEDM], x2[PM_SPACEDM];
    double dx1[PM_SPACEDM], dx2[PM_SPACEDM];
    double nx[PM_SPACEDM][PM_SPACEDM];
    double d[PM_SPACEDM];

    nd  = 3;
    tol = TOLERANCE;

/* start with the 3rd node of the polygon */
    PM_polygon_get_point(x1, py, 2);

    for (i = 0; i < 3; i++)
        {PM_polygon_get_point(x2, py, i);
	      
	 ds1 = 0.0;
	 ds2 = 0.0;
	 for (id = 0; id < nd; id++)
	     {dx1c = x2[id] - x1[id];
	      dx2c = xc[id] - x1[id];
	      dx1[id] = dx1c;
	      dx2[id] = dx2c;
	      ds1 += dx1c*dx1c;
	      ds2 += dx2c*dx2c;};

	 ds1 = sqrt(ds1 + SMALL);
	 ds2 = sqrt(ds2 + SMALL);
	 nrm = 1.0/(ds1*ds2);

         nx[0][i] = (dx1[1]*dx2[2] - dx1[2]*dx2[1])*nrm;
         nx[1][i] = (dx1[2]*dx2[0] - dx1[0]*dx2[2])*nrm;
         nx[2][i] = (dx1[0]*dx2[1] - dx1[1]*dx2[0])*nrm;

	 PM_copy_point(nd, x1, x2);};

    for (id = 0; id < nd; id++)
        {jd = (id + 1) % nd;
	 d[id] = nx[0][id]*nx[0][jd] +
	         nx[1][id]*nx[1][jd] +
	         nx[2][id]*nx[2][jd];};

    rv = TRUE;

    if (bnd == TRUE)
       {for (id = 0; id < nd; id++)
	    rv &= (d[id] > -tol);}
    else
       {for (id = 0; id < nd; id++)
	    rv &= (d[id] > tol);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CONTAINS_ND - check whether the ND point is contained in (below) the
 *                - polygon PY
 *                - if BND is TRUE return TRUE iff the point is in the
 *                - interior
 *                - otherwise return TRUE iff the point is on the boundary
 *                - or in the interior
 */

int PM_contains_nd(double *xc, PM_polygon *py, int bnd)
   {int nd, rv;

    rv = FALSE;
    nd = py->nd;

    if (nd == 2)
       rv = _PM_contains_2d(xc, py, bnd);
    else if (nd == 3)
       rv = _PM_contains_3d(xc, py, bnd);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_POLYGON_AREA - compute the area of the polygon PY */

double PM_polygon_area(PM_polygon *py)
   {int i, np;
    double x1[PM_SPACEDM], x2[PM_SPACEDM], x3[PM_SPACEDM], ac;
    double **x;

    x  = py->x;
    np = py->nn - 1;

    x1[0] = x[0][0];
    x1[1] = x[1][0];

    ac = 0.0;
    for (i = 1; i < np; i++)
        {PM_polygon_get_point(x2, py, i);
	 PM_polygon_get_point(x3, py, i+1);

	 ac += (x2[0] - x1[0])*(x3[1] - x1[1]) -
	       (x3[0] - x1[0])*(x2[1] - x1[1]);};

    return(ac);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_DISTANCE - return the distance between X1 and X2 according to metric G */

double PM_distance(int nd, double *x1, double *x2, double *g)
   {int id, jd;
    double ds, dxs, dxc, dyc, gc;

    dxs = 0.0;
    if (g == NULL)
       {for (id = 0; id < nd; id++)
	    {dxc = x1[id] - x2[id];
	     dxs += dxc*dxc;};}
    else
       {for (id = 0; id < nd; id++)
	    {dxc = x1[id] - x2[id];
	     for (jd = 0; jd < nd; jd++)
	         {gc   = g[jd*nd + id];
		  dyc  = x1[jd] - x2[jd];
		  dxs += gc*dxc*dyc;};};};

    ds = sqrt(dxs);

    return(ds);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SORT_POINTS - order the points in XI by increasing distance from X1 */

static void _PM_sort_points(int nd, double *x1, int n,
			    double **xi, int *sides)
   {int i, j, id, gap, rv;
    double sa, sb;
    double xa[PM_SPACEDM], xb[PM_SPACEDM];

    for (gap = n/2; gap > 0; gap /= 2)
        for (i = gap; i < n; i++)
            for (j = i-gap; j >= 0; j -= gap)
                {PM_vector_get_point(nd, xa, xi, j);
		 PM_vector_get_point(nd, xb, xi, j+gap);
		 
		 sa = PM_distance(nd, xa, x1, NULL);
		 sb = PM_distance(nd, xb, x1, NULL);

		 if (sa < sb)
		    rv = -1;
		 else if (sa > sb)
		    rv = 1;
		 else
		    rv = 0;
        
		 if (rv <= 0)
                    break;

/* swap point and side indeces */
                 SC_SWAP_VALUE(int, sides[j], sides[j+gap]);
		 for (id = 0; id < nd; id++)
		     SC_SWAP_VALUE(double, xi[id][j], xi[id][j+gap]);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERSECT_LINE_POLYGON - compute the intersection points of the
 *                           - given line X1 -> X2
 *                           - with the given polygon PY
 *                           - return the number of intersection points PNI
 *                           - return the intersection points in PXI
 *                           - return the sides of PY intersected in PSIDES
 *                           - actually does clip to polygon
 *                           - return FALSE if the line segment is completely
 *                           - outside the polygon
 *                           - include point X1 if WH & 1
 *                           - include point X2 if WH & 2
 */

int PM_intersect_line_polygon(int *pni, double ***pxi, int **psides,
			      double *x1, double *x2, PM_polygon *py, int wh)
   {int i, n, ni, nd, nn, p1, p2, rv, rej;
    int *sides;
    double x0[PM_SPACEDM], x3[PM_SPACEDM], x4[PM_SPACEDM];
    double **xi;

    nn = py->nn;
    nd = py->nd;

    n     = 100;
    xi    = PM_make_vectors(2, n);
    sides = FMAKE_N(int, n, "PM_INTERSECT_LINE_POLYGON:sides");

    ni = 0;
    for (i = 1; i < nn; i++)
        {PM_polygon_get_point(x3, py, i);
	 PM_polygon_get_point(x4, py, i-1);

         if (PM_cross_seg(x1, x2, x3, x4, x0))
	    {rej  = FALSE;
	     rej |= (((wh & 1) == 0) &&
		     (PM_array_equal(x1, x0, nd, -1.0) == TRUE));
	     rej |= (((wh & 2) == 0) &&
		     (PM_array_equal(x2, x0, nd, -1.0) == TRUE));
	     if (rej == FALSE)
	        {PM_vector_put_point(nd, x0, xi, ni);
		 sides[ni] = i-1;
		 ni++;};};};

/* sort the crossings to increasing distance from X1 */
    _PM_sort_points(nd, x1, ni, xi, sides);

/* adjust return values */
    p1 = FALSE;
    p2 = FALSE;
    if (ni < 2)
       {p1 = PM_contains_nd(x1, py, TRUE);
        p2 = PM_contains_nd(x2, py, TRUE);
        if (ni == 1)
	   {if (p1 == TRUE)
	       {PM_vector_put_point(nd, x1, xi, 1);}
	    else
	       {PM_vector_put_point(nd, x2, xi, 1);};}

	else if ((p1 == TRUE) && (p2 == TRUE))
	   {PM_vector_put_point(nd, x1, xi, 0);
	    PM_vector_put_point(nd, x2, xi, 1);};};

    *pni = ni;

    if (pxi == NULL)
       PM_free_vectors(2, xi);
    else
       *pxi = xi;

    if (psides == NULL)
       {SFREE(sides);}
    else
       *psides = sides;

    rv = ((ni > 1) || p1 || p2);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_POLYGON_ARRAY - initialize an array of polygons */

SC_array *PM_polygon_array(void)
   {SC_array *a;

    a = SC_MAKE_ARRAY("PM_POLYGON_ARRAY", PM_polygon *, NULL);

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FREE_POLYGONS - free an array of polygons */

void PM_free_polygons(SC_array *a, int rl)
   {int ip, np;
    PM_polygon *py;

    if (rl == TRUE)
       {np = SC_array_get_n(a);
	for (ip = 0; ip < np; ip++)
	    {py = PM_polygon_get(a, ip);
	     PM_free_polygon(py);};};

    SC_free_array(a, NULL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_POLYGON_PUSH - push PY onto A */

void PM_polygon_push(SC_array *a, PM_polygon *py)
   {

    SC_array_push(a, &py);
    SC_mark(py, 1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_POLYGON_GET - return the Nth polygon from A */

PM_polygon *PM_polygon_get(SC_array *a, int n)
   {PM_polygon *py;

    py = *(PM_polygon **) SC_array_get(a, n);

    return(py);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ADD_NODE - helper for _PM_combine_polygons
 *              - return TRUE iff X closes the polygon PD
 */

static INLINE int _PM_add_node(PM_polygon *pd, double *x)
   {int nd, n, nm, rv, same;

    n  = pd->nn;
    nd = pd->nd;

/* check if X is same as previous point */
    if (n > 0)
       {nm   = n - 1;
	same = PM_vct_equal(nd, x, pd->x, nm, -1.0);}
    else 
       same = FALSE;

/* only add X if not same as previous point */
    if (same == FALSE)
       {PM_polygon_put_point(x, pd, n);
	pd->nn++;};

    if (n > 1)
       rv = PM_vct_equal(nd, x, pd->x, 0, -1.0);
    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_POLYGON_ENTERING - point XA is the intersection point between
 *                     - side IA of polygon PA and polygon PB
 *                     - side IA is the segment from X1 = PA[IA]
 *                     - to X2 = PA[IA+1]
 *                     - if X2 + eps*(X1 - X2) is outside it is entering
 *                     - otherwise it is leaving
 *                     - return 1 if the side IA is entering PB
 *                     -       -1 if it is leaving
 */

static int PM_polygon_entering(double *xa, PM_polygon *pa, int ia,
			       PM_polygon *pb)
   {int id, ib, na, nd, ok, rv;
    double x1c, x2c, eps;
    double x0[PM_SPACEDM];

    eps = 1.0e-5;
    nd  = pa->nd;
    na  = pa->nn;
    ib  = (ia + 1 + na - 1) % (na - 1);

    ok = TRUE;
    for (id = 0; id < nd; id++)
        {x1c = pa->x[id][ia];
	 x2c = pa->x[id][ib];
	 ok &= PM_CLOSETO_REL(x1c, x2c);
	 x0[id] = xa[id] + eps*(x1c - x2c);};

    if (ok == TRUE)
       rv = 0;
    else
       {rv = PM_contains_nd(x0, pb, FALSE);
	rv = (rv == FALSE) ? 1 : -1;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COMBINE_POLYGONS - compute and return either the intersection or union
 *                      - of the polgons PN and PP depending on
 *                      - the value of OP
 *                      - return the result in PC
 */

static void _PM_combine_polygons(SC_array *a,
				 PM_polygon *pa, PM_polygon *pb,
				 PM_binary_operation op)
   {int i1, i2, id, ip, na, ni, nd, nx;
    int closed, nin, nout, inc, ok;
    int *sides;
    double x1[PM_SPACEDM], x2[PM_SPACEDM], xa[PM_SPACEDM];
    double **xi;
    PM_polygon *pc;

    nd = pa->nd;

    nx = max(pa->np, pb->np);
    nx = 4*nx;
    pc = PM_init_polygon(nd, nx);

/* not absolutely necessary but cleaner */
    for (id = 0; id < PM_SPACEDM; id++)
        {x1[id] = 0.0;
	 x2[id] = 0.0;
	 xa[id] = 0.0;};

    nx = pc->np;

    PM_polygon_get_point(x1, pa, 0);

    i1 = PM_contains_nd(x1, pb, TRUE);
    if (((i1 == TRUE) && (op == PM_INTERSECT)) ||
	((i1 == FALSE) && (op == PM_UNION)))
       _PM_add_node(pc, x1);

/* travserse the perimeter of PA looking for entries and exits from PB */
    inc    = 1;
    nin    = 0;
    nout   = 0;
    closed = FALSE;
    PM_polygon_get_point(x1, pa, 0);
    for (i2 = 1; (pc->nn < nx) && (closed == FALSE); i2 += inc)
        {na = pa->nn - 1;
	 i2 = (i2 + na) % na;
	 PM_polygon_get_point(x2, pa, i2);

	 ok = PM_intersect_line_polygon(&ni, &xi, &sides, x1, x2, pb, 2);

/* no intersection points for (X1, X2) with PB */
	 if (ni == 0)
	    {if (op == PM_INTERSECT)

/* (X1, X2) is interior to PB */
	        {if (ok == TRUE)
		    {closed = _PM_add_node(pc, x2);
		     nin++;}

/* (X1, X2) is exterior to PB */
		 else
		    nout++;

/* if all the points are outside then PB is the intersection */
		 if (nout > na)
		    {closed = TRUE;
		     PM_polygon_copy_points(pc, pb);}
		 else
		    PM_copy_point(nd, x1, x2);}

	     else if (op == PM_UNION)
	        {closed = _PM_add_node(pc, x2);
		 PM_copy_point(nd, x1, x2);};}

/* one or more intersection points for (X1, X2) with PB */
	 else
	    {for (ip = 0; ip < ni; ip++)
	         {PM_vector_get_point(nd, xa, xi, ip);

/* GOTCHA: if XA is also a node of PB we do not know which way to
 * go for the PM_UNION case - need more logic here
 */
		  ok = PM_polygon_entering(xa, pb, sides[ip], pa);

		  closed = _PM_add_node(pc, xa);
		  PM_copy_point(nd, x1, xa);

/* entering */
		  if (((ok == 1) && (op == PM_INTERSECT)) ||
		      ((ok == -1) && (op == PM_UNION)))
		     {inc = 1;
		      i2  = sides[ip];}

/* leaving */
		  else if (((ok == -1) && (op == PM_INTERSECT)) ||
			   ((ok == 1) && (op == PM_UNION)))
		     {inc = -1;
		      i2  = sides[ip] - inc;};

		  SC_SWAP_VALUE(PM_polygon *, pa, pb);
		  break;};};

	 PM_free_vectors(nd, xi);
	 SFREE(sides);};

    PM_polygon_push(a, pc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INTERSECT_POLYGONS - compute an array of polygons defining
 *                       - the intersection polygon PA and PB
 *                       - the polygons may be multiply connected,
 *                       - and non-convex but must be closed
 *                       - return the array of polygons
 */

SC_array *PM_intersect_polygons(SC_array *a, PM_polygon *pa, PM_polygon *pb)
   {

    if (a == NULL)
       a = PM_polygon_array();

    _PM_combine_polygons(a, pa, pb, PM_INTERSECT);

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_UNION_POLYGONS - compute an array of polygons defining
 *                   - the union polygon PA and PB
 *                   - the polygons may be multiply connected,
 *                   - and non-convex but must be closed
 *                   - return the array of polygons
 */

SC_array *PM_union_polygons(SC_array *a, PM_polygon *pa, PM_polygon *pb)
   {

    if (a == NULL)
       a = PM_polygon_array();

    _PM_combine_polygons(a, pa, pb, PM_UNION);

    return(a);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ORIENT_POLYGON - check that the given polygon is
 *                   - oriented so that the perimeter is traversed
 *                   - counter-clockwise
 */

void PM_orient_polygon(PM_polygon *p)
   {int i, id, j, n, nd, nh;
    double orient;
    double **x;

    x  = p->x;
    n  = p->nn;
    nd = p->nd;

    orient = PM_DELTA_CROSS_2D(x[0][1], x[1][1], x[0][2], x[1][2],
			       x[0][0], x[1][0]);
    if (orient < 0.0)
       {nh = n >> 1;
	for (i = 0; i < nh; i++)
	    {j  = n - i - 1;

	     for (id = 0; id < nd; id++)
	         {SC_SWAP_VALUE(double, x[id][i], x[id][j]);};};};

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
    double **x;

    x = p->x;
    n = p->nn;

    nm = 0;
    np = 0;
    for (i = 2; i < n; i++)
        {cp = PM_DELTA_CROSS_2D(x[0][i-1], x[1][i-1], x[0][i], x[1][i],
				x[0][i-2], x[1][i-2]);
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
 *                  - a specified point XS
 *                  - return it in XT
 */

void PM_nearest_point(PM_polygon *py, double *xs, double *xt, int *pi)
   {int i, n, indx;
    double s, smn;
    double dx[PM_SPACEDM];
    double **x;

    x = py->x;
    n = py->nn;

    smn  = HUGE;
    indx = -1;
    for (i = 0; i < n; i++)
        {dx[0]  = x[0][i] - xs[0];
         dx[1]  = x[1][i] - xs[1];
         s   = HYPOT(dx[0], dx[1]);
         smn = min(smn, s);
         if (s == smn)
            indx = i;};

    if (indx > -1)
       {*pi   = indx;
        xt[0] = x[0][indx];
        xt[1] = x[1][indx];};

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
    double r[PM_SPACEDM][PM_SPACEDM];
    double rc[PM_SPACEDM], xc[PM_SPACEDM], x0[PM_SPACEDM];
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

