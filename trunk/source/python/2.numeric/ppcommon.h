/*
 * PPCOMMON.H - header supporting multiple Python versions
 *
 */

#include "cpyright.h"

#ifndef PCK_PY_COMMON

#define PCK_PY_COMMON

/*--------------------------------------------------------------------------*/

/*                                   MACROS                                 */

/*--------------------------------------------------------------------------*/

#if PY_MAJOR_VERSION >= 3

# define PY_HEAD_INIT(_t, _s)       PyVarObject_HEAD_INIT(_t, _s)
# define PY_TYPE(_o)                Py_TYPE(_o)
# define PY_INT_LONG(_o)            PyLong_FromLong((long) (_o))
# define PY_STRING_STRING(_o)       PyBytes_FromString(_o)


# define PY_COBJ_VOID_PTR(_o, _m)   PyCapsule_New(_o, 0, (PyCapsule_Destructor) _m)
# define PY_GET_PTR(_o)             PyCapsule_GetPointer(_o, 0)
# define PY_COBJ_TYPE               PyCapsule_Type

/*
# define MOD_ERROR_VAL NULL
# define MOD_SUCCESS_VAL(val) val
# define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
# define MOD_DEF(ob, name, doc, methods)                         \
    static struct PyModuleDef moduledef =                        \
           { PyModuleDef_HEAD_INIT, name, doc, -1, methods, };   \
    ob = PyModule_Create(&moduledef);
*/

#else

# define PY_HEAD_INIT(_t, _s)       PyObject_HEAD_INIT(_t)
# define PY_TYPE(_o)                (((PyObject *) (_o))->ob_type)
# define PY_INT_LONG(_o)            PyInt_FromLong((long) (_o))
# define PY_STRING_STRING(_o)       PyString_FromString(_o)

# define PY_COBJ_VOID_PTR(_o, _m)   PyCObject_FromVoidPtr(_o, _m)
# define PY_GET_PTR(_o)             PyCObject_AsVoidPtr(_o)
# define PY_COBJ_TYPE               PyCObject_Type

/*
# define MOD_ERROR_VAL
# define MOD_SUCCESS_VAL(val)
# define MOD_INIT(name) void init##name(void)
# define MOD_DEF(ob, name, doc, methods)                         \
    ob = Py_InitModule3(name, methods, doc);
*/
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

