/*
 * PPUTIL.C
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"

PyObject
 *PP_error_internal,
 *PP_error_user;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_Py_DECREF- pact interface to Python reference decrement.
 *  This is used as the delete function when clearing a hash table
 *  of Python objects.
 */

void _PP_Py_decref(PyObject *obj)
{
    Py_XDECREF(obj);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_find_file_obj - given the PDBfile pointer, return the
 *   PP_PDBfileObject pointer.
 *   replace by weak-references later?
 */

PyObject *_PP_find_file_obj(PDBfile *fp)
{
    Py_ssize_t i, len;
    PyObject *rv, *values;
    PP_PDBfileObject *obj;

    if (fp == PP_vif) {
        rv = (PyObject *) PP_vif_obj;
    } else {

        rv = NULL;
        values = PyDict_Values(PP_open_file_dict);
        len = PyList_Size(values);
        
        for (i = 0; i < len; i++) {
            obj = (PP_PDBfileObject *) PyList_GET_ITEM(values, i);
            if (obj->object == fp) {
                rv = (PyObject *) obj;
                break;
            }
        }
        Py_DECREF(values);
    }

    if (rv == NULL)
        PP_error_set(PP_error_internal, NULL, "Unable to find file object for %s", fp->name);

    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_form_object - Create an object of the correct form.
 *  This is used when reading or indexing an object.
 */

PyObject *PP_form_object(void *vr, char *type, long nitems,
                         dimdes *dims, defstr *dp, PP_file *fileinfo,
                         PY_defstr *dpobj, PyObject *parent,
                         PP_form *form)
{
    PyObject *rv;

    if ((nitems > 1 && form->array_kind == AS_PDBDATA) ||
        (dp->members != NULL && form->struct_kind == AS_PDBDATA) ||
        form->scalar_kind == AS_PDBDATA) {
        rv = (PyObject *) PP_pdbdata_newobj(NULL, vr, type, nitems, dims,
                                            dp, fileinfo, dpobj, parent);
    } else {
        rv = _PP_wr_syment(fileinfo, type, dims, nitems, vr, form);
    }

    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_alloc_data - allocate memory for data.
 */

int PP_alloc_data(char *type, long nitems, PP_file *fileinfo, void **vr)
{
    int ierr;

    if (nitems == 0) {
        DEREF(vr) = NULL;
        ierr = 0;
    } else if (strcmp(type, "char") == 0) {
        /* catch char here because _PD_alloc_entry adds two characters
         * for NULL and SX_firsttok which get carried into the
         * PyStringObject making it larger than requested.
         */
        DEREF(vr) = CMAKE_N(char, nitems);
        ierr = 1;
    } else {
        DEREF(vr) = _PD_alloc_entry(fileinfo->file, type, nitems);
        if (DEREF(vr) == NULL) {
            PP_error_set(PP_error_user, NULL, PD_get_error());
            ierr = -1;
        } else {
            ierr = 0;
        }
    }

    if (ierr == 1) {
        if (DEREF(vr) == NULL) {
            ierr = -1;
            PyErr_NoMemory();
        } else {
            ierr = 0;
        }
    }

    return ierr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_free - */

void PP_free(void *p)
{
    CFREE(p);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* PP_num_indirection - return number of levels of indirection */

int PP_num_indirection(char *s)
{
    int iflag = 0;
    char *t;

    for (t = s + strlen(s); t > s; t--)
        if (*t == '*')
            iflag++;

    return iflag;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_add_indirection - add an indirection to a type
 */

char *PP_add_indirection(char *type, int n)
{
    int sz;
    char *ptype;

    /* space for space, star(s), and buffer */
    sz = strlen(type) + 2 + n;
    
    ptype  = CMAKE_N(char, sz);
    if (ptype == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    
    if (n == 0)
        SC_strncpy(ptype, sz, type, -1);
    else if (_PD_indirection(type))
        snprintf(ptype, sz, "%s%.*s", type, n, "********");
    else
        snprintf(ptype, sz, "%s %.*s", type, n, "********");

    return ptype;
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_ind_to_dimdes- convert a ind to a dimdes.
 */

dimdes *PP_ind_to_dimdes(int nd, long *ind)
{
    int i;
    long mini, maxi, leng;
    dimdes *dims, *next, *prev;

    prev = NULL;
    dims = NULL;
    for (i = 0; i < nd; i++) {
        mini = ind[0];
        maxi = ind[1];
        leng = (maxi - mini + 1L) / ind[2];
        ind += 3;
        next = _PD_mk_dimensions(mini, leng);
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

/* PP_dimdes_to_ind - convert a dimdes to index usable by
 *   PD_write,
 * return the rank of the array (number of dimensions)
 */

int PP_dimdes_to_ind(dimdes *dims, long *ind)
{
    int nd;
    dimdes *next;

    if (dims == NULL) {
        nd = 1;
        ind[0] = 1;
        ind[1] = 1;
        ind[2] = 1;
    } else {
        nd = 0;
        for (next = dims; next != NULL; next = next->next) {
            if (nd >= MAXDIM) {
                PP_error_set(PP_error_internal, NULL, "Too many dimensions");
                return -1;
            }
            ind[nd * 3 + 0] = next->index_min;
            ind[nd * 3 + 1] = next->index_max;
            ind[nd * 3 + 2] = 1;
            nd++;
        }
    }

    return nd;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_dimdes_to_obj - */

PyObject *PP_dimdes_to_obj(dimdes *dims)
{
    dimdes *next;
    Py_ssize_t nd;
    PyObject *rv, *dimobj;

    if (dims == NULL) {
        rv = PY_INT_LONG(1);
    } else {
        nd = 0;
        for (next = dims; next != NULL; next = next->next) {
            nd++;
        }
        rv = PyTuple_New(nd);
        nd = 0;
        for (next = dims; next != NULL; next = next->next) {
            dimobj = Py_BuildValue("ii", next->index_min, next->index_max);
            PyTuple_SET_ITEM(rv, nd, dimobj);
            nd++;
        }
    }

    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_obj_to_ind - convert an object to ind.
 *   If the indobj is not a sequence, then it must
 *   be the number of items.
 *   If indobj is a sequence, one item per dimension.
 *   if item is scalar, then it is the number of items
 *   if item is 1-long sequence: (upper, )
 *   if item is 2-long sequence: (lower, upper)
 *   if item is 3-long sequence: (lower, upper, stride)
 */

int PP_obj_to_ind(PyObject *indobj, long *ind)
{
    Py_ssize_t i, nd, nitems;
    PyObject *dim, *upper, *lower, *stride, *num;

    dim = NULL;
    lower = NULL;
    upper = NULL;
    stride = NULL;
    
    if (! PySequence_Check(indobj)) {
        num = PY_NUMBER_INT(indobj);
        if (num == NULL)
            goto err;
        ind[0] = 0;
        ind[1] = PY_INT_AS_LONG(num) - 1;
        ind[2] = 1;
        nd = 1;
    } else {
        nd = PySequence_Length(indobj);
        num = NULL;
    
        for (i = 0; i < nd; i++) {
            dim = PySequence_GetItem(indobj, i);
            if (dim == NULL)
                goto err;
            lower = NULL;
            upper = NULL;
            stride = NULL;
            if (! PySequence_Check(dim)) {
                num = PY_NUMBER_INT(dim);
                if (num == NULL)
                    goto err;
                ind[i * 3 + 0] = 0;
                ind[i * 3 + 1] = PY_INT_AS_LONG(num) - 1;
                ind[i * 3 + 2] = 1;
                Py_DECREF(num);
            } else {
                nitems = PySequence_Length(dim);
                if (nitems < 0)
                    goto err;
                switch (nitems) {
                case 1:
                    upper = PySequence_GetItem(dim, 0);
                    break;
                case 2:
                    lower = PySequence_GetItem(dim, 0);
                    upper = PySequence_GetItem(dim, 1);
                    break;
                case 3:
                    lower = PySequence_GetItem(dim, 0);
                    upper = PySequence_GetItem(dim, 1);
                    stride = PySequence_GetItem(dim, 2);
                    break;
                default:
                    PP_error_set_user(dim, "shape must be 1 - 3 long");
                    goto err;
                }
                if (lower == NULL) {
                    ind[i * 3 + 0] = 0;
                } else {
                    num = PY_NUMBER_INT(lower);
                    if (num == NULL)
                        goto err;
                    ind[i * 3 + 0] = PY_INT_AS_LONG(num);
                    Py_DECREF(num);
                }
                
                num = PY_NUMBER_INT(upper);
                if (num == NULL)
                    goto err;
                ind[i * 3 + 1] = PY_INT_AS_LONG(num);
                Py_DECREF(num);
                num = NULL;
                
                if (stride == NULL) {
                    ind[i * 3 + 2] = 1;
                } else {
                    num = PY_NUMBER_INT(stride);
                    if (num == NULL)
                        goto err;
                    ind[i * 3 + 2] = PY_INT_AS_LONG(num);
                    Py_DECREF(num);
                }
            }
            Py_DECREF(dim);
        }
    }

    return nd;
 err:
    Py_XDECREF(dim);
    Py_XDECREF(lower);
    Py_XDECREF(upper);
    Py_XDECREF(stride);
    return -1;
}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_assign_form - assign to form with error checking */

int _PP_assign_form(PP_form *form, int array, int structure, int scalar)
{
    int ok;

    ok = 0;
    
    switch (array) {
    case AS_NONE:
        break;
    case AS_PDBDATA:
    case AS_TUPLE:
    case AS_LIST:
    case AS_ARRAY:
        form->array_kind = array;
        break;
    default:
        PP_error_set_user(NULL, "Illegal value for array");
        return -1;
    }

    switch (structure) {
    case AS_NONE:
        break;
    case AS_PDBDATA:
    case AS_TUPLE:
    case AS_DICT:
        form->struct_kind = structure;
        break;
    default:
        PP_error_set_user(NULL, "Illegal value for struct");
        return -1;
    }
    
    switch (scalar) {
    case AS_NONE:
        break;
    case AS_PDBDATA:
    case AS_OBJECT:
        form->scalar_kind = scalar;
        break;
    default:
        PP_error_set_user(NULL, "Illegal value for scalar");
        return -1;
    }

    return ok;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_convert_pdbfile -  */

int PP_convert_pdbfile(PyObject *obj, void **addr)
{
    int ok;
    
    if (obj == Py_None) {
        *addr = (void *) PP_vif_obj;
        ok = 1;
    } else if (PP_PDBfile_Check(obj)) {
        *addr = (void *) obj;
        ok = 1;
    } else {
        PP_error_set_user(obj, "Expected PDBfile");
        ok = 0;
    }

    return ok;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_error_set - report a user error with a string and an object 
 *     errobj - error object to set
 *     obj    - object with error
 *     fmt    - format of error message
 *     ...    - additional variables for message
 */

void PP_error_set(PyObject *errobj, PyObject *obj, char *fmt, ...)
{
    char *line;
    PyObject *w;

    SC_VDSNPRINTF(FALSE, line, fmt);

    if (obj == NULL) {
        PyErr_SetString(errobj, line);
    } else {
        w = Py_BuildValue("(sO)", line, obj);
        if (w != NULL) {
            /*  Py_XDECREF(obj);   */
            PyErr_SetObject(errobj, w);
            Py_XDECREF(w);
        }
    }

    return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_error_set_user - report a user error with a string and an object 
 *     errobj - error object to set
 *     obj    - object with error
 *     fmt    - format of error message
 *     ...    - additional variables for message
 */

void PP_error_set_user(PyObject *obj, char *fmt, ...)
{
    char *line;
    PyObject *w;

    SC_VDSNPRINTF(FALSE, line, fmt);

    if (obj == NULL) {
        PyErr_SetString(PP_error_user, line);
    } else {
        w = Py_BuildValue("(sO)", line, obj);
        if (w != NULL) {
            /*  Py_XDECREF(obj);   */
            PyErr_SetObject(PP_error_user, w);
            Py_XDECREF(w);
        }
    }
    
    return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
