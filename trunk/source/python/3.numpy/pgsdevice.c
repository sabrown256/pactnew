/*--------------------------------------------------------------------------*/

/* This is generated code.
 * Any edits must be made between the splicer.begin and splicer.end blocks.
 * All other edits will be lost.
 * Once a block is edited remove the 'UNMODIFIED' on the splicer.begin comment
 * to allow the block to be preserved when it is regenerated.
 */
/*
 * Source Version: 9.0
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */
/*--------------------------------------------------------------------------*/
#include "pgsmodule.h"
#include "py-pgs.h"

/* DO-NOT-DELETE splicer.begin(pgs.device.C_definition) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.device.C_definition) */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_set_font__doc__[] = 
""
;

static PyObject *
PP_device_set_font(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_font) */
    char *face;
    char *style;
    int size;
    char *kw_list[] = {"face", "style", "size", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ssi:set_font", kw_list,
                                     &face, &style, &size))
        return NULL;
#if 0
    int result;
    result = PG_fset_font(self->dev, face, style, size);
    return PY_INT_LONG(result);
#else
    if ((self->dev) != NULL)                                                         \
       {if ((self->dev)->set_font != NULL)                                           \
           (*(self->dev)->set_font)(self->dev, face, style, size);}
    Py_INCREF(Py_None);
    return Py_None;
#endif
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_font) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_set_viewport__doc__[] = 
""
;

static PyObject *
PP_device_set_viewport(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_viewport) */
    double ndc[PG_BOXSZ];
    char *kw_list[] = {"x1", "x2", "y1", "y2", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddd:set_viewport", kw_list,
                                     &ndc[0], &ndc[1], &ndc[2], &ndc[3]))
        return NULL;

    PG_set_viewspace(self->dev, 2, NORMC, ndc);

    Py_INCREF(Py_None);

    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_viewport) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_set_window__doc__[] = 
""
;

static PyObject *
PP_device_set_window(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_window) */
    double wc[PG_BOXSZ];
    char *kw_list[] = {"xmn", "xmx", "ymn", "ymx", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddd:set_window", kw_list,
                                     &wc[0], &wc[1], &wc[2], &wc[3]))
        return NULL;

    PG_set_viewspace(self->dev, 2, WORLDC, wc);

    Py_INCREF(Py_None);

    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_window) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_draw_box__doc__[] = 
""
;

static PyObject *
PP_device_draw_box(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.draw_box) */
    double bx[PG_BOXSZ];
    char *kw_list[] = {"xmin", "xmax", "ymin", "ymax", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddd:draw_box", kw_list,
                                     &bx[0], &bx[1], &bx[2], &bx[3]))
        return NULL;

    PG_draw_box_n(self->dev, 2, WORLDC, bx);

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.draw_box) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_get_text_ext__doc__[] = 
""
;

static PyObject *
PP_device_get_text_ext(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.get_text_ext) */
    char *s;
    double x[PG_SPACEDM];
    char *kw_list[] = {"s", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:get_text_ext", kw_list,
                                     &s))
        return NULL;

    PG_get_text_ext_n(self->dev, 2, WORLDC, s, x);

    return Py_BuildValue("ff", x[0], x[1]);
/* DO-NOT-DELETE splicer.end(pgs.device.method.get_text_ext) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_draw_line__doc__[] = 
""
;

static PyObject *
PP_device_draw_line(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.draw_line) */
    double x1[PG_SPACEDM];
    double x2[PG_SPACEDM];
    char *kw_list[] = {"x1", "y1", "x2", "y2", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddd:draw_line", kw_list,
                                     &x1[0], &x1[1], &x2[0], &x2[0]))
        return NULL;

    PG_draw_line_n(self->dev, 2, WORLDC, x1, x2, self->dev->clipping);

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.draw_line) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_write_WC__doc__[] = 
""
;

static PyObject *
PP_device_write_WC(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.write_WC) */
    double x;
    double y;
    double p[PG_SPACEDM];
    char *fmt;
    char *arg;
    char *kw_list[] = {"x", "y", "fmt", "arg", NULL};
    int result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ddss:write_WC", kw_list,
                                     &x, &y, &fmt, &arg))
        return NULL;
    /* XXX - This is expecting a variable number of arguments */
    p[0] = x;
    p[1] = y;
    result = PG_write_n(self->dev, 2, WORLDC, p, fmt, arg);

    return PY_INT_LONG(result);
/* DO-NOT-DELETE splicer.end(pgs.device.method.write_WC) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_draw_graph__doc__[] = 
""
;

static PyObject *
PP_device_draw_graph(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.draw_graph) UNMODIFIED */
    PG_graph *data;
    PP_graphObject *dataobj;
    char *kw_list[] = {"data", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:draw_graph", kw_list,
                                     &PP_graph_Type, &dataobj))
        return NULL;
    data = dataobj->data;
    PG_draw_graph(self->dev, data);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.draw_graph) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_draw_image__doc__[] = 
""
;

static PyObject *
PP_device_draw_image(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.draw_image) UNMODIFIED */
    PG_image *im;
    char *label;
    void *alist;
    PP_imageObject *imobj;
    char *kw_list[] = {"im", "label", "alist", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!sz:draw_image", kw_list,
                                     &PP_image_Type, &imobj, &label, &alist))
        return NULL;
    im = imobj->im;
    PG_draw_image(self->dev, im, label, alist);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.draw_image) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_contour_plot__doc__[] = 
""
;

static PyObject *
PP_device_contour_plot(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.contour_plot) UNMODIFIED */
    PG_graph *data;
    PP_graphObject *dataobj;
    char *kw_list[] = {"data", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:contour_plot", kw_list,
                                     &PP_graph_Type, &dataobj))
        return NULL;
    data = dataobj->data;
    PG_contour_plot(self->dev, data);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.contour_plot) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_poly_fill_plot__doc__[] = 
""
;

static PyObject *
PP_device_poly_fill_plot(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.poly_fill_plot) UNMODIFIED */
    PG_graph *data;
    PP_graphObject *dataobj;
    char *kw_list[] = {"data", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!:poly_fill_plot", kw_list,
                                     &PP_graph_Type, &dataobj))
        return NULL;
    data = dataobj->data;
    PG_poly_fill_plot(self->dev, data);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.poly_fill_plot) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_draw_surface__doc__[] = 
""
;

static PyObject *
PP_device_draw_surface(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.draw_surface) */
    double *a1;
    double *a2;
    double *extr;
    double *x;
    double *y;
    double *r[PG_SPACEDM];
    int nn;
    double xmn;
    double xmx;
    double ymn;
    double ymx;
    double theta;
    double phi;
    double chi;
    double width;
    double va[3];
    int color;
    int style;
    int type;
    char *name;
    char *mesh_type;
    int *cnnct;
    pcons *alist;
    char *kw_list[] = {"a1", "a2", "extr", "x", "y", "nn", "xmn", "xmx", "ymn", "ymx", "theta", "phi", "chi", "width", "color", "style", "type", "name", "mesh_type", "cnnct", "alist", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&O&O&O&O&iddddddddiiissO&O&:draw_surface", kw_list,
                                     REAL_array_extractor, &a1, REAL_array_extractor, &a2, REAL_array_extractor, &extr, REAL_array_extractor, &x, REAL_array_extractor, &y, &nn, &xmn, &xmx, &ymn, &ymx, &theta, &phi, &chi, &width, &color, &style, &type, &name, &mesh_type, iarray_extractor, &cnnct, PP_assoc_extractor, &alist))
        return NULL;

    r[0] = x;
    r[1] = y;

    va[0] = theta;
    va[1] = phi;
    va[2] = chi;

    PG_draw_surface_n(self->dev, a1, a2, extr, r, nn, va, width,
		      color, style, type, name, mesh_type, cnnct, alist);

    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.draw_surface) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_draw_picture__doc__[] = 
""
;

static PyObject *
PP_device_draw_picture(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.draw_picture) UNMODIFIED */
    PM_mapping *f;
    int ptyp;
    int bndp;
    int cbnd;
    int sbnd;
    double wbnd;
    int mshp;
    int cmsh;
    int smsh;
    double wmsh;
    char *kw_list[] = {"f", "ptyp", "bndp", "cbnd", "sbnd", "wbnd", "mshp", "cmsh", "smsh", "wmsh", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O&iiiidiiid:draw_picture", kw_list,
                                     mapping_extractor, &f, &ptyp, &bndp, &cbnd, &sbnd, &wbnd, &mshp, &cmsh, &smsh, &wmsh))
        return NULL;
    PG_draw_picture(self->dev, f, ptyp, bndp, cbnd, sbnd, wbnd, mshp, cmsh, smsh, wmsh);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.draw_picture) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pgs.device.extra_methods) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.device.extra_methods) */

/*--------------------------------------------------------------------------*/

#if 1

/*--------------------------------------------------------------------------*/

static PyMethodDef PP_device_methods[] = {
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
{"set_font", (PyCFunction)PP_device_set_font, METH_KEYWORDS, PP_device_set_font__doc__},
{"set_viewport", (PyCFunction)PP_device_set_viewport, METH_KEYWORDS, PP_device_set_viewport__doc__},
{"set_window", (PyCFunction)PP_device_set_window, METH_KEYWORDS, PP_device_set_window__doc__},
{"draw_box", (PyCFunction)PP_device_draw_box, METH_KEYWORDS, PP_device_draw_box__doc__},
{"get_text_ext", (PyCFunction)PP_device_get_text_ext, METH_KEYWORDS, PP_device_get_text_ext__doc__},
{"draw_line", (PyCFunction)PP_device_draw_line, METH_KEYWORDS, PP_device_draw_line__doc__},
{"write_WC", (PyCFunction)PP_device_write_WC, METH_KEYWORDS, PP_device_write_WC__doc__},
{"draw_graph", (PyCFunction)PP_device_draw_graph, METH_KEYWORDS, PP_device_draw_graph__doc__},
{"draw_image", (PyCFunction)PP_device_draw_image, METH_KEYWORDS, PP_device_draw_image__doc__},
{"contour_plot", (PyCFunction)PP_device_contour_plot, METH_KEYWORDS, PP_device_contour_plot__doc__},
{"poly_fill_plot", (PyCFunction)PP_device_poly_fill_plot, METH_KEYWORDS, PP_device_poly_fill_plot__doc__},
{"draw_surface", (PyCFunction)PP_device_draw_surface, METH_KEYWORDS, PP_device_draw_surface__doc__},
{"draw_picture", (PyCFunction)PP_device_draw_picture, METH_KEYWORDS, PP_device_draw_picture__doc__},
/* DO-NOT-DELETE splicer.begin(pgs.device.extra_mlist) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.device.extra_mlist) */
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/

#else

/*--------------------------------------------------------------------------*/


static char PP_device_set_char_path__doc__[] = 
""
;

static PyObject *
PP_device_set_char_path(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_char_path) UNMODIFIED */
    double x[PG_SPACEDM];
    char *kw_list[] = {"x", "y", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dd:set_char_path", kw_list,
                                     &x[0], &x[1]))
        return NULL;
    PG_fset_char_path(self->dev, x);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_char_path) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_set_color_line__doc__[] = 
""
;

static PyObject *
PP_device_set_color_line(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_color_line) UNMODIFIED */
    int color;
    int mapped;
    char *kw_list[] = {"color", "mapped", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ii:set_color_line", kw_list,
                                     &color, &mapped))
        return NULL;
    PG_fset_line_color(self->dev, color, mapped);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_color_line) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_set_line_color__doc__[] = 
""
;

static PyObject *
PP_device_set_line_color(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_line_color) UNMODIFIED */
    int color;
    char *kw_list[] = {"color", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:set_line_color", kw_list,
                                     &color))
        return NULL;
    PG_fset_line_color(self->dev, color, TRUE);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_line_color) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_set_line_style__doc__[] = 
""
;

static PyObject *
PP_device_set_line_style(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_line_style) UNMODIFIED */
    int style;
    char *kw_list[] = {"style", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:set_line_style", kw_list,
                                     &style))
        return NULL;
    PG_fset_line_style(self->dev, style);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_line_style) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_set_line_width__doc__[] = 
""
;

static PyObject *
PP_device_set_line_width(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_line_width) UNMODIFIED */
    double width;
    char *kw_list[] = {"width", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "d:set_line_width", kw_list,
                                     &width))
        return NULL;
    PG_fset_line_width(self->dev, width);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_line_width) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_set_text_color__doc__[] = 
""
;

static PyObject *
PP_device_set_text_color(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_text_color) UNMODIFIED */
    int color;
    char *kw_list[] = {"color", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:set_text_color", kw_list,
                                     &color))
        return NULL;
    PG_fset_text_color(self->dev, color, TRUE);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_text_color) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_set_color_text__doc__[] = 
""
;

static PyObject *
PP_device_set_color_text(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_color_text) UNMODIFIED */
    int color;
    int mapped;
    char *kw_list[] = {"color", "mapped", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ii:set_color_text", kw_list,
                                     &color, &mapped))
        return NULL;
    PG_fset_text_color(self->dev, color, mapped);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_color_text) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_turn_data_id__doc__[] = 
""
;

static PyObject *
PP_device_turn_data_id(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.turn_data_id) UNMODIFIED */
    int t;
    char *kw_list[] = {"t", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i:turn_data_id", kw_list,
                                     &t))
        return NULL;
    PG_turn_data_id(self->dev, t);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.turn_data_id) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_clear_window__doc__[] = 
""
;

static PyObject *
PP_device_clear_window(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.clear_window) UNMODIFIED */
    PG_clear_window(self->dev);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.clear_window) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_close__doc__[] = 
""
;

static PyObject *
PP_device_close(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.close) UNMODIFIED */
    PG_close_device(self->dev);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.close) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_finish_plot__doc__[] = 
""
;

static PyObject *
PP_device_finish_plot(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.finish_plot) UNMODIFIED */
    PG_finish_plot(self->dev);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.finish_plot) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_update_vs__doc__[] = 
""
;

static PyObject *
PP_device_update_vs(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.update_vs) UNMODIFIED */
    PG_update_vs(self->dev);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.update_vs) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_open__doc__[] = 
""
;

static PyObject *
PP_device_open(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.open) UNMODIFIED */
    double xf;
    double yf;
    double dxf;
    double dyf;
    char *kw_list[] = {"xf", "yf", "dxf", "dyf", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "dddd:open", kw_list,
                                     &xf, &yf, &dxf, &dyf))
        return NULL;
    PG_open_device(self->dev, xf, yf, dxf, dyf);
    Py_INCREF(Py_None);
    return Py_None;
/* DO-NOT-DELETE splicer.end(pgs.device.method.open) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


static char PP_device_set_palette__doc__[] = 
""
;

static PyObject *
PP_device_set_palette(PP_deviceObject *self,
                  PyObject *args,
                  PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.method.set_palette) UNMODIFIED */
    char *name;
    char *kw_list[] = {"name", NULL};
    PG_palette *result;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s:set_palette", kw_list,
                                     &name))
        return NULL;
    result = PG_fset_palette(self->dev, name);
    return PPpalette_from_ptr(result);
/* DO-NOT-DELETE splicer.end(pgs.device.method.set_palette) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMethodDef PP_device_methods[] = {
{"clear_window", (PyCFunction)PP_device_clear_window, METH_NOARGS, PP_device_clear_window__doc__},
{"close", (PyCFunction)PP_device_close, METH_NOARGS, PP_device_close__doc__},
{"finish_plot", (PyCFunction)PP_device_finish_plot, METH_NOARGS, PP_device_finish_plot__doc__},
{"set_char_path", (PyCFunction)PP_device_set_char_path, METH_KEYWORDS, PP_device_set_char_path__doc__},
{"set_font", (PyCFunction)PP_device_set_font, METH_KEYWORDS, PP_device_set_font__doc__},
{"set_color_line", (PyCFunction)PP_device_set_color_line, METH_KEYWORDS, PP_device_set_color_line__doc__},
{"set_line_style", (PyCFunction)PP_device_set_line_style, METH_KEYWORDS, PP_device_set_line_style__doc__},
{"set_line_width", (PyCFunction)PP_device_set_line_width, METH_KEYWORDS, PP_device_set_line_width__doc__},
{"update_vs", (PyCFunction)PP_device_update_vs, METH_NOARGS, PP_device_update_vs__doc__},
{"set_line_color", (PyCFunction)PP_device_set_line_color, METH_KEYWORDS, PP_device_set_line_color__doc__},
{"set_text_color", (PyCFunction)PP_device_set_text_color, METH_KEYWORDS, PP_device_set_text_color__doc__},
{"set_color_text", (PyCFunction)PP_device_set_color_text, METH_KEYWORDS, PP_device_set_color_text__doc__},
{"turn_data_id", (PyCFunction)PP_device_turn_data_id, METH_KEYWORDS, PP_device_turn_data_id__doc__},
{"set_viewport", (PyCFunction)PP_device_set_viewport, METH_KEYWORDS, PP_device_set_viewport__doc__},
{"set_window", (PyCFunction)PP_device_set_window, METH_KEYWORDS, PP_device_set_window__doc__},
{"draw_box", (PyCFunction)PP_device_draw_box, METH_KEYWORDS, PP_device_draw_box__doc__},
{"get_text_ext", (PyCFunction)PP_device_get_text_ext, METH_KEYWORDS, PP_device_get_text_ext__doc__},
{"draw_line", (PyCFunction)PP_device_draw_line, METH_KEYWORDS, PP_device_draw_line__doc__},
{"write_WC", (PyCFunction)PP_device_write_WC, METH_KEYWORDS, PP_device_write_WC__doc__},
{"open", (PyCFunction)PP_device_open, METH_KEYWORDS, PP_device_open__doc__},
{"set_palette", (PyCFunction)PP_device_set_palette, METH_KEYWORDS, PP_device_set_palette__doc__},
{"draw_graph", (PyCFunction)PP_device_draw_graph, METH_KEYWORDS, PP_device_draw_graph__doc__},
{"draw_image", (PyCFunction)PP_device_draw_image, METH_KEYWORDS, PP_device_draw_image__doc__},
{"contour_plot", (PyCFunction)PP_device_contour_plot, METH_KEYWORDS, PP_device_contour_plot__doc__},
{"poly_fill_plot", (PyCFunction)PP_device_poly_fill_plot, METH_KEYWORDS, PP_device_poly_fill_plot__doc__},
{"draw_surface", (PyCFunction)PP_device_draw_surface, METH_KEYWORDS, PP_device_draw_surface__doc__},
{"draw_picture", (PyCFunction)PP_device_draw_picture, METH_KEYWORDS, PP_device_draw_picture__doc__},
/* DO-NOT-DELETE splicer.begin(pgs.device.extra_mlist) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.device.extra_mlist) */
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

static char PP_device_dev__doc__[] =
""
;

static PyObject *
PP_device_dev_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev_get) UNMODIFIED */
    return PY_COBJ_VOID_PTR((void *) self->dev, NULL);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_name__doc__[] =
""
;

static PyObject *
PP_device_dev_name_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.name_get) UNMODIFIED */
    return Py_BuildValue("s", self->dev->name);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.name_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_title__doc__[] =
""
;

static PyObject *
PP_device_dev_title_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.title_get) UNMODIFIED */
    return Py_BuildValue("s", self->dev->title);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.title_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_type__doc__[] =
""
;

static PyObject *
PP_device_dev_type_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.type_get) UNMODIFIED */
    return Py_BuildValue("s", self->dev->type);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.type_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_BLACK__doc__[] =
""
;

static PyObject *
PP_device_dev_BLACK_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.BLACK_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->BLACK);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.BLACK_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_WHITE__doc__[] =
""
;

static PyObject *
PP_device_dev_WHITE_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.WHITE_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->WHITE);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.WHITE_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_GRAY__doc__[] =
""
;

static PyObject *
PP_device_dev_GRAY_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.GRAY_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->GRAY);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.GRAY_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_DARK_GRAY__doc__[] =
""
;

static PyObject *
PP_device_dev_DARK_GRAY_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.DARK_GRAY_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->DARK_GRAY);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.DARK_GRAY_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_BLUE__doc__[] =
""
;

static PyObject *
PP_device_dev_BLUE_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.BLUE_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->BLUE);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.BLUE_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_GREEN__doc__[] =
""
;

static PyObject *
PP_device_dev_GREEN_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.GREEN_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->GREEN);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.GREEN_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_CYAN__doc__[] =
""
;

static PyObject *
PP_device_dev_CYAN_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.CYAN_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->CYAN);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.CYAN_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_RED__doc__[] =
""
;

static PyObject *
PP_device_dev_RED_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.RED_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->RED);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.RED_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_MAGENTA__doc__[] =
""
;

static PyObject *
PP_device_dev_MAGENTA_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.MAGENTA_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->MAGENTA);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.MAGENTA_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_BROWN__doc__[] =
""
;

static PyObject *
PP_device_dev_BROWN_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.BROWN_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->BROWN);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.BROWN_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_DARK_BLUE__doc__[] =
""
;

static PyObject *
PP_device_dev_DARK_BLUE_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.DARK_BLUE_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->DARK_BLUE);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.DARK_BLUE_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_DARK_GREEN__doc__[] =
""
;

static PyObject *
PP_device_dev_DARK_GREEN_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.DARK_GREEN_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->DARK_GREEN);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.DARK_GREEN_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_DARK_CYAN__doc__[] =
""
;

static PyObject *
PP_device_dev_DARK_CYAN_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.DARK_CYAN_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->DARK_CYAN);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.DARK_CYAN_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_DARK_RED__doc__[] =
""
;

static PyObject *
PP_device_dev_DARK_RED_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.DARK_RED_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->DARK_RED);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.DARK_RED_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_YELLOW__doc__[] =
""
;

static PyObject *
PP_device_dev_YELLOW_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.YELLOW_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->YELLOW);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.YELLOW_get) */
}

/*--------------------------------------------------------------------------*/

static char PP_device_dev_DARK_MAGENTA__doc__[] =
""
;

static PyObject *
PP_device_dev_DARK_MAGENTA_get(PP_deviceObject *self, void *context)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.descriptor.dev.DARK_MAGENTA_get) UNMODIFIED */
    return PY_INT_LONG(self->dev->DARK_MAGENTA);
/* DO-NOT-DELETE splicer.end(pgs.device.descriptor.dev.DARK_MAGENTA_get) */
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* DO-NOT-DELETE splicer.begin(pgs.device.extra_members) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.device.extra_members) */

static PyGetSetDef PP_device_getset[] = {
    {"dev", (getter) PP_device_dev_get, NULL, PP_device_dev__doc__, NULL},
    {"name", (getter) PP_device_dev_name_get, NULL, PP_device_dev_name__doc__, NULL},
    {"title", (getter) PP_device_dev_title_get, NULL, PP_device_dev_title__doc__, NULL},
    {"type", (getter) PP_device_dev_type_get, NULL, PP_device_dev_type__doc__, NULL},
    {"BLACK", (getter) PP_device_dev_BLACK_get, NULL, PP_device_dev_BLACK__doc__, NULL},
    {"WHITE", (getter) PP_device_dev_WHITE_get, NULL, PP_device_dev_WHITE__doc__, NULL},
    {"GRAY", (getter) PP_device_dev_GRAY_get, NULL, PP_device_dev_GRAY__doc__, NULL},
    {"DARK_GRAY", (getter) PP_device_dev_DARK_GRAY_get, NULL, PP_device_dev_DARK_GRAY__doc__, NULL},
    {"BLUE", (getter) PP_device_dev_BLUE_get, NULL, PP_device_dev_BLUE__doc__, NULL},
    {"GREEN", (getter) PP_device_dev_GREEN_get, NULL, PP_device_dev_GREEN__doc__, NULL},
    {"CYAN", (getter) PP_device_dev_CYAN_get, NULL, PP_device_dev_CYAN__doc__, NULL},
    {"RED", (getter) PP_device_dev_RED_get, NULL, PP_device_dev_RED__doc__, NULL},
    {"MAGENTA", (getter) PP_device_dev_MAGENTA_get, NULL, PP_device_dev_MAGENTA__doc__, NULL},
    {"BROWN", (getter) PP_device_dev_BROWN_get, NULL, PP_device_dev_BROWN__doc__, NULL},
    {"DARK_BLUE", (getter) PP_device_dev_DARK_BLUE_get, NULL, PP_device_dev_DARK_BLUE__doc__, NULL},
    {"DARK_GREEN", (getter) PP_device_dev_DARK_GREEN_get, NULL, PP_device_dev_DARK_GREEN__doc__, NULL},
    {"DARK_CYAN", (getter) PP_device_dev_DARK_CYAN_get, NULL, PP_device_dev_DARK_CYAN__doc__, NULL},
    {"DARK_RED", (getter) PP_device_dev_DARK_RED_get, NULL, PP_device_dev_DARK_RED__doc__, NULL},
    {"YELLOW", (getter) PP_device_dev_YELLOW_get, NULL, PP_device_dev_YELLOW__doc__, NULL},
    {"DARK_MAGENTA", (getter) PP_device_dev_DARK_MAGENTA_get, NULL, PP_device_dev_DARK_MAGENTA__doc__, NULL},
/* DO-NOT-DELETE splicer.begin(pgs.device.extra_getset) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.device.extra_getset) */
    {NULL}     /* Sentinel */
};

/*--------------------------------------------------------------------------*/

static int
PP_device_tp_init(PP_deviceObject *self, PyObject *args, PyObject *kwds)
{
/* DO-NOT-DELETE splicer.begin(pgs.device.as_type.init) */
    char *name;
    char *type;
    char *title;
    char *kw_list[] = {"name", "type", "title", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sss:make_device", kw_list,
                                     &name, &type, &title))
        return -1;
    self->dev = PG_make_device(name, type, title);
    return 0;
/* DO-NOT-DELETE splicer.end(pgs.device.as_type.init) */
}
/*--------------------------------------------------------------------------*/

/* PP_device_Check - */

/* static */ int
PP_device_Check(PyObject *op)
{
    if (PyObject_TypeCheck(op, &PP_device_Type))
        return 1;
    else
        return 0;
/*  return PY_TYPE(op) == &PP_device_Type; */
}

/*--------------------------------------------------------------------------*/


static char PP_device_Type__doc__[] = 
""
;

/* static */
PyTypeObject PP_device_Type = {
        PY_HEAD_INIT(&PyType_Type, 0)
        0,                              /* ob_size */
        "device",                       /* tp_name */
        sizeof(PP_deviceObject),         /* tp_basicsize */
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
        PP_device_Type__doc__,          /* tp_doc */
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
        PP_device_methods,              /* tp_methods */
        0,                              /* tp_members */
        PP_device_getset,               /* tp_getset */
        0,                              /* tp_base */
        0,                              /* tp_dict */
        (descrgetfunc)0,                /* tp_descr_get */
        (descrsetfunc)0,                /* tp_descr_set */
        0,                              /* tp_dictoffset */
        (initproc)PP_device_tp_init,    /* tp_init */
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

/* DO-NOT-DELETE splicer.begin(pgs.device.extra) UNMODIFIED */
/* DO-NOT-DELETE splicer.end(pgs.device.extra) */
/* End of code for device objects */
/*--------------------------------------------------------------------------*/
/*                               OBJECT_TAIL                                */
/*--------------------------------------------------------------------------*/
