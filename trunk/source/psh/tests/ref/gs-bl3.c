
#include "sx_int.h"
#include "bl3_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *fe1(str *a1, int a2)| */

static object *_SXI_fe1(SS_psides *si, object *argl)
   {str *_la1;
    int _la2;
    str *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

    _la1       = NULL;
    _la2       = 0;
    SS_args(si, argl,
            SC_POINTER_I, &_la1,
            SC_INT_I, &_la2,
            0);

    _rv = fe1(_la1, _la2);
    _sz = SC_arrlen(_rv)/sizeof(str);
    _arr = PM_make_array("str", _sz, _rv);
    _lo  = SX_mk_C_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fe2(str *dev, double *x, double *y, int n, pcons *info, int l)| */

static object *_SXI_fe2(SS_psides *si, object *argl)
   {str *_ldev;
    double *_lx;
    double *_ly;
    int _ln;
    pcons *_linfo;
    int _ll;
    object *_lo;

    _ldev      = NULL;
    _lx        = NULL;
    _ly        = NULL;
    _ln        = 0;
    _linfo     = NULL;
    _ll        = 0;
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
               "Procedure: fe1s\n     Usage: (fe1s a1 a2)",
               SS_nargs,
               _SXI_fe1, SS_PR_PROC);

    SS_install(si, "fe2s",
               "Procedure: fe2s\n     Usage: (fe2s dev x y n info l)",
               SS_nargs,
               _SXI_fe2, SS_PR_PROC);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
