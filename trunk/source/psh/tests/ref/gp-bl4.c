
#include <Python.h>
#include "bl4_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb1(bool a1, bool *a2)| */

static char _PYI_fb1__doc__[] = "Procedure: fb1\n     Usage: fb1(a1, a2)";

static PyObject *_PYI_fb1(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {bool _la1;
    bool *_la2;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xx,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fb1(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb2(char a1, char *a2)| */

static char _PYI_fb2__doc__[] = "Procedure: fb2\n     Usage: fb2(a1, a2)";

static PyObject *_PYI_fb2(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {char _la1;
    char *_la2;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xx,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fb2(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb3(short a1, short *a2)| */

static char _PYI_fb3__doc__[] = "Procedure: fb3\n     Usage: fb3(a1, a2)";

static PyObject *_PYI_fb3(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {short _la1;
    short *_la2;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xx,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fb3(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb4(int a1, int *a2)| */

static char _PYI_fb4__doc__[] = "Procedure: fb4\n     Usage: fb4(a1, a2)";

static PyObject *_PYI_fb4(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    int *_la2;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     ix,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fb4(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb5(long a1, long *a2)| */

static char _PYI_fb5__doc__[] = "Procedure: fb5\n     Usage: fb5(a1, a2)";

static PyObject *_PYI_fb5(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {long _la1;
    long *_la2;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xx,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fb5(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb6(long long a1, long long *a2)| */

static char _PYI_fb6__doc__[] = "Procedure: fb6\n     Usage: fb6(a1, a2)";

static PyObject *_PYI_fb6(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {long long _la1;
    long long *_la2;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xx,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fb6(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb7(float a1, float *a2)| */

static char _PYI_fb7__doc__[] = "Procedure: fb7\n     Usage: fb7(a1, a2)";

static PyObject *_PYI_fb7(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {float _la1;
    float *_la2;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xx,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fb7(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb8(double a1, double *a2)| */

static char _PYI_fb8__doc__[] = "Procedure: fb8\n     Usage: fb8(a1, a2)";

static PyObject *_PYI_fb8(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {double _la1;
    double *_la2;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     dx,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fb8(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb10(float _Complex a1, float _Complex *a2)| */

static char _PYI_fb10__doc__[] = "Procedure: fb10\n     Usage: fb10(a1, a2)";

static PyObject *_PYI_fb10(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {float _Complex _la1;
    float _Complex *_la2;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xx,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fb10(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb11(double _Complex a1, double _Complex *a2)| */

static char _PYI_fb11__doc__[] = "Procedure: fb11\n     Usage: fb11(a1, a2)";

static PyObject *_PYI_fb11(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {double _Complex _la1;
    double _Complex *_la2;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xx,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fb11(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |bool fr1(int a1)| */

static char _PYI_fr1__doc__[] = "Procedure: fr1\n     Usage: fr1(a1)";

static PyObject *_PYI_fr1(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    bool _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr1(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char fr2(int a1)| */

static char _PYI_fr2__doc__[] = "Procedure: fr2\n     Usage: fr2(a1)";

static PyObject *_PYI_fr2(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    char _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr2(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |short fr3(int a1)| */

static char _PYI_fr3__doc__[] = "Procedure: fr3\n     Usage: fr3(a1)";

static PyObject *_PYI_fr3(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    short _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr3(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fr4(int a1)| */

static char _PYI_fr4__doc__[] = "Procedure: fr4\n     Usage: fr4(a1)";

static PyObject *_PYI_fr4(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr4(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long fr5(int a1)| */

static char _PYI_fr5__doc__[] = "Procedure: fr5\n     Usage: fr5(a1)";

static PyObject *_PYI_fr5(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    long _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr5(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long long fr6(int a1)| */

static char _PYI_fr6__doc__[] = "Procedure: fr6\n     Usage: fr6(a1)";

static PyObject *_PYI_fr6(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    long long _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr6(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float fr7(int a1)| */

static char _PYI_fr7__doc__[] = "Procedure: fr7\n     Usage: fr7(a1)";

static PyObject *_PYI_fr7(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    float _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr7(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double fr8(int a1)| */

static char _PYI_fr8__doc__[] = "Procedure: fr8\n     Usage: fr8(a1)";

static PyObject *_PYI_fr8(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    double _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr8(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float _Complex fr10(int a1)| */

static char _PYI_fr10__doc__[] = "Procedure: fr10\n     Usage: fr10(a1)";

static PyObject *_PYI_fr10(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int _la1;
    float _Complex _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr10(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double _Complex fr11(int a1)| */

static char _PYI_fr11__doc__[] = "Procedure: fr11\n     Usage: fr11(a1)";

static PyObject *_PYI_fr11(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int _la1;
    double _Complex _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr11(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str fr13(int a1)| */

static char _PYI_fr13__doc__[] = "Procedure: fr13\n     Usage: fr13(a1)";

static PyObject *_PYI_fr13(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int _la1;
    str _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fr13(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |bool *fp1(int a1)| */

static char _PYI_fp1__doc__[] = "Procedure: fp1\n     Usage: fp1(a1)";

static PyObject *_PYI_fp1(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    bool *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp1(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fp2(int a1)| */

static char _PYI_fp2__doc__[] = "Procedure: fp2\n     Usage: fp2(a1)";

static PyObject *_PYI_fp2(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    char *_rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp2(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |short *fp3(int a1)| */

static char _PYI_fp3__doc__[] = "Procedure: fp3\n     Usage: fp3(a1)";

static PyObject *_PYI_fp3(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    short *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp3(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int *fp4(int a1)| */

static char _PYI_fp4__doc__[] = "Procedure: fp4\n     Usage: fp4(a1)";

static PyObject *_PYI_fp4(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    int *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp4(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long *fp5(int a1)| */

static char _PYI_fp5__doc__[] = "Procedure: fp5\n     Usage: fp5(a1)";

static PyObject *_PYI_fp5(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    long *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp5(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long long *fp6(int a1)| */

static char _PYI_fp6__doc__[] = "Procedure: fp6\n     Usage: fp6(a1)";

static PyObject *_PYI_fp6(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    long long *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp6(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float *fp7(int a1)| */

static char _PYI_fp7__doc__[] = "Procedure: fp7\n     Usage: fp7(a1)";

static PyObject *_PYI_fp7(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    float *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp7(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double *fp8(int a1)| */

static char _PYI_fp8__doc__[] = "Procedure: fp8\n     Usage: fp8(a1)";

static PyObject *_PYI_fp8(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la1;
    double *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp8(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float _Complex *fp10(int a1)| */

static char _PYI_fp10__doc__[] = "Procedure: fp10\n     Usage: fp10(a1)";

static PyObject *_PYI_fp10(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int _la1;
    float _Complex *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp10(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double _Complex *fp11(int a1)| */

static char _PYI_fp11__doc__[] = "Procedure: fp11\n     Usage: fp11(a1)";

static PyObject *_PYI_fp11(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int _la1;
    double _Complex *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp11(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *fp13(int a1)| */

static char _PYI_fp13__doc__[] = "Procedure: fp13\n     Usage: fp13(a1)";

static PyObject *_PYI_fp13(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int _la1;
    str *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fp13(_la1);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
