
#include "sx_int.h"
#include "bl2_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fbcv1(SS_psides *si, object *argl)
   {int _la;
    float _lb;
    double _lc;
    char _ld;
    PFInt _le;
    PFInt _lf;
    char *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_FLOAT_I, &_lb,
            SC_DOUBLE_I, &_lc,
            SC_CHAR_I, &_ld,
            SC_POINTER_I, &_le,
            SC_POINTER_I, &_lf,
            0);

    _rv = fbcv1(_la, _lb, _lc, _ld, _le, _lf);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fbmcv1(SS_psides *si, object *argl)
   {int _la;
    float _lb;
    double _lc;
    char _ld;
    char *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_FLOAT_I, &_lb,
            SC_DOUBLE_I, &_lc,
            SC_CHAR_I, &_ld,
            0);

    _rv = fbmcv1(_la, _lb, _lc, _ld);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fbmcr2(SS_psides *si, object *argl)
   {int *_la;
    float *_lb;
    double *_lc;
    char *_ld;
    char *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_P_I, &_la,
            SC_FLOAT_P_I, &_lb,
            SC_DOUBLE_P_I, &_lc,
            SC_STRING_I, &_ld,
            0);

    _rv = fbmcr2(_la, _lb, _lc, _ld);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void SX_install_bl2_bindings(SS_psides *si)
   {

    SS_install(si, "fbcv1s",
               "",
               SS_nargs,
               _SXI_fbcv1, SS_PR_PROC);

    SS_install(si, "fbmcv1s",
               "",
               SS_nargs,
               _SXI_fbmcv1, SS_PR_PROC);

    SS_install(si, "fbmcr2s",
               "",
               SS_nargs,
               _SXI_fbmcr2, SS_PR_PROC);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
