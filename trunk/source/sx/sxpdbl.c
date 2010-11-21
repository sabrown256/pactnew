/*
 * SXPDBL.C - PDB list functionality in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

#define	ARRAY_VECTOR(_o, _d, _fnc, _n, _off)                                 \
   {long _i;                                                                 \
    if ((_n == 1) && (_off == 0))                                            \
       _o = _fnc(*_d);                                                       \
    else                                                                     \
       {_o = SS_null;                                                        \
	for (_i = 0; _i < _n; _i++)                                          \
	    _o = SS_mk_cons(_fnc(_d[_i]), _o);                               \
	if (_o != SS_null)                                                   \
	   _o = SS_lstvct(SS_reverse(_o));};}

object
 *_SX_make_list_indirection(PDBfile *file, char **vr, long nitems, char *type),
 *_SX_make_list_leaf(PDBfile *file, char *vr, long nitems, char *type),
 *_SX_make_list_io(PDBfile *file, char *vr, long nitems, char *type);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAKE_LIST_SYMENT - convert a syment into a list */

object *_SX_make_list_syment(PDBfile *file, void *vr, long ni, char *type)
   {object *obj;

/* if the type is an indirection, follow the pointer */
    if (_PD_indirection(type))
       obj = _SX_make_list_indirection(file, (char **) vr, ni, type);

    else
       {if ((strcmp(type, SC_CHAR_S) == 0) && (ni == 1))
	   {char *s;

	    s = SC_strsavef("a", "char*:_SX_MAKE_LIST_SYMENT:s");
	    s[0] = *(char *) vr;
	    obj  = _SX_make_list_leaf(file, s, ni, type);}
	else
	   obj = _SX_make_list_leaf(file, vr, ni, type);};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAKE_LIST_INDIRECTION - handle the indirections for the process of
 *                           - converting a syment into a list
 */

object *_SX_make_list_indirection(PDBfile *file, char **vr,
				  long ni, char *type)
   {long i, ditems;
    char *dtype;
    object *obj, *obj1, *o;

    dtype = PD_dereference(SC_strsavef(type,
                           "char*:_SX_MAKE_LIST_INDIRECTION:dtype"));

    obj = SS_null;

    for (i = 0L; i < ni; i++, vr++)
        {ditems = _PD_number_refd(DEREF(vr), dtype, file->host_chart);

         if (ditems < 0L)
            continue;

/* if the type is an indirection, follow the pointer */
        if (_PD_indirection(dtype))
           obj1 = _SX_make_list_indirection(file, (char **) DEREF(vr),
                                            ditems, dtype);
        else
           obj1 = _SX_make_list_leaf(file, DEREF(vr), ditems, dtype);

        obj = SS_mk_cons(obj1, obj);};

    SFREE(dtype);

    o = SS_reverse(obj);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAKE_LIST_LEAF - if 'type' is a primitive type, display the data,
 *                    - otherwise, lookup the type, and display each member.
 */

object *_SX_make_list_leaf(PDBfile *file, char *vr, long ni, char *type)
   {long ii, sz, member_offs;
    defstr *defp;
    memdes *desc, *mem_lst;
    char *mtype, *svr;
    object *obj, *obj1, *obj2;

    mem_lst = NULL;

    defp = PD_inquire_host_type(file, type);
    if (defp == NULL)
       SS_error("VARIABLE NOT IN STRUCTURE CHART - _SX_MAKE_LIST_LEAF",
                SS_null);
    else
       mem_lst = defp->members;

    if (mem_lst == NULL)
       obj = _SX_make_list_io(file, vr, ni, type);

    else
       {obj = SS_null;                              /* cons of array elements */
	sz  = defp->size;
	svr = vr;
        for (ii = 0L; ii < ni; ii++, svr += sz)
            {if (pdb_wr_hook != NULL)
                mem_lst = (*pdb_wr_hook)(file, svr, defp);
             obj1 = SS_null;
             for (desc = mem_lst; desc != NULL; desc = desc->next)
                 {member_offs = desc->member_offs;
		  PD_CAST_TYPE(mtype, desc, svr + member_offs, svr, SS_error,
			       "BAD CAST - _SX_MAKE_LIST_LEAF", SS_null);
                  obj2 = _SX_make_list_syment(file,
					      (char *) svr + member_offs,
                                              desc->number, mtype);
                  obj1 = SS_mk_cons(obj2, obj1);};

             obj1 = SS_reverse(obj1);
	     obj = SS_mk_cons(obj1, obj);};

        if (ni > 1L)
           obj = SS_reverse(obj);};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_BOOLEAN - wrapper to make Scheme boolean from C bool */

static object *_SX_mk_boolean(bool b)
   {object *o;

    o = (b == true) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAKE_LIST_IO - convert a primitive type into a list */

object *_SX_make_list_io(PDBfile *file, char *vr, long ni, char *type)
   {int id, offset;
    object *obj;

/* print out the type */
    offset = 0;

    id = SC_type_id(type, FALSE);

/* fixed point types (proper) */
    if (SC_fix_type_id(type, TRUE) != -1)
       {long long *d;

	d = SC_convert_id(SC_LONG_LONG_I, NULL, 0, id, vr, 0, 1, ni, FALSE);

	ARRAY_VECTOR(obj, d, SS_mk_integer, ni, offset);

	SFREE(d);}

/* floating point types (proper) */
    else if (SC_fp_type_id(type) != -1)
       {double *d;

	d = SC_convert_id(SC_DOUBLE_I, NULL, 0, id, vr, 0, 1, ni, FALSE);

	ARRAY_VECTOR(obj, d, SS_mk_float, ni, offset);

	SFREE(d);}

/* complex floating point types (proper) */
    else if (SC_cx_type_id(type) != -1)
       {double _Complex *d;

	d = SC_convert_id(SC_DOUBLE_COMPLEX_I, NULL, 0, id, vr, 0, 1, ni, FALSE);

	ARRAY_VECTOR(obj, d, SS_mk_complex, ni, offset);

	SFREE(d);}

    else
       {id = SC_type_id(type, FALSE);

	if (id == SC_BOOL_I)
	   {int *d;

	    d = SC_convert_id(SC_INT_I, NULL, 0, id, vr, 0, 1, ni, FALSE);

	    ARRAY_VECTOR(obj, d, _SX_mk_boolean, ni, offset);

	    SFREE(d);}

	else if (id == SC_CHAR_I)
	   {if (vr == NULL)
	       obj = SS_null;
	    else
	       obj = SS_mk_string(vr);}

/* handle user defined primitive types by turning them into vectors of
 * integers containing the byte values
 * GOTCHA: if the byte size of an item matches a standard machine
 * type size make a vector of that type
 */
	else
	   {long bpi, nb;
	    char *d;

	    d   = (char *) vr;
	    bpi = _PD_lookup_size(type, file->chart);
	    nb  = bpi*ni;

	    ARRAY_VECTOR(obj, d, SS_mk_integer, nb, offset);};};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
