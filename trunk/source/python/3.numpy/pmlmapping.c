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

/* DO-NOT-DELETE splicer.begin(pdb.mapping.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mapping.C_definition) */

/*--------------------------------------------------------------------------*/

static char PP_mapping_map__doc__[] =
""
;

static PyObject *
PP_mapping_map_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map_get) UNMODIFIED */
    return PPmapping_from_ptr(self->map);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mapping_map_name__doc__[] =
""
;

static PyObject *
PP_mapping_map_name_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map.name_get) UNMODIFIED */
    return Py_BuildValue("s", self->map->name);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map.name_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mapping_map_category__doc__[] =
""
;

static PyObject *
PP_mapping_map_category_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map.category_get) UNMODIFIED */
    return Py_BuildValue("s", self->map->category);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map.category_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mapping_map_domain__doc__[] =
""
;

static PyObject *
PP_mapping_map_domain_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map.domain_get) UNMODIFIED */
    return PPset_from_ptr(self->map->domain);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map.domain_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mapping_map_range__doc__[] =
""
;

static PyObject *
PP_mapping_map_range_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map.range_get) UNMODIFIED */
    return PPset_from_ptr(self->map->range);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map.range_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mapping_map_map_type__doc__[] =
""
;

static PyObject *
PP_mapping_map_map_type_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map.map_type_get) UNMODIFIED */
    return Py_BuildValue("s", self->map->map_type);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map.map_type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mapping_map_map__doc__[] =
""
;

static PyObject *
PP_mapping_map_map_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map.map_get) UNMODIFIED */
    return PyCObject_FromVoidPtr((void *) self->map->map, NULL);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map.map_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mapping_map_file_type__doc__[] =
""
;

static PyObject *
PP_mapping_map_file_type_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map.file_type_get) UNMODIFIED */
    return PyInt_FromLong((long) self->map->file_type);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map.file_type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mapping_map_file_info__doc__[] =
""
;

static PyObject *
PP_mapping_map_file_info_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map.file_info_get) UNMODIFIED */
    return PyCObject_FromVoidPtr((void *) self->map->file_info, NULL);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map.file_info_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mapping_map_file__doc__[] =
""
;

static PyObject *
PP_mapping_map_file_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map.file_get) UNMODIFIED */
    return Py_BuildValue("s", self->map->file);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map.file_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_mapping_map_next__doc__[] =
""
;

static PyObject *
PP_mapping_map_next_get(PP_mappingObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.descriptor.map.next_get) UNMODIFIED */
    return PPmapping_from_ptr(self->map->next);
/* DO-NOT-DELETE splicer.end(pdb.mapping.descriptor.map.next_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.mapping.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mapping.extra_members) */

static PyGetSetDef PP_mapping_getset[] = {
    {"map", (getter) PP_mapping_map_get, NULL, PP_mapping_map__doc__, NULL},
    {"name", (getter) PP_mapping_map_name_get, NULL, PP_mapping_map_name__doc__, NULL},
    {"category", (getter) PP_mapping_map_category_get, NULL, PP_mapping_map_category__doc__, NULL},
    {"domain", (getter) PP_mapping_map_domain_get, NULL, PP_mapping_map_domain__doc__, NULL},
    {"range", (getter) PP_mapping_map_range_get, NULL, PP_mapping_map_range__doc__, NULL},
    {"map_type", (getter) PP_mapping_map_map_type_get, NULL, PP_mapping_map_map_type__doc__, NULL},
    {"map", (getter) PP_mapping_map_map_get, NULL, PP_mapping_map_map__doc__, NULL},
    {"file_type", (getter) PP_mapping_map_file_type_get, NULL, PP_mapping_map_file_type__doc__, NULL},
    {"file_info", (getter) PP_mapping_map_file_info_get, NULL, PP_mapping_map_file_info__doc__, NULL},
    {"file", (getter) PP_mapping_map_file_get, NULL, PP_mapping_map_file__doc__, NULL},
    {"next", (getter) PP_mapping_map_next_get, NULL, PP_mapping_map_next__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pdb.mapping.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mapping.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static int
PP_mapping_tp_init(PP_mappingObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.mapping.as_type.init) */
    char *name;
    char *cat;
    PM_set *domain;
    PM_set *range;
    int centering;
    PM_mapping *next;
    PP_setObject *domainobj;
    PP_setObject *rangeobj;
    char *kw_list[] = {"name", "cat", "domain", "range", "centering", "next", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssO!O!iO&:make_mapping", kw_list,
                                     &name, &cat, &PP_set_Type, &domainobj, &PP_set_Type, &rangeobj, &centering, mapping_extractor, &next))
        return -1;
    domain = domainobj->set;
    range = rangeobj->set;
    self->map = PM_make_mapping(name, cat, domain, range, centering, next);
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.mapping.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_mapping_Check - */

/* static */ int
PP_mapping_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_mapping_Type))
        return 1;
    else
        return 0;
/*  return op->ob_type == &PP_mapping_Type; */
}

/*--------------------------------------------------------------------------*/


static char PP_mapping_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_mapping_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        0,                              /* ob_size */
        "mapping",                       /* tp_name */
        sizeof(PP_mappingObject),         /* tp_basicsize */
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
        PP_mapping_Type__doc__,         /* tp_doc */
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
        PP_mapping_getset,              /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_mapping_tp_init,   /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.mapping.extra) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.mapping.extra) */
/* End of code for mapping objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
