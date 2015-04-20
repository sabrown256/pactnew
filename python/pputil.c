/*
 * PPUTIL.C
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"

#define N_PY_SPECS 16

typedef struct s_buildsp buildsp;

struct s_buildsp
   {int ity;
    int ni;
    void *vr;};

PyObject
 *PP_error_internal,
 *PP_error_user;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_SETUP_PACT - do setup for any PACT based module */

int PY_setup_pact(PyObject *m, PyObject *d)
   {int ne;

    ne = 0;

    if (ne == 0)
       ne += _PY_pact_constants(m);

    if (ne == 0)
       ne += _PY_pact_type_system(m);

    if (ne == 0)
       ne += PY_init_score(m, d);

    if (ne == 0)
       ne += PY_init_pml(m, d);

    if (ne == 0)
       ne += PY_init_pdb(m, d);

    if (ne == 0)
       {PY_DEF_GETSET(pcons, "alist");
	PY_DEF_GETSET(hasharr, "hasharr");

	PY_DEF_GETSET(PM_field, "field");
	PY_DEF_GETSET(PM_mesh_topology, "mt");
	PY_DEF_GETSET(PM_set, "set");
	PY_DEF_GETSET(PM_mapping, "map");

	PY_DEF_GETSET(defstr, "dp");
	PY_DEF_GETSET(memdes, "desc");
	PY_DEF_GETSET(PDBfile, "object");

	PY_pdbdata_type.tp_new   = PyType_GenericNew;
	PY_pdbdata_type.tp_alloc = PyType_GenericAlloc;
	ne += (PyType_Ready(&PY_pdbdata_type) < 0);};

/* add some symbolic constants to the module */
    if (ne == 0)
       {ne += (PyDict_SetItemString(d, "pdbdata",
				    (PyObject *) &PY_pdbdata_type) < 0);
	ne += (PyDict_SetItemString(d, "assoc",
				    (PyObject *) &PY_pcons_type) < 0);

/* for backward compatibility with old bindings */
	ne += (PyDict_SetItemString(d, "PDBfile", (PyObject *) &PY_PDBfile_type) < 0);};


    if (ne == 0)
       {PP_init_type_map();

#ifdef HAVE_PY_NUMPY
	_PP_init_numpy();
#endif

/* add Error Exceptions */
	PP_error_internal = PyErr_NewException("pdb.internal", NULL, NULL);
	PyDict_SetItemString(d, "internal", PP_error_internal);

	PP_error_user = PyErr_NewException("pdb.error", NULL, NULL);
	PyDict_SetItemString(d, "error", PP_error_user);};

    return(ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void PY_self_free(void *o)
   {PyObject *pyo;
    struct _typeobject *ot;

    pyo = (PyObject *) o;

    ot = PY_TYPE(pyo);
    if (ot->tp_free != NULL)
       ot->tp_free(pyo);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_STRINGS_TUPLE - return a tuple made from a C string array
 *                  - if NS is less than 0 SA is assumed to be
 *                  - NULL terminated
 *                  - if FR is TRUE then SA will be freed
 */

PyObject *PY_strings_tuple(char **sa, int ns, int fr)
   {int i, ierr;
    PyObject *rv, *po;

    if (sa == NULL)
       {rv = Py_None;
	Py_INCREF(rv);}
    else
       {if (ns < 0)
	   ns = PS_lst_length(sa);

	ierr = 0;

	rv = PyTuple_New(ns);
	for (i = 0; (i < ns) && (ierr >= 0); i++)
	    {po = PY_STRING_STRING(sa[i]);
	     if (po == NULL)
	        ierr = -1;
	     else
	        ierr = PyTuple_SetItem(rv, i, po);};

	if (ierr < 0)
	   {Py_DECREF(rv);
	    rv = NULL;};

	if (fr == TRUE)
	   PS_free_strings(sa);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_BUILD_OBJ - return a single Python object
 *               - made from the Ith item specified by
 *               - the info in SP
 */

static PyObject *_PY_build_obj(buildsp *sp, int i)
   {int ity, ni;
    void *vr;
    PyObject *rv;

    rv = NULL;
    if (sp != NULL)
       {ity = sp->ity;
	ni  = sp->ni;
	vr  = sp->vr;
	if ((i == 0) ||
	    ((0 <= i) && (i < ni)))
	   {if (SC_is_type_prim(ity) == TRUE)
	       {if ((SC_is_type_fix(ity) == TRUE) || (ity == G_ENUM_I))
		   {long lv;

		    SC_convert_id(G_LONG_I, &lv, 0, 1,
				  ity, vr, i, 1, 1, FALSE);

		    rv = PY_INT_LONG(lv);}

	        else if (SC_is_type_fp(ity) == TRUE)
		   {double dv;

		    SC_convert_id(G_DOUBLE_I, &dv, 0, 1,
				  ity, vr, i, 1, 1, FALSE);

		    rv = PyFloat_FromDouble(dv);}

		else if (ity == G_STRING_I)
                   {char *s;

		    s  = ((char **) vr)[i];

		    rv = Py_BuildValue("s", s);}

		else if (SC_is_type_ptr(ity) == TRUE)
		   {void *p;

		    p = ((void **) vr)[i];
		    rv = p;}

		else if (SC_is_type_char(ity) == TRUE)
		   PyErr_SetString(PyExc_NotImplementedError,
				   "char type");

		else if (SC_is_type_cx(ity) == TRUE)
		   PyErr_SetString(PyExc_NotImplementedError,
				   "complex type");}

	    else if (SC_is_type_struct(ity) == TRUE)
	       PyErr_SetString(PyExc_NotImplementedError,
			       "derived type");
	    else
	       PyErr_SetString(PyExc_TypeError,
			       "unknown type");};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_APPEND_OBJ - add a Python object to the Ith slot of tuple TP
 *                - made from info in SP
 */

static void _PY_append_obj(PyObject *tp, int i, buildsp *sp)
   {int l, ni;
    PyObject *o, *t;

    ni = sp->ni;
    if (ni == 0)
       o = _PY_build_obj(sp, 0);

    else
       {o = PyTuple_New(ni);
        for (l = 0; l < ni; l++)
	    {t = _PY_build_obj(sp, l);
	     PyTuple_SET_ITEM(o, l, t);};};

    PyTuple_SET_ITEM(tp, i, o);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PY_BUILD_OBJECT - return a Python object made in
 *                 - analogy with SS_make_list
 *                 - each specification is a triplet:
 *                 -   <type>, <length>, <address>
 *                 - example:
 *                 -   o = PY_build_object("foo",
 *                 -                       G_INT_I, 0, &i,
 *                 -                       G_DOUBLE_I, PG_SPACEDM, pt,
 *                 -                       0);
 */

PyObject *PY_build_object(char *nm, ...)
   {int i, n;
    buildsp sp[N_PY_SPECS];
    PyObject *rv;

    rv = NULL;

    SC_VA_START(nm);

/* find out what we have got - especially the number of specifications
 * NOTE: make this dynamic at some point
 */
    for (n = 0; n < N_PY_SPECS; n++)
        {sp[n].ity = SC_VA_ARG(int);
	 if (sp[n].ity == 0)
	    break;
	 sp[n].ni = SC_VA_ARG(int);
	 sp[n].vr = SC_VA_ARG(void *);};

    if (n < 1)
       rv = Py_None;

/* distinguish a scalar from an array that is 1 item long
 * by specifying 0 for the length of a scalar
 */
    else if ((n == 1) && (sp[0].ni == 0))
       rv = _PY_build_obj(&sp[0], 0);

    else
       {rv = PyTuple_New(n);
        for (i = 0; i < n; i++)
	    _PY_append_obj(rv, i, &sp[i]);};

    SC_VA_END;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_OPT_GENERIC - handle generic BLANG binding related operations */

void *_PY_opt_generic(void *x, bind_opt wh, void *a)
   {void *rv;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
        case BIND_LABEL :
        case BIND_PRINT :
        case BIND_FREE :
        case BIND_ALLOC :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/*
 * METH_KEYWORDS is documented at
 *  https://docs.python.org/2/c-api/structures.html?highlight=meth_keywords#METH_KEYWORDS
 *
 * The other one of interest is METH_NOARGS. This would be used for
 * function like PD_ls which take no arguments.  Currently, since
 * there is no call to parse the arguments, it will silently accept
 * any and all arguments.  By using METH_NOARGS it should complain
 * if called with arguments before it even reaches the wrapper
 * around PD_ls.
 *
 * Usage:
 * 
 * initMOD(void)
 * {
 * 
 * // Create the module and add the functions
 *     m = Py_InitModule4("basis", PB_methods,
 *                        PB__doc__,
 *                        (PyObject*)NULL,PYTHON_API_VERSION);
 *     if (m == NULL)
 *         return;
 * 
 *     if (add_module_function(m, "name", func, METH_KEYWORDS, doc) != 0)
 *     return;}
 *
 */

int add_module_function(PyObject *m, const char *name, PyCFunction meth,
			int flags, const char *doc)
   {int rv;
    char *modname;
    PyMethodDef method;
    PyObject *n, *d, *v;

    rv = -1;

    method.ml_name = name;    /* The name of the built-in function/method */
    method.ml_meth = meth;    /* The C function that implements it */
    method.flags   = flags;   /* Combination of METH_xxx flags, which mostly
                               * describe the args expected by the C func */
    method.ml_doc  = doc;     /* The __doc__ attribute, or NULL */

    modname = PyModule_GetName(m);

    n = PyString_FromString(modname);
    if (n != NULL)
       {d = PyModule_GetDict(m);

	v = PyCFunction_NewEx(&method, NULL, n);
	if (v != NULL)
	   {if (PyDict_SetItemString(d, name, v) == 0)
	       rv = 0;

	    Py_DECREF(v);};

	Py_DECREF(n);};

    return(rv);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* PY_GET_STRING - handle unicode issues */

char *PY_get_string(PyObject *o)
   {char *rv;

#if PY_MAJOR_VERSION >= 3

/*
    rv = (char *) PyUnicode_1BYTE_DATA(o);
    rv = (char *) PyUnicode_2BYTE_DATA(o);
    rv = (char *) PyUnicode_4BYTE_DATA(o);
    rv = (char *) PyUnicode_AS_DATA(o);
*/
    rv = PyUnicode_AsUTF8(o);

#else
    rv = PyString_AsString(o);
#endif

    return(rv);}

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
 *   PY_PDBfile pointer.
 *   replace by weak-references later?
 */

PyObject *_PP_find_file_obj(PDBfile *fp)
{
    Py_ssize_t i, len;
    PyObject *rv, *values;
    PY_PDBfile *obj;

    if (fp == PP_vif) {
        rv = (PyObject *) PP_vif_obj;
    } else {

        rv = NULL;
        values = PyDict_Values(PP_open_file_dict);
        len = PyList_Size(values);
        
        for (i = 0; i < len; i++) {
            obj = (PY_PDBfile *) PyList_GET_ITEM(values, i);
            if (obj->pyo == fp) {
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
        rv = (PyObject *) PY_pdbdata_newobj(NULL, vr, type, nitems, dims,
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
	    dimobj = PY_build_object("dimdes_to_obj",
				     G_LONG_I, 0, &next->index_min,
				     G_LONG_I, 0, &next->index_max,
			     0);
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
    } else if (PY_PDBfile_check(obj)) {
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

void PP_error_set(PyObject *errobj, PyObject *obj, const char *fmt, ...)
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

void PP_error_set_user(PyObject *obj, const char *fmt, ...)
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
