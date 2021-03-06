/*
 * SXPDBR.C - PDB read functionality in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

static void
 _SX_rd_indirection_list(SS_psides *si, object *obj, PDBfile *file,
			 char **vr, const char *type),
 _SX_rd_io_list(SS_psides *si, object *obj,
		char *vr, inti ni, defstr *dp),
 _SX_rd_leaf_list(SS_psides *si, object *obj, PDBfile *file, char *vr,
		  inti ni, const char *type, dimdes *dims);

syment
 *_SX_rd_data(SS_psides *si, PDBfile *file,
	      const char *name, syment *ep, SC_address *addr,
	      object *name_obj);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_TREE_LIST - read NI of TYPE from the PDBfile FILE
 *                  - into the location pointed to by VR
 *                  - return the number of items successfully read
 */

void _SX_rd_tree_list(SS_psides *si, object *obj, PDBfile *file,
		      char *vr, inti ni,
		      const char *type, dimdes *dims)
   {inti i;
    char **lvr;
    char *dtype;

    if (!_PD_indirection(type))
       _SX_rd_leaf_list(si, obj, file, vr, ni, type, dims);
    else
       {lvr = (char **) vr;
        dtype = PD_dereference(CSTRSAVE(type));
        for (i = 0L; i < ni; i++, obj = SS_cdr(si, obj))
            _SX_rd_indirection_list(si, SS_car(si, obj), file, &lvr[i], dtype);
        CFREE(dtype);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_INDIRECTION_LIST - read the information about an indirection,
 *                         - allocate the space, connect the pointer, and
 *                         - read in the data
 */

static void _SX_rd_indirection_list(SS_psides *si, object *obj, PDBfile *file,
				    char **vr, const char *type)
   {inti ni;
    intb bpi;
    char *pv;

    ni = _SS_get_object_length(si, obj);

    if (ni == 0L)
       *vr = NULL;
    else
       {bpi = _PD_lookup_size(type, file->host_chart);
        if (bpi == -1)
           SS_error(si,
		      "CAN'T FIND NUMBER OF BYTES - _SX_RD_INDIRECTION_LIST",
		      SS_null);

	pv = CMAKE_N(char, ni*bpi);
        DEREF(vr) = pv;

        _SX_rd_tree_list(si, obj, file, pv, ni, type, (dimdes *) NULL);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_LEAF_LIST - read NI of TYPE from the PDBfile FILE
 *                  - into the location pointed to by VR
 *                  - at this level it guaranteed that the type will
 *                  - not be a pointer
 *                  - return the number of items successfully read
 */

static void _SX_rd_leaf_list(SS_psides *si, object *obj, PDBfile *file,
			     char *vr, inti ni,
			     const char *type, dimdes *dims)
   {inti i, sz;
    defstr *dp;
    memdes *desc, *mem_lst;
    object *obj1;
    char *svr, *ttype;

    mem_lst = NULL;

/* if this was a derived type and some of its members are pointers
 * fetch in the pointered data
 */
    dp = PD_inquire_host_type(file, type);
    if (dp == NULL)
       SS_error(si, "BAD TYPE - _SX_RD_LEAF_LIST", SS_null);

    else
       {mem_lst = dp->members;

	if (mem_lst == NULL)

/* use dp->type to get past typedef's */
	   _SX_rd_io_list(si, obj, vr, ni, dp);

/* for an array of structs write the indirects for each array element */
	else
	   {sz  = dp->size;
	    svr = vr;
	    for (i = 0L; i < ni; i++, svr += sz, obj  = SS_cdr(si, obj))
	        {obj1 = SS_car(si, obj);
	       
		 for (desc = mem_lst; desc != NULL; desc = desc->next)
		     {SX_CAST_TYPE(si, ttype, desc,
				   svr+desc->member_offs, svr,
				   "BAD CAST - _SX_RD_LEAF_LIST", obj1);

		      _SX_rd_tree_list(si, SS_car(si, obj1), file,
				       svr + desc->member_offs,
				       desc->number, ttype,
				       desc->dimensions);

		      obj1 = SS_cdr(si, obj1);};};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_IO_LIST - if TYPE in DP is a primitive type, display the data,
 *                - otherwise, lookup the type, and display each member
 */

static void _SX_rd_io_list(SS_psides *si, object *obj,
			   char *vr, inti ni, defstr *dp)
   {char *type;

    type = dp->type;

    _SS_list_to_numtype(si, type, vr, ni, obj);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_READ_NUMERIC_DATA - read data from a PDB file into a numeric array
 *                        - if file is missing, use the internal virtual file
 *                        -
 *                        - (pdb-read-numeric-data [file] name)
 */

object *_SXI_read_numeric_data(SS_psides *si, object *argl)
   {inti sz;
    char *name, *s, *dtype;
    object *ret, *name_obj;
    PDBfile *file;
    PD_smp_state *pa;
    syment *ep, *cp;
    SC_address addr;
    C_array *arr;
    SX_file *po;

    pa = _PD_get_state(-1);

    if (!SS_consp(argl))
       SS_error(si, "BAD ARGUMENT LIST - _SXI_READ_NUMERIC_DATA", argl);

/* see if the first object is a pdbfile, if so use it */
    argl = SX_get_pdbfile(si, argl, &file, &po);

    name = NULL;
    SS_args(si, argl,
            G_STRING_I, &name,
            0);

    s = name;
    name = _PD_expand_hyper_name(file, s);
    CFREE(s);
    if (name == NULL)
       return(SS_null);

/* look up the variable name and return FALSE if it is not there */
    cp = NULL;
    ep = _PD_effective_ep(file, name, TRUE, NULL);
    if (ep == NULL)
       return(SS_null);

/* GOTCHA: this will die screaming if name is a member with a cast
 *         an error stack is needed to do this right
 */
    name_obj = SS_null;
    switch (SETJMP(pa->read_err))
       {case ABORT :
             SS_error(si, PD_get_error(), name_obj);
        default :
             memset(PD_gs.err, 0, MAXLINE);
	     cp = _SX_rd_data(si, file, name, ep, &addr, name_obj);
        case ERR_FREE :
	     break;};

    if (cp == NULL)
       {dtype = NULL;
	sz    = 0;}

    else if (!_PD_indirection(PD_entry_type(cp)))
       {dtype = CSTRSAVE(PD_entry_type(cp));
	sz    = PD_entry_number(cp);}
    else
       {dtype = PD_dereference(PD_entry_type(cp));

	if (file == SX_gs.vif)
	   sz = SC_arrlen(addr.memaddr)/
	        _PD_lookup_size(dtype, file->chart);
	else
	   {FILE *fp;
	    PD_itag itag;
	
            fp = file->stream;
            lio_seek(fp, PD_entry_address(cp), SEEK_SET);

/* GOTCHA: need itagless version of this */
	    _PD_rd_itag(file, NULL, &itag);
	    sz = itag.nitems;};};

    arr = PM_make_array(dtype, sz, addr.memaddr);
    ret = SX_make_c_array(si, arr);

    _PD_rl_syment_d(ep);
    _PD_rl_syment_d(cp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_DATA - grab some data from the specified file */

syment *_SX_rd_data(SS_psides *si, PDBfile *file, const char *name,
		    syment *ep, SC_address *addr, object *name_obj)
   {inti n;
    char *type, *dtype;
    defstr *dpf, *odp;
    syment *cp;

    cp = PD_copy_syment(ep);
    if (cp != NULL)
       {n    = PD_entry_number(cp);
	type = PD_entry_type(cp);

	if (!_PD_indirection(type))
	   {if (!_PD_prim_typep(type, file->host_chart, PD_READ))
	       SS_error(si, "MUST BE PRIMITIVE TYPE - _SX_RD_DATA", name_obj);

	    if (file == SX_gs.vif)
	       addr->diskaddr = PD_entry_address(cp);

	    else
	       {addr->memaddr = _PD_alloc_entry(file, type, n);
		PD_read(file, name, addr->memaddr);

/* GOTCHA: we MUST be consistent about this - if PDB ended up doing
 * a conversion here the type name had better reflect the exact
 * type the data has - otherwise someone downstream may decide based
 * on the name to do an unwarranted conversion!!!!!!!
 * The question is where in PDB this really ought to go
 */
		dpf = _PD_type_lookup(file, PD_CHART_FILE, type);
		if ((dpf != NULL) && (dpf->convert == TRUE))
		   {odp = PD_inquire_table_type(file->host_chart, type);
		    if ((odp != NULL) && (strcmp(type, odp->type) != 0))
		       {CFREE(cp->type);
			cp->type = CSTRSAVE(odp->type);};};};}

	else
	   {dtype = PD_dereference(CSTRSAVE(type));

	    if (!_PD_prim_typep(dtype, file->host_chart, PD_READ))
	       SS_error(si, "MUST BE PRIMITIVE TYPE - _SX_RD_DATA", name_obj);

	    if (file == SX_gs.vif)
	       {addr->diskaddr = PD_entry_address(cp);
		addr->memaddr  = DEREF(addr->memaddr);}

	    else
	       PD_read(file, name, &addr->memaddr);

	    CFREE(dtype);};};

    return(cp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
