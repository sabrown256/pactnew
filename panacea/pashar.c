/*
 * PASHAR.C - routines that are shared between PANACEA and its pre and
 *          - post processors
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

#define COMPUTE_CONVERSION_FACTOR(flag, fac, conv, unit)                   \
    switch (flag)                                                          \
       {case INT_CGS : fac = 1.0/unit;                                     \
                       break;                                              \
        case INT_EXT : fac = conv/unit;                                    \
                       break;                                              \
        case EXT_CGS : fac = 1.0/conv;                                     \
                       break;                                              \
        case EXT_INT : fac = unit/conv;                                    \
                       break;                                              \
        case CGS_INT : fac = unit;                                         \
                       break;                                              \
        case CGS_EXT : fac = conv;                                         \
                       break;                                              \
        default      : fac = 1.0;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_VAR - the most general variable installer for PANACEA
 *            - usage:
 *            -
 *            - PA_def_var(<name>, <type>, <ptr to init_val>, <init_func>,
 *            -            <attrribute_spec>, ..., ATTRIBUTE,
 *            -            <dimension_spec>, ..., DIMENSION,
 *            -            <unit_spec>, ..., UNITS
 *            -            [, <ptr to data>)
 *            -
 *            - <attribute_spec> := <attribute_id>, <attribute_val>
 *            - <dimension_spec> := <ptr to # values> |
 *            -                     PA_gs.dul, <ptr to lower>, <ptr to upper> |
 *            -                     PA_gs.don, <ptr to offset>, <ptr to # elem>
 *            - <unit_spec>      := <unit_index> | PER
 *            -
 *            - The valid attribute_id's are:
 *            -      SCOPE, CLASS, PERSIST, CENTER, ALLOCATION
 *            -
 *            - The valid SCOPE's are:
 *            -      DEFN, RESTART, DMND, RUNTIME, EDIT, SCRATCH
 *            -
 *            - The valid CLASS's are:
 *            -      REQU, OPTL, PSEUDO
 *            -
 *            - The valid PERSIST's are:
 *            -      REL, KEEP, CACHE_F, CACHE_R
 *            -
 *            - The valid CENTER's are:
 *            -      Z_CENT, N_CENT, F_CENT, E_CENT, U_CENT
 *            -
 *            - The valid ALLOCATION's are:
 *            -      STATIC, DYNAMIC
 *            -
 *            - The pre-defined unit_index's are:
 *            -      RAD, STER, MOLE, Q, CM, SEC, G, EV, K, ERG, CC
 *            -
 *            - No attributes are required to be set (ATTRIBUTE must appear
 *            - in the arg list) and the default values are:
 *            -      RUNTIME, OPTL, REL, U_CENT, DYNAMIC
 */
 

void PA_def_var(const char *vname, const char *vtype, void *viv,
                PFVarInit vif, ...)
   {PA_variable *pp;
    int dm, *pv, *mini, *maxi, meth;
    int enough, v, vattr[N_ATTRIBUTES];
    PA_dimens *vdims, *next, *prev;
    pcons *nu, *du, *nxt, *prv;

/* make the variable hash table if it doesn't exist yet */
    if (PA_gs.variable_tab == NULL)
       PA_gs.variable_tab = SC_make_hasharr(HSZLARGE, DOC, SC_HA_NAME_KEY, 0);

    SC_VA_START(vif);

    vattr[0] = RUNTIME;
    vattr[1] = OPTL;
    vattr[2] = REL;
    vattr[3] = U_CENT;
    vattr[4] = DYNAMIC;
    enough   = FALSE;
    while (!enough)
       {v = SC_VA_ARG(int);
        switch (v)
           {case SCOPE :
	         vattr[0] = SC_VA_ARG(int);
		 break;
            case CLASS :
	         vattr[1] = SC_VA_ARG(int);
		 break;
            case PERSIST :
	         vattr[2] = SC_VA_ARG(int);
		 break;
            case CENTER :
	         vattr[3] = SC_VA_ARG(int);
		 break;
            case ALLOCATION :
	         vattr[4] = SC_VA_ARG(int);
		 break;
            case ATTRIBUTE :
	         enough = TRUE;
		 break;
            default :
	         PA_ERR(TRUE,
			"BAD ATTRIBUTE %d - PA_DEF_VAR",
			v);};};

/* get the dimensions */
    vdims = NULL;
    while ((maxi = SC_VA_ARG(int *)) != DIMENSION)
       {if (maxi == PA_gs.dul)
           {mini = SC_VA_ARG(int *);
            maxi = SC_VA_ARG(int *);
            meth = *PA_gs.dul;}

        else if (maxi == PA_gs.don)
           {mini = SC_VA_ARG(int *);
            maxi = SC_VA_ARG(int *);
            meth = *PA_gs.don;}

        else
           {mini = &PM_c.izero.i;
            meth = *PA_gs.don;};

        next = _PA_mk_dimens(mini, maxi, meth);
        if (vdims == NULL)
           vdims = next;
        else
           prev->next = next;
        prev = next;};

/* get the units */
    nu = NULL;
    while (TRUE)
       {dm = SC_VA_ARG(int);
        if ((dm == PER) || (dm == UNITS))
           break;
        pv  = CMAKE(int);
        *pv = dm;
        nxt = SC_mk_pcons(G_INT_P_S, pv, G_PCONS_P_S, NULL);
        if (nu == NULL)
           nu = nxt;
        else
           prv->cdr = (void *) nxt;
        prv = nxt;};

    du = NULL;
    if (dm != UNITS)
       while ((dm = SC_VA_ARG(int)) != UNITS)
          {pv  = CMAKE(int);
           *pv = dm;
           nxt = SC_mk_pcons(G_INT_P_S, pv, G_PCONS_P_S, NULL);
           if (du == NULL)
              du = nxt;
           else
              prv->cdr = (void *) nxt;
           prv = nxt;};

    pp = _PA_mk_variable(vname, vdims, viv, vif,
                         vattr, vtype,
                         1.0, 1.0, nu, du, NULL);

    if (PA_VARIABLE_ALLOCATION(pp) == STATIC)
       PA_VARIABLE_DATA(pp) = SC_VA_ARG(void *);

    _PA_install_var(vname, pp);

    SC_VA_END;
 
/* install primitive scalars as implicit commands */
    if ((pp->n_dimensions == 0) && (PA_gs.command_tab != NULL) &&
	(PA_VARIABLE_CLASS(pp) != PSEUDO))
       {void *vaddr;
        int itype;

	itype = SC_type_id(vtype, FALSE);

        PA_CONNECT(vaddr, vname, FALSE);

        PA_inst_c(vname, vaddr, itype, 0, (PFVoid) PA_pshand, PA_sargs);};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INST_VAR - install a variable in the code system variable table.
 *             - The class of the variable determines how the code will treat
 *             - it.  REQUired variables MUST be in a restart file and they
 *             - MUST be written to a restart file or the code
 *             - will abort. OPTionaL variables will be read in if they exist
 *             - and will be written if their pointers are non-NULL.  DeMaND
 *             - variables will be read on demand by the CONNECT process and
 *             - will be written if the pointer is non-NULL.
 *             -
 *             - the definition of syment from PDBLib:
 *             -
 *             - struct s_syment
 *             -    {char *type;
 *             -     dimdes *dimensions;
 *             -     long number;
 *             -     long diskaddr;};
 *             -
 *             - typedef struct s_syment syment;
 */
 

void PA_inst_var(const char *vname, const char *vtype, void *viv,
                 PFVarInit vif, ...)
   {int dm, *pv, *mini, *maxi, meth;
    int enough, v, vattr[N_ATTRIBUTES];
    PA_variable *pp;
    PA_dimens *vdims, *next, *prev;
    pcons *nu, *du, *nxt, *prv;

/* make the variable hash table if it doesn't exist yet */
    if (PA_gs.variable_tab == NULL)
       PA_gs.variable_tab = SC_make_hasharr(HSZLARGE, DOC, SC_HA_NAME_KEY, 0);

    SC_VA_START(vif);

    vattr[0] = RUNTIME;
    vattr[1] = OPTL;
    vattr[2] = REL;
    vattr[3] = U_CENT;
    vattr[4] = DYNAMIC;
    enough   = FALSE;
    while (!enough)
       {v = SC_VA_ARG(int);
        switch (v)
           {case SCOPE :
	         vattr[0] = SC_VA_ARG(int);
		 break;
            case CLASS :
	         vattr[1] = SC_VA_ARG(int);
		 break;
            case PERSIST :
	         vattr[2] = SC_VA_ARG(int);
		 break;
            case CENTER :
	         vattr[3] = SC_VA_ARG(int);
		 break;
            case ALLOCATION :
	         vattr[4] = SC_VA_ARG(int);
		 break;
            case ATTRIBUTE :
	         enough = TRUE;
		 break;
            default :
	         PA_ERR(TRUE,
			"BAD ATTRIBUTE %d - PA_INST_VAR",
			v);};};

/* get the dimensions */
    vdims = NULL;
    while ((maxi = SC_VA_ARG(int *)) != DIMENSION)
       {if (maxi == PA_gs.dul)
           {mini = SC_VA_ARG(int *);
            maxi = SC_VA_ARG(int *);
            meth = *PA_gs.dul;}

        else if (maxi == PA_gs.don)
           {mini = SC_VA_ARG(int *);
            maxi = SC_VA_ARG(int *);
            meth = *PA_gs.don;}

        else
           {mini = &PM_c.izero.i;
            meth = *PA_gs.don;};

        next = _PA_mk_dimens(mini, maxi, meth);
        if (vdims == NULL)
           vdims = next;
        else
           prev->next = next;
        prev = next;};

/* get the units */
    nu = NULL;
    while (TRUE)
       {dm = SC_VA_ARG(int);
        if ((dm == PER) || (dm == UNITS))
           break;
        pv  = CMAKE(int);
        *pv = dm;
        nxt = SC_mk_pcons(G_INT_P_S, pv, G_PCONS_P_S, NULL);
        if (nu == NULL)
           nu = nxt;
        else
           prv->cdr = (void *) nxt;
        prv = nxt;};

    du = NULL;
    if (dm != UNITS)
       while ((dm = SC_VA_ARG(int)) != UNITS)
          {pv  = CMAKE(int);
           *pv = dm;
           nxt = SC_mk_pcons(G_INT_P_S, pv, G_PCONS_P_S, NULL);
           if (du == NULL)
              du = nxt;
           else
              prv->cdr = (void *) nxt;
           prv = nxt;};

    SC_VA_END;
 
    pp = _PA_mk_variable(vname, vdims, viv, vif,
                         vattr, vtype,
                         1.0, 1.0, nu, du, NULL);

    _PA_install_var(vname, pp);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INST_SCALAR - install a variable in the code system variable table.
 *                - The class of the variable determines how the code will
 *                - treat it.  REQUired variables MUST be initializable
 *                - and they MUST be written to a restart file or the code
 *                - will abort. OPTionaL variables will be read in if they
 *                - exist and will be written if their pointers are non-NULL.
 *                - DeMaND variables will be read on demand by the CONNECT
 *                - process and will be written if the pointer is non-NULL.
 *                -
 *                - the definition of syment from PDBLib:
 *                -
 *                - struct s_syment
 *                -    {char *type;
 *                -     dimdes *dimensions;
 *                -     long number;
 *                -     long diskaddr;};
 *                -
 *                - typedef struct s_syment syment;
 */
 

void PA_inst_scalar(const char *vname, const char *vtype,
		    void *vaddr, void *viv, PFVarInit vif, ...)
   {PA_variable *pp;
    int dm, *pv, itype;
    int v, enough, vattr[N_ATTRIBUTES];
    pcons *nu, *du, *next, *prev;

/* make the variable hash table if it doesn't exist yet */
    if (PA_gs.variable_tab == NULL)
       PA_gs.variable_tab = SC_make_hasharr(HSZLARGE, DOC, SC_HA_NAME_KEY, 0);

    SC_VA_START(vif);

    vattr[0] = RUNTIME;
    vattr[1] = OPTL;
    vattr[2] = REL;
    vattr[3] = U_CENT;
    vattr[4] = STATIC;    /* don't allow this to be changed in this call */
    enough   = FALSE;
    while (!enough)
       {v = SC_VA_ARG(int);
        switch (v)
           {case SCOPE :
	         vattr[0] = SC_VA_ARG(int);
		 break;
            case CLASS :
	         vattr[1] = SC_VA_ARG(int);
		 break;
            case PERSIST :
	         vattr[2] = SC_VA_ARG(int);
		 break;
            case CENTER :
	         vattr[3] = SC_VA_ARG(int);
		 break;
            case ATTRIBUTE :
	         enough = TRUE;
		 break;
            default :
	         PA_ERR(TRUE, "BAD ATTRIBUTE %d - PA_INST_SCALAR", v);};};

/* get the units */
    nu = NULL;
    while (TRUE)
       {dm = SC_VA_ARG(int);
        if ((dm == PER) || (dm == UNITS))
           break;
        pv  = CMAKE(int);
        *pv = dm;
        next = SC_mk_pcons(G_INT_P_S, pv, G_PCONS_P_S, NULL);
        if (nu == NULL)
           nu = next;
        else
           prev->cdr = (void *) next;
        prev = next;};

    du = NULL;
    if (dm != UNITS)
       while ((dm = SC_VA_ARG(int)) != UNITS)
          {pv  = CMAKE(int);
           *pv = dm;
           next = SC_mk_pcons(G_INT_P_S, pv, G_PCONS_P_S, NULL);
           if (du == NULL)
              du = next;
           else
              prev->cdr = (void *) next;
           prev = next;};

    SC_VA_END;
 
    pp = _PA_mk_variable(vname, NULL, viv, vif,
                         vattr, vtype,
                         1.0, 1.0, nu, du, NULL);

    PA_VARIABLE_DATA(pp) = vaddr;

    _PA_install_var(vname, pp);

    itype = SC_type_id(vtype, FALSE);

    if (SC_is_type_num(itype) == FALSE)
       PA_ERR(TRUE,
              "TYPE %s UNSUPPORTED FOR SCALARS - PA_INST_SCALAR");

    if (PA_gs.command_tab != NULL)
       PA_inst_c(vname, vaddr, itype, 0, (PFVoid) PA_pshand, PA_sargs);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INSTALL_VAR - install the named variable in the database
 *                 - the actual database name will depend on PA_name_space
 */

void _PA_install_var(const char *vname, PA_variable *pp)
   {char *name, s[MAXLINE];
    PA_package *pck;

    if (PA_gs.name_spaces)
       {pck  = PA_current_package();
        name = pck->name;
	snprintf(s, MAXLINE, "%s-%s", name, vname);}
    else
       SC_strncpy(s, MAXLINE, vname, -1);

    SC_hasharr_install(PA_gs.variable_tab, s, pp, PA_gs.variable, 3, -1);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_PDB_READ - make up a new syment which addresses the desired parts 
 *              - of the variable and using rd_syment read the PDB file data
 *              - into a newly allocated space (type char) pointed to by var
 *              - return pointer to data (var) if successful
 *              - and NULL otherwise
 */

void *_PA_pdb_read(PDBfile *file, const char *name, syment **psp, long *indx)
   {char *token, memb[MAXLINE], *vtype, *s;
    void *vr;
    syment *ep;
    defstr *dp;

    vr = NULL;

    SC_strncpy(memb, MAXLINE, name, -1);
    token = SC_strtok(memb, ".([", s);
    if (token != NULL)

/* look up the variable name and return NULL if it is not there */
       {ep = _PD_effective_ep(file, token, TRUE, NULL);
	if (ep != NULL)
	   {vtype = PD_entry_type(ep);

	    if (_PD_indirection(vtype))
	       dp = PD_inquire_host_type(file, "*");
	    else
	       dp = PD_inquire_host_type(file, vtype);

	    PA_ERR((dp == NULL),
		   "BAD TYPE %s - _PA_PDB_READ", vtype);

	    vr = _PA_alloc(dp, vtype, ep->number, NULL);

	    if (indx == NULL)
	       PA_ERR(!PD_read(file, token, vr),
		      "CAN'T READ %s FROM FILE - _PA_PDB_READ", name);

	    else
	       PA_ERR(!PD_read_alt(file, token, vr, indx),
		      "CAN'T READ %s FROM FILE - _PA_PDB_READ", name);

/* make a copy of the syment because the file's syment will be GC'd */
	    if (psp != NULL)
	       *psp = ep;
	    else
	       _PD_rl_syment(ep);};};

    return(vr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_OPEN - open up a PDB file and install any necessary types
 *         - return the PDB file pointer iff successful
 */

PDBfile *PA_open(const char *name, const char *mode, int flag)
   {PDBfile *fp;

    if (strcmp(name, "vif") == 0)
       {if ((PA_gs.pp != NULL) && (PA_gs.pp->vif != NULL))
           fp = PA_gs.pp->vif;
        else
	   {fp = PD_open_vif("vif");
	    PD_mkdir(fp, "/&ptrs");};

        PA_gs.vif = fp;}
    else
       {fp = PD_open(name, mode);
	PD_mkdir(fp, "/&ptrs");};

    PA_ERR((fp == NULL),
           "CAN'T OPEN FILE %s - PA_OPEN\n", name);

    PD_gs.err[0] = '\0';

    PD_set_major_order(fp, COLUMN_MAJOR_ORDER);
    PD_set_offset(fp, PA_get_default_offset());

/* not all packages have been defined so this will miss structs for
 * any future packages if called to early
 * go get any special structures defined for the dump
 */
    if (flag)
       PA_def_str(fp);

    return(fp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_WR_PSEUDO_PLOT_REQUESTS - write out the pseudo plot requests
 *                             - these have been detached from the plot
 *                             - request list and need to be written out
 */

static void _PA_wr_pseudo_plot_requests(PDBfile *pdrs)
   {PA_package *pck;
    PA_plot_request *pr;
    char bf[MAXLINE];

    for (pck = PA_gs.packages; pck != NULL; pck = pck->next)
        {pr = pck->pseudo_pr;
         if (pr != NULL)
            {snprintf(bf, MAXLINE, "%s-pseudo-plots", pck->name);
             PD_write(pdrs, bf, "PA_plot_request *", &pr);};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RD_PSEUDO_PLOT_REQUESTS - read in the pseudo plot requests
 *                             - these had been detached from the plot
 *                             - request list and need to be re-attached
 */

static void _PA_rd_pseudo_plot_requests(PDBfile *pdrs)
   {PA_package *pck;
    char bf[MAXLINE];

    for (pck = PA_gs.packages; pck != NULL; pck = pck->next)
        {snprintf(bf, MAXLINE, "%s-pseudo-plots", pck->name);
         PD_read(pdrs, bf, &pck->pseudo_pr);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_WRRSTRT - write a restart file
 *             - The class of the variable determines how the code will treat
 *             - it.  REQUired variables MUST be in a restart file and they
 *             - MUST be written to a restart file or the code
 *             - will abort. OPTionaL variables will be read in if they exist
 *             - and will be written if their pointers are non-NULL.  DeMaND
 *             - variables will be read on demand by the CONNECT process and
 *             - will be written if the pointer is non-NULL.
 *             -
 *             - The conversion flag (conv_flag) will control force
 *             - conversions between any of the three systems of units:
 *             - internal, external, and CGS.  This is accomplished by
 *             - multiplying all variables by their associated conversion
 *             - factors before writing them and then dividing the conversion
 *             - factors out after so as not to disturbed their state or
 *             - take up more space in buffering potentially huge arrays.
 */

void _PA_wrrstrt(char *rsname, int conv_flag)
   {int int_conv_flag;
    int pclass, pscope, psz;
    long i;
    double conv_fac;
    char *lname, *pname, *ty;
    void *pdata;
    PA_variable *pp;
    syment *ep;
    PDBfile *pdrs;
    PFWriteState hook;
    FILE *fp;
    PD_smp_state *pa;

    pa = _PD_get_state(-1);

    if (rsname == NULL)
       return;

/* create the restart dump */
    pdrs = PA_open(rsname, "w", TRUE);
    if (pdrs != NULL)
       {fp = pdrs->stream;

	PD_cd(pdrs, "/");

/* save the definition constants */
	PD_write(pdrs, "n_units", G_INT_S, &PA_gs.n_units);
	_PA_wr_pseudo_plot_requests(pdrs);

/* check every element of PA_gs.variable_tab to find the RESTART variables */
	switch (SETJMP(pa->write_err))
	   {case ABORT :
	         PRINT(stdout, "\n\n%s \n\n", PD_get_error());
		 PA_ERR(!PD_close(pdrs),
			"CAN'T CLOSE STATE FILE %s - _PA_WRRSTRT",
			rsname);
		 LONGJMP(SC_gs.cpu, ABORT);
	    case ERR_FREE :
		 return;
	    default :
	         break;};

	for (i = 0; SC_hasharr_next(PA_gs.variable_tab, &i, NULL, &ty, (void **) &pp); i++)
	    {if (ty[3] == 'p')                   /* skip the packages */
	        continue;

	     pname  = PA_VARIABLE_NAME(pp);
	     pclass = PA_VARIABLE_CLASS(pp);
	     pdata  = PA_VARIABLE_DATA(pp);
	     pscope = PA_VARIABLE_SCOPE(pp);

/* if conversions are requested check that the conversion factor isn't unity */
	     COMPUTE_CONVERSION_FACTOR(conv_flag, conv_fac,
				       PA_VARIABLE_EXT_UNIT(pp),
				       PA_VARIABLE_INT_UNIT(pp));
	     if (conv_fac != 1.0)
	        int_conv_flag = TRUE;
	     else
	        int_conv_flag = FALSE;

	     if ((pscope == RESTART) || (pscope == DEFN) || (pscope == DMND))
	        {inti ni;

		 ep = PD_copy_syment(PA_VARIABLE_DESC(pp));
		 PA_ERR((ep == NULL),
			"COPY FAILED SYMENT - _PA_WRRSTRT");

		 ni = _PD_comp_num(PD_entry_dimensions(ep));

		 PD_entry_set_address(ep, _PD_get_next_address(pdrs, 
							       PD_entry_type(ep),
							       ni,
							       pdata,
							       FALSE,
							       TRUE,
							       FALSE));

/* if a DMND variable has no data PA_CONNECT it */
		 if ((pscope == DMND) && (pdata == NULL))
		    PA_CONNECT(pdata, pname, FALSE);

/* force consistency in variable size and shape */
		 psz = _PA_list_to_dims(PA_VARIABLE_DIMS(pp),
					PD_entry_dimensions(ep));
		 PA_VARIABLE_SIZE(pp) = psz;
		 ep->number           = psz;

		 switch (pclass)
		    {case REQU :
		          if (pdata == NULL)
			     {PA_CONNECT(pdata, pname, FALSE);
			      PA_ERR((pdata == NULL),
				     "VARIABLE %s IS REQUIRED",
				     pname);};

		     case PSEUDO :
		     case OPTL   :
			  if (pdata != NULL)
			     {lname = _PD_fixname(pdrs, pname);
			      _PD_e_install(pdrs, lname, ep, TRUE);

/* convert units before writing if requested */
			      if (int_conv_flag)
				 PM_array_scale(pdata, psz,
						conv_fac);

			      PA_ERR(!_PD_sys_write(pdrs, lname, pdata,
						    PD_entry_number(ep),
						    PD_entry_type(ep),
						    PD_entry_type(ep)),
				     "CAN'T WRITE VARIABLE: %s",
				     pname);

/* convert back so that runtime data isn't trashed */
			      if (int_conv_flag)
				 PM_array_scale(pdata, psz,
						1.0/conv_fac);}
			  else
			     _PD_rl_syment_d(ep);

			  break;

		     default :
			  PA_ERR(TRUE,
				 "ILLEGAL STORAGE CLASS %d - _PA_WRRSTRT",
				 pclass);};};};

/* give the application a chance to add anything special to the state dump */
	hook = PA_GET_FUNCTION(PFWriteState, "write-state");
	if (hook != NULL)
	   (*hook)(pdrs);

	pdrs->chrtaddr = lio_tell(fp);
	PA_ERR(pdrs->chrtaddr == -1,
	       "CAN'T FIND ADDRESS FOR RESTART FILE - _PA_WRRSTRT");

/* close the restart dump and advance the name */
	PA_ERR(!PD_close(pdrs),
	       "CAN'T PROPERLY CLOSE RSTART FILE - %s\n", rsname);
	PA_advance_name(rsname);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RDRSTRT - read a restart file
 *             - The class of the variable determines how the code will treat
 *             - it.  REQUired variables MUST be in a restart file and they
 *             - MUST be written to a restart file or the code
 *             - will abort. OPTionaL variables will be read in if they exist
 *             - and will be written if their pointers are non-NULL.  DeMaND
 *             - variables will be read on demand by the CONNECT process and
 *             - will be written if the pointer is non-NULL.
 *             -
 *             - The conversion flag (conv_flag) will control force
 *             - conversions between any of the three systems of units:
 *             - internal, external, and CGS.  This is accomplished by
 *             - multiplying all variables by their associated conversion
 *             - factors before writing them and then dividing the conversion
 *             - factors out after so as not to disturbed their state or
 *             - take up more space in buffering potentially huge arrays.
 */

void _PA_rdrstrt(const char *fname, int conv_flag)
   {int n_dmnd;
    long i;
    char *ty;
    PA_variable *pp;
    PDBfile *pdrs;
    char *pck_name, bf[MAXLINE], *s, *tok;
    PA_package *pck;
    PFReadState hook;

    PA_control_set("global");

/* set the rsname to this name to be able to CONNECT DeMaND variables */
    CFREE(_PA.rsname)
    _PA.rsname = CSTRSAVE(fname);

    SC_strncpy(bf, MAXLINE, fname, -1);
    tok = SC_strtok(bf, ". \r\n\t", s);
    if (tok != NULL)
       SC_strncpy(_PA.base_name, MAXLINE, tok, -1);

    if (PA_gs.vif == NULL)
       PA_gs.vif = PA_open("vif", "w+", TRUE);

/* open the restart dump */
    pdrs = PA_open(_PA.rsname, "r", FALSE);
    PA_ERR((pdrs == NULL),
           "CAN'T OPEN RSTART FILE - %s", _PA.rsname);

/* read the definition constants */
    PD_read(pdrs, "n_units", &PA_gs.n_units);

/* define the problem definition variables */
    PA_definitions();

/* read in the definition variables */
    for (i = 0; SC_hasharr_next(PA_gs.variable_tab, &i, NULL, &ty, (void **) &pp); i++)
        {if (ty[3] == 'p')                   /* skip the packages */
	    continue;

	 _PA_rd_variable(pdrs, pp, conv_flag, DEFN);};

/* connect all of the package control variables */
    for (pck = PA_gs.packages; pck != NULL; pck = pck->next)
        {pck_name = pck->name;
         snprintf(bf, MAXLINE, "%s-names", pck_name);
         CFREE(pck->ascii);
         pp = PA_INQUIRE_VARIABLE(bf);
         if (pp != NULL)
            PA_CONNECT(pck->ascii, bf, TRUE);

         snprintf(bf, MAXLINE, "%s-params", pck_name);
         CFREE(pck->rparam);
         pp = PA_INQUIRE_VARIABLE(bf);
         if (pp != NULL)
            PA_CONNECT(pck->rparam, bf, TRUE);

         snprintf(bf, MAXLINE, "%s-swtchs", pck_name);
         CFREE(pck->iswtch);
         pp = PA_INQUIRE_VARIABLE(bf);
         if (pp != NULL)
            PA_CONNECT(pck->iswtch, bf, TRUE);};

/* connect all of the global control variables */
    PA_control_set("global");
    PA_gs.global_swtch = SWTCH;
    PA_gs.global_param = PARAM;
    PA_gs.global_name  = NAME;

/* connect the internal and extern unit definition variables */
    PA_CONNECT(PA_gs.units,    "unit",    TRUE);
    PA_CONNECT(PA_gs.convrsns, "convrsn", TRUE);

/* define the variables - and leave the unit conversions alone */
    PA_variables(NONE);  
    PA_proc_units();

/* check every element of PA_gs.variable_tab to find the
 * RESTART and DMND variables
 */
    n_dmnd = 0;
    for (i = 0; SC_hasharr_next(PA_gs.variable_tab, &i, NULL, &ty, (void **) &pp); i++)
        {if (ty[3] == 'p')                   /* skip the packages */
	    continue;

	 if (PA_VARIABLE_SCOPE(pp) == DMND)
	    {PA_VARIABLE_FILE_NAME(pp)    = CSTRSAVE(_PA.rsname);
	     PA_VARIABLE_FILE_CONVERS(pp) = conv_flag;
	     PA_VARIABLE_FILE(pp)         = pdrs;
	     n_dmnd++;}
	 else
	    _PA_rd_variable(pdrs, pp, conv_flag, RESTART);};

/* connect the initial value specifications */
    PA_CONNECT(PA_gs.iv_spec_lst, "initial-value-specifications", TRUE);

/* connect the plot requests now */
    PA_CONNECT(PA_gs.plot_reqs, "plot-requests", TRUE);
    _PA_rd_pseudo_plot_requests(pdrs);

/* give the application a chance to read anything special from the state dump */
    hook = PA_GET_FUNCTION(PFReadState, "read-state");
    if (hook != NULL)
       (*hook)(pdrs);

/* advance the state file name */
    PA_advance_name(_PA.rsname);

    if (_PA.state_files == NULL)
       {_PA.state_files = CMAKE_N(PDBfile *, _PA.max_state_files);}

    else if (_PA.n_state_files >= _PA.max_state_files)
       {_PA.max_state_files += 10;
        CREMAKE(_PA.state_files, PDBfile *, _PA.max_state_files);};

    _PA.state_files[_PA.n_state_files++] = pdrs;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_RD_VARIABLE - read a variable of the given scope */

void _PA_rd_variable(PDBfile *pdrs, PA_variable *pp,
		     int conv_flag, int scope)
   {int psz, int_conv_flag;
    double conv_fac;
    syment *ep;
    int pclass, pscope;
    char *pname;
    void *pdata;

    pname  = PA_VARIABLE_NAME(pp);
    pclass = PA_VARIABLE_CLASS(pp);
    pscope = PA_VARIABLE_SCOPE(pp);

    ep = PD_inquire_entry(pdrs, pname, FALSE, NULL);
    if (ep == NULL)
       {PA_ERR(((pscope == scope) && (pclass == REQU)),
               "REQUIRED VARIABLE %s NOT IN %s - _PA_RD_VARIABLE",
               pname, _PA.rsname);
        return;};

/* recompute the size of all variables now that the defining parameters
 * will have been read in
 */
    psz                  = _PA_dims_to_list(PD_entry_dimensions(ep),
                                            PA_VARIABLE_DIMS(pp));
    PA_VARIABLE_SIZE(pp) = psz;
    ep->number           = psz;

/* if conversions are requested check that the conversion factor isn't unity */
    COMPUTE_CONVERSION_FACTOR(conv_flag, conv_fac,
                              PA_VARIABLE_EXT_UNIT(pp),
                              PA_VARIABLE_INT_UNIT(pp));
    if (conv_fac != 1.0)
       int_conv_flag = TRUE;
    else
       int_conv_flag = FALSE;

    if (pscope == scope)

/* STATIC may have their space already and only need a value read in */
       {if ((PA_VARIABLE_ALLOCATION(pp) == STATIC) &&
	    (PA_VARIABLE_DATA(pp) != NULL))

/* initialize the space and let the restart value overwrite it if it
 * exists
 */
           {_PA_init_space(pp, psz);
            PD_read(pdrs, pname, PA_VARIABLE_DATA(pp));}

/* allocate and read values in to an array or unallocated scalar */
        else
           PA_VARIABLE_DATA(pp) = _PA_pdb_read(pdrs, pname, NULL, NULL);

        PA_VARIABLE_DESC(pp) = PD_copy_syment(ep);
        pdata                = PA_VARIABLE_DATA(pp);

/* convert the file data to the requested unit system */
        if (int_conv_flag)
           PM_array_scale(pdata, psz, conv_fac);

        PA_ERR(((pdata == NULL) && (pclass == REQU)),
               "FAILED TO READ REQUIRED VARIABLE %s - _PA_RD_VARIABLE",
               pname);};

/* remember the file so that PA_CONNECTS can access the structure chart
 * for allocations
 */
    PA_VARIABLE_FILE(pp) = pdrs;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_ERROR_HANDLER - the error handler
 *                  - give the most graceful possible shutdown of a run
 */

void PA_error_handler(int test, const char *fmt, ...)
   {

/* reroute the error handler if required - first time only */
    ONCE_SAFE(TRUE, NULL)
      {PFErrHand hook;

       hook = PA_GET_FUNCTION(PFErrHand, "error_handler");
       if (hook != NULL)
          {PA_gs.error_hook = hook;
	   SC_VA_START(fmt);
	   SC_VSNPRINTF(PA_gs.errbuf, MAXLINE, fmt);
	   SC_VA_END;
           PA_ERR(test, PA_gs.errbuf);};}
    END_SAFE;

    if (!test)
       return;

    PRINT(stdout,"\nERROR: ");
    SC_VA_START(fmt);
    SC_VSNPRINTF(PA_gs.errbuf, MAXLINE, fmt);
    SC_VA_END;

    PA_error(PA_gs.errbuf);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_WARNING_HANDLER - warn the user if the application hits
 *                    - a non-fatal error
 */

void PA_warning_handler(int test, const char *fmt, ...)
   {

/* reroute the warning handler if required - first time only */
    ONCE_SAFE(TRUE, NULL)
      {PFErrHand hook;

       hook = PA_GET_FUNCTION(PFErrHand, "warning_handler");
       if (hook != NULL)
          {PA_gs.warn_hook = hook;
	   SC_VA_START(fmt);
	   SC_VSNPRINTF(PA_gs.errbuf, MAXLINE, fmt);
	   SC_VA_END;
           PA_WARN(test, PA_gs.errbuf);};}
    END_SAFE;

    if (!test)
       return;

    PRINT(stdout,"\nWARNING: ");
    SC_VA_START(fmt);
    SC_VSNPRINTF(PA_gs.errbuf, MAXLINE, fmt);
    SC_VA_END;
    PRINT(stdout, "%s \n\n", PA_gs.errbuf);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_ERROR - the lowest level error handler
 *
 * #bind PA_error fortran() scheme()
 */

void PA_error(const char *msg)
   {

    PRINT(stdout, "%s \n\n", msg);

    LONGJMP(SC_gs.cpu, ABORT);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SET_DEFAULT_OFFSET - set the default file offset and
 *                       - return the old value
 *
 * #bind PA_set_default_offset fortran() scheme(pa-set-default-offset!)
 */

int PA_set_default_offset(int d)
   {int ov;

    ov = _PA.default_offset;
    _PA.default_offset = d;

    return(ov);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_DEFAULT_OFFSET - return the default file offset value
 *
 * #bind PA_get_default_offset fortran() scheme()
 */

int PA_get_default_offset(void)
   {

    return(_PA.default_offset);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
