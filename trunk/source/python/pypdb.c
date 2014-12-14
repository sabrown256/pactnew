/*
 * PYPDB.C
 *
 * include cpyright.h
 */

#include "pdbmodule.h"

#if 0
#define DEBUG1(_a_, _b_)  fprintf(stderr, _a_, _b_)
#else
#define DEBUG1(_a_, _b_)
#endif

/* This table is used to map between the Python Type object
 * for the defstr (with the members as descriptors) and the
 * defstr pointer.
 */

static hasharr
  *_PY_defstr_tab;
  
char
 PP_getdefstr_doc[] = "",
 PP_gettype_doc[] = "",
 PP_getfile_doc[] = "",
 PP_getdata_doc[] = "",
 PP_getmember_doc[] = "",
 PP_unpack_doc[] = "";

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

/* PY_SETUP_PDB - */

int PY_setup_pdb(PyObject *m)
   {int nerr;

    nerr = 0;

    nerr += (PyModule_AddIntConstant(m, "AS_NONE", AS_NONE) < 0);
    nerr += (PyModule_AddIntConstant(m, "AS_PDBDATA", AS_PDBDATA) < 0);
    nerr += (PyModule_AddIntConstant(m, "AS_OBJECT", AS_OBJECT) < 0);
    nerr += (PyModule_AddIntConstant(m, "AS_TUPLE", AS_TUPLE) < 0);
    nerr += (PyModule_AddIntConstant(m, "AS_LIST", AS_LIST) < 0);
    nerr += (PyModule_AddIntConstant(m, "AS_DICT", AS_DICT) < 0);
    nerr += (PyModule_AddIntConstant(m, "AS_ARRAY", AS_ARRAY) < 0);
    
    PP_open_file_dict = PyDict_New();
    nerr += (PyModule_AddObject(m, "files", PP_open_file_dict) < 0);

    PP_vif_info = _PP_open_vif("PP_vif");
    if (PP_vif_info != NULL)
       {int err;

        PP_vif_obj = PY_PDBfile_newobj(NULL, PP_vif_info);
        PP_vif     = PP_vif_info->file;

/* mark every currently existing defstr in the host_chart */
	err = SC_hasharr_foreach(PP_vif->host_chart,
				 _PP_extra_defstr_mark,
				 NULL);
	nerr += (err != FALSE);

/* XXX - test err */
        nerr += (PyModule_AddObject(m, "vif", (PyObject *) PP_vif_obj) < 0);};

    return(nerr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_GETDEFSTR - */

PyObject *PP_getdefstr(PyObject *self, PyObject *args, PyObject *kwds)
   {PY_pdbdata *obj;
    PyObject *rv;
    char *kw_list[] = {"obj", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:getdefstr", kw_list,
				    &PY_pdbdata_type, &obj))
       {rv = (PyObject *) obj->dpobj;
	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_GETTYPE - */

PyObject *PP_gettype(PyObject *self, PyObject *args, PyObject *kwds)
   {PY_pdbdata *obj;
    PyObject *rv;
    char *kw_list[] = {"obj", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:gettype", kw_list,
				    &PY_pdbdata_type, &obj))
       rv = PY_STRING_STRING(obj->type);

   return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_GETFILE - */

PyObject *PP_getfile(PyObject *self, PyObject *args, PyObject *kwds)
   {PY_pdbdata *obj;
    PY_PDBfile *file;
    PyObject *rv;
    char *kw_list[] = {"obj", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:getfile", kw_list,
				    &PY_pdbdata_type, &obj))
       {file = (PY_PDBfile *) _PP_find_file_obj(obj->fileinfo->file);
	if (file != NULL)
	   {Py_INCREF(file);
	    rv = (PyObject *) file;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_GETDATA - */

PyObject *PP_getdata(PyObject *self, PyObject *args, PyObject *kwds)
   {PY_pdbdata *obj;
    PyObject *rv;
    char *kw_list[] = {"obj", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds, "O!:getdata", kw_list,
				    &PY_pdbdata_type, &obj))
       rv = PY_COBJ_VOID_PTR(obj->data, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_GETMEMBER - */

PyObject *PP_getmember(PyObject *self, PyObject *args, PyObject *kwds)
   {char *name;
    PY_pdbdata *obj;
    PyObject *rv;
    char *kw_list[] = {"obj", "name", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!s:getmember", kw_list,
				    &PY_pdbdata_type, &obj, &name))
       {rv = Py_None;
	Py_INCREF(Py_None);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_UNPACK - */

PyObject *PP_unpack(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok, array, structure, scalar;
    PyObject *rv;
    PY_pdbdata *data;
    PP_form form;
    char *kw_list[] = {"data", "array", "struct", "scalar", NULL};

    rv = NULL;

    array     = AS_NONE;
    structure = AS_NONE;
    scalar    = AS_NONE;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!|iii:unpack", kw_list,
				    &PY_pdbdata_type, &data,
				    &array, &structure, &scalar))
/* default to global values */
       {form = PP_global_form;
	ok   = _PP_assign_form(&form, array, structure, scalar);
	if (ok >= 0)

/* replace AS_PDBDATA with unpack defaults */
	   {if (form.array_kind == AS_PDBDATA)
	       form.array_kind = AS_LIST;
	    if (form.struct_kind == AS_PDBDATA)
	       form.struct_kind = AS_TUPLE;
	    if (form.scalar_kind == AS_PDBDATA)
	       form.scalar_kind = AS_OBJECT;

	    rv = _PP_wr_syment(data->fileinfo, data->type, data->dims,
			       data->nitems, data->data, &form);};};
    return(rv);}

/*--------------------------------------------------------------------------*/

/*                               DEFSTR ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PY_OPT_DEFSTR - handle BLANG binding related operations */

void *_PY_opt_defstr(defstr *x, bind_opt wh, void *a)
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

static int _PP_pack_defstr(void *p, PyObject *v, long nitems, PP_types tc)
   {

    PP_error_set(PP_error_internal, NULL, "_PP_unpack_defstr");

    return(-1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *_PP_unpack_defstr(void *p, long nitems)
   {PyObject *rv;
    
    if (nitems == 1)
       rv = (PyObject *) PY_defstr_newobj(NULL, (defstr *) p, PP_vif_info);

    else
       {PP_error_set(PP_error_internal, NULL, "_PP_unpack_defstr");
        rv = NULL;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PP_descr *_PP_get_defstr_descr(PP_file *fileinfo, PyObject *obj)
   {defstr *dp;
    PP_descr *descr;
    PY_defstr *self;

    self = (PY_defstr *) obj;

    dp = _PD_type_lookup(PP_vif, PD_CHART_HOST, "DEFSTR");

    descr           = CMAKE(PP_descr);
    descr->typecode = PP_UNKNOWN_I;
    descr->bpi      = dp->size;
    descr->type     = dp->type;
    descr->dims     = NULL;
    descr->data     = self->pyo;

    SC_mark(descr->type, 1);
    SC_mark(descr, 1);

    return(descr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PY_defstr_entry(PP_file *fileinfo)
   {PP_descr *descr;
    PP_type_entry *entry;

    descr = PP_make_descr(PP_UNKNOWN_I, "defstr", sizeof(defstr));

    entry = PP_make_type_entry(PP_UNKNOWN_I, FALSE, descr, &PY_defstr_type,
			       _PP_pack_defstr,
			       _PP_unpack_defstr,
			       _PP_get_defstr_descr);
    
    PP_register_type(fileinfo, entry);
    PP_register_object(fileinfo, entry);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_defstr_alt - make a defstr from a list
 *   A wrapper over PD_defstr_alt.
 *
 *   memberdict is a dictionary of descriptor objects
 *   used to lookup members as python attributes.
 */

defstr *PY_defstr_alt(PDBfile *file, char *name, PyObject *members)
   {char *s, **list;
    defstr *dp;
    Py_ssize_t i, nmemb;
    PyObject *item;

    dp = NULL;

    if (!PySequence_Check(members))
       PP_error_set_user(members, "members must be a sequence");

    else
       {nmemb = PySequence_Length(members);
	if (nmemb != -1)
	   {list = CMAKE_N(char *, nmemb);

/* create list of names */
	    for (i = 0; i < nmemb; i++)
	        {item = PySequence_GetItem(members, i);
		 if (!PY_STRING_CHECK(item))
		    {PP_error_set_user(item, "members must be strings");
		     return(NULL);};  /* XXX cleanup */

		 s = PY_STRING_AS_STRING(item);
		 list[i] = s;};

	    dp = PD_defstr_alt(file, name, nmemb, list);
	    if (dp == NULL)
	       PP_error_from_pdb();

/* clean up list */
	    CFREE(list);};};

    return(dp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_defstr_ctor_tp_init(PY_pdbdata *self,
				  PyObject *args, PyObject *kwds)
   {int ierr, nd, rv;
    long number, ind[MAXDIM * 3];
    char *ts;
    void *vr;
    dimdes *dims;
    defstr *dp;
    PyObject *data, *indobj;
    PP_file *fileinfo;
    PY_defstr *dpobj;
    char *kw_list[] = {"data", "ind", NULL};

    rv   = -1;
    ierr = 0;

    indobj = NULL;
    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O|O", kw_list,
                                    &data, &indobj))
       {vr = NULL;

	DEBUG1("%s", "In PY_defstr_ctor_tp_init\n");
    
	dpobj = (PY_defstr *) SC_hasharr_def_lookup(_PY_defstr_tab,
						    PY_TYPE(self));
	if (dpobj == NULL)
	   PP_error_set(PP_error_internal, (PyObject *) self,
			"Unable to locate constructor for %s",
			PY_TYPE(self)->tp_name);

/* a sanity check */
	else if (PY_TYPE(self) != dpobj->ctor)
	   PP_error_set(PP_error_internal, (PyObject *) self,
			"Unable to find constructor");
	else
	   {dp = dpobj->pyo;
	    ts = dp->type;
	    fileinfo = dpobj->fileinfo;

	    dims = NULL;
	    number = 1;
	    if (indobj != NULL)
	       {nd = PP_obj_to_ind(indobj, ind);
		if (nd < 0)
		   ierr = -1;
		else
		   {dims   = PP_ind_to_dimdes(nd, ind);
		    number = _PD_comp_num(dims);};};
    
	    if (ierr != -1)
	       {ierr = PP_alloc_data(ts, number, fileinfo, &vr);
		if (ierr < 0)
		   PP_error_set(PP_error_internal, (PyObject *) self,
				"Error allocating memory");
		else
		   {ierr = _PP_rd_syment(data, fileinfo, ts,
					 dims, number, vr);
		    if (ierr != -1)
		       {self = PY_pdbdata_newobj(self, vr, ts, number,
						 dims, dp,
						 fileinfo, dpobj, NULL);
			rv = 0;};};};};};

    if (ierr == -1)
       {_PP_rel_syment(dpobj->host_chart, vr, number, ts);
	CFREE(vr);
/* XXX  CFREE(ts); */
	_PD_rl_dimensions(dims);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_defstr_mk_ctor -
 *   dpobj is not complete at this point since we're in the
 *   process of making the ctor field.
 *   However, dpobj->pyo is set.
 *   The _PY_defstr_tab allows dpobj to be looked up given
 *   the ctor.  This is necessary for PY_defstr_ctor_tp_init
 *   to find what type of defstr it needs to build, including which
 *   file to use.
 */

PyTypeObject *PY_defstr_mk_ctor(PY_defstr *dpobj)
   {haelem *hp;
    defstr *dp;
    PyTypeObject *ctor;
    PyHeapTypeObject *et;  

    ctor = (PyTypeObject *) PyType_GenericAlloc(&PyType_Type, 0);
    
    if (ctor != NULL)
       {dp = dpobj->pyo;

	ctor->tp_name  = dp->type;
	ctor->tp_base  = (PyTypeObject *) &PY_pdbdata_type;
	Py_INCREF(&PY_pdbdata_type);
	ctor->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HEAPTYPE;
	ctor->tp_new   = PyType_GenericNew;

/* FIXME: why are there two assignments to ctor->tp_init ??? */
	ctor->tp_init  = PY_pdbdata_type.tp_init;
	ctor->tp_init  = (initproc) PY_defstr_ctor_tp_init;
	ctor->tp_alloc = PyType_GenericAlloc;

/* set name in the extended type object */
	et = (PyHeapTypeObject *) ctor;
#if PYTHON_API_VERSION >= 1013
	et->ht_name = PY_STRING_STRING(ctor->tp_name);
#else
	et->name = PY_STRING_STRING(ctor->tp_name);
#endif

	if (PyType_Ready(ctor) >= 0)
	   {if (PY_defstr_dict(dp, ctor->tp_dict) >= 0)
	       {hp = SC_hasharr_install(_PY_defstr_tab, ctor, dpobj,
					"PY_defstr", 3, -1); 
		if (hp != NULL)
		   Py_INCREF(dpobj);};};};

    return(ctor);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_rl_defstr - release a defstr object.
 *   It may not be fully formed if there was an error during creation.
 */

void _PP_rl_defstr(PY_defstr *self)
   {

    _PD_clr_table(self->host_chart, _PD_ha_rl_defstr);
    _PD_rl_defstr(self->pyo);

/* PACT will not release the definition since it is not
 * pact memory, so use python decref
 */
    if (self->ctor != NULL)
       {SC_hasharr_remove(_PY_defstr_tab, self->ctor);
        Py_DECREF(self->ctor);};

    self->pyo        = NULL;
    self->host_chart = NULL;
    self->ctor       = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_defstr_make_singleton -
 *   Return value: Borrowed reference
 */

PY_defstr *_PY_defstr_make_singleton(PY_defstr *self,
				     char *name, PyObject *members,
				     PP_file *fileinfo)
   {defstr *dp;
    PDBfile *fp;
    PY_defstr *rv;

    rv = NULL;

    fp = fileinfo->file;

    dp = PD_inquire_table_type(fp->host_chart, name);
    if (dp == NULL)
       {if (PY_defstr_check(members) == 1)
	   {PY_defstr *dpobj;
	    memdes *lst;

	    dpobj = (PY_defstr *) members;
	    lst   = PD_copy_members(dpobj->pyo->members);

	    dp = _PD_defstr_inst(fp, name, KIND_STRUCT, lst,
				 -1, NULL, NULL, PD_CHART_HOST);
	    if (dp == NULL)
	       PP_error_from_pdb();}

	else
	   dp = PY_defstr_alt(fp, name, members);}
    else
       {PP_error_set_user(NULL, "type already defined - %s", name);
        dp = NULL;};

    if (dp != NULL)

/* If self already exists, increment reference since
 * it will be stored in deftypes. Otherwise the self
 * returned by newobj will only have one reference which
 * will be used to store in deftypes.
 */
       {Py_XINCREF(self);
	self = PY_defstr_newobj(self, dp, fileinfo);

/* Install object into the file's table */
	if (self != NULL)
	   SC_hasharr_install(fileinfo->deftypes, dp->type, self,
			      "PY_defstr", 2, -1);

	rv = self;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_defstr_find_singleton -
 *   Return value: Borrowed reference
 */

PY_defstr *_PY_defstr_find_singleton(char *name, defstr *dp,
				     PP_file *fileinfo)
   {PDBfile *fp;
    PY_defstr *rv;

    rv = (PY_defstr *) SC_hasharr_def_lookup(fileinfo->deftypes, name);
    if (rv == NULL)
       {fp = fileinfo->file;
        if (dp == NULL)
	   {dp = PD_inquire_table_type(fp->host_chart, name);
            if (dp == NULL)
	       PP_error_set_user(NULL, "No such type %s in file %s",
				 name, fp->name);
	    else

/* this will use the new reference to install the item */
	       {rv = PY_defstr_newobj(NULL, dp, fileinfo);
		if (rv != NULL)
		   SC_hasharr_install(fileinfo->deftypes, dp->type, rv,
				      "PY_defstr", 2, -1);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *defstr_getter(PY_pdbdata *self, void *closure)
   {memdes *member;
    PyObject *rv;
    
    member = (memdes *) closure;

    rv = PP_getattr_from_defstr(self->fileinfo, self->data,
                                self->type, member->name,
                                self->nitems, (PyObject *) self);
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int defstr_setter(PY_pdbdata *self, PyObject *value, void *closure)
   {int ierr;
    void *vr;
    memdes *desc;

    desc = (memdes *) closure;

    vr   = ((char *) self->data) + desc->member_offs;
    ierr = _PP_rd_syment(value, self->fileinfo, desc->type,
                         desc->dimensions, desc->number, vr);
    
    return(ierr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_defstr_dict - populate dictionary with desriptor objects
 *                - to access member of a defstr
 */

int PY_defstr_dict(defstr *dp, PyObject *dict)
   {int rv;
    memdes *pm;
    PyGetSetDef *getset;
    PyObject *descr;

    rv = 0;

    for (pm = dp->members; (pm != NULL) && (rv == 0); pm = pm->next)
        {getset          = PyMem_New(PyGetSetDef, 1);
	 getset->name    = pm->name;
	 getset->get     = (getter) defstr_getter;
	 getset->set     = (setter) defstr_setter;
	 getset->doc     = NULL;
	 getset->closure = pm;

	 descr = PyDescr_NewGetSet(&PY_pdbdata_type, getset);
	 if ((descr == NULL) ||
	     (PyDict_SetItemString(dict, getset->name, descr) < 0))
            rv = -1;
	 else
	    Py_DECREF(descr);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_getattr_from_defstr - return an attribute for a pdbdata object.
 *     loop over all members of the structure until a name is
 *     found, then make an object out of it.
 *     If the name is not found, return NULL.
 */

PyObject *PP_getattr_from_defstr(PP_file *fileinfo, void *vr, char *type,
                                 char *name, long nitems,
                                 PyObject *parent)
   {char *ttype, *ts, *svr;
    defstr *dp;
    memdes *desc;
    PP_form form;
    PY_defstr *dpobj;
    PyObject *obj;

    obj  = NULL;
    form = PP_global_form;

    if (fileinfo == NULL)
       {PP_error_set_user(NULL, "fileinfo is NULL");
        return(NULL);};

/* get base type */
    ts = CSTRSAVE(type);
    strtok(ts, " *()[");
    dpobj = _PY_defstr_find_singleton(ts, NULL, fileinfo);
    CFREE(ts);

    if (dpobj != NULL)
       {dp  = dpobj->pyo;
	svr = vr;

	for (desc = dp->members; desc != NULL; desc = desc->next)
	    {if (strcmp(desc->name, name) == 0)
		{PP_CAST_TYPE(ttype, desc, svr + desc->member_offs, svr,
			      Py_None, NULL);

/* get member base type */
		 ts = CSTRSAVE(ttype);
		 strtok(ts, " *()[");
		 dpobj = _PY_defstr_find_singleton(ts, NULL, fileinfo);
		 CFREE(ts);

		 if (dpobj != NULL)
		    {dp = dpobj->pyo;

		     svr += desc->member_offs;
		     obj = PP_form_object(svr, ttype,
					  desc->number, desc->dimensions,
					  dp, fileinfo, dpobj,
					  parent, &form);};
		 break;};};};
    
    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_create_defstr_tab - */

void _PP_create_defstr_tab(void)
   {

    _PY_defstr_tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_ADDR_KEY, 0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_clr_defstr - if defstr is in fp, decrement */
 
static int _PP_clr_defstr(haelem *hp, void *arg)
   {int ok;
    PY_defstr *self;
    PDBfile *fp;
    
    fp = arg;
    
    ok = SC_haelem_data(hp, NULL, NULL, (void **) &self, FALSE);
    if ((ok == TRUE) && (self->fileinfo->file == fp))
       {Py_DECREF(self);};

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_cleanup_defstrs - Go thru all defstrs in the global table,
 * if it is in this file, delete it.
 *  _PY_defstr_tab is file static.
 */
 
void _PP_cleanup_defstrs(PDBfile *fp)
   {int err;
    
    err = SC_hasharr_foreach(_PY_defstr_tab, _PP_clr_defstr, fp);

    SC_ASSERT(err == TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_defstr_rem - remove defstrs from file. */

void PY_defstr_rem(char *name, PDBfile *file)
   {defstr *dp;
    
    dp = PD_inquire_table_type(file->chart, name);
    SC_mark(dp, 1);
    SC_hasharr_remove(file->chart, name);
    _PD_rl_defstr(dp);
   
    dp = PD_inquire_table_type(file->host_chart, name);
    SC_mark(dp, 1);
    SC_hasharr_remove(file->host_chart, name);
    _PD_rl_defstr(dp);
    
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_defstr_has_key_doc[] = "";

static PyObject *PY_defstr_has_key(PY_defstr *self,
				   PyObject *args,
				   PyObject *kwds)
   {char *key;
    char *kw_list[] = {"key", NULL};

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "s:has_key", kw_list,
				    &key))
       PyErr_SetString(PyExc_NotImplementedError, "has_key");

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_defstr_items_doc[] = "";

static PyObject *PY_defstr_items(PY_defstr *self,
				 PyObject *args,
				 PyObject *kwds)
   {

    PyErr_SetString(PyExc_NotImplementedError, "items");

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_defstr_keys_doc[] = "";

static PyObject *PY_defstr_keys(PY_defstr *self,
				PyObject *args,
				PyObject *kwds)
   {int err;
    defstr *dp;
    memdes *desc;
    Py_ssize_t i;
    PyObject *rv;
 
    dp = self->pyo;
    if (dp == NULL)
       {PP_error_set(PP_error_internal,
                     NULL, "Defstr is NULL");
        return(NULL);};

    if (dp->members == 0)
       {Py_INCREF(Py_None);
        return(Py_None);};

    for (i = 0, desc = self->pyo->members;
         desc != NULL;
         i++, desc = desc->next);

    rv = PyTuple_New(i);
    if (rv != NULL)
       {for (i = 0, desc = self->pyo->members;
	     desc != NULL;
	     i++, desc = desc->next)
	    {err = PyTuple_SetItem(rv, i, PY_STRING_STRING(desc->name));
	     if (err < 0)
	        {Py_DECREF(rv);
		 rv = NULL;
		 break;};};};
  
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_defstr_values_doc[] = "";

static PyObject *PY_defstr_values(PY_defstr *self,
				  PyObject *args,
				  PyObject *kwds)
   {

    PyErr_SetString(PyExc_NotImplementedError, "values");

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_defstr_get_doc[] = "";

static PyObject *PY_defstr_get(PY_defstr *self,
			       PyObject *args, PyObject *kwds)
   {

    PyErr_SetString(PyExc_NotImplementedError, "get");

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_defstr_doc_order_flag[] = "";

static PyObject *PY_defstr_get_order_flag(PY_defstr *self,
					  PyObject *args, PyObject *kwds)
   {

    return(PY_INT_LONG(self->pyo->fix.order));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMethodDef
 PY_defstr_methods[] = {
   {"has_key", (PyCFunction)PY_defstr_has_key, PY_ARG_KEY, PY_defstr_has_key_doc},
   {"items", (PyCFunction)PY_defstr_items, METH_NOARGS, PY_defstr_items_doc},
   {"keys", (PyCFunction)PY_defstr_keys, METH_NOARGS, PY_defstr_keys_doc},
   {"values", (PyCFunction)PY_defstr_values, METH_NOARGS, PY_defstr_values_doc},
   {"get", (PyCFunction)PY_defstr_get, METH_NOARGS, PY_defstr_get_doc},
   {"order_flag", (PyCFunction) PY_defstr_get_order_flag, METH_NOARGS, PY_defstr_doc_order_flag},
   {NULL, NULL, 0, NULL}};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PY_defstr *PY_defstr_newobj(PY_defstr *obj, defstr *dp, PP_file *fileinfo)
   {PyTypeObject *ctor;

    if (obj == NULL)
       obj = (PY_defstr *) PyType_GenericAlloc(&PY_defstr_type, 0);

    if (obj != NULL)
       {obj->pyo      = dp;
	obj->fileinfo = fileinfo;

/* save a reference to the host_chart.
 * This is used when the file has been closed but some
 * references to defstrObjects still exist.
 */
	obj->host_chart = fileinfo->file->host_chart;

	ctor = PY_defstr_mk_ctor(obj);
	if (ctor == NULL)
	   obj = NULL;
	else
	   {obj->ctor = ctor;

	     SC_mark(dp, 1);
	     SC_mark(fileinfo->file->host_chart, 1);};};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void PY_defstr_tp_dealloc(PY_defstr *self)
   {

    _PP_rl_defstr(self);

    PY_self_free(self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_defstr_tp_print(PY_defstr *self, FILE *file, int flags)
   {

    PD_write_defstr(file, self->pyo);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_defstr_tp_call(PY_defstr *self,
				   PyObject *args, PyObject *kwds)
   {PyObject *rv;
    
    if (self->ctor == NULL)
       {PyErr_Format(PyExc_TypeError,
		     "'%s' object is not callable",
		     PY_TYPE(self)->tp_name);
        rv = NULL;}

    else
       rv = PyObject_Call((PyObject *) self->ctor, args, kwds);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_defstr_tp_init(PY_defstr *self, PyObject *args, PyObject *kwds)
   {int rv;
    char *name;
    PyObject *members;
    PY_PDBfile *file;
    char *kw_list[] = {"name", "members", "file", NULL};

    rv = -1;

    file = PP_vif_obj;
    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "sO|O&", kw_list,
				    &name, &members,
				    PP_convert_pdbfile, &file))
       {self = _PY_defstr_make_singleton(self, name, members, file->fileinfo);
	if (self != NULL)
	   rv = 0;};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                           OBJECT_TP_AS_MAPPING                           */

/*--------------------------------------------------------------------------*/

/* Code to access defstr objects as mappings */

static Py_ssize_t PY_defstr_mp_length(PyObject *_self)
   {Py_ssize_t nitems;
    defstr *dp;
    memdes *desc;
    PY_defstr *self;
 
    self = (PY_defstr *) _self;
 
    nitems = -1;

    dp = self->pyo;
    if (dp == NULL)
       PP_error_set(PP_error_internal,
		    NULL, "Defstr is NULL");
    else
       {for (nitems = 0, desc = self->pyo->members;
	     desc != NULL;
	     nitems++, desc = desc->next);};

    return(nitems);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_defstr_mp_subscript(PyObject *_self, PyObject *key)
   {char *name;
    defstr *dp;
    memdes *desc;
    PyObject *rv;
    PY_defstr *self;
 
    rv = NULL;

    self = (PY_defstr *) _self;
 
    if (!PY_STRING_CHECK(key))
       PP_error_set_user(key, "key must be string");

    else
       {name = PY_STRING_AS_STRING(key);

	dp = self->pyo;
	if (dp == NULL)
	   PP_error_set(PP_error_internal,
			NULL, "Defstr is NULL");
	else
	   {for (desc = self->pyo->members; desc != NULL; desc = desc->next)
	        {if (strcmp(desc->name, name) == 0)
		    break;};

	    if (desc != NULL)
	       rv = (PyObject *) PY_memdes_newobj(NULL, desc);
	    else
	       {PyErr_SetObject(PyExc_KeyError, key);
		rv = NULL;};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMappingMethods
 PY_defstr_as_mapping = {PY_defstr_mp_length,            /* mp_length */
			 PY_defstr_mp_subscript,         /* mp_subscript */
			 0,};                            /* mp_ass_subscript */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#undef PY_DEF_DESTRUCTOR
#undef PY_DEF_TP_METH
#undef PY_DEF_TP_PRINT
#undef PY_DEF_TP_CALL
#undef PY_DEF_AS_MAP

#define PY_DEF_DESTRUCTOR	    PY_defstr_tp_dealloc
#define PY_DEF_TP_PRINT             PY_defstr_tp_print
#define PY_DEF_TP_CALL              PY_defstr_tp_call
#define PY_DEF_TP_METH              PY_defstr_methods
#define PY_DEF_AS_MAP	            &PY_defstr_as_mapping

char
 PY_defstr_doc[] = "";

PY_DEF_TYPE(defstr);

/*--------------------------------------------------------------------------*/

/*                               MEMDES ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PY_OPT_MEMDES - handle BLANG binding related operations */

void *_PY_opt_memdes(memdes *x, bind_opt wh, void *a)
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

PY_memdes *PY_memdes_newobj(PY_memdes *obj, memdes *desc)
   {

    if (obj == NULL)
       obj = (PY_memdes *) PyType_GenericAlloc(&PY_memdes_type, 0);

    if (obj != NULL)
       {obj->pyo = desc;
	SC_mark(desc, 1);};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void PY_memdes_tp_dealloc(PY_memdes *self)
   {

    _PD_rl_descriptor(self->pyo);

    PY_self_free(self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_memdes_tp_init(PY_memdes *self, PyObject *args, PyObject *kwds)
   {

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#undef PY_DEF_DESTRUCTOR
#undef PY_DEF_TP_METH
#undef PY_DEF_TP_PRINT
#undef PY_DEF_TP_CALL
#undef PY_DEF_AS_MAP

#define PY_DEF_DESTRUCTOR	    PY_memdes_tp_dealloc
#define PY_DEF_TP_METH              NULL
#define PY_DEF_TP_PRINT             NULL
#define PY_DEF_TP_CALL              NULL
#define PY_DEF_AS_MAP               NULL

char
 PY_memdes_doc[] = "";

PY_DEF_TYPE(memdes);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
