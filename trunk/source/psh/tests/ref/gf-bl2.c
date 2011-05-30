/*
 * GF-BL2.C - F90 wrappers for bl2
 *          - NOTE: this file was automatically generated
 *          - any manual changes will not be effective
 *
 */

#include "cpyright.h"
#include "bl2_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char *FF_ID(fbcv1_f, FBCV1_F)(int *a, float *b, double *c, char *d, PFInt e, PFInt f)
   {int _la;
    float _lb;
    double _lc;
    char _ld;
    PFInt _le;
    PFInt _lf;
    char *_rv;

    _la        = (int) *a;
    _lb        = (float) *b;
    _lc        = (double) *c;
    _ld        = (char) *d;
    _le        = (PFInt) e;
    _lf        = (PFInt) f;

    _rv = fbcv1(_la, _lb, _lc, _ld, _le, _lf);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char *FF_ID(fbmcv1_f, FBMCV1_F)(int *a, float *b, double *c, char *d)
   {int _la;
    float _lb;
    double _lc;
    char _ld;
    char *_rv;

    _la        = (int) *a;
    _lb        = (float) *b;
    _lc        = (double) *c;
    _ld        = (char) *d;

    _rv = fbmcv1(_la, _lb, _lc, _ld);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char *FF_ID(fbmcr2_f, FBMCR2_F)(int *a, float *b, double *c, char *d, int sncd)
   {int *_la;
    float *_lb;
    double *_lc;
    char _ld[MAXLINE];
    char *_rv;

    _la        = (int *) a;
    _lb        = (float *) b;
    _lc        = (double *) c;
    SC_FORTRAN_STR_C(_ld, d, sncd);

    _rv = fbmcr2(_la, _lb, _lc, _ld);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
