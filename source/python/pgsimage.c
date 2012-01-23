/*--------------------------------------------------------------------------*/

/* This is generated code.
 * Any edits must be made between the splicer.begin and splicer.end blocks.
 * All other edits will be lost.
 * Once a block is edited remove the 'UNMODIFIED' on the splicer.begin comment
 * to allow the block to be preserved when it is regenerated.
 */
/*
 * Source Version: 9.0
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */
/*--------------------------------------------------------------------------*/
#include "pgsmodule.h"

/* DO-NOT-DELETE splicer.begin(pgs.image.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.image.C_definition) */

/*--------------------------------------------------------------------------*/

static char PP_image_im__doc__[] =
""
;

static PyObject *
PP_image_im_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im_get) UNMODIFIED */
    return PPimage_from_ptr(self->im);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_version_id__doc__[] =
""
;

static PyObject *
PP_image_im_version_id_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.version_id_get) UNMODIFIED */
    return PyInt_FromLong((long) self->im->version_id);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.version_id_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_label__doc__[] =
""
;

static PyObject *
PP_image_im_label_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.label_get) UNMODIFIED */
    return Py_BuildValue("s", self->im->label);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.label_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_xmin__doc__[] =
""
;

static PyObject *
PP_image_im_xmin_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.xmin_get) UNMODIFIED */
    return PyFloat_FromDouble(self->im->xmin);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.xmin_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_xmax__doc__[] =
""
;

static PyObject *
PP_image_im_xmax_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.xmax_get) UNMODIFIED */
    return PyFloat_FromDouble(self->im->xmax);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.xmax_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_ymin__doc__[] =
""
;

static PyObject *
PP_image_im_ymin_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.ymin_get) UNMODIFIED */
    return PyFloat_FromDouble(self->im->ymin);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.ymin_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_ymax__doc__[] =
""
;

static PyObject *
PP_image_im_ymax_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.ymax_get) UNMODIFIED */
    return PyFloat_FromDouble(self->im->ymax);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.ymax_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_zmin__doc__[] =
""
;

static PyObject *
PP_image_im_zmin_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.zmin_get) UNMODIFIED */
    return PyFloat_FromDouble(self->im->zmin);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.zmin_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_zmax__doc__[] =
""
;

static PyObject *
PP_image_im_zmax_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.zmax_get) UNMODIFIED */
    return PyFloat_FromDouble(self->im->zmax);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.zmax_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_element_type__doc__[] =
""
;

static PyObject *
PP_image_im_element_type_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.element_type_get) UNMODIFIED */
    return Py_BuildValue("s", self->im->element_type);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.element_type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_buffer__doc__[] =
""
;

static PyObject *
PP_image_im_buffer_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.buffer_get) */
    PyObject *rv;

#ifdef HAVE_PYTHON_NUMERIC
    int dims[2];
    PG_image *im;

    im = self->im;

    dims[0] = im->kmax;
    dims[1] = im->lmax;

    rv = PyArray_FromDimsAndData(2, dims, 'b', (char *) im->buffer);
#else
    rv = NULL;
#endif

    return rv;
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.buffer_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_kmax__doc__[] =
""
;

static PyObject *
PP_image_im_kmax_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.kmax_get) UNMODIFIED */
    return PyInt_FromLong((long) self->im->kmax);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.kmax_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_lmax__doc__[] =
""
;

static PyObject *
PP_image_im_lmax_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.lmax_get) UNMODIFIED */
    return PyInt_FromLong((long) self->im->lmax);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.lmax_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_size__doc__[] =
""
;

static PyObject *
PP_image_im_size_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.size_get) UNMODIFIED */
    return PyInt_FromLong(self->im->size);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.size_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_bits_pixel__doc__[] =
""
;

static PyObject *
PP_image_im_bits_pixel_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.bits_pixel_get) UNMODIFIED */
    return PyInt_FromLong((long) self->im->bits_pixel);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.bits_pixel_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_image_im_palette__doc__[] =
""
;

static PyObject *
PP_image_im_palette_get(PP_imageObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.descriptor.im.palette_get) UNMODIFIED */
    return PPpalette_from_ptr(self->im->palette);
/* DO-NOT-DELETE splicer.end(pgs.image.descriptor.im.palette_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pgs.image.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.image.extra_members) */

static PyGetSetDef PP_image_getset[] = {
    {"im", (getter) PP_image_im_get, NULL, PP_image_im__doc__, NULL},
    {"version_id", (getter) PP_image_im_version_id_get, NULL, PP_image_im_version_id__doc__, NULL},
    {"label", (getter) PP_image_im_label_get, NULL, PP_image_im_label__doc__, NULL},
    {"xmin", (getter) PP_image_im_xmin_get, NULL, PP_image_im_xmin__doc__, NULL},
    {"xmax", (getter) PP_image_im_xmax_get, NULL, PP_image_im_xmax__doc__, NULL},
    {"ymin", (getter) PP_image_im_ymin_get, NULL, PP_image_im_ymin__doc__, NULL},
    {"ymax", (getter) PP_image_im_ymax_get, NULL, PP_image_im_ymax__doc__, NULL},
    {"zmin", (getter) PP_image_im_zmin_get, NULL, PP_image_im_zmin__doc__, NULL},
    {"zmax", (getter) PP_image_im_zmax_get, NULL, PP_image_im_zmax__doc__, NULL},
    {"element_type", (getter) PP_image_im_element_type_get, NULL, PP_image_im_element_type__doc__, NULL},
    {"buffer", (getter) PP_image_im_buffer_get, NULL, PP_image_im_buffer__doc__, NULL},
    {"kmax", (getter) PP_image_im_kmax_get, NULL, PP_image_im_kmax__doc__, NULL},
    {"lmax", (getter) PP_image_im_lmax_get, NULL, PP_image_im_lmax__doc__, NULL},
    {"size", (getter) PP_image_im_size_get, NULL, PP_image_im_size__doc__, NULL},
    {"bits_pixel", (getter) PP_image_im_bits_pixel_get, NULL, PP_image_im_bits_pixel__doc__, NULL},
    {"palette", (getter) PP_image_im_palette_get, NULL, PP_image_im_palette__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pgs.image.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.image.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static void
PP_image_tp_dealloc(PP_imageObject *self)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.as_type.dealloc) */
    PG_rl_image(self->im);
    self->ob_type->tp_free((PyObject*)self);
/* DO-NOT-DELETE splicer.end(pgs.image.as_type.dealloc) */
}

static int
PP_image_tp_init(PP_imageObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.image.as_type.init) */
    return 0;
/* DO-NOT-DELETE splicer.end(pgs.image.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_image_Check - */

/* static */ int
PP_image_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_image_Type))
        return 1;
    else
        return 0;
/*  return op->ob_type == &PP_image_Type; */
}

/*--------------------------------------------------------------------------*/


static char PP_image_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_image_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /* ob_size */
        "image",                       /* tp_name */
        sizeof(PP_imageObject),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PP_image_tp_dealloc, /* tp_dealloc */
        (printfunc)0,                   /* tp_print */
        (getattrfunc)0,                 /* tp_getattr */
        (setattrfunc)0,                 /* tp_setattr */
        (cmpfunc)0,                     /* tp_compare */
        (reprfunc)0,                    /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        0,                              /* tp_as_sequence */
        0,                              /* tp_as_mapping */
        /* More standard operations (here for binary compatibility) */
        (hashfunc)0,                    /* tp_hash */
        (ternaryfunc)0,                 /* tp_call */
        (reprfunc)0,                    /* tp_str */
        (getattrofunc)0,                /* tp_getattro */
        (setattrofunc)0,                /* tp_setattro */
        /* Functions to access object as input/output buffer */
        0,                              /* tp_as_buffer */
        /* Flags to define presence of optional/expanded features */
        Py_TPFLAGS_DEFAULT,             /* tp_flags */
        PP_image_Type__doc__,           /* tp_doc */
        /* Assigned meaning in release 2.0 */
        /* call function for all accessible objects */
        (traverseproc)0,                /* tp_traverse */
        /* delete references to contained objects */
        (inquiry)0,                     /* tp_clear */
        /* Assigned meaning in release 2.1 */
        /* rich comparisons */
        (richcmpfunc)0,                 /* tp_richcompare */
        /* weak reference enabler */
        0,                              /* tp_weaklistoffset */
        /* Added in release 2.2 */
        /* Iterators */
        (getiterfunc)0,                 /* tp_iter */
        (iternextfunc)0,                /* tp_iternext */
        /* Attribute descriptor and subclassing stuff */
        0,                              /* tp_methods */
        0,                              /* tp_members */
        PP_image_getset,                /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_image_tp_init,     /* tp_init */
        (allocfunc)0,                   /* tp_alloc */
        (newfunc)0,                     /* tp_new */
#if PYTHON_API_VERSION >= 1012
        (freefunc)0,                    /* tp_free */
#else
        (destructor)0,                  /* tp_free */
#endif
        (inquiry)0,                     /* tp_is_gc */
        0,                              /* tp_bases */
        0,                              /* tp_mro */
        0,                              /* tp_cache */
        0,                              /* tp_subclasses */
        0,                              /* tp_weaklist */
#if PYTHON_API_VERSION >= 1012
        (destructor)0,                  /* tp_del */
#endif
};

/* DO-NOT-DELETE splicer.begin(pgs.image.extra) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.image.extra) */
/* End of code for image objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
