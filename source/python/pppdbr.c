/*
 * PPPDBR.C
 *
 * Source Version: 9.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pdbmodule.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PP_read_data - allocate memory for a variable and read a from a file.
 * adapted from SX_read_numeric_data
 */

PyObject *_PP_read_data(PP_file *fileinfo, char *name, char *intype,
                        int nd, long *ind, PP_form *form)
{
    int iok;
    long number;
    char *type;
    syment *ep;
    defstr *dp;
    dimdes *dims;
    SC_address addr;
    PDBfile *file;
    PyObject *rv;
    PY_defstr *dpobj;

    rv           = NULL;
    addr.memaddr = NULL;
    ep           = NULL;
    file         = fileinfo->file;

    name = _PD_expand_hyper_name(file, name);
    if (name == NULL) {
        PP_error_set(PP_error_user, NULL,
                     "error from _PD_expand_hyper_name");
        goto done;
    }

    ep = _PD_effective_ep(file, name, TRUE, NULL);
    if (ep == NULL) {
        PP_error_set(PP_error_user, NULL, "Can not find variable %s\n%s", name, PD_get_error());
        goto done;
    }
    
    type   = PD_entry_type(ep);
    number = PD_entry_number(ep);
    dims = PD_entry_dimensions(ep);
    
    if (file->virtual_internal) {
        addr.memaddr = NULL;
        number = 0;
    } else {
        addr.memaddr = _PD_alloc_entry(file, type, number);
        
        if (nd == 0 && intype == NULL) {
            iok = PD_read(file, name, addr.memaddr);
        } else if (intype == NULL) {
            _PD_rl_dimensions(dims);
            dims = PP_ind_to_dimdes(nd, ind);
            number = _PD_comp_num(dims);
            iok = PD_read_alt(file, name, addr.memaddr, ind);
        } else if (nd == 0) {
            iok = PD_read_as(file, name, intype, addr.memaddr);
            type = intype;
        } else { 
            _PD_rl_dimensions(dims);
            dims = PP_ind_to_dimdes(nd, ind);
            number = _PD_comp_num(dims);
            type = intype;
            iok = PD_read_as_alt(file, name, intype, addr.memaddr, ind);
        }
        if (iok == FALSE) {
            PP_error_set(PP_error_user, NULL, PD_get_error());
            return NULL;
        }
    }

    SC_mark(addr.memaddr, 1);
    if (form->array_kind == AS_PDBDATA) {
        dpobj = _PY_defstr_find_singleton(type, NULL, fileinfo);
        if (dpobj == NULL)
            return NULL;
        dp = dpobj->pyo;
    
        rv = (PyObject *) PP_pdbdata_newobj(NULL, addr.memaddr, type, number,
                                            dims, dp, fileinfo, dpobj, NULL);
    } else {
        rv = _PP_wr_syment(fileinfo, type, dims, number, addr.memaddr, form);
    }

 done:
    if (addr.memaddr != NULL) {
        (void) _PP_rel_syment(file->host_chart, addr.memaddr, number, type);
        CFREE(addr.memaddr);
    }
    CFREE(name);
    _PD_rl_syment_d(ep);
    
    return rv;
}

/*--------------------------------------------------------------------------*/
