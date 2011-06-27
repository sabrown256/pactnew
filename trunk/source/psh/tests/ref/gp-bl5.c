
#include <Python.h>
#include "bl5_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f1(PFDouble a)| */

static char _PYI_a_f1__doc__[] = "Procedure: a_f1\n     Usage: a_f1(a)";

static PyObject *_PYI_a_f1(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {PFDouble _la;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     x,
                                     kw_list,
                                     &_la);
    if (ok == FALSE)
       _lo = NULL;
    else
       {a_f1(_la);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f2(int (*b)(int *a))| */

static char _PYI_a_f2__doc__[] = "Procedure: a_f2\n     Usage: a_f2(b)";

static PyObject *_PYI_a_f2(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int (*_lb)(int *a);
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     x,
                                     kw_list,
                                     &_lb);
    if (ok == FALSE)
       _lo = NULL;
    else
       {a_f2(_lb);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f3(PFDouble a, int (*b)(int *a))| */

static char _PYI_a_f3__doc__[] = "Procedure: a_f3\n     Usage: a_f3(a, b)";

static PyObject *_PYI_a_f3(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {PFDouble _la;
    int (*_lb)(int *a);
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xx,
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       _lo = NULL;
    else
       {a_f3(_la, _lb);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f4(int (*b)(int *a), PFDouble a)| */

static char _PYI_a_f4__doc__[] = "Procedure: a_f4\n     Usage: a_f4(b, a)";

static PyObject *_PYI_a_f4(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int (*_lb)(int *a);
    PFDouble _la;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xx,
                                     kw_list,
                                     &_lb, &_la);
    if (ok == FALSE)
       _lo = NULL;
    else
       {a_f4(_lb, _la);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f1(PFDouble a, int b)| */

static char _PYI_b_f1__doc__[] = "Procedure: b_f1\n     Usage: b_f1(a, b)";

static PyObject *_PYI_b_f1(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {PFDouble _la;
    int _lb;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xi,
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       _lo = NULL;
    else
       {B_f1(_la, _lb);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f2(int (*a)(int *a), int b)| */

static char _PYI_b_f2__doc__[] = "Procedure: b_f2\n     Usage: b_f2(a, b)";

static PyObject *_PYI_b_f2(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int (*_la)(int *a);
    int _lb;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xi,
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       _lo = NULL;
    else
       {B_f2(_la, _lb);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f3(PFDouble a, int (*b)(int *a), int c)| */

static char _PYI_b_f3__doc__[] = "Procedure: b_f3\n     Usage: b_f3(a, b, c)";

static PyObject *_PYI_b_f3(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {PFDouble _la;
    int (*_lb)(int *a);
    int _lc;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xxi,
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       _lo = NULL;
    else
       {B_f3(_la, _lb, _lc);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f4(int (*a)(int *a), PFDouble b, int c)| */

static char _PYI_b_f4__doc__[] = "Procedure: b_f4\n     Usage: b_f4(a, b, c)";

static PyObject *_PYI_b_f4(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int (*_la)(int *a);
    PFDouble _lb;
    int _lc;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     xxi,
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       _lo = NULL;
    else
       {B_f4(_la, _lb, _lc);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f5(int a, PFDouble b, int (*c)(int *a))| */

static char _PYI_b_f5__doc__[] = "Procedure: b_f5\n     Usage: b_f5(a, b, c)";

static PyObject *_PYI_b_f5(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int _la;
    PFDouble _lb;
    int (*_lc)(int *a);
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     ixx,
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       _lo = NULL;
    else
       {B_f5(_la, _lb, _lc);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f6(int a, int (*b)(int *a), PFDouble c)| */

static char _PYI_b_f6__doc__[] = "Procedure: b_f6\n     Usage: b_f6(a, b, c)";

static PyObject *_PYI_b_f6(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int _la;
    int (*_lb)(int *a);
    PFDouble _lc;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     ixx,
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       _lo = NULL;
    else
       {B_f6(_la, _lb, _lc);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
