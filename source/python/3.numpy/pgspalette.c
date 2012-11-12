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

/* DO-NOT-DELETE splicer.begin(pgs.palette.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.palette.C_definition) */

/*--------------------------------------------------------------------------*/

static char PP_palette_pal__doc__[] =
""
;

static PyObject *
PP_palette_pal_get(PP_paletteObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.palette.descriptor.pal_get) UNMODIFIED */
    return PPpalette_from_ptr(self->pal);
/* DO-NOT-DELETE splicer.end(pgs.palette.descriptor.pal_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_palette_pal_name__doc__[] =
""
;

static PyObject *
PP_palette_pal_name_get(PP_paletteObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.palette.descriptor.pal.name_get) UNMODIFIED */
    return Py_BuildValue("s", self->pal->name);
/* DO-NOT-DELETE splicer.end(pgs.palette.descriptor.pal.name_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_palette_pal_max_pal_dims__doc__[] =
""
;

static PyObject *
PP_palette_pal_max_pal_dims_get(PP_paletteObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.palette.descriptor.pal.max_pal_dims_get) UNMODIFIED */
    return PyInt_FromLong((long) self->pal->max_pal_dims);
/* DO-NOT-DELETE splicer.end(pgs.palette.descriptor.pal.max_pal_dims_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_palette_pal_n_pal_colors__doc__[] =
""
;

static PyObject *
PP_palette_pal_n_pal_colors_get(PP_paletteObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.palette.descriptor.pal.n_pal_colors_get) UNMODIFIED */
    return PyInt_FromLong((long) self->pal->n_pal_colors);
/* DO-NOT-DELETE splicer.end(pgs.palette.descriptor.pal.n_pal_colors_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_palette_pal_n_dev_colors__doc__[] =
""
;

static PyObject *
PP_palette_pal_n_dev_colors_get(PP_paletteObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.palette.descriptor.pal.n_dev_colors_get) UNMODIFIED */
    return PyInt_FromLong((long) self->pal->n_dev_colors);
/* DO-NOT-DELETE splicer.end(pgs.palette.descriptor.pal.n_dev_colors_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_palette_pal_max_red_intensity__doc__[] =
""
;

static PyObject *
PP_palette_pal_max_red_intensity_get(PP_paletteObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.palette.descriptor.pal.max_red_intensity_get) UNMODIFIED */
    return PyFloat_FromDouble((double) self->pal->max_red_intensity);
/* DO-NOT-DELETE splicer.end(pgs.palette.descriptor.pal.max_red_intensity_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_palette_pal_max_green_intensity__doc__[] =
""
;

static PyObject *
PP_palette_pal_max_green_intensity_get(PP_paletteObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.palette.descriptor.pal.max_green_intensity_get) UNMODIFIED */
    return PyFloat_FromDouble((double) self->pal->max_green_intensity);
/* DO-NOT-DELETE splicer.end(pgs.palette.descriptor.pal.max_green_intensity_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_palette_pal_max_blue_intensity__doc__[] =
""
;

static PyObject *
PP_palette_pal_max_blue_intensity_get(PP_paletteObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.palette.descriptor.pal.max_blue_intensity_get) UNMODIFIED */
    return PyFloat_FromDouble((double) self->pal->max_blue_intensity);
/* DO-NOT-DELETE splicer.end(pgs.palette.descriptor.pal.max_blue_intensity_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_palette_pal_next__doc__[] =
""
;

static PyObject *
PP_palette_pal_next_get(PP_paletteObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.palette.descriptor.pal.next_get) UNMODIFIED */
    return PPpalette_from_ptr(self->pal->next);
/* DO-NOT-DELETE splicer.end(pgs.palette.descriptor.pal.next_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pgs.palette.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.palette.extra_members) */

static PyGetSetDef PP_palette_getset[] = {
    {"pal", (getter) PP_palette_pal_get, NULL, PP_palette_pal__doc__, NULL},
    {"name", (getter) PP_palette_pal_name_get, NULL, PP_palette_pal_name__doc__, NULL},
    {"max_pal_dims", (getter) PP_palette_pal_max_pal_dims_get, NULL, PP_palette_pal_max_pal_dims__doc__, NULL},
    {"n_pal_colors", (getter) PP_palette_pal_n_pal_colors_get, NULL, PP_palette_pal_n_pal_colors__doc__, NULL},
    {"n_dev_colors", (getter) PP_palette_pal_n_dev_colors_get, NULL, PP_palette_pal_n_dev_colors__doc__, NULL},
    {"max_red_intensity", (getter) PP_palette_pal_max_red_intensity_get, NULL, PP_palette_pal_max_red_intensity__doc__, NULL},
    {"max_green_intensity", (getter) PP_palette_pal_max_green_intensity_get, NULL, PP_palette_pal_max_green_intensity__doc__, NULL},
    {"max_blue_intensity", (getter) PP_palette_pal_max_blue_intensity_get, NULL, PP_palette_pal_max_blue_intensity__doc__, NULL},
    {"next", (getter) PP_palette_pal_next_get, NULL, PP_palette_pal_next__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pgs.palette.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.palette.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static int
PP_palette_tp_init(PP_paletteObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.palette.as_type.init) */
    return 0;
/* DO-NOT-DELETE splicer.end(pgs.palette.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_palette_Check - */

/* static */ int
PP_palette_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_palette_Type))
        return 1;
    else
        return 0;
/*  return op->ob_type == &PP_palette_Type; */
}

/*--------------------------------------------------------------------------*/


static char PP_palette_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_palette_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        0,                              /* ob_size */
        "palette",                       /* tp_name */
        sizeof(PP_paletteObject),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)0,                  /* tp_dealloc */
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
        PP_palette_Type__doc__,         /* tp_doc */
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
        PP_palette_getset,              /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_palette_tp_init,   /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pgs.palette.extra) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.palette.extra) */
/* End of code for palette objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
