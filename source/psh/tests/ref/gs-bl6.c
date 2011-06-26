
#include "sx_int.h"
#include "bl6_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa1(int a ARG(2,in))| */

static object *_SXI_fa1(SS_psides *si, object *argl)
   {int _la;
    int _rv;
    object *_lo;

    _la        = 2;
    SS_args(si, argl,
            SC_INT_I, &_la,
            0);

    _rv = fa1(_la);
    _lo = SS_mk_integer(si, _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa3(int *a ARG([1,2,3],in), int *b ARG([1,2,3],io), int *c ARG([1,2,3],out))| */

static object *_SXI_fa3(SS_psides *si, object *argl)
   {int _la[3];
    int _lb[3];
    int _lc[3];
    int _rv;
    object *_lo;

    _la[0] = 1;
    _la[1] = 2;
    _la[2] = 3;
    _lb[0] = 1;
    _lb[1] = 2;
    _lb[2] = 3;
    SS_args(si, argl,
            SC_INT_I, &_la[0],
            SC_INT_I, &_la[1],
            SC_INT_I, &_la[2],
            SC_INT_I, &_lb[0],
            SC_INT_I, &_lb[1],
            SC_INT_I, &_lb[2],
            0);

    _rv = fa3(_la, _lb, _lc);
    _lo = SS_make_list(si,
			SC_INT_I, &_lb[0],
			SC_INT_I, &_lb[1],
			SC_INT_I, &_lb[2],
			SC_INT_I, &_lc[0],
			SC_INT_I, &_lc[1],
			SC_INT_I, &_lc[2],
			0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa4(int *a ARG(,in), int *b ARG(,io), int *c ARG(,out))| */

static object *_SXI_fa4(SS_psides *si, object *argl)
   {int *_la;
    int _lb;
    int *_lc;
    int _rv;
    object *_lo;

    _la        = NULL;
    _lb        = 0;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb,
            0);

    _rv = fa4(_la, &_lb, _lc);
    _lo = SS_make_list(si,
			SC_INT_I, &_lb,
			SC_INT_I, &_lc,
			0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb1(int a, int b ARG(1,in))| */

static object *_SXI_fb1(SS_psides *si, object *argl)
   {int _la;
    int _lb;
    object *_lo;

    _la        = 0;
    _lb        = 1;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb,
            0);

    fb1(_la, _lb);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb2(int a, int b ARG(,in))| */

static object *_SXI_fb2(SS_psides *si, object *argl)
   {int _la;
    int _lb;
    object *_lo;

    _la        = 0;
    _lb        = 0;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb,
            0);

    fb2(_la, _lb);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb3(int a, int b ARG(*,in))| */

static object *_SXI_fb3(SS_psides *si, object *argl)
   {int _la;
    int _lb;
    object *_lo;

    _la        = 0;
    _lb        = 0;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb,
            0);

    fb3(_la, _lb);
    _lo = SS_f;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc1(int a, int *b ARG(NULL,in), int *c ARG(NULL,io), int *d ARG(NULL,out))| */

static object *_SXI_fc1(SS_psides *si, object *argl)
   {int _la;
    int *_lb;
    int _lc;
    int *_ld;
    object *_lo;

    _la        = 0;
    _lb        = NULL;
    _lc        = 0;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb,
            SC_INT_I, &_lc,
            0);

    fc1(_la, _lb, &_lc, _ld);
    _lo = SS_make_list(si,
			SC_INT_I, &_lc,
			SC_INT_I, &_ld,
			0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc2(int a, int *b ARG(,in), int *c ARG(,io), int *d ARG(,out))| */

static object *_SXI_fc2(SS_psides *si, object *argl)
   {int _la;
    int *_lb;
    int _lc;
    int *_ld;
    object *_lo;

    _la        = 0;
    _lb        = NULL;
    _lc        = 0;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb,
            SC_INT_I, &_lc,
            0);

    fc2(_la, _lb, &_lc, _ld);
    _lo = SS_make_list(si,
			SC_INT_I, &_lc,
			SC_INT_I, &_ld,
			0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc3(int a, int *b ARG(*,in), int *c ARG(*,io), int *d ARG(*,out))| */

static object *_SXI_fc3(SS_psides *si, object *argl)
   {int _la;
    int *_lb;
    int _lc;
    int *_ld;
    object *_lo;

    _la        = 0;
    _lb        = NULL;
    _lc        = 0;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb,
            SC_INT_I, &_lc,
            0);

    fc3(_la, _lb, &_lc, _ld);
    _lo = SS_make_list(si,
			SC_INT_I, &_lc,
			SC_INT_I, &_ld,
			0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc4(int a, int *b ARG([1],in), int *c ARG([1],io), int *d ARG([1],out))| */

static object *_SXI_fc4(SS_psides *si, object *argl)
   {int _la;
    int _lb;
    int _lc;
    int *_ld;
    object *_lo;

    _la        = 0;
    _lb        = 1;
    _lc        = 0;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb,
            SC_INT_I, &_lc,
            0);

    fc4(_la, &_lb, &_lc, _ld);
    _lo = SS_make_list(si,
			SC_INT_I, &_lc,
			SC_INT_I, &_ld,
			0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc5(int a, int *b ARG([1,2],in), int *c ARG([1,2],io), int *d ARG([1,2],out))| */

static object *_SXI_fc5(SS_psides *si, object *argl)
   {int _la;
    int _lb[2];
    int _lc[2];
    int _ld[2];
    object *_lo;

    _la        = 0;
    _lb[0] = 1;
    _lb[1] = 2;
    _lc[0] = 1;
    _lc[1] = 2;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb[0],
            SC_INT_I, &_lb[1],
            SC_INT_I, &_lc[0],
            SC_INT_I, &_lc[1],
            0);

    fc5(_la, _lb, _lc, _ld);
    _lo = SS_make_list(si,
			SC_INT_I, &_lc[0],
			SC_INT_I, &_lc[1],
			SC_INT_I, &_ld[0],
			SC_INT_I, &_ld[1],
			0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc6(int a, int *b ARG([*],in), int *c ARG([*],io), int *d ARG([*],out))| */

static object *_SXI_fc6(SS_psides *si, object *argl)
   {int _la;
    int _lb;
    int _lc;
    int *_ld;
    object *_lo;

    _la        = 0;
    _lb        = 0;
    _lc        = 0;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb,
            SC_INT_I, &_lc,
            0);

    fc6(_la, &_lb, &_lc, _ld);
    _lo = SS_make_list(si,
			SC_INT_I, &_lc,
			SC_INT_I, &_ld,
			0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc7(int a, int *b ARG([*,*],in), int *c ARG([*,*],io), int *d ARG([*,*],out))| */

static object *_SXI_fc7(SS_psides *si, object *argl)
   {int _la;
    int _lb[2];
    int _lc[2];
    int _ld[2];
    object *_lo;

    _la        = 0;
    _lb[0] = 0;
    _lb[1] = 0;
    _lc[0] = 0;
    _lc[1] = 0;
    SS_args(si, argl,
            SC_INT_I, &_la,
            SC_INT_I, &_lb[0],
            SC_INT_I, &_lb[1],
            SC_INT_I, &_lc[0],
            SC_INT_I, &_lc[1],
            0);

    fc7(_la, _lb, _lc, _ld);
    _lo = SS_make_list(si,
			SC_INT_I, &_lc[0],
			SC_INT_I, &_lc[1],
			SC_INT_I, &_ld[0],
			SC_INT_I, &_ld[1],
			0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void SX_install_bl6_bindings(SS_psides *si)
   {

    SS_install(si, "fa1s",
               "Procedure: fa1s\n     Usage: (fa1s a)",
               SS_nargs,
               _SXI_fa1, SS_PR_PROC);

    SS_install(si, "fa3s",
               "Procedure: fa3s\n     Usage: (fa3s a b c)",
               SS_nargs,
               _SXI_fa3, SS_PR_PROC);

    SS_install(si, "fa4s",
               "Procedure: fa4s\n     Usage: (fa4s a b c)",
               SS_nargs,
               _SXI_fa4, SS_PR_PROC);

    SS_install(si, "fb1",
               "Procedure: fb1\n     Usage: (fb1 a b)",
               SS_nargs,
               _SXI_fb1, SS_PR_PROC);

    SS_install(si, "fb2",
               "Procedure: fb2\n     Usage: (fb2 a b)",
               SS_nargs,
               _SXI_fb2, SS_PR_PROC);

    SS_install(si, "fb3",
               "Procedure: fb3\n     Usage: (fb3 a b)",
               SS_nargs,
               _SXI_fb3, SS_PR_PROC);

    SS_install(si, "fc1",
               "Procedure: fc1\n     Usage: (fc1 a b c d)",
               SS_nargs,
               _SXI_fc1, SS_PR_PROC);

    SS_install(si, "fc2",
               "Procedure: fc2\n     Usage: (fc2 a b c d)",
               SS_nargs,
               _SXI_fc2, SS_PR_PROC);

    SS_install(si, "fc3",
               "Procedure: fc3\n     Usage: (fc3 a b c d)",
               SS_nargs,
               _SXI_fc3, SS_PR_PROC);

    SS_install(si, "fc4",
               "Procedure: fc4\n     Usage: (fc4 a b c d)",
               SS_nargs,
               _SXI_fc4, SS_PR_PROC);

    SS_install(si, "fc5",
               "Procedure: fc5\n     Usage: (fc5 a b c d)",
               SS_nargs,
               _SXI_fc5, SS_PR_PROC);

    SS_install(si, "fc6",
               "Procedure: fc6\n     Usage: (fc6 a b c d)",
               SS_nargs,
               _SXI_fc6, SS_PR_PROC);

    SS_install(si, "fc7",
               "Procedure: fc7\n     Usage: (fc7 a b c d)",
               SS_nargs,
               _SXI_fc7, SS_PR_PROC);

   return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
