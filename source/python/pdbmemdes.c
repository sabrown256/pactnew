/*
 * Source Version: 9.0
 * Software Release: LLNL-CODE-422942
 *
 * include cpyright.h
 */

#define PY_DEF_DESTRUCTOR	    PY_memdes_tp_dealloc

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

PY_memdes *PY_memdes_newobj(PY_memdes *obj, memdes *desc)
   {

    if (obj == NULL)
       {obj = (PY_memdes *) PyType_GenericAlloc(&PY_memdes_type, 0);
        if (obj == NULL)
	   return NULL;};

    obj->pyo = desc;
    SC_mark(desc, 1);

    return obj;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static void PY_memdes_tp_dealloc(PY_memdes *self)
   {

    _PD_rl_descriptor(self->pyo);

    PY_self_free(self);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static int PY_memdes_tp_init(PY_memdes *self, PyObject *args, PyObject *kwds)
   {

    return 0;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

char
 PY_memdes_doc[] = "";

PY_DEF_TYPE(memdes);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
