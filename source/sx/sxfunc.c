/*
 * SXFUNC.C - basic math functions for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

#define GET_DATA_1D(_f, _n, _x, _y)                                         \
    {PM_set *_dm, *_rn;                                                     \
     _dm = (_f)->domain;                                                    \
     _rn = (_f)->range;                                                     \
     _n  = _dm->n_elements;                                                 \
     _x  = PM_array_real(_dm->element_type,                                 \
			 DEREF(_dm->elements), _n, NULL);                   \
     _y  = PM_array_real(_rn->element_type,                                 \
			 DEREF(_rn->elements), _n, NULL);}

/*--------------------------------------------------------------------------*/

/*                          RUDAMENTARY FUNCTIONS                           */

#if 0

/*--------------------------------------------------------------------------*/

/* COMPOSE - a functional composition of curves */

double compose(double a, int i)
   {int j;
    double x1, x2, y1, y2, rv;

    rv = HUGE;

    j = lfind(a, dataset[i].x[0], dataset[i].n);
    if (j >= 0)
       {x1 = dataset[i].x[0][j];
        y1 = dataset[i].x[1][j];
        x2 = dataset[i].x[0][j+1];
        y2 = dataset[i].x[1][j+1];
	rv = interp(a, x1, y1, x2, y2);
        return(rv);};

    return(rv);}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* SX_QUIT - gracefully exit from PDBView */

void SX_quit(int i)
   {

    SC_exit_all();

/* check the need to close the command log */
    if (SX_command_log != NULL)
       io_close(SX_command_log);

    exit(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_QUIT - gracefully exit from this session */

static object *_SXI_quit(SS_psides *si, object *arg)
   {int rv;

    SS_args(si, arg,
            SC_INT_I, &rv,
            0);

    SX_quit(rv);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_TOGGLE_LOG - toggle the command log file */

static object *_SXI_toggle_log(SS_psides *si, object *argl)
   {object *obj;

    if (SX_command_log != NULL)
       {io_close(SX_command_log);
	SX_command_log = NULL;};

    if (SS_consp(argl))
       {obj       = SS_car(si, argl);
        _SX.fname = SC_dsnprintf(FALSE, SS_get_string(obj));
        if (strcmp(_SX.fname, "off") == 0)
           *_SX.fname = '\0';
        else if (strcmp(_SX.fname, "on") == 0)
           _SX.fname = SC_dsnprintf(FALSE, SX_command_log_name);}
    else
       _SX.fname = SC_dsnprintf(FALSE, SX_command_log_name);

    if ((_SX.fname != NULL) && (*_SX.fname != '\0'))
       {SX_command_log = io_open(_SX.fname, "a");
        if (SX_command_log == NULL)
           SS_error(si, "CANNOT OPEN LOG FILE - _SXI_TOGGLE_LOG", SS_null);
        else
           SC_setbuf(SX_command_log, NULL);};

    obj = (SX_command_log == NULL) ? SS_f : SS_t;

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_DIMENSION - return a cons whose car is the dimension of the
 *                    - domain and whose cdr is the dimension of the range
 *                    - of the given mapping
 */

static object *_SXI_get_dimension(SS_psides *si, object *obj)
   {int ndd, ndr;
    PG_graph *g;
    PM_mapping *f;
    PM_set *set;
    object *rv;

    f = NULL;
    if (SX_GRAPHP(obj))
       {g = SS_GET(PG_graph, obj);
        f = g->f;}
    else if (SX_MAPPINGP(obj))
       f = SS_GET(PM_mapping, obj);
    else
       SS_error(si, "BAD MAPPING - _SXI_GET_DIMENSION", obj);

    if (f != NULL)
       {set = f->domain;
	ndd = (set == NULL) ? 0 : set->dimension_elem;
	set = f->range;
	ndr = (set == NULL) ? 0 : set->dimension_elem;

	rv = SS_mk_cons(si, SS_mk_integer(si, ndd), SS_mk_integer(si, ndr));}
    else
       rv = SS_null;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_DOMAIN - return the domain of the mapping object as a set object */

static object *_SXI_get_domain(SS_psides *si, object *arg)
   {PM_set *set;
    object *rv;

    set = NULL;
    if (SX_MAPPINGP(arg))
       set = MAPPING_DOMAIN(arg);
    else if (SX_GRAPHP(arg))
       set = GRAPH_F(arg)->domain;
    else if (SX_SETP(arg))
       set = SS_GET(PM_set, arg);
    else
       SS_error(si, "BAD SET - _SXI_GET_DOMAIN", arg);

    rv = SX_mk_set(si, set);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_RANGE - return the range of the mapping object as a set object */

static object *_SXI_get_range(SS_psides *si, object *arg)
   {PM_mapping *f;
    object *rv;

    f = NULL;
    SS_args(si, arg,
            G_MAPPING, &f,
	    0);

    rv = NULL;
    if (f == NULL)
       SS_error(si, "BAD MAPPING - _SXI_GET_RANGE", arg);

    else
       rv = SX_mk_set(si, f->range);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_GET_MAPPING_NAME - return the name of the mapping object
 *                       - as a string object
 */

static object *_SXI_get_mapping_name(SS_psides *si, object *arg)
   {char *name;
    object *rv;

    if (!SX_MAPPINGP(arg))
       SS_error(si, "BAD MAPPING - _SXI_GET_RANGE", arg);

    name = MAPPING_NAME(arg);
    rv   = SS_mk_string(si, name);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SET_VOLUME - return the product of the differences of the extrema
 *                 - of the given set
 */

static object *_SXI_set_volume(SS_psides *si, object *arg)
   {int i, nd;
    double *extr, vol, mn, mx;
    PM_set *set;
    object *o;

    if (!SX_SETP(arg))
       SS_error(si, "BAD SET - _SXI_SET_VOLUME", arg);

    set = SS_GET(PM_set, arg);
    
    extr = (double *) set->extrema;
    nd   = set->dimension_elem;
    vol  = 1.0;
    for (i = 0; i < nd; i++)
        {mn = *extr++;
         mx = *extr++;
         vol *= (mx - mn);};
    
    o = SS_mk_float(si, vol);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SHIFT_SET - shift the components of all elements of the given
 *               - set by the given value
 */

static void _SX_shift_set(PM_set *set, double val)
   {double **elem, *pe;
    int i, j, nde, ne;

    ne   = set->n_elements;
    nde  = set->dimension_elem;
    elem = (double **) set->elements;

    for (i = 0; i < nde; i++)
        {pe = elem[i];
         for (j = 0; j < ne; j++)
             *pe++ += val;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SHIFT_DOMAIN - shift all components of all elements of the
 *                   - domain of the given mapping
 *                   - return the mapping
 */

static object *_SXI_shift_domain(SS_psides *si, object *argl)
   {object *obj, *ret;
    PM_set *set;
    double val;

    ret = SS_null;
    if (SS_consp(argl))
       {SX_GET_OBJECT_FROM_LIST(si, SX_MAPPINGP(obj), set,
				MAPPING_DOMAIN(obj),
                                argl, "BAD MAPPING - _SXI_SHIFT_DOMAIN");
        SS_assign(si, ret, obj);};

    if (SS_consp(argl))
       {SX_GET_FLOAT_FROM_LIST(si, val, argl,
                               "BAD FLOATING POINT VALUE - _SXI_SHIFT_DOMAIN");}

    else
       SS_error(si, "INSUFFICIENT ARGUMENTS - _SXI_SHIFT_DOMAIN", argl);

    _SX_shift_set(set, val);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SHIFT_RANGE - shift all components of all elements of the
 *                  - range of the given mapping
 *                  - return the mapping
 */

static object *_SXI_shift_range(SS_psides *si, object *argl)
   {object *obj, *ret;
    PM_set *set;
    double val;

    ret = SS_null;
    if (SS_consp(argl))
       {SX_GET_OBJECT_FROM_LIST(si, SX_MAPPINGP(obj), set, MAPPING_RANGE(obj),
                                argl, "BAD MAPPING - _SXI_SHIFT_RANGE");
        SS_assign(si, ret, obj);};

    if (SS_consp(argl))
       {SX_GET_FLOAT_FROM_LIST(si, val, argl,
                               "BAD FLOATING POINT VALUE - _SXI_SHIFT_RANGE");}

    else
       SS_error(si, "INSUFFICIENT ARGUMENTS - _SXI_SHIFT_RANGE", argl);

    _SX_shift_set(set, val);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_SCALE_SET - scale the components of all elements of the given
 *               - set by the given value
 */

static void _SX_scale_set(PM_set *set, double val)
   {double **elem, *pe;
    int i, j, nde, ne;

    ne   = set->n_elements;
    nde  = set->dimension_elem;
    elem = (double **) set->elements;

    for (i = 0; i < nde; i++)
        {pe = elem[i];
         for (j = 0; j < ne; j++)
             *pe++ *= val;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SCALE_DOMAIN - scale all components of all elements of the
 *                   - domain of the given mapping
 *                   - return the mapping
 */

static object *_SXI_scale_domain(SS_psides *si, object *argl)
   {object *obj, *ret;
    PM_set *set;
    double val;

    ret = SS_null;
    if (SS_consp(argl))
       {SX_GET_OBJECT_FROM_LIST(si, SX_MAPPINGP(obj), set,
				MAPPING_DOMAIN(obj),
                                argl, "BAD MAPPING - _SXI_SCALE_DOMAIN");
        SS_assign(si, ret, obj);};

    if (SS_consp(argl))
       {SX_GET_FLOAT_FROM_LIST(si, val, argl,
                               "BAD FLOATING POINT VALUE - _SXI_SCALE_DOMAIN");}
    else
       SS_error(si, "INSUFFICIENT ARGUMENTS - _SXI_SCALE_DOMAIN", argl);

    _SX_scale_set(set, val);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SCALE_RANGE - scale all components of all elements of the
 *                  - range of the given mapping
 *                  - return the mapping
 */

static object *_SXI_scale_range(SS_psides *si, object *argl)
   {object *obj, *ret;
    PM_set *set;
    double val;

    ret = SS_null;
    if (SS_consp(argl))
       {SX_GET_OBJECT_FROM_LIST(si, SX_MAPPINGP(obj), set, MAPPING_RANGE(obj),
                                argl, "BAD MAPPING - _SXI_SCALE_RANGE");
        SS_assign(si, ret, obj);};

    if (SS_consp(argl))
       {SX_GET_FLOAT_FROM_LIST(si, val, argl,
                               "BAD FLOATING POINT VALUE - _SXI_SCALE_RANGE");}

    else
       SS_error(si, "INSUFFICIENT ARGUMENTS - _SXI_SCALE_RANGE", argl);

    _SX_scale_set(set, val);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MAP_COUNT - keep track of generated mappings */

int SX_map_count(void)
   {

    if (_SX.mid > 'z')
       _SX.mid = 'a';

    return(_SX.mid++);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_NORM_MAPPING - build a mapping whose range elements are the Euclidean
 *                   - norm of the input mapping range elements
 *                   - return a new mapping
 */

static PM_mapping *_SXI_norm_mapping(SS_psides *si, PM_mapping *h)
   {int i, j, nde, ne;
    char *lbl;
    double **sre, *dre, *pr, *ps, dx;
    void *elem[1];
    PM_centering centering;
    PM_set *domain, *range;
    PM_mapping *f;

/* find the additional mapping information */
    centering = N_CENT;
    PM_mapping_info(h,
		    "CENTERING", &centering,
		    NULL);

/* build the return mapping */
    domain = PM_copy_set(h->domain);
    range  = h->range;

    nde = range->dimension_elem;
    ne  = range->n_elements;
    sre = (double **) range->elements;

    dre = CMAKE_N(double, ne);

    for (j = 0; j < nde; j++)
        {pr = dre;
         ps = sre[j];
         for (i = 0; i < ne; i++)
             {dx     = *ps++;
              *pr++ += dx*dx;};};

    pr = dre;
    for (i = 0; i < ne; i++, pr++)
        *pr = sqrt(*pr);

    elem[0] = (void *) dre;
    lbl = SC_dsnprintf(FALSE, "||%s||", range->name);
    range = PM_make_set_alt(lbl, SC_DOUBLE_S, FALSE,
			    range->dimension, range->max_index,
			    nde, elem);

    lbl = SC_dsnprintf(FALSE, "%s->%s", domain->name, range->name);
    f = PM_make_mapping(lbl, PM_LR_S, domain, range, centering, NULL);

    return(f);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_COPY_MAPPING - make a copy of the given mapping */

static PM_mapping *_SXI_copy_mapping(SS_psides *si, PM_mapping *h)
   {char *name, *cat;
    PM_centering centering;
    PM_set *domain, *range;
    PM_mapping *f;

    domain = PM_copy_set(h->domain);
    range  = PM_copy_set(h->range);
    cat    = h->category;
    name   = SC_dsnprintf(FALSE, "copy %s", h->name);

/* find the additional mapping information */
    centering = N_CENT;
    PM_mapping_info(h,
		    "CENTERING", &centering,
		    NULL);

    f = PM_make_mapping(name, cat, domain, range, centering, NULL);

    if (strcmp(f->map_type, SC_PCONS_P_S) == 0)
       {SC_free_alist(f->map, 0);
	f->map = SC_copy_alist(h->map);};

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_INTEGRATE_MAPPING - integrate a mapping in place */

static void _SX_integrate_mapping(PM_mapping *f)
   {PM_set *domain, *range;
    int i, j, jn, jc, is, ndd, nde, ned, ndr;
    int id, lne, dj, ix;
    int *dmx;
    long step, npts;
    double vol;
    double **re, *dsc, *rn;

/* build the return mapping */
    domain = f->domain;
    range  = f->range;

    dsc = (double *) domain->scales;
    ndd = domain->dimension;
    nde = domain->dimension_elem;
    dmx = domain->max_index;
    ned = domain->n_elements;

    ndr = range->dimension_elem;
    re  = (double **) range->elements;

/* compute the node volume */
    vol = 1.0;
    for (j = 0; j < nde; j++)
        vol *= dsc[j];

/* multiply by the volume */
    for (j = 0; j < ndr; j++)
        {rn = re[j];
         for (i = 0; i < ned; i++)
             *rn++ *= vol;};

/* scale each face by one half - corrects volume factor */
/*
    for (i = 0; i < ned; i++)
        {id  = i;
         lne = ned;
         for (j = ndd-1; j >= 0; j--)
             {dj   = dmx[j];
              lne /= dj;
              ix   = id / lne;
              if ((ix == 0) || (ix == dj-1))
                 for (jc = 0; jc < ndr; jc++)
                     re[jc][i] *= 0.5;
              id %= lne;};};
*/
/* sum it up */
    lne = ned;
    for (j = ndd-1; j >= 0; j--)
        {npts = 1L;
         for (jn = 0; jn <= j; jn++)
             npts *= dmx[jn];

         dj   = dmx[j];
         step = lne;
         lne /= dj;
         for (i = 0; i < ned; i++)
             {id = i % step;
              ix = id / lne;
              if (ix == 0)
                 for (jc = 0; jc < ndr; jc++)
                     {rn = re[jc] + i;
                      for (is = lne; is < npts; is += lne)
                          rn[is] += rn[is-lne];};};};

    return;}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_INTEGRATE_MAPPING - integrate a mapping and
 *                        - return a new mapping
 */

static PM_mapping *_SXI_integrate_mapping(SS_psides *si, PM_mapping *h)
   {PM_mapping *f;
    char *lbl;

    lbl = SC_dsnprintf(FALSE, "Integral(%s)", h->name);

/* build the return mapping */
    f = SX_build_return_mapping(si, h, lbl, NULL, TRUE, FALSE);

    _SX_integrate_mapping(f);

    PM_find_extrema(f->range);

    return(f);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_PLANE - generate the hyper-plane mapping: sum(i = 0,n; ci*xi)
 *          - usage: (hyper-plane c0 (c1 x1min x1max np1) ...)
 */

object *SX_plane(SS_psides *si, object *argl)
   {int i, j, nd, nde, plf;
    int *maxes, *pm;
    long ne;
    double *extr, *px, *coeff, *pc, *r, *pr, *pt, *ratio, v;
    double xmn, xmx;
    double **delem;
    char *name;
    object *lst, *mo;
    PM_set *dom, *ran;
    PM_mapping *f;

    plf   = SX_have_display_list(si);
    nd    = SS_length(si, argl);
    coeff = pc = CMAKE_N(double, nd--);

    SS_args(si, argl,
	    SC_DOUBLE_I, pc++,
            0);
    argl = SS_cdr(si, argl);

/* organize the args into input to make the domain */
    ratio = pt = CMAKE_N(double, nd);
    maxes = pm = CMAKE_N(int, nd);
    extr  = px = CMAKE_N(double, 2*nd);
    for (i = 0, ne = 0L; i < nd; i++, argl = SS_cdr(si, argl))
        {lst = SS_car(si, argl);
	 
	 ratio[i] = 1.0;
	 SS_args(si, lst,
		 SC_DOUBLE_I, pc++,
		 SC_DOUBLE_I, &xmn,
		 SC_DOUBLE_I, &xmx,
		 SC_INT_I, pm++,
		 SC_DOUBLE_I, &ratio[i],
		 0);
	 *px++ = xmn;
	 *px++ = xmx;};

    nde = nd;

/* make the domain */
    name = SC_dsnprintf(FALSE, "D%d_%d", nd, nde);
    dom  = PM_make_lr_index_domain(name, SC_DOUBLE_S, nd, nde,
				   maxes, extr, ratio);

    CFREE(extr);

/* make the range */
    ne    = dom->n_elements;
    delem = (double **) dom->elements;

    r  = pr = CMAKE_N(double, ne);
    for (i = 0; i < ne; i++)
        {for (j = 0, v = coeff[0]; j < nde; j++)
             {v += coeff[j+1]*delem[j][i];};
	 *pr++ = v;};

    ran = PM_make_set("Plane", SC_DOUBLE_S, FALSE, 1, ne, 1, r);

/* make the mapping */
    name = SC_dsnprintf(FALSE, "%s->%s", dom->name, ran->name);
    f    = PM_make_mapping(name, PM_LR_S, dom, ran, N_CENT, NULL);

    mo = SX_mk_mapping(si, f);
    if (plf)
       mo = SX_display_map(si, mo);

    return(mo);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DERIVATIVE - take the derivative of a 1d mapping */

static PM_mapping *_SXI_derivative(SS_psides *si, PM_mapping *h)
   {int n, m;
    double *x[PG_SPACEDM];
    double *bx, *by;
    char *lbl;
    PM_mapping *f;
    PM_set *d, *r;

    GET_DATA_1D(h, n, x[0], x[1]);

    m = n + 5;

    bx = CMAKE_N(double, m);
    by = CMAKE_N(double, m);

    PM_derivative(n, x[0], x[1], bx, by);

    lbl = SC_dsnprintf(FALSE, "d/dx %s", h->name);

    if (n == 2)
       {n = 3;
	bx[0] = x[0][0];
	bx[1] = x[0][1];
	by[1] = by[0];}
    else if (n > 2)
       n--;

    d = PM_make_set("X", SC_DOUBLE_S, FALSE, 1, n, 1, bx);
    r = PM_make_set("Y", SC_DOUBLE_S, FALSE, 1, n, 1, by);
    f = PM_make_mapping(lbl, PM_LR_S, d, r, N_CENT, NULL);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_FILTER_COEFF - worker for coefficient based filters */

void SX_filter_coeff(SS_psides *si, double *yp, int n,
		     C_array *arr, int ntimes)
   {int i, ne, nc, nh, ne0, sid;
    char type[MAXLINE];
    double *coeff;
    void *d;

    if (arr != NULL)
       {PM_ARRAY_CONTENTS(arr, void, ne, type, d);
	sid   = SC_type_id(type, FALSE);
	coeff = SC_convert_id(SC_DOUBLE_I, NULL, 0, 1,
			      sid, d, 0, 1, ne, FALSE);
        ne--;

        nc  = coeff[0];
        nh  = nc >> 1;
        ne0 = nc + nh*(3*nh + 1);
        if (ne != ne0)
           SS_error(si, "INCORRECT FILTER SIZE - _SX_FILTER_COEF", SS_null);

	for (i = 0; i < ntimes; i++)
	    PM_filter_coeff(yp, n, coeff + 1, nc);

	CFREE(coeff);}

    else
       SS_error(si, "BAD COEFFICIENT ARRAY - _SX_FILTER_COEF", SS_null);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FILTER_COEF - the n point filter routine */

static PM_mapping *_SXI_filter_coef(SS_psides *si, PM_mapping *h,
				    object *argl)
   {int n, ntimes;
    double *x[PG_SPACEDM];
    C_array *arr;

    GET_DATA_1D(h, n, x[0], x[1]);

    arr    = NULL;
    ntimes = 1;
    SS_args(si, argl,
            G_NUM_ARRAY, &arr,
	    SC_INT_I, &ntimes,
	    0);

    SX_filter_coeff(si, x[1], n, arr, ntimes);

    PM_find_extrema(h->range);

    return(h);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SMOOTH - the n point smoothing routine */

static PM_mapping *_SXI_smooth(SS_psides *si, PM_mapping *h, object *argl)
   {int i, n, pts, ntimes;
    double *x[PG_SPACEDM];


    GET_DATA_1D(h, n, x[0], x[1]);

    pts    = 3;
    ntimes = 1;
    SS_args(si, argl,
	    SC_INT_I, &pts,
	    SC_INT_I, &ntimes,
	    0);

    if (SC_str_icmp(SX_smooth_method, "fft") == 0)
       {for (i = 0; i < ntimes; i++)
	    PM_smooth_fft(x[0], x[1], n, pts, PM_smooth_filter);}

    else if (SC_str_icmp(SX_smooth_method, "averaging") == 0)
       {for (i = 0; i < ntimes; i++)
	    PM_smooth_int_ave(x[0], x[1], n, pts);}

    else
       {C_array *arr;
	char *bf;
	object *obj;

        obj = SS_INQUIRE_OBJECT(si, SX_smooth_method);
        if (obj == NULL)
           {bf = SC_dsnprintf(FALSE, "NO FILTER NAMED %s EXISTS - _SXI_SMOOTH",
			      SX_smooth_method);
	    SS_error(si, bf, SS_null);};

        SS_args(si, SS_lk_var_val(si, obj),
                G_NUM_ARRAY, &arr,
		0);

        if (arr == NULL)
           {bf = SC_dsnprintf(FALSE, "%s IS NOT A FILTER - _SXI_SMOOTH",
			      SX_smooth_method);
	    SS_error(si, bf, SS_null);};

	SX_filter_coeff(si, x[1], n, arr, ntimes);};

    PM_find_extrema(h->range);

    return(h);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PP_NAMES - pretty print a list of names
 *               - this should be generalized later
 */

static object *_SXI_pp_names(SS_psides *si, object *argl)
   {int i, j, k, n, m, jo, nc, nchar, ncol, ncc, nrow, nfcl, nfrw;
    char bf[MAXLINE], **lst, *text, **slst;
    object *obj;

    n   = SS_length(si, argl);
    lst = CMAKE_N(char *, n+1);

    nchar = 0;
    for (i = 0; i < n; i++)
        {SX_GET_STRING_FROM_LIST(text, argl, NULL);
         lst[i] = text;
         nc     = strlen(text);
         nchar  = max(nchar, nc);};

    lst[n] = NULL;

    memset(bf, ' ', MAXLINE);
    bf[MAXLINE-1] = '\0';

    nchar += 3;

    m    = n;
    ncol = 80/nchar;
    ncol = max(1, ncol);
    ncc  = (80 + ncol - 1)/ncol;
    nrow = (n + ncol - 1)/ncol;
    nfrw = n/ncol;
    if (nrow*ncol != n)
       nfcl = n - ncol*nfrw;
    else
       nfcl = ncol;

    slst = lst + nfcl*nrow;

    for (i = 0; i < nrow; i++)
        {for (j = 0; j < nfcl; j++)
             {k = j*nrow + i;
              m--;
              memcpy(&bf[j*ncc], lst[k], strlen(lst[k]));};

         for (jo = 0; jo < ncol - nfcl; jo++, j++)
             {k = jo*(nrow-1) + i;
              if (m <= 0)
                 continue;
              m--;
              memcpy(&bf[j*ncc], slst[k], strlen(slst[k]));};

	 SC_trim_right(bf, " \t\r\n");
         PRINT(stdout, "%s\n", bf);
         memset(bf, ' ', MAXLINE);
	 bf[MAXLINE-1] = '\0';};

    SC_free_strings(lst);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INSTALL_GLOBAL_FUNCS - install some functions that everybody can use */

void SX_install_global_funcs(SS_psides *si)
   {

    SS_install(si, "command-log",
               "Macro: Control command logging\n     Usage: command-log [on | off | <filename>]",
               SS_znargs,
               _SXI_toggle_log, SS_UR_MACRO);

    SS_install(si, "curve?",
               "Prodedure: Return #t iff the argument is an Ultra curve object\n     Usage: curve? <object>",
               SS_sargs,
               _SXI_curveobjp, SS_PR_PROC);

    SS_install(si, "end",
               "Procedure: End the session\n     Usage: end",
               SS_sargs,
               _SXI_quit, SS_PR_PROC);

    SS_install(si, "ld",
               "Macro: Read SCHEME forms from file\n     Usage: ld <file-name>",
               SS_nargs,
               SS_load, SS_UR_MACRO);

    SS_install(si, "pm-mapping-dimension",
               "Returns (d(domain). d(range)) of the given mapping",
               SS_sargs,
               _SXI_get_dimension, SS_PR_PROC);

    SS_install(si, "pm-mapping-domain",
               "Returns the domain of the given mapping",
               SS_sargs,
               _SXI_get_domain, SS_PR_PROC);

    SS_install(si, "pm-mapping-range",
               "Returns the range of the given mapping",
               SS_sargs,
               _SXI_get_range, SS_PR_PROC);

    SS_install(si, "pm-mapping-name",
               "Returns the name of the given mapping",
               SS_sargs,
               _SXI_get_mapping_name, SS_PR_PROC);

    SS_install(si, "pm-set-volume",
               "Return the product of the extrema of the given set",
               SS_sargs,
               _SXI_set_volume, SS_PR_PROC);

    SS_install(si, "pm-shift-domain",
               "Add a scalar value to all components of a mapping domain",
               SS_nargs,
               _SXI_shift_domain, SS_PR_PROC);

    SS_install(si, "pm-shift-range",
               "Add a scalar value to all components of a mapping range",
               SS_nargs,
               _SXI_shift_range, SS_PR_PROC);

    SS_install(si, "pm-scale-domain",
               "Multiply all components of a mapping domain by a scalar value",
               SS_nargs,
               _SXI_scale_domain, SS_PR_PROC);

    SS_install(si, "pm-scale-range",
               "Multiply all components of a mapping range by a scalar value",
               SS_nargs,
               _SXI_scale_range, SS_PR_PROC);

    SS_install(si, "pp",
               "Procedure: pretty print a list",
               SS_sargs,
               _SXI_pp_names, SS_PR_PROC);

    SS_install(si, "ultra-file?",
               "Procedure: Return #t iff the file is a valid ULTRA II file\n     Usage: ultra-file? <file-name>",
               SS_sargs,
               _SXI_valid_ultra_filep, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MF_INST_G - install math functions from this file scope */

void _SX_mf_inst_g(SS_psides *si)
   {

    SS_install(si, "copy-map",
               "Copies a mapping\nFORM copy-mapping <graph-list>",
               _SX_mh_u_m,
	       _SXI_copy_mapping, SS_PR_PROC);

    SS_install(si, "derivative",
               "Take derivative of curves\nFORM derivative <graph-list>",
               _SX_mh_u_m,
	       _SXI_derivative, SS_PR_PROC);

    SS_install(si, "filter-coef",
	       "Filter a mapping through a set of coefficents\nFORM: filter-coef (<coeff-array> <ntimes>) <mappings>*",
               _SX_m11_b_mro,
	       _SXI_filter_coef, SS_PR_PROC);

    SS_install(si, "integrate",
               "Integrate mappings\nFORM integrate <graph-list>",
               _SX_mh_u_m,
	       _SXI_integrate_mapping, SS_PR_PROC);

    SS_install(si, "norm",
               "Euclidean norm of range elements\nFORM norm <graph-list>",
               _SX_mh_u_m,
	       _SXI_norm_mapping, SS_PR_PROC);

    SS_install(si, "smooth",
               "Smooth mapping using n point integral to average\nFORM smooth <graph-list> <n>",
               _SX_m11_b_mro,
	       _SXI_smooth, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
