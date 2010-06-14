/*
 * GSAXIS.C - axis routines for Portable Graphics System
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_GRID_LINES - draw in grid lines */

static void _PG_draw_grid_lines(PG_device *dev, PG_axis_def *ad,
                                double *x1n, double *x1x,
                                double *x2n, double *x2x,
                                int logsp)
   {int n;
    double *dx;
    PG_axis_tick_def *td;

    td = ad->tick + (AXIS_TICK_MAJOR >> 1);
    n  = td->n;
    dx = td->dx;

    PG_set_line_style(dev, _PG_gattrs.axis_grid_style);

    PG_draw_multiple_line(dev, n, x1n, x1x, x2n, x2x, dx);

    PG_set_line_style(dev, _PG_gattrs.axis_line_style);

    _PG_rl_axis_def(ad);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_POLAR_GRID_LINES - draw in a set of polar grid lines */

static void _PG_draw_polar_grid_lines(PG_device *dev, PG_axis_def *ad,
                                      double *bx)
   {int i, n, na, nr;
    double fr, en[2], tpi;
    double dr, rc, r1, r2, r3, r4;
    double a1, a2, amx, aincr;
    double r[PG_SPACEDM];
    PG_axis_tick_def *td;

    tpi = 2.0*PI;

    td = ad->tick + (AXIS_TICK_MAJOR >> 1);
    n  = td->n;
    en[0] = td->en[0];
    en[1] = td->en[1];

    fr    = max(ABS(bx[2]), ABS(bx[3]))/(bx[3] - bx[2]);
    nr    = fr*n;
    fr    = max(ABS(bx[0]), ABS(bx[1]))/(bx[1] - bx[0]);
    nr    = max(nr, fr*n);
    na    = 32;
    aincr = tpi/na;
    amx   = na*aincr;
    a1    = 0.0;
    a2    = na*aincr;
    nr    = max(nr, fr*n);
    r1    = HYPOT(bx[2], bx[0]);
    r2    = HYPOT(bx[2], bx[1]);
    r3    = HYPOT(bx[3], bx[1]);
    r4    = HYPOT(bx[3], bx[0]);
    r[0]  = min(r1, r2);
    r[0]  = min(r[0], r3);
    r[0]  = min(r[0], r4);
    r[1]  = max(r1, r2);
    r[1]  = max(r[1], r3);
    r[1]  = max(r[1], r4);

    PG_set_line_style(dev, _PG_gattrs.axis_grid_style);
    PG_set_clipping(dev, TRUE);

    dr = (en[1] - en[0])/((double) n - 1.0);
    for (i = 1; i <= nr; i++)
        {rc = i*dr;

         if (rc < r[0])
            {a1 = 0.0;
             a2 = tpi;}
         else
            {a1 = atan2(bx[2],  ABS(bx[0]));
             a2 = tpi + atan2(bx[2], -ABS(bx[0]));};

         PG_draw_arc(dev, rc, a1, a2, 0.0, 0.0, RADIAN);};

    for (i = 0, a1 = 0.0; a1 <= (amx*(1.0 - TOLERANCE)); a1 += aincr, i++)
        {if ((i % 8) != 0)
            PG_draw_rad(dev, 0.0, r[1], a1, 0.0, 0.0, RADIAN);};

    PG_set_clipping(dev, FALSE);
    PG_set_line_style(dev, _PG_gattrs.axis_line_style);

    _PG_rl_axis_def(ad);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_CARTESIAN_AXIS - do work of drawing a set of Cartesian axes */

static void _PG_draw_cartesian_axes(PG_device *dev, int labl)
   {int nt, troa[2], lroa[2];
    double majoren[2], wext[2], tn[2], vw[2];
    double bnd[PG_BOXSZ];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    double x3[PG_SPACEDM], x4[PG_SPACEDM];
    PG_axis_def *ad;
    PG_axis_tick_def *td;
    PG_dev_geometry *g;

    g = &dev->g;

/* get window and viewport limits */
    PG_get_viewspace(dev, BOUNDC, bnd);

    wext[0] = bnd[1] - bnd[0];
    wext[1] = bnd[3] - bnd[2];

/* if the coordinate system has x increasing to the right */
    if (bnd[1] >= bnd[0])
       {lroa[0] = AXIS_TICK_RIGHT;
	troa[0] = (_PG_gattrs.axis_tick_type == AXIS_TICK_RIGHT) ?
	          AXIS_TICK_RIGHT : AXIS_TICK_LEFT;}

/* otherwise use the left hand rule for ticks */
    else
       {lroa[0] = AXIS_TICK_LEFT;
	troa[0] = (_PG_gattrs.axis_tick_type == AXIS_TICK_RIGHT) ?
	          AXIS_TICK_LEFT : AXIS_TICK_RIGHT;};

/* if the coordinate system has y increasing up */
    if (bnd[3] >= bnd[2])
       {lroa[1] = AXIS_TICK_RIGHT;
	troa[1] = (_PG_gattrs.axis_tick_type == AXIS_TICK_RIGHT) ?
	          AXIS_TICK_RIGHT : AXIS_TICK_LEFT;}

/* otherwise use the left hand rule for ticks */
    else
       {lroa[1] = AXIS_TICK_LEFT;
	troa[1] = (_PG_gattrs.axis_tick_type == AXIS_TICK_RIGHT) ?
	          AXIS_TICK_LEFT : AXIS_TICK_RIGHT;};

    if (_PG_gattrs.axis_tick_type == AXIS_TICK_STRADDLE)
       {troa[0] = AXIS_TICK_STRADDLE;
        troa[1] = AXIS_TICK_STRADDLE;};

    nt = _PG_gattrs.axis_n_minor_ticks;
            
/* if grid desired draw axes with minor ticks, save major tick data, then
 * draw the grid
 */
    if (_PG_gattrs.axis_grid_on == TRUE)
       {_PG_gattrs.axis_n_minor_ticks = (_PG_gattrs.axis_n_minor_x_ticks >= 0) ?
	                                _PG_gattrs.axis_n_minor_x_ticks :
					nt;

/* lower X axis */
	xl[0] = bnd[0];
	xl[1] = bnd[2];
	xr[0] = bnd[1];
	xr[1] = bnd[2];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = bnd[0];
	vw[1] = bnd[1];
	ad = PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
			    _PG_gattrs.axis_label_x_format,
			    troa[0], lroa[0], TRUE,
			    AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);
	if (ad == NULL)
	   return;

	td = ad->tick + (AXIS_TICK_MAJOR >> 1);

	if (bnd[0] < bnd[1])
	   {majoren[0] = td->en[0] + bnd[0];
	    majoren[1] = td->en[1] + bnd[0];}
	else
	   {majoren[1] = -td->en[0] + bnd[0];
	    majoren[0] = -td->en[1] + bnd[0];};

	x1[0] = majoren[0];
	x1[1] = bnd[2];
	x2[0] = majoren[0];
	x2[1] = bnd[3];
	x3[0] = majoren[1];
	x3[1] = bnd[2];
	x4[0] = majoren[1];
	x4[1] = bnd[3];
	_PG_draw_grid_lines(dev, ad, x1, x2, x3, x4, g->iflog[0]);

/* upper X axis */
	xl[0] = bnd[1];
	xl[1] = bnd[3];
	xr[0] = bnd[0];
	xr[1] = bnd[3];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = bnd[1];
	vw[1] = bnd[0];
	PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
		       _PG_gattrs.axis_label_x_format,
		       troa[0], AXIS_TICK_NONE, FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR, 0);

	_PG_gattrs.axis_n_minor_ticks = (_PG_gattrs.axis_n_minor_y_ticks >= 0) ?
	                                _PG_gattrs.axis_n_minor_y_ticks :
	                                nt;

/* left Y axis */
	xl[0] = bnd[0];
	xl[1] = bnd[3];
	xr[0] = bnd[0];
	xr[1] = bnd[2];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = bnd[3];
	vw[1] = bnd[2];
	PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
		       _PG_gattrs.axis_label_y_format,
		       troa[1], lroa[1], FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);

/* right Y axis */
	xl[0] = bnd[1];
	xl[1] = bnd[2];
	xr[0] = bnd[1];
	xr[1] = bnd[3];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = bnd[2];
	vw[1] = bnd[3];
	ad = PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
			    _PG_gattrs.axis_label_y_format,
			    troa[1], AXIS_TICK_NONE, TRUE,
			    AXIS_TICK_MAJOR | AXIS_TICK_MINOR, 0);
	if (ad == NULL)
	   return;

	td = ad->tick + (AXIS_TICK_MAJOR >> 1);

	if (bnd[2] < bnd[3])
	   {majoren[0] = td->en[0] + bnd[2];
	    majoren[1] = td->en[1] + bnd[2];}
	else
	   {majoren[1] = -td->en[0] + bnd[2];
	    majoren[0] = -td->en[1] + bnd[2];};

	x1[0] = bnd[0];
	x1[1] = majoren[0];
	x2[0] = bnd[1];
	x2[1] = majoren[0];
	x3[0] = bnd[0];
	x3[1] = majoren[1];
	x4[0] = bnd[1];
	x4[1] = majoren[1];
	_PG_draw_grid_lines(dev, ad, x1, x2, x3, x4, g->iflog[1]);}

/* without grid simply draw the axes */
    else
       {_PG_gattrs.axis_n_minor_ticks = (_PG_gattrs.axis_n_minor_x_ticks >= 0) ?
	                                _PG_gattrs.axis_n_minor_x_ticks :
	                                nt;

/* lower X axis */
	xl[0] = bnd[0];
	xl[1] = bnd[2];
	xr[0] = bnd[1];
	xr[1] = bnd[2];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = bnd[0];
	vw[1] = bnd[1];
	PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
		       _PG_gattrs.axis_label_x_format,
		       troa[0], lroa[0], FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);

/* upper X axis */
	xl[0] = bnd[1];
	xl[1] = bnd[3];
	xr[0] = bnd[0];
	xr[1] = bnd[3];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = bnd[1];
	vw[1] = bnd[0];
	PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
		       _PG_gattrs.axis_label_x_format,
		       troa[0], AXIS_TICK_NONE, FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR, 0);

	_PG_gattrs.axis_n_minor_ticks = (_PG_gattrs.axis_n_minor_y_ticks >= 0) ?
	                                _PG_gattrs.axis_n_minor_y_ticks :
	                                nt;

/* left Y axis */
	xl[0] = bnd[0];
	xl[1] = bnd[3];
	xr[0] = bnd[0];
	xr[1] = bnd[2];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = bnd[3];
	vw[1] = bnd[2];
	PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
		       _PG_gattrs.axis_label_y_format,
		       troa[1], lroa[1], FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);

/* right Y axis */
	xl[0] = bnd[1];
	xl[1] = bnd[2];
	xr[0] = bnd[1];
	xr[1] = bnd[3];
	tn[0] = 0.0;
	tn[1] = 1.0;
	vw[0] = bnd[2];
	vw[1] = bnd[3];
	PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
		       _PG_gattrs.axis_label_y_format,
		       troa[1], AXIS_TICK_NONE, FALSE,
		       AXIS_TICK_MAJOR | AXIS_TICK_MINOR, 0);};

    _PG_gattrs.axis_n_minor_ticks = nt;
            
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_INSEL_AXIS - do work of drawing a set of Inselberg axes */

static void _PG_draw_insel_axes(PG_device *dev, int labl)
   {int ndim;
    double xsep, xp1, xp2, scale;
    double tn[2], vw[2], wext[2];
    double bnd[PG_BOXSZ];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];

/* get window and viewport limits */
    PG_get_viewspace(dev, BOUNDC, bnd);

    wext[0] = bnd[1] - bnd[0];
    wext[1] = bnd[3] - bnd[2];

    ndim  = 2;
    xsep  = wext[0]/((double) (++ndim));
    xp1   = bnd[0] + xsep;
    xp2   = bnd[0] + 2.0*xsep;
    scale = wext[1]/wext[0];

/* draw the axis lines */
    xl[0] = xp1;
    xl[1] = bnd[2];
    xr[0] = xp1;
    xr[1] = bnd[3];
    tn[0] = 0.0;
    tn[1] = 1.0;
    vw[0] = bnd[0];
    vw[1] = bnd[1];
    PG_draw_axis_n(dev, xl, xr, tn, vw, scale,
		   _PG_gattrs.axis_label_x_format,
		   AXIS_TICK_STRADDLE, AXIS_TICK_ENDS, FALSE,
		   AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);

    xl[0] = xp2;
    xl[1] = bnd[2];
    xr[0] = xp2;
    xr[1] = bnd[3];
    tn[0] = 0.0;
    tn[1] = 1.0;
    vw[0] = bnd[2];
    vw[1] = bnd[3];
    PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
		   _PG_gattrs.axis_label_y_format,
		   AXIS_TICK_STRADDLE, AXIS_TICK_ENDS, FALSE,
		   AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_POLAR_AXIS - do work of drawing a set of polar axes */

static void _PG_draw_polar_axes(PG_device *dev, int labl)
   {double tn[2], vw[2];
    double bnd[PG_BOXSZ];
    double xl[PG_SPACEDM], xr[PG_SPACEDM];
    PG_axis_def *ad;

/* get window and viewport limits */
    PG_get_viewspace(dev, BOUNDC, bnd);

/* with grid */
    if (_PG_gattrs.axis_grid_on == TRUE)

/* if the y = 0 line is in the range draw the axis at y = 0 */
       {if ((bnd[2] <= 0.0) && (0.0 <= bnd[3]))
	   {xl[0] = bnd[0];
	    xl[1] = 0.0;
	    xr[0] = bnd[1];
	    xr[1] = 0.0;
	    tn[0] = 0.0;
	    tn[1] = 1.0;
	    vw[0] = bnd[0];
	    vw[1] = bnd[1];
	    PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
			   _PG_gattrs.axis_label_x_format,
			   AXIS_TICK_STRADDLE, AXIS_TICK_ENDS, FALSE,
			   AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);}

/* otherwise draw it at the bottom edge */
        else
	   {xl[0] = bnd[0];
	    xl[1] = bnd[2];
	    xr[0] = bnd[1];
	    xr[1] = bnd[2];
	    tn[0] = 0.0;
	    tn[1] = 1.0;
	    vw[0] = bnd[0];
	    vw[1] = bnd[1];
	    PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
			   _PG_gattrs.axis_label_x_format,
			   AXIS_TICK_STRADDLE, AXIS_TICK_ENDS, FALSE,
			   AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);};

/* if the x = 0 line is in the domain draw the axis at x = 0 */
	if ((bnd[0] <= 0.0) && (0.0 <= bnd[1]))
	   {xl[0] = 0.0;
	    xl[1] = bnd[2];
	    xr[0] = 0.0;
	    xr[1] = bnd[3];
	    tn[0] = 0.0;
	    tn[1] = 1.0;
	    vw[0] = bnd[2];
	    vw[1] = bnd[3];
	    ad = PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
				_PG_gattrs.axis_label_y_format,
				AXIS_TICK_STRADDLE, AXIS_TICK_ENDS, TRUE,
				AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);}

/* otherwise draw it at the left edge */
	else
	   {xl[0] = bnd[0];
	    xl[1] = bnd[2];
	    xr[0] = bnd[0];
	    xr[1] = bnd[3];
	    tn[0] = 0.0;
	    tn[1] = 1.0;
	    vw[0] = bnd[2];
	    vw[1] = bnd[3];
	    ad = PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
				_PG_gattrs.axis_label_y_format,
				AXIS_TICK_STRADDLE, AXIS_TICK_ENDS, TRUE,
				AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);};

	if (ad == NULL)
	   return;

/* draw the grid */
	_PG_draw_polar_grid_lines(dev, ad, bnd);}

/* no grid */
    else

/* if the y = 0 line is in the range draw the axis at y = 0 */
       {if ((bnd[2] <= 0.0) && (0.0 <= bnd[3]))
	   {xl[0] = bnd[0];
	    xl[1] = 0.0;
	    xr[0] = bnd[1];
	    xr[1] = 0.0;
	    tn[0] = 0.0;
	    tn[1] = 1.0;
	    vw[0] = bnd[0];
	    vw[1] = bnd[1];
	    PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
			   _PG_gattrs.axis_label_x_format,
			   AXIS_TICK_STRADDLE, AXIS_TICK_ENDS, FALSE,
			   AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);}

/* otherwise draw it at the bottom edge */
        else
	   {xl[0] = bnd[0];
	    xl[1] = bnd[2];
	    xr[0] = bnd[1];
	    xr[1] = bnd[2];
	    tn[0] = 0.0;
	    tn[1] = 1.0;
	    vw[0] = bnd[0];
	    vw[1] = bnd[1];
	    PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
			   _PG_gattrs.axis_label_x_format,
			   AXIS_TICK_STRADDLE, AXIS_TICK_ENDS, FALSE,
			   AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);};

/* if the x = 0 line is in the domain draw the axis at x = 0 */
	if ((bnd[0] <= 0.0) && (0.0 <= bnd[1]))
	   {xl[0] = 0.0;
	    xl[1] = bnd[2];
	    xr[0] = 0.0;
	    xr[1] = bnd[3];
	    tn[0] = 0.0;
	    tn[1] = 1.0;
	    vw[0] = bnd[2];
	    vw[1] = bnd[3];
	    PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
			   _PG_gattrs.axis_label_y_format,
			   AXIS_TICK_STRADDLE, AXIS_TICK_ENDS, FALSE,
			   AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);}

/* otherwise draw it at the left edge */
	else
	   {xl[0] = bnd[0];
	    xl[1] = bnd[2];
	    xr[0] = bnd[0];
	    xr[1] = bnd[3];
	    tn[0] = 0.0;
	    tn[1] = 1.0;
	    vw[0] = bnd[2];
	    vw[1] = bnd[3];
	    PG_draw_axis_n(dev, xl, xr, tn, vw, 1.0,
			   _PG_gattrs.axis_label_y_format,
			   AXIS_TICK_STRADDLE, AXIS_TICK_ENDS, FALSE,
			   AXIS_TICK_MAJOR | AXIS_TICK_MINOR | labl, 0);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_AUX_AXIS - draw a set of axes for a rectangular plot complete with
 *              - labels, ticks, and/or grid lines.  The 
 *              - starting values and interval for the labels and ticks are 
 *              - determined by this routine.
 */

static void _PG_aux_axis(PG_device *dev, int axis_type)
   {int labl;

    _PG_gattrs.axis_type = axis_type;

    labl = (_PG_gattrs.axis_labels) ? AXIS_TICK_LABEL : AXIS_TICK_NONE;
            
/* draw a set of rectangular axes */
    if (axis_type == CARTESIAN_2D)
       _PG_draw_cartesian_axes(dev, labl);

/* draw a set of Inselberg axes */
    else if (axis_type == INSEL)
       _PG_draw_insel_axes(dev, labl);

/* draw a set of polar axes */
    else if (axis_type == POLAR)
       _PG_draw_polar_axes(dev, labl);
            
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* PG_AXIS - draw the axes
 *         - the information for this is set up by a setlimits call
 */
 
void PG_axis(PG_device *dev, int axis_type)
   {PG_dev_attributes *attr;

    if (_PG_gattrs.axis_on == FALSE)
       return;

/* save user's values for various attributes */
    attr = PG_get_attributes(dev);

    switch (axis_type)
       {case INSEL :
	     PG_set_axis_attributes(dev,
				    0);
	     _PG_aux_axis(dev, INSEL);
	     break;

        case POLAR :
	     if (dev->grid == ON)          /* dotted grid lines */
	        {PG_set_axis_attributes(dev,
					AXIS_GRID_ON, TRUE,
					0);
		 _PG_aux_axis(dev, POLAR);}
	     else
	        {PG_set_axis_attributes(dev,
					0);
		 _PG_aux_axis(dev, POLAR);};
	     break;

        case CARTESIAN_2D :
	     if (dev->grid == ON)          /* dotted grid lines */
	        PG_set_axis_attributes(dev,
				       AXIS_GRID_ON, TRUE,
				       0);
	     else
	        PG_set_axis_attributes(dev,
				       0);
	     _PG_aux_axis(dev, CARTESIAN_2D);
	     break;

        default :
	     break;};

/* reset user's values for various attributes */
    PG_set_attributes(dev, attr);
    SFREE(attr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_SET_AXIS_ATTRIBUTES - set the parameters which control the look
 *                        - of the axes being drawn
 *                        -
 *                        - the control parameters are:
 *                        -
 *                        - AXIS_LINESTYLE    - the style of the lines
 *                        -                     (see PG_set_line_style)
 *                        - AXIS_LINETHICK    - the thickness of the lines
 *                        -                     (see PG_set_line_width)
 *                        - AXIS_LINECOLOR    - the color of the lines
 *                        -                     (see PG_set_color_line)
 *                        - AXIS_LABELCOLOR   - the color of the labels
 *                        -                     (see PG_set_color_text)
 *                        - AXIS_LABELFONT    - the label font type_face
 *                        -                     (see PG_set_font)
 *                        - AXIS_LABELPREC    - the character precision
 *                        -                     (see PG_set_char_precision)
 *                        - AXIS_X_FORMAT     - the format of the x labels
 *                        -                     (e.g. %10.2f)
 *                        - AXIS_Y_FORMAT     - the format of the y labels
 *                        -                     (e.g. %10.2f)
 *                        - AXIS_TICKSIZE     - set the tick size in
 *                        -                     fraction of axis length
 *                        - AXIS_GRID_ON      - turn on grid iff TRUE
 *                        - AXIS_CHAR_ANGLE   - orientation angle for label
 *                        -                     characters
 */

int PG_set_axis_attributes(PG_device *dev, ...)
   {int type, linecolor, txtcolor, prec;
    double charspace, chup[2], chpth[2];

/* load default values for external variables */
    if (dev != NULL)
       {linecolor = dev->WHITE;
	txtcolor  = dev->WHITE;}
    else
       {linecolor = 4;
	txtcolor  = 4;};

    prec = TEXT_CHARACTER_PRECISION;

    if (_PG_gattrs.axis_label_x_format == NULL)
       _PG_gattrs.axis_label_x_format = SC_strsavef("%10.2g",
						    "PERM|char*:PG_SET_AXIS_ATTRIBUTES:xlab_format");

    if (_PG_gattrs.axis_label_y_format == NULL)
       _PG_gattrs.axis_label_y_format = SC_strsavef("%10.2g",
						    "PERM|char*:PG_SET_AXIS_ATTRIBUTES:ylab_format");

    if (_PG_gattrs.axis_type_face == NULL)
       _PG_gattrs.axis_type_face = SC_strsavef("helvetica",
					       "PERM|char*:PG_SET_AXIS_ATTRIBUTES:type_face");

    charspace = 0.0;
    chpth[0]  = 1.0;
    chpth[1]  = 0.0;
    chup[0]   = 0.0;
    chup[1]   = 1.0;

    _PG_gattrs.axis_grid_on = FALSE;

    SC_VA_START(dev);

/* the type of the second part of the pair is dependent on the
 * value of type 
 */
    while ((type = SC_VA_ARG(int)) != 0)
       {switch (type)
           {case AXIS_LINESTYLE :
	         _PG_gattrs.axis_grid_style = SC_VA_ARG(int);
		 break;

            case AXIS_LINETHICK :
	         _PG_gattrs.axis_line_width = SC_VA_ARG(double);
		 break;

            case AXIS_LINECOLOR :
	         linecolor = SC_VA_ARG(int);
		 break;

            case AXIS_LABELCOLOR :
	         txtcolor = SC_VA_ARG(int);
		 break;

            case AXIS_LABELSIZE :
	         _PG_gattrs.axis_char_size = SC_VA_ARG(int);
		 break;

            case AXIS_CHAR_ANGLE :
	         _PG_gattrs.axis_char_angle = (double) SC_VA_ARG(double);
		 break;

            case AXIS_LABELPREC :
	         prec = SC_VA_ARG(int);
		 break;

            case AXIS_TICKSIZE :
	         _PG_gattrs.axis_major_tick_size = (double) SC_VA_ARG(double);
		 break;

            case AXIS_GRID_ON :
	         _PG_gattrs.axis_grid_on = SC_VA_ARG(int);
		 break;

            case AXIS_LABELFONT :
	         SFREE(_PG_gattrs.axis_type_face);
		 _PG_gattrs.axis_type_face = SC_strsavef(SC_VA_ARG(char *),
							 "char*:PG_SET_AXIS_ATTRIBUTES:labelfont");
		 break;

            case AXIS_X_FORMAT :
	         strcpy(_PG_gattrs.axis_label_x_format, SC_VA_ARG(char *));
		 break;

            case AXIS_Y_FORMAT :
	         strcpy(_PG_gattrs.axis_label_y_format, SC_VA_ARG(char *));
		 break;

            default :
	         return(FALSE);};};

    SC_VA_END;

    chpth[0] = cos(_PG_gattrs.axis_char_angle);
    chpth[1] = sin(_PG_gattrs.axis_char_angle);
    chup[0]  = -chpth[1];
    chup[1]  =  chpth[0];

/* set attribute values */
    PG_set_clipping(dev, FALSE);
    PG_set_color_text(dev, txtcolor, TRUE);
    PG_set_font(dev, _PG_gattrs.axis_type_face, dev->type_style, dev->type_size);
    PG_set_char_precision(dev, prec);
    PG_set_char_path(dev, chpth[0], chpth[1]);
    PG_set_char_up(dev, chup[0], chup[1]);
    PG_set_char_space(dev, charspace);
    PG_set_color_line(dev, linecolor, TRUE);
    PG_set_line_style(dev, _PG_gattrs.axis_line_style);
    PG_set_line_width(dev, _PG_gattrs.axis_line_width);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_TICK - draw the ticks
 *               -    ad   - the axis definition info
 *               -    sz   - the size of the ticks (PC)
 *               -    tick - major or minor ticks
 */

static int _PG_draw_tick(PG_device *dev, PG_axis_def *ad, double sz, int tick)
   {int n;
    double en[2], csa, sna, log_scale;
    double x1[PG_SPACEDM], x2[PG_SPACEDM], x3[PG_SPACEDM], x4[PG_SPACEDM];
    double xa[PG_SPACEDM], xb[PG_SPACEDM], xs[PG_SPACEDM], sx[PG_SPACEDM];
    double fdx[PG_SPACEDM];
    double *dx;
    PG_axis_tick_def *td;
    PG_dev_geometry *g;

    if ((dev == NULL) || (ad == NULL))
       return(FALSE);

    g = &dev->g;

    td = ad->tick + (tick >> 1);
    n  = td->n;
    dx = td->dx;
    en[0] = td->en[0];
    en[1] = td->en[1];

    xs[0] = ad->x0[0];
    xs[1] = ad->x0[1];
    sx[0] = ad->scale_x[0];
    sx[1] = ad->scale_x[1];
    csa = ad->cosa;
    sna = ad->sina;

/* compute the actual point on the axis of the first and last tick */
    xa[0] = xs[0] + en[0]*csa*sx[0];
    xa[1] = xs[1] + en[0]*sna*sx[1];
    xb[0] = xs[0] + en[1]*csa*sx[0];
    xb[1] = xs[1] + en[1]*sna*sx[1];

    log_scale = _PG_axis_place(dev, fdx, ad, sz, tick);

/* depending on the tick type compute the actual starting and ending points
 * for the first and last ticks
 */
    switch (ad->tick_type)
       {case AXIS_TICK_LEFT :
	     x2[0] = xa[0];
	     x2[1] = xa[1];
	     x4[0] = xb[0];
	     x4[1] = xb[1];
	     if (g->iflog[0])
	        {if (((ABS(xs[0]) - ABS(en[1])) == 0.0) && (xb[0] == 0.0))
		    x4[0] = x2[0]*POW(10.0, -_PG_gattrs.axis_n_decades);
		 x1[0] = x2[0]/fdx[0];
		 x3[0] = x4[0]/fdx[0];}

	     else
	        {x1[0] = x2[0] - fdx[0];
		 x3[0] = x4[0] - fdx[0];};

	     if (g->iflog[1])
	        {if (((ABS(xs[1]) - ABS(en[1])) == 0.0) && (xb[1] == 0.0))
		    x4[1] = x2[1]*POW(10.0, -_PG_gattrs.axis_n_decades);
		 x1[1] = x2[1]*fdx[1];
		 x3[1] = x4[1]*fdx[1];}

	     else
	        {x1[1] = x2[1] + fdx[1];
		 x3[1] = x4[1] + fdx[1];};

	     break;

        case AXIS_TICK_RIGHT :
	     x1[0] = xa[0];
	     x1[1] = xa[1];
	     x3[0] = xb[0];
	     x3[1] = xb[1];
	     if (g->iflog[0])
	        {if (((ABS(xs[0]) - ABS(en[1])) == 0.0) && (xb[0] == 0.0))
		    x3[0] = x1[0]*POW(10.0, -_PG_gattrs.axis_n_decades);
		 x2[0] = x1[0]*fdx[0];
		 x4[0] = x3[0]*fdx[0];}

	     else
	        {x2[0] = x1[0] + fdx[0];
		 x4[0] = x3[0] + fdx[0];};

	     if (g->iflog[1])
	        {if (((ABS(xs[1]) - ABS(en[1])) == 0.0) && (xb[1] == 0.0))
		    x3[1] = x1[1]*POW(10.0, -_PG_gattrs.axis_n_decades);
		 x2[1] = x1[1]/fdx[1];
		 x4[1] = x3[1]/fdx[1];}

	     else
	        {x2[1] = x1[1] - fdx[1];
		 x4[1] = x3[1] - fdx[1];};

	     break;

        case AXIS_TICK_STRADDLE :
	     if (g->iflog[0])
	        {if (((ABS(xs[0]) - ABS(en[1])) == 0.0) && (xb[0] == 0.0))
		    xb[0] = xa[0]*POW(10.0, -_PG_gattrs.axis_n_decades);
		 x1[0] = xa[0]/fdx[0];
		 x2[0] = xa[0]*fdx[0];
		 x3[0] = xb[0]/fdx[0];
		 x4[0] = xb[0]*fdx[0];}

	     else
	        {x1[0] = xa[0] - fdx[0];
		 x2[0] = xa[0] + fdx[0];
		 x3[0] = xb[0] - fdx[0];
		 x4[0] = xb[0] + fdx[0];}

	     if (g->iflog[1])
	        {if (((ABS(xs[1]) - ABS(en[1])) == 0.0) && (xb[1] == 0.0))
		    xb[1] = xa[1]*POW(10.0, -_PG_gattrs.axis_n_decades);
		 x1[1] = xa[1]*fdx[1];
		 x2[1] = xa[1]/fdx[1];
		 x3[1] = xb[1]*fdx[1];
		 x4[1] = xb[1]/fdx[1];}

	     else
	        {x1[1] = xa[1] + fdx[1];
		 x2[1] = xa[1] - fdx[1];
		 x3[1] = xb[1] + fdx[1];
		 x4[1] = xb[1] - fdx[1];};

	     break;

        default :
	     return(FALSE);};

/* draw the ticks */
    PG_draw_multiple_line(dev, n, x1, x2, x3, x4, dx);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_SPRINTF - converts a number to a character string
 *             - and removes additional spaces
 *             - arguments are the same as for sprintf
 */

static void _PG_sprintf(char *string, char *format, double value, double tol)
   {char temp[40], *token, *s;

    if (ABS(value) < tol)
       value = 0.0;

    if (strchr(format, 'd') != NULL)
       snprintf(temp, 40, format, (int) value);
    else
       snprintf(temp, 40, format, value);

    SC_strsubst(temp, 40, temp, "e-00", "e+00", -1);

    token = SC_strtok(temp, " ", s);
    if (token != NULL)
       strcpy(string, token);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_AXIS_LABEL_FMT - determine whether to use 'e' or 'f' format
 *                    - since 'g' format is a dog
 */

static void _PG_axis_label_fmt(PG_device *dev,
			       char *format, double *pdx, double *pdy,
			       double *ptol, char *fmt,
			       PG_axis_tick_def *td)
   {int i, n, ti;
    double tol, yo, sp, ls, db, dv;
    double en[2], vo[2];
    double ld[PG_SPACEDM], tdx[PG_SPACEDM];
    double *dx;
    char s[20], fc;
    PG_dev_geometry *g;

    g = &dev->g;

    strcpy(format, fmt);

    sp = td->space;
    dx = td->dx;
    n  = td->n;
    en[0] = td->en[0];
    en[1] = td->en[1];
    vo[0] = td->vo[0];
    vo[1] = td->vo[1];
    if (g->iflog[0] || g->iflog[1])
       tol = 0.0;
    else
       tol = 1.0e-6*ABS(vo[1] - vo[0]);

/* determine whether to use 'e' or 'f' format */
    yo = max(ABS(en[0]), ABS(en[1]));
    ti = strlen(format) - 1;
    ti = max(ti, 0);
    fc = format[ti];
    if (fc == 'g')
       {if ((yo > 1000.0) || (yo < 0.011))
           format[ti] = 'e';
        else
           format[ti] = 'f';};

/* find the maximum label size */
    ld[0] = 0.0;
    ld[1] = 0.0;
    dv  = vo[1] - vo[0];
    for (i = 0; i < n; i++)
        {db = dx[i];
         ls = vo[0] + db*dv;
	 _PG_sprintf(s, format, ls, tol);
         PG_get_text_ext_n(dev, 2, WORLDC, s, tdx);
         ld[0] = max(ld[0], tdx[0]);
         ld[1] = max(ld[1], tdx[1]);};

    *pdx  = ld[0];
    *pdy  = ld[1];
    *ptol = tol;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_WRITE_LABEL - write a single label */

static void _PG_write_label(PG_device *dev, char *format,
                            double *xa, double lss, double tol,
                            double *fx, int flg)
   {int id;
    double tdx[PG_SPACEDM], xc[PG_SPACEDM];
    char s[80];
    char *ps;
    PG_dev_geometry *g;

    g = &dev->g;

    if (flg)
       {s[0] = flg;
        _PG_sprintf(s+1, format, lss, tol);}
    else
       _PG_sprintf(s, format, lss, tol);
    
    ps = SC_dstrsubst(s, "e-00", "e+00", -1);

    PG_get_text_ext_n(dev, 2, WORLDC, ps, tdx);

    for (id = 0; id < 2; id++)
        {if (g->iflog[id] == TRUE)
	    xc[id] = xa[id]*POW(tdx[id], fx[id]);
	 else
	    xc[id] = xa[id] + fx[id]*tdx[id];};

    PG_write_n(dev, 2, WORLDC, xc, "%s", ps);

    SFREE(ps);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_DRAW_LABEL - draw the axis numerics */

static int _PG_draw_label(PG_device *dev, PG_axis_def *ad, char *fmt)
   {int i, id, l, lt, n, inc, sz;
    int ovlp[PG_SPACEDM];
    double Dr, rmx, idm;
    double sp, ls, dv, sv, db, tol, log_scale;
    double en[2], vo[2];
    double ldx[PG_SPACEDM], xs[PG_SPACEDM], sx[PG_SPACEDM];
    double fdx[PG_SPACEDM], fx[PG_SPACEDM], dx0[PG_SPACEDM];
    double xa[PG_SPACEDM], xb[PG_SPACEDM], xp[PG_SPACEDM];
    double ac[PG_SPACEDM];
    double bnd[PG_BOXSZ];
    double *dx;
    char s[20], t[20], format[10], fc;
    char *tf, *ts;
    PG_axis_tick_def *td;
    PG_dev_geometry *g;

    if ((dev == NULL) || (ad == NULL))
       return(FALSE);

    g = &dev->g;

    td = ad->tick + (AXIS_TICK_LABEL >> 1);

    en[0] = td->en[0];
    en[1] = td->en[1];
    n  = td->n;
    dx = td->dx;

    xs[0] = ad->x0[0];
    xs[1] = ad->x0[1];
    sx[0] = ad->scale_x[0];
    sx[1] = ad->scale_x[1];
    ac[0] = ad->cosa;
    ac[1] = ad->sina;

/* compute the actual point on the axis of the first and last tick */
    for (id = 0; id < 2; id++)
        {xa[id] = xs[id] + ABS(en[0])*ac[id]*sx[id];
	 xb[id] = xs[id] + ABS(en[1])*ac[id]*sx[id];};

    log_scale = _PG_axis_place(dev, fdx, ad, 0.0, AXIS_TICK_LABEL);

    PG_get_viewspace(dev, BOUNDC, bnd);

    sp  = td->space;
    lt  = ad->label_type;
    inc = 1;

/* determine whether to use 'e' or 'f' format since 'g' format is a dog */
    _PG_axis_label_fmt(dev, format, &ldx[0], &ldx[1], &tol, fmt, td);

    switch (lt)
       {case AXIS_TICK_LEFT :
	     for (id = 0; id < 2; id++)
	         dx0[id] = (en[1] - en[0])*ac[id]*sx[id];

             if (g->iflog[0])
                {xa[0] /= fdx[0];
                 xb[0] /= fdx[0];}
             else
                {xa[0] -= fdx[0];
                 xb[0] -= fdx[0];};

             if (g->iflog[1])
                {xa[1] *= fdx[1];
                 xb[1] *= fdx[1];}
             else
                {xa[1] += fdx[1];
                 xb[1] += fdx[1];};

             fx[0] = -0.47*(1.0 + ac[1]);
             fx[1] = 0.5*ac[0] - 0.25*ABS(ac[0]) - 0.33*ABS(ac[1]);
             break;

        case AXIS_TICK_RIGHT :
	     for (id = 0; id < 2; id++)
	         dx0[id] = (en[1] - en[0])*ac[id]*sx[id];

             if (g->iflog[0])
                {xa[0] *= fdx[0];
                 xb[0] *= fdx[0];}
             else
                {xa[0] += fdx[0];
                 xb[0] += fdx[0];};

             if (g->iflog[1])
                {xa[1] /= fdx[1];
                 xb[1] /= fdx[1];}
             else
                {xa[1] -= fdx[1];
                 xb[1] -= fdx[1];};

             fx[0] = -0.47*(1.0 - ac[1]);
             fx[1] = -0.5*ac[0] - 0.25*ABS(ac[0]) - 0.33*ABS(ac[1]);
             break;

        case AXIS_TICK_ENDS :
             sp = ad->vo[1] - ad->vo[0];

	     for (id = 0; id < 2; id++)
	         xa[id] = 0.015*sp*ac[id]*sx[id];

             fx[0] = -0.47*(1.0 + ac[0]);
             fx[1] = -0.33*(1.0 + ac[1]);

             Dr = ad->dr;

	     for (id = 0; id < 2; id++)
	         {l = 2*id;
		  dx0[id] = Dr*ac[id];
		  if (g->iflog[id])
		     {idm = log_scale/
			    (ABS(0.5*(bnd[l] + bnd[l+1])) + SMALL);
		      dx0[id] *= (1.0 + (2.0*xa[id] - fx[id]*ldx[id]*ac[id])*idm);
		      xb[id]   = (xs[id] + 1.015*sp*ac[id]*sx[id])*(1.0 + xa[id]*idm);
		      xa[id]   = xs[id]/(1.0 + xa[id]*idm);}
		  else
		     {dx0[id] += 2.0*xa[id] - fx[id]*ldx[id]*ac[id];
		      xb[id]   = xs[id] + 1.015*sp*ac[id]*sx[id];
		      xa[id]   = xs[id] - xa[id];};};

             inc = n - 1;
             break;

        default :
             return(FALSE);};

/* draw the labels (always low to high - for numerics to come out right) */
    vo[0] = td->vo[0];
    vo[1] = td->vo[1];

/* set the axis type size now */
    if (_PG_gattrs.axis_char_size >= 8)
       {PG_get_font(dev, &tf, &ts, &sz);
	PG_set_font(dev, tf, ts, _PG_gattrs.axis_char_size);};

/* if the labels cannot be distinguished because of the precision
 * try a different tack
 */
    dv = ABS(vo[1] - vo[0]);
    sv = ABS(vo[0]) + ABS(vo[1]);

/* treat the first label specially because of the possible '~' or '>' */
    db = dx[0];
    for (id = 0; id < 2; id++)
        xp[id] = xa[id] + db*dx0[id];

    ls = vo[0] + db*dv;
    fc = FALSE;
    if (dv < 0.001*sv)
       {snprintf(s, 20, format, vo[0]);
        snprintf(t, 20, format, vo[0] + dx[1]*dv);
        fc = (strcmp(s, t) == 0) ? '~' : '>';
	vo[0] = 0.0;};

    _PG_write_label(dev, format, xp, ls, tol, fx, fc);

/* draw remaining labels */
    for (i = inc; i < n; i += inc)
        {db = dx[i];
	 for (id = 0; id < 2; id++)
	     xs[id] = xa[id] + db*dx0[id];
         ls = vo[0] + db*dv;

/* skip over labels that would overlap with the last one printed */
	 if (g->iflog[0])
	    {rmx      = max(xs[0]/xp[0], xp[0]/xs[0]);
	     ovlp[0]  = ((ldx[0] > 1.1) && (ldx[0] > 0.9*rmx));
	     ovlp[0] |= ((ldx[0] <= 1.1) && (ldx[0] > rmx));
	     ovlp[0] &= (xs[0]*xp[0] != 0.0);}
	 else
	    ovlp[0] = (ldx[0] > ABS(xs[0] - xp[0]));

	 if (g->iflog[1])
	    {rmx     = max(xs[1]/xp[1], xp[1]/xs[1]);
	     ovlp[1] = ((xs[1]*xp[1] != 0.0) && (ldx[1] > rmx));}
	 else
	    ovlp[1] = (ldx[1] > ABS(xs[1] - xp[1]));

	 if (ovlp[0] && ovlp[1])
	    continue;

/* remember this position and print it */
	 for (id = 0; id < 2; id++)
	     xp[id] = xs[id];

         _PG_write_label(dev, format, xs, ls, tol, fx, FALSE);};

/* reset the axis type */
    if (_PG_gattrs.axis_char_size >= 8)
       {PG_set_font(dev, tf, ts, sz);
        SFREE(tf);
        SFREE(ts);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PG_DRAW_AXIS_N - draw and label a plot axis
 *                - this routine will produce labels and
 *                - ticks or just ticks, depending on the arguments.
 *                -
 *                -    XL         - world coordinate of beginning end
 *                -    XR         - world coordinate of terminating end
 *                -    TN         - fractional position of v1 and v2 along axis
 *                -               - i.e. normalized coordinates wrt (XL, XR)
 *                -    VW         - first and last tick or label value
 *                -               - internally VW are ordered into VO and
 *                -               - the order of TN changed if needed
 *                -               - VW are world coordinate values
 *                -    sc         - an additional scale factor which is
 *                -                 used, for example, when doing an
 *                -                 Inselberg axis in which the range may
 *                -                 correspond to one apropriate for
 *                -                 the perpendicular dimension
 *                -    format     - specifies the format in the standard C way
 *                -    tick_type  - types of ticks
 *                -                 AXIS_TICK_RIGHT    - ticks on right
 *                -                 AXIS_TICK_LEFT     - ticks on left
 *                -                 AXIS_TICK_STRADDLE - ticks straddle (both)
 *                -    label_type - types of labels
 *                -                 AXIS_TICK_RIGHT - labels on right
 *                -                 AXIS_TICK_LEFT  - labels on left
 *                -                 AXIS_TICK_NONE  - no labels
 *                -                 AXIS_TICK_ENDS  - labels at ends of axis
 *                -    tickdef    - specifies the labels and ticks
 *                -                 AXIS_TICK_MAJOR - major ticks
 *                -                 AXIS_TICK_MINOR - minor ticks
 *                -                 AXIS_TICK_LABEL - labels
 *                -
 *                - An axis is a directed line segment (from Xl to Xr) with ticks
 *                - The label values as defined by VW, TN, and ticks
 *                - associate with the line segment as follows:
 *                -
 *                -      XL                                       XR
 *                -      .------------------------------------------>
 *                -        |                                      |
 *                - en[0] <-> vw[0] = v(tn[0])             en[1] <-> vw[1] = v(tn[1])
 *                -   
 *                - (rotations done by theta)
 *                - EN cannot be magnitudes only because the interval (-1, 1)
 *                - would have en[0] = en[1]
 *                - This is independent of tick type!
 */

PG_axis_def *PG_draw_axis_n(PG_device *dev, double *xl, double *xr,
			    double *tn, double *vw, double sc,
			    char *format, int tick_type, int label_type,
			    int flag, ...)
   {int tickdef, fx[PG_SPACEDM];
    double vo[2], minorsz, majorsz;
    double bnd[PG_BOXSZ];
    PG_axis_def *ad;
    PG_dev_geometry *g;

    ad = NULL;
    g  = &dev->g;

/* NOTE: because the log space is handled with the coordinate system
 * and the coordinate system is biased toward quadrant one
 * drawing log axes in the sense that PGS defines axes is ill-posed
 * if the log scale increases toward the origin
 * do not even try to make sense of it or you will go stark raving
 * mad - been there, done that, bought the t-shirt
 * it would require shifting the log space handling from the coordinate
 * system to the individual drawing operations
 */
    fx[0] = (((xl[0] < xr[0]) ^ (vw[0] < vw[1])) &&
	     (xl[0] != xr[0]) && (g->iflog[0] == TRUE));
    fx[1] = (((xl[1] < xr[1]) ^ (vw[0] < vw[1])) &&
	     (xl[1] != xr[1]) && (g->iflog[1] == TRUE));
    if ((fx[0] == TRUE) || (fx[1] == TRUE))
       return(ad);

    if ((dev != NULL) && (_PG_gattrs.axis_on == TRUE))
       {if (vw[0] == vw[1])
	   {vw[1] += SMALL;
	    vw[0] -= SMALL;};

/* internally keep the VW ordered as (VO[0], VO[1]) - min to max
 * for clarity and to minimize operations
 * then the tick spacing array DX can always be simply increasing
 * have the TN interval change to absorb any swap
 */
	vo[0] = vw[0];
	vo[1] = vw[1];
	if (vo[0] > vo[1])
	   {SC_SWAP_VALUE(double, tn[0], tn[1]);
	    SC_SWAP_VALUE(double, vo[0], vo[1]);};

	PG_set_color_line(dev, dev->line_color, TRUE);
	PG_draw_line_n(dev, 2, WORLDC, xl, xr, dev->clipping);

	PG_get_viewspace(dev, BOUNDC, bnd);

	ad = _PG_mk_axis_def(_PG_gattrs.axis_type, tick_type, label_type,
			     xl, xr, tn, vo, sc, g->iflog);

	majorsz = _PG_gattrs.axis_major_tick_size*dev->g.phys_width;
	minorsz = majorsz/2.0;

	SC_VA_START(flag);

	while ((tickdef = SC_VA_ARG(int)) != 0)
	   {if (tickdef & AXIS_TICK_MINOR)
	       {_PG_tick(ad, AXIS_TICK_MINOR);
		_PG_draw_tick(dev, ad, minorsz, AXIS_TICK_MINOR);};

	    if (tickdef & AXIS_TICK_MAJOR)
	       {_PG_tick(ad, AXIS_TICK_MAJOR);
		_PG_draw_tick(dev, ad, majorsz, AXIS_TICK_MAJOR);};

	    if (tickdef & AXIS_TICK_LABEL)
	       {PG_set_color_text(dev, dev->text_color, TRUE);
		_PG_tick(ad, AXIS_TICK_LABEL);
		_PG_draw_label(dev, ad, format);};};

	SC_VA_END;

	if (flag == FALSE)
	   {_PG_rl_axis_def(ad);
	    ad = NULL;};};

    return(ad);}

/*--------------------------------------------------------------------------*/

/*                               3D AXIS ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* PG_AXIS_3 - draw a set of axes for a surface plot */

void PG_axis_3(PG_device *dev, double **x, int np, double *databox)
   {int lc, ls;
    double x1[PG_SPACEDM], x2[PG_SPACEDM];
    double **p;
    PG_dev_geometry *g;

    g = &dev->g;

/* axis drawing properties */
    PG_get_line_color(dev, &lc);
    PG_get_line_style(dev, &ls);
    PG_set_color_line(dev, dev->WHITE, TRUE);
    PG_set_line_style(dev, 3);

/* draw axes on the databox */
    if (databox != NULL)
       p = PG_get_space_box(dev, databox, TRUE);

/* draw axes on the boundary as in the 2d case */
    else
       p = PG_get_space_box(dev, g->bnd, TRUE);

    x1[0] = p[0][0];
    x1[1] = p[1][0];

/* x-axis */
    x2[0] = p[0][1];
    x2[1] = p[1][1];
    PG_draw_line_n(dev, 2, WORLDC, x1, x2, FALSE);
    PG_write_n(dev, 2, WORLDC, x2, "X");

/* y-axis */
    x2[0] = p[0][2];
    x2[1] = p[1][2];
    PG_draw_line_n(dev, 2, WORLDC, x1, x2, FALSE);
    PG_write_n(dev, 2, WORLDC, x2, "Y");

/* z-axis */
    x2[0] = p[0][4];
    x2[1] = p[1][4];
    PG_draw_line_n(dev, 2, WORLDC, x1, x2, FALSE);
    PG_write_n(dev, 2, WORLDC, x2, "Z");

    PM_free_vectors(3, p);

    PG_set_line_color(dev, lc);
    PG_set_line_style(dev, ls);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
