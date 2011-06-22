/*
 * GF-BL6.C - F90 wrappers for bl6
 *          - NOTE: this file was automatically generated
 *          - any manual changes will not be effective
 *
 */

#include "cpyright.h"
#include "bl6_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa1(int a ARG(2,in), float b ARG(2.0,io), long c ARG(2L,out))| */

int FF_ID(fa1f, FA1F)(int *a, float *b, long *c)
   {int _la;
    float _lb;
    long _lc;
    int _rv;

    _la        = (int) *a;
    _lb        = (float) *b;
    _lc        = (long) *c;

    _rv = fa1(_la, _lb, _lc);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa2(int a ARG([1,2,3],in), int b ARG([1,2,3],io), int c ARG([1,2,3],out))| */

int FF_ID(fa2f, FA2F)(int *a, int *b, int *c)
   {int _la;
    int _lb;
    int _lc;
    int _rv;

    _la        = (int) *a;
    _lb        = (int) *b;
    _lc        = (int) *c;

    _rv = fa2(_la, _lb, _lc);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa3(int *a ARG([1,2,3],in), int *b ARG([1,2,3],io), int *c ARG([1,2,3],out))| */

int FF_ID(fa3f, FA3F)(int *a, int *b, int *c)
   {int *_la;
    int *_lb;
    int *_lc;
    int _rv;

    _la        = (int *) a;
    _lb        = (int *) b;
    _lc        = (int *) c;

    _rv = fa3(_la, _lb, _lc);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa4(int *a ARG(,in), int *b ARG(,io), int *c ARG(,out))| */

int FF_ID(fa4f, FA4F)(int *a, int *b, int *c)
   {int *_la;
    int *_lb;
    int *_lc;
    int _rv;

    _la        = (int *) a;
    _lb        = (int *) b;
    _lc        = (int *) c;

    _rv = fa4(_la, _lb, _lc);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
