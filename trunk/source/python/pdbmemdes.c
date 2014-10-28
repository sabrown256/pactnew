/*
 * Source Version: 9.0
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */

#define PY_DEF_DESTRUCTOR	    PY_memdes_tp_dealloc

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PY_memdes *PP_memdes_newobj(PY_memdes *obj, memdes *desc)
   {

    if (obj == NULL)
       {obj = (PY_memdes *) PyType_GenericAlloc(&PY_memdes_type, 0);
        if (obj == NULL)
	   return NULL;};

    obj->pyo = desc;
    SC_mark(desc, 1);

    return obj;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void PY_memdes_tp_dealloc(PY_memdes *self)
   {struct _typeobject *ot;

    _PD_rl_descriptor(self->pyo);

    ot = PY_TYPE(self);
    if (ot->tp_free != NULL)
       ot->tp_free(ot);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_memdes_tp_init(PY_memdes *self, PyObject *args, PyObject *kwds)
   {

    return 0;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_memdes_doc[] = "";

PY_DEF_TYPE(memdes);

#if 0

/* static */
PyTypeObject PY_memdes_type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        "memdes",                       /* tp_name */
        sizeof(PY_memdes),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PY_memdes_tp_dealloc, /* tp_dealloc */
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
        PY_memdes_type_doc,          /* tp_doc */
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
        PY_memdes_getset,               /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PY_memdes_tp_init,    /* tp_init */
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
