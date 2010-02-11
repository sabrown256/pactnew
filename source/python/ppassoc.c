/*
 * PPASSOC.C
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PPassoc_from_ptr - create an assoc object from a pointer.
 *                  - getter function.
 */

PyObject *PP_assoc_from_ptr(pcons *alist)
{
    PP_assocObject *self;

    self = PyObject_NEW(PP_assocObject, &PP_assoc_Type);
    if (self == NULL)
        return NULL;
    self->alist = alist;
    SC_mark(alist, 1);

    return (PyObject *) self;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_assoc_extractor - find the pcons from from an assoc object.
 *                   - extractor function.
 */

int PP_assoc_extractor(PyObject *obj, void *ptr)
{
    int ok;

    ok = 1;
    
    if (obj == Py_None) {
        *(void **) ptr = NULL;
    } else if (obj->ob_type == &PP_assoc_Type) {
        PP_assocObject *work;
        work = (PP_assocObject *) obj;
        *(pcons **) ptr = work->alist;
    } else {
        PyErr_SetString(PyExc_TypeError, "None or a pdb.assoc is required");
        ok = 0;
    }

    return ok;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#if 0
/* _PP_pack_assoc - */

static int _PP_pack_assoc(void *p, PyObject *v, long nitems, PP_types tc)
{
    PP_error_set(PP_error_internal, NULL, "_PP_pack_assoc");
    return -1;
}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_unpack_assoc - */

PyObject *_PP_unpack_assoc(void *p, long nitems)
{
    int i, ierr;
    char *name;
    PyObject *dict, *item;
    pcons *alist, *pa, *c;

    ierr = 0;
    alist = (pcons *) p;
    
    dict = PyDict_New();
    if (dict == NULL)
        return NULL;

    for (pa = alist, i = 0;
         pa != NULL;
         pa = (pcons *) pa->cdr, i++) {
        c = (pcons *) pa->car;
        name = (char *) c->car;
        item = PP_unpack_hashtab_haelem(c->cdr_type, c->cdr);
        if (item == NULL) {
            ierr = -1;
            break;
        }
        ierr = PyDict_SetItemString(dict, name, item);
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

/* PP_update_assoc - update an assoc list from a dictionary.
 *   Note: assoc also support the dictionary protocol.
 */

int PP_update_assoc(pcons **in, PyObject *dict)
{
    int i;
    int nkeys;
    int err;
    char *keyname;
    char *ptype;
    void *vr;
    PP_descr *descr;
    pcons *alist;
    PyObject *keys, *keyitem, *value;

    descr = NULL;
    value = NULL;
    
    alist = *in;
    err = 0;
    keys = PyMapping_Keys(dict);
    if (keys == NULL)
        return -1;
    nkeys = PyList_GET_SIZE(keys);
    for (i = 0; i < nkeys; i++) {
        keyitem = PyList_GET_ITEM(keys, i);
        keyname = PyString_AS_STRING(keyitem);
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

        alist = SC_add_alist(alist, keyname, ptype, vr);
        if (alist == NULL) {
            err = -1;
            break;
        }

        SFREE(ptype); /* SC_add_alist copies */
        _PP_rl_descr(descr);
        descr = NULL;
        Py_XDECREF(value);
        value = NULL;
    }

    *in = alist;
    if (descr != NULL)
        _PP_rl_descr(descr);
    Py_XDECREF(value);
    Py_DECREF(keys);

    return err;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
