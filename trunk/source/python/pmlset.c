/*
 * PMLSET.C - Python binding routines for PM_set
 *
 * include cpyright.h
 */

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/

/*                              GENERATED STUFF                             */

/*--------------------------------------------------------------------------*/

static int PY_PM_set_set_name(PY_PM_set *self, PyObject *value, void *context)
{
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "attribute deletion is not supported");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &self->pyo->name))
        return -1;
    return 0;
}

/*--------------------------------------------------------------------------*/

static char PY_PM_set_doc_opers[] = "";

static PyObject *
PY_PM_set_get_opers(PY_PM_set *self, void *context)
{
    return PPfield_from_ptr(self->pyo->opers);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0

static PyGetSetDef PY_PM_set_getset[] = {

    {"name", (getter) PY_PM_set_get_name, (setter) PY_PM_set_set_name, PY_PM_set_doc_name, NULL},

    {NULL}};

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int
PY_PM_set_tp_init(PY_PM_set *self, PyObject *args, PyObject *kwds)
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
    PY_PM_set *nextobj;
    char *kw_list[] = {"name", "type", "cp", "ne", "nd", "nde", "maxes", "elem", "opers", "metric", "symtype", "sym", "toptype", "top", "inftype", "inf", "next", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssiliiO&zO!O&szszszO!:make_set", kw_list,
                                     &name, &type, &cp, &ne, &nd, &nde, iarray_extractor, &maxes, &elem, &PP_field_Type, &opersobj, REAL_array_extractor, &metric, &symtype, &sym, &toptype, &top, &inftype, &inf, &PY_PM_set_type, &nextobj))
       return -1;

    opers = opersobj->opers;
    next  = nextobj->pyo;
    self->pyo = PM_mk_set(name, type, cp, ne, nd, nde, maxes, elem,
			  opers, metric, symtype, sym, toptype, top,
			  inftype, inf, next);
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.set.as_type.init) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PY_PM_set_doc[] = "";

PY_DEF_TYPE(PM_set);

#if 0

/* static */
PyTypeObject PY_PM_set_type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        "set",                       /* tp_name */
        sizeof(PY_PM_set),         /* tp_basicsize */
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
        PY_PM_set_type_doc,             /* tp_doc */
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
        PY_PM_set_getset,                  /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PY_PM_set_tp_init,       /* tp_init */
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
