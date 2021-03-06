/*
 * SXGC.C - garbage collection routines for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

static void
 _SX_gc_data(SS_psides *si, PDBfile *file,
	     void *vr, long ni, const char *type);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GC_INDIRECTION - GC an item which is indirectly referenced
 *                    - i.e. a pointer
 */

static void _SX_gc_indirection(SS_psides *si, PDBfile *file,
			       char **vr, long ni, const char *type)
   {long i, ditems;
    char *dtype, *bf;

    dtype = PD_dereference(CSTRSAVE(type));

    for (i = 0L; i < ni; i++, vr++)
        {ditems = _PD_number_refd(NULL, NULL, NULL, DEREF(vr),
				  dtype, file->host_chart);

         if (ditems == -1L)
            {bf = SC_dsnprintf(FALSE,
			       "CAN'T GET POINTER LENGTH ON %s - _SX_GC_INDIRECTION",
			       dtype);
             SS_error(si, bf, SS_null);};

         if (ditems == -2L)
            {bf = SC_dsnprintf(FALSE,
			       "UNKNOWN TYPE %s - _SX_GC_INDIRECTION",
			       dtype);
             SS_error(si, bf, SS_null);};

/* if the type is an indirection, follow the pointer */
	 _SX_gc_data(si, file, (char **) DEREF(vr), ditems, dtype);
	 
	 CFREE(DEREF(vr));};

    CFREE(dtype);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GC_LEAF - GC indirect members of leaf */

static void _SX_gc_leaf(SS_psides *si, PDBfile *file,
			char *vr, long ni, const char *type)
   {long ii, sz;
    char *svr;
    defstr *defp;
    memdes *desc, *mem_lst;

    if ((defp = PD_inquire_host_type(file, type)) == NULL)
       SS_error(si, "VARIABLE NOT IN STRUCTURE CHART - _SX_GC_LEAF",
                  SS_mk_string(si, type));

    else
       {mem_lst = defp->members;
	if ((mem_lst = defp->members) != NULL)
	   {sz  = defp->size;
	    svr = vr;
	    for (ii = 0L; ii < ni; ii++, svr += sz)
	        {if (PD_gs.write != NULL)
		    mem_lst = (*PD_gs.write)(file, svr, defp);

		 for (desc = mem_lst; desc != NULL; desc = desc->next)
		     {if (_PD_indirection(desc->type))
			 _SX_gc_indirection(si, file,
					    (char **) (svr + desc->member_offs),
					    desc->number, desc->type);};};};};

/* CFREE(vr); */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GC_DATA - garbage collect a variable */

static void _SX_gc_data(SS_psides *si, PDBfile *file,
			void *vr, long ni, const char *type)
   {

/* if the type is an indirection, follow the pointer */
    if (_PD_indirection(type))
       _SX_gc_indirection(si, file, (char **) vr, ni, type);
    else
       _SX_gc_leaf(si, file,  vr, ni, type);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/



