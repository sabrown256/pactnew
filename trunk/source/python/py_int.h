/*
 * PY_INT.H - internal support for Python bindings
 *
 */


#include "cpyright.h"
 
#ifndef PCK_PY_INTERNAL

#define PCK_PY_INTERNAL

#include <Python.h>
#undef HAVE_GETHOSTBYNAME
#include "pgs_int.h"
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
 PY_init_pml(PyObject *m, PyObject *d),
 PP_set_Check(PyObject *op),
 PP_mapping_Check(PyObject *op);

extern PyObject
 *_PY_set_from_ptr(PM_set *data),
 *_PY_mapping_from_ptr(PM_mapping *data);

#ifdef __cplusplus
}
#endif

#endif

