/*
 * SXPDBF.C - PDB find functionality in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

static char
 TYPE[10];

static void
 _SX_find_data(hasharr *tytab, PDBfile *file, void *vr,
	       long nitems, char *type);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_FIND_LEAF - if 'type' is a primitive type, display the data, 
 *               - otherwise, lookup the type, and display each member.
 */

static void _SX_find_leaf(hasharr *tytab, PDBfile *file, char *vr,
			  long nitems, char *type)
   {long i, sz;
    char *svr;
    defstr *defp;
    hasharr *tab;
    memdes *desc, *mem_lst;

    tab = file->host_chart;

/* print out the type */
    defp = PD_inquire_table_type(tab, type);
    if (defp == NULL)
       SS_error("VARIABLE NOT IN STRUCTURE CHART - _SX_FIND_LEAF", SS_null);

    else
       {mem_lst = defp->members;
	if (mem_lst != NULL)
	   {SC_hasharr_install(tytab, type, TYPE, TYPE, TRUE, TRUE);
	    sz  = defp->size;
	    svr = vr;
	    for (i = 0L; i < nitems; i++, svr += sz)
	        {if (pdb_wr_hook != NULL)
		    mem_lst = (*pdb_wr_hook)(file, svr, defp);

		 for (desc = mem_lst; desc != NULL; desc = desc->next)
		     {_SX_find_data(tytab, file,
				    (char *) (svr + desc->member_offs), 
				    desc->number, desc->type);};};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_FIND_INDIRECTION - locate part of a data element thru indirection */

static void _SX_find_indirection(hasharr *tytab, PDBfile *file,
				 char **vr, long nitems, char *type)
   {long i, ditems;
    char *dtype, *bf;

    dtype = PD_dereference(CSTRSAVE(type));

    for (i = 0L; i < nitems; i++, vr++)
        {ditems = _PD_number_refd(DEREF(vr), dtype, file->host_chart);

         if (ditems == -1L)
            {bf = SC_dsnprintf(FALSE,
			       "CAN'T GET POINTER LENGTH ON %s - _SX_FIND_INDIRECTION", 
			       dtype);
             SS_error(bf, SS_null);};

         if (ditems == -2L)
            {bf = SC_dsnprintf(FALSE,
			       "UNKNOWN TYPE %s - _SX_FIND_INDIRECTION", 
			       dtype);
             SS_error(bf, SS_null);};

/* if the type is an indirection, follow the pointer */
         if (_PD_indirection(dtype))
            _SX_find_indirection(tytab, file, (char **) DEREF(vr), 
                                 ditems, dtype);
         else
            _SX_find_leaf(tytab, file, DEREF(vr), ditems, dtype);};

    CFREE(dtype);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_FIND_DATA - locate part of a data element */

static void _SX_find_data(hasharr *tytab, PDBfile *file, void *vr,
			  long nitems, char *type)
   {

/* if the type is an indirection, follow the pointer */
    if (_PD_indirection(type))
       _SX_find_indirection(tytab, file, (char **) vr, nitems, type);
    else
       _SX_find_leaf(tytab, file,  vr, nitems, type);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FIND_TYPES - find all types used by this variable */

object *_SXI_find_types(SS_psides *si, object *arg)
   {char *type;
    long nitems;
    void *vr;
    hasharr *tytab;
    object *args, *obj;
    PDBfile *file;
    syment *ep;

    if (!SX_PDBDATAP(arg))
       SS_error("MUST BE PDBDATA - _SXI_FIND_TYPES", arg);

    file   = PDBDATA_FILE(arg);
    ep     = PDBDATA_EP(arg);
    vr     = PDBDATA_DATA(arg);
    type   = PD_entry_type(ep);
    nitems = PD_entry_number(ep);

/* create a hash table to put all of these types in */
    tytab = SC_make_hasharr(HSZSMALL, NODOC, SC_HA_NAME_KEY);
    strcpy(TYPE, "type");

/* fill the hash table with the types */
    _SX_find_data(tytab, file, vr, nitems, type);

/* convert table to list */
    args = SS_mk_cons(si, SS_mk_hasharr(si, tytab), SS_null);
    obj = SS_hash_dump(si, args);

/* delete hash table */
    SC_free_hasharr(tytab, NULL, NULL);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
