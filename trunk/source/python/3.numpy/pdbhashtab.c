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

/* DO-NOT-DELETE splicer.begin(pdb.hashtab.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.hashtab.C_definition) */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hashtab_install__doc__[] = 
""
;

static PyObject *
PP_hashtab_install(PP_hashtabObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.method.install) */
    char *key, *obj, *type;
    char *kw_list[] = {"key", "obj", "type", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss:install", kw_list,
                                     &key, &obj, &type))
        return NULL;
    SC_hasharr_install(self->data, key, obj, type, 3, -1);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.method.install) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hashtab_def_lookup__doc__[] = 
""
;

static PyObject *
PP_hashtab_def_lookup(PP_hashtabObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.method.def_lookup) */
  char *key;
  char *kw_list[] = {"key", NULL};
  hasharr *tab;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:def_lookup", kw_list,
                                   &key))
      return NULL;
  tab = (hasharr *) self->data;
  SC_hasharr_def_lookup(tab, key);
  Py_INCREF(Py_None);
  return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.method.def_lookup) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hashtab_clear__doc__[] = 
""
;

static PyObject *
PP_hashtab_clear(PP_hashtabObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.method.clear) */
    SC_hasharr_clear(self->data, NULL, NULL);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.method.clear) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hashtab_has_key__doc__[] = 
""
;

static PyObject *
PP_hashtab_has_key(PP_hashtabObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.method.has_key) */
    long ok;
    char *key;
    char *kw_list[] = {"key", NULL};
    haelem *np;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:has_key", kw_list,
                                     &key))
        return NULL;

    np = SC_hasharr_lookup(self->data, key);
    ok = np != NULL;

#if PYTHON_API_VERSION < 1012
    return PyInt_FromLong(ok);
#else
    return PyBool_FromLong(ok);
#endif
/* DO-NOT-DELETE splicer.end(pdb.hashtab.method.has_key) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hashtab_items__doc__[] = 
""
;

static PyObject *
PP_hashtab_items(PP_hashtabObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.method.items) */
    PyErr_SetString(PyExc_NotImplementedError, "items");
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.method.items) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hashtab_keys__doc__[] = 
""
;

static PyObject *
PP_hashtab_keys(PP_hashtabObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.method.keys) */
    int ne, err;
    char **names;
    hasharr *tab;
    Py_ssize_t i;
    PyObject *rv;
 
    tab = self->data;
    if (tab == NULL) {
        PP_error_set(PP_error_internal,
                     NULL, "Hashtab is NULL");
        return NULL;
    }

    ne = SC_hasharr_get_n(tab);
    if (ne == 0) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    rv = PyTuple_New(ne);
    if (rv == NULL)
        return NULL;
    names = SC_hasharr_dump(tab, NULL, NULL, FALSE);
    for (i = 0; i < ne; i++) {
        err = PyTuple_SetItem(rv, i, PyString_FromString(names[i]));
        if (err < 0) {
            Py_DECREF(rv);
            rv = NULL;
            break;
        }
    }
    CFREE(names);
  
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.method.keys) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hashtab_update__doc__[] = 
""
;

static PyObject *
PP_hashtab_update(PP_hashtabObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.method.update) */
    int err;
    PyObject *dict;
    char *kw_list[] = {"dict", NULL};
    hasharr *tab;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O", kw_list,
                                    &dict))
        return NULL;

    tab = (hasharr *) self->data;
    if (tab == NULL) {
        PP_error_set(PP_error_internal,
                     (PyObject *) self, "Hashtab is NULL");
        return NULL;
    }

    err = PP_update_hashtab(tab, dict);
    if (err < 0)
        return NULL;

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.method.update) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hashtab_values__doc__[] = 
""
;

static PyObject *
PP_hashtab_values(PP_hashtabObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.method.values) */
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.method.values) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hashtab_get__doc__[] = 
""
;

static PyObject *
PP_hashtab_get(PP_hashtabObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.method.get) */
    PyErr_SetString(PyExc_NotImplementedError, "get");
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.method.get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.hashtab.extra_methods) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.hashtab.extra_methods) */

/*--------------------------------------------------------------------------*/

static PyMethodDef PP_hashtab_methods[] = {
{"install", (PyCFunction)PP_hashtab_install, METH_KEYWORDS, PP_hashtab_install__doc__},
{"def_lookup", (PyCFunction)PP_hashtab_def_lookup, METH_KEYWORDS, PP_hashtab_def_lookup__doc__},
{"clear", (PyCFunction)PP_hashtab_clear, METH_NOARGS, PP_hashtab_clear__doc__},
{"has_key", (PyCFunction)PP_hashtab_has_key, METH_KEYWORDS, PP_hashtab_has_key__doc__},
{"items", (PyCFunction)PP_hashtab_items, METH_NOARGS, PP_hashtab_items__doc__},
{"keys", (PyCFunction)PP_hashtab_keys, METH_NOARGS, PP_hashtab_keys__doc__},
{"update", (PyCFunction)PP_hashtab_update, METH_KEYWORDS, PP_hashtab_update__doc__},
{"values", (PyCFunction)PP_hashtab_values, METH_NOARGS, PP_hashtab_values__doc__},
{"get", (PyCFunction)PP_hashtab_get, METH_NOARGS, PP_hashtab_get__doc__},
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.extra_mlist) */
/* DO-NOT-DELETE splicer.end(pdb.hashtab.extra_mlist) */
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.hashtab.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.hashtab.extra_members) */

static PyGetSetDef PP_hashtab_getset[] = {

/* DO-NOT-DELETE splicer.begin(pdb.hashtab.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.hashtab.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static int _PP_hash_cln(haelem *hp, void *a)
{
    (void) _PP_rel_syment(PP_vif->host_chart, (char *) &hp->def, 1L, hp->type);
    CFREE(hp->type);
    CFREE(hp->def);

    return(TRUE);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void
PP_hashtab_tp_dealloc(PP_hashtabObject *self)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.as_type.dealloc) */
    int n;

    n = SC_mark(self->data, 0);
    if (n < 2) {
	SC_free_hasharr(self->data, _PP_hash_cln, NULL);
        self->data = NULL;
    } else {
        SC_mark(self->data, -1);
    }
    self->ob_type->tp_free((PyObject*)self);
/* DO-NOT-DELETE splicer.end(pdb.hashtab.as_type.dealloc) */
}

static PyObject *
PP_hashtab_tp_repr(PP_hashtabObject *self)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.as_type.repr) */
    PyObject *rv, *h;

    h = _PP_unpack_hashtab(self->data, 1L);
    if (h == NULL)
        return NULL;
    rv = PyObject_Repr(h);
    Py_DECREF(h);
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.as_type.repr) */
}

static int
PP_hashtab_tp_init(PP_hashtabObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.as_type.init) */

    int size, docflag;
    char *kw_list[] = {"size", "docflag", NULL};
    hasharr *tab;

    size = HSZSMALL;
    docflag = NODOC;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|ii:init", kw_list,
                                     &size, &docflag))
        return -1;

    tab = SC_make_hasharr(size, docflag, SC_HA_NAME_KEY, 0);
    if (tab == NULL) {
        return -1;
    }

    self = PP_hashtab_newobj(self, tab);
    if (self == NULL)
        return -1;

    return 0;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_hashtab_Check - */

/* static */ int
PP_hashtab_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_hashtab_Type))
        return 1;
    else
        return 0;
/*  return op->ob_type == &PP_hashtab_Type; */
}

/*--------------------------------------------------------------------------*/
/*                           OBJECT_TP_AS_MAPPING                           */
/*--------------------------------------------------------------------------*/

/* Code to access hashtab objects as mappings */

static Py_ssize_t
PP_hashtab_mp_length(PyObject *_self)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.as_mapping.length) */
    PP_hashtabObject *self = (PP_hashtabObject *) _self;
    return (Py_ssize_t) SC_hasharr_get_n(self->data);
/* DO-NOT-DELETE splicer.end(pdb.hashtab.as_mapping.length) */
}

static PyObject *
PP_hashtab_mp_subscript(PyObject *_self, PyObject *key)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.as_mapping.subscript) */
    char *name;
    haelem *hp;
    hasharr *hashtab;
    PyObject *rv;
    PP_hashtabObject *self = (PP_hashtabObject *) _self;

    if (!PyString_Check(key)) {
        PP_error_set_user(key, "key must be string");
        return NULL;
    }
    name = PyString_AsString(key);

    hashtab = self->data;
    if (hashtab == NULL) {
        PP_error_set(PP_error_internal,
                     (PyObject *) self, "hashtab is NULL");
        return NULL;
    }

    hp = SC_hasharr_lookup(hashtab, name);
    if (hp == NULL) {
        PyErr_SetObject(PyExc_KeyError, key);
        return NULL;
    }

    rv = PP_unpack_hashtab_haelem(hp->type, hp->def);

    return rv;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.as_mapping.subscript) */
}

static int
PP_hashtab_mp_ass_subscript(PyObject *_self, PyObject *key, PyObject *v)
{
/* DO-NOT-DELETE splicer.begin(pdb.hashtab.as_mapping.ass_subscript) */
    int ok;
    char *name, *ptype;
    hasharr *hashtab;
    PP_hashtabObject *self = (PP_hashtabObject *) _self;
    
    if (!PyString_Check(key)) {
        PP_error_set_user(key, "key must be string");
        return -1;
    }
    name = PyString_AsString(key);

    hashtab = self->data;
    if (hashtab == NULL) {
        PP_error_set(PP_error_internal,
                     (PyObject *) self, "hashtab is NULL");
        return -1;
    }

    if (v == NULL) {
        ok = SC_hasharr_remove(hashtab, name);
        /* XXX - remove data too */
        if (ok != TRUE) {
            PP_error_set(PP_error_internal, NULL, "error removing object");
            return -1;
        }
    } else {
        PP_descr *descr;
        void *vr;
        haelem *hp;

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
        SC_mark(vr, 1);

        ptype = PP_add_indirection(descr->type, 1);
        if (ptype == NULL)
            return -1;

        hp = SC_hasharr_install(hashtab, name, vr, ptype, 3, -1);

        /* XXX release vr as well */
        _PP_rl_descr(descr);

        if (hp == NULL) {
            PP_error_set(PP_error_internal, NULL, "Error from SC_hasharr_install");
            return -1;
        }
    }

    return 0;
/* DO-NOT-DELETE splicer.end(pdb.hashtab.as_mapping.ass_subscript) */
}


static PyMappingMethods PP_hashtab_as_mapping = {
        PP_hashtab_mp_length,           /* mp_length */
        PP_hashtab_mp_subscript,        /* mp_subscript */
        PP_hashtab_mp_ass_subscript,    /* mp_ass_subscript */
};

/*--------------------------------------------------------------------------*/


static char PP_hashtab_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_hashtab_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        0,                              /* ob_size */
        "hashtab",                       /* tp_name */
        sizeof(PP_hashtabObject),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PP_hashtab_tp_dealloc, /* tp_dealloc */
        (printfunc)0,                   /* tp_print */
        (getattrfunc)0,                 /* tp_getattr */
        (setattrfunc)0,                 /* tp_setattr */
        (cmpfunc)0,                     /* tp_compare */
        (reprfunc)PP_hashtab_tp_repr,   /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        0,                              /* tp_as_sequence */
        &PP_hashtab_as_mapping,         /* tp_as_mapping */
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
        PP_hashtab_Type__doc__,         /* tp_doc */
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
        PP_hashtab_methods,             /* tp_methods */
        0,                              /* tp_members */
        PP_hashtab_getset,              /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_hashtab_tp_init,   /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.hashtab.extra) */

PP_hashtabObject *PP_hashtab_newobj(PP_hashtabObject *obj,
                                    hasharr *tab)
{
    PP_defstrObject *dpobj;
    
    if (obj == NULL) {
        obj = (PP_hashtabObject *) PyType_GenericAlloc(&PP_hashtab_Type, 0);
        if (obj == NULL) {
            return NULL;
        }
    }

    dpobj = _PP_defstr_find_singleton("hasharr", NULL, PP_vif_info);
    if (dpobj == NULL)
        return NULL;

    obj = (PP_hashtabObject *) PP_pdbdata_newobj(
        (PP_pdbdataObject *) obj, tab, dpobj->dp->type, 1L, NULL,
        dpobj->dp, dpobj->fileinfo, dpobj, NULL);

    return obj;
}

/* DO-NOT-DELETE splicer.end(pdb.hashtab.extra) */
/* End of code for hashtab objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
