/*
 * GP-BL2.C - generated support routines for BL2
 *
 */


#include "cpyright.h"
#include "py_int.h"
#include "bl2_int.h"
#include "gp-bl2.h"

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
                                     "ifdsOO:fbcv1p",
                                     kw_list,
                                     &_la, &_lb, &_lc, &_ld, &_le, &_lf);
    if (ok == FALSE)
       return(NULL);

    _rv = fbcv1(_la, _lb, _lc, _ld, _le, _lf);
    _lo = Py_BuildValue("s",
                        _rv);

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
                                     "ifds:fbmcv1p",
                                     kw_list,
                                     &_la, &_lb, &_lc, &_ld);
    if (ok == FALSE)
       return(NULL);

    _rv = fbmcv1(_la, _lb, _lc, _ld);
    _lo = Py_BuildValue("s",
                        _rv);

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
    _lo = Py_BuildValue("s",
                        _rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_add_bl2_enum(PyObject *m)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_init_bl2(PyObject *m, PyObject *d)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
