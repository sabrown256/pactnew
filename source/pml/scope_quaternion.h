/*
 * SCOPE_QUATERNION.H - header defining private scope for quaternion
 *                    - math functions in PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

#ifndef PCK_SCOPE_QUATERNION
# define PCK_SCOPE_QUATERNION

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define PM_CLOSETO_QUATERNION(_ok, _x, _y, _tol)                             \
   {long double _del;                                                        \
    _del = PM_distance_qq(_x, _y);                                           \
    _ok  = (_del < _tol);}

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

typedef struct s_quaternion quaternion;

struct s_quaternion
   {double s;
    double i;
    double j;
    double k;};

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern quaternion
 PM_quaternion(double s, double i, double j, double k),
 PM_plus_qq(quaternion a, quaternion b),
 PM_minus_qq(quaternion a, quaternion b),
 PM_times_qq(quaternion a, quaternion b),
 PM_times_rq(double r, quaternion a),
 PM_qconjugate(quaternion a),
 PM_qrecip(quaternion a),
 PM_qversor(quaternion a),
 PM_qvunit(quaternion a),
 PM_lquotient_qq(quaternion a, quaternion b),
 PM_rquotient_qq(quaternion a, quaternion b),
 PM_qexp(quaternion a),
 PM_qln(quaternion a),
 PM_qlog(quaternion a),
 PM_qsqr(quaternion a),
 PM_qsqrt(quaternion a),
 PM_qrpow(quaternion a, double r),
 PM_qpow(quaternion a, quaternion r),
 PM_qsin(quaternion q),
 PM_qcos(quaternion q),
 PM_qtan(quaternion q),
 PM_qcot(quaternion q),
 PM_qsinh(quaternion q),
 PM_qcosh(quaternion q),
 PM_qtanh(quaternion q),
 PM_qcoth(quaternion q),
 PM_qasin(quaternion q),
 PM_qacos(quaternion q),
 PM_qatan(quaternion q),
 PM_qasinh(quaternion q),
 PM_qacosh(quaternion q),
 PM_qatanh(quaternion q),
 PM_qtchn(quaternion x, double n),
 PM_qi0(quaternion x),
 PM_qi1(quaternion x),
 PM_qin(quaternion x, double n),
 PM_qj0(quaternion x),
 PM_qj1(quaternion x),
 PM_qjn(quaternion x, double n),
 PM_qk0(quaternion x),
 PM_qk1(quaternion x),
 PM_qkn(quaternion x, double n),
 PM_qy0(quaternion x),
 PM_qy1(quaternion x),
 PM_qyn(quaternion x, double n),
 PM_qrandom(quaternion a);

extern double
 PM_qvnorm(quaternion a),
 PM_qnorm(quaternion a),
 PM_distance_qq(quaternion a, quaternion b);

extern int
 PM_qequal(quaternion a, quaternion b),
 PM_qclose(quaternion a, quaternion b, double tol);

extern void
 PM_rotate_q(double *v, double *u, double th);

#endif
