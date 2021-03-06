/*
 * GF-BL4.C - F90 wrappers for bl4
 *          - NOTE: this file was automatically generated by blang
 *          - any manual changes will not be effective
 *
 */

#include "cpyright.h"
#include "sx_int.h"
#include "bl4_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb1(bool a1, bool *a2)| */

int FF_ID(fb1_f, FB1_F)(bool *a1, bool *a2)
   {bool _la1;
    bool *_la2;
    int _rv;

    _la1       = (bool) *a1;
    _la2       = (bool *) a2;

    _rv = fb1(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb2(char a1, char *a2)| */

int FF_ID(fb2_f, FB2_F)(char *a1, char *a2, int snca2)
   {char _la1;
    char _la2[MAXLINE];
    int _rv;

    _la1       = (char) *a1;
    SC_FORTRAN_STR_C(_la2, a2, snca2);

    _rv = fb2(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb3(short a1, short *a2)| */

int FF_ID(fb3_f, FB3_F)(short *a1, short *a2)
   {short _la1;
    short *_la2;
    int _rv;

    _la1       = (short) *a1;
    _la2       = (short *) a2;

    _rv = fb3(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb4(int a1, int *a2)| */

int FF_ID(fb4_f, FB4_F)(int *a1, int *a2)
   {int _la1;
    int *_la2;
    int _rv;

    _la1       = (int) *a1;
    _la2       = (int *) a2;

    _rv = fb4(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb5(long a1, long *a2)| */

int FF_ID(fb5_f, FB5_F)(long *a1, long *a2)
   {long _la1;
    long *_la2;
    int _rv;

    _la1       = (long) *a1;
    _la2       = (long *) a2;

    _rv = fb5(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb6(long long a1, long long *a2)| */

int FF_ID(fb6_f, FB6_F)(long long *a1, long long *a2)
   {long long _la1;
    long long *_la2;
    int _rv;

    _la1       = (long long) *a1;
    _la2       = (long long *) a2;

    _rv = fb6(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb7(float a1, float *a2)| */

int FF_ID(fb7_f, FB7_F)(float *a1, float *a2)
   {float _la1;
    float *_la2;
    int _rv;

    _la1       = (float) *a1;
    _la2       = (float *) a2;

    _rv = fb7(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb8(double a1, double *a2)| */

int FF_ID(fb8_f, FB8_F)(double *a1, double *a2)
   {double _la1;
    double *_la2;
    int _rv;

    _la1       = (double) *a1;
    _la2       = (double *) a2;

    _rv = fb8(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb10(float _Complex a1, float _Complex *a2)| */

int FF_ID(fb10_f, FB10_F)(float _Complex *a1, float _Complex *a2)
   {float _Complex _la1;
    float _Complex *_la2;
    int _rv;

    _la1       = (float _Complex) *a1;
    _la2       = (float _Complex *) a2;

    _rv = fb10(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb11(double _Complex a1, double _Complex *a2)| */

int FF_ID(fb11_f, FB11_F)(double _Complex *a1, double _Complex *a2)
   {double _Complex _la1;
    double _Complex *_la2;
    int _rv;

    _la1       = (double _Complex) *a1;
    _la2       = (double _Complex *) a2;

    _rv = fb11(_la1, _la2);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |bool fr1(int a1)| */

bool FF_ID(fr1_f, FR1_F)(int *a1)
   {int _la1;
    bool _rv;

    _la1       = (int) *a1;

    _rv = fr1(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char fr2(int a1)| */

char FF_ID(fr2_f, FR2_F)(int *a1)
   {int _la1;
    char _rv;

    _la1       = (int) *a1;

    _rv = fr2(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |short fr3(int a1)| */

short FF_ID(fr3_f, FR3_F)(int *a1)
   {int _la1;
    short _rv;

    _la1       = (int) *a1;

    _rv = fr3(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fr4(int a1)| */

int FF_ID(fr4_f, FR4_F)(int *a1)
   {int _la1;
    int _rv;

    _la1       = (int) *a1;

    _rv = fr4(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long fr5(int a1)| */

long FF_ID(fr5_f, FR5_F)(int *a1)
   {int _la1;
    long _rv;

    _la1       = (int) *a1;

    _rv = fr5(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long long fr6(int a1)| */

long long FF_ID(fr6_f, FR6_F)(int *a1)
   {int _la1;
    long long _rv;

    _la1       = (int) *a1;

    _rv = fr6(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float fr7(int a1)| */

float FF_ID(fr7_f, FR7_F)(int *a1)
   {int _la1;
    float _rv;

    _la1       = (int) *a1;

    _rv = fr7(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double fr8(int a1)| */

double FF_ID(fr8_f, FR8_F)(int *a1)
   {int _la1;
    double _rv;

    _la1       = (int) *a1;

    _rv = fr8(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float _Complex fr10(int a1)| */

float _Complex FF_ID(fr10_f, FR10_F)(int *a1)
   {int _la1;
    float _Complex _rv;

    _la1       = (int) *a1;

    _rv = fr10(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double _Complex fr11(int a1)| */

double _Complex FF_ID(fr11_f, FR11_F)(int *a1)
   {int _la1;
    double _Complex _rv;

    _la1       = (int) *a1;

    _rv = fr11(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str fr13(int a1)| */

str FF_ID(fr13_f, FR13_F)(int *a1)
   {int _la1;
    str _rv;

    _la1       = (int) *a1;

    _rv = fr13(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |bool *fp1(int a1)| */

bool *FF_ID(fp1_f, FP1_F)(int *a1)
   {int _la1;
    bool *_rv;

    _la1       = (int) *a1;

    _rv = fp1(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fp2(int a1)| */

char *FF_ID(fp2_f, FP2_F)(int *a1)
   {int _la1;
    char *_rv;

    _la1       = (int) *a1;

    _rv = fp2(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |short *fp3(int a1)| */

short *FF_ID(fp3_f, FP3_F)(int *a1)
   {int _la1;
    short *_rv;

    _la1       = (int) *a1;

    _rv = fp3(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int *fp4(int a1)| */

int *FF_ID(fp4_f, FP4_F)(int *a1)
   {int _la1;
    int *_rv;

    _la1       = (int) *a1;

    _rv = fp4(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long *fp5(int a1)| */

long *FF_ID(fp5_f, FP5_F)(int *a1)
   {int _la1;
    long *_rv;

    _la1       = (int) *a1;

    _rv = fp5(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long long *fp6(int a1)| */

long long *FF_ID(fp6_f, FP6_F)(int *a1)
   {int _la1;
    long long *_rv;

    _la1       = (int) *a1;

    _rv = fp6(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float *fp7(int a1)| */

float *FF_ID(fp7_f, FP7_F)(int *a1)
   {int _la1;
    float *_rv;

    _la1       = (int) *a1;

    _rv = fp7(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double *fp8(int a1)| */

double *FF_ID(fp8_f, FP8_F)(int *a1)
   {int _la1;
    double *_rv;

    _la1       = (int) *a1;

    _rv = fp8(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float _Complex *fp10(int a1)| */

float _Complex *FF_ID(fp10_f, FP10_F)(int *a1)
   {int _la1;
    float _Complex *_rv;

    _la1       = (int) *a1;

    _rv = fp10(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double _Complex *fp11(int a1)| */

double _Complex *FF_ID(fp11_f, FP11_F)(int *a1)
   {int _la1;
    double _Complex *_rv;

    _la1       = (int) *a1;

    _rv = fp11(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *fp13(int a1)| */

void *FF_ID(fp13_f, FP13_F)(int *a1)
   {int _la1;
    void *_rv;

    _la1       = (int) *a1;

    _rv = (void *) fp13(_la1);

    return(_rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
