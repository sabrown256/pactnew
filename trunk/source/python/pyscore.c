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

/* PY_set_count - */

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

/* PY_set_io_hooks - */

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

/* PP_setform - */

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

/* PP_print_controls - */

PyObject *PP_print_controls(PyObject *self, PyObject *args, PyObject *kwds)
   {int i, j;
    char *kw_list[] = {"i", "j", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ii:print_controls", kw_list,
                                     &i, &j))
        return NULL;

    if (i < 0 || i > 4) {
        PP_error_set_user(NULL, "i out of range (0-4)");
        return NULL;
    }
    
    PD_gs.print_ctrl[i] = j;
    
    Py_INCREF(Py_None);
    return Py_None;

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_zero_space - */

PyObject *PP_zero_space(PyObject *self, PyObject *args, PyObject *kwds)
{
    int flag;
    char *kw_list[] = {"flag", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:zero_space", kw_list,
                                     &flag))
        return NULL;

    result = SC_zero_space_n(flag, -2);

    return PY_INT_LONG(result);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_alloc - */

PyObject *PP_alloc(PyObject *self, PyObject *args, PyObject *kwds)
{

    long nitems;
    long bpi;
    char *name;
    char *kw_list[] = {"nitems", "bpi", "name", NULL};
    void *result;
    PyObject *rv;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "lls:alloc", kw_list,
                                     &nitems, &bpi, &name))
        return NULL;
    result = SC_alloc_n(nitems, bpi,
			SC_MEM_ATTR_FUNC,       name,
                        SC_MEM_ATTR_NO_ACCOUNT, FALSE,
                        SC_MEM_ATTR_ZERO_SPACE, -1,
                        0);
    if (result != NULL) {
        SC_mark(result, 1);
        rv = PY_COBJ_VOID_PTR(result, PP_free);
    } else {
        PyErr_NoMemory();
        rv = NULL;
    }
    return rv;

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_realloc - */

PyObject *PP_realloc(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    long nitems;
    long bpi;
    char *kw_list[] = {"p", "nitems", "bpi", NULL};
    void *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!ll:realloc", kw_list,
                                     &PY_COBJ_TYPE, &pobj, &nitems, &bpi))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    result = CREMAKE(p, char, nitems*bpi);
    return PY_COBJ_VOID_PTR((void *) result, NULL);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_cfree - */

PyObject *PP_cfree(PyObject *self, PyObject *args, PyObject *kwds)
{

    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:cfree", kw_list,
                                     &PY_COBJ_TYPE, &pobj))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    PP_free(p);   /*  SC_free_n(p); */
    Py_INCREF(Py_None);
    return Py_None;

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mem_print - */

PyObject *PP_mem_print(PyObject *self, PyObject *args, PyObject *kwds)
{

    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:mem_print", kw_list,
                                     &PY_COBJ_TYPE, &pobj))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    SC_mem_print(p);
    Py_INCREF(Py_None);
    return Py_None;

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mem_trace - */

PyObject *PP_mem_trace(PyObject *self, PyObject *args, PyObject *kwds)
{

    int result;

    result = SC_mem_trace();
    return PY_INT_LONG(result);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_reg_mem - */

PyObject *PP_reg_mem(PyObject *self, PyObject *args, PyObject *kwds)
{

    PyObject *pobj;
    void *p;
    long length;
    char *name;
    char *kw_list[] = {"p", "length", "name", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!ls:reg_mem", kw_list,
                                     &PY_COBJ_TYPE, &pobj, &length, &name))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    result = SC_reg_mem(p, length, name);
    return PY_INT_LONG(result);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_dereg_mem - */

PyObject *PP_dereg_mem(PyObject *self, PyObject *args, PyObject *kwds)
{

    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:dereg_mem", kw_list,
                                     &PY_COBJ_TYPE, &pobj))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    result = SC_dereg_mem(p);
    return PY_INT_LONG(result);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mem_lookup - */

PyObject *PP_mem_lookup(PyObject *self, PyObject *args, PyObject *kwds)
{

    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};
    char *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:mem_lookup", kw_list,
                                     &PY_COBJ_TYPE, &pobj))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    result = SC_mem_lookup(p);
    return Py_BuildValue("s", result);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mem_monitor - */

PyObject *PP_mem_monitor(PyObject *self, PyObject *args, PyObject *kwds)
{

    int old;
    int lev;
    char *id;
    char msg[80];
    char *kw_list[] = {"old", "lev", "id", NULL};
    long result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "iis:mem_monitor", kw_list,
                                     &old, &lev, &id))
        return NULL;
    *msg = '\0';
    result = SC_mem_monitor(old, lev, id, msg);
    if (*msg != '\0')
        return Py_BuildValue("(ls)", result, msg);
    else
        return Py_BuildValue("(lO)", result, Py_None);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mem_chk - */

PyObject *PP_mem_chk(PyObject *self, PyObject *args, PyObject *kwds)
{

    int type;
    char *kw_list[] = {"type", NULL};
    long result;
    PyObject *rv;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:mem_chk", kw_list,
                                     &type))
        return NULL;
    result = SC_mem_chk(type);
    if (result < 0) {
        PP_error_set(PP_error_internal, NULL,
                     "SC_mem_chk: %d", result);
        rv = NULL;
    } else {
        rv = PY_INT_LONG(result);
    }
    return rv;

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_is_score_ptr - */

PyObject *PP_is_score_ptr(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:is_score_ptr", kw_list,
                                     &PY_COBJ_TYPE, &pobj))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    result = SC_is_score_ptr(p);
    return PY_INT_LONG(result);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_arrlen - */

PyObject *PP_arrlen(PyObject *self, PyObject *args, PyObject *kwds)
{

    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};
    long result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:arrlen", kw_list,
                                     &PY_COBJ_TYPE, &pobj))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    result = SC_arrlen(p);
    return PY_INT_LONG(result);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mark - */

PyObject *PP_mark(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    int n;
    char *kw_list[] = {"p", "n", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!i:mark", kw_list,
                                     &PY_COBJ_TYPE, &pobj, &n))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    result = SC_mark(p, n);
    return PY_INT_LONG(result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_ref_count - */

PyObject *PP_ref_count(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:ref_count", kw_list,
                                     &PY_COBJ_TYPE, &pobj))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    result = SC_ref_count(p);
    return PY_INT_LONG(result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_permanent - */

PyObject *PP_permanent(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:permanent", kw_list,
                                     &PY_COBJ_TYPE, &pobj))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    SC_permanent(p);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_arrtype - */

PyObject *PP_arrtype(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    int type;
    char *kw_list[] = {"p", "type", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!i:arrtype", kw_list,
                                     &PY_COBJ_TYPE, &pobj, &type))
        return NULL;
    p = (void *) PY_GET_PTR(pobj);
    result = SC_arrtype(p, type);
    return PY_INT_LONG(result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_pause - */

PyObject *PP_pause(PyObject *self, PyObject *args, PyObject *kwds)
{
    SC_pause();
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
