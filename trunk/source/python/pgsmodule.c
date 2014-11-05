/*
 * PGSMODULE.C
 *
 * include cpyright.h
 */

#define _pgs_MODULE

#include "pgsmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static PyMethodDef
 PP_methods[] = {
{"print_controls", (PyCFunction)PP_print_controls, PY_ARG_KEY, PP_print_controls_doc},
{"zero_space", (PyCFunction)PP_zero_space, PY_ARG_KEY, PP_zero_space_doc},
{"alloc", (PyCFunction)PP_alloc, PY_ARG_KEY, PP_alloc_doc},
{"realloc", (PyCFunction)PP_realloc, PY_ARG_KEY, PP_realloc_doc},
{"cfree", (PyCFunction)PP_cfree, PY_ARG_KEY, PP_cfree_doc},
{"sfree", (PyCFunction)PP_cfree, PY_ARG_KEY, PP_cfree_doc},
{"mem_print", (PyCFunction)PP_mem_print, PY_ARG_KEY, PP_mem_print_doc},
{"mem_trace", (PyCFunction)PP_mem_trace, METH_NOARGS, PP_mem_trace_doc},
{"reg_mem", (PyCFunction)PP_reg_mem, PY_ARG_KEY, PP_reg_mem_doc},
{"dereg_mem", (PyCFunction)PP_dereg_mem, PY_ARG_KEY, PP_dereg_mem_doc},
{"mem_lookup", (PyCFunction)PP_mem_lookup, PY_ARG_KEY, PP_mem_lookup_doc},
{"mem_monitor", (PyCFunction)PP_mem_monitor, PY_ARG_KEY, PP_mem_monitor_doc},
{"mem_chk", (PyCFunction)PP_mem_chk, PY_ARG_KEY, PP_mem_chk_doc},
{"is_score_ptr", (PyCFunction)PP_is_score_ptr, PY_ARG_KEY, PP_is_score_ptr_doc},
{"arrlen", (PyCFunction)PP_arrlen, PY_ARG_KEY, PP_arrlen_doc},
{"mark", (PyCFunction)PP_mark, PY_ARG_KEY, PP_mark_doc},
{"ref_count", (PyCFunction)PP_ref_count, PY_ARG_KEY, PP_ref_count_doc},
{"set_count", (PyCFunction)PY_set_count, PY_ARG_KEY, PY_set_count_doc},
{"permanent", (PyCFunction)PP_permanent, PY_ARG_KEY, PP_permanent_doc},
{"arrtype", (PyCFunction)PP_arrtype, PY_ARG_KEY, PP_arrtype_doc},
{"pause", (PyCFunction)PP_pause, METH_NOARGS, PP_pause_doc},
{"set_io_hooks", (PyCFunction)PY_set_io_hooks, PY_ARG_KEY, PY_set_io_hooks_doc},
{"setform", (PyCFunction)PP_setform, PY_ARG_KEY, PP_setform_doc},
{"make_set_1d", (PyCFunction)PP_make_set_1d, PY_ARG_KEY, PP_make_set_1d_doc},
{"make_ac_set", (PyCFunction)PP_make_ac_set, PY_ARG_KEY, PP_make_ac_set_doc},
{"open_vif", (PyCFunction)PY_open_vif, PY_ARG_KEY, PY_open_vif_doc},
{"getdefstr", (PyCFunction)PP_getdefstr, PY_ARG_KEY, PP_getdefstr_doc},
{"gettype", (PyCFunction)PP_gettype, PY_ARG_KEY, PP_gettype_doc},
{"getfile", (PyCFunction)PP_getfile, PY_ARG_KEY, PP_getfile_doc},
{"getdata", (PyCFunction)PP_getdata, PY_ARG_KEY, PP_getdata_doc},
{"getmember", (PyCFunction)PP_getmember, PY_ARG_KEY, PP_getmember_doc},
{"unpack", (PyCFunction)PP_unpack, PY_ARG_KEY, PP_unpack_doc},
{"make_graph_1d", (PyCFunction)PP_make_graph_1d, PY_ARG_KEY, PP_make_graph_1d_doc},
{"make_graph_r2_r1", (PyCFunction)PP_make_graph_r2_r1, PY_ARG_KEY, PP_make_graph_r2_r1_doc},
{"set_line_info", (PyCFunction)PY_set_line_info, PY_ARG_KEY, PY_set_line_info_doc},
{"set_tds_info", (PyCFunction)PY_set_tds_info, PY_ARG_KEY, PY_set_tds_info_doc},
{"set_tdv_info", (PyCFunction)PY_set_tdv_info, PY_ARG_KEY, PY_set_tdv_info_doc},
{"make_image", (PyCFunction)PP_make_image, PY_ARG_KEY, PP_make_image_doc},
{"get_processor_number", (PyCFunction)PP_get_processor_number, METH_NOARGS, PP_get_processor_number_doc},
{"get_number_processors", (PyCFunction)PP_get_number_processors, METH_NOARGS, PP_get_number_processors_doc},
{"iso_limit", (PyCFunction)PP_iso_limit, PY_ARG_KEY, PP_iso_limit_doc},
{NULL,   (PyCFunction)NULL, 0, NULL}            /* sentinel */
};

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PY_PGSMODULE_CONST - add BLANG generated constant bindings
 *                     - for PGS module
 */

static int _PY_pgsmodule_const(PyObject *m)
   {int ne;
    extern int PY_add_score_enum(PyObject *m);
    extern int PY_add_pml_enum(PyObject *m);
    extern int PY_add_pdb_enum(PyObject *m);
    extern int PY_add_pgs_enum(PyObject *m);

    ne = 0;

    ne += PY_add_score_enum(m);
    ne += PY_add_pml_enum(m);
    ne += PY_add_pdb_enum(m);
    ne += PY_add_pgs_enum(m);

    return(ne);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PGS - initialization function for the module
 *          - NOTE: MUST be called init_pgs
 */

static char pgs_module_documentation[] = "";

PY_MOD_BEGIN(_pgs, pgs_module_documentation, PP_methods)
   {int ne;
    PyObject *d;

    d = PyModule_GetDict(m);
    if (d == NULL)
       PY_MOD_RETURN_ERR;

    ne = PY_setup_pact(m, d);
    if (ne > 0)
       PY_MOD_RETURN_ERR;
    
    ne += _PY_pgsmodule_const(m);
    if (ne > 0)
       PY_MOD_RETURN_ERR;

    ne += PY_init_pgs(m, d);
    if (ne > 0)
       PY_MOD_RETURN_ERR;

    PY_DEF_GETSET(PG_device, "dev");
    PY_DEF_GETSET(PG_palette, "pal");
    PY_DEF_GETSET(PG_image, "im");
    PY_DEF_GETSET(PG_graph, "data");

    PY_MOD_END(_pgs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
