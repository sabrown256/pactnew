
#include <Python.h>
#include "bl6_int.h"
#include "sx_int.h"

typedef struct s_strO *strOp;
typedef struct s_strO strO;

struct s_strO
   {PyObject_HEAD
    str *pobj;};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fa1(int a ARG(2,in))| */

PyObject *_PY_fa1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int _rv;
    char *kw_list[] = {"a", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "i:fa1p",
                                     kw_list,
                                     &_la);
    if (ok == FALSE)
       return(NULL);

    _rv = fa1(_la);
    _lo = Py_BuildValue("i",
                        &_rv);

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

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iiiiii:fa3p",
                                     kw_list,
                                     &_la[0], &_la[1], &_la[2], &_lb[0], &_lb[1], &_lb[2]);
    if (ok == FALSE)
       return(NULL);

    _rv = fa3(_la, _lb, _lc);
    _lo = Py_BuildValue("iiiiiii",
                        &_rv, &_lb[0], &_lb[1], &_lb[2], &_lc[0], &_lc[1], &_lc[2]);

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

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "ii:fa4p",
                                     kw_list,
                                     &_la, &_lb);
    if (ok == FALSE)
       return(NULL);

    _rv = fa4(_la, &_lb, _lc);
    _lo = Py_BuildValue("iii",
                        &_rv, &_lb, &_lc);

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

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iii:fc1_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    fc1(_la, _lb, &_lc, _ld);
    _lo = Py_BuildValue("ii",
                        &_lc, &_ld);

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

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iii:fc2_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    fc2(_la, _lb, &_lc, _ld);
    _lo = Py_BuildValue("ii",
                        &_lc, &_ld);

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

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iii:fc3_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    fc3(_la, _lb, &_lc, _ld);
    _lo = Py_BuildValue("ii",
                        &_lc, &_ld);

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

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iii:fc4_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    fc4(_la, &_lb, &_lc, &_ld);
    _lo = Py_BuildValue("ii",
                        &_lc, &_ld);

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

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iiiii:fc5_p",
                                     kw_list,
                                     &_la, &_lb[0], &_lb[1], &_lc[0], &_lc[1]);
    if (ok == FALSE)
       return(NULL);

    fc5(_la, _lb, _lc, _ld);
    _lo = Py_BuildValue("iiii",
                        &_lc[0], &_lc[1], &_ld[0], &_ld[1]);

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

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iii:fc6_p",
                                     kw_list,
                                     &_la, &_lb, &_lc);
    if (ok == FALSE)
       return(NULL);

    fc6(_la, &_lb, &_lc, &_ld);
    _lo = Py_BuildValue("ii",
                        &_lc, &_ld);

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

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "iiiii:fc7_p",
                                     kw_list,
                                     &_la, &_lb[0], &_lb[1], &_lc[0], &_lc[1]);
    if (ok == FALSE)
       return(NULL);

    fc7(_la, _lb, _lc, _ld);
    _lo = Py_BuildValue("iiii",
                        &_lc[0], &_lc[1], &_ld[0], &_ld[1]);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fc8(char *s1 ARG("abc",in), char **s2 ARG(["def"],in), char *s3 ARG([*],in), char **s4 ARG([*,*],in))| */

PyObject *_PY_fc8(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    char _ls1;
    char *_ls2;
    char _ls3;
    char *_ls4[2];
    char *kw_list[] = {"s1", "s2", "s3", "s40", "s41", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "sssss:fc8_p",
                                     kw_list,
                                     &_ls1, &_ls2, &_ls3, &_ls4[0], &_ls4[1]);
    if (ok == FALSE)
       return(NULL);

    fc8(&_ls1, &_ls2, &_ls3, _ls4);

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
    char _lc3;
    char _lc4;
    char _lc5[2];
    char *kw_list[] = {"c1", "c2", "c3", "c4", "c50", "c51", NULL};

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
                                     "bbssss:fc9_p",
                                     kw_list,
                                     &_lc1, &_lc2, &_lc3, &_lc4, &_lc5[0], &_lc5[1]);
    if (ok == FALSE)
       return(NULL);

    fc9(_lc1, _lc2, &_lc3, &_lc4, _lc5);

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fd1(str *a ARG(,,cls), int b)| */

PyObject *_PY_fd1(strO *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_la;
    int _lb;
    char *kw_list[] = {"b", NULL};

    _la = self->pobj;

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

PyObject *_PY_fd2(strO *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_la;
    int _lb;
    char *kw_list[] = {"b", NULL};

    _la = self->pobj;

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

PyObject *_PY_PD_fd3(strO *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_la;
    int _lb;
    char *kw_list[] = {"b", NULL};

    _la = self->pobj;

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

PyObject *_PY_PD_fd4(strO *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    str *_la;
    int _lb;
    char *kw_list[] = {"b", NULL};

    _la = self->pobj;

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

PyMethodDef
 _PYD_fa1 = {"fa1p", (PyCFunction) _PY_fa1, METH_KEYWORDS,
             "Procedure: fa1p\n     Usage: fa1p(a)"},
 _PYD_fa3 = {"fa3p", (PyCFunction) _PY_fa3, METH_KEYWORDS,
             "Procedure: fa3p\n     Usage: fa3p(a, b, c)"},
 _PYD_fa4 = {"fa4p", (PyCFunction) _PY_fa4, METH_KEYWORDS,
             "Procedure: fa4p\n     Usage: fa4p(a, b, c)"},
 _PYD_fb1 = {"fb1", (PyCFunction) _PY_fb1, METH_KEYWORDS,
             "Procedure: fb1\n     Usage: fb1(a, b)"},
 _PYD_fb2 = {"fb2", (PyCFunction) _PY_fb2, METH_KEYWORDS,
             "Procedure: fb2\n     Usage: fb2(a, b)"},
 _PYD_fb3 = {"fb3", (PyCFunction) _PY_fb3, METH_KEYWORDS,
             "Procedure: fb3\n     Usage: fb3(a, b)"},
 _PYD_fc1 = {"fc1", (PyCFunction) _PY_fc1, METH_KEYWORDS,
             "Procedure: fc1\n     Usage: fc1(a, b, c, d)"},
 _PYD_fc2 = {"fc2", (PyCFunction) _PY_fc2, METH_KEYWORDS,
             "Procedure: fc2\n     Usage: fc2(a, b, c, d)"},
 _PYD_fc3 = {"fc3", (PyCFunction) _PY_fc3, METH_KEYWORDS,
             "Procedure: fc3\n     Usage: fc3(a, b, c, d)"},
 _PYD_fc4 = {"fc4", (PyCFunction) _PY_fc4, METH_KEYWORDS,
             "Procedure: fc4\n     Usage: fc4(a, b, c, d)"},
 _PYD_fc5 = {"fc5", (PyCFunction) _PY_fc5, METH_KEYWORDS,
             "Procedure: fc5\n     Usage: fc5(a, b, c, d)"},
 _PYD_fc6 = {"fc6", (PyCFunction) _PY_fc6, METH_KEYWORDS,
             "Procedure: fc6\n     Usage: fc6(a, b, c, d)"},
 _PYD_fc7 = {"fc7", (PyCFunction) _PY_fc7, METH_KEYWORDS,
             "Procedure: fc7\n     Usage: fc7(a, b, c, d)"},
 _PYD_fc8 = {"fc8", (PyCFunction) _PY_fc8, METH_KEYWORDS,
             "Procedure: fc8\n     Usage: fc8(s1, s2, s3, s4)"},
 _PYD_fc9 = {"fc9", (PyCFunction) _PY_fc9, METH_KEYWORDS,
             "Procedure: fc9\n     Usage: fc9(c1, c2, c3, c4, c5)"},
 _PYD_fd1 = {"fd1", (PyCFunction) _PY_fd1, METH_KEYWORDS,
             "Method: fd1\n     Usage: a.fd1(b)"},
 _PYD_fd2 = {"nfd", (PyCFunction) _PY_fd2, METH_KEYWORDS,
             "Method: nfd\n     Usage: a.nfd(b)"},
 _PYD_PD_fd3 = {"fd3", (PyCFunction) _PY_PD_fd3, METH_KEYWORDS,
                "Method: fd3\n     Usage: a.fd3(b)"},
 _PYD_PD_fd4 = {"mfd", (PyCFunction) _PY_PD_fd4, METH_KEYWORDS,
                "Method: mfd\n     Usage: a.mfd(b)"},
 _PYD_bl6_null;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
