/*
 * SHPRM2.C - arithmetic functions for Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

typedef double          (*PFDoubled)(double);
typedef double          (*PFDoubledd)(double, double);
typedef double _Complex (*PFComplexc)(double _Complex);
typedef double _Complex (*PFComplexcc)(double _Complex, double _Complex);
typedef quaternion      (*PFQuaternionq)(quaternion);
typedef quaternion      (*PFQuaternionqq)(quaternion, quaternion);
typedef int             (*PFIntd)(double);
typedef int             (*PFIntdd)(double, double);
typedef BIGINT          (*PFBIGINTB)(BIGINT);
typedef BIGINT          (*PFBIGINTBB)(BIGINT, BIGINT);
typedef object         *(*PFBINOBJ)(object *argl);

#define SS_GET_OPERAND(_oper, _arg, _typ)                                    \
    {int _ityp;                                                              \
     object *_num;                                                           \
     _oper = 0;                                                              \
     _num  = SS_car(_arg);                                                   \
     _arg  = SS_cdr(_arg);                                                   \
     _ityp = SC_arrtype(_num, -1);                                           \
     if (_ityp == SC_INT_I)                                                  \
        _oper = SS_INTEGER_VALUE(_num);                                      \
     else if (_ityp == SC_FLOAT_I)                                           \
        {_oper = SS_FLOAT_VALUE(_num);                                       \
         _typ  = SC_FLOAT_I;}                                                \
     else                                                                    \
        SS_error("ARGUMENT MUST BE A NUMBER - SS_GET_OPERAND", _num);}

int
 SS_strict_c = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_INSTALL_MF - install math functions
 *               - NOTE: this is not a vararg function, we are using
 *               - varargs to defeat type checking on the function
 *               - pointers which is inappropriate for this general
 *               - purpose mechanism
 *               - Example: SS_install_mf("exp", "..", SS_unary_flt,
 *               -                        exp, PM_cexp, PM_qexp, SS_PR_PROC)
 */

void SS_install_mf(char* pname, char *pdoc, PFPHand phand, ...)
   {int i, n;
    SS_form ptype;
    PFVoid *pr;

    n  = 3;
    pr = FMAKE_N(PFVoid, n, "SS_INSTALL_MF:pr");

    SC_VA_START(phand);

    for (i = 0; i < n; i++)
        pr[i] = SC_VA_ARG(PFVoid);

    ptype = SC_VA_ARG(SS_form);

    SC_VA_END;

    _SS_install(pname, pdoc, phand, n, pr, ptype);

    return;}

/*--------------------------------------------------------------------------*/

/*                          MATHEMATICS HANDLERS                            */

/*--------------------------------------------------------------------------*/

/* SS_UNARY_FLT - the unary operator handler returning floats */

object *SS_unary_flt(C_procedure *cp, object *argl)
   {int id;
    object *x, *rv;

    if (SS_nullobjp(argl))
       SS_error("WRONG NUMBER OF ARGUMENTS - SS_UNARY_FLT", argl);

    x  = SS_car(argl);
    id = SC_arrtype(x, -1);

    if (id == SC_INT_I)
       {double y;
	PFDoubled f;
	f  = (PFDoubled) cp->proc[0];
	y  = f(SS_INTEGER_VALUE(x));
	rv = SS_mk_float(y);}

    else if (id == SC_FLOAT_I)
       {double y;
	PFDoubled f;
	f  = (PFDoubled) cp->proc[0];
	y  = f(SS_FLOAT_VALUE(x));
	rv = SS_mk_float(y);}

    else if (id == SC_DOUBLE_COMPLEX_I)
       {double _Complex y;
	PFComplexc f;
	f  = (PFComplexc) cp->proc[1];
	y  = f(SS_COMPLEX_VALUE(x));
	rv = SS_mk_complex(y);}

    else if (id == SC_QUATERNION_I)
       {quaternion y;
	PFQuaternionq f;
	f  = (PFQuaternionq) cp->proc[2];
	y  = f(SS_QUATERNION_VALUE(x));
	rv = SS_mk_quaternion(y);}

    else
       SS_error("ARGUMENT MUST BE A NUMBER - SS_GET_OPERAND", x);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_UNARY_FIX - the unary operator handler returning fixed point numbers */

object *SS_unary_fix(C_procedure *cp, object *argl)
   {int type;
    BIGINT iv;
    double operand;
    PFDoubled fnc;
    object *rv;

    operand = 0.0;

    if (SS_nullobjp(argl))
       SS_error("WRONG NUMBER OF ARGUMENTS - SS_UNARY_FIX", argl);

    fnc = (PFDoubled) cp->proc[0];
    SS_GET_OPERAND(operand, argl, type);

    iv = (*fnc)(operand);
    rv = SS_mk_integer(iv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_UNARY_BIT - the unary operator handler for bit operations */

static object *SS_unary_bit(C_procedure *cp, object *argl)
   {int type;
    BIGINT iv, operand;
    PFBIGINTB fnc;
    object *rv;

    operand = 0.0;

    if (SS_nullobjp(argl))
       SS_error("WRONG NUMBER OF ARGUMENTS - SS_UNARY_BIT", argl);

    fnc = (PFBIGINTB) cp->proc[0];
    SS_GET_OPERAND(operand, argl, type);

    iv = (*fnc)(operand);
    rv = SS_mk_integer(iv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BINARY_OPR - the binary arithmetic operator handler */

static object *SS_binary_opr(C_procedure *cp, object *argl)
   {PFBINOBJ fnc;
    object *rv;

    if (SS_length(argl) != 2)
       SS_error("WRONG NUMBER OF ARGUMENTS - SS_BINARY_OPR", argl);

    fnc = (PFBINOBJ) cp->proc[0];

    rv = (*fnc)(argl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BINARY_FIX - the binary fixed point arithmetic operator handler */

object *SS_binary_fix(C_procedure *cp, object *argl)
   {BIGINT i1, i2, iv;
    PFBIGINTBB fnc;
    object *x1, *x2, *rv;

    if (SS_length(argl) != 2)
       SS_error("WRONG NUMBER OF ARGUMENTS - SS_BINARY_FIX", argl);

    fnc = (PFBIGINTBB) cp->proc[0];
    x1  = SS_car(argl);
    x2  = SS_cadr(argl);

    if (!SS_integerp(x1) || !SS_integerp(x2))
       SS_error("ARGUMENTS NOT BOTH INTEGERS - SS_BINARY_FIX",
                SS_mk_cons(x1, x2));

    i1 = SS_INTEGER_VALUE(x1);
    i2 = SS_INTEGER_VALUE(x2);

    iv = (*fnc)(i1, i2);
    rv = SS_mk_integer(iv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BIN_FIX - apply binary operator OP to list
 *             - with fixed point maximal type 
 */

static object *_SS_bin_fix(long ni, object *argl, PFDoubledd op)
   {int idf, ident;
    long i, off;
    double accv, *v;
    object *acc;

    idf = ((op == PM_fdivide) || (op == HYPOT)) ? SC_FLOAT_I : SC_INT_I;

    v = FMAKE_N(double, ni, "_SS_BIN_FIX:double");
    _SS_list_to_numtype_id(SC_DOUBLE_I, v, ni, argl);

    if (ni < 2)
       {ident = ((op != PM_fplus) && (op != PM_fminus));
	accv  = ident;
        off   = 0;}
    else
       {accv = v[0];
        off  = 1;};

    for (i = off; i < ni; i++)
        accv = op(accv, v[i]);

    if ((idf < SC_FLOAT_I) && (accv < LONG_MAX))
       acc = SS_mk_integer((long long) accv);
    else
       acc = SS_mk_float(accv);

    SFREE(v);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BIN_FLOAT - apply binary operator OP to list
 *               - with floating point maximal type 
 */

static object *_SS_bin_float(long ni, object *argl, PFDoubledd op)
   {int ident;
    long i, off;
    double accv, *v;
    object *acc;

    v = FMAKE_N(double, ni, "_SS_BIN_FLOAT:double");
    _SS_list_to_numtype_id(SC_DOUBLE_I, v, ni, argl);

    if (ni < 2)
       {ident = ((op != PM_fplus) && (op != PM_fminus));
	accv  = ident;
	off   = 0;}
    else
       {accv = v[0];
	off  = 1;};

    for (i = off; i < ni; i++)
        accv = op(accv, v[i]);

    acc = SS_mk_float(accv);

    SFREE(v);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BIN_COMPLEX - apply binary operator OP to list
 *                 - with complex floating point maximal type 
 */

static object *_SS_bin_complex(long ni, object *argl, PFComplexcc op)
   {int ident;
    long i, off;
    double _Complex accv, *v;
    object *acc;

    v = FMAKE_N(double _Complex, ni, "SS_BINARY_FLT:complex");
    _SS_list_to_numtype_id(SC_DOUBLE_COMPLEX_I, v, ni, argl);

    if (ni < 2)
       {ident = ((op != PM_plus_cc) && (op != PM_minus_cc));
	accv  = ident;
	off   = 0;}
    else
       {accv = v[0];
        off  = 1;};

    for (i = off; i < ni; i++)
        accv = op(accv, v[i]);

    acc = SS_mk_complex(accv);

    SFREE(v);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BIN_QUATERNION - apply binary operator OP to list
 *                    - with quaternion floating point maximal type 
 */

static object *_SS_bin_quaternion(long ni, object *argl, PFQuaternionqq op)
   {int ident;
    long i, off;
    quaternion accv, *v;
    object *acc;

    v = FMAKE_N(quaternion, ni, "SS_BINARY_FLT:quaternion");
    _SS_list_to_numtype_id(SC_QUATERNION_I, v, ni, argl);

    accv.i = 0;
    accv.j = 0;
    accv.k = 0;

    if (ni < 2)
       {ident  = ((op != PM_plus_qq) && (op != PM_minus_qq));
	accv.s = ident;
        off    = 0;}
    else
       {accv = v[0];
        off  = 1;};

    for (i = off; i < ni; i++)
        accv = op(accv, v[i]);

    acc = SS_mk_quaternion(accv);

    SFREE(v);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BINARY_FLT - the binary floating point arithmetic operator handler */

object *SS_binary_flt(C_procedure *cp, object *argl)
   {int ido;
    long ni;
    object *acc;

    ido = _SS_max_numeric_type(argl, &ni);
    if (ido == -1)
       SS_error("NON-NUMERIC OPERAND - SS_BINARY_FLT", argl);

/* fixed point operands */
    else if (ido < SC_FLOAT_I)
       acc = _SS_bin_fix(ni, argl, (PFDoubledd) cp->proc[0]);

/* floating point operands */
    else if (ido < SC_FLOAT_COMPLEX_I)
       acc = _SS_bin_float(ni, argl, (PFDoubledd) cp->proc[0]);

/* complex floating point operands */
    else if (ido < SC_QUATERNION_I)
       acc = _SS_bin_complex(ni, argl, (PFComplexcc) cp->proc[1]);

/* quaternion operands */
    else
       acc = _SS_bin_quaternion(ni, argl, (PFQuaternionqq) cp->proc[2]);

    return(acc);}

/*--------------------------------------------------------------------------*/

/*                      HANDLER AUXILLIARY FUNCTIONS                        */

/*--------------------------------------------------------------------------*/

/*                        UNARAY OPERATIONS                                 */

/*--------------------------------------------------------------------------*/

/*                         PREDICATE HANDLERS                               */

/*--------------------------------------------------------------------------*/

/* SS_UN_COMP - the unary comparison handler */

object *SS_un_comp(C_procedure *cp, object *argl)
   {int type, lv;
    double operand;
    PFIntd fnc;
    object *rv;

    operand = 0.0;

    if (SS_nullobjp(argl))
       SS_error("WRONG NUMBER OF ARGUMENTS - SS_UN_COMP", argl);

    fnc = (PFIntd) cp->proc[0];
    SS_GET_OPERAND(operand, argl, type);

    lv = (*fnc)(operand);
    rv = lv ? SS_t : SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BIN_COMP - the binary comparison handler */

object *SS_bin_comp(C_procedure *cp, object *argl)
   {int type, lv;
    double c1, c2;
    PFIntdd fnc;
    object *rv;

    c1 = 0.0;
    c2 = 0.0;

    if (SS_length(argl) != 2)
       SS_error("WRONG NUMBER OF ARGUMENTS - SS_BIN_COMP", argl);

    fnc = (PFIntdd) cp->proc[0];
    SS_GET_OPERAND(c1, argl, type);
    SS_GET_OPERAND(c2, argl, type);

    lv = (*fnc)(c1, c2);
    rv = lv ? SS_t : SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_ZERO - C level 0 test */

static int _SS_zero(double x1)
   {int rv;

    rv = (x1 == 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_NEG - C level negative test */

static int _SS_neg(double x1)
   {int rv;

    rv = (x1 < 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_POS - C level positive test */

static int _SS_pos(double x1)
   {int rv;

    rv = (x1 > 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_EVEN - even predicate in C */

static int _SS_even(double f)
   {int rv;
    long g;

    g = (((long) PM_fix(f)) >> 1) << 1;

    rv = (f == (double) g);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_ODD - odd predicate in C */

static int _SS_odd(double f)
   {int rv;
    long g;

    g = (((long) PM_fix(f)) >> 1) << 1;

    rv = (f != (double) g);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_MACHINE_PREC - function version of SC_machine_prec */

static object *_SS_machine_prec(void)
   {object *rv;

    rv = SS_mk_float(PM_machine_precision());

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_XOR_POW - return exclusive or iff in strict C conformance mode
 *             - otherwise return the power of a^b
 *             - NOTE: deviate from strict C and allow '^' to be exponentiation
 */

static object *_SS_xor_pow(object *argl)
   {int type;
    BIGINT i1, i2, iv;
    double d1, d2, dv;
    object *rv;

    rv = SS_null;

    if (SS_strict_c == TRUE)
       {SS_GET_OPERAND(i1, argl, type);
	SS_GET_OPERAND(i2, argl, type);
	iv = PM_lxor(i1, i2);
	rv = SS_mk_integer(iv);}

    else
       {SS_GET_OPERAND(d1, argl, type);
	SS_GET_OPERAND(d2, argl, type);
	dv = pow(d1, d2);
	rv = SS_mk_float(dv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INSTALL_MATH - install the SCHEME primitives for math */

void _SS_install_math(void)
   {

    SS_install("&",
               "Procedure: Returns bitwise and of args (left associative)",
               SS_binary_fix,
               PM_land, SS_PR_PROC);

    SS_install("|",
               "Procedure: Returns bitwise or of args (left associative)",
               SS_binary_fix,
               PM_lor, SS_PR_PROC);

    SS_install("~",
               "Procedure: Returns bitwise complement of its arg",
               SS_unary_bit, 
               PM_lcmp, SS_PR_PROC);

    SS_install("<<",
               "Procedure: Returns left shift of args (left associative)",
               SS_binary_fix,
               PM_llshft, SS_PR_PROC);

    SS_install(">>",
               "Procedure: Returns right shift of args (left associative)",
               SS_binary_fix,
               PM_lrshft, SS_PR_PROC);

    SS_install("<",
               "Procedure: Returns #t iff the first argument is less than the second",
               SS_bin_comp,
               PM_flt, SS_PR_PROC);

    SS_install("<=",
               "Procedure: Returns #t iff the first argument is less than or equal to the second",
               SS_bin_comp,
               PM_fle, SS_PR_PROC);

    SS_install("=",
               "Procedure: Returns #t iff the first argument is equal to the second",
               SS_bin_comp,
               PM_feq, SS_PR_PROC);

    SS_install("!=",
               "Procedure: Returns #t iff the first argument is not equal to the second",
               SS_bin_comp,
               PM_fneq, SS_PR_PROC);

    SS_install(">",
               "Procedure: Returns #t iff the first argument is greater than the second",
               SS_bin_comp,
               PM_fgt, SS_PR_PROC);

    SS_install(">=",
               "Procedure: Returns #t iff the first argument is greater than or equal to the second",
               SS_bin_comp,
               PM_fge, SS_PR_PROC);

    SS_install("acos",
               "Procedure: Returns the arc cosine of the argument",
               SS_unary_flt, 
               acos, SS_PR_PROC);

    SS_install("asin",
               "Procedure: Returns the arc sine of the argument",
               SS_unary_flt, 
               asin, SS_PR_PROC);

    SS_install("atan",
               "Procedure: Returns the arc tangent of the argument",
               SS_unary_flt, 
               atan, SS_PR_PROC);

    SS_install("ceiling",
               "Procedure: Returns the smallest integer greater than the argument",
               SS_unary_fix, 
               ceil, SS_PR_PROC);

    SS_install("cos",
               "Procedure: Returns the cosine of the argument",
               SS_unary_flt, 
               cos, SS_PR_PROC);

    SS_install("cosh",
               "Procedure: Returns the hyperbolic cosine of the argument",
               SS_unary_flt, 
               cosh, SS_PR_PROC);

    SS_install("i0",
	       "Procedure: Take zeroth order modified Bessel function of the first kind",
               SS_unary_flt, 
               PM_i0, SS_PR_PROC);

    SS_install("i1",
	       "Procedure: Take first order modified Bessel function of the first kind",
               SS_unary_flt, 
               PM_i1, SS_PR_PROC);

    SS_install("in",
	       "Procedure: Take nth order modified Bessel function of the first kind",
               SS_binary_flt, 
               PM_in, SS_PR_PROC);

    SS_install("j0",
	       "Procedure: Take zeroth order Bessel function of the first kind",
               SS_unary_flt, 
               PM_j0, SS_PR_PROC);

    SS_install("j1",
	       "Procedure: Take first order Bessel function of the first kind",
               SS_unary_flt, 
               PM_j1, SS_PR_PROC);

    SS_install("jn",
	       "Procedure: Take nth order Bessel function of the first kind",
               SS_binary_flt, 
               PM_jn, SS_PR_PROC);

    SS_install("k0",
	       "Procedure: Take zeroth order modified Bessel function of the first kind",
               SS_unary_flt, 
               PM_k0, SS_PR_PROC);

    SS_install("k1",
	       "Procedure: Take first order modified Bessel function of the first kind",
               SS_unary_flt, 
               PM_k1, SS_PR_PROC);

    SS_install("kn",
	       "Procedure: Take nth order modified Bessel function of the first kind",
               SS_binary_flt, 
               PM_kn, SS_PR_PROC);

    SS_install("y0",
	       "Procedure: Take zeroth order Bessel function of the second kind",
               SS_unary_flt, 
               PM_y0, SS_PR_PROC);

    SS_install("y1",
	       "Procedure: Take first order Bessel function of the second kind",
               SS_unary_flt, 
               PM_y1, SS_PR_PROC);

    SS_install("yn",
	       "Procedure: Take nth order Bessel function of the second kind",
               SS_binary_flt, 
               PM_yn, SS_PR_PROC);

    SS_install("even?",
               "Procedure: Returns #t iff the argument is a number divisible exactly by 2",
               SS_un_comp,
               _SS_even, SS_PR_PROC);

    SS_install("floor",
               "Procedure: Returns the greatest integer less than the argument",
               SS_unary_fix, 
               floor, SS_PR_PROC);

    SS_install("hypot",
               "Procedure: Returns the sqrt of the sum of the squares of the arguments",
               SS_binary_flt,
               HYPOT, SS_PR_PROC);

    SS_install("log",
               "Procedure: Returns the logarithm base 10 of the argument",
               SS_unary_flt, 
               PM_log, SS_PR_PROC);

    SS_install("machine-precision",
               "Procedure: Returns a floating point machine precision value",
               SS_zargs,
               _SS_machine_prec, SS_PR_PROC);

    SS_install("max",
               "Procedure: Returns the greater of args",
               SS_binary_flt,
               PM_fmax, SS_PR_PROC);

    SS_install("min",
               "Procedure: Returns the smaller of args",
               SS_binary_flt,
               PM_fmin, SS_PR_PROC);

    SS_install("negative?",
               "Procedure: Returns #t iff the argument is a number less than 0",
               SS_un_comp,
               _SS_neg, SS_PR_PROC);

    SS_install("odd?",
               "Procedure: Returns #t iff the argument is a number that is not even",
               SS_un_comp,
               _SS_odd, SS_PR_PROC);

    SS_install("positive?",
               "Procedure: Returns #t iff the argument is a number greater than 0",
               SS_un_comp,
               _SS_pos, SS_PR_PROC);

    SS_install("quotient",
               "Procedure: Returns quotient of two integers",
               SS_binary_fix,
               PM_ldivide, SS_PR_PROC);

    SS_install("remainder",
               "Procedure: Returns remainder of division of the two arguments",
               SS_binary_fix,
               PM_lmodulo, SS_PR_PROC);

    SS_install("sin",
               "Procedure: Returns the sine of the argument",
               SS_unary_flt, 
               sin, SS_PR_PROC);

    SS_install("sinh",
               "Procedure: Returns the hyperbolic sine of the argument",
               SS_unary_flt, 
               sinh, SS_PR_PROC);

    SS_install("sqrt",
               "Procedure: Returns the principal square root of the argument",
               SS_unary_flt, 
               sqrt, SS_PR_PROC);

    SS_install("tan",
               "Procedure: Returns the tangent of the argument",
               SS_unary_flt, 
               tan, SS_PR_PROC);

    SS_install("tanh",
               "Procedure: Returns the hyperbolic tangent of the argument",
               SS_unary_flt, 
               tanh, SS_PR_PROC);

    SS_install("truncate",
               "Procedure: Returns the integer resulting from the truncation of the argument",
               SS_unary_fix, 
               PM_fix, SS_PR_PROC);

    SS_install("zero?",
               "Procedure: Returns #t iff the argument is a number equal to 0",
               SS_un_comp,
               _SS_zero, SS_PR_PROC);

    SS_install("random",
               "Procedure: Returns a random number between -1.0 and 1.0 using <seed>\nUsage: (random <seed>)",
               SS_unary_flt, 
               PM_random_s, SS_PR_PROC);

/* special functions */

    SS_install("beta",
               "Procedure: Returns the beta function\nUsage: (beta z w)",
               SS_binary_flt, 
               PM_beta, SS_PR_PROC);

    SS_install("lngamma",
               "Procedure: Returns the log of the gamma function\nUsage: (lngamma x)",
               SS_unary_flt, 
               PM_ln_gamma, SS_PR_PROC);

    SS_install("igamma-p",
               "Procedure: Returns the incomplete gamma function P\nUsage: (igamma-p a x)",
               SS_binary_flt, 
               PM_igamma_p, SS_PR_PROC);

    SS_install("igamma-q",
               "Procedure: Returns the incomplete gamma function Q\nUsage: (igamma-q a x)",
               SS_binary_flt, 
               PM_igamma_q, SS_PR_PROC);

    SS_install("erf",
               "Procedure: Returns the error function ERF\nUsage: (erf x)",
               SS_unary_flt, 
               PM_erf, SS_PR_PROC);

    SS_install("erfc",
               "Procedure: Returns the complementary error function ERFC\nUsage: (erfc x)",
               SS_unary_flt, 
               PM_erfc, SS_PR_PROC);

    SS_install("lei1",
               "Procedure: Returns the Legendre elliptic integral of the first kind\nUsage: (lei1 x k)",
               SS_binary_flt, 
               PM_elliptic_integral_l1, SS_PR_PROC);

    SS_install("lei2",
               "Procedure: Returns the Legendre elliptic integral of the second kind\nUsage: (lei2 x k)",
               SS_binary_flt, 
               PM_elliptic_integral_l2, SS_PR_PROC);

    SS_install("cei1",
               "Procedure: Returns the complete elliptic integral of the first kind\nUsage: (cei1 k)",
               SS_unary_flt, 
               PM_elliptic_integral_c1, SS_PR_PROC);

    SS_install("cei2",
               "Procedure: Returns the complete elliptic integral of the second kind\nUsage: (cei2 k)",
               SS_unary_flt, 
               PM_elliptic_integral_c2, SS_PR_PROC);

    SS_install("cei3",
               "Procedure: Returns the complete elliptic integral of the third kind\nUsage: (cei3 n k)",
               SS_binary_flt, 
               PM_elliptic_integral_c3, SS_PR_PROC);

    SS_install("sn",
               "Procedure: Returns the Jacobian elliptic function sn\nUsage: (sn x k)",
               SS_binary_flt, 
               PM_sn, SS_PR_PROC);

    SS_install("cn",
               "Procedure: Returns the Jacobian elliptic function cn\nUsage: (cn x k)",
               SS_binary_flt, 
               PM_cn, SS_PR_PROC);

    SS_install("dn",
               "Procedure: Returns the Jacobian elliptic function dn\nUsage: (dn x k)",
               SS_binary_flt, 
               PM_dn, SS_PR_PROC);

/* multi type functions */
    SS_install_mf("*",
		  "Procedure: Returns product of args or 1 if no args are supplied",
		  SS_binary_flt,
		  PM_ftimes, PM_times_cc, PM_times_qq, SS_PR_PROC);

    SS_install_mf("+",
		  "Procedure: Returns sum of args or 0 if no args are supplied",
		  SS_binary_flt,
		  PM_fplus, PM_plus_cc, PM_plus_qq, SS_PR_PROC);

    SS_install_mf("-",
		  "Procedure: Returns difference of args",
		  SS_binary_flt,
		  PM_fminus, PM_minus_cc, PM_minus_qq, SS_PR_PROC);

    SS_install_mf("/",
		  "Procedure: Returns quotient of args (left associative)",
		  SS_binary_flt,
		  PM_fdivide, PM_divide_cc, PM_rquotient_qq, SS_PR_PROC);

    SS_install_mf("abs",
		  "Procedure: Returns the absolute value of a numeric object",
		  SS_unary_flt,
		  ABS, PM_cabs, PM_qnorm, SS_PR_PROC);

    SS_install_mf("exp",
		  "Procedure: Returns the exponential of the argument",
		  SS_unary_flt, 
		  exp, PM_cexp, PM_qexp, SS_PR_PROC);

    SS_install_mf("ln",
		  "Procedure: Returns the natural logarithm of the argument",
		  SS_unary_flt, 
		  PM_ln, PM_cln, PM_qln, SS_PR_PROC);


/* special syntax context */

    SS_install("expt",
               "Procedure: Returns the first argument raised to the power of the second",
               SS_binary_flt,
               POW, SS_PR_PROC);

    SS_install("xor",
               "Procedure: Returns bitwise exclusive or of args (left associative)",
               SS_binary_fix,
               PM_lxor, SS_PR_PROC);

    SS_install("^",
               "Procedure: Returns xor if strict C mode otherwise expt",
               SS_binary_opr,
               _SS_xor_pow, SS_PR_PROC);

    SS_install_cf("strict-c", 
		  "Variable: Strict C compliance for operators if 1\n     Usage: strict-c [0|1]",
		  SS_acc_int,
		  &SS_strict_c);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
