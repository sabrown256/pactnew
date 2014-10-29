/*
 * PYPGS.C - PGS binding support
 *
 * include cpyright.h
 */

#include "pgsmodule.h"

/*--------------------------------------------------------------------------*/

/*                             PG_PALETTE ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* PALETTE_EXTRACTOR - extract a palette from an object */

int palette_extractor(PyObject *obj, void *arg)
   {int ret;

    ret = 1;

    if (obj == Py_None)
       *(void **) arg = NULL;

    else if (PY_PG_image_check(obj))
       {PY_PG_palette *self;

        self = (PY_PG_palette *) obj;
        *(PG_palette **) arg = self->pyo;}

    else
       ret = 0;

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_PG_palette_tp_init(PY_PG_palette *self,
			      PyObject *args, PyObject *kwds)
   {

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_palette_doc[] = "";

PY_DEF_TYPE(PG_palette);

/*--------------------------------------------------------------------------*/

/*                              PG_IMAGE ROUTINES                           */

/*--------------------------------------------------------------------------*/

#undef PY_DEF_DESTRUCTOR
#define PY_DEF_DESTRUCTOR	    PY_PG_image_tp_dealloc

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_BUFFER_EXTRACTOR - unsigned byte array */

int PP_buffer_extractor(PyObject *obj, void *arg)
   {int ret;

    ret = 1;

#ifdef HAVE_PY_NUMERIC

    PyArrayObject *arr;
    
    if (obj == Py_None)
       *(void **) arg = NULL;

    else
       {arr = (PyArrayObject *) PyArray_ContiguousFromObject(obj, 'd', 0, 0);
        if (arr == NULL)
           ret = 0;
        else
           *(void **) arg = (void *) arr->data;};

#endif

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *PY_PG_image_buffer_get(PY_PG_image *self, void *context)
   {PyObject *rv;

#ifdef HAVE_PY_NUMERIC
    int dims[2];
    PG_image *im;

    im = self->pyo;

    dims[0] = im->kmax;
    dims[1] = im->lmax;

    rv = PyArray_FromDimsAndData(2, dims, 'b', (char *) im->buffer);
#else
    rv = NULL;
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0

/* this has the correct image buffer routine */

static PyGetSetDef PY_PG_image_getset[] = {
    {"im", (getter) PY_PG_image_get, NULL, PY_PG_image_doc, NULL},
    {"version_id", (getter) PY_PG_image_version_id_get, NULL, PY_PG_image_version_id_doc, NULL},
    {"label", (getter) PY_PG_image_label_get, NULL, PY_PG_image_label_doc, NULL},
    {"xmin", (getter) PY_PG_image_xmin_get, NULL, PY_PG_image_xmin_doc, NULL},
    {"xmax", (getter) PY_PG_image_xmax_get, NULL, PY_PG_image_xmax_doc, NULL},
    {"ymin", (getter) PY_PG_image_ymin_get, NULL, PY_PG_image_ymin_doc, NULL},
    {"ymax", (getter) PY_PG_image_ymax_get, NULL, PY_PG_image_ymax_doc, NULL},
    {"zmin", (getter) PY_PG_image_zmin_get, NULL, PY_PG_image_zmin_doc, NULL},
    {"zmax", (getter) PY_PG_image_zmax_get, NULL, PY_PG_image_zmax_doc, NULL},
    {"element_type", (getter) PY_PG_image_element_type_get, NULL, PY_PG_image_element_type_doc, NULL},
    {"buffer", (getter) PY_PG_image_buffer_get, NULL, PY_PG_image_buffer_doc, NULL},
    {"kmax", (getter) PY_PG_image_kmax_get, NULL, PY_PG_image_kmax_doc, NULL},
    {"lmax", (getter) PY_PG_image_lmax_get, NULL, PY_PG_image_lmax_doc, NULL},
    {"size", (getter) PY_PG_image_size_get, NULL, PY_PG_image_size_doc, NULL},
    {"bits_pixel", (getter) PY_PG_image_bits_pixel_get, NULL, PY_PG_image_bits_pixel_doc, NULL},
    {"palette", (getter) PY_PG_image_palette_get, NULL, PY_PG_image_palette_doc, NULL},
    {NULL}     /* Sentinel */
};

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void PY_PG_image_tp_dealloc(PY_PG_image *self)
   {

    PG_rl_image(self->pyo);
    PY_TYPE(self)->tp_free((PyObject*)self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_PG_image_tp_init(PY_PG_image *self,
			       PyObject *args, PyObject *kwds)
   {

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_image_doc[] = "";

PY_DEF_TYPE(PG_image);

/*--------------------------------------------------------------------------*/

/*                               PG_GRAPH ROUTINES                          */

/*--------------------------------------------------------------------------*/

#undef PY_DEF_DESTRUCTOR
#define PY_DEF_DESTRUCTOR	    PY_PG_graph_tp_dealloc

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
				    PY_pcons_extractor,
				    &info, &type, &axis_type, &style,
				    &scatter, &marker, &color,
				    &start, &width))
       {result = PG_set_line_info(info, type, axis_type, style,
				  scatter, marker, color, start, width);
	rv     = PY_pcons_from_ptr(result);};

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
				    PY_pcons_extractor, &info, &type,
				    &axis_type, &style, &color,
				    &nlev, &ratio, &width,
				    &theta, &phi, &chi, &d))
       {result = PG_set_tds_info(info, type, axis_type,
				 style, color, nlev, ratio, width,
				 theta, phi, chi, d);
	rv     = PY_pcons_from_ptr(result);};

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
				    PY_pcons_extractor, &info, &type,
				    &axis_type, &style, &color, &width))
       {result = PG_set_tdv_info(info, type, axis_type,
				 style, color, width);
	rv     = PY_pcons_from_ptr(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GOTCHA: note difference between this and generated version */

static PyObject *PY_PG_graph_get_info(PY_PG_graph *self, void *context)
   {PyObject *rv;

    rv = PY_pcons_from_ptr(self->pyo->info);

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

    else if (PY_pcons_extractor(value, &self->pyo->info) != 0)
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
