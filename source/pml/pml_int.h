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

/* initializes to 0 bytes */

/* MLRAND.C */
    unsigned short dseed[3];
    double rn;

/* MLOPER.C */
    BIGINT bmx;

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


/* MLINTS.C declarations */

extern void
 _PM_spline(double *x, double *y, int n, double yp1, double ypn, double *d2y);


/* MLGEOM.C declarations */

extern int
 _PM_cross(double x1, double y1, double x2, double y2,
	   double x3, double y3, double x4, double y4,
	   double *px0, double *py0,
	   int line1, int line2);


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

