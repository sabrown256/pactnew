/*
 * PYPDBL.C - PDB list functionality in Python convert 'C' memory into
 *          - python object
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"
#include "pdbrw.h"

#define STATIC
/* #define STATIC  static */

#if 0
#define DEBUG_WR(s1, s, nitems, obj)  printobjold(s1, s, nitems, obj)
#else
#define DEBUG_WR(s1, s, nitems, obj)
#endif

static PyObject
 *_leaf_work(PP_file *fileinfo, char *outtype, long nitems,
	     dimdes *dims, void *vr, PP_form *form, int need_sequence),
 *_indirect_work(PP_file *fileinof, char *outtype, long nitems,
		 dimdes *dims, void *vr, PP_form *form, int need_sequence);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

STATIC void printobjold(char *s1, char *s, long nitems, PyObject *obj)
{
    static FILE *outto;

    outto = stderr;
    fprintf(outto, "%s %s %ld: ", s1, s, nitems);
    PyObject_Print(obj, outto, 0);
    fprintf(outto, "\n");

}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/*
 * If a ctor is provided, then create a dictionary and call ctor.
 */

static PyObject *_leaf_derived(PP_file *fileinfo, defstr *dp, void *vr,
			       PP_form *form, PP_type_entry *entry,
			       int need_sequence)
{
    int ierr;
    int ii;
    char *ttype;
    void *svr;
    memdes *desc, *mem_lst;
    PyObject *obj, *item;
    PyObject *argtuple, *rv;
    int struct_kind;
    int doobj;

    if (entry == NULL || entry->cdescr->ctor == NULL) {
        struct_kind = form->struct_kind;
        doobj = FALSE;
    } else {
        struct_kind = AS_DICT;
        doobj = TRUE;
    }

    mem_lst = dp->members;
    
    switch (struct_kind) {
    case AS_TUPLE:
        /* compute number of entries in struct */
        for (ii = 0, desc = mem_lst; desc != NULL; ii++, desc = desc->next)
            ;
        obj = PyTuple_New(ii);

        ii = 0;
        for (desc = mem_lst; desc != NULL; desc = desc->next) {
            svr = (void *) (((char *) vr) + desc->member_offs);
            PP_CAST_TYPE(ttype, desc, svr, vr, Py_None, NULL);
            if (_PD_indirection(ttype)) {
                item = _indirect_work(fileinfo, ttype, desc->number, desc->dimensions,
                                      svr, form, TRUE);
            } else {
                item = _leaf_work(fileinfo, ttype, desc->number, desc->dimensions,
                                  svr, form, FALSE);
            }
            
            ierr = PyTuple_SetItem(obj, ii, item);      /* steals reference */
            if (ierr < 0)
                break;
            ii++;
        }
        break;
    case AS_DICT:
        obj = PyDict_New();
        for (desc = mem_lst; desc != NULL; desc = desc->next) {
            svr = (void *) (((char *) vr) + desc->member_offs);
            PP_CAST_TYPE(ttype, desc, svr, vr, Py_None, NULL);
            item = _leaf_work(fileinfo, ttype, desc->number, desc->dimensions,
                              svr, form, FALSE);
            ierr = PyDict_SetItemString(obj, desc->name, item);      /* steals reference */
            if (ierr < 0)
                break;
        }
        break;
    default:
        PP_error_set(PP_error_internal, NULL,
                     "Unexpected value of form->array_struct in _PP_wr_syment: %d",
                     form->array_kind);
        return NULL;
    }
    
        
    if (ierr < 0) {
        obj = NULL;
    }

    if (obj != NULL && doobj == TRUE) {
        argtuple = PyTuple_Pack(1, obj);
        rv = PyObject_Call((PyObject *) entry->cdescr->ctor, argtuple, NULL);
        Py_DECREF(argtuple);
        Py_DECREF(obj);
        obj = rv;
    }
    
    return obj;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *_leaf_work_1d(PP_file *fileinfo, long nitems, void *vr,
			       defstr *dp, PP_type_entry *entry,
			       PP_form *form)
{
    int i;
    PyObject *obj, *item;

    if (entry == NULL || entry->cdescr != NULL) {
        /* must be structure type. */
        /* Can not use PySequence_SetItem because tuples
         * do not support sequence protocol */
        switch (form->array_kind) {
        case AS_LIST:
            obj = PyList_New(nitems);
            if (obj == NULL)
                return NULL;
            for (i=0; i< nitems; i++) {
                item = _leaf_derived(fileinfo, dp, vr, form, entry, FALSE);  /* XXX need_sequence */
                if (item == NULL)  /* XXX cleanup */
                    return NULL;
                PyList_SET_ITEM(obj, i, item);
                vr = (void *) (((char *) vr) + dp->size);
            }
            break;
        case AS_TUPLE:
            obj = PyTuple_New(nitems);
            if (obj == NULL)
                return NULL;
            for (i=0; i< nitems; i++) {
                item =_leaf_derived(fileinfo, dp, vr, form, entry, FALSE);  /* need_sequence); */
                if (item == NULL)  /* XXX cleanup */
                    return NULL;
                PyTuple_SET_ITEM(obj, i, item);
                vr = (void *) (((char *) vr) + dp->size);
            }
            break;
        default:
            PP_error_set(PP_error_internal, NULL,
                         "Unexpected value of form->array_kind in PP_copy_mem_to_obj: %d",
                         form->array_kind);
            obj = NULL;
        }
    } else if (entry->sequence == TRUE) {
        /* it can make it own sequence (like PyString), then do */
        obj = entry->unpack(vr, nitems);
    } else {
        /* Can not use PySequence_SetItem because tuples
         * do not support sequence protocol */
        switch (form->array_kind) {
        case AS_LIST:
            obj = PyList_New(nitems);
            if (obj == NULL)
                return NULL;
            for (i=0; i< nitems; i++) {
                item = entry->unpack(vr, 1);
                if (item == NULL)  /* XXX cleanup */
                    return NULL;
                PyList_SET_ITEM(obj, i, item);
                vr = (void *) (((char *) vr) + entry->descr->bpi);
            }
            break;
        case AS_TUPLE:
            obj = PyTuple_New(nitems);
            if (obj == NULL)
                return NULL;
            for (i=0; i< nitems; i++) {
                item = entry->unpack(vr, 1);
                if (item == NULL)  /* XXX cleanup */
                    return NULL;
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

    return obj;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *_leaf_work_n(PP_file *fileinfo, long nitems, dimdes *dims,
			      void *vr, defstr *dp, PP_type_entry *entry,
			      PP_form *form)
{
    int i;
    long size;
    PyObject *obj, *item;

    if (dims == NULL) {
        /* entire array */
        obj = _leaf_work_1d(fileinfo, nitems, vr, dp, entry, form);
    } else if (dims->next == NULL) {
        /* last dimension of array */
        obj = _leaf_work_1d(fileinfo, dims->number, vr, dp, entry, form);
    } else {
        long mitems;

        mitems = nitems / dims->number;
        nitems = dims->number;
        if (dp != NULL) {
            size = dp->size;
        }
        if (entry != NULL) {
            size = entry->descr->bpi;
        }
            
        switch (form->array_kind) {
        case AS_LIST:
            obj = PyList_New(nitems);
            if (obj == NULL)
                return NULL;
            for (i=0; i< nitems; i++) {
                item = _leaf_work_n(fileinfo, mitems, dims->next, vr, dp, entry, form);
                if (item == NULL)  /* XXX cleanup */
                    return NULL;
                PyList_SET_ITEM(obj, i, item);
                vr = (void *) (((char *) vr) + mitems*size);
            }
            break;
        case AS_TUPLE:
            obj = PyTuple_New(nitems);
            if (obj == NULL)
                return NULL;
            for (i=0; i< nitems; i++) {
                item = _leaf_work_n(fileinfo, mitems, dims->next, vr, dp, entry, form);
                if (item == NULL)  /* XXX cleanup */
                    return(NULL);
                PyTuple_SET_ITEM(obj, i, item);
/*                vr = (void *) (((char *) vr) + mitems*entry->descr->bpi);*/
                vr = (void *) (((char *) vr) + mitems*size);
            }
            break;
        default:
            PP_error_set(PP_error_internal, NULL,
                         "Unexpected value of form->array_kind in PP_copy_mem_to_obj: %d",
                         form->array_kind);
            obj = NULL;
        }
    }
    return obj;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *_leaf_work(PP_file *fileinfo, char *outtype, long nitems,
			    dimdes *dims, void *vr, PP_form *form,
			    int need_sequence)
{
    defstr *dp;
    PyObject *obj;
    PP_type_entry *entry;

    if (vr == NULL) {
        Py_INCREF(Py_None);
        obj = Py_None;
    } else {
        
        entry = PP_inquire_type(fileinfo, outtype);

        if (entry == NULL || entry->cdescr != NULL) {
            /* the host type must be used to get the correct member offsets for the
             * in memory copy - the file ones might be wrong!!
             */
            dp = PD_inquire_host_type(fileinfo->file, outtype);
            if (dp == NULL) {
                PP_error_set_user(NULL, "Unknown type: %s", outtype);
                return NULL;
            }
            if (nitems == 1 && need_sequence == FALSE) {
                obj = _leaf_derived(fileinfo, dp, vr, form, entry, need_sequence);
            } else {
                obj = _leaf_work_n(fileinfo, nitems, dims, vr, dp, entry, form);
            }
        } else if (entry->unpack == NULL) {
            PP_error_set_user(NULL, "No 'unpack' function for type %s", outtype);
            obj = NULL;
        } else if (form->array_kind == AS_ARRAY) {
            /* try to create an array out of entire thing */
            /* XXX - multi dimensional arrays */
            obj = PP_array_unpack(vr, nitems, entry->typecode);
        } else if (nitems == 1 && need_sequence == FALSE) {
            obj = entry->unpack(vr, 1);
        } else {
            obj = _leaf_work_n(fileinfo, nitems, dims, vr, NULL, entry, form);
        }
    }
    
    return obj;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *_indirect_work(PP_file *fileinfo, char *outtype,
				long nitems, dimdes *dims, void *vr,
				PP_form *form, int need_sequence)
{
    long i, mitems;
    char *litype;
    char *pvr;
    PyObject *obj, *item;
    PDBfile *file;

    PyObject *(*hook)(PP_file *fileinfo,
                      char *outtype,
                      long nitems,
                      dimdes *dims,
                      void *vr,
                      PP_form *form,
                      int need_sequence);

    file  = fileinfo->file;
    litype = CSTRSAVE(outtype);
    SC_dereference(litype);

    if (_PD_indirection(litype)) {
        hook = _indirect_work;
    } else {
        hook = _leaf_work;
    }

    if (nitems == 1) {
        pvr = DEREF(vr);
        if (pvr == NULL) {
            Py_INCREF(Py_None);
            obj = Py_None;
        } else {
            mitems = _PD_number_refd(NULL, NULL, NULL, pvr,
				     litype, file->host_chart);
            if (mitems == -1L) {
                PP_error_set_user(NULL,
                                  "CAN'T GET POINTER LENGTH ON %s - _PP_WR_SYMENT",
                                  litype);
            }
            
            if (mitems == -2L) {
                PP_error_set_user(NULL,
                                  "UNKNOWN TYPE %s - _PD_WR_SYMENT",
                                  litype);
            }
            
            obj = hook(fileinfo, litype, mitems, NULL, pvr, form, need_sequence);
/*            CFREE(pvr) */
        }
    } else {
        switch (form->array_kind) {
        case AS_LIST:
            obj = PyList_New(nitems);
            break;
        case AS_TUPLE:
            obj = PyTuple_New(nitems);
            break;
/*        default:*/
        }
        for (i=0; i< nitems; i++) {
            pvr = DEREF(vr);
            if (pvr == NULL) {
                Py_INCREF(Py_None);
                item = Py_None;
            } else {
                mitems = _PD_number_refd(NULL, NULL, NULL, pvr,
					 litype, file->host_chart);
                if (mitems == -1L) {
                    PP_error_set_user(NULL,
                                      "CAN'T GET POINTER LENGTH ON %s - _PP_WR_SYMENT",
                                      litype);
                }
                if (mitems == -2L) {
                    PP_error_set_user(NULL,
                                      "UNKNOWN TYPE %s - _PD_WR_SYMENT",
                                      litype);
                }
                item = hook(fileinfo, litype, mitems, NULL, pvr, form, need_sequence);
/*                CFREE(pvr) */
            }

            
            switch (form->array_kind) {
            case AS_LIST:
                PyList_SET_ITEM(obj, i, item);
                break;
            case AS_TUPLE:
                PyTuple_SET_ITEM(obj, i, item);
                break;
/*            default: */
            }
            vr = (void *) (((char **) vr) + 1);
        }
    }

    CFREE(litype);

    return obj;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *_PP_wr_syment(PP_file *fileinfo, char *outtype, dimdes *dims,
                        long nitems, void* vr, PP_form *form)
{
    int need_sequence;
    PyObject *obj;

    if (_PD_indirection(outtype)) {
        obj = _indirect_work(fileinfo, outtype, nitems, dims, vr, form, TRUE);
    } else {
        need_sequence = FALSE;
        obj = _leaf_work(fileinfo, outtype, nitems, dims, vr, form, need_sequence);
    }

    return obj;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
