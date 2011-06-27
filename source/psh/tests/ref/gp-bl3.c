
#include <Python.h>
#include "bl3_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *fe1(str *a1, int a2)| */

static char _PYI_fe1__doc__[] = "Procedure: fe1\n     Usage: fe1(a1, a2)";

static PyObject *_PYI_fe1(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {str *_la1;
    int _la2;
    str *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xi,
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fe1(_la1, _la2);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fe2(str *dev, double *x, double *y, int n, pcons *info, int l)| */

static char _PYI_fe2__doc__[] = "Procedure: fe2\n     Usage: fe2(dev, x, y, n, info, l)";

static PyObject *_PYI_fe2(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {str *_ldev;
    double *_lx;
    double *_ly;
    int _ln;
    pcons *_linfo;
    int _ll;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xxxixi,
                                     kw_list,
                                     &_ldev, &_lx, &_ly, &_ln, &_linfo, &_ll);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fe2(_ldev, _lx, _ly, _ln, _linfo, _ll);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
