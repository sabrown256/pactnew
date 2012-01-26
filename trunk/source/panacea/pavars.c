/*
 * PAVARS.C - the variable data base control routines for PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

/* physical constants */

double
 alpha,              /* fine structure constant e^2/HbarC  - 7.297353080e-3 */
 c,                              /* speed of light (cm/sec) - 2.99792458e10 */
 Coulomb,                /* Coulomb in fundamental charges - 6.241506363e18 */
 e,                              /* electron charge in esu - 4.80320680e-10 */
 eV_erg,                                      /* eV to erg - 1.60217733e-12 */
 Gn,       /* Newtonian gravitational constant (cm^3/g-sec^2) - 6.673231e-8 */
 Hbar,                                  /* Hbar in erg-sec - 1.05457267e-27 */
 HbarC,                                  /* Hbar*C in eV-cm - 1.97327054e-5 */
 kBoltz,                     /* Boltzman constant in (erg/K) - 1.380658e-16 */
 K_eV,                                        /* Kelvin to eV 8.6173856e-05 */
 M_a,                            /* atomic mass unit in g - 1.660540210e-24 */
 M_e,                               /* electron mass in g - 9.109389754e-28 */
 M_e_eV,                              /* electron mass in eV - 5.10999065e5 */
 N0,                                   /* Avagadro's number - 6.02213665e23 */
 Ryd;                           /* (M_e*c^2*alpha^2)/2 in eV - 13.605698140 */

/* useful conversion constants */

double
 icm_g,                                /* inverse cm to g  - 3.51767578e-38 */
 g_icm,                                 /* g to inverse cm  - 2.84278615e37 */
 icm_eV,                                       /* inverse cm to eV  - HbarC */
 eV_icm,                                 /* eV toinverse cm  - 5.06772882e4 */
 icm_erg,                            /* inverse cm to erg  - 3.16152932e-17 */
 erg_icm;                             /* erg to inverse cm  - 3.16302617e16 */

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
   {unsigned long i, n;
    unsigned long size, partst, offset, dims, nd;
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
 *                            - system of units as defined by the unit array
 *                            - e.g. length_internal = length_cgs*unit[CM]
 */

void PA_physical_constants_int(void)
   {

    PA_physical_constants_cgs();

    Hbar    *= (PA_gs.units[ERG]*PA_gs.units[SEC]);         /* Hbar in erg-sec */
    HbarC   *= (PA_gs.units[EV]*PA_gs.units[CM]);           /* Hbar*C in eV-cm */
    eV_erg  *= (PA_gs.units[ERG]/PA_gs.units[EV]);                /* eV to erg */
    N0      *= PA_gs.units[MOLE];                         /* Avagadro's number */
    c       *= (PA_gs.units[CM]/PA_gs.units[SEC]);  /* speed of light (cm/sec) */
    M_e_eV  *= PA_gs.units[EV];                         /* electron mass in eV */
    M_e     *= PA_gs.units[G];                           /* electron mass in g */
    M_a     *= PA_gs.units[G];                 /* atomic mass PA_gs.units in g */
    kBoltz  *= (PA_gs.units[ERG]/PA_gs.units[K]); /* Boltzman constant (erg/K) */
    e       *= PA_gs.units[Q];                       /* electron charge in esu */
    Ryd     *= PA_gs.units[EV];                               /* Rydberg in eV */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PHYSICAL_CONSTANTS_EXT - set up the physical constants in the external
 *                            - system of PA_gs.unitss as defined by the PA_gs.convrsns
 *                            - array
 *                            - e.g. length_external = length_cgs*PA_gs.convrsns[CM]
 */

void PA_physical_constants_ext(void)
   {

    PA_physical_constants_cgs();

    Hbar    *= (PA_gs.convrsns[ERG]*PA_gs.convrsns[SEC]);         /* Hbar in erg-sec */
    HbarC   *= (PA_gs.convrsns[EV]*PA_gs.convrsns[CM]);           /* Hbar*C in eV-cm */
    eV_erg  *= (PA_gs.convrsns[ERG]/PA_gs.convrsns[EV]);                /* eV to erg */
    N0      *= PA_gs.convrsns[MOLE];                            /* Avagadro's number */
    c       *= (PA_gs.convrsns[CM]/PA_gs.convrsns[SEC]);  /* speed of light (cm/sec) */
    M_e_eV  *= PA_gs.convrsns[EV];                            /* electron mass in eV */
    M_e     *= PA_gs.convrsns[G];                              /* electron mass in g */
    M_a     *= PA_gs.convrsns[G];                    /* atomic mass PA_gs.units in g */
    kBoltz  *= (PA_gs.convrsns[ERG]/PA_gs.convrsns[K]); /* Boltzman constant (erg/K) */
    e       *= PA_gs.convrsns[Q];                          /* electron charge in esu */
    Ryd     *= PA_gs.convrsns[EV];                                  /* Rydberg in eV */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_PHYSICAL_CONSTANTS_CGS - set up some useful conversion and physical
 *                            - constants in CGS units
 *                            - various compiler systems require explicit
 *                            - initializations
 */

void PA_physical_constants_cgs(void)

/* fundamental constants */
   {c       = 2.997924580e+10;                   /* speed of light (cm/sec) */
    Hbar    = 1.054572670e-27;                           /* Hbar in erg-sec */
    alpha   = 7.297353020e-03;         /* fine structure constant e^2/HbarC */
    N0      = 6.022136650e+23;                         /* Avagadro's number */
    M_e     = 9.109389754e-28;                        /* electron mass in g */
    kBoltz  = 1.380658000e-16;                 /* Boltzman constant (erg/K) */
    Gn      = 6.673232000e-08;     /* gravitational constant (cm^3/g-sec^2) */
    Coulomb = 6.241506363e+18;           /* fundamental charges per Coulomb */

/* useful constants */
    eV_erg  = 1.0e7/Coulomb;                                   /* eV to erg */
    K_eV    = kBoltz/eV_erg;                                     /* K to eV */
    HbarC   = Hbar*c/eV_erg;                             /* Hbar*C in eV-cm */
    e       = sqrt(alpha*Hbar*c);                 /* electron charge in esu */
    M_a     = 1.0/N0;                              /* atomic mass unit in g */
    M_e_eV  = M_e*c*c/eV_erg;                        /* electron mass in eV */
    Ryd     = 0.5*M_e*c*c*alpha*alpha/eV_erg;              /* Rydberg in eV */

/* useful conversion factors */
    icm_g   = Hbar/c,                                    /* inverse cm to g */
    g_icm   = 1.0/icm_g;                                 /* g to inverse cm */
    icm_eV  = Hbar*c/eV_erg,                            /* inverse cm to eV */
    eV_icm  = 1.0/icm_eV,                               /* eV to inverse cm */
    icm_erg = Hbar*c,                                  /* inverse cm to erg */
    erg_icm = 1.0/icm_erg;                             /* erg to inverse cm */

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

