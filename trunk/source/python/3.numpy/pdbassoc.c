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

/* DO-NOT-DELETE splicer.begin(pdb.assoc.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.assoc.C_definition) */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_assoc_clear__doc__[] = 
""
;

static PyObject *
PP_assoc_clear(PP_assocObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.method.clear) */
#if 0
    PD_clear(self->alist);
#endif
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.assoc.method.clear) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_assoc_has_key__doc__[] = 
""
;

static PyObject *
PP_assoc_has_key(PP_assocObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.method.has_key) */
    long ok;
    char *key;
    char *kw_list[] = {"key", NULL};
    pcons *alist, *pa;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:has_key", kw_list,
                                     &key))
        return NULL;

    alist = self->alist;
    if (alist == NULL) {
        ok = 0;
    } else {
        pa = SC_assoc_entry(alist, key);
        if (pa == NULL) {
            ok = 0;
        } else {
            ok = 1;
        }
    }

#if PYTHON_API_VERSION < 1012
    return PY_INT_LONG(ok);
#else
    return PyBool_FromLong(ok);
#endif
/* DO-NOT-DELETE splicer.end(pdb.assoc.method.has_key) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_assoc_items__doc__[] = 
""
;

static PyObject *
PP_assoc_items(PP_assocObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.method.items) */
    char *s;
    pcons *pa, *c;
    Py_ssize_t nitems, i;
    PyObject *rv, *item, *obj;

    nitems = 0;
    for (pa = self->alist; pa != NULL; pa = (pcons *) pa->cdr)
        nitems++;

    rv = PyList_New(nitems);
    if (rv == NULL)
        return NULL;

    for (pa = self->alist, i = 0;
         pa != NULL;
         pa = (pcons *) pa->cdr, i++) {
        c = (pcons *) pa->car;
        s = (char *) c->car;
        item = PyTuple_New(2);
        if (item == NULL) {
            item = NULL;
            break;
        }
        obj = PY_STRING_STRING(s);
        PyTuple_SET_ITEM(item, 0, obj);
        obj = PP_unpack_hashtab_haelem(c->cdr_type, c->cdr);
        PyTuple_SET_ITEM(item, 1, obj);
        
        PyList_SET_ITEM(rv, i, item);
    }
        
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.assoc.method.items) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_assoc_keys__doc__[] = 
""
;

static PyObject *
PP_assoc_keys(PP_assocObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.method.keys) */
    int err;
    char *s;
    pcons *pa, *c;
    Py_ssize_t nitems, i;
    PyObject *rv;

    nitems = 0;
    for (pa = self->alist; pa != NULL; pa = (pcons *) pa->cdr)
        nitems++;

    rv = PyTuple_New(nitems);
    if (rv == NULL)
        return NULL;

    for (pa = self->alist, i = 0;
         pa != NULL;
         pa = (pcons *) pa->cdr, i++) {
        c = (pcons *) pa->car;
        s = (char *) c->car;
        err = PyTuple_SetItem(rv, i, PY_STRING_STRING(s));
        if (err < 0) {
            Py_DECREF(rv);
            rv = NULL;
            break;
        }
    }
        
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.assoc.method.keys) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_assoc_update__doc__[] = 
""
;

static PyObject *
PP_assoc_update(PP_assocObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.method.update) */
    int err;
    pcons *alist;
    PyObject *dict;
    char *kw_list[] = {"dict", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O:update", kw_list,
                                     &dict))
        return NULL;

    alist = self->alist;
    err = PP_update_assoc(&alist, dict);
    if (err < 0)
        return NULL;
    self->alist = alist;

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.assoc.method.update) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_assoc_values__doc__[] = 
""
;

static PyObject *
PP_assoc_values(PP_assocObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.method.values) */
    PyErr_SetString(PyExc_NotImplementedError, "values");
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.assoc.method.values) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_assoc_get__doc__[] = 
""
;

static PyObject *
PP_assoc_get(PP_assocObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.method.get) */
    PyErr_SetString(PyExc_NotImplementedError, "get");
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.assoc.method.get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.assoc.extra_methods) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.assoc.extra_methods) */

/*--------------------------------------------------------------------------*/

static PyMethodDef PP_assoc_methods[] = {
{"clear", (PyCFunction)PP_assoc_clear, METH_NOARGS, PP_assoc_clear__doc__},
{"has_key", (PyCFunction)PP_assoc_has_key, METH_KEYWORDS, PP_assoc_has_key__doc__},
{"items", (PyCFunction)PP_assoc_items, METH_NOARGS, PP_assoc_items__doc__},
{"keys", (PyCFunction)PP_assoc_keys, METH_NOARGS, PP_assoc_keys__doc__},
{"update", (PyCFunction)PP_assoc_update, METH_KEYWORDS, PP_assoc_update__doc__},
{"values", (PyCFunction)PP_assoc_values, METH_NOARGS, PP_assoc_values__doc__},
{"get", (PyCFunction)PP_assoc_get, METH_NOARGS, PP_assoc_get__doc__},
/* DO-NOT-DELETE splicer.begin(pdb.assoc.extra_mlist) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.assoc.extra_mlist) */
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/

static char PP_assoc_alist__doc__[] =
""
;

static PyObject *
PP_assoc_alist_get(PP_assocObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.descriptor.alist_get) */
    return PY_COBJ_VOID_PTR((void *) self->alist, NULL);
   /* return PP_assoc_from_ptr(self->alist); */
/* DO-NOT-DELETE splicer.end(pdb.assoc.descriptor.alist_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.assoc.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.assoc.extra_members) */

static PyGetSetDef PP_assoc_getset[] = {
    {"alist", (getter) PP_assoc_alist_get, NULL, PP_assoc_alist__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pdb.assoc.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.assoc.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static void
PP_assoc_tp_dealloc(PP_assocObject *self)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.as_type.dealloc) */
    if (SC_safe_to_free(self->alist))
        SC_free_alist(self->alist, 3);
    PY_TYPE(self)->tp_free((PyObject*)self);
/* DO-NOT-DELETE splicer.end(pdb.assoc.as_type.dealloc) */
}

static PyObject *
PP_assoc_tp_repr(PP_assocObject *self)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.as_type.repr) */
    PyObject *rv, *h;

    h = _PP_unpack_assoc(self->alist, 1L);
    if (h == NULL)
        return NULL;
    rv = PyObject_Repr(h);
    Py_DECREF(h);
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.assoc.as_type.repr) */
}

static int
PP_assoc_tp_init(PP_assocObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.as_type.init) */
    self->alist = NULL;
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.assoc.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_assoc_Check - */

/* static */ int
PP_assoc_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_assoc_Type))
        return 1;
    else
        return 0;
/*  return PY_TYPE(op) == &PP_assoc_Type; */
}

/*--------------------------------------------------------------------------*/
/*                           OBJECT_TP_AS_MAPPING                           */
/*--------------------------------------------------------------------------*/

/* Code to access assoc objects as mappings */

static Py_ssize_t
PP_assoc_mp_length(PyObject *_self)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.as_mapping.length) */
    Py_ssize_t i;
    pcons *pa;
    PP_assocObject *self = (PP_assocObject *) _self;

    i = 0;
    for (pa = self->alist; pa != NULL; pa = (pcons *) pa->cdr) {
        i++;
    }
    
    return i;
/* DO-NOT-DELETE splicer.end(pdb.assoc.as_mapping.length) */
}

static PyObject *
PP_assoc_mp_subscript(PyObject *_self, PyObject *key)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.as_mapping.subscript) */
    char *name;
    pcons *alist, *pa;
    PyObject *rv;
    PP_assocObject *self = (PP_assocObject *) _self;

    if (!PY_STRING_CHECK(key)) {
        PP_error_set_user(key, "key must be string");
        return NULL;
    }
    name = PY_STRING_AS_STRING(key);

    alist = self->alist;
    if (alist == NULL) {
        PyErr_SetString(PyExc_KeyError, name);
        return NULL;
    }

    pa = SC_assoc_entry(alist, name);
    if (pa == NULL) {
        PyErr_SetString(PyExc_KeyError, name);
        return NULL;
    }

    rv = PP_unpack_hashtab_haelem(pa->cdr_type, pa->cdr);

    return rv;
/* DO-NOT-DELETE splicer.end(pdb.assoc.as_mapping.subscript) */
}

static int
PP_assoc_mp_ass_subscript(PyObject *_self, PyObject *key, PyObject *v)
{
/* DO-NOT-DELETE splicer.begin(pdb.assoc.as_mapping.ass_subscript) */
    char *name;
    pcons *alist;
    PP_assocObject *self = (PP_assocObject *) _self;
    
#if 0
    if (!PY_STRING_CHECK(key)) {
        PP_user_error(PP_score_errorObject, key, "key must be string");
        return -1;
    }
#endif
    name = PY_STRING_AS_STRING(key);
    
    alist = self->alist;

    if (v == NULL) {
        /* delete item */
        if (alist == NULL) {
            PyErr_SetString(PyExc_KeyError, name);
        }
        alist = SC_rem_alist(alist, name);
    } else {
        int ok;
        char *ptype;
        PP_descr *descr;
        void *vr;

        descr = PP_get_object_descr(PP_vif_info, v);
        if (descr == NULL)
            return -1;

        if (descr->data == NULL) {
            ok = PP_make_data(v, PP_vif_info, descr->type, descr->dims, &vr);
            if (ok == -1)
                return -1;
        } else {
            vr = descr->data;
        }
/*        SC_mark(vr, 1);*/

        ptype = PP_add_indirection(descr->type, 1);
        if (ptype == NULL)
            return -1;

        /* remove the item first, if it exists */
        if (alist != NULL)
            alist = SC_rem_alist(alist, name);

        /* SC_add_alist will CSTRSAVE ptype and SC_mark vr */
        alist = SC_add_alist(alist, name, ptype, vr);

        CFREE(ptype);
        _PP_rl_descr(descr);

        if (alist == NULL) {
            return -1;
        }
    }
    self->alist = alist;

    return 0;
/* DO-NOT-DELETE splicer.end(pdb.assoc.as_mapping.ass_subscript) */
}


static PyMappingMethods PP_assoc_as_mapping = {
        PP_assoc_mp_length,             /* mp_length */
        PP_assoc_mp_subscript,          /* mp_subscript */
        PP_assoc_mp_ass_subscript,      /* mp_ass_subscript */
};

/*--------------------------------------------------------------------------*/


static char PP_assoc_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_assoc_Type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        0,                              /* ob_size */
        "assoc",                       /* tp_name */
        sizeof(PP_assocObject),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PP_assoc_tp_dealloc, /* tp_dealloc */
        (printfunc)0,                   /* tp_print */
        (getattrfunc)0,                 /* tp_getattr */
        (setattrfunc)0,                 /* tp_setattr */
        (cmpfunc)0,                     /* tp_compare */
        (reprfunc)PP_assoc_tp_repr,     /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        0,                              /* tp_as_sequence */
        &PP_assoc_as_mapping,           /* tp_as_mapping */
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
        PP_assoc_Type__doc__,           /* tp_doc */
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
        PP_assoc_methods,               /* tp_methods */
        0,                              /* tp_members */
        PP_assoc_getset,                /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_assoc_tp_init,     /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.assoc.extra) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.assoc.extra) */
/* End of code for assoc objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
