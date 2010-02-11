/*
 * pypdbr.c - PDB read functionality in Python
 *          - convert python objects into 'C' memory
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"
#include "pdbrw.h"

#if 0
int printobj(char *s1, char *s, long nitems, PyObject * obj);
#define DEBUG_RD(s1, s, nitems, obj)  printobj(s1, s, nitems, obj)
#else
#define DEBUG_RD(s1, s, nitems, obj)
#endif

static long
    call_ptr = 0L,
    call_stack[1000],
    lval_ptr = 0L,
    str_ptr = 0L;

static SC_address
    lval_stack[1000];

static char
   *str_stack[1000];


/*--------------------------------------------------------------------------*/

/* _PP_RD_SYMENT - from the information in the symbol table entry EP
 *               - read the entry from the PDBfile FILE into the location
 *               - pointed to by VR
 *               - at this point the things have been completely dereferenced
 *               - return the number of items successfully read
 *               - this new version is written in a continuation passing
 *               - style so that PDB has control over the stack and
 *               - isn't blowing out the execution stack for long linked
 *               - lists
 */

long _PP_rd_syment(PyObject *obj, PP_file *fileinfo,
                   char *outtype, dimdes *dims, long nitems, void *vr)
{
    int dst, size, as_obj, ierr;
    long ii;                    /* index of current structure member */
    Py_ssize_t nmembers;        /* number of members in struct */
    Py_ssize_t i;
    long bpi;
    char *pv, *litype, *ttype, *svr, **lvr;
    defstr *dp;
    memdes *desc, *mem_lst;
    PP_type_entry *entry;
    int have_dims;
    PDBfile *file;
    
    i = 0;
    ii = 0;
    size = 0;
    as_obj = 0;
    bpi = 0;
    nmembers = 0;
    svr = NULL;
    dp = NULL;
    desc = NULL;
    mem_lst = NULL;
    entry = NULL;
    file = fileinfo->file;
    
    call_ptr = 0L;
    lval_ptr = 0L;
    str_ptr  = 0L;
    litype   = NULL;

    SAVE_S(litype, outtype);

    call_stack[call_ptr++] = DONE;
    dst = _PD_indirection(litype) ? INDIRECT : LEAF;

    pv = (char *) vr;
    lvr = (char **) pv;
#if 0
    if (nitems == 1)
        have_dims = FALSE;
    else
        have_dims = TRUE;
#else
    if (dims == NULL) {
        have_dims = FALSE;
    } else {
        have_dims = TRUE;
        i = (long) PP_get_object_length(obj);
        if (i != dims->number) {
            PP_error_set_user(obj,
                              "expected sequence of %ld items",
                              dims->number);
            RESTORE_S(litype);  /* clean up */
            return -1;
        }
    }
#endif
    DEBUG_RD("START", litype, nitems, obj);

/* some AIX compilers will erroneously take the default case if
 * this is terminated with a semicolon
 */
    START
/* NOTE: count on this being right and _PD_effective_ep will handle all
 * issues about partial reads across discontiguous blocks by correctly
 * making an effective syment for which this logic works!!!!!!
 */
            
    case LEAF:
        DEBUG_RD("LEAF", litype, nitems, obj);

        /* read a basic type */
#if 1
        entry = PP_inquire_type(fileinfo, litype);
#else
        entry = PP_inquire_object(fileinfo, obj);
#endif
        if (entry != NULL && entry->typecode < PP_INSTANCE_I) {  /* primitive type */
            ierr = PP_copy_obj_to_mem(obj, fileinfo, entry, nitems, pv, dims);
            if (ierr == -1) {
                RESTORE_S(litype);  /* clean up */
                return -1;
            }
            if (ierr > 0)
                GO_CONT;
        }

/* the host type must be used to get the correct member offsets for the
 * in memory copy - the file ones might be wrong!!
 */
        dp = PD_inquire_host_type(file, litype);
        if (dp == NULL) {
            PP_error_set_user(obj, "Unknown type: %s", litype);
            return -1;
        }

        mem_lst = dp->members;

        if (mem_lst == NULL) {
            /* If we get here with a non-derived type, it is an error 
             * because it should of been caught at the start of LEAF.
             */
            PP_error_set_user(Py_None,
                              "No handler for primitive type: %s", litype);
            return -1;
        }

        if (nitems > 1) {           /* have_dims) { */
            i = (long) PP_get_object_length(obj);
            if (i != nitems) {
                PP_error_set_user(obj, "expected %d items, found %d",
                                  nitems, i);
                return -1;
            }
        }
#if 0
        if (pdb_rd_hook != NULL)
            mem_lst = (*pdb_rd_hook) (dp->members);
#endif

        /* compute number of entries in struct */
        for (nmembers = 0, desc = mem_lst; desc != NULL;
             nmembers++, desc = desc->next) ;
        
/* for an array of structs read the indirects for each array element */
        size = dp->size;
        svr = pv;
        i = 0L;
        
    case LEAF_ITEM:
        if (i >= nitems) {
            GO_CONT;
        }

        DEBUG_RD("LEAF_ITEM - SAVE", "obj", i, obj);
        SAVE_P(obj);
        if (have_dims) {
            obj = PySequence_GetItem(obj, i);
        } else {
            Py_INCREF(obj);
        }
        if (obj == NULL) {
            PP_error_set(PyExc_IndexError, obj, "No such index - %d", i);
            return -1;
        }
        DEBUG_RD("GetItem", litype, i, obj);

        desc = mem_lst;
        ii = 0L;

        if (entry != NULL) {
            as_obj = AS_OBJECT;
        } else if (PyObject_IsSubclass((PyObject *) obj->ob_type, (PyObject *) &PP_pdbdata_Type)) {
            PP_type_entry *entry2;
            PP_descr *descr;
            entry2 = PP_inquire_object(fileinfo, obj);
            descr = _PP_get_object_descr(fileinfo, obj, entry2);
            if (descr == NULL) {
                PP_error_set_user(obj, "Unable to get descriptor");
                return -1;
            }
            if (strcmp(descr->type, litype) != 0) {
                PP_error_set_user(obj,
                                  "scoredata object has wrong type %s, expected %s",
                                  descr->type, litype);
                _PP_rl_descr(descr);
                return -1;
            }
            memcpy(svr, descr->data, descr->bpi);
            _PP_rl_descr(descr);
            i++;
            svr += size;
            RESTORE_P(PyObject, obj);
            DEBUG_RD("RESTORE", "obj", 0, obj);
            GO(LEAF_ITEM);
        } else if (PySequence_Check(obj)) {
            if (nmembers != PyObject_Length(obj)) {
                PP_error_set_user(obj,
                                  "Wrong number of fields for struct type %s",
                                  litype);
                RESTORE_S(litype);  /* clean up */
                return -1;
            }
            as_obj = AS_TUPLE;
        } else if (PyMapping_Check(obj)) {
            as_obj = AS_DICT;
        } else {
            PP_error_set_user(obj,
                              "Expected sequence or mapping for struct type %s",
                              litype);
            RESTORE_S(litype);  /* clean up */
            return -1;
        }
        /* XXX - count number fields in dp and make sure we have the
           same number of fields in obj,
           make sure obj is a sequence */
        
    case LEAF_INDIR:               /* LEAF_MEMBER */
        if (desc == NULL) {
            /* Py_DECREF(obj); */
            RESTORE_P(PyObject, obj);
            DEBUG_RD("RESTORE", "obj", 0, obj);
            i++;
            svr += size;
            GO(LEAF_ITEM);
        }

        PP_CAST_TYPE(ttype, desc, svr + desc->member_offs, svr, obj, -1);

        DEBUG_RD("LEAF_INDR - SAVE", ttype, i, obj);
        SAVE_P(obj);
        switch (as_obj) {
        case AS_OBJECT:
            obj = PyObject_GetAttrString(obj, desc->name);
            break;
        case AS_DICT:
            obj = PyMapping_GetItemString(obj, desc->name);
            break;
        case AS_TUPLE:
            obj = PySequence_GetItem(obj, ii);
            break;
        default:
            obj = NULL;
        }
        if (obj == NULL) {
            RESTORE_P(PyObject, obj);
            PP_error_set_user(obj,
                              "No data for %s.%s of type %s",
                              dp->type, desc->name, ttype);
            RESTORE_S(litype);  /* clean up */
            return -1;
        }
        SAVE_I(as_obj);
        DEBUG_RD("GetItem", "obj", 0, obj);

        SAVE_I(i);
        SAVE_I(ii);
        SAVE_I(nmembers);
        SAVE_I(size);
        SAVE_P(entry);
        SAVE_P(mem_lst);
        SAVE_P(desc);
        SAVE_P(svr);
        SAVE_P(pv);
        pv = svr + desc->member_offs;
        lvr = (char **) pv;
        SAVE_S(litype, ttype);
        SAVE_I(nitems);
        SAVE_I(have_dims);
        SAVE_P(dims);
        
        nitems = desc->number;
        dims   = desc->dimensions;

        if (nitems > 1) {
            have_dims = TRUE;
        } else {
            have_dims = FALSE;
        }
        
        SET_CONT(LEAF_RET);

    case LEAF_RET:
        RESTORE_P(dimdes, dims);
        RESTORE_I(have_dims);
        RESTORE_I(nitems);
        RESTORE_S(litype);
        RESTORE_P(char, pv);
        RESTORE_P(char, svr);
        
        RESTORE_P(memdes, desc);
        RESTORE_P(memdes, mem_lst);
        RESTORE_P(PP_type_entry, entry);
        RESTORE_I(size);
        RESTORE_I(nmembers);
        RESTORE_I(ii);
        RESTORE_I(i);
        /* Py_DECREF(obj); */
        RESTORE_I(as_obj);
        RESTORE_P(PyObject, obj);
        DEBUG_RD("RESTORE", "obj", 0, obj);

        desc = desc->next;
        ii++;
        GO(LEAF_INDIR);
        
    case INDIRECT:
        DEBUG_RD("INDIRECT - SAVE", litype, nitems, obj);
        SAVE_S(litype, litype);
        SC_dereference(litype);
        entry = PP_inquire_type(fileinfo, litype);
#if 0
        /* attempt to cast obj to type litype */
        if (entry != NULL) {
            SAVE_I(nitems);
            nitems = (long) PP_get_object_length(obj);
            ierr =
                PP_get_object_data(obj, entry, nitems, (void **) lvr,
                                   PP_GC_YES);
            RESTORE_I(nitems);
            if (ierr == -1)
                return -1;
            if (ierr == 0) {
                RESTORE_S(litype);
                GO_CONT;
            }
        }
#endif

#if 0
        /* find any entry for elements of this type */
        SAVE_S(litype, litype);
        SC_dereference(litype);
        entry = PP_inquire_type(fileinfo, litype);
        RESTORE_S(litype);
#endif

        bpi = _PD_lookup_size(litype, file->host_chart);
        if (bpi == -1) {
            PP_error_set_user(obj,
                              "can't find number of bytes for type %s",
                              litype);
            return -1;
        }

        i = 0L;

    case INDIR_ITEM:
        if (i >= nitems) {
            RESTORE_S(litype);
            GO_CONT;
        };

        DEBUG_RD("INDR_ITEM - SAVE", litype, i, obj);

        SAVE_I(i);
        SAVE_P(lvr);
        SAVE_P(pv);
        SAVE_I(nitems);
        SAVE_I(have_dims);
        SAVE_I(bpi);
        SAVE_P(entry);
        SAVE_P(obj);

        if (obj == Py_None) {
            Py_INCREF(obj);
        } else if (!have_dims) {
            nitems = (long) PP_get_object_length(obj);
        } else {
            obj = PySequence_GetItem(obj, i);
            nitems = (long) PP_get_object_length(obj);
        }
        if (obj == NULL) {
            RESTORE_P(PyObject, obj);
            PP_error_set_user(obj, "No such index %d", i);
            return -1;
        }

        DEBUG_RD("GetItem", "obj", i, obj);

        if (obj == Py_None) {
            lvr[i] = NULL;
            GO(INDIR_RET);
#if 1
        } else if (entry != NULL && entry->typecode < PP_INSTANCE_I) {
            DEBUG_RD("entry", "obj", i, obj);
            ierr = PP_unpack_sequence(obj, entry,  (void **) &lvr[i], fileinfo);
            if (ierr == -1)
                return -1;
            SC_mark((void **) lvr[i], 1);
            if (ierr == 0) {
                GO(INDIR_RET);
            }
#endif
        }

        lvr = &lvr[i];

        DEREF(lvr) = pv = (char *) SC_alloc(nitems, bpi,
                                            "_PP_rd_syment:pv");
        if (pv == NULL) {
            PyErr_NoMemory();
            return -1;
        }
        SC_mark(pv, 1);

        lvr = (char **) pv;

        have_dims = TRUE;

        SET_CONT(INDIR_RET);

    case INDIR_RET:
        /* Py_DECREF(obj); */
        RESTORE_P(PyObject, obj);
        RESTORE_P(PP_type_entry, entry);
        RESTORE_I(bpi);
        RESTORE_I(have_dims);
        RESTORE_I(nitems);
        RESTORE_P(char, pv);
        RESTORE_P(char *, lvr);

        RESTORE_I(i);
        i++;
        DEBUG_RD("INDIR_RET", litype, i, obj);

        GO(INDIR_ITEM);

    case DONE:
        RESTORE_S(litype);
        
        return (0);
        
        FINISH("_PD_RD_SYMENT", -1);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int printobj(char *s1, char *s, long nitems, PyObject * obj)
{
    static FILE *outto;

    outto = stderr;
    outto = stdout;

    fprintf(outto, "%s %s %ld: ", s1, s, nitems);
    PyObject_Print(obj, outto, 0);
    fprintf(outto, "\n");

    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
