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
   {int nt, n5, n2;
    double drl, divs, sp, isp, dv, dt;

    dv = ad->vo[1] - ad->vo[0];
    if (dv > 0.0)

/* find the integral points (10**sp) */
       {sp = log10(dv);

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
		drl *= 2.0;};};}

    else
       {drl = (1.0 - TOLERANCE);
	nt  = 1;};

    if (nt <= 2)
       divs = 20.0;
    else
       divs = 10.0;

/* values decreasing along the vector of the axis */
/*    if (ad->t2 < ad->t1) */
    if (ad->vo[1] < ad->vo[0])
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
   {int tty;
    int idx[PG_SPACEDM];
    double logsc, tcksc, lmt, csa, sna;
    double tdx[PG_SPACEDM], bnd[PG_BOXSZ];
    PG_dev_geometry *g;

    g = &dev->g;

    memset(tdx, 0, sizeof(tdx));

    lmt = 0.030;
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
       {tdx[0]  = _PG_gattrs.axis_major_tick_size*dev->g.phys_dx[0] +
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

/* _PG_TICK_DIV_FIX - remove anybody outside the range VO
 *                  - also any duplications
 */

static void _PG_tick_div_fix(PG_axis_tick_def *td, double *vo,
			     double dv, int tick, int dec)
   {int na, jin, jout;
    double r, s, t;
    double vod[2];
    double *dx;

    na = td->n;
    dx = td->dx;

    if (na > 2)
       {vod[0] = max(vo[0] - 0.5*dv, 0.9999*vo[0]);
	vod[1] = min(vo[1] + 0.5*dv, 1.0001*vo[1]);

	for (jin = 0, jout = 0; jin < na; jin++)
            {t = dx[jin];
             if ((vod[0] <= t) && (t <= vod[1]))
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
		 if ((vod[0] <= t) && (t <= vod[1]))
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

static void _PG_tick_div_dec(PG_axis_tick_def *td, double *vo,
			     double dv, int tick, double dlv)
   {int j, k;
    int da, db, dexp, na, step, decade;;
    int lvo[2];
    double t;
    double sub[10], va[2];
    double *dx;

/* find the extremal values for major ticks and labels */
    lvo[0] = log10(vo[0]);
    lvo[1] = log10(vo[1]);
    lvo[0] = max(lvo[0], lvo[1] - _PG_gattrs.axis_n_decades - 1);

    va[0] = POW(10.0, lvo[0]);
    va[1] = POW(10.0, lvo[1]);

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
    da = 1;                               /* above */
    db = 1;                               /* below */
    na = dlv + 1 + da + db;
    na = max(2, na);
    if (na > _PG_gattrs.axis_max_major_ticks)
       dexp = na/(_PG_gattrs.axis_max_major_ticks + 1);
    else
       dexp = 1;

    na = (na/dexp) + step*(na - 1);
    step++;

    dx = CMAKE_N(double, step*na);

/* fill array of division locations - subdivide decades */
    for (j = 0, decade = lvo[0]-db; j < na; decade += dexp)
        {t = PM_pow(10.0, decade);

	 dx[j++] = t;
	 if (step > 1)
	    {for (k = 1; (k < step) && (j < na); k++, j++)
	         dx[j] = t*sub[k];};};

    td->n         = na;
    td->ndiv      = j;
    td->vo[0]     = va[0];
    td->vo[1]     = va[1];
    td->dx        = dx;
    td->log_scale = 1;

    _PG_tick_div_fix(td, vo, dv, tick, TRUE);

    td->vo[0] = dx[0];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK_DIV_EQR - find extremal label values and compute
 *                  - equal ratio tick spacings
 *                  - a log subdivision of the decades
 */

static void _PG_tick_div_eqr(PG_axis_tick_def *td, double *vo,
			     double dv, int tick)
   {int i, j, k;
    int dexp, na, step;
    double va[2], vd, nv, r, t;
    double *dx;

/* find the extremal values for major ticks and labels
 * also the number of divisions NA - corresponds to minor tick spacings
 */
    va[0] = PM_round(vo[0]/dv);
    va[1] = PM_round(vo[1]/dv);
    nv    = PM_round(va[1] - va[0]) + 1.0;

/* too many divisions - increase DV by factors of 10 */
    while (nv > 10)
       {dv *= 10.0;
        va[0] = PM_round(vo[0]/dv);
	va[1] = PM_round(vo[1]/dv);
	nv    = PM_round(va[1] - va[0]) + 1.0;};

/* too few divisions - reduce DV by factors of 10 */
    while (nv <= 2)
       {dv *= 0.1;
        va[0] = PM_round(vo[0]/dv);
	va[1] = PM_round(vo[1]/dv);
	nv    = PM_round(va[1] - va[0]) + 1.0;};
    na  = nv;
    va[0] *= dv;
    va[1] *= dv;

/* do not let VA[0] be more than maximum number of decades down from VA[1] */
    vd    = va[1]*PM_pow(10.0, -_PG_gattrs.axis_n_decades);
    va[0] = max(va[0], vd);

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

    dx = CMAKE_N(double, step*na);

/* fill array of division locations - equal ratio spacing */
    for (i = 0, j = 0, t = va[0]; i < na; i++)
        {dx[j++] = t;

	 r  = POW((t + dv)/t, 0.1);
	 t += dv;

	 if ((step > 1) && (tick == AXIS_TICK_MINOR))
	    {for (k = 1; (k < step) && (j < na); k++, j++)
	         dx[j] = dx[j-1]*r;};};

    td->n         = na;
    td->vo[0]     = va[0];
    td->vo[1]     = va[1];
    td->dx        = dx;
    td->log_scale = -1;

    _PG_tick_div_fix(td, vo, dv, tick, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK_DIV_NRM - return the spacings in the range from 0 to 1 */

static void _PG_tick_div_nrm(PG_axis_tick_def *td)
   {int j, na;
    double di, nrm;
    double extr[2];
    double *dx;

    na = td->n;
    dx = td->dx;

    if (na > 1)
       {

#if 0
        extr[1] = dx[na - 1];
        extr[0] = dx[0];
#else
        extr[1] = td->vo[1];
        extr[0] = td->vo[0];
#endif

/* shift by the mimimum value */
        for (j = 0; j < na; j++)
            dx[j] -= extr[0];

/* normalize the spacings */
        di = extr[1] - extr[0];
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
   {double va, vo[2], dlu, dlv, dv;

/* get out the limits VO */
    vo[0] = ad->vo[0];
    vo[1] = ad->vo[1];
    va    = vo[1]*POW(10.0, -_PG_gattrs.axis_n_decades);
    vo[0] = max(vo[0], va);
 
/* divide the interval into reasonable sized units wrt the scale of the values */
    dlv = log10(vo[1]/vo[0]);
    dlu = (dlv > 0.0) ? floor(dlv) : ceil(dlv);
    if (dlu == 0.0)
       dlv = 0.0;
    else
       dlv = PM_round(dlv);

    dv = POW(10.0, dlv);

/* if the interval is greater than one decade the major ticks fall on decades */
    if (dlv > 0.0)
       _PG_tick_div_dec(td, vo, dv, tick, dlv);

/* if the interval is less than one decade use equal ratio spacing */
    else
       _PG_tick_div_eqr(td, vo, dv, tick);

/* put the spacings into the range 0 to 1 */
    _PG_tick_div_nrm(td);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK_DIV_LIN - find the extremal values VA
 *                  - number of divisions N and spacings DX
 *                  - for a linear scale axis
 */

static void _PG_tick_div_lin(PG_axis_tick_def *td,
			     PG_axis_def *ad, double sp)
   {int i, n;
    double dx0;
    double va[2], vo[2];
    double *dx;

    vo[0] = ad->vo[0];
    vo[1] = ad->vo[1];
    sp    = ABS(sp);

    va[0] = sp*PM_fix(vo[0]/sp);
    va[1] = sp*PM_fix(vo[1]/sp);

    if (ad->label_type != AXIS_TICK_ENDS)
       {if (vo[0] < vo[1])
	   {if (va[0] < vo[0])
	       va[0] += sp;
	    if (va[1] > vo[1])
	       va[1] -= sp;};};

    n = (va[1] - va[0])/sp + 1.0 + EPSILON;
    n = max(n, 2.0 + EPSILON);

/* fill the tick spacings array */
    dx  = CMAKE_N(double, n);
    dx0 = 1.0/((double) n - 1.0);
    for (i = 0; i < n; i++)
        dx[i] = i*dx0;

    td->n         = n;
    td->vo[0]     = va[0];
    td->vo[1]     = va[1];
    td->dx        = dx;
    td->log_scale = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_TICK - given a spacing compute the following additional tick
 *          - information:
 *          -
 *          -    en[0] - first tick position
 *          -    en[1] - last tick position
 *          -    n     - number of ticks
 */

void _PG_tick(PG_axis_def *ad, int tick)
   {int i;
    double dvi, sp;
    double w[2], a[2], vo[2], tn[2];
    PG_axis_tick_def *td;

    td = ad->tick + (tick >> 1);
    sp = td->space;

/* determine good values for the extremal labels VO the
 * number of ticks N and the tick spacings DX
 */

/* log scale */
    if ((ad->log_scale[0] && (ABS(ad->cosa) >= 0.9999)) ||
        (ad->log_scale[1] && (ABS(ad->sina) >= 0.9999)))
       _PG_tick_div_log(td, ad, tick);

/* linear scale */
    else
       _PG_tick_div_lin(td, ad, sp);

    for (i = 0; i < 2; i++)
        {vo[i] = ad->vo[i];
	 tn[i] = ad->tn[i];};

/* TN is only used to compute the scale - not for drawing */
    dvi  = ad->dr/((vo[1] - vo[0])*ad->scale);
    a[0] = (vo[0]*tn[1] - vo[1]*tn[0])*dvi;
    a[1] = (tn[1] - tn[0])*dvi;

/* shift down by A[0] - taking care about roundoff */
    for (i = 0; i < 2; i++)
        {w[i]      = a[1]*(td->vo[i]);
	 td->en[i] = (PM_CLOSETO_REL(w[i], a[0]) == TRUE) ? 0.0 : w[i] - a[0];};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_MK_AXIS_DEF - initialize and return a PG_axis_def which contains
 *                 - the information necessary to draw an axis
 */

PG_axis_def *_PG_mk_axis_def(int atype, int ttype, int ltype,
			     double *xl, double *xr,
			     double *tn, double *vo,
			     double as, int *iflg)
   {int i;
    double dr, idr, cosa, sina, toler;
    double dx[2];
    PG_axis_def *ad;
    PG_axis_tick_def *td;

    ad = CMAKE(PG_axis_def);

/* compute various quantities needed by the axis routines */
    dx[0] = xr[0] - xl[0];
    dx[1] = xr[1] - xl[1];
    dr    = HYPOT(dx[0], dx[1]);

    idr = 1.0/(dr + SMALL);

/* compute the orientation of the axis */
    cosa = dx[0]*idr;
    sina = dx[1]*idr;

/* VO[0] and VO[1] must be further apart than toler apart */
    toler = 1.0e-8;
    if (ABS(vo[0] - vo[1]) < toler*(ABS(vo[0]) + ABS(vo[1])))
       {if (vo[0] == 0.0)
	   {vo[0] -= 0.1;
	    vo[1] += 0.1;}
	else
	   {vo[0] -= 0.5*toler*vo[0];
	    vo[1] += 0.5*toler*vo[1];};};

/* setup the general axis characteristics */
    ad->axis_type    = atype;
    ad->tick_type    = ttype;
    ad->label_type   = ltype;
    ad->log_scale[0] = iflg[0];
    ad->log_scale[1] = iflg[1];
    ad->eps_rel      = 1.0e-4*(vo[1] - vo[0]);

/* define the axis vector */
    ad->x0[0] = xl[0];
    ad->x0[1] = xl[1];
    ad->dr    = dr;
    ad->cosa  = cosa;
    ad->sina  = sina;

/* define axis scale factors */
    ad->scale      = as;
    ad->scale_x[0] = as;
    ad->scale_x[1] = as;

/* TN[0] and TN[1] are only used to compute the scale - not for drawing */
    ad->tn[0] = tn[0];
    ad->tn[1] = tn[1];
    ad->vo[0] = vo[0];
    ad->vo[1] = vo[1];

/* initialize the tick/label spacings structs */
    for (i = 0; i < 3; i++)
        {td = ad->tick + i;

	 td->n     = 0;
	 td->en[0] = 0.0;
	 td->en[1] = 0.0;
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
        {CFREE(ad->tick[i].dx);};

    CFREE(ad);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PG_AXIS_TICK_TEST - test log axis tick computation */

void _PG_axis_tick_test(double *xl, double *xr, double *vo)
   {int id;
    int iflg[PG_SPACEDM];
    double tn[2];
    PG_axis_def *ad;
    
    for (id = 0; id < PG_SPACEDM; id++)
        iflg[id] = TRUE;

    tn[0] = 0.0;
    tn[1] = 1.0;

    ad = _PG_mk_axis_def(CARTESIAN_2D, AXIS_TICK_STRADDLE, AXIS_TICK_RIGHT,
			 xl, xr, tn, vo, 1.0, iflg);

    _PG_tick(ad, AXIS_TICK_MAJOR);
    _PG_tick(ad, AXIS_TICK_MINOR);
    _PG_tick(ad, AXIS_TICK_LABEL);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
