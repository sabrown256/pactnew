/*
 * GMESH.C - mesh generation controller for the LR global package
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "gloin.h"

#define ANGLE_DEVIATE(orth, dx1, dy1, dx2, dy2)                              \
    ot    = 1.0/sqrt(dx1*dx1 + dy1*dy1 + SMALL);                             \
    dx1  *= ot;                                                              \
    dy1  *= ot;                                                              \
    ot    = 1.0/sqrt(dx2*dx2 + dy2*dy2 + SMALL);                             \
    dx2  *= ot;                                                              \
    dy2  *= ot;                                                              \
    ot    = RAD_DEG*(pot - acos(dx1*dx2 + dy1*dy2));                         \
    orth += ot*ot

void
 compute_fractions(double *fr, PM_side *base, int n),
 compute_ratios(double *fr, PM_side *base),
 compute_points(double x1, double y1,
		double x2, double y2, double *f,
		int *indx, int n, int dir, PM_conic_curve *crv),
 find_points(double *xp, double *yp,
	     int m, double t, double *f,
	     int *indx, int n, PM_conic_curve *crv);

hasharr
 *curves;

double
 *apk,
 *apl,
 *kra,
 *lra;

int
 N_parts,
 N_regs;

char
 *MESH_CURVE = NULL;

static int
 new_part = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_LRM - make a logical rectangular mesh */

int make_lrm(void)
   {int zs;
    PM_mesh *mesh;

    zs = SC_zero_space(2);

    if (curves == NULL)
       curves = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);

    mesh = PM_make_lr_mesh(parts, curves,
			   SWTCH[17], SWTCH[15], SWTCH[16],
			   PARAM[8], PARAM[9], PARAM[10]);

    rx      = mesh->rx;
    ry      = mesh->ry;
    nodet   = mesh->nodet;
    zone    = mesh->zone;
    reg_map = mesh->reg_map;

    kmax    = mesh->kmax;
    kmin    = mesh->kmin;
    lmax    = mesh->lmax;
    lmin    = mesh->lmin;
    N_zones = mesh->n_zones;
    N_nodes = mesh->n_nodes;
    lrn     = mesh->lrn;
    lrz     = mesh->lrz;
    frn     = mesh->frn;
    frz     = mesh->frz;

/* now that we have the mesh set other mesh variables */
    vx = CMAKE_N(double, N_nodes);
    vy = CMAKE_N(double, N_nodes);

    kbnd = kmax + 1;
    lbnd = lmax + 1;

    CFREE(mesh);

    zs = SC_zero_space(zs);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPUTE_MESH_QUALITY - compute the quality of the mesh and return the
 *                      - information via a pointer to a mesh_quality
 *                      - structure
 */

mesh_quality *compute_mesh_quality(void)
   {int j, count;
    double *x1, *x2, *x3, *x4;
    double *y1, *y2, *y3, *y4;
    double dx1, dx2, dy1, dy2, dx13, dy13, dx24, dy24;
    double dxtb, dytb, dxrl, dyrl;
    double skew, jac, orth, jt, ot, st, vol;
    mesh_quality *mq;
    static double pot = PI/2.0;

    mq = CMAKE(mesh_quality);

    count = 0;
    skew  = 0.0;
    jac   = 0.0;
    orth  = 0.0;

    vecset4(rx, x1, x2, x3, x4);
    vecset4(ry, y1, y2, y3, y4);
    for (j = frz; j <= lrz; j++)
        {if (zone[j] == 0.0)
            continue;

         count++;

         dx13 = x1[j] - x3[j];
         dy13 = y1[j] - y3[j];
         dx24 = x2[j] - x4[j];
         dy24 = y2[j] - y4[j];

         dxrl = 0.5*(x1[j] + x2[j] - x3[j] - x4[j]);
         dyrl = 0.5*(y1[j] + y2[j] - y3[j] - y4[j]);
         dxtb = 0.5*(x2[j] + x3[j] - x1[j] - x4[j]);
         dytb = 0.5*(y2[j] + y3[j] - y1[j] - y4[j]);

         vol   = 0.5*(dx13*dy24 - dx24*dy13);
         jt    = vol/(ABS(dxrl*dytb) + ABS(dxtb*dyrl) + SMALL);
         jac  += jt*jt;

         dx1   = sqrt(dx13*dx13 + dy13*dy13);
         dx2   = sqrt(dx24*dx24 + dy24*dy24);
         st    = 2.0*(dx1 - dx2)/(dx1 + dx2);
         skew += st*st;

         dx1 = x1[j] - x2[j];
         dy1 = y1[j] - y2[j];
         dx2 = x2[j] - x3[j];
         dy2 = y2[j] - y3[j];
         ANGLE_DEVIATE(orth, dx1, dy1, dx2, dy2);

         dx1 = dx2;
         dy1 = dy2;
         dx2 = x3[j] - x4[j];
         dy2 = y3[j] - y4[j];
         ANGLE_DEVIATE(orth, dx1, dy1, dx2, dy2);

         dx1 = dx2;
         dy1 = dy2;
         dx2 = x4[j] - x1[j];
         dy2 = y4[j] - y1[j];
         ANGLE_DEVIATE(orth, dx1, dy1, dx2, dy2);

         dx1 = dx2;
         dy1 = dy2;
         dx2 = x1[j] - x2[j];
         dy2 = y1[j] - y2[j];
         ANGLE_DEVIATE(orth, dx1, dy1, dx2, dy2);};

    mq->skew = sqrt(skew/((double) count));
    mq->jac  = sqrt(jac/((double) count));
    mq->orth = sqrt(orth/(4.0*count));

    return(mq);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NORM_CLINE - put curves in internal normal form
 *            - the first of {axx ayy axy} that is nonvanishing
 *            - divides all coefficients
 *            - if linear the first of {ay ax} that is nonvanishing
 *            - divides all coefficients
 */

int norm_cline(PM_conic_curve *cp)
   {double axx, ayy, axy, ax, ay, ac;

    axx = cp->xx;
    ayy = cp->yy;
    axy = cp->xy;
    ax = cp->x;
    ay = cp->y;
    ac = cp->c;

    if (!CLOSETO(axx, 0.0))
       {ac /= axx;
        ax /= axx;
        ay /= axx;
        axy /= axx;
        ayy /= axx;
        axx = 1.0;}
    else if (!CLOSETO(ayy, 0.0))
       {ac /= ayy;
        ax /= ayy;
        ay /= ayy;
        axy /= ayy;
        ayy = 1.0;}
    else if (!CLOSETO(axy, 0.0))
       {ac /= axy;
        ax /= axy;
        ay /= axy;
        axy = 1.0;}
    else if (!CLOSETO(ay, 0.0))
       {ac /= ac;
        ax /= ay;
        ay = 1.0;}
    else
       {ac /= ax;
        ax = 1.0;};

    cp->xx = axx;
    cp->yy = ayy;
    cp->xy = axy;
    cp->x = ax;
    cp->y = ay;
    cp->c = ac;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CONTAINSW - return the index of the zone
 *             - with contains the given point
 */

int containsw(double xr, double yr)
   {int j;
    double *rx1, *rx2, *rx3, *rx4;
    double *ry1, *ry2, *ry3, *ry4;

    vecset4(rx, rx1, rx2, rx3, rx4);
    vecset4(ry, ry1, ry2, ry3, ry4);

    for (j = frz; j <= lrz; j++)
        {if (zone[j] == 0.0)
            continue;
         if (inside(xr, yr, rx1[j], ry1[j], rx2[j], ry2[j],
                    rx3[j], ry3[j], rx4[j], ry4[j]))
            return(j);};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INSIDE - return TRUE iff the first point is in the
 *        - quadrilateral defined by the other four
 */

int inside(double xx, double yy, double x1, double y1,
	   double x2, double y2, double x3, double y3,
	   double x4, double y4)
   {double xb, yb, xr, yr, xt, yt, xl, yl;
    double dx1, dy1, dx2, dy2, dx3, dy3, dx4, dy4;
    double c1, c2, c3, c4;

    xb = x1 - x4;
    yb = y1 - y4;
    xr = x2 - x1;
    yr = y2 - y1;
    xt = x3 - x2;
    yt = y3 - y2;
    xl = x4 - x3;
    yl = y4 - y3;

    dx1 = xx - x1;
    dy1 = yy - y1;
    dx2 = xx - x2;
    dy2 = yy - y2;
    dx3 = xx - x3;
    dy3 = yy - y3;
    dx4 = xx - x4;
    dy4 = yy - y4;

    c1 = xr*dy1 - yr*dx1;
    c2 = xt*dy2 - yt*dx2;
    c3 = xl*dy3 - yl*dx3;
    c4 = xb*dy4 - yb*dx4;

    if ((c1 >= 0.0) && (c2 >= 0.0) && (c3 > 0.0) && (c4 > 0.0))
       return(TRUE);
    else
       return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VOLUMEW - compute the zone volumes */

int volumew(double *vol, double *rx, double *ry)
   {int j;
    double r;
    double *rx1, *rx2, *rx3, *rx4;
    double *ry1, *ry2, *ry3, *ry4;
 
    vecset4(rx, rx1, rx2, rx3, rx4);
    vecset4(ry, ry1, ry2, ry3, ry4);
 
    for (j = frz; j <= lrz; j++)
        {r      = zone[j];
	 vol[j] = r*VOLR(rx1[j], rx2[j], rx3[j], rx4[j],
			 ry1[j], ry2[j], ry3[j], ry4[j]);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GEN_DUMPW - dump the mesh coordinate arrays at generation time */

int gen_dumpw(hasharr *curves)
   {int i, j, k, l, sz;
    double length;
    char *edname;
    haelem *np, **tb;
    FILE *fp;
    PM_conic_curve *cp;

    length = 1.0/unit[CM];

    edname = NAME[3];

    PA_ERR(((fp = fopen(edname, "w")) == NULL),
           "CAN'T OPEN EDIT FILE - %s", edname);

    PRINT(fp, "GENERATION TIME MESH DUMP\n\n\n");

    sz = curves->size;
    tb = curves->table;
    PRINT(fp, "\nCURVES\n\n");
    for (i = 0; i < sz; i++)
        for (np = *(tb+i); np != NULL; np = np->next)
            {cp = (PM_conic_curve *) np->def;
             PRINT(fp, "Curve %s: ", cp->type);
             PRINT(fp,
                     "%4f*x^2 + %4f*y^2 + %4f*xy + %4f*x + %4f*y + %4f = 0\n",
                     cp->xx, cp->yy, cp->xy, cp->x, cp->y, cp->c);};

    PRINT(fp, "\n\nNUMBER OF LOGICAL NODES: %d\n", N_nodes);
    PRINT(fp, "FIRST REAL NODE: %d\n", frn);
    PRINT(fp, "LAST REAL NODE: %d\n\n", lrn);
    PRINT(fp, "   K      L         X             Y   \n\n");
    for (k = 1; k <= kbnd; k++)
        for (l = 1; l <= lbnd; l++)
            {i = NODE_OF(k, l);
             PRINT(fp, " %4d   %4d    %10.4e    %10.4e\n",
                         k, l, rx[i]*length, ry[i]*length);};

    PRINT(fp, "\n\n\nNODE MAP\n\n");
    PRINT(fp, "l\\k |");
    for (k = 1; k <= kbnd; k++)
        PRINT(fp, "%2d|", k);
    PRINT(fp, "\n-----");
    for (k = 1; k <= kbnd; k++)
        PRINT(fp, "---");
    for (l = lbnd; l >= 1; l--)
        {PRINT(fp, "\n%3d |", l);
         for (k = 1; k <= kbnd; k++)
             {j = NODE_OF(k, l);
              PRINT(fp, "%2d|", (int) nodet[j]);};
         PRINT(fp, "\n-----");
         for (k = 1; k <= kbnd; k++)
             PRINT(fp, "---");};
    PRINT(fp, "\n");

    PRINT(fp, "\n\n\nZONE MAP\n\n");
    PRINT(fp, "l\\k |");
    for (k = 1; k <= kbnd; k++)
        PRINT(fp, "%2d|", k);
    PRINT(fp, "\n-----");
    for (k = 1; k <= kbnd; k++)
        PRINT(fp, "---");
    for (l = lbnd; l >= 1; l--)
        {PRINT(fp, "\n%3d |", l);
         for (k = 1; k <= kbnd; k++)
             {j = NODE_OF(k, l);
              PRINT(fp, "%2d|", (int) zone[j]);};
         PRINT(fp, "\n-----");
         for (k = 1; k <= kbnd; k++)
             PRINT(fp, "---");};
    PRINT(fp, "\n");

    PRINT(fp, "\n\nNUMBER OF REAL ZONES: %d\n\n", lrz-frz+1);
    PRINT(fp, "FIRST REAL ZONE: %d\n", frz);
    PRINT(fp, "LAST REAL ZONE: %d\n\n", lrz);
    PRINT(fp, "REGION MAP\n\n");
    PRINT(fp, "l\\k |");
    for (k = 1; k <= kbnd; k++)
        PRINT(fp, "%2d|", k);
    PRINT(fp, "\n-----");
    for (k = 1; k <= kbnd; k++)
        PRINT(fp, "---");
    for (l = lbnd; l >= 1; l--)
        {PRINT(fp, "\n%3d |", l);
         for (k = 1; k <= kbnd; k++)
             {j = NODE_OF(k, l);
              PRINT(fp, "%2d|", reg_map[j]);};
         PRINT(fp, "\n-----");
         for (k = 1; k <= kbnd; k++)
             PRINT(fp, "---");};
    PRINT(fp, "\n");

    fclose(fp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAKE_MESH - control mesh generation */

void make_mesh(void)
   {int i;
    double conv;
    PM_part *parta;

/* be sure to be using the global controls at this point */
    PA_control_set("global");

/* count the number of parts */
    for (N_parts = 0, parta = parts; parta; parta = parta->next, N_parts++);

/* make the logical mesh */
    make_lrm();

/* fill the array node with 1's for real nodes and 0's for phony nodes */
    node = CMAKE_N(double, N_nodes);
    for (i = 0; i < N_nodes; i++)
        if (nodet[i] != 0.0)
           node[i] = 1.0;

/* reprocess the plot requests now that the mesh is known */
    SWTCH[4] = N_plots;

/* do some conversions and set up */
    conv = unit[SEC]/convrsn[SEC];                /* time to internal units */
    PARAM[2]  *= conv;                                        /* start time */
    PARAM[3]  *= conv;                                         /* stop time */
    PARAM[1]  = PARAM[2];                                   /* current time */
    SWTCH[3]  = 0;                                         /* current cycle */

/* pre-intern these things so that the interactive plotting
 * can find database variables
 */
    PA_definitions();
    PA_variables(FALSE);
    intern_global();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CLEARH - re-initialize the system in preparation for new IV problem */

void clearh(void)
   {

    rl_mesh();
    PA_clear();
    
#ifndef GEN_ONLY
    window_init = FALSE;

    PG_clear_screen(mesh_display);
    make_frame();
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PARTH - handler of part commands */

void parth(void)
   {int i, reg;
    char *name, *token;
    double krat, kms, kes, kme, kee, lrat, lms, les, lme, lee;

    name  = PA_get_field("NAME", "PART", REQU);
    reg   = part_reg(name);

    krat = 1.0;
    kms  = 0.0;
    kes  = 0.0;
    kme  = 0.0;
    kee  = 0.0;
    lrat = 1.0;
    lms  = 0.0;
    les  = 0.0;
    lme  = 0.0;
    lee  = 0.0;
    for (i = 0; i < 2; i++)
        {token = PA_get_field("ZONING SPEC", "PART", OPTL);
         if (token == NULL)
            break;

         if (strcmp(token, "k") == 0)
            {kms  = PA_get_num_field("K START MAGNITUDE", "PART", OPTL);
             kes  = PA_get_num_field("K START EXPONENT", "PART", OPTL);
             kme  = PA_get_num_field("K END MAGNITUDE", "PART", OPTL);
             kee  = PA_get_num_field("K END EXPONENT", "PART", OPTL);
             krat = PA_get_num_field("K SPACING RATIO", "PART", OPTL);}

         else if (strcmp(token, "l") == 0)
            {lms  = PA_get_num_field("L START MAGNITUDE", "PART", OPTL);
             les  = PA_get_num_field("L START EXPONENT", "PART", OPTL);
             lme  = PA_get_num_field("L END MAGNITUDE", "PART", OPTL);
             lee  = PA_get_num_field("L END EXPONENT", "PART", OPTL);
             lrat = PA_get_num_field("L SPACING RATIO", "PART", OPTL);};};

    parts = PM_mk_part(name, reg, 0, NULL, NULL, NULL,
		       kms, kes, kme, kee, krat,
		       lms, les, lme, lee, lrat,
		       parts);

    new_part = TRUE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RECTH - handler of rect commands */

void recth(void)
   {int dk, dl, reg;
    char *token, *name;
    double krat, lrat;
    double x1, y1, x2, y2, x3, y3, x4, y4;
    PM_side *base1, *base2, *base3, *base4;

    x1 = PA_get_num_field("FIRST CORNER X", "RECT", REQU);
    y1 = PA_get_num_field("FIRST CORNER Y", "RECT", REQU);
    x2 = PA_get_num_field("SECOND CORNER X", "RECT", REQU);
    y2 = PA_get_num_field("SECOND CORNER Y", "RECT", REQU);

    token = PA_get_field("TOKEN", "RECT", REQU);
    if (strcmp(token, "extent") == 0)
       {if (x1 > x2)
           {x3 = x1;
            x1 = x2;
            x2 = x3;};

        if (y1 > y2)
           {y3 = y1;
            y1 = y2;
            y2 = y3;};

        x3 = x2;
        y3 = y2;
        y2 = y1;

        x4 = x1;
        y4 = y3;}

    else
       {x3 = SC_stof(token);
        y3 = PA_get_num_field("THIRD CORNER Y", "RECT", REQU);
        x4 = PA_get_num_field("FOURTH CORNER X", "RECT", REQU);
        y4 = PA_get_num_field("FOURTH CORNER Y", "RECT", REQU);

        token = PA_get_field("EXTENT", "RECT", REQU);};

    dk   = PA_get_num_field("LOGICAL K INTERVAL", "RECT", REQU);
    dl   = PA_get_num_field("LOGICAL L INTERVAL", "RECT", REQU);

    krat = PA_get_num_field("K SPACING RATIO", "RECT", OPTL);
    if (krat == 0.0)
       krat = 1.0;

    lrat = PA_get_num_field("L SPACING RATIO", "RECT", OPTL);
    if (lrat == 0.0)
       lrat = 1.0;

    base4 = PM_mk_side(NULL, 1, x4, y4,   0, -dl, krat, lrat, 0.0, 0.0, 0.0,
		       0.0, 1.0, 0.0, 1.0,
		       NULL, NULL);
    base3 = PM_mk_side(NULL, 1, x3, y3, -dk,   0, krat, lrat, 0.0, 0.0, 0.0,
		       0.0, 1.0, 0.0, 1.0,
		       NULL, base4);
    base2 = PM_mk_side(NULL, 1, x2, y2,   0,  dl, krat, lrat, 0.0, 0.0, 0.0,
		       0.0, 1.0, 0.0, 1.0,
		       NULL, base3);
    base1 = PM_mk_side(NULL, 1, x1, y1,  dk,   0, krat, lrat, 0.0, 0.0, 0.0,
		       0.0, 1.0, 0.0, 1.0,
		       NULL, base2);

    base4->next = base1;

    if (new_part)
       {parts->leg = base1;
        new_part   = FALSE;}

    else
       {name  = parts->name;
        reg   = parts->reg;

        parts = PM_mk_part(name, reg, 4, base1, NULL, NULL,
			   0.0, 0.0, 0.0, 0.0, krat,
			   0.0, 0.0, 0.0, 0.0, lrat,
			   parts);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SIDEH - handler of side commands */

void sideh(void)
   {int dk, dl, dir;
    char *s;
    double x, y, rat, drn, drx, ms, es, me, ee;
    PM_side *base, *ib;
    PM_conic_curve *cur;

    PA_ERR((parts == NULL),
           "SIDE command preceding any PART command");

    (parts->n_sides)++;
    dir = 1;

    s   = PA_get_field("CURVE", "SIDE", OPTL);
    cur = atocur(s);
    if (cur != NULL)
       s = PA_get_field("DIRECTION", "SIDE", OPTL);

    if (strcmp(s, "+") == 0)
       {dir = 1;
        s = PA_get_field("CORNER", "SIDE", REQU);}
    else if (strcmp(s, "-") == 0)
       {dir = -1;
        s = PA_get_field("CORNER", "SIDE", REQU);};

    x = SC_stof(s);
    y = PA_get_num_field("CORNER", "SIDE", REQU);

    dk  = PA_get_num_field("LOGICAL-INTERVAL", "SIDE", REQU);
    dl  = PA_get_num_field("LOGICAL-INTERVAL", "SIDE", REQU);
    rat = PA_get_num_field("RATIO", "SIDE", OPTL);
    drn = PA_get_num_field("STARTING DR FRACTION", "SIDE", OPTL);
    drx = PA_get_num_field("ENDING DR FRACTION", "SIDE", OPTL);

    ms = PA_get_num_field("START MAGNITUDE", "SIDE", OPTL);
    es = PA_get_num_field("START EXPONENT", "SIDE", OPTL);
    me = PA_get_num_field("END MAGNITUDE", "SIDE", OPTL);
    ee = PA_get_num_field("END EXPONENT", "SIDE", OPTL);

    base = PM_mk_side(cur, dir, x, y, dk, dl, parts->k_ratio, parts->l_ratio,
		      rat, drn, drx, ms, es, me, ee,
		      NULL, NULL);

    ib = parts->leg;
    if (ib == NULL)
       {parts->leg = base;
        base->next = parts->leg;}
    else
       {while (ib->next != parts->leg)
           ib = ib->next;
        ib->next = base;
        base->next  = parts->leg;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CURVEH - handle for curve commands */

void curveh(void)
   {char *s;

    s = PA_get_field("NAME", "CURVE", REQU);
    rd_cline(s);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* RD_CLINE - given a name, if a curve of the given name already exists
 *          - signal an error
 *          - otherwise, make a new curve, install it, and return a 
 *          - pointer to it
 */

PM_conic_curve *rd_cline(char *s)
   {double axx, ayy, axy, ax, ay, ac;
    PM_conic_curve *cur;

    if (curves == NULL)
       {curves = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
        MESH_CURVE = CSTRSAVE("PM_conic_curve");};

    PA_ERR((SC_hasharr_lookup(curves, s) != NULL),
           "CURVE: %s ALREADY EXISTS", s);

    axx = PA_get_num_field("X^2 COEFF", "CURVE", REQU);
    ayy = PA_get_num_field("Y^2 COEFF", "CURVE", REQU);
    axy = PA_get_num_field("XY COEFF", "CURVE", REQU);
    ax  = PA_get_num_field("X COEFF", "CURVE", REQU);
    ay  = PA_get_num_field("Y COEFF", "CURVE", REQU);
    ac  = PA_get_num_field("CONSTANT", "CURVE", REQU);

    cur = PM_mk_cline(s, axx, ayy, axy, ax, ay, ac);
    norm_cline(cur);
    SC_hasharr_install(curves, s, cur, MESH_CURVE, TRUE, TRUE);

    return(cur);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ATOCUR - if the given string is not the representation of a number
 *        - initiate the reading of a curve description and return
 *        - the pointer to the new curve
 *        - otherwise return NULL
 */

PM_conic_curve *atocur(char *s)
   {haelem *hp;

    if (SC_numstrp(s))
       return(NULL);

    else if (*s == '(')
       return(rd_cline(++s));

    else
       {hp = SC_hasharr_lookup(curves, s);
        PA_ERR((hp == NULL),
               "NO SUCH CURVE - %s", s);
        return((PM_conic_curve *) hp->def);};}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WHAT_AXIS - parse an axis field and return the zone number specified
 *           - replace the spatial indexes by the zone index and assign
 *           - the internal axis field to the result
 */

int what_axis(char *s, char **sub)
   {int zon;
    char bfi[MAXLINE], bfo[MAXLINE], t[MAXLINE];
    char *xrs, *yrs, *rest, *pt;

/* copy the name part into the output buffer */
    strcpy(bfi, s);
    snprintf(bfo, MAXLINE, "%s", SC_strtok(bfi, "(", pt));

/* the first index or two are the zone specification */
    if ((xrs = SC_strtok(NULL, ",)", pt)) == NULL)
       {zon = 0;
        *sub = NULL;}
    else
       {PA_ERR(((yrs = SC_strtok(NULL, ",)", pt)) == NULL),
               "BAD COORDINATE FOR PLOT: %s", s);
        zon = containsw(SC_stof(xrs), SC_stof(yrs));
        snprintf(t, MAXLINE, "(%d", zon);
        SC_strcat(bfo, MAXLINE, t);

/* complete the reformatted request axis specification */
        if ((rest = SC_strtok(NULL, ")", pt)) == NULL)
           SC_strcat(bfo, MAXLINE, ")");
        else
           {snprintf(t, MAXLINE, ",%s)", rest);
            SC_strcat(bfo, MAXLINE, t);};

/* reassign the axis specification to a fresh copy of the output buffer */
        *sub = CSTRSAVE(bfo);};

    return(zon);}
    
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_CLEAN_VARIABLE - put SMALL in all phony zones for the given variable */

void LR_clean_variable(double *v)
   {int j;

    for (j = 0; j < N_zones; j++)
        if (zone[j] == 0.0)
           v[j] = SMALL;

    return;}

/*--------------------------------------------------------------------------*/

/*                        MEMMORY MANAGEMENT ROUTINES                       */

/*--------------------------------------------------------------------------*/

/* RL_MESH - release the parts, sides, and ends */

int rl_mesh(void)
   {PM_part *ipart;
    PM_end_point *iend, *jend;
    PM_side *ib, *jbase;

    while (TRUE)
        {if (parts == NULL)
            break;
         ipart = parts;
         parts = parts->next;
         CFREE(ipart->name);
         for (ib = ipart->leg; ib != ipart->leg; )
             {jbase = ib;
              ib = ib->next;
              CFREE(jbase);};
         for (iend = ipart->ends; iend != NULL; )
             {jend = iend;
              iend = iend->next;
              CFREE(jend);};
         CFREE(ipart);};

    mesh_bndry   = FALSE;
    mesh_bndry_r = FALSE;
    mesh_mesh    = FALSE;
    mesh_mesh_r  = FALSE;
    mesh_plots   = FALSE;
    mesh_plots_r = FALSE;
    mesh_vectr   = FALSE;
    mesh_vectr_r = FALSE;

    SC_free_hasharr(curves, NULL, NULL);

    CFREE(rx);
    CFREE(ry);
    CFREE(nodet);
    CFREE(reg_map);
    CFREE(zone);

    kmax = kmin = 0;
    lmax = lmin = 0;
    N_nodes = 0;
    N_parts = 0;
    N_regs  = 0;
    lrn = frn = 0;
    lrz = 0;

    parts = NULL;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
