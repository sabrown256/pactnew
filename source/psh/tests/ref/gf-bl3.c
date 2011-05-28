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

FIXNUM FF_ID(fe1_f, FE1_F)(str **ta1, int *sa2)
   {str *_lta1;
    int _lsa2;
    FIXNUM _rv;
    SC_address _adfe1;

    _lta1      = *(str **) ta1;
    _lsa2      = (int) *sa2;

    _adfe1.memaddr = (void *) fe1(_lta1, _lsa2);

    _rv = _adfe1.diskaddr;

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void FF_ID(fe2_f, FE2_F)(str **tdev, double *ax, double *ay, int *sn, pcons *ainfo, int *sl)
   {str *_ltdev;
    double *_lax;
    double *_lay;
    int _lsn;
    pcons *_lainfo;
    int _lsl;
    
    _ltdev     = *(str **) tdev;
    _lax       = (double *) ax;
    _lay       = (double *) ay;
    _lsn       = (int) *sn;
    _lainfo    = (pcons *) ainfo;
    _lsl       = (int) *sl;

    fe2(_ltdev, _lax, _lay, _lsn, _lainfo, _lsl);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
