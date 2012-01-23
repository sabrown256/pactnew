/*
 * MLMM.C - memory management routines for PML
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "pml_int.h"

char
 *PM_AC_S              = "Arbitrarily-Connected",
 *PM_LR_S              = "Logical-Rectangular",
 *PM_MAP_INFO_P_S      = "PM_map_info *",
 *PM_MAPPING_P_S       = "PM_mapping *",
 *PM_SET_P_S           = "PM_set *",
 *PM_MAP_INFO_S        = "PM_map_info",
 *PM_MAPPING_S         = "PM_mapping",
 *PM_MESH_TOPOLOGY_S   = "PM_mesh_topology",
 *PM_MESH_TOPOLOGY_P_S = "PM_mesh_topology *",
 *PM_SET_S             = "PM_set";

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_VECTORS - allocate a set of arrays */

double **PM_make_vectors(int nd, int n)
   {int id;
    double **x;

    x = CMAKE_N(double *, nd);
    for (id = 0; id < nd; id++)
	{x[id] = CMAKE_N(double, n);
	 SC_mark(x[id], 1);};

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INIT_VECTORS - initialize the vector values */

void PM_init_vectors(int nd, int n, double **x, double v)
   {int id;

    for (id = 0; id < nd; id++)
	PM_array_set(x[id], n, v);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COPY_VECTORS - copy a set of  arrays */

double **PM_copy_vectors(int nd, int n, double **x)
   {int id;
    double **y;

    y = PM_make_vectors(nd, n);
    for (id = 0; id < nd; id++)
        PM_array_copy(y[id], x[id], n);

    return(y);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FREE_VECTORS - release a set of coordinate arrays */

void PM_free_vectors(int nd, double **x)
   {int id;

/* NOTE: don't use the macro or it will foul up the vectors
 * the reference counting is on this so setting x[id] == NULL is
 * a bad idea in general
 */
    for (id = 0; id < nd; id++)
        _SC_FREE_N(x[id], NULL);

    CFREE(x);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_ARRAY - make a C_array of TYPE and SIZE */

C_array *PM_make_array(char *type, long size, void *data)
   {long bpi;
    char ltype[MAXLINE], ttype[MAXLINE];
    char *pt;
    C_array *arr;

    arr = CMAKE(C_array);

    strcpy(ttype, type);
    pt = SC_firsttok(ttype, " *");

    bpi = SIZEOF(pt);
    snprintf(ltype, MAXLINE, "%s *", pt);

    arr->type = CSTRSAVE(ltype);
    SC_mark(arr->type, 1);

    arr->length = size;

    if (data == NULL)
       {arr->data = (void *) CMAKE_N(char, size*bpi);
	data = arr->data;}
    else
       arr->data = data;

    SC_mark(data, 1);

    return(arr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_REL_ARRAY - release a C_array */

void PM_rel_array(C_array *arr)
   {

    CFREE(arr->type);
    CFREE(arr->data);
    CFREE(arr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_INIT_POLYGON - initialize and return a clean PM_polygon with all space
 *                 - ready to be used
 */

PM_polygon *PM_init_polygon(int nd, int n)
   {int id;
    double **x;
    PM_polygon *py;

    x = CMAKE_N(double *, nd);

    for (id = 0; id < nd; id++)
	{x[id] = CMAKE_N(double, n);
	 SC_mark(x[id], 1);};

    py = CMAKE(PM_polygon);

    py->nd = nd;
    py->np = n;
    py->nn = 0;
    py->x  = x;

    return(py);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_POLYGON - initialize and return a PM_polygon
 *                 - which encapsulates existing arrays
 */

PM_polygon *PM_make_polygon(int nd, int n, ...)
   {int id;
    double **x;
    PM_polygon *py;

    x = CMAKE_N(double *, nd);

    SC_VA_START(n);

    for (id = 0; id < nd; id++)
	{x[id] = SC_VA_ARG(double *);
	 SC_mark(x[id], 1);};

    SC_VA_END;

    py = CMAKE(PM_polygon);

    py->nd = nd;
    py->np = n;
    py->nn = n;
    py->x  = x;

    return(py);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_POLYGON_BOX - initialize and return a PM_polygon
 *                - from a box
 */

PM_polygon *PM_polygon_box(int nd, double *bx)
   {int i, n;
    double a;
    PM_polygon *py;

    py = PM_init_polygon(nd, 10);

    n = 0;

    if (nd == 1)
       {n = 3;

	py->x[0][0] = bx[0];
	py->x[0][1] = bx[1];
	py->x[0][2] = bx[0];}

    else if (nd == 2)
       {n = 5;

	py->x[0][0] = bx[0];
	py->x[0][1] = bx[1];
	py->x[0][2] = bx[1];
	py->x[0][3] = bx[0];
	py->x[0][4] = bx[0];

	py->x[1][0] = bx[2];
	py->x[1][1] = bx[2];
	py->x[1][2] = bx[3];
	py->x[1][3] = bx[3];
	py->x[1][4] = bx[2];}

/* project 3D polygon to mid plane - pending further requirements */
    else if (nd == 3) 
       {n = 5;

	py->x[0][0] = bx[0];
	py->x[0][1] = bx[1];
	py->x[0][2] = bx[1];
	py->x[0][3] = bx[0];
	py->x[0][4] = bx[0];

	py->x[1][0] = bx[2];
	py->x[1][1] = bx[2];
	py->x[1][2] = bx[3];
	py->x[1][3] = bx[3];
	py->x[1][4] = bx[2];

	a = 0.5*(bx[4] + bx[5]);
	for (i = 0; i < n; i++)
	    py->x[2][i] = a;};

    py->nn = n;

    return(py);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COPY_POLYGON - copy and return a PM_polygon PY
 *                 - which encapsulates existing arrays
 */

PM_polygon *PM_copy_polygon(PM_polygon *py)
   {int n, nd;
    double **x;
    PM_polygon *ly;

    nd = py->nd;
    n  = py->nn;
    x  = PM_copy_vectors(nd, n, py->x);

    ly = CMAKE(PM_polygon);
    *ly   = *py;
    ly->x = x;

    return(ly);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_POLYGON_COPY_POINTS - copy the points of PB into PA */

void PM_polygon_copy_points(PM_polygon *pa, PM_polygon *pb)
   {int id, n, nd;
    double **x, **y;

    nd = pb->nd;
    n  = pb->nn;
    x  = pb->x;
    y  = pa->x;
    for (id = 0; id < nd; id++)
        PM_array_copy(y[id], x[id], n);

    pa->nd = nd;
    pa->nn = n;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FREE_POLYGON - release a set of coordinate arrays */

void PM_free_polygon(PM_polygon *py)
   {int nd;

    if (py != NULL)
       {if (SC_ref_count(py) < 2)
	   {nd = py->nd;

	    PM_free_vectors(nd, py->x);};

	CFREE(py);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COPY_SET - make an independent copy of the set S */

PM_set *PM_copy_set(PM_set *s)
   {int i, nd, nde, *maxes;
    int *mx;
    char *tt;
    void **elem, **selem;
    PM_mesh_topology *mt;
    PM_set *ns;

    nd    = s->dimension;
    nde   = s->dimension_elem;
    mx    = s->max_index;
    tt    = s->topology_type;
    mt    = s->topology;
    selem = s->elements;

    if (mx != NULL)
       {maxes = CMAKE_N(int, nd);
	for (i = 0; i < nd; i++)
	    maxes[i] = mx[i];}
    else
       {maxes = NULL;
	mt    = PM_copy_topology(mt);};

    elem = CMAKE_N(void *, nde);
    for (i = 0; i < nde; i++)
        elem[i] = selem[i];

    ns = PM_mk_set(s->name, s->element_type, TRUE,
		   s->n_elements, nd, nde,
		   maxes, elem, s->opers, s->metric,
		   s->symmetry_type, s->symmetry,
		   tt, mt,
		   s->info_type, s->info,
		   NULL);

    return(ns);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_SET_ALT - given the name, shape, and elements of a set
 *                 - build the set structure and fill it
 *
 * #bind PM_make_set_alt fortran() scheme() python()
 */

PM_set *PM_make_set_alt(char *name, char *type, int cp, int nd,
			int *diml, int nde, void **elml)
   {int i, d, ne, *maxes;
    void **elem;
    PM_set *set;

    maxes = CMAKE_N(int, nd);
    ne    = 1;
    for (i = 0; i < nd; i++)
        {d = diml[i];
	 maxes[i] = d;
         ne *= d;};

    elem = CMAKE_N(void *, nde);
    for (i = 0; i < nde; i++)
        elem[i] = elml[i];

    set = PM_mk_set(name, type, cp,
		    ne, nd, nde, maxes, elem, PM_gs.fp_opers,
		    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_SET - given the name, shape, and elements of a set
 *             - build the set structure and fill it
 */

PM_set *PM_make_set(char *name, char *type, int cp, int nd, ...)
   {int i, d, ne, nde, *maxes;
    void **elem;
    PM_set *set;

    SC_VA_START(nd);

    maxes = CMAKE_N(int, nd);
    ne    = 1;
    for (i = 0; i < nd; i++)
        {d = SC_VA_ARG(int);
         maxes[i] = d;
         ne *= d;};

    nde  = SC_VA_ARG(int);
    elem = CMAKE_N(void *, nde);
    for (i = 0; i < nde; i++)
        elem[i] = SC_VA_ARG(void *);

    SC_VA_END;

    set = PM_mk_set(name, type, cp,
		    ne, nd, nde, maxes, elem, PM_gs.fp_opers,
		    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_AC_SET - given the name, connectivity, and elements of a set
 *                - build the set structure and fill it
 */

PM_set *PM_make_ac_set(char *name, char *type, int cp,
		       PM_mesh_topology *mt, int nde, ...)
   {int i, nd, ne;
    void **elem;
    PM_set *set;

    nd = mt->n_dimensions;
    ne = mt->n_cells[0];

    SC_VA_START(nde);

    elem = CMAKE_N(void *, nde);
    for (i = 0; i < nde; i++)
        elem[i] = SC_VA_ARG(void *);

    SC_VA_END;

    set = PM_mk_set(name, type, cp,
		    ne, nd, nde,
		    NULL, elem,
		    NULL, NULL, NULL, NULL,
		    PM_MESH_TOPOLOGY_P_S, mt,
		    NULL, NULL, NULL);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MK_SET - allocate and initialize a set in the most primitive
 *           - terms
 *           - return the set
 */

PM_set *PM_mk_set(char *name, char *type, int cp, long ne,
		  int nd, int nde, int *maxes, void *elem,
		  PM_field *opers, double *metric,
		  char *symtype, void *sym,
		  char *toptype, void *top,
		  char *inftype, void *inf,
		  PM_set *next)
   {int i;
    void *ov, *nv, **el;
    char bf[MAXLINE];
    long bpi;
    pcons *info;
    PM_set *set;

    info = (pcons *) inf;
    if ((inftype != NULL) && (info != NULL))
       info = SC_copy_alist(info);

    if (inftype == NULL)
       inftype = SC_PCONS_P_S;

    SC_CHANGE_VALUE_ALIST(info, int, SC_INT_P_S, "COPY-MEMORY", cp);

    inf = (void *) info;

/* build the set */
    set                 = CMAKE(PM_set);
    set->name           = CSTRSAVE(name);
    set->n_elements     = ne;
    set->dimension      = nd;
    set->dimension_elem = nde;
    set->max_index      = maxes;
    set->elements       = (void *) elem;
    set->opers          = PM_gs.fp_opers;
    set->metric         = metric;
    set->symmetry_type  = CSTRSAVE(symtype);
    set->symmetry       = sym;
    set->topology_type  = CSTRSAVE(toptype);
    set->topology       = top;
    set->info_type      = CSTRSAVE(inftype);
    set->info           = inf;
    set->next           = next;

    strcpy(bf, type);
    strtok(bf, " *");

/* get the byte size of type before it is changes with indirections */
    bpi = SIZEOF(bf);

    set->extrema = (void *) CMAKE_N(char, 2*nde*bpi);
    set->scales  = (void *) CMAKE_N(char, nde*bpi);

    SC_strcat(bf, MAXLINE, " *");
    set->es_type = CSTRSAVE(bf);

    SC_strcat(bf, MAXLINE, "*");
    set->element_type = CSTRSAVE(bf);

/* if requested copy the incoming data */
    if (cp == TRUE)
       {el = (void **) elem;
	for (i = 0; i < nde; i++)
            {ov = el[i];
	     nv = CMAKE_N(char, ne*bpi);
	     memcpy(nv, ov, ne*bpi);
	     el[i] = nv;};};

/* mark the arrays for proper reference count */
    el = (void **) elem;
    for (i = 0; i < nde; i++)
        {nv = el[i];
	 SC_mark(nv, 1);};

    SC_mark(set->name, 1);
    SC_mark(set->max_index, 1);
    SC_mark(set->elements, 1);
    SC_mark(set->extrema, 1);
    SC_mark(set->scales, 1);
    SC_mark(set->element_type, 1);
    SC_mark(set->es_type, 1);
    SC_mark(set->symmetry_type, 1);
    SC_mark(set->topology_type, 1);
    SC_mark(set->info_type, 1);

    PM_find_extrema(set);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_REL_SET - release the storage associated with a set
 *
 * #bind PM_rel_set fortran() scheme() python()
 */

void PM_rel_set(PM_set *set, int mfl)
   {pcons *inf;
    PM_mesh_topology *mt;

    if (set == NULL)
       return;

    if (SC_safe_to_free(set))
       {CFREE(set->name);
	CFREE(set->element_type);
	CFREE(set->es_type);
	CFREE(set->max_index);

	if (set->topology_type != NULL)
	   {if (strcmp(set->topology_type, PM_MESH_TOPOLOGY_P_S) == 0)
	       {mt = (PM_mesh_topology *) set->topology;
		if (mt != NULL)
		   PM_rel_topology(mt);};

	    CFREE(set->topology_type);};

	if (set->info_type != NULL)
	   {if (strcmp(set->info_type, SC_PCONS_P_S) == 0)
	       {inf = (pcons *) set->info;
		if (inf != NULL)
		   SC_free_alist(inf, 3);};
	    CFREE(set->info_type);};

	if (mfl)
	   {int i, nde;
	    void **elem;

	    nde  = set->dimension_elem;
	    elem = (void **) set->elements;
	    for (i = 0; i < nde; i++)
	        {CFREE(elem[i]);};};

	CFREE(set->elements);

	CFREE(set->extrema);
	CFREE(set->scales);};

    CFREE(set);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_TOPOLOGY - make a PM_mesh_topology */

PM_mesh_topology *PM_make_topology(int nd, int *bnp, int *bnc, long **bnd)
   {PM_mesh_topology *mt;

    mt = CMAKE(PM_mesh_topology);

    mt->n_dimensions   = nd;
    mt->n_bound_params = bnp;
    mt->n_cells        = bnc;
    mt->boundaries     = bnd;

    SC_mark(bnp, 1);
    SC_mark(bnc, 1);
    SC_mark(bnd, 1);

    return(mt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_COPY_TOPOLOGY - make an independent copy of the mesh topology mt */

PM_mesh_topology *PM_copy_topology(PM_mesh_topology *mt)
   {int i, j, nd, ndp, nc, np;
    int *bnp, *bnc, *nbnp, *nbnc;
    long **bnd, **nbnd, *pb, *pbp;
    PM_mesh_topology *nmt;

    nd  = mt->n_dimensions;
    bnp = mt->n_bound_params;
    bnc = mt->n_cells;
    bnd = mt->boundaries;

    ndp = nd + 1;

    nbnp = CMAKE_N(int, ndp);
    nbnc = CMAKE_N(int, ndp);
    for (i = 0; i < ndp; i++)
        {nbnp[i] = bnp[i];
	 nbnc[i] = bnc[i];};

/* copy the boundary arrays */
    nbnd = CMAKE_N(long *, ndp);
    for (i = 0; i < ndp; i++)
        {pb = bnd[i];
	 if (pb != NULL)
	    {np = bnp[i];
	     nc = bnc[i];
	     nc = (np == 1) ? nc + 1 : nc*np;
	     pbp = CMAKE_N(long, nc);
	     for (j = 0; j < nc; j++)
	         pbp[j] = pb[j];}

	 else
	    pbp = NULL;

	 nbnd[i] = pbp;};

    nmt = PM_make_topology(nd, nbnp, nbnc, nbnd);

    return(nmt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_REL_TOPOLOGY - release the storage associated with a PM_mesh_topology */

void PM_rel_topology(PM_mesh_topology *mt)
   {int id, nd;
    long **bnd;

    if (mt == NULL)
       return;

    if (SC_safe_to_free(mt))
       {bnd = mt->boundaries;

	nd = mt->n_dimensions;
	for (id = 0; id <= nd; id++)
	    {CFREE(bnd[id]);};

	CFREE(mt->boundaries);
	CFREE(mt->n_bound_params);
	CFREE(mt->n_cells);};

    CFREE(mt);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_ARRAY_REAL - make a copy of an arbitrary type array that is all
 *               - type double
 */

double *PM_array_real(char *type, void *p, int n, double *x)
   {int sid;
    char bf[MAXLINE];

    SC_strncpy(bf, MAXLINE, type, -1);
    sid = SC_type_id(strtok(bf, " *"), FALSE);

    x = SC_convert_id(SC_DOUBLE_I, x, 0, 1, sid, p, 0, 1, n, FALSE);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_REAL_SET_ELEMENTS - return an array of double values arrays
 *                           - representing the set elements
 */

double **PM_make_real_set_elements(PM_set *s)
   {int i, n, m;
    double **r;
    char *t;
    void **e;

    n = s->n_elements;
    m = s->dimension_elem;
    e = s->elements;
    t = s->es_type;

    r = CMAKE_N(double *, m);
    for (i = 0; i < m; i++)
        {r[i] = CMAKE_N(double, n);
	 PM_array_real(t, e[i], n, r[i]);};

    return(r);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_REL_REAL_SET_ELEMENTS - return an array of double values arrays
 *                          - representing the set elements
 */

void PM_rel_real_set_elements(double **r)
   {int i, n;

    n = SC_MEM_GET_N(double *, r);

    for (i = 0; i < n; i++)
        CFREE(r[i]);

    CFREE(r);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FIND_EXIST_EXTREMA - record the extrema of each of the element array
 *                       - of the given set
 #                       - using only those elements for which EMAP is non-zero
 *                       -
 *                       -  a[0]    = var_0_min
 *                       -  a[1]    = var_0_max
 *                       -          .
 *                       -          .
 *                       -          .
 *                       -  a[2n]   = var_n_min
 *                       -  a[2n+1] = var_n_max
 *                       -
 *                       - also find the mesh scales
 */

void PM_find_exist_extrema(PM_set *s, char *typ, void *em)
   {int i, nd, nde, ne, sid;
    int *maxes;
    void **elem;
    double dx, ds, xmn, xmx;
    double **x, *extr, *pe, *scales;
    char *emap;

    ne = s->n_elements;
    if (ne == 0)
       return;

    nde = s->dimension_elem;
    nd  = s->dimension;
       
    sid  = SC_type_id(typ, FALSE);
    emap = SC_convert_id(SC_CHAR_I, NULL, 0, 1, sid, em, 0, 1, ne, FALSE);

    extr   = CMAKE_N(double, 2*nde);
    scales = CMAKE_N(double, nde);

    sid = SC_deref_id(s->element_type, TRUE);

/* compute the extrema in each dimension */
    elem = (void **) s->elements;
    x    = PM_make_vectors(nde, ne);
    for (i = 0; i < nde; i++)
        x[i] = SC_convert_id(SC_DOUBLE_I, x[i], 0, 1,
			     sid, elem[i], 0, 1, ne, FALSE);

    PM_vector_select_extrema(nde, ne, x, emap, extr);

    PM_free_vectors(nde, x);

    CFREE(emap);

/* compute the scales of the components */
    pe    = extr;
    maxes = s->max_index;
    if (maxes == NULL)
       {double sc;

	sc = 1.5/POW((double) ne, (1.0/((double) nde)));
	for (i = 0; i < nde; i++)
	    {xmn = *pe++;
	     xmx = *pe++;
	     dx  = xmx - xmn;
	     scales[i] = sc*dx;};}
    else
       {for (i = 0; i < nde; i++)
	    {xmn = *pe++;
	     xmx = *pe++;
	     dx  = xmx - xmn;
	     ds  = (nd == 1) ? maxes[0] - 1.0 : maxes[i] - 1.0;
	     ds  = max(ds, 1.0);
	     scales[i] = dx/ds;};};

    s->extrema = SC_convert_id(sid, s->extrema, 0, 1,
			       SC_DOUBLE_I, extr, 0, 1, 2*nde, TRUE);
    s->scales  = SC_convert_id(sid, s->scales, 0, 1,
			       SC_DOUBLE_I, scales, 0, 1, nde, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_FIND_EXTREMA - record the extrema of each of the element array
 *                 - of the given set
 *                 -
 *                 -  a[0]    = var_0_min
 *                 -  a[1]    = var_0_max
 *                 -          .
 *                 -          .
 *                 -          .
 *                 -  a[2n]   = var_n_min
 *                 -  a[2n+1] = var_n_max
 *                 -
 *                 - also find the mesh scales
 */

void PM_find_extrema(PM_set *s)
   {int ne;
    char *emap;

    ne = s->n_elements;
    if (ne == 0)
       return;

    emap = CMAKE_N(char, ne);
    memset(emap, 1, ne);

    PM_find_exist_extrema(s, SC_CHAR_S, emap);

    CFREE(emap);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SET_EXTREMA - find the extrema of a list of sets
 *                - return newly allocated array
 */

double *PM_set_extrema(PM_set *s)
   {int i, nd, nl;
    double mn, mx;
    double *extr, *tex, *pex;
    PM_set *p;

    nd = s->dimension_elem;
    nl = 2*nd;

    tex  = CMAKE_N(double, nl);
    extr = CMAKE_N(double, nl);

    if (extr != NULL)
       {for (i = 0; i < nl; i += 2)
	    {extr[i]   = HUGE;
	     extr[i+1] = -HUGE;};

	for (p = s; p != NULL; p = p->next)
	    {pex = PM_array_real(p->element_type, p->extrema, nl, tex);
	     for (i = 0; i < nl; i += 2)
	         {mn = extr[i];
		  mx = extr[i+1];
		  mn = min(mn, pex[i]);
		  mx = max(mx, pex[i+1]);
		  extr[i]   = mn;
		  extr[i+1] = mx;};};};

    CFREE(tex);

    return(extr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_FILL_CP_ARRAY - compute the values to fill the Cartesian product
 *                   - arrays of X recursively
 *                   - NOTE: this must be type double!!!!!
 */

static void _PM_fill_cp_array(int n, long ne, int offs, double **x,
			      int *maxes, double **x1)
   {long i, is, nx, ns;
    double *xc, *xv, val;

    if (n < 0)
       return;

    nx = maxes[n];
    xv = x1[n];
    ns = ne/nx;
    xc = x[n] + offs;

    for (i = 0L; i < nx; i++)
        {val = xv[i];
         for (is = 0L; is < ns; is++)
             *xc++ = val;
         _PM_fill_cp_array(n-1, ns, (int) (i*ns + offs), x, maxes, x1);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_GENERATE_LR_CP - compute the values of the LR coordinate arrays
 *                   - as the Cartesian product of the arrays X1 with
 *                   - lengths in MAXES
 *                   - the caller is responsible for freeing X1
 */

double **PM_generate_lr_cp(int nd, int *maxes, double **x1)
   {long i, ne;
    double **x;

/* compute the number of points */
    ne = 1L; 
    for (i = 0; i < nd; i++)
        ne *= maxes[i];

/* compute the components as doubles */
    x = PM_make_vectors(nd, ne);

    _PM_fill_cp_array(nd-1, ne, 0, x, maxes, x1);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_LR_CP_DOMAIN - build a set suitable for
 *                      - use as the domain of a Logical-Rectangular mapping
 *                      - by constructing the Cartesian product of 1D
 *                      - sets
 */

PM_set *PM_make_lr_cp_domain(char *name, char *type, int nd, PM_set **sets)
   {int id, mx, *maxes;
    long ne;
    double **x, **x1;
    void **el;
    PM_set *set, *s;

    maxes = CMAKE_N(int, nd);
    x1    = CMAKE_N(double *, nd);

/* compute the number of points */
    ne = 1L; 
    for (id = 0; id < nd; id++)
        {s  = sets[id];
         mx = s->max_index[0];
	 el = (void **) s->elements;

	 ne       *= mx;
         maxes[id] = mx;
	 x1[id]    = PM_array_real(s->element_type, el[0], mx, NULL);};

/* compute the components as doubles */
    x = PM_make_vectors(nd, ne);

    _PM_fill_cp_array(nd-1, ne, 0, x, maxes, x1);

    PM_free_vectors(nd, x1);

/* build the set
 * NOTE: since we just allocated the space in x above
 * we do NOT need to copy it in the set
 * to copy it would be to cause a memory leak
 */
    set = PM_mk_set(name, SC_DOUBLE_S, FALSE,
		    ne, nd, nd, maxes, x, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    for (id = 0; id < nd; id++)
        SC_mark(x[id], -1);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_GEN_INDEX - recursively compute the LR index arrays
 *               - from MAXES, EXTR, and RATIO
 *               - NOTE: this must be type double!!!!!
 */

static void _PM_gen_index(int n, int ne, int offs,
			  double **x, int *maxes,
			  double *extr, double *ratio)
   {int i, is, nx, ns;
    double a, d, b, r, f;
    double nxd, val;
    double *xc;

    if (n < 0)
       return;

    nx  = maxes[n];
    nxd = nx;
    ns  = ne/nx;

    if (extr == NULL)
       {a = 1.0;
	b = nx;}
    else
       {a = extr[2*n];
	b = extr[2*n + 1];};

    xc = x[n] + offs;

    d = (b - a)/(nxd - 1.0);
    r = 1.0;
    if (ratio != NULL)
       {r = ratio[n];
	if ((r != 0.0) && (r != 1.0))
	   {f = (b - a)/(1.0 - POW(r, nxd - 1.0));
	    d = f*(1.0/r - 1.0);};};

    val = a - d;
    for (i = 0; i < nx; i++)
        {val += d;
         for (is = 0; is < ns; is++)
             *xc++ = val;
	 d *= r;

         _PM_gen_index(n-1, ns, i*ns + offs,
		       x, maxes, extr, ratio);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_GENERATE_LR_INDEX - compute the values of the LR coordinate
 *                      - arrays from integer dimensions MAXES and
 *                      - physical EXTREMA
 */

double **PM_generate_lr_index(int nd, int *maxes,
			      double *extrema, double *ratio)
   {int id;
    long ne;
    double **x;

/* compute the number of points */
    ne = 1L; 
    for (id = 0; id < nd; id++)
        ne *= maxes[id];

/* compute the components as doubles */
    x = PM_make_vectors(nd, ne);

    _PM_gen_index(nd-1, ne, 0, x, maxes, extrema, ratio);

    return(x);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_LR_INDEX_DOMAIN - build a set suitable for
 *                         - use as the domain of a Logical-Rectangular mapping
 *                         - the elements are the logical indeces of the points
 */

PM_set *PM_make_lr_index_domain(char *name, char *type, int nd, int nde,
				int *maxes, double *extrema, double *ratio)
   {int i, did;
    long ne;
    double **x;
    void **elem;
    PM_set *set;

/* compute the number of points */
    ne = 1L; 
    for (i = 0; i < nd; i++)
        ne *= maxes[i];

/* compute the components as doubles */
    x = PM_make_vectors(nde, ne);

    _PM_gen_index(nd-1, ne, 0, x, maxes, extrema, ratio);

/* convert the components to the desired type */
    did  = SC_type_id(type, FALSE);
    elem = CMAKE_N(void *, nde);
    for (i = 0; i < nde; i++)
        elem[i] = SC_convert_id(did, NULL, 0, 1,
				SC_DOUBLE_I, x[i], 0, 1, ne, FALSE);

    PM_free_vectors(nde, x);

/* build the set */
    set = PM_mk_set(name, type, FALSE, ne, nd, nde, maxes, elem, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_RESOLVE_TYPE - resolve the meaning of NTYP and return the 
 *                 - result in LTYP
 *                 - return TRUE iff there will be something to do
 */

int PM_resolve_type(char *ltyp, char *ntyp, char *btyp)
   {int ret;
    char type[MAXLINE], *t;

    ret = TRUE;
    if (strcmp(ntyp, "none") == 0)
       ret = FALSE;
       
    else if (strcmp(ntyp, "nearest") == 0)
       {strcpy(type, btyp);
	if (strchr(type, '*') == NULL)
	   t = type;
	else
	   t = SC_firsttok(type, " *\t\f\n\r");

	CONTAINER(ltyp, t);}

    else
       strcpy(ltyp, ntyp);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_PROMOTE_SET - promote the set data to be of type NTYP
 *                - free the original data if FLAG is TRUE
 */

void PM_promote_set(PM_set *s, char *ntyp, int flag)
   {int id, nde, did, sid;
    long ne;
    char otyp[MAXLINE], ltyp[MAXLINE], nelt[MAXLINE], nest[MAXLINE];
    char *elt, *est, *ot;
    void **elem;

    if (!PM_resolve_type(ltyp, ntyp, s->element_type))
       return;

    did = SC_type_id(ltyp, FALSE);

    elt = s->element_type;
    est = s->es_type;
    strcpy(otyp, est);
    if (strchr(otyp, '*') == NULL)
       ot = otyp;
    else
       ot = SC_firsttok(otyp, " *\t\f\n\r");
    sid = SC_type_id(ot, FALSE);

    snprintf(nelt, MAXLINE, "%s **", ltyp);
    snprintf(nest, MAXLINE, "%s *", ltyp);

/* change the type names */
    CFREE(elt);
    CFREE(est);

    s->element_type = CSTRSAVE(nelt);
    s->es_type      = CSTRSAVE(nest);

/* change the element data */
    ne   = s->n_elements;
    nde  = s->dimension_elem;
    elem = (void **) s->elements;
    for (id = 0; id < nde; id++)
        elem[id] = SC_convert_id(did, NULL, 0, 1,
				 sid, elem[id], 0, 1, ne, flag);

/* change the extrema/scale data */
    s->extrema = SC_convert_id(did, NULL, 0, 1,
			       sid, s->extrema, 0, 1, 2*nde, TRUE);
    s->scales  = SC_convert_id(did, NULL, 0, 1,
			       sid, s->scales, 0, 1, nde, TRUE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_PROMOTE_ARRAY - promote the array data to be of type NTYP
 *                  - free the original data if FLAG is TRUE
 */

void PM_promote_array(C_array *a, char *ntyp, int flag)
   {int sid, did;
    long ne;
    char ltyp[MAXLINE];
    char *elt;

    elt = a->type;

    if ((PM_resolve_type(ltyp, ntyp, elt) == TRUE) &&
	(strcmp(ltyp, elt) != 0))
       {sid = SC_type_id(elt, FALSE);
	did = SC_type_id(ltyp, FALSE);

/* change the type name */
	CFREE(elt);

	a->type = CSTRSAVE(ltyp);

/* change the element data */
	ne      = a->length;
	a->data = SC_convert_id(did, NULL, 0, 1,
				sid, a->data, 0, 1, ne, flag);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAKE_MAPPING - given a domain, range, and attributes bind them
 *                 - all together in a freshly allocated PM_mapping
 *                 - and return a pointer to it
 *
 * #bind PM_make_mapping fortran() scheme() python()
 */

PM_mapping *PM_make_mapping(char *name, char *cat,
			    PM_set *domain, PM_set *range,
			    PM_centering centering, PM_mapping *next)
   {int *pi;
    PM_mapping *f;
    pcons *inf;

/* build the map information */
    pi  = CMAKE(int);
    *pi = centering;
    inf = SC_add_alist(NULL, "CENTERING", SC_INT_P_S, (void *) pi);

/* build the mapping */
    f             = CMAKE(PM_mapping);
    f->name       = CSTRSAVE(name);
    f->category   = CSTRSAVE(cat);
    f->domain     = domain;
    f->range      = range;
    f->map_type   = SC_PCONS_P_S;
    f->map        = (void *) inf;
    f->file_info  = NULL;
    f->file_type  = SC_NO_FILE;
    f->file       = NULL;
    f->next       = next;

    SC_mark(f->name, 1);
    SC_mark(f->category, 1);
    SC_mark(domain, 1);
    SC_mark(range, 1);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_BUILD_GROTRIAN - take the data defining a grotrian diagram
 *                   - build and return a PM_mapping for it
 */

PM_mapping *PM_build_grotrian(char *name, char *type,
			      int cp, void *xv, void *yv,
			      char **labels, int n_s,
			      void *tr, void *up, void *low,
			      int n_tr)
   {PM_mapping *pm;
    PM_set *domain, *range;

    domain = PM_make_set("Grotrian-Domain", type, cp,
                         1, n_s, 2, xv, yv);
    if (labels != NULL)
       {domain->info = SC_add_alist(NULL, "GROTRIAN-LABELS",
				    "char **", (void *) labels);
        domain->info_type = CSTRSAVE("pcons *");};

    range  = PM_make_set("Transitions", type, cp,
                         1, n_tr, 3, tr, up, low);
    pm     = PM_make_mapping(name, "Grotrian-Diagram",
                             domain, range, N_CENT, NULL);

    return(pm);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_REL_MAPPING - release the given mapping and its sets
 *
 * #bind PM_rel_mapping fortran() scheme() python()
 */

void PM_rel_mapping(PM_mapping *f, int rld, int rlr)
   {PM_map_info *mi;
    PM_mapping *nxt;

    if (f == NULL)
       return;

    if (SC_safe_to_free(f))
       {mi = (PM_map_info *) f->map;

	if (f->domain != NULL)
	   PM_rel_set(f->domain, rld);
	if (f->range != NULL)
	   PM_rel_set(f->range, rlr);

	CFREE(f->name);
	CFREE(f->category);
	if (strcmp(f->map_type, SC_PCONS_P_S) == 0)
	   SC_free_alist((pcons *) mi, 3);
	else
	   CFREE(mi);

	nxt = f->next;
	if (nxt != NULL)
	   PM_rel_mapping(nxt, rld, rlr);};

    CFREE(f);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_LR_AC_2D_1_CELL - compute the 1 cells for a single LR zone */

static void _PM_lr_ac_2d_1_cell(long *ncell, int k, int l,
				int k1, int k2, int l1, int l2,
				int dkz, int dkn, int ord)
   {int i, iz, in, is;
    int in1, in2, ioc, inc, icc;
    long *pcell;

    iz = (l - l1)*dkz + (k - k1);
    in = (l - l1)*dkn + (k - k1);
    for (i = 0; i < 4; i++)
        {is    = 4*iz + i;
	 pcell = ncell + is*ord;
	 switch (i+1)
	    {case 1 :
	          in1 = in + 1;
		  in2 = in1 + dkn;
		  ioc = (k == k2-1) ? -1 : is + 6;
		  inc = iz + 1;
		  icc = -1;
		  break;

	     case 2 :
		  in1 = in + dkn + 1;
		  in2 = in1 - 1;
		  ioc = (l == l2-1) ? -1 : is + 2 + 4*(dkz - 1) + 4;
		  inc = iz + dkz;
		  icc = -1;
		  break;

	     case 3 :
		  in1 = in + dkn;
		  in2 = in1 - dkn;
		  ioc = (k == k1) ? -1 : is - 3 - 4*(dkz - 1) + 1;
		  inc = iz - 1;
		  icc = -1;
		  break;

	     case 4 :
		  in1 = in;
		  in2 = in1 + 1;
		  ioc = (l == l1) ? -1 : is - 3 - 4*dkz + 1;
		  inc = iz - dkz;
		  icc = -1;
		  break;};

	 switch (ord-1)
	    {case DEC_CELL_MAX :
	     case DEC_CELL_MIN :
	          pcell[DEC_CELL_MIN] = -1;
		  pcell[DEC_CELL_MAX] = -1;
	      
	     case CENTER_CELL :
		  pcell[CENTER_CELL] = icc;
	      
	     case NGB_CELL:
		  pcell[NGB_CELL] = inc;
	      
	     case PARENT_CELL :
		  pcell[PARENT_CELL] = iz;
	      
	     case OPPOSITE_CELL :
		  pcell[OPPOSITE_CELL] = ioc;
	      
	     case BND_CELL_MAX :
	     case BND_CELL_MIN :
		  pcell[BND_CELL_MIN] = in1;
		  pcell[BND_CELL_MAX] = in2;
		  break;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PM_LR_AC_2D_2_CELL - compute the 2 cells for a single LR zone */

static int _PM_lr_ac_2d_2_cell(long *ncell, int fz, int k, int l,
			       int k1, int l1, int dkz, int dkn, int ord)
   {int iz;
    long *pcell;

    iz    = (l - l1)*dkz + (k - k1);
    pcell = ncell + iz*ord;
    switch (ord-1)
       {case DEC_CELL_MAX :
	case DEC_CELL_MIN :
	     pcell[DEC_CELL_MIN] = fz;
	     pcell[DEC_CELL_MAX] = fz + 3;
	     fz += 4;

	case CENTER_CELL :
	     pcell[CENTER_CELL] = -1;

	case NGB_CELL :
	     pcell[NGB_CELL] = -1;

	case PARENT_CELL :
	     pcell[PARENT_CELL] = -1;

	case OPPOSITE_CELL :
	     pcell[OPPOSITE_CELL] = -1;

	case BND_CELL_MAX :
	case BND_CELL_MIN :
	     pcell[BND_CELL_MIN] = 4*iz;
	     pcell[BND_CELL_MAX] = 4*iz + 3;
	     break;};

    return(fz);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_LR_AC_MESH_2D - map a 2D logical rectangular mesh section to an
 *                  - arbitrarily connected representation
 *                  - Arguments are:
 *                  -   (x, y)       arrays of x and y values
 *                  -   (kmax, lmax) logical extent of mesh
 *                  -   (k1, k2)     logical extent in k of desired section
 *                  -   (l1, l2)     logical extent in l of desired section
 *                  -   ord          number of boundary parameters to use
 */

PM_mesh_topology *PM_lr_ac_mesh_2d(double **px, double **py,
				   int kmax, int lmax,
				   int k1, int k2, int l1, int l2, int ord)
   {int i, j, k, l, fz, dkz, dkn;
    int ns, nz, nrz, nn, nrn;
    int *nbp, *nc;
    long **bnd, *ncell;
    double *x, *y, *rx, *ry;
    PM_mesh_topology *mt;

    dkz = k2 - k1;
    nrz = dkz*(l2 - l1);
    dkn = k2 - k1 + 1;
    nrn = dkn*(l2 - l1 + 1);

    nn = nrn;
    nz = nrz;
    fz = nrz;
    switch (ord)
       {case DEC_CELL_MAX  :
        case DEC_CELL_MIN  :
             ord = DEC_CELL_MAX;
             nz += 4*nrz;
             fz  = nrz;
             break;

        case CENTER_CELL   :
        case NGB_CELL      :
        case PARENT_CELL   :
        case OPPOSITE_CELL :
             break;

        case BND_CELL_MAX  :
        case BND_CELL_MIN  :
             ord = BND_CELL_MAX;
             break;};

    ord++;
    ns  = 4*nz;

/* setup new node arrays */
    x = CMAKE_N(double, nn);
    y = CMAKE_N(double, nn);

    rx = *px;
    ry = *py;
    for (l = l1; l <= l2; l++)
        for (k = k1; k <= k2; k++)
            {i    = (l - l1)*dkn + (k - k1);
             j    = (l - 1)*kmax + k - 1;
             x[i] = rx[j];
             y[i] = ry[j];};

/* allocate the boundary arrays */
    bnd = CMAKE_N(long *, 3);
    bnd[2] = CMAKE_N(long, ord*nz);
    bnd[1] = CMAKE_N(long, ord*ns);
    bnd[0] = NULL;

/* fill the 2-cells */
    ncell = bnd[2];
    for (l = l1; l < l2; l++)
        for (k = k1; k < k2; k++)
	    fz = _PM_lr_ac_2d_2_cell(ncell, fz, k, l, k1, l1, dkz, dkn, ord);

/* fill the 1-cells */
    ncell = bnd[1];
    for (l = l1; l < l2; l++)
        for (k = k1; k < k2; k++)
	    _PM_lr_ac_2d_1_cell(ncell, k, l, k1, k2, l1, l2, dkz, dkn, ord);

/* setup the number of cells array */
    nc = CMAKE_N(int, 3);
    nc[0] = nn;
    nc[1] = ns;
    nc[2] = nz;

/* setup the number of boundary parameters array */
    nbp = CMAKE_N(int, 3);
    nbp[0] = 1;
    nbp[1] = ord;
    nbp[2] = ord;

/* put it all together */
    mt = PM_make_topology(2, nbp, nc, bnd);

    *px = x;
    *py = y;

    return(mt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAPPING_INFO - extract any of the following information from
 *                 - a mapping
 */

pcons *PM_mapping_info(PM_mapping *h, ...)
   {int id;
    char **ps, *name, bf[MAXLINE];
    pcons *map_alist, *asc;
    PM_map_info *hmap;

    if (h == NULL)
       return(NULL);

    if (h->map == NULL)
       return(NULL);

    SC_VA_START(h);

    if (strncmp(h->map_type, "PM_map_info", 11) == 0)
       {hmap = (PM_map_info *) (h->map);
	map_alist = NULL;}
    else if (strcmp(h->map_type, SC_PCONS_P_S) == 0)
       {map_alist = (pcons *) (h->map);
	hmap = NULL;}
    else
       return(NULL);

    while (TRUE)
       {name = SC_VA_ARG(char *);
        if (name == NULL)
           break;

        if (map_alist != NULL)
	   {for (asc = map_alist; asc != NULL; asc = (pcons *) asc->cdr)
	        if (strcmp((char *) ((pcons *) asc->car)->car, name) == 0)
		   {asc = (pcons *) asc->car;
		    break;};

            if (asc != NULL)
               {strcpy(bf, asc->cdr_type);
		SC_dereference(bf);

		id = SC_type_id(bf, FALSE);
		if (SC_is_type_num(id) == TRUE)
		   {SC_VA_ARG_STORE(id, asc->cdr);}

                else if (id == SC_STRING_I)
	           {ps  = SC_VA_ARG(char **);
                    *ps = (char *) asc->cdr;};};}

        else if ((hmap != NULL) && (strncmp(name, "CENTERING", 11) == 0))
	   {SC_VA_ARG_STORE(id, &hmap->centering);};};

    SC_VA_END;

    return(map_alist);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_GET_LIMITS - get the "LIMITS" property of the specified set */

double *PM_get_limits(PM_set *s)
   {double *extr;

    extr = NULL;
    if ((s != NULL) && (s->info_type != NULL))
       if (strcmp(s->info_type, SC_PCONS_P_S) == 0)
	  SC_assoc_info((pcons *) s->info,
			"LIMITS", &extr,
			NULL);

    return(extr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_SET_LIMITS - set the "LIMITS" property of the specified set
 *               - assume that EXTR has been dynamically allocated
 *               - and can be released whenever it suits the library
 */

void PM_set_limits(PM_set *s, double *extr)
   {pcons *inf, *data;

/* get the current list */
    if (s->info_type != NULL)
       {if (strcmp(s->info_type, SC_PCONS_P_S) == 0)
	   inf = (pcons *) s->info;

        else
	   inf = NULL;}
    else
       inf = NULL;

    data = SC_assoc_entry(inf, "LIMITS");

/* if there are no limits, remove any existing one
 * this does dom de and ran de
 */
    if (extr == NULL)
       {if (data != NULL)
           {inf = SC_rem_alist(inf, "LIMITS");

/* GOTCHA: data was just released by SC_rem_alist and it just happened to
 *         corrupt memory one time
 *         this has been here for years which is why the comment
 *         in case this has to be restored for some other reason
 *         this can all go away by 6/1/03 if nothing turns up
 *	    SC_rl_pcons(data, 3);
 */
           };}

/* if it's there change it */
    else if (data != NULL)
       {CFREE(data->cdr);
	data->cdr = (void *) extr;}

/* otherwise add it */
    else
       inf = SC_change_alist(inf, "LIMITS", SC_DOUBLE_P_S, (void *) extr);

    s->info_type = SC_PCONS_P_S;
    s->info      = (void *) inf;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_MAP_INFO_ALIST - convert PM_map_info data to an alist
 *                   -   PM_centering centering -> "CENTERING"
 */

pcons *PM_map_info_alist(PM_map_info *ti)
   {pcons *inf;
    int *pi;

    inf = NULL;

    pi  = CMAKE(int);
    *pi = ti->centering;
    inf = SC_add_alist(inf, "CENTERING", SC_INT_P_S, (void *) pi);

    return(inf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PM_CHECK_EMAP - return the existence map in the form of a byte array
 *               - check ALST for a map or allocate one that is N long
 *               - set EFLAG to TRUE if a new space was allocated
 */

char *PM_check_emap(int *peflag, pcons *alst, long n)
   {int sid, eflg;
    pcons *pc;
    char *emap;

    if (alst != NULL)
       pc = SC_assoc_entry(alst, "EXISTENCE");
    else
       pc = NULL;

    eflg = (pc == NULL);
    if (eflg == TRUE)
       {emap = CMAKE_N(char, n);
	memset(emap, 1, n);}

    else
       {sid = SC_deref_id(pc->cdr_type, FALSE);
	if (sid == SC_CHAR_I)
	   emap = pc->cdr;
	else
	   {emap = SC_convert_id(SC_CHAR_I, NULL, 0, 1,
				 sid, pc->cdr, 0, 1,
				 n, FALSE);
	    eflg = TRUE;};};

    *peflag = eflg;

    return(emap);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
