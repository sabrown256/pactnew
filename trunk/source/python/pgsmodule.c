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

#define _pgs_MODULE
#include "pgsmodule.h"

/* DO-NOT-DELETE splicer.begin(pgs.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.C_definition) */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_print_controls - */

static char PP_print_controls__doc__[] = 
""
;

static PyObject *
PP_print_controls(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    int i;
    int j;
    char *kw_list[] = {"i", "j", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ii:print_controls", kw_list,
                                     &i, &j))
        return NULL;

    if (i < 0 || i > 4) {
        PP_error_set_user(NULL, "i out of range (0-4)");
        return NULL;
    }
    
    PD_print_controls[i] = j;
    
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_zero_space - */

static char PP_zero_space__doc__[] = 
""
;

static PyObject *
PP_zero_space(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    int flag;
    char *kw_list[] = {"flag", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:zero_space", kw_list,
                                     &flag))
        return NULL;

    result = SC_zero_space_n(flag, -2);

    return PyInt_FromLong((long) result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_alloc - */

static char PP_alloc__doc__[] = 
""
;

static PyObject *
PP_alloc(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
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
    result = SC_alloc_nz(nitems, bpi, name, FALSE, -1);
    if (result != NULL) {
        SC_mark(result, 1);
        rv = PyCObject_FromVoidPtr(result, PP_free);
    } else {
        PyErr_NoMemory();
        rv = NULL;
    }
    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_realloc - */

static char PP_realloc__doc__[] = 
""
;

static PyObject *
PP_realloc(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    long nitems;
    long bpi;
    char *kw_list[] = {"p", "nitems", "bpi", NULL};
    void *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!ll:realloc", kw_list,
                                     &PyCObject_Type, &pobj, &nitems, &bpi))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    result = CREMAKE(p, char, nitems*bpi);
    return PyCObject_FromVoidPtr((void *) result, NULL);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_sfree - */

static char PP_sfree__doc__[] = 
""
;

static PyObject *
PP_sfree(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:sfree", kw_list,
                                     &PyCObject_Type, &pobj))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    PP_free(p);   /*  SC_sfree(p); */
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mem_print - */

static char PP_mem_print__doc__[] = 
""
;

static PyObject *
PP_mem_print(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:mem_print", kw_list,
                                     &PyCObject_Type, &pobj))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    SC_mem_print(p);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mem_trace - */

static char PP_mem_trace__doc__[] = 
""
;

static PyObject *
PP_mem_trace(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    int result;

    result = SC_mem_trace();
    return PyInt_FromLong((long) result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_reg_mem - */

static char PP_reg_mem__doc__[] = 
""
;

static PyObject *
PP_reg_mem(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    long length;
    char *name;
    char *kw_list[] = {"p", "length", "name", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!ls:reg_mem", kw_list,
                                     &PyCObject_Type, &pobj, &length, &name))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    result = SC_reg_mem(p, length, name);
    return PyInt_FromLong((long) result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_dereg_mem - */

static char PP_dereg_mem__doc__[] = 
""
;

static PyObject *
PP_dereg_mem(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:dereg_mem", kw_list,
                                     &PyCObject_Type, &pobj))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    result = SC_dereg_mem(p);
    return PyInt_FromLong((long) result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mem_lookup - */

static char PP_mem_lookup__doc__[] = 
""
;

static PyObject *
PP_mem_lookup(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};
    char *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:mem_lookup", kw_list,
                                     &PyCObject_Type, &pobj))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    result = SC_mem_lookup(p);
    return Py_BuildValue("s", result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mem_monitor - */

static char PP_mem_monitor__doc__[] = 
""
;

static PyObject *
PP_mem_monitor(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
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

static char PP_mem_chk__doc__[] = 
""
;

static PyObject *
PP_mem_chk(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
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
        rv = PyInt_FromLong(result);
    }
    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_is_score_ptr - */

static char PP_is_score_ptr__doc__[] = 
""
;

static PyObject *
PP_is_score_ptr(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:is_score_ptr", kw_list,
                                     &PyCObject_Type, &pobj))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    result = SC_is_score_ptr(p);
    return PyInt_FromLong((long) result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_arrlen - */

static char PP_arrlen__doc__[] = 
""
;

static PyObject *
PP_arrlen(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};
    long result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:arrlen", kw_list,
                                     &PyCObject_Type, &pobj))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    result = SC_arrlen(p);
    return PyInt_FromLong(result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_mark - */

static char PP_mark__doc__[] = 
""
;

static PyObject *
PP_mark(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    int n;
    char *kw_list[] = {"p", "n", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!i:mark", kw_list,
                                     &PyCObject_Type, &pobj, &n))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    result = SC_mark(p, n);
    return PyInt_FromLong((long) result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_ref_count - */

static char PP_ref_count__doc__[] = 
""
;

static PyObject *
PP_ref_count(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:ref_count", kw_list,
                                     &PyCObject_Type, &pobj))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    result = SC_ref_count(p);
    return PyInt_FromLong((long) result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_set_count - */

static char PP_set_count__doc__[] = 
""
;

static PyObject *
PP_set_count(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    int n;
    char *kw_list[] = {"p", "n", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!i:set_count", kw_list,
                                     &PyCObject_Type, &pobj, &n))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    result = SC_set_count(p, n);
    return PyInt_FromLong((long) result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_permanent - */

static char PP_permanent__doc__[] = 
""
;

static PyObject *
PP_permanent(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    char *kw_list[] = {"p", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:permanent", kw_list,
                                     &PyCObject_Type, &pobj))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    SC_permanent(p);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_arrtype - */

static char PP_arrtype__doc__[] = 
""
;

static PyObject *
PP_arrtype(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PyObject *pobj;
    void *p;
    int type;
    char *kw_list[] = {"p", "type", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!i:arrtype", kw_list,
                                     &PyCObject_Type, &pobj, &type))
        return NULL;
    p = (void *) PyCObject_AsVoidPtr(pobj);
    result = SC_arrtype(p, type);
    return PyInt_FromLong((long) result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_pause - */

static char PP_pause__doc__[] = 
""
;

static PyObject *
PP_pause(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    SC_pause();
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_set_io_hooks - */

static char PP_set_io_hooks__doc__[] = 
""
;

static PyObject *
PP_set_io_hooks(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    int whch;
    char *kw_list[] = {"whch", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:set_io_hooks", kw_list,
                                     &whch))
        return NULL;
    SC_set_io_hooks(whch);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_setform - */

static char PP_setform__doc__[] = 
""
;

static PyObject *
PP_setform(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    int ok;
    int array, structure, scalar;
    char *kw_list[] = {"array", "struct", "scalar", NULL};

    array = AS_NONE;
    structure = AS_NONE;
    scalar = AS_NONE;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iii:setform", kw_list,
                                     &array, &structure, &scalar))
        return NULL;

    ok = _PP_assign_form(&PP_global_form, array, structure, scalar);
    if (ok < 0)
        return NULL;

    return Py_BuildValue("(iii)",
                         PP_global_form.array_kind,
                         PP_global_form.struct_kind,
                         PP_global_form.scalar_kind);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_make_set_1d - */

static char PP_make_set_1d__doc__[] = 
""
;

static PyObject *
PP_make_set_1d(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    char *name;
    char *type;
    int cp;
    int nd;
    int max;
    int nde;
    double *elem;
    char *kw_list[] = {"name", "type", "cp", "nd", "max", "nde", "elem", NULL};
    PM_set *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssiiiiO&:make_set_1d", kw_list,
                                     &name, &type, &cp, &nd, &max, &nde, REAL_array_extractor, &elem))
        return NULL;
    result = PM_make_set(name, type, cp, nd, max, nde, elem);
    return PPset_from_ptr(result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_make_ac_set - */

static char PP_make_ac_set__doc__[] = 
""
;

static PyObject *
PP_make_ac_set(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    char *name;
    char *type;
    int cp;
    PM_mesh_topology *mt;
    int nde;
    double *x;
    double *y;
    PP_mesh_topologyObject *mtobj;
    char *kw_list[] = {"name", "type", "cp", "mt", "nde", "x", "y", NULL};
    PM_set *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssiO!iO&O&:make_ac_set", kw_list,
                                     &name, &type, &cp, &PP_mesh_topology_Type, &mtobj, &nde, REAL_array_extractor, &x, REAL_array_extractor, &y))
        return NULL;
    mt = mtobj->mt;
    result = PM_make_ac_set(name, type, cp, mt, nde, x, y);
    return PPset_from_ptr(result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_open_vif - */

static char PP_open_vif__doc__[] = 
""
;

static PyObject *
PP_open_vif(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    char *name;
    char *kw_list[] = {"name", NULL};
    PP_file *fileinfo;
    PP_PDBfileObject *fileobj;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:open_vif", kw_list,
                                     &name))
        return NULL;
    fileinfo = _PP_open_vif(name);
    if (fileinfo == NULL)
        return NULL;
    fileobj = _PP_PDBfile_newobj(NULL, fileinfo);

    /* store singleton instance */
    PyDict_SetItemString(PP_open_file_dict, name, (PyObject *) fileobj);

    return (PyObject *) fileobj;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_getdefstr - */

static char PP_getdefstr__doc__[] = 
""
;

static PyObject *
PP_getdefstr(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PP_pdbdataObject *obj;
    PP_defstrObject *rv;
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

static char PP_gettype__doc__[] = 
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
    return PyString_FromString(obj->type);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_getfile - */

static char PP_getfile__doc__[] = 
""
;

static PyObject *
PP_getfile(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
    PP_pdbdataObject *obj;
    PP_PDBfileObject *file;
    char *kw_list[] = {"obj", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:getfile", kw_list,
                                     &PP_pdbdata_Type, &obj))
        return NULL;

    file = (PP_PDBfileObject *) _PP_find_file_obj(obj->fileinfo->file);
    if (file == NULL)
        return NULL;

    Py_INCREF(file);
    return (PyObject *) file;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_getdata - */

static char PP_getdata__doc__[] = 
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
    return PyCObject_FromVoidPtr(obj->data, NULL);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_getmember - */

static char PP_getmember__doc__[] = 
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

static char PP_unpack__doc__[] = 
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

static char PP_make_graph_1d__doc__[] = 
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

static char PP_make_graph_r2_r1__doc__[] = 
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

/* PP_set_line_info - */

static char PP_set_line_info__doc__[] = 
""
;

static PyObject *
PP_set_line_info(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.method.set_line_info) UNMODIFIED */
    pcons *info;
    int type;
    int axis_type;
    int style;
    int scatter;
    int marker;
    int color;
    int start;
    double width;
    char *kw_list[] = {"info", "type", "axis_type", "style", "scatter", "marker", "color", "start", "width", NULL};
    pcons *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&iiiiiiid:set_line_info", kw_list,
                                     PP_assoc_extractor, &info, &type, &axis_type, &style, &scatter, &marker, &color, &start, &width))
        return NULL;
    result = PG_set_line_info(info, type, axis_type, style, scatter, marker, color, start, width);
    return PP_assoc_from_ptr(result);
/* DO-NOT-DELETE splicer.end(pgs.method.set_line_info) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_set_tds_info - */

static char PP_set_tds_info__doc__[] = 
""
;

static PyObject *
PP_set_tds_info(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.method.set_tds_info) UNMODIFIED */
    pcons *info;
    int type;
    int axis_type;
    int style;
    int color;
    int nlev;
    double ratio;
    double width;
    double theta;
    double phi;
    double chi;
    double d;
    char *kw_list[] = {"info", "type", "axis_type", "style", "color", "nlev", "ratio", "width", "theta", "phi", "chi", "d", NULL};
    pcons *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&iiiiidddddd:set_tds_info", kw_list,
                                     PP_assoc_extractor, &info, &type, &axis_type, &style, &color, &nlev, &ratio, &width, &theta, &phi, &chi, &d))
        return NULL;
    result = PG_set_tds_info(info, type, axis_type, style, color, nlev, ratio, width, theta, phi, chi, d);
    return PP_assoc_from_ptr(result);
/* DO-NOT-DELETE splicer.end(pgs.method.set_tds_info) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_set_tdv_info - */

static char PP_set_tdv_info__doc__[] = 
""
;

static PyObject *
PP_set_tdv_info(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.method.set_tdv_info) UNMODIFIED */
    pcons *info;
    int type;
    int axis_type;
    int style;
    int color;
    double width;
    char *kw_list[] = {"info", "type", "axis_type", "style", "color", "width", NULL};
    pcons *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&iiiid:set_tdv_info", kw_list,
                                     PP_assoc_extractor, &info, &type, &axis_type, &style, &color, &width))
        return NULL;
    result = PG_set_tdv_info(info, type, axis_type, style, color, width);
    return PP_assoc_from_ptr(result);
/* DO-NOT-DELETE splicer.end(pgs.method.set_tdv_info) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_make_image - */

static char PP_make_image__doc__[] = 
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

static char PP_get_processor_number__doc__[] = 
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
    return PyInt_FromLong((long) result);
/* DO-NOT-DELETE splicer.end(pgs.method.get_processor_number) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_get_number_processors - */

static char PP_get_number_processors__doc__[] = 
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
    return PyInt_FromLong((long) result);
/* DO-NOT-DELETE splicer.end(pgs.method.get_number_processors) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_iso_limit - */

static char PP_iso_limit__doc__[] = 
""
;

static PyObject *
PP_iso_limit(
  PyObject *self,    /* not used */
  PyObject *args,
  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.method.iso_limit) UNMODIFIED */
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
/* DO-NOT-DELETE splicer.end(pgs.method.iso_limit) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pgs.extra_methods) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.extra_methods) */

/*--------------------------------------------------------------------------*/

static PyMethodDef PP_methods[] = {
{"print_controls", (PyCFunction)PP_print_controls, METH_KEYWORDS, PP_print_controls__doc__},
{"zero_space", (PyCFunction)PP_zero_space, METH_KEYWORDS, PP_zero_space__doc__},
{"alloc", (PyCFunction)PP_alloc, METH_KEYWORDS, PP_alloc__doc__},
{"realloc", (PyCFunction)PP_realloc, METH_KEYWORDS, PP_realloc__doc__},
{"sfree", (PyCFunction)PP_sfree, METH_KEYWORDS, PP_sfree__doc__},
{"mem_print", (PyCFunction)PP_mem_print, METH_KEYWORDS, PP_mem_print__doc__},
{"mem_trace", (PyCFunction)PP_mem_trace, METH_NOARGS, PP_mem_trace__doc__},
{"reg_mem", (PyCFunction)PP_reg_mem, METH_KEYWORDS, PP_reg_mem__doc__},
{"dereg_mem", (PyCFunction)PP_dereg_mem, METH_KEYWORDS, PP_dereg_mem__doc__},
{"mem_lookup", (PyCFunction)PP_mem_lookup, METH_KEYWORDS, PP_mem_lookup__doc__},
{"mem_monitor", (PyCFunction)PP_mem_monitor, METH_KEYWORDS, PP_mem_monitor__doc__},
{"mem_chk", (PyCFunction)PP_mem_chk, METH_KEYWORDS, PP_mem_chk__doc__},
{"is_score_ptr", (PyCFunction)PP_is_score_ptr, METH_KEYWORDS, PP_is_score_ptr__doc__},
{"arrlen", (PyCFunction)PP_arrlen, METH_KEYWORDS, PP_arrlen__doc__},
{"mark", (PyCFunction)PP_mark, METH_KEYWORDS, PP_mark__doc__},
{"ref_count", (PyCFunction)PP_ref_count, METH_KEYWORDS, PP_ref_count__doc__},
{"set_count", (PyCFunction)PP_set_count, METH_KEYWORDS, PP_set_count__doc__},
{"permanent", (PyCFunction)PP_permanent, METH_KEYWORDS, PP_permanent__doc__},
{"arrtype", (PyCFunction)PP_arrtype, METH_KEYWORDS, PP_arrtype__doc__},
{"pause", (PyCFunction)PP_pause, METH_NOARGS, PP_pause__doc__},
{"set_io_hooks", (PyCFunction)PP_set_io_hooks, METH_KEYWORDS, PP_set_io_hooks__doc__},
{"setform", (PyCFunction)PP_setform, METH_KEYWORDS, PP_setform__doc__},
{"make_set_1d", (PyCFunction)PP_make_set_1d, METH_KEYWORDS, PP_make_set_1d__doc__},
{"make_ac_set", (PyCFunction)PP_make_ac_set, METH_KEYWORDS, PP_make_ac_set__doc__},
{"open_vif", (PyCFunction)PP_open_vif, METH_KEYWORDS, PP_open_vif__doc__},
{"getdefstr", (PyCFunction)PP_getdefstr, METH_KEYWORDS, PP_getdefstr__doc__},
{"gettype", (PyCFunction)PP_gettype, METH_KEYWORDS, PP_gettype__doc__},
{"getfile", (PyCFunction)PP_getfile, METH_KEYWORDS, PP_getfile__doc__},
{"getdata", (PyCFunction)PP_getdata, METH_KEYWORDS, PP_getdata__doc__},
{"getmember", (PyCFunction)PP_getmember, METH_KEYWORDS, PP_getmember__doc__},
{"unpack", (PyCFunction)PP_unpack, METH_KEYWORDS, PP_unpack__doc__},
{"make_graph_1d", (PyCFunction)PP_make_graph_1d, METH_KEYWORDS, PP_make_graph_1d__doc__},
{"make_graph_r2_r1", (PyCFunction)PP_make_graph_r2_r1, METH_KEYWORDS, PP_make_graph_r2_r1__doc__},
{"set_line_info", (PyCFunction)PP_set_line_info, METH_KEYWORDS, PP_set_line_info__doc__},
{"set_tds_info", (PyCFunction)PP_set_tds_info, METH_KEYWORDS, PP_set_tds_info__doc__},
{"set_tdv_info", (PyCFunction)PP_set_tdv_info, METH_KEYWORDS, PP_set_tdv_info__doc__},
{"make_image", (PyCFunction)PP_make_image, METH_KEYWORDS, PP_make_image__doc__},
{"get_processor_number", (PyCFunction)PP_get_processor_number, METH_NOARGS, PP_get_processor_number__doc__},
{"get_number_processors", (PyCFunction)PP_get_number_processors, METH_NOARGS, PP_get_number_processors__doc__},
{"iso_limit", (PyCFunction)PP_iso_limit, METH_KEYWORDS, PP_iso_limit__doc__},
/* DO-NOT-DELETE splicer.begin(pgs.extra_mlist) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.extra_mlist) */
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* init_pgs - Initialization function for the module
 * (*must* be called init_pgs) */

static char pgs_module_documentation[] = 
""
;

DL_EXPORT(void)
init_pgs(void)
{
    PyObject *m, *d;
/* DO-NOT-DELETE splicer.begin(pgs._init_locals) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs._init_locals) */

    /* Create the module and add the functions */
    m = Py_InitModule4("_pgs", PP_methods,
                       pgs_module_documentation,
                       (PyObject*)NULL,PYTHON_API_VERSION);
    if (m == NULL)
        return;

    if (PyModule_AddIntConstant(m, "TRUE", (long) TRUE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "FALSE", (long) FALSE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "ON", (long) ON) < 0)
        return;
    if (PyModule_AddIntConstant(m, "OFF", (long) OFF) < 0)
        return;
    if (PyModule_AddObject(m, "SMALL", PyFloat_FromDouble((double) SMALL)) < 0)
        return;
    if (PyModule_AddObject(m, "HUGE", PyFloat_FromDouble((double) HUGE)) < 0)
        return;
    if (PyModule_AddIntConstant(m, "Z_CENT", (long) Z_CENT) < 0)
        return;
    if (PyModule_AddIntConstant(m, "N_CENT", (long) N_CENT) < 0)
        return;
    if (PyModule_AddIntConstant(m, "F_CENT", (long) F_CENT) < 0)
        return;
    if (PyModule_AddIntConstant(m, "E_CENT", (long) E_CENT) < 0)
        return;
    if (PyModule_AddIntConstant(m, "U_CENT", (long) U_CENT) < 0)
        return;
    if (PyModule_AddObject(m, "TOLERANCE", PyFloat_FromDouble((double) TOLERANCE)) < 0)
        return;
    if (PyModule_AddStringConstant(m, "AC_S", PM_AC_S) < 0)
        return;
    if (PyModule_AddStringConstant(m, "LR_S", PM_LR_S) < 0)
        return;
    if (PyModule_AddStringConstant(m, "MAP_INFO_P_S", PM_MAP_INFO_P_S) < 0)
        return;
    if (PyModule_AddStringConstant(m, "MAPPING_P_S", PM_MAPPING_P_S) < 0)
        return;
    if (PyModule_AddStringConstant(m, "SET_P_S", PM_SET_P_S) < 0)
        return;
    if (PyModule_AddStringConstant(m, "MAP_INFO_S", PM_MAP_INFO_S) < 0)
        return;
    if (PyModule_AddStringConstant(m, "MAPPING_S", PM_MAPPING_S) < 0)
        return;
    if (PyModule_AddStringConstant(m, "MESH_TOPOLOGY_S", PM_MESH_TOPOLOGY_S) < 0)
        return;
    if (PyModule_AddStringConstant(m, "MESH_TOPOLOGY_P_S", PM_MESH_TOPOLOGY_P_S) < 0)
        return;
    if (PyModule_AddStringConstant(m, "SET_S", PM_SET_S) < 0)
        return;
    if (PyModule_AddIntConstant(m, "LINE_SOLID", (long) LINE_SOLID) < 0)
        return;
    if (PyModule_AddIntConstant(m, "LINE_DASHED", (long) LINE_DASHED) < 0)
        return;
    if (PyModule_AddIntConstant(m, "LINE_DOTTED", (long) LINE_DOTTED) < 0)
        return;
    if (PyModule_AddIntConstant(m, "LINE_DOTDASHED", (long) LINE_DOTDASHED) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_LINESTYLE", (long) AXIS_LINESTYLE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_LINETHICK", (long) AXIS_LINETHICK) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_LINECOLOR", (long) AXIS_LINECOLOR) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_LABELCOLOR", (long) AXIS_LABELCOLOR) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_LABELSIZE", (long) AXIS_LABELSIZE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_LABELFONT", (long) AXIS_LABELFONT) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_LABELPREC", (long) AXIS_LABELPREC) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_X_FORMAT", (long) AXIS_X_FORMAT) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_Y_FORMAT", (long) AXIS_Y_FORMAT) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_TICKSIZE", (long) AXIS_TICKSIZE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_GRID_ON", (long) AXIS_GRID_ON) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AXIS_CHAR_ANGLE", (long) AXIS_CHAR_ANGLE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "GRID_LINESTYLE", (long) GRID_LINESTYLE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "GRID_LINETHICK", (long) GRID_LINETHICK) < 0)
        return;
    if (PyModule_AddIntConstant(m, "GRID_LINECOLOR", (long) GRID_LINECOLOR) < 0)
        return;
    if (PyModule_AddIntConstant(m, "VEC_SCALE", (long) VEC_SCALE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "VEC_ANGLE", (long) VEC_ANGLE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "VEC_HEADSIZE", (long) VEC_HEADSIZE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "VEC_FIXSIZE", (long) VEC_FIXSIZE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "VEC_MAXSIZE", (long) VEC_MAXSIZE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "VEC_LINESTYLE", (long) VEC_LINESTYLE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "VEC_LINETHICK", (long) VEC_LINETHICK) < 0)
        return;
    if (PyModule_AddIntConstant(m, "VEC_COLOR", (long) VEC_COLOR) < 0)
        return;
    if (PyModule_AddIntConstant(m, "VEC_FIXHEAD", (long) VEC_FIXHEAD) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_CARTESIAN", (long) PLOT_CARTESIAN) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_POLAR", (long) PLOT_POLAR) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_INSEL", (long) PLOT_INSEL) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_HISTOGRAM", (long) PLOT_HISTOGRAM) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_LOGICAL", (long) PLOT_LOGICAL) < 0)
        return;
    if (PyModule_AddIntConstant(m, "CARTESIAN_3D", (long) CARTESIAN_3D) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_CURVE", (long) PLOT_CURVE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_CONTOUR", (long) PLOT_CONTOUR) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_IMAGE", (long) PLOT_IMAGE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_WIRE_MESH", (long) PLOT_WIRE_MESH) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_SURFACE", (long) PLOT_SURFACE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_VECTOR", (long) PLOT_VECTOR) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_FILL_POLY", (long) PLOT_FILL_POLY) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_MESH", (long) PLOT_MESH) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_ERROR_BAR", (long) PLOT_ERROR_BAR) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_SCATTER", (long) PLOT_SCATTER) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_DV_BND", (long) PLOT_DV_BND) < 0)
        return;
    if (PyModule_AddIntConstant(m, "PLOT_DEFAULT", (long) PLOT_DEFAULT) < 0)
        return;

    PP_field_Type.tp_new = PyType_GenericNew;
    PP_field_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_field_Type) < 0)
        return;
    PP_mesh_topology_Type.tp_new = PyType_GenericNew;
    PP_mesh_topology_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_mesh_topology_Type) < 0)
        return;
    PP_set_Type.tp_new = PyType_GenericNew;
    PP_set_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_set_Type) < 0)
        return;
    PP_mapping_Type.tp_new = PyType_GenericNew;
    PP_mapping_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_mapping_Type) < 0)
        return;
    PP_defstr_Type.tp_new = PyType_GenericNew;
    PP_defstr_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_defstr_Type) < 0)
        return;
    PP_pdbdata_Type.tp_new = PyType_GenericNew;
    PP_pdbdata_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_pdbdata_Type) < 0)
        return;
    PP_hashtab_Type.tp_new = PyType_GenericNew;
    PP_hashtab_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_hashtab_Type) < 0)
        return;
    PP_assoc_Type.tp_new = PyType_GenericNew;
    PP_assoc_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_assoc_Type) < 0)
        return;
    PP_memdes_Type.tp_new = PyType_GenericNew;
    PP_memdes_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_memdes_Type) < 0)
        return;
    PP_PDBfile_Type.tp_new = PyType_GenericNew;
    PP_PDBfile_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_PDBfile_Type) < 0)
        return;
    PP_palette_Type.tp_new = PyType_GenericNew;
    PP_palette_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_palette_Type) < 0)
        return;
    PP_graph_Type.tp_new = PyType_GenericNew;
    PP_graph_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_graph_Type) < 0)
        return;
    PP_image_Type.tp_new = PyType_GenericNew;
    PP_image_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_image_Type) < 0)
        return;
    PP_device_Type.tp_new = PyType_GenericNew;
    PP_device_Type.tp_alloc = PyType_GenericAlloc;
    if (PyType_Ready(&PP_device_Type) < 0)
        return;

  /* Add some symbolic constants to the module */
    d = PyModule_GetDict(m);
    if (d == NULL)
        return;

    if (PyDict_SetItemString(d, "field", (PyObject *) &PP_field_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "mesh_topology", (PyObject *) &PP_mesh_topology_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "set", (PyObject *) &PP_set_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "mapping", (PyObject *) &PP_mapping_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "defstr", (PyObject *) &PP_defstr_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "pdbdata", (PyObject *) &PP_pdbdata_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "hashtab", (PyObject *) &PP_hashtab_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "assoc", (PyObject *) &PP_assoc_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "memdes", (PyObject *) &PP_memdes_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "PDBfile", (PyObject *) &PP_PDBfile_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "palette", (PyObject *) &PP_palette_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "graph", (PyObject *) &PP_graph_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "image", (PyObject *) &PP_image_Type) < 0)
        return;
    if (PyDict_SetItemString(d, "device", (PyObject *) &PP_device_Type) < 0)
        return;

/* DO-NOT-DELETE splicer.begin(pgs._init_body) */
    PP_init_type_map();
    
#ifdef HAVE_NUMPY
    _PP_init_numpy();
#endif

    /* add Error Exceptions */
    PP_error_internal = PyErr_NewException("pdb.internal", NULL, NULL);
    PyDict_SetItemString(d, "internal", PP_error_internal);

    PP_error_user = PyErr_NewException("pdb.error", NULL, NULL);
    PyDict_SetItemString(d, "error", PP_error_user);

/* DO-NOT-DELETE splicer.end(pgs._init_body) */

    /* Check for errors */
    if (PyErr_Occurred())
        Py_FatalError("can't initialize module _pgs");
}

/*--------------------------------------------------------------------------*/
/*                               MODULE_TAIL                                */
/*--------------------------------------------------------------------------*/
