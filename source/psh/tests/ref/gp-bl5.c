
#include <Python.h>
#include "bl5_int.h"
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f1(PFDouble a)| */

static PyObject *_PY_a_f1(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    PFDouble _la;
    char *kw_list[] = {"a", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "O:a_f1_p",
                                     kw_list,
                                     &_la);
    if (ok == FALSE)
       return(NULL);

    a_f1(_la);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f2(int (*b)(int *a))| */

static PyObject *_PY_a_f2(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int (*_lb)(int *a);
    char *kw_list[] = {"b", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "O:a_f2_p",
                                     kw_list,
                                     &_lb);
    if (ok == FALSE)
       return(NULL);

    a_f2(_lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f3(PFDouble a, int (*b)(int *a))| */

static PyObject *_PY_a_f3(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    PFDouble _la;
    int (*_lb)(int *a);
    char *kw_list[] = {"a", "b", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OO:a_f3_p",
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       return(NULL);

    a_f3(_la, _lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f4(int (*b)(int *a), PFDouble a)| */

static PyObject *_PY_a_f4(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int (*_lb)(int *a);
    PFDouble _la;
    char *kw_list[] = {"b", "a", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OO:a_f4_p",
                                     kw_list,
                                     &_lb, &_la);
    if (ok == FALSE)
       return(NULL);

    a_f4(_lb, _la);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f1(PFDouble a, int b)| */

static PyObject *_PY_B_f1(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    PFDouble _la;
    int _lb;
    char *kw_list[] = {"a", "b", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "Oi:b_f1_p",
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       return(NULL);

    B_f1(_la, _lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f2(int (*a)(int *a), int b)| */

static PyObject *_PY_B_f2(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int (*_la)(int *a);
    int _lb;
    char *kw_list[] = {"a", "b", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "Oi:b_f2_p",
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       return(NULL);

    B_f2(_la, _lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f3(PFDouble a, int (*b)(int *a), int c)| */

static PyObject *_PY_B_f3(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    PFDouble _la;
    int (*_lb)(int *a);
    int _lc;
    char *kw_list[] = {"a", "b", "c", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OOi:b_f3_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    B_f3(_la, _lb, _lc);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f4(int (*a)(int *a), PFDouble b, int c)| */

static PyObject *_PY_B_f4(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int (*_la)(int *a);
    PFDouble _lb;
    int _lc;
    char *kw_list[] = {"a", "b", "c", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OOi:b_f4_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    B_f4(_la, _lb, _lc);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f5(int a, PFDouble b, int (*c)(int *a))| */

static PyObject *_PY_B_f5(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    PFDouble _lb;
    int (*_lc)(int *a);
    char *kw_list[] = {"a", "b", "c", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iOO:b_f5_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    B_f5(_la, _lb, _lc);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void B_f6(int a, int (*b)(int *a), PFDouble c)| */

static PyObject *_PY_B_f6(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int (*_lb)(int *a);
    PFDouble _lc;
    char *kw_list[] = {"a", "b", "c", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iOO:b_f6_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    B_f6(_la, _lb, _lc);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyMethodDef
 _PYD_a_f1 = {"a_f1", (PyCFunction) _PY_a_f1, METH_KEYWORDS,
              "Procedure: a_f1\n     Usage: a_f1(a)"},
 _PYD_a_f2 = {"a_f2", (PyCFunction) _PY_a_f2, METH_KEYWORDS,
              "Procedure: a_f2\n     Usage: a_f2(b)"},
 _PYD_a_f3 = {"a_f3", (PyCFunction) _PY_a_f3, METH_KEYWORDS,
              "Procedure: a_f3\n     Usage: a_f3(a, b)"},
 _PYD_a_f4 = {"a_f4", (PyCFunction) _PY_a_f4, METH_KEYWORDS,
              "Procedure: a_f4\n     Usage: a_f4(b, a)"},
 _PYD_B_f1 = {"b_f1", (PyCFunction) _PY_B_f1, METH_KEYWORDS,
              "Procedure: b_f1\n     Usage: b_f1(a, b)"},
 _PYD_B_f2 = {"b_f2", (PyCFunction) _PY_B_f2, METH_KEYWORDS,
              "Procedure: b_f2\n     Usage: b_f2(a, b)"},
 _PYD_B_f3 = {"b_f3", (PyCFunction) _PY_B_f3, METH_KEYWORDS,
              "Procedure: b_f3\n     Usage: b_f3(a, b, c)"},
 _PYD_B_f4 = {"b_f4", (PyCFunction) _PY_B_f4, METH_KEYWORDS,
              "Procedure: b_f4\n     Usage: b_f4(a, b, c)"},
 _PYD_B_f5 = {"b_f5", (PyCFunction) _PY_B_f5, METH_KEYWORDS,
              "Procedure: b_f5\n     Usage: b_f5(a, b, c)"},
 _PYD_B_f6 = {"b_f6", (PyCFunction) _PY_B_f6, METH_KEYWORDS,
              "Procedure: b_f6\n     Usage: b_f6(a, b, c)"},
 _PYD_bl5_null;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
