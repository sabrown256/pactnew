
#include <Python.h>
#include "bl1_int.h"
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fav1(void)| */

PyObject *_PY_fav1(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    
    fav1();

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fav2(void)| */

PyObject *_PY_fav2(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    int _rv;
    
    _rv = fav2();
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double fav3(void)| */

PyObject *_PY_fav3(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    double _rv;
    
    _rv = fav3();
    _lo = Py_BuildValue("d",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str fav4(void)| */

PyObject *_PY_fav4(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    str _rv;
    
    _rv = fav4();
    _lo = Py_BuildValue("s",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void *favp1(void)| */

PyObject *_PY_favp1(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    void *_rv;
    
    _rv = favp1();
    _lo = Py_BuildValue("O",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int *favp2(void)| */

PyObject *_PY_favp2(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    int *_rv;
    
    _rv = favp2();
    _lo = Py_BuildValue("i",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double *favp3(void)| */

PyObject *_PY_favp3(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    double *_rv;
    
    _rv = favp3();
    _lo = Py_BuildValue("d",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *favp4(void)| */

PyObject *_PY_favp4(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    str *_rv;
    
    _rv = favp4();
    _lo = Py_BuildValue("s",
                        &_rv);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fav5(void)| */

PyObject *_PY_fav5(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    
    fav5();

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyMethodDef
 _PYD_fav1 = {"fav1p", (PyCFunction) _PY_fav1, METH_KEYWORDS,
              "Procedure: fav1p\n     Usage: fav1p()"},
 _PYD_fav2 = {"fav2p", (PyCFunction) _PY_fav2, METH_KEYWORDS,
              "Procedure: fav2p\n     Usage: fav2p()"},
 _PYD_fav3 = {"fav3p", (PyCFunction) _PY_fav3, METH_KEYWORDS,
              "Procedure: fav3p\n     Usage: fav3p()"},
 _PYD_fav4 = {"fav4p", (PyCFunction) _PY_fav4, METH_KEYWORDS,
              "Procedure: fav4p\n     Usage: fav4p()"},
 _PYD_favp1 = {"favp1p", (PyCFunction) _PY_favp1, METH_KEYWORDS,
               "Procedure: favp1p\n     Usage: favp1p()"},
 _PYD_favp2 = {"favp2p", (PyCFunction) _PY_favp2, METH_KEYWORDS,
               "Procedure: favp2p\n     Usage: favp2p()"},
 _PYD_favp3 = {"favp3p", (PyCFunction) _PY_favp3, METH_KEYWORDS,
               "Procedure: favp3p\n     Usage: favp3p()"},
 _PYD_favp4 = {"favp4p", (PyCFunction) _PY_favp4, METH_KEYWORDS,
               "Procedure: favp4p\n     Usage: favp4p()"},
 _PYD_fav5 = {"fav5", (PyCFunction) _PY_fav5, METH_KEYWORDS,
              "Procedure: fav5\n     Usage: fav5()"},
 _PYD_bl1_null;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
