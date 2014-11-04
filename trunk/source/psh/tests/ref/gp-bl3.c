/*
 * GP-BL3.C - generated support routines for BL3
 *
 */


#include "cpyright.h"
#include "py_int.h"
#include "bl3_int.h"
#include "gp-bl3.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *fe1(str *a1, int a2)| */

PyObject *_PY_fe1(PY_str *self, PyObject *args, PyObject *kwds)
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
       {PyErr_SetString(PP_error_user, "fe1");
        return(NULL);};

    _rv = fe1(_la1, _la2);
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fe2(str *dev, double *x, double *y, int n, pcons *info, int l)| */

PyObject *_PY_fe2(PY_str *self, PyObject *args, PyObject *kwds)
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
       {PyErr_SetString(PP_error_user, "fe2");
        return(NULL);};

    fe2(_ldev, _lx, _ly, _ln, _linfo, _ll);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_add_bl3_enum(PyObject *m)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_init_bl3(PyObject *m, PyObject *d)
   {int nerr;

    nerr = 0;

    PY_str_type.tp_new   = PyType_GenericNew;
    PY_str_type.tp_alloc = PyType_GenericAlloc;
    nerr += (PyType_Ready(&PY_str_type) < 0);
    nerr += (PyDict_SetItemString(d, "str", (PyObject *) &PY_str_type) < 0);

    return(nerr);}

/*--------------------------------------------------------------------------*/

/*                     STR ROUTINES               */

/*--------------------------------------------------------------------------*/

int PY_str_check(PyObject *op)
   {int rv;

    rv = PyObject_TypeCheck(op, &PY_str_type);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *PY_str_from_ptr(str *x)
   {PY_str *self;
    PyObject *rv;

    rv = NULL;

    self = PyObject_NEW(PY_str, &PY_str_type);
    if (self != NULL)
       {self->pyo = x;
        rv = (PyObject *) self;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_str_get(PY_str *self, void *context)
   {PyObject *rv;

    rv = PY_str_from_ptr(self->pyo);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_str_get_nc(PY_str *self, void *context)
   {PyObject *rv;

    rv = PY_INT_LONG(self->pyo->nc);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_str_get_s(PY_str *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->pyo->s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_str_set_nc(PY_str *self,
                   PyObject *value, void *context)
   {int rv;

    rv = -1;

    if (value == NULL)
       PyErr_SetString(PyExc_TypeError,
                       "attribute deletion is not supported");

    else
       {int ok;
        long lv;

        ok = PyArg_Parse(value, "l", &lv);
        if (ok == TRUE)
           {self->pyo->nc = lv;
            rv = 0;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_str_set_s(PY_str *self,
                   PyObject *value, void *context)
   {int rv;

    rv = -1;

    if (value == NULL)
       PyErr_SetString(PyExc_TypeError,
                       "attribute deletion is not supported");

    else
       {int ok;

        ok = PyArg_Parse(value, "s", &self->pyo->s);
        if (ok == TRUE)
           rv = 0;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_str_tp_init(PY_str *self, PyObject *args, PyObject *kwds)
   {int rv;

    rv = -1;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char
 PY_str_doc_nc[] = "",
 PY_str_doc_s[] = "",
 PY_str_doc[] = "";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyGetSetDef PY_str_getset[] = {
    {"str", (getter) PY_str_get, NULL, PY_str_doc, NULL},
    {"nc", (getter) PY_str_get_nc, (setter) PY_str_set_nc, PY_str_doc_nc, NULL},
    {"s", (getter) PY_str_get_s, (setter) PY_str_set_s, PY_str_doc_s, NULL},
#ifdef PY_EXT_GETSET_STR
    PY_EXT_GETSET_STR
#endif
    {NULL}};

PyTypeObject PY_str_type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
