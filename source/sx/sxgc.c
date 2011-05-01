/*
 * SXGC.C - garbage collection routines for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GC_DATA - garbage collect a variable */

void _SX_gc_data(PDBfile *file, void *vr, long nitems, char *type)
   {

/* if the type is an indirection, follow the pointer */
    if (_PD_indirection(type))
       _SX_gc_indirection(file, (char **) vr, nitems, type);
    else
       _SX_gc_leaf(file,  vr, nitems, type);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GC_INDIRECTION - GC an item which is indirectly referenced
 *                    - i.e. a pointer
 */

void _SX_gc_indirection(PDBfile *file, char **vr, long nitems, char *type)
   {long i, ditems;
    char *dtype, *bf;

    dtype = PD_dereference(CSTRSAVE(type));

    for (i = 0L; i < nitems; i++, vr++)
        {ditems = _PD_number_refd(DEREF(vr), dtype, file->host_chart);

         if (ditems == -1L)
            {bf = SC_dsnprintf(FALSE,
			       "CAN'T GET POINTER LENGTH ON %s - _SX_GC_INDIRECTION",
			       dtype);
             SS_error(bf, SS_null);};

         if (ditems == -2L)
            {bf = SC_dsnprintf(FALSE,
			       "UNKNOWN TYPE %s - _SX_GC_INDIRECTION",
			       dtype);
             SS_error(bf, SS_null);};

/* if the type is an indirection, follow the pointer */
	 _SX_gc_data(file, (char **) DEREF(vr), ditems, dtype);
	 
	 CFREE(DEREF(vr));};

    CFREE(dtype);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GC_LEAF - GC indirect members of leaf */

void _SX_gc_leaf(PDBfile *file, char *vr, long nitems, char *type)
   {long ii, sz;
    defstr *defp;
    memdes *desc, *mem_lst;
    char *svr;
    SS_psides *si = &_SS_si;

    if ((defp = PD_inquire_host_type(file, type)) == NULL)
       SS_error("VARIABLE NOT IN STRUCTURE CHART - _SX_GC_LEAF",
                SS_mk_string(si, type));

    else
       {mem_lst = defp->members;
	if ((mem_lst = defp->members) != NULL)
	   {sz  = defp->size;
	    svr = vr;
	    for (ii = 0L; ii < nitems; ii++, svr += sz)
	        {if (pdb_wr_hook != NULL)
		    mem_lst = (*pdb_wr_hook)(file, svr, defp);

		 for (desc = mem_lst; desc != NULL; desc = desc->next)
		     {if (_PD_indirection(desc->type))
			 _SX_gc_indirection(file,
					    (char **) (svr + desc->member_offs),
					    desc->number, desc->type);};};};};

/* CFREE(vr); */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/



