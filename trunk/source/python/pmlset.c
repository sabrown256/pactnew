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

/* DO-NOT-DELETE splicer.begin(pdb.set.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.set.C_definition) */

/*--------------------------------------------------------------------------*/

static char PP_set_set__doc__[] =
""
;

static PyObject *
PP_set_set_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set_get) UNMODIFIED */
    return _PY_set_from_ptr(self->set);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_name__doc__[] =
""
;

static PyObject *
PP_set_set_name_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.name_get) UNMODIFIED */
    return Py_BuildValue("s", self->set->name);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.name_get) */
}

static int
PP_set_set_name_set(PP_setObject *self, PyObject *value, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.name_set) UNMODIFIED */
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "attribute deletion is not supported");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &self->set->name))
        return -1;
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.name_set) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_element_type__doc__[] =
""
;

static PyObject *
PP_set_set_element_type_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.element_type_get) UNMODIFIED */
    return Py_BuildValue("s", self->set->element_type);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.element_type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_dimension__doc__[] =
""
;

static PyObject *
PP_set_set_dimension_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.dimension_get) UNMODIFIED */
    return PY_INT_LONG(self->set->dimension);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.dimension_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_max_index__doc__[] =
""
;

static PyObject *
PP_set_set_max_index_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.max_index_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->set->max_index, NULL);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.max_index_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_dimension_elem__doc__[] =
""
;

static PyObject *
PP_set_set_dimension_elem_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.dimension_elem_get) UNMODIFIED */
    return PY_INT_LONG(self->set->dimension_elem);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.dimension_elem_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_n_elements__doc__[] =
""
;

static PyObject *
PP_set_set_n_elements_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.n_elements_get) UNMODIFIED */
    return PY_INT_LONG(self->set->n_elements);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.n_elements_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_elements__doc__[] =
""
;

static PyObject *
PP_set_set_elements_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.elements_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->set->elements, NULL);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.elements_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_es_type__doc__[] =
""
;

static PyObject *
PP_set_set_es_type_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.es_type_get) UNMODIFIED */
    return Py_BuildValue("s", self->set->es_type);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.es_type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_extrema__doc__[] =
""
;

static PyObject *
PP_set_set_extrema_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.extrema_get) UNMODIFIED */
    return Py_BuildValue("s", self->set->extrema);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.extrema_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_scales__doc__[] =
""
;

static PyObject *
PP_set_set_scales_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.scales_get) UNMODIFIED */
    return Py_BuildValue("s", self->set->scales);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.scales_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_opers__doc__[] =
""
;

static PyObject *
PP_set_set_opers_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.opers_get) UNMODIFIED */
    return PPfield_from_ptr(self->set->opers);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.opers_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_metric__doc__[] =
""
;

static PyObject *
PP_set_set_metric_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.metric_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->set->metric, NULL);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.metric_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_symmetry_type__doc__[] =
""
;

static PyObject *
PP_set_set_symmetry_type_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.symmetry_type_get) UNMODIFIED */
    return Py_BuildValue("s", self->set->symmetry_type);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.symmetry_type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_symmetry__doc__[] =
""
;

static PyObject *
PP_set_set_symmetry_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.symmetry_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->set->symmetry, NULL);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.symmetry_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_topology_type__doc__[] =
""
;

static PyObject *
PP_set_set_topology_type_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.topology_type_get) UNMODIFIED */
    return Py_BuildValue("s", self->set->topology_type);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.topology_type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_topology__doc__[] =
""
;

static PyObject *
PP_set_set_topology_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.topology_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->set->topology, NULL);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.topology_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_info_type__doc__[] =
""
;

static PyObject *
PP_set_set_info_type_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.info_type_get) UNMODIFIED */
    return Py_BuildValue("s", self->set->info_type);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.info_type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_info__doc__[] =
""
;

static PyObject *
PP_set_set_info_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.info_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->set->info, NULL);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.info_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_set_set_next__doc__[] =
""
;

static PyObject *
PP_set_set_next_get(PP_setObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.descriptor.set.next_get) UNMODIFIED */
    return _PY_set_from_ptr(self->set->next);
/* DO-NOT-DELETE splicer.end(pdb.set.descriptor.set.next_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.set.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.set.extra_members) */

static PyGetSetDef PP_set_getset[] = {
    {"set", (getter) PP_set_set_get, NULL, PP_set_set__doc__, NULL},
    {"name", (getter) PP_set_set_name_get, (setter) PP_set_set_name_set, PP_set_set_name__doc__, NULL},
    {"element_type", (getter) PP_set_set_element_type_get, NULL, PP_set_set_element_type__doc__, NULL},
    {"dimension", (getter) PP_set_set_dimension_get, NULL, PP_set_set_dimension__doc__, NULL},
    {"max_index", (getter) PP_set_set_max_index_get, NULL, PP_set_set_max_index__doc__, NULL},
    {"dimension_elem", (getter) PP_set_set_dimension_elem_get, NULL, PP_set_set_dimension_elem__doc__, NULL},
    {"n_elements", (getter) PP_set_set_n_elements_get, NULL, PP_set_set_n_elements__doc__, NULL},
    {"elements", (getter) PP_set_set_elements_get, NULL, PP_set_set_elements__doc__, NULL},
    {"es_type", (getter) PP_set_set_es_type_get, NULL, PP_set_set_es_type__doc__, NULL},
    {"extrema", (getter) PP_set_set_extrema_get, NULL, PP_set_set_extrema__doc__, NULL},
    {"scales", (getter) PP_set_set_scales_get, NULL, PP_set_set_scales__doc__, NULL},
    {"opers", (getter) PP_set_set_opers_get, NULL, PP_set_set_opers__doc__, NULL},
    {"metric", (getter) PP_set_set_metric_get, NULL, PP_set_set_metric__doc__, NULL},
    {"symmetry_type", (getter) PP_set_set_symmetry_type_get, NULL, PP_set_set_symmetry_type__doc__, NULL},
    {"symmetry", (getter) PP_set_set_symmetry_get, NULL, PP_set_set_symmetry__doc__, NULL},
    {"topology_type", (getter) PP_set_set_topology_type_get, NULL, PP_set_set_topology_type__doc__, NULL},
    {"topology", (getter) PP_set_set_topology_get, NULL, PP_set_set_topology__doc__, NULL},
    {"info_type", (getter) PP_set_set_info_type_get, NULL, PP_set_set_info_type__doc__, NULL},
    {"info", (getter) PP_set_set_info_get, NULL, PP_set_set_info__doc__, NULL},
    {"next", (getter) PP_set_set_next_get, NULL, PP_set_set_next__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pdb.set.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.set.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static int
PP_set_tp_init(PP_setObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.set.as_type.init) */
    char *name;
    char *type;
    int cp;
    long ne;
    int nd;
    int nde;
    int *maxes;
    void *elem;
    PM_field *opers;
    double *metric;
    char *symtype;
    void *sym;
    char *toptype;
    void *top;
    char *inftype;
    void *inf;
    PM_set *next;
    PP_fieldObject *opersobj;
    PP_setObject *nextobj;
    char *kw_list[] = {"name", "type", "cp", "ne", "nd", "nde", "maxes", "elem", "opers", "metric", "symtype", "sym", "toptype", "top", "inftype", "inf", "next", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssiliiO&zO!O&szszszO!:make_set", kw_list,
                                     &name, &type, &cp, &ne, &nd, &nde, iarray_extractor, &maxes, &elem, &PP_field_Type, &opersobj, REAL_array_extractor, &metric, &symtype, &sym, &toptype, &top, &inftype, &inf, &PP_set_Type, &nextobj))
       return -1;

    opers = opersobj->opers;
    next  = nextobj->set;
    self->set = PM_mk_set(name, type, cp, ne, nd, nde, maxes, elem,
			  opers, metric, symtype, sym, toptype, top,
			  inftype, inf, next);
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.set.as_type.init) */
}
/*--------------------------------------------------------------------------*/


static char PP_set_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_set_Type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        "set",                       /* tp_name */
        sizeof(PP_setObject),         /* tp_basicsize */
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
        PP_set_Type__doc__,             /* tp_doc */
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
        PP_set_getset,                  /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_set_tp_init,       /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.set.extra) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.set.extra) */
/* End of code for set objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
