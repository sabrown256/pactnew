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

#include "scope_complex.h"

/*--------------------------------------------------------------------------*/

/*                          PROCEDURAL MACROS                               */

/*--------------------------------------------------------------------------*/

#undef PI
#define PI        PM_c.pi

#define RAD_DEG   PM_c.rad_deg
#define DEG_RAD   PM_c.deg_rad

#define Czero     PM_c.zero.c
#define Cone      PM_c.one.c
#define CPHUGE    PM_c.hugep.c
#define CMHUGE    PM_c.hugem.c

#define Qzero     PM_c.zero.q
#define Qone      PM_c.one.q
#define QPHUGE    PM_c.hugep.q
#define QMHUGE    PM_c.hugem.q

/*--------------------------------------------------------------------------*/

/*                         STRUCT DEFINITIONS                               */

/*--------------------------------------------------------------------------*/
    

typedef struct s_multi_rep_fix multi_rep_fix;
typedef struct s_multi_rep_float multi_rep_float;
typedef struct s_PM_constants PM_constants;

/* use multi_rep_fix to contain representation of a value
 * in several forms
 */

struct s_multi_rep_fix
   {short s;
    int i;
    long l;
    long long L;};

/* use multi_rep_float to contain representation of a value
 * in several forms
 */

struct s_multi_rep_float
   {float f;
    double d;
    long double l;
    complex c;
    quaternion q;};

struct s_PM_constants
   {multi_rep_fix izero;
    multi_rep_fix ione;
    multi_rep_fix imax;

    multi_rep_float zero;
    multi_rep_float smallp;
    multi_rep_float smallm;
    multi_rep_float one;
    multi_rep_float hugep;
    multi_rep_float hugem;

/* fundamental numerical constants */
    long double pi;                  /* ratio of circumference to diameter */
    long double e;                                     /* natural log base */
    long double f;                                         /* golden ratio */
    long double g;                            /* Euler-Mascheroni constant */

    long double rad_deg;
    long double deg_rad;

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


