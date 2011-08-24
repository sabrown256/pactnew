/*
 * PASHAS.C - more routines that are shared between PANACEA and its pre and
 *          - post processors
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

#define MAXSIZE 4000000L

char
 *PA_SET_INDEX_P_S = NULL,
 *PA_SET_INDEX_S = NULL;

int
 _PA_halt_fl = FALSE;

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TRAP_SIGNALS - trap errors and to do graceful things
 *                 - attach the specified functions to the specified signals
 */

void PA_trap_signals(int nsig, ...)
   {int i, sig;
    PFSignal_handler fnc;

    SC_VA_START(nsig);

    for (i = 0; i < nsig; i++)
        {sig = SC_VA_ARG(int);
         fnc = SC_VA_ARG(PFSignal_handler);
         SC_signal(sig, fnc);};

    SC_VA_END;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SIGNAL_HANDLER - handle signals for PANACEA */

void PA_signal_handler(int sig)
   {

    switch (sig)
       {
#ifdef SIGSEGV
        case SIGSEGV :
             PA_ERR(TRUE, "SEGMENTATION VIOLATION");
             break;
#endif

#ifdef SIGABRT
        case SIGABRT :
             PA_ERR(TRUE, "ABORT SIGNAL RECIEVED");
             break;
#endif

#ifdef SIGBUS
        case SIGBUS  :
             PA_ERR(TRUE, "BUS ERROR");
             break;
#endif

#ifdef SIGFPE
        case SIGFPE  :
             PA_ERR(TRUE, "FLOATING POINT EXCEPTION");
             break;
#endif
        default      :
             PA_ERR(TRUE, "UNKNOWN SIGNAL");
             break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INTERRUPT_HANDLER - handle interrupts for PANACEA */

void PA_interrupt_handler(int sig)
   {char bf[MAXLINE], *cmnd, *name, *s;
    int cyc;
    double tcon, t, dt;
    PA_package *pck;

    SC_signal_n(SIGINT, PA_interrupt_handler, NULL);

    PRINT(stdout, "\n\nInterrupt:\n");
    PRINT(stdout, "  c     - Enter a command loop\n");
    PRINT(stdout, "  f     - Finish after the current cycle\n");
    PRINT(stdout, "  p     - Current package\n");
    PRINT(stdout, "  q     - Quit immediately (no state dump)\n");
    PRINT(stdout, "  r     - Resume\n");
    PRINT(stdout, "  t     - Problem time\n");
    PRINT(stdout, "\nI-> ");

    GETLN(bf, MAXLINE, stdin);

    cmnd = SC_strtok(bf, " \t\n\r", s);
    if (cmnd == NULL)
       return;

    SC_strtok(bf, " \t\n\r", s);

    fflush(stdout);

    pck  = PA_current_package();
    tcon = convrsn[SEC]/unit[SEC];
    name = pck->name;
    t    = pck->p_t*tcon;
    dt   = pck->p_dt*tcon;
    cyc  = pck->p_cycle;

    switch (cmnd[0])
       {case 'c' : PA_get_commands(stdin, NULL);
                   break;

        case 'f' : _PA_halt_fl = TRUE;
                   break;

        case 'p' : PRINT(stdout, "Running %s\n", name);
                   break;

        case 'q' : exit(1);

        case 'r' : 
        default  : PRINT(stdout, "\nResuming\n\n");
                   break;

        case 't' : PRINT(stdout, "Cycle = %4d, Time = %10.3e, Dt = %10.3e\n",
                                 cyc, t, dt);
                   break;};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_FILE_MON - monitor the auxilliary files */

void PA_file_mon(char *edname, char *ppname, char *gfname)
   {int64_t curpos;
    
/* check on the edit file */
    if (PA_edit_file != NULL)
       {if (_PA.edstr == 0L)
           _PA.edstr = io_tell(PA_edit_file);
        curpos = io_tell(PA_edit_file);

        if (curpos+_PA.edstr >= MAXSIZE)
           {io_close(PA_edit_file);
            PA_advance_name(edname);
            PA_edit_file = io_open(edname, "w");
            PA_ERR((PA_edit_file == NULL),
                   "CAN'T OPEN FILE %s - PA_FILE_MON", edname);
            PRINT(stdout, "Edit file %s opened\n", edname);
            _PA.edstr = 0L;};};

/* check on the post processor */
    if (PA_pp_file != NULL)
       {FILE *str;

	str = PA_pp_file->stream;

        if (_PA.ppstr == 0L)
           _PA.ppstr = lio_tell(str);
        curpos = lio_tell(str);

        if (curpos+_PA.ppstr >= MAXSIZE)
           {PA_close_pp();
            PA_advance_name(ppname);
            PA_pp_file = PA_open(ppname, "w", FALSE);
            PRINT(stdout, "Post processor file %s opened\n", ppname);
            _PA.ppstr = 0L;};};

/* check on the PVA file */
    if (PA_pva_file != NULL)
       {FILE *str;

	str = PA_pva_file->stream;

        if (_PA.ppstr == 0L)
           _PA.ppstr = lio_tell(str);
        curpos = lio_tell(str);

        if (curpos+_PA.ppstr >= MAXSIZE)
           {PD_close(PA_pva_file);
            PA_advance_name(gfname);
            PA_pva_file = PA_open(gfname, "w", FALSE);
            PA_ERR(!PD_def_mapping(PA_pva_file),
                   "CAN`T DEFINE MAPPINGS - PA_FILE_MON");
            PRINT(stdout, "PVA file %s opened\n", gfname);
            _PA.ppstr = 0L;};};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_RD_RESTART - a friendly interface to the process of reading a
 *               - restart dump
 *               -
 *               - Arguments:
 *               -   rsname - the name of the restart file
 *               -   convs  - the type of conversions to do (see options)
 *               -
 *               - Options:
 *               -   NONE    - perform no conversions
 *               -   INT_CGS - convert from internal units to CGS units
 *               -   INT_EXT - convert from internal units to external units
 *               -   EXT_CGS - convert from external units to CGS units
 *               -   EXT_INT - convert from external units to internal units
 *               -   CGS_INT - convert from CGS units to internal units
 *               -   CGS_EXT - convert from CGS units to external units
 *               -
 *               - The internal system of units is defined by the "unit"
 *               - array.
 *               - The external system of units is defined by the "convrsn"
 *               - array.
 *               -
 *               - The system of units of the data in the restart dump is
 *               - under the control of the code developer, but it must
 *               - be consistent.
 *
 * #bind PA_rd_restart fortran() scheme(pa-read-state-file)
 */

void PA_rd_restart(char *rsname, int convs)
   {int64_t lsa, lsb;
    float space;

    SC_mem_stats(&lsb, NULL, NULL, NULL);

    _PA_rdrstrt(rsname, convs);

    SC_mem_stats(&lsa, NULL, NULL, NULL);

    space = ((float) (lsa - lsb)) / 1024.0;

    PRINT(STDOUT, "Space for state data and variable data base:");
    PRINT(STDOUT, " %g (kByte)\n", space);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_WR_RESTART - prepare and write a restart dump
 *
 * #bind PA_wr_restart fortran() scheme(pa-write-state-file)
 */

void PA_wr_restart(char *rsname ARG("state.tmp",in))
   {

    if (rsname != NULL)
       {PRINT(stdout, "Restart dump %s ...", rsname);
	_PA_wrrstrt(rsname, NONE);
	PRINT(stdout, " written\n");};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CLOSE_PP - close out the current pp file */

void PA_close_pp(void)
   {int err;

    err = !PD_close(PA_pp_file);

    PA_ERR(err, "TROUBLE CLOSING POST PROCESSOR FILE %s - PA_CLOSE_PP",
           (err) ? PA_pp_file->name : NULL);

    PA_current_pp_cycle = -1;

    PA_pp_file = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INIT_STRINGS - convert the compiler allocated strings to
 *                 - dynamic memory so PDBLib is happy
 */

void PA_init_strings(void)
   {

/* replace the static type strings with dynamic ones */
    ONCE_SAFE(TRUE, NULL)
       PA_SET_INDEX_P_S = CSTRDUP("PA_set_index *", 3);
       PA_SET_INDEX_S   = CSTRDUP("PA_set_index", 3);
    END_SAFE;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_DEF_STR - let the packages define their defstr's
 *            - for the restart dump
 */

void PA_def_str(PDBfile *pdrs)
   {PA_package *pck;
    PFPkgDfstrc pck_dfstrc;
    PFDefTyp hook;

/* get the PM_set in for the PA_plot_request */
    PA_ERR(!PD_def_mapping(pdrs),
           "CAN`T DEFINE MAPPINGS - PA_DEF_STR");

/* define PML types */

    PD_defstr(pdrs, "complex",
              "double real",
              "double imaginary",
              LAST);

    PD_defstr(pdrs, "C_array",
              "char *type",
              "long length",
              "char *data",
              LAST);

    PD_cast(pdrs, "C_array", "data", "type");

    PD_defstr(pdrs, "PM_matrix",
	      "int nrow",
	      "int ncol",
	      "double *array",
	      LAST);

/* define PANACEA types */

    PD_defstr(pdrs, "PA_iv_specification",
              "int type",
              "char *name",
              "char *file",
              "int num",
              "int index",
              "int interpolate",
              "char *spec",           /* this is a fudge, spec will be NULL */
              "double *data",
              "PA_iv_specification *next",
              LAST);

    PD_defstr(pdrs, "PA_set_spec",
              "char *var_name",
              "char *function",
              "char *text",
	      "int n_values",
	      "double *values",
              "PA_set_spec *next",
              LAST);

    PD_defstr(pdrs, "PA_plot_request",
              "PA_set_spec *range",
              "char *range_name",
              "PA_set_spec *domain",
              "char *base_domain_name",
              "C_array *domain_map",
              "char *text",
              "int time_plot",
              "int mesh_plot",
              "int status",
              "long size",
              "long offset",
              "long stride",
              "int str_index",
              "double conv",
              "int centering",
              "int allocate_data",
              "PM_set *data",
              "int data_index",
              "int data_type",
              "PA_plot_request *next",
              LAST);

    PD_defstr(pdrs, "PA_set_index",
              "double val",
              "int index",
              "int imax",
              "double conv",
              "char *name",
              LAST);

/* see if there is a special hook */
    hook = PA_GET_FUNCTION(PFDefTyp, "define-types");
    if (hook != NULL)
       (*hook)(pdrs);

/* poll the packages */
    for (pck = Packages; pck != NULL; pck = pck->next)
        {pck_dfstrc = pck->dfstrc;
         if (pck_dfstrc != NULL)
            (*pck_dfstrc)(pdrs);};

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_ADJUST_REFS - update the pointer list of the specified variable PP
 *                 - with the new data pointer PDATA and size PSZ
 */

static void _PA_adjust_refs(PA_variable *pp, void *pdata, long psz)
   {pcons *lst;

/* now update the pointer list */
    lst = PA_VARIABLE_ACCESS_LIST(pp);
    for (; lst != NULL; lst = (pcons *) lst->cdr)
        *(char **) lst->car = (char *) pdata;

    PA_VARIABLE_DATA(pp)         = pdata;
    PA_VARIABLE_SIZE(pp)         = psz;
    PA_VARIABLE_DESC(pp)->number = psz;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_REALLOC - reallocate (change the length) the named variable
 *             - to the given new size and
 *             - return a pointer to it
 */

static void *_PA_realloc(PA_variable *pp, dimdes *olddm, int flag)
   {char *pname;
    int pclass, n_ref;
    long psz;
    pcons *lst;
    void *pdata;
    defstr *pd;

    pname  = PA_VARIABLE_NAME(pp);
    pclass = PA_VARIABLE_CLASS(pp);
    pdata  = PA_VARIABLE_DATA(pp);
    pd     = PA_VARIABLE_TYPE(pp);

/* count the references */
    n_ref = 0;
    lst   = PA_VARIABLE_ACCESS_LIST(pp);
    for (; lst != NULL; lst = (pcons *) lst->cdr, n_ref++);

/* force consistency in variable size and shape */
    psz = _PA_consistent_size(pp, flag);

    PA_ERR((pclass == PSEUDO),
           "CANNOT REALLOCATE PSEUDO VARIABLE %s - _PA_REALLOC", pname);

/* handle case when there is no data hence reallocation is inappropriate */
    if (pdata == NULL)
       {if ((n_ref == 0) || (psz <= 0L))
           return(NULL);

        else
           {pdata = _PA_init_space(pp, psz);
            PA_ERR((pdata == NULL),
                   "ALLOCATION FAILED - _PA_REALLOC");};}
    else
       {void *oldsp, *newsp;
        PFChangeDim proc;
        long nbn, nbo;

/* get the byte size of the spaces
 * NOTE: 1000 times NO to fiddling with statically allocated arrays
 * even to shrink them!
 */
        nbn = psz*(pd->size);
        nbo = SC_arrlen(pdata);

        newsp = CMAKE_N(char, nbn);
        oldsp = pdata;

/* see whether there is a routine to map the old space into the new one */
        proc = PA_GET_VAR_FUNCTION(pp, PFChangeDim, "change_dim");
        if (proc != NULL)
           {long i, nd, *ndm, *odm;
	    dimdes *newdm, *pnd, *pod;

/* safe to use _PA_mk_sym_dims because _PA_consistent_size has
 * already been done
 */
	    newdm = _PA_mk_sym_dims(PA_VARIABLE_DIMS(pp));

/* count the dimensions */
            for (nd = 0, pod = olddm; pod != NULL; nd++, pod = pod->next);

            ndm = CMAKE_N(long, nd);
            odm = CMAKE_N(long, nd);

/* make arrays of values */
            for (i = 0, pnd = newdm, pod = olddm;
		 i < nd;
		 i++, pnd = pnd->next, pod = pod->next)
                {ndm[i] = pnd->number;
                 odm[i] = pod->number;};

            (*proc)(pname, newsp, oldsp, &nd, ndm, odm);

	    _PD_rl_dimensions(newdm);

            CFREE(ndm);
            CFREE(odm);}

/* copy the smaller number of bytes
 * oldsp+nbn may be out of bounds and a coredump will ensue
 * the remaining new bytes would be zero anyway!
 */
        else
	   memcpy(newsp, oldsp, min(nbo, nbn));

        pdata = newsp;

        CFREE(oldsp);};

    _PA_adjust_refs(pp, pdata, psz);

    return(pdata);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CHANGE_DIM - store a new value in the given variable and reallocate
 *               - every variable in PA_variable_tab which uses the
 *               - given variable address to find dimension values
 */

void PA_change_dim(int *pdm, int val)
   {int oval;
    int *dmn, *dmx;
    long i;
    char *ty;
    dimdes *olddm;
    PA_variable *pp;
    PA_dimens *pvd;

    oval = *pdm;
    *pdm = val;

/* check every variable's dimension list for a match to this address */
    for (i = 0; SC_hasharr_next(PA_variable_tab, &i, NULL, &ty, (void **) &pp); i++)
        {if (ty[3] == 'p')                   /* skip the packages */
	    continue;

/* check the dimension list for this variable and reallocate it if it
 * changed (only one dimension need change so break after only one)
 */
	 for (pvd = PA_VARIABLE_DIMS(pp); pvd != NULL; pvd = pvd->next)
	     {dmn = pvd->index_min;
	      dmx = pvd->index_max;
	      if ((dmn == pdm) || (dmx == pdm))

/* change it back long enough to make a list of the original dimensions */
		 {*pdm  = oval;
		  olddm = _PA_mk_sym_dims(PA_VARIABLE_DIMS(pp));
		  *pdm  = val;

		  _PA_realloc(pp, olddm, PA_DATABASE);

		  _PD_rl_dimensions(olddm);

		  break;};};};

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_CHANGE_SIZE - reallocate the variable to the current size specified
 *                - by the dimensions specified (PA_FILE or PA_DATABASE)
 */

void PA_change_size(char *name, int flag)
   {PA_variable *pp;

    pp = PA_inquire_variable(name);
    PA_ERR((pp == NULL),
           "VARIABLE %s IS NOT IN THE DATA BASE - PA_CHANGE_SIZE", name);

    _PA_realloc(pp, NULL, flag);

    return;}
 
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INTERN - intern a variable pointed to by the first arg in the
 *           - variable table under the name given by the second arg
 *           - if vr is NULL and is an array allocate the space
 *           - return a pointer to the data
 *
 * #bind PA_intern fortran() scheme()
 */

void *PA_intern(void *vr, char *name)
   {PA_variable *pp;
    char *ptype;
    long psz;
    void *pdata;

    pp = PA_inquire_variable(name);
    PA_ERR((pp == NULL),
           "VARIABLE %s IS NOT IN THE DATA BASE - PA_INTERN", name);

/* force consistency in variable size and shape */
    psz = _PA_consistent_size(pp, PA_DATABASE);

/* decide what to do based on the size and whether
 * or not the pointer vr is NULL
 */
    if ((psz < 1L) || (vr != NULL))
       pdata  = vr;
    else
       {ptype = PA_VARIABLE_TYPE_S(pp);
        pdata = _PA_alloc(PA_VARIABLE_TYPE(pp),
                          ptype, psz,
                          PA_VARIABLE_INIT_VAL(pp));};

    _PA_adjust_refs(pp, pdata, psz);

    return(pdata);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SIZEOF - sizeof operator for PANACEA */

int PA_sizeof(char *s)
   {int rv;

    if (PA_vif != NULL)
       rv = _PD_lookup_size(s, PA_vif->chart);

    else
       rv = SC_sizeof(s);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
