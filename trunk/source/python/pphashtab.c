/*
 * PPhasharr.C
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_pack_hashtab - */

static int _PP_pack_hashtab(void *p, PyObject *v, long nitems, PP_types tc)
{
    PP_error_set(PP_error_internal, NULL, "_PP_pack_hashtab");
    return -1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_unpack_hashtab - */

PyObject *_PP_unpack_hashtab(void *p, long nitems)
{
    int ierr;
    long i;
    char *nm, *ty;
    void *df;
    PyObject *dict, *item;
    hasharr *tab;

    ierr = 0;
    tab = (hasharr *) p;
    if (tab == NULL) {
        Py_INCREF(Py_None);
        dict = Py_None;
        return dict;
    }

    dict = PyDict_New();
    if (dict == NULL)
        return NULL;

    for (i = 0; SC_hasharr_next(tab, &i, &nm, &ty, &df); i++) {
         item = PP_unpack_hashtab_haelem(ty, df);
	 if (item == NULL) {
	    ierr = -1;
	    break;
	 }
	 ierr = PyDict_SetItemString(dict, nm, item);
	 if (ierr < 0)
	    break;
        }

    if (ierr < 0) {
        Py_DECREF(dict);
        dict = NULL;
    }
    
    return dict;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_unpack_hashtab_haelem - */

/* PyObject *PP_unpack_hashtab_haelem(haelem *hp)*/
PyObject *PP_unpack_hashtab_haelem(char *type, void *vr)
{
    intb bpi;
    inti nitems;
    dimdes *dims;
    PyObject *rv;
    PP_form form;

    form = PP_global_form;

    type = SC_dereference(CSTRSAVE(type));
    bpi = _PD_lookup_size(type, PP_vif->host_chart);
    if (bpi < 0)
        return NULL;
    nitems = SC_arrlen(vr) / bpi;
    if (nitems > 1)
        dims = _PD_mk_dimensions(0, nitems);
    else
        dims = NULL;
    rv = _PP_wr_syment(PP_vif_info, type, dims, nitems, vr, &form);
    CFREE(type);
    _PD_rl_dimensions(dims);

    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PP_descr *_PP_get_hashtab_descr(PP_file *fileinfo, PyObject *obj)
   {PP_descr *descr;
    PP_hashtabObject *self = (PP_hashtabObject *) obj;
    defstr *dp;

    dp = _PD_type_lookup(PP_vif, PD_CHART_HOST, "hasharr");
    if (dp == NULL)
       {PP_error_set_user(NULL, "PD_lookup_type failed with hasharr");
        return(NULL);};

    descr = CMAKE(PP_descr);
    descr->typecode = PP_UNKNOWN_I;
    descr->bpi = dp->size;
    descr->type = dp->type;
    descr->dims = NULL;
    descr->data = self->data;
    SC_mark(descr->type, 1);
    SC_mark(descr, 1);

    return(descr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PP_hashtab_entry(PP_file *fileinfo)
{
    PP_descr *descr;
    PP_type_entry *entry;

    descr = PP_make_descr(
        PP_UNKNOWN_I,                   /* typecode */
        "hasharr",                      /* type */
        sizeof(hasharr)                 /* bpi */
        );

    entry = PP_make_type_entry(
        PP_UNKNOWN_I,                    /* typecode */
        FALSE,                           /* sequence */
        descr,                           /* descr */
        &PP_hashtab_Type,                /* otype */
        _PP_pack_hashtab,                /* pack */
        _PP_unpack_hashtab,              /* unpack */
        _PP_get_hashtab_descr            /* get_descr */
        );
    
    PP_register_type(fileinfo, entry);
    PP_register_object(fileinfo, entry);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_update_hashtab - update a hashtab from a dictionary.
 *   Note: hashtabs also support the dictionary protocol.
 */

int PP_update_hashtab(hasharr *htab, PyObject *dict)
{
    int i;
    int nkeys;
    int err;
    char *keyname;
    char *ptype;
    void *vr;
    PP_descr *descr;
    haelem *entry;
    PyObject *keys, *keyitem, *value;

    err = 0;
    descr = NULL;
    value = NULL;
    keys = PyMapping_Keys(dict);
    if (keys == NULL)
        return -1;
    nkeys = PyList_GET_SIZE(keys);
    for (i = 0; i < nkeys; i++) {
        keyitem = PyList_GET_ITEM(keys, i);
        keyname = PY_STRING_AS_STRING(keyitem);
        value = PyMapping_GetItemString(dict, keyname);

        descr = PP_get_object_descr(PP_vif_info, value);
        if (descr == NULL) {
            err = -1;
            break;
        }

        if (descr->data == NULL) {
            err = PP_make_data(value, PP_vif_info, descr->type, descr->dims, &vr);
            if (err == -1) {
                break;
            }
        } else {
            vr = descr->data;
        }

        ptype = PP_add_indirection(descr->type, 1);
        if (ptype == NULL) {
            err = -1;
            break;
        }

        /* SC_hasharr_install will SC_mark vr */
        entry = SC_hasharr_install(htab, keyname, vr, ptype, 3, -1);
        if (entry == NULL) {
            err = -1;
            break;
        }

        _PP_rl_descr(descr);
        descr = NULL;
        Py_XDECREF(value);
        value = NULL;
    }

    if (descr != NULL)
        _PP_rl_descr(descr);
    Py_XDECREF(value);
    Py_DECREF(keys);

    return err;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

