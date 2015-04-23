/*
 * GF-BL3.C - F90 wrappers for bl3
 *          - NOTE: this file was automatically generated
 *          - any manual changes will not be effective
 *
 */

#include "cpyright.h"
#include "sx_int.h"
#include "bl3_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *fe1(str *a1, int a2)| */

void *FF_ID(fe1f, FE1F)(str **a1, int *a2)
   {str *_la1;
    int _la2;
    void *_rv;

    _la1       = *(str **) a1;
    _la2       = (int) *a2;

    _rv = (void *) fe1(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fe2(str *dev, double *x, double *y, int n, int l)| */

void FF_ID(fe2f, FE2F)(str **dev, double *x, double *y, int *n, int *l)
   {str *_ldev;
    double *_lx;
    double *_ly;
    int _ln;
    int _ll;
    
    _ldev      = *(str **) dev;
    _lx        = (double *) x;
    _ly        = (double *) y;
    _ln        = (int) *n;
    _ll        = (int) *l;

    fe2(_ldev, _lx, _ly, _ln, _ll);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/