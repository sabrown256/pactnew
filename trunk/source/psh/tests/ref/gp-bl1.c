/*
 * GP-BL1.C - generated support routines for BL1
 *
 */


#include "cpyright.h"
#include "py_int.h"
#include "bl1_int.h"
#include "gp-bl1.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fav1(void)| */

PyObject *_PY_fav1(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    
/* local variable initializations */

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
    
/* local variable initializations */

    _rv = fav2();
    _lo = PY_build_object("fav2",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double fav3(void)| */

PyObject *_PY_fav3(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    double _rv;
    
/* local variable initializations */

    _rv = fav3();
    _lo = PY_build_object("fav3",
                          SC_DOUBLE_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str fav4(void)| */

PyObject *_PY_fav4(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    str _rv;
    
/* local variable initializations */

    _rv = fav4();
    _lo = PY_build_object("fav4",
                          G_STR_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void *favp1(void)| */

PyObject *_PY_favp1(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    void *_rv;
    
/* local variable initializations */

    _rv = favp1();
    _lo = PY_build_object("favp1",
                          SC_VOID_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |int *favp2(void)| */

PyObject *_PY_favp2(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    int *_rv;
    
/* local variable initializations */

    _rv = favp2();
    _lo = PY_build_object("favp2",
                          SC_INT_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |double *favp3(void)| */

PyObject *_PY_favp3(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    double *_rv;
    
/* local variable initializations */

    _rv = favp3();
    _lo = PY_build_object("favp3",
                          SC_DOUBLE_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |str *favp4(void)| */

PyObject *_PY_favp4(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    str *_rv;
    
/* local variable initializations */

    _rv = favp4();
    _lo = PY_build_object("favp4",
                          G_STR_I, 0, &_rv,
                          0);

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* WRAP |void fav5(void)| */

PyObject *_PY_fav5(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *_lo;
    
/* local variable initializations */

    fav5();

    Py_INCREF(Py_None);
    _lo = Py_None;

    return(_lo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_add_bl1_enum(PyObject *m)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_init_bl1(PyObject *m, PyObject *d)
   {int nerr;

    register_bl1_types();

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
