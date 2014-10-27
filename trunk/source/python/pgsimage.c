/*
 * Source Version: 9.0
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */

#define PY_DEF_DESTRUCTOR	    PY_PG_image_tp_dealloc

#include "pgsmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_PG_image_buffer_get(PY_PG_image *self, void *context)
   {PyObject *rv;

#ifdef HAVE_PY_NUMERIC
    int dims[2];
    PG_image *im;

    im = self->pyo;

    dims[0] = im->kmax;
    dims[1] = im->lmax;

    rv = PyArray_FromDimsAndData(2, dims, 'b', (char *) im->buffer);
#else
    rv = NULL;
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0

/* this has the correct image buffer routine */

static PyGetSetDef PY_PG_image_getset[] = {
    {"im", (getter) PY_PG_image_get, NULL, PY_PG_image_doc, NULL},
    {"version_id", (getter) PY_PG_image_version_id_get, NULL, PY_PG_image_version_id_doc, NULL},
    {"label", (getter) PY_PG_image_label_get, NULL, PY_PG_image_label_doc, NULL},
    {"xmin", (getter) PY_PG_image_xmin_get, NULL, PY_PG_image_xmin_doc, NULL},
    {"xmax", (getter) PY_PG_image_xmax_get, NULL, PY_PG_image_xmax_doc, NULL},
    {"ymin", (getter) PY_PG_image_ymin_get, NULL, PY_PG_image_ymin_doc, NULL},
    {"ymax", (getter) PY_PG_image_ymax_get, NULL, PY_PG_image_ymax_doc, NULL},
    {"zmin", (getter) PY_PG_image_zmin_get, NULL, PY_PG_image_zmin_doc, NULL},
    {"zmax", (getter) PY_PG_image_zmax_get, NULL, PY_PG_image_zmax_doc, NULL},
    {"element_type", (getter) PY_PG_image_element_type_get, NULL, PY_PG_image_element_type_doc, NULL},
    {"buffer", (getter) PY_PG_image_buffer_get, NULL, PY_PG_image_buffer_doc, NULL},
    {"kmax", (getter) PY_PG_image_kmax_get, NULL, PY_PG_image_kmax_doc, NULL},
    {"lmax", (getter) PY_PG_image_lmax_get, NULL, PY_PG_image_lmax_doc, NULL},
    {"size", (getter) PY_PG_image_size_get, NULL, PY_PG_image_size_doc, NULL},
    {"bits_pixel", (getter) PY_PG_image_bits_pixel_get, NULL, PY_PG_image_bits_pixel_doc, NULL},
    {"palette", (getter) PY_PG_image_palette_get, NULL, PY_PG_image_palette_doc, NULL},
    {NULL}     /* Sentinel */
};

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void PY_PG_image_tp_dealloc(PY_PG_image *self)
   {

    PG_rl_image(self->pyo);
    PY_TYPE(self)->tp_free((PyObject*)self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_PG_image_tp_init(PY_PG_image *self,
			       PyObject *args, PyObject *kwds)
   {

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_image_doc[] = "";

PY_DEF_TYPE(PG_image);

#if 0

/* static */
PyTypeObject PY_PG_image_type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        "image",                       /* tp_name */
        sizeof(PY_PG_image),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PY_PG_image_tp_dealloc, /* tp_dealloc */
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
        PY_PG_image_type_doc,           /* tp_doc */
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
        PY_PG_image_getset,                /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PY_PG_image_tp_init,     /* tp_init */
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

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
