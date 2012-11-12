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

/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.C_definition) */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_flush__doc__[] = 
""
;

static PyObject *
PP_PDBfile_flush(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.flush) UNMODIFIED */
    PD_flush(self->object);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.flush) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_extra_defstr_delete - Delete any defstrs that are not marked.
 *    This will delete any defstrs added after the inital mark.
 *    (_PP_extra_defstr_mark)
 *    Do nothing, don't even change the refcount
 *    if it is not save to free.
 */

static int _PP_extra_defstr_delete(haelem *hp, void *arg)
{
    int ok;
    defstr *dp;
    PDBfile *fp = arg;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &dp, FALSE);
    SC_ASSERT(ok == TRUE);

    if (SC_safe_to_free(dp)) {
        char *name = CSTRSAVE(dp->type);
        PP_defstr_rem(name, fp);
        CFREE(name);
    }
#if 0
    int dorm;
    if (SC_safe_to_free(dp)) {
        char *name = CSTRSAVE(dp->type);
        PP_defstr_rem(name, fp);
        CFREE(name);
    } else {
        /* Do nothing, don't even change the refcount
         * so the extra mark will remain.
         */
        dorm = FALSE;
    }
#endif
    
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_close__doc__[] = 
""
;

static PyObject *
PP_PDBfile_close(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.close) */
    int err;
    PP_file *fileinfo;
    PDBfile *fp;

    fp = self->object;
    fileinfo = self->fileinfo;
    
    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    if (fp == PP_vif) {
        /* XXX - special case for now for testsuite */
        /* cleanup defstr constructors */
        _PP_cleanup_defstrs(fp);

        /* cleanup defstr singletons */
	SC_free_hasharr(fileinfo->deftypes, _PP_decref_object, NULL);

        fileinfo->deftypes   = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
        /* closing the virtual internal file will free up memory
         * from defstrs thus allowing leak-detection in the
         * test suite to work
         */
	err = SC_hasharr_foreach(fp->host_chart, _PP_extra_defstr_delete, fp);

    } else {
        err = PyDict_DelItemString(PP_open_file_dict, fp->name);
        if (err < 0) {
            PP_error_set_user((PyObject *) self,
                              "File not found in pdb.files: %s",
                              fp->name);
            return NULL;
        }
        _PP_close_file(fileinfo);
        self->object   = NULL;
        self->fileinfo = NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.close) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_write__doc__[] = 
""
;

static PyObject *
PP_PDBfile_write(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.write) */
    int ok, nd;
    long ind[MAXDIM * 3];
    char *name, *outtype;
    PyObject *obj, *indobj;
    char *kw_list[] = {"name", "var", "outtype", "ind", NULL};
    void *vr;
    PP_descr *descr, *olddescr;
    PDBfile *fp;
    PP_file *fileinfo;

    fileinfo = self->fileinfo;
    if (fileinfo == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    fp = fileinfo->file;
    
    outtype = NULL;
    indobj = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO|sO:write", kw_list,
                                     &name, &obj, &outtype, &indobj))
        return NULL;

    descr = PP_get_object_descr(fileinfo, obj);
    if (descr == NULL)
        return NULL;

    olddescr = descr;
    descr = PP_outtype_descr(fp, descr, outtype);
    _PP_rl_descr(olddescr);
    if (descr == NULL) {
        return NULL;
    }
    
    if (descr->data == NULL) {
        ok = PP_make_data(obj, fileinfo, descr->type, descr->dims, &vr);
        if (ok == -1)
            return NULL;
    } else {
        vr = descr->data;
    }

    if (indobj != NULL) {
        nd = PP_obj_to_ind(indobj, ind);
        ok = PD_write_as_alt(fp, name, descr->type, descr->type,
                             vr, nd, ind);
    } else if (descr->dims == NULL) {
        ok = PD_write_as(fp, name, descr->type, descr->type, vr);
    } else {
        nd = PP_dimdes_to_ind(descr->dims, ind);
        if (nd < 0)
            return NULL;
        ok = PD_write_as_alt(fp, name, descr->type, descr->type,
                             vr, nd, ind);
    }
    
    if (descr->data == NULL) {
        /* clean up memory here */
        char *ptype;
        ptype = PP_add_indirection(descr->type, 1);
        (void) _PP_rel_syment(fp->host_chart, (char *) &vr, 1, ptype);
/*        (void) _PP_rel_syment(fp->host_chart, &vr, nitems, descr->ptype);*/
        CFREE(vr);
        CFREE(ptype);
    }
    _PP_rl_descr(descr);
    
    if (ok == FALSE) {
        PyErr_SetString(PP_error_user, PD_get_error());
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.write) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_write_raw__doc__[] = 
""
;

static PyObject *
PP_PDBfile_write_raw(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.write_raw) */
    int ok, nd, buffer_len;
    long ind[MAXDIM * 3];
    char *name, *type, *buffer;
    char *kw_list[] = {"name", "var", "type", "ind", NULL};
    PDBfile *fp;
    PyObject *indobj;

    fp = self->object;
    indobj = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "st#s|O:write_raw", kw_list,
                                     &name, &buffer, &buffer_len, &type, &indobj))
        return NULL;

    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    if (indobj == NULL) {
        ok = PD_write(fp, name, type, buffer);
    } else {
        nd = PP_obj_to_ind(indobj, ind);
        if (nd < 0)
            return NULL;
        ok = PD_write_alt(fp, name, type, buffer, nd, ind);
    }
    if (ok == FALSE) {
        PyErr_SetString(PP_error_user, PD_get_error());
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.write_raw) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_read__doc__[] = 
""
;

static PyObject *
PP_PDBfile_read(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.read) */
    int nd;
    long ind[MAXDIM * 3];
    char *name, *intype;
    PyObject *vr, *indobj;
    char *kw_list[] = {"name", "intype", "ind", NULL};
    PDBfile *fp;

    fp = self->object;
    intype = NULL;
    indobj = NULL;
    nd = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|sO:read", kw_list,
                                     &name, &intype, &indobj))
        return NULL;

    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    if (indobj != NULL) {
        nd = PP_obj_to_ind(indobj, ind);
        if (nd < 0)
            return NULL;
    };

    vr = _PP_read_data(self->fileinfo, name, intype, nd, ind,
                       &self->form);
    if (vr == NULL) {
        return NULL;
    }
    return vr;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.read) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_defent__doc__[] = 
""
;

static PyObject *
PP_PDBfile_defent(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.defent) */
    char *name;
    char *type;
    char *kw_list[] = {"name", "type", NULL};
    PDBfile *fp;

    fp = self->object;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss:defent", kw_list,
                                     &name, &type))
        return NULL;

    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    PD_defent(fp, name, type);

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.defent) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_defstr__doc__[] = 
""
;

static PyObject *
PP_PDBfile_defstr(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.defstr) */
    char *name;
    char *kw_list[] = {"name", "members", NULL};
    PyObject *members;
    PDBfile *fp;
    PP_defstrObject *rv;

    fp = self->object;
    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    members = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|O:defstr", kw_list,
                                     &name, &members))
        return NULL;

    if (members == NULL) {
        rv = _PP_defstr_find_singleton(name, NULL, self->fileinfo);
    } else {
        rv = _PP_defstr_make_singleton(NULL, name, members, self->fileinfo);
    }
    if (rv == NULL)
        return NULL;
    Py_INCREF(rv);
    
    return (PyObject *) rv;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.defstr) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_cd__doc__[] = 
""
;

static PyObject *
PP_PDBfile_cd(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.cd) */
    int ierr;
    char *dirname;
    char *kw_list[] = {"dirname", NULL};
    PDBfile *fp;

    fp = self->object;
    dirname = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|z:cd", kw_list,
                                     &dirname))
        return NULL;

    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    ierr = PD_cd(fp, dirname);
    if (ierr == FALSE) {
        PP_error_from_pdb();
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.cd) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_mkdir__doc__[] = 
""
;

static PyObject *
PP_PDBfile_mkdir(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.mkdir) */
    int ierr;
    char *dirname;
    char *kw_list[] = {"dirname", NULL};
    PDBfile *fp;

    fp = self->object;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:mkdir", kw_list,
                                     &dirname))
        return NULL;
    
    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    ierr = PD_mkdir(fp, dirname);

    if (ierr == FALSE) {
        PP_error_from_pdb();
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.mkdir) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_isdir__doc__[] = 
""
;

static PyObject *
PP_PDBfile_isdir(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.isdir) */
    int ierr;
    char *dirname;
    char *kw_list[] = {"dirname", NULL};
    PDBfile *fp;
    PyObject *rv;

    fp = self->object;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:isdir", kw_list,
                                     &dirname))
        return NULL;
    
    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    ierr = PD_isdir(fp, dirname);

    rv = PyInt_FromLong((long) ierr);

    return rv;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.isdir) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_ln__doc__[] = 
""
;

static PyObject *
PP_PDBfile_ln(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.ln) */
    int ierr;
    char *var, *link;
    char *kw_list[] = {"var", "link", NULL};
    PDBfile *fp;

    fp = self->object;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss:ln", kw_list,
                                     &var, &link))
        return NULL;

    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }
    
    ierr = PD_ln(fp, var, link);
    if (ierr == FALSE) {
        PP_error_from_pdb();
        return NULL;
    }
    
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.ln) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_ls__doc__[] = 
""
;

static PyObject *
PP_PDBfile_ls(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.ls) */
    int ierr, num;
    char *path, *type;
    char **out;
    char *kw_list[] = {"path", "type", NULL};
    Py_ssize_t i;
    PyObject *rv, *item;
    PDBfile *fp;

    fp = self->object;
    path = NULL;
    type = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|zz:ls", kw_list,
                                     &path, &type))
        return NULL;

    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    out = PD_ls(fp, path, type, &num);
    ierr = 0;

    rv = PyTuple_New(num);
    for (i = 0; i < num; i++) {
        item = PyString_FromString(out[i]);
        if (item == NULL) {
            ierr = -1;
            break;
        }
        ierr = PyTuple_SetItem(rv, i, item);
        if (ierr < 0)
            break;
    }

    if (ierr < 0) {
        Py_DECREF(rv);
        rv = NULL;
    }
    CFREE(out);

    return rv;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.ls) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_pwd__doc__[] = 
""
;

static PyObject *
PP_PDBfile_pwd(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.pwd) */
    char *pwd;
    PyObject *rv;
    PDBfile *fp;

    fp = self->object;
    
    if (fp == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    pwd = PD_pwd(fp);

    rv = PyString_FromString(pwd);

    return rv;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.pwd) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_get_obj_descr__doc__[] = 
""
;

static PyObject *
PP_PDBfile_get_obj_descr(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.get_obj_descr) */
    void *obj, *dim, *rv;
    char *kw_list[] = {"obj", NULL};
    PP_descr *descr;

    if (self->object == NULL) {
        PP_error_set_user(NULL, "file is not open");
        return NULL;
    }

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O:get_obj_descr", kw_list,
                                     &obj))
        return NULL;

    descr = PP_get_object_descr(self->fileinfo, obj);

    if (descr == NULL) {
        PP_error_set_user(obj, "Unable to find PDB type");
        return NULL;
    } else if (descr->dims == NULL) {
        dim = Py_None;
        Py_INCREF(Py_None);
    } else {
        dim = PP_dimdes_to_obj(descr->dims);
    }
    rv = Py_BuildValue("iisO", (int) descr->typecode, descr->bpi,
                       descr->type, dim);
    _PP_rl_descr(descr);
    return rv;

/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.get_obj_descr) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_register_class__doc__[] = 
""
;

static PyObject *
PP_PDBfile_register_class(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.register_class) */
    /* This function is used to register a type.
     * It allows pypact to take an object with type and
     * extract it's information.
     *   cls  = Python class object
     *   type = defstr type
     */
    PyTypeObject *cls;
    PyFunctionObject *ctor;
    char *type;
    char *kw_list[] = {"cls", "type", "ctor", NULL};
    PP_class_descr *cdescr;
    PDBfile *fp;
    defstr *dp;

    ctor = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!s|O!:register_class", kw_list,
                                     &PyClass_Type, &cls,
                                     &type,
                                     &PyFunction_Type, &ctor))
        return NULL;

    fp = self->fileinfo->file;
    dp = PD_inquire_table_type(fp->host_chart, type);
    if (dp == NULL) {
        PP_error_set_user(NULL, "No such type %s in file %s",
                          type, fp->name);
        return NULL;
    }

    cdescr = PP_make_class_descr(cls, type, ctor);
    if (cdescr == NULL)
        return NULL;
    PP_init_type_map_instance(self->fileinfo, cdescr);

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.register_class) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_PDBfile_reset_vif__doc__[] = 
""
;

static PyObject *
PP_PDBfile_reset_vif(PP_PDBfileObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.method.reset_vif) */
    /* Clear out type info and reset with just native types
     * sort of like closing and reopening
     * Useful for testing with leak detection
     */
    PP_file *fileinfo;

    if (self->object->virtual_internal == FALSE) {
        PP_error_set_user(NULL, "file is not virtual internal");
        return NULL;
    }
    
    /* clear old */
    fileinfo = self->fileinfo;

    SC_free_hasharr(fileinfo->type_map, PP_rl_type_entry, NULL);
    SC_free_hasharr(fileinfo->object_map, PP_rl_type_entry, NULL);
    SC_free_hasharr(fileinfo->class_map, _PP_rl_class_descr, NULL);
    SC_free_hasharr(fileinfo->deftypes,  _PP_decref_object, NULL);

    _PP_cleanup_defstrs(fileinfo->file);

    /* reset */
    PP_init_type_map_basic(fileinfo);

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.method.reset_vif) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.extra_methods) */
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.extra_methods) */

/*--------------------------------------------------------------------------*/

static PyMethodDef PP_PDBfile_methods[] = {
{"flush", (PyCFunction)PP_PDBfile_flush, METH_NOARGS, PP_PDBfile_flush__doc__},
{"close", (PyCFunction)PP_PDBfile_close, METH_NOARGS, PP_PDBfile_close__doc__},
{"write", (PyCFunction)PP_PDBfile_write, METH_KEYWORDS, PP_PDBfile_write__doc__},
{"write_raw", (PyCFunction)PP_PDBfile_write_raw, METH_KEYWORDS, PP_PDBfile_write_raw__doc__},
{"read", (PyCFunction)PP_PDBfile_read, METH_KEYWORDS, PP_PDBfile_read__doc__},
{"defent", (PyCFunction)PP_PDBfile_defent, METH_KEYWORDS, PP_PDBfile_defent__doc__},
{"defstr", (PyCFunction)PP_PDBfile_defstr, METH_KEYWORDS, PP_PDBfile_defstr__doc__},
{"cd", (PyCFunction)PP_PDBfile_cd, METH_KEYWORDS, PP_PDBfile_cd__doc__},
{"mkdir", (PyCFunction)PP_PDBfile_mkdir, METH_KEYWORDS, PP_PDBfile_mkdir__doc__},
{"isdir", (PyCFunction)PP_PDBfile_isdir, METH_KEYWORDS, PP_PDBfile_isdir__doc__},
{"ln", (PyCFunction)PP_PDBfile_ln, METH_KEYWORDS, PP_PDBfile_ln__doc__},
{"ls", (PyCFunction)PP_PDBfile_ls, METH_KEYWORDS, PP_PDBfile_ls__doc__},
{"pwd", (PyCFunction)PP_PDBfile_pwd, METH_NOARGS, PP_PDBfile_pwd__doc__},
{"get_obj_descr", (PyCFunction)PP_PDBfile_get_obj_descr, METH_KEYWORDS, PP_PDBfile_get_obj_descr__doc__},
{"register_class", (PyCFunction)PP_PDBfile_register_class, METH_KEYWORDS, PP_PDBfile_register_class__doc__},
{"reset_vif", (PyCFunction)PP_PDBfile_reset_vif, METH_NOARGS, PP_PDBfile_reset_vif__doc__},
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.extra_mlist) */
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.extra_mlist) */
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object_get) */
  return PyCObject_FromVoidPtr(self->object, NULL);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_name__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_name_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.name_get) UNMODIFIED */
    return Py_BuildValue("s", self->object->name);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.name_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_type__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_type_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.type_get) UNMODIFIED */
    return Py_BuildValue("s", self->object->type);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_symtab__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_symtab_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.symtab_get) */
    return (PyObject *) PP_hashtab_newobj(NULL, self->object->symtab);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.symtab_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_chart__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_chart_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.chart_get) */
    return (PyObject *) PP_hashtab_newobj(NULL, self->object->chart);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.chart_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_host_chart__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_host_chart_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.host_chart_get) */
    return (PyObject *) PP_hashtab_newobj(NULL, self->object->host_chart);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.host_chart_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_attrtab__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_attrtab_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.attrtab_get) */
    return (PyObject *) PP_hashtab_newobj(NULL, self->object->attrtab);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.attrtab_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_previous_file__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_previous_file_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.previous_file_get) UNMODIFIED */
    return Py_BuildValue("s", self->object->previous_file);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.previous_file_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_date__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_date_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.date_get) UNMODIFIED */
    return Py_BuildValue("s", self->object->date);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.date_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_mode__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_mode_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.mode_get) */
    PyObject *rv;
    switch (PD_get_mode(self->object)) {
    case PD_READ:
        rv = PyString_FromString("PD_READ");
        break;
    case PD_WRITE:
        rv = PyString_FromString("PD_WRITE");
        break;
    case PD_APPEND:
        rv = PyString_FromString("a");
        break;
    case PD_OPEN:
        rv = PyString_FromString("r");
        break;
    case PD_CREATE:
        rv = PyString_FromString("w");
        break;
    case PD_CLOSE:
        rv = PyString_FromString("PD_CLOSE");
        break;
    case PD_TRACE:
        rv = PyString_FromString("PD_TRACE");
        break;
    case PD_PRINT:
        rv = PyString_FromString("PD_PRINT");
        break;
    default:
        rv = PyInt_FromLong((long) PD_get_mode(self->object));
        break;
    }
        
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.mode_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_default_offset__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_default_offset_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.default_offset_get) */
    return PyInt_FromLong((long) PD_get_offset(self->object));
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.default_offset_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_virtual_internal__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_virtual_internal_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.virtual_internal_get) UNMODIFIED */
    return PyInt_FromLong((long) self->object->virtual_internal);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.virtual_internal_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_system_version__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_system_version_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.system_version_get) UNMODIFIED */
    return PyInt_FromLong((long) self->object->system_version);
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.system_version_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_PDBfile_object_major_order__doc__[] =
""
;

static PyObject *
PP_PDBfile_object_major_order_get(PP_PDBfileObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.descriptor.object.major_order_get) */
    PyObject *rv;
    
    switch (PD_get_major_order(self->object)) {
    case ROW_MAJOR_ORDER:
        rv = PyString_FromString("ROW MAJOR");
        break;
    case COLUMN_MAJOR_ORDER:
        rv = PyString_FromString("COLUMN MAJOR");
        break;
    default:
        PP_error_set(PP_error_internal, NULL,
                     "major_order had unexpected value %d",
                     PD_get_major_order(self->object));
        rv = NULL;
    }
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.descriptor.object.major_order_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.extra_members) */

static PyGetSetDef PP_PDBfile_getset[] = {
    {"object", (getter) PP_PDBfile_object_get, NULL, PP_PDBfile_object__doc__, NULL},
    {"name", (getter) PP_PDBfile_object_name_get, NULL, PP_PDBfile_object_name__doc__, NULL},
    {"type", (getter) PP_PDBfile_object_type_get, NULL, PP_PDBfile_object_type__doc__, NULL},
    {"symtab", (getter) PP_PDBfile_object_symtab_get, NULL, PP_PDBfile_object_symtab__doc__, NULL},
    {"chart", (getter) PP_PDBfile_object_chart_get, NULL, PP_PDBfile_object_chart__doc__, NULL},
    {"host_chart", (getter) PP_PDBfile_object_host_chart_get, NULL, PP_PDBfile_object_host_chart__doc__, NULL},
    {"attrtab", (getter) PP_PDBfile_object_attrtab_get, NULL, PP_PDBfile_object_attrtab__doc__, NULL},
    {"previous_file", (getter) PP_PDBfile_object_previous_file_get, NULL, PP_PDBfile_object_previous_file__doc__, NULL},
    {"date", (getter) PP_PDBfile_object_date_get, NULL, PP_PDBfile_object_date__doc__, NULL},
    {"mode", (getter) PP_PDBfile_object_mode_get, NULL, PP_PDBfile_object_mode__doc__, NULL},
    {"default_offset", (getter) PP_PDBfile_object_default_offset_get, NULL, PP_PDBfile_object_default_offset__doc__, NULL},
    {"virtual_internal", (getter) PP_PDBfile_object_virtual_internal_get, NULL, PP_PDBfile_object_virtual_internal__doc__, NULL},
    {"system_version", (getter) PP_PDBfile_object_system_version_get, NULL, PP_PDBfile_object_system_version__doc__, NULL},
    {"major_order", (getter) PP_PDBfile_object_major_order_get, NULL, PP_PDBfile_object_major_order__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static int
PP_PDBfile_tp_init(PP_PDBfileObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.as_type.init) */
    char *name;
    char *mode = "r";
    char *kw_list[] = {"name", "mode", NULL};
    PDBfile *fp;
    PP_file *fileinfo;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|s:open", kw_list,
                                     &name, &mode))
        return -1;

    fp = PD_open(name, mode);
    if (fp == NULL) {
        PyErr_SetString(PP_error_user, PD_get_error());
        return -1;
    }

    /* Do not track pointers since we do lots of releasing memory in
     * places like _PP_rel_syment.
     */
    PD_set_track_pointers(fp, FALSE);

    fileinfo = CMAKE(PP_file);
    fileinfo->file = fp;
    PP_init_type_map_basic(fileinfo);
    
    self = _PP_PDBfile_newobj(self, fileinfo);
    if (self == NULL)
        return -1;

    /* store singleton instance */
    PyDict_SetItemString(PP_open_file_dict, name, (PyObject *) self);
    
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.PDBfile.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_PDBfile_Check - */

/* static */ int
PP_PDBfile_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_PDBfile_Type))
        return 1;
    else
        return 0;
/*  return op->ob_type == &PP_PDBfile_Type; */
}

/*--------------------------------------------------------------------------*/


static char PP_PDBfile_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_PDBfile_Type = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,                              /* ob_size */
        "PDBfile",                       /* tp_name */
        sizeof(PP_PDBfileObject),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)0,                  /* tp_dealloc */
        (printfunc)0,                   /* tp_print */
        (getattrfunc)0,                 /* tp_getattr */
        (setattrfunc)0,                 /* tp_setattr */
        (cmpfunc)0,                     /* tp_compare */
        (reprfunc)0,                    /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        0,                              /* tp_as_sequence */
        0,                              /* tp_as_mapping */
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
        PP_PDBfile_Type__doc__,         /* tp_doc */
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
        PP_PDBfile_methods,             /* tp_methods */
        0,                              /* tp_members */
        PP_PDBfile_getset,              /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_PDBfile_tp_init,   /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.PDBfile.extra) */

/* _PP_PDBfile_newobj -  */

PP_PDBfileObject *_PP_PDBfile_newobj(PP_PDBfileObject *obj,
                                     PP_file *fileinfo)
{

    if (obj == NULL) {
        obj = (PP_PDBfileObject *) PyType_GenericAlloc(&PP_PDBfile_Type, 0);
        if (obj == NULL) {
            return NULL;
        }
    }

    obj->object   = fileinfo->file;
    obj->fileinfo = fileinfo;
    obj->form     = PP_global_form;

    return obj;
}


/* DO-NOT-DELETE splicer.end(pdb.PDBfile.extra) */
/* End of code for PDBfile objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
