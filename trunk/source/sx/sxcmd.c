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

C_procedure
 *Mapping_Proc;

static char
 _SX_bf[MAXLINE];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_DESCRIBE - the SX documentation function */

static object *_SXI_describe(SS_psides *si, object *argl)
   {object *obj;

    SX_prep_arg(argl);

    for ( ; !SS_nullobjp(argl); argl = SS_cdr(argl))
        {obj = SS_car(argl);
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
   {

    SS_call_scheme(si, "cf",
                   SC_STRING_I, _SX_bf,
                   0);

    return(TRUE);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_RD_SCM - do a SCHEME level rd with error protection */

int SX_rd_scm(char *name)
   {int rv;
    SS_psides *si;

    si = &_SS_si;

    strcpy(_SX_bf, name);

    rv = SS_err_catch(si, _SX_rd_scm, NULL);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INIT_VIEW - initialize the plot parameters */

void SX_init_view(void)
   {SS_psides *si;

    si = &_SS_si;

    PG_set_attrs_glb(TRUE,
		     "numeric-data-id", TRUE,
		     "restore-viewport", TRUE,
		     NULL);

/*    SX_gr_mode         = TRUE; */
    SX_plot_flag       = TRUE;
    SX_default_npts    = 100;

    PG_box_init(3, SX_gwc, 0.0, 1.0);

    SX_view_x[0]       = 0.18;
    SX_view_x[1]       = 0.95;
    SX_view_x[2]       = 0.2;
    SX_view_x[3]       = 0.95;
    SX_view_x[4]       = 0.0;
    SX_view_x[5]       = 1.0;

    SX_view_width      = 0.75;
    SX_view_height     = 0.75;
    SX_view_aspect     = 1.0;
    SX_window_x[0]     = 0.5;
    SX_window_x[1]     = 0.1;
    SX_window_width    = 0.4;
    SX_window_height   = 0.4;
    SX_window_width_P  = 1.0;
    SX_window_height_P = 1.0;

    SX_text_output_format = CSTRSAVE("%13.6e");

    SX_display_name  = CSTRSAVE("WINDOW");
    SX_display_type  = CSTRSAVE("COLOR");
    SX_display_title = CSTRSAVE("PDBView");

    si->interactive = FALSE;
    si->print_flag  = FALSE;
    si->stat_flag   = FALSE;

    SX_command_log_name = CSTRSAVE("pdbview.log");

    SX_enlarge_dataset(NULL);

    return;}

/*--------------------------------------------------------------------------*/

/*                       MAPPING MANAGEMENT ROUTINES                        */

/*--------------------------------------------------------------------------*/

/* _SX_MAPPING_REF - give a SCHEME level reference to a mapping */

static object *_SX_mapping_ref(char *fname, char *dtype, int indx)
   {g_file *po;
    object *ret;
    SS_psides *si;

    si = &_SS_si;

    if (strcmp(fname, "#t") == 0)
       {for (po = SX_file_list; po != NULL; po = po->next)
            {_SX_get_menu(po);

             ret = SX_get_ref_map(si, po, indx, dtype);
             if (!SS_nullobjp(ret))
                return(ret);}

	if (po == NULL)
	   return(SS_null);}

    else if ((strcmp(fname, "nil") == 0) || (strcmp(fname, "#f") == 0))
       {po = SX_gvif;
        _SX_get_menu(po);

        ret = SX_get_ref_map(si, po, indx, dtype);
        if (!SS_nullobjp(ret))
          return(ret);}

    else
       {for (po = SX_file_list; po != NULL; po = po->next)
            {if (strcmp(fname, po->name) == 0)
                break;};

        if (po == NULL)
           return(SS_null);

        _SX_get_menu(po);

        ret = SX_get_ref_map(si, po, indx, dtype);
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
    SS_args(argl,
            SC_STRING_I, &fname,
            SC_STRING_I, &dtype,
            SC_INT_I, &indx,
            0);

    o = _SX_mapping_ref(fname, dtype, indx);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_MK_MAPPING - the C level function that creates a new mapping object */

object *_SX_mk_mapping(PM_mapping *f, g_file *po)
   {

/* be sure to make a copy of the data */

    return(SS_null);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_RL_MAPPING - release a mapping object */

object *SX_rl_mapping(char *s)
   {

    return(SS_null);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INIT_MAPPINGS - initialize the mapping data and references */

void SX_init_mappings(void)
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

    s[0] = SX_dataset[i].id;
    s[1] = '\0';
    
    j = _SX_mapping_id(s[0]);

    obj = SX_get_curve_obj(j);

    p = SX_get_curve_proc(j);

    SS_PROCEDURE_DOC(p)    = SX_dataset[i].text;
    SS_VARIABLE_VALUE(obj) = p;
    SS_VARIABLE_NAME(obj)  = SS_PROCEDURE_NAME(p);

    SX_dataset[i].obj = (void *) obj;
*/

    return(obj);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_SET_ID - change the data-id of the given mapping */

object *SX_set_id(object *argl)
   {int iold;
    object *o;

    SX_prep_arg(argl);

    iold = -1;

#if 0
    iold    = -1;
    id = '\0';
    SS_args(argl,
            SC_STRING_I, &iold,
            SC_STRING_I, &id,
            0);

    if (iold < 0)
       SS_error("BAD MAPPING ARGUMENT - SX_SET_ID", argl);

    if ((id == '\0') || (id < 'A') || (id > 'Z'))
       SS_error("BAD ID ARGUMENT - SX_SET_ID", argl);

    if (_SX_mappingp(&id))
       SS_error("SPECIFIED ID ALREADY IN USE - SX_SET_ID", argl);

    jnew = id - 'A';
    jold = SX_dataset[iold].id - 'A';

/* sever the connection with the old mapping */
    obj = SX_get_curve_obj(jold);
    SS_VARIABLE_VALUE(obj) = varbl[jold];
    SS_VARIABLE_NAME(obj)  = SS_VARIABLE_NAME(varbl[jold]);
    SX_data_index[jold] = -1;

/* make the connection with the new mapping */
    SX_dataset[iold].id  = id;
    SX_data_index[jnew] = iold;
#endif

    o = SX_mk_mapping_proc(iold);

    return(o);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SXI_THRU - return an expanded list of mappings */

static object *_SXI_thru(SS_psides *si, object *argl)
   {int n;
    object *ret;
    g_file *po;

    ret = SS_null;

    SX_prep_arg(argl);

    if (SS_numbp(SS_car(argl)))
       {int first, last, id;

	first = 0;
	last  = 0;
        SS_args(argl,
                SC_INT_I, &first,
                SC_INT_I, &last,
                0);

        if (first < 1)
           SS_error("FIRST ARGUMENT NOT A VALID MAPPING NUMBER - _SXI_THRU",
                    argl);

        if (last < 1)
           SS_error("SECOND ARGUMENT NOT A VALID MAPPING NUMBER - _SXI_THRU",
                    argl);

/* GOTCHA: in general, when processing something like "f1.1:4",
           f1 will not refer to the current input file */

	SS_args(SS_lk_var_val(si, SX_curfile),
		G_FILE, &po,
		0);

	_SX_get_menu(po);

	n = SC_array_get_n(po->menu_lst);
        if (first <= last)
           {last = min(last, n);
            for (id = first; id <= last; id++)
                SS_Assign(ret, SS_mk_cons(si, SS_mk_integer(si, id), ret));}
        else
           {first = min(first, n);
            for (id = first; id >= last; id--)
                SS_Assign(ret, SS_mk_cons(si, SS_mk_integer(si, id), ret));};}
#if 0
    else
       {char first = '\0', last = '\0', id;

        SS_args(argl,
                SX_DATA_ID_I, &first,
                SX_DATA_ID_I, &last,
                0);

        if ((first < 'A') || (first > 'Z'))
           SS_error("FIRST ARGUMENT NOT A VALID DATA-ID - _SXI_THRU",
                    argl);

        if ((last < 'A') || (last > 'Z'))
           SS_error("SECOND ARGUMENT NOT A VALID DATA-ID - _SXI_THRU",
                    argl);

        if (first <= last)
           {for (id = first; id <= last; id++)
                if (_SX_mappingp(&id))
                   SS_Assign(ret, SS_mk_cons(si, SX_get_curve_obj(id - 'A'),
					     ret));}
        else
           {for (id = first; id >= last; id--)
                if (_SX_mappingp(&id))
                   SS_Assign(ret, SS_mk_cons(si, SX_get_curve_obj(id - 'A'),
					     ret));};};
#endif
    SX_prep_ret(ret);

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
       rv = min(toupper(c)-'A', SX_N_Curves);
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

void SX_init_env(void)
   {

/* initialize the prefix list */
    SX_reset_prefix();

    SX_pui_file  = CSTRSAVE("pdbview.pui");

/* initialize the some global objects */

    SX_curfile = SS_install_cv("current-file", SS_null, SS_OBJECT_I);

    SS_install("describe*",
               "Procedure: Describe an SX function or variable\n     Usage: describe* <function-list> <variable-list>",
               SS_nargs,
               _SXI_describe, SS_PR_PROC);

    SS_install("describe",
               "Macro: Describe an SX function or variable\n     Usage: describe <function-list> <variable-list>",
               SS_nargs,
               _SXI_describe, SS_UR_MACRO);

    SS_install("thru",
               "Procedure: Return an expanded list of mappings\n     Usage: thru <first-mapping> <last-mapping>",
               SS_nargs,
               _SXI_thru, SS_PR_PROC);

    SS_install("mapping-ref",
               "Find a mapping object.\nFORM (mapping-ref <file> <type> <index>)",
               SS_nargs,
               SXI_mapping_ref, SS_PR_PROC);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_COMMAND - load a file and run a command
 *            - return the exit status of the command
 */

int SX_command(char *file, char *cmd)
   {int ret, zsp;
    SS_psides *si;
    static int first = TRUE;

    si = &_SS_si;

/* NOTE: be able to access remote files
 * this MUST be set before the PD_init_threads uses the current
 * io hooks to initialize the pio hooks
 */
    if (first == TRUE)
       {first = FALSE;
	SC_io_connect(SC_REMOTE);

	PD_set_format_version(3);

	PD_init_threads(0, NULL);

	PG_IO_INTERRUPTS(FALSE);

	SS_init(si, "Aborting with error", SX_end,
		TRUE, SS_interrupt_handler,
		TRUE, NULL, 0);

/* connect the I/O functions */
	SC_set_put_line(SS_printf);
	SC_set_put_string(SS_fputs);
	SC_set_get_line(io_gets);

	PG_IO_INTERRUPTS(FALSE);

	SX_gr_mode = TRUE;
	SX_qflag   = TRUE;
	zsp        = 2;

	SC_zero_space_n(zsp, -2);

/* initialize SX
 * the following variables must be initialized before SX_init
 */
	SX_console_type     = CSTRSAVE("MONOCHROME");
	SX_console_x        = 0.0;
	SX_console_y        = 0.0;
	SX_console_width    = 0.33;
	SX_console_height   = 0.33;
	SX_background_color_white = TRUE;

	SX_init(PCODE, VERSION);
	si->trap_error = FALSE;

	SX_init_view();
	SX_install_global_vars();
	SX_init_mappings();
	SX_init_env();

	SS_load_scm(si, "nature.scm");

	PG_set_use_pixmap(TRUE);

/* initialize the available syntax modes */
	DEF_SYNTAX_MODES(si);

	if (SX_gr_mode)
	   SX_mode_graphics();
	else
	   SX_mode_text();

	PG_expose_device(PG_console_device);

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
