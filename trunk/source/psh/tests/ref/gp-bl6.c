
#include <Python.h>
#include "bl6_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa1(int a ARG(2,in))| */

static char _PYI_fa1__doc__[] = "Procedure: fa1\n     Usage: fa1(a)";

static PyObject *_PYI_fa1(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     i,
                                     kw_list,
                                     &_la);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fa1(_la);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa3(int *a ARG([1,2,3],in), int *b ARG([1,2,3],io), int *c ARG([1,2,3],out))| */

static char _PYI_fa3__doc__[] = "Procedure: fa3\n     Usage: fa3(a, b, c)";

static PyObject *_PYI_fa3(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la[3];
    int _lb[3];
    int _lc[3];
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     iiiiii,
                                     kw_list,
                                     &_la[0], &_la[1], &_la[2], &_lb[0], &_lb[1], &_lb[2]);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fa3(_la, _lb, _lc);
        _lo = Py_BuildValue("iiiiii",
                            &_lb[0], &_lb[1], &_lb[2], &_lc[0], &_lc[1], &_lc[2]);};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa4(int *a ARG(,in), int *b ARG(,io), int *c ARG(,out))| */

static char _PYI_fa4__doc__[] = "Procedure: fa4\n     Usage: fa4(a, b, c)";

static PyObject *_PYI_fa4(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int *_la;
    int _lb;
    int *_lc;
    int _rv;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     ii,
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       _lo = NULL;
    else
       {_rv = fa4(_la, &_lb, _lc);
        _lo = Py_BuildValue("ii",
                            &_lb, &_lc);};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb1(int a, int b ARG(1,in))| */

static char _PYI_fb1__doc__[] = "Procedure: fb1\n     Usage: fb1(a, b)";

static PyObject *_PYI_fb1(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int _lb;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     ii,
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fb1(_la, _lb);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb2(int a, int b ARG(,in))| */

static char _PYI_fb2__doc__[] = "Procedure: fb2\n     Usage: fb2(a, b)";

static PyObject *_PYI_fb2(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int _lb;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     ii,
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fb2(_la, _lb);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb3(int a, int b ARG(*,in))| */

static char _PYI_fb3__doc__[] = "Procedure: fb3\n     Usage: fb3(a, b)";

static PyObject *_PYI_fb3(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int _lb;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     ii,
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fb3(_la, _lb);

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc1(int a, int *b ARG(NULL,in), int *c ARG(NULL,io), int *d ARG(NULL,out))| */

static char _PYI_fc1__doc__[] = "Procedure: fc1\n     Usage: fc1(a, b, c, d)";

static PyObject *_PYI_fc1(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int *_lb;
    int _lc;
    int *_ld;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     iii,
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fc1(_la, _lb, &_lc, _ld);
        _lo = Py_BuildValue("ii",
                            &_lc, &_ld);};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc2(int a, int *b ARG(,in), int *c ARG(,io), int *d ARG(,out))| */

static char _PYI_fc2__doc__[] = "Procedure: fc2\n     Usage: fc2(a, b, c, d)";

static PyObject *_PYI_fc2(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int *_lb;
    int _lc;
    int *_ld;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     iii,
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fc2(_la, _lb, &_lc, _ld);
        _lo = Py_BuildValue("ii",
                            &_lc, &_ld);};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc3(int a, int *b ARG(*,in), int *c ARG(*,io), int *d ARG(*,out))| */

static char _PYI_fc3__doc__[] = "Procedure: fc3\n     Usage: fc3(a, b, c, d)";

static PyObject *_PYI_fc3(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int *_lb;
    int _lc;
    int *_ld;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     iii,
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fc3(_la, _lb, &_lc, _ld);
        _lo = Py_BuildValue("ii",
                            &_lc, &_ld);};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc4(int a, int *b ARG([1],in), int *c ARG([1],io), int *d ARG([1],out))| */

static char _PYI_fc4__doc__[] = "Procedure: fc4\n     Usage: fc4(a, b, c, d)";

static PyObject *_PYI_fc4(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int _lb;
    int _lc;
    int *_ld;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     iii,
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fc4(_la, &_lb, &_lc, _ld);
        _lo = Py_BuildValue("ii",
                            &_lc, &_ld);};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc5(int a, int *b ARG([1,2],in), int *c ARG([1,2],io), int *d ARG([1,2],out))| */

static char _PYI_fc5__doc__[] = "Procedure: fc5\n     Usage: fc5(a, b, c, d)";

static PyObject *_PYI_fc5(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int _lb[2];
    int _lc[2];
    int _ld[2];
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     iiiii,
                                     kw_list,
                                     &_la, &_lb[0], &_lb[1], &_lc[0], &_lc[1]);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fc5(_la, _lb, _lc, _ld);
        _lo = Py_BuildValue("iiii",
                            &_lc[0], &_lc[1], &_ld[0], &_ld[1]);};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc6(int a, int *b ARG([*],in), int *c ARG([*],io), int *d ARG([*],out))| */

static char _PYI_fc6__doc__[] = "Procedure: fc6\n     Usage: fc6(a, b, c, d)";

static PyObject *_PYI_fc6(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int _lb;
    int _lc;
    int *_ld;
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     iii,
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fc6(_la, &_lb, &_lc, _ld);
        _lo = Py_BuildValue("ii",
                            &_lc, &_ld);};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc7(int a, int *b ARG([*,*],in), int *c ARG([*,*],io), int *d ARG([*,*],out))| */

static char _PYI_fc7__doc__[] = "Procedure: fc7\n     Usage: fc7(a, b, c, d)";

static PyObject *_PYI_fc7(PyObject *self,
                          PyObject *args,
                          PyObject *kwds)
   {int _la;
    int _lb[2];
    int _lc[2];
    int _ld[2];
    PyObject *_lo;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     iiiii,
                                     kw_list,
                                     &_la, &_lb[0], &_lb[1], &_lc[0], &_lc[1]);
    if (ok == FALSE)
       _lo = NULL;
    else
       {fc7(_la, _lb, _lc, _ld);
        _lo = Py_BuildValue("iiii",
                            &_lc[0], &_lc[1], &_ld[0], &_ld[1]);};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
