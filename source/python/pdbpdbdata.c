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

/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.C_definition) */

static PyObject *indexerr = NULL;

/* DO-NOT-DELETE splicer.end(pdb.pdbdata.C_definition) */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.extra_members) */

static PyGetSetDef PP_pdbdata_getset[] = {

/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static void
PP_pdbdata_tp_dealloc(PP_pdbdataObject *self)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_type.dealloc) */
/* XXX    PD_free(self->file, self->type, self->data) ;*/

    /* Check if the data actually belongs to another object.
     * For example, by indexing.  In this case self->data
     * may be pointer into the middle of an array so
     * we can not delete it directly.  Instead decrement the parent.
     */
    if (self->parent == NULL) {
        if (self->data != NULL) {
            (void) _PP_rel_syment(self->dpobj->host_chart, self->data, self->nitems, self->type);
            CFREE(self->data);
        }
    } else {
        Py_DECREF(self->parent);
        self->parent = NULL;
        self->data = NULL;
    }
    CFREE(self->type);
    _PD_rl_dimensions(self->dims);
    _PD_rl_defstr(self->dp);
    Py_XDECREF(self->dpobj);
    PY_TYPE(self)->tp_free((PyObject*)self);
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_type.dealloc) */
}

static int
PP_pdbdata_tp_print(PP_pdbdataObject *self, FILE *file, int flags)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_type.print) */
    syment *ep;
    SC_address addr;

    addr.memaddr = self->data;
    ep = _PD_mk_syment(self->type, self->nitems,
                       addr.diskaddr, NULL, self->dims);
    
    PD_write_entry(file, self->fileinfo->file, "data", self->data,
                   ep, 0, NULL);
    _PD_rl_syment(ep);
    return 0;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_type.print) */
}

static PyObject *
PP_pdbdata_tp_str(PP_pdbdataObject *self)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_type.str) */
    intb bpi;
    char *str;
    Py_ssize_t size;

#if 1
    if (_PD_indirection(self->type)) {
        str = DEREF(self->data);
        size = SC_arrlen(str);
    } else {
        str = self->data;
        bpi = _PD_lookup_size(self->type, self->fileinfo->file->host_chart);
        size = bpi * self->nitems;
    }
#else
    str = self->data;
    bpi = _PD_lookup_size(self->type, self->file->host_chart);
    size = bpi * self->nitems;
#endif
    
    return PY_STRING_STRING_SIZE(str, size);
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_type.str) */
}

static int
PP_pdbdata_tp_init(PP_pdbdataObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_type.init) */
    char *type, *tbase;
    PyObject *obj;
    PDBfile *fp;
    PP_file *fileinfo;
    char *kw_list[] = {"obj", "type", "file", NULL};
    int ierr;
    long number;
    defstr *dp;
    void *vr;
    PY_defstr *dpobj;
    PP_PDBfileObject *file;
    PP_descr *descr;
    PP_descr *olddescr;

    type = NULL;
    file = PP_vif_obj;
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O|sO&", kw_list,
                                    &obj, &type, PP_convert_pdbfile, &file))
            return -1;

    vr       = NULL;
    fileinfo = file->fileinfo;
    fp       = fileinfo->file;

    descr = PP_get_object_descr(fileinfo, obj);
    if (descr == NULL) {
        PP_error_set_user(obj, "Unable to find PDB type");
        return -1;
    }

    olddescr = descr;
    descr = PP_outtype_descr(fp, descr, type);
    _PP_rl_descr(olddescr);
    if (descr == NULL) {
        return -1;
    }

    number = _PD_comp_num(descr->dims);

    if (descr->data == NULL) {
        ierr = PP_make_data(obj, fileinfo, descr->type, descr->dims, &vr);
        if (ierr == -1) goto err;
    } else {
        vr = descr->data;
    }

    /* get base type, without dimension information */
    tbase = CSTRSAVE(descr->type);
    strtok(tbase, " *()[");
    dpobj = _PY_defstr_find_singleton(tbase, NULL, fileinfo);
    CFREE(tbase);
    if (dpobj == NULL)
        goto err;
/*        return -1;*/
    dp = dpobj->pyo;
    
    self = PP_pdbdata_newobj(self, vr, descr->type, number,
                             descr->dims, dp, fileinfo, dpobj, NULL);
    
/*    self->dict = PY_defstr_dict(ts, fp);*/

#if 0
    dimdes *dims;
    dims   = NULL;
    retobj = PP_make_object(fp, vr, number, ts, dims);
    if (retobj == NULL) return -1;
#endif
/*    CFREE(ts);  XXX saved in self */
    _PP_rl_descr(descr);
    
    return 0;

 err:
    if (descr != NULL) {
        if (descr->data == NULL) {
            (void) _PP_rel_syment(fp->host_chart, vr, number, descr->type);
            CFREE(vr);
        }
        _PP_rl_descr(descr);
    }    
/*    CFREE(ts);*/
/*    _PD_rl_dimensions(dims); */
    return -1;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_pdbdata_Check - */

/* static */ int
PP_pdbdata_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_pdbdata_Type))
        return 1;
    else
        return 0;
/*  return PY_TYPE(op) == &PP_pdbdata_Type; */
}

/*--------------------------------------------------------------------------*/
/*                          OBJECT_TP_AS_SEQUENCE                           */
/*--------------------------------------------------------------------------*/

/* Code to handle accessing pdbdata objects as sequence objects */

static Py_ssize_t
PP_pdbdata_sq_length(PyObject *_self)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_sequence.length) */
    Py_ssize_t nitems;
    PP_pdbdataObject *self = (PP_pdbdataObject *) _self;

    if (self->data == NULL) {
        nitems = 0;
    } else if (self->dims == NULL) {
        nitems = 1;
    } else {
        nitems = self->dims->number;
    }
    return nitems;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_sequence.length) */
}

#if 0
static PyObject *
PP_pdbdata_sq_concat(PyObject *_self, PyObject *obj)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_sequence.concat) UNMODIFIED */
UNDEFINED
    PP_pdbdataObject self = (PP_pdbdataObject *) _self;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_sequence.concat) */
}
#endif

#if 0
static PyObject *
PP_pdbdata_sq_repeat(PyObject *_self, Py_ssize_t i)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_sequence.repeat) UNMODIFIED */
UNDEFINED
    PP_pdbdataObject self = (PP_pdbdataObject *) _self;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_sequence.repeat) */
}
#endif

static PyObject *
PP_pdbdata_sq_item(PyObject *_self, Py_ssize_t i)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_sequence.item) */
    Py_ssize_t nitems;
    PyObject *rv;
    PP_form *form;
    PP_pdbdataObject *self = (PP_pdbdataObject *) _self;
    
    if (self->data == NULL) {
        nitems = 0;
    } else if (self->dims == NULL) {
        nitems = 1;
    } else {
        nitems = self->dims->number;
    }

    if (i < 0 || i >= nitems) {
        if (indexerr == NULL)
            indexerr = PY_STRING_STRING(
                "pdbdata index out of range");
        PyErr_SetObject(PyExc_IndexError, indexerr);
        return NULL;
    }

    form = &PP_global_form;

    /* special case indexing a scalar item */
    if (nitems == 1 && form->scalar_kind == AS_PDBDATA) {
        Py_INCREF(self);
        rv = (PyObject *) self;
    } else {
        void *vr;

        vr = ((char *) self->data) + i * self->dp->size;
        rv = PP_form_object(vr, self->type, 1, NULL,
                            self->dp, self->fileinfo, self->dpobj,
                            (PyObject *) self, form);
    }
        
    return rv;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_sequence.item) */
}

static PyObject *
PP_pdbdata_sq_slice(PyObject *_self, Py_ssize_t ilow, Py_ssize_t ihigh)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_sequence.slice) */
    Py_ssize_t nitems;
    void *vr;
    PyObject *rv;
    PP_form *form;
    dimdes *dims;
    PP_pdbdataObject *self = (PP_pdbdataObject *) _self;
    
    if (self->data == NULL) {
        nitems = 0;
    } else if (self->dims == NULL) {
        nitems = 1;
    } else {
        nitems = self->dims->number;
    }


    /* lifted from listobject.c */
    if (ilow < 0)
        ilow = 0;
    else if (ilow > nitems)
        ilow = nitems;
    if (ihigh < ilow)
        ihigh = ilow;
    else if (ihigh > nitems)
        ihigh = nitems;

#if 0
    if (ilow < 0 || ilow >= nitems) {
        if (indexerr == NULL)
            indexerr = PY_STRING_STRING(
                "pdbdata index out of range");
        PyErr_SetObject(PyExc_IndexError, indexerr);
        return NULL;
    }

    if (ihigh < 0 || ihigh >= nitems) {
        if (indexerr == NULL)
            indexerr = PY_STRING_STRING(
                "pdbdata index out of range");
        PyErr_SetObject(PyExc_IndexError, indexerr);
        return NULL;
    }
#endif
    
    form = &PP_global_form;

    nitems = ihigh - ilow;
    vr = ((char *) self->data) + ilow * self->dp->size;
    dims = _PD_mk_dimensions(ilow, nitems);
    SC_mark(dims, 1);
    rv = PP_form_object(vr, self->type, nitems, dims,
                        self->dp, self->fileinfo, self->dpobj,
                        (PyObject *) self, form);
    _PD_rl_dimensions(dims);
        
    return rv;

/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_sequence.slice) */
}

static int
PP_pdbdata_sq_ass_item(PyObject *_self, Py_ssize_t i, PyObject *v)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_sequence.ass_item) */
    int ierr;
    Py_ssize_t nitems;
    void *vr;
    PP_pdbdataObject *self = (PP_pdbdataObject *) _self;
    
    if (self->data == NULL) {
        nitems = 0;
    } else if (self->dims == NULL) {
        nitems = 1;
    } else {
        nitems = self->dims->number;
    }

    if (i < 0 || i >= nitems) {
        if (indexerr == NULL)
            indexerr = PY_STRING_STRING(
                "pdbdata index out of range");
        PyErr_SetObject(PyExc_IndexError, indexerr);
        return -1;
    }

    vr = ((char *) self->data) + i * self->dp->size;
    ierr = _PP_rd_syment(v, self->fileinfo, self->type, NULL, 1, vr);

    return ierr;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_sequence.ass_item) */
}

#if 0
static int
PP_pdbdata_sq_ass_slice(PyObject *_self, Py_ssize_t ilow, Py_ssize_t ihigh, PyObject *v)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_sequence.ass_slice) UNMODIFIED */
UNDEFINED
    PP_pdbdataObject self = (PP_pdbdataObject *) _self;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_sequence.ass_slice) */
}
#endif

#if 0
static int
PP_pdbdata_sq_contains(PyObject *_self, PyObject *key)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_sequence.contains) UNMODIFIED */
UNDEFINED
    PP_pdbdataObject self = (PP_pdbdataObject *) _self;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_sequence.contains) */
}
#endif

#if 0
static PyObject *
PP_pdbdata_sq_inplace_concat(PyObject *_self, PyObject *obj)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_sequence.inplace_concat) UNMODIFIED */
UNDEFINED
    PP_pdbdataObject self = (PP_pdbdataObject *) _self;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_sequence.inplace_concat) */
}
#endif

#if 0
static PyObject *
PP_pdbdata_sq_inplace_repeat(PyObject *_self, Py_ssize_t i)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_sequence.inplace_repeat) UNMODIFIED */
UNDEFINED
    PP_pdbdataObject self = (PP_pdbdataObject *) _self;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_sequence.inplace_repeat) */
}
#endif


static PySequenceMethods PP_pdbdata_as_sequence = {
        PP_pdbdata_sq_length,           /* sq_length */
        0,                              /* sq_concat */
        0,                              /* sq_repeat */
        PP_pdbdata_sq_item,             /* sq_item */
        PP_pdbdata_sq_slice,            /* sq_slice */
        PP_pdbdata_sq_ass_item,         /* sq_ass_item */
        0,                              /* sq_ass_slice */
        0,                              /* sq_contains */
        /* Added in release 2.0 */
        0,                              /* sq_inplace_concat */
        0,                              /* sq_inplace_repeat */
};

/*--------------------------------------------------------------------------*/
/*                           OBJECT_TP_AS_BUFFER                            */
/*--------------------------------------------------------------------------*/

#if PY_MAJOR_VERSION < 3

/* Code to access pdbdata objects as buffer */

static Py_ssize_t
PP_pdbdata_bf_getreadbuffer(PyObject *_self, Py_ssize_t segment, void **ptrptr)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_buffer.getreadbuffer) */
    Py_ssize_t n;
    PP_pdbdataObject *self = (PP_pdbdataObject *) _self;
    
    if ( segment != 0 ) {
        PP_error_set(PP_error_internal, NULL,
                     "Accessing non-existent buffer segment:%d", segment);
        n = -1;
    } else {
        *ptrptr = self->data;
        n = self->nitems * self->dp->size;
    }
    return n;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_buffer.getreadbuffer) */
}

static Py_ssize_t
PP_pdbdata_bf_getwritebuffer(PyObject *_self, Py_ssize_t segment, void **ptrptr)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_buffer.getwritebuffer) */
    Py_ssize_t n;
    PP_pdbdataObject *self = (PP_pdbdataObject *) _self;

    if ( segment != 0 ) {
        PP_error_set(PP_error_internal, NULL,
                     "Accessing non-existent buffer segment:%d", segment);
        n = -1;
    } else {
        *ptrptr = self->data;
        n = self->nitems * self->dp->size;
    }
    return n;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_buffer.getwritebuffer) */
}

static Py_ssize_t
PP_pdbdata_bf_getsegcount(PyObject *_self, Py_ssize_t *lenp)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_buffer.getsegcount) */
    if (lenp != NULL)
        *lenp = 1;
    return 1;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_buffer.getsegcount) */
}

static Py_ssize_t
PP_pdbdata_bf_getcharbuffer(PyObject *_self, Py_ssize_t segment, char **ptrptr)
{
/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.as_buffer.getcharbuffer) */
    Py_ssize_t n;
    PP_pdbdataObject *self = (PP_pdbdataObject *) _self;

    if ( segment != 0 ) {
        PP_error_set(PP_error_internal, NULL,
                     "Accessing non-existent buffer segment:%d", segment);
        n = -1;
    } else {
        *ptrptr = self->data;
        n = self->nitems * self->dp->size;
    }
    return n;
/* DO-NOT-DELETE splicer.end(pdb.pdbdata.as_buffer.getcharbuffer) */
}

#endif

static PyBufferProcs
 PP_pdbdata_as_buffer = PY_INIT_BUFFER_PROCS(PP_pdbdata_bf_getreadbuffer,
                                             PP_pdbdata_bf_getwritebuffer,
                                             PP_pdbdata_bf_getsegcount,
                                             PP_pdbdata_bf_getcharbuffer,
                                             NULL,
                                             NULL);

/*--------------------------------------------------------------------------*/


static char PP_pdbdata_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_pdbdata_Type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        "pdbdata",                       /* tp_name */
        sizeof(PP_pdbdataObject),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)PP_pdbdata_tp_dealloc, /* tp_dealloc */
        (printfunc)PP_pdbdata_tp_print, /* tp_print */
        (getattrfunc)0,                 /* tp_getattr */
        (setattrfunc)0,                 /* tp_setattr */
        (cmpfunc)0,                     /* tp_compare */
        (reprfunc)0,                    /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        &PP_pdbdata_as_sequence,        /* tp_as_sequence */
        0,                              /* tp_as_mapping */
        /* More standard operations (here for binary compatibility) */
        (hashfunc)0,                    /* tp_hash */
        (ternaryfunc)0,                 /* tp_call */
        (reprfunc)PP_pdbdata_tp_str,    /* tp_str */
        (getattrofunc)0,                /* tp_getattro */
        (setattrofunc)0,                /* tp_setattro */
        /* Functions to access object as input/output buffer */
        &PP_pdbdata_as_buffer,          /* tp_as_buffer */
        /* Flags to define presence of optional/expanded features */
        Py_TPFLAGS_DEFAULT,             /* tp_flags */
        PP_pdbdata_Type__doc__,         /* tp_doc */
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
        PP_pdbdata_getset,              /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_pdbdata_tp_init,   /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pdb.pdbdata.extra) */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PP_pdbdataObject *PP_pdbdata_newobj(PP_pdbdataObject *obj,
                                    void *vr, char *type, long nitems,
                                    dimdes *dims, defstr *dp, PP_file *fileinfo,
                                    PY_defstr *dpobj, PyObject *parent)
{
    if (obj == NULL) {
/*        obj = (PP_pdbdataObject *) PyType_GenericAlloc(&PP_pdbdata_Type, 0);*/
        obj = (PP_pdbdataObject *) PyType_GenericAlloc(dpobj->ctor, 0);
        if (obj == NULL) {
            return NULL;
        }
    }

    obj->data     = vr;
    obj->type     = type;
    obj->nitems   = nitems;
    obj->dims     = dims;
    obj->dp       = dp;
    obj->fileinfo = fileinfo;
    obj->dpobj    = dpobj;
    obj->parent   = parent;

    /* Are the data belong to us or someone else? */
    if (parent == NULL) {
        SC_mark(vr, 1);
    } else {
        Py_INCREF(parent);
    }
    SC_mark(type, 1);
    if (dims != NULL)
        SC_mark(dims, 1);
    SC_mark(dp, 1);
/* XXX    SC_mark(file, 1); */
    Py_INCREF(dpobj);

    return obj;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.end(pdb.pdbdata.extra) */
/* End of code for pdbdata objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
