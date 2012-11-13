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

/* DO-NOT-DELETE splicer.begin(pdb.memdes.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.memdes.C_definition) */

/*--------------------------------------------------------------------------*/

static char PP_memdes_desc__doc__[] =
""
;

static PyObject *
PP_memdes_desc_get(PP_memdesObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.descriptor.desc_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->desc, NULL);
/* DO-NOT-DELETE splicer.end(pdb.memdes.descriptor.desc_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_memdes_desc_member__doc__[] =
""
;

static PyObject *
PP_memdes_desc_member_get(PP_memdesObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.descriptor.desc.member_get) UNMODIFIED */
    return Py_BuildValue("s", self->desc->member);
/* DO-NOT-DELETE splicer.end(pdb.memdes.descriptor.desc.member_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_memdes_desc_cast_memb__doc__[] =
""
;

static PyObject *
PP_memdes_desc_cast_memb_get(PP_memdesObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.descriptor.desc.cast_memb_get) UNMODIFIED */
    return Py_BuildValue("s", self->desc->cast_memb);
/* DO-NOT-DELETE splicer.end(pdb.memdes.descriptor.desc.cast_memb_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_memdes_desc_cast_offs__doc__[] =
""
;

static PyObject *
PP_memdes_desc_cast_offs_get(PP_memdesObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.descriptor.desc.cast_offs_get) UNMODIFIED */
    return PY_INT_LONG(self->desc->cast_offs);
/* DO-NOT-DELETE splicer.end(pdb.memdes.descriptor.desc.cast_offs_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_memdes_desc_is_indirect__doc__[] =
""
;

static PyObject *
PP_memdes_desc_is_indirect_get(PP_memdesObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.descriptor.desc.is_indirect_get) */
#if PYTHON_API_VERSION < 1012
    return PY_INT_LONG(self->desc->is_indirect);
#else
    return PyBool_FromLong((long) self->desc->is_indirect);
#endif
/* DO-NOT-DELETE splicer.end(pdb.memdes.descriptor.desc.is_indirect_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_memdes_desc_type__doc__[] =
""
;

static PyObject *
PP_memdes_desc_type_get(PP_memdesObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.descriptor.desc.type_get) UNMODIFIED */
    return Py_BuildValue("s", self->desc->type);
/* DO-NOT-DELETE splicer.end(pdb.memdes.descriptor.desc.type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_memdes_desc_base_type__doc__[] =
""
;

static PyObject *
PP_memdes_desc_base_type_get(PP_memdesObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.descriptor.desc.base_type_get) UNMODIFIED */
    return Py_BuildValue("s", self->desc->base_type);
/* DO-NOT-DELETE splicer.end(pdb.memdes.descriptor.desc.base_type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_memdes_desc_name__doc__[] =
""
;

static PyObject *
PP_memdes_desc_name_get(PP_memdesObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.descriptor.desc.name_get) UNMODIFIED */
    return Py_BuildValue("s", self->desc->name);
/* DO-NOT-DELETE splicer.end(pdb.memdes.descriptor.desc.name_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_memdes_desc_number__doc__[] =
""
;

static PyObject *
PP_memdes_desc_number_get(PP_memdesObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.descriptor.desc.number_get) UNMODIFIED */
    return PY_INT_LONG(self->desc->number);
/* DO-NOT-DELETE splicer.end(pdb.memdes.descriptor.desc.number_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.memdes.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.memdes.extra_members) */

static PyGetSetDef PP_memdes_getset[] = {
    {"desc", (getter) PP_memdes_desc_get, NULL, PP_memdes_desc__doc__, NULL},
    {"member", (getter) PP_memdes_desc_member_get, NULL, PP_memdes_desc_member__doc__, NULL},
    {"cast_memb", (getter) PP_memdes_desc_cast_memb_get, NULL, PP_memdes_desc_cast_memb__doc__, NULL},
    {"cast_offs", (getter) PP_memdes_desc_cast_offs_get, NULL, PP_memdes_desc_cast_offs__doc__, NULL},
    {"is_indirect", (getter) PP_memdes_desc_is_indirect_get, NULL, PP_memdes_desc_is_indirect__doc__, NULL},
    {"type", (getter) PP_memdes_desc_type_get, NULL, PP_memdes_desc_type__doc__, NULL},
    {"base_type", (getter) PP_memdes_desc_base_type_get, NULL, PP_memdes_desc_base_type__doc__, NULL},
    {"name", (getter) PP_memdes_desc_name_get, NULL, PP_memdes_desc_name__doc__, NULL},
    {"number", (getter) PP_memdes_desc_number_get, NULL, PP_memdes_desc_number__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pdb.memdes.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.memdes.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static void
PP_memdes_tp_dealloc(PP_memdesObject *self)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.as_type.dealloc) */
    _PD_rl_descriptor(self->desc);
    PY_TYPE(self)->tp_free((PyObject*)self);
/* DO-NOT-DELETE splicer.end(pdb.memdes.as_type.dealloc) */
}

static int
PP_memdes_tp_init(PP_memdesObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.memdes.as_type.init) */
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.memdes.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_memdes_Check - */

/* static */ int
PP_memdes_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_memdes_Type))
        return 1;
    else
        return 0;
/*  return PY_TYPE(op) == &PP_memdes_Type; */
}

/*--------------------------------------------------------------------------*/


static char PP_memdes_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_memdes_Type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        "memdes",                       /* tp_name */
        sizeof(PP_memdesObject),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PP_memdes_tp_dealloc, /* tp_dealloc */
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
        PP_memdes_Type__doc__,          /* tp_doc */
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
        PP_memdes_getset,               /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_memdes_tp_init,    /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.memdes.extra) */

PP_memdesObject *PP_memdes_newobj(PP_memdesObject *obj, memdes *desc)
{
    if (obj == NULL) {
        obj = (PP_memdesObject *) PyType_GenericAlloc(&PP_memdes_Type, 0);
        if (obj == NULL) {
            return NULL;
        }
    }

    obj->desc = desc;
    SC_mark(desc, 1);

    return obj;
}

/* DO-NOT-DELETE splicer.end(pdb.memdes.extra) */
/* End of code for memdes objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
