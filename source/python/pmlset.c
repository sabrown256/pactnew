/*
 * PMLSET.C - Python binding routines for PM_set
 *
 * include cpyright.h
 */

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/

/*                              GENERATED STUFF                             */

/*--------------------------------------------------------------------------*/

static int PY_PM_set_set_name(PY_PM_set *self, PyObject *value, void *context)
{
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError,
                        "attribute deletion is not supported");
        return -1;
    }
    if (!PyArg_Parse(value, "s", &self->pyo->name))
        return -1;
    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0

/* GOTCHA: example of a getset with setter method */

static PyGetSetDef PY_PM_set_getset[] = {

    {"name", (getter) PY_PM_set_get_name, (setter) PY_PM_set_set_name, PY_PM_set_doc_name, NULL},

    {NULL}};

#endif

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

static char PY_PM_set_doc[] = "";

PY_DEF_TYPE(PM_set);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
