/*
 * PYPML.C - support for PML Python bindings
 *
 */

#include "cpyright.h"
#include "py_int.h"

char
 PP_make_set_1d_doc[] = "",
 PP_make_ac_set_doc[] = "";

/*--------------------------------------------------------------------------*/

/*                               FIELD ROUTINES                             */

/*--------------------------------------------------------------------------*/

/* _PY_OPT_PM_FIELD - handle BLANG binding related operations */

void *_PY_opt_PM_field(PM_field *x, bind_opt wh, void *a)
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

static int PY_PM_field_tp_init(PY_PM_field *self,
			       PyObject *args, PyObject *kwds)
   {

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char
 PY_PM_field_doc[] = "";

PY_DEF_TYPE_R(PM_field, "field");

/*--------------------------------------------------------------------------*/

/*                           MESH_TOPOLOGY ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* _PY_OPT_PM_MESH_TOPOLOGY - handle BLANG binding related operations */

void *_PY_opt_PM_mesh_topology(PM_mesh_topology *x, bind_opt wh, void *a)
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

static int PY_PM_mesh_topology_tp_init(PY_PM_mesh_topology *self,
				       PyObject *args, PyObject *kwds)
   {int nd, rv;
    int *bnp, *bnc;
    long **bnd;
    char *kw_list[] = {"nd", "bnp", "bnc", "bnd", NULL};

    rv = -1;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "iO&O&O&:make_topology", kw_list,
				    &nd,
				    iarray_extractor, &bnp,
				    iarray_extractor, &bnc,
				    pplong_extractor, &bnd))
       {self->pyo = PM_make_topology(nd, bnp, bnc, bnd);
	rv        = 0;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char
 PY_PM_mesh_topology_doc[] = "";

PY_DEF_TYPE_R(PM_mesh_topology, "mesh_topology");

/*--------------------------------------------------------------------------*/

/*                              PM_SET ROUTINES                             */

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

static int PY_PM_set_name_set(PY_PM_set *self, PyObject *value,
			      void *context)
   {int rv;

    rv = -1;

    if (value == NULL)
       PyErr_SetString(PyExc_TypeError,
		       "attribute deletion is not supported");

    else if (PyArg_Parse(value, "s", &self->pyo->name))
       rv = 0;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_MAKE_SET_1D - */

PyObject *PP_make_set_1d(PyObject *self, PyObject *args, PyObject *kwds)
   {int cp, nd, max, nde;
    char *name, *type;
    double *elem;
    PM_set *result;
    PyObject *rv;
    char *kw_list[] = {"name", "type", "cp", "nd", "max",
		       "nde", "elem", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "ssiiiiO&:make_set_1d", kw_list,
				    &name, &type, &cp, &nd,
				    &max, &nde,
				    REAL_array_extractor, &elem))
       {result = PM_make_set(name, type, cp, nd, max, nde, elem);
	rv     = PY_PM_set_from_ptr(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_MAKE_AC_SET - */

PyObject *PP_make_ac_set(PyObject *self, PyObject *args, PyObject *kwds)
   {int cp, nde;
    double *x, *y;
    char *name, *type;
    PM_set *result;
    PM_mesh_topology *mt;
    PY_PM_mesh_topology *mto;
    PyObject *rv;
    char *kw_list[] = {"name", "type", "cp", "mt", "nde", "x", "y", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "ssiO!iO&O&:make_ac_set", kw_list,
				    &name, &type, &cp,
				    &PY_PM_mesh_topology_type, &mto,
				    &nde,
				    REAL_array_extractor, &x,
				    REAL_array_extractor, &y))
       {mt     = mto->pyo;
	result = PM_make_ac_set(name, type, cp, mt, nde, x, y);
	rv     = PY_PM_set_from_ptr(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_PM_set_tp_init(PY_PM_set *self, PyObject *args, PyObject *kwds)
   {int cp, nd, nde, rv;
    int *maxes;
    long ne;
    char *name, *type, *symtype, *toptype, *inftype;
    void *elem, *sym, *top, *inf;
    double *metric;
    PM_field *opers;
    PM_set *next;
    PY_PM_field *opersobj;
    PY_PM_set *nextobj;
    char *kw_list[] = {"name", "type", "cp", "ne", "nd",
		       "nde", "maxes", "elem", "opers",
		       "metric", "symtype", "sym",
		       "toptype", "top", "inftype", "inf",
		       "next", NULL};

    rv = -1;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "ssiliiO&zO!O&szszszO!:make_set", kw_list,
				    &name, &type, &cp, &ne, &nd, &nde,
				    iarray_extractor, &maxes, &elem,
				    &PY_PM_field_type, &opersobj,
				    REAL_array_extractor, &metric,
				    &symtype, &sym, &toptype, &top,
				    &inftype, &inf,
				    &PY_PM_set_type, &nextobj))
       {rv = 0;
	opers = opersobj->pyo;
	next  = nextobj->pyo;
	self->pyo = PM_mk_set(name, type, cp, ne, nd, nde, maxes, elem,
			      opers, metric, symtype, sym, toptype, top,
			      inftype, inf, next);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char
 PY_PM_set_doc[] = "";

PY_DEF_TYPE_R(PM_set, "set");

/*--------------------------------------------------------------------------*/

/*                            PM_MAPPING ROUTINES                           */

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

static int PY_PM_mapping_tp_init(PY_PM_mapping *self, PyObject *args,
				 PyObject *kwds)
   {int centering, rv;
    char *name, *cat;
    PM_set *domain, *range;
    PM_mapping *next;
    PY_PM_set *domainobj, *rangeobj;
    char *kw_list[] = {"name", "cat", "domain", "range",
	               "centering", "next", NULL};

    rv = -1;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "ssO!O!iO&:make_mapping", kw_list,
				    &name, &cat,
				    &PY_PM_set_type, &domainobj,
				    &PY_PM_set_type, &rangeobj,
				    &centering,
				    PY_PM_mapping_extractor, &next))
       {rv        = 0;
	domain    = domainobj->pyo;
	range     = rangeobj->pyo;
	self->pyo = PM_make_mapping(name, cat, domain, range,
				    centering, next);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char
 PY_PM_mapping_doc[] = "";

PY_DEF_TYPE_R(PM_mapping, "mapping");

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
