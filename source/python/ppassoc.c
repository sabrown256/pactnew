/*
 * PPASSOC.C
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#define PY_DEF_DESTRUCTOR	    PY_pcons_tp_dealloc
#define PY_DEF_REPR                 PY_pcons_tp_repr
#define PY_DEF_TP_METH              PY_pcons_methods
#define PY_DEF_AS_MAP	            &PY_pcons_as_mapping

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/

/*                              PCONS ROUTINES                              */

/*--------------------------------------------------------------------------*/

PY_DEF_EXTRACTOR(pcons)

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_OPT_PCONS - handle BLANG binding related operations */

void *_PY_opt_pcons(pcons *x, bind_opt wh, void *a)
   {void *rv;

    rv = NULL;
    switch (wh)
       {case BIND_ALLOC :
	     SC_mark(x, 1);
	     break;
        case BIND_ARG :
        case BIND_LABEL :
        case BIND_PRINT :
        case BIND_FREE :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0
/* _PY_PCONS_PACK - */

static int _PY_pcons_pack(void *p, PyObject *v, long nitems, PP_types tc)
   {

    PP_error_set(PP_error_internal, NULL, "_PY_pcons_pack");

    return(-1);}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_PCONS_UNPACK - */

static PyObject *_PY_pcons_unpack(void *p, long nitems)
   {int i, ierr;
    char *name;
    PyObject *dict, *item;
    pcons *alist, *pa, *c;

    ierr  = 0;
    alist = (pcons *) p;
    
    dict = PyDict_New();
    if (dict == NULL)
       return(NULL);

    for (pa = alist, i = 0; pa != NULL; pa = (pcons *) pa->cdr, i++)
        {c    = (pcons *) pa->car;
	 name = (char *) c->car;
	 item = PP_unpack_hasharr_haelem(c->cdr_type, c->cdr);
	 if (item == NULL)
	    {ierr = -1;
	     break;};

	 ierr = PyDict_SetItemString(dict, name, item);
	 if (ierr < 0)
            break;};

    if (ierr < 0)
       {Py_DECREF(dict);
        dict = NULL;};
    
    return(dict);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_PCONS_UPDATE - update an pcons list from a dictionary.
 *   Note: pcons also support the dictionary protocol.
 */

static int _PY_pcons_update(pcons **in, PyObject *dict)
   {int i, nkeys, err;
    char *keyname, *ptype;
    void *vr;
    PP_descr *descr;
    pcons *alist;
    PyObject *keys, *keyitem, *value;

    descr = NULL;
    value = NULL;
    
    alist = *in;
    err   = 0;
    keys  = PyMapping_Keys(dict);
    if (keys == NULL)
       return(-1);

    nkeys = PyList_GET_SIZE(keys);
    for (i = 0; i < nkeys; i++)
        {keyitem = PyList_GET_ITEM(keys, i);
	 keyname = PY_STRING_AS_STRING(keyitem);
	 value = PyMapping_GetItemString(dict, keyname);

	 descr = PP_get_object_descr(PP_vif_info, value);
	 if (descr == NULL)
	    {err = -1;
            break;};

	 if (descr->data == NULL)
	    {err = PP_make_data(value, PP_vif_info, descr->type, descr->dims, &vr);
	     if (err == -1)
                break;}
	 else
            vr = descr->data;

	 ptype = PP_add_indirection(descr->type, 1);
	 if (ptype == NULL)
	    {err = -1;
	     break;};

	 alist = SC_add_alist(alist, keyname, ptype, vr);
	 if (alist == NULL)
	    {err = -1;
	     break;};

	 CFREE(ptype); /* SC_add_alist copies */
	 _PP_rl_descr(descr);
	 descr = NULL;
	 Py_XDECREF(value);
	 value = NULL;};

    *in = alist;
    if (descr != NULL)
        _PP_rl_descr(descr);

    Py_XDECREF(value);
    Py_DECREF(keys);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_pcons_clear_doc[] = "";

static PyObject *PY_pcons_clear(PY_pcons *self,
				PyObject *args,
				PyObject *kwds)
   {

#if 0
    PD_clear(self->pyo);
#endif

    Py_INCREF(Py_None);

    return(Py_None);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_pcons_has_key_doc[] = "";

static PyObject *PY_pcons_has_key(PY_pcons *self,
				  PyObject *args,
				  PyObject *kwds)
   {long ok;
    char *key;
    pcons *alist, *pa;
    PyObject *rv;
    char *kw_list[] = {"key", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds, "s:has_key", kw_list,
				    &key))
       {alist = self->pyo;
	if (alist == NULL)
	   ok = 0;
	else
	   {pa = SC_assoc_entry(alist, key);
	    ok = (pa != NULL);};

#if PYTHON_API_VERSION < 1012
	rv = PY_INT_LONG(ok);
#else
	rv = PyBool_FromLong(ok);
#endif
	};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_pcons_items_doc[] = "";

static PyObject *PY_pcons_items(PY_pcons *self,
				PyObject *args,
				PyObject *kwds)
   {char *s;
    pcons *pa, *c;
    Py_ssize_t nitems, i;
    PyObject *rv, *item, *obj;

    nitems = 0;
    for (pa = self->pyo; pa != NULL; pa = (pcons *) pa->cdr)
        nitems++;

    rv = PyList_New(nitems);
    if (rv == NULL)
       return(NULL);

    for (pa = self->pyo, i = 0; pa != NULL; pa = (pcons *) pa->cdr, i++)
        {c = (pcons *) pa->car;
	 s = (char *) c->car;
	 item = PyTuple_New(2);
	 if (item == NULL)
	    {item = NULL;
	     break;};

	 obj = PY_STRING_STRING(s);
	 PyTuple_SET_ITEM(item, 0, obj);
	 obj = PP_unpack_hasharr_haelem(c->cdr_type, c->cdr);
	 PyTuple_SET_ITEM(item, 1, obj);
        
	 PyList_SET_ITEM(rv, i, item);};
        
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_pcons_keys_doc[] = "";

static PyObject *PY_pcons_keys(PY_pcons *self,
			       PyObject *args,
			       PyObject *kwds)
   {int err;
    char *s;
    pcons *pa, *c;
    Py_ssize_t nitems, i;
    PyObject *rv;

    nitems = 0;
    for (pa = self->pyo; pa != NULL; pa = (pcons *) pa->cdr)
        nitems++;

    rv = PyTuple_New(nitems);
    if (rv == NULL)
       return(NULL);

    for (pa = self->pyo, i = 0; pa != NULL; pa = (pcons *) pa->cdr, i++)
        {c = (pcons *) pa->car;
	 s = (char *) c->car;
	 err = PyTuple_SetItem(rv, i, PY_STRING_STRING(s));
	 if (err < 0)
	    {Py_DECREF(rv);
	     rv = NULL;
	     break;};};
        
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_pcons_update_doc[] = "";

static PyObject *PY_pcons_update(PY_pcons *self,
				 PyObject *args,
				 PyObject *kwds)
   {int err;
    pcons *alist;
    PyObject *dict;
    char *kw_list[] = {"dict", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O:update", kw_list,
                                     &dict))
        return(NULL);

    alist = self->pyo;
    err = _PY_pcons_update(&alist, dict);
    if (err < 0)
        return(NULL);
    self->pyo = alist;

    Py_INCREF(Py_None);

    return(Py_None);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_pcons_values_doc[] = "";

static PyObject *PY_pcons_values(PY_pcons *self,
				 PyObject *args,
				 PyObject *kwds)
   {

    PyErr_SetString(PyExc_NotImplementedError, "values");

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_pcons_get_doc[] = "";

static PyObject *PY_pcons_get(PY_pcons *self,
			      PyObject *args,
			      PyObject *kwds)
   {

    PyErr_SetString(PyExc_NotImplementedError, "get");

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMethodDef
 PY_pcons_methods[] = {
   {"clear", (PyCFunction)PY_pcons_clear, METH_NOARGS, PY_pcons_clear_doc},
   {"has_key", (PyCFunction)PY_pcons_has_key, METH_KEYWORDS, PY_pcons_has_key_doc},
   {"items", (PyCFunction)PY_pcons_items, METH_NOARGS, PY_pcons_items_doc},
   {"keys", (PyCFunction)PY_pcons_keys, METH_NOARGS, PY_pcons_keys_doc},
   {"update", (PyCFunction)PY_pcons_update, METH_KEYWORDS, PY_pcons_update_doc},
   {"values", (PyCFunction)PY_pcons_values, METH_NOARGS, PY_pcons_values_doc},
   {"get", (PyCFunction)PY_pcons_get, METH_NOARGS, PY_pcons_get_doc},
   {NULL, NULL, 0, NULL}};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void PY_pcons_tp_dealloc(PY_pcons *self)
   {

    if (SC_safe_to_free(self->pyo))
        SC_free_alist(self->pyo, 3);

    PY_self_free(self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_pcons_tp_repr(PY_pcons *self)
   {PyObject *rv, *h;

    h = _PY_pcons_unpack(self->pyo, 1L);
    if (h == NULL)
        return(NULL);
    rv = PyObject_Repr(h);
    Py_DECREF(h);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_pcons_tp_init(PY_pcons *self, PyObject *args, PyObject *kwds)
   {

    self->pyo = NULL;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Code to access pcons objects as mappings */

static Py_ssize_t PY_pcons_mp_length(PyObject *_self)
   {Py_ssize_t i;
    pcons *pa;
    PY_pcons *self = (PY_pcons *) _self;

    i = 0;
    for (pa = self->pyo; pa != NULL; pa = (pcons *) pa->cdr)
        i++;
    
    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_pcons_mp_subscript(PyObject *_self, PyObject *key)
   {char *name;
    pcons *alist, *pa;
    PyObject *rv;
    PY_pcons *self = (PY_pcons *) _self;

    if (!PY_STRING_CHECK(key))
       {PP_error_set_user(key, "key must be string");
        return(NULL);};

    name = PY_STRING_AS_STRING(key);

    alist = self->pyo;
    if (alist == NULL)
       {PyErr_SetString(PyExc_KeyError, name);
        return(NULL);};

    pa = SC_assoc_entry(alist, name);
    if (pa == NULL)
       {PyErr_SetString(PyExc_KeyError, name);
        return(NULL);};

    rv = PP_unpack_hasharr_haelem(pa->cdr_type, pa->cdr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_pcons_mp_ass_subscript(PyObject *_self,
				     PyObject *key, PyObject *v)
   {char *name;
    pcons *alist;
    PY_pcons *self = (PY_pcons *) _self;
    
#if 0
    if (!PY_STRING_CHECK(key))
       {PP_user_error(PP_score_errorObject, key, "key must be string");
        return(-1);};
#endif

    name = PY_STRING_AS_STRING(key);
    
    alist = self->pyo;

    if (v == NULL)

/* delete item */
       {if (alist == NULL)
	   PyErr_SetString(PyExc_KeyError, name);

        alist = SC_rem_alist(alist, name);}

    else
       {int ok;
        char *ptype;
        PP_descr *descr;
        void *vr;

        descr = PP_get_object_descr(PP_vif_info, v);
        if (descr == NULL)
           return(-1);

        if (descr->data == NULL)
	   {ok = PP_make_data(v, PP_vif_info, descr->type, descr->dims, &vr);
            if (ok == -1)
               return(-1);}
	else
           vr = descr->data;

/*        SC_mark(vr, 1);*/

        ptype = PP_add_indirection(descr->type, 1);
        if (ptype == NULL)
           return(-1);

/* remove the item first, if it exists */
        if (alist != NULL)
	   alist = SC_rem_alist(alist, name);

/* SC_add_alist will CSTRSAVE ptype and SC_mark vr */
        alist = SC_add_alist(alist, name, ptype, vr);

        CFREE(ptype);
        _PP_rl_descr(descr);

        if (alist == NULL)
           return(-1);};

    self->pyo = alist;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMappingMethods
 PY_pcons_as_mapping = {PY_pcons_mp_length,             /* mp_length */
                        PY_pcons_mp_subscript,          /* mp_subscript */
                        PY_pcons_mp_ass_subscript,};    /* mp_ass_subscript */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char
 PY_pcons_doc[] = "";

PY_DEF_TYPE_R(pcons, "assoc");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
