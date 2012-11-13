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
#include "pdbmodule.h"

/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.C_definition) */

/*--------------------------------------------------------------------------*/

static char PP_mesh_topology_mt__doc__[] =
""
;

static PyObject *
PP_mesh_topology_mt_get(PP_mesh_topologyObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.descriptor.mt_get) UNMODIFIED */
    return PPtopology_from_ptr(self->mt);
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.descriptor.mt_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mesh_topology_mt_n_dimensions__doc__[] =
""
;

static PyObject *
PP_mesh_topology_mt_n_dimensions_get(PP_mesh_topologyObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.descriptor.mt.n_dimensions_get) UNMODIFIED */
    return PY_INT_LONG(self->mt->n_dimensions);
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.descriptor.mt.n_dimensions_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mesh_topology_mt_n_bound_params__doc__[] =
""
;

static PyObject *
PP_mesh_topology_mt_n_bound_params_get(PP_mesh_topologyObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.descriptor.mt.n_bound_params_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->mt->n_bound_params, NULL);
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.descriptor.mt.n_bound_params_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mesh_topology_mt_n_cells__doc__[] =
""
;

static PyObject *
PP_mesh_topology_mt_n_cells_get(PP_mesh_topologyObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.descriptor.mt.n_cells_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->mt->n_cells, NULL);
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.descriptor.mt.n_cells_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mesh_topology_mt_boundaries__doc__[] =
""
;

static PyObject *
PP_mesh_topology_mt_boundaries_get(PP_mesh_topologyObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.descriptor.mt.boundaries_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->mt->boundaries, NULL);
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.descriptor.mt.boundaries_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.extra_members) */

static PyGetSetDef PP_mesh_topology_getset[] = {
    {"mt", (getter) PP_mesh_topology_mt_get, NULL, PP_mesh_topology_mt__doc__, NULL},
    {"n_dimensions", (getter) PP_mesh_topology_mt_n_dimensions_get, NULL, PP_mesh_topology_mt_n_dimensions__doc__, NULL},
    {"n_bound_params", (getter) PP_mesh_topology_mt_n_bound_params_get, NULL, PP_mesh_topology_mt_n_bound_params__doc__, NULL},
    {"n_cells", (getter) PP_mesh_topology_mt_n_cells_get, NULL, PP_mesh_topology_mt_n_cells__doc__, NULL},
    {"boundaries", (getter) PP_mesh_topology_mt_boundaries_get, NULL, PP_mesh_topology_mt_boundaries__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static int
PP_mesh_topology_tp_init(PP_mesh_topologyObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.as_type.init) */
    int nd;
    int *bnp;
    int *bnc;
    long **bnd;
    char *kw_list[] = {"nd", "bnp", "bnc", "bnd", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iO&O&O&:make_topology", kw_list,
                                     &nd, iarray_extractor, &bnp, iarray_extractor, &bnc, pplong_extractor, &bnd))
        return -1;
    self->mt = PM_make_topology(nd, bnp, bnc, bnd);
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_mesh_topology_Check - */

/* static */ int
PP_mesh_topology_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_mesh_topology_Type))
        return 1;
    else
        return 0;
/*  return PY_TYPE(op) == &PP_mesh_topology_Type; */
}

/*--------------------------------------------------------------------------*/


static char PP_mesh_topology_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_mesh_topology_Type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        0,                              /* ob_size */
        "mesh_topology",                       /* tp_name */
        sizeof(PP_mesh_topologyObject),         /* tp_basicsize */
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
        PP_mesh_topology_Type__doc__,   /* tp_doc */
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
        PP_mesh_topology_getset,        /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_mesh_topology_tp_init, /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.mesh_topology.extra) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mesh_topology.extra) */
/* End of code for mesh_topology objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
