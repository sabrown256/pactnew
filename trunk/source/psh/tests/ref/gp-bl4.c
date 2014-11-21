/*
 * GP-BL4.C - generated support routines for BL4
 *
 */


#include "cpyright.h"
#include "py_int.h"
#include "bl4_int.h"
#include "gp-bl4.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb1(bool a1, bool *a2)| */

PyObject *_PY_fb1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    bool _la1;
    bool *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

/* local variable initializations */
    _la1       = FALSE;
    _la2       = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OO:fb1_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb1(_la1, _la2);
    _lo = PY_build_object("fb1",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb2(char a1, char *a2)| */

PyObject *_PY_fb2(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    char _la1;
    char *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

/* local variable initializations */
    _la1       = '\0';
    _la2       = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "ss:fb2_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb2(_la1, _la2);
    _lo = PY_build_object("fb2",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb3(short a1, short *a2)| */

PyObject *_PY_fb3(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    short _la1;
    short *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

/* local variable initializations */
    _la1       = 0;
    _la2       = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "hO:fb3_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb3(_la1, _la2);
    _lo = PY_build_object("fb3",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb4(int a1, int *a2)| */

PyObject *_PY_fb4(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    int *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

/* local variable initializations */
    _la1       = 0;
    _la2       = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iO:fb4_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb4(_la1, _la2);
    _lo = PY_build_object("fb4",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb5(long a1, long *a2)| */

PyObject *_PY_fb5(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    long _la1;
    long *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

/* local variable initializations */
    _la1       = 0L;
    _la2       = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "lO:fb5_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb5(_la1, _la2);
    _lo = PY_build_object("fb5",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb6(long long a1, long long *a2)| */

PyObject *_PY_fb6(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    long long _la1;
    long long *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

/* local variable initializations */
    _la1       = 0LL;
    _la2       = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "LO:fb6_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb6(_la1, _la2);
    _lo = PY_build_object("fb6",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb7(float a1, float *a2)| */

PyObject *_PY_fb7(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    float _la1;
    float *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

/* local variable initializations */
    _la1       = 0.0;
    _la2       = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "fO:fb7_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb7(_la1, _la2);
    _lo = PY_build_object("fb7",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb8(double a1, double *a2)| */

PyObject *_PY_fb8(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    double _la1;
    double *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

/* local variable initializations */
    _la1       = 0.0;
    _la2       = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "dO:fb8_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb8(_la1, _la2);
    _lo = PY_build_object("fb8",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb10(float _Complex a1, float _Complex *a2)| */

PyObject *_PY_fb10(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    float _Complex _la1;
    float _Complex *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

/* local variable initializations */
    _la1       = 0.0;
    _la2       = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OO:fb10_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb10(_la1, _la2);
    _lo = PY_build_object("fb10",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb11(double _Complex a1, double _Complex *a2)| */

PyObject *_PY_fb11(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    double _Complex _la1;
    double _Complex *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

/* local variable initializations */
    _la1       = 0.0;
    _la2       = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OO:fb11_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb11(_la1, _la2);
    _lo = PY_build_object("fb11",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |bool fr1(int a1)| */

PyObject *_PY_fr1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    bool _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr1_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr1(_la1);
    _lo = PY_build_object("fr1",
                          SC_BOOL_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char fr2(int a1)| */

PyObject *_PY_fr2(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    char _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr2_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr2(_la1);
    _lo = PY_build_object("fr2",
                          SC_CHAR_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |short fr3(int a1)| */

PyObject *_PY_fr3(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    short _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr3_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr3(_la1);
    _lo = PY_build_object("fr3",
                          SC_SHORT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fr4(int a1)| */

PyObject *_PY_fr4(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    int _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr4_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr4(_la1);
    _lo = PY_build_object("fr4",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long fr5(int a1)| */

PyObject *_PY_fr5(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    long _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr5_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr5(_la1);
    _lo = PY_build_object("fr5",
                          SC_LONG_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long long fr6(int a1)| */

PyObject *_PY_fr6(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    long long _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr6_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr6(_la1);
    _lo = PY_build_object("fr6",
                          SC_LONG_LONG_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float fr7(int a1)| */

PyObject *_PY_fr7(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    float _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr7_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr7(_la1);
    _lo = PY_build_object("fr7",
                          SC_FLOAT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double fr8(int a1)| */

PyObject *_PY_fr8(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    double _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr8_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr8(_la1);
    _lo = PY_build_object("fr8",
                          SC_DOUBLE_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float _Complex fr10(int a1)| */

PyObject *_PY_fr10(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    float _Complex _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr10_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr10(_la1);
    _lo = PY_build_object("fr10",
                          SC_FLOAT_COMPLEX_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double _Complex fr11(int a1)| */

PyObject *_PY_fr11(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    double _Complex _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr11_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr11(_la1);
    _lo = PY_build_object("fr11",
                          SC_DOUBLE_COMPLEX_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str fr13(int a1)| */

PyObject *_PY_fr13(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    str _rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr13_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr13(_la1);
    _lo = PY_build_object("fr13",
                          SX_STR_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |bool *fp1(int a1)| */

PyObject *_PY_fp1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    bool *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp1_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp1(_la1);
    _lo = PY_build_object("fp1",
                          SC_BOOL_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fp2(int a1)| */

PyObject *_PY_fp2(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    char *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp2_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp2(_la1);
    _lo = PY_build_object("fp2",
                          SC_CHAR_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |short *fp3(int a1)| */

PyObject *_PY_fp3(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    short *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp3_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp3(_la1);
    _lo = PY_build_object("fp3",
                          SC_SHORT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int *fp4(int a1)| */

PyObject *_PY_fp4(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    int *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp4_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp4(_la1);
    _lo = PY_build_object("fp4",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long *fp5(int a1)| */

PyObject *_PY_fp5(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    long *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp5_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp5(_la1);
    _lo = PY_build_object("fp5",
                          SC_LONG_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long long *fp6(int a1)| */

PyObject *_PY_fp6(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    long long *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp6_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp6(_la1);
    _lo = PY_build_object("fp6",
                          SC_LONG_LONG_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float *fp7(int a1)| */

PyObject *_PY_fp7(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    float *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp7_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp7(_la1);
    _lo = PY_build_object("fp7",
                          SC_FLOAT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double *fp8(int a1)| */

PyObject *_PY_fp8(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    double *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp8_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp8(_la1);
    _lo = PY_build_object("fp8",
                          SC_DOUBLE_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float _Complex *fp10(int a1)| */

PyObject *_PY_fp10(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    float _Complex *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp10_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp10(_la1);
    _lo = PY_build_object("fp10",
                          SC_FLOAT_COMPLEX_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double _Complex *fp11(int a1)| */

PyObject *_PY_fp11(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    double _Complex *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp11_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp11(_la1);
    _lo = PY_build_object("fp11",
                          SC_DOUBLE_COMPLEX_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *fp13(int a1)| */

PyObject *_PY_fp13(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    str *_rv;
    char *kw_list[] = {"a1", NULL};

/* local variable initializations */
    _la1       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp13_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp13(_la1);
    _lo = PY_build_object("fp13",
                          SX_STR_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_add_bl4_enum(PyObject *m)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_init_bl4(PyObject *m, PyObject *d)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
