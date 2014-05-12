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

SX_scope_private
 _SX = { FAIL, 3,
	 1.0e-15, 1.0e-3, 2.0,
	 'a', LITERAL, 1, 1, '@', "curves.a",
         TRUE, };

SX_scope_public
 SX_gs = {100, -1,
          0, 0, 1,
#if 0
          0,                     /* default to IWD interpolation */
#else
          -1,                    /* default to MQ interpolation */
#endif
          0.0, 1.1, 0.0, 1.0e-10,

	  12, {0.5, 0.01, 0.0}, {0.0, 0.0, 0.0}, };

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* _SX_INSTALL_GENERATED - install the automatically generated wrappers */

static void _SX_install_generated(SS_psides *si)
   {extern void SX_install_score_bindings(SS_psides *si);
    extern void SX_install_pml_bindings(SS_psides *si);
    extern void SX_install_pdb_bindings(SS_psides *si);
    extern void SX_install_ppc_bindings(SS_psides *si);
    extern void SX_install_pgs_bindings(SS_psides *si);
    extern void SX_install_panacea_bindings(SS_psides *si);
    extern void SX_install_scheme_bindings(SS_psides *si);

    SX_install_score_bindings(si);
    SX_install_pml_bindings(si);
    SX_install_pdb_bindings(si);
    SX_install_ppc_bindings(si);
    SX_install_pgs_bindings(si);
    SX_install_panacea_bindings(si);
    SX_install_scheme_bindings(si);

    return;}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
 
/* SX_INSTALL_FUNCS - install the extended Scheme functions */
 
void SX_install_funcs(SS_psides *si)
   {

    PA_init_strings();

    SX_install_global_vars(si);

    SX_install_modes(si);

    _SX_install_generated(si);

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

SS_psides *SX_init(char *code, char *vers, int c, char **v, char **env)
   {char *script;
    SS_psides *si;

/* scheme initializations */
    si = SS_init_scheme(code, vers, c, v, env, FALSE);

/* SX initializations depending on scheme */
    SX_install_funcs(si);

    SX_gs.vif  = PD_open_vif("SX_gs.vif");
    SX_gs.gvif = _SX_mk_file("virtual-internal", PDBFILE_S, SX_gs.vif);
    SX_gs.ovif = SX_mk_gfile(si, SX_gs.gvif);

/* PDB initializations */
    PD_gs.write = _SX_hash_hook;
    SC_REGISTER_CONTEXT(_SX_hash_hook, SS_psides, si);

    if ((SX_gs.vif != NULL) && (SX_gs.gvif != NULL))
       {_SX_init_hash_objects(si, SX_gs.vif);

/* default formats */
	_PD_set_digits(SX_gs.vif);
	_SC_set_format_defaults();

#ifndef SPDBX_ONLY

/* PANACEA initializations */
	PA_def_str(SX_gs.vif);

	SX_gs.var_tab = SS_mk_hasharr(si, PA_gs.variable_tab);
	SS_UNCOLLECT(SX_gs.var_tab);
	if (SC_hasharr_install(si->symtab, "pa-variable-table", SX_gs.var_tab,
			       SS_POBJECT_S, 3, -1) == NULL)
	   SS_error(si, "CAN'T INSTALL PANACEA DATA BASE - SX_INIT_SYSTEM",
		    SX_gs.var_tab);

#endif
	};

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

    SC_REGISTER_CONTEXT(_SX_type_container, SS_psides, si);

    _SX.file_exist_action = FAIL;

/* check if we are supposed to run a script and if so
 * load the script file - this might not return
 */
    script = SS_exe_script(c, v);
    if (script != NULL)
       {SX_gs.qflag = TRUE;
	if (SX_gs.sm == SX_MODE_PDBVIEW)
           SX_mode_pdbview(si, TRUE, TRUE);

	switch (SETJMP(SC_gs.cpu))
	   {case ABORT :
		 SX_end(si, ABORT);
		 exit(1);
	    case ERR_FREE :
	         SX_end(si, ERR_FREE);
		 exit(0);};
	SS_load_scm(si, script);};

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
        if (SX_gs.prefix_list[i] == 0)
           return(i);

    i = NPREFIX - 1;

    return(i);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* SX_RESET_PREFIX - reset all prefixes to 0 */

void SX_reset_prefix(void)
   {int i;

    for (i = 0; i < NPREFIX; i++)
        SX_gs.prefix_list[i] = 0;

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
    p  = NULL;

    if (_SX.gets == NULL)
       {if (PG_gs.console == NULL)
	   _SX.gets = io_gets;
        else
	   _SX.gets = (PFfgets) PG_wind_fgets;};

    fp = SS_INSTREAM(str);

    if (fp == NULL)
       rv = -1;

    else if (fp == stdin)
       {PFread r;
	PFfgets f;

        r = SC_leh_set_read(PG_wind_read);
        f = SC_leh_set_fgets(PG_wind_fgets);
	p = SC_prompt(_SS.pr_prompt, SS_BUFFER(str), MAXLINE);
        SC_leh_set_read(r);
        SC_leh_set_fgets(f);

	_SS.pr_prompt = NULL;

	if (p == NULL)
	   rv = 0;

/* the \r check is for the benefit of those non-UNIX systems who use it */
	else if ((*p == '\n') || (*p == '\r'))
	   {if ((SX_gs.autoplot == ON) && (SX_gs.plot_hook != NULL))
	       SX_gs.plot_hook(si);
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

/* SX_IMPORT_SO - dynamically generate bindings for functions in
 *              - the specified shared library
 */

int SX_import_so(SS_psides *si, char *hdr, char *so, char *flags)
   {int ok, st;
    char cmd[MAXLINE], pck[MAXLINE], fnc[MAXLINE];
    char gso[MAXLINE], path[MAXLINE];
    char *p;
    void (*f)(SS_psides *si);

    ok = FALSE;

/* prepare the import-so command to generate and compile
 * the bindings for the SO
 */
    if (flags == NULL)
       snprintf(cmd, MAXLINE, "import-so -i %s -l %s",
		hdr, so);
    else
       snprintf(cmd, MAXLINE, "import-so %s -i %s -l %s",
		flags, hdr, so);

    st = system(cmd);
    if (st == 0)

/* make the name of the function generated to load and install the bindings */
       {SC_strncpy(pck, MAXLINE, hdr, -1);
	p = strrchr(pck, '.');
	if (p != NULL)
	   *p = '\0';

	p = strrchr(pck, '/');
	if (p != NULL)
	   {p++;

	    SC_strncpy(path, MAXLINE, PACT_SO_CACHE, -1);
	    snprintf(gso, MAXLINE, "%s/pact-%s.so", path, p);
	    snprintf(fnc, MAXLINE, "SX_load_%s", p);

	    ok = SC_so_register_func(OBJ_FUNC, gso, fnc, path,
				     NULL, "void", "(SS_psides *si)");

/* get the function and invoke it */
	    ASSIGN_FNC(f, SC_so_get(OBJ_FUNC, fnc));
	    if (f != NULL)
	       {f(si);
		ok = TRUE;};};};

    return(ok);}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
