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

PFByte
 SX_plot_hook = NULL;

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
 
void SX_install_funcs(void)
   {

    PA_init_strings();

    SX_install_global_vars();

/* PDBLib related functions */
    SX_install_pdb_funcs();

#ifndef SPDBX_ONLY

/* PANACEA related functions */
    SX_install_panacea_funcs();

/* PGS related functions */
    SX_install_pgs_funcs();

/* PML related functions */
    SX_install_pml_funcs();

/* install the SX math handled functions */
    SX_mf_install();

/* pure SX functions */
    SX_install_global_funcs();

#endif

    SX_install_ascii_funcs();

    SX_install_file_funcs();

    SX_install_ext_funcs();

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_INIT - initialize SX */

void SX_init(char *code, char *vers)
   {

/* scheme initializations */
    SS_init_scheme(code, vers);

    SS_init_cont();
    
/* SX initializations depending on scheme */
    SX_install_funcs();

    SX_vif  = PD_open_vif("SX_vif");
    SX_gvif = _SX_mk_file("virtual-internal", PDBFILE_S, SX_vif);
    SX_ovif = SX_mk_gfile(SX_gvif);

/* PDB initializations */
    pdb_wr_hook = _SX_hash_hook;
    _SX_init_hash_objects(SX_vif);

/* default formats */
    _PD_set_digits(SX_vif);
    _SC_set_format_defaults();

#ifndef SPDBX_ONLY

/* PANACEA initializations */
    PA_def_str(SX_vif);

    _SX_var_tab = SS_mk_hasharr(PA_variable_tab);
    SS_UNCOLLECT(_SX_var_tab);
    if (SC_hasharr_install(SS_symtab, "pa-variable-table", _SX_var_tab,
			   SS_POBJECT_S, TRUE, TRUE) == NULL)
       SS_error("CAN'T INSTALL PANACEA DATA BASE - SX_INIT_SYSTEM",
                _SX_var_tab);

#endif

/* these lisp package special variables are initialized in all modes
 * give default values to the lisp package interface variables
 * set some default values for flags
 */
    SS_pr_ch_un   = SS_unget_ch;
    SS_pr_ch_out  = SS_put_ch;
    SS_print_flag = TRUE;
    SS_stat_flag  = TRUE;

    SS_set_prompt("SX-> ");

    SS_set_print_err_func(NULL, TRUE);

    SS_arg_hook          = _SX_args;
    SS_call_arg_hook     = _SX_call_args;
    SC_gs.atof           = SC_atof;
    SC_gs.strtod         = SC_strtod;
    SC_gs.type_container = SX_type_container;

    SS_interactive = TRUE;

    SX_file_exist_action = FAIL;

    return;}

/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_ARG_PREP - prepare the arg list by merging lists and
 *             - delistifying one element lists
 */

object *SX_arg_prep(object *argl)
   {object *acc, *lst, *obj;

/* because argl might be SS_Argl and since it is passed as an argument to
 * a function instead of being SS_Assign'd in the usual manner we MUST
 * protect against GC'ing it when the reference from SS_Argl is lost
 */
    SS_MARK(argl);
    SS_Assign(SS_Argl, SS_null);

/* make a copy of the arg list other people may be pointing at it */
    for (lst = SS_null, acc = argl; !SS_nullobjp(acc); acc = SS_cdr(acc))
        lst = SS_mk_cons(SS_car(acc), lst);
    SS_MARK(lst);

    acc = SS_null;
    while (!SS_nullobjp(lst))
       {obj = SS_car(lst);
        if (SS_consp(obj))
           {SS_Assign(acc, SS_append(obj, acc));}
        else
           {SS_Assign(acc, SS_append(SS_mk_cons(obj, SS_null), acc));};

/* this frees the cons we made above */
        SS_Assign(lst, SS_cdr(lst));};

/* undo the additional reference that was added at the beginning */
    SS_Assign(argl, SS_null);

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

int _SX_get_input(object *str)
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
	       SX_plot_hook();
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
