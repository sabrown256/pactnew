/*
 * GSSCALE.C - compute axis tick and label positions
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgs_int.h"

#define EPSILON     1.0e-4
#define MIN_DECADE  0.1

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static INLINE double _PG_axis_delta(double c, double lwr, double upr,
				    int flg, int lsc)
    {double del;

     if (flg == TRUE)
        {del = 1.0 + lsc*ABS(c*log10(upr/lwr));
         if (c < 0.0)
	    del = 1.0/del;
         if (upr < lwr)
	    del = 1.0/del;}
     else
        del = c*(upr - lwr);

     return(del);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_COMP_TICK_SPACINGS - compute and return the
 *                        - major and minor tick spacings
 *                        - and the label spacings
 */

static void _PG_comp_tick_spacings(PG_axis_def *ad)
   {double drl, divs, sp, isp, dv, dt;
    int nt, n5, n2;

    dv = ad->vx - ad->vn;

/* find the integral points (10**sp) */
    sp = log10(dv);

    isp = 0.0;
    if (sp < 0.0)
       isp = PM_fix(sp - (1.0 + EPSILON));

    else if (sp > 0.0)
       isp = PM_fix(sp + EPSILON);

/* find the number of integral points in the interval */
    drl = (1.0 - TOLERANCE)*POW(10.0, isp);
    nt  = PM_fix(dv/drl);

    if (nt <= 2)
       {drl *= 0.1;
        nt  *= 10;};

    while (nt >= _PG_gattrs.axis_max_major_ticks)
       {n5 = nt/5;
        n2 = nt/2;
        dt = (5.0*n5 - 2.0*n2);
        if (dt >= -TOLERANCE)
           {nt   = n5;
            drl *= 5.0;}
	else
           {nt   = n2;
            drl *= 2.0;};};

    if (nt <= 2)
       divs = 20.0;
    else
       divs = 10.0;

/* values decreasing along the vector of the axis */
/*    if (ad->t2 < ad->t1) */
    if (ad->vx < ad->vn)
       drl *= -1.0;

    ad->tick[AXIS_TICK_MAJOR >> 1].space = drl;
    if (_PG_gattrs.axis_n_minor_ticks == -1)
       ad->tick[AXIS_TICK_MINOR >> 1].space = drl/divs;
    else
       ad->tick[AXIS_TICK_MINOR >> 1].space = drl/((double) _PG_gattrs.axis_n_minor_ticks);

    ad->tick[AXIS_TICK_LABEL >> 1].space = drl;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_AXIS_PLACE - place labels and ticks
 *                - actually convert limits from PC to WC
 *                - IDX and IDY are spacings in PC
 *                - return PDX and PDY in WC
 */

double _PG_axis_place(PG_device *dev, double *dx,
		      PG_axis_def *ad, double sz, int tick)
   {int idx[PG_SPACEDM], tty;
    double logsc, tcksc, lmt, csa, sna;
    double tdx[PG_SPACEDM], bnd[PG_BOXSZ];
    PG_dev_geometry *g;

    g = &dev->g;

    csa = ad->cosa;
    sna = ad->sina;
    tty = ad->tick_type;

    if ((tick & AXIS_TICK_MINOR) || (tick & AXIS_TICK_MAJOR))
       {if (tty == AXIS_TICK_STRADDLE)
	   tcksc = 0.5;
        else
	   tcksc = 1.0;

	tdx[0] = tcksc*sz;
	tdx[1] = tcksc*sz;

	lmt = 0.030;}

/* compute the offset from the major tick for the aligned label text
 * alignment is horizontal and vertical centering
 */
    else if (tick & AXIS_TICK_LABEL)
       {tdx[0]  = _PG_gattrs.axis_major_tick_size*dev->g.phys_width +
	       _PG.axis_label_standoff_pc;
	tdx[1]  = tdx[0];
	tdx[0] += ((_PG_gattrs.axis_tick_type == AXIS_TICK_LEFT) ? 
		0.0 : _PG_gattrs.axis_label_x_standoff);
	tdx[1] += ((_PG_gattrs.axis_tick_type == AXIS_TICK_LEFT) ? 
		0.0 : _PG_gattrs.axis_label_y_standoff);

	lmt = 0.035;};

    idx[0] = tdx[0];
    idx[1] = tdx[1];

/* tick size in PC */
    tdx[0] = idx[0];
    tdx[1] = idx[1];

/* tick size in NDC */
    tdx[0] /= PG_window_width(dev);
    tdx[1] /= PG_window_height(dev);

    if (tick == AXIS_TICK_MINOR)
       lmt *= 0.5;

/* put NDC minimum size limit on ticks */
    tdx[0] = min(tdx[0], lmt);
    tdx[1] = min(tdx[1], lmt);

/* fold in the tick orientation */
    tdx[0] *= sna;
    tdx[1] *= csa;

    logsc = 2.0;

    PG_get_viewspace(dev, BOUNDC, bnd);

/* find tick size in WC */
    dx[0] = _PG_axis_delta(tdx[0], bnd[0], bnd[1], g->iflog[0], logsc);
    dx[1] = _PG_axis_delta(tdx[1], bnd[2], bnd[3], g->iflog[1], logsc);

    return(logsc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK_DIV_FIX - remove anybody outside the range (VN, VX)
 *                  - also any duplications
 */

static void _PG_tick_div_fix(PG_axis_tick_def *td,
			     double vn, double vx,
			     double dv, int tick, int dec)
   {int na, jin, jout;
    double vnd, vxd, r, s, t;
    double *dx;

    na = td->n;
    dx = td->dx;

    if (na > 2)
       {vnd = max(vn - 0.5*dv, 0.9999*vn);
	vxd = min(vx + 0.5*dv, 1.0001*vx);

	for (jin = 0, jout = 0; jin < na; jin++)
            {t = dx[jin];
             if ((vnd <= t) && (t <= vxd))
                {if ((tick == AXIS_TICK_MAJOR) || (tick == AXIS_TICK_LABEL))
                    {s = log10(dv/t);
		     r = floor(s);

/* MAJOR and LABEL ticks go on only on the decades
 * when the number of decades is greater than zero
 */
                     if (((s != 0.0) || (t != 0.0)) &&
			 !PM_CLOSETO_REL(s, r) &&
			 (dec == TRUE))
                        continue;};

                 dx[jout++] = t;};};

/* if we have thrown everything out go back and repeat with
 * the simpler criteria that it be in the interval
 */
	if (jout == 0)
	   {for (jin = 0, jout = 0; jin < na; jin++)
	        {t = dx[jin];
		 if ((vnd <= t) && (t <= vxd))
		    dx[jout++] = t;};};

	if (jout > 0)
	   na = jout;};

    td->n = na;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK_DIV_DEC - find extremal label values and compute
 *                  - tick spacings that fall on decades and
 *                  - a log subdivision of the decades
 */

static void _PG_tick_div_dec(PG_axis_tick_def *td, double vn, double vx,
			     double dv, int tick, double dlv)
   {int j, k;
    int lvn, lvx, dexp, na, step, decade;
    int da, db;
    double sub[10];
    double va, vb, t;
    double *dx;

/* find the extremal values for major ticks and labels */
    lvn = log10(vn);
    lvx = log10(vx);
    lvn = max(lvn, lvx - _PG_gattrs.axis_n_decades - 1);

    va = POW(10.0, lvn);
    vb = POW(10.0, lvx);

/* empirical relation - more decades fewer ticks/steps per decade */
    if (dlv > 9)
       step = 3;
    else if (dlv > 6)
       step = 7;
    else
       step = 8;

    for (j = 0; j <= step; j++)
        sub[j] = j + 1.0;

    if (step < 4)
       sub[step] = 5.0;

/* compute the number of divisions NA - corresponds to minor tick spacings
 * add extra decades to get minor ticks above and below the extremal major ticks
 */
    da = 0;                               /* above */
    db = 1;                               /* below */
    na = dlv + 1 + da + db;
    na = max(2, na);
    if (na > _PG_gattrs.axis_max_major_ticks)
       dexp = na/_PG_gattrs.axis_max_major_ticks;
    else
       dexp = 1;

    na = (na/dexp) + step*(na - 1);
    step++;

    dx = FMAKE_N(double, step*na, "_PG_TICK_DIV_DEC:dx");

/* fill array of division locations - subdivide decades */
    for (j = 0, decade = lvn-db; j < na; decade += dexp)
        {t = PM_pow(10.0, decade);

	 dx[j++] = t;
	 if (step > 1)
	    {for (k = 1; (k < step) && (j < na); k++, j++)
	         dx[j] = t*sub[k];};};

    td->n         = na;
    td->ndiv      = j;
    td->vn        = va;
    td->vx        = vb;
    td->dx        = dx;
    td->log_scale = 1;

    _PG_tick_div_fix(td, vn, vx, dv, tick, TRUE);

    td->vn = dx[0];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK_DIV_EQR - find extremal label values and compute
 *                  - equal ratio tick spacings
 *                  - a log subdivision of the decades
 */

static void _PG_tick_div_eqr(PG_axis_tick_def *td, double vn, double vx,
			     double dv, int tick)
   {int i, j, k;
    int dexp, na, step;
    double va, vb, vd, nv, r, t;
    double *dx;

/* find the extremal values for major ticks and labels
 * also the number of divisions NA - corresponds to minor tick spacings
 */
    va = PM_round(vn/dv);
    vb = PM_round(vx/dv);
    nv = PM_round(vb - va) + 1.0;

/* too many divisions - increase DV by factors of 10 */
    while (nv > 10)
       {dv *= 10.0;
        va = PM_round(vn/dv);
	vb = PM_round(vx/dv);
	nv = PM_round(vb - va) + 1.0;};

/* too few divisions - reduce DV by factors of 10 */
    while (nv <= 2)
       {dv *= 0.1;
        va = PM_round(vn/dv);
	vb = PM_round(vx/dv);
	nv = PM_round(vb - va) + 1.0;};
    na  = nv;
    va *= dv;
    vb *= dv;

/* do not let VA be more than maximum number of decades down from VB */
    vd = vb*PM_pow(10.0, -_PG_gattrs.axis_n_decades);
    va = max(va, vd);

/* empirical relation - almost linear anyway so fill it up */
    step = _PG_gattrs.axis_n_minor_ticks;
    if (step < 0)
       step = 9;

    if (na > _PG_gattrs.axis_max_major_ticks)
       dexp = na/_PG_gattrs.axis_max_major_ticks;
    else
       dexp = 1;

    na = (na/dexp) + step*(na - 1);
    step++;

    dx = FMAKE_N(double, step*na, "_PG_TICK_DIV_EQR:dx");

/* fill array of division locations - equal ratio spacing */
    for (i = 0, j = 0, t = va; i < na; i++)
        {dx[j++] = t;

	 r  = POW((t + dv)/t, 0.1);
	 t += dv;

	 if ((step > 1) && (tick == AXIS_TICK_MINOR))
	    {for (k = 1; (k < step) && (j < na); k++, j++)
	         dx[j] = dx[j-1]*r;};};

    td->n         = na;
    td->vn        = va;
    td->vx        = vb;
    td->dx        = dx;
    td->log_scale = -1;

    _PG_tick_div_fix(td, vn, vx, dv, tick, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK_DIV_NRM - return the spacings in the range from 0 to 1 */

static void _PG_tick_div_nrm(PG_axis_tick_def *td)
   {int j, na;
    double mx, mn, di, nrm;
    double *dx;

    na = td->n;
    dx = td->dx;

    if (na > 1)
       {

#if 0
        mx = dx[na - 1];
        mn = dx[0];
#else
        mx = td->vx;
        mn = td->vn;
#endif

/* shift by the mimimum value */
        for (j = 0; j < na; j++)
            dx[j] -= mn;

/* normalize the spacings */
        di = mx - mn;
        if (di == 0.0)
           nrm = 1.0;
        else
           nrm = 1.0/di;

        for (j = 0; j < na; j++)
            dx[j] *= nrm;}

    else
       dx[0] = 1;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK_DIV_LOG - compute a nice set of log spacings
 *                  - the minor spacings are chosen as follows:
 *                  -     step = 1, spaces = (2)
 *                  -     step = 2, spaces = (2, 5)
 *                  -     step = 3, spaces = (2, 3, 5)
 *                  -     step = 4, spaces = (2, 3, 4, 5)
 *                  -     step = 5, spaces = (2, 3, 4, 5, 6)
 *                  -     step = 6, spaces = (2, 3, 4, 5, 6, 7)
 *                  -     step = 7, spaces = (2, 3, 4, 5, 6, 7, 8)
 *                  -     step = 8, spaces = (2, 3, 4, 5, 6, 7, 8, 9)
 */

static void _PG_tick_div_log(PG_axis_tick_def *td,
			     PG_axis_def *ad, int tick)
   {double va, vn, vx, dlu, dlv, dv;

/* get out the limits (VN, VX) */
    vn = ad->vn;
    vx = ad->vx;
    va = vx*POW(10.0, -_PG_gattrs.axis_n_decades);
    vn = max(vn, va);
 
/* divide the interval into reasonable sized units wrt the scale of the values */
    dlv = log10(vx/vn);
    dlu = (dlv > 0.0) ? floor(dlv) : ceil(dlv);
    if (dlu == 0.0)
       dlv = 0.0;
    else
       dlv = PM_round(dlv);

    dv = POW(10.0, dlv);

/* if the interval is greater than one decade the major ticks fall on decades */
    if (dlv > 0.0)
       _PG_tick_div_dec(td, vn, vx, dv, tick, dlv);

/* if the interval is less than one decade use equal ratio spacing */
    else
       _PG_tick_div_eqr(td, vn, vx, dv, tick);

/* put the spacings into the range 0 to 1 */
    _PG_tick_div_nrm(td);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK_DIV_LIN - find the extremal values (VA, VB)
 *                  - number of divisions N and spacings DX
 *                  - for a linear scale axis
 */

static void _PG_tick_div_lin(PG_axis_tick_def *td,
			     PG_axis_def *ad, double sp)
   {int i, n;
    double va, vb, vn, vx, dx0;
    double *dx;

    vn = ad->vn;
    vx = ad->vx;
    sp = ABS(sp);

    va = sp*PM_fix(vn/sp);
    vb = sp*PM_fix(vx/sp);

    if (ad->label_type != AXIS_TICK_ENDS)
       {if (vn < vx)
	   {if (va < vn)
	       va += sp;
	    if (vb > vx)
	       vb -= sp;};};

    n = (vb - va)/sp + 1.0 + EPSILON;
    n = max(n, 2.0 + EPSILON);

/* fill the tick spacings array */
    dx  = FMAKE_N(double, n, "_PG_TICK:dx");
    dx0 = 1.0/((double) n - 1.0);
    for (i = 0; i < n; i++)
        dx[i] = i*dx0;

    td->n         = n;
    td->vn        = va;
    td->vx        = vb;
    td->dx        = dx;
    td->log_scale = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK - given a spacing compute the following additional tick
 *          - information:
 *          -
 *          -    st - first tick position
 *          -    en - last tick position
 *          -    n  - number of ticks
 */

void _PG_tick(PG_axis_def *ad, int tick)
   {double vx, vn, t1, t2, a, b, dvi, sp;
    PG_axis_tick_def *td;

    td = ad->tick + (tick >> 1);
    sp = td->space;

/* determine good values for the extremal labels (VN, VX) the
 * number of ticks N and the tick spacings DX
 */

/* log scale */
    if ((ad->log_scale[0] && (ABS(ad->cosa) >= 0.9999)) ||
        (ad->log_scale[1] && (ABS(ad->sina) >= 0.9999)))
       _PG_tick_div_log(td, ad, tick);

/* linear scale */
    else
       _PG_tick_div_lin(td, ad, sp);

    vx = ad->vx;
    vn = ad->vn;
    t2 = ad->t2;
    t1 = ad->t1;

/* T1 and T2 are only used to compute the scale - not for drawing */
    dvi = ad->dr/((vx - vn)*ad->scale);
/*    if ((t1 < t2) || (td->log_scale == FALSE)) */
       {a = (vn*t2 - vx*t1)*dvi;
	b = (t2 - t1)*dvi;}
/*    else
       {b = (vn*t2 - vx*t1)*dvi;
	a = (t2 - t1)*dvi;}; */

    td->start = b*(td->vn) - a;
    td->end   = b*(td->vx) - a;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MK_AXIS_DEF - initialize and return a PG_axis_def which contains
 *                 - the information necessary to draw an axis
 */

PG_axis_def *_PG_mk_axis_def(int atype, int ttype, int ltype,
			     double *xl, double *xr,
			     double t1, double t2, double vn, double vx,
			     double as, int *iflg)
   {int i;
    double dx, dy, dr, idr, cosa, sina, toler;
    PG_axis_def *ad;
    PG_axis_tick_def *td;

    ad = FMAKE(PG_axis_def, "_PG_MK_AXIS_DEF:ad");

/* compute various quantities needed by the axis routines */
    dx = xr[0] - xl[0];
    dy = xr[1] - xl[1];
    dr = HYPOT(dx, dy);

    idr = 1.0/(dr + SMALL);

/* compute the orientation of the axis */
    cosa = dx*idr;
    sina = dy*idr;

/* vn and vx must be further apart than toler apart */
    toler = 1.0e-8;
    if (ABS(vn - vx) < toler*(ABS(vn) + ABS(vx)))
       {if (vn == 0.0)
	   {vn -= 0.1;
	    vx += 0.1;}
	else
	   {vn -= 0.5*toler*vn;
	    vx += 0.5*toler*vx;};};

/* setup the general axis characteristics */
    ad->axis_type   = atype;
    ad->tick_type   = ttype;
    ad->label_type  = ltype;
    ad->log_scale[0] = iflg[0];
    ad->log_scale[1] = iflg[1];
    ad->eps_rel     = 1.0e-4*(vx - vn);

/* define the axis vector */
    ad->x0[0]   = xl[0];
    ad->x0[1]   = xl[1];
    ad->dr   = dr;
    ad->cosa = cosa;
    ad->sina = sina;

/* define axis scale factors */
    ad->scale   = as;
    ad->scale_x[0] = as;
    ad->scale_x[1] = as;

/* T1 and T2 are only used to compute the scale - not for drawing */
    ad->t1 = t1;
    ad->t2 = t2;
    ad->vn = vn;
    ad->vx = vx;

/* initialize the tick/label spacings structs */
    for (i = 0; i < 3; i++)
        {td = ad->tick + i;

	 td->n     = 0;
	 td->end   = 0.0;
	 td->start = 0.0;
	 td->space = 0.0;
	 td->dx    = NULL;};

    _PG_comp_tick_spacings(ad);

    return(ad);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_RL_AXIS_DEF - free a PG_axis_def instance */

void _PG_rl_axis_def(PG_axis_def *ad)
   {int i;

    for (i = 0; i < 3; i++)
        {SFREE(ad->tick[i].dx);};

    SFREE(ad);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_AXIS_TICK_TEST - test log axis tick computation */

void _PG_axis_tick_test(double *xl, double *xr, double vn, double vx)
   {int id;
    int iflg[PG_SPACEDM];
    PG_axis_def *ad;
    
    for (id = 0; id < PG_SPACEDM; id++)
        iflg[id] = TRUE;

    ad = _PG_mk_axis_def(CARTESIAN_2D, AXIS_TICK_STRADDLE, AXIS_TICK_RIGHT,
			 xl, xr, 0.0, 1.0, vn, vx, 1.0, iflg);

    _PG_tick(ad, AXIS_TICK_MAJOR);
    _PG_tick(ad, AXIS_TICK_MINOR);
    _PG_tick(ad, AXIS_TICK_LABEL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
