/*
 * MLCONST.C - numerical and physical constants
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */
 
#include "cpyright.h"

#include "pml_int.h"

PM_constants
 PM_c;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_PHYSICAL_CONSTANTS_CGS - set up some useful conversion and physical
 *                            - constants in CGS units
 *                            - various compiler systems require explicit
 *                            - initializations
 */

void PM_physical_constants_cgs(void)
   {

/* fundamental numerical constants */

/* Pi */
    PM_c.pi = acosl(-1.0);

/* e */
    PM_c.e  = expl(1.0);

/* golden ratio */
    PM_c.f  = 1.618033988749894848207210029666925L;

/* Euler-Mascheroni constant - gamma */
    PM_c.g  = 0.5772156649015328606163073577040024L;

/* fundamental physical constants */
    PM_c.alpha   = 7.297352569824e-03; /* fine structure constant e^2/HbarC */
    PM_c.c       = 2.997924580e+10;              /* speed of light (cm/sec) */
    PM_c.coulomb = 6.24150934190e+18;    /* fundamental charges per Coulomb */
    PM_c.gn      = 6.6738480e-08;  /* gravitational constant (cm^3/g-sec^2) */
    PM_c.kboltz  = 1.380648813e-16;            /* Boltzman constant (erg/K) */
    PM_c.hbar    = 1.05457172647e-27;                    /* Hbar in erg-sec */
    PM_c.m_e     = 9.1093829140e-28;                  /* electron mass in g */
    PM_c.n0      = 6.0221412927e+23;                   /* Avagadro's number */

/* eV to erg */
    PM_c.ev_erg  = 1.0e7/PM_c.coulomb;

/* derived physical constansts */

/* hbar*c in eV-cm */
    PM_c.hbarc   = PM_c.hbar*PM_c.c/PM_c.ev_erg;

/* electron charge in esu */
    PM_c.qe      = sqrt(PM_c.alpha*PM_c.hbar*PM_c.c);

/* atomic mass unit in g */
    PM_c.m_a     = 1.0/PM_c.n0;

/* electron mass in eV */
    PM_c.m_e_ev  = PM_c.m_e*PM_c.c*PM_c.c/PM_c.ev_erg;

/* Rydberg in eV */
    PM_c.ryd     = 0.5*PM_c.m_e*PM_c.c*PM_c.c*PM_c.alpha*PM_c.alpha /
                   PM_c.ev_erg;

/* derived conversion constants */

/* K to eV */
    PM_c.k_ev    = PM_c.kboltz/PM_c.ev_erg;

/* inverse cm to g */
    PM_c.icm_g   = PM_c.hbar/PM_c.c;

/* g to inverse cm */
    PM_c.g_icm   = 1.0/PM_c.icm_g;

/* inverse cm to eV */
    PM_c.icm_ev  = PM_c.hbar*PM_c.c/PM_c.ev_erg;

/* eV to inverse cm */
    PM_c.ev_icm  = 1.0/PM_c.icm_ev;

/* inverse cm to erg */
    PM_c.icm_erg = PM_c.hbar*PM_c.c;

/* erg to inverse cm */
    PM_c.erg_icm = 1.0/PM_c.icm_erg;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
