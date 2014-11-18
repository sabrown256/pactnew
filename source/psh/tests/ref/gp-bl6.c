/*
 * GP-BL6.C - generated support routines for BL6
 *
 */


#include "cpyright.h"
#include "py_int.h"
#include "bl6_int.h"
#include "gp-bl6.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa1(int a ARG(2,in))| */

PyObject *_PY_fa1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int _rv;
    char *kw_list[] = {"a", NULL};

/* local variable initializations */
    _la        = 2;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fa1p",
                                     kw_list,
                                     &_la);
    if (ok == FALSE)
       return(NULL);

    _rv = fa1(_la);
    _lo = Py_BuildValue("i",
                        _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa3(int *a ARG([1,2,3],in), int *b ARG([1,2,3],io), int *c ARG([1,2,3],out))| */

PyObject *_PY_fa3(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la[3];
    int _lb[3];
    int _lc[3];
    int _rv;
    char *kw_list[] = {"a0", "a1", "a2", "b0", "b1", "b2", NULL};

/* local variable initializations */
    _la[0] = 1;
    _la[1] = 2;
    _la[2] = 3;
    _lb[0] = 1;
    _lb[1] = 2;
    _lb[2] = 3;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iiiiii:fa3p",
                                     kw_list,
                                     &_la[0], &_la[1], &_la[2], &_lb[0], &_lb[1], &_lb[2]);
    if (ok == FALSE)
       return(NULL);

    _rv = fa3(_la, _lb, _lc);
    _lo = Py_BuildValue("iiiiiii",
                        _rv, _lb[0], _lb[1], _lb[2], _lc[0], _lc[1], _lc[2]);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa4(int *a ARG(,in), int *b ARG(,io), int *c ARG(,out))| */

PyObject *_PY_fa4(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int *_la;
    int _lb;
    int *_lc;
    int _rv;
    char *kw_list[] = {"a", "b", NULL};

/* local variable initializations */
    _la        = NULL;
    _lb        = 0;
    _lc        = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "ii:fa4p",
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       return(NULL);

    _rv = fa4(_la, &_lb, _lc);
    _lo = Py_BuildValue("iii",
                        _rv, _lb, _lc);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb1(int a, int b ARG(1,in))| */

PyObject *_PY_fb1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int _lb;
    char *kw_list[] = {"a", "b", NULL};

/* local variable initializations */
    _la        = 0;
    _lb        = 1;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "ii:fb1_p",
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       return(NULL);

    fb1(_la, _lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb2(int a, int b ARG(,in))| */

PyObject *_PY_fb2(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int _lb;
    char *kw_list[] = {"a", "b", NULL};

/* local variable initializations */
    _la        = 0;
    _lb        = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "ii:fb2_p",
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       return(NULL);

    fb2(_la, _lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fb3(int a, int b ARG(*,in))| */

PyObject *_PY_fb3(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int _lb;
    char *kw_list[] = {"a", "b", NULL};

/* local variable initializations */
    _la        = 0;
    _lb        = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "ii:fb3_p",
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       return(NULL);

    fb3(_la, _lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc1(int a, int *b ARG(NULL,in), int *c ARG(NULL,io), int *d ARG(NULL,out))| */

PyObject *_PY_fc1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int *_lb;
    int _lc;
    int *_ld;
    char *kw_list[] = {"a", "b", "c", NULL};

/* local variable initializations */
    _la        = 0;
    _lb        = NULL;
    _lc        = 0;
    _ld        = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iii:fc1_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    fc1(_la, _lb, &_lc, _ld);
    _lo = Py_BuildValue("ii",
                        _lc, _ld);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc2(int a, int *b ARG(,in), int *c ARG(,io), int *d ARG(,out))| */

PyObject *_PY_fc2(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int *_lb;
    int _lc;
    int *_ld;
    char *kw_list[] = {"a", "b", "c", NULL};

/* local variable initializations */
    _la        = 0;
    _lb        = NULL;
    _lc        = 0;
    _ld        = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iii:fc2_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    fc2(_la, _lb, &_lc, _ld);
    _lo = Py_BuildValue("ii",
                        _lc, _ld);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc3(int a, int *b ARG(*,in), int *c ARG(*,io), int *d ARG(*,out))| */

PyObject *_PY_fc3(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int *_lb;
    int _lc;
    int *_ld;
    char *kw_list[] = {"a", "b", "c", NULL};

/* local variable initializations */
    _la        = 0;
    _lb        = NULL;
    _lc        = 0;
    _ld        = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iii:fc3_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    fc3(_la, _lb, &_lc, _ld);
    _lo = Py_BuildValue("ii",
                        _lc, _ld);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc4(int a, int *b ARG([1],in), int *c ARG([1],io), int *d ARG([1],out))| */

PyObject *_PY_fc4(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int _lb;
    int _lc;
    int _ld;
    char *kw_list[] = {"a", "b", "c", NULL};

/* local variable initializations */
    _la        = 0;
    _lb        = 1;
    _lc        = 0;
    _ld        = 1;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iii:fc4_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    fc4(_la, &_lb, &_lc, &_ld);
    _lo = Py_BuildValue("ii",
                        _lc, _ld);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc5(int a, int *b ARG([1,2],in), int *c ARG([1,2],io), int *d ARG([1,2],out))| */

PyObject *_PY_fc5(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int _lb[2];
    int _lc[2];
    int _ld[2];
    char *kw_list[] = {"a", "b0", "b1", "c0", "c1", NULL};

/* local variable initializations */
    _la        = 0;
    _lb[0] = 1;
    _lb[1] = 2;
    _lc[0] = 1;
    _lc[1] = 2;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iiiii:fc5_p",
                                     kw_list,
                                     &_la, &_lb[0], &_lb[1], &_lc[0], &_lc[1]);
    if (ok == FALSE)
       return(NULL);

    fc5(_la, _lb, _lc, _ld);
    _lo = Py_BuildValue("iiii",
                        _lc[0], _lc[1], _ld[0], _ld[1]);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc6(int a, int *b ARG([*],in), int *c ARG([*],io), int *d ARG([*],out))| */

PyObject *_PY_fc6(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int _lb;
    int _lc;
    int _ld;
    char *kw_list[] = {"a", "b", "c", NULL};

/* local variable initializations */
    _la        = 0;
    _lb        = 0;
    _lc        = 0;
    _ld        = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iii:fc6_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    fc6(_la, &_lb, &_lc, &_ld);
    _lo = Py_BuildValue("ii",
                        _lc, _ld);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc7(int a, int *b ARG([*,*],in), int *c ARG([*,*],io), int *d ARG([*,*],out))| */

PyObject *_PY_fc7(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int _lb[2];
    int _lc[2];
    int _ld[2];
    char *kw_list[] = {"a", "b0", "b1", "c0", "c1", NULL};

/* local variable initializations */
    _la        = 0;
    _lb[0] = 0;
    _lb[1] = 0;
    _lc[0] = 0;
    _lc[1] = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iiiii:fc7_p",
                                     kw_list,
                                     &_la, &_lb[0], &_lb[1], &_lc[0], &_lc[1]);
    if (ok == FALSE)
       return(NULL);

    fc7(_la, _lb, _lc, _ld);
    _lo = Py_BuildValue("iiii",
                        _lc[0], _lc[1], _ld[0], _ld[1]);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc8(char *s1 ARG("abc",in), char **s2 ARG(["def"],in), char *s3 ARG([*],in), char **s4 ARG([*,*],in))| */

PyObject *_PY_fc8(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    char *_ls1;
    char *_ls2[1];
    char *_ls3;
    char *_ls4[2];
    char *kw_list[] = {"s1", "s20", "s3", "s40", "s41", NULL};

/* local variable initializations */
    _ls1       = "abc";
    _ls2[0] = "def";
    _ls3       = '\0';
    _ls4[0] = NULL;
    _ls4[1] = NULL;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "sssss:fc8_p",
                                     kw_list,
                                     &_ls1, &_ls2[0], &_ls3, &_ls4[0], &_ls4[1]);
    if (ok == FALSE)
       return(NULL);

    fc8(_ls1, _ls2, _ls3, _ls4);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc9(char c1, char c2 ARG('a',in), char *c3 ARG(['b'],in), char *c4 ARG([*],in), char *c5 ARG([*,*],in))| */

PyObject *_PY_fc9(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    char _lc1;
    char _lc2;
    char *_lc3;
    char *_lc4;
    char _lc5[2];
    char *kw_list[] = {"c1", "c2", "c3", "c4", "c50", "c51", NULL};

/* local variable initializations */
    _lc1       = '\0';
    _lc2       = 'a';
    _lc3       = 'b';
    _lc4       = '\0';
    _lc5[0] = '\0';
    _lc5[1] = '\0';

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "ssssss:fc9_p",
                                     kw_list,
                                     &_lc1, &_lc2, &_lc3, &_lc4, &_lc5[0], &_lc5[1]);
    if (ok == FALSE)
       return(NULL);

    fc9(_lc1, _lc2, _lc3, _lc4, _lc5);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fd1(str *a ARG(,,cls), int b)| */

PyObject *_PY_fd1(PY_str *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_la;
    int _lb;
    char *kw_list[] = {"b", NULL};

    _la = self->pyo;

/* local variable initializations */
    _lb        = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fd1_p",
                                     kw_list,
                                     &_lb);
    if (ok == FALSE)
       return(NULL);

    fd1(_la, _lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fd2(str *a ARG(,,cls), int b)| */

PyObject *_PY_fd2(PY_str *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_la;
    int _lb;
    char *kw_list[] = {"b", NULL};

    _la = self->pyo;

/* local variable initializations */
    _lb        = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:nfd",
                                     kw_list,
                                     &_lb);
    if (ok == FALSE)
       return(NULL);

    fd2(_la, _lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void PD_fd3(str *a ARG(,,cls), int b)| */

PyObject *_PY_PD_fd3(PY_str *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_la;
    int _lb;
    char *kw_list[] = {"b", NULL};

    _la = self->pyo;

/* local variable initializations */
    _lb        = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fd3",
                                     kw_list,
                                     &_lb);
    if (ok == FALSE)
       return(NULL);

    PD_fd3(_la, _lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void PD_fd4(str *a ARG(,,cls), int b)| */

PyObject *_PY_PD_fd4(PY_str *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_la;
    int _lb;
    char *kw_list[] = {"b", NULL};

    _la = self->pyo;

/* local variable initializations */
    _lb        = 0;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:mfd",
                                     kw_list,
                                     &_lb);
    if (ok == FALSE)
       return(NULL);

    PD_fd4(_la, _lb);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_add_bl6_enum(PyObject *m)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_init_bl6(PyObject *m, PyObject *d)
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

    rv = PY_build_object("nc",
                         SC_INT_I, 0, &self->pyo->nc,
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
