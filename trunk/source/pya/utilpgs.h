/*
 * UTILPGS.H
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"


int PP_buffer_extractor(PyObject *obj, void *arg);
int palette_extractor(PyObject *obj, void *arg);

PyObject *PPgraph_from_ptr(PG_graph *data);
PyObject *PPpalette_from_ptr(PG_palette *pal);
PyObject *PPimage_from_ptr(PG_image *im);
