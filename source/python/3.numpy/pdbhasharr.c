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

/* DO-NOT-DELETE splicer.begin(pdb.hasharr.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.hasharr.C_definition) */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hasharr_install__doc__[] = 
""
;

static PyObject *
PP_hasharr_install(PP_hasharrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.method.install) */
    char *key, *obj, *type;
    char *kw_list[] = {"key", "obj", "type", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss:install", kw_list,
                                     &key, &obj, &type))
        return NULL;
    SC_hasharr_install(self->data, key, obj, type, 3, -1);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.method.install) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hasharr_def_lookup__doc__[] = 
""
;

static PyObject *
PP_hasharr_def_lookup(PP_hasharrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.method.def_lookup) */
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
/* DO-NOT-DELETE splicer.end(pdb.hasharr.method.def_lookup) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int _PP_rl_haelem(haelem *hp, void *a)
{

    (void) _PP_rel_syment(PP_vif->host_chart, (char *) &hp->def, 1L, hp->type);
    CFREE(hp->type);
    CFREE(hp->def);

    return TRUE;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hasharr_clear__doc__[] = 
""
;

static PyObject *
PP_hasharr_clear(PP_hasharrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.method.clear) */
    SC_hasharr_foreach(self->data, _PP_rl_haelem, NULL);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.method.clear) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hasharr_has_key__doc__[] = 
""
;

static PyObject *
PP_hasharr_has_key(PP_hasharrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.method.has_key) */
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
/* DO-NOT-DELETE splicer.end(pdb.hasharr.method.has_key) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hasharr_items__doc__[] = 
""
;

static PyObject *
PP_hasharr_items(PP_hasharrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.method.items) */
    PyErr_SetString(PyExc_NotImplementedError, "items");
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.method.items) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hasharr_keys__doc__[] = 
""
;

static PyObject *
PP_hasharr_keys(PP_hasharrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.method.keys) */
    int ne, err;
    char **names;
    hasharr *ha;
    Py_ssize_t i;
    PyObject *rv;
 
    ha = self->data;
    if (ha == NULL) {
        PP_error_set(PP_error_internal,
                     NULL, "Hasharr is NULL");
        return NULL;
    }

    ne = SC_hasharr_get_n(ha);
    if (ne == 0) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    rv = PyTuple_New(ne);
    if (rv == NULL)
        return NULL;
    names = SC_hasharr_dump(ha, NULL, NULL, FALSE);
    for (i = 0; i < ne; i++) {
        err = PyTuple_SetItem(rv, i, PyString_FromString(names[i]));
        if (err < 0) {
            Py_DECREF(rv);
            rv = NULL;
            break;
        }
    }
    SC_free_strings(names);
  
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.method.keys) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hasharr_update__doc__[] = 
""
;

static PyObject *
PP_hasharr_update(PP_hasharrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.method.update) */
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
                     (PyObject *) self, "Hasharr is NULL");
        return NULL;
    }

    err = PP_update_hasharr(tab, dict);
    if (err < 0)
        return NULL;

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.method.update) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hasharr_values__doc__[] = 
""
;

static PyObject *
PP_hasharr_values(PP_hasharrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.method.values) */
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.method.values) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_hasharr_get__doc__[] = 
""
;

static PyObject *
PP_hasharr_get(PP_hasharrObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.method.get) */
    PyErr_SetString(PyExc_NotImplementedError, "get");
    return NULL;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.method.get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.hasharr.extra_methods) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.hasharr.extra_methods) */

/*--------------------------------------------------------------------------*/

static PyMethodDef PP_hasharr_methods[] = {
{"install", (PyCFunction)PP_hasharr_install, METH_KEYWORDS, PP_hasharr_install__doc__},
{"def_lookup", (PyCFunction)PP_hasharr_def_lookup, METH_KEYWORDS, PP_hasharr_def_lookup__doc__},
{"clear", (PyCFunction)PP_hasharr_clear, METH_NOARGS, PP_hasharr_clear__doc__},
{"has_key", (PyCFunction)PP_hasharr_has_key, METH_KEYWORDS, PP_hasharr_has_key__doc__},
{"items", (PyCFunction)PP_hasharr_items, METH_NOARGS, PP_hasharr_items__doc__},
{"keys", (PyCFunction)PP_hasharr_keys, METH_NOARGS, PP_hasharr_keys__doc__},
{"update", (PyCFunction)PP_hasharr_update, METH_KEYWORDS, PP_hasharr_update__doc__},
{"values", (PyCFunction)PP_hasharr_values, METH_NOARGS, PP_hasharr_values__doc__},
{"get", (PyCFunction)PP_hasharr_get, METH_NOARGS, PP_hasharr_get__doc__},
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.extra_mlist) */
/* DO-NOT-DELETE splicer.end(pdb.hasharr.extra_mlist) */
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.hasharr.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.hasharr.extra_members) */

static PyGetSetDef PP_hasharr_getset[] = {

/* DO-NOT-DELETE splicer.begin(pdb.hasharr.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.hasharr.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static void
PP_hasharr_tp_dealloc(PP_hasharrObject *self)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.as_type.dealloc) */
    int n;

    n = SC_mark(self->data, 0);
    if (n < 2) {
	SC_free_hasharr(self->data, _PP_rl_haelem, NULL);
        self->data = NULL;
    } else {
        SC_mark(self->data, -1);
    }
    self->ob_type->tp_free((PyObject*)self);
/* DO-NOT-DELETE splicer.end(pdb.hasharr.as_type.dealloc) */
}

static PyObject *
PP_hasharr_tp_repr(PP_hasharrObject *self)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.as_type.repr) */
    PyObject *rv, *h;

    h = _PP_unpack_hasharr(self->data, 1L);
    if (h == NULL)
        return NULL;
    rv = PyObject_Repr(h);
    Py_DECREF(h);
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.as_type.repr) */
}

static int
PP_hasharr_tp_init(PP_hasharrObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.as_type.init) */

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

    self = PP_hasharr_newobj(self, tab);
    if (self == NULL)
        return -1;

    return 0;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_hasharr_Check - */

/* static */ int
PP_hasharr_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_hasharr_Type))
        return 1;
    else
        return 0;
/*  return op->ob_type == &PP_hasharr_Type; */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_unpack_hasharr - */

PyObject *_PP_unpack_hasharr(void *p, long nitems)
{
    int ierr;
    long i;
    char *nm;
    PyObject *dict, *item;
    hasharr *tab;

    ierr = 0;
    tab = (hasharr *) p;
    if (tab == NULL) {
        Py_INCREF(Py_None);
        dict = Py_None;
        return dict;
    }

    dict = PyDict_New();
    if (dict == NULL)
        return NULL;

    for (i = 0; SC_hasharr_next(tab, &i, &nm, NULL, (void **) &item); i++) {
        if (item == NULL) {
	  ierr = -1;
	  break;
	}
	ierr = PyDict_SetItemString(dict, nm, item);
	if (ierr < 0)
	  break;
      }


    if (ierr < 0) {
        Py_DECREF(dict);
        dict = NULL;
    }
    
    return dict;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_unpack_hasharr_haelem - */

/* PyObject *PP_unpack_hasharr_haelem(haelem *hp)*/
PyObject *PP_unpack_hasharr_haelem(char *type, void *vr)
{
    intb bpi;
    inti nitems;
    dimdes *dims;
    PyObject *rv;
    PP_form form;

    form = PP_global_form;

    type = SC_dereference(CSTRSAVE(type));
    bpi = _PD_lookup_size(type, PP_vif->host_chart);
    if (bpi < 0)
        return NULL;
    nitems = SC_arrlen(vr) / bpi;
    if (nitems > 1)
        dims = _PD_mk_dimensions(0, nitems);
    else
        dims = NULL;
    rv = _PP_wr_syment(PP_vif_info, type, dims, nitems, vr, &form);
    CFREE(type);
    _PD_rl_dimensions(dims);

    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_update_hasharr - update a hasharr from a dictionary.
 *   Note: hasharrs also support the dictionary protocol.
 */

int PP_update_hasharr(hasharr *tab, PyObject *dict)
{
    int i;
    int nkeys;
    int err;
    char *keyname;
    char *ptype;
    void *vr;
    PP_descr *descr;
    haelem *entry;
    PyObject *keys, *keyitem, *value;

    err = 0;
    descr = NULL;
    value = NULL;
    keys = PyMapping_Keys(dict);
    if (keys == NULL)
        return -1;
    nkeys = PyList_GET_SIZE(keys);
    for (i = 0; i < nkeys; i++) {
        keyitem = PyList_GET_ITEM(keys, i);
        keyname = PyString_AS_STRING(keyitem);
        value = PyMapping_GetItemString(dict, keyname);

        descr = PP_get_object_descr(PP_vif_info, value);
        if (descr == NULL) {
            err = -1;
            break;
        }

        if (descr->data == NULL) {
            err = PP_make_data(value, PP_vif_info, descr->type, descr->dims, &vr);
            if (err == -1) {
                break;
            }
        } else {
            vr = descr->data;
        }

        ptype = PP_add_indirection(descr->type, 1);
        if (ptype == NULL) {
            err = -1;
            break;
        }

        /* SC_hasharr_install will SC_mark vr */
        entry = SC_hasharr_install(tab, keyname, vr, ptype, 3, -1);
        if (entry == NULL) {
            err = -1;
            break;
        }

        _PP_rl_descr(descr);
        descr = NULL;
        Py_XDECREF(value);
        value = NULL;
    }

    if (descr != NULL)
        _PP_rl_descr(descr);
    Py_XDECREF(value);
    Py_DECREF(keys);

    return err;
}

/*--------------------------------------------------------------------------*/
/*                           OBJECT_TP_AS_MAPPING                           */
/*--------------------------------------------------------------------------*/

/* Code to access hasharr objects as mappings */

static Py_ssize_t
PP_hasharr_mp_length(PyObject *_self)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.as_mapping.length) */
    int ne;
    PP_hasharrObject *self = (PP_hasharrObject *) _self;

    ne = SC_hasharr_get_n(self->data);

    return (Py_ssize_t) ne;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.as_mapping.length) */
}

static PyObject *
PP_hasharr_mp_subscript(PyObject *_self, PyObject *key)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.as_mapping.subscript) */
    char *name;
    haelem *hp;
    hasharr *hasharr;
    PyObject *rv;
    PP_hasharrObject *self = (PP_hasharrObject *) _self;

    if (!PyString_Check(key)) {
        PP_error_set_user(key, "key must be string");
        return NULL;
    }
    name = PyString_AsString(key);

    hasharr = self->data;
    if (hasharr == NULL) {
        PP_error_set(PP_error_internal,
                     (PyObject *) self, "hasharr is NULL");
        return NULL;
    }

    hp = SC_hasharr_lookup(hasharr, name);
    if (hp == NULL) {
        PyErr_SetObject(PyExc_KeyError, key);
        return NULL;
    }

    rv = PP_unpack_hasharr_haelem(hp->type, hp->def);

    return rv;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.as_mapping.subscript) */
}

static int
PP_hasharr_mp_ass_subscript(PyObject *_self, PyObject *key, PyObject *v)
{
/* DO-NOT-DELETE splicer.begin(pdb.hasharr.as_mapping.ass_subscript) */
    int ok;
    char *name, *ptype;
    hasharr *hasharr;
    PP_hasharrObject *self = (PP_hasharrObject *) _self;
    
    if (!PyString_Check(key)) {
        PP_error_set_user(key, "key must be string");
        return -1;
    }
    name = PyString_AsString(key);

    hasharr = self->data;
    if (hasharr == NULL) {
        PP_error_set(PP_error_internal,
                     (PyObject *) self, "hasharr is NULL");
        return -1;
    }

    if (v == NULL) {
        ok = SC_hasharr_remove(hasharr, name);
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

        hp = SC_hasharr_install(hasharr, name, vr, ptype, 3, -1);

        /* XXX release vr as well */
        _PP_rl_descr(descr);

        if (hp == NULL) {
            PP_error_set(PP_error_internal, NULL, "Error from SC_hasharr_install");
            return -1;
        }
    }

    return 0;
/* DO-NOT-DELETE splicer.end(pdb.hasharr.as_mapping.ass_subscript) */
}


static PyMappingMethods PP_hasharr_as_mapping = {
        PP_hasharr_mp_length,           /* mp_length */
        PP_hasharr_mp_subscript,        /* mp_subscript */
        PP_hasharr_mp_ass_subscript,    /* mp_ass_subscript */
};

/*--------------------------------------------------------------------------*/


static char PP_hasharr_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_hasharr_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        0,                              /* ob_size */
        "hasharr",                       /* tp_name */
        sizeof(PP_hasharrObject),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PP_hasharr_tp_dealloc, /* tp_dealloc */
        (printfunc)0,                   /* tp_print */
        (getattrfunc)0,                 /* tp_getattr */
        (setattrfunc)0,                 /* tp_setattr */
        (cmpfunc)0,                     /* tp_compare */
        (reprfunc)PP_hasharr_tp_repr,   /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        0,                              /* tp_as_sequence */
        &PP_hasharr_as_mapping,         /* tp_as_mapping */
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
        PP_hasharr_Type__doc__,         /* tp_doc */
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
        PP_hasharr_methods,             /* tp_methods */
        0,                              /* tp_members */
        PP_hasharr_getset,              /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_hasharr_tp_init,   /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.hasharr.extra) */

PP_hasharrObject *PP_hasharr_newobj(PP_hasharrObject *obj,
                                    hasharr *tab)
{
    PP_defstrObject *dpobj;
    
    if (obj == NULL) {
        obj = (PP_hasharrObject *) PyType_GenericAlloc(&PP_hasharr_Type, 0);
        if (obj == NULL) {
            return NULL;
        }
    }

    dpobj = _PP_defstr_find_singleton("HASHARR", NULL, PP_vif_info);
    if (dpobj == NULL)
        return NULL;

    obj = (PP_hasharrObject *) PP_pdbdata_newobj(
        (PP_pdbdataObject *) obj, tab, dpobj->dp->type, 1L, NULL,
        dpobj->dp, dpobj->fileinfo, dpobj, NULL);

    return obj;
}

/* DO-NOT-DELETE splicer.end(pdb.hasharr.extra) */
/* End of code for hasharr objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
