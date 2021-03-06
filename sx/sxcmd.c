/*
 * SXCMD.C - high level session support for SX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"
#include "scope_raster.h"
#include "syntax.h"
#include "scope_proc.h"

#define PCODE "PDBView 2.0"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DESCRIBE - the SX documentation function */

static object *_SXI_describe(SS_psides *si, object *argl)
   {object *obj;

    SX_prep_arg(si, argl);

    for ( ; !SS_nullobjp(argl); argl = SS_cdr(si, argl))
        {obj = SS_car(si, argl);
         if (obj != NULL)
            {if (!SS_prim_des(si, si->outdev, obj))
                PRINT(stdout, " Unknown function\n");};};

    return(SS_f);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_END - gracefully exit from SX */

void SX_end(SS_psides *si, int val)
   {

    SC_exit_all();

    SS_end_scheme(si, val);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_RD_SCM - do a SCHEME level open with error protection */

static int _SX_rd_scm(SS_psides *si)
   {char *cmd;

    cmd = (SX_gs.sm == SX_MODE_PDBVIEW) ? "cf" : "rd";

    SS_call_scheme(si, cmd,
                   G_STRING_I, _SX.bf,
                   0);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_RD_SCM - do a SCHEME level rd with error protection */

int SX_rd_scm(SS_psides *si, const char *name)
   {int rv;

    SC_strncpy(_SX.bf, MAXLINE, name, -1);

    rv = SS_err_catch(si, _SX_rd_scm, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INIT_VIEW - initialize the plot parameters */

void SX_init_view(SS_psides *si)
   {

    PG_set_attrs_glb(TRUE,
		     "numeric-data-id", TRUE,
		     "restore-viewport", TRUE,
		     NULL);

/*    SX_gs.gr_mode         = TRUE; */
    SX_gs.plot_flag       = TRUE;
    SX_gs.default_npts    = 100;

    PG_box_init(3, SX_gs.gwc, 0.0, 1.0);

    SX_gs.view_x[0]       = 0.18;
    SX_gs.view_x[1]       = 0.95;
    SX_gs.view_x[2]       = 0.2;
    SX_gs.view_x[3]       = 0.95;
    SX_gs.view_x[4]       = 0.0;
    SX_gs.view_x[5]       = 1.0;

    SX_gs.view_dx[0]      = 0.75;
    SX_gs.view_dx[1]      = 0.75;
    SX_gs.view_aspect     = 1.0;
    SX_gs.window_x[0]     = 0.5;
    SX_gs.window_x[1]     = 0.1;
    SX_gs.window_dx[0]    = 0.4;
    SX_gs.window_dx[1]    = 0.4;
    SX_gs.window_dx_P[0]  = 1.0;
    SX_gs.window_dx_P[1]  = 1.0;

    SX_gs.display_name  = CSTRSAVE("WINDOW");
    SX_gs.display_type  = CSTRSAVE("COLOR");
    SX_gs.display_title = CSTRSAVE("PDBView");

    SS_gs.fmts[1]       = CSTRSAVE("%13.6Le");

    si->interactive = FALSE;
    si->print_flag  = FALSE;
    si->stat_flag   = FALSE;

    SX_gs.command_log_name = CSTRSAVE("pdbview.log");

    SX_enlarge_dataset(si, NULL);

    return;}

/*--------------------------------------------------------------------------*/

/*                       MAPPING MANAGEMENT ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* _SX_MAPPING_REF - give a SCHEME level reference to a mapping */

static object *_SX_mapping_ref(SS_psides *si, const char *fname, int indx)
   {SX_file *po;
    object *ret;

    if (strcmp(fname, "#t") == 0)
       {for (po = SX_gs.file_list; po != NULL; po = po->next)
            {_SX_get_menu(si, po);

             ret = SX_get_ref_map(si, po, indx);
             if (!SS_nullobjp(ret))
                return(ret);}

	if (po == NULL)
	   return(SS_null);}

    else if ((strcmp(fname, "nil") == 0) || (strcmp(fname, "#f") == 0))
       {po = SX_gs.gvif;
        _SX_get_menu(si, po);

        ret = SX_get_ref_map(si, po, indx);
        if (!SS_nullobjp(ret))
          return(ret);}

    else
       {for (po = SX_gs.file_list; po != NULL; po = po->next)
            {if (strcmp(fname, po->name) == 0)
                break;};

        if (po == NULL)
           return(SS_null);

        _SX_get_menu(si, po);

        ret = SX_get_ref_map(si, po, indx);
        if (!SS_nullobjp(ret))
           return(ret);};

    return(SS_null);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SXI_MAPPING_REF - give a SCHEME level reference to a mapping */

static object *SXI_mapping_ref(SS_psides *si, object *argl)
   {int indx;
    char *fname, *dtype;
    object *o;

    fname = NULL;
    dtype = NULL;
    indx  = -1;
    SS_args(si, argl,
            G_STRING_I, &fname,
            G_STRING_I, &dtype,
            G_INT_I, &indx,
            0);

    o = _SX_mapping_ref(si, fname, indx);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INIT_MAPPINGS - initialize the mapping data and references */

void SX_init_mappings(SS_psides *si)
   {

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_NEXT_NUMBER - return the index for the next available mapping
 *                - menu slot
 */

int SX_next_number(void)
   {int rv;

    rv = SX_next_number();

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MK_MAPPING_PROC - return a Scheme procedure object made from the
 *                    - specified mapping
 *                    - also bind the variable whose name is the lower
 *                    - case version of the mapping label (mapping.id) to
 *                    - the new object
 */

object *SX_mk_mapping_proc(int i)
   {object *obj;

    obj = SS_null;

/*
    object *p;

    s[0] = SX_gs.dataset[i].id;
    s[1] = '\0';
    
    j = _G_PM_MAPPING_Id(s[0]);

    obj = SX_get_curve_obj(j);

    p = SX_get_curve_proc(j);

    SS_PROCEDURE_DOC(p)    = SX_gs.dataset[i].text;
    SS_VARIABLE_VALUE(obj) = p;
    SS_VARIABLE_NAME(obj)  = SS_PROCEDURE_NAME(p);

    SX_gs.dataset[i].obj = (void *) obj;
*/

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SET_ID - change the data-id of the given mapping */

object *SX_set_id(SS_psides *si, object *argl)
   {int iold;
    object *o;

    SX_prep_arg(si, argl);

    iold = -1;

    o = SX_mk_mapping_proc(iold);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_THRU - return an expanded list of mappings */

static object *_SXI_thru(SS_psides *si, object *argl)
   {int n;
    object *ret;
    SX_file *po;

    ret = SS_null;

    SX_prep_arg(si, argl);

    if (SS_numbp(SS_car(si, argl)))
       {int first, last, id;

	first = 0;
	last  = 0;
        SS_args(si, argl,
                G_INT_I, &first,
                G_INT_I, &last,
                0);

        if (first < 1)
           SS_error(si,
		      "FIRST ARGUMENT NOT A VALID MAPPING NUMBER - _SXI_THRU",
		      argl);

        if (last < 1)
           SS_error(si,
		      "SECOND ARGUMENT NOT A VALID MAPPING NUMBER - _SXI_THRU",
		      argl);

/* GOTCHA: in general, when processing something like "f1.1:4",
           f1 will not refer to the current input file */

	SS_args(si, SS_lk_var_val(si, SX_gs.curfile),
		G_SX_FILE_I, &po,
		0);

	_SX_get_menu(si, po);

	n = SC_array_get_n(po->menu_lst);
        if (first <= last)
           {last = min(last, n);
            for (id = first; id <= last; id++)
                SS_assign(si, ret, SS_mk_cons(si, SS_mk_integer(si, id), ret));}
        else
           {first = min(first, n);
            for (id = first; id >= last; id--)
                SS_assign(si, ret, SS_mk_cons(si, SS_mk_integer(si, id), ret));};}

    SX_prep_ret(si, ret);

    return(ret);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_XINDEX_MAPPING - make a copy of the mapping indexed by j
 *                   - with x values replaced by the index of the x values
 *                   - and return the objectified mapping SX_label
 */

object *SX_xindex_mapping(int j)
   {int i;
    object *o;

    i = 0;
    o = SX_mk_mapping_proc(i);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MAPPING_ID - given the mapping character return
 *                - the mapping name identifier
 */

char *_SX_mapping_id(int c)
   {char *rv;

    rv = NULL;

#if 0
    if (islower(c))
       rv = min(toupper(c)-'A', SX_gs.n_curves);
    else
       rv = c - 'A';
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_MAPPING_ID - given the mapping object return the
 *               - mapping name identifier
 */

char *SX_mapping_id(object *c)
   {char *rv;

    rv = NULL;

#if 0
    rv = _SX_mapping_id(*SX_get_string(c));
#endif

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INIT_ENV - initialize the SCHEME environment variables for PDBView */

void SX_init_env(SS_psides *si)
   {

/* initialize the prefix list */
    SX_reset_prefix();

    SX_gs.pui_file  = CSTRSAVE("pdbview.pui");

/* initialize the some global objects */

    SX_gs.curfile = SS_install_cv(si, "current-file", SS_null, G_OBJECT_I);

    SS_install(si, "describe*",
               "Procedure: Describe an SX function or variable\n     Usage: describe* <function-list> <variable-list>",
               SS_nargs,
               _SXI_describe, SS_PR_PROC);

    SS_install(si, "describe",
               "Macro: Describe an SX function or variable\n     Usage: describe <function-list> <variable-list>",
               SS_nargs,
               _SXI_describe, SS_UR_MACRO);

    SS_install(si, "thru",
               "Procedure: Return an expanded list of mappings\n     Usage: thru <first-mapping> <last-mapping>",
               SS_nargs,
               _SXI_thru, SS_PR_PROC);

    SS_install(si, "mapping-ref",
               "Find a mapping object.\nFORM (mapping-ref <file> <type> <index>)",
               SS_nargs,
               SXI_mapping_ref, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_COMMAND - load a file and run a command
 *            - return the exit status of the command
 */

int SX_command(SS_psides *si, const char *file, const char *cmd)
   {int ret, zsp;
    static int first = TRUE;

/* NOTE: be able to access remote files
 * this MUST be set before the PD_init_threads uses the current
 * io hooks to initialize the pio hooks
 */
    if (first == TRUE)
       {first = FALSE;
	SC_io_connect(SC_REMOTE);

	PD_set_fmt_version(3);

	PD_init_threads(0, NULL);

	SC_set_io_interrupts(FALSE);

	SS_init(si, "Aborting with error", SX_end,
		TRUE, SS_interrupt_handler,
		TRUE, NULL, 0);

/* connect the I/O functions */
	SS_set_put_line(si, SS_printf);
	SS_set_put_string(si, SS_fputs);
	SC_set_get_line(io_gets);

	SC_set_io_interrupts(FALSE);

	SX_gs.gr_mode = TRUE;
	SX_gs.qflag   = TRUE;
	zsp        = 2;

	SC_zero_space_n(zsp, -2);

/* initialize SX
 * the following variables must be initialized before SX_init
 */
	SX_gs.console_type           = CSTRSAVE("MONOCHROME");
	SX_gs.console_x[0]           = 0.0;
	SX_gs.console_x[1]           = 0.0;
	SX_gs.console_dx[0]          = 0.33;
	SX_gs.console_dx[1]          = 0.33;
	SX_gs.background_color_white = TRUE;

	SX_init(PCODE, VERSION, 0, NULL, NULL);
	si->trap_error = FALSE;

	SX_init_view(si);
	SX_install_global_vars(si);
	SX_init_mappings(si);
	SX_init_env(si);

	SS_load_scm(si, "nature.scm");

	PG_fset_use_pixmap(TRUE);

/* initialize the available syntax modes */
	DEF_SYNTAX_MODES(si);

	if (SX_gs.gr_mode)
	   SX_mode_graphics(si);
	else
	   SX_mode_text(si);

	PG_expose_device(PG_gs.console);

	si->nsave        = 0;
	si->nrestore     = 0;
	si->nsetc        = 0;
	si->ngoc         = 0;
	si->bracket_flag = TRUE;

	SC_mem_stats_set(0L, 0L);};

    SS_load_scm(si, file);

    ret = SS_run(si, cmd);

    return(ret);}
        
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
