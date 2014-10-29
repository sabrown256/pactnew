/*
 * PDBMODULE.C
 *
 * include cpyright.h
 */

#define _pdb_MODULE
#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_getdefstr - */

static char PP_getdefstr_doc[] = 
""
;

static PyObject *
PP_getdefstr(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PP_pdbdataObject *obj;
    PY_defstr *rv;
    char *kw_list[] = {"obj", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:getdefstr", kw_list,
                                     &PP_pdbdata_Type, &obj))
        return NULL;

    rv = obj->dpobj;
    
    Py_INCREF(rv);
    return (PyObject *) rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_gettype - */

static char PP_gettype_doc[] = 
""
;

static PyObject *
PP_gettype(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PP_pdbdataObject *obj;
    char *kw_list[] = {"obj", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:gettype", kw_list,
                                     &PP_pdbdata_Type, &obj))
        return NULL;
    return PY_STRING_STRING(obj->type);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_getfile - */

static char PP_getfile_doc[] = 
""
;

static PyObject *
PP_getfile(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PP_pdbdataObject *obj;
    PY_PDBfile *file;
    char *kw_list[] = {"obj", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:getfile", kw_list,
                                     &PP_pdbdata_Type, &obj))
        return NULL;

    file = (PY_PDBfile *) _PP_find_file_obj(obj->fileinfo->file);
    if (file == NULL)
        return NULL;

    Py_INCREF(file);
    return (PyObject *) file;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_getdata - */

static char PP_getdata_doc[] = 
""
;

static PyObject *
PP_getdata(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PP_pdbdataObject *obj;
    char *kw_list[] = {"obj", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:getdata", kw_list,
                                     &PP_pdbdata_Type, &obj))
        return NULL;
    /* XXX add deallocator */
    return PY_COBJ_VOID_PTR(obj->data, NULL);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_getmember - */

static char PP_getmember_doc[] = 
""
;

static PyObject *
PP_getmember(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PP_pdbdataObject *obj;
    char *name;
    char *kw_list[] = {"obj", "name", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!s:getmember", kw_list,
                                     &PP_pdbdata_Type, &obj, &name))
        return NULL;

    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_unpack - */

static char PP_unpack_doc[] = 
""
;

static PyObject *
PP_unpack(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    char *kw_list[] = {"data", "array", "struct", "scalar", NULL};
    int ok;
    int array, structure, scalar;
    PyObject *rv;
    PP_pdbdataObject *data;
    PP_form form;

    array = AS_NONE;
    structure = AS_NONE;
    scalar = AS_NONE;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!|iii:unpack", kw_list,
                                     &PP_pdbdata_Type, &data,
                                     &array, &structure, &scalar))
        return NULL;

    /* default to global values */
    form = PP_global_form;
    ok = _PP_assign_form(&form, array, structure, scalar);
    if (ok < 0)
        return NULL;
    /* replace AS_PDBDATA with unpack defaults */
    if (form.array_kind == AS_PDBDATA)
        form.array_kind = AS_LIST;
    if (form.struct_kind == AS_PDBDATA)
        form.struct_kind = AS_TUPLE;
    if (form.scalar_kind == AS_PDBDATA)
        form.scalar_kind = AS_OBJECT;

    rv = _PP_wr_syment(data->fileinfo, data->type, data->dims,
                       data->nitems, data->data, &form);
    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMethodDef PP_methods[] = {
{"print_controls", (PyCFunction)PP_print_controls, METH_KEYWORDS, PP_print_controls_doc},
{"zero_space", (PyCFunction)PP_zero_space, METH_KEYWORDS, PP_zero_space_doc},
{"alloc", (PyCFunction)PP_alloc, METH_KEYWORDS, PP_alloc_doc},
{"realloc", (PyCFunction)PP_realloc, METH_KEYWORDS, PP_realloc_doc},
{"cfree", (PyCFunction)PP_cfree, METH_KEYWORDS, PP_cfree_doc},
{"sfree", (PyCFunction)PP_cfree, METH_KEYWORDS, PP_cfree_doc},
{"mem_print", (PyCFunction)PP_mem_print, METH_KEYWORDS, PP_mem_print_doc},
{"mem_trace", (PyCFunction)PP_mem_trace, METH_NOARGS, PP_mem_trace_doc},
{"reg_mem", (PyCFunction)PP_reg_mem, METH_KEYWORDS, PP_reg_mem_doc},
{"dereg_mem", (PyCFunction)PP_dereg_mem, METH_KEYWORDS, PP_dereg_mem_doc},
{"mem_lookup", (PyCFunction)PP_mem_lookup, METH_KEYWORDS, PP_mem_lookup_doc},
{"mem_monitor", (PyCFunction)PP_mem_monitor, METH_KEYWORDS, PP_mem_monitor_doc},
{"mem_chk", (PyCFunction)PP_mem_chk, METH_KEYWORDS, PP_mem_chk_doc},
{"is_score_ptr", (PyCFunction)PP_is_score_ptr, METH_KEYWORDS, PP_is_score_ptr_doc},
{"arrlen", (PyCFunction)PP_arrlen, METH_KEYWORDS, PP_arrlen_doc},
{"mark", (PyCFunction)PP_mark, METH_KEYWORDS, PP_mark_doc},
{"ref_count", (PyCFunction)PP_ref_count, METH_KEYWORDS, PP_ref_count_doc},
{"set_count", (PyCFunction)PY_set_count, METH_KEYWORDS, PY_set_count_doc},
{"permanent", (PyCFunction)PP_permanent, METH_KEYWORDS, PP_permanent_doc},
{"arrtype", (PyCFunction)PP_arrtype, METH_KEYWORDS, PP_arrtype_doc},
{"pause", (PyCFunction)PP_pause, METH_NOARGS, PP_pause_doc},
{"set_io_hooks", (PyCFunction)PY_set_io_hooks, METH_KEYWORDS, PY_set_io_hooks_doc},
{"setform", (PyCFunction)PP_setform, METH_KEYWORDS, PP_setform_doc},
{"make_set_1d", (PyCFunction)PP_make_set_1d, METH_KEYWORDS, PP_make_set_1d_doc},
{"make_ac_set", (PyCFunction)PP_make_ac_set, METH_KEYWORDS, PP_make_ac_set_doc},
{"open_vif", (PyCFunction)PY_open_vif, METH_KEYWORDS, PY_open_vif_doc},
{"getdefstr", (PyCFunction)PP_getdefstr, METH_KEYWORDS, PP_getdefstr_doc},
{"gettype", (PyCFunction)PP_gettype, METH_KEYWORDS, PP_gettype_doc},
{"getfile", (PyCFunction)PP_getfile, METH_KEYWORDS, PP_getfile_doc},
{"getdata", (PyCFunction)PP_getdata, METH_KEYWORDS, PP_getdata_doc},
{"getmember", (PyCFunction)PP_getmember, METH_KEYWORDS, PP_getmember_doc},
{"unpack", (PyCFunction)PP_unpack, METH_KEYWORDS, PP_unpack_doc},
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_EXTRA_DEFSTR_MARK - add an extra mark into the virtual
 *                       - internal file's predefined defstrs
 */

static int _PP_extra_defstr_mark(haelem *hp, void *arg)
   {int ok;
    defstr *dp;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &dp, FALSE);
    SC_ASSERT(ok == TRUE);

    SC_mark(dp, 1);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_PDBMODULE_CONST - add BLANG generated constant bindings
 *                     - for PDB module
 */

static int _PY_pdbmodule_const(PyObject *m)
   {int ne;
    extern int PY_add_score_enum(PyObject *m);
    extern int PY_add_pml_enum(PyObject *m);
    extern int PY_add_pdb_enum(PyObject *m);

    ne = 0;

    ne += PY_add_score_enum(m);
    ne += PY_add_pml_enum(m);
    ne += PY_add_pdb_enum(m);

    return(ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PDB - initialization function for the module
 *          - NOTE: MUST be called init_pdb
 */

static char pdb_module_documentation[] = "";

PY_MOD_BEGIN(_pdb, pdb_module_documentation, PP_methods)
   {int ne;
    PyObject *d;

    d = PyModule_GetDict(m);
    if (d == NULL)
       PY_MOD_RETURN_ERR;

    ne = _PY_pact_constants(m);
    if (ne > 0)
       PY_MOD_RETURN_ERR;

    ne = _PY_pact_type_system(m);
    if (ne > 0)
       PY_MOD_RETURN_ERR;

    ne = _PY_pdbmodule_const(m);
    if (ne > 0)
       PY_MOD_RETURN_ERR;

    ne += PY_init_score(m, d);
    if (ne > 0)
       PY_MOD_RETURN_ERR;

    ne += PY_init_pml(m, d);
    if (ne > 0)
       PY_MOD_RETURN_ERR;

    ne += PY_init_pdb(m, d);
    if (ne > 0)
       PY_MOD_RETURN_ERR;

    PY_DEF_GETSET(pcons, "alist");
    PY_DEF_GETSET(hasharr, "hasharr");

    PY_DEF_GETSET(PM_field, "field");
    PY_DEF_GETSET(PM_mesh_topology, "mt");
    PY_DEF_GETSET(PM_set, "set");
    PY_DEF_GETSET(PM_mapping, "map");

    PY_DEF_GETSET(defstr, "dp");
    PY_DEF_GETSET(memdes, "desc");
    PY_DEF_GETSET(PDBfile, "object");

    PP_pdbdata_Type.tp_new = PyType_GenericNew;
    PP_pdbdata_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_pdbdata_Type) < 0)
       PY_MOD_RETURN_ERR;

/* add some symbolic constants to the module */
    if (PyDict_SetItemString(d, "pdbdata", (PyObject *) &PP_pdbdata_Type) < 0)
       PY_MOD_RETURN_ERR;

/* make 'open' a synonym for 'PDBfile' */
    if (PyDict_SetItemString(d, "open", (PyObject *) &PY_PDBfile_type) < 0)
       PY_MOD_RETURN_ERR;

    PP_init_type_map();

#ifdef HAVE_PY_NUMPY
    _PP_init_numpy();
#endif
  
    if (PyModule_AddIntConstant(m, "AS_NONE", AS_NONE) < 0)
       PY_MOD_RETURN_ERR;
    if (PyModule_AddIntConstant(m, "AS_PDBDATA", AS_PDBDATA) < 0)
       PY_MOD_RETURN_ERR;
    if (PyModule_AddIntConstant(m, "AS_OBJECT", AS_OBJECT) < 0)
       PY_MOD_RETURN_ERR;
    if (PyModule_AddIntConstant(m, "AS_TUPLE", AS_TUPLE) < 0)
       PY_MOD_RETURN_ERR;
    if (PyModule_AddIntConstant(m, "AS_LIST", AS_LIST) < 0)
       PY_MOD_RETURN_ERR;
    if (PyModule_AddIntConstant(m, "AS_DICT", AS_DICT) < 0)
       PY_MOD_RETURN_ERR;
    if (PyModule_AddIntConstant(m, "AS_ARRAY", AS_ARRAY) < 0)
       PY_MOD_RETURN_ERR;
    
    PP_open_file_dict = PyDict_New();
    if (PyModule_AddObject(m, "files", PP_open_file_dict) < 0)
       PY_MOD_RETURN_ERR;

    PP_vif_info = _PP_open_vif("PP_vif");
    if (PP_vif_info != NULL)
       {int err;

        PP_vif_obj = PY_PDBfile_newobj(NULL, PP_vif_info);
        PP_vif     = PP_vif_info->file;

/* mark every currently existing defstr in the host_chart */
	err = SC_hasharr_foreach(PP_vif->host_chart, _PP_extra_defstr_mark, NULL);
	SC_ASSERT(err == TRUE);

/* XXX - test err */
        if (PyModule_AddObject(m, "vif", (PyObject *) PP_vif_obj) < 0)
           PY_MOD_RETURN_ERR;};

/* add Error Exceptions */
    PP_error_internal = PyErr_NewException("pdb.internal", NULL, NULL);
    PyDict_SetItemString(d, "internal", PP_error_internal);
    
    PP_error_user = PyErr_NewException("pdb.error", NULL, NULL);
    PyDict_SetItemString(d, "error", PP_error_user);
    
    PY_MOD_END(_pdb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
