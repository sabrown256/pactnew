/*
 * PANACEA.C - main service routines provided by PANACEA
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "panacea_int.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_SIMULATE - given that a numerical simulation code has been defined
 *             - with def_system, and properly initialized, perform the
 *             - simulation as defined
 *             -
 *             - Arguments:
 *             -
 *             -    tc       - current problem time
 *             -    nc       - current problem cycle
 *             -    ti       - initial problem time
 *             -    tf       - final time
 *             -    dtf_init - initial fractional dt
 *             -    dtf_min  - minimum fractional dt
 *             -    dtf_max  - maximum fractional dt
 *             -    dtf_inc  - fractional increase in dt per cycle
 */

void PA_simulate(tc, nc, nz, ti, tf, dtf_init, dtf_min, dtf_max, dtf_inc,
                 rsname, edname, ppname, gfname)
   double tc;
   int nc, nz;
   double ti, tf, dtf_init, dtf_min, dtf_max, dtf_inc;
   char *rsname, *edname, *ppname, *gfname;
   {double deltat, tconv, dtmn, dtmx, dtf, dt, t;
    int cycle;

/* clear the halt flag */
    _PA_halt_fl = FALSE;

/* initialize the time-cycle loop */
    tconv  = convrsn[SEC]/unit[SEC];
    deltat = tf - ti;
    dt     = dtf_init*deltat;
    dtmn   = dtf_min*deltat;
    dtmx   = dtf_max*deltat;
    PRINT(stdout, "Starting simulation: ti = %10.3e to tf = %10.3e\n",
           tc*tconv, tf*tconv);

/* loop over time-cycles */
    for (cycle = nc+1, t = tc; t <= tf; cycle++)

/* update the sourced variables */
        {PA_source_variables(t, dt);

/* call the packages and their post-processors */
         PA_run_packages(t, dt, cycle);
         
/* make post processor entries from the global data base */
         PA_dump_pp(t, dt, cycle);

/* monitor the auxilliary files */
         PA_file_mon(edname, ppname, gfname);

/* if a halt has been called to the simulation clean up and exit */
         if (_PA_halt_fl)
            {PRINT(stdout, "Halting computation: t = %10.3e\n\n",
                           t*tconv);
             break;};

/* advance the time */
         t  += dt;
         dt  = PA_advance_t(dtmn, dt*dtf_inc, dtmx);

/* adjust for the final cycle it this is it */
         if ((dtf = tf - t) > 0.0)
            dt = min(dtf, dt);};

/* announce the completion of the calculation */
    PRINT(stdout, "\nSimulation complete: t = %10.3e\n\n", (t-dt)*tconv);

/* finalize the packages and dump the statistics */
    PA_fin_system(nz, cycle, FALSE);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_INIT_SYSTEM - initialize the code system
 *                - connect the global variables, open the post-processor
 *                - file and the edit file, and run the package initializers
 */

void PA_init_system(t, dt, nc, edname, ppname, gfname)
   double t, dt;
   int nc;
   char *edname, *ppname, *gfname;
   {PA_package *pck;
    PFPkgInizer pck_init;
    PFBuildMap hook;

    SC_sizeof_hook = PA_sizeof;

    PA_init_strings();
    PA_cpp_init();
    PA_cpp_default();
    PA_def_var_init();

/* initialize the source files and their variables */
    _PA_init_sources(t, dt);

/* initialize the pp files and structures */
    _PA_init_files(edname, ppname, gfname);

/* initialize the packages */
    for (pck = Packages; pck != NULL; pck = pck->next)
        {pck_init = pck->inizer;
         if (pck_init != NULL)
            {PA_control_set(pck->name);
             (*pck_init)(pck);};

         pck->time    = 0.0;
         pck->space   = 0.0;
         pck->dt      = HUGE;
         pck->dt_zone = -1;};

/* reset the global control arrays */
    PA_control_set("global");

    hook = PA_GET_FUNCTION(PFBuildMap, "build_mapping");
    PA_ERR(((PA_pva_file != NULL) && (hook == NULL)),
           "CAN`T BUILD GRAPHS WITHOUT HOOK - PA_INIT_SYSTEM");

/* reprocess plots after the package initializers and the source files
 * have had their impact on both the data base and the plot requests
 */
    _PA_scan_pp();

/* do an initial dump */
    PA_dump_pp(t, dt, nc);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_RUN_PACKAGES - call the packages and their post-processors */

void PA_run_packages(t, dt, cycle)
   double t, dt;
   int cycle;
   {PA_package *pck;
    PFPkgMain pck_entry;
    char *pck_name;

/* loop over all packages */
    for (pck = Packages; pck != NULL; pck = pck->next)
        {pck_entry = pck->main;
         pck_name  = pck->name;

/* execute the package */
         if (pck_entry != NULL)
            {pck->p_t     = t;
             pck->p_dt    = dt;
             pck->p_cycle = cycle;

             PA_control_set(pck_name);
             _PA_allocate_mapping_space(pck);

             (*pck_entry)(pck);

             _PA_dump_package_mappings(pck, t, dt, cycle);};};

/* reconnect the global controls */
    PA_control_set("global");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_ADVANCE_T - compute and return the new dt
 *              - the time steps returned by the packages are checked
 *              - against the min and max allowable dt (dtmn and dtmx
 *              - respectively) and an initial dt value
 */

double PA_advance_t(dtmn, dtn, dtmx)
   double dtmn, dtn, dtmx;
   {double dt, pck_dt;
    PA_package *pck;

/* get the new time step */
    for (dt = dtn, pck = Packages; pck != NULL; pck = pck->next)
        {pck_dt = pck->dt;
         dt = min(pck_dt, dt);};

    dt = min(dt, dtmx);
    dt = max(dt, dtmn);

    return(dt);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_FIN_SYSTEM - call the finalizers for all of the packages and
 *               - dump the performance statistics for the run
 */

void PA_fin_system(nz, nc, silent)
   int nz, nc, silent;
   {int i;
    long lmx;
    PA_package *pck;
    PFPkgFinzer pck_fin;
    char *pck_name, bf[MAXLINE];
    double pck_t, pck_s, prb_s, prb_t, izc, smx, szf;

/* finalize the packages and dump the statistics */
    if (!silent)
       {smx = 0.0;
	for (pck = Packages; pck != NULL; pck = pck->next)
	    {pck_s = pck->space;
	     smx = max(smx, pck_s);};

	PRINT(STDOUT, "\nProblem Statistics:\n\n");

	memset(bf, ' ', MAXLINE);
	snprintf(bf,    MAXLINE, "Package");
	snprintf(bf+30, MAXLINE-30, "Rate");
	snprintf(bf+45, MAXLINE-45, "Time");

	snprintf(bf+60, MAXLINE-60, "Space");
	snprintf(bf+79, MAXLINE-79, "\n");
	for (i = 0; i < 80; i++)
	    if (bf[i] == '\0')
	       bf[i] = ' ';
	bf[80] = '\0';
	PRINT(STDOUT, "%s", bf);

	memset(bf, ' ', MAXLINE);
	snprintf(bf+21, MAXLINE-21, "(microsec/zone-cycle)");
	snprintf(bf+45, MAXLINE-45, "(sec)");

	if (smx > 1.0e9)
	   {szf = 1.0e9;
	    snprintf(bf+59, MAXLINE-59, "(GBytes)");}

	else if (smx > 1.0e6)
	   {szf = 1.0e6;
	    snprintf(bf+59, MAXLINE-59, "(MBytes)");}

	else
	   {szf = 1.0e3;
	    snprintf(bf+59, MAXLINE-59, "(kBytes)");};

	snprintf(bf+78, MAXLINE-78, "\n\n");
	for (i = 0; i < 80; i++)
	    if (bf[i] == '\0')
	       bf[i] = ' ';
	bf[80] = '\0';
	PRINT(STDOUT, "%s", bf);};

    if (nc == 0)
       izc = 0.0;
    else
       izc = (1.0e6)/((double) nz*nc);

    for (prb_t = 0.0, pck = Packages; pck != NULL; pck = pck->next)
        {pck_fin  = pck->finzer;
         pck_name = pck->name;
         pck_t    = pck->time;
         pck_s    = pck->space;

         PA_control_set(pck_name);

         if (pck_fin != NULL)
            (*pck_fin)(pck);

/* if no time was spent in the package and it took no space don't log it */
         if ((pck_t == 0.0) && (pck_s == 0))
            continue;

/* only collect the statistics for the packages which are on */
         if (PA_PACKAGE_ON(pck) && !silent)
            {prb_t += pck_t;

             memset(bf, ' ', MAXLINE);
             snprintf(bf,    MAXLINE, "%s", pck_name);
             snprintf(bf+27, MAXLINE-27, "%10.3e", izc*pck_t);
             snprintf(bf+42, MAXLINE-42, "%10.3e", pck_t);

             pck_s /= szf;
             snprintf(bf+59, MAXLINE-59, "%g", pck_s);
             snprintf(bf+79, MAXLINE-79, "\n");
             for (i = 0; i < 80; i++)
                 if (bf[i] == '\0')
                    bf[i] = ' ';
             bf[80] = '\0';
             PRINT(STDOUT, "%s", bf);};};

    if (!silent)
       {memset(bf, '-', 79);
	PRINT(STDOUT, "%s", bf);

	memset(bf, ' ', MAXLINE);
	snprintf(bf,    MAXLINE, "Total");
	snprintf(bf+27, MAXLINE-27, "%10.3e", izc*prb_t);
	snprintf(bf+42, MAXLINE-42, "%10.3e", prb_t);

	SC_mem_stats(NULL, NULL, NULL, &lmx);

	prb_s = ((float) lmx) / 1024.0;
	snprintf(bf+59, MAXLINE-59, "%g", prb_s);
	snprintf(bf+78, MAXLINE-78, "\n\n");
	for (i = 0; i < 80; i++)
	    if (bf[i] == '\0')
	       bf[i] = ' ';
	bf[80] = '\0';
	PRINT(STDOUT, "%s", bf);};

/* reconnect the global controls */
    PA_control_set("global");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PA_TERMINATE - close all the open files and generally gracefully end
 *              - the PANACEA run
 */

void PA_terminate(edname, ppname, gfname, cycle)
   char *edname, *ppname, *gfname;
   int cycle;
   {int i;

    if (PA_edit_file != NULL)
       PA_ERR(io_close(PA_edit_file),
              "TROUBLE CLOSING EDIT FILE %s - PA_TERMINATE", edname);

/* save the number of time plots and the number of points (cycles) */
    if (PA_pp_file != NULL)
       PA_close_pp();

    if (PA_pva_file != NULL)
        PA_ERR(!PD_close(PA_pva_file),
               "TROUBLE CLOSING PVA FILE %s - PA_TERMINATE", gfname);
     
    if (PA_cache_file != NULL)
        PA_ERR(!PD_close(PA_cache_file),
               "TROUBLE CLOSING CACHE FILE - PA_TERMINATE");
     
/* close open state files */
    for (i = 0; i < _PA_n_state_files; i++)
        {PA_ERR(!PD_close(_PA_state_files[i]),
                "TROUBLE CLOSING STATE FILE %d - PA_TERMINATE", i);
         _PA_state_files[i] = NULL;};

    SFREE(_PA_state_files);

    PA_pva_file   = NULL;
    PA_edit_file  = NULL;
    PA_cache_file = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _PA_INIT_FILES - initialize the auxilliary files */

void _PA_init_files(edname, ppname, gfname)
   char *edname, *ppname, *gfname;
   {

/* make the name of the first edit file */
    if (edname != NULL)
       {PA_edit_file = io_open(edname, "w");
	PA_ERR((PA_edit_file == NULL),
	       "CAN'T OPEN FILE %s - _PA_INIT_FILES", edname);
	PRINT(stdout, "Edit file %s opened\n", edname);};

/* initialize the post processor file */
    _PA_init_pp(ppname, gfname);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
