/*
 * PML_INT.H - internal header support Portable Mathematics Library
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_PML_INT

#define PCK_PML_INT

#include "score_int.h"
#include "pml.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                           STRUCTs and TYPEDEFs                           */

/*--------------------------------------------------------------------------*/

typedef double          (*PFDoubled)(double);
typedef double          (*PFDoubledd)(double, double);

typedef double _Complex (*PFComplexc)(double _Complex);
typedef double _Complex (*PFComplexcc)(double _Complex, double _Complex);
typedef double _Complex (*PFComplexcd)(double _Complex, double);

typedef quaternion      (*PFQuaternionq)(quaternion);
typedef quaternion      (*PFQuaternionqq)(quaternion, quaternion);
typedef quaternion      (*PFQuaternionqd)(quaternion, double);

typedef int             (*PFIntd)(double);
typedef int             (*PFIntdd)(double, double);
typedef int64_t         (*PFInt64I)(int64_t);
typedef int64_t         (*PFInt64II)(int64_t, int64_t);

typedef struct s_PM_state PM_state;
typedef struct s_PM_smp_state PM_smp_state;

struct s_PM_state
   {
 
/* initializes to non-zero values */

/* MLSMP.C */
    int ita;

/* MLNLS.C */
    double mlt;
    double precision;

/* MLGEOM.C */
    double tol;

/* MLSFNC.C */
    double igamma_tol;

/* initializes to 0 bytes */

/* MLRAND.C */
    unsigned short dseed[3];
    double rn;

/* MLOPER.C */
    int64_t bmx;

/* MLFPE.C */
    int fpe_enabled;
    unsigned int features;
    unsigned int mask;

/* MLCFNC.C */
    double ln10e;

/* MLCMSH.C */
    int ncv;
    JMP_BUF mesh_err;};

struct s_PM_smp_state
   {long last;

    double dseed[2];
    double dmult[2];
    double dadder;
    unsigned short multplr[3];};

/*--------------------------------------------------------------------------*/

/*                          VARIABLE DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

extern PM_state
 _PM;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/


/* MLICCG.C declarations */

extern double
 _PM_iccg_v(int km, int lm, double eps, int ks, int maxit,
	    double *a0, double *a1, double *b0, double *b1, double *bm1,
	    double *x, double *y);


/* MLICGS.C declarations */

extern double
 _PM_iccg_s(int km, int lm, double eps, int maxit,
	    double *a0, double *a1, double *b0, double *b1, double *bm1,
	    double *x, double *y, int neq, int method);


/* MLGEOM.C declarations */

extern int
 _PM_cross(double *x1, double *x2, double *x3, double *x4,
	   double *x0, int line1, int line2);


/* MLMATR.C declarations */

extern PM_matrix
 *_PM_negative_times(PM_matrix *m, PM_matrix *a, PM_matrix *b),
 *_PM_times(PM_matrix *c, PM_matrix *a, PM_matrix *b),
 *_PM_plus(PM_matrix *c, PM_matrix *a, PM_matrix *b),
 *_PM_minus(PM_matrix *c, PM_matrix *a, PM_matrix *b),
 *_PM_transpose(PM_matrix *m, PM_matrix *a);


/* MLSMP.C declarations */

extern PM_smp_state
 *_PM_get_state(int id);


/* MLSRCH.C declarations */

extern int
 _PM_find_value(int *nout, int **out,
		int nx, char *type,
		void *x, int (*prd)(double u, double v), double val,
		int nin, int *in);


#ifdef __cplusplus
}
#endif

#endif

