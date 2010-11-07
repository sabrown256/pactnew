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
 PM_conjugate_q(quaternion a),
 PM_recip_q(quaternion a),
 PM_versor_q(quaternion a),
 PM_lquotient_qq(quaternion a, quaternion b),
 PM_rquotient_qq(quaternion a, quaternion b),
 PM_exp_q(quaternion a),
 PM_ln_q(quaternion a),
 PM_pow_q(quaternion a, double r);

extern double
 PM_norm_q(quaternion a),
 PM_distance_qq(quaternion a, quaternion b);

extern void
 PM_rotate_q(double *v, double *u, double th);

#endif
