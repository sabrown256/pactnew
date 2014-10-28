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

/* PP_make_set_1d - */

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

/* PP_make_ac_set - */

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

/* PY_INIT_PML_INT - interim initializations for PML bindings */

int PY_init_pml_int(PyObject *m, PyObject *d)
   {int nerr;
    extern int PY_init_pml(PyObject *m, PyObject *d);

    nerr = PY_init_pml(m, d);

    return(nerr);}

/*--------------------------------------------------------------------------*/

/*                              PM_SET_ROUTINES                             */

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

/*                            PM_MAPPING_ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PY_MAPPING_EXTRACTOR - extract a mapping from an object */

int _PY_mapping_extractor(PyObject *obj, void *arg)
   {int rv;
    PM_mapping **ppm;

    rv  = TRUE;
    ppm = (PM_mapping **) arg;

    if (obj == Py_None)
        *ppm = NULL;

    else if (PY_PM_mapping_check(obj))
       {PY_PM_mapping *self;

        self = (PY_PM_mapping *) obj;

        *ppm = self->pyo;}

    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
