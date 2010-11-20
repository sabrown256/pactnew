/*
 * PACCESS.C - routines that access database variables
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

/* PA_SET_UP_SCALAR - initialize a database scalar
 *                  - the order in which to attempt initialization is
 *                  -   (1) iv/source variable
 *                  -   (2) application supplied function
 *                  -   (3) application supplied constant
 */

#define PA_SET_UP_SCALAR(name, type, d, val, fun)                            \
    {PA_iv_specification *sp;                                                \
     if (d != NULL)                                                          \
        {sp = PA_get_iv_source(name);                                        \
         if (sp != NULL)                                                     \
            {double *sd;                                                     \
             sd = sp->data;                                                  \
             PA_ERR((sp->num != 1),                                          \
                    "SOURCE VARIABLE NOT SCALAR - PA_SET_UP_SCALAR");        \
             *(type *) d = *(type *) sd;}                                    \
         else if (fun != NULL)                                               \
            (*fun)(d, 1L, name);                                             \
         else if (val != NULL)                                               \
            *(type *) d = *(type *) val;};}

SC_thread_lock
 PA_access_lock = SC_LOCK_INIT_STATE,
 PA_track_lock  = SC_LOCK_INIT_STATE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INQUIRE_VARIABLE - return a PA_variable pointer from
 *                     - the PA_variable_tab
 */

PA_variable *PA_inquire_variable(char *x)
   {char *name, s[MAXLINE];
    PA_package *pck;
    PA_variable *vr;

    if (PA_name_spaces)
       {pck  = PA_current_package();
	name = pck->name;
	snprintf(s, MAXLINE, "%s-%s", name, x);}
    else
       strcpy(s, x);

    vr = (PA_variable *) SC_hasharr_def_lookup(PA_variable_tab, s);

    return(vr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_EQUIVALENCE - proclaim that the given pointer's address is to
 *                - contain the same content as the named variable's
 *                - data
 */

void PA_equivalence(void *vr, char *s)
   {PA_variable *pp;
    int pall;
    long sz;
    defstr *dp;
    size_t nb;
    void *pdata;
    pcons *pc;
    char bf[MAXLINE];

    pp = PA_inquire_variable(s);
    PA_ERR((pp == NULL),
           "VARIABLE %s NOT IN DATA BASE - PA_EQUIVALENCE", s);

    pdata = PA_VARIABLE_DATA(pp);
    pall  = PA_VARIABLE_ALLOCATION(pp);

    PA_ERR((pall != STATIC),
           "EQUIVALENCE UNDEFINED FOR NON-STATICS - PA_EQUIVALENCE");

/* copy the data over now */
    sz = PA_VARIABLE_SIZE(pp);
    dp = PA_VARIABLE_TYPE(pp);

    nb = max(1, sz)*dp->size;
    memcpy(vr, pdata, nb);

/* search the reference list for this pointer */
    pc = PA_VARIABLE_REFERENCE_LIST(pp);
    for (; pc != NULL; pc = (pcons *) pc->cdr)
        {if (vr == (void *) pc->car)
	    break;};

/* if this one is already on the reference list don't add it again */
    if (pc == NULL)
       {snprintf(bf, MAXLINE, "%s *", (char *) PA_VARIABLE_TYPE(pp));
	pc = SC_mk_pcons(bf, vr,
			 SC_PCONS_P_S, PA_VARIABLE_REFERENCE_LIST(pp));

	PA_VARIABLE_REFERENCE_LIST(pp) = pc;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SET_EQUIV - set the value of every body who is equivalences to the
 *              - named variable
 */

void PA_set_equiv(char *s, void *vl)
   {PA_variable *pp;
    int pall;
    long sz;
    defstr *dp;
    size_t nb;
    void *pdata, *vr;
    pcons *pc;

    pp = PA_inquire_variable(s);
    PA_ERR((pp == NULL),
           "VARIABLE %s NOT IN DATA BASE - PA_SET_EQUIV", s);

    pdata = PA_VARIABLE_DATA(pp);
    pall  = PA_VARIABLE_ALLOCATION(pp);

    PA_ERR((pall != STATIC),
           "EQUIVALENCE UNDEFINED FOR NON-STATICS - PA_SET_EQUIV");

    sz = PA_VARIABLE_SIZE(pp);
    dp = PA_VARIABLE_TYPE(pp);
    nb = max(1, sz)*dp->size;

/* search the reference list for this pointer */
    pc = PA_VARIABLE_REFERENCE_LIST(pp);
    for (; pc != NULL; pc = (pcons *) pc->cdr)
        {vr = (void *) pc->car;

/* copy the data over now */
	 memcpy(vr, vl, nb);};

    memcpy(pdata, vl, nb);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_HANDLE_REQUIRED - obtain and return a pointer to a suitably
 *                     - initialized REQU variable
 *                     - REQU variables CANNOT have a NULL data pointer
 *                     - that is a fatal error
 */

static void *_PA_handle_required(PA_variable *pp, long psz)
   {char *pname;
    void *pdata;

    pdata = PA_VARIABLE_DATA(pp);
    pname = PA_VARIABLE_NAME(pp);
    if ((pdata == NULL) && (psz > 0))
       pdata = _PA_init_space(pp, psz);

    PA_ERR((pdata == NULL),
           "FAILED TO INITIALIZE VARIABLE %s - _PA_HANDLE_REQUIRED",
           pname);

    return(pdata);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_HANDLE_OPTIONAL - handle all cases for OPTL variables
 *                     - OPTL variables may correctly have a NULL
 *                     - data pointer if the type is not primitive or
 *                     - the size is zero
 */

static void *_PA_handle_optional(PA_variable *pp, long psz)
   {void *pdata;

    pdata = PA_VARIABLE_DATA(pp);
    if ((pdata == NULL) && (psz > 0))
       pdata = _PA_init_space(pp, psz);

    return(pdata);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_LOAD_SPACE - load up the given space with data */

static void _PA_load_space(PA_variable *pp, long psz)
   {int ssz, ppers, ret, pall;
    double t, dt;
    char *pname, *pfn;
    void (*pfun)(void *p, long sz, char *s);
    void *pval, *pdata;
    PA_package *pck;
    PA_src_variable *svp;
    PDBfile *pfile;

    pck = PA_current_package();

    pname = PA_VARIABLE_NAME(pp);
    pdata = PA_VARIABLE_DATA(pp);
    ppers = PA_VARIABLE_PERSISTENCE(pp);
    pfun  = PA_VARIABLE_INIT_FUNC(pp);
    pval  = PA_VARIABLE_INIT_VAL(pp);
    pfn   = PA_VARIABLE_CACHE_FILE_NAME(pp);
    pall  = PA_VARIABLE_ALLOCATION(pp);

    if ((ppers == CACHE_F) && (pfn != NULL))
       {if (PA_cache_file == NULL)
           {PA_cache_file = PD_open(pfn, "r");
            ret = PD_read(PA_cache_file, pname, pdata),
            PD_close(PA_cache_file);
            PA_cache_file = NULL;}
        else
           ret = PD_read(PA_cache_file, pname, pdata),

        PA_ERR(!ret,
	       "FAILED TO READ CACHED VARIABLE %s - _PA_LOAD_SPACE", pname);}

    else if ((ppers == CACHE_R) && (pfn != NULL))
       {pfile = PD_open(pfn, "a");
        PA_ERR((pfile == NULL),
               "CAN'T OPEN CACHE FILE %s - _PA_LOAD_SPACE", pfn);
        PA_ERR(!PD_read(pfile, pname, pdata),
               "FAILED TO READ CACHED VARIABLE %s - _PA_LOAD_SPACE", pname);
	PA_ERR(!PD_close(pfile),
               "CAN'T CLOSE CACHE FILE %s - _PA_LOAD_SPACE", pfn);}

    else if ((svp = PA_get_source(pname, TRUE)) != NULL)
       {t   = pck->p_t;
        dt  = pck->p_dt;
        ssz = svp->size;
        PA_ERR((psz != ssz),
               "SOURCE VARIABLE SIZE DOESN'T MATCH - _PA_LOAD_SPACE");
        PA_interp_src(pdata, svp, 0, psz, t, dt);}

    else if (pfun != NULL)
       {(*pfun)(pdata, psz, pname);

/* initialize statics at most once */
        if (pall == STATIC)
           PA_VARIABLE_INIT_FUNC(pp) = NULL;}

    else if (pval != NULL)
       {long i, bpi;
        char *pvr;

        psz = max(1, psz);
        bpi = _PD_lookup_size(PA_VARIABLE_TYPE(pp)->type, PA_vif->host_chart);
        for (pvr = (char *) pdata, i = 0L; i < psz; i++, pvr += bpi)
            memcpy(pvr, pval, bpi);

/* initialize statics at most once */
        if (pall == STATIC)
           PA_VARIABLE_INIT_VAL(pp) = NULL;}

    else
       PA_ERR((PA_VARIABLE_CLASS(pp) == REQU),
              "NO INITIALIZATION FOR REQUIRED VAR %s - _PA_LOAD_SPACE",
              pname);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_GET_ACCESS - obtain a pointer to part of a data array 
 *               - some of which may be off on disk
 *               - return a pointer to it
 */

void *PA_get_access(void **vr, char *s, long offs, long ne, int track)
   {PA_variable *pp;
    char *pname, bf[MAXLINE];
    int pclass, pscope, pall;
    long psz;
    void *pdata;

    pp = PA_inquire_variable(s);
    PA_ERR((pp == NULL),
           "VARIABLE %s NOT IN DATA BASE - PA_GET_ACCESS", s);
    pname  = PA_VARIABLE_NAME(pp);
    pclass = PA_VARIABLE_CLASS(pp);
    pscope = PA_VARIABLE_SCOPE(pp);
    pall   = PA_VARIABLE_ALLOCATION(pp);

    PA_ERR(((pclass == PSEUDO) && (pall == DYNAMIC)),
           "CANNOT CONNECT DYNAMIC PSEUDO VARIABLE %s - PA_GET_ACCESS",
	   pname);

    pdata = PA_VARIABLE_DATA(pp);
    if (pdata == NULL)

/* get DMND variable in if it hasn't been cached already */
       {SC_LOCKON(PA_access_lock);

	if ((pscope == DMND) && (PA_VARIABLE_CACHE_FILE_NAME(pp) != NULL))
           _PA_rd_variable(PA_VARIABLE_FILE(pp), pp,
                           PA_VARIABLE_FILE_CONVERS(pp), DMND);

/* force consistency in variable size and shape */
        psz = _PA_consistent_size(pp, PA_DATABASE);

/* if DYNAMIC decide what to do based on the class of the variable */
        if (PA_VARIABLE_ALLOCATION(pp) == DYNAMIC)
           {switch (pclass)
               {case REQU   :
		     pdata = _PA_handle_required(pp, psz);
                     break;

		case OPTL   :
		     pdata = _PA_handle_optional(pp, psz);
                     break;

                case PSEUDO :
		     PA_ERR(TRUE,
                            "DON'T ALLOCATE PSEUDO %s - PA_GET_ACCESS",
                            pname);

                default     :
		     PA_ERR(TRUE,
                            "BAD CLASS ON %s - PA_GET_ACCESS",
                            pname);};}

        else
           PA_ERR(TRUE, "BAD STATIC VARIABLE - PA_GET_ACCESS");

	SC_LOCKOFF(PA_access_lock);}

    else if ((pall == STATIC) &&
	     ((PA_VARIABLE_INIT_VAL(pp) != NULL) ||
	      (PA_VARIABLE_INIT_FUNC(pp) != NULL)))
       {SC_LOCKON(PA_access_lock);

	psz = _PA_consistent_size(pp, PA_DATABASE);
	_PA_load_space(pp, psz);

	SC_LOCKOFF(PA_access_lock);};

    *vr = pdata;

/* if PANACEA is to keep track of this reference add it to the list */
    if (track && (pall != STATIC))
       {pcons *pc;

	SC_LOCKON(PA_track_lock);

/* search the access list for this pointer */
        pc = PA_VARIABLE_ACCESS_LIST(pp);
        for (; pc != NULL; pc = (pcons *) pc->cdr)
            {if (vr == (void **) pc->car)
                break;};

/* if this one is already on the access list don't add it again */
        if (pc == NULL)
           {snprintf(bf, MAXLINE, "%s *", (char *) PA_VARIABLE_TYPE(pp));
	    pc = SC_make_pcons(bf, FALSE, vr,
			       SC_PCONS_P_S, TRUE,
			       PA_VARIABLE_ACCESS_LIST(pp));

            PA_VARIABLE_ACCESS_LIST(pp) = pc;};

	SC_LOCKOFF(PA_track_lock);};

    return(pdata);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_CACHE_GROUP - store the given variable in the collective cache
 *                 - file
 *                 - this is for variables which will never be resized
 */

static void _PA_cache_group(PA_variable *pp)
   {int new;
    char *type, *s;
    syment *ep;
    dimdes *dims;
    PA_thread_state *ps;
    PFBinWrite wr;

    ps = PA_get_thread(-1);
    s  = ps->cache_fname;

    if (PA_cache_file == NULL)
       {snprintf(s, MAXLINE, "%s.c00", _PA_base_name);
        REMOVE(s);
        PA_cache_file = PD_open(s, "w");
        PA_ERR((PA_cache_file == NULL),
               "CAN'T OPEN CACHE FILE - _PA_CACHE_GROUP");};

    wr = PA_cache_file->tr->write;

    ep   = PA_VARIABLE_DESC(pp);
    type = ep->type;
    dims = ep->dimensions;
    ep   = wr(PA_cache_file,
	      PA_VARIABLE_NAME(pp),
	      type, type,
	      PA_VARIABLE_DATA(pp), dims, FALSE, &new);
    PA_ERR((ep == NULL),
	   "WRITE TO CACHE FILE %s FAILED - _PA_CACHE_GROUP", s);
    if (!new)
       _PD_rl_syment_d(ep);

    PA_VARIABLE_CACHE_FILE_NAME(pp) = SC_strsavef(s,
						  "char*:_PA_CACHE_GROUP:s");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_CACHE_INDIVID - store the given variable in is own seperate cache
 *                   - file
 *                   - this is for variables which may be resized
 */

static void _PA_cache_individ(PA_variable *pp)
   {int new;
    char s[MAXLINE], *type, *pfn;
    PDBfile *fp;
    syment *ep;
    dimdes *dims;
    PA_thread_state *ps;
    PFBinWrite wr;

    ps = PA_get_thread(-1);

    pfn = PA_VARIABLE_CACHE_FILE_NAME(pp);
    if (pfn == NULL)
       {snprintf(s, MAXLINE, "%s.c%02d", _PA_base_name, ps->ic++);
        pfn = SC_strsavef(s, "char*:_PA_CACHE_INDIVID:s");
        REMOVE(pfn);
        PA_VARIABLE_CACHE_FILE_NAME(pp) = pfn;};

    fp = PD_open(pfn, "a");
    if (fp == NULL)
       fp = PD_open(pfn, "w");

    PA_ERR((fp == NULL),
           "CAN'T OPEN CACHE FILE %s - _PA_CACHE_INDIVID", pfn);

    ep   = PA_VARIABLE_DESC(pp);
    type = ep->type;
    dims = ep->dimensions;

    wr = PA_cache_file->tr->write;
    ep = wr(fp,
	    PA_VARIABLE_NAME(pp),
	    type, type,
	    PA_VARIABLE_DATA(pp), PD_copy_dims(dims), FALSE, &new);
    PA_ERR((ep == NULL),
	   "WRITE TO CACHE FILE %s FAILED - _PA_CACHE_INDIVID", pfn);
    if (!new)
       _PD_rl_syment_d(ep);

    PA_ERR(!PD_close(fp),
           "CAN'T CLOSE CACHE FILE %s - _PA_CACHE_INDIVID", pfn);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INIT_SPACE - allocate and initialize space for a variable */

void *_PA_init_space(PA_variable *pp, long psz)
   {char *ptype;
    void *pdata;
    int pall;

    ptype = PA_VARIABLE_TYPE_S(pp);
    pall  = PA_VARIABLE_ALLOCATION(pp);

/* since the restart reading process can call this function
 * check whether the variable is STATIC or DYNAMIC
 */
    if (pall == STATIC)
       pdata = PA_VARIABLE_DATA(pp);
    else
       {pdata = _PA_alloc(PA_VARIABLE_TYPE(pp), ptype, psz, NULL);
        PA_VARIABLE_DATA(pp) = pdata;};

    if (pdata != NULL)
       _PA_load_space(pp, psz);

    else
       PA_ERR((PA_VARIABLE_CLASS(pp) == REQU),
              "CAN'T ALLOCATE SPACE FOR REQUIRED VAR - _PA_INIT_SPACE");

/* handle cache variables (which better be DYNAMIC) */
    if (pall == DYNAMIC)
       {switch (PA_VARIABLE_PERSISTENCE(pp))
	   {case CACHE_F :
	         _PA_cache_group(pp);
		 break;

	    case CACHE_R :
	         _PA_cache_individ(pp);
		 break;

            default :
                 break;};};

    return(pdata);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_LOAD_DATA - fill an array of space passed with the data
 *              - of the named variable
 */

void *PA_load_data(char *s)
   {PA_variable *pp;
    char *pname;
    long psz;
    void *pdata;

    pp = PA_inquire_variable(s);
    PA_ERR((pp == NULL),
           "VARIABLE %s NOT IN DATA BASE - PA_LOAD_DATA", s);

    PA_ERR((PA_VARIABLE_ALLOCATION(pp) == DYNAMIC),
           "VARIABLE %s NOT STATIC - PA_LOAD_DATA", s);

    pname = PA_VARIABLE_NAME(pp);
    PA_ERR((PA_VARIABLE_CLASS(pp) == PSEUDO),
           "PSEUDO VARIABLE ILLEGAL %s - PA_LOAD_DATA", pname);

/* force consistency in variable size and shape */
    psz = _PA_consistent_size(pp, PA_DATABASE);

    pdata = PA_VARIABLE_DATA(pp);
    PA_ERR((pdata == NULL),
           "BAD STATIC VARIABLE - PA_LOAD_DATA");

    _PA_load_space(pp, psz);

    return(pdata);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_REMOVE_REF - remove the reference from the list of pointers
 *                - pointing to this data
 *                - also count the number of references to this data
 */

static int _PA_remove_ref(void **vp, PA_variable *pp)
   {int refs;
    pcons *prev, *next, *lst;

    SC_LOCKON(PA_track_lock);

    refs = 0;
    prev = NULL;
    for (lst = PA_VARIABLE_ACCESS_LIST(pp); lst != NULL; lst = next)
        {next = (pcons *) lst->cdr;
         refs++;

         if ((void *) vp == lst->car)
            {SC_rl_pcons(lst, 0);
             if (prev == NULL)
                PA_VARIABLE_ACCESS_LIST(pp) = next;
             else
                prev->cdr = (void *) next;
             refs--;};

         prev = lst;};

    SC_LOCKOFF(PA_track_lock);

    return(refs);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_REL_ACCESS - release a connection obtained by PA_get_access
 *               - put away any scratch variables first
 */

void PA_rel_access(void **vp, char *s, long offs, long ne)
   {PA_variable *pp;
    int pscope, ppersist, refs;
    long psz, nvoids;
    void *pdata;

    pp = PA_inquire_variable(s);
    PA_ERR((pp == NULL),
           "VARIABLE %s NOT IN DATA BASE - PA_REL_ACCESS", s);
    pdata    = PA_VARIABLE_DATA(pp);
    pscope   = PA_VARIABLE_SCOPE(pp);
    ppersist = PA_VARIABLE_PERSISTENCE(pp);
    psz      = PA_VARIABLE_SIZE(pp);

/* remove the reference from the list of pointers pointing to this data */
    refs = _PA_remove_ref(vp, pp);

    SC_LOCKON(PA_access_lock);

    switch (pscope)
       {case EDIT    :
	     if (psz > 1)
	        {SFREE(pdata);};
	     PA_VARIABLE_DATA(pp) = NULL;
	     break;

        case SCRATCH :
	     if (offs >= 0L)
/*              PD_replace(); */
	     break;

        case RUNTIME :
        case DMND    :
        case RESTART :
        case DEFN    :
        default      :
	     break;};

/* if DYNAMIC NULL out the applications pointer */
    if (PA_VARIABLE_ALLOCATION(pp) == DYNAMIC)
       *vp = NULL;

/* if there are no more references see what is to be done */
    if (refs == 0)
       {switch (ppersist)
           {case KEEP    :
                 break;

            case CACHE_F :
                 _PA_cache_group(pp);
                 SFREE(PA_VARIABLE_DATA(pp));
                 break;

            case CACHE_R :
                 _PA_cache_individ(pp);

            case REL     :
                 SFREE(PA_VARIABLE_DATA(pp));
                 break;};

/* zero out a scalar value */
        if (PA_VARIABLE_ALLOCATION(pp) == STATIC)
           {nvoids = psz*PA_VARIABLE_TYPE(pp)->size;
            memset(vp, 0, nvoids);};};

    SC_LOCKOFF(PA_access_lock);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INIT_SCALAR - initialize a PANACEA scalar that was defined
 *                - via PA_INST_SCALAR
 */

void PA_init_scalar(char *s)
   {int id, pclass;
    char *pname, *ptype;
    void *pdata, *pval;
    void (*pfun)(void *p, long sz, char *s);
    PA_variable *pp;

    pp = PA_inquire_variable(s);
    PA_ERR((pp == NULL),
           "VARIABLE %s NOT IN DATA BASE - PA_INIT_SCALAR", s);
    pname  = PA_VARIABLE_NAME(pp);
    pclass = PA_VARIABLE_CLASS(pp);

    PA_ERR((pclass == PSEUDO),
           "CANNOT CONNECT PSEUDO VARIABLE %s - PA_INIT_SCALAR", pname);

    pdata = PA_VARIABLE_DATA(pp);
    pname = PA_VARIABLE_NAME(pp);
    ptype = PA_VARIABLE_TYPE_S(pp);
    pfun  = PA_VARIABLE_INIT_FUNC(pp);
    pval  = PA_VARIABLE_INIT_VAL(pp);

    id = SC_type_id(ptype, FALSE);

/* floating point types */
    if (id == SC_DOUBLE_I)
       {PA_SET_UP_SCALAR(pname, double, pdata, pval, pfun);}

    else if (id == SC_FLOAT_I)
       {PA_SET_UP_SCALAR(pname, float, pdata, pval, pfun);}

/* fixed point types */
    else if (id == SC_INT_I)
       {PA_SET_UP_SCALAR(pname, int, pdata, pval, pfun);}

    else if (id == SC_LONG_I)
       {PA_SET_UP_SCALAR(pname, long, pdata, pval, pfun);}

    else if (id == SC_SHORT_I)
       {PA_SET_UP_SCALAR(pname, short, pdata, pval, pfun);}

    else if (id == SC_CHAR_I)
       {PA_SET_UP_SCALAR(pname, char, pdata, pval, pfun);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_ALLOC - allocate memory for the variable type
 *           - if given a non NULL PVAL broadcast the value into the space
 *           - it is the callers responsibility to make sure that pval
 *           - points to something of type TYPE
 */

void *_PA_alloc(defstr *dp, char *type, long nitems, void *pval)
   {void *vr;
    char *pvr;
    long i, bpi;

    bpi = dp->size;
    if (_PD_indirection(type))
       vr = SC_alloc_nzt(nitems, sizeof(char *), "_PA_ALLOC:vra", NULL);

    else
       {PA_ERR((dp == NULL),
               "BAD TYPE %s - _PA_ALLOC", type);
        vr = SC_alloc_nzt(nitems, bpi, "_PA_ALLOC:vrb", NULL);};

/* if given a non-NULL initial value, broadcast it in
 * otherwise make sure the space is filled with zeroes
 */
    if (pval == NULL)
       {if (SC_zero_on_alloc() == FALSE)
	   memset(vr, 0, nitems*bpi);}
    else
       {for (pvr = (char *) vr, i = 0L; i < nitems; i++, pvr += bpi)
            memcpy(pvr, pval, bpi);};

    return(vr);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
