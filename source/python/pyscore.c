/*
 * PYSCORE.C - support for SCORE Python bindings
 *
 */

#include "cpyright.h"
#include "py_int.h"

char
 PY_set_count_doc[] = "",
 PY_set_io_hooks_doc[] = "",
 PP_setform_doc[] = "",
 PP_print_controls_doc[] = "",
 PP_zero_space_doc[] = "",
 PP_alloc_doc[] = "",
 PP_realloc_doc[] = "",
 PP_cfree_doc[] = "",
 PP_mem_print_doc[] = "",
 PP_mem_trace_doc[] = "",
 PP_reg_mem_doc[] = "",
 PP_dereg_mem_doc[] = "",
 PP_mem_lookup_doc[] = "",
 PP_mem_monitor_doc[] = "",
 PP_mem_chk_doc[] = "",
 PP_is_score_ptr_doc[] = "",
 PP_arrlen_doc[] = "",
 PP_mark_doc[] = "",
 PP_ref_count_doc[] = "",
 PP_permanent_doc[] = "",
 PP_arrtype_doc[] = "",
 PP_pause_doc[] = "";


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_SET_COUNT - */

PyObject *PY_set_count(PyObject *self, PyObject *args, PyObject *kwds)
   {int n, result;
    void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", "n", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds, "O!i:set_count", kw_list,
				    &PY_COBJ_TYPE, &pobj, &n))
       {p      = (void *) PY_GET_PTR(pobj);
	result = SC_set_count(p, n);
	rv     = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_SET_IO_HOOKS - */

PyObject *PY_set_io_hooks(PyObject *self, PyObject *args, PyObject *kwds)
   {int whch;
    PyObject *rv;
    char *kw_list[] = {"whch", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds, "i:set_io_hooks", kw_list,
                                     &whch))
       {SC_set_io_hooks(whch);
	Py_INCREF(Py_None);
	rv = Py_None;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_SETFORM - */

PyObject *PP_setform(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok, array, structure, scalar;
    PyObject *rv;
    char *kw_list[] = {"array", "struct", "scalar", NULL};

    rv = NULL;

    array     = AS_NONE;
    structure = AS_NONE;
    scalar    = AS_NONE;
    if (PyArg_ParseTupleAndKeywords(args, kwds, "|iii:setform", kw_list,
				    &array, &structure, &scalar))
       {ok = _PP_assign_form(&PP_global_form, array, structure, scalar);
	if (ok >= 0)
	   rv = Py_BuildValue("(iii)",
			      PP_global_form.array_kind,
			      PP_global_form.struct_kind,
			      PP_global_form.scalar_kind);};
   return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_PRINT_CONTROLS - */

PyObject *PP_print_controls(PyObject *self, PyObject *args, PyObject *kwds)
   {int i, j;
    PyObject *rv;
    char *kw_list[] = {"i", "j", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "ii:print_controls", kw_list,
				    &i, &j))
       {if ((i < 0) || (i > 4))
	   PP_error_set_user(NULL, "i out of range (0-4)");
	else
	   {PD_gs.print_ctrl[i] = j;
    	    rv = Py_None;

	    Py_INCREF(rv);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_ZERO_SPACE - */

PyObject *PP_zero_space(PyObject *self, PyObject *args, PyObject *kwds)
   {int flag, result;
    PyObject *rv;
    char *kw_list[] = {"flag", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "i:zero_space", kw_list,
				    &flag))
       {result = SC_zero_space_n(flag, -2);
	rv = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_ALLOC - */

PyObject *PP_alloc(PyObject *self, PyObject *args, PyObject *kwds)
   {long nitems, bpi;
    char *name;
    void *result;
    PyObject *rv;
    char *kw_list[] = {"nitems", "bpi", "name", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "lls:alloc", kw_list,
				    &nitems, &bpi, &name))
       {result = SC_alloc_n(nitems, bpi,
			    SC_MEM_ATTR_FUNC,       name,
			    SC_MEM_ATTR_NO_ACCOUNT, FALSE,
			    SC_MEM_ATTR_ZERO_SPACE, -1,
			    0);
	if (result != NULL)
	   {SC_mark(result, 1);
	    rv = PY_COBJ_VOID_PTR(result, PP_free);}
	else
	   {PyErr_NoMemory();
	    rv = NULL;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_REALLOC - */

PyObject *PP_realloc(PyObject *self, PyObject *args, PyObject *kwds)
   {long nitems, bpi;
    void *p, *result;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", "nitems", "bpi", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!ll:realloc", kw_list,
				    &PY_COBJ_TYPE, &pobj, &nitems, &bpi))
       {p      = (void *) PY_GET_PTR(pobj);
	result = CREMAKE(p, char, nitems*bpi);
	rv     = PY_COBJ_VOID_PTR((void *) result, NULL);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_CFREE - */

PyObject *PP_cfree(PyObject *self, PyObject *args, PyObject *kwds)
   {void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:cfree", kw_list,
				    &PY_COBJ_TYPE, &pobj))
       {p = (void *) PY_GET_PTR(pobj);
	PP_free(p);
/*  SC_free_n(p); */
	rv = Py_None;

	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_MEM_PRINT - */

PyObject *PP_mem_print(PyObject *self, PyObject *args, PyObject *kwds)
   {void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:mem_print", kw_list,
				    &PY_COBJ_TYPE, &pobj))
       {p = (void *) PY_GET_PTR(pobj);
	SC_mem_print(p);
	rv = Py_None;
	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_MEM_TRACE - */

PyObject *PP_mem_trace(PyObject *self, PyObject *args, PyObject *kwds)
   {int result;
    PyObject *rv;

    result = SC_mem_trace();
    rv     = PY_INT_LONG(result);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_REG_MEM - */

PyObject *PP_reg_mem(PyObject *self, PyObject *args, PyObject *kwds)
   {int result;
    long length;
    char *name;
    void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", "length", "name", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!ls:reg_mem", kw_list,
				    &PY_COBJ_TYPE, &pobj, &length, &name))
       {p      = (void *) PY_GET_PTR(pobj);
	result = SC_reg_mem(p, length, name);
	rv     = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_DEREG_MEM - */

PyObject *PP_dereg_mem(PyObject *self, PyObject *args, PyObject *kwds)
   {int result;
    void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:dereg_mem", kw_list,
				    &PY_COBJ_TYPE, &pobj))
       {p      = (void *) PY_GET_PTR(pobj);
	result = SC_dereg_mem(p);
	rv     = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_MEM_LOOKUP - */

PyObject *PP_mem_lookup(PyObject *self, PyObject *args, PyObject *kwds)
   {char *result;
    void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:mem_lookup", kw_list,
				    &PY_COBJ_TYPE, &pobj))
       {p      = (void *) PY_GET_PTR(pobj);
	result = SC_mem_lookup(p);
	rv     = Py_BuildValue("s", result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_MEM_MONITOR - */

PyObject *PP_mem_monitor(PyObject *self, PyObject *args, PyObject *kwds)
   {int old, lev;
    long result;
    char msg[80];
    char *id;
    PyObject *rv;
    char *kw_list[] = {"old", "lev", "id", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "iis:mem_monitor", kw_list,
				    &old, &lev, &id))
       {*msg = '\0';
	result = SC_mem_monitor(old, lev, id, msg);
	if (*msg != '\0')
	   rv = Py_BuildValue("(ls)", result, msg);
	else
	   rv = Py_BuildValue("(lO)", result, Py_None);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_MEM_CHK - */

PyObject *PP_mem_chk(PyObject *self, PyObject *args, PyObject *kwds)
   {int type;
    long result;
    PyObject *rv;
    char *kw_list[] = {"type", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "i:mem_chk", kw_list,
				    &type))
       {result = SC_mem_chk(type);
	if (result < 0)
	   {PP_error_set(PP_error_internal, NULL,
			 "SC_mem_chk: %d", result);
	    rv = NULL;}
	else
	   rv = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_IS_SCORE_PTR - */

PyObject *PP_is_score_ptr(PyObject *self, PyObject *args, PyObject *kwds)
   {int result;
    void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:is_score_ptr", kw_list,
				    &PY_COBJ_TYPE, &pobj))
       {p      = (void *) PY_GET_PTR(pobj);
	result = SC_is_score_ptr(p);
	rv     = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_ARRLEN - */

PyObject *PP_arrlen(PyObject *self, PyObject *args, PyObject *kwds)
   {long result;
    void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:arrlen", kw_list,
				    &PY_COBJ_TYPE, &pobj))
       {p      = (void *) PY_GET_PTR(pobj);
	result = SC_arrlen(p);
	rv     = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_MARK - */

PyObject *PP_mark(PyObject *self, PyObject *args, PyObject *kwds)
   {int result, n;
    void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", "n", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!i:mark", kw_list,
				    &PY_COBJ_TYPE, &pobj, &n))
       {p      = (void *) PY_GET_PTR(pobj);
	result = SC_mark(p, n);
	rv     = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_REF_COUNT - */

PyObject *PP_ref_count(PyObject *self, PyObject *args, PyObject *kwds)
   {int result;
    void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:ref_count", kw_list,
				    &PY_COBJ_TYPE, &pobj))
       {p      = (void *) PY_GET_PTR(pobj);
	result = SC_ref_count(p);
	rv     = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_PERMANENT - */

PyObject *PP_permanent(PyObject *self, PyObject *args, PyObject *kwds)
   {void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:permanent", kw_list,
				    &PY_COBJ_TYPE, &pobj))
       {p = (void *) PY_GET_PTR(pobj);
	SC_permanent(p);
	rv = Py_None;
	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_ARRTYPE - */

PyObject *PP_arrtype(PyObject *self, PyObject *args, PyObject *kwds)
   {int result, type;
    void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", "type", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!i:arrtype", kw_list,
				    &PY_COBJ_TYPE, &pobj, &type))
       {p      = (void *) PY_GET_PTR(pobj);
	result = SC_arrtype(p, type);
	rv     = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_PAUSE - */

PyObject *PP_pause(PyObject *self, PyObject *args, PyObject *kwds)
   {PyObject *rv;

    SC_pause();

    rv = Py_None;

    Py_INCREF(rv);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             HASHARRAY ROUTINES                           */

/*--------------------------------------------------------------------------*/

#undef PY_DEF_DESTRUCTOR
#undef PY_DEF_REPR
#undef PY_DEF_TP_METH
#undef PY_DEF_AS_MAP

#define PY_DEF_DESTRUCTOR	    PY_hasharr_tp_dealloc
#define PY_DEF_REPR                 PY_hasharr_tp_repr
#define PY_DEF_TP_METH              PY_hasharr_methods
#define PY_DEF_AS_MAP	            &PY_hasharr_as_mapping

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_install[] = "";

static PyObject *PY_hasharr_install(PY_hasharr *self,
				    PyObject *args,
				    PyObject *kwds)
   {char *key, *obj, *type;
    PyObject *rv;
    char *kw_list[] = {"key", "obj", "type", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "sss:install", kw_list,
				    &key, &obj, &type))
       {SC_hasharr_install(self->pyo, key, obj, type, 3, -1);
	rv = Py_None;
	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_def_lookup[] = "";

static PyObject *PY_hasharr_def_lookup(PY_hasharr *self,
				       PyObject *args,
				       PyObject *kwds)
   {char *key;
    hasharr *tab;
    PyObject *rv;
    char *kw_list[] = {"key", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "s:def_lookup", kw_list,
				    &key))
       {tab = (hasharr *) self->pyo;
	SC_hasharr_def_lookup(tab, key);
	rv = Py_None;
	Py_INCREF(Py_None);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int _PP_rl_haelem(haelem *hp, void *a)
   {

    _PP_rel_syment(PP_vif->host_chart, (char *) &hp->def, 1L, hp->type);

    CFREE(hp->type);
    CFREE(hp->def);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_clear[] = "";

static PyObject *PY_hasharr_clear(PY_hasharr *self,
				  PyObject *args,
				  PyObject *kwds)
   {PyObject *rv;

    SC_hasharr_foreach(self->pyo, _PP_rl_haelem, NULL);
    rv = Py_None;
    Py_INCREF(rv);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_has_key[] = "";

static PyObject *PY_hasharr_has_key(PY_hasharr *self,
				    PyObject *args,
				    PyObject *kwds)
   {long ok;
    char *key;
    haelem *np;
    PyObject *rv;
    char *kw_list[] = {"key", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "s:has_key", kw_list,
				    &key))
       {np = SC_hasharr_lookup(self->pyo, key);
	ok = np != NULL;

#if PYTHON_API_VERSION < 1012
	rv = PY_INT_LONG(ok);
#else
	rv = PyBool_FromLong(ok);
#endif
       };

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_items[] = "";

static PyObject *PY_hasharr_items(PY_hasharr *self,
				  PyObject *args,
				  PyObject *kwds)
   {PyObject *rv;

    rv = NULL;

    PyErr_SetString(PyExc_NotImplementedError, "items");

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_keys[] = "";

static PyObject *PY_hasharr_keys(PY_hasharr *self,
				 PyObject *args,
				 PyObject *kwds)
   {int ne, err;
    Py_ssize_t i;
    char **names;
    hasharr *ha;
    PyObject *rv;
 
    rv = NULL;

    ha = self->pyo;
    if (ha == NULL)
       PP_error_set(PP_error_internal, NULL, "Hasharr is NULL");

    else
       {ne = SC_hasharr_get_n(ha);
	if (ne == 0)
	   {rv = Py_None;
	    Py_INCREF(rv);}

	else
	   {rv = PyTuple_New(ne);
	    if (rv != NULL)
	       {names = SC_hasharr_dump(ha, NULL, NULL, FALSE);
		for (i = 0; i < ne; i++)
		    {err = PyTuple_SetItem(rv, i, PY_STRING_STRING(names[i]));
		     if (err < 0)
		        {Py_DECREF(rv);
			 rv = NULL;
			 break;};};

		SC_free_strings(names);};};};
  
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_update[] = "";

static PyObject *PY_hasharr_update(PY_hasharr *self,
				   PyObject *args,
				   PyObject *kwds)
   {int err;
    hasharr *tab;
    PyObject *dict, *rv;
    char *kw_list[] = {"dict", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O", kw_list,
                                    &dict))
       {tab = (hasharr *) self->pyo;
	if (tab == NULL)
	   PP_error_set(PP_error_internal,
			(PyObject *) self, "Hasharr is NULL");
	else
	   {err = PP_update_hasharr(tab, dict);
	    if (err >= 0)
	       {rv = Py_None;
		Py_INCREF(rv);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_values[] = "";

static PyObject *PY_hasharr_values(PY_hasharr *self,
				   PyObject *args,
				   PyObject *kwds)
   {

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc_get[] = "";

static PyObject *PY_hasharr_get(PY_hasharr *self,
				PyObject *args,
				PyObject *kwds)
   {

    PyErr_SetString(PyExc_NotImplementedError, "get");

    return(NULL);}

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

static void PY_hasharr_tp_dealloc(PY_hasharr *self)
   {int n;

    n = SC_mark(self->pyo, 0);
    if (n < 2)
       {SC_free_hasharr(self->pyo, _PP_rl_haelem, NULL);
        self->pyo = NULL;}
    else
       SC_mark(self->pyo, -1);

    PY_TYPE(self)->tp_free((PyObject*)self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_hasharr_tp_repr(PY_hasharr *self)
   {PyObject *rv, *h;

    h = _PP_unpack_hasharr(self->pyo, 1L);
    if (h == NULL)
       rv = NULL;
    else
       {rv = PyObject_Repr(h);
	Py_DECREF(h);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_hasharr_tp_init(PY_hasharr *self,
			      PyObject *args, PyObject *kwds)
   {int size, docflag, rv;
    char *kw_list[] = {"size", "docflag", NULL};
    hasharr *tab;

     rv = -1;

    size = HSZSMALL;
    docflag = NODOC;
    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "|ii:init", kw_list,
				    &size, &docflag))
       {tab = SC_make_hasharr(size, docflag, SC_HA_NAME_KEY, 0);
	if (tab != NULL)
	   {self = PY_hasharr_newobj(self, tab);
	    rv   = (self == NULL) ? -1 : 0;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_UNPACK_HASHARR - */

PyObject *_PP_unpack_hasharr(void *p, long nitems)
   {int ierr;
    long i;
    char *nm;
    hasharr *tab;
    PyObject *dict, *item;

    ierr = 0;
    tab = (hasharr *) p;
    if (tab == NULL)
       {dict = Py_None;
        Py_INCREF(dict);}

    else
       {dict = PyDict_New();
	if (dict != NULL)
	   {for (i = 0; SC_hasharr_next(tab, &i, &nm, NULL, (void **) &item); i++)
	        {if (item == NULL)
		    {ierr = -1;
		     break;};

		 ierr = PyDict_SetItemString(dict, nm, item);
		 if (ierr < 0)
		    break;};

	    if (ierr < 0)
	       {Py_DECREF(dict);
		dict = NULL;};};};
    
    return(dict);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_UNPACK_HASHARR_HAELEM - */

PyObject *PP_unpack_hasharr_haelem(char *type, void *vr)
   {intb bpi;
    inti nitems;
    dimdes *dims;
    PP_form form;
    PyObject *rv;

     rv = NULL;

    form = PP_global_form;

    type = SC_dereference(CSTRSAVE(type));
    bpi = _PD_lookup_size(type, PP_vif->host_chart);
    if (bpi >= 0)
       {nitems = SC_arrlen(vr) / bpi;
	if (nitems > 1)
	   dims = _PD_mk_dimensions(0, nitems);
	else
	   dims = NULL;

	rv = _PP_wr_syment(PP_vif_info, type, dims, nitems, vr, &form);
	CFREE(type);
	_PD_rl_dimensions(dims);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_UPDATE_HASHARR - update a hasharr from a dictionary.
 *   Note: hasharrs also support the dictionary protocol.
 */

int PP_update_hasharr(hasharr *tab, PyObject *dict)
   {int i, nkeys, err;
    char *keyname, *ptype;
    void *vr;
    PP_descr *descr;
    haelem *entry;
    PyObject *keys, *keyitem, *value;

    err   = 0;
    descr = NULL;
    value = NULL;
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
	    {err = PP_make_data(value, PP_vif_info,
				descr->type, descr->dims, &vr);
	     if (err == -1)
                break;}
	 else
            vr = descr->data;

	 ptype = PP_add_indirection(descr->type, 1);
	 if (ptype == NULL)
	    {err = -1;
	     break;};

/* SC_hasharr_install will SC_mark vr */
	 entry = SC_hasharr_install(tab, keyname, vr, ptype, 3, -1);
	 if (entry == NULL)
	    {err = -1;
	     break;};

	 _PP_rl_descr(descr);
	 descr = NULL;
	 Py_XDECREF(value);
	 value = NULL;};

    if (descr != NULL)
       _PP_rl_descr(descr);

    Py_XDECREF(value);
    Py_DECREF(keys);

    return(err);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PY_hasharr *PY_hasharr_newobj(PY_hasharr *obj, hasharr *tab)
   {PP_defstrObject *dpobj;
    
    if (obj == NULL)
       {obj = (PY_hasharr *) PyType_GenericAlloc(&PY_hasharr_type, 0);
        if (obj == NULL)
	   return(NULL);};

    dpobj = _PP_defstr_find_singleton("HASHARR", NULL, PP_vif_info);
    if (dpobj == NULL)
       return(NULL);

    obj = (PY_hasharr *) PP_pdbdata_newobj((PP_pdbdataObject *) obj,
					   tab, dpobj->dp->type, 1L, NULL,
					   dpobj->dp, dpobj->fileinfo,
					   dpobj, NULL);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Code to access hasharr objects as mappings */

static Py_ssize_t PY_hasharr_mp_length(PyObject *_self)
   {Py_ssize_t ne;
    PY_hasharr *self = (PY_hasharr *) _self;

    ne = SC_hasharr_get_n(self->pyo);

    return(ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_hasharr_mp_subscript(PyObject *_self, PyObject *key)
   {char *name;
    haelem *hp;
    hasharr *harr;
    PyObject *rv;
    PY_hasharr *self = (PY_hasharr *) _self;

    if (!PY_STRING_CHECK(key))
       {PP_error_set_user(key, "key must be string");
        return(NULL);};

    name = PY_STRING_AS_STRING(key);

    harr = self->pyo;
    if (harr == NULL)
       {PP_error_set(PP_error_internal,
                     (PyObject *) self, "hasharr is NULL");
        return(NULL);};

    hp = SC_hasharr_lookup(harr, name);
    if (hp == NULL)
       {PyErr_SetObject(PyExc_KeyError, key);
        return(NULL);};

    rv = PP_unpack_hasharr_haelem(hp->type, hp->def);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_hasharr_mp_ass_subscript(PyObject *_self,
				       PyObject *key, PyObject *v)
   {int ok;
    char *name, *ptype;
    hasharr *harr;
    PY_hasharr *self = (PY_hasharr *) _self;
    
    if (!PY_STRING_CHECK(key))
       {PP_error_set_user(key, "key must be string");
        return(-1);};

    name = PY_STRING_AS_STRING(key);

    harr = self->pyo;
    if (harr == NULL)
       {PP_error_set(PP_error_internal,
                     (PyObject *) self, "hasharr is NULL");
        return(-1);};

    if (v == NULL)
       {ok = SC_hasharr_remove(harr, name);
/* XXX - remove data too */
        if (ok != TRUE)
	   {PP_error_set(PP_error_internal, NULL, "error removing object");
            return(-1);};}

    else
       {PP_descr *descr;
        void *vr;
        haelem *hp;

        descr = PP_get_object_descr(PP_vif_info, v);
        if (descr == NULL)
            return(-1);

        if (descr->data == NULL)
	   {ok = PP_make_data(v, PP_vif_info, descr->type, descr->dims, &vr);
            if (ok == -1)
               return(-1);}
	else
           vr = descr->data;

        SC_mark(vr, 1);

        ptype = PP_add_indirection(descr->type, 1);
        if (ptype == NULL)
            return(-1);

        hp = SC_hasharr_install(harr, name, vr, ptype, 3, -1);

/* XXX release vr as well */
        _PP_rl_descr(descr);

        if (hp == NULL)
	   {PP_error_set(PP_error_internal, NULL, "Error from SC_hasharr_install");
            return(-1);};};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMappingMethods
 PY_hasharr_as_mapping = {PY_hasharr_mp_length,          /* mp_length */
			  PY_hasharr_mp_subscript,       /* mp_subscript */
			  PY_hasharr_mp_ass_subscript,}; /* mp_as_subscript */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_hasharr_doc[] = "";

PY_DEF_TYPE(hasharr);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
