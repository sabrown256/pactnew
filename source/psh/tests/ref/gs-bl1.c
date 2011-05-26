
#include "sx_int.h"
#include "bl1_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fav1(SS_psides *si, object *argl)
   {object *_lo;

    fav1();
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fav2(SS_psides *si, object *argl)
   {FIXNUM _rv;
    object *_lo;

    _rv = fav2();
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fav3(SS_psides *si, object *argl)
   {double _rv;
    object *_lo;

    _rv = fav3();
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fav4(SS_psides *si, object *argl)
   {str _rv;
    object *_lo;

    _rv = fav4();
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_favp1(SS_psides *si, object *argl)
   {void *_rv;
    object *_lo;

    _rv = favp1();
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_favp2(SS_psides *si, object *argl)
   {void *_rv;
    object *_lo;

    _rv = favp2();
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_favp3(SS_psides *si, object *argl)
   {void *_rv;
    object *_lo;

    _rv = favp3();
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_favp4(SS_psides *si, object *argl)
   {void *_rv;
    object *_lo;

    _rv = favp4();
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void SX_install_bl1_bindings(SS_psides *si)
   {

    SS_install(si, "fav1s",
               "",
               SS_nargs,
               _SXI_fav1, SS_PR_PROC);

    SS_install(si, "fav2s",
               "",
               SS_nargs,
               _SXI_fav2, SS_PR_PROC);

    SS_install(si, "fav3s",
               "",
               SS_nargs,
               _SXI_fav3, SS_PR_PROC);

    SS_install(si, "fav4s",
               "",
               SS_nargs,
               _SXI_fav4, SS_PR_PROC);

    SS_install(si, "favp1s",
               "",
               SS_nargs,
               _SXI_favp1, SS_PR_PROC);

    SS_install(si, "favp2s",
               "",
               SS_nargs,
               _SXI_favp2, SS_PR_PROC);

    SS_install(si, "favp3s",
               "",
               SS_nargs,
               _SXI_favp3, SS_PR_PROC);

    SS_install(si, "favp4s",
               "",
               SS_nargs,
               _SXI_favp4, SS_PR_PROC);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
