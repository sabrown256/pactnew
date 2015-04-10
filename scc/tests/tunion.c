/*
 * TUNION.C - union parse test
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

union u_info
   {int m1;
    double m2;};

static union u_info
 v1s;

extern union u_info
 v1e;

union u_info
 v1d;

static union {int m1; double m2;}
 v2s;

extern union {int m1; double m2;}
 v2e;

union {int m1; double m2;}
 v2d;

