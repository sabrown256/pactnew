/*
 * SHPRM2.C - arithmetic functions for Scheme
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scheme_int.h"

typedef object *(*PFBINOBJ)(SS_psides *si, object *argl);

#define SS_GET_OPERAND(_si, _oper, _arg, _typ)                               \
    {int _ityp;                                                              \
     object *_num;                                                           \
     _oper = 0;                                                              \
     _num  = SS_car(si, _arg);                                               \
     _arg  = SS_cdr(si, _arg);                                               \
     _ityp = SC_arrtype(_num, -1);                                           \
     _typ  = G_INT_I;                                                       \
     if (_ityp == G_INT_I)                                                  \
        _oper = SS_INTEGER_VALUE(_num);                                      \
     else if (_ityp == G_FLOAT_I)                                           \
        {_oper = SS_FLOAT_VALUE(_num);                                       \
         _typ  = G_FLOAT_I;}                                                \
     else                                                                    \
        SS_error(_si, "ARGUMENT MUST BE A NUMBER - SS_GET_OPERAND", _num);}

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

void SS_install_mf(SS_psides *si, char* pname, char *pdoc, PFPHand phand, ...)
   {int i, n;
    SS_form ptype;
    PFVoid *pr;

    n  = 3;
    pr = CMAKE_N(PFVoid, n);

    SC_VA_START(phand);

    for (i = 0; i < n; i++)
        pr[i] = SC_VA_ARG(PFVoid);

    ptype = SC_VA_ARG(SS_form);

    SC_VA_END;

    _SS_install(si, pname, pdoc, phand, n, pr, ptype);

    return;}

/*--------------------------------------------------------------------------*/

/*                          MATHEMATICS HANDLERS                            */

/*--------------------------------------------------------------------------*/

/* SS_UNARY_FLT - the unary operator handler returning floats */

object *SS_unary_flt(SS_psides *si, SS_C_procedure *cp, object *argl)
   {int id;
    object *x, *rv;

    rv = SS_null;

    if (SS_nullobjp(argl))
       SS_error(si, "WRONG NUMBER OF ARGUMENTS - SS_UNARY_FLT", argl);

    x  = SS_car(si, argl);
    id = SC_arrtype(x, -1);

    if (id == G_INT_I)
       {double y;
	PFDoubled f;
	f  = (PFDoubled) cp->proc[0];
	y  = f(SS_INTEGER_VALUE(x));
	rv = SS_mk_float(si, y);}

    else if (id == G_FLOAT_I)
       {double y;
	PFDoubled f;
	f  = (PFDoubled) cp->proc[0];
	y  = f(SS_FLOAT_VALUE(x));
	rv = SS_mk_float(si, y);}

    else if (id == G_DOUBLE_COMPLEX_I)
       {double _Complex y;
	PFComplexc f;
	f  = (PFComplexc) cp->proc[1];
	y  = f(SS_COMPLEX_VALUE(x));
	rv = SS_mk_complex(si, y);}

    else if (id == G_QUATERNION_I)
       {quaternion y;
	PFQuaternionq f;
	f  = (PFQuaternionq) cp->proc[2];
	y  = f(SS_QUATERNION_VALUE(x));
	rv = SS_mk_quaternion(si, y);}

    else
       SS_error(si, "ARGUMENT MUST BE A NUMBER - SS_GET_OPERAND", x);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_UNARY_FIX - the unary operator handler returning fixed point numbers */

object *SS_unary_fix(SS_psides *si, SS_C_procedure *cp, object *argl)
   {int type;
    int64_t iv;
    double operand;
    PFDoubled fnc;
    object *rv;

    operand = 0.0;

    if (SS_nullobjp(argl))
       SS_error(si, "WRONG NUMBER OF ARGUMENTS - SS_UNARY_FIX", argl);

    fnc = (PFDoubled) cp->proc[0];
    SS_GET_OPERAND(si, operand, argl, type);

    SC_ASSERT(type != 0);

    iv = (*fnc)(operand);
    rv = SS_mk_integer(si, iv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_UNARY_BIT - the unary operator handler for bit operations */

static object *SS_unary_bit(SS_psides *si, SS_C_procedure *cp, object *argl)
   {int type;
    int64_t iv, operand;
    PFInt64I fnc;
    object *rv;

    operand = 0.0;

    if (SS_nullobjp(argl))
       SS_error(si, "WRONG NUMBER OF ARGUMENTS - SS_UNARY_BIT", argl);

    fnc = (PFInt64I) cp->proc[0];
    SS_GET_OPERAND(si, operand, argl, type);

    SC_ASSERT(type != 0);

    iv = (*fnc)(operand);
    rv = SS_mk_integer(si, iv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BINARY_OPR - the binary arithmetic operator handler */

static object *_SS_binary_opr(SS_psides *si, SS_C_procedure *cp, object *argl)
   {PFBINOBJ fnc;
    object *rv;

    if (SS_length(si, argl) != 2)
       SS_error(si, "WRONG NUMBER OF ARGUMENTS - _SS_BINARY_OPR", argl);

    fnc = (PFBINOBJ) cp->proc[0];

    rv = (*fnc)(si, argl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BINARY_FIX - the binary fixed point arithmetic operator handler */

object *SS_binary_fix(SS_psides *si, SS_C_procedure *cp, object *argl)
   {int64_t i1, i2, iv;
    PFInt64II fnc;
    object *x1, *x2, *rv;

    if (SS_length(si, argl) != 2)
       SS_error(si, "WRONG NUMBER OF ARGUMENTS - SS_BINARY_FIX", argl);

    fnc = (PFInt64II) cp->proc[0];
    x1  = SS_car(si, argl);
    x2  = SS_cadr(si, argl);

    if (!SS_integerp(x1) || !SS_integerp(x2))
       SS_error(si, "ARGUMENTS NOT BOTH INTEGERS - SS_BINARY_FIX",
                SS_mk_cons(si, x1, x2));

    i1 = SS_INTEGER_VALUE(x1);
    i2 = SS_INTEGER_VALUE(x2);

    iv = (*fnc)(i1, i2);
    rv = SS_mk_integer(si, iv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BIN_FIX - apply binary operator OP to list
 *             - with fixed point maximal type 
 */

static object *_SS_bin_fix(SS_psides *si, long ni,
			   object *argl, PFDoubledd op)
   {int idf, ident;
    long i, off;
    double accv, *v;
    object *acc;

    idf = ((op == PM_fdivide) || (op == HYPOT)) ? G_FLOAT_I : G_INT_I;

    v = CMAKE_N(double, ni);
    _SS_list_to_numtype_id(si, G_DOUBLE_I, v, ni, argl);

    if (ni < 2)
       {ident = ((op != PM_fplus) && (op != PM_fminus));
	accv  = ident;
        off   = 0;}
    else
       {accv = v[0];
        off  = 1;};

    for (i = off; i < ni; i++)
        accv = op(accv, v[i]);

    if ((SC_is_type_fix(idf) == TRUE) && (accv < LONG_MAX))
       acc = SS_mk_integer(si, accv);
    else
       acc = SS_mk_float(si, accv);

    CFREE(v);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BIN_FLOAT - apply binary operator OP to list
 *               - with floating point maximal type 
 */

static object *_SS_bin_float(SS_psides *si, long ni,
			     object *argl, PFDoubledd op)
   {int ident;
    long i, off;
    double accv, *v;
    object *acc;

    v = CMAKE_N(double, ni);
    _SS_list_to_numtype_id(si, G_DOUBLE_I, v, ni, argl);

    if (ni < 2)
       {ident = ((op != PM_fplus) && (op != PM_fminus));
	accv  = ident;
	off   = 0;}
    else
       {accv = v[0];
	off  = 1;};

    for (i = off; i < ni; i++)
        accv = op(accv, v[i]);

    acc = SS_mk_float(si, accv);

    CFREE(v);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BIN_COMPLEX - apply binary operator OP to list
 *                 - with complex floating point maximal type 
 */

static object *_SS_bin_complex(SS_psides *si, long ni,
			       object *argl, PFComplexcc op)
   {int ident;
    long i, off;
    double _Complex accv, *v;
    object *acc;

    v = CMAKE_N(double _Complex, ni);
    _SS_list_to_numtype_id(si, G_DOUBLE_COMPLEX_I, v, ni, argl);

    if (ni < 2)
       {ident = ((op != PM_plus_cc) && (op != PM_minus_cc));
	accv  = ident;
	off   = 0;}
    else
       {accv = v[0];
        off  = 1;};

    for (i = off; i < ni; i++)
        accv = op(accv, v[i]);

    acc = SS_mk_complex(si, accv);

    CFREE(v);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_BIN_QUATERNION - apply binary operator OP to list
 *                    - with quaternion floating point maximal type 
 */

static object *_SS_bin_quaternion(SS_psides *si, long ni,
				  object *argl, PFQuaternionqq op)
   {int ident;
    long i, off;
    quaternion accv, *v;
    object *acc;

    v = CMAKE_N(quaternion, ni);
    _SS_list_to_numtype_id(si, G_QUATERNION_I, v, ni, argl);

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

    acc = SS_mk_quaternion(si, accv);

    CFREE(v);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BINARY_HOMOGENEOUS - the binary floating point, complex, and quaternion
 *                       - arithmetic operator handler
 *                       - when both arguments have the same type
 */

object *SS_binary_homogeneous(SS_psides *si, SS_C_procedure *cp, object *argl)
   {int ido;
    long ni;
    object *acc;

    acc = SS_null;
    ido = _SS_max_numeric_type(si, argl, &ni);
    if (ido == -1)
       SS_error(si, "NON-NUMERIC OPERAND - SS_BINARY_HOMOGENEOUS", argl);

/* fixed point operands */
    else if (SC_is_type_fix(ido) == TRUE)
       acc = _SS_bin_fix(si, ni, argl, (PFDoubledd) cp->proc[0]);

/* floating point operands */
    else if (SC_is_type_fp(ido) == TRUE)
       acc = _SS_bin_float(si, ni, argl, (PFDoubledd) cp->proc[0]);

/* complex floating point operands */
    else if (SC_is_type_cx(ido) == TRUE)
       acc = _SS_bin_complex(si, ni, argl, (PFComplexcc) cp->proc[1]);

/* quaternion operands */
    else
       acc = _SS_bin_quaternion(si, ni, argl, (PFQuaternionqq) cp->proc[2]);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BINARY_HETEROGENEOUS - the binary floating point, complex, and
 *                         - quaternion arithmetic operator handler
 *                         - when both arguments have the different type
 *                         - for Bessel functions and Legendre polynomials
 */

object *SS_binary_heterogeneous(SS_psides *si, SS_C_procedure *cp, object *argl)
   {int ido;
    long ni;
    double n;
    object *acc;

    acc = SS_null;
    ido = _SS_max_numeric_type(si, argl, &ni);
    if (ido == -1)
       SS_error(si, "NON-NUMERIC OPERAND - SS_BINARY_HETEROGENEOUS", argl);

/* fixed point and floating point operands */
    else if ((SC_is_type_fix(ido) == TRUE) || (SC_is_type_fp(ido) == TRUE))
       {double x, accv;
	PFDoubledd op;

	SS_args(si, argl,
		G_DOUBLE_I, &x,
		G_DOUBLE_I, &n,
		0);

	op   = (PFDoubledd) cp->proc[0];
	accv = op(x, n);
	acc  = SS_mk_float(si, accv);}

/* complex floating point operands */
    else if (SC_is_type_cx(ido) == TRUE)
       {double _Complex z, accv;
	PFComplexcd op;

	SS_args(si, argl,
		G_DOUBLE_COMPLEX_I, &z,
		G_DOUBLE_I, &n,
		0);

	op   = (PFComplexcd) cp->proc[1];
	accv = op(z, n);
	acc  = SS_mk_complex(si, accv);}

/* quaternion operands */
    else
       {quaternion q, accv;
	PFQuaternionqd op;

	SS_args(si, argl,
		G_QUATERNION_I, &q,
		G_DOUBLE_I, &n,
		0);

	op   = (PFQuaternionqd) cp->proc[2];
	accv = op(q, n);
	acc  = SS_mk_quaternion(si, accv);};

    return(acc);}

/*--------------------------------------------------------------------------*/

/*                      HANDLER AUXILLIARY FUNCTIONS                        */

/*--------------------------------------------------------------------------*/

/*                        UNARAY OPERATIONS                                 */

/*--------------------------------------------------------------------------*/

/*                         PREDICATE HANDLERS                               */

/*--------------------------------------------------------------------------*/

/* SS_UN_COMP - the unary comparison handler */

object *SS_un_comp(SS_psides *si, SS_C_procedure *cp, object *argl)
   {int type, lv;
    double operand;
    PFIntd fnc;
    object *rv;

    operand = 0.0;

    if (SS_nullobjp(argl))
       SS_error(si, "WRONG NUMBER OF ARGUMENTS - SS_UN_COMP", argl);

    fnc = (PFIntd) cp->proc[0];
    SS_GET_OPERAND(si, operand, argl, type);

    SC_ASSERT(type != 0);

    lv = (*fnc)(operand);
    rv = lv ? SS_t : SS_f;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SS_BIN_COMP - the binary comparison handler */

object *SS_bin_comp(SS_psides *si, SS_C_procedure *cp, object *argl)
   {int type, lv;
    double c1, c2;
    PFIntdd fnc;
    object *rv;

    type = 0;
    c1   = 0.0;
    c2   = 0.0;

    if (SS_length(si, argl) != 2)
       SS_error(si, "WRONG NUMBER OF ARGUMENTS - SS_BIN_COMP", argl);

    fnc = (PFIntdd) cp->proc[0];
    SS_GET_OPERAND(si, c1, argl, type);
    SS_GET_OPERAND(si, c2, argl, type);

    SC_ASSERT(type != 0);

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

static object *_SS_machine_prec(SS_psides *si)
   {object *rv;

    rv = SS_mk_float(si, PM_machine_precision());

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_XOR_POW - return exclusive or iff in strict C conformance mode
 *             - otherwise return the power of a^b
 *             - NOTE: deviate from strict C and allow '^' to be exponentiation
 */

static object *_SS_xor_pow(SS_psides *si, object *argl)
   {int type;
    int64_t i1, i2, iv;
    double d1, d2, dv;
    object *rv;

    rv = SS_null;

    if (si->strict_c == TRUE)
       {SS_GET_OPERAND(si, i1, argl, type);
	SS_GET_OPERAND(si, i2, argl, type);
	iv = PM_lxor(i1, i2);
	rv = SS_mk_integer(si, iv);}

    else
       {SS_GET_OPERAND(si, d1, argl, type);
	SS_GET_OPERAND(si, d2, argl, type);
	dv = pow(d1, d2);
	rv = SS_mk_float(si, dv);};

    SC_ASSERT(type != 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INSTALL_HETEROGENEOUS - binary heterogeneous functions */

static void _SS_install_heterogeneous(SS_psides *si)
   {

    SS_install_mf(si, "in",
		  "Procedure: Take nth order modified Bessel function of the first kind",
		  SS_binary_heterogeneous, 
		  PM_in, PM_cin, PM_qin, SS_PR_PROC);

    SS_install_mf(si, "jn",
		  "Procedure: Take nth order Bessel function of the first kind",
		  SS_binary_heterogeneous, 
		  PM_jn, PM_cjn, PM_qjn, SS_PR_PROC);

    SS_install_mf(si, "kn",
		  "Procedure: Take nth order modified Bessel function of the first kind",
		  SS_binary_heterogeneous, 
		  PM_kn, PM_ckn, PM_qkn, SS_PR_PROC);

    SS_install_mf(si, "yn",
		  "Procedure: Take nth order Bessel function of the second kind",
		  SS_binary_heterogeneous, 
		  PM_yn, PM_cyn, PM_qyn, SS_PR_PROC);

    SS_install_mf(si, "tchn",
		  "Procedure: Take nth order Tchebyshev polynomial function",
		  SS_binary_heterogeneous, 
		  PM_tchn, PM_ctchn, PM_qtchn, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INSTALL_HOMOGENEOUS - binary homogeneous functions */

static void _SS_install_homogeneous(SS_psides *si)
   {

    SS_install(si, "hypot",
               "Procedure: Returns the sqrt of the sum of the squares of the arguments",
               SS_binary_homogeneous,
               HYPOT, SS_PR_PROC);

    SS_install(si, "max",
               "Procedure: Returns the greater of args",
               SS_binary_homogeneous,
               PM_fmax, SS_PR_PROC);

    SS_install(si, "min",
               "Procedure: Returns the smaller of args",
               SS_binary_homogeneous,
               PM_fmin, SS_PR_PROC);

    SS_install(si, "beta",
               "Procedure: Returns the beta function\nUsage: (beta z w)",
               SS_binary_homogeneous, 
               PM_beta, SS_PR_PROC);

    SS_install(si, "igamma-p",
               "Procedure: Returns the incomplete gamma function P\nUsage: (igamma-p a x)",
               SS_binary_homogeneous, 
               PM_igamma_p, SS_PR_PROC);

    SS_install(si, "igamma-q",
               "Procedure: Returns the incomplete gamma function Q\nUsage: (igamma-q a x)",
               SS_binary_homogeneous, 
               PM_igamma_q, SS_PR_PROC);

    SS_install(si, "lei1",
               "Procedure: Returns the Legendre elliptic integral of the first kind\nUsage: (lei1 x k)",
               SS_binary_homogeneous, 
               PM_elliptic_integral_l1, SS_PR_PROC);

    SS_install(si, "lei2",
               "Procedure: Returns the Legendre elliptic integral of the second kind\nUsage: (lei2 x k)",
               SS_binary_homogeneous, 
               PM_elliptic_integral_l2, SS_PR_PROC);

    SS_install(si, "cei3",
               "Procedure: Returns the complete elliptic integral of the third kind\nUsage: (cei3 n k)",
               SS_binary_homogeneous, 
               PM_elliptic_integral_c3, SS_PR_PROC);

    SS_install(si, "sn",
               "Procedure: Returns the Jacobian elliptic function sn\nUsage: (sn x k)",
               SS_binary_homogeneous, 
               PM_sn, SS_PR_PROC);

    SS_install(si, "cn",
               "Procedure: Returns the Jacobian elliptic function cn\nUsage: (cn x k)",
               SS_binary_homogeneous, 
               PM_cn, SS_PR_PROC);

    SS_install(si, "dn",
               "Procedure: Returns the Jacobian elliptic function dn\nUsage: (dn x k)",
               SS_binary_homogeneous, 
               PM_dn, SS_PR_PROC);

    SS_install_mf(si, "*",
		  "Procedure: Returns product of args or 1 if no args are supplied",
		  SS_binary_homogeneous,
		  PM_ftimes, PM_times_cc, PM_times_qq, SS_PR_PROC);

    SS_install_mf(si, "+",
		  "Procedure: Returns sum of args or 0 if no args are supplied",
		  SS_binary_homogeneous,
		  PM_fplus, PM_plus_cc, PM_plus_qq, SS_PR_PROC);

    SS_install_mf(si, "-",
		  "Procedure: Returns difference of args",
		  SS_binary_homogeneous,
		  PM_fminus, PM_minus_cc, PM_minus_qq, SS_PR_PROC);

    SS_install_mf(si, "/",
		  "Procedure: Returns quotient of args (left associative)",
		  SS_binary_homogeneous,
		  PM_fdivide, PM_divide_cc, PM_rquotient_qq, SS_PR_PROC);

    SS_install_mf(si, "expt",
		  "Procedure: Returns the first argument raised to the power of the second",
		  SS_binary_homogeneous,
		  POW, PM_cpow, PM_qpow, SS_PR_PROC);


    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SS_INSTALL_MATH - install the SCHEME primitives for math */

void _SS_install_math(SS_psides *si)
   {

    SS_install(si, "&",
               "Procedure: Returns bitwise and of args (left associative)",
               SS_binary_fix,
               PM_land, SS_PR_PROC);

    SS_install(si, "|",
               "Procedure: Returns bitwise or of args (left associative)",
               SS_binary_fix,
               PM_lor, SS_PR_PROC);

    SS_install(si, "~",
               "Procedure: Returns bitwise complement of its arg",
               SS_unary_bit, 
               PM_lcmp, SS_PR_PROC);

    SS_install(si, "<<",
               "Procedure: Returns left shift of args (left associative)",
               SS_binary_fix,
               PM_llshft, SS_PR_PROC);

    SS_install(si, ">>",
               "Procedure: Returns right shift of args (left associative)",
               SS_binary_fix,
               PM_lrshft, SS_PR_PROC);

    SS_install(si, "<",
               "Procedure: Returns #t iff the first argument is less than the second",
               SS_bin_comp,
               PM_flt, SS_PR_PROC);

    SS_install(si, "<=",
               "Procedure: Returns #t iff the first argument is less than or equal to the second",
               SS_bin_comp,
               PM_fle, SS_PR_PROC);

    SS_install(si, "=",
               "Procedure: Returns #t iff the first argument is equal to the second",
               SS_bin_comp,
               PM_feq, SS_PR_PROC);

    SS_install(si, "!=",
               "Procedure: Returns #t iff the first argument is not equal to the second",
               SS_bin_comp,
               PM_fneq, SS_PR_PROC);

    SS_install(si, ">",
               "Procedure: Returns #t iff the first argument is greater than the second",
               SS_bin_comp,
               PM_fgt, SS_PR_PROC);

    SS_install(si, ">=",
               "Procedure: Returns #t iff the first argument is greater than or equal to the second",
               SS_bin_comp,
               PM_fge, SS_PR_PROC);

    SS_install(si, "ceiling",
               "Procedure: Returns the smallest integer greater than the argument",
               SS_unary_fix, 
               ceil, SS_PR_PROC);

    SS_install(si, "even?",
               "Procedure: Returns #t iff the argument is a number divisible exactly by 2",
               SS_un_comp,
               _SS_even, SS_PR_PROC);

    SS_install(si, "floor",
               "Procedure: Returns the greatest integer less than the argument",
               SS_unary_fix, 
               floor, SS_PR_PROC);

    SS_install(si, "machine-precision",
               "Procedure: Returns a floating point machine precision value",
               SS_zargs,
               _SS_machine_prec, SS_PR_PROC);

    SS_install(si, "negative?",
               "Procedure: Returns #t iff the argument is a number less than 0",
               SS_un_comp,
               _SS_neg, SS_PR_PROC);

    SS_install(si, "odd?",
               "Procedure: Returns #t iff the argument is a number that is not even",
               SS_un_comp,
               _SS_odd, SS_PR_PROC);

    SS_install(si, "positive?",
               "Procedure: Returns #t iff the argument is a number greater than 0",
               SS_un_comp,
               _SS_pos, SS_PR_PROC);

    SS_install(si, "quotient",
               "Procedure: Returns quotient of two integers",
               SS_binary_fix,
               PM_ldivide, SS_PR_PROC);

    SS_install(si, "remainder",
               "Procedure: Returns remainder of division of the two arguments",
               SS_binary_fix,
               PM_lmodulo, SS_PR_PROC);

    SS_install(si, "truncate",
               "Procedure: Returns the integer resulting from the truncation of the argument",
               SS_unary_fix, 
               PM_fix, SS_PR_PROC);

    SS_install(si, "zero?",
               "Procedure: Returns #t iff the argument is a number equal to 0",
               SS_un_comp,
               _SS_zero, SS_PR_PROC);

/* special functions */

    SS_install(si, "lngamma",
               "Procedure: Returns the log of the gamma function\nUsage: (lngamma x)",
               SS_unary_flt, 
               PM_ln_gamma, SS_PR_PROC);

    SS_install(si, "erf",
               "Procedure: Returns the error function ERF\nUsage: (erf x)",
               SS_unary_flt, 
               PM_erf, SS_PR_PROC);

    SS_install(si, "erfc",
               "Procedure: Returns the complementary error function ERFC\nUsage: (erfc x)",
               SS_unary_flt, 
               PM_erfc, SS_PR_PROC);

    SS_install(si, "cei1",
               "Procedure: Returns the complete elliptic integral of the first kind\nUsage: (cei1 k)",
               SS_unary_flt, 
               PM_elliptic_integral_c1, SS_PR_PROC);

    SS_install(si, "cei2",
               "Procedure: Returns the complete elliptic integral of the second kind\nUsage: (cei2 k)",
               SS_unary_flt, 
               PM_elliptic_integral_c2, SS_PR_PROC);

/* multi type functions */
    SS_install_mf(si, "abs",
		  "Procedure: Returns the absolute value of a numeric object",
		  SS_unary_flt,
		  ABS, PM_cabs, PM_qnorm, SS_PR_PROC);

    SS_install_mf(si, "acos",
		  "Procedure: Returns the arc cosine of the argument",
		  SS_unary_flt, 
		  acos, PM_cacos, PM_qacos, SS_PR_PROC);

    SS_install_mf(si, "acosh",
		  "Procedure: Returns the arc hyperbolic cosine of the argument",
		  SS_unary_flt, 
		  acosh, PM_cacosh, PM_qacosh, SS_PR_PROC);

    SS_install_mf(si, "asin",
		  "Procedure: Returns the arc sine of the argument",
		  SS_unary_flt, 
		  asin, PM_casin, PM_qasin, SS_PR_PROC);

    SS_install_mf(si, "asinh",
		  "Procedure: Returns the arc hyperbolic sine of the argument",
		  SS_unary_flt, 
		  asinh, PM_casinh, PM_qasinh, SS_PR_PROC);

    SS_install_mf(si, "atan",
		  "Procedure: Returns the arc tangent of the argument",
		  SS_unary_flt, 
		  atan, PM_catan, PM_qatan, SS_PR_PROC);

    SS_install_mf(si, "atanh",
		  "Procedure: Returns the arc hyperbolic tangent of the argument",
		  SS_unary_flt, 
		  atanh, PM_catanh, PM_qatanh, SS_PR_PROC);

    SS_install_mf(si, "conjugate",
		  "Procedure: Returns the conjugate of the argument",
		  SS_unary_flt, 
		  PM_fplus, PM_cconjugate, PM_qconjugate, SS_PR_PROC);

    SS_install_mf(si, "cos",
		  "Procedure: Returns the cosine of the argument",
		  SS_unary_flt, 
		  cos, PM_ccos, PM_qcos, SS_PR_PROC);

    SS_install_mf(si, "cosh",
		  "Procedure: Returns the hyperbolic cosine of the argument",
		  SS_unary_flt, 
		  cosh, PM_ccosh, PM_qcosh, SS_PR_PROC);

    SS_install_mf(si, "cot",
		  "Procedure: Returns the cotangent of the argument",
		  SS_unary_flt, 
		  PM_cot, PM_ccot, PM_qcot, SS_PR_PROC);

    SS_install_mf(si, "coth",
		  "Procedure: Returns the hyperbolic cotangent of the argument",
		  SS_unary_flt, 
		  PM_coth, PM_ccoth, PM_qcoth, SS_PR_PROC);

    SS_install_mf(si, "exp",
		  "Procedure: Returns the exponential of the argument",
		  SS_unary_flt, 
		  exp, PM_cexp, PM_qexp, SS_PR_PROC);

    SS_install_mf(si, "i0",
		  "Procedure: Take zeroth order modified Bessel function of the first kind",
		  SS_unary_flt, 
		  PM_i0, PM_ci0, PM_qi0, SS_PR_PROC);

    SS_install_mf(si, "i1",
		  "Procedure: Take first order modified Bessel function of the first kind",
		  SS_unary_flt, 
		  PM_i1, PM_ci1, PM_qi1, SS_PR_PROC);

    SS_install_mf(si, "j1",
		  "Procedure: Take first order Bessel function of the first kind",
		  SS_unary_flt, 
		  PM_j1, PM_cj1, PM_qj1, SS_PR_PROC);

    SS_install_mf(si, "k0",
		  "Procedure: Take zeroth order modified Bessel function of the first kind",
		  SS_unary_flt, 
		  PM_k0, PM_ck0, PM_qk0, SS_PR_PROC);

    SS_install_mf(si, "k1",
		  "Procedure: Take first order modified Bessel function of the first kind",
		  SS_unary_flt, 
		  PM_k1, PM_ck1, PM_qk1, SS_PR_PROC);

    SS_install_mf(si, "y0",
		  "Procedure: Take zeroth order Bessel function of the second kind",
		  SS_unary_flt, 
		  PM_y0, PM_cy0, PM_qy0, SS_PR_PROC);

    SS_install_mf(si, "y1",
		  "Procedure: Take first order Bessel function of the second kind",
		  SS_unary_flt, 
		  PM_y1, PM_cy1, PM_qy1, SS_PR_PROC);

    SS_install_mf(si, "j0",
		  "Procedure: Take zeroth order Bessel function of the first kind",
		  SS_unary_flt, 
		  PM_j0, PM_cj0, PM_qj0, SS_PR_PROC);

    SS_install_mf(si, "ln",
		  "Procedure: Returns the natural logarithm of the argument",
		  SS_unary_flt, 
		  PM_ln, PM_cln, PM_qln, SS_PR_PROC);

    SS_install_mf(si, "log",
		  "Procedure: Returns the logarithm base 10 of the argument",
		  SS_unary_flt, 
		  PM_log, PM_clog, PM_qlog, SS_PR_PROC);

    SS_install_mf(si, "random",
		  "Procedure: Returns a random number between -1.0 and 1.0 using <seed>\nUsage: (random <seed>)",
		  SS_unary_flt, 
		  PM_random_s, PM_crandom, PM_qrandom, SS_PR_PROC);

    SS_install_mf(si, "recip",
		  "Procedure: Returns the reciprocal of the argument",
		  SS_unary_flt, 
		  PM_fdivide, PM_crecip, PM_qrecip, SS_PR_PROC);

    SS_install_mf(si, "square",
		  "Procedure: Returns the argument squared",
		  SS_unary_flt, 
		  PM_sqr, PM_csqr, PM_qsqr, SS_PR_PROC);

    SS_install_mf(si, "sqrt",
		  "Procedure: Returns the principal square root of the argument",
		  SS_unary_flt, 
		  sqrt, PM_csqrt, PM_qsqrt, SS_PR_PROC);

    SS_install_mf(si, "sin",
		  "Procedure: Returns the sine of the argument",
		  SS_unary_flt, 
		  sin, PM_csin, PM_qsin, SS_PR_PROC);

    SS_install_mf(si, "sinh",
		  "Procedure: Returns the hyperbolic sine of the argument",
		  SS_unary_flt, 
		  sinh, PM_csinh, PM_qsinh, SS_PR_PROC);

    SS_install_mf(si, "tan",
		  "Procedure: Returns the tangent of the argument",
		  SS_unary_flt, 
		  tan, PM_ctan, PM_qtan, SS_PR_PROC);

    SS_install_mf(si, "tanh",
		  "Procedure: Returns the hyperbolic tangent of the argument",
		  SS_unary_flt, 
		  tanh, PM_ctanh, PM_qtanh, SS_PR_PROC);

/* special syntax context */

    SS_install(si, "xor",
               "Procedure: Returns bitwise exclusive or of args (left associative)",
               SS_binary_fix,
               PM_lxor, SS_PR_PROC);

    SS_install(si, "^",
               "Procedure: Returns xor if strict C mode otherwise expt",
               _SS_binary_opr,
               _SS_xor_pow, SS_PR_PROC);

    SS_install_cf(si, "strict-c", 
		  "Variable: Strict C compliance for operators if 1\n     Usage: strict-c [0|1]",
		  SS_acc_int,
		  &si->strict_c);


    _SS_install_heterogeneous(si);
    _SS_install_homogeneous(si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
