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
#define PI        3.141592653589793238462643383279502884

#if 0
/* golden section - F^2 + 1 = F */
#define F         1.61803398874989484820458683436563811772
#define E         2.718281828459045235360287471352662497757

/* Euler-Mascheroni constant = lim(sum(1/i, i = 1,n) - ln(n), n -> inf) */
#define G         0.577215664901532860606512090082402431
#endif

#define RAD_DEG    57.295779513082323
#define DEG_RAD     0.017453292519943295

/*--------------------------------------------------------------------------*/

/*                          PROCEDURAL MACROS                               */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                         STRUCT DEFINITIONS                               */

/*--------------------------------------------------------------------------*/
    

typedef struct s_PM_constants PM_constants;

struct s_PM_constants

/* fundamental numerical constants */
   {long double pi;

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


