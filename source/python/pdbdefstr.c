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

/* DO-NOT-DELETE splicer.begin(pdb.defstr.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.defstr.C_definition) */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_defstr_has_key__doc__[] = 
""
;

static PyObject *
PP_defstr_has_key(PP_defstrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.method.has_key) */
    char *key;
    char *kw_list[] = {"key", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:has_key", kw_list,
                                     &key))
        return NULL;

    PyErr_SetString(PyExc_NotImplementedError, "has_key");
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.defstr.method.has_key) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_defstr_items__doc__[] = 
""
;

static PyObject *
PP_defstr_items(PP_defstrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.method.items) */
    PyErr_SetString(PyExc_NotImplementedError, "items");
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.defstr.method.items) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_defstr_keys__doc__[] = 
""
;

static PyObject *
PP_defstr_keys(PP_defstrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.method.keys) */
    int err;
    defstr *dp;
    memdes *desc;
    Py_ssize_t i;
    PyObject *rv;
 
    dp = self->dp;
    if (dp == NULL) {
        PP_error_set(PP_error_internal,
                     NULL, "Defstr is NULL");
        return NULL;
    }

    if (dp->members == 0) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    for (i = 0, desc = self->dp->members;
         desc != NULL;
         i++, desc = desc->next)
        ;


    rv = PyTuple_New(i);
    if (rv == NULL)
        return NULL;
    for (i = 0, desc = self->dp->members;
         desc != NULL;
         i++, desc = desc->next) {
        err = PyTuple_SetItem(rv, i, PyString_FromString(desc->name));
        if (err < 0) {
            Py_DECREF(rv);
            rv = NULL;
            break;
        }
    }
  
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.defstr.method.keys) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_defstr_values__doc__[] = 
""
;

static PyObject *
PP_defstr_values(PP_defstrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.method.values) */
    PyErr_SetString(PyExc_NotImplementedError, "values");
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.defstr.method.values) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_defstr_get__doc__[] = 
""
;

static PyObject *
PP_defstr_get(PP_defstrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.method.get) */
    PyErr_SetString(PyExc_NotImplementedError, "get");
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.defstr.method.get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.defstr.extra_methods) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.defstr.extra_methods) */

/*--------------------------------------------------------------------------*/

static PyMethodDef PP_defstr_methods[] = {
{"has_key", (PyCFunction)PP_defstr_has_key, METH_KEYWORDS, PP_defstr_has_key__doc__},
{"items", (PyCFunction)PP_defstr_items, METH_NOARGS, PP_defstr_items__doc__},
{"keys", (PyCFunction)PP_defstr_keys, METH_NOARGS, PP_defstr_keys__doc__},
{"values", (PyCFunction)PP_defstr_values, METH_NOARGS, PP_defstr_values__doc__},
{"get", (PyCFunction)PP_defstr_get, METH_NOARGS, PP_defstr_get__doc__},
/* DO-NOT-DELETE splicer.begin(pdb.defstr.extra_mlist) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.defstr.extra_mlist) */
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp__doc__[] =
""
;

static PyObject *
PP_defstr_dp_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp_get) UNMODIFIED */
    return PyCObject_FromVoidPtr((void *) self->dp, NULL);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp_type__doc__[] =
""
;

static PyObject *
PP_defstr_dp_type_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp.type_get) UNMODIFIED */
    return Py_BuildValue("s", self->dp->type);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp.type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp_size_bits__doc__[] =
""
;

static PyObject *
PP_defstr_dp_size_bits_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp.size_bits_get) UNMODIFIED */
    return PyInt_FromLong(self->dp->size_bits);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp.size_bits_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp_size__doc__[] =
""
;

static PyObject *
PP_defstr_dp_size_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp.size_get) UNMODIFIED */
    return PyInt_FromLong(self->dp->size);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp.size_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp_alignment__doc__[] =
""
;

static PyObject *
PP_defstr_dp_alignment_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp.alignment_get) UNMODIFIED */
    return PyInt_FromLong((long) self->dp->alignment);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp.alignment_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp_n_indirects__doc__[] =
""
;

static PyObject *
PP_defstr_dp_n_indirects_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp.n_indirects_get) UNMODIFIED */
    return PyInt_FromLong((long) self->dp->n_indirects);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp.n_indirects_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp_is_indirect__doc__[] =
""
;

static PyObject *
PP_defstr_dp_is_indirect_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp.is_indirect_get) UNMODIFIED */
    return PyInt_FromLong((long) self->dp->is_indirect);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp.is_indirect_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp_convert__doc__[] =
""
;

static PyObject *
PP_defstr_dp_convert_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp.convert_get) UNMODIFIED */
    return PyInt_FromLong((long) self->dp->convert);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp.convert_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp_onescmp__doc__[] =
""
;

static PyObject *
PP_defstr_dp_onescmp_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp.onescmp_get) UNMODIFIED */
    return PyInt_FromLong((long) self->dp->onescmp);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp.onescmp_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp_unsgned__doc__[] =
""
;

static PyObject *
PP_defstr_dp_unsgned_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp.unsgned_get) UNMODIFIED */
    return PyInt_FromLong((long) self->dp->unsgned);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp.unsgned_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_defstr_dp_order_flag__doc__[] =
""
;

static PyObject *
PP_defstr_dp_order_flag_get(PP_defstrObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.descriptor.dp.fix.order_get) UNMODIFIED */
    return PyInt_FromLong((long) self->dp->fix.order);
/* DO-NOT-DELETE splicer.end(pdb.defstr.descriptor.dp.fix.order_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.defstr.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.defstr.extra_members) */

static PyGetSetDef PP_defstr_getset[] = {
    {"dp", (getter) PP_defstr_dp_get, NULL, PP_defstr_dp__doc__, NULL},
    {"type", (getter) PP_defstr_dp_type_get, NULL, PP_defstr_dp_type__doc__, NULL},
    {"size_bits", (getter) PP_defstr_dp_size_bits_get, NULL, PP_defstr_dp_size_bits__doc__, NULL},
    {"size", (getter) PP_defstr_dp_size_get, NULL, PP_defstr_dp_size__doc__, NULL},
    {"alignment", (getter) PP_defstr_dp_alignment_get, NULL, PP_defstr_dp_alignment__doc__, NULL},
    {"n_indirects", (getter) PP_defstr_dp_n_indirects_get, NULL, PP_defstr_dp_n_indirects__doc__, NULL},
    {"is_indirect", (getter) PP_defstr_dp_is_indirect_get, NULL, PP_defstr_dp_is_indirect__doc__, NULL},
    {"convert", (getter) PP_defstr_dp_convert_get, NULL, PP_defstr_dp_convert__doc__, NULL},
    {"onescmp", (getter) PP_defstr_dp_onescmp_get, NULL, PP_defstr_dp_onescmp__doc__, NULL},
    {"unsgned", (getter) PP_defstr_dp_unsgned_get, NULL, PP_defstr_dp_unsgned__doc__, NULL},
    {"order_flag", (getter) PP_defstr_dp_order_flag_get, NULL, PP_defstr_dp_order_flag__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pdb.defstr.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.defstr.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static void
PP_defstr_tp_dealloc(PP_defstrObject *self)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.as_type.dealloc) */
    _PP_rl_defstr(self);
    self->ob_type->tp_free((PyObject*)self);
/* DO-NOT-DELETE splicer.end(pdb.defstr.as_type.dealloc) */
}

static int
PP_defstr_tp_print(PP_defstrObject *self, FILE *file, int flags)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.as_type.print) */
    PD_write_defstr(file, self->dp);
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.defstr.as_type.print) */
}

static PyObject *
PP_defstr_tp_call(PP_defstrObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.as_type.call) */
    PyObject *rv;
    
    if (self->ctor == NULL) {
        PyErr_Format(
            PyExc_TypeError,
            "'%s' object is not callable",
            self->ob_type->tp_name);
        rv = NULL;
    } else {
        rv = PyObject_Call((PyObject *) self->ctor, args, kwds);
    }
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.defstr.as_type.call) */
}

static int
PP_defstr_tp_init(PP_defstrObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.as_type.init) */
    char *name;
    PyObject *members;
    PP_PDBfileObject *file;
    char *kw_list[] = {"name", "members", "file", NULL};

    file = PP_vif_obj;
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "sO|O&", kw_list,
                                    &name, &members, PP_convert_pdbfile, &file))
        return -1;

    self = _PP_defstr_make_singleton(self, name, members, file->fileinfo);
    if (self == NULL)
        return -1;
    
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.defstr.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_defstr_Check - */

/* static */ int
PP_defstr_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_defstr_Type))
        return 1;
    else
        return 0;
/*  return op->ob_type == &PP_defstr_Type; */
}

/*--------------------------------------------------------------------------*/
/*                           OBJECT_TP_AS_MAPPING                           */
/*--------------------------------------------------------------------------*/

/* Code to access defstr objects as mappings */

static Py_ssize_t
PP_defstr_mp_length(PyObject *_self)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.as_mapping.length) */
    Py_ssize_t nitems;
    defstr *dp;
    memdes *desc;
    PP_defstrObject *self = (PP_defstrObject *) _self;
 
    dp = self->dp;
    if (dp == NULL) {
        PP_error_set(PP_error_internal,
                     NULL, "Defstr is NULL");
        return -1;
    }

    for (nitems = 0, desc = self->dp->members;
         desc != NULL;
         nitems++, desc = desc->next)
        ;

    return nitems;
/* DO-NOT-DELETE splicer.end(pdb.defstr.as_mapping.length) */
}

static PyObject *
PP_defstr_mp_subscript(PyObject *_self, PyObject *key)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.as_mapping.subscript) */
    char *name;
    defstr *dp;
    memdes *desc;
    PyObject *rv;
    PP_defstrObject *self = (PP_defstrObject *) _self;
 
    if (!PyString_Check(key)) {
        PP_error_set_user(key, "key must be string");
        return NULL;
    }
    name = PyString_AsString(key);

    dp = self->dp;
    if (dp == NULL) {
        PP_error_set(PP_error_internal,
                     NULL, "Defstr is NULL");
        return NULL;
    }

    for (desc = self->dp->members; desc != NULL; desc = desc->next) {
        if (strcmp(desc->name, name) == 0)
            break;
    }

    if (desc != NULL) {
        rv = (PyObject *) PP_memdes_newobj(NULL, desc);
    } else {
        PyErr_SetObject(PyExc_KeyError, key);
        rv = NULL;
    }

    return rv;
/* DO-NOT-DELETE splicer.end(pdb.defstr.as_mapping.subscript) */
}

#if 0
static int
PP_defstr_mp_ass_subscript(PyObject *_self, PyObject *key, PyObject *v)
{
/* DO-NOT-DELETE splicer.begin(pdb.defstr.as_mapping.ass_subscript) */
UNDEFINED
    PP_defstrObject *self = (PP_defstrObject *) _self;
/* DO-NOT-DELETE splicer.end(pdb.defstr.as_mapping.ass_subscript) */
}
#endif


static PyMappingMethods PP_defstr_as_mapping = {
        PP_defstr_mp_length,            /* mp_length */
        PP_defstr_mp_subscript,         /* mp_subscript */
        0,                              /* mp_ass_subscript */
};

/*--------------------------------------------------------------------------*/


static char PP_defstr_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_defstr_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /* ob_size */
        "defstr",                       /* tp_name */
        sizeof(PP_defstrObject),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PP_defstr_tp_dealloc, /* tp_dealloc */
        (printfunc)PP_defstr_tp_print,  /* tp_print */
        (getattrfunc)0,                 /* tp_getattr */
        (setattrfunc)0,                 /* tp_setattr */
        (cmpfunc)0,                     /* tp_compare */
        (reprfunc)0,                    /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        0,                              /* tp_as_sequence */
        &PP_defstr_as_mapping,          /* tp_as_mapping */
        /* More standard operations (here for binary compatibility) */
        (hashfunc)0,                    /* tp_hash */
        (ternaryfunc)PP_defstr_tp_call, /* tp_call */
        (reprfunc)0,                    /* tp_str */
        (getattrofunc)0,                /* tp_getattro */
        (setattrofunc)0,                /* tp_setattro */
        /* Functions to access object as input/output buffer */
        0,                              /* tp_as_buffer */
        /* Flags to define presence of optional/expanded features */
        Py_TPFLAGS_DEFAULT,             /* tp_flags */
        PP_defstr_Type__doc__,          /* tp_doc */
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
        PP_defstr_methods,              /* tp_methods */
        0,                              /* tp_members */
        PP_defstr_getset,               /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_defstr_tp_init,    /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.defstr.extra) */
/*--------------------------------------------------------------------------*/

PP_defstrObject *PP_defstr_newobj(PP_defstrObject *obj, defstr *dp,
                                  PP_file *fileinfo)
{
    PyTypeObject *ctor;

    if (obj == NULL) {
        obj = (PP_defstrObject *) PyType_GenericAlloc(&PP_defstr_Type, 0);
        if (obj == NULL) {
            return NULL;
        }
    }

    obj->dp       = dp;
    obj->fileinfo = fileinfo;

    /* save a reference to the host_chart.
     * This is used when the file has been closed but some
     * references to defstrObjects still exist.
     */
    obj->host_chart = fileinfo->file->host_chart;

    ctor = PP_defstr_mk_ctor(obj);
    if (ctor == NULL)
        return NULL;
    obj->ctor = ctor;

    SC_mark(dp, 1);
    SC_mark(fileinfo->file->host_chart, 1);

    return obj;
}

/* DO-NOT-DELETE splicer.end(pdb.defstr.extra) */
/* End of code for defstr objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
