/*
 * GS-BL3.C - generated support routines for BL3
 *
 */

#include "cpyright.h"
#include "sx_int.h"
#include "bl3_int.h"
#include "gs-bl3.h"

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

/* local variable initializations */
    _la1       = NULL;
    _la2       = 0;

    SS_args(si, argl,
            G_STR_I, &_la1,
            SC_INT_I, &_la2,
            0);

    _rv = fe1(_la1, _la2);
    _sz = SC_arrlen(_rv)/sizeof(str);
    _arr = PM_make_array("str", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

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

/* local variable initializations */
    _ldev      = NULL;
    _lx        = NULL;
    _ly        = NULL;
    _ln        = 0;
    _linfo     = NULL;
    _ll        = 0;

    SS_args(si, argl,
            G_STR_I, &_ldev,
            SC_DOUBLE_P_I, &_lx,
            SC_DOUBLE_P_I, &_ly,
            SC_INT_I, &_ln,
            SC_PCONS_P_I, &_linfo,
            SC_INT_I, &_ll,
            0);

    fe2(_ldev, _lx, _ly, _ln, _linfo, _ll);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void _SX_install_bl3_consts(SS_psides *si)
   {


    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void _SX_wr_str(SS_psides *si, object *o, object *fp)
   {str *x;

    x = SS_GET(str, o);

    PRINT(SS_OUTSTREAM(fp), "<STR|%s>", _SX_opt_generic(x, BIND_PRINT, NULL));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void _SX_rl_str(SS_psides *si, object *o)
   {str *x;

    x = SS_GET(str, o);

    _SX_opt_generic(x, BIND_FREE, NULL);

    SS_rl_object(si, o);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


object *SX_make_str(SS_psides *si, str *x)
   {object *rv;

    if (x == NULL)
       rv = SS_null;
    else
       {char *nm;

        _SX_opt_generic(x, BIND_ALLOC, NULL);
        nm = _SX_opt_generic(x, BIND_LABEL, NULL);
        rv = SS_mk_object(si, x, G_STR_I, SELF_EV, nm,
                          _SX_wr_str, _SX_rl_str);}

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void *_SX_arg_str(SS_psides *si, object *o)
   {void *rv;

    rv = _SX_opt_generic(NULL, BIND_ARG, o);

    if (rv == _SX.unresolved)
       SS_error(si, "OBJECT NOT STR - _SX_ARG_STR", o);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_strp(SS_psides *si, object *o)
   {object *rv;

    rv = SX_STRP(o) ? SS_t : SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int _SX_install_bl3_derived(SS_psides *si)
   {int nerr;

    register_bl3_types();

    nerr = TRUE;

    nerr &= G_DEFINE_STR(SX_gs.vif);

    SS_install(si, "str?",
               "Returns #t if the object is a str, and #f otherwise",
               SS_sargs,
               _SXI_strp, SS_PR_PROC);

    SS_set_type_method(G_STR_I,
                       "C->Scheme", SX_make_str,
                       "Scheme->C", _SX_arg_str,
                       NULL);

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void SX_install_bl3_bindings(SS_psides *si)
   {

    _SX_install_bl3_consts(si);
    _SX_install_bl3_derived(si);

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
