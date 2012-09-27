/*
 * PPARRAYS.C
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"

#define PY_ARRAY_UNIQUE_SYMBOL PP_

#ifdef HAVE_PY_NUMERIC
# include <Numeric/arrayobject.h>
#endif

struct s_PP_numpy_map {
    int typecode;
    int type_num;
    char *name;
};

typedef struct s_PP_numpy_map PP_numpy_map;

static PP_numpy_map *tc_to_entry[PP_NUM_TYPES];

static hasharr *_numpy_map;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int darray_extractor(PyObject *obj, void *arg)
{

#ifdef HAVE_PY_NUMERIC
    PyArrayObject *arr;
    
    /* create an array object */
    arr = (PyArrayObject *) PyArray_ContiguousFromObject(obj, 'd', 0, 0);
    if (arr == NULL)
        return 0;

    *(double **) arg = (double *) arr->data;
#endif
    
    return 1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int iarray_extractor(PyObject *obj, void *arg)
{

#ifdef HAVE_PY_NUMERIC
    PyArrayObject *arr;
    
    /* create an array object */
    arr = (PyArrayObject *) PyArray_ContiguousFromObject(obj, 'i', 0, 0);
    if (arr == NULL)
        return 0;

    *(int **) arg = (int *) arr->data;
#endif
    
    return 1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* pplong_extractor - tries to create a long ** variable from obj.
 */

int pplong_extractor(PyObject *obj, void *arg)
{

#ifdef HAVE_PY_NUMERIC
    long **buf;
    Py_ssize_t nd, i;
    PyObject *subobj;
    PyArrayObject *arr;
    
    if (obj == Py_None) {
        *(void **) arg = NULL;
        return 1;
    }

    if (PySequence_Check(obj) == 0) {
        /* must be sequence */
        return 0;
    }

    nd = PySequence_Size(obj);
    buf = CMAKE_N(long *, nd);

    for (i = 0; i < nd; i++) {
        subobj = PySequence_GetItem(obj, i);
        if (subobj == Py_None) {
            buf[i] = NULL;
        } else {
            /* create an array object */
            arr = (PyArrayObject *) PyArray_ContiguousFromObject(subobj, 'l', 0, 0);
            if (arr == NULL)
                return 0;
            buf[i] = (long *) arr->data;
        }
        Py_DECREF(subobj);
    }
    *(long ***) arg = buf;
#endif    
    
    return 1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int REAL_array_extractor(PyObject *obj, void *arg)
{

#ifdef HAVE_PY_NUMERIC
    PyArrayObject *arr;
    char t;

    t = 'd';
    
    /* create an array object */
    arr = (PyArrayObject *) PyArray_ContiguousFromObject(obj, t, 0, 0);
    if (arr == NULL)
        return 0;

    *(double **) arg = (double *) arr->data;
#endif
    
    return 1;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PP_numpy_map *PP_lookup_type_num(char *name)
{
    PP_numpy_map *entry;
    
    entry = (PP_numpy_map *) SC_hasharr_def_lookup(_numpy_map, name);

    return entry;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int array_pack(void *vr, PyObject *obj, long nitems, PP_types tc)
{

#ifdef HAVE_PY_NUMERIC
    PyArrayObject *arr;
    PP_numpy_map *entry;

    if (tc > 0 && tc < PP_NUM_TYPES) {
        entry = tc_to_entry[tc];
    } else {
        entry = NULL;
    }
    if (entry == NULL) {
        /* XXX set error */
        return -1;
    }

    arr = (PyArrayObject *)
        PyArray_ContiguousFromObject(obj, entry->type_num, 0, 0);
    if (arr == NULL)
        return -1;

    memcpy(vr, arr->data, nitems * arr->descr->elsize);

    Py_DECREF(arr);
#endif

    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *PP_array_unpack(void *vr, long nitems, PP_types tc)
{
    int ndims, dimensions[MAXDIM];
    PP_numpy_map *entry;
    PyObject *rv;

    if (tc > 0 && tc < PP_NUM_TYPES)
       entry = tc_to_entry[tc];
    else
       entry = NULL;

    rv = NULL;

    if (entry != NULL)
       {ndims = 1;
	dimensions[0] = nitems;

#ifdef HAVE_PY_NUMERIC
	rv = PyArray_FromDimsAndData(ndims, dimensions, entry->type_num, vr);
#endif
       };

    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_init_numpy -
 */

void _PP_init_numpy(void)
{

#ifdef HAVE_PY_NUMERIC
    PP_numpy_map *nentry;
    static char *XX_FOO_MAP;
    static PP_numpy_map
           entries[] = {{PP_CHAR_I,     PyArray_CHAR,    "char"},
                        {PP_UBYTE_I,    PyArray_UBYTE,   "u_char"},
/*                                      PyArray_SBYTE,                 */
                        {PP_SHORT_I,    PyArray_SHORT,   "short"},
/*                        {PP__I,         PyArray_USHORT,  "u_short"},   */
                        {PP_INT_I,      PyArray_INT,     "int"},
/*                        {PP__I,         PyArray_UINT,    "u_int"},     */
                        {PP_LONG_I,     PyArray_LONG,    "long"},
                        {PP_FLOAT_I,    PyArray_FLOAT,   "float"},
                        {PP_DOUBLE_I,   PyArray_DOUBLE,  "double"},
/*                        {PP__I,         PyArray_CFLOAT,  ""},          */
/*                        {PP__I,         PyArray_CDOUBLE, ""},          */
                        {0,             0,               NULL}};

    XX_FOO_MAP = CSTRSAVE("PP_numpy_map");
    SC_permanent(XX_FOO_MAP);
    
    import_array();

    _numpy_map = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);

    for (nentry = entries; nentry->name != NULL; nentry++) {
        tc_to_entry[nentry->typecode] = nentry;
        SC_hasharr_install(_numpy_map, nentry->name, nentry,
			   XX_FOO_MAP, 3, -1);
    }

#endif    

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_init_numpy_types -
 */

void _PP_init_numpy_types(PP_file *fileinfo)
{
#ifdef HAVE_PY_NUMERIC
    PP_type_entry *entry;

    entry = PP_make_type_entry(
        PP_NONE_I,                      /* typecode */
        TRUE,                           /* sequence */
        NULL,                           /* descr */
        (PyTypeObject *) &PyArray_Type, /* ob_type */
        array_pack,                     /* pack */
        NULL,                           /* unpack */
        NULL                            /* get_descr */
   );
    PP_register_object(fileinfo, entry);
#endif
}

/*--------------------------------------------------------------------------*/

#if 0
/*--------------------------------------------------------------------------*/

PyObject *_PP_unpack_to_array(PP_pdbdataObject *self)
{
    PyObject *rv;

    rv = _PP_wr_syment(self->fileinfo, self->type, self->dims,
                       self->nitems, self->data, FALSE);
    return rv;
}

/*--------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------*/





#if 0

/*--------------------------------------------------------------------------*/
/*                 NumPy arrays                                             */
/*--------------------------------------------------------------------------*/

/* new_array_object -
 */

static PyObject *new_array_object(void *value, long nitems, dimdes * dims,
                                  int typecode, int need_array)
{
    PyObject *object;
    int nd;

    nd = PP_dimdes_to_sizes(dims, nitems, sizes);
    if (nd == -1)
        return NULL;

/* XXXX - This will present problems with garbage collection because
 * we currently have no way to reclaim this memory.
 * So for now, allow python to allocate and release the memory.
 * What we need is a hook to call our own deallocator when the 
 * PyArray object is deallocated.
 */
#if 0
    object = PyArray_FromDimsAndData(nd, sizes, typecode, value);

    SC_mark(value, 1);
#else
    object = PyArray_FromDims(nd, sizes, typecode);

    {
        PyArrayObject *array = (PyArrayObject *) object;
        int elsize = array->descr->elsize;

        memcpy(array->data, value, nitems * elsize);
    }
#endif

    return object;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* copy_array_object_data - create an array out of an object.
   then copy the data to vr. */

static int copy_array_object_data(PyObject *obj, void *vr, long nitems,
                                  int type)
{
    int elsize;
    int len;
    void *value;
    PyArrayObject *array;

    /* see array_array in multiarraymodule.c */
    array =
        (PyArrayObject *) PyArray_ContiguousFromObject(obj, type, 0, 0);
    if (array == NULL)
        return -1;

#if 0
    if (PyArray_As1D(&array, &value, &len, 'd') == -1)
        return -1;
#else
    value = array->data;
    if (array->dimensions != NULL) {
        len = array->dimensions[0];
    }
#endif

    elsize = array->descr->elsize;
    memcpy(vr, value, nitems * elsize);

#if 0
    if (PyArray_Free(array, NULL) == -1)
        return -1;
#endif

    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* get_array_object_data - create an array out of an object.
 *   then copy the data to vr.
 */

static int get_array_object_data(PyObject *obj, void *vr, long nitems,
                                 int type, int gc)
{
    int elsize;
    int len, nb;
    void *value;
    PyArrayObject *array;

    /* see array_array in multiarraymodule.c */
    array = (PyArrayObject *) PyArray_FromObject(obj, type, 0, 0);
    if (array == NULL)
        return -1;

#if 0
    if (PyArray_As1D(&array, &value, &len, 'd') == -1)
        return -1;
#else
    value = array->data;
    if (array->dimensions != NULL) {
        len = array->dimensions[0];
    }
#endif

    elsize = array->descr->elsize;

    /* XXXX - under some cases it may be possible to just point this memory 
     * instead of allocating our own */
    if (gc == PP_GC_YES) {
        nb = nitems*elsize;
        DEREF(vr) = CMAKE_N(char, nb);
        memcpy(DEREF(vr), value, nb);
    } else {
        DEREF(vr) = value;
    }

#if 0
    if (PyArray_Free(array, NULL) == -1)
        return -1;
#endif
/* XXXX - */
    Py_DECREF(array);

    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 1
/*  get_array_object_type - 
 */

static char *get_array_object_type(PyObject *obj)
{
    PyArrayObject *mp = (PyArrayObject *) obj;

    switch (mp->descr->type) {
    case 'c':
        return SC_CHAR_S;
    case 'b':
        return NULL;            /* PyArray_UBYTE */
    case '1':
        return NULL;            /* PyArray_SBYTE */
    case 's':
        return NULL;            /* PyArray_SHORT */
    case 'i':
        return SC_INT_S;
    case 'l':
        return SC_LONG_S;
    case 'f':
        return SC_FLOAT_S;
    case 'd':
        return SC_DOUBLE_S;
    case 'F':
        return NULL;            /* PyArray_CFLOAT */
    case 'D':
        return NULL;            /* PyArray_CDOUBLE */
    case 'O':
        return NULL;            /* PyArray_OBJECT */
    default:
        return NULL;
    }
}
#else
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*  get_array_object_type - 
 */

static char *get_array_object_type(PyObject *obj)
{
    PyArrayObject *mp = (PyArrayObject *) obj;

    switch (mp->descr->type) {
    case 'c':
        return SC_CHAR_S;
    case 'b':
        return NULL;            /* PyArray_UBYTE */
    case '1':
        return NULL;            /* PyArray_SBYTE */
    case 's':
        return NULL;            /* PyArray_SHORT */
    case 'i':
        return SC_INT_P_S;
    case 'l':
        return SC_LONG_P_S;
    case 'f':
        return SC_FLOAT_P_S;
    case 'd':
        return SC_DOUBLE_P_S;
    case 'F':
        return NULL;            /* PyArray_CFLOAT */
    case 'D':
        return NULL;            /* PyArray_CDOUBLE */
    case 'O':
        return NULL;            /* PyArray_OBJECT */
    default:
        return NULL;
    }
}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* get_array_object_dims -
 */

static dimdes *get_array_object_dims(PyObject *obj)
{
    int i;
    dimdes *dims, *next, *prev;
    PyArrayObject *mp = (PyArrayObject *) obj;

    dims = NULL;
    prev = NULL;

    for (i = 0; i < mp->nd; i++) {
        next = _PP_mk_dimensions(0L, mp->dimensions[i]);
        if (dims == NULL) {
            dims = next;
        } else {
            prev->next = next;
            SC_mark(next, 1);
        }
        prev = next;
    }

    return dims;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NumPy Array */

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#endif
