/*
 * PYPML.C - support for PML Python bindings
 *
 */

#include "cpyright.h"
#include "py_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_OPT_PM_SET - handle BLANG binding related operations */

void *_PY_opt_PM_set(PM_set *x, bind_opt wh, void *a)
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

/* _PY_OPT_PM_MAPPING - handle BLANG binding related operations */

void *_PY_opt_PM_mapping(PM_mapping *x, bind_opt wh, void *a)
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

/* PY_INIT_PML_INT - interim initializations for PML bindings */

int PY_init_pml_int(PyObject *m, PyObject *d)
   {int nerr;
    extern int PY_init_pml(PyObject *m, PyObject *d);

    nerr = PY_init_pml(m, d);

    PP_set_Type.tp_new   = PyType_GenericNew;
    PP_set_Type.tp_alloc = PyType_GenericAlloc;
    nerr += (PyType_Ready(&PP_set_Type) < 0);
    nerr += (PyDict_SetItemString(d, "set", (PyObject *) &PP_set_Type) < 0);

    PP_mapping_Type.tp_new   = PyType_GenericNew;
    PP_mapping_Type.tp_alloc = PyType_GenericAlloc;
    nerr += (PyType_Ready(&PP_mapping_Type) < 0);

    nerr += (PyDict_SetItemString(d, "mapping", (PyObject *) &PP_mapping_Type) < 0);

    return(nerr);}

/*--------------------------------------------------------------------------*/

/*                              PM_SET_ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* PP_set_Check - */

int PP_set_Check(PyObject *op)
   {int rv;

    rv = PyObject_TypeCheck(op, &PP_set_Type);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *_PY_set_from_ptr(PM_set *data)
   {PP_setObject *self;
    PyObject *rv;

    rv = NULL;

    self = PyObject_NEW(PP_setObject, &PP_set_Type);
    if (self != NULL)
       {self->set = data;
	rv = (PyObject *) self;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = _PY_set_from_ptr(self->set);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_name_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->set->name);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PP_set_set_name_set(PP_setObject *self, PyObject *value,
			       void *context)
   {int rv;

    rv = -1;

    if (value == NULL)
       PyErr_SetString(PyExc_TypeError,
		       "attribute deletion is not supported");

    else if (PyArg_Parse(value, "s", &self->set->name))
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_element_type_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->set->element_type);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_dimension_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = PY_INT_LONG(self->set->dimension);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_max_index_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = PY_COBJ_VOID_PTR((void *) self->set->max_index, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_dimension_elem_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = PY_INT_LONG(self->set->dimension_elem);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_n_elements_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = PY_INT_LONG(self->set->n_elements);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_elements_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = PY_COBJ_VOID_PTR((void *) self->set->elements, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_es_type_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->set->es_type);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_extrema_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->set->extrema);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_scales_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->set->scales);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_opers_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = PPfield_from_ptr(self->set->opers);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_metric_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = PY_COBJ_VOID_PTR((void *) self->set->metric, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_symmetry_type_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->set->symmetry_type);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_symmetry_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = PY_COBJ_VOID_PTR((void *) self->set->symmetry, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_topology_type_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->set->topology_type);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_topology_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = PY_COBJ_VOID_PTR((void *) self->set->topology, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_info_type_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->set->info_type);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_info_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = PY_COBJ_VOID_PTR((void *) self->set->info, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_set_set_next_get(PP_setObject *self, void *context)
   {PyObject *rv;

    rv = _PY_set_from_ptr(self->set->next);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char
 PP_set_set__doc__[] = "",
 PP_set_set_name__doc__[] = "",
 PP_set_set_element_type__doc__[] = "",
 PP_set_set_dimension__doc__[] = "",
 PP_set_set_max_index__doc__[] = "",
 PP_set_set_dimension_elem__doc__[] = "",
 PP_set_set_n_elements__doc__[] = "",
 PP_set_set_elements__doc__[] = "",
 PP_set_set_es_type__doc__[] = "",
 PP_set_set_extrema__doc__[] = "",
 PP_set_set_scales__doc__[] = "",
 PP_set_set_opers__doc__[] = "",
 PP_set_set_metric__doc__[] = "",
 PP_set_set_symmetry_type__doc__[] = "",
 PP_set_set_symmetry__doc__[] = "",
 PP_set_set_topology_type__doc__[] = "",
 PP_set_set_topology__doc__[] = "",
 PP_set_set_info_type__doc__[] = "",
 PP_set_set_info__doc__[] = "",
 PP_set_set_next__doc__[] = "",
 PP_set_Type__doc__[] = "";


static PyGetSetDef PP_set_getset[] = {
    {"set", (getter) PP_set_set_get, NULL, PP_set_set__doc__, NULL},
    {"name", (getter) PP_set_set_name_get, (setter) PP_set_set_name_set, PP_set_set_name__doc__, NULL},
    {"element_type", (getter) PP_set_set_element_type_get, NULL, PP_set_set_element_type__doc__, NULL},
    {"dimension", (getter) PP_set_set_dimension_get, NULL, PP_set_set_dimension__doc__, NULL},
    {"max_index", (getter) PP_set_set_max_index_get, NULL, PP_set_set_max_index__doc__, NULL},
    {"dimension_elem", (getter) PP_set_set_dimension_elem_get, NULL, PP_set_set_dimension_elem__doc__, NULL},
    {"n_elements", (getter) PP_set_set_n_elements_get, NULL, PP_set_set_n_elements__doc__, NULL},
    {"elements", (getter) PP_set_set_elements_get, NULL, PP_set_set_elements__doc__, NULL},
    {"es_type", (getter) PP_set_set_es_type_get, NULL, PP_set_set_es_type__doc__, NULL},
    {"extrema", (getter) PP_set_set_extrema_get, NULL, PP_set_set_extrema__doc__, NULL},
    {"scales", (getter) PP_set_set_scales_get, NULL, PP_set_set_scales__doc__, NULL},
    {"opers", (getter) PP_set_set_opers_get, NULL, PP_set_set_opers__doc__, NULL},
    {"metric", (getter) PP_set_set_metric_get, NULL, PP_set_set_metric__doc__, NULL},
    {"symmetry_type", (getter) PP_set_set_symmetry_type_get, NULL, PP_set_set_symmetry_type__doc__, NULL},
    {"symmetry", (getter) PP_set_set_symmetry_get, NULL, PP_set_set_symmetry__doc__, NULL},
    {"topology_type", (getter) PP_set_set_topology_type_get, NULL, PP_set_set_topology_type__doc__, NULL},
    {"topology", (getter) PP_set_set_topology_get, NULL, PP_set_set_topology__doc__, NULL},
    {"info_type", (getter) PP_set_set_info_type_get, NULL, PP_set_set_info_type__doc__, NULL},
    {"info", (getter) PP_set_set_info_get, NULL, PP_set_set_info__doc__, NULL},
    {"next", (getter) PP_set_set_next_get, NULL, PP_set_set_next__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pdb.set.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pdb.set.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

static int PP_set_tp_init(PP_setObject *self, PyObject *args, PyObject *kwds)
   {int cp, nd, nde, rv, ok;
    int *maxes;
    long ne;
    char *name, *type;
    char *symtype, *toptype, *inftype;
    void *elem, *sym, *top, *inf;
    PM_field *opers;
    double *metric;
    PM_set *next;
    PP_fieldObject *opersobj;
    PP_setObject *nextobj;
    char *kw_list[] = {"name", "type", "cp", "ne", "nd",
		       "nde", "maxes", "elem", "opers",
		       "metric", "symtype", "sym", "toptype",
		       "top", "inftype", "inf", "next", NULL};

    rv = -1;

    ok = PyArg_ParseTupleAndKeywords(args, kwds,
				     "ssiliiO&zO!O&szszszO!:make_set",
				     kw_list,
                                     &name, &type, &cp,
				     &ne, &nd, &nde,
				     iarray_extractor,
				     &maxes, &elem,
				     &PP_field_Type, &opersobj,
				     REAL_array_extractor,
				     &metric, &symtype, &sym,
				     &toptype, &top,
				     &inftype, &inf,
				     &PP_set_Type, &nextobj);
    if (ok)
       {rv    = 0;
	opers = opersobj->opers;
	next  = nextobj->set;
	self->set = PM_mk_set(name, type, cp, ne, nd, nde, maxes, elem,
			      opers, metric, symtype, sym, toptype, top,
			      inftype, inf, next);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyTypeObject PP_set_Type =
    {
        PY_HEAD_INIT(&PyType_Type, 0)
        "set",                          /* tp_name */
        sizeof(PP_setObject),           /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor) 0,                 /* tp_dealloc */
        (printfunc) 0,                  /* tp_print */
        (getattrfunc) 0,                /* tp_getattr */
        (setattrfunc) 0,                /* tp_setattr */
        (cmpfunc) 0,                    /* tp_compare */
        (reprfunc) 0,                   /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        0,                              /* tp_as_sequence */
        0,                              /* tp_as_mapping */
        /* More standard operations (here for binary compatibility) */
        (hashfunc) 0,                   /* tp_hash */
        (ternaryfunc) 0,                /* tp_call */
        (reprfunc) 0,                   /* tp_str */
        (getattrofunc) 0,               /* tp_getattro */
        (setattrofunc) 0,               /* tp_setattro */
        /* Functions to access object as input/output buffer */
        0,                              /* tp_as_buffer */
        /* Flags to define presence of optional/expanded features */
        Py_TPFLAGS_DEFAULT,             /* tp_flags */
        PP_set_Type__doc__,             /* tp_doc */
        /* Assigned meaning in release 2.0 */
        /* call function for all accessible objects */
        (traverseproc) 0,               /* tp_traverse */
        /* delete references to contained objects */
        (inquiry) 0,                    /* tp_clear */
        /* Assigned meaning in release 2.1 */
        /* rich comparisons */
        (richcmpfunc) 0,                /* tp_richcompare */
        /* weak reference enabler */
        0,                              /* tp_weaklistoffset */
        /* Added in release 2.2 */
        /* Iterators */
        (getiterfunc) 0,                /* tp_iter */
        (iternextfunc) 0,               /* tp_iternext */
        /* Attribute descriptor and subclassing stuff */
        0,                              /* tp_methods */
        0,                              /* tp_members */
        PP_set_getset,                  /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc) 0,               /* tp_descr_get */
        (descrsetfunc) 0,               /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_set_tp_init,       /* tp_init */
        (allocfunc) 0,                  /* tp_alloc */
        (newfunc) 0,                    /* tp_new */
#if PYTHON_API_VERSION >= 1012
        (freefunc) 0,                   /* tp_free */
#else
        (destructor) 0,                 /* tp_free */
#endif
        (inquiry) 0,                    /* tp_is_gc */
        0,                              /* tp_bases */
        0,                              /* tp_mro */
        0,                              /* tp_cache */
        0,                              /* tp_subclasses */
        0,                              /* tp_weaklist */
#if PYTHON_API_VERSION >= 1012
        (destructor) 0,                 /* tp_del */
#endif
	};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/*                            PM_MAPPING_ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* PP_mapping_Check - */

int PP_mapping_Check(PyObject *op)
   {int rv;

    rv = PyObject_TypeCheck(op, &PP_mapping_Type);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *_PY_mapping_from_ptr(PM_mapping *data)
   {PP_mappingObject *self;
    PyObject *rv;

    rv = NULL;

    self = PyObject_NEW(PP_mappingObject, &PP_mapping_Type);
    if (self != NULL)
       {self->map = data;
	rv = (PyObject *) self;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_MAPPING_EXTRACTOR - extract a mapping from an object */

int _PY_mapping_extractor(PyObject *obj, void *arg)
   {int rv;
    PM_mapping **ppm;

    rv  = TRUE;
    ppm = (PM_mapping **) arg;

    if (obj == Py_None)
        *ppm = NULL;

    else if (PP_mapping_Check(obj))
       {PP_mappingObject *self;

        self = (PP_mappingObject *) obj;

        *ppm = self->map;}

    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = _PY_mapping_from_ptr(self->map);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_name_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->map->name);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_category_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->map->category);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_domain_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = _PY_set_from_ptr(self->map->domain);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_range_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = _PY_set_from_ptr(self->map->range);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_map_type_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->map->map_type);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_map_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = PY_COBJ_VOID_PTR((void *) self->map->map, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_file_type_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = PY_INT_LONG(self->map->file_type);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_file_info_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = PY_COBJ_VOID_PTR((void *) self->map->file_info, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_file_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = Py_BuildValue("s", self->map->file);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PP_mapping_map_next_get(PP_mappingObject *self, void *context)
   {PyObject *rv;

    rv = _PY_mapping_from_ptr(self->map->next);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char
 PP_mapping_map__doc__[] = "",
 PP_mapping_map_name__doc__[] = "",
 PP_mapping_map_category__doc__[] = "",
 PP_mapping_map_domain__doc__[] = "",
 PP_mapping_map_range__doc__[] = "",
 PP_mapping_map_map_type__doc__[] = "",
 PP_mapping_map_map__doc__[] = "",
 PP_mapping_map_file_type__doc__[] = "",
 PP_mapping_map_file_info__doc__[] = "",
 PP_mapping_map_file__doc__[] = "",
 PP_mapping_map_next__doc__[] = "",
 PP_mapping_Type__doc__[] = "";


static PyGetSetDef
 PP_mapping_getset[] = {
    {"map",       (getter) PP_mapping_map_get,           NULL, PP_mapping_map__doc__,           NULL},
    {"name",      (getter) PP_mapping_map_name_get,      NULL, PP_mapping_map_name__doc__,      NULL},
    {"category",  (getter) PP_mapping_map_category_get,  NULL, PP_mapping_map_category__doc__,  NULL},
    {"domain",    (getter) PP_mapping_map_domain_get,    NULL, PP_mapping_map_domain__doc__,    NULL},
    {"range",     (getter) PP_mapping_map_range_get,     NULL, PP_mapping_map_range__doc__,     NULL},
    {"map_type",  (getter) PP_mapping_map_map_type_get,  NULL, PP_mapping_map_map_type__doc__,  NULL},
    {"map",       (getter) PP_mapping_map_map_get,       NULL, PP_mapping_map_map__doc__,       NULL},
    {"file_type", (getter) PP_mapping_map_file_type_get, NULL, PP_mapping_map_file_type__doc__, NULL},
    {"file_info", (getter) PP_mapping_map_file_info_get, NULL, PP_mapping_map_file_info__doc__, NULL},
    {"file",      (getter) PP_mapping_map_file_get,      NULL, PP_mapping_map_file__doc__,      NULL},
    {"next",      (getter) PP_mapping_map_next_get,      NULL, PP_mapping_map_next__doc__,      NULL},
    {NULL}};

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

static int PP_mapping_tp_init(PP_mappingObject *self,
			      PyObject *args, PyObject *kwds)
   {int centering, rv, ok;
    char *name;
    char *cat;
    PM_set *domain;
    PM_set *range;
    PM_mapping *next;
    PP_setObject *odom;
    PP_setObject *oran;
    char *kw_list[] = {"name", "cat", "domain", "range",
                       "centering", "next", NULL};

    rv = -1;

    ok = PyArg_ParseTupleAndKeywords(args, kwds, "ssO!O!iO&:make_mapping",
				     kw_list, &name, &cat,
				     &PP_set_Type, &odom,
				     &PP_set_Type, &oran,
				     &centering,
				     _PY_mapping_extractor, &next);
    if (ok)
       {rv        = 0;
	domain    = odom->set;
	range     = oran->set;
	self->map = PM_make_mapping(name, cat, domain, range,
				    centering, next);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyTypeObject
 PP_mapping_Type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        "mapping",                      /* tp_name */
        sizeof(PP_mappingObject),       /* tp_basicsize */
        0,                              /* tp_itemsize */

/* methods to implement standard operations */
        (destructor) 0,                 /* tp_dealloc */
        (printfunc) 0,                  /* tp_print */
        (getattrfunc) 0,                /* tp_getattr */
        (setattrfunc) 0,                /* tp_setattr */
        (cmpfunc) 0,                    /* tp_compare */
        (reprfunc) 0,                   /* tp_repr */

/* method suites for standard classes */
        0,                              /* tp_as_number */
        0,                              /* tp_as_sequence */
        0,                              /* tp_as_mapping */

/* more standard operations (here for binary compatibility) */
        (hashfunc) 0,                   /* tp_hash */
        (ternaryfunc) 0,                /* tp_call */
        (reprfunc) 0,                   /* tp_str */
        (getattrofunc) 0,               /* tp_getattro */
        (setattrofunc) 0,               /* tp_setattro */

/* functions to access object as input/output buffer */
        0,                              /* tp_as_buffer */

/* flags to define presence of optional/expanded features */
        Py_TPFLAGS_DEFAULT,             /* tp_flags */
        PP_mapping_Type__doc__,         /* tp_doc */

/* assigned meaning in release 2.0 */

/* call function for all accessible objects */
        (traverseproc) 0,               /* tp_traverse */

/* delete references to contained objects */
        (inquiry) 0,                    /* tp_clear */

/* assigned meaning in release 2.1 */

/* rich comparisons */
        (richcmpfunc) 0,                /* tp_richcompare */

/* weak reference enabler */
        0,                              /* tp_weaklistoffset */

/* added in release 2.2 */

/* iterators */
        (getiterfunc) 0,                /* tp_iter */
        (iternextfunc) 0,               /* tp_iternext */

/* attribute descriptor and subclassing stuff */
        0,                              /* tp_methods */
        0,                              /* tp_members */
        PP_mapping_getset,              /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc) 0,               /* tp_descr_get */
        (descrsetfunc) 0,               /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_mapping_tp_init,   /* tp_init */
        (allocfunc) 0,                  /* tp_alloc */
        (newfunc) 0,                    /* tp_new */

#if PYTHON_API_VERSION >= 1012
        (freefunc) 0,                   /* tp_free */
#else
        (destructor) 0,                 /* tp_free */
#endif
        (inquiry) 0,                    /* tp_is_gc */
        0,                              /* tp_bases */
        0,                              /* tp_mro */
        0,                              /* tp_cache */
        0,                              /* tp_subclasses */
        0,                              /* tp_weaklist */

#if PYTHON_API_VERSION >= 1012
        (destructor) 0,                 /* tp_del */
#endif
	};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/
