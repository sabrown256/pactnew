/*
 * GF-BL2.C - F90 wrappers for bl2
 *          - NOTE: this file was automatically generated
 *          - any manual changes will not be effective
 *
 */

#include "cpyright.h"
#include "sx_int.h"
#include "bl2_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fbcv1(int a, float b, double c, char d, int (*e)(void), PFInt f)| */

char *FF_ID(fbcv1f, FBCV1F)(int *a, float *b, double *c, char *d, int (*e)(void), PFInt f)
   {int _la;
    float _lb;
    double _lc;
    char _ld;
    char *_rv;

    _la        = (int) *a;
    _lb        = (float) *b;
    _lc        = (double) *c;
    _ld        = (char) *d;

    _rv = fbcv1(_la, _lb, _lc, _ld, e, f);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fbmcv1(int a, float b, double c, char d)| */

char *FF_ID(fbmcv1f, FBMCV1F)(int *a, float *b, double *c, char *d)
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

/* WRAP |char *fbmcr2(int *a, float *b, double *c, char *d)| */

char *FF_ID(fbmcr2f, FBMCR2F)(int *a, float *b, double *c, char *d, int sncd)
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
