/*
 * LAGIN.C - the gateway for the lag process in ABC
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "lagin.h"

typedef struct s_par_nm par_nm;
typedef struct s_par_pr par_pr;
typedef struct s_par_hr par_hr;

struct s_par_nm
   {double *volq;
    double *mass;};

struct s_par_pr
   {double *pn;};

struct s_par_hr
   {double dts;};

int
 *indc,
 *idup,
 *irc;

double
 *aq,
 *di,
 *csp,
 csmin,
 *dtvg,
 *dvxdt,
 *dvydt,
 etoto,
 hgamma,
 *lpm,
 *massn,
 *massz,
 *pdv,
 *voln,
 *volo;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FIX_BCW - enforce the boundary conditions */

static int fix_bcw(double *rx, double *ry, double *dvxdt, double *dvydt)
   {int k, l, i;
    int k1, k2, l1, l2;
    double *iv_data;
    char *iv_name;
    PA_iv_specification *sp;

/* zero out all quantities (implicit bc) */
    for (i = 0; i < N_nodes; i++)
        {dvxdt[i] *= node[i];
	 dvydt[i] *= node[i];
	 rx[i]    *= node[i];
	 ry[i]    *= node[i];
	 vx[i]    *= node[i];
	 vy[i]    *= node[i];};

/* NOTE: here is where applications access initial value specifications
 * which PANACEA has gathered without any knowledge of the semantics.
 * These were all specified in the form:
 *    specify bc <name> <k1> <l1> <k2> <l2>
 */
    for (sp = iv_spec_lst; sp != NULL; sp = sp->next)
        {iv_name = sp->name;
         if ((iv_data = sp->data) == NULL)
            continue;
         k1 = (int) iv_data[0];
         l1 = (int) iv_data[1];
         k2 = (int) iv_data[2];
         l2 = (int) iv_data[3];
         if (strcmp(iv_name, "constant-vx") == 0)
            for (l = l1; l <= l2; l++)
                for (k = k1; k <= k2; k++)
                    dvxdt[NODE_OF(k, l)] = 0.0;
         else if (strcmp(iv_name, "constant-vy") == 0)
            for (l = l1; l <= l2; l++)
                for (k = k1; k <= k2; k++)
                    dvydt[NODE_OF(k, l)] = 0.0;
         else if (strcmp(iv_name, "fixed") == 0)
            for (l = l1; l <= l2; l++)
                for (k = k1; k <= k2; k++)
                    {i = NODE_OF(k, l);
                     dvxdt[i] = 0.0;
                     dvydt[i] = 0.0;};};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* AWC - compute contribution of P to DVDT */

static void *awc(void *arg)
   {int j, n1, n2;
    double nbx, nrx, ntx, nlx;
    double nby, nry, nty, nly;
    double ptot;
    double *rx1, *rx2, *rx3, *rx4;
    double *ry1, *ry2, *ry3, *ry4;
    double *dvxdt1, *dvxdt2, *dvxdt3, *dvxdt4;
    double *dvydt1, *dvydt2, *dvydt3, *dvydt4;
    void *rv;

    vecset4(rx, rx1, rx2, rx3, rx4);
    vecset4(ry, ry1, ry2, ry3, ry4);
    vecset4(dvxdt, dvxdt1, dvxdt2, dvxdt3, dvxdt4);
    vecset4(dvydt, dvydt1, dvydt2, dvydt3, dvydt4);

    SC_chunk_split(&n1, &n2, &rv);

    for (j = n1; j < n2; j++)
        {nbx = 0.5*(ry1[j] - ry4[j]);
         nby = 0.5*(rx4[j] - rx1[j]);
         nrx = 0.5*(ry2[j] - ry1[j]);
         nry = 0.5*(rx1[j] - rx2[j]);
         ntx = 0.5*(ry3[j] - ry2[j]);
         nty = 0.5*(rx2[j] - rx3[j]);
         nlx = 0.5*(ry4[j] - ry3[j]);
         nly = 0.5*(rx3[j] - rx4[j]);

         ptot = p[j];

         dvxdt1[j] += ptot*(nbx + nrx);
         dvydt1[j] += ptot*(nby + nry);
         dvxdt2[j] += ptot*(nrx + ntx);
         dvydt2[j] += ptot*(nry + nty);
         dvxdt3[j] += ptot*(ntx + nlx);
         dvydt3[j] += ptot*(nty + nly);
         dvxdt4[j] += ptot*(nlx + nbx);
         dvydt4[j] += ptot*(nly + nby);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* FW - move the mesh */

static int fw(double *p, double *rx, double *ry, double *rxn, double *ryn,
	      double *vx, double *vy, double *vxn, double *vyn, double dts)
   {int i;

    for (i = 0; i < N_nodes; i++)
        {dvxdt[i] = 0.0;
         dvydt[i] = 0.0;};

/* NOTE: do the work of awc using SMP parallelism */
    SC_chunk_loop(awc, frz, lrz, TRUE, NULL);

    fix_bcw(rx, ry, dvxdt, dvydt);

    for (i = frn; i <= lrn; i++)
        {vxn[i] = vx[i] + dts*dvxdt[i];
         vyn[i] = vy[i] + dts*dvydt[i];};

    for (i = frn; i <= lrn; i++)
        {rxn[i] = rx[i] + dts*vx[i];
         ryn[i] = ry[i] + dts*vy[i];};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* HWC - compute the H terms */

static void *hwc(void *arg)
   {int j, n1, n2;
    double nbx, nrx, ntx, nlx;
    double nby, nry, nty, nly;
    double *rx1, *rx2, *rx3, *rx4;
    double *ry1, *ry2, *ry3, *ry4;
    double vbx, vrx, vtx, vlx;
    double vby, vry, vty, vly;
    double *vx1, *vx2, *vx3, *vx4;
    double *vy1, *vy2, *vy3, *vy4;
    double nvbt, nvrl;
    double dvol, dts;
    void *rv;
    par_hr *par;

    vecset4(rx, rx1, rx2, rx3, rx4);
    vecset4(ry, ry1, ry2, ry3, ry4);
    vecset4(vx, vx1, vx2, vx3, vx4);
    vecset4(vy, vy1, vy2, vy3, vy4);

    SC_chunk_split(&n1, &n2, &rv);

    par = (par_hr *) arg;
    dts = par->dts;

    for (j = n1; j < n2; j++)
        pdv[j] = 0.0;;

    for (j = n1; j < n2; j++)
        {nbx = ry1[j] - ry4[j];
         nby = rx4[j] - rx1[j];
         nrx = ry2[j] - ry1[j];
         nry = rx1[j] - rx2[j];
         ntx = ry3[j] - ry2[j];
         nty = rx2[j] - rx3[j];
         nlx = ry4[j] - ry3[j];
         nly = rx3[j] - rx4[j];

         vbx = 0.5*(vx1[j] + vx4[j]);
         vby = 0.5*(vy1[j] + vy4[j]);
         vrx = 0.5*(vx2[j] + vx1[j]);
         vry = 0.5*(vy2[j] + vy1[j]);
         vtx = 0.5*(vx3[j] + vx2[j]);
         vty = 0.5*(vy3[j] + vy2[j]);
         vlx = 0.5*(vx4[j] + vx3[j]);
         vly = 0.5*(vy4[j] + vy3[j]);

         nvbt = (nbx*vbx + nby*vby + ntx*vtx + nty*vty)*dts;
         nvrl = (nrx*vrx + nry*vry + nlx*vlx + nly*vly)*dts;
         dvol = nvbt + nvrl;

         pdv[j] -= p[j]*dvol;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PWC - update the P values on a chunk */

static void *pwc(void *arg)
   {int j, n1, n2;
    double b, c, vninv;
    double *pn;
    void *rv;
    par_pr *par;

    SC_chunk_split(&n1, &n2, &rv);

    par = (par_pr *) arg;
    pn  = par->pn;

    for (j = n1; j < n2; j++)
        {vninv = 1.0/(voln[j] + SMALL);
         b     = volo[j]*vninv;
         c     = (hgamma - 1.0)*vninv;
         pn[j] = p[j]*b + c*pdv[j];};

    for (j = n1; j < n2; j++)
        rpdv[j] += pdv[j];

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PW - calculate the new P values */

static int pw(double *pn, double dts)
   {par_pr pp;
    par_hr ph;

    ph.dts = dts;
    pp.pn = pn;

    SC_chunk_loop(hwc, frz, lrz, TRUE, &ph);
    SC_chunk_loop(pwc, frz, lrz, TRUE, &pp);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* AREAW - compute the areas of the quadrants of a zone */

static void *areaw(void *arg)
   {int j, k, n1, n2;
    double *rx1, *rx2, *rx3, *rx4;
    double *ry1, *ry2, *ry3, *ry4;
    void *rv;

    vecset4(rx, rx1, rx2, rx3, rx4);
    vecset4(ry, ry1, ry2, ry3, ry4);

    SC_chunk_split(&n1, &n2, &rv);

    for (j = n1; j < n2; j++)
        {k = 5*j;

         aq[k+1] = VOLQR(rx1[j], rx2[j], rx3[j], rx4[j],
			 ry1[j], ry2[j], ry3[j], ry4[j]);

         aq[k+2] = VOLQR(rx2[j], rx3[j], rx4[j], rx1[j],
			 ry2[j], ry3[j], ry4[j], ry1[j]);

         aq[k+3] = VOLQR(rx3[j], rx4[j], rx1[j], rx2[j],
			 ry3[j], ry4[j], ry1[j], ry2[j]);

         aq[k+4] = VOLQR(rx4[j], rx1[j], rx2[j], rx3[j],
			 ry4[j], ry1[j], ry2[j], ry3[j]);

         aq[k] = aq[k+1] + aq[k+2] + aq[k+3] + aq[k+4];};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DAREADT - calculate the projected quadrant volume change */

static double dareadt(double *rx, double *ry, double *vx, double *vy)
   {int j;
    double *rx1, *rx2, *rx3, *rx4;
    double *ry1, *ry2, *ry3, *ry4;
    double *vx1, *vx2, *vx3, *vx4;
    double *vy1, *vy2, *vy3, *vy4;
    double da, dainv, dta, dtav;
    double mdtv;

    vecset4(rx, rx1, rx2, rx3, rx4);
    vecset4(ry, ry1, ry2, ry3, ry4);
    vecset4(vx, vx1, vx2, vx3, vx4);
    vecset4(vy, vy1, vy2, vy3, vy4);

    mdtv = PARAM[5];
    dta  = HUGE;

    for (j = frz; j <= lrz; j++)
        {da = VOLR(rx1[j], rx2[j], rx3[j], rx4[j],
                   vy1[j], vy2[j], vy3[j], vy4[j])  +
              VOLR(vx1[j], vx2[j], vx3[j], vx4[j],
                   ry1[j], ry2[j], ry3[j], ry4[j]);

         dainv = 1.0/(ABS(da) + SMALL);
         dtav = mdtv*dainv;

         dta = min(dta, dtav);};

    return(dta);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* VOLFRAC - compute the volumes of quadrants associated with
 *         - each for the nodes
 */

static int volfrac(double *volq, double *rx, double *ry)
   {int i, io, k, l;
    double *rxa, *rxb, *rxc, *rxd, *rxe, *rxf, *rxg, *rxh, *rxi;
    double *rya, *ryb, *ryc, *ryd, *rye, *ryf, *ryg, *ryh, *ryi;

    rxa = rx;
    rxb = rx + 1;
    rxf = rx - 1;
    rxd = rx + kbnd;
    rxc = rxd + 1;
    rxe = rxd - 1;
    rxh = rx - kbnd;
    rxi = rxh + 1;
    rxg = rxh - 1;

    rya = ry;
    ryb = ry + 1;
    ryf = ry - 1;
    ryd = ry + kbnd;
    ryc = ryd + 1;
    rye = ryd - 1;
    ryh = ry - kbnd;
    ryi = ryh + 1;
    ryg = ryh - 1;

    for (i = 0; i < 4*N_nodes; i++)
        volq[i] = 0.0;

    for (i = frn; i <= lrn; i++)
        {k = K_OF_NODE(i);
         l = L_OF_NODE(i);
         io = 4*i;

	 if ((1 < k) && (1 < l))
	    volq[io] = VOLQR(rxa[i], rxf[i], rxg[i], rxh[i],
			     rya[i], ryf[i], ryg[i], ryh[i]);
	 else
	    volq[io] = 0.0;

	 if ((k < kmax) && (1 < l))
	    volq[io+1] = VOLQR(rxa[i], rxh[i], rxi[i], rxb[i],
			       rya[i], ryh[i], ryi[i], ryb[i]);
	 else
	    volq[io+1] = 0.0;

	 if ((k < kmax) && (l < lmax))
	    volq[io+2] = VOLQR(rxa[i], rxb[i], rxc[i], rxd[i],
			       rya[i], ryb[i], ryc[i], ryd[i]);
	 else
	    volq[io+2] = 0.0;

	 if ((1 < k) && (l < lmax))
	    volq[io+3] = VOLQR(rxa[i], rxd[i], rxe[i], rxf[i],
			       rya[i], ryd[i], rye[i], ryf[i]);
	 else
	    volq[io+3] = 0.0;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NMW - compute the volume average of the pressures
 *     - for the zones initially
 */

static void *nmw(void *arg)
   {int j, n1, n2;
    int i1, i2, i3, i4, j1, j2, j3, j4;
    double v1, v2, v3, v4;
    double *volq, *mass;
    void *rv;
    par_nm *par;

    SC_chunk_split(&n1, &n2, &rv);

    par = (par_nm *) arg;

    volq = par->volq;
    mass = par->mass;

    for (j = n1; j < n2; j++)
        {i1 = j;
         j1 = 4*i1;
         v1 = volq[j1];

         i2 = i1 - 1;
         j2 = 4*i2;
         v2 = volq[j2+1];

         i3 = i1 - kbnd - 1;
         j3 = 4*i3;
         v3 = volq[j3+2];

         i4 = i1 - kbnd;
         j4 = 4*i4;
         v4 = volq[j4+3];

         mass[i1] += rho[j]*v1;
         mass[i2] += rho[j]*v2;
         mass[i3] += rho[j]*v3;
         mass[i4] += rho[j]*v4;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NODE_MASSW - compute the volume average of the pressures
 *            - for the zones initially
 */

int node_massw(double *mass, double *rho, double *rx, double *ry)
   {double *volq;
    par_nm pn;

    volq = FMAKE_N(double, 4*N_nodes, "NODE_MASSW:volq");
    volfrac(volq, rx, ry);

    PA_set_value(mass, N_nodes, 0.0);

    pn.volq = volq;
    pn.mass = mass;

    SC_chunk_loop(nmw, frz, lrz, TRUE, &pn);

    SFREE_N(volq, 4*N_nodes);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CWC - compute the timestep for this chunk */

static void *cwc(void *arg)
   {int j, k, n1, n2;
    double drx, dry;
    double *rx1, *rx2, *rx3, *rx4;
    double *ry1, *ry2, *ry3, *ry4;
    void *rv;

    vecset4(rx, rx1, rx2, rx3, rx4);
    vecset4(ry, ry1, ry2, ry3, ry4);

    SC_chunk_split(&n1, &n2, &rv);

    for (j = n1; j < n2; j++)
        {drx = 0.5*(rx2[j] + rx3[j] - rx4[j] - rx1[j]);
         dry = 0.5*(ry2[j] + ry3[j] - ry4[j] - ry1[j]);

         di[j] = 0.25*(drx*drx + dry*dry);

         drx = 0.5*(rx1[j] + rx2[j] - rx3[j] - rx4[j]);
         dry = 0.5*(ry1[j] + ry2[j] - ry3[j] - ry4[j]);

         di[j] += 0.25*(drx*drx + dry*dry);};

    for (j = n1; j < n2; j++)
        {k = 5*j;
         di[j] *= csp[j]*csp[j]/(aq[k]*aq[k] + SMALL);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CW - compute the timestep for next cycle */

static double cw(double *csp)
   {int j;
    double dtc;

    SC_chunk_loop(cwc, frz, lrz, TRUE, NULL);

    dtc = 0.0;
    for (j = frz; j <= lrz; j++)
        {if (di[j] > dtc)
            dtc = di[j];};

    dtc = PARAM[1]*hgamma/(sqrt(dtc) + SMALL);

    return(dtc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LAGW - Lagrange equation solver in cartesian coordinates */

int lagw(void)
   {int i, j;
    int nhcycle;
    double dts0, dts, dtsmin, dtelapse, dtmax;
    double dtc, dta, dvol;
    double *rxn, *ryn, *vxn, *vyn;
    double *pn;

/* allocate the temporary storage */
    aq  = FMAKE_N(double, 5*N_zones, "LAGW:aq");
    rxn = FMAKE_N(double, N_nodes, "LAGW:rxn");
    ryn = FMAKE_N(double, N_nodes, "LAGW:ryn");
    vxn = FMAKE_N(double, N_nodes, "LAGW:vxn");
    vyn = FMAKE_N(double, N_nodes, "LAGW:vyn");
    pn  = FMAKE_N(double, N_zones, "LAGW:pn");
    di  = FMAKE_N(double, N_zones, "LAGW:di");

    nhcycle = 0;
    dtsmin  = PARAM[12];
    dtmax   = 0.9999*dt;
    for (dtelapse = 0.0; dtelapse <= dtmax; dtelapse += dts)
        {nhcycle++;

	 SC_chunk_loop(areaw, frz, lrz, TRUE, NULL);

         cspeed(csp, p, rho);

         dtc = cw(csp);
         dta = dareadt(rx, ry, vx, vy);

         dts0 = ABS(dt-dtelapse);
         dts  = min(dtc, dts0);
         dts  = min(dta, dts);
         if (dts != dts0)
            dts = max(dts, dtsmin);

         fw(p, rx, ry, rxn, ryn,
	    vx, vy, vxn, vyn,
	    dts);

         volumew(voln, rxn, ryn);

         pw(pn, dts);

         for (j = frz; j <= lrz; j++)
             {dvol    = vol[j]/(voln[j] + SMALL);
              rho[j] *= dvol;
              p[j]    = pn[j];
              vol[j]  = voln[j];};

         for (i = frn; i <= lrn; i++)
             {rx[i] = rxn[i];
              ry[i] = ryn[i];
              vx[i] = vxn[i];
              vy[i] = vyn[i];};};

/* release temporary storage */
    SFREE(aq);
    SFREE(rxn);
    SFREE(ryn);
    SFREE(vxn);
    SFREE(vyn);
    SFREE(pn);
    SFREE(di);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                            PANACEA INTERFACE                             */

/*--------------------------------------------------------------------------*/

/* INTERN_LAG - intern the variables of the lagrange package
 *            - prior to writing the initial restart dump
 */

int intern_lag(void)
   {

    PA_control_set("lag");

    SWTCH[4]   = global_swtch[12];

    PARAM[12]  *= (global_param[3] - global_param[2]);
    PARAM[13]  *= (global_param[3] - global_param[2]);

    PA_INTERN(n,   "n");
    PA_INTERN(p,   "p");
    PA_INTERN(rho, "rho");

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LAG - dispatch to the correct lag routine */

int lag(PA_package *pck)
   {int j, lgz;
    double dvov, dvol, fact, lgdt;
    static int first = TRUE;

/* check that this package was requested */
    if (SWTCH[1] == FALSE)
       return(FALSE);

    if (first)
       PA_mark_space(pck);

/* start timing */
    PA_mark_time(pck);

/* initialize the lag calculation */
    init_cycle();

/* dispatch to the correct lag routine */
    lagw();

/* compute fractional volume change time step control */
    for (lgdt = HUGE, j = frz; j <= lrz; j++)
        {if (zone[j] != 0.0)
	    {dvov = ABS(volo[j] - voln[j])/voln[j];
	     if ((dvov > PARAM[5]) && (dt*PARAM[7] < lgdt))
	        {lgdt = dt*PARAM[7];
		 lgz = j;};};};
    pck->dt = lgdt;
    pck->dt_zone = lgz;

/* updating pressures, densities, areas, volumes */
    for (j = frz; j <= lrz; j++)
        {dvol    = volo[j]/(voln[j] + SMALL);
         rho[j] *= dvol;
         n[j]   *= dvol;};

/* fix the heating rates for the temperature equation */
    for (j = frz; j <= lrz; j++)
        {fact = 1.0/(vol[j]*n[j]*dt + SMALL);
         rpdv[j] *= fact;};

    PA_accm_time(pck);

    if (first)
       {PA_accm_space(pck);
        first = FALSE;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_LAG - initialize and allocate lag variables one time only */

int init_lag(PA_package *pck)
   {int i, j;
    double dvx, dvy;

    PA_PACKAGE_ON(pck) = SWTCH[1];

/* connect global zonal variables */
    PA_CONNECT(rho,  "rho",  TRUE);
    PA_CONNECT(n,    "n",    TRUE);
    PA_CONNECT(p,    "p",    TRUE);
    PA_CONNECT(rpdv, "rpdv", TRUE);

/* allocate local zonal variables */
    csp   = FMAKE_N(double, N_zones, "INIT_LAG:csp");
    dtvg  = FMAKE_N(double, N_zones, "INIT_LAG:dtvg");
    massz = FMAKE_N(double, N_zones, "INIT_LAG:massz");
    pdv   = FMAKE_N(double, N_zones, "INIT_LAG:pdv");
    voln  = FMAKE_N(double, N_zones, "INIT_LAG:voln");
    volo  = FMAKE_N(double, N_zones, "INIT_LAG:volo");

/* set some scalars */
    csmin  = PARAM[9];
    hgamma = 1.0 + 1.0/(PARAM[11] + SMALL);

/* initialize the zonal arrays */
    for (j = frz; j <= lrz; j++)
        {volo[j]  = vol[j];
         massz[j] = rho[j]*vol[j];};

/* nodal variables */
    lpm   = FMAKE_N(double, N_nodes, "INIT_LAG:lpm");
    dvxdt = FMAKE_N(double, N_nodes, "INIT_LAG:dvxdt");
    dvydt = FMAKE_N(double, N_nodes, "INIT_LAG:dvydt");
    massn = FMAKE_N(double, N_nodes, "INIT_LAG:massn");

    for (i = 0; i < N_nodes; i++)
        massn[i] = 0.0;

/* compute the node masses and the total energy */
    etoto = 0.0;
    node_massw(massn, rho, rx, ry);

    for (i = 0; i < N_nodes; i++)
        {dvx = vx[i];
         dvy = vy[i];
         etoto += 0.5*massn[i]*(dvx*dvx + dvy*dvy);};

    for (j = frz; j <= lrz; j++)
        etoto += p[j]*vol[j]/(hgamma - 1.0);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_CYCLE - initialize the lagrange state for this cycle */

int init_cycle(void)
   {int j;

    for (j = frz; j <= lrz; j++)
        p[j] = PARAM[8]*kBoltz*n[j]*tm[j];

    for (j = 0; j < N_zones; j++)
        {rpdv[j]  = 0.0;
         dvxdt[j] = 0.0;
         volo[j]  = vol[j];};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* CSPEED - compute CSP */

int cspeed(double *csp, double *p, double *rho)
   {int j;

    PA_set_value(csp, N_zones, 0.0);

    for (j = frz; j <= lrz; j++)
        csp[j] = 2.0*p[j]/(rho[j] + SMALL);

    for (j = frz; j <= lrz; j++)
        if (csp[j] < csmin)
           csp[j] = csmin;

    for (j = frz; j <= lrz; j++)
        csp[j] = sqrt(csp[j]);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
