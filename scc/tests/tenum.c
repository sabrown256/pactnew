/*
 * TENUM.C - enum parse test
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

enum e_info
   {E1, E2, E3};

static enum e_info
 v1s;

extern enum e_info
 v1e;

enum e_info
 v1d;

static enum {E1, E2}
 v2s;

extern enum {E1, E2}
 v2e;

enum {E1, E2}
 v2d;

