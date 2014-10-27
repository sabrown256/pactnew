/*
 * Source Version: 9.0
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */

#define PY_DEF_TP_METH    PY_PG_device_methods

#include "pgsmodule.h"
#include "gp-pgs.h"

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
				    PP_assoc_extractor, &alist))
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
				    _PY_mapping_extractor, &f,
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

static char PY_PG_device_doc[] = "";

PY_DEF_TYPE(PG_device);

#if 0

static char PY_PG_device_type_doc[] = "";

/* static */
PyTypeObject PY_PG_device_type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        "device",                       /* tp_name */
        sizeof(PY_PG_device),         /* tp_basicsize */
        0,                              /* tp_itemsize */
        /* Methods to implement standard operations */
        (destructor)0,                  /* tp_dealloc */
        (printfunc)0,                   /* tp_print */
        (getattrfunc)0,                 /* tp_getattr */
        (setattrfunc)0,                 /* tp_setattr */
        (cmpfunc)0,                     /* tp_compare */
        (reprfunc)0,                    /* tp_repr */
        /* Method suites for standard classes */
        0,                              /* tp_as_number */
        0,                              /* tp_as_sequence */
        0,                              /* tp_as_mapping */
        /* More standard operations (here for binary compatibility) */
        (hashfunc)0,                    /* tp_hash */
        (ternaryfunc)0,                 /* tp_call */
        (reprfunc)0,                    /* tp_str */
        (getattrofunc)0,                /* tp_getattro */
        (setattrofunc)0,                /* tp_setattro */
        /* Functions to access object as input/output buffer */
        0,                              /* tp_as_buffer */
        /* Flags to define presence of optional/expanded features */
        Py_TPFLAGS_DEFAULT,             /* tp_flags */
        PY_PG_device_type_doc,          /* tp_doc */
        /* Assigned meaning in release 2.0 */
        /* call function for all accessible objects */
        (traverseproc)0,                /* tp_traverse */
        /* delete references to contained objects */
        (inquiry)0,                     /* tp_clear */
        /* Assigned meaning in release 2.1 */
        /* rich comparisons */
        (richcmpfunc)0,                 /* tp_richcompare */
        /* weak reference enabler */
        0,                              /* tp_weaklistoffset */
        /* Added in release 2.2 */
        /* Iterators */
        (getiterfunc)0,                 /* tp_iter */
        (iternextfunc)0,                /* tp_iternext */
        /* Attribute descriptor and subclassing stuff */
        PY_PG_device_methods,              /* tp_methods */
        0,                              /* tp_members */
        PY_PG_device_getset,               /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PY_PG_device_tp_init,    /* tp_init */
        (allocfunc)0,                   /* tp_alloc */
        (newfunc)0,                     /* tp_new */
#if PYTHON_API_VERSION >= 1012
        (freefunc)0,                    /* tp_free */
#else
        (destructor)0,                  /* tp_free */
#endif
        (inquiry)0,                     /* tp_is_gc */
        0,                              /* tp_bases */
        0,                              /* tp_mro */
        0,                              /* tp_cache */
        0,                              /* tp_subclasses */
        0,                              /* tp_weaklist */
#if PYTHON_API_VERSION >= 1012
        (destructor)0,                  /* tp_del */
#endif
};

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
