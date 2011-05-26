
#include "sx_int.h"
#include "bl3_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fe1(SS_psides *si, object *argl)
   {str *_la1;
    int _la2;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_POINTER_I, &_la1,
            SC_INT_I, &_la2,
            0);

    _rv = fe1(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fe2(SS_psides *si, object *argl)
   {str *_ldev;
    double *_lx;
    double *_ly;
    int _ln;
    pcons *_linfo;
    int _ll;
    object *_lo;

    SS_args(si, argl,
            SC_POINTER_I, &_ldev,
            SC_DOUBLE_P_I, &_lx,
            SC_DOUBLE_P_I, &_ly,
            SC_INT_I, &_ln,
            SC_PCONS_P_I, &_linfo,
            SC_INT_I, &_ll,
            0);

    fe2(_ldev, _lx, _ly, _ln, _linfo, _ll);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void SX_install_bl3_bindings(SS_psides *si)
   {

    SS_install(si, "fe1s",
               "",
               SS_nargs,
               _SXI_fe1, SS_PR_PROC);

    SS_install(si, "fe2s",
               "",
               SS_nargs,
               _SXI_fe2, SS_PR_PROC);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
