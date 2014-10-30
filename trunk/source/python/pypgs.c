/*
 * PYPGS.C - PGS binding support
 *
 * include cpyright.h
 */

#include "pgsmodule.h"

/*--------------------------------------------------------------------------*/

/*                             PG_PALETTE ROUTINES                          */

/*--------------------------------------------------------------------------*/

PY_DEF_EXTRACTOR(PG_palette);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_PG_palette_tp_init(PY_PG_palette *self,
			      PyObject *args, PyObject *kwds)
   {

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char
 PY_PG_palette_doc[] = "";

#undef PY_DEF_DESTRUCTOR
#undef PY_DEF_TP_METH
#undef PY_DEF_TP_PRINT
#undef PY_DEF_TP_CALL
#undef PY_DEF_AS_MAP

#define PY_DEF_DESTRUCTOR           NULL
#define PY_DEF_TP_METH              NULL
#define PY_DEF_TP_PRINT             NULL
#define PY_DEF_TP_CALL              NULL
#define PY_DEF_AS_MAP               NULL

PY_DEF_TYPE(PG_palette);

/*--------------------------------------------------------------------------*/

/*                              PG_IMAGE ROUTINES                           */

/*--------------------------------------------------------------------------*/

PY_DEF_EXTRACTOR(PG_image);

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

    PY_self_free(self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_PG_image_tp_init(PY_PG_image *self,
			       PyObject *args, PyObject *kwds)
   {

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#undef PY_DEF_DESTRUCTOR
#undef PY_DEF_TP_METH
#undef PY_DEF_TP_PRINT
#undef PY_DEF_TP_CALL
#undef PY_DEF_AS_MAP

#define PY_DEF_DESTRUCTOR	    PY_PG_image_tp_dealloc
#define PY_DEF_TP_METH              NULL
#define PY_DEF_TP_PRINT             NULL
#define PY_DEF_TP_CALL              NULL
#define PY_DEF_AS_MAP               NULL

char
 PY_PG_image_doc[] = "";

PY_DEF_TYPE(PG_image);

/*--------------------------------------------------------------------------*/

/*                               PG_GRAPH ROUTINES                          */

/*--------------------------------------------------------------------------*/

char
 PY_set_line_info_doc[] = "",
 PY_set_tds_info_doc[] = "",
 PY_set_tdv_info_doc[] = "";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PY_DEF_EXTRACTOR(PG_graph);

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

    PY_self_free(self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_PG_graph_tp_init(PY_PG_graph *self,
			       PyObject *args, PyObject *kwds)
   {

    return 0;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#undef PY_DEF_DESTRUCTOR
#undef PY_DEF_TP_METH
#undef PY_DEF_TP_PRINT
#undef PY_DEF_TP_CALL
#undef PY_DEF_AS_MAP

#define PY_DEF_DESTRUCTOR	    PY_PG_graph_tp_dealloc
#define PY_DEF_TP_METH              NULL
#define PY_DEF_TP_PRINT             NULL
#define PY_DEF_TP_CALL              NULL
#define PY_DEF_AS_MAP               NULL

char
 PY_PG_graph_doc[] = "";

PY_DEF_TYPE(PG_graph);

/*--------------------------------------------------------------------------*/

/*                               PG_DEVICE ROUTINES                         */

/*--------------------------------------------------------------------------*/

PY_DEF_EXTRACTOR(PG_device);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_font_doc[] = "";

static PyObject *PY_PG_device_set_font(PY_PG_device *self,
				       PyObject *args,
				       PyObject *kwds)
   {int size;
    char *face, *style;
    PyObject *rv;
    char *kw_list[] = {"face", "style", "size", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds, "ssi:set_font", kw_list,
				    &face, &style, &size))
       {
#if 0
	int result;
	result = PG_fset_font(self->pyo, face, style, size);
	rv = PY_INT_LONG(result);
#else
	if (self->pyo != NULL)
	   {if (self->pyo->set_font != NULL)
	       self->pyo->set_font(self->pyo, face, style, size);}
	Py_INCREF(Py_None);
	rv = Py_None;
#endif
        };

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_viewport_doc[] = "";

static PyObject *PY_PG_device_set_viewport(PY_PG_device *self,
					   PyObject *args,
					   PyObject *kwds)
   {double ndc[PG_BOXSZ];
    PyObject *rv;
    char *kw_list[] = {"x1", "x2", "y1", "y2", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "dddd:set_viewport", kw_list,
				    &ndc[0], &ndc[1], &ndc[2], &ndc[3]))
       {PG_set_viewspace(self->pyo, 2, NORMC, ndc);
	rv = Py_None;

	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_window_doc[] = "";

static PyObject *PY_PG_device_set_window(PY_PG_device *self,
					 PyObject *args,
					 PyObject *kwds)
   {double wc[PG_BOXSZ];
    PyObject *rv;
    char *kw_list[] = {"xmn", "xmx", "ymn", "ymx", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "dddd:set_window", kw_list,
				    &wc[0], &wc[1], &wc[2], &wc[3]))
       {PG_set_viewspace(self->pyo, 2, WORLDC, wc);
	rv = Py_None;

	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_draw_box_doc[] = "";

static PyObject *PY_PG_device_draw_box(PY_PG_device *self,
				       PyObject *args,
				       PyObject *kwds)
   {double bx[PG_BOXSZ];
    PyObject *rv;
    char *kw_list[] = {"xmin", "xmax", "ymin", "ymax", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "dddd:draw_box", kw_list,
				    &bx[0], &bx[1], &bx[2], &bx[3]))
       {PG_draw_box_n(self->pyo, 2, WORLDC, bx);
	rv = Py_None;

	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_get_text_ext_doc[] = "";

static PyObject *PY_PG_device_get_text_ext(PY_PG_device *self,
					   PyObject *args,
					   PyObject *kwds)
   {char *s;
    double x[PG_SPACEDM];
    PyObject *rv;
    char *kw_list[] = {"s", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "s:get_text_ext", kw_list,
				    &s))
       {PG_get_text_ext_n(self->pyo, 2, WORLDC, s, x);
	rv = Py_BuildValue("ff", x[0], x[1]);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_draw_line_doc[] = "";

static PyObject *PY_PG_device_draw_line(PY_PG_device *self,
					PyObject *args,
					PyObject *kwds)
   {double x1[PG_SPACEDM], x2[PG_SPACEDM];
    PyObject *rv;
    char *kw_list[] = {"x1", "y1", "x2", "y2", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "dddd:draw_line", kw_list,
				    &x1[0], &x1[1], &x2[0], &x2[0]))
       {PG_draw_line_n(self->pyo, 2, WORLDC, x1, x2, self->pyo->clipping);
	rv = Py_None;

	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_write_WC_doc[] = "";

static PyObject *PY_PG_device_write_WC(PY_PG_device *self,
				       PyObject *args,
				       PyObject *kwds)
   {int result;
    double x, y;
    double p[PG_SPACEDM];
    char *fmt, *arg;
    PyObject *rv;
    char *kw_list[] = {"x", "y", "fmt", "arg", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "ddss:write_WC", kw_list,
				    &x, &y, &fmt, &arg))
       {

/* XXX - This is expecting a variable number of arguments */
	p[0] = x;
	p[1] = y;

	result = PG_write_n(self->pyo, 2, WORLDC, p, fmt, arg);
	rv = PY_INT_LONG(result);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_draw_graph_doc[] = "";

static PyObject *PY_PG_device_draw_graph(PY_PG_device *self,
					 PyObject *args,
					 PyObject *kwds)
   {PG_graph *data;
    PY_PG_graph *dataobj;
    PyObject *rv;
    char *kw_list[] = {"data", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:draw_graph", kw_list,
				    &PY_PG_graph_type, &dataobj))
       {data = dataobj->pyo;
	PG_draw_graph(self->pyo, data);
	rv = Py_None;
	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_draw_image_doc[] = "";

static PyObject *PY_PG_device_draw_image(PY_PG_device *self,
					 PyObject *args,
					 PyObject *kwds)
   {char *label;
    void *alist;
    PY_PG_image *imobj;
    PG_image *im;
    PyObject *rv;
    char *kw_list[] = {"im", "label", "alist", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!sz:draw_image", kw_list,
				    &PY_PG_image_type, &imobj,
				    &label, &alist))
       {im = imobj->pyo;
	PG_draw_image(self->pyo, im, label, alist);
	rv = Py_None;
	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_contour_plot_doc[] = "";

static PyObject *PY_PG_device_contour_plot(PY_PG_device *self,
					   PyObject *args,
					   PyObject *kwds)
   {PG_graph *data;
    PY_PG_graph *dataobj;
    PyObject *rv;
    char *kw_list[] = {"data", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:contour_plot", kw_list,
				    &PY_PG_graph_type, &dataobj))
       {data = dataobj->pyo;
	PG_contour_plot(self->pyo, data);
	rv = Py_None;
	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_poly_fill_plot_doc[] = "";

static PyObject *PY_PG_device_poly_fill_plot(PY_PG_device *self,
					     PyObject *args,
					     PyObject *kwds)
   {PG_graph *data;
    PY_PG_graph *dataobj;
    PyObject *rv;
    char *kw_list[] = {"data", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O!:poly_fill_plot", kw_list,
				    &PY_PG_graph_type, &dataobj))
       {data = dataobj->pyo;
	PG_poly_fill_plot(self->pyo, data);
	rv = Py_None;
	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_draw_surface_doc[] = "";

static PyObject *PY_PG_device_draw_surface(PY_PG_device *self,
					   PyObject *args,
					   PyObject *kwds)
   {int nn, color, style, type;
    int *cnnct;
    double xmn, xmx, ymn, ymx;
    double theta, phi, chi;
    double width;
    double va[3];
    double *a1, *a2, *extr, *x, *y;
    double *r[PG_SPACEDM];
    char *name, *mesh_type;
    pcons *alist;
    PyObject *rv;
    char *kw_list[] = {"a1", "a2", "extr", "x", "y", "nn",
		       "xmn", "xmx", "ymn", "ymx",
		       "theta", "phi", "chi", "width",
		       "color", "style", "type",
		       "name", "mesh_type", "cnnct", "alist", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O&O&O&O&O&iddddddddiiissO&O&:draw_surface", kw_list,
				    REAL_array_extractor, &a1,
				    REAL_array_extractor, &a2,
				    REAL_array_extractor, &extr,
				    REAL_array_extractor, &x,
				    REAL_array_extractor, &y,
				    &nn, &xmn, &xmx, &ymn, &ymx,
				    &theta, &phi, &chi, &width,
				    &color, &style, &type,
				    &name, &mesh_type,
				    iarray_extractor, &cnnct,
				    PY_pcons_extractor, &alist))
       {r[0] = x;
	r[1] = y;

	va[0] = theta;
	va[1] = phi;
	va[2] = chi;

	PG_draw_surface_n(self->pyo, a1, a2, extr, r, nn, va, width,
			  color, style, type, name, mesh_type, cnnct, alist);
	rv = Py_None;

	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_draw_picture_doc[] = "";

static PyObject *PY_PG_device_draw_picture(PY_PG_device *self,
					   PyObject *args,
					   PyObject *kwds)
   {int ptyp, bndp, cbnd, sbnd, mshp, cmsh, smsh;
    double wbnd, wmsh;
    PM_mapping *f;
    PyObject *rv;
    char *kw_list[] = {"f", "ptyp", "bndp", "cbnd", "sbnd", "wbnd",
		       "mshp", "cmsh", "smsh", "wmsh", NULL};

    rv = NULL;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "O&iiiidiiid:draw_picture", kw_list,
				    PY_PM_mapping_extractor, &f,
				    &ptyp, &bndp, &cbnd, &sbnd, &wbnd,
				    &mshp, &cmsh, &smsh, &wmsh))
       {PG_draw_picture(self->pyo, f, ptyp, bndp, cbnd, sbnd, wbnd,
			mshp, cmsh, smsh, wmsh);
	rv = Py_None;

	Py_INCREF(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                         BLANG GENERATED WRAPPERS                         */

/*--------------------------------------------------------------------------*/

#if defined(HAVE_PY_NUMPY)

/*--------------------------------------------------------------------------*/

static PyMethodDef PY_PG_device_methods[] = {
 _PYD_PG_fset_char_path,
 _PYD_PG_fset_line_style,
 _PYD_PG_fset_line_width,
 _PYD_PG_fset_line_color,
 _PYD_PG_fset_text_color,
 _PYD_PG_turn_data_id,
 _PYD_PG_clear_window,
 _PYD_PG_close_device,
 _PYD_PG_finish_plot,
 _PYD_PG_update_vs,
 _PYD_PG_open_device,
 _PYD_PG_fset_palette,
{"set_font", (PyCFunction)PY_PG_device_set_font, METH_KEYWORDS, PY_PG_device_set_font_doc},
{"set_viewport", (PyCFunction)PY_PG_device_set_viewport, METH_KEYWORDS, PY_PG_device_set_viewport_doc},
{"set_window", (PyCFunction)PY_PG_device_set_window, METH_KEYWORDS, PY_PG_device_set_window_doc},
{"draw_box", (PyCFunction)PY_PG_device_draw_box, METH_KEYWORDS, PY_PG_device_draw_box_doc},
{"get_text_ext", (PyCFunction)PY_PG_device_get_text_ext, METH_KEYWORDS, PY_PG_device_get_text_ext_doc},
{"draw_line", (PyCFunction)PY_PG_device_draw_line, METH_KEYWORDS, PY_PG_device_draw_line_doc},
{"write_WC", (PyCFunction)PY_PG_device_write_WC, METH_KEYWORDS, PY_PG_device_write_WC_doc},
{"draw_graph", (PyCFunction)PY_PG_device_draw_graph, METH_KEYWORDS, PY_PG_device_draw_graph_doc},
{"draw_image", (PyCFunction)PY_PG_device_draw_image, METH_KEYWORDS, PY_PG_device_draw_image_doc},
{"contour_plot", (PyCFunction)PY_PG_device_contour_plot, METH_KEYWORDS, PY_PG_device_contour_plot_doc},
{"poly_fill_plot", (PyCFunction)PY_PG_device_poly_fill_plot, METH_KEYWORDS, PY_PG_device_poly_fill_plot_doc},
{"draw_surface", (PyCFunction)PY_PG_device_draw_surface, METH_KEYWORDS, PY_PG_device_draw_surface_doc},
{"draw_picture", (PyCFunction)PY_PG_device_draw_picture, METH_KEYWORDS, PY_PG_device_draw_picture_doc},
/* DO-NOT-DELETE splicer.begin(pgs.device.extra_mlist) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.device.extra_mlist) */
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/

/*                             MANUAL WRAPPERS                              */

/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_char_path_doc[] = "";

static PyObject *PY_PG_device_set_char_path(PY_PG_device *self,
                                            PyObject *args,
                                            PyObject *kwds)
{
    double x[PG_SPACEDM];
    char *kw_list[] = {"x", "y", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dd:set_char_path", kw_list,
                                     &x[0], &x[1]))
        return NULL;
    PG_fset_char_path(self->pyo, x);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_color_line_doc[] = "";

static PyObject *PY_PG_device_set_color_line(PY_PG_device *self,
					     PyObject *args,
					     PyObject *kwds)
{
    int color;
    int mapped;
    char *kw_list[] = {"color", "mapped", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ii:set_color_line", kw_list,
                                     &color, &mapped))
        return NULL;
    PG_fset_line_color(self->pyo, color, mapped);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_line_color_doc[] = "";

static PyObject *PY_PG_device_set_line_color(PY_PG_device *self,
					     PyObject *args,
					     PyObject *kwds)
{
    int color;
    char *kw_list[] = {"color", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:set_line_color", kw_list,
                                     &color))
        return NULL;
    PG_fset_line_color(self->pyo, color, TRUE);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_line_style_doc[] = "";

static PyObject *PY_PG_device_set_line_style(PY_PG_device *self,
					     PyObject *args,
					     PyObject *kwds)
{
    int style;
    char *kw_list[] = {"style", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:set_line_style", kw_list,
                                     &style))
        return NULL;
    PG_fset_line_style(self->pyo, style);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_line_width_doc[] = "";

static PyObject *PY_PG_device_set_line_width(PY_PG_device *self,
					     PyObject *args,
					     PyObject *kwds)
{
    double width;
    char *kw_list[] = {"width", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "d:set_line_width", kw_list,
                                     &width))
        return NULL;
    PG_fset_line_width(self->pyo, width);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_text_color_doc[] = "";

static PyObject *PY_PG_device_set_text_color(PY_PG_device *self,
					     PyObject *args,
					     PyObject *kwds)
{
    int color;
    char *kw_list[] = {"color", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:set_text_color", kw_list,
                                     &color))
        return NULL;
    PG_fset_text_color(self->pyo, color, TRUE);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_color_text_doc[] = "";

static PyObject *PY_PG_device_set_color_text(PY_PG_device *self,
					     PyObject *args,
					     PyObject *kwds)
{
    int color;
    int mapped;
    char *kw_list[] = {"color", "mapped", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ii:set_color_text", kw_list,
                                     &color, &mapped))
        return NULL;
    PG_fset_text_color(self->pyo, color, mapped);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_turn_data_id_doc[] = "";

static PyObject *PY_PG_device_turn_data_id(PY_PG_device *self,
					   PyObject *args,
					   PyObject *kwds)
{
    int t;
    char *kw_list[] = {"t", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:turn_data_id", kw_list,
                                     &t))
        return NULL;
    PG_turn_data_id(self->pyo, t);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_clear_window_doc[] = "";

static PyObject *PY_PG_device_clear_window(PY_PG_device *self,
					   PyObject *args,
					   PyObject *kwds)
{
    PG_clear_window(self->pyo);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_close_doc[] = "";

static PyObject *PY_PG_device_close(PY_PG_device *self,
				    PyObject *args,
				    PyObject *kwds)
{
    PG_close_device(self->pyo);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_finish_plot_doc[] = "";

static PyObject *PY_PG_device_finish_plot(PY_PG_device *self,
					  PyObject *args,
					  PyObject *kwds)
{
    PG_finish_plot(self->pyo);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_update_vs_doc[] = "";

static PyObject *PY_PG_device_update_vs(PY_PG_device *self,
					PyObject *args,
					PyObject *kwds)
{
    PG_update_vs(self->pyo);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_open_doc[] = "";

static PyObject *PY_PG_device_open(PY_PG_device *self,
				   PyObject *args,
				   PyObject *kwds)
{
    double xf;
    double yf;
    double dxf;
    double dyf;
    char *kw_list[] = {"xf", "yf", "dxf", "dyf", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddd:open", kw_list,
                                     &xf, &yf, &dxf, &dyf))
        return NULL;
    PG_open_device(self->pyo, xf, yf, dxf, dyf);
    Py_INCREF(Py_None);
    return Py_None;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static char PY_PG_device_set_palette_doc[] = "";

static PyObject *PY_PG_device_set_palette(PY_PG_device *self,
					  PyObject *args,
					  PyObject *kwds)
{
    char *name;
    char *kw_list[] = {"name", NULL};
    PG_palette *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:set_palette", kw_list,
                                     &name))
        return NULL;
    result = PG_fset_palette(self->pyo, name);
    return PPpalette_from_ptr(result);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMethodDef
 PY_PG_device_methods[] = {
{"clear_window", (PyCFunction)PY_PG_device_clear_window, METH_NOARGS, PY_PG_device_clear_window_doc},
{"close", (PyCFunction)PY_PG_device_close, METH_NOARGS, PY_PG_device_close_doc},
{"finish_plot", (PyCFunction)PY_PG_device_finish_plot, METH_NOARGS, PY_PG_device_finish_plot_doc},
{"set_char_path", (PyCFunction)PY_PG_device_set_char_path, METH_KEYWORDS, PY_PG_device_set_char_path_doc},
{"set_font", (PyCFunction)PY_PG_device_set_font, METH_KEYWORDS, PY_PG_device_set_font_doc},
{"set_color_line", (PyCFunction)PY_PG_device_set_color_line, METH_KEYWORDS, PY_PG_device_set_color_line_doc},
{"set_line_style", (PyCFunction)PY_PG_device_set_line_style, METH_KEYWORDS, PY_PG_device_set_line_style_doc},
{"set_line_width", (PyCFunction)PY_PG_device_set_line_width, METH_KEYWORDS, PY_PG_device_set_line_width_doc},
{"update_vs", (PyCFunction)PY_PG_device_update_vs, METH_NOARGS, PY_PG_device_update_vs_doc},
{"set_line_color", (PyCFunction)PY_PG_device_set_line_color, METH_KEYWORDS, PY_PG_device_set_line_color_doc},
{"set_text_color", (PyCFunction)PY_PG_device_set_text_color, METH_KEYWORDS, PY_PG_device_set_text_color_doc},
{"set_color_text", (PyCFunction)PY_PG_device_set_color_text, METH_KEYWORDS, PY_PG_device_set_color_text_doc},
{"turn_data_id", (PyCFunction)PY_PG_device_turn_data_id, METH_KEYWORDS, PY_PG_device_turn_data_id_doc},
{"set_viewport", (PyCFunction)PY_PG_device_set_viewport, METH_KEYWORDS, PY_PG_device_set_viewport_doc},
{"set_window", (PyCFunction)PY_PG_device_set_window, METH_KEYWORDS, PY_PG_device_set_window_doc},
{"draw_box", (PyCFunction)PY_PG_device_draw_box, METH_KEYWORDS, PY_PG_device_draw_box_doc},
{"get_text_ext", (PyCFunction)PY_PG_device_get_text_ext, METH_KEYWORDS, PY_PG_device_get_text_ext_doc},
{"draw_line", (PyCFunction)PY_PG_device_draw_line, METH_KEYWORDS, PY_PG_device_draw_line_doc},
{"write_WC", (PyCFunction)PY_PG_device_write_WC, METH_KEYWORDS, PY_PG_device_write_WC_doc},
{"open", (PyCFunction)PY_PG_device_open, METH_KEYWORDS, PY_PG_device_open_doc},
{"set_palette", (PyCFunction)PY_PG_device_set_palette, METH_KEYWORDS, PY_PG_device_set_palette_doc},
{"draw_graph", (PyCFunction)PY_PG_device_draw_graph, METH_KEYWORDS, PY_PG_device_draw_graph_doc},
{"draw_image", (PyCFunction)PY_PG_device_draw_image, METH_KEYWORDS, PY_PG_device_draw_image_doc},
{"contour_plot", (PyCFunction)PY_PG_device_contour_plot, METH_KEYWORDS, PY_PG_device_contour_plot_doc},
{"poly_fill_plot", (PyCFunction)PY_PG_device_poly_fill_plot, METH_KEYWORDS, PY_PG_device_poly_fill_plot_doc},
{"draw_surface", (PyCFunction)PY_PG_device_draw_surface, METH_KEYWORDS, PY_PG_device_draw_surface_doc},
{"draw_picture", (PyCFunction)PY_PG_device_draw_picture, METH_KEYWORDS, PY_PG_device_draw_picture_doc},
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

static int PY_PG_device_tp_init(PY_PG_device *self,
                                PyObject *args, PyObject *kwds)
   {int rv;
    char *name, *type, *title;
    char *kw_list[] = {"name", "type", "title", NULL};

    rv = -1;

    if (PyArg_ParseTupleAndKeywords(args, kwds,
				    "sss:make_device", kw_list,
				    &name, &type, &title))
       {rv = 0;
	self->pyo = PG_make_device(name, type, title);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#undef PY_DEF_DESTRUCTOR
#undef PY_DEF_TP_METH
#undef PY_DEF_TP_PRINT
#undef PY_DEF_TP_CALL
#undef PY_DEF_AS_MAP

#define PY_DEF_DESTRUCTOR           NULL
#define PY_DEF_TP_METH              PY_PG_device_methods
#define PY_DEF_TP_PRINT             NULL
#define PY_DEF_TP_CALL              NULL
#define PY_DEF_AS_MAP               NULL

char
 PY_PG_device_doc[] = "";

PY_DEF_TYPE(PG_device);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
