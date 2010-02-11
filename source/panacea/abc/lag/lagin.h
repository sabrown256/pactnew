/*
 * LAGIN.H - internal header for the LAGRANGE PACKAGE
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "lag.h"
#include "global.h"
#include "mesh.h"

/*--------------------------------------------------------------------------*/

/*			       LAG CONSTANTS				    */

/*--------------------------------------------------------------------------*/

enum e_bc_type
   {FIX	   = 1,
    XVCNST = 2,
    YVCNST = 3,
    WALL   = 4};

typedef enum e_bc_type bc_type;

/*--------------------------------------------------------------------------*/

/*			       LAG VARIABLES				    */

/*--------------------------------------------------------------------------*/

/* LAG package variables */

extern int
 *indc,
 *idup,
 *irc;

extern double
 *ab,
 *csp,
 csmin,
 *dtvg,
 *dvxdt,
 *dvydt,
 *errad,
 etoto,
 hgamma,
 *lpm,
 *massn,
 *massz,
 *pdv,
 *tm,
 *voln,
 *volo,
 *vx,
 *vy,
 *zb;

/*--------------------------------------------------------------------------*/

/*				LAG FUNCTIONS				    */

/*--------------------------------------------------------------------------*/

/* LAG.C declarations */

extern int
 def_lag(PA_package *pck),
 cont_lag(PA_package *pck),
 intern_lag(void),
 lag(PA_package *pck),
 init_lag(PA_package *pck),
 init_cycle(void),
 cspeed(double *csp, double *p, double *rho);


/* LAGIN.C declarations */

extern int
 node_massw(double *mass, double *rho, double *rx, double *ry);

