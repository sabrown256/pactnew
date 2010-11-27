/*
 * PADUMP.C - handle the dumps and edits
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"

#include "panacea_int.h"

struct s_PA_domain_spec
   {char *name;
    C_array *map;
    PA_set_spec *time;
    PM_set *data;};

typedef struct s_PA_domain_spec PA_domain_spec;

int
 max_domains,
 n_domains,
 N_time_plots,
 N_unique_variables;

double
 *t_data;

char
 *PAN_EDIT = NULL,
 *PAN_PLOT_REQ = NULL;

PA_domain_spec
 *domain_list;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_BEGIN_PREMAP - start the pre-processing of plot requests */

static void _PA_begin_premap(PA_plot_request *pr)
   {int first;
    PM_centering centering;
    PA_set_spec *pi, *s;
    PA_variable *pp;
    char dname[MAXLINE], *t;

    pi = pr->range;

/* figure out the centering of this set */
    pp        = NULL;
    centering = U_CENT;
    for (first = TRUE, s = pi; s != NULL; s = s->next)
        {t  = s->var_name;
         pp = PA_inquire_variable(t);
         PA_ERR((pp == NULL),
                "VARIABLE NOT %s NOT IN DATABASE - _PA_BEGIN_PREMAP" ,t);
         if (first)
            {centering = PA_VARIABLE_CENTERING(pp);
             first     = FALSE;}
         else
            PA_ERR((centering != PA_VARIABLE_CENTERING(pp)),
                   "VARIABLE %s HAS INCONSISTENT CENTERING - _PA_BEGIN_PREMAP",
                   t);};

    if (pp != NULL)
       {pr->centering = centering;
	pr->conv      = PA_VARIABLE_EXT_UNIT(pp)/PA_VARIABLE_INT_UNIT(pp);

/* compute the domain information */
	pr->domain_map       = PA_get_domain_info(pr, dname, MAXLINE);
	pr->base_domain_name = SC_strsavef(dname, "char*:_PA_BEGIN_PREMAP:dname");

/* if a PSEUDO variable, splice it out into the package list */
	if (PA_VARIABLE_CLASS(pp) == PSEUDO)
	   {pr->status = PSEUDO;
	    _PA_splice_out_request(pr, PA_VARIABLE_PACKAGE(pp)->pseudo_pr);
	    PA_VARIABLE_PACKAGE(pp)->pseudo_pr = pr;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_SPLICE_OUT_REQUEST - splice a plot_request out of the global list
 *                        - and cons it onto the given list of plot_requests
 *                        - return a pointer the spliced out request
 */

PA_plot_request *_PA_splice_out_request(PA_plot_request *pr,
					PA_plot_request *lst)
   {PA_plot_request *ths, *prv, *nxt;

    prv = NULL;
    for (ths = plot_reqs; ths != NULL; ths = nxt)
        {nxt = ths->next;
         if (ths == pr)
            {if (prv == NULL)
                plot_reqs = nxt;
             else
                prv->next = nxt;
             break;};
         prv = ths;};

    pr->next = lst;

    return(pr);}

/*--------------------------------------------------------------------------*/

/*                            TIME PLOT ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PA_TIME_PLOT_PRED - return TRUE iff the given plot_request is a time plot */

static int _PA_time_plot_pred(PA_plot_request *pr)
   {int rv;
    double *v, start;
    PA_set_spec *pi;
    PA_variable *pp;

    pi    = pr->domain;
    v     = pi->values;
    start = (v == NULL) ? -HUGE : v[0];

    if (strcmp(pi->var_name, "t") == 0)
       {if (start != -HUGE)
           return(FALSE);
        pi = pi->next;};

    rv = TRUE;
    for (; pi != NULL; pi = pi->next)
        {pp = PA_inquire_variable(pi->var_name);
         if (pp == NULL)
            continue;

	 v     = pi->values;
	 start = (v == NULL) ? -HUGE : v[0];
	 if (start == -HUGE)
            {rv = FALSE;
	     break;};};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_TIME_PLOTP - return TRUE iff the given plot_request is a time plot */

static int _PA_time_plotp(PA_plot_request *pr)
   {int rv;
    PFTimePlotP hook;

    hook = PA_GET_FUNCTION(PFTimePlotP, "time-plot-predicate");

    rv = (hook == NULL) ? _PA_time_plot_pred(pr) : (*hook)(pr);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INIT_TIME_PLOT - initialize a single time plot */

static int _PA_init_time_plot(PA_plot_request *pr,
			      char **members, char **labels,
			      int itu, int flag)
   {char bf[MAXLINE], dname[MAXLINE], *rname, *dtype;
    C_array *arr;
    PA_variable *pp;

    arr   = PA_get_domain_info(pr, dname, MAXLINE);
    rname = pr->range->var_name;

    pr->domain_map       = arr;
    pr->base_domain_name = SC_strsavef(dname,
				       "char*:_PA_INIT_TIME_PLOT:dname");
    pr->range_name       = SC_strsavef(rname,
				       "char*:_PA_INIT_TIME_PLOT:rname");
    pr->str_index        = itu;
    pr->size             = 1L;
    pr->offset           = 0L;
    pr->stride           = 1L;

    pp = PA_inquire_variable(pr->range_name);

/* if it's not in PA_variable_tab skip it or has no data pointer attached */
    if ((pp == NULL) && (pr->data == NULL))
       return(itu);

/* this is a way of handling the unit problem in the post-processor */
    if (pp != NULL)
       {dtype = pp->desc->type;
        pr->data_type = SC_type_id(dtype, FALSE);
	pr->conv      = PA_VARIABLE_EXT_UNIT(pp)/PA_VARIABLE_INT_UNIT(pp);
	if (PA_VARIABLE_CLASS(pp) == PSEUDO)
	   pr->status = PSEUDO;};

    labels[itu-1] = pr->text;
    snprintf(bf, MAXLINE, "y%d", itu);
    members[itu] = SC_strsavef(bf, "char*:_PA_INIT_TIME_PLOT:bf");

/* if not a PSEUDO variable we're done */
    if (pr->status == PSEUDO)

/* splice the PSEUDO variable time plots into the package pseudo lists
 * this must be done before the package initializers are called because
 * they are supposed to have the opportunity to work over the PSEUDO
 * plot requests before anything is plotted
 */
       {pp = PA_inquire_variable(pr->range_name);
	PA_ERR((pp == NULL),
	       "VARIABLE %s GOT LOST - _PA_INIT_TIME_PLOTS",
	       pr->range_name);

	if (flag)
	   {_PA_splice_out_request(pr, PA_VARIABLE_PACKAGE(pp)->pseudo_pr);
	    PA_VARIABLE_PACKAGE(pp)->pseudo_pr = pr;};};

    itu++;

    return(itu);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INIT_TIME_PLOTS - initialize the time plots from the list of
 *                     - plot_requests
 *                     - this is split to allow the package initializers
 *                     - adjust the plot requests based on information
 *                     - from the data base or from source files
 *                     - the latter half is _PA_SCAN_TIME_PLOTS
 */

static void _PA_init_time_plots(char *ppname)
   {int itu, i, n_dom;
    char bf[MAXLINE], **labels, **members;
    PA_plot_request *pr, *nxt;
    PA_package *pck;

/* count the number of time plots */
    for (pr = plot_reqs; pr != NULL; pr = pr->next)
        {if (_PA_time_plotp(pr))
            {N_time_plots++;
             pr->time_plot = TRUE;};};             

/* count the number of time plots on the packages (restart has this) */
    for (pck = Packages; pck != NULL; pck = pck->next)
        for (pr = pck->pseudo_pr; pr != NULL; pr = pr->next)
	    {if (_PA_time_plotp(pr))
	        {N_time_plots++;
		 pr->time_plot = TRUE;};};             

    if (N_time_plots <= 0)
       return;

    if (ppname != NULL)
       {PA_pp_file = PD_open(ppname, "w");
	PRINT(stdout, "Post processor file %s opened\n", ppname);

	PD_set_offset(PA_pp_file, PA_get_default_offset());}
    else
       PA_pp_file = NULL;

    PA_current_pp_cycle = -1;

/* allow for t in the data stripe */
    n_dom = 1;
    labels  = FMAKE_N(char *, N_time_plots, "_PA_INIT_TIME_PLOTS:labels");
    members = FMAKE_N(char *, N_time_plots+1, "_PA_INIT_TIME_PLOTS:members");

    itu = n_dom;
    snprintf(bf, MAXLINE, "t");
    members[0] = SC_strsavef(bf, "char*:_PA_INIT_TIME_PLOTS:bf");

/* finish the curve arrays for the packages */
    for (pck = Packages; pck != NULL; pck = pck->next)
        for (pr = pck->pseudo_pr; pr != NULL; pr = pr->next)
	    {if (pr->time_plot)
	        itu = _PA_init_time_plot(pr, members, labels, itu, FALSE);};

/* finish the curve arrays and fill in the name and number arrays */
    for (pr = plot_reqs; pr != NULL; pr = nxt)
        {nxt = pr->next;
         if (pr->time_plot)
	    itu = _PA_init_time_plot(pr, members, labels, itu, TRUE);};

    if (PA_pp_file != NULL)
       PA_th_def_rec(PA_pp_file, "time_data", "time_stripe",
                     itu, members, labels);

    SFREE(labels);

    for (i = 0; i < N_time_plots; i++)
        SFREE(members[i]);
    SFREE(members);

    if (N_time_plots > 0)
       {N_unique_variables = itu;
        t_data = FMAKE_N(double, N_unique_variables,
                         "_PA_INIT_TIME_PLOTS:t_data");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_ARRAY_REF_I - given an array of unknown type, an index, and
 *                 - an integer type specification
 *                 - return a double containing the value referenced
 */

static double _PA_array_ref_i(void *vr, long indx, int type)
   {double d;

    d = 0.0;

    if (SC_is_type_num(type) == TRUE)
       SC_convert_id(SC_DOUBLE_I, &d, 0, 1, type, vr, indx, 1, 1, FALSE);

    else
       PA_ERR(TRUE,
	      "CAN'T PLOT DATA TYPE - _PA_DUMP_TIME_PLOTS");

    return(d);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_DUMP_TIME_PLOTS - dump the values at this time/cycle for all of the
 *                     - time plot requests from the "plot" command
 */

static void _PA_dump_time_plots(double tc, double dtc, int cycle)
   {int i;
    double d;
    void *pv;
    PA_plot_request *pr;

    if (PA_pp_file == NULL)
       return;

    if (PA_current_pp_cycle == -1)
       PD_write(PA_pp_file, "first-cycle", SC_INT_S, &cycle);

    PA_current_pp_cycle = cycle;

    t_data[0] = tc*convrsn[SEC]/unit[SEC];

/* write the time plot data */
    for (pr = plot_reqs; pr != NULL; pr = pr->next)
        {if ((pr->time_plot) && (pr->status != PSEUDO))
            {pv = (void *) pr->data;
             if (pv == NULL)
                PA_CONNECT(pv, pr->range_name, FALSE);

             if (pv == NULL)
                continue;

             d = _PA_array_ref_i(pv, pr->data_index, pr->data_type);

             t_data[pr->str_index] = pr->conv*d;};};

/* if appropriate append to the time history data */
    if (N_unique_variables > 0)
       PA_th_write(PA_pp_file, "time_data", "time_stripe", cycle, 1, t_data);

/* zero out the time data stripe */
    for (i = 0; i < N_unique_variables; i++)
        t_data[i] = 0.0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DUMP_TIME - return TRUE iff the given time matches the time
 *              - specifications of the given plot request
 */

int PA_dump_time(PA_set_spec *pi, double tc, double dtc, int cycle)
   {int ic, istop, istart, istep, rv;
    double lt, stop, start, step, tn;
    double *v;

    rv = TRUE;

    v = pi->values;
    if (v == NULL)
       {start = -HUGE;
	stop  =  HUGE;
	step  = -SMALL;}

    else if (pi->n_values == 1)
       {start = v[0];
	stop  = v[0];
	step  = v[0];}

    else
       {start = v[0];
	stop  = v[1];
	step  = v[2];};

/* any time specification had better be first on the list */
    if (strcmp("t", pi->var_name) == 0)
       {if ((start == -HUGE) && (stop == HUGE) && (step == -SMALL))
           rv = TRUE;

/* if the time is wrong, process the next graph request */
        else if ((tc + dtc < start) || (stop < tc))
           rv = FALSE;

	else
	   {tn = tc + dtc;
	    for (lt = start; lt <= tn; lt += step)
	        if ((tc <= lt) && (lt < tn))
		   break;

	    if (lt > tn)
	       rv = FALSE;};}

/* or any cycle specification had better be first on the list */
    else if (strcmp("cycle", pi->var_name) == 0)
       {if (stop > ((double) INT_MAX))
	   istop = INT_MAX;
	else
	   istop  = stop;

	if (start < 0)
	   istart = 0;
	else
	   istart = start;

	if (step < 1)
           istep = 1;
	else
           istep = (int) (step + 0.5);

/* if the time is wrong, process the next graph request */
        if ((cycle < istart) || (cycle > istop))
           rv = FALSE;

	else
	   {for (ic = istart; ic <= istop; ic += istep)
	        if (ic >= cycle)
		   break;

	    if (ic != cycle)
	       rv = FALSE;};};

    return(rv);}

/*--------------------------------------------------------------------------*/

/*                             MESH PLOT ROUTINES                           */

/*--------------------------------------------------------------------------*/

/* _PA_DUMP_MAPPINGS - dump all of the PM_mappings requested/implied by
 *                   - the "graph" commands
 */

static void _PA_dump_mappings(double tc, double dtc, int cycle)
   {int i;
    char *name;
    PA_plot_request *pr;
    PM_set *s;
    PM_mapping *f;
    C_array *arr;

    if (_PA.build_domain != NULL)
       {for (i = 0; i < n_domains; i++)
            {name = domain_list[i].name;
             arr  = domain_list[i].map;

             if (!PA_dump_time(domain_list[i].time, tc, dtc, cycle))
                continue;

             s = (*_PA.build_domain)(name, arr, tc);
             if (s == NULL)
                continue;

             PA_put_set(PA_pva_file, s);
             PM_rel_set(s, FALSE);};};

    if (_PA.build_mapping != NULL)
       {for (pr = plot_reqs; pr != NULL; pr = pr->next)
            {if ((pr->status == PSEUDO) || pr->time_plot)
                continue;

             if (!PA_dump_time(pr->domain, tc, dtc, cycle))
                continue;

             f = (*_PA.build_mapping)(pr, tc);
             PA_put_mapping(NULL, PA_pva_file, f, PLOT_NONE);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_DUMP_PACKAGE_MAPPINGS - dump the mappings for this package
 *                           - and release their space
 *                           - this is called after the package
 *                           - post-processor
 *                           - it is aimed primarily at PSEUDO, EDIT
 *                           - variables
 */

void _PA_dump_package_mappings(PA_package *pck, double t, double dt,
			       int cycle)
   {PA_plot_request *pr, *lst;
    PM_mapping *f;
    PFPkgPpsor build_mapping;

    lst = pck->pseudo_pr;
    if (lst == NULL)
       return;

    build_mapping = pck->ppsor;
    if (build_mapping == NULL)
       return;

    for (pr = lst; pr != NULL; pr = pr->next)
        {if (!PA_dump_time(pr->domain, t, dt, cycle))
            continue;

         if (pr->data == NULL)
            continue;

         f = build_mapping(pr, t);
         if (f != NULL)
            {PM_find_extrema(f->range);
             PA_put_mapping(NULL, PA_pva_file, f, PLOT_NONE);};

         PM_rel_set(pr->data, FALSE);
         pr->data = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_DOMAIN_INFO - given a PA_set_spec compute the domain name
 *                    - and the map indexes
 *                    - allocate and return a C_array containing the
 *                    - map indexes
 */

C_array *PA_get_domain_info(PA_plot_request *pr, char *dname, int nc)
   {int i, n;
    char *pname, **ps;
    long size, indx, ndx;
    double *v, start;
    C_array *arr;
    PA_set_spec *pi, *pd;
    PA_set_index *dmap;
    PA_variable *pp;

    pd = PA_non_time_domain(pr);

/* construct the domain name */
    snprintf(dname, nc, "{");
    for (n = 0, pi = pd; pi != NULL; pi = pi->next, n++)
        {v     = pi->values;
	 start = (v == NULL) ? -HUGE : v[0];

	 strcat(dname, pi->var_name);
	 if (pi->function != NULL)
            SC_vstrcat(dname, nc, "= %s,", pi->function);
            
         else if (start != -HUGE)
            {if (pi->text != NULL)
                SC_vstrcat(dname, MAXLINE, "=%s,", pi->text);
             else
                SC_vstrcat(dname, MAXLINE, "=%.2e,", start);};};

    SC_LAST_CHAR(dname) = '\0';
    SC_strcat(dname, nc, "}");

    if (domain_list == NULL)
       {domain_list = FMAKE_N(PA_domain_spec, 10,
                              "PA_GET_DOMAIN_INFO:domain_list");
        n_domains   = 0;
        max_domains = 10;};

/* check to see if this domain is known already */
    for (i = 0; i < n_domains; i++)
        if (strcmp(dname, domain_list[i].name) == 0)
           break;

/* if so return the map */
    if (i < n_domains)
       {arr = domain_list[i].map;
        return(arr);};

    dmap = FMAKE_N(PA_set_index, n, "PA_GET_DOMAIN_INFO:dmap");

/* encapsulate the domain map indices */
    arr = PM_make_array(PA_SET_INDEX_P_S, n, dmap);

/* time plots will be handled separately - so don't record them in the
 * domain_list
 */
    if (!pr->time_plot)
       {domain_list[i].name = PD_process_set_name(SC_strsavef(dname,
							      "char*:PA_GET_DOMAIN_INFO:name"));
        domain_list[i].map  = arr;
        domain_list[i].time = pr->domain;
        n_domains++;

        if (n_domains >= max_domains)
           {max_domains += 10;
            REMAKE_N(domain_list, PA_domain_spec, max_domains);};};

/* initialize the map */
    size = 1L;
    for (i = 0, pi = pd; pi != NULL; pi = pi->next, i++)
        {v     = pi->values;
	 start = (v == NULL) ? -HUGE : v[0];

	 pname = pi->var_name;
         pp    = PA_inquire_variable(pname);

         dmap[i].name = SC_strsavef(pname,
                         "char*:PA_GET_DOMAIN_INFO:name");
         if (pi->text != NULL)
            {ps  = (char **) PA_VARIABLE_DATA(pp);
             ndx = PA_VARIABLE_SIZE(pp);
             if (strcmp(PA_VARIABLE_TYPE_S(pp), SC_STRING_S) == 0)
                {for (indx = 0; indx < ndx; indx++)
                     {if (strcmp(pi->text, ps[indx]) == 0)
                         {dmap[i].val = indx;
                          break;};};

                 if (indx == ndx)
                    dmap[i].val = 0L;};

             dmap[i].imax  = PA_VARIABLE_SIZE(pp);
             dmap[i].index = (int) dmap[i].val;}

         else if (SC_intstrp(pname, 10))
             {dmap[i].index = SC_stoi(pname);}

         else if (SC_fltstrp(pname))
             {dmap[i].index = (int) SC_stof(pname);}

         else
            {dmap[i].val = start;

             if ((pp == NULL) || (PA_VARIABLE_PACKAGE(pp) == NULL))
                {if (dmap[i].val == -HUGE)
                    dmap[i].index = NO_VALUE_I;}
             else
                {dmap[i].imax = PA_VARIABLE_SIZE(pp);

                 if (dmap[i].val == -HUGE)
                    {dmap[i].index = NO_VALUE_I;
                     size *= PA_VARIABLE_SIZE(pp);}
                 else if (PA_VARIABLE_DATA(pp) != NULL)
		    {if (dmap[i].imax < 1)
		        dmap[i].index = 0;
		     else
		        dmap[i].index = PA_find_index(PA_VARIABLE_DATA(pp),
						      dmap[i].val,
						      dmap[i].imax);}
                 else
                    dmap[i].index = PA_DELAY;};};};

    pr->size = size;

    return(arr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_RANGE_INFO - compute a name, number of data elements,
 *                   - offset of first element, and stride through the
 *                   - data array from a PA_set_spec
 */

void PA_get_range_info(PA_plot_request *pr)
   {long size, offs, stride, lsize, loffs, lstride;
    PA_variable *pp;
    PA_set_spec *ps, *pi;
    C_array *arr;
    char *vr, rname[MAXLINE];

    if (pr == NULL)
       return;

    ps  = pr->range;
    arr = pr->domain_map;
    if ((ps == NULL) || (arr == NULL))
       return;

    size   = 0L;
    offs   = 0L;
    stride = 1L;
    if (ps->next == NULL)
       {strcpy(rname, ps->var_name);
        vr = ps->var_name;
        pp = PA_inquire_variable(vr);
        if (pp != NULL)
           {PA_sub_select(pp, arr, &size, &offs, &stride);
            if (pr->time_plot)
               pr->data_index = offs;
            else
               {pr->size       = size;
                pr->offset     = offs;
                pr->stride     = stride;};}

        else
           PA_ERR((!(pr->time_plot) || (pr->data_index == -INT_MAX)),
                  "VARIABLE %s NOT IN DATABASE - PA_GET_RANGE_INFO", vr);}

    else
       {strcpy(rname, "{");
        for (pi = ps; pi != NULL; pi = pi->next)
            {vr = pi->var_name; 
	     SC_vstrcat(rname, MAXLINE, "%s,", vr);
             pp = PA_inquire_variable(vr);
             PA_ERR((pp == NULL),
                    "VARIABLE %s NOT IN DATABASE - PA_GET_RANGE_INFO", vr);
             lsize   = size;
             loffs   = offs;
             lstride = stride;
             PA_sub_select(pp, arr, &size, &offs, &stride);
             if (pi != ps)
                {PA_ERR(((lsize != size) ||
                         (loffs != offs) ||
                         (lstride != stride)),
                        "INCOMENSURATE RANGE COMPONENT %s - PA_GET_RANGE_INFO",
                        rname);};};

        SC_LAST_CHAR(rname) = '\0';
        SC_strcat(rname, MAXLINE, "}");};

    pr->range_name = SC_strsavef(rname, "char*:PA_GET_RANGE_INFO:rname");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INIT_PSEUDO_SET - initialize a set for the data associated
 *                     - with a PSEUDO EDIT variable given the plot request
 *                     - centering for the data, and
 *                     - attach a PM_set to the plot request
 *                     - ready to recieve the requested data 
 */

static void _PA_init_pseudo_set(PA_plot_request *pr)
   {int ne, nd, nde;
    int *maxes;
    double **elem;
    char *t;
    PM_centering centering;
    PM_set *set;
    PA_set_spec *pi;
    PA_variable *pp;

/* figure out the centering of this set */
    centering = U_CENT;
    for (nde = 0, pi = pr->range; pi != NULL; pi = pi->next, nde++)
        {t  = pi->var_name;
         pp = PA_inquire_variable(t);
         PA_ERR((pp == NULL),
                "VARIABLE NOT %s NOT IN DATABASE - _PA_INIT_PSEUDO_SET" ,t);
         if (nde == 0)
            centering = PA_VARIABLE_CENTERING(pp);
         else
            PA_ERR((centering != PA_VARIABLE_CENTERING(pp)),
                   "VARIABLE %s HAS INCONSISTENT CENTERING - _PA_INIT_PSEUDO_SET",
                   t);};

    pr->centering = centering;

    ne = pr->size;

/* this is not quite right, but shouldn't be too wrong either */
    nd = 1;
    maxes = FMAKE_N(int, nd, "_PA_INIT_PSEUDO_SET:maxes");
    maxes[0] = ne;

/* compute the dimensionality of the set elements */
    for (nde = 0, pi = pr->range; pi != NULL; pi = pi->next, nde++);
    elem = FMAKE_N(double *, nde, "_PA_INIT_PSEUDO_SET:elem");

    for (nde = 0, pi = pr->range; pi != NULL; pi = pi->next, nde++)
        {elem[nde] = FMAKE_N(double, ne, "_PA_INIT_PSEUDO_SET:elem");};

    set = PM_mk_set(pr->range_name, SC_DOUBLE_S, FALSE,
		    ne, nd, nde, maxes, elem,
		    NULL, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL,
		    NULL);

    pr->data = set;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_NON_TIME_DOMAIN - extract the part of the domain that is independent
 *                    - of time from the given nplot request
 */

PA_set_spec *PA_non_time_domain(PA_plot_request *pr)
   {PA_set_spec *pd;

    pd = pr->domain;
    if (strcmp(pd->var_name, "t") == 0)
       pd = pd->next;

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_BUILD_MAPPING - given the plot request
 *                  - and functions which know about the specifics
 *                  - of the packages (provided by the packages) as far as
 *                  - extracting the data from the database is concerned
 *                  - build a mapping and return a pointer to it
 *                  - return NULL if not successful or if a time plot is
 *                  - encountered
 *                  - NOTE: it is crucial that this routine fail
 *                  -       gracefully on a plot request that it does
 *                  -       not know how to handle, because the packages
 *                  -       may have routines to handle these specially!!!
 */

PM_mapping *PA_build_mapping(PA_plot_request *pr, PM_set *(*build_ran)(PA_plot_request *pr, char *name),
			     double t)
   {PM_mapping *f;
    PM_set *range;
    char label[MAXLINE], dname[MAXLINE], *bname, *rname;

    if (pr->time_plot)
       return(NULL);

    rname = pr->range_name;
    bname = pr->base_domain_name;

    range = build_ran(pr, rname);
    PA_ERR((range == NULL),
           "CAN'T HANDLE %s - PA_BUILD_GRAPH", rname);

    snprintf(dname, MAXLINE, "{t=%.2e,%s", t, &bname[1]);
    snprintf(label, MAXLINE, "%s->%s", dname, rname);
    f = PM_make_mapping(label, PM_LR_S, NULL, range, pr->centering, NULL);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_COPY_SUB_SELECT - copy the data from the sub-selection into the
 *                     - target array
 */

static long _PA_copy_sub_select(int did, void *d, long od,
				int sid, void *s, long os,
				unsigned long *sshp, unsigned long *sstr,
				int dm)
   {int bpi;
    long i, n, ss, ni, nb, m;
    char *ps;

    ss = sstr[0];
    n  = sshp[0];

    if (dm == 0)
       {SC_convert_id(did, d, od, 1, sid, s, os, ss, n, FALSE);
        ni = n;}

    else
       {ni  = 0L;
	bpi = SC_type_size_i(sid);
	nb  = bpi*ss;
	ps  = (char *) s;
        for (i = 0L; i < n; i++, ps += nb)
            {m   = _PA_copy_sub_select(did, d, ni, sid, s, os,
				       sshp+1, sstr+1, dm-1);
             ni += m;};};

    return(ni);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SET_DATA - extract data from a PANACEA variable, convert it to an
 *             - array of doubles, and return it
 *             - the array of doubles is newly allocated here
 *             - the centering of the data is passed back through the
 *             - argument PCENT
 *             - a C_array of qualifiers, ARR, is passed in to specify 
 *             - sub-selections of the data
 */

double *PA_set_data(char *name, C_array *arr, PM_centering *pcent)
   {int nd, id;
    unsigned long nitems, dims, offset, stride;
    unsigned long strides[50], maxes[50];
    char *type;
    double conv;
    double *data;
    void *ps;
    PA_variable *pp;

    pp = PA_inquire_variable(name);
    if (pp == NULL)
       return(NULL);

    *pcent = PA_VARIABLE_CENTERING(pp);

/* interactive plot requests among others will have arr = NULL */
    PA_general_select(pp, arr, &nitems, &dims, &offset, strides, maxes);

    data = FMAKE_N(double, nitems, "PA_SET_DATA:data");

    stride = strides[0];
    type   = PA_VARIABLE_TYPE_S(pp);
    id     = SC_type_id(type, FALSE);

    nd = dims - 1;
        
    if (SC_is_type_num(id) == TRUE)
       {ps = PA_VARIABLE_DATA(pp);
        _PA_copy_sub_select(SC_DOUBLE_I, data, 0,
			    id, ps, offset, maxes, strides, nd);};

    conv = PA_VARIABLE_EXT_UNIT(pp)/PA_VARIABLE_INT_UNIT(pp);
    PA_scale_array(data, nitems, conv);

    return(data);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_GET_DATA - extract a subset of data from a PANACEA variable as
 *              - defined by a number of items, NITEMS, an offset, OFFSET,
 *              - from the beginning of the variable, and a stride, STRIDE,
 *              - through the variable, VR
 *              - put the data in the pre-allocated space D
 *              - the data is converted to doubles!!
 *              - return TRUE iff successful
 */

int _PA_get_data(double *d, char *vr, long ni, long offset, long stride)
   {int id;
    char *type;
    void *pv;
    double conv_fac;
    PA_variable *pp;

    pp = PA_inquire_variable(vr);
    if (pp == NULL)
       return(FALSE);

    type = PA_VARIABLE_TYPE_S(pp);
    id   = SC_type_id(type, FALSE);

    if (SC_is_type_num(id) == TRUE)
       {pv = PA_VARIABLE_DATA(pp);
	SC_convert_id(SC_DOUBLE_I, d, 0, 1, id, pv, offset, stride, ni, FALSE);}

    else
       PA_ERR(TRUE,
              "CAN'T HANDLE TYPE %s - _PA_GET_DATA", type);

    conv_fac = PA_VARIABLE_EXT_UNIT(pp)/PA_VARIABLE_INT_UNIT(pp);
    PA_scale_array(d, ni, conv_fac);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_FILL_COMPONENT - contruct and return one component of a set element
 *                   - using the given data array and the parameters
 *                   - which describe its position order
 */

double *PA_fill_component(double *data, int len, int *pist, int ne)
   {int i, j, k, ist, nst;
    double *pc, *comp, val;

    comp = FMAKE_N(double, ne, "PA_FILL_COMPONENT:comp");

    ist = *pist;
    nst = ist*len;
    pc  = comp;

/* some variables don't have data - treat them as identity arrays */
    if (data == NULL)
       {for (k = 0; k < ne; k += nst)
            for (i = 0; i < len; i++)
                {val = i + 1.0;
                 for (j = 0; j < ist; j++)
                     *pc++ = val;};}

    else
       {for (k = 0; k < ne; k += nst)
            for (i = 0; i < len; i++)
                {val = data[i];
                 for (j = 0; j < ist; j++)
                     *pc++ = val;};};

    *pist = nst;

    return(comp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PUT_SET - handle the process of writing out a set for PANACEA */

void PA_put_set(PDBfile *file, PM_set *s)
   {

    if (s != NULL)
       {s->opers = NULL;

        PA_ERR(!PD_put_set(file, s),
               "PD_PUT_SET FAILED %s - PA_PUT_MAPPING", PD_err);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_CONNECT_DOMAIN - check the domain_list for a domain to connect
 *                    - to this mapping
 */

static void _PA_connect_domain(PM_mapping *f)
   {int i;
    char dname[MAXLINE];

    PA_ERR((domain_list == NULL),
           "NO DOMAINS TO CONNECT - _PA_CONNECT_DOMAIN");

/* extract the domain name from the mapping name */
    strcpy(dname, f->name);
    PD_process_set_name(dname);

    for (i = 0; i < n_domains; i++)
        {if (strcmp(dname, domain_list[i].name) == 0)
            {f->domain = domain_list[i].data;
             return;};};

    PA_ERR(TRUE,
           "NO DOMAIN MATCHES %s - _PA_CONNECT_DOMAIN", dname);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PUT_PICTURE - handle the process of putting out a picture in PANACEA
 *                - picture will go to file iff file is non-NULL
 *                - picture will go to display iff dev is non-NULL
 */

void PA_put_picture(PG_device *dev, PDBfile *file, PM_mapping *f,
		    PG_rendering ptyp, int bndp, int cbnd, int sbnd,
		    double wbnd, int mshp, int cmsh, int smsh, double wmsh)
   {int flag;
    PM_mapping *pf;

    if (file != NULL)

/* disconnect any function pointers or undefined structs/members */
       {for (pf = f; pf != NULL; pf = pf->next)
            {if (pf->domain != NULL)
                pf->domain->opers = NULL;
             if (pf->range != NULL)
                pf->range->opers = NULL;};

        PA_ERR(!PD_put_mapping(file, f, _PA.imap++),
               "PD_PUT_MAPPING FAILED %s - PA_PUT_PICTURE", PD_err);}
           
/* draw the mappings */
    if (dev != NULL)
       {flag = FALSE;
        if ((f != NULL) && (f->domain == NULL))
           {_PA_connect_domain(f);
            flag = TRUE;};

	PG_draw_picture(dev, f, ptyp,
			bndp, cbnd, sbnd, wbnd,
			mshp, cmsh, smsh, wmsh);

        if (flag)
           f->domain = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PUT_MAPPING - handle the process of putting out a mapping in PANACEA
 *                - mapping will go to file iff file is non-NULL
 *                - mapping will go to display iff dev is non-NULL
 */

void PA_put_mapping(PG_device *dev, PDBfile *file, PM_mapping *f,
		    PG_rendering ptyp)
   {int clr;

    clr = (dev == NULL) ? 0 : dev->WHITE;

    PA_put_picture(dev, file, f, ptyp,
		   TRUE, clr, LINE_SOLID, 1.2,
		   FALSE, clr, LINE_SOLID, 0.0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_ALLOCATE_MAPPING_SPACE - allocate space for the I/O data which
 *                            - will be put out this cycle
 *                            - this is called just before the package
 *                            - main entry
 */

void _PA_allocate_mapping_space(PA_package *pck)
   {PA_plot_request *pr;

    for (pr = pck->pseudo_pr; pr != NULL; pr = pr->next)
        if ((!pr->time_plot) && (pr->allocate_data))
           _PA_init_pseudo_set(pr);

    return;}

/*--------------------------------------------------------------------------*/

/*                     DEFAULT MAPPING BUILDERS                             */

/*--------------------------------------------------------------------------*/

/* _PA_GET_N_SET - extract and return an N dimensional subset of a
 *               - variable given the name of a variable in the PANACEA
 *               - data base, a centering for the data, and an array of
 *               - indexing information which specifies the subset
 *               - selection return a PM_set with the requested data
 *               - unless an error occurs then return NULL
 */

static PM_set *_PA_get_n_set(PA_plot_request *pr, char *name)
   {int ne, nd, nde;
    int *maxes;
    double **elem;
    C_array *arr;
    PM_centering centering;
    PM_set *set;
    PA_set_spec *s, *rp;

    rp        = pr->range;
    arr       = pr->domain_map;
    centering = pr->centering;

/* compute the dimensionality of the set elements */
    for (nde = 0, s = rp; s != NULL; s = s->next, nde++);
    elem = FMAKE_N(double *, nde, "_PA_GET_N_SET:elem");

/* get the element components */
    for (nde = 0, s = rp; s != NULL; s = s->next, nde++)
        elem[nde] = PA_set_data(s->var_name, arr, &centering);

    ne = SC_arrlen(elem[0])/SIZEOF(arr->type);

    nd       = 1;
    maxes    = FMAKE_N(int, nd, "_PA_GET_N_SET:maxes");
    maxes[0] = ne;
    pr->size = ne;

    set = PM_mk_set(name, SC_DOUBLE_S, FALSE,
		    ne, nd, nde, maxes, elem,
		    NULL, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL,
		    NULL);

    return(set);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_BUILD_MAPPING - build a graph from the specifications of the
 *                   - given plot request and return a pointer to it
 */

static PM_mapping *_PA_build_mapping(PA_plot_request *pr, double t)
   {PM_mapping *f;

    f = PA_build_mapping(pr, _PA_get_n_set, t);

    return(f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_BUILD_DOMAIN - given the index map as a domain specification
 *                  - build a domain set and return a pointer to it
 *                  - return NULL on failure
 *                  - (grotrian plots are an example of things that 
 *                  - may fail here but will be correctly handled later)
 */

static PM_set *_PA_build_domain(char *base_name, C_array *arr, double t)
   {int i, n, nd, ne, nde, ist;
    int *maxes, *pm;
    double **elem;
    char dname[MAXLINE];
    PA_variable *pp;
    PA_set_index *dmap;
    PM_set *set;

    PM_ARRAY_CONTENTS(arr, PA_set_index, n, NULL, dmap);

/* build the domain name */
    snprintf(dname, MAXLINE, "{t=%.2e,%s", t, &base_name[1]);
    PD_process_set_name(dname);

    nd = 0;
    for (i = 0; i < n; i++)
        {if (dmap[i].val == -HUGE)
            {pp = PA_inquire_variable(dmap[i].name);
             if ((pp == NULL) || (PA_VARIABLE_PACKAGE(pp) == NULL))
                continue;
             else
                nd++;};};

    maxes = FMAKE_N(int, nd, "_PA_BUILD_DOMAIN:maxes");
    pm    = maxes;
    for (i = 0; i < n; i++)
        {if (dmap[i].val == -HUGE)
            {pp = PA_inquire_variable(dmap[i].name);

             if ((pp == NULL) || (PA_VARIABLE_PACKAGE(pp) == NULL))
                continue;
             else if (strcmp(PA_VARIABLE_NAME(pp), "iter") == 0)
                *pm++ = PA_VARIABLE_SIZE(pp);
             else
                *pm++ = dmap[i].imax;};};

    ne = 1L;
    for (i = 0; i < nd; i++)
        ne *= maxes[i];

    elem = FMAKE_N(double *, nd, "_PA_BUILD_DOMAIN:elem");

    nde = 0;
    ist = 1;
    for (i = 0; i < n; i++)
        {if (dmap[i].val == -HUGE)
            {pp = PA_inquire_variable(dmap[i].name);
             if ((pp == NULL) || (PA_VARIABLE_PACKAGE(pp) == NULL))
                continue;
             else
                elem[nde++] = PA_fill_component(PA_VARIABLE_DATA(pp),
                                                PA_VARIABLE_SIZE(pp),
                                                &ist, ne);};};

    set = PM_mk_set(dname, SC_DOUBLE_S, FALSE,
		    ne, nd, nde, maxes, elem,
		    NULL, NULL,
		    NULL, NULL, NULL, NULL, NULL, NULL,
		    NULL);

    return(set);}

/*--------------------------------------------------------------------------*/

/*                           HIGH LEVEL ROUTINES                            */

/*--------------------------------------------------------------------------*/

/* _PA_INIT_MAPPINGS - check all nplot requests for ranges composed of
 *                   - PSEUDO variables and install those in the
 *                   - package pseudo_pr list
 *                   - this is split to allow the package initializers
 *                   - adjust the plot requests based on information
 *                   - from the data base or from source files
 *                   - the latter half is PA_SCAN_MAPPINGS
 */

static void _PA_init_mappings(char *ppname)
   {PA_plot_request *pr, *nxt;
    PFPreMap hook;

    PAN_EDIT     = SC_strsavef("edit_variable",
                               "char*:_PA_INIT_MAPPINGS:edit_var");
    PAN_PLOT_REQ = SC_strsavef("PA_plot_reqest",
                               "char*:_PA_INIT_MAPPINGS:plot_req");
    SC_permanent(PAN_EDIT);
    SC_permanent(PAN_PLOT_REQ);

    _PA_init_time_plots(ppname);

    hook = PA_GET_FUNCTION(PFPreMap, "preprocess-mapping");
    if (hook == NULL)
       hook = _PA_begin_premap;

    for (pr = plot_reqs; pr != NULL; pr = nxt)
        {nxt = pr->next;
         if (pr->time_plot)
            continue;
         (*hook)(pr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INIT_PP - initialize the post processor file
 *             - count the time plots and allocate the stripe
 *             - this is split to allow the package initializers
 *             - adjust the plot requests based on information
 *             - from the data base or from source files
 *             - the latter half is PA_SCAN_PP
 */

void _PA_init_pp(char *ppname, char *gfname)
   {int n_maps, ok;
    PA_plot_request *pr;
    PA_package *pck;
    PFNeedPVA hook;

    _PA.build_domain = PA_GET_FUNCTION(PFBuildDom, "build_domain");
    if (_PA.build_domain == NULL)
       _PA.build_domain = _PA_build_domain;

    _PA.build_mapping = PA_GET_FUNCTION(PFBuildMap, "build_mapping");
    if (_PA.build_mapping == NULL)
       _PA.build_mapping = _PA_build_mapping;

    N_time_plots       = 0;
    N_unique_variables = 0;
    n_maps = 0;

/* count the number of plot requests */
    for (pr = plot_reqs; pr != NULL; pr = pr->next, n_maps++);

/* count the number of plot requests on the packages (restart has this) */
    for (pck = Packages; pck != NULL; pck = pck->next)
        for (pr = pck->pseudo_pr; pr != NULL; pr = pr->next, n_maps++);

    if (n_maps > 0)
       {_PA_init_mappings(ppname);

        n_maps -= N_time_plots;
        if ((gfname != NULL) && (n_maps > 0))
	   {hook = PA_GET_FUNCTION(PFNeedPVA, "need-pva-file");
	    if (hook != NULL)
	       ok = (*hook)();
	    else
	       ok = TRUE;

	    if (ok)
	       {PA_pva_file = PA_open(gfname, "w", FALSE);
		PA_ERR(!PD_def_mapping(PA_pva_file),
		       "CAN`T DEFINE MAPPINGS - _PA_INIT_PP");
		PRINT(stdout, "PVA file %s opened\n", gfname);};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_SCAN_PP - scan the mappings and complete the data structures for
 *             - efficient I/O
 *             -
 *             - check the ranges of all plot requests
 *             - and fill in any information that could not be
 *             - computes prior to the package initializers pass
 *             - over the plot request domains
 *             -
 *             - this is split to allow the package initializers
 *             - adjust the plot requests based on information
 *             - from the data base or from source files
 *             - the prior half is _PA_INIT_PP
 */

void _PA_scan_pp(void)
   {PA_plot_request *pr;

    for (pr = plot_reqs; pr != NULL; pr = pr->next)
        PA_get_range_info(pr);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DUMP_PP - dump the current time step's data to the pp file */

void PA_dump_pp(double t, double dt, int cycle)
   {double conv_fac, tc, dtc;

    conv_fac = convrsn[SEC]/unit[SEC];
    tc       = t*conv_fac;
    dtc      = dt*conv_fac;

    if (N_time_plots > 0)
       _PA_dump_time_plots(tc, dtc, cycle);

    if (plot_reqs != NULL)
       _PA_dump_mappings(tc, dtc, cycle);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
