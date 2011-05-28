
#include "sx_int.h"
#include "bl4_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fb1(SS_psides *si, object *argl)
   {bool _la1;
    bool *_la2;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_BOOL_I, &_la1,
            SC_BOOL_P_I, &_la2,
            0);

    _rv = fb1(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fb2(SS_psides *si, object *argl)
   {char _la1;
    char *_la2;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_CHAR_I, &_la1,
            SC_STRING_I, &_la2,
            0);

    _rv = fb2(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fb3(SS_psides *si, object *argl)
   {short _la1;
    short *_la2;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_SHORT_I, &_la1,
            SC_SHORT_P_I, &_la2,
            0);

    _rv = fb3(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fb4(SS_psides *si, object *argl)
   {int _la1;
    int *_la2;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            SC_INT_P_I, &_la2,
            0);

    _rv = fb4(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fb5(SS_psides *si, object *argl)
   {long _la1;
    long *_la2;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_LONG_I, &_la1,
            SC_LONG_P_I, &_la2,
            0);

    _rv = fb5(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fb6(SS_psides *si, object *argl)
   {long long _la1;
    long long *_la2;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_LONG_LONG_I, &_la1,
            SC_LONG_LONG_P_I, &_la2,
            0);

    _rv = fb6(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fb7(SS_psides *si, object *argl)
   {float _la1;
    float *_la2;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_FLOAT_I, &_la1,
            SC_FLOAT_P_I, &_la2,
            0);

    _rv = fb7(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fb8(SS_psides *si, object *argl)
   {double _la1;
    double *_la2;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_DOUBLE_I, &_la1,
            SC_DOUBLE_P_I, &_la2,
            0);

    _rv = fb8(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fb10(SS_psides *si, object *argl)
   {float _Complex _la1;
    float _Complex *_la2;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_FLOAT_COMPLEX_I, &_la1,
            SC_POINTER_I, &_la2,
            0);

    _rv = fb10(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fb11(SS_psides *si, object *argl)
   {double _Complex _la1;
    double _Complex *_la2;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_DOUBLE_COMPLEX_I, &_la1,
            SC_POINTER_I, &_la2,
            0);

    _rv = fb11(_la1, _la2);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr1(SS_psides *si, object *argl)
   {int _la1;
    bool _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr1(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr2(SS_psides *si, object *argl)
   {int _la1;
    char _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr2(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr3(SS_psides *si, object *argl)
   {int _la1;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr3(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr4(SS_psides *si, object *argl)
   {int _la1;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr4(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr5(SS_psides *si, object *argl)
   {int _la1;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr5(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr6(SS_psides *si, object *argl)
   {int _la1;
    FIXNUM _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr6(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr7(SS_psides *si, object *argl)
   {int _la1;
    float _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr7(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr8(SS_psides *si, object *argl)
   {int _la1;
    double _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr8(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr10(SS_psides *si, object *argl)
   {int _la1;
    float _Complex _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr10(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr11(SS_psides *si, object *argl)
   {int _la1;
    double _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr11(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fr13(SS_psides *si, object *argl)
   {int _la1;
    str _rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fr13(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp1(SS_psides *si, object *argl)
   {int _la1;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp1(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp2(SS_psides *si, object *argl)
   {int _la1;
    char *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp2(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp3(SS_psides *si, object *argl)
   {int _la1;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp3(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp4(SS_psides *si, object *argl)
   {int _la1;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp4(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp5(SS_psides *si, object *argl)
   {int _la1;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp5(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp6(SS_psides *si, object *argl)
   {int _la1;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp6(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp7(SS_psides *si, object *argl)
   {int _la1;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp7(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp8(SS_psides *si, object *argl)
   {int _la1;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp8(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp10(SS_psides *si, object *argl)
   {int _la1;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp10(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp11(SS_psides *si, object *argl)
   {int _la1;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp11(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static object *_SXI_fp13(SS_psides *si, object *argl)
   {int _la1;
    void *_rv;
    object *_lo;

    SS_args(si, argl,
            SC_INT_I, &_la1,
            0);

    _rv = fp13(_la1);
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void SX_install_bl4_bindings(SS_psides *si)
   {

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fb1, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fb2, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fb3, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fb4, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fb5, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fb6, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fb7, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fb8, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fb10, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fb11, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr1, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr2, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr3, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr4, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr5, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr6, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr7, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr8, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr10, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr11, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fr13, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp1, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp2, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp3, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp4, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp5, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp6, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp7, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp8, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp10, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp11, SS_PR_PROC);

    SS_install(si, "yes",
               "",
               SS_nargs,
               _SXI_fp13, SS_PR_PROC);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
