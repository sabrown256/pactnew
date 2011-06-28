
#include <Python.h>
#include "bl4_int.h"
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb1(bool a1, bool *a2)| */

static PyObject *_PY_fb1(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    bool _la1;
    bool *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OO:fb1_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb1(_la1, _la2);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb2(char a1, char *a2)| */

static PyObject *_PY_fb2(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    char _la1;
    char *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "bs:fb2_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb2(_la1, _la2);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb3(short a1, short *a2)| */

static PyObject *_PY_fb3(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    short _la1;
    short *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "hO:fb3_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb3(_la1, _la2);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb4(int a1, int *a2)| */

static PyObject *_PY_fb4(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    int *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iO:fb4_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb4(_la1, _la2);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb5(long a1, long *a2)| */

static PyObject *_PY_fb5(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    long _la1;
    long *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "lO:fb5_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb5(_la1, _la2);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb6(long long a1, long long *a2)| */

static PyObject *_PY_fb6(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    long long _la1;
    long long *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "LO:fb6_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb6(_la1, _la2);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb7(float a1, float *a2)| */

static PyObject *_PY_fb7(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    float _la1;
    float *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "fO:fb7_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb7(_la1, _la2);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb8(double a1, double *a2)| */

static PyObject *_PY_fb8(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    double _la1;
    double *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "dO:fb8_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb8(_la1, _la2);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb10(float _Complex a1, float _Complex *a2)| */

static PyObject *_PY_fb10(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    float _Complex _la1;
    float _Complex *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OO:fb10_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb10(_la1, _la2);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fb11(double _Complex a1, double _Complex *a2)| */

static PyObject *_PY_fb11(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    double _Complex _la1;
    double _Complex *_la2;
    int _rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OO:fb11_p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fb11(_la1, _la2);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |bool fr1(int a1)| */

static PyObject *_PY_fr1(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    bool _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr1_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr1(_la1);
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char fr2(int a1)| */

static PyObject *_PY_fr2(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    char _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr2_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr2(_la1);
    _lo = Py_BuildValue("b",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |short fr3(int a1)| */

static PyObject *_PY_fr3(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    short _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr3_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr3(_la1);
    _lo = Py_BuildValue("h",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fr4(int a1)| */

static PyObject *_PY_fr4(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    int _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr4_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr4(_la1);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long fr5(int a1)| */

static PyObject *_PY_fr5(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    long _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr5_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr5(_la1);
    _lo = Py_BuildValue("l",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long long fr6(int a1)| */

static PyObject *_PY_fr6(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    long long _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr6_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr6(_la1);
    _lo = Py_BuildValue("L",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float fr7(int a1)| */

static PyObject *_PY_fr7(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    float _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr7_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr7(_la1);
    _lo = Py_BuildValue("f",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double fr8(int a1)| */

static PyObject *_PY_fr8(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    double _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr8_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr8(_la1);
    _lo = Py_BuildValue("d",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float _Complex fr10(int a1)| */

static PyObject *_PY_fr10(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    float _Complex _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr10_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr10(_la1);
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double _Complex fr11(int a1)| */

static PyObject *_PY_fr11(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    double _Complex _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr11_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr11(_la1);
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str fr13(int a1)| */

static PyObject *_PY_fr13(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    str _rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fr13_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fr13(_la1);
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |bool *fp1(int a1)| */

static PyObject *_PY_fp1(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    bool *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp1_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp1(_la1);
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fp2(int a1)| */

static PyObject *_PY_fp2(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    char *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp2_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp2(_la1);
    _lo = Py_BuildValue("b",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |short *fp3(int a1)| */

static PyObject *_PY_fp3(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    short *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp3_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp3(_la1);
    _lo = Py_BuildValue("h",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int *fp4(int a1)| */

static PyObject *_PY_fp4(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    int *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp4_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp4(_la1);
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long *fp5(int a1)| */

static PyObject *_PY_fp5(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    long *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp5_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp5(_la1);
    _lo = Py_BuildValue("l",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |long long *fp6(int a1)| */

static PyObject *_PY_fp6(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    long long *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp6_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp6(_la1);
    _lo = Py_BuildValue("L",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float *fp7(int a1)| */

static PyObject *_PY_fp7(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    float *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp7_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp7(_la1);
    _lo = Py_BuildValue("f",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double *fp8(int a1)| */

static PyObject *_PY_fp8(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    double *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp8_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp8(_la1);
    _lo = Py_BuildValue("d",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |float _Complex *fp10(int a1)| */

static PyObject *_PY_fp10(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    float _Complex *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp10_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp10(_la1);
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double _Complex *fp11(int a1)| */

static PyObject *_PY_fp11(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    double _Complex *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp11_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp11(_la1);
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *fp13(int a1)| */

static PyObject *_PY_fp13(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la1;
    str *_rv;
    char *kw_list[] = {"a1", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fp13_p",
                                     kw_list,
                                     &_la1);
    if (ok == FALSE)
       return(NULL);

    _rv = fp13(_la1);
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyMethodDef
 _PYD_fb1 = {"fb1", (PyCFunction) _PY_fb1, METH_KEYWORDS,
             "Procedure: fb1\n     Usage: fb1(a1, a2)"},
 _PYD_fb2 = {"fb2", (PyCFunction) _PY_fb2, METH_KEYWORDS,
             "Procedure: fb2\n     Usage: fb2(a1, a2)"},
 _PYD_fb3 = {"fb3", (PyCFunction) _PY_fb3, METH_KEYWORDS,
             "Procedure: fb3\n     Usage: fb3(a1, a2)"},
 _PYD_fb4 = {"fb4", (PyCFunction) _PY_fb4, METH_KEYWORDS,
             "Procedure: fb4\n     Usage: fb4(a1, a2)"},
 _PYD_fb5 = {"fb5", (PyCFunction) _PY_fb5, METH_KEYWORDS,
             "Procedure: fb5\n     Usage: fb5(a1, a2)"},
 _PYD_fb6 = {"fb6", (PyCFunction) _PY_fb6, METH_KEYWORDS,
             "Procedure: fb6\n     Usage: fb6(a1, a2)"},
 _PYD_fb7 = {"fb7", (PyCFunction) _PY_fb7, METH_KEYWORDS,
             "Procedure: fb7\n     Usage: fb7(a1, a2)"},
 _PYD_fb8 = {"fb8", (PyCFunction) _PY_fb8, METH_KEYWORDS,
             "Procedure: fb8\n     Usage: fb8(a1, a2)"},
 _PYD_fb10 = {"fb10", (PyCFunction) _PY_fb10, METH_KEYWORDS,
              "Procedure: fb10\n     Usage: fb10(a1, a2)"},
 _PYD_fb11 = {"fb11", (PyCFunction) _PY_fb11, METH_KEYWORDS,
              "Procedure: fb11\n     Usage: fb11(a1, a2)"},
 _PYD_fr1 = {"fr1", (PyCFunction) _PY_fr1, METH_KEYWORDS,
             "Procedure: fr1\n     Usage: fr1(a1)"},
 _PYD_fr2 = {"fr2", (PyCFunction) _PY_fr2, METH_KEYWORDS,
             "Procedure: fr2\n     Usage: fr2(a1)"},
 _PYD_fr3 = {"fr3", (PyCFunction) _PY_fr3, METH_KEYWORDS,
             "Procedure: fr3\n     Usage: fr3(a1)"},
 _PYD_fr4 = {"fr4", (PyCFunction) _PY_fr4, METH_KEYWORDS,
             "Procedure: fr4\n     Usage: fr4(a1)"},
 _PYD_fr5 = {"fr5", (PyCFunction) _PY_fr5, METH_KEYWORDS,
             "Procedure: fr5\n     Usage: fr5(a1)"},
 _PYD_fr6 = {"fr6", (PyCFunction) _PY_fr6, METH_KEYWORDS,
             "Procedure: fr6\n     Usage: fr6(a1)"},
 _PYD_fr7 = {"fr7", (PyCFunction) _PY_fr7, METH_KEYWORDS,
             "Procedure: fr7\n     Usage: fr7(a1)"},
 _PYD_fr8 = {"fr8", (PyCFunction) _PY_fr8, METH_KEYWORDS,
             "Procedure: fr8\n     Usage: fr8(a1)"},
 _PYD_fr10 = {"fr10", (PyCFunction) _PY_fr10, METH_KEYWORDS,
              "Procedure: fr10\n     Usage: fr10(a1)"},
 _PYD_fr11 = {"fr11", (PyCFunction) _PY_fr11, METH_KEYWORDS,
              "Procedure: fr11\n     Usage: fr11(a1)"},
 _PYD_fr13 = {"fr13", (PyCFunction) _PY_fr13, METH_KEYWORDS,
              "Procedure: fr13\n     Usage: fr13(a1)"},
 _PYD_fp1 = {"fp1", (PyCFunction) _PY_fp1, METH_KEYWORDS,
             "Procedure: fp1\n     Usage: fp1(a1)"},
 _PYD_fp2 = {"fp2", (PyCFunction) _PY_fp2, METH_KEYWORDS,
             "Procedure: fp2\n     Usage: fp2(a1)"},
 _PYD_fp3 = {"fp3", (PyCFunction) _PY_fp3, METH_KEYWORDS,
             "Procedure: fp3\n     Usage: fp3(a1)"},
 _PYD_fp4 = {"fp4", (PyCFunction) _PY_fp4, METH_KEYWORDS,
             "Procedure: fp4\n     Usage: fp4(a1)"},
 _PYD_fp5 = {"fp5", (PyCFunction) _PY_fp5, METH_KEYWORDS,
             "Procedure: fp5\n     Usage: fp5(a1)"},
 _PYD_fp6 = {"fp6", (PyCFunction) _PY_fp6, METH_KEYWORDS,
             "Procedure: fp6\n     Usage: fp6(a1)"},
 _PYD_fp7 = {"fp7", (PyCFunction) _PY_fp7, METH_KEYWORDS,
             "Procedure: fp7\n     Usage: fp7(a1)"},
 _PYD_fp8 = {"fp8", (PyCFunction) _PY_fp8, METH_KEYWORDS,
             "Procedure: fp8\n     Usage: fp8(a1)"},
 _PYD_fp10 = {"fp10", (PyCFunction) _PY_fp10, METH_KEYWORDS,
              "Procedure: fp10\n     Usage: fp10(a1)"},
 _PYD_fp11 = {"fp11", (PyCFunction) _PY_fp11, METH_KEYWORDS,
              "Procedure: fp11\n     Usage: fp11(a1)"},
 _PYD_fp13 = {"fp13", (PyCFunction) _PY_fp13, METH_KEYWORDS,
              "Procedure: fp13\n     Usage: fp13(a1)"},
 _PYD_bl4_null;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
