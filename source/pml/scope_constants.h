/*
 * SCOPE_CONSTANTS.H - header supporting natural and physical constants
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_CONSTANTS

#define PCK_CONSTANTS

#undef PI
#define PI          3.141592653589793238462643383279502884

#define RAD_DEG    57.29577951308232087665461840231274
#define DEG_RAD     0.01745329251994329576913914624236579

/*--------------------------------------------------------------------------*/

/*                          PROCEDURAL MACROS                               */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                         STRUCT DEFINITIONS                               */

/*--------------------------------------------------------------------------*/
    

typedef struct s_PM_constants PM_constants;

struct s_PM_constants

/* fundamental numerical constants */
   {long double pi;                  /* ratio of circumference to diameter */
    long double e;                                     /* natural log base */
    long double f;                                         /* golden ratio */
    long double g;                            /* Euler-Mascheroni constant */

/* fundamental physical constants */
    double alpha;                     /* fine structure constant e^2/hbarc */
    double c;                                   /* speed of light (cm/sec) */
    double coulomb;                      /* coulomb in fundamental charges */
    double gn;          /* Newtonian gravitational constant (cm^3/g-sec^2) */
    double kboltz;                         /* Boltzman constant in (erg/k) */
    double hbar;                                        /* hbar in erg-sec */
    double m_e;                                      /* electron mass in g */
    double n0;                                        /* avagadro's number */

/* derived physical constansts */
    double qe;                                   /* electron charge in esu */
    double hbarc;                                       /* hbar*c in ev-cm */
    double m_a;                                   /* atomic mass unit in g */
    double m_e_ev;                                  /* electron mass in ev */
    double ryd;                               /* (m_e*c^2*alpha^2)/2 in ev */

/* derived conversion constants */
    double ev_erg;                                            /* ev to erg */
    double k_ev;                                           /* kelvin to ev */
    double icm_g;                                       /* inverse cm to g */
    double g_icm;                                       /* g to inverse cm */
    double icm_ev;                                     /* inverse cm to ev */
    double ev_icm;                                      /* ev toinverse cm */
    double icm_erg;                                   /* inverse cm to erg */
    double erg_icm;};                                 /* erg to inverse cm */

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern PM_constants
 PM_c;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* MLCONST.C - declarations */

extern void
 PM_physical_constants_cgs(void);


#ifdef __cplusplus
}
#endif

#endif


