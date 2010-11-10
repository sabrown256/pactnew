/*
 * PAMM.C - the memory manager for PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_MK_VARIABLE - allocate and initialize a new PA_variable
 *                 - and return a pointer to it
 */

PA_variable *_PA_mk_variable(char *vname, PA_dimens *vdims, void *iv,
			     PFVarInit ifn,
			     int *vattr, char *vtype,
			     double conv_fact, double unit_fact,
			     pcons *nu, pcons *du, pcons *alist)
   {int nd;
    dimdes *dims;
    defstr *dp;
    PA_variable *pp;
    PA_dimens *pvd;
    
    if (PA_vif == NULL)
       PA_vif = PA_open("PA_vif", "r+", TRUE);

    pp = FMAKE(PA_variable, "_PA_MK_VARIABLE:pp");

    nd = 0;
    if (vdims == NULL)
       PA_VARIABLE_DESC(pp) = _PD_mk_syment(vtype, 1L, 0L, NULL, NULL);
    else
       {dims = _PA_mk_sym_dims(vdims);
        PA_VARIABLE_DESC(pp) = _PD_mk_syment(vtype, 0L, 0L, NULL, dims);
        for (pvd = vdims; pvd != NULL; pvd = pvd->next, nd++);};

    if (_PD_indirection(vtype))
       dp = PD_inquire_host_type(PA_vif, "*");
    else
       dp = PD_inquire_host_type(PA_vif, vtype);

    PA_ERR((dp == NULL),
           "TYPE %s NOT DEFINED - _PA_MK_VARIABLE", vtype);

    PA_VARIABLE_PACKAGE(pp)         = PA_current_package();
    PA_VARIABLE_UNIT_NUMER(pp)      = nu;
    PA_VARIABLE_UNIT_DENOM(pp)      = du;
    PA_VARIABLE_FILE_NAME(pp)       = NULL;
    PA_VARIABLE_ATTR(pp)            = alist;

    PA_VARIABLE_NAME(pp)            = SC_strsavef(vname,
						  "char*:_PA_MK_VARIABLE:vname");
    PA_VARIABLE_FILE(pp)            = NULL;
    PA_VARIABLE_FILE_CONVERS(pp)    = NONE;
    PA_VARIABLE_CACHE_FILE_NAME(pp) = NULL;

    PA_VARIABLE_DATA(pp)            = NULL;
    PA_VARIABLE_INIT_VAL(pp)        = iv;
    PA_VARIABLE_INIT_FUNC(pp)       = ifn;
    PA_VARIABLE_SIZE(pp)            = 0L;

    PA_VARIABLE_INT_UNIT(pp)        = unit_fact;
    PA_VARIABLE_EXT_UNIT(pp)        = conv_fact;

    PA_VARIABLE_TYPE(pp)            = dp;
    PA_VARIABLE_DIMS(pp)            = vdims;
    PA_VARIABLE_N_DIMS(pp)          = nd;

    PA_VARIABLE_SCOPE(pp)           = vattr[0];
    PA_VARIABLE_CLASS(pp)           = vattr[1];
    PA_VARIABLE_PERSISTENCE(pp)     = vattr[2];
    PA_VARIABLE_CENTERING(pp)       = (PM_centering) vattr[3];
    PA_VARIABLE_ALLOCATION(pp)      = vattr[4];

    PA_VARIABLE_ACCESS_LIST(pp)     = NULL;

    return(pp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_MK_DIMENS - allocate, initialize, and return a PA_dimens structure */

PA_dimens *_PA_mk_dimens(int *mini, int *maxi, int method)
   {PA_dimens *pd;

    pd = FMAKE(PA_dimens, "_PA_MK_DIMENS:pd");
    pd->index_min = mini;
    pd->index_max = maxi;
    pd->method    = method;
    pd->next      = NULL;

    return(pd);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_MK_SYM_DIMS - make the dimdes list from the variable dimension
 *                 - list given
 *                 - if min and/or max not given (NULL pointer) allocate
 *                 - a value and set it to zero instead of using Zero_I
 *                 - this protects against a _PA_consistent_size operation
 *                 - resetting Zero_I!!!!
 */

dimdes *_PA_mk_sym_dims(PA_dimens *vdims)
   {dimdes *dims, *prev, *next;
    PA_dimens *pvd;
    int *mini, *maxi;
    long ixn, ixx;

    dims = NULL;
    prev = NULL;
    next = NULL;
    for (pvd = vdims; pvd != NULL; pvd = pvd->next)
        {if (pvd->index_max != NULL)
            maxi = pvd->index_max;
         else
            {maxi  = FMAKE(int, "_PA_MK_SYM_DIMS:maxi");
             *maxi = 0;};

         if (pvd->index_min != NULL)
            mini = pvd->index_min;
         else
            {mini  = FMAKE(int, "_PA_MK_SYM_DIMS:mini");
             *mini = 0;};

         switch (pvd->method)
            {case PA_UPPER_LOWER :
                  ixn  = *mini;
                  ixx  = *maxi - *mini + 1L;
                  next = _PD_mk_dimensions(ixn, ixx);
                  break;

             case PA_OFFSET_NUMBER :
                  ixn  = *mini;
                  ixx  = *maxi;
                  next = _PD_mk_dimensions(ixn, ixx);
                  break;};

         if (dims == NULL)
            dims = next;
         else
            {prev->next = next;
	     SC_mark(next, 1);};

         prev = next;};

    return(dims);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_MK_SRC_VARIABLE - make a PA_src_variable
 *                     - and return a pointer to it
 */

PA_src_variable *_PA_mk_src_variable(char *name, int vindx, int n,
				     double *pt, PDBfile *fp)
   {PA_src_variable *svp;

    svp = FMAKE(PA_src_variable, "_PA_MK_SRC_VARIABLE:svp");

    svp->name        = name;
    svp->var_index   = vindx;
    svp->n_times     = n;
    svp->times       = pt;
    svp->index       = 0;
    svp->queue_times = FMAKE_N(double, 4, "_PA_MK_SRC_VARIABLE:queue_times");
    svp->queue       = FMAKE_N(double *, 4, "_PA_MK_SRC_VARIABLE:queue");
    svp->interpolate = TRUE;
    svp->file        = fp;

    return(svp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_MK_CONTROL - allocate and initialize the controls for a package
 *               - set the control array pointer: name, rparam, iswtch to
 *               - these new arrays since this routine is intended to
 *               - be called from the package control initializer functions
 *               - they will reset name, rparam, and iswtch to the global
 *               - arrays
 */

void PA_mk_control(PA_package *pck, char *s, int n_a, int n_p, int n_s)
   {char bf[MAXLINE], **pb;
    double *pd;
    int i, *pi, *pn_a, *pn_p, *pn_s;
    PA_thread_state *ps;

    ps = PA_get_thread(-1);

/* allocate the name array, initialize it, intern it, and set name to it */
    pck->n_ascii = n_a;
    if (n_a > 0)
       {snprintf(bf, MAXLINE, "%s-names", s);
        pn_a  = FMAKE(int, "PA_MK_CONTROL:pn_a");
        *pn_a = n_a;
        PA_inst_var(bf, SC_STRING_S, NULL, NULL,
                    SCOPE, DEFN, ATTRIBUTE,
                    pn_a, DIMENSION, UNITS);
        pck->ascii = (char **) PA_intern(NULL, bf);
        for (pb = pck->ascii, i = 0; i < n_a; i++, *(pb++) = NULL);
	ps->aname = pck->ascii - 1;}
    else
       {pck->ascii = NULL;
	ps->aname  = NULL;};

/* allocate the rparam array, initialize it, intern it, and set rparam to it */
    pck->n_param = n_p;
    if (n_p > 0)
       {snprintf(bf, MAXLINE, "%s-params", s);
        pn_p  = FMAKE(int, "PA_MK_CONTROL:pn_p");
        *pn_p = n_p;
        PA_inst_var(bf, SC_DOUBLE_S, NULL, NULL,
                    SCOPE, DEFN, ATTRIBUTE,
                    pn_p, DIMENSION, UNITS);
        pck->rparam = (double *) PA_intern(NULL, bf);
        for (pd = pck->rparam, i = 0; i < n_p; i++, *(pd++) = 0.0);
	ps->rparam = pck->rparam - 1;}
    else
       {pck->rparam = NULL;
	ps->rparam  = NULL;}

/* allocate the iswtch array, initialize it, intern it, and set iswtch to it */
    pck->n_swtch = n_s;
    if (n_s > 0)
       {snprintf(bf, MAXLINE, "%s-swtchs", s);
        pn_s  = FMAKE(int, "PA_MK_CONTROL:pn_s");
        *pn_s = n_s;
        PA_inst_var(bf, SC_INT_S, NULL, NULL,
                    SCOPE, DEFN, ATTRIBUTE,
                    pn_s, DIMENSION, UNITS);
        pck->iswtch = (int *) PA_intern(NULL, bf);
        for (pi = pck->iswtch, i = 0; i < n_s; i++, *(pi++) = 0);
	ps->iswtch = pck->iswtch - 1;}
    else
       {pck->iswtch = NULL;
	ps->iswtch  = NULL;}

    return;}

/*--------------------------------------------------------------------------*/

/*                        MEMMORY MANAGEMENT ROUTINES                       */

/*--------------------------------------------------------------------------*/

/* PA_MK_SPEC - bundle up initial value specification information
 *            - and return it in a pointer to an PA_iv_specification struct
 */

PA_iv_specification *PA_mk_spec(char *id, int type, char *fn, int n,
				int interp, pcons *lst,
				PA_iv_specification *nxt)
   {PA_iv_specification *sp;

    sp = FMAKE(PA_iv_specification, "_PA_MK_SPEC:sp");

    sp->type = type;
    sp->name = SC_strsavef(id, "char*:_PA_MK_SPEC :id");
    if (fn != NULL)
       sp->file = SC_strsavef(fn, "char*:_PA_MK_SPEC :fn");
    else
       sp->file = NULL;
    sp->num         = n;
    sp->index       = 0;
    sp->interpolate = interp;
    sp->spec        = lst;
    sp->data        = NULL;
    sp->next        = nxt;

    return(sp);}

/*--------------------------------------------------------------------------*/

/*                             DESTRUCTORS                                  */

/*--------------------------------------------------------------------------*/

/* _PA_RL_VARIABLE - de-allocate and release a PA_variable */

void _PA_rl_variable(PA_variable *pp)
   {

    SFREE(PA_VARIABLE_NAME(pp));
    _PD_rl_syment_d(PA_VARIABLE_DESC(pp));
    SFREE(pp);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RL_SET_SPEC - release a single PA_set_spec instance */

void _PA_rl_set_spec(PA_set_spec *ip)
   {

    SFREE(ip->var_name);
    SFREE(ip->function);
    SFREE(ip->text);
    SFREE(ip->values);

    SFREE(ip);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RL_REQUEST - release the list of PA_plot_requests */

void _PA_rl_request(PA_plot_request *rq)
   {PA_plot_request *pp, *nxt;
    PA_set_spec *ip, *inxt;

    for (pp = rq; pp != NULL; pp = nxt)
        {for (ip = pp->range; ip != NULL; ip = inxt)
             {inxt = ip->next;
	      _PA_rl_set_spec(ip);};

         for (ip = pp->domain; ip != NULL; ip = inxt)
             {inxt = ip->next;
	      _PA_rl_set_spec(ip);};

         nxt = pp->next;
         SFREE(pp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RL_SPEC - release the initial value specifications */

void _PA_rl_spec(PA_iv_specification *lst)
   {PA_iv_specification *sp, *nxt;

    for (sp = lst; sp != NULL; sp = nxt)
        {SFREE(sp->name);
         SFREE(sp->file);
         SFREE(sp->data);
         nxt = sp->next;
         SFREE(sp);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RL_DOMAIN_MAP - release a set index map */

void _PA_rl_domain_map(PA_set_index *dmap)
  {int i, nmax;

   nmax = SC_arrlen(dmap)/sizeof(PA_set_index);

    for (i = 0; i < nmax; i++)
        if (dmap[i].name != NULL)
           {SFREE(dmap[i].name);};

    SFREE_N(dmap, nmax); 

    return;}

/*--------------------------------------------------------------------------*/

/*                        ASSOCIATED MEMORY MANAGEMENT                      */

/*--------------------------------------------------------------------------*/

/* _PA_LIST_TO_DIMS - compute the size of a variable from its present
 *                  - dimension pointer list, fill in the dimension
 *                  - descriptor for the PANACEA variable
 *                  - and return the size
 */

long _PA_list_to_dims(PA_dimens *vd, dimdes *sd)
   {dimdes *psd;
    PA_dimens *pvd;
    long lnum, num, ixx, ixn;

    lnum = 1L;
    psd  = sd;
    pvd  = vd;

    if (sd == NULL)
       {if (vd != NULL)
           {PA_ERR((vd->next != NULL),
                   "VARIABLE DIMENSIONS INCONSISTENT - _PA_LIST_TO_DIMS");

            switch (vd->method)
               {case PA_UPPER_LOWER   :
                     ixx  = *vd->index_max;
                     ixn  = *vd->index_min;
                     lnum = ixx - ixn + 1L;
                     break;

                case PA_OFFSET_NUMBER :
                     lnum = *vd->index_max;
                     break;};};}

    else        
       {while (pvd != NULL)
           {num = 0;
	    ixn = 0;
	    ixx = -1;
	    switch (pvd->method)
               {case PA_UPPER_LOWER :
                     ixx = *pvd->index_max;
                     ixn = *pvd->index_min;
                     num = ixx - ixn + 1L;
                     break;

                case PA_OFFSET_NUMBER :
                     num = *pvd->index_max;
                     ixn = *pvd->index_min;;
                     ixx = ixn + num - 1L;
                     break;};

            ixx = max(ixn, ixx);
            num = max(num, 0L);

            lnum          *= num;
            psd->index_min = ixn;
            psd->index_max = ixx;
            psd->number    = num;
            pvd            = pvd->next;
            psd            = psd->next;};};

    return(lnum);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_DIMS_TO_LIST - compute the size of a variable from its dimension
 *                  - descriptor, fill in the dimension pointer list for
 *                  - the PANACEA variable
 *                  - and return the size
 *                  - NOTE: what happens if sd == NULL and vd != NULL
 *                  -       which is legal!
 */

long _PA_dims_to_list(dimdes *sd, PA_dimens *vd)
   {dimdes *psd;
    PA_dimens *pvd;
    long num, lnum, ixx, ixn;

    lnum = 1L;
    psd  = sd;
    pvd  = vd;
    ixn  = 0;
    ixx  = 0;
    while (psd != NULL)
       {num = psd->number;
        ixn = psd->index_min;
        switch (pvd->method)
           {case PA_UPPER_LOWER :
	         ixx = psd->index_max;
		 ixx = max(ixn, ixx);
		 break;

            case PA_OFFSET_NUMBER :
	         ixx = num;
		 ixx = max(ixx, 0L);
		 break;};

        num = max(num, 0L);

        lnum            *= num;
        *pvd->index_min  = ixn;
        *pvd->index_max  = ixx;
        psd              = psd->next;
        pvd              = pvd->next;};

    return(lnum);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_CONSISTENT_SIZE - determine which of the dimension list or the
 *                     - dimension descriptor is self consistent
 *                     - and take that one to reflect the current state of
 *                     - the variable by adjusting the inconsistent one to
 *                     - be the same
 *                     - FLAG may be specified to force the file (PA_FILE)
 *                     - or the database (PA_DATABASE) dimensions to be used
 *                     - and return the size
 */

long _PA_consistent_size(PA_variable *pp, int flag)
   {dimdes *filed, *pfiled;
    PA_dimens *memd, *pmemd;
    long num, filenum, memnum;

    filed = PA_VARIABLE_DESC(pp)->dimensions;
    memd  = PA_VARIABLE_DIMS(pp);

    if ((filed == NULL) && (memd == NULL))
       return(0L);

    filenum = 1L;
    for (pfiled = filed; pfiled != NULL; pfiled = pfiled->next)
        filenum *= pfiled->number;

    memnum = 1L;
    for (pmemd = memd; pmemd != NULL; pmemd = pmemd->next)
        memnum *= PA_VAR_DIM(pmemd);

/* if they agree assume that they are consistent */
    if (filenum == memnum)
       num = filenum;

/* the file implies zero length */
    else if (filenum == 0L)
       {switch (flag)
           {case PA_FILE     : num = _PA_dims_to_list(filed, memd);
                               break;
            case PA_DATABASE :
            default          : num = _PA_list_to_dims(memd, filed);
                               break;};}

/* the database implies zero length */
    else if (memnum == 0L)
       {switch (flag)
           {case PA_DATABASE : num = _PA_list_to_dims(memd, filed);
                               break;
            case PA_FILE     :
            default          : num = _PA_dims_to_list(filed, memd);
                               break;};}

/* else use the flag */
    else
       {switch (flag)
           {case PA_FILE     : num = _PA_dims_to_list(filed, memd);
                               break;
            case PA_DATABASE : num = _PA_list_to_dims(memd, filed);
                               break;
            default          : num = memnum;
                               break;};};
    num = max(num, 0L);

    PA_VARIABLE_SIZE(pp)         = num;
    PA_VARIABLE_DESC(pp)->number = num;

    return(num);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_COMP_SIZE - compute the size of a variable from its present
 *               - dimensions
 *               - and return the size
 */

long _PA_comp_size(PA_dimens *vdims)
   {PA_dimens *pv;
    long lnum;

    lnum = 1L;
    for (pv = vdims; pv != NULL; pv = pv->next)
        lnum *= PA_VAR_DIM(pv);

    lnum = max(lnum, 0L);

    return(lnum);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_MARK_SPACE - mark the current allocated space */

void PA_mark_space(PA_package *pck)
   {long lsp;
    PA_thread_state *ps;

    ps = PA_get_thread(-1);

    SC_mem_stats(&lsp, NULL, NULL, NULL);

    ps->mark_space = lsp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_ACCM_SPACE - count the space allocated since the previous MARK_SPACE */

void PA_accm_space(PA_package *pck)
   {long lsp;
    PA_thread_state *ps;

    ps = PA_get_thread(-1);

    SC_mem_stats(&lsp, NULL, NULL, NULL);

    pck->space += ((double) lsp - ps->mark_space);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_MARK_TIME - mark the current CPU time */

void PA_mark_time(PA_package *pck)
   {PA_thread_state *ps;

    ps = PA_get_thread(-1);

    ps->mark_time = SC_cpu_time();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_ACCM_TIME - count the time allocated since the previous MARK_TIME */

void PA_accm_time(PA_package *pck)
   {double dt;
    PA_thread_state *ps;

    ps = PA_get_thread(-1);

    dt = SC_cpu_time() - ps->mark_time;

    pck->time += ((dt > 0.0) ? dt : 0.0);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

