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

    PY_PM_set_type.tp_new   = PyType_GenericNew;
    PY_PM_set_type.tp_alloc = PyType_GenericAlloc;
    nerr += (PyType_Ready(&PY_PM_set_type) < 0);
    nerr += (PyDict_SetItemString(d, "set", (PyObject *) &PY_PM_set_type) < 0);

    PY_PM_mapping_type.tp_new   = PyType_GenericNew;
    PY_PM_mapping_type.tp_alloc = PyType_GenericAlloc;
    nerr += (PyType_Ready(&PY_PM_mapping_type) < 0);

    nerr += (PyDict_SetItemString(d, "mapping", (PyObject *) &PY_PM_mapping_type) < 0);

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
/*--------------------------------------------------------------------------*/

static PyObject *PY_PM_set_opers_get(PY_PM_set *self, void *context)
   {PyObject *rv;

    rv = PPfield_from_ptr(self->pyo->opers);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            PM_MAPPING_ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PY_MAPPING_EXTRACTOR - extract a mapping from an object */

int _PY_mapping_extractor(PyObject *obj, void *arg)
   {int rv;
    PM_mapping **ppm;
    extern int PY_PM_mapping_Check(PyObject *op);

    rv  = TRUE;
    ppm = (PM_mapping **) arg;

    if (obj == Py_None)
        *ppm = NULL;

    else if (PY_PM_mapping_Check(obj))
       {PY_PM_mapping *self;

        self = (PY_PM_mapping *) obj;

        *ppm = self->pyo;}

    else
       rv = FALSE;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
