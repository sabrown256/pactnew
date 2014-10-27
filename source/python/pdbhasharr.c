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

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_install[] = "";

static PyObject *PY_hasharr_install(PY_hasharr *self,
				    PyObject *args,
				    PyObject *kwds)
{
    char *key, *obj, *type;
    char *kw_list[] = {"key", "obj", "type", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss:install", kw_list,
                                     &key, &obj, &type))
        return NULL;
    SC_hasharr_install(self->pyo, key, obj, type, 3, -1);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_def_lookup[] = "";

static PyObject *PY_hasharr_def_lookup(PY_hasharr *self,
				       PyObject *args,
				       PyObject *kwds)
{
  char *key;
  char *kw_list[] = {"key", NULL};
  hasharr *tab;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:def_lookup", kw_list,
                                   &key))
      return NULL;
  tab = (hasharr *) self->pyo;
  SC_hasharr_def_lookup(tab, key);
  Py_INCREF(Py_None);
  return Py_None;
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

static char PY_hasharr_doc_clear[] = "";

static PyObject *PY_hasharr_clear(PY_hasharr *self,
				  PyObject *args,
				  PyObject *kwds)
{
    SC_hasharr_foreach(self->pyo, _PP_rl_haelem, NULL);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_has_key[] = "";

static PyObject *PY_hasharr_has_key(PY_hasharr *self,
				    PyObject *args,
				    PyObject *kwds)
{
    long ok;
    char *key;
    char *kw_list[] = {"key", NULL};
    haelem *np;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:has_key", kw_list,
                                     &key))
        return NULL;

    np = SC_hasharr_lookup(self->pyo, key);
    ok = np != NULL;

#if PYTHON_API_VERSION < 1012
    return PY_INT_LONG(ok);
#else
    return PyBool_FromLong(ok);
#endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_items[] = "";

static PyObject *
PY_hasharr_items(PY_hasharr *self,
                  PyObject *args,
                  PyObject *kwds)
{
    PyErr_SetString(PyExc_NotImplementedError, "items");
    return NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_keys[] = "";

static PyObject *PY_hasharr_keys(PY_hasharr *self,
				 PyObject *args,
				 PyObject *kwds)
{
    int ne, err;
    char **names;
    hasharr *ha;
    Py_ssize_t i;
    PyObject *rv;
 
    ha = self->pyo;
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
        err = PyTuple_SetItem(rv, i, PY_STRING_STRING(names[i]));
        if (err < 0) {
            Py_DECREF(rv);
            rv = NULL;
            break;
        }
    }
    SC_free_strings(names);
  
    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_update[] = "";

static PyObject *PY_hasharr_update(PY_hasharr *self,
				   PyObject *args,
				   PyObject *kwds)
{
    int err;
    PyObject *dict;
    char *kw_list[] = {"dict", NULL};
    hasharr *tab;

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O", kw_list,
                                    &dict))
        return NULL;

    tab = (hasharr *) self->pyo;
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
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_values[] = "";

static PyObject *PY_hasharr_values(PY_hasharr *self,
				   PyObject *args,
				   PyObject *kwds)
{
    return NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_get[] = "";

static PyObject *PY_hasharr_get(PY_hasharr *self,
				PyObject *args,
				PyObject *kwds)
{
    PyErr_SetString(PyExc_NotImplementedError, "get");
    return NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMethodDef PY_hasharr_methods[] = {
{"install", (PyCFunction)PY_hasharr_install, METH_KEYWORDS, PY_hasharr_doc_install},
{"def_lookup", (PyCFunction)PY_hasharr_def_lookup, METH_KEYWORDS, PY_hasharr_doc_def_lookup},
{"clear", (PyCFunction)PY_hasharr_clear, METH_NOARGS, PY_hasharr_doc_clear},
{"has_key", (PyCFunction)PY_hasharr_has_key, METH_KEYWORDS, PY_hasharr_doc_has_key},
{"items", (PyCFunction)PY_hasharr_items, METH_NOARGS, PY_hasharr_doc_items},
{"keys", (PyCFunction)PY_hasharr_keys, METH_NOARGS, PY_hasharr_doc_keys},
{"update", (PyCFunction)PY_hasharr_update, METH_KEYWORDS, PY_hasharr_doc_update},
{"values", (PyCFunction)PY_hasharr_values, METH_NOARGS, PY_hasharr_doc_values},
{"get", (PyCFunction)PY_hasharr_get, METH_NOARGS, PY_hasharr_doc_get},
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyGetSetDef PY_hasharr_getset[] = {
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void PY_hasharr_tp_dealloc(PY_hasharr *self)
{
    int n;

    n = SC_mark(self->pyo, 0);
    if (n < 2) {
	SC_free_hasharr(self->pyo, _PP_rl_haelem, NULL);
        self->pyo = NULL;
    } else {
        SC_mark(self->pyo, -1);
    }
    PY_TYPE(self)->tp_free((PyObject*)self);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_hasharr_tp_repr(PY_hasharr *self)
{
    PyObject *rv, *h;

    h = _PP_unpack_hasharr(self->pyo, 1L);
    if (h == NULL)
        return NULL;
    rv = PyObject_Repr(h);
    Py_DECREF(h);
    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_hasharr_tp_init(PY_hasharr *self,
			      PyObject *args, PyObject *kwds)
{

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

    self = PY_hasharr_newobj(self, tab);
    if (self == NULL)
        return -1;

    return 0;
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
        keyname = PY_STRING_AS_STRING(keyitem);
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

static Py_ssize_t PY_hasharr_mp_length(PyObject *_self)
{
    int ne;
    PY_hasharr *self = (PY_hasharr *) _self;

    ne = SC_hasharr_get_n(self->pyo);

    return (Py_ssize_t) ne;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_hasharr_mp_subscript(PyObject *_self, PyObject *key)
{
    char *name;
    haelem *hp;
    hasharr *harr;
    PyObject *rv;
    PY_hasharr *self = (PY_hasharr *) _self;

    if (!PY_STRING_CHECK(key)) {
        PP_error_set_user(key, "key must be string");
        return NULL;
    }
    name = PY_STRING_AS_STRING(key);

    harr = self->pyo;
    if (harr == NULL) {
        PP_error_set(PP_error_internal,
                     (PyObject *) self, "hasharr is NULL");
        return NULL;
    }

    hp = SC_hasharr_lookup(harr, name);
    if (hp == NULL) {
        PyErr_SetObject(PyExc_KeyError, key);
        return NULL;
    }

    rv = PP_unpack_hasharr_haelem(hp->type, hp->def);

    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_hasharr_mp_ass_subscript(PyObject *_self,
				       PyObject *key, PyObject *v)
{
    int ok;
    char *name, *ptype;
    hasharr *harr;
    PY_hasharr *self = (PY_hasharr *) _self;
    
    if (!PY_STRING_CHECK(key)) {
        PP_error_set_user(key, "key must be string");
        return -1;
    }
    name = PY_STRING_AS_STRING(key);

    harr = self->pyo;
    if (harr == NULL) {
        PP_error_set(PP_error_internal,
                     (PyObject *) self, "hasharr is NULL");
        return -1;
    }

    if (v == NULL) {
        ok = SC_hasharr_remove(harr, name);
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

        hp = SC_hasharr_install(harr, name, vr, ptype, 3, -1);

        /* XXX release vr as well */
        _PP_rl_descr(descr);

        if (hp == NULL) {
            PP_error_set(PP_error_internal, NULL, "Error from SC_hasharr_install");
            return -1;
        }
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMappingMethods PY_hasharr_as_mapping = {
        PY_hasharr_mp_length,           /* mp_length */
        PY_hasharr_mp_subscript,        /* mp_subscript */
        PY_hasharr_mp_ass_subscript,    /* mp_ass_subscript */
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc[] = "";

#define PY_DEF_DESTRUCTOR	    PY_hasharr_tp_dealloc
#define PY_DEF_REPR                 PY_hasharr_tp_repr
#define PY_DEF_TP_METH              PY_hasharr_methods
#define PY_DEF_AS_MAP	            &PY_hasharr_as_mapping

PY_DEF_TYPE(hasharr);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PY_hasharr *PY_hasharr_newobj(PY_hasharr *obj,
			      hasharr *tab)
{
    PP_defstrObject *dpobj;
    
    if (obj == NULL) {
        obj = (PY_hasharr *) PyType_GenericAlloc(&PY_hasharr_type, 0);
        if (obj == NULL) {
            return NULL;
        }
    }

    dpobj = _PP_defstr_find_singleton("HASHARR", NULL, PP_vif_info);
    if (dpobj == NULL)
        return NULL;

    obj = (PY_hasharr *) PP_pdbdata_newobj(
        (PP_pdbdataObject *) obj, tab, dpobj->dp->type, 1L, NULL,
        dpobj->dp, dpobj->fileinfo, dpobj, NULL);

    return obj;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
