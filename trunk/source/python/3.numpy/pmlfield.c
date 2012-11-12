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

/* DO-NOT-DELETE splicer.begin(pdb.field.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.field.C_definition) */

/*--------------------------------------------------------------------------*/

static char PP_field_opers__doc__[] =
""
;

static PyObject *
PP_field_opers_get(PP_fieldObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.field.descriptor.opers_get) */
    return PPfield_from_ptr(self->opers);
/* DO-NOT-DELETE splicer.end(pdb.field.descriptor.opers_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.field.extra_members) */
/* DO-NOT-DELETE splicer.end(pdb.field.extra_members) */

static PyGetSetDef PP_field_getset[] = {
    {"opers", (getter) PP_field_opers_get, NULL, PP_field_opers__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pdb.field.extra_getset) */
/* DO-NOT-DELETE splicer.end(pdb.field.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static int
PP_field_tp_init(PP_fieldObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.field.as_type.init) */
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.field.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_field_Check - */

/* static */ int
PP_field_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_field_Type))
        return 1;
    else
        return 0;
/*  return op->ob_type == &PP_field_Type; */
}

/*--------------------------------------------------------------------------*/


static char PP_field_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_field_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        0,                              /* ob_size */
        "field",                       /* tp_name */
        sizeof(PP_fieldObject),         /* tp_basicsize */
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
        PP_field_Type__doc__,           /* tp_doc */
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
        PP_field_getset,                /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_field_tp_init,     /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.field.extra) */
/* DO-NOT-DELETE splicer.end(pdb.field.extra) */
/* End of code for field objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
