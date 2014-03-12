/*
 * PAVARS.C - the variable data base control routines for PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SUB_SELECT - given a variable and an array of index specifications
 *               - compute the number of items pointed to, the
 *               - offset from the beginning of the data set, and the
 *               - stride through the data set
 */

void PA_sub_select(PA_variable *pp, C_array *arr, long *pitems,
		   long *poffs, long *pstr)
   {long i, j, n;
    long size, stride, partst, offset, nd;
    PA_set_index *dmap;
    PA_dimens *pv;
    void *pd;

/* get the information from the C_array */
    PM_ARRAY_CONTENTS(arr, PA_set_index, n, NULL, dmap);

/* resolve any unknown indexes (this could probably go higher up) */
    for (j = 0; j < n; j++)
        {if (dmap[j].index == PA_DELAY)
            {PA_CONNECT(pd, dmap[j].name, FALSE);
             PA_ERR((pd == NULL),
                    "NO WAY TO RESOLVE INDEX %s - PA_SUB_SELECT",
                    dmap[j].name);
             dmap[j].index = PA_find_index(pd,
                                           dmap[j].val,
                                           dmap[j].imax);};};
                 
/* find the offset and the stride
 * NOTE: this can be generalized to an array of offsets and strides for
 * m-dimensional subspaces of n-dimensional arrays
 */
    size   = 1L;
    stride = 1L;
    partst = 1L;
    offset = 0L;
    if (n > 0)
       {pv = PA_VARIABLE_DIMS(pp);
        for (j = 0; pv != NULL; pv = pv->next, j++)
            {nd = PA_VAR_DIM(pv);
             if (dmap[j].imax == 0)
                {if (dmap[j].name[0] == '*')
                    {size   *= nd;
                     stride  = partst;}
                 else
                    {offset += partst*dmap[j].index;
                     partst *= nd;};}
            
             else
                for (i = 0; i < n; i++)
                    {if (nd == dmap[i].imax)
                        {if (dmap[i].val == -HUGE)
                            {size  *= nd;
                             stride = partst;}
                         else
                            offset += partst*dmap[i].index;
                         partst *= nd;
                         break;};};};};

    *pitems = size;
    *poffs  = offset;
    *pstr   = stride;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GENERAL_SELECT - given a variable and an array of index specifications
 *                   - compute the number of items pointed to, the
 *                   - offset from the beginning of the data set, the
 *                   - array of strides through the data set, and the 
 *                   - array of maxes on the indices
 */

void PA_general_select(PA_variable *pp, C_array *arr, unsigned long *pitems,
		       unsigned long *pdims, unsigned long *poffs,
		       unsigned long *pstr, unsigned long *pmax)
   {long nd;
    unsigned long i, n;
    unsigned long size, partst, offset, dims;
    PA_set_index *dmap;
    PA_dimens *pv;

    size   = 1L;
    dims   = 0L;
    partst = 1L;
    offset = 0L;

/* if NULL C_array assume whole mesh selected */
    if (arr == NULL)
       {dims = pp->n_dimensions;
        for (pv = PA_VARIABLE_DIMS(pp); pv != NULL; pv = pv->next)
            {nd      = PA_VAR_DIM(pv);
             *pstr++ = size;
             size   *= nd;
             *pmax++ = nd;};}

/* get the information from the C_array */
    else
       {PM_ARRAY_CONTENTS(arr, PA_set_index, n, NULL, dmap);

        for (pv = PA_VARIABLE_DIMS(pp); pv != NULL; )
            {nd = PA_VAR_DIM(pv);
             for (i = 0; i < n; i++)
                 {if (nd == dmap[i].imax)
                     {if (dmap[i].val == -HUGE)
                         {size  *= dmap[i].imax;
                          dims++;
                          *pstr++ = partst;
                          *pmax++ = nd;}
                      else
                         offset += partst*dmap[i].index;
                      partst *= dmap[i].imax;
                      break;};};

             pv = pv->next;};};

    *pitems = size;
    *pdims  = dims;
    *poffs  = offset;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEFINITIONS - maintain the problem definition variables for PANACEA */

void PA_definitions(void)
   {int *P_Units;

    P_Units = &PA_gs.n_units;

/* this is the first time that this can be called with a guarantee that
 * all packages have been defined
 */
    if (PA_gs.vif == NULL)
       PA_gs.vif = PA_open("vif", "w+", TRUE);

    if (PA_gs.vif != NULL)
       PA_def_str(PA_gs.vif);

/* global problem definition variables */
    PA_inst_var("unit", SC_DOUBLE_S, NULL, NULL,
                SCOPE, DEFN, CLASS, REQU, ATTRIBUTE,
                P_Units, DIMENSION, UNITS);
    PA_inst_var("convrsn", SC_DOUBLE_S, NULL, NULL,
                SCOPE, DEFN, CLASS, REQU, ATTRIBUTE,
                P_Units, DIMENSION, UNITS);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_VARIABLES - maintain the table of memory managed and controlled access
 *              - variables needed to run PANACEA
 *              - variables not installed here have either unrestricted acces
 *              - throughout PANACEA (externs)
 *              - or are "local" to their respective packages and/or routines
 *              -
 *              - RESTART variables are required to be written to restart
 *              - dumps and are necessary for the code to restart
 *              - RUNTIME variables are computed at run time from other data
 *              - in the code system and as such are not required to restart
 *              - a problem
 *              -
 *              - The class of the variable determines how the code will treat
 *              - it.  REQUired variables MUST be in a restart file and they
 *              - MUST be written to a restart file or the code
 *              - will abort. OPTionaL variables will be read in if they exist
 *              - and will be written if their pointers are non-NULL.  DeMaND
 *              - variables will be read on demand by the CONNECT process and
 *              - will be written if the pointer is non-NULL.
 *              -
 *              - NO FLOATS SHOULD BE USED WITHOUT CAREFUL CONSIDERATION!!!
 *
 *              - physical variables MUST have their units specified in the
 *              - CGS sytem. The UNIT and CONVRSN arrays are used to specify
 *              - the internal and external systems of units respectively.
 */

void PA_variables(int flag)
   {PA_package *pck;
    PFPkgDefvar pck_def;

/* set up the unit system for the code */
    PA_def_units(flag);

/* boundary condition variables */
    PA_inst_var("initial-value-specifications", "PA_iv_specification",
                NULL, NULL, 
                SCOPE, RESTART, ATTRIBUTE,
                DIMENSION, UNITS);

/* install some panacea variables */
    PA_inst_var("plot-requests", "PA_plot_request",
                NULL, NULL, 
                SCOPE, RESTART, ATTRIBUTE,
                DIMENSION, UNITS);

/* have the package definers install their variables */
    for (pck = PA_gs.packages; pck != NULL; pck = pck->next)
        {if (pck->db_file != NULL)
            continue;
         pck_def = pck->defvar;
         if (pck_def != NULL)
            {PA_control_set(pck->name);
             (*pck_def)(pck);};};

/* reset to global controls */
    PA_control_set("global");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PROC_UNITS - process all of the units for all of the variables
 *               - in the database to generate the conversion factors
 *               - for the variables
 */

void PA_proc_units(void)
   {int id;
    long i;
    double conv_fac, unit_fac;
    char *ty;
    PA_variable *pp;
    pcons *pn, *pd, *pc, *nxt;

    for (i = 0; SC_hasharr_next(PA_gs.variable_tab, &i, NULL, &ty, (void **) &pp); i++)
        {if (ty[3] == 'p')                   /* skip the packages */
	    continue;

	 pn = PA_VARIABLE_UNIT_NUMER(pp);
	 pd = PA_VARIABLE_UNIT_DENOM(pp);

	 if ((pn == NULL) && (pd == NULL))
	    continue;

	 conv_fac = 1.0;
	 unit_fac = 1.0;
	 for (pc = pd; pc != NULL; pc = nxt)
	     {id = PA_LST_VAL(pc);
	      conv_fac *= PA_gs.convrsns[id];
	      unit_fac *= PA_gs.units[id];

	      nxt = (pcons *) pc->cdr;
	      SC_rl_pcons(pc, 1);};

	 conv_fac = 1.0/conv_fac;
	 unit_fac = 1.0/conv_fac;
	 for (pc = pn; pc != NULL; pc = nxt)
	     {id = PA_LST_VAL(pc);
	      conv_fac *= PA_gs.convrsns[id];
	      unit_fac *= PA_gs.units[id];

	      nxt = (pcons *) pc->cdr;
	      SC_rl_pcons(pc, 1);};

	 PA_VARIABLE_EXT_UNIT(pp)   = conv_fac;
	 PA_VARIABLE_INT_UNIT(pp)   = unit_fac;
	 PA_VARIABLE_UNIT_NUMER(pp) = NULL;
	 PA_VARIABLE_UNIT_DENOM(pp) = NULL;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_UNITS - handle the definitions of the unit systems of the packages
 *              - including the conversion arrays, UNIT and CONVRSN
 */

void PA_def_units(int flag)
   {PA_package *pck;
    PFPkgDefun pck_def;

    ONCE_SAFE(TRUE, NULL)

/* set the physical constants in CGS units */
       PA_physical_constants_cgs();

       PA_gs.n_units = 0;
       _PA.units     = NULL;

/* once panacea is defined as a package,
 * this call will be done in the following loop
 */
       PA_defunc();

/* have the package defuns install their units */
       for (pck = PA_gs.packages; pck != NULL; pck = pck->next)
	   {pck_def = pck->defun;
	    if (pck_def != NULL)
	       {PA_control_set(pck->name);
		(*pck_def)(pck);};};

/* reset to global controls */
       PA_control_set("global");

/* on restart let's avoid messing up the units */
       if (flag != NONE)
	  PA_set_conversions(flag);

/* set the physical constants in the internal system of units */
       PA_physical_constants_int();

    END_SAFE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_UNIT - define a derived unit in terms of previous units
 *             - if this has been defined already return the
 *             - index into UNIT and CONVRSN corresponding to the unit
 *             - otherwise expand UNIT and CONVRSN and return a new index
 *             - for this unit
 */

int PA_def_unit(double fac, ...)
   {int i, j, in, id, nn, nd, dm, rv, per_flag, fundamental;
    int num[50], den[50], *pa;
    PA_unit_spec *pu, *unit;

    SC_VA_START(fac);

    pa = num;
    nn = 0;
    nd = 0;
    i  = 0;
    per_flag = FALSE;
    while (TRUE)
       {dm = SC_VA_ARG(int);
        PA_ERR((dm >= PA_gs.n_units),
               "ILLEGAL UNIT SPECIFIER - PA_DEF_UNIT");
        if (dm == PER)
           {pa = den;
            nn = i;
            i  = 0;
            per_flag = TRUE;}
        else if (dm == UNITS)
           {if (per_flag)
               nd = i;
            else
               nn = i;
            break;}
        else
           pa[i++] = dm;};

    SC_VA_END;
 
    fundamental = ((nn + nd) == 0);

/* if it's not fundamental, check to see if this one exists already */
    if (!fundamental)
       {for (pu = _PA.units; pu != NULL; pu = pu->next)
            {if (pu->fundamental)
                continue;

             if (fac != pu->factor)
                continue;
             in = pu->n_num;
             id = pu->n_den;
             if ((nn != in) || (nd != id))
                continue;

             pa = pu->denominator;
             for (j = 0; j < nd; j++)
                 if (den[j] != pa[j])
                    break;
             if (j < nd)
                continue;

             pa = pu->numerator;
             for (j = 0; j < nn; j++)
                 if (num[j] != pa[j])
                    break;
             if (j < nn)
                continue;};

/* if so return it */
        if (pu != NULL)
	   {rv = pu->index;
            return(rv);};};

/* add a unit to the list */
    unit = CMAKE(PA_unit_spec);

    unit->index       = PA_gs.n_units++;
    unit->factor      = fac;
    unit->fundamental = fundamental;
    unit->n_num       = nn;
    unit->numerator   = CMAKE_N(int, nn);
    unit->n_den       = nd;
    unit->denominator = CMAKE_N(int, nd);

/* add fundamental units at the front */
    if (fundamental)
       {unit->next   = _PA.units;
        _PA.units = unit;}

/* add derived units at the end */
    else
       {for (pu = _PA.units; pu->next != NULL; pu = pu->next);
        unit->next = NULL;
        pu->next   = unit;};

    pa = unit->numerator;
    for (i = 0; i < nn; i++)
        *pa++ = num[i];

    pa = unit->denominator;
    for (i = 0; i < nd; i++)
        *pa++ = den[i];

    rv = unit->index;

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SET_CONVERSIONS - set the UNIT and CONVRSN arrays
 *                    - this forces the entire defined system of units
 *                    - to be consistent
 *                    - the argument FLAG tells which quantities are to be
 *                    - the basis for making units consistent
 *                    - if TRUE make UNIT and CONVRSN consistent with 
 *                    - themselves
 *                    - if FALSE make UNIT and CONVRSN consistent with their
 *                    - original definition
 *                    - NOTE: the application can mess this up badly!!!
 */

void PA_set_conversions(int flag)
   {int i, j, nn, nd, indx, *pa;
    double uf, cf;
    PA_unit_spec *pu;

    ONCE_SAFE(TRUE, NULL)
       if ((PA_gs.units == NULL) || (PA_gs.convrsns == NULL))
	  {PA_gs.units    = CMAKE_N(double, PA_gs.n_units);
	   PA_gs.convrsns = CMAKE_N(double, PA_gs.n_units);};
    END_SAFE;

    for (pu = _PA.units; pu != NULL; pu = pu->next)
        {indx = pu->index;

         if (flag)
            {uf = PA_gs.units[indx];
             cf = PA_gs.convrsns[indx];}
         else
            {uf = pu->factor;
             cf = uf;};

         nn = pu->n_num;
         nd = pu->n_den;

         if (pu->fundamental)
            {PA_gs.units[indx]    = uf;
             PA_gs.convrsns[indx] = cf;}
         else
            {pa = pu->numerator;
             for (j = 0; j < nn; j++)
                 {i   = pa[j];
                  uf *= PA_gs.units[i];
                  cf *= PA_gs.convrsns[i];};

             pa = pu->denominator;
             for (j = 0; j < nd; j++)
                 {i   = pa[j];
                  uf /= PA_gs.units[i];
                  cf /= PA_gs.convrsns[i];};

             PA_gs.units[indx]    = uf;
             PA_gs.convrsns[indx] = cf;};};

    return;}    

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PHYSICAL_CONSTANTS_INT - set up the physical constants in the internal
 *                           - system of units as defined
 *                           - by the PA_gs.units array
 *                           - e.g. length_internal = length_cgs*unit[CM]
 */

void PA_physical_constants_int(void)
   {

    PA_physical_constants_cgs();

/* Hbar in erg-sec */
    PM_c.hbar    *= (PA_gs.units[ERG]*PA_gs.units[SEC]);

/* Hbar*C in eV-cm */
    PM_c.hbarc   *= (PA_gs.units[EV]*PA_gs.units[CM]);

/* eV to erg */
    PM_c.ev_erg  *= (PA_gs.units[ERG]/PA_gs.units[EV]);

/* Avagadro's number */
    PM_c.n0      *= PA_gs.units[MOLE];

/* speed of light (cm/sec) */
    PM_c.c       *= (PA_gs.units[CM]/PA_gs.units[SEC]);

/* electron mass in eV */
    PM_c.m_e_ev  *= PA_gs.units[EV];

/* electron mass in g */
    PM_c.m_e     *= PA_gs.units[G];

/* atomic mass unit in g */
    PM_c.m_a     *= PA_gs.units[G];

/* Boltzman constant (erg/K) */
    PM_c.kboltz  *= (PA_gs.units[ERG]/PA_gs.units[K]);

/* electron charge in esu */
    PM_c.qe      *= PA_gs.units[Q];

/* Rydberg in eV */
    PM_c.ryd     *= PA_gs.units[EV];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PHYSICAL_CONSTANTS_EXT - set up the physical constants in the external
 *                           - system of units as defined
 *                           - by the PA_gs.convrsns array
 *                           - e.g. length_external = length_cgs*convrsns[CM]
 */

void PA_physical_constants_ext(void)
   {

    PA_physical_constants_cgs();

/* Hbar in erg-sec */
    PM_c.hbar    *= (PA_gs.convrsns[ERG]*PA_gs.convrsns[SEC]);

/* Hbar*C in eV-cm */
    PM_c.hbarc   *= (PA_gs.convrsns[EV]*PA_gs.convrsns[CM]);

/* eV to erg */
    PM_c.ev_erg  *= (PA_gs.convrsns[ERG]/PA_gs.convrsns[EV]);

/* Avagadro's number */
    PM_c.n0      *= PA_gs.convrsns[MOLE];

/* speed of light (cm/sec) */
    PM_c.c       *= (PA_gs.convrsns[CM]/PA_gs.convrsns[SEC]);

/* electron mass in eV */
    PM_c.m_e_ev  *= PA_gs.convrsns[EV];

/* electron mass in g */
    PM_c.m_e     *= PA_gs.convrsns[G];

/* atomic mass PA_gs.units in g */
    PM_c.m_a     *= PA_gs.convrsns[G];

/* Boltzman constant (erg/K) */
    PM_c.kboltz  *= (PA_gs.convrsns[ERG]/PA_gs.convrsns[K]);

/* electron charge in esu */
    PM_c.qe      *= PA_gs.convrsns[Q];

/* Rydberg in eV */
    PM_c.ryd     *= PA_gs.convrsns[EV];

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PHYSICAL_CONSTANTS_CGS - set up some useful conversion and physical
 *                            - constants in CGS units
 *                            - various compiler systems require explicit
 *                            - initializations
 */

void PA_physical_constants_cgs(void)
   {

    PM_physical_constants_cgs();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

