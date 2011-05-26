/*
 * GF-BL3.C - F90 wrappers for bl3
 *          - NOTE: this file was automatically generated
 *          - any manual changes will not be effective
 *
 */


#include "cpyright.h"
#include "bl3_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

FIXNUM FF_ID(wfe1f, WFE1F)(str **ta1, int *sa2)
   {str *_lta1;
    int _lsa2;
    FIXNUM _rv;

    _lta1      = (str *) *ta1;
    _lsa2      = (int) *sa2;

    _rv = SC_ADD_POINTER(fe1(_lta1, _lsa2));

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
