/*
 * PASRC.C - handle source files for PANACEA
 *         - source files for PANACEA are special PDB files
 *         - written by source file builder for this purpose
 *         - individual PANACEA package make requests to provide the
 *         - source information and these routines know nothing about
 *         - the packages and their purposes
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

#include "pasrc.h"

PA_src_variable
 **SV_List;

int
 N_Variables;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INIT_SOURCES - scan the iv_spec_lst for source files and initialize the
 *                  - various buffers for each of the source variables
 */

void _PA_init_sources(double t, double dt)
   {int j, N_sf, n_vars, count;
    int n_times;
    char *name, *token, *iv_file, **files, *s, *tok;
    char title[MAXLINE], title_name[MAXLINE];
    double *times;
    PA_iv_specification *ivs;
    PDBfile *fp;

/* count up the number of source files to be managed and the
 * number of source variables
 */
    for (N_sf = 0, ivs = iv_spec_lst;
         ivs != NULL;
         ivs = ivs->next, N_sf++);

    files = FMAKE_N(char *, N_sf, "_PA_INIT_SOURCES:files");
    N_Variables = 0;
    N_sf = 0;
    for (ivs = iv_spec_lst; ivs != NULL; ivs = ivs->next)
        {iv_file = ivs->file;
         if (iv_file == NULL)
            continue;
         if (*(iv_file) == '\0')
            continue;
         for (j = 0; j < N_sf; j++)
             if (strcmp(iv_file, files[j]) == 0)
                break;
         if ((j < N_sf) && (N_sf > 0))
            continue;
         files[N_sf++] = iv_file;

         fp = PD_open(iv_file, "r");
         PA_ERR((fp == NULL),
                "MISSING SOURCE FILE - %s", iv_file);
         PA_ERR(!PD_read(fp, "N_Variables", &n_vars),
                "N_VARIABLES MISSING FROM %s", iv_file);
         N_Variables += n_vars;
         PD_close(fp);};

/* loop over all source files and make a list of PA_src_variables */
    SV_List = FMAKE_N(PA_src_variable *, N_Variables,
                      "_PA_INIT_SOURCES:List");
    n_vars  = 0;
    for (j = 0; j < N_sf; j++)
        {fp = PD_open(files[j], "r");

/* NOTE: this has N_Variables greater than the number actually referenced
 *       this give more efficiency but one has to be careful when
 *       checking SV_List for NULL entries
 */
         for (ivs = iv_spec_lst; ivs != NULL; ivs = ivs->next)
             {if (ivs->file == NULL)
                 continue;

              count = 0;
              while (TRUE)
                 {snprintf(title_name, MAXLINE, "src%d", count);
                  if (PD_read(fp, title_name, title) == FALSE)
                     break;

                  tok  = SC_strtok(title, "|", s);
                  name = SC_strsavef(tok, "char*:_PA_INIT_SOURCES:tok");
                  if ((ivs->type != 'v') ||
                      (strcmp(name, ivs->name) == 0))
                     {n_times = SC_stoi(SC_strtok(NULL, "|", s));
                      token   = SC_strtok(NULL, "|", s);
                      times   = FMAKE_N(double, n_times,
                                        "_PA_INIT_SOURCES:times");
                      PA_ERR(!PD_read(fp, token, times),
                             "CAN'T READ %s IN %s - INIT-SOURCES",
                             token, files[j]);
                      PA_scale_array(times, n_times, unit[SEC]);

/* install this source variable in the array, SV_List and have the
 * entry in iv_spec_lst point to it
 */
                      SV_List[n_vars] = _PA_mk_src_variable(name,
                                                            count, n_times,
                                                            times, fp);
                      ivs->index = n_vars;

                      SV_List[n_vars]->interpolate = ivs->interpolate;

/* initialize the queue buffer for this PA_src_variable */
                      _PA_init_queue(SV_List[n_vars], t, dt);

/* the iv_spec needs to know about the size of the source variable
 * for inits in PA_CONNECT
 */
                      ivs->num = SV_List[n_vars]->size;

                      n_vars++;};
                  count++;};};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_SOURCE - given the name of a source variable return a pointer
 *               - to the source variable data if it is there
 *               - and return NULL if not
 *               - remember the index and use the start_flag to reset it
 *               - so that multiple sources with the same name may be used
 */

PA_src_variable *PA_get_source(char *s, int start_flag)
   {int i;
    PA_src_variable *svp, *rv;
    char *pb, bf[MAXLINE];

    i = _PA.isrc;

    if (start_flag)
       i = 0;

/* NOTE: N_Variables is greater than the number actually referenced
 *       this give more efficiency but one has to be careful when
 *       checking SV_List for NULL entries
 */
    for (rv = NULL; i < N_Variables; i++)
        {svp = SV_List[i];
         if (svp == NULL)
            continue;

	 strcpy(bf, svp->name);
	 pb = SC_firsttok(bf, "()");

         if (strcmp(s, pb) == 0)
	    {rv = svp;
	     break;};};

    _PA.isrc = i;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_IV_SOURCE - this routine gets the named PA_iv_specification */
 
PA_iv_specification *PA_get_iv_source(char *name)
   {PA_iv_specification *rv;

    rv = PA_find_iv_source(name, 0);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_FIND_IV_SOURCE - this routine gets the named PA_iv_specification */
 
PA_iv_specification *PA_find_iv_source(char *name, int off)
   {PA_iv_specification *sp;
    double *data;

/* check for the named iv_spec in the given zone/region */
    for (sp = iv_spec_lst; sp != NULL; sp = sp->next)
        {data = sp->data;
         if (data == NULL)
            continue;

         if (!strcmp(sp->name, name))
            {sp->index = (int) data[off];
             return(sp);};};

    return(NULL);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INIT_QUEUE - initialize a queue of data for
 *                - PA_src_variables so that interpolations come off nicely
 *                - and source file access is minimized
 */

void _PA_init_queue(PA_src_variable *svp, double t, double dt)
   {syment *desc;
    char *token, title[MAXLINE], var_name[MAXLINE], *s;
    double conv;
    double *times, **pd, *qtimes;
    int check, indx, iv, i, nv, n_times, var_indx;
    int psz, ssz, fsz;
    PA_variable *pp;
    PDBfile *fp;

    fp       = svp->file;
    var_indx = svp->var_index;
    indx     = svp->index;
    n_times  = svp->n_times;
    times    = svp->times;
    qtimes   = svp->queue_times;
    pd       = svp->queue;
    
/* lookup up the PA_variable that this PA_src_variable is controlling
 * and set the conversion from CGS units to INTERNAL units
 */
    strcpy(var_name, svp->name);
    token = SC_strtok(var_name, "(", s);

    pp = PA_inquire_variable(token);
    if (pp != NULL)
       {conv    = svp->conv_fact = PA_VARIABLE_INT_UNIT(pp);
        svp->pp = pp;}
    else
       {conv    = svp->conv_fact = 1.0;
        svp->pp = NULL;};

/* find the index of the first source time less than the
 * current problem time
 */
    indx = PA_find_index(times, t, n_times);

/* load the queue */
    desc = NULL;
    if (indx == 0)
       {snprintf(title, MAXLINE, "src%d:%d", var_indx, indx);
        pd[0] = pd[1] = pd[2] = (double *) _PA_pdb_read(fp, title, &desc,
                                                        NULL);
        ssz = desc->number;
        PA_scale_array(pd[2], ssz, conv);
        qtimes[0] = -2.0e100;
        qtimes[1] = -1.0e100;
        qtimes[2] = times[indx];
        indx++;
        if (n_times != 1)
           {snprintf(title, MAXLINE, "src%d:%d", var_indx, indx);
            pd[3] = (double *) _PA_pdb_read(fp, title, &desc, NULL);
            ssz = desc->number;
            PA_scale_array(pd[3], ssz, conv);
            qtimes[3] = times[indx];
            indx++;}
        else
           {qtimes[3] = 1.0e100;
            pd[3] = pd[2];};}
    else
       {nv = min(4, n_times - indx + 2);
        if (indx > 1)
           {iv = 0;
            check = FALSE;
            indx -= 2;}
        else
           {iv = 1;
            check = TRUE;
            indx--;};

/* load data into the iv'th thru nv'th places in the queue */
        for (i = iv; i < nv; i++)
            {snprintf(title, MAXLINE, "src%d:%d", var_indx, indx);
             pd[i] = (double *) _PA_pdb_read(fp, title, &desc, NULL);
             PA_ERR((pd[i] == NULL),
                    "MISSING SOURCE DATA %s - _PA_INIT_QUEUE", title);
             ssz = desc->number;
             PA_scale_array(pd[i], ssz, conv);
             qtimes[i] = times[indx];
             indx++;};

/* if at the end equate the missing pointers to the last valid one */
        for (; i < 4; i++)
            {pd[i] = pd[nv-1];
             qtimes[i] = qtimes[i-1] + 1.0e100;};

/* check the first place in the queue one last time */
        if (check)
           {pd[0] = pd[1];
            qtimes[0] = qtimes[1] - 1.0e100;};};

    if (pp != NULL)

/* replace the variable's syment with the file syment */
       {if (PA_VARIABLE_DESC(pp) != NULL)
	   _PD_rl_syment(PA_VARIABLE_DESC(pp));
	PA_VARIABLE_DESC(pp) = desc;

/* force consistency by recomputing the variable size */
	ssz = _PA_dims_to_list(PA_VARIABLE_DESC(pp)->dimensions,
			       PA_VARIABLE_DIMS(pp));
	PA_VARIABLE_SIZE(pp) = ssz;

/* reset the PA_src_variable index to the next one to read */
	svp->index = indx;

/* get the size out of the desc */
	svp->size = ssz;

/* set the PA_variable's size to match (also check for consistency)
 * the source's size
 */
	psz = PA_VARIABLE_SIZE(pp);
        fsz = PA_VARIABLE_DESC(pp)->number;
        PA_ERR((ssz == 0),
               "SOURCE VARIABLE %s HAS ZERO SIZE - _PA_INIT_QUEUE",
               token);
        PA_ERR((((psz != 0) && (psz != ssz)) ||
                ((fsz != 0) && (fsz != ssz))),
               "INCONSISTENT SOURCE VARIABLE SIZE %s - _PA_INIT_QUEUE",
               token);
        PA_VARIABLE_SIZE(pp)         = ssz;
        PA_VARIABLE_DESC(pp)->number = ssz;};

/* initialize the panacea variable by interpolating from this
 * initialized queue
 */
/* do this only if the application requests a connection
    PA_CONNECT(v, var_name, FALSE);
    PA_interp_src(v, svp, 0, ssz, t, dt);
*/
    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_STEP_QUEUE - buffer a new entry into the queue for the specified
 *                - PA_src_variable
 */

void _PA_step_queue(PA_src_variable *svp, double t)
   {syment *desc;
    int indx, i, n_times, var_indx, ssz;
    double *times, **pd, *qtimes, conv;
    PDBfile *fp;
    char title[MAXLINE];

    var_indx = svp->var_index;
    indx     = svp->index;
    n_times  = svp->n_times;
    times    = svp->times;
    qtimes   = svp->queue_times;
    conv     = svp->conv_fact;

    fp = svp->file;
    pd = svp->queue;

/* loop thru the source data until it is exhausted or until
 * the appropriate time is found
 */
    while (t > qtimes[2])

/* check on whether or not to free the first slot in the queue
 * if indx is less than 4 the first slot in the queue is a pointer to
 * the second slot in the queue
 */
      {if (indx > 3)
          {SFREE_N(pd[0], svp->size);};

/* shift everybody up the queue by one */
       for (i = 1; i < 4; i++)
           {pd[i-1] = pd[i];
            qtimes[i-1] = qtimes[i];};

/* read the next entry into the queue */
       if (indx < n_times)
          {snprintf(title, MAXLINE, "src%d:%d", var_indx, indx);
           pd[3] = (double *) _PA_pdb_read(fp, title, &desc, NULL);
           ssz = desc->number;
           PA_scale_array(pd[3], ssz, conv);
           qtimes[3] = times[indx];
           indx++;}
       else
          {indx++;
           pd[3] = pd[2];
           qtimes[3] = qtimes[2] + 1.0e100;};};

/* reset the PA_src_variable index to the next one to read */
    svp->index = indx;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SOURCE_VARIABLES - this routine sets the various PANACEA variables
 *                     - which have been tied to data from source files
 */
 
void PA_source_variables(double t, double dt)
   {int i;
    PA_src_variable *svp;
    PA_variable *pp;

/* check all source specifications which are
 * tied to variables in the data base
 *
 * NOTE: N_Variables is greater than the number actually referenced
 *       this gives more efficiency but one has to be careful when
 *       checking SV_List for NULL entries
 */
    for (i = 0; i < N_Variables; i++)
        {svp = SV_List[i];
         if (svp == NULL)
            continue;

         pp  = PA_inquire_variable(svp->name);

/* if the variable is missing or unconnected skip it */
         if ((pp == NULL) || (PA_VARIABLE_DATA(pp) == NULL))
            continue;

         PA_interp_src(PA_VARIABLE_DATA(pp),
		       svp, 0, PA_VARIABLE_SIZE(pp), t, dt);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INTERP_SRC - interpolate a variable from PA_src_variable data
 *               - into variable v from v[ni] thru v[nf-1]
 *               - for now this is a quadratic interpolation only scheme
 */

void PA_interp_src(void *v, PA_src_variable *svp, int ni, int nf,
		   double t, double dt)
   {int id;
    char *type;
    double *times;
    double t0, t1, t2, t3;

/* make sure that the queue is up to date with this time */
    _PA_step_queue(svp, t);

    times = svp->queue_times;
    t0    = times[0];
    t1    = times[1];
    t2    = times[2];
    t3    = times[3];

    type = svp->pp->desc->type;
    id   = SC_type_id(type, FALSE);

    if (SC_is_type_prim(id) == TRUE)
       {if (PA_interp_src_fnc[id] != NULL)
	   PA_interp_src_fnc[id](v, svp, ni, nf, t, dt, t0, t1, t2, t3);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INTR_SPEC - find the interpolated source value from the appropriate
 *              - PA_iv_specification 
 *              - sp is a pointer to the initial value specification
 *              - t is the problem time
 *              - val is a value to return when the problem time is outside
 *              - of the iv_spec data
 *              - off is the offset past the descriptive information in the
 *              - data array
 */

double PA_intr_spec(PA_iv_specification *sp, double t, double val, long off)
   {double tin, st1, st2, sv1, sv2, *svp;
    int sptmax, spt, spx;

/* find the appropriate range in the specification */
    spt = sp->index;
    spx = sp->num - off;
    if ((0 <= spt) && (spt < spx))
       {svp    = sp->data + spt + off;
	sptmax = 0;
        if (t >= *svp)
           {for (sptmax = sp->num - 1; spt < sptmax; spt += 2, svp += 2)
                {st1 = svp[0];
                 st2 = svp[2];
                 if ((st1 <= t) && (t < st2))
                    break;};};

	if (spt >= sptmax)
           tin = 0.0;
 
/* interpolate the source value */
	else
	   {sv1 = svp[1];
	    sv2 = svp[3];
	    st1 = svp[0];
	    st2 = svp[2];
	    tin = ((t - st1)*sv2 + (st2 - t)*sv1) / (st2 - st1);};}

    else
       tin = val;

    sp->index = spt;

    return(tin);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
