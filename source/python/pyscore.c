/*
 * PYSCORE.C - support for SCORE Python bindings
 *
 */

#include "cpyright.h"
#include "py_int.h"

char
 PY_set_count_doc[] = "",
 PY_set_io_hooks_doc[] = "",
 PP_setform_doc[] = "";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_set_count - */

PyObject *PY_set_count(PyObject *self, PyObject *args, PyObject *kwds)
   {int n, result;
    void *p;
    PyObject *pobj, *rv;
    char *kw_list[] = {"p", "n", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds, "O!i:set_count", kw_list,
				    &PY_COBJ_TYPE, &pobj, &n))
       {p      = (void *) PY_GET_PTR(pobj);
	result = SC_set_count(p, n);
	rv     = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_set_io_hooks - */

PyObject *PY_set_io_hooks(PyObject *self, PyObject *args, PyObject *kwds)
   {int whch;
    PyObject *rv;
    char *kw_list[] = {"whch", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds, "i:set_io_hooks", kw_list,
                                     &whch))
       {SC_set_io_hooks(whch);
	Py_INCREF(Py_None);
	rv = Py_None;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_setform - */

PyObject *PP_setform(PyObject *self, PyObject *args, PyObject *kwds)
   {int ok, array, structure, scalar;
    PyObject *rv;
    char *kw_list[] = {"array", "struct", "scalar", NULL};

    rv = NULL;

    array     = AS_NONE;
    structure = AS_NONE;
    scalar    = AS_NONE;
    if (PyArg_ParseTupleAndKeywords(args, kwds, "|iii:setform", kw_list,
				    &array, &structure, &scalar))
       {ok = _PP_assign_form(&PP_global_form, array, structure, scalar);
	if (ok >= 0)
	   rv = Py_BuildValue("(iii)",
			      PP_global_form.array_kind,
			      PP_global_form.struct_kind,
			      PP_global_form.scalar_kind);};
   return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
