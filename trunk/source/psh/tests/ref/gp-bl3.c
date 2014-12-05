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

/* local variable initializations */
    _la1       = NULL;
    _la2       = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "Oi:fe1p",
                                     kw_list,
                                     &_la1, &_la2);
    if (ok == FALSE)
       return(NULL);

    _rv = fe1(_la1, _la2);
    _lo = PY_build_object("fe1",
                          G_STR_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fe2(str *dev, double *x, double *y, int n, int l)| */

PyObject *_PY_fe2(PY_str *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_ldev;
    double *_lx;
    double *_ly;
    int _ln;
    int _ll;
    char *kw_list[] = {"dev", "x", "y", "n", "l", NULL};

/* local variable initializations */
    _ldev      = NULL;
    _lx        = NULL;
    _ly        = NULL;
    _ln        = 0;
    _ll        = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "OOOii:fe2p",
                                     kw_list,
                                     &_ldev, &_lx, &_ly, &_ln, &_ll);
    if (ok == FALSE)
       return(NULL);

    fe2(_ldev, _lx, _ly, _ln, _ll);

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

    register_bl3_types();

    nerr = 0;

    PY_str_type.tp_new   = PyType_GenericNew;
    PY_str_type.tp_alloc = PyType_GenericAlloc;
    nerr += (PyType_Ready(&PY_str_type) < 0);
    nerr += (PyDict_SetItemString(d, "str", (PyObject *) &PY_str_type) < 0);

    return(nerr);}

/*--------------------------------------------------------------------------*/

/*                     STR ROUTINES               */

/*--------------------------------------------------------------------------*/

PY_DEF_EXTRACTOR(str);

/*--------------------------------------------------------------------------*/
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

static PyObject *PY_str_get_n(PY_str *self, void *context)
   {PyObject *rv;

    rv = PY_build_object("n",
                         SC_LONG_I, 0, &self->pyo->n,
                         0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_str_get_s(PY_str *self, void *context)
   {PyObject *rv;

    rv = PY_build_object("s",
                         SC_STRING_I, 0, &self->pyo->s,
                         0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_str_set_n(PY_str *self,
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
           {self->pyo->n = lv;
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
 PY_str_doc_n[] = "",
 PY_str_doc_s[] = "",
 PY_str_doc[] = "";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyGetSetDef PY_str_getset[] = {
    {"str", (getter) PY_str_get, NULL, PY_str_doc, NULL},
    {"n", (getter) PY_str_get_n, (setter) PY_str_set_n, PY_str_doc_n, NULL},
    {"s", (getter) PY_str_get_s, (setter) PY_str_set_s, PY_str_doc_s, NULL},
#ifdef PY_EXT_GETSET_STR
    PY_EXT_GETSET_STR
#endif
    {NULL}};

PyTypeObject PY_str_type;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
