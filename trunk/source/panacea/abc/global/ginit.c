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

static object
 *B_variables(void),
 *B_files(void);

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
    int load_sch, interactive_flag, commnd_flag;
    char commnd[MAXLINE];

/* process the command line arguments */
    interactive_flag = FALSE;
    commnd_flag      = FALSE;
    fname            = NULL;
    load_sch         = 0;
    n_threads        = 1;
    for (i = 1; i < c; i++)
        if (v[i][0] == '-')
           {switch (v[i][1])
               {case 'i' : interactive_flag = TRUE;
                           break;
                case 'l' : load_sch = ++i;
                           break;
                case 'n' : n_threads = SC_stoi(v[++i]);
                           break;
                default  : break;};}
         else if (v[i][0] != '(')
            fname = v[i];
         else
           {commnd_flag = TRUE;
            strcpy(commnd, " ");
            for ( ; i < c; i++)
               {SC_strcat(commnd, MAXLINE, v[i]);
                SC_strcat(commnd, MAXLINE, " ");};};

    n_threads = max(n_threads, 1);
    PA_init_threads(n_threads, NULL);

    if (fname != NULL)
       {PA_rd_restart(fname, NONE);
        NAME[2] = SC_strsave(fname);};

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

    NAME[8] = SC_strsave(s);

    N_parts  = 0;
    N_regs   = 0;
    N_plots  = 0;
    N_graphs = 0;

    plot_reqs   = NULL;
    iv_spec_lst = NULL;
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
   {STAND_ALONE = TRUE;

    PA_def_units(FALSE);

/* UNITS
 *
 * unit defines the internal system of units (default is physicist units)
 * multiplying by a unit converts a quantity from CGS to the internal system
 *
 * e.g. length_internal = length_cgs*unit[CM]
 */

/* set up the physical constants in CGS units */
    PA_physical_constants_cgs();

/* the default internal system of units
 * is "natural units" with Hbar = c = 1
 */
    if (def_unit_flag)
       {unit[G]   = g_icm;
        unit[SEC] = c;
        unit[K]   = eV_icm*(kBoltz/eV_erg);
        unit[EV]  = eV_icm*unit[Q]*unit[Q]/unit[CM];}

/* the alternate default system of units is the modified CGS system
 * these values should be used in the generator deck if desired
 */
    else
       unit[K] = kBoltz/eV_erg;

/* CONVERSIONS
 *
 * convrsn defines the external system of units (default is CGS)
 * multiplying by a convrsn converts a quantity from CGS to the external system
 *
 * e.g. length_external = length_cgs*convrsn[CM]
 */

    convrsn[K] = kBoltz/eV_erg;

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
   {int i;
    PM_part *ipart;

    for (i = 0, ipart = parts; ipart != NULL; ipart = ipart->next)
        if (strcmp(ipart->name, s) == 0)
           return(ipart->reg);

    N_regs++;
    return(N_regs);}

/*--------------------------------------------------------------------------*/

/*                            SX CONNECTIONS                                */

/*--------------------------------------------------------------------------*/

/* B_INIT - install the B specific objects in the Scheme world */

void B_init(void)
   {

    SX_init(CODE, VERSION);

    SS_install("def-cc-vars",
               "Define compiled variables to interpreter",
	       SS_zargs,
	       B_variables, SS_PR_PROC);

    SS_install("plot-var",
               "Plots the given variables",
	       SS_nargs,
	       LR_var_plot, SS_PR_PROC);

    SS_install("files",
               "Return a list of files: state, pp, PVA, ascii",
	       SS_zargs,
	       B_files, SS_PR_PROC);

    SS_install("time-data",
               "Return a list with t, dt, and cycle",
	       SS_zargs,
	       LR_get_time_data, SS_PR_PROC);

    SS_install("mesh-data",
               "Return a list with N_zones, frz, lrz, frn, lrn, and optionally kmax and lmax",
	       SS_zargs,
	       LR_get_mesh_data, SS_PR_PROC);

    SS_install("domain",
               "Define a domain set on which to plot appropriate variables",
	       SS_nargs,
	       LR_def_domain, SS_UR_MACRO);

    SS_install("domain*",
               "Define a domain set on which to plot appropriate variables",
	       SS_nargs,
	       LR_def_domain, SS_PR_PROC);

    SX_pan_data_hook = (PFInt) LR_get_data;

    PRINT(stdout, "\n");
    strcpy(SS_prompt, "B-> ");

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* B_VARIABLES - define some compiled objects to the interpreter */

static object *B_variables(void)
   {

/* these are required */
    SS_install_cf("number-of-zones",
		  "Number of zones in the current problem",
		  SS_acc_int,
                  &global_swtch[12]);		  

    SS_install_cf("current-cycle",
		  "Current problem cycle",
		  SS_acc_int,
                  &cycle);

    SS_install_cf("current-time",
		  "Current problem time",
		  SS_acc_double,
                  &t);

    SS_install_cf("current-timestep",
		  "Current problem time step",
		  SS_acc_double,
                  &dt);

    SS_install_cf("state-file-name",
		  "Current state file name",
		  SS_acc_ptr,
                  &global_name[2]);

    SS_install_cf("edit-file-name",
		  "Current ASCII edit file name",
		  SS_acc_ptr,
                  &global_name[3]);

    SS_install_cf("pp-file-name",
		  "Current post processor file name",
		  SS_acc_ptr,
                  &global_name[4]);

    SS_install_cf("PVA-file-name",
		  "Current PVA file name",
		  SS_acc_ptr,
                  &global_name[5]);

/* these are not required */
    SS_install_cf("initial-dtf",
		  "Initial time step fraction",
		  SS_acc_double,
                  &global_param[4]);

    SS_install_cf("minimum-dtf",
		  "Minimum time step fraction",
		  SS_acc_double,
                  &global_param[5]);

    SS_install_cf("maximum-dtf",
		  "Maximum time step fraction",
		  SS_acc_double,
                  &global_param[6]);

    SS_install_cf("increment-dtf",
		  "Maximum time step increment factor",
		  SS_acc_double,
                  &global_param[7]);

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* B_FILES - return a list of file names
 *         - (state    pp     PVA     edit)
 *         -  NAME[2] NAME[5] NAME[3] NAME[4]
 */

static object *B_files(void)
   {object *o;

    if ((global_name[2] == NULL) || (global_name[3] == NULL) ||
	(global_name[4] == NULL) || (global_name[5] == NULL))
       o = SS_null;

    else
       o = SS_make_list(SC_STRING_I, global_name[2],
			SC_STRING_I, global_name[5],
			SC_STRING_I, global_name[3],
			SC_STRING_I, global_name[4],
			0);

    return(o);}

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
