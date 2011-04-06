/*
 * GPP.C - interface for interactive and post processing
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "gloin.h"

static PG_device
 *global_dev;

extern object
 *LR_get_time_data(byte),
 *LR_get_mesh_data(byte),
 *LR_lrm_plot(object *argl),
 *LR_def_domain(object *argl);

static PM_set
 *LR_mesh_set(char *name);

static void
 _LR_fill_coordw(double **elem, int ne, int *pist);

/*--------------------------------------------------------------------------*/

/*                   PSEUDO PLOT REQUEST INITIALIZATION                     */

/*--------------------------------------------------------------------------*/

/* LR_INIT_IO - initialize PSEUDO plot requests for the given package */

void LR_init_io(PA_package *pck, PFPreMap func)
   {PA_plot_request *pr, *lst;

    lst = pck->pseudo_pr;
    if (lst == NULL)
       return;

    for (pr = lst; pr != NULL; pr = pr->next)
        {LR_flatten_space(pr);
         PA_complete_premap(pr);

         (*func)(pr);};

    return;}

/*--------------------------------------------------------------------------*/

/*                       PLOT REQUEST ROUTINES                              */

/*--------------------------------------------------------------------------*/

/* LR_BUILD_MAPPING - build a graph from the specifications of the
 *                  - given plot request and return a pointer to it
 */

PM_mapping *LR_build_mapping(PA_plot_request *pr, double t)
   {

    return(PA_build_mapping(pr, _LR_get_n_set, t));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_BUILD_PSEUDO_MAPPING - build a graph from the specifications of the
 *                         - given plot request and return a pointer to it
 *                         - this is the package ppsor function for LR
 *                         - packages
 */

PM_mapping *LR_build_pseudo_mapping(PA_plot_request *pr, double t)
   {

    return(PA_build_mapping(pr, _LR_get_pseudo_set, t));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LR_GET_PSEUDO_SET - return a set made from the data associated
 *                    - with a PSEUDO EDIT variable given the plot request
 *                    - centering for the data, and
 *                    - return a PM_set with the
 *                    - requested data unless an error occurs then
 *                    - return NULL
 */

PM_set *_LR_get_pseudo_set(PA_plot_request *pr, char *name)
   {PM_set *set;
    PA_set_spec *pi;
    double *data, **elem;
    int ne, nde, centering, space, sz;
    int *maxes;

    centering = pr->centering;
    space     = pr->mesh_plot;
    set       = pr->data;
    set->name = CSTRSAVE(name);

    elem = (double **) set->elements;
    ne   = pr->size;
    nde  = 0;
    for (pi = pr->range; pi != NULL; pi = pi->next, nde++)
        {data = elem[nde];

/* resize the data array to the correct length - don't lie to others */
	 sz = SC_MEM_GET_N(double, data);
         if (sz != ne)
            CREMAKE(data, double, ne);

/* if this is a mesh plot worry about centering */
         if (space)
            elem[nde] = LR_map_centers(data, centering);
         else
            elem[nde] = data;};

    ne = SC_MEM_GET_N(double, elem[0]);

    maxes     = set->max_index;
    maxes[0]  = ne;

    pr->centering = N_CENT;
    PA_PR_RANGE_SIZE(pr, ne);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LR_GET_N_SET - extract and return an N dimensional subset of a
 *               - variable given the name of a variable in the PANACEA
 *               - data base, a centering for the data, and an array of
 *               - indexing information which specifies the subset
 *               - selection return a PM_set with the requested data
 *               - unless an error occurs then return NULL
 */

PM_set *_LR_get_n_set(PA_plot_request *pr, char *name)
   {int ne, nd, nde, space;
    int *maxes;
    double *data, **elem;
    C_array *arr;
    PM_centering centering;
    PM_set *set;
    PA_set_spec *s, *rp;

    rp        = pr->range;
    arr       = pr->domain_map;
    space     = pr->mesh_plot;
    centering = pr->centering;

/* compute the dimensionality of the set elements */
    for (nde = 0, s = rp; s != NULL; s = s->next, nde++);
    elem = CMAKE_N(double *, nde);

    for (nde = 0, s = rp; s != NULL; s = s->next, nde++)
        {data = PA_set_data(s->var_name, arr, &centering);

/* if this is a mesh plot worry about centering */
         if (space)
            elem[nde] = LR_map_centers(data, centering);
         else
            elem[nde] = data;};

    ne = SC_MEM_GET_N(double, elem[0]);

    nd       = 1;
    maxes    = CMAKE_N(int, nd);
    maxes[0] = ne;

    pr->centering = N_CENT;
    pr->size      = ne;

/* build the set */
    set                 = CMAKE(PM_set);
    set->name           = CSTRSAVE(name);
    set->element_type   = CSTRSAVE("double **");
    set->n_elements     = ne;
    set->dimension      = nd;
    set->dimension_elem = nde;
    set->max_index      = maxes;
    set->elements       = (byte *) elem;
    set->es_type        = CSTRSAVE("double *");
    set->extrema        = (byte *) CMAKE_N(double, 2*nde);
    set->scales         = (byte *) CMAKE_N(double, nde);
    set->opers          = PM_fp_opers;
    set->metric         = NULL;
    set->symmetry_type  = NULL;
    set->symmetry       = NULL;
    set->topology_type  = NULL;
    set->topology       = NULL;
    set->info_type      = NULL;
    set->info           = NULL;

    PM_find_extrema(set);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_SET_SET_LIMITS - set the limits for the set whose array of extremal
 *                   - indexes is passed in
 */

void LR_set_set_limits(int *maxes, int centering)
   {

    switch (centering)
       {case U_CENT :
        case N_CENT : maxes[0] = kmax;
                      maxes[1] = lmax;
                      break;
        case F_CENT :
        case Z_CENT : maxes[0] = kmax-1;
                      maxes[1] = lmax-1;
                      break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_MAP_CENTERS - map the range data to the apropriate centering */

double *LR_map_centers(double *data, int centering)
   {int i, j, k, l, n, nt, nz, ns, sz;
    double *dp, *pd, *fp, *fp1, *fp2, *fp3, *fp4;
    double val;

    dp = pd = data;
    sz = SC_MEM_GET_N(double, data);
    nt = sz/N_zones;
    ns = nt*kmax*lmax;

    switch (centering)
       {case F_CENT :
        case E_CENT :
        case U_CENT :
        case N_CENT : for (n = 0; n < nt; n++)
                          {for (l = 1; l <= lmax; l++)
                               for (k = 1; k <= kmax; k++)
                                   {i = NODE_OF(k, l);
                                    *dp++ = pd[i];};
                           pd += N_zones;};
                      break;

        case Z_CENT : nz  = kmax*lmax;
                      fp  = CMAKE_N(double, nz);
                      fp2 = fp;
                      fp1 = fp2 - kmax;
                      fp3 = fp2 - 1;
                      fp4 = fp1 - 1;

                      for (n = 0; n < nt; n++)
                          {for (l = 2; l <= lmax; l++)
                               for (k = 2; k <= kmax; k++)
                                   {j = NODE_OF(k, l);
                                    val = 0.25*pd[j];
                                    i   = j - j/kbnd;
                                    fp1[i] += val;
                                    fp2[i] += val;
                                    fp3[i] += val;
                                    fp4[i] += val;};

                          for (l = 2; l <= lmax; l++)
                              {j   = NODE_OF(2, l);
                               val = 0.25*pd[j];
                               i   = j - j/kbnd;
                               fp3[i] += val;
                               fp4[i] += val;};

                          for (l = 2; l <= lmax; l++)
                              {j   = NODE_OF(kmax, l);
                               val = 0.25*pd[j];
                               i   = j - j/kbnd;
                               fp2[i] += val;
                               fp1[i] += val;};

                          for (k = 2; k <= kmax; k++)
                              {j   = NODE_OF(k, 2);
                               val = 0.25*pd[j];
                               i   = j - j/kbnd;
                               fp4[i] += val;
                               fp1[i] += val;};

                          for (k = 2; k <= kmax; k++)
                              {j   = NODE_OF(k, lmax);
                               val = 0.25*pd[j];
                               i   = j - j/kbnd;
                               fp2[i] += val;
                               fp3[i] += val;};

                           j = NODE_OF(2, 2);
                           i = j - j/kbnd;
                           fp4[i] += 0.25*pd[j];

                           j = NODE_OF(2, lmax);
                           i = j - j/kbnd;
                           fp3[i] += 0.25*pd[j];

                           j = NODE_OF(kmax, 2);
                           i = j - j/kbnd;
                           fp1[i] += 0.25*pd[j];

                           j = NODE_OF(kmax, lmax);
                           i = j - j/kbnd;
                           fp2[i] += 0.25*pd[j];

                           for (k = 0; k < nz; k++)
                               *dp++ = fp[k];
                           pd += N_zones;};

                      SFREE_N(fp, nz);

                      break;};

    CREMAKE(data, double, ns);

    return(data);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_EXPAND_PLOT_DATA - enlarge the data arrays for the given plot
 *                     - this undoes the effect of the reductions done
 *                     - for pseudo plots so that their mappings are
 *                     - self-consistent
 */

void LR_expand_plot_data(PA_plot_request *pr, int ni)
   {int osz, nsz, nz, nde;
    double **elem, *data;
    PA_set_spec *rp;

    if (pr->data == NULL)
       return;

    if (pr->mesh_plot)
       nz = N_zones;
    else
       nz = 1;

    elem = (double **) pr->data->elements;
    nde  = 0;
    for (rp = pr->range; rp != NULL; rp = rp->next)
        {data = elem[nde];
	 osz  = SC_MEM_GET_N(double, data);
         nsz  = ni*nz;
         if (osz <= nsz)
            {elem[nde] = CREMAKE(data, double, nsz);
             PA_PR_RANGE_SIZE(pr, nsz);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_BUILD_DOMAIN - given the index map as a domain specification
 *                 - build a domain set and return a pointer to it
 *                 - return NULL on failure
 *                 - (grotrian plots are an example of things that 
 *                 - may fail here but will be correctly handled later)
 */

PM_set *LR_build_domain(char *base_name, C_array *arr, double t)
   {int i, n, nd, ne, nde, ist;
    int *maxes, *pm;
    double **elem;
    char dname[MAXLINE];
    PA_variable *pp;
    PA_set_index *dmap;
    PM_set *set;

    n    = arr->length;
    dmap = (PA_set_index *) arr->data;

/* build the domain name */
    snprintf(dname, MAXLINE, "{t=%.2e,%s", t, &base_name[1]);
    PD_process_set_name(dname);

    nd = 0;
    for (i = 0; i < n; i++)
        {if (dmap[i].val == -HUGE)
            {if (strcmp(dmap[i].name, "R") == 0)
                nd += 2;
             else
                {pp = PA_INQUIRE_VARIABLE(dmap[i].name);
                 if ((pp == NULL) || (PA_VARIABLE_PACKAGE(pp) == NULL))
                    continue;
                 else
                    nd++;};};};

    maxes = CMAKE_N(int, nd);
    pm    = maxes;
    for (i = 0; i < n; i++)
        {if (dmap[i].val == -HUGE)
            {if (strcmp(dmap[i].name, "R") == 0)
                {*pm++ = kmax;
		 *pm++ = lmax;}

             else
                {pp = PA_INQUIRE_VARIABLE(dmap[i].name);

                 if ((pp == NULL) || (PA_VARIABLE_PACKAGE(pp) == NULL))
                    continue;

                 else if (strcmp(PA_VARIABLE_NAME(pp), "iter") == 0)
                    *pm++ = PA_VARIABLE_SIZE(pp);

                 else
                    *pm++ = dmap[i].imax;};};};

    ne = 1L;
    for (i = 0; i < nd; i++)
        ne *= maxes[i];

    elem = CMAKE_N(double *, nd);

    nde = 0;
    ist = 1;
    for (i = 0; i < n; i++)
        {if (dmap[i].val == -HUGE)
            {if (strcmp(dmap[i].name, "R") == 0)
                {_LR_fill_coordw(elem, ne, &ist);
                 nde += 2;}

             else
                {pp = PA_INQUIRE_VARIABLE(dmap[i].name);
                 if ((pp == NULL) || (PA_VARIABLE_PACKAGE(pp) == NULL))
                    continue;
                 else
                    elem[nde++] = PA_fill_component(PA_VARIABLE_DATA(pp),
						    PA_VARIABLE_SIZE(pp),
						    &ist,
                                                    ne);};};};

    set                 = CMAKE(PM_set);
    set->name           = CSTRSAVE(dname);
    set->element_type   = CSTRSAVE("double **");
    set->n_elements     = ne;
    set->dimension      = nd;
    set->dimension_elem = nde;
    set->max_index      = maxes;
    set->elements       = (byte *) elem;
    set->es_type        = CSTRSAVE("double *");
    set->extrema        = (byte *) CMAKE_N(double, 2*nde);
    set->scales         = (byte *) CMAKE_N(double, nde);
    set->opers          = PM_fp_opers;
    set->metric         = NULL;
    set->symmetry_type  = NULL;
    set->symmetry       = NULL;
    set->topology_type  = NULL;
    set->topology       = NULL;
    set->info_type      = NULL;
    set->info           = NULL;

    PM_find_extrema(set);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _LR_FILL_COORDW - fill in compenent with coordinates */

static void _LR_fill_coordw(double **elem, int ne, int *pist)
   {int i, j, k, l, m, ist, nst;
    double *xc, *yc, *pxc, *pyc, conv;

    pxc = *elem++ = xc = CMAKE_N(double, ne);
    pyc = *elem++ = yc = CMAKE_N(double, ne);

    ist = *pist;
    nst = ist*kmax*lmax;
    for (m = 0; m < ne; m += nst)
        for (l = 1; l <= lmax; l++)
            for (k = 1; k <= kmax; k++)
                {i = NODE_OF(k, l);
                 for (j = 0; j < ist; j++)
                     {*pxc++ = rx[i];
                      *pyc++ = ry[i];};};

    conv = unit[CM]/convrsn[CM];
    PA_scale_array(xc, ne, conv);
    PA_scale_array(yc, ne, conv);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_FLATTEN_SPACE - flatten out any spatial dimensional
 *                  - references to a single index into a 1d array
 */

int LR_flatten_space(PA_plot_request *pr)
   {int i, j;
    double xv, yv;
    C_array *arr;
    PA_set_index *dmap;
    int n;

/* get the domain information */
    arr  = pr->domain_map;
    n    = arr->length;
    dmap = (PA_set_index *) arr->data;

    xv = 0.0;
    yv = 0.0;

    for (i = 0; i < n; i++)
        {if (strcmp("rx", dmap[i].name) == 0)
            xv = dmap[i].val;
         else if (strcmp("ry", dmap[i].name) == 0)
            yv = dmap[i].val;};

    for (i = 0; i < n; i++)
        if (strcmp(dmap[i].name, "rx") == 0)
           {if ((xv == -HUGE) && (yv == -HUGE))
               dmap[i].val = -HUGE;
            else if ((xv != -HUGE) && (yv != -HUGE))
               dmap[i].val = containsw(xv, yv);
            else
               PA_ERR(TRUE,
                      "NOT READY FOR MESH SUB-SETS - LR_FLATTEN_SPACE");
            dmap[i].name = CSTRSAVE("R");
            n--;
            for (j = i+1; j < n; j++)
                dmap[j] = dmap[j+1];};

/* fill in the maximum array index values and the index value */
    for (i = 0; i < n; i++)
        if (strcmp(dmap[i].name, "R") == 0)
           {dmap[i].imax = N_zones;
            if (dmap[i].val != -HUGE)
               dmap[i].index = dmap[i].val;
            else
               pr->mesh_plot = TRUE;};

    arr->length = n;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_FLATTEN_PLOTS - flatten out any spatial dimensional
 *                  - references to a single index into a 1d array
 *                  - for all plot requests
 */

int LR_flatten_plots(void)
   {PA_plot_request *pr;

    for (pr = plot_reqs; pr != NULL; pr = pr->next)
        LR_flatten_space(pr);

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                       INTERACTIVE PLOT ROUTINES                          */

/*--------------------------------------------------------------------------*/

/* LR_INT_PLOT - handle an interactive plot request at generation time */

object *LR_int_plot(PG_device *dev, char *rname, PM_centering centering,
		    int nde, double **elem)
   {int nd, ne, *maxes;
    char label[MAXLINE], dname[MAXLINE];
    PM_mapping *f;
    PM_set *range, *domain;
    PG_rendering pty;

    snprintf(dname, MAXLINE, "{rx, ry}");
    nd = 2;
    maxes    = CMAKE_N(int, nd);
    maxes[0] = kmax;
    maxes[1] = lmax;

    switch (nde)
       {case 1 :
	     pty = PLOT_CONTOUR;
	     break;
        case 2 :
	     pty = PLOT_VECTOR;
	     break;
        default :
	     break;};

    ne = SC_MEM_GET_N(double, elem[0]);

/* build the set */
    range                 = CMAKE(PM_set);
    range->name           = CSTRSAVE(rname);
    range->element_type   = CSTRSAVE("double **");
    range->n_elements     = ne;
    range->dimension      = nd;
    range->dimension_elem = nde;
    range->max_index      = maxes;
    range->elements       = (byte *) elem;
    range->es_type        = CSTRSAVE("double *");
    range->extrema        = (byte *) CMAKE_N(double, 2*nde);
    range->scales         = (byte *) CMAKE_N(double, nde);

    PM_find_extrema(range);

    PA_ERR((range == NULL),
           "CAN'T HANDLE %s - LR_INT_PLOT", rname);

    domain = LR_mesh_set(dname);
    snprintf(label, MAXLINE, "%s->%s", domain->name, range->name);
    f = PM_make_mapping(label, PM_LR_S, domain, range, centering, NULL);

    PA_put_mapping(dev, NULL, f, pty);
 
    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_VAR_PLOT - plot the specified variables in the implied or specified
 *             - manner for a 1d problem
 *             - use the global defaults if various data are not supplied
 *             - return a list of PG_graph objects
 *             -
 *             - FORM:
 *             -   (show <spec1> ... <specn>)
 *             -   <spec> = <plot_type>|<set>|("device" <device>)|("file" <PDBfile>)
 *             -   <set>  = <name>|(["domain"] <name> [<color> <width> <style>])
 *             -
 *             - the last plot_type wins as do the last domain and device
 *             - if no domain is specified, the mesh is taken as default
 *             - the domain should be specified first
 */

object *LR_var_plot(object *argl)
   {int color, style, id, space;
    int lcolor, lstyle;
    double width, lwidth;
    char *dev_name, *name, label[MAXLINE], dname[MAXLINE];
    object *obj, *sobj, *head, *gr, *val;
    C_array *arr;
    PM_centering centering;
    PM_mapping *f;
    PM_set *range, *domain;
    PG_rendering pty;
    PG_graph *g;
    PG_device *dev;
    PDBfile *file;
    PA_variable *pp;

    gr     = SS_null;
    g      = NULL;
    f      = NULL;
    color  = 1;
    style  = LINE_SOLID;
    width  = 0.0;
    id     = 'A';
    space  = TRUE;
    pty    = PLOT_CARTESIAN;
    range  = NULL;
    domain = NULL;
    file   = NULL;
    dev    = NULL;
    arr    = NULL;

    snprintf(dname, MAXLINE, "{t=%.2e, rx, ry}", t);

/* get the various specifications */
    while (SS_consp(argl))
       {sobj = SS_car(argl);
        argl = SS_cdr(argl);
        if (SS_consp(sobj))
           {head = SS_car(sobj);
            name = SS_get_string(head);

            if (strcmp(name, "domain") == 0)
               {val    = SS_eval(sobj);
                domain = SS_GET(PM_set, SS_cadr(val));
                arr    = SS_GET(C_array, SS_cddr(val));}

            else if (strcmp(name, "device") == 0)
               {head = SS_cadr(sobj);
                if (SX_DEVICEP(head))
                   dev = SS_GET(PG_device, head);
                else
                   SS_error("BAD DEVICE - LR_VAR_PLOT", head);}

            else if (strcmp(name, "file") == 0)
               {head = SS_cadr(sobj);
                if (SX_pdbfilep(head))
                   file = FILE_STREAM(PDBfile, head);
                else
                   SS_error("BAD FILE - LR_VAR_PLOT", head);}

            else
               {range = LR_get_set(name, &centering, arr, space);
                if (range == NULL)
                   {snprintf(SX_err, MAXLINE,
			     "CAN'T HANDLE %s - LR_VAR_PLOT",
			     name);
                    SS_error(SX_err, sobj);};
                lcolor = color;
                lstyle = style;
                lwidth = width;
                sobj   = SS_cdr(sobj);
                if (SS_consp(sobj))
                   SX_GET_INTEGER_FROM_LIST(lcolor, sobj,
                                            "BAD COLOR - LR_VAR_PLOT");
                if (SS_consp(sobj))
                   SX_GET_FLOAT_FROM_LIST(lwidth, sobj,
                                          "BAD WIDTH - LR_VAR_PLOT");
                if (SS_consp(sobj))
                   SX_GET_INTEGER_FROM_LIST(lstyle, sobj,
                                            "BAD STYLE - LR_VAR_PLOT");
                if (domain == NULL)
                   domain = LR_mesh_set(dname);

                snprintf(label, MAXLINE, "%s->%s", domain->name, range->name);
                f = PM_make_mapping(label, PM_LR_S, domain, range, centering, f);};}

        else if (SS_integerp(sobj) || SS_floatp(sobj))
           pty = SS_INTEGER_VALUE(sobj);

        else
           {name = SS_get_string(sobj);
            pp   = PA_INQUIRE_VARIABLE(name);
            if (pp == NULL)
               {val  = SS_eval(sobj);
                head = SS_car(val);
                name = SS_get_string(head);
                if (strcmp(name, "domain") == 0)
                   {space  = SS_INTEGER_VALUE(SS_cadr(val));
                    domain = SS_GET(PM_set, SS_caddr(val));
                    arr    = SS_GET(C_array, SS_cdddr(val));}
                else
                   SS_error("BAD OBJECT IN CONTEXT - LR_VAR_PLOT", sobj);}
            else
               {range = LR_get_set(name, &centering, arr, space);
                if (range == NULL)
                   {snprintf(SX_err, MAXLINE,
                            "CAN'T HANDLE %s - LR_VAR_PLOT",
                            name);
                    SS_error(SX_err, sobj);};

/* check the domain */
                if (domain == NULL)
                   domain = LR_mesh_set(dname);

                snprintf(label, MAXLINE, "%s->%s", domain->name, range->name);
                f = PM_make_mapping(label, PM_LR_S, domain, range, centering, f);};};};

/* check the device global default device */
    if (dev == NULL)
       {if (global_dev == NULL)
           {dev_name = getenv("DISPLAY");
            global_dev = PG_make_device(dev_name, "MONOCHROME", "LR");
            PG_open_device(global_dev, 0.0, 0.0, 0.0, 0.0);};
        dev = global_dev;};

    PA_put_mapping(dev, file, f, pty);

/* objectify the graphs */
    if (g != NULL)
       {g  = PG_make_graph_from_mapping(f, NULL, NULL, id++, g);

	g->info_type = SC_PCONS_P_S;
	g->info = (byte *) PG_set_tds_info((pcons *) g->info,
					   PLOT_CONTOUR, CARTESIAN_2D, 
					   lstyle, lcolor, 10, 1.0,
					   lwidth, 0.0, 0.0, 0.0,
					   HUGE);
        
        gr = SX_mk_graph(g);}

    else
       gr = SS_f;
 
    return(gr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_MESH_SET - make a set out of the problem mesh and
 *             - return a pointer to it
 */

static PM_set *LR_mesh_set(char *name)
   {PM_set *set;
    double *xc, *yc, *pxc, *pyc;
    int nitems, i, k, l;
    double conv;

    nitems = kmax*lmax;
    pxc = xc = CMAKE_N(double, nitems);
    pyc = yc = CMAKE_N(double, nitems);
    for (l = 1; l <= lmax; l++)
        for (k = 1; k <= kmax; k++)
            {i = NODE_OF(k, l);
             *pxc++ = rx[i];
             *pyc++ = ry[i];};
    conv = unit[CM]/convrsn[CM];
    PA_scale_array(xc, nitems, conv);
    PA_scale_array(yc, nitems, conv);
    set = PM_make_set(name, SC_DOUBLE_S, FALSE,
                            2, kmax, lmax,
                            2, xc, yc);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_GET_SET - extract and return a subset of a given the name of a
 *            - variable in the PANACEA data base, a centering for the
 *            - data, and an array of indexing information which specifies
 *            - the subset selection
 *            - return a PM_set with the requested data
 *            - unless an error occurs then return NULL
 */

PM_set *LR_get_set(char *name, PM_centering *pcent, C_array *arr, int space)
   {int i, j, k, l;
    long nitems;
    double *data, *dp;
    PM_centering centering;
    PM_set *set;

    data = PA_set_data(name, arr, pcent);

    centering = *pcent;

/* if this is a mesh plot worry about centering */
    if (space)
       {switch (centering)
           {case N_CENT : dp = data;
                          for (l = 1; l <= lmax; l++)
                              for (k = 1; k <= kmax; k++)
                                  {i = NODE_OF(k, l);
                                   *dp++ = data[i];};
                          set = PM_make_set(name, SC_DOUBLE_S, FALSE,
                                            2, kmax, lmax,
                                            1, data);
                          break;
            case Z_CENT : dp = data;
                          for (l = 2; l <= lmax; l++)
                              for (k = 2; k <= kmax; k++)
                                  {j = NODE_OF(k, l);
                                   *dp++ = data[j];};
                          set = PM_make_set(name, SC_DOUBLE_S, FALSE,
                                            2, kmax-1, lmax-1,
                                            1, data);
                          break;
            case F_CENT :
            case U_CENT :
            default     : return(NULL);};}

/* otherwise build a suitable 1d set 
 * NOTE: when multidimensional sub-spaces are allowed this will change
 */
    else
       {nitems = SC_MEM_GET_N(double, data);
        set = PM_make_set(name, SC_DOUBLE_S, FALSE,
			  1, nitems, 1, data);};

    return(set);}

/*--------------------------------------------------------------------------*/

/*                       INTERACTIVE QUERY ROUTINES                         */

/*--------------------------------------------------------------------------*/

/* LR_GET_TIME_DATA - return a list of t, dt, and cycle */

object *LR_get_time_data(void)
   {double tconv, tc, dtc;

    tconv = convrsn[SEC]/unit[SEC];
    tc    = tconv*t;
    dtc   = tconv*dt;
    return(SS_make_list(SC_DOUBLE_I, &tc,
                        SC_DOUBLE_I, &dtc,
                        SC_INTEGER_I, &cycle,
			0));}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_GET_DATA - report the number of zones and the current cycle */

int LR_get_data(double *pnz, double *pnc, double *pt, double *pdt,
	        char **prs, char **ped, char **ppp, char **pgf)
   {

    *pnz = (double) global_swtch[12];
    *pnc = (double) cycle;

    *pt  = global_param[1];
    *pdt = global_param[4]*(global_param[3] - global_param[2]);

    *prs = global_name[2];
    *ped = global_name[3];
    *ppp = global_name[4];
    *pgf = global_name[5];

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_GET_MESH_DATA - return a list of N_zones, frz, lrz, frn, lrn
 *                  - and if kmax and lmax
 */

object *LR_get_mesh_data(void)
   {object *th;

    th = SS_make_list(SC_INTEGER_I, &N_zones,
		      SC_INTEGER_I, &frz,
		      SC_INTEGER_I, &lrz,
		      SC_INTEGER_I, &frn,
		      SC_INTEGER_I, &lrn,
		      SC_INTEGER_I, &kmax,
		      SC_INTEGER_I, &lmax,
		      0);

    return(th);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_GET_DOMAIN_ZONE - return the zone index from the spatial part of the 
 *                    - domain set specification of the given plot request 
 *                    - if it refers to a single zone
 *                    - return -1 otherwise
 */

int LR_get_domain_zone(PA_plot_request *pr)
   {int i, n;
    PA_set_index *dmap;
    C_array *arr;

    arr  = pr->domain_map;
    dmap = (PA_set_index *) arr->data;
    n    = arr->length;

    for (i = 0; i < n; i++)
        if (strcmp(dmap[i].name, "R") == 0)
           return((dmap[i].val == -HUGE) ? -1 : dmap[i].index);

    return(0);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LR_DEF_DOMAIN - define a domain set which can be used to build
 *               - range sets for LR plot requests
 *               - start off with two cases:
 *               -
 *               - 1) selecting all non-spatial indices for a mesh plot
 *               -
 *               - 2) selecting all indices but one for a line plot
 *               -
 *               - Forms:
 *               -
 *               -    (domain rx ry nu 10.4)
 *               -    (domain rx 0.23 ry 0.31 nu)
 *               -    (domain rx 0.23 ry nu 10.4)
 *               -
 *               - The order must match that associated with the variables
 *               - which will be plotted on this domain.  Otherwise the
 *               - variables themselves would have to be specified to
 *               - supply that information.
 */

object *LR_def_domain(object *argl)
   {PM_set *dom;
    object *obj, *ret;
    int i, n, nmax, space;
    PA_set_index *dmap;
    C_array *arr;
    char dname[MAXLINE], numval[20];

    nmax = SS_length(argl);
    dmap = CMAKE_N(PA_set_index, nmax);

    arr         = CMAKE(C_array);
    arr->type   = PA_SET_INDEX_P_S;
    arr->length = nmax;
    arr->data   = (byte *) dmap;

/* make a list of names and indices suitable for C */
    i = 0;
    while (SS_consp(argl))
       {obj  = SS_car(argl);
        argl = SS_cdr(argl);
        dmap[i].name = CSTRSAVE(SS_get_string(obj));
        if (SS_consp(argl))
           {obj = SS_car(argl);
            if (SS_numbp(obj))
               {dmap[i].val = SS_FLOAT_VALUE(obj);
                argl        = SS_cdr(argl);}
            else
               dmap[i].val = -HUGE;}
        else
           dmap[i].val = -HUGE;

        i++;};

    n = i;

    dname[0] = '\0';
    for (i = 0; i < n; i++)
        {SC_strcat(dname, MAXLINE, dmap[i].name);
         if (dmap[i].val != -HUGE)
            {SC_strcat(dname, MAXLINE, "=");
             snprintf(numval, 20, "%.2e", (double) dmap[i].val);
             SC_strcat(dname, MAXLINE, numval);};

         SC_strcat(dname, MAXLINE, ",");};
    SC_LAST_CHAR(dname) = '\0';
    SC_strcat(dname, MAXLINE, "}");

    dom = LR_build_domain(dname, arr, t);

/* encapsulate the domain map indices */
    arr         = CMAKE(C_array);
    arr->type   = PA_SET_INDEX_S;
    arr->length = n;
    arr->data   = (byte *) dmap;

/* now make the domain map and the domain arrays */
    ret = SS_make_list(SC_STRING_I, "domain",
                       SC_INTEGER_I, &space,
                       SS_OBJECT_I, SX_mk_set(dom),
                       SS_OBJECT_I, SX_mk_C_array(arr),
                       0);
/*
    ret = SS_mk_cons(SS_mk_string("domain"),
                     SS_mk_cons(SS_mk_integer(space),
                                SS_mk_cons(SS_mk_guest(SS_mk_object(dom, G_SET),
                                                       TRUE),
                                           SS_mk_guest(SS_mk_object(arr,
                                                                    G_NUM_ARRAY),
                                                       TRUE))));
*/
    return(ret);}

/*--------------------------------------------------------------------------*/

/*                                RANDOMS                                   */

/*--------------------------------------------------------------------------*/

/* LR_PLOT_ACCEPT - filter out plot requests which don't involve the
 *                - components in the argument list
 */

int LR_plot_accept(PA_plot_request *pr, ...)
   {va_list arg_ptr;
    char *s;
    PA_set_spec *rp;

    va_start(arg_ptr, pr);

    while ((s = va_arg(arg_ptr, char *)))
       {if (s == NULL)
           break;

        for (rp = pr->range; rp != NULL; rp = rp->next)
            {if (strcmp(s, rp->var_name) == 0)
                {va_end(arg_ptr);
                 return(TRUE);};};};

    va_end(arg_ptr);

    return(FALSE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
