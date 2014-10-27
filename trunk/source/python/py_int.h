/*
 * PY_INT.H - internal support for Python bindings
 *
 */


#include "cpyright.h"
 
#ifndef PCK_PY_INTERNAL

#define PCK_PY_INTERNAL

#include <Python.h>
#undef HAVE_GETHOSTBYNAME
#include "sx_int.h"
#include "scope_hash.h"
#include "py_gen.h"
#include <pputil.h>

/*--------------------------------------------------------------------------*/

/*                                CONSTANTS                                 */

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

/*                            PROCEDURAL MACROS                             */

/*--------------------------------------------------------------------------*/

/* PY_DEF_TYPE - handle the boiler plate defining a new Python type
 *             - example:
 *             -    PY_DEF_TYPE(PM_mapping, NULL, NULL, NULL, NULL)
 */

#ifndef PY_DEF_DESTRUCTOR
# define PY_DEF_DESTRUCTOR NULL
#endif

#ifndef PY_DEF_REPR
# define PY_DEF_REPR NULL
#endif

#ifndef PY_DEF_AS_MAP
# define PY_DEF_AS_MAP NULL
#endif

#ifndef PY_DEF_TP_METH
# define PY_DEF_TP_METH NULL
#endif

#define PY_DEF_TYPE(_t)                                                      \
PyTypeObject                                                                 \
 PY_ ## _t ## _type = {PY_HEAD_INIT(&PyType_Type, 0)                         \
                  #_t,                                                       \
                  sizeof(PY_ ## _t),                                         \
                  0,                                                         \
                  (destructor) PY_DEF_DESTRUCTOR,                            \
                  (printfunc) 0,                                             \
                  (getattrfunc) 0,                                           \
                  (setattrfunc) 0,                                           \
                  (cmpfunc) 0,                                               \
                  (reprfunc) PY_DEF_REPR,                                    \
                  0,                                                         \
                  0,                                                         \
                  PY_DEF_AS_MAP,                                             \
                  (hashfunc) 0,                                              \
                  (ternaryfunc) 0,                                           \
                  (reprfunc) 0,                                              \
                  (getattrofunc) 0,                                          \
                  (setattrofunc) 0,                                          \
                  0,                                                         \
                  Py_TPFLAGS_DEFAULT,                                        \
                  PY_ ## _t ## _doc,                                         \
                  (traverseproc) 0,                                          \
                  (inquiry) 0,                                               \
                  (richcmpfunc) 0,                                           \
                  0,                                                         \
                  (getiterfunc) 0,                                           \
                  (iternextfunc) 0,                                          \
                  PY_DEF_TP_METH,                                            \
                  0,                                                         \
                  PY_ ## _t ## _getset,                                      \
                  0,                                                         \
                  0,                                                         \
                  (descrgetfunc) 0,                                          \
                  (descrsetfunc) 0,                                          \
                  0,                                                         \
                  (initproc) PY_ ## _t ## _tp_init,                          \
                  (allocfunc) 0,                                             \
                  (newfunc) 0,                                               \
                  (freefunc) 0,                                              \
                  (inquiry) 0,                                               \
                  0,                                                         \
                  0,                                                         \
                  0,                                                         \
                  0,                                                         \
                  0,                                                         \
                  (destructor) 0,}

/*--------------------------------------------------------------------------*/

/*                           STRUCT DEFINITIONS                             */

/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/* PYSCORE.C declarations */

extern char
 PY_set_count_doc[],
 PY_set_io_hooks_doc[],
 PP_setform_doc[],
 PP_print_controls_doc[],
 PP_zero_space_doc[],
 PP_alloc_doc[],
 PP_realloc_doc[],
 PP_cfree_doc[],
 PP_mem_print_doc[],
 PP_mem_trace_doc[],
 PP_reg_mem_doc[],
 PP_dereg_mem_doc[],
 PP_mem_lookup_doc[],
 PP_mem_monitor_doc[],
 PP_mem_chk_doc[],
 PP_is_score_ptr_doc[],
 PP_arrlen_doc[],
 PP_mark_doc[],
 PP_ref_count_doc[],
 PP_permanent_doc[],
 PP_arrtype_doc[],
 PP_pause_doc[];


/* PGSGRAPH.C declarations */

extern char
 PY_set_line_info_doc[],
 PY_set_tds_info_doc[],
 PY_set_tdv_info_doc[];


/* PYPML.C declarations */

extern char
 PP_make_set_1d_doc[],
 PP_make_ac_set_doc[];


/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/* PYPML.C declarations */

extern int
 _PY_mapping_extractor(PyObject *obj, void *arg),
 PY_init_pml_int(PyObject *m, PyObject *d);

extern PyObject
 *PP_make_set_1d(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_make_ac_set(PyObject *self, PyObject *args, PyObject *kwds);


/* PYSCORE.C declarations */

extern PyObject
 *PY_set_count(PyObject *self, PyObject *args, PyObject *kwds),
 *PY_set_io_hooks(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_setform(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_print_controls(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_zero_space(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_alloc(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_realloc(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_cfree(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_mem_print(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_mem_trace(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_reg_mem(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_dereg_mem(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_mem_lookup(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_mem_monitor(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_mem_chk(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_is_score_ptr(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_arrlen(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_mark(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_ref_count(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_permanent(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_arrtype(PyObject *self, PyObject *args, PyObject *kwds),
 *PP_pause(PyObject *self, PyObject *args, PyObject *kwds);


/* PGSGRAPH.C declarations */

extern PyObject
 *PY_set_line_info(PyObject *self, PyObject *args, PyObject *kwds),
 *PY_set_tds_info(PyObject *self, PyObject *args, PyObject *kwds),
 *PY_set_tdv_info(PyObject *self, PyObject *args, PyObject *kwds);


#ifdef __cplusplus
}
#endif

#endif

