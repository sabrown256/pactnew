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

# define cmpfunc                    void *

# define PY_HEAD_INIT(_t, _s)       PyVarObject_HEAD_INIT(_t, _s)
# define PY_TYPE(_o)                Py_TYPE(_o)
# define PY_TYPE_TYPE               PyType_Type
# define PY_NUMBER_INT              PyNumber_Long

# define PY_INT_OBJECT                  PyLongObject
# define PY_INT_TYPE                    PyLong_Type
# define PY_STRING_TYPE                 PyBytes_Type

# define PY_INT_LONG(_o)                PyLong_FromLong((long) (_o))
# define PY_INT_CHECK(_o)               PyLong_Check(_o)
# define PY_INT_AS_LONG(_o)             PyLong_AsLong(_o)
# define PY_STRING_STRING_SIZE(_o, _s)  PyBytes_FromStringAndSize(_o, _s)
# define PY_STRING_STRING(_o)           PyBytes_FromString(_o)
# define PY_STRING_CHECK(_o)            PyBytes_Check(_o)
# define PY_STRING_SIZE(_o)             PyBytes_Size(_o)
# define PY_STRING_AS_STRING(_o)        PyBytes_AsString(_o)

# define PY_COBJ_VOID_PTR(_o, _m)   PyCapsule_New(_o, 0, (PyCapsule_Destructor) _m)
# define PY_GET_PTR(_o)             PyCapsule_GetPointer(_o, 0)
# define PY_COBJ_TYPE               PyCapsule_Type
# define PY_NUM_INT(_m)             ((_m)->nb_int)

# define PY_MOD_INIT(_nm)           PyMODINIT_FUNC PyInit##_nm(void)
# define PY_MOD_DEF(_o, _nm, _doc, _mth)                                    \
    static struct PyModuleDef moduledef =                                   \
           { PyModuleDef_HEAD_INIT, _nm, _doc, -1, _mth, };                 \
    _o = PyModule_Create(&moduledef)
# define PY_MOD_RETURN_ERR          return(NULL)
# define PY_MOD_RETURN_OK(_x)       return(_x)

# define PY_INIT_BUFFER_PROCS(_rd, _wr, _sc, _gc, _gb, _rb)                 \
    {_gb, _rb}

#else

# define PY_HEAD_INIT(_t, _s)       PyObject_HEAD_INIT(_t) _s,
# define PY_TYPE(_o)                (((PyObject *) (_o))->ob_type)
# define PY_TYPE_TYPE               PyClass_Type
# define PY_NUMBER_INT              PyNumber_Int

# define PY_INT_OBJECT                  PyIntObject
# define PY_INT_TYPE                    PyInt_Type
# define PY_STRING_TYPE                 PyString_Type

# define PY_INT_LONG(_o)                PyInt_FromLong((long) (_o))
# define PY_INT_CHECK(_o)               PyInt_Check(_o)
# define PY_INT_AS_LONG(_o)             PyInt_AsLong(_o)
# define PY_STRING_STRING_SIZE(_o, _s)  PyString_FromStringAndSize(_o, _s)
# define PY_STRING_STRING(_o)           PyString_FromString(_o)
# define PY_STRING_CHECK(_o)            PyString_Check(_o)
# define PY_STRING_SIZE(_o)             PyString_Size(_o)
# define PY_STRING_AS_STRING(_o)        PyString_AsString(_o)

# define PY_COBJ_VOID_PTR(_o, _m)   PyCObject_FromVoidPtr(_o, _m)
# define PY_GET_PTR(_o)             PyCObject_AsVoidPtr(_o)
# define PY_COBJ_TYPE               PyCObject_Type
# define PY_NUM_INT(_m)             ((_m)->nb_long)

# define PY_MOD_INIT(_nm)           void init##_nm(void)
# define PY_MOD_DEF(_o, _nm, _doc, _mth)                                    \
    _o = Py_InitModule4(_nm, _mth, _doc, NULL, PYTHON_API_VERSION)
# define PY_MOD_RETURN_ERR          return
# define PY_MOD_RETURN_OK(_x)       return

# define PY_INIT_BUFFER_PROCS(_rd, _wr, _sc, _gc, _gb, _rb)                 \
    {_rd, _wr, _sc, _gc}

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

