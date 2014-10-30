/*
 * PDBMODULE.C
 *
 * include cpyright.h
 */

#define _pdb_MODULE

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMethodDef
 PP_methods[] = {
{"print_controls", (PyCFunction)PP_print_controls, METH_KEYWORDS, PP_print_controls_doc},
{"zero_space", (PyCFunction)PP_zero_space, METH_KEYWORDS, PP_zero_space_doc},
{"alloc", (PyCFunction)PP_alloc, METH_KEYWORDS, PP_alloc_doc},
{"realloc", (PyCFunction)PP_realloc, METH_KEYWORDS, PP_realloc_doc},
{"cfree", (PyCFunction)PP_cfree, METH_KEYWORDS, PP_cfree_doc},
{"sfree", (PyCFunction)PP_cfree, METH_KEYWORDS, PP_cfree_doc},
{"mem_print", (PyCFunction)PP_mem_print, METH_KEYWORDS, PP_mem_print_doc},
{"mem_trace", (PyCFunction)PP_mem_trace, METH_NOARGS, PP_mem_trace_doc},
{"reg_mem", (PyCFunction)PP_reg_mem, METH_KEYWORDS, PP_reg_mem_doc},
{"dereg_mem", (PyCFunction)PP_dereg_mem, METH_KEYWORDS, PP_dereg_mem_doc},
{"mem_lookup", (PyCFunction)PP_mem_lookup, METH_KEYWORDS, PP_mem_lookup_doc},
{"mem_monitor", (PyCFunction)PP_mem_monitor, METH_KEYWORDS, PP_mem_monitor_doc},
{"mem_chk", (PyCFunction)PP_mem_chk, METH_KEYWORDS, PP_mem_chk_doc},
{"is_score_ptr", (PyCFunction)PP_is_score_ptr, METH_KEYWORDS, PP_is_score_ptr_doc},
{"arrlen", (PyCFunction)PP_arrlen, METH_KEYWORDS, PP_arrlen_doc},
{"mark", (PyCFunction)PP_mark, METH_KEYWORDS, PP_mark_doc},
{"ref_count", (PyCFunction)PP_ref_count, METH_KEYWORDS, PP_ref_count_doc},
{"set_count", (PyCFunction)PY_set_count, METH_KEYWORDS, PY_set_count_doc},
{"permanent", (PyCFunction)PP_permanent, METH_KEYWORDS, PP_permanent_doc},
{"arrtype", (PyCFunction)PP_arrtype, METH_KEYWORDS, PP_arrtype_doc},
{"pause", (PyCFunction)PP_pause, METH_NOARGS, PP_pause_doc},
{"set_io_hooks", (PyCFunction)PY_set_io_hooks, METH_KEYWORDS, PY_set_io_hooks_doc},
{"setform", (PyCFunction)PP_setform, METH_KEYWORDS, PP_setform_doc},
{"make_set_1d", (PyCFunction)PP_make_set_1d, METH_KEYWORDS, PP_make_set_1d_doc},
{"make_ac_set", (PyCFunction)PP_make_ac_set, METH_KEYWORDS, PP_make_ac_set_doc},
{"open_vif", (PyCFunction)PY_open_vif, METH_KEYWORDS, PY_open_vif_doc},
{"getdefstr", (PyCFunction)PP_getdefstr, METH_KEYWORDS, PP_getdefstr_doc},
{"gettype", (PyCFunction)PP_gettype, METH_KEYWORDS, PP_gettype_doc},
{"getfile", (PyCFunction)PP_getfile, METH_KEYWORDS, PP_getfile_doc},
{"getdata", (PyCFunction)PP_getdata, METH_KEYWORDS, PP_getdata_doc},
{"getmember", (PyCFunction)PP_getmember, METH_KEYWORDS, PP_getmember_doc},
{"unpack", (PyCFunction)PP_unpack, METH_KEYWORDS, PP_unpack_doc},
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_PDBMODULE_CONST - add BLANG generated constant bindings
 *                     - for PDB module
 */

static int _PY_pdbmodule_const(PyObject *m)
   {int ne;
    extern int PY_add_score_enum(PyObject *m);
    extern int PY_add_pml_enum(PyObject *m);
    extern int PY_add_pdb_enum(PyObject *m);

    ne = 0;

    ne += PY_add_score_enum(m);
    ne += PY_add_pml_enum(m);
    ne += PY_add_pdb_enum(m);

    return(ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PDB - initialization function for the module
 *          - NOTE: MUST be called init_pdb
 */

static char pdb_module_documentation[] = "";

PY_MOD_BEGIN(_pdb, pdb_module_documentation, PP_methods)
   {int ne;
    PyObject *d;

    d = PyModule_GetDict(m);
    if (d == NULL)
       PY_MOD_RETURN_ERR;

    ne = PY_setup_pact(m, d);
    if (ne > 0)
       PY_MOD_RETURN_ERR;
    
    ne += _PY_pdbmodule_const(m);
    if (ne > 0)
       PY_MOD_RETURN_ERR;

    ne += PY_setup_pdb(m);
    if (ne > 0)
       PY_MOD_RETURN_ERR;

/* make 'open' a synonym for 'PDBfile' */
    if (PyDict_SetItemString(d, "open", (PyObject *) &PY_PDBfile_type) < 0)
       PY_MOD_RETURN_ERR;

    PY_MOD_END(_pdb);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
