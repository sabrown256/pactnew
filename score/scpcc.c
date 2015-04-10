/*
 * SCPCC.C - support for PCC compiler
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "scstd.h"

#ifdef COMPILER_PCC

# ifndef __cplusplus

static long double
 _PM_i_double[2] = { 0.0, 0.1 };

long double _Complex
 *_PM_i = (long double _Complex *) _PM_i_double;

# endif

#endif
