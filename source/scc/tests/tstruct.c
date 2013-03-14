/*
 * TSTRUCT.C - struct parse test
 *
 * Source Version: 5.0
 * Software Release #: LLNL-CODE-422942
 *
 * #include "cpyright.h"
 *
 */

struct s_info
   {int m1;
    double m2;
    short m3;};

static struct s_info
 v1s = {1, 2.0, };

extern struct s_info
 v1e;

struct s_info
 v1d;

static struct {int m1; double m2;}
 v2s;

extern struct {int m1; double m2;}
 v2e;

struct {int m1; double m2;}
 v2d;

/*
struct s2
   {int m1;
    int m2;};

extern struct s2 *f(struct s2 *a);
*/
