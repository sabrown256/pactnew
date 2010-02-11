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

#define _pdb_MODULE
#include "pdbmodule.h"

/* DO-NOT-DELETE splicer.begin(pdb.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.C_definition) */

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
/* DO-NOT-DELETE splicer.begin(pdb.method.print_controls) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.print_controls) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.zero_space) UNMODIFIED */
    int flag;
    char *kw_list[] = {"flag", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:zero_space", kw_list,
                                     &flag))
        return NULL;
    result = SC_zero_space(flag);
    return PyInt_FromLong((long) result);
/* DO-NOT-DELETE splicer.end(pdb.method.zero_space) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.alloc) */
    long nitems;
    long bpi;
    char *name;
    char *kw_list[] = {"nitems", "bpi", "name", NULL};
    void *result;
    PyObject *rv;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "lls:alloc", kw_list,
                                     &nitems, &bpi, &name))
        return NULL;
    result = SC_alloc(nitems, bpi, name);
    if (result != NULL) {
        SC_mark(result, 1);
        rv = PyCObject_FromVoidPtr(result, PP_free);
    } else {
        PyErr_NoMemory();
        rv = NULL;
    }
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.method.alloc) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.realloc) */
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
    result = SC_realloc(p, nitems, bpi);
    return PyCObject_FromVoidPtr((void *) result, NULL);
/* DO-NOT-DELETE splicer.end(pdb.method.realloc) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.sfree) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.sfree) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.mem_print) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.mem_print) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.mem_trace) UNMODIFIED */
    int result;

    result = SC_mem_trace();
    return PyInt_FromLong((long) result);
/* DO-NOT-DELETE splicer.end(pdb.method.mem_trace) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.reg_mem) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.reg_mem) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.dereg_mem) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.dereg_mem) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.mem_lookup) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.mem_lookup) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.mem_monitor) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.mem_monitor) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.mem_chk) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.mem_chk) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.is_score_ptr) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.is_score_ptr) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.arrlen) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.arrlen) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.mark) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.mark) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.ref_count) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.ref_count) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.set_count) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.set_count) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.permanent) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.permanent) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.arrtype) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.arrtype) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.pause) */
    SC_pause();
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.method.pause) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.set_io_hooks) UNMODIFIED */
    int whch;
    char *kw_list[] = {"whch", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:set_io_hooks", kw_list,
                                     &whch))
        return NULL;
    SC_set_io_hooks(whch);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.method.set_io_hooks) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.setform) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.setform) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.make_set_1d) UNMODIFIED */
    char *name;
    char *type;
    int cp;
    int nd;
    int max;
    int nde;
    REAL *elem;
    char *kw_list[] = {"name", "type", "cp", "nd", "max", "nde", "elem", NULL};
    PM_set *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssiiiiO&:make_set_1d", kw_list,
                                     &name, &type, &cp, &nd, &max, &nde, REAL_array_extractor, &elem))
        return NULL;
    result = PM_make_set(name, type, cp, nd, max, nde, elem);
    return PPset_from_ptr(result);
/* DO-NOT-DELETE splicer.end(pdb.method.make_set_1d) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.make_ac_set) UNMODIFIED */
    char *name;
    char *type;
    int cp;
    PM_mesh_topology *mt;
    int nde;
    REAL *x;
    REAL *y;
    PP_mesh_topologyObject *mtobj;
    char *kw_list[] = {"name", "type", "cp", "mt", "nde", "x", "y", NULL};
    PM_set *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssiO!iO&O&:make_ac_set", kw_list,
                                     &name, &type, &cp, &PP_mesh_topology_Type, &mtobj, &nde, REAL_array_extractor, &x, REAL_array_extractor, &y))
        return NULL;
    mt = mtobj->mt;
    result = PM_make_ac_set(name, type, cp, mt, nde, x, y);
    return PPset_from_ptr(result);
/* DO-NOT-DELETE splicer.end(pdb.method.make_ac_set) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.open_vif) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.open_vif) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.getdefstr) */
    PP_pdbdataObject *obj;
    PP_defstrObject *rv;
    char *kw_list[] = {"obj", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:getdefstr", kw_list,
                                     &PP_pdbdata_Type, &obj))
        return NULL;

    rv = obj->dpobj;
    
    Py_INCREF(rv);
    return (PyObject *) rv;
/* DO-NOT-DELETE splicer.end(pdb.method.getdefstr) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.gettype) */
    PP_pdbdataObject *obj;
    char *kw_list[] = {"obj", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:gettype", kw_list,
                                     &PP_pdbdata_Type, &obj))
        return NULL;
    return PyString_FromString(obj->type);
/* DO-NOT-DELETE splicer.end(pdb.method.gettype) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.getfile) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.getfile) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.getdata) */
    PP_pdbdataObject *obj;
    char *kw_list[] = {"obj", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:getdata", kw_list,
                                     &PP_pdbdata_Type, &obj))
        return NULL;
    /* XXX add deallocator */
    return PyCObject_FromVoidPtr(obj->data, NULL);
/* DO-NOT-DELETE splicer.end(pdb.method.getdata) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.getmember) */
    PP_pdbdataObject *obj;
    char *name;
    char *kw_list[] = {"obj", "name", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!s:getmember", kw_list,
                                     &PP_pdbdata_Type, &obj, &name))
        return NULL;

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.method.getmember) */
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
/* DO-NOT-DELETE splicer.begin(pdb.method.unpack) */
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
/* DO-NOT-DELETE splicer.end(pdb.method.unpack) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.extra_methods) */
/* DO-NOT-DELETE splicer.end(pdb.extra_methods) */

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
/* DO-NOT-DELETE splicer.begin(pdb.extra_mlist) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.extra_mlist) */
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

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &dp);
    SC_mark(dp, 1);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* init_pdb - Initialization function for the module
 * (*must* be called init_pdb) */

static char pdb_module_documentation[] = 
""
;

DL_EXPORT(void)
init_pdb(void)
{
    PyObject *m, *d;
/* DO-NOT-DELETE splicer.begin(pdb._init_locals) */

    /* subtype pdbdata */
    PP_hashtab_Type.tp_base = &PP_pdbdata_Type;

/* DO-NOT-DELETE splicer.end(pdb._init_locals) */

    /* Create the module and add the functions */
    m = Py_InitModule4("_pdb", PP_methods,
                       pdb_module_documentation,
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

/* DO-NOT-DELETE splicer.begin(pdb._init_body) */
    /* remove sequence behavior from hashtab that is inherited from pdbdata */
    PP_hashtab_Type.tp_as_sequence = NULL;
    
    /* make 'open' a synonym for 'PDBfile' */
    if (PyDict_SetItemString(d, "open", (PyObject *) &PP_PDBfile_Type) < 0)
        return;

    PP_init_type_map();

#ifdef HAVE_NUMPY
    _PP_init_numpy();
#endif
  
    if (PyModule_AddIntConstant(m, "AS_NONE", AS_NONE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AS_PDBDATA", AS_PDBDATA) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AS_OBJECT", AS_OBJECT) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AS_TUPLE", AS_TUPLE) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AS_LIST", AS_LIST) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AS_DICT", AS_DICT) < 0)
        return;
    if (PyModule_AddIntConstant(m, "AS_ARRAY", AS_ARRAY) < 0)
        return;
    
    PP_open_file_dict = PyDict_New();
    if (PyModule_AddObject(m, "files", PP_open_file_dict) < 0)
        return;

    PP_vif_info = _PP_open_vif("PP_vif");
    if (PP_vif_info != NULL) {
        int err;

        PP_vif_obj = _PP_PDBfile_newobj(NULL, PP_vif_info);
        PP_vif      = PP_vif_info->file;

        /* mark every currently existing defstr in the host_chart */
	err = SC_hasharr_foreach(PP_vif->host_chart, _PP_extra_defstr_mark, NULL);

        /* XXX - test err */
        if (PyModule_AddObject(m, "vif", (PyObject *) PP_vif_obj) < 0)
            return;
    }

    /* add Error Exceptions */
    PP_error_internal = PyErr_NewException("pdb.internal", NULL, NULL);
    PyDict_SetItemString(d, "internal", PP_error_internal);
    
    PP_error_user = PyErr_NewException("pdb.error", NULL, NULL);
    PyDict_SetItemString(d, "error", PP_error_user);
    
/* DO-NOT-DELETE splicer.end(pdb._init_body) */

    /* Check for errors */
    if (PyErr_Occurred())
        Py_FatalError("can't initialize module _pdb");
}

/*--------------------------------------------------------------------------*/
/*                               MODULE_TAIL                                */
/*--------------------------------------------------------------------------*/
