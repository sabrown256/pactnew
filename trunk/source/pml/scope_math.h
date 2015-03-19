/*
 * SCOPE_MATH.H - header defining private scope for math functions in PML
 *              - all other files using the scope MUST include the scope with:
 *              -
 *              -   #include "scope_math.h"
 *              -
 *              - this results in the variables being declared in the compilation
 *              - unit.
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#ifndef PCK_SCOPE_MATH
# define PCK_SCOPE_MATH

#ifdef WIN32
# define NO_BESSEL_FUNCTIONS
#endif

/*--------------------------------------------------------------------------*/

/*                           DEFINED CONSTANTS                              */

/*--------------------------------------------------------------------------*/

#define BESS_ACC 40.0

/*--------------------------------------------------------------------------*/

/*                           PROCEDURAL MACROS                              */

/*--------------------------------------------------------------------------*/

#define SIGNF(_x, _y)     ((_y < 0.0) ? -fabs(_x) : fabs(_x))

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

extern double
 _PM_j0_coeff_na[6],
 _PM_j0_coeff_da[6],
 _PM_j0_coeff_nb[5],
 _PM_j0_coeff_db[5],
 _PM_j1_coeff_na[6],
 _PM_j1_coeff_da[6],
 _PM_j1_coeff_nb[5],
 _PM_j1_coeff_db[5],
 _PM_y0_coeff_na[6],
 _PM_y0_coeff_da[6],
 _PM_y0_coeff_nb[5],
 _PM_y0_coeff_db[5],
 _PM_y1_coeff_na[6],
 _PM_y1_coeff_da[7],
 _PM_y1_coeff_nb[5],
 _PM_y1_coeff_db[5],
 _PM_i0_coeff_na[7],
 _PM_i0_coeff_nb[9],
 _PM_i1_coeff_na[7],
 _PM_i1_coeff_nb[9],
 _PM_k0_coeff_na[7],
 _PM_k0_coeff_nb[7],
 _PM_k1_coeff_na[7],
 _PM_k1_coeff_nb[7];

extern double
 two_ov_pi,
 pi_1_4,
 pi_3_4;

extern int
 _PM_use_pml_bessel;

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

#endif
