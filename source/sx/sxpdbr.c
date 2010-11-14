/*
 * SXPDBR.C - PDB read functionality in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

/*--------------------------------------------------------------------------*/

#define READ_NUM(_type, _vr, _i, _ityp, _o)                                  \
    {_type *_pv;                                                             \
     _pv = (_type *) _vr;                                                    \
     if (_ityp == SC_INT_I)                                                  \
        _pv[_i] = (_type) SS_INTEGER_VALUE(_o);                              \
     else if (_ityp == SC_FLOAT_I)                                           \
        _pv[_i] = (_type) SS_FLOAT_VALUE(_o);                                \
     else if (_ityp == SC_DOUBLE_COMPLEX_I)                                  \
        _pv[_i] = (_type) SS_COMPLEX_VALUE(_o);                              \
     else if (_ityp == SC_QUATERNION_I)                                      \
        _pv[_i] = (_type) (SS_QUATERNION_VALUE(_o)).s;                       \
     else if (_ityp == SC_BOOL_I)                                            \
        _pv[_i] = (_type) SS_BOOLEAN_VALUE(_o);                              \
     else                                                                    \
        SS_error("EXPECTED A NUMBER", _o);}

/*--------------------------------------------------------------------------*/

#define READ_IO(_type, _vr, _n, _o)                                          \
   {int _ityp, _jtyp;                                                        \
    long _i;                                                                 \
    _ityp = SC_arrtype(_o, -1);                                              \
    if (_ityp == SS_CONS_I)                                                  \
       {if (SS_consp(SS_car(_o)))                                            \
	   _o = SS_car(_o);                                                  \
        for (_i = 0; _i < _n; _i++)                                          \
            {obj1  = SS_car(_o);                                             \
             _jtyp = SC_arrtype(obj1, -1);                                   \
	     READ_NUM(_type, _vr, _i, _jtyp, obj1);                          \
	     obj1 = SS_cdr(_o);                                              \
	     if (obj1 != SS_null)                                            \
		_o = obj1;};}                                                \
    else if (_ityp == SS_VECTOR_I)                                           \
       {va = SS_VECTOR_ARRAY(_o);                                            \
        for (_i = 0; _i < _n; _i++)                                          \
            {_jtyp = SC_arrtype(va[_i], -1);                                 \
	     READ_NUM(_type, _vr, _i, _jtyp, va[_i]);};}                     \
    else                                                                     \
       READ_NUM(_type, _vr, 0, _ityp, _o);}

/*--------------------------------------------------------------------------*/

static void
 _SX_rd_indirection_list(object *obj, PDBfile *file, char **vr, char *type), 
 _SX_rd_io_list(object *obj, char *vr, long nitems, defstr *dp),
 _SX_rd_leaf_list(object *obj, PDBfile *file, char *vr,
		  long nitems, char *type, dimdes *dims);

syment
 *_SX_rd_data(PDBfile *file, char *name, syment *ep, SC_address *addr,
	      object *name_obj);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_TREE_LIST - read NITEMS of TYPE from the PDBfile FILE
 *                  - into the location pointed to by VR
 *                  - return the number of items successfully read
 */

void _SX_rd_tree_list(object *obj, PDBfile *file, char *vr, long nitems,
		      char *type, dimdes *dims)
   {long i;
    char **lvr;
    char *dtype;

    if (!_PD_indirection(type))
       _SX_rd_leaf_list(obj, file, vr, nitems, type, dims);
    else
       {lvr = (char **) vr;
        dtype = PD_dereference(SC_strsavef(type,
                               "char*:_SX_RD_TREE_LIST:dtype"));
        for (i = 0L; i < nitems; i++, obj = SS_cdr(obj))
            _SX_rd_indirection_list(SS_car(obj), file, &lvr[i], dtype);
        SFREE(dtype);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_INDIRECTION_LIST - read the information about an indirection,
 *                         - allocate the space, connect the pointer, and
 *                         - read in the data
 */

static void _SX_rd_indirection_list(object *obj, PDBfile *file,
				    char **vr, char *type)
   {long bpi, nitems;
    char *pv;

    nitems = (long) _SX_get_object_length(obj);

    if (nitems == 0L)
       *vr = NULL;
    else
       {bpi = _PD_lookup_size(type, file->host_chart);
        if (bpi == -1)
           SS_error("CAN'T FIND NUMBER OF BYTES - _SX_RD_INDIRECTION_LIST",
                    SS_null);

        DEREF(vr) = pv = SC_alloc_nzt(nitems, bpi,
				      "_SX_RD_INDIRECTION_LIST:pv", NULL);
        _SX_rd_tree_list(obj, file, pv, nitems, type, (dimdes *) NULL);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_LEAF_LIST - read NITEMS of TYPE from the PDBfile FILE
 *                  - into the location pointed to by VR
 *                  - at this level it guaranteed that the type will
 *                  - not be a pointer
 *                  - return the number of items successfully read
 */

static void _SX_rd_leaf_list(object *obj, PDBfile *file, char *vr,
			     long nitems, char *type, dimdes *dims)
   {long i, sz;
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
       SS_error("BAD TYPE - _SX_RD_LEAF_LIST", SS_null);

    else
       {mem_lst = dp->members;

	if (mem_lst == NULL)

/* use dp->type to get past typedef's */
	   _SX_rd_io_list(obj, vr, nitems, dp);

/* for an array of structs write the indirects for each array element */
	else
	   {sz  = dp->size;
	    svr = vr;
	    for (i = 0L; i < nitems; i++, svr += sz, obj  = SS_cdr(obj))
	        {obj1 = SS_car(obj);
	       
		 for (desc = mem_lst; desc != NULL; desc = desc->next)
		     {PD_CAST_TYPE(ttype, desc, svr+desc->member_offs, svr,
				   SS_error, "BAD CAST - _SX_RD_LEAF_LIST", obj1);

		      _SX_rd_tree_list(SS_car(obj1), file,
				       svr + desc->member_offs,
				       desc->number, ttype,
				       desc->dimensions);

		      obj1 = SS_cdr(obj1);};};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_IO_LIST - if TYPE in DP is a primitive type, display the data,
 *                - otherwise, lookup the type, and display each member
 */

static void _SX_rd_io_list(object *obj, char *vr, long nitems, defstr *dp)
   {int i, id, ityp;
    char *msg, *type;
    object *obj1,  **va;

    type = dp->type;
    id   = SC_type_id(type, FALSE);

/* print out the type */
    if (id == SC_CHAR_I)
       {ityp = SC_arrtype(obj, -1);
	if (ityp == SC_STRING_I)
	   strncpy(vr, SS_STRING_TEXT(obj), nitems);
	else if (ityp == SS_CONS_I)
	   strncpy(vr, SS_STRING_TEXT(SS_car(obj)), nitems);
        else
	   SS_error("EXPECTED A STRING - _SX_RD_IO_LIST", obj);

        return;};

/* only need to check non-char types 
 * NOTE: the '\0' terminator on char strings cause the following
 *       test to fail unnecessarily (DRS.SCM for example)
 */
    if (nitems < (long) _SX_get_object_length(obj))
       {msg = SC_dsnprintf(FALSE, "DATA TOO LONG FOR TYPE %s - _SX_RD_IO_LIST",
			   type);
        SS_error(msg, obj);};

/* fixed point types */
    if (id == SC_SHORT_I)
       {READ_IO(short, vr, nitems, obj);}
    else if (id == SC_INT_I)
       {READ_IO(int, vr, nitems, obj);}

    else if (id == SC_LONG_I)
       {READ_IO(long, vr, nitems, obj);}

    else if (id == SC_LONG_LONG_I)
       {READ_IO(long long, vr, nitems, obj);}

/* floating point types */
    else if (id == SC_FLOAT_I)
       {READ_IO(float, vr, nitems, obj);}
 
    else if (id == SC_DOUBLE_I)
       {READ_IO(double, vr, nitems, obj);}

    else if (id == SC_LONG_DOUBLE_I)
       {READ_IO(long double, vr, nitems, obj);}

/* complex floating point types */
    else if (id == SC_FLOAT_COMPLEX_I)
       {READ_IO(float _Complex, vr, nitems, obj);}

    else if (id == SC_DOUBLE_COMPLEX_I)
       {READ_IO(double _Complex, vr, nitems, obj);}

    else if (id == SC_LONG_DOUBLE_COMPLEX_I)
       {READ_IO(long double _Complex, vr, nitems, obj);}

    else if (id == SC_BOOL_I)
       {READ_IO(bool, vr, nitems, obj);}

    else
#if 1
       {int jtyp;

        ityp = SC_arrtype(obj, -1);
	if (ityp == SC_INT_I)
	   {int *pv;
	    pv  = (int *) vr;
	    *pv = (int) SS_INTEGER_VALUE(obj);}

	else if (ityp == SC_FLOAT_I)
	   {float *pv;
	    pv  = (float *) vr;
	    *pv = (float) SS_FLOAT_VALUE(obj);}

	else if (ityp == SS_CONS_I)
	   {if (SS_consp(SS_car(obj)))
	       obj = SS_car(obj);

	    for (i = 0; i < nitems; i++)
	        {obj1  = SS_car(obj);
		 jtyp = SC_arrtype(obj1, -1);
		 if (jtyp == SC_INT_I)
		    {int *pv;
		     pv    = (int *) vr;
		     pv[i] = (int) SS_INTEGER_VALUE(obj1);}
		 else if (jtyp == SC_FLOAT_I)
		    {float *pv;
		     pv    = (float *) vr;
		     pv[i] = (char) SS_FLOAT_VALUE(obj1);}
		 else
		    SS_error("EXPECTED A NUMBER", obj1);
		 obj1 = SS_cdr(obj);
		 if (obj1 != SS_null)
		    obj = obj1;};}

	else if (ityp == SS_VECTOR_I)
	   {va = SS_VECTOR_ARRAY(obj);
	    for (i = 0; i < nitems; i++)
	        {jtyp = SC_arrtype(va[i], -1);
		 if (jtyp == SC_INT_I)
		    {int *pv;
		     pv    = (int *) vr;
		     pv[i] = (int) SS_INTEGER_VALUE(va[i]);}
		 else if (jtyp == SC_FLOAT_I)
		    {float *pv;
		     pv    = (float *) vr;
		     pv[i] = (float) SS_FLOAT_VALUE(va[i]);}
		 else
		    SS_error("EXPECTED A NUMBER", va[i]);};}
	else
	   SS_error("EXPECTED A NUMBER", obj);}
#else
       SS_error("ILLEGAL TYPE - _SX_RD_IO_LIST", SS_mk_string(type));
#endif

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_READ_NUMERIC_DATA - read data from a PDB file into a numeric array
 *                        - if file is missing, use the internal virtual file
 *                        -
 *                        - (pdb-read-numeric-data [file] name)
 */

object *_SXI_read_numeric_data(object *argl)
   {long sz;
    char *name, *s, *dtype;
    object *ret, *name_obj;
    PDBfile *file;
    PD_smp_state *pa;
    syment *ep, *cp;
    SC_address addr;
    C_array *arr;
    g_file *po;

    pa = _PD_get_state(-1);

    if (!SS_consp(argl))
       SS_error("BAD ARGUMENT LIST - _SXI_READ_NUMERIC_DATA", argl);

/* see if the first object is a pdbfile, if so use it */
    argl = SX_get_pdbfile(argl, &file, &po);

    name = NULL;
    SS_args(argl,
            SC_STRING_I, &name,
            0);

    s = name;
    name = _PD_expand_hyper_name(file, s);
    SFREE(s);
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
             SS_error(PD_err, name_obj);
        default :
             memset(PD_err, 0, MAXLINE);
	     cp = _SX_rd_data(file, name, ep, &addr, name_obj);
        case ERR_FREE :
	     break;};

    if (cp == NULL)
       {dtype = NULL;
	sz    = 0;}

    else if (!_PD_indirection(PD_entry_type(cp)))
       {dtype = SC_strsavef(PD_entry_type(cp),
			    "_SXI_READ_NUMERIC_DATA:type");
	sz    = PD_entry_number(cp);}
    else
       {dtype = PD_dereference(PD_entry_type(cp));

	if (file == SX_vif)
	   sz = SC_arrlen(addr.memaddr)/
	        _PD_lookup_size(dtype, file->chart);
	else
	   {FILE *fp;
	    PD_itag itag;
	
            fp = file->stream;
            lio_seek(fp, PD_entry_address(cp), SEEK_SET);

/* GOTCHA: need itagless version of this */
	    (*file->rd_itag)(file, NULL, &itag);
	    sz = itag.nitems;};};

    arr = PM_make_array(dtype, sz, addr.memaddr);
    ret = SX_mk_C_array(arr);

    _PD_rl_syment_d(ep);
    _PD_rl_syment_d(cp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_DATA - grab some data from the specified file */

syment *_SX_rd_data(PDBfile *file, char *name, syment *ep,
		    SC_address *addr, object *name_obj)
   {long n;
    char *type, *dtype;
    defstr *dpf, *odp;
    syment *cp;

    cp = PD_copy_syment(ep);

    n    = PD_entry_number(cp);
    type = PD_entry_type(cp);

    if (!_PD_indirection(type))

       {if (!_PD_prim_typep(type, file->host_chart, PD_READ))
	   SS_error("MUST BE PRIMITIVE TYPE - _SX_RD_DATA", name_obj);

	if (file == SX_vif)
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
	    dpf = _PD_lookup_type(type, file->chart);
	    if (dpf->convert)
	       {odp = PD_inquire_table_type(file->host_chart, type);
		if (strcmp(type, odp->type) != 0)
		   {SFREE(PD_entry_type(cp));
		    PD_entry_type(cp) = SC_strsavef(odp->type,
						    "_SX_RD_DATA:type");};};};}

    else

       {dtype = PD_dereference(SC_strsavef(type,
                               "char*:_SX_RD_DATA:dtype"));

	if (!_PD_prim_typep(dtype, file->host_chart, PD_READ))
	   SS_error("MUST BE PRIMITIVE TYPE - _SX_RD_DATA", name_obj);

	if (file == SX_vif)
	   {addr->diskaddr = PD_entry_address(cp);
	    addr->memaddr  = DEREF(addr->memaddr);}

	else
	    PD_read(file, name, &addr->memaddr);

	SFREE(dtype);};

    return(cp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
