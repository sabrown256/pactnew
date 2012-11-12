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

# define PY_HEAD_INIT(_t, _s)   PyVarObject_HEAD_INIT(_t, _s)
# define PY_TYPE(_o)            Py_TYPE(_o)

#else

# define PY_HEAD_INIT(_t, _s)   PyObject_HEAD_INIT(_t)
# define PY_TYPE(_o)            (((PyObject *) (_o))->ob_type)

#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif

