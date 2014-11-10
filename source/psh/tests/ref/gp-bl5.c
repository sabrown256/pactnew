/*
 * GP-BL5.C - generated support routines for BL5
 *
 */


#include "cpyright.h"
#include "py_int.h"
#include "bl5_int.h"
#include "gp-bl5.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void a_f1(PFDouble a)| */

PyObject *_PY_a_f1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    PFDouble _la;
    char *kw_list[] = {"a", NULL};

/* local variable initializations */
    _la        = NULL;

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

PyObject *_PY_a_f2(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int (*_lb)(int *a);
    char *kw_list[] = {"b", NULL};

/* local variable initializations */
    _lb        = NULL;

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

PyObject *_PY_a_f3(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    PFDouble _la;
    int (*_lb)(int *a);
    char *kw_list[] = {"a", "b", NULL};

/* local variable initializations */
    _la        = NULL;
    _lb        = NULL;

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

PyObject *_PY_a_f4(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int (*_lb)(int *a);
    PFDouble _la;
    char *kw_list[] = {"b", "a", NULL};

/* local variable initializations */
    _lb        = NULL;
    _la        = NULL;

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

PyObject *_PY_B_f1(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    PFDouble _la;
    int _lb;
    char *kw_list[] = {"a", "b", NULL};

/* local variable initializations */
    _la        = NULL;
    _lb        = 0;

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

PyObject *_PY_B_f2(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int (*_la)(int *a);
    int _lb;
    char *kw_list[] = {"a", "b", NULL};

/* local variable initializations */
    _la        = NULL;
    _lb        = 0;

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

PyObject *_PY_B_f3(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    PFDouble _la;
    int (*_lb)(int *a);
    int _lc;
    char *kw_list[] = {"a", "b", "c", NULL};

/* local variable initializations */
    _la        = NULL;
    _lb        = NULL;
    _lc        = 0;

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

PyObject *_PY_B_f4(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int (*_la)(int *a);
    PFDouble _lb;
    int _lc;
    char *kw_list[] = {"a", "b", "c", NULL};

/* local variable initializations */
    _la        = NULL;
    _lb        = NULL;
    _lc        = 0;

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

PyObject *_PY_B_f5(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    PFDouble _lb;
    int (*_lc)(int *a);
    char *kw_list[] = {"a", "b", "c", NULL};

/* local variable initializations */
    _la        = 0;
    _lb        = NULL;
    _lc        = NULL;

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

PyObject *_PY_B_f6(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok;
    PyObject *_lo;
    int _la;
    int (*_lb)(int *a);
    PFDouble _lc;
    char *kw_list[] = {"a", "b", "c", NULL};

/* local variable initializations */
    _la        = 0;
    _lb        = NULL;
    _lc        = NULL;

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

int PY_add_bl5_enum(PyObject *m)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_init_bl5(PyObject *m, PyObject *d)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
