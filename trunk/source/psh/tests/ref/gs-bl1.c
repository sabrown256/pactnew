
#include "cpyright.h"
#include "sx_int.h"
#include "bl1_int.h"
#include "gs-bl1.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fav1(void)| */

static object *_SXI_fav1(SS_psides *si, object *argl)
   {object *_lo;

    fav1();
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fav2(void)| */

static object *_SXI_fav2(SS_psides *si, object *argl)
   {int _rv;
    object *_lo;

    _rv = fav2();
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double fav3(void)| */

static object *_SXI_fav3(SS_psides *si, object *argl)
   {double _rv;
    object *_lo;

    _rv = fav3();
    _lo = SS_mk_float(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str fav4(void)| */

static object *_SXI_fav4(SS_psides *si, object *argl)
   {str _rv;
    object *_lo;

    _rv = fav4();

/* no way to return 'str' */
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void *favp1(void)| */

static object *_SXI_favp1(SS_psides *si, object *argl)
   {void *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

    _rv = favp1();
    _sz = SC_arrlen(_rv);
    _arr = PM_make_array("char", _sz, _rv);
    _lo  = SX_mk_C_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int *favp2(void)| */

static object *_SXI_favp2(SS_psides *si, object *argl)
   {int *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

    _rv = favp2();
    _sz = SC_arrlen(_rv)/sizeof(int);
    _arr = PM_make_array("int", _sz, _rv);
    _lo  = SX_mk_C_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double *favp3(void)| */

static object *_SXI_favp3(SS_psides *si, object *argl)
   {double *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

    _rv = favp3();
    _sz = SC_arrlen(_rv)/sizeof(double);
    _arr = PM_make_array("double", _sz, _rv);
    _lo  = SX_mk_C_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *favp4(void)| */

static object *_SXI_favp4(SS_psides *si, object *argl)
   {str *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

    _rv = favp4();
    _sz = SC_arrlen(_rv)/sizeof(str);
    _arr = PM_make_array("str", _sz, _rv);
    _lo  = SX_mk_C_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fav5(void)| */

static object *_SXI_fav5(SS_psides *si, object *argl)
   {object *_lo;

    fav5();
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void SX_install_bl1_bindings(SS_psides *si)
   {

    _SX_install_bl1_consts(si);
    _SX_install_bl1_derived(si);

    SS_install(si, "fav1s",
               "Procedure: fav1s\n     Usage: (fav1s)",
               SS_zargs,
               _SXI_fav1, SS_PR_PROC);

    SS_install(si, "fav2s",
               "Procedure: fav2s\n     Usage: (fav2s)",
               SS_zargs,
               _SXI_fav2, SS_PR_PROC);

    SS_install(si, "fav3s",
               "Procedure: fav3s\n     Usage: (fav3s)",
               SS_zargs,
               _SXI_fav3, SS_PR_PROC);

    SS_install(si, "fav4s",
               "Procedure: fav4s\n     Usage: (fav4s)",
               SS_zargs,
               _SXI_fav4, SS_PR_PROC);

    SS_install(si, "favp1s",
               "Procedure: favp1s\n     Usage: (favp1s)",
               SS_zargs,
               _SXI_favp1, SS_PR_PROC);

    SS_install(si, "favp2s",
               "Procedure: favp2s\n     Usage: (favp2s)",
               SS_zargs,
               _SXI_favp2, SS_PR_PROC);

    SS_install(si, "favp3s",
               "Procedure: favp3s\n     Usage: (favp3s)",
               SS_zargs,
               _SXI_favp3, SS_PR_PROC);

    SS_install(si, "favp4s",
               "Procedure: favp4s\n     Usage: (favp4s)",
               SS_zargs,
               _SXI_favp4, SS_PR_PROC);

    SS_install(si, "fav5",
               "Procedure: fav5\n     Usage: (fav5)",
               SS_zargs,
               _SXI_fav5, SS_PR_PROC);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
