/*
 * GINIT.C - handle initializations of the LR global package
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "gloin.h"

static int
 def_unit_flag;

hasharr
 *swtab;

int
 mesh_bndry,
 mesh_bndry_r,
 mesh_mesh,
 mesh_mesh_r,
 mesh_plots,
 mesh_plots_r,
 mesh_vectr,
 mesh_vectr_r;

double
 pc;

static void
 init_prob(char *s);

void
 LR_SIGFPE_handler(int sig);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PROBLEM_B - initialize this session with an initial value problem */

int init_problem_b(int c, char **v)
   {int i, n_threads;
    char *fname;
    char commnd[MAXLINE];

/* process the command line arguments */
    fname     = NULL;
    n_threads = 1;
    for (i = 1; i < c; i++)
        if (v[i][0] == '-')
           {switch (v[i][1])
               {case 'n' :
		     n_threads = SC_stoi(v[++i]);
		     break;
                default :
		     break;};}
         else if (v[i][0] != '(')
            fname = v[i];
         else
           {SC_strncpy(commnd, MAXLINE, " ", -1);
            for ( ; i < c; i++)
                {SC_strcat(commnd, MAXLINE, v[i]);
                 SC_strcat(commnd, MAXLINE, " ");};};

    n_threads = max(n_threads, 1);
    PA_init_threads(n_threads, NULL);

    if (fname != NULL)
       {PA_rd_restart(fname, NONE);
        NAME[2] = CSTRSAVE(fname);};

    PA_trap_signals(4, SIGSEGV, PA_signal_handler,
                       SIGABRT, PA_signal_handler,
                       SIGBUS,  PA_signal_handler,
                       SIGFPE,  PA_signal_handler);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_PROB - this function is the BASE_NAME hook
 *           - it is called when the "read" command is executed
 *           - and sets the names of the files for this problem from
 *           - the base name taken from the generator name
 *           - the files are: edit file, restart file, post-processor file,
 *           - and the graph file
 */

static void init_prob(char *s)
   {

    PA_name_files(s, &NAME[3], &NAME[2], &NAME[4], &NAME[5]);

    NAME[8] = CSTRSAVE(s);

    N_parts  = 0;
    N_regs   = 0;
    PA_gs.n_plots  = 0;
    PA_gs.n_graphs = 0;

    PA_gs.plot_reqs   = NULL;
    PA_gs.iv_spec_lst = NULL;
    parts       = NULL;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INTERN_GLOBAL - intern the variables of the global package
 *               - prior to writing the initial restart dump
 */

int intern_global(void)
   {

    SWTCH[5]   = N_regs;                      /* number of regions - N_regs */
    SWTCH[6]   = N_nodes;             /* number of logical points - N_nodes */
    SWTCH[7]   = lrz;                      /* index of last real zone - lrz */
    SWTCH[8]   = frn;                     /* index of first real node - frn */
    SWTCH[9]   = lrn;                      /* index of last real node - lrn */
    SWTCH[10]  = kmax;                            /* maximum k index - kmax */
    SWTCH[11]  = lmax;                            /* maximum l index - lmax */
    SWTCH[12]  = N_zones;              /* number of logical zones - N_zones */
    SWTCH[13]  = frz;                     /* index of first real zone - frz */

    PA_INTERN(reg_map, "reg_map");
    PA_INTERN(zone, "zone");

    PA_INTERN(node, "node");
    PA_INTERN(nodet, "nodet");
    PA_INTERN(rx, "rx");
    PA_INTERN(vx, "vx");

    PA_INTERN(ry, "ry");
    PA_INTERN(vy, "vy");
    PA_INTERN(apk, "k-product");
    PA_INTERN(apl, "l-product");
    PA_INTERN(kra, "k-ratio");
    PA_INTERN(lra, "l-ratio");

    if (SWTCH[14])
       {gen_dumpw(curves);
        PA_advance_name(NAME[3]);};

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GEN_ARGS - process the input arguments for the generator */

char *gen_args(int c, char **v)
   {int i;
    char *fname;

/* process the command line arguments */
    def_unit_flag = FALSE;
    for (i = 1; i < c; i++)
        if (v[i][0] == '-')
           {switch (v[i][1])
               {case 'c' : def_unit_flag = FALSE;
                           break;
                case 'n' : def_unit_flag = TRUE;
                           break;
                default  : break;};}
         else
            fname = v[i];

    return(fname);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_UNIT - initialize the units */

int init_unit(void)
   {PA_gs.stand_alone = TRUE;

    PA_def_units(FALSE);

/* UNITS
 *
 * unit defines the internal system of units (default is physicist units)
 * multiplying by a unit converts a quantity from CGS to the internal system
 *
 * e.g. length_internal = length_cgs*PA_gs.units[CM]
 */

/* set up the physical constants in CGS units */
    PA_physical_constants_cgs();

/* the default internal system of units
 * is "natural units" with Hbar = c = 1
 */
    if (def_unit_flag)
       {PA_gs.units[G]   = PM_c.g_icm;
        PA_gs.units[SEC] = PM_c.c;
        PA_gs.units[K]   = PM_c.ev_icm*(PM_c.kboltz/PM_c.ev_erg);
        PA_gs.units[EV]  = PM_c.ev_icm*PA_gs.units[Q]*PA_gs.units[Q]/
	                   PA_gs.units[CM];}

/* the alternate default system of units is the modified CGS system
 * these values should be used in the generator deck if desired
 */
    else
       PA_gs.units[K] = PM_c.kboltz/PM_c.ev_erg;

/* CONVERSIONS
 *
 * PA_gs.convrsns defines the external system of units (default is CGS)
 * multiplying by a PA_gs.convrsns converts a quantity from CGS to the external system
 *
 * e.g. length_external = length_cgs*PA_gs.convrsns[CM]
 */

    PA_gs.convrsns[K] = PM_c.kboltz/PM_c.ev_erg;

/* make the conversion factors consistent with these changes */
    PA_set_conversions(TRUE);

/* set up the physical constants in the external system of units */
    PA_physical_constants_ext();

/* carry out a few random extra tasks before reading an input file */
    PA_add_hook("base_name", (void *) init_prob);

    signal(SIGFPE, LR_SIGFPE_handler);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* INIT_VARS - initialize global variables that
 *           - are not restart variables
 *           - and are general service variables
 */

int init_vars(PA_package *pck)
   {

/* set the values of some global constants */
    assign_globals();

/* connect some ubiquitous global arrays */
    PA_CONNECT(reg_map, "reg_map", TRUE);
    PA_CONNECT(zone,    "zone", TRUE);

    PA_CONNECT(nodet, "nodet", TRUE);
    PA_CONNECT(node,  "node", TRUE);
    PA_CONNECT(rx,    "rx", TRUE);
    PA_CONNECT(vx,    "vx", TRUE);

    PA_CONNECT(ry, "ry", TRUE);
    PA_CONNECT(vy, "vy", TRUE);

/* zonal variables */
    PA_CONNECT(vol, "vol", TRUE);

/* initialize the volumes */
    comp_volume();

/* flatten multidimensional spatial references in plot requests */
    LR_flatten_plots();

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* GLOBAL_ENTRY - do some things for everybody */

int global_entry(PA_package *pck)
   {t     = pck->p_t;
    dt    = pck->p_dt;
    cycle = pck->p_cycle;

    pc = cycle;

    PARAM[1] = t;
    PARAM[4] = dt;
    SWTCH[3] = cycle;

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* COMP_VOLUME - compute the zone volumes */

void comp_volume(void)
   {

    volumew(vol, rx, ry);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* ASSIGN_GLOBALS - connect some global constants from the global
 *                - control set
 */

void assign_globals(void)
   {

    N_regs  = SWTCH[5];
    N_nodes = SWTCH[6];
    lrz     = SWTCH[7];
    frn     = SWTCH[8];
    lrn     = SWTCH[9];
    kmax    = SWTCH[10];
    lmax    = SWTCH[11];
    N_zones = SWTCH[12];
    frz     = SWTCH[13];

    kbnd    = kmax + 1;
    lbnd    = lmax + 1;

    t  = PARAM[1];
    dt = PARAM[4]*(PARAM[3] - PARAM[2]);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* PART_REG - give all parts with the same name the same material region */

int part_reg(char *s)
   {PM_part *ipart;

    for (ipart = parts; ipart != NULL; ipart = ipart->next)
        if (strcmp(ipart->name, s) == 0)
           return(ipart->reg);

    N_regs++;

    return(N_regs);}

/*--------------------------------------------------------------------------*/

/*                            SX CONNECTIONS                                */

/*--------------------------------------------------------------------------*/

/* B_VARIABLES - define some compiled objects to the interpreter */

static object *B_variables(SS_psides *si)
   {

/* these are required */
    SS_install_cf(si, "number-of-zones",
		  "Number of zones in the current problem",
		  SS_acc_int,
                  &PA_gs.global_swtch[12]);		  

    SS_install_cf(si, "current-cycle",
		  "Current problem cycle",
		  SS_acc_int,
                  &cycle);

    SS_install_cf(si, "current-time",
		  "Current problem time",
		  SS_acc_double,
                  &t);

    SS_install_cf(si, "current-timestep",
		  "Current problem time step",
		  SS_acc_double,
                  &dt);

    SS_install_cf(si, "state-file-name",
		  "Current state file name",
		  SS_acc_ptr,
                  &PA_gs.global_name[2]);

    SS_install_cf(si, "edit-file-name",
		  "Current ASCII edit file name",
		  SS_acc_ptr,
                  &PA_gs.global_name[3]);

    SS_install_cf(si, "pp-file-name",
		  "Current post processor file name",
		  SS_acc_ptr,
                  &PA_gs.global_name[4]);

    SS_install_cf(si, "PVA-file-name",
		  "Current PVA file name",
		  SS_acc_ptr,
                  &PA_gs.global_name[5]);

/* these are not required */
    SS_install_cf(si, "initial-dtf",
		  "Initial time step fraction",
		  SS_acc_double,
                  &PA_gs.global_param[4]);

    SS_install_cf(si, "minimum-dtf",
		  "Minimum time step fraction",
		  SS_acc_double,
                  &PA_gs.global_param[5]);

    SS_install_cf(si, "maximum-dtf",
		  "Maximum time step fraction",
		  SS_acc_double,
                  &PA_gs.global_param[6]);

    SS_install_cf(si, "increment-dtf",
		  "Maximum time step increment factor",
		  SS_acc_double,
                  &PA_gs.global_param[7]);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* B_FILES - return a list of file names
 *         - (state    pp     PVA     edit)
 *         -  NAME[2] NAME[5] NAME[3] NAME[4]
 */

static object *B_files(SS_psides *si)
   {object *o;

    if ((PA_gs.global_name[2] == NULL) || (PA_gs.global_name[3] == NULL) ||
	(PA_gs.global_name[4] == NULL) || (PA_gs.global_name[5] == NULL))
       o = SS_null;

    else
       o = SS_make_list(si,
			SC_STRING_I, PA_gs.global_name[2],
			SC_STRING_I, PA_gs.global_name[5],
			SC_STRING_I, PA_gs.global_name[3],
			SC_STRING_I, PA_gs.global_name[4],
			0);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* B_INIT - install the B specific objects in the Scheme world */

void B_init(void)
   {SS_psides *si;

    si = SS_get_current_scheme(-1);

    SX_init(CODE, VERSION, 0, NULL, NULL);

    SS_install(si, "def-cc-vars",
               "Define compiled variables to interpreter",
	       SS_zargs,
	       B_variables, SS_PR_PROC);

    SS_install(si, "plot-var",
               "Plots the given variables",
	       SS_nargs,
	       LR_var_plot, SS_PR_PROC);

    SS_install(si, "files",
               "Return a list of files: state, pp, PVA, ascii",
	       SS_zargs,
	       B_files, SS_PR_PROC);

    SS_install(si, "time-data",
               "Return a list with t, dt, and cycle",
	       SS_zargs,
	       LR_get_time_data, SS_PR_PROC);

    SS_install(si, "mesh-data",
               "Return a list with N_zones, frz, lrz, frn, lrn, and optionally kmax and lmax",
	       SS_zargs,
	       LR_get_mesh_data, SS_PR_PROC);

    SS_install(si, "domain",
               "Define a domain set on which to plot appropriate variables",
	       SS_nargs,
	       LR_def_domain, SS_UR_MACRO);

    SS_install(si, "domain*",
               "Define a domain set on which to plot appropriate variables",
	       SS_nargs,
	       LR_def_domain, SS_PR_PROC);

    SX_gs.pan_data_hook = (PFInt) LR_get_data;

    PRINT(stdout, "\n");
    SC_strncpy(si->prompt, MAXLINE, "B-> ", -1);

    return;}

/*--------------------------------------------------------------------------*/

/*                           RANDOM HELPERS                                 */

/*--------------------------------------------------------------------------*/

/* LR_SIGFPE_HANDLER - handle floating point exception signals */

void LR_SIGFPE_handler(int sig)
   {

    PRINT(stdout, "ERROR: FLOATING POINT EXCEPTION\n");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
