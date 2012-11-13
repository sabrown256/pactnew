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

/* DO-NOT-DELETE splicer.begin(pgs.graph.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.graph.C_definition) */

/*--------------------------------------------------------------------------*/

static char PP_graph_data__doc__[] =
""
;

static PyObject *
PP_graph_data_get(PP_graphObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.graph.descriptor.data_get) UNMODIFIED */
    return PPgraph_from_ptr(self->data);
/* DO-NOT-DELETE splicer.end(pgs.graph.descriptor.data_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_graph_data_rendering__doc__[] =
""
;

static PyObject *
PP_graph_data_rendering_get(PP_graphObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.graph.descriptor.data.rendering_get) UNMODIFIED */
    return PY_INT_LONG(self->data->rendering);
/* DO-NOT-DELETE splicer.end(pgs.graph.descriptor.data.rendering_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_graph_data_mesh__doc__[] =
""
;

static PyObject *
PP_graph_data_mesh_get(PP_graphObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.graph.descriptor.data.mesh_get) UNMODIFIED */
    return PY_INT_LONG(self->data->mesh);
/* DO-NOT-DELETE splicer.end(pgs.graph.descriptor.data.mesh_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_graph_data_info_type__doc__[] =
""
;

static PyObject *
PP_graph_data_info_type_get(PP_graphObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.graph.descriptor.data.info_type_get) UNMODIFIED */
    return Py_BuildValue("s", self->data->info_type);
/* DO-NOT-DELETE splicer.end(pgs.graph.descriptor.data.info_type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_graph_data_info__doc__[] =
""
;

static PyObject *
PP_graph_data_info_get(PP_graphObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.graph.descriptor.data.info_get) UNMODIFIED */
    return PP_assoc_from_ptr(self->data->info);
/* DO-NOT-DELETE splicer.end(pgs.graph.descriptor.data.info_get) */
}

static int
PP_graph_data_info_set(PP_graphObject *self, PyObject *value, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.graph.descriptor.data.info_set) */
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "attribute deletion is not supported");
        return -1;
    }
    if (PP_assoc_extractor(value, &self->data->info) == 0)
        return -1;
    SC_mark(self->data->info, 1);  /* ADDED */
    return 0;
/* DO-NOT-DELETE splicer.end(pgs.graph.descriptor.data.info_set) */
}

/*--------------------------------------------------------------------------*/

static char PP_graph_data_identifier__doc__[] =
""
;

static PyObject *
PP_graph_data_identifier_get(PP_graphObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.graph.descriptor.data.identifier_get) UNMODIFIED */
    return PY_INT_LONG(self->data->identifier);
/* DO-NOT-DELETE splicer.end(pgs.graph.descriptor.data.identifier_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_graph_data_use__doc__[] =
""
;

static PyObject *
PP_graph_data_use_get(PP_graphObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.graph.descriptor.data.use_get) UNMODIFIED */
    return Py_BuildValue("s", self->data->use);
/* DO-NOT-DELETE splicer.end(pgs.graph.descriptor.data.use_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pgs.graph.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.graph.extra_members) */

static PyGetSetDef PP_graph_getset[] = {
    {"data", (getter) PP_graph_data_get, NULL, PP_graph_data__doc__, NULL},
    {"rendering", (getter) PP_graph_data_rendering_get, NULL, PP_graph_data_rendering__doc__, NULL},
    {"mesh", (getter) PP_graph_data_mesh_get, NULL, PP_graph_data_mesh__doc__, NULL},
    {"info_type", (getter) PP_graph_data_info_type_get, NULL, PP_graph_data_info_type__doc__, NULL},
    {"info", (getter) PP_graph_data_info_get, (setter) PP_graph_data_info_set, PP_graph_data_info__doc__, NULL},
    {"identifier", (getter) PP_graph_data_identifier_get, NULL, PP_graph_data_identifier__doc__, NULL},
    {"use", (getter) PP_graph_data_use_get, NULL, PP_graph_data_use__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pgs.graph.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.graph.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static void
PP_graph_tp_dealloc(PP_graphObject *self)
{
/* DO-NOT-DELETE splicer.begin(pgs.graph.as_type.dealloc) */
    PG_rl_graph(self->data, TRUE, TRUE);
    PY_TYPE(self)->tp_free((PyObject*)self);
/* DO-NOT-DELETE splicer.end(pgs.graph.as_type.dealloc) */
}

static int
PP_graph_tp_init(PP_graphObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.graph.as_type.init) */
    return 0;
/* DO-NOT-DELETE splicer.end(pgs.graph.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_graph_Check - */

/* static */ int
PP_graph_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_graph_Type))
        return 1;
    else
        return 0;
/*  return PY_TYPE(op) == &PP_graph_Type; */
}

/*--------------------------------------------------------------------------*/


static char PP_graph_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_graph_Type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        "graph",                       /* tp_name */
        sizeof(PP_graphObject),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PP_graph_tp_dealloc, /* tp_dealloc */
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
        PP_graph_Type__doc__,           /* tp_doc */
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
        PP_graph_getset,                /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_graph_tp_init,     /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pgs.graph.extra) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.graph.extra) */
/* End of code for graph objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
