/*
 * PGSMODULE.C
 *
 * include cpyright.h
 */

#define _pgs_MODULE
#include "pgsmodule.h"

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

/* PP_make_graph_1d - */

static char PP_make_graph_1d_doc[] = 
""
;

static PyObject *
PP_make_graph_1d(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.method.make_graph_1d) */
    int id;
    char *label;
    int cp;
    int n;
    double *x;
    double *y;
    char *xname;
    char *yname;
    char *kw_list[] = {"id", "label", "cp", "n", "x", "y", "xname", "yname", NULL};
    PG_graph *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "isiiO&O&ss:make_graph_1d", kw_list,
                                     &id, &label, &cp, &n,
                                     REAL_array_extractor, &x, REAL_array_extractor, &y,
                                     &xname, &yname))
        return NULL;
    result = PG_make_graph_1d(id, label, cp, n, x, y, xname, yname);
    SC_mark(result->info, 1);
    return PPgraph_from_ptr(result);
/* DO-NOT-DELETE splicer.end(pgs.method.make_graph_1d) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_make_graph_r2_r1 - */

static char PP_make_graph_r2_r1_doc[] = 
""
;

static PyObject *
PP_make_graph_r2_r1(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.method.make_graph_r2_r1) */
    int id;
    char *label;
    int cp;
    int imx;
    int jmx;
    int centering;
    double *x;
    double *y;
    double *r;
    char *dname;
    char *rname;
    char *kw_list[] = {"id", "label", "cp", "imx", "jmx", "centering", "x", "y", "r", "dname", "rname", NULL};
    PG_graph *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "isiiiiO&O&O&ss:make_graph_r2_r1", kw_list,
                                     &id, &label, &cp, &imx, &jmx, &centering,
                                     REAL_array_extractor, &x, REAL_array_extractor, &y,
                                     REAL_array_extractor, &r, &dname, &rname))
        return NULL;
    result = PG_make_graph_r2_r1(id, label, cp, imx, jmx, centering, x, y, r, dname, rname);
    SC_mark(result->info, 1);
    return PPgraph_from_ptr(result);
/* DO-NOT-DELETE splicer.end(pgs.method.make_graph_r2_r1) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_make_image - */

static char PP_make_image_doc[] = 
""
;

static PyObject *
PP_make_image(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.method.make_image) */
    int w, h, bpp;
    double dbx[PG_BOXSZ];
    double rbx[PG_BOXSZ];
    char *label;
    char *type;
    void *z;    /* XXX */
    PG_palette *palette;
    char *kw_list[] = {"label", "type", "z", "xmn", "xmx", "ymn", "ymx", "zmn", "zmx", "w", "h", "bpp", "palette", NULL};
    PG_image *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssO&ddddddiiiO&:make_image",
				     kw_list, &label, &type,
				     PP_buffer_extractor, &z,
				     &dbx[0], &dbx[1], &dbx[2], &dbx[3],
				     &rbx[0], &rbx[1],
				     &w, &h, &bpp,
				     palette_extractor, &palette))
        return NULL;

    result = PG_make_image_n(label, type, z, 2, WORLDC,
			     dbx, rbx, w, h, bpp, palette);

    return PPimage_from_ptr(result);
/* DO-NOT-DELETE splicer.end(pgs.method.make_image) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_get_processor_number - */

static char PP_get_processor_number_doc[] = 
""
;

static PyObject *
PP_get_processor_number(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.method.get_processor_number) UNMODIFIED */
    int result;

    result = PG_get_processor_number();
    return PY_INT_LONG(result);
/* DO-NOT-DELETE splicer.end(pgs.method.get_processor_number) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_get_number_processors - */

static char PP_get_number_processors_doc[] = 
""
;

static PyObject *
PP_get_number_processors(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.method.get_number_processors) UNMODIFIED */
    int result;

    result = PG_get_number_processors();
    return PY_INT_LONG(result);
/* DO-NOT-DELETE splicer.end(pgs.method.get_number_processors) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_iso_limit - */

static char PP_iso_limit_doc[] = 
""
;

static PyObject *
PP_iso_limit(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    double *a;
    int npts;
    double min;
    double max;
    char *kw_list[] = {"a", "npts", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&i:iso_limit", kw_list,
                                     REAL_array_extractor, &a, &npts))
        return NULL;
    PG_iso_limit(a, npts, &min, &max);
    return Py_BuildValue("ff", min, max);
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
{"make_graph_1d", (PyCFunction)PP_make_graph_1d, METH_KEYWORDS, PP_make_graph_1d_doc},
{"make_graph_r2_r1", (PyCFunction)PP_make_graph_r2_r1, METH_KEYWORDS, PP_make_graph_r2_r1_doc},
{"set_line_info", (PyCFunction)PY_set_line_info, METH_KEYWORDS, PY_set_line_info_doc},
{"set_tds_info", (PyCFunction)PY_set_tds_info, METH_KEYWORDS, PY_set_tds_info_doc},
{"set_tdv_info", (PyCFunction)PY_set_tdv_info, METH_KEYWORDS, PY_set_tdv_info_doc},
{"make_image", (PyCFunction)PP_make_image, METH_KEYWORDS, PP_make_image_doc},
{"get_processor_number", (PyCFunction)PP_get_processor_number, METH_NOARGS, PP_get_processor_number_doc},
{"get_number_processors", (PyCFunction)PP_get_number_processors, METH_NOARGS, PP_get_number_processors_doc},
{"iso_limit", (PyCFunction)PP_iso_limit, METH_KEYWORDS, PP_iso_limit_doc},
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_PGSMODULE_CONST - add BLANG generated constant bindings
 *                     - for PGS module
 */

static int _PY_pgsmodule_const(PyObject *m)
   {int ne;
    extern int PY_add_score_enum(PyObject *m);
    extern int PY_add_pml_enum(PyObject *m);
    extern int PY_add_pdb_enum(PyObject *m);
    extern int PY_add_pgs_enum(PyObject *m);

    ne = 0;

    ne += PY_add_score_enum(m);
    ne += PY_add_pml_enum(m);
    ne += PY_add_pdb_enum(m);
    ne += PY_add_pgs_enum(m);

    return(ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PGS - initialization function for the module
 *          - NOTE: MUST be called init_pgs
 */

static char pgs_module_documentation[] = 
""
;

PY_MOD_BEGIN(_pgs, pgs_module_documentation, PP_methods)
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

    ne = _PY_pgsmodule_const(m);
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

    ne += PY_init_pgs(m, d);
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

    PY_DEF_GETSET(PG_device, "dev");
    PY_DEF_GETSET(PG_palette, "pal");
    PY_DEF_GETSET(PG_image, "im");
    PY_DEF_GETSET(PG_graph, "data");

    PP_pdbdata_Type.tp_new = PyType_GenericNew;
    PP_pdbdata_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_pdbdata_Type) < 0)
       PY_MOD_RETURN_ERR;

/* add some symbolic constants to the module */
    if (PyDict_SetItemString(d, "pdbdata", (PyObject *) &PP_pdbdata_Type) < 0)
       PY_MOD_RETURN_ERR;

    PP_init_type_map();
    
#ifdef HAVE_PY_NUMPY
    _PP_init_numpy();
#endif

/* add Error Exceptions */
    PP_error_internal = PyErr_NewException("pdb.internal", NULL, NULL);
    PyDict_SetItemString(d, "internal", PP_error_internal);

    PP_error_user = PyErr_NewException("pdb.error", NULL, NULL);
    PyDict_SetItemString(d, "error", PP_error_user);

    PY_MOD_END(_pgs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
