
#include <Python.h>
#include "bl2_int.h"
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fbcv1(int a, float b, double c, char d, int (*e)(void), PFInt f)| */

PyObject *_PY_fbcv1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    float _lb;
    double _lc;
    char _ld;
    int (*_le)(void);
    PFInt _lf;
    char *_rv;
    char *kw_list[] = {"a", "b", "c", "d", "e", "f", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "ifdbOO:fbcv1p",
                                     kw_list,
                                     &_la, &_lb, &_lc, &_ld, &_le, &_lf);
    if (ok == FALSE)
       return(NULL);

    _rv = fbcv1(_la, _lb, _lc, _ld, _le, _lf);
    _lo = Py_BuildValue("b",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fbmcv1(int a, float b, double c, char d)| */

PyObject *_PY_fbmcv1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    float _lb;
    double _lc;
    char _ld;
    char *_rv;
    char *kw_list[] = {"a", "b", "c", "d", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "ifdb:fbmcv1p",
                                     kw_list,
                                     &_la, &_lb, &_lc, &_ld);
    if (ok == FALSE)
       return(NULL);

    _rv = fbmcv1(_la, _lb, _lc, _ld);
    _lo = Py_BuildValue("b",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |char *fbmcr2(int *a, float *b, double *c, char *d)| */

PyObject *_PY_fbmcr2(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int *_la;
    float *_lb;
    double *_lc;
    char *_ld;
    char *_rv;
    char *kw_list[] = {"a", "b", "c", "d", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OOOs:fbmcr2p",
                                     kw_list,
                                     &_la, &_lb, &_lc, &_ld);
    if (ok == FALSE)
       return(NULL);

    _rv = fbmcr2(_la, _lb, _lc, _ld);
    _lo = Py_BuildValue("b",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyMethodDef
 _PYD_fbcv1 = {"fbcv1p", (PyCFunction) _PY_fbcv1, METH_KEYWORDS,
               "Procedure: fbcv1p\n     Usage: fbcv1p(a, b, c, d, e, f)"},
 _PYD_fbmcv1 = {"fbmcv1p", (PyCFunction) _PY_fbmcv1, METH_KEYWORDS,
                "Procedure: fbmcv1p\n     Usage: fbmcv1p(a, b, c, d)"},
 _PYD_fbmcr2 = {"fbmcr2p", (PyCFunction) _PY_fbmcr2, METH_KEYWORDS,
                "Procedure: fbmcr2p\n     Usage: fbmcr2p(a, b, c, d)"},
 _PYD_bl2_null;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/