/*
 * GSTRTST.C - test of PGS geometrical transformations
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs.h"

#define N       100
#define N_TEST  7
#define N_DEV   5

#undef TOLERANCE
#define TOLERANCE  1.0e-8

#define CLOSETO_REL(x1, x2, tol)                                            \
    ((2.0*ABS((x1)-(x2))/(ABS(x1)+ABS(x2)+SMALL) < tol))

typedef struct testdes_s testdes;
typedef int (*gtest)(PG_device *dev, int nd, int np, int icc);
typedef int (*dtest)(char *name, testdes *d, int nt, gtest *tst);

struct testdes_s
   {int mode;
    int debug_mode;};

static double
 dfmx = -HUGE;

static int
 dbg = TRUE;

static char
 *lgf = "log.trans";

#undef WtoS
#undef StoW
#undef PtoS
#undef StoP

/*--------------------------------------------------------------------------*/

/*                                  MACROS                                  */

/*--------------------------------------------------------------------------*/

/* WTOS - converts x and y from world coordinates to screen coordinates */

#define WtoS(dev, x, y)                                                      \
   {x = dev->g.w_nd[0] + dev->g.w_nd[1]*x;                                   \
    y = dev->g.w_nd[2] + dev->g.w_nd[3]*y;                                   \
    x = max(x, -10.0);                                                       \
    x = min(x, 11.0);                                                        \
    y = max(y, -10.0);                                                       \
    y = min(y, 11.0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* STOW - converts x and y from screen coordinates to world coordinates */

#define StoW(dev, x, y)                                                      \
   {x = dev->g.nd_w[0] + dev->g.nd_w[1]*x;                                   \
    y = dev->g.nd_w[2] + dev->g.nd_w[3]*y;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* STOP - converts x and y from NDC/Screen coordinates to pixel coordinates */

#ifdef CRAY                        /* try to compensate for cray arithmetic */

#define StoP(dev, _x, _y, _ix, _iy)                                          \
   {int _pxmn, _pxmx, _pymn, _pymx;                                          \
    double _xd, _yd, _h, _w;                                                 \
    PG_dev_geometry *_g;                                                     \
    _g    = &dev->g;                                                         \
    _w    = _g->hwin[1] - _g->hwin[0];                                       \
    _h    = _g->hwin[3] - _g->hwin[2];                                       \
    _pxmn = _g->cpc[0];                                                      \
    _pxmx = _g->cpc[1];                                                      \
    _pymn = _g->cpc[2];                                                      \
    _pymx = _g->cpc[3];                                                      \
    _x    = max(_x, -10.0);                                                  \
    _x    = min(_x, 11.0);                                                   \
    _y    = max(_y, -10.0);                                                  \
    _y    = min(_y, 11.0);                                                   \
    _xd   = 0;                                                               \
    _yd   = 0;                                                               \
    _xd = _x*_w + 0.50001;                                                   \
    _yd = _y*_h + 0.50001;                                                   \
    _ix = floor(_xd) + dev->window_x[0];                                     \
    _iy = floor(_yd) + dev->window_x[2];                                     \
    _ix = (_ix < _pxmn) ? _pxmn : _ix;                                       \
    _ix = (_ix > _pxmx) ? _pxmx : _ix;                                       \
    _iy = (_iy < _pymn) ? _pymn : _iy;                                       \
    _iy = (_iy > _pymx) ? _pymx : _iy;}

#else

#define StoP(dev, _x, _y, _ix, _iy)                                          \
   {int _pxmn, _pxmx, _pymn, _pymx;                                          \
    double _xd, _yd, _w, _h;                                                 \
    PG_dev_geometry *_g;                                                     \
    _g    = &dev->g;                                                         \
    _w    = _g->hwin[1] - _g->hwin[0];                                       \
    _h    = _g->hwin[3] - _g->hwin[2];                                       \
    _pxmn = _g->cpc[0];                                                      \
    _pxmx = _g->cpc[1];                                                      \
    _pymn = _g->cpc[2];                                                      \
    _pymx = _g->cpc[3];                                                      \
    _x    = max(_x, -10.0);                                                  \
    _x    = min(_x, 11.0);                                                   \
    _y    = max(_y, -10.0);                                                  \
    _y    = min(_y, 11.0);                                                   \
    _xd   = _x*_w + 0.5;                                                     \
    _yd   = _y*_h + 0.5;                                                     \
    _ix = floor(_xd) + dev->window_x[0];                                     \
    _iy = floor(_yd) + dev->window_x[2];                                     \
    _ix = max(_ix, _pxmn);                                                   \
    _ix = min(_ix, _pxmx);                                                   \
    _iy = max(_iy, _pymn);                                                   \
    _iy = min(_iy, _pymx);}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PTOS - converts x and y from pixel coordinates to NDC/Screen coordinates */

#define PtoS(dev, ix, iy, x, y)                                              \
   {double _h, _w;                                                           \
    PG_dev_geometry *_g;                                                     \
    _g = &dev->g;                                                            \
    _w = _g->hwin[1] - _g->hwin[0];                                          \
    _h = _g->hwin[3] - _g->hwin[2];                                          \
     x = (ix - dev->window_x[0])/_w;                                         \
     y = (iy - dev->window_x[2])/_h;}


/*--------------------------------------------------------------------------*/

/*                                FUNCTIONS                                 */

/*--------------------------------------------------------------------------*/

/* DUMP_ERR - dump context of error for reconstruction
 *          - otherwise difficult because all values are randomly chosen
 */

static void dump_err(PG_device *dev, char *s, int nd,
		     double xco, double xcn,
		     double *xo, double *xn,
		     double *bo, double *bn, double tol)
   {int id, l, n;
    double d;
    PG_dev_geometry *g;
    FILE *fp;

    fp = io_open(lgf, "a");

    g = &dev->g;
    n = 2*nd;

    io_printf(fp, "----------------------------------------------\n");
    io_printf(fp, "%s\n", s);

    io_printf(fp, "bnd: ");
    for (l = 0; l < n; l++)
        io_printf(fp, " %12.5e", g->bnd[l]);
    io_printf(fp, "\n");

    io_printf(fp, "wc: ");
    for (l = 0; l < n; l++)
        io_printf(fp, " %12.5e", g->wc[l]);
    io_printf(fp, "\n");

    io_printf(fp, "ndc: ");
    for (l = 0; l < n; l++)
        io_printf(fp, " %12.5e", g->ndc[l]);
    io_printf(fp, "\n");

    io_printf(fp, "pc: ");
    for (l = 0; l < n; l++)
        io_printf(fp, " %12.5e", g->pc[l]);
    io_printf(fp, "\n");

    io_printf(fp, "\n");

    if ((xcn != HUGE) && (xco != HUGE))
       {d = 2.0*(xco - xcn)/(xco + xcn);
	d = ABS(d);
	dfmx = max(dfmx, d);
	io_printf(fp, "cmp:     %12.5e %12.5e %12.5e %12.5e  %s\n",
		  xco, xcn, d, tol,
		  (d < tol) ? "ok" : "ng");};

    if ((xn != NULL) && (xo != NULL))
       {for (id = 0; id < nd; id++)
	    {d = 2.0*(xo[id] - xn[id])/(xo[id] + xn[id]);
	     d = ABS(d);
	     dfmx = max(dfmx, d);
	     io_printf(fp, "pnt:  %d   %12.5e %12.5e %12.5e %12.5e  %s\n",
		       id+1, xo[id], xn[id], d, tol,
		       (d < tol) ? "ok" : "ng");};};

    if ((bn != NULL) && (bo != NULL))
       {for (l = 0; l < n; l++)
	    {d = 2.0*(bo[l] - bn[l])/(bo[l] + bn[l]);
	     d = ABS(d);
	     dfmx = max(dfmx, d);
	     io_printf(fp, "box:  %d   %12.5e %12.5e %12.5e %12.5e  %s\n",
		       l+1, bo[l], bn[l], d, tol,
		       (d < tol) ? "ok" : "ng");};};

    io_printf(fp, "\n");

    io_close(fp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WORLD_BOX_REV - make WC data for test reversibility */

static void world_box_rev(int nd, double *xo, double *bo, double *wc)
   {int id, l;
    double xn[PG_SPACEDM];

    for (id = 0; id < nd; id++)
        {xo[id] = 0.5*(PM_random(0.0) + 1.0);
	 xn[id] = 0.5*(PM_random(0.0) + 1.0);};

    for (id = 0; id < nd; id++)
        {l       = 2*id;
	 xo[id]  = wc[l] + (wc[l+1] - wc[l])*xo[id];
	 xn[id]  = wc[l] + (wc[l+1] - wc[l])*xn[id];
	 bo[l]   = xo[id];
	 bo[l+1] = xn[id];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NORM_BOX_REV - make NDC data for test reversibility */

static void norm_box_rev(int nd, double *xo, double *bo)
   {int id, l;
    double xn[PG_SPACEDM];

    for (id = 0; id < nd; id++)
        {l       = 2*id;
	 xo[id]  = 0.5*(PM_random(0.0) + 1.0);
	 xn[id]  = 0.5*(PM_random(0.0) + 1.0);
	 bo[l]   = xo[id];
	 bo[l+1] = xn[id];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PIXEL_BOX_REV - make PC data for test reversibility */

static void pixel_box_rev(int nd, double *xo, double *bo, int np)
   {int id, l;
    double xn[PG_SPACEDM];

    for (id = 0; id < nd; id++)
        {l       = 2*id;
	 xo[id]  = SC_random_int(1, np);
	 xn[id]  = SC_random_int(1, np);
	 bo[l]   = xo[id];
	 bo[l+1] = xn[id];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_REV - compare points and box for test reversibility */

static int compare_rev(PG_device *dev, char *s, int nd,
		       double *xo, double *xn,
		       double *bo, double *bn, double tol)
   {int id, l, n, ok;

    ok = TRUE;

    for (id = 0; id < nd; id++)
	ok &= CLOSETO_REL(xo[id], xn[id], tol);

    n = 2*nd;
    for (l = 0; l < n; l++)
	ok &= CLOSETO_REL(bo[l], bn[l], tol);

    if ((ok != TRUE) && (dbg == TRUE))
       dump_err(dev, s, nd, HUGE, HUGE, xo, xn, bo, bn, tol);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_BOX - compare boxes for equality */

static int compare_box(PG_device *dev, char *s, int nd,
		       double *ibx, double *rbx, double tol)
   {int l, n, ok;

    ok = TRUE;

    n = 2*nd;
    for (l = 0; l < n; l++)
	ok &= CLOSETO_REL(ibx[l], rbx[l], tol);

    if ((ok != TRUE) && (dbg == TRUE))
       dump_err(dev, s, nd, HUGE, HUGE, NULL, NULL, ibx, rbx, tol);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_CMP - compare compenents for equality */

static int compare_cmp(PG_device *dev, char *s,
		       double o, double n, double tol)
   {int ok;
    double d;
    static int count = 0;

    d = 2.0*(n - o)/(n + o);

    ok = (ABS(d) < tol);

    if ((ok != TRUE) && (dbg == TRUE))
       dump_err(dev, s, PG_SPACEDM, o, n, NULL, NULL, NULL, NULL, tol);

    count++;

    return(ok);}

/*--------------------------------------------------------------------------*/

/*                               TEST #1 ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* TEST_1 - compare old and new transformations on points */

static int test_1(PG_device *dev, int nd, int np, int icc)
   {int j, l, id, ok, rv;
    double x, n, w;
    double xo[PG_SPACEDM], bo[PG_BOXSZ], wc[PG_BOXSZ];
    PG_dev_geometry *g;

    ok = 0;

    g = &dev->g;

    PG_get_viewspace(dev, WORLDC, wc);

    for (j = 0; j < N; j++)

/* check w_nd followed by nd_w */
        {world_box_rev(nd, xo, bo, wc);

	 for (id = 0; id < nd; id++)
	     {l = 2*id;
	      x = xo[id];
	      n = g->w_nd[l] + g->w_nd[l+1]*x;
	      w = g->nd_w[l] + g->nd_w[l+1]*n;

	      ok += compare_cmp(dev, "Test #1 WC->NDC", x, w, TOLERANCE);};

/* check nd_w followed by w_nd */
         norm_box_rev(nd, xo, bo);

	 for (id = 0; id < nd; id++)
	     {l = 2*id;
	      x = xo[id];
	      w = g->nd_w[l] + g->nd_w[l+1]*x;
	      n = g->w_nd[l] + g->w_nd[l+1]*w;

	      ok += compare_cmp(dev, "Test #1 NDC->WC", x, n, TOLERANCE);};};

    rv = (2*N*nd - ok);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                               TEST #2 ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* COMPARE_2 - compare XO and XN */

static int compare_2(PG_device *dev, char *s, int nd,
		     double *xo, double *xn, double tol)
   {int id, ok;
    double d;

    ok = TRUE;

    for (id = 0; id < nd; id++)
        {d = 2.0*(xn[id] - xo[id])/(ABS(xn[id]) + ABS(xo[id]) + SMALL);
	 ok &= (d < tol);};

    if ((ok != TRUE) && (dbg == TRUE))
       dump_err(dev, s, nd, HUGE, HUGE, xo, xn, NULL, NULL, tol);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* POINT_COPY - copy point S to D */

void point_copy(int nd, double *d, double *s)
   {int id;

    for (id = 0; id < nd; id++)
        d[id] = s[id];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_2 - compare old and new transformations on points */

static int test_2(PG_device *dev, int nd, int np, int icc)
   {int j, l, id, ok, rv;
    int ix[PG_SPACEDM];
    double xo[PG_SPACEDM], xn[PG_SPACEDM], wc[PG_BOXSZ];
    double *p[PG_SPACEDM];

    ok = TRUE;

    if (nd == 2)
       {for (id = 0; id < PG_SPACEDM; id++)
	    p[id] = &xn[id];

	PG_get_viewspace(dev, WORLDC, wc);

	for (j = 0; j < N; j++)
	    {

/* compare WtoS */
	     for (id = 0; id < nd; id++)
	         xo[id] = 0.5*(PM_random(0.0) + 1.0);

	     for (id = 0; id < nd; id++)
	         {l      = 2*id;
		  xo[id] = wc[l] + (wc[l+1] - wc[l])*xo[id];};

	     point_copy(nd, xn, xo);

	     WtoS(dev, xo[0], xo[1]);

	     PG_trans_points(dev, 1, nd, WORLDC, p, NORMC, p);

	     ok &= compare_2(dev, "Test #2 WtoS", nd, xo, xn, TOLERANCE);

/* compare StoW */
	     for (id = 0; id < nd; id++)
	         xo[id] = 0.5*(PM_random(0.0) + 1.0);

	     point_copy(nd, xn, xo);

	     StoW(dev, xo[0], xo[1]);

	     PG_trans_points(dev, 1, nd, NORMC, p, WORLDC, p);

	     ok &= compare_2(dev, "Test #2 StoW", nd, xo, xn, TOLERANCE);

/* compare StoP */
	     for (id = 0; id < nd; id++)
	         xo[id] = 0.5*(PM_random(0.0) + 1.0);

	     point_copy(nd, xn, xo);

	     StoP(dev, xo[0], xo[1], xo[0], xo[1]);

	     PG_trans_points(dev, 1, nd, NORMC, p, PIXELC, p);

	     ok &= compare_2(dev, "Test #2 StoP", nd, xo, xn, TOLERANCE);

/* compare PtoS */
	     for (id = 0; id < nd; id++)
	         {ix[id] = SC_random_int(1, np);
		  xn[id] = ix[id];};

	     PtoS(dev, ix[0], ix[1], xo[0], xo[1]);

	     PG_trans_points(dev, 1, nd, PIXELC, p, NORMC, p);

	     ok &= compare_2(dev, "Test #2 PtoS", nd, xo, xn, TOLERANCE);};};

    rv = (1 - ok);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                              TEST #3 ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* WORLD_BOX_3 - make WC data for test #3 */

static void world_box_3(int nd, double *xl, double *xu,
			double *b, double *wc)
   {int id, l;

    for (id = 0; id < nd; id++)
        {xl[id] = 0.5*(PM_random(0.0) + 1.0);
	 xu[id] = 0.5*(PM_random(0.0) + 1.0);};

    for (id = 0; id < nd; id++)
        {l      = 2*id;
	 xl[id] = wc[l] + (wc[l+1] - wc[l])*xl[id];
	 xu[id] = wc[l] + (wc[l+1] - wc[l])*xu[id];
	 b[l]   = xl[id];
	 b[l+1] = xu[id];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NORM_BOX_3 - make NDC data for test #3 */

static void norm_box_3(int nd, double *xl, double *xu, double *b)
   {int id, l;

    for (id = 0; id < nd; id++)
        {l      = 2*id;
	 xl[id] = 0.5*(PM_random(0.0) + 1.0);
	 xu[id] = 0.5*(PM_random(0.0) + 1.0);
	 b[l]   = xl[id];
	 b[l+1] = xu[id];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PIXEL_BOX_3 - make PC data for test #3 */

static void pixel_box_3(int nd, int np, double *xl, double *xu, double *b)
   {int id, l;

    for (id = 0; id < nd; id++)
        {l      = 2*id;
	 xl[id] = SC_random_int(1, np);
	 xu[id] = SC_random_int(1, np);
	 b[l]   = xl[id];
	 b[l+1] = xu[id];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPARE_3 - compare points and box for test #3 */

static int compare_3(PG_device *dev, int nd,
		     double *xl, double *xu, double *b)
   {int id, l, ok;
    double d, tol;

    ok = TRUE;
    tol = 5.0e-3;

    for (id = 0; id < nd; id++)
        {l = 2*id;

	 d   = 2.0*(xl[id] - b[l])/(xl[id] + b[l]);
	 ok &= (ABS(d) < tol);

	 d   = 2.0*(xu[id] - b[l+1])/(xu[id] + b[l+1]);
	 ok &= (ABS(d) < tol);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_3 - compare box and point transformations */

static int test_3(PG_device *dev, int nd, int np, int icc)
   {int j, id, ok, rv;
    double xl[PG_SPACEDM], xu[PG_SPACEDM], wc[PG_BOXSZ], b[PG_BOXSZ];
    double *lw[PG_SPACEDM], *up[PG_SPACEDM];

    ok = 0;

    for (id = 0; id < PG_SPACEDM; id++)
        {lw[id] = &xl[id];
	 up[id] = &xu[id];};

    PG_get_viewspace(dev, WORLDC, wc);

    for (j = 0; j < N; j++)

/* compare WORLDC to NORMC */
        {world_box_3(nd, xl, xu, b, wc);

	 PG_trans_points(dev, 1, nd, WORLDC, lw, NORMC, lw);
	 PG_trans_points(dev, 1, nd, WORLDC, up, NORMC, up);
	 PG_trans_box(dev, nd, WORLDC, b, NORMC, b);

	 ok += compare_3(dev, nd, xl, xu, b);

/* compare NORMC to WORLDC */
	 norm_box_3(nd, xl, xu, b);

	 PG_trans_points(dev, 1, nd, NORMC, lw, WORLDC, lw);
	 PG_trans_points(dev, 1, nd, NORMC, up, WORLDC, up);
	 PG_trans_box(dev, nd, NORMC, b, WORLDC, b);

	 ok += compare_3(dev, nd, xl, xu, b);

/* compare NORMC to PIXELC */
	 norm_box_3(nd, xl, xu, b);

	 PG_trans_points(dev, 1, nd, NORMC, lw, PIXELC, lw);
	 PG_trans_points(dev, 1, nd, NORMC, up, PIXELC, up);
	 PG_trans_box(dev, nd, NORMC, b, PIXELC, b);

/*	 ok += compare_3(dev, nd, xl, xu, b); */

/* compare PIXELC to NORMC */
	 pixel_box_3(nd, np, xl, xu, b);

	 PG_trans_points(dev, 1, nd, PIXELC, lw, NORMC, lw);
	 PG_trans_points(dev, 1, nd, PIXELC, up, NORMC, up);
	 PG_trans_box(dev, nd, PIXELC, b, NORMC, b);

/*	 ok += compare_3(dev, nd, xl, xu, b); */

/* compare WORLDC to PIXELC */
         world_box_3(nd, xl, xu, b, wc);

	 PG_trans_points(dev, 1, nd, WORLDC, lw, PIXELC, lw);
	 PG_trans_points(dev, 1, nd, WORLDC, up, PIXELC, up);
	 PG_trans_box(dev, nd, WORLDC, b, PIXELC, b);

	 ok += compare_3(dev, nd, xl, xu, b);

/* compare PIXELC to WORLDC */
	 pixel_box_3(nd, np, xl, xu, b);

	 PG_trans_points(dev, 1, nd, PIXELC, lw, WORLDC, lw);
	 PG_trans_points(dev, 1, nd, PIXELC, up, WORLDC, up);
	 PG_trans_box(dev, nd, PIXELC, b, WORLDC, b);

	 ok += compare_3(dev, nd, xl, xu, b);};

    rv = (4*N - ok);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_4 - test reversibility of WC to NDC */

static int test_4(PG_device *dev, int nd, int np, int icc)
   {int j, id, ok, rv;
    double xo[PG_SPACEDM], xn[PG_SPACEDM];
    double bo[PG_BOXSZ], bn[PG_BOXSZ], wc[PG_BOXSZ];
    double *po[PG_SPACEDM], *pn[PG_SPACEDM];

    ok = 0;

    for (id = 0; id < PG_SPACEDM; id++)
        {po[id] = &xo[id];
	 pn[id] = &xn[id];};

    PG_get_viewspace(dev, WORLDC, wc);

    for (j = 0; j < N; j++)

/* compare WORLDC to NORMC */
        {world_box_rev(nd, xo, bo, wc);

	 PG_trans_points(dev, 1, nd, WORLDC, po, NORMC, pn);
	 PG_trans_points(dev, 1, nd, NORMC, pn, WORLDC, pn);

	 PG_trans_box(dev, nd, WORLDC, bo, NORMC, bn);
	 PG_trans_box(dev, nd, NORMC, bn, WORLDC, bn);

	 ok += compare_rev(dev, "Test #4 WC->NDC", nd,
			   xo, xn, bo, bn, TOLERANCE);

/* compare NORMC to WORLDC */
         norm_box_rev(nd, xo, bo);

	 PG_trans_points(dev, 1, nd, NORMC, po, WORLDC, pn);
	 PG_trans_points(dev, 1, nd, WORLDC, pn, NORMC, pn);

	 PG_trans_box(dev, nd, NORMC, bo, WORLDC, bn);
	 PG_trans_box(dev, nd, WORLDC, bn, NORMC, bn);

	 ok += compare_rev(dev, "Test #4 NDC->WC", nd,
			   xo, xn, bo, bn, TOLERANCE);};

    rv = (2*N - ok);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_5 - test reversibility of WC to PC */

static int test_5(PG_device *dev, int nd, int np, int icc)
   {int j, id, ok, rv;
    double xo[PG_SPACEDM], xn[PG_SPACEDM];
    double bo[PG_BOXSZ], bn[PG_BOXSZ], wc[PG_BOXSZ];
    double *po[PG_SPACEDM], *pn[PG_SPACEDM];

    ok = 0;

    for (id = 0; id < PG_SPACEDM; id++)
        {po[id] = &xo[id];
	 pn[id] = &xn[id];};

    PG_get_viewspace(dev, WORLDC, wc);

    for (j = 0; j < N; j++)

/* compare WORLDC to PIXELC */
        {world_box_rev(nd, xo, bo, wc);

	 PG_trans_points(dev, 1, nd, WORLDC, po, PIXELC, pn);
	 PG_trans_box(dev, nd, WORLDC, bo, PIXELC, bn);

	 PG_trans_points(dev, 1, nd, PIXELC, pn, WORLDC, pn);
	 PG_trans_box(dev, nd, PIXELC, bn, WORLDC, bn);

/*	 ok += compare_rev(dev, "Test #5 WC->PC", nd,
	                   xo, xn, bo, bn, 5.0e-3); */

/* compare PIXELC to WORLDC */
         pixel_box_rev(nd, xo, bo, np);

	 PG_trans_points(dev, 1, nd, PIXELC, po, WORLDC, pn);
	 PG_trans_box(dev, nd, PIXELC, bo, WORLDC, bn);

	 PG_trans_points(dev, 1, nd, WORLDC, pn, PIXELC, pn);
	 PG_trans_box(dev, nd, WORLDC, bn, PIXELC, bn);

	 ok += compare_rev(dev, "Test #5 PC->WC", nd,
			   xo, xn, bo, bn, TOLERANCE);};

    rv = (N - ok);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_6 - test reversibility of NDC to PC */

static int test_6(PG_device *dev, int nd, int np, int icc)
   {int j, id, ok;
    double xo[PG_SPACEDM], xn[PG_SPACEDM];
    double bo[PG_BOXSZ], bn[PG_BOXSZ];
    double *po[PG_SPACEDM], *pn[PG_SPACEDM];

    ok = 0;

    for (id = 0; id < PG_SPACEDM; id++)
        {po[id] = &xo[id];
	 pn[id] = &xn[id];};

    for (j = 0; j < N; j++)

/* compare NORMC to PIXELC */
        {norm_box_rev(nd, xo, bo);

	 PG_trans_points(dev, 1, nd, NORMC, po, PIXELC, pn);
	 PG_trans_box(dev, nd, NORMC, bo, PIXELC, bn);

	 PG_trans_points(dev, 1, nd, PIXELC, pn, NORMC, pn);
	 PG_trans_box(dev, nd, PIXELC, bn, NORMC, bn);

/*	 ok += compare_rev(dev, "Test #6 NDC->PC", nd,
	                   xo, xn, bo, bn, 5.0e-3); */

/* compare PIXELC to NORMC */
	 pixel_box_rev(nd, xo, bo, np);

	 PG_trans_points(dev, 1, nd, PIXELC, po, NORMC, pn);
	 PG_trans_box(dev, nd, PIXELC, bo, NORMC, bn);

	 PG_trans_points(dev, 1, nd, NORMC, pn, PIXELC, pn);
	 PG_trans_box(dev, nd, NORMC, bn, PIXELC, bn);

	 ok += compare_rev(dev, "Test #6 PC->NDC", nd,
			   xo, xn, bo, bn, TOLERANCE);};

    ok = (N - ok);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_7 - test that coordinate transform functions
 *        - do not change args incorrectly
 */

static int test_7(PG_device *dev, int nd, int np, int icc)
   {int ok;
    double ibx[PG_BOXSZ], rbx[PG_BOXSZ];

    ok = 0;
    nd = PG_SPACEDM;

    PG_box_init(nd, ibx, 0.0, 1.0);
    PG_box_copy(nd, rbx, ibx);

    PG_set_viewspace(dev, nd, FRAMEC, ibx);
    ok += compare_box(dev, "Test #7 FRAMEC", nd, ibx, rbx, TOLERANCE);

    PG_set_viewspace(dev, nd, BOUNDC, ibx);
    ok += compare_box(dev, "Test #7 BOUNDC", nd, ibx, rbx, TOLERANCE);

    PG_set_viewspace(dev, nd, WORLDC, ibx);
    ok += compare_box(dev, "Test #7 WORLDC", nd, ibx, rbx, TOLERANCE);

    PG_set_viewspace(dev, nd, NORMC, ibx);
    ok += compare_box(dev, "Test #7 NORMC", nd, ibx, rbx, TOLERANCE);

    PG_set_viewspace(dev, nd, PIXELC, ibx);
    ok += compare_box(dev, "Test #7 PIXELC", nd, ibx, rbx, TOLERANCE);

    ok = (5 - ok);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RUN_TEST - run the sequence of tests for the specified device */

static int run_test(char *name, PG_device *dev, int nt, gtest *tst)
   {int i, j, nd, np, err, nerr;
    double nl[PG_SPACEDM], wl[PG_SPACEDM];
    double ndc[PG_BOXSZ], wc[PG_BOXSZ];
    gtest t;

    err = 0;

    np = max(PG_window_width(dev), PG_window_height(dev));

    ONCE_SAFE(TRUE, NULL)
       printf("        Test        ");
       printf("#1    #2    #3    #4    #5    #6    #7");
       printf("  Errs    Leaked\n");
    END_SAFE;

    printf("        %-8s", name);

    for (i = 0; i < nt; i++)
        {t = tst[i];
	 if (t != NULL)
	    {nerr = 0;
	     for (nd = 1; nd < PG_SPACEDM; nd++)
	         {for (j = 0; j < N; j++)

/* pick random viewport */
		      {nl[0] = 0.01*SC_random_int(1, 40);
		       nl[1] = 0.01*SC_random_int(60, 100);
		       if (nl[0] > nl[1])
			  {SC_SWAP_VALUE(double, nl[0], nl[1]);};

/* pick random coordinate system */
		       wl[0] = 1.0e2*PM_random(0.0);
		       wl[1] = 1.0e12*PM_random(0.0);
		       if (wl[0] > wl[1])
			  {SC_SWAP_VALUE(double, wl[0], wl[1]);};

		       PG_box_init(nd, ndc, nl[0], nl[1]);
		       PG_box_init(nd, wc,  wl[0], wl[1]);

		       PG_set_viewspace(dev, nd, NORMC, ndc);
		       PG_set_viewspace(dev, nd, WORLDC, wc);

		       nerr += (*t)(dev, nd, np, j);};};

	     printf(" %5d", nerr);

	     err += nerr;};};

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* TEST_CHECK - begin/end memory checks for each test */

static long test_check(char *name, long which, testdes *a, int st)
   {long bytaa, bytfa, rv;
    char msg[MAXLINE];
    static long bytab, bytfb;

    rv = 0;

    if (a->debug_mode)
       rv = SC_mem_monitor(which, 2, "G", msg);

    if (which == -1L)
       SC_mem_stats(&bytab, &bytfb, NULL, NULL);

    else if (which != -1L)
       {SC_mem_stats(&bytaa, &bytfa, NULL, NULL);

	bytaa -= bytab;
	bytfa -= bytfb;

	PRINT(STDOUT, " %5d %9d\n", st, bytaa - bytfa);};

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

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SCR_TEST - test the SCREEN device */

static int scr_test(char *name, testdes *a, int nt, gtest *tst)
   {int ok;
    long sc;
    PG_device *dev;

    sc = test_check(name, -1L, a, 0);

    register_devices();

    dev = PG_make_device("SCREEN", "COLOR", name);

    PG_open_device(dev, 0.01, 0.01, 0.3, 0.3);

    ok = run_test(name, dev, nt, tst);

    PG_close_device(dev);

    PG_rl_all();

    sc = test_check(name, 1L, a, ok);
    SC_ASSERT(sc == TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PS_TEST - test the PostScript device */

static int ps_test(char *name, testdes *a, int nt, gtest *tst)
   {int ok;
    long sc;
    PG_device *dev;

    sc = test_check(name, -1L, a, 0);

    register_devices();

    if (a->mode == PORTRAIT_MODE)
       dev = PG_make_device("PS", "COLOR", name);
    else
       dev = PG_make_device("PS", "COLOR LANDSCAPE", name);

    PG_open_device(dev, 0.0, 0.0, 0.0, 0.0);

    ok = run_test(name, dev, nt, tst);

    PG_close_device(dev);

    PG_rl_all();

    sc = test_check(name, 1L, a, ok);
    SC_ASSERT(sc == TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CGM_TEST - test the CGM device */

static int cgm_test(char *name, testdes *a, int nt, gtest *tst)
   {int ok;
    long sc;
    PG_device *dev;

    sc = test_check(name, -1L, a, 0);

    register_devices();

    dev = PG_make_device("CGM", "COLOR", name);

    PG_open_device(dev, 0.0, 0.0, 0.0, 0.0);

    ok = run_test(name, dev, nt, tst);

    PG_close_device(dev);

    PG_rl_all();

    sc = test_check(name, 1L, a, ok);
    SC_ASSERT(sc == TRUE);

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* JPEG_TEST - test the JPEG device */

static int jpeg_test(char *name, testdes *a, int nt, gtest *tst)
   {int ok;
    long sc;
    PG_device *dev;

    ok = FALSE;

    sc = test_check(name, -1L, a, 0);

    register_devices();

    dev = PG_make_device("JPEG", "COLOR", name);

    if (dev != NULL)
       {PG_open_device(dev, 0.0, 0.0, 400.0, 400.0);

	ok = run_test(name, dev, nt, tst);

	PG_close_device(dev);

	PG_rl_all();

	sc = test_check(name, 1L, a, ok);
	SC_ASSERT(sc == TRUE);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PNG_TEST - test the PNG device */

static int png_test(char *name, testdes *a, int nt, gtest *tst)
   {int ok;
    long sc;
    PG_device *dev;

    ok = FALSE;

    sc = test_check(name, -1L, a, 0);

    register_devices();

    dev = PG_make_device("PNG", "COLOR", name);

    if (dev != NULL)
       {PG_open_device(dev, 0.0, 0.0, 400.0, 400.0);

	ok = run_test(name, dev, nt, tst);

	PG_close_device(dev);

	PG_rl_all();

	sc = test_check(name, 1L, a, ok);
	SC_ASSERT(sc == TRUE);};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HELP - print the help message */

void help(void)
   {

    PRINT(STDOUT, "Usage: gsatst [-dhlABCDEF123456]\n");
    PRINT(STDOUT, "   -d  debug memory usage\n");
    PRINT(STDOUT, "   -h  this help message\n");
    PRINT(STDOUT, "   -l  landscape mode for PS\n");
    PRINT(STDOUT, "   -A  don't test SCREEN device\n");
    PRINT(STDOUT, "   -B  don't test PS device\n");
    PRINT(STDOUT, "   -C  don't test CGM device\n");
    PRINT(STDOUT, "   -D  don't test JPEG device\n");
    PRINT(STDOUT, "   -E  don't test PNG device\n");
    PRINT(STDOUT, "   -1  don't do test #1 (lowest level)\n");
    PRINT(STDOUT, "   -2  don't do test #2 (old and new)\n");
    PRINT(STDOUT, "   -3  don't do test #3 (box and point)\n");
    PRINT(STDOUT, "   -4  don't do test #4 (WC to NDC)\n");
    PRINT(STDOUT, "   -5  don't do test #5 (WC to PC)\n");
    PRINT(STDOUT, "   -6  don't do test #6 (NDC to PC)\n");
    PRINT(STDOUT, "   -7  don't do test #7 (Unperturbed args)\n");
    PRINT(STDOUT, "\n");

    return;}

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
   {int i, in, nd, nt, rv;
    char *tok, *n, tc;
    char *name[N_DEV];
    dtest d;
    gtest tst[N_TEST];
    dtest dev[N_DEV];
    testdes a;

    SC_setup_sig_handlers(sig_handler, NULL, TRUE);

/* let's hear it for non-ANSI compilers */
    tst[0]  = test_1;
    tst[1]  = test_2;
    tst[2]  = test_3;
    tst[3]  = test_4;
    tst[4]  = test_5;
    tst[5]  = test_6;
    tst[6]  = test_7;

    dev[0] = scr_test;
    dev[1] = ps_test;
    dev[2] = cgm_test;
    dev[3] = jpeg_test;
    dev[4] = png_test;

    name[0] = "trscr";
    name[1] = "trps";
    name[2] = "trcgm";
    name[3] = "trjpg";
    name[4] = "trpng";

    SC_zero_space_n(0, -2);

    nt = N_TEST;
    nd = N_DEV;

    a.mode       = PORTRAIT_MODE;
    a.debug_mode = FALSE;

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
			 name[0] = "gsscrta";
			 name[1] = "gspsta";
			 name[2] = "gscgmta";
			 name[3] = "gsjpgta";
			 name[4] = "gspngta";
			 break;
		    case 'd' :
		         a.debug_mode = TRUE;
			 break;
		    case 'l' :
		         a.mode = LANDSCAPE_MODE;
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
			 return(1);
			 break;};};};

    unlink(lgf);

    rv = TRUE;
    for (i = 0; i < nd; i++)
        {d = dev[i];
	 n = name[i];
	 if (d != NULL)
	    rv &= ((*d)(n, &a, nt, tst) == 0);};

    rv = (rv != TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
