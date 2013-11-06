
#include "cpyright.h"
#include "sx_int.h"
#include "bl5_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f1(PFDouble a)| */

static object *_SXI_a_f1(SS_psides *si, object *argl)
   {PFDouble _la;
    object *_lo;

    _la        = NULL;
    SS_args(si, argl,
            SC_POINTER_I, &_la,
            0);

    a_f1(_la);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f2(int (*b)(int *a))| */

static object *_SXI_a_f2(SS_psides *si, object *argl)
   {int (*_lb)(int *a);
    object *_lo;

    _lb        = NULL;
    SS_args(si, argl,
            SC_POINTER_I, &_lb,
            0);

    a_f2(_lb);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f3(PFDouble a, int (*b)(int *a))| */

static object *_SXI_a_f3(SS_psides *si, object *argl)
   {PFDouble _la;
    int (*_lb)(int *a);
    object *_lo;

    _la        = NULL;
    _lb        = NULL;
    SS_args(si, argl,
            SC_POINTER_I, &_la,
            SC_POINTER_I, &_lb,
            0);

    a_f3(_la, _lb);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f4(int (*b)(int *a), PFDouble a)| */

static object *_SXI_a_f4(SS_psides *si, object *argl)
   {int (*_lb)(int *a);
    PFDouble _la;
    object *_lo;

    _lb        = NULL;
    _la        = NULL;
    SS_args(si, argl,
            SC_POINTER_I, &_lb,
            SC_POINTER_I, &_la,
            0);

    a_f4(_lb, _la);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f1(PFDouble a, int b)| */

static object *_SXI_b_f1(SS_psides *si, object *argl)
   {PFDouble _la;
    int _lb;
    object *_lo;

    _la        = NULL;
    _lb        = 0;
    SS_args(si, argl,
            SC_POINTER_I, &_la,
            SC_INT_I, &_lb,
            0);

    B_f1(_la, _lb);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f2(int (*a)(int *a), int b)| */

static object *_SXI_b_f2(SS_psides *si, object *argl)
   {int (*_la)(int *a);
    int _lb;
    object *_lo;

    _la        = NULL;
    _lb        = 0;
    SS_args(si, argl,
            SC_POINTER_I, &_la,
            SC_INT_I, &_lb,
            0);

    B_f2(_la, _lb);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f3(PFDouble a, int (*b)(int *a), int c)| */

static object *_SXI_b_f3(SS_psides *si, object *argl)
   {PFDouble _la;
    int (*_lb)(int *a);
    int _lc;
    object *_lo;

    _la        = NULL;
    _lb        = NULL;
    _lc        = 0;
    SS_args(si, argl,
            SC_POINTER_I, &_la,
            SC_POINTER_I, &_lb,
            SC_INT_I, &_lc,
            0);

    B_f3(_la, _lb, _lc);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f4(int (*a)(int *a), PFDouble b, int c)| */

static object *_SXI_b_f4(SS_psides *si, object *argl)
   {int (*_la)(int *a);
    PFDouble _lb;
    int _lc;
    object *_lo;

    _la        = NULL;
    _lb        = NULL;
    _lc        = 0;
    SS_args(si, argl,
            SC_POINTER_I, &_la,
            SC_POINTER_I, &_lb,
            SC_INT_I, &_lc,
            0);

    B_f4(_la, _lb, _lc);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f5(int a, PFDouble b, int (*c)(int *a))| */

static object *_SXI_b_f5(SS_psides *si, object *argl)
   {int _la;
    PFDouble _lb;
    int (*_lc)(int *a);
    object *_lo;

    _la        = 0;
    _lb        = NULL;
    _lc        = NULL;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_POINTER_I, &_lb,
            SC_POINTER_I, &_lc,
            0);

    B_f5(_la, _lb, _lc);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f6(int a, int (*b)(int *a), PFDouble c)| */

static object *_SXI_b_f6(SS_psides *si, object *argl)
   {int _la;
    int (*_lb)(int *a);
    PFDouble _lc;
    object *_lo;

    _la        = 0;
    _lb        = NULL;
    _lc        = NULL;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_POINTER_I, &_lb,
            SC_POINTER_I, &_lc,
            0);

    B_f6(_la, _lb, _lc);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void SX_install_bl5_bindings(SS_psides *si)
   {

    SS_install(si, "a-f1",
               "Procedure: a-f1\n     Usage: (a-f1 a)",
               SS_nargs,
               _SXI_a_f1, SS_PR_PROC);

    SS_install(si, "a-f2",
               "Procedure: a-f2\n     Usage: (a-f2 b)",
               SS_nargs,
               _SXI_a_f2, SS_PR_PROC);

    SS_install(si, "a-f3",
               "Procedure: a-f3\n     Usage: (a-f3 a b)",
               SS_nargs,
               _SXI_a_f3, SS_PR_PROC);

    SS_install(si, "a-f4",
               "Procedure: a-f4\n     Usage: (a-f4 b a)",
               SS_nargs,
               _SXI_a_f4, SS_PR_PROC);

    SS_install(si, "b-f1",
               "Procedure: b-f1\n     Usage: (b-f1 a b)",
               SS_nargs,
               _SXI_b_f1, SS_PR_PROC);

    SS_install(si, "b-f2",
               "Procedure: b-f2\n     Usage: (b-f2 a b)",
               SS_nargs,
               _SXI_b_f2, SS_PR_PROC);

    SS_install(si, "b-f3",
               "Procedure: b-f3\n     Usage: (b-f3 a b c)",
               SS_nargs,
               _SXI_b_f3, SS_PR_PROC);

    SS_install(si, "b-f4",
               "Procedure: b-f4\n     Usage: (b-f4 a b c)",
               SS_nargs,
               _SXI_b_f4, SS_PR_PROC);

    SS_install(si, "b-f5",
               "Procedure: b-f5\n     Usage: (b-f5 a b c)",
               SS_nargs,
               _SXI_b_f5, SS_PR_PROC);

    SS_install(si, "b-f6",
               "Procedure: b-f6\n     Usage: (b-f6 a b c)",
               SS_nargs,
               _SXI_b_f6, SS_PR_PROC);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
