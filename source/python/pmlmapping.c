/*
 * Source Version: 9.0
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/

/*                              GENERATED STUFF                             */

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
				    _PY_mapping_extractor, &next))
       {rv        = 0;
	domain    = domainobj->pyo;
	range     = rangeobj->pyo;
	self->pyo = PM_make_mapping(name, cat, domain, range,
				    centering, next);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PM_mapping_doc[] = "";

PY_DEF_TYPE(PM_mapping);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
