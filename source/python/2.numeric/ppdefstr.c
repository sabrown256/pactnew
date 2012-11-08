/*
 * PPDEFSTR.C
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"

#if 0
#define DEBUG1(_a_, _b_)  fprintf(stderr, _a_, _b_)
#else
#define DEBUG1(_a_, _b_)
#endif

/* This table is used to map between the Python Type object
 * for the defstr (with the members as descriptors) and the
 * defstr pointer.
 */
static hasharr
  *_PP_defstr_tab;
  

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int _PP_pack_defstr(void *p, PyObject *v, long nitems, PP_types tc)
{
    PP_error_set(PP_error_internal, NULL, "_PP_unpack_defstr");
    return -1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *_PP_unpack_defstr(void *p, long nitems)
{
    PyObject *rv;
    
    if (nitems == 1) {
        rv = (PyObject *) PP_defstr_newobj(NULL, (defstr *) p, PP_vif_info);
    } else {
#if 0
        rv = (PyObject *) PP_pdbdata_newobj(NULL,
                                            p, "defstr", nitems,
                                            dims, dp, fileinfo, XXX1, XXX2);
#else
        PP_error_set(PP_error_internal, NULL, "_PP_unpack_defstr");
        rv = NULL;
#endif
    }
    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PP_descr *_PP_get_defstr_descr(PP_file *fileinfo, PyObject *obj)
{
    PP_descr *descr;
    PP_defstrObject *self = (PP_defstrObject *) obj;
    defstr *dp;

    dp = _PD_type_lookup(PP_vif, PD_CHART_HOST, "DEFSTR");

    descr = CMAKE(PP_descr);
    descr->typecode = PP_UNKNOWN_I;
    descr->bpi = dp->size;
    descr->type = dp->type;
    descr->dims = NULL;
    descr->data = self->dp;
    SC_mark(descr->type, 1);
    SC_mark(descr, 1);

    return descr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

void _PP_defstr_entry(PP_file *fileinfo)
{
    PP_descr *descr;
    PP_type_entry *entry;

    descr = PP_make_descr(
        PP_UNKNOWN_I,                   /* typecode */
        "defstr",                       /* type */
        sizeof(defstr)                  /* bpi */
        );

    entry = PP_make_type_entry(
        PP_UNKNOWN_I,                    /* typecode */
        FALSE,                           /* sequence */
        descr,                           /* descr */
        &PP_defstr_Type,                 /* ob_type */
        _PP_pack_defstr,                 /* pack */
        _PP_unpack_defstr,               /* unpack */
        _PP_get_defstr_descr             /* get_descr */
        );
    
    PP_register_type(fileinfo, entry);
    PP_register_object(fileinfo, entry);

    return;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_defstr_alt - make a defstr from a list
 *   A wrapper over PD_defstr_alt.
 *
 *   memberdict is a dictionary of descriptor objects
 *   used to lookup members as python attributes.
 */

defstr *PP_defstr_alt(PDBfile *file, char *name, PyObject *members)
{
    char **list;
    defstr *dp;
    Py_ssize_t i, nmemb;
    PyObject *item;

    if (!PySequence_Check(members)) {
        PP_error_set_user(members, "members must be a sequence");
        return NULL;
    }
    
    nmemb = PySequence_Length(members);
    if (nmemb == -1)
        return NULL;

    list = CMAKE_N(char *, nmemb);

    /* create list of names */
    for (i = 0; i < nmemb; i++) {
        item = PySequence_GetItem(members, i);
        if (! PyString_Check(item)) {
            PP_error_set_user(item, "members must be strings");
            return NULL;  /* XXX cleanup */
        }
        list[i] = PyString_AS_STRING(item);
    }

    dp = PD_defstr_alt(file, name, nmemb, list);
    if (dp == NULL)
        PP_error_from_pdb();

    /* clean up list */
    CFREE(list);

    return dp;
}

/*--------------------------------------------------------------------------*/
#if 0
/*--------------------------------------------------------------------------*/

/* _PP_mk_defmap - */

PP_defmap *_PP_mk_defmap(PyTypeObject *defctor, defstr *dp)
{
    PP_defmap *map;

    map = CMAKE(PP_defmap);

    map->defctor = defctor;
    map->dp = dp;

    Py_INCREF(defctor);
    SC_mark(dp, 1);

    return map;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_rl_defmap - */

void _PP_rl_defmap(PP_defmap *map)
{
    if (SC_save_to_free(map)) {
        Py_DECREF(map->defctor);
        _PD_rl_descriptor(map->dp);
    }

    CFREE(map);
}

/*--------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int
PP_defstr_ctor_tp_init(PP_pdbdataObject *self, PyObject *args, PyObject *kwds)
{
    char *ts;
    PyObject *data, *indobj;
    PP_file *fileinfo;
    char *kw_list[] = {"data", "ind", NULL};
    int ierr, nd;
    long number, ind[MAXDIM * 3];
    dimdes *dims;
    defstr *dp;
    void *vr;
    PP_defstrObject *dpobj;

    indobj = NULL;
    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O|O", kw_list,
                                    &data, &indobj))
        return -1;

    vr   = NULL;

    DEBUG1("%s", "In PP_defstr_ctor_tp_init\n");
    
    dpobj = (PP_defstrObject *)
        SC_hasharr_def_lookup(_PP_defstr_tab, self->ob_type);
    if (dpobj == NULL) {
        PP_error_set(PP_error_internal, (PyObject *) self,
                     "Unable to locate constructor for %s",
                     self->ob_type->tp_name);
        return -1;
    }

    /* A sanity check */
    if (self->ob_type != dpobj->ctor) {
        PP_error_set(PP_error_internal, (PyObject *) self,
                     "Unable to find constructor");
        return -1;
    }
    
    dp = dpobj->dp;
    ts = dp->type;
    fileinfo = dpobj->fileinfo;

    dims = NULL;
    number = 1;
    if (indobj != NULL) {
        nd = PP_obj_to_ind(indobj, ind);
        if (nd < 0)
            goto err;
        dims = PP_ind_to_dimdes(nd, ind);
        number = _PD_comp_num(dims);
    }
    
    ierr = PP_alloc_data(ts, number, fileinfo, &vr);
    if (ierr < 0) {
        PP_error_set(PP_error_internal, (PyObject *) self,
                     "Error allocating memory");
        goto err;
    }

    ierr = _PP_rd_syment(data, fileinfo, ts, dims, number, vr);
    if (ierr == -1) goto err;
    
    self = PP_pdbdata_newobj(self, vr, ts, number, dims, dp, fileinfo, dpobj, NULL);
    
    return 0;

 err:
/* XXX   PD_free(file, ts, vr); */
    (void) _PP_rel_syment(dpobj->host_chart, vr, number, ts);
    CFREE(vr);
/* XXX  CFREE(ts); */
    _PD_rl_dimensions(dims);

    return -1;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_defstr_mk_ctor -
 *   dpobj is not complete at this point since we're in the
 *   process of making the ctor field.
 *   However, dpobj->dp is set.
 *   The _PP_defstr_tab allows dpobj to be looked up given
 *   the ctor.  This is necessary for PP_defstr_ctor_tp_init
 *   to find what type of defstr it needs to build, including which
 *   file to use.
 */

PyTypeObject *PP_defstr_mk_ctor(PP_defstrObject *dpobj)
{
    haelem *hp;
    defstr *dp;
    PyTypeObject *ctor;
    PyHeapTypeObject *et;  

    ctor = (PyTypeObject *) PyType_GenericAlloc(&PyType_Type, 0);
    
    if (ctor == NULL)
        return NULL;

    dp = dpobj->dp;

    ctor->tp_name = dp->type;
    ctor->tp_base = (PyTypeObject *) &PP_pdbdata_Type;
    Py_INCREF(&PP_pdbdata_Type);
    ctor->tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HEAPTYPE;
    ctor->tp_new = PyType_GenericNew;
    /* FIXME: why are there two assignments to ctor->tp_init ??? */
    ctor->tp_init = PP_pdbdata_Type.tp_init;
    ctor->tp_init = (initproc) PP_defstr_ctor_tp_init;
    ctor->tp_alloc = PyType_GenericAlloc;

    /* Set name in the extended type object */
    et = (PyHeapTypeObject *)ctor;
#if PYTHON_API_VERSION >= 1013
    et->ht_name = PyString_FromString(ctor->tp_name);
#else
    et->name = PyString_FromString(ctor->tp_name);
#endif

    if (PyType_Ready(ctor) < 0)
        return NULL;
    if (PP_defstr_dict(dp, ctor->tp_dict) < 0)
        return NULL;

    hp = SC_hasharr_install(_PP_defstr_tab, ctor, dpobj,
			    "PP_defstrObject", 3, -1); 
    if (hp == NULL) {
        return NULL;
    }
    
    Py_INCREF(dpobj);

    return ctor;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_rl_defstr - release a defstr object.
 *   It may not be fully formed if there was an error during creation.
 */

void _PP_rl_defstr(PP_defstrObject *self)
{

    _PD_clr_table(self->host_chart, _PD_ha_rl_defstr);
    _PD_rl_defstr(self->dp);

    /* PACT will not release the definition since it is not
     * pact memory, so use python decref
     */
    if (self->ctor != NULL) {
        SC_hasharr_remove(_PP_defstr_tab, self->ctor);
        Py_DECREF(self->ctor);
    }

    self->dp = NULL;
    self->host_chart = NULL;
    self->ctor = NULL;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_defstr_make_singleton -
 *   Return value: Borrowed reference
 */

PP_defstrObject *_PP_defstr_make_singleton(
    PP_defstrObject *self,
    char *name, PyObject *members, PP_file *fileinfo)
{
    defstr *dp;
    PDBfile *fp;

    fp       = fileinfo->file;

    dp = PD_inquire_table_type(fp->host_chart, name);
    if (dp != NULL) {
        PP_error_set_user(NULL, "type already defined - %s", name);
        return NULL;
    }

    if (PP_defstr_Check(members) == 1) {
        PP_defstrObject *dpobj;
        memdes *lst;

        dpobj = (PP_defstrObject *) members;
        lst = PD_copy_members(dpobj->dp->members);

        dp = _PD_defstr_inst(fp, name, STRUCT_KIND, lst,
			     -1, NULL, NULL, PD_CHART_HOST);
        if (dp == NULL) {
            PP_error_from_pdb();
            return NULL;
        }
    } else {
        dp = PP_defstr_alt(fp, name, members);
        if (dp == NULL)
            return NULL;
    }

    /* If self already exists, increment reference since
     * it will be stored in deftypes. Otherwise the self
     * returned by newobj will only have one reference which
     * will be used to store in deftypes.
     */
    Py_XINCREF(self);
    self = PP_defstr_newobj(self, dp, fileinfo);

    /* Install object into the file's table */
    if (self != NULL) {
        SC_hasharr_install(fileinfo->deftypes, dp->type, self,
			   "PP_defstrObject", 2, -1);
    }

    return self;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_defstr_find_singleton -
 *   Return value: Borrowed reference
 */

PP_defstrObject *_PP_defstr_find_singleton(
    char *name, defstr *dp, PP_file *fileinfo)
{
    PDBfile *fp;
    PP_defstrObject *rv;

    rv = (PP_defstrObject *) SC_hasharr_def_lookup(fileinfo->deftypes, name);
    if (rv == NULL) {
        fp = fileinfo->file;
        if (dp == NULL) {
            dp = PD_inquire_table_type(fp->host_chart, name);
            if (dp == NULL) {
                PP_error_set_user(NULL, "No such type %s in file %s",
                                  name, fp->name);
                return NULL;
            }
        }
        rv = PP_defstr_newobj(NULL, dp, fileinfo);
        if (rv != NULL) {
            /* This will use the new reference to install the item */
	   SC_hasharr_install(fileinfo->deftypes, dp->type, rv,
			      "PP_defstrObject", 2, -1);
        }
    }

    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyObject *defstr_getter(PP_pdbdataObject *self, void *closure)
{
    memdes *member = (memdes *) closure;
    PyObject *rv;
    
    rv = PP_getattr_from_defstr(self->fileinfo, self->data,
                                self->type, member->name,
                                self->nitems, (PyObject *) self);
    
    return rv;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

int defstr_setter(PP_pdbdataObject *self, PyObject *value, void *closure)
{
    int ierr;
    void *vr;
    memdes *desc = (memdes *) closure;

    vr = ((char *) self->data) + desc->member_offs;
    ierr = _PP_rd_syment(value, self->fileinfo, desc->type,
                         desc->dimensions, desc->number, vr);
    
    return ierr;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_defstr_dict - populate dictionary with desriptor objects
 *                - to access member of a defstr.*/

int PP_defstr_dict(defstr *dp, PyObject *dict)
{
    memdes *member;
    PyGetSetDef *getset;
    PyObject *descr;

    for (member = dp->members; member != NULL; member = member->next) {
        getset = PyMem_New(PyGetSetDef, 1);
        getset->name = member->name;
        getset->get = (getter) defstr_getter;
        getset->set = (setter) defstr_setter;
        getset->doc = NULL;
        getset->closure = member;
        descr = PyDescr_NewGetSet(&PP_pdbdata_Type, getset);
        if (descr == NULL)
            return -1;
        if (PyDict_SetItemString(dict, getset->name, descr) < 0)
            return -1;
        Py_DECREF(descr);
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PP_getattr_from_defstr - return an attribute for a pdbdata object.
 *     loop over all members of the structure until a name is
 *     found, then make an object out of it.
 *     If the name is not found, return NULL.
 */

PyObject *PP_getattr_from_defstr(PP_file *fileinfo, void *vr, char *type,
                                 char *name, long nitems,
                                 PyObject *parent)
{
    char *ttype, *ts;
    char *svr;
    PyObject *obj;
    defstr *dp;
    memdes *desc;
    PP_form form;
    PP_defstrObject *dpobj;

    obj = NULL;
    form = PP_global_form;

    if (fileinfo == NULL) {
        PP_error_set_user(NULL, "fileinfo is NULL");
        return NULL;
    }

    /* get base type */
    ts = CSTRSAVE(type);
    strtok(ts, " *()[");
    dpobj = _PP_defstr_find_singleton(ts, NULL, fileinfo);
    CFREE(ts);
    if (dpobj == NULL)
        return NULL;

    dp = dpobj->dp;

#if 0
    switch (PP_num_indirection(type)) {
    case 0:
        dp = (defstr *) SC_hasharr_def_lookup(file->host_chart, type);
        break;
    case 1:
        dtype = SC_dereference(CSTRSAVE(type));
        dp = (defstr *) SC_hasharr_def_lookup(file->host_chart, dtype);
        vr = DEREF(vr);
        CFREE(dtype);
        break;
    default:
        PP_error_set_user(NULL, "Too many levels of indirection: %s",
                          type);
        return NULL;
        break;
    }

    if (dp == NULL) {
        PP_error_set_user(NULL, "Type %s is not in file %s", type,
                          file->name);
        return NULL;
    }
#endif
    
    svr = vr;

    for (desc = dp->members; desc != NULL; desc = desc->next) {
        if (strcmp(desc->name, name) == 0) {

            PP_CAST_TYPE(ttype, desc, svr + desc->member_offs, svr,
                         Py_None, NULL);

            /* get base type */
            ts = CSTRSAVE(ttype);
            strtok(ts, " *()[");
            dpobj = _PP_defstr_find_singleton(ts, NULL, fileinfo);
            CFREE(ts);
            if (dpobj == NULL)
                return NULL;

            dp = dpobj->dp;

            svr += desc->member_offs;
            obj = PP_form_object(svr, ttype, desc->number, desc->dimensions,
                                 dp, fileinfo, dpobj, parent, &form);
                
#if 0
            obj =
                _PP_wr_syment(fileinfo, ttype, desc->dimensions,
                              desc->number, svr, &form);
#endif
            break;
        }
    }
    
    return obj;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_create_defstr_tab - */

void _PP_create_defstr_tab(void)
{
    _PP_defstr_tab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_ADDR_KEY, 0);
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_clr_defstr - if defstr is in fp, decrement */
 
static int _PP_clr_defstr(haelem *hp, void *arg)
{
    int ok;
    PP_defstrObject *self;
    PDBfile *fp = arg;
    
    ok = SC_haelem_data(hp, NULL, NULL, (void **) &self, FALSE);
    if ((ok == TRUE) && (self->fileinfo->file == fp)) {
        Py_DECREF(self);
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_cleanup_defstrs - Go thur all defstrs in the global table,
 * if it is in this file, delete it.
 *  _PP_defstr_tab is file static.
 */
 
void _PP_cleanup_defstrs(PDBfile *fp)
{
    int err;
    
    err = SC_hasharr_foreach(_PP_defstr_tab, _PP_clr_defstr, fp);
    SC_ASSERT(err == TRUE);

    /* XXX - test err? */
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
