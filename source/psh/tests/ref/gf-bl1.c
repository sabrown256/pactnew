/*
 * GF-BL1.C - F90 wrappers for bl1
 *          - NOTE: this file was automatically generated
 *          - any manual changes will not be effective
 *
 */


#include "cpyright.h"
#include "bl1_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void FF_ID(wfav1f, WFAV1F)(void)
   {
    fav1();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

FIXNUM FF_ID(wfav2f, WFAV2F)(void)
   {FIXNUM _rv;

    _rv = fav2();

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

double FF_ID(wfav3f, WFAV3F)(void)
   {double _rv;

    _rv = fav3();

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int FF_ID(wfav4f, WFAV4F)(void)
   {int _rv;

    _rv = fav4();

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void *FF_ID(wfavp1f, WFAVP1F)(void)
   {void *_rv;

    _rv = favp1();

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

FIXNUM *FF_ID(wfavp2f, WFAVP2F)(void)
   {FIXNUM *_rv;

    _rv = favp2();

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

double *FF_ID(wfavp3f, WFAVP3F)(void)
   {double *_rv;

    _rv = favp3();

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void **FF_ID(wfavp4f, WFAVP4F)(void)
   {void **_rv;

    _rv = SC_ADD_POINTER(favp4());

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
