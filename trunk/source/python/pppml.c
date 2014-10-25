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
