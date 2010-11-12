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
 PM_plus_qq(quaternion a, quaternion b),
 PM_minus_qq(quaternion a, quaternion b),
 PM_times_qq(quaternion a, quaternion b),
 PM_times_rq(double r, quaternion a),
 PM_qconjugate(quaternion a),
 PM_qrecip(quaternion a),
 PM_qversor(quaternion a),
 PM_lquotient_qq(quaternion a, quaternion b),
 PM_rquotient_qq(quaternion a, quaternion b),
 PM_qexp(quaternion a),
 PM_qln(quaternion a),
 PM_qpow(quaternion a, double r);

extern double
 PM_qnorm(quaternion a),
 PM_distance_qq(quaternion a, quaternion b);

extern void
 PM_rotate_q(double *v, double *u, double th);

#endif
