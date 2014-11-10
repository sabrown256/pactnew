/*
 * PDBDATA.C
 *
 * include cpyright.h
 */

#include "pdbmodule.h"

static PyObject
 *indexerr = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyGetSetDef
 PY_pdbdata_getset[] = { {NULL} };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PY_pdbdata *PY_pdbdata_newobj(PY_pdbdata *obj,
                                    void *vr, char *type, long nitems,
                                    dimdes *dims, defstr *dp,
				    PP_file *fileinfo,
                                    PY_defstr *dpobj, PyObject *parent)
   {

    if (obj == NULL)
       obj = (PY_pdbdata *) PyType_GenericAlloc(dpobj->ctor, 0);

    if (obj != NULL)
       {obj->data     = vr;
	obj->type     = type;
	obj->nitems   = nitems;
	obj->dims     = dims;
	obj->dp       = dp;
	obj->fileinfo = fileinfo;
	obj->dpobj    = dpobj;
	obj->parent   = parent;

/* are the data belong to us or someone else? */
	if (parent == NULL)
	   {SC_mark(vr, 1);}
	else
	   Py_INCREF(parent);

	SC_mark(type, 1);
	if (dims != NULL)
	   SC_mark(dims, 1);

	SC_mark(dp, 1);

	Py_INCREF(dpobj);};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void PY_pdbdata_tp_dealloc(PY_pdbdata *self)
   {

/* XXX    PD_free(self->file, self->type, self->data) ;*/

/* Check if the data actually belongs to another object.
 * For example, by indexing.  In this case self->data
 * may be pointer into the middle of an array so
 * we can not delete it directly.  Instead decrement the parent.
 */
    if (self->parent == NULL)
       {if (self->data != NULL)
	   {_PP_rel_syment(self->dpobj->host_chart, self->data,
			   self->nitems, self->type);
            CFREE(self->data);};}
    else
       {Py_DECREF(self->parent);
        self->parent = NULL;
        self->data   = NULL;};

    CFREE(self->type);
    _PD_rl_dimensions(self->dims);
    _PD_rl_defstr(self->dp);
    Py_XDECREF(self->dpobj);

    PY_self_free(self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_pdbdata_tp_print(PY_pdbdata *self, FILE *file, int flags)
   {syment *ep;
    SC_address addr;

    addr.memaddr = self->data;
    ep = _PD_mk_syment(self->type, self->nitems,
                       addr.diskaddr, NULL, self->dims);
    
    PD_write_entry(file, self->fileinfo->file, "data", self->data,
                   ep, 0, NULL);

    _PD_rl_syment(ep);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_pdbdata_tp_str(PY_pdbdata *self)
   {intb bpi;
    char *str;
    Py_ssize_t size;
    PyObject *rv;

    if (_PD_indirection(self->type))
       {str = DEREF(self->data);
        size = SC_arrlen(str);}
    else
       {str = self->data;
        bpi = _PD_lookup_size(self->type, self->fileinfo->file->host_chart);
        size = bpi * self->nitems;};
    
#if PY_MAJOR_VERSION >= 3
    PyObject *io;

    io = PyBytes_FromStringAndSize(str, size);
    rv = PyObject_Str(io);

#else
    rv = PY_STRING_STRING_SIZE(str, size);
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_pdbdata_tp_init(PY_pdbdata *self,
			      PyObject *args, PyObject *kwds)
   {int ierr, rv;
    long number;
    char *type, *tbase;
    void *vr;
    defstr *dp;
    PDBfile *fp;
    PyObject *obj;
    PP_file *fileinfo;
    PY_defstr *dpobj;
    PY_PDBfile *file;
    PP_descr *descr, *olddescr;
    char *kw_list[] = {"obj", "type", "file", NULL};

    rv   = -1;
    ierr = 0;

    type = NULL;
    file = PP_vif_obj;
    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O|sO&", kw_list,
				    &obj, &type,
				    PP_convert_pdbfile, &file))
       {vr       = NULL;
	fileinfo = file->fileinfo;
	fp       = fileinfo->file;

	descr = PP_get_object_descr(fileinfo, obj);
	if (descr == NULL)
	   PP_error_set_user(obj, "Unable to find PDB type");

	else
	   {olddescr = descr;
	    descr    = PP_outtype_descr(fp, descr, type);
	    _PP_rl_descr(olddescr);
	    if (descr != NULL)
	       {number = _PD_comp_num(descr->dims);

		if (descr->data == NULL)
		   ierr = PP_make_data(obj, fileinfo, descr->type,
				       descr->dims, &vr);
		else
		   vr = descr->data;

/* get base type, without dimension information */
		if (ierr != -1)
		   {tbase = CSTRSAVE(descr->type);
		    strtok(tbase, " *()[");
		    dpobj = _PY_defstr_find_singleton(tbase, NULL, fileinfo);
		    CFREE(tbase);

		    if (dpobj == NULL)
		       ierr = -1;

		    else
		       {dp = dpobj->pyo;
    
			self = PY_pdbdata_newobj(self, vr, descr->type, number,
						 descr->dims, dp, fileinfo,
						 dpobj, NULL);
    
			_PP_rl_descr(descr);

			rv = 0;};};};
    
	    if ((ierr == -1) && (descr != NULL))
	       {if (descr->data == NULL)
		   {_PP_rel_syment(fp->host_chart, vr, number, descr->type);
		    CFREE(vr);};

		_PP_rl_descr(descr);};};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_pdbdata_Check - */

int PY_pdbdata_Check(PyObject *op)
   {int rv;

    rv = (PyObject_TypeCheck(op, &PY_pdbdata_type) != 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Code to handle accessing pdbdata objects as sequence objects */

static Py_ssize_t PY_pdbdata_sq_length(PyObject *_self)
   {Py_ssize_t nitems;
    PY_pdbdata *self;

    self = (PY_pdbdata *) _self;

    if (self->data == NULL)
       nitems = 0;
    else if (self->dims == NULL)
       nitems = 1;
    else
       nitems = self->dims->number;

    return(nitems);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_pdbdata_sq_item(PyObject *_self, Py_ssize_t i)
   {Py_ssize_t nitems;
    PyObject *rv;
    PP_form *form;
    PY_pdbdata *self;
    
    rv = NULL;

    self = (PY_pdbdata *) _self;
    
    if (self->data == NULL)
       nitems = 0;
    else if (self->dims == NULL)
       nitems = 1;
    else
       nitems = self->dims->number;

    if ((i < 0) || (i >= nitems))
       {if (indexerr == NULL)
	   indexerr = PY_STRING_STRING("pdbdata index out of range");
        PyErr_SetObject(PyExc_IndexError, indexerr);}

    else
       {form = &PP_global_form;

/* special case indexing a scalar item */
	if (nitems == 1 && form->scalar_kind == AS_PDBDATA)
	   {Py_INCREF(self);
	    rv = (PyObject *) self;}
	else
	   {void *vr;

	    vr = ((char *) self->data) + i * self->dp->size;
	    rv = PP_form_object(vr, self->type, 1, NULL,
				self->dp, self->fileinfo, self->dpobj,
				(PyObject *) self, form);};};
        
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_pdbdata_sq_slice(PyObject *_self,
				     Py_ssize_t ilow, Py_ssize_t ihigh)
   {Py_ssize_t nitems;
    void *vr;
    PyObject *rv;
    PP_form *form;
    dimdes *dims;
    PY_pdbdata *self = (PY_pdbdata *) _self;
    
    if (self->data == NULL)
       nitems = 0;
    else if (self->dims == NULL)
       nitems = 1;
    else
       nitems = self->dims->number;

/* lifted from listobject.c */
    if (ilow < 0)
       ilow = 0;
    else if (ilow > nitems)
       ilow = nitems;

    if (ihigh < ilow)
       ihigh = ilow;
    else if (ihigh > nitems)
       ihigh = nitems;

    form = &PP_global_form;

    nitems = ihigh - ilow;
    vr     = ((char *) self->data) + ilow * self->dp->size;
    dims   = _PD_mk_dimensions(ilow, nitems);
    SC_mark(dims, 1);

    rv = PP_form_object(vr, self->type, nitems, dims,
                        self->dp, self->fileinfo, self->dpobj,
                        (PyObject *) self, form);

    _PD_rl_dimensions(dims);
        
    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_pdbdata_sq_ass_item(PyObject *_self,
				  Py_ssize_t i, PyObject *v)
   {int ierr;
    Py_ssize_t nitems;
    void *vr;
    PY_pdbdata *self;
    
    self = (PY_pdbdata *) _self;
    
    if (self->data == NULL)
       nitems = 0;
    else if (self->dims == NULL)
       nitems = 1;
    else
       nitems = self->dims->number;

    if ((i < 0) || (i >= nitems))
       {if (indexerr == NULL)
	   indexerr = PY_STRING_STRING("pdbdata index out of range");
        PyErr_SetObject(PyExc_IndexError, indexerr);
        ierr = -1;}
    else
       {vr   = ((char *) self->data) + i * self->dp->size;
	ierr = _PP_rd_syment(v, self->fileinfo, self->type, NULL, 1, vr);};

    return(ierr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PySequenceMethods
 PY_pdbdata_as_sequence = {
        PY_pdbdata_sq_length,           /* sq_length */
        0,                              /* sq_concat */
        0,                              /* sq_repeat */
        PY_pdbdata_sq_item,             /* sq_item */
        PY_pdbdata_sq_slice,            /* sq_slice */
        PY_pdbdata_sq_ass_item,         /* sq_ass_item */
        0,                              /* sq_ass_slice */
        0,                              /* sq_contains */
        /* Added in release 2.0 */
        0,                              /* sq_inplace_concat */
        0,                              /* sq_inplace_repeat */
};

/*--------------------------------------------------------------------------*/

#if PY_MAJOR_VERSION < 3

/*--------------------------------------------------------------------------*/

/* Code to access pdbdata objects as buffer */

static Py_ssize_t PY_pdbdata_bf_getreadbuffer(PyObject *_self,
					      Py_ssize_t segment,
					      void **ptrptr)
   {Py_ssize_t n;
    PY_pdbdata *self;
    
    self = (PY_pdbdata *) _self;
    
    if (segment != 0)
       {PP_error_set(PP_error_internal, NULL,
                     "Accessing non-existent buffer segment:%d", segment);
        n = -1;}
    else
       {*ptrptr = self->data;
        n = self->nitems * self->dp->size;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static Py_ssize_t PY_pdbdata_bf_getwritebuffer(PyObject *_self,
					       Py_ssize_t segment,
					       void **ptrptr)
   {Py_ssize_t n;
    PY_pdbdata *self = (PY_pdbdata *) _self;

    if (segment != 0)
       {PP_error_set(PP_error_internal, NULL,
                     "Accessing non-existent buffer segment:%d", segment);
        n = -1;}
    else
       {*ptrptr = self->data;
        n = self->nitems * self->dp->size;};

    return(n);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static Py_ssize_t PY_pdbdata_bf_getsegcount(PyObject *_self,
					    Py_ssize_t *lenp)
   {

    if (lenp != NULL)
       *lenp = 1;

    return(1);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static Py_ssize_t PY_pdbdata_bf_getcharbuffer(PyObject *_self,
					      Py_ssize_t segment,
					      char **ptrptr)
   {Py_ssize_t n;
    PY_pdbdata *self;

    self = (PY_pdbdata *) _self;

    if (segment != 0)
       {PP_error_set(PP_error_internal, NULL,
                     "Accessing non-existent buffer segment:%d", segment);
        n = -1;}
    else
       {*ptrptr = self->data;
        n = self->nitems * self->dp->size;};

    return(n);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

static PyBufferProcs
 PY_pdbdata_as_buffer = PY_INIT_BUFFER_PROCS(PY_pdbdata_bf_getreadbuffer,
                                             PY_pdbdata_bf_getwritebuffer,
                                             PY_pdbdata_bf_getsegcount,
                                             PY_pdbdata_bf_getcharbuffer,
                                             NULL,
                                             NULL);

/*--------------------------------------------------------------------------*/

#if 0

#undef PY_DEF_DESTRUCTOR
#undef PY_DEF_REPR
#undef PY_DEF_TP_METH
#undef PY_DEF_TP_PRINT
#undef PY_DEF_TP_STR
#undef PY_DEF_TP_CALL
#undef PY_DEF_AS_SEQ
#undef PY_DEF_AS_MAP
#undef PY_DEF_AS_BUFFER

#define PY_DEF_DESTRUCTOR	    PY_pdbdata_tp_dealloc
#define PY_DEF_REPR                 NULL
#define PY_DEF_TP_METH              NULL
#define PY_DEF_TP_PRINT             PY_pdbdata_tp_print
#define PY_DEF_TP_STR               PY_pdbdata_tp_str
#define PY_DEF_TP_CALL              NULL
#define PY_DEF_AS_SEQ               &PY_pdbdata_as_sequence
#define PY_DEF_AS_MAP               NULL
#define PY_DEF_AS_BUFFER            &PY_pdbdata_as_buffer

char PY_pdbdata_doc[] = "";

PY_DEF_TYPE(pdbdata);

#else

static char PY_pdbdata_type__doc__[] = "";

PyTypeObject
 PY_pdbdata_type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        "pdbdata",                       /* tp_name */
        sizeof(PY_pdbdata),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PY_pdbdata_tp_dealloc, /* tp_dealloc */
        (printfunc)PY_pdbdata_tp_print, /* tp_print */
        (getattrfunc)0,                 /* tp_getattr */
        (setattrfunc)0,                 /* tp_setattr */
        (cmpfunc)0,                     /* tp_compare */
        (reprfunc)0,                    /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        &PY_pdbdata_as_sequence,        /* tp_as_sequence */
        0,                              /* tp_as_mapping */
        /* More standard operations (here for binary compatibility) */
        (hashfunc)0,                    /* tp_hash */
        (ternaryfunc)0,                 /* tp_call */
        (reprfunc)PY_pdbdata_tp_str,    /* tp_str */
        (getattrofunc)0,                /* tp_getattro */
        (setattrofunc)0,                /* tp_setattro */
        /* Functions to access object as input/output buffer */
        &PY_pdbdata_as_buffer,          /* tp_as_buffer */
        /* Flags to define presence of optional/expanded features */
        Py_TPFLAGS_DEFAULT,             /* tp_flags */
        PY_pdbdata_type__doc__,         /* tp_doc */
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
        0,                              /* tp_methods */
        0,                              /* tp_members */
        PY_pdbdata_getset,              /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PY_pdbdata_tp_init,   /* tp_init */
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

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
