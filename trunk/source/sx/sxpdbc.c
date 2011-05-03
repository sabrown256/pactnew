/*
 * SXPDBC.C - PDB copy functionality in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

void
 _SX_copy_indirection(SS_psides *si, PDBfile *file,
		      char **vrin, char **vrout, char *type), 
 _SX_copy_leaf(SS_psides *si, PDBfile *file,
	       char *vrin, char*vrout, long nitems, char *type);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_COPY_TREE - read NITEMS of TYPE from the PDBfile FILE
 *               - into the location pointed to by VR
 *               - return the number of items successfully read
 *               - type "char *" is special cased because we not sure
 *               - how long it is until we look at the string.
 */

void _SX_copy_tree(SS_psides *si, PDBfile *file, char *vrin, char *vrout,
		   long nitems, char *type)
   {long i;
    char **lvr, **lvo, *dtype;

    if (!_PD_indirection(type))
       _SX_copy_leaf(si, file, vrin, vrout, nitems, type);
    else
       {lvr = (char **) vrin;
        lvo = (char **) vrout;
        dtype = PD_dereference(CSTRSAVE(type));
        for (i = 0L; i < nitems; i++)
            _SX_copy_indirection(si, file, &lvr[i], &lvo[i], dtype);
        CFREE(dtype);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_COPY_INDIRECTION - read the information about an indirection,
 *                      - allocate the space, connect the pointer, and
 *                      - read in the data
 */

void _SX_copy_indirection(SS_psides *si, PDBfile *file,
			  char **vrin, char **vrout, char *type)
   {long nitems, bpi, nbytes;
    char *pv;

    if (*vrin == NULL)
       *vrout = NULL;
    else
       {nbytes = (long) SC_arrlen(*vrin);
        if ((bpi = _PD_lookup_size(type, file->host_chart)) == -1)
           SS_error_n(si, "CAN'T FIND NUMBER OF BYTES - _SX_COPY_INDIRECTION",
		      SS_mk_string(si, type));
        nitems = nbytes / bpi;

        pv = CMAKE_N(char, nitems*bpi);
        DEREF(vrout) = pv;

        SC_arrtype(pv, SC_arrtype(*vrin, 0));
        _SX_copy_tree(si, file, *vrin, pv, nitems, type);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_COPY_LEAF - read NITEMS of TYPE from the PDBfile FILE
 *               - into the location pointed to by VR
 *               - at this level it guaranteed that the type will
 *               - not be a pointer
 *               - return the number of items successfully read
 */

void _SX_copy_leaf(SS_psides *si, PDBfile *file, char *vrin, char *vrout,
		   long nitems, char *type)
   {long i, bpi, sz, member_offs;
    char *mtype, *dtype, *svrin, *svrout;
    defstr *dp;
    memdes *desc, *mem_lst;

    mem_lst = NULL;

    bpi = _PD_lookup_size(type, file->host_chart);
    if (bpi == -1)
       SS_error_n(si, "CAN'T GET NUMBER OF BYTES - _SX_COPY_LEAF",
		  SS_mk_string(si, type));

    memcpy(vrout, vrin, (int) nitems*bpi);

/* if this was a derived type and some of its members are pointers
 * fetch in the pointered data
 */
    dp = PD_inquire_host_type(file, type);
    if (dp == NULL)
       SS_error_n(si, "BAD TYPE - _SX_COPY_LEAF", SS_null);
    else
       {mem_lst = dp->members;

	if (mem_lst != NULL)
	   if (pdb_wr_hook != NULL)
	      mem_lst = (*pdb_wr_hook)(file, vrin, dp);

/* for an array of structs copy the indirects for each array element */
	if (dp->n_indirects)
	   {sz     = dp->size;
	    svrin  = vrin;
	    svrout = vrout;
	    for (i = 0L; i < nitems; i++, svrin += sz, svrout += sz)
	        {for (desc = mem_lst; desc != NULL; desc = desc->next)
		     {member_offs = desc->member_offs;
		      PD_CAST_TYPE(mtype, desc, svrin + member_offs, svrin,
				   SS_error, "BAD CAST - _SX_COPY_LEAF",
				   SS_null);

		      if (_PD_indirection(mtype))
			 {dtype = PD_dereference(CSTRSAVE(mtype));
			  _SX_copy_indirection(si, file,
					       (char **) (svrin + member_offs),
					       (char **) (svrout + member_offs),
                                          dtype);
			  CFREE(dtype);};};};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
