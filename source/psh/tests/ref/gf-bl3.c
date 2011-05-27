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

FIXNUM FF_ID(fe1, FE1)(str **ta1, int *sa2)
   {str *_lta1;
    int _lsa2;
    FIXNUM _rv;

    _lta1      = (str *) *ta1;
    _lsa2      = (int) *sa2;

    _rv = SC_ADD_POINTER(fe1(_lta1, _lsa2));

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void FF_ID(fe2, FE2)(str **tdev, double *ax, double *ay, int *sn, pcons *ainfo, int *sl)
   {str *_ltdev;
    double *_lax;
    double *_lay;
    int _lsn;
    pcons *_lainfo;
    int _lsl;
    
    _ltdev     = (str *) *tdev;
    _lax       = (double *) ax;
    _lay       = (double *) ay;
    _lsn       = (int) *sn;
    _lainfo    = (pcons *) ainfo;
    _lsl       = (int) *sl;

    fe2(_ltdev, _lax, _lay, _lsn, _lainfo, _lsl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
