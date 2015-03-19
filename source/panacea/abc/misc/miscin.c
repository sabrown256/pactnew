/*
 * MISCIN.C - material initialization routines for A
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "miscin.h"

static material
 *cur_mat;

static component
 *cur_comp = NULL;

long
 *li;

float
 *fi;

double
 *ab,
 *tm,
 *zb;

/*--------------------------------------------------------------------------*/

/*                         MEMMORY MANAGEMENT ROUTINES                      */

/*--------------------------------------------------------------------------*/

/* MK_MATERIAL - allocate and initialize an instance of a
 *             - material
 */

static material *mk_material(char *s, double lrho, double ln, double a,
			     double z, double ltm, double lp)
   {material *mat;

    mat = CMAKE(material);

    mat->index       = 0;
    mat->name        = CSTRSAVE(s);
    mat->rho         = lrho;
    mat->n           = ln;
    mat->a           = a;
    mat->z           = z;
    mat->tm          = ltm;
    mat->p           = lp;
    mat->composition = NULL;

    return(mat);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MK_COMPONENT - allocate and initialize an instance of a component */

static component *mk_component(int z, double a, double frac, component *nxt)
   {component *comp;

    comp = CMAKE(component);

    comp->z      = z;
    comp->a      = a;
    comp->frac   = frac;
    comp->next = nxt;

    return(comp);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPOS - compute abar and zbar from
 *        - the composition or from any information supplied
 *        - return the following through pointers:
 *        -
 *        - abar - the material average atomic weight
 *        - zbar - the material average effective charge
 */

static int compos(component *cmp, double *abarp, double *zstarp)
   {double abars, zstars, tot, fr, zl, al;
    component *cmpa;

    abars  = 0.0;
    zstars = 0.0;
    tot    = SMALL;
    for (cmpa = cmp; cmpa != NULL; cmpa = cmpa->next)
        {fr = cmpa->frac;
         zl = cmpa->z;
         al = cmpa->a;

         tot    += fr;
         abars  += al*fr;
         zstars += zl*fr;};

/* normalize abar and zstar */
    *abarp  = abars/tot;
    *zstarp = zstars/tot;

    return(TRUE);}

/*--------------------------------------------------------------------------*/

/*                                  WORKERS                                 */

/*--------------------------------------------------------------------------*/

/* INTERN_MISC - intern the variables of the miscellaneous package
 *             - prior to writing the initial restart dump
 */

int intern_misc(void)
   {double *tc1;

    PA_control_set("misc");

    tc1 = CMAKE_N(double, N_zones);
    PA_INTERN(tc1, "cache-f-1");

    PA_INTERN(ab, "ab");
    PA_INTERN(zb, "zb");
    PA_INTERN(tm, "tm");

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* UPDATE_TEMP - update the temperatures */

int update_temp(PA_package *pck)
   {int j;
    double a, *tr, *tc1, *tc2, *tc3, *tc4;
    static int first = TRUE;

    if (first)
       PA_mark_space(pck);

/* test the connect of a RUNTIME, DMND variable */
    PA_CONNECT(tr, "tr", TRUE);
    PA_CONNECT(tc1, "cache-f-1", TRUE);
    PA_CONNECT(tc2, "cache-f-2", TRUE);
    PA_CONNECT(tc3, "cache-f-3", TRUE);
    PA_CONNECT(tc4, "cache-f-4", TRUE);

/* start timing */
    PA_mark_time(pck);

    for (j = frz; j <= lrz; j++)
        {tm[j] += dt*rpdv[j];};

/* adjust the long and float index test variables */
    for (j = frz; j < lrz; j++)
        {a = ((double) j)/PI;

         tc1[j] += 1.0;
         tc2[j] *= 2.0;
         tc3[j] -= 1.0;
         tc4[j] *= 0.5;

         fi[j] = t*sin(a)/dt;
         li[j] = cycle*((double) (lrz + 1))/((double) (j + 1));};

/* test the disconnect of a RUNTIME, DMND variable */
    PA_DISCONNECT("tr", tr);
    PA_DISCONNECT("cache-f-1", tc1);
    PA_DISCONNECT("cache-f-2", tc2);
    PA_DISCONNECT("cache-f-3", tc3);
    PA_DISCONNECT("cache-f-4", tc4);

    PA_accm_time(pck);

    if (first)
       {PA_accm_space(pck);
        first = FALSE;};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_MISC - initialize the temperatures */

int init_misc(PA_package *pck)
   {

    PA_CONNECT(tm, "tm", TRUE);

    PA_CONNECT(li, "lindex", TRUE);
    PA_CONNECT(fi, "findex", TRUE);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* MATERIALH - handle for material commands */

void materialh(void)
   {char *s;
    material *mat;
    double lrho, ln, a, z, ltm, lp;
    PM_part *parta;

    s    = PA_get_field("PART", "MATERIAL", REQU);
    lrho = PA_get_num_field("DENSITY", "MATERIAL", REQU);
    ln   = PA_get_num_field("ION DENSITY", "MATERIAL", OPTL);
    a    = PA_get_num_field("AVERAGE A", "MATERIAL", OPTL);
    z    = PA_get_num_field("AVERAGE Z", "MATERIAL", OPTL);
    ltm  = PA_get_num_field("TEMPERATURE", "MATERIAL", OPTL);
    lp   = PA_get_num_field("PRESSURE", "MATERIAL", OPTL);

    mat      = mk_material(s, lrho, ln, a, z, ltm, lp);
    cur_mat  = mat;
    cur_comp = NULL;

/*  associate mat with apropriate part (ie same name) */    
    for (parta = parts; parta != NULL; parta = parta->next)
        if (strcmp(parta->name, s) == 0)
           {parta->comp = mat;
            return;};

    PA_ERR(TRUE, "NO PART FOR THIS MATERIAL\n-> %s %g", s, lrho);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMPONENTH - handler for component command */

void componenth(void)
   {int z;
    double a, frac;

    z    = PA_get_num_field("Z", "COMPONENT", REQU);
    a    = PA_get_num_field("A", "COMPONENT", REQU);
    frac = PA_get_num_field("FRACTION", "COMPONENT", REQU);

    cur_comp = mk_component(z, a, frac, cur_comp);
    cur_mat->composition = cur_comp;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* C_PROPERTIES - compute the bulk material properties */

static int c_properties(material *mat, int m)
   {int j;
    double rhl, nel, nil, al, zl, tel, til, pel, pil;
    double conv;
    component *cmp;

    conv = PM_c.kboltz/PA_gs.convrsns[K];

    rhl = mat->rho;
    al  = mat->a;
    zl  = mat->z;
    nil = mat->n;
    nel = zl*nil;
    tel = mat->tm;
    til = tel;
    pel = mat->p/conv;
    pil = pel;
    cmp = mat->composition;

/* iff sufficient information compute: p = nkT */
    if ((tel != 0.0) && (nel != 0.0))
       pel = nel*tel;
    else if ((pel != 0.0) && (nel != 0.0))
       tel = pel/nel;
    else if ((pel != 0.0) && (tel != 0.0))
       nel = pel/tel;

    if ((til != 0.0) && (nil != 0.0))
       pil = nil*til;
    else if ((pil != 0.0) && (nil != 0.0))
       til = pil/nil;
    else if ((pil != 0.0) && (til != 0.0))
       nil = pil/til;

/* iff sufficient information compute: (a, z, y) */
    if (cmp != NULL)
       compos(cmp, &al, &zl);
    else
       {if (al == 0.0)
           {if ((rhl != 0.0) && (nil != 0.0))
               al = rhl*PM_c.n0/nil;};
        if (zl == 0.0)
           {if ((nil != 0.0) && (nel != 0.0))
               zl = nil/nel;
            else if ((rhl != 0.0) && (nel != 0.0))
               zl = rhl*PM_c.n0/(al*nel);};
        PA_ERR(((al == 0.0) || (zl == 0.0)),
             "MUST HAVE Ab, Zb, or COMPOSITION - C_PROPERTIES\n");};

/* iff sufficient information compute: (rho, ni, ne) */
    if (nil == 0.0)
       {PA_ERR(((rhl == 0.0) && (nel == 0.0)),
             "MUST HAVE RHO, NI, OR NE - C_PROPERTIES\n");

        if (rhl == 0.0)
           nil = nel/zl;
        else            
           nil = rhl*PM_c.n0/al;};
    rhl = al*nil/PM_c.n0 + nel*PM_c.m_e;

/* now iff sufficient information compute: p = nkT */
    if ((til != 0.0) && (nil != 0.0))
       pil = nil*til;
    else if ((pil != 0.0) && (nil != 0.0))
       til = pil/nil;

/* fill in the mesh arrays */
    for (j = frz; j <= lrz; j++)
        if (reg_map[j] == m)
           {rho[j] = rhl;
            tm[j]  = til;
            n[j]   = nil;
            p[j]   = pil*conv;
            ab[j]  = al;
            zb[j]  = zl;};

/* fill the material struct for the populations and associated maps */
    mat->rho = rhl;
    mat->n   = nil;
    mat->a   = al;
    mat->z   = zl;
    mat->tm  = til;
    mat->p   = pil*conv;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* LOAD_REG - do the material loading */

void load_reg(void)
   {int m, j;
    material *mat;
    PM_part *parta;

    ab  = CMAKE_N(double, N_zones);
    zb  = CMAKE_N(double, N_zones);
    tm  = CMAKE_N(double, N_zones);
    n   = CMAKE_N(double, N_zones);
    p   = CMAKE_N(double, N_zones);
    rho = CMAKE_N(double, N_zones);

    for (j = 0; j < N_zones; j++)
        {ab[j]  = 0.0;
         zb[j]  = 0.0;
         tm[j]  = 0.0;
         n[j]   = 0.0;
         p[j]   = 0.0;
         rho[j] = 0.0;};

/* collect some properties of the components and make maps */
    for (parta = parts; parta != NULL; parta = parta->next)
        if (((mat = (material *) parta->comp) != NULL) &&
            ((m = parta->reg) > 0))
           c_properties(mat, m);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
