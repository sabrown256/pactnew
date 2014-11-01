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
    _lo = Py_BuildValue("O",
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
    _lo = Py_BuildValue("O",
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

int PY_add_bl1_enum(PyObject *m)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PY_init_bl1(PyObject *m, PyObject *d)
   {int nerr;

    nerr = 0;

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
