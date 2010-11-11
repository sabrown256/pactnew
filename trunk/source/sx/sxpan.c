/*
 * SXPAN.C - PANACEA extensions in SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

object
 *_SX_var_tab;

PFInt
 SX_pan_data_hook = NULL;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PACKAGEP - function version of SX_PACKAGEP macro */

static object *_SXI_packagep(object *obj)
   {object *o;

    o = SX_PACKAGEP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PANVARP - function version of SX_PANVARP macro */

static object *_SXI_panvarp(object *obj)
   {object *o;

    o = SX_PANVARP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_SRCVARP - function version of SX_SOURCE_VARIABLEP macro */

static object *_SXI_srcvarp(object *obj)
   {object *o;

    o = SX_SOURCE_VARIABLEP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_IV_SPECP - function version of SX_IV_SPECIFICATIONP macro */

static object *_SXI_iv_specp(object *obj)
   {object *o;

    o = SX_IV_SPECIFICATIONP(obj) ? SS_t : SS_f;

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GPACKAGE - print a g_package */

static void _SX_wr_gpackage(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<PACKAGE|%s>", PACKAGE_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_PACKAGE - make and return a g_package */

object *SX_mk_package(PA_package *pck)
   {object *op;

    op = SS_mk_object(pck, G_PACKAGE, SELF_EV, pck->name,
		      _SX_wr_gpackage, SS_rl_object);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GVARIABLE - print a g_variable */

static void _SX_wr_gvariable(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<PANVAR|%s>", PANVAR_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_VARIABLE - make and return a g_variable */

object *SX_mk_variable(PA_variable *pp)
   {object *op;

    op = SS_mk_object(pp, G_PANVAR, SELF_EV, pp->name,
		      _SX_wr_gvariable, SS_rl_object);

    return(op);}

/*--------------------------------------------------------------------------*/

#if 0

/*--------------------------------------------------------------------------*/

/* SX_MK_SOURCE_VARIABLE - make and return a g_source_variable */

static object *SX_mk_source_variable(PA_src_variable *sv)
   {object *op;

    op = SS_mk_object(sv, G_SOURCE_VARIABLE, SELF_EV, sv->name,
		      _SX_wr_gsource_variable, SS_rl_object);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GSOURCE_VARIABLE - print a g_source_variable */

static void _SX_wr_gsource_variable(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<SOURCE_VARIABLE|%s>",
                              SOURCE_VARIABLE_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_IV_SPECIFICATION - make and return a g_iv_specification */

static object *SX_mk_iv_specification(PA_iv_specification *iv)
   {object *op;

    op = SS_mk_object(iv, G_IV_SPECIFICATION, SELF_EV, iv->name,
		      _SX_wr_giv_specification, SS_rl_object);

    return(op);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_WR_GIV_SPECIFICATION - print a g_iv_specification */

static void _SX_wr_giv_specification(object *obj, object *strm)
   {

    PRINT(SS_OUTSTREAM(strm), "<IV_SPECIFICATION|%s>",
                              IV_SPECIFICATION_NAME(obj));

    return;}

/*--------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------*/

/* _SXI_PACKAGE_NAME - return the package name */

static object *_SXI_package_name(object *obj)
   {object *rv;

    rv = SS_null;
    if (SX_PACKAGEP(obj))
       rv = SS_mk_string(PACKAGE_NAME(obj));
    else
       SS_error("BAD PACKAGE - _SXI_PACKAGE_NAME", obj);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PAN_SIMULATE - Scheme level call to PA_simulate
 *                   - arguments are :
 *                   -
 *                   -    ti - initial time
 *                   -    tf - final time
 */

static object *_SXI_pan_simulate(object *argl)
   {double t, ti, tf;
    double *pt, *pdt;
    int *pcy, *pnz;

    ti = 0.0;
    tf = 0.0;
    SS_args(argl,
            SC_DOUBLE_I, &ti,
            SC_DOUBLE_I, &tf,
            SC_DOUBLE_I, &_SX.dtf_min,
            SC_DOUBLE_I, &_SX.dtf_max,
            SC_DOUBLE_I, &_SX.dtf_inc,
            SC_STRING_I, &_SX.rsname,
            SC_STRING_I, &_SX.edname,
            SC_STRING_I, &_SX.ppname,
            SC_STRING_I, &_SX.gfname,
            0);

    ti *= unit[SEC]/convrsn[SEC];
    tf *= unit[SEC]/convrsn[SEC];

    pt  = (double *) SS_var_reference("current-time");
    pdt = (double *) SS_var_reference("current-timestep");
    pcy = (int *) SS_var_reference("current-cycle");
    pnz = (int *) SS_var_reference("number-of-zones");

    t = *pt;

    if ((t < ti) || (tf < t) || (tf < ti))
       {PRINT(stdout, "PROBLEM OUTSIDE OF TIME WINDOW: ");
        PRINT(stdout, "ti = %11.3e, t = %11.3e, tf = %11.3e", ti, t, tf);}

/* run the simulation */
    else
       PA_simulate(t, *pcy, *pnz, ti, tf, *pdt,
                   _SX.dtf_min, _SX.dtf_max, _SX.dtf_inc,
                   _SX.rsname, _SX.edname, _SX.ppname, _SX.gfname);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RUN_PACKAGE - execute the given package from Scheme level
 *                  - return the timestep and zone controlling it
 *                  - in a cons
 */

static object *_SXI_run_package(object *argl)
   {int cycle;
    double t, dt;
    char *pck_name;
    PA_package *pck;
    PFPkgMain pck_entry;
    object *numdt, *zondt, *o;

    SS_args(argl,
            G_PACKAGE, &pck,
            SC_DOUBLE_I, &t,
            SC_DOUBLE_I, &dt,
            SC_INT_I, &cycle,
            0);

    pck_entry = (PFPkgMain) pck->main;
    pck_name  = pck->name;

/* execute the package */
    if (pck_entry != NULL)
       {pck->p_t     = t;
        pck->p_dt    = dt;
        pck->p_cycle = cycle;

        PA_control_set(pck_name);
        _PA_allocate_mapping_space(pck);

        (*pck_entry)(pck);

        _PA_dump_package_mappings(pck, t, dt, cycle);};

/* reconnect the global controls */
    PA_control_set("global");

    numdt = SS_mk_float(pck->dt);
    zondt = SS_mk_integer((BIGINT) pck->dt_zone);

    o = SS_mk_cons(numdt, zondt);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ADVANCE_NAME - advance the given file family name */

static object *_SXI_advance_name(object *obj)
   {char *name;

    name = NULL;
    if (SS_stringp(obj))
       name = SS_STRING_TEXT(obj);

    if (name != NULL)
       PA_advance_name(name);

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_ADVANCE_TIME - set new dt from given value at Scheme level
 *                   - add old dt to t
 *                   - advance the cycle count
 */

static object *_SXI_advance_time(object *argl)
   {double dt, pck_dt, t1, t2, t3, tconv;
    double *pt, *pdt;
    int *pcy;
    PA_package *pck;
    int nargs;

    pt  = (double *) SS_var_reference("current-time");
    pdt = (double *) SS_var_reference("current-timestep");
    pcy = (int *) SS_var_reference("current-cycle");

    dt = *pdt;

    *pt += dt;
    (*pcy)++;

    nargs = SS_args(argl,
                    SC_DOUBLE_I, &t1,
                    SC_DOUBLE_I, &t2,
                    SC_DOUBLE_I, &t3,
                    0);

    tconv = unit[SEC]/convrsn[SEC];
    t1   *= tconv;
    t2   *= tconv;
    t3   *= tconv;
    switch (nargs)
       {case 0  : dt = HUGE;
                  for (pck = Packages; pck != NULL; pck = pck->next)
                      {pck_dt = pck->dt;
                       dt = min(pck_dt, dt);};
                  break;
        case 1  : dt = t1;
                  break;
        case 2  : dt = HUGE;
                  for (pck = Packages; pck != NULL; pck = pck->next)
                      {pck_dt = pck->dt;
                       dt = min(pck_dt, dt);};
                  dt = min(dt, t1);
                  dt = max(dt, t2);
                  break;
        default : dt = t1;
                  for (pck = Packages; pck != NULL; pck = pck->next)
                      {pck_dt = pck->dt;
                       dt = min(pck_dt, dt);};
                  dt = min(dt, t2);
                  dt = max(dt, t3);
                  break;};

    *pdt = dt;

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_INDEX_PTR - get the next dimension index pointer from the list
 *               - return special values if the dimension name matches
 *               -   "dimension"     - NULL
 *               -   "upper-lower"   - PA_DUL
 *               -   "offset-number" - PA_DON
 *               - also return NULL on error
 */

static int *_SX_index_ptr(object **pargl, char *msg)
   {int *rv;
    char *ds;
    object *obj;
    haelem *hp;

    SX_GET_STRING_FROM_LIST(ds, *pargl, msg);
    if (strcmp(ds, "dimension") == 0)
       rv = NULL;

    else if (strcmp(ds, "upper-lower") == 0)
       rv = PA_DUL;

    else if (strcmp(ds, "offset-number") == 0)
       rv = PA_DON;

    else
       {hp = SC_hasharr_lookup(PA_symbol_tab, ds);
	if (hp != NULL)
	   {if (strcmp(hp->type, "variable") == 0)
	       rv = (int *) hp->def;
	    else
	       rv = NULL;}
	else
	   rv = (int *) SS_var_reference(ds);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DEF_VAR - the most general variable installer for PANACEA
 *              - usage:
 *              -
 *              - (define-variable <name> <type> <init_val> <init_func>
 *              -            <attrribute_spec> ... ATTRIBUTE
 *              -            <dimension_spec> ... DIMENSION
 *              -            <unit_spec> ... UNITS
 *              -            [<ptr to data>])
 *              -
 *              - <attribute_spec> := <attribute_id> <attribute_val>
 *              - <dimension_spec> := <ptr to # values> |
 *              -                     PA_DUL <ptr to lower> <ptr to upper> |
 *              -                     PA_DON <ptr to offset> <ptr to # elem>
 *              - <unit_spec>      := <unit_index> | PER
 *              -
 *              - The valid attribute_id's are:
 *              -      SCOPE, CLASS, PERSIST, CENTER, ALLOCATION
 *              -
 *              - The valid SCOPE's are:
 *              -      DEFN, RESTART, DMND, RUNTIME, EDIT, SCRATCH
 *              -
 *              - The valid CLASS's are:
 *              -      REQU, OPTL, PSEUDO
 *              -
 *              - The valid PERSIST's are:
 *              -      REL, KEEP, CACHE_F, CACHE_R
 *              -
 *              - The valid CENTER's are:
 *              -      Z_CENT, N_CENT, F_CENT, E_CENT, U_CENT
 *              -
 *              - The valid ALLOCATION's are:
 *              -      STATIC, DYNAMIC
 *              -
 *              - The pre-defined unit_index's are:
 *              -      RAD, STER, MOLE, Q, CM, SEC, G, EV, K, ERG, CC
 *              -
 *              - No attributes are required to be set (ATTRIBUTE must appear
 *              - in the arg list) and the default values are:
 *              -      RUNTIME, OPTL, REL, U_CENT, DYNAMIC
 */
 
static object *_SXI_def_var(object *argl)
   {int i, dm, *pv, *mini, *maxi, meth;
    int enough, at, vattr[N_ATTRIBUTES];
    char *vname, *vtype, *s;
    void *viv;
    void (*vif)(void *p, long sz, char *s);
    haelem *hp;
    SC_address addr;
    object *oviv, *ovif, *obj;
    PA_variable *pp;
    PA_dimens *vdims, *next, *prev;
    pcons *nu, *du, *nxt, *prv;

    dm  = 0;
    at  = 0;
    vif = NULL;
    viv = NULL;

    SS_args(argl,
            SC_STRING_I, &vname,
            SC_STRING_I, &vtype,
            SS_OBJECT_I, &oviv,
            SS_OBJECT_I, &ovif,
            0);

    for (i = 0; i < 4; i++, argl = SS_cdr(argl));

    if (!SS_nullobjp(oviv))
       {if (!SS_numbp(oviv))
           {s = SS_get_string(oviv);
            hp = SC_hasharr_lookup(PA_symbol_tab, s);
            if (strcmp(hp->type, "variable") == 0)
               viv = hp->def;};};

    if (!SS_nullobjp(ovif))
       {s = SS_get_string(ovif);
        hp = SC_hasharr_lookup(PA_symbol_tab, s);
        if (strcmp(hp->type, "procedure") == 0)
           {addr.memaddr = hp->def;
            vif          = (void (*)(void *p, long sz, char *s)) addr.funcaddr;};};

/* make the variable hash table if it doesn't exist yet */
    if (PA_variable_tab == NULL)
       PA_variable_tab = SC_make_hasharr(HSZLARGE, DOC, SC_HA_NAME_KEY);

    vattr[0] = RUNTIME;
    vattr[1] = OPTL;
    vattr[2] = REL;
    vattr[3] = U_CENT;
    vattr[4] = DYNAMIC;
    enough   = FALSE;
    while (!enough)
       {SX_GET_INTEGER_FROM_LIST(at, argl,
                                 "CAN'T GET ATTRIBUTE - _SXI_DEF_VAR");
        switch (at)
           {case SCOPE      :
                 SX_GET_INTEGER_FROM_LIST(vattr[0], argl,
                                          "CAN'T GET ATTRIBUTE VALUE - _SXI_DEF_VAR");
                 break;
            case CLASS      :
                 SX_GET_INTEGER_FROM_LIST(vattr[1], argl,
                                          "CAN'T GET ATTRIBUTE VALUE - _SXI_DEF_VAR");
                 break;
            case PERSIST    :
                 SX_GET_INTEGER_FROM_LIST(vattr[2], argl,
                                          "CAN'T GET ATTRIBUTE VALUE - _SXI_DEF_VAR");
                 break;
            case CENTER     :
                 SX_GET_INTEGER_FROM_LIST(vattr[3], argl,
                                          "CAN'T GET ATTRIBUTE VALUE - _SXI_DEF_VAR");
                 break;
            case ALLOCATION :
                 SX_GET_INTEGER_FROM_LIST(vattr[4], argl,
                                          "CAN'T GET ATTRIBUTE VALUE - _SXI_DEF_VAR");
                 break;
            case ATTRIBUTE  :
                 enough = TRUE;
                 break;
            default         :
                 PA_ERR(TRUE,
                        "BAD ATTRIBUTE %d - _SXI_DEF_VAR",
                        at);};};

/* get the dimensions */
    vdims = NULL;
    while (TRUE)
       {maxi = _SX_index_ptr(&argl, "BAD DIMENSION NAME - _SXI_DEF_VAR");
        if (maxi == NULL)
           break;

        if (maxi == PA_DUL)
           {mini = _SX_index_ptr(&argl, "BAD LOWER INDEX - _SXI_DEF_VAR");
            maxi = _SX_index_ptr(&argl, "BAD UPPER INDEX - _SXI_DEF_VAR");
            meth = *PA_DUL;}

        else if (maxi == PA_DON)
           {mini = _SX_index_ptr(&argl, "BAD OFFSET - _SXI_DEF_VAR");
            maxi = _SX_index_ptr(&argl, "BAD NUMBER - _SXI_DEF_VAR");
            meth = *PA_DON;}

        else
           {mini = &Zero_I;
            meth = *PA_DON;};

        next = _PA_mk_dimens(mini, maxi, meth);
        if (vdims == NULL)
           vdims = next;
        else
           prev->next = next;
        prev = next;};

/* get the units */
    nu = NULL;
    while (TRUE)
       {SX_GET_INTEGER_FROM_LIST(dm, argl,
                                 "BAD UNIT - _SXI_DEF_VAR");
        if ((dm == PER) || (dm == UNITS))
           break;
        pv  = FMAKE(int, "_SXI_DEF_VAR:pv");
        *pv = dm;
        nxt = SC_mk_pcons(SC_INT_P_S, pv, SC_PCONS_P_S, NULL);
        if (nu == NULL)
           nu = nxt;
        else
           prv->cdr = (void *) nxt;
        prv = nxt;};

    du = NULL;
    if (dm != UNITS)
       while (TRUE)
          {SX_GET_INTEGER_FROM_LIST(dm, argl,
                                    "BAD DENOMINATOR UNIT - _SXI_DEF_VAR");
           if (dm == UNITS)
              break;

           pv  = FMAKE(int, "_SXI_DEF_VAR:pv");
           *pv = dm;
           nxt = SC_mk_pcons(SC_INT_P_S, pv, SC_PCONS_P_S, NULL);
           if (du == NULL)
              du = nxt;
           else
              prv->cdr = (void *) nxt;
           prv = nxt;};

    pp = _PA_mk_variable(vname, vdims, viv, vif,
                         vattr, vtype,
                         1.0, 1.0, nu, du, NULL);

    PA_VARIABLE_DATA(pp) = NULL;

    SC_hasharr_install(PA_variable_tab, vname, pp, PAN_VARIABLE, TRUE, TRUE);

/* install scalars as implicit commands */
    if ((pp->n_dimensions == 0) && (PA_commands != NULL))
       {void *vaddr;
        int itype;

	itype = 0;
        if (strcmp(vtype, SC_INT_S) == 0)
           itype = SC_INT_I;
        else if (strcmp(vtype, SC_DOUBLE_S) == 0)
           itype = SC_DOUBLE_I;
        else if (strcmp(vtype, SC_CHAR_S) == 0)
           itype = SC_CHAR_I;
        else
           PA_WARN(TRUE,
                   "TYPE %s UNSUPPORTED AS IMPLICIT COMMAND - _SXI_DEF_VAR",
                   vtype);

        PA_CONNECT(vaddr, vname, FALSE);

        PA_inst_c(vname, vaddr, itype, 0, (PFVoid) PA_pshand, PA_sargs);};

    obj = SX_mk_variable(pp);

    return(obj);}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_RD_RESTART - read a restart dump from Scheme level */

static object *_SXI_rd_restart(object *argl)
   {int convs;
    char *name;

    name  = NULL;
    convs = NONE;
    SS_args(argl,
            SC_STRING_I, &name,
            SC_INT_I, &convs,
            0);

    if (name == NULL)
       SS_error("BAD FILE NAME - _SXI_RD_RESTART", argl);

    PA_rd_restart(name, convs);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_LIST_PAN_PCK - return a list of PANACEA packages */

static object *_SXI_list_pan_pck(void)
   {object *obj, *lst, *lst_nxt;
    PA_package *pck;
    char *name;
    haelem *hp;

    lst     = SS_null;
    lst_nxt = SS_null;
    for (pck = Packages; pck != NULL; pck = pck->next)
        {name = pck->name;
         if ((hp = SC_hasharr_lookup(SS_symtab, name)) != NULL)
            obj = (object *) hp->def;
         else
            {obj = SS_mk_variable(name, SS_null);
             SS_UNCOLLECT(obj);
             if (SC_hasharr_install(SS_symtab, name, obj, SS_POBJECT_S, TRUE, TRUE) == NULL)
                SS_error("CAN'T INSTALL PACKAGE - SX_INTERN_PACKAGE", obj);};

/* put these in the top level environment frame
 * not the best place but SS_Env won't last through most expression
 * evaluations
 */
         SS_def_var(obj,
                    SX_mk_package(pck),
                    SS_Global_Env);

         SS_end_cons(lst, lst_nxt, obj);};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_INTERN_PACKAGES - make a Scheme variable for each PANACEA
 *                      - package and return them in a list
 */

static object *_SXI_intern_packages(void)
   {object *obj, *lst, *lst_nxt;
    PA_package *pck;
    char *name;
    haelem *hp;

    lst     = SS_null;
    lst_nxt = SS_null;
    for (pck = Packages; pck != NULL; pck = pck->next)
        {name = pck->name;
         if ((hp = SC_hasharr_lookup(SS_symtab, name)) != NULL)
            obj = (object *) hp->def;
         else
            {obj = SS_mk_variable(name, SS_null);
             SS_UNCOLLECT(obj);
             if (SC_hasharr_install(SS_symtab, name, obj, SS_POBJECT_S, TRUE, TRUE) == NULL)
                SS_error("CAN'T INSTALL PACKAGE - SX_INTERN_PACKAGE", obj);};

/* put these in the top level environment frame
 * not the best place but SS_Env won't last through most expression
 * evaluations
 */
         SS_def_var(obj,
                    SX_mk_package(pck),
                    SS_Global_Env);

         SS_end_cons(lst, lst_nxt, obj);};

    return(lst);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_INIT_PROBLEM - execute PA_init_system at Scheme level
 *                   - this should happen after the dump has been read
 */

static object *_SXI_init_problem(object *argl)
   {int nc;
    double t, dt;
    char *edname, *ppname, *gfname;

    t  = 0.0;
    dt = 0.0;
    nc = 0;
    edname = NULL;
    ppname = NULL;
    gfname = NULL;
    SS_args(argl,
            SC_DOUBLE_I, &t,
            SC_DOUBLE_I, &dt,
            SC_INT_I, &nc,
            SC_STRING_I, &edname,
            SC_STRING_I, &ppname,
            SC_STRING_I, &gfname,
            0);

    t  *= unit[SEC]/convrsn[SEC];
    dt *= unit[SEC]/convrsn[SEC];

/* read the source files, initialize the packages */
    PA_init_system(t, dt, nc, edname, ppname, gfname);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_INST_COM - execute PA_inst_com at Scheme level */

static object *_SXI_inst_com(void)
   {

    PA_inst_com();

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_READH - execute PA_readh at Scheme level */

static object *_SXI_readh(object *argl)
   {char *name;

    name = NULL;
    SS_args(argl,
            SC_STRING_I, &name,
            0);

    PA_readh(name);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_PAN_CMMND - execute the command specified in the given string */

static object *_SXI_pan_cmmnd(object *argl)
   {char *s, *token, *t;
    haelem *hp;
    PA_command *cp;
    object *o;

    s = NULL;
    SS_args(argl,
            SC_STRING_I, &s,
            0);

    o     = SS_f;
    token = SC_strtok(s, " \n\r\t/(", t);
    if (token != NULL)
       {hp = SC_hasharr_lookup(PA_commands, token);
        if (hp != NULL)
           {cp = (PA_command *) hp->def;
            (*(cp->handler))(cp);};

        o = SS_t;};

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_FIN_SYSTEM - execute fin_system at Scheme level */

static object *_SXI_fin_system(object *argl)
   {int nz, cy;

    nz = 0;
    cy = 0;
    SS_args(argl,
            SC_INT_I, &nz,
            SC_INT_I, &cy,
            0);

    PA_fin_system(nz, cy, FALSE);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DUMP_PP - execute PA_dump_pp and PA_file_mon at Scheme level */

static object *_SXI_dump_pp(object *argl)
   {double t, dt;
    int cy;
    char *ed, *pp, *gf;

    t  = 0.0;
    dt = 0.0;
    cy = 0;
    ed = NULL;
    pp = NULL;
    gf = NULL;
    SS_args(argl,
            SC_DOUBLE_I, &t,
            SC_DOUBLE_I, &dt,
            SC_INT_I, &cy,
            SC_STRING_I, &ed,
            SC_STRING_I, &pp,
            SC_STRING_I, &gf,
            0);

    PA_dump_pp(t, dt, cy);

    PA_file_mon(ed, pp, gf);

    return(SS_t);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DB_NUMERIC_DATA - connect to numeric data in PANACEA data base
 *                      -
 *                      - (pa-variable->pm-array name)
 */

static object *_SXI_db_numeric_data(object *obj)
   {int i, n, id;
    char *name, *type;
    double *dp;
    C_array *arr;
    PA_variable *pp;
    void *pd;
    object *rv;

    rv   = SS_null;
    name = NULL;
    SS_args(obj,
            SC_STRING_I, &name,
            0);

/* if no other arguments, read the variable */
    pp = PA_INQUIRE_VARIABLE(name);
    if (pp == NULL)
       SS_error("VARIABLE NOT FOUND - _SXI_DB_NUMERIC_DATA", obj);

    else
       {PA_CONNECT(pd, name, FALSE);
	n    = PD_entry_number(pp->desc);
	type = PD_entry_type(pp->desc);

/* all arrays will be REALS */
	if (strcmp(type, SC_DOUBLE_S) == 0)
	   arr = PM_make_array(SC_DOUBLE_S, n, pd);
	else
	   {arr = PM_make_array(SC_DOUBLE_S, n, NULL);
	    dp  = (double *) arr->data;

	    id  = SC_type_id(type);

/* fixed point types */
	    if (id == SC_SHORT_I)
	       {short *pv;
	        pv = (short *) pd;
		for (i = 0; i < n; i++)
		    *dp++ = *pv++;}
	    else if (id == SC_INT_I)
	       {int *pv;
		pv = (int *) pd;
		for (i = 0; i < n; i++)
		    *dp++ = *pv++;}
	    else if (id == SC_LONG_I)
	       {long *pv;
		pv = (long *) pd;
		for (i = 0; i < n; i++)
		    *dp++ = *pv++;}
	    else if (id == SC_LONG_LONG_I)
	       {long long *pv;
		pv = (long long *) pd;
		for (i = 0; i < n; i++)
		    *dp++ = *pv++;}

/* floating point types */
	    else if (id == SC_FLOAT_I)
	       {float *pv;
		pv = (float *) pd;
		for (i = 0; i < n; i++)
		    *dp++ = *pv++;}
	    else if (id == SC_LONG_DOUBLE_I)
	       {long double *pv;
		pv = (long double *) pd;
		for (i = 0; i < n; i++)
		    *dp++ = *pv++;}

	    else if (id == SC_CHAR_I)
	       {char *pv;
		pv = (char *) pd;
		for (i = 0; i < n; i++)
		    *dp++ = *pv++;}
	    else
	       SS_error("BAD DATA TYPE - _SXI_DB_NUMERIC_DATA", obj);};

	rv = SX_mk_C_array(arr);};

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_WR_RESTART - write a restart dump from Scheme level */

static object *_SXI_wr_restart(object *argl)
   {char *name;

    name = NULL;
    SS_args(argl,
            SC_STRING_I, &name,
            0);

    if (name == NULL)
       PA_wr_restart("state.tmp");
    else
       PA_wr_restart(name);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_PANACEA_FUNCS - install the PANACEA extensions to Scheme */
 
void SX_install_panacea_funcs(void)
   {

    SS_install("pa-advance-name",
               "Advance the given file name",
               SS_sargs,
               _SXI_advance_name, SS_PR_PROC);

    SS_install("pa-advance-time",
               "Sets the problem time and time step",
               SS_nargs,
               _SXI_advance_time, SS_PR_PROC);

    SS_install("pa-command",
               "Process a command line string",
               SS_nargs,
               _SXI_pan_cmmnd, SS_PR_PROC);

    SS_install("pa-define-variable",
               "Define a new variable to the database",
               SS_nargs,
               _SXI_def_var, SS_PR_PROC);

    SS_install("pa-describe-entity",
               "Displays a description of a panacea package or variable",
               SS_sargs,
               _SXI_desc_pan, SS_PR_PROC);

    SS_install("pa-display",
               "Displays a panacea object in nice form",
               SS_sargs,
               _SXI_display_pan_object, SS_PR_PROC);

    SS_install("pa-finish-simulation",
               "Gracefully conclude a numerical simulation",
               SS_nargs,
               _SXI_fin_system, SS_PR_PROC);

    SS_install("pa-init-simulation",
               "Initialize a numerical simulation",
               SS_nargs,
               _SXI_init_problem, SS_PR_PROC);

    SS_install("pa-install-commands",
               "Install the commands from all packages",
               SS_zargs,
               _SXI_inst_com, SS_PR_PROC);

    SS_install("pa-intern-packages",
               "Returns a list of variables which are bound to the PANACEA packages",
               SS_zargs,
               _SXI_intern_packages, SS_PR_PROC);

    SS_install("pa-iv-specification?",
               "Returns #t if the object is a PANACEA initial value specification, and #f otherwise",
               SS_sargs,
               _SXI_iv_specp, SS_PR_PROC);

    SS_install("pa-list-packages",
               "List current panacea packages",
               SS_zargs,
               _SXI_list_pan_pck, SS_PR_PROC);

    SS_install("pa-package?",
               "Returns #t if the object is a PANACEA package, and #f otherwise",
               SS_sargs,
               _SXI_packagep, SS_PR_PROC);

    SS_install("pa-package-name",
               "Returns a the name of the PANACEA package",
               SS_sargs,
               _SXI_package_name, SS_PR_PROC);

    SS_install("pa-read-commands",
               "Read the commands in the specified source file",
               SS_nargs,
               _SXI_readh, SS_PR_PROC);

    SS_install("pa-read-state-file",
               "Reads the named state file and does the specified conversions",
               SS_nargs,
               _SXI_rd_restart, SS_PR_PROC);

    SS_install("pa-run-package",
               "Executes the given package and returns its time step and controlling zone",
               SS_nargs,
               _SXI_run_package, SS_PR_PROC);

    SS_install("pa-save-to-pp",
               "Save the data for the output requests from this cycle",
               SS_nargs,
               _SXI_dump_pp, SS_PR_PROC);

    SS_install("pa-simulate",
               "Runs a simulation from Ti to Tf",
               SS_nargs,
               _SXI_pan_simulate, SS_PR_PROC);

    SS_install("pa-source-variable?",
               "Returns #t if the object is a PANACEA source variable, and #f otherwise",
               SS_sargs,
               _SXI_srcvarp, SS_PR_PROC);

    SS_install("pa-variable?",
               "Returns #t if the object is a PANACEA variable, and #f otherwise",
               SS_sargs,
               _SXI_panvarp, SS_PR_PROC);

    SS_install("pa-variable->pm-array",
               "Save the data for the output requests from this cycle",
               SS_sargs,
               _SXI_db_numeric_data, SS_PR_PROC);

    SS_install("pa-write-state-file",
               "Write a state file",
               SS_sargs,
               _SXI_wr_restart, SS_PR_PROC);

    SS_define_constant(0,
                       "dimension",     SC_STRING_I, "dimension",
                       "upper-lower",   SC_STRING_I, "upper-lower",
                       "offset-number", SC_STRING_I, "offset-number",
                       "units",         SC_INT_I, -1,
                       "per",           SC_INT_I, -2,
                       "attribute",     SC_INT_I, 102,
                       NULL);

    SS_define_constant(0,
                       "scope",         SC_INT_I, 97,
                       "defn",          SC_INT_I, -1,
                       "restart",       SC_INT_I, -2,
                       "demand",        SC_INT_I, -3,
                       "runtime",       SC_INT_I, -4,
                       "edit",          SC_INT_I, -5,
                       "scratch",       SC_INT_I, -6,
                       NULL);

    SS_define_constant(0,
                       "class",         SC_INT_I, 98,
                       "required",      SC_INT_I, 1,
                       "optional",      SC_INT_I, 2,
                       "pseudo",        SC_INT_I, 3,
                       NULL);

    SS_define_constant(0,
                       "persist",       SC_INT_I, 99,
                       "release",       SC_INT_I, -10,
                       "keep",          SC_INT_I, -11,
                       "cache",         SC_INT_I, -12,
                       NULL);

    SS_define_constant(0,
                       "center",        SC_INT_I, 100,
                       "zone-centered", SC_INT_I, -1,
                       "node-centered", SC_INT_I, -2,
                       "face-centered", SC_INT_I, -3,
                       "edge-centered", SC_INT_I, -4,
                       "uncentered",    SC_INT_I, -5,
                       NULL);

    SS_define_constant(0,
                       "allocation",    SC_INT_I, 101,
                       "static",        SC_INT_I, -100,
                       "dynamic",       SC_INT_I, -101,
                       NULL);

    SS_install_cv("radian",    &PA_radian,          SC_INT_I);
    SS_install_cv("steradian", &PA_steradian,       SC_INT_I);
    SS_install_cv("mole",      &PA_mole,            SC_INT_I);
    SS_install_cv("Q",         &PA_electric_charge, SC_INT_I);
    SS_install_cv("cm",        &PA_cm,              SC_INT_I);
    SS_install_cv("sec",       &PA_sec,             SC_INT_I);
    SS_install_cv("g",         &PA_gram,            SC_INT_I);
    SS_install_cv("eV",        &PA_eV,              SC_INT_I);
    SS_install_cv("K",         &PA_kelvin,          SC_INT_I);
    SS_install_cv("erg",       &PA_erg,             SC_INT_I);
    SS_install_cv("cc",        &PA_cc,              SC_INT_I);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
