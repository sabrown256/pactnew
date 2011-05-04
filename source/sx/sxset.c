/*
 * SXSET.C - set up routines for SX and SPDBX
 *
 * Source Version: 3.0
 * Software Release #: LLNL-CODE-422942
 *
 */

#include "cpyright.h"
 
#include "sx_int.h"

#define ELSE_IF(str, typ)                                                    \
    else if (strcmp(type, str) == 0)                                         \
       *((typ *) (var->val)) =  *((typ *) val)

char
 *SX_current_palette = NULL,
 *SX_console_type;

double
 SX_console_x,
 SX_console_y,
 SX_console_width,
 SX_console_height;

object
 *(*SX_plot_hook)(SS_psides *si);

FILE
 *SX_command_log = NULL;

char
 *SX_command_log_name = NULL;

int
 SX_prefix_list[NPREFIX];

PDBfile
 *SX_vif;

g_file
 *SX_gvif;

object
 *SX_ovif;

char
 SX_err[MAXLINE];

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

static INLINE int EOI(object *str)
   {int rv;
    char *p, *s;

    p = SS_PTR(str);
    s = SS_BUFFER(str);

/*    rv = (((p != s) && (*(p - 1) == '\n')) || (*p == '\0')); */
    rv = (((p != s) && (*(p - 1) == '\n') && (*p == '\0')) || (*p == '\0'));

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_FUNCS - install the extended Scheme functions */
 
void SX_install_funcs(SS_psides *si)
   {

    PA_init_strings();

    SX_install_global_vars(si);

/* PDBLib related functions */
    SX_install_pdb_funcs(si);

#ifndef SPDBX_ONLY

/* PANACEA related functions */
    SX_install_panacea_funcs(si);

/* PGS related functions */
    SX_install_pgs_funcs(si);

/* PML related functions */
    SX_install_pml_funcs(si);

/* install the SX math handled functions */
    SX_mf_install(si);

/* pure SX functions */
    SX_install_global_funcs(si);

#endif

    SX_install_ascii_funcs(si);

    SX_install_file_funcs(si);

    SX_install_ext_funcs(si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INIT - initialize SX */

SS_psides *SX_init(char *code, char *vers)
   {SS_psides *si;

/* scheme initializations */
    si = SS_init_scheme(code, vers);

    SS_init_cont(si);
    
/* SX initializations depending on scheme */
    SX_install_funcs(si);

    SX_vif  = PD_open_vif("SX_vif");
    SX_gvif = _SX_mk_file("virtual-internal", PDBFILE_S, SX_vif);
    SX_ovif = SX_mk_gfile(si, SX_gvif);

/* PDB initializations */
    pdb_wr_hook = _SX_hash_hook;
    SC_register_context(_SX_hash_hook, si);

    _SX_init_hash_objects(si, SX_vif);

/* default formats */
    _PD_set_digits(SX_vif);
    _SC_set_format_defaults();

#ifndef SPDBX_ONLY

/* PANACEA initializations */
    PA_def_str(SX_vif);

    _SX_var_tab = SS_mk_hasharr(si, PA_variable_tab);
    SS_UNCOLLECT(_SX_var_tab);
    if (SC_hasharr_install(si->symtab, "pa-variable-table", _SX_var_tab,
			   SS_POBJECT_S, TRUE, TRUE) == NULL)
       SS_error(si, "CAN'T INSTALL PANACEA DATA BASE - SX_INIT_SYSTEM",
                _SX_var_tab);

#endif

/* these lisp package special variables are initialized in all modes
 * give default values to the lisp package interface variables
 * set some default values for flags
 */
    si->pr_ch_un    = SS_unget_ch;
    si->pr_ch_out   = SS_put_ch;
    si->print_flag  = TRUE;
    si->stat_flag   = TRUE;
    si->get_arg     = _SX_args;
    si->call_arg    = _SX_call_args;
    si->interactive = TRUE;

    SS_set_prompt(si, "SX-> ");

    SS_set_print_err_func(NULL, TRUE);

    SC_gs.atof           = SC_atof;
    SC_gs.strtod         = SC_strtod;
    SC_gs.type_container = _SX_type_container;

    SC_register_context(_SX_type_container, si);

    SX_file_exist_action = FAIL;

    return(si);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_ARG_PREP - prepare the arg list by merging lists and
 *             - delistifying one element lists
 */

object *SX_arg_prep(SS_psides *si, object *argl)
   {object *acc, *lst, *obj;

/* because argl might be si->argl and since it is passed as an argument to
 * a function instead of being SS_assign'd in the usual manner we MUST
 * protect against GC'ing it when the reference from si->argl is lost
 */
    SS_mark(argl);
    SS_assign(si, si->argl, SS_null);

/* make a copy of the arg list other people may be pointing at it */
    for (lst = SS_null, acc = argl; !SS_nullobjp(acc); acc = SS_cdr(si, acc))
        lst = SS_mk_cons(si, SS_car(si, acc), lst);
    SS_mark(lst);

    acc = SS_null;
    while (!SS_nullobjp(lst))
       {obj = SS_car(si, lst);
        if (SS_consp(obj))
           {SS_assign(si, acc, SS_append(si, obj, acc));}
        else
           {SS_assign(si, acc, SS_append(si, SS_mk_cons(si, obj, SS_null), acc));};

/* this frees the cons we made above */
        SS_assign(si, lst, SS_cdr(si, lst));};

/* undo the additional reference that was added at the beginning */
    SS_assign(si, argl, SS_null);

    return(acc);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_NEXT_PREFIX - return the index of the next available menu prefix */

int SX_next_prefix(void)
   {int i;

    for (i = 0; i < NPREFIX; i++)
        if (SX_prefix_list[i] == 0)
           return(i);

    i = NPREFIX - 1;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_RESET_PREFIX - reset all prefixes to 0 */

void SX_reset_prefix(void)
   {int i;

    for (i = 0; i < NPREFIX; i++)
        SX_prefix_list[i] = 0;

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_GET_INPUT - get more text to feed SS_get_ch
 *               - return -1 to break
 *               -         0 to continue
 *               -         1 for further processing
 */

int _SX_get_input(SS_psides *si, object *str)
   {int rv;
    char *p;
    FILE *fp;

    rv = 0;

    if (_SX.gets == NULL)
       {if (PG_console_device == NULL)
	   _SX.gets = io_gets;
        else
	   _SX.gets = (PFfgets) PG_wind_fgets;};

    fp = SS_INSTREAM(str);

    if (fp == NULL)
       rv = -1;

    else if (fp == stdin)
       {

#ifdef HAVE_READLINE
        rl_getc_function = PG_wind_fgetc;
	p = SC_prompt(_SS.pr_prompt, SS_BUFFER(str), MAXLINE);
#else
	if (_SS.pr_prompt != NULL)
	   PRINT(stdout, "%s", _SS.pr_prompt);
	p = _SX.gets(SS_BUFFER(str), MAXLINE, fp);
#endif

	_SS.pr_prompt = NULL;

	if (p == NULL)
	   rv = 0;

/* the \r check is for the benefit of those non-UNIX systems who use it */
	else if ((*p == '\n') || (*p == '\r'))
	   {if ((SX_autoplot == ON) && (SX_plot_hook != NULL))
	       SX_plot_hook(si);
	    rv = -1;};}

    else
       p = _SX.gets(SS_BUFFER(str), MAXLINE, fp);

    if (p == NULL)
       {*SS_PTR(str) = (char) EOF;
	rv = -1;}

    else 
       SS_PTR(str) = SS_BUFFER(str);

    return(rv);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
