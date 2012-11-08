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








/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *PPgraph_from_ptr(PG_graph *data)
{
    PP_graphObject *self;

    self = PyObject_NEW(PP_graphObject, &PP_graph_Type);
    if (self == NULL)
        return NULL;
    self->data = data;

    return (PyObject *) self;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* palette_extractor - extract a palette from an object */

int palette_extractor(PyObject *obj, void *arg)
{
    int ret = 1;

    if (obj == Py_None) {
        *(void **) arg = NULL;
    } else if (PP_image_Check(obj)) {
        PP_paletteObject *self;
        self = (PP_paletteObject *) obj;
        *(PG_palette **) arg = self->pal;
    } else {
        ret = 0;
    }

    return ret;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *PPpalette_from_ptr(PG_palette *pal)
{
    PP_paletteObject *self;

    self = PyObject_NEW(PP_paletteObject, &PP_palette_Type);
    if (self == NULL)
        return NULL;
    self->pal = pal;

    return (PyObject *) self;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *PPimage_from_ptr(PG_image *im)
{
    PP_imageObject *self;

    self = PyObject_NEW(PP_imageObject, &PP_image_Type);
    if (self == NULL)
        return NULL;
    self->im = im;

    return (PyObject *) self;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

