
#include <Python.h>
#include "bl3_int.h"
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *fe1(str *a1, int a2)| */

static PyObject *_PY_fe1(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_la1;
    int _la2;
    str *_rv;
    char *kw_list[] = {"a1", "a2", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "Oi:fe1p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fe1(_la1, _la2);
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fe2(str *dev, double *x, double *y, int n, pcons *info, int l)| */

static PyObject *_PY_fe2(PyObject *self,
                         PyObject *args,
                         PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_ldev;
    double *_lx;
    double *_ly;
    int _ln;
    pcons *_linfo;
    int _ll;
    char *kw_list[] = {"dev", "x", "y", "n", "info", "l", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OOOiOi:fe2p",
                                     kw_list,
                                     &_ldev, &_lx, &_ly, &_ln, &_linfo, &_ll);
    if (ok == FALSE)
       return(NULL);

    fe2(_ldev, _lx, _ly, _ln, _linfo, _ll);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyMethodDef
 _PYD_fe1 = {"fe1p", (PyCFunction) _PY_fe1, METH_KEYWORDS,
             "Procedure: fe1p\n     Usage: fe1p(a1, a2)"},
 _PYD_fe2 = {"fe2p", (PyCFunction) _PY_fe2, METH_KEYWORDS,
             "Procedure: fe2p\n     Usage: fe2p(dev, x, y, n, info, l)"},
 _PYD_bl3_null;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
