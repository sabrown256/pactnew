/*
 * SXPML.C - PML extensions in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

typedef int (*PF_int_dd)(double, double);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_C_ARRAY - handle BLANG binding related operations */

void *_SX_opt_C_array(C_array *x, bind_opt wh, void *a)
   {void *rv;
    object *o;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
             if (SS_nullobjp(o))
                rv = NULL;
	     else if (SX_C_ARRAYP(o))
	        rv = SS_GET(C_array, o);
	     else
	        rv = _SX.unresolved;
	     break;

	case BIND_LABEL :
        case BIND_PRINT :
	     rv = x->type;
	     break;

        case BIND_ALLOC :
	     SC_mark(x, 1);
	     SC_mark(x->type, 1);
	     SC_mark(x->data, 1);
	     break;

        case BIND_FREE :
	     CFREE(x);

/*  GOTCHA - it's currently possible that some PM_set may still be pointing
 *  at type and/or array even though the reference count doesn't reflect it
 *            CFREE(x->type);
 *            CFREE(x->data);
 */
	     break;

        default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PCONS - handle BLANG binding related operations */

void *_SX_opt_pcons(pcons *x, bind_opt wh, void *a)
   {void *rv;
    object *o;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
             if (SS_nullobjp(o))
                rv = NULL;
	     else if (SX_PCONSP(o))
	        rv = SS_GET(pcons, o);
	     else
	        rv = _SX.unresolved;
	     break;

	case BIND_LABEL :
        case BIND_PRINT :
        case BIND_ALLOC :
        case BIND_FREE :
        default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_QUATERNION - handle BLANG binding related operations */

void *_SX_opt_quaternion(quaternion *x, bind_opt wh, void *a)
   {void *rv;
    object *o;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
             if (SS_nullobjp(o))
                rv = NULL;
	     else if (SX_QUATERNIONP(o))
	        rv = SS_GET(quaternion, o);
	     else
	        rv = _SX.unresolved;
	     break;

	case BIND_LABEL :
        case BIND_PRINT :
        case BIND_ALLOC :
        case BIND_FREE :
        default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PM_POLYGON - handle BLANG binding related operations */

void *_SX_opt_PM_polygon(PM_polygon *x, bind_opt wh, void *a)
   {void *rv;
    object *o;
    static char nm[BFSML];

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
	     if (SX_POLYGONP(o))
	        rv = SS_GET(PM_polygon, o);
	     else
	        rv = _SX.unresolved;
	     break;

	case BIND_LABEL :
             snprintf(nm, BFSML, "%ld", (long) x->nn);
	     rv = CSTRSAVE(nm);
	     break;

        case BIND_PRINT :
             snprintf(nm, BFSML, "%ld", (long) x->nn);
	     rv = nm;
	     break;

        case BIND_ALLOC :
	     SC_mark(x, 1);
	     SC_mark(x->x, 1);
	     break;

        case BIND_FREE :
	     PM_free_polygon(x);
	     break;

        default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PM_FIELD - handle BLANG binding related operations */

void *_SX_opt_PM_field(PM_field *x, bind_opt wh, void *a)
   {void *rv;
    object *o;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
	     if (SX_FIELDP(o))
	        rv = SS_GET(PM_field, o);
	     else
	        rv = _SX.unresolved;
	     break;

        case BIND_LABEL :
        case BIND_PRINT :
        case BIND_ALLOC :
        case BIND_FREE :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PM_SET - handle BLANG binding related operations */

void *_SX_opt_PM_set(PM_set *x, bind_opt wh, void *a)
   {void *rv;
    object *o;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
	     if (SX_SETP(o))
	        rv = SS_GET(PM_set, o);
	     else
	        rv = _SX.unresolved;
	     break;

        case BIND_LABEL :
        case BIND_PRINT :
	     rv = x->name;
	     break;

        case BIND_ALLOC :
        case BIND_FREE :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PM_MAPPING - handle BLANG binding related operations */

void *_SX_opt_PM_mapping(PM_mapping *x, bind_opt wh, void *a)
   {void *rv;
    object *o;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
	     if (SX_GRAPHP(o))
	        rv = SS_GET(PG_graph, o)->f;
	     else if (SX_MAPPINGP(o))
	        rv = SS_GET(PM_mapping, o);
	     else
	        rv = _SX.unresolved;
	     break;

        case BIND_LABEL :
        case BIND_PRINT :
	     rv = x->name;
	     break;

        case BIND_FREE :
	     PM_rel_mapping(x, TRUE, TRUE);
	     break;

        case BIND_ALLOC :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_OPT_PM_MESH_TOPOLOGY - handle BLANG binding related operations */

void *_SX_opt_PM_mesh_topology(PM_mesh_topology *x, bind_opt wh, void *a)
   {void *rv;
    object *o;

    rv = NULL;
    switch (wh)
       {case BIND_ARG :
	     o = (object *) a;
	     if (SX_MESH_TOPOLOGYP(o))
	        rv = SS_GET(PM_mesh_topology, o);
	     else
	        rv = _SX.unresolved;
	     break;

        case BIND_LABEL :
        case BIND_PRINT :
        case BIND_ALLOC :
        case BIND_FREE :
	default:
	     break;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MK_ARRAY - allocate and return a C_array
 *               - form: (pm-make-array <type> <size>)
 */

static object *_SXI_mk_array(SS_psides *si, object *argl)
   {long size;
    char *type;
    C_array *arr;
    object *rv;

    rv   = SS_null;
    type = NULL;
    size = 0L;
    SS_args(si, argl,
            SC_STRING_I, &type,
            SC_LONG_I, &size,
            0);

    if (SX_gs.vif != NULL)
       {defstr *dp;

	dp = PD_inquire_host_type(SX_gs.vif, type);
	if (dp != NULL)
	   type = dp->type;

	arr = PM_make_array(type, size, NULL);

	rv = SX_make_c_array(si, arr);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RESZ_ARRAY - doublelocate and return a C_array
 *                 - form: (pm-resize-array <array> <size>)
 */

static object *_SXI_resz_array(SS_psides *si, object *argl)
   {long os, size;
    intb bpi;
    C_array *arr;
    void *d;
    object *o;

    arr  = NULL;
    size = 0L;
    SS_args(si, argl,
            SX_C_ARRAY_I, &arr,
            SC_LONG_I, &size,
            0);

    if (arr != NULL)
       {PM_ARRAY_CONTENTS(arr, void, os, NULL, d);

	bpi = SC_arrlen(d)/os;

        arr->length = size;
        arr->data   = CREMAKE(d, char, size*bpi);};

    o = SS_car(si, argl);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SUB_ARRAY - allocate and return a C_array that is a sub-array of the
 *                - input array.
 *                - form: (pm-sub-array <array> <dimensions> <region>)
 */

static object *_SXI_sub_array(SS_psides *si, object *argl)
   {int i, nd, nr;
    long n, length, rlength;
    intb bpi;
    long *idims, *ireg, *pd, *pr;
    char typ[MAXLINE];
    void *d;
    C_array *arr, *newarr;
    object *dims, *reg, *obj, *rv;

    arr  = NULL;
    n    = 0;
    d    = NULL;
    rv   = SS_null;
    dims = SS_null;
    reg  = SS_null;
    SS_args(si, argl,
            SX_C_ARRAY_I, &arr,
            SS_OBJECT_I, &dims,
            SS_OBJECT_I, &reg,
            0);

    if (arr == NULL)
       SS_error(si, "NO ARRAY SPECIFIED - _SXI_SUB_ARRAY", argl);
    else
       {PM_ARRAY_CONTENTS(arr, void, n, typ, d);};

/* extract the array dimensions */
    nd    = 0;
    idims = NULL;
    if (SS_nullobjp(dims))
       SS_error(si, "NO ARRAY DIMENSIONS SPECIFIED - _SXI_SUB_ARRAY", argl);

    else
       {nd       = SS_length(si, dims);
        idims    = CMAKE_N(long, nd + 1);
	if (idims == NULL)
	   return(rv);

        idims[0] = nd/2;
        for (pd = idims + 1; !SS_nullobjp(dims); pd++)
            SX_GET_INTEGER_FROM_LIST(si, *pd, dims,
                                     "BAD ARRAY DIMENSIONS - _SXI_SUB_ARRAY");};

/* extract the region specifications */
    ireg = NULL;
    if (SS_nullobjp(reg))
       SS_error(si, "NO REGION SPECIFIED - _SXI_SUB_ARRAY", argl);

    else
       {nr   = SS_length(si, reg);
        ireg = CMAKE_N(long, nr + 1);
	if (ireg == NULL)
	   {CFREE(idims);
	    return(rv);};

        ireg[0] = nr/2;
        for (pr = ireg + 1; !SS_nullobjp(reg); pr++)
            SX_GET_INTEGER_FROM_LIST(si, *pr, reg,
                                     "BAD REGION - _SXI_SUB_ARRAY");};

/* do some error checking */
    length = 1;
    if (idims != NULL)
       {for (pd = idims + 1, i = 0; i < idims[0]; i++)
	    length *= pd[2*i + 1] - pd[2*i] + 1;};

    rlength = 1;
    if (ireg != NULL)
       {for (pr = ireg + 1, i = 0; i < ireg[0];  i++)
	    rlength *= pr[2*i + 1] - pr[2*i] + 1;};

    if (length != n)
       SS_error(si, "BAD DIMENSIONS SPECIFIED FOR ARRAY - _SXI_SUB_ARRAY", argl);

    if ((rlength > length)  || (rlength < 0))
       SS_error(si, "BAD REGION SPECIFIED FOR ARRAY - _SXI_SUB_ARRAY", argl);
    
/* allocate the output array */
    newarr = PM_make_array(typ, rlength, NULL);
    if (newarr != NULL)
       {bpi = SIZEOF(typ);

	PM_sub_array(d, newarr->data, idims, ireg, bpi);

	rv = SX_make_c_array(si, newarr);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ARRAY_REF - reference the nth element of a pm-array
 *                - form: (pm-array-ref <array> <n>)
 */

static object *_SXI_array_ref(SS_psides *si, object *argl)
   {long n;
    C_array *arr;
    object *o;

    arr = NULL;
    n   = 0L;
    SS_args(si, argl,
            SX_C_ARRAY_I, &arr,
            SC_LONG_I, &n,
            0);

    o = SS_null;
    if (arr != NULL)
       {char type[MAXLINE];

/* get the array type and dereference indirections down to a base type */
        SC_strncpy(type, MAXLINE, arr->type, -1);
	while (_PD_indirection(type))
	   PD_dereference(type);

/* in case the type is a typedef resolve it to a native type */
        if (SX_gs.vif != NULL)
           {defstr *dp;

            dp = PD_inquire_host_type(SX_gs.vif, type);
            if (dp != NULL)
               SC_strncpy(type, MAXLINE, dp->type, -1);};

	o = _SS_numtype_to_object(si, type, arr->data, n);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ARRAY_SET - set the nth element of a pm-array
 *                - form: (pm-array-set! <array> <n> <value>)
 */

static object *_SXI_array_set(SS_psides *si, object *argl)
   {long n;
    C_array *arr;
    object *val;

    arr = NULL;
    n   = 0L;
    SS_args(si, argl,
            SX_C_ARRAY_I, &arr,
            SC_LONG_I, &n,
            SS_OBJECT_I, &val,
            0);

    if (arr != NULL)
       {char type[MAXLINE];

        SC_strncpy(type, MAXLINE, arr->type, -1);
        if (SX_gs.vif != NULL)
           {defstr *dp;

            dp = PD_inquire_host_type(SX_gs.vif, type);
            if (dp != NULL)
               SC_strncpy(type, MAXLINE, dp->type, -1);};

	while (_PD_indirection(type))
	   PD_dereference(type);

	_SS_object_to_numtype(type, arr->data, n, val);};

    return(val);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_LIST_ARRAY - turn a list of numbers into a numeric array */

object *SX_list_array(SS_psides *si, object *argl)
   {int n, fixp;
    long *lp;
    double *fp;
    C_array *arr;
    object *num, *lst, *rv;

    n    = 0;
    lst  = argl;
    for (lst = argl; !SS_nullobjp(lst); lst = SS_cdr(si, lst))
        {num = SS_car(si, lst);
         if (!SS_numbp(num))
            SS_error(si, "LIST ELEMENT NOT A NUMBER - SX_LIST_ARRAY", num);

         n++;};

/* determine whether the list is all integers */
    fixp = TRUE;
    for (lst = argl; !SS_nullobjp(lst); lst = SS_cdr(si, lst))
        fixp &= SS_integerp(SS_car(si, lst));

    if (fixp)
       {arr = PM_make_array(SC_LONG_S, n, NULL);
	lp  = (long *) arr->data;
	for (lst = argl; !SS_nullobjp(lst); lst = SS_cdr(si, lst), lp++)
	    {SS_args(si, lst,
		     SC_LONG_I, lp,
		     0);};}
    else
       {arr = PM_make_array(SC_DOUBLE_S, n, NULL);
	fp  = (double *) arr->data;
	for (lst = argl; !SS_nullobjp(lst); lst = SS_cdr(si, lst), fp++)
	    {SS_args(si, lst,
		     SC_DOUBLE_I, fp,
		     0);};};

    rv = SX_make_c_array(si, arr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_LIST_ARRAY - wrapper over SX_list_array */

object *_SXI_list_array(SS_psides *si, object *argl)
   {object *rv;

    rv = SX_list_array(si, argl);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ARRAY_LIST - turn a numeric array into a list of numbers */

static object *_SXI_array_list(SS_psides *si, object *argl)
   {long n;
    char type[MAXLINE];
    object *lst;
    C_array *arr;
    void *data;

    arr = NULL;
    SS_args(si, argl,
            SX_C_ARRAY_I, &arr,
            0);

    if (arr == NULL)
       return(SS_null);

    PM_ARRAY_CONTENTS(arr, void, n, type, data);

    lst = _SS_numtype_to_list(si, type, data, n);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_NUM_ARR_LEN - return the length of a numeric array */

static object *_SXI_num_arr_len(SS_psides *si, object *obj)
   {long n;
    object *o;

    if (!SX_C_ARRAYP(obj))
       SS_error(si, "ARGUMENT NOT NUMERIC ARRAY - _SXI_NUM_ARR_LEN", obj);

    n = C_ARRAY_LENGTH(obj);

    o = SS_mk_integer(si, n);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_NUM_ARR_EXTR - return the extrema of a numeric array */

static object *_SXI_num_arr_extr(SS_psides *si, object *arg)
   {int n, imin, imax;
    char *type;
    void *data;
    object *lst, *obj;
    double fmn, fmx;
    double *d;

    if (!SX_C_ARRAYP(arg))
       SS_error(si, "ARGUMENT NOT NUMERIC ARRAY - _SXI_NUM_ARR_EXTR", arg);

    n    = C_ARRAY_LENGTH(arg);
    type = C_ARRAY_TYPE(arg);
    data = C_ARRAY_DATA(arg);

    d = PM_array_real(type, data, n, NULL);

    PM_minmax(d, n, &fmn, &fmx, &imin, &imax);

    CFREE(d);

    lst = SS_null;
    obj = SS_mk_integer(si, imax);
    lst = SS_mk_cons(si, obj, lst);
    obj = SS_mk_integer(si, imin);
    lst = SS_mk_cons(si, obj, lst);
    obj = SS_mk_float(si, fmx);
    lst = SS_mk_cons(si, obj, lst);
    obj = SS_mk_float(si, fmn);
    lst = SS_mk_cons(si, obj, lst);

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_PDBDATA - given a PM_set object
 *                  - allocate and fill a PDB data object with the
 *                  - PM_set and return it
 */

static object *_SXI_set_pdbdata(SS_psides *si, object *argl)
   {char *mn, set_name[MAXLINE];
    PM_set *s;
    g_file *po;
    PDBfile *file;
    object *rv;

    s    = NULL;
    po   = NULL;
    mn   = NULL;
    file = NULL;
    SS_args(si, argl,
            SX_SET_I, &s,
            G_FILE, &po,
            SC_STRING_I, &mn,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;

    else if (strcmp(po->type, PDBFILE_S) == 0)
       file = FILE_FILE(PDBfile, po);

    else
       SS_error(si, "BAD FILE - _SXI_SET_PDBDATA", argl);

    if (mn == NULL)
       SC_strncpy(set_name, MAXLINE, s->name, -1);
    else
       {SC_strncpy(set_name, MAXLINE, mn, -1);
        CFREE(mn);};

    if (s == NULL)
       SS_error(si, "BAD ARGUMENT - _SXI_SET_PDBDATA", argl);

    rv = SX_pdbdata_handler(si, file, set_name, "PM_set *", &s, TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDBDATA_SET - read a PM_set out of a PDB file and
 *                  - return a PML set object
 *                  - FORM:
 *                  -    (pdbdata->pm-set <file> <name>)
 */

static object *_SXI_pdbdata_set(SS_psides *si, object *argl)
   {char *name;
    PDBfile *file;
    g_file *po;
    syment *ep;
    SC_address data;
    PM_set *s;
    object *obj;

    if (!SS_consp(argl))
       SS_error(si, "BAD ARGUMENT LIST - _SXI_PDBDATA_SET", argl);

/* if the first object is a pdbfile, use it, otherwise, use default file */
    argl = SX_get_file(si, argl, &po);
    file = FILE_FILE(PDBfile, po);

    obj  = SS_car(si, argl);
    argl = SS_cdr(si, argl);
    name = CSTRSAVE(SS_get_string(obj));

/* check to see whether or not the variable is in the file */
    ep = PD_inquire_entry(file, name, TRUE, NULL);
    if (ep == NULL)
       obj = SS_null;

    else

/* read the set */
       {if (file == SX_gs.vif)
	   {data.diskaddr = PD_entry_address(ep);
	    s = *(PM_set **) data.memaddr;}
        else
	   {if (!PD_read(file, name, &data.memaddr))
	       SS_error(si, PD_get_error(), obj);
	    s = (PM_set *) data.memaddr;};

	if (s->info_type == NULL)
	   s->info_type = SC_PCONS_P_S;

	obj = SX_make_pm_set(si, s);};

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_PML_SET - build a PM_set from some numeric arrays at SX level
 *                   -
 *                   - Form:
 *                   -
 *                   - (pm-make-set <name>
 *                   -               <mesh-shape-list>
 *                   -               <element-arrays>)
 *                   -
 *                   - <name>            - name of the set
 *                   - <mesh-shape-list> - List of numbers specifying the mesh
 *                   -                   - shape.  These numbers are the 
 *                   -                   - maximum values of the indexes in 
 *                   -                   - each direction.  The minima are
 *                   -                   - assumed to be zero.
 *                   - <element-arrays>  - List of numeric arrays whose
 *                   -                   - entries specify the elements of
 *                   -                   - the set.
 *                   - Example:
 *                   -
 *                   - (pm-make-set "section" '(10 20) '(x y z))
 *                   -
 *                   - This specifies a two dimensional section of a three
 *                   - dimensional space.  The mesh has 10x20 elements and
 *                   - the numeric arrays x, y, and z are each 200 long.
 *                   -
 *                   - If no element arrays are given a logical set is
 *                   - built from the type and shape specifications.
 *                   -
 *                   - (pm-make-set "section" '(10 20))
 *                   -
 *                   - This specifies a two dimensional set of 2 vectors.
 *                   - The mesh has 10x20 elements which go from (0,0) to (9,19).
 *                   -
 *                   - Note: Logical rectangular meshes are assumed.
 *                   -       This can be generalized later by having the
 *                   -       <mesh-shape-list> specify the mesh connectivity
 *                   -       more generally.
 */

static object *_SXI_make_pml_set(SS_psides *si, object *argl)
   {int *maxes, *pm, nd, ne, nde;
    char *name, *type;
    void **elem, **pe;
    PM_set *set;
    C_array *arr;
    object *obj, *shape, *components, *rv;

    nd    = 0;
    name  = NULL;
    shape = SS_null;
    components = SS_null;
    SS_args(si, argl,
            SC_STRING_I, &name,
            SS_OBJECT_I, &shape,
            SS_OBJECT_I, &components,
            0);

/* extract the name */
    if (name == NULL)
       SS_error(si, "BAD NAME - _SXI_MAKE_PML_SET", argl);

/* extract the mesh shape */
    maxes = NULL;
    if (SS_nullobjp(shape))
       SS_error(si, "BAD MESH SHAPE - _SXI_MAKE_PML_SET", argl);
    else
       {nd    = SS_length(si, shape);
        maxes = CMAKE_N(int, nd);
        for (pm = maxes; !SS_nullobjp(shape); )
            SX_GET_INTEGER_FROM_LIST(si, *pm++, shape,
                                     "BAD MESH INDEX - _SXI_MAKE_PML_SET");};

/* extract the set elements */
    if (SS_nullobjp(components))
       set = PM_make_lr_index_domain(name, SC_DOUBLE_S, nd, nd,
				     maxes, NULL, NULL);

    else
       {nde  = SS_length(si, components);
	elem = CMAKE_N(void *, nde);

/* get the number of elements */
	obj = SS_car(si, components);
	if (!SX_C_ARRAYP(obj))
	   SS_error(si, "OBJECT NOT NUMERIC ARRAY - _SXI_MAKE_PML_SET", obj);

	ne      = C_ARRAY_LENGTH(obj);
	type    = C_ARRAY_TYPE(obj);
	elem[0] = C_ARRAY_DATA(obj);

	for (pe = elem; !SS_nullobjp(components); )
	    {arr        = NULL;
	     obj        = SS_car(si, components);
	     components = SS_cdr(si, components);
	     if (SX_C_ARRAYP(obj))
	        arr = C_ARRAY(obj);
	     else
	        SS_error(si, "BAD ELEMENT ARRAY - _SXI_MAKE_PML_SET", obj);

	     if (arr != NULL)
	        {if (strcmp(SX_gs.promotion_type, "none") != 0)
		    {PM_promote_array(arr, SX_gs.promotion_type, TRUE);
		     type = arr->type;};

		 *pe++ = arr->data;};};

	set = PM_mk_set(name, type, FALSE,
			ne, nd, nde, maxes, elem,
			NULL, NULL,
			NULL, NULL, NULL, NULL, NULL, NULL,
			NULL);};

    rv = SX_make_pm_set(si, set);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_CP_SET - build and return the PML set object which is the
 *                  - cartesian product of the argument sets
 *                  -
 *                  - FORM:
 *                  - (pm-make-cartesian-product-set [<set>]*)
 */

static object *_SXI_make_cp_set(SS_psides *si, object *argl)
   {int i, n;
    char *name;
    PM_set **sets, *cp;
    object *obj;

    n = SS_length(si, argl);
    sets = CMAKE_N(PM_set *, n);

    for (i = 0; i < n; i++)
        {SX_GET_SET_FROM_LIST(si, sets[i], argl,
			      "ARGUMENT NOT SET - _SXI_MAKE_CP_SET");};

    name = SC_dsnprintf(FALSE, "CP %d", n);

    cp = PM_make_lr_cp_domain(name, SC_DOUBLE_S, n, sets);

    CFREE(sets);

    obj = SX_make_pm_set(si, cp);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_PML_MAPPING - build a PML mapping object out of a domain, range
 *                       - and attribute set
 *                       -
 *                       - FORM:
 *                       - (pm-make-mapping <domain> <range>
 *                       -                  [<centering> <category> <name>
 *                       -                   <emap> <next>])
 *                       -
 *                       - Centering defaults to zone
 *                       - Existence map thru which to plot defaults to all 1's
 *                       - mapping name
 */

static object *_SXI_make_pml_mapping(SS_psides *si, object *argl)
   {char *lbl, *name, *category;
    PM_centering centering;
    PM_mapping *f, *next;
    PM_set *domain, *range;
    C_array *arr;
    object *o;

    centering = N_CENT;
    category  = PM_LR_S;
    arr       = NULL;
    name      = NULL;
    next      = NULL;
    SS_args(si, argl,
            SX_SET_I, &domain,
            SX_SET_I, &range,
            SC_ENUM_I, &centering,
            SC_STRING_I, &category,
            SC_STRING_I, &name,
	    SX_C_ARRAY_I, &arr,
	    SX_MAPPING_I, &next,
            0);

    if (name == NULL)
       lbl = SC_dsnprintf(FALSE, "%s->%s", domain->name, range->name);
    else
       lbl = SC_dsnprintf(FALSE, name);

    f = PM_make_mapping(lbl, category, domain, range, centering, next);

/* if an existence map was supplied add it to the mapping's attribute list */
    if (arr != NULL)
       {int sid;
	long n;
	char type[MAXLINE];
	char *emap;
	void *data;

	PM_ARRAY_CONTENTS(arr, void, n, type, data);

	sid  = SC_type_id(type, FALSE);
        emap = SC_convert_id(SC_CHAR_I, NULL, 0, 1,
			     sid, data, 0, 1, n, FALSE);

	PG_set_attrs_mapping(f,
			     "EXISTENCE", SC_CHAR_I, TRUE, emap,
			     NULL);};

    o = SX_make_pm_mapping(si, f);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_TEXT_SET_NAME - given a file and reference to a set by
 *                        - name or menu number,
 *                        - return the set name as an object
 */

static object *_SXI_get_text_set_name(SS_psides *si, object *set)
   {PM_set *s;
    object *o;

    o = SS_null;
    if (SX_SETP(set))
       {s = SS_GET(PM_set, set);
        o = SS_mk_string(si, s->name);};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_ATTR_SET - set an attribute of a PM_set
 *                   - usage: (pm-set-set-attribute! <set>
 *                   -                               <name> <type> <value>)
 */

static object *_SXI_set_attr_set(SS_psides *si, object *argl)
   {PM_set *s;
    char *name, *type;
    object *val, *o, *rv;
    pcons *inf;

    rv = SS_f;

    s    = NULL;
    o    = NULL;
    name = NULL;
    type = NULL;
    val  = SS_null;
    SS_args(si, argl,
            SS_OBJECT_I, &o,
            SC_STRING_I, &name,
            SC_STRING_I, &type,
            SS_OBJECT_I, &val,
            0);

    if ((o == NULL) || (name == NULL) || (type == NULL))
       SS_error(si, "INSUFFICIENT ARGUMENTS - _SXI_SET_ATTR_SET", argl);

    else if (SX_SETP(o))
       {s = SS_GET(PM_set, o);

/* get the current list */
	if (s->info_type != NULL)
	   {if (strcmp(s->info_type, SC_PCONS_P_S) == 0)
	       inf = (pcons *) s->info;
	    else
	       inf = NULL;}
	else
	   inf = NULL;

	s->info      = SX_set_attr_alist(si, inf, name, type, val);
	s->info_type = SC_PCONS_P_S;

	rv = SS_t;};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_MAP_TYPE - set the type of a PM_mapping or the mapping part of
 *                   - a PG_graph object
 */

static object *_SXI_set_map_type(SS_psides *si, object *argl)
   {PM_mapping *f;
    char *name;

    f    = NULL;
    name = NULL;
    SS_args(si, argl,
            SX_MAPPING_I, &f,
            SC_STRING_I, &name,
            0);

    if ((f == NULL) || (name == NULL))
       SS_error(si, "INSUFFICIENT ARGUMENTS - _SXI_SET_MAP_TYPE", argl);

    else
       f->map_type = CSTRSAVE(name);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAPPING_PDBDATA - given a PM_mapping object
 *                      - allocate and fill a PDB data object with the
 *                      - PM_mapping and return it
 */

static object *_SXI_mapping_pdbdata(SS_psides *si, object *argl)
   {PM_mapping *f, *pf;
    g_file *po;
    PDBfile *file;
    char *mn, *name;
    long i;
    object *ret;

    f    = NULL;
    po   = NULL;
    mn   = NULL;
    file = NULL;
    SS_args(si, argl,
            SX_MAPPING_I, &f,
            G_FILE, &po,
            SC_STRING_I, &mn,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       {po   = SX_gs.gvif;
        file = SX_gs.vif;}

    else if (strcmp(po->type, PDBFILE_S) == 0)
       file = FILE_FILE(PDBfile, po);

    else
       SS_error(si, "BAD FILE - _SXI_MAPPING_PDBDATA", argl);

    if (f == NULL)
       SS_error(si, "BAD ARGUMENT - _SXI_MAPPING_PDBDATA", argl);

    if (mn == NULL)
       {_SX_get_menu(si, po);
        for (i = 0; TRUE; i++)
            {name = SC_dsnprintf(FALSE, "/Mapping%ld", i);
             if (PD_inquire_entry(file, name, TRUE, NULL) == NULL)
                break;};}
    else
       {name = SC_dsnprintf(FALSE, mn);
        CFREE(mn);};

/* disconnect any function pointers or undefined structs/members */
    for (pf = f; pf != NULL; pf = pf->next)
        {pf->domain->opers = NULL;
         pf->range->opers = NULL;};

/* make sure that the necessary types are known */
    if (PD_inquire_type(file, "PM_mapping") == NULL)
       PD_def_mapping(file);

    ret = SX_pdbdata_handler(si, file, name, "PM_mapping *", &f , TRUE);

/* add to menu */
    _SX_push_menu_item(si, po, name, "PM_mapping *");

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDBDATA_MAPPING - read a PM_mapping out of a PDB file and
 *                      - return a PML mapping object
 *                      - FORM:
 *                      -    (pdbdata->pm-mapping <file> <name>)
 */

static object *_SXI_pdbdata_mapping(SS_psides *si, object *argl)
   {int i, ret;
    char *name, dname[MAXLINE];
    PDBfile *file;
    g_file *po;
    syment *ep;
    SC_address data;
    PM_set *domain, *range;
    PM_mapping *pf, *f;
    SX_menu_item *mi;
    object *obj, *rv;

    rv = SS_null;

    if (!SS_consp(argl))
       SS_error(si, "BAD ARGUMENT LIST - _SXI_PDBDATA_MAPPING", argl);

/* if the first object is a pdbfile, use it, otherwise, use default file */
    argl = SX_get_file(si, argl, &po);
    file = FILE_FILE(PDBfile, po);

    obj = SS_car(si, argl);
    if (SS_integerp(obj))
       {i    = SS_INTEGER_VALUE(obj);
	mi   = _SX_get_menu_item(si, po, i);
	name = (mi == NULL) ? NULL : mi->vname;}
    else
       {argl = SS_cdr(si, argl);
        name = CSTRSAVE(SS_get_string(obj));};

    if (name == NULL)
       return(SS_null);

/* check to see whether or not the variable is in the file */
    ep = PD_inquire_entry(file, name, TRUE, NULL);
    if (ep == NULL)
       return(SS_null);

/* read the mapping */
    if (file == SX_gs.vif)
       {data.diskaddr = PD_entry_address(ep);
        data.memaddr  = DEREF(data.memaddr);}
    else
       {if (!PD_read(file, name, &data.memaddr))
           SS_error(si, PD_get_error(), obj);};

    PD_reset_ptr_list(file);

/* reconnect any function pointers or undefined structs/members */
    f = (PM_mapping *) data.memaddr;
    if (f != NULL)
       {if (f->domain == NULL)
	   {SC_strncpy(dname, MAXLINE, f->name, -1);
	    PD_process_set_name(dname);

	    if (!PD_read(file, dname, &data.memaddr))
	       SS_error(si, PD_get_error(), SS_null);

	    f->domain = (PM_set *) data.memaddr;};

	ret = TRUE;
	for (pf = f; pf != NULL; pf = pf->next)
	    {domain = pf->domain;
	     range  = pf->range;
	     if (domain != NULL)
	        {ret &= PM_set_opers(domain);
		 if (domain->info_type == NULL)
		    domain->info_type = SC_PCONS_P_S;};

	     if (range != NULL)
	        {ret &= PM_set_opers(range);
		 if (range->info_type == NULL)
		    range->info_type = SC_PCONS_P_S;};

	     if (ret == FALSE)
	        SS_error(si, "NO FIELD FOR TYPE - _SXI_PDBDATA_MAPPING", SS_null);};

	rv = SX_make_pm_mapping(si, f);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ARRAYS_SET - convert the data from a list of C_array objects to
 *                 - a set of one higher dimension than the arrays
 *                 - FORM:
 *                 -    (pm-arrays->set name <array> ...)
 */

static object *_SXI_arrays_set(SS_psides *si, object *argl)
   {int i, j, n, sid;
    int nd, ne, nde, nep, tflag;
    int *maxes, *pm;
    char type[MAXLINE];
    char *name;
    double **elem, *pe;
    void *data;
    object *obj, *components, *shape, *lst;
    C_array *arr;
    PM_set *set;

    n     = 0;
    name  = NULL;
    shape = SS_null;
    components = SS_null;
    tflag = FALSE;
    SS_args(si, argl,
            SC_STRING_I, &name,
            SC_INT_I, &tflag,
            SS_OBJECT_I, &shape,
            0);

    components = SS_cdddr(si, argl);

/* extract the name */
    if (name == NULL)
       SS_error(si, "BAD NAME - _SXI_ARRAYS_SET", argl);

/* extract the mesh shape */
    if (SS_nullobjp(shape))
       {nd    = SS_length(si, components) + tflag;
        maxes = CMAKE_N(int, nd);
        maxes[nd-1] = SS_length(si, SS_car(si, components));}
    else
       {nd    = SS_length(si, shape);
        maxes = CMAKE_N(int, nd);
        for (pm = maxes; !SS_nullobjp(shape); )
            SX_GET_INTEGER_FROM_LIST(si, *pm++, shape,
                                     "BAD MESH INDEX - _SXI_ARRAYS_SET");};

/* each component is a list of arrays */
    nde  = SS_length(si, components) + tflag;
    elem = CMAKE_N(double *, nde);
    for (lst = components; !SS_nullobjp(lst); lst = SS_cdr(si, lst))
        {if (lst == components)
            n = SS_length(si, SS_car(si, lst));
         else if (n != SS_length(si, SS_car(si, lst)))
            SS_error(si, "COMPONENT LISTS NOT SAME LENGTH - _SXI_ARRAYS_SET",
                     lst);};

/* get the number of elements */
    lst = SS_caar(si, components);
    if (!SX_C_ARRAYP(lst))
       SS_error(si, "OBJECT NOT NUMERIC ARRAY - _SXI_ARRAYS_SET", lst);

    SC_strncpy(type, MAXLINE, C_ARRAY_TYPE(lst), -1);
    PD_dereference(type);
    sid = SC_type_id(type, FALSE);

    data = C_ARRAY_DATA(lst);
    nep  = C_ARRAY_LENGTH(lst);
    ne   = nep*n;

    for (j = 0; !SS_nullobjp(components); j++, components = SS_cdr(si, components))
        {pe = CMAKE_N(double, ne);
         elem[j] = pe;
         lst = SS_car(si, components);
         for (i = 0; i < n; i++)
             {SX_GET_C_ARRAY_FROM_LIST(si, arr, lst,
				       "BAD ELEMENT ARRAY - _SXI_ARRAYS_SET");

	      data = arr->data;
              SC_convert_id(SC_DOUBLE_I, pe, 0, 1,
			    sid, data, 0, 1, nep, FALSE);

              pe += nep;};};

/* the new component must be made */
    if (tflag)
       {pe = CMAKE_N(double, ne);
        elem[j] = pe;
        for (i = 0; i < n; i++)
            {for (j = 0; j < nep; j++)
                 *pe++ = i;};};

    set = PM_mk_set(name, SC_DOUBLE_S, FALSE,
		    ne, nd, nde, maxes, elem,
		    NULL, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL,
		    NULL);

    obj = SX_make_pm_set(si, set);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_LR_AC - convert an LR based mapping into an AC based one
 *            - FORM:
 *            -    (pm-lr->ac <mapping>)
 */

static object *_SXI_lr_ac(SS_psides *si, object *argl)
   {int ord, *maxes;
    void **elements;
    PM_centering cent;
    PM_mapping *f, *g;
    PM_set *odom, *oran, *ndom, *nran;
    PM_mesh_topology *mt;
    object *rv;

    rv  = SS_null;
    f   = NULL;
    ord = BND_CELL_MAX;
    SS_args(si, argl,
            SX_MAPPING_I, &f,
            SC_INT_I, &ord,
            0);

    if (f == NULL)
       SS_error(si, "BAD MAPPING - _SXI_LR_AC", argl);

    else
       {odom = f->domain;
	oran = f->range;

/* find the additional mapping information */
	cent = N_CENT;
	PM_mapping_info(f,
			"CENTERING", &cent,
			NULL);

	if (odom->dimension != 2)
	   SS_error(si, "ONLY 2D MESHES CURRENTLY - _SXI_LR_AC", SS_null);

	else
	   {int kmax, lmax;
	    double *x, *y, *px, *py;

	    maxes = odom->max_index;
	    kmax = maxes[0];
	    lmax = maxes[1];

	    elements = (void **) odom->elements;
	    px = x = (double *) elements[0];
	    py = y = (double *) elements[1];

	    mt = PM_lr_ac_mesh_2d(&x, &y, kmax, lmax, 1, kmax, 1, lmax, ord);

/* check the new mesh */
	    {int i, nn;
	     nn = kmax*lmax;
	     for (i = 0; i < nn; i++)
	         {if ((x[i] != px[i]) || (y[i] != py[i]))
		     SS_error(si, "BAD CONVERSION - _SXI_LR_AC", SS_null);};};

	    elements = CMAKE_N(void *, 2);
	    elements[0] = x;
	    elements[1] = y;

	    ndom = PM_mk_set(odom->name, SC_DOUBLE_S, FALSE, odom->n_elements,
			     odom->dimension, odom->dimension_elem,
			     odom->max_index, elements,
			     odom->opers, odom->metric,
			     odom->symmetry_type, odom->symmetry,
			     PM_MESH_TOPOLOGY_P_S, mt,
			     odom->info_type, odom->info, NULL);

	    nran = PM_copy_set(oran);

	    g = PM_make_mapping(f->name, PM_AC_S, ndom, nran, cent, NULL);

	    rv = SX_make_pm_mapping(si, g);};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_MAKE_AC_SET - make an Arbitrarily-Connect set from
 *                  - lists of cell boundary specs
 *                  -    (pm-make-ac-set nodes edges faces zones ... )
 *                  - where
 *                  -
 *                  - nodes is a list of n-tuples (list of number)
 *                  -       representing the node position
 *                  - edges is a list of 1cells, faces 2cells, etc
 *                  - an ncell is a list of numbers describing the
 *                  - boundary of the ncell
 *                  - see the description in pml.h for details
 */

static object *_SXI_make_ac_set(SS_psides *si, object *argl)
   {int i, j, k;
    int ne, nd, nde;
    int *nbp, *ncs, nc, ord;
    long **bnd, *pb;
    double **elem;
    char *name;
    object *nodes, *node, *ncells, *ncell, *o;
    PM_set *set;
    PM_mesh_topology *mt;

    nd = SS_length(si, argl);
    if (nd < 1)
       SS_error(si, "NO DOMAIN INFO - _SXI_MAKE_AC_SET", argl);

    nodes = SS_car(si, argl);
    argl  = SS_cdr(si, argl);

    ne = SS_length(si, nodes);
    if (ne < 1)
       SS_error(si, "BAD NODE LIST - _SXI_MAKE_AC_SET", nodes);

    nde = SS_length(si, SS_car(si, nodes));

/* construct the element arrays from the lists */
    elem = CMAKE_N(double *, nde);
    for (i = 0; i < nde; i++)
        elem[i] = CMAKE_N(double, ne);

    for (i = 0; i < ne; i++, nodes = SS_cdr(si, nodes))
        {node = SS_car(si, nodes);
	 for (j = 0; j < nde; j++, node = SS_cdr(si, node))
             {SS_args(si, node,
		      SC_DOUBLE_I, elem[j]+i,
		      0);};};

/* construct the cell boundary arrays */
    bnd = CMAKE_N(long *, nd);
    ncs = CMAKE_N(int, nd);
    nbp = CMAKE_N(int, nd);
    for (j = 1; j < nd; j++, argl = SS_cdr(si, argl))
        {ncells = SS_car(si, argl);
         ord = SS_length(si, SS_car(si, ncells));
         nc  = SS_length(si, ncells);
         pb  = CMAKE_N(long, ord*nc);
         bnd[j] = pb;
         ncs[j] = (ord == 1) ? nc - 1 : nc;
         nbp[j] = ord;
         for (i = 0; i < nc; i++, ncells = SS_cdr(si, ncells))
             {ncell = SS_car(si, ncells);
	      for (k = 0; k < ord; k++, ncell = SS_cdr(si, ncell))
                  {SS_args(si, ncell,
			   SC_LONG_I, pb++,
			   0);};};};

/* reduce the number of dimensions to its proper value now */
    nd--;

/* fill in the 0d part */
    bnd[0] = NULL;
    ncs[0] = ne;
    nbp[0] = 1;

/* put it all together */
    mt = PM_make_topology(nd, nbp, ncs, bnd);

    name = SC_dsnprintf(FALSE, "D%d-%d", nd, nde);

    set = PM_mk_set(name, SC_DOUBLE_S, FALSE,
		    ne, nd, nde, NULL, elem,
		    NULL, NULL, NULL, NULL,
		    PM_MESH_TOPOLOGY_P_S, mt,
		    NULL, NULL, NULL);

    o = SX_make_pm_set(si, set);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ARRAY_PDBDATA - convert the data from a C_array object to
 *                    - a pdbdata object
 *                    - FORM:
 *                    -    (pm-array->pdbdata <array> <file> [<name>])
 */

static object *_SXI_array_pdbdata(SS_psides *si, object *argl)
   {char *mn, *name;
    C_array *arr;
    g_file *po;
    PDBfile *file;
    object *rv;

    rv   = SS_null;
    arr  = NULL;
    po   = NULL;
    mn   = NULL;
    file = NULL;
    name = NULL;
    SS_args(si, argl,
            SX_C_ARRAY_I, &arr,
            G_FILE, &po,
            SC_STRING_I, &mn,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;

    else if (strcmp(po->type, PDBFILE_S) == 0)
       file = FILE_FILE(PDBfile, po);

    else
       SS_error(si, "BAD FILE - _SXI_ARRAY_PDBDATA", argl);

    if (arr == NULL)
       SS_error(si, "INVALID ARRAY OBJECT - _SXI_ARRAY_PDBDATA", argl);

    else if (mn == NULL)
       name = SC_dsnprintf(FALSE, "Pm-Array%d", _SX.ap++);

    else
       {name = SC_dsnprintf(FALSE, mn);
        CFREE(mn);};

    rv = SX_pdbdata_handler(si, file, name, "C_array", arr, TRUE);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ARRAY_PDBDATA_I - inverse of PDBDATA->PM-ARRAY
 *                      - a C_array object
 *                      - FORM:
 *                      -    (pm-array->pdbdata-i <array> <file> [<name>])
 */

static object *_SXI_array_pdbdata_i(SS_psides *si, object *argl)
   {char type[MAXLINE];
    char *mn, *pt, *name;
    void *x;
    C_array *arr;
    g_file *po;
    PDBfile *file;
    object *rv;

    rv   = SS_null;
    arr  = NULL;
    po   = NULL;
    mn   = NULL;
    file = NULL;
    SS_args(si, argl,
            SX_C_ARRAY_I, &arr,
            G_FILE, &po,
            SC_STRING_I, &mn,
            0);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;

    else if (strcmp(po->type, PDBFILE_S) == 0)
       file = FILE_FILE(PDBfile, po);

    else
       SS_error(si, "BAD FILE - _SXI_ARRAY_PDBDATA_I", argl);

    if (arr == NULL)
       SS_error(si, "INVALID ARRAY OBJECT - _SXI_ARRAY_PDBDATA_I", argl);

    else if (mn == NULL)
       name = SC_dsnprintf(FALSE, "Pm-Array%d", _SX.api++);

    else
       {name = SC_dsnprintf(FALSE, mn);
        CFREE(mn);};

    if (arr != NULL)
       {SC_strncpy(type, MAXLINE, arr->type, -1);
	pt = SC_firsttok(type, " *");
	x  = arr->data;

	rv = SX_pdbdata_handler(si, file, name, pt, x, TRUE);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PDBDATA_ARRAY - convert the data from a pdbdata object to
 *                    - a C_array object
 *                    - NOTE: if the pdbdata object contains a C_array already
 *                    -       just use it
 *                    - FORM:
 *                    -    (pdbdata->pm-array <pdbdata>)
 */

static object *_SXI_pdbdata_array(SS_psides *si, object *arg)
   {g_pdbdata *pd;
    syment *ep;
    C_array *arr;
    object *rv;

    rv = SS_null;
    pd = NULL;
    SS_args(si, arg,
            G_PDBDATA, &pd,
            0);

    if (pd == NULL)
       SS_error(si, "INVALID PDBDATA OBJECT - _SXI_PDBDATA_ARRAY", arg);

    else
       {ep = pd->ep;

	if (strcmp(PD_entry_type(ep), "C_array") == 0)
	   arr = (C_array *) pd->data;
	else
	   arr = PM_make_array(PD_entry_type(ep), PD_entry_number(ep),
			       pd->data);

	rv = SX_make_c_array(si, arr);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_REP_TO_AC - map a the given connectivity representation into
 *              - the PACT mesh topology representation
 *              - and return a domain set
 *
 * #bind SX_rep_to_ac fortran()
 */

PM_set *SX_rep_to_ac(char *name, double *rx, double *ry,
		     int n_nodes, int n_zones, int *zones)
   {int iz, j, ja, jb, incr;
    int n_sides, *nc, *nbp, *pzone;
    double **elem;
    PM_set *s;
    PM_mesh_topology *mt;
    long **bnd, *ncell, *pcell;

    elem = CMAKE_N(double *, 2);
    elem[0] = rx;
    elem[1] = ry;

/* allocate the boundary arrays */
    bnd = CMAKE_N(long *, 3);
    bnd[2] = CMAKE_N(long, 2*n_zones);

/* fill the 2-cells */
    ncell   = bnd[2];
    n_sides = 0;
    pzone   = zones;

#pragma omp parallel for private(incr, pcell) shared(n_sides)
    for (iz = 0; iz < n_zones; iz++)
        {incr  = 0;
         while (pzone[incr] != -1)
            incr++;
         pcell = ncell + 2*iz;
	 pcell[BND_CELL_MIN] = n_sides;
	 pcell[BND_CELL_MAX] = n_sides + incr - 1;
	 n_sides += incr;
         pzone   += incr + 1;};

    bnd[1] = CMAKE_N(long, 2*n_sides);

/* fill the 1-cells */
    pcell = bnd[1];
    pzone = zones;
    for (iz = 0; iz < n_zones; iz++)
        {incr  = 0;
         while (pzone[incr] != -1)
            incr++;
         for (j = 0; j < incr; j++)
             {ja = j;
	      jb = (j + 1) % incr;
	      pcell[BND_CELL_MIN] = pzone[ja];
	      pcell[BND_CELL_MAX] = pzone[jb];
              pcell += 2;};
         pzone   += incr + 1;};

    bnd[0] = NULL;

/* setup the number of cells array */
    nc = CMAKE_N(int, 3);
    nc[0] = n_nodes;
    nc[1] = n_sides;
    nc[2] = n_zones;

/* setup the number of boundary parameters array */
    nbp = CMAKE_N(int, 3);
    nbp[0] = 1;
    nbp[1] = 2;
    nbp[2] = 2;

/* put it all together */
    mt = PM_make_topology(2, nbp, nc, bnd);
    s  = PM_mk_set(name, SC_DOUBLE_S, FALSE, n_nodes, 2, 2,
		   NULL, elem,
		   NULL, NULL, NULL, NULL, 
		   PM_MESH_TOPOLOGY_P_S, mt,
		   NULL, NULL, NULL);

    return(s);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_REP_AC_DOMAIN - build an Arbitrarily-Connected domain from a
 *                    - specified representation of the connectivity
 *                    - which is: an array of nodes bounding the zones;
 *                    - an offset; and a maximum number of nodes per zone
 */

static object *_SXI_rep_ac_domain(SS_psides *si, object *argl)
   {int incr;
    int n_zones, n_nodes, *zones;
    char *xname, *yname, *nzname, *nnname, *sname;
    double *rx, *ry;
    PM_set *s;
    g_file *po;
    PDBfile *file;
    C_array *connct;
    object *rv;

    rv     = SS_null;
    file   = NULL;
    xname  = NULL;
    yname  = NULL;
    connct = NULL;
    nzname = NULL;
    nnname = NULL;
    SS_args(si, argl,
            G_FILE, &po,
            SC_STRING_I, &xname,
            SC_STRING_I, &yname,
            SC_STRING_I, &nzname,
            SC_STRING_I, &nnname,
            SX_C_ARRAY_I, &connct,
	    LAST);

    if ((po == NULL) || (po == SX_gs.gvif))
       file = SX_gs.vif;

    else if (strcmp(po->type, PDBFILE_S) == 0)
       file = FILE_FILE(PDBfile, po);

    else
       SS_error(si, "BAD FILE - _SXI_REP_AC_DOMAIN", argl);

    if (file != NULL)
       {if (!PD_read(file, nnname, &n_nodes))
	   SS_error(si, "CAN'T READ NUMBER OF NODES - _SXI_REP_AC_DOMAIN", argl);

	if (!PD_read(file, nzname, &n_zones))
	   SS_error(si, "CAN'T READ NUMBER OF ZONES - _SXI_REP_AC_DOMAIN", argl);

	rx = CMAKE_N(double, n_nodes);
	ry = CMAKE_N(double, n_nodes);

	incr = PD_read(file, xname, rx);
	if (incr != n_nodes)
	   SS_error(si, "READING X VALUES - _SXI_REP_AC_DOMAIN", argl);

	incr = PD_read(file, yname, ry);
	if (incr != n_nodes)
	   SS_error(si, "READING Y VALUES - _SXI_REP_AC_DOMAIN", argl);

	zones = (int *) connct->data;

	sname = SC_dsnprintf(FALSE, "{%s,%s}", xname, yname);

	s = SX_rep_to_ac(sname, rx, ry, n_nodes, n_zones, zones);

	rv = SX_make_pm_set(si, s);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FIND_INDEX - find values matching the specified conditions
 *                 - Usage:  (find-value array oper val indeces)
 */

static object *_SXI_find_index(SS_psides *si, object *argl)
   {int nx, no, ni;
    int *out, *in;
    double val;
    char nt[MAXLINE];
    void *na;
    C_array *arr, *indx, *iarr;
    C_procedure *pred;
    PF_int_dd fnc;
    object *rv;

    rv   = SS_null;
    val  = 0.0;
    arr  = NULL;
    indx = NULL;
    pred = NULL;
    SS_args(si, argl,
            SX_C_ARRAY_I, &arr,
            SS_PROCEDURE_I, &pred,
            SC_DOUBLE_I, &val,
            SX_C_ARRAY_I, &indx,
            0);

    if (pred == NULL)
       SS_error(si, "NO PREDICATE SPECIFIED - _SXI_FIND_INDEX", argl);

    else if (arr == NULL)
       SS_error(si, "NO ARRAY SPECIFIED - _SXI_FIND_INDEX", argl);

    else
       {fnc = _SS_GET_C_PROCEDURE_N(PF_int_dd, pred, 0);

	if (indx == NULL)
	   {ni = 0; 
	    in = NULL;}
	else
	   PM_ARRAY_CONTENTS(indx, void, ni, nt, in);

	PM_ARRAY_CONTENTS(arr, void, nx, nt, na);

	_PM_find_value(&no, &out, nx, nt, na, fnc, val, ni, in);

	iarr = PM_make_array(SC_INT_S, no, out);

	rv = SX_make_c_array(si, iarr);};

    return(rv);}
        
/*--------------------------------------------------------------------------*/

/*                             POLYGON FUNCTIONS                            */

/*--------------------------------------------------------------------------*/

/* _SXI_MK_POLYGON - allocate and return a PM_polygon
 *                 - form: (pm-make-polygon <nd> <point>*)
 *                 - where each <point> is an <nd>-tuple
 *                 - polygons may be open or closed
 */

static object *_SXI_mk_polygon(SS_psides *si, object *argl)
   {long id, ip, nd, ne, np;
    double v;
    PM_polygon *py;
    object *rv;

    rv = SS_null;
    nd = 0L;
    SS_args(si, argl,
            SC_LONG_I, &nd,
            0);

    argl = SS_cdr(si, argl);
    ne   = SS_length(si, argl);
    np   = ne/nd;

    py = PM_init_polygon(nd, np);

    for (id = 0, ip = 0; !SS_nullobjp(argl); argl = SS_cdr(si, argl))
	{SS_args(si, argl,
		 SC_DOUBLE_I, &v,
		 0);
	 py->x[id++][ip] = v;
	 if (id == nd)
	    {id = 0;
	     ip++;};};

    py->nn = ip;

    rv = SX_make_pm_polygon(si, py);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_POLYGON_PASS - take one pass combining polygons in array A with PY
 *                  - return the resulting array
 */

static SC_array *_SX_polygon_pass(SC_array *a, PM_polygon *py,
				  SC_array *(*op)(SC_array *a,
						  PM_polygon *pa,
						  PM_polygon *pb))
   {long ip, np;
    PM_polygon *pb;
    SC_array *an;

    an = PM_polygon_array();

    np = SC_array_get_n(a);
    for (ip = 0; ip < np; ip++)
        {pb = PM_polygon_get(a, ip);
	 an = op(an, py, pb);};

    return(an);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_COMBINE_POLYGONS - worker for pm-intersect-polygon and
 *                      - pm-union-polygon
 */

static object *_SX_combine_polygons(SS_psides *si, object *argl,
				    SC_array *(*op)(SC_array *a,
						    PM_polygon *pa,
						    PM_polygon *pb))
   {long ip, np, n;
    PM_polygon *pa, *pb;
    SC_array *a, *an;
    object *rv, *o;

    rv = SS_null;

    n = SS_length(si, argl);
    if (n == 1)
       rv = SS_car(si, argl);

    else if (n > 1)
       {o  = SS_car(si, argl);
	pa = SS_GET(PM_polygon, o);

        a = PM_polygon_array();
	PM_polygon_push(a, pa);

	for (argl = SS_cdr(si, argl); !SS_nullobjp(argl); argl = SS_cdr(si, argl))
	    {o  = SS_car(si, argl);
	     pb = SS_GET(PM_polygon, o);

/* combine PB with all polygons in A */
	     an = _SX_polygon_pass(a, pb, op);

/* free the old array A */
	     PM_free_polygons(a, TRUE);

/* use the new array AN in the next pass */
	     a = an;};

/* turn the list into an array */
	np = SC_array_get_n(a);
	for (ip = 0; ip < np; ip++)
	    {pa = PM_polygon_get(a, ip);
	     o  = SX_make_pm_polygon(si, pa);
	     rv = SS_mk_cons(si, o, rv);};

	PM_free_polygons(a, FALSE);

	rv = SS_reverse(si, rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_INTERSECT_POLYGON - return the intersection of one or more polygons
 *                        - form: (pm-intersect-polygon <py>*)
 */

static object *_SXI_intersect_polygon(SS_psides *si, object *argl)
   {object *rv;

    rv = _SX_combine_polygons(si, argl, PM_intersect_polygons);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_UNION_POLYGON - return the unionion of one or more polygons
 *                    - form: (pm-union-polygon <py>*)
 */

static object *_SXI_union_polygon(SS_psides *si, object *argl)
   {object *rv;

    rv = _SX_combine_polygons(si, argl, PM_union_polygons);

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                            INSTALL PML FUNCTIONS                         */

/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_PML_FUNCS - install the PML extensions to Scheme */
 
void SX_install_pml_funcs(SS_psides *si)
   {

    SS_install(si, "find-index",
               "Returns the array of indeces for which a predicate it true",
               SS_nargs,
               _SXI_find_index, SS_PR_PROC);

    SS_install(si, "list->pm-array",
               "Returns a numeric array built from a list of numbers",
               SS_nargs,
               _SXI_list_array, SS_PR_PROC);

    SS_install(si, "pm-array->list",
               "Returns a list of numbers built from a numeric array",
               SS_sargs,
               _SXI_array_list, SS_PR_PROC);

    SS_install(si, "pm-make-array",
               "Allocate and return a pm-array of the specified type and size",
               SS_nargs,
               _SXI_mk_array, SS_PR_PROC);

    SS_install(si, "pm-resize-array",
               "reallocate the given pm-array to the specified size",
               SS_nargs,
               _SXI_resz_array, SS_PR_PROC);

    SS_install(si, "pm-array-ref",
               "Reference the nth element of a pm-array",
               SS_nargs,
               _SXI_array_ref, SS_PR_PROC);

    SS_install(si, "pm-array-set!",
               "Set the nth element of a pm-array",
               SS_nargs,
               _SXI_array_set, SS_PR_PROC);

    SS_install(si, "pm-array-length",
               "Returns the length of the given numeric array",
               SS_sargs,
               _SXI_num_arr_len, SS_PR_PROC);

    SS_install(si, "pm-array-extrema",
               "Returns the extrema of the given numeric array",
               SS_sargs,
               _SXI_num_arr_extr, SS_PR_PROC);

    SS_install(si, "pm-sub-array",
               "Return a sub-array of the given numeric array",
               SS_nargs,
               _SXI_sub_array, SS_PR_PROC);

    SS_install(si, "pm-set-mapping-type",
               "Set the type of a mapping object to the given string",
               SS_nargs,
               _SXI_set_map_type, SS_PR_PROC);

    SS_install(si, "pm-grotrian-mapping?",
               "Returns #t if the object is a PML grotrian mapping, and #f otherwise",
               SS_sargs,
               _SXI_grotrian_mappingp, SS_PR_PROC);

    SS_install(si, "pm-mapping->pdbdata",
               "Write a PML mapping object to a PDB file\nFORM (pm-mapping->pdbdata <mapping> <file> <name>)",
               SS_nargs,
               _SXI_mapping_pdbdata, SS_PR_PROC);

    SS_install(si, "pdbdata->pm-mapping",
               "Read a PML mapping object from a PDB file\nFORM (pdbdata->pm-mapping <file> <name>)",
               SS_nargs,
               _SXI_pdbdata_mapping, SS_PR_PROC);

    SS_install(si, "pm-set-name",
               "Return the name of the object iff it is a PM_set and () otherwise",
               SS_sargs,
               _SXI_get_text_set_name, SS_PR_PROC);

    SS_install(si, "pm-set->pdbdata",
               "Write a PML set object to a PDB file\nFORM (pm-set->pdbdata <set> <file> <name>)",
               SS_nargs,
               _SXI_set_pdbdata, SS_PR_PROC);

    SS_install(si, "pdbdata->pm-set",
               "Read a PML set object from a PDB file\nFORM (pdbdata->pm-set <file> <name>)",
               SS_nargs,
               _SXI_pdbdata_set, SS_PR_PROC);

    SS_install(si, "pdbdata->pm-array",
               "Convert a PDBDATA object to a numeric array object\nFORM (pdbdata->pm-array <pdbdata>)",
               SS_nargs,
               _SXI_pdbdata_array, SS_PR_PROC);

    SS_install(si, "pm-array->pdbdata",
               "Convert a numeric array object to a PDBDATA object\nFORM (pm-array->pdbdata <array>)",
               SS_nargs,
               _SXI_array_pdbdata, SS_PR_PROC);

    SS_install(si, "pm-array->pdbdata-i",
               "Inverse of pdbdata->pm-array\nFORM (pm-array->pdbdata-i <array> <file> [<name>])",
               SS_nargs,
               _SXI_array_pdbdata_i, SS_PR_PROC);

    SS_install(si, "pm-lr->ac",
               "Convert a logical rectangular mesh based mapping into an arbitrarily connected mesh base one\nFORM (pm-lr->ac <mapping>)",
               SS_nargs,
               _SXI_lr_ac, SS_PR_PROC);

    SS_install(si, "pm-make-ac-set",
               "Construct an arbitrarily connected set",
               SS_nargs,
               _SXI_make_ac_set, SS_PR_PROC);

    SS_install(si, "pm-arrays->set",
               "Convert a list of numeric array objects to a set\nFORM (pm-arrays->set (<array> ...) ...)",
               SS_nargs,
               _SXI_arrays_set, SS_PR_PROC);

    SS_install(si, "pm-make-set",
               "Returns a PML set\nFORM (pm-make-set <name> <mesh-shape-list> <element-arrays>)",
               SS_nargs,
               _SXI_make_pml_set, SS_PR_PROC);

    SS_install(si, "pm-make-mapping",
               "Returns a PML mapping\nFORM (pm-make-mapping <domain> <range> [<centering> <category> <name> <emap> <next>])",
               SS_nargs,
               _SXI_make_pml_mapping, SS_PR_PROC);

    SS_install(si, "pm-make-cartesian-product-set",
               "Returns a newly constructed set\nFORM (pm-make-cartesian-product-set [<set>]*)",
               SS_nargs,
               _SXI_make_cp_set, SS_PR_PROC);

    SS_install(si, "pm-set-set-attribute!",
               "Set an attribute of the given set",
               SS_nargs,
               _SXI_set_attr_set, SS_PR_PROC);

    SS_install(si, "pm-connection->ac-domain",
               "Build an Arbitrarily-Connected domain set from the given connectivity representation",
	       SS_nargs,
	       _SXI_rep_ac_domain, SS_PR_PROC);

/* polygon routines */
    SS_install(si, "pm-make-polygon",
               "Allocate and return a pm-polygon of the specified type and size",
               SS_nargs,
               _SXI_mk_polygon, SS_PR_PROC);

    SS_install(si, "pm-intersect-polygon",
               "Return the intersection of the given pm-polygons",
               SS_nargs,
               _SXI_intersect_polygon, SS_PR_PROC);

    SS_install(si, "pm-union-polygon",
               "Return the union of the given pm-polygons",
               SS_nargs,
               _SXI_union_polygon, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
