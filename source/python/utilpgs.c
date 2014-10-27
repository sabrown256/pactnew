/*
 * UTILPGS.C
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pgsmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_BUFFER_EXTRACTOR - unsigned byte array */

int PP_buffer_extractor(PyObject *obj, void *arg)
{
    int ret = 1;

#ifdef HAVE_PY_NUMERIC

    PyArrayObject *arr;
    
    if (obj == Py_None) {
        *(void **) arg = NULL;
    } else {
        /* create an array object */
        arr = (PyArrayObject *) PyArray_ContiguousFromObject(obj, 'd', 0, 0);
        if (arr == NULL)
            ret = 0;
        else
            *(void **) arg = (void *) arr->data;
    }
    
#endif

    return ret;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PALETTE_EXTRACTOR - extract a palette from an object */

int palette_extractor(PyObject *obj, void *arg)
{
    int ret = 1;

    if (obj == Py_None) {
        *(void **) arg = NULL;
    } else if (PY_PG_image_Check(obj)) {
        PY_PG_palette *self;
        self = (PY_PG_palette *) obj;
        *(PG_palette **) arg = self->pyo;
    } else {
        ret = 0;
    }

    return ret;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

