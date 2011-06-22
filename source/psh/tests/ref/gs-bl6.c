
#include "sx_int.h"
#include "bl6_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa1(int a ARG(2,in), float b ARG(2.0,io), long c ARG(2L,out))| */

static object *_SXI_fa1(SS_psides *si, object *argl)
   {int _la;
    float _lb;
    long _lc;
    int _rv;
    object *_lo;

    _la        = 2;
    _lb        = 2.0;
    _lc        = 2L;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_FLOAT_I, &_lb,
            SC_LONG_I, &_lc,
            0);

    _rv = fa1(_la, _lb, _lc);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa2(int a ARG([1,2,3],in), int b ARG([1,2,3],io), int c ARG([1,2,3],out))| */

static object *_SXI_fa2(SS_psides *si, object *argl)
   {int _la;
    int _lb;
    int _lc;
    int _rv;
    object *_lo;

    _la        = NULL;
    _lb        = NULL;
    _lc        = NULL;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb,
            SC_INT_I, &_lc,
            0);

    _rv = fa2(_la, _lb, _lc);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa3(int *a ARG([1,2,3],in), int *b ARG([1,2,3],io), int *c ARG([1,2,3],out))| */

static object *_SXI_fa3(SS_psides *si, object *argl)
   {int *_la;
    int *_lb;
    int *_lc;
    int _rv;
    object *_lo;

    _la        = NULL;
    _lb        = NULL;
    _lc        = NULL;
    SS_args(si, argl,
            SC_INT_P_I, &_la,
            SC_INT_P_I, &_lb,
            SC_INT_P_I, &_lc,
            0);

    _rv = fa3(_la, _lb, _lc);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa4(int *a ARG(,in), int *b ARG(,io), int *c ARG(,out))| */

static object *_SXI_fa4(SS_psides *si, object *argl)
   {int *_la;
    int *_lb;
    int *_lc;
    int _rv;
    object *_lo;

    _la        = NULL;
    _lb        = NULL;
    _lc        = NULL;
    SS_args(si, argl,
            SC_INT_P_I, &_la,
            SC_INT_P_I, &_lb,
            SC_INT_P_I, &_lc,
            0);

    _rv = fa4(_la, _lb, _lc);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void SX_install_bl6_bindings(SS_psides *si)
   {

    SS_install(si, "fa1s",
               "Procedure: fa1s\n     Usage: (fa1s a b c)",
               SS_nargs,
               _SXI_fa1, SS_PR_PROC);

    SS_install(si, "fa2s",
               "Procedure: fa2s\n     Usage: (fa2s a b c)",
               SS_nargs,
               _SXI_fa2, SS_PR_PROC);

    SS_install(si, "fa3s",
               "Procedure: fa3s\n     Usage: (fa3s a b c)",
               SS_nargs,
               _SXI_fa3, SS_PR_PROC);

    SS_install(si, "fa4s",
               "Procedure: fa4s\n     Usage: (fa4s a b c)",
               SS_nargs,
               _SXI_fa4, SS_PR_PROC);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
