/*
 * GS-BL4.C - generated support routines for BL4
 *
 */

#include "cpyright.h"
#include "sx_int.h"
#include "bl4_int.h"
#include "gs-bl4.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb1(bool a1, bool *a2)| */

static object *_SXI_fb1(SS_psides *si, object *argl)
   {bool _la1;
    bool *_la2;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = FALSE;
    _la2       = NULL;

    SS_args(si, argl,
            G_BOOL_I, &_la1,
            G_BOOL_P_I, &_la2,
            0);

    _rv = fb1(_la1, _la2);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb2(char a1, char *a2)| */

static object *_SXI_fb2(SS_psides *si, object *argl)
   {char _la1;
    char *_la2;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = '\0';
    _la2       = NULL;

    SS_args(si, argl,
            G_CHAR_I, &_la1,
            G_STRING_I, &_la2,
            0);

    _rv = fb2(_la1, _la2);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb3(short a1, short *a2)| */

static object *_SXI_fb3(SS_psides *si, object *argl)
   {short _la1;
    short *_la2;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;
    _la2       = NULL;

    SS_args(si, argl,
            G_SHORT_I, &_la1,
            G_SHORT_P_I, &_la2,
            0);

    _rv = fb3(_la1, _la2);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb4(int a1, int *a2)| */

static object *_SXI_fb4(SS_psides *si, object *argl)
   {int _la1;
    int *_la2;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;
    _la2       = NULL;

    SS_args(si, argl,
            G_INT_I, &_la1,
            G_INT_P_I, &_la2,
            0);

    _rv = fb4(_la1, _la2);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb5(long a1, long *a2)| */

static object *_SXI_fb5(SS_psides *si, object *argl)
   {long _la1;
    long *_la2;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0L;
    _la2       = NULL;

    SS_args(si, argl,
            G_LONG_I, &_la1,
            G_LONG_P_I, &_la2,
            0);

    _rv = fb5(_la1, _la2);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb6(long long a1, long long *a2)| */

static object *_SXI_fb6(SS_psides *si, object *argl)
   {long long _la1;
    long long *_la2;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0LL;
    _la2       = NULL;

    SS_args(si, argl,
            G_LONG_LONG_I, &_la1,
            G_LONG_LONG_P_I, &_la2,
            0);

    _rv = fb6(_la1, _la2);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb7(float a1, float *a2)| */

static object *_SXI_fb7(SS_psides *si, object *argl)
   {float _la1;
    float *_la2;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0.0;
    _la2       = NULL;

    SS_args(si, argl,
            G_FLOAT_I, &_la1,
            G_FLOAT_P_I, &_la2,
            0);

    _rv = fb7(_la1, _la2);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb8(double a1, double *a2)| */

static object *_SXI_fb8(SS_psides *si, object *argl)
   {double _la1;
    double *_la2;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0.0;
    _la2       = NULL;

    SS_args(si, argl,
            G_DOUBLE_I, &_la1,
            G_DOUBLE_P_I, &_la2,
            0);

    _rv = fb8(_la1, _la2);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb10(float _Complex a1, float _Complex *a2)| */

static object *_SXI_fb10(SS_psides *si, object *argl)
   {float _Complex _la1;
    float _Complex *_la2;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0.0;
    _la2       = NULL;

    SS_args(si, argl,
            G_FLOAT_COMPLEX_I, &_la1,
            G_FLOAT_COMPLEX_P_I, &_la2,
            0);

    _rv = fb10(_la1, _la2);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb11(double _Complex a1, double _Complex *a2)| */

static object *_SXI_fb11(SS_psides *si, object *argl)
   {double _Complex _la1;
    double _Complex *_la2;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0.0;
    _la2       = NULL;

    SS_args(si, argl,
            G_DOUBLE_COMPLEX_I, &_la1,
            G_DOUBLE_COMPLEX_P_I, &_la2,
            0);

    _rv = fb11(_la1, _la2);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |bool fr1(int a1)| */

static object *_SXI_fr1(SS_psides *si, object *argl)
   {int _la1;
    bool _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr1(_la1);
    _lo = SS_mk_boolean(si, "g", (int) _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char fr2(int a1)| */

static object *_SXI_fr2(SS_psides *si, object *argl)
   {int _la1;
    char _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr2(_la1);
    _lo = SS_mk_char(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |short fr3(int a1)| */

static object *_SXI_fr3(SS_psides *si, object *argl)
   {int _la1;
    short _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr3(_la1);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fr4(int a1)| */

static object *_SXI_fr4(SS_psides *si, object *argl)
   {int _la1;
    int _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr4(_la1);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long fr5(int a1)| */

static object *_SXI_fr5(SS_psides *si, object *argl)
   {int _la1;
    long _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr5(_la1);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long long fr6(int a1)| */

static object *_SXI_fr6(SS_psides *si, object *argl)
   {int _la1;
    long long _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr6(_la1);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float fr7(int a1)| */

static object *_SXI_fr7(SS_psides *si, object *argl)
   {int _la1;
    float _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr7(_la1);
    _lo = SS_mk_float(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double fr8(int a1)| */

static object *_SXI_fr8(SS_psides *si, object *argl)
   {int _la1;
    double _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr8(_la1);
    _lo = SS_mk_float(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float _Complex fr10(int a1)| */

static object *_SXI_fr10(SS_psides *si, object *argl)
   {int _la1;
    float _Complex _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr10(_la1);
    _lo = SS_mk_complex(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double _Complex fr11(int a1)| */

static object *_SXI_fr11(SS_psides *si, object *argl)
   {int _la1;
    double _Complex _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr11(_la1);
    _lo = SS_mk_complex(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str fr13(int a1)| */

static object *_SXI_fr13(SS_psides *si, object *argl)
   {int _la1;
    str _rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fr13(_la1);

/* no way to return 'str' */
    _lo = SS_null;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |bool *fp1(int a1)| */

static object *_SXI_fp1(SS_psides *si, object *argl)
   {int _la1;
    bool *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp1(_la1);
    _sz = SC_arrlen(_rv)/sizeof(bool);
    _arr = PM_make_array("bool", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fp2(int a1)| */

static object *_SXI_fp2(SS_psides *si, object *argl)
   {int _la1;
    char *_rv;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp2(_la1);
    _lo = SS_mk_string(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |short *fp3(int a1)| */

static object *_SXI_fp3(SS_psides *si, object *argl)
   {int _la1;
    short *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp3(_la1);
    _sz = SC_arrlen(_rv)/sizeof(short);
    _arr = PM_make_array("short", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int *fp4(int a1)| */

static object *_SXI_fp4(SS_psides *si, object *argl)
   {int _la1;
    int *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp4(_la1);
    _sz = SC_arrlen(_rv)/sizeof(int);
    _arr = PM_make_array("int", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long *fp5(int a1)| */

static object *_SXI_fp5(SS_psides *si, object *argl)
   {int _la1;
    long *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp5(_la1);
    _sz = SC_arrlen(_rv)/sizeof(long);
    _arr = PM_make_array("long", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long long *fp6(int a1)| */

static object *_SXI_fp6(SS_psides *si, object *argl)
   {int _la1;
    long long *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp6(_la1);
    _sz = SC_arrlen(_rv)/sizeof(long long);
    _arr = PM_make_array("long long", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float *fp7(int a1)| */

static object *_SXI_fp7(SS_psides *si, object *argl)
   {int _la1;
    float *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp7(_la1);
    _sz = SC_arrlen(_rv)/sizeof(float);
    _arr = PM_make_array("float", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double *fp8(int a1)| */

static object *_SXI_fp8(SS_psides *si, object *argl)
   {int _la1;
    double *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp8(_la1);
    _sz = SC_arrlen(_rv)/sizeof(double);
    _arr = PM_make_array("double", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float _Complex *fp10(int a1)| */

static object *_SXI_fp10(SS_psides *si, object *argl)
   {int _la1;
    float _Complex *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp10(_la1);
    _sz = SC_arrlen(_rv)/sizeof(float _Complex);
    _arr = PM_make_array("float _Complex", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double _Complex *fp11(int a1)| */

static object *_SXI_fp11(SS_psides *si, object *argl)
   {int _la1;
    double _Complex *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp11(_la1);
    _sz = SC_arrlen(_rv)/sizeof(double _Complex);
    _arr = PM_make_array("double _Complex", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *fp13(int a1)| */

static object *_SXI_fp13(SS_psides *si, object *argl)
   {int _la1;
    str *_rv;
    long _sz;
    C_array *_arr;
    object *_lo;

/* local variable initializations */
    _la1       = 0;

    SS_args(si, argl,
            G_INT_I, &_la1,
            0);

    _rv = fp13(_la1);
    _sz = SC_arrlen(_rv)/sizeof(str);
    _arr = PM_make_array("str", _sz, _rv);
    _lo  = SX_make_c_array(si, _arr);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void _SX_install_bl4_consts(SS_psides *si)
   {


    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int _SX_install_bl4_derived(SS_psides *si)
   {int nerr;

    G_register_bl4_types();

    nerr = TRUE;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void SX_install_bl4_bindings(SS_psides *si)
   {

    _SX_install_bl4_consts(si);
    _SX_install_bl4_derived(si);

    SS_install(si, "fb1",
               "Procedure: fb1\n     Usage: (fb1 a1 a2)",
               SS_nargs,
               _SXI_fb1, SS_PR_PROC);

    SS_install(si, "fb2",
               "Procedure: fb2\n     Usage: (fb2 a1 a2)",
               SS_nargs,
               _SXI_fb2, SS_PR_PROC);

    SS_install(si, "fb3",
               "Procedure: fb3\n     Usage: (fb3 a1 a2)",
               SS_nargs,
               _SXI_fb3, SS_PR_PROC);

    SS_install(si, "fb4",
               "Procedure: fb4\n     Usage: (fb4 a1 a2)",
               SS_nargs,
               _SXI_fb4, SS_PR_PROC);

    SS_install(si, "fb5",
               "Procedure: fb5\n     Usage: (fb5 a1 a2)",
               SS_nargs,
               _SXI_fb5, SS_PR_PROC);

    SS_install(si, "fb6",
               "Procedure: fb6\n     Usage: (fb6 a1 a2)",
               SS_nargs,
               _SXI_fb6, SS_PR_PROC);

    SS_install(si, "fb7",
               "Procedure: fb7\n     Usage: (fb7 a1 a2)",
               SS_nargs,
               _SXI_fb7, SS_PR_PROC);

    SS_install(si, "fb8",
               "Procedure: fb8\n     Usage: (fb8 a1 a2)",
               SS_nargs,
               _SXI_fb8, SS_PR_PROC);

    SS_install(si, "fb10",
               "Procedure: fb10\n     Usage: (fb10 a1 a2)",
               SS_nargs,
               _SXI_fb10, SS_PR_PROC);

    SS_install(si, "fb11",
               "Procedure: fb11\n     Usage: (fb11 a1 a2)",
               SS_nargs,
               _SXI_fb11, SS_PR_PROC);

    SS_install(si, "fr1",
               "Procedure: fr1\n     Usage: (fr1 a1)",
               SS_nargs,
               _SXI_fr1, SS_PR_PROC);

    SS_install(si, "fr2",
               "Procedure: fr2\n     Usage: (fr2 a1)",
               SS_nargs,
               _SXI_fr2, SS_PR_PROC);

    SS_install(si, "fr3",
               "Procedure: fr3\n     Usage: (fr3 a1)",
               SS_nargs,
               _SXI_fr3, SS_PR_PROC);

    SS_install(si, "fr4",
               "Procedure: fr4\n     Usage: (fr4 a1)",
               SS_nargs,
               _SXI_fr4, SS_PR_PROC);

    SS_install(si, "fr5",
               "Procedure: fr5\n     Usage: (fr5 a1)",
               SS_nargs,
               _SXI_fr5, SS_PR_PROC);

    SS_install(si, "fr6",
               "Procedure: fr6\n     Usage: (fr6 a1)",
               SS_nargs,
               _SXI_fr6, SS_PR_PROC);

    SS_install(si, "fr7",
               "Procedure: fr7\n     Usage: (fr7 a1)",
               SS_nargs,
               _SXI_fr7, SS_PR_PROC);

    SS_install(si, "fr8",
               "Procedure: fr8\n     Usage: (fr8 a1)",
               SS_nargs,
               _SXI_fr8, SS_PR_PROC);

    SS_install(si, "fr10",
               "Procedure: fr10\n     Usage: (fr10 a1)",
               SS_nargs,
               _SXI_fr10, SS_PR_PROC);

    SS_install(si, "fr11",
               "Procedure: fr11\n     Usage: (fr11 a1)",
               SS_nargs,
               _SXI_fr11, SS_PR_PROC);

    SS_install(si, "fr13",
               "Procedure: fr13\n     Usage: (fr13 a1)",
               SS_nargs,
               _SXI_fr13, SS_PR_PROC);

    SS_install(si, "fp1",
               "Procedure: fp1\n     Usage: (fp1 a1)",
               SS_nargs,
               _SXI_fp1, SS_PR_PROC);

    SS_install(si, "fp2",
               "Procedure: fp2\n     Usage: (fp2 a1)",
               SS_nargs,
               _SXI_fp2, SS_PR_PROC);

    SS_install(si, "fp3",
               "Procedure: fp3\n     Usage: (fp3 a1)",
               SS_nargs,
               _SXI_fp3, SS_PR_PROC);

    SS_install(si, "fp4",
               "Procedure: fp4\n     Usage: (fp4 a1)",
               SS_nargs,
               _SXI_fp4, SS_PR_PROC);

    SS_install(si, "fp5",
               "Procedure: fp5\n     Usage: (fp5 a1)",
               SS_nargs,
               _SXI_fp5, SS_PR_PROC);

    SS_install(si, "fp6",
               "Procedure: fp6\n     Usage: (fp6 a1)",
               SS_nargs,
               _SXI_fp6, SS_PR_PROC);

    SS_install(si, "fp7",
               "Procedure: fp7\n     Usage: (fp7 a1)",
               SS_nargs,
               _SXI_fp7, SS_PR_PROC);

    SS_install(si, "fp8",
               "Procedure: fp8\n     Usage: (fp8 a1)",
               SS_nargs,
               _SXI_fp8, SS_PR_PROC);

    SS_install(si, "fp10",
               "Procedure: fp10\n     Usage: (fp10 a1)",
               SS_nargs,
               _SXI_fp10, SS_PR_PROC);

    SS_install(si, "fp11",
               "Procedure: fp11\n     Usage: (fp11 a1)",
               SS_nargs,
               _SXI_fp11, SS_PR_PROC);

    SS_install(si, "fp13",
               "Procedure: fp13\n     Usage: (fp13 a1)",
               SS_nargs,
               _SXI_fp13, SS_PR_PROC);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
