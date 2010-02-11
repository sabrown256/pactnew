/*
 * F77STD.H - a set of things used with CPP to smooth F77 differences
 *          - in various environments
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

#ifdef CRAY
#define DOUBLE_PRECISION  real
#endif

#ifdef REAL8
#define DOUBLE_PRECISION  real*8
#endif


