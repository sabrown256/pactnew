/*
 * MLCMSH.C - mesh patch fitting/combination routines
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "pml_int.h"

#define PARLLL      1
#define OPPST       0
#define INTERSECT  -1
#define CONTAINED  -2

/* CHCK_ORIENT - check the orientations of the segments */

#define CHCK_ORIENT                                                          \
    PM_MESH_ERR(_PM_seg_orient(basea, baseb) == PARLLL)

/* MATCH_SIDE - mark the two sides as matching */

#define MATCH_SIDE(basea, baseb)                                             \
    basea->match = baseb;                                                    \
    baseb->match = basea

/* ADVANCE - move to the next side in the chain */

#define ADVANCE(base) base = base->next

/* SAME - return TRUE is the points are too close */

#define SAME(x1, y1, x2, y2)                                                 \
    ((ABS(x1-x2) < TOLERANCE) && (ABS(y1-y2) < TOLERANCE))

/* NODE_OF - compute the node associated with (k, l) */

#define NODE_OF(k, l) (((l) - 1)*kbnd + (k) - 1)

/* VECSET4 - set up the pointers for the four corner vector set */

#define vecset4(v,v1,v2,v3,v4)                                               \
   v2 = v;                                                                   \
   v3 = v2 - 1;                                                              \
   v4 = v3 - kbnd;                                                           \
   v1 = v4 + 1

#define PM_MESH_ERR(x) if (x) LONGJMP(_PM.mesh_err, ABORT) 

/*--------------------------------------------------------------------------*/

/*                          MEMORY MANAGEMENT ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* PM_MK_CLINE - build and return a pointer to a curved line */

PM_conic_curve *PM_mk_cline(char *type, double axx, double ayy, double axy,
			    double ax, double ay, double ac)
   {PM_conic_curve *cp;

    cp       = CMAKE(PM_conic_curve);
    cp->type = CSTRSAVE(type);
    cp->xx   = axx;
    cp->xy   = axy;
    cp->yy   = ayy;
    cp->x    = ax;
    cp->y    = ay;
    cp->c    = ac;

    return(cp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MK_SIDE - build and return a pointer to a new side */

PM_side *PM_mk_side(PM_conic_curve *cur, int dir, double x, double y,
		    int dk, int dl, double krat, double lrat, double srat,
		    double drn, double drx, double ms, double es,
		    double me, double ee, PM_side *match, PM_side *next)
   {PM_side *base;
    double rk, rl, sdk, sdl;

    base = CMAKE(PM_side);
    base->x         = x;
    base->y         = y;
    base->dk        = dk;
    base->dl        = dl;
    base->k         = 0;
    base->l         = 0;
    base->side_rat  = srat;
    base->min_dr_f  = drn;
    base->max_dr_f  = drx;
    base->mag_start = ms;
    base->exp_start = es;
    base->mag_end   = me;
    base->exp_end   = ee;
    base->fill      = 0;
    base->scale     = 0.0;
    base->cosine    = 0.0;
    base->sine      = 0.0;
    base->c0        = 0.0;
    base->c1        = 0.0;
    base->c2        = 0.0;
    base->c3        = 0.0;
    base->dir       = dir;
    base->crve      = cur;
    base->match     = match;
    base->next      = next;

    if ((dk = abs(dk)) != 0)
       {sdk = (base->dk)/dk;
        rk  = pow(krat, sdk);}
    else
       rk = 1.0;

    if ((dl = abs(dl)) != 0)
       {sdl = (base->dl)/dl;
        rl  = pow(lrat, sdl);}
    else
       rl = 1.0;

    if ((dk == 0) && (dl == 0))
       base->ratio = 1.0;
    else
       base->ratio = (dk*rk + dl*rl) / (dk + dl);

    return(base);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MK_PART - build and return a pointer to a new part */

PM_part *PM_mk_part(char *name, int rg, int nsides,
		    PM_side *leg, PM_end_point *ends, void *comp,
		    double kms, double kes, double kme, double kee,
		    double krat, double lms, double les, double lme,
		    double lee, double lrat, PM_part *next)
   {PM_part *ipart;

    ipart          = CMAKE(PM_part);
    ipart->n_sides = nsides;
    ipart->leg     = leg;
    ipart->ends    = ends;
    ipart->comp    = comp;
    ipart->name    = CSTRSAVE(name);
    ipart->reg     = rg;
    ipart->next    = next;

    ipart->k_ratio     = krat;
    ipart->k_mag_start = kms;
    ipart->k_exp_start = kes;
    ipart->k_mag_end   = kme;
    ipart->k_exp_end   = kee;
    ipart->l_ratio     = lrat;
    ipart->l_mag_start = lms;
    ipart->l_exp_start = les;
    ipart->l_mag_end   = lme;
    ipart->l_exp_end   = lee;

    return(ipart);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MK_ENDPT - build and return a pointer to a new endpt */

PM_end_point *PM_mk_endpt(double rn, double rx, int dk, double rat,
			  double drn, double drx, PM_end_point *nxt)
   {PM_end_point *end;

    end       = CMAKE(PM_end_point);
    end->rn   = rn;
    end->rx   = rx;
    end->k    = 0;
    end->dk   = dk;
    end->rat  = rat;
    end->drn  = drn;
    end->drx  = drx;
    end->next = nxt;

    return(end);}

/*--------------------------------------------------------------------------*/

/*                           LINE FITTING ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* _PM_FIT_NEG - put in the fitting coefficients for the curve on the
 *             - negative branch of the quadratic
 *             - the negative branch is:
 *                             2
 *                    - 4cx + a x + 2c + ab       a
 *               - ---------------------------- - -
 *                                           2    2
 *                 2 sqrt(- 4cx(x-1) + (ax+b) )
 *
 */

static int _PM_fit_neg(PM_side *base, double a, double b, double c)
   {double t1s, t2s, t3s, t4s, t5s, t6s, t7s, t8s, t9s, t10s;
    double t11s, t12s, t13s, t14s, t15s, t16s, t17s, t18s, t19s, t20s;
    double t21s, t22s, t23s, t24s, t25s, t26s, t27s, t28s, t29s, t30s;
    double t31s, t32s, t33s, t34s, t35s, t36s, t37s, t38s, t39s, t40s;
    double x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11;
    double x12, x13, x14, x15, x16, x17, x18, x19, x20, x21;
    double x22, x23, x24, x25, x26, x27, x28, x29, x30, x31;

    t1s = b*b;
    t2s = -2.0*t1s;
    t3s = 4.0*a*b;
    t4s = 8.0*c;
    t5s = a*a;
    t6s = -2.0*t5s;
    t7s = -t5s;
    t8s = c*c;
    t9s = a*t5s;
    t10s = 8.0*t8s;
    t11s = a*t9s;
    t12s = -t9s;
    t13s = 4.0*c;
    t14s = b*t1s;
    t15s = t5s*t1s;
    t16s = 4.0*t3s;
    t17s = 16.0*t8s;
    t18s = 4.0*a*t1s;
    t19s = c*t8s;
    t20s = 4.0*a*t19s;
    t21s = -t15s;
    x1 = t2s;
    x2 = -t3s-t4s;
    x3 = t4s+t6s;
    x4 = t1s*(-2.0+t7s);
    x5 = -2.0*a*b*c;
    x6 = -2.0*t8s;
    x7 = t10s+c*(-8.0+t3s-4.0*t5s)-b*(4.0*a+2.0*t9s);
    x8 = -t10s-t11s+c*(8.0+6.0*t5s)+t6s;
    x9 = -b*t5s;
    x10 = -2.0*a*c;
    x11 = 4.0*a*c+t12s;
    x12 = x10+x9;
    x13 = t1s;
    t22s = sqrt(x13);
    x14 = 2.0*a*b+t13s;
    x15 = -t13s+t5s;
    x16 = x4+x5+x6;
    t23s = x16+x12*t22s;
    t24s = 1/x1;
    x17 = x13*x13;
    x18 = 4.0*a*t14s+8.0*c*t1s;
    x19 = 6.0*t15s+t17s+c*(t16s-8.0*t1s);
    x20 = c*(-t16s+8.0*t5s)-32.0*t8s+4.0*b*t9s;
    x21 = t11s+t17s-8.0*c*t5s;
    x22 = -b*c*(a*t14s+t15s);
    x23 = 0.5*b*c*x5;
    x24 = -2.0*t15s*c*(a+b)-t20s-(t18s+6.0*b*t5s)*t8s;
    x25 = -c*(b*t11s+a*t15s)+t20s+(t12s+t18s+4.0*b*t5s)*t8s;
    x26 = c*(-a*t14s+t21s);
    x27 = (-3.0*a*b+t2s)*t8s;
    x28 = c*x6;
    x29 = 4.0*t19s+(4.0*t1s+t3s+t7s)*t8s+c*(t21s-b*t9s);
    x30 = x26+x27+x28;
    t25s = x30*t22s;
    x31 = x22+x23;
    t26s = x31+t25s;
    t27s = sqrt(x13*t23s*t24s);
    t28s = x11+x12;
    t29s = x13+x14+x15;
    t30s = sqrt(t29s);
    t31s = t28s*t29s+x16*t30s;
    t32s = x17+x18+x19+x20+x21;
    t33s = x1+x2+x3;
    t34s = (x24+x25+x31+(x29+x30)*t30s)*t33s;
    t35s = x7+x8;
    t36s = x16+x7+x8+t28s*t30s;
    t37s = sqrt(t29s*t36s/t33s);
    t38s = x12*x13+x16*t22s;
    t39s = 1.0/(x1*x17*t32s*t33s*sqrt(x13*t23s*t29s*t36s/(x1*t33s)));
    t40s = t26s/(x17*t27s);

/* coefficients of hermite cubic fit: p(x) = c0+x*(c1+x*(c2+x*c3)) */
    base->c0 = sqrt(t23s*t24s);
    base->c1 = t40s;
    base->c2 = (-x1*x17*t27s*(-3.0*t31s*t32s+t34s-3.0*t30s*t32s*t35s)-
                t32s*t33s*t37s*
                (x1*(x31+t25s+x17*t40s*t27s)+3.0*x17*t38s))*t39s;
    base->c3 = (x1*x17*t27s*(-2.0*t31s*t32s+t34s-2.0*t30s*t32s*t35s)+
                t32s*t33s*t37s*(x1*t26s+2.0*x17*t38s))*t39s;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FIT_CURVE - compute the fitting coefficients to obtain a cubic
 *               - Hermite fit to the side
 */

static int _PM_fit_curve(PM_side *base)
   {double axx, ayy, axy, ax, ay;
    double x1, y1, x2, y2;
    double apxx, apyy, apxy, apy;
    double s, ssq, ct, ctsq, st, stsq;
    double a, b, c;
    double xx1, xx2, xx3, xx4;
    double ts2, ts3, ts4, ts5, ts6, ts7, ts8;
    PM_conic_curve *cur;

    x1 = base->x;
    y1 = base->y;
    x2 = base->next->x;
    y2 = base->next->y;
    cur = base->crve;

/*
 * shift so that (x1, y1) is at (0, 0) 
 * rotate so that (x2, y2) is at (s, 0)
 * rescale so that (x2, y2) is at (1, 0)
 *
 */
    ssq = (y1-y2)*(y1-y2)+(x1-x2)*(x1-x2);
    s = sqrt(ssq);
    base->scale = s;

    ct = (x2-x1)/s;
    ctsq = ct*ct;
    st = (y2-y1)/s;
    stsq = st*st;
    base->cosine = ct;
    base->sine = st;

/* under these transformations apc = 0, and apx = -apxx */
    axx = cur->xx;
    axy = cur->xy;
    ayy = cur->yy;
    ax = cur->x;
    ay = cur->y;

    apyy = ssq*(axx*stsq-axy*ct*st+ayy*ctsq);
    apxy = ssq*(-axy*stsq+(2*ayy-2*axx)*ct*st+axy*ctsq);
    apxx = ssq*(ayy*stsq+axy*ct*st+axx*ctsq);
    apy = s*((2*ayy*ct-axy*st)*y1+(axy*ct-2*axx*st)*x1-ax*st+ay*ct);
/*    apx = s*((2*ayy*st+axy*ct)*y1+(axy*st+2*axx*ct)*x1+ay*st+ax*ct); */

/* case checking on types of equations */
    if (apyy != 0.0)
       {a = apxy/apyy;
        b = apy/apyy;
        c = apxx/apyy;
        _PM_fit_neg(base, a, b, c);}
    else if (apxy == 0.0)
       {if (apy != 0.0)                                    /* y = a*x*(x-1) */
           {a = apxx/apy;
            a = a*a;
            b = sqrt(1.0+a);
            base->c0 = b;
            base->c1 = -2*a/b;
            base->c2 = -(base->c1);
            base->c1 = 0.0;}
        else                                                       /* y = 0 */
           {base->c0 = 1.0;
            base->c1 = 0.0;
            base->c2 = 0.0;
            base->c3 = 0.0;};}
    else if ((-apy/apxy < 0.0) || (apxy < -apy))     /* y = a*x*(x-1)/(x+b) */
       {a = apxx*apxx;
        b = apxy;
        c = apy;
        xx4 = sqrt(a+c*c);
        xx3 = b+c;
        xx2 = xx3*xx3;
        xx1 = xx2*xx2;
        ts2 = 1.0/(c*ABS(c)*xx4);
        ts3 = xx4/c;
        ts4 = sqrt(a*xx2+xx1);
        ts5 = ts4/sqrt(xx1);
        ts6 = a*c*sqrt(xx1)/(xx2*ts4*xx3);
        ts7 = 2.0*(ts5-ts6);
        ts8 = 2.0*a*xx3*ts2;
        base->c0 = ts3;
        base->c1 = -ts8;
        base->c2 = 2.0*ts8+ts7-3.0*ts3+ts5;
        base->c3 = -ts8-2.0*ts3-ts7;}
    else
       PM_MESH_ERR(TRUE);

    base->c1 /= 2.0;
    base->c2 /= 3.0;
    base->c3 /= 4.0;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_ARC_LEN - do the arc length integration using the fitting information
 *             - stored in the curve
 */

static double _PM_arc_len(PM_side *base, double x, double y)
   {double t, x1, y1, c0, c1, c2, c3;
    double s, ct, st;

    c0 = base->c0;
    c1 = base->c1;
    c2 = base->c2;
    c3 = base->c3;

    x1 = base->x;
    y1 = base->y;

    s  = base->scale;
    ct = base->cosine;
    st = base->sine;

    t = ((x-x1)*ct - (y-y1)*st)/s;

    return(t*(c0+t*(c1+t*(c2+t*c3))));}

/*--------------------------------------------------------------------------*/

/*                            MESH MAKER ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PM_EXIST_CP - return a pointer to a curve with the given coefficients
 *              - return NULL if none exists
 */

static PM_conic_curve *_PM_exist_cp(double axx, double ayy, double axy,
				    double ax, double ay, double ac,
				    hasharr *curves)
   {long i;
    PM_conic_curve *cp;

    if (curves != NULL)
       {for (i = 0; SC_hasharr_next(curves, &i, NULL, NULL, (void **) &cp); i++)

/* allocate the scalars now */
	    {if (SAME(cp->xx, cp->yy, axx, ayy) &&
		 SAME(cp->xy, cp->c, axy, ac) &&
		 SAME(cp->x, cp->y, ax, ay))
	        return(cp);};};

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* _PM_NXT_NAME - generate a name for a curve */
 
static char *_PM_nxt_name(void)
   {char s[10];
    char *rv;
 
    snprintf(s, 10, "#C%06d", _PM.ncv++);

    rv = CSTRSAVE(s);
 
    return(rv);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_CHCK_CURVES - check that all sides have non-null curves
 *                 - if the curve entry is null compute a straight
 *                 - line curve to the next point and install it as
 *                 - the curve entry for the side
 */

static int _PM_chck_curves(PM_part *ipart, hasharr *curves)
   {PM_conic_curve *cp;
    PM_side *ib;
    char *s;
    double x1, y1, x2, y2, dx, dy, ax, ay, ac;

    for (ib = ipart->leg; TRUE; )
        {if (ib->crve == NULL)
            {x1 = ib->x;
             y1 = ib->y;
             x2 = ib->next->x;
             y2 = ib->next->y;
             dx = x2 - x1;
             dy = y2 - y1;
             if (dx == 0.0)
                {ay = 0.0;
                 ax = 1.0;
                 ac = -(ib->x);}
             else
                {ay = 1.0;
                 ax = dy/dx;
                 ac = -y1 + ax*x1;};

             cp = _PM_exist_cp(0.0, 0.0, 0.0, ax, ay, ac, curves);
             if (cp == NULL)
                {s = _PM_nxt_name();
                 cp = PM_mk_cline(s, 0.0, 0.0, 0.0, ax, ay, ac);
                 SC_hasharr_install(curves, s, cp, "PM_conic_curve", 3, -1);};
             ib->crve = cp;};

         _PM_fit_curve(ib);

         ib = ib->next;
         if (ib == ipart->leg)
            break;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SPLIT_SIDE - divide the current side at the given point */

static int _PM_split_side(PM_part *prt, PM_side *base, double xi, double yi)
   {PM_side *ib;

    if (!SAME(base->x, base->y, xi, yi) &&
        !SAME(base->next->x, base->next->y, xi, yi))
       {(prt->n_sides)++;

/* use mk_side because it initializes all members of the new side */
        ib = PM_mk_side(base->crve, base->dir, xi, yi, base->dk, base->dl,
                        prt->k_ratio, prt->l_ratio,
                        base->side_rat, base->min_dr_f, base->max_dr_f,
                        base->mag_start, base->exp_start,
                        base->mag_end, base->exp_end,
                        NULL, base->next);

/* reset dk and dl members for MATCH_INT and MATCH_SIDE */
        ib->dk = 0;
        ib->dl = 0;

/* the current side must point to the new side */
        base->next = ib;

/* get the curve members of both sides current */
        _PM_fit_curve(base);
        _PM_fit_curve(ib);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SAME_CP - returns TRUE iff the coefficients of the two curves
 *             - are the same
 */

static int _PM_same_cp(PM_conic_curve *curvea, PM_conic_curve *curveb)
   {int rv;

    rv = SAME(curvea->xx, curvea->yy, curveb->xx, curveb->yy) &&
         SAME(curvea->xy, curvea->c, curveb->xy, curveb->c) &&
         SAME(curvea->x, curvea->y, curveb->x, curveb->y);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_WHEREIN - return a value for the curve parameter, t,  such that
 *             - at base->X  t = 0 and at base->next->X  t = 1
 */

double _PM_wherein(PM_side *base, double x, double y)
   {double ls, lx;

    ls = _PM_arc_len(base, base->next->x, base->next->y);
    lx = _PM_arc_len(base, x, y);

    return(lx/ls);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SEG_ORIENT - returns PARLLL (TRUE) iff the directed segments have the
 *                - same orientation with respect to their shared curve
 *                - else returns OPPST (FALSE)
 */

static int _PM_seg_orient(PM_side *basea, PM_side *baseb)
   {int rv;

    rv = (_PM_wherein(basea, baseb->x, baseb->y) <
	  _PM_wherein(basea, baseb->next->x, baseb->next->y)) ?
	 PARLLL : OPPST;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_INTER_PTS - return the ratio of the distances
 *               - s23/s13 as measured along the curve
 *               - to be used as the fractional number of points
 *               - between X2 and X3
 */

float _PM_inter_pts(PM_side *basea, PM_side *baseb, PM_side *basec)
   {double r, np1, f, fr;
    double s12, s23, s13;

    r = basea->ratio;
    np1 = max(basea->dk, basea->dl) + 1;
    s13 = _PM_arc_len(basea, basec->x, basec->y);
    s12 = _PM_arc_len(basea, baseb->x, baseb->y);
    s23 = s13 - s12;
    if (r < (1.0 - TOLERANCE))
       {f = s13/s12;
        fr = np1 - log(1.0 - (1.0 - pow(r, np1))*f)/log(r);}
    else if (r > (1.0 + TOLERANCE))
       {f = s13/s23;
        r = 1.0/r;
        fr = log(1.0 - (1.0 - pow(r, np1))*f)/log(r) - 1.0;}
    else
       fr = s23/s13;

    return(fr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MATCH_INT - compute the logical intervals for the newly split sides */

static int _PM_match_int(int type, PM_side *basea, PM_side *baseb)
   {PM_side *ib;
    double n, na, nb, nc;
    double dk, dka, dkb;
    double dl, dla, dlb;

/* the original basea properly contained baseb and once split must match
 * both left, middle, and right pieces
 */
    if (type == CONTAINED)
       {for (ib = baseb; ib->next != baseb; ib = ib->next);
        baseb->dk = - basea->dk;
        baseb->dl = - basea->dl;

        na = 1.0 - _PM_inter_pts(ib, baseb, baseb->next->next);
        nc = _PM_inter_pts(ib, baseb->next, baseb->next->next);
        nb = 1.0 - na - nc;

        dk = ib->dk;
        dka = na*dk;
        dkb = nb*dk;

        ib->dk          = dka + (dkb - baseb->dk)*na/(na+nc);
        baseb->next->dk = dk - ib->dk - baseb->dk;

        dl  = ib->dl;
        dla = na*dl;
        dlb = nb*dl;

        ib->dl          = dla + (dlb - baseb->dl)*na/(na+nc);
        baseb->next->dl = dl - ib->dl - baseb->dl;

        PM_MESH_ERR((baseb->dk == 0) && (baseb->dl == 0));

        PM_MESH_ERR((baseb->next->dk == 0) && (baseb->next->dl == 0));}

/* the original basea and baseb overlapped and only have to match with
 * the left or right neighbor
 */
    else if (type == INTERSECT)

/* if basea's downwind neighbor is new fill it up */
       {if ((basea->next->dk == 0) && (basea->next->dl == 0))

/* if basea's downwind neighbor matches baseb */
           {if (SAME(basea->next->next->x, basea->next->next->y,
                     baseb->x, baseb->y))
               {basea->next->dk = - baseb->dk;
                basea->next->dl = - baseb->dl;}

/* if basea matches baseb */
            else
               {n   = _PM_inter_pts(basea, basea->next, basea->next->next);
                dka = (double) basea->dk;
                dla = (double) basea->dl;
                if (basea->dk > 0)
                   basea->next->dk = (int) (max(n*dka, 1.0) + 0.4);
                else if (basea->dk < 0)
                   basea->next->dk = (int) (min(n*dka, -1.0) - 0.4);
                if (basea->dl > 0)
                   basea->next->dl = (int) (max(n*dla, 1.0) + 0.4);
                else if (basea->dl < 0)
                   basea->next->dl = (int) (min(n*dla, -1.0) - 0.4);};

            basea->dk = basea->dk - basea->next->dk;
            basea->dl = basea->dl - basea->next->dl;

            PM_MESH_ERR((basea->dk == 0) && (basea->dl == 0));

            PM_MESH_ERR((basea->next->dk == 0) && (basea->next->dl == 0));};

/* if baseb's downwind neighbor is new fill it up */
        if ((baseb->next->dk == 0) && (baseb->next->dl == 0))

/* if baseb's downwind neighbor matches basea */
           {if (SAME(baseb->next->next->x, baseb->next->next->y,
                     basea->x, basea->y))
               {baseb->next->dk = - basea->dk;
                baseb->next->dl = - basea->dl;}

/* if baseb matches basea */
            else
               {n = _PM_inter_pts(baseb, baseb->next, baseb->next->next);
                dkb = (double) baseb->dk;
                dlb = (double) baseb->dl;
                if (baseb->dk > 0)
                   baseb->next->dk = (int) (max(n*dkb, 1.0) + 0.4);
                else if (baseb->dk < 0)
                   baseb->next->dk = (int) (min(n*dkb, -1.0) - 0.4);
                if (baseb->dl > 0)
                   baseb->next->dl = (int) (max(n*dlb, 1.0) + 0.4);
                else if (baseb->dl < 0)
                   baseb->next->dl = (int) (min(n*dlb, -1.0) - 0.4);};

            baseb->dk = baseb->dk - baseb->next->dk;
            baseb->dl = baseb->dl - baseb->next->dl;

            PM_MESH_ERR((baseb->dk == 0) && (baseb->dl == 0));

            PM_MESH_ERR((baseb->next->dk == 0) && (baseb->next->dl == 0));};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_CROSS_CP - return TRUE if the segments cross */

static int _PM_cross_cp(PM_side *basea, PM_side *baseb)
   {

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MATCH - match part boundaries and split those that have intersections
 *           - with other part boundaries in the middle
 */

static int _PM_match(PM_part *parta, PM_part *partb,
		     PM_side *basea, PM_side *baseb)
   {double x1, x2, x3, x4;
    double y1, y2, y3, y4;
    double t, s;
    PM_conic_curve *curvea, *curveb;

/* because parta and partb may be the same part (a part may wrap around
 * and touch itself) eliminate the case of a side matching itself
 */
    if (basea == baseb)
       return(TRUE);

    curvea = basea->crve;
    curveb = baseb->crve;

    x1 = basea->x;
    y1 = basea->y;
    x2 = basea->next->x;
    y2 = basea->next->y;
    x3 = baseb->x;
    y3 = baseb->y;
    x4 = baseb->next->x;
    y4 = baseb->next->y;

    if (_PM_same_cp(curvea, curveb))
       {if (SAME(x2, y2, x3, y3) && SAME(x1, y1, x4, y4))
           {MATCH_SIDE(basea, baseb);
            return(TRUE);};

        s = _PM_wherein(basea, x3, y3);
        t = _PM_wherein(basea, x4, y4);

/* case (  I) 1-4-2-3 */
        if ((-1.0e-10 <= t) && (t <= 0.9999999999) && (0.9999999999 < s))
           {CHCK_ORIENT;
            _PM_split_side(parta, basea, x4, y4);
            _PM_split_side(partb, baseb, x2, y2);
            _PM_match_int(INTERSECT, basea, baseb);
            if (!SAME(x1, y1, x4, y4))
               ADVANCE(basea);
            if (!SAME(x2, y2, x3, y3))
               ADVANCE(baseb);
            MATCH_SIDE(basea, baseb);}

/* case ( II) 4-1-3-2 */
        else if ((t < 1.0e-10) && (1.0e-10 <= s) && (s <= 1.0000000001))
           {CHCK_ORIENT;
            _PM_split_side(parta, basea, x3, y3);
            _PM_split_side(partb, baseb, x1, y1);
            _PM_match_int(INTERSECT, baseb, basea);
            MATCH_SIDE(basea, baseb);
            if (!SAME(x1, y1, x4, y4))
               ADVANCE(basea);
            if (!SAME(x2, y2, x3, y3))
               ADVANCE(baseb);}

/* case (III) 1-4-3-2 */
        else if ((1.0e-10 <= t) && (t <= 0.9999999999) &&
                 (1.0e-10 <= s) && (s <= 0.9999999999))
           {CHCK_ORIENT;
            _PM_split_side(parta, basea, x4, y4);
            ADVANCE(basea);
            _PM_split_side(parta, basea, x3, y3);
            _PM_match_int(CONTAINED, baseb, basea);
            MATCH_SIDE(basea, baseb);
            ADVANCE(basea);}

/* case ( IV) 4-1-2-3 */
        else if ((-1.0e-10 > t) && (1.0000000001 < s))
           {CHCK_ORIENT;
            _PM_split_side(partb, baseb, x2, y2);
            ADVANCE(baseb);
            _PM_split_side(partb, baseb, x1, y1);
            _PM_match_int(CONTAINED, basea, baseb);
            MATCH_SIDE(basea, baseb);
            ADVANCE(baseb);};
        return(TRUE);}

    else if (_PM_cross_cp(basea, baseb))
       PM_MESH_ERR(TRUE);

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COMPLETE - reduce the part boundaries to a set of segments that
 *              - do not have intersections with other part boundaries
 *              - except at the endpoints
 */

static int _PM_complete(PM_part *parta, PM_part *partb)
   {PM_side *basea, *baseb;

    basea = parta->leg;
    baseb = partb->leg;

    while (TRUE)
       {while (TRUE)
           {_PM_match(parta, partb, basea, baseb);
            baseb = baseb->next;
            if (baseb == partb->leg)
               break;};

        basea = basea->next;
        if (basea == parta->leg)
           break;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FILL_TREE - from the given base traverse the parts list as a tree
 *               - and fill in the logical coordinates of the boundary
 *               - segments endpoints
 */

static int _PM_fill_tree(PM_side *base, PM_mesh *mesh)
   {int kmin, kmax, lmin, lmax;
    PM_side *ib, *mbase, *nxt;

    kmin = mesh->kmin;
    kmax = mesh->kmax;
    lmin = mesh->lmin;
    lmax = mesh->lmax;

    nxt = NULL;
    for (ib = base; nxt != base; ib = nxt)
        {nxt = ib->next;

	 nxt->k = ib->k + ib->dk;
	 nxt->l = ib->l + ib->dl;

	 ib->fill = TRUE;

/* recurse over an unfilled matching/opposing side */
	 mbase = ib->match;
	 if ((mbase != NULL) && !mbase->fill)
            {mbase->k = nxt->k;
	     mbase->l = nxt->l;

	     mesh->kmin = kmin;
	     mesh->kmax = kmax;
	     mesh->lmin = lmin;
	     mesh->lmax = lmax;

	     _PM_fill_tree(mbase, mesh);

	     kmin = mesh->kmin;
	     kmax = mesh->kmax;
	     lmin = mesh->lmin;
	     lmax = mesh->lmax;};

	 kmax = max(kmax, nxt->k);
	 kmin = min(kmin, nxt->k);
	 lmax = max(lmax, nxt->l);
	 lmin = min(lmin, nxt->l);};

    mesh->kmin = kmin;
    mesh->kmax = kmax;
    mesh->lmin = lmin;
    mesh->lmax = lmax;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_CHCK_PART - check that sum of dk and dl separately vanish for
 *               - each part
 */

int _PM_chck_part(PM_part *parts)
   {PM_part *parta;
    PM_side *base;
    int dk, dl;

    for (parta = parts; parta != NULL; parta = parta->next)
        {base = parta->leg;
         dk = 0;
         dl = 0;
         while (TRUE)
            {dk = dk + base->next->k - base->k;
             dl = dl + base->next->l - base->l;
             base = base->next;
             if (base == parta->leg)
                break;};
         PM_MESH_ERR(dk != 0);

         PM_MESH_ERR(dl != 0);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COMP_RATIO - compute the ratio for an equal ratio splitting of a segment
 *                - given the number of intervals and the first interval size
 */

static double _PM_comp_ratio(double b0, int nz)
   {int i;
    double r0, r1, rN, b, db, rN1, dr, nzd;

    nzd = (double) nz;
    db  = HUGE;

/* for large ratios use this method */
    if (b0 > 3.0)
       {r0 = pow(b0, 1.0/(nzd - 1.0));
        r0 = pow(b0*(r0 - 1.0) + 1.0, 1.0/nzd);

        b = b0;
        for (i = 0; i < 10; i++)
            {rN1 = pow(r0, (nzd - 1.0));
             b   = (r0*rN1 - 1.0)*(r0 - 1.0) - b0*(r0 - 1.0)*(r0 - 1.0);
             dr  = (nzd - 1.0)*r0*rN1 - nzd*rN1 + 1.0;
             db  = b/dr;
             if (dr != 0.0)
                r0 -= db;
             else
                r0 = 1.0;};}

/* for moderate ratios use this method */
    else if (b0 <= 3.0)
       {r0  = 1.0 - b0;
        if (r0 != 1.0)
           {for (i = 0; i < 20; i++)
                {r1 = 1.0 - r0;
                 rN = 1.0 - pow(r0, nzd);
                 b  = r1/rN;
                 db = b0 - b;
                 if (ABS(db) < 0.0001)
                    break;

                 rN1 = 1.0 - pow(r0, (nzd-1.0));
                 dr  = db*rN*rN/((nzd-1.0)*rN - nzd*rN1);
                 r0 += dr;};};}

/* this method's range of validity is unknown - keep it anyway */
    else
       {r0  = b0/nz;
        nzd = (double) nz;
        if (r0 != 1.0)
           {for (i = 0; i < 20; i++)
                {r1 = 1.0 - r0;
                 rN = 1.0 - pow(r0, nzd);
                 b  = rN/r1;
                 db = b0 - b;
                 if (ABS(db) < 0.005)
                    break;

                 rN1 = 1.0 - pow(r0, (nzd-1.0));
                 dr  = db*r1*r1/(nzd*rN1 - (nzd-1.0)*rN);
                 r0 += dr;};};};

    PM_MESH_ERR((ABS(db) > 0.01) || (r0 == 1.0));

    return(r0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COMPUTE_RATIOS - find the k and l ratios along the given side in
 *                    - complete generality
 */

static void _PM_compute_ratios(double *fr, PM_side *base, PM_mesh *mesh)
   {int i, j, n, dk, dl, kbnd;
    double t, sdl, sdk, dkdl, dke, dldk, dle;
    double *kra, *lra;

    kbnd = mesh->kmax + 1;
    kra  = mesh->kra;
    lra  = mesh->lra;

    dk = base->dk;
    dl = base->dl;

/* if this a logically vertical side */
    if (dk == 0)
       {sdl = abs(dl)/dl;
        dl  = sdl*dl;
        for (j = 1; j < dl; j++)
            {i = NODE_OF(base->k, base->l + sdl*j);
             t = (fr[j+1] - fr[j])/(fr[j] - fr[j-1]);
             if (sdl > 0)
                lra[i] = t;
             else
                lra[i] = 1.0/t;};

        return;}

/* if this a logically horizontal side */
    else if (dl == 0)
       {sdk = abs(dk)/dk;
        dk  = sdk*dk;
        for (j = 1; j < dk; j++)
            {i = NODE_OF(base->k + sdk*j, base->l);
             t = (fr[j+1] - fr[j])/(fr[j] - fr[j-1]);
             if (sdk > 0)
                kra[i] = t;
             else
                kra[i] = 1.0/t;};

        return;}

/* if this side has an angle of 45 degrees with k axis */
    else if (dk == dl)
       {sdl = abs(dl)/dl;
        dl  = sdl*dl;
        for (j = 0; j < dl; j++)
            {i = NODE_OF(base->k + sdl*j, base->l + sdl*j);
             t = (fr[j+1] - fr[j])/(fr[j] - fr[j-1]);
             if (sdl > 0)
                lra[i] = t;
             else
                lra[i] = 1.0/t;

             kra[i] = lra[i];};

        return;};

    sdk = abs(dk)/dk;
    dk  = sdk*dk;
    sdl = abs(dl)/dl;
    dl  = sdl*dl;

/* if this side has an angle of less than 45 degrees with k axis */
    if (dk > dl)
       {dkdl = ((dk+1) % (dl+1));
        dke  = 0;
        for (j = 0; j <= dl; j++)
            {for (n = 0; n < dkdl; n++)
                 {i = NODE_OF(base->k + sdk*(j*dkdl + n + dke),
                              base->l + sdl*j);
                  t = (fr[j+1] - fr[j])/(fr[j] - fr[j-1]);

                  if (sdk > 0)
                     kra[i] = t;
                  else
                     kra[i] = 1.0/t;

                  if (sdl > 0)
                     lra[i] = t;
                  else
                     lra[i] = 1.0/t;};};}

/* if the side makes an angle of greater the 45 degrees with the k axis */
    else if (dl > dk)
       {dldk = ((dl+1) % (dk+1));
        dle  = 0;
        for (j = 0; j <= dk; j++)
            {for (n = 0; n < dldk; n++)
                 {i = NODE_OF(base->k + sdk*j,
                              base->l + sdl*(j*dldk + n + dle));
                  t = (fr[j+1] - fr[j])/(fr[j] - fr[j-1]);

                  if (sdk > 0)
                     kra[i] = t;
                  else
                     kra[i] = 1.0/t;

                  if (sdl > 0)
                     lra[i] = t;
                  else
                     lra[i] = 1.0/t;};};}

     else
        PM_MESH_ERR(TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COMPUTE_FRACTIONS - compute the spacing of nodes along a side as
 *                       - fractions of the total arc length
 */
   
static void _PM_compute_fractions(double *fr, PM_side *base, int n,
				  PM_mesh *mesh)
   {int i, m;
    double drfn, drfx, srat, d, r, t;

    r    = base->ratio;
    srat = base->side_rat;
    drfn = base->min_dr_f;
    drfx = base->max_dr_f;

    if ((drfn > 0.0) && (drfx > 0.0))
       {m = n/2;
        if (2*m == n)
           {d  = 2.0*drfn;
            r  = _PM_comp_ratio(d, m);
            d *= 0.5;
            for (fr[0] = 0.0, i = 1; i < m; d *= r, i++)
                fr[i] = fr[i-1] + d;
            t = fr[i-1] + d;

            d  = 2.0*drfx;
            r  = _PM_comp_ratio(d, m);
            d *= 0.5;
            m  = n - m;
            for (fr[n] = 1.0, i = n - 1; i > m; d *= r, i--)
                fr[i] = fr[i+1] - d;
            fr[i] = 0.5*(t + fr[i+1] - d);}
        else
           {d  = 2.0*drfn;
            r  = _PM_comp_ratio(d, m);
            d *= 0.5;
            for (fr[0] = 0.0, i = 1; i < m; d *= r, i++)
                fr[i] = fr[i-1] + d;

            d  = 2.0*drfx;
            r  = _PM_comp_ratio(d, m);
            d *= 0.5;
            m  = n - m;
            for (fr[n] = 1.0, i = n - 1; i > m; d *= r, i--)
                fr[i] = fr[i+1] - d;};}

    else if (drfn > 0.0)
       {d = drfn;
        r = _PM_comp_ratio(d, n);
        for (fr[0] = 0.0, i = 1; i <= n; d *= r, i++)
           fr[i] = fr[i-1] + d;}

    else if (drfx > 0.0)
       {d = drfx;
        r = _PM_comp_ratio(d, n);
        for (fr[n] = 1.0, i = n - 1; i >= 0; d *= r, i--)
            fr[i] = fr[i+1] - d;}

    else if ((srat > 0.0) && (srat != 1.0))
       {r = srat;
        d = (1.0 - r)/(1.0 - pow(r, (double) n));
        for (fr[0] = 0.0, i = 1; i <= n; d *= r, i++)
            fr[i] = fr[i-1] + d;}

    else if ((r != 1.0) && (srat == 0.0))
       {d = (1.0 - r)/(1.0 - pow(r, (double) n));
        for (fr[0] = 0.0, i = 1; i <= n; d *= r, i++)
            fr[i] = fr[i-1] + d;}

    else
       {d = 1.0/((double) n);
        for (fr[0] = 0.0, i = 1; i <= n; i++)
            fr[i] = fr[i-1] + d;}

/* adjust the ratios */
    d = 1.0/(fr[n] - fr[0]);
    for (i = 1; i <= n; i++)
        fr[i] = d*(fr[i] - fr[0]);

    _PM_compute_ratios(fr, base, mesh);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FIND_POINTS - find the points whose locations are the fractions, f, of
 *                 - the arc length t along the curve defined by the points
 *                 - (xp, yp)
 */

static void _PM_find_points(double *xp, double *yp, PM_mesh *mesh,
			    int m, double t, double *f, int *indx, int n,
			    PM_conic_curve *crv)
   {int i, j, l;
    double p, ps, ds, x1, x2, y1, y2, dx, dy, rt;
    double *rx, *ry;

    rx = mesh->rx;
    ry = mesh->ry;

    ps = 0;
    x1 = xp[0];
    y1 = yp[0];
    l  = 1;
    for (i = 0; i < n; i++)
        {j = indx[i];
         p = f[i]*t;
         for (; l < m; l++)
             {x2 = xp[l];
              y2 = yp[l];
              dx = x2 - x1;
              dy = y2 - y1;
              ds = sqrt(dx*dx + dy*dy);
              if ((ps <= p) && (p <= (ps+ds)))
                 {rt    = (p - ps)/ds;
                  rx[j] = x1 + rt*(x2 - x1);
                  ry[j] = y1 + rt*(y2 - y1);
                  break;};
              ps += ds;
              x1  = x2;
              y1  = y2;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_COMPUTE_POINTS - given the endpoints of a segment of a quadratic line
 *                    - and a set of distance fractions from the first
 *                    - to the second point, return fill the coordinate of
 *                    - the requested points
 */

static void _PM_compute_points(PM_mesh *mesh, double x1, double y1,
			       double x2, double y2, double *f,
			       int *indx, int n, int dir,
			       PM_conic_curve *crv)
   {int i, j, m;
    double axx, ayy, axy, ax, ay, ac, iax, iay;
    double x, y, t, dt, tx, ty, ds, dsa;
    double bx, by, cx, cy, sg;
    double *xp, *yp, *rx, *ry;

    rx = mesh->rx;
    ry = mesh->ry;

    axx = crv->xx;
    ayy = crv->yy;
    axy = crv->xy;
    ay  = crv->y;
    ax  = crv->x;
    ac  = crv->c;

    if ((axx == 0.0) && (ayy == 0.0) && (axy == 0.0))
       {for (i = 0; i < n; i++)
            {j     = indx[i];
             rx[j] = x1 + f[i]*(x2 - x1);
             ry[j] = y1 + f[i]*(y2 - y1);};}
    else
       {m  = 40*n;
        xp = CMAKE_N(double, m);
        yp = CMAKE_N(double, m);

        dt = 3.0*sqrt(ABS(ac/(axx*axx + ayy*ayy + axy*axy)))/(10.0*n);
        if (dt == 0.0)
           dt = 3.0*sqrt((ax*ax + ay*ay)/(axx*axx + ayy*ayy + axy*axy))/
                (10.0*n);
             
        iax = 1.0/axx;
        iay = 1.0/ayy;

        x = x1;
        y = y1;
        t = 0.0;
        for (i = 0; i < m; i++)
            {ty = 2.0*axx*x + axy*y + ax;
             tx = 2.0*ayy*y + axy*x + ay;

             xp[i] = x;
             yp[i] = y;
             if (ABS(ty) > ABS(tx))
                {sg = dir*dt/sqrt(1.0 + tx*tx/(ty*ty));
                 if (ty >= 0.0)
                    {y += sg;
                     sg = 1.0;}
                 else
                    {y -= sg;
                     sg = -1.0;};

                 if (axx != 0.0)
                    {bx = 0.5*(axy*y + ax)*iax;
                     cx = ((ayy*y + ay)*y + ac)*iax;
                     ds = sqrt(bx*bx - cx);
                     x  = -bx + sg*ds;}
                 else
                    {bx = axy*y + ax;
                     cx = (ayy*y + ay)*y + ac;
                     x  = -cx/bx;};}
             else
                {sg = dir*dt/sqrt(1.0 + ty*ty/(tx*tx));
                 if (tx >= 0.0)
                    {x -= sg;
                     sg = 1.0;}
                 else
                    {x += sg;
                     sg = -1.0;};

                 if (ayy != 0.0)
                    {by = 0.5*(axy*x + ay)*iay;
                     cy = ((axx*x + ax)*x + ac)*iay;
                     ds = sqrt(by*by - cy);
                     y  = -by + sg*ds;}
                 else
                    {by = axy*x + ay;
                     cy = (axx*x + ax)*x + ac;
                     y  = -cy/by;};};

             tx  = x - x2;
             ty  = y - y2;
             dsa = sqrt(tx*tx + ty*ty);

             tx = x - xp[i];
             ty = y - yp[i];
             ds = sqrt(tx*tx + ty*ty);

             if (dsa <= ds)
                {tx  = xp[i] - x2;
                 ty  = yp[i] - y2;
                 dsa = sqrt(tx*tx + ty*ty);
                 i++;
                 xp[i] = x2;
                 yp[i] = y2;
                 t    += dsa;
                 break;};
                
             t += ds;};

        _PM_find_points(xp, yp, mesh, i+1, t, f, indx, n, crv);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_REG_BASE - return the region number associated with the given side */

int PM_reg_base(PM_part *parts, PM_side *base)
   {PM_part *ipart;
    PM_side *ib;

    for (ipart = parts; ipart; ipart = ipart->next)
        {ib = ipart->leg;
         while (TRUE)
            {if (ib == base)
                return(ipart->reg);

             ib = ib->next;
             if (ib == ipart->leg)
                break;};};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MAP_BASE - fill in the physical coordinates for the given side */

static int _PM_map_base(PM_part *parts, PM_side *base, PM_mesh *mesh)
   {int i, j, n, l, dk, dl, kbnd;
    int sdk, sdl, dkdl, dldk, dke, dle, m, m1, bnd_fl;
    int indx[1000], *reg_map;
    double t[1000], *nodet;

    reg_map = mesh->reg_map;
    nodet   = mesh->nodet;
    kbnd    = mesh->kmax + 1;

    dk = base->dk;
    dl = base->dl;
    m  = PM_reg_base(parts, base);

/* if the material on the matching side is different then nodet[i]
 * will be set to 2.0 otherwise it will be 1.0
 */
    if (base->match != NULL)
       m1 = PM_reg_base(parts, base->match);
    else
       m1 = 0;

    if ((m != m1) || (m1 == 0))
       bnd_fl = TRUE;
    else
       bnd_fl = FALSE;

    n = sqrt((double) (dk*dk + dl*dl));
    _PM_compute_fractions(t, base, n, mesh);

/* if this a logically vertical side */
    if (dk == 0)
       {sdl = abs(dl)/dl;
        dl  = sdl*dl;
        for (j = 0; j <= dl; j++)
            {i = indx[j] = NODE_OF(base->k, base->l + sdl*j);

/* the j != 0 test is essential only for the bottom of an ascending side */
             if ((sdl > 0) && (j != 0))
                reg_map[i] = m;

             if (bnd_fl)
                nodet[i] = 2.0;
             else if (nodet[i] != 2.0)
                nodet[i] = 1.0;};

        _PM_compute_points(mesh, base->x, base->y,
			   base->next->x, base->next->y,
			   t, indx, dl+1, base->dir, base->crve);
        return(TRUE);}

/* if this a logically horizontal side */
    else if (dl == 0)
       {sdk = abs(dk)/dk;
        dk  = sdk*dk;
        for (j = 0; j < dk; j++)
            {i = indx[j] = NODE_OF(base->k + sdk*j, base->l);

             if (sdk < 0)
                reg_map[i] = m;

             if (bnd_fl)
                nodet[i] = 2.0;
             else if (nodet[i] != 2.0)
                nodet[i] = 1.0;};

/* to get the corners right it is necessary to check which direction
 * the next leg is heading and fill in the region map accordingly
 */
        if ((sdk < 0) && (base->next->dl >= 0))
            {i = indx[j] = NODE_OF(base->k + sdk*j, base->l);
             reg_map[i] = m;
             if (bnd_fl)
                nodet[i] = 2.0;
             else if (nodet[i] != 2.0)
                nodet[i] = 1.0;};

        _PM_compute_points(mesh, base->x, base->y,
			   base->next->x, base->next->y,
			   t, indx, dk, base->dir, base->crve);
        return(TRUE);}

/* if this side has an angle of 45 degrees with k axis */
    else if (dk == dl)
       {sdl = abs(dl)/dl;
        dl  = sdl*dl;
        for (j = 0; j < dl; j++)
            {i = indx[j] = NODE_OF(base->k + sdl*j, base->l + sdl*j);

             if (sdl > 0)
                reg_map[i] = m;
             if (bnd_fl)
                nodet[i] = 2.0;
             else if (nodet[i] != 2.0)
                nodet[i] = 1.0;};

        _PM_compute_points(mesh, base->x, base->y,
			   base->next->x, base->next->y,
			   t, indx, dl, base->dir, base->crve);
        return(TRUE);};

    sdk = abs(dk)/dk;
    dk  = sdk*dk;
    sdl = abs(dl)/dl;
    dl  = sdl*dl;

/* if this side has an angle of less than 45 degrees with k axis */
    if (dk > dl)
       {dkdl = ((dk+1) % (dl+1));
        dke  = 0;
        l    = 0;
        for (j = 0; j <= dl; j++)
            {for (n = 0; n < dkdl; n++)
                 {i = indx[l] = NODE_OF(base->k + sdk*(j*dkdl + n + dke),
                                        base->l + sdl*j);
                  l++;

                  if (sdk < 0)
                     reg_map[i] = m;
                  if (bnd_fl)
                     nodet[i] = 2.0;
                  else if (nodet[i] != 2.0)
                     nodet[i] = 1.0;};
             if (dk > dkdl*dl)
                {i = NODE_OF(base->k + sdk*(j*dkdl + n), base->l + sdl*j);
                 if (sdk < 0)
                    reg_map[i] = m;
                 if (bnd_fl)
                    nodet[i] = 2.0;
                 else if (nodet[i] != 2.0)
                    nodet[i] = 1.0;
                 dke++;
                 dk--;};};

        _PM_compute_points(mesh, base->x, base->y,
			   base->next->x, base->next->y,
			   t, indx, l, base->dir, base->crve);}

/* if the side makes an angle of greater the 45 degrees with the k axis */
    else if (dl > dk)
       {dldk = ((dl+1) % (dk+1));
        dle  = 0;
        l    = 0;
        for (j = 0; j <= dk; j++)
            {for (n = 0; n < dldk; n++)
                 {i = indx[l] = NODE_OF(base->k + sdk*j,
                                        base->l + sdl*(j*dldk + n + dle));
                  l++;

/* the j != 0 test is essential only for the bottom of an ascending side */
                  if ((sdl > 0) && ((j != 0) || (n != 0)))
                     reg_map[i] = m;
                  if (bnd_fl)
		     {if (nodet[i] != 2.0)
			 nodet[i] = 2.0;
		      else
			 nodet[i] = 1.0;};};

             if (dl > dldk*dk)
                {i = indx[l] = NODE_OF(base->k + sdk*(j*dldk + n),
                                       base->l + sdl*j);
		 if (l > 0)
		    t[l] = t[l-1];
		 else
		    t[0] = 0.0;
                 l++;
                 if (sdl > 0)
                    reg_map[i] = m;
                 if (bnd_fl)
                    nodet[i] = 2.0;
                 else if (nodet[i] != 2.0)
                    nodet[i] = 1.0;
                 dle++;
                 dl--;};};

        _PM_compute_points(mesh, base->x, base->y,
			   base->next->x, base->next->y,
			   t, indx, l, base->dir, base->crve);}
     else
        PM_MESH_ERR(TRUE);

     return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MAP_TREE - from the given base traverse the parts list as a tree
 *              - and map the parts representation into the coordinates
 */

static int _PM_map_tree(PM_part *parts, PM_side *base, PM_mesh *mesh)
   {int kmin, lmin;
    PM_side *ib, *mbase;

    kmin = mesh->kmin;
    lmin = mesh->lmin;

    ib = base;
    while (TRUE)
       {if (base->fill)
           {base->k += (1 - kmin);
            base->l += (1 - lmin);
            base->fill = FALSE;
            _PM_map_base(parts, base, mesh);};

        mbase = base->match;
        if ((mbase != NULL) && mbase->fill)
           _PM_map_tree(parts, mbase, mesh);

        base = base->next;
        if (base == ib)
           break;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_NEARK - given k and a side return the nearest k value for a node
 *           - inside the region whose boundary contains the side
 */

static int _PM_neark(PM_side *ib, int k)
   {

    if (ib->dk > 0)
       k++;

    else if (ib->dk < 0)
       k--;

    else
       {if (ib->dl > 0)
           k--;
        else if (ib->dl < 0)
           k++;
        else
           k = -1;};

    return(k);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_NEARL - given l and a side return the nearest l value for a node
 *           - inside the region whose boundary contains the side
 */

static int _PM_nearl(PM_side *ib, int l)
   {

    if (ib->dl > 0)
       l++;

    else if (ib->dl < 0)
       l--;

    else
       {if (ib->dk > 0)
           l++;
        else if (ib->dk < 0)
           l--;
        else
           l = -1;};

    return(l);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_IN_PART - return a node index that is guaranteed to be in the part
 *             - whose boundary contains the given side 
 */

static int _PM_in_part(PM_side *ib, int *kp, int *lp, PM_mesh *mesh)
   {int k, l, kbnd;

    kbnd = mesh->kmax + 1;

/* find the k value of the nearest node interior to the boundary point
 * explicitly associated with the side
 */
    k = _PM_neark(ib, ib->k);
    if (k == -1)
       return(-1);

/* find the l value of the desired node */
    l = _PM_nearl(ib, ib->l);
    if (l == -1)
       return(-1);

    *kp = k;
    *lp = l;

    return(NODE_OF(k, l));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_SWEEP_NODES - return the number of nodes in the region
 *                 - sweep repeatedly over the mesh until no more nodes can
 *                 - be found in the region - this is most general if not the
 *                 - most efficient algorithm
 */

static int _PM_sweep_nodes(int ks, int ls, PM_part *parta, PM_mesh *mesh)
   {int k, l, n, nn, kbnd;
    int i0, i1, i2, i3, i4;
    int kmn, kmx, lmn, lmx;
    double *nodet;
    PM_side *ib;

    kbnd  = mesh->kmax + 1;
    nodet = mesh->nodet;

/* find the min and max k and l for the region */
    kmx = mesh->kmin;
    kmn = mesh->kmax;
    lmx = mesh->lmin;
    lmn = mesh->lmax;
    for (ib = parta->leg; TRUE; )
        {kmx = max(kmx, ib->k);
         kmn = min(kmn, ib->k);
         lmx = max(lmx, ib->l);
         lmn = min(lmn, ib->l);
         if ((ib = ib->next) == parta->leg)
            break;};

/* starting from the given node check the neighbors to see if they are in
 * the same region
 */
    n  = 0;
    nn = 0;
    i0 = NODE_OF(ks, ls);
    if (nodet[i0] == 0.0)
       {nodet[i0] = -1.0;
        n++;};

/* loop over and over until no new nodes are found */
    while (nn != n)
       {nn = n;
        for (k = kmn; k <= kmx; k++)
            for (l = lmn; l <= lmx; l++)
                {i0 = NODE_OF(k, l);
                 i1 = i0 + 1;
                 i2 = i0 + kbnd;
                 i3 = i0 - 1;
                 i4 = i0 - kbnd;
                 if ((nodet[i0] == 0.0) &&
                     ((nodet[i1] == -1.0) || (nodet[i2] == -1.0) ||
                      (nodet[i3] == -1.0) || (nodet[i4] == -1.0)))
                    {nodet[i0] = -1.0;
                     n++;};};};
    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_MESH_PART - find the rest of the nodes in the part
 *               - and compute the mesh in the part
 */

static void _PM_mesh_part(PM_part *ipart, int k, int l, PM_mesh *mesh,
			  int strategy, int method, int constr,
			  double dspat, double drat, double orient)
   {int n, m, kt, lt;
    int kmn, kmx, lmn, lmx;
    double kr, lr;
    double ask, xsk, aek, xek;
    double asl, xsl, ael, xel;
    PM_side *ib;

    m  = ipart->reg;
    kr = ipart->k_ratio;
    lr = ipart->l_ratio;

    ask = ipart->k_mag_start;
    xsk = ipart->k_exp_start;
    aek = ipart->k_mag_end;
    xek = ipart->k_exp_end;

    asl = ipart->l_mag_start;
    xsl = ipart->l_exp_start;
    ael = ipart->l_mag_end;
    xel = ipart->l_exp_end;

    kmx = 0;
    kmn = mesh->kmax;
    lmx = 0;
    lmn = mesh->lmax;
    for (ib = ipart->leg; TRUE; ib = ib->next)
        {kt = ib->k;
         lt = ib->l;

         kmx = max(kmx, kt);
         kmn = min(kmn, kt);
         lmx = max(lmx, lt);
         lmn = min(lmn, lt);

         if (ib->next == ipart->leg)
            break;};

    n = _PM_sweep_nodes(k, l, ipart, mesh);

    PM_mesh_part(mesh->rx, mesh->ry, mesh->nodet, mesh->reg_map,
		 n, k, l, kmn, kmx, lmn, lmx,
		 mesh->kmax, mesh->lmax,
		 m, kr, lr,
		 mesh->kra, mesh->lra, mesh->apk, mesh->apl,
		 ask, xsk, aek, xek,
		 asl, xsl, ael, xel,
		 strategy, method, constr, dspat, drat, orient);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FILL_PART - fill in the mesh in each part */

void _PM_fill_part(PM_part *parts, PM_mesh *mesh, int strategy,
		   int method, int constr, double dspat, double drat,
		   double orient)
   {int i, k, l;
    double *nodet;
    PM_part *ipart;
    PM_side *ib;

    nodet = mesh->nodet;

    for (ipart = parts; ipart; ipart = ipart->next)
        for (ib = ipart->leg; TRUE; )
            {i = _PM_in_part(ib, &k, &l, mesh);

             if (nodet[i] == 0.0)
                {_PM_mesh_part(ipart, k, l, mesh,
			       strategy, method, constr, dspat, drat, orient);
                 break;};

             if ((ib = ib->next) == ipart->leg)
                break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FILL_MESH - using the parts list as a tree, fill in the absolute
 *               - logical coordinates of each point
 *               - make the coordinate arrays and the map of nodes
 *               - compute kmax and lmax
 */

static PM_mesh *_PM_fill_mesh(PM_part *parts, int strategy, int method,
			      int constr, double dspat, double drat,
			      double orient)
   {PM_side *base;
    int i, j, kmin, kmax, lmin, lmax, kbnd, lbnd;
    int n_zones, n_nodes, frz, frn, lrz, lrn;
    int *reg_map;
    double real;
    double *n1, *n2, *n3, *n4;
    double *kra, *lra, *apk, *apl;
    double *nodet, *zone, *rx, *ry;
    PM_mesh *mesh;

    if (parts == NULL)
       return(NULL);

    base    = parts->leg;
    base->k = 0;
    base->l = 0;

    mesh = CMAKE(PM_mesh);
    SC_MEM_INIT(PM_mesh, mesh);

    _PM_fill_tree(base, mesh);

    kmax = mesh->kmax;
    lmax = mesh->lmax;
    kmin = mesh->kmin;
    lmin = mesh->lmin;

    _PM_chck_part(parts);

    kmax = kmax - kmin + 1;
    lmax = lmax - lmin + 1;
    kbnd = kmax + 1;
    lbnd = lmax + 1;
    n_nodes = n_zones = kbnd*lbnd;

    mesh->kmax = kmax;
    mesh->lmax = lmax;
    mesh->n_zones = n_zones;
    mesh->n_nodes = n_nodes;

    reg_map = CMAKE_N(int, n_zones);
    rx      = CMAKE_N(double, n_nodes);
    ry      = CMAKE_N(double, n_nodes);
    nodet   = CMAKE_N(double, n_nodes);
    zone    = CMAKE_N(double, n_zones);

    kra = CMAKE_N(double, n_nodes);
    lra = CMAKE_N(double, n_nodes);
    apk = CMAKE_N(double, n_zones);
    apl = CMAKE_N(double, n_zones);

    mesh->reg_map = reg_map;
    mesh->nodet   = nodet;
    mesh->zone    = zone;
    mesh->rx      = rx;
    mesh->ry      = ry;
    mesh->kra     = kra;
    mesh->lra     = lra;
    mesh->apk     = apk;
    mesh->apl     = apl;

/* initialize the nodal arrays */
    PM_array_set(rx, n_nodes, 0.0);
    PM_array_set(ry, n_nodes, 0.0);
    PM_array_set(nodet, n_nodes, 0.0);

/* initialize the zonal arrays */
    PM_array_set(zone, n_zones, 0.0);
    PM_array_set(apk,  n_zones, 1.0);
    PM_array_set(apl,  n_zones, 1.0);
    PM_array_set(kra,  n_nodes, 1.0);
    PM_array_set(lra,  n_nodes, 1.0);

    _PM_map_tree(parts, base, mesh);

    mesh->kmin = 1;
    mesh->lmin = 1;

    _PM_fill_part(parts, mesh,
		  strategy, method, constr, dspat, drat, orient);

/* compute frz, lrz, frn, lrn */
    frz = 0;
    frn = 0;
    lrz = n_zones;
    lrn = n_nodes;

    vecset4(nodet, n1, n2, n3, n4);
    for (frz = -1, frn = -1, i = kbnd+1; i < n_nodes; i++)
        {real = n1[i]*n2[i]*n3[i]*n4[i];
         if (real != 0.0)
            {lrn = i;
             if (frz == -1)
                frz = i;
             if (frn == -1)
                frn = i - kbnd - 1;};};

    lrz = lrn;

    mesh->frn = frn;
    mesh->lrn = lrn;
    mesh->frz = frz;
    mesh->lrz = lrz;

/* fill in the double zone map */
    for (j = frz; j <= lrz; j++)
        {real = n1[j]*n2[j]*n3[j]*n4[j];
         if (real != 0.0)
            zone[j] = 1.0;};

    return(mesh);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_LR_MESH - make a logical rectangular mesh
 *                 - returns a structure containing complete mesh
 *                 - description iff successful
 *                 - Arguments:
 *                 -   PARTS    linked list of parts (i)
 *                 -   CURVES   hash table of boundary curves (i)
 *                 -   STRATEGY solver method: 1 - DSCG
 *                 -                           2 - DECSOL
 *                 -                           3 - ICCG
 *                 -                           4 - BICG
 *                 -                          -1 - default, we pick
 *                 -   METHOD   generation method: 1) ratios only;
 *                 -                               2) products and rations
 *                 -   CONSTR   impose mesh generation constraint
 *                 -   DSPAT    spatial differencing: 0.0 -> pure 5 point
 *                 -                                  1.0 -> pure 9 point
 *                 -   DRAT     ratio differencing: 0.0 -> pure 5 point
 *                 -                                1.0 -> pure 9 point
 *                 -   ORIENT   orientation: 0.0 -> pure K orientation
 *                 -                         1.0 -> pure L orientation
 */

PM_mesh *PM_make_lr_mesh(PM_part *parts, hasharr *curves, int strategy,
			 int method, int constr, double dspat,
			 double drat, double orient)
   {PM_part *parta, *partb;
    PM_mesh *msh;

    switch (SETJMP(_PM.mesh_err))
       {case ABORT :
        case ERR_FREE :
             return(NULL);
        default :
             break;};

    PM_MESH_ERR(curves == NULL);

/* check that all sides have valid curves */
    for (parta = parts; parta != NULL; parta = parta->next)
        _PM_chck_curves(parta, curves);

/* complete all parts so that all sides are irreducible
 * the second loop starts at parta and the first loop ends at NULL
 * because a part may wrap around and touch itself!
 */
    if ((parts != NULL) && (parts->next != NULL))
       for (parta = parts; parta != NULL; parta = parta->next)
           for (partb = parta; partb != NULL; partb = partb->next)
               _PM_complete(parta, partb);

/* now draw in the mesh */
    msh = _PM_fill_mesh(parts, strategy, method,
			constr, dspat, drat, orient);

    return(msh);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
