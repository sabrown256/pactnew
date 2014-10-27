/*
 * Source Version: 9.0
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */

#define PY_DEF_DESTRUCTOR	    PY_PG_graph_tp_dealloc

#include "pgsmodule.h"

char
 PY_set_line_info_doc[] = "",
 PY_set_tds_info_doc[] = "",
 PY_set_tdv_info_doc[] = "";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_set_line_info - */

PyObject *PY_set_line_info(PyObject *self, PyObject *args, PyObject *kwds)
   {int type, axis_type, style, scatter, marker, color, start;
    double width;
    pcons *info, *result;
    PyObject *rv;
    char *kw_list[] = {"info", "type", "axis_type", "style",
                       "scatter", "marker", "color", "start",
		       "width", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O&iiiiiiid:set_line_info", kw_list,
				    PP_assoc_extractor,
				    &info, &type, &axis_type, &style,
				    &scatter, &marker, &color,
				    &start, &width))
       {result = PG_set_line_info(info, type, axis_type, style,
				  scatter, marker, color, start, width);
	rv     = PP_assoc_from_ptr(result);};

   return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_set_tds_info - */

PyObject *PY_set_tds_info(PyObject *self, PyObject *args, PyObject *kwds)
   {int type, axis_type, style, color, nlev;
    double ratio, width, theta, phi, chi, d;
    pcons *info, *result;
    PyObject *rv;
    char *kw_list[] = {"info", "type", "axis_type",
		       "style", "color", "nlev", "ratio",
		       "width", "theta", "phi", "chi", "d", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O&iiiiidddddd:set_tds_info", kw_list,
				    PP_assoc_extractor, &info, &type,
				    &axis_type, &style, &color,
				    &nlev, &ratio, &width,
				    &theta, &phi, &chi, &d))
       {result = PG_set_tds_info(info, type, axis_type,
				 style, color, nlev, ratio, width,
				 theta, phi, chi, d);
	rv     = PP_assoc_from_ptr(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_set_tdv_info - */

PyObject *PY_set_tdv_info(PyObject *self, PyObject *args, PyObject *kwds)
   {int type, axis_type, style, color;
    double width;
    pcons *info, *result;
    PyObject *rv;
    char *kw_list[] = {"info", "type", "axis_type",
		       "style", "color", "width", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O&iiiid:set_tdv_info", kw_list,
				    PP_assoc_extractor, &info, &type,
				    &axis_type, &style, &color, &width))
       {result = PG_set_tdv_info(info, type, axis_type,
				 style, color, width);
	rv     = PP_assoc_from_ptr(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                              GENERATED STUFF                             */

/*--------------------------------------------------------------------------*/

/* GOTCHA: note difference between this and generated version */

static PyObject *PY_PG_graph_get_info(PY_PG_graph *self, void *context)
   {PyObject *rv;

    rv = PP_assoc_from_ptr(self->pyo->info);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* setter not generated */

static int PY_PG_graph_set_info(PY_PG_graph *self,
				     PyObject *value, void *context)
   {int rv;

    rv = -1;

    if (value == NULL)
       PyErr_SetString(PyExc_TypeError,
                        "attribute deletion is not supported");

    else if (PP_assoc_extractor(value, &self->pyo->info) != 0)
      {SC_mark(self->pyo->info, 1);
       rv = 0;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0

/* GOTCHA - this is the correct version - it has the setter methods */

static PyGetSetDef PY_PG_graph_getset[] = {
    {"data", (getter) PY_PG_graph_get, NULL, PY_PG_graph_doc, NULL},
    {"rendering", (getter) PY_PG_graph_get_rendering, NULL, PY_PG_graph_doc_rendering, NULL},
    {"mesh", (getter) PY_PG_graph_get_mesh, NULL, PY_PG_graph_doc_mesh, NULL},
    {"info_type", (getter) PY_PG_graph_get_info_type, NULL, PY_PG_graph_doc_info_type, NULL},
    {"info", (getter) PY_PG_graph_get_info, (setter) PY_PG_graph_set_info, PY_PG_graph_doc_info, NULL},
    {"identifier", (getter) PY_PG_graph_get_identifier, NULL, PY_PG_graph_doc_identifier, NULL},
    {"use", (getter) PY_PG_graph_get_use, NULL, PY_PG_graph_doc_use, NULL},
    {NULL}};

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void PY_PG_graph_tp_dealloc(PY_PG_graph *self)
   {

    PG_rl_graph(self->pyo, TRUE, TRUE);

    PY_TYPE(self)->tp_free((PyObject*) self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_PG_graph_tp_init(PY_PG_graph *self,
			       PyObject *args, PyObject *kwds)
   {

    return 0;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_graph_doc[] = "";

PY_DEF_TYPE(PG_graph);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
