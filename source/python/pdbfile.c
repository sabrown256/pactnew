/*
 * PDBFILE.C
 *
 * include cpyright.h
 */

#include "pdbmodule.h"
#include "gp-pdb.h"

static char
 PY_PDBfile_close_doc[] = "",
 PY_PDBfile_write_doc[] = "",
 PY_PDBfile_write_raw_doc[] = "",
 PY_PDBfile_read_doc[] = "",
 PY_PDBfile_defstr_doc[] = "",
 PY_PDBfile_ls_doc[] = "",
 PY_PDBfile_get_obj_descr_doc[] = "",
 PY_PDBfile_register_class_doc[] = "",
 PY_PDBfile_reset_vif_doc[] = "";

/*--------------------------------------------------------------------------*/

/*                               PDBFILE ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PY_OPT_PDBFILE - handle BLANG binding related operations */

void *_PY_opt_PDBfile(PDBfile *x, bind_opt wh, void *a)
   {void *rv;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
        case BIND_LABEL :
        case BIND_PRINT :
        case BIND_FREE :
        case BIND_ALLOC :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_PDBFILE_NEWOBJ -  */

PY_PDBfile *PY_PDBfile_newobj(PY_PDBfile *obj, PP_file *fileinfo)
   {

    if (obj == NULL)
       obj = (PY_PDBfile *) PyType_GenericAlloc(&PY_PDBfile_type, 0);

    if (obj != NULL)
       {obj->pyo      = fileinfo->file;
	obj->fileinfo = fileinfo;
	obj->form     = PP_global_form;};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_open_vif - */

char PY_open_vif_doc[] = "";

PyObject *PY_open_vif(PyObject *self, PyObject *args, PyObject *kwds)
   {char *name;
    PP_file *fileinfo;
    PY_PDBfile *fileobj;
    PyObject *rv;
    char *kw_list[] = {"name", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "s:open_vif", kw_list,
				    &name))
       {fileinfo = _PP_open_vif(name);
	if (fileinfo == NULL)
	   return(NULL);
	fileobj = PY_PDBfile_newobj(NULL, fileinfo);
	rv      = (PyObject *) fileobj;

/* store singleton instance */
	PyDict_SetItemString(PP_open_file_dict, name, rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_PDBfile_flush(PY_PDBfile *self,
				  PyObject *args, PyObject *kwds)
   {

    PD_flush(self->pyo);
    Py_INCREF(Py_None);

    return(Py_None);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_extra_defstr_delete - Delete any defstrs that are not marked.
 *    This will delete any defstrs added after the inital mark.
 *    (_PP_extra_defstr_mark)
 *    Do nothing, don't even change the refcount
 *    if it is not save to free.
 */

static int _PP_extra_defstr_delete(haelem *hp, void *arg)
   {int ok;
    defstr *dp;
    PDBfile *fp = arg;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &dp, FALSE);
    SC_ASSERT(ok == TRUE);

    if (SC_safe_to_free(dp))
       {char *name;

	name = CSTRSAVE(dp->type);
        PY_defstr_rem(name, fp);
        CFREE(name);};
    
    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_PDBfile_close(PY_PDBfile *self,
				  PyObject *args, PyObject *kwds)
   {int err;
    PP_file *fileinfo;
    PDBfile *fp;
    PyObject *rv;

    rv = NULL;

    fp = self->pyo;
    fileinfo = self->fileinfo;
    
    if (fp == NULL)
       PP_error_set_user(NULL, "file is not open");

    else if (fp == PP_vif)

/* XXX - special case for now for testsuite */
/* cleanup defstr constructors */
       {_PP_cleanup_defstrs(fp);

/* cleanup defstr singletons */
	SC_free_hasharr(fileinfo->deftypes, _PP_decref_object, NULL);

        fileinfo->deftypes = SC_make_hasharr(HSZSMALL, NODOC,
					     SC_HA_NAME_KEY, 0);

/* closing the virtual internal file will free up memory
 * from defstrs thus allowing leak-detection in the
 * test suite to work
 */
	err = SC_hasharr_foreach(fp->host_chart,
				 _PP_extra_defstr_delete, fp);
	rv = Py_None;

	Py_INCREF(rv);}

    else
       {err = PyDict_DelItemString(PP_open_file_dict, fp->name);
        if (err < 0)
	   PP_error_set_user((PyObject *) self,
			     "File not found in pdb.files: %s",
			     fp->name);
	else
	   {_PP_close_file(fileinfo);
	    self->pyo      = NULL;
	    self->fileinfo = NULL;
	    rv = Py_None;

	    Py_INCREF(rv);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_PDBfile_write(PY_PDBfile *self,
				  PyObject *args, PyObject *kwds)
   {int ok, nd;
    long ind[MAXDIM * 3];
    char *name, *outtype;
    void *vr;
    PDBfile *fp;
    PP_descr *descr, *olddescr;
    PP_file *fileinfo;
    PyObject *obj, *indobj, *rv;
    char *kw_list[] = {"name", "var", "outtype", "ind", NULL};

    rv = NULL;

    fileinfo = self->fileinfo;
    if (fileinfo == NULL)
       {PP_error_set_user(NULL, "file is not open");
        return(NULL);};

    fp = fileinfo->file;
    
    outtype = NULL;
    indobj  = NULL;
    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "sO|sO:write", kw_list,
				    &name, &obj, &outtype, &indobj))
       {descr = PP_get_object_descr(fileinfo, obj);
	if (descr == NULL)
	   return(NULL);

	olddescr = descr;
	descr = PP_outtype_descr(fp, descr, outtype);
	_PP_rl_descr(olddescr);
	if (descr == NULL)
	   return(NULL);
    
	if (descr->data == NULL)
	   {ok = PP_make_data(obj, fileinfo, descr->type, descr->dims, &vr);
	    if (ok == -1)
	       return(NULL);}
	else
	   vr = descr->data;

	if (indobj != NULL)
	   {nd = PP_obj_to_ind(indobj, ind);
	    ok = PD_write_as_alt(fp, name, descr->type, descr->type,
                             vr, nd, ind);}
	else if (descr->dims == NULL)
	   ok = PD_write_as(fp, name, descr->type, descr->type, vr);
	else
	   {nd = PP_dimdes_to_ind(descr->dims, ind);
	    if (nd < 0)
	       return(NULL);
	    ok = PD_write_as_alt(fp, name, descr->type, descr->type,
				 vr, nd, ind);};
    
	if (descr->data == NULL)
	   {char *ptype;

/* clean up memory here */
	    ptype = PP_add_indirection(descr->type, 1);
	    _PP_rel_syment(fp->host_chart, (char *) &vr, 1, ptype);
/*            _PP_rel_syment(fp->host_chart, &vr, nitems, descr->ptype);*/
	    CFREE(vr);
	    CFREE(ptype);};

	_PP_rl_descr(descr);
    
	if (ok == FALSE)
	   PyErr_SetString(PP_error_user, PD_get_error());
	else
	   {rv = Py_None;
	    Py_INCREF(rv);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_PDBfile_write_raw(PY_PDBfile *self,
				      PyObject *args, PyObject *kwds)
   {int ok, nd, nb;
    long ind[MAXDIM * 3];
    char *name, *type, *bf, *fmt;
    PDBfile *fp;
    PyObject *indobj, *rv;
    char *kw_list[] = {"name", "var", "type", "ind", NULL};

    rv = NULL;

    fp     = self->pyo;
    indobj = NULL;

/* GOTCHA: ask Lee Taylor
 * python 2 worked with t#, python 3 likes s# instead
 */
#if PY_MAJOR_VERSION >= 3
    fmt = "ss#s|O:write_raw";
#else
    fmt = "st#s|O:write_raw";
#endif

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    fmt, kw_list,
				    &name,
				    &bf, &nb,
				    &type, &indobj))
       {if (fp == NULL)
	   PP_error_set_user(NULL, "file is not open");

	else
	   {

#if PY_MAJOR_VERSION >= 3

/* NOTE: python versions are inconsistent here
 * python 2 worked without str(d) and with str(d) shows:
 *  16 double   0x14a1d3c   for double[2]
 *   8 double * 0x14b10e4   for double *  pointing to 4 doubles
 * python 3 requires str(d) and shows:
 *  61 double   0x7f2e4e5d3f40   for double[2]
 *  32 double * 0x7f2e4e51b530   for double *  pointing to 4 doubles
 * for the diagnostic print:
 *    fprintf(stderr, "-> %d %s %p\n", nb, type, bf);
 * so python 3 is inconsistent about type and length
 */
            if (_PD_indirection(type) == TRUE)
               {char ty[BFSML];
                char *t, **p;

                SC_strncpy(ty, BFSML, type, -1);

                t = CMAKE_N(char, nb);
                memcpy(t, bf, nb);

                for ( ; _PD_indirection(ty) == TRUE; t = (char *) p)
		    {p  = CMAKE(char *);
                     *p = t;
                     PD_dereference(ty);};
                bf = (char *) p;};
#endif
            if (indobj == NULL)
	       ok = PD_write(fp, name, type, bf);
	    else
	       {nd = PP_obj_to_ind(indobj, ind);
		if (nd < 0)
		   return(NULL);
		ok = PD_write_alt(fp, name, type, bf, nd, ind);};

	    if (ok == FALSE)
	       PyErr_SetString(PP_error_user, PD_get_error());
	    else
	       {rv = Py_None;
		Py_INCREF(rv);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_PDBfile_read(PY_PDBfile *self,
				 PyObject *args, PyObject *kwds)
   {int nd;
    long ind[MAXDIM * 3];
    char *name, *intype;
    PyObject *indobj, *rv;
    PDBfile *fp;
    char *kw_list[] = {"name", "intype", "ind", NULL};

    rv = NULL;

    fp     = self->pyo;
    intype = NULL;
    indobj = NULL;
    nd     = 0;
    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "s|sO:read", kw_list,
				    &name, &intype, &indobj))
       {if (fp == NULL)
	   PP_error_set_user(NULL, "file is not open");

	else
	   {if (indobj != NULL)
	       {nd = PP_obj_to_ind(indobj, ind);
		if (nd < 0)
		   return(NULL);};

	    rv = _PP_read_data(self->fileinfo, name, intype, nd, ind,
			       &self->form);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_PDBfile_defstr(PY_PDBfile *self,
				   PyObject *args, PyObject *kwds)
   {char *name;
    PDBfile *fp;
    PY_defstr *dp;
    PyObject *members, *rv;
    char *kw_list[] = {"name", "members", NULL};

    rv = NULL;

    fp = self->pyo;
    if (fp == NULL)
       PP_error_set_user(NULL, "file is not open");

    else
       {members = NULL;
	if (PyArg_ParseTupleAndKeywords(args, kwds,
					"s|O:defstr", kw_list,
					&name, &members))
	   {if (members == NULL)
	       dp = _PY_defstr_find_singleton(name, NULL, self->fileinfo);
	    else
	       dp = _PY_defstr_make_singleton(NULL, name, members,
					      self->fileinfo);
	    if (dp != NULL)
	       {rv = (PyObject *) dp;
		Py_INCREF(dp);};};};
    
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_PDBfile_ls(PY_PDBfile *self,
			       PyObject *args, PyObject *kwds)
   {int num;
    char *path, *type;
    char **out;
    PyObject *rv;
    PDBfile *fp;
    char *kw_list[] = {"path", "type", NULL};

    rv = NULL;

    fp   = self->pyo;
    path = NULL;
    type = NULL;
    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "|zz:ls", kw_list,
				    &path, &type))
       {out = PD_ls(fp, path, type, &num);
	rv  = PY_strings_tuple(out, num, TRUE);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_PDBfile_get_obj_descr(PY_PDBfile *self,
					  PyObject *args, PyObject *kwds)
   {int tc, bpi;
    void *obj, *dim, *rv;
    PP_descr *descr;
    char *kw_list[] = {"obj", NULL};

    rv = NULL;

    if (self->pyo == NULL)
       {PP_error_set_user(NULL, "file is not open");
        return(NULL);};

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O:get_obj_descr", kw_list,
				    &obj))
       {descr = PP_get_object_descr(self->fileinfo, obj);
	if (descr == NULL)
	   PP_error_set_user(obj, "Unable to find PDB type");
	else
	   {if (descr->dims == NULL)
	       {dim = Py_None;
		Py_INCREF(Py_None);}
	    else
	       dim = PP_dimdes_to_obj(descr->dims);

	    tc  = descr->typecode;
	    bpi = descr->bpi;

	    rv = PY_build_object("get_obj_descr",
				 SC_INT_I, 0, &tc,
				 SC_INT_I, 0, &bpi,
				 SC_STRING_I, 0, &descr->type,
				 SC_POINTER_I, 0, &dim,
				 0);

	    _PP_rl_descr(descr);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* This function is used to register a type.
 * It allows pypact to take an object with type and
 * extract it's information.
 *   cls  = Python class object
 *   type = defstr type
 */

static PyObject *PY_PDBfile_register_class(PY_PDBfile *self,
					   PyObject *args, PyObject *kwds)
   {char *type;
    PP_class_descr *cdescr;
    PDBfile *fp;
    defstr *dp;
    PyTypeObject *cls;
    PyFunctionObject *ctor;
    PyObject *rv;
    char *kw_list[] = {"cls", "type", "ctor", NULL};

    rv = NULL;

    ctor = NULL;
    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!s|O!:register_class", kw_list,
				    &PY_TYPE_TYPE, &cls,
				    &type,
				    &PyFunction_Type, &ctor))
       {fp = self->fileinfo->file;
	dp = PD_inquire_table_type(fp->host_chart, type);
	if (dp == NULL)
	   PP_error_set_user(NULL, "No such type %s in file %s",
			     type, fp->name);
	else
	   {cdescr = PP_make_class_descr(cls, type, ctor);
	    if (cdescr != NULL)
	       {PP_init_type_map_instance(self->fileinfo, cdescr);

		rv = Py_None;

		Py_INCREF(rv);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Clear out type info and reset with just native types
 * sort of like closing and reopening
 * Useful for testing with leak detection
 */

static PyObject *PY_PDBfile_reset_vif(PY_PDBfile *self,
				      PyObject *args, PyObject *kwds)
   {PP_file *fileinfo;
    PyObject *rv;

    rv = NULL;

    if (self->pyo->virtual_internal == FALSE)
       PP_error_set_user(NULL, "file is not virtual internal");

    else

/* clear old */
       {fileinfo = self->fileinfo;

	SC_free_hasharr(fileinfo->type_map, PP_rl_type_entry, NULL);
	SC_free_hasharr(fileinfo->object_map, PP_rl_type_entry, NULL);
	SC_free_hasharr(fileinfo->class_map, _PP_rl_class_descr, NULL);
	SC_free_hasharr(fileinfo->deftypes,  _PP_decref_object, NULL);

	_PP_cleanup_defstrs(fileinfo->file);

/* reset */
	PP_init_type_map_basic(fileinfo);

	rv = Py_None;

	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *PY_PDBfile_get_mode_alt(PY_PDBfile *self, void *context)
   {PyObject *rv;

    switch (PD_get_mode(self->pyo))
       {case PD_READ:
	     rv = PY_STRING_STRING("PD_READ");
	     break;
        case PD_WRITE:
	     rv = PY_STRING_STRING("PD_WRITE");
	     break;
        case PD_APPEND:
	     rv = PY_STRING_STRING("a");
	     break;
        case PD_OPEN:
             rv = PY_STRING_STRING("r");
	     break;
        case PD_CREATE:
	     rv = PY_STRING_STRING("w");
	     break;
        case PD_CLOSE:
	     rv = PY_STRING_STRING("PD_CLOSE");
	     break;
        case PD_TRACE:
	     rv = PY_STRING_STRING("PD_TRACE");
	     break;
        case PD_PRINT:
	     rv = PY_STRING_STRING("PD_PRINT");
	     break;
        default:
	     rv = PY_INT_LONG(PD_get_mode(self->pyo));
	     break;};
        
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMethodDef
 PY_PDBfile_methods[] = {
 _PYD_PD_flush,
{"close", (PyCFunction)PY_PDBfile_close, METH_NOARGS, PY_PDBfile_close_doc},
{"write", (PyCFunction)PY_PDBfile_write, PY_ARG_KEY, PY_PDBfile_write_doc},
{"write_raw", (PyCFunction)PY_PDBfile_write_raw, PY_ARG_KEY, PY_PDBfile_write_raw_doc},
{"read", (PyCFunction)PY_PDBfile_read, PY_ARG_KEY, PY_PDBfile_read_doc},
 _PYD_PD_defent,
{"defstr", (PyCFunction)PY_PDBfile_defstr, PY_ARG_KEY, PY_PDBfile_defstr_doc},
 _PYD_PD_cd,
 _PYD_PD_mkdir,
 _PYD_PD_isdir,
 _PYD_PD_ln,
{"ls", (PyCFunction)PY_PDBfile_ls, PY_ARG_KEY, PY_PDBfile_ls_doc},
 _PYD_PD_pwd,
{"get_obj_descr", (PyCFunction)PY_PDBfile_get_obj_descr, PY_ARG_KEY, PY_PDBfile_get_obj_descr_doc},
{"register_class", (PyCFunction)PY_PDBfile_register_class, PY_ARG_KEY, PY_PDBfile_register_class_doc},
{"reset_vif", (PyCFunction)PY_PDBfile_reset_vif, METH_NOARGS, PY_PDBfile_reset_vif_doc},
{NULL,   (PyCFunction)NULL, 0, NULL}};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_PDBfile_tp_init(PY_PDBfile *self,
			      PyObject *args, PyObject *kwds)
   {int rv;
    char *name, *mode;
    PDBfile *fp;
    PP_file *fileinfo;
    char *kw_list[] = {"name", "mode", NULL};

    rv = -1;

    mode = "r";

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "s|s:open", kw_list,
				    &name, &mode))
       {fp = PD_open(name, mode);
	if (fp == NULL)
	   PyErr_SetString(PP_error_user, PD_get_error());

/* Do not track pointers since we do lots of releasing memory in
 * places like _PP_rel_syment.
 */
	else
	   {PD_set_track_pointers(fp, FALSE);

	    fileinfo = CMAKE(PP_file);
	    fileinfo->file = fp;
	    PP_init_type_map_basic(fileinfo);
    
	    self = PY_PDBfile_newobj(self, fileinfo);
	    if (self != NULL)

/* store singleton instance */
	      {PyDict_SetItemString(PP_open_file_dict, name, (PyObject *) self);
	       rv = 0;};};};
    
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#undef PY_DEF_TP_METH

#define PY_DEF_TP_METH              PY_PDBfile_methods

char
 PY_PDBfile_doc[] = "";

PY_DEF_TYPE(PDBfile);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
