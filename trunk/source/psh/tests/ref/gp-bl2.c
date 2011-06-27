
#include <Python.h>
#include "bl2_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fbcv1(int a, float b, double c, char d, int (*e)(void), PFInt f)| */

static char _PYI_fbcv1__doc__[] = "Procedure: fbcv1\n     Usage: fbcv1(a, b, c, d, e, f)";

static PyObject *_PYI_fbcv1(PyObject *self,
                            PyObject *args,
                            PyObject *kwds)
   {int _la;
    float _lb;
    double _lc;
    char _ld;
    int (*_le)(void);
    PFInt _lf;
    char *_rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     ixdxxx,
                                     kw_list,
                                     &_la, &_lb, &_lc, &_ld, &_le, &_lf);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fbcv1(_la, _lb, _lc, _ld, _le, _lf);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fbmcv1(int a, float b, double c, char d)| */

static char _PYI_fbmcv1__doc__[] = "Procedure: fbmcv1\n     Usage: fbmcv1(a, b, c, d)";

static PyObject *_PYI_fbmcv1(PyObject *self,
                             PyObject *args,
                             PyObject *kwds)
   {int _la;
    float _lb;
    double _lc;
    char _ld;
    char *_rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     ixdx,
                                     kw_list,
                                     &_la, &_lb, &_lc, &_ld);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fbmcv1(_la, _lb, _lc, _ld);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fbmcr2(int *a, float *b, double *c, char *d)| */

static char _PYI_fbmcr2__doc__[] = "Procedure: fbmcr2\n     Usage: fbmcr2(a, b, c, d)";

static PyObject *_PYI_fbmcr2(PyObject *self,
                             PyObject *args,
                             PyObject *kwds)
   {int *_la;
    float *_lb;
    double *_lc;
    char *_ld;
    char *_rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xxxx,
                                     kw_list,
                                     &_la, &_lb, &_lc, &_ld);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fbmcr2(_la, _lb, _lc, _ld);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
