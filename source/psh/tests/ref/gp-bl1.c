
#include <Python.h>
#include "bl1_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fav1(void)| */

static char _PYI_fav1__doc__[] = "Procedure: fav1\n     Usage: fav1()";

static PyObject *_PYI_fav1(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {PyObject *_lo;

       {fav1();

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int fav2(void)| */

static char _PYI_fav2__doc__[] = "Procedure: fav2\n     Usage: fav2()";

static PyObject *_PYI_fav2(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {int _rv;
    PyObject *_lo;

       {_rv = fav2();

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double fav3(void)| */

static char _PYI_fav3__doc__[] = "Procedure: fav3\n     Usage: fav3()";

static PyObject *_PYI_fav3(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {double _rv;
    PyObject *_lo;

       {_rv = fav3();

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str fav4(void)| */

static char _PYI_fav4__doc__[] = "Procedure: fav4\n     Usage: fav4()";

static PyObject *_PYI_fav4(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {str _rv;
    PyObject *_lo;

       {_rv = fav4();

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void *favp1(void)| */

static char _PYI_favp1__doc__[] = "Procedure: favp1\n     Usage: favp1()";

static PyObject *_PYI_favp1(PyObject *self,
                            PyObject *args,
                            PyObject *kwds)
   {void *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

       {_rv = favp1();

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int *favp2(void)| */

static char _PYI_favp2__doc__[] = "Procedure: favp2\n     Usage: favp2()";

static PyObject *_PYI_favp2(PyObject *self,
                            PyObject *args,
                            PyObject *kwds)
   {int *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

       {_rv = favp2();

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double *favp3(void)| */

static char _PYI_favp3__doc__[] = "Procedure: favp3\n     Usage: favp3()";

static PyObject *_PYI_favp3(PyObject *self,
                            PyObject *args,
                            PyObject *kwds)
   {double *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

       {_rv = favp3();

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *favp4(void)| */

static char _PYI_favp4__doc__[] = "Procedure: favp4\n     Usage: favp4()";

static PyObject *_PYI_favp4(PyObject *self,
                            PyObject *args,
                            PyObject *kwds)
   {str *_rv;
    long _sz;
    C_array *_arr;
    PyObject *_lo;

       {_rv = favp4();

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fav5(void)| */

static char _PYI_fav5__doc__[] = "Procedure: fav5\n     Usage: fav5()";

static PyObject *_PYI_fav5(PyObject *self,
                           PyObject *args,
                           PyObject *kwds)
   {PyObject *_lo;

       {fav5();

        Py_INCREF(Py_None);
        _lo = Py_None;};

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
