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

#if 0
static long
    call_ptr = 0L,
    call_stack[1000],
    lval_ptr = 0L,
    str_ptr = 0L;

static SC_address
    lval_stack[1000];

static char
   *str_stack[1000];
#endif

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
#if 0
/*--------------------------------------------------------------------------*/

/* _PP_WR_SYMENT - 
 *               - it is written in a continuation passing
 *               - style so that PDB has control over the stack and
 *               - isn't blowing out the execution stack for long linked
 *               - lists
 */

PyObject *_PP_wr_syment(PP_file *fileinfo,
                        char *outtype,
                        dimdes *dims,
                        long nitems, void* vr, PP_form *form)
{
    int dst, size, ierr;
    long ii;                    /* index of current structure member */
    long i;
    char *pv, *litype, *ttype, *svr, **lvr;
    defstr *dp;
    memdes *desc, *mem_lst;
    PP_type_entry *entry;
    PyObject *obj, *obj1;
    int need_array;
    PDBfile *file;
    
    long call_ptr = 0L, call_stack[1000], lval_ptr = 0L, str_ptr = 0L;

    SC_address lval_stack[1000];

    char *str_stack[1000];

    i = 0;
    ii = 0;
    size = 0;
    svr = NULL;
    desc = NULL;
    mem_lst = NULL;
    entry = NULL;
    obj = NULL;
    obj1 = NULL;
    
    call_ptr = 0L;
    lval_ptr = 0L;
    str_ptr  = 0L;
    litype   = NULL;
    file     = fileinfo->file;

    SAVE_S(litype, outtype);
    SC_mark(litype, 1);

    call_stack[call_ptr++] = DONE;
    dst = _PD_indirection(litype) ? INDIRECT : LEAF;

#if 1
    if (nitems > 1)
        need_array = TRUE;
    else
        need_array = FALSE;
#else
    need_array = FALSE;
    if (dst == LEAF) {
        if (nitems == 1)
            need_array = FALSE;
    } else {
    }
/*    nitems = PD_entry_number(ep); */
#endif
    pv = (char *) vr;
    lvr = (char **) pv;

/* some AIX compilers will erroneously take the default case if
 * this is terminated with a semicolon
 */
    START
            
/* NOTE: count on this being right and _PD_effective_ep will handle all
 * issues about partial reads across discontiguous blocks by correctly
 * making an effective syment for which this logic works!!!!!!
 */
    case LEAF:
        /* read a basic type */
        entry = PP_inquire_type(fileinfo, litype);
        if (entry != NULL) {
            obj =
                PP_copy_mem_to_obj(file, pv, dims, nitems, litype, entry, form, FALSE);
            if (obj == NULL)
                return NULL;
            DEBUG_WR("LEAF", litype, 0, obj);
            GO_CONT;
        }

#if 0
        if (form->want_pdbdata) {
            obj =
                (PyObject *) PP_scoredata_New(pv, litype, nitems, dims, file);
            if (obj == NULL)
                return NULL;
            DEBUG_WR("LEAF", litype, 0, obj);
            GO_CONT;
        }
#endif
        
/* the host type must be used to get the correct member offsets for the
 * in memory copy - the file ones might be wrong!!
 */
        dp = PD_inquire_host_type(file, litype);
        if (dp == NULL) {
            PP_error_set_user(NULL, "Unknown type: %s", litype);
            return NULL;
        }

        mem_lst = dp->members;

        if (mem_lst == NULL) {
            /* If we get here with a non-derived type, it is an error 
             * because it should of been caught at the start of LEAF.
             */
            PP_error_set_user(NULL,
                              "No handler for primitive type: %s", litype);
            return NULL;
        }
#if 0
        XXX - lat if (PD_gs.read != NULL)
            mem_lst = (*PD_gs.read) (dp->members);
#endif

        if (need_array) {
            obj = PyList_New(nitems);       /* new reference */
            DEBUG_WR("LEAF", litype, 0, obj);
        }

        /* compute number of entries in struct */
        for (ii = 0, desc = mem_lst; desc != NULL; ii++, desc = desc->next) ;
        
        /* for an array of structs read the indirects for each array element */
        size = dp->size;
        svr = pv;
        i = 0L;

    case LEAF_ITEM:
        if (i >= nitems) {
            DEBUG_WR("LEAF_ITEM", litype, nitems, obj);
            DEBUG_WR("LEAF_ITEM", "obj1", nitems, obj1);
            /*      obj = obj1; */
            GO_CONT;
        }
#if 0
        if (form->want_pdbdata) {
            ep = _PD_mk_syment(litype, 1L /* nitems */ , 0L, NULL,
                               NULL /* dims */ );
            obj1 = PP_scoredata_New(pv, ep, file, NULL);
            if (obj1 == NULL)
                return NULL;
            if (nitems > 1) {
                ierr = PyList_SetItem(obj, i, obj1);        /* steals reference */
                if (ierr == -1)
                    return NULL;
            } else {
                obj = obj1;
            }
            i++;
            GO(LEAF_ITEM);
    }
#endif
        switch (form->struct_kind) {
        case AS_TUPLE:
            obj1 = PyTuple_New(ii);
            break;
        case AS_DICT:
            obj1 = PyDict_New();
            break;
        default:
            PP_error_set(PP_error_internal, NULL,
                         "Unexpected value of form->array_struct in _PP_wr_syment: %d",
                         form->array_kind);
            return NULL;
        }
        SAVE_I(need_array);
        SAVE_P(obj);
            
        DEBUG_WR("LEAF_ITEM - SAVE", "obj1", i, obj1);
        desc = mem_lst;
        ii = 0L;
        
    case LEAF_INDIR:               /* LEAF_MEMBER */
        if (desc == NULL) {
            DEBUG_WR("LEAF_INDR", litype, i, obj1);
            RESTORE_P(PyObject, obj);
            RESTORE_I(need_array);
            if (need_array) {
                ierr = PyList_SetItem(obj, i, obj1);        /* steals reference */
                if (ierr == -1)
                    return NULL;
            } else {
                obj = obj1;
            }
            DEBUG_WR("SetItem", "obj", 0, obj);
            i++;
            svr += size;
            GO(LEAF_ITEM);
        }

        PP_CAST_TYPE(ttype, desc, svr + desc->member_offs, svr, Py_None, NULL);

        SAVE_P(obj1);
        SAVE_I(i);
        SAVE_I(ii);
        SAVE_I(size);
        SAVE_P(mem_lst);
        SAVE_P(desc);
        SAVE_P(svr);
        SAVE_P(pv);
        SAVE_S(litype, ttype);
        SC_mark(litype, 1);
        SAVE_I(nitems);
        SAVE_I(need_array);

        pv = svr + desc->member_offs;
        lvr = (char **) pv;
        nitems = desc->number;
        dims = desc->dimensions;

        if (nitems > 1) {
            need_array = TRUE;
        } else {
            need_array = FALSE;
        }

        SET_CONT(LEAF_RET);

    case LEAF_RET:
        DEBUG_WR("RESTORE", "obj", 0, obj);
        RESTORE_I(need_array);
        RESTORE_I(nitems);
        RESTORE_S(litype);
        RESTORE_P(char, pv);
        RESTORE_P(char, svr);
        
        RESTORE_P(memdes, desc);
        RESTORE_P(memdes, mem_lst);
        RESTORE_I(size);
        RESTORE_I(ii);
        RESTORE_I(i);
        RESTORE_P(PyObject, obj1);

        switch (form->struct_kind) {
        case AS_TUPLE:
            ierr = PyTuple_SetItem(obj1, ii, obj);      /* steals reference */
            break;
        case AS_DICT:
            ierr = PyDict_SetItemString(obj1, desc->name, obj);      /* steals reference */
            break;
        default:
            PP_error_set(PP_error_internal, NULL,
                         "Unexpected value of form->array_struct in _PP_wr_syment: %d",
                         form->array_kind);
            ierr = -1;
        }
        if (ierr == -1)
            /* XXX - Cleanup? */
            return NULL;
        DEBUG_WR("SetItem", "obj1", 0, obj1);

        desc = desc->next;
        ii++;
        GO(LEAF_INDIR);

    case INDIRECT:
        SAVE_S(litype, litype);
        SC_mark(litype, 1);
        SC_dereference(litype);
#if 0
        entry = PP_inquire_type(fileinfo, litype);
        if (entry != NULL) {
            nitems = _PD_number_refd(DEREF(pv), litype, file->host_chart);
            obj = PP_copy_mem_to_obj(file, DEREF(pv), dims, nitems, litype,
                                     entry, form, TRUE);
            RESTORE_S(litype);
            if (obj == NULL)
                return NULL;
            GO_CONT;
            DEBUG_WR("LEAF", litype, 0, obj);
        }
#endif

        /* XXXX - handle form->want_pdbdata here? */
#if 0
        if (form->want_pdbdata) {
            strcpy(basetype, litype);
            strtok(basetype, " *");
            entry = PP_inquire_type(fileinfo, basetype);
            if (entry == NULL) {
#if 0
                if (DEREF(pv) == NULL) {
                    nitems = 0L;
                } else {
                    nitems =
                        _PD_number_refd(DEREF(pv), litype, file->host_chart);
                }
#endif
                RESTORE_S(litype);
                obj =
                    (PyObject *) PP_scoredata_New(pv, litype, nitems, dims,
                                                  file);
                if (obj == NULL)
                    return NULL;
                DEBUG_WR("INDIRECT", litype, 0, obj);
                GO_CONT;
            }
        }
#endif
        if (need_array) {
            obj1 = PyList_New(nitems);      /* new reference */
        } else {
            obj1 = NULL;
        }
#if 0
        need_array = TRUE;
#endif
        entry = PP_inquire_type(fileinfo, litype);
        SAVE_P(pv);
        lvr = (char **) pv;
        i = 0L;
        
    case INDIR_ITEM:
        if (i >= nitems) {
            RESTORE_P(char, pv);
            
            RESTORE_S(litype);
            DEBUG_WR("INDIR_ITEM", litype, nitems, obj);
            if (need_array) {
                obj = obj1;
                DEBUG_WR("INDIR_ITEM", "obj1", nitems, obj1);
            }
            GO_CONT;
        }

        SAVE_I(i);
        SAVE_P(lvr);
        SAVE_P(pv);
        SAVE_I(nitems);
        SAVE_I(need_array);
        SAVE_P(entry);
        SAVE_P(obj1);
#if 1
        lvr = &lvr[i];
        pv = *lvr;
#else
        pv = DEREF(pv);
#endif
        if (pv == NULL) {
            obj = Py_None;
            Py_INCREF(obj);
            GO(INDIR_RET);
        }

        nitems = _PD_number_refd(pv, litype, file->host_chart);
        if (nitems < 0) {
            PP_error_set_user(NULL,
                              "error from _PP_number_refd called from _PP_wr_syment, type %s",
                              litype);
            return NULL;
        }

        /* indirects are always treated as an array, even an array of 1 */
        need_array = TRUE;
        
        if (entry != NULL) {
            obj =
                PP_copy_mem_to_obj(file, pv, NULL, nitems, litype, entry, form, TRUE);
            if (obj == NULL)
                return NULL;
            GO(INDIR_RET);
            DEBUG_WR("LEAF", litype, 0, obj);       /* recursive with wr_syment ! */
        }

        SET_CONT(INDIR_RET);

    case INDIR_RET:
        RESTORE_P(PyObject, obj1);
        DEBUG_WR("RESTORE", "obj", 0, obj1);
        RESTORE_P(PP_type_entry, entry);
        RESTORE_I(need_array);
        RESTORE_I(nitems);
        RESTORE_P(char, pv);
        RESTORE_P(char *, lvr);
        
        RESTORE_I(i);
        
        if (need_array) {
            ierr = PyList_SetItem(obj1, i, obj);    /* steals reference */
            if (ierr == -1)
                return NULL;
        }
        DEBUG_WR("SetItem", litype, 0, obj);
        
        i++;
        pv++;
        
        GO(INDIR_ITEM);

    case DONE:
        RESTORE_S(litype);
        
        return (obj);
        
        FINISH("_PP_WR_SYMENT", NULL);
}

/*--------------------------------------------------------------------------*/
#else
/*--------------------------------------------------------------------------*/
static PyObject *_leaf_work(
    PP_file *fileinfo,
    char *outtype,
    long nitems,
    dimdes *dims,
    void *vr,
    PP_form *form,
    int need_sequence);

static PyObject *_indirect_work(
    PP_file *fileinof,
    char *outtype,
    long nitems,
    dimdes *dims,
    void *vr,
    PP_form *form,
    int need_sequence);

/*--------------------------------------------------------------------------*/

/*
 * If a ctor is provided, then create a dictionary and call ctor.
 */

static PyObject *_leaf_derived(
    PP_file *fileinfo,
    defstr *dp,
    void *vr,
    PP_form *form,
    PP_type_entry *entry,
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

static PyObject *_leaf_work_1d(
    PP_file *fileinfo,
    long nitems,
    void *vr,
    defstr *dp,
    PP_type_entry *entry,
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

static PyObject *_leaf_work_n(
    PP_file *fileinfo,
    long nitems,
    dimdes *dims,
    void *vr,
    defstr *dp,
    PP_type_entry *entry,
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

static PyObject *_leaf_work(
    PP_file *fileinfo,
    char *outtype,
    long nitems,
    dimdes *dims,
    void *vr,
    PP_form *form,
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

static PyObject *_indirect_work(
    PP_file *fileinfo,
    char *outtype,
    long nitems,
    dimdes *dims,
    void *vr,
    PP_form *form,
    int need_sequence)
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
            mitems = _PD_number_refd(pvr, litype, file->host_chart);
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
                mitems = _PD_number_refd(pvr, litype, file->host_chart);
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

PyObject *_PP_wr_syment(PP_file *fileinfo,
                        char *outtype,
                        dimdes *dims,
                        long nitems, void* vr, PP_form *form)
{
    int need_sequence;
    PyObject *obj;

#if 0
    if (form->array_kind == AS_PDBDATA) {
        obj = PP_pdbdata_newobj(NULL, vr, type, nitems, dims, PP_pdbdataObject *obj,
                                void *vr, char *type, long nitems,
                                dimdes *dims, defstr *dp, PP_file *fileinfo,
                                PP_defstrObject *dpobj, PyObject *parent);
    }
#endif
    
    if (_PD_indirection(outtype)) {
        obj = _indirect_work(fileinfo, outtype, nitems, dims, vr, form, TRUE);
    } else {
#if 0
        if (dims == NULL) {
            need_sequence = FALSE;
        } else {
            need_sequence = TRUE;
        }
#else
        need_sequence = FALSE;
#endif
        obj = _leaf_work(fileinfo, outtype, nitems, dims, vr, form, need_sequence);
    }

    return obj;
}

/*--------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------*/
