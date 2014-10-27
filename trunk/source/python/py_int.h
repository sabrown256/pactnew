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

/* PM_set binding */

typedef struct s_PP_setObject *PP_setObjectp;
typedef struct s_PP_setObject PP_setObject;

struct s_PP_setObject
   {PyObject_HEAD
    PM_set *set;};

extern PyTypeObject
 PP_set_Type;


/* PM_mapping binding */

typedef struct s_PP_mappingObject *PP_mappingObjectp;
typedef struct s_PP_mappingObject PP_mappingObject;

struct s_PP_mappingObject
   {PyObject_HEAD
    PM_mapping *map;};

extern PyTypeObject
 PP_mapping_Type;

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

/*--------------------------------------------------------------------------*/

/*                         FUNCTION DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/* PYPML.C declarations */

extern int
 _PY_mapping_extractor(PyObject *obj, void *arg),
 PY_init_pml_int(PyObject *m, PyObject *d),
 PP_set_Check(PyObject *op),
 PP_mapping_Check(PyObject *op);

extern PyObject
 *_PY_set_from_ptr(PM_set *data),
 *_PY_mapping_from_ptr(PM_mapping *data);

#ifdef __cplusplus
}
#endif

#endif

