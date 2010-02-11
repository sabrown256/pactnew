/*
 * SXPDBL.C - PDB list functionality in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

object
 *_SX_make_list_indirection(PDBfile *file, char **vr, long nitems, char *type),
 *_SX_make_list_leaf(PDBfile *file, char *vr, long nitems, char *type),
 *_SX_make_list_io(PDBfile *file, char *vr, long nitems, char *type);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAKE_LIST_SYMENT - convert a syment into a list */

object *_SX_make_list_syment(PDBfile *file, void *vr, long nitems,
			     char *type)
   {object *obj;

/* if the type is an indirection, follow the pointer */
    if (_PD_indirection(type))
       obj = _SX_make_list_indirection(file, (char **) vr, nitems, type);
    else
       {if ((strcmp(type, "char") == 0) && (nitems == 1))
	   {char *s;

	    s = SC_strsavef("a", "char*:_SX_MAKE_LIST_SYMENT:s");
	    s[0] = *(char *) vr;
	    obj  = _SX_make_list_leaf(file, s, nitems, type);}
	else
	   obj = _SX_make_list_leaf(file, vr, nitems, type);};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAKE_LIST_INDIRECTION - handle the indirections for the process of
 *                           - converting a syment into a list
 */

object *_SX_make_list_indirection(PDBfile *file, char **vr,
				  long nitems, char *type)
   {long i, ditems;
    char *dtype;
    object *obj, *obj1, *o;

    dtype = PD_dereference(SC_strsavef(type,
                           "char*:_SX_MAKE_LIST_INDIRECTION:dtype"));

    obj = SS_null;

    for (i = 0L; i < nitems; i++, vr++)
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

object *_SX_make_list_leaf(PDBfile *file, char *vr, long nitems, char *type)
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
       obj = _SX_make_list_io(file, vr, nitems, type);

    else
       {obj = SS_null;                              /* cons of array elements */
	sz  = defp->size;
	svr = vr;
        for (ii = 0L; ii < nitems; ii++, svr += sz)
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

        if (nitems > 1L)
           obj = SS_reverse(obj);};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAKE_LIST_IO - Convert a primitive type into a list. */

object *_SX_make_list_io(PDBfile *file, char *vr, long nitems, char *type)
   {int i, offset;
    object *obj;

    offset = 0;

/* print out the type */

    if (strcmp(type, "char") == 0)
       {if (vr == NULL)
           obj = SS_null;
        else
           obj = SS_mk_string(vr);}

    else if (strcmp(type, "short") == 0)
       {short *sp;

        sp = (short *) vr;
        if ((nitems == 1) && (offset == 0))
           obj = SS_mk_integer((BIGINT) *sp);

        else
           {obj = SS_null;
            for (i = 0; i < nitems; i++)
                obj = SS_mk_cons(SS_mk_integer((BIGINT) sp[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};}

    else if (strcmp(type, "u_short") == 0)
       {unsigned short *sp;

        sp = (unsigned short *) vr;
        if ((nitems == 1) && (offset == 0))
           obj = SS_mk_integer((BIGINT) *sp);

        else
           {obj = SS_null;
            for (i = 0; i < nitems; i++)
                obj = SS_mk_cons(SS_mk_integer((BIGINT) sp[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};}

    else if ((strcmp(type, "integer") == 0) ||
	     (strcmp(type, "int") == 0))
       {int *ip;

        ip = (int *) vr;
        if ((nitems == 1) && (offset == 0))
           obj = SS_mk_integer((BIGINT) *ip);

        else
           {obj = SS_null;
            for (i = 0; i < nitems; i++)
                obj = SS_mk_cons(SS_mk_integer((BIGINT) ip[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};}

    else if ((strcmp(type, "u_integer") == 0) ||
	     (strcmp(type, "u_int") == 0))
       {unsigned int *ip;

        ip = (unsigned int *) vr;
        if ((nitems == 1) && (offset == 0))
           obj = SS_mk_integer((BIGINT) *ip);

        else
           {obj = SS_null;
            for (i = 0; i < nitems; i++)
                obj = SS_mk_cons(SS_mk_integer((BIGINT) ip[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};}

    else if (strcmp(type, "long") == 0)
       {long *lp;

        lp = (long *) vr;
        if ((nitems == 1) && (offset == 0))
            obj = SS_mk_integer((BIGINT) *lp);

        else
           {obj = SS_null;
            for (i = 0; i < nitems; i++)
                obj = SS_mk_cons(SS_mk_integer((BIGINT) lp[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};}

    else if (strcmp(type, "u_long") == 0)
       {unsigned long *lp;

        lp = (unsigned long *) vr;
        if ((nitems == 1) && (offset == 0))
            obj = SS_mk_integer((BIGINT) *lp);

        else
           {obj = SS_null;
            for (i = 0; i < nitems; i++)
                obj = SS_mk_cons(SS_mk_integer((BIGINT) lp[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};}

    else if (strcmp(type, "long_long") == 0)
       {BIGINT *bp;

        bp = (BIGINT *) vr;
        if ((nitems == 1) && (offset == 0))
            obj = SS_mk_integer(*bp);

        else
           {obj = SS_null;
            for (i = 0; i < nitems; i++)
                obj = SS_mk_cons(SS_mk_integer(bp[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};}

    else if (strcmp(type, "u_long_long") == 0)
       {BIGUINT *bp;

        bp = (BIGUINT *) vr;
        if ((nitems == 1) && (offset == 0))
            obj = SS_mk_integer((BIGINT) *bp);

        else
           {obj = SS_null;
            for (i = 0; i < nitems; i++)
                obj = SS_mk_cons(SS_mk_integer((BIGINT) bp[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};}

    else if (strcmp(type, "float") == 0)
       {float *fp;

        fp = (float *) vr;
        if ((nitems == 1) && (offset == 0))
           obj = SS_mk_float(*fp);

        else
           {obj = SS_null;
            for (i = 0; i < nitems; i++)
                obj = SS_mk_cons(SS_mk_float((double) fp[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};}

    else if (strcmp(type, "double") == 0)
       {double *dp;

        dp = (double *) vr;
        if ((nitems == 1) && (offset == 0))
           obj = SS_mk_float(*dp);
        else
           {obj = SS_null;
            for (i = 0; i < nitems; i++)
                obj = SS_mk_cons(SS_mk_float(dp[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};}

/* handle user defined primitive types by turning them into vectors of
 * integers containing the byte values
 * GOTCHA: if the byte size of an item matches a standard machine
 * type size make a vector of that type
 */
    else
       {long bytespitem, nb;
        char *cp;

        bytespitem = _PD_lookup_size(type, file->chart);
        nb         = bytespitem*nitems;

        cp = (char *) vr;
        if ((nb == 1) && (offset == 0))
           obj = SS_mk_integer((BIGINT) *cp);
        else
           {obj = SS_null;
            for (i = 0; i < nb; i++)
                obj = SS_mk_cons(SS_mk_integer((BIGINT) cp[i]), obj);

            if (obj != SS_null)
               obj = SS_lstvct(SS_reverse(obj));};};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
