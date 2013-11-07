/*
 * GF-BL6.C - F90 wrappers for bl6
 *          - NOTE: this file was automatically generated
 *          - any manual changes will not be effective
 *
 */

#include "cpyright.h"
#include "sx_int.h"
#include "bl6_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa1(int a ARG(2,in))| */

int FF_ID(fa1f, FA1F)(int *a)
   {int _la;
    int _rv;

    _la        = (int) *a;

    _rv = fa1(_la);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa3(int *a ARG([1,2,3],in), int *b ARG([1,2,3],io), int *c ARG([1,2,3],out))| */

int FF_ID(fa3f, FA3F)(int *a, int *b, int *c)
   {int *_la;
    int *_lb;
    int *_lc;
    int _rv;

    _la        = (int *) a;
    _lb        = (int *) b;
    _lc        = (int *) c;

    _rv = fa3(_la, _lb, _lc);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa4(int *a ARG(,in), int *b ARG(,io), int *c ARG(,out))| */

int FF_ID(fa4f, FA4F)(int *a, int *b, int *c)
   {int *_la;
    int *_lb;
    int *_lc;
    int _rv;

    _la        = (int *) a;
    _lb        = (int *) b;
    _lc        = (int *) c;

    _rv = fa4(_la, _lb, _lc);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb1(int a, int b ARG(1,in))| */

void FF_ID(fb1_f, FB1_F)(int *a, int *b)
   {int _la;
    int _lb;
    
    _la        = (int) *a;
    _lb        = (int) *b;

    fb1(_la, _lb);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb2(int a, int b ARG(,in))| */

void FF_ID(fb2_f, FB2_F)(int *a, int *b)
   {int _la;
    int _lb;
    
    _la        = (int) *a;
    _lb        = (int) *b;

    fb2(_la, _lb);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb3(int a, int b ARG(*,in))| */

void FF_ID(fb3_f, FB3_F)(int *a, int *b)
   {int _la;
    int _lb;
    
    _la        = (int) *a;
    _lb        = (int) *b;

    fb3(_la, _lb);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc1(int a, int *b ARG(NULL,in), int *c ARG(NULL,io), int *d ARG(NULL,out))| */

void FF_ID(fc1_f, FC1_F)(int *a, int *b, int *c, int *d)
   {int _la;
    int *_lb;
    int *_lc;
    int *_ld;
    
    _la        = (int) *a;
    _lb        = (int *) b;
    _lc        = (int *) c;
    _ld        = (int *) d;

    fc1(_la, _lb, _lc, _ld);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc2(int a, int *b ARG(,in), int *c ARG(,io), int *d ARG(,out))| */

void FF_ID(fc2_f, FC2_F)(int *a, int *b, int *c, int *d)
   {int _la;
    int *_lb;
    int *_lc;
    int *_ld;
    
    _la        = (int) *a;
    _lb        = (int *) b;
    _lc        = (int *) c;
    _ld        = (int *) d;

    fc2(_la, _lb, _lc, _ld);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc3(int a, int *b ARG(*,in), int *c ARG(*,io), int *d ARG(*,out))| */

void FF_ID(fc3_f, FC3_F)(int *a, int *b, int *c, int *d)
   {int _la;
    int *_lb;
    int *_lc;
    int *_ld;
    
    _la        = (int) *a;
    _lb        = (int *) b;
    _lc        = (int *) c;
    _ld        = (int *) d;

    fc3(_la, _lb, _lc, _ld);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc4(int a, int *b ARG([1],in), int *c ARG([1],io), int *d ARG([1],out))| */

void FF_ID(fc4_f, FC4_F)(int *a, int *b, int *c, int *d)
   {int _la;
    int *_lb;
    int *_lc;
    int *_ld;
    
    _la        = (int) *a;
    _lb        = (int *) b;
    _lc        = (int *) c;
    _ld        = (int *) d;

    fc4(_la, _lb, _lc, _ld);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc5(int a, int *b ARG([1,2],in), int *c ARG([1,2],io), int *d ARG([1,2],out))| */

void FF_ID(fc5_f, FC5_F)(int *a, int *b, int *c, int *d)
   {int _la;
    int *_lb;
    int *_lc;
    int *_ld;
    
    _la        = (int) *a;
    _lb        = (int *) b;
    _lc        = (int *) c;
    _ld        = (int *) d;

    fc5(_la, _lb, _lc, _ld);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc6(int a, int *b ARG([*],in), int *c ARG([*],io), int *d ARG([*],out))| */

void FF_ID(fc6_f, FC6_F)(int *a, int *b, int *c, int *d)
   {int _la;
    int *_lb;
    int *_lc;
    int *_ld;
    
    _la        = (int) *a;
    _lb        = (int *) b;
    _lc        = (int *) c;
    _ld        = (int *) d;

    fc6(_la, _lb, _lc, _ld);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc7(int a, int *b ARG([*,*],in), int *c ARG([*,*],io), int *d ARG([*,*],out))| */

void FF_ID(fc7_f, FC7_F)(int *a, int *b, int *c, int *d)
   {int _la;
    int *_lb;
    int *_lc;
    int *_ld;
    
    _la        = (int) *a;
    _lb        = (int *) b;
    _lc        = (int *) c;
    _ld        = (int *) d;

    fc7(_la, _lb, _lc, _ld);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc8(char *s1 ARG("abc",in), char **s2 ARG(["def"],in), char *s3 ARG([*],in), char **s4 ARG([*,*],in))| */

void FF_ID(fc8_f, FC8_F)(char *s1, char ***s2, char *s3, char ***s4, int sncs1, int sncs3)
   {char _ls1[MAXLINE];
    char **_ls2;
    char _ls3[MAXLINE];
    char **_ls4;
    
    SC_FORTRAN_STR_C(_ls1, s1, sncs1);
    _ls2       = *(char ***) s2;
    SC_FORTRAN_STR_C(_ls3, s3, sncs3);
    _ls4       = *(char ***) s4;

    fc8(_ls1, _ls2, _ls3, _ls4);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc9(char c1, char c2 ARG('a',in), char *c3 ARG(['b'],in), char *c4 ARG([*],in), char *c5 ARG([*,*],in))| */

void FF_ID(fc9_f, FC9_F)(char *c1, char *c2, char *c3, char *c4, char *c5, int sncc3, int sncc4, int sncc5)
   {char _lc1;
    char _lc2;
    char _lc3[MAXLINE];
    char _lc4[MAXLINE];
    char _lc5[MAXLINE];
    
    _lc1       = (char) *c1;
    _lc2       = (char) *c2;
    SC_FORTRAN_STR_C(_lc3, c3, sncc3);
    SC_FORTRAN_STR_C(_lc4, c4, sncc4);
    SC_FORTRAN_STR_C(_lc5, c5, sncc5);

    fc9(_lc1, _lc2, _lc3, _lc4, _lc5);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
