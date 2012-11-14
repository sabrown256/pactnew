/* 
 * PPTYPES.C - routines to create a table of basic system types
 *           - most functions return -1 or NULL on failure
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"

#define STATIC 
/* #define STATIC static */

#if 0
#define DEBUG_OBJ(s1, s, nitems, obj)  printobj(s1, s, nitems, obj)
#else
#define DEBUG_OBJ(s1, s, nitems, obj)
#endif

#if PY_MAJOR_VERSION >= 3
PyAPI_DATA(PyTypeObject) PyInstance_Type;
#endif

/*--------------------------------------------------------------------------*/

/*                         TYPEDEFS AND STRUCTS                             */

/*--------------------------------------------------------------------------*/

/* Pass info around when trying to figure out type a sequence is */

typedef struct s_PP_getdescr PP_getdescr;
struct s_PP_getdescr {
    long          dims[MAXDIM];
    char         *type;
    PP_types      tc;
    long          bpi;
};

/*--------------------------------------------------------------------------*/

/*                         VARIABLE DECLARATIONS                            */

/*--------------------------------------------------------------------------*/

/****************/

char
    PP_line[MAXLINE],
    *XX_OBJECT_MAP_S,
    *XX_CLASS_DESCR_S;

PDBfile
    *PP_vif = NULL;
PP_file
    *PP_vif_info = NULL;

PP_PDBfileObject *PP_vif_obj;
PyObject *PP_open_file_dict;

#if 0
static PP_type_entry *_tc_to_entry[PP_NUM_TYPES];
#endif

PP_form
    PP_global_form;

/*--------------------------------------------------------------------------*/

/*                          FUNCTION DECLARATIONS                           */

/*--------------------------------------------------------------------------*/

static int _PP_get_sequence_descr(PP_file *fileinfo, PyObject *obj, int nd, PP_getdescr *seqinfo);

/*--------------------------------------------------------------------------*/

/*                                 STRINGS                                  */

/*--------------------------------------------------------------------------*/

/* _PP_GET_STRING_DESCR - */

static PP_descr *_PP_get_string_descr(PP_file *fileinfo, PyObject *obj)
{
    dimdes *dims;
    PP_descr *descr;
    Py_ssize_t nitems;

    nitems = PY_STRING_SIZE(obj);
    dims   = _PD_mk_dimensions(0, nitems);

    descr = _PP_mk_descr(SC_CHAR_I, sizeof(char), SC_CHAR_S,
                         dims, NULL);

    return descr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* NEW_STRING_OBJECT - special case, if NEED_ARRAY is FALSE and NITEMS == 1
 *                   - then it must be a 'char' variable
 *                   - if NEED_ARRAY if false and nitems > 1
 *                   - we could also call FromStringAndSize however
 *                   - any extra characters after the terminating
 *                   - NULL but before NITEMS, would get displayed
 *                   - when the object is printed since python uses
 *                   - the given length, not the NULL
 *                   - to find the end of the string
 *                   - if one tries to store arbitary binary data in a string
 *                   - (shame on you) this will stop at the first NULL
 * XXX - make nitems Py_ssize_t?
 */

STATIC PyObject *new_string_object(void *value, long nitems,
                                   dimdes * dims, int type, int need_array)
{
    PyObject *obj;

    if (!need_array && nitems == 1)
       obj = PY_STRING_STRING_SIZE((char *) value, nitems);

    else
       obj = PY_STRING_STRING((char *) value);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COPY_STRING_OBJECT_DATA - initialize to spaces first
 *                         - space has been alocated elsewhere and
 *                         - we are filling it
 */

STATIC int copy_string_object_data(PyObject *obj, void *vr, long nitems,
                                   int type)
   {

    if (!PY_STRING_CHECK(obj)) {
        PP_error_set(PP_error_internal, obj, "Expected a string");
        return(-1);
    }

    strncpy((char *) vr, PY_STRING_AS_STRING(obj), nitems);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_STRING_OBJECT_DATA - we create our own space
 *                        - either by allocating or referencing
 *                        - an existing pointer
 */

STATIC int get_string_object_data(PyObject *obj, void *vr, long nitems,
                                  int type, int gc)
   {

    if (!PY_STRING_CHECK(obj)) {
        PP_error_set(PP_error_internal, obj, "Expected a string");
        return(-1);
    }

    if (gc == PP_GC_YES) {
        DEREF(vr) = CSTRSAVE(PY_STRING_AS_STRING(obj));
    } else {
        DEREF(vr) = PY_STRING_AS_STRING(obj);
    }

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_STRING_OBJECT_DIMS - we create our own space
 *                        - either by allocating or referencing
 *                        - an existing pointer
 */

STATIC dimdes *get_string_object_dims(PyObject *obj)
   {dimdes *dims;

    if (!PY_STRING_CHECK(obj)) {
        PP_error_set(PP_error_user, obj, "Expected a string");
        return(NULL);
    }

    dims = _PD_mk_dimensions(0L, PY_STRING_SIZE(obj));

    return(dims);}

/*--------------------------------------------------------------------------*/

/*                                 FLOAT                                    */

/*--------------------------------------------------------------------------*/

/* NEW_FLOAT_OBJECT - this routine checks if only one item is needed
 *                  - to avoid making everything an array
 *                  - (just for neatness)
 *                  - however; it may be possible that it is needed in
 *                  - an array context, if so then put it in a list
 */

STATIC PyObject *new_float_object(void *value, long nitems, dimdes * dims,
                                  int type, int need_array)
   {int ierr;
    PyObject *obj, *obj1;

    if (nitems == 1) {
        switch (type) {
        case PP_FLOAT_I:
            obj = PyFloat_FromDouble((double) (*(float *) value));
            break;
        case PP_DOUBLE_I:
            obj = PyFloat_FromDouble(*(double *) value);
            break;
        default:
            PP_error_set(PP_error_internal, NULL, "can not convert to float");
            obj = NULL;
            need_array = 0;
        }
        if (need_array) {
            obj1 = PyList_New(nitems);
            if (obj1 == NULL)
                return(NULL);
            ierr = PyList_SetItem(obj1, 0, obj);
            if (ierr == -1)
                return(NULL);
            obj = obj1;
        }
#if 0
-    } else {
-        obj = new_array_object(value, nitems, dims, type, need_array);
#endif
    }

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COPY_FLOAT_OBJECT_DATA - space has been alocated elsewhere and
 *                        - we are filling it
 */

STATIC int copy_float_object_data(PyObject *obj, void *vr, long nitems,
                                  int type)
   {int ierr;

    if (PyFloat_Check(obj)) {
        switch (type) {
        case PP_FLOAT_I:
            *(float *) vr = (float) PyFloat_AS_DOUBLE(obj);
            ierr = 0;
            break;
        case PP_DOUBLE_I:
            *(double *) vr = PyFloat_AS_DOUBLE(obj);
            ierr = 0;
            break;
        default:
            PP_error_set(PP_error_internal, obj, "can not convert to float");
            ierr = -1;
        }
#if 0
-    } else {
-        ierr = copy_array_object_data(obj, vr, nitems, type);
#endif
    }

    return(ierr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_FLOAT_OBJECT_DATA - we create our own space
 *                       - either by allocating or referencing
 *                       - an existing pointer
 */

STATIC int get_float_object_data(PyObject *obj, void *vr, long nitems,
                                 int type, int gc)
   {int ierr;

    if (PyFloat_Check(obj)) {
        switch (type) {
        case PP_FLOAT_I:
            if (gc == PP_GC_YES) {
                *(float **) vr = CMAKE(float);

                **(float **) vr = (float) PyFloat_AS_DOUBLE(obj);
            } else {
                PP_error_set(PP_error_internal, obj, "can not process gc == PP_GC_NO");
                /* WARNING - this is accessing a python structure outside of the API */
                *(double **) vr = &(((PyFloatObject *) obj)->ob_fval);
            }
            ierr = 0;
            break;
        case PP_DOUBLE_I:
            if (gc == PP_GC_YES) {
                *(double **) vr = CMAKE(double);

                **(double **) vr = PyFloat_AS_DOUBLE(obj);
            } else {
                /* WARNING - this is accessing a python structure outside of the API */
                *(double **) vr = &(((PyFloatObject *) obj)->ob_fval);
            }
            ierr = 0;
            break;
        default:
            PP_error_set(PP_error_internal, obj, "can not convert to float");
            ierr = -1;
        }
        ierr = 0;
#if 0
-    } else {
-        ierr = get_array_object_data(obj, vr, nitems, type, gc);
#endif
    }

    return(ierr);}

/*--------------------------------------------------------------------------*/

/*                                   INT                                    */

/*--------------------------------------------------------------------------*/

/* NEW_INT_OBJECT -  */

STATIC PyObject *new_int_object(void *value, long nitems, dimdes * dims,
                                int type, int need_array)
   {int ierr;
    PyObject *obj, *obj1;

    if (nitems == 1) {
        switch (type) {
        case PP_INT_I:
            obj = PY_INT_LONG((*(int *) value));
            break;
        case PP_LONG_I:
            obj = PY_INT_LONG(*(long *) value);
            break;
        default:
            PP_error_set(PP_error_internal, NULL, "can not convert to int");
            obj = NULL;
            need_array = 0;
            break;
        }
        if (need_array) {
            obj1 = PyList_New(nitems);
            if (obj1 == NULL)
                return(NULL);
            ierr = PyList_SetItem(obj1, 0, obj);
            if (ierr == -1)
                return(NULL);
            obj = obj1;
        }
#if 0
-    } else {
-        obj = new_array_object(value, nitems, dims, type, need_array);
#endif
    }

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COPY_INT_OBJECT_DATA - space has been alocated elsewhere and
 *                      - we are filling it
 */

STATIC int copy_int_object_data(PyObject *obj, void *vr, long nitems,
                                int type)
   {int ierr;

    if (PY_INT_CHECK(obj)) {
        switch (type) {
        case PP_INT_I:
            *(int *) vr = (int) PY_INT_AS_LONG(obj);
            ierr = 0;
            break;
        case PP_LONG_I:
            *(long *) vr = PY_INT_AS_LONG(obj);
            ierr = 0;
            break;
        default:
            PP_error_set(PP_error_internal, obj, "can not convert to int");
            return(-1);
        }
#if 0
-    } else {
-        ierr = copy_array_object_data(obj, vr, nitems, type);
#endif
    }

    return(ierr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_INT_OBJECT_DATA - we create our own space
 *                     - either by allocating or referencing
 *                     - an existing pointer
 */

STATIC int get_int_object_data(PyObject *obj, void *vr, long nitems,
                               int type, int gc)
   {int ierr;

    if (PY_INT_CHECK(obj))
       {switch (type)
	   {case PP_INT_I :
	         {int *ip, **ipp;
		  ip  = NULL;
		  ipp = (int **) vr;
		  if (gc == PP_GC_YES)
		     {ip  = CMAKE(int);
		      *ip = PY_INT_AS_LONG(obj);}

		  else
		     {ierr = -1;
		      PP_error_set(PP_error_internal, obj,
				   "can not process gc == PP_GC_NO");
#if PY_MAJOR_VERSION < 3
/* WARNING - this is accessing a python structure outside of the API */
		      ip = (int *) &(((PY_INT_OBJECT *) obj)->ob_ival);
#endif
		     };

		  *ipp = ip;};
		 break;

	    case PP_LONG_I :
	         {long *lp, **lpp;
		  lp  = NULL;
		  lpp = (long **) vr;
		  if (gc == PP_GC_YES)
		     {lp  = CMAKE(long);
		      *lp = PY_INT_AS_LONG(obj);}
		  else
		     {PP_error_set(PP_error_internal, obj,
				   "can not process gc == PP_GC_NO");
/* WARNING - this is accessing a python structure outside of the API */
#if PY_MAJOR_VERSION < 3
	              lp = &(((PY_INT_OBJECT *) obj)->ob_ival);
#endif
		     };

		  *lpp = lp;
		  ierr = 0;};
		 break;

	    default :
	         PP_error_set(PP_error_internal, obj,
			      "can not convert to int");
		 ierr = -1;
		 break;};}

#if 0
-    else
-        ierr = get_array_object_data(obj, vr, nitems, type, gc);
#endif

    return(ierr);}

/*--------------------------------------------------------------------------*/

/*                                    NONE                                  */

/*--------------------------------------------------------------------------*/

/* NEW_NONE_OBJECT - */

STATIC PyObject *new_none_object(void *value, long nitems, dimdes *dims,
                                 int typecode, int need_array)
   {

    Py_INCREF(Py_None);

    return(Py_None);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COPY_NONE_OBJECT_DATA -  */

STATIC int copy_none_object_data(PyObject *obj, void *vr, long nitems,
                                 int type)
   {

    DEREF(vr) = NULL;

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GET_NONE_OBJECT_DATA - */

STATIC int get_none_object_data(PyObject *obj, void *vr, long nitems,
                                int type, int gc)
   {

    DEREF(vr) = NULL;

    return(0);}

/*--------------------------------------------------------------------------*/

/*                                  SEQUENCE                                */

/*--------------------------------------------------------------------------*/

#if 0
-
-/* get_sequence_object_data - Change a list into C memory.
- *
- * XXX - replace with a single layer version of _PP_rd_syment
- * similar to the way PP_make_object is to _PP_wr_syment.
- *
- * create an array out of an object.
- * If numbers, then create an NumPy array,
- * else create a array of pointers.
- * XXX - must garbage collect.
- */
-
-static int get_sequence_object_data(PyObject *obj, void *vr, long nitems,
-                                    int type, int gc)
-{
-    int typecode;
-    int ierr;
-    char **pv;
-    PP_type_entry *entry;
-    Py_ssize_t i;
-    PyObject *obj1;
-
-    nitems = PyObject_Length(obj);
-    typecode = PyArray_ObjectType(obj, 0);
-
-    switch (typecode) {
-    case PP_CHAR_I:
-        pv = CMAKE_N(char *, nitems);
-
-        obj1 = PySequence_GetItem(obj, 0);
-        entry = PP_inquire_object(fileinfo, obj1);
-        if (entry == NULL) {
-            PP_error_set(PP_error_internal, obj1, "Can not get pact type");
-            return(-1);
-        }
-        for (i = 0; i < nitems; i++) {
-            obj1 = PySequence_GetItem(obj, i);
-            entry->get_data(obj1, &pv[i], 1L, typecode, PP_GC_YES);
-        }
-        DEREF(vr) = (char *) pv;
-        ierr = 0;
-        break;
-    default:
-        ierr = get_array_object_data(obj, vr, nitems, typecode, PP_GC_YES);
-    }
-
-    return(ierr);
-}
-
-/*--------------------------------------------------------------------------*/
-/*--------------------------------------------------------------------------*/
-
-/*  get_sequence_object_type - return(the type of a sequence.
- *    This makes sure that all the elements in a sequence have the
- *    same type.
- *    Py_None will match anything.
- */
-
-static char *get_sequence_object_type(PyObject *obj)
-{
-    int typecode;
-    char ts[MAXLINE];
-    PP_ssize_t i, nitems, istart;
-    PyObject *obj1;
-    PP_descr *descr;
-
-    DEBUG_OBJ("TOP", "obj", 0, obj);
-    nitems = PyObject_Length(obj);
-    if (nitems == -1)
-        return(NULL);
-    if (nitems == 0)
-        return(NULL);
-
-    /* find the type of the first non-None item */
-    istart = -1;
-    for (i = 0; i < nitems; i++) {
-        obj1 = PySequence_GetItem(obj, i);
-        if (obj1 != Py_None) {
-            istart = i + 1;
-            break;
-        }
-    }
-    if (istart == -1) {
-        PP_error_set(PP_error_internal, obj, "All members are None");
-        return(NULL);
-    }
-
-    if (PySequence_Check(obj1)) {
-        /* get type of first member, then make sure all the rest
-           are of the same type */
-        descr = PP_get_object_type(obj1);
-        if (descr == NULL)
-            return(NULL);
-        DEBUG_OBJ("FIRST", descr->type, 0, obj1);
-
-        /* special case string for efficiency */
-        if (PY_STRING_CHECK(obj1)) {
-            PyTypeObject *otype = PY_TYPE(obj1);
-
-            for (i = istart; i < nitems; i++) {
-                obj1 = PySequence_GetItem(obj, i);
-                if (obj1 == Py_None)
-                    continue;
-                DEBUG_OBJ("string", "obj1", i, obj1);
-                if (PY_TYPE(obj1) != otype) {
-                    PP_error_set(PP_error_internal, obj,
-                                 "Sequence members are not all strings");
-                    return(NULL);
-                }
-            }
-#if 0
-            strcpy(ts, type);
-	    SC_strcat(ts, MAXLINE, "**");
-            CFREE(type);
-            type = CSTRSAVE(ts);
-#endif
-        } else {
-            char *nextdescr;
-
-            for (i = istart; i < nitems; i++) {
-                obj1 = PySequence_GetItem(obj, i);
-                if (obj1 == Py_None)
-                    continue;
-                nextdescr = PP_get_object_descr(fileinfo, obj1);
-                DEBUG_OBJ("string", nextdescr->type, i, obj1);
-                if (nexttype == NULL)
-                    return(NULL);
-                if (strcmp(descr->type, nextdescr->type) != 0) {
-                    PP_error_set(PP_error_internal, obj,
-                                 "Sequence members are not all %s",
-                                 type);
-                    return(NULL);
-                }
-            }
-#if 0
-            strcpy(ts, type);
-	    SC_strcat(ts, MAXLINE, "*");
-            CFREE(type);
-            type = CSTRSAVE(ts);
-#endif
-        }
-        strcpy(ts, type);
-	SC_strcat(ts, MAXLINE, "*");
-        CFREE(type);
-        type = CSTRSAVE(ts);
-
-    } else {
-        typecode = PP_ObjectType(obj, 0);
-#if 1
-        switch (typecode) {
-        case PP_CHAR_I:
-            break;
-        case PP_UBYTE_I:
-        case PP_SBYTE_I:
-        case PP_SHORT_I:
-        case PP_INT_I:
-        case PP_LONG_I:
-            type = SC_LONG_S;
-            break;
-        case PP_FLOAT_I:
-        case PP_DOUBLE_I:
-            type = SC_DOUBLE_S;
-            break;
-        case PP_CFLOAT_I:
-        case PP_CDOUBLE_I:
-            PP_error_set(PP_error_internal, obj, "Complex not supported");
-            type = NULL;
-            break;
-        default:
-            PP_error_set(PP_error_internal, obj, "Unknown NumPy type %d", typecode);
-            type = NULL;
-        }
-#else
-        switch (typecode) {
-        case PP_CHAR_I:
-            break;
-        case PP_UBYTE_I:
-        case PP_SBYTE_I:
-        case PP_SHORT_I:
-        case PP_INT_I:
-        case PP_LONG_I:
-            type = SC_LONG_P_S;
-            break;
-        case PP_FLOAT_I:
-        case PP_DOUBLE_I:
-            type = SC_DOUBLE_P_S;
-            break;
-        case PP_CFLOAT_I:
-        case PP_CDOUBLE_I:
-            PP_error_set(PP_error_internal, obj, "Complex not supported");
-            type = NULL;
-            break;
-        default:
-            PP_error_set(PP_error_internal, obj, "Unknown NumPy type %d", typecode);
-            type = NULL;
-        }
-#endif
-    }
-
-    return(type);
-}
-
-/*--------------------------------------------------------------------------*/
-/*--------------------------------------------------------------------------*/
-
-static char *get_sequence_object_ptype(PyObject *obj)
-{
-    char *type;
-    char ts[MAXLINE];
-
-    type = get_sequence_object_type(obj);
-    strcpy(ts, type);
-    SC_strcat(ts, MAXLINE, "*");
-    CFREE(type);
-    type = CSTRSAVE(ts);
-
-    return(type);
-}
-
-/*--------------------------------------------------------------------------*/
-/*--------------------------------------------------------------------------*/
-
-/* get_sequence_object_dims -
- */
-
-static dimdes *get_sequence_object_dims(PyObject *obj)
-{
-    dimdes *dims = NULL;
-    Py_ssize_t nitems;
-
-    nitems = PyObject_Length(obj);
-    if (nitems != -1) {
-        dims = _PD_mk_dimensions(0L, nitems);
-    }
-
-    return(dims);
-}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int _PP_pack_pdbdata(void *p, PyObject *v, long nitems, PP_types tc)
{
    PP_error_set(PP_error_internal, NULL, "_PP_pack_pdbdata");
    return(-1);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *_PP_unpack_pdbdata(void *p, long nitems)
{
    PP_error_set(PP_error_internal, NULL, "_PP_unpack_pdbdata");
    return(NULL);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PP_descr *_PP_get_pdbdata_descr(PP_file *fileinfo, PyObject *obj)
{
    PP_descr *descr;
    PP_pdbdataObject *self = (PP_pdbdataObject *) obj;
    
    descr = _PP_mk_descr(PP_PDBDATA_I, self->dp->size, self->type,
                         self->dims, self->data);

    return descr;
}

/*--------------------------------------------------------------------------*/
#if 0
 UNUSED
/*--------------------------------------------------------------------------*/

static int _PP_pack_instance(void *p, PyObject *v, long nitems, PP_types tc)
{
    PP_error_set(PP_error_internal, NULL, "_PP_pack_pdbdata");
    return(-1);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *_PP_unpack_instance(void *p, long nitems)
{
    PP_error_set(PP_error_internal, NULL, "_PP_unpack_instance");
    return(NULL);
}

/*--------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------*/

/* _PP_get_instance_descr - Take a PyInstanceObject,
 * extract class and look up its descriptor
 */

static PP_descr *_PP_get_instance_descr(PP_file *fileinfo, PyObject *obj)
   {

#if PY_MAJOR_VERSION < 3
    PyInstanceObject *inst;
    PyTypeObject *cls;
    PP_class_descr *cdescr;

    inst = (PyInstanceObject *) obj;
    cls  = (PyTypeObject *) inst->in_class;

    cdescr = PP_inquire_class(fileinfo, cls);
    if (cdescr == NULL)
        return(NULL);

    return(cdescr->descr);
#else
    return(NULL);
#endif

    }

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_make_descr - 
 */

PP_descr *PP_make_descr(
    PP_types      typecode,
    char         *type,
    long          bpi
    )
{
    return _PP_mk_descr(
        typecode, bpi,
        CSTRSAVE(type),
        NULL, NULL);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_mk_descr - */

PP_descr *_PP_mk_descr(
    PP_types      typecode,
    long          bpi,
    char         *type,
    dimdes       *dims,
    void         *data
    )
{
    PP_descr *descr;

    descr = CMAKE(PP_descr);

    descr->typecode  = typecode;
    descr->bpi       = bpi;
    descr->type      = type;
    descr->dims      = dims;
    descr->data      = data;

    SC_mark(descr->type, 1);
    SC_mark(descr->dims, 1);
    
    return descr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PP_rl_descr(PP_descr *descr)
{
    int n;

    n = SC_mark(descr, 0);
    if (n < 2) {
        CFREE(descr->type);
        _PD_rl_dimensions(descr->dims);
        CFREE(descr);
    } else {
        SC_mark(descr, -1);
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_make_type_entry - 
 */

PP_type_entry *PP_make_type_entry(
    PP_types       typecode,
    int            sequence,
    PP_descr      *descr,
    PyTypeObject  *otype,
    PP_pack_func   pack,
    PP_unpack_func unpack,
    PP_get_descr   get_descr
    )
{
    PP_type_entry *entry;

    entry = CMAKE(PP_type_entry);

    entry->typecode  = typecode;
    entry->sequence  = sequence;
    entry->descr     = descr;
    entry->obtyp     = otype;
    entry->pack      = pack;
    entry->unpack    = unpack;
    entry->get_descr = get_descr;

    /* Always NULL except for instance types */
    entry->cdescr    = NULL;

    SC_mark(descr, 1);
    /* XXX refcount otype ? */
    
    return(entry);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int PP_rl_type_entry(haelem *hp, void *a)
{
    int n, ok;
    PP_type_entry *entry;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &entry, FALSE);
    SC_ASSERT(ok == TRUE);

    n = SC_mark(entry, 0);
    if (n < 2) {
        if (entry->descr) {
            _PP_rl_descr(entry->descr);
        }
        CFREE(entry);
    } else {
        SC_mark(entry, -1);
    }
    return(TRUE);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_register_type - 
 */

void PP_register_type(PP_file *fileinfo, PP_type_entry *entry)
{
    if (entry->descr == NULL)
        printf("XXXX - error");
    SC_hasharr_install(fileinfo->type_map, entry->descr->type, entry,
		       XX_OBJECT_MAP_S, 3, -1);

#if 0
    _tc_to_entry[entry->typecode] = entry;
#endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PP_type_entry *PP_inquire_type(PP_file *fileinfo, char *ctype)
{
    PP_type_entry *entry;

    entry = (PP_type_entry *) SC_hasharr_def_lookup(fileinfo->type_map, ctype);

    return(entry);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_register_object -
 */

void PP_register_object(PP_file *fileinfo, PP_type_entry *entry)
{
    SC_hasharr_install(fileinfo->object_map, entry->obtyp, entry,
		       XX_OBJECT_MAP_S, 3, -1);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_make_class_descr -
 *   How to treat Python instance variables of a class
 */

PP_class_descr *PP_make_class_descr(
    PyTypeObject *cls,
    char *type,
    PyFunctionObject *ctor
    )
{
    PP_descr *descr;
    PP_class_descr *cdescr;

    descr = PP_make_descr(PP_INSTANCE_I, type, 0);

    cdescr = CMAKE(PP_class_descr);

    cdescr->cls       = cls;
    cdescr->ctor      = ctor;
    cdescr->type      = descr->type;
    cdescr->descr     = descr;

    SC_mark(type, 1);
    SC_mark(descr, 1);
    
    return(cdescr);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int _PP_rl_class_descr(haelem *hp, void *a)
{
    int n, ok;
    PP_class_descr *cdescr;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &cdescr, FALSE);
    SC_ASSERT(ok == TRUE);

    n = SC_mark(cdescr, 0);
    if (n < 2) {
        CFREE(cdescr->type);
        _PP_rl_descr(cdescr->descr);
        CFREE(cdescr);
    } else {
        SC_mark(cdescr, -1);
    }

    return(TRUE);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_inquire_class -
 */

PP_class_descr *PP_inquire_class(PP_file *fileinfo, PyTypeObject *cls)
{
    PP_class_descr *cdescr;

    cdescr = (PP_class_descr *) SC_hasharr_def_lookup(fileinfo->class_map, (void *) cls);
    
    return(cdescr);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_inquire_object - search PP_object_map sequentially looking of an
 *              - object that matches the type of obj,
 *              - then return PP_type_entry of object.
 */

PP_type_entry *PP_inquire_object(PP_file *fileinfo, PyObject *obj)
{
    PP_type_entry *entry;
    PyTypeObject *tp;

#if 0
    entry = (PP_type_entry *) SC_hasharr_def_lookup(fileinfo->object_map, PY_TYPE(obj));
#else
    entry = NULL;
    for (tp = PY_TYPE(obj); tp != NULL; tp = tp->tp_base) {
        entry = (PP_type_entry *) SC_hasharr_def_lookup(fileinfo->object_map, tp);
        if (entry != NULL)
            break;
    }
#endif
    
    return(entry);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void PP_init_type_map_basic(PP_file *fileinfo)
{
    PP_descr *descr;
    PP_type_entry *entry;

    /* type hash table */
    fileinfo->type_map   = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);
    fileinfo->object_map = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_ADDR_KEY, 0);
    fileinfo->class_map  = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_ADDR_KEY, 0);
    fileinfo->deftypes   = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY, 0);

    descr = PP_make_descr(
        PP_CHAR_I,                      /* typecode */
        SC_CHAR_S,                      /* type */
        sizeof(char)                    /* bpi */
        );

    entry = PP_make_type_entry(
        PP_CHAR_I,                      /* typecode */
        TRUE,                           /* sequence */
        descr,                          /* descr */
        &PY_STRING_TYPE,                 /* otype */
        _PP_get_pack_func(PP_CHAR_I),   /* pack */
        _PP_get_unpack_func(PP_CHAR_I), /* unpack */
        _PP_get_string_descr            /* get_descr */
        );

    PP_register_type(fileinfo, entry);   /* char */
    PP_register_object(fileinfo, entry); /* PY_STRING_TYPE */

    descr = PP_make_descr(
        PP_INT_I,                       /* typecode */
        SC_INT_S,                       /* type */
        sizeof(int)                     /* bpi */
        );

    entry = PP_make_type_entry(
        PP_INT_I,                       /* typecode */
        FALSE,                          /* sequence */
        descr,
        &PY_INT_TYPE,                    /* otype */
        _PP_get_pack_func(PP_INT_I),    /* pack */
        _PP_get_unpack_func(PP_INT_I),  /* unpack */
        NULL                            /* get_descr */
        );

    PP_register_type(fileinfo, entry);  /* int */

    /* make integer just like int */
    
    descr = PP_make_descr(
        PP_INT_I,                       /* typecode */
        SC_INTEGER_S,                   /* type */
        sizeof(int)                     /* bpi */
        );

    entry = PP_make_type_entry(
        PP_INT_I,                       /* typecode */
        FALSE,                          /* sequence */
        descr,
        &PY_INT_TYPE,                    /* otype */
        _PP_get_pack_func(PP_INT_I),    /* pack */
        _PP_get_unpack_func(PP_INT_I),  /* unpack */
        NULL                            /* get_descr */
        );

    PP_register_type(fileinfo, entry);  /* integer */

#if 0
-    entry =
-        PP_make_type_entry(SC_INT_S, SC_INT_P_S, sizeof(int),
-                               &PY_INT_TYPE,
-                               NULL, NULL, PP_INT_I, FALSE,
-                               new_int_object,
-                               -1,
-                               copy_int_object_data,
-                               _PP_get_pack_func(PP_INT_I),
-                               _PP_get_unpack_func(PP_INT_I),
-                               get_int_object_data,
-                               NULL,
-                               NULL);
-    PP_register_type(fileinfo, entry);
#endif
    
    descr = PP_make_descr(
        PP_LONG_I,                      /* typecode */
        SC_LONG_S,                      /* type */
        sizeof(long)                    /* bpi */
        );

    entry = PP_make_type_entry(
        PP_LONG_I,                      /* typecode */
        FALSE,                          /* sequence */
        descr,                          /* descr */
        &PY_INT_TYPE,                    /* otype */
        _PP_get_pack_func(PP_LONG_I),   /* pack */
        _PP_get_unpack_func(PP_LONG_I), /* unpack */
        NULL                            /* get_descr */
        );

    PP_register_type(fileinfo, entry);   /* int */
    PP_register_object(fileinfo, entry); /* PY_INT_TYPE */
    
    entry = PP_make_type_entry(
        PP_LONG_I,                      /* typecode */
        FALSE,                          /* sequence */
        descr,                          /* descr */
        &PyLong_Type,                    /* otype */
        _PP_get_pack_func(PP_LONG_I),   /* pack */
        _PP_get_unpack_func(PP_LONG_I), /* unpack */
        NULL                            /* get_descr */
        );

    /* Only register PyLong_Type, 'long' type maps to PY_INT_TYPE */
    PP_register_object(fileinfo, entry);  /* PyLong_Type */
    
    descr = PP_make_descr(
        PP_FLOAT_I,                     /* typecode */
        SC_FLOAT_S,                     /* type */
        sizeof(float)                   /* bpi */
        );

    entry = PP_make_type_entry(
        PP_FLOAT_I,                     /* typecode */
        FALSE,                          /* sequence */
        descr,                          /* descr */
        &PyFloat_Type,                  /* otype */
        _PP_get_pack_func(PP_FLOAT_I),  /* unpack */
        _PP_get_unpack_func(PP_FLOAT_I), /* unpack */
        NULL                            /* get_descr */
        );

    PP_register_type(fileinfo, entry);   /* float */

    descr = PP_make_descr(
        PP_DOUBLE_I,                     /* typecode */
        SC_DOUBLE_S,                     /* type */
        sizeof(double)                   /* bpi */
        );


    entry = PP_make_type_entry(
        PP_DOUBLE_I,                     /* typecode */
        FALSE,                           /* sequence */
        descr,                           /* descr */
        &PyFloat_Type,                   /* otype */
        _PP_get_pack_func(PP_DOUBLE_I),  /* pack */
        _PP_get_unpack_func(PP_DOUBLE_I),/* unpack */
        NULL                             /* get_descr */
        );

    PP_register_type(fileinfo, entry);   /* double */
    PP_register_object(fileinfo, entry); /* PyFloat_Type */

    entry = PP_make_type_entry(
        PP_PDBDATA_I,                    /* typecode */
        FALSE,                           /* sequence */
        NULL,                            /* descr */
        &PP_pdbdata_Type,                /* otype */
        _PP_pack_pdbdata,                /* pack */
        _PP_unpack_pdbdata,              /* unpack */
        _PP_get_pdbdata_descr            /* get_descr */
        );

    PP_register_object(fileinfo, entry); /* PP_pdbdata_Type */

    /* This entry is used to take a python instance and find
     * the PP_descr so we know the PDBlib type.
     */
    entry = PP_make_type_entry(
        PP_INSTANCE_I,                   /* typecode */
        FALSE,                           /* sequence */
        NULL,                            /* descr */
        &PyInstance_Type,                /* otype */
        NULL, /*_PP_pack_instance, */              /* pack */
        NULL, /* _PP_unpack_instance, */            /* unpack */
        _PP_get_instance_descr           /* get_descr */
        );

    PP_register_object(fileinfo, entry); /* PyInstance_Type */

    return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_init_type_map_instance -
 *   Install into type_map.
 *   Install into class_map.
 */
void PP_init_type_map_instance(PP_file *fileinfo, PP_class_descr *cdescr)
{
    PP_type_entry *entry;

    entry = PP_make_type_entry(
        PP_INSTANCE_I,                   /* typecode */
        FALSE,                           /* sequence */
        cdescr->descr,                   /* descr */
        NULL, /* &PyInstance_Type,      *//* otype */
        NULL, /* _PP_pack_instance,     *//* pack */
        NULL, /* _PP_unpack_instance,   *//* unpack */
        NULL  /* _PP_get_instance_descr *//* get_descr */
        );

    entry->cdescr = cdescr;

    SC_hasharr_install(fileinfo->class_map, cdescr->cls, cdescr,
		       XX_CLASS_DESCR_S, 3, -1);

    SC_hasharr_install(fileinfo->type_map, cdescr->type, entry,
		       XX_OBJECT_MAP_S, 3, -1);

    return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void PP_init_type_map(void)
{
    XX_OBJECT_MAP_S = CSTRSAVE("PP_type_entry");
    SC_permanent(XX_OBJECT_MAP_S);

    _PP_create_defstr_tab();
    
#if 0
-    PP_set_mark_data_hook(NULL);
-    PP_set_rel_data_hook(NULL);
-    PP_set_alloc_data_hook(NULL);
-    PP_set_sizeof_data_hook(NULL);
-    PP_set_convert_pdbfile_hook(PP_missing_pdbfile);
#endif



#if 0
-    /* function pointer */
-    entry = PP_make_type_entry("function", "function", 0L,
-                                   NULL,
-                                   NULL, NULL, PP_FLOAT_I, FALSE,
-                                   new_none_object,
-                                   -1,
-                                   copy_none_object_data,
-                                   _PP_get_pack_func(PP__I),
-                                   _PP_get_unpack_func(PP__I),
-                                   get_none_object_data,
-                                   NULL,
-                                   NULL);
-    PP_register_type(fileinfo, entry);
#endif
    
#if 0
-    /* List */
-    /* A list has no intrinsic dimension because nested lists
-     * lists must be of the same kind to be meaniful to C.
-     * The get_sequence_type takes any nested lists into account
-     * by creating an additional indirection.
-     *  i.e.  ['foo', 'bar'] is type char** with no dims.
-     * It would be possible to also call this char* with dim=[2]
-     * but that then makes a dependency between the
-     * get_type and get_dims functions.
-     */
-    entry = PP_make_type_entry(NULL, NULL, 0L, &PyList_Type,
-                                   get_sequence_object_type,
-                                   NULL /* get_sequence_object_ptype */, 0, TRUE,
-                                   NULL,
-                                   -1,
-                                   NULL,
-                                   _PP_get_pack_func(PP__I),
-                                   _PP_get_unpack_func(PP__I),
-                                   get_sequence_object_data,
-                                   get_sequence_object_dims,
-                                   NULL);
-    PP_register_object(fileinfo, entry);
-
-    entry = PP_make_type_entry(NULL, NULL, 0L, &PyTuple_Type,
-                                   get_sequence_object_type,
-                                   NULL /* get_sequence_object_ptype */, 0, TRUE,
-                                   NULL,
-                                   -1,
-                                   NULL,
-                                   _PP_get_pack_func(PP__I),
-                                   _PP_get_unpack_func(PP__I),
-                                   get_sequence_object_data,
-                                   get_sequence_object_dims,
-                                   NULL);
-    PP_register_object(fileinfo, entry);
#endif

#if 0
-    /* Py_None */
-    entry = PP_make_type_entry(SC_STRING_S, NULL, 0L, PY_TYPE(Py_None),
-                                   NULL, NULL, 0, FALSE,
-                                   NULL,
-                                   -1,
-                                   copy_none_object_data,
-                                   _PP_get_pack_func(PP__I),
-                                   _PP_get_unpack_func(PP__I),
-                                   get_none_object_data, NULL, NULL);
-    PP_register_object(fileinfo, entry);
#endif

    PP_global_form.array_kind = AS_LIST;
    PP_global_form.struct_kind = AS_TUPLE;
    PP_global_form.scalar_kind = AS_OBJECT;

    return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_INIT_FILE - define score types in a PDB file */

void _PP_init_file(PDBfile *file)
   {

#if 0
/* hash table types */
-    PD_def_attr_str(file);
#endif
    
#if 0
/* XXX - double defines SC_array resulting in memory leaks */ 
/* PML types */
    PD_def_mapping(file);
#endif
    
/* SCORE types */
    PD_def_hash_types(file, 3);

/* PDBLib types */
    PD_def_pdb_types(file);
    
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_open_vif - create a virtual internal file.
 *              - install some native types into the fileinfo.
 */

PP_file *_PP_open_vif(char *name)
{
    PP_file *fileinfo;
    PDBfile *fp;
        
    fp = PD_open_vif(name);
    fileinfo = CMAKE(PP_file);
    fileinfo->file = fp;

    _PP_init_file(fp);

    PP_init_type_map_basic(fileinfo);

#ifdef HAVE_PY_NUMPY
    _PP_init_numpy_types(fileinfo);
#endif

    return fileinfo;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_close_file - close a file */

void _PP_close_file(PP_file *fileinfo)
{
    PDBfile *fp;

    fp = fileinfo->file;

    /* cleanup defstr constructors */
    _PP_cleanup_defstrs(fp);

    /* cleanup defstr singletons */
    SC_free_hasharr(fileinfo->deftypes, _PP_decref_object, NULL);
    SC_free_hasharr(fileinfo->type_map, PP_rl_type_entry, NULL);
    SC_free_hasharr(fileinfo->object_map, PP_rl_type_entry, NULL);
    SC_free_hasharr(fileinfo->class_map, _PP_rl_class_descr, NULL);
    
    CFREE(fileinfo);
    PD_close(fp);
    
    return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_copy_obj_to_mem - copy the object into the memory pointed to by vr.
 *
 *    obj      - Python object to copy to memory.
 *    entry    -
 *    nitems   - number of items
 *    vr       - address of memory to be filled.
 *    sequence - obj must be a sequence (possibly of 1 item)
 *
 *    return values:
 *    -1   : error exception
 *     0   : copied object
 *
 */

int PP_copy_obj_to_mem(PyObject *obj, PP_file *fileinfo, PP_type_entry *entry, long nitems,
                       void *vr, dimdes *dims)
{
    int mitems = 0;
    int err = 0;
    PP_type_entry *objentry;

    if (entry == NULL) {
        PP_error_set_user(NULL, "entry is NULL");
        obj = NULL;
    } else if (entry->pack != NULL) {
        if (nitems == 1 && PySequence_Check(obj) == 0) {
            err = entry->pack(vr, obj, 1, entry->typecode);
            mitems = 1;
        } else {
            /* lookup the item we have to find out how to get what we want */
            objentry = PP_inquire_object(fileinfo, obj);
            if (objentry != NULL && objentry->pack != NULL) {
                err = objentry->pack(vr, obj, nitems, entry->typecode);
                mitems = nitems;
            } else {
                int i;
                PyObject *fast, *next;
                
                fast = PySequence_Fast(obj, "need sequence");
                if (fast == NULL)
                    return -1;

                if (dims == NULL || dims->next == NULL) {
                    if (dims != NULL) {
                        nitems = dims->number;
                    }
                    for (i = 0; i < nitems; i++) {
                        next = PySequence_Fast_GET_ITEM(fast, i);
                        if (next == NULL)
                            return -1;
                        err = entry->pack(vr, next, 1, entry->typecode);
                        if (err == -1)
                            break;
                        vr = (void *) (((char *) vr) + entry->descr->bpi);
                    }
                    mitems = nitems;
                } else {
                    int mitems2 = 0;
                    for (i = 0; i < dims->number; i++) {
                        next = PySequence_Fast_GET_ITEM(fast, i);
                        if (next == NULL)
                            return -1;
                        mitems = PP_copy_obj_to_mem(next, fileinfo, entry, dims->number, vr, dims->next);
                        if (mitems == -1)
                            break;
                        vr = (void *) (((char *) vr) + entry->descr->bpi*mitems);
                        mitems2 += mitems;
                    }
                    if (mitems > 0) 
                        mitems = mitems2;
                }
                Py_DECREF(fast);
            }
        }
    } else {
        err = -1;
    }
         
    if (err == -1) {
        if (PyErr_Occurred() == NULL) {
            PP_error_set_user(obj, "No way to pack object data: %s",
                              PY_TYPE(obj)->tp_name);
        }
        mitems = -1;
    }

    return mitems;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_unpack_sequence -
 */

int PP_unpack_sequence(PyObject *obj, PP_type_entry *entry,
                       void **vr, PP_file *fileinfo)
{
    int ierr;
    long mitems;
    Py_ssize_t nitems;
    
    nitems = PySequence_Length(obj);
    if (nitems < 0) {
        PP_error_set_user(obj, "expected sequence");
        return(-1);
    }

    ierr = PP_alloc_data(entry->descr->type, nitems, fileinfo, vr);
    if (ierr < 0)
        return(ierr);

    mitems = PP_copy_obj_to_mem(obj, fileinfo, entry, nitems, *vr, NULL);

    if (nitems == mitems) {
        ierr = 0;
    } else {
        ierr = -1;
    }
    
    return ierr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_copy_mem_to_obj - Convert a data pointer into a python object. */

PyObject *PP_copy_mem_to_obj(PDBfile *file,
                             void *vr,
                             dimdes *dims,
                             long nitems,
                             char *type,
                             PP_type_entry *entry,
                             PP_form *form,
                             int sequence)
{
    PyObject *obj;

    obj = NULL;
    
    if (vr == NULL) {
        Py_INCREF(Py_None);
        obj = Py_None;
    } else if (entry == NULL || entry->unpack == NULL) {
#if 0
-        obj = (PyObject *) PP_scoredata_New(vr, type, nitems, dims, file);
#endif
        PP_error_set_user(NULL, "No 'unpack' function for type %s", type);
    } else {
#if 0
-        if (form->array_kind == AS_PDBDATA) {
-            obj = PP_pdbdata_newobj(NULL, vr, type, nitems, dims, PP_pdbdataObject *obj,
-                                    void *vr, char *type, long nitems,
-                                    dimdes *dims, defstr *dp, PDBfile *file,
-                                    PP_defstrObject *dpobj, PyObject *parent)
-        }
#endif
        if (form->array_kind == AS_ARRAY) {
            /* try to create an array out of it */
            obj = PP_array_unpack(vr, nitems, entry->typecode);
        }
        if (obj == NULL) {
            /* if object can create it's own sequence let it,
             * else put multiple items into a sequence */
            if ((nitems == 1 || entry->sequence == TRUE) &&
                sequence == FALSE) {
                obj = entry->unpack(vr, nitems);
            } else {
                int i;
                PyObject *item;
                /* Can not use PySequence_SetItem because tuples
                 * do not support sequence protocal */
                switch (form->array_kind) {
                case AS_LIST:
                    obj = PyList_New(nitems);
                    if (obj == NULL)
                        return(NULL);
                    for (i=0; i< nitems; i++) {
                        item = entry->unpack(vr, 1);
                        if (item == NULL)  /* XXX cleanup */
                            return(NULL);
                        PyList_SET_ITEM(obj, i, item);
                        vr = (void *) (((char *) vr) + entry->descr->bpi);
                    }
                    break;
                case AS_TUPLE:
                    obj = PyTuple_New(nitems);
                    if (obj == NULL)
                        return(NULL);
                    for (i=0; i< nitems; i++) {
                        item = entry->unpack(vr, 1);
                        if (item == NULL)  /* XXX cleanup */
                            return(NULL);
                        PyTuple_SET_ITEM(obj, i, item);
                        vr = (void *) (((char *) vr) + entry->descr->bpi);
                    }
                    break;
                default:
                    PP_error_set(PP_error_internal, NULL,
                                 "Unexpected value of form->array_kind in PP_copy_mem_to_obj: %d",
                                 form->array_kind);
                    obj = NULL;
                }
            }
        }
    }
    /* XXX - verify order of this test */
#if 0
-    } else if (need_scoredata) {
-        obj = (PyObject *) PP_scoredata_New(vr, type, nitems, dims, file);
#endif

    return(obj);
}

/*--------------------------------------------------------------------------*/
#if 0
-/*--------------------------------------------------------------------------*/
-
-/* PP_get_object_data - return a pointer to the C memory contained in the type.
- *            - also return the type.
- *    object 'obj'.
- *
- *    obj      - Python object to get the pointer for.
- *    entry    -
- *    vr       - address of pointer to set.
- *               note: cannot return vr as value of function
- *               because NULL is a legal value and can not
- *               signal when an error has occured.
- *    gc       - PP_GC_YES or PP_GC_NO
- *               yes will produce an item that must be garbage collected
- *               that is it has been allocated by pact, and thus uses the
- *               pact memory manager.
- *               no will return a pointer into the python object.
- *
- *   return -1 error
- *   return  0 worked
- *   return  1 didn't get object
- */
-
-int PP_get_object_data(PyObject *obj, PP_type_entry *entry, long nitems,
-                       void **vr, int gc)
-{
-    int err = 1;                /* assume no way to get object */
-
-    if (entry != NULL) {
-        if (obj == Py_None) {
-            *vr = NULL;
-            err = 0;
-        } else if (entry->get_data != NULL) {
-            err = entry->get_data(obj, vr, nitems, entry->typecode, gc);
-        } else if (PY_TYPE(obj) == entry->obtyp) {
-            if (entry->poffset != -1) {
-#if 1
-                DEREF(vr) = DEREF((char *) obj + entry->poffset);
-#else
-                DEREF(vr) = (char *) obj + entry->poffset;
-#endif
-                SC_mark(DEREF(vr), 1);
-                err = 0;
-            } else {
-                err = -1;
-            }
-        } else {
-            /* XXXX - did not make, try to create out of given data */
-#if 0
-            _PP_rd_syment(obj, PP_vif, ep, entry->ptype, vr);
-#endif
-            /* didn't make, inform caller so it may try alternative method
-               to get the object */
-            err = 1;
-        }
-    } else {
-        err = -1;
-    }
-
-    if (err == -1) {
-        if (PyErr_Occurred() == NULL) {
-            PP_error_set_user(obj, "No way to get object data: %s",
-                              PY_TYPE(obj)->tp_name);
-        }
-    }
-
-    return(err);
-}
-
/*--------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------*/

/* PP_make_data - Make 'C' data out of objects given the type.
 *  obj  -   input object
 *  file -   file to lookup types
 *  type -   type to create
 *  dims -   dimension of data
 *  vr   -   returned address data to fill.
 *           a pointer to the data to fill.   (type *) vr
 */

int PP_make_data(PyObject *obj, PP_file *fileinfo, char *type,
                 dimdes *dims, void **vr)
{
    int ierr;
    long nitems;
    void *pv;
#if 0
    static PP_type_entry *scoredata_entry = NULL;

    if (PP_scoredata_Check(obj)) {
        /* XXXX - note: this is a total hack */
        if (scoredata_entry == NULL)
            scoredata_entry = PP_inquire_object(fileinfo, obj);
        nitems = 1L;            /* XXXX - compute from dims ? */
        ierr = PP_get_object_data(obj, scoredata_entry, nitems, &pv, TRUE);
        if (ierr == 0) {
            SC_mark(pv, 1);
        }
        DEREF(vr) = pv;
    } else {
#endif
        nitems = _PD_comp_num(dims);

        ierr = PP_alloc_data(type, nitems, fileinfo, &pv);
        if (ierr < 0)
            return(-1);
        ierr = _PP_rd_syment(obj, fileinfo, type, dims, nitems, pv);

        if (ierr < 0) {
            (void) _PP_rel_syment(fileinfo->file->host_chart, pv, nitems, type);
            CFREE(pv);
        }
        DEREF(vr) = pv;

    return(ierr);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_get_object_descr - given a python object, return the descriptor.
 *                      - Check for a hook first.  This is especially useful
 *                      - for String type where the dimension must be computed
 *                      - but a entry->descr entry also exists.
 *  Return a new reference.
 */

PP_descr *_PP_get_object_descr(PP_file *fileinfo, PyObject *obj, PP_type_entry *entry)
{
    PP_descr *descr;

    if (entry->get_descr != NULL) {
        descr = entry->get_descr(fileinfo, obj);
        SC_mark(descr, 1);
    } else if (entry->descr != NULL) {
        descr = entry->descr;
        SC_mark(descr, 1);
    } else {
        PP_error_set_user(obj, "No way to get object descriptor: %s",
                          PY_TYPE(obj)->tp_name);
        return(NULL);
    }

    return(descr);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_get_sequence_descr_work - find type and shape for a member of a sequence.
 *  See if object is a of a registered type.
 *  Nested sequence
 */

static int _PP_get_sequence_descr_work(PP_file *fileinfo, PyObject *obj, int nd, PP_getdescr *seqinfo)
{
    int rv;
    PP_descr *descr;
    PP_type_entry *entry;

    rv = 0;
    entry = PP_inquire_object(fileinfo, obj);
    if (entry != NULL) {
        descr = _PP_get_object_descr(fileinfo, obj, entry);
        if (descr == NULL)
            return -1;

        switch (descr->typecode) {
        case PP_PDBDATA_I:
            if (seqinfo->tc == PP_NONE_I) {
                seqinfo->tc = PP_PDBDATA_I;
                seqinfo->type = descr->type;
                SC_mark(seqinfo->type, 1);
                seqinfo->bpi  = descr->bpi;
            } else if (seqinfo->tc != PP_PDBDATA_I) {
                /* mixing types */
            } else if (strcmp(seqinfo->type, descr->type) != 0) {
                /* mixing types */
            }
            break;
        case PP_CHAR_I:
            if (seqinfo->tc == PP_NONE_I) {
                seqinfo->tc = PP_CHAR_I;
                seqinfo->type = descr->type;
                SC_mark(seqinfo->type, 1);
                seqinfo->bpi  = descr->bpi;
            } else if (seqinfo->tc != PP_CHAR_I) {
                /* mixing chars and non-char */
                rv = -1;
            }
            break;
        case PP_NONE_I:
            rv = -1;
            break;
        default:
            /* coerce to largest type */
            if (descr->typecode > seqinfo->tc) {
                seqinfo->tc = descr->typecode;
                if (seqinfo->type != NULL) {
                    CFREE(seqinfo->type);
                }
                seqinfo->type = descr->type;
                SC_mark(seqinfo->type, 1);
                seqinfo->bpi  = descr->bpi;
            }
        }

#if 0
        if (descr->dims != NULL) {
            /* the object itself is a sequence */
#if 0
            PP_error_set_user(NULL, "_PP_get_sequence_descr_work does not accept sequences: %s", PY_TYPE(obj)->tp_name);
            rv = -1;
#else
            if (nd >= MAXDIM) {
                PP_error_set_user(NULL, "Sequences nested too deeply, max %d", MAXDIM);
                return -1;
            }
            nitems = _PD_comp_num(descr->dims);
            if (seqinfo->dims[nd] == 0) {
                seqinfo->dims[nd] = nitems;
            } else if (seqinfo->dims[nd] != nitems) {
                seqinfo->dims[nd] = -1;
            }
#endif
        }
#endif
        _PP_rl_descr(descr);
    } else if (PySequence_Check(obj) == 0) {
        PP_error_set_user(obj, "cannot get PACT type");
        rv = -1;
    } else {
        rv = _PP_get_sequence_descr(fileinfo, obj, nd, seqinfo);
    }

    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_get_sequence_descr - given an python sequence, find type of leaves.
 *     return 0 if all the same PyType
 */

static int _PP_get_sequence_descr(PP_file *fileinfo, PyObject *obj, int nd,  PP_getdescr *seqinfo)
{
    int rv;
    Py_ssize_t i, nitems;
    PyObject *fast, *next;

    if (nd >= MAXDIM) {
        PP_error_set_user(NULL, "Sequences nested too deeply, max %d", MAXDIM);
        return -1;
    }
    fast = PySequence_Fast(obj, "_PP_get_sequence_descr");
    if (fast == NULL)
        return -1;
    nitems = PySequence_Length(obj);

    /* record size of first dimension, then make sure the others match */
    if (seqinfo->dims[nd] == 0) {
        seqinfo->dims[nd] = nitems;
    } else if (seqinfo->dims[nd] != nitems) {
        seqinfo->dims[nd] = -1;
    }
    nd++;

    rv = 0;
    for (i = 0; i < nitems; i++) {
        next = PySequence_Fast_GET_ITEM(fast, i);
        if (next == NULL)
            return -1;

        if (next == Py_None) {
            /* With Py_None, it is impossible to assume a type code or a shape */
            seqinfo->dims[nd] = -1;
        } else {
            rv = _PP_get_sequence_descr_work(fileinfo, next, nd, seqinfo);
            if (rv == -1)
                break;
        }
    }
    Py_DECREF(fast);

    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_get_object_descr - given a python object, return the descriptor.
 * Find out if the shape is regular and if so return dimensions.
 * If irregular, then add indirection to type.
 *
 * Return a new reference.
 */

PP_descr *PP_get_object_descr(PP_file *fileinfo, PyObject *obj)
{
    int i, rv, nd, nindir;
    long ind[3*MAXDIM];
    PP_descr *descr;
    PP_type_entry *entry;
    PP_getdescr seqinfo;

    if (obj == Py_None) {
        /* treat None as int * NULL */
        descr = _PP_mk_descr(PP_INT_I, sizeof(void *),
                             SC_INT_P_S, NULL, NULL);
        SC_mark(descr, 1);
        return descr;
    }

    entry = PP_inquire_object(fileinfo, obj);
    if (entry != NULL) {
        descr = _PP_get_object_descr(fileinfo, obj, entry);
    } else if (PySequence_Check(obj) == 0) {
        /* unregistered type and not a sequence (tuple or list) */
        PP_error_set_user(obj, "cannot get PACT type for %s",
                          PY_TYPE(obj)->tp_name);
        descr = NULL;
    } else {
        for (i = 0; i < MAXDIM; i++) {
            seqinfo.dims[i] = 0;
        }
        seqinfo.type    = NULL;
        seqinfo.tc      = PP_NONE_I;
        seqinfo.bpi     = 0;
        
        rv = _PP_get_sequence_descr(fileinfo, obj, 0, &seqinfo);
        
        if (rv == 0) {
            char *type;
            dimdes *dims;

            /* Find levels of regular dimensions
             * 0  = not set
             * -1 = not regular
             */
            nd = 0;
            for (i = 0; i < MAXDIM; i++) {
                if (seqinfo.dims[i] <= 0)
                    break;
                ind[nd * 3 + 0] = 0;
                ind[nd * 3 + 1] = seqinfo.dims[i] - 1;
                ind[nd * 3 + 2] = 1;
                nd++;
            }
            /* Find levels of indirection */
            nindir = 0;
            for ( ; i < MAXDIM; i++) {
                if (seqinfo.dims[i] == 0)
                    break;
                nindir++;
            }
            if (seqinfo.tc == PP_NONE_I) {
                /*  [ None, None ], assume int */
                seqinfo.tc = PP_INT_I;
                type = PP_add_indirection(SC_INT_S, nindir);
                seqinfo.bpi = sizeof(int);
            } else {
                type = PP_add_indirection(seqinfo.type, nindir);
            }
            dims = PP_ind_to_dimdes(nd, ind);
            descr = _PP_mk_descr(seqinfo.tc, seqinfo.bpi,
                                 type, dims, NULL);
            SC_mark(descr, 1);
            CFREE(seqinfo.type);
        } else {
            PP_error_set_user(obj, "cannot get PACT type for %s",
                              PY_TYPE(obj)->tp_name);
            descr = NULL;
        }
    }

    return descr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* Return a descriptor based on the original descriptor and the
 * requested output type.
 * This makes sure that the output type is possible based on descr.
 */

PP_descr *PP_outtype_descr(PDBfile *fp, PP_descr *descr, char *type)
{
    int defoff, de;
    char *ts;
    dimdes *dims, *dprev;
    PP_descr *newdescr;
    defstr *dp;
    void *vr;

    if (type == NULL) {
        type = descr->type;
        dims = descr->dims;
    } else {
        /* get default offset */
        defoff = PD_get_offset(fp);

        /* requested dimensions */
        dims = _PD_ex_dims(type, defoff, &de);

        if (dims != NULL) {
            /* get base type, without dimension information */
            ts    = CSTRSAVE(type);
            strtok(ts, "()[");
            type = ts;
        }

        if (dims == NULL) {
            /* get dimension from descr
             * If indirection, then write out as pointer - no dimension
             */
            if (_PD_indirection(type) == FALSE) {
                dp = PD_inquire_host_type(fp, type);
                if (dp == NULL) {
                    dims = descr->dims;
                } else if (dp->members == NULL) {
                    dims = descr->dims;
                } else if (descr->dims != NULL) {
                    /* type is a structure */
                    if (descr->dims->next == NULL) {
                        /* 1-d, dimension represent members so do not use */
                        dims = NULL;
                    } else {
                        /* remove last dimension since it represents
                         * the members */
                        dprev = descr->dims;
                        for (dims = dprev->next; ; dims = dims->next) {
                            if (dims->next == NULL)
                                break;
                            dprev = dims;
                        }
                        if (dprev != NULL) {
                            _PD_rl_dimensions(dims);
                            dprev->next = NULL;
                        }
                        dims = descr->dims;
                    }
                }
            }
        }
    }

    vr = descr->data;

    newdescr = _PP_mk_descr(descr->typecode, descr->bpi,
                            type, dims, vr);

    return newdescr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/



#if 0
-    int typecode;
-    char ts[MAXLINE];
-    PyObject i, nitems, istart;
-    PyObject *obj1;
-    PP_descr *descr;
-
-    DEBUG_OBJ("TOP", "obj", 0, obj);
-    nitems = PyObject_Length(obj);
-    if (nitems == -1)
-        return(NULL);
-    if (nitems == 0)
-        return(NULL);
-
-    /* find the type of the first non-None item */
-    istart = -1;
-    for (i = 0; i < nitems; i++) {
-        obj1 = PySequence_GetItem(obj, i);
-        if (obj1 != Py_None) {
-            istart = i + 1;
-            break;
-        }
-    }
-    if (istart == -1) {
-        PP_error_set(PP_error_internal, obj, "All members are None");
-        return(NULL);
-    }
-
-    if (PySequence_Check(obj1)) {
-        /* get type of first member, then make sure all the rest
-           are of the same type */
-        descr = PP_get_object_type(obj1);
-        if (descr == NULL)
-            return(NULL);
-        DEBUG_OBJ("FIRST", descr->type, 0, obj1);
-
-        /* special case string for efficiency */
-        if (PY_STRING_CHECK(obj1)) {
-            PyTypeObject *otype = PY_TYPE(obj1);
-
-            for (i = istart; i < nitems; i++) {
-                obj1 = PySequence_GetItem(obj, i);
-                if (obj1 == Py_None)
-                    continue;
-                DEBUG_OBJ("string", "obj1", i, obj1);
-                if (PY_TYPE(obj1) != otype) {
-                    PP_error_set(PP_error_internal, obj,
-                                 "Sequence members are not all strings");
-                    return(NULL);
-                }
-            }
-#if 0
-            strcpy(ts, type);
-	    SC_strcat(ts, MAXLINE, "**");
-            CFREE(type);
-            type = CSTRSAVE(ts);
-#endif
-        } else {
-            char *nextdescr;
-
-            for (i = istart; i < nitems; i++) {
-                obj1 = PySequence_GetItem(obj, i);
-                if (obj1 == Py_None)
-                    continue;
-                nextdescr = PP_get_object_descr(fileinfo, obj1);
-                DEBUG_OBJ("string", nextdescr->type, i, obj1);
-                if (nexttype == NULL)
-                    return(NULL);
-                if (strcmp(descr->type, nextdescr->type) != 0) {
-                    PP_error_set(PP_error_internal, obj,
-                                 "Sequence members are not all %s",
-                                 type);
-                    return(NULL);
-                }
-            }
-#if 0
-            strcpy(ts, type);
-	    SC_strcat(ts, MAXLINE, "*");
-            CFREE(type);
-            type = CSTRSAVE(ts);
-#endif
-        }
-        strcpy(ts, type);
-	SC_strcat(ts, MAXLINE, "*");
-        CFREE(type);
-        type = CSTRSAVE(ts);
-
-    } else {
-        typecode = PP_ObjectType(obj, 0);
-#if 1
-        switch (typecode) {
-        case PP_CHAR_I:
-            break;
-        case PP_UBYTE_I:
-        case PP_SBYTE_I:
-        case PP_SHORT_I:
-        case PP_INT_I:
-        case PP_LONG_I:
-            type = SC_LONG_S;
-            break;
-        case PP_FLOAT_I:
-        case PP_DOUBLE_I:
-            type = SC_DOUBLE_S;
-            break;
-        case PP_CFLOAT_I:
-        case PP_CDOUBLE_I:
-            PP_error_set(PP_error_internal, obj, "Complex not supported");
-            type = NULL;
-            break;
-        default:
-            PP_error_set(PP_error_internal, obj, "Unknown NumPy type %d", typecode);
-            type = NULL;
-        }
-#else
-        switch (typecode) {
-        case PP_CHAR_I:
-            break;
-        case PP_UBYTE_I:
-        case PP_SBYTE_I:
-        case PP_SHORT_I:
-        case PP_INT_I:
-        case PP_LONG_I:
-            type = SC_LONG_P_S;
-            break;
-        case PP_FLOAT_I:
-        case PP_DOUBLE_I:
-            type = SC_DOUBLE_P_S;
-            break;
-        case PP_CFLOAT_I:
-        case PP_CDOUBLE_I:
-            PP_error_set(PP_error_internal, obj, "Complex not supported");
-            type = NULL;
-            break;
-        default:
-            PP_error_set(PP_error_internal, obj, "Unknown NumPy type %d", typecode);
-            type = NULL;
-        }
-#endif
-    }
-
-    return(type);
#endif


#if 0
-/*--------------------------------------------------------------------------*/
-/*--------------------------------------------------------------------------*/
-
-/* _PP_GET_OBJECT_DIMS - */
-
-dimdes *_PP_get_object_dims(PyObject *obj, PP_type_entry *entry)
-   {dimdes *rv;
-
-    if (entry->get_dims != NULL)
-       rv = entry->get_dims(obj);
-    else
-       rv = NULL;
-
-    return(rv);}
-
-/*--------------------------------------------------------------------------*/
-/*--------------------------------------------------------------------------*/
-
-/* PP_GET_OBJECT_DIMS - */
-
-dimdes *PP_get_object_dims(PyObject * obj)
-   {PP_type_entry *entry;
-    dimdes *rv;
-
-    entry = PP_inquire_object(fileinfo, obj);
-    if (entry == NULL)
-       rv = NULL;
-    else
-       rv = _PP_get_object_dims(obj, entry);
-
-    return(rv);}
-
-/*--------------------------------------------------------------------------*/
-
#endif

/*--------------------------------------------------------------------------*/

/* PP_GET_OBJECT_LENGTH - return number of items */

/* XXX - change to Py_ssize_t */

int PP_get_object_length(PyObject *obj)
{
    Py_ssize_t nitems;
    
    if (obj == NULL)
       nitems = 0;
    else
       {nitems = PyObject_Length(obj);
        if (nitems < 0)
	   {PyErr_Clear();
            nitems = 1;};};

    return(nitems);}
 
#if 0
-        if (PyArray_Check(obj)) {
-        return(PyArray_Size(obj));
-    } else if (PY_STRING_CHECK(obj)) {
-        return(PY_STRING_SIZE(obj));
-    } else if (PyFloat_Check(obj)) {
-        return(1);
-    } else if (PY_INT_CHECK(obj)) {
-        return(1);
-    } else if (PySequence_Check(obj)) {
-        return(PySequence_Length(obj));
-    } else if (obj == Py_None) {
-        return(1);
-    } else {
-        /* XXXX - should a hook be provided here? */
-        return(1);
-    }
-    
-}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_decref_object - */

int _PP_decref_object(haelem *hp, void *a)
{
    int ok;
    PyObject *obj;

    ok = SC_haelem_data(hp, NULL, NULL, (void **) &obj, FALSE);
    SC_ASSERT(ok == TRUE);

    Py_DECREF(obj);

    return(TRUE);

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/


