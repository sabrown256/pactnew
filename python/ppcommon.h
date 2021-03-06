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

# define PY_ARG_KEY                 METH_VARARGS | METH_KEYWORDS 


#if PY_MAJOR_VERSION >= 3

# define cmpfunc                    void *

# define PY_HEAD_INIT(_t, _s)       PyVarObject_HEAD_INIT(_t, _s)
# define PY_TYPE(_o)                Py_TYPE(_o)
# define PY_TYPE_TYPE               PyType_Type
# define PY_NUMBER_INT              PyNumber_Long

# define PY_NUM_INT(_m)             ((_m)->nb_int)

# define PY_INT_OBJECT                  PyLongObject
# define PY_INT_TYPE                    PyLong_Type
# define PY_INT_LONG(_o)                PyLong_FromLong((long) (_o))
# define PY_INT_CHECK(_o)               PyLong_Check(_o)
# define PY_INT_AS_LONG(_o)             PyLong_AsLong(_o)

# define PY_STRING_TYPE                 PyUnicode_Type
# define PY_STRING_STRING_SIZE(_o, _s)  PyUnicode_FromStringAndSize(_o, _s)
# define PY_STRING_STRING(_o)           PyUnicode_FromString(_o)
# define PY_STRING_CHECK(_o)            PyUnicode_Check(_o)
# define PY_STRING_SIZE(_o)             PyUnicode_GET_LENGTH(_o)
# define PY_STRING_AS_STRING(_o)        PY_get_string(_o)

# define PY_COBJ_VOID_PTR(_o, _m)   PyCapsule_New(_o, 0, (PyCapsule_Destructor) _m)
# define PY_GET_PTR(_o)             PyCapsule_GetPointer(_o, 0)
# define PY_COBJ_TYPE               PyCapsule_Type

# define PY_MOD_BEGIN(_nm, _doc, _mth)                                      \
PyMODINIT_FUNC PyInit_##_nm(void)                                           \
   {PyObject *m;                                                            \
    static struct PyModuleDef moduledef =                                   \
           { PyModuleDef_HEAD_INIT, #_nm, _doc, -1, _mth, };                \
    m = PyModule_Create(&moduledef);                                        \
    if (m == NULL)                                                          \
       return(NULL);

# define PY_MOD_END(_nm)                                                    \
    if (PyErr_Occurred())                                                   \
       {char msg[MAXLINE];                                                  \
	snprintf(msg, MAXLINE, "can't initialize module %s", #_nm);         \
        Py_FatalError(msg);};                                               \
    return(m);}

# define PY_MOD_RETURN_ERR          return(NULL)

# define PY_INIT_BUFFER_PROCS(_rd, _wr, _sc, _gc, _gb, _rb)                 \
    {_gb, _rb}
#define Py_TPFLAGS_HAVE_NEWBUFFER 0

void PY_init_file_emulator(void);
#define PY_INIT_FILE_EMULATOR  PY_init_file_emulator()

#else

# define PY_HEAD_INIT(_t, _s)       PyObject_HEAD_INIT(_t) _s,
# define PY_TYPE(_o)                (((PyObject *) (_o))->ob_type)
# define PY_TYPE_TYPE               PyClass_Type
# define PY_NUMBER_INT              PyNumber_Int

# define PY_NUM_INT(_m)             ((_m)->nb_long)

# define PY_INT_OBJECT                  PyIntObject
# define PY_INT_TYPE                    PyInt_Type
# define PY_INT_LONG(_o)                PyInt_FromLong((long) (_o))
# define PY_INT_CHECK(_o)               PyInt_Check(_o)
# define PY_INT_AS_LONG(_o)             PyInt_AsLong(_o)

# define PY_STRING_TYPE                 PyString_Type
# define PY_STRING_STRING_SIZE(_o, _s)  PyString_FromStringAndSize(_o, _s)
# define PY_STRING_STRING(_o)           PyString_FromString(_o)
# define PY_STRING_CHECK(_o)            PyString_Check(_o)
# define PY_STRING_SIZE(_o)             PyString_Size(_o)
# define PY_STRING_AS_STRING(_o)        PyString_AsString(_o)

/* needed for python 2.6 and earlier which do not have PyCapsule */
# define PY_COBJ_VOID_PTR(_o, _m)   PyCObject_FromVoidPtr(_o, _m)
# define PY_GET_PTR(_o)             PyCObject_AsVoidPtr(_o)
# define PY_COBJ_TYPE               PyCObject_Type

# define PY_MOD_BEGIN(_nm, _doc, _mth)                                      \
void init##_nm(void)                                                        \
   {PyObject *m;                                                            \
    m = Py_InitModule4(#_nm, _mth, _doc, NULL, PYTHON_API_VERSION);         \
    if (m == NULL)                                                          \
       return;

# define PY_MOD_END(_nm)                                                    \
    if (PyErr_Occurred())                                                   \
       {char msg[MAXLINE];                                                  \
	snprintf(msg, MAXLINE, "can't initialize module %s", #_nm);         \
        Py_FatalError(msg);};                                               \
    return;}

# define PY_MOD_RETURN_ERR          return

# define PY_INIT_BUFFER_PROCS(_rd, _wr, _sc, _gc, _gb, _rb)                 \
    {_rd, _wr, _sc, _gc, _gb, _rb}

#define PY_INIT_FILE_EMULATOR

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

