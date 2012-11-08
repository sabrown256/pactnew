/*
 * PPPML.C - PML PYTHON bindings
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *PPfield_from_ptr(PM_field *data)
{
    PP_fieldObject *self;

    self = PyObject_NEW(PP_fieldObject, &PP_field_Type);
    if (self == NULL)
        return NULL;
    self->opers = data;

    return (PyObject *) self;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *PPtopology_from_ptr(PM_mesh_topology *data)
{
    PP_mesh_topologyObject *self;

    self = PyObject_NEW(PP_mesh_topologyObject, &PP_mesh_topology_Type);
    if (self == NULL)
        return NULL;
    self->mt = data;

    return (PyObject *) self;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *PPset_from_ptr(PM_set *data)
{
    PP_setObject *self;

    self = PyObject_NEW(PP_setObject, &PP_set_Type);
    if (self == NULL)
        return NULL;
    self->set = data;

    return (PyObject *) self;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PyObject *PPmapping_from_ptr(PM_mapping *data)
{
    PP_mappingObject *self;

    self = PyObject_NEW(PP_mappingObject, &PP_mapping_Type);
    if (self == NULL)
        return NULL;
    self->map = data;

    return (PyObject *) self;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MAPPING_EXTRACTOR - extract a mapping from an object */

int mapping_extractor(PyObject *obj, void *arg)
{
    int ret = 1;

    if (obj == Py_None) {
        *(void **) arg = NULL;
    } else if (PP_mapping_Check(obj)) {
        PP_mappingObject *self;
        self = (PP_mappingObject *) obj;
        *(PM_mapping **) arg = self->map;
    } else {
        ret = 0;
    }

    return ret;
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
